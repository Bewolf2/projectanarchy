/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiSilhouetteGeneratorSectionContext::hkaiSilhouetteGeneratorSectionContext()
: 	m_generator(HK_NULL)
,	m_generatorSize(0)
{

}

inline hkaiSilhouetteGeneratorSectionContext::hkaiSilhouetteGeneratorSectionContext( hkFinishLoadedObjectFlag f)
: m_lastRelativeTransform(f)
{
}

inline hkaiSilhouetteGeneratorSectionContext::~hkaiSilhouetteGeneratorSectionContext()
{
	setGenerator(HK_NULL); 
}


inline const hkaiSilhouetteGenerator* hkaiSilhouetteGeneratorSectionContext::getGenerator() const
{ 
	return m_generator; 
}

inline int hkaiSilhouetteGeneratorSectionContext::getGeneratorSize() const
{ 
	return m_generatorSize; 
}

inline void hkaiSilhouetteGeneratorSectionContext::setGenerator( const hkaiSilhouetteGenerator* gen )
{
	if (gen)
	{ 
		gen->addReference();
	}
	
	if (m_generator)
	{
		m_generator->removeReference();
	}
	
	m_generator = gen;
	updateGeneratorSize();
} 


void hkaiSilhouetteGeneratorSectionContext::updateGeneratorSize()
{
	m_generatorSize = (m_generator) ? m_generator->getSize() : 0;
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
