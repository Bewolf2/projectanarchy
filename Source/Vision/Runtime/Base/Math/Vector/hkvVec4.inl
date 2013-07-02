/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVVEC4_INL
#define VBASE_HKVMATH_HKVVEC4_INL

#include <Vision/Runtime/Base/Math/Vector/hkvVec2.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>

// Helper macro to copy vector parameters into local variables
// This might look like stupid unnecessary code, but it helps the compiler to optimize the assembly code
#define HKVVEC4_COPYTEMPS(var) \
  const float var##_x = var.x; const float var##_y = var.y; const float var##_z = var.z; const float var##_w = var.w;

HKV_FORCE_INLINE hkvVec4::hkvVec4 (float _x, float _y, float _z, float _w) : x (_x), y (_y), z (_z), w (_w)
{
}

HKV_FORCE_INLINE hkvVec4::hkvVec4 (float xyzw) : x (xyzw), y (xyzw), z (xyzw), w (xyzw)
{
}

HKV_FORCE_INLINE void hkvVec4::setZero ()
{
  x = 0.0f;
  y = 0.0f;
  z = 0.0f;
  w = 0.0f;
}

HKV_FORCE_INLINE void hkvVec4::set (float _x, float _y, float _z, float _w)
{
  x = _x;
  y = _y;
  z = _z;
  w = _w;
}

HKV_FORCE_INLINE void hkvVec4::set (float xyzw)
{
  x = xyzw;
  y = xyzw;
  z = xyzw;
  w = xyzw;
}

HKV_FORCE_INLINE float hkvVec4::operator[] (hkUint32 i) const
{
  VASSERT (i <= 3);

  return data[i];
}

HKV_FORCE_INLINE float& hkvVec4::operator[] (hkUint32 i)
{
  VASSERT (i <= 3);

  return data[i];
}

HKV_FORCE_INLINE const hkvVec4 hkvVec4::operator- (void) const
{
  return hkvVec4 (-x, -y, -z, -w);
}

HKV_FORCE_INLINE void hkvVec4::operator+= (const hkvVec4& rhs)
{
  HKVVEC4_COPYTEMPS (rhs);
  
  x += rhs_x;
  y += rhs_y;
  z += rhs_z;
  w += rhs_w;
}

HKV_FORCE_INLINE void hkvVec4::operator-= (const hkvVec4& rhs)
{
  HKVVEC4_COPYTEMPS (rhs);

  x -= rhs_x;
  y -= rhs_y;
  z -= rhs_z;
  w -= rhs_w;
}

HKV_FORCE_INLINE void hkvVec4::operator+= (float f)
{
  x += f;
  y += f;
  z += f;
  w += f;
}

HKV_FORCE_INLINE void hkvVec4::operator-= (float f)
{
  x -= f;
  y -= f;
  z -= f;
  w -= f;
}

HKV_FORCE_INLINE void hkvVec4::operator*= (float f)
{
  x *= f;
  y *= f;
  z *= f;
  w *= f;
}

HKV_FORCE_INLINE void hkvVec4::operator/= (float f0)
{
  const float f = 1.0f / f0;

  x *= f;
  y *= f;
  z *= f;
  w *= f;
}

HKV_FORCE_INLINE bool hkvVec4::isZero (float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  if (fEpsilon == 0.0f)
    return ((x == 0) && (y == 0) && (z == 0) && (w == 0));

  return ((x >= -fEpsilon) && (x <= fEpsilon) && 
          (y >= -fEpsilon) && (y <= fEpsilon) && 
          (z >= -fEpsilon) && (z <= fEpsilon) &&
          (w >= -fEpsilon) && (w <= fEpsilon));
}

HKV_FORCE_INLINE bool hkvVec4::isValid () const
{
  return (hkvMath::isFiniteNumber (x) && hkvMath::isFiniteNumber (y) && hkvMath::isFiniteNumber (z) && hkvMath::isFiniteNumber (w));
}

HKV_FORCE_INLINE bool hkvVec4::isNormalized (float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  const float len = getLengthSquared ();

  return hkvMath::isFloatEqual (len, 1.0f, fEpsilon);
}

