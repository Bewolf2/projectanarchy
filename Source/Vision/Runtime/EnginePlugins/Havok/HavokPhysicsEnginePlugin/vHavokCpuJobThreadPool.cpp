/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>
#include <Common/Base/DebugUtil/DeterminismUtil/hkCheckDeterminismUtil.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#ifdef _VISION_PS3
#include <Common/Base/Monitor/Spu/hkSpuMonitorCache.h>
#endif
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCpuJobThreadPool.hpp>

static void* HK_CALL hkWorkerThreadFunc(void* v);

vHavokCpuJobThreadPoolCinfo::vHavokCpuJobThreadPoolCinfo()
:	m_numThreads(1)
,	m_havokStackSize( 4000000 )
,	m_timerBufferPerThreadAllocation(0)
{
	m_threadName = "HavokWorkerThread";
	m_stackSize = hkThread::HK_THREAD_DEFAULT_STACKSIZE;
}


vHavokCpuJobThreadPool::SharedThreadData::SharedThreadData()
:	m_workerThreadFinished( 0, vHavokCpuJobThreadPool::MAX_NUM_THREADS )
{
  m_OnWorkerThreadCreatedPtr = NULL;
  m_OnWorkerThreadFinishedPtr = NULL;
}

vHavokCpuJobThreadPool::WorkerThreadData::WorkerThreadData() : m_semaphore(0,1)
{
	m_monitorStreamBegin = HK_NULL;
	m_monitorStreamEnd = HK_NULL;
	m_killThread = false;
	m_threadId = -1;
	m_sharedThreadData = HK_NULL;
}

	/// Initialize multi-threading sharedThreadData and create threads.
vHavokCpuJobThreadPool::vHavokCpuJobThreadPool(const vHavokCpuJobThreadPoolCinfo& ci)
{
	m_isRunning = false;
	m_threadName = ci.m_threadName;
	m_stackSize = ci.m_stackSize;

	m_sharedThreadData.m_localHavokStackSize = ci.m_havokStackSize;
	m_sharedThreadData.m_timerBufferAllocation = ci.m_timerBufferPerThreadAllocation;

	int numThreads = ci.m_numThreads;
	if (numThreads >= MAX_NUM_THREADS)
	{
		HK_WARN( 0xf0defd23, "You requested 6 or more threads, this is not supported by vHavokCpuJobThreadPool" );
		numThreads = MAX_NUM_THREADS - 1;
	}

	m_sharedThreadData.m_numThreads = numThreads;

  m_sharedThreadData.m_OnWorkerThreadCreatedPtr = ci.m_OnWorkerThreadCreatedPtr;
  m_sharedThreadData.m_OnWorkerThreadFinishedPtr = ci.m_OnWorkerThreadFinishedPtr;

#if defined(HK_PLATFORM_XBOX360) 
	int numCores = 3;
	int numThreadsPerCore = 2;
#elif defined(HK_PLATFORM_WIN32)
	hkHardwareInfo info;
	hkGetHardwareInfo( info );
	int numCores = info.m_numThreads; //This reports actual cores - ignoring hyperthreading
	int numThreadsPerCore = 1; 
	numCores /= numThreadsPerCore;
#endif

	for (int i = 0; i < numThreads; i++ )
	{
		WorkerThreadData& data = m_workerThreads[i];
		data.m_sharedThreadData = &m_sharedThreadData;
		data.m_threadId = i + 1; // don't start with thread 0 (assume that is the calling thread)
		data.m_monitorStreamBegin = HK_NULL;
		data.m_monitorStreamEnd = HK_NULL;
		data.m_killThread = false;
		data.m_clearTimers = false;

#if defined(HK_PLATFORM_XBOX360) || defined(HK_PLATFORM_WIN32)
		if (ci.m_hardwareThreadIds.getSize() > 0)
		{
			HK_ASSERT2( 0x975fe134, ci.m_hardwareThreadIds.getSize() >= numThreads, "If you initialize hardware thread ids, you must give an ID to all threads");
			data.m_hardwareThreadId = ci.m_hardwareThreadIds[i];
		}
		else
		{
			//X360: { 2,4,1,3,5, 0, 2,4,.. }
			int procGroup = (data.m_threadId % numCores) * numThreadsPerCore;
			data.m_hardwareThreadId = procGroup + (numThreadsPerCore > 1? ((data.m_threadId / numCores) % numThreadsPerCore) : 0 );
		}
#endif

		data.m_thread.startThread( &hkWorkerThreadFunc, &data, m_threadName, m_stackSize );
	}
	hkReferencedObject::setLockMode( hkReferencedObject::LOCK_MODE_AUTO);
}

