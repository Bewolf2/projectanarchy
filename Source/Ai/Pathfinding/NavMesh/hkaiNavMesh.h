/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVMESH_H
#define HKAI_NAVMESH_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingSet.h>

extern const class hkClass hkaiNavMeshEdgeClass;
extern const class hkClass hkaiNavMeshFaceClass;
extern const class hkClass hkaiNavMeshClass;

/// This contains the nav mesh data which is central to most of the pathfinding in Havok A.I.
/// 
/// The nav mesh represents navigable space as a set of connected convex faces.
class hkaiNavMesh : public hkReferencedObject
{
	public:
		//+vtable(true)
		//+version(14)

		friend class hkaiNavMeshInstance;

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);
		HK_DECLARE_REFLECTION();

		typedef hkInt32 FaceIndex;
		typedef hkInt32 EdgeIndex;
		typedef hkInt32 VertexIndex;
		typedef hkInt32 FaceData;
		typedef hkInt32 EdgeData;

			/// These flags are used when dynamic nav meshes are generated.
		enum FaceFlagBits
		{
				/// This original face is hidden i.e., it has been cut into smaller faces and is not currently accessible
			FACE_HIDDEN = 1,
				/// This is a face that has been cut from an original nav mesh face
			FACE_CUT = 2,
				/// Face that has edges which are split for streaming (so it should never be uncut)
			FACE_STREAMING = 4
		};

		typedef hkFlags<FaceFlagBits, hkUint8> FaceFlags;

			/// Flags to indicate the properies of the mesh when it was created.
		enum NavMeshFlagBits
		{
				/// No flags set
			MESH_NONE = 0,

				/// Whether or not this is a wall-climbing mesh
			MESH_CLIMBING = 1,
		};

		typedef hkFlags<NavMeshFlagBits, hkUint8> NavMeshFlags;


			/// Constant values
		enum Constants
		{
			INVALID_REGION_INDEX = -1,
			INVALID_FACE_INDEX	= -1,
			INVALID_EDGE_INDEX = -1,
			INVALID_VERTEX_INDEX = -1,

			MAX_DATA_PER_EDGE = 4,
			MAX_DATA_PER_FACE = 4,
		};


		/// A face represents a convex region of navigable space.
		/// It is an ordered list of edges. The edges are wound CCW
		struct Face
		{
			//+version(6)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiNavMesh::Face);
			HK_DECLARE_REFLECTION();
			HK_DECLARE_POD_TYPE();

			friend class hkaiNavMesh;
			friend class hkaiNavMeshInstance;

				/// The start edge index for this face
			EdgeIndex m_startEdgeIndex;

				/// The start index of user edges for this face
			EdgeIndex m_startUserEdgeIndex;

				/// The number of standard geometric edges
			hkInt16 m_numEdges;

				/// The number of user edges
			hkInt16 m_numUserEdges;

				/// Index of the cluster to which this face belongs. Used by hierarchical pathfinding.
			hkInt16 m_clusterIndex;

				/// Padding - the compiler would add this anyway
			hkUint16 m_padding;

		public:

			HK_FORCE_INLINE void init();

			int endEdgeIndex() const { return m_startEdgeIndex + m_numEdges; }
			int endUserEdgeIndex() const { return m_startUserEdgeIndex + m_numUserEdges; }

			// Used for iterating over all edges (normal + user)
			// Usage: 
			//		for ( hkaiNavMesh::EdgeIndex e=face.edgesBegin(); face.edgesHasNext(e); e = face.edgesNext(e) )
			//		{ ... }
			HK_FORCE_INLINE int edgesBegin() const;
			HK_FORCE_INLINE int edgesNext( int e ) const;
			HK_FORCE_INLINE hkBool32 edgesHasNext( int e ) const;
		};


		/// Bits set per edge
		enum EdgeFlagBits
		{
			/// Edge is blocked by a silhouette.
			EDGE_SILHOUETTE = 1,
			/// Edge is formed by re-triangulating an existing face.
			EDGE_RETRIANGULATED = 2,
			/// Edge lies on original cell boundary.
			EDGE_ORIGINAL = 4,
			/// Opposite edge isn't loaded yet (this isn't used anymore)
			OPPOSITE_EDGE_UNLOADED_UNUSED = 8,
			/// User edge: belongs to a face, but isn't part of the convex boundary for the face
			EDGE_USER = 16,
			/// Edge is not traversable during searches (mainly used for one-way user edge pairs).
			EDGE_BLOCKED = 32,
			/// The edge connects to another edge in another nav mesh
			EDGE_EXTERNAL_OPPOSITE = 64,
		};
		typedef hkFlags<EdgeFlagBits, hkUint8> EdgeFlags;

			/// A half edge on a convex face
		struct Edge
		{
			//+version(8)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiNavMesh::Edge);
			HK_DECLARE_REFLECTION();
			HK_DECLARE_POD_TYPE();

			friend class hkaiNavMesh;
			friend class hkaiNavMeshInstance;

				/// The starting vertex
			VertexIndex m_a;

				/// The end vertex
			VertexIndex m_b;

				/// The edge key of the other half of this edge (HKAI_INVALID_PACKED_KEY implies that there is no other half and that this is a boundary edge)
			hkaiPackedKey m_oppositeEdge;

				/// The face key of other half of this edge (HKAI_INVALID_PACKED_KEY implies that there is no other half and that this is a boundary edge)
			hkaiPackedKey m_oppositeFace;

				/// Flags
			EdgeFlags m_flags;

			hkUint8 m_paddingByte; //+nosave

				/// Cost of an user edge. Unused for normal edges.
			hkHalf m_userEdgeCost;

			Edge()
			{
				setInvalid();
			}

			HK_FORCE_INLINE void setInvalid();
			
			HK_FORCE_INLINE void setAsBoundary();

			HK_FORCE_INLINE void init( VertexIndex va, VertexIndex vb, hkaiPackedKey oppEdgeKey=HKAI_INVALID_PACKED_KEY, hkaiPackedKey oppFaceKey=HKAI_INVALID_PACKED_KEY);

			/// Get the face key of the opposite face (m_oppositeFace accessor)
			HK_FORCE_INLINE hkaiPackedKey getOppositeFaceKeyUnchecked() const;

			/// Get edge key of the opposite edge (m_oppositeEdge accessor)
			HK_FORCE_INLINE hkaiPackedKey getOppositeEdgeKeyUnchecked() const;
			
			/// Get the runtime index of the opposite edge's section
			HK_FORCE_INLINE hkaiRuntimeIndex getOppositeSectionRuntimeIndexUnchecked() const;

			/// Get edge index of the opposite edge
			HK_FORCE_INLINE hkaiNavMesh::EdgeIndex getOppositeEdgeIndex() const;
			
			/// Get face index of the opposite face
			HK_FORCE_INLINE hkaiNavMesh::FaceIndex getOppositeFaceIndex() const;

			/// Whether or not the edge is crossable during searches
			HK_FORCE_INLINE hkBool32 isTraversable() const;

			/// Whether or not the edge has an accessible opposite edge.
			/// This is usually equivalent to !isTraversable, but in some cases (one-way user edge),
			/// an edge may have an opposite but not be traversable.
			HK_FORCE_INLINE hkBool32 hasOpposite() const;

			/// Whether or not the edge is a user edge
			HK_FORCE_INLINE hkBool32 isUserEdge() const;
			
			/// Whether or not the edge connects to an edge in a different nav mesh
			HK_FORCE_INLINE hkBool32 isExternal() const;

			/// Returns the additional cost of traversing a user edge.
			/// For simple edges, this will return 0.0f
			HK_FORCE_INLINE hkSimdReal getUserEdgeCost() const;

			/// Sets the additional cost of traversing the user edge.
			/// The cost is stored as an hkHalf, so there is some loss of precision.
			/// This will assert if the edge isn't a user edge.
			HK_FORCE_INLINE void setUserEdgeCost( hkReal c );

		};

		// On SPU, references/pointers to edges, faces, etc. are dangerous since they might get evicted from the nav mesh cache before they're used.
		// The following typedefs will do the right thing on SPU (i.e. copy the data) and the fast thing on CPU (i.e. use a reference).
