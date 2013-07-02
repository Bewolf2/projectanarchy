/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file DemoCamera.hpp

#ifndef DEMOCAMERA_HPP_INCLUDED
#define DEMOCAMERA_HPP_INCLUDED

#ifndef _VISION_DOC

enum CAMERA_DEMO_CONTROL {
  CAMERA_DEMO_MOVE_UP = 0,
  CAMERA_DEMO_MOVE_DOWN,
  CAMERA_DEMO_MOVE_LEFT,
  CAMERA_DEMO_MOVE_RIGHT,

  CAMERA_DEMO_ZOOM_OUT,
  CAMERA_DEMO_ZOOM_IN,

  CAMERA_DEMO_DATA_CHANGED,
  CAMERA_DEMO_ZOOM,
  CAMERA_DEMO_HORIZONTAL,
  CAMERA_DEMO_VERTICAL,

  CAMERA_DEMO_CONTROL_LAST_ELEMENT = CAMERA_DEMO_VERTICAL
};

#define CAMERA_DEMO_CONTROL_ALTERNATIVES 12
#define CAMERA_DEMO_CONTROL_USER_SPACE 8

class VisDemoCamera_cl : public VisBaseEntity_cl
{
public:
  VisDemoCamera_cl(float fModelHeight = 120.f, float fMinDistance = 50.f, float fMaxDistance = 1000.f, float fSpeed = 1.0f);
  ~VisDemoCamera_cl();

  void SetTargetEntity(VisBaseEntity_cl *pEntity);
  void SetCollide(bool bColl);
  bool GetFixed();
  void SetFixed(bool bFixed);

  void SetDistance(float fDistance);
  float GetDistance() const;
  void SetMinDistance(float fDistance);
  float GetMinDistance() const;
  void SetMaxDistance(float fDistance);
  float GetMaxDistance() const;

  VISION_APIFUNC virtual void InitFunction();
  VISION_APIFUNC virtual void ThinkFunction();

private:
  void HandleInput();

  VisBaseEntity_cl *m_pTargetEntity;
  float m_fDistance;
  float m_fModelHeight;
  float m_fMinDistance;
  float m_fMaxDistance;
  float m_fCameraSpeedFactor;
  bool m_bCollides;
  bool m_bFixed;

  VInputMap* m_pInputMap;
  float m_fHalfScreenSizeX;
  float m_fHalfScreenSizeY;

public:
  V_DECLARE_SERIAL( VisDemoCamera_cl, )
  IMPLEMENT_OBJ_CLASS(VisDemoCamera_cl);

};

#endif // _VISION_DOC

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
