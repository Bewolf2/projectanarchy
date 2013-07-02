/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


//
// hkaiNavMesh::Face methods
//
inline void hkaiNavMesh::Face::init()
{
	m_startEdgeIndex = INVALID_EDGE_INDEX;
	m_numEdges = 0;
	m_startUserEdgeIndex = INVALID_EDGE_INDEX;
	m_numUserEdges = 0;
	m_clusterIndex = -1;
	m_padding = 0xcdcd;
}

//
// hkaiNavMesh::Edge methods
//
inline void hkaiNavMesh::Edge::setInvalid()
{
	m_oppositeFace = HKAI_INVALID_PACKED_KEY;
	m_oppositeEdge = HKAI_INVALID_PACKED_KEY;
	m_flags = EDGE_ORIGINAL;
	m_paddingByte = 0;
	m_userEdgeCost.setZero();
}

inline void hkaiNavMesh::Edge::setAsBoundary()
{
	m_oppositeFace = HKAI_INVALID_PACKED_KEY;
	m_oppositeEdge = HKAI_INVALID_PACKED_KEY;
}

inline void hkaiNavMesh::Edge::init(VertexIndex va, VertexIndex vb, hkaiPackedKey oppEdgeKey/* =HKAI_INVALID_PACKED_KEY */, hkaiPackedKey oppFaceKey/* =HKAI_INVALID_PACKED_KEY */)
{
	HK_ASSERT2(0x6ae4d2c1, (oppEdgeKey == HKAI_INVALID_PACKED_KEY) == (oppFaceKey == HKAI_INVALID_PACKED_KEY), "Opposite edge and face must both be valid, or both be invalid" );

	m_a = va;
	m_b = vb;
	m_oppositeEdge = oppEdgeKey;
	m_oppositeFace = oppFaceKey;
	m_userEdgeCost.setZero();
	m_flags = EDGE_ORIGINAL;
	m_paddingByte = 0;
}

inline hkaiRuntimeIndex hkaiNavMesh::Edge::getOppositeSectionRuntimeIndexUnchecked() const
{
	HK_ASSERT(0x33d26c97, hkaiGetRuntimeIdFromPacked(m_oppositeFace) == hkaiGetRuntimeIdFromPacked(m_oppositeEdge));
	return hkaiGetRuntimeIdFromPacked(m_oppositeEdge);
}

inline hkaiPackedKey hkaiNavMesh::Edge::getOppositeFaceKeyUnchecked() const
{
	return m_oppositeFace;
}

inline hkaiPackedKey hkaiNavMesh::Edge::getOppositeEdgeKeyUnchecked() const
{
	return m_oppositeEdge;
}

inline hkaiNavMesh::EdgeIndex hkaiNavMesh::Edge::getOppositeEdgeIndex() const
{
	return hkaiGetIndexFromPacked(m_oppositeEdge);
}

inline hkaiNavMesh::FaceIndex hkaiNavMesh::Edge::getOppositeFaceIndex() const
{
	return hkaiGetIndexFromPacked(m_oppositeFace);
}

inline hkBool32 hkaiNavMesh::Edge::isTraversable() const
{
	// either both edge and face are INVALID, or neither is
	HK_ASSERT(0x6ae4d2c3, (m_oppositeEdge == HKAI_INVALID_PACKED_KEY) == (m_oppositeFace == HKAI_INVALID_PACKED_KEY) );
	return !( (m_oppositeEdge == HKAI_INVALID_PACKED_KEY) || m_flags.anyIsSet( EDGE_BLOCKED ) );
}

inline hkBool32 hkaiNavMesh::Edge::hasOpposite() const
{
	// either both edge and face are INVALID, or neither is
	HK_ASSERT(0x6ae4d2c3, (m_oppositeEdge == HKAI_INVALID_PACKED_KEY) == (m_oppositeFace == HKAI_INVALID_PACKED_KEY) );
	return !(  m_oppositeEdge == HKAI_INVALID_PACKED_KEY   );
}

inline hkBool32 hkaiNavMesh::Edge::isUserEdge() const
{
	return m_flags.anyIsSet( EDGE_USER );
}

inline hkBool32 hkaiNavMesh::Edge::isExternal() const
{
	return m_flags.anyIsSet( EDGE_EXTERNAL_OPPOSITE );
}

inline hkSimdReal hkaiNavMesh::Edge::getUserEdgeCost() const
{
	if (isUserEdge())
	{
		hkSimdReal cost; cost.setFromHalf(m_userEdgeCost);
		return cost;
	}
	else
	{
		return hkSimdReal_0;
	}
}

