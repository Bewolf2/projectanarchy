/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CLIP_GENERATOR_UTILS_H
#define HKB_CLIP_GENERATOR_UTILS_H

class hkaAnimationBinding;
class hkaMirroredSkeleton;
class hkbClipGenerator;
class hkbGeneratorOutput;
class hkaDefaultAnimationControlMapperData;

#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Behavior/Behavior/Generator/hkbGeneratorPartitionInfo.h>

#if defined(HK_PLATFORM_SPU)
#include <Animation/Animation/Animation/hkaAnimation.h>
#endif

class hkbClipGeneratorUtils
{
	public:
		/// Map the given transform tracks to the pose.
		static void HK_CALL convertTrackPoseToBonePose(	int numTransformTracks,
														hkQsTransform* transformTracks,
														hkbGeneratorOutput& output,
														const hkArray< hkInt16 >& transformTrackToBoneIndices,
														const hkInt16* partitionIndices,
														const hkInt16 numPartitionsIndices,
														const hkaDefaultAnimationControlMapperData* mapper,
														const hkQsTransform* referenceTransforms,
														bool isAdditive,
														const hkaSkeleton* skeleton,
														const hkaMirroredSkeleton* mirroredSkeleton,
														const hkbGeneratorPartitionInfo& mirroredPartitionInfo,
														bool mirrorBoneTransforms );

			/// Map the given float tracks with weights to slots.
		static void HK_CALL generateWeightedFloatSlots(	int numFloatTracks,
														hkReal* floatTracks,
														int numFloatSlots,
														const hkaAnimationBinding& binding,
														hkReal weight,
														hkReal* floatSlotsOut );

			/// Blend the given float tracks into the slots.
		static void HK_CALL blendInFloatSlots(	int numFloatTracks,
												hkReal* floatTracks,
												int numFloatSlots,
												const hkaAnimationBinding& binding,
												hkReal weight,
												hkReal* floatSlotsOut );

			/// Initialize float slots
		static void HK_CALL initializeFloatSlots(	hkbGeneratorOutput& output,
													int numReferenceFloats,
													const hkReal* referenceFloats,
													int& numFloatSlotsOut, 
													hkReal*& floatSlotsOut );

			/// The code to perform hkbClipGenerator::generate().
		static void HK_CALL generateInternal(	const hkbClipGenerator& clip,
												const hkaAnimationBinding& binding,
												hkReal timeOffset,
												const hkQsTransform& oldWorldFromModel,
												const hkaMirroredSkeleton* mirroredSkeleton,
												hkReal characterScale,
												const hkaSkeleton* animationSkeleton,
												int numTransformTracks,
												int numFloatTracks,
												bool isAdditive,
												hkbGeneratorOutput& output );


			/// Decompress an animation for one of the echos (or the main animation if echoIndex == numEchos).
		static void HK_CALL decompressAnimation(	const hkbClipGenerator& clip,
													int echoIndex,    // ignored on CPU
													hkReal localTime, // ignored on SPU
													const hkQsTransform* referencePose,
													const hkReal* referenceFloats,
													int numTransformTracks,
													int numFloatTracks,
													hkQsTransform* transformTracks,
													hkReal* floatTracks );
			
			/// Apply the user partitions to the output. Works by setting non-user partition bones weights to 0.0f
		static void HK_CALL applyUserPartitions(const hkbClipGenerator& clip, 
												const hkaSkeleton* animationSkeleton, 
												hkbGeneratorOutput& output);

#if defined(HK_PLATFORM_SPU)
			/// Decompress animation on the SPU.
		static void HK_CALL spuDecompressAnimation(	hkaAnimation::AnimationType animationType, 
													hkUint32 frame, hkReal delta,
													const hkQsTransform* referenceBones, const hkReal* referenceFloats,
													int maxTransformTrack, hkQsTransform* transformOutput, 
													int maxFloatTrack, hkReal* floatOutput, 
													const hkaAnimation::DataChunk* chunks, int numChunks);
#endif
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
