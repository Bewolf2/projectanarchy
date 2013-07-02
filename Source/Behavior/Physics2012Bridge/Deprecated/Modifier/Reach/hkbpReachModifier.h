/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_REACH_MODIFIER_H
#define HKBP_REACH_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbpReachModifierHandClass;

extern const class hkClass hkbpReachModifierClass;

	/// An example custom modifier that reaches the hands toward stuff.  It is recommended that you use the 
	/// fully supported hkbHandIkControlsModifier instead of this one.
class hkbpReachModifier : public hkbModifier
{
	public:

		//+version(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpReachModifier();

		//////////////////////
		// hkbNode interface
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;


		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

		//////////////
		// properties
		//////////////

			/// The parameters for a hand
		struct Hand
		{
			//+version(1)

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Hand );
			HK_DECLARE_REFLECTION();

				/// Construct with initial values.
			Hand()
				:	m_targetOrSensingPosition(hkVector4::getZero()),
					m_targetBackHandNormal(hkVector4::getZero()),
					m_sensingRadius(0.4f),
					m_boneIndex(-1),
					m_handIkTrackIndex(-1)
			{}

				/// When the reach mode is REACH_MODE_TERRAIN or REACH_MODE_BONE_POSITION this acts a relative position
				/// to the m_reachBoneIndex frame of reference. When the mode is REACH_MODE_TERRAIN this acts a radar location
				/// for sensing otherwise it acts as a target position. When the reach mode is REACH_MODE_WORLD_POSITION
				/// this acts as a target point in world space and when the mode is REACH_MODE_MODEL_POSITION it acts as a
				/// target point in model space.
			hkVector4 m_targetOrSensingPosition; //+default(0,0,0)+hkb.RoleAttribute("ROLE_DEFAULT", "FLAG_NONE")+hk.Description("When the reach mode is REACH_MODE_TERRAIN or REACH_MODE_BONE_POSITION this acts a relative position to the m_reachBoneIndex frame of reference. When the mode is reach mode terrain this acts a radar location for sensing otherwise it acts as a target position. When the reach mode is REACH_MODE_WORLD_POSITION this acts as a target point in world space and when the mode is REACH_MODE_MODEL_POSITION it acts as a target point in model space." )

				/// The target orientation of the back hand normal. This needs to be specified when reach mode is not REACH_MODE_TERRAIN.
				/// When reach mode is REACH_MODE_WORLD_POSITION the normal is considered in world space. When reach mode is
				/// REACH_MODE_MODEL_POSITION the normal is considered in model space. When mode is REACH_MODE_BONE_POSITION the normal is
				/// considered to be relative to the bone frame of reference.
			hkVector4 m_targetBackHandNormal; //+default(0,0,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("The target orientation of the back hand normal. This needs to be specified when reach mode is not REACH_MODE_TERRAIN.")

				/// When the reach mode is REACH_MODE_TERRAIN this property determines the region used for sensing.
			hkReal m_sensingRadius; //+default(0.4f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10)+hk.Description("The radius of the spherical region around m_reachPosition which will be considered for sensing.")

				/// The bone whose reference frame is used when the reach mode is REACH_MODE_TERRAIN or REACH_MODE_BONE_POSITION.
			hkInt16 m_boneIndex; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The bone whose reference frame is used when the reach mode is REACH_MODE_TERRAIN or REACH_MODE_BONE_POSITION. When the reach mode is REACH_MODE_TERRAIN it acts as a sensing bone otherwise it acts as a target bone.")

				/// Currently handIk track can handle arbitrary number of tracks. This property lets the user choose
				/// which hand goes into which track. Default is -1.
			hkInt16 m_handIkTrackIndex; //+default(-1)+hk.Description("This is used to determine where to place the data in the hand Ik track.")

			Hand( hkFinishLoadedObjectFlag ) {}
		};

			/// A Hand structure for each hand.
		hkArray< struct Hand > m_hands; //+hk.Description("The parameters describing all of the hands.")

		enum ReachMode
		{
			REACH_MODE_TERRAIN,
			REACH_MODE_WORLD_POSITION,
			REACH_MODE_MODEL_POSITION,
			REACH_MODE_BONE_POSITION,
		};

			/// Used to blend in new targets (between 0 and 1).
		hkReal m_newTargetGain; //+default(0.085f)+hk.RangeReal(absmin=0.0,absmax=1.0)+hk.Description("When the target computed in the current frame is different from the previous frame then this gain is used to smoothly blend from the old target to the new target.")

			/// Used when the hand is leaving a target (between 0 and 1).
		hkReal m_noTargetGain; //+default(0.19f)+hk.RangeReal(absmin=0.0,absmax=1.0)+hk.Description("When there is no target in the current frame then this gain is used to smoothly blend out the old target position.")

			/// Used when the hand is reaching toward a target (between 0 and 1).
		hkReal m_targetGain; //+default(0.3f)+hk.RangeReal(absmin=0.0,absmax=1.0)+hk.Description("When there is a target in the current frame and there is no target in the previous frame then this gain is used to smoothly blend in the target position.")

			/// The duration during which the hand Ik data fades out when there is no target.
		hkReal m_fadeOutDuration; //+default(0.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10)+hk.Description("The duration during which the hand Ik data fades out when there is no target. This flows through the tracks and gets used by the hand Ik modifier. ")

			/// The raycast layer used to find something to reach.
		hkInt32 m_raycastLayer; //+default(-1)+hk.Description("The layer used to filter collisions when raycasting. The character will only reach for things that collide with this layer.")

			/// The property to look for in the rigid bodies while sensing.
		hkUint32 m_sensingPropertyKey; //+default(0)

			/// This mode specifies the different ways we can reach out in the world.
		hkEnum< ReachMode, hkInt8 > m_reachMode; //+default(0)+hk.Description("This mode specifies the different ways we can reach out in the world.")

			/// Whether to ignore any rigid bodies with the same system group as this character's ragdoll.
		hkBool m_ignoreMySystemGroup; //+default(true)+hk.Description("Whether to ignore any rigid bodies with the same system group as this character's ragdoll.")

			/// Whether to extrapolate the position of the target.
		hkReal m_extrapolate; //+default(1.0f)

	private:

			/// Internal data for each hand
		struct InternalHandData
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbpReachModifier::InternalHandData );

				/// A weight for each hand to smooth out reaching
			hkReal m_reachWeight; //+nosave

				/// previous target position in WS
			hkVector4 m_previousReachPointMS; //+nosave

				/// previous target normal in WS
			hkVector4 m_previousNormalWS; //+nosave
		};

			/// An Internal hand structure for each hand.
		hkArray< struct InternalHandData > m_internalHandData; //+nosave

			/// The amount of time that has lapsed since the last modify().
		hkReal m_timeLapse; //+nosave

	public:

		hkbpReachModifier( hkFinishLoadedObjectFlag flag );

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
