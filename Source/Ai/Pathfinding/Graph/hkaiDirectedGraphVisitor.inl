/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiDirectedGraphVisitor::hkaiDirectedGraphVisitor()
{
	m_cachedNode.m_startEdgeIndex = -1;
	m_cachedNode.m_numEdges = -1;

	m_cachedInstanceNode.m_startEdgeIndex = -1;
	m_cachedInstanceNode.m_numEdges = -1;

	m_streamingInfo = HK_NULL;

	m_cachedIncomingSectionId = -1;
	m_cachedOutgoingSectionId = -1;
	m_cachedGeneralSectionId = -1;
}

#ifndef HK_PLATFORM_SPU
inline hkaiDirectedGraphVisitor::hkaiDirectedGraphVisitor(const hkaiDirectedGraphAccessor* dirGraph)
{
	m_incomingGraph = dirGraph;

	m_cachedNode.m_startEdgeIndex = -1;
	m_cachedNode.m_numEdges = -1;
	m_streamingInfo = HK_NULL;

	m_cachedIncomingSectionId = -1;
	m_cachedOutgoingSectionId = -1;
	m_cachedGeneralSectionId = -1;
}
#endif

#define HKAI_NULL_GRAPH_MSG "Null graph at section index " << sectionId << ". Make sure to specify the graph in e.g. hkaiWorld::loadNavMeshInstance, or disable hierarchical A*."

inline void hkaiDirectedGraphVisitor::setIncomingAccessor( int sectionId )
{
	if ( HK_VERY_UNLIKELY(m_cachedIncomingSectionId != sectionId) )
	{
#ifdef HK_PLATFORM_SPU
		m_incomingAccessor.setSection(m_streamingInfo, sectionId);  // DMA the graph and store it's pointers
#else
		m_incomingGraph = hkaiStreamingCollection::getGraphInstanceAt(m_streamingInfo, sectionId);
		HK_ASSERT3(0x6fe6ddc2, m_incomingGraph, HKAI_NULL_GRAPH_MSG);
#endif
	}
}

inline void hkaiDirectedGraphVisitor::setOutgoingAccessor( int sectionId )
{
	if ( HK_VERY_UNLIKELY(m_cachedOutgoingSectionId != sectionId) )
	{
#ifdef HK_PLATFORM_SPU
		m_outgoingAccessor.setSection(m_streamingInfo, sectionId); // DMA the graph and store it's pointers
#else
		m_outgoingGraph = hkaiStreamingCollection::getGraphInstanceAt(m_streamingInfo, sectionId);
		HK_ASSERT3(0x6fe6ddc2, m_outgoingGraph, HKAI_NULL_GRAPH_MSG);
#endif
	}
}

inline void hkaiDirectedGraphVisitor::setGeneralAccessor( int sectionId )
{
	if ( HK_VERY_UNLIKELY(m_cachedGeneralSectionId != sectionId) )
	{
#ifdef HK_PLATFORM_SPU
		m_generalAccessor.setSection(m_streamingInfo, sectionId);  // DMA the graph and store it's pointers
#else
		m_generalGraph = hkaiStreamingCollection::getGraphInstanceAt(m_streamingInfo, sectionId);
		HK_ASSERT3(0x6fe6ddc2, m_generalGraph, HKAI_NULL_GRAPH_MSG);
#endif
		m_cachedGeneralSectionId = sectionId;
	}
}


inline hkBool32 hkaiDirectedGraphVisitor::isGoal( SearchIndex nit ) const
{
	
	return false;
}

// Get an upper bound on the number of nodes adjacent to a given node.
int hkaiDirectedGraphVisitor::getMaxNeighborCount( SearchIndex nit ) const
{
	HK_ASSERT(0x43af1466, nit == m_cachedNodeIndex);
	HK_ON_SPU( hkSpuDmaManager::waitForDmaCompletion(EDGE_DMA_GROUP); )
	return m_cachedNode.m_numEdges + m_cachedInstanceNode.m_numEdges;
}

