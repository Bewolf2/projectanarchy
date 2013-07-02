/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_HAND_IK_CONTROLS_MODIFIER_H
#define HKB_HAND_IK_CONTROLS_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Modifier/HandIk/hkbHandIkControlData.h>

extern const class hkClass hkbHandIkControlsModifierHandClass;

extern const class hkClass hkbHandIkControlsModifierClass;

	/// A modifier for putting hand IK control data into tracks to be used by the hkbHandIkModifier.
class hkbHandIkControlsModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbHandIkControlsModifier() : hkbModifier(HKB_NODE_TYPE_HAND_IK_CONTROLS_MODIFIER) {}

	protected:

		hkbHandIkControlsModifier( const hkbHandIkControlsModifier& mod );

	public:

		//////////////////////
		// hkbNode interface
		//////////////////////

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

			/// A pairing between a hand index and the control data for that hand.
		struct Hand
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbHandIkControlsModifier::Hand );
			HK_DECLARE_REFLECTION();

			Hand()
				:	m_handIndex(0),
					m_enable(true)
			{}

			Hand(hkFinishLoadedObjectFlag f) : m_controlData(f)
			{}

				/// The control data for this hand.
			struct hkbHandIkControlData m_controlData;	//+hk.Description("The control data for this hand.")

				/// The index of the hand.  This is an index into the array of hands defined in the hkbHandIkModifier.
			hkInt32 m_handIndex;	//+default(0)
									//+hk.RangeInt32(absmin=0,absmax=3)
									//+hk.Description("The index of the hand.  This is an index into the array of hands defined in the hkbHandIkModifier.")

				/// Enable this hand for IK. Make sure you provide valid data when this is set to true. 
			hkBool m_enable; //+default(false)
							 //+hk.Description("Enable this hand for IK.")
		};

		hkArray<struct Hand> m_hands;	//+hk.Description("The array of control data for each hand.")

	public:

		hkbHandIkControlsModifier( hkFinishLoadedObjectFlag flag );
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
