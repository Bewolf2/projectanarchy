/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVVEC2_H
#define VBASE_HKVMATH_HKVVEC2_H

#include <Vision/Runtime/Base/Math/hkvMath.h>

/// \brief
///   This class represents a 2-component vector.
class hkvVec2
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
    /// Prefer to not initialize the vector by using hkvNoInitialization.
    /// Note: At some point the Vision Engine will deactivate this old behavior and use the uninitialized version instead.
    /// At that time you need to make sure that whenever you default construct a vector, you do not rely on it being zero.
    ///
    /// You can find all the places where you use the default constructor by defining 
    /// HKVMATH_DEPRECATE_DEFAULT_CONSTRUCTOR in hkvMathConfig.h and compiling your code for Windows.
    /// Then the compiler will generate a warning for every location where you use the default constructor.
    /// Use the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION to prevent that warning
    /// for code that cannot be changed to use a non default constructor (such as for arrays).
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvVec2 () : x (0.0f), y (0.0f) {}

  #else

    /// \brief
    ///   FUTURE BEHAVIOR: Keeps the vector uninitialized.
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvVec2 () {}
  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvVec2 (hkvInit_None init /* = hkvNoInitialization */) { }

  /// \brief
  ///   Initializes all components of the vector to the same value.
  HKV_FORCE_INLINE explicit hkvVec2 (float xy);

  /// \brief
  ///   Initializes each component individually.
  HKV_FORCE_INLINE hkvVec2 (float _x, float _y);

  ///
  /// @}
  ///

  ///
  /// @name Static Functions for Default Vectors
  /// @{
  ///

  /// \brief
  ///   Returns a zero vector.
  HKV_FORCE_INLINE static const hkvVec2 ZeroVector () { return hkvVec2 (0.0f); }

  ///
  /// @}
  ///

  ///
  /// @name Conversions
  /// @{
  ///

  /// \brief
  ///   Returns a hkvVec3 with the same values for x and y, and z set to _z.
  ///
  /// \param _z
  ///   The value that will be stored in the z-component of the hkvVec3.
  ///
  /// \sa hkvVec2::getAsVec4
  HKV_FORCE_INLINE const hkvVec3 getAsVec3 (float _z) const;

  /// \brief
  ///   Returns a hkvVec4 with the same values for x and y, z set to _z and w set to _w.
  ///
  /// \param _z
  ///   The value that will be stored in the z-component of the hkvVec4.
  ///
  /// \param _w
  ///   The value that will be stored in the w-component of the hkvVec4.
  ///
  /// \sa hkvVec2::getAsVec3
  HKV_FORCE_INLINE const hkvVec4 getAsVec4 (float _z, float _w) const;

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
  HKV_FORCE_INLINE void set (float xy);

  /// \brief
  ///   Sets all components to their given value.
  HKV_FORCE_INLINE void set (float _x, float _y);

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
  ///   Must be in range [0;1]
  HKV_FORCE_INLINE float operator[] (hkUint32 i) const;

  /// \brief
  ///   Allows to access the vectors components through an index.
  ///
  /// \param i 
  ///   Must be in range [0;1]
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
  HKV_FORCE_INLINE bool isIdentical (const hkvVec2& rhs) const;

  /// \brief
  ///   Checks whether this vector and rhs are equal within some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvVec2& rhs, float fEpsilon) const;

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
  HKV_FORCE_INLINE float dot (const hkvVec2& rhs) const;

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
  /// \sa hkvVec2::getLengthSquared
  /// \sa hkvVec2::getLengthInverse
  /// \sa hkvVec2::getLengthAndNormalize
  /// \sa hkvVec2::getDistanceTo
  /// \sa hkvVec2::getDistanceToSquared
  HKV_FORCE_INLINE float getLength () const;

  /// \brief
  ///   Returns the squared length of this vector.
  ///
  /// It is more efficient to use the squared length, instead of the real length,
  /// if one only wants to compare vectors for their relative length 
  ///
  /// \sa hkvVec2::dot
  /// \sa hkvVec2::getLength
  /// \sa hkvVec2::getDistanceToSquared
  HKV_FORCE_INLINE float getLengthSquared () const;

  /// \brief
  ///   Returns ( 1.0f / getLength () ), but computes this more efficiently.
  ///
  /// If the length is supposed to be used for normalization purposes, prefer this function.
  ///
  /// \sa hkvVec2::getLength
  HKV_FORCE_INLINE float getLengthInverse () const;

  /// \brief
  ///   Returns the current length and then normalizes this vector. More efficient than calling 'getLength' and then 'normalize'.
  ///
  /// This function will only compute the length once, whereas 'getLength' followed by 'normalize' will do this twice.
  ///
  /// \sa hkvVec2::getLength
  /// \sa hkvVec2::getLengthInverse
  /// \sa hkvVec2::normalize
  HKV_FORCE_INLINE float getLengthAndNormalize ();

  /// \brief
  ///   Normalizes this vector, e.g. makes sure its length is 1.
  ///
  /// If the vector was zero or degenerate before, it will have invalid data afterwards.
  /// Normalizing zero-length vectors will give undefined results and may be very different on each platform.
  ///
  /// \sa hkvVec2::getLength
  /// \sa hkvVec2::getLengthInverse
  /// \sa hkvVec2::getNormalized
  /// \sa hkvVec2::normalizeIfNotZero
  /// \sa hkvVec2::normalizedEnsureUnitLength
  HKV_FORCE_INLINE void normalize ();

  /// \brief
  ///   Returns a normalized copy of this vector.
  ///
  /// The behavior is identical to normalize.
  ///
  /// \sa hkvVec2::normalize
  HKV_FORCE_INLINE const hkvVec2 getNormalized () const;

  /// \brief
  ///   Normalizes the vector, if its length is non-zero. Otherwise the vector will stay unmodified.
  ///
  /// If normalization was not possible, it returns HKV_FAILURE otherwise HKV_SUCCESS.
  /// In case of failure the vector will not be modified. That means the vector will either be all zero
  /// (and thus zero-length) or some component is invalid (+/-Infinity or NaN).
  ///
  /// \param fEpsilon If all values are in range [-fEpsilon; +fEpsilon] the vector is considered to be 'zero'.
  ///
  /// \sa hkvVec2::normalize
  /// \sa hkvVec2::normalizedEnsureUnitLength
  HKV_FORCE_INLINE hkvResult normalizeIfNotZero (float fEpsilon = HKVMATH_SMALL_EPSILON);

  /// \brief
  ///   Normalizes this vector under all circumstances and returns its previous length.
  ///
  /// If the vector can not be normalized, its x component will be set to 1 and all other components to 0.
  /// In this case the function will return 0, as the vectors previous length was 0 (or invalid).
  /// If the vector can be normalized, the previous length is returned.
  ///
  /// \sa hkvVec2::normalize
  /// \sa hkvVec2::normalizeIfNotZero
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
  /// \sa hkvVec2::getDistanceToSquared
  /// \sa hkvVec2::getLength
  HKV_FORCE_INLINE float getDistanceTo (const hkvVec2& rhs) const;

  /// \brief
  ///   Returns the squared distance between this vector and rhs.
  ///
  /// This is more efficient to use than getLength, if only relative comparisons are needed.
  /// E.g. to check whether the distance between v1 and v2 is larger than between v1 and v3.
  ///
  /// \param rhs
  ///   The point to which to compute the distance.
  ///
  /// \sa hkvVec2::getDistanceTo
  /// \sa hkvVec2::getLengthSquared
  HKV_FORCE_INLINE float getDistanceToSquared (const hkvVec2& rhs) const;

  /// \brief
  ///   Normalizes this vector and then multiplies it by f.
  ///
  /// Returns HKV_FAILURE, if the vector could not be normalized.
  /// In this case the vector does not change.
  /// In case of failure the function acts identical to normalizeIfNotZero.
  ///
  /// \param f
  ///   The length to which to set the vector to.
  /// \param fEpsilon
  ///   If the vector's original length is below this threshold normalization fails and thus the vector's length is not changed.
  ///
  /// \sa hkvVec2::normalizeIfNotZero
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
  HKV_FORCE_INLINE const hkvVec2 compMul (const hkvVec2& rhs) const;

  /// \brief
  ///   Returns a vector that is the component-wise division of this vector and rhs.
  HKV_FORCE_INLINE const hkvVec2 compDiv (const hkvVec2& rhs) const;


  /// \brief
  ///   Each component of this vector is set to the minimum of itself and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMin (const hkvVec2& rhs);

  /// \brief
  ///   Each component of this vector is set to the maximum of itself and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMax (const hkvVec2& rhs);

  /// \brief
  ///   Each component of this vector clamped to the min/max range defined by MinVal and MaxVal.
  HKV_FORCE_INLINE void clampTo (const hkvVec2& MinVal, const hkvVec2& MaxVal);

  /// \brief
  ///   Each component of this vector is set to the minimum of lhs and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMin (const hkvVec2& lhs, const hkvVec2& rhs);

  /// \brief
  ///   Each component of this vector is set to the maximum of lhs and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMax (const hkvVec2& lhs, const hkvVec2& rhs);

  /// \brief
  ///   Each component of this vector is set to val clamped to the min/max range defined by MinVal and MaxVal.
  HKV_FORCE_INLINE void setClamped (const hkvVec2& val, const hkvVec2& MinVal, const hkvVec2& MaxVal);

  /// \brief
  ///   All components of this vector are set to the absolute value of the corresponding component of val.
  HKV_FORCE_INLINE void setAbs (const hkvVec2& val);

  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///

  /// \brief
  ///   Returns a negated copy of this vector.
  HKV_FORCE_INLINE const hkvVec2 operator- () const;

  /// \brief
  ///   Adds rhs component-wise to this vector.
  HKV_FORCE_INLINE void operator+= (const hkvVec2& rhs);

  /// \brief
  ///   Subtracts rhs component-wise from this vector.
  HKV_FORCE_INLINE void operator-= (const hkvVec2& rhs);

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
  HKV_FORCE_INLINE void setInterpolate (const hkvVec2& lhs, const hkvVec2& rhs, float t);

  /// \brief
  ///   Makes this vector orthogonal to vNormal if possible. 
  ///
  /// \param vNormal
  ///   Must be normalized. In Debug mode an assert will fire if it isn't.
  ///
  /// This vector will not be normalized afterwards.
  /// If this vector and vNormal are parallel, the resulting vector will be the zero vector.
  ///
  /// \sa hkvVec3::getOrthogonalVector
  HKV_FORCE_INLINE void makeOrthogonalTo (const hkvVec2& vNormal);

  /// \brief
  ///   Returns some arbitrary vector that is orthogonal to this vector.
  ///
  /// There is no guarantee into which direction the resulting vector will point, other
  /// than that it will be orthogonal to this vector.
  ///
  /// \sa hkvVec3::makeOrthogonalTo
  HKV_FORCE_INLINE const hkvVec2 getOrthogonalVector () const;

  /// \brief
  ///   Reflects this vector at the plane that goes through the origin with the given normal.
  ///
  /// \param vNormal
  ///   The normal of a plane through the origin, which will be used to reflect this vector.
  ///
  /// \sa hkvVec3::getReflected
  HKV_FORCE_INLINE void reflect (const hkvVec2& vNormal);

  /// \brief
  ///   Returns a vector that is reflected at the plane that goes through the origin with the given normal.
  ///
  /// \param vNormal
  ///   The normal of a plane through the origin, which will be used to reflect this vector.
  ///
  /// \sa hkvVec3::reflect
  HKV_FORCE_INLINE const hkvVec2 getReflected (const hkvVec2& vNormal) const;

  ///
  /// @}
  ///

