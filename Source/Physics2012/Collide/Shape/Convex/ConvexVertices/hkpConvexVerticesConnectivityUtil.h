/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_CONVEX_VERTICES_CONNECTIVITY_UTIL_H
#define HK_CONVEX_VERTICES_CONNECTIVITY_UTIL_H

#include <Physics2012/Collide/Shape/Convex/hkpConvexShape.h>
#include <Physics2012/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesConnectivity.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>
#include <Common/Base/Types/Geometry/hkStridedVertices.h>

class hkpConvexVerticesShape;

/// This utility class is used to work out connectivity from a hkpConvexVerticesShape and store the results in
/// a hkpConvexVerticesConnectivity.
/// Since it may not be possible to find valid connectivity the methods will return if they successfully found
/// connectivity.
/// The connectivity is used to find the inertia tensor and to cut the shape using a plane.
class hkpConvexVerticesConnectivityUtil
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkpConvexVerticesConnectivityUtil);
            /// Will traverse the shape hierarchy and if any hkpConvexVerticesShape types are found it will set
            /// connectivity
        static hkResult HK_CALL ensureConnectivityAll(const hkpShape* shape);

            /// Checks if there is connectivity information. If not it will create a shape with connectivity.
            /// May return the input shape reference counted
        static hkResult HK_CALL ensureConnectivity(const hkpConvexVerticesShape* shape);

            /// Find the connectivity from the convex shape.
            /// Finding the connectivity may be a fairly slow process - as will need to reconstruct the convex
            /// hull.
        static hkpConvexVerticesConnectivity* HK_CALL findConnectivity(const hkpConvexVerticesShape* shape);

            /// Cuts the \a shape with the \a plane. The part of the shape that is on the positive side of the plane is kept.
            /// Returns HK_NULL if nothing was produced or if the volume of the produced shape is below \a minVolume.
            /// May return shape reference counted (and with connectivity set) if the plane does not cut the shape.
        static const hkpConvexVerticesShape* HK_CALL cut(const hkpConvexVerticesShape* shape, const hkVector4& plane, hkReal convexRadius, hkReal minVolume);

            /// Creates geometry from a hkpConvexVerticesShape and connectivity information
        static hkGeometry* HK_CALL createGeometry(const hkpConvexVerticesShape* shape,const hkpConvexVerticesConnectivity* conn);

			/// Creates geometry from a hkpConvexVerticesShape and connectivity information, and a transform for the vertices
		static hkGeometry* HK_CALL createGeometry(const hkpConvexVerticesShape* shape, const hkpConvexVerticesConnectivity* con, const hkTransform& transform);

            /// Quickly calculates the volume of the shape - shape requires the connectivity information
        static hkReal HK_CALL calculateVolume(const hkpConvexVerticesShape* shape,const hkpConvexVerticesConnectivity* conn);

            /// Quickly calculates the volume specified in the connectivity with the specified vertices
        static hkReal HK_CALL calculateVolume(const hkArrayBase<hkVector4>& vertices,const hkpConvexVerticesConnectivity* conn);

#if 0
        static void HK_CALL drawArrow(const hkVector4& start,const hkVector4& end,int lineCol,int headCol);

        static void HK_CALL drawAsLines(const hkTransform& trans,const hkpConvexVerticesShape* shape,const hkpConvexVerticesConnectivity* conn);
#endif

#if 0
        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_COLLIDE, hkpConvexVerticesConnectivityUtil );

        hkpConvexVerticesConnectivityUtil()
        :   m_vertexThreshold(0.01f),
            m_onPlaneThreshold(0.01f)
        {}

            /// Find the connectivity from the convex shape
        hkBool findConnectivity(const hkpConvexVerticesShape* shape,hkpConvexVerticesConnectivity& out) const;

            /// Find the connectivity for a set of planes and vertices
        hkBool setPlanesVertices(const hkArray<hkVector4>& planes,const hkArray<hkVector4>& vertices,hkpConvexVerticesConnectivity& out) const;

            /// Finds the vertex index which is connected on the outside of the face, from index.
            /// The actual indices that make up the face are passed in in indices.
            /// The normal to the face is passed in 'normal', the list of vertices, indexed via indices in vertices.
            /// To be clear index, and the return value are indices into the indices array which then indexes into vertices
        int findConnected(int index,const hkArray<hkVector4>& vertices,const hkArray<int>& indices,const hkVector4& normal) const;

            /// Find the amount of vertices on a plane.
            /// Uses the onPlaneThresold to determine what is 'on the plane'
        int findNumVerticesOnPlane(const hkVector4& plane,const hkArray<hkVector4>& vertices);

            /// Find the vertices on the plane,and store the indices in 'indicesOut'.
            /// Returns the amount of vertices found on the plane
        int findVerticesOnPlane(const hkVector4& plane,const hkArray<hkVector4>& vertices,hkArray<int>& indicesOut) const;

            /// Returns true if the point is on the plane
        HK_FORCE_INLINE hkBool _onPlane(const hkVector4& plane,const hkVector4& point) const
        {
            hkSimdReal dist = _getPlaneDistance(plane,point);
                /// Square it to make positive
            return dist*dist < m_onPlaneThreshold * m_onPlaneThreshold;
        }



    public:
            /// The minimum distance that vertices must be apart.
        hkReal m_vertexThreshold;

            /// For a point to be considered 'on a plane' the distance must be less than this value
        hkReal m_onPlaneThreshold;

#endif
    public:


        struct Edge
        {
            hkInt32 m_start;        // If -1 means not set
            hkInt32 m_end;
        };
            /// For each face we create a linked list of face edges
            /// we then remove all of the edges which there is the opposite pair - this should leave
            /// only the outside (presumably convex) edges that make up the face
        struct FaceEdge
        {
        	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkpConvexVerticesConnectivityUtil::FaceEdge);
            int m_startIndex;
            int m_endIndex;
            FaceEdge* m_next;
        };
        struct VertexInfo
        {
                /// Index to the new vertex
            hkUint32 m_newIndex;
                /// 1 if outside, 0 otherwise
            hkUint32 m_outside;
                /// The distance from the plane
            hkReal m_side;
        };


    private:
            // disable
        hkpConvexVerticesConnectivityUtil();
};

#endif // HK_CONVEX_VERTICES_CONNECTIVITY_UTIL_H

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
