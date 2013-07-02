
#ifndef VLUA_APIDOC

%nodefaultctor VScriptFileSystem_wrapper;
%nodefaultdtor VScriptFileSystem_wrapper;

class VScriptFileSystem_wrapper
{
public:
  
  bool Exists(const char * szFilename, const char * szOptionalPath = NULL);
};

//use the native implmentation to handle the string properly
%native(GetAbsDir) int VScriptFileSystem_wrapper_GetAbsDir(lua_State *L);
%{
  SWIGINTERN int VScriptFileSystem_wrapper_GetAbsDir(lua_State *L)
  {
    IS_MEMBER_OF(VScriptFileSystem_wrapper) //this will move this function to the method table of the specified class

    DECLARE_ARGS_OK;

    //we can ignore arg1 because it is a static function

    GET_ARG(2, const char *, szFileName);
    
    bool bSuccess = false;
    
    if (ARGS_OK)
    {
      char pBuffer[1024]; //create a buffer on the stack
      Vision::File.GetAbsoluteDirectory(pBuffer, szFileName);
      lua_pushstring(L, pBuffer);
    }
    else
    {
      lua_pushnil(L);
    }
    
    return 1;
  }
%}

//Implement GetAllSurfaces native because it returns an array of surfaces
%native(GetDataDirs) int VScriptFileSystem_wrapper_GetDataDirs(lua_State *L);
%{
  SWIGINTERN int VScriptFileSystem_wrapper_GetDataDirs(lua_State *L)
  {
    IS_MEMBER_OF(VScriptFileSystem_wrapper) //this will move this function to the method table of the specified class

    lua_newtable(L);                                        //stack: ..., table
    
    int iCount = Vision::File.GetMaxDataDirectoryCount();

    for(int i=0;i<iCount;++i)
    {
      lua_pushstring(L, Vision::File.GetDataDirectory(i));  //stack: ..., table, string
      lua_rawseti(L, -2, i+1);                         //stack: ..., table
    } 
    
    return 1; //return the table as stack param
  }
%}

#else

/// \brief File system related methods. It appears as member of the \b Application instance with the name \b FileSystem.
/// \par Example
///   \code
///     local dataDirs = Application.FileSystem:GetDataDirs()
///
///     for i=1,#dataDirs do
///       Debug:PrintLine("Data directory "..i..": "..dataDirs[i])
///     end
///   \endcode
class VScriptFileSystem_wrapper {
public:
 
  /// @name File System Access Functions
  /// @{
 
  /// \brief Check whether a file exists or not.
  /// \param filename Name of the file (relative to one of the data directories).
  /// \param optionalPath (\b optional) Optional directory that the filename is relative to. This is prepended to the filename.
  /// \return Returns true if the file exists at the specified location, otherwise false.
  /// \par Example
  ///   \code
  ///     Application.FileSystem:Exists("MyHero.model", "models")
  ///   \endcode
  boolean Exists(string filename, string optionalPath = nil);

  /// \brief Helper function that takes a relative directory name and combines it with the current directory to make an absolute path.
  /// \note If you call this after engine init then the current directory might already have changed and not be the application directory anymore!
  /// \param relativeDirectory (\b optional) The relative directory.
  /// \return Returns the absolute directory as string.
  /// \par Example
  ///   \code
  ///     local absDir = Application.FileSystem:GetAbsDir("models")
  ///   \endcode
  string GetAbsDir(string relativeDirectory);
  
  /// \brief Get all present data directories.
  /// \return A table containing string entries from index 1 to LEN (could be an empty table too).
  /// \par Example
  ///   \code
  ///     local dataDirs = Application.FileSystem:GetDataDirs()
  ///   
  ///     Debug:PrintLine("Detected "..#dataDirs.." data directories.")
  ///
  ///     for i=1,#dataDirs do
  ///       Debug:PrintLine("Data directory "..i..": "..dataDirs[i])
  ///     end
  ///   \endcode
  table GetDataDirs();
  
  /// @}
};

#endif