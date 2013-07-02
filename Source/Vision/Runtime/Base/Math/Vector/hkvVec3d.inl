/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVVEC3D_INL
#define VBASE_HKVMATH_HKVVEC3D_INL

#include <Vision/Runtime/Base/Math/Vector/hkvVec2.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>

// Helper macro to copy vector parameters into local variables
// This might look like stupid unnecessary code, but it helps the compiler to optimize the assembly code
#define HKVVEC3D_COPYTEMPS(var) \
  const double var##_x = var.x; const double var##_y = var.y; const double var##_z = var.z;

HKV_FORCE_INLINE hkvVec3d::hkvVec3d (const hkvVec3& rhs) 
{ 
  x = (double) rhs.x;
  y = (double) rhs.y;
  z = (double) rhs.z;
}

HKV_FORCE_INLINE hkvVec3d::hkvVec3d (double _x, double _y, double _z) : x (_x), y (_y), z (_z)
{
}

HKV_FORCE_INLINE hkvVec3d::hkvVec3d (double xyz) : x (xyz), y (xyz), z (xyz)
{
}

HKV_FORCE_INLINE void hkvVec3d::setZero ()
{
  x = 0.0;
  y = 0.0;
  z = 0.0;
}

HKV_FORCE_INLINE void hkvVec3d::set (double _x, double _y, double _z)
{
  x = _x;
  y = _y;
  z = _z;
}

HKV_FORCE_INLINE void hkvVec3d::set (double xyz)
{
  x = xyz;
  y = xyz;
  z = xyz;
}

HKV_FORCE_INLINE double hkvVec3d::operator[] (hkUint32 i) const
{
  VASSERT (i <= 2);

  return data[i];
}

HKV_FORCE_INLINE double& hkvVec3d::operator[] (hkUint32 i)
{
  VASSERT (i <= 2);

  return data[i];
}

HKV_FORCE_INLINE const hkvVec3d hkvVec3d::operator- (void) const
{
  return hkvVec3d (-x, -y, -z);
}

HKV_FORCE_INLINE void hkvVec3d::operator+= (const hkvVec3d& rhs)
{
  HKVVEC3D_COPYTEMPS (rhs);
  x += rhs_x;
  y += rhs_y;
  z += rhs_z;
}

HKV_FORCE_INLINE void hkvVec3d::operator-= (const hkvVec3d& rhs)
{
  HKVVEC3D_COPYTEMPS (rhs);
  x -= rhs_x;
  y -= rhs_y;
  z -= rhs_z;
}

HKV_FORCE_INLINE void hkvVec3d::operator+= (double f)
{
  x += f;
  y += f;
  z += f;
}

HKV_FORCE_INLINE void hkvVec3d::operator-= (double f)
{
  x -= f;
  y -= f;
  z -= f;
}

HKV_FORCE_INLINE void hkvVec3d::operator*= (double f)
{
  x *= f;
  y *= f;
  z *= f;
}

HKV_FORCE_INLINE void hkvVec3d::operator/= (double f0)
{
  const double f = 1.0 / f0;

  x *= f;
  y *= f;
  z *= f;
}

HKV_FORCE_INLINE bool hkvVec3d::isZero (double fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0);

  if (fEpsilon == 0.0)
    return ((x == 0) && (y == 0) && (z == 0));

  return ((x >= -fEpsilon) && (x <= fEpsilon) && 
          (y >= -fEpsilon) && (y <= fEpsilon) && 
          (z >= -fEpsilon) && (z <= fEpsilon));
}

HKV_FORCE_INLINE bool hkvVec3d::isNormalized (double fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0);

  const double len = getLengthSquared ();

  return hkvMath::isFloatEqual (len, 1.0, fEpsilon);
}

HKV_FORCE_INLINE bool hkvVec3d::isIdentical (const hkvVec3d& rhs) const
{
  HKVVEC3D_COPYTEMPS (rhs);
  return ((x == rhs_x) && (y == rhs_y) && (z == rhs_z));
}

HKV_FORCE_INLINE bool hkvVec3d::isEqual (const hkvVec3d& rhs, double fEpsilon) const
{
  VASSERT (fEpsilon >= 0.0);

  HKVVEC3D_COPYTEMPS (rhs);

  return (hkvMath::isFloatEqual (x, rhs_x, fEpsilon) &&
          hkvMath::isFloatEqual (y, rhs_y, fEpsilon) && 
          hkvMath::isFloatEqual (z, rhs_z, fEpsilon));
}

HKV_FORCE_INLINE double hkvVec3d::getLengthSquared () const
{
  return (x * x + y * y + z * z);
}

HKV_FORCE_INLINE double hkvVec3d::getLength () const
{
  return hkvMath::sqrtd (getLengthSquared ());
}

HKV_FORCE_INLINE double hkvVec3d::getLengthAndNormalize ()
{
  const double fLength = getLength ();
  operator/= (fLength);

  return fLength;
}

