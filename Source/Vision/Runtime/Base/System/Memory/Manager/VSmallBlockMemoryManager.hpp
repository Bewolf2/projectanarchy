/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSmallBlockMemoryManager.hpp

#ifndef SMALLBLOCKMEMORYMANAGER_HPP
#define SMALLBLOCKMEMORYMANAGER_HPP

#include <Vision/Runtime/Base/System/Memory/Manager/VMemoryManager.hpp>
#include <Vision/Runtime/Base/System/Memory/Manager/VMemoryManager_CRT.hpp>
#include <Vision/Runtime/Base/Container/VMaps.hpp>

class VSmallBlockBin;

#define V_SMALLBLOCK_NUMBER_OF_BINS 20
#define V_SMALLBLOCK_MAX_SIZE 128

#ifndef _VISION_DOC

struct VBlockDescriptor_t
{
  int fixedAllocSize;
  int chunks;
};

#endif


/// \brief
///   Optimized, fragmentation-reducing memory manager (replacement for VMemoryManager_CRT)
/// 
/// VSmallBlockMemoryManager is an alternative memory manager which usually provides significantly better fragmentation
/// management and slightly better performance than the default memory manager. It uses 20 separate buckets for
/// allocations of up to 128 bytes, and allocates buckets of increasing sizes using the OS's memory manager when
/// more allocations of a specific size group are required. Larger allocations are simply passed to its base class,
/// VMemoryManager_CRT, which uses OS memory management functions.
class VSmallBlockMemoryManager : public VMemoryManager_CRT
{
public:

  /// \brief
  ///   Constructor.
  VBASE_IMPEXP VSmallBlockMemoryManager()
  {
    m_bIsInitialized = false;
    Initialize();
  }

  /// \brief
  ///   Destructor.
  VBASE_IMPEXP ~VSmallBlockMemoryManager()
  {
    DeInitialize();
  }

  VBASE_IMPEXP virtual void* Alloc(size_t iSize) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void Free (void* ptr) HKV_OVERRIDE;

  VBASE_IMPEXP virtual size_t MemSize(void* ptr) HKV_OVERRIDE;

  /// \copybrief IVMemoryManager::DumpLeaks()
  VBASE_IMPEXP virtual void DumpLeaks();

  /// \brief
  ///   Returns whether the memory manager is initialized and ready for use.
  inline bool IsInitialized() { return m_bIsInitialized; }

private:
  void Initialize();
  void DeInitialize();

  int FindBlockIndex(void* b);
  VSmallBlockBin* FindBlockInArray(void* p);

  void AddBlockToArray(VSmallBlockBin* b);
  void RemoveBlockFromArray(VSmallBlockBin* b);

  VSmallBlockBin* m_FreeBlocks[V_SMALLBLOCK_NUMBER_OF_BINS];
  long m_iBlockCount;
  long m_iBlockArraySize;
  VSmallBlockBin** m_ppBlockArray; 
  VSmallBlockBin** m_ppBlockArrayEnd;
  bool m_bIsInitialized;
  VSmallBlockBin* m_pLastFoundBlock;

  VBlockDescriptor_t m_BlockDescriptors[V_SMALLBLOCK_NUMBER_OF_BINS];

  signed char m_cBDIndexLookup[V_SMALLBLOCK_MAX_SIZE+1];

  bool m_bIsInAlloc;
  bool m_bIsInFree;

  VMutex m_Mutex;

  unsigned int m_iAllocIndex;
};



#endif //SMALLBLOCKMEMORYMANAGER_HPP

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
