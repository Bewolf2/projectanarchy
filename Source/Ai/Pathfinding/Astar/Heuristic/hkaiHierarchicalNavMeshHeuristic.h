/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_ASTAR_HIERARCHICAL_HEURISTIC_H
#define HKAI_ASTAR_HIERARCHICAL_HEURISTIC_H

#include <Ai/Pathfinding/Astar/hkaiAstar.h>
#include <Ai/Pathfinding/Astar/hkaiAstarParameters.h>
#include <Ai/Pathfinding/Astar/Search/DirectedGraphSearch/hkaiDirectedGraphSearch.h>

#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.h>
#include <Ai/Pathfinding/Graph/hkaiNavMeshSectionGraph.h>
#include <Ai/Pathfinding/NavMesh/hkaiGeneralAccessor.h>

// If this is defined, some extra distance information will be used in the heuristic
// This overestimates the cost slightly, but can greatly reduce the number of iterations taken
#define HKAI_USE_EXTRA_DISTANCE_INFORMATION

// The abstract graph (CoarseGraph) is constructed on the faces of the nav mesh.
// But the original graph (OriginalGraph) is on the edges. Be careful!

/// An improved heuristic for use in A* searches.
/// During an A* search, an estimated cost from the current search node to the goal must be provided.
/// Typically the Euclidean distance between the node and goal is used, but for complex graphs, this can be a large
/// underestimate, which in turn results in nodes being search that are eventually discarded.
///
/// Instead of using the Euclidean distance, we can build a simplified representation of the original graph,
/// do pathfinding on the simplified graph (which will be very fast), and use the final distance for the simplified path
/// as an estimate for distance in the original graph. Since this estimate is a more accurate representation of the actual cost,
/// A* usually reaches its goal in fewer iterations - typically less than half the number of iterations are needed.
///
/// NOTE - The hierarchical heuristic can sometimes produce paths that are slightly longer than the Euclidean heuristic.
/// In practice, this difference is usually less than 5% of the optimal path. The reduced runtime cost is generally an acceptable tradeoff
/// for the slightly longer paths.
///
/// If no hierarchy information is provided, the Euclidean heuristic is used.
struct hkaiHierarchicalNavMeshHeuristic
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiHierarchicalNavMeshHeuristic);
		typedef int SearchIndex;
		typedef hkReal PathCost;
		typedef struct hkaiNavMeshSectionGraph OriginalGraph;
		typedef hkaiDirectedGraphVisitor CoarseGraph;

		typedef hkaiPackedKey FaceKey;
		typedef hkaiPackedKey EdgeKey;
		typedef hkaiPackedKey ClusterKey;
		typedef int ClusterIndex;


		enum { HEURISTIC_IS_INVARIANT = 1};

		enum { MAX_GOALS = 16 };

		inline hkaiHierarchicalNavMeshHeuristic(char* openSetStorage, int openSetStorageCount, char* searchStorage, int searchStorageCount);

		~hkaiHierarchicalNavMeshHeuristic() {}

			/// Initialize with hierarchy information
		void init( OriginalGraph* originalGraph, CoarseGraph* clusterGraph, FaceKey originalStartFaceKey, hkVector4Parameter startPosition, FaceKey originalgoalFaceKey, hkVector4Parameter goalPosition );

			/// Initialize with hierarchy information and multiple goals
		void init( OriginalGraph* originalGraph, CoarseGraph* clusterGraph, FaceKey originalStartFaceKey, hkVector4Parameter startPosition, const FaceKey* originalGoalFaceKeys, const hkVector4* goalPositions, int numGoals );

			/// Get the approximate cost from an edge to the goal.
		hkReal getHeuristic(EdgeKey edgeId) const;


		//
		// These are defined separately for SPU and CPU implementations
		//
		inline const hkaiNavMesh::Edge& getEdge(hkaiPackedKey edgeId) const;
		inline ClusterKey getClusterForFace(FaceKey faceKey) const;
		inline ClusterKey getClusterForFace(const hkaiNavMesh::Face& face, hkaiRuntimeIndex sectionId) const;


		inline void getEdgePosition(EdgeKey edgeId, hkVector4& posOut) const;
		void getClusterPosition(ClusterKey idx, hkVector4& posOut) const;

	protected:
		/// Returns the Euclidean distance to the closest goal
		inline hkSimdReal getMinDistanceToGoals( hkVector4Parameter edgePosition ) const;

		// sets goalIndexOut to the index of the goal cluster that the face is adjacent to, or -1 if not adjacent
		bool isFaceChunkAdjacentToGoalChunk(CoarseGraph::NodeIndex faceIndex, int& goalClusterOut, CoarseGraph::EdgeCost& costOut) const;

		hkSimdReal adjacentDistance(hkVector4Parameter startPosition, hkVector4Parameter startCenter,
			hkVector4Parameter endPosition, hkVector4Parameter endCenter, CoarseGraph::EdgeCost edgeCost) const;

	public:
		HK_PAD_ON_SPU(OriginalGraph*) m_originalGraph;
		HK_PAD_ON_SPU( CoarseGraph*) m_clusterGraph;
	
		HK_PAD_ON_SPU(const hkaiStreamingCollection::InstanceInfo*) m_streamingInfo;
		
		HK_PAD_ON_SPU(int) m_numGoals;
		HK_PAD_ON_SPU(bool) m_someGoalCluster;
		HK_PAD_ON_SPU(ClusterKey) m_originalStartCluster;
		
		ClusterKey		m_endClusterKeys[MAX_GOALS];
		hkVector4		m_endClusterPositions[MAX_GOALS];
		
		hkVector4		m_startPosition;
		HK_PAD_ON_SPU(FaceKey) m_startFaceKey;
		
		mutable hkaiDirectedGraphSearch m_coarseSearch;
			
		hkaiGeneralAccessor m_accessor;

		//
		// Goal information
		//

		HK_PAD_ON_SPU(const FaceKey*) m_goalFaceKeys;
		HK_PAD_ON_SPU(const hkVector4*) m_goalPositions;
};

#include <Ai/Pathfinding/Astar/Heuristic/hkaiHierarchicalNavMeshHeuristic.inl>
#endif // HKAI_ASTAR_HIERARCHICAL_HEURISTIC_H

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
