/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_RAGDOLL_UTILS_H
#define HKB_RAGDOLL_UTILS_H

#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>
#include <Behavior/Behavior/Context/hkbContext.h>
#include <Behavior/Behavior/Character/hkbCharacter.h>
#include <Behavior/Utilities/Physics/Interface/hkbRagdollInterface.h>

class hkbCharacterSetup;
class hkaSkeletonMapper;
class hkaSkeleton;

	/// Utility functions having to do with ragdolls.
	/// Note all locking is the responsibility of the caller.
class hkbRagdollUtils
{
	public:

			/// Warn if the context doesn't contain a ragdoll interface.
		static hkbRagdollInterface* HK_CALL warnRagdollInterface( const hkbContext& context );

			/// Warn if the character doesn't contain a ragdoll interface.
		static hkbRagdollInterface* HK_CALL warnRagdollInterface( const hkbCharacter* character );

			/// Warn if the ragdoll interface is null.
		static hkbRagdollInterface* HK_CALL warnRagdollInterface( const hkbRagdollInterface* ragdollInterface );
	
			/// Assert if the context doesn't contain a ragdoll interface.
		static hkbRagdollInterface* HK_CALL requireRagdollInterface( const hkbContext& context );

			/// Assert if the character doesn't contain a ragdoll interface.
		static hkbRagdollInterface* HK_CALL requireRagdollInterface( const hkbCharacter* character );

			/// Assert if the ragdoll interface is null.
		static hkbRagdollInterface* HK_CALL requireRagdollInterface( const hkbRagdollInterface* ragdollInterface );

			/// Debug check that buffer size is greater than num of ragdoll bones.
		static void HK_CALL requireCorrectBufferSize(
			hkbRagdollInterface* ragdollInterface,
			int bufferSize );
		
			/// Set the character property on all the rigid bodies in the ragdoll.
		static void HK_CALL addCharacterPropertyToRagdollRigidBodies(
			hkbPhysicsInterface* physicsInterface,
			hkbCharacter* character,
			hkUint16 propertyKey,
			int* rigidBodyIndices = HK_NULL,
			int numRigidBodyIndices = 0 );

			/// Scales the animation skeleton. If the ragdoll exists then it scales 
			/// the ragdoll along with mappers and ragdoll skeleton.
		static void HK_CALL scaleCharacter(
			hkbCharacterSetup& setup,
			hkbRagdollInterface* ragdollInterface,
			hkaSkeleton* ragdollSkeleton,
			hkReal oldScaleFactor,
			hkReal newScaleFactor );

			/// Set the characters animation skeleton to the clone of the incoming animation skeleton.
		static void HK_CALL setAnimationSkeletonClone( hkbCharacter* character, hkaSkeleton* animationSkeleton );

			/// Given a ragdoll find all the rigid bodies with collision info that collides with the input collision info.
		static void HK_CALL getRigidBodiesInRagdoll(
			hkbPhysicsInterface* physicsInterface,
			const hkbRagdollInterface* ragdollInterface,
			hkUint32 collisionFilterInfo,
			hkArray<hkbRigidBodyHandle>& rigidBodies,
			bool onlyIncludeBodiesInWorld = true );

			/// Get the modelspace pose of the ragdoll.
		static void HK_CALL getPoseModelSpace(
			hkbRagdollInterface* ragdollInterface,
			const hkQsTransform& worldFromModel,
			hkQsTransform* poseModelSpaceOut,
			int poseOutSize );

			/// Set the modelspace pose of the ragdoll (this will zero out rigid body velocities).
		static void HK_CALL setPoseModelSpace(
			hkbRagdollInterface* ragdollInterface,
			const hkQsTransform& worldFromModel,
			hkQsTransform* poseModelSpaceIn,
			int poseInSize );

