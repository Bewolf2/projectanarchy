/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkaiNavMesh::EdgeIndex HK_CALL hkaiNavMeshUtils::getNextEdgeIndex( const hkaiNavMesh::Face& face, EdgeIndex edgeIndex )
{
	HK_ASSERT(0x1f12add8, edgeIndex >= face.m_startEdgeIndex && edgeIndex < face.m_startEdgeIndex + face.m_numEdges);
	const hkaiNavMesh::EdgeIndex nextEdgeIndex = (edgeIndex+1 == face.m_startEdgeIndex + face.m_numEdges) ? face.m_startEdgeIndex : (edgeIndex + 1) ;
	return nextEdgeIndex;
}

HK_FORCE_INLINE hkaiNavMesh::EdgeIndex HK_CALL hkaiNavMeshUtils::getPreviousEdgeIndex(  const hkaiNavMesh::Face& face, EdgeIndex edgeIndex )
{
	HK_ASSERT(0x1f12add8, edgeIndex >= face.m_startEdgeIndex && edgeIndex < face.m_startEdgeIndex + face.m_numEdges);
	const hkaiNavMesh::EdgeIndex prevEdgeIndex = (edgeIndex == face.m_startEdgeIndex ) ? face.m_startEdgeIndex + face.m_numEdges - 1 : (edgeIndex - 1);
	return prevEdgeIndex;
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
