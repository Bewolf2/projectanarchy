/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
// Build configuration settings
// These are compile time settings for the libraries.
// Changing these values with compiled libs will not work,

// If this setting is changed you must rebuild all Havok libs

// EQSELECT: HK_CONFIG_THREAD, Choose to enable or disable simd instructions:
// This determines whether or not we use simd instructions.


#ifndef HK_BASE_CONFIGTHREAD_H
#define HK_BASE_CONFIGTHREAD_H

#define HK_CONFIG_SINGLE_THREADED 1
#define HK_CONFIG_MULTI_THREADED  2

#if !defined(HKBASE_HKBASETYPES_H)
#	error hkbase/hkBase.h needs to be included before this file
#endif

#if defined(HK_PLATFORM_WIIU)
#	include <Common/Base/Config/hkConfigWiiU.h>
#endif

#ifndef HK_CONFIG_THREAD

	// ia32 / x6, incl WinRT
#	if defined(HK_PLATFORM_WIN32)
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED

	// PSP(R) (PlayStation(R)Portable)
#	elif defined(HK_PLATFORM_PSP)
#		define HK_CONFIG_THREAD HK_CONFIG_SINGLE_THREADED

	// PlayStation(R)3
#	elif defined(HK_PLATFORM_PS3_PPU)
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED

	// PlayStation(R)3 SPU
#	elif defined(HK_PLATFORM_PS3_SPU)
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED

	// Xbox
#	elif defined(HK_PLATFORM_XBOX)
#		define HK_CONFIG_THREAD HK_CONFIG_SINGLE_THREADED

	// Xbox 360
#	elif defined(HK_PLATFORM_XBOX360)
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED

	// Gamecube/Wii
#	elif defined(HK_PLATFORM_GC)
#		define HK_CONFIG_THREAD HK_CONFIG_SINGLE_THREADED

	// Linux
#	elif defined(HK_PLATFORM_LINUX)
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED

	// Mac
#	elif defined(HK_PLATFORM_MAC386) || defined(HK_PLATFORM_MACPPC) 
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED

#	elif defined(HK_PLATFORM_IOS) // no point on current gen single core iphone etc (but will work if you want)
		#if defined(HK_ARCH_ARM_V7) || defined(HK_PLATFORM_IOS_SIM)
			#define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED
		#else // V6 had no dual cores really
			#define HK_CONFIG_THREAD HK_CONFIG_SINGLE_THREADED
		#endif

	// LRB
#	elif defined(HK_PLATFORM_LRB) 
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED

	// Android 
#	elif defined(HK_PLATFORM_ANDROID)
		#ifdef HK_ARCH_ARM_V7
			#define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED
		#else // V6 had no dual cores really
			#define HK_CONFIG_THREAD HK_CONFIG_SINGLE_THREADED
		#endif

	// CTR 
#	elif defined(HK_PLATFORM_CTR)
#		define HK_CONFIG_THREAD HK_CONFIG_SINGLE_THREADED

	// PS VITA
#	elif defined(HK_PLATFORM_PSVITA)
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED 

	// PlayStation(R)4
#	elif defined(HK_PLATFORM_PS4)
#		define HK_CONFIG_THREAD HK_CONFIG_MULTI_THREADED 

#	else
# 	  	error Unknown Platform
#	endif

#else // ifndef HK_CONFIG_THREAD
#	if (HK_CONFIG_THREAD != HK_CONFIG_MULTI_THREADED) && (HK_CONFIG_THREAD != HK_CONFIG_SINGLE_THREADED)
#		error invalid config thread option
#	endif
#endif // ifndef HK_CONFIG_THREAD

#define HK_MAX_NUM_THREADS 12	// latest I9 has 12 hyperthreads

#endif // HK_BASE_CONFIGTHREAD_H

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
