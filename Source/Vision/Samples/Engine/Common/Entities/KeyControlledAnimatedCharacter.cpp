/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/KeyControlledAnimatedCharacter.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/AnimatedCharacter.hpp>

#define THUMBSTICK_DEADZONE 15000

// ***********************************************************************************************
// KeyControlledAnimatedCharacter_cl
// ***********************************************************************************************
// Extends the state pattern based animated character class with code for converting the keyboard
// input to character movements
// ***********************************************************************************************

KeyControlledAnimatedCharacter_cl::KeyControlledAnimatedCharacter_cl()
{
}

KeyControlledAnimatedCharacter_cl::~KeyControlledAnimatedCharacter_cl()
{
  V_SAFE_DELETE(m_pInputMap);
}

void KeyControlledAnimatedCharacter_cl::SetInputStatus(bool bStatus)
{
  m_bKeyboardInputAllowed = bStatus;
}

bool KeyControlledAnimatedCharacter_cl::GetInputStatus()
{
  return m_bKeyboardInputAllowed;
}


void KeyControlledAnimatedCharacter_cl::InitFunction()
{
  m_pInputMap = new VInputMap(CHARACTER_CONTROL_LAST_ELEMENT+1+CHARACTER_CONTROL_USER_SPACE, CHARACTER_CONTROL_ALTERNATIVES);

#ifdef SUPPORTS_KEYBOARD
  // setup necessary key settings
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_KEYBOARD, CT_KB_UP);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_KEYBOARD, CT_KB_KP_8);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_KEYBOARD, CT_KB_DOWN);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_KEYBOARD, CT_KB_KP_2);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_KEYBOARD, CT_KB_LEFT);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_KEYBOARD, CT_KB_KP_4);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_KEYBOARD, CT_KB_RIGHT);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_KEYBOARD, CT_KB_KP_6);

#ifdef WASD_CONTROL
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_KEYBOARD, CT_KB_W);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_KEYBOARD, CT_KB_S);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_KEYBOARD, CT_KB_A);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_KEYBOARD, CT_KB_D);

#endif

  m_pInputMap->MapTrigger(CHARACTER_TOGGLE_WEAPON, V_KEYBOARD, CT_KB_SPACE,    VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_ATTACK,        V_KEYBOARD, CT_KB_ENTER,    VInputOptions::Once());
  m_pInputMap->MapTrigger(CHARACTER_ATTACK,        V_KEYBOARD, CT_KB_KP_ENTER, VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_RUN,           V_KEYBOARD, CT_KB_LSHIFT);
  m_pInputMap->MapTrigger(CHARACTER_RUN,           V_KEYBOARD, CT_KB_RSHIFT);
 
#endif


#ifdef _VISION_XENON

  // setup necessary key settings
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  m_pInputMap->MapTrigger(CHARACTER_TOGGLE_WEAPON, VInputManagerXenon::GetPad(0), CT_PAD_LEFT_SHOULDER,          VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_ATTACK,        VInputManagerXenon::GetPad(0), CT_PAD_B,                      VInputOptions::OncePerFrame( ONCE_ON_PRESS, 0));
  m_pInputMap->MapTrigger(CHARACTER_RUN,           VInputManagerXenon::GetPad(0), CT_PAD_RIGHT_SHOULDER,         VInputOptions::DeadZone(0.4f, false, 0));

#endif
  



#ifdef _VISION_PS3

  // setup necessary key settings
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  m_pInputMap->MapTrigger(CHARACTER_TOGGLE_WEAPON, VInputManagerPS3::GetPad(0), CT_PAD_LEFT_SHOULDER,          VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_ATTACK,        VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_TRIGGER,          VInputOptions::Once());
  m_pInputMap->MapTrigger(CHARACTER_RUN,           VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_SHOULDER,         VInputOptions::DeadZone(0.4f));

#endif


  m_bKeyboardInputAllowed = true;

  AnimatedCharacter_cl::InitFunction();
}


void KeyControlledAnimatedCharacter_cl::ThinkFunction()
{
  if (m_bKeyboardInputAllowed)
    ProcessKeyboardEvents();
  
  AnimatedCharacter_cl::ThinkFunction();
}


void KeyControlledAnimatedCharacter_cl::ProcessKeyboardEvents()
{

  bool bLeft = m_pInputMap->GetTrigger(CHARACTER_MOVE_LEFT)!=0;
  bool bRight = m_pInputMap->GetTrigger(CHARACTER_MOVE_RIGHT)!=0;
  bool bUp = m_pInputMap->GetTrigger(CHARACTER_MOVE_FORWARD)!=0;
  bool bDown = m_pInputMap->GetTrigger(CHARACTER_MOVE_BACKWARD)!=0;
  bool bShift = m_pInputMap->GetTrigger(CHARACTER_RUN)!=0;
  bool bRaiseLowerWeapon = m_pInputMap->GetTrigger(CHARACTER_TOGGLE_WEAPON)!=0;
  bool bAttack = m_pInputMap->GetTrigger(CHARACTER_ATTACK)!=0;
  
  if (bUp)
  {
    // trigger the run/walk actions when CURSOR UP is pressed.
    // allow rotating the entity while walking/running
    if ( bShift )
      Run();
    else
      Walk();

    if (bLeft)
      RotateLeft();
    else if (bRight)
      RotateRight();
  }
  else if(bDown)
  {
    // trigger the walk backward action when CURSOR DOWN is pressed.
    // allow rotating the entity while walking backwards
    WalkBackwards();

    if(bLeft)
      RotateLeft();
    else if(bRight)
      RotateRight();
  }
  else
  {
    // switch to stand action if character is not rotating ???????????
    if ( !IsInRotateState() && !IsInRotateArmedState() )
      Stand();

    // trigger rotate actions if CURSOR LEFT/RIGHT is pressed, otherwise trigger stand action
    if(bLeft)
      RotateLeft();
    else if(bRight)
      RotateRight();
    else
      Stand();
  }


  // raise or lower weapon on SPACE key
  if(bRaiseLowerWeapon)
  {
    if(IsInUpperBodyIdleState()||IsInUpperBodyLowerWeaponState())
      Arm();
    else if(IsInUpperBodyIdleArmedState()||IsInUpperBodyDrawWeaponState())
      DisArm();
  }

  // attack on ENTER key
  if ( bAttack )
    GetUpperBodyState()->Attack();
}

START_VAR_TABLE(KeyControlledAnimatedCharacter_cl, AnimatedCharacter_cl, "An animated warrior character that can be controlled with the keyboard", VFORGE_HIDECLASS, "Models\\ElfWarrior.model")
END_VAR_TABLE

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
