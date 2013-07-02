/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_INDEXED_ASTAR_SEARCHSTATE_H
#define HKAI_INDEXED_ASTAR_SEARCHSTATE_H

#include <Ai/Pathfinding/Astar/SearchState/hkaiSearchState.h>

/// A simple Search State implementation that stores the states of all of its nodes.
/// This implementation is very fast, but can potentially take up a lot of space for large graphs,
/// even if only a few nodes are searched.
template<typename Heuristic>
struct hkaiIndexedSearchState
{
	typedef typename Heuristic::PathCost PathCost;
	typedef hkUint32 SearchIndex;
	typedef hkaiDeprecatedSearchStateNode Node;

	hkaiIndexedSearchState(Heuristic* h = HK_NULL)
		: m_heuristic(h), m_numClosedNodes(0), m_heuristicWeight(1)
	{
	}

	void setHeuristic(Heuristic* h)
	{
		m_heuristic = h;
	}

	void expand( int numNodes )
	{
		Node* s = m_states.expandBy( numNodes );
		for( int i = 0; i < numNodes; ++i )
		{
			s[i].init();
		}
	}

	void setNumNodes(int numNodes)
	{
		m_states.clear();
		expand(numNodes);
	}

	// Parent

	void setParent( SearchIndex sid, SearchIndex pid )
	{
		m_states[sid].setParent(pid);
	}
	
	void setParentCurrent( SearchIndex sid, SearchIndex pid )
	{
		setParent(sid, pid);
	}

	SearchIndex getParent( SearchIndex sid ) const
	{
		return m_states[sid].getParent();
	}


	// Utility

	void setStartNode( SearchIndex sid, PathCost initialCost = 0 )
	{
		m_states[sid].m_gCost = (hkReal) initialCost;
		m_states[sid].m_hCost = (hkReal) m_heuristic->getHeuristic(sid);
		m_states[sid].setStart();
	}

	void setEndNode( SearchIndex sid )
	{
		m_states[sid].m_hCost = 0;
		m_states[sid].setEnd();
	}

	// Flags

	hkBool32 isOpen( SearchIndex sid ) const
	{
		return m_states[sid].isOpen();
	}
	hkBool32 isClosed( SearchIndex sid ) const
	{
		return m_states[sid].isClosed();
	}
	hkBool32 isNew( SearchIndex sid ) const
	{
		return m_states[sid].isNew();
	}

	void markOpen( SearchIndex sid )
	{
		m_numClosedNodes -= m_states[sid].isClosed() ? 1 : 0;
		m_states[sid].setOpen();
		
	}
	void markClosed( SearchIndex sid )
	{
		m_states[sid].setClosed();
		++m_numClosedNodes;
	}
	
	// Estimate

	hkBool32 estimatedCostLess( SearchIndex a, SearchIndex b ) const
	{
		PathCost ca = m_states[a].m_gCost + m_heuristicWeight * m_states[a].m_hCost;
		PathCost cb = m_states[b].m_gCost + m_heuristicWeight * m_states[b].m_hCost;
		return ca < cb;
	}

	PathCost estimatedCost( SearchIndex i ) const
	{
		const PathCost c = (PathCost) (m_states[i].m_gCost + m_heuristicWeight * m_states[i].m_hCost);
		return c;
	}

	PathCost estimatedCostCurrent( SearchIndex i ) const
	{
		return estimatedCost(i);
	}

	// PathCost

	PathCost getCost( SearchIndex sid ) const
	{
		return (PathCost) m_states[sid].m_gCost;
	}

	PathCost getHeuristicCost( SearchIndex sid ) const
	{
		return m_states[sid].m_hCost;
	}

	void setCost( SearchIndex sid, PathCost d )
	{
		m_states[sid].m_gCost = (hkReal) d;
		if( Heuristic::HEURISTIC_IS_INVARIANT == false || m_states[sid].isNew() )
		{
			m_states[sid].m_hCost = (hkReal) m_heuristic->getHeuristic(sid);
		}
	}

	void copyCosts( SearchIndex fromId, SearchIndex toId)
	{
		m_states[toId].m_gCost = m_states[fromId].m_gCost;
		m_states[toId].m_hCost = m_states[fromId].m_hCost;
	}

	void copyCosts_randomAccess( SearchIndex fromId, SearchIndex toId)
	{
		copyCosts(fromId, toId);
	}

	const Node& getNodeState(SearchIndex nid)
	{
		return m_states[nid];
	}

	void nextAdjacentNode( SearchIndex nid ) {}

	hkArray<Node> m_states;
	Heuristic* m_heuristic;
	int m_numClosedNodes;
	PathCost m_heuristicWeight;
};

#endif // HKAI_INDEXED_ASTAR_SEARCHSTATE_H

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
