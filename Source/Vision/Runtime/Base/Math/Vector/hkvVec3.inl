/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVVEC3_INL
#define VBASE_HKVMATH_HKVVEC3_INL

#include <Vision/Runtime/Base/Math/Vector/hkvVec2.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>

// Helper macro to copy vector parameters into local variables
// This might look like stupid unnecessary code, but it helps the compiler to optimize the assembly code
#define HKVVEC3_COPYTEMPS(var) \
  const float var##_x = var.x; const float var##_y = var.y; const float var##_z = var.z;

HKV_FORCE_INLINE hkvVec3::hkvVec3 (float _x, float _y, float _z) : x (_x), y (_y), z (_z)
{
}

HKV_FORCE_INLINE hkvVec3::hkvVec3 (float xyz) : x (xyz), y (xyz), z (xyz)
{
}

HKV_FORCE_INLINE void hkvVec3::setZero ()
{
  x = 0.0f;
  y = 0.0f;
  z = 0.0f;
}

HKV_FORCE_INLINE void hkvVec3::set (float _x, float _y, float _z)
{
  x = _x;
  y = _y;
  z = _z;
}

HKV_FORCE_INLINE void hkvVec3::set (float xyz)
{
  x = xyz;
  y = xyz;
  z = xyz;
}

HKV_FORCE_INLINE float hkvVec3::operator[] (hkUint32 i) const
{
  VASSERT (i <= 2);

  return data[i];
}

HKV_FORCE_INLINE float& hkvVec3::operator[] (hkUint32 i)
{
  VASSERT (i <= 2);

  return data[i];
}

HKV_FORCE_INLINE const hkvVec3 hkvVec3::operator- (void) const
{
  return hkvVec3 (-x, -y, -z);
}

HKV_FORCE_INLINE void hkvVec3::operator+= (const hkvVec3& rhs)
{
  HKVVEC3_COPYTEMPS (rhs);
  x += rhs_x;
  y += rhs_y;
  z += rhs_z;
}

HKV_FORCE_INLINE void hkvVec3::operator-= (const hkvVec3& rhs)
{
  HKVVEC3_COPYTEMPS (rhs);
  x -= rhs_x;
  y -= rhs_y;
  z -= rhs_z;
}

HKV_FORCE_INLINE void hkvVec3::operator+= (float f)
{
  x += f;
  y += f;
  z += f;
}

HKV_FORCE_INLINE void hkvVec3::operator-= (float f)
{
  x -= f;
  y -= f;
  z -= f;
}

HKV_FORCE_INLINE void hkvVec3::operator*= (float f)
{
  x *= f;
  y *= f;
  z *= f;
}

HKV_FORCE_INLINE void hkvVec3::operator/= (float f0)
{
  const float f = 1.0f / f0;

  x *= f;
  y *= f;
  z *= f;
}

HKV_FORCE_INLINE bool hkvVec3::isZero (float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  if (fEpsilon == 0.0f)
    return ((x == 0) && (y == 0) && (z == 0));

  return ((x >= -fEpsilon) && (x <= fEpsilon) && 
          (y >= -fEpsilon) && (y <= fEpsilon) && 
          (z >= -fEpsilon) && (z <= fEpsilon));
}

HKV_FORCE_INLINE bool hkvVec3::isValid () const
{
  return (hkvMath::isFiniteNumber (x) && hkvMath::isFiniteNumber (y) && hkvMath::isFiniteNumber (z));
}

HKV_FORCE_INLINE bool hkvVec3::isNormalized (float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  const float len = getLengthSquared ();

  return hkvMath::isFloatEqual (len, 1.0f, fEpsilon);
}

HKV_FORCE_INLINE bool hkvVec3::isIdentical (const hkvVec3& rhs) const
{
  HKVVEC3_COPYTEMPS (rhs);
  return ((x == rhs_x) && (y == rhs_y) && (z == rhs_z));
}

HKV_FORCE_INLINE bool hkvVec3::isEqual (const hkvVec3& rhs, float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  HKVVEC3_COPYTEMPS (rhs);

  return (hkvMath::isFloatEqual (x, rhs_x, fEpsilon) &&
          hkvMath::isFloatEqual (y, rhs_y, fEpsilon) && 
          hkvMath::isFloatEqual (z, rhs_z, fEpsilon));
}

HKV_FORCE_INLINE float hkvVec3::getLengthSquared () const
{
  return (x * x + y * y + z * z);
}

HKV_FORCE_INLINE float hkvVec3::getLength () const
{
  return hkvMath::sqrt (getLengthSquared ());
}

HKV_FORCE_INLINE float hkvVec3::getLengthAndNormalize ()
{
  const float fLength = getLength ();
  operator/= (fLength);

  return fLength;
}

