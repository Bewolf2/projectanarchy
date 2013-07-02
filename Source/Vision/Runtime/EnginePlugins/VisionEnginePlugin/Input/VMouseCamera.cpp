/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// VisMouseCamera_cl Source
//***********************************************************************

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VMouseCamera.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>

V_IMPLEMENT_SERIAL( VisMouseCamera_cl, VisBaseEntity_cl, 0, &g_VisionEngineModule );

VisMouseCamera_cl::VisMouseCamera_cl()
  : m_fSensitivity(200.0f)
  , m_fUpDownSpeed(1.5f)
  , m_fMoveSpeed(350.0f * Vision::World.GetGlobalUnitScaling())
  , m_bSimulationIndependent(false) // use ThinkFunction
#if defined(SUPPORTS_MULTITOUCH)
  , m_pVirtualThumbStick(NULL)
#endif
{
}

VisMouseCamera_cl::~VisMouseCamera_cl()
{
  V_SAFE_DELETE(m_pInputMap);
  SetSimulationIndependentMovement(false); // make sure callbacks are de-registered

#if defined(SUPPORTS_MULTITOUCH)
  if (m_pVirtualThumbStick != NULL)
  {
    delete m_pVirtualThumbStick;
    m_pVirtualThumbStick = NULL;
    Vision::Callbacks.OnVideoChanged -= this;
  }
#endif
}

// InitFunction: Will be called during the initialisation of each entity instance.
// It is normally used to initialise the entity (e.g. collision setup) and the private variables.
void VisMouseCamera_cl::InitFunction()
{
  BaseInit();

  m_SpeedMode = 0;
  m_walkMode = NONE;
}


void VisMouseCamera_cl::SetWalkMode( VisMouseCamWalkMode_e mode )
{
  //if (m_walkMode == mode)
  //  return;
  //
  m_walkMode = mode;
}


void VisMouseCamera_cl::SetWalkMode( BOOL walk )
{
  if (walk)
    SetWalkMode( WALK );
  else
    SetWalkMode( NONE );
}


// ---------------------------------------------------------------------------------
// Method: BaseInit
// Author: Fabian Roeken, Patrick Harms
// Notes: Base initialization code shared between InitFunction and Serialize
// ---------------------------------------------------------------------------------
void VisMouseCamera_cl::BaseInit()
{
  m_pInputMap = new VInputMap(API_CAMERA_CONTROL_LAST_ELEMENT+1+API_CAMERA_CONTROL_USER_SPACE, API_CAMERA_CONTROL_ALTERNATIVES);

#ifdef _VISION_XENON

  m_pInputMap->MapTrigger(API_CAMERA_MOVE_FORWARD,  VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_BACKWARD, VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_LEFT,     VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_RIGHT,    VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f));

  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,   VInputManagerXenon::GetPad(0), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,   VInputManagerXenon::GetPad(0), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_LOOK_CHANGED, VInputManagerXenon::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(API_CAMERA_HORIZONTAL_LOOK, VInputManagerXenon::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.15f, true));
  m_pInputMap->MapTriggerAxis(API_CAMERA_VERTICAL_LOOK,   VInputManagerXenon::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.15f, true));

  m_pInputMap->MapTrigger(API_CAMERA_ACTION_1, VInputManagerXenon::GetPad(0), CT_PAD_LEFT_SHOULDER,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_2, VInputManagerXenon::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_3, VInputManagerXenon::GetPad(0), CT_PAD_RIGHT_THUMB);

#endif

#ifdef _VISION_PS3
  
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_FORWARD,  VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_BACKWARD, VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_LEFT,     VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_RIGHT,    VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f));

  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerPS3::GetPad(0), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerPS3::GetPad(0), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_LOOK_CHANGED,  VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(API_CAMERA_HORIZONTAL_LOOK, VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.15f, true));
  m_pInputMap->MapTriggerAxis(API_CAMERA_VERTICAL_LOOK,   VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.15f, true));

  m_pInputMap->MapTrigger(API_CAMERA_ACTION_1, VInputManagerPS3::GetPad(0), CT_PAD_LEFT_SHOULDER,  VInputOptions::DeadZone(0.4f));
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_2, VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_3, VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_THUMB);

