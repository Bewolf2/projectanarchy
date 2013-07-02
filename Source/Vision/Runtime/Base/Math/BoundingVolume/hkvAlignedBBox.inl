/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVALIGNEDBOUNDINGBOX_INL
#define VBASE_HKVMATH_HKVALIGNEDBOUNDINGBOX_INL

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvBoundingSphere.h>

HKV_FORCE_INLINE hkvAlignedBBox::hkvAlignedBBox (const hkvVec3& vMin, const hkvVec3& vMax)
{
  setWithoutValidityCheck (vMin, vMax);
}

HKV_FORCE_INLINE void hkvAlignedBBox::setZero ()
{
  m_vMin.setZero ();
  m_vMax.setZero ();
}

HKV_FORCE_INLINE bool hkvAlignedBBox::isZero (float fEpsilon) const
{
  return (m_vMin.isZero (fEpsilon) && m_vMax.isZero (fEpsilon));
}

HKV_FORCE_INLINE void hkvAlignedBBox::setCenterAndSize (const hkvVec3& vCenter, const hkvVec3& vHalfSizeXYZ)
{
  VASSERT ((vHalfSizeXYZ.x >= 0) && (vHalfSizeXYZ.y >= 0) && (vHalfSizeXYZ.z >= 0));

  m_vMin = vCenter - vHalfSizeXYZ;
  m_vMax = vCenter + vHalfSizeXYZ;
}

HKV_FORCE_INLINE void hkvAlignedBBox::setWithoutValidityCheck (const hkvVec3& vMin, const hkvVec3& vMax)
{
  m_vMin = vMin;
  m_vMax = vMax;
}

HKV_FORCE_INLINE void hkvAlignedBBox::set (const hkvVec3& vMin, const hkvVec3& vMax)
{
  VASSERT ((vMin.x <= vMax.x) && (vMin.y <= vMax.y) && (vMin.z <= vMax.z));

  setWithoutValidityCheck (vMin, vMax);
}

HKV_FORCE_INLINE void hkvAlignedBBox::setInvalid ()
{
  m_vMin.set (HKVMATH_FLOAT_MAX_POS);
  m_vMax.set (HKVMATH_FLOAT_MAX_NEG);
}

HKV_FORCE_INLINE bool hkvAlignedBBox::isValid (void) const
{
  return (m_vMin.x <= m_vMax.x) &&
         (m_vMin.y <= m_vMax.y) && 
         (m_vMin.z <= m_vMax.z);
}

HKV_FORCE_INLINE void hkvAlignedBBox::expandToInclude (const hkvVec3& v)
{
  m_vMin.setMin (v);
  m_vMax.setMax (v);
}

HKV_FORCE_INLINE void hkvAlignedBBox::expandToInclude (const hkvAlignedBBox& cc)
{
  m_vMin.setMin (cc.m_vMin);
  m_vMax.setMax (cc.m_vMax);
}

HKV_FORCE_INLINE void hkvAlignedBBox::expandToInclude (const hkvBoundingSphere& cc)
{
  m_vMin.setMin (cc.m_vCenter - hkvVec3 (cc.m_fRadius));
  m_vMax.setMax (cc.m_vCenter + hkvVec3 (cc.m_fRadius));
}

HKV_FORCE_INLINE void hkvAlignedBBox::transformFromCenter (const hkvMat4& mTransform)
{
  const hkvVec3 vCenter = getCenter ();

  hkvVec3 vCorners[8];
  getCorners (vCorners);

  setInvalid ();

  for (hkUint32 i = 0; i < 8; ++i)
    expandToInclude (vCenter + mTransform.transformPosition (vCorners[i] - vCenter));
}

HKV_FORCE_INLINE void hkvAlignedBBox::transformFromOrigin (const hkvMat4& mTransform)
{
  hkvVec3 vCorners[8];
  getCorners (vCorners);

  setInvalid ();

  for (hkUint32 i = 0; i < 8; ++i)
    expandToInclude (mTransform.transformPosition (vCorners[i]));
}

HKV_FORCE_INLINE void hkvAlignedBBox::translate (const hkvVec3& vTranslation)
{
  m_vMin += vTranslation;
  m_vMax += vTranslation;
}

HKV_FORCE_INLINE void hkvAlignedBBox::scaleFromCenter (const hkvVec3& vScaleXYZ)
{
  const hkvVec3 c = getCenter ();

  hkvVec3 di = m_vMin - c;
  hkvVec3 da = m_vMax - c;

  di = di.compMul (vScaleXYZ);
  da = da.compMul (vScaleXYZ);

  m_vMin = c + di;
  m_vMax = c + da;
}

HKV_FORCE_INLINE void hkvAlignedBBox::scaleFromOrigin (const hkvVec3& vScaleXYZ)
{
  m_vMin = m_vMin.compMul (vScaleXYZ);
  m_vMax = m_vMax.compMul (vScaleXYZ);
}

HKV_FORCE_INLINE void hkvAlignedBBox::addBoundary (const hkvVec3& vChangeXYZ)
{
  m_vMin -= vChangeXYZ;
  m_vMax += vChangeXYZ;
}

