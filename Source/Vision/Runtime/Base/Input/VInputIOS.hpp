/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VInputIOS.hpp

#ifndef VINPUTIOS_HPP_INCLUDED
#define VINPUTIOS_HPP_INCLUDED

class VInputManagerIOS;
typedef VInputManagerIOS VInputManager;

//compatiblity typedefs
HKV_DEPRECATED_2012_2 typedef VInputManagerIOS VInputDeviceManager;
HKV_DEPRECATED_2012_2 typedef VInputManagerIOS VInputDeviceManagerIOS;

#define V_IOS_MAX_TOUCH_POINTS 10

/// \brief
///   Enumeration for the model of the InputDevice, see IVInputDevice::GetModel
enum VInputDeviceModel 
{
  //name ::= "INPUT_DEVICE_" platform "_" device
  INPUT_DEVICE_NONE,

  INPUT_DEVICE_EMULATED_TOUCHSCREEN,  ///< emulated touchscreen by vRemoteInput plugin
  INPUT_DEVICE_EMULATED_MOTIONSENSOR, ///< emulated motion sensor by vRemoteInput plugin
  INPUT_DEVICE_EMULATED_JOYSTICK,     ///< emulated joystick

  INPUT_DEVICE_IOS_TOUCHSCREEN,
  INPUT_DEVICE_IOS_MOTIONSENSOR
};

/// \class  VMotionInputIOS
///
/// \brief  Input device handling motion input by the user. 
///
class VMotionInputIOS : public IVInputDevice
{
  public:

    /// \brief  Default constructor. 
    ///
    VMotionInputIOS();

    /// \brief  Destructor. 
    ///
    ~VMotionInputIOS();

    /// \brief
    ///   See IVInputDevice::Reset().
    virtual void Reset() HKV_OVERRIDE;

    /// \brief
    ///   See IVInputDevice::Update(float fTimeDiff = 0.0f).
    virtual void Update(float fTimeDiff = 0.0f) HKV_OVERRIDE;

    /// \brief
    ///   See IVInputDevice::GetControlName(unsigned int device).
    HKV_DEPRECATED_2012_2 virtual const char* GetControlName(unsigned int control) HKV_OVERRIDE;

    /// \fn void SetEnabled(bool bEnabled)
    ///
    /// \brief  Sets the status of the multitouch input. Since iOS devices are mobile devices certain kinds of input (motion sensor, touch screen) can be activated as needed to conserve memory.
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
    ///   Integer value of the requested control. The following controls are supported: CT_MOTION_ACCELERATION_X/Y/Z.
    ///
    /// \param fDeadZone
    ///   Within the dead zone the return value is always 0.
    ///
    /// \param bTimeScaled
    ///   If true the value will be scaled by the time difference to the previous frame.
    ///
    VBASE_IMPEXP virtual float GetControlValue(unsigned int control, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;


    /// \brief
    ///   See IVInputDevice::GetRawControlValue(unsigned int control).
    /// 
    /// \param control
    ///   Integer value of the requested control. This implementation doesn't support any raw control values.
    virtual int GetRawControlValue(unsigned int control) HKV_OVERRIDE;

    /// \brief
    ///   See IVInputDevice::GetModel().
    virtual int GetModel() HKV_OVERRIDE;

    /// \brief
    ///   See IVInputDevice::GetName().
    virtual const char* GetName() HKV_OVERRIDE;

    /// \fn hkvVec3 GetAcceleration()
    ///
    /// \brief  Gets the acceleration. 
    ///
    /// \return The acceleration. 
    ///
    hkvVec3 GetAcceleration();

    /// \fn void SetAcceleration(float fAccelerationX, float fAccelerationY, float fAccelerationZ)
    ///
    /// \brief  Sets the current acceleration. (Used internally by the Objective-C layer)
    ///
    /// \param  fAccelerationX  The x acceleration. 
    /// \param  fAccelerationY  The y acceleration. 
    /// \param  fAccelerationZ  The z acceleration. 
    ///
    VBASE_IMPEXP void SetAcceleration(float fAccelerationX, float fAccelerationY, float fAccelerationZ);
    
  private:

    float m_fAccelerationX, m_fAccelerationY, m_fAccelerationZ;
  
    float m_fTimeDiff;

    bool m_bEnabled;
};


/// \class  VMultiTouchInputIOS
///
/// \brief  Multi touch device for IOS. 
///
class VMultiTouchInputIOS : public IVMultiTouchInput
{
public:

  /// \fn VMultiTouchInputIOS()
  ///
  /// \brief  Constructor. 
  ///
  VMultiTouchInputIOS();

  /// \fn ~VMultiTouchInputIOS()
  ///
  /// \brief  Destructor. 
  ///
  ~VMultiTouchInputIOS();

  /// \brief
  ///   See IVInputDevice::Reset().
  virtual void Reset() HKV_OVERRIDE;


  /// \brief
  ///   See IVInputDevice::Update(float fTimeDiff = 0.0f).
  virtual void Update(float fTimeDiff = 0.0f) HKV_OVERRIDE;

  /// \brief
  ///   See IVInputDevice::GetControlName(unsigned int device).
  virtual const char* GetControlName(unsigned int control) HKV_OVERRIDE;

