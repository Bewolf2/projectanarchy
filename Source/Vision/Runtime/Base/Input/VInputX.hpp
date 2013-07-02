/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VInputX.hpp

#ifndef VINPUTX_HPP_INCLUDED
#define VINPUTX_HPP_INCLUDED

//@@@L #include <xwndsysinput.h>

extern char vglKeyStatus[256]; ///< Status for all VGL keys
extern char vglKeyStatus_SC[256]; ///< Scancode version status for all VGL keys

// Maximum numbers of controllers supported for the X window system. Must not be greater than the
// number of controller type constants available.
#define X_MAX_INPUT_DEVICES 7

#define X_MODIFIER_STATES 8

//scaling factor for the mouse wheel
#define MOUSE_WHEEL_SCALE_FACTOR 1000.f

class VInputManagerX;
typedef VInputManagerX VInputManager;


/// \brief
///   Implementation of the standard X Gamepad.
class VGamepadX : public IVJoypad
{

protected:


  /// \brief
  ///   The VGamepadX constructor sets up the required data structures, but does not perform the
  ///   initialization.
  /// 
  /// \param index
  ///   Specifies the gamepad number, which has to be unique und inside the valid range.
  VGamepadX(int index);


  /// \brief
  ///   The VGamepadX destructor deinitializes the attached gamepad if the DeInit method has not
  ///   been already invoked.
  ~VGamepadX();


public:

  /// \brief
  ///   See IVInputDevice::Reset().
  VOVERRIDE void Reset();


  /// \brief
  ///   See IVInputDevice::Update(float fTimeDiff = 0.0f).
  VOVERRIDE void Update(float fTimeDiff = 0.0f);


  /// \brief
  ///   Initialize the gamepad.
  void Init();

    
  /// \brief
  ///   Deinitialize the gamepad.
  void DeInit();

  /// \brief
  ///   See IVInputDevice::IsActive().
  VOVERRIDE bool IsActive();


  /// \brief
  ///   Returns true if the pressure sensitive buttons of the gamepad are enabled as analog inputs.
  bool IsPressureSensitive();


  /// \brief
  ///   Enable or diable analog handling of the preasure sensitive buttons.
  /// 
  /// \param enable
  ///   Set to true if analog values are required. Use false to setup default button behavior.
  void SetPressureSensitive(bool enable);


  /// \brief
  ///   Returns true if the built in orientation sensor of the gamepad is enabled.
  bool IsSensorEnabled();


  /// \brief
  ///   Enable or diable analog handling of the preasure sensitive buttons.
  /// 
  /// \param enable
  ///   Set to true if analog values are required. Use false to setup default button behavior.
  void SetSensorEnabled(bool enable);


  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int device).
  VOVERRIDE const char* GetControlName(unsigned int control);


  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControl and
  ///   VInputDeviceControlX enumeration fot the integer representation. The following controls are
  ///   not supported: CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_X,
  ///   CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_Y, CT_PAD_RIGHT_THUMB_STICK_UNTRIGGERABLE_RAW_X,
  ///   CT_PAD_RIGHT_THUMB_STICK_UNTRIGGERABLE_RAW_Y
  ///
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0.
  VOVERRIDE float GetControlValue(unsigned int control, float fDeadZone);

  
  /// \brief
  ///   See IVInputDevice::GetRawControlValue(unsigned int control).
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControl and
  ///   VInputDeviceControlX enumeration fot the integer representation. The following controls are
  ///   not supported: CT_PAD_LEFT_THUMB_STICK_TIME_UP, CT_PAD_LEFT_THUMB_STICK_TIME_DOWN,
  ///   CT_PAD_LEFT_THUMB_STICK_TIME_LEFT, CT_PAD_LEFT_THUMB_STICK_TIME_RIGHT,
  ///   CT_PAD_RIGHT_THUMB_STICK_TIME_UP, CT_PAD_RIGHT_THUMB_STICK_TIME_DOWN,
  ///   CT_PAD_RIGHT_THUMB_STICK_TIME_LEFT, CT_PAD_RIGHT_THUMB_STICK_TIME_RIGHT;
  VOVERRIDE int GetRawControlValue(unsigned int control);


  /// \brief
  ///   See IVInputDevice::GetName().
  VOVERRIDE const char* GetName();

  
  /// \brief
  ///   See IVInputDevice::GetModel().
  VOVERRIDE int GetModel();

  /// \brief
  ///   Currently not supported.
  VOVERRIDE void Rumble(unsigned short speedLeft, unsigned short speedRight) {}

