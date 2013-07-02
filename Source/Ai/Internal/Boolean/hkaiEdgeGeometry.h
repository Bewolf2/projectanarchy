/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_EDGE_GEOMETRY_H
#define HKAI_EDGE_GEOMETRY_H

#include <Common/Base/Types/Color/hkColor.h>

extern const class hkClass hkaiEdgeGeometryFaceClass;

extern const class hkClass hkaiEdgeGeometryEdgeClass;

extern const class hkClass hkaiEdgeGeometryClass;

class hkAabb;
struct hkGeometry;
class hkaiEdgeGeometryValidator;

/// hkaiEdgeGeometry represents the topology of geometry as an unordered list of edges. The representation allows
/// for a face to be convex, concave, have holes or be collections of disjoint islands of edges. An edge refers to a
/// start vertex (m_a) and an end vertex (m_b), and stores the index of the face it belongs to (m_face). It is also
/// possible to associate data with each edge via the m_data member.
///
/// The representation is the geometric type that the hkaiBooleanOperation processes. It is ideal for this processing as
/// the boolean can deal with arbitrary face descriptions, and outputs the resulting edges in arbitrary order.
///
/// The face 0 is special and should have no edges associated with it, and 0 in its m_data in the faces. The method
/// 'clear' which empties the contents of the hkaiEdgeGeometry, will set up the geometry with just face 0. Face 0
/// is taken to mean 'unconnected'. Thus in algorithms which determine edge connectivity, the face index 0 can be used to
/// specify 'unconnected', which can improve the performance of some algorithms significantly.
///
/// For processing hkaiEdgeGeometry, there are a set of classes that can be used to find a variety of information about,
/// faces, edges and vertices
///
/// hkaiFaceEdges - determines all the edges associated with each face.
/// hkaiVertexEdges - determines all of the edges leaving each vertex.
/// hkaiFaceVertices - determines all of the vertices indexed by each face
///
/// All of these classes determine the data for all of geometry, as doing so makes the cost of the determination for each
/// primitive low. If the information is only needed for a single primitive, it may be more efficient to do a linear search
/// or use some other representation.
///
/// For hkaiEdgeGeometry to be valid it must fulfill a variety of constraints. For example each vertex indexed in a face
/// must be a start and an end vertex the same amount of times. The method 'checkConsistency' can determine if
/// an hkaiEdgeGeometry is consistent.
///
/// The m_faces array holds per face information. As it stands this is only per face user data. For a hkaiEdgeGeometry
/// to be valid every face indexed in an edge must exist in the m_faces array, even if it only holds zero in the m_userData.
class hkaiEdgeGeometry : public hkReferencedObject
{
	public:

		//+vtable(True)
        HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		HK_DECLARE_REFLECTION();

        typedef hkUint32 EdgeIndex;
        typedef hkUint32 FaceIndex;
        typedef hkUint32 VertexIndex;

        typedef hkUint32 FaceData;
        typedef hkUint32 EdgeData;

			/// A half edge.
			/// Should be kept as a power of 2
        struct Edge
        {
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiEdgeGeometry::Edge);
			HK_DECLARE_REFLECTION();
			HK_DECLARE_POD_TYPE();

				/// ==
			bool operator==(const Edge& rhs) const
			{
				return m_a == rhs.m_a &&
					   m_b == rhs.m_b &&
                       m_face == rhs.m_face;
			}
				/// !=
			bool operator!=(const Edge& rhs) const { return !(*this == rhs); }

			inline VertexIndex getMinVertexIndex() const {return hkMath::min2(m_a, m_b); }
			inline VertexIndex getMaxVertexIndex() const {return hkMath::max2(m_a, m_b); }

