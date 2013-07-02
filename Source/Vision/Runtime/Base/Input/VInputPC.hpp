/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VInputPC.hpp

#ifndef VINPUTPC_HPP_INCLUDED
#define VINPUTPC_HPP_INCLUDED

// scaling factor for the mouse wheel
#define MOUSE_WHEEL_SCALE_FACTOR 1000.f

// limited number of PC touchpoints
#define V_PC_MAX_TOUCH_POINTS 10

#ifndef _VISION_NO_XINPUT

//max number of enumerated joysticks or gamepads
#define PC_MAX_JOYPADS 4

//do not change this number! (number of keys on a default joypad)
#define RESERVED_GAMEPAD_KEYS 12

#endif

//scaling factors for the mouse position
extern SLONG vglWindowWidth;
extern SLONG vglWindowHeight;

//which modifiers are allowed?
#define TREAT_WIN_KEYS_AS_MODIFIERS
//#undef TREAT_WIN_KEYS_AS_MODIFIERS

#define TREAT_MAC_KEYS_AS_MODIFIERS
#undef TREAT_MAC_KEYS_AS_MODIFIERS

#define TREAT_META_KEYS_AS_MODIFIERS
#undef TREAT_META_KEYS_AS_MODIFIERS

class VInputManagerPC;
typedef VInputManagerPC VInputManager;

//compatiblity typedefs
HKV_DEPRECATED_2012_2 typedef VInputManagerPC VInputDeviceManager;
HKV_DEPRECATED_2012_2 typedef VInputManagerPC VInputDeviceManagerPC;

/// \brief Available input devices on the PC
enum VInputDeviceModel 
{
  //name ::= "INPUT_DEVICE_" platform "_" device
  INPUT_DEVICE_NONE,                  ///< None is used if there is no device present

  INPUT_DEVICE_EMULATED_TOUCHSCREEN,  ///< emulated touchscreen by vRemoteInput plugin
  INPUT_DEVICE_EMULATED_MOTIONSENSOR, ///< emulated motion sensor by vRemoteInput plugin
  INPUT_DEVICE_EMULATED_JOYSTICK,     ///< emulated joystick

  INPUT_DEVICE_PC_GAMEPAD,    ///< Direct input gamepad
  INPUT_DEVICE_PC_GAMEPAD_XI, ///< XInput Gamepad
  INPUT_DEVICE_PC_MOUSE,      ///< PC mouse
  INPUT_DEVICE_PC_KEYBOARD,   ///< PC Keyboard
  INPUT_DEVICE_PC_MULTITOUCH, ///< PC multi touch interface
  INPUT_DEVICE_PC_JOYSTICK    ///< PC joystick
};

#ifndef _VISION_NO_XINPUT

#ifndef _VISION_DOC

//context information for the DirectInput device enumeration
struct DI_ENUM_CONTEXT_NEW_DEVICE
{
  VString hardwareIds[PC_MAX_JOYPADS]; // hardware IDs of plugged in devices to rule out devices that are already plugged in [in]  

  bool bNewDeviceFound; // If a new device was found [out]
  GUID guidInstance; // GUID of newly found device [out]
  GUID guidProduct; // Product GUID of newly found device [out]
  VString productName; // Product name of the newly found device [out]
  VString hardwareIDNewDevice; // Hardware ID of new device [out]    
};

// context information for the DirectInput EnumObjects method
struct DI_ENUM_OBJECTS
{
  LPDIRECTINPUTDEVICE8 pJoypad; // current pad
};

#endif //_VISION_DOC

//callback to enumerate the direct input devices
int CALLBACK EnumJoypadsPluggedCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
int CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);

#endif


/// \brief
///   Implementation of the PC Mouse via the direct input interface.
/// 
/// A global (initialized) instance of this class can be accessed via the static
/// VInputManagerPC::GetMouse function
class VMousePC : public IVInputDevice
{
public:


  /// \brief
  ///   The VMousePC constructor sets up the required data structures, but does not perform the
  ///   initialization.
  VBASE_IMPEXP VMousePC();


  /// \brief
  ///   The VMousePC destructor deinitializes the attached mouse if the DeInit method has not been
  ///   already invoked.
  VBASE_IMPEXP ~VMousePC();


  /// \brief
  ///   Returns true if the device is active
  VBASE_IMPEXP virtual bool IsActive();

  /// \brief
  ///  can be used to enable/disable mouse input. Enabled by default
  ///
  /// \param bActive
  ///   true for active, false for inactive
  VBASE_IMPEXP void SetActive(bool bActive);

