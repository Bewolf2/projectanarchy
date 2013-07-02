/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_PLATFORM_SPU
inline void hkaiDirectedGraphInstance::setOriginalPointers( const hkaiDirectedGraphExplicitCost* graph )
{
	m_originalNodes		= graph->m_nodes.begin();
	m_numOriginalNodes	= graph->m_nodes.getSize();

	m_originalEdges		= graph->m_edges.begin();
	m_numOriginalEdges	= graph->m_edges.getSize();

	m_originalPositions	= graph->m_positions.begin();
}

inline hkaiSectionUid hkaiDirectedGraphInstance::getSectionUid() const
{
	return m_sectionUid;
}

inline int hkaiDirectedGraphInstance::getNumOwnedEdges() const
{
	return m_ownedEdges.getSize();
}

inline const hkaiDirectedGraphExplicitCost::Edge* hkaiDirectedGraphInstance::getOwnedEdgeBegin() const
{
	return m_ownedEdges.begin();
}

inline const hkaiDirectedGraphExplicitCost::Node& hkaiDirectedGraphInstance::getNode( NodeIndex n ) const
{
	HK_ASSERT(0x21bdb0b3, n >= 0 && n < m_numOriginalNodes);
	return m_originalNodes[n];
}

inline void hkaiDirectedGraphInstance::setTransform( const hkTransform& t )
{
	m_transform = t;
}

inline const hkaiDirectedGraphExplicitCost* hkaiDirectedGraphInstance::getOriginalGraph() const
{
	return m_originalGraph;
}

inline void hkaiDirectedGraphInstance::setSectionUid( hkaiSectionUid uid )
{
	m_sectionUid = uid;
}

inline void hkaiDirectedGraphInstance::setRuntimeId( hkaiRuntimeIndex id )
{
	m_runtimeId = id;
}

HK_FORCE_INLINE const hkaiDirectedGraphExplicitCost::Edge& hkaiDirectedGraphInstance::getEdge( EdgeIndex e ) const
{ 
	HK_ASSERT(0x557b576c, e >= 0 && e < m_numOriginalEdges + m_ownedEdges.getSize() );

	const int numOriginalEdges = m_numOriginalEdges;
	if (e < numOriginalEdges)
	{
		return m_originalEdges[e];
	}
	else
	{
		int idx = e - numOriginalEdges;
		return m_ownedEdges[idx];
	}
}

inline void hkaiDirectedGraphInstance::getInstancedNode( NodeIndex n, hkaiDirectedGraphExplicitCost::Node& nodeOut ) const
{
	int mappedIdx = m_nodeMap.getSize() ? m_nodeMap[n] : -1;
	
	// Not instanced
	if (mappedIdx == -1) 
	{
		nodeOut.m_startEdgeIndex = -1;
		nodeOut.m_numEdges = 0;
	}
	else
	{
		nodeOut = m_instancedNodes[mappedIdx];
	}
}

inline hkaiDirectedGraphExplicitCost::Node* hkaiDirectedGraphInstance::getInstancedNodePtr( NodeIndex n )
{
	int mappedIdx = m_nodeMap.getSize() ? m_nodeMap[n] : -1;

	// Not instanced
	if (mappedIdx == -1) 
	{
		return HK_NULL;
	}
	else
	{
		return &m_instancedNodes[mappedIdx];
	}
}


inline hkaiDirectedGraphExplicitCost::Node& hkaiDirectedGraphInstance::getWritableInstancedNode( NodeIndex n )
{
	HK_ASSERT2(0x390b0eba, m_nodeMap.getSize() == m_numOriginalNodes, "Can't instance nodes if hkaiDirectedGraphInstance::tempInit was used" );

	int mappedIndex = m_nodeMap[n];
	if (mappedIndex == -1)
	{
		mappedIndex = m_instancedNodes.getSize();
		m_nodeMap[n] = mappedIndex;
		hkaiDirectedGraphExplicitCost::Node& newNode = m_instancedNodes.expandOne();
		newNode.m_numEdges = 0;
		newNode.m_startEdgeIndex = -1;
	}

	return m_instancedNodes[mappedIndex];
}

inline hkaiDirectedGraphExplicitCost::Edge& hkaiDirectedGraphInstance::getWritableEdge( EdgeIndex e )
{
	HK_ASSERT2(0x2a785951, e >= m_numOriginalEdges, "Can't get write access to an original edge.");
	int idx = e - m_numOriginalEdges;
	return m_ownedEdges[idx];
}