#endif

#ifdef _VISION_PSP2

  m_pInputMap->MapTrigger(API_CAMERA_MOVE_FORWARD,  VInputManagerPSP2::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_BACKWARD, VInputManagerPSP2::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_LEFT,     VInputManagerPSP2::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_RIGHT,    VInputManagerPSP2::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f));

  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerPSP2::GetPad(0), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerPSP2::GetPad(0), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerPSP2::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_LOOK_CHANGED,  VInputManagerPSP2::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(API_CAMERA_HORIZONTAL_LOOK, VInputManagerPSP2::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.15f, true));
  m_pInputMap->MapTriggerAxis(API_CAMERA_VERTICAL_LOOK,   VInputManagerPSP2::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.15f, true));

  m_pInputMap->MapTrigger(API_CAMERA_ACTION_1,   VInputManagerPSP2::GetPad(0), CT_PAD_LEFT_SHOULDER, VInputOptions::DeadZone(0.4f));
  //m_pInputMap->MapTrigger(API_CAMERA_ACTION_2, VInputManagerPSP2::GetPad(0), CT_PAD_RIGHT_TRIGGER);
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_3,   VInputManagerPSP2::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.25f));

#endif
  
#if defined(_VISION_MOBILE) || defined(_VISION_WINRT)
  
  VTouchArea* pCameraLookArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(), -2000.0f);
  m_pInputMap->MapTrigger(API_CAMERA_LOOK_CHANGED,    pCameraLookArea, CT_TOUCH_ANY);

  m_pInputMap->MapTrigger(API_CAMERA_HORIZONTAL_LOOK, pCameraLookArea, CT_TOUCH_ABS_DELTA_X, VInputOptions::Sensitivity(0.25f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(API_CAMERA_VERTICAL_LOOK,   pCameraLookArea, CT_TOUCH_ABS_DELTA_Y, VInputOptions::Sensitivity(0.25f / Vision::Video.GetDeviceDpi()));
  
  m_pInputMap->MapTrigger(API_CAMERA_DOUBLE_TAP, pCameraLookArea, CT_TOUCH_DOUBLE_TAP, VInputOptions::OncePerFrame());
  m_pInputMap->MapTrigger(API_CAMERA_TAP_X, pCameraLookArea, CT_TOUCH_TAP_X);
  m_pInputMap->MapTrigger(API_CAMERA_TAP_Y, pCameraLookArea, CT_TOUCH_TAP_Y);
  
#endif

#ifdef SUPPORTS_KEYBOARD

  m_pInputMap->MapTrigger(API_CAMERA_MOVE_FORWARD,  V_KEYBOARD, CT_KB_UP);
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_BACKWARD, V_KEYBOARD, CT_KB_DOWN);
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_LEFT,     V_KEYBOARD, CT_KB_LEFT);
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_RIGHT,    V_KEYBOARD, CT_KB_RIGHT);

  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    V_KEYBOARD, CT_KB_UNKNOWN);

#endif

#ifdef SUPPORTS_MOUSE
  
  m_pInputMap->MapTrigger(API_CAMERA_LOOK_CHANGED,  V_MOUSE, CT_MOUSE_DATA_CHANGED);

  m_pInputMap->MapTrigger(API_CAMERA_HORIZONTAL_LOOK, V_MOUSE, CT_MOUSE_ABS_DELTA_X, VInputOptions::Sensitivity(0.15f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(API_CAMERA_VERTICAL_LOOK,   V_MOUSE, CT_MOUSE_ABS_DELTA_Y, VInputOptions::Sensitivity(0.15f / Vision::Video.GetDeviceDpi()));

  m_pInputMap->MapTrigger(API_CAMERA_ACTION_1, V_MOUSE, CT_MOUSE_LEFT_BUTTON);
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_2, V_MOUSE, CT_MOUSE_RIGHT_BUTTON);
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_3, V_MOUSE, CT_MOUSE_MIDDLE_BUTTON);
#endif

