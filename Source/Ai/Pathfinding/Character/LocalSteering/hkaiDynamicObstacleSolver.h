/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_DYNAMIC_OBSTACLE_SOLVER_H
#define HK_AI_DYNAMIC_OBSTACLE_SOLVER_H

#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>

	/// Local steering utility to handle dynamic obstacles.
class hkaiDynamicObstacleSolver
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkaiDynamicObstacleSolver);

	/// Velocity obstacle is the elementary representation of the obstacle in the velocity space.
	/// Velocity obstacle is represented by two planes, additional infos are the collision times.
	struct VelocityObstacle
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_STEERING, VelocityObstacle );

		enum ObstacleType
		{
			AGENT = 0,
			SPHERE			
		};

		enum GradientType
		{
			NONE = 0,
			SLOWDOWN,
			STANDARD,
			PREFERRED 
		};

		/// The left plane
		hkVector4 m_leftPlane;

		/// The right plane
		hkVector4 m_rightPlane;

		/// The collision time for the current velocity
		HK_PAD_ON_SPU(hkReal) m_currentCollisionTime;

		/// The collision time for the preferred new velocity
		HK_PAD_ON_SPU(hkReal)m_desiredCollisionTime;

		/// The velocity obstacle origin
		HK_PAD_ON_SPU( int ) m_type;

		/// The velocity obstacle gradient type
		HK_PAD_ON_SPU( int ) m_gradientType;	

	};

	static void HK_CALL solve( const hkaiAvoidanceSolver::WorldInfo& worldInfo,
							   const hkaiAvoidanceSolver::SteeringAgent& agent,
							   const hkVector4& deltaVelocity, 
							   hkaiAvoidanceSolver::ControlGradient& controlGradient );

	/// This method process all nearby agents and returns maximal gradient of the velocity change
	static void HK_CALL processNearbyAgents( const hkaiAvoidanceSolver::WorldInfo& worldInfo,
											 const hkaiAvoidanceSolver::SteeringAgent& agent,
											 const hkVector4& deltaVelocity,
											 hkaiAvoidanceSolver::ControlGradient& controlGradient );

	/// This method process all nearby sphere obstacles and returns maximal gradient of the velocity change
	static void HK_CALL processNearbyObstacles( const hkaiAvoidanceSolver::WorldInfo& worldInfo,
												const hkaiAvoidanceSolver::SteeringAgent& agent, 
												const hkVector4& deltaVelocity,
												hkaiAvoidanceSolver::ControlGradient& controlGradient );

	static void HK_CALL calcGradient( const hkaiAvoidanceSolver::SteeringAgent& agent, 
									  const VelocityObstacle& velocityObstacle, 
									  const hkVector4& deltaVelocity,
									  hkaiAvoidanceSolver::ControlGradient& controlGradient );

	static VelocityObstacle::GradientType HK_CALL selectGradient( const hkaiAvoidanceSolver::SteeringAgent& agent, 
																  const hkaiAvoidanceSolver::ObstacleAgent& obstacle);

	static bool HK_CALL detectFailure( const hkaiAvoidanceSolver::ObstacleAgent* neighborAgent,
										 const hkaiAvoidanceSolver::SphereObstacle* sphereObstacle,
										 const hkaiAvoidanceSolver::SteeringAgent& agent, 
										 hkaiAvoidanceSolver::ControlGradient& controlGradient );
	
	
	/// This method calculates velocity obstacle for agent obstacle
	static void HK_CALL buildAgentRVO(  const hkaiAvoidanceSolver::SteeringAgent& agent, 
										const hkaiAvoidanceSolver::ObstacleAgent& obstacle, 
										const hkVector4& up,
										VelocityObstacle& velocityObstacle );

	/// This method calculates velocity obstacle for sphere obstacle
	static void HK_CALL buildObstacleVO( const hkaiAvoidanceSolver::SphereObstacle& sphereObstacle,
										 const hkaiAvoidanceSolver::SteeringAgent& agent, 
										 const hkVector4& up, 
										 VelocityObstacle& velocityObstacle );

	static void HK_CALL calcVOGeometry( const hkVector4& up, 
										const hkVector4& obstacleAgentPos, 
										const hkVector4& obstacleVelocity, 
										const hkReal radius, 
										VelocityObstacle& velocityObstacle );

	static void HK_CALL calcVOCollisionTimes( const hkVector4& up, 
											  const hkVector4& agentToObstacle, 
											  const hkVector4& obstacleVelocity, 
											  const hkVector4& agentVelocity, 
											  const hkVector4& agentDesiredVelocity,
											  const bool isObstacleActive,
											  const hkReal radius, 
											  VelocityObstacle& velocityObstacle );

};

#endif // HK_AI_DYNAMIC_OBSTACLE_SOLVER_H

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
