/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/FmodManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/ThirdParty/FmodPlugin/FmodManaged/EngineInstances/EngineInstanceFmodSound.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiTriggerComponent.hpp>

#using <mscorlib.dll>

namespace FmodManaged
{

  EngineInstanceFmodSound::EngineInstanceFmodSound()
  {
    m_bIsVisible = true;
    m_pSound = NULL;
    m_eSoundFlags = (FmodCreationFlags_e)0;
  }

  void EngineInstanceFmodSound::DisposeObject()
  {
    DisposeNativeInstance();
  }

  void EngineInstanceFmodSound::DisposeNativeInstance()
  {
    if (m_pSound != NULL)
    {
      m_pSound->DisposeObject();
      m_pSound->Release();
      m_pSound = NULL;
    }
  }

  void EngineInstanceFmodSound::SetPosition(float x, float y, float z)
  {
    m_vPosition.X = x;
    m_vPosition.Y = y;
    m_vPosition.Z = z;
    if (m_pSound != NULL)
      m_pSound->SetPosition(x,y,z);
  }

  void EngineInstanceFmodSound::SetOrientation(float yaw, float pitch, float roll)
  {
    if (m_pSound != NULL)
      m_pSound->SetOrientation(yaw, pitch, roll);
  }

  bool EngineInstanceFmodSound::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    // create a small bounding box for picking
    float fSize = 5.0f * EditorManager::Settings->GlobalUnitScaling;
    (*bbox).Set(-fSize, -fSize, -fSize, fSize, fSize, fSize);
    return true;
  }

  void EngineInstanceFmodSound::TraceShape(Shape3D ^ownerShape, Vector3F rayStart, Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    // don't do anything because the shape code already does the picking
  }

  bool EngineInstanceFmodSound::OnExport(SceneExportInfo ^info)
  {
    if (m_pSound == NULL || m_pSound->GetResource() == NULL)
      return true;

    // do the export: Get the export binary archive and serialize into it.
    // Requires the native sound class to support serialization (derive from VisTypedEngineObject_cl and 
    // implement Serialize function)
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar.WriteObject(m_pSound);
    return true;
  }   

  void EngineInstanceFmodSound::RenderShape(VisionViewBase ^view, CSharpFramework::Shapes::ShapeRenderMode mode)
  {
    IVRenderInterface *pRI = (static_cast<VisionView ^>(view))->GetRenderInterface();
    VSimpleRenderState_t iState(VIS_TRANSP_ALPHA);
    hkvVec3 vPos(m_vPosition.X, m_vPosition.Y, m_vPosition.Z);
    Vector3F vCamPos = view->CameraPosition;
    hkvVec3 diff(vPos.x-vCamPos.X, vPos.y-vCamPos.Y, vPos.z-vCamPos.Z);
    float fSize = 0.03f*diff.getLength();

    // If no valid sound is currently assigned, draw a 'ghost' speaker (grey, less opaque)
    if (m_pSound == NULL)
    {
      pRI->DrawSprite(vPos, "textures\\Speaker_grey.tga",VColorRef(255,255,255,96), iState, fSize, fSize);
      return;
    }

    // do not render attenuation for 2D objects
    if (!((int)m_eSoundFlags & (int)VFMOD_RESOURCEFLAG_3D))
    {
      pRI->DrawSprite(vPos, "textures\\Speaker.tga",VColorRef(255,255,255,220), iState, fSize, fSize);
      return;
    }

    switch (mode)
    {
    case ShapeRenderMode::Normal: 
      pRI->DrawSprite(vPos, "textures\\Speaker.tga", VColorRef(255,255,255,220), iState, fSize, fSize);
      break;

    case ShapeRenderMode::Selected: 
      m_pSound->DebugRender(pRI);  
      break;
    }
  }

  void EngineInstanceFmodSound::InitSound(String ^filename, FmodCreationFlags_e flags, int iPrior)
  {
    m_eSoundFlags = flags;
    VString sFilename;
    ConversionUtils::StringToVString(filename, sFilename);

    // create a new sound instance
    DisposeNativeInstance();

    int iInstanceFlags = (int)flags & (int)VFMOD_FLAG_BITMASK;
    int iResourceUsageFlags = (int)flags & (int)VFMOD_RESOURCEFLAG_BITMASK;

    if (!sFilename.IsEmpty())
      m_pSound = VFmodManager::GlobalManager().CreateSoundInstance(sFilename, iResourceUsageFlags, iInstanceFlags, iPrior);

    if (m_pSound == NULL)
      return;

    m_pSound->AddRef();
    m_pSound->SetMuted(!m_bIsVisible);

    // add trigger target components - only inside vForge. They are only serialized when used.
    VisTriggerTargetComponent_cl *pComp;

    // pause target
    pComp = new VisTriggerTargetComponent_cl(VFMOD_TRIGGER_PAUSE, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pSound->AddComponent(pComp);

    // unpause target
    pComp = new VisTriggerTargetComponent_cl(VFMOD_TRIGGER_RESUME, VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
    m_pSound->AddComponent(pComp);
  }

  void EngineInstanceFmodSound::SetVolume(float fVol) 
  { 
    if (m_pSound != NULL) 
    {
      m_pSound->SetVolume(fVol); 
      // Re-set the muted flag again, as the volume change removed the mute flag of
      // the Fmod sound object on the engine side.
      m_pSound->SetMuted(!m_bIsVisible);
    }
  }

  void EngineInstanceFmodSound::SetPan(float fPan)
  {
    if (m_pSound != NULL && m_pSound->GetResource() != NULL && !m_pSound->GetResource()->Is3D()) 
      m_pSound->SetPan(fPan);
  }

  void EngineInstanceFmodSound::Set3DFadeDistance(float fMin, float fMax) 
  {
    if (m_pSound != NULL) 
      m_pSound->Set3DFadeDistance(fMin,fMax);
  
  }

  void EngineInstanceFmodSound::SetPitch(float fPitch)
  {
    if (m_pSound != NULL) 
      m_pSound->SetPitch(fPitch);
  }

  void EngineInstanceFmodSound::SetConeAngles(bool bDirectional, float fInside, float fOutside) 
  {
    if (m_pSound != NULL) 
      m_pSound->SetConeAngles(bDirectional ? fInside:-1.0f, bDirectional ? fOutside:-1.0f);
  }

  void EngineInstanceFmodSound::Play(bool bAlsoInEditingMode) 
  {
    if (m_pSound != NULL && m_bIsVisible)
      m_pSound->Play(0.0f, bAlsoInEditingMode);
  }

  void EngineInstanceFmodSound::Stop() 
  {
    if (m_pSound != NULL) 
      m_pSound->Stop();
  }

  bool EngineInstanceFmodSound::IsPlaying() 
  {
    if (m_pSound != NULL) 
      return m_pSound->IsPlaying(); 
    return false;
  }

  void EngineInstanceFmodSound::AddFlag(FmodCreationFlags_e flagToAdd)    
  {
    if (m_pSound != NULL)
    {
      m_pSound->Helper_SetFlag((int)flagToAdd, true);
      m_eSoundFlags = (FmodCreationFlags_e) ((int)m_eSoundFlags | (int)flagToAdd);
    }
  }

  void EngineInstanceFmodSound::RemoveFlag(FmodCreationFlags_e flagToRem) 
  {
    if (m_pSound != NULL)
    {
      m_pSound->Helper_SetFlag((int)flagToRem, false);
      m_eSoundFlags = (FmodCreationFlags_e) ((int)m_eSoundFlags & (int)~flagToRem);
    }
  }

  void EngineInstanceFmodSound::SetVisible(bool bStatus)
  {
     m_bIsVisible = bStatus;
     if (m_pSound != NULL)
     {
       m_pSound->SetMuted(!m_bIsVisible);
       if (bStatus && !m_pSound->IsPlaying() && !m_pSound->IsPaused() && Vision::Editor.IsAnimatingOrPlaying())
         Play(false);
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
