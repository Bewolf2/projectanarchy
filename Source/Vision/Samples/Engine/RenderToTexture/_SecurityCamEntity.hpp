/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef H_SECURITYCAMENTITY_HPP_INCLUDED
#define H_SECURITYCAMENTITY_HPP_INCLUDED

////////////////////////////////////////////////////////////////////////////////
// class SecurityCamEntity_cl
////////////////////////////////////////////////////////////////////////////////
// This entity class represents a single security camera that renders the scene
// from its own position and orientation in given time intervals.
// The texture is mapped onto the monitor geometry in the editor and thus the 
// rendered scene gets displayed on a monitor 
////////////////////////////////////////////////////////////////////////////////
enum RTT_CONTROL {
  RTT_PICK_CAMERA = VISION_INPUT_CONTROL_LAST_ELEMENT+1
};

class SecurityCamEntity_cl : public VisBaseEntity_cl, public IVisCallbackHandler_cl
{
public:

  SecurityCamEntity_cl();
  virtual ~SecurityCamEntity_cl();

  // activates the security camera
  void          Activate(BOOL status=TRUE);

  // sets the world surface that the renderable texture will be applied to
  void          SetMonitorSurfaceName(const char *szSurface);

  // entity functions
  virtual void  DeInitFunction();
  virtual void  InitFunction();
  virtual void  ThinkFunction();

  inline bool   IsPicked() const                  { return m_bIsPicked; }
  void          PickCamera(bool status=true);

  inline void   SetYawSpeed(float f)              { m_fYawSpeed = f; }

  // Handle background events
  void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE;


  // render context
  VisRenderContextPtr m_spRenderContext;
  VisRenderableTexturePtr m_spRenderTarget;
  VisRenderableTexturePtr m_spDepthStencilTarget;

  // data:
  float m_fUpdateTime;    // update interval, if 0.0, the texture gets updated every frame
  char m_szMonitorSrfName[FS_MAX_PATH]; // texture filename of the destination texture
  float m_fFOV[2];        // the field of view used to render image
  float m_fYawSpeed;      // speed to oscillate the camera's yaw
  float m_fYawAmplitude;  // aplitude (degree) for yaw-oscillation

private:
  void GetCurrentAngles();
  void SetRenderNextFrame(bool bStatus); // do the rendering

  float m_fUpdatePos; // current time
  BOOL m_bIsActive;   // activation status
  float m_fTurnPos;   // current yaw phase
  float m_fAngles[3]; // store the entity orientation
  bool m_bIsPicked;   // camera picked by player?
  BOOL m_bUseThermalImage; // if enabled, use a renderloop that simulates a thermal image

  static SecurityCamEntity_cl *g_pCurrentlyPickedCam;
public:
  V_DECLARE_SERIAL( SecurityCamEntity_cl, )
    IMPLEMENT_OBJ_CLASS(SecurityCamEntity_cl);
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
