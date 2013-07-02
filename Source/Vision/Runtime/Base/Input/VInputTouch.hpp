/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VInputTouch.hpp

#ifndef VINPUTTOUCH_HPP_INCLUDED
#define VINPUTTOUCH_HPP_INCLUDED

#include <Vision/Runtime/Base/Math/Primitive/VRectanglef.hpp>

/// \brief
///   Represents an area on a touch screen which can be mapped to input controls
class VTouchArea : public VRefCounter
{
  friend class IVMultiTouchInput;
public:
  VBASE_IMPEXP VTouchArea(IVMultiTouchInput& inputDevice, const VRectanglef& area = VRectanglef(), float fPriority = -1000.0f);
  
  VBASE_IMPEXP ~VTouchArea();
  
  inline const VRectanglef& GetArea() const
  {
    return m_area;
  }

  inline void SetArea(const VRectanglef& area)
  { 
    m_area = area; 
  }
  
  inline float GetPriority() const 
  {
    return m_fPriority;
  }
  
  inline int GetTouchPointIndex() const
  {
    return m_iTouchPointIndex;
  }
  
  inline BOOL IsFullscreen() const
  {
	return m_bFullscreen;
  }

  inline IVMultiTouchInput& GetInputDevice()
  {
    VASSERT(m_inputDevice != NULL);
    return *m_inputDevice;
  }
  
private:
  VRectanglef m_area;
  bool m_bFullscreen;
  float m_fPriority;
  
  int m_iTouchPointIndex;
  
  IVMultiTouchInput* m_inputDevice;
};

typedef VSmartPtr<VTouchArea> VTouchAreaPtr;

/// \brief
///   Simple mapping between touch point and value without special behavior.
class VMappedTouchValue : public VMappedInput
{
public:
  
  /// \brief
  ///   Constructor to map a input control to a simple float value.
  /// 
  /// \param area
  ///   The touch area to map.
  /// 
  /// \param control
  ///   The control to map.
  VBASE_IMPEXP VMappedTouchValue(VTouchArea* area, unsigned int control);
  
  VBASE_IMPEXP virtual ~VMappedTouchValue() { }
  
  /// \brief
  ///   Gets the current value (in the valid range) of the mapped control.
  /// 
  /// \param chainValue
  ///   Used to chain the values of multiple controls (required for SingleHitPerFrame behavior)
  VBASE_IMPEXP virtual float GetValue(float chainValue=0);
  
  /// \brief
  ///   Returns the device that is associated with this mapping (see constructor).
  inline IVMultiTouchInput& GetInputDevice()
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
  VBASE_IMPEXP VMappedTouchValue(const VMappedTouchValue &other);
  
protected:
  IVMultiTouchInput &m_inputDevice;
  unsigned int m_uiInputDeviceControl;
  
  VTouchAreaPtr m_area;
};

/// \brief
///   Maps a touch point as toggle switch, which is invoked once if the value differs from 0.
class VMappedTouchOnce : public VMappedTouchValue
{
public:
  /// \brief
  ///   Constructor to map a touch area to a single hit toggle switch.
  /// 
  /// \param pArea
  ///   The touch area to map.
  /// 
  /// \param uiControl
  ///   The control to map.
  /// 
  /// \param eOnceMode
  ///   hit type that determines when the event is triggered. See VMappedTouchOnce::SetMode
  VBASE_IMPEXP VMappedTouchOnce(VTouchArea* pArea, unsigned int uiControl, VOnceMode eOnceMode = ONCE_ON_PRESS);

  HKV_DEPRECATED_2012_2 VBASE_IMPEXP VMappedTouchOnce(VTouchArea* pArea, unsigned int uiControl, VSingleHitType eHitType);
  
  VBASE_IMPEXP virtual ~VMappedTouchOnce() {}  
  
