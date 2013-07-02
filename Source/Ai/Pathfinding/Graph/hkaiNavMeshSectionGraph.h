/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAV_MESH_SECTION_GRAPH_H
#define HKAI_NAV_MESH_SECTION_GRAPH_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/Astar/SearchState/hkaiHashSearchState.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshPathSearchParameters.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshTraversalUtils.h>
#include <Ai/Pathfinding/Astar/CostModifier/hkaiAstarCostModifier.h>
#include <Ai/Pathfinding/Astar/EdgeFilter/hkaiAstarEdgeFilter.h>

#ifdef HK_PLATFORM_SPU
#	include <Common/Base/Memory/PlatformUtils/Spu/SpuDmaCache/hkSpu4WayCache.h>
#	include <Ai/Pathfinding/Multithreaded/Spu/hkaiSpuPathfindingConfig.h>
#   include <Ai/Pathfinding/NavMesh/Spu/hkaiSpuNavMeshAccessor.h>
#endif



/// hkaiNavMeshGraph serves as the bridge between an hkaiNavMesh and the hkaiAStar system.
/// The nav mesh edges are treated as nodes in the A* search.
/// Users don't need to use this directly; instead they should use hkaiPathfindingUtil or hkaiNavMeshAstarJobs.
/// This particular implementation keeps a local copy of the current face and relevant edges,
/// which avoids DMA/cache checks on SPU, and cache misses on CPU
struct hkaiNavMeshSectionGraph
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshSectionGraph);
	
	enum Flags
	{
		CHECK_SPHERE_RADIUS = 1,
		CHECK_CAPSULE_RADIUS = 2,
		CHECK_TRAVERSAL_RADIUS = 4,
		CHECK_USER_EDGE_TRAVERSAL_RADIUS = 8,
		PROJECT_RADIUS_CHECKS = 16,
		USE_GRANDPARENT_DISTANCE_CALCULATION = 32
	};

	typedef hkaiPackedKey SearchIndex;	// Search nodes are nav mesh edge keys
	typedef hkaiPackedKey EdgeKey;
	typedef hkReal EdgeCost;
	typedef hkVector4 Position;

	typedef struct hkaiHierarchicalNavMeshHeuristic Heuristic;
	friend struct hkaiHierarchicalNavMeshHeuristic;

	typedef hkaiHashSearchState<Heuristic> SearchState;

	HK_FORCE_INLINE hkaiNavMeshSectionGraph();

	HK_FORCE_INLINE void init( 
		const hkaiStreamingCollection::InstanceInfo* streamingInfo, 
		hkVector4Parameter up,
		const hkaiAstarCostModifier* costModifier,
		const hkaiAstarEdgeFilter* edgeFilter
		);
	HK_FORCE_INLINE void setStartPointAndFace( hkVector4Parameter startPoint, hkaiPackedKey startFaceKey );
	HK_FORCE_INLINE void setGoalPointsAndFaces( int numGoals, const hkVector4* goalPoints, const hkaiPackedKey* goalFaceKeys );
	HK_FORCE_INLINE void setRadiusLimits( hkReal sphereRadius, hkReal capsuleRadius, bool project);
	HK_FORCE_INLINE void setTraversalInfo ( const hkaiAgentTraversalInfo& info, hkReal simpleSmoothingThreshold, 
											hkaiNavMeshPathSearchParameters::UserEdgeTraversalTestType userEdgeTraversalTestType,
											bool useGrandparentDistance );
	HK_FORCE_INLINE void setSearchState( SearchState* ss );
	HK_FORCE_INLINE void resetFlags();

	HK_FORCE_INLINE const hkaiStreamingCollection::InstanceInfo* getStreamingInfo() const;
	HK_FORCE_INLINE const hkVector4& getUp() const;
	HK_FORCE_INLINE const hkaiAstarCostModifier* getCostModifier() const;
	HK_FORCE_INLINE const hkaiAgentTraversalInfo& getTraversalInfo() const;

	//
	// Begin hkaiAstar interface
	//

	HK_FORCE_INLINE hkBool32 isStart( SearchIndex nit ) const;
	HK_FORCE_INLINE hkBool32 isGoal( SearchIndex nit ) const;
	HK_FORCE_INLINE int getMaxNeighborCount( SearchIndex nit ) const;
	HK_FORCE_INLINE void getNeighbors( SearchIndex incomingEdgePacked, hkArrayBase< EdgeKey >& neighborEdgesPacked ) const;
	HK_FORCE_INLINE SearchIndex edgeTarget( SearchIndex nit, EdgeKey eit ) const;
	HK_FORCE_INLINE EdgeCost getTotalCost(SearchIndex incomingEdge, SearchIndex outgoingEdge, EdgeKey eit_unused, const EdgeCost pathCost) const;
	HK_FORCE_INLINE hkBool32 isEdgeTraversable(SearchIndex nit, SearchIndex adj, EdgeKey eit) const;
	HK_FORCE_INLINE hkBool32 isValidEdgeTarget( SearchIndex eit ) const;
	HK_FORCE_INLINE void getPositionForHeuristic( SearchIndex a, hkVector4& posOut) const;
	HK_FORCE_INLINE hkSimdReal distanceFromPosition(hkVector4Parameter posA, hkVector4Parameter posB) const;

	//
	// Listener interface
	//

	HK_FORCE_INLINE void nextNode(SearchIndex nid);
	HK_FORCE_INLINE void nextEdge(SearchIndex nid, EdgeKey ekey);
	HK_FORCE_INLINE void edgeAccepted(SearchIndex nid, EdgeKey ekey);
	HK_FORCE_INLINE void setParent(SearchIndex cur, SearchIndex newParent, EdgeKey newEdge) { }
	HK_FORCE_INLINE void keepParent(SearchIndex cur, SearchIndex parent, EdgeKey edge) { }
	HK_FORCE_INLINE void nodeOpened(SearchIndex nid) {}
	HK_FORCE_INLINE void openNodeAdjusted(SearchIndex nid) {}
	HK_FORCE_INLINE void nodeClosed(SearchIndex) {}

	//
	// Cached face/edge interface
	// Note that some methods are overloaded; this allows extra checks that the requested indices are consistent
	//
	HK_FORCE_INLINE const hkaiNavMesh::Face& setCachedFace(hkaiPackedKey faceKey);
	HK_FORCE_INLINE const hkaiNavMesh::Edge& setCachedIncomingEdge(EdgeKey ekey);
	HK_FORCE_INLINE const hkaiNavMesh::Edge& setCachedOutgoingEdge(EdgeKey ekey);

