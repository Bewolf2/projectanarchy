/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVVEC4_H
#define VBASE_HKVMATH_HKVVEC4_H

#include <Vision/Runtime/Base/Math/hkvMath.h>

/// \brief
///   This class represents a 4-component vector.
///
/// The 4th component is used in all operations, and never ignored.
/// That means normalize, multiply with a 4x4 matrix etc. all use the
/// 4th component properly. Therefore you must ensure that it contains
/// proper values.\n
/// In most situations it will make more sense to use hkvVec3 instead.
/// hkvVec4 should mostly be used when working with float RGBA color, or with
/// shader registers. For points and directions always use hkvVec3 instead.
///
/// \sa hkvVec3
class hkvVec4
{
public:

  ///
  /// @name Constructors
  /// @{
  ///

  #ifdef HKVMATH_DEFAULTCONSTRUCTORS_INITIALIZEDATA
    /// \brief
    ///   DEPRECATED: Initializes the vector to all zero.
    ///
    /// Prefer to initialize the vector with hkvVec4::Zero if you want a zero initialized vector.
    /// Prefer to initialize the vector with hkvNoInitialization if it does not need to be initialized.
    /// Note: At some point the Vision Engine will deactivate this old behavior and use the uninitialized version instead.
    /// At that time you need to make sure that whenever you default construct a vector, you do not rely on it being zero.
    ///
    /// You can find all the places where you use the default constructor by defining 
    /// HKVMATH_DEPRECATE_DEFAULT_CONSTRUCTOR in hkvMathConfig.h and compiling your code for Windows.
    /// Then the compiler will generate a warning for every location where you use the default constructor.
    /// Use the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION to prevent that warning
    /// for code that cannot be changed to use a non default constructor (such as for arrays).
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvVec4 () : x (0.0f), y (0.0f), z (0.0f), w (0.0f) {}

  #else

    /// \brief
    ///   FUTURE BEHAVIOR: Keeps the vector uninitialized.
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvVec4 () {}
  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvVec4 (hkvInit_None init /* = hkvNoInitialization */) { }

  /// \brief
  ///   Initializes all components of the vector to the same value.
  HKV_FORCE_INLINE explicit hkvVec4 (float xyzw);

  /// \brief
  ///   Initializes each component individually.
  HKV_FORCE_INLINE hkvVec4 (float _x, float _y, float _z, float _w);

  ///
  /// @}
  ///

  ///
  /// @name Static Functions for Default Vectors
  /// @{
  ///

  /// \brief
  ///   Returns a zero vector.
  HKV_FORCE_INLINE static const hkvVec4 ZeroVector () { return hkvVec4 (0.0f); }

  ///
  /// @}
  ///

  ///
  /// @name Conversions
  /// @{
  ///

  /// \brief
  ///   Returns a hkvVec2 with the same values for x and y.
  HKV_FORCE_INLINE const hkvVec2 getAsVec2 () const;

  /// \brief
  ///   Returns a hkvVec3 with the same values for x,y and z.
  HKV_FORCE_INLINE const hkvVec3 getAsVec3 () const;

  ///
  /// @}
  ///

  ///
  /// @name Setting Data
  /// @{
  ///

  /// \brief
  ///   Sets all components to zero.
  HKV_FORCE_INLINE void setZero ();

  /// \brief
  ///   Sets components to the same value.
  HKV_FORCE_INLINE void set (float xyzw);

  /// \brief
  ///   Sets all components to their given value.
  HKV_FORCE_INLINE void set (float _x, float _y, float _z, float _w);

  ///
  /// @}
  ///

  ///
  /// @name Accessing Data
  /// @{
  ///

  /// \brief
  ///   Allows to access the vectors components through an index.
  ///
  /// \param i
  ///   Must be in range [0;3]
  HKV_FORCE_INLINE float operator[] (hkUint32 i) const;

  /// \brief
  ///   Allows to access the vectors components through an index.
  ///
  /// \param i
  ///   Must be in range [0;3]
  HKV_FORCE_INLINE float& operator[] (hkUint32 i);

