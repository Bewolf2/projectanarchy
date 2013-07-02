/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVFileStream.hpp

#ifndef DC_IVFILESTREAM_HPP
#define DC_IVFILESTREAM_HPP

#include <Vision/Runtime/Base/System/IO/System/VFileTime.hpp>
#include <Vision/Runtime/Base/System/VRefCounter.hpp>
#include <Vision/Runtime/Base/System/IO/Asset/hkvAssetLookUpTable.hpp>
#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>

#include <Vision/Runtime/Base/Math/hkvMath.h> //for hkvResult
#include <Vision/Runtime/Base/System/VNameValueListParser.hpp>


class IVFileInStream;
class IVFileOutStream;
class IVFileStreamManager;
struct VFileHandle;

/// \brief
///   Parser that can be used to parse asset metadata strings
typedef VNameValueListParser<',', '=', 1024> VAssetMetadataParser;


/// \brief
///   struct that contains the information attached to an asset
struct VAssetInfo
{
  const char* m_pszRelativePath; ///< relative path to the asset inside the library
  const char* m_pszMetaInfo;     ///< meta information for the asset
  unsigned int m_uiHash;         ///< hash value of path and meta information
};


/// \brief 
///   Internal helper class for resolving assets
struct VAssetResolveContext
{
  /// \brief constructor
  VAssetResolveContext(IVFileStreamManager* pRootManager = NULL)
  : m_pRootManager(pRootManager), m_iVariantKeyIndex(0), m_iRecursionDepth(0), m_szInitialDataDir(NULL)
  {
  }

  IVFileStreamManager* m_pRootManager;
  int m_iVariantKeyIndex;
  int m_iRecursionDepth;
  const char* m_szInitialDataDir;
};


/// \brief An IVFileStreamManager creates the proper IVFileStream objects to access a certain type of data stream.
/// 
/// This is the base class for all file stream managers. There are different types of managers that enable reading from or writing to different data sources.
/// Some file stream managers support one or more "data directories" (e.g. a 'folder' in a file system).
/// 
/// For example VDiskFileStreamManager enables access to a local file system and can mount several different folders as 'data directories'.
class IVFileStreamManager : public VRefCounter, public VTypedObject
{
  V_DECLARE_DYNAMIC_DLLEXP(IVFileStreamManager, VBASE_IMPEXP);

public:
  
  /// \brief Flags for the 'Create' function
  enum CreateFlags
  {
    CREATE_APPEND         = V_BIT(0),                            ///< Opens a file for writing, but keeps existing data and instead appends new data at its end
    SHARE_DENY_WRITE      = V_BIT(1),                            ///< unsupported
    SHARE_DENY_READ       = V_BIT(2),                            ///< unsupported
    SHARE_DENY_READWRITE  = SHARE_DENY_WRITE | SHARE_DENY_READ   ///< unsupported
  };
  
  /// \brief Flags for the 'Open' function
  enum OpenFlags
  {
    OPEN_NO_CACHE     = V_BIT(0),  ///< Disables the read cache for files. The read cache is only used, if its size was specified via VDiskFileStreamCacheManager::SetReadCacheSize, though.
    OPEN_NO_RECURSE   = V_BIT(1),  ///< [internal] Don't descend recursively to sub-FileStreamManagers.
    OPEN_ALLOW_WRITE  = V_BIT(2)   ///< Allow the file to be opened for writing by others. Only supported on WIN32.
  };  

  /// \brief 
  ///   Constructor
  VBASE_IMPEXP IVFileStreamManager();

  /// \brief 
  ///   Destructor
  VBASE_IMPEXP virtual ~IVFileStreamManager();

  /// \brief
  ///   Initializes the asset library
  ///
  /// \return HKV_SUCCESS on success and HKV_FAILURE on failure
  VBASE_IMPEXP hkvResult InitializeAssetLibrary();

  /// \brief
  ///   Looks up an asset in the lookup table associated with the file stream manager
  ///
  /// \param pszAsset
  ///   The asset to look up
  ///
  /// \return NULL if not found, pointer to the data struct otherwise
  VBASE_IMPEXP const VAssetInfo* LookUpAsset(const char* pszAsset);

