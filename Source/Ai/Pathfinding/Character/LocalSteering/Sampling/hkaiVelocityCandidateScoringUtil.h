/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VELOCITY_CANDIDATE_SCORING_H
#define HKAI_VELOCITY_CANDIDATE_SCORING_H

#include <Common/Base/Types/hkBaseTypes.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>

class hkaiVelocityCandidateGenerator;
class hkaiVelocityCandidateScores;


// Define this to enable dumping sidedness planes to a global variable. This slows down steering.
//#define HKAI_ENABLE_SIDEDNESS_PLANE_DEBUGGING

#ifdef HKAI_ENABLE_SIDEDNESS_PLANE_DEBUGGING
extern bool hkaiDebugVelocityCandidateScoring;
extern hkVector4 hkaiDebugVelocityCandidateScoring_P;
#endif


struct hkaiVelocityCandidateScoringParameters
{
	HK_DECLARE_POD_TYPE();

	hkReal m_staticLambda;

	hkReal m_dynamicLambda;
};

/// Static methods for calculating time-to-collision and time-to-resolution between an agent
/// and various obstacles.
///
/// The scoreXXXObstacle functions take a list of candidate velocities, and a list of time-to-collision
/// parameters. Each one determines the time to collision with the obstacle for each candidate velocity.
/// If there is a future collision along a given velocity, and the preexisting value for time to collision
/// passed in is greater than the new time to collision, it will be changed to the new time to collision.
/// If there is no future collision at that candidate velocity, or if the future collision would occur
/// after the preexisting time to collision value, the preexisting value is kept. As a result, all calls
/// to these functions can operate on the same TTC array in order to find, for each candidate velocity,
/// the earliest collision over all obstacles.
///
/// If the agent is currently penetrating an obstacle, these functions will instead return negated
/// time-to-resolution. Zero is never returned as a time-to-resolution if there is a current 
/// penetration, so all such values will be strictly negative. Moreover, the same less-than operation is
/// used to update the scores. As a result, penetrated obstacles will strictly dominate nonpenetrating 
/// obstacles: Following all scoreXXXObstacle calls, either all scores will be nonnegative, indicating 
/// that there is currently no penetration with any obstacle and that each score is the earliest time to 
/// collision for each velocity; or all scores will be negative, indicating that there is at least one
/// penetrated obstacle and that each score's absolute value is the latest time to resolution among all
/// currently penetrated obstacles.
///
/// Some caveats for penetration: Where practical, extremely large times-to-resolution will be returned
/// for velocities which resolve the penetration by going THROUGH (rather than around) the obstacle.
/// Time to resolution, however, may not always be accurate and may have discontinuities, particularly
/// during deep penetrations. It is intended to resolve only shallow penetrations. Additionally, it may
/// underestimate time to resolution in the case of overlapping obstacles, if an agent is penetrating
/// one obstacle but not another obstacle which that obstacle overlaps. In pathological cases, this could
/// potentially cause oscillations near the perimeter of obstacle overlaps, and could even force the agent
/// to go through the entire obstacle rather than resolve the penetration in the correct direction.
///
/// The derived scoring functions are used for obstacle structs.
/// In each case, the agent and the obstacle need not be coplanar, but will be treated as though 
/// they are coplanar (in a plane normal to the up-vector). All velocity candidates should be 
/// tangent to this plane.
class hkaiVelocityCandidateScoringUtil
{
public:

	/// \name Derived scoring functions
	/// @{


	/// Generate time-to-collision scores for possible collision between an agent and a sphere obstacle.
	///
	/// The effective collision distance between the sphere obstacle center and the agent's position will
	/// be the sum of the agent's m_radius and the sphere obstacle's radius.
	///
	/// @return whether the agent is colliding with the given obstacle.
	static bool HK_CALL scoreSphereObstacle(
		hkaiAvoidanceSolver::SteeringAgent const& agent,
		hkaiAvoidanceSolver::SphereObstacle const& sphereObstacle,
		hkVector4Parameter up, 
		hkSimdRealParameter penetrationPenalty,
		hkaiVelocityCandidateGenerator const& velocityCandidates,
		hkaiVelocityCandidateScores & timesToCollision,
		hkaiVelocityCandidateScores & bonuses,
		hkVector4 & bonusVector);

