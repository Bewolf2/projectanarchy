/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVBOUNDINGSPHERE_INL
#define VBASE_HKVMATH_HKVBOUNDINGSPHERE_INL

#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>


HKV_FORCE_INLINE hkvBoundingSphere::hkvBoundingSphere ()
{
}

HKV_FORCE_INLINE hkvBoundingSphere::hkvBoundingSphere (const hkvVec3& vCenter, float fRadius)
{
  set (vCenter, fRadius);
}

HKV_FORCE_INLINE hkvBoundingSphere::hkvBoundingSphere (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride /* = sizeof (hkvVec3) */)
{
  set (pPoints, uiNumPoints, uiStride);
}

HKV_FORCE_INLINE void hkvBoundingSphere::setZero ()
{
  m_vCenter.setZero ();
  m_fRadius = 0.0f;
}

HKV_FORCE_INLINE bool hkvBoundingSphere::isZero (float fEpsilon) const
{
  return (m_vCenter.isZero (fEpsilon) && hkvMath::isZero (m_fRadius, fEpsilon));
}

HKV_FORCE_INLINE void hkvBoundingSphere::set (const hkvVec3& vCenter, float fRadius)
{
  m_vCenter = vCenter;
  m_fRadius = fRadius;
}

HKV_FORCE_INLINE void hkvBoundingSphere::expandToInclude (const hkvVec3& vPoint)
{
  // this function assumes that the sphere is already valid
  // it will only increase its radius to include the point

  float fRadiusSQR = m_vCenter.getDistanceToSquared (vPoint);

  if (fRadiusSQR > m_fRadius * m_fRadius)
    m_fRadius = hkvMath::sqrt (fRadiusSQR);
}

HKV_FORCE_INLINE void hkvBoundingSphere::expandToInclude (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride /* = sizeof (hkvVec3) */)
{
  VASSERT (pPoints != NULL);
  VASSERT (uiNumPoints > 0);
  VASSERT (uiStride >= sizeof (hkvVec3));

  hkvVec3* HKV_RESTRICT pPoint = (hkvVec3*) pPoints;

  float fRadiusSQR = 0.0f;

  for (hkUint32 i = 0; i < uiNumPoints; ++i)
  {
    fRadiusSQR = hkvMath::Max (fRadiusSQR, m_vCenter.getDistanceToSquared (*pPoint));

    pPoint = hkvAddByteOffset (pPoint, uiStride);
  }

  if (fRadiusSQR > m_fRadius * m_fRadius)
    m_fRadius = hkvMath::sqrt (fRadiusSQR);
}

HKV_FORCE_INLINE void hkvBoundingSphere::expandToInclude (const hkvBoundingSphere& rhs)
{
  float fRequiredRadius = m_vCenter.getDistanceTo (rhs.m_vCenter) + rhs.m_fRadius;

  m_fRadius = hkvMath::Max (m_fRadius, fRequiredRadius);
}

HKV_FORCE_INLINE void hkvBoundingSphere::expandToInclude (const hkvAlignedBBox& rhs)
{
  // TODO (Jan, Christopher): More efficient check

  hkvVec3 vCorners[8];
  rhs.getCorners (vCorners);

  expandToInclude (vCorners, 8);
}

HKV_FORCE_INLINE void hkvBoundingSphere::transformFromOrigin (const hkvMat4& mTransform)
{
  m_vCenter = mTransform.transformPosition (m_vCenter);

  const hkvVec3 s = mTransform.getScalingFactors ();
  const float fMaxScaling = hkvMath::Max (s.x, s.y, s.z);

  m_fRadius *= fMaxScaling;
}

HKV_FORCE_INLINE void hkvBoundingSphere::transformFromCenter (const hkvMat4& mTransform)
{
  // when transforming from the sphere's center, the center point will only be translated
  // since rotating and scaling the center will have no effect
  // The radius can also not be rotated or translated, so it will only be scaled

  m_vCenter += mTransform.getTranslation ();

  const hkvVec3 s = mTransform.getScalingFactors ();
  const float fMaxScaling = hkvMath::Max (s.x, s.y, s.z);

  m_fRadius *= fMaxScaling;
}

