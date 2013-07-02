/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Common/Base/Thread/Thread/Arm/hkArmThreadSync.h>

//#include <Common/Base/System/Android/hkAndroidCpuInfo.h>
//#include <android/log.h>

inline hkCriticalSection::hkCriticalSection( int spinCount )
:	m_recursiveLockCount(0), m_currentThread(0)	
{

}

inline hkCriticalSection::~hkCriticalSection()
{
}

inline void hkCriticalSection::enter()
{
#ifdef HK_TIME_CRITICAL_SECTION_LOCKS
	if ( HK_THREAD_LOCAL_GET(hkCriticalSection__m_timeLocks) )
	{
		HK_TIMER_BEGIN("CriticalLock", HK_NULL);
	}
#endif

	const hkUint32 tid = hk_threadid();
	bool haveLock = m_currentThread == tid;
	if (!haveLock) // don't have it already 
	{
	//	__android_log_print(ANDROID_LOG_INFO, "Havok", "0x%x waiting for lock 0x%x",  tid, (hkUlong)this);

		do 
		{
			int curThread = hk_ldrex( &m_currentThread );
			// try to set the curThread to 'me' if cur is 0 (so unlocked
			if (curThread == 0)
			{
				if (0 == hk_strex(tid, &m_currentThread) )
					break; // got it
			}
			// otherwise spin (busy wait)
		} while (true);

		hkArmMemBarrier(); // make sure all access after this is done after we have the lock

	//	__android_log_print(ANDROID_LOG_INFO, "Havok", "0x%x GOT lock 0x%x",  tid, (hkUlong)this);

		m_recursiveLockCount = 1;
	}
	else
	{
		m_recursiveLockCount++;
	}

#ifdef HK_TIME_CRITICAL_SECTION_LOCKS
	if ( HK_THREAD_LOCAL_GET(hkCriticalSection__m_timeLocks) )
	{
		HK_TIMER_END();
	}
#endif
}

inline void hkCriticalSection::leave()
{
#ifdef HK_TIME_CRITICAL_SECTION_LOCKS
	if ( HK_THREAD_LOCAL_GET(hkCriticalSection__m_timeLocks) )
	{
		HK_TIMER_BEGIN("CriticalUnlock", HK_NULL);
	}
#endif

	// only leave if we have the lock
#ifdef HK_DEBUG
	const hkUint32 tid = hk_threadid(); 
	HK_ASSERT2(0xd04a560, m_currentThread == tid, "Releasing lock that you don't hold!");
#endif

	m_recursiveLockCount--;
	HK_ASSERT2(0x2b8f059e, m_recursiveLockCount >= 0, "hkCriticalSection::leave() without matching ::enter!" );
	if (m_recursiveLockCount == 0) // actually release it
	{
//		__android_log_print(ANDROID_LOG_INFO, "Havok", "0x%x RELEASING lock 0x%x",  tid, (hkUlong)this);
		
		hkArmMemBarrier(); // make sure all access before this is done before we unlock
		
		do
		{
			HK_ON_DEBUG(hkUint32 curThread = ) hk_ldrex( &m_currentThread );
			#ifdef HK_DEBUG
				HK_ASSERT2(0xd04a560, curThread == tid, "Releasing lock that you don't hold!");
				//XXXX
			#endif
		} while (0 != hk_strex(0, &m_currentThread) );



//		__android_log_print(ANDROID_LOG_INFO, "Havok", "0x%x have RELEASED lock 0x%x",  tid, (hkUlong)this);
		
	}
	
#ifdef HK_TIME_CRITICAL_SECTION_LOCKS
	if ( HK_THREAD_LOCAL_GET(hkCriticalSection__m_timeLocks) )
	{
		HK_TIMER_END();
	}
#endif
}

inline void HK_CALL hkCriticalSection::setTimersEnabled()
{
#ifdef HK_TIME_CRITICAL_SECTION_LOCKS
	HK_THREAD_LOCAL_SET(hkCriticalSection__m_timeLocks, 1);
#endif
}

inline void HK_CALL hkCriticalSection::setTimersDisabled()
{
#ifdef HK_TIME_CRITICAL_SECTION_LOCKS
	HK_THREAD_LOCAL_SET(hkCriticalSection__m_timeLocks, 0);
#endif
}

hkUint32 HK_CALL hkCriticalSection::atomicExchangeAdd(hkUint32* var, int value)
{
	hkUint32 oldVal = hk_atomic_add( var, value );
//	__android_log_print(ANDROID_LOG_INFO, "Havok", "Atomic Add with %d, old value was %d", value, oldVal );
		
	return oldVal;
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
