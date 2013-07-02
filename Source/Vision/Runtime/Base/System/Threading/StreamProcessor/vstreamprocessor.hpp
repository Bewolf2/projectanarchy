/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vstreamprocessor.hpp

#ifndef DEFINE_VISAPISTREAMPROCESSOR
#define DEFINE_VISAPISTREAMPROCESSOR


#include <Vision/Runtime/Base/System/Threading/StreamProcessor/VDataStream.hpp>
#include <Vision/Runtime/Base/System/Threading/StreamProcessor/VStreamProcessingTask.hpp>

// Forward declarations
class VStreamProcessingWorkflow;

// Definitions
#define NUM_COMMAND_BUFFERS 3
#define NUM_JOB_CHAINS      5
#define JOB_LIST_LENGTH   128

#ifdef VSTREAMPROCESS_RUN_IN_THREADMANAGER

// Workflow task: Used for executing stream processing workflows using Vision's Thread Manager.
class VWorkflowTask : public VThreadedTask
{
public:
  VWorkflowTask(VStreamProcessingWorkflow *pWorkflow);
  virtual void Run(VManagedThread *pThread);
private:
  VStreamProcessingWorkflow *m_pWorkflow;
};

#endif

/// \brief
///   Class for a stream processing workflow.
/// 
/// A stream processing workflow contains a set of stream processing tasks. These tasks can either
/// be independent, or they can depend on each other's results. Tasks can't be submitted to the
/// VStreamProcessor directly, but always have to be part of a workflow.
/// 
/// In order to express task interdependencies, the AddSync() method can be used. It ensures that
/// all tasks inserted before the Sync Object will be finished before execution of the tasks after
/// the Sync Object starts.
/// 
/// The number of tasks in a single VStreamProcessingWorkflow is not limited; however, for
/// performance reasons, you should make sure that that the number of tasks per workflow does not
/// get out of hands (anything between 1 and 128 is typically fine).
class VStreamProcessingWorkflow
{
public:

  /// \brief
  ///   Adds a task to the workflow and returns the index of this task within the workflow.
  ///
  /// It is important to note that, while the index of a task inside the workflow remains constant, the pointers
  /// to task of the same index may differ after subsequent calls to AddTask(). The reason for this is that the
  /// workflow-specific task list may be resized depending on the initialization parameters of the workflow and the
  /// number of tasks added to the workflow. It is therefore recommended to either initialize the workflow to the
  /// final number of tasks (which is also going to reduce memory allocations), or to always obtain the task pointer
  /// via GetTask() each time the task pointer is required.
  ///
  /// \returns
  ///  Index of the task within the workflow.
  VBASE_IMPEXP int AddTask();

  /// \brief
  ///   Returns the task with the specified index in this workflow.
  VBASE_IMPEXP VStreamProcessingTask *GetTask(int iTaskIndex);

  /// \brief
  ///   Returns the number of tasks in this workflow.
  inline int GetTaskCount() const { return m_iNumTasks; }

  /// \brief
  ///   Waits until all tasks in this workflow have finished executing.
  VBASE_IMPEXP void WaitUntilFinished();

  /// \brief
  ///   Returns whether all tasks in this workflow have finished executing.
  VBASE_IMPEXP bool IsFinished();

  /// \brief
  ///   Resets the tasks in this workflow. The task count will be reset to 0.
  VBASE_IMPEXP void ResetTasks();

  /// \brief
  ///   Explicitly resets the status of all tasks in this workflow to "not finished". This is
  ///   implicitly done whenever the workflow is kicked off.
  VBASE_IMPEXP void ResetStatus();

  /// \brief
  ///   Adds a synchronisation object into the task list.
  /// 
  /// \note
  ///   This method can be used to express task interdependencies. It ensures that all tasks
  ///   inserted before the Sync Object will be finished before execution of the tasks after the
  ///   Sync Object starts.
  VBASE_IMPEXP void AddSync();

private:
  VStreamProcessingWorkflow(int iNumTasks, int iPriority);
  ~VStreamProcessingWorkflow();

  friend class VStreamProcessor;

  DynObjArray_cl<VStreamProcessingTask> m_Tasks;
  int m_iNumTasks;

#ifdef VSTREAMPROCESS_RUN_IN_SPURS
  DynArray_cl<uint64_t> m_Commands;
  volatile int m_iCommandCounter;
  volatile int m_iFinished;
  CellSpursJob64 m_FinishJob;
#else
  VWorkflowTask *m_pTask;
#endif
};





