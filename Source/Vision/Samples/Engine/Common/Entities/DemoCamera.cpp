/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/DemoCamera.hpp>

#ifdef _VISION_IOS
  #include <Vision/Runtime/Base/Graphics/Video/IOSBridge.h>
#endif

#ifdef _VISION_IOS
#define MOUSE_SENSITIVITY 0.02f
#else
#define MOUSE_SENSITIVITY 0.5f
#endif
#define MOUSEWHEEL_STEPSIZE 10.f
#define CAMERA_MINANGLE -15.0f
#define CAMERA_MAXANGLE 50.0f

#define THUMBSTICK_SENSITIVITY 0.012f
#define THUMBSTICK_DEADZONE 8000.0f

VisDemoCamera_cl::VisDemoCamera_cl(float fModelHeight, float fMinDistance, float fMaxDistance, float fSpeed)
{
  m_pTargetEntity = NULL;
  m_fDistance = 100.0f;
  m_fModelHeight = fModelHeight;
  m_fMinDistance = fMinDistance;
  m_fMaxDistance = fMaxDistance;
  m_fCameraSpeedFactor = fSpeed;
  m_bCollides = true;
  m_bFixed = false;
}

VisDemoCamera_cl::~VisDemoCamera_cl()
{
  V_SAFE_DELETE(m_pInputMap);
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
  
  m_pInputMap = new VInputMap(CAMERA_DEMO_CONTROL_LAST_ELEMENT+1+CAMERA_DEMO_CONTROL_USER_SPACE, CAMERA_DEMO_CONTROL_ALTERNATIVES);

#ifdef SUPPORTS_KEYBOARD
  m_pInputMap->MapTrigger(CAMERA_DEMO_MOVE_UP,    V_KEYBOARD, CT_KB_U);
  m_pInputMap->MapTrigger(CAMERA_DEMO_MOVE_DOWN,  V_KEYBOARD, CT_KB_J);
  m_pInputMap->MapTrigger(CAMERA_DEMO_MOVE_LEFT,  V_KEYBOARD, CT_KB_H);
  m_pInputMap->MapTrigger(CAMERA_DEMO_MOVE_RIGHT, V_KEYBOARD, CT_KB_K);
  
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_OUT,   V_KEYBOARD, CT_KB_O);
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_IN,    V_KEYBOARD, CT_KB_L);
  
#endif

#ifdef SUPPORTS_MOUSE
  m_pInputMap->MapTrigger(CAMERA_DEMO_DATA_CHANGED,   V_MOUSE, CT_MOUSE_DATA_CHANGED);
  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_ZOOM,       V_MOUSE, CT_MOUSE_WHEEL_DOWN,CT_MOUSE_WHEEL_UP);
  m_pInputMap->MapTrigger(CAMERA_DEMO_HORIZONTAL, V_MOUSE, CT_MOUSE_NORM_DELTA_X, VInputOptions::Sensitivity(2.0f));
  m_pInputMap->MapTrigger(CAMERA_DEMO_VERTICAL,   V_MOUSE, CT_MOUSE_NORM_DELTA_Y, VInputOptions::Sensitivity(2.0f));
#endif

#if defined(WIN32) && !(defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))

  // Windows specific input mappings here

#elif defined(_VISION_XENON) || defined(_VISION_WINRT) 
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_OUT, V_XENON_PAD(0), CT_PAD_DOWN);
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_IN,  V_XENON_PAD(0), CT_PAD_UP);
  
  m_pInputMap->MapTrigger(CAMERA_DEMO_DATA_CHANGED, V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_HORIZONTAL, V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.2f, true));
  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_VERTICAL,   V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.2f, true));

#elif defined(_VISION_PS3)
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_OUT, V_PS3_PAD(0), CT_PAD_DOWN);
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_IN,  V_PS3_PAD(0), CT_PAD_UP);

  m_pInputMap->MapTrigger(CAMERA_DEMO_DATA_CHANGED, V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_HORIZONTAL, V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.2f, true));
  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_VERTICAL,   V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.2f, true));

#elif defined(_VISION_PSP2)
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_OUT, V_PSP2_PAD(0), CT_PAD_DOWN);
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_IN,  V_PSP2_PAD(0), CT_PAD_UP);

  m_pInputMap->MapTrigger(CAMERA_DEMO_DATA_CHANGED, V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_HORIZONTAL, V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.2f, true));
  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_VERTICAL,   V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.2f, true));
  
#elif defined(_VISION_MOBILE)

  VTouchArea* pLookArea = new VTouchArea(VInputManager::GetTouchScreen());
  m_pInputMap->MapTrigger(CAMERA_DEMO_DATA_CHANGED, pLookArea, CT_TOUCH_ANY);
  m_pInputMap->MapTrigger(CAMERA_DEMO_HORIZONTAL,   pLookArea, CT_TOUCH_DELTA_X, VInputOptions::TimeScale());
  m_pInputMap->MapTrigger(CAMERA_DEMO_VERTICAL,     pLookArea, CT_TOUCH_DELTA_Y, VInputOptions::TimeScale());

