/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/FmodManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/EngineInstances/EngineInstanceFmodReverb.hpp>

#using <mscorlib.dll>

namespace FmodManaged
{

  EngineInstanceFmodReverb::EngineInstanceFmodReverb()
  {
    m_bIsVisible = true;
    m_pReverb = new VFmodReverb();
  }

  void EngineInstanceFmodReverb::DisposeObject()
  {
    if (m_pReverb)
      m_pReverb->DisposeObject(); 
    m_pReverb = NULL;
  }

  void EngineInstanceFmodReverb::SetVisible(bool bStatus)
  {
    if (bStatus == m_bIsVisible)
      return;

    m_bIsVisible = bStatus;
    if (m_pReverb)
      m_pReverb->SetActive(m_bIsVisible);
  }
  
  void EngineInstanceFmodReverb::SetPosition(float x, float y, float z)
  {
    if (m_pReverb)
      m_pReverb->SetPosition(x,y,z);
  }

  void EngineInstanceFmodReverb::SetOrientation(float yaw, float pitch, float roll)  
  {
    if (m_pReverb)
      m_pReverb->SetOrientation(yaw, pitch, roll);
  }

  bool EngineInstanceFmodReverb::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    // create a small bounding box for picking
    float fSize = 5.0f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize, -fSize, -fSize, fSize, fSize, fSize);
    return true;
  }

  void EngineInstanceFmodReverb::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    // don't do anything because the shape code already does the picking
  }

  bool EngineInstanceFmodReverb::OnExport(SceneExportInfo ^info) 
  {
    if (!m_pReverb)
      return true;

    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar.WriteObject(m_pReverb);
    return true;
  }

  void EngineInstanceFmodReverb::RenderShape(VisionViewBase ^view, CSharpFramework::Shapes::ShapeRenderMode mode)
  {
    if (!m_pReverb)
      return;

    IVRenderInterface *pRI = (static_cast<VisionView ^>(view))->GetRenderInterface();
    hkvVec3 vPos = m_pReverb->GetPosition();
    VSimpleRenderState_t iState(VIS_TRANSP_ALPHA);

    Vector3F vCamPos = view->CameraPosition;
    hkvVec3 diff(vPos.x-vCamPos.X, vPos.y-vCamPos.Y, vPos.z-vCamPos.Z);
    float fSize = 0.03f*diff.getLength();

    switch (mode)
    {
    case ShapeRenderMode::Normal: 
      pRI->DrawSprite((hkvVec3 &)vPos, "textures\\Reverb.tga", VColorRef(255,255,255,220), iState, fSize, fSize);
      break;

    case ShapeRenderMode::Selected: 
      m_pReverb->DebugRender(pRI);  
      break;
    }
  }

  void EngineInstanceFmodReverb::InitReverb(FmodReverbProps ^reverbProperties, float fReverbMinDistance, float fReverbMaxDistance)
  {
    VFmodReverbProps properties = reverbProperties->CreateNativeProps();
    m_pReverb->Init(properties, fReverbMinDistance, fReverbMaxDistance);
    m_pReverb->SetActive(m_bIsVisible);
    if (VFmodManager::GlobalManager().GetFmodEventSystem())
      VFmodManager::GlobalManager().GetFmodEventSystem()->update();
  }

  void EngineInstanceFmodReverb::SetReverbDistances(float fReverbMinDistance, float fReverbMaxDistance)
  {
    if (m_pReverb) 
      m_pReverb->SetReverbDistances(fReverbMinDistance, fReverbMaxDistance);
  }

  void EngineInstanceFmodReverb::SetReverbProperties(FmodReverbProps ^reverbProperties)
  {
    if (m_pReverb)
    {
      VFmodReverbProps properties = reverbProperties->CreateNativeProps();
      m_pReverb->SetReverbProperties(properties);
    }
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
