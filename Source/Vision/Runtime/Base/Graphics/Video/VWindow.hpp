/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VWindow.hpp

#ifndef DC_VHAL_VWINDOW_HPP
#define DC_VHAL_VWINDOW_HPP

#if (defined(WIN32) || defined(_VISION_DOC)) && !defined(_VISION_WINRT)

class IVisionWindow;

/// \brief
///   Provides access to the basic initialization data for window handling.
///
/// The VWindowConfig class allows you to customize window handling and message dispatching on
/// Windows platforms. It is not used on other platforms.
///
/// An instance of VWindowConfig is contained in the application configuration (VisAppConfig_cl) and
/// used inside VisionApp_cl::InitEngine to configure the engine window.
class VWindowConfig
{
public:
  VBASE_IMPEXP VWindowConfig();
  enum VisionWindowType
  {
    VWNDTYPE_NONE = 0x0000,
    VWNDTYPE_BORDERLESS = 0x0001,
    VWNDTYPE_PICKMESSAGEWITHNULLWINDOWHANDLE = 0x0002,
    // 0x0004
    VWNDTYPE_IGNORE_WORKAREA = 0x0008,
  };

  /// \brief
  ///   Enable handling all messages, including IME-related messages. If this function is not called, IME messages will not be handled by the Vision Window Proc.
  VBASE_IMPEXP void SetPickAllMessage();

  /// \brief
  ///   Returns the window handle for PeekMessage().
  VBASE_IMPEXP HWND GetPeekMessageWndHandle();

  /// \brief
  ///   Use this function to specify whether the window should be borderless or not.
  ///
  /// You can also use this method to change the border style of an existing window.
  ///
  /// \param bState
  ///   Whether the window should be borderless or not.
  VBASE_IMPEXP void SetBorderlessWindow(BOOL bState = true);

  /// \brief
  ///   Use this function to specify a custom window procedure.
  ///
  /// This can be useful in order to write custom mouse/keyboard handling code or handle IME messages.
  ///
  /// \param wndProc
  ///   The custom window procedure to use.
  ///
  /// \note
  ///   We still recommend calling the VisionWindowProc after executing your custom code. Otherwise, you will have to take care
  ///   of all aspects of message handling yourself.
  VBASE_IMPEXP void SetWindowProc(WNDPROC wndProc);

  /// \brief
  ///   Internal use only.
  VBASE_IMPEXP void ChangeWindowCreationFlagsByType(int* pStyle,BOOL bCanRemoveOption = false);

  /// \brief
  ///   Internal use only.
  VBASE_IMPEXP void SetDefaults();

  /// \brief
  ///   Internal use only.
  VBASE_IMPEXP BOOL CheckWindowSettingForIME();

  DWORD WindowCreationFlags;         ///< WIN32 only: Contains the flags used to create the window
  DWORD WindowsClassStyle;           ///< WIN32 only: Contains the style used to create the window class  
  unsigned int VWindowType;         
  WNDPROC s_pfWindowProc;           
  HWND CreatedWindowHandle;         ///< WIN32 only: Used to specify an existing window (if NULL, InitializeScreen will create its own window)
  HWND ParentWindowHandle;          ///< WIN32 only: Parent window (NULL if no parent window)
};

/// \brief
///   Provides creation and customization functionality for Vision windows. Only supported on Windows.
class VisionWindowManager
{
public:
  static VBASE_IMPEXP void OneTimeInit(const VWindowConfig& config);
  static VBASE_IMPEXP void OneTimeDeInit();
  
  /// \brief
  ///   Returns the active windows
  static VBASE_IMPEXP IVisionWindow* GetActiveWindow() { return m_pActiveWindow; }

  /// \brief
  ///   Sets the active window to a custom implementation of the IVisionWindow interface.
  static VBASE_IMPEXP void SetActiveWindow(IVisionWindow* pVisionWindow) { m_pActiveWindow = pVisionWindow; }
    
  /// \brief
  ///   Creates an instance of VisionWindow using the configuration parameters passed in windowConfig and sets it as the active window. The newly created windows is internally stored as the default window.
  static VBASE_IMPEXP BOOL CreateDefaultWindow(const VWindowConfig& windowConfig);

  /// \brief
  ///   Destroys the default window previously created with CreateDefaultWindow.
  static VBASE_IMPEXP void DestroyDefaultWindow();  

  /// \brief
  ///   Internal function.
  static VBASE_IMPEXP void SetWindowSize(int width,int height);

  /// \brief
  ///   Returns whether the active window is the default window or a custom one.
  static VBASE_IMPEXP BOOL IsDefaultWindowActive();

