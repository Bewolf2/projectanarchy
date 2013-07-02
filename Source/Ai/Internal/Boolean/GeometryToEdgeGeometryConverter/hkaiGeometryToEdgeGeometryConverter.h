/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_GEOMETRY_TO_EDGE_GEOMETRY_CONVERTER_H
#define HKAI_GEOMETRY_TO_EDGE_GEOMETRY_CONVERTER_H

#include <Ai/Internal/Boolean/hkaiEdgeGeometry.h>

// This class is copied from the hkdMeshToGeometryConverterImpl class.

/// The hkaiGeometryToEdgeGeometryConverter converts hkGeometry objects into hkaiEdgeGeometry. hkaiEdgeGeometry
/// is a representation which allows for a face to be concave and potentially contain holes. hkGeometry objects only
/// consist of Triangles - therefore to allow for a more compact and more rapidly processable hkaiEdgeGeometry it is
/// possible to have the conversion merge co-planar triangles together to make a larger face.
///
/// Merging (see _tryAddNeighbor) is enabled with the mergeCoplanarTriangles parameter to convertTriangleMeshToGeometry method. Determination
/// of co-planarity is controlled by the 'm_edgeAngularDistance' and 'm_planeEpsilon' properties of an instantiation of
/// hkaiGeometryToEdgeGeometryConverter. Note that merging is only performed on triangles that are connected via an edge -
/// i.e., two disjoint islands of locally connected coplanar triangles won't be merged into a single face, even if the islands are coplanar.
///
/// Also note that merging will only take place between triangles with the same value in the 'm_material' member.
class hkaiGeometryToEdgeGeometryConverter
{
public:
    HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiGeometryToEdgeGeometryConverter );

        /// Converts a graphics shape into a hkaiEdgeGeometry. Mesh userdata (normally representing materials) is respected in triangle merging
	hkResult convertGeometryToEdgeGeometry( const hkGeometry& mesh, hkaiEdgeGeometry::FaceFlags faceFlags, hkaiEdgeGeometry& geomOut);

		/// This is the min distance from an edge a vertex to merge needs to be before the plane distance is made 'angular'.
		/// The plane distance is made angular by dividing the plane distance by (edgeDistance / m_edgeAngularDistance).
		/// I.e., at twice the edgeAngularDistance, the plane distance will be halved.
		/// Setting to 0 will disable this test, and only the plane distance test will be performed.
		/// By default this is set to 0.
	hkReal m_edgeAngularDistance;
		/// This is the maximum allowed distance a point can be from a face plane to be merged.
	hkReal m_planeEpsilon;

		/// Whether or not faces with different data can be merged.
	hkBool m_allowMergingDifferentUserData; //+default(false)

		/// Ctor
	hkaiGeometryToEdgeGeometryConverter();

protected:
    typedef hkUint64 EdgeId;

		
		/// Triangle information for the conversion process
	struct Triangle
	{
		int m_userData;								///< User data - only tris that have same value can be merged
		int m_face;                                 ///< Index of the face this triangle belongs to. -1 if doesn't belong
		int m_vertexIndices[3];                     ///< The vertex indices
	};
    
		/// Used for sorting the triangles by area
    struct TriangleArea
    {
    	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiGeometryToEdgeGeometryConverter::TriangleArea);
        Triangle* m_triangle;
        hkReal m_area;
    };

	// Get the edge identifier
    HK_FORCE_INLINE static EdgeId getEdgeId(int a, int b) { return (EdgeId(a) << 32) | EdgeId(b); }
    HK_FORCE_INLINE static int getAFromEdgeId(EdgeId id) { return int(id >> 32); }
    HK_FORCE_INLINE static int getBFromEdgeId(EdgeId id) { return int(id & 0xffffffff); }
	void _addEdge(int a, int b, int triangle, hkPointerMap<EdgeId, int>& edgeToTriangleMap);

	void _calculatePlane(const Triangle& tri, hkVector4& planeOut);
	void _tryAddNeighbor(const Triangle& neigbourTri, int a, int b, const hkVector4& facePlane, const hkPointerMap<EdgeId, int>& edgeToTriangleMap);
        // Returns the neighbouring face. Returns 0 if its an outside edge
        // Returns -1 if it is an inner edge
    int _findNeighboringFace(int faceIndex, int a, int b, const hkPointerMap<EdgeId, int>& edgeToTriangleMap) const;
	static bool HK_CALL _hasEdge(const Triangle& tri, int a, int b);
    void _findCoplanarTriangleFaces(const hkPointerMap<EdgeId, int>& edgeToTriangleMap);

    HK_FORCE_INLINE static bool _orderTriangles(const TriangleArea& a, const TriangleArea& b) { return a.m_area > b.m_area; }

	hkArray<Triangle> m_triangles;

    hkArray<int> m_faceTriangleIndices;             ///< A list of all the triangles per face
    hkArray<int> m_numTrianglesPerFace;             ///< The number of triangles per face
    hkArray<hkVector4> m_vertices;                  ///< The output remapped vertices

    hkArray<TriangleArea> m_orderedTriangles;       ///< Triangles ordered by size (used for working out linear correctness)
};



#endif // HKAI_GEOMETRY_TO_EDGE_GEOMETRY_CONVERTER_H

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
