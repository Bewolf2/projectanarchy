/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFileAccess.hpp

#ifndef VFILEACCESS_HPP
#define VFILEACCESS_HPP



/// \brief Arguments for SetPosition.
enum VFilePositionMode
{
#ifdef _VISION_PSP2
  VFS_SETPOS_SET      = SCE_SEEK_SET, ///< Sets the file position relative to the start of the file.
  VFS_SETPOS_CURRENT  = SCE_SEEK_CUR, ///< Sets the file position relative to the current file position.
  VFS_SETPOS_END      = SCE_SEEK_END  ///< Sets the file position relative to the end of the file. 
#else
  VFS_SETPOS_SET      = SEEK_SET,     ///< Sets the file position relative to the start of the file.
  VFS_SETPOS_CURRENT  = SEEK_CUR,     ///< Sets the file position relative to the current file position.
  VFS_SETPOS_END      = SEEK_END      ///< Sets the file position relative to the end of the file. 
#endif
};

/// \brief
///   Low-level file handle for use with VFileAccess functions.
#if defined(WIN32)  || defined(_VISION_XENON) 

struct VFileHandle
{
  VFileHandle()
  {
    hFile = INVALID_HANDLE_VALUE;
    bUseReadCache = true;
    bEOF = false;
  }
  
  HANDLE hFile;
  bool bUseReadCache;
  bool bEOF;
};


#elif defined(_VISION_PS3)

struct VFileHandle
{
  VFileHandle()
  {
    file = NULL;
    uiSize = 0;
    uiBytesRead = 0;
    bUseReadCache = true;
    fd = 0;
    bUseCellFs = false;
  }

  uint64_t uiSize;
  uint64_t uiBytesRead;
  FILE *file;
  int fd;
  bool bUseCellFs;
  bool bUseReadCache;
};



#elif defined(_VISION_WIIU)

/// \brief
///   Low-level file handle for use with VFileAccess functions.
struct VFileHandle
{
  long            pos;
  bool            bUseReadCache;

  // Intermediate buffer used for reading and writing (avoids alignment issues, etc.).
  s8*  dataBuffer;
  s32  validSize;
  bool writing;

  FSFileHandle fsFileHandle;
  FSStatus fsLastStatus;

  bool bUsesSD;
  
  VFileHandle()
    : pos(-1), dataBuffer(NULL), validSize(0), bUseReadCache(true), fsFileHandle(FS_INVALID_HANDLE_VALUE), fsLastStatus(FS_STATUS_OK), bUsesSD(false)
  {

  }
};


#elif defined(_VISION_PSP2)

struct VFileHandle
{
  VFileHandle() { fd = -1; lastError = 0; bEof = true; bUseReadCache = true; }
  SceUID fd;
  SceUID lastError;
  bool bEof;
  bool bUseReadCache;

  void setLastErrorFromValue(SceUID iVal)
  {
    if(iVal < 0)
      lastError = iVal;
  }

  void clearError()
  {
    lastError = 0;
  }

};



#elif defined(_VISION_POSIX) // Standard fopen(), fread() etc.

struct VFileHandle
{
  VFileHandle() { file = NULL; bUseReadCache = true; }
  FILE* file;
  bool bUseReadCache;
};

#else

  #error "Missing platform!"

#endif



/// \brief
///   The VFileAccess class is a monostate encapsulating low-level file access functions.
/// 
/// VFileAccess abstracts low-level file access functionality with different implementations for
/// file systems of other platforms. All file access of the engine should use this class.
class VFileAccess
{
private:  ///< Not instantiable.
  VFileAccess();

public:
  /// \brief
  ///   Flags which can be passed to the Open function specifying the mode in which the file should
  ///   be opened
  enum OpenFlags {
    read   = V_BIT(0),  ///< file should be opened in read mode
    write  = V_BIT(1),  ///< file should be opened in write mode
    append = V_BIT(2),  ///< file should be opened in append (write) mode


//    modeCreate    =     0x1000,   // file should be created new, otherwise the data would be appended to an existing file (only combinable with modeWrite)
//    modeCreateIfNotExist = 0x2000 // creates the file if it does not exits, opens an existing file
  };


  /// \brief
  ///   Flags which can be passed to the OpenShared function to specify shared reading and writing
  ///   rights
  enum ShareFlags {
    shRead   = V_BIT(0),
    shWrite  = V_BIT(1)
  };


