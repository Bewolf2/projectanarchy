/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_VISION_GAME_DEBUG_H__
#define RPG_PLUGIN_VISION_GAME_DEBUG_H__

#include <Vision/Samples/Engine/RPGPlugin/Character.h>

class RPG_VisionGameDebug
{
public:
  static void DisplayCharacterHealth(RPG_Character const& character);

  static void DisplayEvent(RPG_Character const& character, RPG_CharacterEvent const& event, UBYTE alpha = 255, float verticalOffsetAdjust = 0.0f);

  static void UpdateEventDisplay(RPG_Character const& character);

  static void DebugDisplayCharacterStats(RPG_Character const& character);

  static void DebugDisplayMovementInfo(RPG_Character const& character);

  static void DebugDisplayNavmeshStatus(RPG_Character const& character);

public:
  static float const kDamageDisplayDuration;
};

#endif

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
