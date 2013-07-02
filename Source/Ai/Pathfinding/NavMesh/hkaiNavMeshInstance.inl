/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiNavMeshInstance::AddFaceContext::AddFaceContext()
{
	for (int i=0; i<hkaiNavMesh::MAX_DATA_PER_FACE; i++)
		m_data[i] = 0;
}


inline hkaiNavMeshInstance::AddEdgeContext::AddEdgeContext()
: m_cutInfo(hkaiNavMeshInstance::NOT_CUT_EDGE)
{
	for (int i=0; i<hkaiNavMesh::MAX_DATA_PER_EDGE; i++)
		m_data[i] = 0;
}


// Templated utility function to look up an edge or face from the given index.
template<typename T>
static const T* hkaiNavMeshInstance_get( const T* originalArray, int numOriginal, const hkArray<T>& instancedArray,
										 const hkArray<T>& ownedArray, const hkArray<int>& instanceMap, int index )
{
	if (index >= numOriginal)
	{
		// Looking for a struct edge created by cutting.
		return &ownedArray[ index - numOriginal ];
	}

	// Do a map lookup. If the map is empty, always use the instanced struct
	int mappedIdx = instanceMap.getSize() ? instanceMap[index] : index;

	if ( mappedIdx == -1 )
	{
		// The struct isn't instanced, so return the original struct
		return &originalArray[ index ];
	}
	else
	{
		// There is an instance, so return that instead.
		return &instancedArray[ mappedIdx ];
	}
}


// Templated utility function to look up edge data or face data from the given index.
template<typename T>
static const T* hkaiNavMeshInstance_getWithStriding( const T* originalArray, int numOriginal, const hkArray<T>& instancedArray,
										const hkArray<T>& ownedArray, const hkArray<int>& instanceMap, int index, 
										int striding )
{
	if (index >= numOriginal)
	{
		// Looking for a struct edge created by cutting.
		return &ownedArray[ striding * (index - numOriginal) ];
	}

	// Do a map lookup. If the map is empty, always use the instanced struct
	int mappedIdx = instanceMap.getSize() ? instanceMap[index] : index;

	if ( mappedIdx == -1 )
	{
		// The struct isn't instanced, so return the original struct
		return &originalArray[ striding * index ];
	}
	else
	{
		// There is an instance, so return that instead.
		return &instancedArray[ striding * mappedIdx ];
	}
}

template<typename T, typename DATA>
static const T& hkaiNavMeshInstance_getWithData( 
	const T* originalArray, int numOriginal, const hkArray<T>& instancedArray, const hkArray<T>& ownedArray, 
	const DATA* originalData, const hkArray<DATA>& instancedData, const hkArray<DATA>& ownedData,
	const hkArray<int>& instanceMap, int index, int striding, const DATA*& dataPtrOut )
{
	if (index >= numOriginal)
	{
		// Looking for a struct edge created by cutting.
		int ownedIndex = index - numOriginal;
		dataPtrOut = ownedData.begin() + ownedIndex*striding;

		return ownedArray[ ownedIndex ];
	}

	// Do a map lookup. If the map is empty, always use the instanced struct
	int mappedIdx = instanceMap.getSize() ? instanceMap[index] : index;

	if ( mappedIdx == -1 )
	{
		// The struct isn't instanced, so return the original struct
		dataPtrOut = originalData + (index*striding);
		return originalArray[ index ];
	}
	else
	{
		// There is an instance, so return that instead.
		dataPtrOut = instancedData.begin() + mappedIdx*striding;
		return instancedArray[ mappedIdx ];
	}
}

