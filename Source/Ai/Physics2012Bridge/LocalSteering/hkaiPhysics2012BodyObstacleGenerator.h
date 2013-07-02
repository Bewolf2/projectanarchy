/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_PHYSICS_2012_BODY_OBSTACLE_GENERATOR_H
#define HKAI_PHYSICS_2012_BODY_OBSTACLE_GENERATOR_H

#include <Ai/Pathfinding/Character/LocalSteering/Obstacle/hkaiObstacleGenerator.h>

class hkpRigidBody;

#define HK_PROPERTY_OBSTACLE_ALWAYS 0x654e4323

/// An implementation of hkaiObstacleGenerator that uses an hkpRigidBody to update its transform.
/// If the rigid body is moving below m_velocityThreshold, no obstacles are generated. 
class hkaiPhysics2012BodyObstacleGenerator : public hkaiObstacleGenerator
{
	public:
		//+version(1)
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

		hkaiPhysics2012BodyObstacleGenerator(hkpRigidBody* rigidBody);

		hkaiPhysics2012BodyObstacleGenerator(hkFinishLoadedObjectFlag f);

		virtual ~hkaiPhysics2012BodyObstacleGenerator();

		virtual void update() HK_OVERRIDE;

		virtual void getAabb(hkAabb& aabbOut) const HK_OVERRIDE;

		void setVelocityThreshold( hkReal t) { m_velocityThreshold = t; }

		hkReal getVelocityThreshold() const { return m_velocityThreshold; }

	protected:

			/// Velocity below which obstacle generation is diabled.
			/// This generally should be the same as hkaiPhysics2012BodySilhouetteGenerator::m_velocityThreshold
		hkReal m_velocityThreshold; //+default(3.0f)

		hkRefPtr<const hkpRigidBody>	m_rigidBody;
};

#endif	// HKAI_PHYSICS_2012_BODY_OBSTACLE_GENERATOR_H

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