HKV_FORCE_INLINE void hkvBoundingSphere::translate (const hkvVec3& vTranslation)
{
  m_vCenter += vTranslation;
}

HKV_FORCE_INLINE void hkvBoundingSphere::scaleFromCenter (float xyz)
{
  VASSERT_MSG (xyz >= 0, "Scale may not be negative.");

  m_fRadius *= xyz;
}

HKV_FORCE_INLINE void hkvBoundingSphere::scaleFromOrigin (float xyz)
{
  VASSERT_MSG (xyz >= 0, "Scale may not be negative.");

  m_vCenter *= xyz;
  m_fRadius *= xyz;
}

HKV_FORCE_INLINE void hkvBoundingSphere::scaleFromOrigin (const hkvVec3& vScaleXYZ)
{
  VASSERT_MSG ((vScaleXYZ.x >= 0) && (vScaleXYZ.y >= 0) && (vScaleXYZ.z >= 0), "Scale may not be negative.");

  m_vCenter = m_vCenter.compMul (vScaleXYZ);
  m_fRadius *= hkvMath::Max (vScaleXYZ.x, vScaleXYZ.y, vScaleXYZ.z);
}

HKV_FORCE_INLINE void hkvBoundingSphere::addRadius (float fRadiusDifference)
{
  m_fRadius += fRadiusDifference;
}

HKV_FORCE_INLINE bool hkvBoundingSphere::isIdentical (const hkvBoundingSphere& rhs) const
{
  return ((m_vCenter.isIdentical (rhs.m_vCenter)) && (m_fRadius == rhs.m_fRadius));
}

HKV_FORCE_INLINE bool hkvBoundingSphere::isEqual (const hkvBoundingSphere& rhs, float fEpsilon) const
{
  return ((m_vCenter.isEqual (rhs.m_vCenter, fEpsilon)) && (hkvMath::isFloatEqual (m_fRadius, rhs.m_fRadius, fEpsilon)));
}

HKV_FORCE_INLINE const hkvAlignedBBox hkvBoundingSphere::getAlignedBoundingBox () const
{
  const hkvVec3 diff (m_fRadius);

  return hkvAlignedBBox (m_vCenter - diff, m_vCenter + diff);
}

HKV_FORCE_INLINE bool hkvBoundingSphere::contains (const hkvVec3& v) const
{
  return (m_vCenter.getDistanceToSquared (v) <= m_fRadius * m_fRadius);
}


HKV_FORCE_INLINE bool hkvBoundingSphere::contains (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride /* = sizeof (hkvVec3) */) const
{
  VASSERT (pPoints != NULL);
  VASSERT (uiNumPoints > 0);
  VASSERT (uiStride >= sizeof (hkvVec3));

  const float fRadiusSQR = m_fRadius * m_fRadius;

  hkvVec3* HKV_RESTRICT pPoint = (hkvVec3*) pPoints;

  for (hkUint32 i = 0; i < uiNumPoints; ++i)
  {
    if (m_vCenter.getDistanceToSquared (*pPoint) > fRadiusSQR)
      return false;

    pPoint = hkvAddByteOffset (pPoint, uiStride);
  }

  return true;
}

HKV_FORCE_INLINE bool hkvBoundingSphere::contains (const hkvAlignedBBox& rhs) const
{
  // TODO (Jan, Christopher): More efficient check

  hkvVec3 vCorners[8];
  rhs.getCorners (vCorners);

  return contains (vCorners, 8);
}

HKV_FORCE_INLINE bool hkvBoundingSphere::contains (const hkvBoundingSphere& rhs) const
{
  return (m_vCenter.getDistanceTo (rhs.m_vCenter) + rhs.m_fRadius <= m_fRadius);
}

HKV_FORCE_INLINE bool hkvBoundingSphere::overlaps (const hkvAlignedBBox& rhs) const
{
  return rhs.overlaps (*this);
}

HKV_FORCE_INLINE bool hkvBoundingSphere::overlaps (const hkvBoundingSphere& rhs) const
{
  const float fRadiiSQR = hkvMath::square (m_fRadius + rhs.m_fRadius);

  return (m_vCenter.getDistanceToSquared (rhs.m_vCenter) < fRadiiSQR);
}

