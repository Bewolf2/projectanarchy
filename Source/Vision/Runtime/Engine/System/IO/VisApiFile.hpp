/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiFile.hpp

#ifndef DEFINE_VISAPIFILE
#define DEFINE_VISAPIFILE



/// \brief
///   VisFile_cl class for file operations. Can be accessed via Vision::File
/// 
/// All engine data reading and writing is handled by the installed file manager.
/// 
/// Custom file managers can be installed; e.g., to load encrypted data files.
/// 
/// \sa IVFileStreamManager
/// \sa IVFileInStream
/// \sa IVFileOutStream
class VisFile_cl
{
public:
  VisFile_cl();



  ///
  /// @name Data Directory Functions
  /// @{
  ///
 
  /// \brief
  ///   Adds a directory that will be used by the engine for file operations
  /// 
  /// This function uses the currently installed file manager (Vision::File.GetManager()) and sets
  /// the data directory on it.
  ///
  /// A special notation is required for paths that are located within an archive or a package (.zip, .v, 
  /// .vpk, .zip.sdat, .v.sdat, .vpk.sdat). In this case, the question mark character \c ? is used
  /// to separate the path to the package from the path within the package. 
  /// For example:
  /// \verbatim main.zip?baseData \endverbatim
  /// 
  /// \param pszDataPath
  ///   path of the data directory to add. This can be an absolute path, or a path relative to the
  ///   current directory. 
  /// 
  /// \param bEnableDirectoryCaching
  ///   whether to enable caching of directory entries.
  ///
  /// \return
  ///   \c true if successful; \c false if the directory could not be added (e.g., if there are no more 
  ///   free data directory entries in the installed file manager)
  /// 
  /// \note
  ///   This function does not check for duplicate data directories.
  VISION_APIFUNC bool AddDataDirectory(const char *pszDataPath, bool bEnableDirectoryCaching = false);
  
  /// \brief
  ///   Removes an engine data directory from the installed file manager.
  /// 
  /// No file operation should be pending on this data directory as this function might destroy a 
  /// sub-manager that owns an open file stream.
  /// 
  /// \param szAbsPath
  ///   absolute path to the data directory to remove
  /// 
  /// \return
  ///   \c true if the data directory could be found and removed; \c false otherwise
  VISION_APIFUNC bool RemoveDataDirectory(const char *szAbsPath);

  /// \brief
  ///   Removes all engine data directories
  ///
  /// No file operation should be pending on any data directory as this function might destroy a 
  /// sub-manager that owns an open file stream.
  /// 
  /// \return
  ///   \c true if all data directory could be removed; \c false otherwise
  VISION_APIFUNC bool RemoveAllDataDirectories();


  /// \brief
  ///   Gets the absolute path to a specific data directory.
  /// 
  /// \param i
  ///   the index of the directory in currently installed file manager (Vision::File.GetManager())
  ///
  /// \return
  ///   absolute path to the data directory. Can be \c NULL.
  /// 
  /// \sa Vision::SetDataDirectory
  VISION_APIFUNC static const char *GetDataDirectory(int i = 0);
  
  /// \brief
  ///   Sets a specific data directory that will be used by the engine for file operations.
  /// 
  /// This function uses the currently installed file manager (Vision::File.GetManager()) and sets
  /// the data directory on it. The previous value will be overwritten. Providing a value of \c NULL
  /// for the path removes the data directory with the specified index.
  ///
  /// A special notation is required for paths that are located within an archive or a package (.zip, .v, 
  /// .vpk, .zip.sdat, .v.sdat, .vpk.sdat). In this case, the question mark character \c ? is used
  /// to separate the path to the package from the path within the package. 
  /// For example:
  /// \verbatim main.zip?baseData \endverbatim
  /// 
  /// \param i
  ///   The index of the directory in currently installed file manager (Vision::File.GetManager())
  ///
  /// \param pszDataPath
  ///   path of the data directory to set. This can be an absolute path, or a path relative to the
  ///   current directory. 
  /// 
  /// \param bEnableDirectoryCaching
  ///   whether to enable caching of directory entries.
  ///
  /// \return
  ///   \c true if the data directory has been set; \c false if an error occurred (e.g., the given 
  ///   directory index was not valid)
  /// 
  /// \note
  ///   This function does not check for duplicate data directories.
  VISION_APIFUNC bool SetDataDirectory(int i, const char *pszDataPath, bool bEnableDirectoryCaching = false);


