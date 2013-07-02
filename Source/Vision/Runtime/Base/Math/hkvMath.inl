/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMATH_INL
#define VBASE_HKVMATH_HKVMATH_INL

#include <math.h>

HKV_FORCE_INLINE float hkvMath::sqrt (float f)
{
  return ::sqrtf (f);
}

HKV_FORCE_INLINE double hkvMath::sqrtd (double f)
{
  return ::sqrt (f);
}

HKV_FORCE_INLINE float hkvMath::sqrtInverse (float f)
{
  return (1.0f / sqrtf (f));
}

HKV_FORCE_INLINE float hkvMath::Abs (float t)
{
  return (t < 0 ? -t : t);
}

HKV_FORCE_INLINE double hkvMath::Abs (double t)
{
  return (t < 0 ? -t : t);
}

HKV_FORCE_INLINE hkInt32 hkvMath::Abs (hkInt32 t)
{
  return (t < 0 ? -t : t);
}

HKV_FORCE_INLINE float hkvMath::exp (float exponent)
{
  return ::expf (exponent);
}

HKV_FORCE_INLINE float hkvMath::pow (float base, float exponent)
{
  return ::powf (base, exponent);
}

template<class T>
HKV_FORCE_INLINE T hkvMath::square (T value)
{
  return (value * value);
}

HKV_FORCE_INLINE float hkvMath::pow2 (float exponent)
{
  return pow (2.0f, exponent);
}

HKV_FORCE_INLINE hkUint32 hkvMath::pow2 (hkUint32 exponent)
{
  return (1U << exponent);
}

HKV_FORCE_INLINE float hkvMath::ceil (float f)
{
  return ::ceilf (f);
}

HKV_FORCE_INLINE float hkvMath::floor (float f)
{
  return ::floorf (f);
}

HKV_FORCE_INLINE float hkvMath::sinDeg (float deg)
{
  return ::sinf (deg * HKVMATH_DEG_TO_RAD);
}

HKV_FORCE_INLINE float hkvMath::sinRad (float rad)
{
  return ::sinf (rad);
}

HKV_FORCE_INLINE float hkvMath::cosDeg (float deg)
{
  return ::cosf (deg * HKVMATH_DEG_TO_RAD);
}

HKV_FORCE_INLINE float hkvMath::cosRad (float rad)
{
  return ::cosf (rad);
}

HKV_FORCE_INLINE float hkvMath::tanDeg (float deg)
{
  return ::tan (deg * HKVMATH_DEG_TO_RAD);
}

HKV_FORCE_INLINE float hkvMath::tanRad (float rad)
{
  return ::tan (rad);
}

HKV_FORCE_INLINE float hkvMath::asinDeg (float f)
{
  return ::asinf (f) * HKVMATH_RAD_TO_DEG;
}

HKV_FORCE_INLINE float hkvMath::asinRad (float f)
{
  return ::asinf (f);
}

HKV_FORCE_INLINE float hkvMath::acosDeg (float f)
{
  return ::acosf (f) * HKVMATH_RAD_TO_DEG;
}

HKV_FORCE_INLINE float hkvMath::acosRad (float f)
{
  return ::acosf (f);
}

HKV_FORCE_INLINE float hkvMath::atanDeg (float f)
{
  return ::atan (f) * HKVMATH_RAD_TO_DEG;
}

HKV_FORCE_INLINE float hkvMath::atanRad (float f)
{
  return ::atan (f);
}

HKV_FORCE_INLINE float hkvMath::atan2Deg (float y, float x)
{
  return ::atan2f (y, x) * HKVMATH_RAD_TO_DEG;
}

HKV_FORCE_INLINE float hkvMath::atan2Rad (float y, float x)
{
  return ::atan2f (y, x);
}

HKV_FORCE_INLINE bool hkvMath::isAngleEqualDeg (float fAngle1, float fAngle2, float fEpsilon /* = HKVMATH_DEFAULT_EPSILON */)
{
  VASSERT (fEpsilon >= 0.0f);

  while (fAngle1 < 0.0f)
    fAngle1 += 360.0f;
  while (fAngle2 < 0.0f)
    fAngle2 += 360.0f;

  while (fAngle1 >= 360.0f)
    fAngle1 -= 360.0f;
  while (fAngle2 >= 360.0f)
    fAngle2 -= 360.0f;

  return isFloatEqual (fAngle1, fAngle2, fEpsilon);
}

HKV_FORCE_INLINE float hkvMath::log (float f)
{
  return ::logf (f);
}

HKV_FORCE_INLINE float hkvMath::log2 (float f)
{
  return log10 (f) / log10 (2.0f);
}

