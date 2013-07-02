/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkReal hkaiEdgeFollowingBehavior::getCharacterRadiusMultiplier() const
{
	return m_characterRadiusMultiplier;
}

inline void hkaiEdgeFollowingBehavior::setCharacterRadiusMultiplier( hkReal m )
{
	m_characterRadiusMultiplier = m;
}

inline const hkaiPathFollowingProperties* hkaiEdgeFollowingBehavior::getPathFollowingProperties() const
{
	return m_pathFollowingProperties.val();
}

inline hkaiPathFollowingProperties* hkaiEdgeFollowingBehavior::getPathFollowingPropertiesRW() 
{
	return const_cast<hkaiPathFollowingProperties* >( m_pathFollowingProperties.val() );
}

inline void hkaiEdgeFollowingBehavior::setPathFollowingProperties( const hkaiPathFollowingProperties* pathFollowingProperties )
{
	m_pathFollowingProperties = pathFollowingProperties;
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
