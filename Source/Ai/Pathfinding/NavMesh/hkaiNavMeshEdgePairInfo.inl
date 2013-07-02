/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


hkaiNavMeshEdgePairInfo::EdgeInfo::EdgeInfo()
:	m_edgeKey(HKAI_INVALID_PACKED_KEY),
	m_edge(HK_NULL)
{
}

hkaiNavMeshEdgePairInfo::hkaiNavMeshEdgePairInfo()
:	m_faceKey(HKAI_INVALID_PACKED_KEY),
	m_face(HK_NULL)
{

}

#if defined(HK_DEBUG)
inline bool hkaiNavMeshEdgePairInfo::EdgeInfo::edgeInfoEqual(const hkaiNavMesh::Edge& e0, const hkaiNavMesh::Edge& e1)
{
	return (e0.m_a == e1.m_a)
		&&	(e0.m_b == e1.m_b) 
		&&	( (!e0.hasOpposite() && !e1.hasOpposite()) || (e0.getOppositeEdgeIndex() == e1.getOppositeEdgeIndex())) 
		&&	( (!e0.hasOpposite() && !e1.hasOpposite()) || (e0.getOppositeFaceIndex() == e1.getOppositeFaceIndex()))
		&&	(e0.m_flags == e1.m_flags) 
		&&	(e0.m_userEdgeCost == e1.m_userEdgeCost) 
		/*&&	(e0.m_data == e1.m_data)*/ ;
}
#endif

void hkaiNavMeshEdgePairInfo::EdgeInfo::validate( const hkaiNavMeshAccessor* mesh ) const
{
	HK_ASSERT(0x630f01f0, m_edgeKey == HKAI_INVALID_PACKED_KEY || ( m_edge ) );

#ifndef HK_PLATFORM_SPU
	if(mesh != HK_NULL)
	{
		// Now check against data actually in the mesh
		if( m_edgeKey != HKAI_INVALID_PACKED_KEY )
		{
			HK_ON_DEBUG( const hkaiNavMesh::Edge& e0 = mesh->getEdge( hkaiGetIndexFromPacked(m_edgeKey) ) );
			HK_ON_DEBUG( const hkaiNavMesh::Edge& e1 = *m_edge );
			HK_ASSERT(0x29c46efe, edgeInfoEqual(e0, e1) );
		}
		else
		{
			HK_ASSERT(0x29c46efe, m_edge == HK_NULL );
		}
	}
#endif
}

void hkaiNavMeshEdgePairInfo::validate( const hkaiNavMeshAccessor* mesh ) const
{
	HK_ASSERT(0x56f31f14, m_faceKey != HKAI_INVALID_PACKED_KEY);
	HK_ASSERT(0x15a1f426, m_face);
	// Check that the edge pointers and edge indices are consistent
	m_incomingEdgeInfo.validate(HK_NULL);	// Can't validate input against mesh data as accessor is the outgoing one.
	m_outgoingEdgeInfo.validate(mesh);

	// Make sure the edges aren't the same
	HK_ASSERT(0x2ad0e697, (m_incomingEdgeInfo.getEdgeKey() != m_outgoingEdgeInfo.getEdgeKey()) || m_incomingEdgeInfo.getEdgeKey() == HKAI_INVALID_PACKED_KEY);
	HK_ASSERT(0x6ec88eb1, (m_incomingEdgeInfo.getEdge() != m_outgoingEdgeInfo.getEdge()) || m_incomingEdgeInfo.getEdge() == HK_NULL);

	// Face and outgoing edge must be in same section
	HK_ON_DEBUG(const EdgeInfo& outgoing = m_outgoingEdgeInfo;)
		HK_ASSERT(0x4cc294c2, (outgoing.getEdgeKey() == HKAI_INVALID_PACKED_KEY) || 
		hkaiGetRuntimeIdFromPacked(m_faceKey) == hkaiGetRuntimeIdFromPacked(outgoing.getEdgeKey()));

	// Now check against data actually in the mesh
#if defined(HK_DEBUG) && !defined(HK_PLATFORM_SPU)
	if(mesh != HK_NULL)
	{
		// Notes that we can't check for equality on the pointers, since some point to cached values.
		// Have to check a relevant field instead.
		// Check that the face pointers and section indices are consistent
		HK_ASSERT(0x72b81342, mesh->getFace( hkaiGetIndexFromPacked(m_faceKey) ).m_startEdgeIndex == m_face->m_startEdgeIndex);
		HK_ASSERT(0x3a229f20, hkaiGetRuntimeIdFromPacked(m_faceKey) == mesh->getRuntimeId() );	

		if ( !outgoing.getEdge() )
		{
			HK_ASSERT(0x54704a99, outgoing.getEdgeKey() == HKAI_INVALID_PACKED_KEY);
		}
		else
		{
			HK_ASSERT(0x54704a99, outgoing.getEdgeKey() != HKAI_INVALID_PACKED_KEY);
			HK_ON_DEBUG(const hkaiNavMesh::Edge& tempEdge = mesh->getEdge(hkaiGetIndexFromPacked(outgoing.getEdgeKey())));
			HK_ASSERT(0x688c6385,  (tempEdge.m_oppositeEdge == HKAI_INVALID_PACKED_KEY && outgoing.getEdge()->m_oppositeEdge == HKAI_INVALID_PACKED_KEY) || 
				tempEdge.getOppositeEdgeIndex() == outgoing.getEdge()->getOppositeEdgeIndex() );
		}
	}
#endif
}


inline void hkaiNavMeshEdgePairInfo::setFace( hkaiPackedKey faceKey, const hkaiNavMesh::Face* face, const hkaiNavMesh::FaceData* faceData )
{
	m_faceKey = faceKey;
	m_face = face;
	m_faceData = faceData;
}

inline void hkaiNavMeshEdgePairInfo::setStartPoint( hkVector4Parameter pos )
{
	m_incomingEdgeInfo.setEdge( HKAI_INVALID_PACKED_KEY, HK_NULL, HK_NULL, pos );
}

inline void hkaiNavMeshEdgePairInfo::setEndPoint( hkVector4Parameter pos )
{
	m_outgoingEdgeInfo.setEdge( HKAI_INVALID_PACKED_KEY, HK_NULL, HK_NULL, pos );
}

inline void hkaiNavMeshEdgePairInfo::EdgeInfo::setEdge(hkaiPackedKey edgeKey, const hkaiNavMesh::Edge* edge, const hkaiNavMesh::EdgeData* edgeData, hkVector4Parameter pos)
{
	m_edgeKey = edgeKey;
	m_edge = edge;
	m_edgeData = edgeData;
	m_position = pos;
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