HKV_FORCE_INLINE float hkvBoundingSphere::getDistanceTo (const hkvVec3& v) const
{
  // This will return zero for points inside the sphere

  return hkvMath::Max (0.0f, m_vCenter.getDistanceTo (v) - m_fRadius);
}

HKV_FORCE_INLINE float hkvBoundingSphere::getDistanceTo (const hkvAlignedBBox& rhs) const
{
  // This will return zero for overlapping objects

  return rhs.getDistanceTo (*this);
}

HKV_FORCE_INLINE float hkvBoundingSphere::getDistanceTo (const hkvBoundingSphere& rhs) const
{
  // This will return zero for overlapping objects

   const float fRadii = m_fRadius + rhs.m_fRadius;
   const float fRadiiSQR = hkvMath::square (fRadii);

   const float fDistSQR = m_vCenter.getDistanceToSquared (rhs.m_vCenter);

   if (fDistSQR <= fRadiiSQR)
     return 0.0f;

   return (hkvMath::sqrt (fDistSQR) - fRadii);
}

HKV_FORCE_INLINE bool hkvBoundingSphere::getRayIntersection (const hkvVec3& vRayStartPos, const hkvVec3& vRayDirNormalized, float* HKV_RESTRICT out_fIntersectionTime /* = NULL */, hkvVec3* HKV_RESTRICT out_IntersectionPoint /* = NULL */) const
{
  HKVMATH_ASSERT (vRayDirNormalized.isNormalized (), "vRayDirNormalized must be normalized.");

  const hkvVec3 vPos = vRayStartPos - m_vCenter;

  float b = -vPos.dot (vRayDirNormalized);
  float det = hkvMath::square (b) - vPos.getLengthSquared () + hkvMath::square (m_fRadius);
  
  if (det < 0)
    return false;
  
  det = hkvMath::sqrt (det);

  float i1 = b - det;
  float i2 = b + det;

  if (i2 < 0.0f) 
    return false;

  if (i1 < 0.0f)
    i1 = i2;

  if (out_fIntersectionTime)
    *out_fIntersectionTime = i1;
  if (out_IntersectionPoint)
    *out_IntersectionPoint = vRayStartPos + vRayDirNormalized * i1;

  return true;
}

HKV_FORCE_INLINE bool hkvBoundingSphere::getLineSegmentIntersection (const hkvVec3& vLineStartPos, const hkvVec3& vLineEndPos, float* HKV_RESTRICT out_fHitFraction /* = NULL */, hkvVec3* HKV_RESTRICT out_IntersectionPoint /* = NULL */) const
{
  float fIntersection = 0.0f;

  const hkvVec3 vDir = vLineEndPos - vLineStartPos;
  hkvVec3 vDirNorm = vDir;
  const float fLen = vDirNorm.getLengthAndNormalize ();

  if (!getRayIntersection (vLineStartPos, vDirNorm, &fIntersection))
    return false;

  if (fIntersection > fLen)
    return false;

  if (out_fHitFraction)
    *out_fHitFraction = fIntersection / fLen;

  if (out_IntersectionPoint)
    *out_IntersectionPoint = vLineStartPos + vDirNorm * fIntersection;

  return true;
}

HKV_FORCE_INLINE hkvVec3 hkvBoundingSphere::getClampedPosition (const hkvVec3& vPoint) const
{
  const hkvVec3 vDir = vPoint - m_vCenter;
  const float fDistSQR = vDir.getLengthSquared ();

  // point is inside the sphere
  if (fDistSQR <= hkvMath::square (m_fRadius))
    return vPoint;

  // project the point onto the sphere, by moving it towards the sphere by its distance

  const float fDist = hkvMath::sqrt (fDistSQR);
  const hkvVec3 vDirNorm = vDir / fDist;

  return (m_vCenter + m_fRadius * vDirNorm);
}

HKV_FORCE_INLINE bool operator== (const hkvBoundingSphere& lhs, const hkvBoundingSphere& rhs)
{
  return (lhs.isIdentical (rhs));
}

HKV_FORCE_INLINE bool operator!= (const hkvBoundingSphere& lhs, const hkvBoundingSphere& rhs)
{
  return (!lhs.isIdentical (rhs));
}

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
