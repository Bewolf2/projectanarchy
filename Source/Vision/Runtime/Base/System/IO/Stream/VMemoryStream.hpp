/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMemoryStream.hpp

#ifndef VMEMORYSTREAM_HPP_INCLUDED
#define VMEMORYSTREAM_HPP_INCLUDED

#include <Vision/Runtime/Base/System/Resource/VResource.hpp>
#include <Vision/Runtime/Base/System/Resource/VResourceManager.hpp>
#include <Vision/Runtime/Base/System/Threading/ThreadManager/VThreadManager.hpp>
#include <Vision/Runtime/Base/System/Threading/ThreadManager/VThreadedTask.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VChunkFile.hpp>



/// \brief
///   A memory stream is a block of data, which can be used like a file, which you can write or read using the stream interfaces.
///
/// This allows, for example, to serialize an object to memory, instead of to a file, and then deserialize it again (e.g. to restore an object after an undo operation).
class VMemoryStream : public VRefCounter
{
public:

  /// \brief
  ///   You can specify a 'stream name', which will be passed through to 'VMemoryOutStream::GetFileName' and 'VMemoryInStream::GetFileName'.
  ///   'iInitialSize' specifies the amount of bytes to allocate up front for the memory block.
  VBASE_IMPEXP VMemoryStream(const char *szName, int iInitialSize = 0);

  /// \brief
  ///   Destructor. Cleans up the memory.
  VBASE_IMPEXP virtual ~VMemoryStream()     { Reset(); }

  /// \brief
  ///   Deallocates the internal memory block and resets the stream size to zero.
  inline void Reset(bool bFreeMemory = true) { m_iSize=0; if (bFreeMemory) m_pData.Reset(); }

  /// \brief
  ///   Returns the current number of bytes in the stream
  inline int GetSize() const                {return m_iSize;}

  /// \brief
  ///   Returns the internal stream name.
  inline const char *GetStreamName() const  {return m_sName;}

  /// \brief
  ///   Sets the internal stream name. This name will be passed through to 'VMemoryOutStream::GetFileName' and 'VMemoryInStream::GetFileName'.
  inline void SetStreamName(const char* szName) { m_sName = szName; }

  /// \brief
  ///   Copies a range of bytes [iStart .. iStart+iCount] from the stream into pDest. Does not modify the stream.
  ///   If the range is larger than the stream size, it will be clamped. The return value indicates how many bytes were actually copied.
  inline int ReadBytes(void* pDest, int iStart, int iCount) const
  {
    if (iStart + iCount > m_iSize)
      iCount = m_iSize - iStart;

    if (iCount < 1) 
      return 0;

    VASSERT(pDest);
    memcpy(pDest, &m_pData.GetDataPtr()[iStart], iCount);

    return iCount;
  }

  /// \brief
  ///   Appends iCount bytes from pSrc to the stream.
  inline int AppendBytes(const void* pSrc, int iCount)
  {
    if (iCount < 1) 
      return 0;

    VASSERT(pSrc);

    int iAlignedSize = VPointerArrayHelpers::GetAlignedElementCount(m_pData.GetSize(), m_iSize + iCount);
    m_pData.EnsureSize(iAlignedSize);

    memcpy(&m_pData.GetDataPtr()[m_iSize],pSrc,iCount);
    m_iSize += iCount;
    return iCount;
  }

  /// \brief
  ///   Inserts iCount bytes from pSrc into the memory stream, after position iPos.
  inline void InsertBytesAt(int iPos, const void* pSrc, int iCount)
  {
    if (iCount < 1) 
      return;

    VASSERT(pSrc && iPos>=0 && iPos<=m_iSize);

    m_pData.EnsureSize(m_iSize+iCount);

    int iTailCount = m_iSize-iPos;

    if (iTailCount > 0)
      memmove(&m_pData.GetDataPtr()[iPos+iCount],&m_pData.GetDataPtr()[iPos], iTailCount);

    memcpy(&m_pData.GetDataPtr()[iPos], pSrc, iCount);

    m_iSize += iCount;
  }

  /// \brief
  ///   Allocates a certain amount of bytes at the end of the buffer and returns a pointer to the beginning of that new empty space so that you can fill it manually.
  ///
  /// The size of the memory stream will be increased by iCount.
  ///
  /// \param iCount
  ///   Number of bytes to allocate.
  /// 
  /// \sa GetDataPtr
  inline void* AllocateBytes(int iCount)
  {
    if (iCount < 1) 
      return NULL;

    m_pData.EnsureSize(m_iSize+iCount);

    void* pBuf = &m_pData.GetDataPtr()[m_iSize];

    m_iSize += iCount;
    return pBuf;
  }

