/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVVEC3_H
#define VBASE_HKVMATH_HKVVEC3_H

#include <Vision/Runtime/Base/Math/hkvMath.h>

/// \brief
///   Serializes the vector to/from the archive (reads/writes exactly 3 floats).
VBASE_IMPEXP void SerializeX (VArchive& ar, hkvVec3& obj);

/// \brief 
///   This class represents a 3-component vector.
///
/// When interacting with 4x4 matrices the 4th component is by default considered to be 1,
/// thus the vector acts like a point in 3D space by default.
/// However, there are additional functions to handle the vector like a direction,
/// ie. to consider the 4th component to be 0, or to specify the w-component manually.
class hkvVec3
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
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvVec3 () : x (0.0f), y (0.0f), z (0.0f) {}

  #else

    /// \brief
    ///   FUTURE BEHAVIOR: Keeps the vector uninitialized.
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvVec3 () {}
  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvVec3 (hkvInit_None init /* = hkvNoInitialization */) { }

  /// \brief
  ///   Initializes all components of the vector to the same value.
  HKV_FORCE_INLINE explicit hkvVec3 (float xyz);

  /// \brief
  ///   Initializes each component individually.
  HKV_FORCE_INLINE hkvVec3 (float _x, float _y, float _z);

  /// \brief
  ///   Initializes this vector from a hkvVec3d. This constructor is explicit to prevent accidentally converting to a type with lower precision.
  VBASE_IMPEXP explicit hkvVec3 (const hkvVec3d& rhs);

  ///
  /// @}
  ///

  ///
  /// @name Static Functions for Default Vectors
  /// @{
  ///

  /// \brief
  ///   Returns a zero vector.
  HKV_FORCE_INLINE static const hkvVec3 ZeroVector () { return hkvVec3 (0.0f); }

  ///
  /// @}
  ///


  ///
  /// @name Conversions
  /// @{
  ///

  /// \brief
  ///   Returns a hkvVec4 with the same values for x,y,z and w set to 1.
  ///
  /// \sa hkvVec3::getAsDirection
  /// \sa hkvVec3::getAsVec4
  HKV_FORCE_INLINE const hkvVec4 getAsPosition () const;
  
  /// \brief
  ///   Returns a hkvVec4 with the same values for x,y,z and w set to 0.
  ///
  /// \sa hkvVec3::getAsPosition
  /// \sa hkvVec3::getAsVec4
  HKV_FORCE_INLINE const hkvVec4 getAsDirection () const;
  
  /// \brief
  ///   Returns a hkvVec2 with the same values for x and y.
  HKV_FORCE_INLINE const hkvVec2 getAsVec2 () const;

  /// \brief
  ///   Returns a hkvVec4 with the same values for x,y,z and w set _w.
  ///
  /// \param _w 
  ///   The value that will be stored in the w-component of the hkvVec4.
  ///
  /// \sa hkvVec3::getAsPosition
  /// \sa hkvVec3::getAsVec4
  HKV_FORCE_INLINE const hkvVec4 getAsVec4 (float _w) const;

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
  HKV_FORCE_INLINE void set (float xyz);

  /// \brief
  ///   Sets all components to their given value.
  HKV_FORCE_INLINE void set (float _x, float _y, float _z);

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
  ///   Must be in range [0;2]
  HKV_FORCE_INLINE float operator[] (hkUint32 i) const;

  /// \brief
  ///   Allows to access the vectors components through an index.
  ///
  /// \param i
  ///   Must be in range [0;2]
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
  HKV_FORCE_INLINE bool isIdentical (const hkvVec3& rhs) const;

  /// \brief
  ///   Checks whether this vector and rhs are equal within some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvVec3& rhs, float fEpsilon) const;

  /// \brief
  ///   Checks whether all components are valid floats.
  ///
  ///   If any component is +/- infinity or NaN, false will be returned.
  ///
  /// \sa hkvMath::isFiniteNumber
  HKV_FORCE_INLINE bool isValid () const;

  /// \brief
  ///  Checks whether this vector has a length of 1, within some epsilon range.
  ///
  /// \param fEpsilon
  ///   The maximum deviation that is allowed. The default value is reasonably large to handle the most common cases.
  ///
  /// \sa hkvMath::isFloatEqual
  HKV_FORCE_INLINE bool isNormalized (float fEpsilon = HKVMATH_LARGE_EPSILON) const;

  ///
  /// @}
  ///

  ///
  /// @name Length & Distance
  /// @{
  ///

  /// \brief
  ///   Returns the dot product of this vector and rhs.
  HKV_FORCE_INLINE float dot (const hkvVec3& rhs) const;

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
  /// \sa hkvVec3::getLengthSquared
  /// \sa hkvVec3::getLengthInverse
  /// \sa hkvVec3::getLengthAndNormalize
  /// \sa hkvVec3::getDistanceTo
  /// \sa hkvVec3::getDistanceToSquared
  HKV_FORCE_INLINE float getLength () const;

  /// \brief
  ///   Returns the squared length of this vector.
  ///
  /// It is more efficient to use the squared length, instead of the real length,
  /// if one only wants to compare vectors for their relative length 
  ///
  /// \sa hkvVec3::dot
  /// \sa hkvVec3::getLength
  /// \sa hkvVec3::getDistanceToSquared
  HKV_FORCE_INLINE float getLengthSquared () const;

  /// \brief
  ///   Returns ( 1.0f / getLength () ), but computes this more efficiently.
  ///
  /// If the length is supposed to be used for normalization purposes, prefer this function.
  ///
  /// \sa hkvVec3::getLength
  HKV_FORCE_INLINE float getLengthInverse () const;

  /// \brief
  ///   Returns the current length and then normalizes this vector. More efficient than calling 'getLength' and then 'normalize'.
  ///
  /// This function will only compute the length once, whereas 'getLength' followed by 'normalize' will do this twice.
  ///
  /// \sa hkvVec3::getLength
  /// \sa hkvVec3::getLengthInverse
  /// \sa hkvVec3::normalize
  HKV_FORCE_INLINE float getLengthAndNormalize ();

  /// \brief
  ///   Normalizes this vector, e.g. makes sure its length is 1.
  ///
  /// If the vector was zero or degenerate before, it will have invalid data afterwards.
  /// Normalizing zero-length vectors will give undefined results and may be very different on each platform.
  ///
  /// \sa hkvVec3::getLength
  /// \sa hkvVec3::getLengthInverse
  /// \sa hkvVec3::getNormalized
  /// \sa hkvVec3::normalizeIfNotZero
  /// \sa hkvVec3::normalizedEnsureUnitLength
  HKV_FORCE_INLINE void normalize ();

  /// \brief
  ///   Returns a normalized copy of this vector.
  ///
  /// The behavior is identical to normalize.
  ///
  /// \sa hkvVec3::normalize
  HKV_FORCE_INLINE const hkvVec3 getNormalized () const;

  /// \brief
  ///   Normalizes the vector, if its length is non-zero. Otherwise the vector will stay unmodified.
  ///
  /// If normalization was not possible, it returns HKV_FAILURE otherwise HKV_SUCCESS.
  /// In case of failure the vector will not be modified. That means the vector will either be all zero
  /// (and thus zero-length) or some component is invalid (+/-Infinity or NaN).
  ///
  /// \param fEpsilon If all values are in range [-fEpsilon; +fEpsilon] the vector is considered to be 'zero'.
  ///
  /// \sa hkvVec3::normalize
  /// \sa hkvVec3::normalizedEnsureUnitLength
  HKV_FORCE_INLINE hkvResult normalizeIfNotZero (float fEpsilon = HKVMATH_SMALL_EPSILON);

  /// \brief
  ///   Normalizes this vector under all circumstances and returns its previous length.
  ///
  /// If the vector can not be normalized, its x component will be set to 1 and all other components to 0.
  /// In this case the function will return 0, as the vectors previous length was 0 (or invalid).
  /// If the vector can be normalized, the previous length is returned.
  ///
  /// \sa hkvVec3::normalize
  /// \sa hkvVec3::normalizeIfNotZero
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
  /// \sa hkvVec3::getDistanceToSquared
  /// \sa hkvVec3::getLength
  HKV_FORCE_INLINE float getDistanceTo (const hkvVec3& rhs) const;

  /// \brief
  ///   Returns the squared distance between this vector and rhs.
  ///
  /// This is more efficient to use than getLength, if only relative comparisons are needed.
  /// E.g. to check whether the distance between v1 and v2 is larger than between v1 and v3.
  ///
  /// \param rhs
  ///   The point to which to compute the distance.
  ///
  /// \sa hkvVec3::getDistanceTo
  /// \sa hkvVec3::getLengthSquared
  HKV_FORCE_INLINE float getDistanceToSquared (const hkvVec3& rhs) const;

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
  /// \sa hkvVec3::normalizeIfNotZero
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
  HKV_FORCE_INLINE const hkvVec3 compMul (const hkvVec3& rhs) const;

  /// \brief
  ///   Returns a vector that is the component-wise division of this vector and rhs.
  HKV_FORCE_INLINE const hkvVec3 compDiv (const hkvVec3& rhs) const;


  /// \brief
  ///   Each component of this vector is set to the minimum of itself and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMin (const hkvVec3& rhs);

  /// \brief
  ///   Each component of this vector is set to the maximum of itself and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMax (const hkvVec3& rhs);

  /// \brief
  ///   Each component of this vector clamped to the min/max range defined by MinVal and MaxVal.
  HKV_FORCE_INLINE void clampTo (const hkvVec3& MinVal, const hkvVec3& MaxVal);


  /// \brief
  ///   Each component of this vector is set to the minimum of lhs and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMin (const hkvVec3& lhs, const hkvVec3& rhs);

  /// \brief
  ///   Each component of this vector is set to the maximum of lhs and the corresponding component of rhs.
  HKV_FORCE_INLINE void setMax (const hkvVec3& lhs, const hkvVec3& rhs);

  /// \brief
  ///   Each component of this vector is set to val clamped to the min/max range defined by MinVal and MaxVal.
  HKV_FORCE_INLINE void setClamped (const hkvVec3& val, const hkvVec3& MinVal, const hkvVec3& MaxVal);

  /// \brief
  ///   All components of this vector are set to the absolute value of the corresponding component of val.
  HKV_FORCE_INLINE void setAbs (const hkvVec3& val);

  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///

  /// \brief
  ///   Returns a negated copy of this vector.
  HKV_FORCE_INLINE const hkvVec3 operator- () const;

  /// \brief
  ///   Adds rhs component-wise to this vector.
  HKV_FORCE_INLINE void operator+= (const hkvVec3& rhs);

  /// \brief
  ///   Subtracts rhs component-wise from this vector.
  HKV_FORCE_INLINE void operator-= (const hkvVec3& rhs);

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
  ///   Returns the cross product of this vector and rhs, using a right-handed coordinate system.
  ///
  /// The cross product is not commutative, that means 
  /// v1.cross (v2) will yield a different result than
  /// v2.cross (v1) (it will be the negated vector).
  ///
  /// \param rhs
  ///   The second vector to compute the cross product with.
  ///
  /// \sa hkvVec3::setCross
  HKV_FORCE_INLINE const hkvVec3 cross (const hkvVec3& rhs) const;

  /// \brief
  ///   Sets this vector to the cross product of lhs and rhs.
  ///
  /// See 'cross' for details.
  ///
  /// \param lhs
  ///   The first vector to compute the cross product with.
  ///
  /// \param rhs
  ///   The second vector to compute the cross product with.
  ///
  /// \sa hkvVec3::cross
  HKV_FORCE_INLINE void setCross (const hkvVec3& lhs, const hkvVec3& rhs);

  /// \brief
  ///   Sets this vector to the linear interpolation of lhs and rhs using t as the interpolation factor.
  HKV_FORCE_INLINE void setInterpolate (const hkvVec3& lhs, const hkvVec3& rhs, float t);

  /// \brief
  ///   Makes this vector orthogonal to vNormal if possible. 
  ///
  /// \param vNormal
  ///   must be normalized. In Debug mode an assert will fire if it isn't.
  ///
  /// This vector will not be normalized afterwards.
  /// If this vector and vNormal are parallel, the resulting vector will be the zero vector.
  ///
  /// \sa hkvVec3::getOrthogonalVector
  HKV_FORCE_INLINE void makeOrthogonalTo (const hkvVec3& vNormal);

  /// \brief
  ///   Returns some arbitrary vector that is orthogonal to this vector.
  ///
  /// There is no guarantee into which direction the resulting vector will point, other
  /// than that it will be orthogonal to this vector.
  ///
  /// \sa hkvVec3::makeOrthogonalTo
  HKV_FORCE_INLINE const hkvVec3 getOrthogonalVector () const;

  /// \brief
  ///   Reflects this vector at the plane that goes through the origin with the given normal.
  ///
  /// \param vNormal 
  ///   The normal of a plane through the origin, which will be used to reflect this vector.
  ///
  /// \sa hkvVec3::getReflected
  HKV_FORCE_INLINE void reflect (const hkvVec3& vNormal);

  /// \brief
  ///   Returns a vector that is reflected at the plane that goes through the origin with the given normal.
  ///
  /// \param vNormal
  ///   The normal of a plane through the origin, which will be used to reflect this vector.
  ///
  /// \sa hkvVec3::reflect
  HKV_FORCE_INLINE const hkvVec3 getReflected (const hkvVec3& vNormal) const;


  /// \brief
  ///   Returns the angle (in degree) between the two vectors. The vectors need not be normalized.
  HKV_FORCE_INLINE float getAngleBetween (const hkvVec3& rhs) const;

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   Will serialize the hkvVec3 to/from an archive by reading/writing 3 floats.
  ///
  /// The function will check whether the archive is opened for loading or saving and will then
  /// read or write the data accordingly.
  /// Prefer this function for new code (meaning new file formats), as it does not store any unneeded data.
  /// If however you have replaced a VisVector_cl by hkvVec3, then serialization should be done as the VisVector_cl
  /// did it, meaning you need to read and write 4 floats instead to keep your code compatible to your saved files.
  /// In this case use SerializeAsVisVector instead.
  ///
  /// \param ar
  ///   The archive that this vector should be serialized to / from.
  ///
  /// \sa hkvVec3::SerializeAsVisVector
  HKV_FORCE_INLINE void SerializeAsVec3 (VArchive& ar) { SerializeX (ar, *this); }

  /// \brief
  ///   Will serialize the hkvVec3 to/from an archive by reading/writing 4 floats.
  ///
  /// The function will check whether the archive is opened for loading or saving and will then
  /// read or write the data accordingly.
  ///
  /// \param ar
  ///   The archive to serialize the vector to / from.
  ///
  /// \param fWriteW
  ///   The 4th value that is written to the archive (ignored when reading data).
  ///   The parameter is only relevant if there is other code that might still rely on a certain value
  ///   to be stored in the w-component (such as 0 or 1).
  ///
  /// Prefer SerializeAsVec3 for new code (meaning new file formats), as it does not store any unneeded data.
  /// If however you have replaced a VisVector_cl by hkvVec3, then serialization should be done as the VisVector_cl
  /// did it, meaning you need to read and write 4 floats instead to keep your code compatible to your saved files.
  /// In this case use this function.
  /// In the long run try to get rid of this function and use SerializeAsVec3 everywhere instead.
  ///
  /// \sa hkvVec3::SerializeAsVec3
  VBASE_IMPEXP void SerializeAsVisVector (VArchive& ar, float fWriteW = 1.0f);

  ///
  /// @}
  ///