// Templated utility function to create a new instance of an edge or face it it doesn't already exist.
template<typename T, typename DATA>
static T& hkaiNavMeshInstance_instance( const T* originalArray, hkArray<T>& instancedArray, const DATA* originalData, hkArray<DATA>& instancedData, hkArray<int>& instanceMap, int index, int dataStriding)
{
	HK_ASSERT(0x7b5c4caa, instanceMap.getSize() );
	int mapIdx = instanceMap[index];
	if ( mapIdx == -1)
	{
		int newIdx = instancedArray.getSize();
		T& newT = instancedArray.expandOne();
		newT = originalArray[index];
		if (dataStriding)
		{
			DATA* newData = instancedData.expandBy(dataStriding);
			const DATA* dataSrc = originalData + dataStriding*index;
			hkMemUtil::memCpy< HK_ALIGN_OF(DATA) >( newData, dataSrc, dataStriding*sizeof(DATA) );
		}
		instanceMap[index] = newIdx;
		return newT;
	}
	else 
	{
		return instancedArray[mapIdx];
	}
}

template<typename T, typename DATA>
inline T* hkaiNavMeshInstance_getWritable(const T* originalArray, int numOriginal, hkArray<T>& instancedArray, 
	hkArray<T>& ownedArray, const DATA* originalData, hkArray<DATA>& instancedData, hkArray<int>& instanceMap,
	int index, int dataStriding, hkBool32 instanceIfNotAlready) 
{
	if (index >= numOriginal)
	{
		// new struct
		return &ownedArray[ index - numOriginal ];
	}

	// Do a map lookup. If the map is empty, always use the instanced struct
	int mappedIdx = instanceMap.getSize() ? instanceMap[index] : index;

	HK_ASSERT(0x6779775f, instanceIfNotAlready || mappedIdx != -1);

	if ( HK_VERY_UNLIKELY(!instanceIfNotAlready && mappedIdx == -1) )
	{
		// fail fast - never allow write-access to the original struct.
		return HK_NULL;
	}

	if (mappedIdx == -1)
	{
		return &hkaiNavMeshInstance_instance(originalArray, instancedArray, originalData, instancedData, instanceMap, index, dataStriding );
	}
	else
	{
		// old, modified edge
		return &instancedArray[ mappedIdx ];
	}
}

#ifndef HK_PLATFORM_SPU
template<typename DATA>
inline DATA HK_CALL hkaiNavMeshInstance::getUserDataFromMainMemory(const DATA* dataPtr)
{
	return *dataPtr;
}
#endif

inline const hkaiNavMesh::Edge& hkaiNavMeshInstance::getEdge( hkaiNavMesh::EdgeIndex edgeIndex ) const
{
	return *hkaiNavMeshInstance_get( m_originalEdges, m_numOriginalEdges, m_instancedEdges, m_ownedEdges, m_edgeMap, edgeIndex );
}


inline const hkaiNavMesh::Face& hkaiNavMeshInstance::getFace( hkaiNavMesh::FaceIndex fIdx ) const
{
	return *hkaiNavMeshInstance_get( m_originalFaces, m_numOriginalFaces, m_instancedFaces, m_ownedFaces, m_faceMap, fIdx);
}


const hkaiNavMesh::Face& hkaiNavMeshInstance::getFaceAndDataPtr( hkaiNavMesh::FaceIndex fIdx, const hkaiNavMesh::FaceData*& faceDataOut ) const
{
	return hkaiNavMeshInstance_getWithData( m_originalFaces, m_numOriginalFaces, m_instancedFaces, m_ownedFaces, 
		m_originalFaceData, m_instancedFaceData, m_ownedFaceData,
		m_faceMap, fIdx, m_faceDataStriding, faceDataOut);
}


const hkaiNavMesh::Edge& hkaiNavMeshInstance::getEdgeAndDataPtr( hkaiNavMesh::EdgeIndex eIdx, const hkaiNavMesh::EdgeData*& edgeDataOut ) const
{
	return hkaiNavMeshInstance_getWithData( m_originalEdges, m_numOriginalEdges, m_instancedEdges, m_ownedEdges, 
		m_originalEdgeData, m_instancedEdgeData, m_ownedEdgeData,
		m_edgeMap, eIdx, m_edgeDataStriding, edgeDataOut );

}

