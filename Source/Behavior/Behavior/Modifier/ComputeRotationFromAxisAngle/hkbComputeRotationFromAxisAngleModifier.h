/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER_H
#define HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbComputeRotationFromAxisAngleModifierClass;

	/// Deprecated.  Please use the hkbScriptGenerator instead.
	/// 
	/// This modifier converts an axis and angle into a quaternion.
class hkbComputeRotationFromAxisAngleModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbComputeRotationFromAxisAngleModifier();

	protected:

		hkbComputeRotationFromAxisAngleModifier( const hkbComputeRotationFromAxisAngleModifier& mod );

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


			/// The output rotation.
		hkQuaternion m_rotationOut;	//+default(0,0,0,1)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("The output rotation")

			/// The axis of rotation.
		hkVector4 m_axis;	//+default(1,0,0,0)
							//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
							//+hk.Description("The axis of rotation")

			/// The number of degrees to rotate.
		hkReal m_angleDegrees;	//+default(0.0f)
								//+hk.RangeReal(absmin=-360,absmax=360)
								//+hk.Description("The number of degrees to rotate")

	protected:

		HKB_BEGIN_INTERNAL_STATE(0);

		// this is commented out on purpose to force the output properties to be part of the internal state
		//hkQuaternion m_rotationOut;

		HKB_END_INTERNAL_STATE();

	public:

		hkbComputeRotationFromAxisAngleModifier( hkFinishLoadedObjectFlag flag );
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
