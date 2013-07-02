/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VStreamProcessingTask.hpp

#ifndef DEFINE_VISAPISTREAMPROCESSINGTASK
#define DEFINE_VISAPISTREAMPROCESSINGTASK


#ifdef _VISION_PS3
  #ifdef SPU
    #ifdef SPU
      #include <Vision/Runtime/Base/System/VSpuDefs.hpp>
    #endif

    #include <cell/spurs/job_chain.h>
    #include <cell/spurs/event_flag.h>
    #include <cell/atomic.h>
    #include <cell/dma.h>
    #include <spu_printf.h>

    #define VSTREAMPROCESS_RUN_IN_SPURS
    #define VBASE_IMPEXP

  #else

    #include <sys/spu_initialize.h>
    #include <sys/spu_thread.h>
    #include <sys/ppu_thread.h>
    #include <sys/process.h>
    #include <cell/spurs.h>
    #include <cell/atomic.h>

  #endif

  #define VSTREAMPROCESSOR_JOBTYPE CellSpursJob64

#endif


// Definitions
#define VSTREAMTASK_STATUS_IDLE                 0
#define VSTREAMTASK_STATUS_PROCESSING           1
#define VSTREAMTASK_STATUS_FINISHED             2

#define VSTREAMPROCESSOR_TASK_MAX_OUT_STREAMS   4
#define VSTREAMPROCESSOR_TASK_MAX_IN_STREAMS    8

#if defined(VSTREAMPROCESS_RUN_IN_SPURS) && !defined(SPU)
  #include <Vision/Runtime/Base/System/Threading/SpursHandler/VSpursHandler.hpp>
#endif

#include <Vision/Runtime/Base/System/Threading/StreamProcessor/VDataStream.hpp>

class VStreamProcessingTask;


#ifdef VSTREAMPROCESS_RUN_IN_SPURS
  
class VStreamProcessingKernel
{
public:
  void Execute(VStreamProcessingTask *pTask);
};

#else

/// \brief
///   Base class for stream processing kernels. All stream processing kernel implementations have to be
///   derived from this class.
class VStreamProcessingKernel 
{
public:

  VStreamProcessingKernel()
  {
  }

  virtual ~VStreamProcessingKernel()
  {
  }

  /// \brief
  ///   Overridable function for processing one task. Subclasses need to implement this
  ///   function.
  virtual void Execute(VStreamProcessingTask *pTask) = 0;
};

#endif


/// \brief
///   Class for a stream processing task.
/// 
/// Stream processing tasks are part of stream processing workflows. They can be kicked off and
/// executed by the VStreamProcessor class as part of a workflow.
/// 
/// Each Stream Processing Task has up to 8 input and up to 4 output streams. These streams are of
/// the type VDataStream and may point to an arbitrary type of data on the heap. See the
/// task/iterator macros for further details on this.
/// 
/// VStreamProcessingTask can be accessed by both the code which kicks off workflows and by the
/// task code itself. On Playstation 3, VStreamProcessingTasks exist on both the PPU and the SPUs,
/// though some of its member functions only make sense on one type of execution unit.
class VStreamProcessingTask
{
public:
  VBASE_IMPEXP VStreamProcessingTask();

  ///
  /// @name Methods To Be Called From the Ppu/handler Thread Side
  /// @{
  ///

  /// \brief
  ///   Adds an input stream definition to the task.
  /// 
  /// The input stream definition is copied into the task, so the original stream definition may be
  /// discarded.
  /// 
  /// A maximum of 8 input stream may be defined.
  /// 
  /// Note that streams may only reference data on the heap, not on the stack!
  /// 
  /// \param dataStream
  ///   The data stream definition.
  VBASE_IMPEXP void AddInStream(VDataStream &dataStream);

  /// \brief
  ///   Adds an output stream definition to the task.
  /// 
  /// The output stream definition is copied into the task, so the original stream definition may
  /// be discarded.
  /// 
  /// A maximum of 4 output stream may be defined.
  /// 
  /// Note that streams may only reference data on the heap, not on the stack!
  /// 
  /// \param dataStream
  ///   The data stream definition.
  VBASE_IMPEXP void AddOutStream(VDataStream &dataStream);

  /// \brief
  ///   Resets the input and output stream definitions. The number of input and output streams are
  ///   reset to 0.
  VBASE_IMPEXP void ResetStreams();

  /// \brief
  ///   Resets the status of this task to VSTREAMTASK_STATUS_IDLE.
  VBASE_IMPEXP void ResetStatus();

  /// \brief
  ///   Waits until this task has finished executing. Note that even after this method returns,
  ///   output DMA operations may still be pending. Use
  ///   VStreamProcessingWorkflow::WaitUntilFinished instead if you need conservative feedback!
  VBASE_IMPEXP void WaitUntilFinished();

