/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_HAND_IK_MODIFIER_H
#define HKB_HAND_IK_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Utils/hkbBlendCurveUtils.h>
#include <Behavior/Behavior/Utils/hkbHandle.h>

extern const class hkClass hkbHandIkModifierHandClass;

extern const class hkClass hkbHandIkModifierClass;

class hkaSkeleton;
class hkaPose;

	/// Deprecated.  Please use the hkbHandIkControlsModifier and the hkbHandIkDriver instead of this modifier.
	///
	/// A modifier that performs hand IK.
	///
	/// The hkbHandIkModifier performs hand IK using data that it gets from the tracks. Currently there is a track data
	/// for hand positions. There is one for each hand. The same is for hand normals. The hand IK has the ability to
	/// fade out when the modifier stops getting the track data. The duration that it should take to fade
	/// out is provided by the track data itself.
class hkbHandIkModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbHandIkModifier();

	protected:

		hkbHandIkModifier( const hkbHandIkModifier& mod );

	public:

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

	public:

		///////////////
		// properties
		///////////////

		struct Hand
		{
			// +version(4)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Hand );
			HK_DECLARE_REFLECTION();

				/// Construct with initial values.
			Hand()
				:	m_elbowAxisLS(hkVector4(0.0f, 1.0f, 0.0f)),
					m_backHandNormalLS(hkVector4(0.0f, 1.0f, 0.0f)),
					m_handOffsetLS(hkVector4::getZero()),
					m_handOrientationOffsetLS(hkQuaternion::getIdentity()),
					m_maxElbowAngleDegrees(180.0f),
					m_minElbowAngleDegrees(0.0f),
					m_shoulderIndex(-1),
					m_shoulderSiblingIndex(-1),
					m_elbowIndex(-1),
					m_elbowSiblingIndex(-1),
					m_wristIndex(-1),
					m_enforceEndPosition(true),
					m_enforceEndRotation(false)
			{}
					/// The axis of rotation for the elbow, in the coordinate frame of the elbow (forearm).
				hkVector4 m_elbowAxisLS; //+default(0,1,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("The axis of the elbow in the coordinate frame of the elbow (forearm).")

					/// The vector that is normal to the back of the hand, in the coordinate frame of the hand.
				hkVector4 m_backHandNormalLS; //+default(0,1,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("The vector that is normal to the back of the hand, in the coordinate frame of the hand.")

					/// The offset of the point in the hand that will be positioned by the IK.
				hkVector4 m_handOffsetLS;	//+default(0,0,0)
											// +hk.Description("The offset of the point in the hand that will be positioned by the IK.")

					/// The local orientation of the offset point in the hand.  
				hkQuaternion m_handOrientationOffsetLS;	//+default(0,0,0,1)
													// +hk.Description("The local orientation of the offset point in the hand.")

					/// Limit the elbow angle (to avoid elbow popping artifacts). Default is 180 degrees.
				hkReal m_maxElbowAngleDegrees;	//+default(180.0f)+hk.RangeReal(absmin=0,absmax=180)+hk.Description("The maximum elbow angle allowed.")

					/// Limit the elbow angle (to avoid elbow artifacts). Default is 0 degrees.
				hkReal m_minElbowAngleDegrees;	//+default(0.0f)+hk.RangeReal(absmin=0,absmax=180)+hk.Description("The minimum elbow angle allowed.")

					/// Index of the shoulder bone.  If this is not -1, it overrides the character boneInfo.
				hkInt16 m_shoulderIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The shoulder bone.")

					/// Index of the sibling of the shoulder bone. The same rotation that is applied to the shoulder bone is also applied to this bone. This is useful if your character has additional skinning bones.
				hkInt16 m_shoulderSiblingIndex; //+default(-1)
												//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
												//+hk.Description("The same rotation that is applied to the shoulder bone is also applied to this bone. This is useful if your character has additional skinning bones.")

					/// Index of the elbow bone.  If this is not -1, it overrides the character boneInfo.
				hkInt16 m_elbowIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The elbow bone.")

					/// Index of the sibling of the elbow bone. The same rotation that is applied to the elbow bone is also applied to this bone. This is useful if your character has additional skinning bones.
				hkInt16 m_elbowSiblingIndex;	//+default(-1)
												//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
												//+hk.Description("The same rotation that is applied to the elbow bone is also applied to this bone. This is useful if your character has additional skinning bones.")

					/// Index of the wrist bone.  If this is not -1, it overrides the character boneInfo.
				hkInt16 m_wristIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The wrist bone.")

					/// Set to true if the position of the end effector is to be solved for
				hkBool m_enforceEndPosition;	//+default(true)
												//+hk.Description("If this property is set, the IK solver will place the hand at the target location.")

					/// Set to true if the rotation of the end effector is to be solved for
				hkBool m_enforceEndRotation;	//+default(false)
												//+hk.Description("If this property is set, the IK solver will orient the hand to the target rotation.")

					/// An optional local frame which describes the point in the hand that IK will be applied to.
				hkStringPtr m_localFrameName;	//+hk.Description("Optional local frame which describes the contact location and rotation of the end effector.")

				Hand( hkFinishLoadedObjectFlag flag ) : m_localFrameName(flag) {}
		};

			/// A Hand structure for each hand.
		hkArray< struct Hand > m_hands; //hk.Description("The parameters describing all of the hands.")

			/// The current on-fraction is passed through this blend curve to produce the current gain that is used in the IK.
		hkEnum< hkbBlendCurveUtils::BlendCurve, hkInt8 > m_fadeInOutCurve;	//+default(hkbBlendCurveUtils::BLEND_CURVE_SMOOTH)
																			//+hk.Description("This blend curve is applied when fading in and out the IK.")

	private:

		struct InternalHandData
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbHandIkModifier::InternalHandData );

			// current target in model space
			hkVector4 m_currentTargetMS;

			// current target in model space
			hkQuaternion m_currentTargetRotationMS;

			// current target hand normal in model space
			hkVector4 m_currentTargetHandNormalMS;

			// The local frame corresponding to the given name (if found)
			class hkbHandle m_localFrameHandle;

			// how much the position IK is on now
			hkReal m_currentPositionOnFraction;

			// the goal for how much the position IK should be on
			hkReal m_targetPositionOnFraction;

			// how much the hand normal IK is on now
			hkReal m_currentNormalOnFraction;

			// the goal for how much the hand normal IK should be on
			hkReal m_targetNormalOnFraction;

			// how long to fade in IK
			hkReal m_fadeInDuration;

			// how long to fade out IK
			hkReal m_fadeOutDuration;

			// the value of the track on fraction in the previous frame.
			hkReal m_previousTrackOnFraction;
		};

		void doArmIKModelSpace( int hand, hkReal firstJointGain, hkReal secondJointGain, hkReal endJointGain, const hkVector4& targetInModelSpace, const hkQuaternion& targetRotationInModelSpace, hkaPose& poseInOut );
		void doArmIKWorldSpace( int hand, hkReal firstJointGain, hkReal secondJointGain, hkReal endJointGain, const hkVector4& targetInWorldSpace, const hkQuaternion& targetRotationInModelSpace, const hkQsTransform& worldFromModel, hkaPose& poseInOut );
		void orientHand( int hand, hkReal fractionToRotate, const hkVector4& desiredUpMS, const hkQsTransform& worldFromModel, hkaPose& poseInOut );

		hkArray< struct InternalHandData > m_internalHandData; //+nosave

	public:

		hkbHandIkModifier( hkFinishLoadedObjectFlag flag );

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
