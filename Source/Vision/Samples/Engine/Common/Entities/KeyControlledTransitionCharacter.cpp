/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// KeyControlledTransitionCharacter_cl
// ***********************************************************************************************
// Extends the transition state machine based animated character class with code 
// for converting the keyboard input to character movements.
// ***********************************************************************************************

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/KeyControlledTransitionCharacter.hpp>
#include <Vision/Samples/Engine/Common/Entities/TransitionCharacter.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>

#if defined(_VISION_IOS)
  #include <Vision/Runtime/Base/Graphics/Video/IOSBridge.h>
#endif

KeyControlledTransitionCharacter_cl::KeyControlledTransitionCharacter_cl() 
  : m_bKeyboardInputAllowed(false)
  , m_pInputMap(NULL)
#if defined(SUPPORTS_MULTITOUCH)
  , m_pVirtualThumbStick(NULL)
#endif
{
}

KeyControlledTransitionCharacter_cl::~KeyControlledTransitionCharacter_cl()
{
#if defined(SUPPORTS_MULTITOUCH)
  V_SAFE_DELETE(m_pVirtualThumbStick);
#endif
}

void KeyControlledTransitionCharacter_cl::SetInputStatus(bool bStatus)
{
  m_bKeyboardInputAllowed = bStatus;
}

bool KeyControlledTransitionCharacter_cl::GetInputStatus()
{
  return m_bKeyboardInputAllowed;
}

void KeyControlledTransitionCharacter_cl::InitFunction()
{
  m_pInputMap = new VInputMap(CHARACTER_CONTROL_LAST_ELEMENT+1+CHARACTER_CONTROL_USER_SPACE, CHARACTER_CONTROL_ALTERNATIVES);

#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
  #ifdef SUPPORTS_KEYBOARD

    // setup necessary key settings
    m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_PC_KEYBOARD, CT_KB_UP,    VInputOptions::Alternative(0));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_PC_KEYBOARD, CT_KB_KP_8,  VInputOptions::Alternative(1));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_PC_KEYBOARD, CT_KB_DOWN,  VInputOptions::Alternative(0));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_PC_KEYBOARD, CT_KB_KP_2,  VInputOptions::Alternative(1));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_PC_KEYBOARD, CT_KB_LEFT,  VInputOptions::Alternative(0));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_PC_KEYBOARD, CT_KB_KP_4,  VInputOptions::Alternative(1));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_PC_KEYBOARD, CT_KB_RIGHT, VInputOptions::Alternative(0));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_PC_KEYBOARD, CT_KB_KP_6,  VInputOptions::Alternative(1));
  
  #ifdef WASD_CONTROL
    m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_PC_KEYBOARD, CT_KB_W, VInputOptions::Alternative(2));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_PC_KEYBOARD, CT_KB_S, VInputOptions::Alternative(2));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_PC_KEYBOARD, CT_KB_A, VInputOptions::Alternative(2));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_PC_KEYBOARD, CT_KB_D, VInputOptions::Alternative(2));
  #endif

    m_pInputMap->MapTrigger(CHARACTER_RUN, V_PC_KEYBOARD, CT_KB_LSHIFT, VInputOptions::Alternative(0));
    m_pInputMap->MapTrigger(CHARACTER_RUN, V_PC_KEYBOARD, CT_KB_RSHIFT, VInputOptions::Alternative(1));
  
  #endif
#endif


#if defined(_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))

  // setup necessary key settings
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f, false, 0));

  m_pInputMap->MapTrigger(CHARACTER_RUN, V_XENON_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f, false, 0));


  #ifdef SUPPORTS_KEYBOARD

    // Movement
    m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  VInputManagerXenon::GetKeyboard(), CT_KB_KP_8, VInputOptions::Alternative(1));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, VInputManagerXenon::GetKeyboard(), CT_KB_KP_2, VInputOptions::Alternative(1));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     VInputManagerXenon::GetKeyboard(), CT_KB_KP_4, VInputOptions::Alternative(1));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    VInputManagerXenon::GetKeyboard(), CT_KB_KP_6, VInputOptions::Alternative(1));
    
  #ifdef WASD_CONTROL
    m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  VInputManagerXenon::GetKeyboard(), CT_KB_W, VInputOptions::Alternative(2));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, VInputManagerXenon::GetKeyboard(), CT_KB_S, VInputOptions::Alternative(2));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     VInputManagerXenon::GetKeyboard(), CT_KB_A, VInputOptions::Alternative(2));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    VInputManagerXenon::GetKeyboard(), CT_KB_D, VInputOptions::Alternative(2));
  #endif

    // Run
    m_pInputMap->MapTrigger(CHARACTER_RUN, VInputManagerXenon::GetKeyboard(), CT_KB_LSHIFT, VInputOptions::Alternative(1));

  #endif

