/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVALIGNEDBOUNDINGBOX_H
#define VBASE_HKVMATH_HKVALIGNEDBOUNDINGBOX_H

#include <Vision/Runtime/Base/Math/hkvMath.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>

/// \brief
///   Serializes the bbox to/from the archive (reads/writes exactly 6 floats).
VBASE_IMPEXP void SerializeX (VArchive& ar, hkvAlignedBBox& obj);


/// \brief
///   An axis aligned bounding box class.
class hkvAlignedBBox
{
public:

  ///
  /// @name Constructors
  /// @{
  ///

  #ifdef HKVMATH_DEFAULTCONSTRUCTORS_INITIALIZEDATA
    /// \brief
    ///   DEPRECATED: The default constructor will currently initialize the bbox to an 'invalid' state.
    ///
    /// For compatibility reasons the default constructor will set m_vMin to the maximum positive 
    /// float value, and m_vMax to the maximum negative float value (min > max), making it an invalid bbox.
    ///
    /// At some point the Vision Engine will switch behavior such that all math classes
    /// are default constructed to uninitialized values. Therefore wherever possible
    /// please prefer the constructor that explicitly initializes the box to hkvNoInitialization.
    ///
    /// You can find all the places where you use the default constructor by defining 
    /// HKVMATH_DEPRECATE_DEFAULT_CONSTRUCTOR in hkvMathConfig.h and compiling your code for Windows.
    /// Then the compiler will generate a warning for every location where you use the default constructor.
    /// Use the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION to prevent that warning
    /// for code that cannot be changed to use a non default constructor (such as for arrays).
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvAlignedBBox () { setInvalid (); }

  #else

    /// \brief 
    ///   FUTURE BEHAVIOR: The data is not initialized.
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvAlignedBBox () {}
  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvAlignedBBox (hkvInit_None init /* = hkvNoInitialization */) { }

  /// \brief
  ///   Initializes the box with the given min and max values.
  HKV_FORCE_INLINE hkvAlignedBBox (const hkvVec3& vMin, const hkvVec3& vMax);

  ///
  /// @}
  ///

  ///
  /// @name Setup
  /// @{
  ///

  /// \brief
  ///   Sets the box to all zero.
  ///
  /// \note
  ///   This is a valid bounding box with a volume of zero.
  HKV_FORCE_INLINE void setZero ();

  /// \brief
  ///   Sets the box's min and max values from the given center and half-size extents.
  HKV_FORCE_INLINE void setCenterAndSize (const hkvVec3& vCenter, const hkvVec3& vHalfSizeXYZ);

  /// \brief
  ///   Sets the box's min and max values. In debug mode this will check that vMin <= vMax, to ensure a valid bbox.
  HKV_FORCE_INLINE void set (const hkvVec3& vMin, const hkvVec3& vMax);

  /// \brief
  ///   Sets the box's min and max values. Never checks whether vMin <= vMax, to allow to set invalid boxes. Useful when initializing data from serialized data.
  HKV_FORCE_INLINE void setWithoutValidityCheck (const hkvVec3& vMin, const hkvVec3& vMax);

