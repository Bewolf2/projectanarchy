/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !(defined(HK_DEBUG) || defined(PROFILING))
  #error This file must only be included for profiling builds
#endif

#if !defined(_VISION_POSIX)
  #error This file must not be included on this platform!
#endif

#if defined(_VISION_IOS)

  #define INSERT_PERF_MARKER(_markerString_) glInsertEventMarkerEXT(0, _markerString_);
  #define INSERT_PERF_MARKER_SCOPE(_markerString_) VisPerformanceMarker_cl ___visPerfMarker___(_markerString_);
  #define START_PERF_MARKER_BRACKET(_markerString_) glPushGroupMarkerEXT(0, _markerString_);
  #define STOP_PERF_MARKER_BRACKET(_markerString_) glPopGroupMarkerEXT();

#elif defined(_VISION_ANDROID)

  #define INSERT_PERF_MARKER(_markerString_)
  #define INSERT_PERF_MARKER_SCOPE(_markerString_)
  #define START_PERF_MARKER_BRACKET(_markerString_)
  #define STOP_PERF_MARKER_BRACKET(_markerString_)

#else

  #error Unknown posix platform

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
