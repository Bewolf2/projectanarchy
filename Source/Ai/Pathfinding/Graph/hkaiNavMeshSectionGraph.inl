/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Geometry/Internal/Algorithms/Distance/hkcdDistancePointLine.h>
#include <Geometry/Internal/Algorithms/Distance/hkcdDistanceSegmentSegment.h>

#ifdef HK_PLATFORM_SPU
hkaiNavMeshSectionGraph::hkaiNavMeshSectionGraph()
:	m_streamingInfo(HK_NULL),
	m_startFaceKey(HKAI_INVALID_PACKED_KEY),
	m_numGoals(0),
	m_searchSphereRadius(-1.f),
	m_searchCapsuleRadius(-1.f),
	m_costModifier(HK_NULL)
{
	m_cachedIncomingSectionId = -1;
	m_cachedOutgoingSectionId = -1;
	m_searchState = HK_NULL;
}


#else
hkaiNavMeshSectionGraph::hkaiNavMeshSectionGraph()
:	m_streamingInfo(HK_NULL),
	m_startFaceKey(HKAI_INVALID_PACKED_KEY),
	m_numGoals(0),
	m_searchSphereRadius(-hkSimdReal_1),
	m_searchCapsuleRadius(-hkSimdReal_1),
	m_costModifier(HK_NULL),
	m_edgeFilter(HK_NULL),
	m_cachedIncomingNavMesh(HK_NULL),
	m_cachedOutgoingNavMesh(HK_NULL)
{
	m_cachedIncomingSectionId = -1;
	m_cachedOutgoingSectionId = -1;
	m_searchState = HK_NULL;
	m_flags = 0;
}
#endif

inline void hkaiNavMeshSectionGraph::init( 
	const hkaiStreamingCollection::InstanceInfo* streamingInfo, 
	hkVector4Parameter up, 
	const hkaiAstarCostModifier* costModifier,
	const hkaiAstarEdgeFilter* edgeFilter)
{
	m_streamingInfo = streamingInfo;
	m_up = up;
	HK_ASSERT(0x5e1ccfa7, m_up.isNormalized<3>() );
	m_costModifier = costModifier;
	m_edgeFilter = edgeFilter;
}

inline void hkaiNavMeshSectionGraph::setStartPointAndFace( hkVector4Parameter startPoint, hkaiPackedKey startFaceKey )
{
	m_startPoint = startPoint;
	m_startFaceKey = startFaceKey;
}

inline void hkaiNavMeshSectionGraph::setGoalPointsAndFaces( int numGoals, const hkVector4* goalPoints, const hkaiPackedKey* goalFaceKeys )
{
	m_numGoals = numGoals;
	m_endPoints = goalPoints;
	m_endFaceKeys = goalFaceKeys;
}

inline void hkaiNavMeshSectionGraph::setRadiusLimits( hkReal sphereRadius, hkReal capsuleRadius, bool project)
{
	hkUint32 flags = 0;
	m_searchSphereRadius.setFromFloat( sphereRadius );
	if (sphereRadius >= 0.0f )
	{
		flags = flags | (hkUint32) (CHECK_SPHERE_RADIUS);
	}

	m_searchCapsuleRadius.setFromFloat( capsuleRadius );
	if (capsuleRadius >= 0.0f )
	{
		flags = flags | (hkUint32) (CHECK_CAPSULE_RADIUS);
	}

	if(project) 
	{
		flags = flags | (hkUint32)(PROJECT_RADIUS_CHECKS);
	}

	hkUint32 mask = (CHECK_SPHERE_RADIUS | CHECK_CAPSULE_RADIUS | PROJECT_RADIUS_CHECKS);
	m_flags = (m_flags & ~mask) | flags;
}

inline void hkaiNavMeshSectionGraph::setTraversalInfo ( 
	const hkaiAgentTraversalInfo& info, hkReal simpleSmoothingThreshold, 
	hkaiNavMeshPathSearchParameters::UserEdgeTraversalTestType userEdgeTraversalTestType, bool useGrandparentDistance)
{
	m_info = info;
	if ( m_info.m_diameter > 2.0f * simpleSmoothingThreshold )
	{
		m_flags = m_flags | (hkUint32) (CHECK_TRAVERSAL_RADIUS);

		if ( userEdgeTraversalTestType == hkaiNavMeshPathSearchParameters::USER_EDGE_TRAVERSAL_TEST_ENABLED )
		{
			m_flags = m_flags | (hkUint32) (CHECK_USER_EDGE_TRAVERSAL_RADIUS);
		}
	}
	if (useGrandparentDistance)
	{
		m_flags = m_flags | (hkUint32) (USE_GRANDPARENT_DISTANCE_CALCULATION);
	}
}

