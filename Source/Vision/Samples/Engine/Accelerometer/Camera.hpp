/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// Camera_cl Class
//***********************************************************************

#ifndef _Camera_cl_HEADER_
#define _Camera_cl_HEADER_

class VVirtualThumbStick;

////////////////////////////////////////////////////////////////////////////////
// defines

// movement keys
enum CAMERA_CONTROL 
{
  CAMERA_MOVE_FORWARD = VISION_INPUT_CONTROL_LAST_ELEMENT+1,
  CAMERA_MOVE_BACKWARD,
  CAMERA_MOVE_LEFT,
  CAMERA_MOVE_RIGHT,
  CAMERA_HORIZONTAL_LOOK,
  CAMERA_VERTICAL_LOOK
};

// speed things
#define CAMERA_MOVE_SPEED         200.0f
#define CAMERA_LOOK_SENSITIVITY   0.3f

//
// *** CameraEntity_cl  ***
//
// Synopsis:
//    - Entity class that holds the camera 
//
class Camera_cl : public VisBaseEntity_cl, IVisCallbackHandler_cl
{
public:
  // is called upon entity initialization
  virtual void InitFunction() HKV_OVERRIDE;

  // is called upon entity de-initialization
  virtual void DeInitFunction() HKV_OVERRIDE;

  // is called each frame
  virtual void ThinkFunction() HKV_OVERRIDE;

  // place camera control-buttons on screen
  void SetArrowControlPosition(int x, int y, int areaSize);

  // callback handler implementation
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

private:
  // private variables
  float m_fHalfScreenSizeX;			
  float m_fHalfScreenSizeY;	

  IVMultiTouchInput *m_pMultiTouchInput;
  VisScreenMaskPtr m_spArrows;

#if defined(SUPPORTS_MULTITOUCH)
  VVirtualThumbStick* m_pVirtualThumbStick;
#endif

  V_DECLARE_SERIAL(Camera_cl,)
  IMPLEMENT_OBJ_CLASS(Camera_cl);
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
