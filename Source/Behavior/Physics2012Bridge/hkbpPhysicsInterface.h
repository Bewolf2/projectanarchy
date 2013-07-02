/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_PHYSICS_INTERFACE_H
#define HKBP_PHYSICS_INTERFACE_H

#include <Behavior/Utilities/Physics/Interface/hkbPhysicsInterface.h>
#include <Physics2012/Dynamics/World/hkpWorld.h>

class hkVisualDebugger;

	/// This is a Havok Physics2012 implementation of the hkbPhysicsInterface.
class hkbpPhysicsInterface : public hkbPhysicsInterface
{

	public:

			
			
			//+serializable(false)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			/// Create a Havok Physics2012 interface which wraps the provided hkpWorld.
			/// A jobQueue and jobThreadPool must be provided for multithreaded physics stepping.
			/// A physics interface without a world is still useful for certain serialization functions (Eg. createRagdollInstance).
		hkbpPhysicsInterface( hkpWorld* world = HK_NULL, hkJobQueue* jobQueue = HK_NULL, hkJobThreadPool* jobThreadPool = HK_NULL );

			/// Create a Physics2012 interface.
			/// An hkpWorld will be created from the provided cinfo.
			/// A jobQueue and jobThreadPool must be provided for multithreaded physics stepping.
		hkbpPhysicsInterface( hkpWorldCinfo& cinfo, hkJobQueue* jobQueue = HK_NULL, hkJobThreadPool* jobThreadPool = HK_NULL );

			// Dtor.
		virtual ~hkbpPhysicsInterface();

			/// Get the current hkpWorld wrapped by this physics interface.
		hkpWorld* getWorld() const { return m_world; }

			/// Add hkp default physics viewers to the visual debugger instance.
		static void addDefaultViewers( hkVisualDebugger* visualDebugger );

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface Factory Methods
	//////////////////////////////////////////////////////////////////////////

			/// Create an hkbpRagdollInterface from the ragdoll data in the provide root level container.
			/// Note: characterSetup can be null, it is provided only as extra information when available.
		virtual hkbRagdollInterface* createRagdollInterface( const hkbCharacterSetup* characterSetup, const hkRootLevelContainer& rootContainer ) const HK_OVERRIDE;
	
			/// Create an hkbpCharacterController (either proxy or rigid body) based on the provided setup information.
			/// This character controller is ready to be integrated.
		virtual hkbCharacterController* createCharacterController( const struct hkbCharacterControllerSetup& setup, const struct hkbCharacterControllerInstanceSetup& instanceSetup  ) const HK_OVERRIDE;

			/// Create an hkbpRagdollXController (either powered or rigid body) based on the provided setup information.
			/// This ragdoll controller is ready to be driven.
		virtual hkbRagdollController* createRagdollController( const struct hkbRagdollControllerSetup& setup, const struct hkbRagdollControllerInstanceSetup& instanceSetup ) const HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbSpatialQueryInterface Methods
	//////////////////////////////////////////////////////////////////////////

			/// Cast a ray into the hkpWorld.
		virtual hkBool castRay( const hkVector4& fromWS, const hkVector4& toWS, hkReal& hitFractionOut, hkVector4& normalWSOut ) HK_OVERRIDE;

			/// Cast a ray into the hkpWorld.
		virtual hkBool castRay( const hkVector4& fromWS, const hkVector4& toWS, hkUint32 collisionFilterInfo, hkReal& hitFractionOut, hkVector4& normalWSOut ) HK_OVERRIDE;

			/// Get nearby rigid bodies in the hkpWorld.
			/// This implementation calls hkpWorld::getClosestPoints() with a hkpSphereShape as input.  You may want to
			/// subclass hkbCharacter and implement this method in a way that is more efficient in your game.
			/// For example, you may want to use an hkpPhantom that follows the character around to collect
			/// nearby rigid bodies.  You may also want to assume that sensorPosWS is always near the character.
			/// But whether that assumption holds depends on how you configure your behavior graphs.
			/// (see hkbCharacter::setSpatialQueryInterface).
		virtual void getNearbyRigidBodies(
			const hkVector4& sensorPosWS,
			hkReal maxDistance,
			hkUint32 collisionFilterInfo,
			hkArray<hkbRigidBodyHandle>& rbsOut ) const HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface Validation Methods
	//////////////////////////////////////////////////////////////////////////

			/// Returns if the rigid body is in the hkpWorld.
		virtual hkBool isRigidBodyInWorld( hkbRigidBodyHandle handle ) const HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface Global Methods
	//////////////////////////////////////////////////////////////////////////

			/// Returns if collisions are enabled between the two filter infos.
		virtual hkBool isCollisionEnabled( hkUint32 collisionFilterA, hkUint32 collisionFilterB ) const HK_OVERRIDE;

			/// Get the gravity in the hkpWorld.
		virtual void getGravity( hkVector4& gravityOut ) const HK_OVERRIDE;

			/// Step the hkpWorld.
		virtual void step( hkReal timestep ) HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbPhysicsInterface Rigid Body / Constraint Access Methods
	//////////////////////////////////////////////////////////////////////////

	protected:

		virtual void getRigidBodyCenterOfMassImpl( hkbRigidBodyHandle rigidBody, hkVector4& comOut ) const HK_OVERRIDE;

		virtual void setRigidBodyTransformImpl( hkbRigidBodyHandle rigidBody, const hkQsTransform& transform) const HK_OVERRIDE;

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

	#if defined(HK_DEBUG_MULTI_THREADING)

			/// Mark the hkpWorld for read operations.
		virtual void markForRead() const HK_OVERRIDE;

			/// Mark the hkpWorld for write operations.
		virtual void markForWrite() HK_OVERRIDE;

			/// Unmark the hkpWorld for read operations.
		virtual void unmarkForRead() const HK_OVERRIDE;

			/// Unmark the hkpWorld for write operations.
		virtual void unmarkForWrite() HK_OVERRIDE;

	#endif
	
			/// Lock the hkpWorld.
		virtual void lock() HK_OVERRIDE;

			/// Unlock the hkpWorld.
		virtual void unlock() HK_OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// Internal functions for HavokAssembly
		//////////////////////////////////////////////////////////////////////////

		virtual void addEntity( hkbRigidBodyHandle );

		virtual void removeEntity( hkbRigidBodyHandle );

	protected:

			/// Common init for ctors.
		void init( hkpWorld* world, hkJobQueue* jobQueue, hkJobThreadPool* jobThreadPool );

			/// The hkpWorld instance for this physics interface.
		hkRefPtr<hkpWorld> m_world; //+nosave

			/// The jobQueue for multithreading.
		hkJobQueue* m_jobQueue; //+nosave

			/// The jobThreadPool for multithreading.
		hkJobThreadPool* m_jobThreadPool; //+nosave

	public:

		hkbpPhysicsInterface( hkFinishLoadedObjectFlag flag );

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
