/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiHashSearchStateBase::hkaiHashSearchStateBase( )
:	m_nodes(HK_NULL),
	m_hashNext(HK_NULL),
	m_hashFirst(HK_NULL),
	m_nodeCapacity(0),
	m_nodeSize(0),
	m_heuristicWeight(1.0f)
{
	HK_ON_DEBUG( m_currentSearchIndex = SearchIndex(-1); )
	HK_ON_DEBUG( m_searchIndexToClose = SearchIndex(-1); )

	m_bestNode = hkUint32(-1);
	m_bestNodeCost = HK_REAL_MAX;
	m_maxPathCost = HK_REAL_MAX;
	
	m_dummyNode.init();
}

inline hkaiHashSearchStateBase::~hkaiHashSearchStateBase()
{

}

inline int hkaiHashSearchStateBase::getMemoryRequirement( int numNodes )
{
	//return 16*numNodes + 8*numNodes;
	return (sizeof(Node) * numNodes )  +  (sizeof(hkInt16) * numNodes)  +  (sizeof(hkInt16)*HASH_SIZE);
}

inline int hkaiHashSearchStateBase::getAllowedNumNodes( int numBytes )
{
	int numBytesMinusHash = numBytes - sizeof(hkInt16)*HASH_SIZE;
	return  hkMath::max2(0, numBytesMinusHash) / ( sizeof(Node)+ sizeof(hkInt16) );
}

inline void hkaiHashSearchStateBase::setHeuristicWeight( PathCost w )
{
	m_heuristicWeight = w;
}

inline hkBool32 hkaiHashSearchStateBase::isFull() const
{
	return m_nodeSize >= m_nodeCapacity;
}

inline void hkaiHashSearchStateBase::setParent( SearchIndex sid, SearchIndex pid )
{
	Node& thisNode = getNodeState(sid);
	if (pid == (hkUint32)(-1))
	{
		thisNode.m_parentIndex = -1;
	}
	else
	{
		const Node& parent = getNodeStateRO(pid);
		HK_ASSERT(0x1ac890d3, &parent != & m_dummyNode);
		int parentIdx = (int) (&parent - m_nodes);
		HK_ASSERT(0x50a0125e, parentIdx == (hkInt16) parentIdx );

		thisNode.m_parentIndex = (hkInt16) parentIdx;
	}
}

inline void hkaiHashSearchStateBase::setParentCurrent( SearchIndex sid, SearchIndex pid )
{
	HK_ASSERT(0x123e2ba4, m_nodes[m_parentIndex].m_index == pid);
	HK_ASSERT(0x585f2e94, sid == m_currentSearchIndex );
	Node& node = *m_currentNodeState;

	node.m_parentIndex = (hkInt16) m_parentIndex;
}

inline hkaiHashSearchStateBase::SearchIndex hkaiHashSearchStateBase::getParent( SearchIndex sid ) const
{
	const Node& thisNode = getNodeStateRO(sid);
	return (thisNode.m_parentIndex == -1) ? hkUint32(-1) : m_nodes[ thisNode.m_parentIndex ].m_index;
}

inline const hkaiSearchStateNode* hkaiHashSearchStateBase::getParent( const hkaiSearchStateNode* node) const
{
	return ( node->m_parentIndex == -1) ? HK_NULL : &m_nodes[ node->m_parentIndex ];
}

inline hkBool32 hkaiHashSearchStateBase::isStart( SearchIndex sid ) const
{
	return getNodeStateRO(sid).isStart();
}

inline hkBool32 hkaiHashSearchStateBase::isEnd( SearchIndex sid ) const
{
	return getNodeStateRO(sid).isEnd();
}

inline void hkaiHashSearchStateBase::setEndNode( SearchIndex sid )
{
	Node& node = getNodeState(sid);
	node.m_hCost = 0;
	node.setEnd();
}

inline hkBool32 hkaiHashSearchStateBase::isOpen( SearchIndex sid ) const
{
	HK_ASSERT(0x16ee2346, sid == m_currentSearchIndex );
	Node& node = *m_currentNodeState;

	return node.isOpen();
}

inline hkBool32 hkaiHashSearchStateBase::isClosed( SearchIndex sid ) const
{
	return getNodeStateRO(sid).isClosed();
}

inline hkBool32 hkaiHashSearchStateBase::isNew( SearchIndex sid ) const
{
	HK_ASSERT(0x76fa046a, sid == m_currentSearchIndex);
	return m_currentNodeState->isNew();
}

inline void hkaiHashSearchStateBase::markOpen( SearchIndex sid )
{
	HK_ASSERT(0x95a1a85, sid == m_currentSearchIndex );
	Node& node = *m_currentNodeState;

	node.setOpen();
}

inline void hkaiHashSearchStateBase::markClosed( SearchIndex sid )
{
	HK_ASSERT(0x58158e3e, m_searchIndexToClose == sid);

	m_parentNodeState->setClosed();
}

