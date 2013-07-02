/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMATH_H
#define VBASE_HKVMATH_HKVMATH_H

#include <Vision/Runtime/Base/Math/hkvMathConfig.h>

// we could directly use the hkMath defines here (HK_REAL_PI etc.)
// but that would break our code, if hkMath is compiled to use doubles
#define HKVMATH_E 2.71828182845904f
#define HKVMATH_PI 3.14159265358979f
#define HKVMATH_DEG_TO_RAD (HKVMATH_PI / 180.0f)
#define HKVMATH_RAD_TO_DEG (180.0f / HKVMATH_PI)
#define HKVMATH_EPSILON FLT_EPSILON

#define HKVMATH_SMALL_EPSILON     0.000001f
#define HKVMATH_DEFAULT_EPSILON   0.00001f
#define HKVMATH_LARGE_EPSILON     0.0001f
#define HKVMATH_HUGE_EPSILON      0.001f
#define HKVMATH_GIGANTIC_EPSILON  0.01f

#define HKVMATH_FLOAT_MAX_POS (3.40282e+38f)    // max value - not actually FLT_MAX since on some systems FLT_MAX is indistinguishable from NaN or Inf
#define HKVMATH_FLOAT_MAX_NEG -HKVMATH_FLOAT_MAX_POS

// FLT_MIN is very irritating, because it is an even smaller value than FLT_EPSILON
// it is NOT the smallest possible float (ie. largest negative float)
// Therefore it is not defined here anymore, use HKVMATH_FLOAT_MAX_NEG instead



/// \brief
///   This class provides static functions for various basic mathematical operations.
///
/// It encapsulates all the frequently used math functions. They are either implemented in place
/// or forwarded to the proper (platform dependent) function. Therefore always prefer to call
/// functions from the class, instead of functions directly provided by the platform libraries.
/// This allows to implement each function in the most optimal way for each platform.
/// In debug builds you will additionally benefit from parameter validity checks.
class hkvMath
{
public:

  ///
  /// @name Constants and Conversions
  /// @{
  ///

  /// \brief
  ///   Returns Euler's number (2.718...)
  HKV_FORCE_INLINE static float e () { return HKVMATH_E; }

  /// \brief
  ///   Returns Pi (3.141...)
  HKV_FORCE_INLINE static float pi () { return HKVMATH_PI; }

  /// \brief
  ///   Returns the largest positive value that a 32 Bit float can represent.
  HKV_FORCE_INLINE static float FloatMaxPos () { return HKVMATH_FLOAT_MAX_POS; }

  /// \brief
  ///   Returns the largest negative value that a 32 Bit float can represent.
  HKV_FORCE_INLINE static float FloatMaxNeg () { return HKVMATH_FLOAT_MAX_NEG; }

  ///
  /// @}
  ///

  ///
  /// @name Trigonometric Functions
  /// @{
  ///

  /// \brief
  ///   Converts a given value from degree to radians.
  HKV_FORCE_INLINE static float Deg2Rad (float f) { return (HKVMATH_DEG_TO_RAD * f); }

  /// \brief
  ///   Converts a given value from radians to degree.
  HKV_FORCE_INLINE static float Rad2Deg (float f) { return (HKVMATH_RAD_TO_DEG * f); }

  /// \brief
  ///   Returns the sine of the given value, which is expected to be in degree.
  HKV_FORCE_INLINE static float sinDeg (float deg);

  /// \brief
  ///   Returns the sine of the given value, which is expected to be in radians.
  HKV_FORCE_INLINE static float sinRad (float rad);

  /// \brief
  ///   Returns the cosine of the given value, which is expected to be in degree.
  HKV_FORCE_INLINE static float cosDeg (float deg);

  /// \brief
  ///   Returns the cosine of the given value, which is expected to be in radians.
  HKV_FORCE_INLINE static float cosRad (float rad);

  /// \brief
  ///   Returns the tangents of the given value, which is expected to be in degree.
  HKV_FORCE_INLINE static float tanDeg (float deg);

  /// \brief
  ///   Returns the tangents of the given value, which is expected to be in radians.
  HKV_FORCE_INLINE static float tanRad (float rad);



  /// \brief
  ///   Returns the inverse sine (in degree) of f.
  HKV_FORCE_INLINE static float asinDeg (float f);

