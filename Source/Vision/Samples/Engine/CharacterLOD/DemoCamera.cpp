/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/CharacterLOD/CharacterLODPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VZoomDetector.hpp>
#include <Vision/Samples/Engine/CharacterLOD/DemoCamera.hpp>

#ifdef _VISION_IOS
  #include <Vision/Runtime/Base/Graphics/Video/IOSBridge.h>
#endif

#define MOUSE_SENSITIVITY 2.0f
#define MOUSEWHEEL_STEPSIZE 175.f

#define CAMERA_MINANGLE -15.0f
#define CAMERA_MAXANGLE 50.0f

VisDemoCamera_cl::VisDemoCamera_cl(float fModelHeight, float fMinDistance, float fMaxDistance, float fSpeed)
{
  m_pTargetEntity = NULL;
  m_fDistance = 200.0f;
  m_fNewDistance = 650.0f;
  m_fModelHeight = fModelHeight;
  m_fMinDistance = fMinDistance;
  m_fMaxDistance = fMaxDistance;
  m_fCameraSpeedFactor = fSpeed;
  m_bCollides = true;
  m_bFixed = false;

#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
  #ifdef SUPPORTS_KEYBOARD
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_UP,    V_PC_KEYBOARD, CT_KB_UP);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_DOWN,  V_PC_KEYBOARD, CT_KB_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_LEFT,  V_PC_KEYBOARD, CT_KB_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_RIGHT, V_PC_KEYBOARD, CT_KB_RIGHT);
  
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_OUT,   V_PC_KEYBOARD, CT_KB_PGDN);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_IN,    V_PC_KEYBOARD, CT_KB_PGUP);
  #endif
  #ifdef SUPPORTS_MOUSE
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_DATA_CHANGED, V_PC_MOUSE, CT_MOUSE_DATA_CHANGED);

  VisSampleApp::GetInputMap()->MapTriggerAxis(CAMERA_ZOOM, V_PC_MOUSE, CT_MOUSE_WHEEL_DOWN, CT_MOUSE_WHEEL_UP);

  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_INVOKE_MOVEMENT, V_PC_MOUSE, CT_MOUSE_RIGHT_BUTTON);
  
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_HORIZONTAL_MOVEMENT, V_PC_MOUSE, CT_MOUSE_NORM_DELTA_X);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_VERTICAL_MOVEMENT,   V_PC_MOUSE, CT_MOUSE_NORM_DELTA_Y);
  #endif
#elif defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_UP,    V_XENON_PAD(0), CT_PAD_UP);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_DOWN,  V_XENON_PAD(0), CT_PAD_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_LEFT,  V_XENON_PAD(0), CT_PAD_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_RIGHT, V_XENON_PAD(0), CT_PAD_RIGHT);

  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_OUT,   V_XENON_PAD(0), CT_PAD_LEFT_TRIGGER);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_IN,    V_XENON_PAD(0), CT_PAD_RIGHT_TRIGGER);
  #ifdef SUPPORTS_KEYBOARD
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_UP,    VInputManagerXenon::GetKeyboard(), CT_KB_KP_UP);
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_DOWN,  VInputManagerXenon::GetKeyboard(), CT_KB_KP_DOWN);
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_LEFT,  VInputManagerXenon::GetKeyboard(), CT_KB_KP_LEFT);
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_RIGHT, VInputManagerXenon::GetKeyboard(), CT_KB_KP_RIGHT);
    
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_OUT,   VInputManagerXenon::GetKeyboard(), CT_KB_PGDN);
    VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_IN,    VInputManagerXenon::GetKeyboard(), CT_KB_PGUP);
  
  #endif
#elif defined (_VISION_PS3)
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_UP,    V_PS3_PAD(0), CT_PAD_UP);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_DOWN,  V_PS3_PAD(0), CT_PAD_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_LEFT,  V_PS3_PAD(0), CT_PAD_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_RIGHT, V_PS3_PAD(0), CT_PAD_RIGHT);

  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_OUT,   V_PS3_PAD(0), CT_PAD_LEFT_TRIGGER);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_IN,    V_PS3_PAD(0), CT_PAD_RIGHT_TRIGGER);
#elif defined (_VISION_WIIU)
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_UP,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_UP);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_DOWN,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_DOWN);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_LEFT,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_MOVE_RIGHT, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT);

  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_OUT,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_TRIGGER);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_IN,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_TRIGGER);
#endif

