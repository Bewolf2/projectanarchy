/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VZoomDetector.hpp

#ifndef V_VZOOMDETECTOR_HPP_INCLUDED
#define V_VZOOMDETECTOR_HPP_INCLUDED

#ifdef SUPPORTS_MULTITOUCH

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

enum VZoomGesture
{
  CT_TOUCH_ZOOM,
  CT_TOUCH_ZOOM_IN,
  CT_TOUCH_ZOOM_OUT
};

class VZoomDetector : public IVInputDevice
{
public:
  EFFECTS_IMPEXP VZoomDetector(const VRectanglef& validArea = VRectanglef(), float fMaxDistance = -1.0f);

  EFFECTS_IMPEXP virtual void Reset() HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual void Update(float timeDiff = 0.0f) HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual bool IsActive() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual int GetRawControlValue(unsigned int uiControl) HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual float GetControlValue(unsigned int uiControl, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual const char* GetName() HKV_OVERRIDE;
  
private:
  
  VRectanglef m_area;
  bool m_fullScreen;
  
  float m_fMaxDistance;
  
  float m_fLastValue;
  float m_fValue;
  
  bool m_bActive;
   
  IVMultiTouchInput* m_pInputDevice;
};

#endif

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
