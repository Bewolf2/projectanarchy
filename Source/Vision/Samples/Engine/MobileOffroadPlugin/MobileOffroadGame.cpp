/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/MobileOffroadPlugin/MobileOffroadPCH.h>
#include <Vision/Samples/Engine/MobileOffroadPlugin/MobileOffroadGame.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>
#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

#ifdef _VISION_MOBILE
#include <Vision/Runtime/Common/VSampleTouchMenu.hpp>
#endif

#include <Vision/Samples/Engine/MobileOffroadPlugin/VehicleSetup.h>
#include <Vision/Samples/Engine/MobileOffroadPlugin/Vehicle.h>

#include <Physics2012/Dynamics/World/hkpPhysicsSystem.h>

Vehicle g_cruiser;

#if defined(_VISION_PSP2)
VMotionInputPSP2* pMotionInput = NULL;
#elif defined(_VISION_IOS)
VMotionInputIOS* pMotionInput = NULL;
#elif defined(_VISION_ANDROID)
VMotionInputAndroid* pMotionInput = NULL;
#elif defined(_VISION_WINRT)
VMotionInputWinRT* pMotionInput = NULL;
#endif

/// -------------------------------------------------------------------------- ///
/// Havok demo control                                                         ///
/// -------------------------------------------------------------------------- ///
enum HAVOK_DEMO_CONTROL 
{
  HAVOK_ENABLE_VISUAL_DEBUG,    // Toggle Havok Visual Debugger
  HAVOK_PAUSE_SIMULATION,                                             // Pause Simulation
  HAVOK_STEER_LEFT,                                                   // Steer the car left
  HAVOK_STEER_RIGHT,                                                  // Steer the car right
  HAVOK_ACCELERATE,                                                   // Accelerate
  HAVOK_DECCELERATE,                                                  // Deccelerate
  HAVOK_BRAKE,
  HAVOK_REORIENT,
  HAVOK_RESTART_SIMULATION
};

#define HAVOK_MAX_MPH 50.f
#define HAVOK_ACCELERATION 0.3f
#define HAVOK_STEERING 3.0f
IVTimerPtr s_timer = NULL;

MobileOffRoadGame MobileOffRoadGame::s_instance;

void MobileOffRoadGame::OneTimeInit()
{
  m_playingTheGame = false;

  // the game manager should listen to the following callbacks:
  Vision::Callbacks.OnEditorModeChanged += this;
  Vision::Callbacks.OnAfterSceneLoaded += this;
  Vision::Callbacks.OnAfterSceneUnloaded += this;
  Vision::Callbacks.OnUpdateSceneFinished += this;

  // Set-up the physics callbacks
  vHavokPhysicsModule::OnBeforeInitializePhysics	+= this;
  vHavokPhysicsModule::OnUnsyncHavokStatics		+= this;
  vHavokVisualDebugger::OnCreatingContexts		+= this;
}

void MobileOffRoadGame::OneTimeDeInit()
{
  // Make sure we deinit the objects when playing the game
  SetPlayTheGame(false);

  Vision::Callbacks.OnEditorModeChanged -= this;
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  Vision::Callbacks.OnAfterSceneUnloaded -= this;
  Vision::Callbacks.OnUpdateSceneFinished -= this;

  // Cancel the physics callbacks
  vHavokPhysicsModule::OnBeforeInitializePhysics	-= this;
  vHavokPhysicsModule::OnUnsyncHavokStatics		-= this;
  vHavokVisualDebugger::OnCreatingContexts		-= this;
}

