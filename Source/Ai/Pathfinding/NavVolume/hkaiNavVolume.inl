/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

hkaiNavVolume::hkaiNavVolume()
{
	m_aabb.setEmpty();
	m_quantizationOffset.setZero();
	m_quantizationScale.setZero();
	m_userData = 0;
}

inline const hkaiNavVolume::Edge& hkaiNavVolume::getEdge( int idx ) const
{ 
	return m_edges[idx];
}

inline hkaiNavVolume::Edge& hkaiNavVolume::getEdge( int idx )
{
	return m_edges[idx];
}

inline int hkaiNavVolume::getNumEdges() const
{
	return m_edges.getSize();
}

inline int hkaiNavVolume::getNumCells() const 
{ 
	return m_cells.getSize(); 
}

inline const hkaiNavVolume::Cell& hkaiNavVolume::getCell(CellIndex cellIndex) const 
{ 
	return m_cells[cellIndex]; 
}

hkaiNavVolume::Cell& hkaiNavVolume::getCell(CellIndex cellIndex)
{ 
	return m_cells[cellIndex]; 
}

inline const hkAabb& hkaiNavVolume::getAabb() const 
{ 
	return m_aabb;
}

inline void hkaiNavVolume::getOffsetAndScale(hkVector4& offsetOut, hkVector4& scaleOut ) const
{
	offsetOut = m_quantizationOffset;
	scaleOut = m_quantizationScale;
}

inline const hkVector4& hkaiNavVolume::getQuantizationOffset() const 
{ 
	return m_quantizationOffset;
}

inline const hkVector4& hkaiNavVolume::getQuantizationScale() const
{
	return m_quantizationScale;
}

inline const int* hkaiNavVolume::getResolution() const
{
	return m_res;
}

///////////////////////////////////////////////////////////////////////////////
// Edge methods
///////////////////////////////////////////////////////////////////////////////

inline hkaiPackedKey hkaiNavVolume::Edge::getOppositeCellKeyUnchecked() const
{
	return m_oppositeCell;
}

inline hkaiNavVolume::CellIndex hkaiNavVolume::Edge::getOppositeCellIndex() const
{
	return hkaiGetIndexFromPacked(m_oppositeCell);
}

inline void hkaiNavVolume::Edge::init() 
{
	m_flags.clear();
	m_oppositeCell = HKAI_INVALID_PACKED_KEY;
}

inline hkBool32 hkaiNavVolume::Edge::isTraversable() const
{
	return true;
}

inline hkBool32 hkaiNavVolume::Edge::isExternal() const
{
	return m_flags.anyIsSet( EDGE_EXTERNAL_OPPOSITE );
}

inline void hkaiNavVolume::Edge::setTargetKey( hkaiPackedKey key)
{
	m_oppositeCell = key;
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