  /// \brief
  ///   The seek position mode defines what a new position is relative to
  enum SeekPosition { 
    begin   = VFS_SETPOS_SET,       ///< new position is relative to the beginning of the file (GetSize() - 1 would be the last byte in the file)
    current = VFS_SETPOS_CURRENT,   ///< new position is relative to the current position in the file
    end     = VFS_SETPOS_END        ///< new position is relative to the end of the file (0 would be the last byte in the file)
  };


  /// \brief
  ///   Error code.
  enum FileErrorState { 
    FERROR_NONE,                ///< No error
    FERROR_UNKNOWN,             ///< Unknown error
    FERROR_NO_DISK,             ///< No disk in the drive
    FERROR_WRONG_DISK,          ///< Wrong disk in the drive
    FERROR_RETRY,               ///< Retry error occurred
    FERROR_FATAL,               ///< Fatal error occurred
    FERROR_FILENOTFOUND,        ///< File not found
    FERROR_ACCESSDENIED,        ///< Access to file denied
    FERROR_DISKFULL,            ///< Disk full
    FERROR_READWRITEFAULT,      ///< Read/Write Fault (e.g. media broken)
    FERROR_INVALID_FILEOP,      ///< General invalid file operation
    FERROR_MEDIA_NOT_READY      ///< Media not ready (e.g. no SD card)
  };


  /// \brief
  ///   Callback function that gets called after an error occurred. Returns true if the failed
  ///   operation must be repeated or false if the application must be terminated
  typedef bool(*ErrorHandlerFunc)(FileErrorState errorState);


#if defined(_VISION_WIIU)
  /// \brief
  ///   Callback function that gets called after a file operation error was recovered.
  typedef void(*ErrorRecoverHandlerFunc)();
#endif

  /// \brief
  ///   Reads \c size bytes from the file and store it at \c data. Returns the number of bytes which
  ///   have successfully been read, and a handle to the file.
  VBASE_IMPEXP static bool Open(VFileHandle* pFileHandle, const char* szFileName, unsigned int uiOpenFlags, unsigned int uiShareFlags  = 0);

  /// \brief
  ///   Reads \c size bytes from the file and store it at \c data. Returns the number of bytes which
  ///   have successfully been read.
  VBASE_IMPEXP static long Read(VFileHandle* pFileHandle, void* pDataBuffer, size_t uiReadSize);


#if defined(_VISION_WIIU)

  typedef void (*AsyncReadFinishedCallback)(void* pData, u32 numBytesRead);

#endif

  /// \brief
  ///   Writes \c size bytes to the file, source is \c data. Returns the number of bytes which have
  ///   successfully been written.
  VBASE_IMPEXP static long Write(VFileHandle* pFileHandle, const void* pDataBuffer, size_t uiWriteSize);

  /// \brief
  ///   Returns the overall size of the file in bytes.
  VBASE_IMPEXP static long GetSize(VFileHandle* pFileHandle);

  /// \brief
  ///   Return the current position in the file (or -1 if an error occurred).
  VBASE_IMPEXP static long GetPos(VFileHandle* pFileHandle);

  /// \brief
  ///   Sets the current position in the file, either relative to the beginning (begin), the
  ///   current position (current) or the end of the file (end).
  VBASE_IMPEXP static bool SetPos(VFileHandle* pFileHandle, long iOffset, SeekPosition mode);

  /// \brief
  ///   Closes the file.
  VBASE_IMPEXP static void Close(VFileHandle* pFileHandle);

  /// \brief
  ///   Returns TRUE if the last operation did read beyond the end of the file.
  VBASE_IMPEXP static bool IsEOF(VFileHandle* pFileHandle);

  /// \brief
  ///   Causes the file to flush all caches (in case there are any caches, otherwise it does
  ///   nothing).
  VBASE_IMPEXP static void Flush(VFileHandle* pFileHandle);


  /// \brief
  ///   Returns the last error state.
  VBASE_IMPEXP static FileErrorState GetLastError(VFileHandle* handle);

  /// \brief
  ///   Returns the error handler callback function.
  VBASE_IMPEXP static ErrorHandlerFunc GetErrorHandlerCallback();

  /// \brief
  ///   Sets a new error handler function and returns the old one.
  VBASE_IMPEXP static ErrorHandlerFunc SetErrorHandlerCallback(ErrorHandlerFunc func);

  /// \brief
  ///   Default error handler.
  VBASE_IMPEXP static bool HandleError(FileErrorState errorState);

  /// \brief
  ///   On PS3, this can be used to ensure that no file operations are executed from the
  ///   game/render thread(s) (relevant for certification). In debug builds, simply call this
  ///   method from the render or logic thread with "true" as parameter. All file operations called
  ///   from this thread will then write an error message to stderr. Not supported on other
  ///   platforms.
  VBASE_IMPEXP static void SetThreadValidation(bool bStatus);

