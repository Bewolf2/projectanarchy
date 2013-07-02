
#ifndef VLUA_APIDOC

class VScriptDebug_wrapper
{
public:

  VScriptDebug_wrapper();
  ~VScriptDebug_wrapper();
  
  VScriptDraw_wrapper Draw;
  
  inline void Enable(bool bEnable);
  inline bool IsEnabled();

  inline void PrintLine(const char * szLine, VColorRef color = V_RGBA_WHITE);

  inline void ClearLines();

  inline void PrintAt(float x, float y, const char * szLine, VColorRef color = V_RGBA_WHITE, const char * szFont=NULL);

  inline void PrintAt(const hkvVec3* vPos, const char * szLine, VColorRef color = V_RGBA_WHITE, const char * szFont=NULL);

  inline void SetupLines(int iLines, float fDuration = 2.f);

  inline void Log(const char * szLogMsg);

  inline void Error(const char * szError);
};

#else

/// \brief This wrapper will appear in Lua as the global instance named \b Debug.
/// \par Example
///   \code
///  function OnAfterSceneLoaded()
///    Debug:Enable(true)
///  end
///
///  function OnThink(self)
///    local x,y = Input:GetMousePosition()
///    Debug:PrintAt(x,y, "This is the current mouse position!")
///  end
///   \endcode
class VScriptDebug_wrapper {
public:

 
  /// @name Debug Control
  /// @{
 
  /// \brief Enable or disable all script debug commands (expect Debug:Error and Debug:Log, which will always work)
  /// \note
  ///   Any Debug:PrintLine or Debug.Draw:Line will be effectless if disabled via this switch!
  ///   If you woul like to perfrom any debug interface independent draw operations, please use
  ///   the VScriptRenderer_wrapper (e.g. Render.Draw:Line)
  /// \param enable Set to true to enable all debuggin functions (default for Debug builds!)
  /// \see IsEnabled
  /// \par Example
  ///   \code
  ///     Debug:Enable(true)
  ///   \endcode
  void Enable(boolean enable);
  
  /// \brief Check if the script debug interface is enabled.
  /// \return true if debugging is turned on, otherwise false.
  /// \see Enable
  /// \par Example
  ///   \code
  ///  function OnAfterSceneLoaded(self)
  ///    if Debug:IsEnabled() then
  ///      Debug:Log("Debug interface is enabled.")
  ///    else
  ///      Debug:Log("Debug interface is disabled.")
  ///    end
  ///  end
  ///   \endcode
  boolean IsEnabled();

  /// \brief Print a single text line which will be appended to the output at the viewport.
  /// \param msg The output text.
  /// \param color (\b optional) The color of the text message.
  /// \par Example
  ///   \code
  ///  function OnAfterSceneLoaded(self)
  ///    Debug:PrintLine("Hello, I'm inside the scene!")
  ///  end
  ///   \endcode
  void PrintLine(string msg, VColorRef color = V_RGBA_WHITE);

  /// \brief Clear all present lines at the viewport.
  void ClearLines();

  /// \brief Print text at a specified 2D point.
  /// \param x The screen space x position.
  /// \param y The screen space y postion.
  /// \param msg The output text.
  /// \param color (\b optional) The color of the text message.
  /// \param fontName (\b optional) The name and path of the desired font.
  /// \par Example
  ///   \code
  ///  function OnThink(self)
  ///    local x,y = Input:GetMousePosition()
  ///    Debug:PrintAt(x,y, "X")
  ///  end
  ///   \endcode
  void PrintAt(number x, number y, string msg, VColorRef color = V_RGBA_WHITE, string fontName);

  /// \brief Print text at a specified 3D point.
  /// \param pos The world space position, where the text should appear.
  /// \param msg The output text.
  /// \param color (\b optional) The color of the text message.
  /// \param fontName (\b optional) The name and path of the desired font.
  /// \par Example
  ///   \code
  ///  function OnThink(self)
  ///    Debug:PrintAt(self:GetPosition(), "I'm here!")
  ///  end
  ///   \endcode
  void PrintAt(hkvVec3 pos, string msg, VColorRef color = V_RGBA_WHITE, string fontName);

  /// \brief Setup the appearance of the text lines displayed at the viewport.
  /// \param lines The max number of stacked lines (FIFO).
  /// \param duration (\b optional) The duration of a single line, before it gets cleaned up.
  /// \par Example
  ///   \code
  ///   function OnCreate(self)
  ///     Debug:SetupLines(20, 4)
  ///   end
  ///   \endcode
  void SetupLines(number lines, number duration = 2);

  /// \brief Add a message to the system log.
  /// \note This will always work, independent of the scripting debug switch.
  /// \param log The message to add to the log.
  /// \par Example
  ///   \code
  ///   function OnCreate(self)
  ///     Debug:Log("Created Entity '"..tostring(self).."'")
  ///   end
  ///   \endcode
  void Log(string log);

  /// \brief Add a message to the error log.
  /// \note This will always work, independent of the scripting debug switch.
  /// \param error The message to add to the error log.
  void Error(string error);
  
  /// @} 
  /// @name Public Members
  /// @{
  
  /// \brief Use this member to perform any draw operation.
  /// \par Example
  ///   \code
  ///   function OnThink(self)
  ///     Debug.Draw:BoundingBox(self, Vision.V_RGBA_RED);
  ///   end
  ///   \endcode
  VScriptDraw_wrapper Draw;
  
  /// @}
};

#endif