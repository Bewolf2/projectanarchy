/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file KeyControlledTransitionCharacter.hpp

#ifndef KEYCONTROLLED_TRANSITIONBARBARIAN_HPP_INCLUDED
#define KEYCONTROLLED_TRANSITIONBARBARIAN_HPP_INCLUDED

#include <Vision/Samples/Engine/ModelMerging/TransitionBarbarian.hpp>

#ifndef _VISION_DOC

enum CHARACTER_CONTROL 
{
  CHARACTER_MOVE_FORWARD = VISION_INPUT_CONTROL_LAST_ELEMENT + 1,
  CHARACTER_ROTATE_LEFT,
  CHARACTER_ROTATE_RIGHT,
  CHARACTER_RUN,
  CHARACTER_CONTROL_LAST_ELEMENT = CHARACTER_RUN
};

#define CHARACTER_CONTROL_ALTERNATIVES 4
#define CHARACTER_CONTROL_USER_SPACE 8

// Extends the transition state machine based animated character class with code 
// for converting the keyboard input to character movements.
class KeyControlledTransitionBarbarian_cl : public TransitionBarbarian_cl
{
public:
  KeyControlledTransitionBarbarian_cl();
  ~KeyControlledTransitionBarbarian_cl();

  VOVERRIDE void InitFunction();
  VOVERRIDE void ThinkFunction();
  void SetInputStatus(bool bStatus);
  bool GetInputStatus();

protected:

  void ProcessKeyboardEvents();
  bool m_bInputAllowed;

public:
  V_DECLARE_SERIAL( KeyControlledTransitionBarbarian_cl, )
  IMPLEMENT_OBJ_CLASS( KeyControlledTransitionBarbarian_cl );
};

#endif // _VISION_DOC

#endif // KEYCONTROLLED_TRANSITIONBARBARIAN_HPP_INCLUDED

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
