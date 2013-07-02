/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/BasePCH.h>

#include <Vision/Runtime/Base/System/Memory/Manager/VMemoryManager_CRT.hpp>


#if (defined(_MSC_VER) && (_MSC_VER < 1500))  || defined (_VISION_XENON)  || defined(_VISION_LINUX)
#include <malloc.h>	// Usually this is done via stdlib.h, but VS 2003 doesn't seem to get this right.
#endif

#if defined(_VISION_IOS) || defined(_VISION_ANDROID)
#include <stdlib.h>
#endif

#if defined(_DEBUG)                      &&  \
  (defined(WIN32)  || defined (_VISION_XENON) )
#define VBASE_USE_CRT_DEBUG
#include <crtdbg.h>
#endif


void* VMemoryManager_CRT::Alloc(size_t iSize)
{
#if defined(VBASE_USE_CRT_DEBUG)
  return _malloc_dbg(iSize, _NORMAL_BLOCK, 0, 0);
#elif defined(_VISION_WIIU)
  return MEMAllocFromDefaultHeap(iSize);
#else
  return malloc(iSize);
#endif
}


void* VMemoryManager_CRT::AlignedAlloc(size_t iSize, int iAlignment)
{
#if defined(WIN32)  || defined (_VISION_XENON) 
#if defined(VBASE_USE_CRT_DEBUG)
  return _aligned_malloc_dbg(iSize, iAlignment, 0, 0);
#else
  return _aligned_malloc(iSize, iAlignment);
#endif


#elif defined (_VISION_PS3)
  return memalign(iAlignment, iSize);


#elif defined (_VISION_POSIX) && !defined(_VISION_ANDROID)
  void* pMemPointer = NULL;
  posix_memalign(&pMemPointer, iAlignment, iSize);
  return pMemPointer;
#elif defined(_VISION_PSP2)
  return memalign(iAlignment, iSize);
#elif defined(_VISION_ANDROID)

  return memalign(iAlignment, iSize);

#elif defined(_VISION_WIIU)
  return MEMAllocFromDefaultHeapEx(iSize, iAlignment);

#else
#error Undefined platform!
#endif
}


void VMemoryManager_CRT::Free(void* ptr)
{
#if defined(VBASE_USE_CRT_DEBUG)
  _free_dbg(ptr, _NORMAL_BLOCK);
#elif defined(_VISION_WIIU)
  return MEMFreeToDefaultHeap(ptr);
#else
  free(ptr);
#endif
}


void VMemoryManager_CRT::AlignedFree(void* ptr)
{
#if defined(WIN32)  || defined (_VISION_XENON) 
#if defined(VBASE_USE_CRT_DEBUG)
  _aligned_free_dbg(ptr);
#else
  _aligned_free(ptr);
#endif


#elif defined (_VISION_PS3)
  return free(ptr);


#elif defined (_VISION_POSIX)
  return free(ptr);//@@@L test
#elif defined(_VISION_PSP2)
  return free(ptr);
#elif defined(_VISION_WIIU)

  return MEMFreeToDefaultHeap(ptr);
#else
#error Undefined platform!
#endif
}


size_t VMemoryManager_CRT::MemSize(void* ptr)
{
  // Not all memory size functions cope with NULL pointer
  if(ptr == NULL)
    return 0;

#if defined(WIN32)  || defined (_VISION_XENON) 
#if defined(VBASE_USE_CRT_DEBUG)
  return _msize_dbg(ptr, _NORMAL_BLOCK);
#else
  return _msize(ptr);
#endif


#elif defined (_VISION_PS3)
  return malloc_usable_size(ptr);



#elif defined (_VISION_POSIX)
  return 0; // TODO: IOS

#elif defined (_VISION_PSP2)
  return malloc_usable_size(ptr);
#elif defined (_VISION_WIIU)
  return 0; //XXCK can fake if you really need it
#else
#error Undefined platform!
#endif
}


size_t VMemoryManager_CRT::AlignedMemSize(void* ptr, int iAlignment)
{
  // Not all memory size functions cope with NULL pointer
  if(ptr == NULL)
    return 0;

#if defined(WIN32)  || defined (_VISION_XENON) 
#if defined(VBASE_USE_CRT_DEBUG)
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
  return _aligned_msize_dbg(ptr, iAlignment, 0);
#else
  return _msize_dbg(ptr, _NORMAL_BLOCK);//@@@@ VStudio < 2005 and code might be wrong
#endif
#else
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
  return _aligned_msize(ptr, iAlignment, 0);
#else
  return _msize(ptr);//@@@@ VStudio < 2005 and code might be wrong
#endif
#endif


#elif defined (_VISION_PS3)
  return malloc_usable_size(ptr);



#elif defined (_VISION_POSIX)
  return 0; // TODO: IOS

#elif defined (_VISION_PSP2)
  return malloc_usable_size(ptr);

#elif defined (_VISION_WIIU)
  return 0; // XXCK

#else
#error Undefined platform!
#endif
}

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
