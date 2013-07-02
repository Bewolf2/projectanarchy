/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_ASTAR_GRAPHDISTANCEHEURISTIC_H
#define HKAI_ASTAR_GRAPHDISTANCEHEURISTIC_H

	/// Heuristic which uses the Euclidean distance as an approximation for the cost to the goal.
template< typename Graph >
struct hkaiGraphDistanceHeuristic
{
	typedef typename Graph::EdgeCost PathCost;
	typedef int SearchIndex;

	enum{ HEURISTIC_IS_INVARIANT = 1};

	void init(const Graph* g, int e)
	{
		m_graph = g;
		m_endKey = e;
		g->getPositionForHeuristic(e, m_endPosition);
	}

	void setGoal(int e)
	{
		m_endKey = e;
		m_graph->getPositionForHeuristic(e, m_endPosition);
	}

	SearchIndex getGoalIndex() const
	{
		return m_endKey;
	}

	PathCost getHeuristic(SearchIndex nid) const
	{
		hkVector4 currentPosition;
		m_graph->getPositionForHeuristic(nid, currentPosition);
		return m_graph->distanceFromPosition(currentPosition, m_endPosition).getReal();
	}

	HK_PAD_ON_SPU(const Graph*) m_graph;
	HK_PAD_ON_SPU(SearchIndex) m_endKey;
	hkVector4 m_endPosition;
};

	/// Heuristic for multiple goals.
	/// The goal with the shortest Euclidean distance is used.
template< typename Graph >
struct hkaiGraphMultiDistanceHeuristic
{
	typedef typename Graph::Position Position;
	typedef typename Graph::EdgeCost PathCost;
	typedef int SearchIndex;
	
	enum{ HEURISTIC_IS_INVARIANT = 1};

		/// A goal node and its position.
	struct Goal
	{
		HK_ALIGN16(Position) m_position;
		hkPadSpu<SearchIndex> m_index;
	};

	void init(const Graph* g)
	{
		m_graph = g;
		m_numGoals = 0;
	}

	int getNumGoals()
	{
		return m_numGoals;
	}

	PathCost getHeuristic(SearchIndex nid) const
	{
		HK_ASSERT(0x76f9a897, m_numGoals > 0 );
		Position current;
		m_graph->getPositionForHeuristic(nid, current);
		hkSimdReal r = m_graph->distanceFromPosition( current, m_goals[0].m_position) ;

		for( int i = 1; i < m_numGoals; ++i )
		{
			r.setMin( r, m_graph->distanceFromPosition( current, m_goals[i].m_position) );
		}

		return r.getReal();
	}

	void addGoal(SearchIndex nid)
	{
		HK_ASSERT(0x1975e966, m_numGoals < MAX_NUM_GOALS && m_numGoals >= 0);

		Position p;
		m_graph->getPositionForHeuristic(nid, p);

		addGoal(nid, p);
	}

	void addGoal(SearchIndex nid, const Position& p)
	{
		HK_ASSERT(0x1975e966, m_numGoals < MAX_NUM_GOALS && m_numGoals >= 0);

		Goal& g = m_goals[m_numGoals];
		g.m_index = nid;
		g.m_position = p;

		m_numGoals = m_numGoals + 1;
	}

	SearchIndex indexOfGoal(int g) const
	{
		for (int i=0; i<m_numGoals; i++)
		{
			if (m_goals[i].m_index == g)
			{
				return i;
			}
		}
		return -1;
	}

	HK_PAD_ON_SPU(const Graph*) m_graph;
	HK_PAD_ON_SPU(int) m_numGoals;

	enum { MAX_NUM_GOALS = 4 };
	Goal m_goals[MAX_NUM_GOALS];

};

#endif // HKAI_ASTAR_GRAPHDISTANCEHEURISTIC_H

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