  /// \brief
  ///   Returns the maximum number of data directories supported by the installed file manager
  /// 
  /// This function wraps around IVFileStreamManager::GetNumDataDirectories().
  /// 
  /// \return
  ///   the maximum number of supported data directories
  VISION_APIFUNC static int GetMaxDataDirectoryCount();

  /// \brief
  ///   Helper function that takes a relative directory name and combines it with the current
  ///   directory (getcwd) to make an absolute path.
  /// 
  /// \param pszOutDir
  ///   buffer to store the result
  /// 
  /// \param pszRelDir
  ///   relative directory name
  /// 
  /// \note
  ///   If you call this after engine init then the current directory might  already have changed
  ///   and not be the application directory anymore!
  /// 
  /// \example
  ///   \code
  ///   char tmp[FS_MAX_PATH];
  ///   Vision::File.GetAbsoluteDirectory(tmp,"data");
  ///   Vision::File.AddDataDirectory(tmp);
  ///   \endcode
  VISION_APIFUNC void GetAbsoluteDirectory(char *pszOutDir, const char *pszRelDir);


  ///
  /// @}
  ///

  ///
  /// @name Manager Functions
  /// @{
  ///
 
  /// \brief
  ///   Returns the current file manager used by the engine
  /// 
  /// By default this is an instance of the VisionFileStreamManager class that supports mulitple
  /// data directories and loading from ZIP.
  VISION_APIFUNC IVFileStreamManager* GetManager();

  /// \brief
  ///   Sets the current file manager used by the engine
  /// 
  /// If you pass \c NULL, the engine recreates a VisionFileStreamManager instance.
  /// 
  /// All file access operations go through this one global file manager.
  /// 
  /// You can install your own file manager to load encrypted data files.
  /// 
  /// Since the IVFileStreamManager interface is reference counted, no custom cleanup is required
  /// 
  /// \param pManager
  ///   New file manager that is used for engine file operations. Can be \c NULL to use a default file
  ///   manager.
  /// 
  /// \example
  ///   \code
  ///   Vision::File.SetManager(new MyFileManager());
  ///   \endcode
  VISION_APIFUNC void SetManager(IVFileStreamManager *pManager);

  ///
  /// @}
  ///

  ///
  /// @name File Functions
  /// @{
  ///
  
  /// \brief
  ///   Open a file for input using the current engine file manager (IVFileStreamManager::Open)
  /// 
  /// This function uses the default engine file manager. All engine internal file read 
  /// accesses use this function.
  /// 
  /// \param pszFileName
  ///   Name of the file to open (relative to one of the data directories)
  /// 
  /// \param pszDirectory
  ///   Optional directory that the filename is relative to. This is prepended to the filename.
  /// 
  /// \param iFlags
  ///   Optional flags for opening the file. See IVFileStreamManager::OpenFlags.
  /// 
  /// \return
  ///   a file stream for reading the requested file, or \c NULL if the file could not be opened.
  /// 
  /// \example
  ///   \code
  ///   IVFileInStream* pIn = Vision::File.Open("myfile.txt", "data");
  ///   \endcode
  VISION_APIFUNC IVFileInStream* Open(const char* pszFileName, const char* pszDirectory = NULL, int iFlags = 0);

