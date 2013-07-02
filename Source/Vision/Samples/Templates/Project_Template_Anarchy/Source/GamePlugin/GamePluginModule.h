/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//  Plugin Defines removed from stdafx and placed here.
// 

#ifndef GAMEPLUGINMODULE_H_INCLUDED
#define GAMEPLUGINMODULE_H_INCLUDED

#ifdef WIN32
  #ifdef SAMPLEPLUGIN_EXPORTS
    #define SAMPLEPLUGIN_IMPEXP __declspec(dllexport)
  #elif SAMPLEPLUGIN_IMPORTS
    #define SAMPLEPLUGIN_IMPEXP __declspec(dllimport)
  #else
    #define SAMPLEPLUGIN_IMPEXP __declspec()
  #endif

#elif defined (_VISION_IOS) || defined(_VISION_ANDROID)
  #define SAMPLEPLUGIN_IMPEXP

#else
  #error Undefined platform!
#endif

#endif //  GAMEPLUGINMODULE_H_INCLUDED

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
