/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVPLANE_INL
#define VBASE_HKVMATH_HKVPLANE_INL

#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>
#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>

HKV_FORCE_INLINE hkvPlane::hkvPlane (const hkvPlane& rhs)
{
  *this = rhs;
}

HKV_FORCE_INLINE void hkvPlane::operator= (const hkvPlane& plane)
{
  m_vNormal  = plane.m_vNormal;
  m_fNegDist = plane.m_fNegDist;
}

HKV_FORCE_INLINE void hkvPlane::setFromPointAndNormal (const hkvVec3& vPointOnPlane, const hkvVec3& vNormal)
{
  HKVMATH_ASSERT (vNormal.isNormalized (HKVMATH_DEFAULT_EPSILON), "vNormal must be normalized.");

  m_vNormal  =  vNormal;
  m_fNegDist = -vNormal.dot (vPointOnPlane);
}

HKV_FORCE_INLINE hkvResult hkvPlane::setFromPoints (const hkvVec3* pVertices, hkvTriangleOrientation::Enum ori, hkUint32 uiStride /* = sizeof (hkvVec3) */)
{
  VASSERT (pVertices != NULL);
  VASSERT (uiStride >= sizeof (hkvVec3));

  hkvVec3* p = (hkvVec3*) pVertices;

  return setFromPoints (pVertices[0], *hkvAddByteOffset (p, uiStride), *hkvAddByteOffset (p, uiStride * 2), ori);
}

HKV_FORCE_INLINE hkvResult hkvPlane::setFromPoints (const hkvVec3& v0, const hkvVec3& v1, const hkvVec3& v2, hkvTriangleOrientation::Enum ori)
{
  hkvVec3 v = v1 - v0;
  hkvVec3 w = v2 - v0;

  hkvVec3 vNormal = v.cross (w);
  const hkvResult res = vNormal.normalizeIfNotZero ();

  m_vNormal  =  vNormal;
  m_fNegDist = -vNormal.dot (v0);

  if (ori == hkvTriangleOrientation::ClockWise)
    flip ();

  return res;
}

HKV_FORCE_INLINE hkvResult hkvPlane::setFromDirections (const hkvVec3& vTangent1, const hkvVec3& vTangent2, const hkvVec3& vPointOnPlane)
{
  hkvVec3 vNormal = vTangent1.cross (vTangent2);
  const hkvResult res = vNormal.normalizeIfNotZero ();

  m_vNormal  =  vNormal;
  m_fNegDist = -vNormal.dot (vPointOnPlane);

  return res;
}

HKV_FORCE_INLINE void hkvPlane::getPoints (hkvVec3& out_v0, hkvVec3& out_v1, hkvVec3& out_v2) const
{
  out_v0 = m_vNormal * -m_fNegDist;

  const hkvVec3 vTangent   = m_vNormal.getOrthogonalVector ();
  const hkvVec3 vBiTangent = m_vNormal.cross (vTangent);

  out_v1 = out_v0 + vTangent;
  out_v2 = out_v0 + vBiTangent;
}

HKV_FORCE_INLINE bool hkvPlane::getRayIntersectionBiDirectional (const hkvVec3& vRayStartPos, const hkvVec3& vRayDir, float* out_fIntersectionTime, hkvVec3* out_IntersectionPoint) const
{
  const float fPlaneSide = getDistanceTo (vRayStartPos);
  const float fCosAlpha = m_vNormal.dot (vRayDir);

  if (fCosAlpha == 0.0f)  // ray is orthogonal to plane
    return false;

  const float fTime = -fPlaneSide / fCosAlpha;

  if (out_fIntersectionTime)
    *out_fIntersectionTime = fTime;

  if (out_IntersectionPoint)
    *out_IntersectionPoint = vRayStartPos + fTime * vRayDir;

  return (true); 
}

HKV_FORCE_INLINE bool hkvPlane::getRayIntersection (const hkvVec3& vRayStartPos, const hkvVec3& vRayDir, float* out_fIntersectionTime, hkvVec3* out_IntersectionPoint) const
{
  const float fPlaneSide = getDistanceTo (vRayStartPos);
  const float fCosAlpha = m_vNormal.dot (vRayDir);

  if ((fCosAlpha == 0.0f) ||	// ray is orthogonal to plane
    (hkvMath::sign (fPlaneSide) == hkvMath::sign (fCosAlpha))) // ray points away from the plane
    return false;

  const float fTime = -fPlaneSide / fCosAlpha;

  if (out_fIntersectionTime)
    *out_fIntersectionTime = fTime;

  if (out_IntersectionPoint)
    *out_IntersectionPoint = vRayStartPos + fTime * vRayDir;

  return (true); 
}