public:
  // *** Data ***

  union
  {
    /// \brief
    ///   The components of this vector as different variables.
    struct { float x, y, z; };

    /// \brief
    ///   The components of this vector as RGB variables.
    struct { float r, g, b; };

    /// \brief
    ///   The components of this vector as a contiguous array.
    float data[3];
  };

public:

  ///
  /// @name Compatibility Interface (DEPRECATED)
  /// @{
  ///

  /// \brief DEPRECATED: DO NOT USE!
  HKVMATH_DEPRECATED_STAGE4 typedef float COMPONENT_TYPE;

  /// \brief DEPRECATED: Use hkvNoInitialization instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE explicit hkvVec3 (bool b) { }

  /// \brief DEPRECATED: Use floats for initialization instead.
  HKVMATH_DEPRECATED_STAGE4 HKV_FORCE_INLINE explicit hkvVec3 (hkInt32 xyz) { set ((float) xyz); } 

  /// \brief DEPRECATED: Use hkvVec3::set instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Set (float _x, float _y, float _z) { set (_x, _y, _z); }

  /// \brief DEPRECATED: Use hkvVec3::normalizeIfNotZero instead. Be careful to check the result for HKV_SUCCESS instead of TRUE.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE VBool Normalize () { return (normalizeIfNotZero () == HKV_SUCCESS) ? TRUE : FALSE; }

  /// \brief DEPRECATED: Use hkvVec3::set instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void SetValue (float _x, float _y, float _z) { set (_x, _y, _z); }

  /// \brief DEPRECATED: Instead of 'A.Cross (B)' use 'A = A.cross (B)'.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Cross (const hkvVec3& rhs) { *this = cross (rhs); }

  /// \brief DEPRECATED: Use hkvVec3::setLength instead. Be careful to check the result for HKV_SUCCESS instead of true.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE bool SetLength (float len) { return (setLength (len) == HKV_SUCCESS); }

  /// \brief DEPRECATED: Use hkvVec3::isZero instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE bool IsZero () const { return isZero (0.0f); }

  /// \brief DEPRECATED: Use hkvVec3::getLength instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetLength () const { return getLength (); }

  /// \brief DEPRECATED: Use hkvVec3::dot instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float Dot (const hkvVec3& rhs) const { return dot (rhs); }

  /// \brief DEPRECATED: Use hkvVec3::getLength instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float Length () const { return getLength (); }

  /// \brief DEPRECATED: Use hkvVec3::getLengthSquared instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float LengthSqr () const { return getLengthSquared (); }

  /// \brief DEPRECATED: Use hkvVec3::getDistanceTo instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float Dist (const hkvVec3& v2) const { return getDistanceTo (v2); }

  /// \brief DEPRECATED: Use hkvVec3::getDistanceToSquared instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float DistSqr (const hkvVec3& v2) const { return getDistanceToSquared (v2); }

  /// \brief
  ///   DEPRECATED: DO NOT USE IN NEW CODE!
  ///
  /// Use hkvVec3::makeOrthogonalTo instead.
  /// However be careful because makeOrthogonalTo is implemented differently than MakeOrthogonal.
  /// The current inline implementation of MakeOrthogonal is:\n
  /// void MakeOrthogonal (hkvVec3& v) const { v.makeOrthogonalTo (*this); }\n
  ///
  /// \param v
  ///   DEPRECATED
  ///
  /// \sa hkvVec3::makeOrthogonalTo
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void MakeOrthogonal (hkvVec3& v) const { v.makeOrthogonalTo (*this); }

  /// \brief DEPRECATED: Use hkvVec3::isIdentical instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE bool IsEqual (const hkvVec3& rhs) const { return isIdentical (rhs); }

  /// \brief DEPRECATED: Use hkvVec3::isEqual instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE bool IsEqual (const hkvVec3& rhs, float eps) const { return isEqual (rhs, eps); }

  /// \brief
  ///   DEPRECATED: DO NOT USE IN NEW CODE!
  ///
  /// Replace this function by creating a hkvMat4 from pfMatrixScalars (using row-major data layout)
  /// and then multiply this vector by that matrix.
  ///
  /// \param pfMatrixScalars
  ///   DEPRECATED
  ///
  /// \sa hkvMat4::set4x4RowMajor
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void MultiplyWith4x4Matrix (const float* pfMatrixScalars)
  {
    float oldX=x, oldY=y, oldZ=z;
    x = oldX*pfMatrixScalars[0]+oldY*pfMatrixScalars[1]+oldZ*pfMatrixScalars[2]+pfMatrixScalars[3];
    y = oldX*pfMatrixScalars[4]+oldY*pfMatrixScalars[5]+oldZ*pfMatrixScalars[6]+pfMatrixScalars[7];
    z = oldX*pfMatrixScalars[8]+oldY*pfMatrixScalars[9]+oldZ*pfMatrixScalars[10]+pfMatrixScalars[11];
  }

  /// \brief DEPRECATED: Use hkvVec3::getReflected instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE const hkvVec3 Reflect (const hkvVec3& normal) const { return getReflected (normal); }

  /// \brief DEPRECATED: Use hkvVec3::set instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void SetXYZ (float _x, float _y, float _z) { set (_x, _y, _z); }

  /// \brief DEPRECATED: Use hkvVec3::getLength instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetLen () const { return getLength (); }

  /// \brief DEPRECATED: Use hkvVec3::setZero instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Clear () { setZero (); }

  /// \brief DEPRECATED: Use hkvVec3::setLength instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void SetLen (float len) { setLength (len); }

  /// \brief DEPRECATED: Use hkvVec3::setMin instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Min (const hkvVec3& v) { setMin (v); }

  /// \brief DEPRECATED: Use hkvVec3::setMax instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Max (const hkvVec3& v) { setMax (v); }

  /// \brief DEPRECATED: Use hkvVec3::cross instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE const hkvVec3 GetCrossProduct (const hkvVec3& rhs) const { return cross (rhs); }

  /// \brief DEPRECATED: Use hkvVec3::isIdentical instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE bool Equals (const hkvVec3& rhs) const { return isIdentical (rhs); }

  /// \brief DEPRECATED: Use hkvVec3::isEqual instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE bool Equals (const hkvVec3& rhs, float eps) const { return isEqual (rhs, eps); }

  /// \brief DEPRECATED: Use hkvVec3::getLengthSquared instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float GetSquaredLen () const { return getLengthSquared (); }

  /// \brief DEPRECATED: Use hkvPlane::getMirrored instead.
  HKVMATH_DEPRECATED_STAGE2 VBASE_IMPEXP const hkvVec3 Mirror (const hkvPlane& p) const;

  /// \brief
  ///   DEPRECATED: DO NOT USE IN NEW CODE!
  ///
  /// Use hkvMat4::transformDirection instead.\n
  /// E.g. 'hkvVec3 v1 = m.transformDirection (v2)'\n
  ///
  /// \param m
  ///   DEPRECATED
  ///
  /// \sa hkvMat4::transformDirection
  HKVMATH_DEPRECATED_STAGE2 VBASE_IMPEXP void MultiplyWithRotationalPart (const hkvMat4& m);

  /// \brief
  ///   DEPRECATED: DO NOT USE IN NEW CODE!
  ///
  /// Instead transpose the matrix manually and multiply the vector with that.\n
  /// The compatibility implementation of MultiplyWithTransposedMatrix is:\n
  ////  void hkvVec3::MultiplyWithTransposedMatrix (const hkvMat3& m) {\n
  ///     hkvMat3 mT = m.getTransposed (); \n
  ///     *this = mT.transformDirection (*this); \n
  ///   }\n
  ///
  /// \param m
  ///   DEPRECATED
  ///
  /// \sa hkvMat3::getTransposed
  /// \sa hkvMat3::transpose
  /// \sa hkvMat3::transformDirection
  HKVMATH_DEPRECATED_STAGE2 VBASE_IMPEXP void MultiplyWithTransposedMatrix (const hkvMat3& m);

  /// \brief DEPRECATED: Use hkvMat3::transformDirection instead.
  HKVMATH_DEPRECATED_STAGE2 VBASE_IMPEXP void operator*= (const hkvMat3& m);

  /// \brief DEPRECATED: Use hkvMat4::transformPosition instead.
  HKVMATH_DEPRECATED_STAGE2 VBASE_IMPEXP void operator*= (const hkvMat4& m);

  /// \brief DEPRECATED: Use hkvVec3::setZero instead.
  HKVMATH_DEPRECATED_STAGE3 VBASE_IMPEXP void Zero () { setZero (); }

  /// \brief DEPRECATED: Use compMul instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE const hkvVec3 ComponentMul (const hkvVec3& vec) const { return compMul (vec); }

  /// \brief DEPRECATED: Use hkvPlane::getPointPosition instead.
  HKVMATH_DEPRECATED_STAGE2 VBASE_IMPEXP bool IsOnPlane (const hkvPlane& p) const;

  ///
  /// @}
  ///

  // Missing Functions:

  // VDIRECTION GetDirection();
  // void MultiplyWithRotationalPart(const float *pfMatrixScalars)
};