private:
	HK_FORCE_INLINE const hkaiNavMesh::Face& getCachedFace() const;
	HK_FORCE_INLINE const hkVector4&		 getCachedOutgoingEdgeMidpoint() const;
	HK_FORCE_INLINE const hkaiNavMesh::Edge& getCachedOutgoingEdge();
	HK_FORCE_INLINE const hkaiNavMesh::Edge& getCachedOutgoingEdge(EdgeKey eIdx);
	HK_FORCE_INLINE const hkaiNavMesh::Edge& getCachedIncomingEdge();
	HK_FORCE_INLINE const hkaiNavMesh::Edge& getCachedIncomingEdge(EdgeKey eIdx);

	HK_FORCE_INLINE const hkaiNavMeshAccessor* getIncomingAccessor( ) const;
	HK_FORCE_INLINE const hkaiNavMeshAccessor* getOutgoingAccessor( ) const;

	HK_FORCE_INLINE void setIncomingAccessor( hkaiPackedKey key );
	HK_FORCE_INLINE void setOutgoingAccessor( hkaiPackedKey key );

private:
	HK_PAD_ON_SPU( const hkaiStreamingCollection::InstanceInfo* ) m_streamingInfo ;

	hkaiNavMesh::Face		m_cachedFace;

	hkaiNavMesh::Edge		m_cachedIncomingEdge;
	hkVector4				m_cachedIncomingEdgeVertA;
	hkVector4				m_cachedIncomingEdgeVertB;
	hkVector4				m_cachedIncomingEdgeMidpoint;

	hkaiNavMesh::Edge		m_cachedOutgoingEdge;
	hkVector4				m_cachedOutgoingEdgeVertA;
	hkVector4				m_cachedOutgoingEdgeVertB;
	hkVector4				m_cachedOutgoingEdgeMidpoint;

	// Opposite the incoming edge
	// "opposite" and "outgoing" are both on the same face and therefore in the same section
	// We use the vertices from this edge during traversability checks
	hkaiNavMesh::Edge		m_cachedOppositeEdge;
	hkVector4				m_cachedOppositeVertexA;
	hkVector4				m_cachedOppositeVertexB;

	// "Grandparent" information
	// This is the current best edge that leads to the incoming edge.
	hkVector4				m_grandparentMidpoint;
	hkSimdReal				m_grandparentCost;

	mutable hkaiNavMeshEdgePairInfo m_cachedEdgePairInfo;

	hkaiAgentTraversalInfo	m_info;

	// Up vector, needed for traversability checks
	hkVector4				m_up;

	// Start/goal info
	hkVector4 m_startPoint;
	hkaiPackedKey m_startFaceKey;
	HK_PAD_ON_SPU(int) m_numGoals;
	HK_PAD_ON_SPU(const hkVector4*) m_endPoints;
	HK_PAD_ON_SPU(const hkaiPackedKey*) m_endFaceKeys;

	// Leashing
	hkSimdReal m_searchSphereRadius;
	hkSimdReal m_searchCapsuleRadius;

	HK_PAD_ON_SPU(const hkaiAstarCostModifier*) m_costModifier;
	HK_PAD_ON_SPU(const hkaiAstarEdgeFilter*) m_edgeFilter;

	HK_PAD_ON_SPU(SearchState*)	m_searchState;

	HK_ON_CPU(const hkaiNavMeshInstance* m_cachedIncomingNavMesh);
	HK_ON_CPU(const hkaiNavMeshInstance* m_cachedOutgoingNavMesh);
	HK_ON_SPU(hkaiSpuNavMeshAccessor m_incomingAccessor);
	HK_ON_SPU(hkaiSpuNavMeshAccessor m_outgoingAccessor);


	HK_PAD_ON_SPU(hkaiPackedKey)	m_cachedFaceKey;
	HK_PAD_ON_SPU(int)	m_cachedIncomingSectionId;
	HK_PAD_ON_SPU(int)	m_cachedOutgoingSectionId;

	HK_PAD_ON_SPU(hkUint32) m_flags;

	// Just used for asserts/debugging
#ifdef HK_DEBUG
	SearchIndex m_cachedIncomingEdgeIndex;
	SearchIndex m_cachedOutgoingEdgeIndex;
	SearchIndex m_cachedOppositeEdgeIndex;
#endif
};

#include <Ai/Pathfinding/Graph/hkaiNavMeshSectionGraph.inl>
#endif // HKAI_NAV_MESH_GRAPH_H

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