			/// Set the modelspace pose of the ragdoll and update velocities accordingly.
		static void HK_CALL setPoseAndVelocitiesModelSpace(
			hkbPhysicsInterface* physicsInterface,
			hkbRagdollInterface* ragdollInterface,
			const hkQsTransform* poseModelSpaceA,
			int poseModelSpaceASize,
			const hkQsTransform& worldFromModelA,
			const hkQsTransform* poseModelSpaceB,
			int poseModelSpaceBSize,
			const hkQsTransform& worldFromModelB,
			hkReal timestep );

			/// Get the current world space pose. This will be approximate if the physics world is being asynchronously stepped.
		static void HK_CALL getApproxCurrentPoseWorldSpace(
			hkbPhysicsInterface* physicsInterface,
			hkbRagdollInterface* ragdollInterface,
			hkQsTransform* poseWorldSpaceOut,
			int poseOutSize );

			/// Drive the hkbRagdollController to the provided pose.
		static void HK_CALL driveControllerToPose(
			hkbPhysicsInterface* physicsInterface,
			struct hkbRagdollControllerSetup& setup,
			struct hkbRagdollControllerInstanceSetup& instanceSetup,
			struct hkbRagdollControllerData& data,
			hkBool createController,
			hkReal* currentWeights,
			int currentWeightsSize,
			hkReal deltaTime,
			const hkQsTransform* poseLocal,
			int poseLocalSize,
			const hkQsTransform& worldFromModel,
			hkbRagdollController*& persistentControllerInOut );

		//////////////////////////////////////////////////////////////////////////
		// Deprecated Physics Implementation Specific Functions
		// Must link against hkbPhysics2012Bridge
		//////////////////////////////////////////////////////////////////////////

			/// Clone the motors of a ragdoll.  This is useful if the tool chain used the same motor instance for all
			/// powered constraints, but you want them all to be independent.
		static void HK_CALL cloneRagdollMotors( hkbDeprecatedRagdollPtr ragdoll );

			/// Set the rigid bodies of a ragdoll to use sphere inertias, scale the inertias to reduce rotational accelerations,
			/// and optimize the constraint tree.
		static void HK_CALL optimizeRagdollInertia( hkbDeprecatedRagdollPtr ragdoll );

			/// Set the constraint limits of a constraint very liberally.
		static void HK_CALL relaxConstraintLimits( hkbDeprecatedConstraintDataPtr constraintData );

			/// Set all of the ragdoll constraint limits very liberally.
		static void HK_CALL relaxConstraintLimits( hkbDeprecatedRagdollPtr ragdoll );

			/// use hkaRagdollUtils::setCollisionLayer instead
		// static void HK_CALL initCollisionFilterInfo( hkbDeprecatedRagdollPtr ragdoll, int layer, int systemGroup );

			/// Set a sensing property in the rigid bodies of the ragdoll.
		static void HK_CALL addCharacterPropertyToRagdollRigidBodies(
			hkbCharacter* character,
			const hkUint32 propertyKey,
			int* rigidBodyIndices = HK_NULL,
			int numRigidBodyIndices = 0 );

			/// Enable or disable cone limit stabilization in the ragdoll constraints.
		static void HK_CALL setConeLimitStabilization( hkbDeprecatedRagdollPtr ragdoll, bool enable );

			/// Scales the animation skeleton. If the ragdoll exists then it scales 
			/// the ragdoll instance along with mappers and ragdoll skeleton.
		static void HK_CALL scaleCharacter(	hkbCharacterSetup& setup,
			hkbDeprecatedRagdollPtr ragdoll,
			hkaSkeleton* ragdollSkeleton,
			hkReal oldScaleFactor, 
			hkReal newScaleFactor );

	protected:

			/// for internal use only
		static void HK_CALL getApproxCurrentPoseWorldSpaceInternal(
			hkbPhysicsInterface* physicsInterface,
			hkbRagdollInterface* ragdollInterface,
			int i,
			hkQsTransform& boneWorldSpaceOut );


};

#include <Behavior/Utilities/Physics/hkbRagdollUtils.inl>

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
