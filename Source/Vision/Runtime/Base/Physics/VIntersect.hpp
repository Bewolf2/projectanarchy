/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VIntersect.hpp

#ifndef _VISION_VINTERSECT_HPP_
#define _VISION_VINTERSECT_HPP_

// ---------------------------------------------------------------------------------
// Function: IntersectSegmentPolygon
// Purpose:  Test if the given segment intersects the given polygon
// Params:
// poly       [in]  array of vertices of the polygon. They are oredered in the
//                  counterclockwise direction when looking against the polygon
//                  normal.
// polySize   [in]  number of polygon vertices
// p1         [in]  first end point of the segment
// p2         [in]  second end point of the segment
// touchPoint [out] intersection point (valid only if return value is TRUE).
// Return:  TRUE if the intersection exists, FALSE otherwise
// ---------------------------------------------------------------------------------

VBASE_IMPEXP VBool IntersectSegmentPolygon( const hkvVec3* poly, int polySize, const hkvVec3& p1, const hkvVec3& p2, hkvVec3* touchPoint = 0 );

// ---------------------------------------------------------------------------------
// Function: IntersectRayPolygon
// Purpose:  Test if the given ray intersects the polygon
// Params:
// poly       [in]  array of vertices of the polygon. They are oredered in the
//                  counterclockwise direction when looking against the polygon
//                  normal.
// polySize   [in]  number of polygon vertices
// start      [in]  ray start point
// dir        [in]  ray direction
// touchPoint [out] intersection point (valid only if return value is TRUE).
// Return:  TRUE if the intersection exists, FALSE otherwise
// ---------------------------------------------------------------------------------

VBASE_IMPEXP VBool IntersectRayPolygon( const hkvVec3* poly, int polySize, const hkvVec3& start, const hkvVec3& dir, hkvVec3* touchPoint = 0 );

// ---------------------------------------------------------------------------------
// Function: PointInPolygon
// Purpose:  Determine if the given point lays in the polygon interior
// Params:
// poly      [in]  array of vertices of the polygon. They are oredered in the
//                 counterclockwise direction when looking against the polygon
//                 normal.
// polySize  [in]  number of polygon vertices. It must be at least 3.
// testPoint [in]  point to be tested
// Precons:  the testing point must lay in the polygon plane. The otherwise possibility
//           is not checked.
// Return:   TRUE if the test point is in the polygon interior, FALSE otherwise
// ---------------------------------------------------------------------------------

VBASE_IMPEXP VBool PointInPolygon( const hkvVec3* poly, int polySize, const hkvVec3& testPoint );

// ---------------------------------------------------------------------------------
// Function: PolygonNormal
// Purpose:  Calculate the normal of a polygon.
// Params:
// poly      [in]  array of vertices of the polygon. They are oredered in the
//                 counterclockwise direction when looking towards the polygon
//                 normal.
// polySize  [in]  number of polygon vertices. It must be at least 3.
// normal    [out] polygon normal
// Return:   TRUE if the normal could be calculated, FALSE otherwise (polygon
//           vertices are colinear)
// ---------------------------------------------------------------------------------

VBASE_IMPEXP VBool PolygonNormal( const hkvVec3* poly, int polySize, hkvVec3& normal );

// \brief
//   calculate distance of point to a segment
// 
// this function calculates the distance of a given point to the given segment. If requested then
// it returns also the point on the segment which has minimum distance from the input point.
// 
// \param pt
//   [in] test point which distance to the segment is calculated
// 
// \param segStart
//   [in] start point of the segment
// 
// \param segEnd
//   [in] end point of the segment
// 
// \param pClosestPoint
//   [out] if nonzero then after the function finishes it contains the point on the segment which
//   has minimum distance to the input point (pt).
// 
// \return
//   distance of the point from the segment 
VBASE_IMPEXP float GetPointSegmentDistance( const hkvVec3& pt, const hkvVec3& segStart, const hkvVec3& segEnd, hkvVec3* pClosestPoint = 0 );

// \brief
//   calculate distance of point to the ray
// 
// this function calculates the distance of a given point to the given ray. If requested then it
// returns also the point on the ray which has minimum distance from the input point.
// 
// \param pt
//   [in] test point which distance to the ray is calculated
// 
// \param rayStart
//   [in]:  start point of the ray
// 
// \param rayDir
//   [in] ray direction
// 
// \param pClosestPoint
//   [out] if nonzero then after the function finishes it contains the point on the ray which has
//   minimum distance to the input point (pt).
// 
// \return
//   distance of the point from the ray 
VBASE_IMPEXP float GetPointRayDistance( const hkvVec3& pt, const hkvVec3& rayStart, const hkvVec3& rayDir, hkvVec3* pClosestPoint = 0 );

// \brief
//   calculate distance of a segment to the ray
// 
// this function calculates the distance of a given segment to the given ray. If requested then it
// returns also the points on the ray and segment which distance is minimum.
// 
// \param segStart
//   [in] start point of the segment
// 
// \param segEnd
//   [in] end point of the segment
// 
// \param rayStart
//   [in] start point of the ray
// 
// \param rayDir
//   [in] ray direction
// 
// \param pSegPoint
//   [out] if nonzero then point on the segment most close to the ray.
// 
// \param pRayPoint
//   [out] if nonzero then point on the ray most close to the segment.
// 
// \return
//   distance of the segment to the ray 
VBASE_IMPEXP float GetSegmentRayDistance( const hkvVec3& segStart, const hkvVec3& segEnd, const hkvVec3& rayStart, const hkvVec3& rayDir, hkvVec3* pSegPoint = 0, hkvVec3* pRayPoint = 0 );

#endif // _VISION_VINTERSECT_HPP_

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
