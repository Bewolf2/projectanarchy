/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvStateSync.hpp

#ifndef HKV_STATE_SYNC_HPP_INCLUDED
#define HKV_STATE_SYNC_HPP_INCLUDED

ANALYSIS_IGNORE_ALL_START
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Memory/Allocator/LargeBlock/hkLargeBlockAllocator.h>
#include <Common/Base/Memory/System/FreeList/hkFreeListMemorySystem.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/DebugUtil/MultiThreadCheck/hkMultiThreadCheck.h>
ANALYSIS_IGNORE_ALL_END

class hkvStateSync
{
private:
  struct ModuleSyncInfo
  {
    hkMemoryInitUtil::SyncInfo m_baseSystemInfo; // mem, singletons etc
    hkCriticalSection* m_mtCheckSection; // the multithreading debug check 
    hkMonitorStream* m_monitors; // montor ptrs as monitors not a normal singleton
    hkStackTracer* m_mtCheckStackTracer; // the multithreading debug check 
    hkStackTracer::CallTree* m_mtCheckStackTree; // the multithreading debug check 
  };

  struct ThreadSyncInfo
  {
    hkMemoryRouter* m_memoryRouter;
    hkMonitorStream* m_monitors;
  };

  typedef void (HK_CALL *moduleThreadStartedFunc)(const ThreadSyncInfo&);
  typedef void (HK_CALL *moduleThreadFinishingFunc)();

  struct ModuleNotifyEntry
  {
    ModuleNotifyEntry(hkUlong moduleId, moduleThreadStartedFunc threadStartedHandler, 
      moduleThreadFinishingFunc threadFinishingHandler) :
      m_moduleId(moduleId), m_threadStartedHandler(threadStartedHandler), m_threadFinishingHandler(threadFinishingHandler)
    {
    }

    hkUlong m_moduleId;
    moduleThreadStartedFunc m_threadStartedHandler;
    moduleThreadFinishingFunc m_threadFinishingHandler;
  };


public:
  // Initializes the Havok base system in the hkBase that's linked into this module
  ASSETFRAMEWORK_IMPEXP static void initBaseSystem(hkErrorReportFunction errorReport);
  // Shuts down the Havok base system in the hkBase that's linked into this module
  ASSETFRAMEWORK_IMPEXP static void shutdownBaseSystem();
  // Returns whether the base system is initialized
  ASSETFRAMEWORK_IMPEXP static hkBool isBaseSystemInitialized();

  // Notifies the state synchronization that a new module got loaded and wants 
  // to use the already initialized base system.
  HK_FORCE_INLINE static void notifyModuleLoaded()
  {
    if (getMyModuleId() == getGlobalModuleId())
    {
      VASSERT_MSG(FALSE, "Trying to notify from the module that owns the master base system!");
      return;
    }
    if (!isBaseSystemInitialized())
    {
      VASSERT_MSG(FALSE, "Base system is not initialized!");
      return;
    }
    
    // Sync statics and singletons from the global module
    ModuleSyncInfo moduleInfo;
    getGlobalModuleSyncInfo(moduleInfo);
    setMyModuleSyncInfo(moduleInfo);

    // Register this module to receive thread notifications
    registerModuleForThreadNotifications(
      getMyModuleId(), setMyThreadSyncInfo, zeroMyThreadSyncInfo);
  }


  // Notifies the state synchronization that a module is no longer interested 
  // in using the base system.
  HK_FORCE_INLINE static void notifyModuleUnloading()
  {
    if (getMyModuleId() == getGlobalModuleId())
    {
      VASSERT_MSG(FALSE, "Trying to notify from the module that owns the master base system!");
      return;
    }

    unregisterModuleFromThreadNotifications(getMyModuleId());
  }
  
  // Notifies the state synchronization that a new thread got started. The state
  // synchronization takes care of initializing the memory system for the new thread,
  // and of communicating the thread-local state to other modules.
  HK_FORCE_INLINE static void notifyThreadStarted(const char* threadName)
  {
    // Initialize this thread for using the base system
    hkMemoryRouter* memoryRouter = ::new hkMemoryRouter;
    hkMemorySystem::getInstance().threadInit(*memoryRouter, threadName);
    hkBaseSystem::initThread(memoryRouter);
    hkReferencedObject::lockInit(hkReferencedObject::LOCK_MODE_AUTO);
    
    // Notify other modules, and allow them to sync to our initialization
    ThreadSyncInfo myThreadInfo;
    getMyThreadSyncInfo(myThreadInfo);
    notifyOtherModulesOfThreadStarted(getMyModuleId(), myThreadInfo);
  }
  
  // Notifies the state synchronization that a is going to end. The state
  // synchronization takes care of deinitializing the memory system of the thread,
  // and of clearing the corresponding thread-local state in other modules.
  HK_FORCE_INLINE static void notifyThreadFinishing()
  {
    // Allow other modules to release hold of our base system info
    notifyOtherModulesOfThreadFinishing(getMyModuleId());

    // Deinitialize the base system for this thread
    hkMemoryRouter* memoryRouter = hkMemoryRouter::getInstancePtr();
    VASSERT(memoryRouter != NULL);
    if (memoryRouter != NULL)
    {
      hkBaseSystem::quitThread();
      hkMemorySystem::getInstance().threadQuit(*memoryRouter);
      ::delete memoryRouter;
    }
  }


private:
  // Writes the static module state of this module to moduleInfo
  HK_FORCE_INLINE static void getMyModuleSyncInfo(ModuleSyncInfo& moduleInfo)
  {
    moduleInfo.m_baseSystemInfo.m_memoryRouter = hkMemoryRouter::getInstancePtr();
    moduleInfo.m_baseSystemInfo.m_singletonList = hkSingletonInitList;
    moduleInfo.m_baseSystemInfo.m_memorySystem = hkMemorySystem::getInstancePtr();
    moduleInfo.m_monitors = hkMonitorStream::getInstancePtr();
    moduleInfo.m_mtCheckSection = hkMultiThreadCheck::m_criticalSection;
    moduleInfo.m_mtCheckStackTracer = hkMultiThreadCheck::s_stackTracer;
    moduleInfo.m_mtCheckStackTree = hkMultiThreadCheck::s_stackTree;
  }

