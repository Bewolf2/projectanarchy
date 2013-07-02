/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//
// Nav mesh path request access
//
hkaiNavMeshPathRequestInfo::hkaiNavMeshPathRequestInfo()
{
	m_priority = 0;
	m_owner = HK_NULL;
	m_markedForDeletion = false;
}

hkaiNavMeshPathRequestInfo::hkaiNavMeshPathRequestInfo( hkFinishLoadedObjectFlag f)
: hkReferencedObject(f)
, m_input(f)
, m_output(f)
{

}

inline int hkaiNavMeshPathRequestInfo::getPriority() const
{
	return m_priority;
}

inline hkBool32 hkaiNavMeshPathRequestInfo::isProcessed() const
{
	return (m_output != HK_NULL);
}

inline hkaiPathfindingUtil::FindPathInput* hkaiNavMeshPathRequestInfo::getNavMeshInput()
{
	return m_input;
}

inline const hkaiPathfindingUtil::FindPathInput* hkaiNavMeshPathRequestInfo::getNavMeshInput() const
{
	return m_input;
}

inline hkaiPathfindingUtil::FindPathOutput* hkaiNavMeshPathRequestInfo::getNavMeshOutput()
{
	return m_output;
}

inline const hkaiPathfindingUtil::FindPathOutput* hkaiNavMeshPathRequestInfo::getNavMeshOutput() const
{
	return m_output;
}

inline hkBool32 hkaiNavMeshPathRequestInfo::operator< ( const hkaiNavMeshPathRequestInfo& other ) const
{
	return m_priority > other.m_priority;
}

inline void hkaiNavMeshPathRequestInfo::markForDelete()
{
	m_markedForDeletion = true;
}

inline bool hkaiNavMeshPathRequestInfo::isMarkedForDeletion() const
{
	return m_markedForDeletion;
}


inline void hkaiNavMeshPathRequestInfo::deallocate()
{
	m_markedForDeletion = true;

	// Each of these assignments removes a reference if the old ptr was non-NULL
	m_input = HK_NULL;
	m_output = HK_NULL;
}



//
// Nav volume path requests
//

hkaiNavVolumePathRequestInfo::hkaiNavVolumePathRequestInfo()
{
	m_priority = 0;
	m_owner = HK_NULL;
	m_markedForDeletion = false;
}

hkaiNavVolumePathRequestInfo::hkaiNavVolumePathRequestInfo( hkFinishLoadedObjectFlag f)
: hkReferencedObject(f)
, m_input(f)
, m_output(f)
{

}

inline int hkaiNavVolumePathRequestInfo::getPriority() const
{
	return m_priority;
}

inline hkBool32 hkaiNavVolumePathRequestInfo::isProcessed() const
{
	return (m_output != HK_NULL);
}

inline hkaiVolumePathfindingUtil::FindPathInput* hkaiNavVolumePathRequestInfo::getNavVolumeInput()
{
	return m_input;
}

inline const hkaiVolumePathfindingUtil::FindPathInput* hkaiNavVolumePathRequestInfo::getNavVolumeInput() const
{
	return m_input;
}

inline hkaiVolumePathfindingUtil::FindPathOutput* hkaiNavVolumePathRequestInfo::getNavVolumeOutput()
{
	return m_output;
}

inline const hkaiVolumePathfindingUtil::FindPathOutput* hkaiNavVolumePathRequestInfo::getNavVolumeOutput() const
{
	return m_output;
}

inline hkBool32 hkaiNavVolumePathRequestInfo::operator< ( const hkaiNavVolumePathRequestInfo& other ) const
{
	return m_priority > other.m_priority;
}

inline void hkaiNavVolumePathRequestInfo::markForDelete()
{
	m_markedForDeletion = true;
}

inline bool hkaiNavVolumePathRequestInfo::isMarkedForDeletion() const
{
	return m_markedForDeletion;
}


inline void hkaiNavVolumePathRequestInfo::deallocate()
{
	m_markedForDeletion = true;

	// Each of these assignments removes a reference if the old ptr was non-NULL
	m_input = HK_NULL;
	m_output = HK_NULL;
}



//
// hkaiWorld access
//


inline const hkaiWorld::ObstacleGeneratorArray& hkaiWorld::getObstacleGenerators() const
{
	return m_obstacleGenerators;
}

inline hkaiWorld::ObstacleGeneratorArray& hkaiWorld::getObstacleGenerators()
{
	return m_obstacleGenerators;
}

inline hkaiWorld::SilhouetteGeneratorArray&	 hkaiWorld::getSilhouetteGenerators()
{
	return m_silhouetteGenerators;
}

inline const hkaiWorld::SilhouetteGeneratorArray&	 hkaiWorld::getSilhouetteGenerators() const
{
	return m_silhouetteGenerators;
}

