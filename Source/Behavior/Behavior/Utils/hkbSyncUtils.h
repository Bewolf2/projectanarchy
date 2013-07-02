/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SYNC_UTILS_H
#define HKB_SYNC_UTILS_H

#include <Behavior/Behavior/Generator/hkbGeneratorSyncInfo.h>
#include <Common/Base/Container/LocalArray/hkLocalArray.h>
#include <Common/Base/Container/LocalArray/hkLocalBuffer.h>


class hkbGenerator;

class hkbSyncUtils
{
	public:
			/// Compute the sync points that are common to a set of hkbGeneratorSyncInfos.  The sync points are
			/// output in the same order as they occur in syncInfos[0].  Returns the number of common sync points.
		static int HK_CALL computeCommonSyncPoints(	hkbGeneratorSyncInfo** syncInfos, 
													int numSyncInfos, 
													int commonSyncPointIdsOut[hkbGeneratorSyncInfo::MAX_SYNC_POINTS] );
			
			/// Compute playback speeds and local times for active and new sync infos using the frequency-based synchronization method.
		static void HK_CALL computePlaybackSpeeds( hkbGeneratorSyncInfo** activeSyncInfos, hkReal* activeBlendWeights, int numActiveSyncInfos,
												   hkbGeneratorSyncInfo** newSyncInfos, hkReal* newBlendWeights, int numNewSyncInfos,												   
												   hkbGeneratorSyncInfo* parentSyncInfo );

			/// Compute playback speeds and local times for active and new sync infos using the velocity-based synchronization method.
		static void HK_CALL computePlaybackSpeedsVelocityBased( hkbGenerator** activeGenerators, hkbGeneratorSyncInfo** activeSyncInfos, hkReal* activeBlendWeights, int numActiveSyncInfos,
													hkbGenerator** newGenerators, hkbGeneratorSyncInfo** newSyncInfos, hkReal* newBlendWeights, int numNewSyncInfos,												   
													hkbGeneratorSyncInfo* parentSyncInfo );

	protected:
			/// Find and initialize sync points we're going to synchronize the clips to.
		static hkbGeneratorSyncInfo* HK_CALL findAndInitializeSyncData( hkbGeneratorSyncInfo* parentSyncInfo, int numActiveSyncInfos, hkLocalArray<hkReal>& allBlendWeights, 
													hkLocalArray<hkbGeneratorSyncInfo*>& allSyncInfos, hkLocalBuffer<hkReal>& durationsToNextSyncPoint, hkLocalArray<int>& commonSyncPointIds, 
													hkReal& outSumOfWeights );

			/// Updates clips playback speeds based on the computed synchronization parameters.
		static void HK_CALL updatePlaybackSpeeds( hkLocalArray<hkbGeneratorSyncInfo*>& allSyncInfos, hkbGeneratorSyncInfo* masterSyncInfo, hkbGeneratorSyncInfo* parentSyncInfo, 
													hkLocalBuffer<hkReal>& durationsToNextSyncPoint, hkReal desiredDurationToNextSyncPoint, const hkLocalArray<int>& commonSyncPointIds );
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
