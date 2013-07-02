/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Ai/Pathfinding/Graph/hkaiNavVolumeGraph.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolume.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolumeUtils.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>

// Turn on/off internal timers. Warning: turning on timers always slows things down as a side-effect!
#if 0
#include <Common/Base/Monitor/hkMonitorStream.h>
#define MY_TIMER_BEGIN( NAME, OBJECT ) HK_TIMER_BEGIN( NAME, OBJECT )
#define MY_TIMER_END(  ) HK_TIMER_END(  )
#define MY_TIME_CODE_BLOCK( NAME, OBJECT ) HK_TIME_CODE_BLOCK( NAME, OBJECT )
#else
#define MY_TIMER_BEGIN( NAME, OBJECT )
#define MY_TIMER_END(  )
#define MY_TIME_CODE_BLOCK( NAME, OBJECT )
#endif

inline hkaiNavVolumeGraph::hkaiNavVolumeGraph()
:	m_streamingInfo(HK_NULL),
	m_costModifier(HK_NULL),
	m_edgeFilter(HK_NULL),
	m_startCellKey(HKAI_INVALID_PACKED_KEY),
	m_numGoals(0),
	m_searchState(HK_NULL),
	m_cachedCurrentSectionId(-1),
	m_cachedAdjacentSectionId(-1)
#ifdef HK_DEBUG
	,m_cachedCurrentCellKey(-1)
	,m_cachedCurrentEdgeIndex(-1)
	,m_cachedAdjacentCellKey(-1)
#endif
{
#ifdef HK_DEBUG
	m_cachedCurrentAabb.setEmpty();
	m_cachedAdjacentAabb.setEmpty();
#endif
}

inline void hkaiNavVolumeGraph::init( 
	const hkaiStreamingCollection::InstanceInfo* streamingInfo, 
	const hkaiAstarCostModifier* costModifier,
	const hkaiAstarEdgeFilter* edgeFilter )
{
	m_streamingInfo = streamingInfo;
	m_costModifier = costModifier;
	m_edgeFilter = edgeFilter;
}

inline void hkaiNavVolumeGraph::setStartPointAndCell( hkVector4Parameter startPoint, hkaiPackedKey startCellKey )
{
	m_startPoint = startPoint;
	m_startCellKey = startCellKey;
}

inline void hkaiNavVolumeGraph::setGoalPointsAndCells( int numGoals, const hkVector4* goalPoints, const hkaiPackedKey* goalCellKeys )
{
	m_numGoals = numGoals;
	m_endPoints = goalPoints;
	m_endCellKeys = goalCellKeys;
}

inline void hkaiNavVolumeGraph::setTraversalInfo ( const hkaiAgentTraversalInfo& info )
{
	m_info = info;
}

void hkaiNavVolumeGraph::setSearchState(SearchState* ss)
{
	m_searchState = ss;
}


inline const hkaiStreamingCollection::InstanceInfo* hkaiNavVolumeGraph::getStreamingInfo() const
{
	return m_streamingInfo;
}

inline const hkaiAstarCostModifier* hkaiNavVolumeGraph::getCostModifier() const
{
	return m_costModifier;
}

inline const hkaiAstarEdgeFilter* hkaiNavVolumeGraph::getEdgeFilter() const
{
	return m_edgeFilter;
}

inline const hkaiAgentTraversalInfo& hkaiNavVolumeGraph::getTraversalInfo() const
{
	return m_info;
}


inline hkBool32 hkaiNavVolumeGraph::isGoal( SearchIndex nit ) const
{
	
	return false;
}

// Get an upper bound on the number of nodes adjacent to a given node.
int hkaiNavVolumeGraph::getMaxNeighborCount( SearchIndex nit ) const
{
	HK_ASSERT(0x73cc9e07, hkaiGetRuntimeIdFromPacked(nit) == m_cachedCurrentSectionId );
	HK_ASSERT(0x7a72201d, nit == m_cachedCurrentCellKey);
	return m_cachedCurrentCell.m_numEdges + m_cachedCurrentCellInstance.m_numEdges;
}