  /// \brief
  ///   Returns true if the device is initialized
  VBASE_IMPEXP virtual bool IsInitialized() const { return m_bInitialized; }
  

  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_MOUSE_REL/ABS_X/Y,
  ///   CT_MOUSE_REL/ABS_DELTA_X/Y, CT_MOUSE_LEFT/MIDDLE/RIGHT_BUTTON, CT_MOUSE_WHEEL,
  ///   CT_MOUSE_WHEEL_UP/DOWN, CT_MOUSE_DATA_CHANGED
  ///
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0. See also SetDeadZone()
  ///
  /// \param bTimeScaled
  ///   Indicates if returned control value is scaled by the frame time difference.
  ///   Ignored in conjunction with mouse input.
  VBASE_IMPEXP virtual float GetControlValue(unsigned int control, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;


  /// \brief
  ///   Returns the raw integer representation of the specified control.
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_MOUSE_RAW_CURSOR_X/Y,
  ///   CT_MOUSE_ABS_X/Y, CT_MOUSE_ABS_DELTA_X/Y, CT_MOUSE_LEFT/MIDDLE/RIGHT_BUTTON, CT_MOUSE_WHEEL,
  ///   CT_MOUSE_WHEEL_UP/DOWN, CT_MOUSE_DATA_CHANGED
  VBASE_IMPEXP virtual int GetRawControlValue(unsigned int control) HKV_OVERRIDE;


  /// \brief
  ///   See IVInputDevice::Update(float timeDiff=0).
  VBASE_IMPEXP virtual void Update(float timeDiff=0.f);


  /// \brief
  ///   Initializes the mouse.
  /// 
  /// \param eMethod
  ///   Method of data retreivement - VGL_DIRECTINPUT or VGL_WINDOWSINPUT (default value is
  ///   VGL_DIRECTINPUT)
  //
  /// \param bHideCursor
  ///   Hide or show the mouse cursor (optional, default: true).
  /// 
  /// \param bExclusive
  ///   Exclusive mode (optional, default: true).
  VBASE_IMPEXP void Init(int eMethod = VGL_DIRECTINPUT, bool bHideCursor = true, bool bExclusive = true);


  /// \brief
  ///   Deinitializes the mouse.
  VBASE_IMPEXP void DeInit();

  /// \brief
  ///   Fills the given parameters with the flags used to initialize the cursor
  VBASE_IMPEXP void GetInitializationFlags(int& eMethod, bool& bHideCursor, bool& bExclusive);

  /// \brief
  ///   See IVInputDevice::Reset().
  VBASE_IMPEXP virtual void Reset();


  /// \brief
  ///   See IVInputDevice::GetName().
  VBASE_IMPEXP virtual const char* GetName();


  /// \brief
  ///   See IVInputDevice::GetModel().
  VBASE_IMPEXP virtual int GetModel();

  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int control).
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP virtual const char* GetControlName(unsigned int control);

  /// \brief
  ///   Sets the sensitivity of this mouse device. The default value is 1.0.
  /// 
  /// This value is used to scale up (or down) all mouse delta movements that are queried via the
  /// CT_MOUSE_DELTA_XYZ controls.
  ///
  /// \param fNewVal
  ///   The new sensitivity value. Larger values represent higher sensitivity.
  VBASE_IMPEXP void SetSensitivity(float fNewVal);

  /// \brief
  ///   Returns the value that has been set via SetSensitivity.
  inline float GetSensitivity() const
  {
    return m_fSensitivity;
  }
  
  inline const VGLMouse_t& GetStatus() const { return m_CurrentStatus; }

  /// \brief
  ///   Gets the input method the mouse is initialized with. Can be VGL_DIRECTINPUT or VGL_WINDOWSINPUT.
  inline SLONG GetInputMethod() const { return m_Method; }

  /// \brief
  ///   Returns the underlying DirectInput device (if input is initialized in DirectInput mode) 
  VBASE_IMPEXP LPDIRECTINPUTDEVICE8 GetDInputDevice();
 
private:
  //in order to access the logging functionality
  friend class VInputManagerPC;
  bool m_bInitialized;
  bool m_bHideCursor;
  bool m_bExclusive;
  bool m_bActive;

  SLONG m_Method;                 ///<VGL_DIRECTINPUT used as default

  VGLMouse_t m_OldStatus;         ///< old position and button status
  VGLMouse_t m_CurrentStatus;     ///< current position and button status

  float m_MaxXReciprocal;         ///< 1/screenResX
  float m_MaxYReciprocal;         ///< 1/screenResY
  float m_MaxX2Reciprocal;        ///< 1/(screeResX/2)
  float m_MaxY2Reciprocal;        ///< 1/(screeResY/2)
  float m_fSensitivity;           ///< default 1.0f
};



/// \brief
///   Implementation of the PC Keyboard via the direct input interface.
/// 
/// A global (initialized) instance of this class can be accessed via the static
/// VInputManagerPC::GetKeyboard function.
class VKeyboardPC : public IVInputDevice
{
public:


  /// \brief
  ///   The VKeyboardPC constructor sets up the required data structures, but does not perform the
  ///   initialization.
  VBASE_IMPEXP VKeyboardPC();


  /// \brief
  ///   The VKeyboardPC destructor deinitializes the keyboard if the DeInit method has not been
  ///   already invoked.
  VBASE_IMPEXP ~VKeyboardPC();


  /// \brief
  ///   Returns true if the device is initialized.
  VBASE_IMPEXP virtual bool IsActive();


  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the requested control. Supported control values are CT_KB_*.
  /// 
  ///   For key modifier treatment, the control value can be logically OR'd with bit defines of type CT_MODIFIER_*.
  ///   For instance, if an event should be mapped to Shift-A, then CT_MODIFIER_SHIFT_PRESSED|CT_KB_A can be passed. There is also a counterpart CT_MODIFIER_SHIFT_NOT_PRESSED,
  ///   so that CT_MODIFIER_SHIFT_PRESSED|CT_MODIFIER_CTRL_NOT_PRESSED|CT_KB_A responds to Shift-A, but not to CTRL-Shift-A. To ignore keyboard input with any modifier,
  ///   the keycode can be combined with the CT_EXCLUDE_MODIFIERS constant which is naturally defined as a bit combination of all the CT_MODIFIER_*_NOT_PRESSED defines.
  ///   Combined Modifiers/key events are thus not controlled with the CT_KB_LSHIFT/CT_KB_RSHIFT key codes - however, these codes can be used to query the status of a single modifier key.
  ///   Note that the modifier bit flags do not differentiate between right or left Shift/Alt/CTRL..
  ///
  /// \param fDeadZone
  ///   Naturally ignored for keyboard
  ///
  /// \param bTimeScaled
  ///   Naturally ignored for keyboard
  VBASE_IMPEXP virtual float GetControlValue(unsigned int control, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;


  /// \brief
  ///   Returns the raw integer representation of the specified control.
  /// 
  /// \param control
  ///   Integer value of the requested control. Supported control values are CT_KB_*.  Modifier exclusion is not supported (See
  ///   VKeyboardPC::GetControlValue).
  VBASE_IMPEXP virtual int GetRawControlValue(unsigned int control) HKV_OVERRIDE;


  /// \brief
  ///   Returns the raw integer representation of the specified scan code.
  /// 
  /// \param scanCode
  ///   The scan code of the requested key. Modifier exclusion is not supported (See
  ///   VKeyboardPC::GetControlValue).
  VBASE_IMPEXP int GetRawScanCode(SLONG scanCode);


  /// \brief
  ///   See IVInputDevice::Update(float timeDiff=0).
  VBASE_IMPEXP virtual void Update(float timeDiff=0);


  /// \brief
  ///   Initializes the keyboard.
  /// 
  /// \param method
  ///   Method of data retreivement - VGL_DIRECTINPUT or VGL_WINDOWSINPUT (optional, default value
  ///   is VGL_DIRECTINPUT)
  /// \param bExclusive
  ///   Exclusive mode (optional, default: true).
  VBASE_IMPEXP void Init(SLONG method = VGL_DIRECTINPUT/*VGL_WINDOWSINPUT*/, bool bExclusive = true);

  
  /// \brief
  ///   Deinitializes the keyboard.
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
  ///   Returns the underlying DirectInput device (if input is initialized in DirectInput mode) 
  VBASE_IMPEXP LPDIRECTINPUTDEVICE8 GetDInputDevice();

private:
  friend class VInputManagerPC;
  friend class VisKey_cl;
  bool m_bInitialized;
  float m_fTimeDiff;

  SLONG m_Method;

};

// ****************************************************************
// *                                                              *
// * VMultiTouchInputPC:                                          *
// * Extracted defines & structs from Windows 7 SDK               *
// *                                                              *
// ****************************************************************

#ifndef _VISION_DOC

// Extracted defines from tpcshrd.h (Windows 7 SDK)
#define TABLET_DISABLE_PRESSANDHOLD        0x00000001
#define TABLET_DISABLE_PENTAPFEEDBACK      0x00000008
#define TABLET_DISABLE_PENBARRELFEEDBACK   0x00000010
#define TABLET_DISABLE_FLICKS              0x00010000

// Extracted defines / structs from WinUser.h (WINVER >= 0x0601)
#define SM_DIGITIZER        94
#define NID_MULTI_INPUT     0x00000040
#define NID_READY           0x00000080
#define TWF_WANTPALM        (0x00000002)

DECLARE_HANDLE(HTOUCHINPUTL); // Touch input handle

// Struct that is delivered via WM_TOUCH message for each touchpoint
typedef struct tagTOUCHINPUTL {
  LONG x;
  LONG y;
  HANDLE hSource;
  DWORD dwID;
  DWORD dwFlags;
  DWORD dwMask;
  DWORD dwTime;
  ULONG_PTR dwExtraInfo;
  DWORD cxContact;
  DWORD cyContact;
} TOUCHINPUTL, *PTOUCHINPUTL;
typedef TOUCHINPUTL const * PCTOUCHINPUTL;

// Touch input flag values (TOUCHINPUT.dwFlags)
#define TOUCHEVENTF_MOVE            0x0001
#define TOUCHEVENTF_DOWN            0x0002
#define TOUCHEVENTF_UP              0x0004

#endif // _VISION_DOC

class VRemoteInput;

/// \class  VMultiTouchInputPC
///
/// \brief  Multi touch device for the PC (Windows 7)
///
class VMultiTouchInputPC : public IVMultiTouchInput
{
  friend class VRemoteInput;
public:

  /// \fn VMultiTouchInputPC()
  ///
  /// \brief  Constructor. 
  ///
  VMultiTouchInputPC();

  /// \fn ~VMultiTouchInputPC()
  ///
  /// \brief  Destructor. 
  ///
  ~VMultiTouchInputPC();

  /// \brief
  ///   See IVInputDevice::Reset().
  virtual void Reset();

  /// \brief
  ///   Tries to initialize Windows 7 Multitouch
  ///
  ///   Prerequisites for successful initialization are
  ///   Windows 7 / Server 2008 R2 and connected & available
  ///   Touch Device. If the prerequisites aren't met, the
  ///   the interface will remain non-initialized and all
  ///   calls to it will just return empty values.
  ///   
  ///   Use IsInitialized() to check if initialization was successful
  ///
  /// \param bEmulate
  ///   if set to true the initializatoin will always succeed so that touch updates can be input manually
  VBASE_IMPEXP void Init(bool bEmulate = false);

  /// \brief
  ///   Deinitializes the multitouch input
  VBASE_IMPEXP void DeInit();

  /// \brief
  ///   See IVInputDevice::Update(float fTimeDiff = 0.0f).
  virtual void Update(float fTimeDiff = 0.0f);

  /// \brief  Gets the number of currently active touch points. 
  ///
  /// \return The number of currently active touch points. 
  ///
  VBASE_IMPEXP virtual int GetMaximumNumberOfTouchPoints()
  {
    return V_PC_MAX_TOUCH_POINTS;
  }

  /// \brief
  ///   Returns true if the device is enabled (see SetEnabled).
  VBASE_IMPEXP virtual bool IsActive();