// Destroy threads and delete sharedThreadData.
vHavokCpuJobThreadPool::~vHavokCpuJobThreadPool()
{
	waitForCompletion();

	for (int i = 0; i < m_sharedThreadData.m_numThreads; i++)
	{
		WorkerThreadData& data = m_workerThreads[i];
		data.m_killThread = true;
		data.m_semaphore.release(); // sets the thread off to enable it to finish
	}

	for (int i = 0; i < m_sharedThreadData.m_numThreads; i++)
	{
		m_sharedThreadData.m_workerThreadFinished.acquire(); // wait for the N threads to actually end
	}
	hkReferencedObject::setLockMode( hkReferencedObject::LOCK_MODE_NONE);
}

void vHavokCpuJobThreadPool::processAllJobs( hkJobQueue* queue, hkJobType firstJobType_unused )
{
	m_sharedThreadData.m_jobQueue = queue;

	HK_ASSERT2(0xad56dd77, m_isRunning == false, "Calling vHavokCpuJobThreadPool::processJobs() for the second time, without having called vHavokCpuJobThreadPool::waitForCompletion().");

	m_isRunning = true;

	for (int i = m_sharedThreadData.m_numThreads - 1; i >=0; i--)
	{
		WorkerThreadData& data = m_workerThreads[i];
		data.m_semaphore.release();
	}
}

bool vHavokCpuJobThreadPool::isProcessing()
{
	return m_isRunning;
}

int vHavokCpuJobThreadPool::getNumThreads()
{
	return m_sharedThreadData.m_numThreads;
}


void vHavokCpuJobThreadPool::setNumThreads(int numThreads)
{
	if ( numThreads > MAX_NUM_THREADS )
	{
		numThreads = MAX_NUM_THREADS;
	}
	while( m_sharedThreadData.m_numThreads < numThreads )
	{
		addThread();
	}

	while( m_sharedThreadData.m_numThreads > numThreads )
	{
		removeThread();
	}
}

//
//	Cause threads to garbage collect their memory immediately before signaling completion.

void vHavokCpuJobThreadPool::gcThreadMemoryOnNextCompletion()
{
	m_sharedThreadData.m_gcThreadMemoryOnCompletion = true;
}

