/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VInput.hpp

#ifndef VINPUT_HPP_INCLUDED
#define VINPUT_HPP_INCLUDED

#define CT_MODIFIER_SHIFT_PRESSED      0x00010000
#define CT_MODIFIER_ALT_PRESSED        0x00020000
#define CT_MODIFIER_CTRL_PRESSED       0x00040000
#define CT_MODIFIER_WIN_PRESSED        0x00080000
#define CT_MODIFIER_META_PRESSED       0x00100000

#define CT_MODIFIER_SHIFT_NOT_PRESSED  0x01000000
#define CT_MODIFIER_ALT_NOT_PRESSED    0x02000000
#define CT_MODIFIER_CTRL_NOT_PRESSED   0x04000000
#define CT_MODIFIER_WIN_NOT_PRESSED    0x08000000
#define CT_MODIFIER_META_NOT_PRESSED   0x10000000

#define CT_EXCLUDE_MODIFIERS           0xFF000000
#define CT_MODIFIER_MASK               0xFFFF0000

// backwards compatibility
#define EXCLUDE_MODIFIERS              CT_EXCLUDE_MODIFIERS

/// \brief Value to indicate a key is pressed in the VMappedPressHold
#define PRESSHOLD_PRESSED              (-1.0f)
/// \brief Value to indicate a key is hold in the VMappedPressHold
#define PRESSHOLD_HOLD                 (1.0f)

/// \brief Used to validate if a key is pressed in VMappedPressHold 
#define PRESSHOLD_ISPRESSED(triggerState)     (triggerState<-0.01f)
/// \brief Used to validate if a key is hold in VMappedPressHold
#define PRESSHOLD_ISHOLD(triggerState)        (triggerState> 0.01f)

//shortcut for the most common used input devices
#define V_PS3_PAD(x)      VInputManagerPS3::GetPad(x)

#define V_PSP2_PAD(x)     VInputManagerPSP2::GetPad(x)

#define V_XENON_PAD(x)    VInputManagerXenon::GetPad(x)

#define V_WIIU_PAD(x)     VInputManagerWiiU::GetPad(x)
#define V_WIIU_REMOTE(x)  VInputManagerWiiU::GetRemote(x)

#ifdef _VISION_WINRT
#define V_PC_INPUTMANAGER VInputManagerWinRT
#define V_PC_MOUSE        VInputManagerWinRT::GetMouse()
#define V_PC_KEYBOARD     VInputManagerWinRT::GetKeyboard()
#define V_PC_PAD(x)       VInputManagerWinRT::GetPad(x)
#else
#define V_PC_INPUTMANAGER VInputManagerPC
#define V_PC_MOUSE        VInputManagerPC::GetMouse()
#define V_PC_KEYBOARD     VInputManagerPC::GetKeyboard()
#define V_PC_PAD(x)       VInputManagerPC::GetPad(x)
#endif


//generic:
#define V_MOUSE           VInputManager::GetMouse()
#define V_KEYBOARD        VInputManager::GetKeyboard()
#define V_PAD(x)          VInputManager::GetPad(x)


/// \brief
///   Enumeration for the type of a mapped value, see VMappedInput::GetType
enum VMappedValueType 
{
  NOT_MAPPED,
  VALUE_MAPPED,
  SINGLE_HIT_MAPPED,
  ONCE_MAPPED = SINGLE_HIT_MAPPED, //compatibility to old style names
  AXIS_MAPPED,
  PRESS_HOLD_MAPPED,
  TOUCH_VALUE_MAPPED,
  TOUCH_SINGLE_HIT_MAPPED,
  TOUCH_ONCE_MAPPED = TOUCH_SINGLE_HIT_MAPPED //compatibility to old style names
};

/// \brief
///   Enumeration that defines the single hit event type for single hit mapped values. See
///   VMappedValueSingleHit::SetSingleHitType and VMappedValueSingleHitPerFrame::SetSingleHitType
enum VOnceMode
{
  ONCE_ON_PRESS = 1,                ///< Trigger on press
  ONCE_ON_RELEASE = 2,              ///< Trigger on release
  ONCE_ON_PRESS_AND_RELEASE = ONCE_ON_PRESS | ONCE_ON_RELEASE       ///< Trigger on both press and release
};

// Deprecated.
enum VSingleHitType
{
  SINGLE_HIT_ONPRESSED = ONCE_ON_PRESS,
  SINGLE_HIT_ONRELEASE = ONCE_ON_RELEASE,
  SINGLE_HIT_ONPRESSEDANDRELEASE = ONCE_ON_PRESS_AND_RELEASE
};

/// \brief
///   Enumeration for the type of the InputDevice
enum VInputDeviceType 
{
  INPUT_DEVICE_FIRST = 1,
  INPUT_DEVICE_1 = INPUT_DEVICE_FIRST, //PAD 1 - 8
  INPUT_DEVICE_2,
  INPUT_DEVICE_3,
  INPUT_DEVICE_4,
  INPUT_DEVICE_5,
  INPUT_DEVICE_6,
  INPUT_DEVICE_7,
  INPUT_DEVICE_8,
  INPUT_DEVICE_LAST = INPUT_DEVICE_8,
  INPUT_DEVICE_KEYBOARD = 64,
  INPUT_DEVICE_MOUSE,
  INPUT_DEVICE_TOUCHSCREEN,
  INPUT_DEVICE_TOUCHSCREEN_BACK,
  INPUT_DEVICE_MOTION_SENSOR
};

// this would be the new enum - unified names, removed duplicates ambiguous entries
// 
// still not sure how to handle most of the??_X_NEG and ??_X_POS controls,
// sometimes we support it also as ??_X (and sometimes just as ??_X) we should always provide all 3 versions...
//
//  UNTRIGGERABLE is still confusing.. maybe also needs some rework!
// 
// name convention:
// ================
// name       ::= "CT_" device [ "_" platform ] "_" control
// 
// platform   ::= ( "PC" | "PS3" | "PSP2" | "WIIU" | "ANDROID" )
// device     ::= ( "KB" | "PAD" | "POINTER" | "MOUSE" | "BALANCEBOARD" | "NUNCHUCK" | "MOTIONPLUS" | "MOTION" | "TOUCH" | "JOYSTICK" )
// control    ::= [ "LEFT_" | "RIGHT_" | "MIDDLE_" ] identifier [ "_DELTA" ] [ "_X" | "_Y" | "_Z" ] [ "_NEG" | "_POS" ]
// identifier ::= { "A".."Z" | "0".."9" | "_" }*
// 

/// \brief Control (buttons, keys, touch points, etc) names. Use them to assign a control to a specific trigger in your VInputMap.
/// 			 Except for PC, there are platform specific versions of that enumeration as well.
/// \note Use VInputManagerBase::GetUniqueControlName for serialization, because the values of VInputControl may change, but these
/// 			unique names will not change.
/// \see VInputMap
/// \see VInputManagerBase::GetUniqueControlName
/// \see VInputManagerBase::GetUniqueControlIdentifier
enum VInputControl 
{
  CT_PAD_UP = 0,
  CT_PAD_DOWN,
  CT_PAD_LEFT,
  CT_PAD_RIGHT,
  CT_PAD_START,
  CT_PAD_BACK,
  CT_PAD_SELECT = CT_PAD_BACK,

  CT_PAD_LEFT_THUMB,        // push the left thumb stick
  CT_PAD_LEFT_THUMB_STICK_UP,
  CT_PAD_LEFT_THUMB_STICK_DOWN,
  CT_PAD_LEFT_THUMB_STICK_LEFT,
  CT_PAD_LEFT_THUMB_STICK_RIGHT,
  CT_PAD_LEFT_THUMB_STICK_CHANGED,
  CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_X,
  CT_PAD_LEFT_THUMB_STICK_UNTRIGGERABLE_RAW_Y,

  CT_PAD_LEFT_TRIGGER,
  CT_PAD_LEFT_SHOULDER,

  CT_PAD_RIGHT_THUMB,       // push the right thumb stick
  CT_PAD_RIGHT_THUMB_STICK_UP,
  CT_PAD_RIGHT_THUMB_STICK_DOWN,
  CT_PAD_RIGHT_THUMB_STICK_LEFT,
  CT_PAD_RIGHT_THUMB_STICK_RIGHT,
  CT_PAD_RIGHT_THUMB_STICK_CHANGED,
  CT_PAD_RIGHT_THUMB_STICK_UNTRIGGERABLE_RAW_X,
  CT_PAD_RIGHT_THUMB_STICK_UNTRIGGERABLE_RAW_Y,

  CT_PAD_RIGHT_TRIGGER,
  CT_PAD_RIGHT_SHOULDER,

  CT_PAD_A,                 //generic mapping for different kind of game pads
  CT_PAD_B,
  CT_PAD_X,
  CT_PAD_Y,

  CT_PAD_SQUARE = CT_PAD_X,
  CT_PAD_CROSS = CT_PAD_A,
  CT_PAD_CIRCLE = CT_PAD_B,
  CT_PAD_TRIANGLE = CT_PAD_Y,

  CT_PAD_PC_1 = CT_PAD_X,
  CT_PAD_PC_2 = CT_PAD_A,
  CT_PAD_PC_3 = CT_PAD_B,
  CT_PAD_PC_4 = CT_PAD_Y,

  CT_PAD_ANY_KEY = CT_PAD_Y + 1,           //meta control

  CT_PAD_PC_5,
  CT_PAD_PC_6,
  CT_PAD_PC_7,
  CT_PAD_PC_8,
  CT_PAD_PC_9,
  CT_PAD_PC_10,
  CT_PAD_PC_11,
  CT_PAD_PC_12,
  CT_PAD_PC_13,
  CT_PAD_PC_14,
  CT_PAD_PC_15,
  CT_PAD_PC_16,
  CT_PAD_PC_17,
  CT_PAD_PC_18,
  CT_PAD_PC_19,
  CT_PAD_PC_20,
  CT_PAD_PC_21,
  CT_PAD_PC_22,
  CT_PAD_PC_23,
  CT_PAD_PC_24,

  CT_PAD_PC_SLIDER1_NEG,
  CT_PAD_PC_SLIDER1_POS,

  CT_PAD_PC_SLIDER2_NEG,
  CT_PAD_PC_SLIDER2_POS,

  CT_JOYSTICK_UNTRIGGERABLE_RAW_X,
  CT_JOYSTICK_UNTRIGGERABLE_RAW_Y,
  CT_JOYSTICK_UNTRIGGERABLE_RAW_Z,
  CT_JOYSTICK_UNTRIGGERABLE_RAW_X_ROTATION,
  CT_JOYSTICK_UNTRIGGERABLE_RAW_Y_ROTATION,
  CT_JOYSTICK_UNTRIGGERABLE_RAW_Z_ROTATION,

  //generic touch inputs
  CT_TOUCH_POINT_0_X,
  CT_TOUCH_POINT_0_Y,
  CT_TOUCH_POINT_0_FORCE,