  /// \brief
  ///   Create a file for output using the current engine file manager.
  /// 
  /// This function uses the default engine file manager.
  /// 
  /// Before testing the data directories, this function tries to create a file via the manager
  /// installed with  SetOutputDirectory resp. SetOutputManager.
  /// 
  /// This function wraps around IVFileStreamManager::Create; if there is more than one data
  /// directories specified, IVFileStreamManager::Create loops through all and returns the stream
  /// for the first success.
  /// 
  /// Streams cannot be created inside archives or packages files.
  /// 
  /// \param pszFileName
  ///   Name of the file to create (relative to the data directories)
  /// 
  /// \param pszDirectory
  ///   Optional directory that the filename is relative to. This is prepended to the filename.
  ///
  /// \param iFlags
  ///   see IVFileStreamManager::CreateFlags
  /// 
  /// \return
  ///   a stream for writing to the created file, or \c NULL if the file could not be created.
  /// 
  /// \example
  ///   \code
  ///   IVFileOutStream* pOut = Vision::File.Create("myfile.txt", "data");
  ///   \endcode
  VISION_APIFUNC IVFileOutStream* Create(const char* pszFileName, const char* pszDirectory = NULL, int iFlags = 0);


  /// \brief
  ///   Returns whether a file exists.
  /// 
  /// This function uses the default engine file manager (IVFileStreamManager::Exists).
  /// 
  /// \param pszFileName
  ///   Name of the file (relative to one of the data directories)
  /// 
  /// \param pszDirectory
  ///   Optional directory that the filename is relative to. This is prepended to the filename.
  /// 
  /// \return
  ///   \c TRUE if the file exists; \c false if not.
  /// 
  /// \example
  ///   \code
  ///   if (Vision::File.Exists("myfile.txt", "data"))
  ///   \endcode
  VISION_APIFUNC BOOL Exists(const char* pszFileName, const char* pszDirectory = NULL);


  ///
  /// @}
  ///

  ///
  /// @name Output Related
  /// @{
  ///

  /// \brief
  ///   Sets a directory that will be used by the engine as a target directory for file creation
  ///   operations via Vision::File.Create
  /// 
  /// Setting this directory is optional. Alternatively the first data directory is used that can
  /// be used for writing files.
  /// 
  /// There can only be one explicitly set output directory.
  /// 
  /// This function creates a manager of type VDiskFileStreamManager that is returned by the
  /// GetOutputManager function
  /// 
  /// The path can be \c NULL to uninstall the dedicated output file manager.
  /// 
  /// \param szPath
  ///   Absolute or relative (to current directory) path to data directory. Can be \c NULL.
  /// 
  /// \return
  ///   \c true if the output directory was successfully set; \c false if not.
  VISION_APIFUNC bool SetOutputDirectory(const char *szPath);

  /// \brief
  ///   Returns a file manager that is used for file creation.
  /// 
  /// If no dedicated manager is installed, this function returns GetManager().
  /// 
  /// \return
  ///   the file manager that is used for output file creation.
  VISION_APIFUNC IVFileStreamManager* GetOutputManager();

  /// \brief
  ///   Sets a file manager to be used for output file creation.
  /// 
  /// \param pManager
  ///   a file manager that supports file creation. Can be \c NULL to uninstall the 
  ///   currently installed output manager.
  ///
  /// \sa SetOutputDirectory
  VISION_APIFUNC void SetOutputManager(IVFileStreamManager* pManager);

  ///
  /// @}
  ///

  ///
  /// @name Pre-Caching
  /// @{
  ///

  /// \brief
  ///   Creates a loading background task that loads a file into memory.
  /// 
  /// The next time Vision::File.Open is called with a cached file, the file system returns a
  /// memory stream.
  ///
  /// \param szFilename
  ///   File to precache.
  ///
  /// \param pManager
  ///   File manager to use. Can be \c NULL (default) to use the Vision's current file manager.
  ///
  /// \return
  ///   the loading task for loading the file into cache, or \c NULL on error.
  VISION_APIFUNC VLoadingTask* PrecacheFile(const char *szFilename, IVFileStreamManager* pManager=NULL);


  /// \brief
  ///   Creates a loading task that has the passed data already loaded into memory
  ///   for the specified file.
  ///
  /// \param szFilename
  ///   File to which the passed data should belong
  /// \param pData
  ///   Pointer to the file data
  /// \param iDataSize
  ///   Size of the file data
  ///
  /// \return
  ///   The loading task for the specified file. In case a loading task for this file 
  ///   already existed, the existing task is returned. Note that in this case, the task
  ///   may not have finished loading yet!
  VISION_APIFUNC VLoadingTask* PrecacheFileFromMemory(const char *szFilename,
    unsigned char *pData, int iDataSize);


