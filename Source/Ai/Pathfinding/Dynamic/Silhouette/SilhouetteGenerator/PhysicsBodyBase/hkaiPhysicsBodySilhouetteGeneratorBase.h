/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_PHYSICS_BODY_SILHOUETTE_GENERATOR_BASE_H
#define HK_PHYSICS_BODY_SILHOUETTE_GENERATOR_BASE_H

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/PointCloud/hkaiPointCloudSilhouetteGenerator.h>

	/// An abstract base class for handling silhouette generation from rigid bodies.
	/// This can be used for Havok Physics (see Ai/PhysicsBridge/Silhouette/hkaiPhysicsBodySilhouetteGenerator.h),
	/// Or adapted to another physics engine.
class hkaiPhysicsBodySilhouetteGeneratorBase : public hkaiPointCloudSilhouetteGenerator
{
	public:
			//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

			/// Constructor
		hkaiPhysicsBodySilhouetteGeneratorBase();

			/// Constructor
		hkaiPhysicsBodySilhouetteGeneratorBase( hkFinishLoadedObjectFlag f);

			/// Destructor
		virtual ~hkaiPhysicsBodySilhouetteGeneratorBase();
		
		//
		// hkaiSilhouetteGenerator interface
		// 
		
			/// Update internal state - Method called automatically during silhouette updating, and therefore does not need to be called by the user each frame.
		virtual void update( hkVector4Parameter up ) HK_OVERRIDE;

			/// Whether or not the silhouette from this generator should be considered this frame on the given nav mesh section.
		virtual bool willGenerateSilhouettes( const hkaiNavMeshAccessor& meshInstance, hkVector4Parameter up ) const HK_OVERRIDE;

		//
		// hkaiPhysicsBodySilhouetteGeneratorBase-specific methods
		// 

			/// Returns the current velocity threshold used for velocity culling.
		inline hkSimdReal getVelocityThreshold() const;

			/// Sets the velocity threshold used for velocity culling.	
			/// When the projected velocity (in the non-up direction) is greater than this threshold, no silhouettes are produced. Default 3.0f m/s.
		inline void setVelocityThreshold( hkReal velThreshold );

		//
		// Virtual interface
		//

			/// Get the world-space rotation and translation of the rigid body.
		virtual void getBodyRotationAndTranslation( hkQuaternion& rotationOut, hkVector4& translationOut ) const = 0;

			/// Get the world-space linear velocity of the rigid body.
		virtual void getBodyLinearVelocity( hkVector4& velocityOut ) const = 0;

		//
		// Static utility methods
		//

		enum
		{
			NUM_SPHERE_VERTS = 14,
			NUM_CAPSULE_VERTS = 2*NUM_SPHERE_VERTS
		};

		static void HK_CALL getPointsForSphere( hkVector4Parameter p, hkSimdRealParameter radius, hkArray<hkVector4>::Temp& vertsOut);
		static void HK_CALL getPointsForCapsule( hkVector4Parameter p0, hkVector4Parameter p1, hkSimdRealParameter radius, hkArray<hkVector4>::Temp& vertsOut);

	protected:

		void updateFromBodyInfo( hkQuaternionParameter rot, hkVector4Parameter translation, hkVector4Parameter linearVel );

			/// Cached velocity of the body at the last update().
			/// Velocity threshold is stored in the w component.
		hkVector4 m_linearVelocityAndThreshold; //+default(0.0f 0.0f 0.0f 3.0f)
};

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/PhysicsBodyBase/hkaiPhysicsBodySilhouetteGeneratorBase.inl>

#endif // HK_PHYSICS_BODY_SILHOUETTE_GENERATOR_BASE_H

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