  CT_TOUCH_POINT_1_X,
  CT_TOUCH_POINT_1_Y,
  CT_TOUCH_POINT_1_FORCE,

  CT_TOUCH_POINT_2_X,
  CT_TOUCH_POINT_2_Y,
  CT_TOUCH_POINT_2_FORCE,

  CT_TOUCH_POINT_3_X,
  CT_TOUCH_POINT_3_Y,
  CT_TOUCH_POINT_3_FORCE,

  CT_TOUCH_POINT_4_X,
  CT_TOUCH_POINT_4_Y,
  CT_TOUCH_POINT_4_FORCE,

  CT_TOUCH_POINT_5_X,
  CT_TOUCH_POINT_5_Y,
  CT_TOUCH_POINT_5_FORCE,

  CT_TOUCH_POINT_6_X,
  CT_TOUCH_POINT_6_Y,
  CT_TOUCH_POINT_6_FORCE,

  CT_TOUCH_POINT_7_X,
  CT_TOUCH_POINT_7_Y,
  CT_TOUCH_POINT_7_FORCE,

  CT_TOUCH_POINT_8_X,
  CT_TOUCH_POINT_8_Y,
  CT_TOUCH_POINT_8_FORCE,

  CT_TOUCH_POINT_9_X,
  CT_TOUCH_POINT_9_Y,
  CT_TOUCH_POINT_9_FORCE,

  CT_TOUCH_ANY,
  CT_TOUCH_ABS_X,
  CT_TOUCH_ABS_Y,
  CT_TOUCH_NORM_X,
  CT_TOUCH_NORM_Y,
  CT_TOUCH_ABS_DELTA_X,
  CT_TOUCH_ABS_DELTA_Y,
  CT_TOUCH_NORM_DELTA_X,
  CT_TOUCH_NORM_DELTA_Y,

  CT_TOUCH_DOUBLE_TAP,
  CT_TOUCH_TRIPLE_TAP,
  CT_TOUCH_TAP_X,
  CT_TOUCH_TAP_Y,

  CT_MOUSE_LEFT_BUTTON,
  CT_MOUSE_MIDDLE_BUTTON,
  CT_MOUSE_RIGHT_BUTTON,
  CT_MOUSE_WHEEL_UP,
  CT_MOUSE_WHEEL_DOWN,
  CT_MOUSE_WHEEL,

  // Mouse: position
  CT_MOUSE_ABS_X,
  CT_MOUSE_ABS_Y,
  CT_MOUSE_NORM_X,
  CT_MOUSE_NORM_Y,
  CT_MOUSE_ABS_DELTA_X,
  CT_MOUSE_ABS_DELTA_Y,
  CT_MOUSE_NORM_DELTA_X,
  CT_MOUSE_NORM_DELTA_Y,

  CT_MOUSE_DELTA_X_POS_DEPRECATED,
  CT_MOUSE_DELTA_Y_POS_DEPRECATED,
  CT_MOUSE_DELTA_X_NEG_DEPRECATED,
  CT_MOUSE_DELTA_Y_NEG_DEPRECATED,

  CT_MOUSE_DATA_CHANGED,

  CT_MOUSE_RAW_CURSOR_X,          ///< Client window X coordinate from Windows cursor
  CT_MOUSE_RAW_CURSOR_Y,          ///< Client window Y coordinate from Windows cursor

  //psp2/android/iOS/PC motion control
  CT_MOTION_ACCELERATION_X,
  CT_MOTION_ACCELERATION_Y,
  CT_MOTION_ACCELERATION_Z,

  //pc motion device
  CT_MOTION_PC_TILT_LR, ///< The angle in degrees the device is tilted left to right
  CT_MOTION_PC_TILT_FB, ///< The angle in degrees the device is tilted front to back
  CT_MOTION_PC_FACING_DIR, ///< The direction the device is facing in degrees (compass)

  // Generic
  CT_KB_UNKNOWN, ///< Unknown key
  CT_KB_ANYKEY = CT_KB_UNKNOWN, ///< can be passed to test any key between CT_KB_FIRST_ELEMENT and CT_KB_LAST_ELEMENT)

  CT_KB_FIRST_ELEMENT = CT_KB_UNKNOWN + 2, ///< first keyboard relevant element (there is also a CT_KB_LAST_ELEMENT)

  // Control keys
  CT_KB_LSHIFT = CT_KB_FIRST_ELEMENT,  ///< Left shift (or shift, if no right shift exists)
  CT_KB_RSHIFT,  ///< Right shift
  CT_KB_LCTRL,   ///< Left control (or control, if no right control exists)
  CT_KB_RCTRL,   ///< Right control

  CT_KB_LALT,    ///< Left alt (or alt, if no right alt exists)
  CT_KB_RALT,    ///< Right Alt / AltGr (Right Alt on German keyboards)
  CT_KB_ALTGR = CT_KB_RALT,

  CT_KB_BACKSP,  ///< Backspace (ASCII compliant)
  CT_KB_TAB,     ///< Tab (ASCII compliant)

  CT_KB_LWIN,    ///< Left Windows key
  CT_KB_RWIN,    ///< Right Windows key
  CT_KB_APPS,    ///< Windows menu (application) key  //Used to be called VGLK_MENU incorrectly
  CT_KB_ENTER,   ///< Guess what :) (ASCII compliant)
  CT_KB_LCOM,    ///< Left command (Mac)
  CT_KB_RCOM,    ///< Right command (Mac)
  CT_KB_MAC,     ///< Special mac key (will come later :)
  CT_KB_LMETA,   ///< Left meta key
  CT_KB_RMETA,   ///< Right meta key

  // Non-numerical edit and cursor movement keys
  CT_KB_INS = CT_KB_RMETA +3,   ///< Insert
  CT_KB_DEL,   ///< Delete
  CT_KB_HOME,  ///< Home
  CT_KB_END,   ///< End
  CT_KB_PGUP,  ///< PageUp
  CT_KB_PGDN,  ///< PageDown

  CT_KB_ESC,   ///< Escape (ASCII compliant)

  CT_KB_UP,    ///< Up key
  CT_KB_DOWN,  ///< Down key
  CT_KB_LEFT,  ///< Left key
  CT_KB_RIGHT, ///< Right key

  CT_KB_SPACE, ///< Space bar (ASCII compliant)

  //////////////////////////////////////////////////////////////////////////
  //The following keys might not be available on all keyboard layouts:
  // (They work on US and German layouts)
  //////////////////////////////////////////////////////////////////////////

  CT_KB_COMMA,  ///< Comma "," on american keyboard
  CT_KB_PERIOD, ///< Period "." on american keyboard
  CT_KB_MINUS,  ///< Minus on american keyboard

  CT_KB_GRAVE,  ///< Grave ` on american keyboard (same key as tilde)
  CT_KB_CONSOLE_ENABLE_KEY1 = CT_KB_GRAVE,

  /////////////////////////////////////////////
  // Standard number codes comply with ASCII codes
  /////////////////////////////////////////////
  CT_KB_0 = CT_KB_GRAVE +12,
  CT_KB_1,
  CT_KB_2,
  CT_KB_3,
  CT_KB_4,
  CT_KB_5,
  CT_KB_6,
  CT_KB_7,
  CT_KB_8,
  CT_KB_9,

  /////////////////////////////////////////////
  // Special keys
  /////////////////////////////////////////////
  CT_KB_CAPS,    ///< Caps Lock
  CT_KB_SCROLL,  ///< Scroll Lock
  CT_KB_NUM,     ///< Num Lock
  CT_KB_PRSCR,   ///< Print screen
  CT_KB_PAUSE,   ///< Pause key

  /////////////////////////////////////////////
  // Standard letter codes comply with ASCII codes
  /////////////////////////////////////////////
  CT_KB_A = CT_KB_PAUSE + 3,
  CT_KB_B,
  CT_KB_C,
  CT_KB_D,
  CT_KB_E,
  CT_KB_F,
  CT_KB_G,
  CT_KB_H,
  CT_KB_I,
  CT_KB_J,
  CT_KB_K,
  CT_KB_L,
  CT_KB_M,
  CT_KB_N,
  CT_KB_O,
  CT_KB_P,
  CT_KB_Q,
  CT_KB_R,
  CT_KB_S,
  CT_KB_T,
  CT_KB_U,
  CT_KB_V,
  CT_KB_W,
  CT_KB_X,
  CT_KB_Y,
  CT_KB_Z,

  /////////////////////////////////////////////////////////////////////
  // Numeric keypad keys. Some of these keys are doubled for your leisure :)
  /////////////////////////////////////////////////////////////////////
  CT_KB_KP_SLASH,  ///< Slash (or divide) key on the numeric keypad
  CT_KB_KP_MUL,    ///< Multiply (or asterisk) key on the numeric keypad
  CT_KB_KP_MINUS,  ///< Minus on the numeric keypad
  CT_KB_KP_PLUS,   ///< Plus...
  CT_KB_KP_ENTER, 
  CT_KB_KP_PERIOD, ///< Period (or comma or del) on the numeric keypad
  CT_KB_KP_DEL = CT_KB_KP_PERIOD,
  CT_KB_KP_0,      ///< Insert or 0
  CT_KB_KP_INS = CT_KB_KP_0,
  CT_KB_KP_1,      ///< End or 1
  CT_KB_KP_END = CT_KB_KP_1,
  CT_KB_KP_2,      ///< Down or 2
  CT_KB_KP_DOWN = CT_KB_KP_2,
  CT_KB_KP_3,      ///< PgDn or 3
  CT_KP_KP_PGDN = CT_KB_KP_3,
  CT_KB_KP_4,      ///< Left or 4
  CT_KB_KP_LEFT = CT_KB_KP_4,
  CT_KB_KP_5,      ///< Keypad 5
  CT_KB_KP_6,      ///< Right or 6
  CT_KB_KP_RIGHT = CT_KB_KP_6,
  CT_KB_KP_7,      ///< Home or 7
  CT_KB_KP_HOME = CT_KB_KP_7,
  CT_KB_KP_8,      ///< Up or 8
  CT_KB_KP_UP = CT_KB_KP_8,
  CT_KB_KP_9,      ///< PgUp or 9
  CT_KP_KP_PGUP = CT_KB_KP_9,

  CT_KB_F1 = CT_KP_KP_PGUP + 4,
  CT_KB_F2,
  CT_KB_F3,
  CT_KB_F4,
  CT_KB_F5,
  CT_KB_F6,
  CT_KB_F7,
  CT_KB_F8,
  CT_KB_F9,
  CT_KB_F10,
  CT_KB_F11,
  CT_KB_F12,

