/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline void hkaiDynamicNavMeshJob::atomicIncrementAndReleaseSemaphore() const
{
	if (m_jobDoneFlag)
	{
#ifdef HK_PLATFORM_SPU
		hkSpuDmaManager::atomicExchangeAdd(m_jobDoneFlag, 1);
#else
		hkCriticalSection::atomicExchangeAdd(m_jobDoneFlag, 1);
#endif
	}

	if (m_semaphore)
	{
		hkSemaphoreBusyWait::release(m_semaphore);
	}
}


HK_FORCE_INLINE hkaiNavMeshCutFaceJob::hkaiNavMeshCutFaceJob( )
:	hkaiDynamicNavMeshJob( JOB_DYNAMIC_NAVMESH_CUT_FACE, sizeof(*this))
{
	m_meshInstance = HK_NULL;
	m_faceKey = HKAI_INVALID_PACKED_KEY;
	m_generatorContexts = HK_NULL;
	m_numGenerators = -1;
	m_firstNonCuttingGeneratorIndex = -1;
	m_worldUp.setZero();
	m_extrusionAmount = 0.0f;
	m_radiusExpansionAmount = 0.0f;
	m_isWallClimbing = false;
	m_results = HK_NULL;
	m_generationParams = HK_NULL;
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
