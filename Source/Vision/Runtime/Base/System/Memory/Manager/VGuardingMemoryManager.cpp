/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/BasePCH.h>

#include <Vision/Runtime/Base/System/Memory/Manager/VGuardingMemoryManager.hpp>

#ifdef WIN32

#define NOMANSLANDSIZE 32
#define MAGIC 0x52574152
#define LEADING_PAGES 1
#define TRAILING_PAGES 1
#define MINALIGNMENT (sizeof(void*))

// Allocation Layout:
// +------------+ Page border
// | 0x00000000 | Leading pages (LEADING_PAGES pages, access protected)
// | ...        |
// | 0x00000000 |
// +------------+ Page border
// | 0x00000000 | Zero padding
// | ...        |
// | 0x00000000 |
// | XXXXXXXXXX | Management data (8-12 bytes)
// | 0x52574152 | No man's land (NOMANSLANDSIZE * 4 bytes of magic)
// | ...        |
// | 0x52574152 |
// | XXXXXXXXXX | Allocated data (iMemSize bytes)
// | ...        |
// | XXXXXXXXXX |
// | 0x00000000 | Alignment padding (iPaddedSize - iMemSize bytes)
// | ...        |
// | 0x00000000 |
// +------------+ Page border
// | 0x00000000 | Trailing pages (TRAILING_PAGES pages, access protected)
// |  ...       |
// | 0x00000000 |
// +------------+ Page border

namespace {
  int GetPageSize()
  {
    static int iPageSize = 0;

    if(iPageSize == 0)
    {
      SYSTEM_INFO info;
      GetSystemInfo(&info);
      iPageSize = info.dwPageSize;
    }

    return iPageSize;
  }

  int GetPageGranularity()
  {
    static int iPageGranularity = 0;

    if(iPageGranularity == 0)
    {
      SYSTEM_INFO info;
      GetSystemInfo(&info);
      iPageGranularity = info.dwAllocationGranularity;
    }

    return iPageGranularity;
  }

  struct VGuardingData
  {
    size_t iMemSize;
    int iAlignment;
    int iNoMansLand[NOMANSLANDSIZE];

    VGuardingData()
    {
      for(int i = 0; i < NOMANSLANDSIZE; i++)
      {
        iNoMansLand[i] = MAGIC;
      }
    }

    void verify()
    {
      for(int i = 0; i < NOMANSLANDSIZE; i++)
      {
        VASSERT_MSG(iNoMansLand[i] == MAGIC, "Invalid memory write detected");
      }
    }
  };

  bool IsGuardedAllocation(void* ptr)
  {
    int iPageSize = GetPageSize();

    void* firstPage = reinterpret_cast<void*>(((reinterpret_cast<size_t>(ptr) - sizeof(VGuardingData)) / iPageSize - LEADING_PAGES) * iPageSize);

    // Our allocations are aligned at page allocation granularity (usually 64KB)
    if(reinterpret_cast<size_t>(firstPage) % GetPageGranularity() != 0)
    {
      return false;
    }

    // The first LEADING_PAGES pages must be noaccess protected
    MEMORY_BASIC_INFORMATION memInfo;
    VirtualQuery(firstPage, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));
    if(memInfo.AllocationProtect != PAGE_NOACCESS || memInfo.RegionSize != GetPageSize() * LEADING_PAGES)
    {
      return false;
    }

    return true;
  }
}

VGuardingMemoryManager::VGuardingMemoryManager()
{
  iLastRetainedAllocation = 0;
  iNumRetainedAllocations = 0;
  pRetainedAllocations = reinterpret_cast<void**>(malloc(iRetainAllocations * sizeof(void*)));
}

VGuardingMemoryManager::~VGuardingMemoryManager()
{
  free(pRetainedAllocations);
}

void* VGuardingMemoryManager::Alloc(size_t iSize)
{
  return AlignedAlloc(iSize, MINALIGNMENT);
}

