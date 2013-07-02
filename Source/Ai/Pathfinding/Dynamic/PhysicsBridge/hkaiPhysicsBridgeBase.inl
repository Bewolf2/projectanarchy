/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


inline const hkVector4& hkaiPhysicsBridgeBase::getWorldShift() const
{
	return m_worldShift;
}


inline void hkaiPhysicsBridgeBase::setWorldShift( hkVector4Parameter shift )
{
	m_worldShift = shift;
}

inline void hkaiPhysicsBridgeBase::setIgnoreBodyFlags( BodyTypeFlags flags)
{
	m_ignoreBodyFlags = flags;
}

inline hkaiPhysicsBridgeBase::BodyTypeFlags hkaiPhysicsBridgeBase::getIgnoreBodyFlags() const
{
	return m_ignoreBodyFlags;
}

inline hkReal hkaiPhysicsBridgeBase::getDefaultSilhouetteDistanceThreshold() const
{
	return m_defaultSilhouetteDistanceThreshold;
}

inline void hkaiPhysicsBridgeBase::setDefaultSilhouetteDistanceThreshold( hkReal val )
{
	m_defaultSilhouetteDistanceThreshold = val;
}

inline hkReal hkaiPhysicsBridgeBase::getDefaultSilhouetteVelocityThreshold() const
{
	return m_defaultSilhouetteVelocityThreshold;
}

inline void hkaiPhysicsBridgeBase::setDefaultSilhouetteVelocityThreshold( hkReal val )
{
	m_defaultSilhouetteVelocityThreshold = val;
}

inline hkaiPointCloudSilhouetteGenerator::DetailLevel hkaiPhysicsBridgeBase::getDefaultDetailLevel() const
{
	return m_defaultDetailLevel;
}

inline void hkaiPhysicsBridgeBase::setDefaultDetailLevel( hkaiPointCloudSilhouetteGenerator::DetailLevel val )
{
	m_defaultDetailLevel = val;
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