  /// \brief
  ///   See IVInputDevice::GetControlValue.
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_TOUCH_ANY, CT_TOUCH_POINT_n_X/Y for n in [0, 9]
  ///
  /// \param fDeadZone
  ///   Within the dead zone the return value is always 0.
  ///
  /// \param bTimeScaled
  ///   If true the control value will be scaled with the time difference from the previous frame.
  ///
  VBASE_IMPEXP virtual float GetControlValue(unsigned int control, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;

  /// \brief
  ///   See IVInputDevice::GetRawControlValue(unsigned int control).
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_TOUCH_ANY, CT_TOUCH_POINT_n_X/Y for n in [0, 9]
  virtual int GetRawControlValue(unsigned int control) HKV_OVERRIDE;

  /// \brief
  ///   See IVInputDevice::GetModel().
  VOVERRIDE int GetModel();

  /// \brief
  ///   See IVInputDevice::GetName().
  VOVERRIDE const char* GetName();
  
  VBASE_IMPEXP VOVERRIDE int GetNumberOfTouchPoints();  

  /// \fn VBASE_IMPEXP virtual int GetMaximumNumberOfTouchPoints()
  ///
  /// \brief  Gets the maximum number of touch points. 
  ///
  /// \return The maximum number of touch points. 
  ///
  VBASE_IMPEXP virtual int GetMaximumNumberOfTouchPoints() HKV_OVERRIDE
  {
    return V_IOS_MAX_TOUCH_POINTS;
  }

  /// \brief
  ///   Get the value of a touch point.
  ///
  /// \param index
  ///   Index of the touch point
  /// 
  /// \param control
  ///   Integer value of the requested control. The following controls are supported: CT_TOUCH_ANY, CT_TOUCH_ABS/REL_X/Y, CT_TOUCH_ABS/REL_X/Y,
  ///   CT_TOUCH_DOUBLE_TAP, CT_TOUCH_TRIPLE_TAP, CT_TOUCH_TAP_X/Y
  ///
  /// \param bTimeScaled
  ///   If true, scales the difference from the dead zone by the frame time.
  ///
  VBASE_IMPEXP virtual float GetTouchPointValue(int index, unsigned int control, bool bTimeScaled = false) HKV_OVERRIDE;


  /// \brief  Sets the status of the multitouch input.
  ///
  /// \param  bEnabled  true to enable, false to disable. 
  ///
  VBASE_IMPEXP void SetEnabled(bool bEnabled);

  /// \brief  Query if this multitouch input is enabled. 
  ///
  /// \return true if enabled, false if not. 
  ///
  VBASE_IMPEXP bool IsEnabled() const
  {
    return m_bEnabled;
  }

  /// \brief  
  ///   Get a touch point.
  ///
  /// \param  uiIndex  
  ///   Index of the touch point
  ///
  VBASE_IMPEXP VOVERRIDE const VTouchPoint& GetTouch(unsigned int uiIndex) const;

  /// \brief  
  ///   Query, if touch point is active.
  ///
  /// \param  uiIndex  
  ///   Index of the touch point
  ///
  VBASE_IMPEXP VOVERRIDE bool IsActiveTouch(unsigned int uiIndex) const;  
  
  
  /// \brief
  ///   internal use only
  VBASE_IMPEXP void AddNewTouch(void* pTouchObject, int iXAbsolute, int iYAbsolute);
  
  /// \brief
  ///   internal use only
  VBASE_IMPEXP void UpdateTouch(void* pTouchObject, int iXAbsolute, int iYAbsolute, int iXPrevLocAbsolute, int iYPrevLocAbsolute);
  
  /// \brief
  ///   internal use only
  VBASE_IMPEXP void RemoveTouch(void* pTouchObject);

private:

  float m_fTimeDiff;

  float m_fInvX;
  float m_fInvY;

  int m_iTouchScale;

  VTouchPoint m_TouchPoints[V_IOS_MAX_TOUCH_POINTS];

  bool m_bEnabled;
};

/// \class  VInputManagerIOS
///
/// \brief  The input device manager for iOS based devices. 
///
class VInputManagerIOS : public VInputManagerBase
{
public:

  /// \brief
  ///   Initialize the IOS device manager.
  static void Init();


  /// \brief
  ///   De-initialize the IOS device manager.
  static void DeInit();


  /// \brief
  ///   Get the initialization status of the IOS device manager.
  static bool IsInitialized();

  static void Update(float timeDiff = 0.0f);

  /// \brief
  ///   Gets an input device of the specified type.
  /// 
  /// \param type
  ///   The type of the requested input device.
  /// 
  /// \return
  ///   IVInputDevice& device: Reference to the requested input device. If the requested type was
  ///   not available the method returns a reference to the field s_NoInputDevice of
  ///   VInputManagerIOS
  static IVInputDevice& GetInputDevice(VInputDeviceType type);

  /// \brief
  ///   Gets an input device of the specified type.
  /// 
  /// \param szDeviceName
  ///   The name of the requested input device, which can be "touch" (or "touchscreen") and "motion" (or "motionsensor");
  /// 
  /// \return
  ///   IVInputDevice &device: Reference to the requested input device. If the requested type was
  ///   not available the method returns a reference to VInputManagerIOS::s_NoInputDevice.
  static IVInputDevice& GetInputDevice(const char * szDeviceName);
	
  static VMultiTouchInputIOS& GetTouchScreen();
  
  static VMotionInputIOS& GetMotionSensor();

  static IVSoftKeyboardAdapter& GetSoftkeyboardAdapter();
  
  static IVInputDevice s_NoInputDevice;
  
  static int s_iFrameCounter;

private:

  static VMultiTouchInputIOS* s_pMultiTouchInput;

  static VMotionInputIOS* s_pMotionInput;
  
  static IVSoftKeyboardAdapter* s_pSoftkeyboardAdapter;
  

  static int s_iInitCount;
  // Here stuff like touch input can be added

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
