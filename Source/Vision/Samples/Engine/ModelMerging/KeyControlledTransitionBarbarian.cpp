/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// KeyControlledTransitionBarbarian_cl
// ***********************************************************************************************
// Extends the transition state machine based animated character class with code 
// for converting the keyboard input to character movements.
// ***********************************************************************************************

#include <Vision/Samples/Engine/ModelMerging/ModelMergingPCH.h>
#include <Vision/Samples/Engine/ModelMerging/KeyControlledTransitionBarbarian.hpp>
#include <Vision/Samples/Engine/ModelMerging/TransitionBarbarian.hpp>

KeyControlledTransitionBarbarian_cl::KeyControlledTransitionBarbarian_cl()
{
}

KeyControlledTransitionBarbarian_cl::~KeyControlledTransitionBarbarian_cl()
{
}

void KeyControlledTransitionBarbarian_cl::SetInputStatus(bool bStatus)
{
  m_bInputAllowed = bStatus;
}

bool KeyControlledTransitionBarbarian_cl::GetInputStatus()
{
  return m_bInputAllowed;
}

void KeyControlledTransitionBarbarian_cl::InitFunction()
{

#ifdef SUPPORTS_KEYBOARD

  // setup necessary key settings
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_MOVE_FORWARD, V_KEYBOARD, CT_KB_UP);
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_LEFT,  V_KEYBOARD, CT_KB_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_RIGHT, V_KEYBOARD, CT_KB_RIGHT);

#ifdef WASD_CONTROL
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_MOVE_FORWARD, V_KEYBOARD, CT_KB_W);
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_LEFT,  V_KEYBOARD, CT_KB_A);
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_RIGHT, V_KEYBOARD, CT_KB_D);
#endif

  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_RUN, V_KEYBOARD, CT_KB_LSHIFT);
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_RUN, V_KEYBOARD, CT_KB_RSHIFT);

#endif

  
#if defined(_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))

  // setup necessary key settings
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_MOVE_FORWARD, V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_LEFT,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_RIGHT, V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_RUN, V_XENON_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::DeadZone(0.4f));
  
#elif defined(_VISION_PS3)

  // setup necessary key settings
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_MOVE_FORWARD, V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_LEFT,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_RIGHT, V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_RUN, V_PS3_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::DeadZone(0.4f));

#elif defined(_VISION_PSP2)

  // setup necessary key settings
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_MOVE_FORWARD, V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_LEFT,  V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_RIGHT, V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_RUN, V_PSP2_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::DeadZone(0.4f));

#elif defined(_VISION_WIIU)

  // setup necessary key settings
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_MOVE_FORWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_LEFT,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_ROTATE_RIGHT, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  VisSampleApp::GetInputMap()->MapTrigger(CHARACTER_RUN, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_SHOULDER, VInputOptions::DeadZone(0.4f));

#endif


  m_bInputAllowed = true;

  TransitionBarbarian_cl::InitFunction();
}

void KeyControlledTransitionBarbarian_cl::ThinkFunction()
{
  if (m_bInputAllowed)
    ProcessKeyboardEvents();
  
  TransitionBarbarian_cl::ThinkFunction();
}

void KeyControlledTransitionBarbarian_cl::ProcessKeyboardEvents()
{
  bool bUp = VisSampleApp::GetInputMap()->GetTrigger(CHARACTER_MOVE_FORWARD)!=0;
  bool bRotLeft = VisSampleApp::GetInputMap()->GetTrigger(CHARACTER_ROTATE_LEFT)!=0;
  bool bRotRight = VisSampleApp::GetInputMap()->GetTrigger(CHARACTER_ROTATE_RIGHT)!=0;
  bool bShift = VisSampleApp::GetInputMap()->GetTrigger(CHARACTER_RUN)!=0;

	if (bUp)
  {
    // Trigger the run/walk actions when CURSOR UP is pressed.
    // Allow rotating the entity while walking/running
    if (bShift)
      Run();
    else
      Walk();

		if (bRotLeft)
      RotateLeft();
    else if (bRotRight)
      RotateRight();
  }
  else
  {
    if (bRotLeft)
      RotateLeft();
    else if (bRotRight)
      RotateRight();
    else
      Stand();
  }
}

V_IMPLEMENT_SERIAL( KeyControlledTransitionBarbarian_cl, TransitionBarbarian_cl, 0, Vision::GetEngineModule() );
  START_VAR_TABLE( KeyControlledTransitionBarbarian_cl, TransitionBarbarian_cl, "An animated character that can be controlled with the keyboard", VFORGE_HIDECLASS, "")
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
