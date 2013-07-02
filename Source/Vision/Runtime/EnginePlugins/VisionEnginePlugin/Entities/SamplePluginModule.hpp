/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file SamplePluginModule.hpp


extern VModule g_sampleModule; ///< implemented in main using the DECLARE_THIS_MODULE macro

#ifdef SAMPLEPLUGIN_EXPORTS
  #define SAMPLEPLUGIN_IMPEXP __declspec(dllexport)
#elif defined SAMPLEPLUGIN_IMPORTS
  #define SAMPLEPLUGIN_IMPEXP __declspec(dllimport)
#else
  #if (defined (WIN32) || defined(_VISION_XENON)) && !defined(_VISION_WINRT)
    #define SAMPLEPLUGIN_IMPEXP __declspec()
  #else
    #define SAMPLEPLUGIN_IMPEXP    
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
