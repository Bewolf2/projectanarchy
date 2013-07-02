/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



inline hkaiNavVolumeCellPairInfo::CellInfo::CellInfo()
:	m_cellKey(HKAI_INVALID_PACKED_KEY),
	m_cell(HK_NULL)
{
}

inline hkaiNavVolumeCellPairInfo::hkaiNavVolumeCellPairInfo()
:	m_edgeKey(HKAI_INVALID_PACKED_KEY),
	m_edge(HK_NULL)
{

}

inline void hkaiNavVolumeCellPairInfo::CellInfo::validate( const hkaiNavVolumeAccessor* volume ) const
{
#ifdef HK_DEBUG
	HK_ASSERT(0x630f01f0, m_cellKey == HKAI_INVALID_PACKED_KEY || ( m_cell ) );

	if (volume && m_cell)
	{
		const hkaiNavVolume::Cell& cell = volume->getCell( hkaiGetIndexFromPacked(m_cellKey) );
		HK_ASSERT(0x334051d7, cell.m_numEdges == m_cell->m_numEdges );
		HK_ASSERT(0x334051d7, cell.m_startEdgeIndex== m_cell->m_startEdgeIndex );
		HK_ASSERT(0x334051d7, (cell.m_min[0] == m_cell->m_min[0])  && (cell.m_min[1] == m_cell->m_min[1]) && (cell.m_min[2] == m_cell->m_min[2]) );
		HK_ASSERT(0x334051d7, (cell.m_max[0] == m_cell->m_max[0])  && (cell.m_max[1] == m_cell->m_max[1]) && (cell.m_max[2] == m_cell->m_max[2]) );

	}
#endif
}

inline void hkaiNavVolumeCellPairInfo::validate( const hkaiNavVolumeAccessor* accessor ) const
{
#ifdef HK_DEBUG
	HK_ASSERT(0x56f31f14, (m_edgeKey != HKAI_INVALID_PACKED_KEY) == (m_edge != HK_NULL) );
	m_currentCellInfo.validate( accessor );
	m_adjacentCellInfo.validate( /* can't validate adjacent since it might belong to a different volume */ );

	// Make sure the cells aren't the same (unless the edge is NULL)
	HK_ASSERT(0x2ad0e697, (m_currentCellInfo.m_cellKey != m_adjacentCellInfo.m_cellKey) || m_edgeKey == HKAI_INVALID_PACKED_KEY);
	HK_ASSERT(0x6ec88eb1, (m_currentCellInfo.m_cell != m_adjacentCellInfo.m_cell) || m_edge == HK_NULL);

	if (accessor && m_edge)
	{
		const hkaiNavVolume::Edge& edge = accessor->getEdge( hkaiGetIndexFromPacked(m_edgeKey) );
		HK_ASSERT(0x5a3fef83, edge.m_flags == m_edge->m_flags);
		HK_ASSERT(0x5a3fef83, accessor->getOppositeCellKeyForEdge(edge) == m_edge->m_oppositeCell);
	}
#endif
}

inline void hkaiNavVolumeCellPairInfo::CellInfo::setCell(hkaiPackedKey eIdx, const hkaiNavVolume::Cell* cell, hkVector4Parameter pos)
{
	m_cellKey = eIdx;
	m_cell = cell;
	m_position = pos;
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
