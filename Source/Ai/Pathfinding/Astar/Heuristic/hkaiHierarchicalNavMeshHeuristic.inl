/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// Note: "cluster" and "chunk" are used interchangeably below.



inline hkaiHierarchicalNavMeshHeuristic::hkaiHierarchicalNavMeshHeuristic( char* openSetStorage, int openSetStorageCount,  char* searchStorage, int searchStorageCount)
:	m_coarseSearch(openSetStorage, openSetStorageCount, searchStorage, searchStorageCount),
	m_accessor(HK_NULL)
{
}

inline hkSimdReal hkaiHierarchicalNavMeshHeuristic::getMinDistanceToGoals( hkVector4Parameter edgePosition ) const
{
	hkSimdReal minDistance = hkSimdReal_Max;

	// Check each goal, choose the one with the closest Euclidean distance
	for (int iGoal=0; iGoal < m_numGoals; iGoal++)
	{
		hkSimdReal distance_i = edgePosition.distanceTo(m_goalPositions[iGoal]);
		minDistance.setMin(minDistance, distance_i);
	}

	return minDistance;
}

inline const hkaiNavMesh::Edge& hkaiHierarchicalNavMeshHeuristic::getEdge( hkaiPackedKey edgeId ) const
{
	// We know that the outgoing edge we're looking at in the graph is the current edge
	return m_originalGraph->getCachedOutgoingEdge(edgeId);
}


inline hkaiHierarchicalNavMeshHeuristic::ClusterKey hkaiHierarchicalNavMeshHeuristic::getClusterForFace( FaceKey faceKey ) const
{	
	if (faceKey == HKAI_INVALID_PACKED_KEY)
	{
		return HKAI_INVALID_PACKED_KEY;
	}
	const hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked( faceKey );
	const hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(faceKey);
	
	
	m_accessor.setSection(sectionId);
	const hkaiNavMesh::Face& face = m_accessor->getFace( faceIndex );
	return getClusterForFace(face, sectionId);
}

inline hkaiHierarchicalNavMeshHeuristic::ClusterKey 
hkaiHierarchicalNavMeshHeuristic::getClusterForFace(const hkaiNavMesh::Face& face, hkaiRuntimeIndex sectionId) const
{
	const int clusterIndex = face.m_clusterIndex;
	if (clusterIndex == -1)
	{
		return HKAI_INVALID_PACKED_KEY;
	}
	return hkaiGetPackedKey(sectionId, clusterIndex);
}

inline void hkaiHierarchicalNavMeshHeuristic::getEdgePosition( EdgeKey edgeId, hkVector4& posOut ) const
{
	m_originalGraph->getPositionForHeuristic(edgeId, posOut);
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
