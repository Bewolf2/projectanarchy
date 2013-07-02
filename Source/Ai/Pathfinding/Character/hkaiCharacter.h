/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_AI_CHARACTER_H
#define HK_AI_CHARACTER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>
#include <Ai/Pathfinding/Character/hkaiPath.h>
#include <Ai/Pathfinding/Character/Utilities/hkaiAdaptiveRanger.h>

class hkaiAstarCostModifier;
class hkaiAstarEdgeFilter;
class hkaiBehavior;
class hkaiSpatialQueryHitFilter;
class hkaiLocalSteeringFilter;
class hkaiNavMeshPathRequestInfo;
class hkaiNavVolumePathRequestInfo;
class hkaiWorld;

	/// The hkaiCharacter class binds a character to path and tracks the character movement along the path.
	/// The class is used to provide velocity information to keep the character on the path.
class hkaiCharacter : public hkReferencedObject 
{
	//+vtable(true)
	//+version(28)
	public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		
		HK_DECLARE_REFLECTION();
			/// This interface allows you to receive callbacks as the character traverses along a path
		class BehaviorListener
		{
			public:
				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiCharacter::BehaviorListener);
				enum FailureCause
				{
						/// The start point of the path is invalid
					FAILURE_INVALID_START,

						/// The end points of the path are invalid
					FAILURE_INVALID_END,

						/// There is no path from the start point to the end point
					FAILURE_UNREACHABLE,
					
						/// User-triggered failure case
					FAILURE_USER
				};

				virtual ~BehaviorListener() {}

					/// This is called when the character is approaching the goal and has begun slowing down.
				virtual void goalSlowdownDistanceReached(hkaiBehavior* behavior, hkaiCharacter* character) {}

					/// This is called when the character is at the goal. hkaiPathFollowingProperties::m_goalDistTolerance is used to determine this.
				virtual void goalReached(hkaiBehavior* behavior, hkaiCharacter* character) {}

					/// This is called if path finding fails to find a valid path between the start and goal nodes
				virtual void pathFailed(hkaiBehavior* behavior, hkaiCharacter* character, FailureCause cause = FAILURE_USER ) {}

					/// This is called if path finding succeeds in finding a valid path between the start and goal nodes
				virtual void pathSucceeded(hkaiBehavior* behavior, hkaiCharacter* character) {}

				/// Data passed to userEdgeEntered below.
				struct UserEdgeEntry
				{
					HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, hkaiCharacter::BehaviorListener::UserEdgeEntry);

					/// The runtime ID of the nav mesh instance the entry edge is on
					hkaiRuntimeIndex m_entrySection;

					/// The point on the user edge being entered, in instance-local coordinates
					hkVector4 m_entryPoint;

					/// The userdata attached to the user edge being entered. If path-based steering is used, 
					/// only the first element is valid.
					hkaiNavMesh::EdgeData const* m_entryDataPtr;

					/// The runtime ID of the nav mesh instance the exit edge is on
					hkaiRuntimeIndex m_oppositeSection;

					/// The tangent direction of the entry edge, pointing from point A to point B, in the 
					/// space of the entry edge section. This may not be normalized.
					///
					/// Only edge-based steering correctly fills this field. If path-based steering is 
					/// used, this will be set to zero.
					hkVector4 m_entryEdgeDirection;

					/// If edge-based steering is used, a transformation which maps points 
					/// on the exit edge (in the space of the exit edge section) to points on the entry edge
					/// (in the space of the entry edge section). If path-based steering is used, the
					/// transform will map the exit point to the entry point, but is not otherwise valid.
					hkMatrix4 m_edgeTransform;

					/// The AI world
					hkaiWorld* m_world;

					/// Calculates the point on the user edge being entered, in world coordinates
					void calcEntryPointWorld(hkVector4 & entryPointWorldOut) const;

					/// Calculates the point on the user edge exit corresponding to the entry point,
					/// in the user edge exit's instance's local coordinates
					void calcExitPointLocal(hkVector4 & exitPointLocalOut) const;

					/// Calculates the point on the user edge exit corresponding to the entry point,
					/// in world coordinates
					void calcExitPointWorld(hkVector4 & exitPointWorldOut) const;

					/// Calculates the tangent direction of the entry edge, pointing from point A to point
					/// B, in world space. This may not be normalized.
					void calcEntryEdgeDirectionWorld(hkVector4 & entryEdgeDirectionWorldOut) const;

					/// Calculates the tangent direction of the exit edge, pointing from point A to point
					/// B, in the space of the exit edge section. This may not be normalized, and
					/// will have a different length than the entry edge direction if the entry edge is not
					/// the same length as the exit edge.
					void calcExitEdgeDirectionLocal(hkVector4 & exitEdgeDirectionLocalOut) const;