  /// \brief
  ///   Gets the current value (in the valid range) of the mapped control.
  /// 
  /// \param fChainValue
  ///   Used to chain the values of multiple controls (required for SingleHitPerFrame behavior)
  VBASE_IMPEXP virtual float GetValue(float fChainValue=0); 
  
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
  ///   Sets the ramp type that defines when the event is triggered for this single hit type. A
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
  ///   Returns the mode defined via SetSingleHitType. The default mode is SINGLE_HIT_ONPRESSED
  HKV_DEPRECATED_2012_2 inline VSingleHitType GetSingleHitType() const
  {
    return (VSingleHitType)m_eOnceMode;
  }
  
private:
  VBASE_IMPEXP VMappedTouchOnce(const VMappedValueSingleHit &other);
  
protected:
  bool m_bQueried;
  VOnceMode m_eOnceMode;
};

//compatibility type
HKV_DEPRECATED_2012_2 typedef VMappedTouchOnce VMappedTouchValueSingleHit;


/// \brief
///   Maps a touch point as toggle switch, which is invoked once if the value differs from 0.
class VMappedTouchOncePerFrame : public VMappedTouchValue
{
public:
    /// \brief
    ///   Constructor to map a touch area to a single hit toggle switch.
    /// 
    /// \param pArea
    ///   The touch area to map.
    /// 
    /// \param uiControl
    ///   The control to map.
    /// 
    /// \param eOnceMode
    ///   hit type that determines when the event is triggered. See VMappedTouchOncePerFrame::SetMode.
    VBASE_IMPEXP VMappedTouchOncePerFrame(VTouchArea* pArea, unsigned int uiControl, VOnceMode eOnceMode = ONCE_ON_PRESS);

    // \brief Compatibility constructor.
    HKV_DEPRECATED_2012_2 VBASE_IMPEXP VMappedTouchOncePerFrame(VTouchArea* pArea, unsigned int uiControl, VSingleHitType eHitType);
    
    VBASE_IMPEXP virtual ~VMappedTouchOncePerFrame() {}  
    
    /// \brief
    ///   Gets the current value (in the valid range) of the mapped control.
    /// 
    /// \param fChainValue
    ///   Used to chain the values of multiple controls (required for SingleHitPerFrame behavior)
    VBASE_IMPEXP virtual float GetValue(float fChainValue=0); 
    
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
    ///   Sets the ramp type that defines when the event is triggered for this single hit type. A
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
    ///   Returns the mode defined via SetSingleHitType. The default mode is SINGLE_HIT_ONPRESSED
    HKV_DEPRECATED_2012_2 inline VSingleHitType GetSingleHitType() const
    {
      return (VSingleHitType)m_eOnceMode;
    }
    
private:
    VBASE_IMPEXP VMappedTouchOncePerFrame(const VMappedValueSingleHit &other);
    
protected:
    bool m_bQueried;
    bool m_bStateLastFrame;
    float m_fLastResult;
    int m_iFrameQueried;
    VOnceMode m_eOnceMode;
};

//compatibility type
HKV_DEPRECATED_2012_2 typedef VMappedTouchOncePerFrame VMappedTouchValueSingleHitPerFrame;

#define V_MAX_TAPS 3

/// \class  IVMultiTouchInput
///
/// \brief  Base class for multitouch interfaces. 
///
class IVMultiTouchInput : public IVInputDevice
{
  friend class VTouchArea;
protected:
  int m_iNumberOfTouchPoints;
  
  VPListT<VTouchArea> m_areaMappings;
  
  VBASE_IMPEXP void Map(int iTouchPointIndex, int iPosX, int iPosY);
  VBASE_IMPEXP void UpdateMapping(int iTouchPointIndex, int iPosX, int iPosY);
  VBASE_IMPEXP void Unmap(int iTouchPointIndex);
  
  VBASE_IMPEXP void AddTouchArea(VTouchArea* area);
  VBASE_IMPEXP void RemoveTouchArea(VTouchArea* area);
  
  VBASE_IMPEXP void ResetTaps();
  VBASE_IMPEXP void DetectTaps(int iPosX, int iPosY);
  
#ifndef _VISION_DOC
  struct VTapPoint
  {
    uint64 uiTimeStamp;
    float fXAbsolute;
    float fYAbsolute;
  };
#endif
  
  unsigned int m_uiNumberOfTaps;
  unsigned int m_uiLastTapIndex;
  VTapPoint m_TapPoints[V_MAX_TAPS];
  
