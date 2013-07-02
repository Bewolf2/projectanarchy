/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbPhysicsInterface* hkbPhysicsUtils::warnPhysicsInterface( const hkbContext& context )
{
	return warnPhysicsInterface( context.getPhysicsInterface() );
}

HK_FORCE_INLINE hkbPhysicsInterface* hkbPhysicsUtils::warnPhysicsInterface( hkbPhysicsInterface* physicsInterface )
{
	HK_WARN_ON_DEBUG_IF( physicsInterface == HK_NULL, 0x22440065, "Physics Interface required for this function call." );
	return physicsInterface;
}

HK_FORCE_INLINE hkbPhysicsInterface* hkbPhysicsUtils::requirePhysicsInterface( const hkbContext& context )
{
	return requirePhysicsInterface( context.getPhysicsInterface() );
}

HK_FORCE_INLINE hkbPhysicsInterface* hkbPhysicsUtils::requirePhysicsInterface( hkbPhysicsInterface* physicsInterface )
{
	HK_ASSERT2(0x20cc800e, physicsInterface != HK_NULL, "Physics Interface required for this function call.");
	return physicsInterface;
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
