/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VAtomic.hpp

#ifndef VATOMIC_HPP_INCLUDED
#define VATOMIC_HPP_INCLUDED

#ifdef _VISION_PS3
#include <sys/ppu_thread.h>
#include <sys/synchronization.h>
#endif

#ifdef _VISION_POSIX
#include <pthread.h>
#include <semaphore.h>
#endif

/// \brief
///   Class containing platform-agnostic atomic functionality as a set of static functions
class VAtomic
{
public:
  
  /// \brief
  ///   Atomically increments the passed value and returns the new value
  inline static int Increment(int &i32);

  /// \brief
  ///   Atomically decrements the passed value and returns the new value
  inline static int Decrement(int &i32);
};

#if defined(WIN32)
  #include <Vision/Runtime/Base/System/Threading/Atomic/VAtomicPC.inl>
#elif defined(_VISION_XENON)
  #include <Vision/Runtime/Base/System/Threading/Atomic/VAtomicXenon.inl>
#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Base/System/Threading/Atomic/VAtomicPS3.inl>
#elif defined(_VISION_IOS)
  #include <Vision/Runtime/Base/System/Threading/Atomic/VAtomicIOS.inl>
#elif defined(_VISION_ANDROID)
  #include <Vision/Runtime/Base/System/Threading/Atomic/VAtomicAndroid.inl>
#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Base/System/Threading/Atomic/VAtomicPSP2.inl>
#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Base/System/Threading/Atomic/VAtomicWiiU.inl>
#else
  #error Undefined platform!
#endif


#endif  // VATOMIC_HPP_INCLUDED

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