  /// \brief
  ///   Returns true if the device is initialized
  VBASE_IMPEXP virtual bool IsInitialized();

  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int device).
  HKV_DEPRECATED_2012_2 virtual const char* GetControlName(unsigned int control);

  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_TOUCH_ANY, CT_TOUCH_POINT_n_X/Y for n in [0, 9]
  ///
  /// \param fDeadZone
  ///   Has no meaning for this device.
  ///
  /// \param bTimeScaled
  ///   Has no meaning for this device.
  ///
  VBASE_IMPEXP virtual float GetControlValue(unsigned int control, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;

  /// \brief
  ///   See IVInputDevice::GetRawControlValue(unsigned int control).
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_TOUCH_ANY, CT_TOUCH_POINT_n_X/Y for n in [0, 9]
  virtual int GetRawControlValue(unsigned int control);

  /// \brief
  ///   See IVInputDevice::GetModel().
  virtual int GetModel();

  /// \brief
  ///   See IVInputDevice::GetName().
  virtual const char* GetName();

  /// \brief
  ///   Get the value of a touch point.
  ///
  /// \param index
  ///   Index of the touch point
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_TOUCH_ANY, CT_TOUCH_ABS/REL_X/Y, CT_TOUCH_ABS/REL_X/Y
  ///
  /// \param bTimeScaled
  ///   Has no meaning for this device.
  ///
  VBASE_IMPEXP virtual float GetTouchPointValue(int index, unsigned int control, bool bTimeScaled = false) HKV_OVERRIDE;

  /// \brief  
  ///   Get a touch point.
  ///
  /// \param  uiIndex  
  ///   Index of the touch point
  ///
  VBASE_IMPEXP virtual const VTouchPoint& GetTouch(unsigned int uiIndex) const;

  /// \brief  
  ///   Query, if touch point is active.
  ///
  /// \param  uiIndex  
  ///   Index of the touch point
  ///
  VBASE_IMPEXP virtual bool IsActiveTouch(unsigned int uiIndex) const;

  /// \fn void SetEnabled(bool bEnabled)
  ///
  /// \brief  Sets the status of the multitouch input.
  ///
  /// \param  bEnabled  true to enable, false to disable. 
  ///
  VBASE_IMPEXP void SetEnabled(bool bEnabled);

  /// \brief
  ///   Handle the results of a WM_TOUCH message
  ///   This function should only be called from WndProc
  ///   with the params of a WM_TOUCH message
  ///
  /// \param wParam
  ///   wParam from WndProc
  /// 
  /// \param lParam
  ///   lParam from WndProc
  void HandleTouchMessage(WPARAM wParam, LPARAM lParam);

  /// \brief
  ///   recalculates all currently active touch areas according to a new screen resolution
  ///
  /// \param iOldXRes 
  ///   old width of the screen
  ///
  /// \param iOldYRes
  ///   old height of the screen
  ///
  /// \param iNewXRes
  ///   new width of the screen
  ///
  /// \param iNewYRes
  ///   new height of the screen
  VBASE_IMPEXP void RecalculateAfterResize(int iOldXRes, int iOldYRes, int iNewXRes, int iNewYRes);

private:

  /// \brief
  ///   Add a new touchpoint when it has been detected.
  ///
  /// \param id
  ///   ID of the touchpoint as given by the touch interface
  /// 
  /// \param fXAbsolute
  ///   x-coordinate of touchpoint as given by the device
  ///
  /// \param fYAbsolute
  ///   y-coordinate of touchpoint as given by the device
  VBASE_IMPEXP void AddNewTouch(int id, float fXAbsolute, float fYAbsolute);

  /// \brief
  ///   Update a touchpoint
  ///
  /// \param id
  ///   ID of the touchpoint as given by the touch interface
  /// 
  /// \param fXAbsolute
  ///   x-coordinate of touchpoint as given by the device
  ///
  /// \param fYAbsolute
  ///   y-coordinate of touchpoint as given by the device
  VBASE_IMPEXP void UpdateTouch(int id, float fXAbsolute, float fYAbsolute);

  /// \brief
  ///   Remove a touchpoint.
  ///
  /// \param id
  ///   ID of the touchpoint as given by the touch interface
  VBASE_IMPEXP void RemoveTouch(int id);

  /* Wrapper functions for Win API calls related to Multitouch */

  // Registers a window as no longer being touch-capable
  bool UnregisterTouchWindow (HWND hwnd);

  // Registers a window as being touch-capable
  bool RegisterTouchWindow (HWND hwnd, ULONG ulFlags);

  // Retrieves detailed information about touch inputs associated with a particular touch input handle
  bool GetTouchInputInfo (HTOUCHINPUTL hTouchInput, UINT cInputs, PTOUCHINPUTL pInputs, int cbSize);

  // Closes a touch input handle, frees process memory associated with it, and invalidates the handle
  bool CloseTouchInputHandle (HTOUCHINPUTL hTouchInput);

  /* Local Members */  

  VTouchPoint* m_TouchPoints; // Stores information about each touchpoint
  TOUCHINPUTL* m_pInputs; // Information about touchpoints in WinAPI format
  bool m_bInitialized;
  bool m_bEmulate;
  bool m_bActive;  
  float m_fXScale, m_fYScale; // Scaling used for output of non-raw values

  // Handle to User32.dll
  HINSTANCE m_pGetProcIDUser32;

  // Function pointers for WinAPI calls
  typedef BOOL (__stdcall * pFuncRegisterTouchWindow)(HWND, ULONG);
  typedef BOOL (__stdcall * pFuncUnregisterTouchWindow)(HWND);
  typedef BOOL (__stdcall * pFuncGetTouchInputInfo)(HTOUCHINPUTL, UINT, PTOUCHINPUTL, int);
  typedef BOOL (__stdcall * pFuncCloseTouchInputHandle)(HTOUCHINPUTL);
  pFuncRegisterTouchWindow m_pRegisterTouchWindow;
  pFuncUnregisterTouchWindow m_pUnregisterTouchWindow;
  pFuncGetTouchInputInfo m_pGetTouchInputInfo;
  pFuncCloseTouchInputHandle m_pCloseTouchInputHandle;
};


/// \class  VMotionInputPC
///
/// \brief  Emulating motion input on the PC for use with the vRemoteInput plugin
///
class VMotionInputPC : public IVInputDevice
{
  friend class VRemoteInput;
public:

  VMotionInputPC();

  ~VMotionInputPC();

  /// \brief
  ///   See IVInputDevice::Reset().
  virtual void Reset();

  /// \brief
  ///   See IVInputDevice::Update(float fTimeDiff = 0.0f).
  virtual void Update(float fTimeDiff = 0.0f);

  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int device).
  HKV_DEPRECATED_2012_2 virtual const char* GetControlName(unsigned int control);