HKV_FORCE_INLINE bool hkvVec4::isIdentical (const hkvVec4& rhs) const
{
  HKVVEC4_COPYTEMPS (rhs);

  return ((x == rhs_x) && (y == rhs_y) && (z == rhs_z) && (w == rhs_w));
}

HKV_FORCE_INLINE bool hkvVec4::isEqual (const hkvVec4& rhs, float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  HKVVEC4_COPYTEMPS (rhs);

  return (hkvMath::isFloatEqual (x, rhs_x, fEpsilon) &&
          hkvMath::isFloatEqual (y, rhs_y, fEpsilon) && 
          hkvMath::isFloatEqual (z, rhs_z, fEpsilon) &&
          hkvMath::isFloatEqual (w, rhs_w, fEpsilon));
}

HKV_FORCE_INLINE float hkvVec4::getLengthSquared () const
{
  return ((x * x) + (y * y) + (z * z) + (w * w));
}

HKV_FORCE_INLINE float hkvVec4::getLength () const
{
  return hkvMath::sqrt (getLengthSquared ());
}

HKV_FORCE_INLINE float hkvVec4::getLengthAndNormalize ()
{
  const float fLength = getLength ();
  operator*= (1.0f / fLength);

  return fLength;
}

HKV_FORCE_INLINE float hkvVec4::getLengthInverse () const
{
  return hkvMath::sqrtInverse (getLengthSquared ());
}

HKV_FORCE_INLINE const hkvVec4 hkvVec4::getNormalized () const
{
  hkvVec4 temp (*this);
  temp.normalize ();
  return temp;
}

HKV_FORCE_INLINE void hkvVec4::normalize ()
{
  operator*= (getLengthInverse ());
}

HKV_FORCE_INLINE hkvResult hkvVec4::normalizeIfNotZero (float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  if ((isZero (fEpsilon)) || (!isValid ()))
    return HKV_FAILURE;
  
  normalize ();
  return HKV_SUCCESS;
}

HKV_FORCE_INLINE float hkvVec4::normalizedEnsureUnitLength ()
{
  if ((isZero (HKVMATH_EPSILON)) || (!isValid ()))
  {
    set (1, 0, 0, 0);
    return 0.0f;
  }

  const float fLen = getLength ();
  *this /= fLen;

  return fLen;
}

HKV_FORCE_INLINE float hkvVec4::getDistanceTo (const hkvVec4& rhs) const
{
  return (rhs - *this).getLength ();
}

HKV_FORCE_INLINE float hkvVec4::getDistanceToSquared (const hkvVec4& rhs) const
{
  return (rhs - *this).getLengthSquared ();
}

HKV_FORCE_INLINE hkvResult hkvVec4::setLength (float f, float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  const hkvResult res = normalizeIfNotZero (fEpsilon);
  operator*= (f);

  return res;
}

HKV_FORCE_INLINE void hkvVec4::setMin (const hkvVec4& rhs)
{
  setMin (*this, rhs);
}

HKV_FORCE_INLINE void hkvVec4::setMax (const hkvVec4& rhs)
{
  setMax (*this, rhs);
}

HKV_FORCE_INLINE void hkvVec4::clampTo (const hkvVec4& MinVal, const hkvVec4& MaxVal)
{
  setClamped (*this, MinVal, MaxVal);
}

HKV_FORCE_INLINE const hkvVec4 hkvVec4::compMul (const hkvVec4& rhs) const
{
  HKVVEC4_COPYTEMPS (rhs);

  return hkvVec4 (x * rhs_x, y * rhs_y, z * rhs_z, w * rhs_w);
}

HKV_FORCE_INLINE const hkvVec4 hkvVec4::compDiv (const hkvVec4& rhs) const
{
  HKVVEC4_COPYTEMPS (rhs);

  return hkvVec4 (x / rhs_x, y / rhs_y, z / rhs_z, w / rhs_w);
}

HKV_FORCE_INLINE void hkvVec4::setMin (const hkvVec4& lhs, const hkvVec4& rhs)
{
  HKVVEC4_COPYTEMPS (lhs);
  HKVVEC4_COPYTEMPS (rhs);

  x = hkvMath::Min (lhs_x, rhs_x);
  y = hkvMath::Min (lhs_y, rhs_y);
  z = hkvMath::Min (lhs_z, rhs_z);
  w = hkvMath::Min (lhs_w, rhs_w);
}

