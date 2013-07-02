/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiOverlapManager::Section::GeneratorData::GeneratorData( const hkaiSilhouetteGenerator* gen, const hkQTransform& initialTransform )
{
	m_context.setGenerator( gen );
	m_context.m_lastRelativeTransform = initialTransform;
	m_context.m_generatedLastFrame = true;
	m_context.m_generatingThisFrame = true;
}

inline hkaiOverlapManager::Section::GeneratorData* hkaiOverlapManager::Section::getGeneratorData(const hkaiSilhouetteGenerator* gen)
{
	int idx = m_generatorDataMap->getWithDefault(gen, -1);
	return ( idx == -1 ? HK_NULL : m_generatorData[idx] );
}

inline const hkaiOverlapManager::Section::GeneratorData* hkaiOverlapManager::Section::getGeneratorData(const hkaiSilhouetteGenerator* gen) const
{
	int idx = m_generatorDataMap->getWithDefault(gen, -1);
	return ( idx == -1 ? HK_NULL : m_generatorData[idx] );
}

inline void hkaiOverlapManager::Section::setFaceDirty( hkaiNavMesh::FaceIndex f, hkReal priorityDelta )
{
	hkIntRealPair pair(f, priorityDelta); 
	{
		PriorityMap::Iterator iter = m_facePriorities.findElement( pair);
		if ( m_facePriorities.isValid(iter) )
		{
			hkIntRealPair oldPair = m_facePriorities.getElement(iter);
			pair.m_value += oldPair.m_value;
		}
	}
	
	m_facePriorities.insert(pair);
}

inline void hkaiOverlapManager::Section::setFaceClean( hkaiNavMesh::FaceIndex f )
{
	hkIntRealPair pair(f, 0); // value doesn't matter, only the key
	hkResult res = m_facePriorities.remove(pair);
	HK_ASSERT(0x6f593509, res == HK_SUCCESS);
}

inline bool hkaiOverlapManager::Section::isFaceDirty(hkaiNavMesh::FaceIndex f) const
{
	hkIntRealPair pair(f, 0); // value doesn't matter, only the key
	return (bool) m_facePriorities.contains(pair);
}

inline bool hkaiOverlapManager::Section::isEmpty() const
{
	return (m_numOriginalFaces == 0);
}

inline int hkaiOverlapManager::Section::getNumFaces() const
{
	return m_numOriginalFaces;
}

const inline hkArray<hkaiOverlapManager::Section>& hkaiOverlapManager::getSectionData() const
{
	return m_sections;
}

inline const hkaiSpatialQueryHitFilter* hkaiOverlapManager::getSilhouetteFilter() const
{
	return m_silhouetteFilter;
}

inline void hkaiOverlapManager::setHasMovedTolerance( hkReal tolerance )
{
	m_hasMovedTolerance = tolerance;
}

inline hkReal hkaiOverlapManager::getHasMovedTolerance() const
{
	return m_hasMovedTolerance;
}

inline void hkaiOverlapManager::setMaxCutFacesPerStep( int numFaces )
{
	m_maxCutFacesPerStep = numFaces;
}

inline int hkaiOverlapManager::getMaxCutFacesPerStep( ) const
{
	return m_maxCutFacesPerStep;
}

inline const hkaiSilhouettePriorityController* hkaiOverlapManager::getSilhouettePriorityController() const
{
	return m_priorityController;
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
