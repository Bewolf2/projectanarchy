/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkBool32 hkaiVisibilityUtil::VisibleSet::Arc::overlaps( const Arc& other ) const
{
	return (m_min <= other.m_max && other.m_min <= m_max);
}

inline hkBool32 hkaiVisibilityUtil::VisibleSet::Arc::contains( const Arc& other ) const
{
	return m_min <= other.m_min && m_max >= other.m_max;
}

inline hkBool32 hkaiVisibilityUtil::VisibleSet::Arc::isBigEnough( hkReal minAngle ) const
{
	return (m_max - m_min) >= minAngle;
}


inline void hkaiVisibilityUtil::VisibleSet::init()
{
	m_arcs.clear();

	Arc fullCircle;
	fullCircle.m_min = -HK_REAL_PI;
	fullCircle.m_max =  HK_REAL_PI;
	m_arcs.pushBack( fullCircle );
	m_minAngle = .01f;
}

inline bool hkaiVisibilityUtil::VisibleSet::isEmpty() const
{
	return m_arcs.isEmpty();
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
