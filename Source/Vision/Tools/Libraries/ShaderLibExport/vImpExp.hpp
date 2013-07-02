/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vImpExp.hpp

#ifndef VIMPEX_HPP_INCLUDED
#define VIMPEX_HPP_INCLUDED


#ifdef WIN32

  // Define VBASE_LIB to compile vBase as a static lib
  // or if you want to use vBase as a static lib in your project
  #ifdef VSHADERLIBEXPORT_LIB
    #define VSHADERLIBEXPORT_IMPEXP       __declspec()
    #define VSHADERLIBEXPORT_IMPEXP_CLASS __declspec()
    #define VSHADERLIBEXPORT_IMPEXP_TEMPLATE
  #endif

  // Only defined in the vBase project to export functions
  #ifdef VSHADERLIBEXPORT_EXPORTS
    #define VSHADERLIBEXPORT_IMPEXP       __declspec( dllexport )
    #define VSHADERLIBEXPORT_IMPEXP_CLASS __declspec( dllexport )
    #define VSHADERLIBEXPORT_IMPEXP_TEMPLATE
  #endif

  // if not defined (or in a LIB, not exported from a dll) 
  // define it as import from a DLL (this is the case of client project which uses the dll)
  #ifndef VSHADERLIBEXPORT_IMPEXP_CLASS
    #define VSHADERLIBEXPORT_IMPEXP       __declspec( dllimport )
    #define VSHADERLIBEXPORT_IMPEXP_CLASS __declspec( dllimport )
    #define VSHADERLIBEXPORT_IMPEXP_TEMPLATE extern
  #endif

#else

  #error "You cannot compile this project on anything else than Windows."

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
