/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Boss.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/BossAiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

V_IMPLEMENT_SERIAL(RPG_Boss, RPG_AiCharacter, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_Boss, RPG_AiCharacter, "Boss", 0, "")  
END_VAR_TABLE

/// Constructor
RPG_Boss::RPG_Boss()
  : RPG_AiCharacter()
{
}

void RPG_Boss::InitializeCharacterEffects()
{
  // @todo: this is a bit of an off-topic place for this. Organize better once we data-drive this bit.
  m_projectileMeshFilename = "Effects/ProjectileStandIn.model";
  RPG_CharacterEffect_e effectType;

  //const VString& fmodEventGroup = "Characters/Boss";
  //VVERIFY_MSG(RPG_VisionEffectHelper::LoadFmodEventGroup(fmodEventGroup), fmodEventGroup.AsChar());   // pre-load this character's sounds

  effectType = FX_Ambient;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Boss_Idle.xml";
  }

  // Ranged Attack ////////////////////////////////////////////////////////////
  effectType = FX_RangedAttackFire;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Boss_Ranged_Attack_Fire_mobile.xml";
    //m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    //m_characterEffectDefinitions[effectType].m_fmodEventName = "Ranged_Attack_Fire";
  }

  effectType = FX_RangedAttackProjectile;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Boss_Ranged_Attack_Proj_mobile.xml";
  }

  effectType = FX_RangedAttackImpact;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Boss_Ranged_Attack_Impact_mobile.xml";
  }

  // AoE Attack ///////////////////////////////////////////////////////////////
  effectType = FX_AoeAttackFire;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Boss_Aoe_Attack_mobile.xml";
    m_characterEffectDefinitions[effectType].m_fmodEventGroupName = "Characters/Guardian/Caster";
    m_characterEffectDefinitions[effectType].m_fmodEventName = "Aoe_Attack_Cast";
  }

  effectType = FX_ReceiveHeal;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Any_Character_Receive_Heal.xml";
    //m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    //m_characterEffectDefinitions[effectType].m_fmodEventName = "Receive_Heal";
    m_characterEffectDefinitions[effectType].m_shaderFlashDuration = 1.f;
    m_characterEffectDefinitions[effectType].m_shaderFlashColor = VColorRef(0, 208, 24);
  }

  effectType = FX_TakeHit;
  {
    m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Boss_TakeHit.xml";
    //m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    //m_characterEffectDefinitions[effectType].m_fmodEventName = "Take_Hit";
    m_characterEffectDefinitions[effectType].m_shaderFlashDuration = 1.f;
    m_characterEffectDefinitions[effectType].m_shaderFlashColor = VColorRef(150, 200, 255);
  }

  effectType = FX_Die;
  {
    //m_characterEffectDefinitions[effectType].m_vfxFilename = "Particles/CharacterEffects/Boss_Die.xml";
    //m_characterEffectDefinitions[effectType].m_fmodEventGroupName = fmodEventGroup;
    //m_characterEffectDefinitions[effectType].m_fmodEventName = "Die";
  }
}

/// Returns type type of AiControllerComponent to create
VType* RPG_Boss::GetControllerComponentType()
{
  return V_RUNTIME_CLASS(RPG_BossAiControllerComponent);
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
