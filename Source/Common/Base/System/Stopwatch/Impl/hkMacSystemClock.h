/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// OS X framework headers 
#ifdef HK_PLATFORM_IOS
#include <mach/mach_time.h>
#else
#include <CoreServices/CoreServices.h> // Carbon header
#include <Kernel/mach/mach_time.h>
#endif
// AbsoluteTime/Nanoseconds are structures equivalent to hkUint64

static hkUint64 _computeTicksPerSecond()
{
	uint64_t nano = 1000000000;

#ifdef HK_PLATFORM_IOS

	mach_timebase_info_data_t info;
	kern_return_t err = mach_timebase_info(&info); // in nanosecs

	if (err == 0)
		return ( nano * (hkUint64)info.denom ) / (hkUint64)info.numer; // invert as we will div by this num when using it
	else 
		return 1;


#else // use Carbon core services

	Nanoseconds oneSecondInNano =  *(Nanoseconds *) &nano;
	AbsoluteTime oneSecondInAbsolute = NanosecondsToAbsolute( oneSecondInNano );
	return  *(hkUint64 *) &oneSecondInAbsolute;

#endif
}

static const hkUint64 s_ticksPerSecond = _computeTicksPerSecond();

hkUint64 HK_CALL hkSystemClock::getTickCounter()
{
	return (hkUint64) mach_absolute_time();
}

hkUint64 HK_CALL hkSystemClock::getTicksPerSecond()
{
	return s_ticksPerSecond;
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
