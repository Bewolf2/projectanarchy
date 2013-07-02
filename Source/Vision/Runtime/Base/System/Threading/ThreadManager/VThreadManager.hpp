/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VThreadManager.hpp

#ifndef VTHREADMANAGER_HPP_INCLUDED
#define VTHREADMANAGER_HPP_INCLUDED


#include <Vision/Runtime/Base/System/Threading/ThreadManager/VThreadedTask.hpp>
#include <Vision/Runtime/Base/System/Threading/ThreadManager/VManagedThread.hpp>
#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>
#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEvent.hpp>
#include <Vision/Runtime/Base/System/Threading/Thread/VThread.hpp>


// config flag defines
#define REVERSE_ORDER_ON_WAIT       0x00000001
#define DO_NOT_IDLE                 0x00000002

//////////////////////////////////////////////////////////////////////////////////////////////////
// VThreadTaskList class
//////////////////////////////////////////////////////////////////////////////////////////////////

/// \brief
///   Double linked list for thread management
class VThreadTaskList
{
   public:
      /// \brief
      ///   constructor
      VThreadTaskList() : m_pHead(NULL), m_pTail(NULL) {}

      /// \brief
      ///  Gets the task at the beginning (head) of the list
      VThreadedTask*                GetHead() {return(m_pHead);}
      /// \brief
      ///  Gets the task at the end (tail) of the list
      VThreadedTask*                GetTail() {return(m_pTail);}

      /// \brief
      ///   inserts a task at the end (tail) of the list
      ///
      /// \param pTask
      ///   the task to insert
      void InsertTail(VThreadedTask* pTask)
      {
         if(m_pTail == NULL)
         {
            pTask->m_pNext = NULL;
            pTask->m_pPrevious = NULL;
            m_pHead = pTask;
            m_pTail = pTask;
         }
         else
         {
            pTask->m_pPrevious = m_pTail;
            pTask->m_pNext = NULL;
            m_pTail->m_pNext = pTask;
            m_pTail = pTask;
         }
      }
      
      /// \brief
      ///   removes a task from the list
      ///
      /// \param pTask
      ///   the task to remove
      void Remove(VThreadedTask* pTask)
      {
         if(pTask->m_pPrevious == NULL)
            m_pHead = pTask->m_pNext;
         else
            pTask->m_pPrevious->m_pNext = pTask->m_pNext;

         if(pTask->m_pNext == NULL)
            m_pTail = pTask->m_pPrevious;
         else
            pTask->m_pNext->m_pPrevious = pTask->m_pPrevious;
         
         pTask->m_pPrevious = NULL;
         pTask->m_pNext = NULL;
         pTask->m_iPriority = -1;
      }

   protected:
      VThreadedTask*                m_pHead;
      VThreadedTask*                m_pTail;
};