					/// Calculates the tangent direction of the exit edge, pointing from point A to point
					/// B, in the space of the exit edge section. This may not be normalized, and
					/// will have a different length than the entry edge direction if the entry edge is not
					/// the same length as the exit edge.
					void calcExitEdgeDirectionWorld(hkVector4 & exitEdgeDirectionWorldOut) const;
				};

				/// This is called when the character enters a user edge. 
				///
				/// \param behavior the behavior invoking this callback
				/// \param character the character entering the user edge
				/// \param userEdgeEntry a struct containing information about the user edge entry.
				virtual void userEdgeEntered(
					hkaiBehavior* behavior, 
					hkaiCharacter* character, 
					UserEdgeEntry const& userEdgeEntry) {}

				/// This is called when the character crosses either the beginning or the end of a user edge. 
				///
				/// \deprecated You should use the other overload of userEdgeCrossed if using hkaiEdgeFollowingBehavior.
				/// The invocation of this callback by hkaiEdgeFollowingBehavior will be removed in a future release.
				virtual void userEdgeCrossed(hkaiBehavior* behavior, hkaiCharacter* character, const hkaiPath::PathPoint& point) {}

				/// This is called immediately before the hkaiWorld processes a path request, and allows you
				/// to modify the request. The behavior itself may also modify the request after this callback
				/// returns. If you return false from this callback, the request will not be processed, path
				/// following will cease, and a pathFailed callback will be generated on the following frame. 
				virtual bool pathRequestProcessing(hkaiNavMeshPathRequestInfo* request) { return true; }

				/// This is called immediately before the hkaiWorld processes a volume path request, and allows 
				/// you to modify the request. The behavior itself may also modify the request after this 
				/// callback returns. If you return false from this callback, the request will not be processed,
				/// path following will cease, and a pathFailed callback will be generated on the following 
				/// frame. 
				virtual bool volumePathRequestProcessing(hkaiNavVolumePathRequestInfo* request) { return true; }
		};

			///
		struct Cinfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiCharacter::Cinfo);
			Cinfo();

				/// Initial position for the character
			hkVector4 m_initialPosition;

				/// Initial forward direction for the character
			hkVector4 m_initialForward;

				/// Initial up direction for the character
			hkVector4 m_up;

				/// Desired speed;
			hkReal m_desiredSpeed;

				/// Settings for local steering/avoidance
			const hkaiAvoidanceProperties* m_avoidanceProperties;
			
				/// Character radius
			hkReal m_radius;

			hkUlong m_userData;
		};

		enum State 
		{
			STATE_NEEDS_NEW_PATH,
			STATE_FOLLOWING_PATH,
			STATE_SLOWING_TO_GOAL,
			STATE_GOAL_REACHED,
			STATE_PATH_FAILED,
			STATE_WANDERED_OFF_PATH,
			STATE_REPATHING_INCOMPLETE_PATH,
			STATE_MANUAL_CONTROL
		};

			/// Mask bits to determine what kinds of entities will be avoided
		enum AvoidanceEnabledMaskBits
		{
				/// Steer away from collision with the boundary edges of nav mesh
			AVOID_BOUNDARIES = 1,
				/// Steer away from collisions with other characters
			AVOID_CHARACTERS = 2,
				/// Steer away from collisions with dynamic obstacles
			AVOID_OBSTACLES = 4,

			AVOID_NONE = 0,

			AVOID_ALL = (AVOID_BOUNDARIES | AVOID_CHARACTERS | AVOID_OBSTACLES)
		};
		typedef hkFlags<AvoidanceEnabledMaskBits, hkUint8> AvoidanceEnabledMask;

		enum AvoidanceState
		{
			AVOIDANCE_SUCCESS,
			AVOIDANCE_FAILURE			
		};

			///
		hkaiCharacter( const Cinfo& cinfo );

		//
		hkaiCharacter(hkFinishLoadedObjectFlag f);

		hkaiCharacter(hkaiCharacter const&);

		~hkaiCharacter();


		
		/* steering */

			/// Read-only access to the avoidance properties.
		inline const hkaiAvoidanceProperties* getAvoidanceProperties() const;

			/// Non-const access to the avoidance properties. This is never called internally (the character and steering never modify the properties),
			/// but users may call this to update properties on the fly.
		inline hkaiAvoidanceProperties* getAvoidancePropertiesRW();

			/// Set the avoidance properties for the character.
		void setAvoidanceProperties( const hkaiAvoidanceProperties* props );


		/* position and velocity */

			/// Get the desired speed.
		inline hkReal getDesiredSpeed() const;

			/// Set the desired speed.
		inline void setDesiredSpeed( hkReal speed );

			/// Get the current position.
		inline const hkVector4& getPosition() const;

			/// Set the position of the character.
		void setPosition( hkVector4Parameter position );

		/// Get the current up direction.
		inline const hkVector4& getUp() const;

		/// Set the up direction of the character.
		void setUp( hkVector4Parameter up );


			/// Get the characters angular velocity.
		inline hkReal getAngularVelocity() const { return m_velocity(3); } 
		

			/// Get the characters velocity. The w component contains the angular velocity.
		inline const hkVector4& getVelocity() const;
		

			/// Simultaneously set the linear and angular velocity. The angular velocity is in the .w component.
		inline void setLinearAndAngularVelocity( hkVector4Parameter velocity );

			/// Set the linear velocity. The angular velocity is unchanged.
		inline void setLinearVelocity( hkVector4Parameter velocity );

			/// Get the characters facing direction.
		inline const hkVector4& getForward() const;
		
			/// Set the characters facing direction.
		inline void setForward(hkVector4Parameter forward);

		
		/* listener, sensor, filter */

			/// Return sensor AABB in world space.
		void getSensorAabb(hkAabb& aabb) const;

		/// Return adaptively contracted sensor AABB in world space.
		void getAdaptiveSensorAabb(hkAabb& aabb) const;


			/// Return sensor AABB in local space.
		void getLocalSensorAabb(hkAabb& aabb) const;

		/// Return sensor AABB in local space.
		void getAdaptiveLocalSensorAabb(hkAabb& aabb) const;

			/// Sets the local space sensor AABB
			///
			/// \deprecated This function overwrites the AABB in AvoidanceProperties, even if it is shared between multiple characters.
		void setSensorAabb(const hkAabb& aabb);

			/// Adjust sensor size, to try to approach the desired 
			/// number of sensed characters during local steering.
		void adaptSensorAabb(hkUint32 numSensedCharacters);

			/// Get the cost modifier for A* searches of the navmesh.
		const hkaiAstarCostModifier* getCostModifier() const;

			/// Sets the cost modifier that the character will use for A* searches of the navmesh
		void  setCostModifier(const hkaiAstarCostModifier*);

			/// Get the edge filter for A* searches of the navmesh and local steering.
		const hkaiAstarEdgeFilter* getEdgeFilter() const;

			/// Sets the edge filter that the character will use for A* searches of the navmesh and local steering
		void  setEdgeFilter(const hkaiAstarEdgeFilter*);

			/// Gets the hit filter the character will use for spatial queries
		const hkaiSpatialQueryHitFilter* getQueryHitFilter() const;

			/// Sets the hit filter the character will use for spatial queries
		void setQueryHitFilter( hkaiSpatialQueryHitFilter* hitFilter );

			/// Gets the hit filter the character will use for spatial queries
		const hkaiLocalSteeringFilter* getSteeringFilter() const;

			/// Sets the hit filter the character will use for spatial queries
		void setSteeringFilter( hkaiLocalSteeringFilter* steeringFilter );

			/// Return the number of behavior listeners currently set on the character
		inline int getNumBehaviorListeners() const;

			/// Return the behavior listener at the current index
		inline BehaviorListener* getBehaviorListener(int listenerIdx) const;

			/// Add the specified behavior listener to the character.
			/// This should not be called during a behavior listener callback.
		inline void addBehaviorListener(BehaviorListener* listener );

			/// Remove the specified behavior listener. If the specified listener is not
			/// currently set as a listener for the character, raises an assertion.
			/// This should not be called during a behavior listener callback.
		inline void removeBehaviorListener(BehaviorListener* listener );

			/// Deprecated method to return the only behavior listener if there is only one, or NULL if 
			/// there are none. If there are multiple behavior listeners, raises an assertion.
			///
			/// \deprecated You should use the 1-argument version of getBehaviorListener instead.
		inline BehaviorListener* getBehaviorListener() const;

			/// Deprecated method for setting a behavior listener.
			///
			/// If the character already had exactly one behavior listener, it is removed. Then, if
			/// listener is not NULL, it is added. If the character has more than one listener, raises
			/// an assertion.
			///
			/// \deprecated you should use addBehaviorListener and/or removeBehaviorListener instead.
		inline void setBehaviorListener(BehaviorListener* listener );

			/// Get the filter info for this character.
		inline hkUint32 getAgentFilterInfo() const;
		
			/// Set the filter info for this character.
		inline void setAgentFilterInfo( hkUint32 newInfo );

			/// Get the filter info for this character.
		inline hkUint32 getAgentPriority() const;

			/// Set the filter info for this character.
		inline void setAgentPriority( hkUint32 priority );

			/// Get the current character state.
		inline State getState() const;

			/// Change the state of the character.
		inline void setState( State state );

			/// Get the current avoidance state.
		inline AvoidanceState getAvoidanceState() const;

			/// Change the state of the character.
		void setAvoidanceState( hkResult state );

			/// Returns the avoidance type of the character.
			/// Avoidance types are used to lookup the avoidance pair properties table
		inline hkUint16 getAvoidanceType() const;

			/// Sets the avoidance type of the character
		inline void setAvoidanceType( hkUint16 type );

			/// Returns whether local avoidance steering is permitted for certain types of entities
			/// Use isAvoidanceEnabled to check whether any avoidance flags are set.
		inline AvoidanceEnabledMask getAvoidanceEnabledMask() const;

			/// Permit or disable local avoidance steering for certain types of entities
		inline void setAvoidanceEnabledMask( AvoidanceEnabledMask mask );

			/// Whether or not any avoidance flags are set
		inline hkBool32 isAvoidanceEnabled() const;
		
			/// Whether or not kinematic constraints are enabled.
		inline hkBool32 areConstraintsEnabled() const;

			/// Get the radius of the character
		inline hkReal getRadius() const;

			/// Set the radius of the character.
			/// Note that this may invalidate the current path of the character - this does
			/// NOT automatically request a new path.
		inline void setRadius(hkReal r);

		
		/// Returns the key of the most recent face the character was on.
		inline hkaiPackedKey getCurrentNavMeshFace() const;

		/// Stores the key of the most recent face the character was on.
		/// This is called automatically during hkaiWorld::step or 
		/// hkaiWorld::stepMultithreaded.
		inline void	setCurrentNavMeshFace( hkaiPackedKey faceKey );

			/// Validate determinism of data. Does nothing unless determinism checks enabled.
		void checkDeterminism();
		
			/// User data.
		hkUlong m_userData;

	protected:

		/// @name Character motion state
		/// 
		/// These motion state variables are the main interface between the hkaiCharacter and the application's 
		/// simulation world: they are used to propagate state information from users' simulation code to the AI
		/// world, and vice versa.
		//@{


		/// Character's current position. This should be updated by calling hkaiCharacter::setPosition() before stepping the hkaiWorld.
		/// \note This is an input state value that must be set by your application (ex. as the result of a physics integration step).
		hkVector4 m_position;

		/// Character's current forward-facing direction. This should be updated by calling hkaiCharacter::setForward() before stepping the hkaiWorld.
		/// \note This is an input state alue that must be set by your application (ex. as the result of a physics integration step).
		hkVector4 m_forward;

		/// Character's current velocity. Linear velocity is stored in the x, y, and z components, and angular velocity is stored in the w component.
		/// This should be updated as input by calling hkaiCharacter::setLinearAndAngularVelocity() before stepping the hkaiWorld .
		/// When the hkaiWorld is stepped, recommended velocity to avoid collisions is stored here as output.
		/// \note This is an input state value that must be set by your application (ex. as the result of a physics integration step). It is also
		/// the main output value of the avoidance steering, and should be used to drive the application's simulation (ex. the next frame's
		/// integration step).
		hkVector4 m_velocity;

		hkVector4 m_up; //+default(0.0f,0.0f,1.0f,0.0f)

		/// Character's current nav mesh face. This is used to optimize spatial
		/// queries, and to define the character's frame of reference when 
		/// working with moving nav mesh instances.
		hkaiPackedKey m_currentNavMeshFace;

		//@}

	protected:

		
		hkReal m_radius;
		hkReal m_desiredSpeed;

		hkaiAdaptiveRanger m_adaptiveRanger;

		// Pathfinding properties
		hkRefPtr<const hkaiAstarCostModifier>		m_costModifier; //+default(HK_NULL)
		hkRefPtr<const hkaiAstarEdgeFilter>		m_edgeFilter; //+default(HK_NULL)
		hkRefPtr<const hkaiSpatialQueryHitFilter>	m_hitFilter; //+nosave
		hkRefPtr<const hkaiLocalSteeringFilter>		m_steeringFilter; //+nosave
		hkUint32 m_agentFilterInfo; //+default(0)

		hkRefPtr<const hkaiAvoidanceProperties> m_avoidanceProperties;

		// <dg.rev> local steering properties

		hkReal m_avoidanceState;		// must be float value for filtering

		hkUint32 m_agentPriority;		//+default(1)

		hkUint16 m_avoidanceType;		//+default(1)

		AvoidanceEnabledMask m_avoidanceEnabledMask;	//+default( hkaiCharacter::AVOID_ALL )

		// <dg.rev> path following properties
		hkEnum<State,int> m_state;

		hkArray<BehaviorListener*> m_behaviorListeners; //+nosave
		
};

#include <Ai/Pathfinding/Character/hkaiCharacter.inl>

#endif // HK_AI_CHARACTER_H

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