  /// \fn void SetEnabled(bool bEnabled)
  ///
  /// \brief  Sets the enabled status of the motion input.
  ///
  /// \param  bEnabled  true to enable, false to disable. 
  ///
  VBASE_IMPEXP void SetEnabled(bool bEnabled);

  /// \fn VBASE_IMPEXP bool IsEnabled()
  ///
  /// \brief  Query if this multitouch input is enabled. 
  ///
  /// \return true if enabled, false if not. 
  ///
  VBASE_IMPEXP bool IsEnabled()
  {
    return m_bEnabled;
  }

  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_MOTION_ACCELERATION_X/Y/Z,
  ///   CT_MOTION_PC_TILT_LR/FB, CT_MOTION_PC_FACING_DIR
  ///
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0.
  ///
  /// \param bTimeScaled
  ///   If true, scales the difference from the dead zone by the frame time.
  ///
  VBASE_IMPEXP virtual float GetControlValue(unsigned int control, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;


  /// \brief
  ///   See IVInputDevice::GetRawControlValue(unsigned int control).
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_MOTION_ACCELERATION_X/Y/Z,
  ///   CT_MOTION_PC_TILT_LR/FB, CT_MOTION_PC_FACING_DIR
  virtual int GetRawControlValue(unsigned int control);

  /// \brief
  ///   See IVInputDevice::GetModel().
  virtual int GetModel();

  /// \brief
  ///   See IVInputDevice::GetName().
  virtual const char* GetName();

  /// \brief  Gets the acceleration. 
  ///
  /// \return The acceleration. 
  ///
  inline const hkvVec3& GetAcceleration() const { return m_Acceleration; }

  /// \brief  Gets the device orientation. 
  ///
  /// \return The device orientation in degrees for x,y,z
  ///
  inline const hkvVec3& GetOrientation() const { return m_Orientation; }

private:

  VBASE_IMPEXP void SetAcceleration(float fAccelerationX, float fAccelerationY, float fAccelerationZ);

  VBASE_IMPEXP void SetOrientation(float fTiltLR, float fTiltFB, float fDir);

  bool m_bEnabled;
  float m_fTimeDiff;
  hkvVec3 m_Acceleration;
  hkvVec3 m_Orientation;
};

#ifndef _VISION_NO_XINPUT


/// \brief
///   PC Gamepad / Joystick implementation via direct input.
/// 
/// Please use the VGamepadXI class if the device is XInput compatible.
/// 
/// Initialized instances can be accessed via the static VInputManagerPC::GetPad function.
class VGamepadPC : public IVJoypad
{

public:


  /// \brief
  ///   The VGamepadPC constructor sets up the required data structures, but does not perform the
  ///   initialization.
  VBASE_IMPEXP VGamepadPC();


  /// \brief
  ///   The VGamepadPC destructor deinitializes the attached gamepad if the DeInit method has not
  ///   been already invoked.
  VBASE_IMPEXP ~VGamepadPC();


  /// \brief
  ///   Returns true if the device is working properly.
  VBASE_IMPEXP virtual bool IsActive();


  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param device
  ///   Integer value of the requested control. Supported control values include CT_PAD_*with the exception of the UNTRIGGERED enumeration values.
  ///
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0. See also SetDeadZone()
  ///
  /// \param bTimeScaled
  ///   Indicates if returned control value is scaled by the frame time difference.
  VBASE_IMPEXP virtual float GetControlValue(unsigned int device, float fDeadZone, bool bTimeScaled = false);


  /// \brief
  ///   Returns the raw integer representation of the specified control.
  /// 
  /// \param control
  ///   Integer value of the requested control. Supported control values include CT_PAD_*. Note that
  ///   for controls which are mapped to be time scaled, the returned value is always
  ///   a non-scaled one (raw value). If a control was mapped to be time-scaled then
  ///   the GetControlValue() function will return the scaled value as intended.
  VBASE_IMPEXP virtual int GetRawControlValue(unsigned int control);
  
  
  /// \brief
  ///   See IVInputDevice::Update(float timeDiff=0).
  VBASE_IMPEXP virtual void Update(float timeDiff=0);


  /// \brief
  ///   Initialize the gamepad or joystick.
  VBASE_IMPEXP void Init();
    
  /// \brief
  ///   Deinitialize the gamepad.
  VBASE_IMPEXP virtual void DeInit();

  /// \brief
  ///   See IVInputDevice::Reset().
  VBASE_IMPEXP virtual void Reset();


  /// \brief
  ///   See IVInputDevice::IsInitialized().
  VBASE_IMPEXP virtual bool IsInitialized();
 

  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int control).
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP virtual const char* GetControlName(unsigned int control);


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
  ///   See IVJoypad::GetHardwareId().
  VBASE_IMPEXP virtual const char* GetHardwareId();