///
/// @name External Operators for hkvVec3
/// @{
///

/// \brief
///   Returns true if lhs and rhs are identical.
HKV_FORCE_INLINE bool operator== (const hkvVec3& lhs, const hkvVec3& rhs);

/// \brief
///   Returns true if lhs and rhs are not identical.
HKV_FORCE_INLINE bool operator!= (const hkvVec3& lhs, const hkvVec3& rhs);

/// \brief
///   Returns a temporary vector that is the sum of lhs and rhs.
HKV_FORCE_INLINE const hkvVec3 operator+ (const hkvVec3& lhs, const hkvVec3& rhs);

/// \brief
///   Returns a temporary vector that is the subtraction of lhs and rhs.
HKV_FORCE_INLINE const hkvVec3 operator- (const hkvVec3& lhs, const hkvVec3& rhs);

/// \brief
///   Returns a temporary vector that is lhs multiplied by f.
HKV_FORCE_INLINE const hkvVec3 operator* (const hkvVec3& lhs, float f);

/// \brief
///   Returns a temporary vector that is lhs multiplied by f.
HKV_FORCE_INLINE const hkvVec3 operator* (float f, const hkvVec3& rhs);

/// \brief
///   Returns a temporary vector that is lhs divided by f.
HKV_FORCE_INLINE const hkvVec3 operator/ (const hkvVec3& lhs, float f);

///
/// @}
///

#ifdef HKVMATH_ENABLE_NEW_SERIALIZATION_OPERATORS
  // Currently deactivated to force people to use SerializeAsVec3 or SerializeAsVisVector instead.
  // Will be activated in some later release.
  V_DECLARE_SERIALX_NONINTRUSIVE (hkvVec3, VBASE_IMPEXP);
#endif


#include <Vision/Runtime/Base/Math/Vector/hkvVec3.inl>

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