  CT_KB_EQUAL = CT_KB_F12 + 9,  ///< Equal "=" on american keyboard
  CT_KB_EQUAL_LSQBRK,         ///< Left square bracket "[" on american keyboard
  CT_KB_EQUAL_RSQBRK,         ///< Right square bracket "]" on american keyboard
  CT_KB_EQUAL_SEMICL,         ///< Semicolon ";" on american keyboard
  CT_KB_EQUAL_APOSTR,         ///< Apostrophe key "'" on american keyboard
  CT_KB_EQUAL_BACKSL,         ///< Backslash "\" on american keyboard
  CT_KB_EQUAL_SLASH,          ///< Slash "/" on american keyboard

  CT_KB_DE_SS = CT_KB_EQUAL_SLASH + 4, /// Double-S (or Beta) on german keyboard
  CT_KB_DE_ACCENT,  ///< Accent key on german keyboard
  CT_KB_DE_UE,      ///< U with umlaut on german keyboard
  CT_KB_DE_PLUS,    ///< Plus on german keyboard
  CT_KB_DE_OE,      ///< O with umlaut on german keyboard
  CT_KB_DE_AE,      ///< A with umlaut on german keyboard
  CT_KB_DE_HASH,    ///< Hash "#" on german keyboard
  CT_KB_DE_LT,      ///< Less than "<" key on german keyboard
  CT_KB_DE_CIRC,    ///< Circumflex "^"on german keyboard

  CT_KB_CONSOLE_ENABLE_KEY2 = CT_KB_DE_CIRC,

  CT_KB_LAST_ELEMENT = CT_KB_DE_CIRC,

  CT_PLATFORM_FIRST =  CT_KB_LAST_ELEMENT+1
  // This is the end of the Keyboard block (from CT_KB_UNKNOWN to CT_KB_LAST_ELEMENT) which
  // should not be changed (see above)
};


/// Deprecated. Replace by CT_MOUSE_ABS_X.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_UNTRIGGERABLE_RAW_X = CT_MOUSE_ABS_X;

/// Deprecated. Replace by CT_MOUSE_ABS_Y.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_UNTRIGGERABLE_RAW_Y = CT_MOUSE_ABS_Y;

/// Deprecated. Replace by CT_MOUSE_NORM_X.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_X = CT_MOUSE_NORM_X;

/// Deprecated. Replace by CT_MOUSE_NORM_Y.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_Y = CT_MOUSE_NORM_Y;

/// Deprecated. Replace by CT_MOUSE_ABS_DELTA_X.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_UNTRIGGERABLE_RAW_DELTA_X = CT_MOUSE_ABS_DELTA_X;

/// Deprecated. Replace by CT_MOUSE_ABS_DELTA_Y.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_UNTRIGGERABLE_RAW_DELTA_Y = CT_MOUSE_ABS_DELTA_Y;

/// Deprecated. Replace by CT_TOUCH_ABS_X.
HKV_DEPRECATED_2013_1 static const VInputControl CT_TOUCH_X = CT_TOUCH_ABS_X;

/// Deprecated. Replace by CT_TOUCH_ABS_Y.
HKV_DEPRECATED_2013_1 static const VInputControl CT_TOUCH_Y = CT_TOUCH_ABS_Y;

/// Deprecated. Replace by CT_TOUCH_ABS_DELTA_X.
HKV_DEPRECATED_2013_1 static const VInputControl CT_TOUCH_DELTA_X = CT_TOUCH_ABS_DELTA_X;

/// Deprecated. Replace by CT_TOUCH_ABS_DELTA_Y.
HKV_DEPRECATED_2013_1 static const VInputControl CT_TOUCH_DELTA_Y = CT_TOUCH_ABS_DELTA_Y;

/// Deprecated. Replace by CT_MOUSE_NORM_DELTA_X and scale the result by 2.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_DELTA_X_POS = CT_MOUSE_DELTA_X_POS_DEPRECATED;

/// Deprecated. Replace by CT_MOUSE_NORM_DELTA_Y and scale the result by 2.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_DELTA_Y_POS = CT_MOUSE_DELTA_Y_POS_DEPRECATED;

/// Deprecated. Replace by CT_MOUSE_NORM_DELTA_X and scale the result by -2.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_DELTA_X_NEG = CT_MOUSE_DELTA_X_NEG_DEPRECATED;

/// Deprecated. Replace by CT_MOUSE_NORM_DELTA_Y and scale the result by -2.
HKV_DEPRECATED_2013_1 static const VInputControl CT_MOUSE_DELTA_Y_NEG = CT_MOUSE_DELTA_Y_NEG_DEPRECATED;

/// Deprecated. Replace by CT_TOUCH_TRIPLE_TAP.
HKV_DEPRECATED_2013_1 static const VInputControl CT_TOUCH_TRIPPLE_TAP = CT_TOUCH_TRIPLE_TAP;

class IVInputDevice;


/// \brief Input manager base class, hold the mapping between the controls and their names.
class VInputManagerBase
{

  static const int s_iMapInitLen = CT_PLATFORM_FIRST;
  static const int s_iMapMaxLen = CT_PLATFORM_FIRST*2; //this is an approximation
  static const int s_iNumOfGaps = 30; //these gaps are caused by the keyboard layout...

  static int s_iNumOfAliasMappings;

  static VStrMap<int> s_map;
  static int s_pIndices[];

public:

  VBASE_IMPEXP static void Init();
  VBASE_IMPEXP static void DeInit();

  /// \brief     
  /// 	Will return the unique name of the specified control.
  ///
  /// \note
  /// 	Always use the unique names for serialization since the control number could change in future versions.
  /// 	
  /// \param iControl
  ///   Numeric value of the requested control - the integer representation of a constant defined in VInputControl.
  ///
  /// \return
  ///   The name of the of the control or NULL if the specified control does not exist.
  ///   
  /// \see  VInputControl
  VBASE_IMPEXP static const char* GetUniqueControlName(int iControl);


  /// \brief
  /// 	Will return the unique number of the specified control name.
  ///
  /// \note
  /// 	Always use the unique names for serialization since the control number could change in future versions.
  /// 	
  /// \param szControlName
  ///   String value of the requested control - the integer representation of a constant defined in VInputControl.
  ///
  /// \return
  /// 	The unique number of the specified control or -1 if there is no such control.
  /// 
  /// \see VInputControl
  /// \see GetUniqueControlName
  VBASE_IMPEXP static int GetUniqueControlIdentifier(const char * szControlName);

  /// \brief
  ///   Helper to apply a positive or negative deadzone to a given input value
  static float ApplyDeadZone(float fValue, float fDeadZone);

protected:

  VBASE_IMPEXP static bool ConsistencyCheck(int iLastPlatformControl);

  enum eControlMappingType
  {
    CONTROL_MAP_AS_PRIMARY,
    CONTROL_MAP_AS_ALIAS
  };

  //helper function to register an inputControl an ensure that it has an unique name and unique identifier
  static void MapControlToName(int iInputControlIndex, const char * szUniqueControlName, enum eControlMappingType eMapping = CONTROL_MAP_AS_PRIMARY);

};


/// \brief
///   Simple input device interface to retrieve the status of the device's controls.
/// 
/// Each instance of an input device should implement that interface in order to make the handling
/// via a VInputMap possible.
class IVInputDevice
{
private:
  VBASE_IMPEXP IVInputDevice(const IVInputDevice &other);

public:

  VBASE_IMPEXP IVInputDevice() {}
  VBASE_IMPEXP virtual ~IVInputDevice() {}


  /// \brief
  ///   Empty virtual overridable to reset the associated input device.
  VBASE_IMPEXP virtual void Reset();


  /// \brief
  ///   Empty virtual overridable to update the state of the device, called every single frame by
  ///   the VInputManager.
  /// 
  /// On Windows this expects that the message pump has been called (e.g. by
  /// VInputManagerPC::Update)
  /// 
  /// \param timeDiff
  ///   The time difference (in seconds) elapsed since the last call of the update (optional).
  VBASE_IMPEXP virtual void Update(float timeDiff = 0.0f);


  /// \brief
  ///   Empty virtual overridable which indicates whether the device is active or not.
  VBASE_IMPEXP virtual bool IsActive();


  /// \brief
  ///   Empty virtual overridable which indicates whether the device is initialized or not.
  VBASE_IMPEXP virtual bool IsInitialized();


  /// \brief
  ///   Deprecated. Use VInputManagerBase::GetUniqueControlName instead.
  ///   Empty virtual overridable which returns the name of the specified control.
  /// 
  /// \param control
  ///   Name of the requested control.
 /* HKV_DEPRECATED_2012_2 */VBASE_IMPEXP virtual const char* GetControlName(unsigned int control);

  /// \brief
  ///   Empty virtual overridable to get the float representation of the state of the requested control.
  /// 
  /// \param control
  ///   Numeric value of the requested control.
  ///
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0. See also SetDeadZone()
  ///
  /// \param bTimeScaled
  ///   Indicates if returned control value is scaled by the frame time difference.
  VBASE_IMPEXP virtual float GetControlValue(unsigned int control, float fDeadZone, bool bTimeScaled = false);
  
  /// \brief
  ///   Empty virtual overridable to get the platform dependent integer raw value of the specified control
  VBASE_IMPEXP virtual int GetRawControlValue(unsigned int control);

  /// \brief
  ///   Empty virtual overridable which returns a nice name of the input device, e.g. "Gamepad".
  VBASE_IMPEXP virtual const char* GetName();

  /// \brief
  ///   Empty virtual overridable which returns the model of the input device, e.g. "INPUT_DEVICE_PC_GAMEPAD".
  VBASE_IMPEXP virtual int GetModel();
};



/// \brief
///   Simple gamepad and joystick interface to retrieve the status of gamepad/joystick controls.
/// 
/// Each instance of a gamepad or joystick should implement that interface in order to make the
/// handling via a VInputMap and the VInputManager as joystick/gamepad possible.
/// \li This interface does not implement any IVInputDevice functions, but adds a new virtual
///   Rumble function.
class IVJoypad : public IVInputDevice
{
  public:


  /// \brief
  ///   Empty virtual overridable which performs force feedback.
  /// 
  /// Actual joy pad implementations have to implement this function.
  /// 
  /// \param speedLeft
  ///   Activate the left force feedback motor with the specified speed (full value range
  ///   [0..65535]).
  /// 
  /// \param speedRight
  ///   Activate the right force feedback motor with the specified speed (full value range
  ///   [0..65535]).
  VBASE_IMPEXP virtual void Rumble(unsigned short speedLeft, unsigned short speedRight)
  {
  }

  /// \brief
  ///   Empty virtual overridable which returns unique hardware id for joypad. The id is a string generated from the product's GUID.
  /// 
  /// Actual joypad implementations have to implement this function.
  /// 
  VBASE_IMPEXP virtual const char* GetHardwareId() {return "";}

  /// \brief
  ///   Empty virtual overridable which returns the name of the joypad.
  /// 
  /// Actual joypad implementations have to implement this function.
  /// 
  VBASE_IMPEXP virtual const char* GetProductName() {return "";}

};

/// \brief
///   Base class for a mapped control representation of an input device.
/// 
/// Provides functionality to get the current control value (0..1) and to apply dead zones.
class VMappedInput 
{
public:

  VBASE_IMPEXP VMappedInput();
  VBASE_IMPEXP virtual ~VMappedInput() {}


  /// \brief
  ///   Specifies a threshold [0..1[ for the mapped values
  /// 
  /// It defines a range [0..value] inside which the control does not react and the GetValue()
  /// function returns 0.0.
  /// 
  /// The values in the range [value..1] will be re-mapped to range [0..1].
  /// 
  /// This feature compensates for uncalibrated thumbsticks (for instance), but it is not useful
  /// for digital buttons or keyboard.
  /// 
  /// \param dz
  ///   Threshold value. Must be &lt; 1.0.
  VBASE_IMPEXP void SetDeadZone(float dz);

  /// \brief
  ///   Returns the value that has been set via SetDeadZone
  inline float GetDeadZone() const 
  {
    return m_fDeadZone;
  }

  /// \brief
  ///   Specifies a new sensitivity value for the mapped value.
  /// 
  /// This is a scaling factor that is applied to the values returned via GetValue.
  /// 
  /// This feature can be used to tweak the scaling between alternatives of the same event.
  /// The passed scaling factor is applied to the value returned by GetValue.
  /// However the scaling is not applied to mapped inputs of type 'Once' (SingleHit) or 'OncePerFrame'
  /// (SingleHitPerFrame) as these types rather have a digital button characteristic.
  /// 
  /// \param fScaling
  ///   New sensitivity value. Use 1.0 for default.
  inline void SetSensitivity(float fScaling)
  {
    m_fSensitivity = fScaling;
  }

  /// \brief
  ///   Returns the value that has been set via SetSensitivity. The default scaling is 1.0
  inline float GetSensitivity() const
  {
    return m_fSensitivity;
  }

  /// \brief
  ///   Returns true if mapped input is scaled by frame time.
  inline bool GetTimeScaled() const
  {
    return m_bTimeScaled;
  }

  /// \brief
  ///   Enable/disable the ability to scale input value by frame time.
  inline void SetTimeScaled(bool bState)
  {
    m_bTimeScaled = bState;
  }

  /// \brief
  ///   Returns the current value (in the valid range) of the mapped control.
  /// 
  /// \param chainValue
  ///   Used to chain the values of multiple controls (required for 'OncePerFrame' behavior)
  VBASE_IMPEXP virtual float GetValue(float chainValue=0);

  /// \brief
  ///   Gets the type of the mapped control. Type can be normal value, single hit or axis.
  inline VMappedValueType GetType() const 
  {
    return m_MappedValueType;
  }

private:
  VBASE_IMPEXP VMappedInput(const VMappedInput &other);

protected:
  float m_fDeadZone;
  float m_fSensitivity;
  bool m_bTimeScaled;

  VMappedValueType m_MappedValueType;

};


/// \brief
/// A mapping input that returns a constant value. Useful for emulating missing controls for combos
class VMappedConstant : public VMappedInput
{
public:
  VBASE_IMPEXP VMappedConstant(float fConstant = 0.f) { m_fConstant = fConstant;}

  /// \brief
  ///   Returns the constant value 
  /// 
  virtual float GetValue(float chainValue=0);
  
  float m_fConstant;
};


/// \brief
///   Simple mapping between control and value without special behavior.
class VMappedValue : public VMappedInput
{
public:
  VBASE_IMPEXP virtual ~VMappedValue() {}

  /// \brief
  ///   Constructor to map a input control to a simple float value.
  /// 
  /// \param inputDevice
  ///   The device of the specified control.
  /// 
  /// \param control
  ///   The control to map.
  VBASE_IMPEXP VMappedValue(IVInputDevice &inputDevice, unsigned int control);


  /// \brief
  ///   Gets the current value (in the valid range) of the mapped control.
  /// 
  /// \param chainValue
  ///   Used to chain the values of multiple controls (required for OncePerFrame behavior)
  VBASE_IMPEXP virtual float GetValue(float chainValue=0);

  /// \brief
  ///   Returns the device that is associated with this mapping (see constructor).
  inline IVInputDevice& GetInputDevice()
  {
    return m_inputDevice;
  }

  /// \brief
  ///   Returns the control that is associated with this mapping (see constructor).
  inline unsigned int GetInputDeviceControl() const
  {
    return m_uiInputDeviceControl;
  }


private:
  VBASE_IMPEXP VMappedValue(const VMappedValue &other);

protected:
  IVInputDevice &m_inputDevice;
  unsigned int m_uiInputDeviceControl;
};


/// \brief
///   Maps control as toggle switch, which is invoked once if the value differs from 0.
class VMappedOnce : public VMappedInput
{
public:
  VBASE_IMPEXP virtual ~VMappedOnce() {}


  /// \brief
  ///   Constructor to map an input control to a single hit toggle switch.
  /// 
  /// \param inputDevice
  ///   The device of the specified control.
  /// 
  /// \param uiControl
  ///   The control to map.
  /// 
  /// \param eOnceMode
  ///   hit type that determines when the event is triggered. See VMappedValueOnce::SetOnceMode.
  VBASE_IMPEXP VMappedOnce(IVInputDevice &inputDevice, unsigned int uiControl, VOnceMode eOnceMode=ONCE_ON_PRESS);

  // \brief Compatibility constructor.
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP VMappedOnce(IVInputDevice &inputDevice, unsigned int uiControl, VSingleHitType eHitType);
  

  /// \brief
  ///   Gets the current value (in the valid range) of the mapped control.
  /// 
  /// \param fChainValue
  ///   Used to chain the values of multiple controls (required for OncePerFrame behavior)
  VBASE_IMPEXP virtual float GetValue(float fChainValue=0);


  /// \brief
  ///   Returns the device that is associated with this mapping (see constructor).
  inline IVInputDevice& GetInputDevice()
  {
    return m_inputDevice;
  }

  /// \brief
  ///   Returns the control that is associated with this mapping (see constructor).
  inline unsigned int GetInputDeviceControl() const
  {
    return m_uiInputDeviceControl;
  }

  /// \brief
  ///   Sets the ramp type that defines when the event is triggered once. A 'once'
  ///   (single hit) event can be triggered when the button is pressed (default), released, or in
  ///   both cases.
  /// 
  /// \param eNewMode
  ///   enum that defines the ramp type. Can be ONCE_ON_PRESS, ONCE_ON_RELEASE, or
  ///   ONCE_ON_PRESS_AND_RELEASE (which is a bit combination of the previous flags)
  inline void SetMode(VOnceMode eNewMode)
  {
    m_eOnceMode = eNewMode;
  }

  /// \brief
  ///   Returns the mode defined via SetOnceMode. The default mode is ONCE_ON_PRESS
  inline VOnceMode GetMode() const
  {
    return m_eOnceMode;
  }

  /// \brief
  ///   Compatibility method. Sets the ramp type that defines when the event is triggered for this single hit type. A
  ///   single hit event can be triggered when the button is pressed (default), released, or in
  ///   both cases.
  /// 
  /// \param eNewType
  ///   enum that defines the ramp type. Can be SINGLE_HIT_ONPRESSED, SINGLE_HIT_ONRELEASE, or
  ///   SINGLE_HIT_ONPRESSEDANDRELEASE (which is a bit combination of the previous flags)
  HKV_DEPRECATED_2012_2 inline void SetSingleHitType(VSingleHitType eNewType)
  {
    m_eOnceMode = (VOnceMode)eNewType;
  }

  /// \brief
  ///   Compatibility method. Returns the mode defined via SetSingleHitType. The default mode is SINGLE_HIT_ONPRESSED
  HKV_DEPRECATED_2012_2 inline VSingleHitType GetSingleHitType() const
  {
    return (VSingleHitType)m_eOnceMode;
  }

private:
  VBASE_IMPEXP VMappedOnce(const VMappedOnce &other);

protected:
  IVInputDevice &m_inputDevice;
  unsigned int m_uiInputDeviceControl;
  bool m_bQueried;
  VOnceMode m_eOnceMode;
};

//compatibility typedef
typedef VMappedOnce VMappedValueSingleHit;


/// \brief
///   Map control as toggle switch, which is invoked once if the value differs from 0 (The value is
///   constant inside a frame).
/// 
/// The value of the control is constant inside a frame. It is possible to query the state for
/// multiple times inside a frame without a change to the control value.
class VMappedOncePerFrame : public VMappedInput
{
public:
  VBASE_IMPEXP virtual ~VMappedOncePerFrame() {}


  /// \brief
  ///   Constructor to map an input control to a single hit toggle switch (The switch has a constant
  ///   value inside a frame).
  /// 
  /// \param inputDevice
  ///   The device of the specified control.
  /// 
  /// \param uiControl
  ///   The control to map.
  /// 
  /// \param eOnceMode
  ///   hit type that determines when the event is triggered. See SetMode.
  VBASE_IMPEXP VMappedOncePerFrame(IVInputDevice &inputDevice, unsigned int uiControl, VOnceMode eOnceMode=ONCE_ON_PRESS);
  
  // \brief Compatibility constructor.
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP VMappedOncePerFrame(IVInputDevice &inputDevice, unsigned int uiControl, VSingleHitType eHitType);

  /// \brief
  ///   Gets the current value (in the valid range) of the mapped control.
  /// 
  /// \param fChainValue
  ///   Used to chain the values of multiple controls (required for OncePerFrame behavior).
  VBASE_IMPEXP virtual float GetValue(float fChainValue=0);


  /// \brief
  ///   Returns the device that is associated with this mapping (see constructor).
  inline IVInputDevice& GetInputDevice()
  {
    return m_inputDevice;
  }


  /// \brief
  ///   Returns the control that is associated with this mapping (see constructor).
  inline unsigned int GetInputDeviceControl() const
  {
    return m_uiInputDeviceControl;
  }

  /// \brief
  ///   Sets the ramp type that defines when the event is triggered once. A once
  ///   (single hit) event can be triggered when the button is pressed (default), released, or in
  ///   both cases.
  /// 
  /// \param eNewMode
  ///   enum that defines the ramp type. Can be ONCE_ON_PRESS, ONCE_ON_RELEASE, or
  ///   ONCE_ON_PRESS_AND_RELEASE (which is a bit combination of the previous flags)
  inline void SetMode(VOnceMode eNewMode)
  {
    m_eOnceMode = eNewMode;
  }

  /// \brief
  ///   Returns the mode defined via SetOnceMode. The default mode is ONCE_ON_PRESS
  inline VOnceMode GetMode() const
  {
    return m_eOnceMode;
  }