  /// \brief
  ///   Returns the inverse sine (in radians) of f.
  HKV_FORCE_INLINE static float asinRad (float f);

  /// \brief
  ///   Returns the inverse cosine (in degree) of f.
  HKV_FORCE_INLINE static float acosDeg (float f);

  /// \brief
  ///   Returns the inverse cosine (in radians) of f.
  HKV_FORCE_INLINE static float acosRad (float f);

  /// \brief
  ///   Returns the inverse tangents (in degree) of f.
  HKV_FORCE_INLINE static float atanDeg (float f);

  /// \brief
  ///   Returns the inverse tangents (in radians) of f.
  HKV_FORCE_INLINE static float atanRad (float f);

  /// \brief
  ///   Returns the inverse tangents (in degree) of (y / x).
  HKV_FORCE_INLINE static float atan2Deg (float y, float x);

  /// \brief
  ///   Returns the inverse tangents (in radians) of (y / x).
  HKV_FORCE_INLINE static float atan2Rad (float y, float x);

  /// \brief
  ///   Checks whether two angles, given in degree, are equal. Wraps around 360, if necessary.
  HKV_FORCE_INLINE static bool isAngleEqualDeg (float fAngle1, float fAngle2, float fEpsilon = HKVMATH_DEFAULT_EPSILON);

  ///
  /// @}
  ///

  ///
  /// @name Exponential Functions
  /// @{
  ///

  /// \brief
  ///   Returns the square root of f.
  HKV_FORCE_INLINE static float sqrt (float f);

  /// \brief
  ///   Returns the square root of f.
  HKV_FORCE_INLINE static double sqrtd (double f);

  /// \brief
  ///   Returns the inverse square root of f, ie. (1.0f / sqrt (f)).
  ///
  /// \param f
  ///   The value from which to compute the inverse square root.
  ///
  /// This function may use approximations to speed up the computation, compare to actually
  /// computing (1.0f / sqrt (f)).
  /// It should be preferred to multiply values with 'sqrtInverse (f)' instead of
  /// dividing by 'sqrt (f)'
  HKV_FORCE_INLINE static float sqrtInverse (float f);

  /// \brief
  ///   Returns the n-th root of f.
  ///
  /// \param f
  ///   The value from which to compute the nth root.
  ///
  /// \param fNthRoot
  ///   Which root to compute.
  ///
  /// E.g. 'root (16.0f, 2.0f)' will compute the square root of 16.0f, and will thus return 4.0f.
  /// E.g. 'root (81.0f, 4.0f)' will compute the 4th root of 81.0f, and will thus return 3.0f.
  HKV_FORCE_INLINE static float root (float f, float fNthRoot);

  /// \brief
  ///   Returns 'pow (e, exponent)' with e being Euler's number.
  HKV_FORCE_INLINE static float exp (float exponent);

  /// \brief
  ///   Returns 'base ^ exponent'.
  HKV_FORCE_INLINE static float pow (float base, float exponent);

  /// \brief
  ///   Returns 'value * value'
  ///
  /// \param value
  ///   The value that should be squared.
  ///
  /// This can be used to square long computations without writing them out twice.
  template<class T>
  HKV_FORCE_INLINE static T square (T value);

  /// \brief
  ///   Returns '2.0f ^ exponent'
  HKV_FORCE_INLINE static float pow2 (float exponent);

  /// \brief
  ///   Returns '2 ^ exponent'. Implemented as a left shift.
  HKV_FORCE_INLINE static hkUint32 pow2 (hkUint32 exponent);

  /// \brief
  ///   Checks whether x is base raised to any power.
  ///
  /// \param x
  ///   The value to check whether it is a power of 'base'
  ///
  /// \param base
  ///   The base value
  ///
  /// E.g. isPowerOf (81, 3) would return true.
  /// Always returns true for x == 1, as any number raised to 0 yields 1.
  HKV_FORCE_INLINE static bool isPowerOf (hkUint32 x, hkUint32 base);

  /// \brief
  ///   Checks whether x is 2 raised to any power.
  ///
  /// \param x
  ///   The value to check whether it is a power of 2.
  ///
  /// E.g. will return true for x == 1, 2, 4, 8, 16, ...
  HKV_FORCE_INLINE static bool isPowerOf2 (hkUint32 x);


  /// \brief
  ///   Returns the logarithm of f using e as its base.
  HKV_FORCE_INLINE static float log (float f);

