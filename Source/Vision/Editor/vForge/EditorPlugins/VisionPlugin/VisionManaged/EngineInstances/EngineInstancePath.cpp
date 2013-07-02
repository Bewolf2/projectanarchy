/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstancePath.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;

// number of iterations for path sampling
#define NUM_ITERATIONS 5000

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif

namespace VisionManaged
{

  EngineInstancePath::EngineInstancePath()
  {
    bBBoxValid = false;
    m_pPath = new VisPath_cl();
    m_pPath->AddRef();
    m_iColor = V_RGBA_GREEN.GetRGBA();
    m_eLineDisplayMode = PathLineDisplayMode_e::Always;
    m_bPickRendering = false;
  }

  void EngineInstancePath::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pPath);
  }

  /*
  void EngineInstancePath::SetKey(String ^key)
  {
    VString sKey;
    ConversionUtils::StringToVString(key,sKey);
    m_pPath->SetKey(sKey);
  }
*/
  void EngineInstancePath::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
    hkvVec3 vIntersect;
   
    BoundingBox ^pBBox = ownerShape->AbsoluteBoundingBox;
    
    // extend bounding box of path by average picking radius, in order to allow picking at flat angles
    float fAvRadius = (m_pPath->GetLen()/NUM_ITERATIONS)*10.f;
    hkvAlignedBBox bbox (hkvVec3 (pBBox->vMin.X-fAvRadius, pBBox->vMin.Y-fAvRadius, pBBox->vMin.Z-fAvRadius),
                         hkvVec3 (pBBox->vMax.X+fAvRadius, pBBox->vMax.Y+fAvRadius, pBBox->vMax.Z+fAvRadius));
    
    if(!bbox.getLineSegmentIntersection(vStart,vEnd,NULL,&vIntersect))
      return;

    float fMinDist;
    if (GetPathDistFromRay(vStart,vEnd,fMinDist))
      pResult->SetShapeHit_IfCloser(ownerShape,fMinDist);
  }

  bool EngineInstancePath::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    // NOTE: This function is obsolete (and also not correct). The local bbox is computed by the shape
    System::Diagnostics::Debug::Assert((bbox)!=nullptr);
    if (!m_pPath)
      return false;
    if (!bBBoxValid)
    {
      hkvAlignedBBox bbox(hkvNoInitialization);
      m_pPath->ComputeBoundingBox(bbox); // absolute box
      if (!bbox.isValid()) // no nodes?
        return false;
      bbox.translate (-m_pPath->GetPosition()); // now local
      bbox.addBoundary (hkvVec3 (2)); // border
      bx1 = bbox.m_vMin.x;
      by1 = bbox.m_vMin.y;
      bz1 = bbox.m_vMin.z;
      bx2 = bbox.m_vMax.x;
      by2 = bbox.m_vMax.y;
      bz2 = bbox.m_vMax.z;
      bBBoxValid = true;
    }

    (*bbox).Set(bx1,by1,bz1,bx2,by2,bz2);
    return true;
  }

 
  void EngineInstancePath::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (Vision::Editor.IsAnimatingOrPlaying())
      return;
    VColorRef iColor;
    iColor.SetRGBA(m_iColor);

    if (m_bPickRendering)
    {
      m_pPath->DrawPath(iColor);
    }
    else
    {
      switch (m_eLineDisplayMode)
      {
      case PathLineDisplayMode_e::Always :
        m_pPath->DrawPath(iColor);
        break;
      case PathLineDisplayMode_e::WhenSelected : 
        if (mode==ShapeRenderMode::Selected)
          m_pPath->DrawPath(iColor);
        break;
      }
    }
  }

  void EngineInstancePath::AddPathNode(EngineInstancePathNode ^pNodeInst)
  {
    System::Diagnostics::Debug::Assert(m_pPath!=nullptr);
    System::Diagnostics::Debug::Assert(pNodeInst!=nullptr);
    System::Diagnostics::Debug::Assert(pNodeInst->GetEngineNode()!=nullptr);
    m_pPath->AddPathNode(pNodeInst->GetEngineNode());
    bBBoxValid = false;
  }

  float EngineInstancePath::GetPathTimeAtScreenPos(float fScreenX, float fScreenY, float fStart, float fEnd, int iRecursions)
  {
    if (m_pPath==nullptr)
      return -1.f;

    if (fStart<0.f) fStart = 0.f;
    if (fEnd>1.f) fEnd = 1.f;

    float fBestT = -1.f;
    float fBestDist = 100000000000.f;
    int iIterations = 200;
    if (fStart==0.f && fEnd==1.0f)
      iIterations = 5000;

    float fStep = (fEnd-fStart)/(float)iIterations;
    hkvVec3 vPos;

    for (int i=0;i<iIterations;i++)
    {
      float t = fStart + (float)i*fStep;
      m_pPath->EvalPointSmooth(t,vPos);
      float fX,fY;
      if (!Vision::Contexts.GetMainRenderContext()->Project2D(vPos,fX,fY))
        continue;
      fX-=fScreenX;
      fY-=fScreenY;
      float fDist = fX*fX+fY*fY;
      if (fDist>fBestDist)
        continue;

      fBestDist = fDist;
      fBestT = t;
    }

    // use recursive refinement:
    if (iRecursions>0 && fBestT>=0.f)
      fBestT = GetPathTimeAtScreenPos(fScreenX,fScreenY,fBestT-fStep,fBestT+fBestT,iRecursions-1);

    return fBestT;
  }

  bool EngineInstancePath::GetPathDistFromRay(const hkvVec3& vStart, const hkvVec3& vEnd, float &fMinDist)
  { 
    bool bResult = false;
    if (m_pPath==nullptr)
      return bResult;

    hkvVec3 vRay = vEnd-vStart;
    vRay.normalizeIfNotZero();

    float fBestDist = 100000000.0f;
    float fStep = 1.f/(float)NUM_ITERATIONS;

    hkvVec3 vPos;
    hkvVec3 vLastPos;
    m_pPath->EvalPointSmooth(0.0f,vLastPos);

    for (int i=1;i<NUM_ITERATIONS;i++)
    {
      float t = (float)i*fStep;
      m_pPath->EvalPointSmooth(t,vPos);

      hkvVec3 vStartToPos = vPos-vStart;
      float fProj = vStartToPos.dot (vRay);
      float fDist = hkvMath::sqrt(vStartToPos.getLengthSquared () - fProj*fProj);

      float fThreshold = (vPos-vLastPos).getLength()*10.f;
      vLastPos = vPos;

      if (fDist>fBestDist || fDist>fThreshold)
        continue;

      fBestDist = fDist;
      fMinDist = vStartToPos.getLength();
      bResult = true;
    }    

    return bResult;
  }

  bool EngineInstancePath::OnExport(SceneExportInfo ^info) 
  {
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pPath;
    return true;
  }


  //////////////////////////////////////////////////////////////////////////////////////////
  // Path node
  //////////////////////////////////////////////////////////////////////////////////////////

  EngineInstancePathNode::EngineInstancePathNode()
  {
    m_pPathNode = new VisPathNode_cl();
    m_pPathNode->AddRef();
    tx1 = ty1 = tz1 = 0.f;
    tx2 = ty2 = tz2 = 0.f;
    m_pPathNode->SetTypeOut(VisPathNode_cl::BEZIER);
    m_pPathNode->SetTypeIn(VisPathNode_cl::BEZIER);
    m_pNodeTex = Vision::TextureManager.Load2DTexture("textures\\pin_green32.dds",VTM_FLAG_DEFAULT_NON_MIPMAPPED);
    m_pNodeTex->AddRef();
  }

  void EngineInstancePathNode::DisposeObject()
  {
    V_SAFE_RELEASE(m_pPathNode);
    V_SAFE_RELEASE(m_pNodeTex);
  }


  void EngineInstancePathNode::SetObjectKey(String ^key)
  {
    if (m_pPathNode==nullptr)
      return;
    VString sKey;
    ConversionUtils::StringToVString(key,sKey);
    if (sKey.IsEmpty())
      m_pPathNode->SetObjectKey(NULL);
    else
      m_pPathNode->SetObjectKey(sKey);
  }


  void EngineInstancePathNode::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vDir(rayEnd.X-rayStart.X,rayEnd.Y-rayStart.Y,rayEnd.Z-rayStart.Z);
