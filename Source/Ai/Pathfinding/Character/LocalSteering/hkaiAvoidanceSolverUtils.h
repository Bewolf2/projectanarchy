/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_AVOIDANCE_SOLVER_UTILS_H
#define HK_AI_AVOIDANCE_SOLVER_UTILS_H

#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiStaticObstacleSolver.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiDynamicObstacleSolver.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

class hkaiCharacter;
class hkaiObstacleCollector;
class hkaiObstacleGenerator;

/// Set of utility methods for hkaiAvoidanceSolver
class hkaiAvoidanceSolverUtils
{
	public:

		/// Set up all the data to solve local avoidance for one character
		static void HK_CALL setupAgent(const hkaiLocalSteeringInput& steeringInput,
			const hkaiStreamingCollection::InstanceInfo* sectionInfo,
			hkaiPackedKey navMeshFaceKey, hkReal erosionRadius,
			const hkArrayBase< const hkaiCharacter* >& neighbors, 
			const hkArrayBase< const hkaiObstacleGenerator* >& obstacles, 
			const hkaiAvoidancePairProperties* avoidancePairProps, 
			hkaiAvoidanceSolver::SteeringAgent& agentOut, 
			hkaiObstacleCollector& collector);

		/// Apply simple kinematic and dynamic constraints to the desired velocity
		/// \param[out] constrainedVelocity Resulting velocity after kinematic constraints are applied
		static void HK_CALL applyKinematicConstraints(	hkReal timestep, 
			const hkaiAvoidanceSolver::MovementProperties& movementLimits, 
			hkVector4Parameter up, hkVector4Parameter oldVelocity, hkVector4Parameter oldForward, 
			hkVector4Parameter desiredVelocity, hkVector4& constrainedVelocity, bool limitEnabled = true);

		/// Calculate the resulting velocity for the agent to avoid the given set of obstacles and agents.
		/// \param[out] steerVelocityOut Resulting linear and angular velocity
		/// \param[out] steerForwardOut Resulting forward direction, perpendicular to up
		/// \return HK_FAILURE if the penalty forces-based solver is used and is unable to determine 
		/// a noncolliding trajectory (it returns a "best available" trajectory instead); otherwise, 
		/// HK_SUCCESS.
		static hkResult HK_CALL calcSteeringVelocity(
			hkReal timestep, 
			hkVector4Parameter up, 
			const hkaiAvoidanceSolver::SteeringAgent& agentIn, 
			hkVector4& steerVelocityOut,
			hkVector4& steerForwardOut);

		/// Calculate the resulting velocity from the desired velocity, without applying any avoidance.
		/// If steeringInput.m_applyKinematicConstraints is true, this call will apply kinematic constraints and angular acceleration limits.
		/// \param[out] steerVelocityOut Resulting linear and angular velocity
		/// \param[out] steerForwardOut Resulting forward direction, perpendicular to up
		static void HK_CALL calcUnsteeredVelocity(
			hkReal timestep, 
			hkVector4Parameter up, 
			const hkaiAvoidanceSolver::MovementProperties& movementProperties, 
			const hkaiLocalSteeringInput& steeringInput, 
			hkVector4& steerVelocityOut,
			hkVector4& steerForwardOut);

		/// Fill angular velocity (measured in terms of the angle between the forward direction and the new velocity direction)
		/// into the W component of the velocity vector. If movementProperties.m_useAngularConstraints is true, this will 
		/// limit angular velocity and acceleration.
		static void HK_CALL calcAngularVelocityAndForward(
			hkVector4Parameter currentVelocity,
			hkVector4Parameter newVelocity, 
			hkVector4Parameter forward, 
			hkVector4Parameter up, 
			hkaiAvoidanceSolver::MovementProperties const& movementProperties,
			hkReal timestep,
			hkSimdReal & angularVelocity,
			hkVector4& steerForward);

		// This method detects relative position of agent in front of boundary
		static bool detectFrontArea( hkReal radius, const hkVector4& agentPos, const hkaiAvoidanceSolver::BoundaryObstacle& boundary, const hkVector4& boundaryPlane );

		// This method return true if the obstacle is inside view angle cone of the agent
		static bool HK_CALL inView(const hkReal cosViewAngle, const hkaiAvoidanceSolver::ObstacleAgent& agent, const hkaiAvoidanceSolver::ObstacleAgent& obstacle);

		// This method calculates the velocity obstacle avoidance gradient
		static void HK_CALL calcVOGradient(const hkReal agentMinvel, const hkVector4& agentVel, const hkaiDynamicObstacleSolver::VelocityObstacle& velocityObstacle, const hkVector4& preferred, hkVector4& gradient );
		
		// This method calculates boundary plane with normal perpendicular to boundary and up vector
		static void HK_CALL calcBoundaryPlane(const hkaiAvoidanceSolver::BoundaryObstacle& boundary, const hkVector4& up, hkVector4& boundaryPlane);

		// This method computes the collision time for circle obstacle in local coordinate system of the agent
		static hkReal HK_CALL intersectVelCircle( const hkVector4& vel, hkReal circleRadius, const hkVector4& circlePos, const hkVector4& up );