HKV_FORCE_INLINE void hkvAlignedBBox::expandToCube ()
{
  hkvVec3 diff = m_vMax - m_vMin;

  diff.set (hkvMath::Max (diff.x, diff.y, diff.z) * 0.5f);

  const hkvVec3 c = getCenter ();

  m_vMin = c - diff;
  m_vMax = c + diff;
}

HKV_FORCE_INLINE bool hkvAlignedBBox::isIdentical (const hkvAlignedBBox& rhs) const
{
  return (m_vMin.isIdentical (rhs.m_vMin) && m_vMax.isIdentical (rhs.m_vMax));
}

HKV_FORCE_INLINE bool hkvAlignedBBox::isEqual (const hkvAlignedBBox& rhs, float fEpsilon) const
{
  return (m_vMin.isEqual (rhs.m_vMin, fEpsilon) && m_vMax.isEqual (rhs.m_vMax, fEpsilon));
}

HKV_FORCE_INLINE const hkvVec3 hkvAlignedBBox::getCenter () const
{
  hkvVec3 half_diff = (m_vMax - m_vMin) * 0.5f;

  return m_vMin + half_diff;
}

HKV_FORCE_INLINE float hkvAlignedBBox::getSizeX () const
{
  return (m_vMax.x - m_vMin.x);
}

HKV_FORCE_INLINE float hkvAlignedBBox::getSizeY () const
{
  return (m_vMax.y - m_vMin.y);
}

HKV_FORCE_INLINE float hkvAlignedBBox::getSizeZ () const
{
  return (m_vMax.z - m_vMin.z);
}

HKV_FORCE_INLINE void hkvAlignedBBox::getCorners (hkvVec3* HKV_RESTRICT out_pVertices) const
{
  VASSERT (out_pVertices != NULL);

  out_pVertices[0].set (m_vMin.x, m_vMin.y, m_vMin.z);
  out_pVertices[1].set (m_vMin.x, m_vMin.y, m_vMax.z);
  out_pVertices[2].set (m_vMin.x, m_vMax.y, m_vMin.z);
  out_pVertices[3].set (m_vMin.x, m_vMax.y, m_vMax.z);
  out_pVertices[4].set (m_vMax.x, m_vMin.y, m_vMin.z);
  out_pVertices[5].set (m_vMax.x, m_vMin.y, m_vMax.z);
  out_pVertices[6].set (m_vMax.x, m_vMax.y, m_vMin.z);
  out_pVertices[7].set (m_vMax.x, m_vMax.y, m_vMax.z);
}

HKV_FORCE_INLINE hkvVec3 hkvAlignedBBox::getCorner (bool bMaxX, bool bMaxY, bool bMaxZ) const
{
  return hkvVec3 (bMaxX ? m_vMax.x : m_vMin.x, 
                  bMaxY ? m_vMax.y : m_vMin.y, 
                  bMaxZ ? m_vMax.z : m_vMin.z);
}

HKV_FORCE_INLINE float hkvAlignedBBox::getMaxExtent (hkUint32* HKV_RESTRICT out_iAxis) const
{
  if (out_iAxis == NULL)
    return hkvMath::Max (getSizeX (), getSizeY (), getSizeZ ());

  const hkvVec3 e = m_vMax - m_vMin;

  if (e.x > e.y)
  {
    if (e.x > e.z)
    {
      *out_iAxis = 0;// X
      return e.x;
    }

    *out_iAxis = 2; // Z
    return e.z;
  }
  else
  {
    if (e.y > e.z)
    {
      *out_iAxis = 1;// Y
      return e.y;
    }

    *out_iAxis = 2; // Z
    return e.z;
  }
}

HKV_FORCE_INLINE float hkvAlignedBBox::getMinExtent (hkUint32* HKV_RESTRICT out_iAxis) const
{
  if (out_iAxis == NULL)
    return hkvMath::Min (getSizeX (), getSizeY (), getSizeZ ());

  const hkvVec3 e = m_vMax - m_vMin;

  if (e.x < e.y)
  {
    if (e.x < e.z)
    {
      *out_iAxis = 0;// X
      return e.x;
    }

    *out_iAxis = 2; // Z
    return e.z;
  }
  else
  {
    if (e.y < e.z)
    {
      *out_iAxis = 1;// Y
      return e.y;
    }

    *out_iAxis = 2; // Z
    return e.z;
  }
}

HKV_FORCE_INLINE const hkvBoundingSphere hkvAlignedBBox::getBoundingSphere () const
{
  return hkvBoundingSphere (getCenter (), (m_vMax - m_vMin).getLength () * 0.5f);
}

HKV_FORCE_INLINE bool hkvAlignedBBox::contains (const hkvVec3& v) const
{
  return ((v.x >= m_vMin.x) && (v.y >= m_vMin.y) && (v.z >= m_vMin.z) && 
          (v.x <= m_vMax.x) && (v.y <= m_vMax.y) && (v.z <= m_vMax.z));
}