inline void hkaiDirectedGraphInstance::getLocalPosition( NodeIndex pid, hkVector4& posOut ) const
{
	HK_ASSERT(0x1b391b2c, pid >= 0 && pid < m_numOriginalNodes );
	posOut = m_originalPositions[pid];
}

inline int hkaiDirectedGraphInstance::getNumNodes() const
{
	return m_numOriginalNodes;
}

#endif //!SPU


///////////////////////////////////////////////////////////////////////////////
#ifdef HK_PLATFORM_SPU
inline hkaiSpuDirectedGraphAccessor::hkaiSpuDirectedGraphAccessor()
{	
	m_originalNodes = HK_NULL;
	m_numOriginalNodes = -1;

	m_originalEdges = HK_NULL;
	m_numOriginalEdges = -1;

	m_originalPositions = HK_NULL;

	m_runtimeId = HKAI_INVALID_RUNTIME_INDEX;
	m_nodeMap = HK_NULL;
	
	m_instancedNodes = HK_NULL;
	m_numInstancedNodes = -1;
	
	m_ownedEdges = HK_NULL;
	m_numOwnedEdges = -1;

	m_graphMainMem = HK_NULL;
}

inline const hkaiSpuDirectedGraphAccessor::Node& hkaiSpuDirectedGraphAccessor::getNode(NodeIndex nid) const
{
	extern hkSpu4WayCache* g_hkaiGraphCache;
	return *hkGetArrayElemWithMaxSizeUsingCache(m_originalNodes.val(), m_numOriginalNodes, nid, g_hkaiGraphCache, HK_SPU_PATHFINDING_GRAPH_CACHE_LINE_SIZE);
}

inline void hkaiSpuDirectedGraphAccessor::getLocalPosition(int pid, hkVector4& posOut) const
{
	extern hkSpu4WayCache* g_hkaiGraphCache;
	posOut = *hkGetArrayElemWithMaxSizeUsingCache(m_originalPositions.val(), m_numOriginalNodes, pid, g_hkaiGraphCache, HK_SPU_PATHFINDING_GRAPH_CACHE_LINE_SIZE);
}

inline void hkaiSpuDirectedGraphAccessor::getInstancedNode( NodeIndex n, PaddedNode& nodeOut ) const
{
	extern hkSpu4WayCache* g_hkaiGraphCache;

	int mappedIdx = -1;
	if ( m_nodeMap )
	{
		mappedIdx = hkSpuDmaUtils::getInt32FromMainMemory(m_nodeMap.val() + n);
	}

	// Not instanced
	if (mappedIdx == -1) 
	{
		nodeOut.m_startEdgeIndex = -1;
		nodeOut.m_numEdges = 0;
	}
	else
	{
		const hkaiDirectedGraphExplicitCost::Node& instancedNode = *hkGetArrayElemWithMaxSizeUsingCache(m_instancedNodes.val(), m_numInstancedNodes, mappedIdx, g_hkaiGraphCache, HK_SPU_PATHFINDING_GRAPH_CACHE_LINE_SIZE);
		nodeOut = instancedNode;
	}
}

inline const hkaiSpuDirectedGraphAccessor::Edge* hkaiSpuDirectedGraphAccessor::getOwnedEdgeBegin() const
{
	return m_ownedEdges; 
}

#endif
///////////////////////////////////////////////////////////////////////////////

// These are identical for the CPU and SPU implementations.

inline hkaiRuntimeIndex hkaiDirectedGraphAccessor::getRuntimeId() const
{
	return m_runtimeId;
}

inline int hkaiDirectedGraphAccessor::getNumOriginalEdges() const
{
	return m_numOriginalEdges;
}

inline const hkaiDirectedGraphExplicitCost::Edge* hkaiDirectedGraphAccessor::getEdgeBegin() const
{
	return m_originalEdges;
}

inline void hkaiDirectedGraphAccessor::getPosition( NodeIndex pid, hkVector4& posOut ) const
{
	hkVector4 localPos;
	getLocalPosition(pid, localPos);

	hkVector4 posTrans; posTrans._setTransformedPos(getTransform(), localPos);
	posOut = posTrans;
}

inline hkaiPackedKey hkaiDirectedGraphAccessor::getOppositeNodeKeyForEdge( const hkaiDirectedGraphExplicitCost::Edge& edge ) const
{
	return hkaiGetOppositePackedKey( (hkUint8) edge.m_flags.get(), getRuntimeId(), edge.getOppositeNodeKeyUnchecked() );
}

inline const hkTransform& hkaiDirectedGraphAccessor::getTransform() const
{
	return m_transform;
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
