/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFileHelper.hpp

#ifndef DC_VFILEHELPER_HPP
#define DC_VFILEHELPER_HPP

#include <Vision/Runtime/Base/String/VString.hpp>

#include <Vision/Runtime/Base/System/IO/System/VFileData.hpp>


#if defined (_VISION_PS3)
#include <sysutil/sysutil_syscache.h>
#include <sysutil/sysutil_gamecontent.h>
#endif



/// \brief
///   File enumeration callback function type
/// \param dir
///   the directory whose contents are enumerated
/// \param filename
///   the current file or (sub-)directory
/// \param pFileInfo
///   extended information about the current file/(sub-)directory
/// \param context
///   the context pointer passed to the enumeration function that called this callback
typedef void (*ENUMFILES_CALLBACK)(const char *dir, const char *filename, VFileData *pFileInfo, void *context);

// forward declaration
VBASE_IMPEXP IVFileStreamManager* VBase_GetFileStreamManager();

/// \brief
///   Helps with file path string handling
/// 
class VPathHelper
{
public:
  ///
  /// @name Path Helpers
  /// @{
  ///

  /// \brief
  ///   Turns slashes into backslashes in the passed string buffer
  VBASE_IMPEXP static void FrontToBackSlash(char *ch);

  /// \brief
  ///   Turns backslashes into slashes in the passed string buffer
  VBASE_IMPEXP static void BackToFrontSlash(char *ch);

  /// \brief
  ///   Converts a filename to compatible filename, i.e. slashes to platform specific separators,
  ///   remove invalid characters etc.
  /// \param szDest
  ///   buffer receiving the target path. Must be a char buffer that is large 
  ///   enough to hold any file path (e.g. char[FS_MAX_PATH])
  /// \param szSrc
  ///   the source  path
  /// \param szDefaultExt
  ///   extension to add if the source path does not have an extension. Can be \c NULL.
  /// \return
  ///   \c true if the path was successfully converted; \c false if not
  VBASE_IMPEXP static bool ConvertFilename(char *szDest, const char *szSrc, const char *szDefaultExt);

  /// \brief
  ///   Combines a path and filename, turns slashes to platform specific separators and resolves ".."
  ///
  /// \param res
  ///   Result string. Must be a char buffer that is large enough (e.g. char[FS_MAX_PATH])
  ///
  /// \param szDir
  ///   The source directory
  ///
  /// \param szFileName
  ///   The filename to concatenate with the path
  ///
  /// \param bPreserveLeadingSlash
  ///   If true, filenames that start with '\' (i.e. base data files) will also put this slash into the result
  ///
  VBASE_IMPEXP static void CombineDirAndFile(char *res, const char* szDir, const char* szFileName, bool bPreserveLeadingSlash=false);

  /// \brief
  ///   Combine a path and filename, turns slashes to platform specific separators and resolves ".."
  ///
  /// \param szDir
  ///   The source directory
  ///
  /// \param szFileName
  ///   The filename to concatenate with the path
  ///
  /// \param bPreserveLeadingSlash
  ///   If true, filenames that start with '\' (i.e. base data files) will also put this slash into the result
  ///
  /// \return
  ///   Returns a Vstring containing the combined result.
  ///  
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.  
  ///
  VBASE_IMPEXP static inline VString CombineDirAndFile(const char* szDir, const char* szFileName, bool bPreserveLeadingSlash=false)
  {
    char szBuffer[FS_MAX_PATH];
    CombineDirAndFile(szBuffer, szDir, szFileName, bPreserveLeadingSlash);
    return VString(szBuffer);
  }

  /// \brief
  ///   Combine a path and another path, turns slashes to platform specific separators and resolves ".."
  ///
  /// \param res
  ///   Result string. Must be a char buffer that is large enough (e.g. char[FS_MAX_PATH])
  ///
  /// \param szBaseDir
  ///   The base directory, will always be the first part in the string ( so put UNC paths here)
  ///
  /// \param szAddDir
  ///   The path to be concatenated with the path. UNC paths in this parameter are not supported and will
  ///   return a zero string in the parameter res. This parameter has to be a relative path.
  ///
  VBASE_IMPEXP static void CombineDirAndDir(char *res, const char* szBaseDir, const char* szAddDir);