#ifdef HK_PLATFORM_SPU
		typedef const Face		FaceReference;
		typedef const Edge		EdgeReference;
#else
		typedef const Face&		FaceReference;
		typedef const Edge&		EdgeReference;
#endif

		hkaiNavMesh();

		hkaiNavMesh( hkFinishLoadedObjectFlag f );

		~hkaiNavMesh();

			/// Set this as a copy of other.
		hkResult copy( const hkaiNavMesh& other );

			/// Swap arrays with the other mesh
		void swap( hkaiNavMesh& other );

		/// Clear all data, reverting to default values.
		void clear();

		//
		// Accessor methods
		//
#ifndef HK_PLATFORM_SPU
		HK_FORCE_INLINE const Face& getFace(FaceIndex fIdx) const;
		HK_FORCE_INLINE       Face& getFace(FaceIndex fIdx);

		HK_FORCE_INLINE const Edge& getEdge(EdgeIndex eIdx) const;
		HK_FORCE_INLINE       Edge& getEdge(EdgeIndex eIdx);

		HK_FORCE_INLINE const hkVector4& getVertex(VertexIndex eIdx) const;
		HK_FORCE_INLINE       hkVector4& getVertex(VertexIndex eIdx);

		HK_FORCE_INLINE void getVertexPosition(VertexIndex v, hkVector4& vOut) const;
		HK_FORCE_INLINE void getLocalVertexPosition( int vIdx, hkVector4& vOut ) const;
		HK_FORCE_INLINE void getEdgePoints( int edgeIndex, hkVector4& ea, hkVector4& eb) const;
		HK_FORCE_INLINE void getEdgePoints( const Edge&, hkVector4& ea, hkVector4& eb) const;

		HK_FORCE_INLINE EdgeData getEdgeData( hkaiNavMesh::EdgeIndex eIdx) const;
		HK_FORCE_INLINE FaceData getFaceData( hkaiNavMesh::FaceIndex fIdx) const;
		HK_FORCE_INLINE void setEdgeData( hkaiNavMesh::EdgeIndex eIdx, EdgeData d);
		HK_FORCE_INLINE void setFaceData( hkaiNavMesh::FaceIndex fIdx, FaceData d);

		HK_FORCE_INLINE const EdgeData* getEdgeDataPtr( hkaiNavMesh::EdgeIndex eIdx) const;
		HK_FORCE_INLINE       EdgeData* getEdgeDataPtr( hkaiNavMesh::EdgeIndex eIdx);
		HK_FORCE_INLINE const FaceData* getFaceDataPtr( hkaiNavMesh::FaceIndex fIdx) const;
		HK_FORCE_INLINE       FaceData* getFaceDataPtr( hkaiNavMesh::FaceIndex fIdx);

		// Utility method to copy user data.
		template<typename DATA>
		static HK_FORCE_INLINE void copyUserData(DATA* dst, const DATA* src, int striding);

		template<typename DATA>
		static HK_FORCE_INLINE void copyUserData( hkArrayBase<DATA>& dataArray, int dstIndex, int srcIndex, int striding );

			/// Get the local-space vertices for the given edge index (same as getEdgePoints but keeps the same interface as hkaiNavMeshInstance)
		inline void getEdgeLocalPoints( hkaiNavMesh::EdgeIndex eIdx, hkVector4& edgeA, hkVector4& edgeB ) const;

			/// Get the local-space vertices for the given edge (same as getEdgePoints but keeps the same interface as hkaiNavMeshInstance)
		inline void getEdgeLocalPoints( const hkaiNavMesh::Edge& edge, hkVector4& edgeA, hkVector4& edgeB ) const;

		HK_FORCE_INLINE void setMaxGlobalClearance(hkReal value);
