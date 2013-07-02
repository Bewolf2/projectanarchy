/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline void hkaiLocalSteeringJob::atomicIncrementAndReleaseSemaphore() const
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


HK_FORCE_INLINE hkaiAvoidanceSolverJob::hkaiAvoidanceSolverJob( )
:	hkaiLocalSteeringJob( JOB_LOCAL_STEERING_AVOIDANCE_SOLVE, sizeof(*this)),
	m_sectionInfo(HK_NULL),
	m_avoidancePairProps(HK_NULL),
	m_numAvoidancePairProps(0),
	m_commands(HK_NULL),
	m_numCommands(-1)
{
	
}

HK_FORCE_INLINE hkaiBehaviorCalcVelocitiesJob::hkaiBehaviorCalcVelocitiesJob()
: hkaiLocalSteeringJob(JOB_LOCAL_STEERING_CALC_VELOCITIES, sizeof(*this))
, m_behaviors(HK_NULL)
, m_numBehaviors(-1)
, m_outputs(HK_NULL)
, m_numOutputs(-1)
, m_timestep(-1.0f)
{
	m_jobSpuType = HK_JOB_SPU_TYPE_DISABLED;
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