  /// \brief
  ///   Compatibility method. Sets the ramp type that defines when the event is triggered for this single hit type. A
  ///   single hit event can be triggered when the button is pressed (default), released, or in
  ///   both cases.
  /// 
  /// \param eNewType
  ///   enum that defines the ramp type. Can be SINGLE_HIT_ONPRESSED, SINGLE_HIT_ONRELEASE, or
  ///   SINGLE_HIT_ONPRESSEDANDRELEASE (which is a bit combination of the previous flags)
  HKV_DEPRECATED_2012_2 inline void SetSingleHitType(VSingleHitType eNewType)
  {
    m_eOnceMode = (VOnceMode)eNewType;
  }

  /// \brief
  ///   Compatibility method. Returns the mode defined via SetSingleHitType. The default mode is SINGLE_HIT_ONPRESSED
  HKV_DEPRECATED_2012_2 inline VSingleHitType GetSingleHitType() const
  {
    return (VSingleHitType)m_eOnceMode;
  }

private:
  VBASE_IMPEXP VMappedOncePerFrame(const VMappedOncePerFrame &other);

protected:
  IVInputDevice &m_inputDevice;
  unsigned int m_uiInputDeviceControl;
  bool m_bStateLastFrame;
  float m_fLastResult;
  int m_iFrameQueried;
  VOnceMode m_eOnceMode;
};

//compatibility typedef
typedef VMappedOncePerFrame VMappedValueSingleHitPerFrame;


/// \brief
///   Maps two controls into an axis with a positive and negative range.
class VMappedAxis : public VMappedInput
{
public:
  VBASE_IMPEXP virtual ~VMappedAxis() {}


  /// \brief
  ///   Constructor to map two controls as axis.
  /// 
  /// \param inputDevice
  ///   The device of the specified control.
  /// 
  /// \param uiControlNegative
  ///   The control which is responsible for the negative deflection along the axis.
  /// 
  /// \param uiControlPositive
  ///   The control which is responsible for the positive deflection along the axis.
  VBASE_IMPEXP VMappedAxis(IVInputDevice &inputDevice, unsigned int uiControlNegative, unsigned int uiControlPositive);


  /// \brief
  ///   Gets the current value (in the valid range) of the mapped control.
  /// 
  /// \param fChainValue
  ///   Used to chain the values of multiple controls (required for OncePerFrame behavior).
  VBASE_IMPEXP virtual float GetValue(float fChainValue=0);

private:
  VBASE_IMPEXP VMappedAxis(const VMappedAxis &other);

protected:
  IVInputDevice &m_inputDevice;
  unsigned int m_uiControlNegative;
  unsigned int m_uiControlPositive;

};


/// \brief
///   Map control to check if a key is pressed or hold
///   A key is considered 'hold' when is pressed for an amount of time greater or equal than a time threshold (configurable).
///   Otherwise the key is 'pressed'. Do not call two times in same frame to GetValue method, instead use follow client code
///   to get the trigger state:
///
///   float triggetState = pInput->GetTriggetState( MY_HOLD_KEY );
///   if ( PRESSHOLD_ISPRESSED( triggetState ) )
///     // do when is pressed
///   else if ( PRESSHOLD_ISHOLD( triggetState ) )
///     // do when is hold
class VMappedPressHold : public VMappedInput
{
public:
  VBASE_IMPEXP virtual ~VMappedPressHold() {}


  /// \param inputDevice
  ///   The device of the specified control.
  /// 
  /// \param uiControl
  ///   The control to map.
  /// 
  /// \param fHoldTime
  ///   Time in seconds to consider the key press as 'hold'.
  VBASE_IMPEXP VMappedPressHold(IVInputDevice &inputDevice, unsigned int uiControl, float fHoldTime = 0.3f );


  /// \brief 
  ///   Returns the current value (in the valid range) of the mapped control.
  ///
  /// \param fChainValue Used to chain the values of multiple controls (required for SingleHitPerFrame behavior).
  VBASE_IMPEXP virtual float GetValue(float fChainValue=0);


  /// \brief Set the time to consider a press key as 'hold'.
  /// \param fHoldTime The time to check when a key is hold.
  inline void SetHoldTime(float fHoldTime)
  {
    VASSERT( fHoldTime >= 0.0f );
    m_fHoldTime = fHoldTime;
  }

  /// \brief Returns the time for a 'hold' state
  inline float GetHoldTime( )
  {
    return m_fHoldTime;
  }

private:
  VBASE_IMPEXP VMappedPressHold(const VMappedPressHold &other);

protected:
  IVInputDevice &m_inputDevice;
  unsigned int m_uiInputDeviceControl;
  float m_fHoldTime;    ///< Time to consider a hold state
  double m_dPressTime;   ///< Time when the key was pressed
  bool m_bQueried;      ///< Last key state
};

typedef VMappedPressHold VMappedValuePressHold;

/// \brief
///  Helper struct to setup input options like DeadZones, OnceBehavior, TimeScaling, etc. for triggers.
///
///  Configure the structure for your needs and use it when mapping the triggers with MapTrigger and MapTriggerAxis.
///  Please note that some settings are mutual exclusive. A control with 'once' (single hit) behavior cannot be
///  combined with hold-time! Also touch input devices currently do not support hold-time.
///
/// \see MapTrigger
/// \see MapTriggerAxis
VBASE_IMPEXP typedef struct VInputOptions
{
  /// \brief Constructor.
  VInputOptions(bool bTimeScaled=false, bool bOnce=false,
                bool bOncePerFrame=false, float fDeadZone=0.0f,
                float fHoldTime=0.0f, VOnceMode eOnceMode = ONCE_ON_PRESS,
                float fSensitivity=1.0f, int iAlternative = -1) :
      m_bTimeScaled(bTimeScaled),
      m_bOnce(bOnce),
      m_bOncePerFrame(bOncePerFrame),
      m_fDeadZone(fDeadZone),
      m_fHoldTime(fHoldTime),
      m_fSensitivity(fSensitivity),
      m_eOnceMode(eOnceMode),
      m_iAlternative(iAlternative)
  {}

  /// \brief Factory method to create a ready configured VInputOptions struct for time-scaled mapped triggers.
  static VInputOptions TimeScale(int iAlternative = -1)
    { return VInputOptions(true, false, false, 0.0f, 0.0f, ONCE_ON_PRESS, 1.0f, iAlternative); };

  /// \brief Factory method to create a ready configured VInputOptions struct with a specific dead zone.
  static VInputOptions DeadZone(float fDeadZone = 0.2f, bool bTimeScale=false, int iAlternative = -1)
    { return VInputOptions(bTimeScale, false, false, fDeadZone, 0.0f, ONCE_ON_PRESS, 1.0f, iAlternative); };

  /// \brief Factory method to create a ready configured VInputOptions for a Trigger with a 'Hold-Time'.
  static VInputOptions HoldTime(float fHoldTime = 0.3f, int iAlternative = -1)
    { return VInputOptions(false, false, false, 0.0f, fHoldTime, ONCE_ON_PRESS, 1.0f, iAlternative); };

  /// \brief Factory method to create a ready configured VInputOptions for a Trigger reacting just once when a key is pressed.
  static VInputOptions Once(VOnceMode eHitType=ONCE_ON_PRESS, int iAlternative = -1)
    { return VInputOptions(false, true, false, 0.0f, 0.0f, eHitType, 1.0f, iAlternative); };

  /// \brief Factory method to create a ready configured VInputOptions for a Trigger reacting just once per frame when a key is pressed.
  static VInputOptions OncePerFrame(VOnceMode eHitType=ONCE_ON_PRESS, int iAlternative = -1)
    { return VInputOptions(false, true, true, 0.0f, 0.0f, eHitType, 1.0f, iAlternative); };

  /// \brief Factory method to create a ready configured VInputOptions with a specific alternative within your input map.
  static VInputOptions Alternative(int iAlternative, bool bOnce=false, VOnceMode eOnceMode = ONCE_ON_PRESS)
    { return VInputOptions(false, bOnce, false, 0.0f, 0.0f, eOnceMode, 1.0f, iAlternative); };

  /// \brief Factory method to create a ready configured VInputOptions with a specific sensitivity for the used control.
  static VInputOptions Sensitivity(float fSensitivity, int iAlternative = -1)
    { return VInputOptions(false, false, false, 0.0f, 0.0f, ONCE_ON_PRESS, fSensitivity, iAlternative); };

  bool m_bTimeScaled;     ///< Set to true to scale the trigger value with the frame time delta.
  bool m_bOnce;           ///< Set to true if your trigger should be set just once (set state is consumed after a query).
  bool m_bOncePerFrame;   ///< Set to true if your 'once' trigger should stay active during the whole frame (requires m_bOnce to be true).

  float m_fDeadZone;      ///< Create a dead zone, e.g. Trigger gets active at a min level of 0.3 (values may range from 0 to 1).
  float m_fHoldTime;      ///< Setup a minimum hold time to get set. Incompatible with 'once' behavior (m_bOnce = true).
  float m_fSensitivity;   ///< Define the sensitivity of the control (e.g. less influence to the trigger than another control, mapped to the same trigger).

  VOnceMode m_eOnceMode;   ///< Configure when the 'once' event is triggered: ONCE_ON_PRESS, ONCE_ON_RELEASE, ONCE_ON_PRESS_AND_RELEASE

  /// \brief Set the behavior for a specific alternative in your input map, e.g. if you would like to over write an existing alternative.
  ///   It is a 0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor of your input map. The default value -1, is used
  ///   to map the control to the next unused alternative.
  int m_iAlternative;

} VInputOptions;

/// \brief
///   Interface to get the state of a control independent of the source (device).
/// 
/// See VInputMap class for implementation.
class IVInputMap
{
public:
  inline virtual ~IVInputMap(){;}


  /// \brief
  ///   Abstract Method to get the current value of the defined trigger (mapped control values).
  virtual float GetTrigger(int iTriggerIndex) = 0;
};

class VTouchArea;
class IVMultiTouchInput;

/// \brief
///   Interface to get the state of a control independent of the source (device).
/// 
/// An input map is a table that maps the result of one or more input control to a trigger ID.
/// 
/// For instance, the result of the left mouse button and the keyboard SPACE key can be mapped to
/// the same trigger ID which can be queried inside the game application.
/// 
/// This abstracts away the actual key binding inside the game code as the code only works with
/// IDs.
class VInputMap : public IVInputMap, public VRefCounter
{  
public:


  /// \brief
  ///   Constructor to create a new blank input map with the specified dimension.
  /// 
  /// Internally it creates a table of size inputs*alternatives with mapped inputs.
  /// 
  /// The dimension of the trigger map can be overestimated to fit all triggers defined in an
  /// application.
  /// 
  /// \param iNumInputs
  ///   The maximum number of possible triggers inside this map, that means a trigger ID must be in
  ///   the range [0..inputs-1].
  /// 
  /// \param iNumAlternatives
  ///   The maximum number of possible controls per trigger, which are mapped to one trigger.
  VBASE_IMPEXP VInputMap(int iNumInputs, int iNumAlternatives);

  VBASE_IMPEXP virtual ~VInputMap();

