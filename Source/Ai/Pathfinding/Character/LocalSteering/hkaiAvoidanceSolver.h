/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_AVOIDANCE_SOLVER_H
#define HK_AI_AVOIDANCE_SOLVER_H

#include <Ai/Pathfinding/Character/LocalSteering/hkaiLocalSteeringInput.h>
#include <Common/Base/Types/Geometry/Sphere/hkSphere.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>


#ifdef HK_PLATFORM_SPU
#	include <Common/Base/Container/LocalArray/hkLocalArray.h>
#endif

extern const class hkClass hkaiAvoidancePairPropertiesPairDataClass;

extern const class hkClass hkaiAvoidancePairPropertiesClass;

extern const class hkClass hkaiAvoidanceSolverAvoidancePropertiesClass;

extern const class hkClass hkaiAvoidanceSolverMovementPropertiesClass;

#define HK_AI_OBSTACLE_PROPERTY		8989

// If this is not defined, new-style RVO will not be available for SPU jobs
#define HKAI_INCLUDE_NEW_AVOIDANCE_ON_SPU



/// Movement properties define kinematic and dynamic constraints for agent movement
struct hkaiMovementProperties
{
	//+version(7)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_STEERING, hkaiMovementProperties );

	/// Sets properties to reasonable defaults, based on maximum speed and linear acceleration and
	/// optional turn radius. The only parameter not set by this function is m_kinematicConstraintType.
	HK_FORCE_INLINE void setReasonableProperties(hkReal maxSpeed, hkReal maxAcceleration, hkReal turnRadius = -1.0f);

	inline hkaiMovementProperties();
	hkaiMovementProperties(hkFinishLoadedObjectFlag f){}

	/// Returns m_maxTurnVelocity if constraint mode is CONSTRAINTS_LINEAR_AND_ANGULAR, m_maxVelocity otherwise
	HK_FORCE_INLINE hkReal getMaxTurnVelocity() const;

	/// Sets m_maxTurnVelocity, and warns if constraint mode is not CONSTRAINTS_LINEAR_AND_ANGULAR
	HK_FORCE_INLINE void setMaxTurnVelocity( hkReal newMaxTurnVel );

	/// Computes the min turn radius at the specified speed
	/// Uses m_maxAngularVelocity if constraint mode is CONSTRAINTS_LINEAR_AND_ANGULAR,
	/// m_maxAcceleration and m_maxDeceleration otherwise.
	HK_FORCE_INLINE hkReal getTurnRadiusAtSpeed( hkReal speed ) const;

	/// Returns m_maxAngularVelocity
	HK_FORCE_INLINE hkReal getMaxAngularVelocity() const;

	/// Sets m_maxAngularVelocity
	HK_FORCE_INLINE void setMaxAngularVelocity( hkReal newMaxAngVel );

	/// Returns m_maxAngularAcceleration and warns if constraint mode is not CONSTRAINTS_LINEAR_AND_ANGULAR
	HK_FORCE_INLINE hkReal getMaxAngularAcceleration() const;

	/// Sets m_maxAngularAcceleration and warns if constraint mode is not CONSTRAINTS_LINEAR_AND_ANGULAR
	HK_FORCE_INLINE void setMaxAngularAcceleration( hkReal newMaxAngAccel );

	/// Minimal speed
	hkReal m_minVelocity; //+default(.02f)

	/// Maximal speed
	hkReal m_maxVelocity; //+default(5.64f)

	/// Maximal acceleration
	hkReal m_maxAcceleration; //+default(20.0f)

	/// Maximal deceleration
	hkReal m_maxDeceleration; //+default(30.0f)

	hkReal m_leftTurnRadius; //+default(-1.0f)

	hkReal m_rightTurnRadius; //+default(-1.0f)

	/// Maximal angular velocity
	hkReal m_maxAngularVelocity; //+default( 1.25f * HK_REAL_PI )

protected:

	/// Maximal angular acceleration
	hkReal m_maxAngularAcceleration; //+default( 37.5f * HK_REAL_PI )

