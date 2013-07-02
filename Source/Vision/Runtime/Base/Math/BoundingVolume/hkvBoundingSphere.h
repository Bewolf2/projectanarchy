/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVBOUNDINGSPHERE_H
#define VBASE_HKVMATH_HKVBOUNDINGSPHERE_H

#include <Vision/Runtime/Base/Math/hkvMath.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>

/// \brief
///   A bounding sphere that can be used for culling operations.
///
/// The sphere is represented as a point and a radius.
class hkvBoundingSphere
{
public:

  ///
  /// @name Constructors
  /// @{
  ///

  /// \brief
  ///   The default constructor does not initialize any data.
  HKV_FORCE_INLINE hkvBoundingSphere ();

  /// \brief
  ///   Initializes the center and radius directly.
  HKV_FORCE_INLINE hkvBoundingSphere (const hkvVec3& vCenter, float fRadius);

  /// \brief 
  ///   The sphere center is set to the center-of-gravity of the given set of points. The radius is set such that the sphere includes all points.
  ///
  /// \param pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  HKV_FORCE_INLINE hkvBoundingSphere (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3));

  ///
  /// @}
  ///

  ///
  /// @name Setup
  /// @{
  ///

  /// \brief
  ///   Sets all data to zero.
  HKV_FORCE_INLINE void setZero ();