  /// \brief
  ///   Returns a reference to the global background loading manager.
  VISION_APIFUNC VMemoryStreamManager& GetMemoryStreamManager();

  /// \brief
  ///   Sets whether to log warnings whenever an uncached file is loaded.
  ///
  /// This feature is useful for optimizing zone streaming. If enabled, a warning will be printed to 
  /// the log for every file that is opened but not cached through a memory stream.
  ///
  /// \param bStatus
  ///   \c true to print log warnings about uncached file accesses
  inline void SetWarnOnUncachedFiles(bool bStatus)
  {
    m_bWarnOnUncachedFiles = bStatus;
  }

  /// \brief
  ///   Returns whether warnings about uncached file accesses are logged.
  inline bool GetWarnOnUncachedFiles() const
  {
    return m_bWarnOnUncachedFiles;
  }

  ///
  /// @}
  ///


  /// \brief
  ///   Sets if absolute paths should be passed to the file manager or not
  ///
  /// \param bUseAbsolutePaths
  ///   true for using absolute paths, false otherwise
  inline void SetUseAbsolutePaths(bool bUseAbsolutePaths) 
  {
    m_bUseAbsolutePaths = bUseAbsolutePaths;
  }

  ///
  /// @name Asset Related
  /// @{
  ///

  /// \brief
  ///   Sets the asset profile to be used. This is usually done by the engine automatically.
  ///
  /// \param pszAssetProfile
  ///   the asset profile to be used, if NULL the default asset profile for the platform is used
  ///
  /// \param pszAssetLookUpTable
  ///   the name of the look up table to be used. If NULL, pszAssetProfile will be used instead. Setting a different value than pszAssetProfile is only done in vForge.
  VISION_APIFUNC void SetAssetProfile(const char* pszAssetProfile, const char* pszAssetLookUpTable = NULL);

  /// \brief
  ///   Gets the currently active asset profile
  VISION_APIFUNC const char* GetAssetProfile() const;

  /// \brief
  ///  Sets whether or not asset mappings should be logged.
  /// \param bLog
  ///   \c true if asset mappings should be logged; \c false if not
  /// \note
  ///   This functionality is currently not supported; setting this state has no effect.
  inline void SetLogAssetMappings(bool bLog)
  {
    m_bLogAssetMappings = bLog;
  }

  /// \brief
  ///  Returns whether asset mappings are being logged.
  /// \return
  ///   \c true if asset mappings should be logged; \c false if not
  /// \note
  ///   This functionality is currently not supported.
  inline bool GetLogAssetMappings() const
  {
    return m_bLogAssetMappings;
  }

  /// \brief
  ///   Reloads the asset lookup tables for all data directories currently registered
  ///   with Vision.
  /// \param pszDataDir
  ///   The data directory for which to reload the asset lookup table. If the data directory
  ///   is \c NULL, the asset lookup tables are reloaded for all data directories.
  VISION_APIFUNC void ReloadAssetLookupTables(const char* pszDataDir = NULL);

  ///
  /// @}
  ///


private:
  VSmartPtr<IVFileStreamManager> m_spManager; ///<smart pointer to active file manager
  VSmartPtr<IVFileStreamManager> m_spOutputManager; ///< alternative stream manager for output
  bool m_bWarnOnUncachedFiles;
  bool m_bUseAbsolutePaths;
  bool m_bLogAssetMappings;
};

