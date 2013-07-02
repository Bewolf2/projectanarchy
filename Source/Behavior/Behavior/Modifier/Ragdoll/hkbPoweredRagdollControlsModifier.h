/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_POWERED_RAGDOLL_CONTROLS_MODIFIER_H
#define HKB_POWERED_RAGDOLL_CONTROLS_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Modifier/Ragdoll/hkbPoweredRagdollControlData.h>
#include <Behavior/Behavior/Modifier/Ragdoll/hkbWorldFromModelModeData.h>
#include <Behavior/Behavior/Character/hkbBoneIndexArray.h>
#include <Behavior/Behavior/Character/hkbBoneWeightArray.h>

extern const class hkClass hkbPoweredRagdollControlsModifierClass;

/// Produces control data used by hkbPoweredRagdollModifier and puts it into
/// the tracks of the output when modify() is called.  This data is passed 
/// through the behavior tree and can be blended by blend nodes and during transitions.
class hkbPoweredRagdollControlsModifier : public hkbModifier
{
	public:
		// +version(6)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbPoweredRagdollControlsModifier();

	protected:

		hkbPoweredRagdollControlsModifier( const hkbPoweredRagdollControlsModifier& mod );

	public:

		~hkbPoweredRagdollControlsModifier();

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		/////////////////////////
		// hkbModifier interface
		/////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		//////////////
		// properties
		//////////////

			/// The control data for an hkbPoweredRagdollModifier.
		struct hkbPoweredRagdollControlData m_controlData; //+hk.Description("The control parameters that influence the Powered Ragdoll Modifier.")

					/// The bones to be driven by the rigid body ragdoll controller.  If this is empty, all bones will be driven.
		hkRefPtr<hkbBoneIndexArray> m_bones;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_RAGDOLL")
												//+hk.Description("The bones to be driven by the rigid body ragdoll controller.  If this is empty, all bones will be driven.")	

			/// How to process the world-from-model.
		struct hkbWorldFromModelModeData m_worldFromModelModeData; //+hk.Description("How to process the world-from-model.")

			/// DEPRECATED: A weight for each bone of the ragdoll.
		hkRefPtr<hkbBoneWeightArray> m_boneWeights;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_RAGDOLL|FLAG_NOT_VARIABLE")
													//+hk.Description("DEPRECATED: The bone weights that determine what fraction of the maximum force to apply to each bone.")

			/// How much to blend in the animation pose with the ragdoll pose. Set to 0 for only ragdoll and 1 for only animation.
		hkReal m_animationBlendFraction;	//+default(0.0f)
												//+hk.RangeReal(absmin=0.0,absmax=1.0)
												//+hk.Description("How much to blend in the animation pose with the ragdoll pose. Set to 0 for only ragdoll and 1 for only animation.")

	public:

		hkbPoweredRagdollControlsModifier( hkFinishLoadedObjectFlag flag );

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
