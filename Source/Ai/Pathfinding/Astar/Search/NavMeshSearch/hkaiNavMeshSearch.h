/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_NAV_MESH_SEARCH_H
#define HK_AI_NAV_MESH_SEARCH_H

#include <Ai/Pathfinding/Astar/hkaiAstarParameters.h>
#include <Ai/Pathfinding/Astar/Heuristic/hkaiHierarchicalNavMeshHeuristic.h>
#include <Ai/Pathfinding/Graph/hkaiNavMeshSectionGraph.h>

#include <Ai/Pathfinding/Astar/SearchState/hkaiHashSearchState.h>
#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.h>

#include <Ai/Pathfinding/Astar/OpenSet/hkaiHeapOpenSet.h>

	/// Utility for performing A* searches on a nav mesh.
	/// This is used by hkaiPathfindingUtility and the multithreaded jobs; it is recommended that you use those instead of
	/// using this directly.
struct hkaiNavMeshSearch
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshSearch);
	typedef hkaiNavMeshSectionGraph Graph;

	typedef hkaiDirectedGraphVisitor ClusterGraph;

	typedef struct hkaiHierarchicalNavMeshHeuristic Heuristic;
	typedef hkaiHashSearchState<Heuristic> SearchState;

	typedef hkaiHeapOpenSet OpenSet;

	typedef hkaiPackedKey SearchIndex;
	typedef hkaiPackedKey FaceIndex;

	/// DMA groups used on the SPU for prefetching data
	enum DmaGroups
	{
		STATE_INFO_PREFETCH,
		CHARACTER_INFO_PREFETCH,
		GOAL_PREFETCH
	};

		/// Memory initialization information
	struct MemInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshSearch::MemInfo);
		char* m_openSetStorage;
		char* m_searchStateStorage;
		int   m_openSetSize; // size in bytes
		int   m_searchStateSize; // size in bytes

		inline void setEmpty();
		inline void init( hkArray<char>& openSetArray, hkArray<char>& searchStateArray );
#ifndef HK_PLATFORM_SPU
		inline void init( hkArray<char>::Temp& openSetArray, hkArray<char>::Temp& searchStateArray );
#endif
	};

		/// Start point and goal information
	struct StartGoalInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshSearch::StartGoalInfo);
		hkVector4 m_startPoint;
		const hkVector4* m_goalPoints;
		hkaiPackedKey m_startFaceKey;
		const hkaiPackedKey* m_goalFaceKeys;
		int m_numGoals;
	};
	

	hkaiNavMeshSearch( const MemInfo& memInfo, const MemInfo& hierarchyMemInfo )
	:	m_openset(memInfo.m_openSetStorage, memInfo.m_openSetSize),
		m_heuristic(hierarchyMemInfo.m_openSetStorage, hierarchyMemInfo.m_openSetSize,
						hierarchyMemInfo.m_searchStateStorage, hierarchyMemInfo.m_searchStateSize)
	{
	}

		/// Initialize the search
	void init(Graph* graph, const MemInfo& memInfo, const hkaiAgentTraversalInfo& agentInfo, const StartGoalInfo& goalInfo, bool isHierarchical);

		/// Initialize the search state
	void initState();


	hkReal getCostForFace(hkaiNavMesh::FaceIndex faceIndex);

protected:
	HK_FORCE_INLINE const hkaiNavMesh::Face& getFace( hkaiPackedKey faceKey );
	HK_FORCE_INLINE const hkaiNavMesh::Edge& getEdge( hkaiPackedKey egdeKey );
	
public:

	/// Do one iteration of A*
	hkaiAstarOutputParameters::SearchStatus iteration();
		
		/// Pointer to the hkaiNavMeshGraph being searched
	hkPadSpu<Graph*> m_graph;

		/// A* search state
	SearchState m_state;

		/// A* open set
	OpenSet m_openset;

		/// A* Heuristic
	Heuristic m_heuristic;

		/// The most recent search node that was processed by A*.
		/// This will be a nav mesh edge, the start pseudo-node, or a goal-pseudo node
	SearchIndex m_lastClosedNode;

		/// On success, index of the goal that was reached. -1 on failure.
	int m_goalFoundIndex;

		/// Hierarchy graph (if applicable)
	ClusterGraph m_spuClusterGraph;

		/// Character diameter
	hkReal m_diameter;
};

#include <Ai/Pathfinding/Astar/Search/NavMeshSearch/hkaiNavMeshSearch.inl>
#endif // HK_AI_NAV_MESH_SEARCH_H

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