HKV_FORCE_INLINE float hkvMath::log10 (float f)
{
  return ::log10f (f);
}

HKV_FORCE_INLINE float hkvMath::logB (float f, float base)
{
  return log10 (f) / log10 (base);
}

HKV_FORCE_INLINE bool hkvMath::isFloatEqual (float f1, float f2, float fEpsilon)
{
  // epsilon MUST be larger than zero, otherwise hkMath::equal will fail
  VASSERT (fEpsilon >= 0.0f);

  if (fEpsilon == 0.0f)
    return (f1 == f2);

  // phrasing the test this way will make sure that NaN's will also return 'false'
  return ((f1 >= f2 - fEpsilon) && (f1 <= f2 + fEpsilon));
}

HKV_FORCE_INLINE bool hkvMath::isFloatEqual (double f1, double f2, double fEpsilon)
{
  // epsilon MUST be larger than zero, otherwise hkMath::equal will fail
  VASSERT (fEpsilon >= 0.0);

  if (fEpsilon == 0.0)
    return (f1 == f2);

  // phrasing the test this way will make sure that NaN's will also return 'false'
  return ((f1 >= f2 - fEpsilon) && (f1 <= f2 + fEpsilon));
}

HKV_FORCE_INLINE bool hkvMath::isZero (float f, float fEpsilon)
{
  return isFloatEqual (f, 0.0f, fEpsilon);
}

HKV_FORCE_INLINE bool hkvMath::isZero (double f, double fEpsilon)
{
  return isFloatEqual (f, 0.0, fEpsilon);
}


template<class T>
HKV_FORCE_INLINE T hkvMath::Min (T t1, T t2)
{
  return (t1 < t2 ? t1 : t2);
}

template<class T>
HKV_FORCE_INLINE T hkvMath::Min (T t1, T t2, T t3)
{
  return hkvMath::Min (hkvMath::Min (t1, t2), t3);
}

template<class T>
HKV_FORCE_INLINE T hkvMath::Min (T t1, T t2, T t3, T t4)
{
  return hkvMath::Min (hkvMath::Min (t1, t2), hkvMath::Min (t3, t4));
}

template<class T>
HKV_FORCE_INLINE T hkvMath::Min (T t1, T t2, T t3, T t4, T t5)
{
  return hkvMath::Min (hkvMath::Min (t1, t2, t3, t4), t5);
}

template<class T>
HKV_FORCE_INLINE T hkvMath::Max (T t1, T t2)
{
  return (t1 < t2 ? t2 : t1);
}

template<class T>
HKV_FORCE_INLINE T hkvMath::Max (T t1, T t2, T t3)
{
  return hkvMath::Max (hkvMath::Max (t1, t2), t3);
}

template<class T>
HKV_FORCE_INLINE T hkvMath::Max (T t1, T t2, T t3, T t4)
{
  return hkvMath::Max (hkvMath::Max (t1, t2), hkvMath::Max (t3, t4));
}

template<class T>
HKV_FORCE_INLINE T hkvMath::Max (T t1, T t2, T t3, T t4, T t5)
{
  return hkvMath::Max (hkvMath::Max (t1, t2, t3, t4), t5);
}

#if defined( _VISION_XENON ) || ( defined( _VISION_PS3 ) && !defined( SPU ) )

  template<>
  HKV_FORCE_INLINE float hkvMath::Min<float>(float t1, float t2)
  {
    return (float)__fsel( t1 - t2, t2, t1 );
  }

  template<>
  HKV_FORCE_INLINE double hkvMath::Min<double>(double t1, double t2)
  {
    return __fsel( t1 - t2, t2, t1 );
  }

  template<>
  HKV_FORCE_INLINE float hkvMath::Max<float>(float t1, float t2)
  {
    return (float)__fsel( t1 - t2, t1, t2 );
  }

  template<>
  HKV_FORCE_INLINE double hkvMath::Max<double>(double t1, double t2)
  {
    return __fsel( t1 - t2, t1, t2 );
  }
  
#endif

template<class T>
HKV_FORCE_INLINE T hkvMath::clamp (T val, T tMin, T tMax)
{
  VASSERT (tMin <= tMax);

  if (val < tMin) 
    return tMin;
  if (val > tMax)
    return tMax;

  return val;
}

template<class T>
HKV_FORCE_INLINE T hkvMath::saturate(T val)
{
  return clamp(val, T(0), T(1));
}

HKV_FORCE_INLINE float hkvMath::mod (float f, float d)
{
  return f - (d * float2int (f / d));
}

HKV_FORCE_INLINE bool hkvMath::isPowerOf (hkUint32 x, hkUint32 base)
{
  VASSERT (base > 1);

  if (x == 1)
    return true;

  while (x > base)
  {
    if (x % base == 0)
      x /= base;
    else
      return (false);
  }

  return (x == base);
}