  /// \brief
  ///   Combine a path and another path, turns slashes to platform specific separators and resolves ".."
  ///
  /// \param szBaseDir
  ///   The base directory, will always be the first part in the string ( so put UNC paths here)
  ///
  /// \param szAddDir
  ///   The path to be concatenated with the path. UNC paths in this parameter are not supported and will
  ///   return a zero string in the parameter res. This parameter has to be a relative path.
  ///  
  /// \return
  ///   Returns a Vstring containing the combined result.
  ///
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.  
  ///
  VBASE_IMPEXP static inline VString CombineDirAndDir(const char* szBaseDir, const char* szAddDir)
  {
    char szBuffer[FS_MAX_PATH];
    CombineDirAndDir(szBuffer, szBaseDir, szAddDir);
    return VString(szBuffer);
  }

  /// \brief
  ///   Computes the hash value from the filename of szPath.
  /// \param szPath
  ///   the path to hash
  /// \return
  ///   the hash of the filename part of \c szPath, as computed by VStringUtil::Hash()
  VBASE_IMPEXP static int GetHash(const char *szPath);

  /// \brief
  ///   Compares two paths case-insensitive, ignoring the direction of slashes.
  /// \param szPath1
  ///   first path to compare
  /// \param szPath2
  ///   second path to compare
  /// \return
  ///   \c true if the paths are equal according to the above criteria, \c false if not
  VBASE_IMPEXP static bool CompareNoSlashes(const char *szPath1, const char *szPath2);

  /// \brief
  ///   Resolves all possible ".." in the path. Accepts \ and /
  ///   
  /// \param szRes
  ///   The buffer to fill. Has to be allocated!
  ///
  /// \param szPath
  ///   The actual path to resolve
  ///   
  /// \return
  ///   A VString containing the resolved path (no more '..' inside the path)
  VBASE_IMPEXP static void ResolvePath(char *szRes, const char* szPath);

  /// \brief
  ///   Resolves all possible ".." in the path. Accepts \ and /
  ///   
  /// \param szPath
  ///   The actual path to resolve
  ///   
  /// \return
  ///   A VString containing the resolved path (no more '..' inside the path)
  ///   
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.  
  VBASE_IMPEXP static VString ResolvePath(const char* szPath)
  {
    char szBuffer[FS_MAX_PATH];
    ResolvePath(szBuffer, szPath);
    return VString(szBuffer);
  }

  /// \brief
  ///   Makes szDestPath relative to szStartPath. The result might start with "..\"
  ///  
  /// \param szRes
  /// 	The result string containing the relative path. Has to be allocated!
  ///   
  /// \param szStartPath
  /// 	Starting point of your relative path.
  ///   
  /// \param szDestPath
  ///   The resulting string is the relative path between szDestPath and szStartPath.
  VBASE_IMPEXP static void MakePathRelative(char *szRes, const char* szStartPath, const char *szDestPath);

  /// \brief
  ///   Makes szDestPath relative to szStartPath. The result might start with "..\"
  ///   
  /// \param szStartPath
  /// 	Starting point of your relative path.
  ///   
  /// \param szDestPath
  ///   The resulting string is the relative path between szDestPath and szStartPath.
  ///   
  /// \return
  /// 	A VString containing the relative path.
  ///   
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.  
  VBASE_IMPEXP static inline VString MakePathRelative(const char* szStartPath, const char *szDestPath)
  {
    char szBuffer[FS_MAX_PATH];
    MakePathRelative(szBuffer, szStartPath, szDestPath);
    return VString(szBuffer);
  }