  /// \brief
  ///   See IVJoypad::GetProductName().
  VBASE_IMPEXP virtual const char* GetProductName();

  /// \brief
  ///   Check if the gamepad is plugged in
  VBASE_IMPEXP bool IsPlugged() const;

private:
  //in order to access the logging functionality
  friend class VInputManagerPC;
  bool m_bActive;
  bool m_bInitialized;

  GUID m_HardwareGUID;
  VString m_HardwareId;
  VString m_ProductName;

  float m_fTimeDiff;
  float m_fRScaling;
  float m_fLScaling;

  DIJOYSTATE2 m_CurrentStatus;
  DIJOYSTATE2 m_OldStatus;
  LPDIRECTINPUTDEVICE8 m_pJoypad;
  VSmartPtr<IDirectInput8> m_spDInput;    

  static float s_MaxRThumb;
  static float s_MaxLThumb;
  static float s_MaxSlider;
};

#endif


/// \brief
///   This class provides static methods to access and handle all connected PC input devices.
/// 
/// The device manager supports one keyboard, one mouse and PC_MAX_JOYPADS (default 4)  gamepads /
/// joysticks. XInput capable gamepads / joysticks are detected and handled as XInput devices
/// instead of direct input.
class VInputManagerPC : public VInputManagerBase
{
public:

  
  /// \brief
  ///   Initialize the PC device manager.
  /// 
  /// This function has to be called by the application once. The default application class of the
  /// engine calls it inside its InitInput function
  /// 
  /// \param initMethodKeyboard
  ///   Method of data retrieval for the keyboard - VGL_DIRECTINPUT or VGL_WINDOWSINPUT (optional, default value is
  ///   VGL_DIRECTINPUT)
  ///
  /// \param initMethodMouse
  ///   Method of data retrieval for the mouse - VGL_DIRECTINPUT or VGL_WINDOWSINPUT (optional, default value is
  ///   VGL_DIRECTINPUT)
  /// 
  /// \param bHideCursor
  ///   Set to true in order to hide the mouse cursor.
  ///
  /// \param bMouseExclusive
  ///   Set to true for exclusive mouse mode
  ///
  /// \param bKeyboardExclusive
  ///   Set to true for exclusive keyboard mode
  VBASE_IMPEXP static void Init(int initMethodKeyboard = VGL_DIRECTINPUT, int initMethodMouse = VGL_DIRECTINPUT, bool bHideCursor=true, 
    bool bMouseExclusive = true, bool bKeyboardExclusive = true);


  /// \brief
  ///   De-initialize the PC device manager. (Again, by default this is called by the application's
  ///   DeInitInput function)
  VBASE_IMPEXP static void DeInit();


  /// \brief
  ///   Get the initialization status of the PC device manager.
  VBASE_IMPEXP static bool IsInitialized();

  ///  \brief
  ///    Handle a device that was unplugged (DInput or XInput). Is called from the windows message handler
  ///    upon DBT_DEVICEREMOVECOMPLETE
  VBASE_IMPEXP static void HandleUnpluggedDevice();

  ///  \brief
  ///    Handle a DInput device that was just plugged in.
  ///    WARNING: Don't call this function from a windows message handler as it triggers a COM call, which cannot be done a message handler
  VBASE_IMPEXP static void HandlePluggedDeviceDI();

  ///   \brief
  ///    Mark that a DInput device was plugged in. We receive DBT_DEVICEARRIVAL in the windows message handler, but can't directly 
  ///    call VInputManagerPC::HandlePluggedDeviceDI because it triggers a COM call, which can't be done in a message handler.
  ///    For each call of IncreaseDIHandlerCounter, HandlePluggedDeviceDI will be called once during the next input update.
  VBASE_IMPEXP static void IncreaseDIHandlerCounter();

  ///  \brief
  ///    Handle an XInput device that was just plugged in.
  VBASE_IMPEXP static bool HandlePluggedDeviceXI();    

  /// \brief
  ///    Set the behavior of gamepad ports when unplugging gamepad devices
  ///    Off by default.
  ///
  /// \param bFillPorts
  ///   If true, gamepad ports will be filled from zero. E.g. if you have two devices and unplug the pad with index 0
  ///   the second device will now receive index 0. If set to false, plugged gamepads will always keep their index
  VBASE_IMPEXP static void SetFillEmptyGamepadPorts(bool bFillPorts);

  /// \brief
  /// \return
  ///   If true, gamepad ports will be filled from zero. E.g. if you have two devices and unplug the pad with index 0
  ///   the second device will now receive index 0. If set to false, plugged gamepads will always keep their index.
  ///   Off by default.
  VBASE_IMPEXP static bool GetFillEmptyGamepadPorts();

