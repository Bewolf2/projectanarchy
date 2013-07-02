/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPackageFileStreamManager.hpp

#ifndef VBASE_VPACKAGEFILESTREAMMANAGER_HPP
#define VBASE_VPACKAGEFILESTREAMMANAGER_HPP

#include <Vision/Runtime/Base/System/IO/Asset/hkvAssetLookUpTable.hpp>

class VPackagedFileInStream;

/// \brief Class that encapsulates a complete package file.
class VPackage
{
public:
  /// \brief Constructor.
  VPackage () {}

  /// \brief File Version of a .vArc file
  enum VVersion_e
  {
    VPV_Version_1 = 1,
    VPV_Current = VPV_Version_1
  };

  /// \brief Represents one file in the archive
  struct VPackageFile
  {
    /// \brief The filename of the file in the archive (including path)
    const char* m_szFilename;
    /// \brief The uncompressed size of the file. Identical to m_uiFileSizeOnDisk, if the file is stored uncompressed.
    unsigned int m_uiFileSizeUncompressed;
    /// \brief The compressed size of the file as stored on disk. Identical to m_uiFileSizeUncompressed, if the file is stored uncompressed.
    unsigned int m_uiFileSizeOnDisk;
    /// \brief The position of the (compressed) file in the archive. Seek to this position to start reading from the file.
    unsigned int m_uiFileSeekPosition;
  };

public:
  /// \brief Opens the archive and reads all the important information for bookkeeping.
  bool ReadArchive (const char* szFile);

  /// \brief Checks at which index the file exists in the archive. Returns -1 if it does not exist at all.
  int FindFile (const char* szFile);

  /// \brief Returns the information about the file with the given index.
  const VPackageFile* GetPackagedFile (unsigned int uiFile) const { return &m_Files[uiFile]; }

private:
  /// \brief Deactivated copy constructor.
  VPackage (const VPackage& cc);
  /// \brief Deactivated assignment operator.
  void operator= (const VPackage& rhs);

  /// \brief Compares two VPackageFileIndex CRC values for equality. Used for sorting and searching.
  static int CRCCompare (const void* p1, const void* p2);

  /// \brief struct used for sorting and searching files by CRC values.
  struct VPackageFileIndex
  {
    unsigned int m_uiFileNameCRC1;
    unsigned int m_uiFileNameCRC2;
    unsigned int m_uiFileIndex;
  };

  /// \brief The VPackageFileIndex for every file in the archive. Sorted by CRCs for binary searches.
  VArray<VPackageFileIndex> m_FileIndices;
  /// \brief The information about every file in the archive.
  VArray<VPackageFile> m_Files;
  /// \brief One big array for all filename strings. The VPackageFile objects just point into this array.
  VArray<char> m_FileNameStrings;
};


/// \brief
///   Zip file access stream manager
class VPackageFileStreamManager : public IVFileStreamManager
{
  V_DECLARE_DYNAMIC_DLLEXP(VPackageFileStreamManager, VBASE_IMPEXP);

public:
  /// \brief
  ///   Constructor
  VBASE_IMPEXP VPackageFileStreamManager ();
  VBASE_IMPEXP ~VPackageFileStreamManager ();

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual const char* GetName (void) HKV_OVERRIDE { return "VPackageFileStreamManager"; }

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual IVFileOutStream* Create (const char* szFileName, int iFlags = 0) HKV_OVERRIDE { return NULL; }

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual int GetNumDataDirectories (void) HKV_OVERRIDE { return 1; }

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual const char* GetDataDirectory (int idx = 0) HKV_OVERRIDE { return m_sDataDirectory.AsChar (); }

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual void SetEnableDirectoryCaching (bool bStatus, int idx = 0) HKV_OVERRIDE { }

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual bool IsDirectoryCachingEnabled (int idx = 0) HKV_OVERRIDE { return true; }


protected:
  VBASE_IMPEXP virtual IVFileInStream* InternalOpen (const char* szFileName, int iFlags) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL InternalExists (const char* szFileName) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL InternalGetTimeStamp (const char* szFileName, VFileTime &destTime) HKV_OVERRIDE { return false; }
  VBASE_IMPEXP virtual bool InternalSetDataDirectory (int idx, const char* szPath) HKV_OVERRIDE;


private:
  void ResetDirectoryManager (void);

  friend class VPackagedFileInStream;
  void CloseFile (VPackagedFileInStream* pFile) { m_pArchiveFilePool.Append (pFile); }

  VStaticString<FS_MAX_PATH> m_LastExistsCheck;
  int m_iLastExistsCheckResult;

  VMutex m_IOMutex;
  VPackage m_Package;

  int m_iArchiveSubDirectoryStringLength;
  VStaticString<FS_MAX_PATH> m_sArchiveSubDirectory;
  
  VArray<VPackagedFileInStream*> m_pArchiveFilePool;

  VString m_sDataDirectory;
};


/// \brief
///   Internal class
class VPackagedFileInStream : public IVFileInStream
{
public:
  /// \brief
  /// Constructor
  VBASE_IMPEXP VPackagedFileInStream (IVFileStreamManager* pManager, const char* szArchiveFile);
  /// \brief
  /// Destructor, closes the file automatically
  VBASE_IMPEXP virtual ~VPackagedFileInStream ();

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual void Close (void) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual size_t Read (void* pBuffer, int iLen) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual BOOL SetPos (LONG iPos, int iMode) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual LONG GetPos (void) HKV_OVERRIDE { return m_uiVirtualFilePos; }

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual LONG GetSize (void) HKV_OVERRIDE { return m_pPackagedFile->m_uiFileSizeUncompressed; }

  /// \brief
  ///   Overridden function
  VBASE_IMPEXP virtual const char* GetFileName (void) HKV_OVERRIDE { return m_pPackagedFile->m_szFilename; }

private:
  friend class VPackageFileStreamManager;

  bool Open (const VPackage::VPackageFile* pPackagedFile);

  const VPackage::VPackageFile* m_pPackagedFile;

  unsigned int m_uiVirtualFilePos;
  unsigned int m_uiDiskFilePos;

  IVFileStreamManagerPtr m_spFileManager;
  IVFileInStream* m_pArchiveFile;

  enum { 
    m_uiDiskCacheSize = 1024 * 64,          // 64 KB disk cache
    m_uiExtractCacheSize = 1024 * 64 * 3,   // 192 KB extraction cache
    m_uiDiskCacheOffset = 0,
    m_uiExtractCacheOffset = m_uiDiskCacheSize,
    m_uiDataCacheSize = m_uiDiskCacheSize + m_uiExtractCacheSize   // 256 KB cache
  };

  unsigned int m_uiDiskCachePosition;
  unsigned int m_uiDiskBytesCached;
  unsigned int m_uiExtractCachePosition;
  unsigned int m_uiExtractedBytes;

  unsigned char* m_ExtractCache;
  unsigned char* m_DiskCache;
  unsigned char  m_DataCache[m_uiDataCacheSize];

  int m_iStreamState;
  z_stream m_zLibStream;
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
