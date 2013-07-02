/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKASTAR_HASH_SEARCHSTATE_H
#define HKASTAR_HASH_SEARCHSTATE_H
#include <Ai/Pathfinding/Astar/SearchState/hkaiSearchState.h>

/// Search state implementation that uses a hash table.
struct hkaiHashSearchStateBase
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, hkaiHashSearchStateBase);

	typedef hkReal PathCost;
	typedef hkUint32 SearchIndex;
	typedef hkaiSearchStateNode Node;

	enum
	{
		HASH_SIZE = 256,
		MAX_CAPACITY = 0x8000,
	};


	HK_FORCE_INLINE hkaiHashSearchStateBase();
	HK_FORCE_INLINE ~hkaiHashSearchStateBase();

	/// Returns the number of bytes required to store the specified number of nodes
	HK_FORCE_INLINE static int getMemoryRequirement( int numNodes );

	/// Returns the number of nodes that can be stored in the specified number of bytes.
	HK_FORCE_INLINE static int getAllowedNumNodes( int numBytes );

	HK_FORCE_INLINE void setHeuristicWeight(PathCost w);
	void setStorage( char* storage, int size);
	HK_FORCE_INLINE hkBool32 isFull() const;

	// Parent

	HK_FORCE_INLINE void setParent( SearchIndex sid, SearchIndex pid );
	HK_FORCE_INLINE void setParentCurrent( SearchIndex sid, SearchIndex pid );
	HK_FORCE_INLINE SearchIndex getParent( SearchIndex sid ) const;
	HK_FORCE_INLINE const Node* getParent( const Node* sid ) const;
	HK_FORCE_INLINE hkBool32 isStart( SearchIndex sid ) const;
	HK_FORCE_INLINE hkBool32 isEnd( SearchIndex sid ) const;


	// Utility

	HK_FORCE_INLINE void setEndNode( SearchIndex sid );

	// Flags

	HK_FORCE_INLINE hkBool32 isOpen( SearchIndex sid ) const;
	HK_FORCE_INLINE hkBool32 isClosed( SearchIndex sid ) const;
	HK_FORCE_INLINE hkBool32 isNew( SearchIndex sid ) const;

	HK_FORCE_INLINE void markOpen( SearchIndex sid );
	HK_FORCE_INLINE void markClosed( SearchIndex sid );


	// Estimate

	HK_FORCE_INLINE hkBool32 estimatedCostLess( SearchIndex a, SearchIndex b ) const;
	HK_FORCE_INLINE PathCost estimatedCost( SearchIndex i ) const;
	HK_FORCE_INLINE PathCost estimatedCostCurrent( SearchIndex i ) const;


	// PathCost
	HK_FORCE_INLINE PathCost getCost( SearchIndex sid ) const;
	HK_FORCE_INLINE bool isCostTooHigh( PathCost c) const;


	// Direct access
	/// Returns the node for the specified key, or creates a new one if not found.
	Node& getNodeState(SearchIndex sid );
	/// Returns the node for the specified key, or a blank node if none found
	const Node& getNodeStateRO(SearchIndex sid ) const;


	HK_FORCE_INLINE void setCachedNode(SearchIndex sid);
	HK_FORCE_INLINE void nextNode(SearchIndex sid);
	HK_FORCE_INLINE void nextEdge(SearchIndex sid);
	HK_FORCE_INLINE static hkUint32 hash(hkUint32 x);


	HK_FORCE_INLINE void copyCosts( SearchIndex fromId, SearchIndex toId);
	/// Used during setup when we can't guarantee anything about the cached state
	HK_FORCE_INLINE void copyCosts_randomAccess( SearchIndex fromId, SearchIndex toId);

	HK_PAD_ON_SPU( Node* ) m_nodes;
	HK_PAD_ON_SPU( hkInt16* ) m_hashNext;
	HK_PAD_ON_SPU( hkInt16* ) m_hashFirst;

	HK_PAD_ON_SPU( int ) m_nodeCapacity;
	HK_PAD_ON_SPU( int ) m_nodeSize;

	// An uninitialized node that is returned for 
	Node m_dummyNode;

	// Keep track of the best node we've found, and its cost.
	// This lets us get a good path even if we have to break out of the search early.
	HK_PAD_ON_SPU( SearchIndex ) m_bestNode;
	HK_PAD_ON_SPU( PathCost )    m_bestNodeCost;
	HK_PAD_ON_SPU( PathCost )    m_maxPathCost;

	HK_PAD_ON_SPU(PathCost) m_heuristicWeight;

	// The index (in the node array) that new nodes will get their parents set to.
	HK_PAD_ON_SPU(int) m_parentIndex;

	//
	// Some extra bookkeeping to avoid multiple hash lookups of the same item
	//

	/// A pointer to the most recent node.
	HK_PAD_ON_SPU(Node*) m_currentNodeState;

	/// A pointer to the next node that we'll close. This is set in nextNode() and used in markClosed()
	HK_PAD_ON_SPU(Node*) m_parentNodeState;

	//
	// Debug info
	//
	HK_ON_DEBUG(SearchIndex m_currentSearchIndex);
	HK_ON_DEBUG(SearchIndex m_searchIndexToClose);
};

	/// Search state implementation that uses a hash table.
template<typename Heuristic>
struct hkaiHashSearchState : public hkaiHashSearchStateBase
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, hkaiHashSearchStateBase);
	hkaiHashSearchState( Heuristic* h = HK_NULL );
	HK_FORCE_INLINE void setHeuristic(Heuristic* h);
	HK_FORCE_INLINE hkBool32 setStartNode( SearchIndex sid, PathCost initialCost = 0 );
	HK_FORCE_INLINE void setCost( SearchIndex sid, PathCost d );

	HK_PAD_ON_SPU(Heuristic*) m_heuristic;
};

#include <Ai/Pathfinding/Astar/SearchState/hkaiHashSearchState.inl>

#endif // HKASTAR_SPUSEARCHSTATE_H

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
