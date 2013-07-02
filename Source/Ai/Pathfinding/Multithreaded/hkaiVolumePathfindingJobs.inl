/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline void hkaiVolumePathfindingJob::atomicIncrementAndReleaseSemaphore() const
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

HK_FORCE_INLINE hkaiNavVolumeAStarJob::hkaiNavVolumeAStarJob( const hkaiStreamingCollection& collection)
:	hkaiVolumePathfindingJob( JOB_PATHFINDING_VOLUME_ASTAR, sizeof(*this)),
	m_maxTotalIterations(100000),
	m_commands(HK_NULL),
	m_numCommands(0)
{
	m_streamingSectionInfo = collection.getInstanceInfoPtr();
}

HK_FORCE_INLINE void hkaiNavVolumeAStarJob::set( const hkaiVolumePathfindingUtil::FindPathInput& input)
{
	m_searchParameters = input.m_searchParameters;
}

HK_FORCE_INLINE hkBool32 hkaiNavVolumeAStarJob::isEquivalent( const hkaiVolumePathfindingUtil::FindPathInput& input) const
{
	return( m_searchParameters == input.m_searchParameters );
}


inline void hkaiNavVolumeAStarCommand::init()
{
	m_startPoint.setZero();
	m_goalPoint.setZero();
	m_startCellKey = HKAI_INVALID_PACKED_KEY;
	m_goalCellKey = HKAI_INVALID_PACKED_KEY;

	m_multipleGoalPoints = HK_NULL;
	m_multipleGoalCellKeys = HK_NULL;
	m_numGoals = 0;

	m_maxNumberOfIterations = 100000;
	m_agentInfo.init();

// 	m_searchSphereRadius = -1.f;
// 	m_searchCapsuleRadius = -1.f;

	m_AStarOutput = HK_NULL;
	m_cellsOut = HK_NULL;
	m_maxCellsOut = 0;
	m_pointsOut = HK_NULL; 
	m_maxPointsOut = 0;
}

inline void hkaiNavVolumeAStarCommand::setStartPointAndCell( hkVector4Parameter startPoint, hkaiPackedKey startCellKey )
{
	m_startPoint = startPoint;
	m_startCellKey = startCellKey;
}

inline void hkaiNavVolumeAStarCommand::setGoalPointAndCell( hkVector4Parameter goalPoint, hkaiPackedKey goalCellKey )
{
	// Copy the single goal by value. This scheme avoids an extra DMA in the single-goal case.
	m_goalPoint = goalPoint;
	m_goalCellKey = goalCellKey;
	m_multipleGoalPoints = HK_NULL;
	m_multipleGoalCellKeys = HK_NULL;
	m_numGoals = 1;
}

inline void hkaiNavVolumeAStarCommand::setMultipleGoalPointsAndCells( const hkVector4* goalPoints, const hkaiPackedKey* goalCellKeys, int numGoals )
{
	HK_ASSERT(0x346b517a, numGoals < hkaiNavVolumeAStarCommand::MAX_GOALS);

	if ( numGoals == 1 )
	{
		setGoalPointAndCell( goalPoints[0], goalCellKeys[0] );
	}
	else if ( numGoals > 1)
	{
		HK_CHECK_ALIGN16(goalPoints);	// Address must be aligned for DMA
		HK_CHECK_ALIGN16(goalCellKeys);	// Address must be aligned for DMA

		// Copy the first goal by value. This scheme avoids an extra DMA in the single-goal case.
		m_goalPoint = goalPoints[0];
		m_goalCellKey = goalCellKeys[0];
		m_multipleGoalPoints = goalPoints;
		m_multipleGoalCellKeys = goalCellKeys;
		m_numGoals = numGoals;
	}
	else
	{
		m_multipleGoalPoints = HK_NULL;
		m_multipleGoalCellKeys = HK_NULL;
		m_numGoals = 0;
	}
}

HK_FORCE_INLINE hkaiNavVolumeLineOfSightJob::hkaiNavVolumeLineOfSightJob( const hkaiStreamingCollection& collection)
:	hkaiVolumePathfindingJob( JOB_PATHFINDING_VOLUME_LINE_OF_SIGHT, sizeof(*this)),
	m_commands(HK_NULL),
	m_numCommands(0),
	m_maxNumberOfIterations(100000),
	m_edgeFilter(HK_NULL)
{
	m_streamingInfo = collection.getInstanceInfoPtr();
}

inline void hkaiNavVolumeLineOfSightCommand::init()
{
	m_startPoint.setZero();
	m_endPoint.setZero();

	m_startCellKey = HKAI_INVALID_PACKED_KEY;
	m_endCellKey = HKAI_INVALID_PACKED_KEY;

	m_agentInfo.init();

//	m_searchSphereRadius = -1.f;
//	m_searchCapsuleRadius = -1.f;

	m_result = HK_NULL;
	m_cellsOut = HK_NULL;
	m_maxCellsOut = 0;
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