  /// \brief
  ///   Returns a triggerID/alternative pair as an instance of type VMappedInput (base class, can
  ///   be casted).
  VBASE_IMPEXP VMappedInput* GetMapping(int iTriggerIndex, int iAlternative);

  /// \brief
  ///   Sets a triggerID/alternative pair directly
  ///
  /// This function just stores the passed VMappedInput pointer reference in the map. The entry will be deleted at de-init time.
  /// Accordingly an entry must be passed that has been created with the new operator. Sharing of entries is also not
  /// possible.
  ///
  /// \param iTriggerIndex
  ///   The id number of the trigger to add a new input
  /// 
  /// \param iAlternative
  ///   0..max alternatives-1: index of the trigger control alternative. 
  ///   -1: the next unused alternative.
  /// 
  /// \param pMapped
  ///   pointer to the reference to set at specified map location
  ///
  /// \returns
  ///   the alternative index or -1 if adding failed (e.g. no free alternative left, then you have to create a bigger input map - more alternatives)
  VBASE_IMPEXP int SetMapping(int iTriggerIndex, int iAlternative, VMappedInput* pMapped);
  

  /// \brief
  ///   Returns the current value of the defined trigger (maximum/sum over all mapped control
  ///   values).
  /// 
  /// The function loops through all alternative controls assigned to this trigger ID and returns
  /// the combined result.
  /// 
  /// The way the result is determined is defined by the control's type. I.e. if the alternative is
  /// an axis it takes the sum, otherwise it takes the maximum. This contributes to the logical OR
  /// of digital buttons.
  ///
  /// \param iTriggerIndex
  ///   The trigger id to get the state from
  VBASE_IMPEXP virtual float GetTrigger(int iTriggerIndex);
  
  /// \brief Default input options without any specif behavior (trigger is active as long as a key is pressed)
  VBASE_IMPEXP static const VInputOptions DEFAULT_INPUT_OPTIONS;
  
  /// \brief Map a specific key/button to a trigger. You can also replace an existing mapping.
  /// 
  /// \param iTriggerIndex
  ///   The numerical representation of the trigger to assign a new/alternative control.
  ///   You can query the state of the mapped control via the trigger id.
  /// 
  /// \param inputDevice
  ///   The device that provides the specified control. E.g. VInputManagerPC::Keyboard() or it's macro V_PC_KEYBOARD.
  /// 
  /// \param uiControl
  ///   The control (key, button, position, etc.) which gets mapped to the specified trigger.
  /// 
  /// \param options
  ///   The VInputOptions structure let you specify the behavior of the control/trigger. E.g. if your trigger
  ///   should only be set once when pressed set it's member m_bOnce to true. As alternative you can also use the
  ///   supplied factory method VInputOptions::Once() creating a proper structure for you. Please take a look at
  ///   the provided sample code.
  ///
  /// \returns
  ///   The 0 based index of the assigned alternative within the input map. The value is bigger than 0 on success.
  ///
  /// \par Example
  ///   \code
  ///     //map a control without a specific behavior (active as long as pressed):
  ///     m_pInputMap->MapTrigger(CHARACTER_MOVE_FORWARD, V_KEYBOARD, CT_KB_UP);
  ///
  ///     //map a control with 'once' behavior:
  ///     m_pInputMap->MapTrigger(CHARACTER_ATTACK, V_KEYBOARD, CT_KB_ENTER, VInputOptions::Once());
  ///
  ///     //map a control with 'once' behavior which is active for the whole frame and reacts on press and release:
  ///     m_pInputMap->MapTrigger(CHARACTER_ATTACK, V_KEYBOARD, CT_KB_KP_ENTER, VInputOptions::OncePerFrame(ONCE_ON_PRESS_AND_RELEASE));
  ///
  ///     //apply a dead zone and let the value be time scaled:
  ///     m_pInputMap->MapTrigger(CHARACTER_RUN, V_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f, true));
  ///
  ///     //map a time scaled control to a alternative 1:
  ///     m_pInputMap->MapTrigger(CHARACTER_RUN, V_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::TimeScale(1));
  ///
  ///     //assign a default control to alternative 2:
  ///     m_pInputMap->MapTrigger(CHARACTER_RUN, V_KEYBOARD, CT_KB_UP, VInputOptions::Alternative(2));
  ///
  ///     //configure the control to be active after 0.4 sec:
  ///     m_pInputMap->MapTrigger(CHARACTER_RUN, V_KEYBOARD, CT_KB_KP_8, VInputOptions::HoldTime(0.4f));
  ///
  ///     //manually configure the structure:
  ///     VInputOptions inputOptions(false, true, true, 0, 0, ONCE_ON_PRESS, 1.0f, 2); //once per frame, on key press, sensitivity 1.0, overwrite alternative 2
  ///     m_pInputMap->MapTrigger(CHARACTER_RUN, V_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN, inputOptions);
  ///
  ///   \endcode
  ///
  /// \note
  ///   There are several convenience macros for the input devices:
  ///    - For PS3:
  ///      V_PS3_PAD(x): VInputManagerPS3::GetPad(x)
  ///      V_PSP2_PAD(x): VInputManagerPSP2::GetPad(x)
  ///    - For XBox360:
  ///      V_XENON_PAD(x): VInputManagerXenon::GetPad(x)
  ///    - For WiiU:
  ///      V_WIIU_PAD(x): VInputManagerWiiU::GetPad(x)
  ///      V_WIIU_REMOTE(x): VInputManagerWiiU::GetRemote(x)
  ///    - For PC:
  ///      V_PC_MOUSE: VInputManagerPC::GetMouse()
  ///      V_PC_KEYBOARD: VInputManagerPC::GetKeyboard()
  ///      V_PC_PAD(x): VInputManagerPC::GetPad(x)
  ///    - Generic:
  ///      V_MOUSE: VInputManager::GetMouse()
  ///      V_KEYBOARD: VInputManager::GetKeyboard()
  ///      V_PAD(x): VInputManager::GetPad(x)
  ///   Feel free to create your own macros since not every device is covered here.
  ///   
  /// \see VInputOptions
  /// \see MapTriggerAxis
  VBASE_IMPEXP int MapTrigger(int iTriggerIndex, IVInputDevice &inputDevice, unsigned int uiControl, const VInputOptions &options = VInputMap::DEFAULT_INPUT_OPTIONS);

  /// \brief Map a specific key/button to a trigger. You can also replace an existing mapping.
  /// 
  /// \param iTriggerIndex
  ///   The numerical representation of the trigger to assign a new/alternative control.
  ///   You can query the state of the mapped control via the trigger id.
  /// 
  /// \param pArea
  ///   The touch area which to map. The touch area already contains the device information.
  /// 
  /// \param uiControl
  ///   The control (key, button, position, etc.) which gets mapped to the specified trigger.
  /// 
  /// \param options
  ///   The VInputOptions structure let you specify the behavior of the control/trigger. E.g. if your trigger
  ///   should only be set once when pressed set it's member m_bOnce to true. As alternative you can also use the
  ///   supplied factory method VInputOptions::Once() creating a proper structure for you. Please take a look at
  ///   the provided sample code.
  ///
  /// \returns
  ///   The 0 based index of the assigned alternative within the input map. The value is bigger than 0 on success.
  ///
  /// \par Example
  ///   \code
  ///     //map any touch for the 'forward' area to the trigger CAMERA_MOVE_FORWARD:
  ///     m_pInputMap->MapTrigger(CAMERA_MOVE_FORWARD, pForwardArea, CT_TOUCH_ANY);
  ///
  ///     //map any touch for the 'cam' area to the trigger CAMERA_MOVE_FORWARD and scale it with the frame time delta:
  ///     m_pInputMap->MapTrigger(CAMERA_TURN_LEFT, pCamArea, CT_TOUCH_ANY, VInputOptions::TimeScale());
  ///
  ///     //map any touch for the 'shhot' area to the trigger PLAYER_SHOOT and trigger only once:
  ///     m_pInputMap->MapTrigger(PLAYER_SHOOT, pShootArea, CT_TOUCH_ANY, VInputOptions::Once());
  ///
  ///   \endcode
  ///   
  /// \see VInputOptions
  VBASE_IMPEXP int MapTrigger(int iTriggerIndex, VTouchArea* pArea, unsigned int uiControl, const VInputOptions &options = VInputMap::DEFAULT_INPUT_OPTIONS);


  /// \brief Establish an axis mapping out of two controls and map it to a trigger.
  /// 
  ///   An axis needs two controls that are responsible for positive resp. negative deflection.
  ///   For instance these controls can be cursor right/left keys to emulate the mouse delta axis.
  ///   This function can also be used to replace an existing mapping.
  /// 
  /// \param iTriggerIndex
  ///   The numerical representation of the trigger to assign a new/alternative control.
  ///   You can query the state of the mapped control via the trigger id.
  /// 
  /// \param inputDevice
  ///   The device that provides the specified control. E.g. VInputManagerPC::Keyboard() or it's macro V_PC_KEYBOARD.
  /// 
  /// \param uiControlNegative
  ///   The control which is associated with the negative deflection along the axis.
  /// 
  /// \param uiControlPositive
  ///   The control which is associated with the positive deflection along the axis.
  /// 
  /// \param options
  ///   The VInputOptions structure let you specify the behavior of the control/trigger. Please take a look at
  ///   the provided sample code and the documentation of VInputOptions.
  ///
  /// \returns
  ///   The 0 based index of the assigned alternative within the input map. The value is bigger than 0 on success.
  ///
  /// \par Example
  ///   \code
  ///     //map the mouse delta values as axis:
  ///     m_pInputMap->MapTriggerAxis(HORIZONTAL_LOOK, V_MOUSE, CT_MOUSE_DELTA_X_NEG, CT_MOUSE_DELTA_X_POS);
  ///     m_pInputMap->MapTriggerAxis(VERTICAL_LOOK,   V_MOUSE, CT_MOUSE_DELTA_Y_NEG, CT_MOUSE_DELTA_Y_POS);
  ///
  ///     //map the analog pad to the same trigger (so you can use mouse or pad for the same trigger) and apply a dead zone with time scaling:
  ///     m_pInputMap->MapTriggerAxis(HORIZONTAL_LOOK, V_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  ///     m_pInputMap->MapTriggerAxis(VERTICAL_LOOK,   V_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f, true));
  ///
  ///   \endcode
  ///
  /// \note
  ///   There are several convenience macros for the input devices:
  ///    - For PS3:
  ///      V_PS3_PAD(x): VInputManagerPS3::GetPad(x)
  ///      V_PSP2_PAD(x): VInputManagerPSP2::GetPad(x)
  ///    - For XBox360:
  ///      V_XENON_PAD(x): VInputManagerXenon::GetPad(x)
  ///    - For WiiU:
  ///      V_WIIU_PAD(x): VInputManagerWiiU::GetPad(x)
  ///      V_WIIU_REMOTE(x): VInputManagerWiiU::GetRemote(x)
  ///    - For PC:
  ///      V_PC_MOUSE: VInputManagerPC::GetMouse()
  ///      V_PC_KEYBOARD: VInputManagerPC::GetKeyboard()
  ///      V_PC_PAD(x): VInputManagerPC::GetPad(x)
  ///    - Generic:
  ///      V_MOUSE: VInputManager::GetMouse()
  ///      V_KEYBOARD: VInputManager::GetKeyboard()
  ///      V_PAD(x): VInputManager::GetPad(x)
  ///   Feel free to create your own macros since not every device is covered here.
  ///   
  /// \see VInputOptions
  /// \see MapTrigger
  VBASE_IMPEXP int MapTriggerAxis(int iTriggerIndex, IVInputDevice &inputDevice, unsigned int uiControlNegative, unsigned int uiControlPositive, const VInputOptions &options = VInputMap::DEFAULT_INPUT_OPTIONS);
  

