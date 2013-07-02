/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_TARGET_RIGID_BODY_MODIFIER_H
#define HKBP_TARGET_RIGID_BODY_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Physics2012Bridge/Deprecated/Modifier/Target/hkbpTarget.h>

extern const class hkClass hkbpTargetRigidBodyModifierClass;

class hkpRigidBody;
class hkbpTarget;

/// An example custom modifier that chooses a rigid body as a target.
class hkbpTargetRigidBodyModifier : public hkbModifier
{
	public:
		// +version(4)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpTargetRigidBodyModifier();

	private:

		hkbpTargetRigidBodyModifier( const hkbpTargetRigidBodyModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		hkBool isActive() const { return m_isActive; }

		enum EventModeTRBAM 
		{
			EVENT_MODE_DO_NOT_SEND = 0,
			EVENT_MODE_SEND_ONCE,
			EVENT_MODE_RESEND,
		};

		enum TargetMode
		{
			TARGET_MODE_CONE_AROUND_CHARACTER_FORWARD,
			TARGET_MODE_CONE_AROUND_LOCAL_AXIS,
			TARGET_MODE_RAYCAST_ALONG_CHARACTER_FORWARD,
			TARGET_MODE_RAYCAST_ALONG_LOCAL_AXIS,
			TARGET_MODE_ANY_DIRECTION,
		};

		enum ComputeTargetAngleMode
		{
			COMPUTE_ANGLE_USING_TARGET_COM,
			COMPUTE_ANGLE_USING_TARGET_CONTACT_POINT,
		};

		enum ComputeTargetDistanceMode
		{
			COMPUTE_DISTANCE_USING_TARGET_POSITION,
			COMPUTE_DISTANCE_USING_TARGET_CONTACT_POINT,
		};

		///////////////
		// properties
		///////////////

			/// The target that was found.
		hkRefPtr<hkbpTarget> m_targetOut;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
											//+hk.Description("The target that was found.")
										
			/// How to do the targeting.
		hkEnum< TargetMode, hkInt8 > m_targetMode; //+hk.Description("How to do the targeting.")

			/// The layer to use for the sensor when detecting a target.
		int m_sensingLayer; //+default(-1)+hk.Description("The layer to use for the sensor when detecting a target.")

			/// Whether to target once or every time modify is called.
		hkBool m_targetOnlyOnce; //+default(true)+hk.Description("Whether to target once or every time modify is called.")

			/// If true, rigid bodies in this character's system group will be ignored.
		hkBool m_ignoreMySystemGroup; //+default(true)+hk.Description("If true, rigid bodies in this character's system group will be ignored.")

			/// The maximum distance to look for a target.
		hkReal m_maxTargetDistance; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The maximum distance to look for a target.")

			/// The maximum height above the sensor the target can be.
		hkReal m_maxTargetHeightAboveSensor; //+default(2.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The maximum height above the sensor the target can be.")

			/// If the target is closer than this distance then it is considered close enough and m_closeToTargetEventId is sent.
		hkReal m_closeToTargetDistanceThreshold; //+default(0.1f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("If the target is closer than this distance then it is considered close enough and m_closeToTargetEventId is sent.")

			/// Mode for computing the target angle
		hkEnum< ComputeTargetAngleMode, hkInt8 > m_targetAngleMode; //+hk.Description("Mode for computing the target angle")

			/// Mode for computing the target distance
		hkEnum< ComputeTargetDistanceMode, hkInt8 > m_targetDistanceMode; //+hk.Description("Mode for computing the target distance")

			/// The culling angle (only targets within +- this angle from the forward direction are considered).
		hkReal m_maxAngleToTarget; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The culling angle. Only targets within +- this angle from the forward direction are considered.")

			/// The index of the ragdoll bone whose coordinates will be used to specify the sensor location.
		hkInt16 m_sensorRagdollBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")+hk.Description("The ragdoll bone to be used for sensing.  You must specify a ragdoll bone or an animation bone, but not both.")

			/// The index of the animation bone whose coordinates will be used to specify the sensor location.
		hkInt16 m_sensorAnimationBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX")+hk.Description("The animation bone to be used for sensing.  You must specify a ragdoll bone or an animation bone, but not both.")

			/// The index of the rogdoll bone whose coordinates will be used to determine how close the target is.
		hkInt16 m_closeToTargetRagdollBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")+hk.Description("The index of the ragdoll bone whose coordinates will be used to determine how close the target is.")

			/// The index of the animation bone whose coordinates will be used to determine how close the target is.
		hkInt16 m_closeToTargetAnimationBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX")+hk.Description("The index of the animation bone whose coordinates will be used to determine how close the target is.")

			/// The offset of the sensor location in the space of the sensor bone.
		hkVector4 m_sensorOffsetInBoneSpace; //+default(0,1,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("The offset of the sensor location in the space of the sensor bone.")

			/// The offset of the close to target bone in the space of close to target bone.
		hkVector4 m_closeToTargetOffsetInBoneSpace; //+default(0,1,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("The offset of the close to target bone in the space of close to target bone.")

			/// The sensor direction in bone space.
		hkVector4 m_sensorDirectionBS; //+default(0,1,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("The sensor direction in bone space.")

			/// How often to send events.
		hkEnum< EventModeTRBAM, hkInt8 > m_eventMode; //+hk.Description("How often to send events.")

			/// The property to look for in the rigid bodies while sensing.
		hkUint32 m_sensingPropertyKey; //+default(0)+hk.Description("The property to look for in the rigid bodies while sensing.")

			/// If the property is set to true the sensorOffsetInBoneSpace is added in world space.
		hkBool m_sensorInWS; //+default(false)+hk.Description("If the property is set to true the sensorOffsetInBoneSpace is added in world space.")

			/// The event to send to the character that is using this modifier when a target is found.
		class hkbEventProperty m_eventToSend; //+hk.Description("The event to send to the character that is using this modifier when a target is found.")

			/// The event to send to the character that is being targeted when a target is found.
		class hkbEventProperty m_eventToSendToTarget; //+hk.Description("The event to send to the character that is being targeted when a target is found.")
		
			/// The event to send when the target is close enough.
		class hkbEventProperty m_closeToTargetEvent; //+hk.Description("The event to send when the target is close enough.")

			/// Take into account the sensing bone velocity to compute sensing bone position. This works only with ragdoll sensing bone.
		hkBool m_useVelocityPrediction; //+default(false)+hk.Description("Take into account the sensing bone velocity to compute sensing bone position. This works only with ragdoll sensing bone.")

			/// Only target rigid bodies that contain a sphere shape.
		hkBool m_targetOnlySpheres; //+default(false)+hk.Description("If this is true, a target rigid body will only be chosen if it contains a sphere shape.")

			/// Is the target close enough.
		hkBool m_isCloseToTargetOut;	//+default(false)
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
										//+hk.Description("Is the target close enough.")

			/// This property is used only when there are multiple producers of target information linked to the 
			/// same variable. The producer which has the highest target priority takes preference. If the target 
			/// priorities are the same for two producers then the one that is processed later takes preference.
		hkInt8 m_targetPriority; //+default(0)+hk.Description("The producer which has the highest target priority takes preference in case of multiple producers.")

	private:

			// The targeted rigid body.
		hkpRigidBody* m_targetRb; //+default(HK_NULL)+nosave
		
			// The distance of the targeted rigid body
		hkReal m_targetDistance; //+default(HK_REAL_MAX)+nosave

			// time elapsed since modify was called
		hkReal m_timeSinceLastModify; //+nosave

			// whether the event has been sent yet
		hkBool m_eventHasBeenSent; //+nosave

			// whether close to target event has been sent yet
		hkBool m_closeToTargetEventHasBeenSent;	//+nosave

			// whether modifier is currently active or inactive
		hkBool m_isActive;	//+nosave

	public:

		hkbpTargetRigidBodyModifier( hkFinishLoadedObjectFlag flag );

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
