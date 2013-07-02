/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_PLATFORM_SPU
inline void hkaiNavVolumeInstance::setOriginalPointers( const hkaiNavVolume* vol )
{
	m_originalCells		= vol->m_cells.begin();
	m_numOriginalCells	= vol->m_cells.getSize();

	m_originalEdges		= vol->m_edges.begin();
	m_numOriginalEdges	= vol->m_edges.getSize();

}

inline void hkaiNavVolumeInstance::setSectionUid( hkaiSectionUid uid )
{
	m_sectionUid = uid;
}

inline void hkaiNavVolumeInstance::setRuntimeId( hkaiRuntimeIndex id)
{
	m_runtimeId = id;
}

inline hkaiPackedKey hkaiNavVolumeInstance::getOppositeCellKeyForEdge( const hkaiNavVolume::Edge& edge ) const
{
	return hkaiGetOppositePackedKey( (hkUint8) edge.m_flags.get(), getRuntimeId(), edge.getOppositeCellKeyUnchecked() );
}

inline const hkaiNavVolume::Edge& hkaiNavVolumeInstance::getEdge( EdgeIndex e ) const
{ 
	HK_ASSERT(0x557b576c, e >= 0 && e < m_numOriginalEdges + m_ownedEdges.getSize() );

	const int numOriginalEdges = m_numOriginalEdges;
	if (e < numOriginalEdges)
	{
		return m_originalEdges[e];
	}
	else
	{
		int idx = e - numOriginalEdges;
		return m_ownedEdges[idx];
	}
}

HK_FORCE_INLINE const hkaiNavVolume::Cell& hkaiNavVolumeInstance::getCell( CellIndex cellIndex ) const
{
	HK_ASSERT(0x6857120e, cellIndex >= 0 && cellIndex < m_numOriginalCells );
	return m_originalCells[cellIndex];
}

inline void hkaiNavVolumeInstance::getInstancedCell( CellIndex n, CellInstance& cellOut ) const
{
	int mappedIdx = m_cellMap.getSize() ? m_cellMap[n] : -1;

	// Not instanced
	if (mappedIdx == -1) 
	{
		cellOut.m_startEdgeIndex = -1;
		cellOut.m_numEdges = 0;
	}
	else
	{
		cellOut = m_instancedCells[mappedIdx];
	}
}

inline hkaiNavVolumeInstance::CellInstance& hkaiNavVolumeInstance::getWritableCellInstance( CellIndex n )
{
	HK_ASSERT2(0x390b0eba, m_cellMap.getSize() == m_numOriginalCells, "Can't instance cell if hkaiNavVolumeInstance::tempInit was used" );

	int mappedIndex = m_cellMap[n];
	if (mappedIndex == -1)
	{
		mappedIndex = m_instancedCells.getSize();
		m_cellMap[n] = mappedIndex;
		CellInstance& newCell = m_instancedCells.expandOne();
		newCell.m_numEdges = 0;
		newCell.m_startEdgeIndex = -1;
	}

	return m_instancedCells[mappedIndex];
}

inline hkaiNavVolume::Edge& hkaiNavVolumeInstance::getWritableEdge( EdgeIndex e )
{
	HK_ASSERT2(0x2a785951, e >= m_numOriginalEdges, "Can't get write access to an original edge.");
	int idx = e - m_numOriginalEdges;
	return m_ownedEdges[idx];
}

inline void hkaiNavVolumeInstance::getOffsetAndScale( hkVector4& offsetOut, hkVector4& scaleOut ) const
{
	hkVector4 originalScale, originalOffset;
	getOriginalVolume()->getOffsetAndScale(originalOffset, originalScale);
	
	offsetOut.setAdd(originalOffset, m_translation);
	scaleOut = originalScale;
}

inline void hkaiNavVolumeInstance::getAabb( hkAabb& aabbOut ) const
{
	hkAabb localAabb = getOriginalVolume()->getAabb();
	aabbOut.m_min.setAdd(localAabb.m_min, m_translation);
	aabbOut.m_max.setAdd(localAabb.m_max, m_translation);
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
