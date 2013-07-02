/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Geometry/Internal/Algorithms/Distance/hkcdDistancePointLine.h>
#include <Geometry/Internal/Algorithms/Distance/hkcdDistanceSegmentSegment.h>

hkaiNavMeshFaceGraph::hkaiNavMeshFaceGraph()
:	m_collection(HK_NULL),
	m_costModifier(HK_NULL),
	m_edgeFilter(HK_NULL),
	m_searchState(HK_NULL),
	m_searchSphereRadius(-hkSimdReal_1),
	m_searchCapsuleRadius(-hkSimdReal_1)
{
}

inline hkBool32 hkaiNavMeshFaceGraph::isGoal( SearchIndex nit ) const
{
	
	return false;
}

int hkaiNavMeshFaceGraph::getNumNodes() const
{
	
	HK_ASSERT(0x73370915, m_collection->getNumInstances() == 1);
	return m_collection->getInstanceAt(0)->getNumFaces();
}

// Get an upper bound on the number of nodes adjacent to a given node.
int hkaiNavMeshFaceGraph::getMaxNeighborCount( SearchIndex nit ) const
{
	const hkaiNavMesh::Face& face = m_collection->getFaceFromPacked(nit);
	const int numRealEdges = (face.m_numEdges);
	return numRealEdges;
}

void hkaiNavMeshFaceGraph::getNeighbors( SearchIndex nit, hkArrayBase< EdgeKey >& neighbors ) const
{
	const hkaiNavMesh::Face& face = m_collection->getFaceFromPacked(nit);

	const int numRealEdges = (face.m_numEdges);

	HK_ASSERT(0x66c7197b, neighbors.isEmpty());
	HK_ASSERT2(0x79b7d4c0, neighbors.getCapacity() >= numRealEdges, 
		"For performance, neighbors array should be pre-allocated to contain all possible neighbors.");
	neighbors.setSizeUnchecked(numRealEdges);
	EdgeKey* HK_RESTRICT neighborPtr = neighbors.begin();

	const hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(nit);
	for ( int e=0; e < numRealEdges; e++ )
	{
		const EdgeKey edgeKey = hkaiGetPackedKey( sectionId, face.m_startEdgeIndex + e);
		*neighborPtr++ = edgeKey;
	}
}

hkaiNavMeshFaceGraph::SearchIndex hkaiNavMeshFaceGraph::edgeTarget( SearchIndex nit, EdgeKey edgeKey ) const
{
	const hkaiNavMeshInstance* instance = m_collection->getInstanceAt( hkaiGetRuntimeIdFromPacked(edgeKey) );
	return instance->getOppositeFaceKeyForEdge( hkaiGetIndexFromPacked(edgeKey) );
}

hkaiNavMeshFaceGraph::EdgeCost hkaiNavMeshFaceGraph::getTotalCost( SearchIndex nit, SearchIndex adj, EdgeKey eit, const EdgeCost costToParent ) const
{
	Position centroidA; getPosition(nit, centroidA);
	Position centroidB; getPosition(adj, centroidB);

	// The setup looks something like this:
	//		 centroidB
	//			 ^
	//			 |
	//	B < -----+------- A  edge
	//			 |
	//		   <-| plane
	//			 |
	//		 centroidA
	//

	// We could just take the distance between the face centroids
	// But if the segment between the segment doesn't pass though the edge, we should split the segment into 2 parts:
	//	centroidA->edge  + edge->centroidB

	HK_ASSERT(0x6886c84f, hkaiGetRuntimeIdFromPacked(nit) == hkaiGetRuntimeIdFromPacked(eit) );
	const hkaiNavMeshInstance* meshInstance = m_collection->getInstanceAt( hkaiGetRuntimeIdFromPacked(eit) );
	const hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(eit);
	const hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(nit);
	
	hkVector4 edgeA, edgeB;
	meshInstance->getEdgePoints( edgeIndex, edgeA, edgeB );

	hkVector4 faceNormal; hkaiNavMeshUtils::calcFaceNormal(*meshInstance, faceIndex, faceNormal);

	hkVector4 dir; dir.setSub(centroidB, centroidA);
	hkVector4 plane; plane.setCross(faceNormal, dir);
	// Don't need to normalize the plane since we're comparing distances against 0
	//plane.normalize<3>();
	plane.setPlaneConstant(centroidA);

	hkSimdReal distanceA = plane.dot4xyz1(edgeA);
	hkSimdReal distanceB = plane.dot4xyz1(edgeB);

	// If the segment passes through the edge, then A is on the negative side of the plane, and B is on the positive
	hkSimdReal euclideanDistance;
	hkVector4 pointOnEdge;
	if (distanceA.isGreaterZero() || distanceB.isLessZero() )
	{
		pointOnEdge.setSelect( distanceA.greaterZero(), edgeA, edgeB );
		euclideanDistance = distanceFromPosition(centroidA, pointOnEdge) +  distanceFromPosition(pointOnEdge, centroidB);
	}
	else
	{
		euclideanDistance = dir.length<3>();
		pointOnEdge.setInterpolate( edgeA, edgeB, hkSimdReal_Half);
	}

	if (!m_costModifier)
	{
		return costToParent + euclideanDistance.getReal();
	}
	else
	{
		hkaiNavMeshEdgePairInfo edgeInfo;
		edgeInfo.setFace( nit, &meshInstance->getFace( faceIndex ), meshInstance->getFaceDataPtr(faceIndex) );
		edgeInfo.m_outgoingEdgeInfo.setEdge( eit, &meshInstance->getEdge(edgeIndex), meshInstance->getEdgeDataPtr(edgeIndex), pointOnEdge );

		hkaiAstarCostModifier::NavMeshGetModifiedCostCallbackContext callbackContext( meshInstance,
			m_info,
			edgeInfo,
			(hkaiNavMeshEdgePairInfo::QUERY_TYPE_ASTAR | hkaiNavMeshEdgePairInfo::QUERY_INTERMEDIATE_POINT),
			costToParent,
			euclideanDistance.getReal()
			);
		return costToParent + m_costModifier->getModifiedCost( callbackContext );
	}

}