  /// \brief
  ///   Looks up an asset in the lookup table associated with the file stream manager
  ///
  /// \param pszAsset
  ///   The asset to look up
  ///
  /// \param resolveContext
  ///   The resolve context used to track the current look-up
  ///
  /// \return NULL if not found, pointer to the data struct otherwise
  VBASE_IMPEXP const VAssetInfo* LookUpAsset(const char* pszAsset, VAssetResolveContext& resolveContext);

  /// \brief
  ///   Get the manager's name.
  VBASE_IMPEXP virtual const char* GetName() = 0;

  /// \brief
  ///   Opens an existing file for reading.
  /// 
  /// A pointer to an instance of IVFileInStream will be returned.
  /// 
  /// If the operation fails, e.g. because the file does not exist, this function returns NULL.
  /// 
  /// \param szFileName
  ///   String of the full path name of the file to open. If it is a relative path, the file will be searched in all data directories.
  /// 
  /// \param iFlags
  ///   see IVFileStreamManager::OpenFlags
  ///
  /// \return
  ///   IVFileInStream* pInFile : File object instance of the opened file (or NULL).
  VBASE_IMPEXP IVFileInStream* Open(const char* szFileName, int iFlags = 0);

  /// \brief
  ///   Opens or creates a file for writing.
  /// 
  /// If the file exists, it will be overwritten.
  /// 
  /// If the operation fails, e.g. because the path does not exist, this function returns NULL.
  /// 
  /// \param szFileName
  ///   String of the full path name of the file to create. If it is a relative path, the file 
  ///   will be created in the first writable data directory.
  ///
  /// \param iFlags
  ///   see IVFileStreamManager::CreateFlags
  /// 
  /// \return
  ///   IVFileOutStream* pOutFile : File object instance of the new file (or NULL).
  VBASE_IMPEXP virtual IVFileOutStream* Create(const char* szFileName, int iFlags = 0) = 0;

  /// \brief
  ///   Checks whether a file of specified filename exists.
  VBASE_IMPEXP BOOL Exists(const char* szFileName);


  /// \brief
  ///   Fills out the VFileTime structure with the filetime information. Returns FALSE, if the time-stamp could not be retrieved (e.g. because the file does not exist).
  VBASE_IMPEXP BOOL GetTimeStamp(const char* szFileName, VFileTime& out_destTime);

  /// \brief
  ///   Returns the maximum number of data directories supported by this manager (in most cases 1).
  VBASE_IMPEXP virtual int GetNumDataDirectories() = 0;

  /// \brief
  ///   Sets a data directory. The index must be in valid range.
  VBASE_IMPEXP bool SetDataDirectory(int idx, const char* szPath);

  /// \brief
  ///   Returns the path of a data directory. Returns NULL if the index is not valid or the data directory is not set.
  VBASE_IMPEXP virtual const char* GetDataDirectory(int idx = 0) = 0;

  /// \brief
  ///   Enables or disables caching of entries in a data directory (where applicable in derived classes). This can speed up file lookups (especially 'Exists' calls).
  VBASE_IMPEXP virtual void SetEnableDirectoryCaching(bool bStatus, int idx = 0) {}

  /// \brief
  ///   Checks whether or not directory caching is enabled. If a file stream manager does not implement directory caching, this function will always return false.
  VBASE_IMPEXP virtual bool IsDirectoryCachingEnabled(int idx = 0) { return false; }

  /// \brief
  ///   Returns the number of sub-manager slots this file stream manager has.
  VBASE_IMPEXP virtual int GetNumSubManagerSlots() const { return 0; }

  /// \brief
  ///   Returns the sub-manager at the specified slot. Caution: Even with a valid slot, the returned manager may be NULL!
  VBASE_IMPEXP virtual IVFileStreamManager* GetSubManager(int iSlot) const { return NULL; }

  /// \brief
  ///   Notifies this file stream manager that the structure (data directories, 
  ///   asset lookup tables) changed in some way. In the base implementation, this clears
  ///   any cached look-up results.
  VBASE_IMPEXP virtual void StructureChanged();

protected:
  /// \brief
  ///   Opens an existing file for reading.
  /// 
  /// If this manager implementation is potentially used for streaming, this method 
  /// must be implemented thread-safe. See class VMutex.
  /// 
  /// \param szFileName
  ///   string of the full path name (might be relative) of the file to open.
  /// 
  /// \param iFlags
  ///   see IVFileStreamManager::OpenFlags
  ///
  /// \return
  ///   Pointer to a new instance of a IVFileInStream for the opened file, or NULL, if the file
  ///   could not be opened.
  virtual IVFileInStream* InternalOpen(const char* szFileName, int iFlags) = 0;
  
