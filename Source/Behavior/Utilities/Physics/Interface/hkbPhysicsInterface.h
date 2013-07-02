/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PHYSICS_INTERFACE_H
#define HKB_PHYSICS_INTERFACE_H

#include <Common/Base/Types/Properties/hkSimpleProperty.h>
#include <Common/Base/Object/hkReferencedObject.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>
#include <Behavior/Utilities/Physics/Interface/hkbSpatialQueryInterface.h>

class hkRootLevelContainer;
class hkbCharacterSetup;
class hkbCharacter;
class hkbContext;

	
	/// This is an abstraction into an underlying physics system for use by the Behavior Runtime.
class hkbPhysicsInterface : public hkReferencedObject, public hkbSpatialQueryInterface
{

public:

		
		
		//+serializable(false)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

	//////////////////////////////////////////////////////////////////////////
	// Factory methods
	//////////////////////////////////////////////////////////////////////////

		/// This is called during loading time. Any underlying data should not be cleaned up unless the hkbRagdollInterface is deleted.
		/// Note: characterSetup can be null, it is provided only as extra information when available.
	virtual hkbRagdollInterface* createRagdollInterface( const hkbCharacterSetup* characterSetup, const hkRootLevelContainer& rootContainer ) const = 0;

		/// This is called during character driving. The returned controller should be light-weight as it is created/destroyed as needed by the behavior graph.
		/// Any underlying data should not be cleaned up unless the hkbRagdollInterface is deleted.
	virtual hkbCharacterController* createCharacterController( const struct hkbCharacterControllerSetup& setup, const struct hkbCharacterControllerInstanceSetup& instanceSetup ) const = 0;

		/// This is called during ragdoll driving. The returned controller should be light-weight as it is created/destroyed as needed by the behavior graph.
		/// Any underlying data should not be cleaned up unless the hkbRagdollInterface is deleted.
	virtual hkbRagdollController* createRagdollController( const struct hkbRagdollControllerSetup& setup, const struct hkbRagdollControllerInstanceSetup& instanceSetup ) const = 0;

	//////////////////////////////////////////////////////////////////////////
	// hkbSpatialQueryInterface
	//////////////////////////////////////////////////////////////////////////

		/// Given two points, "from" and "to", specified in world space, castRay should return whether a ray between those two points
		/// intersects the scene or not. If it does (if it returns true), the hitFractionOut parameter should return a value between (0..1)
		/// to specify the point of intersection with the ray (where 0 is the start ("from") of the ray and 1 the end ("to") of the ray).
		/// Also in that case the parameter normalWSOut should return the normal of the surface intersected, in world space.
	virtual hkBool castRay( const hkVector4& fromWS, const hkVector4& toWS, hkReal& hitFractionOut, hkVector4& normalWSOut ) HK_OVERRIDE = 0;

		/// This interface is used instead of the above if collision filter info is available.
		/// In the Behavior SDK, this is always the case.  The above function is only used by the Demos ATM.
	virtual hkBool castRay( const hkVector4& fromWS, const hkVector4& toWS, hkUint32 collisionFilterInfo, hkReal& hitFractionOut, hkVector4& normalWSOut ) HK_OVERRIDE = 0;

	//////////////////////////////////////////////////////////////////////////
	// Validation Methods
	//////////////////////////////////////////////////////////////////////////

		/// Returns true if the rigid body is in the physics world.
	virtual hkBool isRigidBodyInWorld( hkbRigidBodyHandle handle ) const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Global Methods
	//////////////////////////////////////////////////////////////////////////

		/// Returns true if collisions are enabled between the provided filter infos.
	virtual hkBool isCollisionEnabled( hkUint32 collisionFilterA, hkUint32 collisionFilterB ) const = 0;

		/// Get the gravity of the physics world.
	virtual void getGravity( hkVector4& gravityOut ) const = 0;

		/// Step the physics world forward by timestep.
	virtual void step( hkReal timestep ) = 0;

		/// Called at the end of the Behavior frame. Useful for cleaning up resources only needed during Behavior-access.
	virtual void endFrame() {}

	//////////////////////////////////////////////////////////////////////////
	// Rigid Body / Constraint Access
	//////////////////////////////////////////////////////////////////////////