  // Writes the static module state of the module owning the base system
  // to moduleInfo
  ASSETFRAMEWORK_IMPEXP static void getGlobalModuleSyncInfo(ModuleSyncInfo& moduleInfo);

  // Initializes the static state of this module from moduleInfo
  HK_FORCE_INLINE static void setMyModuleSyncInfo(const ModuleSyncInfo& moduleInfo)
  {
    extern hkBool hkBaseSystemIsInitialized;
    extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance;
    if (!hkBaseSystemIsInitialized && (moduleInfo.m_baseSystemInfo.m_memoryRouter != NULL))
    {
      hkMemoryRouter::replaceInstance(moduleInfo.m_baseSystemInfo.m_memoryRouter);
      hkMemorySystem::replaceInstance(moduleInfo.m_baseSystemInfo.m_memorySystem);
      if (moduleInfo.m_baseSystemInfo.m_singletonList != NULL) 
      { 
        hkSingletonInitNode::populate(hkSingletonInitList, moduleInfo.m_baseSystemInfo.m_singletonList); 
      }
      hkReferencedObject::lockInit(hkReferencedObject::LOCK_MODE_AUTO);
      hkBaseSystem::initSingletons();
      HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, moduleInfo.m_monitors);
      HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::m_criticalSection = moduleInfo.m_mtCheckSection; )
      HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::s_stackTracer = moduleInfo.m_mtCheckStackTracer; )
      HK_ON_DEBUG_MULTI_THREADING( hkMultiThreadCheck::s_stackTree = moduleInfo.m_mtCheckStackTree; )
      hkBaseSystemIsInitialized = true;
    }
  }
  
  // Initializes the static state of the module owning the base system from moduleInfo
  ASSETFRAMEWORK_IMPEXP static void setGlobalModuleSyncInfo(const ModuleSyncInfo& moduleInfo);

  // Gets the thread-local state of the current thread in this module and writes it 
  // to threadInfo
  HK_FORCE_INLINE static void getMyThreadSyncInfo(ThreadSyncInfo& threadInfo)
  {
    threadInfo.m_memoryRouter = hkMemoryRouter::getInstancePtr();
    threadInfo.m_monitors = hkMonitorStream::getInstancePtr();
  }

  // Writes the information in threadInfo to the thread-local state of the current thread
  // in this module
  HK_FORCE_INLINE static void HK_CALL setMyThreadSyncInfo(const ThreadSyncInfo& threadInfo)
  {
    extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance;
    hkMemoryRouter::replaceInstance(threadInfo.m_memoryRouter);
    hkReferencedObject::lockInit(hkReferencedObject::LOCK_MODE_AUTO);
    HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, threadInfo.m_monitors);
  }

  // Clears the thread-local state of the current thread in this module
  HK_FORCE_INLINE static void HK_CALL zeroMyThreadSyncInfo()
  {
    extern HK_THREAD_LOCAL( hkMonitorStream* ) hkMonitorStream__m_instance;
    hkMemoryRouter::replaceInstance(HK_NULL);
    HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, HK_NULL);
  }

  // Gets an ID that uniquely identifies this module
  HK_FORCE_INLINE static hkUlong getMyModuleId()
  {
    return (hkUlong)&getMyModuleSyncInfo;
  }
  
  // Gets an ID that uniquely identifies the module owning the base system
  ASSETFRAMEWORK_IMPEXP static hkUlong getGlobalModuleId();

  // Registers the specified module to receive thread start/finishing notifications
  // via the passed callback functions
  ASSETFRAMEWORK_IMPEXP static void registerModuleForThreadNotifications(hkUlong moduleHandle,
    moduleThreadStartedFunc threadStartedHandler, moduleThreadFinishingFunc threadFinishingHandler);

  // Unregisters the specified module from receiving thread start/finishing notifications
  ASSETFRAMEWORK_IMPEXP static void unregisterModuleFromThreadNotifications(hkUlong moduleHandle);

  // Notifies all modules except the calling one (identified by moduleId) that a new
  // thread got started. This method must be called in the context of the thread being
  // started.
  ASSETFRAMEWORK_IMPEXP static void notifyOtherModulesOfThreadStarted(
    hkUlong moduleId, ThreadSyncInfo& threadInfo);

  // Notifies all modules except the calling one (identified by moduleId) that a 
  // thread is finishing. This method must be called from that thread.
  ASSETFRAMEWORK_IMPEXP static void notifyOtherModulesOfThreadFinishing(
    hkUlong moduleId);

private:
  static hkFreeListMemorySystem* s_memorySystem;
  static hkMallocAllocator* s_baseMalloc;
  static hkLargeBlockAllocator* s_largeBlockAllocator;
  static hkFreeListAllocator* s_freeListAllocator;

  static hkUint32 s_initCount;

  static hkArray<ModuleNotifyEntry>* s_moduleNotifications;
  static hkCriticalSection* s_threadProtect;
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
