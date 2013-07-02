/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VCALLSTACKTRACKINGMEMORYMANAGER_INCLUDED
#define VCALLSTACKTRACKINGMEMORYMANAGER_INCLUDED

#include <Vision/Runtime/Base/System/Memory/Manager/VMemoryManager.hpp>

/// \brief
///   A memory manager that saves call stacks in order to exactly locate memory leaks.
/// 
/// Call stack and symbol information is currently only available on Windows builds,
/// and is most reliable in Debug builds. On other platforms, only basic leak information
/// and dependency analysis are available.
///
class VCallStackTrackingMemoryManager : public IVMemoryManager
{
public:
  /// \brief Dummy constructor - Internal use only.
  VBASE_IMPEXP VCallStackTrackingMemoryManager() {
    pInternal = NULL;
  }

  /// \brief Constructs a callstack tracking memory manager wrapping around pBaseManager.
  VBASE_IMPEXP VCallStackTrackingMemoryManager(IVMemoryManager* pBaseManager, int iMinTrackingSize = 0);

  VBASE_IMPEXP virtual ~VCallStackTrackingMemoryManager();

  // Overridden base functions
  VBASE_IMPEXP virtual void DumpLeaks();
  VBASE_IMPEXP virtual void LoadSymbols();

  VBASE_IMPEXP virtual void*   Alloc(size_t iSize) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void*   AlignedAlloc(size_t iSize, int iAlignment) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void    Free(void* ptr) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void    AlignedFree(void* ptr) HKV_OVERRIDE;
  VBASE_IMPEXP virtual size_t  MemSize(void* ptr) HKV_OVERRIDE;
  VBASE_IMPEXP virtual size_t  AlignedMemSize(void* ptr, int iAlignment) HKV_OVERRIDE;

  /// \brief
  ///   Returns the number of memory leaks detected.
  /// \param [out] rootLeaks
  ///   the number of root memory leaks. A root leak is a leaked memory block that is
  ///   not referenced from any other leaked memory block.
  /// \param [out] dependentLeaks
  ///   the number of dependent memory leaks. A dependent leak is a memory block that
  ///   is referenced from at least one other leaked memory block.
  VBASE_IMPEXP void GetNumLeaks(int& rootLeaks, int& dependentLeaks);
private:
  // Pointer to internal data
  void* pInternal;
};

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
