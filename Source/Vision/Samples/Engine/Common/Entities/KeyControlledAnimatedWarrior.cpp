/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/KeyControlledAnimatedWarrior.hpp>
#include <Vision/Samples/Engine/Common/Entities/AnimatedWarrior.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>

#if defined(_VISION_IOS)
  #include <Vision/Runtime/Base/Graphics/Video/IOSBridge.h>
#endif

#define THUMBSTICK_DEADZONE 15000

// ***********************************************************************************************
// KeyControlledAnimatedWarrior_cl
// ***********************************************************************************************
// Extends the state pattern based animated character class with code for converting the keyboard
// input to character movements
// ***********************************************************************************************

KeyControlledAnimatedWarrior_cl::KeyControlledAnimatedWarrior_cl()
  : m_pInputMap(NULL)
  , m_bKeyboardInputAllowed(false)
#if defined(SUPPORTS_MULTITOUCH)
  , m_pVirtualThumbStick(NULL)
#endif
{
}

KeyControlledAnimatedWarrior_cl::~KeyControlledAnimatedWarrior_cl()
{
  V_SAFE_DELETE(m_pInputMap);
#if defined(SUPPORTS_MULTITOUCH)
  V_SAFE_DELETE(m_pVirtualThumbStick);
#endif
}

void KeyControlledAnimatedWarrior_cl::SetInputStatus(bool bStatus)
{
  m_bKeyboardInputAllowed = bStatus;
}

bool KeyControlledAnimatedWarrior_cl::GetInputStatus()
{
  return m_bKeyboardInputAllowed;
}


void KeyControlledAnimatedWarrior_cl::InitFunction()
{
  m_pInputMap = new VInputMap(CHARACTER_CONTROL_LAST_ELEMENT+1+CHARACTER_CONTROL_USER_SPACE, CHARACTER_CONTROL_ALTERNATIVES);

    // setup necessary key settings
#if defined(_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
#if defined(USE_DIGITAL_PAD_CHARACTER_CONTROL)
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_XENON_PAD(0), CT_PAD_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_XENON_PAD(0), CT_PAD_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_XENON_PAD(0), CT_PAD_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_XENON_PAD(0), CT_PAD_RIGHT, VInputOptions::DeadZone(0.4f));
#else
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));
#endif

  m_pInputMap->MapTrigger(CHARACTER_TOGGLE_WEAPON, V_XENON_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_ATTACK, V_XENON_PAD(0), CT_PAD_B, VInputOptions::Once());
  m_pInputMap->MapTrigger(CHARACTER_RUN,    V_XENON_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));

#elif defined(_VISION_PS3)
#if defined(USE_DIGITAL_PAD_CHARACTER_CONTROL)
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_PS3_PAD(0), CT_PAD_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_PS3_PAD(0), CT_PAD_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_PS3_PAD(0), CT_PAD_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_PS3_PAD(0), CT_PAD_RIGHT, VInputOptions::DeadZone(0.4f));
#else
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));
#endif

  m_pInputMap->MapTrigger(CHARACTER_TOGGLE_WEAPON, V_PS3_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_ATTACK, V_PS3_PAD(0), CT_PAD_SQUARE, VInputOptions::Once());
  m_pInputMap->MapTrigger(CHARACTER_RUN,    V_PS3_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  
#elif defined(_VISION_PSP2)
#if defined(USE_DIGITAL_PAD_CHARACTER_CONTROL)
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_PSP2_PAD(0), CT_PAD_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_PSP2_PAD(0), CT_PAD_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_PSP2_PAD(0), CT_PAD_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_PSP2_PAD(0), CT_PAD_RIGHT, VInputOptions::DeadZone(0.4f));
#else
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));
#endif

  m_pInputMap->MapTrigger(CHARACTER_TOGGLE_WEAPON, V_PSP2_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_ATTACK, V_PSP2_PAD(0), CT_PAD_SQUARE, VInputOptions::Once());
  m_pInputMap->MapTrigger(CHARACTER_RUN,    V_PSP2_PAD(0), CT_PAD_RIGHT_SHOULDER, 0.4f);

#elif defined(_VISION_MOBILE)

  #if defined(SUPPORTS_MULTITOUCH) 
    m_pVirtualThumbStick = new VVirtualThumbStick();

    m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_UP,     VInputOptions::DeadZone(0.2f));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_DOWN,   VInputOptions::DeadZone(0.2f));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_LEFT,   VInputOptions::DeadZone(0.2f));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_RIGHT,  VInputOptions::DeadZone(0.2f));

  #endif