HKV_FORCE_INLINE bool hkvMath::isPowerOf2 (hkUint32 x)
{
  return ((x != 0) && ((x & (x - 1U)) == 0));
}

template<class T>
HKV_FORCE_INLINE T hkvMath::interpolate (const T& val1, const T& val2, float fInterpolation)
{
  return val1 * (1.0f - fInterpolation) + val2 * fInterpolation;
}

template<class T>
HKV_FORCE_INLINE T hkvMath::interpolate (const T& val1, const T& val2, double fInterpolation)
{
  return val1 * (1.0 - fInterpolation) + val2 * fInterpolation;
}


HKV_FORCE_INLINE float hkvMath::root (float f, float fNthRoot)
{
  VASSERT ((f >= 0.0f) && (fNthRoot != 0.0f));

  return pow (f, 1.0f / fNthRoot);
}

template<class T>
HKV_FORCE_INLINE T hkvMath::sign (T t)
{
  if (t == 0)
    return 0;

  return t < 0 ? (T) (-1) : (T) (1);
}

HKV_FORCE_INLINE float hkvMath::round (float f)
{
  return floor (f + 0.5f);
}

HKV_FORCE_INLINE float hkvMath::roundTo (float f, float fRoundTo)
{
  return (round (f / fRoundTo) * fRoundTo);
}

HKV_FORCE_INLINE hkInt32 hkvMath::float2int (float f) 
{ 
  return (hkInt32) f; 
}

HKV_FORCE_INLINE float hkvMath::fraction (float f)
{
  return f - float2int (f);
}

template<class T>
HKV_FORCE_INLINE void hkvMath::swap (T& v1, T& v2)
{
  T temp = v1;
  v1 = v2;
  v2 = temp;
}

HKV_FORCE_INLINE hkUint32 hkvMath::powerOf2_floor (hkUint32 x)
{
  return powerOf2_ceil(x / 2 + 1);
}

HKV_FORCE_INLINE hkUint32 hkvMath::powerOf2_ceil (hkUint32 x)
{		
  // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2

  // The method below returns 0 for x == 0, so fix this case manually
  x += (x == 0);

  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;

  return x;
}

HKV_FORCE_INLINE float hkvMath::generateNaN (void)
{
  union 
  {
    float f;
    hkUint32 i;
  } val;

  // Usually NAN == (exponent = all 1, mantissa = non-zero)
  //         INF == (exponent = all 1, mantissa = zero)

  // = 0111 1111 1000 0000 0000 0000 0000 0001
  val.i = 0x7f800001;

  return val.f;
}

HKV_FORCE_INLINE bool hkvMath::isNaN (float r)
{
  // Havoks way to check for NaN:

  // Check the 8 exponent bits.
  // Usually NAN == (exponent = all 1, mantissa = non-zero)
  //         INF == (exponent = all 1, mantissa = zero)
  // This simply checks the exponent

  union 
  {
    float f;
    hkUint32 i;
  } val;

  val.f = r;
  return (((val.i & 0x7f800000) == 0x7f800000) && ((val.i & 0x7FFFFF) != 0));


  // all comparisons fail, if a float is NaN
  // even the equality test fails with a NaN
  // for all valid numbers 'f == f' will succeed and thus the final result will be false
  // for NaN 'f == f' will fail and thus the final result will be true

  // Note: DO NOT change it into 'f != f' !!!

  // Note: This code does not run in 64 Bit (Release), probably because the compiler optimizes too much away.
  //return (! (f == f));
}

HKV_FORCE_INLINE bool hkvMath::isFiniteNumber (float r)
{
  // Havoks way to check for Infinity:

  // Check the 8 exponent bits.
  // Usually NAN == (exponent = all 1, mantissa = non-zero)
  //         INF == (exponent = all 1, mantissa = zero)
  // This simply checks the exponent

  union 
  {
    float f;
    hkUint32 i;
  } val;

  val.f = r;
  return ((val.i & 0x7f800000) != 0x7f800000);


  // If f is inifinity or -infinity one of these comparisions will fail
  // If f is NaN, both comparisions will fail

  // Thus this function will only return true for a valid, finite number

  //return ((f <= HKVMATH_FLOAT_MAX_POS) && (f >= HKVMATH_FLOAT_MAX_NEG));
}

template<class T>
HKV_FORCE_INLINE bool hkvMath::isInRange (T val, T minVal, T maxVal)
{
  if (minVal < maxVal)
    return (val >= minVal) && (val <= maxVal);
  else
    return (val <= minVal) && (val >= maxVal);
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
