/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CATCH_FALL_MODIFIER_H
#define HKB_CATCH_FALL_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Character/hkbBoneIndexArray.h>

extern const class hkClass hkbpCatchFallModifierHandClass;

extern const class hkClass hkbpCatchFallModifierClass;

class hkaPose;

/// An example custom modifier for making the character procedurally catch his
/// or her fall by protecting with the arms.
class hkbpCatchFallModifier : public hkbModifier
{
	public:
		// +version(2)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpCatchFallModifier();

	protected:

		hkbpCatchFallModifier( const hkbpCatchFallModifier& mod );

	public:

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

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

		//////////////
		// properties
		//////////////

			/// The forward direction of the ragdoll bone used to compute direction of fall in local space.
		hkVector4 m_directionOfFallForwardLS;	//+default(0,1,0)
												//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
												//+hk.Description("The forward direction of the ragdoll bone used to compute direction of fall in local space.")

			/// The right direction of the ragdoll bone used to compute direction of fall in local space.
		hkVector4 m_directionOfFallRightLS;	//+default(0,0,1)
											//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
											//+hk.Description("The rightward direction of the ragdoll bone used to compute direction of fall in local space.")

			/// The up direction of the ragdoll bone used to compute direction of fall in local space.
		hkVector4 m_directionOfFallUpLS;	//+default(1,0,0)
											//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
											//+hk.Description("The upward direction of the ragdoll bone used to compute direction of fall in local space.")

			/// The indices of the spine used for twisting the upper body.
		hkRefPtr<hkbBoneIndexArray> m_spineIndices;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
													//+hk.Description("The spine bones used for twisting the upper body.")

			/// The parameters for a hand.
		struct Hand
		{
			//+version(2)

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Hand );
			HK_DECLARE_REFLECTION();

			/// Construct with initial values.
			Hand()
				:	m_animShoulderIndex(-1),
					m_ragdollShoulderIndex(-1),
					m_ragdollAnkleIndex(-1)
			{}

				/// Index of the animation shoulder bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_animShoulderIndex; //+default(-1)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

				/// Index of the ragdoll shoulder bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_ragdollShoulderIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")+hk.Description("The shoulder bone.")