HKV_FORCE_INLINE void hkvVec4::setMax (const hkvVec4& lhs, const hkvVec4& rhs)
{
  HKVVEC4_COPYTEMPS (lhs);
  HKVVEC4_COPYTEMPS (rhs);

  x = hkvMath::Max (lhs_x, rhs_x);
  y = hkvMath::Max (lhs_y, rhs_y);
  z = hkvMath::Max (lhs_z, rhs_z);
  w = hkvMath::Max (lhs_w, rhs_w);
}

HKV_FORCE_INLINE void hkvVec4::setClamped (const hkvVec4& val, const hkvVec4& MinVal, const hkvVec4& MaxVal)
{
  HKVVEC4_COPYTEMPS (val);
  HKVVEC4_COPYTEMPS (MinVal);
  HKVVEC4_COPYTEMPS (MaxVal);

  x = hkvMath::clamp (val_x, MinVal_x, MaxVal_x);
  y = hkvMath::clamp (val_y, MinVal_y, MaxVal_y);
  z = hkvMath::clamp (val_z, MinVal_z, MaxVal_z);
  w = hkvMath::clamp (val_w, MinVal_w, MaxVal_w);
}

HKV_FORCE_INLINE void hkvVec4::setAbs (const hkvVec4& val)
{
  HKVVEC4_COPYTEMPS (val);
  
  x = hkvMath::Abs (val_x);
  y = hkvMath::Abs (val_y);
  z = hkvMath::Abs (val_z);
  w = hkvMath::Abs (val_w);
}

HKV_FORCE_INLINE void hkvVec4::setInterpolate (const hkvVec4& lhs, const hkvVec4& rhs, float t)
{
  HKVVEC4_COPYTEMPS (lhs);
  HKVVEC4_COPYTEMPS (rhs);

  x = hkvMath::interpolate (lhs_x, rhs_x, t);
  y = hkvMath::interpolate (lhs_y, rhs_y, t);
  z = hkvMath::interpolate (lhs_z, rhs_z, t);
  w = hkvMath::interpolate (lhs_w, rhs_w, t);
}

HKV_FORCE_INLINE float hkvVec4::dot (const hkvVec4& rhs) const
{
  HKVVEC4_COPYTEMPS (rhs);

  return ((x * rhs_x) + (y * rhs_y) + (z * rhs_z) + (w * rhs_w));
}

HKV_FORCE_INLINE void hkvVec4::negate ()
{
  x = -x;
  y = -y;
  z = -z;
  w = -w;
}

HKV_FORCE_INLINE const hkvVec2 hkvVec4::getAsVec2 () const
{
  return hkvVec2 (x, y);
}

HKV_FORCE_INLINE const hkvVec3 hkvVec4::getAsVec3 () const
{
  return hkvVec3 (x, y, z);
}

HKV_FORCE_INLINE bool operator== (const hkvVec4& lhs, const hkvVec4& rhs)
{
  return lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE bool operator!= (const hkvVec4& lhs, const hkvVec4& rhs)
{
  return !lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE const hkvVec4 operator+ (const hkvVec4& lhs, const hkvVec4& rhs)
{
  hkvVec4 temp (lhs);
  temp += rhs;
  return temp;
}

HKV_FORCE_INLINE const hkvVec4 operator- (const hkvVec4& lhs, const hkvVec4& rhs)
{
  hkvVec4 temp (lhs);
  temp -= rhs;
  return temp;
}

HKV_FORCE_INLINE const hkvVec4 operator* (const hkvVec4& lhs, float f)
{
  hkvVec4 temp (lhs);
  temp *= f;
  return temp;
}

HKV_FORCE_INLINE const hkvVec4 operator* (float f, const hkvVec4& rhs)
{
  hkvVec4 temp (rhs);
  temp *= f;
  return temp;
}

HKV_FORCE_INLINE const hkvVec4 operator/ (const hkvVec4& lhs, float f)
{
  hkvVec4 temp (lhs);
  temp /= f;
  return temp;
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
