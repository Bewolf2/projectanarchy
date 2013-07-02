/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_ASTAR_SEARCH_H
#define HKAI_ASTAR_SEARCH_H

#include <Common/Base/Container/LocalArray/hkLocalArray.h>
#include <Common/Base/Monitor/hkMonitorStream.h>

//#define HK_ASTAR_DETAILED_TIMERS

#ifdef HK_ASTAR_DETAILED_TIMERS
#undef HK_INTERNAL_TIMER_BEGIN
#undef HK_INTERNAL_TIMER_END
#undef HK_INTERNAL_TIMER_BEGIN_LIST
#undef HK_INTERNAL_TIMER_SPLIT_LIST
#undef HK_INTERNAL_TIMER_END_LIST
#undef HK_INTERNAL_MONITOR_ADD_VALUE

#	define HK_INTERNAL_TIMER_BEGIN(NAME, OBJECT)		HK_TIMER_BEGIN(NAME,OBJECT)
#	define HK_INTERNAL_TIMER_END				HK_TIMER_END
#	define HK_INTERNAL_TIMER_BEGIN_LIST(a,b)	HK_TIMER_BEGIN_LIST(a,b)
#	define HK_INTERNAL_TIMER_SPLIT_LIST(a)		HK_TIMER_SPLIT_LIST(a)
#	define HK_INTERNAL_TIMER_END_LIST			HK_TIMER_END_LIST
#	define HK_INTERNAL_MONITOR_ADD_VALUE(NAME, VALUE, MONITOR_TYPE)		HK_MONITOR_ADD_VALUE(NAME, VALUE, MONITOR_TYPE)
#endif

/// Empty implementation of the A* Listener interface.
/// The simplest for of hkaiAstarSearchIteration will create an hkaiNoopListener and use this for the listener.
template< typename SearchIndex, typename EdgeKey >
struct hkaiNoopListener
{
	inline void nextNode(SearchIndex nid) { }
	inline void nextEdge(SearchIndex nid, EdgeKey ekey) { }

	inline void setParent(SearchIndex cur, SearchIndex newParent, EdgeKey newEdge) { }
	inline void keepParent(SearchIndex cur, SearchIndex parent, EdgeKey edge) { }

	inline void nodeOpened(SearchIndex) {}
	inline void openNodeAdjusted(SearchIndex) {}
	inline void nodeClosed(SearchIndex) {}

#if 0
	nextNode ->
		( nextEdge ->
			(reParent -> (newNodeQueued|openNodeAdjusted|closedNodeReopened))
			(keepParent)
	nodeClosed
#endif
};

template<
	typename GraphType,
		//hkBool32 isGoal( SearchIndex nit ) const
		//int getMaxNeighborCount( SearchIndex nit ) const
		//void getNeighbors( SearchIndex nit, hkArrayBase< EdgeKey >& neighbors ) const
		//SearchIndex edgeTarget(SearchIndex, EdgeKey )
		//Cost getTotalCost(SearchIndex nit, SearchIndex adj, EdgeKey, EdgeCost costToParent) 
	typename EdgeCostType,
		//Cost edgeCost(SearchIndex nit, EdgeKey eit)
		//hkBool32 isValidEdgeTarget( SearchIndex eit ) const
		//hkBool32 isEdgeTraversable(SearchIndex nit, SearchIndex adj, EdgeKey eit, TraversalInfo info)
	typename OpenSetType,
		//SearchIndex popOpenNode()
		//void pushOpenNode(SearchIndex nid)
	typename FlagsType,
		//hkBool32 isNew(SearchIndex nid) const
		//hkBool32 isOpen(SearchIndex nid) const
		//hkBool32 isClosed(SearchIndex nid) const
		//void markOpen(SearchIndex nid)
		//void markClosed(SearchIndex nid)
	typename ParentType,
		//void setParent(SearchIndex nid, SearchIndex parentId)
	typename PathCostType,
		//Cost getCost(SearchIndex nid) const
		//void setCost(SearchIndex nid, CostId c);
	typename ListenerType
		// nextNode(SearchIndex nid)
		// nextEdge(SearchIndex nid, EdgeKey ekey)
		// setParent(SearchIndex nid, SearchIndex parent, EdgeKey edge)
		// keepParent(SearchIndex nid, SearchIndex parent, EdgeKey edge)
		// nodeOpened(SearchIndex)
		// openNodeAdjusted(SearchIndex)
		// nodeClosed(SearchIndex)
	>
static typename OpenSetType::SearchIndex hkaiAstarSearchIteration(
	GraphType& graph,
	EdgeCostType& edgeCost,
	OpenSetType& openset,
	FlagsType& flags,
	ParentType& parent,
	PathCostType& pathCost,
	ListenerType& listener );

template<
	typename GraphType,
	typename EdgeCostType,
	typename OpenSetType,
	typename FlagsType,
	typename ParentType,
	typename PathCostType
	>
static typename OpenSetType::SearchIndex hkaiAstarSearchIteration(
	GraphType& graph,
	EdgeCostType& edgeCost,
	OpenSetType& openset,
	FlagsType& flags,
	ParentType& parent,
	PathCostType& pathCost );

template<
	typename GraphType,
	typename EdgeCostType,
	typename OpenSetType,
	typename AllStateType
	>
static typename OpenSetType::SearchIndex hkaiAstarSearchIteration(
	GraphType& graph,
	EdgeCostType& edgeCost,
	OpenSetType& openset,
	AllStateType& allState );

#include <Ai/Pathfinding/Astar/hkaiAstar.inl>

#endif // HKAI_ASTAR_SEARCH_H

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