  /// \brief
  ///   Reads up to iSize bytes from pInStream and appends it to this stream. Can optionally write its progress regularly to pfProgress. Returns how many bytes were actually copied.
  VBASE_IMPEXP int CopyFromStream(IVFileInStream* pInStream, int iSize, volatile float* pfProgress = NULL);

  /// \brief
  ///   Copies up to iSize bytes from this stream to pOutStream. Returns how many bytes were actually copied.
  VBASE_IMPEXP int CopyToStream(IVFileOutStream *pOutStream, int iSize=-1) const;

  /// \brief
  ///   Allows direct access to the block of memory. Do not cache this pointer as it might change
  ///   when appending bytes
  inline void *GetDataPtr() {return m_pData.GetDataPtr();}
  
  /// \brief
  ///   Returns the amount of memory that is used by this memory stream. This does not necessarily match the value returned by GetSize.
  inline size_t GetMemoryFootprint() const
  { 
    return m_pData.GetSize() + m_sName.GetSize(); 
  }

  /// \brief Time stamp of the buffered file.
  VFileTime m_timeStamp;

private:
  int m_iSize;
  VString m_sName;
  DynArray_cl<char> m_pData;
};

typedef VSmartPtr<VMemoryStream> VMemoryStreamPtr;


/// \brief
///   VThreadedTask class that performs background loading
class VLoadingTask : public VThreadedTask, public VManagedResource, public VUserDataObj
{
public:
  V_DECLARE_DYNCREATE_DLLEXP(VLoadingTask,  VBASE_IMPEXP)

    /// \brief Constructor.
  inline VLoadingTask() : VManagedResource(NULL) { }

  /// \brief Constructor.
  VBASE_IMPEXP VLoadingTask(class VMemoryStreamManager* pParentManager, const char* szFilename, IVFileStreamManager* pManager = NULL);

  /// \brief Constructor.
  VBASE_IMPEXP VLoadingTask(class VMemoryStreamManager* pParentManager, const char* szFilename, const unsigned char* pSourceBuffer, int iSourceBufferSize);

  /// \brief Destructor.
  VBASE_IMPEXP virtual ~VLoadingTask();

  /// \internal Called by VMemoryStreamManager.
  VBASE_IMPEXP void ScheduleLoadingTask();

  /// \internal Called by VMemoryStreamManager.
  inline void FlagForQueuing()
  {
    RemoveResourceFlag(VRESOURCEFLAG_LOADING_RELATED);
    SetResourceFlag(VRESOURCEFLAG_ISQUEUED);
  }

  /// \brief
  ///   Indicates whether this task has a valid stream to read from. False for file not found.
  inline bool IsValid() const
  {
    return m_bSuccess;
  }

  /// \brief
  ///   Returns the memory stream that is used for streaming in the data 
  inline VMemoryStream* GetStream() const 
  {
    return m_spStream;
  }

  /// \brief
  ///   If the underlying file was resolved via asset lookup, returns the data directory in 
  ///   which the asset was first found. Returns NULL if the file was not resolved via asset lookup.
  inline const char* GetAssetInitialDataDir() const
  {
    return m_sAssetInitialDataDir;
  }

  /// \brief
  ///   If the underlying file was resolved via asset lookup, returns the asset metadata from 
  ///   the lookup table entry. Returns NULL if the file was not resolved via asset lookup.
  inline const char* GetAssetMetadata() const
  {
    return m_sAssetMetadata;
  }

  /// \brief
  ///   If the underlying file was resolved via asset lookup, returns the hash identifying the
  ///   lookup table entry. Returns 0 if the file was not resolved via asset lookup.
  inline unsigned int GetAssetLookupHash() const
  {
    return m_uiAssetLookupHash;
  }

  /// \brief
  ///   Returns the current loading progress as a percentage value.
  inline float GetProgress() const 
  {
    return m_fProgress;
  }

  // overridden task functions
  virtual void Run(VManagedThread *pThread) HKV_OVERRIDE;
  virtual void OnFinished(VManagedThread *pThread) HKV_OVERRIDE;

protected:
  friend class VMemoryStreamManager;

  // overridden resource functions
  virtual BOOL Reload() HKV_OVERRIDE;
  virtual BOOL Unload() HKV_OVERRIDE;
  virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU) HKV_OVERRIDE;

  IVFileStreamManagerPtr m_spStreamManager;
  VMemoryStreamPtr m_spStream;
  bool m_bSuccess;
  volatile float m_fProgress;
  size_t m_iStreamSize;
  VString m_sAssetInitialDataDir;
  VString m_sAssetMetadata;
  unsigned int m_uiAssetLookupHash;