  /// \brief
  ///   Returns whether this task has finished executing. Note that this function may return true
  ///   even when output DMA operations may still be pending. Use
  ///   VStreamProcessingWorkflow::IsFinished instead if you need conservative feedback!
  inline bool IsFinished() const { return m_iStatus == VSTREAMTASK_STATUS_FINISHED; }


  #if defined(VSTREAMPROCESS_RUN_IN_SPURS) && !defined(SPU)
  
  /// \brief
  ///   Sets the kernel for this stream processing task (PS3 SPU version).
  /// 
  /// The passed kernel has to follow the specifications for stream processing kernels according to
  /// the Vision Programmer's documentation.
  /// 
  /// \param pKernel
  ///   Kernel (a SPURS Job 2.0 binary) for this task.
  VBASE_IMPEXP void SetKernel(VSpuBinary *pKernel);

  #elif defined(VSTREAMPROCESS_RUN_IN_THREADMANAGER)

  /// \brief
  ///   Sets the kernel for this stream processing task (PC and Xbox360 version).
  /// 
  /// The passed kernel has to be derived from the VStreamProcessingKernel class and has to follow
  /// the specifications for stream processing kernels according to the Vision Programmer's
  /// documentation.  
  /// 
  /// \param pKernel
  ///   Kernel for this task.
  VBASE_IMPEXP void SetKernel(VStreamProcessingKernel *pKernel);

  /// \brief
  ///   Returns the kernel previously specified with SetKernel.
  inline VStreamProcessingKernel *GetKernel() const { return m_pKernel; }

  #endif
  
  /// \brief
  ///   Sets an arbitrary user data pointer.
  inline void SetUserData(void *pData) { m_pUserData = pData; }

  /// \brief
  ///   Returns the user data previously specified with SetUserData.
  inline void *GetUserData() const { return m_pUserData; }


  ///
  /// @}
  ///

  ///
  /// @name Methods To Be Called From the Ppu/handler Thread Side, Playstation 3 Only
  /// @{
  ///

  /// \brief
  ///   Sets the size in bytes of the scratch buffer for the stream processing kernel (which is a
  ///   SPURS Job).
  VBASE_IMPEXP void SetScratchBufferSize(int iScratchBufferSize);

  /// \brief
  ///   Sets the size in bytes of the output buffer for the stream processing kernel (which is a
  ///   SPURS Job).
  VBASE_IMPEXP void SetOutputBufferSize(int iOutputBufferSize);

  /// \brief
  ///   Sets the size in bytes of the output buffer for the stream processing kernel (which is a
  ///   SPURS Job).
  VBASE_IMPEXP void SetStackSize(int iStackSize);

  
  ///
  /// @}
  ///

  ///
  /// @name Methods To Be Called From the Task Kernel
  /// @{
  ///

  /// \brief
  ///   Returns a pointer to the input stream definition with the specified index.
  inline VDataStream *GetInStream(int iStreamIndex) { VASSERT(iStreamIndex<m_iNumInStreams); return &m_InStreams[iStreamIndex]; }

  /// \brief
  ///   Returns a pointer to the output stream definition with the specified index.
  inline VDataStream *GetOutStream(int iStreamIndex) { VASSERT(iStreamIndex<m_iNumOutStreams); return &m_OutStreams[iStreamIndex]; }

  /// \brief
  ///   Returns the number of input streams in this task.
  inline int GetInStreamCount() { return m_iNumInStreams; }

  /// \brief
  ///   Returns the number of output streams in this task.
  inline int GetOutStreamCount() { return m_iNumOutStreams; }

  /// \brief
  ///   Internally used to signify that this task's execution has finished.
  VBASE_IMPEXP void SetFinished();


  #ifdef VSTREAMPROCESS_RUN_IN_SPURS

  /// \brief
  ///   Internally used to set up final pointers to task object etc. within the SPURS Job (PS3
  ///   only)
  VBASE_IMPEXP void Prepare();

  #endif

  ///
  /// @}
  ///

private:
  friend class VStreamProcessingWorkflow;
  friend class VStreamProcessor;

  int   m_iNumInStreams;
  int   m_iNumOutStreams;
  
  volatile unsigned int m_iStatus;

  VDataStream m_InStreams[VSTREAMPROCESSOR_TASK_MAX_IN_STREAMS];
  VDataStream m_OutStreams[VSTREAMPROCESSOR_TASK_MAX_OUT_STREAMS];

#if defined(VSTREAMPROCESS_RUN_IN_SPURS) || defined(SPU)
  VSTREAMPROCESSOR_JOBTYPE m_SpursJob;
#else
  VStreamProcessingKernel *m_pKernel;
#endif

  void *m_pUserData;
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
