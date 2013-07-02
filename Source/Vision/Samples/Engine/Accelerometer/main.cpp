/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Accelerometer
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This sample demonstrates the use of multi-touch and motion input for the PS Vita.
//
// The multi-touch feature is used for controlling the camera. You can simultaneously 
// move the camera by pressing screen buttons in the lower left corner of the screen
// and adjust the orientation by scrolling over the screen in view direction.
// Thereby the area of the movement buttons have a higher priority than the area, which 
// is assigned for the camera orientation.
// 
// The motion-input (accelerometer) feature is used to move a ball through the maze
// scene. The ball will roll into the direction, to which the device is inclined to.
// ***********************************************************************************************
#include <Vision/Samples/Engine/Accelerometer/AccelerometerPCH.h>
#include <Vision/Samples/Engine/Accelerometer/GameState.hpp>
#include <Vision/Samples/Engine/Accelerometer/Camera.hpp>
#include <Vision/Samples/Engine/Accelerometer/Ball.hpp>

#ifdef WIN32
#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/IVRemoteInput.hpp>
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

// defines 
#define CAMERA_START_POSITION     hkvVec3(-100.f, 0.f, 800.f)
#define CAMERA_START_ORIENTATION  hkvVec3(0.0f, 90.0f, 0.0f)
#define BALL_START_POSITION       hkvVec3(392.0f, -207.0f, -237.0f)
#define OFFSET 25
#define TOUCHDEBUG VISION_INPUT_CONTROL_LAST_ELEMENT + 10

VisSampleAppPtr spApp;
GameState_cl* pGameStateEntity = NULL;
Camera_cl* pCameraEntity = NULL;
Ball_cl* pBallEntity = NULL;
VGUIMainContextPtr spGUIContext;
float valueX;
float valueY;
float diffX;
float diffY;
bool renderDebug;
IVMultiTouchInput* touchscreen;


#if defined(_VISION_PSP2)
  VMotionInputPSP2* pMotionInput = NULL;
#elif defined(_VISION_IOS)
  VMotionInputIOS* pMotionInput = NULL;
#elif defined(_VISION_ANDROID)
  VMotionInputAndroid* pMotionInput = NULL;
#elif defined(_VISION_WINRT)
  VMotionInputWinRT* pMotionInput = NULL;
#elif defined(WIN32)
  VMotionInputPC* pMotionInput = NULL;
  IVRemoteInput* pRemoteInput = NULL;
#endif


VISION_SET_SUPPORTED_ORIENTATIONS(VisOrientationLandscapeLeft);

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);
  VisionAppHelpers::MakeEXEDirCurrent();

  VisSampleApp::LoadVisionEnginePlugin();
  VISION_PLUGIN_ENSURE_LOADED(vHavok);

#if defined(WIN32) && !defined(_VISION_WINRT)
  VISION_PLUGIN_ENSURE_LOADED(vRemoteInput);
  pRemoteInput = (IVRemoteInput*)Vision::Plugins.GetRegisteredPlugin("vRemoteInput");
  pRemoteInput->StartServer("RemoteGui");
  pRemoteInput->AddVariable("ballXPos",0.0f);
  pRemoteInput->AddVariable("ballYPos",0.0f);
#endif

  // Create an application
  spApp = new VisSampleApp();
  
  int flags = VSAMPLE_INIT_DEFAULTS;
  flags |= VSAMPLE_WAITRETRACE;
  flags &= ~VSAMPLE_ASKFULLSCREEN;

#ifdef _VISION_WIIU
  flags |= VSAMPLE_MULTISAMPLE4X;
  flags &= ~VSAMPLE_WIIU_DRCDEMO;
  flags |= VSAMPLE_WIIU_DRCCOPY;
#endif

  valueX = 0.f;
  valueY = 0.f;
  diffX = 0.f;
  diffY = 0.f;
  renderDebug = false;
  touchscreen = NULL;


  // Init the application
  if (!spApp->InitSample("Accelerometer" /*DataDir*/, "Maze" /*SampleScene*/, flags))
    return false;
  
  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  // Create a game state entity
  pGameStateEntity = (GameState_cl*)Vision::Game.CreateEntity( "GameState_cl", hkvVec3(), "");

  // Create a camera entity
  pCameraEntity = (Camera_cl*)Vision::Game.CreateEntity( "Camera_cl", CAMERA_START_POSITION);
  pCameraEntity->SetOrientation(CAMERA_START_ORIENTATION);
 
  // Create a ball entity
  pBallEntity = (Ball_cl*)Vision::Game.CreateEntity("Ball_cl", BALL_START_POSITION, "Ball.MODEL");

  // Create a GUI context and load the GUI resources
  spGUIContext = new VGUIMainContext(NULL); // NULL means using the VGUIManager::GlobalManager
  spGUIContext->SetActivate(false);  

  VGUIManager::GlobalManager().LoadResourceFile("GUI\\MenuSystem.xml");

