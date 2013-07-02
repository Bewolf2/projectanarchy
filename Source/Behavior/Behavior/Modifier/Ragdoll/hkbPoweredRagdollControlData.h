/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_POWERED_RAGDOLL_CONTROL_DATA_H
#define HKB_POWERED_RAGDOLL_CONTROL_DATA_H


extern const class hkClass hkbPoweredRagdollControlDataClass;

/// The controls that influence the behavior of an hkbPoweredRagdollModifier.
struct hkbPoweredRagdollControlData
{
	// +version(3)

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbPoweredRagdollControlData );
	HK_DECLARE_REFLECTION();
	
	hkbPoweredRagdollControlData()
		:	m_maxForce(50.0f),
			m_tau(0.8f),
			m_damping(1.0f),
			m_proportionalRecoveryVelocity(2.0f),
			m_constantRecoveryVelocity(1.0f){}

		/// The maximum force to apply.
	HK_ALIGN16(hkReal m_maxForce); //+default(50.0f)+hk.RangeReal(absmin=0.0,absmax=1000.0)+hk.Description("The maximum force applied to each joint.")

		/// The relative motor stiffness between 0 and 1.
	hkReal m_tau; //+default(0.8f)+hk.RangeReal(absmin=0.0,absmax=1.0)+hk.Description("The stiffness of the motors. Larger numbers will result in faster, more abrupt, convergence.")

		/// The relative motor damping between 0 and 1.
	hkReal m_damping; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=1.0)+hk.Description("The damping of the motors.")

		/// A factor to the current error to calculate the recovery velocity of the motors.
	hkReal m_proportionalRecoveryVelocity; //+default(2.0f)+hk.RangeReal(absmin=0.0,absmax=100.0)+hk.Description("This term is multipled by the error of the motor to compute a velocity for the motor to move it toward zero error.")

		/// A constant velocity which is used by the motors to recover from errors.
	hkReal m_constantRecoveryVelocity; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0)+hk.Description("This term is added to the velocity of the motor to move it toward zero error.")		

public:

	hkbPoweredRagdollControlData( hkFinishLoadedObjectFlag flag ) {}

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