  IVFileInStream* CallInternalOpen(IVFileStreamManager& other, const char* szFileName, int iFlags)
  {
    return other.InternalOpen(szFileName, iFlags);
  }

  /// \brief
  ///   Checks whether a file with the specified filename exists.
  virtual BOOL InternalExists(const char* szFileName) = 0;

  BOOL CallInternalExists(IVFileStreamManager& other, const char* szFileName)
  {
    return other.InternalExists(szFileName);
  }

  /// \brief
  ///   Fills out the VFileTime structure with the filetime information. Returns FALSE if the information could not be retrieved.
  virtual BOOL InternalGetTimeStamp(const char* szFileName, VFileTime& out_destTime) = 0;

  /// \brief
  ///   Helper function to allow file stream managers to call 'InternalGetTimeStamp' of other file stream managers, although the function is not public.
  BOOL CallInternalGetTimeStamp(IVFileStreamManager& other, const char *szFileName, VFileTime &destTime)
  {
    return other.InternalGetTimeStamp(szFileName, destTime);
  }

  /// \brief
  ///   Sets a data directory. The index must be in valid range.
  virtual bool InternalSetDataDirectory(int idx, const char* szPath)  = 0;

  /// \brief
  ///   Locks the structure of this file manager against concurrent modifications.
  void LockStructure();

  /// \brief
  ///   Unlocks the structure modification protection again.
  void UnlockStructure();

private:

  /// The lookup table used to resolve assets to files that can be opened by the file managers.
  hkvAssetLookUpTable m_AssetLookUpTable;

  /// A mutex to protect the structure of this file stream manager
  VMutex m_structureProtect;

  /// The asset queried for in the last successful look-up
  VString m_sLastAsset;

  /// The result of the last successful asset look-up
  const VAssetInfo* m_pLastAssetInfo;

  /// The initial data directory of the last successful asset look-up
  const char* m_szLastAssetDataDir;
};

/// \brief Typedef for smart pointer to IVFileStreamManager.
typedef VSmartPtr<IVFileStreamManager> IVFileStreamManagerPtr;



/// \brief
///   This is the base class for all 'in streams', ie. streams that are used for reading from a data source (e.g. a file or a memory stream).
class IVFileInStream
{
public:

  /// \brief
  ///   The object must be constructed by a IVFileStreamManager, which will pass itself as the parent manager to the instance.
  IVFileInStream(IVFileStreamManager* pManager)
  {
    m_pParentManager = pManager;
    m_bEOF = true; // no data till successfully opened
    m_uiLookupHash = 0;
  }

  /// \brief
  ///   Destructor
  virtual ~IVFileInStream() {}

  /// \brief
  ///   Returns the manager that created this stream.
  inline IVFileStreamManager* GetManager() const {return m_pParentManager;}

  /// \brief
  ///   This function closes the stream and deallocates its data. The object can not be used anymore after this call.
  virtual void Close() = 0;

  /// \brief
  ///   Reads up to iLen number of bytes to pBuffer. Returns the exact number of bytes that was read (may be smaller than iLen).
  /// 
  /// \param pBuffer
  ///   The destination buffer.
  /// 
  /// \param iLen
  ///   Number of bytes to read.
  /// 
  /// \return
  ///   size_t iRead : Number of actual bytes read.
  virtual size_t Read(void* pBuffer,int iLen) = 0;

