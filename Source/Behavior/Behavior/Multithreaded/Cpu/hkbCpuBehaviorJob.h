/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_CPU_BEHAVIOR_JOB_H
#define HK_CPU_BEHAVIOR_JOB_H

#include <Behavior/Behavior/Multithreaded/hkbBehaviorJob.h>
#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h>

struct hkbNodePartitionInfoInternal;

	/// Behavior job processing functions for the CPU only.
class hkbCpuBehaviorJob
{
	public:

			/// Process an hkbBehaviorGraphBehaviorJob.
		static hkJobQueue::JobStatus HK_CALL processGenerateBehaviorGraphJob( hkJobQueue& jobQueue, hkJobQueue::JobQueueEntry& nextJobOut );

			/// Process an hkbGenerateNodesJob.
		static hkJobQueue::JobStatus HK_CALL processGenerateNodesJob( hkJobQueue& jobQueue, hkJobQueue::JobQueueEntry& nextJobOut );

			/// Process an hkbUpdateCharacterJob.
		static hkJobQueue::JobStatus HK_CALL processUpdateCharacterJob( hkJobQueue& jobQueue, hkJobQueue::JobQueueEntry& nextJobOut );

	private:

			/// Initialize the node setup.  This usually involves creating the data that needs to be sent to the SPU
			/// in order for the node to be generated.  The node itself must be passed in in nodeSetup.
		static void HK_CALL initNodeSetup(	hkbGenerateNodesJobNodeSetup& nodeSetupInOut,
											hkbGenerateNodesJob& job,
											hkbBehaviorGraph::GenerateWorkingData& data );
											
			/// Create a job for the SPU.
		static void HK_CALL initSpuJob( hkbBehaviorGraph::GenerateWorkingData& data,
										hkbNodePartitionInfoInternal* partitionInfoInternal,
										const int firstNodeInPartitionIndex );

			/// Add a SPU job for the next partition
		static void HK_CALL addSpuJob( hkJobQueue& jobQueue, hkbBehaviorGraph::GenerateWorkingData& data );

			/// Simulate the CPU job.
		static void HK_CALL simulateCpuJob( hkbBehaviorGraph::GenerateWorkingData& data );

			/// Compute the children for a single partition
		static void HK_CALL computeChildren(	hkbGenerateNodesJob& job,
												hkbBehaviorGraph::GenerateWorkingData& data,
												hkbNodePartitionInfoInternal* partitionInfoInternal,
												hkInt16* intermediateOutputSizes,
												const int numTotalChildren,
												const int firstNodeInPartitionIndex );

};

#endif

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