  unsigned int m_uiTapTimeout;
  float m_fMaxTapDistance;
  
public:
  
  /// \struct  VTouchPoint
  ///
  /// \brief  Description of a touch point.
  ///
  struct VTouchPoint
  {
    inline VTouchPoint()
    {
      fXAbsolute = fYAbsolute = 0.f;
      fXPrevLocationAbsolute = fYPrevLocationAbsolute = 0.f;
      iForce = 0;
      iID = -1;
    }
    
    float fXAbsolute;
    float fYAbsolute;
    
    float fXPrevLocationAbsolute;
    float fYPrevLocationAbsolute;
    
    int iForce;
    
    int iID;
    
    /// \brief  Query if the touch is in the given area.
    ///
    /// \param  iX1 The first i x coordinate. 
    /// \param  iY1 The first i y coordinate. 
    /// \param  iX2 The second i x coordinate. 
    /// \param  iY2 The second i y coordinate. 
    ///
    /// \return true if in area, false if not. 
    ///
    inline bool IsInArea(int iX1, int iY1, int iX2, int iY2) const
    {
      return (fXAbsolute >= (float)iX1 && fXAbsolute <= (float)iX2) && (fYAbsolute >= (float)iY1 && fYAbsolute <= (float)iY2);
    }
    
    /// \brief  Query if the touch is in the given area.
    ///
    /// \param  area The area to test.
    ///
    /// \return true if in area, false if not. 
    ///
    inline bool IsInArea(const VRectanglef& area) const
    {
      return area.IsInside(fXAbsolute, fYAbsolute);
    }

  };
  
  /// \brief  Default constructor. 
  ///
  VBASE_IMPEXP IVMultiTouchInput();
  
  VBASE_IMPEXP ~IVMultiTouchInput();
  
  /// \brief
  ///   Get the value of a touch point.
  ///
  /// \param index
  ///   Index of the touch point
  /// 
  /// \param control
  ///   Integer value of the demanded control. Please see the platform specific implementation for supported control values.
  ///
  /// \param bTimeScaled
  ///   If true, scales the difference from the dead zone by the frame time.
  ///
  VBASE_IMPEXP virtual float GetTouchPointValue(int index, unsigned int control, bool bTimeScaled = false) = 0;
  
  /// \brief  Gets the number of currently active touch points. 
  ///
  /// \return The number of currently active touch points. 
  ///
  VBASE_IMPEXP virtual int GetNumberOfTouchPoints()
  {
    return m_iNumberOfTouchPoints;
  }
  
  /// \brief  Gets the maximum number of active touch points. 
  ///
  /// \return The maximum number of active touch points. 
  ///
  VBASE_IMPEXP virtual int GetMaximumNumberOfTouchPoints() = 0;
  
  /// \brief
  ///  Gets the number of taps, e.g. 2 for a double tap
  inline unsigned int GetNumberOfTaps() const
  {
    return m_uiNumberOfTaps;
  }
  
  /// \brief
  ///  Sets the maximum time in ms between two taps to detect as a double tap
  inline void SetTapTimeout(unsigned int uiTimeout)
  {
    m_uiTapTimeout = uiTimeout;
  }
  
  /// \brief
  ///  Sets the maximum distance in pixel between two taps to detect as a double tap
  inline void SetMaxTapDistance(float fDistance)
  {
    m_fMaxTapDistance = fDistance * fDistance;
  }
  
  /// \brief  
  ///   Get a touch point.
  ///
  /// \param  uiIndex  
  ///   Index of the touch point
  ///
  VBASE_IMPEXP virtual const VTouchPoint& GetTouch(unsigned int uiIndex) const = 0;
  
  /// \brief  
  ///   Query, if touch point is active.
  ///
  /// \param  uiIndex  
  ///   Index of the touch point
  ///
  VBASE_IMPEXP virtual bool IsActiveTouch(unsigned int uiIndex) const = 0;  

  /// \brief
  ///  gets a list of all registered touch areas
  inline const VPListT<VTouchArea>& GetTouchAreas() const { return m_areaMappings; }
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