void hkaiNavMeshSectionGraph::setSearchState(SearchState* ss)
{
	m_searchState = ss;
}

void hkaiNavMeshSectionGraph::resetFlags()
{
	m_flags = 0;
}


inline const hkaiStreamingCollection::InstanceInfo* hkaiNavMeshSectionGraph::getStreamingInfo() const
{
	return m_streamingInfo;
}

inline const hkVector4& hkaiNavMeshSectionGraph::getUp() const
{
	return m_up;
}

inline const hkaiAstarCostModifier* hkaiNavMeshSectionGraph::getCostModifier() const
{
	return m_costModifier;
}

inline const hkaiAgentTraversalInfo& hkaiNavMeshSectionGraph::getTraversalInfo() const
{
	return m_info;
}


inline hkBool32 hkaiNavMeshSectionGraph::isStart( SearchIndex nit ) const
{
	return (hkaiGetRuntimeIdFromPacked(nit) == HKAI_START_NODE_SECTION_ID);
}

inline hkBool32 hkaiNavMeshSectionGraph::isGoal( SearchIndex nit ) const
{
	return (hkaiGetRuntimeIdFromPacked(nit) == HKAI_GOAL_NODE_SECTION_ID);
}

// Get an upper bound on the number of nodes adjacent to a given node.
int hkaiNavMeshSectionGraph::getMaxNeighborCount( SearchIndex nit ) const
{
	const hkaiNavMesh::Face& face = getCachedFace();
	// In the worst case, all goal points could be in the target face
	int maxNumNeighbors = (face.m_numEdges + face.m_numUserEdges + m_numGoals);
	return maxNumNeighbors;
}

void hkaiNavMeshSectionGraph::getNeighbors( SearchIndex incomingEdgePacked, hkArrayBase< EdgeKey >& neighborEdgesPacked ) const
{
	if ( isGoal(incomingEdgePacked) )
	{
		// This is a goal pseudo-node, expand no more neighbors.
		return;
	}

	const hkaiNavMesh::Face& face = getCachedFace();
	
	const int numEdges = face.m_numEdges;
	const int numUserEdges = face.m_numUserEdges;
	const int numGoals = m_numGoals;
	const int numEdgeNeighbors = (numEdges + numUserEdges);
	HK_ON_DEBUG(const int maxNumNeighbors = (numEdgeNeighbors + numGoals));

	HK_ASSERT(0x6001f1ed, neighborEdgesPacked.isEmpty());
	HK_ASSERT2(0x53746f00, neighborEdgesPacked.getCapacity() >= maxNumNeighbors, 
		"For performance, neighbors array should be pre-allocated to contain all possible neighbors.");
	neighborEdgesPacked.setSizeUnchecked(numEdgeNeighbors);
	EdgeKey* HK_RESTRICT neighborPtr = neighborEdgesPacked.begin();
	
	const int sectionId = m_cachedOutgoingSectionId;
	const int startEdgeIndex = face.m_startEdgeIndex;
	for ( int e=0; e < numEdges; e++ )
	{
		const hkaiNavMesh::EdgeIndex edgeIndex = (startEdgeIndex + e);
		const EdgeKey packedEdgeKey = hkaiGetPackedKey( sectionId, edgeIndex );
		*neighborPtr++ = packedEdgeKey;
	}
	const int startUserEdgeIndex = face.m_startUserEdgeIndex;
	for ( int e=0; e < numUserEdges; e++ )
	{
		const hkaiNavMesh::EdgeIndex edgeIndex = (startUserEdgeIndex + e);
		const EdgeKey packedEdgeKey = hkaiGetPackedKey( sectionId, edgeIndex );
		*neighborPtr++ = packedEdgeKey;
	}

	// For every goal point that is in the target face, add a goal pseudo-node to the neighbors list.
	const hkaiPackedKey faceKey = m_cachedFaceKey;
	const hkaiPackedKey* const HK_RESTRICT goalFaceKeys = m_endFaceKeys;
	for ( int iGoal=0; iGoal < numGoals; iGoal++ )
	{
		const hkaiPackedKey goalFaceKey = goalFaceKeys[iGoal];
		if ( goalFaceKey == faceKey )
		{
			// Create a key for a pseudo node: a special mark to indicate a goal node, and the goal index.
			const hkaiPackedKey goalPseudoNodeKey = hkaiGetPackedKey( HKAI_GOAL_NODE_SECTION_ID, iGoal );
			neighborEdgesPacked.pushBackUnchecked( goalPseudoNodeKey );
		}
	}
}

hkaiNavMeshSectionGraph::SearchIndex hkaiNavMeshSectionGraph::edgeTarget( SearchIndex nit, EdgeKey eit ) const
{
	return eit;
}