		// This method computes the collision time for boundary obstacle in local coordinate system of the agent
		static hkReal HK_CALL intersectVelLine( const hkVector4& vel, const hkVector4& start, const hkVector4& end, const hkVector4& up);

		// This method computes the intersection of velocity+deltaVeleocity and input plane
		static hkReal HK_CALL intersectVelDeltaVelRVO(const hkVector4& vel, const hkVector4& dvel, const hkVector4& plane);

		// This method computes the intersection of velocity+deltaVeleocity and input edge (vo edge direction vector)
		static hkReal HK_CALL intersectVelDeltaVelVOEdge(const hkVector4& vel, const hkVector4& dvel, const hkVector4& up, const hkVector4& edge);
		
		// This method returns true if input velocity is inside velocity obstacle
		static bool HK_CALL isInsideRVO(const hkVector4& v, const hkVector4& planeA, const hkVector4& planeB);

		// This method calculates 2x2 determinant (p1*q2-p2*q1) according to input up vector
		static hkReal HK_CALL determinant2d(const hkVector4& p, const hkVector4& q, const hkVector4& up); 

		// This method computes closest distance to line segment
		static hkReal HK_CALL closestDistToLineSegment( const hkVector4& p, const hkVector4& a, const hkVector4& b); 

		// This method calculates oriented angle between two vectors (first vector is defined by two perpendicular vectors )
		static hkReal HK_CALL calcAngle( const hkVector4& va, const hkVector4& vap, const hkVector4& vb );

		static hkReal HK_CALL fabsAngle( const hkReal openEdge, const hkReal closeEdge );

		static hkReal HK_CALL clampAngle( const hkReal angle );

		static bool HK_CALL isAngleInsideVO(const hkReal angle, const hkReal openEdge, const hkReal closeEdge );

		static bool HK_CALL clipSegmentInsideAabb( const hkAabb& aabb, hkVector4& startOut, hkVector4& endOut);

		static hkReal HK_CALL calcSphereCollisionTime(const hkReal totalRadius, const hkVector4& agentToObstacle, const hkVector4& relativeVelocity, const hkVector4& up );
		
		static hkReal HK_CALL calcBoundaryCollisionTime(const hkReal radius, const hkVector4& agentToStart, const hkVector4& agentToEnd, const hkVector4& relativeVelocity, const hkVector4& normal, const hkVector4& up );


		static void	  HK_CALL manageAgentPenetration( hkReal timestep,
													  const hkaiAvoidanceSolver::SteeringAgent& agent, 
													  const hkaiAvoidanceSolver::ObstacleAgent& obstacle,
													  hkaiAvoidanceSolver::ControlGradient& gradient );

		static void   HK_CALL manageObstaclePenetration( hkReal timestep,
														 const hkaiAvoidanceSolver::SteeringAgent& agent,
													     const hkaiAvoidanceSolver::SphereObstacle& obstacle, 
													     hkaiAvoidanceSolver::ControlGradient& gradient );

		static void   HK_CALL manageBoundaryPenetration( hkReal timestep,
														 const hkaiAvoidanceSolver::SteeringAgent& agent,
														 const hkVector4& boundaryPlane,
														 const hkReal closestDist,
													     hkaiAvoidanceSolver::ControlGradient& gradient );
};

#ifdef HKAI_DEBUG_DISPLAY_LOCAL_STEERING

/// Set of debug display methods
class hkaiAvoidanceSolverDebugDisplayUtils
{
	public:
		// This method applies full kinematic and dynamic constraints to the new velocity
		static void HK_CALL applyKinematicConstraintsDisplay( hkReal timestep, 
			const hkaiAvoidanceSolver::MovementProperties& movementLimits, 
			hkVector4Parameter pos,		// only for display debug 
			hkVector4Parameter up,
			hkVector4Parameter oldVelocity, 
			hkVector4Parameter oldForward,
			hkVector4Parameter desiredVelocity,
			hkVector4& constrainedVelocity,
			bool limitEnabled = true );

		// This method calculates intersection point of two planes in the plane defined by up vector
		static void HK_CALL calcPlaneIntersection2d(const hkVector4& plane1, const hkVector4& plane2, const hkVector4& up, hkVector4& p);

		// Helper debug method displays preferred, current and new avoidance velocities
		static void HK_CALL displayVelocities(const hkVector4& agentPos, const hkVector4& agentVel, const hkVector4& agentDesiredVel, const hkVector4& agentAvoidanceVel, const hkVector4& up, hkReal dshift = 0.0f);

		// Helper debug method displays the velocity obstacle
		static void HK_CALL displayVelocityObstacle(const hkVector4& agentPos, const hkaiDynamicObstacleSolver::VelocityObstacle& vo, const hkVector4& up, hkColor::Argb color, hkReal dshift = 0.0f);			
		
		// 
		static void HK_CALL displayMergedVelocityObstacleEdges( const hkVector4& agentPos, const hkVector4& agentVelocityForward, const hkArray<hkaiStaticObstacleSystem::VelocityObstacleEdge>& edges, const hkVector4& up, hkColor::Argb color, hkReal dshift = 0.0f);			
};
#endif

#endif // HK_AI_AVOIDANCE_SOLVER_UTILS_H

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
