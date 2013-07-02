/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/MeleeGuardian.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/MeleeGuardianAiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

V_IMPLEMENT_SERIAL(RPG_MeleeGuardian, RPG_AiCharacter, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_MeleeGuardian, RPG_AiCharacter, "Guardian Entity", 0, "")
END_VAR_TABLE

/// Constructor
RPG_MeleeGuardian::RPG_MeleeGuardian()
: RPG_AiCharacter()
{
}

void RPG_MeleeGuardian::InitializeCharacterEffects()
{
  const VString& fmodEventGroup = "Characters/Guardian/Melee";
  RPG_CharacterEffect_e effectType;

  // Basics ///////////////////////////////////////////////////////////////////
  effectType = FX_Spawn;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Melee_Guardian_Spawn_mobile.xml";
    m_characterEffectDefinitions[effectType].m_vfxBoneName = "origin_jnt";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Spawn";
    m_characterEffectDefinitions[effectType].m_attachEffect = false;
  }

  effectType = FX_Challenge;
  {
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Challenge";
  }

  effectType = FX_Footstep;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Melee_Guardian_Footstep_mobile.xml";
    //m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    //m_characterEffectDefinitions[effectType].m_fmodEventName = "Footstep_Run_Stone";
  }

  // Basic Melee Attack ///////////////////////////////////////////////////////
  effectType = FX_MeleeBasicAttack;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Melee_Guardian_Basic_Melee_Swing.xml";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Melee_Basic_Swing";
    m_characterEffectDefinitions[effectType].m_vfxBoneName = "right_attachment_jnt";
  }

  // Damage & Dying ///////////////////////////////////////////////////////////
  effectType = FX_ReceiveHeal;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Any_Guardian_Receive_Heal_mobile.xml";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Receive_Heal";
    m_characterEffectDefinitions[effectType].m_shaderFlashDuration = 1.f;
    m_characterEffectDefinitions[effectType].m_shaderFlashColor = VColorRef(49, 208, 255);
  }

  effectType = FX_TakeHit;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Melee_Guardian_TakeHit_mobile.xml";
    m_characterEffectDefinitions[effectType].m_vfxBoneName = "Spine_jnt";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Take_Hit";
    //m_characterEffectDefinitions[effectType].m_wallmarkTextureFilename = "Particles/Textures/Effects_Splash_01_D.tga";
    //m_characterEffectDefinitions[effectType].m_wallmarkLifetime = 2.f;
    //m_characterEffectDefinitions[effectType].m_wallmarkFadeoutTime = 1.f;
    m_characterEffectDefinitions[effectType].m_shaderFlashDuration = 1.f;
    m_characterEffectDefinitions[effectType].m_shaderFlashColor = VColorRef(150, 200, 255);
  }

  effectType = FX_Die;
  {
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Die";
  }

  effectType = FX_DeathDissolve;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Any_Character_Death_Dissolve.xml";
  }
}

/// Returns type type of AiControllerComponent to create
VType* RPG_MeleeGuardian::GetControllerComponentType()
{
  return V_RUNTIME_CLASS(RPG_MeleeGuardianAiControllerComponent);
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
