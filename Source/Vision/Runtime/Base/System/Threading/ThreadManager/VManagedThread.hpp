/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VManagedThread.hpp

#ifndef VMANAGEDTHREAD_HPP_INCLUDED
#define VMANAGEDTHREAD_HPP_INCLUDED

#include <Vision/Runtime/Base/System/Threading/ThreadManager/VThreadedTask.hpp>


#ifdef _VISION_POSIX
  #include <semaphore.h>
#endif

#define TASKMASK_ALL                0xFFFFFFFF
#define TASKMASK_BACKGROUNDLOADING  0x1
#define TASKMASK_BACKGROUNDCRITICAL 0x2

class VThreadManager;

#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>
#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEvent.hpp>
#include <Vision/Runtime/Base/System/Threading/Thread/VThread.hpp>

/// \brief
///   Thread controlled by a VThreadManager object in which VManagedTasks can be
///   executed.
/// 
/// Direct access to this thread object is only available in the Run method of the task.
/// 
/// The VManagedThread class is primarily exposed to application developers in order to provide
/// convenient access to the thread's scratch heap allocation methods.
class VManagedThread
{
public:

  /// \brief
  ///   Returns the thread handle
  /// 
  /// \return
  ///   HANDLE: The thread handle
  VBASE_IMPEXP HANDLE GetHandle() const;

  /// \brief
  ///   Ensures that one of the thread's local heaps has at least the specified size.
  /// 
  /// A thread can have its own thread-local memory (divided in multiple heaps)
  /// 
  /// The heap memory will be freed when the thread exits (e.g. when the VThreadManager is
  /// deinitialized or destroyed, or when the number of threads in the VThreadManager is changed).
  /// 
  /// \param iHeapID
  ///   The unique identifier of the local heap to be used
  /// 
  /// \param iBytes
  ///   Minimum local heap size in Number of bytes
  VBASE_IMPEXP void EnsureHeapSize(unsigned int iHeapID, unsigned int iBytes);

  /// \brief
  ///   Frees one of the thread's local heaps' memory.
  /// 
  /// A thread can have its own thread-local memory.
  /// 
  /// \param iHeapID
  ///   The unique identifier of the local heap to be used.
  VBASE_IMPEXP void FreeHeap(unsigned int iHeapID);

  /// \brief
  ///   Frees the memory of all the thread's local heaps.
  /// 
  /// A thread can have its own thread-local memory.
  VBASE_IMPEXP void FreeHeaps();

  /// \brief
  ///   Returns a pointer to one of the thread's local heaps' memory.
  /// 
  /// Will return \c NULL if no memory has been allocated yet.
  /// 
  /// \param iHeapID
  ///   The unique identifier of the local heap to be used.
  /// 
  /// \return
  ///   a pointer to the thread's local heap memory.
  VBASE_IMPEXP void *GetHeapPointer(unsigned int iHeapID);

  
  // Internal use only:
  VManagedThread(VThreadManager *pManager, int iStackSize = 0);
  ~VManagedThread();

  VBASE_IMPEXP void Start();

  bool IsIdle() const { return m_pTask == NULL; }
  inline bool IsSignaled() const { return m_bSignaled; }
  //bool IsLocked() const { return m_bIsLocked; }

#ifdef WIN32
  static DWORD WINAPI Process(void *pParam);

#elif defined(_VISION_XENON)
  static DWORD Process(void *pParam);

#elif defined(_VISION_PS3)
  static void Process(uint64_t arg);

#elif defined(_VISION_POSIX)
  static void* Process(void* pParam);

#elif defined(_VISION_PSP2)
  static SceInt32 Process(SceSize ArgSize, void* pArgBlock);

#elif defined(_VISION_WIIU)
  static int Process(int intArg, void *ptrArg);

#else
  #error "Missing Platform!"

#endif

  void ScheduleTask(VThreadedTask *pTask);
  bool SetPriority(VThreadPriority ePriority);
  VThreadPriority GetPriority() const;
  int GetProcessor() const;
  void SetProcessor(int iProcessor);
  void SetTaskMask(unsigned int iBitmask);
  unsigned int GetTaskMask() const;
  void WaitUntilFinished();
  void Signal();
  unsigned int GetStackSize() const;

  /// \brief
  ///   Register to this callback to listen to thread local variable initialization events
  VBASE_IMPEXP static VCallback OnThreadLocalVariableInit;

  /// \brief
  ///   Register to this callback to listen to thread local variable de-initialization events
  VBASE_IMPEXP static VCallback OnThreadLocalVariableDeInit;

private:
  friend class VThreadManager;
  volatile unsigned int m_iTaskMask;

#if defined (WIN32)  || defined (_VISION_XENON) 
  DynArray_cl<void * volatile> m_pLocalScratchHeaps;
  DynArray_cl<volatile unsigned int> m_iScratchHeapSizes;
#else
  DynArray_cl<void *> m_pLocalScratchHeaps;
  DynArray_cl<unsigned int> m_iScratchHeapSizes;
#endif

  VThreadManager * volatile m_pManager;

  volatile bool m_bProcessingFinished;
  volatile bool m_bExit;
  volatile unsigned int m_iTasksExecuted;
  volatile unsigned int m_iTasksExecutedLastFrame;
  volatile float m_fBusyTime;
  volatile float m_fWorkload;

  VEvent m_Event;
  VThread *m_pThread;

  VThreadedTask * volatile m_pTask;
  volatile bool m_bSignaled;

  void ResetTypeRecording(bool bResetTaskNames);
  void AddTaskToRecording(VThreadedTask *pTask);
  DynArray_cl<VType *> m_pTaskType;
  DynArray_cl<VType *> m_pTaskTypeLastFrame;
  DynArray_cl<int> m_iTaskTypeExecuted;
  DynArray_cl<int> m_iTaskTypeExecutedLastFrame;
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
