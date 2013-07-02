/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVIGATION_MESH_SIMPLIFICATION_UTILS_H
#define HKAI_NAVIGATION_MESH_SIMPLIFICATION_UTILS_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>

extern const class hkClass hkaiNavMeshSimplificationUtilsSettingsClass;

class hkaiNavMeshGenerationProgressCallback;
struct hkaiNavMeshGenerationSettings;
class hkaiEdgeGeometryRaycaster;
class hkBitField;

/// This utility class simplifies nav meshes.
class hkaiNavMeshSimplificationUtils
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshSimplificationUtils);

			/// Settings for the addition of extra vertices (also known as Steiner points) to the nav mesh in order to reduce long edges.	
			/// These generally improve the quality of the mesh, but increase the final data size by roughly m_vertexFraction.
		struct ExtraVertexSettings
		{
				//+version(3)
			HK_DECLARE_REFLECTION();
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, ExtraVertexSettings);

			ExtraVertexSettings();
			ExtraVertexSettings(hkFinishLoadedObjectFlag f)
				: m_userVertices(f)
			{}

				/// Copy constructor.
			ExtraVertexSettings( const ExtraVertexSettings& other );

				/// Turn off extra vertex creation
			void disable();

			enum VertexSelectionMethod
			{
					/// The number of vertices added will be proportional to the area (in meters^2) of the partition.
					/// This gives a more uniform distribruion of the extra vertices, but it's harder to preict the
					/// increase in data size.
				PROPORTIONAL_TO_AREA,

					/// The number of vertices added will be proportional to the number of vertices in the triangulation.
					/// In this case, m_vertexFraction is used.
				PROPORTIONAL_TO_VERTICES,
			};

				/// Determines how the number of extra vertices is chosen.
			hkEnum< VertexSelectionMethod, hkUint8 >	m_vertexSelectionMethod; //+default(hkaiNavMeshSimplificationUtils::ExtraVertexSettings::PROPORTIONAL_TO_AREA)

				/// Controls how many additional vertices will be inserted to the mesh when using PROPORTIONAL_TO_VERTICES.
				/// The number of points is m_vertexFraction * (number of triangles in Delaunay triangulation).
				/// To disable additional vertices, set this value to 0.
			hkReal m_vertexFraction; //+default(.025f)
									//+hk.RangeReal(absmin=0.0, absmax=1.0)

				/// Controls how many additional vertices will be inserted to the mesh when using PROPORTIONAL_TO_AREA.
				/// The number of points is m_areaFraction * (total are of triangles in Delaunay triangulation).
				/// To disable additional vertices, set this value to 0.
			hkReal m_areaFraction;	//+default(.000125f)
									//+hk.RangeReal(absmin=0.0, softmax=1.0)

				/// Additional vertices will only be considered for partitions greater than this area.
			hkReal m_minPartitionArea; //+default(1000.0f)
										//+hk.RangeReal(absmin=0.0, softmax=10000.0)

				/// The number of smoothing steps performed. Increasing this will increase the generation time.
				/// This doesn't have much effect on the final mesh and doesn't generally need to be tuned.
			int m_numSmoothingIterations; //+default(20)

				/// Controls how much the old point contributes at each iteration.
				/// This doesn't have much effect on the final mesh and doesn't generally need to be tuned.
			hkReal m_iterationDamping; //+default(.05f)

				/// Whether or not to automatically split long boundary edges.
			hkBool m_addVerticesOnBoundaryEdges; //+default(true)

				/// If m_addVerticesOnBoundaryEdges is enabled, boundary edges over this length will be split
				/// into smaller segments.
			hkReal m_boundaryEdgeSplitLength; //+default(50.0f)

				/// Tolerance for determining if a user vertex (m_userVertices) is on a boundary edge.
			hkReal m_userVertexOnBoundaryTolerance; //+default(1e-3f)

				/// Manually placed vertices. These will be added to the mesh before any random vertices.
			hkArray<hkVector4> m_userVertices;
		};

		/// This input structure holds the settings used to control the amount of simplification applied to a nav mesh
		struct Settings
		{
			//+version(12)
			HK_DECLARE_REFLECTION();
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, Settings);

			Settings();
			Settings(hkFinishLoadedObjectFlag f)
				: m_extraVertexSettings(f)
				{}

				/// Sets the simplification values to avoid almost all simplification.
				/// This is often used to avoid simplifying borders when setting up streaming.
			void setForBorderPreservation();

				/// This controls the amount of simplification applied to internal segments and convex boundary segments
			hkReal m_maxBorderSimplifyArea;				//+default(1.5f)
														//+hk.RangeReal(absmin=0.0, softmax=5.0)

				/// This controls the amount of simplification applied to concave boundary segments
			hkReal m_maxConcaveBorderSimplifyArea;		//+default(0.15f)
														//+hk.RangeReal(absmin=0.0, softmax=1.0)

				/// Corridors narrower than this value may be collapsed freely
			hkReal m_minCorridorWidth;					//+default(0.4f)
														//+hk.RangeReal(absmin=0.0, softmax=5.0)

				/// Corridors wider than this value may be collapsed down to this
			hkReal m_maxCorridorWidth;					//+default(0.6f)
														//+hk.RangeReal(absmin=0.0, softmax=10.0)

				/// Holes with area less than this are candidates for conversion to quads
			hkReal m_holeReplacementArea;				//+default(1.0f)
														//+hk.RangeReal(absmin=0.0, softmax=100.0)

				/// Holes that will be converted to quads will use an AABB instead of an OBB if the AABB area is not more than the OBB area multiplied by this fraction
			hkReal m_aabbReplacementAreaFraction;		//+default(1.2f)
														//+hk.RangeReal(absmin=1.0, softmax=5.0)

				/// The area of any partition loop will not be reduced by more than this fraction
			hkReal m_maxLoopShrinkFraction;				//+default(0.75f)
														//+hk.RangeReal(absmin=0.0, absmax=1.0)

				/// The maximum allowable border height error when simplifying segments
			hkReal m_maxBorderHeightError;				//+default(1.5f)
														//+hk.RangeReal(absmin=0.0)

				/// When simplifying internal segments and convex boundary segments, this controls the maximum
				/// projected distance by which the simplified segment can deviate from the unsimplified segment
			hkReal m_maxBorderDistanceError;			//+default(2.f)

				/// The maximum number of faces that can be included in a partition.
				/// Increasing this will result in fewer internal edges in large open areas, but
				/// may slow down generation times slightly.
			int m_maxPartitionSize;						//+default(40000)
														//+hk.RangeInt32(absmin=0, softmax=100000)

				/// Whether or not to partition by height
			hkBool m_useHeightPartitioning;				//+default(false)
				
				/// When partitioning by height, the max deviation allowed for a given partition
			hkReal m_maxPartitionHeightError;			//+default(100.f)
														//+hk.RangeReal(absmin=0.0, softmax=10000.0)

				/// If this is true (and m_useHeightPartitioning is true), a more conservative algorithm will be used to guarantee that the total vertical
				/// deviation between the simplified and unsimplified nav meshes does not exceed (m_maxPartitionHeightError + m_hertelMehlhornHeightError).
				/// This will generally result is less simplification however.
			hkBool m_useConservativeHeightPartitioning;	//+default(false)

				/// Additional height error allowed when merging faces into convex pieces. This is only used if m_useConservativeHeightPartitioning is true.
			hkReal m_hertelMehlhornHeightError;			//+default(0.01f)

				/// Faces will only be merged if the cosine of the angle between their normals is greater than this
			hkReal m_cosPlanarityThreshold;				//+default(0.99f)
														//+hk.RangeReal(absmin=-1.0, softmin=.9, absmax=1.0)

				/// Amount of non-convexity (in degrees) allowed when merging faces.
			hkReal m_nonconvexityThreshold;				//+default(0.1f)

				/// Connected sequences of boundary segments contained within a projected OBB of area less than this threshold will be removed if safe to do so
			hkReal m_boundaryEdgeFilterThreshold;		//+default(0.01f)

				/// The maximum allowable projected distance a vertex that isn't used by a boundary edge can be moved by when merging shared vertices
			hkReal m_maxSharedVertexHorizontalError;	//+default(1.f)

				/// The maximum allowable vertical distance a vertex that isn't used by a boundary edge can be moved by when merging shared vertices
			hkReal m_maxSharedVertexVerticalError;		//+default(1.f)

				/// The maximum allowable projected distance a vertex that is used by a boundary edge can be moved by when merging shared vertices
			hkReal m_maxBoundaryVertexHorizontalError;	//+default(1.f)

				/// The maximum allowable vertical distance a vertex that is used by a boundary edge can be moved by when merging shared vertices
			hkReal m_maxBoundaryVertexVerticalError;	//+default(1.f)

			hkBool m_mergeLongestEdgesFirst; //+default(true);

			ExtraVertexSettings m_extraVertexSettings;
		};

			/// Sequence of nav mesh edges that lie on the border between two partitions
		struct Segment
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshSimplificationUtils::Segment);
			Segment() {}
			Segment(const Segment& s) : m_materialSettingsIndex(s.m_materialSettingsIndex), m_partitionIdA(s.m_partitionIdA), m_partitionIdB(s.m_partitionIdB) { m_boundary = s.m_boundary; m_indices = s.m_indices; }

			int m_materialSettingsIndex;	// -1 if global settings should be used instead
			int m_partitionIdA;
			int m_partitionIdB;				// -1 if this is an external boundary run
			hkArray<hkaiNavMesh::EdgeIndex> m_boundary; 
			hkArray<int> m_indices;			// corresponding vertex indices forming polyline that defines the border (loops have equal first and last indices)
		};

			/// Sortable structure for partitions.
		struct SegmentIdxPid
		{
			HK_DECLARE_POD_TYPE();
			int m_segmentIndex;
			int m_partitionId;

			HK_FORCE_INLINE hkBool32 operator < (const SegmentIdxPid& other) const
			{
				return m_partitionId < other.m_partitionId;
			}
		};

		enum SimplificationResult
		{
			/// Simplification was stopped by the user
			SIMPLIFICATION_HALTED,

			/// Simplification failed (e.g. due to insufficient memory)
			SIMPLIFICATION_FAILED,

			/// Simplification succeeded
			SIMPLIFICATION_SUCCESS,
		};

		/// Merges shared vertices into a single vertex
		static hkResult HK_CALL mergeSharedVertices( hkaiNavMesh& mesh );

		/// Partitions the nav mesh into non overlapping (in 3d) partitions
		static hkResult HK_CALL partition( const hkaiNavMesh& mesh, const hkaiNavMesh& origMesh, const hkaiNavMeshGenerationSettings& settings, hkArray<int>& numFacesInPartition, hkArray<hkaiNavMesh::FaceIndex>& partitions, hkArray<int>& numEdgesInGroupInOut, hkArray<hkaiNavMesh::EdgeIndex>& edgesToRemoveInOut );

		static void HK_CALL partitionWC( const hkaiNavMesh& mesh, const hkaiNavMesh& origMesh, const hkaiNavMeshGenerationSettings& settings, hkArray<int>& numFacesInPartition, hkArray<int>& partitionDir, hkArray<hkaiNavMesh::FaceIndex>& partitions );

		/// Traces through each partition and produces a set of loops
		static hkResult HK_CALL tracePartitionBoundaries( const hkaiNavMesh& mesh, const hkArray<int>& numFacesInPartition, const hkArray<hkaiNavMesh::FaceIndex>& partitions, hkArray< Segment >& segments, const hkArray<hkaiNavMesh::EdgeIndex>& edgesToRemove );

		/// Converts the edge-based description of each segment into a list of vertex indices
		static hkResult HK_CALL setSegmentIndices( const hkaiNavMesh& navMesh, hkArray<Segment>& segmentsInOut, hkArray<hkVector4>& verticesOut, hkPointerMap<int, int>& vertexIndexMapOut );

		/// Fills in m_materialSettingsIndex members of Segment objects
		static hkResult HK_CALL setMaterialSettingsIndices( const hkaiNavMeshGenerationSettings& settings, const hkaiNavMesh& mesh, const hkArray<int>& numFacesInPartition, const hkArray<hkaiNavMesh::FaceIndex>& partitions, hkArray<Segment>& segmentsInOut );

		/// Simplifies an array of Segment objects by removing (and moving) vertices
		static hkResult HK_CALL simplifySegments( const hkaiNavMeshGenerationSettings& settings, hkArray<hkVector4>& verticesInOut, hkArray<Segment>& segmentsInOut, const hkaiNavMesh& navMesh, const hkBitField& unremovableVertices, const hkArray<int>* partitionDir = HK_NULL, hkaiNavMeshGenerationProgressCallback* callbacks = HK_NULL );

		// Converts all segments into triangles.
		static hkResult HK_CALL triangulatePartitions( const hkaiNavMeshGenerationSettings& settings, bool wallClimbing, const hkaiNavMesh& originalNavMesh, hkaiNavMesh& navMeshOut, const hkArray<Segment>& segments, const hkArrayBase<hkaiNavMesh::FaceIndex>& partitions, 	const hkArrayBase<int>& numFacesInPartition, const hkArrayBase<int>& partitionDir, hkArray<int>& faceToPartitionMap, const hkBitField& edgeIsBoundary, const hkArrayBase<hkUint32>& faceData );

		/// Converts a single partition (defined by one or more segments) into a list of triangles
		static hkResult HK_CALL triangulatePartition( const hkaiNavMeshSimplificationUtils::ExtraVertexSettings& extraVertSettings, const hkaiNavMesh& originalMesh, hkArray<hkVector4>& vertices, const hkArray<Segment>& segments, const hkArray<int>::Temp& partitionSegments, const hkArray<const hkaiNavMesh::FaceIndex>& facesInPartition, hkVector4Parameter up, hkArray<int>::Temp& triangleIndicesOut, hkArray<int>::Temp& sharedEdgesOut, const hkBitField& edgeIsBoundary, hkReferencedObject* globalTriangulatorPtr = HK_NULL, const hkAabb* globalDomainAabb = HK_NULL );

		/// Tries to merge coplanar triangles in the nav mesh into convex pieces
		static hkResult HK_CALL hertelMehlhorn( const hkaiNavMeshGenerationSettings& settings, hkaiNavMesh & mesh, const hkReal* heightErrorOverride = HK_NULL, hkArray<int>* faceToPartitionMapInOut = HK_NULL );

		/// Simplifies a nav mesh built using hkaiNavMeshGenerationUtils::generateNavMesh.
		/// Note that the input mesh is modified during simplification.
		static SimplificationResult HK_CALL simplifyNavMesh( const hkaiNavMeshGenerationSettings& settings, hkaiNavMesh& navMesh, hkaiNavMesh& navMeshOut, hkaiNavMeshGenerationProgressCallback* callback = HK_NULL );

		/// Simplifies a nav mesh built using hkaiNavMeshGenerationUtils::generateWallClimbingNavMesh.
		/// Note that the input mesh is modified during simplification.
		static SimplificationResult HK_CALL simplifyWallClimbingNavMesh( const hkaiNavMeshGenerationSettings& settings, hkaiNavMesh& navMesh, hkaiNavMesh& navMeshOut, hkaiNavMeshGenerationProgressCallback* callback = HK_NULL );

		/// Internal simplification function with a few advanced parameters
		static SimplificationResult HK_CALL _simplifyNavMesh( const hkaiNavMeshGenerationSettings& settings, hkaiNavMesh& navMesh, hkaiNavMesh& navMeshOut, hkaiEdgeGeometryRaycaster* raycaster, hkaiNavMeshGenerationProgressCallback* callback );

		/// Merges shared vertices into a single vertex, but only keeps merges which are "safe"
		static hkResult HK_CALL _selectiveMergeSharedVertices( const hkaiNavMeshGenerationSettings& settings, hkaiNavMesh& mesh );
	
		/// Find boundary edges which are "removable", e.g small parts of edges that weren't matched.
		static hkResult HK_CALL _findRemovableBoundaryEdges( const hkaiNavMeshGenerationSettings& settings, const hkaiNavMesh& mesh, const hkaiEdgeGeometryRaycaster* raycaster, hkArray<int>* numEdgesInGroupOut, hkArray<hkaiNavMesh::EdgeIndex>* edgesToRemoveOut, class hkBitField* removeableEdges );

		
		/// Internal class used to fit a plane to several points
		class PlaneFitter : public hkReferencedObject
		{
			public:
				HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH );
				PlaneFitter();
				void reset();
				void addPoint( hkVector4Parameter newPoint );
				void calcCovariance( hkMatrix3& covariance, hkSimdReal& invNumPointsOut ) const;
				hkResult calcPlane( hkVector4& planeOut ) const;

			private:
				hkVector4 m_sum;
				hkMatrix3 m_productSums;
				int		  m_numPoints;

		};


		static hkVector4 _classToDir( int i );

		static const hkReal s_triangulatorEpsilon;
};

#endif	// HKAI_NAVIGATION_MESH_SIMPLIFICATION_UTILS_H

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