	/// Maximal linear speed during a full turn (Default: 50% of maximum speed).
	/// This is intended to be smaller than m_maxVelocity (i.e. slowing down going into a turn)
	hkReal m_maxTurnVelocity; //+default( .5f * 5.64f )

public:
	/// The type of kinematic constraint that is applied to the character's movement.
	enum KinematicConstraintType
	{
		/// No kinematic constraints are applied to the character's movement.
		CONSTRAINTS_NONE = 0,

		/// Constrain turning separately, instead as a byproduct of constrained linear acceleration.
		CONSTRAINTS_LINEAR_AND_ANGULAR = 1,

		/// Only linear constraints are applied .
		CONSTRAINTS_LINEAR_ONLY = 2,
	};

	hkEnum<KinematicConstraintType, hkUint8> m_kinematicConstraintType; //+default(hkaiAvoidanceSolver::MovementProperties::CONSTRAINTS_LINEAR_ONLY)
};

/// Avoidance properties define control parameters for agent local avoidance
struct hkaiAvoidanceProperties : public hkReferencedObject
{
	// +vtable(true)
	//+version(12)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING);
	inline hkaiAvoidanceProperties();

	/// Agent movement kinematic and dynamic constraints
	struct hkaiMovementProperties m_movementProperties;

	/// Determines which solver will be used to determine avoidance velocities.
	enum AvoidanceSolverType
	{
		/// Use the sampling solver.
		AVOIDANCE_SOLVER_SAMPLING = 0,

		/// Use the old penalty-forces-based solver. This will be deprecated in a future release.
		AVOIDANCE_SOLVER_PENALTY_FORCES
	};

	hkEnum<AvoidanceSolverType, hkUint8> m_avoidanceSolverType; //+default(hkaiAvoidanceSolver::AvoidanceProperties::AVOIDANCE_SOLVER_SAMPLING)


	/// Search type for detecting nearby boundary edges to avoid
	enum NearbyBoundariesSearchType
	{
		/// Only search the current nav mesh face its the adjacent faces.
		SEARCH_NEIGHBORS,

		/// Do floodfill to find all boundary edges within the character's sensor.
		SEARCH_FLOOD_FILL
	};

	/// How nearby edges should be detected.
	hkEnum<NearbyBoundariesSearchType, hkUint8> m_nearbyBoundariesSearchType; //+default(hkaiAvoidanceSolver::AvoidanceProperties::SEARCH_FLOOD_FILL)

	/// Character/obstacle sensor, in world axes and relative to character position
	hkAabb m_localSensorAabb;

	/// \name Parameters only used by the penalty forces-based solver
	/// @{

	/// Critical collision time for agent obstacle.
	///
	/// This value is not used by the sampling-based solver.
	hkReal m_agentLimitTime; //+default(1.0f)

	/// Critical collision time for passive sphere obstacle
	///
	/// This value is not used by the sampling-based solver.
	hkReal m_obstacleLimitTime; //+default(1.0f)

	/// Critical collision time for passive boundary obstacle
	///
	/// This value is not used by the sampling-based solver.
	hkReal m_boundaryLimitTime; //+default(0.3f)

	/// Limit distance to 'static' obstacle taken into account by sos solver
	///
	/// This value is not used by the sampling-based solver.
	hkReal m_limitObstacleDistance; //+default(1.0f)

	/// Limit distance to 'static' boundary taken into account by sos solver.
	/// If this is set to a negative value, the actual value used will be
	/// 3 times the character radius, which is a good default to avoid
	/// becoming "pinched" between characters and boundaries.
	///
	/// This value is not used by the sampling-based solver.
	hkReal m_limitBoundaryDistance; //+default(-1.0f)

	/// Limit velocity to 'static' obstacle taken into account by sos solver
	///
	/// This value is not used by the sampling-based solver.
	hkReal m_limitObstacleVelocity; //+default(0.5f)

	/// @}

	/// \name Parameters only used by the sampling-based solver
	/// @{

