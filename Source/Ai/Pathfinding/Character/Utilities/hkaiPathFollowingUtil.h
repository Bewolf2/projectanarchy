/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PATH_FOLLOWING_UTIL_H
#define HK_AI_PATH_FOLLOWING_UTIL_H

#include <Common/Base/Object/hkReferencedObject.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>

class hkaiCharacter;
class hkaiReferenceFrame;
class hkaiPath;

extern const class hkClass hkaiPathFollowingPropertiesClass;


	/// Properties that control how characters follow a path
	/// \note These properties are kept in an \a hkReferencedObject to facilitate easy sharing among many behaviors
struct hkaiPathFollowingProperties : public hkReferencedObject
{
	// +vtable(true)
	// +version(6)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING);

	hkaiPathFollowingProperties();
	hkaiPathFollowingProperties(hkFinishLoadedObjectFlag f) : hkReferencedObject(f) {}

	/// Maximum distance a character can wander from the path before a new path is requested
	hkReal m_repathDistance;	//+default(2.0f)

	/// If a character has an incomplete path, they will repath when they reach this number of segments
	/// from the end of that path.
	hkInt32 m_incompleteRepathSegments; //+default(8)

	/// If a character deviates from the current segment by more than this value then the entire path is searched to find a way back
	
	hkReal m_searchPathLimit;	//+default(5.0f)

	/// this is the desired speed we want the character to have when they reach the goal
	hkReal m_desiredSpeedAtEnd; //+default(0.0f) 

	/// A character that is within this distance of the goal is considered to have reached the goal
	hkReal m_goalDistTolerance; //+default(0.25f)

	/// A character that is within this distance of a user edge is considered to have entered the user edge
	hkReal m_userEdgeTolerance; //+default(0.25f)

	/// If true, attempt to repair paths when the character moves onto a nav mesh face which was not part
	/// of the path. This is only performed by edge-based steering.
	hkBool m_repairPaths; //+default(true)

	/// If true, when a character moves through a user edge, it will immediately change its state to
	/// STATE_MANUAL_CONTROL, and the behavior will not perform any steering until the user sets the 
	/// state back to STATE_FOLLOWING_PATH.
	hkBool m_setManualControlOnUserEdges; //+default(false)

	/// If true, attempt to dynamically improve paths by finding more efficient routes around internal
	/// nav mesh vertices. This is only performed by edge-based steering.
	hkBool m_pullThroughInternalVertices; //+default(true)
};

/// Defines the character state needed to update path following.
/// This state data is stored in the frame of reference of the character
struct hkaiPathFollowingCharacterState
{
	hkVector4 m_position;
	hkVector4 m_forward;
	hkVector4 m_velocity;
	hkVector4 m_up;
	hkReal m_radius;
	hkReal m_desiredSpeed;
};

	/// Utilities for handling path following (on both nav mesh and nav volumes)
class hkaiPathFollowingUtil
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, hkaiPathFollowingUtil);

	typedef hkaiPathFollowingCharacterState CharacterState;

		/// Character's relative position along a path.
	struct PathPosition
	{
			/// Shortest distance from the character to the path
		hkReal m_distFromPath;

			/// Distance to the end of the path.
			/// \note This is not a straight line distance and represents the cumulative distance along the path to the end.
		hkReal m_distFromEnd;

			/// Current path segment
		int m_pathSegment;

			/// Distance from the projecion onto the line segment to the end of the current segment.
		hkReal m_distFromProjectionToSegmentEnd;

			/// Distance from the projecion onto the line segment to the start of the current segment.
		hkReal m_distFromProjectionToSegmentStart;
	};

		/// Calculate the distance at which this character should start to slow down before reaching the path goal
	static hkReal HK_CALL computeSlowdownToGoalDistance( hkReal desiredSpeed, hkReal desiredSpeedAtEnd, const hkaiAvoidanceSolver::MovementProperties& movementProperties );

		///
	static hkReal HK_CALL computeDefaultChangeSegmentDistance( const hkaiAvoidanceSolver::MovementProperties& movementProperties );

		/// Get the character's current state data in the desired frame of reference
	static void HK_CALL initCharacterState( const hkaiCharacter& character, const hkaiReferenceFrame* characterReferenceFrame, CharacterState& characterState );

		/// Determine where along the path the character currently lies
	static void HK_CALL updatePathPosition( const CharacterState& characterState, const hkaiPathFollowingProperties& pathFollowingProperties,
		const hkaiPath& path, int prevPathSegment, hkReal changeSegmentDist, PathPosition& output );

		/// Determine the character's desired velocity to follow the current path
	static void HK_CALL getPathFollowingVelocity( const CharacterState& characterState, const hkaiAvoidanceSolver::MovementProperties& movementProperties, 
		const hkaiPathFollowingProperties& pathFollowingProperties, const hkaiPath& path, const PathPosition& input, 
		hkVector4& velocityOut, hkReal& changeSegmentDistanceInOut );

		/// Look ahead to predict the next direction a path will take
	static void HK_CALL getDesiredFutureDirOnPathByDistance( const CharacterState& characterState, const hkaiPath& path, int currentSegmentId,
		hkReal distance, hkVector4& desiredFutureDir );

		/// Look ahead and smooth the path normal 
	static void HK_CALL getSmoothedNormalOnPathByDistance( const hkaiCharacter& character, const hkaiPath& path, int currentSegmentId,
		hkReal distance, hkVector4& smoothedNormal);

	static void HK_CALL getDistsFromLineSegment( hkVector4Parameter position, hkVector4Parameter segStart, hkVector4Parameter segEnd, hkVector4Parameter normal, 
		hkReal& distFromProjectionToSegStart, hkReal& distFromProjectionToSegEnd, hkReal& perpDistFromLine, hkReal& projectedDistToEnd );

};

#endif // HK_AI_PATH_FOLLOWING_UTIL_H

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
