/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PATHFINDING_JOBS_H
#define HK_AI_PATHFINDING_JOBS_H

#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshPathSearchParameters.h>
#include <Ai/Pathfinding/Astar/CostModifier/hkaiDefaultAstarCostModifier.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.h>
#include <Ai/Pathfinding/Character/hkaiPath.h>
#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>

struct hkaiAstarOutputParameters;


/// Jobs for computing shortest paths on directed graphs and nav meshes.
struct hkaiPathfindingJob : public hkJob
{
	enum JobSubType
	{
		JOB_PATHFINDING_DIRECTED_GRAPH_ASTAR,
		JOB_PATHFINDING_NAVMESH_ASTAR,
		JOB_PATHFINDING_NAVMESH_LINE_OF_SIGHT,
		JOB_PATHFINDING_END
	};

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiPathfindingJob );

	HK_FORCE_INLINE hkaiPathfindingJob( JobSubType type, hkUint16 size )
		:	hkJob( HK_JOB_TYPE_AI_PATHFINDING, type, size),
			m_semaphore(HK_NULL),
			m_jobDoneFlag(HK_NULL)
	{
	}

	/// Called at the end of the job
	/// m_semaphore will be released if non-NULL, and the m_jobDoneFlag will be incremented of non-NULL
	inline void atomicIncrementAndReleaseSemaphore() const;

	/// This semaphore is released once the original job (and all its spawned children) has finished.
	hkSemaphoreBusyWait* m_semaphore;

	/// The variable at this location will be incremented (atomically) when the job is done.
	hkUint32* m_jobDoneFlag;
};


/// Command for performing an A* search on a directed graph.
/// Each command corresponds to a single path; several commands can be performed in a single job.
struct hkaiDirectedGraphAStarCommand
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiDirectedGraphAStarCommand);

	/// Initializes data to sane values
	HK_FORCE_INLINE void init();


	/// Key of the starting node.
	hkaiPackedKey m_startNodeKey;

	/// Key of the goal node.
	hkaiPackedKey m_goalNodeKey;

	/// The maximum number of A* iterations that should be performed.
	/// This defaults to a very large value, but can be reduced to avoid spending too much time per frame.
	int m_maxNumberOfIterations;

	//
	// Output
	//

	/// Optional output parameters (to query number of iterations, search status, etc.)
	hkaiAstarOutputParameters* m_AStarOutput;

	/// Main memory pointer where the search results are written out.
	/// The last node is followed by a -1 to indicate the end of the results
	hkaiPackedKey* m_nodesOut;

	/// Size of the output array.
	int m_maxNodesOut;
};

	/// A job that runs multiple A* searches on a single directed graph collection.
	/// The individual path requests are represented as an array of hkaiDirectedGraphAStarCommands.
	/// Note that this job is PPU only, to conserve SPU code space
struct hkaiDirectedGraphAStarJob : public hkaiPathfindingJob
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiDirectedGraphAStarJob );
	
	HK_FORCE_INLINE hkaiDirectedGraphAStarJob( const hkaiStreamingCollection& collection );

	/// Search parameters
	hkaiGraphPathSearchParameters m_searchParameters;

	/// Pointer to streaming collection's information
	const hkaiStreamingCollection::InstanceInfo* m_streamingSectionInfo;

	/// Array of search commands to be processed
	hkaiDirectedGraphAStarCommand* m_commands;

	/// Number of search commands
	int m_numCommands;
};


/// Command for performing an A* search on a nav mesh.
/// Each command corresponds to a single path; several commands can be performed in a single job.
struct hkaiNavMeshAStarCommand
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshAStarCommand);
	enum
	{
		/// Maximum number of goals allowed
		MAX_GOALS = 16
	};

		/// Initializes data to sane values
	HK_FORCE_INLINE void init();

		/// Set the start point and start face for the search
	HK_FORCE_INLINE void setStartPointAndFace( hkVector4Parameter startPoint, hkaiPackedKey startFaceKey );

		/// Set a single goal for the search.
		/// To search with multiple goals, use setMultipleGoalPointsAndFaces
	HK_FORCE_INLINE void setGoalPointAndFace( hkVector4Parameter goalPoint, hkaiPackedKey goalFaceKey );

		/// Set multiple goals for the search.
		/// To search with a single goal, use setGoalPointAndFace
		/// \param goalPoints Array of goal points. This memory must remain valid until the command is processed.
		/// \param goalFaceKeys Array of goal face keys. This memory must remain valid until the command is processed.
		/// \param numGoal Number of elements in both the goalPoints and goalFaceKeys arrays
	HK_FORCE_INLINE void setMultipleGoalPointsAndFaces( const hkVector4* goalPoints, const hkaiPackedKey* goalFaceKeys, int numGoals );


		/// Start point
	hkVector4 m_startPoint;

		/// Goal point.
		/// If there are multiple goals, the goal points are stored in m_multipleGoalPoints
	hkVector4 m_goalPoint;

	/// Key of the start face
	hkaiPackedKey m_startFaceKey;

	/// Key of the goal face.
	/// If there are multiple goals, the goal faces are stored in m_multipleGoalFaceKeys
	hkaiPackedKey m_goalFaceKey;
	
		/// Array of goal points, for multiple-goal queries.
		/// Size of the array is given by m_numGoals.
	const hkVector4* m_multipleGoalPoints;

		/// Array of goal face keys, for multiple-goal queries.
		/// Size of the array is given by m_numGoals.
	const hkaiPackedKey* m_multipleGoalFaceKeys;

		/// Total number of goals, equal to 1 for single-goal queries, greater than 1 for multiple-goal queries.
		/// For multiple-goal queries, this is the size of both the m_multipleGoalPoints and m_multipleGoalFaceKeys arrays.
	int m_numGoals;

		/// The maximum number of A* iterations that should be performed.
		/// This defaults to a very large value, but can be reduced to avoid spending too much time per frame.
	int m_maxNumberOfIterations;

		/// Traversal information to control edge filtering, cost modification, etc.
	hkaiAgentTraversalInfo m_agentInfo;

	//
	// Output
	//

	
		/// Optional output parameters (to query number of iterations, search status, etc.)
	hkaiAstarOutputParameters* m_AStarOutput;

		/// List of edges visited along the final path. The last edge in the list is followed by a terminating HKAI_INVALID_PACKED_KEY.
	hkaiPackedKey* m_edgesOut;

		/// Maximum length of the m_edgesOut array. Note that limiting the size of the output will not early-out of the search.
	int m_maxEdgesOut;
	
		/// Results of path smoothing. The last point in the list is followed by a terminator point.
		/// This final point can be identified with hkaiPath::PathPoint::isTerminator()
	hkaiPath::PathPoint* m_pointsOut; 
	
		/// Maximum length of the smoothed path.  Note that limiting the size of the output will not early-out of the search.
	int  m_maxPointsOut;

	

};


	/// A job that runs multiple A* searches on a single nav mesh collection.
	/// The individual path requests are represented as an array of hkaiNavMeshAStarCommands.