#ifdef _VISION_WIIU

  // DRC mapping
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f));

  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_LOOK_CHANGED,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(API_CAMERA_HORIZONTAL_LOOK, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.15f, true));
  m_pInputMap->MapTriggerAxis(API_CAMERA_VERTICAL_LOOK,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.15f, true));

  m_pInputMap->MapTrigger(API_CAMERA_ACTION_2, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_3, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_SHOULDER,  VInputOptions::DeadZone(0.4f));

  // Pro controller mapping
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_FORWARD,  VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_BACKWARD, VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_LEFT,     VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_RIGHT,    VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f)); 

  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerWiiU::GetRemote(0), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_ANY_ACTION,    VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(API_CAMERA_LOOK_CHANGED,  VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(API_CAMERA_HORIZONTAL_LOOK, VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.05f, true));
  m_pInputMap->MapTriggerAxis(API_CAMERA_VERTICAL_LOOK,   VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.05f, true));

  m_pInputMap->MapTrigger(API_CAMERA_ACTION_2, VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.1f));
  m_pInputMap->MapTrigger(API_CAMERA_ACTION_3, VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_SHOULDER,  VInputOptions::DeadZone(0.2f));

#endif

#if defined(_VISION_ANDROID) 

  // Additional input possibilities for devices with playstation buttons
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_FORWARD,  VInputManagerAndroid::GetKeyInput(), CT_PAD_UP,    VInputOptions::Alternative(1));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_BACKWARD, VInputManagerAndroid::GetKeyInput(), CT_PAD_DOWN,  VInputOptions::Alternative(1));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_LEFT,     VInputManagerAndroid::GetKeyInput(), CT_PAD_LEFT,  VInputOptions::Alternative(1));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_RIGHT,    VInputManagerAndroid::GetKeyInput(), CT_PAD_RIGHT, VInputOptions::Alternative(1));

#endif

#if defined(SUPPORTS_MULTITOUCH) && (defined(_VISION_MOBILE) || defined(_VISION_WINRT))
  VASSERT(m_pVirtualThumbStick == NULL);
  m_pVirtualThumbStick = new VVirtualThumbStick();
  Vision::Callbacks.OnVideoChanged += this; // update valid area when resolution changes

  m_pInputMap->MapTrigger(API_CAMERA_MOVE_LEFT, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_LEFT, VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_RIGHT, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_FORWARD, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(API_CAMERA_MOVE_BACKWARD, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.2f));
#endif

  // attach camera to entity
  Vision::Camera.AttachToEntity(this, hkvVec3::ZeroVector ());
}



