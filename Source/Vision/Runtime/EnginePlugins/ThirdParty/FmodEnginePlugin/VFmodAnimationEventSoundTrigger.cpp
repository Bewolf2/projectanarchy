/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/FmodEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

VFmodAnimationEventSoundTrigger::VFmodAnimationEventSoundTrigger(int iComponentFlags) : IVAnimationEventTrigger(iComponentFlags)
{
}

bool VFmodAnimationEventSoundTrigger::CommonInit()
{
  // Initialize base component
  if (!IVAnimationEventTrigger::CommonInit())
    return false;

  // Fill the event trigger info
  if (m_iEventTriggerInfoCount <= 0)
  {
    VFmodEventSoundTriggerInfo_t* info = NULL;
    if(m_pActiveTriggerInfo == NULL) //make sure it does not get created more than once
    {
      // Create new list with only one entry and set properties
      info = new VFmodEventSoundTriggerInfo_t();
    }
    else
    {
      info = (VFmodEventSoundTriggerInfo_t*)m_pActiveTriggerInfo;
    }
    // Get sound resource
    info->m_spSoundResource = VFmodManager::GlobalManager().LoadSoundResource(SoundFilename, VFMOD_RESOURCEFLAG_DEFAULT);
    if (info->m_spSoundResource == NULL || !GetEventTriggerInfoBaseData(info)) //set sequence and event id
    {
      V_SAFE_DELETE(info);
      m_pActiveTriggerInfo = NULL;
      return false;
    }
    // Set it as the active event trigger info
    m_pActiveTriggerInfo = info;
  }
  
  return true;
}

void VFmodAnimationEventSoundTrigger::OnAnimationEvent() 
{
  // Get the active event trigger info
  VFmodEventSoundTriggerInfo_t* info = (VFmodEventSoundTriggerInfo_t*) m_pActiveTriggerInfo;

  // Check if effect file is specified
  if (info == NULL || info->m_spSoundResource == NULL)
    return; 
  
  // Get entity position
  VisBaseEntity_cl* pEntity = (VisBaseEntity_cl *)m_pOwner;
  hkvVec3 vPos = pEntity->GetPosition();

  // Trigger sound effect  
  VFmodSoundObject* pSound = info->m_spSoundResource->CreateInstance(vPos, VFMOD_FLAG_NONE);
  if(pSound)
    pSound->Play();
}

// Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(VFmodAnimationEventSoundTrigger, IVAnimationEventTrigger, 0, &g_FmodModule);

void VFmodAnimationEventSoundTrigger::Serialize( VArchive &ar )
{
  char iLocalVersion = V_VFMODANIMATIONEVENTSOUNDTRIGGER_VERSION_CURRENT;
  IVAnimationEventTrigger::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion == V_VFMODANIMATIONEVENTSOUNDTRIGGER_VERSION_CURRENT, "Invalid local version. Please re-export");

    ar >> SoundFilename;

    CommonInit();
  } 
  else
  {
    ar << iLocalVersion;

    ar << SoundFilename;
  }
}

START_VAR_TABLE(VFmodAnimationEventSoundTrigger, IVAnimationEventTrigger, "Animation Event Sound Trigger Component.", VCOMPONENT_ALLOW_MULTIPLE, "Event Sound Trigger")
  DEFINE_VAR_VSTRING (VFmodAnimationEventSoundTrigger, SoundFilename, "Filename of the sound effect", "", 0, 0, "filepicker(.wav,.ogg,.mp3)");
END_VAR_TABLE

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