void* VGuardingMemoryManager::AlignedAlloc(size_t iSize, int iAlignment)
{
  if(iSize == 0)
    iSize = 1;

  if(iAlignment < MINALIGNMENT)
    iAlignment = MINALIGNMENT;

  size_t iPaddedSize = ((iSize - 1) / iAlignment + 1) * iAlignment;

  // Used memory region, including allocation data
  size_t iTotalSize = iPaddedSize + sizeof(VGuardingData);

  int iPageSize = GetPageSize();
  size_t iPages = (iTotalSize - 1) / iPageSize + 1;

  size_t iTotalPages = LEADING_PAGES + iPages + TRAILING_PAGES;

  // Allocate the whole memory region as access protected, but don't commit yet
  char* pLeadingPages = reinterpret_cast<char*>(VirtualAlloc(NULL, iTotalPages * iPageSize, MEM_RESERVE, PAGE_NOACCESS));

  VASSERT_MSG(pLeadingPages != NULL, "Guarding allocator: Reserving a virtual page failed.");

  // Commit and allow access for the memory region between the guard pages
  void* pBase = VirtualAlloc(pLeadingPages + LEADING_PAGES * iPageSize, iPages * iPageSize, MEM_COMMIT, PAGE_READWRITE);
  VASSERT_MSG(pBase != NULL, "Guarding allocator: Committing a virtual page failed.")

  char* pAllocDataMem = pLeadingPages + (LEADING_PAGES + iPages) * iPageSize - iTotalSize;
  char* pUserData = pAllocDataMem + sizeof(VGuardingData);

  VGuardingData* pAllocData = reinterpret_cast<VGuardingData*>(pAllocDataMem);

  new (pAllocData) VGuardingData();

  pAllocData->iAlignment = iAlignment;
  pAllocData->iMemSize = iSize;

  return pUserData;
}

void VGuardingMemoryManager::Free(void* ptr)
{
  return AlignedFree(ptr);
}

void VGuardingMemoryManager::AlignedFree(void* ptr)
{
  if(ptr == NULL)
    return;

  // Fallback in case the pointer wasn't allocated by this memory manager
  if(!IsGuardedAllocation(ptr))
  {
    free(ptr);
    return;
  }

  static VMutex mutex;
  VMutexLocker lock(mutex);

  if(iNumRetainedAllocations == iRetainAllocations)
  {
    void* page = pRetainedAllocations[iLastRetainedAllocation];
    BOOL bFreeSucceeded = VirtualFree(page, 0, MEM_RELEASE);
    VASSERT_MSG(bFreeSucceeded, "Guarding allocator: Releasing a virtual page failed.");
    iNumRetainedAllocations--;
  }

  char* pUserData = reinterpret_cast<char*>(ptr);
  char* pAllocDataMem = pUserData - sizeof(VGuardingData);

  VGuardingData* pAllocData = reinterpret_cast<VGuardingData*>(pAllocDataMem);
  pAllocData->verify();

  size_t iSize = pAllocData->iMemSize;
  int iAlignment = pAllocData->iAlignment;

  int iPageSize = GetPageSize();

  size_t iPaddedSize = ((iSize - 1) / iAlignment + 1) * iAlignment;

  size_t iTotalSize = iPaddedSize + sizeof(VGuardingData);

  size_t iPages = (iTotalSize - 1) / iPageSize + 1;

  size_t iTotalPages = iPages + LEADING_PAGES + TRAILING_PAGES;

  char* pLeadingPages = pAllocDataMem + iTotalSize - (LEADING_PAGES + iPages) * iPageSize;

  // Don't release memory immediately, instead append to the queue and release later
  // to catch accesses of recently freed memory
#pragma warning(push)
#pragma warning(disable:6250) 
  BOOL bFreeSucceeded = VirtualFree(pLeadingPages, 0, MEM_DECOMMIT);
  VASSERT_MSG(bFreeSucceeded, "Guarding allocator: Decommitting a virtual page failed.");
#pragma warning(pop)

  pRetainedAllocations[iLastRetainedAllocation] = pLeadingPages;

  iLastRetainedAllocation = (iLastRetainedAllocation + 1) % iRetainAllocations;

  iNumRetainedAllocations++;
}

size_t VGuardingMemoryManager::MemSize(void* ptr)
{
  return AlignedMemSize(ptr, MINALIGNMENT);
}

size_t VGuardingMemoryManager::AlignedMemSize(void* ptr, int iAlignment)
{
  if(ptr == NULL)
    return 0;

  if(!IsGuardedAllocation(ptr))
  {
    return 0;
  }

  VGuardingData* pAllocData = reinterpret_cast<VGuardingData*>(ptr) - 1;
  pAllocData->verify();

  return pAllocData->iMemSize;
}

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