HKV_FORCE_INLINE bool hkvPlane::getLineSegmentIntersection (const hkvVec3& vLineStartPos, const hkvVec3& vLineEndPos, float* out_fHitFraction, hkvVec3* out_IntersectionPoint) const
{
  float fTime = 0.0f;

  if (!getRayIntersection (vLineStartPos, vLineEndPos - vLineStartPos, &fTime, out_IntersectionPoint))
    return false;

  if (out_fHitFraction)
    *out_fHitFraction = fTime;

  return (fTime <= 1.0f);
}

HKV_FORCE_INLINE hkvPlanePosition::Enum hkvPlane::getPointPosition (const hkvVec3& vPoint) const
{
  const float fDistance = getDistanceTo (vPoint);

  if (fDistance > 0.0f)
    return hkvPlanePosition::Front;
  if (fDistance < 0.0f)
    return hkvPlanePosition::Back;

  return hkvPlanePosition::Coplanar;
}

HKV_FORCE_INLINE hkvPlanePosition::Enum hkvPlane::getPointPosition (const hkvVec3& vPoint, float fPlaneHalfThickness) const
{
  VASSERT (fPlaneHalfThickness >= 0.0f);

  const float fDistance = getDistanceTo (vPoint);

  if (fDistance > fPlaneHalfThickness)
    return hkvPlanePosition::Front;
  if (fDistance < -fPlaneHalfThickness)
    return hkvPlanePosition::Back;

  return hkvPlanePosition::Coplanar;
}

HKV_FORCE_INLINE hkvPlanePosition::Enum hkvPlane::getObjectPosition (const hkvVec3* pPoints, hkUint32 uiNumPoints, hkUint32 uiStride /* = sizeof (hkvVec3) */) const
{
  VASSERT (pPoints != NULL);
  VASSERT (uiNumPoints >= 1);
  VASSERT (uiStride >= sizeof(hkvVec3));

  bool bFront = false;
  bool bBack = false;

  hkvVec3* pCurPoint = (hkvVec3*) pPoints;

  for (hkUint32 i = 0; i < uiNumPoints; ++i)
  {
    switch (getPointPosition (*pCurPoint))
    {
    case hkvPlanePosition::Front:
      {
        if (bBack)
          return hkvPlanePosition::Spanning;
        bFront = true;
      }
      break;
    case hkvPlanePosition::Back:
      {
        if (bFront)
          return hkvPlanePosition::Spanning;
        bBack = true;
      }
      break;
    case hkvPlanePosition::Coplanar: // neither front, nor back
      break;
    case hkvPlanePosition::Spanning: // will not happen
      return hkvPlanePosition::Spanning;
    }

    pCurPoint = hkvAddByteOffset (pCurPoint, uiStride);
  }

  if (bFront)
    return hkvPlanePosition::Front;
  if (bBack)
    return hkvPlanePosition::Back;

  return hkvPlanePosition::Coplanar;
}

HKV_FORCE_INLINE hkvPlanePosition::Enum hkvPlane::getObjectPosition (const hkvVec3* pPoints, hkUint32 uiNumPoints, float fPlaneHalfThickness, hkUint32 uiStride /* = sizeof (hkvVec3) */) const
{
  VASSERT (pPoints != NULL);
  VASSERT (uiNumPoints >= 1);
  VASSERT (uiStride >= sizeof(hkvVec3));
  VASSERT (fPlaneHalfThickness >= 0.0f);

  bool bFront = false;
  bool bBack = false;

  hkvVec3* pCurPoint = (hkvVec3*) pPoints;

  for (hkUint32 i = 0; i < uiNumPoints; ++i)
  {
    switch (getPointPosition (*pCurPoint, fPlaneHalfThickness))
    {
    case hkvPlanePosition::Front:
      {
        if (bBack)
          return hkvPlanePosition::Spanning;
        bFront = true;
      }
      break;
    case hkvPlanePosition::Back:
      {
        if (bFront)
          return hkvPlanePosition::Spanning;
        bBack = true;
      }
      break;
    case hkvPlanePosition::Coplanar: // neither front, nor back
      break;
    case hkvPlanePosition::Spanning: // will not happen
      return hkvPlanePosition::Spanning;
    }

    pCurPoint = hkvAddByteOffset (pCurPoint, uiStride);
  }

  if (bFront)
    return hkvPlanePosition::Front;
  if (bBack)
    return hkvPlanePosition::Back;

  return hkvPlanePosition::Coplanar;
}


HKV_FORCE_INLINE hkvPlanePosition::Enum hkvPlane::getObjectPosition (const hkvAlignedBBox& aabb) const
{
  hkvVec3 v[8];
  aabb.getCorners (v);

  return getObjectPosition (v, 8);
}