private:
  friend class VInputManagerX;

  bool m_bActive;

  //index of the pad (Pad0, Pad1, ...)
  int m_iIndex;
  
/*@@@L  CellPadData m_stPadData;
  CellPadData m_stOldPadData;*/

  bool m_bPressureSensitive;
  bool m_bSensorEnabled;

  bool m_bInitialized;

  float m_fTimeDiff;

  //max values of the anlalog sticks (from center)
  static float s_fMaxRThumb;
  static float s_fMaxLThumb;

  void SetActive(bool bActive);
};

/// \brief
///   Implementation of the X Keyboard via libkb.
/// 
/// A global (initialized) instance of this class can be accessed via the static
/// VInputManagerX::GetKeyboard function
/// 
/// \note
///   libkb just supports two simultaneous pressed keys.
class VBASE_IMPEXP_CLASS VKeyboardX : public IVInputDevice
{
public:


  /// \brief
  ///   The VKeyboardX constructor sets up the required data structures, but does not perform the
  ///   initialization.
  VKeyboardX();


  /// \brief
  ///   The VKeyboardX destructor deinitializes the keyboard if the DeInit method has not been
  ///   already invoked.
  ~VKeyboardX();


  /// \brief
  ///   Returns true if the device is initialized.
  VOVERRIDE bool IsActive();

  
  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControlX enumeration for
  ///   the integer representation. You can use the or operation ( controlValue|EXCLUDE_MODIFIERS )
  ///   if you dont want to get a success indicating result if a modifier like ALT or SHIFT is hit.
  ///   If TREAT_WIN_KEYS_AS_MODIFIERS is defined, than even the windows keys are treated as
  ///   modifiers.
  ///
  /// \param deadzone
  ///   Within the dead zone the return value is always 0.
  VOVERRIDE float GetControlValue(unsigned int control, float deadzone);


  /// \brief
  ///   Returns the raw integer representation of the specified control.
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControlX enumeration for
  ///   the integer representation. Modifier exclusion is not supported (See
  ///   VKeyboardX::GetControlValue).
  VOVERRIDE int GetRawControlValue(unsigned int control);

  /// \brief
  ///   Returns the raw integer representation of the specified scan code.
  /// 
  /// \param scanCode
  ///   The scan code of the requested key. Modifier exclusion is not supported (See
  ///   VKeyboardX::GetControlValue).
  int GetRawScanCode(SLONG scanCode);


  /// \brief
  ///   See IVInputDevice::Update(float timeDiff=0).
  VOVERRIDE void Update(float timeDiff=0);


  /// \brief
  ///   Initialize all connected keyboards.
  /// 
  /// Initialize the keyboard.
  /// 
  /// All connected keyboards are treated as one.
  void Init();

  
  /// \brief
  ///   Deinitialize the keyboard.
  void DeInit();

  /// \brief
  ///   See IVInputDevice::Reset().
  VOVERRIDE void Reset();
  

  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int control).
  /// 
  /// \param control
  ///   Numeric representation of the control. Note that only the most common keys return a named
  ///   representation (otherwise NULL).
  VOVERRIDE const char* GetControlName(unsigned int control);


  /// \brief
  ///   See IVInputDevice::GetName().
  VOVERRIDE const char* GetName();


  /// \brief
  ///   See IVInputDevice::GetModel().
  VOVERRIDE VInputDeviceModel GetModel();

  /// \brief
  ///   Returns the language of the first keyboard to find.
  /// 
  /// \note
  ///   If there is no keyboard connected, CELL_KB_MAPPING_ENGLISH_UK  is returned
  unsigned int GetLanguage();
private:
  friend class VInputManagerX;
  friend class VisKey_cl;

