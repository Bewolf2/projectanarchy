/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

/// loads / caches a texture
bool RPG_VisionEffectHelper::LoadTexture(char const* fileName)
{
  return (GetEffectResource(VIS_RESOURCEMANAGER_TEXTURES, fileName) != NULL);
}

/// loads / caches a particle effect
bool RPG_VisionEffectHelper::LoadParticleEffect(char const* fileName)
{
  return (GetEffectResource("Particles", fileName) != NULL);
}

/// loads / caches a sound effect
bool RPG_VisionEffectHelper::LoadSoundEffect(char const* fileName)
{
  return (GetEffectResource("FmodSoundResource", fileName) != NULL);
}

VProjectedWallmark *RPG_VisionEffectHelper::CreateProjectedWallmark(RPG_VisionEffectWallmarkParams const& params)
{
  VTextureObject *textureResource = static_cast<VTextureObject *> (GetEffectResource(VIS_RESOURCEMANAGER_TEXTURES, params.m_textureFilename));

  // Note: In case of VTextureObject a plainwhite texture is used as replacement so the 
  // resource is still valid but marked as missing. 
  if (textureResource)
  {
    return VWallmarkManager::GlobalManager().CreateProjectedWallmark(params.m_position, params.m_origin, params.m_radius, params.m_depth, textureResource, VIS_TRANSP_ALPHA, V_RGBA_WHITE, params.m_angle, params.m_lifetime, params.m_fadeoutTime);
  }

  return NULL;
}

VisParticleEffect_cl *RPG_VisionEffectHelper::CreateParticleEffect(RPG_VisionEffectParticleParams const& params)
{
  return CreateParticleEffect(params.m_effectFilename, params.m_position, params.m_orientation);
}

VisParticleEffect_cl* RPG_VisionEffectHelper::CreateParticleEffect(VString const& particleFilename, hkvVec3 const& position /*= hkvVec3(0.f, 0.f, 0.f)*/, hkvVec3 const& orientation /*= hkvVec3(0.f, 0.f, 0.f)*/)
{
  VASSERT(!particleFilename.IsEmpty());

  VisParticleEffectFile_cl *effectResource = static_cast<VisParticleEffectFile_cl *> (GetEffectResource("Particles", particleFilename.AsChar()));
  if (effectResource)
  {
    VisParticleEffect_cl* effect = effectResource->CreateParticleEffectInstance(position, orientation);

    if(effect)
    {
      for (int index = 0; index < effect->GetParticleGroupCount(); ++index)
      {
        effect->GetParticleGroup(index)->SetUpdateLifetimeIfInvisible(true);
      }
    }

    return effect;
  }

  return NULL;
}

VFmodSoundObject *RPG_VisionEffectHelper::CreateSoundEffect(RPG_VisionEffectSoundParams const& params)
{
  VASSERT(VFmodManager::GlobalManager().IsInitialized()); // ensure that the native Fmod system has been correctly initialized

  VFmodSoundResource *soundResource = static_cast<VFmodSoundResource *> (GetEffectResource("FmodSoundResource", params.m_soundFilename));
  VASSERT_MSG(soundResource, params.m_soundFilename);
  if (soundResource)
  {
    VFmodSoundObject *soundObject = soundResource->CreateInstance(params.m_position, VFMOD_FLAG_NONE);
    VASSERT_MSG(soundObject->IsValid(), params.m_soundFilename);
    if (soundObject->IsValid())
    {
      soundObject->Play();
    }
  }

  return NULL;
}

VManagedResource *RPG_VisionEffectHelper::GetEffectResource(const char* resourceType, const char* szFilename)
{
  VResourceManager *resourceManager = Vision::ResourceSystem.GetResourceManagerByName(resourceType);
  VASSERT_MSG (resourceManager, "RPG: No resource manager found.");
  if (!resourceManager)
    return NULL;

  VManagedResource *resource = resourceManager->GetResourceByName(szFilename);
  if (!resource)
  {    
    resource = resourceManager->LoadResource(szFilename);
    if (!resource)
    {
      Vision::Error.Warning("[RPG] Resource is missing: %s", szFilename);      
      return NULL;
    }
    
    Vision::Error.Warning("[RPG] Resource is not cached: %s", szFilename);   
  }

  return resource; 
}

bool RPG_VisionEffectHelper::PlaySoundFile(VString const& sfxFilename, hkvVec3 const& position /*= hkvVec3(0.f, 0.f, 0.f)*/)
{
  VASSERT(!sfxFilename.IsEmpty());
  RPG_VisionEffectSoundParams soundParams;
  {
    soundParams.m_soundFilename = sfxFilename.AsChar();
    soundParams.m_position = position;
    if (RPG_VisionEffectHelper::CreateSoundEffect(soundParams))
    {
      return true;
    }
  }
  return false;
}

