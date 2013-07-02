/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_SIMPLE_OBSTACLE_GENERATOR_H
#define HKAI_SIMPLE_OBSTACLE_GENERATOR_H

#include <Ai/Pathfinding/Character/LocalSteering/Obstacle/hkaiObstacleGenerator.h>

	/// Basic obstacle generator implementation.
class hkaiSimpleObstacleGenerator : public hkaiObstacleGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		hkaiSimpleObstacleGenerator();
		hkaiSimpleObstacleGenerator(hkFinishLoadedObjectFlag f) : hkaiObstacleGenerator(f) { }

			/// Gets the world-space AABB. This is computed by transforming the local-space AABB by hkaiObstacleGenerator::m_transform.
		virtual void getAabb(hkAabb& aabbOut) const HK_OVERRIDE;

			/// Calls calcAabb to update m_localAabb. 
		virtual void recalcCachedData() HK_OVERRIDE;

			/// Helper function to set m_localAabb from internal obstacle data. This should be called whenever the obstacles are modified.
		void calcAabb();

			/// Checks that the local-space AABB correctly contains all the obstacles. Asserts in debug builds but does nothing in release.
		void _checkLocalAabb() const;

			/// The local-space (untransformed) AABB of all the sphere and boundary obstacles.
		hkAabb m_localAabb;
};

#endif	// HKAI_SIMPLE_OBSTACLE_GENERATOR_H

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
