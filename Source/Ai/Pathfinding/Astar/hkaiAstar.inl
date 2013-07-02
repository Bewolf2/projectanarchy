/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template<
	typename GraphType,
	typename EdgeCostType,
	typename OpenSetType,
	typename FlagsType,
	typename ParentType,
	typename PathCostType,
	typename ListenerType
>
static typename OpenSetType::SearchIndex hkaiAstarSearchIteration(
	GraphType& graph,
	EdgeCostType& edgeCost,
	OpenSetType& openset,
	FlagsType& flags,
	ParentType& parent,
	PathCostType& pathCost,
	ListenerType& listener )
{
	typedef typename OpenSetType::SearchIndex SearchIndex;
	typedef typename GraphType::EdgeKey EdgeKey;
	typedef typename PathCostType::PathCost PathCost;

	HK_INTERNAL_TIMER_BEGIN_LIST("AStarIter", "Pop");

	SearchIndex curNodeIndex = openset.popNextSearchIndex();
	if ( graph.isGoal(curNodeIndex) )
	{
		// This node is in the goal set, stop searching.
		return curNodeIndex;
	}
	listener.nextNode(curNodeIndex);

	HK_INTERNAL_TIMER_SPLIT_LIST("EdgeIter");
	const PathCost currentPathCost = pathCost.getCost(curNodeIndex);

	// Create a stack-allocated array to hold all this node's neighbors.
	const int maxNumNeighbors = graph.getMaxNeighborCount( curNodeIndex );
	hkLocalArray< EdgeKey > neighborEdges( maxNumNeighbors );
	graph.getNeighbors( curNodeIndex, neighborEdges );

	const EdgeKey* const neighborsEnd = neighborEdges.end();
	for ( const EdgeKey* HK_RESTRICT curNeighbor = neighborEdges.begin(); curNeighbor < neighborsEnd; curNeighbor++ )
	{
		const EdgeKey curEdgeId = *curNeighbor;

		if ( flags.isFull() )
		{
			// If there's no space, we have to abort the search.
			break;
		}

		listener.nextEdge(curNodeIndex, curEdgeId);
		const SearchIndex adjNodeIndex = graph.edgeTarget( curNodeIndex, curEdgeId );

		HK_INTERNAL_TIMER_SPLIT_LIST("isValid");

		if( !edgeCost.isValidEdgeTarget(adjNodeIndex) )
		{
			continue;
		}

		HK_INTERNAL_TIMER_SPLIT_LIST("isTraversable");

		if ( !edgeCost.isEdgeTraversable(curNodeIndex, adjNodeIndex, curEdgeId) )
		{
			continue;
		}

		HK_INTERNAL_TIMER_SPLIT_LIST("CheckCost");

		const PathCost newCost = graph.getTotalCost(curNodeIndex, adjNodeIndex, curEdgeId, currentPathCost);

		if ( pathCost.isCostTooHigh(newCost) )
		{
			// We check the total cost below, but we might be able to reject some nodes here before 
			// opening them in the search state.
			continue;
		}

		listener.edgeAccepted(curNodeIndex, curEdgeId);

		const hkBool32 isNew = flags.isNew(adjNodeIndex);
		hkBool32 edgeRelaxed = true;
		//if( isNew == false ) // skip comparison for new nodes
		{
			PathCost oldCost = pathCost.getCost(adjNodeIndex);
			edgeRelaxed = newCost < oldCost;
		}

		if( isNew || edgeRelaxed )
		{
			HK_INTERNAL_TIMER_SPLIT_LIST("Set");
			listener.setParent(adjNodeIndex, curNodeIndex, curEdgeId );

			pathCost.setCost(adjNodeIndex, newCost);
			parent.setParentCurrent(adjNodeIndex, curNodeIndex);
			const hkBool32 isOpen = flags.isOpen(adjNodeIndex);
			const PathCost adjNodeEstimatedCost = pathCost.estimatedCostCurrent( adjNodeIndex );

			if ( pathCost.isCostTooHigh(adjNodeEstimatedCost) )
			{
				continue;
			}

			// Either the node is already opened, or it's about to get reopened.
			flags.markOpen( adjNodeIndex ); 

			if (isOpen == false )
			{
				HK_INTERNAL_TIMER_SPLIT_LIST("Insert");
				listener.nodeOpened( adjNodeIndex );

				// Check to make sure that we can add the node
				if ( openset.isFull() )
				{
					// If there's no space, we have to abort the search.
					break;
				}
				else
				{
					openset.insertSearchIndex( adjNodeIndex, adjNodeEstimatedCost );
				}
			}
			else // rare case, found a quicker way to an open node
			{
				HK_INTERNAL_TIMER_SPLIT_LIST("Adjust");
				listener.openNodeAdjusted( adjNodeIndex );
				openset.adjustValue( adjNodeIndex, adjNodeEstimatedCost );
			}
		}
		else
		{
			listener.keepParent(adjNodeIndex, curNodeIndex, curEdgeId );
		}
	}

	HK_INTERNAL_TIMER_SPLIT_LIST("Close");

	listener.nodeClosed( curNodeIndex );
	flags.markClosed( curNodeIndex );

	HK_INTERNAL_TIMER_END_LIST();
	return curNodeIndex;
}

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
	PathCostType& pathCost )
{
	hkaiNoopListener<typename OpenSetType::SearchIndex, typename GraphType::EdgeKey> listener;
	return hkaiAstarSearchIteration(graph, edgeCost, openset, flags, parent, pathCost, listener);
}

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
	AllStateType& allState )
{
	hkaiNoopListener<typename OpenSetType::SearchIndex, typename GraphType::EdgeKey> listener;
	return hkaiAstarSearchIteration(graph, edgeCost, openset, allState, allState, allState, listener);
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
