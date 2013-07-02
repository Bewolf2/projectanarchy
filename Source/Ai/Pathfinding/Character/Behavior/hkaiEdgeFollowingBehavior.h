/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_EDGE_FOLLOWING_BEHAVIOR_H
#define HKAI_EDGE_FOLLOWING_BEHAVIOR_H

#include <Ai/Pathfinding/Character/Behavior/hkaiSingleCharacterBehavior.h>
#include <Ai/Pathfinding/Character/LocalSteering/EdgePath/hkaiEdgePath.h>
#include <Ai/Pathfinding/Character/Utilities/hkaiPathFollowingUtil.h>
#include <Ai/Pathfinding/Character/hkaiCharacter.h>

class hkaiEdgePath;
class hkaiCornerPredictor;

// #define HKAI_DEBUG_INTERNAL_VERTEX_PULLING

/// hkaiEdgeFollowingBehavior maintains a list of all the corners along the path
/// and recalculates the optimal steering direction every frame. This reduces
/// artificial chokepoints at wall corners, and improves the smoothness of
/// individual agent trajectories when rounding corners. It may also produce more
/// efficient paths when traversing user edges, particularly when paired user
/// edges are of different lengths. 
class hkaiEdgeFollowingBehavior : public hkaiSingleCharacterBehavior
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING);

		/// Construction info
	struct Cinfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_STEERING, hkaiEdgeFollowingBehavior::Cinfo);

		Cinfo()
			:	m_pathFollowingProperties(HK_NULL),
			m_updateQuerySize(2.0f),
			m_characterRadiusMultiplier(1.5f),
			m_maxIgnoredHeight(1.0f)
		{
		}

			/// Reference to path-following parameters. If this is NULL, then default parameters will be created.
		const hkaiPathFollowingProperties* m_pathFollowingProperties;

			/// Raycast length and AABB query size when determining nav mesh face from position
		hkReal m_updateQuerySize;

			/// The character radius is multiplied by this before A* queries.
		hkReal m_characterRadiusMultiplier;

			/// If the character is within this height offset of the goal, the height offset will be disregarded
			/// when calculating distance from the goal.
		hkReal m_maxIgnoredHeight;
	};

	hkaiEdgeFollowingBehavior(hkaiCharacter* character, hkaiWorld* world, Cinfo const& cinfo);

	virtual void update( hkReal timestep ) HK_OVERRIDE;

	virtual int calcVelocities( hkReal timestep, hkaiLocalSteeringInput* outputs, int outputsSize ) HK_OVERRIDE;

	void cancelRequestedPath();

	void forceEdgePath( hkaiEdgePath* edgePath );

	virtual void repath( int priority = 0 ) HK_OVERRIDE;

	virtual void getApproximateFuturePositions(int characterIndex, hkArray<hkVector4>::Temp& positions) const HK_OVERRIDE;

	virtual void getFutureBoundingBoxes( hkArray<hkAabb>::Temp& regions) const HK_OVERRIDE;

	virtual bool pathsThroughAnySection(hkSet<hkaiRuntimeIndex> const& sections) const HK_OVERRIDE;

	virtual bool prePathSearchCallback(class hkaiNavMeshPathRequestInfo* request) HK_OVERRIDE;
	virtual bool prePathSearchCallback(class hkaiNavVolumePathRequestInfo* request) HK_OVERRIDE;

	virtual bool hasPath() const;

		/// Get the character radius multiplier.
	inline hkReal getCharacterRadiusMultiplier() const;

		/// Sets the character radius mutiplier.
	inline void setCharacterRadiusMultiplier( hkReal m );

		/// Get the reference to the path-following parameters
	inline const hkaiPathFollowingProperties* getPathFollowingProperties() const;

		/// Non-const access to the path following properties. This is never called internally (the character and steering never
		/// modify the properties), but users may call this to update properties on the fly.
	inline hkaiPathFollowingProperties* getPathFollowingPropertiesRW();

		/// Update the reference to new path-following parameters
	inline void setPathFollowingProperties( const hkaiPathFollowingProperties* pathFollowingProperties );

	virtual void takeManualControl(int characterIdx = -1) HK_OVERRIDE;

	virtual void releaseManualControl(int characterIdx = -1) HK_OVERRIDE;

	virtual hkaiCharacter::State getSavedCharacterState() HK_OVERRIDE { return m_savedCharacterState; }

	void getCornerPredictor(hkaiCornerPredictor & cornerPredictorOut) const;

protected:
	void handlePossibleNewPath();

private:
	void fillFakePathPoint( int edgeIdx );

		/// Raycast length and AABB query size when determining nav mesh face from position
	hkReal m_updateQuerySize;

		/// The character radius is multiplied by this before A* queries.
	hkReal m_characterRadiusMultiplier;

	/// If the character is within this height offset of the goal, the height offset will be disregarded
	/// when calculating distance from the goal.
	hkReal m_maxIgnoredHeight;

		/// Saved goal information.
	struct RequestedGoalPoint
	{
		hkVector4 m_position;	
		/// Runtime index of the section this goal point is attached to
		hkaiRuntimeIndex m_sectionId;
	};

	hkArray<RequestedGoalPoint> m_requestedGoalPoints;

	hkRefPtr<hkaiEdgePath> m_edgePath;

	hkaiEdgePath::TraversalState m_traversalState;
	
	hkaiCharacter::State m_newCharacterState;

	hkRefPtr<const hkaiPathFollowingProperties> m_pathFollowingProperties;

	hkInt32 m_highestUserEdgeNotified;

	hkaiPath::PathPoint m_userEdgeFakePathPoint;

	hkaiCharacter::State m_savedCharacterState;

	struct CornerPredictorInitInfo
	{
		hkVector4 m_positionLocal;
		hkVector4 m_forwardVectorLocal;
		hkVector4 m_upLocal;
		hkaiRuntimeIndex m_positionSectionIndex;
		int m_nextEdgeIndex;
		hkBool m_nextIsLeft;
		hkBool m_hasInfo;
	};

	/// Extra info used to init corner predictors
	CornerPredictorInitInfo m_cornerPredictorInitInfo;
};

#include <Ai/Pathfinding/Character/Behavior/hkaiEdgeFollowingBehavior.inl>

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
