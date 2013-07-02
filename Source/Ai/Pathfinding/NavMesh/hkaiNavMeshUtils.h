/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVIGATION_MESH_UTILS_H
#define HKAI_NAVIGATION_MESH_UTILS_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>

class hkaiNavMeshQueryMediator;
class hkPseudoRandomGenerator;
class hkaiNavMeshInstance;
class hkaiStreamingCollection;
struct hkaiAgentTraversalInfo;
struct hkcdRay;

/// Miscellaneous utility functions for nav meshes
class hkaiNavMeshUtils
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkaiNavMeshUtils);

	typedef hkaiNavMesh::FaceIndex FaceIndex;
	typedef hkaiNavMesh::EdgeIndex EdgeIndex;

	/// An array of vertices for the face, in world space.
	/// The last vertex is a duplicate of the first one, thus numEdges = FaceVertices::getSize()-1
	typedef hkArrayBase<hkVector4> FaceVertices;

	/// Get the vertices of a nav mesh face in world space.
	/// For nav mesh instances, this transforms the vertices into world space
	static void HK_CALL getFaceVertices( const hkaiNavMeshAccessor& meshInstance, const hkaiNavMesh::Face& face, FaceVertices& faceVerticesOut );

	/// Get the vertices of a nav mesh face in local space.
	template<typename MeshType>
	static void HK_CALL getFaceLocalVertices( const MeshType& mesh, const hkaiNavMesh::Face& face, FaceVertices& faceVerticesOut );

	/// Computes the face plane from the face vertices. The plane points away from the face
	static void HK_CALL calcFacePlane( const FaceVertices& faceVertices, hkVector4& planeOut );

	/// Computes the face normal from the face vertices. The plane points away from the face
	static void HK_CALL calcFaceNormal( const FaceVertices& faceVertices, hkVector4& planeOut );

	/// Computes the plane perpendicular to both the edge and the face. The plane points away the centroid of the face.
	/// The edge index is given relative to the start index of the face.
	static void HK_CALL calcEdgePlane( const FaceVertices& faceVertices, int edgeIndex, hkVector4Parameter faceNormal, hkVector4& edgePlaneOut );

	/// Compute the bounding AABB for a face.
	static void HK_CALL calcFaceAabb(const FaceVertices& faceVertices, hkSimdRealParameter extraRadius, class hkAabb& aabbOut);


#ifdef HK_PLATFORM_SPU
	static void HK_CALL calcFaceNormal( const hkaiSpuNavMeshAccessor& mesh, FaceIndex faceIndex, hkVector4& normalOut );
#else
		/// Computes the normal of the face and returns the area
	template<typename MeshType>
	static hkSimdReal HK_CALL calcFaceNormal( const MeshType& mesh, FaceIndex faceIndex, hkVector4& normalOut );
