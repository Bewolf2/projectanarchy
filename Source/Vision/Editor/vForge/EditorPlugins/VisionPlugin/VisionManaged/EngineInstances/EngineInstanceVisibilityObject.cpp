/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceVisibilityObject.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;


namespace VisionManaged
{

  EngineInstanceVisibilityObject::EngineInstanceVisibilityObject()
  {
    m_pVisObj = new VisVisibilityObjectAABox_cl(VISTESTFLAGS_FRUSTUMTEST|VISTESTFLAGS_ACTIVE);
    m_pVisObj->AddRef();
    m_fCenterX = m_fCenterY = m_fCenterZ = 0.f;
    m_fBoxDimX = m_fBoxDimY = m_fBoxDimZ = 10.f;
  }

  void EngineInstanceVisibilityObject::DisposeObject()
  {
    V_SAFE_DISPOSEANDRELEASE(m_pVisObj);
  }


  void EngineInstanceVisibilityObject::SetPosition(float x,float y,float z)
  {
    m_fCenterX = x;
    m_fCenterY = y;
    m_fCenterZ = z;
    UpdateBox();
  }

  void EngineInstanceVisibilityObject::SetOrientation(float yaw,float pitch,float roll)
  {
  }


  void EngineInstanceVisibilityObject::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (Vision::Editor.IsAnimatingOrPlaying())
      return;

    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
    VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
    
    hkvAlignedBBox bbox(
      hkvVec3 (m_fCenterX-m_fBoxDimX, m_fCenterY-m_fBoxDimY, m_fCenterZ-m_fBoxDimZ),
      hkvVec3 (m_fCenterX+m_fBoxDimX, m_fCenterY+m_fBoxDimY, m_fCenterZ+m_fBoxDimZ));
    pRenderer->RenderAABox(bbox,VColorRef(100,100,255,50),state);
  }

  void EngineInstanceVisibilityObject::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
  }



  bool EngineInstanceVisibilityObject::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    (*bbox).Set(-m_fBoxDimX,-m_fBoxDimY,-m_fBoxDimZ,m_fBoxDimX,m_fBoxDimY,m_fBoxDimZ);
    return true;
  }

  
  void EngineInstanceVisibilityObject::SetTestFlags(bool bFrustum, bool bPortal, bool bHOCQ, float fNearClip, float fFarClip, bool bActive)
  {
    int iFlags = 0;
    if (bFrustum) iFlags|=VISTESTFLAGS_FRUSTUMTEST;
    if (bPortal)  iFlags|=VISTESTFLAGS_PORTALTEST;
    if (bHOCQ)    iFlags|=VISTESTFLAGS_HARDWAREOCCLUSIONQUERY;
    if (bActive)  iFlags|=VISTESTFLAGS_ACTIVE;
    if (fFarClip>0.f || fNearClip>0.f) iFlags|=VISTESTFLAGS_FARCLIPTEST;
    m_pVisObj->SetNearClipDistance(fNearClip);
    m_pVisObj->SetFarClipDistance(fFarClip);
    m_pVisObj->SetVisTestFlags(iFlags);
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