void hkaiNavVolumeGraph::getNeighbors( SearchIndex nit, hkArrayBase< EdgeKey >& neighbors ) const
{
	HK_ASSERT(0x73cc9e07, hkaiGetRuntimeIdFromPacked(nit) == m_cachedCurrentSectionId );
	HK_ASSERT(0x7a72201d, nit == m_cachedCurrentCellKey);

	{
		const int numEdges = m_cachedCurrentCell.m_numEdges + m_cachedCurrentCellInstance.m_numEdges;

		HK_ASSERT(0x6c7197b6, neighbors.isEmpty());
		HK_ASSERT2(0x9b7d4c07, neighbors.getCapacity() >= numEdges, 
			"For performance, neighbors array should be pre-allocated to contain all possible neighbors.");
		neighbors.setSizeUnchecked(numEdges);
	}
	EdgeKey* HK_RESTRICT neighborPtr = neighbors.begin();

	// Original edges
	{
		const int numOriginalEdges = m_cachedCurrentCell.m_numEdges;
		const int startEdgeIndex   = m_cachedCurrentCell.m_startEdgeIndex;
		for ( int e=0; e < numOriginalEdges; e++ )
		{
			const EdgeKey edgeIndex = (startEdgeIndex + e);
			*neighborPtr++ = edgeIndex;
		}
	}

	// Owned edges
	{
		const int numOwnedEdges  = m_cachedCurrentCellInstance.m_numEdges;
		const int startEdgeIndex = m_cachedCurrentCellInstance.m_startEdgeIndex;
		for ( int e=0; e < numOwnedEdges; e++ )
		{
			const EdgeKey edgeIndex = (startEdgeIndex + e);
			*neighborPtr++ = edgeIndex;
		}
	}

}


inline hkaiNavVolumeGraph::SearchIndex hkaiNavVolumeGraph::edgeTarget( SearchIndex nit, EdgeKey eit ) const
{
	HK_ASSERT(0x73cc9e07, hkaiGetRuntimeIdFromPacked(nit) == m_cachedCurrentSectionId );
	HK_ASSERT(0x5411bfef, m_cachedCurrentEdgeIndex == eit );
	return m_cachedCurrentEdge.getOppositeCellKeyUnchecked();
}


inline hkaiNavVolumeGraph::EdgeCost hkaiNavVolumeGraph::getTotalCost( SearchIndex nit, SearchIndex adj, EdgeKey eit, const EdgeCost costToParent_float ) const
{
	HK_ASSERT(0x73cc9e06, hkaiGetRuntimeIdFromPacked(nit) == m_cachedCurrentSectionId );
	HK_ASSERT(0x73cc9e08, hkaiGetRuntimeIdFromPacked(adj) == m_cachedAdjacentSectionId );

	HK_ASSERT(0x7a72201d, nit == m_cachedCurrentCellKey);
	HK_ASSERT(0x7a72201d, adj == m_cachedAdjacentCellKey);

	// Find the shared point.
	// Distance is distance of the centroid nit to this point + distance this point to centroid adj
	
	MY_TIME_CODE_BLOCK("edgeCost", HK_NULL);

	HK_ASSERT(0x1f3b1c58, m_cachedCurrentAabb.isValid()  && !m_cachedCurrentAabb.isEmpty()  );
	HK_ASSERT(0x1f3b1c59, m_cachedAdjacentAabb.isValid() && !m_cachedAdjacentAabb.isEmpty() );
	
	hkSimdReal costToParent = hkSimdReal::fromFloat(costToParent_float);
	
	hkAabb portal; hkaiNavVolumeUtils::getPortalBetweenCellAabbs(m_cachedCurrentAabb, m_cachedAdjacentAabb, portal);
	
	hkVector4 currentCenter  = m_cachedModifierInfo.m_currentCellInfo.m_position;
	hkVector4 adjacentCenter = m_cachedModifierInfo.m_adjacentCellInfo.m_position;

	
	
	hkVector4 portalCenter;   portal.getCenter(portalCenter);

	const hkSimdReal currentCost = currentCenter.distanceTo( portalCenter );
	const hkSimdReal adjacentCost = portalCenter.distanceTo( adjacentCenter );

	if (!m_costModifier)
	{
		return (costToParent + currentCost + adjacentCost).getReal();
	}
	else
	{
		HK_ASSERT(0x6cbeea2d, m_cachedModifierInfo.m_edgeKey == hkaiGetPackedKey( hkaiGetRuntimeIdFromPacked(nit), (hkUint32) eit) );
		HK_ASSERT(0x6cbeea2e, m_cachedModifierInfo.m_currentCellInfo.m_cellKey == (hkUint32) nit);
		HK_ASSERT(0x6cbeea2f, m_cachedModifierInfo.m_adjacentCellInfo.m_cellKey == (hkUint32) adj);

		hkaiAstarCostModifier::NavVolumeGetModifiedCostCallbackContext callbackContext( getCurrentAccessor(),
			m_info, 
			m_cachedModifierInfo, 
			// TODO Check QueryType flags - intermediate, start, end?
			hkaiNavVolumeCellPairInfo::QUERY_TYPE_ASTAR,
			costToParent,
			currentCost,
			adjacentCost
			);

		hkSimdReal totalCost = costToParent + m_costModifier->getModifiedCost( callbackContext );
		return totalCost.getReal();
	}
}

