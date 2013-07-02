/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


inline hkaiDirectedGraphExplicitCost::hkaiDirectedGraphExplicitCost()
{
}

inline void hkaiDirectedGraphExplicitCost::init(const hkaiDirectedGraphExplicitCost* gConst)
{
	// Do a const cast here - we never actually modify the pointers, but hkArray::setDataUserFree doesn't know that
	hkaiDirectedGraphExplicitCost* g = const_cast<hkaiDirectedGraphExplicitCost*> (gConst);	
	init( g->m_positions.begin(), g->m_nodes.begin(), g->m_edges.begin(), g->m_nodes.getSize(), g->m_edges.getSize());
}

inline void hkaiDirectedGraphExplicitCost::init(Position* pos, Node* nodes, Edge* edges, int numNodes, int numEdges)
{
	HK_ASSERT(0x542e5ea6, m_positions.isEmpty() && m_nodes.isEmpty() && m_edges.isEmpty());
	m_positions.setDataUserFree(pos, numNodes, numNodes);
	m_nodes.setDataUserFree(nodes, numNodes, numNodes);
	m_edges.setDataUserFree(edges, numEdges, numEdges);
}

inline void hkaiDirectedGraphExplicitCost::clearAndDeallocate()
{
	m_positions.clearAndDeallocate();
	m_nodes.clearAndDeallocate();
	m_edges.clearAndDeallocate();
}

inline void hkaiDirectedGraphExplicitCost::getPosition( SearchIndex a, hkVector4& v ) const
{
	v = m_positions[a];
}

inline const hkaiDirectedGraphExplicitCost::Node& hkaiDirectedGraphExplicitCost::getNode( SearchIndex a ) const
{
	return m_nodes[a];
}

inline const hkaiDirectedGraphExplicitCost::Edge&  hkaiDirectedGraphExplicitCost::getEdge( EdgeIndex e ) const
{
	return m_edges[e];
}

inline int hkaiDirectedGraphExplicitCost::getNumNodes() const
{
	return m_nodes.getSize();
}

inline int hkaiDirectedGraphExplicitCost::getNumEdges() const
{
	return m_edges.getSize();
}

inline const hkaiDirectedGraphExplicitCost::Edge* hkaiDirectedGraphExplicitCost::getEdgeBegin() const
{
	return m_edges.begin();
}

HK_FORCE_INLINE void hkaiDirectedGraphExplicitCost::Edge::init( SearchIndex target, hkReal cost )
{
	m_cost.setReal<true>(cost);
	m_flags = 0;
	m_target = target;
}

HK_FORCE_INLINE hkaiPackedKey hkaiDirectedGraphExplicitCost::Edge::getOppositeNodeKeyUnchecked() const
{
	return m_target;
}

HK_FORCE_INLINE hkaiDirectedGraphExplicitCost::NodeIndex hkaiDirectedGraphExplicitCost::Edge::getOppositeNodeIndex() const
{
	return hkaiGetIndexFromPacked(m_target);
}

HK_FORCE_INLINE void hkaiDirectedGraphExplicitCost::Edge::setTargetKey( hkaiPackedKey key)
{
	m_target = key;
}

HK_FORCE_INLINE hkBool32 hkaiDirectedGraphExplicitCost::Edge::isExternal() const
{
	HK_ASSERT(0x5203117f, m_flags.get() <= 127);
	return m_flags.anyIsSet( EDGE_EXTERNAL_OPPOSITE );
}

HK_FORCE_INLINE hkBool32 hkaiDirectedGraphExplicitCost::Edge::isUserEdge() const
{
	HK_ASSERT(0x5203117f, m_flags.get() <= 127);
	return m_flags.anyIsSet( EDGE_IS_USER );
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