  /// \brief
  ///   Sets the sensitivity of the mouse wheel
  ///
  /// This sensitivity will be used as a divider of raw mouse wheel value.
  ///
  /// \param 
  ///   sensitivity default : 120
  static VBASE_IMPEXP void SetMouseWheelSensitivity(SLONG sensitivity);

protected:
  static IVisionWindow *m_pDefaultWindow;
  static IVisionWindow *m_pActiveWindow;

};

/// \brief
///   Base class for Vision Window. Supported on Windows only.
///
/// \sa VisionWindowManager
class IVisionWindow
{
public:

  VBASE_IMPEXP IVisionWindow(const VWindowConfig& config);

  /// \brief
  ///   Run window message pump, you can customize teh behavior by overriding this function
  VBASE_IMPEXP virtual BOOL RunMessagePump() = 0;

  /// \brief
  ///   Function for registering the custom window class with the Windows OS
  VBASE_IMPEXP virtual BOOL RegisterWindowClass() = 0;

  /// \brief
  ///   Function for unregistering the custom window class from the Windows OS
  VBASE_IMPEXP virtual BOOL UnregisterWindowClass() = 0;

  /// \brief
  ///   Custom function for creating a window.
  VBASE_IMPEXP virtual HWND CreateVWindow(const char* name, 
    unsigned int style, int x, int y, int width, int height, HWND parent) = 0;

  /// \brief
  ///   Returns the window config used to create this window.
  VBASE_IMPEXP VWindowConfig& GetConfig() { return m_config; }

protected:
  VWindowConfig m_config;  

};

/// \brief
///   Window class representing a window implementation that correctly handles Unicode messages. Used as Vision's default window class.
class VisionWindow : public IVisionWindow
{
public:
  /// \brief
  ///   Constructor
  VBASE_IMPEXP VisionWindow(const VWindowConfig& config);

  /// \brief
  ///   Message pump handler
  VBASE_IMPEXP virtual BOOL RunMessagePump() HKV_OVERRIDE;

  /// \brief
  ///   Windows class registration
  VBASE_IMPEXP virtual BOOL RegisterWindowClass() HKV_OVERRIDE;

  /// \brief
  ///   Windows class de-registration
  VBASE_IMPEXP virtual BOOL UnregisterWindowClass() HKV_OVERRIDE;

  /// \brief
  ///   Window creation
  VBASE_IMPEXP virtual HWND CreateVWindow(const char* name, 
    unsigned int style, int x, int y, int width, int height, HWND parent) HKV_OVERRIDE;
};

/// \brief
///   Returns whether the application wants to close the window
/// 
/// This will return true after the user has pressed the X button on the window or Alt-F4 and
/// VQB_HANDLE_WINDOW_CLOSE wasn't enabled. Has no effect on consoles.
VBASE_IMPEXP BOOL VGLWantClose();


/// \brief
///   The default window proc used by the engine
/// 
/// After processing the relevant messages it calls DefWindowProc.
/// Remember to call this if you install your own window proc.
/// Supported on Windows only.
/// 
/// \param hWnd
///   Window handle
///
/// \param message
///   Message ID
///
/// \param wParam
///   wParam
///
/// \param lParam
///   lParam
VBASE_IMPEXP LRESULT APIENTRY VisionWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


/// \brief
///   Get VGL window
/// 
/// VGLGetWindow returns the window created with VGL
/// Supported on Windows only.
/// 
/// \return
///   HWND hWnd
VBASE_IMPEXP HWND VGLGetWindow(void);

/// \brief
///   Get a flag to check for fullscreen
/// 
/// Supported on Windows only.
/// 
/// \return
///   bool Fullscreen flag
/// 
/// \note
///   This function is specific to windows only.
///   
bool VGLIsFullscreen(void);

/// \brief
///   Set fullscreen flag
/// 
/// Supported on Windows only.
/// 
/// \note
///   This function is specific to windows only.
///   It only sets a flag that can be queried with VGLIsFullscreen()
///   It doesn't actually change any settings.
///   
void VGLSetFullscreenFlag(bool);

/// \brief
///   Manually call the current window proc when using the engine in an embedded view
/// 
/// Use this function to manually call the current window proc when using the engine in an embedded
/// view. VGLWndProcEmbeddedView filters out the messages which are relevant for embedded views and
/// forwards them to the window proc.
/// Supported on Windows only.
/// 
/// \param message
///   message value of message
/// 
/// \param wParam
///   wparam value of message
/// 
/// \param lParam
///   lparam value of message
/// 
/// \return
///   BOOL bProcessed: true if the message was processed
VBASE_IMPEXP BOOL VGLWndProcEmbeddedView(UINT message, WPARAM wParam, LPARAM lParam);

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