#endif


		//
		// Utility method for validating determinism
		//
		void checkDeterminism() const;

		//
		// Utility methods for EdgeIndex
		//
		inline int getNumEdges() const;
		inline int getNumFaces() const;
		inline int getNumVertices() const;

		inline const hkArray<Face>& getFaceArray() const;
		inline const hkArray<Edge>& getEdgeArray() const;
		inline const hkArray<hkVector4>& getVertexArray() const;

		inline const Face* getFacePtr() const;
		inline       Face* getFacePtr();

		inline const Edge* getEdgePtr() const;
		inline       Edge* getEdgePtr();

		inline const hkVector4* getVertexPtr() const;
		inline       hkVector4* getVertexPtr();

		//
		// Misc accessors
		//
		inline const hkAabb& getAabb() const;
		inline hkReal getErosionRadius() const;
		inline NavMeshFlags getFlags() const;
		inline NavMeshFlags& getFlags();
		bool isWallClimbing() const;

		//
		// Misc modifiers
		// 

		/// Add new vertices to the end of the vertex array.
		hkResult appendVertices( const hkVector4* v, int numVerts );

		/// Expand the vertex array, and return a pointer to the start of the new vertices.
		hkVector4* expandVerticesBy( int n );

		/// Expand the face array, and return a pointer to the start of the new faces.
		hkaiNavMesh::Face* expandFacesBy(int n, int estimatedMaxNumFace = -1);

		/// Add new edges and edgeData to the end of the edge array.
		/// The edgeData array must be m_edgeDataStriding*numEdges in size.
		hkResult appendEdges( const Edge* edges, EdgeData* edgeData, int numEdges );

		/// Expand the edge array, and return a pointer to the start of the new edges
		hkaiNavMesh::Edge* expandEdgesBy(int n, int estimatedMaxNumEdges = -1);

		hkResult setFacesSize( int n );
		hkResult setEdgesSize( int n );
		void removeEdgeAtAndCopy(int index );
		void removeFaceAtAndCopy(int index );

		/// Set the FaceData striding. If greater than the old value, new data will be set to zero.
		hkResult setFaceDataStriding( int newStride, FaceData fillValue = 0 );

		/// Set the EdgeData striding. If greater than the old value, new data will be set to zero.
		hkResult setEdgeDataStriding( int newStride, EdgeData fillValue = 0 );

		inline int getFaceDataStriding() const { return m_faceDataStriding; }
		inline int getEdgeDataStriding() const { return m_edgeDataStriding; }

		//
		// Members
		// 
			/// Nav mesh faces see hkaiNavMesh::Face
		hkArray<struct Face>		m_faces;
			
			/// Nav mesh edges see hkaiNavMesh::Edge
		hkArray<struct Edge>		m_edges;

			/// Verts
		hkArray<hkVector4>			m_vertices;

		hkArray<hkaiStreamingSet>	m_streamingSets;

		hkArray<FaceData>			m_faceData;
		
		hkArray<EdgeData>			m_edgeData;

			/// Number of FaceData per face
		int							m_faceDataStriding; //+default(1)

			/// Number of EdgeData per edge
		int							m_edgeDataStriding; //+default(1)

	private:
			/// Property flags of the mesh
		NavMeshFlags				m_flags; //+overridetype(hkUint8)
		
	public:
			/// AABB containing the entire mesh. This is computed automatically during nav
			/// mesh generation; it is also expanded in all directions by the character height.
		hkAabb						m_aabb;

			/// Amount by which the nav mesh has been shrunk.
			/// This is set during nav mesh generation if hkaiNavMeshGenerationSettings::m_characterWidthUsage
			/// is set to hkaiNavMeshGenerationSettings::SHRINK_NAV_MESH
		hkReal						m_erosionRadius;

			/// User-defined data - Havok never touches this.
		hkUlong						m_userData; //+default(0)


	protected:
		hkaiNavMesh( const hkaiNavMesh& other );
		void operator=(const hkaiNavMesh& other);
};

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.inl>

#endif //HKAI_NAVMESH_GEOMETRY_H

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
