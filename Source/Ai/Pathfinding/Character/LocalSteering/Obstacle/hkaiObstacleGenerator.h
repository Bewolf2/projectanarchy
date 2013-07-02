/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_OBSTACLE_GENERATOR_H
#define HKAI_OBSTACLE_GENERATOR_H

#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>

class hkAabb;

	/// Creates and maintains arrays of obstacles for local steering.
class hkaiObstacleGenerator : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		hkaiObstacleGenerator();
		hkaiObstacleGenerator(hkFinishLoadedObjectFlag);

		virtual ~hkaiObstacleGenerator() {}

			/// Called during hkaiWorld::stepCharacters() and hkaiWorld::stepCharactersMT(), allowing the generator to update transforms, etc.
		virtual void update() {}

			/// Update any internal cached data. Should be called whenever the m_spheres or m_boundaries change;
		virtual void recalcCachedData() {}

			/// Sets aabbOut to the world-space AABB of the generator.
		virtual void getAabb(hkAabb& aabbOut) const = 0;

		virtual void shiftWorldSpace( hkVector4Parameter shift );

			/// Gets the sphere and line obstacles for the generator, and appends them to the collector
		void appendObstacles(const hkAabb& sensorAabb, class hkaiObstacleCollector& collectorInOut) const;

			/// Updates the sphere velocities for an obstacles that is moving with the specified linear and angular velocities.
			/// Note that the center of rotation is assumed to be the origin in local space.
		void updateSphereVelocities( hkVector4Parameter linearVelocity, hkVector4Parameter angularVelocity );

	public:

		hkBool m_useSpheres;
		hkBool m_useBoundaries;

		hkBool m_clipBoundaries;

		hkTransform m_transform;

		hkArray<hkaiAvoidanceSolver::SphereObstacle>	m_spheres;
		hkArray<hkaiAvoidanceSolver::BoundaryObstacle>	m_boundaries;

			/// User data.
		hkUlong m_userData;
};

#endif	// HKAI_OBSTACLE_GENERATOR_H

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
