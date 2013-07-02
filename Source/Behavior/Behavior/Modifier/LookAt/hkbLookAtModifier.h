/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_LOOK_AT_MODIFIER_H
#define HKB_LOOK_AT_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbLookAtModifierClass;

	/// A modifier that looks at a target point.
class hkbLookAtModifier : public hkbModifier
{
	public:
		// +version(3)
		
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbLookAtModifier();

	protected:

		hkbLookAtModifier( const hkbLookAtModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

			// hkbModifier interface implementation.
		virtual void predictiveModify( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timeOffset ) HK_OVERRIDE;

	public:

		//////////////
		// properties
		//////////////

			/// The target position in world coordinates.
		hkVector4 m_targetWS;	//+default(0,0,0)
								//+hk.Description("The position in the world that is being looked at. This is used only when m_target is not attached to any variable.")

			/// The forward direction in the coordinates of the head. Default is (0,1,0).
		hkVector4 m_headForwardLS;	//+default(0,1,0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
									//+hk.Description("The forward direction of the head, in the coordinate system of the head.")

			/// The forward direction in the coordinates of the neck. Default is (0,1,0).
		hkVector4 m_neckForwardLS;	//+default(0,1,0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
									//+hk.Description("The forward direction of the neck, in the coordinate system of the neck.")

			/// The right direction in the coordinates of the neck. Default is (1,0,0).
		hkVector4 m_neckRightLS;	//+default(1,0,0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
									//+hk.Description("The rightward direction of the neck, in the coordinate system of the neck.")

			/// The "Eye" position in the coordinates of the head. Default is (0,0,0).
		hkVector4 m_eyePositionHS;	//+default(0,0,0)
									//+hk.Description("The eye position in the coordinates of the head.")

			/// Gain used when a new target is found. This gain is used to smoothly transition
			/// from the old target to the new target. Default value is 0.2.
		hkReal m_newTargetGain;	//+default(0.2f)
								//+hk.RangeReal(absmin=0.0,absmax=1.0)
								//+hk.Description("Gain used to smooth out the motion when a new target is selected.")

			/// Gain used to smooth out the motion when the character starts looking at a target.
			/// Default value is 0.05.
		hkReal m_onGain;	//+default(0.05f)
							//+hk.RangeReal(absmin=0.0,absmax=1.0)
							//+hk.Description("Gain used to smooth out the motion when the character starts looking at a target.")

			/// Gain used to smooth out the motion when the character stops looking at a target.
			/// Default value is 0.05.
		hkReal m_offGain;	//+default(0.05f)
							//+hk.RangeReal(absmin=0.0,absmax=1.0)
							//+hk.Description("Gain used to smooth out the motion when the character stops looking at a target.")

			/// Angle of the lookAt limiting cone, in degrees. Must be in range [ 0, 180 ]
		hkReal m_limitAngleDegrees; //+default(45.0)
									//+hk.RangeReal(absmin=0.0,absmax=180.0)
									//+hk.Description("The forward vector of the head is limited by a cone having this angle, in degrees, between its center and its side.")

			/// The maximum angle to look left
		hkReal m_limitAngleLeft;	//+default(90.0f)
									//+hk.Description("The maximum angle to look left")

			/// The maximum angle to look right
		hkReal m_limitAngleRight;	//+default(-90.0f)
									//+hk.Description("The maximum angle to look right")

			/// The maximum angle to look up
		hkReal m_limitAngleUp;		//+default(30.0f)
									//+hk.Description("The maximum angle to look up")

			/// The maximum angle to look down
		hkReal m_limitAngleDown;	//+default(-30.0f)
									//+hk.Description("The maximum angle to look up")

			/// Index of the head bone. If this is not -1, it overrides the character boneInfo.
		hkInt16 m_headIndex;	//+default(-1)
								//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The head bone.")

			/// Index of the neck bone. If this is not -1, it overrides the character boneInfo.
		hkInt16 m_neckIndex;	//+default(-1)
								//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The neck bone.")

			/// Whether or not the IK is on (if off it will fade out).
		hkBool m_isOn;	//+default(true)
						//+hk.Description("Use this to turn on and off the IK")

			/// Whether a single limit (m_limitAngleDegrees) or individual limits are used
		hkBool m_individualLimitsOn;	//+hk.Description("Use this to enable individual limits")

			/// Whether the target point was found to be inside the target cone. This also becomes false if IK is not run at all because it is turned off or the target is behind the character.
		hkBool m_isTargetInsideLimitCone;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
											//+hk.Description("Whether the target point was found to be inside the target cone. This also becomes false if IK is not run at all because it is turned off or the target is behind the character.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

			/// The previous target in world coordinates. 
		hkVector4 m_lookAtLastTargetWS; //+nosave

			/// This is used when going from looking at fixed angle to looking
			/// at something and from looking at something to looking at a fixed angle.
		hkReal m_lookAtWeight; //+nosave

			/// This is commented out on purpose to force the output property m_isTargetInsideLimitCone to be part of the internal state.
		//hkBool m_isTargetInsideLimitCone;

		HKB_END_INTERNAL_STATE();

	public:

		hkbLookAtModifier( hkFinishLoadedObjectFlag flag );
		
		friend class hkbLookAtModifierUtils;
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
