/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file KeyControlledAnimatedWarrior.hpp

#ifndef KEYCONTROLLED_ANIMATEDCHARACTER_HPP_INCLUDED
#define KEYCONTROLLED_ANIMATEDCHARACTER_HPP_INCLUDED

#include <Vision/Samples/Engine/Common/Entities/AnimatedWarrior.hpp>

class VVirtualThumbStick;

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
class KeyControlledAnimatedWarrior_cl : public AnimatedWarrior_cl
{
public:
  KeyControlledAnimatedWarrior_cl();
  ~KeyControlledAnimatedWarrior_cl();

  VOVERRIDE void InitFunction();
  VOVERRIDE void ThinkFunction();
  VISION_APIFUNC void SetInputStatus(bool bStatus);
  VISION_APIFUNC bool GetInputStatus();
  
  inline VInputMap* GetInputMap() { return m_pInputMap; }

protected:
  VInputMap* m_pInputMap;
  void ProcessKeyboardEvents();
  bool m_bKeyboardInputAllowed;
  
#if defined(SUPPORTS_MULTITOUCH)
  VVirtualThumbStick* m_pVirtualThumbStick;
#endif

public:
  V_DECLARE_SERIAL( KeyControlledAnimatedWarrior_cl, )
  IMPLEMENT_OBJ_CLASS(KeyControlledAnimatedWarrior_cl);
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
