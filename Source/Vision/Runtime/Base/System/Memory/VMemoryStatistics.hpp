/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VMEMORYSTATISTICS_H__
#define VMEMORYSTATISTICS_H__

/// \brief
///   Structure that holds current memory statistics. Can be passed to global function VBaseMem_GetStats
struct VMemoryStatistics_t
{
  VMemoryStatistics_t() {Reset();}

  inline void Reset()
  {
    memset(this,0,sizeof(VMemoryStatistics_t));
  }

  /// \brief
  ///   Sorts the passed size into the m_iAllocationOfSize array
  inline void AddAllocationOfSize(unsigned int iSize)
  {
    unsigned int iRef = 1;
    for (int iIndex=0;iIndex<32;iIndex++,iRef<<=1)
      if (iRef>=iSize)
      {
        m_iAllocationOfSize[iIndex]++;
        break;
      }
  }

  unsigned int m_iTotalNumAllocations; ///< Incremented by one for every allocation call
  unsigned int m_iTotalNumFree;        ///< Incremented by one for every de-allocation call

  __int64 m_iTotalByteAlloc;  ///< Sum of bytes that have been allocated so far
  __int64 m_iTotalByteFree;   ///< Sum of bytes that have been de-allocated so far

  unsigned int m_iAllocationOfSize[32]; // index 0 counts allocations of size 0-1, then 2-3, 4-7, 8-15 etc.
};

#ifndef NO_NEW_OVERRIDE
VBASE_IMPEXP void VBaseMem_GetStats(VMemoryStatistics_t &dest);
#else
inline void VBaseMem_GetStats(VMemoryStatistics_t &dest) {}
#endif

#endif // VMEMORYSTATISTICS_H__

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