  /// \brief
  ///   Extracts the directory from a file name.
  ///
  /// \param szFilename
  ///   The path to the file (must not be a directory)
  ///
  /// \param szDir
  ///   The extracted directory without the filename.
  ///
  /// \see
  ///   VPathHelper::GetFileDirEx
  ///
  /// \note
  ///   If the filename is already a directory then the parent directory will be returned(!)
  VBASE_IMPEXP static void GetFileDir(const char* szFilename, char* szDir);

  /// \brief
  ///   Extracts and returns the directory from a file name.
  ///
  /// \param szFilename
  ///   The path to the file (must not be a directory)
  ///
  /// \see
  ///   VPathHelper::GetFileDirEx
  ///
  /// \return
  ///   A VString containing the directory. If the filename is already a directory then the parent directory will be returned(!)
  ///  
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.
  VBASE_IMPEXP static inline VString GetFileDir(const char *szFilename)
  {
    char szBuffer[FS_MAX_PATH];
    GetFileDir(szFilename, szBuffer);
    return VString(szBuffer);
  }

  /// \brief
  ///   Extracts the directory from a file name or a directory path
  ///
  /// This method is similar to GetFileDir() except, that it also works correctly if the passed
  /// string is a directory (and not a file). In that case the directory is returned unchanged.
  /// However, in comparison to GetFileDir() this method needs to perform a file system operation
  /// on the operating system, bypassing the file manger of the engine.
  ///
  /// \param szFilename
  ///   The path to the file (can be a directory)
  ///
  /// \param szDir
  ///   The extracted directory without the filename.
  /// 
  /// \note
  ///   If the filename is already a directory it will be returned unchanged.
  VBASE_IMPEXP static void GetFileDirEx(const char* szFilename, char* szDir);

  /// \brief
  ///   Extracts and returns the directory from a file name or a directory path
  ///
  /// This method is similar to GetFileDir() except, that it also works correctly if the passed
  /// string is a directory (and not a file). In that case the directory is returned unchanged.
  /// However, in comparison to GetFileDir() this method needs to perform a file system operation
  /// on the operating system, bypassing the file manger of the engine.
  ///
  /// \param szFilename
  ///   The path to the file (can be a directory)
  /// 
  /// \return
  ///   A VString containing the directory. If the filename is already a directory it will be returned unchanged.
  ///  
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.
  VBASE_IMPEXP static inline VString GetFileDirEx(const char *szFilename)
  {
    char szBuffer[FS_MAX_PATH];
    GetFileDirEx(szFilename, szBuffer);
    return VString(szBuffer);
  }

  /// \brief
  ///   Extracts the parent directory of a file name. Truncates the string.
  ///   Resulting path will end in slash
  ///   To use this function with a directory only, the directory must end with a backslash or slash
  VBASE_IMPEXP static void GetParentDir(char* szPath);

  /// \brief
  ///   Extracts the parent dir from a file name
  ///   Resulting path will end in slash
  ///   To use this function with a directory only, the directory must end with a backslash or slash
  VBASE_IMPEXP static void GetParentDir(const char* szFilename, char* szDir);

  /// \brief
  ///   If szSubDirOrFile is a directory, this function returns true if it is a subdirectory of szParentDir.
  ///   If szSubDirOrFile is a file, it returns true if the file's directory is a subdirectory of szParentDir.
  VBASE_IMPEXP static bool IsSubdirectoryOf(const char *szSubDirOrFile, const char *szParentDir);

  /// \brief
  ///   Gets the byte position of the '.' used for file extension. -1 if there is no extension
  ///   
  /// \param szFilename
  /// 	The filename used to find the extension
  /// 	
  /// \return
  /// 	The byte position of the extension dot, -1 if there is no extension.
  ///   
  /// \note 
  ///   Use VString::GetUTF8CharacterIndex to convert a byte index to glyph index.
  VBASE_IMPEXP static int GetExtensionPos(const char* szFilename);

  /// \brief
  ///   Appends a file extension if the file has none, or replaces the existing extension otherwise
  VBASE_IMPEXP static void AddExtension(char* szRes, const char* szFilename, const char* szExt);