#elif defined(_VISION_WIIU)
#if defined(USE_DIGITAL_PAD_CHARACTER_CONTROL)
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT, VInputOptions::DeadZone(0.4f));
#else
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));
#endif

  m_pInputMap->MapTrigger(CHARACTER_TOGGLE_WEAPON, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_ATTACK, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_Y, VInputOptions::Once());
  m_pInputMap->MapTrigger(CHARACTER_RUN,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));

#endif


#ifdef SUPPORTS_KEYBOARD

  // setup necessary key settings
#ifndef WASD_CONTROL_ONLY
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_KEYBOARD, CT_KB_UP);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_KEYBOARD, CT_KB_KP_8);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_KEYBOARD, CT_KB_DOWN);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_KEYBOARD, CT_KB_KP_2);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_KEYBOARD, CT_KB_LEFT);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_KEYBOARD, CT_KB_KP_4);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_KEYBOARD, CT_KB_RIGHT);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_KEYBOARD, CT_KB_KP_6);
#endif

#if defined(WASD_CONTROL) || defined(WASD_CONTROL_ONLY)
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_KEYBOARD, CT_KB_W);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_KEYBOARD, CT_KB_S);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_KEYBOARD, CT_KB_A);
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_KEYBOARD, CT_KB_D);
#endif

  m_pInputMap->MapTrigger(CHARACTER_TOGGLE_WEAPON, V_KEYBOARD, CT_KB_SPACE,    VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_ATTACK,        V_KEYBOARD, CT_KB_ENTER,    VInputOptions::Once());
  m_pInputMap->MapTrigger(CHARACTER_ATTACK,        V_KEYBOARD, CT_KB_KP_ENTER, VInputOptions::Once());

  m_pInputMap->MapTrigger(CHARACTER_RUN, V_KEYBOARD, CT_KB_LSHIFT);
  m_pInputMap->MapTrigger(CHARACTER_RUN, V_KEYBOARD, CT_KB_RSHIFT);

#endif

  m_bKeyboardInputAllowed = true;

  AnimatedWarrior_cl::InitFunction();
}


void KeyControlledAnimatedWarrior_cl::ThinkFunction()
{
  if (m_bKeyboardInputAllowed)
    ProcessKeyboardEvents();
  
  AnimatedWarrior_cl::ThinkFunction();
}


void KeyControlledAnimatedWarrior_cl::ProcessKeyboardEvents()
{
  float fLeft = m_pInputMap->GetTrigger(CHARACTER_MOVE_LEFT);
  float fRight = m_pInputMap->GetTrigger(CHARACTER_MOVE_RIGHT);

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

    if (!hkvMath::isFloatEqual(fLeft, 0.0f))
      RotateLeft(fLeft);
    else if (!hkvMath::isFloatEqual(fRight, 0.0f))
      RotateRight(fRight);
  }
  else if(bDown)
  {
    // trigger the walk backward action when CURSOR DOWN is pressed.
    // allow rotating the entity while walking backwards
    WalkBackwards();

    if (!hkvMath::isFloatEqual(fLeft, 0.0f))
      RotateLeft(fLeft);
    else if (!hkvMath::isFloatEqual(fRight, 0.0f))
      RotateRight(fRight);
  }
  else
  {
    // switch to stand action if character is not rotating ???????????
    if ( !IsInRotateState() && !IsInRotateArmedState() )
      Stand();

    // trigger rotate actions if CURSOR LEFT/RIGHT is pressed, otherwise trigger stand action
    if (!hkvMath::isFloatEqual(fLeft, 0.0f))
      RotateLeft(fLeft);
    else if (!hkvMath::isFloatEqual(fRight, 0.0f))
      RotateRight(fRight);
    else
      Stand();
  }


  // raise or lower weapon on SPACE key
  if(bRaiseLowerWeapon)
  {
    if (IsInUpperBodyIdleState() || IsInUpperBodyHolsterTorchState())
      Arm();
    else if (IsInUpperBodyIdleArmedState() || IsInUpperBodyDrawTorchState())
      DisArm();
  }

  // attack on ENTER key
  if ( bAttack )
    GetUpperBodyState()->Attack();
}

V_IMPLEMENT_SERIAL( KeyControlledAnimatedWarrior_cl, AnimatedWarrior_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(KeyControlledAnimatedWarrior_cl, AnimatedWarrior_cl, "An animated warrior character that can be controlled with the keyboard", VFORGE_HIDECLASS, "Models\\Warrior\\Warrior.model")
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
