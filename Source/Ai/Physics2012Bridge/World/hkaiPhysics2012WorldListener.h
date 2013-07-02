/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_PHYSICS_WORLD_LISTENER_H
#define HKAI_PHYSICS_WORLD_LISTENER_H

#include <Physics2012/Dynamics/Entity/hkpEntityListener.h>
#include <Ai/Physics2012Bridge/Silhouette/hkaiPhysics2012BodySilhouetteGenerator.h>
#include <Ai/Pathfinding/Dynamic/PhysicsBridge/hkaiPhysicsBridgeBase.h>

class hkpEntity;
class hkpWorld;
class hkaiSilhouetteGenerator;
class hkaiObstacleGenerator;

/// This class marshals data between A.I. and Physics systems. It attaches as a listener to a physics world (hkpWorld)
/// and adds/removes AI data when objects are added / removed from the world.
/// Users integrating with Havok physics simply need to instantiate an instance of this class to begin the marshaling.
/// By default any object with a moving motion type i.e., not HK_MOTION_FIXED or HK_MOTION_CHARACTER is considered for inclusion
/// in the hkaiWorld.
class hkaiPhysics2012WorldListener :  public hkaiPhysicsBridgeBase, public hkpEntityListener
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
	typedef hkPointerMap<hkpEntity*, hkaiSilhouetteGenerator*>	EntityToSilhouetteGeneratorMap;
	typedef hkPointerMap<hkpEntity*, hkaiObstacleGenerator*>	EntityToObstacleGeneratorMap;

	hkaiPhysics2012WorldListener(hkpWorld* physicsWorld, hkaiWorld* aiWorld);

	virtual ~hkaiPhysics2012WorldListener();

		/// This callback allows users to control the detail level of silhouettes,
		/// and also filter which objects from the physics world should be included in the A.I. world.
		/// By default any object with a moving motion type i.e., not HK_MOTION_FIXED or HK_MOTION_CHARACTER
		/// will return DETAIL_INVALID, indicating that it will not generate a silhouette.
	virtual hkaiPointCloudSilhouetteGenerator::DetailLevel getDetailLevelForEntity( hkpEntity* entity );

		// hkpEntityListener Interface
		
		/// This method creates silhouette and obstacle generators for an entity and adds them to the AI world
	virtual void entityAddedCallback( hkpEntity* entity ) HK_OVERRIDE;

		/// This method removes the generators added in entityAddedCallback
	virtual void entityRemovedCallback( hkpEntity* entity ) HK_OVERRIDE;

		/// Associate each rigid body with a corresponding entry in the AI world.
	virtual void connectToPhysicsWorld() HK_OVERRIDE;

	virtual void disconnectFromPhysicsWorld() HK_OVERRIDE;

	hkaiSilhouetteGenerator* getSilhouetteGenerator(hkpEntity* entity) const;
	hkaiObstacleGenerator* getObstacleGenerator(hkpEntity* entity) const;

	void setSilhouetteGenerator(hkpEntity* entity, hkaiSilhouetteGenerator* silhouetteGenerator);
	void setObstacleGenerator(hkpEntity* entity, hkaiObstacleGenerator* obstacleGenerator);

	const EntityToSilhouetteGeneratorMap& getEntityToSilhouetteGeneratorMap() const { return m_entityToSilGenMap; }
	const EntityToObstacleGeneratorMap& getEntityToObstacleGeneratorMap() const { return m_entityToObsGenMap; }

	//
	// hkaiWorld::Listener interface.
	// By default, we mark the Physics world for read at the start of the step, and unmark it after.
	// This is controllable by m_markPhysicsWorldForReadDuringStep.
	//

		/// Marks the Physics world for read if m_markPhysicsWorldForReadDuringStep is true.
	virtual void preSilhouetteStepCallback( const class hkaiWorld* world, hkaiWorld::StepThreading threading, const class hkBitField* sectionsToStep ) HK_OVERRIDE;

		/// Unmarks the Physics world for read if m_markPhysicsWorldForReadDuringStep is true.
	virtual void postStepCallback(class hkaiWorld* world, const hkArrayBase<class hkaiBehavior*>& behaviors) HK_OVERRIDE;

	hkBool	m_markPhysicsWorldForReadDuringStep; //+default(true)

protected:

	hkaiPhysicsBridgeBase::BodyTypeFlags getFlagsForBody( const hkpRigidBody* body ) const;

	hkpWorld*	m_physicsWorld;

	EntityToSilhouetteGeneratorMap	m_entityToSilGenMap;
	EntityToObstacleGeneratorMap	m_entityToObsGenMap;
};

#endif	// HKAI_PHYSICS_WORLD_LISTENER_H

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