void vHavokCpuJobThreadPool::addThread()
{
	HK_ASSERT2(0xad67bd88, ! m_isRunning, "You can only add or remove working threads via calls from the master thread and not between processAllJobs() and waitForCompletion() calls. ");

#if defined(HK_PLATFORM_HAS_SPU)
	HK_ASSERT2(0xcede9735, m_sharedThreadData.m_numThreads < 2, "Only 2 PPU threads are supported on the PS3" );
#endif

	HK_ASSERT2(0xcede9734, m_sharedThreadData.m_numThreads < MAX_NUM_THREADS, "A maximum of 6 threads are supported." );
	WorkerThreadData& data = m_workerThreads[m_sharedThreadData.m_numThreads];
	data.m_sharedThreadData = &m_sharedThreadData;
	data.m_threadId = m_sharedThreadData.m_numThreads+1;
	data.m_monitorStreamBegin = HK_NULL;
	data.m_monitorStreamEnd = HK_NULL;
	data.m_killThread = false;
	data.m_clearTimers = false;

#if defined(HK_PLATFORM_WIN32) || defined(HK_PLATFORM_XBOX360)
	#if defined HK_PLATFORM_XBOX360
		int numCores = 3;
		int numThreadsPerCore = 2; 
    #elif defined(HK_PLATFORM_WINRT)
	  //XX	   GetSystemInfoEx(&sysInfo);
		int numCores = 4;
		int numThreadsPerCore = 1; 
	#else    
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
  
		int numCores = sysInfo.dwNumberOfProcessors;
		int numThreadsPerCore = 1; //XX Work out hyper threading (not just logical versus physical processors)
		numCores /= numThreadsPerCore;
	#endif
	
	//X360: { 2,4,1,3,5, 0, 2,4,.. }
	int procGroup = (data.m_threadId % numCores) * numThreadsPerCore;
	data.m_hardwareThreadId = procGroup + (numThreadsPerCore > 1? ((data.m_threadId / numCores) % numThreadsPerCore) : 0 );
#endif

	data.m_thread.startThread( &hkWorkerThreadFunc, &data, m_threadName, m_stackSize);
	m_sharedThreadData.m_numThreads++;
}


void vHavokCpuJobThreadPool::removeThread()
{
	HK_ASSERT2(0xad67bd89, !m_isRunning, "You can only add or remove working threads via calls from the master thread and not between processJobs() and waitForCompletion() calls. ");

	HK_ASSERT2(0xcede9735, m_sharedThreadData.m_numThreads > 0, "You cannot set a negative number of threads" );

	--m_sharedThreadData.m_numThreads;

	WorkerThreadData& data = m_workerThreads[m_sharedThreadData.m_numThreads];

	// Signal the thread to be killed, and release the thread
	data.m_killThread = true;
	data.m_semaphore.release();

	// Wait until the thread actually finishes
	m_sharedThreadData.m_workerThreadFinished.acquire();
	data.m_thread.joinThread();
}




void vHavokCpuJobThreadPool::waitForCompletion()
{
	// This function does nothing if waitForStepWorldFinished() is called before startStepWorld()
	if ( m_isRunning )
	{
		for (int i = 0; i < m_sharedThreadData.m_numThreads; ++i)
		{
			m_sharedThreadData.m_workerThreadFinished.acquire();
		}
		m_isRunning = false;
		m_sharedThreadData.m_gcThreadMemoryOnCompletion = false;
	}
}



void vHavokCpuJobThreadPool::appendTimerData(  hkArrayBase<hkTimerData>& timerDataOut, hkMemoryAllocator& alloc )
{
	for (int i = 0; i < m_sharedThreadData.m_numThreads; ++i)
	{
		hkTimerData& info = timerDataOut._expandOne(alloc);
		info.m_streamBegin = m_workerThreads[i].m_monitorStreamBegin;
		info.m_streamEnd = m_workerThreads[i].m_monitorStreamEnd;
	}
}


void vHavokCpuJobThreadPool::clearTimerData()
{
	for ( int i = 0; i < m_sharedThreadData.m_numThreads; ++i )
	{
		m_workerThreads[i].m_monitorStreamEnd = m_workerThreads[i].m_monitorStreamBegin;
		m_workerThreads[i].m_clearTimers = true;
	}
}


