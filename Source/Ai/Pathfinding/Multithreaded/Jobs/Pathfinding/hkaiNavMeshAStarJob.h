/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAVMESH_ASTAR_JOB_H
#define HKAI_NAVMESH_ASTAR_JOB_H

#include <Common/Base/Thread/JobQueue/hkJobQueue.h>
#include <Ai/Pathfinding/Multithreaded/hkaiPathfindingJobs.h>

/// Processes a given nav mesh A* job
hkJobQueue::JobStatus HK_CALL hkaiProcessNavMeshAStarJob( hkJobQueue& jobQueue, hkJobQueue::JobQueueEntry& jobEntry );

//
// Output functions called by the job
//

/// Search failed
void HK_CALL hkaiNavMeshAStarJob_failure( const hkaiNavMeshAStarCommand& command );

#endif // HKAI_NAVMESH_ASTAR_JOB_H

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