/// \brief
///   Thread manager class
/// 
/// A thread manager handles a variable number of worker threads and VThreadedTask objects.
/// 
/// Typically, every application will only have a single VThreadManager object which handles all
/// threads.
/// 
/// VThreadedTask objects are automatically assigned to idle threads. There is no hard limit
/// regarding the number of tasks that can be scheduled in the VThreadManager.
/// 
/// The thread manager is optimized for low scheduling and synchronization overhead. It can be used
/// for both high-priority tasks (e.g. physics, particles) and low-priority tasks (e.g. background
/// loading).
/// 
/// Tasks assigned to the thread manager have to be derived from the abstract base class
/// VThreadedTask.
class VThreadManager
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Thread manager default constructor.
  VBASE_IMPEXP VThreadManager();

  /// \brief
  ///   Thread manager destructor.
  VBASE_IMPEXP ~VThreadManager(); 
  
  ///
  /// @}
  ///

  ///
  /// @name Singleton
  /// @{
  ///
  
  /// \brief
  ///   Gets the thread manager singleton object.
  VBASE_IMPEXP static inline VThreadManager *GetManager() {return &s_ThreadMan;}

  ///
  /// @}
  ///

  ///
  /// @name Manager Properties
  /// @{
  ///

  /// \brief
  ///   Initializes the thread manager
  /// 
  /// \param iNumThreads
  ///   Number of threads to create. Note that the total number of  threads (including the
  ///   application thread) with significant permanent CPU load should not exceed the number of
  ///   hardware threads supported on the system.
  /// 
  /// \param iFlags
  ///   configs property bitflags:
  ///   \li REVERSE_ORDER_ON_WAIT is used to reverse the order of the task queue when the main
  ///     thread would otherwise be busy waiting for the top task to be finished.
  ///   \li DO_NOT_IDLE will let the main thread handle the next task in line if it would otherwise
  ///     be waiting for a task.
  /// 
  /// \param iStackSizePerThread
  ///   If set to -1, the stack size for every thread is equal to the stack size of the calling
  ///   thread. Otherwise, specify the stack size of the thread manager's threads here.
  /// 
  /// \param iNumPriorities
  ///   The number of priority levels that the manager will support. Tasks with a higher priority
  ///   will be executed earlier. The priority number that tasks can be given ranges from 0 to
  ///   iNumPriorities-1.
  /// 
  /// \param iCleanupFreq
  ///   Cleanup frequency of the task list - this value may be tweaked for optimal scheduling
  ///   performance. Usually, it is best to leave this at the default value.
  VBASE_IMPEXP void Initialize(int iNumThreads, int iFlags = REVERSE_ORDER_ON_WAIT, int iStackSizePerThread = -1, int iNumPriorities = 9, unsigned int iCleanupFreq = 100);

  /// \brief
  ///   Deinitializes the thread manager
  /// 
  /// Waits for all tasks to finish, then terminates all threads and deinitializes the thread
  /// manager.
  VBASE_IMPEXP void DeInitialize();
  
  /// \brief
  ///   Returns whether this thread manager has been initialized.
  /// 
  /// \return
  ///   TRUE if Initialize has been called and Deinitialize isn't yet called, otherwise FALSE.
  VBASE_IMPEXP inline bool IsInitialized() const {return m_bIsInitialized;}

  /// \brief
  ///   Sets new configuration flags in the ThreadManager, replacing the old configuration.
  /// 
  /// \param iFlags
  ///   configs property bitflags:
  ///   \li REVERSE_ORDER_ON_WAIT is used to reverse the order of the task queue when the main
  ///     thread would otherwise be busy waiting for the top task to be finished.
  ///   \li DO_NOT_IDLE will let the main thread handle the next task in line if it would otherwise
  ///     be waiting for a task.
  VBASE_IMPEXP void SetFlags(int iFlags);
  
  /// \brief
  ///   Gets the current configuration flags
  /// 
  /// \return
  ///   Configs property bitflags:
  ///   \li REVERSE_ORDER_ON_WAIT is used to reverse the order of the task queue when the main
  ///     thread would otherwise be busy waiting for the top task to be finished.
  ///   \li DO_NOT_IDLE will let the main thread handle the next task in line if it would otherwise
  ///     be waiting for a task.
  VBASE_IMPEXP int GetFlags();

  ///
  /// @}
  ///

  ///
  /// @name Threads
  /// @{
  ///

  /// \brief
  ///   Waits for all scheduled tasks to complete.
  VBASE_IMPEXP void WaitForAllThreads();

  /// \brief
  ///   Sets the number of worker threads
  /// 
  /// If there are still tasks pending or executing, this method first waits for these tasks to
  /// complete before destroying and re-creating any threads.
  /// 
  /// This method has high overhead, so don't call it too frequently.
  ///
  /// This method needs to be called after the thread manager has been initialized, which typically
  /// happens during engine initialization. Calling this method before initialization will not have
  /// any effect. If you want to use this method to change the thread count in vForge then you can
  /// listen to the VisCallbackManager_cl::OnEngineInit and perform the thread count change there.
  /// 
  /// \param iNumThreads
  ///   New number of threads
  VBASE_IMPEXP void SetThreadCount(int iNumThreads);

  /// \brief
  ///   Returns the number of worker threads
  /// 
  /// \return
  ///   Current number of worker threads
  VBASE_IMPEXP int  GetThreadCount() const;

  /// \brief
  ///   Sets the preferred processor for a specific thread.
  /// 
  /// Note that "processor" doesn't necessarily indicate a separate CPU core; it may also refer to
  /// the number of hardware threads supported on the system. Hyperthreaded systems, for instance,
  /// typically also report two processors per core.
  /// 
  /// Assigning a thread to a processor doesn't guarantee that the thread is always executed on
  /// that processor, it's merely treated as a hint.
  /// 
  /// \param iThreadIndex
  ///   Index of the thread, between 0 and the number of threads in the manager minus one.
  /// 
  /// \param iProcessor
  ///   Processor number, between 0 and GetNumberOfProcessors()-1
  VBASE_IMPEXP void AssignThreadToProcessor(int iThreadIndex, int iProcessor);

  /// \brief
  ///   Returns the preferred processor for a specific thread.
  /// 
  /// May be between 0 and GetNumberOfProcessors()-1
  /// 
  /// Note that "processor" doesn't necessarily indicate a separate CPU core; it may also refer to
  /// the number of hardware threads supported on the system. Hyperthreaded systems, for instance,
  /// typically also report two processors per core.
  /// 
  /// \param iThreadIndex
  ///   Index of the thread, between 0 and the number of threads in the manager minus one.
  /// 
  /// \return
  ///   Processor number, between 0 and GetNumberOfProcessors()-1
  VBASE_IMPEXP int  GetThreadProcessor(int iThreadIndex);

  /// \brief
  ///   Returns the stack size per thread for the thread manager
  /// 
  /// \return
  ///   If -1, the stack size for every thread is equal to the stack size of the thread the thread
  ///   manager was created in. Otherwise, specify the stack  size of the thread manager's threads
  ///   here.
  VBASE_IMPEXP int  GetStackSizePerThread() const;
  
  /// \brief
  ///   Returns the thread handle for a specific thread.
  ///
  /// The thread handle serves as a unique identifier for the thread. The actual meaning of the
  /// handle depends on the platform. For example, on Win32 the returned thread handle corresponds
  /// to a native Win32 thread handle.
  /// 
  /// \param iThreadIndex
  ///   Index of the thread, between 0 and the number of threads in the manager minus one.
  /// 
  /// \return
  ///   Thread handle
  VBASE_IMPEXP HANDLE GetThreadHandle(int iThreadIndex);

  /// \brief
  ///   Returns the number of priority levels that was set in this manager upon initialization.
  /// 
  /// \return
  ///   int: Number of priorities
  VBASE_IMPEXP inline int GetNumberOfPriorities() const {return m_iNumPriorities;}

  /// \brief
  ///   Sets the priority of the thread
  /// 
  /// For valid priority settings, see the VThreadPriority enum.
  /// 
  /// This method, in conjunction with the SetThreadTaskMask method, should be used in order to
  /// separate high-priority tasks (e.g. physics, particles, skinning) from low-priority tasks
  /// (e.g. background loading). There shouldn't be more high priority tasks with significant
  /// workload (including the application's main thread) than number of processors in the system.
  /// 
  /// \param iThreadIndex
  ///   Index of the thread, between 0 and the number of threads in the manager minus one.
  /// 
  /// \param ePriority
  ///   Priority of the thread
  VBASE_IMPEXP bool SetThreadPriority(int iThreadIndex, VThreadPriority ePriority);

  /// \brief
  ///   Returns the priority of the thread
  /// 
  /// For priority settings, see the VThreadPriority enum.
  /// 
  /// \param iThreadIndex
  ///   Index of the thread, between 0 and the number of threads in the manager minus one.
  VBASE_IMPEXP VThreadPriority GetThreadPriority(int iThreadIndex);

  /// \brief
  ///   Sets the task mask for a thread
  /// 
  /// Task masks can be used to specify which task is allowed to run on which thread(s). For
  /// instance, it is typically desired to run certain tasks (networking, background loading) in
  /// low-priority threads, while other tasks (particles, physics, etc.) should run in
  /// high-priority threads.
  /// 
  /// A task may run on a thread if the thread's task mask and the task's thread mask and-ed
  /// together are not zero.
  /// 
  /// \param iThreadIndex
  ///   Index of the thread, between 0 and the number of threads in the manager minus one.
  /// 
  /// \param iBitmask
  ///   The thread's task bitmask.
  VBASE_IMPEXP void SetThreadTaskMask(int iThreadIndex, unsigned int iBitmask);

  /// \brief
  ///   Returns the task mask for a thread
  /// 
  /// \param iThreadIndex
  ///   The thread for which to return the task mask.
  ///
  /// \return
  ///   unsigned int: The thread's task bitmask.
  VBASE_IMPEXP unsigned int GetThreadTaskMask(int iThreadIndex);


  /// \brief
  ///   Returns the VManagedThread object for the thread in which we're executing.
  /// 
  /// \return
  ///   The currently executing thread. NULL means that a thread for which no
  ///   VManagedThread object was made is currently executing (most likely the main thread.)
  VBASE_IMPEXP VManagedThread *GetExecutingThread();


  ///
  /// @}
  ///

  ///
  /// @name Tasks
  /// @{
  ///

  /// \brief
  ///   Schedules the passed task for execution.
  /// 
  /// Scheduling a task for execution multiple times before the task has reached the finished state
  /// is not allowed and may result in application crashes or deadlocks.
  /// 
  /// \param pTask
  ///   Task to be scheduled.
  /// 
  /// \param iPriority
  ///   Priority of the task. Lower numbers indicate higher priorities, i.e. a task with priority 1
  ///   will be executed before a task with priority 2.
  /// 
  /// \return
  ///   \c true if the task could be scheduled, \c false if not. False will only be returned if the
  ///   number of threads is 0; in this case, the task will immediately be executed in the calling
  ///   thread.
  VBASE_IMPEXP bool ScheduleTask(VThreadedTask *pTask, int iPriority = 4);

  /// \brief
  ///   Waits for a task to finish
  /// 
  /// This method should be used for synchronization purposes if the task's result is needed at a
  /// certain point in the calling thread. It has low overhead.
  /// 
  /// If the task isn't finished yet, this method will wait until it has reached the finished
  /// state.
  /// 
  /// If the parameter bMayExecuteInCallingThread is set to true, a task that hasn't started
  /// executing yet may be executed in the calling thread. This will typically result in better
  /// load balancing of the threads and thus improve performance.
  /// 
  /// \param pTask
  ///   Task to wait for.
  /// 
  /// \param bMayExecuteInCallingThread
  ///   Indicates whether pending tasks may be executed immediately in the calling thread.
  VBASE_IMPEXP void WaitForTask(VThreadedTask *pTask, bool bMayExecuteInCallingThread);

  /// \brief
  ///   Changes the priority of a task
  /// 
  /// After changing it may be executed sooner (lower number) or later (higher number) than it was
  /// previously scheduled.
  /// 
  /// \param pTask
  ///   The task to change the priority of.
  /// 
  /// \param iPriority
  ///   The new priority of the task.
  VBASE_IMPEXP void ChangeTaskPriority(VThreadedTask *pTask, int iPriority);



  ///
  /// @}
  ///

  ///
  /// @name Profiling Information
  /// @{
  ///


  /// \brief
  ///   Returns the number of processors in the system.
  /// 
  /// Note that "processor" doesn't necessarily indicate a separate CPU core; it may also refer to
  /// the number of hardware threads supported on the system. Hyperthreaded systems, for instance,
  /// typically also report two processors per core.
  /// 
  /// \return
  ///   int: Number of processors
  VBASE_IMPEXP static int GetNumberOfProcessors();


  /// \brief
  ///   Sets whether the workload of threads should be calculated or not.
  /// 
  /// If enabled, the task manager provides enhanced profiling information such as thread workload
  /// time or tasks evaluated.
  /// 
  /// Since this evaluation costs a bit of performance, it is disabled by default.
  /// 
  /// \param bGatherTaskInfo
  ///   true to enable thread workload evaluation, false to disable it.
  ///
  /// \sa GetThreadWorkload
  VBASE_IMPEXP void SetGatherTaskInfo(bool bGatherTaskInfo);

  /// \brief
  ///   Returns the status set via SetGatherTaskInfo
  inline bool GetGatherTaskInfo() const {return m_bGatherTaskInfo;}

  /// \brief
  ///   Only supported on Win2003Server, XP64 and Vista
  VBASE_IMPEXP static int GetCurrentProcessor();

  /// \brief
  ///   Returns the number of tasks processed since the last update for the specified thread
  VBASE_IMPEXP unsigned int GetTasksExecutedInThread(int iThreadIndex); 

  /// \brief
  ///   Returns the number of tasks processed since the last update for the main thread
  VBASE_IMPEXP unsigned int GetTasksExecutedInMainThread();

  /// \brief
  ///   Provides information about the distinct task types processed
  VBASE_IMPEXP int GetThreadTaskTypes(int iThreadIndex, DynArray_cl<VType *> &tasks, DynArray_cl<int> &executeCounts);

  /// \brief
  ///   Provides information about the distinct task types processed
  VBASE_IMPEXP int GetMainThreadTaskTypes(DynArray_cl<VType *> &tasks, DynArray_cl<int> &executeCounts);

  /// \brief
  ///   Returns the average workload [0..1] of the specified thread since the last update. Requires
  ///   SetGatherTaskInfo to be set, otherwise this function returns 0.0
  VBASE_IMPEXP float GetThreadWorkload(int iThreadIndex);

  /// \brief
  ///   Returns the average workload [0..1] of the main thread since the last update. Requires
  ///   SetGatherTaskInfo to be set, otherwise this function returns 0.0
  VBASE_IMPEXP float GetMainThreadWorkload();


  /// \brief
  ///   Called internally to update statistics
  VBASE_IMPEXP void UpdateWorkloadTick(float fTimeDifference);
  
  // Internal Methods:
  static void ExecuteTask(VThreadedTask *pTask);
  void Lock();
  void Unlock();
  inline void AddTask(VThreadedTask *pTask, int iPriority);
  void RemoveTask(VThreadedTask *pTask);
  void SignalIdle(VManagedThread *pThread);
  VThreadedTask *GetNextTask(VManagedThread *pThread, bool bReverse);

  static VISION_THREADLOCAL_DECL(VManagedThread*, s_pCurrentThread);

  ///
  /// @}
  ///

