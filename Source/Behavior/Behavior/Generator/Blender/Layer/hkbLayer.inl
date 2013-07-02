/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkReal hkbLayer::getBoneWeight( int boneIndex ) const
{
	// if the bone weight was not specified then we return the default of 1
	if ( ( m_boneWeights == HK_NULL ) || ( m_boneWeights->m_boneWeights.getSize() <= boneIndex ) )
	{
		return 1.0f;
	}
	else
	{
		const hkReal boneWeight = m_boneWeights->m_boneWeights[boneIndex];

		HK_ASSERT( 0x5912b239, ( boneWeight >= 0.0f ) && ( boneWeight <= 1.0f ) );

		return boneWeight;
	}
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
