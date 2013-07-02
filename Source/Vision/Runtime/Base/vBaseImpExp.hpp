/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vBaseImpExp.hpp

#ifndef _VISION_VBASEIMPEX_HPP
#define _VISION_VBASEIMPEX_HPP



#if (defined(_WIN32) || defined(_VISION_XENON) ) && !defined(_VISION_WINRT) ///< do it always for MSDev projects (windows and Xenon)

  // Define VBASE_LIB to compile vBase as a static lib
  // or if you want to use vBase as a static lib in your project
  #ifdef VBASE_LIB
    #define VBASE_IMPEXP       __declspec()
    #define VBASE_IMPEXP_CLASS __declspec()
    #define VBASE_IMPEXP_TEMPLATE
    #ifdef HK_DEBUG_SLOW
      //#pragma message( "Automatic linking with vBaseSD.lib" )
      //#pragma comment(lib, "vBaseSD.lib")
    #else
      //#pragma message( "Automatic linking with vBaseS.lib" )
      //#pragma comment(lib, "vBaseS.lib")
    #endif
  #endif

  // Only defined in the vBase project to export functions
  #ifdef VBASE_EXPORTS
    #define VBASE_IMPEXP       __declspec( dllexport )
    #define VBASE_IMPEXP_CLASS __declspec( dllexport )
    #define VBASE_IMPEXP_TEMPLATE
  #endif

  // if not defined (or in a LIB, not exported from a dll) 
  // define it as import from a DLL (this is the case of client project which uses the dll)
  #ifndef VBASE_IMPEXP_CLASS
    #define VBASE_IMPEXP       __declspec( dllimport )
    #define VBASE_IMPEXP_CLASS __declspec( dllimport )
    #define VBASE_IMPEXP_TEMPLATE extern
  #endif


#elif defined(_VISION_PS3) || defined(_VISION_WIIU) || defined(_VISION_POSIX) || defined (_VISION_PSP2) || defined(_VISION_WINRT)

  // Define VBASE_LIB to compile vBase as a static lib
  // or if you want to use vBase as a static lib in your project
  #ifdef VBASE_LIB
    #define VBASE_IMPEXP
    #define VBASE_IMPEXP_CLASS
    #define VBASE_IMPEXP_TEMPLATE
  #endif //VBASE_LIB

  // Only defined in the vBase project to export functions
  #ifdef VBASE_EXPORTS
    //MK: GCC doesn't need specific markers for exporting symbols to a shared library
    #define VBASE_IMPEXP
    #define VBASE_IMPEXP_CLASS
    #define VBASE_IMPEXP_TEMPLATE
  #endif //VBASE_EXPORTS


  // if not defined (or in a LIB, not exported from a dll) 
  // define it as import from a DLL (this is the case of client project which uses the dll)
  #ifndef VBASE_IMPEXP_CLASS
    #define VBASE_IMPEXP
    #define VBASE_IMPEXP_CLASS
    #define VBASE_IMPEXP_TEMPLATE extern
  #endif //VBASE_IMPEXP_CLASS


#else ///<not WIN32, XENON, PS3 or LINUX

  #pragma chMSG( Define the VBASE_IMPEXP here for other operating systems )

#endif



//todo remove these... ?
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef int INT;
  
#endif // _VISION_VBASEIMPEX_HPP

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