#elif defined(_VISION_WIIU)
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_OUT, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_DOWN);
  m_pInputMap->MapTrigger(CAMERA_DEMO_ZOOM_IN,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_UP);

  m_pInputMap->MapTrigger(CAMERA_DEMO_DATA_CHANGED,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_HORIZONTAL, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.2f, true));
  m_pInputMap->MapTriggerAxis(CAMERA_DEMO_VERTICAL,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_UP, CT_PAD_RIGHT_THUMB_STICK_DOWN,    VInputOptions::DeadZone(0.2f, true));

#else
  #error Undefinded platform!
#endif

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


void VisDemoCamera_cl::SetDistance(float fDistance)
{
  hkvMath::clamp( fDistance, m_fMinDistance, m_fMaxDistance );
  m_fDistance = fDistance;
}

float VisDemoCamera_cl::GetDistance() const
{
  return m_fDistance;
}

void VisDemoCamera_cl::SetMinDistance(float fDistance)
{
  m_fMinDistance = fDistance;
  if ( m_fDistance < fDistance )
    m_fDistance = fDistance;
}

float VisDemoCamera_cl::GetMinDistance() const
{
  return m_fMinDistance;
}

void VisDemoCamera_cl::SetMaxDistance(float fDistance)
{
  m_fMaxDistance = fDistance;
  if ( m_fDistance > fDistance )
    m_fDistance = fDistance;
}

float VisDemoCamera_cl::GetMaxDistance() const
{
  return m_fMaxDistance;
}


void VisDemoCamera_cl::HandleInput()
{
  float fTimeDiff = Vision::GetTimer()->GetTimeDifference() * m_fCameraSpeedFactor;
  float dx = m_pInputMap->GetTrigger(CAMERA_DEMO_HORIZONTAL) * m_fHalfScreenSizeX;
  float dy = m_pInputMap->GetTrigger(CAMERA_DEMO_VERTICAL) * m_fHalfScreenSizeY;
  float zoom = m_pInputMap->GetTrigger(CAMERA_DEMO_ZOOM);

  if (m_pInputMap->GetTrigger(CAMERA_DEMO_MOVE_LEFT))
    this->IncOrientation(-100.0f*fTimeDiff, 0.0f, 0.0f);
  if (m_pInputMap->GetTrigger(CAMERA_DEMO_MOVE_RIGHT))
    this->IncOrientation(100.0f*fTimeDiff, 0.0f, 0.0f);
  if (m_pInputMap->GetTrigger(CAMERA_DEMO_MOVE_UP))
    this->IncOrientation(0.0f, -100.0f*fTimeDiff, 0.0f);
  if (m_pInputMap->GetTrigger(CAMERA_DEMO_MOVE_DOWN))
    this->IncOrientation(0.0f, 100.0f*fTimeDiff, 0.0f);
  if (m_pInputMap->GetTrigger(CAMERA_DEMO_ZOOM_IN))
    m_fDistance -= 200.0f * fTimeDiff;
  if (m_pInputMap->GetTrigger(CAMERA_DEMO_ZOOM_OUT))
    m_fDistance += 200.0f*fTimeDiff;

  
  #if defined(_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  zoom *= 20;
  #endif
  

  #if defined(WIN32) && (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
    zoom *= MOUSE_WHEEL_SCALE_FACTOR;
  #endif

  if (m_pInputMap->GetTrigger(CAMERA_DEMO_DATA_CHANGED))
  {
    IncOrientation(dx * MOUSE_SENSITIVITY, 0.f, 0.f);
    IncOrientation(0.f, dy * MOUSE_SENSITIVITY, 0.f);
  }

  m_fDistance -= zoom * MOUSEWHEEL_STEPSIZE;

  hkvVec3 vOrientation = GetOrientation();
  if (vOrientation.y < CAMERA_MINANGLE)
    vOrientation.y = CAMERA_MINANGLE;
  if (vOrientation.y > CAMERA_MAXANGLE)
    vOrientation.y = CAMERA_MAXANGLE;
  SetOrientation(vOrientation);

  if (m_fDistance < m_fMinDistance)
    m_fDistance = m_fMinDistance;
  if (m_fDistance > m_fMaxDistance)
    m_fDistance = m_fMaxDistance;
}

void VisDemoCamera_cl::ThinkFunction()
{
  if (!m_bFixed)
    HandleInput();

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
    VisPhysicsHit_t hitInfo;
    hkvVec3 vDirOfSight = vNewCamPos - vTargetLookAtPos;
    vDirOfSight.normalizeIfNotZero();
    vDirOfSight *= 15.0f;
    if (Vision::GetApplication()->GetPhysicsModule()->Raycast(vTargetLookAtPos, vNewCamPos + vDirOfSight, hitInfo))
    {
      bool bTouchedEntity = (hitInfo.eHitType == VIS_TRACETYPE_ENTITYBOX || hitInfo.eHitType == VIS_TRACETYPE_ENTITYPOLY);
      if (!bTouchedEntity)
        vNewCamPos = hitInfo.vImpactPoint - vDirOfSight;
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
