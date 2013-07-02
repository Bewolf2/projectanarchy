/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_LINE_OF_SIGHT_UTIL_H
#define HKAI_LINE_OF_SIGHT_UTIL_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>

class hkaiGeneralAccessor;
class hkaiAstarCostModifier;

	/// Performs line-of-sight and direct path queries.
	/// For both of these, given a starting point, and edge is traversed depending on the direction of travel.
	/// This continues until either and edge is uncrossable (i.e. because it is a boundary edge) or the goal is 
	/// reached (line-of-sight queries only).
class hkaiLineOfSightUtil
{
public:
	//+serializable(false)	
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR,hkaiLineOfSightUtil );
	
		/// Base class for line-of-sight and direct path queries. Don't use this, use LineOfSightInput or DirectPathInput accordingly.
	struct InputBase
	{
		//+version(2)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR,InputBase );

			/// Controls whether the query is being used for line-of-sight or direct path.
		enum QueryMode 
		{
				/// Performing line-of-sight check
			MODE_LINE_OF_SIGHT,

				/// Performing direct path query
			MODE_DIRECT_PATH,
		};

			/// Constructor. The mode is set by LineOfSightInput or DirectPathInput constructor accordingly.
		InputBase(QueryMode mode);
		InputBase( hkFinishLoadedObjectFlag f );

			/// Key of the starting face. m_startPoint should be contained in this face.
		hkVector4 m_startPoint;

			/// World up vector.
		hkVector4 m_up;

			/// Starting face key.
		HK_PAD_ON_SPU(hkaiPackedKey) m_startFaceKey;

			/// The maximum number of faces iterated through when searching for line of sight.
			/// This defaults to a very large value.
		HK_PAD_ON_SPU(int) m_maxNumberOfIterations;

			/// Width, filter information, etc. for a character.
			/// If an hkaiAstarCostModifier is specified, these can determine which edges are traversable,
			/// or change the cost of crossing certain faces.
		hkaiAgentTraversalInfo m_agentInfo;

			/// Maximum distance that the query will travel from the start point. If negative, the check is ignored.
			/// This check is performed by comparing the current point on the outgoing edge to the start point.
			/// If traversing the next edge would exceed the radius, the last point is adjust so that the character
			/// is tangent to a circle with the search radius centered at the start point.
		HK_PAD_ON_SPU(hkReal) m_searchRadius; //+default(-1.0f)

			/// Maximum accumulated distance that the query can travel from the start point. 
			/// This is intended as a optimization to avoid long queries; the path is not adjust to fit this distance exactly.
		HK_PAD_ON_SPU(hkReal) m_maximumPathLength; //+default(HK_REAL_MAX)

			/// Optional pointer to hkaiAstarCostModifier, which can be used to modify costs based on the hkaiAgentTraversalInfo.
		HK_PAD_ON_SPU(const hkaiAstarCostModifier*) m_costModifier;

			/// Optional pointer to hkaiAstarEdgeFilter, which can be used to reject nav mesh edges
			/// or modify costs based on the hkaiAgentTraversalInfo.
		HK_PAD_ON_SPU(const hkaiAstarEdgeFilter*) m_edgeFilter;

			/// Whether or not to clear the output arrays (if the pointers are non-NULL) if the line-of-sight check fails.
			/// This may be useful to get some information out of failed checks.
		hkBool m_outputEdgesOnFailure; //+default(false)

			/// Whether or not checks between the character position and boundaries should be projected onto the same vertical plane.
		hkBool m_projectedRadiusCheck; //+default(true)

			/// Whether or not a more detailed check should be performed to detect boundary edges near the faces that the path goes through.
			/// If set to false, the query is conservative, and may stop even though a path does exist. In particular, "internal" vertices act as
			/// obstacles in this mode.
			/// If set to true, the query is more accurate but might be slower.
			/// It defaults to true when calling hkaiLineOfSightUtil directly, but by defaut is disabled as an early out
			/// to A* (see hkaiNavMeshPathSearchParameters::HANDLE_INTERNAL_VERTICES).
			/// If an edge filter is specified, it will be used to check nearby boundaries, but fields in the
			/// hkaiNavMeshEdgePairInfo passed to hkaiAstarEdgeFilter::isEnabled() are used slightly differently than normal:
			///  - The incoming edge is NULL.
			///  - The outgoing face is the edge that we're examining right now, but the point is where this iteration stops.
			///  - The face is the one which the outgoing edge belongs to, but might not the opposite face of the incoming edge.
			///  - The incoming and outgoing positions are the "pointOnPreviousEdge" and "pointOnEdge" from the main iteration.
		hkBool m_exactInternalVertexHandling; //+default(true)

			/// Whether or not the query represents wall-climbing movement. In this case the direction will be updated at each face based on
			/// the face normals.
		hkBool m_isWallClimbing; //+default(false)

			/// Whether the query is a line-of-sight check or a direct path check.
		const hkEnum<QueryMode, hkUint8> m_mode;
	};

		/// Input structure for line-of-sight queries. This essentially checks whether the goal point can be reached from 
		/// the start point without turning.
	struct LineOfSightInput : InputBase
	{
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR,LineOfSightInput );
		LineOfSightInput();
		LineOfSightInput( hkFinishLoadedObjectFlag f );
		
			/// Goal point of the query.
		hkVector4 m_goalPoint;

			/// Key of the goal face. m_goalPoint should be contained in this face.
		HK_PAD_ON_SPU(hkaiPackedKey) m_goalFaceKey;
	};

		/// Input structure for direct path queries. These determine the furthest in a specific direction that a character can move.
	struct DirectPathInput : InputBase
	{
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR,DirectPathInput );
		DirectPathInput();
		DirectPathInput( hkFinishLoadedObjectFlag f );

			/// Direction that the query will travel.
		hkVector4 m_direction;
	};

		/// Return values for findDirectPath. These are also used internally during the query.
	enum LineOfSightResult
	{
			/// The input to the query (e.g. face key) was bad.
		RESULT_INVALID_INPUT,

			/// The algorithm can't advance past the current edge.
			/// This can occasionally happen if the query passes directly through an internal vertex.
		RESULT_CANT_ADVANCE,

			/// The internal storage used during _checkAgainstNearbyBoundaries was filled during the query.
			/// This should happen very rarely in practice, only when there are several faces which are small compared to the character size.
		RESULT_REACHED_BOUNDARY_CHECK_LIMIT,

			/// The query is still progressing. This is only used internally during the query and should never be returned.
		RESULT_IN_PROGRESS,

			/// The query reached a boundary edge and stopped.
		RESULT_HIT_BOUNDARY_EDGE,

			/// The query hit an edge on which hkaiAstarCostModifier::isEdgeEnabled return false.
		RESULT_HIT_REJECTED_EDGE,

			/// InputBase::m_searchRadius was reached.
		RESULT_REACHED_RADIUS_LIMIT,

			/// InputBase::m_maximumPathLength was reached
		RESULT_REACHED_PATH_LENGTH_LIMIT,

			/// The maximum number of iterations was reached.
		RESULT_REACHED_ITERATION_LIMIT,

			/// The query successfully reached the goal. This will never be returned by findDirectPath since there is no goal.
		RESULT_REACHED_GOAL,
	};

		/// Output information for checkLineOfSight and findDirectPath.
	struct LineOfSightOutput
	{
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR,LineOfSightOutput);
		LineOfSightOutput();
		LineOfSightOutput( hkFinishLoadedObjectFlag f );

			/// Optional output array of edge keys crossed during the query.
		hkArray<hkaiPackedKey> m_visitedEdgesOut; 

			/// Optional output array of the accumulated distance (including costs modified by hkaiAstarCostModifier::getModifiedCost) to each edge.
			/// If the goal was reached during checkLineOfSight, the total cost to the goal will be the last entry in the array.
		hkArray<hkReal> m_distancesOut;

			/// Optional output array of the path projected onto the nav mesh edges.
			/// If the goal was reached during checkLineOfSight, the goal will be the last point in the array.
		hkArray<hkVector4> m_pointsOut;

			/// If true, the arrays are assumed to have fixed size and will not be expanded.
		hkBool m_doNotExceedArrayCapacity; //+default(false);

			/// Number of iterations performed during the query.
		HK_PAD_ON_SPU(int) m_numIterationsOut; //+default(0);

			/// The face key on which the query terminated.
		HK_PAD_ON_SPU( hkaiPackedKey ) m_finalFaceKey;

			/// The total distance traveled (including costs modified by hkaiAstarCostModifier::getModifiedCost)
		HK_PAD_ON_SPU( hkReal ) m_accumulatedDistance; //+default(0.0f)

			/// The point where the query terminated.
		hkVector4 m_finalPoint; 

		inline bool canAddEdge()
		{
			return (!m_doNotExceedArrayCapacity || m_visitedEdgesOut.getSize() < m_visitedEdgesOut.getCapacity() ) ;
		}

		inline bool canAddDistance()
		{
			return (!m_doNotExceedArrayCapacity || m_distancesOut.getSize() < m_distancesOut.getCapacity() ) ;
		}

		inline bool canAddPoint()
		{
			return (!m_doNotExceedArrayCapacity || m_pointsOut.getSize() < m_pointsOut.getCapacity() ) ;
		}
	};

	typedef LineOfSightOutput DirectPathOutput;

		/// Check whether or not a straight-line path exists between the start and end points.
		/// If LineOfSightInput::m_startFaceKey or LineOfSightInput::m_goalFaceKey are set to HKAI_INVALID_PACKED_KEY,
		/// the check will early out and return false.
	static hkBool32 HK_CALL checkLineOfSight(
		const hkaiStreamingCollection::InstanceInfo* streamingInfo, 
		const LineOfSightInput& input,
		LineOfSightOutput& output );

	/// Check whether or not a straight-line path exists between the start and end points.
	/// If LineOfSightInput::m_startFaceKey or LineOfSightInput::m_goalFaceKey are set to HKAI_INVALID_PACKED_KEY,
	/// the check will early out and return false.
	static hkBool32 HK_CALL checkLineOfSight(
		const hkaiStreamingCollection& collection, 
		const LineOfSightInput& input,
		LineOfSightOutput& output );

		/// Compute the furthest point that can be reached in a specific direction.
	static hkaiLineOfSightUtil::LineOfSightResult HK_CALL findDirectPath(
		const hkaiStreamingCollection& collection, 
		const DirectPathInput& input,
		DirectPathOutput& output );

		/// Main method of both checkLineOfSight and findDirectPath.
		/// Call those instead of this.
	static hkaiLineOfSightUtil::LineOfSightResult HK_CALL _lineOfSightInternal( 
		hkVector4Parameter direction, 
		hkVector4Parameter goalPoint, 
		hkaiPackedKey goalKey, 
		const hkaiStreamingCollection::InstanceInfo* streamingInfo, 
		const InputBase& input,
		LineOfSightOutput& output );

	//
	// Internal method(s)
	//

		/// Check if the segment from the start to the end passes within the character radius of a boundary edge.
	static LineOfSightResult HK_CALL _checkAgainstNearbyBoundaries( hkVector4Parameter _start, 
		hkVector4Parameter _end,
		hkVector4Parameter currentUp,
		hkSimdRealParameter radius, 
		const class hkaiGeneralAccessor& accessor,
		const hkaiLineOfSightUtil::InputBase& input,
		hkaiPackedKey faceKeyIn, 
		hkSimdReal& hitFractionInOut );

	static hkBool32 HK_CALL _raycastAgainstBoundary( hkVector4Parameter start, 
		hkVector4Parameter end, 
		hkSimdRealParameter radius, 
		bool shouldProject,
		hkVector4Parameter up,
		hkVector4Parameter edgeA, hkVector4Parameter edgeB,
		hkSimdReal& hitFractionInOut );
};

#endif // HKAI_LIGHT_OF_SIGHT_UTIL_H

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
