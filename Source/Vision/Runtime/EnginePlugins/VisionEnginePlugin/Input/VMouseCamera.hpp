/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMouseCamera.hpp

//***********************************************************************
// MouseCamera_cl Class
//***********************************************************************

#ifndef _MouseCamera_cl_HEADER_
#define _MouseCamera_cl_HEADER_

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
class VVirtualThumbStick;

enum API_CAMERA_CONTROL 
{
  API_CAMERA_MOVE_FORWARD = 0,
  API_CAMERA_MOVE_BACKWARD,
  API_CAMERA_MOVE_LEFT,
  API_CAMERA_MOVE_RIGHT,
  API_CAMERA_MOVE_UP,
  API_CAMERA_MOVE_DOWN,

  API_CAMERA_ANY_ACTION,

  API_CAMERA_ACTION_1,
  API_CAMERA_ACTION_2,
  API_CAMERA_ACTION_3,
  
  API_CAMERA_LOOK_CHANGED,

  API_CAMERA_HORIZONTAL_LOOK,
  API_CAMERA_VERTICAL_LOOK,
  
  // for touch screen devices
  API_CAMERA_DOUBLE_TAP,
  API_CAMERA_TAP_X,
  API_CAMERA_TAP_Y,

  API_CAMERA_CONTROL_LAST_ELEMENT = API_CAMERA_VERTICAL_LOOK
};

#define API_CAMERA_CONTROL_ALTERNATIVES 20   ///< Used for Wii because the THUMB_CHANGED gets only triggered on deltas, not if the stick remains in the same postion
#define API_CAMERA_CONTROL_USER_SPACE 16

enum VisMouseCamWalkMode_e
{
  NONE = 0,
  WALK
};

/// \brief
///   Entity class that attaches the main camera and implements movement via mouse and keyboard
///   input. The camera can be controlled via the cursor keys (move) and mouse (rotate). Holding
///   down a mouse button while moving increases the speed. Optionally this class supports gravity
///   and collision with static geometry. This camera is created in the
///   VisSampleApp::EnableMouseCamera function
class VisMouseCamera_cl : public VisBaseEntity_cl, public IVisCallbackHandler_cl
{
  V_DECLARE_SERIAL_DLLEXP( VisMouseCamera_cl, EFFECTS_IMPEXP )

public:
  EFFECTS_IMPEXP VisMouseCamera_cl();
  EFFECTS_IMPEXP ~VisMouseCamera_cl();
  
  // Will be called during the initialisation of each entity instance
  EFFECTS_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  
  // Will be called by the engine once per tick/frame
  EFFECTS_IMPEXP virtual void ThinkFunction() HKV_OVERRIDE;
  
  // serialize this entitiy
  EFFECTS_IMPEXP virtual void Serialize(VArchive& ar) HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  // 
  EFFECTS_IMPEXP void SetWalkMode( VisMouseCamWalkMode_e mode );
  EFFECTS_IMPEXP void SetWalkMode( BOOL walk );

  EFFECTS_IMPEXP void SetSimulationIndependentMovement(bool bStatus);
  inline bool GetSimulationIndependentMovement() const 
  {
    return m_bSimulationIndependent;
  }

  //
  inline VisMouseCamWalkMode_e GetWalkMode() const 
  { 
    return m_walkMode; 
  }

  EFFECTS_IMPEXP VInputMap* GetInputMap();

  EFFECTS_IMPEXP void TickFunction(float fTimeDiff);
  IMPLEMENT_OBJ_CLASS(VisMouseCamera_cl);

#if defined(SUPPORTS_MULTITOUCH)
  /// \brief
  ///   Returns the virtual thumb stick instance (if created).
  inline VVirtualThumbStick* GetVirtualThumbStick()
  {
    return m_pVirtualThumbStick;
  }
#endif

  // These variables can be modified directly:  
  float m_fSensitivity;  ///< Look up/down sensitivity
  float m_fMoveSpeed;    ///< Move forward/backward speed
  float m_fUpDownSpeed;  ///< Move up/down speed

protected:
  // base initialization; code that's shared between InitFunction and Serialize
  void BaseInit();

  // protected variables
  int m_SpeedMode;  
  VisMouseCamWalkMode_e m_walkMode;

  VInputMap* m_pInputMap;

  bool m_bSimulationIndependent;

private:
#if defined(SUPPORTS_MULTITOUCH)
  VVirtualThumbStick* m_pVirtualThumbStick;
#endif
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
