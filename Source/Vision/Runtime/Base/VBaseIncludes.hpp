/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBaseIncludes.hpp

#ifndef VBASE_INCLUDES_HPP
#define VBASE_INCLUDES_HPP

#if defined(DEBUG) && !defined(HK_DEBUG)
  #error "Always use HK_DEBUG instead of DEBUG"
#endif

#if defined(_DEBUG) && !defined(HK_DEBUG)
#error "Always use HK_DEBUG instead of _DEBUG"
#endif


// make sure the current version is used in the manifest files
// see also http://stackoverflow.com/questions/59635/app-does-not-run-with-vs-2008-sp1-dlls-previous-version-works-with-rtm-versions
#ifndef _BIND_TO_CURRENT_VCLIBS_VERSION
  #define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#endif

//VC 2005: Allow use of standard C functions without deprecation warnings
#ifndef _CRT_SECURE_NO_DEPRECATE
  #define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
  #define _CRT_NONSTDC_NO_DEPRECATE
#endif
#ifndef _CRT_NON_CONFORMING_SWPRINTFS
  #define _CRT_NON_CONFORMING_SWPRINTFS
#endif

//Add all the external headers that vBase needs as input here


////////////////////////////////////////////////////////////////
//  Standard C headers
////////////////////////////////////////////////////////////////

#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <float.h>
#include <ctype.h>
#include <wchar.h>

#if defined(__CWCC__) || defined(__GNUC__)
  #include <wctype.h>
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
  #include <crtdefs.h>
#endif

#ifndef SPU
  #include <time.h>
  
  #ifndef _VISION_PSP2
  #include <locale.h>
  #include <assert.h>
  #endif
#endif

#include <stddef.h>



////////////////////////////////////////////////////////////////
//  Platform headers
////////////////////////////////////////////////////////////////

#if defined(_VISION_WINRT)
  #include <Vision/Runtime/Base/Platform/VPlatformWinRT.hpp>

#elif defined(WIN32)
  #include <Vision/Runtime/Base/Platform/VPlatformWin32.hpp>

#elif defined(_VISION_XENON)
  #include <Vision/Runtime/Base/Platform/VPlatformXenon.hpp>

#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Base/Platform/VPlatformPS3.hpp>

#elif defined(_VISION_POSIX)
  #include <Vision/Runtime/Base/Platform/VPlatformPosix.hpp>

#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Base/Platform/VPlatformPSP2.hpp>

#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Base/Platform/VPlatformWiiU.hpp>

#else
  #error No Platform defined.
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
