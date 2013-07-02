/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VOLUME_LINE_OF_SIGHT_UTIL_H
#define VOLUME_LINE_OF_SIGHT_UTIL_H

#include <Ai/Pathfinding/NavVolume/hkaiNavVolume.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>	// TODO: Streaming collection file should be moved up in file hierarchy
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>

class hkaiAstarEdgeFilter;

/// A utility for performing line of sight checks on a nav volume.
/// Optionally outputs a list of edges crossed.
class hkaiVolumeLineOfSightUtil
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiVolumeLineOfSightUtil);

	
			/// Input parameters for line of sight check.
		struct LineOfSightInput
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, LineOfSightInput );

				/// Start point of the path.
			hkVector4 m_startPoint;

				/// Goal point of the path.
			hkVector4 m_endPoint;

				/// Key of the starting cell. m_startPoint should be contained in this Cell - TODO: Optional assert/checks?
			hkaiPackedKey m_startCellKey;

				/// Key of the goal cell. m_endPoint should be contained in this Cell - TODO: Optional assert/checks?
			hkaiPackedKey m_endCellKey;

				/// The maximum number of cells iterated through when searching for line of sight.
				/// This defaults to a very large value.
			int m_maxNumberOfIterations;

				/// Width, height, filter information, etc. for a character.
				/// If an hkaiAstarCostModifier is specified, these can determine which edges are traversable,
				/// or change the cost of crossing certain Cells.
			hkaiAgentTraversalInfo m_agentInfo;

				/// TODO: Edges will be rejected if they do not intersect a sphere of this radius, centered on the start point.
				/// If this value is negative, the check will be skipped
		//	hkReal m_searchSphereRadius;

				/// TODO: Edges will be rejected if they do not intersect a capsule of this radius, whose axis extends from the start point to the end point.
				/// If this value is negative, the check will be skipped
		//	hkReal m_searchCapsuleRadius;

				/// Optional pointer to hkaiAstarEdgeFilter, which can be used to reject nav volume edges.
			const hkaiAstarEdgeFilter* m_edgeFilter;

				/// Constructor.
			LineOfSightInput();
		};

		/// Check whether or not a straight-line path exists between the start and end points.
		static bool HK_CALL checkLineOfSight( const hkaiStreamingCollection::InstanceInfo* streamingInfo, const LineOfSightInput& input,
			hkArray<hkaiPackedKey>* cellsArrayOut = HK_NULL, bool doNotExceedArrayCapacity = false,
			int* numIterationsOut = HK_NULL );

			
};

#endif // VOLUME_LINE_OF_SIGHT_UTIL_H

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