public:
  //type management

};

typedef VSmartPtr<VLoadingTask> VLoadingTaskPtr;



/// \brief
///   Resource manager that manages memory streams that can be loaded from file in the background.
class VMemoryStreamManager : public VResourceManager
{
public:

  /// \brief Constructor.
  VBASE_IMPEXP VMemoryStreamManager(const char* szManagerName, int iFlags = VRESOURCEMANAGERFLAG_NONE, int iMemLimit = 0);

  /// \brief Destructor.
  VBASE_IMPEXP virtual ~VMemoryStreamManager();

  /// \brief
  ///   Sets a new task bit mask for all upcoming loading tasks.
  ///   See VThreadManager::SetThreadTaskMask. The default is set to TASKMASK_BACKGROUNDLOADING.
  inline void SetTaskMask(unsigned int iTaskMask)
  {
    m_iTaskMask = iTaskMask;
  }

  /// \brief
  ///   Returns the mask that has been set via SetTaskMask. The default is set to TASKMASK_BACKGROUNDLOADING.
  inline unsigned int SetTaskMask() const
  {
    return m_iTaskMask;
  }

  /// \brief
  ///   Indicates whether there are any files cached.
  inline bool HasFiles() const {return GetResourceCount() > 0;}

  /// \brief
  ///   Looks up a cached entry by filename and returns a memory stream to it.
  VBASE_IMPEXP IVFileInStream* Open(const char *szPathname, int iFlags = 0);

  /// \brief
  ///   Looks up a cached entry by filename.
  VBASE_IMPEXP VLoadingTask* FindPrecachedFile(const char *szFilename) const;

  /// \brief
  ///   Looks up a cached entry by user data pointer (See VLoadingTask::GetUserData)
  VBASE_IMPEXP VLoadingTask* FindPrecachedFileByUserData(const void *pUserData) const;


  /// \brief
  ///   Schedules a file for loading (if not already in the resource list) 
  ///
  ///   If the manager is currently loading a resource, this loading task won't be scheduled
  ///   and the VLoadingTask::GetState() will return TASKSTATE_UNASSIGNED and you would need 
  ///   to call to VLoadingTask::ScheduleLoadingTask() manually.
  ///
  /// \param szFilename
  ///   the path to the file which should be precached.
  /// \param pManager
  ///   the file manager to use for opening the file. If \c NULL, uses the default file stream
  ///   manager set in vBase.
  /// \note 
  ///   You can use VThreadManager::GetManager()->WaitForTask() method along with the 
  ///   returned VLoadingTask if you want to wait until the file has been loaded.
  VBASE_IMPEXP VLoadingTask* PrecacheFile(const char *szFilename, IVFileStreamManager* pManager=NULL);

  /// \brief
  ///   Uses the passed memory block to create a pre-cache entry for the specified file
  ///   (if not already in the resource list). The data can be used as soon as this 
  ///   method returns. The data itself is copied, so there is no dependency on the
  ///   passed memory block after this method return.
  ///
  /// The resulting VLoadingTask is returned. In case no entry for this resource had existed
  /// before, the task is already finished. Otherwise, the preexisting loading task is returned,
  /// and its state has to be determined - see the documentation for PrecacheFile().
  ///
  /// \param szFilename
  ///   the path to the file which should be precached
  ///
  /// \param pData
  ///   the memory block which already contains the file data
  ///
  /// \param iDataSize
  ///   the size of the data block
  VBASE_IMPEXP VLoadingTask* PrecacheFileFromMemory(const char *szFilename, const unsigned char* pData, int iDataSize);

  /// \brief
  ///   Waits until the current loading operation is finished and then cleans up all resources.
  VBASE_IMPEXP void WaitUntilLoadingFinished();

  // overridden tick function
  VBASE_IMPEXP virtual void OnTickFunction(float dtime) HKV_OVERRIDE;

private:
  VSmartPtr<VLoadingTask> m_spCurrentLoadingRes;
  unsigned int m_iTaskMask;
};



/// \brief
///   Derived IVFileInStream class that wraps around a memory stream
class VMemoryInStream : public IVFileInStream
{
public:

  /// \brief
  ///   Constructor. Takes the stream source to read the data from.
  VBASE_IMPEXP VMemoryInStream(IVFileStreamManager* pManager, VMemoryStream* pSource);

