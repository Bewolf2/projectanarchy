/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

#include <Vision/Samples/Engine/RPGPlugin/AiSpawnPoint.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Boss.h>
#include <Vision/Samples/Engine/RPGPlugin/BossAiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/CasterGuardian.h>
#include <Vision/Samples/Engine/RPGPlugin/CasterGuardianAiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/DestructibleEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/FloatingEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/HealthPickup.h>
#include <Vision/Samples/Engine/RPGPlugin/LevelInfo.h>
#include <Vision/Samples/Engine/RPGPlugin/ManaPickup.h>
#include <Vision/Samples/Engine/RPGPlugin/MeleeGuardian.h>
#include <Vision/Samples/Engine/RPGPlugin/MeleeGuardianAiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/ProjectileHealing.h>
#include <Vision/Samples/Engine/RPGPlugin/RangedGuardian.h>
#include <Vision/Samples/Engine/RPGPlugin/RangedGuardianAiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Trigger.h>
#include <Vision/Samples/Engine/RPGPlugin/GUIHud_VisionGUI.h>

void RPG_VisionModuleTypes::ForceStaticLink()
{
  //FORCE_LINKDYNCLASS(RPG_AiControllerComponent);
  FORCE_LINKDYNCLASS(RPG_AiSpawnPoint);
  FORCE_LINKDYNCLASS(RPG_AttackableComponent);
  FORCE_LINKDYNCLASS(RPG_Boss);
  FORCE_LINKDYNCLASS(RPG_BossAiControllerComponent);
  FORCE_LINKDYNCLASS(RPG_CasterGuardian);
  FORCE_LINKDYNCLASS(RPG_CasterGuardianAiControllerComponent);
  FORCE_LINKDYNCLASS(RPG_Character);
  FORCE_LINKDYNCLASS(RPG_DestructibleEntity);
  //FORCE_LINKDYNCLASS(RPG_DummyEntity);
  FORCE_LINKDYNCLASS(RPG_FloatingEntity);
  FORCE_LINKDYNCLASS(RPG_HealthPickup);
  FORCE_LINKDYNCLASS(RPG_LevelInfo);
  FORCE_LINKDYNCLASS(RPG_ManaPickup);
  FORCE_LINKDYNCLASS(RPG_MeleeGuardian);
  FORCE_LINKDYNCLASS(RPG_MeleeGuardianAiControllerComponent);
  //FORCE_LINKDYNCLASS(RPG_HighlightableComponent);
  //FORCE_LINKDYNCLASS(RPG_InteractableComponent);
  //FORCE_LINKDYNCLASS(RPG_Pickup);
  FORCE_LINKDYNCLASS(RPG_PlayerControllerComponent);
  FORCE_LINKDYNCLASS(RPG_PlayerCharacter);
  FORCE_LINKDYNCLASS(RPG_PlayerSpawnPoint);
  FORCE_LINKDYNCLASS(RPG_Projectile_Healing);
  FORCE_LINKDYNCLASS(RPG_RangedGuardian);
  FORCE_LINKDYNCLASS(RPG_RangedGuardianAiControllerComponent);
  //FORCE_LINKDYNCLASS(RPG_SpawnPoint);
  FORCE_LINKDYNCLASS(RPG_Trigger);
  FORCE_LINKDYNCLASS(RPG_GUIHud);

  Vision::Error.SystemMessage("RPG_VisionModuleTypes::ForceStaticLink begin type dump");
  for(VType *moduleType = g_RPGPluginModule.GetTypeList(); moduleType; moduleType = moduleType->m_pNextType)
  {
    Vision::Error.SystemMessage("%s", moduleType->m_lpszClassName);
  }
  Vision::Error.SystemMessage("RPG_VisionModuleTypes::ForceStaticLink end type dump");
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
