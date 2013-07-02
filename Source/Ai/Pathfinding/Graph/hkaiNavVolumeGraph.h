/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAV_VOLUME_GRAPH_H
#define HKAI_NAV_VOLUME_GRAPH_H

#include <Ai/Pathfinding/Astar/Heuristic/hkaiGraphDistanceHeuristic.h>
#include <Ai/Pathfinding/Astar/SearchState/hkaiHashSearchState.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/Astar/CostModifier/hkaiAstarCostModifier.h>
#include <Ai/Pathfinding/Astar/EdgeFilter/hkaiAstarEdgeFilter.h>

/// hkaiNavVolumeGraph serves as the bridge between an hkaiNavVolume and the hkaiAStar system.
/// The nav volume cells are treated as nodes in the A* search.
/// Users don't need to use this directly; instead they should use hkaiVolumePathfindingUtil or hkaiNavVolumeAstarJobs.
/// This particular implementation keeps a local copy of the current cell and relevant edges,
/// which avoids DMA/cache checks on SPU, and cache misses on CPU
struct hkaiNavVolumeGraph
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavVolumeGraph);

	typedef int SearchIndex;
	typedef int EdgeKey;
	typedef hkReal EdgeCost;
	typedef hkVector4 Position;

	typedef hkaiGraphMultiDistanceHeuristic<hkaiNavVolumeGraph> Heuristic;
	typedef hkaiHashSearchState<Heuristic> SearchState;

	HK_FORCE_INLINE hkaiNavVolumeGraph();

	HK_FORCE_INLINE void init( const hkaiStreamingCollection::InstanceInfo* streamingInfo, const hkaiAstarCostModifier* costModifier, const hkaiAstarEdgeFilter* edgeFilter );
	HK_FORCE_INLINE void setStartPointAndCell( hkVector4Parameter startPoint, hkaiPackedKey startCellKey );
	HK_FORCE_INLINE void setGoalPointsAndCells( int numGoals, const hkVector4* goalPoints, const hkaiPackedKey* goalCellKeys );
	HK_FORCE_INLINE void setTraversalInfo ( const hkaiAgentTraversalInfo& info );
	HK_FORCE_INLINE void setSearchState( SearchState* ss );

	HK_FORCE_INLINE const hkaiStreamingCollection::InstanceInfo* getStreamingInfo() const;
	HK_FORCE_INLINE const hkaiAstarCostModifier* getCostModifier() const;
	HK_FORCE_INLINE const hkaiAstarEdgeFilter* getEdgeFilter() const;
	HK_FORCE_INLINE const hkaiAgentTraversalInfo& getTraversalInfo() const;

	//
	// Begin hkaiAstar interface
	//

	HK_FORCE_INLINE hkBool32 isGoal( SearchIndex nit ) const;
	HK_FORCE_INLINE int getMaxNeighborCount( SearchIndex nit ) const;
	HK_FORCE_INLINE void getNeighbors( SearchIndex nit, hkArrayBase< EdgeKey >& neighbors ) const;
	HK_FORCE_INLINE SearchIndex edgeTarget( SearchIndex nit, EdgeKey eit ) const;

	HK_FORCE_INLINE EdgeCost getTotalCost(SearchIndex nit, SearchIndex adj, EdgeKey eit, const EdgeCost costToParent) const;

	HK_FORCE_INLINE hkBool32 isEdgeTraversable(SearchIndex nit, SearchIndex adj, EdgeKey eit);

	HK_FORCE_INLINE hkBool32 isValidEdgeTarget( SearchIndex eit ) const;

	HK_FORCE_INLINE void getPosition( SearchIndex a, hkVector4& vecOut) const;
	HK_FORCE_INLINE void getPositionForHeuristic( SearchIndex a, hkVector4& vecOut) const;

	HK_FORCE_INLINE hkReal distance(SearchIndex A, SearchIndex B) const;

	HK_FORCE_INLINE hkSimdReal distanceFromPosition(hkVector4Parameter posA, hkVector4Parameter posB) const;

	//
	// Listener interface
	//

	inline void nextNode(SearchIndex nid);
	inline void nextEdge(SearchIndex nid, EdgeKey ekey);
	inline void edgeAccepted(SearchIndex nid, EdgeKey ekey) {}
	
	HK_FORCE_INLINE void setParent(SearchIndex cur, SearchIndex newParent, EdgeKey newEdge) { }
	HK_FORCE_INLINE void keepParent(SearchIndex cur, SearchIndex parent, EdgeKey edge) { }
	HK_FORCE_INLINE void nodeOpened(SearchIndex nid) {}
	HK_FORCE_INLINE void openNodeAdjusted(SearchIndex nid) {}
	HK_FORCE_INLINE void nodeClosed(SearchIndex) {}

	//
	// Cached cell/edge interface
	//

	HK_FORCE_INLINE const hkaiNavVolumeAccessor* getCurrentAccessor( ) const;
	HK_FORCE_INLINE const hkaiNavVolumeAccessor* getAdjacentAccessor( ) const;

	inline void setCurrentAccessor( int sectionId );
	inline void setAdjacentAccessor( int sectionId );
	inline void setAdjacentCachedCell( hkUint32 cellKey );