inline hkBool32 hkaiNavVolumeGraph::isEdgeTraversable( SearchIndex nit, SearchIndex adj, EdgeKey eit )
{

	HK_ASSERT(0x146f612c, eit == m_cachedCurrentEdgeIndex );
	const hkaiNavVolume::Edge& edge = m_cachedCurrentEdge;
	if ( !edge.isTraversable() )
	{
		return false;
	}

	// TODO: Prevent backtracking!!

	//
	// If the edge is traversable, *now* we should cache the adjacent cell,
	// notify the search state, etc.
	//
	{
		setAdjacentAccessor( hkaiGetRuntimeIdFromPacked(adj) );
		setAdjacentCachedCell( adj );
		m_searchState->nextEdge( adj );
		
	}

	HK_ASSERT(0x73cc9e06, hkaiGetRuntimeIdFromPacked(nit) == m_cachedCurrentSectionId );
	HK_ASSERT(0x73cc9e08, hkaiGetRuntimeIdFromPacked(adj) == m_cachedAdjacentSectionId );

	MY_TIME_CODE_BLOCK("isEdgeTraversable", HK_NULL);

	if (!m_edgeFilter)
	{
		return true;
	}
	else
	{
		hkaiAstarEdgeFilter::NavVolumeIsEnabledCallbackContext callbackContext( getCurrentAccessor(),
			m_info, 
			m_cachedModifierInfo, 
			// TODO Check QueryType flags - intermediate, start, end?
			hkaiNavVolumeCellPairInfo::QUERY_TYPE_ASTAR
			);

		return m_edgeFilter->isEnabled( callbackContext );
	}
}


inline hkBool32 hkaiNavVolumeGraph::isValidEdgeTarget( SearchIndex adj ) const
{
	// We have the target node, not the actual edge here. So we can't make a judgement here.
	return true;
}

inline void hkaiNavVolumeGraph::getPositionForHeuristic( SearchIndex a, hkVector4& vecOut ) const
{
	HK_ASSERT(0x269d778d, hkaiGetRuntimeIdFromPacked(a) == m_cachedAdjacentSectionId );
	HK_ASSERT(0x63362e34, m_cachedAdjacentCellKey == a);
	HK_ASSERT(0x1f3b1c59, m_cachedAdjacentAabb.isValid() && !m_cachedAdjacentAabb.isEmpty() );
	
	m_cachedAdjacentAabb.getCenter(vecOut);
}

inline void hkaiNavVolumeGraph::getPosition( SearchIndex a, hkVector4& vecOut ) const
{
	getPositionForHeuristic(a, vecOut);
}


inline hkSimdReal hkaiNavVolumeGraph::distanceFromPosition( hkVector4Parameter posA, hkVector4Parameter posB ) const
{
	return posA.distanceTo( posB );
}

inline void hkaiNavVolumeGraph::nextNode( SearchIndex nid )
{
	setCurrentAccessor( hkaiGetRuntimeIdFromPacked( nid) );
	hkaiNavVolume::CellIndex cellIndex = hkaiGetIndexFromPacked(nid);
	m_cachedCurrentCell = getCurrentAccessor()->getCell( cellIndex );
	getCurrentAccessor()->getInstancedCell( cellIndex, m_cachedCurrentCellInstance);
	HK_ON_DEBUG( m_cachedCurrentCellKey = nid );

	// Set up the cost modifier info
	m_cachedModifierInfo.m_currentCellInfo.m_cell = &m_cachedCurrentCell;
	m_cachedModifierInfo.m_currentCellInfo.m_cellKey = nid;
	
	hkaiNavVolumeUtils::getCellWorldAabb( *getCurrentAccessor() , m_cachedCurrentCell , m_cachedCurrentAabb );
	m_cachedCurrentAabb.getCenter(m_cachedModifierInfo.m_currentCellInfo.m_position);

	m_searchState->nextNode( nid );
}

