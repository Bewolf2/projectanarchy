/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvBackgroundProcessor.hpp

#ifndef HKV_BACKGROUND_PROCESSOR_HPP_INCLUDED
#define HKV_BACKGROUND_PROCESSOR_HPP_INCLUDED

#include <Common/Base/Container/Array/hkArray.h>
#include <Common/Base/Container/BitField/hkBitField.h>
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>
#include <Common/Base/Thread/Semaphore/hkSemaphore.h>
#include <Common/Base/Thread/Thread/hkThread.h>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvFifoSet.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvStateSync.hpp>

#include <algorithm>
#include <deque>

#define DBGMSG(msg, ...)
//#define DBGMSG(msg, ...) { hkStringBuf dbgBuf; dbgBuf.printf(msg, __VA_ARGS__); dbgBuf.append("\r\n"); OutputDebugStringA(dbgBuf.cString()); }

/// \class hkvCrossThreadWork
/// \brief Helper to allow background operations to wait for some work to be done in
///        a different thread, such as (usually) the main thread.
class hkvCrossThreadWork
{
public:
  hkvCrossThreadWork();
private:
  hkvCrossThreadWork(const hkvCrossThreadWork&);
  hkvCrossThreadWork& operator=(const hkvCrossThreadWork&);
public:
  virtual ~hkvCrossThreadWork();

public:
  void cancel();
  void work();
  void wait();

protected:
  virtual void doWork() = 0;

private:
  hkCriticalSection m_stateProtect;
  hkSemaphore m_notify;
  bool m_canceled;
  bool m_busy;
  bool m_finished;
};


enum hkvBackgroundProcessingResult
{
  HKV_BACKGROUND_RESULT_SUCCESS,
  HKV_BACKGROUND_RESULT_FAILURE,
  HKV_BACKGROUND_RESULT_NOT_READY,
  HKV_BACKGROUND_RESULT_SHOULD_RETRY
};


/// \class hkvBackgroundProcessor
/// \brief Template class to perform background processing tasks in multiple threads.
///
/// Each task is defined by an input and output struct. Concrete specializations of this template
/// class need to define a process method which uses the information of the input struct to
/// perform the required task. Completed outputs can be fetched any time by calling getOutputs().
template <typename Input, typename Output>
class hkvBackgroundProcessor
{
private:
  struct InputEntry
  {
    InputEntry()
    : m_retryCount(0)
    {
    }
    
    explicit InputEntry(const Input& input)
    : m_input(input), m_retryCount(0)
    {
    }

    InputEntry(const Input& input, const hkFileSystem::TimeStamp& lastAttempt, hkUint32 retryCount = 0)
    : m_input(input), m_lastAttempt(lastAttempt), m_retryCount(retryCount)
    {
    }

    bool operator==(const InputEntry& other) const
    {
      return m_input == other.m_input;
    }

    bool operator==(const Input& other) const
    {
      return m_input == other;
    }

    bool operator<(const InputEntry& rhs) const
    {
      return m_input < rhs.m_input;
    }

    Input m_input;
    hkFileSystem::TimeStamp m_lastAttempt;
    hkUint32 m_retryCount;
  };


public:
  hkvBackgroundProcessor(hkUint32 numThreads = 1, bool discardOutput = false);
private:
  hkvBackgroundProcessor(const hkvBackgroundProcessor&);
  hkvBackgroundProcessor& operator=(const hkvBackgroundProcessor&);
public:
  virtual ~hkvBackgroundProcessor();

public:
  hkUint32 getNumThreads() const;

  void setRetryBehavior(hkUint32 maxRetries, hkUint32 waitMs = 0);

  void start();
  void stop(bool keepCurrentWork);

  bool isRunning() const;
  bool isStopping() const;

  void addInput(const Input& input);
  void getOutputs(hkArray<Output>& out_outputs);

  void queryProgressInformation(hkUint32& out_maxInputs, hkUint32& out_remainingInputs) const;

  hkvCrossThreadWork* getCrossThreadWork();

  void clearAll();
  void clearInputs();
  void clearOutputs();

protected:
  virtual void beforeStart();
  virtual void onStopping();
  virtual void afterStop();

  hkvCriticalSectionLock acquireLock();
  void addCrossThreadWork(hkvCrossThreadWork* work);