#if defined(SUPPORTS_MULTITOUCH) && !defined(WIN32)
  
  VTouchArea* pLookArea = new VTouchArea(VInputManager::GetTouchScreen());

  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_INVOKE_MOVEMENT,     pLookArea, CT_TOUCH_ANY);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_HORIZONTAL_MOVEMENT, pLookArea, CT_TOUCH_NORM_DELTA_X);
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_VERTICAL_MOVEMENT,   pLookArea, CT_TOUCH_NORM_DELTA_Y);
  
  m_pZoomDetector = new VZoomDetector();  
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_DATA_CHANGED_TOUCH, *m_pZoomDetector, CT_TOUCH_ZOOM);  
  VisSampleApp::GetInputMap()->MapTrigger(CAMERA_ZOOM_TOUCH, *m_pZoomDetector, CT_TOUCH_ZOOM);
  
#endif
}

VisDemoCamera_cl::~VisDemoCamera_cl()
{
#if defined(SUPPORTS_MULTITOUCH) && !defined(WIN32)
  delete m_pZoomDetector;
#endif
}

void VisDemoCamera_cl::SetCollide(bool bColl)
{
  m_bCollides = bColl;
}

void VisDemoCamera_cl::InitFunction()
{
  hkvVec3 tmpvector(0, 0, 0);
  
  m_fHalfScreenSizeX = (float)(Vision::Video.GetXRes()>>1);
  m_fHalfScreenSizeY = (float)(Vision::Video.GetYRes()>>1);

#if defined( _VISION_MOBILE )
  // as screen resolution is incorporated twice into camera movement
  // we divide our screen factor by the square of the content scale
  // factor (which is 2 if we are running on a retina display and
  // 1 otherwise)
  float fContentScaleFactorSquared = hkvMath::Max( 1.0f, hkvMath::floor( Vision::Video.GetVideoConfig()->DisplayDensity ) );
  fContentScaleFactorSquared *= fContentScaleFactorSquared;
  m_fHalfScreenSizeX /= fContentScaleFactorSquared;
  m_fHalfScreenSizeY /= fContentScaleFactorSquared;
#endif  // defined( _VISION_MOBILE )
  }

void VisDemoCamera_cl::SetTargetEntity(VisBaseEntity_cl *pEntity)
{
  m_pTargetEntity = pEntity;
}


void VisDemoCamera_cl::SetFixed(bool bFixed)
{
  m_bFixed = bFixed;
}

bool VisDemoCamera_cl::GetFixed()
{
  return m_bFixed;
}


void VisDemoCamera_cl::HandleInput()
{
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference() * m_fCameraSpeedFactor;
  
#if defined(SUPPORTS_MULTITOUCH) && !defined(WIN32)
  m_pZoomDetector->Update();
#endif

  float dx = VisSampleApp::GetInputMap()->GetTrigger(CAMERA_HORIZONTAL_MOVEMENT) * m_fHalfScreenSizeX;
  float dy = VisSampleApp::GetInputMap()->GetTrigger(CAMERA_VERTICAL_MOVEMENT) * m_fHalfScreenSizeY;

  float zoom = VisSampleApp::GetInputMap()->GetTrigger(CAMERA_ZOOM);
  
#if defined(SUPPORTS_MULTITOUCH) && !defined(WIN32)
  float touchZoom = VisSampleApp::GetInputMap()->GetTrigger(CAMERA_ZOOM_TOUCH) * 20.f;
  if (static_cast<IVMultiTouchInput&>(VInputManager::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN)).GetNumberOfTouchPoints() > 1)
  {
    dx = dy = 0.0f;
  }
#endif

  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_MOVE_LEFT))
    this->IncOrientation(-100.0f*fTimeDiff, 0.0f, 0.0f);
  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_MOVE_RIGHT))
    this->IncOrientation(100.0f*fTimeDiff, 0.0f, 0.0f);
  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_MOVE_UP))
    this->IncOrientation(0.0f, -100.0f*fTimeDiff, 0.0f);
  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_MOVE_DOWN))
    this->IncOrientation(0.0f, 100.0f*fTimeDiff, 0.0f);
  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_ZOOM_IN))
    m_fNewDistance -= 1250.0f * fTimeDiff;
  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_ZOOM_OUT))
    m_fNewDistance += 1250.0f*fTimeDiff;

  #if defined(WIN32) &&  (!defined(_VISION_WINRT) || defined(_VISION_METRO))
    zoom *= MOUSE_WHEEL_SCALE_FACTOR;
  #endif

  #if defined(_VISION_XENON) || defined(_VISION_PS3) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
    zoom *= 20;
  #endif


  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_INVOKE_MOVEMENT)) 
  {
    IncOrientation(dx * MOUSE_SENSITIVITY, 0.f, 0.f);
    IncOrientation(0.f, dy * MOUSE_SENSITIVITY, 0.f);
  }

  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_DATA_CHANGED))
  {
    m_fNewDistance -= zoom * MOUSEWHEEL_STEPSIZE;
  }