/*@@@L  void KeyDown(CellKbData* data, int port);
  void KeyUp(CellKbData* data, int port);

  void InitPSKeyTable(CellKbConfig* mType, int port);
  void InitVisionKeyTable(int port);
  
  int PSKeyToVision(int iXKey, int port);
  int VisionKeyToPS(int iVisionKey, int port);

  void HandleModifierKey(uint32_t pressedModifiers,int VModifierToCheck, int XModifierToCheck);

  // For converting X to Vision-keycode.
  int s_iPSKeyTable[256*X_MAX_INPUT_DEVICES];
  // For converting Vision to X-keycode.
  int s_iVisionKeyTable[256*X_MAX_INPUT_DEVICES];

  // Stores the length of the last keyboard data for every keyboard.
  // To recognize keyup/keydown events.
  int32_t m_iLastKbLength[X_MAX_INPUT_DEVICES];

  // Checks wheater new keyboards are plugged in.
  void ReInit(void);

  // Stores the last state of the modifiers.
  bool m_bModifierStates[X_MODIFIER_STATES];
  bool m_bInitialized;

  // Stores which keyboard is currently active.
  bool m_bActive[X_MODIFIER_STATES];*/
};

/// \brief
///   Implementation of the X Mouse via the libmouse interface.
/// 
/// A global (initialized) instance of this class can be accessed via the static
/// VInputManagerX::GetMouse function
class VBASE_IMPEXP_CLASS VMouseX : public IVInputDevice
{
public:


  /// \brief
  ///   The VMouseX constructor sets up the required data structures, but does not perform the
  ///   initialization.
  VMouseX();


  /// \brief
  ///   The VMouseX destructor deinitializes the attached mouse if the DeInit method has not been
  ///   already invoked.
  ~VMouseX();


  /// \brief
  ///   Returns true if the device is initialized.
  VOVERRIDE bool IsActive();


  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControlX enumeration for
  ///   the integer representation. The following controls are not supported:
  ///   CT_MOUSE_UNTRIGGERABLE_RAW_DELTA_X, CT_MOUSE_UNTRIGGERABLE_RAW_DELTA_Y,
  ///   CT_MOUSE_UNTRIGGERABLE_RAW_X, CT_MOUSE_UNTRIGGERABLE_RAW_Y
  ///
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0.
  VOVERRIDE float GetControlValue(unsigned int control, float fDeadZone);


  /// \brief
  ///   Returns the raw integer representation of the specified control.
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please use the VInputDeviceControlX enumeration for
  ///   the integer representation. The following controls are not supported, because there does
  ///   not exist a raw representation: CT_MOUSE_DELTA_POS_Y, CT_MOUSE_DELTA_POS_X,
  ///   CT_MOUSE_DELTA_NEG_Y, CT_MOUSE_DELTA_NEG_X;
  VOVERRIDE int GetRawControlValue(unsigned int control);


  /// \brief
  ///   See IVInputDevice::Update(float timeDiff=0).
  VOVERRIDE void Update(float timeDiff=0.f);


  /// \brief
  ///   Initializes the mouse.
  /// 
  /// \param hideCursor
  ///   Because there is no mouse cursor from the operating system on X this parameter will be
  ///   ignored. 
  /// 
  /// \param method
  ///   Because of the lack of alternative input libaries on X this parameter is ignored.
  void Init(bool hideCursor = true, SLONG method = VGL_DIRECTINPUT/*VGL_WINDOWSINPUT*/);


  /// \brief
  ///   Deinitialize the mouse.
  void DeInit();


  /// \brief
  ///   See IVInputDevice::Reset().
  VOVERRIDE void Reset();


  /// \brief
  ///   See IVInputDevice::GetName().
  VOVERRIDE const char* GetName();


  /// \brief
  ///   See IVInputDevice::GetModel().
  VOVERRIDE VInputDeviceModel GetModel();

  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int control).
  VOVERRIDE const char* GetControlName(unsigned int control);

  /// \brief
  ///   Sets the sensitivity of this mouse device. The default value is 1.0
  /// 
  /// Sets the sensitivity of this mouse device.
  /// 
  /// The default value is 1.0.
  /// 
  /// This value is used to scale up (or down) all mouse delta movements that are queried via the
  /// CT_MOUSE_DELTA_XYZ controls
  /// 
  /// \param fNewVal
  ///   This value is used to scale up (or down) all mouse delta movements
  ///   that are queried via the CT_MOUSE_DELTA_XYZ controls
  void SetSensitivity(float fNewVal);

  /// \brief
  ///   Returns th value that has been set via SetSensitivity
  inline float GetSensitivity() const
  {
    return m_fSensitivity;
  }

  /// \brief
  ///   Sets the resolution for mouse movements relativ to screen size.
  /// 
  /// \param width
  ///   Width of the screen in pixels.
  /// 
  /// \param height
  ///   Height of the screen in pixels.
  void SetResolution(int width, int height);

  /// \brief
  ///   Sets the mouse-cursor to the middle of the screen.
  void CenterMouseCursor(void);

  /// \brief
  ///   Sets the position of the mouse-cursor to the specified coordinates.
  /// 
  /// \param xPos
  ///   Position on x-axis in pixels.
  /// 
  /// \param yPos
  ///   Position on y-axis in pixels.
  void SetCursorPosition(int xPos, int yPos);

  inline const VGLMouse_t& GetStatus() const { return m_sCurrentStatus; }
  