inline hkBool32 hkaiHashSearchStateBase::estimatedCostLess( SearchIndex a, SearchIndex b ) const
{
	const Node& nodeA = getNodeStateRO(a);
	const Node& nodeB = getNodeStateRO(b);
	const PathCost ca = nodeA.m_gCost + m_heuristicWeight * nodeA.m_hCost;
	const PathCost cb = nodeB.m_gCost + m_heuristicWeight * nodeB.m_hCost;
	return ca < cb;
}

inline hkaiHashSearchStateBase::PathCost hkaiHashSearchStateBase::estimatedCost( SearchIndex i ) const
{
	const Node& node = getNodeStateRO(i);

	const PathCost c = node.m_gCost + m_heuristicWeight * node.m_hCost;
	return c;
}

inline hkaiHashSearchStateBase::PathCost hkaiHashSearchStateBase::estimatedCostCurrent( SearchIndex i ) const
{
	const PathCost c = m_currentNodeState->m_gCost + m_heuristicWeight * m_currentNodeState->m_hCost;
	return c;
}

inline hkaiHashSearchStateBase::PathCost hkaiHashSearchStateBase::getCost( SearchIndex sid ) const
{
	HK_ASSERT2(0x25105174, sid == m_currentSearchIndex, "Mismatch in cached nodes. Did you forget to implement the nextNode interface?" );
	return m_currentNodeState->m_gCost;
}

inline bool hkaiHashSearchStateBase::isCostTooHigh( PathCost c ) const
{
	return c > m_maxPathCost;
}

inline void hkaiHashSearchStateBase::setCachedNode( SearchIndex sid )
{
	HK_ON_DEBUG(m_currentSearchIndex = sid);
	m_currentNodeState = &getNodeState(sid);
}

inline void hkaiHashSearchStateBase::nextNode( SearchIndex sid )
{
	setCachedNode(sid);
	HK_ASSERT(0x4bb31625, m_currentNodeState - m_nodes >= 0);
	m_parentIndex = hkInt16(m_currentNodeState - m_nodes);
	HK_ASSERT(0x68795eaf, m_parentIndex >=0 );

	m_parentNodeState = m_currentNodeState;
	HK_ON_DEBUG(m_searchIndexToClose = sid;)
}

inline void hkaiHashSearchStateBase::nextEdge( SearchIndex sid )
{
	if (sid != (SearchIndex)(-1))
	{
		setCachedNode(sid);
	}
	else
	{
		m_currentNodeState = HK_NULL;	
	}
}

inline void hkaiHashSearchStateBase::copyCosts( SearchIndex fromId, SearchIndex toId )
{
	HK_ASSERT(0x255ff893, fromId == m_currentSearchIndex);

	Node& toNode   = getNodeState(toId);
	Node& fromNode = *m_currentNodeState;
	toNode.m_gCost = fromNode.m_gCost;
	toNode.m_hCost = fromNode.m_hCost;
}

inline void hkaiHashSearchStateBase::copyCosts_randomAccess( SearchIndex fromId, SearchIndex toId )
{
	if( isFull() )
		return;
	Node& toNode   = getNodeState(toId);
	const Node& fromNode = getNodeStateRO(fromId);
	toNode.m_gCost = fromNode.m_gCost;
	toNode.m_hCost = fromNode.m_hCost;
}

inline hkUint32 hkaiHashSearchStateBase::hash( hkUint32 a )
{
	// Knuth golden ratio
	
	
	
	return a * 2654435761U;
}

//
// Template on the heuristic - only a few methods actually need the heuristic!
//

template<typename Heuristic>
inline hkaiHashSearchState<Heuristic>::hkaiHashSearchState( Heuristic* h /*= HK_NULL*/ )
:	hkaiHashSearchStateBase(),
	m_heuristic(h)
{
}

template<typename Heuristic>
inline void hkaiHashSearchState<Heuristic>::setHeuristic( Heuristic* h )
{
	m_heuristic = h;
}

template<typename Heuristic>
inline hkBool32 hkaiHashSearchState<Heuristic>::setStartNode( SearchIndex sid, PathCost initialCost /*= 0 */ )
{
	Node& node = getNodeState(sid);
	node.m_gCost = initialCost;
	PathCost heurCost = m_heuristic->getHeuristic(sid);
	node.m_hCost = heurCost;
	node.setStart();

	if ( heurCost < m_bestNodeCost )
	{
		m_bestNodeCost = heurCost;
		m_bestNode = sid;
	}

	return node.m_hCost < HK_REAL_MAX;
}

template<typename Heuristic>
inline void hkaiHashSearchState<Heuristic>::setCost( SearchIndex sid, PathCost d )
{
	HK_ASSERT(0x30ea1bab, sid == m_currentSearchIndex );
	Node& node = *m_currentNodeState;

	node.m_gCost = d;
	if( Heuristic::HEURISTIC_IS_INVARIANT == false || node.isNew() )
	{
		PathCost heurCost = m_heuristic->getHeuristic(sid);
		node.m_hCost = heurCost;

		if ( heurCost < m_bestNodeCost )
		{
			m_bestNodeCost = heurCost;
			m_bestNode = sid;
		}
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