hkBool32 hkaiNavMeshFaceGraph::isEdgeTraversable( SearchIndex nit, SearchIndex adj, EdgeKey eit )
{
	const hkaiNavMesh::Edge& edge = m_collection->getEdgeFromPacked(eit);
	if (!edge.isTraversable())
	{
		return false;
	}
	
	hkVector4 edgeA, edgeB;

	// NB - this is the instance for nit and eit, but not necessarily adj
	const hkaiNavMeshInstance* meshInstance = m_collection->getInstanceAt( hkaiGetRuntimeIdFromPacked(eit) );
	const hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(eit);
	const hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(nit);
	meshInstance->getEdgePoints(edgeIndex, edgeA, edgeB);

	// Sphere check
	if (m_searchSphereRadius.isGreaterEqualZero())
	{

		// We want to allow and face that's within the radius of the start point.
		// We could get the closest point on the face, and compare it against the start.
		// But it's equivalent to check the edge that we're crossing now.
		hkSimdReal distanceSqToEdge = hkcdPointSegmentDistanceSquared(m_startPoint, edgeA, edgeB);

		if ( distanceSqToEdge > m_searchSphereRadius*m_searchSphereRadius )
		{
			return false;
		}
	}


	// Capsule check
	if (m_searchCapsuleRadius.isGreaterEqualZero())
	{
		hkVector4 dab; dab.setSub(edgeB, edgeA);

		int i = 0;
		for (; i < m_numGoals; i++)
		{
			hkVector4 dse; dse.setSub(m_endPoints[i], m_startPoint);

			hkSimdReal distanceSquared = hkcdDistanceSegmentSegment(m_startPoint, dse, edgeA, dab);

			if (distanceSquared <= m_searchCapsuleRadius*m_searchCapsuleRadius)
			{
				break;
			}

		}
		if (i == m_numGoals)
		{
			return false;
		}
	}
	
	if (!m_edgeFilter)
	{
		return true;
	}

	// We don't fill out the "incoming" edge edgeInfo.
	// The face edgeInfo that is specified is for the current (nit) face.
	hkaiNavMeshEdgePairInfo edgeInfo;
	hkVector4 pos; pos.setInterpolate( edgeA, edgeB, hkSimdReal_Half );

	edgeInfo.setFace( nit, &meshInstance->getFace( faceIndex ), meshInstance->getFaceDataPtr(faceIndex) );
	edgeInfo.m_outgoingEdgeInfo.setEdge( eit, &meshInstance->getEdge(edgeIndex), meshInstance->getEdgeDataPtr(edgeIndex), pos );

	hkaiAstarEdgeFilter::NavMeshIsEnabledCallbackContext callbackContext( meshInstance,
		m_info,
		edgeInfo,
		(hkaiNavMeshEdgePairInfo::QUERY_TYPE_ASTAR | hkaiNavMeshEdgePairInfo::QUERY_INTERMEDIATE_POINT)
		);
	return m_edgeFilter->isEnabled(callbackContext);
}

hkBool32 hkaiNavMeshFaceGraph::isValidEdgeTarget( SearchIndex eit ) const
{
	return (eit != HKAI_INVALID_PACKED_KEY);
}

void hkaiNavMeshFaceGraph::getPositionForHeuristic( SearchIndex a, hkVector4& vecOut ) const
{
	getPosition(a, vecOut);
}

void hkaiNavMeshFaceGraph::getPosition( SearchIndex a, hkVector4& vecOut ) const
{
	const hkaiNavMeshInstance* meshInstance = m_collection->getInstanceAt( hkaiGetRuntimeIdFromPacked(a) );
	hkaiNavMeshUtils::calcFaceCentroid( *meshInstance, hkaiGetIndexFromPacked(a) , vecOut );
}

hkSimdReal hkaiNavMeshFaceGraph::distance( SearchIndex A, SearchIndex B ) const
{
	hkVector4 posA; getPosition(A, posA);
	hkVector4 posB; getPosition(B, posB);
	return posA.distanceTo( posB );
}

hkSimdReal hkaiNavMeshFaceGraph::distanceFromPosition( hkVector4Parameter posA, hkVector4Parameter posB ) const
{
	return posA.distanceTo( posB );
}

void hkaiNavMeshFaceGraph::nextNode( SearchIndex nid )
{
	m_searchState->nextNode( nid );
}

void hkaiNavMeshFaceGraph::nextEdge( SearchIndex nid, EdgeKey ekey )
{
	m_searchState->nextEdge( edgeTarget(nid, ekey) );
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
