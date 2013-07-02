/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GENERATOR_OUTPUT_UTILS_H
#define HKB_GENERATOR_OUTPUT_UTILS_H

#include <Behavior/Behavior/Generator/hkbGeneratorOutput.h>
#include <Behavior/Behavior/Generator/hkbGeneratorPartitionInfo.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>

	/// Helper functions for dealing with hkbGeneratorOutput and auxiliary track data.
class hkbGeneratorOutputUtils
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, hkbGeneratorOutputUtils );

			/// Replicate the track structure in outputFrom to outputTo, using the given buffer memory.
			/// The number of hkReals pointed to by buffer should have been computed by calling computeTrackBufferSize.
		static void HK_CALL replicateTrackStructure( const hkbGeneratorOutput& from, hkbGeneratorOutput& to );

			/// Copy only valid track data in outputFrom to outputTo, optionally you can ignore coping pose track.
		static void HK_CALL copyTrackData( const hkbGeneratorOutput& from, hkbGeneratorOutput& to, bool copyPoseTrack=true );

			/// Set all of the track data to zero.
		static void HK_CALL zeroTracks( hkbGeneratorOutput& output );

			/// Blend the track data.
		static void HK_CALL blendInTracks(	const hkbGeneratorOutput& outputIn, 
											hkReal weight, 
											hkbGeneratorOutput& finalOutput,
											hkReal* trackWeightSums,
											hkReal* sparseTrackWeights );

			/// Overlay a palette track onto another one.  If the data overlaps, the data from trackIn is used.
		static void HK_CALL overlayPaletteTrack(	const hkbGeneratorOutput::ConstTrack& trackIn,
													hkbGeneratorOutput::Track& trackInOut );

			/// Overlay a sparse track onto another one.  If the data overlaps, the data from trackIn is used.
		static void HK_CALL overlaySparseTrack(	const hkbGeneratorOutput::ConstTrack& trackIn,
												hkbGeneratorOutput::Track& trackInOut );

			/// Scale the track data (same weight for all tracks).
		static void HK_CALL scaleTracks( hkReal scalar, hkbGeneratorOutput& output );

			/// Scale the m_onFraction member of each track.
		static void HK_CALL scaleTracksOnFraction( hkReal scalar, hkbGeneratorOutput& output );

			/// Divide the track data by sumOfWeights.
		static void HK_CALL normalizeTracks(	hkReal* trackWeightSums,
												hkReal* sparseTrackWeights,
												hkbGeneratorOutput& output );

			/// Normalize the m_onFraction member of each track.
		static void HK_CALL normalizeTracksOnFraction( hkReal weightSum, hkbGeneratorOutput& output );

			/// Copy outputIn into outputOut.
		static void HK_CALL copyGeneratorOutput( const hkbGeneratorOutput& from, hkbGeneratorOutput& to );

			// Copies the track data from a generatorOutputIn to the rootGeneratorOutput.
		static void HK_CALL copyDataToRootGeneratorOutput(	const hkbGeneratorOutput& generatorOutputIn,
															const hkaSkeleton* skeleton,
															hkbGeneratorOutput& rootGeneratorOutput );

			// A desciption of a track used to initialize it.
		struct TrackInfo
		{
				/// The capacity of this track (in number of elements, not bytes).
			hkInt16 m_capacity;

				/// The size of each element in the track.
			hkInt16 m_elementSizeBytes;

				/// The flags for this track.
			hkFlags<hkbGeneratorOutput::TrackFlags,hkInt8> m_flags;

				/// The type of data in the track.
			hkEnum<hkbGeneratorOutput::TrackTypes,hkInt8> m_type;
		};

			/// Initialize an array of TrackInfos based on the number of bones, etc.
			/// You should pass in a value for numTracks that corresponds to the number
			/// of elements that trackInfos points to.
			/// \param numBones The number of bones in the animation skeleton.
			/// \param numRagdollBones The number of bones in the ragdoll skeleton.
			/// \param numAttributes The number of behavior attributes.
			/// \param numHands The number of hands/arms your character has.
			/// \param numFloatSlots The number of float slots that your animation skeleton has.
			/// \param numTracks The number of tracks desired (if you want less than the full set of standard tracks).
			/// \param trackInfos A buffer of TrackInfos with numTracks elements.
		static void HK_CALL initTrackInfos(	int numBones,
											int numRagdollBones, 
											int numAttributes, 
											int numHands,
											int numFloatSlots,
											int numTracks,
											TrackInfo* trackInfos );

			/// Computes the number of bytes needed for the given tracks.
		static int HK_CALL computeTrackBufferSizeBytes(	int numTracks,
														TrackInfo* trackInfos );

			/// Adjusts the track buffer size when the number of bones change.
		static int HK_CALL adjustTrackBufferSizeBytes( int trackBufferSizeBytesIn, int numBonesIn, int numBonesOut );

			/// Initializes tracks from trackInfos.
		static void HK_CALL initTracks(	int numBytes,
										int numTracks,
										const TrackInfo* trackInfos,
										hkbGeneratorOutput::Tracks* tracks );

			/// Compute the offset for each track into the flat track buffer.
		static void HK_CALL initOffsets( hkbGeneratorOutput::Tracks* tracks );

			/// Determines whether the output pose with the given boneweights is dense.
			/// Note that the last bone weight in boneWeights is assumed to not be a bone; it
			/// is an extra bit that indicates whether the pose is dense.  This function does not
			/// check that bit but looks at all of the bone weights to see if they are all set.
		static bool HK_CALL isOutputPoseDense( hkReal* boneWeights, int numBones );

			/// Fills in any missing bones (transforms) from poseLocal with bones
			/// from referencePose.  A bone is considered missing if boneWeight 
			/// is 0 for the index of the bone.  If referencePose is HK_NULL,
			/// missing bones will be filled in with the identity transform,
			/// which is useful for filling in sparse additive poses.
		static void HK_CALL fillInMissingBones(	int numPoseLocal,
												hkQsTransform* poseLocal,
												const hkReal* boneWeights,
												const hkQsTransform* referencePose);

			/// Fill in a bone with the reference pose or the identity (if no reference pose is provided) if its bone weight is zero.
		static void HK_CALL fillInMissingBone(	hkbGeneratorOutput& inOut,
												hkInt16 boneIndex,
												const hkQsTransform* referencePose );

			/// Force a dense pose by filling in the missing bones with the reference pose (or identity if 
			/// the reference pose is null).
		static void HK_CALL forceDensePose(	hkbGeneratorOutput& inOut,
											const hkQsTransform* referencePose );

			/// Blend two poses with bone weights.
		static void HK_CALL blendPoses(	const hkaSkeleton* skeleton,
										const hkQsTransform* poseA,
										const hkReal* boneWeightsA,
										const hkbGeneratorPartitionInfo& partitionInfoA,
										const int numPoseLocalA,
										const hkQsTransform* poseB,
										const hkReal* boneWeightsB,
										const hkbGeneratorPartitionInfo& partitionInfoB,
										const int numPoseLocalB,
										hkReal weightB,
										hkQsTransform* poseOut,
										hkReal* boneWeights,
										const hkbGeneratorPartitionInfo& outputPartitionInfo,
										const int numPoseLocal);

			/// Blend the child output pose and bone weights taking into consideration partitions.
		static void HK_CALL blendPose(	const hkaSkeleton* skeleton,
										const hkQsTransform* childPose,
										const hkReal* childBoneWeights,
										const hkbGeneratorPartitionInfo& childPartitionInfo,
										int childNumPoseLocal,
										hkReal weight,
										hkQsTransform* poseOut,
										hkReal* boneWeights,
										const hkbGeneratorPartitionInfo& partitionInfoOut);

			/// Blend the child output pose and bone weights taking into consideration partitions.
		static void HK_CALL blendAdditivePose(	const hkaSkeleton* skeleton,
												const hkQsTransform* childPose,
												const hkReal* childBoneWeights,
												const hkbGeneratorPartitionInfo& childPartitionInfo,
												int childNumPoseLocal,
												hkReal weight,
												hkQsTransform* poseOut,
												const hkbGeneratorPartitionInfo& partitionInfoOut);

			/// Subtract the child output pose and bone weights taking into consideration partitions.
		static void HK_CALL subtractPose(	const hkaSkeleton* skeleton,
											const hkQsTransform* childPose,
											const hkReal* childBoneWeights,
											const hkbGeneratorPartitionInfo& childPartitionInfo,
											int childNumPoseLocal,
											hkReal weight,
											hkQsTransform* poseOut,
											const hkbGeneratorPartitionInfo& partitionInfoOut);

			/// Blend two outputs, taking into consideration sparse poses.  If a bone is missing
			/// from both poses it is omitted from the output.  If a bone is present in only one
			/// pose it is copied to the output.  If a bone is present in both poses it is blended.
		static void HK_CALL blendOutputs(	const hkaSkeleton* skeleton,
											const hkbGeneratorOutput& outputA, 
											const hkbGeneratorOutput& outputB,
											hkReal weightB, 
											hkbGeneratorOutput& output );

			/// For internal use.  Sparse and palette tracks require per-element weights during blending.
			/// This method sums the capacity of all such tracks so that we can allocate the weights.
		static int computeNumSparseTrackWeights( const hkbGeneratorOutput& output );
			