  /// \brief
  ///   Read function that performs endianess conversion on the read buffer.
  /// 
  /// This version of the read function calls the virtual Read function and performs the conversion
  /// on the read block.
  /// 
  /// The expected format is passed in the format string.
  /// 
  /// Each character in the format string represents a data type: i=int, s=short, c=char, f=float, q=quad word (8bytes)
  /// For full documentation of the format string see the EndianSwitchWorker() documentation.
  /// 
  /// \param pBuffer
  ///   The destination buffer.
  /// 
  /// \param iLen
  ///   Length of the buffer in bytes, must be large enough to hold the data that the format string specifies (ie. if "f" is used to read a float, the buffer must be at least 4 bytes large).
  /// 
  /// \param pFormat
  ///   Format specification, see EndianSwitchWorker() for more details.
  /// 
  /// \param iRepetitions
  ///   Number of repetitions of the format string.
  /// 
  /// \return
  ///   size_t iRead : Number of actual bytes read.
  /// 
  /// \example
  ///   Reads two integers into a structure:
  ///   \code
  ///   pFile->Read(&header, sizeof(header),"ii", 1);
  ///   \endcode
  VBASE_IMPEXP size_t Read(void* pBuffer, int iLen, const char *pFormat, unsigned int iRepetitions = 1);


  /// \brief
  ///   Sets the byte position where to read/write the file. This function will be removed in the future.
  /// 
  /// \param iPos
  ///   Position relative to base position (iMode)
  /// 
  /// \param iMode
  ///   Base position. Must be VFS_SETPOS_SET, VFS_SETPOS_CURRENT or VFS_SETPOS_END (from the VFilePositionMode enum).
  virtual BOOL SetPos(LONG iPos, int iMode) = 0;

  /// \brief
  ///   Returns the current file posistion. This function will be removed in the future.
  virtual LONG GetPos() = 0;

  /// \brief
  ///   Returns the size of the file in bytes.
  virtual LONG GetSize() = 0;

  /// \brief
  ///   Returns the name of the file.
  virtual const char* GetFileName() = 0;

  /// \brief
  ///   Indicates whether the end of file has been reached.
  bool IsEOF()
  {
    return m_bEOF;
  }

  /// \brief
  ///  Returns the modification date of the file the stream points to.
  VBASE_IMPEXP virtual bool GetTimeStamp(VFileTime& result);

  /// \brief
  ///   Sets the data directory in which the asset for this stream was first successfully looked up.
  /// \param szDataDir
  ///   The data directory to set.
  VBASE_IMPEXP void SetInitialDataDir(const char* szDataDir);

  /// \brief
  ///   Gets the data directory in which the asset for this stream was first
  ///   successfully looked up. If the stream was not opened as the result of 
  ///   an asset look-up, this function returns \c NULL.
  /// \return
  ///   The initial data directory, or \c NULL if this stream was not opened as a result of an asset look-up.
  const char* GetInitialDataDir() const
  {
    return m_sInitialDataDir;
  }

  /// \brief
  ///   Sets the metadata string to go with this stream.
  /// \param szMetadata
  ///   The metadata string. Can be NULL.
  VBASE_IMPEXP void SetMetadata(const char* szMetadata);

  /// \brief
  ///   Returns the metadata string set for this stream.
  /// \return
  ///   The metadata set for this stream. May be NULL.
  const char* GetMetadata() const
  {
    return m_sMetaData;
  }

  /// \brief
  ///   Sets the hash of the asset lookup entry that identified the underlying file in the relevant asset lookup table.
  /// \param uiHash
  ///   the hash value to set
  void SetLookupHash(unsigned int uiHash)
  {
    m_uiLookupHash = uiHash;
  }

  /// \brief
  ///   Returns the hash of the asset lookup entry that identified the underlying file in the relevant asset lookup table.
  /// \return
  ///   the hash value (0 if not explicitly set)
  unsigned int GetLookupHash() const
  {
    return m_uiLookupHash;
  }

protected:
  bool m_bEOF;
  IVFileStreamManager *m_pParentManager;
  VString m_sMetaData;
  VString m_sInitialDataDir;
  unsigned int m_uiLookupHash;
};



/// \brief
///   This is the base class for all 'out streams', ie. streams that are used for writing to a data stream (e.g. a file or a memory stream).
class IVFileOutStream
{
public:

  /// \brief
  ///   The object must be constructed by a IVFileStreamManager, which will pass itself as the parent manager to the instance.
  IVFileOutStream(IVFileStreamManager *pManager)
  {
    m_pParentManager = pManager;
  }

  /// \brief
  ///   Destructor
  virtual ~IVFileOutStream() {}