void VisMouseCamera_cl::TickFunction(float fTimeDiff)
{
  hkvVec3 vMove = hkvVec3::ZeroVector();
  BOOL bUpDownMode = FALSE;

  float dx = m_pInputMap->GetTrigger(API_CAMERA_HORIZONTAL_LOOK);
  float dy = m_pInputMap->GetTrigger(API_CAMERA_VERTICAL_LOOK);

  if (m_pInputMap->GetTrigger(API_CAMERA_ACTION_1) && m_pInputMap->GetTrigger(API_CAMERA_ACTION_2))
    bUpDownMode = TRUE;
  else if (m_pInputMap->GetTrigger(API_CAMERA_ACTION_2))
    m_SpeedMode = 1;
  else if (m_pInputMap->GetTrigger(API_CAMERA_ACTION_3))
    m_SpeedMode = 2;
  else
    m_SpeedMode = 0;

  // handle keyboard status
#if !defined(_VISION_MOBILE) && !defined(_VISION_WINRT)
  if (m_pInputMap->GetTrigger(API_CAMERA_ANY_ACTION))
#endif
  {
    hkvVec3 vDir(hkvNoInitialization), vRight(hkvNoInitialization), vUp(hkvNoInitialization);
    vUp.set(0.f,0.f,1.f);
    if (GetPhysicsObject())
    {
      // local space
      vDir.set(1.f,0.f,0.f);
      vRight.set(0.f,1.f,0.f);
    } 
    else
    {
      hkvMat3 mat(hkvNoInitialization);
      GetRotationMatrix(mat);
      vDir = mat.getAxis(0);
      vRight = mat.getAxis(1);
    }

    float fMaxSpeed = m_fMoveSpeed;
    if (m_SpeedMode == 1)
      fMaxSpeed *= 3.0f;
    else if (m_SpeedMode == 2)
      fMaxSpeed *= 9.0f;

    // Accumulate move directions (multiply in order to take analog input into account).
    vMove += vDir * m_pInputMap->GetTrigger(API_CAMERA_MOVE_FORWARD);
    vMove -= vDir * m_pInputMap->GetTrigger(API_CAMERA_MOVE_BACKWARD);
    vMove -= vRight * m_pInputMap->GetTrigger(API_CAMERA_MOVE_RIGHT);
    vMove += vRight * m_pInputMap->GetTrigger(API_CAMERA_MOVE_LEFT);
    vMove += vUp *  m_pInputMap->GetTrigger(API_CAMERA_MOVE_UP);
    vMove -= vUp * m_pInputMap->GetTrigger(API_CAMERA_MOVE_DOWN);
    vMove *= fMaxSpeed;
   
    // Clamp movement, so that moving diagonally is not faster than moving straight 
    // when using digital input.
    const float fSpeed = vMove.getLength();
    if (fSpeed > fMaxSpeed)
      vMove.setLength(fMaxSpeed);
    vMove *= fTimeDiff;
  }

  if (m_pInputMap->GetTrigger(API_CAMERA_LOOK_CHANGED))
  {
    if (bUpDownMode)
    {
      IncOrientation(-dx * m_fSensitivity, 0, 0);
      vMove.z -= dy * m_fUpDownSpeed;
    }
    else
    {
      IncOrientation(-dx * m_fSensitivity, dy * m_fSensitivity, 0);                   
    }
  }

  switch(m_walkMode)
  {
  case WALK:
    // constrain vMove to the ground
    float len = vMove.getLength();
    vMove.z = 0.f;
    vMove.setLength(len);
    break;
  }

  if (GetPhysicsObject())
  {
    IncMotionDeltaLocalSpace(vMove);
  }
  else
  {
    IncPosition( vMove );
  }
}


// ThinkFunction: Will be called by the engine once per tick/frame.
// Useful for regular updates (e.g. keyboard, artificial intelligence).
void VisMouseCamera_cl::ThinkFunction()
{
  if (m_bSimulationIndependent)
    return;

  float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
  TickFunction(fTimeDiff);
}



void VisMouseCamera_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnFrameUpdatePreRender)
  {
    VASSERT(m_bSimulationIndependent);

    // this function gets called once per frame, not once per simulation tick

    if (this->GetThinkFunctionStatus()) // take the same flag into account
    {
      float fTimeDiff = Vision::GetUITimer()->GetTimeDifference();
      TickFunction(fTimeDiff);
    }
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
#if defined(SUPPORTS_MULTITOUCH)
    // Re-initialize valid area
    if (m_pVirtualThumbStick != NULL)
      m_pVirtualThumbStick->SetValidArea(VRectanglef());
#endif
  }
}


void VisMouseCamera_cl::SetSimulationIndependentMovement(bool bStatus)
{
  if (m_bSimulationIndependent==bStatus)
    return;
  m_bSimulationIndependent = bStatus;
  if (m_bSimulationIndependent)
    Vision::Callbacks.OnFrameUpdatePreRender += this;
  else
    Vision::Callbacks.OnFrameUpdatePreRender -= this;
}


// ---------------------------------------------------------------------------------
// Method: Serialize
// Author: Fabian Roeken, Patrick Harms
// ---------------------------------------------------------------------------------
void VisMouseCamera_cl::Serialize( VArchive& ar )
{
  VisBaseEntity_cl::Serialize( ar );

  if ( ar.IsLoading() )
  {
    ar >> m_SpeedMode;

    // all base initialization takes place now :-)
    BaseInit();
  }
  else
  {
    ar << m_SpeedMode;
  }
}

VInputMap* VisMouseCamera_cl::GetInputMap()
{
  return m_pInputMap;
}

START_VAR_TABLE(VisMouseCamera_cl, VisBaseEntity_cl, "VisMouseCamera_cl", VFORGE_HIDECLASS, "")  
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
