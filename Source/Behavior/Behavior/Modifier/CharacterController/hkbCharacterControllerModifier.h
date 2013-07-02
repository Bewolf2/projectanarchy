/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_CONTROLLER_MODIFIER_H
#define HKB_CHARACTER_CONTROLLER_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

/// The controls that influence the behavior of the character controller and the relationship between the world from model and the character controller.
struct hkbCharacterControllerModifierControlData
{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbCharacterControllerModifierControlData );
		HK_DECLARE_REFLECTION();

		hkbCharacterControllerModifierControlData()
			:
			m_verticalGain(1.0f),
			m_horizontalCatchUpGain(1.0f),
			m_maxVerticalSeparation(0.0f),
			m_maxHorizontalSeparation(0.0f)
		{}
		
		hkbCharacterControllerModifierControlData(const hkbCharacterControllerModifierControlData& ctrlData)
			:
			m_verticalGain(ctrlData.m_verticalGain),
			m_horizontalCatchUpGain(ctrlData.m_horizontalCatchUpGain),
			m_maxVerticalSeparation(ctrlData.m_maxVerticalSeparation),
			m_maxHorizontalSeparation(ctrlData.m_maxHorizontalSeparation)
		{}

			/// The gain used to decide how to move the world-from-model transform towards the character controller in the vertical direction.
		hkReal m_verticalGain;	//+default(1.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1.0)
								//+hk.Description("The gain used to decide how to move the world-from-model transform towards the character controller in the vertical direction.")

			/// When going up stairs the character controller may lose velocity due to collisions with the steps. This gain is used to decide 
			/// how much of that loss should be taken into account while computing the character controller velocity.
		hkReal m_horizontalCatchUpGain;	//+default(1.0f)
										//+hk.RangeReal(absmin=0.0,absmax=1.0)
										//+hk.Description("When going up stairs the character controller may lose velocity due to collisions with the steps. This gain is used to decide how much of that loss should be taken into account while computing the character controller velocity.")

			/// The maximum distance between the world-from-model and the character controller in the vertical direction.
		hkReal m_maxVerticalSeparation; //+default(0.0f)
										//+hk.RangeReal(absmin=0.0,absmax=1.0)
										//+hk.Description("The maximum distance between the world-from-model transform and the character controller in the vertical direction.")

			/// The maximum distance between the actual character controller position due to loss in velocity and the expected character controller position in the horizontal direction.
		hkReal m_maxHorizontalSeparation;	//+default(0.0f)
											//+hk.RangeReal(absmin=0.0,absmax=1.0)
											//+hk.Description("The maximum distance between the actual character controller position due to loss in velocity and the expected character controller position in the horizontal direction.")

		hkbCharacterControllerModifierControlData( hkFinishLoadedObjectFlag flag ) {}
};

/// Produces control data used by hkbCharacterControllerController and puts it into
/// the tracks of the output when modify() is called.  This data is passed 
/// through the behavior tree and can be blended by blend nodes and during transitions.
class hkbCharacterControllerModifier : public hkbModifier
{
	public:
		//+version(1)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbCharacterControllerModifier();

	protected:

		hkbCharacterControllerModifier( const hkbCharacterControllerModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

			/// The control data for the character controller.
		struct hkbCharacterControllerModifierControlData m_controlData; //+hk.Description("The controls data can be used to provide a soft connection between the world from model and the character controller.")

			/// Whether m_initialVelocity is in world space or model space.
		enum InitialVelocityCoordinates
		{
				/// Indicates that m_initialVelocity is in world space.
			INITIAL_VELOCITY_IN_WORLD_COORDINATES = 0,

				/// Indicates that m_initialVelocity is in model space.
			INITIAL_VELOCITY_IN_MODEL_COORDINATES,
		};

			/// Determines how the character controller is moving.
		enum MotionMode
		{
				/// Indicates that the character controller should follow the motion extracted from the animation.
			MOTION_MODE_FOLLOW_ANIMATION = 0,

				/// Indicates that the character controller is following its own momentum.
			MOTION_MODE_DYNAMIC,
		};

			/// If m_setInitialVelocity is true, the velocity of the character controller is set to m_initialVelocity on the first frame that
			/// this modifier is active.  The initial velocity is only set if m_motionMode is MOTION_MODE_DYNAMIC.
		hkVector4 m_initialVelocity;	//+default(0,0,0)
										//+hk.Description("If m_setInitialVelocity is true, the velocity of the character controller is set to m_initialVelocity on the first frame that that this modifier is active.  If m_motionMode is MOTION_MODE_FOLLOW_ANIMATION then m_motionMode is ignored.")

			/// Whether the m_initialVelocity is in world space or model space.
		hkEnum<InitialVelocityCoordinates, hkInt8> m_initialVelocityCoordinates; //+hk.Description("Whether m_initialVelocity is in world space or model space.")

			/// If this is set to MOTION_MODE_FOLLOW_ANIMATION then the character controller will
			/// follow the motion extracted from the animation. If it is set to MOTION_MODE_DYNAMIC then the character controller preserves its momentum.
		hkEnum<MotionMode, hkInt8> m_motionMode;	//+default(hkbCharacterControllerModifier::MOTION_MODE_FOLLOW_ANIMATION)
													//+hk.Description("If this is set to MOTION_MODE_FOLLOW_ANIMATION then the character controller will follow the motion extracted from the animation. If it is set to MOTION_MODE_DYNAMIC then it preserves its momentum.")

			/// When this is set to true in conjunction with m_motionMode set to MOTION_MODE_FOLLOW_ANIMATION, whenever the character controller has a downward velocity it is
			/// maintained, ignoring the vertical component of the motion extracted from the animation.  This allows the character to fall through the air and
			/// stick to the ground while running down hills, all while allowing the horizontal motion to come from the animation.
		hkBool m_forceDownwardMomentum;	//+default(true)
										//+hk.Description("When this is set to true in conjunction with m_motionMode set to MOTION_MODE_FOLLOW_ANIMATION, whenever the character controller has a downward velocity it is maintained, ignoring the vertical component of the motion extracted from the animation.  This allows the character to fall through the air and stick to the ground while running down hills, all while allowing the horizontal motion to come from the animation.")

			/// If this is true, gravity will be applied when the character controller is not touching the ground.
		hkBool m_applyGravity;	//+default(true)
								//+hk.Description("If this is true, gravity will be applied when the character controller is not touching the ground.")

			/// Whether to set the velocity of the character controller to m_initialVelocity on the first frame.  This on only applied if m_motionMode is MOTION_MODE_DYNAMIC.
		hkBool m_setInitialVelocity;	//+default(false)
										//+hk.Description("Whether to set the velocity of the character controller to m_initialVelocity on the first frame.  This on only applied if m_motionMode is MOTION_MODE_DYNAMIC.")

			/// Whether or not character controller is touching the ground.
		hkBool m_isTouchingGround;	//+default(true)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("Whether or not character controller is touching the ground.")

	private:

			// Compute the desired velocity of the character controller using the character motion and gravity.
		void computeDesiredVelocity(	const hkbContext& context, 
										const hkQsTransform& worldFromModel,
										hkVector4& desiredVelocity ) const;

	private:
		
		HKB_BEGIN_INTERNAL_STATE(0);

			// Gravity required to compute the character controller velocity.
		hkVector4 m_gravity; //+nosave
		
			// Is initial velocity added
		hkBool m_isInitialVelocityAdded; //+nosave

			// this is commented out on purpose to force the output properties to be part of the internal state
		//hkBool m_isTouchingGround;

		HKB_END_INTERNAL_STATE();

	public:

		hkbCharacterControllerModifier( hkFinishLoadedObjectFlag flag );

		friend class hkbCharacterControllerModifierUtils;
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
