/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVQUATERNION_INL
#define VBASE_HKVMATH_HKVQUATERNION_INL

#include <Vision/Runtime/Base/Math/Helper/hkvEulerUtil.h>


HKV_FORCE_INLINE const hkvQuat hkvQuat::IdentityQuaternion () 
{ 
  hkvQuat id (hkvNoInitialization); 
  id.setIdentity (); 
  return id; 
}

HKV_FORCE_INLINE void hkvQuat::setIdentity ()
{
  x = 0.0f;
  y = 0.0f;
  z = 0.0f;
  w = 1.0f;
}

HKV_FORCE_INLINE void hkvQuat::normalize ()
{
  const float fLengthSQR = getLengthSquared ();
  HKVMATH_ASSERT (!hkvMath::isZero (fLengthSQR), "The length of the quaternion must not be zero.");

  const float fLengthInv = hkvMath::sqrtInverse (fLengthSQR);

  x *= fLengthInv;
  y *= fLengthInv;
  z *= fLengthInv;
  w *= fLengthInv;
}

HKV_FORCE_INLINE bool hkvQuat::isNormalized (float fEpsilon) const
{
  return hkvMath::isFloatEqual (getLengthSquared (), 1.0f, fEpsilon);
}

HKV_FORCE_INLINE void hkvQuat::setFromMat3 (const hkvMat3& m)
{
  const float trace = m.m_Column[0][0] + m.m_Column[1][1] + m.m_Column[2][2];
  const float half = 0.5f;

  float v[4];

  if (trace > 0.0f)
  {
    float s = hkvMath::sqrt (trace + 1.0f);
    float t = half / s;

    v[0] = (m.m_Column[1][2] - m.m_Column[2][1]) * t;
    v[1] = (m.m_Column[2][0] - m.m_Column[0][2]) * t;
    v[2] = (m.m_Column[0][1] - m.m_Column[1][0]) * t;

    v[3] = half * s;
  }
  else
  {
    const hkInt32 next[] = {1,2,0};
    hkInt32 i = 0;

    if (m.m_Column[1][1] > m.m_Column[0][0]) 
      i = 1;

    if (m.m_Column[2][2] > m.m_Column[i][i]) 
      i = 2;

    hkInt32 j = next[i];
    hkInt32 k = next[j];

    float s = hkvMath::sqrt (m.m_Column[i][i] - (m.m_Column[j][j] + m.m_Column [k][k]) + 1.0f);
    float t = half / s;

    v[i] = half * s;
    v[3] = (m.m_Column[j][k] - m.m_Column[k][j]) * t;
    v[j] = (m.m_Column[i][j] + m.m_Column[j][i]) * t;
    v[k] = (m.m_Column[i][k] + m.m_Column[k][i]) * t;
  }

  x = v[0];
  y = v[1];
  z = v[2];
  w = v[3];
}

HKV_FORCE_INLINE void hkvQuat::invert ()
{
  w = -w;
}

HKV_FORCE_INLINE const hkvQuat hkvQuat::operator- () const
{
  hkvQuat r;
  r.x = x;
  r.y = y;
  r.z = z;
  r.w = -w;

  return r;
}

HKV_FORCE_INLINE float hkvQuat::getLength () const
{
  return hkvMath::sqrt (getLengthSquared ());
}

HKV_FORCE_INLINE float hkvQuat::getLengthSquared () const
{
  return this->dot (*this);
}

HKV_FORCE_INLINE float hkvQuat::dot (const hkvQuat& rhs) const
{
  return (x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w);
}

HKV_FORCE_INLINE bool hkvQuat::isIdentical (const hkvQuat& rhs) const
{
  const hkvMat3 ml = getAsMat3 ();
  const hkvMat3 mr = rhs.getAsMat3 ();

  return ml.isIdentical (mr);

  //return ((hkvMath::isFloatEqual (x, rhs.x, fEpsilon) && hkvMath::isFloatEqual (y, rhs.y, fEpsilon) && hkvMath::isFloatEqual (z, rhs.z, fEpsilon) && hkvMath::isFloatEqual (w, rhs.w, fEpsilon)) ||
  //        (hkvMath::isFloatEqual (x,-rhs.x, fEpsilon) && hkvMath::isFloatEqual (y,-rhs.y, fEpsilon) && hkvMath::isFloatEqual (z,-rhs.z, fEpsilon) && hkvMath::isFloatEqual (w,-rhs.w, fEpsilon)));
}

