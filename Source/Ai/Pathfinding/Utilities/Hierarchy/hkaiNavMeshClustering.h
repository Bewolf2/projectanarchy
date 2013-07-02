/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAV_MESH_CLUSTERING_H
#define HKAI_NAV_MESH_CLUSTERING_H

class hkaiNavMesh;
class hkaiAstarCostModifier;
class hkaiAstarEdgeFilter;
class hkaiDirectedGraphExplicitCost;

	/// Alternative nav mesh cluserting method. Attempts to merge clusters in order to maximize an evaluation function.
class hkaiNavMeshClustering
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshClustering);
	
			/// Weighting on different criteria for the evaluation function.
		struct Weights
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, Weights);
			hkReal m_interconnectivity;
			hkReal m_sizeDelta;
			hkReal m_homogeneity;
			hkReal m_numBoundaryEdges;
			hkReal m_heightDifference;

			hkBool32 operator==(const Weights& other) const;
			hkBool32 operator!=(const Weights& other) const;
		};

	private:

		typedef hkaiDirectedGraphExplicitCost ClusterGraph;

			/// Intermediate cluster information.
		struct Cluster
		{
			int m_size;
			hkVector4 m_centroid;
			hkReal m_area;
			int m_rep;
			int m_numBoundaryEdges;
			int m_intervisibility;
			hkReal m_minHeight;
			hkReal m_maxHeight;
			hkBool m_homogeneous;
			hkBool m_valid;
		};

			/// Information for potential merge between clusters
		struct Merge
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshClustering::Merge);

			bool operator==(const Merge& other) const;

			void init(int first, int second, int ic = 0);
			void setFirst(int first);
			void setSecond(int second);

			int m_first;
			int m_second;

			int m_interconnectivity;
			int m_score;

			Merge* prev;
			Merge* next;

			hkBool32 operator<(const Merge& other) const;
			static hkBool32 comparePointers(Merge* merge1, Merge* merge2);
			void sortClusters();
		};

			/// List of merges
		class MergeListBase
		{
		public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshClustering::MergeListBase);
			MergeListBase();

			bool isEmpty() const;
			Merge* begin();
			void remove(Merge* merge);
			void add(Merge* merge);
			void setHead(Merge* head);
			void setup(Merge* storage, int size);

		private:

			Merge* m_head;
		};

	public:
	
		hkaiNavMeshClustering(hkaiNavMesh& mesh, int blockSize, const Weights& weights,
							  hkVector4Parameter up, hkaiAstarCostModifier* costModifier = HK_NULL);

		void cluster();

		static void HK_CALL cluster(hkaiNavMesh& mesh, hkaiDirectedGraphExplicitCost& graphOut, 
			int numGroups, hkVector4Parameter up, const hkaiNavMeshClustering::Weights& weights, 
			hkaiAstarCostModifier* costModifier, hkaiAstarEdgeFilter* edgeFilter);

	private:

		void performMerge(const Merge& merge);
		void generateMerges();
		void regenerateMerges(const Merge& lastMerge);
		Merge* getBestMerge();
		void removeCluster(int cluster);
		void moveFaces(int from, int to);
		void computeMergeScore(Merge* merge);
		void compactSortedMerges(Merge* end = HK_NULL);
		bool isHomogeneous(const Merge& merge);
		void fillIntervisibilityArray();
		int mergedIntervisibility(const Merge& merge);
		void computeCentroids();
		void computeRepresentatives(hkArrayBase<int>& reps) const;
		void removeSingletons();

	private:

		hkaiNavMesh& m_mesh;
		int m_blockSize;
		hkaiAstarCostModifier* m_costModifier;
		Weights m_weights;

		hkArray<int>::Temp m_clusterFromNode;
		hkArray<Cluster>::Temp m_clusters;
		MergeListBase m_mergeList;
		hkArray<Merge>::Temp m_merges;

		hkVector4 m_up;
};

#endif // HKAI_NAV_MESH_CLUSTERING_H

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
