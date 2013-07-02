/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VEvent.hpp

#ifndef VEVENT_HPP_INCLUDED
#define VEVENT_HPP_INCLUDED

#ifdef _VISION_PS3
#include <sys/ppu_thread.h>
#include <sys/synchronization.h>
#endif

#ifdef _VISION_POSIX
#include <pthread.h>
#include <semaphore.h>
#endif

/// \brief
///   Synchronization object class for events.
///
/// VEvent is a simple synchronization object that allows one thread to wait for another thread
/// to signal it.
class VEvent
{
public:
  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor; creates the event.
  inline VEvent();

  /// \brief
  ///   Destructor; destroys the event.
  inline ~VEvent();

  /// \brief
  ///   Waits for the event to be signaled
  inline void Wait();

  /// \brief
  ///   Signals the event
  inline void Signal();

private:

  #if defined(WIN32)  || defined(_VISION_XENON)  
    volatile HANDLE m_hEvent;

  #elif defined(_VISION_PS3)
    sys_semaphore_t m_Semaphore;

  #elif defined(_VISION_POSIX)
    pthread_cond_t m_Condition;
    pthread_mutex_t m_Mutex;
    volatile bool m_bEventCondition;

  #elif defined(_VISION_PSP2)
    SceUID m_Semaphore;

  #elif defined(_VISION_WIIU)
    OSEvent m_Event;

  #else
    #error "Missing Platform!"
  #endif

};

#if defined(WIN32)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventPC.inl>
#elif defined(_VISION_XENON)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventXenon.inl>
#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventPS3.inl>
#elif defined(_VISION_POSIX)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventPosix.inl>
#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventPSP2.inl>
#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventWiiU.inl>
#else
  #error Undefined platform!
#endif


#endif  // VEVENT_HPP_INCLUDED

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
