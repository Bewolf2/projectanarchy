/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_FOOT_IK_CONTROLS_MODIFIER_H
#define HKB_FOOT_IK_CONTROLS_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Modifier/FootIk/hkbFootIkControlData.h>

extern const class hkClass hkbFootIkControlsModifierLegClass;

extern const class hkClass hkbFootIkControlsModifierClass;

/// Places foot IK control data into the track data to be blended in a behavior graph.
/// 
/// The control data is consumed by the hkbFootIkModifier, which should be placed
/// rootward of this node in the behavior graph.
class hkbFootIkControlsModifier : public hkbModifier
{
	//+version(1)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbFootIkControlsModifier() : hkbModifier(HKB_NODE_TYPE_FOOT_IK_CONTROLS_MODIFIER) {}

	protected:

		hkbFootIkControlsModifier( const hkbFootIkControlsModifier& mod );

	public:

		///////////////////////////
		// hkbNode interface
		///////////////////////////

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

		// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		//////////////
		// properties
		//////////////

			/// The control data for an hkbFootIkModifier.
		struct hkbFootIkControlData m_controlData;

		/// The structure holds the output information for each leg.
		struct Leg
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Leg );
			HK_DECLARE_REFLECTION();

				/// Construct with initial values.
				/// 
				/// We use the same initial values as in hkaFootPlacementIkSolver::Setup.
			Leg()
				:	m_groundPosition(hkVector4::getZero()),
					m_verticalError(0.0f),
					m_hitSomething(false),
					m_isPlantedMS(false)
			{}

				/// The position of the ground below the foot, as computed by the foot IK raycasts.
			hkVector4 m_groundPosition; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")

				/// This event is sent if the foot raycast does not find a hit when the foot is supposed to be planted.
			class hkbEventProperty m_ungroundedEvent;	//+hk.Description("This event is sent if any of the foot raycasts do not find a hit.")

				/// The distance between the input foot height and the ground.
			hkReal m_verticalError; //+default(0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("The distance between the input foot height and the ground.")

				/// Whether or not the ground was hit by the raycast.
			hkBool m_hitSomething;	//+default(false)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("Whether or not the ground was hit by the raycast.")

				/// Whether or not the foot is planted in model space in the incoming animation. If the height 
				/// of the ankle goes below m_footPlantedAnkleHeightMS the foot is considered planted. 
			hkBool m_isPlantedMS;	//+default(false)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("Whether or not the foot is planted in model space in the incoming animation. If the height of the ankle goes below m_footPlantedAnkleHeightMS the foot is considered planted.")

			Leg( hkFinishLoadedObjectFlag flag ) : m_ungroundedEvent(flag) {}
		};

			/// The output properties of each leg of the character.
		hkArray< struct Leg > m_legs; //+hk.Description("The output properties of each leg of the character.")

	public:

		hkbFootIkControlsModifier( hkFinishLoadedObjectFlag flag );

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