  /// \brief
  ///   Helper function to convert an existing mapping entry to once mode (single hit).
  /// 
  ///   This will iterate over the complete input map, just use it if really needed.
  ///   Please note that it will not work for touch controls.
  /// 
  /// \param iTriggerIndex
  ///   The id number of the trigger.
  /// 
  /// \param iAlternative
  ///   The control alternative to convert.
  ///   
  /// \param bPerFrame
  ///   [\b optional] Set to true to convert to 'OncePerFrame' behavior.
  VBASE_IMPEXP bool ConvertToOnce(int iTriggerIndex, int iAlternative, bool bPerFrame = false);

  /// \brief
  ///   Helper function to convert an existing mapping entry to normal value mode.
  /// 
  ///   This will iterate over the complete input map, just use it if really needed.
  ///   Please note that it will not work for touch controls.
  /// 
  /// \param iTriggerIndex
  ///   The id number of the trigger.
  /// 
  /// \param iAlternative
  ///   The control alternative to convert.
  VBASE_IMPEXP bool ConvertToValue(int iTriggerIndex, int iAlternative);

  /// \brief
  ///   Helper function to convert an existing mapping entry to press-hold input
  /// 
  ///   This will iterate over the complete input map, just use it if really needed.
  ///   Please note that it will not work for touch controls.
  /// 
  /// \param iTriggerIndex
  ///   The id number of the trigger.
  /// 
  /// \param iAlternative
  ///   The control alternative to convert.
  ///
  /// \param fHoldTime
  ///   Time to consider the 'hold' state. Default=0.3f
  VBASE_IMPEXP bool ConvertToHold(int iTriggerIndex, int iAlternative, float fHoldTime=0.3f );

  /// \brief
  ///   Unmap a single input control, i.e. remove a alternative from the specified trigger ID from
  ///   the mapping table.
  /// 
  /// \param iTriggerIndex
  ///   The id number of the trigger.
  /// 
  /// \param iAlternativeIndex
  ///   The control to remove.
  VBASE_IMPEXP bool UnmapInput(int iTriggerIndex, int iAlternativeIndex);


  /// \brief
  ///   Unmap all input controls of a trigger, i.e. remove all alternatives associated with the
  ///   trigger.
  /// 
  /// \param iTriggerIndex
  ///   The id number of the trigger which gets cleared.
  VBASE_IMPEXP bool UnmapInput(int iTriggerIndex);

  /// \brief
  ///   Unmaps all input controls of all triggers.
  ///
  /// \param iNumInputs
  ///   The maximum number of possible triggers inside this map, that means a trigger ID must be in
  ///   the range [0..inputs-1]. If set to < 0, the current number of inputs is maintained
  /// 
  /// \param iNumAlternatives
  ///   The maximum number of possible controls per trigger, which are mapped to one trigger. If
  ///   set to < 0, the current number of alternatives is maintained.
  VBASE_IMPEXP void Clear(int iNumInputs=-1, int iNumAlternatives=-1);


  ///
  /// @name Deprecated
  /// @{
  ///

  /// \brief
  ///   Deprecated, please use GetTrigger instead. Returns the current value of the defined trigger
  ///   (maximum/sum over all mapped control values).
  /// 
  /// The function loops through all alternative controls assigned to this trigger ID and returns
  /// the combined result.
  /// 
  /// The way the result is determined is defined by the control's type. I.e. if the alternative is
  /// an axis it takes the sum, otherwise it takes the maximum. This contributes to the logical OR
  /// of digital buttons.
  ///
  /// \param index
  ///   The trigger id to get the state from
  ///
  /// \see GetTrigger
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP float GetTriggerState(int index);

  /// \brief
  ///   Deprecated, please use MapTrigger instead. Create a simple mapping between a control and a trigger.
  /// 
  /// This function can also be used to replace an existing mapping.
  /// 
  /// \param index
  ///   The number of the trigger to assign a new/alternative control.
  /// 
  /// \param alternative
  ///   0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor. If equal -1, the
  ///   control will be mapped to the next unused alternative.
  /// 
  /// \param inputDevice
  ///   The device that provides the specified control.
  /// 
  /// \param control
  ///   The control which gets mapped to the specified trigger.
  /// 
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0. See also SetDeadZone()
  ///
  /// \param usedAlternative
  ///   returns the alternative used when automatic mapping was enabled 1(alternative == -1).
  ///
  /// \param bTimeScaled
  ///   Indicates that for this mapped control, the returned value will be scaled by the time difference.
  ///
  /// \see MapTrigger
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapInputDeviceControlValue(int index, int alternative, IVInputDevice &inputDevice, unsigned int control, float fDeadZone=0.2f, int* usedAlternative = NULL, bool bTimeScaled = false);

  /// \brief
  ///   Deprecated, please use MapTrigger instead. Create toggle switch style mapping between a control and a trigger.
  /// 
  /// This function can also be used to replace an existing mapping.
  /// 
  /// In this single hit mode, the result is always 0.0f or 1.0f. Furthermore, getting the result
  /// once resets the value and returns 0.0 until the control is released and pressed again.
  /// 
  /// To preserve the result over a full frame, use the MapInputDeviceControlValueSingleHitPerFrame
  /// function instead.
  /// 
  /// \param index
  ///   The number of the trigger to assign a new/alternative control.
  /// 
  /// \param alternative
  ///   0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor. If equal -1, the
  ///   control will be mapped to the next unused alternative.
  /// 
  /// \param inputDevice
  ///   The device that provides the specified control.
  /// 
  /// \param control
  ///   The control which gets mapped to the specified trigger.
  /// 
  /// \param eHitType
  ///   hit type that determines when the event is triggered. See SetMode
  /// 
  /// \param usedAlternative
  ///   returns the alternative used when automatic mapping was enabled 1(alternative == -1).
  ///
  /// \see MapTrigger
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapInputDeviceControlValueSingleHit(int index, int alternative, IVInputDevice &inputDevice, unsigned int control, VSingleHitType eHitType=SINGLE_HIT_ONPRESSED, int* usedAlternative = NULL);

  /// \brief
  ///   Deprecated, please use MapTrigger instead. Create toggle switch style mapping between a control and a trigger.
  /// 
  /// This function can also be used to replace an existing mapping.
  /// 
  /// In this single hit mode, the result is always 0.0f or 1.0f. 
  /// 
  /// Unlike MapInputDeviceControlValueSingleHit the result gets reset at the end of a frame thus
  /// preserving the same result for every call inside a frame.
  /// 
  /// \param index
  ///   The number of the trigger to assign a new/alternative control.
  /// 
  /// \param alternative
  ///   0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor. If equal -1, the
  ///   control will be mapped to the next unused alternative.
  /// 
  /// \param inputDevice
  ///   The device that provides the specified control.
  /// 
  /// \param control
  ///   The control which gets mapped to the specified trigger.
  /// 
  /// \param eHitType
  ///   hit type that determines when the event is triggered. See SetMode
  /// 
  /// \param usedAlternative
  ///   returns the alternative used when automatic mapping was enabled 1(alternativ == -1).
  ///
  /// \see MapTrigger
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapInputDeviceControlValueSingleHitPerFrame(int index, int alternative, IVInputDevice &inputDevice, unsigned int control, VSingleHitType eHitType=SINGLE_HIT_ONPRESSED, int* usedAlternative = NULL);


  /// \brief
  ///   Deprecated, please use MapTriggerAxis instead. Establish an axis mapping out of two controls and map it to a trigger.
  /// 
  /// An axis needs two controls that are responsible for positive resp. negative deflection.
  /// 
  /// For instance these controls can be cursor right/left keys to emulate the mouse delta axis.
  /// 
  /// This function can also be used to replace an existing mapping.
  /// 
  /// \param index
  ///   The number of the trigger.
  /// 
  /// \param alternative
  ///   0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor. If equal -1, the
  ///   control will be mapped to the next unused alternative.
  /// 
  /// \param inputDevice
  ///   The device that provides the specified control.
  /// 
  /// \param controlNegative
  ///   The control which is associated with the negative deflection along the axis.
  /// 
  /// \param controlPositive
  ///   The control which is associated with the positive deflection along the axis.
  /// 
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0. See also SetDeadZone()
  ///
  /// \param usedAlternative
  ///   Returns the alternative used when automatic mapping was enabled 1(alternative == -1).
  ///
  /// \param bTimeScaled
  ///   Indicates that for this mapped control, the returned value will be scaled by the time difference.
  ///
  /// \see MapTriggerAxis
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapInputDeviceControlAxis(int index, int alternative, IVInputDevice &inputDevice, unsigned int controlNegative, unsigned int controlPositive, float fDeadZone=0.0f, int* usedAlternative = NULL,bool bTimeScaled = false);

  /// \brief
  ///   Deprecated, please use MapTrigger instead. Create a pressed versus hold key mapping. 
  /// 
  /// This function can also be used to replace an existing mapping.
  /// 
  /// A 'hold' key is when a key is down for a configurable amount of time. Otherwise if we release 
  /// the key before reach that amount of time, the key is considered 'pressed'.
  /// 
  /// 
  /// \param index
  ///   The number of the trigger to assign a new/alternative control.
  /// 
  /// \param alternative
  ///   0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor. If equal -1, the
  ///   control will be mapped to the next unused alternative.
  /// 
  /// \param inputDevice
  ///   The device that provides the specified control.
  /// 
  /// \param control
  ///   The control which gets mapped to the specified trigger.
  /// 
  /// \param holdTime
  ///   Amount of time to consider the key pressed as a hold state.
  /// 
  /// \param usedAlternative
  ///   returns the alternative used when automatic mapping was enabled 1(alternative == -1).
  ///
  /// \see MapTrigger
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapInputDeviceControlValuePressHold(int index, int alternative, IVInputDevice &inputDevice, unsigned int control, float holdTime=0.3f, int* usedAlternative = NULL);
  
