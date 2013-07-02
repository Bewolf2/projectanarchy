/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VThreadedTask.hpp

#ifndef VTHREADEDTASK_HPP_INCLUDED
#define VTHREADEDTASK_HPP_INCLUDED

class VManagedThread;


/// \brief
///   enum used to describe the state of a VThreadedTask
enum VTaskState
{
  TASKSTATE_UNASSIGNED,
  TASKSTATE_PENDING,
  TASKSTATE_EXECUTING,
  TASKSTATE_FINISHED
};

/// \brief
///   Task class for multi-threading
/// 
/// VThreadedTask is the base class for all tasks that can be scheduled for execution with the
/// VThreadManager.
/// 
/// VThreadedTask objects are automatically assigned to idle threads in the VThreadManager. There
/// is no hard limit  regarding the number of tasks that can be scheduled in the VThreadManager.
class VThreadedTask : public VTypedObject
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Task default constructor.
  VBASE_IMPEXP VThreadedTask();

  /// \brief
  ///   Task destructor.
  VBASE_IMPEXP virtual ~VThreadedTask();

  ///
  /// @}
  ///

  ///
  /// @name Manager Properties
  /// @{
  ///

  /// \brief
  ///   Returns the current state of the task
  /// 
  /// This method does not block and has low overhead.
  /// 
  /// \return The current state of the task as a VTaskState enum:
  ///   \li \b TASKSTATE_UNASSIGNED: Task has not been scheduled yet.
  ///   \li \b TASKSTATE_PENDING: Task has been scheduled for execution with the
  ///       VThreadManager::ScheduleTask method, but execution hasn't started yet.
  ///   \li \b TASKSTATE_EXECUTING: Task is currently being executed.
  ///   \li \b TASKSTATE_FINISHED: Task has finished executing.
  VBASE_IMPEXP VTaskState GetState() const
  {
    return m_eState;
  }

  /// \brief
  ///   Sets the thread mask of the task.
  /// 
  /// The thread mask can be used to specify which thread(s) this task is allowed to be executed
  /// in. For instance, it is typically desired to run certain tasks (networking, background
  /// loading) in low-priority threads, while other tasks (particles, physics, etc.) should run in
  /// high-priority threads.
  /// 
  /// The thread mask should only be set before scheduling the task. Changing the thread mask of an
  /// already scheduled task may result in the modification being ignored.
  /// 
  /// \param iMask
  ///   The task's thread bitmask.
  VBASE_IMPEXP void SetThreadMask(unsigned int iMask);

  /// \brief
  ///   Returns the thread mask of the task.
  /// 
  /// The thread mask can be used to specify which thread(s) this task is allowed to be executed
  /// in. For instance, it is typically desired to run certain tasks (networking, background
  /// loading) in low-priority threads, while other tasks (particles, physics, etc.) should run in
  /// high-priority threads.
  /// 
  /// \return
  ///   unsigned int: The task's thread bitmask.
  VBASE_IMPEXP unsigned int GetThreadMask() const;

  /// \brief
  ///   Pure virtual Run method of the task
  /// 
  /// This method has to be implemented in derived classes. It will be called by the thread this
  /// task is scheduled in. 
  /// 
  /// \param pThread
  ///   Pointer to the thread the task is executed in. May be NULL if the task is executed in the
  ///   main thread.
  VBASE_IMPEXP virtual void Run(VManagedThread *pThread) = 0;

  /// \brief
  ///   Overridable that  is called from the thread.
  ///
  /// This method is scheduled in just after the task has finished executing.
  /// 
  /// The default implementation does nothing.
  /// 
  /// \param pThread
  ///   Pointer to the thread the task is executed in. May be NULL if the task is executed in the
  ///   main thread.
  VBASE_IMPEXP virtual void OnFinished(VManagedThread *pThread);

  /// \brief
  ///   Overridable that is called by the engine to notify the task that the engine will call
  ///   WaitForAllThreads soon
  /// 
  /// Implement this function in case the Run function takes significantly longer than a game tick.
  /// 
  /// In such a case you can use this function to set a flag (volatile!) that aborts the worker
  /// loop inside the Run function.
  /// 
  /// In other cases it is not necessary to override this function as WaitForAllThreads waits until
  /// all pending and busy tasks have returned from their Run function.
  /// 
  /// The default implementation is empty.
  VBASE_IMPEXP virtual void OnCancel()
  {
  }


  V_DECLARE_DYNAMIC_DLLEXP(VThreadedTask, VBASE_IMPEXP)

  // Internal use only:
  void SetState(VTaskState state);
  void WaitUntilFinished();

protected:

  volatile unsigned int m_iMask;
  volatile VTaskState m_eState;

  // Linked list members for the thread manager
  friend class VThreadManager;
  friend class VThreadTaskList;

  VThreadedTask* m_pPrevious;
  VThreadedTask* m_pNext;
  int m_iPriority;

  ///
  /// @}
  ///

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
