/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_PHYSICS_2012_SILHOUETTE_GENERATOR_H
#define HK_PHYSICS_2012_SILHOUETTE_GENERATOR_H

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/PhysicsBodyBase/hkaiPhysicsBodySilhouetteGeneratorBase.h>

#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>

class hkaiPhysics2012WorldListener;

	/// The rigid body silhouette generator produces convex silhouettes for each convex leaf shape in its shape hierarchy.
class hkaiPhysics2012BodySilhouetteGenerator : public hkaiPhysicsBodySilhouetteGeneratorBase
{
	public:

			//+version(2)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

			/// Constructor
		hkaiPhysics2012BodySilhouetteGenerator( const hkpRigidBody* rb, const hkaiPhysics2012WorldListener* listener = HK_NULL);
		hkaiPhysics2012BodySilhouetteGenerator( hkFinishLoadedObjectFlag f);

		void setRigidBody( const hkpRigidBody* rb );
		void setPointCloudFromShape( const hkpShape* shape );

			/// Destructor
		virtual ~hkaiPhysics2012BodySilhouetteGenerator();

		//
		// hkaiPhysicsBodySilhouetteGeneratorBase interface
		//

			/// Get the world-space rotation and translation of the rigid body.
		virtual void getBodyRotationAndTranslation( hkQuaternion& rotationOut, hkVector4& translationOut ) const HK_OVERRIDE;

			/// Get the world-space linear velocity of the rigid body.
		virtual void getBodyLinearVelocity( hkVector4& velocityOut ) const HK_OVERRIDE;

		//
		// hkaiPhysics2012BodySilhouetteGenerator-specific methods
		// 

			/// Get body
		inline const hkpRigidBody* getRigidBody() const;

			/// Get the hkaiPhysics2012WorldListener that created this silhouette
		inline const hkaiPhysics2012WorldListener* getWorldListener() const;

		inline void setWorldListener( const hkaiPhysics2012WorldListener* listerner );

	protected:

		void addWorldShift( hkVector4& translationInOut ) const;

			/// Rigid body
		hkRefPtr<const hkpRigidBody> m_rigidBody;

		const hkaiPhysics2012WorldListener* m_physicsWorldListener; //+nosave
};

#include <Ai/Physics2012Bridge/Silhouette/hkaiPhysics2012BodySilhouetteGenerator.inl>

#endif // HK_PHYSICS_2012_SILHOUETTE_GENERATOR_H

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