HKV_FORCE_INLINE bool hkvAlignedBBox::contains (const hkvAlignedBBox& rhs) const
{
  return ((rhs.m_vMin.x >= m_vMin.x) && (rhs.m_vMin.y >= m_vMin.y) && (rhs.m_vMin.z >= m_vMin.z) && 
          (rhs.m_vMax.x <= m_vMax.x) && (rhs.m_vMax.y <= m_vMax.y) && (rhs.m_vMax.z <= m_vMax.z));
}

HKV_FORCE_INLINE bool hkvAlignedBBox::contains (const hkvBoundingSphere& rhs) const
{
  return ((rhs.m_vCenter.x - rhs.m_fRadius >= m_vMin.x) &&
          (rhs.m_vCenter.y - rhs.m_fRadius >= m_vMin.y) &&
          (rhs.m_vCenter.z - rhs.m_fRadius >= m_vMin.z) &&
          (rhs.m_vCenter.x + rhs.m_fRadius <= m_vMax.x) &&
          (rhs.m_vCenter.y + rhs.m_fRadius <= m_vMax.y) &&
          (rhs.m_vCenter.z + rhs.m_fRadius <= m_vMax.z));
}

HKV_FORCE_INLINE bool hkvAlignedBBox::overlaps (const hkvAlignedBBox& rhs) const
{
  if (m_vMin.x > rhs.m_vMax.x) return false;
  if (m_vMin.y > rhs.m_vMax.y) return false;
  if (m_vMin.z > rhs.m_vMax.z) return false;
  if (m_vMax.x < rhs.m_vMin.x) return false;
  if (m_vMax.y < rhs.m_vMin.y) return false;
  if (m_vMax.z < rhs.m_vMin.z) return false;

  return true;
}

HKV_FORCE_INLINE bool hkvAlignedBBox::overlaps (const hkvBoundingSphere& rhs) const
{
  return (getDistanceToSquared (rhs.m_vCenter) <= (rhs.m_fRadius * rhs.m_fRadius));
}

HKV_FORCE_INLINE float hkvAlignedBBox::getDistanceToSquared (const hkvVec3& v) const
{
  // This will return zero for points inside the box

  const hkvVec3 c  = getClampedPosition (v);
  return v.getDistanceToSquared (c);
}

HKV_FORCE_INLINE float hkvAlignedBBox::getDistanceToSquared (const hkvAlignedBBox& rhs) const
{
  // This will return zero for overlapping boxes

  float fSqrDist = 0.0f;

  for(hkUint32 i = 0;i < 3;i++)
  {
    if (rhs.m_vMax[i] < m_vMin[i])
    {
      float d = m_vMin[i] - rhs.m_vMax[i];
      fSqrDist += d * d;
    }
    else 
    if (rhs.m_vMin[i] > m_vMax[i])
    {
      float d = rhs.m_vMin[i] - m_vMax[i];
      fSqrDist += d * d;
    }
  }
  return fSqrDist;
}

HKV_FORCE_INLINE float hkvAlignedBBox::getDistanceTo (const hkvVec3& v) const
{
  // This will return zero for points inside the box

  hkvVec3 c = getClampedPosition (v);
  return v.getDistanceTo (c);
}

HKV_FORCE_INLINE float hkvAlignedBBox::getDistanceTo (const hkvAlignedBBox& rhs) const
{
  // This will return zero for overlapping boxes

  return hkvMath::sqrt (getDistanceToSquared (rhs));
}

HKV_FORCE_INLINE float hkvAlignedBBox::getDistanceTo (const hkvBoundingSphere& rhs) const
{
  // This will return zero for overlapping volumes

  hkvVec3 c = getClampedPosition (rhs.m_vCenter);

  return hkvMath::Max (0.0f, c.getDistanceTo (rhs.m_vCenter) - rhs.m_fRadius);
}

HKV_FORCE_INLINE bool hkvAlignedBBox::getLineSegmentIntersection (const hkvVec3& vLineStartPos, const hkvVec3& vLineEndPos, float* HKV_RESTRICT out_fHitFraction, hkvVec3* HKV_RESTRICT out_IntersectionPoint) const
{
  float t;

  if (!getRayIntersection (vLineStartPos, vLineEndPos - vLineStartPos, &t, out_IntersectionPoint))
    return false;

  if (t > 1.0f)
    return false;

  if (out_fHitFraction)
    *out_fHitFraction = t;

  return true;
}

HKV_FORCE_INLINE hkvVec3 hkvAlignedBBox::getClampedPosition (const hkvVec3& v) const
{
  hkvVec3 c (v);
  c.setMax (m_vMin);
  c.setMin (m_vMax);
  return c;
}

HKV_FORCE_INLINE bool operator== (const hkvAlignedBBox& lhs, const hkvAlignedBBox& rhs)
{
  return lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE bool operator!= (const hkvAlignedBBox& lhs, const hkvAlignedBBox& rhs)
{
  return !lhs.isIdentical (rhs);
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