	/// Maximum angle between wall following direction and sideways direction.
	/// Characters will not wall follow if the angle between the wall following
	/// direction and the sideways direction (a direction perpendicular to the
	/// forward direction) exceeds this angle. This should be between 0 and pi/2; 
	/// a value of zero disables wall following. (Wall following is used to 
	/// resolve certain collision avoidance stalemates, so should not generally
	/// be disabled.) 
	///
	/// Increasing this value allows characters to successfully traverse larger
	/// obstacles which are not in the navmesh, but can cause agents to take
	/// the long way around these obstacles, or wall-follow in crowded situations
	/// where it would be more effective to simply wait.
	///
	/// This value is only used by the sampling-based solver. 
	/// Wall-following above the default value is in beta.
	hkReal m_wallFollowingAngle; //+default(0.1f)

	/// A penalty applied to velocity candidates which avoid collisions by 
	/// turning rather than by changing speed. A character with a low dodging
	/// penalty (near zero) will tend to deal with upcoming collisions by 
	/// dodging around them and continuing near their preferred velocity, 
	/// making them appear urgent, panicky, or uncooperative. A character with 
	/// a high dodging penalty (near 1) will instead tend to speed up or slow 
	/// down and turning only slightly, making them appear unhurried, focused, 
	/// or cooperative.
	/// 
	/// The dodging penalty should be greater than zero. Dodging penalties
	/// greater than 1 are allowed, but will generally not have a noticeably
	/// different effect than that of a dodging penalty of 1, and may cause
	/// problems if used in conjunction with a wall following angle which is
	/// greater than the default. 
	/// 
	/// The default dodging penalty of 1 is similar to that experimentally 
	/// demonstrated by human pedestrians in non-panic crowd situations.
	hkReal m_dodgingPenalty; //+default(1.0f)

	/// Preference for continuing along the same velocity when another
	/// velocity candidate scores more highly. Characters will continue
	/// on their current velocity unless they can improve their rate of 
	/// progress by at least this amount.
	///
	/// Velocity hysteresis can avoid certain types of velocity twitch. 
	/// If it is set too high, characters may follow visibly inefficient 
	/// trajectories, or have difficulty reaching their goals.
	///
	/// This value is only used by the sampling-based solver.
	hkReal m_velocityHysteresis; //+default(0.01f)

	/// Unwillingness to change the side on which an agent plans to pass
	/// another agent. This value is divided by the time until the two agents
	/// will pass each other, and the result is used as a speed: the agent
	/// will not change the passing side unless by doing so it can increase 
	/// its rate of progress by at least that speed.
	///
	/// Increasing this value can help resolve certain types of reciprocal
	/// oscillation which occurs when agents are approaching each other.
	/// If it is set too high, agents may wander far from their desired
	/// trajectories to avoid changing sides.
	///
	/// This value is only used by the sampling-based solver.
	hkReal m_sidednessChangingPenalty; //+default(0.0f)

	/// A multiplier applied to the penalty for projected collisions. This
	/// value is divided by the projected time to collision along each
	/// possible trajectory, and the result is subtracted from that 
	/// trajectory's progress rate score.
	///
	/// Increasing this value causes agents to be more timid; they will
	/// react earlier and more smoothly to distant collisions, but will
	/// be less effective at pushing through very crowded situations. 
	///
	/// This value is only used by the sampling-based solver.
	hkReal m_collisionPenalty; //+default(1.0f)

	/// A multiplier applied to the penalty for ongoing penetrations.
	/// This value is multiplied by the penetration depth, and the result
	/// is used as a weighting for the agent's desire to resolve the
	/// penetration (rather than making forward progress).
	///
	/// Increasing this value will make agents veer off more quickly 
	/// and sharply when collisions occur, minimizing the time in a 
	/// penetrating state. Decreasing this value can lead to smoother
	/// locomotion, particularly if agents are controlled by physics.
	///
	/// This value is only used by the sampling-based solver.
	hkReal m_penetrationPenalty; //+default(100.0f)

	/// @}