HKV_FORCE_INLINE bool hkvQuat::isEqual (const hkvQuat& rhs, float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  const hkvMat3 ml = getAsMat3 ();
  const hkvMat3 mr = rhs.getAsMat3 ();

  return ml.isEqual (mr, fEpsilon);

  //return ((hkvMath::isFloatEqual (x, rhs.x, fEpsilon) && hkvMath::isFloatEqual (y, rhs.y, fEpsilon) && hkvMath::isFloatEqual (z, rhs.z, fEpsilon) && hkvMath::isFloatEqual (w, rhs.w, fEpsilon)) ||
  //        (hkvMath::isFloatEqual (x,-rhs.x, fEpsilon) && hkvMath::isFloatEqual (y,-rhs.y, fEpsilon) && hkvMath::isFloatEqual (z,-rhs.z, fEpsilon) && hkvMath::isFloatEqual (w,-rhs.w, fEpsilon)));
}

HKV_FORCE_INLINE void hkvQuat::setFromAxisAndAngle (const hkvVec3& vAxis, float fAngleDeg)
{
  HKVMATH_ASSERT (vAxis.isNormalized (), "The rotation axis must be normalized.");

  const float fHalfAngleRad = 0.5f * hkvMath::Deg2Rad (fAngleDeg);

  float sinHalf = hkvMath::sinRad (fHalfAngleRad);

  x = sinHalf * vAxis.x;
  y = sinHalf * vAxis.y;
  z = sinHalf * vAxis.z;
  w = hkvMath::cosRad (fHalfAngleRad);
}

HKV_FORCE_INLINE void hkvQuat::setFromEulerAngles (float fRollDeg, float fPitchDeg, float fYawDeg)
{
  *this = hkvEulerUtil::ConvertEulerToQuaternion_Deg (fRollDeg, fPitchDeg, fYawDeg, hkvEulerMode::VisionDefault);
}

HKV_FORCE_INLINE void hkvQuat::getAsEulerAngles (float& out_fRollDeg, float& out_fPitchDeg, float& out_fYawDeg)
{
  hkvEulerUtil::ConvertQuaternionToEuler_Deg (*this, out_fRollDeg, out_fPitchDeg, out_fYawDeg, hkvEulerMode::VisionDefault);
}

HKV_FORCE_INLINE void hkvQuat::getAxisAndAngle (hkvVec3& out_vAxis, float& out_fAngleDeg) const
{
  // axis
  {
    if (x * x + y * y + z * z <= HKVMATH_EPSILON)
    {
      out_vAxis.set (1, 0, 0);
      out_fAngleDeg = 0.0f;
      return;
    }

    hkvVec3 vAxisTemp (x, y, z);
    vAxisTemp.normalize ();

    if (w < 0)
      out_vAxis = -vAxisTemp;
    else
      out_vAxis = vAxisTemp;
  }

  // angle
  {
    const float absangle = hkvMath::Abs (w);
    const float angle = hkvMath::acosRad (absangle);
    out_fAngleDeg = hkvMath::Rad2Deg (angle * 2.0f);
  }
}

HKV_FORCE_INLINE const hkvQuat hkvQuat::multiply (const hkvQuat& rhs) const
{
  const hkvVec3 lhs_img (x, y, z);
  const hkvVec3 rhs_img (rhs.x, rhs.y, rhs.z);
  const float lhs_real = w;
  const float rhs_real = rhs.w;

  hkvVec3 res;
  res = lhs_img.cross (rhs_img);
  res += lhs_real * rhs_img;
  res += rhs_real * lhs_img;

  const float res_w = (lhs_real * rhs_real) - lhs_img.dot (rhs_img);

  hkvQuat qr;
  qr.x = res.x;
  qr.y = res.y;
  qr.z = res.z;
  qr.w = res_w;

  return qr;
}

HKV_FORCE_INLINE const hkvQuat hkvQuat::multiplyReverse (const hkvQuat& rhs) const
{
  return rhs.multiply (*this);
}