inline void hkaiNavVolumeGraph::nextEdge( SearchIndex nid, EdgeKey eIdx )
{
	m_cachedCurrentEdge = getCurrentAccessor()->getEdge(eIdx);
	// Overwrite the opposite cell key, so that we can use getOppositeCellKeyUnchecked() later
	m_cachedCurrentEdge.m_oppositeCell = getCurrentAccessor()->getOppositeCellKeyForEdge( m_cachedCurrentEdge );
	HK_ON_DEBUG( m_cachedCurrentEdgeIndex = eIdx );
	
	// Set up cost modifier info
	m_cachedModifierInfo.m_edgeKey = hkaiGetPackedKey(m_cachedCurrentSectionId, eIdx);
	m_cachedModifierInfo.m_edge = &m_cachedCurrentEdge;

#ifdef HK_DEBUG
	m_cachedAdjacentAabb.setEmpty();
#endif
}

inline const hkaiNavVolumeAccessor* hkaiNavVolumeGraph::getCurrentAccessor() const
{
	HK_ON_CPU( return m_cachedCurrentNavVolume );
	HK_ON_SPU( return &m_currentAccessor );
}

inline const hkaiNavVolumeAccessor* hkaiNavVolumeGraph::getAdjacentAccessor() const
{
	HK_ON_CPU( return m_cachedAdjacentNavVolume );
	HK_ON_SPU( return &m_adjacentAccessor );
}

inline void hkaiNavVolumeGraph::setCurrentAccessor( int sectionId )
{
	if (HK_VERY_UNLIKELY(m_cachedCurrentSectionId != sectionId))
	{
		

#ifdef HK_PLATFORM_SPU
		m_currentAccessor.setSection( m_streamingInfo, sectionId );
#else
		const hkaiNavVolumeInstance* volPtr = hkaiStreamingCollection::getVolumeInstanceAt(m_streamingInfo, sectionId);
		HK_ASSERT(0x32951d49, volPtr );
		m_cachedCurrentNavVolume = volPtr;
#endif

		m_cachedCurrentSectionId = sectionId;
	}
}

inline void hkaiNavVolumeGraph::setAdjacentAccessor( int sectionId )
{

	if (HK_VERY_UNLIKELY(m_cachedAdjacentSectionId != sectionId))
	{
#ifdef HK_PLATFORM_SPU
		m_adjacentAccessor.setSection( m_streamingInfo, sectionId );
#else
		const hkaiNavVolumeInstance* volPtr = hkaiStreamingCollection::getVolumeInstanceAt(m_streamingInfo, sectionId);
		// The pointer may legitimately be NULL here, because we set the outgoing accessor before we check that the edge is valid.
		m_cachedAdjacentNavVolume = volPtr;
#endif

		m_cachedAdjacentSectionId = sectionId;
	}
}

inline void hkaiNavVolumeGraph::setAdjacentCachedCell( hkUint32 cellKey )
{
	HK_ASSERT(0x44582b93, hkaiGetRuntimeIdFromPacked(cellKey) == m_cachedAdjacentSectionId);
	m_cachedAdjacentCell = getAdjacentAccessor()->getCell( hkaiGetIndexFromPacked(cellKey) );
	HK_ON_DEBUG( m_cachedAdjacentCellKey = cellKey );

	hkaiNavVolumeUtils::getCellWorldAabb( *getAdjacentAccessor() , m_cachedAdjacentCell , m_cachedAdjacentAabb );
	
	// Set up cost modifier info
	m_cachedAdjacentAabb.getCenter(m_cachedModifierInfo.m_adjacentCellInfo.m_position);
	m_cachedModifierInfo.m_adjacentCellInfo.m_cellKey = cellKey;
	m_cachedModifierInfo.m_adjacentCellInfo.m_cell = &m_cachedAdjacentCell;
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