HKV_FORCE_INLINE float hkvVec3::getLengthInverse () const
{
  return hkvMath::sqrtInverse (getLengthSquared ());
}

HKV_FORCE_INLINE const hkvVec3 hkvVec3::getNormalized () const
{
  hkvVec3 temp (*this);
  temp.normalize ();
  return temp;
}

HKV_FORCE_INLINE void hkvVec3::normalize ()
{
  operator*= (getLengthInverse ());
}

HKV_FORCE_INLINE hkvResult hkvVec3::normalizeIfNotZero (float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  if ((isZero (fEpsilon)) || (!isValid ()))
    return HKV_FAILURE;

  normalize ();
  return HKV_SUCCESS;
}

HKV_FORCE_INLINE float hkvVec3::normalizedEnsureUnitLength ()
{
  if ((isZero (HKVMATH_EPSILON)) || (!isValid ()))
  {
    set (1, 0, 0);
    return 0.0f;
  }

  const float fLen = getLength ();
  *this /= fLen;

  return fLen;
}

HKV_FORCE_INLINE float hkvVec3::getDistanceTo (const hkvVec3& rhs) const
{
  return (rhs - *this).getLength ();
}

HKV_FORCE_INLINE float hkvVec3::getDistanceToSquared (const hkvVec3& rhs) const
{
  return (rhs - *this).getLengthSquared ();
}

HKV_FORCE_INLINE hkvResult hkvVec3::setLength (float f, float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  const hkvResult res = normalizeIfNotZero (fEpsilon);
  operator*= (f);

  return res;
}

HKV_FORCE_INLINE void hkvVec3::setMin (const hkvVec3& rhs)
{
  setMin (*this, rhs);
}

HKV_FORCE_INLINE void hkvVec3::setMax (const hkvVec3& rhs)
{
  setMax (*this, rhs);
}

HKV_FORCE_INLINE void hkvVec3::clampTo (const hkvVec3& MinVal, const hkvVec3& MaxVal)
{
  setClamped (*this, MinVal, MaxVal);
}

HKV_FORCE_INLINE const hkvVec3 hkvVec3::compMul (const hkvVec3& rhs) const
{
  HKVVEC3_COPYTEMPS (rhs);

  return hkvVec3 (x * rhs_x, y * rhs_y, z * rhs_z);
}

HKV_FORCE_INLINE const hkvVec3 hkvVec3::compDiv (const hkvVec3& rhs) const
{
  HKVVEC3_COPYTEMPS (rhs);

  return hkvVec3 (x / rhs_x, y / rhs_y, z / rhs_z);
}

HKV_FORCE_INLINE void hkvVec3::setMin (const hkvVec3& lhs, const hkvVec3& rhs)
{
  HKVVEC3_COPYTEMPS (lhs);
  HKVVEC3_COPYTEMPS (rhs);

  x = hkvMath::Min (lhs_x, rhs_x);
  y = hkvMath::Min (lhs_y, rhs_y);
  z = hkvMath::Min (lhs_z, rhs_z);
}

HKV_FORCE_INLINE void hkvVec3::setMax (const hkvVec3& lhs, const hkvVec3& rhs)
{
  HKVVEC3_COPYTEMPS (lhs);
  HKVVEC3_COPYTEMPS (rhs);

  x = hkvMath::Max (lhs_x, rhs_x);
  y = hkvMath::Max (lhs_y, rhs_y);
  z = hkvMath::Max (lhs_z, rhs_z);
}

HKV_FORCE_INLINE void hkvVec3::setClamped (const hkvVec3& val, const hkvVec3& MinVal, const hkvVec3& MaxVal)
{
  HKVVEC3_COPYTEMPS (val);
  HKVVEC3_COPYTEMPS (MinVal);
  HKVVEC3_COPYTEMPS (MaxVal);

  x = hkvMath::clamp (val_x, MinVal_x, MaxVal_x);
  y = hkvMath::clamp (val_y, MinVal_y, MaxVal_y);
  z = hkvMath::clamp (val_z, MinVal_z, MaxVal_z);
}

HKV_FORCE_INLINE void hkvVec3::setAbs (const hkvVec3& val)
{
  HKVVEC3_COPYTEMPS (val);
  
  x = hkvMath::Abs (val_x);
  y = hkvMath::Abs (val_y);
  z = hkvMath::Abs (val_z);
}

HKV_FORCE_INLINE void hkvVec3::setCross (const hkvVec3& lhs, const hkvVec3& rhs)
{
  *this = lhs.cross (rhs);
}

