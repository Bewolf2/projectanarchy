/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <pthread.h>
#include <stdio.h>	

#define CHECK( A ) HK_MULTILINE_MACRO_BEGIN \
	if( A != 0 ) { perror(#A); HK_BREAKPOINT(0);} \
	HK_MULTILINE_MACRO_END

inline hkCriticalSection::hkCriticalSection( int spinCount )
{
	pthread_mutexattr_t attr;
	CHECK( pthread_mutexattr_init(&attr) );
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	CHECK( pthread_mutex_init(&m_mutex, &attr ) ); //&attr
}

inline hkCriticalSection::~hkCriticalSection()
{	
	CHECK( pthread_mutex_destroy(&m_mutex) );
}


// Removed in 4.5 Release?
/*inline int hkCriticalSection::tryEnter()
{
	//trylock returns 0 if successful
	if ( pthread_mutex_trylock(&m_mutex) == 0   )
	{
		return 1;
	}
	return 0;
}*/


inline void hkCriticalSection::setTimersEnabled()
{
#ifdef HK_TIME_CRITICAL_SECTION_LOCKS
	HK_THREAD_LOCAL_SET(hkCriticalSection__m_timeLocks, 1);
#endif
}
inline void hkCriticalSection::setTimersDisabled()
{
#ifdef HK_TIME_CRITICAL_SECTION_LOCKS
	HK_THREAD_LOCAL_SET(hkCriticalSection__m_timeLocks, 0);
#endif
}


#ifndef HK_TIME_CRITICAL_SECTION_LOCKS
	inline void hkCriticalSection::enter()
	{
		pthread_mutex_lock(&m_mutex );
	}

	inline void hkCriticalSection::leave()
	{
		pthread_mutex_unlock(&m_mutex);
						
	}
#else // HK_TIME_CRITICAL_SECTION_LOCKS

	inline void hkCriticalSection::enter()
	{
		if ( pthread_mutex_trylock(&m_mutex) == 0 )
		{
			//lock acquired
		}
		else
		{
			//Busy wait
			if ( HK_THREAD_LOCAL_GET(hkCriticalSection__m_timeLocks) )
			{
				HK_TIMER_BEGIN("CriticalLock", HK_NULL);
				pthread_mutex_lock(&m_mutex );				
				HK_TIMER_END();
			}
			else
			{
				pthread_mutex_lock(&m_mutex );				
			}
		}
	}

	inline void hkCriticalSection::leave()
	{
		pthread_mutex_unlock(&m_mutex);		
	}
#endif // HK_TIME_CRITICAL_SECTION_LOCKS



hkUint32 HK_CALL hkCriticalSection::atomicExchangeAdd(hkUint32* var, int value)
{
#if defined(HK_COMPILER_GCC) || defined(HK_COMPILER_CLANG) // iOS Base SDK now supports this
	return __sync_fetch_and_add( var, value );
#else
	#if (HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED)
		HK_ASSERT2(0x43254fe, 0, "ATOMIC XADD NOT IMPLIMENTED!");
	#endif
	hkUint32 r = *var;
	*var += value;
	return r;
#endif
}

HK_FORCE_INLINE hkUint32 hkCriticalSection::waitForValueEqual( hkUint32* var, hkUint32 value )
{
	volatile hkUint32* pVar = (volatile hkUint32*)var;
	hkUint32 ret;
	while ( (ret=*pVar) != value ){}
	return ret;
}

HK_FORCE_INLINE hkUint32 hkCriticalSection::waitForValueNotEqual( hkUint32* var, hkUint32 value )
{
	volatile hkUint32* pVar = (volatile hkUint32*)var;
	hkUint32 ret;
	while ( (ret=*pVar) == value ){}
	return ret;
}


HK_FORCE_INLINE hkUint16 hkCriticalSection::waitForValueNotEqual( hkUint16* var, hkUint16 value )
{
	volatile hkUint16* pVar = (volatile hkUint16*)var;
	hkUint16 ret;
	while ( (ret=*pVar) == value ){}
	return ret;
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
