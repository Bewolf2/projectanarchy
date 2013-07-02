/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_DAMPING_MODIFIER_H
#define HKB_DAMPING_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbDampingModifierClass;

	/// Deprecated.  Please use the hkbScriptGenerator instead.
	/// 
	/// This is a modifier that uses a PID controller to provide variable damping.
class hkbDampingModifier : public hkbModifier
{
	//+version(1)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Constructor.
		hkbDampingModifier();
		hkbDampingModifier( const hkbDampingModifier& );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		/////////////////////////
		// hkbModifier interface
		/////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

			/// The coefficient for the proportional component of the damping system.
		hkReal m_kP;					//+default(0.20f)
										//+hk.RangeReal(absmin=-3,absmax=3)
										//+hk.Description("Coefficient for the Proportional component of damping system. Larger Kp results in a faster response. Too large Kp can cause oscillation and other instabilities.")

			/// The coefficient for the integral component of the damping system.
		hkReal m_kI;					//+default(0.015f)
										//+hk.RangeReal(absmin=-3,absmax=3)
										//+hk.Description("Coefficient for the Integral component of damping system. A larger Ki eliminates errors more quickly, but can result in overcorrections.")

			/// The coefficient for the derivative component of the damping system.
		hkReal m_kD;					//+default(-0.10f)
										//+hk.RangeReal(absmin=-3,absmax=3)
										//+hk.Description("Coefficient for the Derivative component of damping system. A larger Kd decreases overshoot, but slows down response.")

			/// Enable/disable scalar damping.
		hkBool m_enableScalarDamping;	//+default(true)
										//+hk.Description("Enable/disable scalar damping")

			/// Enable/disable Vector4 damping.
		hkBool m_enableVectorDamping;	//+default(false)
										//+hk.Description("Enable/disable Vector4 damping")

			/// The value that is being damped.
		hkReal m_rawValue;				//+default(0.0f)
										//+hk.RangeReal(absmin=-10000,absmax=10000)
										//+hk.Description("The value that is being damped")

			/// The resulting damped value.
		hkReal m_dampedValue;			//+default(0.0f)
										//+hk.RangeReal(absmin=-10000,absmax=10000)
										//+hk.Description("The resulting damped value")
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")

			/// The vector being damped.
		hkVector4 m_rawVector;			//+default(0,0,0)
										//+hk.Description("The vector being damped")
		
			/// The resulting damped vector.
		hkVector4 m_dampedVector;		//+default(0,0,0)
										//+hk.Description("The vector being damped")
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

		// this is commented out on purpose to force the output properties to be part of the internal state
		//hkVector4 m_dampedVector;

			// The sum of errors so far for the damped vector.
		hkVector4 m_vecErrorSum;		//+default(0,0,0)
										//+hk.Description("The sum of errors so far for the damped vector.")

			// The previous error for the damped vector.
		hkVector4 m_vecPreviousError;	//+default(0,0,0)
										//+hk.Description("The previous error for the damped vector.")
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")

		// this is commented out on purpose to force the output properties to be part of the internal state
		//hkReal m_dampedValue;

			/// The sum of the errors so far.
		hkReal m_errorSum;				//+default(0.0f)
										//+hk.RangeReal(absmin=-100,absmax=100)
										//+hk.Description("The sum of the errors so far")
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
			/// The previous error.
		hkReal m_previousError;			//+default(0.0f)
										//+hk.RangeReal(absmin=-100,absmax=100)
										//+hk.Description("The previous error")
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")

		HKB_END_INTERNAL_STATE();

	public:

		hkbDampingModifier( hkFinishLoadedObjectFlag flag );

		friend class hkbDampingModifierUtils;
};

#endif // HKB_DAMPING_MODIFIER_H

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
