/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_OBSTACLE_COLLECTOR_H
#define HK_AI_OBSTACLE_COLLECTOR_H

#include <Common/Base/Types/Geometry/Sphere/hkSphere.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>

#ifdef HK_PLATFORM_SPU
#	include <Common/Base/Container/LocalArray/hkLocalArray.h>
#endif

class hkaiReferenceFrame;

/// A utility class to encapsulate accumulating obstacles for a character.
/// The maximum number of obstacles are fixed -
/// if obstacles are added when the arrays at this limit, the furthest obstacle will be dropped
class hkaiObstacleCollector
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_STEERING, hkaiObstacleCollector );

	enum
	{
		MAX_NAV_MESH_BOUNDARIES = 16,

		MAX_NUM_BOUNDARY_OBSTACLES = 32,
		MAX_NUM_SPHERE_OBSTACLES = 32,
	};

	hkaiObstacleCollector(int numAgents, hkVector4Parameter position, hkVector4Parameter up);

	void addBoundary( hkVector4Parameter A, hkVector4Parameter B );
	void addSphereObstacle( const hkaiAvoidanceSolver::SphereObstacle& sphere);


	inline const hkArrayBase<hkaiAvoidanceSolver::SphereObstacle>& getSpheres() const { return m_spheres; }
	inline const hkArrayBase<hkaiAvoidanceSolver::BoundaryObstacle>& getBoundaries() const { return m_boundaries; }
	inline const hkArrayBase<hkaiAvoidanceSolver::ObstacleAgent>& getNeighborAgents() const { return m_neighborAgents; }

	inline hkArrayBase<hkaiAvoidanceSolver::SphereObstacle>& getSpheres() { return m_spheres; }
	inline hkArrayBase<hkaiAvoidanceSolver::BoundaryObstacle>& getBoundaries() { return m_boundaries; }
	inline hkArrayBase<hkaiAvoidanceSolver::ObstacleAgent>& getNeighborAgents() { return m_neighborAgents; }

	inline void setReferenceFrame( const hkaiReferenceFrame* refFrame ) { m_referenceFrame = refFrame; }

protected:

		/// Returns the distance between the character and the sphere
	hkSimdReal getSphereDistance( const hkaiAvoidanceSolver::SphereObstacle& sphere ) const;
		
		/// Returns the distance between the character and the boundary
	hkSimdReal getBoundaryDistance( hkVector4Parameter A, hkVector4Parameter B ) const;
	hkSimdReal getBoundaryDistance( const hkaiAvoidanceSolver::BoundaryObstacle& boundary ) const;

#ifdef HK_PLATFORM_SPU
	hkLocalArray<hkaiAvoidanceSolver::SphereObstacle> m_spheres;
	hkLocalArray<hkaiAvoidanceSolver::BoundaryObstacle> m_boundaries;
	hkLocalArray<hkaiAvoidanceSolver::ObstacleAgent> m_neighborAgents;
#else
	hkArray<hkaiAvoidanceSolver::SphereObstacle>::Temp m_spheres;
	hkArray<hkaiAvoidanceSolver::BoundaryObstacle>::Temp m_boundaries;
	hkArray<hkaiAvoidanceSolver::ObstacleAgent>::Temp m_neighborAgents;
#endif

	hkVector4 m_up;
	hkVector4 m_position;
	// Reference frame (possibly NULL) used to transform the input obstacles into character space
	const hkaiReferenceFrame* m_referenceFrame;

};
#endif // HK_AI_OBSTACLE_COLLECTOR_H

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