	/// The approximate maximum number of neighbors before a character
	/// adaptively contracts its sensor region. 
	///
	/// Lower values will improve performance. Higher values may improve
	/// characters' ability to navigate in extremely crowded situations.
	hkInt32 m_maxNeighbors; //+default(16)

	hkaiAvoidanceProperties(hkFinishLoadedObjectFlag f)
		:	hkReferencedObject(f),
		m_movementProperties(f)
	{
	}

};

	///	Pairwise avoidance properties for local steering. This allows an override between different types of characters.
struct hkaiAvoidancePairProperties : public hkReferencedObject
{
	//+version(2)
public:
HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

	HK_DECLARE_REFLECTION();
	
		/// Modified weight and view angle information
	struct PairData
	{
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_STEERING, PairData );

		PairData() : m_key(0), m_weight(0.5f), m_cosViewAngle(-0.4216f) {}

		hkUint32	m_key;		//+default(0)

		hkReal		m_weight;	//+default(0.5f)

		hkReal		m_cosViewAngle;	//+default(-0.4216f)
	};

	/// Constructor
	hkaiAvoidancePairProperties(){}
	
	// In-place constructor for use during jobs
	hkaiAvoidancePairProperties( PairData* pairs, int numPairs)
	:	m_avoidancePairDataMap(pairs, numPairs, numPairs)
	{

	}

	hkaiAvoidancePairProperties(hkFinishLoadedObjectFlag f) : hkReferencedObject(f), m_avoidancePairDataMap(f) { }

	
	virtual ~hkaiAvoidancePairProperties()
	{	
	}

	/// Add pairwise avoidance properties for a pair of avoidance types
	///
	/// Avoidance properties will affect both A's avoidance of B, and B's avoidance of A.
	/// 
	/// \param typeA the first avoidance type
	/// \param typeB the second avoidance type
	/// \param weightAB the relative distribution of collision avoidance duties. Setting weight to 0 will cause
	/// A to assume full responsibility for collision avoidance; a weight of 1 will cause B to assume full
	/// responsibility.
	void addPair(
		hkUint16 typeA, 
		hkUint16 typeB, 
		hkReal weightAB);

	/// Function inserts corresponded keys and weights for both combinations to map
	///
	/// \deprecated You should use addPair() instead of constructing a PairData directly.
	HK_DEPRECATED2("You should use addPair() instead of constructing a PairData directly.")
	void insertData(hkUint16 typeA, hkUint16 typeO, PairData& dataAO) 
	{
		dataAO.m_key = calcKey( typeA, typeO );
		m_avoidancePairDataMap.pushBack( dataAO );	
	}

	/// Sets dataOut to the corresponding weight and view angle if the key is found.
	/// Returns whether or not the key was found
	hkBool32 getData(hkUint16 typeA, hkUint16 typeO, PairData& dataOut) const
	{
		const hkUint32 key = calcKey( typeA, typeO );

		for (int i=0; i<m_avoidancePairDataMap.getSize(); i++)
		{	
			if (key == m_avoidancePairDataMap[i].m_key)
			{
				dataOut = m_avoidancePairDataMap[i];
				return true;
			}			
		}

		return false;
	}

	/// Calculate keys for avoidance pair
	static HK_FORCE_INLINE hkUint32 calcKey( hkUint16 typeA, hkUint16 typeO )
	{
		return typeA + (typeO << 16);
	}

	const hkArray<struct PairData>& getPairs() const { return m_avoidancePairDataMap; }

protected:
	hkArray<struct PairData>	m_avoidancePairDataMap; 
};

#define SOLVER_EPS	1e-3

