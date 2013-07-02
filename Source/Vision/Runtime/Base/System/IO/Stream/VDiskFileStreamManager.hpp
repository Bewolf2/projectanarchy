/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDiskFileStreamManager.hpp

#ifndef DC_VBASE_VDISKFILESTREAMMANAGER_HPP
#define DC_VBASE_VDISKFILESTREAMMANAGER_HPP

#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>
#include <Vision/Runtime/Base/System/IO/Asset/hkvAssetLookUpTable.hpp>


/// \brief
///   Stream that reads from a file on disk.
class VDiskFileInStream : public IVFileInStream
{
public:
  /// \brief VDiskFileInStream instances are created by VDiskFileStreamManager.
  VDiskFileInStream(IVFileStreamManager* pManager = NULL) : IVFileInStream(pManager)
  {
    m_sFilename[0] = 0;
    m_iPoolIndex = -1;
    m_iCacheEntry = -1;
    m_iReadCachePos = 0;
    m_iBytesInCache = 0;
    m_iFileSize = -1;
    m_iFilePos = 0;
  }

  /// \internal Called by VDiskFileInStream.
  bool Open(const char *pszFile, int iFlags = 0);

  /// \internal Sets the parent manager. Only used by VDiskFileInStream.
  void SetManager(IVFileStreamManager* pManager)
  {
    m_pParentManager = pManager;
  }

  /// \internal Called by VDiskFileInStream.
  inline void SetCacheEntry(int iEntry) { m_iCacheEntry = iEntry; }

  /// \internal Called by VDiskFileInStream.
  inline int GetCacheEntry() const { return m_iCacheEntry; }

  /// \internal Called by VDiskFileInStream.
  inline void InvalidateReadCache()
  {  
    m_iReadCachePos = 0;
    m_iBytesInCache = 0;
    m_iFilePos = 0;
  }


  // implement IVFileInStream

  VBASE_IMPEXP virtual size_t Read(void* pBuffer,int iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL SetPos(LONG iPos, int iMode) HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetPos() HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetSize() HKV_OVERRIDE;
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;

  int m_iPoolIndex;
  VFileHandle m_handle;
  VStaticString<FS_MAX_PATH> m_sFilename;

  int m_iCacheEntry;

  long m_iReadCachePos;
  long m_iBytesInCache;
  long m_iFilePos;
  long m_iFileSize;
};

/// \brief
///   Stream that writes directly to a file
class VDiskFileOutStream : public IVFileOutStream
{
public:
  /// \brief VDiskFileOutStream instances are created by VDiskFileStreamManager.
  VDiskFileOutStream(IVFileStreamManager* pManager = NULL) : IVFileOutStream(pManager)
  {
    m_iPoolIndex = -1;
    m_sFilename[0] = 0;
  }

  /// \internal Called by VDiskFileInStream.
  bool Create(const char *pszFile, int iFlags = 0);

  /// \internal Called by VDiskFileInStream.
  inline void SetManager(IVFileStreamManager *pManager) {m_pParentManager=pManager;}

  //Implement IVFileOutStream

  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;
  VBASE_IMPEXP virtual size_t Write(const void* pBuffer,size_t iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;
  VBASE_IMPEXP virtual void Flush() HKV_OVERRIDE;

  int m_iPoolIndex;
  VFileHandle m_handle;
  VStaticString<FS_MAX_PATH> m_sFilename;
};

/// \brief
///   Direct disk access stream manager (wrapper around fopen,...)
class VDiskFileStreamManager : public IVFileStreamManager
{
  V_DECLARE_DYNAMIC_DLLEXP(VDiskFileStreamManager, VBASE_IMPEXP);

public:

  enum VFilePools
  {
    DISKFILEMANAGER_INPOOL  = 4,
    DISKFILEMANAGER_OUTPOOL = 1
  };


  VBASE_IMPEXP VDiskFileStreamManager();
  VBASE_IMPEXP ~VDiskFileStreamManager();

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP VOVERRIDE const char* GetName();

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP VOVERRIDE IVFileOutStream* Create(const char* pszFileName, int iFlags = 0);

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP VOVERRIDE int GetNumDataDirectories();

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP VOVERRIDE const char* GetDataDirectory(int idx = 0);

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP VOVERRIDE void SetEnableDirectoryCaching(bool bStatus, int idx = 0);

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP VOVERRIDE bool IsDirectoryCachingEnabled(int idx = 0);