  /// \brief
  ///   Returns the logarithm of f using 2 as its base.
  HKV_FORCE_INLINE static float log2 (float f);

  /// \brief
  ///   Returns the logarithm of f using 10 as its base.
  HKV_FORCE_INLINE static float log10 (float f);

  /// \brief
  ///   Returns the logarithm of f using 'base' as its base.
  HKV_FORCE_INLINE static float logB (float f, float base);

  ///
  /// @}
  ///

  ///
  /// @name Clamping and Rounding
  /// @{
  ///

  /// \brief
  ///   Returns the sign (0, -1 or +1) of t.
  ///
  /// \param t
  ///   The value from which to return the sign.
  ///
  /// \note For t == 0 it will return 0, not '+1'.
  template<class T>
  HKV_FORCE_INLINE static T sign (T t);

  /// \brief
  ///   Returns the absolute value of t.
  HKV_FORCE_INLINE static hkInt32 Abs (hkInt32  t);

  /// \brief
  ///   Returns the absolute value of t.
  HKV_FORCE_INLINE static float Abs (float t);

  /// \brief
  ///   Returns the absolute value of t.
  HKV_FORCE_INLINE static double Abs (double t);

  /// \brief
  ///   Returns the smallest integral value that is not less than f.
  ///
  /// That means it will always round towards the larger value, even for negative values of f.
  /// E.g. 'ceil (3.4f) == 4.0f'
  /// E.g. 'ceil (-3.4f) == -3.0f'
  ///
  /// \param f
  ///   The value which will be rounded up to the next integral value.
  HKV_FORCE_INLINE static float ceil (float f);

  /// \brief
  ///   Returns the largest integral value that is not greater than f.
  ///
  /// That means it will always round towards the smaller value, even for negative values of f.
  /// E.g. 'ceil (3.4f) == 3.0f'
  /// E.g. 'ceil (-3.4f) == -4.0f'
  ///
  /// \param f
  ///   The value which will be rounded down to the next integral value.
  HKV_FORCE_INLINE static float floor (float f);

  /// \brief
  ///   Returns the smallest power of two that is larger or equal to x.
  ///
  /// E.g. 'powerOf2_ceil (31) == 32'
  /// E.g. 'powerOf2_ceil (32) == 32'
  /// E.g. 'powerOf2_ceil (33) == 64'
  ///
  /// The result is undefined if the result can't be represented as an unsigned 32bit integer.
  ///
  /// \param x
  ///   The value which will be rounded up to the next power-of-two.
  HKV_FORCE_INLINE static hkUint32 powerOf2_ceil (hkUint32 x);

  /// \brief
  ///   Returns the largest power of two that is smaller or equal to x.
  ///
  /// E.g. 'powerOf2_ceil (31) == 16'
  /// E.g. 'powerOf2_ceil (32) == 32'
  /// E.g. 'powerOf2_ceil (33) == 32'
  ///
  /// The result of powerOf2_floor(0) is undefined.
  ///
  /// \param x
  ///   The value which will be rounded down to the next power-of-two.
  HKV_FORCE_INLINE static hkUint32 powerOf2_floor (hkUint32 x);

  /// \brief
  ///   Rounds f to the closest integral value.
  ///
  /// When exactly between two numbers (fractional part == 0.5f)
  /// the number will always be rounded 'up' ie. towards the larger value.
  /// E.g. 'round (1.49f)   ==  1.0f' // rounds 'down'
  /// E.g. 'round (1.5f)    ==  2.0f' // rounds 'up'
  /// E.g. 'round (-1.49f)  == -1.0f' // rounds 'down'
  /// E.g. 'round (-1.5f)   == -1.0f' // rounds 'up'
  /// E.g. 'round (-1.51f)  == -2.0f' // rounds 'down'
  ///
  /// \param f
  ///   The value which will be rounded to the closest integral value.
  HKV_FORCE_INLINE static float round (float f);

  /// \brief
  ///   Rounds f to the closest multiple of fRoundTo.
  ///
  /// E.g. 'roundTo ( 15.0f, 4.0f) == 16.0f'
  /// E.g. 'roundTo (-15.0f, 4.0f) ==-16.0f'
  ///
  /// \param f
  ///   The value which will be rounded to the closest multiple of fRoundTo.
  ///
  /// \param fRoundTo
  ///   To a multiple of this value will f be rounded.
  HKV_FORCE_INLINE static float roundTo (float f, float fRoundTo);