  /// \brief
  ///   Reset all connected devices.
  VBASE_IMPEXP static void Reset();


  /// \brief
  ///   Gets an input device of the specified type.
  /// 
  /// \param type
  ///   The type of the requested input device.
  /// 
  /// \return
  ///   IVInputDevice &device: Reference to the requested input device. If the requested type was
  ///   not available the method returns a reference to VInputManagerPC::s_NoInputDevice.
  VBASE_IMPEXP static IVInputDevice& GetInputDevice(VInputDeviceType type);

  /// \brief
  ///   Gets an input device of the specified type.
  /// 
  /// \param szDeviceName
  ///   The name of the requested input device, which can be "MOUSE", "KEYBOARD", "MOTION" (or "MOTIONSENSOR"),
  ///   "TOUCH" (or "TOUCHSCREEN") , "PAD1", "PAD2", "PAD3" or "PAD4";
  /// 
  /// \return
  ///   IVInputDevice &device: Reference to the requested input device. If the requested type was
  ///   not available the method returns a reference to VInputManagerPC::s_NoInputDevice.
  VBASE_IMPEXP static IVInputDevice& GetInputDevice(const char * szDeviceName);

  /// \brief
  ///   Update the state of each connected device.
  /// 
  /// This function is called once a frame by the engine
  ///
  /// \param timeDiff
  ///   The time difference between this and the previous call (used by a few input devices).
  VBASE_IMPEXP static void Update(float timeDiff=0);


  /// \brief
  ///   Get a reference to the PC mouse.
  VBASE_IMPEXP static VMousePC& GetMouse();


  /// \brief
  ///   Get the input method of the input system of the keyboard (VGL_DIRECTINPUT or VGL_WINDOWSINPUT).
  VBASE_IMPEXP static SLONG GetInputMethodKeyboard();

  /// \brief
  ///   Get the input method of the input system of the mouse (VGL_DIRECTINPUT or VGL_WINDOWSINPUT).
  VBASE_IMPEXP static SLONG GetInputMethodMouse();


  /// \brief
  ///   Return true if the mouse cursor has been hidden.
  VBASE_IMPEXP static bool IsMouseCursorHidden();


  /// \brief
  ///   Get a reference to the PC keyboard.
  VBASE_IMPEXP static VKeyboardPC& GetKeyboard();

  /// \brief
  ///   Get a reference to the Multitouch Device
  VBASE_IMPEXP static VMultiTouchInputPC& GetMultitouch();

  /// \brief
  ///   Get a reference to the emulated Motion Sensor Device
  VBASE_IMPEXP static VMotionInputPC& GetMotionSensor();

#ifndef _VISION_NO_XINPUT

  /// \brief
  ///   Get a reference to a gamepad / joystick of the specified index.
  /// 
  /// \param index
  ///   The index of the gamepad / joystick (The first device has the index 0, the last
  ///   PC_MAX_JOYPADS-1).
  /// 
  /// \return
  ///   IVJoypad& joypad: Reference to the requested gamepad / joystick. The method returns always
  ///   a valid joypad if the index is in the valid range, even if the device is currently not
  ///   connect. You can check the status of the joypad by calling IsInitialized(), because only
  ///   connected devices are initialized.
  VBASE_IMPEXP static IVJoypad& GetPad(int index);

#endif

  /// \brief
  /// NoInputDevice for comparison checks
  VBASE_IMPEXP static IVInputDevice s_NoInputDevice;

  static int s_iFrameCounter;

private:

#ifndef _VISION_NO_XINPUT

  // Get a currently unused gamepad port
  // This will return the first free port starting from 0
  static int GetFreeGamepadSlot();

  // Init a gamepad to a specified port
  static void SetGamepadToPort(IVJoypad* pPad, int iPort, bool bIsXInput);

  static IVJoypad *s_ppJoypad[PC_MAX_JOYPADS];
  static bool s_bFillEmptyGamepadPorts;
  static bool s_bXInputPortMonitor[PC_MAX_JOYPADS]; 
  static int s_iDeferredPluggedDICounter;
#endif

  static VMousePC s_Mouse;
  static VKeyboardPC s_Keyboard;
  static VMultiTouchInputPC s_Multitouch;
  static VMotionInputPC s_Motion;

  static int s_iInitCounter;
  static bool s_bMouseCursorHidden;
  static SLONG s_iInputMethodKeyboard;

  /// Register for detailed device plugging notifications in message handler
  static void InitDeviceNotification();
  static void DeInitDeviceNotification();
  static HDEVNOTIFY s_pDevNotifyHandle;
};


#endif //VINPUTPC_HPP_INCLUDED

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
