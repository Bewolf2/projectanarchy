/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VInputXI.hpp

#ifndef VINPUTXI_HPP_INCLUDED
#define VINPUTXI_HPP_INCLUDED

#if defined(WIN32) && !defined(_VISION_WINRT)

  #ifndef _VISION_DOC

  struct XI_ENUM_CONTEXT
  {
      bool isXInput;
      int iDeviceNum;
      int iDeviceCounter;
      VString hardwareId;
      VString productName;
  };

  #endif // _VISION_DOC

  //callback to enumerate the direct input devices
  int CALLBACK EnumGamepadXICallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );

#endif


 
/// \brief
///   Implementation of a XInput capable input device like a joystick or a gamepad.
/// 
/// Please use VGamepadPC if your gamepad supports only direct input.
/// 
/// XInput on PC is only available for WindowsXP Service Pack 1 or newer Windows Versions.
class VGamepadXI : public IVJoypad
{
public:


  /// \brief
  ///   The VGamepadXI constructor sets up the required data structures, but does not perform the
  ///   initialization.
  /// 
  /// \param index
  ///   Specifies the XInput index, which has to be unique and inside the valid range.
  VBASE_IMPEXP VGamepadXI(int index);


  /// \brief
  ///   The VGamepadXI destructor deinitializes the attached gamepad if the DeInit method has not
  ///   been already invoked.
  VBASE_IMPEXP ~VGamepadXI();


  /// \brief
  ///   See IVInputDevice::Update(float timeDiff=0).
  VBASE_IMPEXP virtual void Update(float timeDiff=0);


  /// \brief
  ///   Returns true if the device is working.
  VBASE_IMPEXP virtual bool IsActive();


  /// \brief
  ///   Initialize the XInput device.
  VBASE_IMPEXP void Init();


  /// \brief
  ///   Deinitialize the XInput device.
  VBASE_IMPEXP void DeInit();


  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param device
  ///   Integer value of the demanded control. Please use the VInputDeviceControl enumeration for
  ///   the integer representation. The following controls are not supported:
  ///   CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_X, CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_Y,
  ///   CT_PAD_RIGHT_THUMB_STICK_UNTRIGGERABLE_RAW_X, CT_PAD_RIGHT_THUMB_STICK_UNTRIGGERABLE_RAW_Y;
  ///
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0. See also SetDeadZone()
  ///
  /// \param bTimeScaled
  ///   Indicates if returned control value is scaled by the frame time difference.
  VBASE_IMPEXP virtual float GetControlValue(unsigned int device, float fDeadZone=0.0f, bool bTimeScaled = false);


  /// \brief
  ///   Returns the raw integer representation of the specified control.
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControl enumeration for
  ///   the integer representation. Note that for controls which are mapped to be time scaled, 
  ///   the returned value is always a non-scaled one (raw value). If a control was mapped to be
  ///   time-scaled then the GetControlValue() function will return the scaled value as intended.
  VBASE_IMPEXP virtual int GetRawControlValue(unsigned int control);


  /// \brief
  ///   See IVInputDevice::IsInitialized().
  VBASE_IMPEXP virtual bool IsInitialized();


  /// \brief
  ///   See IVInputDevice::Reset(bool bActive).
  VBASE_IMPEXP virtual void Reset();


  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int device).
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP virtual const char* GetControlName(unsigned int device);
  
  
  /// \brief
  ///   See IVInputDevice::GetName().
  VBASE_IMPEXP virtual const char* GetName();


  /// \brief
  ///   See IVInputDevice::GetModel().
  VBASE_IMPEXP virtual int GetModel();


  /// \brief
  ///   See IVJoypad::Rumble(unsigned short speedLeft, unsigned short speedRight).
  VBASE_IMPEXP virtual void Rumble(unsigned short speedLeft, unsigned short speedRight);

  /// \brief
  ///   Currently not supported.
  VBASE_IMPEXP VOVERRIDE const char* GetHardwareId() {return "";}

  /// \brief
  ///   Currently not supported.
  VBASE_IMPEXP VOVERRIDE const char* GetProductName() {return "";}

  /// \brief
  ///   Test if the XInput device is still plugged in
  VBASE_IMPEXP bool IsPlugged() const;

private:

#ifdef WIN32
  friend class VInputManagerPC;
#endif

  // Helper method to check if a XInput port is used
  static bool TestXInputPort(int port);

  bool m_bActive;
  bool m_bInitialized;

  int m_iIndex;

  VString m_HardwareId;
  VString m_ProductName;

  XINPUT_STATE m_OldStatus;
  XINPUT_STATE m_CurrentStatus;
  XINPUT_VIBRATION m_RumbleStatus;

  bool m_bFeedbackChanged;
  
  float m_fTimeDiff;

  static float s_MaxRThumb;
  static float s_MaxLThumb;
};



#if defined(_VISION_XENON)

/// \brief
///   Implementation of the Xenon Keyboard via the XInput interface.
/// 
/// A global (initialized) instance of this class can be accessed via the static
/// VInputManagerXenon::GetKeyboard function
class VKeyboardXI : public IVInputDevice
{
public:


  /// \brief
  ///   The VKeyboardXI constructor sets up the required data structures, but does not perform the
  ///   initialization.
  VBASE_IMPEXP VKeyboardXI();


  /// \brief
  ///   The VKeyboardXI destructor deinitializes the keyboard if the DeInit method has not been
  ///   already invoked.
  VBASE_IMPEXP ~VKeyboardXI();


  /// \brief
  ///   Returns true if the device is initialized.
  VBASE_IMPEXP virtual bool IsActive();

  
  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControlPC enumeration for
  ///   the integer representation. 
  ///   For key modifier treatment, the control value from the VInputDeviceControlPC enum can be logically OR'd with bit defines of type CT_MODIFIER_XYZ.
  ///   For instance, if an event should be mapped to Shift-A, then CT_MODIFIER_SHIFT_PRESSED|CT_KB_A can be passed. There is also a counterpart CT_MODIFIER_SHIFT_NOT_PRESSED,
  ///   so that CT_MODIFIER_SHIFT_PRESSED|CT_MODIFIER_CTRL_NOT_PRESSED|CT_KB_A responds to Shift-A, but not to CTRL-Shift-A. To ignore keyboard input with any modifier,
  ///   the keycode can be combined with the CT_EXCLUDE_MODIFIERS constant which is naturally defined as a bit combination of all the CT_MODIFIER_xyz_NOT_PRESSED defines.
  ///   Combined Modifiers/key events are thus not controlled with the CT_KB_LSHIFT/CT_KB_RSHIFT key codes - however, these codes can be used to query the status of a single modifier key.
  ///   Note that the modifier bit flags do not differentiate between right or left Shift/Alt/CTRL..
  ///
  /// \param deadzone
  ///   Naturally ignored for keyboard
  ///
  /// \param bTimeScaled
  ///   Naturally ignored for keyboard
  VBASE_IMPEXP virtual float GetControlValue(unsigned int control, float deadzone, bool bTimeScaled = false);


  /// \brief
  ///   Returns the raw integer representation of the specified control.
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControlXbox360
  ///   enumeration for the integer representation. Modifier exclusion is not supported (See
  ///   VKeyboardXI::GetControlValue).
  VBASE_IMPEXP virtual int GetRawControlValue(unsigned int control);

  /// \brief
  ///   Retrieves a keystroke input event on the keyboard.
  /// 
  /// Retrieves a keystroke (pKeystroke parameter).
  /// 
  /// \param pKeystroke
  ///   This type stores informations like, which key is pressed, for more details see the Xbox
  ///   Documentation  of XInputGetKeystroke.
  /// 
  /// \return
  ///   A DWORD value denoting the result:
  ///   \li If the function succeeds, the return value is ERROR_SUCCESS.
  /// 
  ///   \li If no new keys have been pressed, the return value is ERROR_EMPTY.
  /// 
  ///   \li If the controller is not connected or the user has not activated it, the return value
  ///     is ERROR_DEVICE_NOT_CONNECTED. 
  /// 
  ///   \li For more details see the Xbox Documentation for XInputGetKeystroke.
  VBASE_IMPEXP DWORD GetKeystroke(PXINPUT_KEYSTROKE pKeystroke);

  /// \brief
  ///   Returns the raw integer representation of the specified scan code.
  /// 
  /// \param scanCode
  ///   The scan code of the requested key. Modifier exclusion is not supported (See
  ///   VKeyboardXI::GetControlValue).
  VBASE_IMPEXP int GetRawScanCode(SLONG scanCode);


  /// \brief
  ///   See IVInputDevice::Update(float timeDiff=0).
  VBASE_IMPEXP virtual void Update(float timeDiff=0);


  /// \brief
  ///   Initialize the keyboard.
  VBASE_IMPEXP void Init();

  
  /// \brief
  ///   Deinitialize the keyboard.
  VBASE_IMPEXP void DeInit();

  /// \brief
  ///   See IVInputDevice::Reset().
  VBASE_IMPEXP virtual void Reset();
  

  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int control).
  /// 
  /// \param control
  ///   Numeric representation of the control. Note that only the most common keys return a named
  ///   representation  (otherwise "Unnamed Control").
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP virtual const char* GetControlName(unsigned int control);


  /// \brief
  ///   See IVInputDevice::GetName().
  VBASE_IMPEXP virtual const char* GetName();


  /// \brief
  ///   See IVInputDevice::GetModel().
  VBASE_IMPEXP virtual int GetModel();

  /// \brief
  ///   Returns the language of the first keyboard to find.
  /// 
  /// \note
  ///   If there is no keyboard connected, CELL_KB_MAPPING_ENGLISH_UK  is returned
  VBASE_IMPEXP DWORD GetLanguage();

private:
  friend class VInputManagerXenon;

  bool m_bInitialized;
};
#endif


#endif //VINPUTXI_HPP_INCLUDED

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
