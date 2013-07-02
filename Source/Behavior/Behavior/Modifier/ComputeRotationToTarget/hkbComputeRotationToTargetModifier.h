/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_COMPUTE_ROTATION_TO_TARGET_MODIFIER_H
#define HKB_COMPUTE_ROTATION_TO_TARGET_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbComputeRotationToTargetModifierClass;

	/// Deprecated.  Please use the hkbScriptGenerator instead.
	/// 
	/// Computes the rotation about a given axis that will face an object toward
	/// a given target.
class hkbComputeRotationToTargetModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbComputeRotationToTargetModifier();

	protected:

		hkbComputeRotationToTargetModifier( const hkbComputeRotationToTargetModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// The modified rotation.
		hkQuaternion m_rotationOut;	//+default(0,0,0,1)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("The modified rotation")

			/// The position of the facee/target.
		hkVector4 m_targetPosition;	//+default(0,0,0,0)
									//+hk.Description("The position of the facee/target")

			/// The current position of the facer.
		hkVector4 m_currentPosition;	//+default(0,0,0,0)
										//+hk.Description("The current position of the facer")

			/// The current rotation of the facer.
		hkQuaternion m_currentRotation;	//+default(0,0,0,0)
										//+hk.Description("The current rotation of the facer")

			/// The axis of rotation in the local space of the facer.
		hkVector4 m_localAxisOfRotation;	//+default(0,0,1,0)
											//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
											//+hk.Description("The axis of rotation in the local space of the facer")

			/// The facing direction in the local space of the facer.
		hkVector4 m_localFacingDirection;	//+default(1,0,0,0)
											//+hk.Description("The facing direction in the local space of the facer")

			/// The resulting transform is a delta from the initial.
		hkBool m_resultIsDelta;	//+default(false)
								//+hk.Description("The resulting transform is a delta from the original")

	protected:

		HKB_BEGIN_INTERNAL_STATE(0);

		// this is commented out on purpose to force the output properties to be part of the internal state
		//hkQuaternion m_rotationOut;

		HKB_END_INTERNAL_STATE();

	public:

		hkbComputeRotationToTargetModifier( hkFinishLoadedObjectFlag flag );
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