#ifdef HK_DEBUG
			/// Checks whether a pose has any garbage bones and optionally whether it's dense.  This is for debugging only.
		static void checkPose( const hkbGeneratorOutput& output, bool mustBeDense=false );
#endif

			/// Compute the size of bone weights buffer.
			/// The buffer size is in bytes and is a multiple of 16
		static int HK_CALL getSizeOfBoneWeightsBuffer( int numBones );

			/// Compute the size of partition info buffer
			/// The buffer size is in bytes and is a multiple of 16
		static int HK_CALL getSizeOfPartitionInfoBuffer();

			/// Determine if the output partitions contain pose data between leafwardBoneIdx and
			/// rootwardBoneIdx.
		static bool HK_CALL isBoneWithinRange(	const hkbGeneratorOutput& output,
												const hkaSkeleton& skeleton,
												hkInt16 leafwardBoneIdx,
												hkInt16 rootwardBoneIdx = 0);

			/// Determine if the bone is in the partition or Lod pose.
		static bool  HK_CALL hasBoneIndex(	const hkbGeneratorPartitionInfo& partitionInfo,
											int numPoseLocal, hkInt16 boneIdx );

	private:

			/// Blend a palette track into another one.
		static void HK_CALL blendInPaletteTrack(	const hkbGeneratorOutput::ConstTrack& trackIn,
													hkReal weight,
													hkbGeneratorOutput::Track trackInOut,
													hkReal* sparseTrackWeights,
													bool overlay=false );

			/// Blend a sparse track into another one.
		static void HK_CALL blendInSparseTrack(	const hkbGeneratorOutput::ConstTrack& trackIn,
												hkReal weight,
												hkbGeneratorOutput::Track trackInOut,
												hkReal* sparseTrackWeights,
												bool overlay=false );

			/// Blend the child output pose and bone weights taking into consideration partitions.
		static void HK_CALL blendOutput(	const hkaSkeleton* skeleton,
											const hkbGeneratorOutput& childOutput,
											hkReal weight, 
											hkbGeneratorOutput& output );

	private:

			// Track descriptions for the default tracks.
		static TrackInfo m_defaultTrackInfos[hkbGeneratorOutput::NUM_STANDARD_TRACKS];
};

#include <Behavior/Behavior/Generator/hkbGeneratorOutputUtils.inl>

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
