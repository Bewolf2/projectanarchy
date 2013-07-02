/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SPATIAL_QUERY_INTERFACE_H
#define HKB_SPATIAL_QUERY_INTERFACE_H

#include <Animation/Animation/Ik/FootPlacement/hkaFootPlacementIkSolver.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>
#include <Common/Base/DebugUtil/MultiThreadCheck/hkMultiThreadCheck.h>

// We do this so that marks can get properly compiled out when multithreading is disabled
#if defined(HK_DEBUG_MULTI_THREADING)
#define HKB_VIRTUAL_MT_CHECK_FUNC virtual
#define HKB_VIRTUAL_MT_CHECK_OVERRIDE HK_OVERRIDE
#else
#define HKB_VIRTUAL_MT_CHECK_FUNC
#define HKB_VIRTUAL_MT_CHECK_OVERRIDE
#endif

	/// This interface provides and interface for Behavior to execute spatial queries.
	/// These can occur at the global level using the hkbWorld's hkbPhysicsInterface (the default) or specialized per-character
	/// (see hkbCharacter::setSpatialQueryInterface).
	
class hkbSpatialQueryInterface : public hkaRaycastInterface
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbSpatialQueryInterface );

		/// Returns all of the rigid bodies in the world that are within maxDistance of sensorPosWS.
	virtual void getNearbyRigidBodies(
		const hkVector4& sensorPosWS,
		hkReal maxDistance,
		hkUint32 collisionFilterInfo,
		hkArray<hkbRigidBodyHandle>& rbsOut ) const = 0;

		/// Behavior is accessing the spatial query interface.
		/// This is done during a single threaded operation of Behavior. However, we call this function as an opportunity
		/// for debug checks against other threads which may be changing any shared data (Eg. Physics threads).
	HKB_VIRTUAL_MT_CHECK_FUNC void markForRead() const {}

		/// Behavior is done accessing the spatial query interface for reading.
		/// (see also markForRead).
	HKB_VIRTUAL_MT_CHECK_FUNC void unmarkForRead() const {}

		/// Behavior is accessing the spatial query interface in a multithreaded environment.
		/// Any other locks requested for the interface should wait until unlock is called.
	virtual void lock() = 0;

		/// Behavior is releasing the spatial query interface in a multithreaded environment.
		/// The next pending lock requested for the interface should be able to proceed.
	virtual void unlock() = 0;
};

#endif // HKB_SPATIAL_QUERY_INTERFACE_H

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