#if defined(SUPPORTS_MULTITOUCH) && !defined(WIN32)
  if (VisSampleApp::GetInputMap()->GetTrigger(CAMERA_DATA_CHANGED_TOUCH))
  {
    m_fNewDistance -= touchZoom * MOUSEWHEEL_STEPSIZE;
  }
#endif

  hkvVec3 vOrientation = GetOrientation();
  if (vOrientation.y < CAMERA_MINANGLE)
    vOrientation.y = CAMERA_MINANGLE;
  if (vOrientation.y > CAMERA_MAXANGLE)
    vOrientation.y = CAMERA_MAXANGLE;
  
  SetOrientation(vOrientation);

  if (m_fNewDistance < m_fMinDistance)
    m_fNewDistance = m_fMinDistance;
  if (m_fNewDistance > m_fMaxDistance)
    m_fNewDistance = m_fMaxDistance;
}

void VisDemoCamera_cl::ThinkFunction()
{
  if (!m_bFixed)
    HandleInput();

  float fTimeDiff = Vision::GetTimer()->GetTimeDifference() * m_fCameraSpeedFactor;

  //smooth zoom
  if((m_fNewDistance < m_fDistance && m_fNewDistance > m_fDistance-15.f) || 
     (m_fNewDistance > m_fDistance && m_fNewDistance < m_fDistance+15.f) )
  {
    m_fDistance = m_fNewDistance;
  } 
  else 
  {
    if (m_fNewDistance < m_fDistance)
      m_fDistance -= 1500.f * fTimeDiff;
    if (m_fNewDistance > m_fDistance)
      m_fDistance += 1500.f * fTimeDiff;
  }

  SetPosition(m_pTargetEntity->GetPosition());

  hkvVec3 vRotPos;
  hkvVec3 vTargetEntityPos;
  hkvVec3 vTargetLookAtPos;

  if (m_bFixed)
  {
    hkvMat3 mRotation = m_pTargetEntity->GetRotationMatrix();
    hkvVec3 vRelPos(-1.0f, 0.0f, 0.15f);
    vRotPos = (mRotation * vRelPos) * m_fDistance;
    vRotPos.z += m_fModelHeight * 1.5f;
    vTargetEntityPos = m_pTargetEntity->GetPosition();
    vTargetLookAtPos = vTargetEntityPos;
    vTargetLookAtPos.z += m_fModelHeight * 1.5f;
  }
  else
  {
    hkvMat3 mRotation = GetRotationMatrix();
    hkvVec3 vRelPos(1.0f, 0.0f, 1.0f);
    vRotPos = (mRotation * vRelPos) * m_fDistance;
    vRotPos.z += m_fModelHeight;
    vTargetEntityPos = m_pTargetEntity->GetPosition();
    vTargetLookAtPos = vTargetEntityPos;
    vTargetLookAtPos.z += m_fModelHeight;
  }


  hkvMat3 mLookAtMatrix (hkvNoInitialization);
  mLookAtMatrix.setLookInDirectionMatrix (vTargetLookAtPos - (vRotPos + vTargetEntityPos));

  hkvVec3 vNewCamPos = vRotPos + vTargetEntityPos;

  if (m_bCollides)
  {
    VisTraceLineInfo_t traceLineInfo;
    hkvVec3 vDirOfSight = vNewCamPos - vTargetLookAtPos;
    vDirOfSight.normalizeIfNotZero();
    vDirOfSight *= 15.0f;
    BOOL bCanSee = Vision::CollisionToolkit.TraceLine(vTargetLookAtPos, vNewCamPos + vDirOfSight, (ULONG)VIS_TRACE_STOREPRIMITIVES, VIS_TRACE_NONE, NULL, NULL, &traceLineInfo);
    if (!bCanSee)
    {
      vNewCamPos = traceLineInfo.touchPoint - vDirOfSight;
    }
  }

  Vision::Camera.Set(mLookAtMatrix, vNewCamPos);
}



V_IMPLEMENT_SERIAL( VisDemoCamera_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(VisDemoCamera_cl, VisBaseEntity_cl, "VisDemoCamera_cl", 0, "")  
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