  /// \brief
  ///   Get the manager that created this stream.
  inline IVFileStreamManager* GetManager() const {return m_pParentManager;}

  /// \brief
  ///   This function closes the stream and deallocates its data. The object can not be used anymore after this call.
  virtual void Close() = 0;

  /// \brief
  ///   Writes up to iLen number of bytes from pBuffer to the stream. Returns the exact number of bytes that were written.
  /// 
  /// \param pBuffer
  ///   The source buffer.
  /// 
  /// \param iLen
  ///   Number of bytes to write.
  /// 
  /// \return
  ///   size_t : Number of actual bytes written.
  virtual size_t Write(const void* pBuffer, size_t iLen) = 0;

  /// \brief
  ///   Write function that performs endianess conversion before writing the data to the stream
  /// 
  /// This version of the write function performs the conversion on a copy of the write block and
  /// then calls the virtual Write function.
  /// 
  /// The expected format is passed in the format string.
  /// 
  /// Each character in the format string represents a data type: i=int, s=short, c=char, f=float, q=quad word (8bytes)
  /// For full documentation of the format string see the EndianSwitchWorker() documentation.
  /// 
  /// \param pBuffer
  ///   Buffer to write (not modified by endian conversion)
  /// 
  /// \param iLen
  ///   Length of the buffer in bytes, must be large enough to hold the data that the format string specifies (ie. if "f" is used to write a float, the buffer must be at least 4 bytes large).
  /// 
  /// \param pFormat
  ///   Format specification, see EndianSwitchWorker() for more details.
  /// 
  /// \param iRepetitions
  ///   Number of repetitions of the format string.
  /// 
  /// \return
  ///   size_t : Number of actual bytes written.
  /// 
  /// \example
  ///   Writes two integers from a structure to the stream:
  ///   \code
  ///   pFile->Write(&header, sizeof(header),"ii", 1);
  ///   \endcode
  VBASE_IMPEXP size_t Write(const void* pBuffer, size_t iLen, const char *pFormat, unsigned int iRepetitions = 1);

  /// \brief
  ///   Makes sure all data is written to disk (not necessarily supported by all file-systems).
  virtual void Flush() = 0;

  /// \brief
  ///   Returns the name of the file.
  virtual const char* GetFileName() = 0;


  /// \brief
  ///   Helper function to write a string to the stream.
  /// 
  /// This function just writes the string content, not the length and also not the 0-termination.
  /// Thus this function is useful for pure text output rather than binary output.
  ///
  /// \param szString
  ///   The string which will be written to the file.
  /// 
  /// \return
  ///   size_t : Number of actual bytes written.
  size_t WriteText(const char* szString)
  {
    if (!szString)
      return 0;

    return Write(szString, strlen(szString));
  }

protected:
  /// \brief The parent file stream manager.
  IVFileStreamManager* m_pParentManager;
};


/// \brief
///   Class containing helper functions for streams.
class VStreamHelper
{
private:

  /// \brief
  ///   Constructor
  VStreamHelper();

public:

  ///
  /// @name Write and Read String Helpers
  /// @{
  ///


  /// \brief
  ///   Writes a string to file in binary format.
  /// 
  /// That is, writing the length as an int followed by the string characters (without terminating 0).
  /// 
  /// A length of -1 is written if szString==NULL.
  ///
  /// \param pOutStream
  ///   The stream to write to.
  /// 
  /// \param szString
  ///   The string to write.
  /// 
  /// \return
  ///   bool : true, if successful
  VBASE_IMPEXP static bool WriteStringBinary(IVFileOutStream *pOutStream, const char *szString);

  /// \brief
  ///   Reads a string from a binary file
  /// 
  /// The assumed format is the same as in WriteStringBinary.
  /// The returned string must be free'd with VBaseDealloc().
  ///
  /// \param pInStream
  ///   The stream to read from.
  /// 
  /// \return
  ///   char* szString : pointer to a new allocated string (or NULL if length was -1)
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines an empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  VBASE_IMPEXP static char* ReadStringBinary(IVFileInStream *pInStream);

