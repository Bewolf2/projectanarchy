/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file KeyControlledAnimatedCharacter.hpp

#ifndef KEYCONTROLLED_ANIMATEDCHARACTER_HPP_INCLUDED
#define KEYCONTROLLED_ANIMATEDCHARACTER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/AnimatedCharacter.hpp>

#ifndef _VISION_DOC

enum CHARACTER_CONTROL {
  CHARACTER_MOVE_FORWARD = 0,
  CHARACTER_MOVE_BACKWARD,
  CHARACTER_MOVE_LEFT,
  CHARACTER_MOVE_RIGHT,

  CHARACTER_RUN,
  CHARACTER_ATTACK,
  CHARACTER_TOGGLE_WEAPON,

  CHARACTER_CONTROL_LAST_ELEMENT = CHARACTER_TOGGLE_WEAPON
};

#define CHARACTER_CONTROL_ALTERNATIVES 4
#define CHARACTER_CONTROL_USER_SPACE 8

// Extends the state pattern based animated character class with code for converting the keyboard
// input to character movements
class KeyControlledAnimatedCharacter_cl : public AnimatedCharacter_cl
{
public:
  KeyControlledAnimatedCharacter_cl();
  ~KeyControlledAnimatedCharacter_cl();

  VOVERRIDE void InitFunction();
  VOVERRIDE void ThinkFunction();
  void SetInputStatus(bool bStatus);
  bool GetInputStatus();

protected:
  VInputMap* m_pInputMap;
  void ProcessKeyboardEvents();
  bool m_bKeyboardInputAllowed;

public:
  IMPLEMENT_OBJ_CLASS(KeyControlledAnimatedCharacter_cl);
};

#endif // _VISION_DOC

#endif  // KEYCONTROLLED_ANIMATEDCHARACTER_HPP_INCLUDED

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