void MobileOffRoadGame::SetPlayTheGame(bool status)
{
  
  if (status == m_playingTheGame)
    return;

  if (status)
  {
    if (Vision::Game.SearchEntity("Cruiser") == HK_NULL)
    {
      // then not allowed to play as no car!
      return;
    }
  }
  
  m_playingTheGame = status;

  if (m_playingTheGame)
  {
    //create input map with free slots for all samples (user_space)
    m_inputMap = new VInputMap(16, 16);

    // For PC debugging
#if defined(SUPPORTS_KEYBOARD) 
    m_inputMap->MapTrigger(HAVOK_PAUSE_SIMULATION,    V_KEYBOARD, CT_KB_P, VInputOptions::Once());
    m_inputMap->MapTrigger(HAVOK_STEER_LEFT,          V_KEYBOARD, CT_KB_LEFT);
    m_inputMap->MapTrigger(HAVOK_STEER_RIGHT,         V_KEYBOARD, CT_KB_RIGHT);  

    m_inputMap->MapTrigger(HAVOK_ACCELERATE,          V_KEYBOARD, CT_KB_UP);
    m_inputMap->MapTrigger(HAVOK_DECCELERATE,         V_KEYBOARD, CT_KB_DOWN);
    m_inputMap->MapTrigger(HAVOK_BRAKE,               V_KEYBOARD, CT_KB_ENTER);
    m_inputMap->MapTrigger(HAVOK_REORIENT,            V_KEYBOARD, CT_KB_SPACE);

    m_inputMap->MapTrigger(HAVOK_RESTART_SIMULATION,            V_KEYBOARD, CT_KB_R, VInputOptions::Once());

#endif

    // Enable motion input for demonstration
#if defined(_VISION_PSP2)
    pMotionInput = (VMotionInputPSP2*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
    pMotionInput->SetEnabled(true);
#elif defined(_VISION_IOS)
    pMotionInput = (VMotionInputIOS*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
    pMotionInput->SetEnabled(true);
#elif defined(_VISION_WINRT)
    pMotionInput = (VMotionInputWinRT*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
#elif defined(_VISION_ANDROID)
    pMotionInput = (VMotionInputAndroid*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
    pMotionInput->SetEnabled(true);
#endif 

#if defined(_VISION_MOBILE)
    const float width = (float)VVideo::GetVideoConfig()->ViewWidth;
    const float height = (float)VVideo::GetVideoConfig()->ViewHeight;

    // Accelerate touch area
    VTouchArea* pScreenAreaRight = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(width * 0.75f, 0, width, height*0.8f));
    m_inputMap->MapTrigger(HAVOK_ACCELERATE, pScreenAreaRight,  CT_TOUCH_ANY);
    
    // Brake touch area
    VTouchArea* pScreenAreaLeft = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(0.f, 0.f, width * 0.25f, height*0.8f));
    m_inputMap->MapTrigger(HAVOK_DECCELERATE, pScreenAreaLeft,  CT_TOUCH_ANY);

    // Reorient touch area
    VTouchArea* pScreenAreaMiddle = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(width * 0.4f, 0.f, width * 0.6f, height));
    m_inputMap->MapTrigger(HAVOK_REORIENT, pScreenAreaMiddle,  CT_TOUCH_ANY);

    // Reset touch area (the anarchy logo currently)
    VTouchArea* pScreenAreaLogo = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(width * 0.8f, height * 0.8f, width, height));
    m_inputMap->MapTrigger(HAVOK_RESTART_SIMULATION, pScreenAreaLogo,  CT_TOUCH_ANY);

#endif

    // Set the timer to a fixed step timer to guarantees that logic updates are run with a fixed time difference
    int fps = 60;
    Vision::GetApplication()->SetSceneUpdateController(new VFixStepSceneUpdateController(fps, 10));
    s_timer = new VFixStepTimer(fps);
    s_timer->Init();
    Vision::SetTimer(s_timer);

    //vHavokPhysicsModule::GetInstance()->SetPhysicsTickCount(60, 1, true);
    g_cruiser.Init();

    RecordBodyState();
  }
  else
  {
    V_SAFE_DELETE(m_inputMap);

    // Remove physics syncing timer
    Vision::SetTimer(NULL);
    Vision::GetTimer()->Init();
    s_timer = NULL;

    g_cruiser.DeInit();

    ClearBodyState();
  }
}

void MobileOffRoadGame::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &vHavokPhysicsModule::OnBeforeInitializePhysics)
  {
    vHavokPhysicsModuleCallbackData* pHavokData = (vHavokPhysicsModuleCallbackData*)pData;

    VISION_HAVOK_SYNC_STATICS();
    VISION_HAVOK_SYNC_PER_THREAD_STATICS(pHavokData->GetHavokModule());
  }

  if ( pData->m_pSender == &vHavokPhysicsModule::OnUnsyncHavokStatics )
  {
    vHavokPhysicsModuleCallbackData *pHavokData = (vHavokPhysicsModuleCallbackData*)pData;

    VISION_HAVOK_UNSYNC_STATICS();
    VISION_HAVOK_UNSYNC_PER_THREAD_STATICS(pHavokData->GetHavokModule());
  }

  if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneLoaded)
  {
    SetPlayTheGame(true);
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneUnloaded) // this is important when running outside vForge
  {
    SetPlayTheGame(false);
    return;
  }

  if(m_playingTheGame && (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished))
  {
    Update();
  }
}
      

