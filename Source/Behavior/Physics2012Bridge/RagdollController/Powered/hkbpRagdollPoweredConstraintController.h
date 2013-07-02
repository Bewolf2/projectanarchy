/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_RAGDOLL_POWERED_CONTROLLER_H
#define HKBP_RAGDOLL_POWERED_CONTROLLER_H

#include <Behavior/Utilities/Physics/Interface/hkbRagdollController.h>

struct hkbPoweredRagdollControlData;
class hkpConstraintMotor;
class hkaRagdollInstance;
class hkbpRagdollInterface;

	/// This controller wraps an hkbpRagdollInterface and drives it using powered constraints.
class hkbpRagdollPoweredConstraintController : public hkbRagdollController
{
public:		

	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );		

		// Ctor.
	hkbpRagdollPoweredConstraintController( hkbpRagdollInterface* ragdollInterface );

		// Dtor.
	virtual ~hkbpRagdollPoweredConstraintController();

		// hkbRagdollController interface.
	virtual void driveToPose(
		const struct hkbRagdollControllerData& data,
		hkReal deltaTime ) HK_OVERRIDE;

protected:

		/// Updates all joint motors based on the state of the powered ragdoll control parameters.
	int updateJointMotors(
		const hkbPoweredRagdollControlData* poweredRagdollControlData,
		const hkInt32* poweredRagdollControlDataIndex,
		const hkReal* boneWeights );

		/// Sets the joint motor parameters based on the parameters provided.
	void setMotor( hkpConstraintMotor* motor, const hkbPoweredRagdollControlData& data, hkReal boneWeight ) const;

		/// Pointer to the ragdollInterface's underlying ragdoll.
	hkaRagdollInstance* m_ragdoll;
};

#endif // HKBP_RAGDOLL_POWERED_CONTROLLER_H

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