  /// \brief
  ///   Checks whether all components are zero within some epsilon range.
  HKV_FORCE_INLINE bool isZero (float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  /// \brief
  ///   Sets the sphere center and radius.
  HKV_FORCE_INLINE void set (const hkvVec3& vCenter, float fRadius);

  /// \brief
  ///   The sphere center is set to the center-of-gravity of the given set of points. The radius is set such that the sphere includes all points.
  ///
  /// \param pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  VBASE_IMPEXP void set (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3));

  ///
  /// @}
  ///

  ///
  /// @name Piecewise setup
  /// @{
  ///

  /// \brief
  ///   Increases the sphere's radius such that the given point is contained in the resulting sphere.
  ///
  /// The sphere's center is never changed. Also the radius does not change, if vPoint is already inside in the sphere.
  ///
  /// \param vPoint
  ///   After expanding the sphere this point will be contained in it.
  HKV_FORCE_INLINE void expandToInclude (const hkvVec3& vPoint);

  /// \brief
  ///   Increases the sphere's radius such that all the given points are contained in the resulting sphere.
  ///
  ///   The sphere's center is never changed. Also the radius does not change, if all the points are already inside in the sphere.
  ///
  /// \param pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  HKV_FORCE_INLINE void expandToInclude (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3));

  /// \brief
  ///   Increases the sphere's radius such that the given sphere is contained in the resulting sphere.
  ///
  /// The sphere's center is never changed. Also the radius does not change, if the given sphere is already inside in the sphere.
  ///
  /// \param rhs
  ///   After expansion this object will be contained in the sphere.
  HKV_FORCE_INLINE void expandToInclude (const hkvBoundingSphere& rhs);

  /// \brief
  ///   Increases the sphere's radius such that the given box is contained in the resulting sphere.
  ///
  /// The sphere's center is never changed. Also the radius does not change, if the box is already inside in the sphere.
  ///
  /// \param rhs
  ///   After expansion this object will be contained in the sphere.
  HKV_FORCE_INLINE void expandToInclude (const hkvAlignedBBox& rhs);

  ///
  /// @}
  ///


  ///
  /// @name Modification
  /// @{
  ///

  /// \brief
  ///   Transforms the sphere using the origin (ie. (0|0|0)) as the pivot.
  ///
  /// \param mTransform 
  ///   The transformation to be applied on the sphere.
  ///
  /// The center of the sphere is simply multiplied by mTransform. 
  /// Thus all translations, rotations and scalings are applied as to any other point.
  /// The radius is multiplied by the largest scaling factor in the transformation matrix.
  /// That means for a uniform scaling matrix, the sphere scales as expected. However in case
  /// of a non-uniform scaling the radius will be multiplied by the largest scaling factor
  /// of the matrix. E.g. if the matrix scales an object by 2 along X and Y, but by 3 along Z,
  /// then the bounding sphere's radius will be scaled by 3.
  /// This makes sure that the bounding sphere will always be large enough to enclose all
  /// points of an object.
  ///
  /// \sa hkvBoundingSphere::transformFromCenter
  HKV_FORCE_INLINE void transformFromOrigin (const hkvMat4& mTransform);

  /// \brief
  ///   Transforms the sphere using the center of the sphere as the pivot.
  ///
  /// \param mTransform
  ///   The transformation to be applied on the sphere.
  ///
  /// The sphere's center is used as the pivot for the transformation. 
  /// That means that only the translation part of mTransform will have any effect
  /// on the sphere's center, as rotating a sphere around itself does not change the sphere
  /// and scaling a pivot-point also has no effect on that point.
  /// 
  /// The radius is multiplied by the largest scaling factor in the transformation matrix.
  /// That means for a uniform scaling matrix, the sphere scales as expected. However in case
  /// of a non-uniform scaling the radius will be multiplied by the largest scaling factor
  /// of the matrix. E.g. if the matrix scales an object by 2 along X and Y, but by 3 along Z,
  /// then the bounding sphere's radius will be scaled by 3.
  /// This makes sure that the bounding sphere will always be large enough to enclose all
  /// points of an object.
  ///
  /// \sa hkvBoundingSphere::transformFromOrigin
  HKV_FORCE_INLINE void transformFromCenter (const hkvMat4& mTransform);

  /// \brief
  ///   Translates the sphere's center by the given amount.
  HKV_FORCE_INLINE void translate (const hkvVec3& vTranslation);

  /// \brief
  ///   Scales the sphere's radius but does not change its position.
  HKV_FORCE_INLINE void scaleFromCenter (float xyz);

  /// \brief
  ///   Scales the sphere's center and radius by the given factor, thus moving it away from the origin.
  HKV_FORCE_INLINE void scaleFromOrigin (float xyz);

  /// \brief
  ///   Scales the sphere's center component-wise by vScaleXYZ. Scales its radius by the largest component of vScaleXYZ.
  ///
  /// The center of the sphere is simply multiplied component-wise by vScaleXYZ.
  /// The radius is multiplied by the largest component of vScaleXYZ.
  /// E.g. if vScaleXYZ scales an object by 2 along X and Y, but by 3 along Z,
  /// then the bounding sphere's radius will be scaled by 3.
  /// This makes sure that the bounding sphere will always be large enough to enclose all
  /// points of an object.
  ///
  /// \param vScaleXYZ
  ///   The scaling factors along each axis.
  ///
  /// \sa hkvBoundingSphere::transformFromCenter
  HKV_FORCE_INLINE void scaleFromOrigin (const hkvVec3& vScaleXYZ);

  /// \brief
  ///   Adds the given value to the sphere's center, thus increasing or decreasing its volume.
  HKV_FORCE_INLINE void addRadius (float fRadiusDifference);

  ///
  /// @}
  ///

  ///
  /// @name Equality Checks
  /// @{
  ///

  /// \brief
  ///   Checks whether this sphere and rhs are identical.
  HKV_FORCE_INLINE bool isIdentical (const hkvBoundingSphere& rhs) const;

  /// \brief
  ///   Checks whether this sphere and rhs are equal within some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvBoundingSphere& rhs, float fEpsilon) const;

  ///
  /// @}
  ///


  ///
  /// @name Retrieving Data
  /// @{
  ///

  /// \brief
  ///   Returns a hkvAlignedBBox that perfectly fits around this sphere.
  HKV_FORCE_INLINE const hkvAlignedBBox getAlignedBoundingBox () const;

  ///
  /// @}
  ///

  ///
  /// @name Bounding Volume Checks
  /// @{
  ///

  /// \brief
  ///   Checks whether the given point is inside the sphere.
  HKV_FORCE_INLINE bool contains (const hkvVec3& v) const;

  /// \brief
  ///   Checks whether all the given points are inside the sphere.
  ///
  /// If any of the points is not inside the sphere, false will be returned.
  ///
  /// \note There is no 'overlaps' function for a set of points, as it is possible for an object
  ///       to overlap with a sphere, although not a single point is contained in it. Without additional
  ///       connectivity information such cases cannot be properly detected.
  ///
  /// \param pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  HKV_FORCE_INLINE bool contains (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   Checks whether the given box is completely inside the sphere.
  ///
  /// If the box and the sphere overlap but the box is not completely inside the sphere,
  /// this function will return false.
  ///
  /// \param rhs
  ///   The box that is checked whether it is inside this sphere.
  HKV_FORCE_INLINE bool contains (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Checks whether the given sphere is completely inside this sphere.
  ///
  /// If the two spheres overlap but the given sphere is not completely inside this sphere,
  /// this function will return false.
  ///
  /// \param rhs
  ///   The sphere that is checked whether it is inside this sphere.
  HKV_FORCE_INLINE bool contains (const hkvBoundingSphere& rhs) const;

  /// \brief
  ///   Checks whether the given box overlaps with this sphere.
  ///
  /// If the box is inside this sphere, the function will also return true.
  /// It will only return false if the two objects are completely disjunct.
  ///
  /// \param rhs
  ///   The box that is checked whether it is inside this sphere.
  HKV_FORCE_INLINE bool overlaps (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Checks whether the given sphere overlaps with this sphere.
  ///
  /// If the sphere is inside this sphere, the function will also return true.
  /// It will only return false if the two objects are completely disjunct.
  ///
  /// \param rhs
  ///   The sphere that is checked whether it overlaps with this sphere.
  HKV_FORCE_INLINE bool overlaps (const hkvBoundingSphere& rhs) const;

  ///
  /// @}
  ///

  ///
  /// @name Distance Checks
  /// @{
  ///

  /// \brief
  ///   Returns the minimum distance between the sphere and the given point.
  ///
  /// Will return 0 for all points inside the sphere.
  ///
  /// \note There is no getDistanceToSquared function, as this cannot be computed
  /// any more efficiently.
  ///
  /// \param v
  ///   The point to which the distance should be computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvVec3& v) const;

  /// \brief
  ///   Returns the minimum distance between this sphere and the given box.
  ///
  /// Will return 0 if the two objects overlap.
  ///
  /// \note There is no getDistanceToSquared function, as this cannot be computed
  /// any more efficiently.
  ///
  /// \param rhs
  ///   The box to which the distance should be computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Returns the minimum distance between this sphere and the given sphere.
  ///
  /// Will return 0 if the two objects overlap.
  ///
  /// \note There is no getDistanceToSquared function, as this cannot be computed
  /// any more efficiently.
  ///
  /// \param rhs
  ///   The sphere to which the distance should be computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvBoundingSphere& rhs) const;

  ///
  /// @}
  ///

  ///
  /// @name Intersection Tests
  /// @{
  ///

  /// \brief
  ///   Returns true if the ray hits the surface of the sphere and computes the point of intersection.
  ///
  /// \param vRayStartPos
  ///   The start position of the ray. If the ray starts within the sphere
  ///   the function will always return true. However even in that case the returned point of
  ///   intersection will always be on the sphere's surface, ie. when the
  ///   ray exits the sphere.
  ///
  /// \param vRayDirNormalized
  ///   The direction of the ray. This parameter must be normalized, otherwise
  ///   the results are undefined.
  ///
  /// \param out_fIntersectionTime
  ///   Optional. If specified the 'intersection-time' will be stored here.\n
  ///   vRayStartPos + vRayDirNormalized * *out_fIntersectionTime == *out_IntersectionPoint\n
  ///   Nothing is written into this variable, if the ray does not hit the sphere.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If specified the point of intersection will be stored here.\n
  ///   vRayStartPos + vRayDirNormalized * *out_fIntersectionTime == *out_IntersectionPoint\n
  ///   Nothing is written into this variable, if the ray does not hit the sphere.
  ///
  /// \return true if the ray hits the sphere
  ///
  /// \sa hkvBoundingSphere::getLineSegmentIntersection
  HKV_FORCE_INLINE bool getRayIntersection (const hkvVec3& vRayStartPos, const hkvVec3& vRayDirNormalized, float* HKV_RESTRICT out_fIntersectionTime = NULL, hkvVec3* HKV_RESTRICT out_IntersectionPoint = NULL) const;

  /// \brief
  ///   Returns true if the line intersects the surface of the sphere and computes the point of intersection.
  ///
  /// This function internally just calls hkvBoundingSphere::getRayIntersection, thus it is
  /// more efficient to call that function directly, if possible.
  ///
  /// \param vLineStartPos
  ///   The start position of the line. If the line starts within the sphere
  ///   the function will only find an intersection, if the line end is outside the sphere.
  ///
  /// \param vLineEndPos
  ///   The end position of the line.
  ///
  /// \param out_fHitFraction
  ///   Optional. If specified the fraction along the line where the spher surface was hit will be stored here.\n
  ///   vLineStartPos + (vLineEndPos - vLineStartPos) * *out_fHitFraction == *out_IntersectionPoint\n
  ///   Nothing is written into this variable, if the line does not intersect the sphere.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If specified the point of intersection will be stored here.\n
  ///   vLineStartPos + (vLineEndPos - vLineStartPos) * *out_fHitFraction == *out_IntersectionPoint\n
  ///   Nothing is written into this variable, if the line does not intersect the sphere.
  ///
  /// \return true if the line intersects the sphere
  ///
  /// \sa hkvBoundingSphere::getRayIntersection
  HKV_FORCE_INLINE bool getLineSegmentIntersection (const hkvVec3& vLineStartPos, const hkvVec3& vLineEndPos, float* HKV_RESTRICT out_fHitFraction = NULL, hkvVec3* HKV_RESTRICT out_IntersectionPoint = NULL) const;

  ///
  /// @}
  ///

  ///
  /// @name Utility Functions
  /// @{
  ///

  /// \brief
  ///   Clamps vPoint to the volume of the sphere and returns that point.
  ///
  /// \param vPoint
  ///   The point that should be clamped to the sphere's volume.
  ///
  /// \return
  ///   If vPoint is inside the sphere, it will be returned without any modification.
  ///   If it is outside the sphere, the closest point on the sphere's surface is returned.
  ///
  /// \sa hkvBoundingSphere::clampPositions
  HKV_FORCE_INLINE hkvVec3 getClampedPosition (const hkvVec3& vPoint) const;

  /// \brief
  ///   Clamps all the given points to the surface of the sphere.
  ///
  /// \param inout_pPoints
  ///   The array of points that should get clamped.
  ///
  /// \param uiNumPoints
  ///   The number of points in the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the array.
  ///   Must be at least sizeof (hkvVec3) for a densely packed array.
  ///
  /// If a point of the array is inside the sphere, it will not be modified.
  /// If it is outside the sphere, it will be changed to the closest point on the sphere's surface.
  ///
  /// \sa hkvBoundingSphere::getClampedPosition
  VBASE_IMPEXP void clampPositions (hkvVec3* HKV_RESTRICT inout_pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  ///
  /// @}
  ///

public:

  /// \brief The center of the sphere
  hkvVec3 m_vCenter;

  /// \brief The radius of the sphere
  float m_fRadius;
};

V_DECLARE_SERIALX_NONINTRUSIVE (hkvBoundingSphere, VBASE_IMPEXP);

/// \brief
///   Checks whether the two spheres are identical.
HKV_FORCE_INLINE bool operator== (const hkvBoundingSphere& lhs, const hkvBoundingSphere& rhs);

/// \brief
///   Checks whether the two spheres are not identical.
HKV_FORCE_INLINE bool operator!= (const hkvBoundingSphere& lhs, const hkvBoundingSphere& rhs);



#include <Vision/Runtime/Base/Math/BoundingVolume/hkvBoundingSphere.inl>

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
