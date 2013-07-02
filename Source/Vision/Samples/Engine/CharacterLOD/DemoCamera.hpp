/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef DEMOCAMERA_HPP_INCLUDED
#define DEMOCAMERA_HPP_INCLUDED

enum CAMERA_CONTROL {
  CAMERA_MOVE_UP = VISION_INPUT_CONTROL_LAST_ELEMENT+1,

  CAMERA_MOVE_DOWN,
  CAMERA_MOVE_LEFT,
  CAMERA_MOVE_RIGHT,

  CAMERA_ZOOM_OUT,
  CAMERA_ZOOM_IN,

  CAMERA_DATA_CHANGED,
  CAMERA_ZOOM,

  CAMERA_DATA_CHANGED_TOUCH,
  CAMERA_ZOOM_TOUCH,

  CAMERA_HORIZONTAL_MOVEMENT,
  CAMERA_VERTICAL_MOVEMENT,

  CAMERA_INVOKE_MOVEMENT /*,
  TOGGLE_SHADOW */

};

class VZoomDetector;

//
// *** VisDemoCamera_cl ***
// 
// Synopsis:
//  - Simple third-person demo camera with fixed or variable view point relative to a target entity's
//    position
//  - Optionally, TraceLines are used to make sure that the camera can always "see" the target entity
//
class VisDemoCamera_cl : public VisBaseEntity_cl
{
public:
  VisDemoCamera_cl(float fModelHeight = 50.f, float fMinDistance = 50.f, float fMaxDistance = 3500.f, float fSpeed = 1.0f);
  ~VisDemoCamera_cl();

  void SetTargetEntity(VisBaseEntity_cl *pEntity);
  void SetCollide(bool bColl);
  bool GetFixed();
  void SetFixed(bool bFixed);

  VISION_APIFUNC virtual void InitFunction();
  VISION_APIFUNC virtual void ThinkFunction();

protected:
  void HandleInput();

  VisBaseEntity_cl *m_pTargetEntity;
  float m_fDistance;
  float m_fModelHeight;
  float m_fMinDistance;
  float m_fMaxDistance;
  float m_fCameraSpeedFactor;
  bool m_bCollides;
  bool m_bFixed;
  float m_fNewDistance;
  
  float m_fHalfScreenSizeX;
  float m_fHalfScreenSizeY;
  
#ifdef SUPPORTS_MULTITOUCH
  VZoomDetector* m_pZoomDetector;
#endif

public:
  V_DECLARE_SERIAL( VisDemoCamera_cl, )
  IMPLEMENT_OBJ_CLASS(VisDemoCamera_cl);

};

#endif

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