void hkaiDirectedGraphVisitor::getNeighbors( SearchIndex nit, hkArrayBase< EdgeKey >& neighbors ) const
{
	HK_ASSERT(0x43af1466, nit == m_cachedNodeIndex);

	const int numOriginalEdges = m_cachedNode.m_numEdges;
	const int numInstancedEdges = m_cachedInstanceNode.m_numEdges;
	const int numEdges = numOriginalEdges + numInstancedEdges;

	HK_ASSERT(0xc7197b66, neighbors.isEmpty());
	HK_ASSERT2(0xb7d4c079, neighbors.getCapacity() >= numEdges, 
		"For performance, neighbors array should be pre-allocated to contain all possible neighbors.");
	neighbors.setSizeUnchecked(numEdges);
	EdgeKey* HK_RESTRICT neighborPtr = neighbors.begin();

	{
		const int edgeStartIndex = m_cachedNode.m_startEdgeIndex;
		for ( int e=0; e < numOriginalEdges; e++ )
		{
			const EdgeKey edgeIndex = (edgeStartIndex + e);
			*neighborPtr++ = edgeIndex;
		}
	}

	{
		const int edgeStartIndex = m_cachedInstanceNode.m_startEdgeIndex;
		for ( int e=0; e < numInstancedEdges; e++ )
		{
			const EdgeKey edgeIndex = (edgeStartIndex + e);
			*neighborPtr++ = edgeIndex;
		}
	}
}

inline hkaiDirectedGraphVisitor::SearchIndex hkaiDirectedGraphVisitor::edgeTarget( SearchIndex nit, EdgeKey eit ) const
{
	//HK_ASSERT(0x1ab9fd3b, eit - m_cachedNode.m_startEdgeIndex >= 0);
	//HK_ASSERT(0xb2326f3, eit - m_cachedNode.m_startEdgeIndex < MAX_VERTEX_DEGREE + 2);

	//HK_ASSERT(0x2c21c36c, m_currentEdge == m_localEdgeStart + eit - m_cachedNode.m_startEdgeIndex );
	return m_currentEdge->getOppositeNodeKeyUnchecked();
}

inline hkaiDirectedGraphVisitor::EdgeCost hkaiDirectedGraphVisitor::getTotalCost(SearchIndex nit, SearchIndex adj, EdgeKey eit, const EdgeCost costToParent) const
{
	//HK_ASSERT(0x2665283f, eit - m_cachedNode.m_startEdgeIndex >= 0);
	//HK_ASSERT(0x59a664b2, eit - m_cachedNode.m_startEdgeIndex < MAX_VERTEX_DEGREE + 2);

	//HK_ASSERT(0x2c21c36c, m_currentEdge == m_localEdgeStart + eit - m_cachedNode.m_startEdgeIndex );
	return costToParent + m_currentEdge->m_cost;
}

//
// End hkaiAstar interface
//

inline hkBool32 hkaiDirectedGraphVisitor::isValidEdgeTarget( SearchIndex adj ) const
{
	return true;
}

// Edges with negative costs aren't traversable
// During streaming, we negate the edge cost for edges into sections that haven't been loaded let
inline hkBool32 hkaiDirectedGraphVisitor::isEdgeTraversable(SearchIndex nit, SearchIndex adj, EdgeKey eit) const
{
	//HK_ASSERT(0x2665283f, eit - m_cachedNode.m_startEdgeIndex >= 0);
	//HK_ASSERT(0x59a664b2, eit - m_cachedNode.m_startEdgeIndex < MAX_VERTEX_DEGREE + 2);

	//HK_ASSERT(0x2c21c36c, m_currentEdge == m_localEdgeStart + eit - m_cachedNode.m_startEdgeIndex );
	return true;
}


inline void hkaiDirectedGraphVisitor::getCurrentPosition( SearchIndex a, hkVector4& v ) const
{
	HK_ASSERT(0x27c345f7, m_cachedIncomingSectionId == hkaiGetRuntimeIdFromPacked(a) );
	NodeIndex nodeIdx = hkaiGetIndexFromPacked( a );
	getIncomingAccessor()->getPosition(nodeIdx, v);
}

