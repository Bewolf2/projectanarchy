/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// SerializePlayer_cl Source
//***********************************************************************

#include <Vision/Samples/Engine/Serialization/SerializationPCH.h>
#include <Vision/Samples/Engine/Serialization/SerializeBaseEntity.hpp>
#include <Vision/Samples/Engine/Serialization/SerializePlayer.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>

V_IMPLEMENT_SERIAL( SerializePlayer_cl, SerializeBaseEntity_cl, 0, &g_serializeSampleModule );

#define MOUSE_SENSITIVITY 0.5f
#define MOVE_UNITSPERSECOND 350.0f * Vision::World.GetGlobalUnitScaling()

// ---------------------------------------------------------------------------------
// Method: InitFunction
// Author: Trinigy
// Notes: 
// ---------------------------------------------------------------------------------
void SerializePlayer_cl::InitFunction()
{
  // call base class InitFunction
  Inherited::InitFunction();

  CharacterController *pPhys = new CharacterController;
  pPhys->Initialize();

  pPhys->Character_Top.set(0,0,0);
  pPhys->Character_Bottom.set(0,0,-140);
	pPhys->Capsule_Radius = 30;

  AddComponent(pPhys);

  // carry out the rest of the initialization
  BaseInit();
}

void SerializePlayer_cl::DeInitFunction()
{
#if defined(SUPPORTS_MULTITOUCH)
  if (m_pVirtualThumbStick != NULL)
  {
    // Unmap triggers because input device (thumb stick) is deleted.
    if (VisSampleApp::GetInputMap() != NULL)
    {
      VisSampleApp::GetInputMap()->UnmapInput(SERIALIZE_MOVE_FORWARD, 2);
      VisSampleApp::GetInputMap()->UnmapInput(SERIALIZE_MOVE_BACKWARD, 2);
      VisSampleApp::GetInputMap()->UnmapInput(SERIALIZE_MOVE_LEFT, 2);
      VisSampleApp::GetInputMap()->UnmapInput(SERIALIZE_MOVE_RIGHT, 2);
    }

    V_SAFE_DELETE(m_pVirtualThumbStick);
  }
#endif
}

// ---------------------------------------------------------------------------------
// Method: BaseInit
// Author: Trinigy
// Notes: Code that is shared between InitFunction and Serialize
// ---------------------------------------------------------------------------------
void SerializePlayer_cl::BaseInit()
{
#if defined(SUPPORTS_MULTITOUCH)
  m_pVirtualThumbStick = NULL;
#endif

  m_SpeedMode = 0;

  m_fHalfScreenSizeX = (float)(Vision::Video.GetXRes()>>1);
  m_fHalfScreenSizeY = (float)(Vision::Video.GetYRes()>>1);

  // attach the camera to ourselves
  Vision::Camera.AttachToEntity(this, hkvVec3(0,0,0));

  // Get the physics component
  m_pPhys = (vHavokCharacterController *) Components().GetComponentOfType(V_RUNTIME_CLASS(vHavokCharacterController));

#if defined(_VISION_MOBILE)

  // as screen resolution is incorporated twice into camera movement
  // we divide our screen factor by the square of the content scale
  // factor (which is 2 if we are running on a retina display and
  // 1 otherwise)
  float fContentScaleFactorSquared = hkvMath::Max( 1.0f, hkvMath::floor( Vision::Video.GetVideoConfig()->DisplayDensity ) );
  fContentScaleFactorSquared *= fContentScaleFactorSquared;
  m_fHalfScreenSizeX /= fContentScaleFactorSquared;
  m_fHalfScreenSizeY /= fContentScaleFactorSquared;

  // Define alternative explicitly because init function can be called multiple times.
  // This way, it won't add up but replace the mappings in the input map.
  VTouchArea* pCameraLookArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(), -2000.0f);
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,    pCameraLookArea, CT_TOUCH_ANY, VInputOptions::Alternative(2));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_HORIZONTAL_LOOK, pCameraLookArea, CT_TOUCH_NORM_DELTA_X, VInputOptions::Sensitivity(2.0f, 2));
  VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_VERTICAL_LOOK,   pCameraLookArea, CT_TOUCH_NORM_DELTA_Y, VInputOptions::Sensitivity(2.0f, 2));
  
  #if defined(SUPPORTS_MULTITOUCH)
    m_pVirtualThumbStick = new VVirtualThumbStick();
  
    VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  *m_pVirtualThumbStick,   CT_PAD_LEFT_THUMB_STICK_UP,     VInputOptions::DeadZone(0.2f, false, 2));
    VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, *m_pVirtualThumbStick,   CT_PAD_LEFT_THUMB_STICK_DOWN,   VInputOptions::DeadZone(0.2f, false, 2));
    VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     *m_pVirtualThumbStick,   CT_PAD_LEFT_THUMB_STICK_LEFT,   VInputOptions::DeadZone(0.2f, false, 2));
    VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    *m_pVirtualThumbStick,   CT_PAD_LEFT_THUMB_STICK_RIGHT,  VInputOptions::DeadZone(0.2f, false, 2));
  
  #endif

  #if defined(_VISION_ANDROID) 
  
    // Additional input possibilities for devices with playstation buttons
    VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  VInputManagerAndroid::GetKeyInput(), CT_PAD_UP,    VInputOptions::Alternative(3));
    VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, VInputManagerAndroid::GetKeyInput(), CT_PAD_DOWN,  VInputOptions::Alternative(3));
    VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     VInputManagerAndroid::GetKeyInput(), CT_PAD_LEFT,  VInputOptions::Alternative(3));
    VisSampleApp::GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    VInputManagerAndroid::GetKeyInput(), CT_PAD_RIGHT, VInputOptions::Alternative(3));
  
  #endif