inline void hkaiNavMeshInstance::getFaceAndEdgePointers( hkaiNavMesh::FaceIndex faceIndex, hkaiNavMesh::EdgeIndex edgeIndex, 
														const hkaiNavMesh::Face*& facePtr, const hkaiNavMesh::Edge*& edgePtr, const hkaiNavMesh::FaceData*& faceDataOut ) const
{
	facePtr = &getFaceAndDataPtr(faceIndex, faceDataOut);
	edgePtr = &getEdge(edgeIndex);
}

inline hkUlong hkaiNavMeshInstance::getDisplayIdForOriginalFace( hkaiNavMesh::FaceIndex faceIndex ) const
{
	HK_ASSERT(0x5fc1a672, faceIndex >= 0 && faceIndex < m_numOriginalFaces);
	return hkUlong(m_faceMap.begin() + faceIndex);
}

inline hkUlong hkaiNavMeshInstance::getDisplayIdForOwnedFace( hkaiNavMesh::FaceIndex faceIndex ) const
{
	return hkUlong( &getOwnedFace(faceIndex) );
}


inline hkaiNavMesh::Edge* hkaiNavMeshInstance::getWritableEdge( hkaiNavMesh::EdgeIndex eIdx, hkBool32 instanceIfNotAlready /*= false*/) 
{
	return hkaiNavMeshInstance_getWritable(m_originalEdges, m_numOriginalEdges, m_instancedEdges, m_ownedEdges, 
		m_originalEdgeData, m_instancedEdgeData, m_edgeMap, eIdx, getEdgeDataStriding(), instanceIfNotAlready );
}

inline hkaiNavMesh::Face* hkaiNavMeshInstance::getWritableFace( hkaiNavMesh::FaceIndex fIdx, hkBool32 instanceIfNotAlready /*= false*/ )
{
	return hkaiNavMeshInstance_getWritable(m_originalFaces, m_numOriginalFaces, m_instancedFaces, m_ownedFaces, 
		m_originalFaceData, m_instancedFaceData, m_faceMap, fIdx, getFaceDataStriding(), instanceIfNotAlready );
}

inline hkaiNavMesh::EdgeData hkaiNavMeshInstance::getEdgeData( hkaiNavMesh::EdgeIndex eIdx ) const
{
	hkaiNavMesh::EdgeData arrayVal = getEdgeDataStriding() ? 
		*hkaiNavMeshInstance_getWithStriding( m_originalEdgeData, m_numOriginalEdges, m_instancedEdgeData, m_ownedEdgeData, m_edgeMap, eIdx, getEdgeDataStriding() )
		: 0;

	return arrayVal;
}

inline hkaiNavMesh::FaceData hkaiNavMeshInstance::getFaceData( hkaiNavMesh::FaceIndex fIdx ) const
{
	hkaiNavMesh::EdgeData arrayVal = getFaceDataStriding() ? 
		*hkaiNavMeshInstance_getWithStriding( m_originalFaceData, m_numOriginalFaces, m_instancedFaceData, m_ownedFaceData, m_faceMap, fIdx, getFaceDataStriding() )
		: 0;
	
	return arrayVal;
}

inline const hkaiNavMesh::EdgeData* hkaiNavMeshInstance::getEdgeDataPtr( hkaiNavMesh::EdgeIndex eIdx ) const
{
	return getEdgeDataStriding() ? 
		hkaiNavMeshInstance_getWithStriding( m_originalEdgeData, m_numOriginalEdges, m_instancedEdgeData, m_ownedEdgeData, m_edgeMap, eIdx, getEdgeDataStriding() )
		: HK_NULL;
}

inline const hkaiNavMesh::FaceData* hkaiNavMeshInstance::getFaceDataPtr( hkaiNavMesh::FaceIndex fIdx ) const
{
	return getFaceDataStriding() ? 
		hkaiNavMeshInstance_getWithStriding( m_originalFaceData, m_numOriginalFaces, m_instancedFaceData, m_ownedFaceData, m_faceMap, fIdx, getFaceDataStriding() )
		: HK_NULL;
}

