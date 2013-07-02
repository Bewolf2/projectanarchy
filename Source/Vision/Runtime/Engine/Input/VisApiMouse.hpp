/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiMouse.hpp

#ifndef DEFINE_VISAPIMOUSE
#define DEFINE_VISAPIMOUSE

#ifdef SUPPORTS_MOUSE


/// \brief
///   Class for accessing mouse input. The functions are accessible via Vision::Mouse.
class VisMouse_cl
{

public:

  /// \brief
  ///   Initializes mouse control.
  /// 
  /// \param sensitivity
  ///   Mouse sensitivity. This functionality is currently not implemented, though setting a value
  ///   of 0 will mean "set default sensitivity".
  /// 
  /// \param method
  ///   The preferred method for input. The folowing constants are supported:
  ///   \li VGL_WINDOWSINPUT: Use Windows mouse input,
  /// 
  ///   \li VGL_DIRECTINPUT: Use DirectInput mouse input
  /// 
  /// \return
  ///   RETVAL result: result, VERR_NOERROR if everything is OK, or VERR_NOWINDOW if there was no
  ///   window with which the mouse could be associated
  /// 
  /// \note
  ///   This function may only be called after Vision.Video.SetMode.
  /// 
  /// \note
  ///   On Win32 based systems it is usually the best to use the VGL_WINDOWSINPUT method during the
  ///   development and the VGL_DIRECTINPUT method for release builds. While the VGL_WINDOWSINPUT
  ///   method does still allow you to use the mouse in the debugger, the VGL_DIRECTINPUT method is
  ///   much more accurate esp. at low frame rates.
  VISION_APIFUNC RETVAL Init(SLONG sensitivity = 0, SLONG method = VGL_DEFAULTINPUT);


  /// \brief
  ///   Deinitializes mouse control.
  /// 
  /// DeInit releases the resources needed for mouse handling and displays the mouse cursor again.
  /// 
  /// \return
  ///   RETVAL result:  VGL_NOERROR if everything is OK or VGL_NOWINDOW if there was no window
  ///   associated with the mouse
  /// 
  /// \note
  ///   Call this before calling Vision.Video.DeInit
  VISION_APIFUNC void DeInit();


  /// \brief
  ///   Checks whether mouse status has been changed since the last frame
  /// 
  /// It indicates whether the mouse has been moved or a button has been pressed/released.
  /// 
  /// \return
  ///   BOOL bChanged: TRUE if mouse status has changed
  VISION_APIFUNC BOOL HasChanged() const;

  /// \brief
  ///   Sets the new on-screen position of the mouse
  ///   The position must be inside current viewport coordinate range.
  VISION_APIFUNC void SetPosition(float x, float y);

  /// \brief
  ///   Gets the current on-screen position of the mouse
  /// 
  /// The position is clamped to the current viewport size.
  /// 
  /// \param x,y
  ///   Output variables to receive the mouse position as floats in pixels
  VISION_APIFUNC void GetPosition(float &x, float &y) const;

  
  /// \brief
  ///   Checks whether the left mouse button is currently pressed
  /// 
  /// \return
  ///   BOOL bPressed: left mouse button state
  VISION_APIFUNC BOOL IsLeftButtonPressed() const;

  /// \brief
  ///   Checks whether the middle mouse button is currently pressed
  /// 
  /// \return
  ///   BOOL bPressed: middle mouse button state
  VISION_APIFUNC BOOL IsMiddleButtonPressed() const;

  
  /// \brief
  ///   Checks whether the right mouse button is currently pressed
  /// 
  /// \return
  ///   BOOL bPressed: right mouse button state
  VISION_APIFUNC BOOL IsRightButtonPressed() const;

  
  /// \brief
  ///   Returns the current on-screen x position of the mouse
  /// 
  /// The position is clamped to the current viewport size.
  /// 
  /// \return
  ///   float xpos : x position of the mouse
  VISION_APIFUNC float GetX() const;

  
  /// \brief
  ///   Returns the current on-screen y position of the mouse
  /// 
  /// The position is clamped to the current viewport size.
  /// 
  /// \return
  ///   float ypos : y position of the mouse
  VISION_APIFUNC float GetY() const;

  
  /// \brief
  ///   Returns the mouse delta movement in x-direction since the last frame
  /// 
  /// \return
  ///   float fDeltaX : movement in pixels since last frame
  VISION_APIFUNC float GetDeltaX() const;

  
  /// \brief
  ///   Returns the mouse delta movement in y-direction since the last frame
  /// 
  /// \return
  ///   float fDeltaY : movement in pixels since last frame
  VISION_APIFUNC float GetDeltaY() const;

  

