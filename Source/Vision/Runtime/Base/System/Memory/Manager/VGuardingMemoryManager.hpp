/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef VGuardingMemoryManager_h__
#define VGuardingMemoryManager_h__


#include <Vision/Runtime/Base/System/Memory/Manager/VMemoryManager.hpp>
#include <Vision/Runtime/Base/System/Memory/Manager/VMemoryManager_CRT.hpp>


/// \brief
///  A memory manager that catches accesses to out-of-bounds array indices and freed pointers.
///
/// On non-Windows platforms, this class is typedef'd to the default memory manager class.
///
/// Every allocation performed with this manager is surrounded with access protected pages. The allocation
/// is right-aligned inside the accessible pages, so any accesses to memory past the allocated region
/// will crash immediately. The memory before the allocation is padded with a magic value, which allows
/// catching invalid writes immediately before the allocated region.
///
/// The most recently freed allocations are not immediately released, but made access-protected instead
/// which allows finding invalid accesses to freed pointers.
///
/// Using this manager has an extremely high memory and CPU overhead.
///
#if !defined(WIN32) && !defined(_VISION_DOC)

typedef VMemoryManager_CRT VGuardingMemoryManager;

#else

class VGuardingMemoryManager : public IVMemoryManager
{
public:
  VBASE_IMPEXP VGuardingMemoryManager();

  VBASE_IMPEXP virtual ~VGuardingMemoryManager();

  VBASE_IMPEXP virtual void*   Alloc(size_t iSize) HKV_OVERRIDE;

  VBASE_IMPEXP virtual void*   AlignedAlloc(size_t iSize, int iAlignment) HKV_OVERRIDE;

  VBASE_IMPEXP virtual void    Free(void* ptr) HKV_OVERRIDE;

  VBASE_IMPEXP virtual void    AlignedFree(void* ptr) HKV_OVERRIDE;

  VBASE_IMPEXP virtual size_t  MemSize(void* ptr) HKV_OVERRIDE;

  VBASE_IMPEXP virtual size_t  AlignedMemSize(void* ptr, int iAlignment) HKV_OVERRIDE;

private:
  static const int iRetainAllocations = 1 << 16;

  void** pRetainedAllocations;

  int iLastRetainedAllocation;
  int iNumRetainedAllocations;
};

#endif

#endif // VGuardingMemoryManager_h__

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