inline hkaiNavMesh::EdgeData* hkaiNavMeshInstance::getWritableEdgeDataPtr( hkaiNavMesh::EdgeIndex eIdx, hkBool32 instanceIfNotAlready )
{
	getWritableEdge(eIdx, instanceIfNotAlready); // instance if needed
	return hkaiNavMeshInstance_getWritable(m_originalEdgeData, m_numOriginalEdges, m_instancedEdgeData, m_ownedEdgeData, 
		m_originalEdgeData, m_instancedEdgeData, m_edgeMap, eIdx, getEdgeDataStriding(), false );
}

inline hkaiNavMesh::FaceData* hkaiNavMeshInstance::getWritableFaceDataPtr( hkaiNavMesh::FaceIndex fIdx, hkBool32 instanceIfNotAlready )
{
	getWritableFace(fIdx, instanceIfNotAlready); // instance if needed
	return hkaiNavMeshInstance_getWritable( m_originalFaceData, m_numOriginalFaces, m_instancedFaceData, m_ownedFaceData, 
		m_originalFaceData, m_instancedFaceData, m_faceMap, fIdx, getFaceDataStriding(), false );
}

inline const hkaiReferenceFrame& hkaiNavMeshInstance::getReferenceFrame() const
{
	return m_referenceFrame;
}

inline hkaiReferenceFrame& hkaiNavMeshInstance::getReferenceFrame()
{
	return m_referenceFrame;
}

inline const hkTransform& hkaiNavMeshInstance::getTransform() const
{
	return m_referenceFrame.getTransform();
}

inline void hkaiNavMeshInstance::getLocalVertexPosition( hkaiNavMesh::VertexIndex vIdx, hkVector4& vOut ) const
{
	hkVector4 v = (vIdx < m_numOriginalVertices) ? m_originalVertices[vIdx] : m_ownedVertices[ vIdx - m_numOriginalVertices ];
	vOut = v;
}

inline void hkaiNavMeshInstance::getVertexPosition( hkaiNavMesh::VertexIndex vIdx, hkVector4& vOut ) const
{
	hkVector4 v = (vIdx < m_numOriginalVertices) ? m_originalVertices[vIdx] : m_ownedVertices[ vIdx - m_numOriginalVertices ];
	hkVector4 vTrans; vTrans._setTransformedPos(getTransform(), v);
	vOut = vTrans;
}

inline void hkaiNavMeshInstance::getEdgePoints( hkaiNavMesh::EdgeIndex eIdx, hkVector4& edgeA, hkVector4& edgeB ) const
{
	const hkaiNavMesh::Edge& edge = getEdge(eIdx);

	
	hkVector4 tempA, tempB;
	getVertexPosition( edge.m_a, tempA );
	getVertexPosition( edge.m_b, tempB );
	edgeA = tempA;
	edgeB = tempB;
}

inline void hkaiNavMeshInstance::getEdgePoints( const hkaiNavMesh::Edge& edge, hkVector4& edgeA, hkVector4& edgeB ) const
{
	hkVector4 tempA, tempB;
	getVertexPosition( edge.m_a, tempA );
	getVertexPosition( edge.m_b, tempB );
	edgeA = tempA;
	edgeB = tempB;
}

inline void hkaiNavMeshInstance::getEdgeLocalPoints( hkaiNavMesh::EdgeIndex eIdx, hkVector4& edgeA, hkVector4& edgeB ) const
{
	const hkaiNavMesh::Edge& edge = getEdge(eIdx);
	hkVector4 tempA, tempB;
	getLocalVertexPosition( edge.m_a, tempA );
	getLocalVertexPosition( edge.m_b, tempB );
	edgeA = tempA;
	edgeB = tempB;
}

