/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionConsoleManager.hpp

#ifndef DEFINE_VISIONCONSOLEMANAGER
#define DEFINE_VISIONCONSOLEMANAGER


#define MAX_EDIT_CHARS 512

/// \brief
///   Interface class that handles and displays the console
///
/// You can implement your own console by implement this interface
/// and registering a console instance via Vision::SetConsoleManager.
class IVConsoleManager : public VRefCounter
{
public:
  ///
  /// @name Constructors / Destructors
  /// @{
  ///

  /// \brief
  ///   Constructor of the VisConsoleManager class
  VISION_APIFUNC IVConsoleManager();

  ///
  /// @}
  ///

  /// \brief
  /// Initializes the VisConsoleManager class
  /// Sets action manager and calls back.
  VISION_APIFUNC virtual void Init();

  /// \brief
  /// Deinitializes the VisConsoleManager class
  /// Removes action manager and calls back.
  VISION_APIFUNC virtual void DeInit();

  /// \brief
  /// Handles the complete console 
  /// Updates the console per frame
  VISION_APIFUNC virtual void Update() = 0;

  /// \brief
  /// render console (into main view)
  VISION_APIFUNC virtual void Render() = 0; 

  /// \brief
  /// Outputs a single line. 
  /// The string may contain line breaks (\n) to be printed 
  ///
  /// \param text
  ///   Text to be printed.
  VISION_APIFUNC virtual void OutputTextLine(const char *text) = 0;

  ///
  /// @name Debug Console
  /// @{
  ///

  /// \brief
  ///   Enables or disables the debug console functionality
  /// 
  /// if you press the console key ('~' on US keyboard, '^' on German keyboard) the debug console
  /// will open if settings allow.
  /// 
  /// \param bAllow
  ///   The new status. If allowed, the console key will open the  debug console, if disallowed,
  ///   pressing the key has no effect
  /// 
  /// \note
  ///   By default, the console functionality is enabled for debug and development builds of Vision,
  ///   and disabled for release builds. Calling this function will override this setting.
  /// 
  /// \note
  ///   We do not recommended to ship a game with the debug console enabled, unless you intend this to
  ///   be a way for users to customize your game at runtime. In this case, you should make sure you're
  ///   exposing only that functionality which is safe for the users to access. In all other cases, make
  ///   sure the debug console is disabled.
  /// 
  /// \sa VisConsole_cl::Show()
  VISION_APIFUNC virtual void SetAllowed(bool bAllow);

  /// \brief
  ///   Determines whether console may be opened. See SetAllowed
  VISION_APIFUNC virtual bool IsAllowed() const;

  /// \brief
  ///   Shows or hides the debug console
  VISION_APIFUNC virtual void Show(bool bShow=true) = 0;

  /// \brief
  ///   Indicates whether the console is enabled (status of Show method)
  VISION_APIFUNC virtual bool IsVisible() const;

  /// \brief
  /// Check whether console window need to open or not.
  VISION_APIFUNC virtual bool RequestsToOpen() = 0;

protected:
  // variables
  bool m_bIsActive;
  bool m_bAllowConsole;

  VActionManager *m_pActionManager;
  VReceiver m_Receiver;
};

typedef VSmartPtr<IVConsoleManager> IVConsoleManagerPtr;

/// \brief
///   Default console manager implementation of the Vision Engine
///
/// You can also implement own console managers if desired. Please refer to the "Debugging and
/// Profiling" chapter in the programmer's documentation for more details about this topic.
class VisionConsoleManager_cl : public IVConsoleManager
{
public:
  ///
  /// @name Constructors / Destructors
  /// @{
  ///

  /// \brief
  ///   Constructor of the VisionConsoleManager_cl class
  VISION_APIFUNC VisionConsoleManager_cl();

  ///
  /// @}
  ///


  VISION_APIFUNC virtual void Init() HKV_OVERRIDE;
  VISION_APIFUNC virtual void DeInit() HKV_OVERRIDE;