HKV_FORCE_INLINE double hkvVec3d::getLengthInverse () const
{
  return 1.0 / getLength ();
}

HKV_FORCE_INLINE const hkvVec3d hkvVec3d::getNormalized () const
{
  hkvVec3d temp (*this);
  temp.normalize ();
  return temp;
}

HKV_FORCE_INLINE void hkvVec3d::normalize ()
{
  operator*= (getLengthInverse ());
}

HKV_FORCE_INLINE hkvResult hkvVec3d::normalizeIfNotZero (double fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  if ((isZero (fEpsilon))/* || (!isValid ())*/)
    return HKV_FAILURE;

  normalize ();
  return HKV_SUCCESS;
}

HKV_FORCE_INLINE double hkvVec3d::normalizedEnsureUnitLength ()
{
  if ((isZero (HKVMATH_EPSILON))/* || (!isValid ())*/)
  {
    set (1, 0, 0);
    return 0.0;
  }

  const double fLen = getLength ();
  *this /= fLen;

  return fLen;
}

HKV_FORCE_INLINE double hkvVec3d::getDistanceTo (const hkvVec3d& rhs) const
{
  return (rhs - *this).getLength ();
}

HKV_FORCE_INLINE double hkvVec3d::getDistanceToSquared (const hkvVec3d& rhs) const
{
  return (rhs - *this).getLengthSquared ();
}

HKV_FORCE_INLINE hkvResult hkvVec3d::setLength (double f, double fEpsilon /* = HKVMATH_SMALL_EPSILON */)
{
  const hkvResult res = normalizeIfNotZero (fEpsilon);
  operator*= (f);

  return res;
}

HKV_FORCE_INLINE void hkvVec3d::setMin (const hkvVec3d& rhs)
{
  setMin (*this, rhs);
}

HKV_FORCE_INLINE void hkvVec3d::setMax (const hkvVec3d& rhs)
{
  setMax (*this, rhs);
}

HKV_FORCE_INLINE void hkvVec3d::clampTo (const hkvVec3d& MinVal, const hkvVec3d& MaxVal)
{
  setClamped (*this, MinVal, MaxVal);
}

HKV_FORCE_INLINE const hkvVec3d hkvVec3d::compMul (const hkvVec3d& rhs) const
{
  HKVVEC3D_COPYTEMPS (rhs);

  return hkvVec3d (x * rhs_x, y * rhs_y, z * rhs_z);
}

HKV_FORCE_INLINE const hkvVec3d hkvVec3d::compDiv (const hkvVec3d& rhs) const
{
  HKVVEC3D_COPYTEMPS (rhs);

  return hkvVec3d (x / rhs_x, y / rhs_y, z / rhs_z);
}

HKV_FORCE_INLINE void hkvVec3d::setMin (const hkvVec3d& lhs, const hkvVec3d& rhs)
{
  HKVVEC3D_COPYTEMPS (lhs);
  HKVVEC3D_COPYTEMPS (rhs);

  x = hkvMath::Min (lhs_x, rhs_x);
  y = hkvMath::Min (lhs_y, rhs_y);
  z = hkvMath::Min (lhs_z, rhs_z);
}

HKV_FORCE_INLINE void hkvVec3d::setMax (const hkvVec3d& lhs, const hkvVec3d& rhs)
{
  HKVVEC3D_COPYTEMPS (lhs);
  HKVVEC3D_COPYTEMPS (rhs);

  x = hkvMath::Max (lhs_x, rhs_x);
  y = hkvMath::Max (lhs_y, rhs_y);
  z = hkvMath::Max (lhs_z, rhs_z);
}

HKV_FORCE_INLINE void hkvVec3d::setClamped (const hkvVec3d& val, const hkvVec3d& MinVal, const hkvVec3d& MaxVal)
{
  HKVVEC3D_COPYTEMPS (val);
  HKVVEC3D_COPYTEMPS (MinVal);
  HKVVEC3D_COPYTEMPS (MaxVal);

  x = hkvMath::clamp (val_x, MinVal_x, MaxVal_x);
  y = hkvMath::clamp (val_y, MinVal_y, MaxVal_y);
  z = hkvMath::clamp (val_z, MinVal_z, MaxVal_z);
}

HKV_FORCE_INLINE void hkvVec3d::setAbs (const hkvVec3d& val)
{
  HKVVEC3D_COPYTEMPS (val);
  
  x = hkvMath::Abs (val_x);
  y = hkvMath::Abs (val_y);
  z = hkvMath::Abs (val_z);
}

HKV_FORCE_INLINE void hkvVec3d::setCross (const hkvVec3d& lhs, const hkvVec3d& rhs)
{
  *this = lhs.cross (rhs);
}

