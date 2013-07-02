/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HK_ARM_THREAD_SYNC__H
#define HK_ARM_THREAD_SYNC__H

#define hkArmMemBarrier() __asm__ __volatile__("dmb": : :"memory")

// Intrinic like ver of LDREX (handy for more readable code)
inline hkUint32 hk_ldrex(volatile hkUint32* v )
{
    hkUint32 ret; 
	__asm__ __volatile__(
		"     ldrex   %0, [%1]\n"
        : "=&r" (ret)
        : "r" (v)
		: 
        );

	return ret;
}

inline hkUint32 hk_strex(hkUint32 s, volatile hkUint32* v)
{
    hkArmMemBarrier();

     hkUint32 ret; 
	__asm__ __volatile__(
		"	strex   %0, %2, [%1]\n"
        : "=&r" (ret)
        : "r" (v), "r" (s)
        : 
        );

	return ret;
}	

inline hkUint32 hk_threadid()
{
    pthread_t tid = pthread_self();
    return (hkUint32) tid;
}	

inline hkUint32 hk_atomic_add(volatile hkUint32* ptr, hkInt32 value)
{
	hkUint32 o;
	hkUint32 n;
	do
	{
		o = hk_ldrex(ptr);
		n = o + value;
	} while ( hk_strex( n, ptr) != 0 );

	hkArmMemBarrier(); // make sure no code can run after the incl until it is complete

	return o;
}

#endif /* HK_ARM_THREAD_SYNC__H */

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
