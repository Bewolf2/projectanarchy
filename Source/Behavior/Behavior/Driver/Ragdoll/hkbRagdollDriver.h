/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_RAGDOLL_DRIVER_H
#define HKB_RAGDOLL_DRIVER_H

extern const class hkClass hkbRagdollDriverClass;

class hkbCharacter;
class hkbContext;
class hkbGeneratorOutput;

#include <Behavior/Behavior/Modifier/Ragdoll/hkbPoweredRagdollControlData.h>
#include <Behavior/Behavior/Modifier/Ragdoll/hkbWorldFromModelModeData.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

#include <Common/Base/Container/BitField/hkBitField.h>

	/// Manages the ragdoll simulation associated with a behavior character.
class hkbRagdollDriver : public hkReferencedObject
{
	public:		
	
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );		
		
			/// Initializes the ragdoll controller.  
		hkbRagdollDriver( hkbCharacter* owner, bool autoAddRemoveRagdoll = true );

			/// Destructor
		~hkbRagdollDriver();
		
		/// Returns if the controller is enabled.
		bool isEnabled() const;

			/// Sets if the controller is enabled.
		void setEnabled( bool enabled );
			
			/// Returns true if the powered ragdoll controller is enabled, false otherwise.
		bool isPoweredControllerEnabled();
		
			/// Returns true if the rigid body ragdoll controller is enabled, false otherwise.
		bool isRigidBodyControllerEnabled();
		
			/// Returns whether either ragdoll controller is active.
		bool isRagdollActive();

			/// Set the handle for the character's ragdoll.  This will update the hkbRagdollController.
		void setRagdollInterface(hkbRagdollInterface* ragdoll);

			/// Returns the ragdoll handle from the hkbRagdollController.
		hkbRagdollInterface* getRagdollInterface() const;

			/// Gets the current ragdoll pose in world space.
		const hkQsTransform* getRagdollPoseWS();

			/// Set the ragdoll pose to match the given high res pose in world space.
		void setRagdollPoseFromHighResLocal( const hkQsTransform* highResPoseLocal, int numTransforms );

			/// Map the ragdoll pose to an animation pose and optionally compute the world-from-model transform.
		void extractRagdollPose(	const hkbContext& context,
									hkbGeneratorOutput& inOut,
									bool computeWorldFromModel = false,
									int poseMatchingBone0 = -1,
									int poseMatchingBone1 = -1,
									int poseMatchingBone2 = -1 );

			/// This must be called before calling hkbBehaviorGraph::generate() for an associated character.
		void preGenerate( const hkbContext& context );

			/// This must be called after processing all hkbRagdollModifiers to drive the ragdoll.
		void driveToPose( hkReal deltaTime, const hkbContext& context, hkbGeneratorOutput& generatorOutput );

			/// This must be called after stepping the physics.  This method just extracts the ragdoll
			/// pose into a local buffer.  If you are allowing a 1-frame latency in your
			/// ragdoll and stepping the physics yourself outside of Behavior, you should call postPhysics()
			/// immediately after driveToPose().  That will store the most recent ragdoll pose from the
			/// last frame.  If you do not want any ragdoll latency you should call driveToPose(), then
			/// step the physics, and then call postPhysics() to extract the up-to-date ragdoll pose.
		void postPhysics( const hkbContext& context, hkbGeneratorOutput& inOut );

			/// Reset the ragdoll controller to its initial state.
		void reset();
	
			/// If you want to toggle the quality of the keyframed rigid bodies between HK_COLLIDABLE_QUALITY_KEYFRAMED
			/// and HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING then call this function with the relevent bone index.
			/// Passing in true sets the quality to HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING and false to HK_COLLIDABLE_QUALITY_KEYFRAMED.
			/// Make sure to call this function after setRagdoll.
		void setReportingWhenKeyframed( int ragdollBoneIndex, bool reportWhenKeyframed );

			/// The ragdoll driver stores a collection of ragdoll rigid body bones that are being attached using the attachment system.
			/// This is called when the attachment modifier makes an attachment that involves a ragdoll rigid body. 
			/// For internal use only.
		void addAttachedRigidBody( hkbRigidBodyHandle rb );

			/// When the attachment is removed we remove the rigid body from the collection of attached rigid bodies.
			/// For internal use only.
		void removeAttachedRigidBody( hkbRigidBodyHandle rb );

		//////////////////////////////////////////////////////////////////////////
		// Deprecated Physics Implementation Specific Functions
		// Must link against hkbPhysics2012Bridge
		//////////////////////////////////////////////////////////////////////////

			/// Set the handle for the character from a ragdoll pointer.
			/// This accessor will be deprecated in the future (see setRagdollInterface).
		void setRagdoll(hkbDeprecatedRagdollPtr ragdoll);

			/// Get the handle for the character as a ragdoll pointer.
			/// This accessor will be deprecated in the future (see getRagdollInterface).
		hkbDeprecatedRagdollPtr getRagdoll() const;

	public:

			/// DEPRECATED (HKF-1433). Use hkbBlendingTransitionEffect instead.
			/// For how long to blend the ragdoll pose with the incoming pose after
			/// the ragdoll has been deactivated.  If this is 0 then there will be no
			/// blending so there will likely be a glitch when ending ragdoll.
		hkReal m_ragdollBlendOutTime;

			/// How to treat the world-from-model when using the powered ragdoll controller.
		struct hkbWorldFromModelModeData m_worldFromModelModeData;

			/// If true the ragdoll being controlled by this controller is added to the physics world on the first call
			/// to preGenerate().  If false, the user is responsible for adding the ragdoll to the world.
			/// If the ragdoll is not added to the world the controller is disabled.
		hkBool m_autoAddRemoveRagdollToWorld;

			/// Set this to true if you are stepping the physics world asynchronously.
		hkBool m_useAsynchronousStepping;

	protected:
			
			/// Maps a high res pose, in local space, to a low res pose, in local space, using the character's skeleton mapper
		void mapHighResPoseLocalToLowResPoseLocal( const hkQsTransform* highResPoseLocal, int numBonesHighResPose, hkQsTransform* lowResPoseLocal, int numBonesLowResPose ) const;

			/// Maps a high res pose, in local space, to a low res pose, in world space, using the character's skeleton mapper
		void mapHighResPoseLocalToLowResPoseWorld( const hkQsTransform* highResPoseLocal, const hkQsTransform& worldFromModel, hkQsTransform* lowResPoseWorld ) const;
		
			/// Sets the ragdoll pose to the pose in generatorOutput.  If m_lastPoseLocal is not null, it
			/// will be used to set the velocities of the ragdoll bones (transferring momentum from the animation
			/// to the ragdoll).  Otherwise, the ragdoll bones will have zero velocity.
		void initRagdollPoseAndVelocity( const hkbContext& context, const hkbGeneratorOutput& generatorOutput, hkReal timestep );
		
			// DEPRECATED (HKF-1433). Use hkbBlendingTransitionEffect instead.
			// Sets m_ragdollPoseHighResLocal from m_ragdollPoseWS.
		void saveRagdollPoseHiResLocal( const hkbContext& context, hkbGeneratorOutput& output );

			// Gets the ragdoll pose from the ragdoll and stores it in m_ragdollPoseWS.
		void updateRagdollPoseWS( const hkbContext& context );

			// Throw away the ragdoll pose and memory.
		void discardRagdollPoseWS();

			// Add ragdoll to world
		void addRagdollToWorld( const hkbContext& context, hkbGeneratorOutput& generatorOutput, hkReal deltaTime );

	private:

			/// Map the ragdoll pose to an animation pose, optionally blends the ragdoll pose with the input pose and 
			/// and optionally compute the world-from-model transform.
		void extractRagdollPoseInternal(	const hkbContext& context,
											hkbGeneratorOutput& inOut,
											hkReal rigidBodyRagdollOnFraction,
											hkReal poweredRagdollOnFraction,
											bool blendPoses,
											bool computeWorldFromModel = false,
											int poseMatchingBone0 = -1,
											int poseMatchingBone1 = -1,
											int poseMatchingBone2 = -1 );

	protected:

			// Store the last world-from-model whenever ragdoll is off so that we can transfer
			// momentum to the ragdoll when we add it to the world.
		hkQsTransform m_lastWorldFromModel;

		struct hkbWorldFromModelModeData m_worldFromModelModeDataInternal;

			/// Map to store ragdoll bones that are used by the attachment system. We want to
			/// store all the attached rigid bodies and so the values are all true and are
			/// not being used for anything.
		hkPointerMap<hkbRigidBodyHandle, hkBool32> m_attachedRigidBodyToIndexMap;

			/// Character this controller is associated with.
		hkbCharacter* m_character;

			/// Ragdoll controlled by the controller.
		hkbRagdollInterface* m_ragdollInterface;

			/// The current ragdoll pose in world space.
		hkQsTransform* m_ragdollPoseWS;

			/// powered ragdoll controller.
		hkbRagdollController* m_ragdollPoweredController;

			/// rigid body ragdoll controller.
		hkbRagdollController* m_ragdollRigidBodyController;

			// DEPRECATED (HKF-1433). Use hkbBlendingTransitionEffect instead.
			// when transitioning from ragdoll to no ragdoll we save the ragdoll pose to blend smoothly
		hkQsTransform* m_ragdollPoseHiResLocal;

			// Store the last pose whenever ragdoll is off so that we can transfer
			// momentum to the ragdoll when we add it to the world.
		hkQsTransform* m_lastPoseLocal;

			// Store the last pose size whenever ragdoll is off so that we can transfer
			// momentum to the ragdoll when we add it to the world.
		hkInt32 m_lastNumPoseLocal;

			// Store the last frame rigid body ragdoll controller on fraction to determine 
			// whether we are going from or to rigid body ragdoll controller state.
		hkReal m_lastFrameRigidBodyOnFraction;

			// Store the last frame powered ragdoll controller on fraction to determine 
			// whether we are going from or to powered ragdoll controller state.
		hkReal m_lastFramePoweredOnFraction;

			/// DEPRECATED (HKF-1433). Use hkbBlendingTransitionEffect instead.
			/// Seconds elapsed since the rigid body controller was activated.
		hkReal m_timeRigidBodyControllerActive;

			/// DEPRECATED (HKF-1433). Use hkbBlendingTransitionEffect instead.
			/// how long we have been blending out the ragdoll pose
		hkReal m_ragdollBlendOutTimeElapsed;

			/// Flag that is set right after ragdoll is added to the world.  On the next call to ::postGenerate the ragdoll's
			/// pose will be set to the results of the behavior graph.  Also, if this flag is true, then the pose sampled on
			/// the first call to preGenerate() will be the ragdoll's reference pose.  This is required because, in HAT, states
			/// get entered and exited as users built the graph.  This will always reset the pose being edited in HAT to the output
			/// of the graph.
		hkBool m_firstGenerateAfterRagdollIsAddedToWorld;

			/// This is set for first frame we become active. It indicates that we must create the controller.
		hkBool m_firstGenerateAfterRigidBodyControllerActive;

			/// This is set for first frame we become active. It indicates that we must create the controller.
		hkBool m_firstGenerateAfterPoweredControllerActive;

			/// Is the controller enabled.  If false the ragdoll for the instance will not be set.  
			/// This effectively disables the controller.
		hkBool m_isEnabled;

			/// Whether the powered ragdoll controller is enabled.
		hkBool m_isPoweredControllerEnabled;

			/// Whether the ragdoll rigid body controller is enabled.
		hkBool m_isRigidBodyControllerEnabled;

			/// Whether the ragdoll pose was read this frame.
		hkBool m_ragdollPoseWasUsed;

			/// This is set each frame in driveToPose and then used when extracting the pose.
		hkBool m_allBonesKeyframed;

			/// Information about whether each bone is keyframed and what the target is (if any).
		struct KeyframeInfo
		{
			hkVector4 m_targetPosition;
			hkQuaternion m_targetRotation;
			hkBool m_isKeyframed;
			hkBool m_isTargetValid;
		};
};

#include <Behavior/Behavior/Driver/Ragdoll/hkbRagdollDriver.inl>

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
