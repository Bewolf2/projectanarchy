/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMutex.hpp

#ifndef VMUTEX_HPP_INCLUDED
#define VMUTEX_HPP_INCLUDED


#ifdef _VISION_PS3
  #include <sys/ppu_thread.h>
  #include <sys/synchronization.h>
#endif


#ifdef _VISION_POSIX
  #include <pthread.h>
  #include <semaphore.h>
#endif

/// \brief
///   Lightweight synchronization object for mutual-exclusive code execution. Wraps around a "critical
///   section" object on Windows platforms.
class VMutex
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor; initializes the mutex.
  inline VMutex();

  /// \brief
  ///   Destructor; deinitializes the mutex.
  inline  ~VMutex();

  ///
  /// @}
  ///

  ///
  /// @name Lock Mutex
  /// @{
  ///

  /// \brief
  ///   Locks the mutex, i.e. resides in the function until the calling thread grants ownership.
  ///   Wraps around EnterCriticalSection on windows platforms.
  inline void Lock();

  /// \brief
  ///   Unlocks the mutex, i.e. releases ownership for this thread. Wraps around
  ///   LeaveCriticalSection on windows platforms.
  inline void Unlock();

  ///
  /// @}
  ///

private:
  // platform specific members
  #if defined(WIN32)  || defined (_VISION_XENON) 
    CRITICAL_SECTION m_CriticalSection;
  
  #elif defined(_VISION_PS3)
    sys_lwmutex_t m_Mutex;
  
  #elif defined(_VISION_POSIX)
    pthread_mutex_t m_Mutex;

  #elif defined(_VISION_PSP2)
    SceUID m_Mutex;
    SceKernelLwMutexWork m_WorkArea;

  #elif defined(_VISION_WIIU)
    OSMutex m_Mutex;

  #else
    #error Undefined platform!

  #endif
};

/// \brief
/// Simple RAII class to automatically unlock a mutex when a code block is exited
class VMutexLocker
{
public:
  //For backwards compatibility only
  VMutexLocker(VMutex* pMutex) : m_Mutex(*pMutex)
  {
    VASSERT(pMutex != NULL);
    m_Mutex.Lock ();
  }

  VMutexLocker(VMutex& mutex) : m_Mutex(mutex)
  {
    m_Mutex.Lock();
  }

  ~VMutexLocker ()
  {
    m_Mutex.Unlock();
  }


private:
  VMutex& m_Mutex;
};


#if defined(WIN32)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutexPC.inl>
#elif defined(_VISION_XENON)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutexXenon.inl>
#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutexPS3.inl>
#elif defined(_VISION_POSIX)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutexPosix.inl>
#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutexPSP2.inl>
#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutexWiiU.inl>
#else
  #error Undefined platform!
#endif



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
