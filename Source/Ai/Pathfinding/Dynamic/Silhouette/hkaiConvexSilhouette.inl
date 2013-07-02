/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


inline hkaiConvexSilhouetteSet::hkaiConvexSilhouetteSet()
{

}

inline int hkaiConvexSilhouetteSet::getNumSilhouettes() const
{
	return m_silhouetteOffsets.getSize();
}

inline const hkVector4* hkaiConvexSilhouetteSet::getSilhouetteVertexPointer( int i ) const
{
	return m_vertexPool.begin() + m_silhouetteOffsets[i];
}


inline int hkaiConvexSilhouetteSet::getSilhouetteSize( int i ) const
{
	int nextOffset = (i==m_silhouetteOffsets.getSize() - 1) ? m_vertexPool.getSize() : m_silhouetteOffsets[i+1];
	return nextOffset - m_silhouetteOffsets[i];	
}

inline const hkArrayBase< hkVector4 >& hkaiConvexSilhouetteSet::getVertexPool() const
{
	return m_vertexPool;
}

inline void hkaiConvexSilhouetteSet::clear()
{
	m_silhouetteOffsets.clear();
	m_vertexPool.clear();
}

inline void hkaiConvexSilhouetteSet::set( const hkArrayBase< hkVector4 >& vertexPool, const hkArrayBase<int>& silhouetteSizes, const hkQTransform& lastLocalTransform, hkVector4Parameter up  )
{
	// Copy the vertex data
	// To avoid extra copying, we assume the size of the pool is set to to the sum of the sizes.
	// This is checked below.
	m_vertexPool = vertexPool;

	// Set the per-silhouette references into the pool
	const int numSilhouettes = silhouetteSizes.getSize();
	m_silhouetteOffsets.setSize( numSilhouettes );
	int vertexPoolOffset = 0;
	for(int i = 0; i < numSilhouettes; i++)
	{
		const int silhouetteSize = silhouetteSizes[i];
		HK_ASSERT2(0x716e26fe, vertexPoolOffset < m_vertexPool.getSize(), "Insufficient vertex pool vertices for silhouettes");
		m_silhouetteOffsets[i] = vertexPoolOffset;
		vertexPoolOffset += silhouetteSize;
	}

	HK_ASSERT(0x3fdebadc, m_vertexPool.getSize() == vertexPoolOffset);

	m_cachedTransform = lastLocalTransform;
	m_cachedUp = up;
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
