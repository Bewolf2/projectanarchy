/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_MOVE_BONE_TOWARD_TARGET_MODIFIER_H
#define HKBP_MOVE_BONE_TOWARD_TARGET_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Physics2012Bridge/Deprecated/Modifier/Target/hkbpTarget.h>

extern const class hkClass hkbpMoveBoneTowardTargetModifierClass;

class hkaSkeleton;

	/// An example custom modifier for moving a bone toward a target using keyframing.
class hkbpMoveBoneTowardTargetModifier : public hkbModifier
{
	public:
		// +version(3)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpMoveBoneTowardTargetModifier();
		
	private:

		hkbpMoveBoneTowardTargetModifier( const hkbpMoveBoneTowardTargetModifier& mod );

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
		virtual void precompute( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

			/// Set up the data that is computed from the generator being used while
			/// moving the bone.
		void initAnimationData( hkbGenerator* generator, const hkbContext& context, const hkaSkeleton* ragdollSkeleton );

		///////////////
		// properties
		///////////////

		enum TargetModeMBTT
		{
			/// move toward the position of the target
			TARGET_POSITION,

			/// move toward the center-of-mass of the target
			TARGET_COM,

			/// move toward the contact point on the target
			TARGET_CONTACT_POINT,

			/// move toward the center of the shape of the target
			TARGET_SHAPE_CENTROID,
		};

			/// The target which we want to move towards.
		hkRefPtr<hkbpTarget> m_targetIn;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
										//+hk.Description("The target which we want to move towards.")
										
			/// The offset from the ragdoll bone in boneSpace
		hkVector4 m_offsetInBoneSpace; //+default(0,1,0)+hk.Description("The offset from the ragdoll bone in boneSpace.")

			/// An axis in the local space of the bone that we want to be aligned with the target.
		hkVector4 m_alignAxisBS; //+default(0,1,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("An axis in the local space of the bone that we want to be aligned with the target.")

			/// An axis in the local space of the target that we want to align with the bone.
		hkVector4 m_targetAlignAxisTS; //+default(0,1,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("An axis in the local space of the target that we want to align with the bone.")

			/// The vector in local space of the rigidbody to be aligned with the character forward vector
			
		hkVector4 m_alignWithCharacterForwardBS; //+default(0,1,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("The vector in local space of the rigidbody to be aligned with the character forward vector.")

			/// The current position of the bone that is moving toward the target.
		hkVector4 m_currentBonePositionOut;	//+default(0,0,0)
											//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
											//+hk.Description("The current position of the bone that is moving toward the target. You should bind this to a variable.")

			/// The current orientation of the bone that is moving toward the target.
		hkQuaternion m_currentBoneRotationOut;	//+default(0,0,0,1)
												//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
												//+hk.Description("The current orientation of the bone that is moving toward the target. You should bind this to a variable.")

			/// Event to send when the bone has reached the target.
		class hkbEventProperty m_eventToSendWhenTargetReached; //+hk.Description("Event to send when the bone has reached the target.")

			/// The child generator that this modifier will be applied to.  While typical modifiers do not
			/// know in advance what generator they will be applied to (via an hkbModifierGenerator), this
			/// node does some precomputation so it needs to know.
		hkRefPtr<hkbGenerator> m_childGenerator; 

			/// The duration the bone should take to reach the target
		hkReal m_duration; //+default(1)+hk.RangeReal(absmin=0,absmax=10)+hk.Description("The duration the bone should take to reach the target.")

			/// The index of the ragdoll bone to be moved toward the target
		hkInt16 m_ragdollBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")+hk.Description("The index of the ragdoll bone to be moved toward the target.")

			/// The index of the animation bone to be moved toward the target.
		hkInt16 m_animationBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX")+hk.Description("The index of the animation bone to be moved toward the target.")

			/// which point on the target rigid body is the actual target point to move toward
		hkEnum< TargetModeMBTT, hkInt8 > m_targetMode; //+hk.Description("Which point on the target rigid body is the actual target point to move toward.")
		
		enum AlignModeBits
		{
			/// align m_alignAxisBS in the bone with m_targetAlignAxisTS in the target
			ALIGN_AXES = 0x1,

			/// align m_alignAxisBS in the bone with the contact normal 
			ALIGN_BONE_AXIS_WITH_CONTACT_NORMAL = 0x2,

			/// align m_alignWithCharacterForwardBS in the bone with the character forward axis
			ALIGN_WITH_CHARACTER_FORWARD = 0x4,
		};

			/// How to align.
		hkInt8 m_alignMode; //+default(0)+hk.Description("How to align the bone when it reaches the target.")

			/// Use the target's velocity to estimate its future position.
		hkBool m_useVelocityPrediction; //+default(false)+hk.Description("Whether to use the target's velocity to estimate its future position.")

			/// If the user wants to change the orientation of the bone while moving towards 
			/// the target this flag should be set to true. The orientation of the bone would 
			/// then be affected based on the m_alignMode;
		hkBool m_affectOrientation; //+default(true)+hk.Description("If the user wants to change the orientation of the bone while moving towards the target this flag should be set to true.")

			/// Whether or not m_currentBonePosition and m_currentBoneRotation are valid.
		hkBool m_currentBoneIsValidOut;	//+default(false)
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
										//+hk.Description("Whether or not Whether or not m_currentBonePosition and m_currentBoneRotation are valid.  You should bind this to a variable.")
								

	private:

		/// The target to which the bone has to be moved
		class hkpRigidBody* m_targetInternal; //+nosave+default(HK_NULL)

		/// the point in target space to move the bone toward (if m_targetMode == TARGET_POINT)
		hkVector4 m_targetPointTS; //+nosave

		hkReal m_time; //+nosave+default(0.0f)
		hkReal m_timeSinceLastModify; //+nosave+default(0.0f)

		hkVector4 m_lastAnimBonePositionMS; //+nosave
		hkVector4 m_finalAnimBonePositionMS;
		hkVector4 m_initialAnimBonePositionMS;

		hkQuaternion m_finalAnimBoneOrientationMS;

		hkQuaternion m_animationFromRagdoll;

		hkQsTransform m_totalMotion;
		hkQsTransform m_accumulatedMotion;

		// When set to true, the modifier tries to follow the animation.
		// By default the value is false. To set it to true call function
		// initAnimationData
		hkBool m_useAnimationData;

	public:

		hkbpMoveBoneTowardTargetModifier( hkFinishLoadedObjectFlag flag );

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