static void* HK_CALL hkWorkerThreadFunc(void *v)
{
	vHavokCpuJobThreadPool::WorkerThreadData& data = *static_cast<vHavokCpuJobThreadPool::WorkerThreadData*>(v);
	vHavokCpuJobThreadPool::SharedThreadData& sharedThreadData = *data.m_sharedThreadData;

	HK_THREAD_LOCAL_SET( hkThreadNumber, data.m_threadId);

#if defined(HK_COMPILER_HAS_INTRINSICS_IA32) && HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
	// Flush all denormal/subnormal numbers (2^-1074 to 2^-1022) to zero.
	// Typically operations on denormals are very slow, up to 100 times slower than normal numbers.
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif

#ifdef HK_PLATFORM_XBOX360
	XSetThreadProcessor(GetCurrentThread(), data.m_hardwareThreadId );
#elif defined(HK_PLATFORM_WIN32) && !defined(_VISION_WINRT)
	SetThreadIdealProcessor(GetCurrentThread(), data.m_hardwareThreadId);
	// Can use SetThreadAffityMask to be more force-full.
#endif

	hkMemoryRouter memoryRouter;
	hkMemorySystem::getInstance().threadInit( memoryRouter, "vHavokCpuJobThreadPool" );
	hkBaseSystem::initThread( &memoryRouter );
	//hkUint32 allLockVal = 0;
	//hkReferencedObject::initThread(&allLockVal);
    
	if (sharedThreadData.m_timerBufferAllocation > 0)
	{
		// Allocate a monitor stream for this thread - this  enables timers.
		hkMonitorStream::getInstance().resize(sharedThreadData.m_timerBufferAllocation);
	}
	data.m_monitorStreamBegin = hkMonitorStream::getInstance().getStart();
	data.m_monitorStreamEnd = hkMonitorStream::getInstance().getEnd();

	hkCheckDeterminismUtil::initThread();


	// VISION specific: Call any per thread callback
	if (sharedThreadData.m_OnWorkerThreadCreatedPtr)
	{
		sharedThreadData.m_CallbackProtect.enter();
		sharedThreadData.m_OnWorkerThreadCreatedPtr->TriggerCallbacks( /* data needed? */);
		sharedThreadData.m_CallbackProtect.leave();
	}
	// END  VISION specific

	// Wait for the main thread to release the worker thread
	data.m_semaphore.acquire();

	// The thread "main loop"
	while (data.m_killThread == false)
	{
		if (data.m_clearTimers)
		{
			hkMonitorStream::getInstance().reset();
			data.m_monitorStreamEnd = hkMonitorStream::getInstance().getEnd();
			data.m_clearTimers = false;
		}

		const bool isNotPrimary = false;
		hkCheckDeterminismUtil::workerThreadStartFrame(isNotPrimary);

		// Enable timers for critical sections just during the step call
		hkCriticalSection::setTimersEnabled();

		sharedThreadData.m_jobQueue->processAllJobs();

		// Disable timers for critical sections just during the step call
		hkCriticalSection::setTimersDisabled();

		// Note collected timer data
		hkMonitorStream& stream = hkMonitorStream::getInstance();
		data.m_monitorStreamEnd = stream.getEnd();

		hkCheckDeterminismUtil::workerThreadFinishFrame();

		if( sharedThreadData.m_gcThreadMemoryOnCompletion )
		{
			hkMemorySystem::getInstance().garbageCollectThread( memoryRouter );
		}

		// Release any thread (usually the main thread) which may be waiting for all worker threads to finish.
		sharedThreadData.m_workerThreadFinished.release();

		// Immediately wait until the main thread releases the thread again
		data.m_semaphore.acquire();
	}


	// Perform cleanup operations
	
	// VISION specific: Call any per thread callback
	if (sharedThreadData.m_OnWorkerThreadFinishedPtr)
	{
		sharedThreadData.m_CallbackProtect.enter();
		sharedThreadData.m_OnWorkerThreadFinishedPtr->TriggerCallbacks( /* data needed? */);
		sharedThreadData.m_CallbackProtect.leave();
	}
	// END  VISION specific
	
	hkCheckDeterminismUtil::quitThread();

	hkBaseSystem::quitThread();
	hkMemorySystem::getInstance().threadQuit( memoryRouter );

	sharedThreadData.m_workerThreadFinished.release();

	return 0;
}

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
