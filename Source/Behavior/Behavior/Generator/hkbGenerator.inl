/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// can't construct hkbGeneratorSyncInfo on SPU
#if !defined(HK_PLATFORM_SPU)

HK_FORCE_INLINE hkbGeneratorSyncInfo& hkbGenerator::accessSyncInfo()
{
	HK_ASSERT2( 0x45813bf7, canBeClone(), "you cannot call accessSyncInfo on a node template" );

	if ( m_syncInfo == HK_NULL )
	{
		m_syncInfo = new hkbGeneratorSyncInfo();
	}

	return *m_syncInfo;
}

#endif

HK_FORCE_INLINE const hkbGeneratorSyncInfo& hkbGenerator::getSyncInfo() const
{
	HK_ASSERT2( 0x591b296a, canBeClone(), "you cannot call getSyncInfo on a node template" );
	HK_ASSERT2( 0xd86351c7, m_syncInfo != HK_NULL, "m_syncInfo should not be null" );

	return *m_syncInfo;
}

HK_FORCE_INLINE hkbGeneratorPartitionInfo& hkbGenerator::accessPartitionInfo()
{
	HK_ASSERT2( 0x45813bf7, canBeClone(), "you cannot call accessPartitionInfo on a node template" );

	return m_partitionInfo;
}

HK_FORCE_INLINE const hkbGeneratorPartitionInfo& hkbGenerator::getPartitionInfo() const
{
	HK_ASSERT2( 0x591b296a, canBeClone(), "you cannot call getPartitionInfo on a node template" );
	
	return m_partitionInfo;
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
