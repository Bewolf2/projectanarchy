/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkInt16 hkbRagdollInterface::getNumBones() const
{
	HK_ASSERT2(0x30db277c, getSkeleton() != HK_NULL, "Ragdolls must have hkaSkeletons.");
	return (hkInt16)getSkeleton()->m_bones.getSize();
}

HK_FORCE_INLINE int hkbRagdollInterface::getBoneIndexOfRigidBody(hkbRigidBodyHandle rb) const
{
	if ( rb != HKB_INVALID_PHYSICS_HANDLE )
	{
		for ( int i = 0; i < getNumBones(); i++ )
		{
			if ( rb == getRigidBodyOfBone( i ) )
			{
				return i;
			}
		}
	}
	return -1;
}

HK_FORCE_INLINE void hkbRagdollInterface::getPoseModelSpace(
	hkQsTransform* poseModelSpaceOut,
	const hkQsTransform& worldFromModel ) const
{
	// Adapted from hkaRagdollInstance

	getPoseWorldSpace( poseModelSpaceOut );

	const int numBones = getNumBones();

	hkQsTransform modelFromWorld;
	modelFromWorld.setInverse( worldFromModel );

	for (int b=0; b < numBones; b++)
	{
		poseModelSpaceOut[b].setMul(modelFromWorld, poseModelSpaceOut[b]);
	}
}

HK_FORCE_INLINE void hkbRagdollInterface::setPoseModelSpace(
	hkQsTransform* poseModelSpaceIn,
	const hkQsTransform& worldFromModel )
{
	// Adapted from hkaRagdollInstance

	const int numBones = getNumBones();
	hkLocalBuffer<hkQsTransform> poseWorldSpace( numBones );

	for (int b=0; b<numBones; b++)
	{
		poseWorldSpace[b].setMul( worldFromModel, poseModelSpaceIn[b] );
		poseWorldSpace[b].m_rotation.normalize();
	}

	setPoseWorldSpace( poseWorldSpace.begin() );
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