HKV_FORCE_INLINE void hkvVec3::setInterpolate (const hkvVec3& lhs, const hkvVec3& rhs, float t)
{
  HKVVEC3_COPYTEMPS (lhs);
  HKVVEC3_COPYTEMPS (rhs);

  x = hkvMath::interpolate (lhs_x, rhs_x, t);
  y = hkvMath::interpolate (lhs_y, rhs_y, t);
  z = hkvMath::interpolate (lhs_z, rhs_z, t);
}

HKV_FORCE_INLINE float hkvVec3::dot (const hkvVec3& rhs) const
{
  HKVVEC3_COPYTEMPS (rhs);

  return ((x * rhs_x) + (y * rhs_y) + (z * rhs_z));
}

HKV_FORCE_INLINE const hkvVec3 hkvVec3::cross (const hkvVec3& rhs) const
{
  HKVVEC3_COPYTEMPS (rhs);
  
  const float nx = y * rhs_z - z * rhs_y;
  const float ny = z * rhs_x - x * rhs_z;
  const float nz = x * rhs_y - y * rhs_x;

  return hkvVec3 (nx, ny, nz);
}

HKV_FORCE_INLINE void hkvVec3::makeOrthogonalTo (const hkvVec3& vNormal)
{
  HKVMATH_ASSERT (vNormal.isNormalized (), "The normal must be normalized.");

  // get the distance v is along this vector
  const float fDot = this->dot (vNormal);

  //subtract the distance to make it orthogonal
  *this -= fDot * vNormal;
}

HKV_FORCE_INLINE const hkvVec3 hkvVec3::getOrthogonalVector (void) const
{
  const hkvVec3 n = getNormalized ();

  if (hkvMath::Abs (n.dot (hkvVec3 (1, 0, 0))) > 0.8f)
    return n.cross (hkvVec3 (0, 1, 0));

  return n.cross (hkvVec3 (1, 0, 0));
}

HKV_FORCE_INLINE void hkvVec3::negate ()
{
  x = -x;
  y = -y;
  z = -z;
}

HKV_FORCE_INLINE void hkvVec3::reflect (const hkvVec3& vNormal)
{
  *this = getReflected (vNormal);
}

HKV_FORCE_INLINE const hkvVec3 hkvVec3::getReflected (const hkvVec3& vNormal) const
{
  HKVMATH_ASSERT (vNormal.isNormalized (), "The normal must be normalized.");

  const float d = this->dot (vNormal);

  return (*this) - 2.0f * d * vNormal;
}

HKV_FORCE_INLINE const hkvVec4 hkvVec3::getAsPosition () const
{
  return hkvVec4 (x, y, z, 1.0f);
}

HKV_FORCE_INLINE const hkvVec4 hkvVec3::getAsDirection () const
{
  return hkvVec4 (x, y, z, 0.0f);
}

HKV_FORCE_INLINE const hkvVec2 hkvVec3::getAsVec2 () const
{
  return hkvVec2 (x, y);
}

HKV_FORCE_INLINE const hkvVec4 hkvVec3::getAsVec4 (float _w) const
{
  return hkvVec4 (x, y, z, _w);
}

HKV_FORCE_INLINE bool operator== (const hkvVec3& lhs, const hkvVec3& rhs)
{
  return lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE bool operator!= (const hkvVec3& lhs, const hkvVec3& rhs)
{
  return !lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE const hkvVec3 operator+ (const hkvVec3& lhs, const hkvVec3& rhs)
{
  hkvVec3 temp (lhs);
  temp += rhs;
  return temp;
}

HKV_FORCE_INLINE const hkvVec3 operator- (const hkvVec3& lhs, const hkvVec3& rhs)
{
  hkvVec3 temp (lhs);
  temp -= rhs;
  return temp;
}

HKV_FORCE_INLINE const hkvVec3 operator* (const hkvVec3& lhs, float f)
{
  hkvVec3 temp (lhs);
  temp *= f;
  return temp;
}

HKV_FORCE_INLINE const hkvVec3 operator* (float f, const hkvVec3& rhs)
{
  hkvVec3 temp (rhs);
  temp *= f;
  return temp;
}

HKV_FORCE_INLINE const hkvVec3 operator/ (const hkvVec3& lhs, float f)
{
  hkvVec3 temp (lhs);
  temp /= f;
  return temp;
}

HKV_FORCE_INLINE float hkvVec3::getAngleBetween (const hkvVec3& rhs) const
{
  const hkvVec3 n1 = getNormalized ();
  const hkvVec3 n2 = rhs.getNormalized ();

  float fDot = hkvMath::clamp (n1.dot (n2), -1.0f, 1.0f); // to ensure that no precision loss yields values outside this range
  return hkvMath::acosDeg (fDot);

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
