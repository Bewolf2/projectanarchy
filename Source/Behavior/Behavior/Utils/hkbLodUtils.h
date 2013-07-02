/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_LOD_UTILS_H
#define HKB_LOD_UTILS_H

#include <Behavior/Behavior/Generator/hkbGeneratorPartitionInfo.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>

class hkaSkeleton;
class hkaPose;
class hkaSkeletonMapper;
class hkbGeneratorOutput;

class hkbLodUtils
{
	public:
			/// Compute a complete model pose from a partial local pose.  If the local pose does not
			/// contain all of the bones (due to LOD or sparsity), the missing ones are taken from the 
			/// reference pose of the skeleton.  If boneWeights is null the input is assumed to be dense.
		static void HK_CALL computeFullPoseModelFromPartialPoseLocal(	const hkQsTransform* poseLocal, 
																		const int numPoseLocal,
																		const hkaSkeleton* skeleton,
																		const hkbGeneratorPartitionInfo& partitionInfo,
																		const hkReal* boneWeights,
																		hkQsTransform* poseModelOut );

			/// Compute a complete model pose from a partial local pose.  If the local pose does not
			/// contain all of the bones (due to LOD or sparsity), the missing ones are taken from the 
			/// reference pose of the skeleton.
		static void HK_CALL computeFullPoseModelFromPartialPoseLocal(	const hkbGeneratorOutput& output,
																		const hkaSkeleton* skeleton,
																		hkQsTransform* poseModelOut );

		/// Initialize a pose buffer from raw local pose data.  If the pose provided is
		/// incomplete (due to LOD or sparsity), the missing bones are taken from the reference pose.
		/// If boneWeights is null the input is assumed to be dense.
		static void HK_CALL initPoseFromPartialPoseLocal(	const hkbGeneratorOutput& outputIn,
															const hkaSkeleton* skeleton,
															hkbGeneratorOutput& outputOut);

		/// Initialize a pose buffer from raw local pose data.  If the pose provided is
		/// incomplete (due to LOD or sparsity), the missing bones are taken from the reference pose.
		/// If boneWeights is null the input is assumed to be dense.
		static void HK_CALL initPoseFromPartialPoseLocal(	const hkbGeneratorOutput& outputIn,
															const hkaSkeleton* skeleton,
															const hkbGeneratorPartitionInfo& partitionInfo,
															hkQsTransform* poseLocalOut,
															int numPoseLocalOut);

		/// Initialize a pose buffer from raw local pose data.  If the pose provided is
		/// incomplete (due to LOD or sparsity), the missing bones are taken from the reference pose.
		/// If boneWeights is null the input is assumed to be dense.
		static void HK_CALL initPoseFromPartialPoseLocal(	const hkQsTransform* poseLocalIn, 
															int numPoseLocal,
															const hkReal* boneWeightsIn,
															const hkaSkeleton* skeleton,
															const hkbGeneratorPartitionInfo& partitionInfo,
															hkQsTransform* poseLocalOut,
															int numPoseLocalOut);

		
		/// Initialize a pose buffer from raw local pose data.  If the pose provided is
		/// incomplete (due to LOD or sparsity), the missing bones are taken from the reference pose.
		/// If boneWeights is null the input is assumed to be dense.
		static void HK_CALL initPoseFromPartialPoseLocal(	const hkQsTransform* poseLocalIn, 
															int numPoseLocal,
															const hkReal* boneWeightsIn,
															const hkaSkeleton* skeleton,
															const hkbGeneratorPartitionInfo& partitionInfo,
															hkQsTransform* poseLocalOut,
															int numPoseLocalOut,
															hkReal* boneWeightsOut,
															bool fillInMissingBones = true);

			/// Map a ragdoll pose to another skeleton, using a partial local pose as input.
			/// The bones missing (due to LOD or sparsity) will be taken
			/// from the reference pose.
		static void HK_CALL mapRagdollPoseUsingPartialLocalPose(	const hkQsTransform* ragdollPoseModel,
																	const hkbGeneratorOutput& output,
																	const hkaSkeletonMapper* mapper,
																	hkQsTransform* fullPoseModelInOut );

			/// Compute a model bone transform from a partial local pose.
			/// The bones missing (due to LOD or sparsity) will be taken
			/// from the reference pose.
		static void HK_CALL transformLocalBoneToModelBone(	int boneIndex, 
															const hkbGeneratorOutput& output,
															const hkaSkeleton* skeleton,
															hkQsTransform& boneModelOut,
															bool ignoreBoneWeights = false );

			/// Fill in the weights array with 1.0f
		static void HK_CALL fillWeights(int numWeightsBones, hkReal* weightsInOut);

	private:

			/// This function will return either the local or reference transform depending on partition and LOD
			/// information.
		static const hkQsTransform& HK_CALL getLocalOrReferenceTransform(
			hkInt16 boneIndex,
			const hkQsTransform* poseLocal,
			const int numPoseLocal,
			const hkReal* boneWeights,
			const hkaSkeleton* skeleton,
			const hkbGeneratorPartitionInfo& partitionInfo,
			bool ignoreBoneWeights );

			/// When using partitions it is necessary to clear out the boneweights  since they are going to be filled in sparsely
			/// The pose will get filled with reference pose, so we don't need to clear it
		static void HK_CALL clearWeights(int numWeightsBones, hkReal* weightsInOut);
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