  /// \brief
  ///   Deprecated, please use MapTrigger instead. Create a simple mapping between a touch area and a trigger.
  /// 
  /// This function can also be used to replace an existing mapping.
  /// 
  /// \param index
  ///   The number of the trigger to assign a new/alternative control.
  /// 
  /// \param alternative
  ///   0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor. If equal -1, the
  ///   control will be mapped to the next unused alternative.
  /// 
  /// \param control
  ///   The control which gets mapped to the specified trigger. Must be a value between 
  ///   CT_TOUCH_ANY and CT_TOUCH_DELTA_Y.
  /// 
  /// \param area
  ///   The touch area which should be mapped.
  ///
  /// \param usedAlternative
  ///   returns the alternative used when automatic mapping was enabled 1(alternative == -1).
  ///
  /// \param bTimeScaled
  ///   Indicates that for this mapped control, the returned value will be scaled by the time difference.
  ///
  /// \see MapTrigger
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapInputDeviceTouchArea(int index, int alternative, unsigned int control, VTouchArea* area, int* usedAlternative = NULL, bool bTimeScaled = false);
  
  /// \brief
  ///   Deprecated, please use MapTrigger instead. Create toggle switch style mapping between a touch area and a trigger.
  /// This function can also be used to replace an existing mapping.
  /// 
  /// \param index
  ///   The number of the trigger to assign a new/alternative control.
  /// 
  /// \param alternative
  ///   0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor. If equal -1, the
  ///   control will be mapped to the next unused alternative.
  /// 
  /// \param control
  ///   The control which gets mapped to the specified trigger. Must be a value between 
  ///   CT_TOUCH_ANY and CT_TOUCH_DELTA_Y.
  /// 
  /// \param area
  ///   The touch area which should be mapped.
  ///
  /// \param eHitType
  ///   hit type that determines when the event is triggered. See SetMode
  /// 
  /// \param usedAlternative
  ///   returns the alternative used when automatic mapping was enabled 1(alternative == -1)
  ///
  /// \see MapTrigger
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapInputDeviceTouchAreaSingleHit(int index, int alternative, unsigned int control, VTouchArea* area, VSingleHitType eHitType = SINGLE_HIT_ONPRESSED, int* usedAlternative = NULL);
    
  /// \brief
  ///   Deprecated, please use MapTrigger instead. Create toggle switch style mapping between a touch area and a trigger.
  /// This function can also be used to replace an existing mapping.
  /// 
  /// \param index
  ///   The number of the trigger to assign a new/alternative control.
  /// 
  /// \param alternative
  ///   0-based index of the trigger control alternative. The valid range for this value is the
  ///   maximum number of alternatives per trigger passed to the the constructor. If equal -1, the
  ///   control will be mapped to the next unused alternative.
  /// 
  /// \param control
  ///   The control which gets mapped to the specified trigger. Must be a value between 
  ///   CT_TOUCH_ANY and CT_TOUCH_DELTA_Y.
  /// 
  /// \param area
  ///   The touch area which should be mapped.
  ///
  /// \param eHitType
  ///   hit type that determines when the event is triggered. See SetMode
  /// 
  /// \param usedAlternative
  ///
  /// \see MapTrigger
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapInputDeviceTouchAreaSingleHitPerFrame(int index, int alternative, unsigned int control, VTouchArea* area, VSingleHitType eHitType = SINGLE_HIT_ONPRESSED, int* usedAlternative = NULL);

  /// \brief
  ///   Deprecated, please use ConvertToOnce instead. Helper function to convert an existing mapping entry to single hit mode.
  /// 
  /// \param index
  ///   The number of the trigger.
  /// 
  /// \param alternative
  ///   The control to convert.
  ///
  /// \see ConvertToOnce
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapConvertToControlValueSingleHit(int index, int alternative);


  /// \brief
  ///   Deprecated, please use ConvertToOnce instead. Helper function to convert an existing mapping entry to single hit mode.
  /// 
  /// \param index
  ///   The number of the trigger.
  /// 
  /// \param alternative
  ///   The control to convert.
  ///
  /// \see ConvertToOnce
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapConvertToControlValueSingleHitPerFrame(int index, int alternative);


  /// \brief
  ///   Deprecated, please use ConvertToValue instead. Helper function to convert an existing mapping entry to normal value mode.
  /// 
  /// \param index
  ///   The number of the trigger.
  /// 
  /// \param alternative
  ///   The control to convert.
  ///
  /// \param bTimeScaled
  ///   Indicates that for this mapped control, the returned value will be scaled by the time difference.
  ///
  /// \see ConvertToValue
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapConvertToControlValue(int index, int alternative, bool bTimeScaled = false);

  /// \brief
  ///   Deprecated, please use ConvertToHold instead. Helper function to convert an existing mapping entry to press-hold input
  /// 
  /// \param index
  ///   The number of the trigger.
  /// 
  /// \param alternative
  ///   The control to convert.
  ///
  /// \param holdTime
  ///   Time to consider the 'hold' state. Default=0.3f
  ///
  /// \see ConvertToHold
  HKV_DEPRECATED_2012_2 VBASE_IMPEXP bool MapConvertToControlValuePressHold(int index, int alternative, float holdTime=0.3f );

  ///
  /// @}
  ///

  /// \brief Returns the number of possible triggers in this map.
  inline int GetNumOfTriggers() { return m_iNumMappedInputs; }

  /// \brief Returns the number of alternatives per trigger (how many controls are mapable to one trigger).
  inline int GetNumOfAlternatives() { return m_iNumAlternatives; }

protected:
  int GetNextFreeAlternative(int iTriggerIndex) const;

  VInputMap(const VInputMap &other);

  VMappedInput** m_ppMappedInputs;
  int m_iNumMappedInputs;
  int m_iNumAlternatives;
  
};


/// \brief
///   Enumerates mouse buttons. Used by VInputCallbacks::OnMouseButtonUp/OnMouseButtonDown
enum VMouseButtonIndex
{
  LeftMouseButton = 0,
  RightMouseButton = 1,
  MiddleMouseButton = 2
};


/// \brief
///   Data object sent by the VInputCallbacks::OnKeyUp / VInputCallbacks::OnKeyDown callbacks.
///   Provides platform specific scan code and Vision keycode
class VInputKeyDataObject : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Internal constructor
  inline VInputKeyDataObject(VCallback *pSender, int iScanCode, int iVGLKey) : IVisCallbackDataObject_cl(pSender)
  {
    m_iScanCode = iScanCode;
    m_iVGLKey = iVGLKey;
  }

  int m_iScanCode; ///< platform specific scan code value (e.g. DirectInput scan code)
  int m_iVGLKey;   ///< translated Vision key code (e.g. VGLK_SPACE)
};


/// \brief
///   Data object sent by the VInputCallbacks::OnMouseButtonUp / VInputCallbacks::OnMouseButtonDown
///   callbacks.
class VInputMouseButtonDataObject : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Internal constructor
  inline VInputMouseButtonDataObject(VCallback *pSender, VMouseButtonIndex iButton) : IVisCallbackDataObject_cl(pSender)
  {
    m_iMouseButton = iButton;
  }
  VMouseButtonIndex m_iMouseButton; ///< enum that specifies the mouse button index
};


#ifdef WIN32

/// \brief
///   Callback data object for the Windows specific VInputCallbacks::OnPreTranslateMessage
///   callback. Contains the message structure
class VWindowsMessageDataObject : public IVisCallbackDataObject_cl
{
public:
  inline VWindowsMessageDataObject(VCallback *pSender, HWND hwnd) : IVisCallbackDataObject_cl(pSender)
  {
    m_Hwnd = hwnd;
    m_bProcessed = false;
  }
  HWND m_Hwnd;  ///< the current window
  MSG m_Msg;    ///< the windows message structure
  bool m_bProcessed;  ///< set this member to true to skip the next TranslateMessage/DispatchMessage calls
};
#endif


/// \brief
///   Global namespace for input related callbacks. Example: VInputCallbacks::OnKeyDown +=
///   myKeyHandler
class VInputCallbacks
{
public:

  /// \brief
  ///   This callback is triggered when a keyboard key is released. The callback's data object can
  ///   be casted to VInputKeyDataObject and provides the keycode
  VBASE_IMPEXP static VCallback OnKeyUp;

  /// \brief
  ///   This callback is triggered when a keyboard key is pressed. The callback's data object can
  ///   be casted to VInputKeyDataObject and provides the keycode
  VBASE_IMPEXP static VCallback OnKeyDown;

  /// \brief
  ///   This callback is triggered when a mouse button is released. The callback's data object can
  ///   be casted to VInputMouseButtonDataObject and provides the mouse button index
  VBASE_IMPEXP static VCallback OnMouseButtonUp;

  /// \brief
  ///   This callback is triggered when a mouse button is pressed. The callback's data object can
  ///   be casted to VInputMouseButtonDataObject and provides the mouse button index
  VBASE_IMPEXP static VCallback OnMouseButtonDown;

#ifdef WIN32
  /// \brief
  ///   Windows specific callback that is called inside the windows message loop.
  /// 
  /// The data object can be casted to VWindowsMessageDataObject which provides the MSG data
  /// structure.
  /// 
  /// The data's m_bProcessed member can be set by listeners to skip subsequent calls of
  /// TranslateMessage/DispatchMessage for the specific message.
  VBASE_IMPEXP static VCallback OnPreTranslateMessage;

  /// \brief
  ///   Windows specific callback that is called inside the window proc before any other messages are handled.
  /// 
  /// The data object can be casted to VWindowsMessageDataObject which provides the MSG data
  /// structure.
  /// 
  /// The data's m_bProcessed member can be set by listeners to avoid any futher processing
  /// in the window proc function (then DefWindowProc will not be called):
  /// \par Implementation Details:
  ///   \code
  ///   LRESULT CALLBACK WindwProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
  ///   {
  ///     VWindowsMessageDataObject data(&VInputCallbacks::OnPostTranslateMessage, hwnd);
  ///     data.m_Msg.message = message;
  ///     data.m_Msg.wParam = wParam;
  ///     data.m_Msg.lParam = lParam;
  ///     data.m_bProcessed = false;
  ///     data.Trigger();
  ///
  ///     if(data.m_bProcessed)
  ///       return FALSE;  
  ///     
  ///     //further msg handling
  ///     ...
  ///   }
  ///   \endcode
  VBASE_IMPEXP static VCallback OnPostTranslateMessage;
  
#endif

  /// \brief
  ///   Internal trigger helper used by the engine
  static inline void TriggerKeyEvent(VCallback &callback, int iScanCode, int iVGLKey)
  {
    if (callback.HasCallbacks())
    {
      VInputKeyDataObject data(&callback,iScanCode,iVGLKey);
      data.Trigger();
    }
  }

  /// \brief
  ///   Internal trigger helper used by the engine
  static inline void TriggerMouseButtonEvent(VCallback &callback, VMouseButtonIndex iButton)
  {
    if (callback.HasCallbacks())
    {
      VInputMouseButtonDataObject data(&callback,iButton);
      data.Trigger();
    }
  }

};

#endif //VINPUT_HPP_INCLUDED

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