  VBASE_IMPEXP virtual size_t Read(void* pBuffer,int iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL SetPos(LONG iPos, int iMode) HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetPos() HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetSize() HKV_OVERRIDE;
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;
  VBASE_IMPEXP virtual bool GetTimeStamp(VFileTime& result) HKV_OVERRIDE;

private:
  VMemoryStreamPtr m_spStream;
  int m_iCurrentPos;
};



/// \brief
///   Derived IVFileOutStream class that wraps around a memory stream
class VMemoryOutStream : public IVFileOutStream
{
public:
  /// \brief
  ///   Constructor. Takes the stream destination to write the data to.
  VBASE_IMPEXP VMemoryOutStream(IVFileStreamManager *pManager, VMemoryStream *pDest);

  VBASE_IMPEXP virtual size_t Write(const void* pBuffer,size_t iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;
  VBASE_IMPEXP virtual void Flush() HKV_OVERRIDE;
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;

  /// \brief Returns the pointer to the VMemoryStream object.
  inline VMemoryStream* GetStream() { return m_spStream; }

private:
  VMemoryStreamPtr m_spStream;
};


/// \brief
///   Same as class VMemoryInStream but does not delete itself in the Close routine, so it can be used as a local variable or class member
class VMemoryInStreamLocal : public VMemoryInStream
{
public:
  /// \brief Constructor.
  VMemoryInStreamLocal(VMemoryStream* pSource) : VMemoryInStream(NULL,pSource) {}

  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE { }
};

/// \brief
///   Same as class VMemoryOutStream but does not delete itself in the Close routine, so it can be used as a local variable or class member
class VMemoryOutStreamLocal : public VMemoryOutStream
{
public:
  /// \brief Constructor.
  VMemoryOutStreamLocal(VMemoryStream* pSource) : VMemoryOutStream(NULL,pSource) {}

  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE { }
};


/// \brief
///   Derived IVFileInStream class that wraps around a block of memory.
///
/// This is the easiest way to feed a plain memory block to functions that take a IVFileInStream to read from.
/// Since the Close method does not delete the stream object, this wrapper can be a local variable.
class VMemBlockWrapperStream : public IVFileInStream
{
public:

  /// \brief
  ///   Constructor. Takes the memory pointer and the amount of memory in the block. The memory must persist the lifetime of this stream
  VBASE_IMPEXP VMemBlockWrapperStream(const void* pData, int iSize);

  /// \brief
  ///   Left empty (does not delete this stream object).
  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;

  /// \brief
  ///   Always returns NULL.
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;


  VBASE_IMPEXP virtual size_t Read(void* pBuffer, int iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL SetPos(LONG iPos, int iMode) HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetPos() HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetSize() HKV_OVERRIDE;

private:
  const char *m_pData;
  int m_iSize;
  int m_iCurrentPos;
};



/// \brief
///   Helper class for using temporary memory (on the stack) without allocating memory
/// 
/// This class is very useful in cases a temporary buffer of variable size is needed in a local
/// function.
/// 
/// Instead of allocating the required size every time in the function, this class provides a
/// static memory size on the stack.
/// 
/// If this static size is not sufficient, it allocates memory via malloc.
/// 
/// The static size can be specified as a template parameter. It can thus be chosen to be
/// sufficient for 99% of the cases in the function to avoid memory allocation.
/// 
/// For instance, for string conversions, most of the strings have less than 256 characters (but
/// this is however not guaranteed).
/// 
/// The EnsureCapacity function provides you with a buffer that is large enough. If it needs to be
/// larger than the templated static size, it allocates memory.
/// 
/// The allocated memory is free'd in the destructor.
template<int iStaticSize>
class VMemoryTempBuffer
{
public:

  /// \brief
  ///   Constructor. Makes sure there is enough capacity for iInitialSize bytes
  inline VMemoryTempBuffer(int iInitialSize = 0)
  {
    m_iAllocatedSize = iStaticSize;
    m_pBuffer = m_StaticBuffer;
    EnsureCapacity(iInitialSize);
  }

  /// \brief
  ///   Constructor. Creates a copy string (reserves enough memory)
  inline VMemoryTempBuffer(const char* szCopyStr)
  {
    if (szCopyStr == NULL)
      szCopyStr = "";

    m_iAllocatedSize = iStaticSize;
    m_pBuffer = m_StaticBuffer;

    int iLen = (int)strlen(szCopyStr);
    EnsureCapacity(iLen + 1);

	#pragma warning(push)
	#pragma warning(disable:4996)
    strcpy(AsChar(), szCopyStr);
	#pragma warning(pop)
  }