            VertexIndex m_a;                ///< Starting vertex index.
            VertexIndex m_b;                ///< Ending vertex index.
            FaceIndex m_face;               ///< The face this edge belongs to.
            EdgeData m_data;                ///< User data associate with the edge.
        };

		enum FaceFlagBits
		{
			FLAGS_NONE,

				/// Face came from input geometry
			FLAGS_INPUT_GEOMETRY = 1,

				/// Face came from a painter volume
			FLAGS_PAINTER = 2,

				/// Face came from a carver volume
			FLAGS_CARVER = 4,

				/// Face was added by extrusion
			FLAGS_EXTRUDED = 8,

				/// All flags
			FLAGS_ALL = (FLAGS_INPUT_GEOMETRY | FLAGS_PAINTER | FLAGS_CARVER | FLAGS_EXTRUDED),
		};

		typedef hkFlags<FaceFlagBits, hkUint8> FaceFlags;

			/// Data associated with a face.
        struct Face
        {
			// +version(1)
            HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiEdgeGeometry::Face);
			HK_DECLARE_REFLECTION();
			HK_DECLARE_POD_TYPE();
			Face() { m_flags = FLAGS_NONE; }
			Face( hkFinishLoadedObjectFlag f) { }

			FaceData m_data;                ///< User data associated with the face
			FaceIndex m_faceIndex;			///< Stores the face this face was originally associated with (the initial indexing is set by calling setFaceIndexes)
			FaceFlags m_flags;				///< Indicates where the face came from.
        };

			/// Extract the specified face into geomOut.
		void extractFace(const Edge*const* start, const Edge*const* end, hkaiEdgeGeometry& geomOut) const;
			/// Extract the specified face into geomOut.
		void extractFace(int faceIndex, hkaiEdgeGeometry& geomOut) const;

			/// Set geometry. All faces will use the specified FaceFlags.
			/// If setFaceData, Face::m_faceIndex will be set to the triangleIndex+1, and Face::m_data will be triangle material
			/// Returns HK_SUCCESS if all memory allocations succeeded, HK_FAILURE otherwise.
		hkResult set(const hkGeometry& geom, FaceFlags newFaceFlags, bool setFaceData);

			/// Set the contents to be the same as the passed in geometry.
			/// Returns HK_SUCCESS if all memory allocations succeeded, HK_FAILURE otherwise.
		hkResult set(const hkaiEdgeGeometry& geom);

			/// Set this up to contain exactly one triangle.
			/// The vertices are not initialized
		hkResult setForSingleTriangle();

            /// Remove a face.
        void removeFace(int face);
			/// Remove a set of faces.
		hkResult removeFaces(const FaceIndex* faceIndices, int numFaces);

			/// Remove all of the edges of the specified faces (the faces remain - just with zero edges).
		void removeFacesEdges(const FaceIndex* faceIndices, int numRemoveFaces);

			/// Concatenate geom into this geometry.
			/// Returns HK_SUCCESS if all memory allocations succeeded, HK_FAILURE otherwise.
		hkResult concat(const hkaiEdgeGeometry& geom);

            /// Calculate the normal for a face - (defined with a list of edges).
        void calculateFaceNormal(const Edge*const* start, const Edge*const* end, hkVector4& normalOut) const;

			/// Calculate all of the face normals
		void calculateFaceNormals(const class hkaiFaceEdges& faceEdges, hkArray<hkVector4>& normals) const;

			/// Calculate the normal for a face - (using its index).
		void calculateFaceNormal(const class hkaiFaceEdges& faceEdges, int i, hkVector4& normalOut) const;

			/// Calculate the centroid for a face - (defined with a list of edges).
		void calculateFaceCentroid(const Edge*const* start, const Edge*const* end, hkVector4& centroidOut) const;

			/// Calculate face area.
		hkSimdReal calculateFaceArea(const Edge*const* start, const Edge*const* end) const;

			/// Calculate the AABB surrounding the face.
		void calculateFaceAabb(const Edge*const* start, const Edge*const* end, hkAabb& aabb) const;

            /// Remove everything. A cleared hkaiEdgeGeometry, consists of a single face, face 0, the so called 'outside face'.
        void clear();

			/// Swap the contents of two hkaiEdgeGeometry.
		void swap(hkaiEdgeGeometry& geom);

            /// Checks consistency of a hkaiEdgeGeometry - returns HK_SUCCESS if it is consistent, otherwise HK_FAILURE.
		hkResult checkConsistency() const;

			/// Checks consistency of a hkaiEdgeGeometry, allowing reuse of memory for internal data.
			/// Returns HK_SUCCESS if it is consistent, otherwise HK_FAILURE.
		hkResult checkConsistency( hkaiEdgeGeometryValidator& checker ) const;

            /// Returns the index of the first edge found that has the start and end vertex indices specified. If no edge is found will return -1.
            /// Searches for the edge via a linear search - using a hkaiVertexEdges may be more suitable if looking for lots of edge pairs on a static piece of geometry.
		int findEdgeIndex(int startVertexIndex, int endVertexIndex) const;

			/// Returns the indices of the loop starting on start. NOTE its common for faces have multiple loops - this only extracts _any_ loop
			/// that contains 'start'. Use the hkaiLoopFinder, to find the unique loops making up a face.
		static void HK_CALL findLoopIndices(const hkaiEdgeGeometry::Edge*const* start, const hkaiEdgeGeometry::Edge*const* end, hkArray<int>& indicesOut);

			/// Ctor
		hkaiEdgeGeometry();

            /// Ctor (for loading)
		hkaiEdgeGeometry(hkFinishLoadedObjectFlag f) : hkReferencedObject(f), m_edges(f), m_faces(f), m_vertices(f), m_zeroFace(f) {}

	protected:

	public:

        hkArray<struct Edge> m_edges;              ///< All of the edges (they are not in any order)
        hkArray<struct Face> m_faces;              ///< The faces (The Face only holds per face user data)
        hkArray<hkVector4> m_vertices;             ///< All of the vertices
        struct Face m_zeroFace;                    ///< Used when cleared or initialized
};

#endif

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
