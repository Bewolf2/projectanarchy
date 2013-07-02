/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// Camera_cl Source
//***********************************************************************

#include <Vision/Samples/Engine/Accelerometer/AccelerometerPCH.h>
#include <Vision/Samples/Engine/Accelerometer/Camera.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>

// InitFunction: Is called upon entity initialization.
void Camera_cl::InitFunction()
{
  m_pMultiTouchInput = NULL;
  m_spArrows = NULL;
#if defined(SUPPORTS_MULTITOUCH)
  m_pVirtualThumbStick = NULL;
#endif

#if !defined(_VISION_WIIU)
  m_pMultiTouchInput = static_cast<IVMultiTouchInput*>(&VInputManager::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN));
#endif

#if defined(_VISION_PSP2) || defined(_VISION_MOBILE) || defined(WIN32)

  // Enable multi touch input
  #if defined(_VISION_PSP2)
    VMultiTouchInputPSP2* pInput = static_cast<VMultiTouchInputPSP2*>(m_pMultiTouchInput);
    pInput->SetEnabled(true);

  #elif defined(WIN32)
    VMultiTouchInputPC* pInput = static_cast<VMultiTouchInputPC*>(m_pMultiTouchInput);
    pInput->SetEnabled(true);

  #endif

  #if defined(SUPPORTS_MULTITOUCH)
    m_pVirtualThumbStick = new VVirtualThumbStick();

    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_FORWARD,  *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.2f));
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_BACKWARD, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.2f));
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_LEFT,     *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.2f));
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_RIGHT,    *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.2f));

  #endif

#elif defined(_VISION_WIIU)
  // Use arrows to navigate
  m_spArrows = new VisScreenMask_cl("GUI/arrows.dds");
  m_spArrows->SetTransparency(VIS_TRANSP_ALPHA);

  m_pMultiTouchInput = &VInputManager::GetTouchScreen(V_DRC_FIRST);
  VTouchInputWiiU* pInput = static_cast<VTouchInputWiiU*>(m_pMultiTouchInput);
  pInput->SetEnabled(true);

  SetArrowControlPosition(0, Vision::Video.GetVideoConfig()->uiDRCRenderHeight - 120, 160);

#else
  #error Undefined platform!
#endif

#if defined(WIN32)
  VTouchArea* pCameraLookArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef(0,0,(float)Vision::Video.GetXRes(),(float)Vision::Video.GetYRes()), -2000.0f);
#else
  VTouchArea* pCameraLookArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef(), -2000.0f);
#endif

  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_HORIZONTAL_LOOK, pCameraLookArea, CT_TOUCH_ABS_DELTA_X);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_VERTICAL_LOOK,   pCameraLookArea, CT_TOUCH_ABS_DELTA_Y);

  m_fHalfScreenSizeX = (float)(Vision::Video.GetXRes()>>1);
  m_fHalfScreenSizeY = (float)(Vision::Video.GetYRes()>>1);

  // attach camera to entity
  Vision::Camera.AttachToEntity(this, hkvVec3(0, 0, 0));

  Vision::Callbacks.OnVideoChanged += this;
}

// DeInitFunction:: Is called upon entity de-initialization.
void Camera_cl::DeInitFunction()
{
  m_spArrows = NULL;
  m_pMultiTouchInput = NULL;
#if defined(SUPPORTS_MULTITOUCH)
  V_SAFE_DELETE(m_pVirtualThumbStick);
#endif

  Vision::Callbacks.OnVideoChanged -= this;
}

