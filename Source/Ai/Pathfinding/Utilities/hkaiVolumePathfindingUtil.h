/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PATHFINDING_UTILITIES_VOLUMEPATHFINDING_UTIL_H
#define HK_AI_PATHFINDING_UTILITIES_VOLUMEPATHFINDING_UTIL_H

#include <Ai/Pathfinding/Astar/hkaiAstarParameters.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolumePathSearchParameters.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/Character/hkaiPath.h>

class hkaiStreamingCollection;
class hkaiNavVolume;

/// A simple utility for pathfinding on a nav volume.
/// Computes the edges that are traveled across, and (optionally) a smoothed path.
class hkaiVolumePathfindingUtil
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiVolumePathfindingUtil);
		HK_DECLARE_REFLECTION();
	
			/// Pathfinding input
		struct FindPathInput : public hkReferencedObject
		{
			// +version(6)
			HK_DECLARE_REFLECTION();
			HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR );

				/// Start point of the path
			hkVector4 m_startPoint;

				/// Goal point(s) of the path. The number of goal points is currently limited to 16 (see hkaiHierarchicalNavMeshHeuristic::MAX_GOALS)
			hkArray<hkVector4> m_goalPoints;

				/// Key of the starting cell. m_startPoint should be contained in this cell.
			hkaiPackedKey m_startCellKey;

				/// Key(s) of the goal cell(s). Each point in m_goalPoints should be contained in the corresponding cell.
			hkArray<hkaiPackedKey> m_goalCellKeys;

				/// The maximum number of A* iterations that should be performed.
				/// This defaults to a very large value, but can be reduced to avoid spending too much time per frame.
			int m_maxNumberOfIterations;

				/// Width, height, filter information, etc. for a character.
				/// If an hkaiAstarCostModifier is specified, these can determine which edges are traversable,
				/// or change the cost of crossing certain cells
			hkaiAgentTraversalInfo m_agentInfo;

				/// Request search parameters
			hkaiNavVolumePathSearchParameters m_searchParameters;

			//
			// Pre-allocated memory buffers
			// If a pointer is not specified here, memory will be allocated during the search
			//

				/// Open set buffer.
				/// Assumed to be of size m_searchParameters.m_maxOpenSetSizeBytes
			char* m_openSetBuffer; //+nosave

				/// Search state buffer.
				/// Assumed to be of size m_searchParameters.m_maxSearchStateSizeBytes
			char* m_searchStateBuffer; //+nosave

			FindPathInput( int numGoals = 0 );
			FindPathInput(hkFinishLoadedObjectFlag f);
			~FindPathInput()
			{

			}

			void checkDeterminism() const;

		private:
			FindPathInput(const FindPathInput& input);

		};

			/// Pathfinding output
		struct FindPathOutput : public hkReferencedObject
		{
			HK_DECLARE_REFLECTION();
			HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR );


			FindPathOutput() {}
			FindPathOutput( hkFinishLoadedObjectFlag f);

				/// List of cells crossed during A*
			hkArray<hkaiPackedKey> m_visitedCells;

				/// List of points for the shortest path
			hkArray<hkaiPath::PathPoint> m_pathOut;

				/// Results from A* search, e.g. # of iterations performed
			hkaiAstarOutputParameters m_outputParameters;

			void checkDeterminism() const;
		};

			/// Compute a path from the points given in the input, and store the results in the output
		static void HK_CALL findPath(const hkaiStreamingCollection& collection, const FindPathInput& input, FindPathOutput& output);

			/// Same as above, but using a single nav volume
		static void HK_CALL findPath(const hkaiNavVolume& navVolume, const FindPathInput& input, FindPathOutput& output);
		
			/// Check whether or not a path exists without turning a corner
		static bool HK_CALL checkLineOfSight(const hkaiStreamingCollection& collection, const FindPathInput& input, int goalIndex, FindPathOutput* output = HK_NULL);

			/// Utility function used internally to determine whether a path on the initial cell can be used
		static hkBool32 HK_CALL _checkInitialCell(const hkaiStreamingCollection& collection, const FindPathInput& input, int& closestGoalIndex );

};

#endif // HK_AI_PATHFINDING_UTILITIES_VOLUMEPATHFINDING_UTIL_H

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
