/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVVEC2_INL
#define VBASE_HKVMATH_HKVVEC2_INL

#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>

// Helper macro to copy vector parameters into local variables
// This might look like stupid unnecessary code, but it helps the compiler to optimize the assembly code
#define HKVVEC2_COPYTEMPS(var) \
  const float var##_x = var.x; const float var##_y = var.y;

HKV_FORCE_INLINE hkvVec2::hkvVec2 (float _x, float _y) : x (_x), y (_y)
{
}

HKV_FORCE_INLINE hkvVec2::hkvVec2 (float xy) : x (xy), y (xy)
{
}

HKV_FORCE_INLINE void hkvVec2::setZero ()
{
  x = 0.0f;
  y = 0.0f;
}

HKV_FORCE_INLINE void hkvVec2::set (float _x, float _y)
{
  x = _x;
  y = _y;
}

HKV_FORCE_INLINE void hkvVec2::set (float xy)
{
  x = xy;
  y = xy;
}

HKV_FORCE_INLINE float hkvVec2::operator[] (hkUint32 i) const
{
  VASSERT (i <= 1);

  return data[i];
}

HKV_FORCE_INLINE float& hkvVec2::operator[] (hkUint32 i)
{
  VASSERT (i <= 1);

  return data[i];
}

HKV_FORCE_INLINE const hkvVec2 hkvVec2::operator- () const
{
  return hkvVec2 (-x, -y);
}

HKV_FORCE_INLINE void hkvVec2::operator+= (const hkvVec2& rhs)
{
  HKVVEC2_COPYTEMPS (rhs);
  x += rhs_x;
  y += rhs_y;
}

HKV_FORCE_INLINE void hkvVec2::operator-= (const hkvVec2& rhs)
{
  HKVVEC2_COPYTEMPS (rhs);
  x -= rhs_x;
  y -= rhs_y;
}

HKV_FORCE_INLINE void hkvVec2::operator+= (float f)
{
  x += f;
  y += f;
}

HKV_FORCE_INLINE void hkvVec2::operator-= (float f)
{
  x -= f;
  y -= f;
}

HKV_FORCE_INLINE void hkvVec2::operator*= (float f)
{
  x *= f;
  y *= f;
}

HKV_FORCE_INLINE void hkvVec2::operator/= (float f0)
{
  const float f = 1.0f / f0;

  x *= f;
  y *= f;
}

HKV_FORCE_INLINE bool hkvVec2::isZero (float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  if (fEpsilon == 0.0f)
    return ((x == 0) && (y == 0));

  return ((x >= -fEpsilon) && (x <= fEpsilon) && 
          (y >= -fEpsilon) && (y <= fEpsilon));
}

HKV_FORCE_INLINE bool hkvVec2::isValid () const
{
  return (hkvMath::isFiniteNumber (x) && hkvMath::isFiniteNumber (y));
}

HKV_FORCE_INLINE bool hkvVec2::isNormalized (float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  const float len = getLengthSquared ();

  return hkvMath::isFloatEqual (len, 1.0f, fEpsilon);
}

HKV_FORCE_INLINE bool hkvVec2::isIdentical (const hkvVec2& rhs) const
{
  HKVVEC2_COPYTEMPS (rhs);

  return ((x == rhs_x) && (y == rhs_y));
}

HKV_FORCE_INLINE bool hkvVec2::isEqual (const hkvVec2& rhs, float fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0f);

  HKVVEC2_COPYTEMPS (rhs);

  return (hkvMath::isFloatEqual (x, rhs_x, fEpsilon) &&
          hkvMath::isFloatEqual (y, rhs_y, fEpsilon));
}

HKV_FORCE_INLINE float hkvVec2::getLengthSquared () const
{
  return ((x * x) + (y * y));
}

HKV_FORCE_INLINE float hkvVec2::getLength () const
{
  return hkvMath::sqrt (getLengthSquared ());
}

HKV_FORCE_INLINE float hkvVec2::getLengthAndNormalize ()
{
  const float fLength = getLength ();
  operator*= (1.0f / fLength);

  return fLength;
}

HKV_FORCE_INLINE float hkvVec2::getLengthInverse () const
{
  return hkvMath::sqrtInverse (getLengthSquared ());
}

HKV_FORCE_INLINE const hkvVec2 hkvVec2::getNormalized () const
{
  hkvVec2 temp (*this);
  temp.normalize ();
  return temp;
}

HKV_FORCE_INLINE void hkvVec2::normalize ()
{
  operator*= (getLengthInverse ());
}

HKV_FORCE_INLINE hkvResult hkvVec2::normalizeIfNotZero (float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  if ((isZero (fEpsilon)) || (!isValid ()))
    return HKV_FAILURE;

  normalize ();
  return HKV_SUCCESS;
}

HKV_FORCE_INLINE float hkvVec2::normalizedEnsureUnitLength ()
{
  if ((isZero (HKVMATH_EPSILON)) || (!isValid ()))
  {
    set (1, 0);
    return 0.0f;
  }

  const float fLen = getLength ();
  *this /= fLen;

  return fLen;
}

HKV_FORCE_INLINE float hkvVec2::getDistanceTo (const hkvVec2& rhs) const
{
  return (rhs - *this).getLength ();
}

HKV_FORCE_INLINE float hkvVec2::getDistanceToSquared (const hkvVec2& rhs) const
{
  return (rhs - *this).getLengthSquared ();
}

HKV_FORCE_INLINE hkvResult hkvVec2::setLength (float f, float fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  const hkvResult res = normalizeIfNotZero (fEpsilon);
  operator*= (f);

  return res;
}