#if defined(WIN32)
  VisSampleApp::GetInputMap()->MapTrigger(TOUCHDEBUG, V_PC_KEYBOARD, CT_KB_Q, VInputOptions::Once());
#elif defined(_VISION_PSP2)
  VisSampleApp::GetInputMap()->MapTrigger(TOUCHDEBUG, VInputManager::GetPad(0), CT_PAD_CROSS, VInputOptions::Once());
#endif

  // Try to setup pointer to touchscreen device
  IVInputDevice* inputDevice = &VInputManager::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN);
  if(inputDevice != &VInputManager::s_NoInputDevice)
  {
    touchscreen = static_cast<IVMultiTouchInput*>(inputDevice);
  }

  // Enable motion input
#if defined(_VISION_PSP2)
  pMotionInput = (VMotionInputPSP2*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
  pMotionInput->SetEnabled(true);
#elif defined(_VISION_IOS)
  pMotionInput = (VMotionInputIOS*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
  pMotionInput->SetEnabled(true);
#elif defined(_VISION_WINRT)
  pMotionInput = (VMotionInputWinRT*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
#elif defined(WIN32)
  pMotionInput = (VMotionInputPC*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
#elif defined(_VISION_ANDROID)
  pMotionInput = (VMotionInputAndroid*)(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
  pMotionInput->SetEnabled(true);
#endif 
  
  spApp->AddHelpText("rotate your device and try to get the ball onto the finish field");
  spApp->SetShowHelpText(true);
}

VISION_SAMPLEAPP_RUN
{
  // Run the main loop of the application until we quit
  if (spApp->Run())
  { 
#if defined(WIN32) && !defined(_VISION_WINRT)
    pRemoteInput->DebugDrawTouchAreas(VColorRef(255,255,255));
    pRemoteInput->DebugDrawTouchPoints(VColorRef(255,0,0));

    hkvVec3 vBallPos = pBallEntity->GetPosition();
    pRemoteInput->UpdateVariable("ballXPos",vBallPos.x);
    pRemoteInput->UpdateVariable("ballYPos",vBallPos.y);
#endif

#if defined(_VISION_IOS) || defined(_VISION_PSP2) || defined(WIN32) || defined(_VISION_ANDROID) || defined(_VISION_WIIU)
    hkvVec3 vAcceleration;
#if defined(_VISION_WIIU)
    vAcceleration.x = -VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_X, 0.0f);
    vAcceleration.y = VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_Z, 0.0f);
    vAcceleration.z = VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_Y, 0.0f);
#else
    vAcceleration = pMotionInput->GetAcceleration();
#endif

    hkvVec2 vMult;
    vMult.x = vAcceleration.x * fabs( vAcceleration.x );
    vMult.y = vAcceleration.y * fabs( vAcceleration.y );
    vMult *= BALL_MASS;
    
    hkvMat3 mat(hkvNoInitialization);
    pCameraEntity->GetRotationMatrix(mat);

    hkvVec3 vRight = -mat.getColumn( 1 );
    hkvVec3 vUp = mat.getColumn( 2 );

    hkvVec3 vForce = (vUp * vMult.y) + (vRight * vMult.x);
    vForce *= 100.0f / 60.0f;
    pBallEntity->ApplyForce(vForce, Vision::GetTimer()->GetTimeDifference());

    if(pGameStateEntity->GetCurrentState()==GAME_STATE_RESTART)
    {
      spGUIContext->SetActivate(true);
#ifdef _VISION_MOBILE
      //modal dialogs are not supported on iOS
      int iResult = VGUIManager::ID_YES;
#else
      int iResult = spGUIContext->ShowDialogModal(NULL, "GUI\\MessageBox.xml", hkvVec2(0,0), spGUIContext->GetNullApp());     
#endif
      if (iResult==VGUIManager::ID_YES) 
      {
        spGUIContext->SetActivate(false);
        pGameStateEntity->SetCurrentState(GAME_STATE_RUN);
        pBallEntity->SetPosition(BALL_START_POSITION);
      }
      else
      {
        return false;
      }
    }
#endif

#if defined(WIN32) && !defined(_VISION_WINRT)
    if(!VInputManagerPC::GetMultitouch().IsActive())
    {
      Vision::Game.GetDebugRenderInterface()->DrawText2D(10, 10, "No Windows 7 Touch Device is available", V_RGBA_WHITE);
    }
    else
    {
      if(VisSampleApp::GetInputMap()->GetTrigger(TOUCHDEBUG))
      {
        renderDebug = !renderDebug;
      }
      Vision::Game.GetDebugRenderInterface()->DrawText2D(10, 10, "Press Q to toggle touchpoint visual feedback", V_RGBA_WHITE);
    }
#elif defined(_VISION_PSP2)
    if(VisSampleApp::GetInputMap()->GetTrigger(TOUCHDEBUG))
    {
      renderDebug = !renderDebug;
    }
    //Vision::Game.GetDebugRenderInterface()->DrawText2D(10, 10, "Press Cross to toggle touchpoint visual feedback", V_RGBA_WHITE);
#endif

    // Touchpoint visual feedback (if enabled)
    if(renderDebug && (touchscreen != NULL))
    {        
        int count = touchscreen->GetMaximumNumberOfTouchPoints();
        for(int i = 0; i < count; i++)
        {
          if(!touchscreen->IsActiveTouch(i))
            continue;

          // Get touchpoint values and deltas
          valueX = touchscreen->GetTouchPointValue(i, CT_TOUCH_ABS_X);
          valueY = touchscreen->GetTouchPointValue(i, CT_TOUCH_ABS_Y);
          diffX = touchscreen->GetTouchPointValue(i, CT_TOUCH_ABS_DELTA_X);
          diffY = touchscreen->GetTouchPointValue(i, CT_TOUCH_ABS_DELTA_Y);

          // Draw crosshair
          Vision::Game.DrawSingleLine2D(valueX - OFFSET, valueY - OFFSET, valueX + OFFSET, valueY - OFFSET, V_RGBA_WHITE, 3.f);
          Vision::Game.DrawSingleLine2D(valueX - OFFSET, valueY + OFFSET, valueX + OFFSET, valueY + OFFSET, V_RGBA_WHITE, 3.f);
          Vision::Game.DrawSingleLine2D(valueX - OFFSET, valueY + OFFSET, valueX - OFFSET, valueY - OFFSET, V_RGBA_WHITE, 3.f);
          Vision::Game.DrawSingleLine2D(valueX + OFFSET, valueY + OFFSET, valueX + OFFSET, valueY - OFFSET, V_RGBA_WHITE, 3.f);
          Vision::Game.DrawSingleLine2D(valueX - OFFSET, valueY, valueX + OFFSET, valueY, V_RGBA_WHITE, 3.f);
          Vision::Game.DrawSingleLine2D(valueX, valueY + OFFSET, valueX, valueY - OFFSET, V_RGBA_WHITE, 3.f);

          // Draw delta vector
          Vision::Game.DrawSingleLine2D(valueX, valueY, valueX + diffX * 100.f, valueY + diffY * 100.f, V_RGBA_WHITE, 3.f);

          const int BufferSize = 30;
          char debugID[BufferSize];
          char debugX[BufferSize];
          char debugY[BufferSize];

          // Print ID & position for every active touchpoint          
          vis_snprintf(debugID, BufferSize, "ID: %i", i);
          Vision::Game.GetDebugRenderInterface()->DrawText2D(valueX + 30, valueY - 30, debugID, V_RGBA_WHITE);
          vis_snprintf(debugX, BufferSize, "X:  %.0f", valueX);
          Vision::Game.GetDebugRenderInterface()->DrawText2D(valueX + 30, valueY - 20, debugX, V_RGBA_WHITE);
          vis_snprintf(debugY, BufferSize, "Y:  %.0f", valueY);
          Vision::Game.GetDebugRenderInterface()->DrawText2D(valueX + 30, valueY - 10, debugY, V_RGBA_WHITE);
        }

    }
    return true;
  }
  return false;
}

VISION_DEINIT
{
  pGameStateEntity = NULL;
  pCameraEntity = NULL;
  pBallEntity = NULL;
  touchscreen = NULL;
#if defined(_VISION_PSP2) || defined(_VISION_IOS) || defined(_VISION_ANDROID) || defined(WIN32)
  pMotionInput = NULL;
#endif
#if defined(WIN32) && !defined(_VISION_WINRT)
  pRemoteInput = NULL;
#endif
  // cleanup GUI related stuff just before sample deinitialisation
  spGUIContext->SetActivate(false);
  spGUIContext = NULL;
  VGUIManager::GlobalManager().CleanupResources();

  // Deinit the application
  spApp->DeInitSample();
  spApp = NULL;
  return 0;
}

VISION_MAIN_DEFAULT

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