inline hkaiOverlapManager*  hkaiWorld::getOverlapManager()
{
	return m_overlapManager;
}

inline const hkaiOverlapManager*  hkaiWorld::getOverlapManager() const
{
	return m_overlapManager;
}

inline const hkaiAvoidancePairProperties* hkaiWorld::getAvoidancePairProperties() const
{
	return m_avoidancePairProps;
}

inline hkaiStreamingCollection* hkaiWorld::getStreamingCollection()
{
	return m_streamingCollection;
}

inline const hkaiStreamingCollection* hkaiWorld::getStreamingCollection() const
{
	return m_streamingCollection;
}

inline const hkaiPathfindingUtil::FindPathInput& hkaiWorld::getDefaultPathfindingInput() const
{
	return m_defaultPathfindingInput;
}

inline hkaiPathfindingUtil::FindPathInput& hkaiWorld::getDefaultPathfindingInput()
{
	return m_defaultPathfindingInput;
}

inline const hkaiVolumePathfindingUtil::FindPathInput& hkaiWorld::getDefaultVolumePathfindingInput() const
{
	return m_defaultVolumePathfindingInput;
}

inline hkaiVolumePathfindingUtil::FindPathInput& hkaiWorld::getDefaultVolumePathfindingInput()
{
	return m_defaultVolumePathfindingInput;
}

inline const hkaiNavMeshCutter* hkaiWorld::getNavMeshCutter() const
{
	return m_cutter;
}

inline hkaiNavMeshCutter* hkaiWorld::getNavMeshCutter()
{
	return m_cutter;
}

inline bool hkaiWorld::getForceSilhouetteUpdates() const
{
	return m_forceSilhouetteUpdates;
}

inline void hkaiWorld::setForceSilhouetteUpdates( bool forceSilhouetteUpdates )
{
	m_forceSilhouetteUpdates = forceSilhouetteUpdates;
}

inline hkaiSilhouetteGenerationParameters& hkaiWorld::getSilhouetteGenerationParams()
{
	return m_silhouetteGenerationParameters;
}

inline const hkaiSilhouetteGenerationParameters& hkaiWorld::getSilhouetteGenerationParams() const
{
	return m_silhouetteGenerationParameters;
}

inline int hkaiWorld::getMaxRequestsPerStep() const
{
	return m_maxRequestsPerStep;
}

inline void hkaiWorld::setMaxRequestsPerStep( int m )
{
	m_maxRequestsPerStep = m;
}

inline int hkaiWorld::getMaxEstimatedIterationsPerStep() const
{
	return m_maxEstimatedIterationsPerStep;
}

inline void hkaiWorld::setMaxEstimatedIterationsPerStep( int maxEstimatedIterationsPerStep )
{
	m_maxEstimatedIterationsPerStep = maxEstimatedIterationsPerStep;
}

inline int hkaiWorld::getPriorityThreshold() const
{
	return m_priorityThreshold;
}

inline void hkaiWorld::setPriorityThreshold( int t )
{
	m_priorityThreshold = t;
}

inline int hkaiWorld::getNumPathRequestsPerJob() const
{
	return m_numPathRequestsPerJob;
}

inline void hkaiWorld::setNumPathRequestsPerJob(int n)
{
	m_numPathRequestsPerJob = n;
}

inline int hkaiWorld::getNumBehaviorUpdatesPerJob() const
{
	return m_numBehaviorUpdatesPerJob;
}

inline void hkaiWorld::setNumBehaviorUpdatesPerJob(int n)
{
	m_numBehaviorUpdatesPerJob = n;
}

inline int hkaiWorld::getNumCharactersPerAvoidanceJob() const
{
	return m_numCharactersPerAvoidanceJob;
}

inline void hkaiWorld::setNumCharactersPerAvoidanceJob(int n) 
{
	m_numCharactersPerAvoidanceJob = n;
}

inline int hkaiWorld::getMaxPathSearchEdgesOut() const
{
	return m_maxPathSearchEdgesOut;
}

inline void hkaiWorld::setMaxPathSearchEdgesOut(int n)
{
	m_maxPathSearchEdgesOut = n;
}

inline int hkaiWorld::getMaxPathSearchPointsOut() const
{
	return m_maxPathSearchPointsOut;
}

inline void hkaiWorld::setMaxPathSearchPointsOut( int n )
{
	m_maxPathSearchPointsOut = n;
}

inline hkReal hkaiWorld::getErosionRadius() const
{
	return m_erosionRadius;
}

inline void hkaiWorld::setErosionRadius(hkReal erosionRadius)
{
	m_erosionRadius = erosionRadius;
	m_silhouetteGenerationParameters.m_extraExpansion = m_erosionRadius;
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