  /// \brief
  ///   Gets the delta of the mouse wheel movement since the last frame.
  /// 
  /// \return
  ///   SLONG wheelDelta: positive value for up-movement and negative value for down-value.
  /// 
  /// \note
  ///   Moving the wheel slowly upwards results in a wheelDelta of 1.
  /// 
  /// \note
  ///   Under windows you should use initialise the mouse with the method VGL_DIRECTINPUT, the
  ///   standard windows input method does report wheel deltas over multiple frames in case the
  ///   wheel has been moved fast.
  /// 
  /// \example
  ///   \code
  ///   BOOL upDir = ( Vision::Mouse.GetWheelDelta() > 0 );
  ///   \endcode
  inline SLONG GetWheelDelta() const;

  /// \brief
  ///   Captures the mouse cursor.
  /// 
  /// After calling this function, the mouse cursor is not visible anymore and the engine
  /// application owns the mouse.
  /// 
  /// \sa VisMouse_cl::Release()
  /// \sa VisMouse_cl::IsCaptured()
  /// 
  /// \example
  ///   \code
  ///   Vision::Mouse.Capture();
  ///   ShowWarningDialog();
  ///   Vision::Mouse.Release();
  ///   \endcode
  VISION_APIFUNC void Capture();

  /// \brief
  ///   Releases the mouse cursor
  /// 
  ///  After calling this function, the mouse cursor is  visible and not controlled by the engine
  /// application.
  /// 
  /// \note
  ///   After the mouse has been initialised by the engine, it is captured
  /// 
  /// \sa VisMouse_cl::Capture()
  /// \sa VisMouse_cl::IsCaptured()
  /// 
  /// \example
  ///   \code
  ///   Vision::Mouse.Capture();
  ///   ShowWarningDialog();
  ///   Vision::Mouse.Release();
  ///   \endcode
  VISION_APIFUNC void Release();

  /// \brief
  ///   Indicates whether the mouse cursor is currently captured
  /// 
  /// \return
  ///   BOOL bCaptured: TRUE, if the mouse is currently captured
  /// 
  /// \note
  ///   After initialising the mouse, it is captured
  /// 
  /// \sa VisMouse_cl::Capture()
  /// \sa VisMouse_cl::Release()
  VISION_APIFUNC BOOL IsCaptured() const;

  /// \brief
  ///   Updates the mouse status. This function will automatically be called once a frame.
  VISION_APIFUNC void Handle();

  /// \brief
  ///   Return the mouse input method specified in the Init function (VGL_DIRECTINPUT resp.
  ///   VGL_WINDOWSINPUT)
  VISION_APIFUNC RETVAL GetMethod() const;


private:  

  VGLMouse_t oldStatus;         ///< old position and button status
  VGLMouse_t currentStatus;     ///< current position and button status

  RETVAL currentMethod;
  bool isCaptured;
    
  friend class Vision;
  friend class VisGame_cl;
};



inline BOOL VisMouse_cl::IsLeftButtonPressed() const
{
  return currentStatus.buttonL;
}


inline BOOL VisMouse_cl::IsMiddleButtonPressed() const
{
  return currentStatus.buttonM;
}


inline BOOL VisMouse_cl::IsRightButtonPressed() const
{
  return currentStatus.buttonR;
}

inline void VisMouse_cl::GetPosition(float &x, float &y) const
{
  x = (float) currentStatus.posX;
  y = (float) currentStatus.posY;
}


inline float VisMouse_cl::GetX() const
{
  return(float)( currentStatus.posX);
}


inline float VisMouse_cl::GetY() const
{
  return(float)( currentStatus.posY);
}


inline float VisMouse_cl::GetDeltaX() const
{
  return(float)( currentStatus.deltaX);
}


inline float VisMouse_cl::GetDeltaY() const
{
  return(float)( currentStatus.deltaY);
}


inline SLONG VisMouse_cl::GetWheelDelta() const
{
  return currentStatus.wheelUp - currentStatus.wheelDown;
}




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