HKV_FORCE_INLINE void hkvVec3d::setInterpolate (const hkvVec3d& lhs, const hkvVec3d& rhs, double t)
{
  HKVVEC3D_COPYTEMPS (lhs);
  HKVVEC3D_COPYTEMPS (rhs);

  x = hkvMath::interpolate (lhs_x, rhs_x, t);
  y = hkvMath::interpolate (lhs_y, rhs_y, t);
  z = hkvMath::interpolate (lhs_z, rhs_z, t);
}

HKV_FORCE_INLINE double hkvVec3d::dot (const hkvVec3d& rhs) const
{
  HKVVEC3D_COPYTEMPS (rhs);

  return ((x * rhs_x) + (y * rhs_y) + (z * rhs_z));
}

HKV_FORCE_INLINE const hkvVec3d hkvVec3d::cross (const hkvVec3d& rhs) const
{
  HKVVEC3D_COPYTEMPS (rhs);
  
  const double nx = y * rhs_z - z * rhs_y;
  const double ny = z * rhs_x - x * rhs_z;
  const double nz = x * rhs_y - y * rhs_x;

  return hkvVec3d (nx, ny, nz);
}

HKV_FORCE_INLINE void hkvVec3d::makeOrthogonalTo (const hkvVec3d& vNormal)
{
  HKVMATH_ASSERT (vNormal.isNormalized (), "The normal must be normalized.");

  // get the distance v is along this vector
  const double fDot = this->dot (vNormal);

  //subtract the distance to make it orthogonal
  *this -= fDot * vNormal;
}

HKV_FORCE_INLINE const hkvVec3d hkvVec3d::getOrthogonalVector (void) const
{
  const hkvVec3d n = getNormalized ();

  if (hkvMath::Abs (n.dot (hkvVec3d (1, 0, 0))) > 0.8f)
    return n.cross (hkvVec3d (0, 1, 0));

  return n.cross (hkvVec3d (1, 0, 0));
}

HKV_FORCE_INLINE void hkvVec3d::negate ()
{
  x = -x;
  y = -y;
  z = -z;
}

HKV_FORCE_INLINE void hkvVec3d::reflect (const hkvVec3d& vNormal)
{
  *this = getReflected (vNormal);
}

HKV_FORCE_INLINE const hkvVec3d hkvVec3d::getReflected (const hkvVec3d& vNormal) const
{
  HKVMATH_ASSERT (vNormal.isNormalized (), "The normal must be normalized.");

  const double d = this->dot (vNormal);

  return (*this) - 2.0 * d * vNormal;
}

HKV_FORCE_INLINE const hkvVec4 hkvVec3d::getAsPosition () const
{
  return hkvVec4 ((float) x, (float) y, (float) z, 1.0f);
}

HKV_FORCE_INLINE const hkvVec4 hkvVec3d::getAsDirection () const
{
  return hkvVec4 ((float) x, (float) y, (float) z, 0.0f);
}

HKV_FORCE_INLINE const hkvVec2 hkvVec3d::getAsVec2 () const
{
  return hkvVec2 ((float) x, (float) y);
}

HKV_FORCE_INLINE const hkvVec3 hkvVec3d::getAsVec3 () const
{
  return hkvVec3 ((float) x, (float) y, (float) z);
}

HKV_FORCE_INLINE const hkvVec4 hkvVec3d::getAsVec4 (float _w) const
{
  return hkvVec4 ((float) x, (float) y, (float) z, _w);
}

HKV_FORCE_INLINE bool operator== (const hkvVec3d& lhs, const hkvVec3d& rhs)
{
  return lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE bool operator!= (const hkvVec3d& lhs, const hkvVec3d& rhs)
{
  return !lhs.isIdentical (rhs);
}

HKV_FORCE_INLINE const hkvVec3d operator+ (const hkvVec3d& lhs, const hkvVec3d& rhs)
{
  hkvVec3d temp (lhs);
  temp += rhs;
  return temp;
}

HKV_FORCE_INLINE const hkvVec3d operator- (const hkvVec3d& lhs, const hkvVec3d& rhs)
{
  hkvVec3d temp (lhs);
  temp -= rhs;
  return temp;
}

HKV_FORCE_INLINE const hkvVec3d operator* (const hkvVec3d& lhs, double f)
{
  hkvVec3d temp (lhs);
  temp *= f;
  return temp;
}

HKV_FORCE_INLINE const hkvVec3d operator* (double f, const hkvVec3d& rhs)
{
  hkvVec3d temp (rhs);
  temp *= f;
  return temp;
}

HKV_FORCE_INLINE const hkvVec3d operator/ (const hkvVec3d& lhs, double f)
{
  hkvVec3d temp (lhs);
  temp /= f;
  return temp;
}

HKV_FORCE_INLINE double hkvVec3d::getAngleBetween (const hkvVec3d& rhs) const
{
  const hkvVec3d n1 = getNormalized ();
  const hkvVec3d n2 = rhs.getNormalized ();

  double fDot = hkvMath::clamp (n1.dot (n2), -1.0, 1.0); // to ensure that no precision loss yields values outside this range
  return hkvMath::acosDeg ((float) fDot);

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