  /// \brief
  ///   Makes sure there is enough capacity for iRequiredSize bytes. It uses the static memory if
  ///   possible.
  inline void* EnsureCapacity(int iRequiredSize, bool bPreserveContent = false)
  {
    if (iRequiredSize <= m_iAllocatedSize)
      return m_pBuffer;

    void* pTemp = NULL;

    #ifdef USE_VBASE_RESOURCE_ALLOC
      pTemp = VBaseResourceAlloc(iRequiredSize);
    #else
      pTemp = vMemAlloc(iRequiredSize);
    #endif

    if (bPreserveContent)
    {
      memcpy(pTemp, m_pBuffer, m_iAllocatedSize);
    }

    FreeBuffer();
    m_iAllocatedSize = iRequiredSize;
    m_pBuffer = pTemp;

    return m_pBuffer;
  }

  /// \brief
  ///   Releases the allocated buffer and resets the pointers to use the static buffer
  inline void FreeBuffer()
  {
    if (m_pBuffer && m_pBuffer != m_StaticBuffer) 
    {
      #ifdef USE_VBASE_RESOURCE_ALLOC
        VBaseResourceDealloc(m_pBuffer);
      #else
        vMemFree(m_pBuffer);
      #endif

      m_pBuffer = m_StaticBuffer;
      m_iAllocatedSize = iStaticSize;
    }
  }

  /// \brief
  ///   Returns a pointer to the current buffer.
  inline void* GetBuffer() const { return m_pBuffer; }

  /// \brief
  ///   Returns a pointer to the current buffer as a char pointer. This is just the casted version of GetBuffer()
  inline char* AsChar() const { return (char*) m_pBuffer; }

  /// \brief
  ///   Helper function to read a binary string from a stream. It tries to use the static memory
  ///
  /// \param pInStream
  ///   The file to read from
  ///
  /// \returns
  ///   String pointer (or NULL) that defines the null-terminated string
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines and empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  inline char* ReadStringBinary(IVFileInStream* pInStream)
  {
    VASSERT(pInStream);
    int iLen;
    if (pInStream->Read(&iLen,4,"i")!=4)
      return NULL;
    if (iLen<0)
      return NULL;
    EnsureCapacity(iLen+1);
    ((char *)m_pBuffer)[iLen] = 0;
    pInStream->Read(m_pBuffer,iLen);
    return (char *)m_pBuffer;
  }

  /// \brief
  ///   Helper function to read a binary string from an archive. It tries to use the static memory
  ///
  /// \param ar
  ///   The archive to read from
  ///
  /// \returns
  ///   String pointer (or NULL) that defines the null-terminated string
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines and empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  inline char* ReadStringBinary(VArchive& ar)
  {
    VASSERT(ar.IsLoading());
    int iLen;
    if (ar.Read(&iLen,4,"i")!=4)
      return NULL;
    if (iLen<0)
      return NULL;
    EnsureCapacity(iLen+1);
    ((char *)m_pBuffer)[iLen] = 0;
    ar.Read(m_pBuffer,iLen);
    return (char *)m_pBuffer;
  }

  /// \brief
  ///   Helper function to read a binary string from a chunk file. It tries to use the static
  ///   memory
  ///
  /// \param cfile
  ///   The file to read from
  ///
  /// \returns
  ///   String pointer (or NULL) that defines the null-terminated string
  ///
  /// \note
  ///   As all variants of the ReadStringBinary function, this function first reads a 32bit integer that defines the length of the string, where 0 defines and empty string and -1 a NULL string
  ///   Accordingly, all variants of ReadStringBinary are compatible.
  inline char* ReadStringBinary(VChunkFile& cfile)
  {
    VASSERT(cfile.IsLoading());
    int iLen;
    if (cfile.Read(&iLen,4,"i")!=4)
      return NULL;
    if (iLen<0)
      return NULL;
    EnsureCapacity(iLen+1);
    ((char *)m_pBuffer)[iLen] = 0;
    cfile.Read(m_pBuffer,iLen);
    return (char *)m_pBuffer;
  }

  /// \brief
  ///   Destructor. Cleans up the memory if allocated.
  inline ~VMemoryTempBuffer()
  {
    FreeBuffer();
  }

  /// \brief
  ///   Indicates whether memory had to be allocated or if the static buffer is still sufficient.
  inline bool HasAllocatedMemory() const
  {
    return m_pBuffer != m_StaticBuffer;
  }

  /// \brief
  ///   Returns the amount of memory used by this instance. This is iStaticSize + iAllocatedsSize
  inline int GetMemSize() const
  {
    if (HasAllocatedMemory())
      return iStaticSize + m_iAllocatedSize;

    return iStaticSize;
  }

private:

  int m_iAllocatedSize;
  VISION_ALIGN(16) char m_StaticBuffer[iStaticSize];

  void* m_pBuffer;
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
