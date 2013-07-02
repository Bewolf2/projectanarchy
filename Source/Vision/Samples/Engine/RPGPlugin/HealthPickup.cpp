/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/HealthPickup.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>

V_IMPLEMENT_SERIAL(RPG_HealthPickup, RPG_Pickup, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_HealthPickup, RPG_Pickup, "A Health pickup item. Gives Health to a character when used.", 0, "");
DEFINE_VAR_INT(RPG_HealthPickup, m_healthAmount, "The amount of health to give", "30", 0, 0);
END_VAR_TABLE



RPG_HealthPickup::RPG_HealthPickup() 
  : RPG_Pickup()
  , m_healthAmount(30)
{
}

void RPG_HealthPickup::OnPickup(RPG_Character* character)
{
  // add health to character
  character->AddHealth(m_healthAmount);
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