  /// \brief
  ///   Sets the box to be the bounding box of the given set of points.
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
  VBASE_IMPEXP void setFromPoints (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3));

  ///
  /// @}
  ///

  ///
  /// @name Piecewise Setup
  /// @{
  ///

  /// \brief
  ///
  /// Sets m_vMin to the maximum positive float value, and m_vMax to the maximum negative float value (min > max),
  /// making it an invalid box.
  ///
  /// The various expandToInclude functions can then be used to enlarge the box until it is large enough.
  ///
  /// \sa hkvAlignedBBox::expandToInclude
  HKV_FORCE_INLINE void setInvalid ();

  /// \brief
  ///   Returns true if m_vMin <= m_vMax.
  HKV_FORCE_INLINE bool isValid (void) const;

  /// \brief
  ///   Enlarges the box such that v is contained in it (if necessary).
  HKV_FORCE_INLINE void expandToInclude (const hkvVec3& v);

  /// \brief
  ///   Enlarges the box such that the given set of points is contained in it (if necessary).
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
  VBASE_IMPEXP    void expandToInclude (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3));

  /// \brief
  ///   Enlarges the box such that the given object is contained in it (if necessary).
  HKV_FORCE_INLINE void expandToInclude (const hkvAlignedBBox& rhs);

  /// \brief
  ///   Enlarges the box such that the given object is contained in it (if necessary).
  HKV_FORCE_INLINE void expandToInclude (const hkvBoundingSphere& rhs);

  /// \brief
  ///   Makes all sides of the box as long as its longest side, thus making it a cube.
  HKV_FORCE_INLINE void expandToCube ();

  /// \brief
  ///   Changes the size by vChangeXYZ in each direction, e.g. along the positive AND negative axis.
  HKV_FORCE_INLINE void addBoundary (const hkvVec3& vChangeXYZ);

  ///
  /// @}
  ///

  ///
  /// @name Modification
  /// @{
  ///

  /// \brief 
  ///   Transforms the 8 corners in object-space and then recomputes the AABB.
  HKV_FORCE_INLINE void transformFromCenter (const hkvMat4& mTransform);

  /// \brief 
  ///   Transforms the 8 corners in world-space and then recomputes the AABB.
  HKV_FORCE_INLINE void transformFromOrigin (const hkvMat4& mTransform);

  /// \brief
  ///   Translates the box in world-space.
  HKV_FORCE_INLINE void translate (const hkvVec3& vTranslation);

  /// \brief
  ///   Scales the box's size, but does not change its position.
  HKV_FORCE_INLINE void scaleFromCenter (const hkvVec3& vScaleXYZ);

  /// \brief
  ///   Scales the box's min and max extents, thus changing its size and position.
  HKV_FORCE_INLINE void scaleFromOrigin (const hkvVec3& vScaleXYZ);

  ///
  /// @}
  ///

  ///
  /// @name Checks
  /// @{
  ///

  /// \brief
  ///   Checks whether the two boxes are identical.
  HKV_FORCE_INLINE bool isIdentical (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Checks whether the two boxes are equal withing some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvAlignedBBox& rhs, float fEpsilon) const;

  /// \brief
  ///   Checks whether the box is all zero within some epsilon range.
  HKV_FORCE_INLINE bool isZero (float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  ///
  /// @}
  ///

  ///
  /// @name Retrieving Data
  /// @{
  ///

  /// \brief
  ///   Returns the center point of the box.
  HKV_FORCE_INLINE const hkvVec3 getCenter () const;

  /// \brief
  ///   Returns the box's extent along the X axis.
  HKV_FORCE_INLINE float getSizeX () const;

  /// \brief
  ///   Returns the box's extent along the Y axis.
  HKV_FORCE_INLINE float getSizeY () const;

  /// \brief
  ///   Returns the box's extent along the Z axis.
  HKV_FORCE_INLINE float getSizeZ () const;

  /// \brief
  ///   Writes the 8 corners of the box to the provided array.
  ///
  /// \param out_pVertices
  ///   The array to which the corners are written. Must be large enough to store 8 vectors.
  ///
  /// \sa hkvAlignedBBox::getCorner
  HKV_FORCE_INLINE void getCorners (hkvVec3* HKV_RESTRICT out_pVertices) const;

  /// \brief
  ///   Returns one corner of the box.
  ///
  /// \param bMaxX
  ///   If set to true the X coordinate of the returned corner will be from the max extent.
  ///   Otherwise it will be from the min extent.
  ///
  /// \param bMaxY
  ///   If set to true the Y coordinate of the returned corner will be from the max extent.
  ///   Otherwise it will be from the min extent.
  ///
  /// \param bMaxZ
  ///   If set to true the Z coordinate of the returned corner will be from the max extent.
  ///   Otherwise it will be from the min extent.
  ///
  /// \sa hkvAlignedBBox::getCorners
  HKV_FORCE_INLINE hkvVec3 getCorner (bool bMaxX, bool bMaxY, bool bMaxZ) const;

  /// \brief
  ///   Checks which of the extents (X,Y,Z) is the longest. Returns that value and its index (0,1,2).
  ///
  /// \param out_iAxis
  ///   Optional. If set the index of the longest extent (0, 1 or 2) will be stored here.
  ///
  /// \return
  ///   The length of the longest extent.
  HKV_FORCE_INLINE float getMaxExtent (hkUint32* HKV_RESTRICT out_iAxis = NULL) const;

  /// \brief
  ///   Checks which of the extents (X,Y,Z) is the shortest. Returns that value and its index (0,1,2).
  ///
  /// \param out_iAxis
  ///   Optional. If set the index of the shortest extent (0, 1 or 2) will be stored here.
  ///
  /// \return
  ///   The length of the shortest extent.
  HKV_FORCE_INLINE float getMinExtent (hkUint32* HKV_RESTRICT out_iAxis = NULL) const;

  /// \brief
  ///   Returns a hkvBoundingSphere that perfectly fits around this box.
  HKV_FORCE_INLINE const hkvBoundingSphere getBoundingSphere () const;

  ///
  /// @}
  ///

  ///
  /// @name Bounding Volume Checks
  /// @{
  ///

  /// \brief
  ///   Returns true if the given point is inside the box.
  HKV_FORCE_INLINE bool contains (const hkvVec3& v) const;

  /// \brief
  ///   Returns true if all points from the given set are inside the box.
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
  VBASE_IMPEXP    bool contains (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   Returns true if the given box is inside this box.
  HKV_FORCE_INLINE bool contains (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Returns true if the given sphere is inside this box.
  HKV_FORCE_INLINE bool contains (const hkvBoundingSphere& rhs) const;

  /// \brief
  ///   Returns true if the given object is inside this box or partially overlaps it.
  HKV_FORCE_INLINE bool overlaps (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Returns true if the given object is inside this box or partially overlaps it.
  HKV_FORCE_INLINE bool overlaps (const hkvBoundingSphere& rhs) const;

  ///
  /// @}
  ///

  ///
  /// @name Distance Checks
  /// @{
  ///

  /// \brief
  ///   Returns the squared distance of the point v to this box.
  ///
  /// For points that are inside the box this function returns zero.
  ///
  /// \param v
  ///   The point for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceToSquared (const hkvVec3& v) const;

  /// \brief
  ///   Returns the squared distance of the given box to this box.
  ///
  /// If the two boxes overlap, zero is returned.
  ///
  /// \param rhs
  ///   The box for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceToSquared (const hkvAlignedBBox& rhs) const;
  
  /// \brief
  ///   Returns the distance of the point v to this box.
  ///
  /// For points that are inside the box this function returns zero.
  ///
  /// \param v
  ///   The point for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvVec3& v) const;

  /// \brief
  ///   Returns the distance of the given box to this box.
  ///
  /// If the two boxes overlap, zero is returned.
  ///
  /// \param rhs
  ///   The box for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvAlignedBBox& rhs) const;


  /// \brief
  ///   Returns the distance of the given sphere to this box.
  ///
  /// If the two objects overlap, zero is returned.
  ///
  /// \param rhs
  ///   The sphere for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvBoundingSphere& rhs) const;

  ///
  /// @}
  ///

  ///
  /// @name Intersection Tests
  /// @{
  ///

  /// \brief
  ///   Checks whether the given ray intersects the surface of the box and computes the intersection time and point.
  ///
  /// If the ray start position is within the box, and intersection with the surface will always be detected.
  /// If however, you need to handle such cases as if the box was solid and the intersection point should already be
  /// at the ray start position, you should check first whether the box contains the ray start position already.
  ///
  /// \param vRayStartPos
  ///   The start position of the ray. If the start position is inside the box, an intersection will always
  ///   be detected, however not at time 0, but wherever the ray hits the surface of the box.
  ///
  /// \param vRayDir
  ///   The direction of the ray. May be normalized but is not required.
  ///
  /// \param out_fIntersectionTime
  ///   Optional. If set, the 'intersection-time' will be stored here.
  ///   The intersection time will always be >= 0.
  ///   If the ray misses the box, this value will not be modified.
  ///   vRayStartPos + vRayDir * *out_fIntersectionTime == *out_IntersectionPoint
  ///   That means the intersection time scales anti-proportionally to the length of vRayDir.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If set, the point of intersection will be stored here.
  ///   If the ray misses the box, this value will not be modified.
  ///   vRayStartPos + vRayDir * *out_fIntersectionTime == *out_IntersectionPoint
  ///
  /// \return
  ///   True if the ray hits the box, false otherwise.
  ///
  /// \sa hkvAlignedBBox::getLineSegmentIntersection
  VBASE_IMPEXP    bool getRayIntersection (const hkvVec3& vRayStartPos, const hkvVec3& vRayDir, float* HKV_RESTRICT out_fIntersectionTime = NULL, hkvVec3* HKV_RESTRICT out_IntersectionPoint = NULL) const;

  /// \brief
  ///   Computes the intersection of a line segment and the surface of the box.
  ///
  /// If the line segment's start and end point are both within the box, no intersection will be reported.
  /// If you need to handle these cases as intersections, test first, whether the box contains the start 
  /// position already.
  ///
  /// \param vLineStartPos
  ///   The start position of the line.
  ///   If the start position is inside the box, an intersection will only be detected, if the end point
  ///   is outside the box.
  ///
  /// \param vLineEndPos
  ///   The end position of the line.
  ///
  /// \param out_fHitFraction
  ///   Optional. If set, the fraction along the line, where the intersection point is located,
  ///   will be stored here. This value will always be between 0 and 1.\n
  ///   E.g: vLineStartPos + (vLineEndPos - vLineStartPos) * *out_fHitFraction == *out_IntersectionPoint\n
  ///   Or in other words:\n
  ///   hkvMath::interpolate (vLineStartPos, vLineEndPos, *out_fHitFraction) == *out_IntersectionPoint\n
  ///   If the line does not intersect with the box, this variable will not be modified.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If set, the intersection point will be stored here.
  ///   If the line does not intersect with the box, this variable will not be modified.\n
  ///   vLineStartPos + (vLineEndPos - vLineStartPos) * *out_fHitFraction == *out_IntersectionPoint\n
  ///
  /// \return
  ///   true if the line segment intersects with the box, false otherwise.
  ///
  /// \sa hkvAlignedBBox::getRayIntersection
  HKV_FORCE_INLINE bool getLineSegmentIntersection (const hkvVec3& vLineStartPos, const hkvVec3& vLineEndPos, float* HKV_RESTRICT out_fHitFraction = NULL, hkvVec3* HKV_RESTRICT out_IntersectionPoint = NULL) const;

  ///
  /// @}
  ///

  ///
  /// @name Utility Functions
  /// @{
  ///

  /// \brief
  ///   Returns v clamped to the volume of the box.
  ///
  /// If v is already inside the box, it will be returned unchanged.
  /// If v is outside the box, its components will be clamped to the min and max extents of the box.
  ///
  /// \param v
  ///   The point which shall be clamped to the box's volume.
  ///
  /// \sa hkvAlignedBBox::clampPositions
  HKV_FORCE_INLINE hkvVec3 getClampedPosition (const hkvVec3& v) const;

  /// \brief
  ///   Clamps a set of points to the volume of the box.
  ///
  /// If a point is already inside the box, it will not be modified.
  /// If a point is outside the box, its components will be clamped to the min and max extents of the box.
  ///
  /// \param inout_pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  ///
  /// \sa hkvAlignedBBox::getClampedPosition
  VBASE_IMPEXP void clampPositions (hkvVec3* HKV_RESTRICT inout_pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief Serializes the BBox the way VBBoxf was serialized (6 floats). Prefer this for NEW code.
  HKV_FORCE_INLINE void SerializeAs_VBBoxf (VArchive& ar) { SerializeX (ar, *this); }

  /// \brief Serializes the BBox the way VisBoundingBox_cl was serialized (8 floats).
  VBASE_IMPEXP void SerializeAs_VisBoundingBox (VArchive& ar);

  ///
  /// @}
  ///

public:

  /// \brief
  ///   The minimum corner of the box.
  hkvVec3 m_vMin;

  /// \brief
  ///   The maximum corner of the box.
  hkvVec3 m_vMax;

public:


  ///
  /// @name Compatibility Interface (DEPRECATED)
  /// @{
  ///

  /// \brief DEPRECATED: Use hkvNoInitialization instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE explicit hkvAlignedBBox (bool b) { }

  /// \brief DEPRECATED: Use hkvAlignedBBox::hkvAlignedBBox (hkvVec3 vMin, hkvVec3 vMax) instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE hkvAlignedBBox (float minx, float miny, float minz, float maxx, float maxy, float maxz)
  {
    setWithoutValidityCheck (hkvVec3 (minx, miny, minz), hkvVec3 (maxx, maxy, maxz));
  }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getCenter instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE const hkvVec3 GetCenter () const { return getCenter (); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::overlaps instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE VBool Touches (const hkvAlignedBBox& rhs) const { return overlaps (rhs) ? TRUE : FALSE; } 

  /// \brief DEPRECATED: Use hkvAlignedBBox::setInvalid instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Reset () { setInvalid (); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getCorners instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void GetCorners (hkvVec3 v[8]) const { getCorners (v); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::isValid instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE bool IsValid() const { return isValid (); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getSizeX instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetSizeX () const { return getSizeX (); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getSizeY instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetSizeY () const { return getSizeY (); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getSizeZ instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetSizeZ () const { return getSizeZ (); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::addBoundary instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Expand (float f) { addBoundary (hkvVec3 (f)); }

  /// \brief DEPRECATED: Access m_vMin and m_vMax directly instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void GetBounds (hkvVec3& vMin, hkvVec3& vMax) const { vMin = m_vMin; vMax = m_vMax; }

  /// \brief DEPRECATED: Use hkvAlignedBBox::set or hkvAlignedBBox::setWithoutValidityCheck instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Set (const hkvVec3& vMin, const hkvVec3& vMax) { setWithoutValidityCheck (vMin, vMax); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::set or hkvAlignedBBox::setWithoutValidityCheck instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Set (float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ) 
  { 
    setWithoutValidityCheck (hkvVec3 (fMinX, fMinY, fMinZ), hkvVec3 (fMaxX, fMaxY, fMaxZ)); 
  }

  /// \brief DEPRECATED: Use hkvAlignedBBox::contains instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE VBool Contains (const hkvAlignedBBox& rhs) const { return contains (rhs); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::addBoundary instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Add (float border) {addBoundary (hkvVec3 (border)); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::expandToInclude instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Add (const hkvVec3& v) { expandToInclude (v); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::expandToInclude instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Add (float x, float y, float z) { expandToInclude (hkvVec3 (x, y, z)); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::expandToInclude instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Add (const hkvAlignedBBox& rhs) { expandToInclude (rhs); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getBoundingSphere or compute (m_vMin.getDistanceTo (m_vMax) * 0.5f) directly.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE float GetRadius () const { return (m_vMin.getDistanceTo (m_vMax) * 0.5f); }

  /// \brief
  ///   DEPRECATED
  ///
  /// Use hkvAlignedBBox::getLineSegmentIntersection or hkvAlignedBBox::getRayIntersection instead.
  /// See the inline implementation of this function for more details about how to convert old code.
  ///
  /// \param rayStart
  ///   DEPRECATED
  ///
  /// \param rayDir
  ///   DEPRECATED
  ///
  /// \param farT
  ///   DEPRECATED
  ///
  /// \param pT
  ///   DEPRECATED
  ///
  /// \param pTouchPoint
  ///   DEPRECATED
  ///
  /// \sa hkvAlignedBBox::getLineSegmentIntersection
  /// \sa hkvAlignedBBox::getRayIntersection
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE VBool Intersect (const hkvVec3& rayStart, const hkvVec3& rayDir, float farT = -1, float *pT = NULL, hkvVec3* pTouchPoint = NULL) const
  {
    if (farT > 0)
    {
      float fFraction;
      hkvVec3 vHit;
      if (!getLineSegmentIntersection (rayStart, rayStart + rayDir * farT, &fFraction, &vHit))
        return FALSE;

      if (pTouchPoint)
        *pTouchPoint = vHit;

      if (pT)
        *pT = fFraction * farT;

      return TRUE;
    }
    else
      return (getRayIntersection (rayStart, rayDir, pT, pTouchPoint) ? TRUE : FALSE);
  }

  /// \brief DEPRECATED: Use hkvAlignedBBox::contains instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE VBool PointInBox (const hkvVec3& point) const { return contains (point) ? TRUE : FALSE; }

  /// \brief DEPRECATED: Use hkvAlignedBBox::expandToCube instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void MakeCubic () { expandToCube (); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::isValid instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE VBool IsEmpty () const { return (!isValid ()) ? TRUE : FALSE; }

  /// \brief DEPRECATED: Access the members m_vMin and m_vMax directly instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void GetBounds (float &minX, float &minY, float &minZ, float &maxX, float &maxY, float &maxZ) const { minX = m_vMin.x; minY = m_vMin.y; minZ = m_vMin.z;  maxX = m_vMax.x; maxY = m_vMax.y; maxZ = m_vMax.z; }

  /// \brief DEPRECATED: Use 'hkvAlignedBBox::operator=' instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Set (const hkvAlignedBBox& rhs) { *this = rhs; }

  /// \brief DEPRECATED: Use 'hkvAlignedBBox::expandToInclude' instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Inflate (const hkvAlignedBBox& rhs) { expandToInclude (rhs); }

  /// \brief DEPRECATED: Use 'hkvAlignedBBox::expandToInclude' instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Inflate (const hkvVec3& pos) { expandToInclude (pos); }

  /// \brief DEPRECATED: Use 'hkvAlignedBBox::addBoundary' instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Inflate (float fDist) { addBoundary (hkvVec3 (fDist)); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::expandToInclude instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Inflate (hkvAlignedBBox bbox, const hkvMat4& mTransform)
  {
    bbox.transformFromOrigin (mTransform);
    expandToInclude (bbox);
  }

  /// \brief DEPRECATED: Use hkvAlignedBBox::expandToInclude instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Inflate (hkvAlignedBBox bbox, const hkvMat3& mOrientation, const hkvVec3& vBoxCenter)
  {
    hkvAlignedBBox temp = bbox;
    hkvMat4 mTransform (mOrientation, vBoxCenter);
    temp.transformFromOrigin (mTransform);
    this->expandToInclude (temp);
  }

  /// \brief DEPRECATED: Use hkvAlignedBBox::contains instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE BOOL IsInside(const hkvVec3& pos) const { return (contains (pos) ? TRUE : FALSE); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::contains instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE BOOL IsCompletelyInside (const hkvAlignedBBox& bbox) const { return (contains (bbox) ? TRUE : FALSE); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::contains instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE BOOL IsCompletelyInside (const hkvBoundingSphere& sphere) const { return (contains (sphere) ? TRUE : FALSE); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::translate instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void operator+= (const hkvVec3& v) { translate (v); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::translate instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void operator-= (const hkvVec3& v) { translate (-v); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getDistanceTo instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetDistance (const hkvVec3& v) const { return getDistanceTo (v); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getDistanceToSquared instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetDistanceSqr (const hkvVec3& v) const { return getDistanceToSquared (v); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getMinExtent instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE float GetMinExtent () const { return getMinExtent (); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getMaxExtent instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE float GetMaxExtent () const { return getMaxExtent (); }

  /// \brief DEPRECATED: Use hkvPlane::getMinimumDistanceTo instead.
  VBASE_IMPEXP float DetermineMinDistanceFromPlane(const hkvPlane& planeDist) const;

  /// \brief DEPRECATED: Use hkvPlane::getMinMaxDistanceTo instead.
  VBASE_IMPEXP float DetermineMaxDistanceFromPlane(const hkvPlane& planeDist) const;

  /// \brief DEPRECATED: Use hkvAlignedBBox::overlaps instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE BOOL IntersectsWith (const hkvAlignedBBox& bbox) const { return overlaps (bbox); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::overlaps instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE BOOL IntersectsWith (const hkvBoundingSphere& sphere) const { return overlaps (sphere); }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getRayIntersection instead.
  VBASE_IMPEXP bool IntersectsLine (const hkvVec3& startPos, const hkvVec3& endPos) const;

  /// \brief DEPRECATED: Use hkvAlignedBBox::getLineSegmentIntersection instead.
  VBASE_IMPEXP bool IntersectsRay (const hkvVec3& startPos, const hkvVec3& endPos, hkvVec3& intersectionPos) const;

  /// \brief DEPRECATED: Use hkvAlignedBBox::getCorners and hkvMat4::transformPositions instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void GetCorners (hkvVec3 v[8], const hkvMat4& mTransformation) const
  {
    getCorners (v);
    mTransformation.transformPositions (v, 8);
  }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getCorners and hkvMat4::transformPositions instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void GetCorners (hkvVec3 v[8], const hkvMat3& mOrientation, const hkvVec3& vTranslation) const
  {
    hkvMat4 mTransformation (mOrientation, vTranslation);
    getCorners (v);
    mTransformation.transformPositions (v, 8);
  }

  /// \brief DEPRECATED: Use hkvAlignedBBox::getMaxExtent instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE int GetLargestExtentAxis () const 
  {
    hkUint32 iAxis = 0;
    getMaxExtent (&iAxis);
    return (int) iAxis;
  }

  /// \brief DEPRECATED
  HKV_FORCE_INLINE float GetDistanceSqrXY (const hkvVec3& pos) const
  {
    hkvVec2 clamped (hkvMath::Min (pos.x,m_vMax.x), hkvMath::Min (pos.y,m_vMax.y));
    clamped.x = hkvMath::Max (clamped.x, m_vMin.x) - pos.x;
    clamped.y = hkvMath::Max (clamped.y, m_vMin.y) - pos.y;
    return clamped.x*clamped.x + clamped.y*clamped.y;

  }

  /// \brief DEPRECATED
  HKV_FORCE_INLINE float GetDistanceXY (const hkvVec3& pos) const {return hkvMath::sqrt (GetDistanceSqrXY (pos));}

  /// \brief DEPRECATED: Use expandToInclude instead.
  VBASE_IMPEXP void Inflate (const hkvVec3& vOrigin, const hkvVec3& vDir, const hkvVec3& vRight, const hkvVec3& vUp, float fFOVX, float fFOVY, float fNearClip, float fFarClip);

  /// \brief DEPRECATED: Use getBoundingSphere instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE float GetLocalRadius() const { return (m_vMax-m_vMin).getLength () * 0.5f; }

  /// \brief DEPRECATED: Use setZero instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Clear() { setZero (); }

  /// \brief DEPRECATED: Use getClampedPosition instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Clamp (hkvVec3& v) const { v = getClampedPosition (v); }

  /// \brief DEPRECATED: Use getDistanceTo instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float MinDist(const hkvVec3& v) const { return getDistanceTo (v); }

  /// \brief DEPRECATED: Use getDistanceTo instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float MinDist(const hkvAlignedBBox& b) const { return getDistanceTo (b); }

  /// \brief DEPRECATED: Use getClampedPosition instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void ClampPosition (hkvVec3& pos) const { pos = getClampedPosition (pos); }

  /// \brief DEPRECATED: Use expandToInclude instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Add (const hkvVec3* pVertList, int iCount) { expandToInclude (pVertList, iCount); }

  /// \brief DEPRECATED: Use getDistanceTo instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetMinDist (const hkvAlignedBBox& b) const { return getDistanceTo (b); }

  ///
  /// @}
  ///

  // Missing Functions:

  // void Add(const VBBoxf &bb, const M &m)
  // float GetMaxAbsExtent() const


};

#ifdef HKVMATH_ENABLE_NEW_SERIALIZATION_OPERATORS
  // Currently deactivated to force people to use SerializeAs_VBBoxf or SerializeAs_VisBoundingBox instead.
  // Will be activated in some later release.
  V_DECLARE_SERIALX_NONINTRUSIVE (hkvAlignedBBox, VBASE_IMPEXP);
#endif

/// \brief
///   Checks whether the two boxes are identical.
HKV_FORCE_INLINE bool operator== (const hkvAlignedBBox& lhs, const hkvAlignedBBox& rhs);

/// \brief
///   Checks whether the two boxes are not identical.
HKV_FORCE_INLINE bool operator!= (const hkvAlignedBBox& lhs, const hkvAlignedBBox& rhs);



#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.inl>

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
