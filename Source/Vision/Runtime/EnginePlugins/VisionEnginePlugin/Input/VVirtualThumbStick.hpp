/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VVirtualThumbStick.hpp

#ifndef V_VVIRTUALTHUMBSTICK_HPP_INCLUDED
#define V_VVIRTUALTHUMBSTICK_HPP_INCLUDED

#ifdef SUPPORTS_MULTITOUCH

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

/// \brief
///   Input class that implements a virtual stumb stick for touch devices.
class VVirtualThumbStick : public IVInputDevice, public IVisCallbackHandler_cl
{
public:
  /// \brief
  ///   Constructor
  ///
  /// \param validArea
  ///   Defines the area in pixel coordinates where the virtual thumb stick is accessible (optional).
  ///   If an invalid rectangle is given the default rectangle is used.
  /// \param fRelativeInitialX
  ///   Initial x-position of the thumb stick relative to the validArea.
  /// \param fRelativeInitialY
  ///   Initial y-position of the thumb stick relative to the validArea.
  /// \param szCirclePath
  ///   File path to the inner circle texture (optional).
  /// \param szRingPath
  ///   File path to the outer ring texture (optional).
  EFFECTS_IMPEXP VVirtualThumbStick(const VRectanglef& validArea = VRectanglef(), 
    float fRelativeInitialX = 0.5f, float fRelativeInitialY = 0.5f,
    const char* szCirclePath = NULL, const char* szRingPath = NULL);

  /// \brief
  ///   Destructor
  EFFECTS_IMPEXP virtual ~VVirtualThumbStick();
  
  /// \brief
  ///   Shows the virtual thumb stick control.
  ///
  /// \param bShow
  ///   Set to false if the control should not be shown.
  EFFECTS_IMPEXP void Show(bool bShow = true);

  /// \brief
  ///   Hides the virtual thumb stick control.
  EFFECTS_IMPEXP void Hide();
  
  EFFECTS_IMPEXP virtual void Reset() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual bool IsActive() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual int GetRawControlValue(unsigned int uiControl) HKV_OVERRIDE;

  /// \brief
  ///   Returns the control value of a specific control.
  ///
  /// \param uiControl
  ///   Can be: CT_PAD_LEFT_THUMB_STICK_UP, CT_PAD_LEFT_THUMB_STICK_DOWN, 
  ///   CT_PAD_LEFT_THUMB_STICK_LEFT or CT_PAD_LEFT_THUMB_STICK_RIGHT.
  /// \param fDeadZone
  ///   The minimum value for which input is detected.
  /// \param bTimeScaled
  ///   Determines if control value is scaled by the frame duration.
  EFFECTS_IMPEXP virtual float GetControlValue(unsigned int uiControl, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual const char* GetName() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual int GetModel() HKV_OVERRIDE
  {
    return INPUT_DEVICE_EMULATED_JOYSTICK;
  }

  /// \brief
  ///   Returns the area in which the virtual thumb stick can be used.
  inline const VRectanglef GetValidArea() const
  {
    return m_validArea;
  }

  /// \brief
  ///   Sets the are in which the virtual thumb stick can be used.
  ///
  // \param validArea
  ///   Defines the area in pixel coordinates where the virtual thumb stick is accessible.
  ///   If an invalid rectangle is given the default rectangle is used.
  EFFECTS_IMPEXP void SetValidArea(const VRectanglef& validArea);

  /// \brief
  ///   Callback handler implementation.
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

protected:
  /// \brief
  ///   Overridden Update function which is called by the virtual thumb stick
  ///   object itself every frame.
  EFFECTS_IMPEXP virtual void Update(float fTimeDiff) HKV_OVERRIDE;

private:
  // Member variables
  int m_iRingWidth, m_iRingHeight;
  int m_iCircleWidth, m_iCircleHeight;
  int m_iCircleCenterX, m_iCircleCenterY;
  
  float m_fXValue, m_fYValue;
  float m_fRelativeInitialX, m_fRelativeInitialY;
  
  VisScreenMaskPtr m_spCircleMask;
  VisScreenMaskPtr m_spRingMask;
  
  float m_fTimeDiff;
  
  bool m_bActive;
  
  VTouchAreaPtr m_spTouchArea;
  VRectanglef m_validArea;
  int m_iLastTouchPointIndex;
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