  ///
  /// @}
  ///

  ///
  /// @name Comparisons & Checks
  /// @{
  ///

  /// \brief
  ///   Checks whether all components are zero within some epsilon range.
  HKV_FORCE_INLINE bool isZero (float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  /// \brief
  ///   Checks whether this vector and rhs are exactly equal.
  HKV_FORCE_INLINE bool isIdentical (const hkvVec4& rhs) const;

  /// \brief
  ///   Checks whether this vector and rhs are equal within some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvVec4& rhs, float fEpsilon) const;

  /// \brief
  ///   Checks whether all components are valid floats.
  ///
  /// If any component is +/- infinity or NaN, false will be returned.
  ///
  /// \sa hkvMath::isFiniteNumber
  HKV_FORCE_INLINE bool isValid () const;

  /// \brief
  ///   Checks whether this vector has a length of 1, within some epsilon range.
  ///
  /// \param fEpsilon
  ///   The maximum deviation that is allowed. The default value is reasonably large to handle the most common cases.
  ///
  /// \sa hkvMath::isFloatEqual
  HKV_FORCE_INLINE bool isNormalized (float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  ///
  /// @}
  ///

  ///
  /// @name Length & Distance
  /// @{
  ///

  /// \brief
  ///   Returns the dot product of this vector and rhs.
  HKV_FORCE_INLINE float dot (const hkvVec4& rhs) const;

  /// \brief
  ///   Returns the length of this vector.
  ///
  /// Prefer to use getLengthSquared, if you only want to compare relative lengths,
  /// since the computation of the squared length does not require a sqrt.
  /// Prefer to use getLengthInverse if you want to divide other values by the length,
  /// since that function might be implemented more efficiently.
  /// Prefer to use getLengthAndNormalize if you want to know the current length but
  /// also need to normalize the vector later.
  ///
  /// \sa hkvVec4::getLengthSquared
  /// \sa hkvVec4::getLengthInverse
  /// \sa hkvVec4::getLengthAndNormalize
  /// \sa hkvVec4::getDistanceTo
  /// \sa hkvVec4::getDistanceToSquared
  HKV_FORCE_INLINE float getLength () const;

  /// \brief
  ///   Returns the squared length of this vector.
  ///
  /// It is more efficient to use the squared length, instead of the real length,
  /// if one only wants to compare vectors for their relative length 
  ///
  /// \sa hkvVec4::dot
  /// \sa hkvVec4::getLength
  /// \sa hkvVec4::getDistanceToSquared
  HKV_FORCE_INLINE float getLengthSquared () const;

  /// \brief
  ///   Returns ( 1.0f / getLength () ), but computes this more efficiently.
  ///
  /// If the length is supposed to be used for normalization purposes, prefer this function.
  ///
  /// \sa hkvVec4::getLength
  HKV_FORCE_INLINE float getLengthInverse () const;

  /// \brief
  ///   Returns the current length and then normalizes this vector. More efficient than calling 'getLength' and then 'normalize'.
  ///
  /// This function will only compute the length once, whereas 'getLength' followed by 'normalize' will do this twice.
  ///
  /// \sa hkvVec4::getLength
  /// \sa hkvVec4::getLengthInverse
  /// \sa hkvVec4::normalize
  HKV_FORCE_INLINE float getLengthAndNormalize ();

  /// \brief
  ///   Normalizes this vector, e.g. makes sure its length is 1.
  ///
  /// If the vector was zero or degenerate before, it will have invalid data afterwards.
  /// Normalizing zero-length vectors will give undefined results and may be very different on each platform.
  ///
  /// \sa hkvVec4::getLength
  /// \sa hkvVec4::getLengthInverse
  /// \sa hkvVec4::getNormalized
  /// \sa hkvVec4::normalizeIfNotZero
  /// \sa hkvVec4::normalizedEnsureUnitLength
  HKV_FORCE_INLINE void normalize ();

  /// \brief
  ///   Returns a normalized copy of this vector.
  ///
  /// The behavior is identical to normalize.
  ///
  /// \sa hkvVec4::normalize
  HKV_FORCE_INLINE const hkvVec4 getNormalized () const;

  /// \brief
  ///   Normalizes the vector, if its length is non-zero. Otherwise the vector will stay unmodified.
  ///
  /// If normalization was not possible, it returns HKV_FAILURE otherwise HKV_SUCCESS.
  /// In case of failure the vector will not be modified. That means the vector will either be all zero
  /// (and thus zero-length) or some component is invalid (+/-Infinity or NaN).
  ///
  /// \param fEpsilon If all values are in range [-fEpsilon; +fEpsilon] the vector is considered to be 'zero'.
  ///
  /// \sa hkvVec4::normalize
  /// \sa hkvVec4::normalizedEnsureUnitLength
  HKV_FORCE_INLINE hkvResult normalizeIfNotZero (float fEpsilon = HKVMATH_SMALL_EPSILON);

  /// \brief
  ///   Normalizes this vector under all circumstances and returns its previous length.
  ///
  /// If the vector can not be normalized, its x component will be set to 1 and all other components to 0.
  /// In this case the function will return 0, as the vectors previous length was 0 (or invalid).
  /// If the vector can be normalized, the previous length is returned.
  ///
  /// \sa hkvVec4::normalize
  /// \sa hkvVec4::normalizeIfNotZero
  HKV_FORCE_INLINE float normalizedEnsureUnitLength ();

  /// \brief
  ///   Returns the distance between this vector and rhs.
  ///
  /// Prefer to use getDistanceToSquared if you only need to compare relative distances,
  /// as this is more efficient to compute.
  ///
  /// \param rhs
  ///   The point to which to compute the distance.
  ///
  /// \sa hkvVec4::getDistanceToSquared
  /// \sa hkvVec4::getLength
  HKV_FORCE_INLINE float getDistanceTo (const hkvVec4& rhs) const;

  /// \brief
  ///   Returns the squared distance between this vector and rhs.
  ///
  ///   This is more efficient to use than getLength, if only relative comparisons are needed.
  ///   E.g. to check whether the distance between v1 and v2 is larger than between v1 and v3.
  ///
  /// \param rhs
  ///   The point to which to compute the distance.
  ///
  /// \sa hkvVec4::getDistanceTo
  /// \sa hkvVec4::getLengthSquared
  HKV_FORCE_INLINE float getDistanceToSquared (const hkvVec4& rhs) const;

  /// \brief
  ///   Normalizes this vector and then multiplies it by f.
  ///
  /// Returns HKV_FAILURE, if the vector could not be normalized.
  /// In this case the vector does not change.
  /// In case of failure the function acts identical to normalizeIfNotZero.
  ///
  /// \param f
  ///   The length to which to set the vector.
  /// \param fEpsilon
  ///   If the vector's original length is below this threshold normalization fails and thus the vector's length is not changed.
  ///
  /// \sa hkvVec4::normalizeIfNotZero
  HKV_FORCE_INLINE hkvResult setLength (float f, float fEpsilon = HKVMATH_SMALL_EPSILON);

  ///
  /// @}
  ///

  ///
  /// @name Component-wise Modifications
  /// @{
  ///

  /// \brief
  ///   Negates this vector, e.g. flips the sign of each component.
  HKV_FORCE_INLINE void negate ();

  /// \brief
  ///   Returns a vector that is the component-wise multiplication of this vector and rhs.
  HKV_FORCE_INLINE const hkvVec4 compMul (const hkvVec4& rhs) const;

  /// \brief
  ///   Returns a vector that is the component-wise division of this vector and rhs.
  HKV_FORCE_INLINE const hkvVec4 compDiv (const hkvVec4& rhs) const;

  /// \brief
  ///   Each component of this vector is set to the minimum of itself and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMin (const hkvVec4& rhs);

  /// \brief
  ///   Each component of this vector is set to the maximum of itself and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMax (const hkvVec4& rhs);

  /// \brief
  ///   Each component of this vector clamped to the min/max range defined by MinVal and MaxVal.
  HKV_FORCE_INLINE void clampTo (const hkvVec4& MinVal, const hkvVec4& MaxVal);

  /// \brief
  ///   Each component of this vector is set to the minimum of lhs and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMin (const hkvVec4& lhs, const hkvVec4& rhs);

  /// \brief
  ///   Each component of this vector is set to the maximum of lhs and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMax (const hkvVec4& lhs, const hkvVec4& rhs);

  /// \brief
  ///   Each component of this vector is set to val clamped to the min/max range defined by MinVal and MaxVal.
  HKV_FORCE_INLINE void setClamped (const hkvVec4& val, const hkvVec4& MinVal, const hkvVec4& MaxVal);

  /// \brief
  ///   All components of this vector are set to the absolute value of the corresponding component of val.
  HKV_FORCE_INLINE void setAbs (const hkvVec4& val);

  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///

  /// \brief
  ///   Returns a negated copy of this vector.
  HKV_FORCE_INLINE const hkvVec4 operator- (void) const;

  /// \brief
  ///   Adds rhs component-wise to this vector.
  HKV_FORCE_INLINE void operator+= (const hkvVec4& rhs);

  /// \brief
  ///   Subtracts rhs component-wise from this vector.
  HKV_FORCE_INLINE void operator-= (const hkvVec4& rhs);

  /// \brief
  ///   Adds f to every component of this vector.
  HKV_FORCE_INLINE void operator+= (float f);

  /// \brief
  ///   Subtracts f from every component of this vector.
  HKV_FORCE_INLINE void operator-= (float f);

  /// \brief
  ///   Multiplies every component of this vector by f.
  HKV_FORCE_INLINE void operator*= (float f);

  /// \brief
  ///   Divides every component of this vector by f.
  HKV_FORCE_INLINE void operator/= (float f);

  ///
  /// @}
  ///

  ///
  /// @name Utility Functions
  /// @{
  ///

  /// \brief
  ///   Sets this vector to the linear interpolation of lhs and rhs using t as the interpolation factor.
  HKV_FORCE_INLINE void setInterpolate (const hkvVec4& lhs, const hkvVec4& rhs, float t);

  ///
  /// @}
  ///



public:
  // *** Data ***

  union
  {
    /// \brief
    ///   The components of this vector as XYZW variables.
    struct { float x, y, z, w; };

    /// \brief
    ///   The components of this vector as RGBA variables.
    struct { float r, g, b, a; };

    /// \brief
    ///   The components of this vector as a contiguous array.
    float data[4];
  };
};

V_DECLARE_SERIALX_NONINTRUSIVE (hkvVec4, VBASE_IMPEXP);

///
/// @name External Operators for hkvVec4
/// @{
///

/// \brief
///   Returns true if lhs and rhs are identical.
HKV_FORCE_INLINE bool operator== (const hkvVec4& lhs, const hkvVec4& rhs);

/// \brief
///   Returns true if lhs and rhs are not identical.
HKV_FORCE_INLINE bool operator!= (const hkvVec4& lhs, const hkvVec4& rhs);

/// \brief
///   Returns a temporary vector that is the sum of lhs and rhs.
HKV_FORCE_INLINE const hkvVec4 operator+ (const hkvVec4& lhs, const hkvVec4& rhs);

/// \brief
///   Returns a temporary vector that is the subtraction of lhs and rhs.
HKV_FORCE_INLINE const hkvVec4 operator- (const hkvVec4& lhs, const hkvVec4& rhs);

/// \brief
///   Returns a temporary vector that is lhs multiplied by f.
HKV_FORCE_INLINE const hkvVec4 operator* (const hkvVec4& lhs, float f);

/// \brief
///   Returns a temporary vector that is lhs multiplied by f.
HKV_FORCE_INLINE const hkvVec4 operator* (float f, const hkvVec4& rhs);

/// \brief
///   Returns a temporary vector that is lhs divided by f.
HKV_FORCE_INLINE const hkvVec4 operator/ (const hkvVec4& lhs, float f);

///
/// @}
///

#include <Vision/Runtime/Base/Math/Vector/hkvVec4.inl>

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