HKV_FORCE_INLINE const hkvVec3 hkvQuat::transform (const hkvVec3& rhs) const
{
  const hkvVec3 QuatImg (x, y, z);

  float qreal = w;
  float q2minus1 = qreal*qreal - 0.5f;

  hkvVec3 ret = rhs * q2minus1;

  const float imagDotDir = QuatImg.dot (rhs);

  ret += QuatImg * imagDotDir; 

  hkvVec3 imagCrossDir = QuatImg.cross (rhs);
  ret += imagCrossDir * qreal;

  return (ret + ret);
}

HKV_FORCE_INLINE const hkvVec3 hkvQuat::transformReverse (const hkvVec3& rhs) const
{
  const hkvVec3 QuatImg (x, y, z);

  float qreal = -w;
  float q2minus1 = qreal*qreal - 0.5f;

  hkvVec3 ret = rhs * q2minus1;

  const float imagDotDir = QuatImg.dot (rhs);

  ret += QuatImg * imagDotDir; 

  hkvVec3 imagCrossDir = QuatImg.cross (rhs);
  ret += imagCrossDir * qreal;

  return (ret + ret);
}

HKV_FORCE_INLINE const hkvVec3 operator* (const hkvQuat& lhs, const hkvVec3& rhs)
{
  return lhs.transform (rhs);
}

#ifdef HKVMATH_ENABLE_NEW_OPERATORS

  HKV_FORCE_INLINE const hkvQuat operator* (const hkvQuat& lhs, const hkvQuat& rhs)
  {
    return lhs.multiply (rhs);
  }

#endif

HKV_FORCE_INLINE bool operator== (const hkvQuat& lhs, const hkvQuat& rhs)
{
  return lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE bool operator!= (const hkvQuat& lhs, const hkvQuat& rhs)
{
  return !lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE const hkvMat3 hkvQuat::getAsMat3 () const
{
  const float xx =  x * x * 2.0f;
  const float yy =  y * y * 2.0f;
  const float zz =  z * z * 2.0f;
  const float xy =  x * y * 2.0f;
  const float xz =  x * z * 2.0f;
  const float yz =  y * z * 2.0f;
  const float wx =  w * x * 2.0f;
  const float wy =  w * y * 2.0f;
  const float wz =  w * z * 2.0f;

  hkvMat3 res;

  // x axis
  res.m_Column[0][0] = 1.0f - (yy + zz);
  res.m_Column[1][0] = xy - wz;
  res.m_Column[2][0] = xz + wy;

  // y axis
  res.m_Column[0][1] = xy + wz;
  res.m_Column[1][1] = 1.0f - (xx + zz);
  res.m_Column[2][1] = yz - wx;

  // z axis
  res.m_Column[0][2] = xz - wy;
  res.m_Column[1][2] = yz + wx;
  res.m_Column[2][2] = 1.0f - (xx + yy);

  return res;
}

HKV_FORCE_INLINE const hkvMat4 hkvQuat::getAsMat4 () const
{
  return hkvMat4 (getAsMat3 (), hkvVec3::ZeroVector ());
}

HKV_FORCE_INLINE bool hkvQuat::isIdentity (float fEpsilon) const
{
  return hkvMath::isFloatEqual (x, 0, fEpsilon) && 
         hkvMath::isFloatEqual (y, 0, fEpsilon) &&
         hkvMath::isFloatEqual (z, 0, fEpsilon) &&
         (hkvMath::isFloatEqual (w, 1, fEpsilon) || hkvMath::isFloatEqual (w, -1, fEpsilon));
}

HKV_FORCE_INLINE void hkvQuat::flipSigns ()
{
  x = -x;
  y = -y;
  z = -z;
  w = -w;
}

HKV_FORCE_INLINE bool hkvQuat::isValid () const
{
  return (hkvMath::isFiniteNumber (x) && 
          hkvMath::isFiniteNumber (y) &&
          hkvMath::isFiniteNumber (z) &&
          hkvMath::isFiniteNumber (w));
}


HKV_FORCE_INLINE void hkvMat4::setFromQuaternion (const hkvQuat& q)
{
  *this = q.getAsMat4 ();
}

HKV_FORCE_INLINE hkvQuat hkvMat4::getAsQuaternion () const
{
  return getRotationalPart ().getAsQuaternion ();
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