inline void hkaiNavMeshInstance::getEdgeLocalPoints( const hkaiNavMesh::Edge& edge, hkVector4& edgeA, hkVector4& edgeB ) const
{
	hkVector4 tempA, tempB;
	getLocalVertexPosition( edge.m_a, tempA );
	getLocalVertexPosition( edge.m_b, tempB );
	edgeA = tempA;
	edgeB = tempB;
}

inline const hkaiNavMesh::Face& hkaiNavMeshInstance::getOriginalFace( hkaiNavMesh::FaceIndex fIdx ) const
{
	HK_ASSERT(0x1bb01670, fIdx < m_numOriginalFaces);
	return m_originalFaces[fIdx];
}

inline const hkaiNavMesh::Edge& hkaiNavMeshInstance::getOriginalEdge( hkaiNavMesh::EdgeIndex eIdx ) const
{
	HK_ASSERT(0x49137b4a, eIdx < m_numOriginalEdges);
	return m_originalEdges[eIdx];
}

inline const hkaiNavMesh::Face& hkaiNavMeshInstance::getOwnedFace( int fIdx ) const
{
	HK_ASSERT(0x2ab99959, fIdx >= m_numOriginalFaces);
	return m_ownedFaces[ fIdx - m_numOriginalFaces ];
}

inline hkaiNavMesh::Face& hkaiNavMeshInstance::getOwnedFace( int fIdx )
{
	HK_ASSERT(0x2ab99959, fIdx >= m_numOriginalFaces);
	return m_ownedFaces[ fIdx - m_numOriginalFaces ];
}

inline const hkaiNavMesh::Edge& hkaiNavMeshInstance::getOwnedEdge( int eIdx ) const
{
	HK_ASSERT(0x7fa15fb9, eIdx >= m_numOriginalEdges);
	return m_ownedEdges[eIdx - m_numOriginalEdges];
}

inline hkaiNavMesh::Edge& hkaiNavMeshInstance::getOwnedEdge( int eIdx )
{
	HK_ASSERT(0x7fa15fb9, eIdx >= m_numOriginalEdges);
	return m_ownedEdges[eIdx - m_numOriginalEdges];
}

inline void hkaiNavMeshInstance::setTransform( const hkTransform& transform )
{
	m_referenceFrame.setTransform(transform);
}

inline const hkaiNavMesh::FaceFlags hkaiNavMeshInstance::getFlagsForFace( hkaiNavMesh::FaceIndex f ) const
{
	return m_faceFlags.getSize() ? m_faceFlags[f] : 0;
}

inline hkBool32 hkaiNavMeshInstance::isFaceHidden( hkaiNavMesh::FaceIndex f ) const
{
	const hkaiNavMesh::FaceFlags flags = getFlagsForFace(f);
	return flags.anyIsSet(hkaiNavMesh::FACE_HIDDEN);
}

inline hkBool32 hkaiNavMeshInstance::isFaceCut( hkaiNavMesh::FaceIndex f ) const
{
	const hkaiNavMesh::FaceFlags flags = getFlagsForFace(f);
	return flags.anyIsSet(hkaiNavMesh::FACE_CUT);
}

inline hkBool32 hkaiNavMeshInstance::isFaceStreaming( hkaiNavMesh::FaceIndex f ) const
{
	const hkaiNavMesh::FaceFlags flags = getFlagsForFace(f);
	return flags.anyIsSet(hkaiNavMesh::FACE_STREAMING);
}

inline hkaiNavMeshInstance::CutInfo hkaiNavMeshInstance::getCutInfoForEdge( hkaiNavMesh::EdgeIndex e ) const
{
	return (e >= m_numOriginalEdges) ? m_cuttingInfo[ e - m_numOriginalEdges ] : (hkaiNavMeshInstance::CutInfo) NOT_CUT_EDGE;
}

inline void hkaiNavMeshInstance::setFlagsForFace( hkaiNavMesh::FaceIndex f, hkaiNavMesh::FaceFlags flags)
{
	m_faceFlags[f] = flags;
}

