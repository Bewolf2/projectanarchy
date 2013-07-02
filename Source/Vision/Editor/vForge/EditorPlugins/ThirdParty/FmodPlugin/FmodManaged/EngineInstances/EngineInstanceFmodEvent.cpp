/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/FmodManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/EngineInstances/EngineInstanceFmodEvent.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiTriggerComponent.hpp>

#using <mscorlib.dll>

namespace FmodManaged
{

  EngineInstanceFmodEvent::EngineInstanceFmodEvent()
  {
    m_bIsVisible = true;
    m_pEvent = NULL;
    m_eEventFlags = (FmodCreationFlags_e)0;
  }

  void EngineInstanceFmodEvent::DisposeObject()
  {
    DisposeNativeInstance();
  }

  void EngineInstanceFmodEvent::DisposeNativeInstance()
  {
    if (m_pEvent)
    {
      m_pEvent->DisposeObject();
      m_pEvent->Release();
      m_pEvent = NULL;
    }
  }

  void EngineInstanceFmodEvent::SetPosition(float x, float y, float z)
  {
    m_vPosition.X = x;
    m_vPosition.Y = y;
    m_vPosition.Z = z;
    if (m_pEvent)
      m_pEvent->SetPosition(x,y,z);
  }

  void EngineInstanceFmodEvent::SetOrientation(float yaw, float pitch, float roll)
  {
    if (m_pEvent)
      m_pEvent->SetOrientation(yaw, pitch, roll);
  }

  bool EngineInstanceFmodEvent::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    // create a small bounding box for picking
    float fSize = 5.0f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize, -fSize, -fSize, fSize, fSize, fSize);
    return true;
  }

  void EngineInstanceFmodEvent::TraceShape(Shape3D ^ownerShape, Vector3F rayStart, Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    // don't do anything because the shape code already does the picking
  }

  bool EngineInstanceFmodEvent::OnExport(SceneExportInfo ^info)
  {
    if (m_pEvent==NULL || m_pEvent->GetEventGroup()==NULL)
      return true;

    // do the export: Get the export binary archive and serialize into it.
    // Requires the native event class to support serialization (derive from VisTypedEngineObject_cl and 
    // implement Serialize function)
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar.WriteObject(m_pEvent);
    return true;
  }   

  void EngineInstanceFmodEvent::RenderShape(VisionViewBase ^view, CSharpFramework::Shapes::ShapeRenderMode mode)
  {
    IVRenderInterface *pRI = (static_cast<VisionView ^>(view))->GetRenderInterface(); 
    VSimpleRenderState_t iState(VIS_TRANSP_ALPHA);
    hkvVec3 vPos(m_vPosition.X, m_vPosition.Y, m_vPosition.Z);
    Vector3F vCamPos = view->CameraPosition;
    hkvVec3 diff(vPos.x-vCamPos.X, vPos.y-vCamPos.Y, vPos.z-vCamPos.Z);
    float fSize = 0.03f*diff.getLength();

    if (m_pEvent)
      pRI->DrawSprite((hkvVec3 &)vPos, "textures\\EventSpeaker.tga",VColorRef(255,255,255,220), iState, fSize, fSize);
    else
      pRI->DrawSprite((hkvVec3 &)vPos, "textures\\EventSpeaker_grey.tga",VColorRef(255,255,255,96), iState, fSize, fSize);
  }

  void EngineInstanceFmodEvent::InitEvent(String ^eventProjectPath, String ^eventGroupName, String ^eventName, FmodCreationFlags_e flags)
  {
    m_eEventFlags = flags;
    VString sEventProjectPath, sEventGroupName, sEventName;
    ConversionUtils::StringToVString(eventProjectPath, sEventProjectPath);
    ConversionUtils::StringToVString(eventGroupName, sEventGroupName);
    ConversionUtils::StringToVString(eventName, sEventName);

    // create a new event instance
    DisposeNativeInstance();

    int iInstanceFlags = (int)flags & (int)VFMOD_FLAG_BITMASK;

    if (!sEventProjectPath.IsEmpty() && !sEventGroupName.IsEmpty() && !sEventName.IsEmpty())
    {
      VFmodEventGroup *pEventGroup = VFmodManager::GlobalManager().LoadEventGroup(sEventProjectPath, sEventGroupName);
      if (pEventGroup)
        m_pEvent = pEventGroup->CreateEvent(sEventName, hkvVec3(0.0f,0.0f,0.0f), iInstanceFlags);
    }

    if (!m_pEvent)
      return;

    m_pEvent->AddRef();
    m_pEvent->SetMuted(!m_bIsVisible);

    // add trigger target components - only inside vForge. They are only serialized when used.
    VisTriggerTargetComponent_cl *pComp;

    // pause target
    pComp = new VisTriggerTargetComponent_cl(VFMOD_TRIGGER_PAUSE, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pEvent->AddComponent(pComp);

    // unpause target
    pComp = new VisTriggerTargetComponent_cl(VFMOD_TRIGGER_RESUME, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pEvent->AddComponent(pComp);
  }

  bool EngineInstanceFmodEvent::IsValid()
  {
    return (m_pEvent && m_pEvent->IsValid() && !m_pEvent->IsInfoOnly());
  }

  void EngineInstanceFmodEvent::Start(bool bAlsoInEditingMode) 
  {
    if (IsValid() && m_bIsVisible)
      m_pEvent->Start(bAlsoInEditingMode);
  }

  void EngineInstanceFmodEvent::Stop() 
  {
    if (IsValid()) 
      m_pEvent->Stop();
  }

  void EngineInstanceFmodEvent::Update() 
  {
    // Update info-only events, in order to offer these events the possibility to convert into a playable event instance.
    if (m_pEvent && m_pEvent->IsValid() && m_pEvent->IsInfoOnly())
      m_pEvent->Update(true);
  }

  bool EngineInstanceFmodEvent::IsPlaying() 
  {
    if (m_pEvent) 
      return m_pEvent->IsPlaying(); 
    return false;
  }

  void EngineInstanceFmodEvent::AddFlag(FmodCreationFlags_e flagToAdd)    
  {
    if (m_pEvent)
    {
      m_pEvent->Helper_SetFlag((int)flagToAdd, true);
      m_eEventFlags = (FmodCreationFlags_e) ((int)m_eEventFlags | (int)flagToAdd);
    }
  }

  void EngineInstanceFmodEvent::RemoveFlag(FmodCreationFlags_e flagToRem) 
  {
    if (m_pEvent)
    {
      m_pEvent->Helper_SetFlag((int)flagToRem, false);
      m_eEventFlags = (FmodCreationFlags_e) ((int)m_eEventFlags & (int)~flagToRem);
    }
  }

  void EngineInstanceFmodEvent::SetVisible(bool bStatus)
   { 
     m_bIsVisible = bStatus; 
     if (m_pEvent != NULL)
     {
       m_pEvent->SetMuted(!m_bIsVisible);
       if (bStatus && !m_pEvent->IsPlaying() && !m_pEvent->IsPaused() && Vision::Editor.IsAnimatingOrPlaying())
         Start(false);
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