HKV_FORCE_INLINE hkvPlanePosition::Enum hkvPlane::getObjectPosition (const hkvBoundingSphere& sphere) const
{
  const float fDist = getDistanceTo (sphere.m_vCenter);

  if (fDist >=  sphere.m_fRadius)
    return hkvPlanePosition::Front;
  if (fDist <= -sphere.m_fRadius)
    return hkvPlanePosition::Back;

  return hkvPlanePosition::Spanning;
}

HKV_FORCE_INLINE hkvResult hkvPlane::get3PlaneIntersectionPoint (const hkvPlane& p0, const hkvPlane& p1, const hkvPlane& p2, hkvVec3& out_Result)
{
  const hkvVec3 n1 (p0.m_vNormal);
  const hkvVec3 n2 (p1.m_vNormal);
  const hkvVec3 n3 (p2.m_vNormal);

  const float det = n1.dot (n2.cross (n3));

  if (hkvMath::isZero (det))
    return HKV_FAILURE;

  out_Result = (-p0.m_fNegDist * n2.cross (n3) + 
                -p1.m_fNegDist * n3.cross (n1) + 
                -p2.m_fNegDist * n1.cross (n2)) / det;

  return HKV_SUCCESS;
}

HKV_FORCE_INLINE void hkvPlane::flip ()
{
  m_vNormal.negate ();
  m_fNegDist = -m_fNegDist;
}

HKV_FORCE_INLINE bool hkvPlane::flipIfNecessary (const hkvVec3& vPointOnPositiveSide, bool bPointShouldBeOnPositiveSide)
{
  if (bPointShouldBeOnPositiveSide)
  {
    // if the point is on the back then flip the plane
    // if it is exactly on the plane, don't do anything
    if (getPointPosition (vPointOnPositiveSide) == hkvPlanePosition::Back)
    {
      flip ();
      return true;
    }
  }
  else
  {
    // if the point is on the front then flip the plane
    // if it is exactly on the plane, don't do anything
    if (getPointPosition (vPointOnPositiveSide) == hkvPlanePosition::Front)
    {
      flip ();
      return true;
    }
  }

  return false;
}

HKV_FORCE_INLINE void hkvPlane::setInvalid ()
{
  m_vNormal.setZero ();
  m_fNegDist = 0.0f;
}

HKV_FORCE_INLINE bool hkvPlane::isValid () const
{
  return (m_vNormal.isValid () && hkvMath::isFiniteNumber (m_fNegDist) && (m_vNormal.isNormalized (HKVMATH_DEFAULT_EPSILON)));
}

HKV_FORCE_INLINE float hkvPlane::getDistanceTo (const hkvVec3& vPoint) const
{
  return m_vNormal.dot (vPoint) + m_fNegDist;
}

HKV_FORCE_INLINE float hkvPlane::getMinimumDistanceTo (const hkvVec3* pPoints, hkUint32 uiNumPoints, hkUint32 uiStride /* = sizeof (hkvVec3) */) const
{
  VASSERT (pPoints != NULL);
  VASSERT (uiStride >= sizeof (hkvVec3));
  VASSERT (uiNumPoints >= 1);

  float fMinDist = HKVMATH_FLOAT_MAX_POS;

  hkvVec3* pCurPoint = (hkvVec3*) pPoints;

  for (hkUint32 i = 0; i < uiNumPoints; ++i)
  {
    fMinDist = hkvMath::Min (m_vNormal.dot (*pCurPoint), fMinDist);

    pCurPoint = hkvAddByteOffset (pCurPoint, uiStride);
  }

  return fMinDist + m_fNegDist;
}

HKV_FORCE_INLINE void hkvPlane::getMinMaxDistanceTo (float &out_fMin, float &out_fMax, const hkvVec3* pPoints, hkUint32 uiNumPoints, hkUint32 uiStride /* = sizeof (hkvVec3) */) const
{
  VASSERT (pPoints != NULL);
  VASSERT (uiNumPoints >= 1);
  VASSERT (uiStride >= sizeof (hkvVec3));

  out_fMin = HKVMATH_FLOAT_MAX_POS;
  out_fMax = HKVMATH_FLOAT_MAX_NEG;

  hkvVec3* pCurPoint = (hkvVec3*) pPoints;

  for (hkUint32 i = 0; i < uiNumPoints; ++i)
  {
    const float f = m_vNormal.dot (*pCurPoint);

    out_fMin = hkvMath::Min (f, out_fMin);
    out_fMax = hkvMath::Max (f, out_fMax);

    pCurPoint = hkvAddByteOffset (pCurPoint, uiStride);
  }

  out_fMin += m_fNegDist;
  out_fMax += m_fNegDist;
}

