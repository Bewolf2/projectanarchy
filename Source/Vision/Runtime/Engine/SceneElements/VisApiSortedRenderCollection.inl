/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSortedRenderCollection.inl

inline unsigned int VisRenderCollection_cl::GetNumEntries() const
{
  return m_iNumEntries;
}

inline BOOL VisRenderCollection_cl::IsEmpty() const
{
  return m_iNumEntries==0;
}

inline void **VisRenderCollection_cl::GetDataPtr() const
{
  return m_pEntries;
}



inline void* VisRenderCollection_cl::GetEntry(unsigned int iIndex) const
{
  VASSERT(iIndex<m_iSize);
  VASSERT(m_pEntries[iIndex]); ///< 0 resp NULL pointers are not allowed!
  return m_pEntries[iIndex];
}

inline void VisRenderCollection_cl::AppendEntryFast(void *pValue)
{
  VASSERT(m_iNumEntries<m_iSize);
  VASSERT(pValue); ///< 0 resp NULL pointers are not allowed!
  m_pEntries[m_iNumEntries++] = pValue;
}

inline void VisRenderCollection_cl::AppendEntry(void *pValue)
{
  VASSERT(pValue); ///< 0 resp NULL pointers are not allowed!

  unsigned int iSizeAdd = m_iNumEntries/4;
  if (iSizeAdd < m_iMemGranularity)
    iSizeAdd = m_iMemGranularity;

  if (m_iNumEntries >= m_iSize)
    Resize(m_iSize + iSizeAdd); 
  m_pEntries[m_iNumEntries++] = pValue;
}

inline unsigned int VisRenderCollection_cl::GetSize() const
{
  return m_iSize;
}

inline unsigned int VisRenderCollection_cl::GetGranularity() const
{
  return m_iMemGranularity;
}

inline void VisRenderCollection_cl::SetGranularity(unsigned int iGranularity)
{
  VASSERT(iGranularity > 0);
  m_iMemGranularity = iGranularity;
}


inline void VisRenderCollection_cl::EnsureSize(unsigned int iMinSize)
{
  if (iMinSize<=m_iSize) return;
  Resize(iMinSize);
}



inline void VisRenderCollection_cl::Clear()
{
  m_iNumEntries = 0;
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