  /// \brief
  ///   Returns the value previously set with SetThreadValidation (default: false).
  VBASE_IMPEXP static bool GetThreadValidation();

  
  #ifdef _VISION_PS3

  /// \brief
  ///   Switches between regular C-style and CellFs file access functions on PS3
  ///
  /// Use this function to switch between C-style and CellFs file access functions on PS3.
  /// Passing true will cause all following file accesses to use cellFs functions. Passing false (which
  /// is the default) will result in all regular file accesses using the standard C-style functions,
  /// while accesses to .sdat files will use the cellFs functions.
  ///
  /// \note
  ///   The chosen file access method may significantly affect loading performance. Especially when
  ///   loading data from the host PC, cellFs functions are typically slower than C-style file accesses.
  ///
  /// \note
  ///   Make sure not to call this functions while you still have files open. The best time to call
  ///   this function is at startup time of your application.
  ///
  /// \param bStatus
  ///   true to use cellFs file access functions for all file accesses.
  inline static void SetForceCellFs(bool bStatus) 
  { 
    s_bForceCellFs = bStatus; 
  }

  /// \brief
  ///   Returns the value previously set with SetForceCellFs. Default is false.
  inline static bool GetForceCellFs() 
  { 
    return s_bForceCellFs; 
  }

  #endif
  
  #if defined(_VISION_WIIU)
  VBASE_IMPEXP static bool MountSD(VString& szMountPath);
  VBASE_IMPEXP static bool UnmountSD();
  #endif


private:

  static bool Open_Internal(VFileHandle* pFileHandle, const char* szFileName, unsigned int uiOpenFlags, unsigned int uiShareFlags);

  static size_t Read_Internal(VFileHandle* pFileHandle, void* pDataBuffer, size_t uiReadSize);

  static size_t Write_Internal(VFileHandle* pFileHandle, const void* pDataBuffer, size_t uiWriteSize);

  static bool IsValidHandle_Internal(VFileHandle* pFileHandle);

  static size_t GetSize_Internal(VFileHandle* pFileHandle);

  static size_t GetPos_Internal(VFileHandle* pFileHandle);

  static bool SetPos_Internal(VFileHandle* pFileHandle, long iOffset, SeekPosition mode);

  static void Close_Internal(VFileHandle* pFileHandle);

  static bool IsEOF_Internal(VFileHandle* pFileHandle);

  static void Flush_Internal(VFileHandle* pFileHandle);

  // Internal error check helper
  #if defined(WIN32)  || defined(_VISION_XENON) || defined(_VISION_PSP2) || defined(_VISION_POSIX) || defined(_VISION_WIIU)
    static bool CheckForError(VFileHandle* handle);
  
  #elif defined(_VISION_PS3)
    static int s_iCellFsMemContainerRefs;
    static sys_memory_container_t s_cellFsMemContainer;
    static bool s_bForceCellFs;

    VBASE_IMPEXP static bool Open_CellFs(VFileHandle* handle, const char *name, int flags);
    VBASE_IMPEXP static long Read_CellFs(VFileHandle* handle, void* data, long size);
    VBASE_IMPEXP static long Write_CellFs(VFileHandle* handle, const void *data, long size);
    VBASE_IMPEXP static long GetSize_CellFs(VFileHandle* handle);
    VBASE_IMPEXP static long GetPos_CellFs(VFileHandle* handle);
    VBASE_IMPEXP static bool SetPos_CellFs(VFileHandle* handle, long pos, SeekPosition mode);
    VBASE_IMPEXP static void Close_CellFs(VFileHandle* handle);
    VBASE_IMPEXP static bool IsEOF_CellFs(VFileHandle* handle);
    VBASE_IMPEXP static void Flush_CellFs(VFileHandle* handle);
    static bool CheckForError(VFileHandle* handle);
    static bool CheckForError_CellFs(CellFsErrno errNo);
  
  #endif

  // Vision default file error handler
  static VFileAccess::ErrorHandlerFunc ms_errorHandler;


#if defined(_VISION_WIIU)
  // Vision default file error recover handler
  static VFileAccess::ErrorRecoverHandlerFunc ms_errorRecoverHandler;

  static VString m_sSDMountPath;
#endif


  
  #if defined(_VISION_PS3)
    static sys_ppu_thread_t s_mainThread;
  #endif
  
};


#endif  // VFILEACCESS_HPP

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