public:
  // *** Data ***

  union
  {
    /// \brief
    ///   The components of as XY coordinates.
    struct { float x, y; };

    /// \brief
    ///   The components of as UV coordinates.
    struct { float u, v; };

    /// \brief
    ///   The components of this vector as a contiguous array.
    float data[2];
  };

};

// Allows to serialize the hkvVec2 to a VArchiv using >> and <<
V_DECLARE_SERIALX_NONINTRUSIVE (hkvVec2, VBASE_IMPEXP);

///
/// @name External Operators for hkvVec2
/// @{
///

/// \brief
///   Returns true if lhs and rhs are identical.
HKV_FORCE_INLINE bool operator== (const hkvVec2& lhs, const hkvVec2& rhs);

/// \brief
///   Returns true if lhs and rhs are not identical.
HKV_FORCE_INLINE bool operator!= (const hkvVec2& lhs, const hkvVec2& rhs);

/// \brief
///   Returns a temporary vector that is the sum of lhs and rhs.
HKV_FORCE_INLINE const hkvVec2 operator+ (const hkvVec2& lhs, const hkvVec2& rhs);

/// \brief
///   Returns a temporary vector that is the subtraction of lhs and rhs.
HKV_FORCE_INLINE const hkvVec2 operator- (const hkvVec2& lhs, const hkvVec2& rhs);

/// \brief
///   Returns a temporary vector that is lhs multiplied by f.
HKV_FORCE_INLINE const hkvVec2 operator* (const hkvVec2& lhs, float f);

/// \brief
///   Returns a temporary vector that is lhs multiplied by f.
HKV_FORCE_INLINE const hkvVec2 operator* (float f, const hkvVec2& rhs);

/// \brief
///   Returns a temporary vector that is lhs divided by f.
HKV_FORCE_INLINE const hkvVec2 operator/ (const hkvVec2& lhs, float f);

///
/// @}
///

#include <Vision/Runtime/Base/Math/Vector/hkvVec2.inl>

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
