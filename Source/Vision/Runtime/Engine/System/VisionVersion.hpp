/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionVersion.hpp

#define VISION_RESOURCE_VERSION_MAJOR     2013
#define VISION_RESOURCE_VERSION_MINOR     1
#define VISION_RESOURCE_VERSION_FIX       0
#define VISION_RESOURCE_VERSION_BUILD     1
#define VISION_RESOURCE_VERSION_NUMBER VISION_RESOURCE_VERSION_MAJOR, VISION_RESOURCE_VERSION_MINOR, VISION_RESOURCE_VERSION_FIX, VISION_RESOURCE_VERSION_BUILD
#define VISION_RESOURCE_VERSION_STRING    "2013, 1, 0, 1\0"
#define VISION_RESOURCE_VERSION_ASSEMBLY  "2013.1.0.*\0"
#define VISION_RESOURCE_VERSION_PRETTY_PRINT V_STRINGIZE(V_CAT(V_CAT(V_CAT(V_CAT(VISION_RESOURCE_VERSION_MAJOR, .), VISION_RESOURCE_VERSION_MINOR), .), VISION_RESOURCE_VERSION_FIX))

#define VISION_RESOURCE_COPYRIGHT         "Copyright (c) 2012 Havok\0"

#if defined(HK_DEBUG_SLOW)
  #define VISION_RESOURCE_VERSION_ADDITIONALINFO    "Debug Build"
#elif defined(HK_DEBUG)
  #define VISION_RESOURCE_VERSION_ADDITIONALINFO    "Development Build"
#else
  #define VISION_RESOURCE_VERSION_ADDITIONALINFO    "Release Build"
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
