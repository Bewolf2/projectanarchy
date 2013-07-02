/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_HIERARCHY_UTILS_H
#define HKAI_HIERARCHY_UTILS_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>

class hkaiDirectedGraphExplicitCost;
class hkaiDirectedGraphInstance;
class hkPseudoRandomGenerator;
class hkaiNavMesh;
class hkaiNavMeshInstance;
struct hkaiNavMeshFaceGraph;
class hkaiAstarCostModifier;
class hkaiStreamingCollection;
class hkaiAstarEdgeFilter;


	/// Utilities for constructing clusters of graph nodes (e.g., nav mesh faces).
	/// These clusters can the be used as a good heuristic for A* searches.
class hkaiHierarchyUtils
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiHierarchyUtils);
	HK_DECLARE_REFLECTION();

	struct ClusterSettings
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, ClusterSettings);
		HK_DECLARE_REFLECTION();
		ClusterSettings();
		ClusterSettings( hkFinishLoadedObjectFlag f ) {}

			/// The desired number of faces per cluster.
			/// This is approximate and will vary with the actual nav mesh.
		int m_desiredFacesPerCluster; //+default(20)

			/// Whether or not every region is guaranteed to get at least one cluster.
			/// If this is set to false, regions with fewer faces than the group size will not receive a node in the 
			/// graph. This is a more efficient distribution of the nodes, but means that A* searchs between disconnected
			/// regions won't early out (and could flood fill).
		hkBool m_ensureClusterPerRegion; //+default(true)

			/// Cost modifier to be used during clustering. This affects the final costs for the graph edges.
		const hkaiAstarCostModifier*	m_costModifier;	//+default(HK_NULL)

			/// Edge filter to be used during clustering. This affects the final costs for the graph edges.
		const hkaiAstarEdgeFilter*		m_edgeFilter;	//+default(HK_NULL)
	};

		/// Creates a cluster of the input nav mesh using the specified ClusterSettings.
	static void HK_CALL clusterNavMesh( hkaiNavMesh& mesh, hkaiDirectedGraphExplicitCost& graphOut, const ClusterSettings& settings);

		/// Creates a cluster of the input nav mesh.
		/// Each cluster contains approximately blockSize faces per cluster.
		/// This interface is deprecrated and may be removed soon.
	static void HK_CALL clusterNavMesh( hkaiNavMesh& mesh, hkaiDirectedGraphExplicitCost& graphOut, int desiredNumGroups, bool ensureClusterPerRegion = true, const hkaiAstarCostModifier* costModifier = HK_NULL, const hkaiAstarEdgeFilter* edgeFilter = HK_NULL);
	

		/// Adds an edge to the corresponding directed graph instances, or increments its reference count
	static void HK_CALL addDirectedGraphEdgeForUserEdge( hkaiPackedKey startFaceKey, hkaiPackedKey userEdgeKey, hkaiStreamingCollection* collection );
	static void HK_CALL removeDirectedGraphEdgeForUserEdge( hkaiPackedKey startFaceKey, hkaiPackedKey userEdgeKey, hkaiStreamingCollection* collection );
	
		/// Removes all graph user edges connected to the section.
	static void HK_CALL removeDirectedUserEdgesToSection( hkaiStreamingCollection* collection, hkaiRuntimeIndex sectionIndex);

	static void HK_CALL compactOwnedEdges( hkaiDirectedGraphInstance& graph );

	enum Adjustment
	{
		GRAPH_ADD_EDGE = 1,
		GRAPH_REMOVE_EDGE = -1,
	};

	static void HK_CALL _adjustDirectedGraphEdge( hkaiPackedKey startFaceKey, hkaiPackedKey userEdgeKey, hkaiStreamingCollection* collection, Adjustment adj );

	//
	// All other structs and methods are internal to the cluster builder.
	//

		/// Sortable pair of representative and group.
	struct RepGroupPair
	{
		int m_representative;
		int m_group;

		HK_FORCE_INLINE hkBool32 operator<( const RepGroupPair& other ) const
		{
			return m_representative < other.m_representative;
		}
	};

		/// A 2d array which is sparse in one direction and direct in the other.
	struct SemiSparse2dArraySorted
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, SemiSparse2dArraySorted );
		int getSize() { return m_n; }

			/// Cost of travelling to edge t.
		struct Cost
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, Cost );
			int t;
			hkReal r;
		};

		typedef hkArray<Cost> Array;

		int indexOf(const Array& a, int y, int& greaterThanIndex) const; // returns index of the element with t == y
		int indexOf_binarySearch(const Array& a, int y, int& greaterThanIndex) const; // returns index of the element with t == y
		int indexOfGreaterThan(const Array& a, int y) const; // returns index of the first element with t > y
		void setSize(int n, hkReal r);
		hkReal get(int x, int y) const;
		void set(int x, int y, hkReal r);

		void findMinValFromSet( int x, const hkArrayBase<struct RepGroupPair>& groupReps, int& minIndex, hkReal& minCost ) const;
		
		void validate();
		void validateSub(int i);

		hkReal m_default;
		hkArray< Array > m_costs;

		int m_n;
	};

	typedef SemiSparse2dArraySorted SemiSparse2dArray;

		/// A simple structure for caching costs between nodes in a graph.
		/// This is used during graph clustering to get the costs between several nearest neighbors for a graph node.
	struct CostAdaptor
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, CostAdaptor );
		typedef hkReal PathCost;
		typedef int SearchIndex;

		CostAdaptor(SemiSparse2dArray& costs, int sourceNid);

		PathCost getCost( SearchIndex nid ) const;
		void setCost( SearchIndex nid, PathCost d );
		bool isCostTooHigh( PathCost c) { return false; }
		hkBool32 estimatedCostLess( SearchIndex a, SearchIndex b ) const;

		PathCost estimatedCost( SearchIndex a ) const;
		PathCost estimatedCostCurrent( SearchIndex a ) const;

		SemiSparse2dArray& m_costs;
		int m_sourceNid;
	};
	
	static void HK_CALL buildAbstractGraphFromClusterInfo( hkaiNavMeshFaceGraph& graph, hkaiDirectedGraphExplicitCost& graphOut, hkArray<int>&, hkArray<int>&, const hkArray<hkVector4>&, const hkaiAstarCostModifier*, const hkaiAstarEdgeFilter* edgeFilter);
};

#endif // HKAI_HIERARCHY_UTILS_H

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
