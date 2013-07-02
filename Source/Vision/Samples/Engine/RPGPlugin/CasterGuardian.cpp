/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/CasterGuardian.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/CasterGuardianAiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

V_IMPLEMENT_SERIAL(RPG_CasterGuardian, RPG_AiCharacter, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_CasterGuardian, RPG_AiCharacter, "Caster Guardian", 0, "")  
  DEFINE_VAR_VSTRING_AND_NAME(RPG_CasterGuardian, m_rangedAttackEffectFileName, "Ranged Attack Effect", "The effect played for ranged attacks", "", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_CasterGuardian, m_healingEffectFileName, "Healing Effect", "The effect played for healing", "", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_FLOAT_AND_NAME(RPG_CasterGuardian, m_healingThresholdPercentage, "Healing Threshold Percentage", "The health percentage at which the caster will heal a unit", "70", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_CasterGuardian, m_canHealSelf, "Can Heal Self", "Whether or not the caster can heal itself", "TRUE", 0, 0);
END_VAR_TABLE

/// Constructor
RPG_CasterGuardian::RPG_CasterGuardian()
  : RPG_AiCharacter()
  , m_rangedAttackEffectFileName()
  , m_healingEffectFileName()
  , m_healingThresholdPercentage(70.0f)
  , m_canHealSelf(TRUE)
{
}

void RPG_CasterGuardian::InitializeCharacterEffects()
{
  const VString& fmodEventGroup = "Characters/Guardian/Caster";
  RPG_CharacterEffect_e effectType;

  // Basics ///////////////////////////////////////////////////////////////////
  effectType = FX_Spawn;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Spawn_mobile.xml";
    m_characterEffectDefinitions[effectType].m_vfxBoneName = "GUARDIAN_ROOT";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Spawn";
    m_characterEffectDefinitions[effectType].m_attachEffect = false;
  }

  effectType = FX_Ambient;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Hover_mobile.xml";
  }

  effectType = FX_Challenge;
  {
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Challenge";
  }

  // Ranged Attack ////////////////////////////////////////////////////////////
  effectType = FX_RangedAttackChargeLoop;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Ranged_Attack_Charge_Loop_mobile.xml";
    m_characterEffectDefinitions[effectType].m_vfxBoneName = "left_attachment_jnt";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Ranged_Attack_Charge";
  }

  effectType = FX_RangedAttackFire;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Ranged_Attack_Fire_mobile.xml";
    m_characterEffectDefinitions[effectType].m_vfxBoneName = "left_attachment_jnt";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Ranged_Attack_Fire";
  }

  effectType = FX_RangedAttackProjectile;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Ranged_Attack_Proj_mobile.xml";
  }

  effectType = FX_RangedAttackImpact;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Ranged_Attack_Impact_mobile.xml";
  }

  // AoE Attack ///////////////////////////////////////////////////////////////
  effectType = FX_AoeAttackBegin;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Aoe_Attack_Begin_mobile.xml";
    m_characterEffectDefinitions[effectType].m_vfxBoneName = "left_attachment_jnt";
  }

  effectType = FX_AoeAttackChargeLoop;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Aoe_Attack_Charge_Loop.xml";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Aoe_Attack_Charge";
  }

  effectType = FX_AoeAttackFire;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Aoe_Attack_mobile.xml";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Aoe_Attack_Cast";
  }

  // Cast Heal ////////////////////////////////////////////////////////////////
  effectType = FX_CastHealChargeLoop;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Cast_Heal_Loop_mobile.xml";
    m_characterEffectDefinitions[effectType].m_vfxBoneName = "left_attachment_jnt";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Heal_Charge";
  }

  effectType = FX_CastHealOther;
  {
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Cast_Heal";
  }

  effectType = FX_CastHealSelf;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Cast_Heal_Self_mobile.xml";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Cast_Heal";
  }

  effectType = FX_HealProjectile;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Cast_Heal_Proj.xml";
  }

  effectType = FX_HealProjectileImpact;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_Cast_Heal_Proj_Impact_mobile.xml";
  }

  effectType = FX_ReceiveHeal;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Any_Guardian_Receive_Heal_mobile.xml";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Receive_Heal";
    m_characterEffectDefinitions[effectType].m_shaderFlashDuration = 1.f;
    m_characterEffectDefinitions[effectType].m_shaderFlashColor = VColorRef(49, 208, 255);
  }

  // Damage & Dying ///////////////////////////////////////////////////////////
  effectType = FX_TakeHit;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Caster_Guardian_TakeHit_mobile.xml";
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
VType* RPG_CasterGuardian::GetControllerComponentType()
{
  return V_RUNTIME_CLASS(RPG_CasterGuardianAiControllerComponent);
}

void RPG_CasterGuardian::PostInitialize()
{
  RPG_AiCharacter::PostInitialize();

  // HACK - setting the range here so we can test this character until we have proper weapon definitions.
  RPG_InventoryItem* equippedWeapon = GetInventoryHandler().GetEquippedWeapon();

  if(equippedWeapon)
  {
    equippedWeapon->SetRange(550.0f);
  }
}

const VString& RPG_CasterGuardian::GetRangedAttackEffectName()
{
  return m_rangedAttackEffectFileName;
}

const VString& RPG_CasterGuardian::GetHealingEffectName()
{
  return m_healingEffectFileName;
}

float RPG_CasterGuardian::GetHealingThresholdPercentage() const
{
  return m_healingThresholdPercentage;
}

BOOL RPG_CasterGuardian::CanHealSelf() const
{
  return m_canHealSelf;
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