  VBASE_IMPEXP VOVERRIDE void StructureChanged() HKV_OVERRIDE;

  // internal functions
  VDiskFileInStream *CreateNewInStream(bool enableCaching = true);
  VDiskFileOutStream *CreateNewOutStream();
  void CloseFile(VDiskFileInStream *pInStream);
  void CloseFile(VDiskFileOutStream *pOutStream);

  /// \brief
  ///   Dumps the filenames of all currently open in and out streams to the log
  void DumpOpenStreams();

protected:
  virtual IVFileInStream* InternalOpen(const char* pszFileName, int iFlags = 0) HKV_OVERRIDE;
  virtual BOOL InternalExists(const char* pszFileName) HKV_OVERRIDE;
  virtual BOOL InternalGetTimeStamp(const char *pszFileName, VFileTime &destTime) HKV_OVERRIDE;
  virtual bool InternalSetDataDirectory(int idx, const char* pszPath) HKV_OVERRIDE;

  void BuildDirectory();
  bool TextExistsLookup(const char *szFilename, bool bAddIfMissing);


  VMutex m_IOMutex;
  int m_iInPoolUsedMask,m_iOutPoolUsedMask;
  VDiskFileInStream m_InPool[DISKFILEMANAGER_INPOOL];
  VDiskFileOutStream m_OutPool[DISKFILEMANAGER_OUTPOOL];

  VStaticString<FS_MAX_PATH> m_sPathPrefix;

  int m_iDirectoryFileCount;
  DynArray_cl<__int64> m_FilesInDirectory;
  bool m_bEnableDirectoryCaching;

  static bool g_bSupportsDirectoryCaching; 
};


/// \brief
///   Class for managing the read cache of the disk file stream managers.
class VDiskFileStreamCacheManager
{
public:
  /// \brief
  ///   Sets the read cache size to be used for each opened file.
  ///
  /// Note that the specified size is the size of the cache for each opened file, so if two files are opened and
  /// being read from using a VDiskFileInStream, 2*iSize bytes are reserved for the cache. The memory will remain
  /// allocated until FreeMemory() is called.
  ///
  /// Up to four files can be cached at the same time. All further simultaneously opened files will be read from
  /// directly without going through the read cache.
  ///
  /// It is allowed to change this value at any time. Note, however, that read caches already allocated to currently open
  /// files will not be resized.
  ///
  /// By default, read caching is disabled. 
  ///
  /// \param iSize
  ///   Size of the cache per file. Setting 0 will disable read caching. The read cache size must be a multiple of 32 bytes.
  VBASE_IMPEXP static void SetReadCacheSize(int iSize);

  /// \brief
  ///   Returns the read cache size to be used for each opened file.
  VBASE_IMPEXP static int GetReadCacheSize();

  /// \brief
  ///   Frees all unused memory currently allocated to file read caches. Call this when loading has finished. Note
  ///   that this function will not free memory allocated to files which are still open. Furthermore, this function
  ///   is implicitly called by the destructor of VDiskFileStreamManager.
  VBASE_IMPEXP static void FreeUnusedEntries();

  /// \brief
  ///   Internal function. Do not call.
  static int AllocateEntry();

  /// \brief
  ///   Internal function. Do not call.
  static void FreeEntry(int iEntry);

  /// \brief
  ///   Internal function. Do not call.
  static void EnsureReadCacheAllocated(int iEntry);

  /// \brief
  ///   Internal function. Do not call.
  static void *GetCacheEntry(int iEntry, int &iSize);

private:
  static int s_iReadCacheSizePerFile;
  static int s_iEntryInUse;

  static void *s_pReadCache[VDiskFileStreamManager::DISKFILEMANAGER_INPOOL];
  static int s_iReadCacheSize[VDiskFileStreamManager::DISKFILEMANAGER_INPOOL];

  static VMutex s_CacheMutex;
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