hkaiNavMeshSectionGraph::EdgeCost hkaiNavMeshSectionGraph::getTotalCost( SearchIndex incomingEdge, SearchIndex outgoingEdge, EdgeKey eit_unused, EdgeCost costToParent ) const
{
	

	HK_ASSERT(0x4f1dd1d1, m_cachedIncomingEdgeIndex == incomingEdge);
	HK_ASSERT(0x60adbc9c, m_cachedOutgoingEdgeIndex == outgoingEdge);

	hkSimdReal incomingCost = hkSimdReal::fromFloat(costToParent);
	const hkSimdReal originalIncomingCost = incomingCost;
	Position incomingPos, outgoingPos;

	incomingPos = m_cachedIncomingEdgeMidpoint;
	outgoingPos = m_cachedOutgoingEdgeMidpoint;

	const hkBool32 incomingIsStart = isStart( incomingEdge );
	const hkBool32 incomingIsUserEdge = m_cachedIncomingEdge.isUserEdge();
	const hkBool32 useGrandparentCalc = ( m_flags & (hkUint32) USE_GRANDPARENT_DISTANCE_CALCULATION );

	// Returns 0 for non-user edges.
	hkSimdReal userEdgeCost = m_cachedOutgoingEdge.getUserEdgeCost();

	hkSimdReal euclideanCostFromIncoming = incomingPos.distanceTo(outgoingPos);
	hkSimdReal euclideanCost = euclideanCostFromIncoming;

	if ( HK_VERY_LIKELY(!incomingIsStart && !incomingIsUserEdge && useGrandparentCalc) )
	{
		// Work out the point incomingOpt on the incoming edge the minimizes
		//     distance( grandparentMidpoint, incomingOpt ) + distance( incomingOpt, outgoingMidpoint)
		// This is roughly the closest point on the incoming edge to the segment [grandparentMidpoint, outgoingMidpoint]
		// (it's exact if the points lie in the same plane)

		hkVector4 dEdge; dEdge.setSub(m_cachedIncomingEdgeVertB, m_cachedIncomingEdgeVertA);
		hkVector4 outgoingToGrandparent; outgoingToGrandparent.setSub(outgoingPos, m_grandparentMidpoint);

		hkSimdReal tEdge, tGp;
		hkcdClosestPointSegmentSegment( m_cachedIncomingEdgeVertA, dEdge, m_grandparentMidpoint, outgoingToGrandparent, tEdge, tGp);

		hkVector4 incomingOpt; incomingOpt.setAddMul(m_cachedIncomingEdgeVertA, dEdge, tEdge);
		hkSimdReal grandparentToOpt = m_grandparentMidpoint.distanceTo(incomingOpt);
		hkSimdReal optToOutgoing = incomingOpt.distanceTo(outgoingPos);
		hkSimdReal euclideanCostFromGp = grandparentToOpt + optToOutgoing;
		
		// The distance and point that we use for cost modification depend on which combinded cost is better,
		// going from the grandparent or from the incoming.
		hkVector4Comparison grandparentBetter = (euclideanCostFromGp + m_grandparentCost).less(euclideanCostFromIncoming + incomingCost);
		euclideanCost.setSelect( grandparentBetter, optToOutgoing, euclideanCostFromIncoming);
		incomingPos.setSelect( grandparentBetter, incomingOpt, m_cachedIncomingEdgeMidpoint);
		incomingCost.setSelect( grandparentBetter, m_grandparentCost + grandparentToOpt, incomingCost);
	}

	hkSimdReal newTotalCost;

	if(m_costModifier)
	{
		HK_ASSERT(0x4622b8ae, m_cachedOutgoingSectionId == hkaiGetRuntimeIdFromPacked(m_cachedEdgePairInfo.getFaceKey()) );

		const hkBool32 outgoingIsGoal = isGoal( outgoingEdge );
		m_cachedEdgePairInfo.m_incomingEdgeInfo.setPosition( incomingPos );

		hkUint8 queryType = hkaiNavMeshEdgePairInfo::QUERY_TYPE_ASTAR;
		if ( incomingIsStart )
			queryType |= hkaiNavMeshEdgePairInfo::QUERY_START_POINT;
		if ( outgoingIsGoal )
			queryType |= hkaiNavMeshEdgePairInfo::QUERY_END_POINT;
		if ( !incomingIsStart && !outgoingIsGoal )
			queryType |= hkaiNavMeshEdgePairInfo::QUERY_INTERMEDIATE_POINT;

		hkaiAstarCostModifier::NavMeshGetModifiedCostCallbackContext callbackContext( getOutgoingAccessor(),
			m_info,
			m_cachedEdgePairInfo,
			queryType,
			costToParent,
			euclideanCost
			);
		hkSimdReal modifiedCost = m_costModifier->getModifiedCost( callbackContext );

		newTotalCost = incomingCost + modifiedCost + userEdgeCost;
	}
	else
	{
		newTotalCost = incomingCost + euclideanCost + userEdgeCost;
	}

	// Don't let an edge cost less than its parent, otherwise we can get a cycle in the search state.
	newTotalCost.setMax(newTotalCost, originalIncomingCost);
	return newTotalCost.getReal();

}

