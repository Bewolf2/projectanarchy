/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBASE_HK_PTHREADS_UTIL
#define HKBASE_HK_PTHREADS_UTIL

#include <Common/Base/Thread/Thread/Posix/hkPosixCheck.h>
#include <pthread.h>
#if defined(HK_PLATFORM_IOS_SIM) ||defined(HK_PLATFORM_MAC386) || defined(HK_PLATFORM_LINUX)
#include <xmmintrin.h>
#endif

#ifdef HK_ARCH_ARM
  #define cpu_relax() asm volatile("":::"memory") 
#endif

/// Static class with utility methods for the posix threads library
class hkPthreadUtil 
{	
	public:

		enum
		{
			/// Delay used by default in the busy wait loops. The actual duration of the delay is platform dependent.
			DEFAULT_DELAY_PER_SPIN = 10
		};

	public:

		/// Tries to lock a pthread mutex up to spin count times before blocking on it. Delay per spin indicates how 
		/// many times the platform specific delay instruction will be executed per iteration of the spin loop.
		static HK_FORCE_INLINE void lockMutexWithSpinCount(
			pthread_mutex_t &mutex, int spinCount, int delayPerSpin = DEFAULT_DELAY_PER_SPIN)
		{			
			for (int spinsLeft = spinCount; spinsLeft; --spinsLeft)
			{
				if (pthread_mutex_trylock(&mutex) == 0)
				{
					return;
				}
			#if defined(HK_ARCH_IA32) || defined(HK_ARCH_X64)
				for (int i = 0; i < delayPerSpin; ++i)
				{
					// This will delay execution of the next instruction for an implementation-specific amount of time 
					// and yield processor to other hyper-threads running on the same core (see Intel PAUSE intrinsic 
					// and Windows YieldProcessor macro for details).
					_mm_pause();
				}
			#else
				cpu_relax();
			#endif
			}

			HK_POSIX_CHECK(pthread_mutex_lock(&mutex));
		}
};

#endif // HKBASE_HK_PTHREADS_UTIL

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