/*  
    hkvAlignedBBox bbox;
    if (!GetLocalBoundingBox(bbox))
      return;
    
    float fT;
    hkvVec3 vTouch;

    if (!bbox.Intersect(vStart,vDir, -1.f, &fT, &vTouch))
      return;

    fT *= vDir.GetLength();
    pResult->SetShapeHit_IfCloser(ownerShape,fT);
  */
  }

  bool EngineInstancePathNode::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    const float fSize = 15.f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
    return true;
  }

 
  void EngineInstancePathNode::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (Vision::Editor.IsAnimatingOrPlaying())
      return;
    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
  
    hkvVec3 vPos = m_pPathNode->GetPosition();

    if (mode==ShapeRenderMode::Selected)
    {
      const float fRad = 7.f;
      hkvAlignedBBox bbox(hkvVec3 (vPos.x-fRad,vPos.y-fRad,vPos.z-fRad), hkvVec3 (vPos.x+fRad,vPos.y+fRad,vPos.z+fRad));
      pRenderer->DrawLineBox(bbox,V_RGBA_YELLOW,2.f);
    }

    if (g_eDisplayMode==PathNodeDisplayMode_e::None)
      return;

    VSimpleRenderState_t state(VIS_TRANSP_ALPHA);
    Vector3F vCamPos = view->CameraPosition;
    hkvVec3 diff(vPos.x-vCamPos.X, vPos.y-vCamPos.Y,vPos.z-vCamPos.Z);
    float fSize = 0.05f*diff.getLength(); // unscaled
    if (g_eDisplayMode==PathNodeDisplayMode_e::Scaled)
      fSize = 16.f * EditorManager::Settings->GlobalUnitScaling;

    pRenderer->DrawSprite((hkvVec3& )vPos,m_pNodeTex,V_RGBA_WHITE,state,fSize,fSize);
  }

}

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