  /// \brief
  ///   Reads a string from a binary file
  /// 
  /// The assumed format is the same as in WriteStringBinary.
  /// 
  /// The string is written to the destination buffer. A maximum size can be specified.
  /// 
  /// \param pInStream
  ///   The stream to read from.
  /// 
  /// \param szDestBuffer
  ///   The destination buffer to write the string into.
  /// 
  /// \param iMaxCount
  ///   The size of the destination buffer (i.e., the maximum number of bytes to read).
  /// 
  /// \return
  ///   int iLength : the length of the string (-1 if a NULL string was saved)
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines and empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  VBASE_IMPEXP static int ReadStringBinary(IVFileInStream *pInStream, char *szDestBuffer, int iMaxCount=-1);
  
  /// \brief
  ///   This version of ReadStringBinary reads all bytes and tries to use the passed buffer.
  /// 
  /// \param pInStream
  ///   The stream to read from.
  /// 
  /// \param szDestBuffer
  ///   The destination buffer to write the string into.
  /// 
  /// \param iMaxCount
  ///   The size of the destination buffer (i.e., the maximum number of bytes to read).
  ///
  /// \param bAllocated
  ///   If the bAllocated flag is set after the function returns, the return value has to be free'd with VBaseDeAlloc.
  ///
  /// \returns
  /// char *szRetString: If the string uses not more than iMaxCount bytes (i.e. it fits into szDestBuffer), then the
  /// return value matches the passed buffer.
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines and empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  VBASE_IMPEXP static char *ReadStringBinary(IVFileInStream *pInStream, char *szDestBuffer, int iMaxCount, bool &bAllocated);

  /// \brief
  ///   WriteLine writes szText into pOutStream and appends a '\n' character.
  /// 
  /// \param pOutStream
  ///   The stream to write to.
  /// 
  /// \param szText
  ///   The text to write to the stream.
  /// 
  /// \returns
  ///   Returns true if the operation was successful.
  VBASE_IMPEXP static bool WriteLine(IVFileOutStream *pOutStream, const char* szText);

  /// \brief
  ///   ReadLine reads characters from pInStream until the first occurrence of a '\\n' character, skipping '\\r' characters.
  ///
  /// \param pInStream
  ///   The stream to read from.
  ///
  /// \param szDestBuffer
  ///   The buffer the function can write to.
  ///
  /// \param iMaxCount
  ///   The size of szDestBuffer. Must be greater than 1 in order to be able to read any characters.
  ///
  /// \returns
  ///   Returns the number of characters written to szDestBuffer, including null termination.
  VBASE_IMPEXP static int ReadLine(IVFileInStream* pInStream, char* szDestBuffer, int iMaxCount);

  ///
  /// @}
  ///

};

/// \brief Helper class that takes ownership of a file stream and automatically calls Close() on it as soon as it goes out of scope
template<class T>
class VScopedFileStream
{
public:
  /// \brief constructor
  ///
  /// \param pStream
  ///   The stream to take ownership of
  explicit VScopedFileStream(T* pStream) : 
    m_pStream(pStream)
  { }

  /// \brief default constructor
  VScopedFileStream() :
    m_pStream(NULL)
  { }

  /// \brief destructor, ensures that the given stream gets closed.
  ~VScopedFileStream()
  {
    if(m_pStream != NULL)
      m_pStream->Close();
  }

  /// \brief dereferencing operator
  T* operator -> () const
  {
    return m_pStream;
  }

  /// \brief cast to T*
  operator T*() const
  {
    return m_pStream;
  }

  /// \brief
  ///  Releases the ownership of the stream and returns it
  T* Release()
  {
    T* temp = m_pStream;
    m_pStream = NULL;
    return temp;
  }

private:

  //non-copyable
  void operator = (const VScopedFileStream<T>& rh);
  VScopedFileStream(const VScopedFileStream<T>& other);

  T* m_pStream;
};


#include <Vision/Runtime/Base/System/IO/System/VFileHelper.hpp>

//Resets the file stream manager and asset management data
VBASE_IMPEXP void VBase_DeInitFileManagement();

//Retrieves the file stream manager used by vBase
VBASE_IMPEXP IVFileStreamManager* VBase_GetFileStreamManager();

//Sets the file stream manager used by vBase
VBASE_IMPEXP void VBase_SetFileStreamManager(IVFileStreamManager* pManager);


