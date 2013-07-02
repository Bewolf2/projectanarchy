/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_RAGDOLL_INTERFACE
#define HKB_RAGDOLL_INTERFACE

#include <Common/Base/Container/LocalArray/hkLocalBuffer.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

extern const class hkClass hkbRagdollInterfaceClass;
class hkbPhysicsInterface;

	/// This is an abstraction into an underlying ragdoll instance for use by the Behavior Runtime.
class hkbRagdollInterface : public hkReferencedObject
{
	public:

		
		
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			/// Returns if the ragdoll is in the physics world.
		virtual bool isInWorld() const = 0;

			/// Cloning - it returns a new hkbRagdollInterface cloned from this one.
			/// This method should deep clone any data necessary for driving constraints independently (Eg. powered constraints)
		virtual hkbRagdollInterface* clone() const = 0;

			/// This method should scale the ragdoll by the specified amount
		virtual void scaleBy( hkReal scaleBy ) = 0;

		//////////////////////////////////////////////////////////////////////////
		// Accessor Methods
		//////////////////////////////////////////////////////////////////////////

			/// Number of bones in the ragdoll.
		hkInt16 getNumBones() const;

			/// Access the rigid body associated with the i-th bone.
		virtual hkbRigidBodyHandle getRigidBodyOfBone( int index ) const = 0;

			/// Find the index of the rigid body (simple linear search)
		int getBoneIndexOfRigidBody( hkbRigidBodyHandle rb ) const;

			/// Access the constraint associated with the i-th bone. The constraint associated with a bone is the constraint that links that bone with its parent.
			/// Notice, therefore, that getConstraint(0) returns HK_NULL.  This is currently unused.
		virtual hkbConstraintHandle getConstraintOfBone( int index ) const = 0;

			/// This is called after a number of rigid bodys have had their motion types changed.
			/// Implementations will often want to disable constraints between fixed/keyframed bodies to avoid system overhead.
		virtual void updateConstraints() = 0;

			/// Access the associated skeleton, this should be the same pointer as used by any associated hkaSkeletonMappers
		virtual const hkaSkeleton* getSkeleton() const = 0;

			/// If scaling is used, a new skeleton will be set for the ragdoll after creation
		virtual void setSkeleton( hkaSkeleton* skeleton ) = 0;

			/// Gets the current pose of the rag doll in world space.
			/// poseWorldSpaceOut will be of size getNumBones().
		virtual void getPoseWorldSpace( hkQsTransform* poseWorldSpaceOut ) const = 0;

			/// Sets the current pose of the rag doll using a pose in world space
			/// poseWorldSpaceIn will be of size getNumBones().
		virtual void setPoseWorldSpace( const hkQsTransform* poseWorldSpaceIn ) = 0;

			/// Gets the current pose of the rag doll in model space.
		void getPoseModelSpace( hkQsTransform* poseModelSpaceOut, const hkQsTransform& worldFromModel ) const;

			/// Sets the current pose of the rag doll using a pose in model space
		void setPoseModelSpace(	hkQsTransform* poseModelSpaceIn, const hkQsTransform& worldFromModel );

		//////////////////////////////////////////////////////////////////////////
		// World Add/Remove Methods
		//////////////////////////////////////////////////////////////////////////

			/// Add to world
		virtual void addToWorld( hkbPhysicsInterface* physicsInterface ) const = 0;

			/// Removes from world
		virtual void removeFromWorld() const = 0;

	public:

		hkbRagdollInterface() {}
		hkbRagdollInterface( hkFinishLoadedObjectFlag flag ) : hkReferencedObject( flag ) { if ( flag.m_finishing ) { HK_ASSERT(0x22440074,false); } }
};

#include <Behavior/Utilities/Physics/Interface/hkbRagdollInterface.inl>

#endif //HKB_RAGDOLL_INTERFACE

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