private:
  friend class VManagedThread;

  unsigned int CreateManagedThread();
  unsigned int GetTotalNumTasks();

  static VManagedThread * volatile s_pNullThread;
  static VThreadedTask * volatile s_pNullTask;

  DynArray_cl<VManagedThread * volatile> m_pThreads;
  VThreadTaskList* m_taskLists;

  volatile int m_iFlags;
  volatile bool m_bGatherTaskInfo;
  volatile int m_iNumPriorities;
  volatile unsigned int m_iNumThreads;
  volatile unsigned int * volatile m_piNumTasks;
  volatile unsigned int m_iNumActualTasks;
  volatile unsigned int * volatile m_piFirstTask;
  volatile unsigned int m_iListCleanupFreq;
  volatile unsigned int m_iListCleanupCount;
  volatile unsigned int m_iTasksExecutedInMainThread;
  volatile unsigned int m_iTasksExecutedInMainThreadLastFrame;
  volatile float m_fMainThreadWaitTime;
  volatile float m_fMainThreadWorkload;

  void ResetTypeRecording(bool bResetTaskNames);
  void AddTaskToRecording(VThreadedTask *pTask);
  DynArray_cl<VType *> m_pMainThreadTaskType;
  DynArray_cl<VType *> m_pMainThreadTaskTypeLastFrame;
  DynArray_cl<int> m_iMainThreadTaskTypeExecuted;
  DynArray_cl<int> m_iMainThreadTaskTypeExecutedLastFrame;

  VMutex m_Mutex;

  volatile bool m_bIsLocked;

  volatile int m_iStackSizePerThread;
  volatile bool m_bIsWaiting;
  volatile bool m_bOrderReversed;

  bool m_bIsInitialized;
  
  static VThreadManager s_ThreadMan;
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