  /// \brief
  ///   Appends a file extension if the file has none, or replaces the existing extension otherwise
  ///  
  /// \param szFilename
  ///   The filename (can contain path and extension).
  /// 
  /// \param szNewExt
  /// 	The intended extension without the leading dot.
  /// 
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.
  VBASE_IMPEXP static inline VString AddExtension(const char* szFilename, const char* szNewExt)
  {
    char szBuffer[FS_MAX_PATH];
    AddExtension(szBuffer, szFilename, szNewExt);
    return VString(szBuffer);
  }

  /// \brief
  ///   Converts the extension to lower case.
  VBASE_IMPEXP static inline VString ExtensionToLower(const char * szFileName)
  {
    int iPos = GetExtensionPos(szFileName);
    VString fileName(szFileName);
    if(iPos>0)
    {
      iPos = VString::GetUTF8CharacterIndex(fileName, iPos);
      fileName.ToLower(iPos);
    }
    return fileName;
  }


  /// \brief
  ///   Takes a filename string and returns a new string with a new extension
  /// 
  /// You must include a . for the new extension
  /// 
  /// The function returns a new string buffer that has to be de-allocated via VBaseDealloc() at
  /// some point.
  ///
  /// \param szFilename
  ///   Input filename.
  ///
  /// \param szNewExt
  ///   The new file extension.
  ///
  /// \returns
  ///   New string with the new file extension.
  /// 
  /// \example
  ///   \code
  ///   ReplaceExtension("c:\xyz\a.b.v3d", ".lit") -> "c:\xyz\a.b.lit"
  ///   ReplaceExtension("c:\xyz\a", ".lit")       -> "c:\xyz\a.lit"
  ///   \endcode
  VBASE_IMPEXP static char* ReplaceExtension(const char* szFilename, const char* szNewExt);

  /// \brief
  ///   This overload of ReplaceExtension does not allocate memory, instead it writes to szResult.
  ///   Same as AddExtension
  inline static void ReplaceExtension(char *szResult, const char* szFilename, const char* szNewExt)
  {
    AddExtension(szResult, szFilename, szNewExt);
  }

  /// \brief
  ///   Extracts the file extension from the file
  VBASE_IMPEXP static bool GetExtension(char* szRes, const char* szFilename);

  /// \brief
  ///   Returns the pointer to file extension (or NULL for no file extension)
  VBASE_IMPEXP static const char* GetExtension(const char* szFilename);

  /// \brief
  ///   Compares the file extension. Returns false if filename has no extension
  VBASE_IMPEXP static bool HasExtension(const char* szFilename, const char *szExt);

  /// \brief
  ///   Removes the file extension from the filename
  /// 
  /// Copies the filename pointed to by szFilename to the buffer pointed to by szRes, excluding
  /// the file extension.
  /// 
  /// Returns true if there was a file extension to be stripped off, otherwise false.
  /// 
  /// Note that the destination buffer should have at least the size of the source buffer for the
  /// function to succeed.
  ///
  /// \param szRes
  ///   The target buffer (will containt the filename without extension)
  ///
  /// \param szFilename
  ///   The source filename.
  VBASE_IMPEXP static bool GetFilenameNoExt(char* szRes, const char* szFilename);

  /// \brief
  ///   Gets the filename from a path, e.g. "wall.dds" for "worlds\textures\wall.dds". This version
  ///   does not allocate memory but just returns a pointer that points into the memory inside
  ///   szPathname
  VBASE_IMPEXP static const char* GetFilename(const char* szPathname);

