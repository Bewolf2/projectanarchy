/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_RAGDOLL_INTERFACE
#define HKBP_RAGDOLL_INTERFACE

#include <Behavior/Utilities/Physics/Interface/hkbRagdollInterface.h>

class hkaSkeleton;
class hkaRagdollInstance;

extern const class hkClass hkbpRagdollInterfaceClass;

	/// Wraps an hkaRagdollInstance and exposes it to the Behavior runtime.
class hkbpRagdollInterface : public hkbRagdollInterface
{
	public:

			
			//+serializable(false)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR);

			// Ctor.
		hkbpRagdollInterface( hkaRagdollInstance* ragdoll );
			
			// Dtor.
		virtual ~hkbpRagdollInterface();

			/// Get the underlying hkaRagdollInstance.
		hkaRagdollInstance* getRagdoll() const { return m_ragdoll; }

			// hkbRagdollInterface
		virtual bool isInWorld() const HK_OVERRIDE;

			// hkbRagdollInterface
		virtual hkbRagdollInterface* clone() const HK_OVERRIDE;

			// hkbRagdollInterface
		virtual void scaleBy( hkReal scaleBy ) HK_OVERRIDE;

			// hkbRagdollInterface
		virtual hkbRigidBodyHandle getRigidBodyOfBone( int index ) const HK_OVERRIDE;

			// hkbRagdollInterface
		virtual hkbConstraintHandle getConstraintOfBone( int index ) const HK_OVERRIDE;

			// hkbRagdollInterface
		virtual void updateConstraints() HK_OVERRIDE;

			// hkbRagdollInterface
		virtual const hkaSkeleton* getSkeleton() const HK_OVERRIDE;

			// hkbRagdollInterface
		virtual void setSkeleton( hkaSkeleton* skeleton ) HK_OVERRIDE;

			// hkbRagdollInterface
		virtual void getPoseWorldSpace( hkQsTransform* poseWorldSpaceOut ) const HK_OVERRIDE;

			// hkbRagdollInterface
		virtual void setPoseWorldSpace( const hkQsTransform* poseWorldSpaceIn ) HK_OVERRIDE;

			// hkbRagdollInterface
		virtual void addToWorld( hkbPhysicsInterface* physicsInterface ) const HK_OVERRIDE;

			// hkbRagdollInterface
		virtual void removeFromWorld() const HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// Internal helpers used by Havok Assembly
	//////////////////////////////////////////////////////////////////////////

		hkaRagdollInstance* getRagdollData() const { return m_ragdoll; }

	protected:

			/// The underlying ragdoll.
		hkRefPtr<hkaRagdollInstance> m_ragdoll; //+nosave

	public:

		hkbpRagdollInterface( hkFinishLoadedObjectFlag flag );
};

#endif //HKBP_RAGDOLL_INTERFACE

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
