
#ifndef VLUA_APIDOC

class VScriptApp_wrapper
{
public:

  VScriptApp_wrapper();
  ~VScriptApp_wrapper();
  
  VScriptFileSystem_wrapper FileSystem;
  
  bool LoadScene(const char * szFileName);
  void UnloadScene();
  void ClearScene();

  bool IsInEditor();
  int GetEditorMode();
  
  const char * GetPlatformName();
  
};

//we need the Lua state for this operation so the implementation is native
%native(LoadScript) int VScriptApp_wrapper_LoadScript(lua_State *L);
%{
  SWIGINTERN int VScriptApp_wrapper_LoadScript(lua_State *L)
  {
    IS_MEMBER_OF(VScriptApp_wrapper) //this will move this function to the method table of the specified class

    DECLARE_ARGS_OK;

    //we can ignore arg1 because it is a static function

    GET_ARG(2, const char *, szFileName);
    
    bool bSuccess = false;
    
    if (ARGS_OK)
    {
      bSuccess = VScriptApp_wrapper::LoadScript(L, szFileName);
    }
    
    lua_pushboolean(L, bSuccess ? TRUE : FALSE);
    return 1;
  }
%}

#else

/// \brief Base application related methods. Present in Lua as the global instance \b Application.
/// \par Example
///   \code
///     local mode = Application:GetEditorMode()
///     if mode ~= Vision.EDITOR_PLAY then
///       Debug:PrinLine("Please run in 'Play the Game' mode for full keyboard access!");
///     end
///   \endcode
class VScriptApp_wrapper {
public:

 
  /// @name Scene Control
  /// @{
 
  /// \brief Load the specified scene.
  /// \param sceneName The scene file name.
  /// \return true on success, otherwise false.
  boolean LoadScene(string sceneName);
  
  /// \brief Unload the current scene - perfom only at application exit.
  void UnloadScene();
  
  /// \brief Vclear the scene content, and prepare empty scene for use.	
  void ClearScene();

  /// \brief Check if the application is running in the vForge editor.
  /// \return true if the application runs in the editor, otherwise false.
  /// \see IsInEditor
  boolean GetEditorMode();
  
  /// \brief Checks the current operating mode of the editor.
  /// \return The edot mode as one of the following values:
  ///  - EDITOR_NONE: No editor active.
  ///  - EDITOR_ANIMATING: Editor is in 'Animation' mode.
  ///  - EDITOR_RUN: Editor is in 'Run in Editr' mode.
  ///  - EDITOR_PLAY: Editor is in 'Play the Game' mode.
  /// \see IsInEditor
  /// \par Example
  ///   \code
  ///     local mode = Application:GetEditorMode()
  ///     if mode == Vision.EDITOR_ANIMATING then
  ///       Debug:PrinLine("Please run this sample at least in the 'Run in Editor' mode...");
  ///     end
  ///   \endcode
  number GetEditorMode();
 
  /// @} 
  /// @name General Purpose
  /// @{
  
  /// \brief Loads a script file. Similar to LUA's dofile, but this one uses the vision file system. 
  /// \param filename The script file to load.
  void LoadScript(string filename);
 
  /// @} 
  /// @name Public Members
  /// @{
  
  /// \brief Use this member for file system access.
  /// \par Example
  ///   \code
  ///     local dataDirs = Application.FileSystem:GetDataDirs()
  ///   \endcode
  VScriptFileSystem_wrapper FileSystem;
  
  /// @}
};

#endif