#endif

}

// ---------------------------------------------------------------------------------
// Method: ThinkFunction
// Author: Trinigy
// Notes: 
// ---------------------------------------------------------------------------------
void SerializePlayer_cl::ThinkFunction()
{
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference();

  float dx = VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_HORIZONTAL_LOOK)*m_fHalfScreenSizeX;
  float dy = VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_VERTICAL_LOOK)*m_fHalfScreenSizeY;
  
  // check modes
  if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_RUN))
    m_SpeedMode = 1;
  else if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_RUN_FASTER))
    m_SpeedMode = 2;
  else
    m_SpeedMode = 0;

  float fMaxSpeed = MOVE_UNITSPERSECOND;
  if (m_SpeedMode == 1)
    fMaxSpeed *= 3.0f;
  else if (m_SpeedMode == 2)
    fMaxSpeed *= 9.0f;

  // constrain movement to the ground plane by setting the Z axis to 0
  hkvVec3 vForward = GetObjDir();
  vForward.z = 0.0f;
  vForward.normalize();

  // Accumulate move directions (multiply in order to take analog input into account).
  hkvVec3 vMove(hkvVec3::ZeroVector());
  vMove += vForward *  VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_MOVE_FORWARD);
  vMove -= vForward *  VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_MOVE_BACKWARD);
  vMove -= GetObjDir_Right() *  VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_MOVE_RIGHT);
  vMove += GetObjDir_Right() *  VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_MOVE_LEFT);
  vMove *= fMaxSpeed;

  // Clamp movement, so that moving diagonally is not faster than moving straight 
  // when using digital input.
  const float fSpeed = vMove.getLength();
  if (fSpeed > fMaxSpeed)
    vMove.setLength(fMaxSpeed);
  vMove *= Vision::GetTimer()->GetTimeDifference();

  IncMotionDeltaWorldSpace(vMove);

  // handle mouse movement
  if (VisSampleApp::GetInputMap()->GetTrigger(SERIALIZE_LOOK_CHANGED))
  {
    IncOrientation(-dx * MOUSE_SENSITIVITY, dy * MOUSE_SENSITIVITY, 0);   
  }
}


// ---------------------------------------------------------------------------------
// Method: Serialize
// Author: Trinigy
// Notes: 
// ---------------------------------------------------------------------------------
void SerializePlayer_cl::Serialize( VArchive& ar )
{
  // call base class Serialize function
  Inherited::Serialize( ar );

  // serialize additional data
  if ( ar.IsLoading() )
  {
    BaseInit();
  }
  else
  {
  }
}

START_VAR_TABLE(SerializePlayer_cl, SerializeBaseEntity_cl, "SerializePlayer_cl", 0, "")
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
