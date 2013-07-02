/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


inline hkaiReferenceFrame::hkaiReferenceFrame()
{
	m_transform.setIdentity();
	m_linearVelocity.setZero();
	m_angularVelocity.setZero();
}

inline hkaiReferenceFrame::hkaiReferenceFrame(hkFinishLoadedObjectFlag f)
{
}

inline const hkTransform& hkaiReferenceFrame::getTransform() const
{
	return m_transform;
}

inline const hkVector4& hkaiReferenceFrame::getLinearVelocity() const
{
	return m_linearVelocity;
}

inline const hkVector4& hkaiReferenceFrame::getAngularVelocity() const
{
	return m_angularVelocity;
}

inline void hkaiReferenceFrame::setTransform( const hkTransform& transform )
{
	m_transform = transform;
}

inline void hkaiReferenceFrame::setLinearVelocity( hkVector4Parameter linearVelocity )
{
	m_linearVelocity = linearVelocity;
}

inline void hkaiReferenceFrame::setAngularVelocity( hkVector4Parameter angularVelocity )
{
	m_angularVelocity = angularVelocity;
}


inline void hkaiReferenceFrame::transformLocalPosToWorldPos( hkVector4Parameter localPos, hkVector4& worldPos ) const
{
	hkVector4 _worldPos; _worldPos._setTransformedPos(m_transform, localPos);
	worldPos = _worldPos;
}

inline void hkaiReferenceFrame::transformLocalDirToWorldDir( hkVector4Parameter localDir, hkVector4& worldDir ) const
{
	hkVector4 _worldDir; _worldDir._setRotatedDir(m_transform.getRotation(), localDir);
	worldDir = _worldDir;
}

inline void hkaiReferenceFrame::transformLocalAngVelToWorldAngVel( hkSimdRealParameter localAngVel, hkVector4Parameter worldUp, hkSimdReal& worldAngVel ) const
{
	hkSimdReal worldAngVelImplicit; worldAngVelImplicit = m_angularVelocity.dot<3>(worldUp);
	hkSimdReal worldAngVelExplicit; worldAngVelExplicit = localAngVel;	// Already relative to world-space up vector
	hkSimdReal _worldAngVel; _worldAngVel.setAdd( worldAngVelExplicit, worldAngVelImplicit );

	worldAngVel = _worldAngVel;
}


inline void hkaiReferenceFrame::transformWorldPosToLocalPos( hkVector4Parameter worldPos, hkVector4& localPos ) const
{
	hkVector4 _localPos; _localPos._setTransformedInversePos(m_transform, worldPos);
	localPos = _localPos;
}

inline void hkaiReferenceFrame::transformWorldDirToLocalDir( hkVector4Parameter worldDir, hkVector4& localDir ) const
{
	hkVector4 _localDir; _localDir._setRotatedInverseDir(m_transform.getRotation(), worldDir);
	localDir = _localDir;
}

inline void hkaiReferenceFrame::transformWorldAngVelToLocalAngVel( hkSimdRealParameter worldAngVel, hkVector4Parameter worldUp, hkSimdReal& localAngVel ) const
{
	hkSimdReal worldAngVelImplicit; worldAngVelImplicit = m_angularVelocity.dot<3>(worldUp);
	hkSimdReal worldAngVelExplicit; worldAngVelExplicit = worldAngVel;	// Already relative to world-space up vector
	hkSimdReal _localAngVel; _localAngVel.setSub( worldAngVelExplicit, worldAngVelImplicit );

	localAngVel = _localAngVel;
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