  /// \brief
  ///   Returns the integral part of f as an int.
  ///
  /// No rounding will take place.
  /// Might be implemented more efficiently than simply casting f to an int.
  /// Note that the sign of f will be preserved.
  /// In general the following should always hold:
  ///   'float2int (f) + fraction (f) == f'
  /// E.g. 'float2int (1.3f)  ==  1'
  /// E.g. 'float2int (1.5f)  ==  1'
  /// E.g. 'float2int (1.9f)  ==  1'
  /// E.g. 'float2int (-1.3f) == -1'
  /// E.g. 'float2int (-1.5f) == -1'
  /// E.g. 'float2int (-1.9f) == -1'
  ///
  /// \param f
  ///   The float value which will be converted to an int.
  HKV_FORCE_INLINE static hkInt32 float2int (float f);

  /// \brief
  ///   Returns the fractional part of f.
  ///
  /// Note that the sign of f will be preserved.
  /// In general the following should always hold:
  ///   'float2int (f) + fraction (f) == f'
  /// E.g. 'fraction (1.3f)  ==  0.3'
  /// E.g. 'fraction (1.5f)  ==  0.5'
  /// E.g. 'fraction (1.9f)  ==  0.9'
  /// E.g. 'fraction (-1.3f) == -0.3'
  /// E.g. 'fraction (-1.5f) == -0.5'
  /// E.g. 'fraction (-1.9f) == -0.9'
  ///
  /// \param f
  ///   The float whose fraction will be returned.
  HKV_FORCE_INLINE static float fraction (float f);

  /// \brief
  ///   Returns the smaller value t1 or t2.
  template<class T>
  HKV_FORCE_INLINE static T Min (T t1, T t2);

  /// \brief
  ///   Returns the smallest value t1 or t2 or t3.
  template<class T>
  HKV_FORCE_INLINE static T Min (T t1, T t2, T t3);

  /// \brief
  ///   Returns the smallest value t1 or t2 or t3 or t4.
  template<class T>
  HKV_FORCE_INLINE static T Min (T t1, T t2, T t3, T t4);

  /// \brief
  ///   Returns the smallest value t1 or t2 or t3 or t4 or t5.
  template<class T>
  HKV_FORCE_INLINE static T Min (T t1, T t2, T t3, T t4, T t5);


  /// \brief
  ///   Returns the larger value t1 or t2.
  template<class T>
  HKV_FORCE_INLINE static T Max (T t1, T t2);

  /// \brief
  ///   Returns the largest value t1 or t2 or t3.
  template<class T>
  HKV_FORCE_INLINE static T Max (T t1, T t2, T t3);

  /// \brief
  ///   Returns the largest value t1 or t2 or t3 or t4.
  template<class T>
  HKV_FORCE_INLINE static T Max (T t1, T t2, T t3, T t4);

  /// \brief
  ///   Returns the largest value t1 or t2 or t3 or t4 or t5.
  template<class T>
  HKV_FORCE_INLINE static T Max (T t1, T t2, T t3, T t4, T t5);

  /// \brief Returns the smallest of the given values.
  HKV_FORCE_INLINE static hkInt32 Min (hkInt32 t1, hkInt32 t2) { return Min<hkInt32> (t1, t2); }
  /// \brief Returns the smallest of the given values.
  HKV_FORCE_INLINE static hkInt32 Min (hkInt32 t1, hkInt32 t2, hkInt32 t3) { return Min<hkInt32> (t1, t2, t3); }
  /// \brief Returns the smallest of the given values.
  HKV_FORCE_INLINE static hkInt32 Min (hkInt32 t1, hkInt32 t2, hkInt32 t3, hkInt32 t4) { return Min<hkInt32> (t1, t2, t3, t4); }
  /// \brief Returns the smallest of the given values.
  HKV_FORCE_INLINE static hkInt32 Min (hkInt32 t1, hkInt32 t2, hkInt32 t3, hkInt32 t4, hkInt32 t5) { return Min<hkInt32> (t1, t2, t3, t4, t5); }