#endif

		/// Computes the face plane for the given face. The plane points away from the face
	template<typename MeshType>
	static void HK_CALL calcFacePlane( const MeshType& mesh, FaceIndex faceIndex, hkVector4& planeOut );

		/// Computes a random point on the face
	template<typename MeshType>
	static void HK_CALL calcRandomPointOnFace( const MeshType& mesh, FaceIndex faceIndex, hkPseudoRandomGenerator& rand, hkVector4& pointOut);

		/// Computes the centroid of a face
	template<typename MeshType>
	static void HK_CALL calcFaceCentroid( const MeshType& mesh, FaceIndex faceIndex, hkVector4& centroid);

		/// Computes the plane perpendicular to both the edge and the face. The plane points away the centroid of the face
	template<typename MeshType>
	static void HK_CALL calcEdgePlane( const MeshType& mesh, FaceIndex faceIndex, EdgeIndex edgeIndex, hkVector4Parameter faceNormal, hkVector4& edgePlaneOut );

		/// Computes the edge midpoint.
	template<typename MeshType>
	static void HK_CALL calcEdgeMidpoint( const MeshType& mesh, EdgeIndex edgeIndex, hkVector4& midpointOut);
	
		/// Computes the vertex plane by averaging the incoming and outgoing edge normals. The plane points away from the centroid
	template<typename MeshType>
	static void HK_CALL calcVertexPlane( const MeshType& mesh, FaceIndex faceIndex, EdgeIndex edgeIndex, hkVector4Parameter normal, hkVector4& vertexPlaneOut );

		/// Check if a point lies on the given face. This actually tests if the point is bounded by the edge planes and so check if the point is in the infinite volume of the edges extruded along the face normal.
	static bool HK_CALL isPointOnFace( const hkaiNavMeshInstance& meshInstance, int faceIndex, hkVector4Parameter faceNormal, hkVector4Parameter point, hkSimdRealParameter tolerance);

		/// Check if a point is near constrained edges. Useful for positioning a character at a "safe" spot on a nav mesh.
	static bool HK_CALL isAwayFromBoundaryEdges(const hkaiStreamingCollection* collection, const hkaiNavMeshQueryMediator* mediator, hkVector4Parameter point, hkSimdRealParameter radius);
	
		/// Compute the bounding AABB for a face.
	template<typename MeshType>
	static void HK_CALL calcFaceAabb(const MeshType& mesh, hkaiNavMesh::FaceIndex faceIndex, hkSimdRealParameter extraRadius, class hkAabb& aabbOut);

		/// Compute the bounding AABB for the mesh.
	static void HK_CALL calcMeshAabb(const hkaiNavMesh& mesh, hkSimdRealParameter extraRadius, class hkAabb& aabbOut);

		/// Compute the point on the specified face that is closest to the specified position
	static void HK_CALL getClosestPointOnFace( const hkaiNavMesh& mesh, hkVector4Parameter position, hkaiNavMesh::FaceIndex faceIndex, hkVector4& closestPointOut );

		/// Compute the point on the specified face that is closest to the specified position
	static void HK_CALL getClosestPointOnFace( const hkaiNavMeshInstance& mesh, hkVector4Parameter position, hkaiNavMesh::FaceIndex faceIndex, hkVector4& closestPointOut );
		/// Compute the point on the specified face that is closest to the specified position. Calculations are done in local space.
	static void HK_CALL getClosestPointOnFaceLocal( const hkaiNavMeshInstance& mesh, hkVector4Parameter position, hkaiNavMesh::FaceIndex faceIndex, hkVector4& closestPointOut );
		/// Compute the point on the face loop that is closest to the specified position.
	static void HK_CALL getClosestPointOnFace( const FaceVertices& facePoints, hkVector4Parameter position, hkVector4& closestPointOut );


		/// Returns true if the ray specified by from,to hits the given face.
		/// \param hitFractionOut contains the fraction (between 0 and 1 where the intersection occurs)
	template <typename MeshType>
	static bool HK_CALL castRayFace( const MeshType& mesh, hkaiNavMesh::FaceIndex faceIndex, const hkcdRay& ray, hkSimdReal& hitFractionOut );

		/// Returns true if the line containing the given ray hits the given face
		/// \param hitFractionOut contains the fraction (with 0 and 1 referring to the start and end points of the ray)
	template <typename MeshType>
	static bool HK_CALL intersectLineFace( const MeshType& mesh, hkaiNavMesh::FaceIndex faceIndex, const hkcdRay& ray, hkSimdReal& hitFractionOut );

		/// Return the next edge index, wrapping around to the first edge if we are at the last
	static HK_FORCE_INLINE EdgeIndex HK_CALL getNextEdgeIndex    ( const hkaiNavMesh::Face& face, EdgeIndex edgeIndex );

		/// Return the previous edge index, wrapping around to the last edge if we are at the first
	static HK_FORCE_INLINE EdgeIndex HK_CALL getPreviousEdgeIndex( const hkaiNavMesh::Face& face, EdgeIndex edgeIndex );

		/// In Debug this validates a nav mesh is correct. It checks for face and edge degeneracy and edge consistency (ensure opposite edges are paired correctly).
		/// Call this method to validate any changes made to a nav mesh
	static void HK_CALL validate( const hkaiNavMesh& mesh, bool checkVerticesInsideAabb = true );
	static void HK_CALL validate( const hkaiNavMeshInstance& meshInstance, bool checkVerticesInsideAabb = true, bool checkClearanceCache = true );

		/// Create an array that maps edge indices to face indices
	static void HK_CALL createFaceIndexArray(const hkaiNavMesh& mesh, hkArray<int>::Temp& faceIndices);
	static void HK_CALL createOwnedFaceIndexArray(const hkaiNavMeshInstance& meshInstance, hkArray<int>::Temp& faceIndices);
	static void HK_CALL createInstancedFaceIndexArray(const hkaiNavMeshInstance& meshInstance, hkArray<int>::Temp& faceIndices);

		/// Removes a single face from a nav mesh. Other faces connected to the face being removed have their opposite edges invalidated
	static void HK_CALL removeFace( hkaiNavMesh& mesh, FaceIndex faceIndex, bool invalidateOppositeEdges = true );

		/// Removes a set of faces from a nav mesh. Other faces connected to faces being removed have their opposite edges invalidated if invalidateOppositeEdges is true.
	static hkResult HK_CALL removeFaces( hkaiNavMesh& mesh, hkArray<int>::Temp& facesToRemove, bool invalidateOppositeEdges = true );

	enum RemoveOwnedFacesMode
	{
		ALWAYS_COMPACT,
		EXIT_IF_NO_FACES_TO_REMOVE
	};

		/// Removes a set of owned faces from an hkaiNavMeshInstance. Other faces connected to faces being removed have their opposite edges invalidated.
		/// The owned edge array is also compacted, e.g. edges that aren't referenced by faces are removed.
	static void HK_CALL removeOwnedFaces( hkaiNavMeshInstance& meshInstance, hkaiStreamingCollection* collection, hkArray<int>::Temp& facesToRemove, RemoveOwnedFacesMode mode );

		/// Removes redundant vertices from the nav mesh
	static hkResult HK_CALL compactVertices( hkaiNavMesh& mesh, int edgeStartIdx, int vertexStartIdx );
	static void HK_CALL compactOwnedVertices( hkaiNavMeshInstance& meshInstance );

		/// Removes redundant edges from the nav mesh
	static hkResult HK_CALL compactEdges( hkaiNavMesh& mesh, int edgeStartIdx = 0, bool checkUserData = false  );

		/// Remove redundant instanced edges from the nav mesh instance
	static void HK_CALL compactInstancedFaces( hkaiNavMeshInstance& instance );
	static void HK_CALL compactInstancedEdges( hkaiNavMeshInstance& instance );

	static void HK_CALL compactOwnedEdges( hkaiNavMeshInstance& instance, hkaiStreamingCollection* collection );
	static void HK_CALL compactClearanceCache( hkaiNavMeshInstance& meshInstance ); 

		/// Compute the connected region for each face. Returns the number of regions, or -1 if any memory allocations failed
	template<typename MeshType>
	static int HK_CALL computeRegions( const MeshType& mesh, bool sortRegionsByArea, hkArrayBase<int>& regionsOut, hkArray<hkReal>* areasOut = HK_NULL);

		/// Computes a random point inside the AABB and projects it onto the nav mesh.
		/// The point is guaranteed to be at least radius away from the nearest edge.
	static hkResult HK_CALL getPointOnMeshInsideAabb( const hkaiStreamingCollection* collection, const hkaiNavMeshQueryMediator* mediator, const hkAabb& aabb, hkVector4Parameter m_up, hkPseudoRandomGenerator& rand, hkSimdRealParameter radius, hkVector4& out, int maxSamples=100 );

		/// Checks whether the specified edge intersects the AABB
	static bool HK_CALL isEdgeInsideAabb( const hkaiNavMeshInstance& meshInstance, const hkAabb& aabb, hkaiNavMesh::EdgeIndex eIdx);

		/// Checks whether the segment intersects the AABB
	static bool HK_CALL _isSegmentInsideAabb( hkVector4Parameter edgeA, hkVector4Parameter edgeB, const hkAabb& aabb );

		/// Get uncrossable edges for the given faces and its neighboring faces.
		/// If an edge filter is provided, it will be used to check traversability.
	static void HK_CALL getNearbyBoundaries(const hkaiStreamingCollection::InstanceInfo* sectionInfo, hkaiPackedKey currentFaceKey, const hkAabb& aabb, const hkaiAgentTraversalInfo& traversalInfo, const class hkaiAstarEdgeFilter* filter, bool doFloodFill, hkVector4Parameter position, hkArrayBase<hkVector4>& boundariesOut);
	
		/// getNearbyBoundaries using only the neighboring faces. This is fast but may miss some edges inside the AABB.
	static void HK_CALL _getNearbyBoundariesNeighbors(const hkaiStreamingCollection::InstanceInfo* sectionInfo, hkaiPackedKey currentFaceKey, const hkAabb& aabb, const hkaiAgentTraversalInfo& traversalInfo, const class hkaiAstarEdgeFilter* filter, hkVector4Parameter position, hkArrayBase<hkVector4>& boundariesOut);
	
		/// getNearbyBoundaries using a floodfill to get all boundaries in the sensor AABB.
	static void HK_CALL _getNearbyBoundariesFlood(const hkaiStreamingCollection::InstanceInfo* sectionInfo, hkaiPackedKey currentFaceKey, const hkAabb& aabb, const hkaiAgentTraversalInfo& traversalInfo, const class hkaiAstarEdgeFilter* filter, hkVector4Parameter position, hkArrayBase<hkVector4>& boundariesOut);

		/// Input for hkaiNavMeshUtils::getDistanceToClosestBoundary().
	struct ClosestBoundaryInput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, ClosestBoundaryInput);
		ClosestBoundaryInput();

		hkaiPackedKey m_startFaceKey;
		hkVector4 m_startPosition;
		hkSimdReal m_testRadius; //+default(HK_REAL_HIGH)
		hkaiAgentTraversalInfo m_traversalInfo;
		const hkaiAstarEdgeFilter* m_edgeFilter; //+default(HK_NULL)
	};
	
		/// Output for hkaiNavMeshUtils::getDistanceToClosestBoundary().
	struct ClosestBoundaryOutput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, ClosestBoundaryOutput);
		hkaiPackedKey m_edgeKey;
		hkSimdReal m_distanceSquared;
	};
	
		/// Similar to getNearbyBoundaries, but only determines the index of the closest edge to the start point. Also returns the distance squared.
		/// This function also can detect boundary edges up to a distance of 'testRadius' away from the start position
	static void HK_CALL getDistanceToClosestBoundary(const hkaiStreamingCollection* collection, const ClosestBoundaryInput& input, ClosestBoundaryOutput& closestBoundary);

		/// Determines how hkaiNavMeshUtils::transform recomputes the AABB
	enum AabbMode
	{
			/// Do a full recomputation of the AABB
		RECALCULATE_AABB,

			/// Transform the old AABB. This is faster but results in a larger AABB.
		TRANSFORM_OLD_AABB,
	};

	/// Transforms the vertices of the navmesh, and recomputes its AABB
	static void HK_CALL transform( const hkTransform& t, hkaiNavMesh& mesh, AabbMode mode = RECALCULATE_AABB);

		/// Simple pair of a face key and edge key.
	struct FaceEdgeKeyPair
	{
		hkaiPackedKey m_faceKey;
		hkaiPackedKey m_edgeKey;
	};

		/// Gets all the edges contained within the AABB
	static void HK_CALL getEdgesInsideAabb( const hkaiNavMeshInstance& meshInstance, const hkaiNavMeshQueryMediator* mediator, const hkAabb& aabb, hkArray<FaceEdgeKeyPair>::Temp& edgeHits);
	static void HK_CALL getEdgesInsideAabb( const hkaiStreamingCollection*, const hkaiNavMeshQueryMediator* mediator, const hkAabb& aabb, hkArray<FaceEdgeKeyPair>::Temp& edgeHits);
	static void HK_CALL _getEdgesInsideAabb( const hkaiStreamingCollection*, const hkAabb& aabb, const hkArrayBase<hkaiPackedKey>& faceHits, hkArray<FaceEdgeKeyPair>::Temp& edgeHits);

		/// Attempts to reposition the point within its face so that it avoids constrained edges with in a radius
	static void HK_CALL resolveEdgePenetrations( const hkaiStreamingCollection*, const hkaiNavMeshQueryMediator* mediator, hkVector4Parameter point, hkSimdRealParameter radius, hkVector4& pointOut);
	

	enum MediatorType
	{
			/// hkcdStaticAabbTree-based mediator. This is currently the only supported type.
		MEDIATOR_STATIC_TREE,
	};

		/// Creates a query mediator for the navmesh. Currently only hkaiStaticTreeNavMeshQueryMediator is supported.
	static hkaiNavMeshQueryMediator* HK_CALL setupQueryMediator( const hkaiNavMesh& mesh, MediatorType mediatorType=MEDIATOR_STATIC_TREE );

	static hkBool32 HK_CALL triangleAabbCheck(const hkAabb& aabb, hkVector4Parameter triA, 
		hkVector4Parameter triB, hkVector4Parameter triC);

	static hkBool32 HK_CALL faceAabbCheck(const hkAabb& aabb, const hkArrayBase<hkVector4>& vertices);

	static hkBool32 HK_CALL faceAabbCheck(const hkAabb& aabb, const hkaiNavMesh& mesh, hkaiNavMesh::FaceIndex faceIndex);

#ifndef HK_PLATFORM_SPU
	static void HK_CALL rotateFace(hkaiNavMesh& navMesh, hkaiNavMesh::FaceIndex faceIndex, hkaiNavMesh::EdgeIndex newFirstEdgeIndex);
	static void HK_CALL rotateOwnedFace( hkaiStreamingCollection* streamingCollection, hkaiPackedKey faceKey, hkaiPackedKey newFirstEdgeKey );
	static hkaiNavMesh::EdgeIndex HK_CALL findBestCenter( hkaiNavMesh& mesh, hkaiNavMesh::FaceIndex faceIndex, hkVector4Parameter upIn);
	static void HK_CALL setBestFaceCenters(hkaiNavMesh& mesh, hkVector4Parameter up);
#endif
};

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshUtils.inl>

#endif // HKAI_NAVIGATION_MESH_UTILS_H

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