struct hkaiNavMeshAStarJob : public hkaiPathfindingJob
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiNavMeshAStarJob );

	HK_FORCE_INLINE hkaiNavMeshAStarJob( const hkaiStreamingCollection& collection );
		
		/// Initializes the relevant fields in the job from the pathfinding input
	HK_FORCE_INLINE void set( const hkaiPathfindingUtil::FindPathInput& input);

		/// Returns true if all the relaxant fields in the job equal to the pathfinding input
	HK_FORCE_INLINE hkBool32 isEquivalent( const hkaiPathfindingUtil::FindPathInput& input ) const;

		/// Search parameters
	hkaiNavMeshPathSearchParameters m_searchParameters;
		
		/// The maximum number of search iterations that should be performed over the whole set of search commands.
		/// This defaults to a very large value, but can be reduced to avoid spending too much time per frame.
	int m_maxTotalIterations;

		/// Pointer to streaming collection's information
	const hkaiStreamingCollection::InstanceInfo* m_streamingSectionInfo;

		/// Array of search commands to be processed
	hkaiNavMeshAStarCommand* m_commands;

		/// Number of search commands
	int m_numCommands;
};


/// Command for performing a line of sight check on a nav mesh.
/// Each command corresponds to a single check; several commands can be performed in a single job.
struct hkaiNavMeshLineOfSightCommand
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshLineOfSightCommand);
	/// Initializes data to sane values
	HK_FORCE_INLINE void init();

	/// Start point.
	hkVector4 m_startPoint;

	/// Goal point.
	hkVector4 m_endPoint;

	/// Key of the starting face. m_startPoint should be contained in this face.
	hkaiPackedKey m_startFaceKey; //+default(HKAI_INVALID_PACKED_KEY)

	/// Key of the goal face. m_endPoint should be contained in this face.
	hkaiPackedKey m_endFaceKey; //+default(HKAI_INVALID_PACKED_KEY)

	/// Traversal information to control edge filtering, cost modification, etc.
	hkaiAgentTraversalInfo m_agentInfo;

	/// Edges will be rejected if they do not intersect a sphere of this radius, centered on the start point.
	/// If this value is negative, the check will be skipped
	hkReal m_searchSphereRadius; //+default(-1.0f)

	/// Whether or to output the crossed edges if the line-of-sight check fails (and if the m_edgesOut is non-NULL).
	/// This may be useful to get some information out of failed checks.
	hkBool m_outputEdgesOnFailure; //+default(false)

	/// Whether or not a more detailed check should be performed to detect boundary edges near the faces that the path goes through.
	/// See hkaiLineOfSightUtil::InputBase::m_exactInternalVertexHandling for more details.
	hkBool m_exactInternalVertexHandling; //+default(true)

	//
	// Output
	//

	/// Main memory pointer where the result of the line of sight check is written out
	hkBool* m_result;

	/// List of edges visited along the final path. The last edge in the list is followed by HKAI_INVALID_PACKED_KEY.
	hkaiPackedKey* m_edgesOut;

	/// Maximum length of the m_edgesOut array. Note that limiting the size of the output will not early-out of the search.
	int m_maxEdgesOut;
};


/// A job that runs multiple line of sight checks on a single nav mesh.
/// The searches are represented as an array of hkaiNavMeshLineOfSightCommands.
struct hkaiNavMeshLineOfSightJob : public hkaiPathfindingJob
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiNavMeshLineOfSightJob );

	HK_FORCE_INLINE hkaiNavMeshLineOfSightJob( const hkaiStreamingCollection& collection);

	/// Pointer to the nav mesh being used
	const hkaiStreamingCollection::InstanceInfo* m_streamingInfo;

	/// Global up direction
	hkVector4 m_up;

	/// Array of line of sight commands to be processed
	hkaiNavMeshLineOfSightCommand* m_commands;

	/// Number of search commands
	int m_numCommands;

	/// Maximum number of faces iterated through when searching for line of sight.
	int m_maxNumberOfIterations;

	/// Optional pointer to hkaiAstarEdgeFilter, which can be used to reject nav mesh edges
	const hkaiAstarEdgeFilter* m_edgeFilter;
};

#include <Ai/Pathfinding/Multithreaded/hkaiPathfindingJobs.inl>

#endif // HK_AI_PATHFINDING_JOBS_H

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