  /// \brief Returns the largest of the given values.
  HKV_FORCE_INLINE static hkInt32 Max (hkInt32 t1, hkInt32 t2) { return Max<hkInt32> (t1, t2); }
  /// \brief Returns the largest of the given values.
  HKV_FORCE_INLINE static hkInt32 Max (hkInt32 t1, hkInt32 t2, hkInt32 t3) { return Max<hkInt32> (t1, t2, t3); }
  /// \brief Returns the largest of the given values.
  HKV_FORCE_INLINE static hkInt32 Max (hkInt32 t1, hkInt32 t2, hkInt32 t3, hkInt32 t4) { return Max<hkInt32> (t1, t2, t3, t4); }
  /// \brief Returns the largest of the given values.
  HKV_FORCE_INLINE static hkInt32 Max (hkInt32 t1, hkInt32 t2, hkInt32 t3, hkInt32 t4, hkInt32 t5) { return Max<hkInt32> (t1, t2, t3, t4, t5); }

  /// \brief Returns the smallest of the given values.
  HKV_FORCE_INLINE static hkUint32 Min (hkUint32 t1, hkUint32 t2) { return Min<hkUint32> (t1, t2); }
  /// \brief Returns the smallest of the given values.
  HKV_FORCE_INLINE static hkUint32 Min (hkUint32 t1, hkUint32 t2, hkUint32 t3) { return Min<hkUint32> (t1, t2, t3); }
  /// \brief Returns the smallest of the given values.
  HKV_FORCE_INLINE static hkUint32 Min (hkUint32 t1, hkUint32 t2, hkUint32 t3, hkUint32 t4) { return Min<hkUint32> (t1, t2, t3, t4); }
  /// \brief Returns the smallest of the given values.
  HKV_FORCE_INLINE static hkUint32 Min (hkUint32 t1, hkUint32 t2, hkUint32 t3, hkUint32 t4, hkUint32 t5) { return Min<hkUint32> (t1, t2, t3, t4, t5); }

  /// \brief Returns the largest of the given values.
  HKV_FORCE_INLINE static hkUint32 Max (hkUint32 t1, hkUint32 t2) { return Max<hkUint32> (t1, t2); }
  /// \brief Returns the largest of the given values.
  HKV_FORCE_INLINE static hkUint32 Max (hkUint32 t1, hkUint32 t2, hkUint32 t3) { return Max<hkUint32> (t1, t2, t3); }
  /// \brief Returns the largest of the given values.
  HKV_FORCE_INLINE static hkUint32 Max (hkUint32 t1, hkUint32 t2, hkUint32 t3, hkUint32 t4) { return Max<hkUint32> (t1, t2, t3, t4); }
  /// \brief Returns the largest of the given values.
  HKV_FORCE_INLINE static hkUint32 Max (hkUint32 t1, hkUint32 t2, hkUint32 t3, hkUint32 t4, hkUint32 t5) { return Max<hkUint32> (t1, t2, t3, t4, t5); }

  /// \brief
  ///   Returns val clamped to the range [tMin;tMax].
  ///
  /// In other words 'Max (tMin, Min (tMax, val))'
  /// E.g. 'clamp (3, 2, 5) == 3' // no clamping
  /// E.g. 'clamp (1, 2, 5) == 2' // clamped to tMin
  /// E.g. 'clamp (7, 2, 5) == 5' // clamped to tMax
  ///
  /// \param val
  ///   The value to be clamped.
  ///
  /// \param tMin
  ///   The lower bound to clamp val to.
  ///
  /// \param tMax
  ///   The upper bound to clamp val to.
  template<class T>
  HKV_FORCE_INLINE static T clamp (T val, T tMin, T tMax);

  /// \brief
  ///   Returns val clamped to the range [0;1].
  ///
  /// E.g. 'saturate (+0.3) == 0.3' // no clamping
  /// E.g. 'saturate (-0.1) == 0.0' // clamped to 0
  /// E.g. 'saturate (+1.2) == 1.0' // clamped to 1
  ///
  /// \param val
  ///   The value to be clamped.
  ///
  template<class T>
  HKV_FORCE_INLINE static T saturate (T val);

  /// \brief
  ///   Checks whether val is between minVal and maxVal (inclusive).
  template<class T>
  HKV_FORCE_INLINE static bool isInRange (T val, T minVal, T maxVal);

  ///
  /// @}
  ///

  ///
  /// @name Special Float Functions
  /// @{
  ///