void MobileOffRoadGame::RecordBodyState()
{
  // Find all dynmaic rbs in thw world and grab enough state to send them back to here
  vHavokPhysicsModule::GetInstance()->WaitForSimulationToComplete();
  vHavokPhysicsModule::GetInstance()->MarkForRead();

  ClearBodyState();

  hkpWorld* w = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  hkpPhysicsSystem* ps = w->getWorldAsOneSystem();
  const hkArray<hkpRigidBody*>& rbs = ps->getRigidBodies();
  m_recordedState.reserve(rbs.getSize());
  for (int ri=0; ri < rbs.getSize(); ++ri)
  {
    if (!rbs[ri]->isFixedOrKeyframed())
    {
      BodyRecordState& bs = m_recordedState.expandOne();
      bs.b = rbs[ri];
      bs.transform = bs.b->getTransform();
      bs.linVel = bs.b->getLinearVelocity();
      bs.angVel = bs.b->getAngularVelocity();
    }
  }
 
  vHavokPhysicsModule::GetInstance()->UnmarkForRead();
}

void MobileOffRoadGame::RestoreBodyState()
{
  vHavokPhysicsModule::GetInstance()->WaitForSimulationToComplete();
  vHavokPhysicsModule::GetInstance()->MarkForWrite();
  for (int bi=0; bi < m_recordedState.getSize(); ++bi)
  {
    BodyRecordState& bs = m_recordedState[bi];
    hkpRigidBody* b = bs.b;
    b->setTransform(bs.transform);
    b->setLinearVelocity(bs.linVel);
    b->setAngularVelocity(bs.angVel);
    b->activate();
  }
  vHavokPhysicsModule::GetInstance()->UnmarkForWrite();
}

void MobileOffRoadGame::ClearBodyState()
{
  m_recordedState.clearAndDeallocate();
}


void MobileOffRoadGame::Update()
{
  if (m_playingTheGame)
  {
    if (m_inputMap->GetTrigger(HAVOK_RESTART_SIMULATION))
    {
      RestoreBodyState();
    }

    // Pause / Continue physics simulation. When Paused reset motion delta for next update
    if (m_inputMap->GetTrigger(HAVOK_PAUSE_SIMULATION))
    {
      vHavokPhysicsModule::GetInstance()->SetPaused(!vHavokPhysicsModule::GetInstance()->IsPaused());
    }

    bool brake = false;
    bool reverse = false;
    bool fixedControl = false;
    float steering = 0.f;
    float acceleration = 0.f;

#if defined(_VISION_MOBILE)
    fixedControl = true;
    hkvVec3 vAcceleration;
#if defined(_VISION_WIIU)
    vAcceleration.x = VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_X, 0.0f);
    vAcceleration.y = VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_Z, 0.0f);
    vAcceleration.z = VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_Y, 0.0f);
#else
    vAcceleration = pMotionInput->GetAcceleration();
#endif
    
    steering = vAcceleration.x * (HAVOK_STEERING * 0.5f);  // turn tablet to steer
    //acceleration = -vAcceleration.z * HAVOK_ACCELERATION;   // tilt forward back to accel 
    
#elif defined(SUPPORTS_KEYBOARD) 
    fixedControl = false;

    // Steering
    if (m_inputMap->GetTrigger(HAVOK_STEER_LEFT))
    {
      steering -= HAVOK_STEERING;
    }

    if (m_inputMap->GetTrigger(HAVOK_STEER_RIGHT))
    {
      steering += HAVOK_STEERING;
    }
#endif

    if (m_inputMap->GetTrigger(HAVOK_ACCELERATE))
    {
      acceleration = -HAVOK_ACCELERATION;
    }
    else if (m_inputMap->GetTrigger(HAVOK_DECCELERATE))
    {
      if (g_cruiser.GetMPH() > 5.f)
      {
        brake = true;
        acceleration = 0.f;
      }
      else
      {
        reverse = true;
        acceleration = HAVOK_ACCELERATION * 2.f;
      }
    }

    vHavokPhysicsModule::GetInstance()->WaitForSimulationToComplete();
    
    if ( m_inputMap->GetTrigger(HAVOK_REORIENT) )
    {
       vHavokPhysicsModule::GetInstance()->MarkForWrite();
         g_cruiser.Reorient(m_inputMap->GetTrigger(HAVOK_REORIENT));
       vHavokPhysicsModule::GetInstance()->UnmarkForWrite();
    }

    if (m_inputMap->GetTrigger(HAVOK_BRAKE))
    {
      brake = true;
      acceleration = 0.f;
    }
    
    // Update Cruiser
    g_cruiser.SetInput(steering, acceleration, brake, reverse, fixedControl);
    g_cruiser.Update();
  }
}

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
