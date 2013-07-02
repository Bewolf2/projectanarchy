/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceHeightfieldGenerator.hpp>
//#include "VisionView.h"
//#include "ConversionUtils.hpp"
#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif

namespace VisionManaged
{
  ///////////////////////////////////////////////////////////////////////////////////////////////
  // Height field generator shape
  ///////////////////////////////////////////////////////////////////////////////////////////////

  EngineInstanceHeightfieldGenerator::EngineInstanceHeightfieldGenerator()
  {
    m_fYaw = m_fPitch = m_fRoll = 0.f;
    m_fPosX = m_fPosY = m_fPosZ = 0.f;
    m_fSizeX = m_fSizeY = m_fSizeZ = 0.f;
    m_pObject3D = new VisObject3D_cl();
    m_pBBox = new hkvAlignedBBox(hkvVec3 (-50,-50,-30), hkvVec3 (50,50,30));
    m_pCollMesh = nullptr;
  }

  void EngineInstanceHeightfieldGenerator::DisposeObject()
  {
    V_SAFE_DELETE(m_pCollMesh);
  }

  void *EngineInstanceHeightfieldGenerator::GetObject3D()
  {
    return m_pObject3D;
  }

  void EngineInstanceHeightfieldGenerator::SetPosition(float x, float y, float z)
  {
    m_fPosX = x;
    m_fPosY = y;
    m_fPosZ = z;
    IEngineInstanceObject3D::SetPosition(x, y, z);
  }

  void EngineInstanceHeightfieldGenerator::SetOrientation(float yaw, float pitch, float roll)
  {
    m_fYaw = yaw;
    m_fPitch = pitch;
    m_fRoll = roll;
    IEngineInstanceObject3D::SetOrientation(yaw, pitch, roll);
  }

  
  void EngineInstanceHeightfieldGenerator::SetScaling(float x, float y, float z)
  {
    if (!m_pBBox) return;
   // m_pBBox->SetScaling((x+y+z)/3.f);
  }


  void EngineInstanceHeightfieldGenerator::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (mode==ShapeRenderMode::Selected)
    {
      IVRenderInterface *pRI = (static_cast<VisionView ^>(view))->GetRenderInterface();
      
      // render the box
      hkvVec3 vPos(m_fPosX, m_fPosY, m_fPosZ);

      hkvAlignedBBox bbox;
      GetLocalBoundingBox(bbox);
      bbox.translate (vPos);
      VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_DOUBLESIDED);
      pRI->RenderAABox(bbox, VColorRef(255,255,0,50),state);
      
      hkvPlane plane (hkvNoInitialization); plane.setFromPointAndNormal (vPos,hkvVec3(0.f,0.f,1.f));
      pRI->RenderPlane(plane,vPos, hkvMath::Min(m_fSizeX,m_fSizeY)*0.5f, VColorRef(0,100,0,100), state, RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID, 8);
    }
  }


  void EngineInstanceHeightfieldGenerator::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
  
    hkvAlignedBBox bbox;
    if (!GetLocalBoundingBox(bbox))
      return;
    
    hkvVec3 vTouch;

    if (!m_pCollMesh)
    {
      m_pCollMesh = new VSimpleCollisionMesh;
      m_pCollMesh->AllocateVertices(4);
      m_pCollMesh->AllocateIndices(6);

      unsigned short* pInd = m_pCollMesh->GetIndexPtr();
      pInd[0] = 0;
      pInd[1] = 1;
      pInd[2] = 2;
      pInd[3] = 0;
      pInd[4] = 2;
      pInd[5] = 3;
    }

    // update vertex positions
    m_pCollMesh->GetVertex(0)->set(bbox.m_vMin.x,bbox.m_vMin.y,m_fPosZ);
    m_pCollMesh->GetVertex(1)->set(bbox.m_vMax.x,bbox.m_vMin.y,m_fPosZ);
    m_pCollMesh->GetVertex(2)->set(bbox.m_vMax.x,bbox.m_vMax.y,m_fPosZ);
    m_pCollMesh->GetVertex(3)->set(bbox.m_vMin.x,bbox.m_vMax.y,m_fPosZ);
    VTraceHitInfo hitInfo;
    if (!m_pCollMesh->GetTraceIntersection(vStart, vEnd, TRACEFLAG_DOUBLESIDED|TRACEFLAG_NOBOUNDINGBOX, &hitInfo))
      return;
    pResult->SetShapeHit_IfCloser(ownerShape,hitInfo.m_fDistance);
  }
    
  bool EngineInstanceHeightfieldGenerator::GetLocalBoundingBox(hkvAlignedBBox &bbox)
  {
    if (m_pBBox->isValid())
    {
      bbox = *m_pBBox;
    }
    else
    {
      float fSize = 30.f;
      bbox.set(hkvVec3 (-fSize), hkvVec3 (fSize));
    }
    return true;
  }

  bool EngineInstanceHeightfieldGenerator::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    if (!m_pBBox)
      return false;
    (*bbox).Set(m_pBBox->m_vMin.x, m_pBBox->m_vMin.y, m_pBBox->m_vMin.z, m_pBBox->m_vMax.x, m_pBBox->m_vMax.y, m_pBBox->m_vMax.z);
    return true;
  }

  void EngineInstanceHeightfieldGenerator::SetBoxSize(float x, float y, float z)
  {
    m_fSizeX = x;
    m_fSizeY = y;
    m_fSizeZ = z;
    hkvVec3 vRadius(x*0.5f, y*0.5f, z*0.5f);
    m_pBBox->m_vMin = -vRadius;
    m_pBBox->m_vMax = vRadius;
  }

  bool EngineInstanceHeightfieldGenerator::OnExport(SceneExportInfo ^info)
  {
    // nothing to export
    return true;
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