VFmodEvent* RPG_VisionEffectHelper::PlayFmodSoundEvent(VString const& eventGroupName, VString const& eventName, hkvVec3 const& position /*= hkvVec3(0.0f,0.0f,0.0f)*/, int flags/*=VFMOD_FLAG_NONE*/)
{
  VASSERT(!eventGroupName.IsEmpty());   // Name of the event group, relative to the event project
  VASSERT(!eventName.IsEmpty());        // name of the event, relative to the parent event-group

  VString const& eventProjectPath = RPG_GameManager::s_instance.GetFmodEventProject();
  VASSERT_MSG(!eventProjectPath.IsEmpty(), "Please ensure that GameManager::FMOD_EVENT_PROJECT points to a valid FMOD project file.");

#ifdef _DEBUG
  // @for testing only: validate the project filename
  FMOD::EventProject* pEventProject = VFmodManager::GlobalManager().LoadEventProject(eventProjectPath.AsChar());
  VASSERT_MSG(pEventProject, eventProjectPath.AsChar());
#endif

  // load event group
  VFmodEventGroup *pEventGroup = VFmodManager::GlobalManager().LoadEventGroup(eventProjectPath.AsChar(), eventGroupName.AsChar());
  VASSERT_MSG(pEventGroup && pEventGroup->IsValid(), eventGroupName.AsChar());

  if (pEventGroup && pEventGroup->IsValid())
  {
    // create event instance
    VFmodEvent *pEvent = pEventGroup->CreateEvent(eventName.AsChar(), position, flags);
    VASSERT_MSG(pEvent && pEvent->IsValid(), eventName.AsChar());
    return pEvent;
  }
  return NULL;
}

bool RPG_VisionEffectHelper::LoadFmodEventGroup(VString const& eventGroupName)
{
  VString const& eventProjectPath = RPG_GameManager::s_instance.GetFmodEventProject();
  VASSERT_MSG(!eventProjectPath.IsEmpty(), "Please ensure that GameManager::FMOD_EVENT_PROJECT points to a valid FMOD project file.");

  VFmodEventGroup *pEventGroup = VFmodManager::GlobalManager().LoadEventGroup(eventProjectPath.AsChar(), eventGroupName.AsChar());
  VASSERT_MSG(pEventGroup && pEventGroup->IsValid(), eventGroupName.AsChar());

  return pEventGroup->IsValid();
}

bool RPG_VisionEffectHelper::LoadMesh(char const* fileName)
{
  VResourceManager *resourceManager = Vision::ResourceSystem.GetResourceManagerByName(VIS_RESOURCEMANAGER_MESHES);
  VASSERT_MSG (resourceManager, "RPG: No resource manager found.");
  if (!resourceManager)
    return NULL;

  VManagedResource *resource = resourceManager->GetResourceByName(fileName);
  if (!resource)
  {    
    VMeshManager *meshManager = static_cast<VMeshManager*>(resourceManager);
    resource = meshManager->LoadDynamicMeshFile(fileName);
    if (!resource)
    {
      Vision::Error.Warning("[RPG] Resource is missing: %s", fileName);      
      return NULL;
    }
    
    Vision::Error.Warning("[RPG] Resource is not cached: %s", fileName);   
  }

  return resource; 
}

bool RPG_VisionEffectHelper::IsPersistentEffect(const VisParticleEffect_cl* effect)
{
  VASSERT(effect);

  // do any of this system's layers have an infinite lifetime?
  if (hkMath::fabs(effect->GetRemainingLifeTime() - -1.0f) < HK_REAL_EPSILON)
  {
    // if so, we need to check the emitter count for each particle layer
    for (int i = 0; i < effect->GetParticleGroupCount(); ++i)
    {
      if (!effect->GetParticleGroup(i)->GetEmitter()->m_FixParticleCount.IsDefined())
      {
        // If true the emitter spawns a fixed number of particles (=> not looped), false means the emitter spawns infinite particles 
        // (=> persistent if GetRemainingLifeTime() == -1).
        return true;
      }
    }
  }
  return false;
}

bool RPG_VisionEffectHelper::IsLoopedFmodEvent(const VFmodEvent* event)
{
  VASSERT(event);
  FMOD_EVENT_INFO info;
  event->GetInfo(NULL, NULL, &info);
  return info.lengthms == -1; // Length in milliseconds of this event. Note: lengthms will be -1 if the length of the event can't be determined i.e. if it has looping sounds.
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
