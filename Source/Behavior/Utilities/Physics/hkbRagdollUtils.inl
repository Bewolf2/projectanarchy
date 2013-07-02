/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbRagdollInterface* hkbRagdollUtils::warnRagdollInterface( const hkbContext& context )
{
	return warnRagdollInterface( context.getCharacter() );
}

HK_FORCE_INLINE hkbRagdollInterface* hkbRagdollUtils::warnRagdollInterface( const hkbCharacter* character )
{
	HK_WARN_ON_DEBUG_IF(character == HK_NULL, 0x22440066, "Character required for this function call.");
	if ( character != HK_NULL )
	{
		return warnRagdollInterface( character->getRagdollInterface() );
	}
	return HK_NULL;
}

HK_FORCE_INLINE hkbRagdollInterface* hkbRagdollUtils::warnRagdollInterface( const hkbRagdollInterface* ragdollInterface )
{
	HK_WARN_ON_DEBUG_IF(ragdollInterface == HK_NULL, 0x22440067, "Ragdoll Interface required for this function call.");
	return const_cast<hkbRagdollInterface*>(ragdollInterface);
}

HK_FORCE_INLINE hkbRagdollInterface* hkbRagdollUtils::requireRagdollInterface( const hkbContext& context )
{
	return requireRagdollInterface( context.getCharacter() );
}

HK_FORCE_INLINE hkbRagdollInterface* hkbRagdollUtils::requireRagdollInterface( const hkbCharacter* character )
{
	HK_ASSERT2(0x4fde144f, character != HK_NULL, "Character required for this function call.");
	return requireRagdollInterface( character->getRagdollInterface() );
}

HK_FORCE_INLINE hkbRagdollInterface* hkbRagdollUtils::requireRagdollInterface( const hkbRagdollInterface* ragdollInterface )
{
	HK_ASSERT2(0xcc673cc, ragdollInterface != HK_NULL, "Ragdoll Interface required for this function call.");
	return const_cast<hkbRagdollInterface*>(ragdollInterface);
}

HK_FORCE_INLINE void hkbRagdollUtils::requireCorrectBufferSize(
	hkbRagdollInterface* ragdollInterface,
	int bufferSize )
{
	requireRagdollInterface( ragdollInterface );
	HK_ASSERT2(0x3fa1eda9, ragdollInterface->getNumBones() <= bufferSize, "Output buffer is not big enough to contain ragdoll pose.");
	HK_WARN_ON_DEBUG_IF( ragdollInterface->getNumBones() != bufferSize, 0x22440068, "Output buffer is a different size than ragdoll pose.");
}

HK_FORCE_INLINE void hkbRagdollUtils::getPoseModelSpace(
	hkbRagdollInterface* ragdollInterface,
	const hkQsTransform& worldFromModel,
	hkQsTransform* poseModelSpaceOut,
	int poseOutSize	)
{
	requireCorrectBufferSize( ragdollInterface, poseOutSize );
	ragdollInterface->getPoseModelSpace( poseModelSpaceOut, worldFromModel );
}

HK_FORCE_INLINE void hkbRagdollUtils::setPoseModelSpace(
	hkbRagdollInterface* ragdollInterface,
	const hkQsTransform& worldFromModel,
	hkQsTransform* poseModelSpaceIn,
	int poseInSize )
{
	requireCorrectBufferSize( ragdollInterface, poseInSize );
	ragdollInterface->setPoseModelSpace( poseModelSpaceIn, worldFromModel );
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
