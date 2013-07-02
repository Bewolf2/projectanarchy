/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE bool hkbRagdollDriver::isEnabled() const
{		
	return m_isEnabled;
}

HK_FORCE_INLINE void hkbRagdollDriver::setEnabled( bool enabled )
{	
	m_isEnabled = enabled;
}

HK_FORCE_INLINE bool hkbRagdollDriver::isPoweredControllerEnabled()
{ 
	return m_isPoweredControllerEnabled; 
}

HK_FORCE_INLINE bool hkbRagdollDriver::isRigidBodyControllerEnabled() 
{ 
	return m_isRigidBodyControllerEnabled; 
}

HK_FORCE_INLINE bool hkbRagdollDriver::isRagdollActive()
{
	return ( m_isRigidBodyControllerEnabled || m_isPoweredControllerEnabled );
}

HK_FORCE_INLINE hkbRagdollInterface* hkbRagdollDriver::getRagdollInterface() const
{
	return m_ragdollInterface;
}

HK_FORCE_INLINE const hkQsTransform* hkbRagdollDriver::getRagdollPoseWS()
{		
	HK_ASSERT2( 0x49084017, m_isEnabled, "Ragdoll controller is not enabled.  Cannot get ragdoll pose." );

	m_ragdollPoseWasUsed = true;

	return m_ragdollPoseWS;	
}

HK_FORCE_INLINE void hkbRagdollDriver::extractRagdollPose(	const hkbContext& context,
	hkbGeneratorOutput& inOut,
	bool computeWorldFromModel,
	int poseMatchingBone0,
	int poseMatchingBone1,
	int poseMatchingBone2 )
{
	extractRagdollPoseInternal( context, 
								inOut, 
								0.0f, 
								0.0f, 
								false,
								computeWorldFromModel, 
								poseMatchingBone0, 
								poseMatchingBone1, 
								poseMatchingBone2 );
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
