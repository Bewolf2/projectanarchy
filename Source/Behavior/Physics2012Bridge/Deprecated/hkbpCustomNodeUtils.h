/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_CUSTOM_NODE_UTILS_H
#define HKBP_CUSTOM_NODE_UTILS_H

class hkaRagdollInstance;
class hkaSkeletonMapper;
class hkbGenerator;
class hkbContext;
class hkaDefaultAnimationControl;

/// Utility functions used by custom nodes.
class hkbpCustomNodeUtils
{
public:
	/// Compute the velocity of the center-of-mass of an animation generator at a specific time.
	/// The deltaTimeForFiniteDifference parameter determines the finite-difference granularity.
	static void HK_CALL computeComVelocity(	hkbGenerator* generator,
		hkbContext& context, 
		const hkReal time, 
		const hkReal deltaTimeForFiniteDifference,
		hkVector4& comVelocity );

	/// Compute the radial velocity of the center-of-mass of an animation generator.  Decomposes the
	/// velocity into polar form, after projecting onto an x-y plane that can be any of the 
	/// standard coordinate planes.
	static void HK_CALL computeRadialComVelocity(	hkbGenerator* generator, 
		hkbContext& context,
		const hkReal time,
		const hkReal deltaTimeForFiniteDifference,
		const int xAxisMS,
		const int yAxisMS,
		hkReal& angle,
		hkReal& radialSpeed );

	/// Compute the center-of-mass of a ragdoll bone given the pose.  Returns the mass.
	static void HK_CALL computeRagdollBoneCenterOfMassFromModelPose(	int boneIndex,
		const hkaRagdollInstance& ragdoll,
		const hkQsTransform* modelPose, 
		const hkQsTransform& worldFromModel, 
		hkVector4& com );

	/// Computes the center-of-mass of the ragdoll in the given model-space pose.  Returns the mass.
	static hkReal HK_CALL computeRagdollCenterOfMassFromModelPose(	const hkaRagdollInstance& ragdoll,
		const hkQsTransform* modelPose, 
		const hkQsTransform& worldFromModel, 
		hkVector4& com );

	/// Computes the center-of-mass of a ragdoll subtree in the given model-space pose.  Returns the mass.
	static hkReal HK_CALL computeRagdollSubtreeCenterOfMassFromModelPose(	int subtreeRootBoneIndex,
		const hkaRagdollInstance& ragdoll, 
		const hkQsTransform* modelPose, 
		const hkQsTransform& worldFromModel,
		hkVector4& com );

	/// Computes the center-of-mass of a bone (rigid body) in the given model-space pose.  The mass of this bone
	/// is added to massAccumulator.  The COM of this bone is multiplied by its mass and then added to comAccumulator.
	/// This function is useful for computing the center-of-mass of a set of bones, or an entire ragdoll.
	static void HK_CALL accumulateRagdollBoneCenterOfMassFromModelPose(	int boneIndex,
		const hkaRagdollInstance& ragdoll,
		const hkQsTransform* modelPose,
		const hkQsTransform& worldFromModel,
		hkVector4& comAccumulator,
		hkReal& massAccumulator );

	/// Compute the goal center-of-mass, which lies along the line between the feet 
	/// rigid body midpoint and the ankle midpoint.
	static void HK_CALL computeBalancedCom(	const hkaRagdollInstance* ragdoll,
		const hkQsTransform* ragdollPoseMS,
		const hkQsTransform& worldFromModel,
		int leftFootBoneIndex,
		int rightFootBoneIndex,
		hkReal balanceOnAnklesFraction,
		hkVector4& goalCom );

	/// Return the sum of the speeds (linear and angular) of the ragdoll rigid bodies.
	static hkReal HK_CALL getRagdollRigidBodySpeedSum( hkaRagdollInstance& ragdoll );

	/// Map an animation pose in local space to a ragdoll pose in model space.
	static void HK_CALL mapAnimationPoseLocalToRagdollPoseModel(	const hkQsTransform* animPoseLocal, 
		const hkaSkeletonMapper* animToRagdollMapper,
		hkQsTransform* ragdollPoseModel );
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
