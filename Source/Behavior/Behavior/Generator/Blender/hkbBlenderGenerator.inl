/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE int hkbBlenderGenerator::getNumGenerators() const  
{ 
	return m_children.getSize(); 
}

HK_FORCE_INLINE hkbGenerator* hkbBlenderGenerator::getGenerator( int generatorIndex ) const
{
	HK_ASSERT( 0xcac36a91, generatorIndex >= 0 && generatorIndex < m_children.getSize() );

	return m_children[generatorIndex]->m_generator;
}

HK_FORCE_INLINE void hkbBlenderGenerator::setGeneratorWeight( int generatorIndex, hkReal weight )
{
	HK_ASSERT( 0xc3cc7a92, generatorIndex >= 0 && generatorIndex < m_children.getSize() );

	m_children[generatorIndex]->m_weight = weight;
}

HK_FORCE_INLINE hkReal hkbBlenderGenerator::getWorldFromModelWeight( int generatorIndex ) const
{
	HK_ASSERT( 0xcfc33a93, generatorIndex >= 0 && generatorIndex < m_children.getSize() );

	return m_children[generatorIndex]->getWorldFromModelWeight();
}

HK_FORCE_INLINE void hkbBlenderGenerator::setWorldFromModelWeight( int generatorIndex, hkReal weight )
{
	HK_ASSERT( 0xcab36a18, generatorIndex >= 0 && generatorIndex < m_children.getSize() );

	m_children[generatorIndex]->m_worldFromModelWeight = weight;
}

HK_FORCE_INLINE hkReal hkbBlenderGenerator::getBoneWeight( int generatorIndex, int boneIndex ) const
{
	return m_children[generatorIndex]->getBoneWeight( boneIndex );
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