inline bool hkaiNavMeshInstance::hasClearanceInfo() const
{
	return (m_clearanceCache.getSize() > 0 || m_globalClearanceCache.getSize() > 0);
}

inline hkHalf hkaiNavMeshInstance::getClearance(int index) const
{
	return m_clearanceCache[index];
}

inline hkHalf hkaiNavMeshInstance::getGlobalClearance( hkaiNavMesh::VertexIndex v) const
{
	return m_globalClearanceCache[v];
}

inline hkReal hkaiNavMeshInstance::getErosionRadius() const
{
	return m_originalMesh->m_erosionRadius;
}

inline hkaiPackedKey hkaiNavMeshInstance::getOppositeFaceKeyForEdge( const hkaiNavMesh::Edge& edge ) const
{
	return hkaiGetOppositePackedKey(edge.m_flags.get(), getRuntimeId(), edge.m_oppositeFace );
}

inline hkaiPackedKey hkaiNavMeshInstance::getOppositeEdgeKeyForEdge( const hkaiNavMesh::Edge& edge ) const
{
	return hkaiGetOppositePackedKey(edge.m_flags.get(), getRuntimeId(), edge.m_oppositeEdge );
}

inline hkaiRuntimeIndex hkaiNavMeshInstance::getOppositeSectionRuntimeIndexForEdge( const hkaiNavMesh::Edge& edge ) const
{
	return hkaiGetOppositeRuntimeIndex( edge.m_flags.get(), getRuntimeId(), edge.m_oppositeEdge );
}

#ifndef HK_PLATFORM_SPU
inline void hkaiNavMeshInstance::copyOwnedEdge( int dstIndex, int srcIndex )
{
	m_ownedEdges[dstIndex] = m_ownedEdges[srcIndex];
	m_cuttingInfo[dstIndex] = m_cuttingInfo[srcIndex];

	hkaiNavMesh::copyUserData( m_ownedEdgeData, dstIndex, srcIndex, getEdgeDataStriding() );

}

inline void hkaiNavMeshInstance::copyOwnedFace( int dstIndex, int srcIndex )
{
	m_ownedFaces[dstIndex] = m_ownedFaces[srcIndex];
	m_faceFlags[dstIndex + m_numOriginalFaces] = m_faceFlags[srcIndex + m_numOriginalFaces];
	m_faceClearanceIndices[dstIndex + m_numOriginalFaces] = m_faceClearanceIndices[srcIndex + m_numOriginalFaces];
	hkaiNavMesh::copyUserData( m_ownedFaceData, dstIndex, srcIndex, getFaceDataStriding() );
}

inline void hkaiNavMeshInstance::copyInstancedEdge( int dstIndex, int srcIndex )
{
	m_instancedEdges[dstIndex] = m_instancedEdges[srcIndex];
	hkaiNavMesh::copyUserData( m_instancedEdgeData, dstIndex, srcIndex, getEdgeDataStriding() );
}


inline void hkaiNavMeshInstance::setNumOwnedEdges( int numOwnedEdges )
{
	m_ownedEdges.setSize( numOwnedEdges );
	m_cuttingInfo.setSize( numOwnedEdges );
	m_ownedEdgeData.setSize( getEdgeDataStriding() * numOwnedEdges );
}

inline void hkaiNavMeshInstance::setNumOwnedFaces( int numOwnedFaces )
{
	const int newNumTotalFaces = numOwnedFaces + getNumOriginalFaces();
	m_ownedFaces.setSize( numOwnedFaces );
	m_faceFlags.setSize(  newNumTotalFaces);
	m_faceClearanceIndices.setSize( newNumTotalFaces );
	m_ownedFaceData.setSize( getFaceDataStriding() * numOwnedFaces );
}

inline void hkaiNavMeshInstance::setNumInstancedEdges( int numInstancedEdges )
{
	m_instancedEdges.setSize( numInstancedEdges );
	m_instancedEdgeData.setSize( getEdgeDataStriding() * numInstancedEdges );
}
#endif

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