  /// \brief
  ///   Extends the base filename (without extension) by the given string, keeping 
  ///   an extension intact if present.
  /// 
  /// \param szRes
  ///   The target buffer (will contain the new extended filename).
  ///
  /// \param szFilename
  ///   Input filename.
  ///
  /// \param szBaseExt
  ///   The extension to the base filename.
  ///
  /// \example
  ///   \code
  ///   ExtendBaseFilename(szRes, "c:\xyz\a.b", "_ext") --> "c:\xyz\a_ext.b"
  ///   ExtendBaseFilename(szRes, "c:\xyz\a.b.c", "_ext") -> "c:\xyz\a.b_ext.c"
  ///   \endcode
  VBASE_IMPEXP static void ExtendBaseFilename(char* szRes, const char* szFilename, const char* szBaseExt);

  /// \brief
  ///   Returns whether the directory is absolute ( Absolute directories start with \\ or
  ///   contain a : )
  VBASE_IMPEXP static bool IsAbsolutePath(const char *szDir);

  /// \brief
  ///   Takes an input directory (NULL, "xyz", or "c:\xyz") and makes it an absolute directory by
  ///   taking the current directory into account.
  ///
  /// \param szDir
  ///   The input directory.
  ///
  /// \param szTmp
  ///   A temporary directory string; must be large enough to hold the output string.
  /// 
  /// \return
  ///   Absolute directory (either szDir or szTmp) or NULL on failure.
  VBASE_IMPEXP static const char* MakeAbsoluteDir(const char *szDir, char *szTmp);

  /// \brief
  ///   Takes an input directory (NULL, "xyz", or "c:\xyz") and makes it an absolute directory by
  ///   taking the current directory into account.
  ///
  /// \param szDir
  ///   The input directory.
  /// 
  /// \return
  ///   Absolute directory (either szDir or szTmp) or an empty VString on failure.
  ///  
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.
  VBASE_IMPEXP static inline VString MakeAbsoluteDir(const char *szDir)
  {
    char szBuffer[FS_MAX_PATH];
    
    if(IsAbsolutePath(szDir))
      return VString(szDir);

    MakeAbsoluteDir(szDir, szBuffer);
    return VString(szBuffer);
  }

  /// \brief
  ///   Checks if a given file matches a search specification.
  ///
  /// \param szFilename
  ///   The filename
  ///
  /// \param szSpec
  ///   The search spec (e.g. "file.*", "*.*", "tex_?.png", etc).
  ///
  VBASE_IMPEXP static bool IsMatch(const char *szFilename, const char *szSpec);

  /// \brief
  ///   Removes all ambiguous parts of a path like starting or trailing slashes, double slashes, etc
  ///   don't use the resulting path for opening files, it will not work as case sensitivity is lost
  /// 
  /// \param szPath
  ///   the path to fix
  ///
  /// \param szResult
  ///   a buffer to write the result to
  VBASE_IMPEXP static void CanonicalizePath(const char* szPath, char* szResult);


  ///
  /// @}
  ///


#if defined (_VISION_PS3)

  /// \brief
  ///   Set the absolute path to the system cache
  ///
  /// \param sysCachePath
  ///   The absolute path to the system cache retrieved by cellSysCacheMount
  VBASE_IMPEXP static void SetSysCachePath(const char * sysCachePath);

  /// \brief
  ///   Set the absolute path to the game content
  ///
  /// \param gameContentPath
  ///   The absolute path to the game content retrieved by cellGameContentPermit
  VBASE_IMPEXP static void SetGameContentPath(const char * gameContentPath);

private:
  static char s_sGameContentPath[CELL_GAME_PATH_MAX];
  static char s_sGameContentPathBS[CELL_GAME_PATH_MAX];
  static int s_iGameContentPathLength;

  static char s_sSysCachePath[CELL_SYSCACHE_PATH_MAX];
  static char s_sSysCachePathBS[CELL_SYSCACHE_PATH_MAX];
  static int s_iSysCachePathLength;

#endif


};


/// \brief
///   Helps with native file handling (i.e. not the FileStreamManager)
/// 
/// Class with helper functions for files and path strings working directly on the OS level
/// 
/// These functions are used when tools need to have direct access to files and not through the
/// FileStreamManager
/// 
/// The OS wrapper helper functions assume absolute path names. If you give relative file names the
/// current directory will be used 
class VFileHelper : public VPathHelper
{
  VFileHelper() {}
public:

  ///
  /// @name Os Wrappers
  /// @{
  ///

  /// \brief
  ///   Deletes a file. If bForce is specified it will also delete read-only files.
  VBASE_IMPEXP static bool Delete(const char *szFileName, bool bForce = false);

  /// \brief
  ///   Returns whether a file exists. szDir is the absolute path to the file
  VBASE_IMPEXP static bool Exists(const char *szFileName);

  /// \brief
  ///   Returns true, if the read-only flag could be retrieved. bIsReadOnly then contains the flags value. If the file does not exist, 'GetReadonlyFlag' returns false.
  VBASE_IMPEXP static bool GetReadonlyFlag(const char *szFileName, bool& bIsReadOnly);

  /// \brief
  ///   Sets the read-only flag of a file to true/false. Not supported on Xbox360.
  VBASE_IMPEXP static bool SetReadonlyFlag(const char *szFileName, bool bReadOnly = false);

  /// \brief
  ///   Gets information about the modification time of a file
  VBASE_IMPEXP static bool GetModifyTime(const char* szFileName, VFileTime& t);

  /// \brief
  ///   Copies a file
  VBASE_IMPEXP static bool Copy(const char *szSrcName, const char *szDestName, bool bFailIfExists = true);

  /// \brief
  ///   Enumerates files in a directory. Only implemented for Win32
  VBASE_IMPEXP static bool EnumFiles(const char *szDir, const char *szFileSpec, ENUMFILES_CALLBACK pCallback, void *pContext);

  /// \brief
  ///   Enumerates files in a directory and subdirectories. Only implemented for Win32
  VBASE_IMPEXP static bool EnumFilesRecursive(const char *dir, const char *file_spec, ENUMFILES_CALLBACK pCallback, void *pContext);

  /// \brief
  ///   Searches a directory for a file. Only implemented for Win32
  VBASE_IMPEXP static bool FindFile(const char *szDir, const char *szSpec, VString *pRes = NULL);

  /// \brief
  ///   Creates a directory, and all necessary directories above it that are necessary 
  //    Only implemented for Win32 and Xbox360
  VBASE_IMPEXP static bool MkDirRecursive(const char *dirname);
  
  /// \brief
  ///   Creates an empty file
  //    Only implemented for Win32 and Xbox360
  VBASE_IMPEXP static bool CreateFile(const char* szFileName);
  
  ///
  /// @}
  ///

  ///
  /// @name Directory Helpers (not available on all platforms)
  /// @{
  ///
  
  /// \brief
  ///   Checks whether the directory exists
  VBASE_IMPEXP static bool ExistsDir(const char *szDirName);

  /// \brief
  ///   Creates a new directory
  VBASE_IMPEXP static bool MkDir(const char *szDirName);
  
  /// \brief
  ///   Changes to a directory
  VBASE_IMPEXP static bool ChDir(const char *szDirName);

  /// \brief
  ///   Outputs the current working directory
  ///
  /// \param szOutDir
  ///   The output buffer for the working directory string.
  ///
  /// \param iMaxSize
  ///   Size of the output buffer.
  ///
  /// \return
  ///   Returns true if working directory has been successfully determined.
  VBASE_IMPEXP static bool GetCurrentWorkingDir(char *szOutDir, int iMaxSize);

  /// \brief
  ///   Removes an empty directory
  VBASE_IMPEXP static bool RmDir(const char *szDirName);


  /// \brief
  ///   Creates a _data directory for the file
  /// 
  /// Simply appends "_data" to the file name
  /// 
  /// \param szFile
  ///   Absolute file path
  /// 
  /// \param pRes
  ///   Optional string to get the resulting directory name
  /// 
  /// \return
  ///   true if successful or the directory exists
  VBASE_IMPEXP static bool MakeDataDir(const char *szFile, VString *pRes = NULL);

