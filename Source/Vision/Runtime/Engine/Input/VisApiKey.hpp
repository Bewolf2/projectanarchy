/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiKey.hpp

#ifndef DEFINE_VISAPIKEY
#define DEFINE_VISAPIKEY


#ifdef SUPPORTS_KEYBOARD


/// \brief
///   Class for accessing keyboard status. The global instance of this class can be accessed via
///   Vision::Key.
/// 
/// The key codes VGLK_XXX are virtual keys to provide method/layout independent key codes.
/// 
/// To translate a VGLL_XXX code to ASCII character(s), do a lookup in a table using VGLGetKeyTable
/// directly.
/// 
/// Alternatively you can also use DirectInput scancodes directly using the corresponding *_SC
/// functions 
class VisKey_cl
{
public:

  /// \brief
  ///   Initialize keyboard handling
  /// 
  /// This is usually called by the application class (see for instance VisionApp_cl::InitInput())
  /// 
  /// \param method
  ///   The preferred method for input. If set to VGL_DEFAULTINPUT, the  best method for the
  ///   platform will be used.
  ///   \li VGL_DEFAULTINPUT: Let VGL select the best method
  /// 
  ///   \li VGL_WINDOWSINPUT: Force Windows keyboard input
  /// 
  ///   \li VGL_DIRECTINPUT: Force DirectInput keyboard input
  /// 
  /// \param bExclusive
  ///   If enabled, direct input will be initialized in exclusive mode (default)
  /// 
  /// \return
  ///   RETVAL result: VERR_NOERROR if successful or VERR_NOWINDOW if there was no window with
  ///   which the keyboard could be associated.
  /// 
  /// \note
  ///   Call this function after Vision.Video.SetMode
  /// 
  /// \note
  ///   The VGL_DIRECTINPUT method is the one which you should use for final release builds on
  ///   Win32 based system. This method causes less problems on lower frame rates. On the other
  ///   hand VGL_WINDOWSINPUT does not mess with the mouse input during debug sessions.
  VISION_APIFUNC RETVAL Init(SLONG method = VGL_DEFAULTINPUT, bool bExclusive = true);
  
  
  /// \brief
  ///   Deinitialize keyboard handling
  /// 
  /// DeInit releases the resources needed for keyboard handling
  /// 
  /// \return
  ///   RETVAL result: VERR_NOERROR if successful or VERR_NOWINDOW if there was no window with
  ///   which the keyboard could be associated.
  /// 
  /// \note
  ///   Call this before calling Vision.Video.DeInit
  VISION_APIFUNC RETVAL DeInit();

  
  /// \brief
  ///   Return the pressed status of a specific key code
  /// 
  /// If the key is marked as SetSingleHit(TRUE), this function will reset the pressed status for
  /// this key.
  /// 
  /// \param key
  ///   The key for which the status should be retrieved. If this value is set to 0, then the
  ///   IsPressed function will return whether any key is currently pressed.
  /// 
  /// \return
  ///   BOOL: TRUE if key is pressed, FALSE if not.
  VISION_APIFUNC BOOL IsPressed(SLONG key = 0);
  

  /// \brief
  ///   Return the pressing status of a specific key code
  /// 
  /// \param key
  ///   The key for which the status should be retrieved. 
  /// \return
  ///   BOOL: TRUE if key is pressed, FALSE if not.
  /// \note
  ///   Will 'NOT' reset the key to logical unpressed status if it is registered with the VGLKeyOnce
  ///   function
  VISION_APIFUNC BOOL IsKeyDownBitSet(SLONG key);


  /// \brief
  ///   Scan code version of IsPressed. Requires DirectInput keyboard method
  VISION_APIFUNC BOOL IsPressed_SC(SLONG key = 0);
  
  
  /// \brief
  ///   Set (or reset) SingleHit status of a key
  /// 
  /// Keys flagged as SetSingleHit will report the IsPressed status only once per frame and only if
  /// the key's status changes from unpressed to pressed.
  /// 
  /// \param key
  ///   Key code of the key.
  /// 
  /// \param keyOnceStatus
  ///   TRUE if SingleHit/KeyOnce is set (only recognized once per press).
  /// 
  /// \return
  ///   RETVAL result : VERR_NOERROR if successful, or VERR_ILLEGALVALUE if the keycode is not
  ///   valid.
  VISION_APIFUNC RETVAL SetSingleHit(SLONG key, BOOL keyOnceStatus = TRUE);
  
  /// \brief
  ///   Scancode version of SetSingleHit. Requires DirectInput keyboard method
  VISION_APIFUNC RETVAL SetSingleHit_SC(SLONG key, BOOL keyOnceStatus = TRUE);
  
  
  /// \brief
  ///   Return the status of a key set via SetSingleHit
  VISION_APIFUNC BOOL IsSingleHit(SLONG key);

  /// \brief
  ///   Scancode version of IsSingleHit. Requires DirectInput keyboard method
  VISION_APIFUNC BOOL IsSingleHit_SC(SLONG key);


  /// \brief
  ///   Toggles the SingleHit status of a key
  VISION_APIFUNC RETVAL ToggleSingleHit(SLONG key);


  /// \brief
  ///   Get the method the keyboard handling has been initialized with. The return value is either
  ///   VGL_DIRECTINPUT or VGL_WINDOWSINPUT
  VISION_APIFUNC RETVAL GetMethod();

  /// \brief
  ///   Do not use
  VISION_APIFUNC void SetKeyboardStatus(BOOL status);

  
  /// \brief
  ///   Do not use
  VISION_APIFUNC BOOL GetKeyboardStatus();

private:
  BOOL m_KeyBoardStatus;
  bool m_SaveStatus[256];
};


#elif defined(_VISION_XENON)
  #include <xtl.h>
  #include <xgraphics.h>
  #include <stdio.h>
  #define CONSOLE_ENABLE_BUTTON   XINPUT_GAMEPAD_START

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
