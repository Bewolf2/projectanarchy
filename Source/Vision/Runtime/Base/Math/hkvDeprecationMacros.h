/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVDEPRECATIONMACROS_H
#define HKVDEPRECATIONMACROS_H

#include <Vision/Runtime/Base/VBaseVersion.hpp>

// If HKV_IGNORE_DEPRECATION_GLOBALLY is defined, deprecation macros will be ignored globally. It is present only to facilitate
// porting. Using this macro should generally be avoided, as it hides warnings indicating the use of deprecated functions.
#ifdef HKV_IGNORE_DEPRECATION_GLOBALLY

  #define VISION_DEPRECATED
  #define HKV_DEPRECATED
  #define HKV_DISABLE_DEPRECATION
  #define HKV_ENABLE_DEPRECATION

#else

  // VISION_DEPRECATED will mark code as deprecated on Win32 and PS3
  // the disadvantage is, that it cannot be temporarily disabled
  // Use HKV_DEPRECATED for such things instead.
  #if defined(__GCC__) || defined(__SNC__) || defined(__GNUC__)
    #define VISION_DEPRECATED __attribute__ ((deprecated))
  #elif defined(_MSC_VER) && (_MSC_VER >= 1300)  ///<MSVC 7.0
    #define VISION_DEPRECATED __declspec(deprecated)
  #else
    #define VISION_DEPRECATED ///< CWCC doesn't seem to have such a 'marker' facility.
  #endif

  #if defined(_MSC_VER) && (_MSC_VER >= 1300)  ///<MSVC 7.0
    // On Windows this macro will mark code as deprecated.
    #define HKV_DEPRECATED __declspec(deprecated)

    // This macro will temporarily disable the deprecated warning, allowing you to use deprecated code without getting a warning.
    #define HKV_DISABLE_DEPRECATION \
      __pragma (warning (push)) \
      __pragma (warning (disable : 4996))

    // This macro will enable the deprecated warning again, this should be used at the end of a code block which had this warning disabled.
    #define HKV_ENABLE_DEPRECATION \
      __pragma (warning (pop))
  #else
    // On Non-Windows platforms this macro is ignored.
    #define HKV_DEPRECATED
  // On Non-Windows platforms this macro is ignored.
    #define HKV_DISABLE_DEPRECATION
  // On Non-Windows platforms this macro is ignored.
    #define HKV_ENABLE_DEPRECATION
  #endif
#endif
// *** Deprecation Macros for different releases ***

// 2012.1
#if (VISION_RESOURCE_VERSION_MAJOR > 2012) || ((VISION_RESOURCE_VERSION_MAJOR == 2012) && (VISION_RESOURCE_VERSION_MINOR >= 1))
  #define HKV_DEPRECATED_2012_1 HKV_DEPRECATED
#else
  #define HKV_DEPRECATED_2012_1
#endif

// 2012.2
#if (VISION_RESOURCE_VERSION_MAJOR > 2012) || ((VISION_RESOURCE_VERSION_MAJOR == 2012) && (VISION_RESOURCE_VERSION_MINOR >= 2))
  #define HKV_DEPRECATED_2012_2 HKV_DEPRECATED
#else
  #define HKV_DEPRECATED_2012_2
#endif

// 2012.3
#if (VISION_RESOURCE_VERSION_MAJOR > 2012) || ((VISION_RESOURCE_VERSION_MAJOR == 2012) && (VISION_RESOURCE_VERSION_MINOR >= 3))
  #define HKV_DEPRECATED_2012_3 HKV_DEPRECATED
#else
  #define HKV_DEPRECATED_2012_3
#endif

// 2013.1
#if (VISION_RESOURCE_VERSION_MAJOR > 2013) || ((VISION_RESOURCE_VERSION_MAJOR == 2013) && (VISION_RESOURCE_VERSION_MINOR >= 1))
  #define HKV_DEPRECATED_2013_1 HKV_DEPRECATED
#else
  #define HKV_DEPRECATED_2013_1
#endif

// 2013.2
#if (VISION_RESOURCE_VERSION_MAJOR > 2013) || ((VISION_RESOURCE_VERSION_MAJOR == 2013) && (VISION_RESOURCE_VERSION_MINOR >= 2))
  #define HKV_DEPRECATED_2013_2 HKV_DEPRECATED
#else
  #define HKV_DEPRECATED_2013_2
#endif

// 2013.3
#if (VISION_RESOURCE_VERSION_MAJOR > 2013) || ((VISION_RESOURCE_VERSION_MAJOR == 2013) && (VISION_RESOURCE_VERSION_MINOR >= 3))
  #define HKV_DEPRECATED_2013_3 HKV_DEPRECATED
#else
  #define HKV_DEPRECATED_2013_3
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