inline void hkaiDirectedGraphVisitor::getPositionForHeuristic( SearchIndex a, hkVector4& v ) const
{
	HK_ASSERT(0x27c345f7, m_cachedOutgoingSectionId == hkaiGetRuntimeIdFromPacked(a) );
	NodeIndex nodeIdx = hkaiGetIndexFromPacked( a );
	getOutgoingAccessor()->getPosition(nodeIdx, v);
}

inline hkSimdReal hkaiDirectedGraphVisitor::distanceFromPosition(hkVector4Parameter posA, hkVector4Parameter posB) const
{
	return posA.distanceTo( posB );
}

inline void hkaiDirectedGraphVisitor::setSearchState( SearchState* state ) 
{
	m_searchState = state;
}


inline const hkaiDirectedGraphAccessor* hkaiDirectedGraphVisitor::getIncomingAccessor() const
{
	HK_ON_CPU( return m_incomingGraph; )
	HK_ON_SPU( return &m_incomingAccessor; )
}

inline const hkaiDirectedGraphAccessor* hkaiDirectedGraphVisitor::getOutgoingAccessor() const
{
	HK_ON_CPU( return m_outgoingGraph; )
	HK_ON_SPU( return &m_outgoingAccessor; )
}

inline const hkaiDirectedGraphAccessor* hkaiDirectedGraphVisitor::getGeneralAccessor() const
{
	HK_ON_CPU( return m_generalGraph; )
	HK_ON_SPU( return &m_generalAccessor; )
}

static void _copyEdges( hkaiDirectedGraphExplicitCost::Edge* edgeLocalDst, const hkaiDirectedGraphExplicitCost::Edge* edgeMainMemSrc, int edgeStartIdx, int numEdges, int arraySize)
{
#ifdef HK_PLATFORM_SPU
	for (int i=0; i<numEdges; i++)
	{
		extern hkSpu4WayCache* g_hkaiGraphCache;
		edgeLocalDst[i] = *hkGetArrayElemWithMaxSizeUsingCache(edgeMainMemSrc, arraySize, edgeStartIdx+i, g_hkaiGraphCache, HK_SPU_PATHFINDING_GRAPH_CACHE_LINE_SIZE);
	}
#else
	hkString::memCpy4( edgeLocalDst, edgeMainMemSrc + edgeStartIdx, numEdges*sizeof(hkaiDirectedGraphExplicitCost::Edge) / 4 );
#endif
}

