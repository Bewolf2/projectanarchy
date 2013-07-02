/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/AiCharacter.h>

#include <Vision/Samples/Engine/RPGPlugin/ControllerComponent.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

V_IMPLEMENT_SERIAL(RPG_AiCharacter, RPG_Character, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_AiCharacter, RPG_Character, "Player Character Entity", 0, "")  
  DEFINE_VAR_FLOAT_AND_NAME(RPG_AiCharacter, m_wanderRadius, "Wander Radius", "The distance this character can wander from its origin", "1000", 0, 0);
END_VAR_TABLE

  /// Constructor
RPG_AiCharacter::RPG_AiCharacter()
  : RPG_Character()
  , m_wanderRadius(1000.0f)
{
  m_defaultTeam = TEAM_Red;
}

float RPG_AiCharacter::GetWanderRadius() const
{
  return m_wanderRadius;
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