HKV_FORCE_INLINE void hkvVec2::setMin (const hkvVec2& rhs)
{
  setMin (*this, rhs);
}

HKV_FORCE_INLINE void hkvVec2::setMax (const hkvVec2& rhs)
{
  setMax (*this, rhs);
}

HKV_FORCE_INLINE void hkvVec2::clampTo (const hkvVec2& MinVal, const hkvVec2& MaxVal)
{
  setClamped (*this, MinVal, MaxVal);
}

HKV_FORCE_INLINE const hkvVec2 hkvVec2::compMul (const hkvVec2& rhs) const
{
  HKVVEC2_COPYTEMPS (rhs);

  return hkvVec2 (x * rhs_x, y * rhs_y);
}

HKV_FORCE_INLINE const hkvVec2 hkvVec2::compDiv (const hkvVec2& rhs) const
{
  HKVVEC2_COPYTEMPS (rhs);

  return hkvVec2 (x / rhs_x, y / rhs_y);
}

HKV_FORCE_INLINE void hkvVec2::setMin (const hkvVec2& lhs, const hkvVec2& rhs)
{
  HKVVEC2_COPYTEMPS (lhs);
  HKVVEC2_COPYTEMPS (rhs);

  x = hkvMath::Min (lhs_x, rhs_x);
  y = hkvMath::Min (lhs_y, rhs_y);
}

HKV_FORCE_INLINE void hkvVec2::setMax (const hkvVec2& lhs, const hkvVec2& rhs)
{
  HKVVEC2_COPYTEMPS (lhs);
  HKVVEC2_COPYTEMPS (rhs);

  x = hkvMath::Max (lhs_x, rhs_x);
  y = hkvMath::Max (lhs_y, rhs_y);
}

HKV_FORCE_INLINE void hkvVec2::setClamped (const hkvVec2& val, const hkvVec2& MinVal, const hkvVec2& MaxVal)
{
  HKVVEC2_COPYTEMPS (val);
  HKVVEC2_COPYTEMPS (MinVal);
  HKVVEC2_COPYTEMPS (MaxVal);

  x = hkvMath::clamp (val_x, MinVal_x, MaxVal_x);
  y = hkvMath::clamp (val_y, MinVal_y, MaxVal_y);
}

HKV_FORCE_INLINE void hkvVec2::setAbs (const hkvVec2& val)
{
  HKVVEC2_COPYTEMPS (val);
  
  x = hkvMath::Abs (val_x);
  y = hkvMath::Abs (val_y);
}

HKV_FORCE_INLINE void hkvVec2::setInterpolate (const hkvVec2& lhs, const hkvVec2& rhs, float t)
{
  HKVVEC2_COPYTEMPS (lhs);
  HKVVEC2_COPYTEMPS (rhs);

  x = hkvMath::interpolate (lhs_x, rhs_x, t);
  y = hkvMath::interpolate (lhs_y, rhs_y, t);
}

HKV_FORCE_INLINE float hkvVec2::dot (const hkvVec2& rhs) const
{
  HKVVEC2_COPYTEMPS (rhs);

  return ((x * rhs_x) + (y * rhs_y));
}

HKV_FORCE_INLINE void hkvVec2::makeOrthogonalTo (const hkvVec2& vNormal)
{
  HKVMATH_ASSERT (vNormal.isNormalized (), "The normal must be normalized.");

  // get the distance v is along this vector
  const float fDot = this->dot (vNormal);

  //subtract the distance to make it orthogonal
  *this -= fDot * vNormal;
}

HKV_FORCE_INLINE const hkvVec2 hkvVec2::getOrthogonalVector (void) const
{
  return hkvVec2 (y, -x);
}

HKV_FORCE_INLINE void hkvVec2::negate ()
{
  x = -x;
  y = -y;
}

HKV_FORCE_INLINE void hkvVec2::reflect (const hkvVec2& vNormal)
{
  *this = getReflected (vNormal);
}

HKV_FORCE_INLINE const hkvVec2 hkvVec2::getReflected (const hkvVec2& vNormal) const
{
  HKVMATH_ASSERT (vNormal.isNormalized (), "The reflection-normal must be normalized.");

  const float d = this->dot (vNormal);

  return (*this) - 2.0f * d * vNormal;
}

HKV_FORCE_INLINE const hkvVec3 hkvVec2::getAsVec3 (float _z) const
{
  return hkvVec3 (x, y, _z);
}

HKV_FORCE_INLINE const hkvVec4 hkvVec2::getAsVec4 (float _z, float _w) const
{
  return hkvVec4 (x, y, _z, _w);
}

HKV_FORCE_INLINE bool operator== (const hkvVec2& lhs, const hkvVec2& rhs)
{
  return lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE bool operator!= (const hkvVec2& lhs, const hkvVec2& rhs)
{
  return !lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE const hkvVec2 operator+ (const hkvVec2& lhs, const hkvVec2& rhs)
{
  hkvVec2 temp (lhs);
  temp += rhs;
  return temp;
}

HKV_FORCE_INLINE const hkvVec2 operator- (const hkvVec2& lhs, const hkvVec2& rhs)
{
  hkvVec2 temp (lhs);
  temp -= rhs;
  return temp;
}

HKV_FORCE_INLINE const hkvVec2 operator* (const hkvVec2& lhs, float f)
{
  hkvVec2 temp (lhs);
  temp *= f;
  return temp;
}

HKV_FORCE_INLINE const hkvVec2 operator* (float f, const hkvVec2& rhs)
{
  hkvVec2 temp (rhs);
  temp *= f;
  return temp;
}

HKV_FORCE_INLINE const hkvVec2 operator/ (const hkvVec2& lhs, float f)
{
  hkvVec2 temp (lhs);
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
