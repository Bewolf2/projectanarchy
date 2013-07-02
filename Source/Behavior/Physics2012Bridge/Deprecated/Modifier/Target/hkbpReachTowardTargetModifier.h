/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_REACH_TOWARD_TARGET_MODIFIER_H
#define HKBP_REACH_TOWARD_TARGET_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Physics2012Bridge/Deprecated/Modifier/Target/hkbpTarget.h>

extern const class hkClass hkbpReachTowardTargetModifierHandClass;

extern const class hkClass hkbpReachTowardTargetModifierClass;

class hkpRigidBody;

/// An example custom modifier that reaches the arms toward a target.
class hkbpReachTowardTargetModifier : public hkbModifier
{
	//+version(2)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpReachTowardTargetModifier();

	private:

		hkbpReachTowardTargetModifier( const hkbpReachTowardTargetModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

		///////////////
		// properties
		///////////////

			/// The parameters for a hand.
		struct Hand
		{
			//+version(2)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Hand );
			HK_DECLARE_REFLECTION();

			/// Construct with initial values.
			Hand()
				:	m_shoulderIndex(-1),
					m_isEnabled(true)
			{}

				/// Index of the shoulder bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_shoulderIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The shoulder bone.")

				/// Indicates whether the hand is enabled for reaching.
			hkBool m_isEnabled; //+default(true)+hk.Description("Indicates whether the hand is enabled for reaching.")

			Hand( hkFinishLoadedObjectFlag ) {}
		};

			/// Hand struct for the left hand
		struct Hand m_leftHand; //+hk.Description("The left hand parameters.")

			/// Hand struct for the right hand
		struct Hand m_rightHand; //+hk.Description("The right hand parameters.")

			/// The target towards which we want to reach.
		hkRefPtr<hkbpTarget> m_targetIn;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
										//+hk.Description("The target towards which we want to reach.")
										
			/// How far apart the hands should be spread.
		hkReal m_distanceBetweenHands; //+default(0.3f)+hk.RangeReal(absmin=0.0,absmax=10.0,softmax=1.0)+hk.Description("How far apart the hands should be spread.")

			/// How far to extend the hands in front of the body.
		hkReal m_reachDistance; //+default(0.5f)+hk.RangeReal(absmin=0.0,absmax=10.0,softmax=1.0)+hk.Description("How far to extend the hands in front of the body.")

			/// The speed with which the gain fades in.
		hkReal m_fadeInGainSpeed; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The speed with which the gain fades in.")

			/// The speed with which the gain fades out.
		hkReal m_fadeOutGainSpeed;	//+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The speed with which the gain fades out.")

			/// The duration during which the hand Ik data fades out. This is passed as the track data.
		hkReal m_fadeOutDuration; //+default(0.0f)+hk.RangeReal(absmin=0.0,absmax=10.0,softmax=1.0)+hk.Description("The duration during which the hand Ik data fades out. This is passed as the track data.")

			/// The speed with which to go from old target to new target.
		hkReal m_targetChangeSpeed; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The speed with which to go from old target to new target.")

			/// Whether or not to keep the same target or look for new targets each time.
		hkBool m_holdTarget; //+default(false)+hk.Description("Whether or not to keep the same target or look for new targets each time.")

			/// If set to true, the hands are always extended m_reachDistance.
		hkBool m_reachPastTarget; //+default(true)+hk.Description("If set to true, the hands are always extended m_reachDistance.")

			/// If no target is found once, then never use another target.
		hkBool m_giveUpIfNoTarget; //+default(false)+hk.Description("If no target is found once, then never re-target again.")

	private:

			/// the target we're reaching toward
		class hkpRigidBody* m_targetRB;	//+nosave+default(HK_NULL)

			/// previous hand ik target position in model space
		hkVector4 m_prevTargetInMS;  //+nosave

			/// how much Ik to do 
		hkReal m_currentGain;	//+nosave

			/// time elapsed since modify was called
		hkReal m_timeSinceLastModify; //+nosave

		enum FadeState
		{ 
			FADE_IN,
			FADE_OUT, 
		};

			/// whether we are fading in or out
		hkEnum< FadeState, hkInt8 > m_fadeState; //+nosave

			/// whether we've given up reaching
		hkBool m_haveGivenUp; //+nosave+default(false)

			/// This is set to true when there is a new target. This is set to false only when the 
			/// old target has faded out completely.
		hkBool m_isTherePrevTarget; //+nosave

	public:

		hkbpReachTowardTargetModifier( hkFinishLoadedObjectFlag flag );

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
