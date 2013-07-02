/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


/// PlayStation(R)3 only: Interface for determining whether the generator is allowed to be run on SPU.
inline hkBool32 hkaiSilhouetteGenerator::canRunOnSpu() const
{ 
	return (m_forceGenerateOntoPpu == 0);
}

/// PlayStation(R)3 only: Set whether the generator is allowed to be run on SPU.
inline hkUint8 hkaiSilhouetteGenerator::getForceGenerateOnPpuReasons() const
{ 
	return m_forceGenerateOntoPpu; 
}

/// PlayStation(R)3 only: Set whether the generator is allowed to be run on SPU.
inline void hkaiSilhouetteGenerator::setForceGenerateOnPpuReasons(hkUint8 c) 
{ 
	m_forceGenerateOntoPpu = c; 
}

inline hkaiSilhouetteGenerator::GeneratorType hkaiSilhouetteGenerator::getType() const
{ 
	return m_type;
}

inline hkUlong hkaiSilhouetteGenerator::getUserData() const
{
	return m_userData;
}

inline void hkaiSilhouetteGenerator::setUserData( hkUlong data )
{
	m_userData = data;
}

inline hkReal hkaiSilhouetteGenerator::getLazyRecomputeDisplacementThreshold() const
{
	return m_lazyRecomputeDisplacementThreshold;
}

inline void hkaiSilhouetteGenerator::setLazyRecomputeDisplacementThreshold( hkReal lazyRecomputeDisplacementThreshold )
{
	m_lazyRecomputeDisplacementThreshold = lazyRecomputeDisplacementThreshold;
}

inline void hkaiSilhouetteGenerator::_getRelativeTransform( const hkQTransform& meshInstanceTransform, hkQTransform& relativeTransform ) const
{
	relativeTransform.setMulInverseMul(meshInstanceTransform, m_transform);
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