#endif




#ifdef _VISION_PS3

  // setup necessary key settings
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f, false, 0));

  m_pInputMap->MapTrigger(CHARACTER_RUN, V_PS3_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f, false, 0));

#endif




#ifdef _VISION_WIIU

  // setup necessary key settings
  m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f, false, 0));
  m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f, false, 0));

  m_pInputMap->MapTrigger(CHARACTER_RUN, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f, false, 0));

#endif




#if defined(_VISION_MOBILE)

  #if defined(SUPPORTS_MULTITOUCH)
    m_pVirtualThumbStick = new VVirtualThumbStick();

    m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD,  *m_pVirtualThumbStick,   CT_PAD_LEFT_THUMB_STICK_UP,     VInputOptions::DeadZone(0.2f));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_BACKWARD, *m_pVirtualThumbStick,   CT_PAD_LEFT_THUMB_STICK_DOWN,   VInputOptions::DeadZone(0.2f));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_LEFT,     *m_pVirtualThumbStick,   CT_PAD_LEFT_THUMB_STICK_LEFT,   VInputOptions::DeadZone(0.2f));
    m_pInputMap->MapTrigger(CHARACTER_MOVE_RIGHT,    *m_pVirtualThumbStick,   CT_PAD_LEFT_THUMB_STICK_RIGHT,  VInputOptions::DeadZone(0.2f));

  #endif

#endif

  m_bKeyboardInputAllowed = true;

  TransitionCharacter_cl::InitFunction();
}

void KeyControlledTransitionCharacter_cl::DeInitFunction()
{
  V_SAFE_DELETE(m_pInputMap);

  TransitionCharacter_cl::DeInitFunction();
}

void KeyControlledTransitionCharacter_cl::ThinkFunction()
{
  if (IsObjectFlagSet(VObjectFlag_IsNetworkReplica))
  {
    SetThinkFunctionStatus(FALSE);
    m_bKeyboardInputAllowed = false;
    return;
  }

  ProcessKeyboardEvents();
  TransitionCharacter_cl::ThinkFunction();
}

void KeyControlledTransitionCharacter_cl::ProcessKeyboardEvents()
{
  if(!m_bKeyboardInputAllowed) 
    return;

  bool bLeft = m_pInputMap->GetTrigger(CHARACTER_MOVE_LEFT)!=0;
  bool bRight = m_pInputMap->GetTrigger(CHARACTER_MOVE_RIGHT)!=0;
  bool bUp = m_pInputMap->GetTrigger(CHARACTER_MOVE_FORWARD)!=0;
  bool bDown = m_pInputMap->GetTrigger(CHARACTER_MOVE_BACKWARD)!=0;
  bool bShift = m_pInputMap->GetTrigger(CHARACTER_RUN)!=0;

	if (bUp)
  {
    // Trigger the run/walk actions when CURSOR UP is pressed.
    // Allow rotating the entity while walking/running
    if (bShift)
      Run();
    else
      Walk();

		if (bLeft)
      RotateLeft();
    else if (bRight)
      RotateRight();
  }
  else if (bDown)
  {
    // Trigger the walk backward action when CURSOR DOWN is pressed.
    // Allow rotating the entity while walking backwards
    WalkBackwards();

    if (bLeft)
      RotateLeft();
    else if (bRight)
      RotateRight();
  }
  else
  {
    if (bLeft)
      RotateLeft();
    else if (bRight)
      RotateRight();
    else
      Stand();
  }
}

V_IMPLEMENT_SERIAL( KeyControlledTransitionCharacter_cl, TransitionCharacter_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(KeyControlledTransitionCharacter_cl, TransitionCharacter_cl, "An animated warrior character that can be controlled with the keyboard", VFORGE_HIDECLASS, "Models\\Warrior\\WarriorEvents.MODEL")
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