//
// Listener interface
//
inline void hkaiDirectedGraphVisitor::nextNode(SearchIndex nid, hkBool32 updateSearchState)
{
	HK_ON_DEBUG( m_cachedNodeIndex = nid );

	int sectionId = hkaiGetRuntimeIdFromPacked(nid);
	
	
	HK_ON_CPU( if (m_streamingInfo) )
	{
		setIncomingAccessor( sectionId );
	}
	m_cachedIncomingSectionId = sectionId;

	{
		const int nodeIndex = hkaiGetIndexFromPacked( nid );
		m_cachedNode = getIncomingAccessor()->getNode(nodeIndex);
		getIncomingAccessor()->getInstancedNode(nodeIndex, m_cachedInstanceNode);

		
		// Clamp the cached node's num edges so that we don't overwrite memory
		m_cachedNode.m_numEdges         = hkMath::min2<int>((int)m_cachedNode.m_numEdges, (int)MAX_VERTEX_DEGREE);
		m_cachedInstanceNode.m_numEdges = hkMath::min2<int>((int)m_cachedInstanceNode.m_numEdges, (int)MAX_VERTEX_DEGREE);
	}
			

	// start grabbing edges, since we're going to need them
	const hkaiDirectedGraphExplicitCost::Edge* edgeBase = getIncomingAccessor()->getEdgeBegin();
	const hkaiDirectedGraphExplicitCost::Edge* ownedEdgeBase = getIncomingAccessor()->getOwnedEdgeBegin();
	const hkaiRuntimeIndex currentRuntimeIndex = getIncomingAccessor()->getRuntimeId();

	// Unfortunately, we have to do this check all the time, since hypothetically we could have a 
	// one-way edge into a section graph with no edges.
	if( HK_VERY_LIKELY(edgeBase != HK_NULL) )
	{
		const int numEdges = m_cachedNode.m_numEdges;

		hkaiDirectedGraphExplicitCost::Edge* edgeLocalDst = m_localEdges;
		int maxNumEdges = getIncomingAccessor()->getNumOriginalEdges();

		_copyEdges( edgeLocalDst, edgeBase, m_cachedNode.m_startEdgeIndex, numEdges, maxNumEdges );

		HK_ON_DEBUG( m_localEdgeStart = m_localEdges);
	}

	if ( HK_VERY_LIKELY(ownedEdgeBase != HK_NULL) && m_cachedInstanceNode.m_numEdges > 0)
	{
		int ownedIndex = m_cachedInstanceNode.m_startEdgeIndex - getIncomingAccessor()->getNumOriginalEdges();
		HK_ASSERT(0x7d41cc7c, ownedIndex >= 0);

		const int numEdges = m_cachedInstanceNode.m_numEdges;
		int maxNumEdges = getIncomingAccessor()->getNumOwnedEdges();

		hkaiDirectedGraphExplicitCost::Edge* edgeLocalDst = m_localEdges + m_cachedNode.m_numEdges;

		_copyEdges( edgeLocalDst, ownedEdgeBase, ownedIndex, numEdges, maxNumEdges );
	}

	// nextEdge increments this right away, so subtract 1 to compensate
	// equivalent to m_currentEdge = m_localEdges - 1;
	int edgeOffset = sizeof(hkaiDirectedGraphExplicitCost::Edge);
	m_currentEdge =  hkAddByteOffset(m_localEdges, -edgeOffset); 

	// Fix up the edges so that we can use getOppositeNodeKeyUnchecked later.
	// All original edges should be internal and need fixup.
	// Owned edges should either be external (from streaming) or from user edges (internal or external)
	const int totalNumEdges = m_cachedNode.m_numEdges + m_cachedInstanceNode.m_numEdges;
	for (int i=0; i<totalNumEdges; i++)
	{
		if ( !m_localEdges[i].isExternal() )
		{
			hkaiPackedKey targetKey = hkaiGetPackedKey( currentRuntimeIndex, m_localEdges[i].getOppositeNodeIndex() ); 
			m_localEdges[i].setTargetKey( targetKey );
		}
	}

	// Inform the SearchState of this node
	if (updateSearchState)
	{
		m_searchState->nextNode( nid );
	}
}

inline void hkaiDirectedGraphVisitor::nextEdge(SearchIndex nid, EdgeKey ekey, hkBool32 updateSearchState) 
{
	m_currentEdge = m_currentEdge + 1;

	// Inform the SearchState of the new adjacent node
	if (updateSearchState)
	{
		m_searchState->nextEdge( edgeTarget(nid, ekey) );
	}

	hkaiPackedKey target = m_currentEdge->getOppositeNodeKeyUnchecked();

	setOutgoingSectionFromPacked(target);
}

inline void hkaiDirectedGraphVisitor::setOutgoingSectionFromPacked(hkaiPackedKey nodeKey)
{
	
	int sectionId = hkaiGetRuntimeIdFromPacked(nodeKey);

	if(m_streamingInfo)
	{
		setOutgoingAccessor( sectionId );
	}
	else
	{
		HK_ON_CPU( m_outgoingGraph = m_incomingGraph );
		HK_ON_SPU( m_outgoingAccessor = m_incomingAccessor );
	}
	m_cachedOutgoingSectionId = sectionId ;
}

inline void hkaiDirectedGraphVisitor::setParent(SearchIndex nid, SearchIndex parent, EdgeKey edge) const {}
inline void hkaiDirectedGraphVisitor::keepParent(SearchIndex nid, SearchIndex parent, EdgeKey edge) const {}
inline void hkaiDirectedGraphVisitor::nodeOpened(SearchIndex) const {}
inline void hkaiDirectedGraphVisitor::openNodeAdjusted(SearchIndex) const {}
inline void hkaiDirectedGraphVisitor::nodeClosed(SearchIndex) const {}

//
// end listener interface
//

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