	/// Generate time-to-collision scores for possible collision between an agent and an agent obstacle.
	///
	/// The effective collision distance between the agents will be the sum of their m_radius fields.
	///
	/// @return whether the agent is colliding with the given obstacle.
	static bool HK_CALL scoreAgentObstacle(
		hkaiAvoidanceSolver::SteeringAgent const& thisAgent, 
		hkaiAvoidanceSolver::ObstacleAgent const& otherAgent, 
		hkVector4Parameter up, 
		hkaiVelocityCandidateGenerator const& velocityCandidates, 
		hkSimdRealParameter sidednessChangingPenalty,
		hkSimdRealParameter penetrationPenalty,
		hkaiVelocityCandidateScores & timesToCollision,
		hkaiVelocityCandidateScores & bonuses,
		hkVector4 & bonusVector);

	/// Generate time-to-collision scores for possible collision between an agent and an boundary obstacle.
	///
	/// The effective radius of the spherical agent against the one-dimensional boundary obstacle -- or,
	/// equivalently, the radius of the Minkowski-summed capsule-shaped boundary obstacle against the
	/// agent point -- will be the agent's m_boundaryRadius parameter.
	///
	/// @return whether the agent is colliding with the given obstacle.
	static bool HK_CALL scoreBoundaryObstacle(
		hkaiAvoidanceSolver::SteeringAgent const& agent,
		hkaiAvoidanceSolver::BoundaryObstacle const& boundaryObstacle,
		hkVector4Parameter up,
		hkSimdRealParameter thresholdDistanceSquared,
		hkSimdRealParameter penetrationPenalty,
		hkaiVelocityCandidateGenerator const& velocityCandidates,
		hkaiVelocityCandidateScores & timesToCollision,
		hkaiVelocityCandidateScores & bonuses,
		hkVector4 & bonusVector);

	/// @}

	/// \name Primitive scoring functions
	/// @{

	/// Generate time-to-collision scores for possible collision between a point and a sphere.
	///
	/// @return whether the position is within the given obstacle.
	static bool HK_CALL scoreStaticSphere(
		hkVector4Parameter position,
		hkSphere const& sphere,
		hkSimdRealParameter penetrationPenalty,
		hkaiVelocityCandidateGenerator const& velocityCandidates,
		hkaiVelocityCandidateScores & timesToCollision,
		hkaiVelocityCandidateScores & bonuses,
		hkVector4 & bonusVector);

	/// Generate time-to-collision scores for possible collision between a point and a moving sphere.
	///
	/// @return whether the position is within the given obstacle.
	static bool HK_CALL scoreMovingSphere(
		hkVector4Parameter position,
		hkSphere const& sphere,
		hkVector4Parameter sphereVelocity,
		hkSimdRealParameter penetrationPenalty,
		hkaiVelocityCandidateGenerator const& velocityCandidates,
		hkaiVelocityCandidateScores & timesToCollision,
		hkaiVelocityCandidateScores & bonuses,
		hkVector4 & bonusVector);

	/// Generate time-to-collision scores for possible collision between a point and a directed capsule.
	/// @return whether the position is within the given obstacle.
	static bool HK_CALL scoreStaticCapsule(
		hkVector4Parameter position,
		hkVector4Parameter c1,
		hkVector4Parameter c2,
		hkSimdRealParameter totalRadius,
		hkSimdRealParameter penetrationPenalty,
		hkaiVelocityCandidateGenerator const& velocityCandidatesIn,
		hkaiVelocityCandidateScores & timesToCollision,
		hkaiVelocityCandidateScores & bonuses,
		hkVector4 & bonusVector);

	/// @}

	/// Determines the best velocity candidate, given ttc, progress and bonuses for each candidate.
	static void HK_CALL chooseBestCandidate(
		hkaiVelocityCandidateGenerator const& velocityCandidates, 
		hkaiVelocityCandidateScores const& timesToStaticCollision, 
		hkaiVelocityCandidateScores const& timesToDynamicCollision,
		hkaiVelocityCandidateScores const& bonuses,
		hkVector4Parameter bonusVector,
		hkReal radialMultiplier,
		hkReal wallFollowingAngle,
		hkReal collisionPenalty,
		hkReal desiredSpeed,
		hkReal wallSearchDistance,
		hkReal dodgingPenalty,
		hkVector4Parameter localGoalPlane,
		hkVector4 & bestCandidate);

	static hkBool32 HK_CALL calcWallFollowingDir(
		hkaiVelocityCandidateGenerator const& velocityCandidatesIn,
		hkaiVelocityCandidateScores const& timesToStaticCollision, 
		hkSimdRealParameter searchDistance,
		hkReal wallFollowingAngle,
		hkSimdReal & forwardFactorOut,
		hkSimdReal & rightFactorOut);
};

#undef HKAI_DISPLAY_WALL_FOLLOWING

#endif

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