/// \brief
///   Stream Processor main class
/// 
/// The stream processor class contains a set of static methods exposing efficient stream
/// processing mechanisms.
/// 
/// It has functionality for creating, destroying and kicking off stream processing workloads
/// (VStreamProcessingWorkflow).
/// 
/// Each stream processing workload may contain an arbitrary number of stream processing tasks (do
/// not confuse stream processing tasks with the VThreadedTasks used by the Thread Manager,
/// though). When a workload is kicked off using the KickWorkflow method, it will be processed on
/// one or multiple processing cores. On Playstation 3, VStreamProcessor will use the SPUs to reach
/// highest performance.
/// 
/// When comparing the Stream Processor with the Thread Manager, stream processing workloads are
/// more limited in scope than threaded tasks (they typically rely on a set of input and output
/// streams over which they iterate), but they have the advantage that they can use exactly the
/// same code across all platforms, including the Playstation 3's SPUs.
/// 
/// The stream processor is optimized for low scheduling and synchronisation overhead.
/// 
/// On Playstation 3, the stream processor uses SPURS for controlling the SPUs. A custom SPURS
/// instance can be passed to the Initialize method; if NULL is passed, it will create the default
/// SPURS instance created by Vision's SpursHandler. In order to achieve the best performance,
/// VStreamProcessor uses up to five independent Job Chains.
/// 
/// On PC and Xbox360, VStreamProcessor internally uses the VThreadManager to schedule and execute
/// stream processing workflows.
class VStreamProcessor
{
public:
#ifdef VSTREAMPROCESS_RUN_IN_SPURS
  VBASE_IMPEXP static void Initialize(int iPriority = 1, int iNumJobChains = 5, CellSpurs *pSpurs = NULL);
#else

  /// \brief
  ///   Initializes the stream processor
  /// 
  ///  Has to be called before any stream processing workloads are created or kicked off.
  /// 
  /// \param iPriority
  ///   Priority value for stream processing workloads. Currently has to be 1.
  VBASE_IMPEXP static void Initialize(int iPriority = 1);

#endif

  /// \brief
  ///   Returns whether the stream processor is initialized.
  VBASE_IMPEXP static bool IsInitialized();

  /// \brief
  ///   Deinitializes the stream processor and cleans up all allocated data.
  VBASE_IMPEXP static void DeInitialize();

  /// \brief
  ///   If set to true (default), additional synchronisation objects are used to ensure thread
  ///   safety of the stream processor.
  VBASE_IMPEXP static void SetMultithreading(bool bStatus);

  /// \brief
  ///   Returns the value previously set with SetMultithreading.
  VBASE_IMPEXP static bool GetMultithreading();

  /// \brief
  ///   Kicks off a stream processing workflow.
  /// 
  /// Once kicked off, the passed stream processing workflow is guaranteed to finish within finite
  /// time.
  /// 
  /// All stream processing workloads have the same priority, so the order in which they are
  /// processed is not defined.
  /// 
  /// Note that, even though this method is usually relatively lightweight, it may block on some
  /// platforms due to limitations of the internal task ring buffer. Therefore, keeping the number
  /// of stream processing tasks to a manageable level is crucial for best performance.
  /// 
  /// \param pWorkflow
  ///   Workflow to kick off.
  /// 
  /// \return
  ///   bool: true if the workflow could successfully be kicked off.
  VBASE_IMPEXP static bool KickWorkflow(VStreamProcessingWorkflow *pWorkflow);

  /// \brief
  ///   Creates a new stream processing workflow.
  /// 
  /// Since creating a new stream processing workflow can be a relatively expensive operation, it
  /// is not recommended to frequently create and destroy workflow objects. If possible, reuse
  /// workflows across frames.
  /// 
  /// Once a stream processing workflow is not used any more, it should be destroyed with
  /// DestroyWorkflow.
  /// 
  /// \param iNumTasks
  ///   The number of tasks that are expected to be added to this workflow. Inserting the right
  ///   number of tasks here will save reallocations when tasks are added to the workflow.
  /// 
  /// \param iPriority
  ///   Currently ignored. Should be 0.
  /// 
  /// \return
  ///   VStreamProcessingWorkflow *: A new workflow object,
  VBASE_IMPEXP static VStreamProcessingWorkflow *CreateWorkflow(int iNumTasks = 0, int iPriority = 0);

  /// \brief
  ///   Destroys a stream processing workflow.
  /// 
  /// \param pWorkflow
  ///   Workflow to destroy.
  VBASE_IMPEXP static void DestroyWorkflow(VStreamProcessingWorkflow *pWorkflow);

private:

#ifdef VSTREAMPROCESS_RUN_IN_SPURS
  static uint64_t CreateFinishMarker(int *pFlag, CellSpursJob64 *pJob);
#endif

  friend class VStreamProcessingWorkflow;

  static void Lock();
  static void Unlock();

#ifdef VSTREAMPROCESS_RUN_IN_SPURS
  static DynArray_cl<CellSpursJobChain *> m_pJobChains;
  static CellSpurs *m_pSpurs;
  static uint64_t *m_CommandList[NUM_JOB_CHAINS][NUM_COMMAND_BUFFERS]; 
  static uint64_t **m_pNextPatchAddress;
  static VStreamProcessingWorkflow **m_pLastWorkflow;
  static int m_iCurrentJobChain;
  static int m_iCurrentCommandList[NUM_JOB_CHAINS];
  static int m_iCommandCounter[NUM_JOB_CHAINS][NUM_COMMAND_BUFFERS];
  static bool m_bMultithreading;
  static sys_lwmutex_t m_Mutex;
#else
  static int m_iPriority;
#endif

  static bool m_bIsInitialized;
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
