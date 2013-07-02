/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VZipFileStreamManager.hpp

#ifndef DC_VBASE_VZIPFILESTREAMMANAGER_HPP
#define DC_VBASE_VZIPFILESTREAMMANAGER_HPP

#include <Vision/Runtime/Base/System/IO/Asset/hkvAssetLookUpTable.hpp>


/// \brief
///   Stores information about a zip file, i.e. one entry in the zip directory
///
/// In order to create an instance of VZipFileInfo, use VZipFileStreamManager::GetZipFileEntry.
class VZipFileInfo
{
public:
  VZipFileInfo();

  const char *m_szFilename; // points to a name in a name block

  ULONG m_iFilePos; ///< file start
  ULONG m_iFileNum; ///< file number in zip
  ULONG m_iSize;
  ULONG m_iCRCFilename; ///< CRC value of the filename for faster lookup
};



/// \brief
///   Implements a file stream to read from zip archives.
class VZipFileInStream : public IVFileInStream
{
public:
  VZipFileInStream();
  virtual ~VZipFileInStream();

  /// \brief 
  ///   Deletes all internal data.
  void Cleanup();

  //implement IVFileInStream
  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;
  VBASE_IMPEXP virtual size_t Read(void* pBuffer,int iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL SetPos(LONG iPos, int iMode) HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetPos() HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetSize() HKV_OVERRIDE;
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;

  /// \brief 
  ///   Sets the parent file stream manager.
  inline void SetManager(IVFileStreamManager *pManager) {m_pParentManager = pManager;}

  /// \brief 
  ///   Opens the given file in the zip archive.
  bool Open(const char *pszFileName, VZipFileInfo& info);

  void *m_pZipFile;
  ULONG m_iSize;
  int m_iPoolIndex;

  VStaticString<FS_MAX_PATH> m_sFilename;
};

#define ZIPFILEMANAGER_INPOOL    4


/// \brief
///   Zip file access stream manager
class VZipFileStreamManager : public IVFileStreamManager
{
  V_DECLARE_DYNAMIC_DLLEXP(VZipFileStreamManager, VBASE_IMPEXP);

public:

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VZipFileStreamManager();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP ~VZipFileStreamManager();

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual const char* GetName() HKV_OVERRIDE;

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual IVFileOutStream* Create(const char* pszFileName, int iFlags = 0) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual int GetNumDataDirectories() HKV_OVERRIDE;

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual const char* GetDataDirectory(int idx = 0) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual void SetEnableDirectoryCaching(bool bStatus, int idx = 0) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual bool IsDirectoryCachingEnabled(int idx = 0) HKV_OVERRIDE;

  ///
  /// @name Zip Specific File Access
  /// @{
  ///

  /// \brief
  ///   Returns the number of file entries in this Zip file. All entries are in a flat list
  inline int GetNumZipFileEntries() const
  {
    return (int)m_iFileCount;
  }

  /// \brief
  ///   Returns an information structure for file entry with specified index. The index must be in
  ///   range [0..GetNumZipFileEntries()-1]
  inline const VZipFileInfo& GetZipFileEntry(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<GetNumZipFileEntries());
    return m_pFileInfo[iIndex];
  }

  ///
  /// @}
  ///

protected:
  VBASE_IMPEXP virtual IVFileInStream* InternalOpen(const char* pszFileName, int iFlags = 0) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL InternalExists(const char* pszFileName) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL InternalGetTimeStamp(const char *pszFileName, VFileTime &destTime) HKV_OVERRIDE;
  VBASE_IMPEXP virtual bool InternalSetDataDirectory(int idx, const char* pszPath) HKV_OVERRIDE;

private:
  friend class VZipFileInStream;
  bool BuildFileList(void *pZipFile);
  int FindFile(const char* pszFilename);
  
  void Cleanup(); ///<resets internal state
  void DumpOpenStreams();

  VZipFileInStream *CreateInStream();
  void CloseFile(VZipFileInStream *pStream);

  ULONG m_iFileCount;
  VZipFileInfo *m_pFileInfo;
  char *m_pNameBuffer;
  VStaticString<FS_MAX_PATH> m_sZipFileName;
  VMemoryTempBuffer<FS_MAX_PATH> m_sZipSubfolderName;
  char* m_pszCompletePath;

  VMutex m_IOMutex;
  int m_iInPoolUsedMask;
  VZipFileInStream m_InPool[VDiskFileStreamManager::DISKFILEMANAGER_INPOOL];
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