  /// \brief
  ///   Returns 'f modulo d' for floats.
  ///
  /// If f is perfectly dividable by d the return value will be zero.
  /// Otherwise it will be the remainder of the division of f and d.
  /// E.g. 'mod (2.7f, 0.3f) == 0.0f' // because 2.7f / 0.3f == 9.0f * 0.3f + 0.0f
  /// E.g. 'mod (2.7f, 0.4f) == 0.3f' // because 2.7f / 0.4f == 6.0f * 0.4f + 0.3f
  ///
  /// \param f
  ///   The value of which the module should be taken.
  ///
  /// \param d
  ///   The divisor by which f will be divided.
  HKV_FORCE_INLINE static float mod (float f, float d);

  /// \brief
  ///   Checks whether f is equal to zero within a given epsilon range.
  HKV_FORCE_INLINE static bool isZero (float f, float fEpsilon = HKVMATH_LARGE_EPSILON);

  /// \brief
  ///   Checks whether f is equal to zero within a given epsilon range.
  HKV_FORCE_INLINE static bool isZero (double f, double fEpsilon = HKVMATH_LARGE_EPSILON);

  /// \brief
  ///   Checks whether f1 is equal to f2 within a given epsilon range.
  HKV_FORCE_INLINE static bool isFloatEqual (float f1, float f2, float fEpsilon = HKVMATH_LARGE_EPSILON);

  /// \brief
  ///   Checks whether f1 is equal to f2 within a given epsilon range.
  HKV_FORCE_INLINE static bool isFloatEqual (double f1, double f2, double fEpsilon = HKVMATH_LARGE_EPSILON);

  /// \brief
  ///   Returns true if f is between -Infinity and +Infinity and is not NaN.
  HKV_FORCE_INLINE static bool isFiniteNumber (float f);

  /// \brief
  ///   Same as 'isFiniteNumber': Returns true if f is between -Infinity and +Infinity and is not NaN.
  HKV_FORCE_INLINE static bool isValidFloat (float f) { return isFiniteNumber (f); }

  /// \brief
  ///   Returns true if f is NaN (Not-a-Number). In all other cases (including +/- Infinity) it will return false.
  HKV_FORCE_INLINE static bool isNaN (float f);

  /// \brief
  ///   Returns a float NaN (Not-A-Number). Can be used to set values to an invalid state.
  HKV_FORCE_INLINE static float generateNaN (void);

  ///
  /// @}
  ///

  ///
  /// @name Utility Functions
  /// @{
  ///

  /// \brief
  ///   Swaps the values of the variables t1 and t2.in place.
  ///
  /// E.g. x = 1; y = 2; swap (x, y); -> x == 2 and y == 1
  /// Internally this creates a temporary object which must be copy-constructible.
  ///
  /// \param t1
  ///   The value to be swapped with t2.
  ///
  /// \param t2
  ///   The value to be swapped with t1.
  template<class T>
  HKV_FORCE_INLINE static void swap (T& t1, T& t2);

  /// \brief
  ///   Returns a value that was linearly interpolated between val1 and val2.
  ///
  /// \param val1
  ///   This value will be returned if fInterpolation is 0.0f.
  ///
  /// \param val2
  ///   This value will be returned if fInterpolation is 1.0f.
  ///
  /// \param fInterpolation
  ///   Interpolation factor, must be between 0 and 1.
  ///   This function will assert if fInterpolation is outside the [0;1] range.
  template<class T>
  HKV_FORCE_INLINE static T interpolate (const T& val1, const T& val2, float fInterpolation);

  /// \brief
  ///   Returns a value that was linearly interpolated between val1 and val2.
  ///
  /// \param val1
  ///   This value will be returned if fInterpolation is 0.0f.
  ///
  /// \param val2
  ///   This value will be returned if fInterpolation is 1.0f.
  ///
  /// \param fInterpolation
  ///   Interpolation factor, must be between 0 and 1.
  ///   This function will assert if fInterpolation is outside the [0;1] range.
  template<class T>
  HKV_FORCE_INLINE static T interpolate (const T& val1, const T& val2, double fInterpolation);

  ///
  /// @}
  ///
};

/// \brief Dummy struct to select a non-initializing constructor.
struct hkvInit_None
{
};

/// \brief Predefined variable of hkvInit_None which can be used to select a non-initializing constructor.
static const hkvInit_None hkvNoInitialization = hkvInit_None ();

//#define hkvNoInitialization hkvInit_None ()

#include <Vision/Runtime/Base/Math/hkvMath.inl>

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