  /// \brief
  ///   Handles command processing in this console.
  ///
  /// The following commands are recognized directly by this console manager:
  /// - \b exit or \b quit: exits the Vision application
  /// - \b clear: clears the console command history
  /// - \b save: saves the console command history to the file \c console_output.txt in the
  ///   currently configured Vision output directory.
  ///
  /// Any other commands are forwarded to the action manager that's set for this console. A 
  /// warning message is printed to the console if no action can handle the command.
  ///
  /// \param pszCommand
  ///   the command to execute
  VISION_APIFUNC void OnExecuteCommand(const char *pszCommand);

  VISION_APIFUNC virtual void Update() HKV_OVERRIDE;
  VISION_APIFUNC virtual void Render() HKV_OVERRIDE;

  /// \brief
  ///   Saves the command history to the specified file.
  VISION_APIFUNC void SaveHistory(const char *filename);

  /// \brief
  ///   Wrapper function to handle key repeat, etc 
  VISION_APIFUNC BOOL IsKeyPressed(int key_id);

  /// \brief
  ///   Prints a text line to the console
  VISION_APIFUNC void OutputTextLine(const char *text);

  /// \brief
  ///   Sets the height (in pixels) of the console and the alignment
  VISION_APIFUNC void SetConsoleHeight(int height, BOOL topAlign = FALSE);

  /// \brief
  ///   Set tabulators for text output containing tab char
  VISION_APIFUNC void SetTabs(int tabcount, const int *charpos, char tabspace=' ');

  VISION_APIFUNC void PrintText(IVRender2DInterface *pRI);

  ///
  /// @name Edit functions
  /// @{
  ///

  /// \brief
  /// Copies current line to undo memory
  VISION_APIFUNC void SaveUndo();

  /// \brief
  ///   Undoes the changes made to the current line
  VISION_APIFUNC void EditUndo();

  /// \brief
  ///   Pastes the clipboard contents
  VISION_APIFUNC void EditPaste();

  /// \brief
  ///   Copies the selected text to the clipboard and removes it
  VISION_APIFUNC void EditCut();

  /// \brief
  ///   Copies the selected text to the clipboard
  VISION_APIFUNC void EditCopy();

  VISION_APIFUNC void DeleteSelection();
  VISION_APIFUNC void CursorEnd(bool shift);
  VISION_APIFUNC void CursorHome(bool shift);
  VISION_APIFUNC void DeleteCharacter(int pos);
  VISION_APIFUNC void CursorLeft(bool shift);
  VISION_APIFUNC void CursorRight(bool shift);
  VISION_APIFUNC void LoadFont();
  VISION_APIFUNC void TrimEditLine();
  VISION_APIFUNC void InsertCharacter(char c);
  VISION_APIFUNC void ClearEditLine();

  ///
  /// @}
  ///

  ///
  /// @name History functions
  /// @{
  ///

  VISION_APIFUNC void GetNextHistory();
  VISION_APIFUNC void GetPrevHistory();
  VISION_APIFUNC void ReleaseHistory();
  VISION_APIFUNC void AddHistoryEntry(const char *command, bool isCommand);

  ///
  /// @}
  ///

  /// \brief
  ///   Shows or hides the debug console
  VISION_APIFUNC virtual void Show(bool bShow=true) HKV_OVERRIDE;

  /// \brief
  /// Check whether console window need to open or not.
  VISION_APIFUNC virtual bool RequestsToOpen() HKV_OVERRIDE;

protected:
  int m_height;
  BOOL m_topAlign;
  int m_yRelPos;
  int m_HorizOfs;
  int m_VertOfs;
  int m_MaxVisLines;
  float m_blinkStatus;
  BOOL m_Insert;
  int m_TabPos[128];
  char m_TabSpaceChar;

  // History
  int m_HistoryIndex;
  int m_SelHistoryIndex;
  int m_HistoryScollIndex;

  DynArray_cl<char *>m_HistoryText;
  DynArray_cl<bool> m_isCommand;

  char m_EditLine[MAX_EDIT_CHARS+1];
  VString m_sUndoMem;
  VString m_sClipboard;

  float m_KeyDelay[256];
  int m_CursorPos;
  int m_CommandLen;
  int m_selectionStart;
  int m_iFontHeight, m_iFontWidth;

  VInputMap *m_pInputMap;
  ///
  /// @}
  ///
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