// ThinkFunction: Is called once per frame.
void Camera_cl::ThinkFunction()
{
  // look around
  float dx = VisSampleApp::GetInputMap()->GetTrigger(CAMERA_HORIZONTAL_LOOK);
  float dy = VisSampleApp::GetInputMap()->GetTrigger(CAMERA_VERTICAL_LOOK);

  hkvVec3 orient;
  GetOrientation(orient);
  orient.x += -dx * CAMERA_LOOK_SENSITIVITY;
  orient.y +=  dy * CAMERA_LOOK_SENSITIVITY;

  SetOrientation(orient);

  // check all movement keys
  hkvMat3 mat(hkvNoInitialization);
  GetRotationMatrix(mat);
  hkvVec3 vDir = mat.getAxis(2);
  hkvVec3 vRight = mat.getAxis(1);

  const float fMaxSpeed = CAMERA_MOVE_SPEED;

  // Accumulate move directions (multiply in order to take analog input into account).
  hkvVec3 vMove = hkvVec3::ZeroVector();
  vMove += vDir * VisSampleApp::GetInputMap()->GetTrigger(CAMERA_MOVE_FORWARD);
  vMove -= vDir * VisSampleApp::GetInputMap()->GetTrigger(CAMERA_MOVE_BACKWARD);
  vMove += vRight * VisSampleApp::GetInputMap()->GetTrigger(CAMERA_MOVE_LEFT);
  vMove -= vRight * VisSampleApp::GetInputMap()->GetTrigger(CAMERA_MOVE_RIGHT);
  vMove *= fMaxSpeed;

  // Clamp movement, so that moving diagonally is not faster than moving straight 
  // when using digital input.
  const float fSpeed = vMove.getLength();
  if (fSpeed > fMaxSpeed)
    vMove.setLength(fMaxSpeed);
  vMove *= Vision::GetTimer()->GetTimeDifference();

  IncPosition(vMove);
}

void Camera_cl::SetArrowControlPosition(int x, int y, int areaSize)
{
#if defined(_VISION_WIIU)
  float fXScale = ((float)Vision::Video.GetXRes() / (float)Vision::Video.GetVideoConfig()->uiDRCRenderWidth);
  float fYScale = ((float)Vision::Video.GetYRes() / (float)Vision::Video.GetVideoConfig()->uiDRCRenderHeight);
  m_spArrows->SetPos((float)x * fXScale, (float)y * fYScale);
  m_spArrows->SetTargetSize((float)areaSize * fXScale, (float)areaSize * fXScale);
#else
  m_spArrows->SetPos((float)x, (float)y);
  m_spArrows->SetTargetSize((float)areaSize, (float)areaSize);
#endif

  int btnSize = areaSize>>2;
 
  VisSampleApp::GetInputMap()->UnmapInput(CAMERA_MOVE_FORWARD);
  VisSampleApp::GetInputMap()->UnmapInput(CAMERA_MOVE_BACKWARD);
  VisSampleApp::GetInputMap()->UnmapInput(CAMERA_MOVE_LEFT);
  VisSampleApp::GetInputMap()->UnmapInput(CAMERA_MOVE_RIGHT);

  VTouchArea* pForwardArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef((float)(x + btnSize), (float)y, (float)(x + 2*btnSize), (float)(y + btnSize)), -900.0f);
  VTouchArea* pBackwardArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef((float)(x + btnSize), (float)(y + 2*btnSize), (float)(x + 2*btnSize), (float)(y + 3*btnSize)), -900.0f);
  VTouchArea* pLeftArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef((float)x, (float)(y + btnSize), (float)(x + btnSize), (float)(y + 2*btnSize)), -900.0f);
  VTouchArea* pRightArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef((float)(x + 2*btnSize), (float)(y + btnSize), (float)(x + 3*btnSize), (float)(y + 2*btnSize)), -900.0f);

  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_FORWARD, pForwardArea, CT_TOUCH_ANY);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_BACKWARD, pBackwardArea, CT_TOUCH_ANY);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_LEFT, pLeftArea, CT_TOUCH_ANY);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_RIGHT, pRightArea, CT_TOUCH_ANY);
}

void Camera_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
    if (m_spArrows != NULL)
      SetArrowControlPosition(15, Vision::Video.GetYRes() - 214, 256);
#if defined(SUPPORTS_MULTITOUCH)
    // Reposition thumb stick area
    if (m_pVirtualThumbStick != NULL)
      m_pVirtualThumbStick->SetValidArea(VRectanglef());
#endif
  }
}

V_IMPLEMENT_SERIAL( Camera_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(Camera_cl, VisBaseEntity_cl, "Camera_cl", 0, "")
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