inline void hkaiNavMesh::Edge::setUserEdgeCost( hkReal cost )
{
	HK_ASSERT(0x2033a1cb, isUserEdge() );
	m_userEdgeCost.setReal<true>(cost);
}

//
// hkaiNavMesh iteration methods
//
inline int hkaiNavMesh::Face::edgesBegin() const
{
	int startIndex = (m_numUserEdges > 0) ? m_startUserEdgeIndex : m_startEdgeIndex;
	return startIndex;
}

inline int hkaiNavMesh::Face::edgesNext( int e ) const
{
	

	// Still in the middle of the user edges
	if (e >= m_startUserEdgeIndex && e < endUserEdgeIndex() - 1)
	{
		return e + 1;
	}
	// At the end of the user edges - switch to normal edges
	else if (e == endUserEdgeIndex() - 1)
	{
		return m_startEdgeIndex;
	}
	else if (e >= m_startEdgeIndex && e < endEdgeIndex() - 1)
	{
		return e + 1;
	}
	else
	{
		HK_ASSERT(0x6b941507, e == endEdgeIndex() - 1);
		return -1;
	}
}

inline hkBool32 hkaiNavMesh::Face::edgesHasNext( int e ) const
{
	return e != -1;
}

//
// hkaiNavMesh methods
//

inline int hkaiNavMesh::getNumEdges() const
{
	return m_edges.getSize();
}

inline int hkaiNavMesh::getNumFaces() const
{
	return m_faces.getSize();
}

inline int hkaiNavMesh::getNumVertices() const
{
	return m_vertices.getSize();
}

//
// const accessors for arrays
// 
inline const hkArray<hkaiNavMesh::Face>& hkaiNavMesh::getFaceArray() const
{
	return m_faces;
}

inline const hkArray<hkaiNavMesh::Edge>& hkaiNavMesh::getEdgeArray() const
{
	return m_edges;
}

inline const hkArray<hkVector4>& hkaiNavMesh::getVertexArray() const
{
	return m_vertices;
}

//
// const and non-const access to array pointers
//

inline const hkaiNavMesh::Face* hkaiNavMesh::getFacePtr() const
{
	return m_faces.begin();
}

inline hkaiNavMesh::Face* hkaiNavMesh::getFacePtr()
{
	return m_faces.begin();
}

inline const hkaiNavMesh::Edge* hkaiNavMesh::getEdgePtr() const
{
	return m_edges.begin();
}

inline hkaiNavMesh::Edge* hkaiNavMesh::getEdgePtr()
{
	return m_edges.begin();
}

inline const hkVector4* hkaiNavMesh::getVertexPtr() const
{
	return m_vertices.begin();
}

inline hkVector4* hkaiNavMesh::getVertexPtr()
{
	return m_vertices.begin();
}

#ifndef HK_PLATFORM_SPU

template<typename DATA>
HK_FORCE_INLINE void hkaiNavMesh::copyUserData( DATA* dst, const DATA* src, int striding )
{
	// Special-case the 0 and 1
	if (striding == 1)
	{
		*dst = *src;
	}
	else if (striding == 0)
	{
		return;
	}
	else
	{
		for (int i=0; i<striding; i++)
		{
			dst[i] = src[i];
		}
	}
}

template< typename DATA >
HK_FORCE_INLINE void hkaiNavMesh::copyUserData( hkArrayBase<DATA>& dataArray, int dstIndex, int srcIndex, int striding)
{
	copyUserData( dataArray.begin() + dstIndex*striding, dataArray.begin() + srcIndex*striding, striding);
}

inline const hkaiNavMesh::Face& hkaiNavMesh::getFace(hkaiNavMesh::FaceIndex fIdx) const
{
	return m_faces[fIdx]; 
}

inline hkaiNavMesh::Face& hkaiNavMesh::getFace(hkaiNavMesh::FaceIndex fIdx)
{
	return m_faces[fIdx]; 
}

inline const hkaiNavMesh::Edge& hkaiNavMesh::getEdge(hkaiNavMesh::EdgeIndex eIdx) const 
{ 
	return m_edges[eIdx];
}

inline hkaiNavMesh::Edge& hkaiNavMesh::getEdge(hkaiNavMesh::EdgeIndex eIdx)
{ 
	return m_edges[eIdx];
}

