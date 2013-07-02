/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PATHFOLLOW_BEHAVIOR_H
#define HK_AI_PATHFOLLOW_BEHAVIOR_H

#include <Ai/Pathfinding/Character/Behavior/hkaiSingleCharacterBehavior.h>
#include <Ai/Pathfinding/Character/Utilities/hkaiPathFollowingUtil.h>
#include <Ai/Pathfinding/Character/hkaiCharacter.h>

class hkaiWorld;

	/// The default behavior class. Manages a single character, driving it towards the next segment.
class hkaiPathFollowingBehavior : public hkaiSingleCharacterBehavior
{
public:

	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING);

	enum PathType
	{
		PATH_TYPE_NAVMESH,
		PATH_TYPE_NAVMESH_CLIMBING,
		PATH_TYPE_NAVVOLUME,
	};
	
		/// Construction info
	struct Cinfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, hkaiPathFollowingBehavior::Cinfo);

		Cinfo()
		:	m_pathFollowingProperties(HK_NULL),
			m_updateQuerySize(2.0f),
			m_characterRadiusMultiplier(1.5f),
			m_characterToPathStartThreshold(HK_REAL_MAX),
			m_useSectionLocalPaths(false),
			m_pathType(PATH_TYPE_NAVMESH)
		{
		}

			/// Reference to path-following parameters. If this is NULL, then default parameters will be created.
		const hkaiPathFollowingProperties* m_pathFollowingProperties;
		
			/// Raycast length and AABB query size when determining nav mesh face from position
		hkReal m_updateQuerySize;

			/// The character radius is multiplied by this before A* queries.
		hkReal m_characterRadiusMultiplier;

			///	When process a path from A*, an extra segment is added between the character's position and the start of the path
			/// if the projected distance between them is greater than this threshold.
		hkReal m_characterToPathStartThreshold;

			/// When this flag is set, the behavior will get paths in section-local space. This is necessary
			/// in order to follow paths with points connected to moving mesh sections. If you are certain
			/// that your paths will be static, then it is slightly more efficient to leave this feature disabled.
		hkBool m_useSectionLocalPaths;

			/// Type of path to compute and follow, e.g. nav mesh, wall climbing, or nav volume.
		hkEnum<PathType, hkUint8> m_pathType;
	};



	hkaiPathFollowingBehavior(hkaiCharacter* character, hkaiWorld* world, const Cinfo& cinfo);

	//
	// hkaiSingleCharacterBehavior / hkaiBehavior interface
	//

	virtual bool prePathSearchCallback(hkaiNavMeshPathRequestInfo* request);
	
	virtual bool prePathSearchCallback(hkaiNavVolumePathRequestInfo* request);
	
		/// Set characters velocity based on path
	virtual int calcVelocities( hkReal timestep, hkaiLocalSteeringInput* outputs, int outputsSize );

		/// Get path for character from manager.
		/// Update desired velocity / future position for character
	virtual void update( hkReal timestep );

	/// Estimate a polyline path which the behavior will cause one of its character(s) to 
	/// follow. Used for debug drawing.
	virtual void getApproximateFuturePositions(int characterIndex, hkArray<hkVector4>::Temp& positions) const;

	/// Get conservative set of bounding boxes through which the behavior may cause its characters to go.
	virtual void getFutureBoundingBoxes( hkArray<hkAabb>::Temp& regions) const;

	/// Returns whether the behavior will cause any of its characters to go through any of the 
	/// sections in the provided set.
	virtual bool pathsThroughAnySection(hkSet<hkaiRuntimeIndex> const& sections) const;

	virtual void forcePath(hkaiPath* path);

	virtual void cancelRequestedPath();

	virtual void repath(int priority = 0);

	//
	// hkaiPathFollowingBehavior methods
	//

		/// Get the reference to the path-following parameters
	inline const hkaiPathFollowingProperties* getPathFollowingProperties() const;

		/// Non-const access to the path following properties. This is never called internally (the character and steering never
		/// modify the properties), but users may call this to update properties on the fly.
	inline hkaiPathFollowingProperties* getPathFollowingPropertiesRW();

		/// Update the reference to new path-following parameters
	inline void setPathFollowingProperties( const hkaiPathFollowingProperties* pathFollowingProperties );

		/// Get index of path segment that the character is currently following.
	inline int getCurrentPathSegment() const;

		/// Set index of path segment that the character is currently following.
	inline int getPreviousPathSegment() const;

		/// Get distance from segment endpoint before switching to the next segment.
	inline hkReal getChangeSegmentDistance() const;

		/// Set distance from segment endpoint before switching to the next segment.
		/// To allow the behavior to set the change segment distance automatically,
		/// set a negative changeSegmentDistance.
	inline void setChangeSegmentDistance(hkReal changeSegmentDistance);

		/// Get the character radius multiplier.
	inline hkReal getCharacterRadiusMultiplier() const;
	
		/// Sets the character radius mutiplier.
	inline void setCharacterRadiusMultiplier( hkReal m );
	
		/// Get the character-to-path-start threshold. See also m_characterToPathStartThreshold.
	inline hkReal getCharacterToPathStartThreshold() const;

		/// Sets the character-to-path-start threshold. See also m_characterToPathStartThreshold.
	inline void setCharacterToPathStartThreshold( hkReal t );


		/// Evaluate the current path in world-space coordinates.
		/// Returns false if there is no current path
	bool getCurrentPathWorldSpace( hkaiPath& pathWS ) const;

		/// Gets the characters next path point in world space.
		/// If no path is available, the character's position is used and the method returns false.
	bool getNextPathPointWorldSpace(hkVector4& pointOut) const;

		/// Get the current path-following type (nav mesh, climbing, nav volume).
	inline PathType getPathType() const;

		/// Changes the path-following type. If different from the old type, the old path is invalidated.
		/// repath() should be called after this to get a new path.
	void setPathType( PathType newType );

		/// Returns whether or not the character is too far of the current path.
	virtual bool wanderedOffPath(hkReal limitDistToPath) const;

	virtual void updateCharacterUpFromPathNormal();

		/// Integrate the character by the velocity, then project its position in the up direction onto the current path segment.
		/// This should be used instead of hkaiCharacterUtil::integrateMotionAndProjectToMesh for wall-climbing characters.
		/// This is not called automatically, it should be called by the user after the hkaiWorld step.
	virtual void integrateMotionAndProjectToPath( hkReal dt, hkReal damping = .5f );

		/// Whether or not the character currently has a path (i.e. is m_currentPath non-NULL)
	inline bool hasPath() const;

	virtual void takeManualControl(int characterIdx = -1) HK_OVERRIDE;

	virtual void releaseManualControl(int characterIdx = -1) HK_OVERRIDE;

	virtual hkaiCharacter::State getSavedCharacterState() HK_OVERRIDE { return m_savedCharacterState; }

