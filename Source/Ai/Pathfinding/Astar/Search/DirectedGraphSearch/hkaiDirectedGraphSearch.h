/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_DIRECTED_GRAPH_SEARCH_H
#define HK_AI_DIRECTED_GRAPH_SEARCH_H

#include <Ai/Pathfinding/Multithreaded/hkaiPathfindingJobs.h>

#include <Ai/Pathfinding/Astar/hkaiAstarParameters.h>

#include <Ai/Pathfinding/Astar/Heuristic/hkaiGraphDistanceHeuristic.h>

#include <Ai/Pathfinding/Astar/SearchState/hkaiHashSearchState.h>

#include <Ai/Pathfinding/Astar/OpenSet/hkaiHeapOpenSet.h>

#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.h>
#include <Ai/Pathfinding/Graph/hkaiDirectedGraphVisitor.h>


/// Utility for performing A* searches on a directed graph
struct hkaiDirectedGraphSearch
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiDirectedGraphSearch);
	typedef hkaiDirectedGraphVisitor Graph;
	typedef hkaiDirectedGraphAccessor GraphAccessor;
	
	typedef hkaiGraphDistanceHeuristic<Graph> Heuristic;
	
	typedef hkaiHashSearchState<Heuristic> SearchState;

	typedef hkaiHeapOpenSet OpenSet;

	typedef hkUint32 SearchIndex;

	enum
	{
		/// On multithreaded jobs, the amount of memory (in bytes) reserved for the open set
		OPEN_SET_SIZE = 8192,

		/// On multithreaded jobs, the amount of memory (in bytes) reserved for the search state
		SEARCH_STATE_SIZE = 37376,
	};

	hkaiDirectedGraphSearch(char* openSetStorage, int openSetStorageCount, char* searchStorage, int searchStorageCount);


		/// Initialize the graph
	void init(const Graph* graph, hkUint32 startNode, int endNode);

	void init(const Graph* graph, const hkUint32* startNodePtr, int numStartNodes, int endNode);
	
		/// Initialize the search state
	void initState();

		/// Set the start node. This should be called *after* settings the end node, so that the cost estimate
		/// for the start node can be computed.
	void setStartNode( SearchIndex nid );

		/// Set the goal node
	void setEndNode( SearchIndex nid );

		/// Do one A* iteration
	hkaiAstarOutputParameters::SearchStatus iteration();

	inline SearchIndex getParent(SearchIndex nid) const
	{
		return m_state.getParent(nid);
	}

	inline int isStart(int nid) const
	{
		//return m_state.getNodeState(nid).isStart();
		return m_state.isStart(nid);
	}
	
	inline hkReal getCost(int nid)
	{
		//return m_state.getNodeState(nid).getGCost();
		m_state.nextNode( nid );
		return m_state.getCost(nid);
	}

		/// Graph being searched
	HK_PAD_ON_SPU( Graph* ) m_graph;

		/// A* search state
	SearchState m_state;

		/// A* open set
	OpenSet m_openSet;

		/// A* heuristic
	Heuristic m_heuristic;
};

#endif // HK_SPU_AI_DIRECTED_GRAPH_ASTAR_H

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