  hkUlong getThreadNum() const;

  virtual hkvBackgroundProcessingResult process(const Input& input, Output& output) = 0;

private:
  hkUlong allocateThreadNum();

  static void* HK_CALL staticThreadFunc(void* data);
  void threadFunc();

  void cancelCrossThreadWork();
  void handleRetryWait(const InputEntry& input);
  bool handleRetryResult(const InputEntry& input, hkvBackgroundProcessingResult lastResult);

private:
  bool m_discardOutput;

  hkUint32 m_maxRetries;
  hkUint32 m_retryWaitMs;

  mutable hkCriticalSection m_mtProtect;
  hkUint32 m_numThreads;
  hkUint32 m_numActiveThreads;
  hkThread** m_threads;
  HK_THREAD_LOCAL(hkUlong) m_threadNum;
  hkBitField m_threadNumUsage;
  hkSemaphore* m_wakeUpEvents;
  hkSemaphore* m_threadsRunning;

  hkvFifoSet<InputEntry> m_inputs;
  hkUint32 m_maxNumInputs;
  hkArray<Output> m_outputs;
  hkArray<hkvCrossThreadWork*> m_crossThreadWork;

  volatile bool m_shouldStop;
};


template <typename Input, typename Output>
hkvBackgroundProcessor<Input, Output>::hkvBackgroundProcessor(
  hkUint32 numThreads/* = 1*/, bool discardOutput/* = false*/)
: m_discardOutput(discardOutput), m_numActiveThreads(0), m_maxNumInputs(0),
  m_maxRetries(0), m_retryWaitMs(1000)
{
  VASSERT_MSG(numThreads > 0, "Number of threads must not be 0; setting it to 1.");
  m_numThreads = (numThreads < 1) ? 1 : numThreads;
  m_threads = new hkThread*[m_numThreads];
  memset(m_threads, 0, m_numThreads * sizeof(hkThread*));
  m_threadNumUsage.setSize(m_numThreads, 0);
  HK_THREAD_LOCAL_SET(m_threadNum, (hkUlong)(-1));

  m_wakeUpEvents = new hkSemaphore(0, m_numThreads);
  m_threadsRunning = new hkSemaphore(m_numThreads, m_numThreads);
}


template <typename Input, typename Output>
hkvBackgroundProcessor<Input, Output>::~hkvBackgroundProcessor()
{
  stop(false);
  
  VASSERT_MSG(m_crossThreadWork.isEmpty(), "Not all cross-thread work has been cleared!");
  
  delete[] m_threads;
  delete m_wakeUpEvents;
  delete m_threadsRunning;
}