protected:

	void handlePossibleNewPath();


		/// Saved goal information.
	struct RequestedGoalPoint
	{
		hkVector4 m_position;	
		/// Runtime index of the section this goal point is attached to
		hkaiRuntimeIndex m_sectionId;
	};

	void setPath(const hkaiCharacterUtil::ProcessedPath& path);

	/// Refresh the fixed-space path, in case any of the local reference frames have moved.
	void refreshFixedPath( hkReal timestep );

protected:
		/// The path-following parameters
	hkRefPtr<const hkaiPathFollowingProperties> m_pathFollowingProperties;

		/// The goal points that were last requested, stored for repath requests
	hkArray< RequestedGoalPoint > m_requestedGoalPoints;

		/// The path that was requested/forced. This may be in local space.
	hkRefPtr< hkaiPath > m_currentPath;

		/// Evaluated version of the current path, fixed to the character's frame of reference.
		/// This is the path data that is actually followed.
		/// This may be updated as the requested path's points' reference frames move.
	hkRefPtr< hkaiPath > m_currentPathFixed;

		/// The index of the segment that the character is on THIS frame.
	int m_currentPathSegment;

		/// The index of the segment from the end of LAST frame.
	int m_previousPathSegment;

		/// The character's state, as determined BEFORE local steering occurs
	int m_newCharacterState;

		/// The character switches to the next path segment when closer than this distance to the end of the current segment.
	hkReal m_changeSegmentDistance;

		/// The change segment distance to be used next frame
	hkReal m_tempChangeSegmentDistance;

/// Raycast length and AABB query size when determining nav mesh face from position
	hkReal m_updateQuerySize;

		/// The character radius is multiplied by this before A* queries.
	hkReal m_characterRadiusMultiplier;

		///	When process a path from A*, an extra segment is added between the character's position and the start of the path
		/// if the projected distance between them is greater than this threshold.
	hkReal m_characterToPathStartThreshold;

		/// When this flag is set, the behavior will get paths in section-local space. This is necessary
		/// in order to follow paths with points connected to moving mesh sections. If you are certain
		/// that your paths will be static, then it is slightly more efficient to disable this feature.
	hkBool m_useSectionLocalPaths;

	hkEnum<PathType, hkUint8> m_pathType;

		/// Whether the last point in the current path, if any, is actually a goal point. If it is not,
		/// the character will automatically repath when approaching the last point.
	hkBool m_lastPointIsGoal;

		/// Whether the agent should request a repath during the next update().
	hkBool m_needsRepath;

	hkaiCharacter::State m_savedCharacterState;
};

#include <Ai/Pathfinding/Character/Behavior/hkaiPathFollowingBehavior.inl>

#endif // HK_AI_PATHFOLLOW_BEHAVIOR_H

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