namespace AssetSettings
{
  /// \brief
  ///   Returns whether the Vision Engine should try to load default replacements for some file types.
  ///
  /// If this functionality is enabled, the Vision Engine tries to load platform-specific alternative 
  /// versions of files whenever a file is being opened. If an alternative version exists, that version
  /// is loaded; otherwise, the original file is loaded.
  ///
  /// \note
  ///   While this functionality is currently enabled (as of Version 2013.1), it has been superseded with
  ///   the functionality offered by the asset management system. If you are working with new scenes, or have
  ///   converted your scenes to use asset management, you should disable this feature.
  ///
  /// \return
  ///   \c true if the functionality is enabled; \c false if not
  VBASE_IMPEXP bool GetUseAlternativeFiles();

  /// \brief
  ///   Sets whether the Vision Engine should try to load default replacements for some file types.
  ///
  /// If this functionality is enabled, the Vision Engine tries to load platform-specific alternative 
  /// versions of files whenever a file is being opened. If an alternative version exists, that version
  /// is loaded; otherwise, the original file is loaded.
  ///
  /// \note
  ///   While this functionality is currently enabled (as of Version 2013.1), it has been superseded with
  ///   the functionality offered by the asset management system. If you are working with new scenes, or have
  ///   converted your scenes to use asset management, you should disable this feature.
  /// \param bUse
  ///   \c true to enabled this functionality; \c false to disable it
  VBASE_IMPEXP void SetUseAlternativeFiles(bool bUse);
}

namespace AssetProfile
{
  /// \brief
  ///   Returns the asset profile name currently being used.
  /// \return
  ///   the name of the asset profile currently being used, or an empty string if no asset profile has been set
  VBASE_IMPEXP const char* GetProfileName();

  /// \brief
  ///   Sets the asset profile to use.
  ///
  /// Calls to this function set the asset profile name and the name of the asset look-up table to use.
  ///
  /// \param szProfileName
  ///   the name of the asset profile. If \c NULL or an empty string is being passed, and there is no profile currently
  ///   set, the profile name as well as the look-up table name are set to the default name for the current
  ///   platform. Otherwise, calls with empty profile names are ignored.
  /// \return
  ///   \c true if the name has been changed; \c false if not
  VBASE_IMPEXP bool SetProfileName(const char* szProfileName);

  /// \brief
  ///   Returns whether an asset profile name has been set.
  /// \return
  ///   \c true if a profile name has been set; \c false if not
  VBASE_IMPEXP bool IsProfileNameSet();

  /// \brief
  ///   Returns the name of the asset look-up table to load.
  ///
  /// If the name has not explicitly set to something else, this returns the same name as GetProfileName().
  ///
  /// \return
  ///   the name of the asset look-up table to load, or an empty string if no asset profile has been set
  VBASE_IMPEXP const char* GetLookUpTableName();

  /// \brief
  ///   Sets the name of the asset look-up table to load.
  ///
  /// Normally, you don't need to call this function, as SetProfileName() also set the name of the look-up
  /// table to the name of the profile. Only call this if you need to load a different look-up table.
  ///
  /// \param szLookUpTableName
  ///   the name of the look-up table to load. If the string is \c NULL or empty, the call to this function
  ///   has no effect.
  VBASE_IMPEXP void SetLookUpTableName(const char* szLookUpTableName);
}

namespace AssetVariantKeys
{
  /// \brief
  ///   Appends a variant key to the list of currently active variant keys.
  ///
  /// If the specified variant key is \c NULL, empty, or already present in the list of active variant
  /// keys, this call has no effect.
  ///
  /// \param key
  ///   the variant key to append.
  VBASE_IMPEXP void Add(const char* key);

  /// \brief
  ///   Returns the number of asset variant keys currently set.
  /// \return
  ///   the number of asset variant keys currently set
  VBASE_IMPEXP int GetCount();
  
  /// \brief
  ///   Returns a specific asset variant key.
  /// \param iIndex
  ///   the key to return
  /// \return
  ///   the requested key, or \c NULL, if \c iIndex was not valid.
  VBASE_IMPEXP const char* Get(int iIndex);

  /// \brief 
  ///   Clears all set asset variant keys.
  VBASE_IMPEXP void Clear();
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