  /// \brief
  ///   Win32 only: finds the first directory above the passed path that contains a .project file
  /// 
  /// All other platforms return false.
  /// 
  /// This function can be used to add the project's directory as a data directory.
  /// 
  /// \param szPath
  ///   Absolute path that should be tested
  /// 
  /// \param szDest
  ///   Target string that receives the path in case there is a project file
  /// 
  /// \return
  ///   bool bResult: true in case there is a .project file in the same folder or above
  VBASE_IMPEXP static bool FindProjectDir(const char *szPath, char *szDest);

  /// \brief
  ///   Win32 only: finds the first directory above the passed path that contains a .project file
  /// 
  /// All other platforms return false.
  /// 
  /// This function can be used to add the project's directory as a data directory.
  /// 
  /// \param szDir
  ///   Absolute path that should be tested
  /// 
  /// \return
  ///   A VString with the project directory of the original directory if no project file present!
  ///  
  /// \note
  ///   Internally creates a temp buffer on the stack. Do not use it extensively for time critical processes.
  VBASE_IMPEXP static inline VString FindProjectDir(const char *szDir)
  {
    char szBuffer[FS_MAX_PATH];
    if(FindProjectDir(szDir, szBuffer))
      return VString(szBuffer);

    return VString(szDir);
  }

  /// \brief
  ///   This function tries to resolve a relative path to an absolute one via the given file stream manager.
  ///
  /// In case 'bUseAssetLookUpTable' is true, the given filename is redirected via the asset look up tables.
  /// Otherwise the path to the original file is returned but it is not guaranteed that this file actually exists.
  /// 
  /// \param szFilename
  ///   Path that should be made absolute.
  /// 
  /// \param out_szResolvedAbsPath
  ///   The absolute path is written to this buffer if the function returns true.
  /// 
  /// \param pFileManager
  ///   The file manager used for the lookup. If NULL, VBase_GetFileStreamManager will be taken instead.
  /// 
  /// \param bUseAssetLookUpTable
  ///   If true, the returned absolute path will follow redirections that are caused by the asset management system's look up tables.
  /// 
  /// \return
  ///   Returns false if the file could not be found.
  VBASE_IMPEXP static bool GetAbsolutePath(const char* szFilename, char* out_szResolvedAbsPath, IVFileStreamManager* pFileManager = NULL, bool bUseAssetLookUpTable = false)
  {
    if (pFileManager == NULL)
      pFileManager = VBase_GetFileStreamManager();

    //open the existing file through the file manager
    IVFileInStream *pIn = pFileManager->Open(szFilename);
    if (pIn)
    {
      // In case pIn->GetInitialDataDir() is NULL, we didn't use a lookup and thus pIn->GetFileName() equals the source file.
      if (bUseAssetLookUpTable || pIn->GetInitialDataDir() == NULL || pIn->GetInitialDataDir()[0] == '\0')
      {
        VFileHelper::ResolvePath(out_szResolvedAbsPath, pIn->GetFileName());
      }
      else
      {
        VPathHelper::CombineDirAndDir(out_szResolvedAbsPath, pIn->GetInitialDataDir(), szFilename);
      }
      
      pIn->Close();
      return true;
    }
    return false; //file doesn't exist.
  }

  /// \brief
  ///   This function tries to open the given file for writing.
  /// 
  /// \param szFilename
  ///   Path to the file.
  /// 
  /// \param pFileManager
  ///   The file manager used for the lookup. If NULL, VBase_GetFileStreamManager will be taken instead.
  /// 
  /// \return
  ///   Returns false if the file could not be found or the file is not writable.
  VBASE_IMPEXP static bool IsFileWritable(const char* szFilename, IVFileStreamManager* pFileManager = NULL)
  {
    if (pFileManager == NULL)
      pFileManager = VBase_GetFileStreamManager();

    IVFileOutStream *pStream = pFileManager->Create(szFilename, IVFileStreamManager::CREATE_APPEND);
    if (pStream)
    {
      pStream->Close();
      return true;
    }
    return false;
  }
  
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