		/// Returns the COM of the rigid body in local space.
	HK_FORCE_INLINE void getRigidBodyCenterOfMass( hkbRigidBodyHandle rigidBody, hkVector4& comOut ) const
	{
		HK_ASSERT2(0x454af73b, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		getRigidBodyCenterOfMassImpl( rigidBody, comOut );
	}

		/// Set the transform of the rigid body.
	HK_FORCE_INLINE void setRigidBodyTransform( hkbRigidBodyHandle rigidBody, const hkQsTransform& transform ) const
	{
		HK_ASSERT2(0x4d5a2a59, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		setRigidBodyTransformImpl( rigidBody, transform );
	}

		/// Get the transform of the rigid body.
	HK_FORCE_INLINE void getRigidBodyTransform( hkbRigidBodyHandle rigidBody, hkQsTransform& transformOut ) const
	{
		HK_ASSERT2(0x78ec1f3b, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		getRigidBodyTransformImpl( rigidBody, transformOut );
		HK_ASSERT2(0x22440057, transformOut.getScale().allExactlyEqual<4>( hkVector4::getConstant<HK_QUADREAL_1>() ), "Rigid bodies should not have scale.");
	}

		/// Get the potential future transform of the rigid body after timestep.
	HK_FORCE_INLINE void getRigidBodyFutureTransform( hkbRigidBodyHandle rigidBody, hkReal timestep, hkQsTransform& futureTransformOut ) const
	{
		HK_ASSERT2(0x45b2199a, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		getRigidBodyFutureTransformImpl( rigidBody, timestep, futureTransformOut );
		HK_ASSERT2(0x22440058, futureTransformOut.getScale().allExactlyEqual<4>( hkVector4::getConstant<HK_QUADREAL_1>() ), "Rigid bodies should not have scale.");
	}

		/// Get the local frame attached to the rigid body if available, otherwise returns HK_NULL.
	HK_FORCE_INLINE const hkLocalFrame* getRigidBodyLocalFrame( hkbRigidBodyHandle rigidBody ) const
	{
		HK_ASSERT2(0x568b0d0b, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		return getRigidBodyLocalFrameImpl( rigidBody );
	}

		/// Set the angular velocity of the rigid body.
	HK_FORCE_INLINE void setRigidBodyAngularVelocity( hkbRigidBodyHandle rigidBody, hkVector4Parameter angularVelocity ) const
	{
		HK_ASSERT2(0x7fc6cff6, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		setRigidBodyAngularVelocityImpl( rigidBody, angularVelocity );
	}

		/// Get the angular velocity of the rigid body.
	HK_FORCE_INLINE void getRigidBodyAngularVelocity( hkbRigidBodyHandle rigidBody, hkVector4& angularVelocityOut ) const
	{
		HK_ASSERT2(0x38ee8d43, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		getRigidBodyAngularVelocityImpl( rigidBody, angularVelocityOut );
	}

		/// Set the linear velocity of the rigid body.
	HK_FORCE_INLINE void setRigidBodyLinearVelocity( hkbRigidBodyHandle rigidBody, hkVector4Parameter linearVelocity ) const
	{
		HK_ASSERT2(0x4ac8a6c, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		setRigidBodyLinearVelocityImpl( rigidBody, linearVelocity );
	}

		/// Get the linear velocity of the rigid body.
	HK_FORCE_INLINE void getRigidBodyLinearVelocity( hkbRigidBodyHandle rigidBody, hkVector4& linearVelocityOut ) const
	{
		HK_ASSERT2(0x4865d106, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		getRigidBodyLinearVelocityImpl( rigidBody, linearVelocityOut );
	}

		/// Get the type of the rigid body.
	HK_FORCE_INLINE hkbRigidBodySetup::Type getRigidBodyType( hkbRigidBodyHandle rigidBody ) const
	{
		HK_ASSERT2(0x7251d5d2, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		return getRigidBodyTypeImpl( rigidBody );
	}

		/// Set the type of the rigid body. 
	HK_FORCE_INLINE void setRigidBodyType( hkbRigidBodyHandle rigidBody, hkbRigidBodySetup::Type type ) const
	{
		HK_ASSERT2(0x752e8991, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		setRigidBodyTypeImpl( rigidBody, type );
	}

		/// Get the collision filter info for the rigid body.
	HK_FORCE_INLINE hkUint32 getRigidBodyCollisionFilterInfo( hkbRigidBodyHandle rigidBody ) const
	{
		HK_ASSERT2(0x1df9534a, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		return getRigidBodyCollisionFilterInfoImpl( rigidBody );
	}

		/// Set the collision filter info for the rigid body.
	HK_FORCE_INLINE void setRigidBodyCollisionFilterInfo( hkbRigidBodyHandle rigidBody, hkUint32 info ) const
	{
		HK_ASSERT2(0xed0e831, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		setRigidBodyCollisionFilterInfoImpl( rigidBody, info );
	}

		/// Disable collisions between the two rigid bodies which are becoming attached to each other.
	HK_FORCE_INLINE void disableRigidBodyCollision( hkbRigidBodyHandle attachee, hkbRigidBodyHandle attacher, int attacheeLayer ) const
	{
		HK_ASSERT2(0x53933b15, attachee != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		disableRigidBodyCollisionImpl( attachee, attacher, attacheeLayer );
	}

		/// Get an arbitrary property associated with the rigid body.
	HK_FORCE_INLINE hkSimplePropertyValue getRigidBodyProperty( hkbRigidBodyHandle rigidBody, hkUint16 key ) const
	{
		HK_ASSERT2(0x1cac0875, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		return getRigidBodyPropertyImpl( rigidBody, key );
	}

		/// Set an arbitrary property associated with the rigid body.  These must be retrievable for the lifetime of the rigid body.
	HK_FORCE_INLINE void setRigidBodyProperty( hkbRigidBodyHandle rigidBody, hkUint16 key, hkSimplePropertyValue value) const
	{
		HK_ASSERT2(0x5d5db501, rigidBody != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated rigid body handles.");
		setRigidBodyPropertyImpl( rigidBody, key, value );
	}

	//////////////////////////////////////////////////////////////////////////
	// Constraints
	//////////////////////////////////////////////////////////////////////////

		/// Create a constraint between two rigid bodies and add it to the physics world.
	HK_FORCE_INLINE hkbConstraintHandle createConstraint( const struct hkbConstraintSetup& setup, const struct hkbConstraintInstanceSetup& instanceSetup )
	{
		HK_ASSERT2(0x531298a9, ( instanceSetup.m_rigidBodyA != HKB_INVALID_PHYSICS_HANDLE ) && ( instanceSetup.m_rigidBodyB != HKB_INVALID_PHYSICS_HANDLE ), "This function should not be called on non-validated rigid body handles.");
		return createConstraintImpl( setup, instanceSetup );
	}

		/// Remove a constraint from the physics world.
	HK_FORCE_INLINE void removeConstraint( hkbConstraintHandle constraint )
	{
		HK_ASSERT2(0x696d8e0a, constraint != HKB_INVALID_PHYSICS_HANDLE, "This function should not be called on non-validated constraint handles.");
		removeConstraintImpl( constraint );
	}

	//////////////////////////////////////////////////////////////////////////
	// Protected Impl calls (see non-Impl versions for descriptions)
	//////////////////////////////////////////////////////////////////////////

protected:

	virtual void getRigidBodyCenterOfMassImpl( hkbRigidBodyHandle rigidBody, hkVector4& comOut ) const = 0;

	virtual void setRigidBodyTransformImpl( hkbRigidBodyHandle rigidBody, const hkQsTransform& transform ) const = 0;

	virtual void getRigidBodyTransformImpl( hkbRigidBodyHandle rigidBody, hkQsTransform& transformOut ) const = 0;
	
	virtual void getRigidBodyFutureTransformImpl( hkbRigidBodyHandle rigidBody, hkReal timestep, hkQsTransform& futureTransformOut ) const { getRigidBodyTransform( rigidBody, futureTransformOut ); }
	
	virtual const hkLocalFrame* getRigidBodyLocalFrameImpl( hkbRigidBodyHandle rigidBody ) const { return HK_NULL; }
	
	virtual void setRigidBodyLinearVelocityImpl( hkbRigidBodyHandle rigidBody, hkVector4Parameter linearVelocity ) const = 0;

	virtual void getRigidBodyLinearVelocityImpl( hkbRigidBodyHandle rigidBody, hkVector4& linearVelocityOut ) const = 0;

	virtual void setRigidBodyAngularVelocityImpl( hkbRigidBodyHandle rigidBody, hkVector4Parameter angularVelocity ) const = 0;

	virtual void getRigidBodyAngularVelocityImpl( hkbRigidBodyHandle rigidBody, hkVector4& angularVelocityOut ) const = 0;
	
	virtual hkbRigidBodySetup::Type getRigidBodyTypeImpl( hkbRigidBodyHandle rigidBody ) const = 0;
	
	virtual void setRigidBodyTypeImpl( hkbRigidBodyHandle rigidBody, hkbRigidBodySetup::Type type ) const = 0;
	
	virtual hkUint32 getRigidBodyCollisionFilterInfoImpl( hkbRigidBodyHandle rigidBody ) const = 0;
	
	virtual void setRigidBodyCollisionFilterInfoImpl( hkbRigidBodyHandle rigidBody, hkUint32 info ) const = 0;

	virtual void disableRigidBodyCollisionImpl( hkbRigidBodyHandle attachee, hkbRigidBodyHandle attacher, int attacheeLayer ) const = 0;
	
	virtual hkSimplePropertyValue getRigidBodyPropertyImpl( hkbRigidBodyHandle rigidBody, hkUint16 key ) const = 0;
	
	virtual void setRigidBodyPropertyImpl( hkbRigidBodyHandle rigidBody, hkUint16 key, hkSimplePropertyValue value) const = 0;

	virtual hkbConstraintHandle createConstraintImpl( const struct hkbConstraintSetup& setup, const struct hkbConstraintInstanceSetup& instanceSetup ) = 0;

	virtual void removeConstraintImpl( hkbConstraintHandle constraint ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Multithreaded Access (see also hkbSpatialQueryInterface)
	//////////////////////////////////////////////////////////////////////////

public:

		/// Behavior is accessing the physics interface to make changes to physics data.
		/// This is done during a single threaded operation of Behavior. However, we call this function as an opportunity
		/// for debug checks against other threads which may be accessing any shared data (Eg. Physics threads).
	HKB_VIRTUAL_MT_CHECK_FUNC void markForWrite() {}

		/// Behavior is done accessing the physics interface for writing.
		/// (see also markForWrite)
	HKB_VIRTUAL_MT_CHECK_FUNC void unmarkForWrite() {}

		/// Behavior is accessing the physics interface to read physics data.
		/// This is done during a single threaded operation of Behavior. However, we call this function as an opportunity
		/// for debug checks against other threads which may be changing any shared data (Eg. Physics threads).
		/// (see also hkbSpatialQueryInterface::markForRead)
	HKB_VIRTUAL_MT_CHECK_FUNC void markForRead() const HKB_VIRTUAL_MT_CHECK_OVERRIDE {}

		/// Behavior is done accessing the physics interface for reading.
		/// (see also markForRead)
		/// (see also hkbSpatialQueryInterface::unmarkForRead)
	HKB_VIRTUAL_MT_CHECK_FUNC void unmarkForRead() const HKB_VIRTUAL_MT_CHECK_OVERRIDE {}
		
		/// Behavior is accessing the physics interface for read/write in a multithreaded environment.
		/// Any other locks requested for the interface should wait until unlock is called.
		/// Any other requests to physics data should wait until unlock is called.
		/// (see also hkbSpatialQueryInterface::lock)

	virtual void lock() HK_OVERRIDE = 0;

		/// Behavior is releasing the physics interface for read/write in a multithreaded environment.
		/// The next pending lock requested for the interface should be able to proceed.
		/// (see also lock)
		/// (see also hkbSpatialQueryInterface::unlock)
	virtual void unlock() HK_OVERRIDE = 0;

	hkbPhysicsInterface(){}
	hkbPhysicsInterface( hkFinishLoadedObjectFlag flag ) : hkReferencedObject( flag ) { if ( flag.m_finishing ) { HK_ASSERT(0x22440074,false); } }
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