HKV_FORCE_INLINE void hkvPlane::transform (const hkvMat3& m)
{
  hkvVec3 vPtOnPlane = m.transformDirection (m_vNormal * -m_fNegDist);
  hkvVec3 vNewNormal = m.transformDirection (m_vNormal).getNormalized ();

  setFromPointAndNormal (vPtOnPlane, vNewNormal);
}

HKV_FORCE_INLINE void hkvPlane::transform (const hkvMat4& m)
{
  hkvVec3 vPtOnPlane = m.transformPosition  (m_vNormal * -m_fNegDist);
  hkvVec3 vNewNormal = m.transformDirection (m_vNormal).getNormalized ();

  setFromPointAndNormal (vPtOnPlane, vNewNormal);
}

HKV_FORCE_INLINE const hkvPlane hkvPlane::getTransformed (const hkvMat3& m) const
{
  hkvPlane temp = *this;
  temp.transform (m);
  return temp;
}

HKV_FORCE_INLINE const hkvPlane hkvPlane::getTransformed (const hkvMat4& m) const
{
  hkvPlane temp = *this;
  temp.transform (m);
  return temp;
}

HKV_FORCE_INLINE void hkvPlane::projectOntoPlane (hkvVec3& inout_vPoint) const
{
  const float fDist = getDistanceTo (inout_vPoint);
  inout_vPoint -= fDist * m_vNormal;
}

HKV_FORCE_INLINE void hkvPlane::projectOntoPlane (hkvVec3* inout_vPoints, hkUint32 uiNumPoints, hkUint32 uiStride /* = sizeof (hkvVec3) */) const
{
  const hkvVec3 vNormal = m_vNormal;

  hkvVec3* pCurPoint = inout_vPoints;

  for (hkUint32 i = 0; i < uiNumPoints; ++i)
  {
    const float fDist = getDistanceTo (*pCurPoint);
    *pCurPoint -= fDist * vNormal;

    pCurPoint = hkvAddByteOffset (pCurPoint, uiStride);
  }
}

HKV_FORCE_INLINE void hkvPlane::makeDirectionCoplanar (hkvVec3& inout_vDirection) const
{
  const hkvVec3 n = m_vNormal;
  
  inout_vDirection.makeOrthogonalTo (n);
}

HKV_FORCE_INLINE void hkvPlane::makeDirectionCoplanar (hkvVec3* inout_vDirections, hkUint32 uiNumPoints, hkUint32 uiStride /* = sizeof (hkvVec3) */) const
{
  const hkvVec3 n = m_vNormal;

  hkvVec3* pCurPoint = inout_vDirections;

  for (hkUint32 i = 0; i < uiNumPoints; ++i)
  {
    pCurPoint->makeOrthogonalTo (n);
    pCurPoint = hkvAddByteOffset (pCurPoint, uiStride);
  }
}

HKV_FORCE_INLINE bool hkvPlane::isIdentical (const hkvPlane& rhs) const
{
  return (m_vNormal.isIdentical (rhs.m_vNormal) && (m_fNegDist == rhs.m_fNegDist));
}

HKV_FORCE_INLINE bool hkvPlane::isEqual (const hkvPlane& rhs, float fEpsilon) const
{
  // should not use a negative epsilon
  VASSERT (fEpsilon >= 0.0f);

  return (m_vNormal.isEqual (rhs.m_vNormal, fEpsilon) && (hkvMath::isFloatEqual (m_fNegDist, rhs.m_fNegDist, fEpsilon)));
}

HKV_FORCE_INLINE bool operator== (const hkvPlane& p0, const hkvPlane& p1)
{
  return p0.isIdentical (p1);
}

HKV_FORCE_INLINE bool operator!= (const hkvPlane& p0, const hkvPlane& p1)
{
  return !p0.isIdentical (p1);
}

HKV_FORCE_INLINE hkUint32 hkvPlane::GetLeastSignificantComponent () const
{
  const float c0 = hkvMath::Abs (m_vNormal[0]);
  const float c1 = hkvMath::Abs (m_vNormal[1]);
  const float c2 = hkvMath::Abs (m_vNormal[2]);

  if (c0 > c1)
  {
    if (c0 > c2)
      return 0;

    return 2;
  }

  if (c1 > c2)
    return 1;

  return 2;
}

HKV_FORCE_INLINE const hkvVec3 hkvPlane::getMirrored (const hkvVec3& v) const
{
  return (v - 2.0f * getDistanceTo (v) * m_vNormal);
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