/// \brief
///   The default file manager in Vision.
///
/// An instance of this class is created upon the first call of VisFile_cl::GetManager(), unless a custom
/// file manager has already been installed at this point.
///
/// This manager can hold multiple data directories, and each data directory is internally represented by 
/// a sub-manager of type VDiskFileStreamManager or VZipFileStreamManager respectively.
///
/// In most cases, it is not necessary to use the functionality of this class directly; all commonly needed
/// functionality is also accessible via the methods in Vision::File (which, in turn, maps to the currently
/// installed file manager).
class VisionFileStreamManager : public IVFileStreamManager
{
  V_DECLARE_DYNAMIC_DLLEXP(VisionFileStreamManager, VISION_APIFUNC);

public:
  /// \brief
  ///   Constructor.
  VISION_APIFUNC VisionFileStreamManager();

  /// \brief
  ///   Destructor.
  VISION_APIFUNC VOVERRIDE ~VisionFileStreamManager();

  ///
  /// @name IVFileStreamManager Implementation
  /// @{
  ///

  VISION_APIFUNC virtual const char* GetName() HKV_OVERRIDE;
  VISION_APIFUNC virtual IVFileOutStream* Create(const char* pszFileName, int iFlags = 0) HKV_OVERRIDE;

  /// \brief
  ///   Returns maximum number of data directories this file manager supports.
  ///
  /// Since this manager can accommodate any number of data directories, this function returns
  /// the current data directory count + 1.
  ///
  /// \return
  ///   the number of supported data directories (always one more that the number currently
  ///   set).
  VISION_APIFUNC virtual int GetNumDataDirectories() HKV_OVERRIDE;

  /// \brief
  ///   Returns the data directory at specified index. GetSubManager can be used to access 
  ///   the underlying manager instance.
  VISION_APIFUNC virtual const char* GetDataDirectory(int idx = 0) HKV_OVERRIDE;

  VISION_APIFUNC virtual void SetEnableDirectoryCaching(bool bStatus, int idx = 0) HKV_OVERRIDE;
  VISION_APIFUNC virtual bool IsDirectoryCachingEnabled(int idx = 0) HKV_OVERRIDE;
  VISION_APIFUNC virtual int GetNumSubManagerSlots() const HKV_OVERRIDE;
  VISION_APIFUNC virtual IVFileStreamManager* GetSubManager(int iSlot) const HKV_OVERRIDE;

  ///
  /// @}
  ///

  /// \brief
  ///   Helper function to create an appropriate sub manager
  ///
  /// \param pszPath
  ///   The path or filename for the file to create a manger for.
  ///
  /// \return
  ///   a new file manager instance, or \c NULL if the specified path was \c NULL.
  VISION_APIFUNC IVFileStreamManager* CreateSubManager(const char* pszPath);

  /// \brief
  ///   Since this manager class implements data directories as sub-managers, this function 
  //    allows for accessing each directory index as an actual manager.
  inline IVFileStreamManager *GetSubManager(int idx)
  {
    VASSERT(idx >= 0);
    if (idx >= m_spManager.Count()) //there are some loops that search for a empty manager space, we need this for them
      return NULL;
    return m_spManager.GetAt(idx);
  }

protected:
  /// \brief
  ///   Loops through all sub-managers and returns the first stream that was successfully opened through a data directory
  VISION_APIFUNC virtual IVFileInStream* InternalOpen(const char* pszFileName, int iFlags) HKV_OVERRIDE;

  /// \brief
  ///   Returns TRUE if the file exists in any sub-manager (i.e. data directory)
  VISION_APIFUNC virtual BOOL InternalExists(const char* pszFileName) HKV_OVERRIDE;

  /// \brief
  ///   Fills out the time stamp using the first sub-manager that supports time stamps
  VISION_APIFUNC virtual BOOL InternalGetTimeStamp(const char *pszFileName, VFileTime &destTime) HKV_OVERRIDE;

  /// \brief
  ///   Sets a data directory at specified index. This internally creates a VDiskFileStreamManager or VZipFileStreamManager depending on path extension
  VISION_APIFUNC virtual bool InternalSetDataDirectory(int idx, const char* pszPath) HKV_OVERRIDE;


  VISION_APIFUNC void SetManager(int idx, IVFileStreamManager* pManager);
  
  VRefCountedCollection<IVFileStreamManager> m_spManager;

  //needed for VBrowserFileManager
  bool m_bReverse;
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