				/// Index of the ragdoll ankle bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_ragdollAnkleIndex; //+default(-1)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			Hand( hkFinishLoadedObjectFlag ) {}
		};

			/// Hand struct for the left hand
		struct Hand m_leftHand; //+hk.Description("The left hand parameters.")

			/// Hand struct for the right hand
		struct Hand m_rightHand; //+hk.Description("The right hand parameters.")

			/// Event to send when catch fall has ended.
		class hkbEventProperty m_catchFallDoneEvent; //+hk.Description("The event to send when catch fall has ended.")

			/// The fraction used to scale the distance between the raycast start position for the two hands.
		hkReal m_spreadHandsMultiplier; //+default(1)+hk.RangeReal(absmin=-10,absmax=10)+hk.Description("The amount to spread the hands, as a multiplier of their distance in the animation.")

			/// The length of the ray used for raycasting.
		hkReal m_radarRange; //+default(1.0f)+hk.RangeReal(absmin=-10,absmax=10)+hk.Description("The distance to raycast in the downward direction from the hands.")

			/// The fraction used to compute how much to factor in the previous hand targets.
		hkReal m_previousTargetBlendWeight;  //+default(0.75f)+hk.RangeReal(absmin=0.0,absmax=1.0)+hk.Description("On each frame a new target point is chosen.  It is blended with the previous target to smooth out the result.  This blend weight determines how much of the previous target is used when computing the final target.")

			/// While the character is falling, the hands are moved this distance from their outstretched positions in order to cause the arms to bend.
		hkReal m_handsBendDistance; //+default(0.1f)+hk.RangeReal(absmin=0,absmax=2)+hk.Description("While the character is falling, the hands are moved this distance from their outstretched positions in order to cause the arms to bend.")

			/// The maximum distance the hands can reach in the forward direction.
		hkReal m_maxReachDistanceForward; //+default(0.6f)+hk.RangeReal(absmin=0,absmax=2)+hk.Description("The maximum distance the hands can reach in the forward direction, measured from their positions in the incoming animation.")

			/// The maximum distance the hands can reach in the backward direction.
		hkReal m_maxReachDistanceBackward; //+default(0.8f)+hk.RangeReal(absmin=0,absmax=2)+hk.Description("The maximum distance the hands can reach in the backward direction, measured from their positions in the incoming animation.")

			/// The speed at which the gains increase as the hands reach towards their targets.  When the gains are 1 the hands meet their targets.
		hkReal m_fadeInReachGainSpeed; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=10.0)+hk.Description("The speed at which the gains are increased as the hands reach towards their targets.  When the gains reach 1 the hands meet their targets.")

			/// The speed at which the gains decrease as the hands stop reaching.  When the gains are 0 the hands stop reaching altogether.
		hkReal m_fadeOutReachGainSpeed; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=10.0)+hk.Description("The speed at which the gains decrease as the hands stop reaching.  When the gains are 0 the hands stop reaching altogether.")

			/// The duration during which the hand IK data fades out when there is no target.
		hkReal m_fadeOutDuration; //+default(0.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The duration during which the hand IK data fades out when there is no target.")

			/// The speed at which the twisting of the upper body fades in.
		hkReal m_fadeInTwistSpeed; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The speed at which the twisting of the upper body fades in.")

			/// The speed at which the twisting of the upper body fades out.
		hkReal m_fadeOutTwistSpeed; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The speed at which the twisting of the upper body fades out.")

			/// The raycast layer used to find something to reach.
		hkInt16 m_raycastLayer; //+default(-1)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")+hk.Description("The raycast layer used to something to reach to.")

			/// The bone is used to decide when to start and stop catch fall.
			///
			/// The velocity of the bone is used to decide when to start catch fall. The vertical
			/// distance of the bone above the ground decides when to stop catch fall. 
		hkInt16 m_velocityRagdollBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")+hk.Description("This bone is used to measure the velocity of the character.")

			/// The direction of fall is computed relative to the orientation of this bone.
		hkInt16 m_directionOfFallRagdollBoneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")+hk.Description("The direction of fall is computed relative to the orientation of this bone.")

			/// Orient hands while doing catch fall.
		hkBool m_orientHands; //+default(true)+hk.Description("Whether to orient the hands with the terrain.")

	private:

		// maximum number of hands the modifier can operate on
		enum { MAX_LIMBS = 2 };

		enum FadeState
		{ 
			FADE_IN,
			FADE_OUT, 
		};

			// current hand target position for the hands in bone space 
		hkVector4 m_catchFallPosInBS[MAX_LIMBS];

			// current gains for the hand Ik
		hkReal m_currentReachGain[MAX_LIMBS];

			// time elapsed since modify was called
		hkReal m_timeSinceLastModify; //nosave

			// current gain for the twist of the upper body 
		hkReal m_currentTwistGain; //nosave

			// the direction in which the upper body is rotated 
		hkInt16 m_currentTwistDirection; //nosave

			// this indicates whether the handIk targets have been found
		hkBool m_catchFallPosIsValid[MAX_LIMBS];

			// Is set to true when catch fall starts. Default value is false.
		hkBool m_catchFallBegin; //+nosave

			// Is set to true when the catch fall ends. Default value is false.
		hkBool m_catchFallEnd; //+nosave

			// whether we are fading in or out
		hkEnum< FadeState, hkInt8 > m_fadeState[MAX_LIMBS]; //+nosave

			// compute the direction of motion of the ragdoll
		void computeDirectionOfMotionOfRagdoll( hkVector4& directionOfMotion, const hkbContext& context );

			// while doing catch fall determine which hand to use for catch fall, whether to twist and twist in which direction.
		void determineHandsAndTwist(	int& startHandIndex, 
										int& numLimbs, 
										bool& twist, 
										hkInt16& twistDirection, 
										hkVector4& directionOfMotionInCharacterForwardRightPlane,
										hkVector4& directionOfMotion,
										bool& fallingBackward,
										const hkbContext& context, 
										const hkbGeneratorOutput& inOut,
										const hkQsTransform& currentOrientation );

			// twist upper body
		void twistUpperBody(	const bool twist, 
								const hkInt16 twistDirection,
								class hkaPose& pose,
								const hkbContext& context );

			// rotate the bone  
		void rotateBone(	int boneIndex, 
							const hkInt16 twistDirection, 
							class hkaPose& pose, 
							const hkVector4& modelUpMS );

			// compute the radar locations for the raycast
		void computeRadarLocations( hkVector4* radarLocation, 
									hkaPose& pose, 
									const hkbContext& context,
									const hkbGeneratorOutput& inOut );

	public:

		hkbpCatchFallModifier( hkFinishLoadedObjectFlag flag );

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