template <typename Input, typename Output>
hkUint32 hkvBackgroundProcessor<Input, Output>::getNumThreads() const
{
  return m_numThreads;
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::setRetryBehavior(hkUint32 maxRetries, hkUint32 waitMs)
{
  m_maxRetries = maxRetries;
  m_retryWaitMs = waitMs;
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::start()
{
  {
    hkCriticalSectionLock lock(&m_mtProtect);

    // Check if already running
    if (isRunning())
    {
      return;
    }

    beforeStart();

    m_shouldStop = false;

    for (hkUint32 i = 0; i < m_numThreads; ++i)
    {
      m_threads[i] = new hkThread();
      m_threads[i]->startThread(staticThreadFunc, this, "hkvBackgroundProcessor");
    }

    m_wakeUpEvents->release((int)m_inputs.GetElementCount());
  }

  // Wait until all threads are really running. We can check this by waiting until each thread
  // has allocated its thread number. This ensures that all semaphore tokens in m_threadsRunning
  // have been acquired by the worker threads, and that acquiring them when we're stopping is a
  // reliable indicator that all thread are really finished.
  while (true)
  {
    hkUint32 numThreadStarted;
    {
      hkCriticalSectionLock lock(&m_mtProtect);
      numThreadStarted = (hkUint32)m_threadNumUsage.bitCount();
    }
    
    if (numThreadStarted >= m_numThreads)
    {
      break;
    }

    Sleep(10);
  }
}


template <typename Input, typename Output>
hkvCrossThreadWork* hkvBackgroundProcessor<Input, Output>::getCrossThreadWork()
{
  hkCriticalSectionLock lock(&m_mtProtect);
  if (m_crossThreadWork.isEmpty())
  {
    return NULL;
  }

  hkvCrossThreadWork* work = m_crossThreadWork.back();
  m_crossThreadWork.popBack();

  return work;
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::stop(bool keepCurrentWork)
{
  {
    hkCriticalSectionLock lock(&m_mtProtect);

    // Check if already stopped
    if (!isRunning())
    {
      return;
    }

    // Set the flag to ask threads to stop, then wake up any sleeping
    // threads so they can do exactly that.
    m_shouldStop = true;
    m_wakeUpEvents->release(m_numThreads);

    // Cancel all cross-thread work threads may be waiting for
    cancelCrossThreadWork();
  }

  // Allow subclasses to perform work such as notifying their worker to quit
  onStopping();

  // Wait for each thread to release its "I'm running" semaphore token
  for (hkUint32 i = 0; i < m_numThreads; ++i)
  {
    m_threadsRunning->acquire();
  }

  // Free the threads, and release the semaphore tokens again
  {
    hkCriticalSectionLock lock(&m_mtProtect);

    m_threadNumUsage.assignAll(0);

    for (hkUint32 i = 0; i < m_numThreads; ++i)
    {
      delete m_threads[i];
      m_threads[i] = NULL;
    }

    m_threadsRunning->release(m_numThreads);

    if (!keepCurrentWork)
    {
      m_inputs.Clear();
    }

    afterStop();

    m_threadNumUsage.assignAll(0);
  }
}


template <typename Input, typename Output>
bool hkvBackgroundProcessor<Input, Output>::isRunning() const
{
  return (m_threads[0] != NULL);
}


template <typename Input, typename Output>
bool hkvBackgroundProcessor<Input, Output>::isStopping() const
{
  return m_shouldStop;
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::addInput(const Input& input)
{
  hkCriticalSectionLock lock(&m_mtProtect);
  
  if (!m_inputs.Insert(InputEntry(input)))
  {
    return;
  }

  ++m_maxNumInputs;
  m_wakeUpEvents->release();
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::getOutputs(hkArray<Output>& out_outputs)
{
  hkCriticalSectionLock lock(&m_mtProtect);
  if (!m_outputs.isEmpty())
  {
    out_outputs.append(m_outputs);
    m_outputs.clear();
  }
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::queryProgressInformation(
  hkUint32& out_maxInputs, hkUint32& out_remainingInputs) const
{
  hkCriticalSectionLock lock(&m_mtProtect);
  out_remainingInputs = (hkUint32)m_inputs.GetElementCount() + m_numActiveThreads;
  out_maxInputs = hkMath::max2(m_maxNumInputs, out_remainingInputs);
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::clearAll()
{
  clearInputs();
  clearOutputs();
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::clearInputs()
{
  hkCriticalSectionLock lock(&m_mtProtect);
  m_inputs.Clear();
  if (m_numActiveThreads == 0)
  {
    m_maxNumInputs = 0;
  }
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::clearOutputs()
{
  hkCriticalSectionLock lock(&m_mtProtect);
  m_outputs.clear();
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::beforeStart()
{
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::onStopping()
{
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::afterStop()
{
}


template <typename Input, typename Output>
hkvCriticalSectionLock hkvBackgroundProcessor<Input, Output>::acquireLock()
{
  return hkvCriticalSectionLock(m_mtProtect);
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::addCrossThreadWork(hkvCrossThreadWork* work)
{
  hkCriticalSectionLock lock(&m_mtProtect);
  if (m_shouldStop)
  {
    work->cancel();
  }
  else
  {
    m_crossThreadWork.pushBack(work);
  }
}


template <typename Input, typename Output>
hkUlong hkvBackgroundProcessor<Input, Output>::getThreadNum() const
{
  return HK_THREAD_LOCAL_GET(m_threadNum);
}


template <typename Input, typename Output>
hkUlong hkvBackgroundProcessor<Input, Output>::allocateThreadNum()
{
  hkCriticalSectionLock lock(&m_mtProtect);
  for (hkUint32 bitIdx = 0; bitIdx < m_numThreads; ++bitIdx)
  {
    if (!m_threadNumUsage.get(bitIdx))
    {
      m_threadNumUsage.set(bitIdx);
      return bitIdx;
    }
  }

  VASSERT_MSG(FALSE, "Expected to find unused thread number, but didn't!");
  return (hkUlong)(-1);
}


template <typename Input, typename Output>
void* HK_CALL hkvBackgroundProcessor<Input, Output>::staticThreadFunc(void* data)
{
  static_cast<hkvBackgroundProcessor*>(data)->threadFunc();
  return HK_NULL;
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::threadFunc()
{
  m_threadsRunning->acquire();
  hkvStateSync::notifyThreadStarted("hkvBackgroundProcessor");

  HK_THREAD_LOCAL_SET(m_threadNum, allocateThreadNum());

  while (!m_shouldStop)
  {
    m_wakeUpEvents->acquire();
    
    // Once signaled, run as long as there are inputs available.
    while (true)
    {
      if (m_shouldStop)
      {
        break;
      }

      // Lock scope for input acquisition
      InputEntry input;
      {
        hkCriticalSectionLock lock(&m_mtProtect);
        if (m_inputs.IsEmpty())
        {
          break;
        }
        input = m_inputs.GetFrontElement();
        m_inputs.RemoveFrontElement();
        ++m_numActiveThreads;
      }

      handleRetryWait(input);

      Output output;
      hkvBackgroundProcessingResult result = process(input.m_input, output);

      // Lock scope for output handling
      {
        hkCriticalSectionLock lock(&m_mtProtect);
      
        // See if we should/can queue a retry of this operation. We always schedule a retry if the input was not
        // yet ready. Otherwise, we check against the retry limit.
        bool shouldRetry = (result == HKV_BACKGROUND_RESULT_NOT_READY) || (result == HKV_BACKGROUND_RESULT_SHOULD_RETRY);
        bool retrying = false;
        if (shouldRetry && !m_shouldStop)
        {
          retrying = handleRetryResult(input, result);
        }

        // If we're not retrying, publish the output
        if (!retrying && !m_discardOutput)
        {
          m_outputs.pushBack(output);
        }

        // This thread is finished... for now!
        --m_numActiveThreads;

        // Check if we're finished. If yes, reset the maximum input counter
        if (m_inputs.IsEmpty() && (m_numActiveThreads == 0))
        {
          m_maxNumInputs = 0;
        }
      }
    }
  }

  hkvStateSync::notifyThreadFinishing();
  m_threadsRunning->release();
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::cancelCrossThreadWork()
{
  const int numCrossThreadWork = m_crossThreadWork.getSize();
  for (int crossThreadWorkIdx = 0; crossThreadWorkIdx < numCrossThreadWork; ++crossThreadWorkIdx)
  {
    m_crossThreadWork[crossThreadWorkIdx]->cancel();
  }
  m_crossThreadWork.clear();
}


template <typename Input, typename Output>
void hkvBackgroundProcessor<Input, Output>::handleRetryWait(const InputEntry& input)
{
  if (input.m_lastAttempt.isValid())
  {
    hkInt64 msElapsed = 
      (hkvTimeHelper::getCurrentTime().get() - input.m_lastAttempt.get()) / 1000000;
    hkInt32 msToWait = (hkInt32)(m_retryWaitMs - msElapsed);
    if (msToWait > 0)
    {
      DBGMSG("handleRetryWait(%d)", msToWait);
      Sleep(msToWait);
    }
  }
}


template <typename Input, typename Output>
bool hkvBackgroundProcessor<Input, Output>::handleRetryResult(const InputEntry& input, hkvBackgroundProcessingResult lastResult)
{
  if (lastResult == HKV_BACKGROUND_RESULT_SHOULD_RETRY && input.m_retryCount >= m_maxRetries)
  {
    return false;
  }

  hkUint32 newRetryCount = (lastResult == HKV_BACKGROUND_RESULT_SHOULD_RETRY) ? input.m_retryCount + 1 : 0;

  // Caution: assumes lock to be in place!
  InputEntry newInput(input.m_input, hkvTimeHelper::getCurrentTime(), newRetryCount);

  if (!m_inputs.Insert(newInput))
  {
    return false;
  }

  m_wakeUpEvents->release();

  return true;
}


#undef DBGMSG

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
