/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef VISPERFORMANCEMARKER_HPP
#define VISPERFORMANCEMARKER_HPP


#if defined(HK_DEBUG_SLOW) || defined(PROFILING)

#if defined(WIN32)
  #include <Vision/Runtime/Engine/Profiling/VisPerformanceMarkerPC.hpp>
#elif defined(_VISION_XENON)
  #include <Vision/Runtime/Engine/Profiling/VisPerformanceMarkerXenon.hpp>
#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Engine/Profiling/VisPerformanceMarkerPS3.hpp>
#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Engine/Profiling/VisPerformanceMarkerPSP2.hpp>
#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Engine/Profiling/VisPerformanceMarkerWiiU.hpp>
#elif defined(_VISION_POSIX)
  #include <Vision/Runtime/Engine/Profiling/VisPerformanceMarkerPOSIX.hpp>
#else
  #error Performance marker not implemented on this platform!
#endif

#else

  #define INSERT_PERF_MARKER(_markerString_)
  #define INSERT_PERF_MARKER_SCOPE(_markerString_)
  #define START_PERF_MARKER_BRACKET(_markerString_)
  #define STOP_PERF_MARKER_BRACKET(_markerString_)
#endif


/// \brief 
///   This is a small helper class that wraps calls to START_PERF_MARKER_BRACKET and STOP_PERF_MARKER_BRACKET (RAII).
///
/// Create an instance of this class in functions that you want to profile, instead of using START_PERF_MARKER_BRACKET and STOP_PERF_MARKER_BRACKET.
/// It will ensure that STOP_PERF_MARKER_BRACKET is always called when the function ends.
class VisPerformanceMarker_cl
{
public:
  inline VisPerformanceMarker_cl(const char *pszString)
  {
    START_PERF_MARKER_BRACKET(pszString);
  }
  inline ~VisPerformanceMarker_cl()
  {
    STOP_PERF_MARKER_BRACKET(0);
  }
};

#endif // VISPERFORMANCEMARKER_HPP

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
