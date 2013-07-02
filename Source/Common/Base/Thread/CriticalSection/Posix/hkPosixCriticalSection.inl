/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Common/Base/Fwd/hkcstdio.h>
#include <Common/Base/Thread/Thread/Posix/hkPthreadUtil.h>

inline hkCriticalSection::hkCriticalSection( int spinCount ) : m_spinCount(spinCount)
{
#ifndef HK_PLATFORM_LRB
	pthread_mutexattr_t attr;
	HK_POSIX_CHECK( pthread_mutexattr_init(&attr) );
	HK_POSIX_CHECK( pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) );
	HK_POSIX_CHECK( pthread_mutex_init(&m_mutex, &attr) );
	HK_POSIX_CHECK( pthread_mutexattr_destroy(&attr) );
#else
	// LRB only supports default ptread attribs
	pthread_mutex_init(&m_mutex, HK_NULL);
#endif
}

inline hkCriticalSection::~hkCriticalSection()
{
	HK_POSIX_CHECK(pthread_mutex_destroy(&m_mutex));
}

inline void hkCriticalSection::enter()
{
	hkPthreadUtil::lockMutexWithSpinCount(m_mutex, m_spinCount, 10);
}

inline void hkCriticalSection::leave()
{
	HK_POSIX_CHECK( pthread_mutex_unlock(&m_mutex) );
}

inline void HK_CALL hkCriticalSection::setTimersEnabled()
{
}

inline void HK_CALL hkCriticalSection::setTimersDisabled()
{
}

hkUint32 HK_CALL hkCriticalSection::atomicExchangeAdd(hkUint32* var, int value)
{
#if defined( HK_COMPILER_GCC ) || defined( HK_COMPILER_CLANG )
	return __sync_fetch_and_add( var, value );
#else
	HK_COMPILE_TIME_ASSERT(0);
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