hkBool32 hkaiNavMeshSectionGraph::isEdgeTraversable( SearchIndex nit, SearchIndex adj, EdgeKey eit ) const
{
	// Remember, SearchIndexes are navmesh edge
	// The face "in between" the edges is the outgoing end's face

	HK_ASSERT(0x1cd1b379, m_cachedIncomingEdgeIndex == nit);
	HK_ASSERT(0x1c8584e9, m_cachedOutgoingEdgeIndex == adj);

	// Prevent backtracking - if the edge we're looking at is the opposite of the one we entered on, reject it.
	const EdgeKey incomingOppositeEdgeKey = m_cachedIncomingEdge.getOppositeEdgeKeyUnchecked();
	if(incomingOppositeEdgeKey == adj)
	{
		return false;
	}

	const hkBool32 incomingIsStart = isStart( nit );
	const hkBool32 outgoingIsGoal = isGoal( adj );
	const hkBool32 incomingIsUser = !incomingIsStart && m_cachedIncomingEdge.isUserEdge();
	const hkBool32 outgoingIsUser = !outgoingIsGoal && m_cachedOutgoingEdge.isUserEdge();

	// Edge filter check
	if ( m_edgeFilter )
	{
		hkUint8 queryType = hkaiNavMeshEdgePairInfo::QUERY_TYPE_ASTAR;
		if ( incomingIsStart )
			queryType |= hkaiNavMeshEdgePairInfo::QUERY_START_POINT;
		if ( outgoingIsGoal )
			queryType |= hkaiNavMeshEdgePairInfo::QUERY_END_POINT;
		if ( !incomingIsStart && !outgoingIsGoal )
			queryType |= hkaiNavMeshEdgePairInfo::QUERY_INTERMEDIATE_POINT;

		hkaiAstarEdgeFilter::NavMeshIsEnabledCallbackContext callbackContext( getOutgoingAccessor(),
			m_info,
			m_cachedEdgePairInfo,
			queryType
			);
		const bool isEdgeEnabled = m_edgeFilter->isEnabled(callbackContext);
		if ( !isEdgeEnabled )
			return false;
	}

	if (m_flags & (hkUint32) (CHECK_SPHERE_RADIUS | CHECK_CAPSULE_RADIUS) )
	{
		const bool projectRadiusChecks = (m_flags & (hkUint32) (PROJECT_RADIUS_CHECKS));
		hkVector4 start = m_startPoint;
		hkVector4 incomingA = m_cachedIncomingEdgeVertA;
		hkVector4 incomingB = m_cachedIncomingEdgeVertB;

		if (projectRadiusChecks)
		{
			start.subMul( start.dot<3>(m_up), m_up);
			incomingA.subMul( incomingA.dot<3>(m_up), m_up);
			incomingB.subMul( incomingB.dot<3>(m_up), m_up);
		}

		// Sphere check
		if (m_flags & (hkUint32) (CHECK_SPHERE_RADIUS) )
		{
			hkSimdReal distanceSq = hkcdPointSegmentDistanceSquared(start, incomingA, incomingB);
			if ( distanceSq > m_searchSphereRadius*m_searchSphereRadius)
			{
				return false;
			}
		}

		// Capsule check
		if (m_flags & (hkUint32) (CHECK_CAPSULE_RADIUS) )
		{
			hkVector4 dab; dab.setSub(incomingB, incomingA);

			int i = 0;
			for (; i < m_numGoals; i++)
			{
				hkVector4 end = m_endPoints[i];
				if(projectRadiusChecks)
				{
					end.subMul( end.dot<3>(m_up), m_up);
				}
				hkVector4 dse; dse.setSub(end, start);

				hkSimdReal distanceSquared = hkcdDistanceSegmentSegment(start, dse, incomingA, dab);

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
	}


	// Check for edge->edge traversability.
	if ( m_flags & (hkUint32) (CHECK_TRAVERSAL_RADIUS) )
	{
		hkSimdReal diameter = hkSimdReal::fromFloat(m_info.m_diameter);

		if ( (incomingIsUser || outgoingIsUser) && !(m_flags & (hkUint32)(CHECK_USER_EDGE_TRAVERSAL_RADIUS)) )
		{
			// User edge traversability tests are disabled.
			return true;
		}

		if ( HK_VERY_UNLIKELY(incomingIsStart && outgoingIsGoal) )
		{
			// Start and goal are on the same face.
			// <fsh.todo> HAI-730. Need to implement a traversability check here.
			return true;
		}
		else if ( incomingIsStart || outgoingIsGoal )
		{
			// In the special cases of the start and goal pseudo-nodes, the traversability check degenerates into
			// an edge->point traversability check.
			hkaiPackedKey insideEdgeKey;
			const hkaiNavMesh::Edge* insideEdge;
			hkVector4 insidePos;

			if ( incomingIsStart )
			{
				// The edge inside the face we're interested in is the outgoing edge.
				insideEdgeKey = adj;
				insideEdge = &m_cachedOutgoingEdge;
				insidePos = m_startPoint;
			}
			else // outgoingIsGoal
			{
				// The edge inside the face we're interested in is the opposite of the incoming edge.
				insideEdgeKey = incomingOppositeEdgeKey;
				insideEdge = &m_cachedOppositeEdge;
				
				const int goalIndex = hkaiGetIndexFromPacked(adj);
				HK_ASSERT(0x544352ad, 0 <= goalIndex && goalIndex < m_numGoals);
				insidePos = m_endPoints[goalIndex];
			}

			hkaiGeneralAccessor accessor(m_streamingInfo);
			hkBool32 isWideEnough = hkaiNavMeshTraversalUtils::isEdgeTraversable(
				diameter, accessor, m_cachedFaceKey, m_cachedFace, m_up, 
				*insideEdge, insideEdgeKey, insidePos);
			return isWideEnough;
		}
		else
		{
			// Transition is from a normal/user edge to another normal/user edge.  Do edge pair traversability check.
			hkaiGeneralAccessor accessor(m_streamingInfo);
			hkBool32 isWideEnough = hkaiNavMeshTraversalUtils::isEdgePairTraversable(
				diameter, accessor, m_cachedFaceKey, m_cachedFace, m_up, 
				m_cachedOppositeEdge, m_cachedOutgoingEdge, incomingOppositeEdgeKey, adj,
				m_cachedOppositeVertexB, m_cachedOppositeVertexA, m_cachedOutgoingEdgeVertA, m_cachedOutgoingEdgeVertB);
			return isWideEnough;
		}
	}

	return true;
}

hkBool32 hkaiNavMeshSectionGraph::isValidEdgeTarget( SearchIndex eit ) const
{
	if ( isGoal(eit) )
	{
		// This target a goal pseudo-node. These are all valid.
		HK_ASSERT(0x2a66082e, hkaiGetIndexFromPacked(eit) < m_numGoals);
		return true;
	}
	else
	{
		HK_ASSERT(0x4f2fa1f4, eit == m_cachedOutgoingEdgeIndex);
		const hkaiNavMesh::Edge& edge = m_cachedOutgoingEdge;
		return ( edge.isTraversable() );
	}
}

void hkaiNavMeshSectionGraph::getPositionForHeuristic( SearchIndex a, hkVector4& posOut ) const
{
	// For user edges, we want to get the position of the *other* end of the user edge.
	// For non-user edges, we probably want that too, but it's close enough.
	HK_ASSERT(0x2ac370e3, a == m_cachedOutgoingEdgeIndex);
	if (!m_cachedOutgoingEdge.isUserEdge())
	{
		posOut = m_cachedOutgoingEdgeMidpoint;
	}
	else
	{
		hkVector4 edgeA, edgeB;
		hkaiStreamingCollection::getEdgePoints(m_streamingInfo, m_cachedOutgoingEdge.getOppositeEdgeKeyUnchecked(), edgeA, edgeB);
		posOut.setInterpolate(edgeA, edgeB, hkSimdReal_Half);
	}
}

hkSimdReal hkaiNavMeshSectionGraph::distanceFromPosition( hkVector4Parameter posA, hkVector4Parameter posB ) const
{
	return posA.distanceTo( posB );
}

void hkaiNavMeshSectionGraph::nextNode( SearchIndex nid )
{
	HK_ASSERT(0x77018d83, !isGoal(nid));

	m_searchState->nextNode( nid );

	setIncomingAccessor( nid );
	setCachedIncomingEdge( nid ); // m_cachedIncomingEdge will be valid for the next lines
	
	if ( HK_VERY_UNLIKELY( isStart(nid) ) )
	{
		// This will happen when expanding the start node.  Set up the cached face data for the start face.
		setCachedFace( m_startFaceKey );
	}
	else
	{
		hkaiNavMesh::FaceIndex oppositeFaceIdx = -1;
		hkaiPackedKey oppositeFaceKey = HKAI_INVALID_PACKED_KEY;
		int oppositeEdgeIdx = -1;
		{
			oppositeFaceIdx = m_cachedIncomingEdge.getOppositeFaceIndex();
			oppositeFaceKey = m_cachedIncomingEdge.getOppositeFaceKeyUnchecked();
			oppositeEdgeIdx = m_cachedIncomingEdge.getOppositeEdgeIndex();
		}

		
		// set the cached face and cached opposite edge
		// on SPU, request them first, then wait for the DMAs
		{
			setOutgoingAccessor( oppositeFaceKey );

			const hkaiNavMesh::Face* oppFace ;
			const hkaiNavMesh::Edge* oppEdge ;
			const hkaiNavMesh::FaceData* faceData ;
			getOutgoingAccessor()->getFaceAndEdgePointers( oppositeFaceIdx, oppositeEdgeIdx, oppFace, oppEdge, faceData );
			// DMAs may still be in flight at this point. Have to wait (below) before it's safe to access oppFace and oppEdge contents

			m_cachedFaceKey = m_cachedIncomingEdge.getOppositeFaceKeyUnchecked();
			m_cachedEdgePairInfo.setFace(m_cachedFaceKey, &m_cachedFace, faceData);


			HK_ON_DEBUG( m_cachedOppositeEdgeIndex = m_cachedIncomingEdge.getOppositeEdgeKeyUnchecked() );

			HK_ON_SPU( hkSpuDmaManager::waitForDmaCompletionUsingBitfield( HK_DMAWAIT_BITSHIFT(hkaiSpuNavMeshAccessor::DMA_GET_FACE) | HK_DMAWAIT_BITSHIFT(hkaiSpuNavMeshAccessor::DMA_GET_EDGE)  ) );

			m_cachedFace = *oppFace;
			m_cachedOppositeEdge = *oppEdge;

			getOutgoingAccessor()->getEdgePoints( m_cachedOppositeEdge, m_cachedOppositeVertexA, m_cachedOppositeVertexB );
			if (m_cachedIncomingEdge.isUserEdge())
			{
				// For user edges, we want the incoming edge point to the be the endpoint of the user edge.
				m_cachedIncomingEdgeMidpoint.setInterpolate(m_cachedOppositeVertexA, m_cachedOppositeVertexB, hkSimdReal_Half);
				m_cachedEdgePairInfo.m_incomingEdgeInfo.setPosition( m_cachedIncomingEdgeMidpoint );
			}
		}
	}

}

void hkaiNavMeshSectionGraph::nextEdge( SearchIndex nid, EdgeKey ekey )
{
	setCachedOutgoingEdge(ekey);

	// We don't actually add the edge to the search state until after we do traversability checks.
}

void hkaiNavMeshSectionGraph::edgeAccepted( SearchIndex nid, EdgeKey ekey )
{
	m_searchState->nextEdge( edgeTarget(nid, ekey) );
}

const hkaiNavMesh::Face& hkaiNavMeshSectionGraph::setCachedFace( hkaiPackedKey faceKey )
{
	setOutgoingAccessor( faceKey );
	const hkaiNavMesh::FaceData* faceData;
	const hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(faceKey);
	const hkaiNavMesh::Face& face = getOutgoingAccessor()->getFaceAndDataPtr(faceIndex, faceData);

	m_cachedFaceKey = faceKey;
	m_cachedFace = face;

	m_cachedEdgePairInfo.setFace( m_cachedFaceKey, &m_cachedFace, faceData );

	return m_cachedFace;
}

const hkaiNavMesh::Edge& hkaiNavMeshSectionGraph::setCachedOutgoingEdge( EdgeKey ekey )
{
	HK_ASSERT(0x66453c99, !isStart(ekey));	// The start pseudo-node should never be set as an outgoing edge

	if ( HK_VERY_UNLIKELY( isGoal(ekey) ) )
	{
		// This graph edge connects to a goal pseudo-node. Set information appropriate to the goal point.
		const int goalIndex = hkaiGetIndexFromPacked(ekey);
		HK_ASSERT(0x73e535ae, 0 <= goalIndex && goalIndex < m_numGoals);

		m_cachedOutgoingEdge.setInvalid();
		HK_ON_DEBUG(m_cachedOutgoingEdgeIndex = ekey);

		const hkVector4 goalPos = m_endPoints[ goalIndex ];
		m_cachedOutgoingEdgeVertA = goalPos;
		m_cachedOutgoingEdgeVertB = goalPos;
		m_cachedOutgoingEdgeMidpoint = goalPos;

		m_cachedEdgePairInfo.setEndPoint( goalPos );
	}
	else
	{
		HK_ASSERT(0x7ff4609a, m_cachedOutgoingSectionId == hkaiGetRuntimeIdFromPacked(ekey));
		const hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(ekey);

		const hkaiNavMesh::EdgeData* edgeData;
		
		hkaiNavMesh::EdgeReference outgoingEdge = getOutgoingAccessor()->getEdgeAndDataPtr(edgeIndex,edgeData);
		hkaiPackedKey outgoingOppositeFaceKey = getOutgoingAccessor()->getOppositeFaceKeyForEdge(outgoingEdge);
		hkaiPackedKey outgoingOppositeEdgeKey = getOutgoingAccessor()->getOppositeEdgeKeyForEdge(outgoingEdge);
		HK_ON_DEBUG(m_cachedOutgoingEdgeIndex = ekey);

		
		hkVector4 edgeVertA, edgeVertB, midPoint;
		getOutgoingAccessor()->getEdgePoints( outgoingEdge, edgeVertA, edgeVertB);
		midPoint.setInterpolate(edgeVertA, edgeVertB, hkSimdReal_Half );
		
		m_cachedOutgoingEdge = outgoingEdge;
		m_cachedOutgoingEdge.m_oppositeFace = outgoingOppositeFaceKey;
		m_cachedOutgoingEdge.m_oppositeEdge = outgoingOppositeEdgeKey;
		m_cachedEdgePairInfo.m_outgoingEdgeInfo.setEdge(ekey, &m_cachedOutgoingEdge, edgeData, midPoint);

		m_cachedOutgoingEdgeVertA = edgeVertA;
		m_cachedOutgoingEdgeVertB = edgeVertB;
		m_cachedOutgoingEdgeMidpoint = midPoint;
	}

	return m_cachedOutgoingEdge;
}

const hkaiNavMesh::Edge& hkaiNavMeshSectionGraph::setCachedIncomingEdge( EdgeKey ekey )
{
 	HK_ASSERT(0x4f25d3e9, !isGoal(ekey));	// Goal pseudo-nodes should never be set as incoming edges.

	if ( HK_VERY_UNLIKELY( isStart(ekey) ))
	{
		// This should only happen when expanding the initial pseudo-node.

		m_cachedIncomingEdge.setInvalid();
		HK_ON_DEBUG(m_cachedIncomingEdgeIndex = ekey);

		const hkVector4 startPos = m_startPoint;
		m_cachedIncomingEdgeVertA = startPos;
		m_cachedIncomingEdgeVertB = startPos;
		m_cachedIncomingEdgeMidpoint = startPos;

		m_cachedEdgePairInfo.setStartPoint( startPos );	
	}
	else
	{
		HK_ASSERT(0x594686e2, m_cachedIncomingSectionId == hkaiGetRuntimeIdFromPacked(ekey));
		const hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(ekey);
		const hkaiNavMesh::EdgeData* edgeData;
		m_cachedIncomingEdge = getIncomingAccessor()->getEdgeAndDataPtr(edgeIndex, edgeData);
		hkaiPackedKey incomingOppositeFaceKey = getIncomingAccessor()->getOppositeFaceKeyForEdge(m_cachedIncomingEdge);
		hkaiPackedKey incomingOppositeEdgeKey = getIncomingAccessor()->getOppositeEdgeKeyForEdge(m_cachedIncomingEdge);
		
		m_cachedIncomingEdge.m_oppositeFace = incomingOppositeFaceKey;
		m_cachedIncomingEdge.m_oppositeEdge = incomingOppositeEdgeKey;

		HK_ON_DEBUG(m_cachedIncomingEdgeIndex = ekey);

		// For simple edges, compute the midpoint of the incoming edge
		// For user edges, we will overwrite this with the midpoint of the OPPOSITE edge later on in setCachedOppositeEdge
		getIncomingAccessor()->getEdgePoints(m_cachedIncomingEdge, m_cachedIncomingEdgeVertA, m_cachedIncomingEdgeVertB);
		m_cachedIncomingEdgeMidpoint.setInterpolate(m_cachedIncomingEdgeVertA, m_cachedIncomingEdgeVertB, hkSimdReal_Half);

		m_cachedEdgePairInfo.m_incomingEdgeInfo.setEdge(ekey, &m_cachedIncomingEdge, edgeData, m_cachedIncomingEdgeMidpoint);

		// Grab the incoming node's parent.
		{
			const hkaiSearchStateNode* grandparentNode = m_searchState->getParent( m_searchState->m_parentNodeState );
			HK_ASSERT(0x12d33695, grandparentNode);
			if ( grandparentNode->isStart() )
			{
				m_grandparentMidpoint = m_startPoint;
				m_grandparentCost = 0.0f;
			}
			else
			{
				hkaiPackedKey grandparentKey = grandparentNode->m_index;
				hkaiNavMesh::EdgeIndex grandparentIndex = hkaiGetIndexFromPacked(grandparentKey);
				
				// "borrow" the outgoing accessor here.
				setOutgoingAccessor(grandparentKey);

				hkaiNavMesh::Edge gpEdge = getOutgoingAccessor()->getEdge(grandparentIndex);
				hkVector4 gpEdgeA, gpEdgeB;
				getOutgoingAccessor()->getEdgePoints(gpEdge, gpEdgeA, gpEdgeB);
				m_grandparentMidpoint.setInterpolate(gpEdgeA, gpEdgeB, hkSimdReal_Half);
				m_grandparentCost.setFromFloat( grandparentNode->m_gCost );
			}
		}
	}

	return m_cachedIncomingEdge;
}

const hkaiNavMesh::Face& hkaiNavMeshSectionGraph::getCachedFace() const
{
	return m_cachedFace;
}

const hkVector4& hkaiNavMeshSectionGraph::getCachedOutgoingEdgeMidpoint() const
{
	return m_cachedOutgoingEdgeMidpoint;
}

const hkaiNavMesh::Edge& hkaiNavMeshSectionGraph::getCachedOutgoingEdge()
{
	return m_cachedOutgoingEdge;
}

const hkaiNavMesh::Edge& hkaiNavMeshSectionGraph::getCachedOutgoingEdge( EdgeKey eIdx )
{
	HK_ASSERT(0x3315bc55, eIdx == m_cachedOutgoingEdgeIndex);
	return m_cachedOutgoingEdge;
}

const hkaiNavMesh::Edge& hkaiNavMeshSectionGraph::getCachedIncomingEdge()
{
	return m_cachedIncomingEdge;
}

const hkaiNavMesh::Edge& hkaiNavMeshSectionGraph::getCachedIncomingEdge( EdgeKey eIdx )
{
	HK_ASSERT(0x3315bc55, eIdx == m_cachedIncomingEdgeIndex);
	return m_cachedIncomingEdge;
}

const hkaiNavMeshAccessor* hkaiNavMeshSectionGraph::getIncomingAccessor() const
{
	HK_ON_SPU( return &m_incomingAccessor );
	HK_ON_CPU( return m_cachedIncomingNavMesh );
}

const hkaiNavMeshAccessor* hkaiNavMeshSectionGraph::getOutgoingAccessor() const
{
	HK_ON_SPU( return &m_outgoingAccessor );
	HK_ON_CPU( return m_cachedOutgoingNavMesh );
}

void hkaiNavMeshSectionGraph::setIncomingAccessor( hkaiPackedKey key )
{
	HK_ASSERT(0x72c4faae, !isGoal(key));	// Goal pseudo-nodes should never be set as incoming edges.
	int sectionId = hkaiGetRuntimeIdFromPacked(key);
	if ( HK_VERY_UNLIKELY( sectionId == HKAI_START_NODE_SECTION_ID ) )
	{
		// This key is for the start pseudo-node, so set the accessor to that of the start face.
		sectionId = hkaiGetRuntimeIdFromPacked(m_startFaceKey);
	}

	if (HK_VERY_UNLIKELY(m_cachedIncomingSectionId != sectionId))
	{

#ifdef HK_PLATFORM_SPU
		m_incomingAccessor.setSection( m_streamingInfo, sectionId );
#else
		const hkaiNavMeshInstance* meshInstancePtr = hkaiStreamingCollection::getInstanceAt(m_streamingInfo, sectionId) ;
		HK_ASSERT(0x32951d49, meshInstancePtr );
		m_cachedIncomingNavMesh = meshInstancePtr;
#endif

		m_cachedIncomingSectionId = sectionId;
	}
}

void hkaiNavMeshSectionGraph::setOutgoingAccessor( hkaiPackedKey key )
{
	HK_ASSERT(0x736a92dd, !isStart(key));	// The start pseudo-node should never be set as an outgoing edge
	int sectionId = hkaiGetRuntimeIdFromPacked(key);
	if ( sectionId == HKAI_GOAL_NODE_SECTION_ID )
	{
		// This key is for a goal pseudo-node, so set the accessor to that of the indexed goal.
		const int goalIndex = hkaiGetIndexFromPacked(key);
		HK_ASSERT(0x2ff6bb4e, 0 <= goalIndex && goalIndex < m_numGoals);
		const hkaiPackedKey goalKey = m_endFaceKeys[ goalIndex ];
		sectionId = hkaiGetRuntimeIdFromPacked(goalKey);
	}

	if( HK_VERY_UNLIKELY(m_cachedOutgoingSectionId != sectionId) )
	{

#ifdef HK_PLATFORM_SPU
		m_outgoingAccessor.setSection( m_streamingInfo, sectionId );
#else
		const hkaiNavMeshInstance* meshInstancePtr = hkaiStreamingCollection::getInstanceAt(m_streamingInfo, sectionId) ;
		HK_ASSERT(0x32951d49, meshInstancePtr );
		m_cachedOutgoingNavMesh = meshInstancePtr;
#endif
		
		m_cachedOutgoingSectionId = sectionId;
	}
	
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