inline const hkVector4& hkaiNavMesh::getVertex(hkaiNavMesh::VertexIndex vIdx) const 
{ 
	return m_vertices[vIdx];
}

inline hkVector4& hkaiNavMesh::getVertex(hkaiNavMesh::VertexIndex vIdx)
{ 
	return m_vertices[vIdx];
}

inline void hkaiNavMesh::getVertexPosition(hkaiNavMesh::VertexIndex v, hkVector4& vOut) const
{ 
	vOut = m_vertices[v]; 
}

inline void hkaiNavMesh::getLocalVertexPosition(hkaiNavMesh::VertexIndex v, hkVector4& vOut) const
{
	// Same as getVertexPosition(), only exists to template code with hkaiNavMeshInstance
	getVertexPosition(v, vOut);
}

inline void hkaiNavMesh::getEdgePoints( int edgeIndex, hkVector4& ea, hkVector4& eb) const
{
	const Edge& e = getEdge(edgeIndex);
	getVertexPosition(e.m_a, ea);
	getVertexPosition(e.m_b, eb);
}

inline void hkaiNavMesh::getEdgePoints( const hkaiNavMesh::Edge& e, hkVector4& ea, hkVector4& eb) const
{
	getVertexPosition(e.m_a, ea);
	getVertexPosition(e.m_b, eb);
}

inline void hkaiNavMesh::getEdgeLocalPoints( int edgeIndex, hkVector4& ea, hkVector4& eb) const
{
	getEdgePoints(edgeIndex, ea, eb);
}

inline void hkaiNavMesh::getEdgeLocalPoints( const hkaiNavMesh::Edge& e, hkVector4& ea, hkVector4& eb) const
{
	getEdgePoints(e, ea, eb);
}

inline hkaiNavMesh::EdgeData hkaiNavMesh::getEdgeData( EdgeIndex eIdx ) const
{
	EdgeData arrayVal = m_edgeDataStriding ? m_edgeData[ eIdx * m_edgeDataStriding ] : 0;
	return arrayVal;
}

inline hkaiNavMesh::FaceData hkaiNavMesh::getFaceData( FaceIndex fIdx ) const
{
	FaceData arrayVal = m_faceDataStriding ? m_faceData[ fIdx * m_faceDataStriding ] : 0;
	return arrayVal;
}

inline void hkaiNavMesh::setEdgeData( EdgeIndex eIdx, EdgeData d )
{
	if (m_edgeDataStriding)
	{
		m_edgeData[ eIdx * m_edgeDataStriding ] = d;
	}
}

inline void hkaiNavMesh::setFaceData( FaceIndex fIdx, FaceData d )
{
	if (m_faceDataStriding)
	{
		m_faceData[ fIdx * m_faceDataStriding ] = d;
	}
}

inline const hkaiNavMesh::EdgeData* hkaiNavMesh::getEdgeDataPtr( EdgeIndex eIdx ) const
{
	return m_edgeDataStriding ? m_edgeData.begin()  + eIdx*m_edgeDataStriding : HK_NULL;
}

inline       hkaiNavMesh::EdgeData* hkaiNavMesh::getEdgeDataPtr( EdgeIndex eIdx )
{
	return m_edgeDataStriding ? m_edgeData.begin()  + eIdx*m_edgeDataStriding : HK_NULL;
}

inline const hkaiNavMesh::FaceData* hkaiNavMesh::getFaceDataPtr( FaceIndex fIdx ) const
{
	return m_faceDataStriding ? m_faceData.begin()  + fIdx*m_faceDataStriding : HK_NULL;
}

inline       hkaiNavMesh::FaceData* hkaiNavMesh::getFaceDataPtr( FaceIndex fIdx )
{
	return m_faceDataStriding ? m_faceData.begin()  + fIdx*m_faceDataStriding : HK_NULL;
}


#endif		

inline const hkAabb& hkaiNavMesh::getAabb() const
{
	return m_aabb;
}

inline hkReal hkaiNavMesh::getErosionRadius() const
{
	return m_erosionRadius;
}

inline hkaiNavMesh::NavMeshFlags hkaiNavMesh::getFlags() const
{
	return m_flags;
}

inline hkaiNavMesh::NavMeshFlags& hkaiNavMesh::getFlags()
{
	return m_flags;
}

inline bool hkaiNavMesh::isWallClimbing() const
{
	return m_flags.anyIsSet( hkaiNavMesh::MESH_CLIMBING ) != 0;
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