/// This avoidance solver solves a local steering problem for a single agent according to its current position in the scene.
/// The solver operates on the array of the agents and obstacles in the velocity space and computes optimal velocity change for safe navigation to local goal.
/// The solver works separately for every single agent and for every update frame.
class hkaiAvoidanceSolver
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiAvoidanceSolver);
		/// Global information about AI world
		struct WorldInfo
		{
			/// AI world up direction
			hkVector4 m_up;

			/// Timestep between two AI update frames
			hkSimdReal m_timeStep;			
		};

		/// The sphere obstacle
		struct SphereObstacle
		{
			HK_DECLARE_POD_TYPE();
			HK_DECLARE_REFLECTION();

			/// The sphere representation of the obstacle. The sphere is defined by its position and radius
			hkSphere m_sphere;

			/// Current velocity of the sphere
			hkVector4 m_velocity;	
		};

		/// The wall obstacle
		struct BoundaryObstacle
		{
			HK_DECLARE_POD_TYPE();
			HK_DECLARE_REFLECTION();

			/// Start point
			hkVector4 m_start;

			/// End point
			hkVector4 m_end;			
		};

		typedef hkaiAvoidanceProperties AvoidanceProperties;

		typedef hkaiMovementProperties MovementProperties;

		
		/// An obstacle which is another agent.
		struct ObstacleAgent
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_STEERING, ObstacleAgent );
				/// The current position
			hkVector4	m_position;

				/// The current velocity
			hkVector4	m_velocity;

				/// The facing direction
			hkVector4	m_forward;

				/// Avoidance properties and kinematic and dynamic movement constraints
			const AvoidanceProperties* m_avoidanceProperties;

				/// The agent priority
			hkUint16	m_priority;

				/// Radius of the agent
			hkReal m_radius;

				/// Radius of the agent to be used when avoiding boundary obstacles
			hkReal m_boundaryRadius;

				/// The agent weight for particular agent-obstacle pair
			hkReal m_weight;

				/// Cosine of view angle is extension to sensor. Only obstacles inside view angle cone are taken into account (Valid only for agents)
			hkReal m_cosViewAngle;			

		};

			/// SteeringAgent is the main input to the avoidance solver.
			/// holds all information about its current position and orientation, dynamic state,
			/// all nearby agents and obstacles, its current goal and its kinematic and dynamic constraints.
		struct SteeringAgent : public ObstacleAgent
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_STEERING, SteeringAgent );

				/// The current desired velocity and state inputs
			hkaiLocalSteeringInput		m_localSteeringInput;

				/// The pointer to the array of all nearby agent ids
			const ObstacleAgent*		m_nearbyAgents;

				/// The number of all nearby agents
			int			m_numNearbyAgents;

				/// The pointer to the array of all nearby boundary obstacles
			const hkaiAvoidanceSolver::BoundaryObstacle*	m_boundaryObstacles;

				/// The number of all nearby boundary obstacles
			int			m_numBoundaryObstacles;

				/// The pointer to the array of all nearby sphere obstacles
			const hkaiAvoidanceSolver::SphereObstacle*	m_sphereObstacles;

				/// The number of all nearby sphere obstacles
			int			m_numSphereObstacles;

				/// Debug display enabled
			hkBool m_enableDebugDisplay;

				/// Assert that all data is OK.
			void validate() const;
		};

		
			/// Internal accumulator for avoidance results
		struct ControlGradient
		{
			// The sos system gradient direction or normal of the nearest boundary (normalized). The fourth element is delta velocity ratio.
			hkVector4	m_direction;

			// The total gradient. The fourth element is total distance to edge (step length).
			hkVector4	m_total;

			// The repulsion velocity in the case of penetration. The fourth element is a sum of penetration depths.
			hkVector4  m_repulsion;

			// True indicates a tricky avoidance situation. 
			HK_PAD_ON_SPU(bool) m_failed;
		};


		/// The solver solves avoidance problem for single agent and returns the velocity change of the current velocity
		static hkResult HK_CALL solveSingleAgent( const WorldInfo& worldInfo,
												  const SteeringAgent& agent,
											      hkVector4& deltaVelocity);
						

		static hkResult HK_CALL calculateDeltaVelocity( const WorldInfo& worldInfo,
														const SteeringAgent& agent,
												   		const ControlGradient& gradient,
														hkVector4& deltaVelocity );

};

#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.inl>

#endif // HK_AI_AVOIDANCE_SOLVER_H

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
