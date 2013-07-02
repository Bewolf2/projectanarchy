/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBaseMem.hpp

#ifndef DC_VBASEMEM_HPP
#define DC_VBASEMEM_HPP




/// \brief
///  Forces allocations to be in the >4GB range to find pointer-to-int conversion issues.
///
/// Maps out the entire 32bit virtual address range with access-protected pages, and fills up the already
/// mapped heap pages so that new allocations will be in the >4GB range. Accessing any 32bit memory
/// location other than those mapped before this call will result in an access violation.
///
/// This can be used as a helper to track down problems caused by pointer-to-int conversions which throw
/// away significant bits. Don't use this feature in production code.
///
/// Does nothing in 32bit applications or on consoles.
VBASE_IMPEXP void VForceAllocationsOver4GB();

#if defined(new) || defined(delete)
  #error "New and delete macros shouldn't be defined prior to the inclusion of this file!"
#endif

// The real functions that the macros use for debug build
VBASE_IMPEXP void*  VBaseAlloc(size_t iSize);
VBASE_IMPEXP void   VBaseDealloc(void* ptr);
VBASE_IMPEXP void*  VBaseAlignedAlloc(size_t iSize, int iAlignment);
VBASE_IMPEXP void   VBaseAlignedDealloc(void* ptr);

#include <new>    // std::bad_alloc

#ifndef NO_NEW_OVERRIDE

#if !defined(SPU) && !defined(_VISION_WIIU)
#pragma warning(push)
  #pragma warning(disable: 4290  28251)  // Ignoring exception specification,  mismatching static analysis annotations.
#endif

// Global overloads of operator new and operator delete and their array versions.
#if !defined(_VISION_ANDROID)
  inline void* VISION_CDECL operator new  (size_t iSize) throw (std::bad_alloc)                 { return VBaseAlloc(iSize); }
  inline void* VISION_CDECL operator new[](size_t iSize) throw (std::bad_alloc)                 { return VBaseAlloc(iSize); }
  inline void* VISION_CDECL operator new  (size_t iSize, size_t iAlign) throw (std::bad_alloc)  { return VBaseAlignedAlloc(iSize,(int)iAlign); }
  inline void* VISION_CDECL operator new[](size_t iSize, size_t iAlign) throw (std::bad_alloc)  { return VBaseAlignedAlloc(iSize,(int)iAlign); }
#endif

  inline void  VISION_CDECL operator delete  (void *ptr) throw()                                { VBaseDealloc(ptr); }
  inline void  VISION_CDECL operator delete[](void *ptr) throw()                                { VBaseDealloc(ptr); }
  inline void  VISION_CDECL operator delete  (void *ptr, size_t iAlign) throw()                 { VBaseAlignedDealloc(ptr); }
  inline void  VISION_CDECL operator delete[](void *ptr, size_t iAlign) throw()                 { VBaseAlignedDealloc(ptr); }

#if defined(_CPPUNWIND)
// Nothrow versions which catch exceptions and convert them to return NULL
  inline void* VISION_CDECL operator new  (size_t iSize, const std::nothrow_t&) throw() try { return VBaseAlloc(iSize); } catch(std::bad_alloc) { return NULL; }
  inline void* VISION_CDECL operator new[](size_t iSize, const std::nothrow_t&) throw() try { return VBaseAlloc(iSize); } catch(std::bad_alloc) { return NULL; }
  inline void* VISION_CDECL operator new  (size_t iSize, size_t iAlign, const std::nothrow_t&) throw () try { return VBaseAlignedAlloc(iSize,(int)iAlign); } catch(std::bad_alloc) { return NULL; }
  inline void* VISION_CDECL operator new[](size_t iSize, size_t iAlign, const std::nothrow_t&) throw () try { return VBaseAlignedAlloc(iSize,(int)iAlign); } catch(std::bad_alloc) { return NULL; }
  inline void  VISION_CDECL operator delete  (void* ptr, const std::nothrow_t&) throw() try { VBaseDealloc(ptr);        } catch(...)            { }
  inline void  VISION_CDECL operator delete[](void* ptr, const std::nothrow_t&) throw() try { VBaseDealloc(ptr);        } catch(...)            { }
  inline void  VISION_CDECL operator delete  (void* ptr, size_t iAlign, const std::nothrow_t&) throw() try { VBaseAlignedDealloc(ptr);        } catch(...)            { }
  inline void  VISION_CDECL operator delete[](void* ptr, size_t iAlign, const std::nothrow_t&) throw() try { VBaseAlignedDealloc(ptr);        } catch(...)            { }
#endif

#if !defined(SPU) && !defined(_VISION_WIIU)
#pragma warning(pop)
#endif

#endif	// #ifndef NO_NEW_OVERRIDE

#include <Vision/Runtime/Base/System/Memory/VBaseObject.hpp>
#include <Vision/Runtime/Base/System/Memory/VConstructorHelpers.h>
#include <Vision/Runtime/Base/System/Memory/VMemoryStatistics.hpp>

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