private:
	/// Streaming collection which contains volumes
	HK_PAD_ON_SPU( const hkaiStreamingCollection::InstanceInfo* ) m_streamingInfo ;

	/// Traversal info for the current search
	hkaiAgentTraversalInfo m_info;

	/// Cost modifier for the graph.
	/// hkaiAstarCostModifier was designed for use with the usual hkaiNavMeshGraph; as such, the callbacks consider 2 edges and the
	/// face that is "between" them. hkaiNavVolumeGraph only has the "outgoing" edge at a given time, though, so callbacks using the
	/// incoming edge might not work.
	const hkaiAstarCostModifier* m_costModifier;
	const hkaiAstarEdgeFilter* m_edgeFilter;

	// Start/goal info
	hkVector4 m_startPoint;
	hkaiPackedKey m_startCellKey;
	HK_PAD_ON_SPU(int) m_numGoals;
	HK_PAD_ON_SPU(const hkVector4*) m_endPoints;
	HK_PAD_ON_SPU(const hkaiPackedKey*) m_endCellKeys;


	HK_PAD_ON_SPU( SearchState* ) m_searchState;

	// Cached information

	HK_ON_CPU( const hkaiNavVolumeAccessor* m_cachedCurrentNavVolume );
	HK_ON_CPU( const hkaiNavVolumeAccessor* m_cachedAdjacentNavVolume );

	HK_ON_SPU( hkaiSpuNavVolumeAccessor m_currentAccessor; )
	HK_ON_SPU( hkaiSpuNavVolumeAccessor m_adjacentAccessor; )

	HK_PAD_ON_SPU(int)	m_cachedCurrentSectionId;
	HK_PAD_ON_SPU(int)	m_cachedAdjacentSectionId;

	// Updated during nextNode()
	hkaiNavVolume::Cell m_cachedCurrentCell;
	hkaiNavVolumeInstance::CellInstance m_cachedCurrentCellInstance;

	// Updated during nextEdge()
	hkaiNavVolume::Edge m_cachedCurrentEdge;
	hkaiNavVolume::Cell m_cachedAdjacentCell;

	hkAabb m_cachedCurrentAabb;
	hkAabb m_cachedAdjacentAabb;

	hkaiNavVolumeCellPairInfo m_cachedModifierInfo;

#ifdef HK_DEBUG
	int m_cachedCurrentCellKey;
	int m_cachedCurrentEdgeIndex;
	int m_cachedAdjacentCellKey;
#endif
};

#include <Ai/Pathfinding/Graph/hkaiNavVolumeGraph.inl>

#endif // HKAI_NAV_VOLUME_GRAPH_H

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
