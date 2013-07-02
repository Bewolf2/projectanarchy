/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


inline void hkaiPointCloudSilhouetteGenerator::setTransform(const hkQTransform& t)
{
	m_transform = t;
}

void hkaiPointCloudSilhouetteGenerator::setEnabled(bool enabled)
{
	m_isEnabled = enabled;
}

bool hkaiPointCloudSilhouetteGenerator::getEnabled() const
{
	return m_isEnabled;
}

inline const hkQTransform& hkaiPointCloudSilhouetteGenerator::getTransform() const
{
	return m_transform;
}

inline void hkaiPointCloudSilhouetteGenerator::setWeldTolerance(hkReal weldTolerance)
{
	m_weldTolerance = weldTolerance;
}

inline hkReal hkaiPointCloudSilhouetteGenerator::getWeldTolerance() const
{
	return m_weldTolerance;
}

inline void hkaiPointCloudSilhouetteGenerator::setLocalPoints(const hkArrayBase<hkVector4>& points)
{
	m_localPointsChanged = true;
	m_localPoints = points;

	// Keep the size array empty, since we're using all the points.
	m_silhouetteSizes.clear();

	hkAabbUtil::calcAabb( m_localPoints.begin(), m_localPoints.getSize(), m_localAabb );
}

inline void hkaiPointCloudSilhouetteGenerator::setLocalPointDataUserFree(const hkVector4* pointData, int numPoints)
{
	// Have to const_cast here, but we never modify the local points
	m_localPoints.setDataUserFree( const_cast<hkVector4*>(pointData), numPoints, numPoints );
	m_localPointsChanged = true;
	m_silhouetteSizes.setSize(1);
	m_silhouetteSizes[0] = m_localPoints.getSize();
	hkAabbUtil::calcAabb( m_localPoints.begin(), m_localPoints.getSize(), m_localAabb );
}

inline const hkArray<hkVector4>& hkaiPointCloudSilhouetteGenerator::getLocalPoints() const
{
	return m_localPoints;
}

inline hkaiPointCloudSilhouetteGenerator::DetailLevel hkaiPointCloudSilhouetteGenerator::getDetailLevel() const
{
	return m_silhouetteDetailLevel;
}

inline void hkaiPointCloudSilhouetteGenerator::setDetailLevel( DetailLevel d, bool setDirty )
{
	m_silhouetteDetailLevel = d;
	if (setDirty)
	{
		m_localPointsChanged = true;
	}
}

inline int hkaiPointCloudSilhouetteGenerator::getNumChildSilhouettes() const
{
	bool useConvexHull = m_silhouetteSizes.isEmpty() || (m_silhouetteDetailLevel == DETAIL_CONVEX_HULL);
	return useConvexHull ? 1 : m_silhouetteSizes.getSize();
}

inline int hkaiPointCloudSilhouetteGenerator::getSizeOfChildSilhouette( int i ) const
{
	bool useConvexHull = m_silhouetteSizes.isEmpty() || (m_silhouetteDetailLevel == DETAIL_CONVEX_HULL);
	return useConvexHull ? m_localPoints.getSize() : m_silhouetteSizes[i];
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
