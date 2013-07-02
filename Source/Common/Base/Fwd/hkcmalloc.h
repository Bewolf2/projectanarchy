/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#if defined( HK_PLATFORM_PS3_PPU ) || \
    defined( HK_PLATFORM_GC ) || \
    defined( HK_PLATFORM_RVL ) || \
    defined( HK_PLATFORM_MAC386 ) || \
	defined( HK_PLATFORM_MACPPC ) || \
    defined( HK_PLATFORM_IOS ) || \
    defined( HK_PLATFORM_CTR ) || \
    defined( HK_PLATFORM_PSVITA ) || \
    defined( HK_PLATFORM_WIIU) || \
	defined( HK_PLATFORM_PS4)
  // Get the normal header for GCC-XML
  #ifdef __GCCXML__
    #define _ANSI_SOURCE
  #endif
  #include <stdlib.h>
#else
  #include <malloc.h>
  namespace std {}
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
