/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_NULL_PHYSICS_INTERFACE_H
#define HKB_NULL_PHYSICS_INTERFACE_H

#include <Behavior/Utilities/Physics/Interface/hkbPhysicsInterface.h>

	/// This is a null implementation of the physics interface.
	/// It's merely provided as a debugging tool. It will assert on any physics calls by the SDK.
class hkbNullPhysicsInterface : public hkbPhysicsInterface
{

public:

		
		
		//+serializable(false)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface Factory Methods
	//////////////////////////////////////////////////////////////////////////

	virtual hkbRagdollInterface* createRagdollInterface( const hkbCharacterSetup* characterSetup, const hkRootLevelContainer& rootContainer ) const HK_OVERRIDE;
	virtual hkbCharacterController* createCharacterController( const struct hkbCharacterControllerSetup& setup, const struct hkbCharacterControllerInstanceSetup& instanceSetup ) const HK_OVERRIDE;
	virtual hkbRagdollController* createRagdollController( const struct hkbRagdollControllerSetup& setup, const struct hkbRagdollControllerInstanceSetup& instanceSetup ) const HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbSpatialQueryInterface Methods
	//////////////////////////////////////////////////////////////////////////

	virtual hkBool castRay( const hkVector4& fromWS, const hkVector4& toWS, hkReal& hitFractionOut, hkVector4& normalWSOut ) HK_OVERRIDE;
	virtual hkBool castRay( const hkVector4& fromWS, const hkVector4& toWS, hkUint32 collisionFilterInfo, hkReal& hitFractionOut, hkVector4& normalWSOut ) HK_OVERRIDE;
	virtual void getNearbyRigidBodies(
		const hkVector4& sensorPosWS,
		hkReal maxDistance,
		hkUint32 collisionFilterInfo,
		hkArray<hkbRigidBodyHandle>& rbsOut ) const HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface Validation Methods
	//////////////////////////////////////////////////////////////////////////

	virtual hkBool isRigidBodyInWorld( hkbRigidBodyHandle handle ) const HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface Global Methods
	//////////////////////////////////////////////////////////////////////////

	virtual hkBool isCollisionEnabled( hkUint32 collisionFilterA, hkUint32 collisionFilterB ) const HK_OVERRIDE;
	virtual void getGravity( hkVector4& gravityOut ) const HK_OVERRIDE;
	virtual void step( hkReal timestep ) HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface Protected Impl calls
	//////////////////////////////////////////////////////////////////////////

protected:

	virtual void getRigidBodyCenterOfMassImpl( hkbRigidBodyHandle rigidBody, hkVector4& comOut ) const HK_OVERRIDE;
	virtual void setRigidBodyTransformImpl( hkbRigidBodyHandle rigidBody, const hkQsTransform& transform ) const HK_OVERRIDE;
	virtual void getRigidBodyTransformImpl( hkbRigidBodyHandle rigidBody, hkQsTransform& transformOut ) const HK_OVERRIDE;
	virtual void getRigidBodyFutureTransformImpl( hkbRigidBodyHandle rigidBody, hkReal timestep, hkQsTransform& futureTransformOut ) const HK_OVERRIDE;
	virtual const hkLocalFrame* getRigidBodyLocalFrameImpl( hkbRigidBodyHandle rigidBody ) const HK_OVERRIDE;
	virtual void setRigidBodyAngularVelocityImpl( hkbRigidBodyHandle rigidBody, hkVector4Parameter angularVelocity ) const HK_OVERRIDE;
	virtual void getRigidBodyAngularVelocityImpl( hkbRigidBodyHandle rigidBody, hkVector4& angularVelocityOut ) const HK_OVERRIDE;
	virtual void setRigidBodyLinearVelocityImpl( hkbRigidBodyHandle rigidBody, hkVector4Parameter linearVelocity ) const HK_OVERRIDE;
	virtual void getRigidBodyLinearVelocityImpl( hkbRigidBodyHandle rigidBody, hkVector4& linearVelocityOut ) const HK_OVERRIDE;
	virtual hkbRigidBodySetup::Type getRigidBodyTypeImpl( hkbRigidBodyHandle rigidBody ) const HK_OVERRIDE;
	virtual void setRigidBodyTypeImpl( hkbRigidBodyHandle rigidBody, hkbRigidBodySetup::Type type ) const HK_OVERRIDE;
	virtual hkUint32 getRigidBodyCollisionFilterInfoImpl( hkbRigidBodyHandle rigidBody ) const HK_OVERRIDE;
	virtual void setRigidBodyCollisionFilterInfoImpl( hkbRigidBodyHandle rigidBody, hkUint32 info ) const HK_OVERRIDE;
	virtual void disableRigidBodyCollisionImpl( hkbRigidBodyHandle attachee, hkbRigidBodyHandle attacher, int attacheeLayer ) const HK_OVERRIDE;
	virtual hkSimplePropertyValue getRigidBodyPropertyImpl( hkbRigidBodyHandle rigidBody, hkUint16 key ) const HK_OVERRIDE;
	virtual void setRigidBodyPropertyImpl( hkbRigidBodyHandle rigidBody, hkUint16 key, hkSimplePropertyValue value) const HK_OVERRIDE;
	virtual hkbConstraintHandle createConstraintImpl( const struct hkbConstraintSetup& setup, const struct hkbConstraintInstanceSetup& instanceSetup ) HK_OVERRIDE;
	virtual void removeConstraintImpl( hkbConstraintHandle constraint ) HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface/hkbSpatialQueryInterface Multithreaded Access Methods
	//////////////////////////////////////////////////////////////////////////

public:

	virtual void lock() HK_OVERRIDE;
	virtual void unlock() HK_OVERRIDE;

	hkbNullPhysicsInterface() {}
	hkbNullPhysicsInterface( hkFinishLoadedObjectFlag flag ) : hkbPhysicsInterface( flag ) { if ( flag.m_finishing ) { HK_ASSERT(0x22440074,false); } }

};

#endif // HKB_NULL_PHYSICS_INTERFACE_H

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