private:
  //in order to access the logging functionality
  friend class VInputManagerX;

  bool m_bInitialized;
  bool m_bDataChanged;            ///< remembers wheater the mouse data changed 
                                  // after last update.

  VGLMouse_t m_sCurrentStatus;     ///< current position and button status

  int m_iXRes;                     ///< resolution on x-axis in pixels
  int m_iYRes;                     ///< resolution on y-axis in pixels

  float m_fMaxXReciprocal;         ///< 1/screenResX
  float m_fMaxYReciprocal;         ///< 1/screenResY
  float m_fMaxX2Reciprocal;        ///< 1/(screeResX/2)
  float m_fMaxY2Reciprocal;        ///< 1/(screeResY/2)
  float m_fSensitivity;           ///< default 1.0f

};


/// \brief
///   This class provides static methods to access and handle all connected X input devices.
/// 
/// The device manager supports X_MAX_INPUT_DEVICES (default 7) gamepads. Currently there is no
/// support for a keyboard device.
class VInputManagerX
{

public:

  /// \brief
  ///   Initialize the X device manager.
  static void Init();


  /// \brief
  ///   De-initialize the X device manager.
  static void DeInit();


  /// \brief
  ///   Get the initialization status of the X device manager.
  static bool IsInitialized();


  /// \brief
  ///   Reset all connected devices.
  static void Reset();


  /// \brief
  ///   Get a input device of the specified type.
  /// 
  /// \param type
  ///   The type of the requested input device.
  /// 
  /// \return
  ///   IVInputDevice& device: Reference to the requested input device. If the requested type was
  ///   not available the method returns a reference to the field s_NoInputDevice of
  ///   VInputManagerX.
  static IVInputDevice& GetInputDevice(VInputDeviceType type);


  /// \brief
  ///   Update the state of each connected device.
  /// 
  /// This function is called by the engine once a frame.
  /// 
  /// \param fTimeDiff
  ///   The time difference between this and the previous call (used by a few input devices).
  static void Update(float fTimeDiff = 0.0f);


  /// \brief
  ///   Get a reference to a gamepad / joystick of the specified index.
  /// 
  /// \param padIndex
  ///   The index of the gamepad / joystick (The first device has the index 0, the last
  ///   X_MAX_INPUT_DEVICES-1).
  /// 
  /// \return
  ///   IVJoypad& joypad: Reference to the requested gamepad / joystick.
  static IVJoypad& GetPad(int padIndex);

  /// \brief
  ///   Get a reference to the keyboard on X.
  /// 
  /// Get a reference to the keyboard.
  /// 
  /// This Feature is just available on X.
  /// 
  /// \return
  ///   VKeyboardX& keyboard: Reference to the requested keyboard. The method returns always a
  ///   valid keyboard, even if the device is currently not connected. You can check the status of
  ///   the keyboard by calling the method IsActive() of the VKeyboardX.
  /// 
  /// \note
  ///   If there are more then one keyboards connected, just one instance of this InputDevices
  ///   exists and handles all of them.
  VBASE_IMPEXP static VKeyboardX& GetKeyboard();

  /// \brief
  ///   Get a reference to the X mouse.
  VBASE_IMPEXP static VMouseX& GetMouse();

  static int s_iFrameCounter;
  static IVInputDevice s_NoInputDevice;

private:
  friend class VGamepadX;

  static VGamepadX* s_ppInputDevices[X_MAX_INPUT_DEVICES];
  static VKeyboardX s_pKeyboard;
  static VMouseX    s_pMouse;

  static int s_iInitCounter;
  static bool s_bInSystemProcess;
};

#endif //VINPUTX_HPP_INCLUDED

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
