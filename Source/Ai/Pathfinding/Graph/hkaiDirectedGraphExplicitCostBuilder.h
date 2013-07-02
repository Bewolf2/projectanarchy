/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_ASTAR_DIRECTEDGRAPHEXPLICITCOST_BUILDER_H
#define HKAI_ASTAR_DIRECTEDGRAPHEXPLICITCOST_BUILDER_H

#include <Ai/Pathfinding/Utilities/Hierarchy/hkaiHierarchyUtils.h>
#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.h>

/// Interface for constructing graphs.
struct hkaiGraphBuilder
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiGraphBuilder);
	typedef int PositionId;
	typedef int EdgeKey;
	virtual ~hkaiGraphBuilder() {}
	virtual PositionId addPosition( hkVector4Parameter p ) = 0;
	virtual EdgeKey addEdge( PositionId a, PositionId b, hkReal w ) = 0;
	virtual void extraPositionData( PositionId, int tag, void* value ) {}
	virtual void extraEdgeData( EdgeKey, int tag, void* value ) {}
};

/// Utility class for constructing graphs
struct hkaiDirectedGraphExplicitCostBuilder : public hkaiGraphBuilder
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiDirectedGraphExplicitCostBuilder);
	typedef hkaiDirectedGraphExplicitCost::Node Node;
	typedef hkaiDirectedGraphExplicitCost::Position Position;
	typedef hkaiDirectedGraphExplicitCost::Edge Edge;
	
		/// A potential edge to be constructed in the graph.
	struct Link
	{
		int a; 
		int b; 
		hkReal w;
	};

	virtual PositionId addPosition( hkVector4Parameter p )
	{
		m_positions.pushBack(p);
		return m_positions.getSize() - 1;
	}
	virtual EdgeKey addEdge( PositionId a, PositionId b, hkReal w )
	{
		Link& l = m_links.expandOne();
		l.a = a; l.b = b; l.w = w;
		return m_links.getSize() - 1;
	}

	hkaiDirectedGraphExplicitCost* build(hkaiDirectedGraphExplicitCost* d)
	{
		hkArray<Node> nodes; nodes.setSize(m_positions.getSize());
		hkArray<Edge> edges;

		for( int ni = 0; ni < nodes.getSize(); ++ni )
		{
			Node& n = nodes[ni];
			n.m_numEdges = 0;
			n.m_startEdgeIndex = edges.getSize();
			for( int li = 0; li < m_links.getSize(); ++li )
			{
				const Link& l = m_links[li];
				if( l.a == ni )
				{
					n.m_numEdges += 1;
					Edge e; 
					e.init(l.b, l.w);
					edges.pushBack(e);
				}
			}
		}
		d->m_positions.swap(m_positions);
		d->m_nodes.swap(nodes);
		d->m_edges.swap(edges);
		return d;
	}
	hkArray<Position> m_positions;
	hkArray<Link> m_links;
};

#endif // HKAI_ASTAR_DIRECTEDGRAPHEXPLICITCOST_BUILDER_H

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
