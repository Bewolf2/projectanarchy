/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVQUATERNION_H
#define VBASE_HKVMATH_HKVQUATERNION_H

#include <Vision/Runtime/Base/Math/hkvMath.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>

/// \brief A quaternion class that can be used to represent rotations.
class hkvQuat
{
public:

  #ifdef HKVMATH_DEFAULTCONSTRUCTORS_INITIALIZEDATA

    /// \brief
    ///   DEPRECATED: Initializes the quaternion to identity.
    ///
    /// Prefer to not initialize the quaternion by using hkvNoInitialization.
    /// Note: At some point the Vision Engine will deactivate this old behavior and use the uninitialized version instead.
    /// At that time you need to make sure that whenever you default construct a vector, you do not rely on it being zero.
    ///
    /// You can find all the places where you use the default constructor by defining 
    /// HKVMATH_DEPRECATE_DEFAULT_CONSTRUCTOR in hkvMathConfig.h and compiling your code for Windows.
    /// Then the compiler will generate a warning for every location where you use the default constructor.
    /// Use the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION to prevent that warning
    /// for code that cannot be changed to use a non default constructor (such as for arrays).
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvQuat () { setIdentity (); }

  #else

    /// \brief Default Constructor: Does not initialize the quaternion.
    HKV_FORCE_INLINE hkvQuat () { }

  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvQuat (hkvInit_None init /* = hkvNoInitialization */) { }

  /// \brief Static function that returns an identity quaternion (ie. no rotation).
  HKV_FORCE_INLINE static const hkvQuat IdentityQuaternion ();

  /// \brief Normalizes the quaternion to unit length. Only a normalized quaternion represents a valid rotation.
  HKV_FORCE_INLINE void normalize ();

  /// \brief Checks whether the quaternion is normalized withing some epsilon range.
  HKV_FORCE_INLINE bool isNormalized (float fEpsilon = HKVMATH_GIGANTIC_EPSILON) const;

  /// \brief Inverts the rotation that the quaternion represents.
  HKV_FORCE_INLINE void invert ();

  /// \brief Sets the quaternion to identity.
  HKV_FORCE_INLINE void setIdentity ();

  /// \brief Checks whether the quaternion is the identity quaternion.
  HKV_FORCE_INLINE bool isIdentity (float fEpsilon = HKVMATH_LARGE_EPSILON) const;

  /// \brief Returns the length of the quaternion. Will always be 1 for quaternions that represent valid rotations.
  HKV_FORCE_INLINE float getLength () const;

  /// \brief Returns the squared length of the quaternion.
  HKV_FORCE_INLINE float getLengthSquared () const;

  /// \brief Creates the quaternion from a 3x3 matrix.
  HKV_FORCE_INLINE void setFromMat3 (const hkvMat3& mRotation);

  /// \brief Sets the quaternion from a rotation axis and and angle.
  ///
  /// \param vAxis
  ///   The rotation axis. Must be normalized.
  ///
  /// \param fAngleDeg
  ///   The amount of rotation around the axis, given in degree.
  ///   The rotation will be counter-clockwise around the rotation axis.
  ///
  // /// \sa getAxisAndAngle
  HKV_FORCE_INLINE void setFromAxisAndAngle (const hkvVec3& vAxis, float fAngleDeg);

  /// \brief Creates a quaternion from euler angles.
  HKV_FORCE_INLINE void setFromEulerAngles (float fRollDeg, float fPitchDeg, float fYawDeg);

  /// \brief Returns euler angles which represent the same rotation as this quaternion.
  HKV_FORCE_INLINE void getAsEulerAngles (float& out_fRollDeg, float& out_fPitchDeg, float& out_fYawDeg);

  /// \brief Returns a 3x3 matrix that represents the same rotation as this quaternion.
  HKV_FORCE_INLINE const hkvMat3 getAsMat3 () const;

  /// \brief Returns a 4x4 matrix that represents the same rotation as this quaternion.
  HKV_FORCE_INLINE const hkvMat4 getAsMat4 () const;

  /// \brief Returns an inverted quaternion, ie. one that rotates into the opposite direction.
  HKV_FORCE_INLINE const hkvQuat operator- () const;

  /// \brief Returns the rotation axis and angle of this quaternion.
  ///
  /// \param out_vRotationAxis
  ///   The rotation axis is stored here. Will be normalized.
  ///
  /// \param out_fAngles
  ///   The rotation angle will be stored here. 
  ///   The rotation is supposed to be counter-clockwise around the rotation axis.
  ///
  /// \sa setFromAxisAndAngle
  HKV_FORCE_INLINE void getAxisAndAngle (hkvVec3& out_vRotationAxis, float& out_fAngles) const;

  /// \brief
  ///   Will check whether two quaternions are identical.
  ///
  /// Since a quaternion is equal to itself with all components negated, that will also
  /// be taken into account. Thus (x,y,z,w) will be considered equal to (-x,-y,-z,-w).
  /// Generally this function creates two matrices from the two quaternions and compares those for equality.
  /// This function should only be used in unit-tests.
  ///
  /// \param rhs      The right-hand side quaternion to check.
  HKV_FORCE_INLINE bool isIdentical (const hkvQuat& rhs) const;

  /// \brief
  ///   Will check whether two quaternions are equal within some epsilon range.
  ///
  /// Since a quaternion is equal to itself with all components negated, that will also
  /// be taken into account. Thus (x,y,z,w) will be considered equal to (-x,-y,-z,-w).
  /// Generally this function creates two matrices from the two quaternions and compares those for equality.
  /// This function should only be used in unit-tests.
  ///
  /// \param rhs      The right-hand side quaternion to check.
  /// \param fEpsilon The allowed epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvQuat& rhs, float fEpsilon) const;

  /// \brief Computes the 4-component dot-product between the two quaternions.
  HKV_FORCE_INLINE float dot (const hkvQuat& rhs) const;

  /// \brief Sets this quaternion to the shortest rotation quaternion that would rotate the direction vector 'vFrom' to 'vTo'.
  ///
  /// This function can be used to take two directions and create a quaternion that defines how to
  /// rotate the direction 'vFrom' such that it becomes 'vTo'.
  ///
  /// \param vFrom
  ///   Original direction vector. Must be normalized.
  ///
  /// \param vTo
  ///   Target direction vector. Must be normalized.
  VBASE_IMPEXP void setShortestRotation (const hkvVec3& vFrom, const hkvVec3& vTo);

  /// \brief Concatenates two rotations. This operation is not commutative.
  HKV_FORCE_INLINE const hkvQuat multiply (const hkvQuat& rhs) const;

  /// \brief Concatenates two rotations in the reverse order than 'multiply' does. This operation is not commutative.
  HKV_FORCE_INLINE const hkvQuat multiplyReverse (const hkvQuat& rhs) const;

  /// \brief Transforms (rotates) the given vector by this quaternion.
  HKV_FORCE_INLINE const hkvVec3 transform (const hkvVec3& rhs) const;

  /// \brief Transforms (rotates) the given vector by the inverse of this quaternion. Ie. rotates it into the opposite direction.
  HKV_FORCE_INLINE const hkvVec3 transformReverse (const hkvVec3& rhs) const;

  /// \brief Flips the sign of all four components.
  HKV_FORCE_INLINE void flipSigns ();

  /// \brief Sets the 4 components of this quaternion. Negates the w component. Useful when porting old, VisQuaternion_cl based, code to hkvQuat.
  HKV_FORCE_INLINE void setValuesDirect_Old (float inx, float iny, float inz, float negw) { x = inx; y = iny; z = inz; w = -negw; }

  /// \brief Sets the 4 components of this quaternion.
  HKV_FORCE_INLINE void setValuesDirect (float inx, float iny, float inz, float inw) { x = inx; y = iny; z = inz; w = inw; }

  /// \brief Returns the x component.
  HKV_FORCE_INLINE float getX () const { return x; }
  
  /// \brief Returns the y component.
  HKV_FORCE_INLINE float getY () const { return y; }

  /// \brief Returns the z component.
  HKV_FORCE_INLINE float getZ () const { return z; }

  /// \brief Returns the w component.
  HKV_FORCE_INLINE float getW () const { return w; }

  /// \brief Checks whether the 4 components are all valid (ie. not NaN or infinity). Does NOT check whether the quaternion is normalized.
  HKV_FORCE_INLINE bool isValid () const;

  /// \brief Returns a float pointer to the internal data.
  HKV_FORCE_INLINE const float* getDataPointer () const { return &x; }

  /// \brief Returns a float pointer to the internal data.
  HKV_FORCE_INLINE float* getDataPointer () { return &x; }

  /// \brief Sets this quaternion to the spherical interpolation of 'qFrom' and 'qTo' by the factor 't'.
  VBASE_IMPEXP void setSlerp (const hkvQuat& qFrom, const hkvQuat& qTo, float t);

  //union
  //{
  //  struct
  //  {
      float x, y, z, w;
  //  };

  //  float data[4];
  //};

private:
  void operator& ();

  // ********************* OLD INTERFACE **********************
public:

  /// \brief DEPRECATED: Use hkvQuat::flipSigns instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Negate () { flipSigns (); }

  /// \brief DEPRECATED: Use hkvQuat::setSlerp instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void SLerp(const hkvQuat &from, const hkvQuat &to, float t) { setSlerp (from, to, t); }

  /// \brief DEPRECATED: Use hkvQuat::setSlerp instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE const hkvQuat Slerp(const hkvQuat& other, float t) { hkvQuat res; res.setSlerp (*this, other, t); return res; }

  /// \brief DEPRECATED: Use hkvQuat::setSlerp instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void SLerpFast(const hkvQuat &from, const hkvQuat &to, float t) { setSlerp (from, to, t); }

  /// \brief DEPRECATED: Use hkvQuat::invert instead. You should also normalize your quaternion first.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Invert () { normalize (); invert (); }

  /// \brief DEPRECATED: Use hkvQuat::transform instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE hkvVec3 PreTransformVector (const hkvVec3& vector1) const { return transform (vector1); }

  /// \brief DEPRECATED: Use hkvQuat::transformReverse instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE hkvVec3 TransformVector (const hkvVec3& vector1) const { return transformReverse (vector1); }

  /// \brief DEPRECATED: Use hkvQuat::setFromMat3 instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void FromMatrix (const hkvMat3& mRotation) { setFromMat3 (mRotation); }

  /// \brief DEPRECATED: Use hkvQuat::dot instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE float Dot (const hkvQuat& rhs) const { return dot (rhs); }

  /// \brief DEPRECATED: Use hkvQuat::dot instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE float DotProduct (const hkvQuat& rhs) const { return dot (rhs); }

  /// \brief DEPRECATED: Use hkvQuat::getLength instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE float Length () const { return getLength (); }

  /// \brief DEPRECATED: Use hkvQuat::setIdentity instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Identity () { setIdentity (); }

  /// \brief DEPRECATED: Use hkvQuat::normalize instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void Normalize () { normalize (); }

  /// \brief DEPRECATED: Use hkvQuat::getAsMat3 instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Get (hkvMat3& m) const { m = getAsMat3 (); }

  /// \brief DEPRECATED: Use hkvQuat::getAsMat3 instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void ToMatrix (hkvMat3& m) const { m = getAsMat3 (); }

  /// \brief DEPRECATED: Use hkvQuat::isEqual instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE bool Equals (const hkvQuat& rhs, float fEpsilon = HKVMATH_LARGE_EPSILON) const { return isEqual (rhs, fEpsilon); }

  /// \brief DEPRECATED: Use hkvQuat::multiplyReverse instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void operator*= (const hkvQuat& rhs) { *this = this->multiplyReverse (rhs); }

  /// \brief DEPRECATED: Use hkvQuat::setFromEulerAngles instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Set (const hkvVec3& vEuler) { setFromEulerAngles (vEuler.z, vEuler.y, vEuler.x); }

  /// \brief DEPRECATED: Use hkvQuat::setFromEulerAngles instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Set (const float eulerangles[3]) { setFromEulerAngles (eulerangles[2], eulerangles[1], eulerangles[0]); }

  /// \brief DEPRECATED: Use hkvQuat::getAxisAndAngle instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Get (float& fAngle, hkvVec3& vAxis) const { getAxisAndAngle (vAxis, fAngle); }

  // DEPRECATED: Use hkvQuat::setValuesDirect_Old instead.
  //HKVMATH_DEPRECATED_STAGE1 HKV_FORCE_INLINE hkvQuat (float inx, float iny, float inz, float negw) { x = inx; y = iny; z = inz; w = -negw; }

  /// \brief DEPRECATED: Use hkvNoInitialization instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE explicit hkvQuat (bool b) { }

  /// \brief DEPRECATED: Use hkvQuat::getAsMat4 instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Get (hkvMat4& m) const { m = getAsMat4 (); }

  /// \brief DEPRECATED: Use hkvQuat::setFromEulerAngles instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void SetEuler (float yaw, float pitch, float roll) { setFromEulerAngles (roll, pitch, yaw); }

  /// \brief DEPRECATED: Use hkvQuat::setFromMat3 instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Set (const hkvMat3& m) { setFromMat3 (m); }

  /// \brief DEPRECATED: Use hkvQuat::setFromMat3 instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void Set (const hkvMat4& m) { setFromMat3 (m.getRotationalPart ()); }

  /// \brief DEPRECATED: Use hkvQuat::setFromAxisAndAngle instead.
  HKVMATH_DEPRECATED_STAGE3 HKV_FORCE_INLINE void FromAngleAxis (const hkvVec3& vAxis, float fAngle) { setFromAxisAndAngle (vAxis.getNormalized (), fAngle); }

  /// \brief DEPRECATED: Use hkvQuat::setFromEulerAngles instead, but be sure to pass the values properly (roll, pitch, yaw, which is the reverse order).
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE explicit hkvQuat (const float eulerAngles[3]) { setFromEulerAngles (eulerAngles[2], eulerAngles[1], eulerAngles[0]); }

  /// \brief DEPRECATED: Use hkvQuat::setFromMat3 instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE explicit hkvQuat (const hkvMat3& rotmatrix) { setFromMat3 (rotmatrix); }

  /// \brief DEPRECATED: Use hkvQuat::setFromMat3 instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE explicit hkvQuat (const hkvMat4& rotmatrix) { setFromMat3 (rotmatrix.getRotationalPart ()); }

  /// \brief DEPRECATED: Use hkvQuat::getAxisAndAngle instead.
  HKVMATH_DEPRECATED_STAGE2 HKV_FORCE_INLINE void ToAngleAxis (hkvVec3& axis, float &angle) const { getAxisAndAngle (axis, angle); }

  // DEPRECATED: Use hkvQuat::getLengthSquared instead.
  //HKVMATH_DEPRECATED_STAGE1 HKV_FORCE_INLINE float NormSquared () const { return getLengthSquared (); }

  // DEPRECATED: Use hkvQuat::getLength instead.
  //HKVMATH_DEPRECATED_STAGE1 HKV_FORCE_INLINE float Norm () const { return getLength (); }

  // DEPRECATED: Use hkvQuat::getAxisAndAngle instead.
  //HKVMATH_DEPRECATED_STAGE1 HKV_FORCE_INLINE float GetAngle () const { hkvVec3 vAxis; float fAngle; getAxisAndAngle (vAxis, fAngle); return fAngle; }

  // Missing Functions:

  // void Conjugate();
  // float GetAngleBetween(const VisQuaternion_cl &otherQuat) const;


  // Not going to be added for now:

  // void SLerpNoAdjust (const VisQuaternion_cl &startQuat, const VisQuaternion_cl &endQuat, float slerp)
  // float MaxAbsError (const VisQuaternion_cl &q) const; 
  // float MeanSquareError (const VisQuaternion_cl &q) const; 
  // void LogDiff (const VisQuaternion_cl &a, const VisQuaternion_cl &b);
  // void InnerQuadPoint (const VisQuaternion_cl &p1, const VisQuaternion_cl &p2, const VisQuaternion_cl &p3);
  // void Log (const VisQuaternion_cl &q1);
  // void Pow(const VisQuaternion_cl &q, float x);
  // void Exp (const VisQuaternion_cl &q1);
  // void Squad (const VisQuaternion_cl &p, const VisQuaternion_cl &a, const VisQuaternion_cl &b, const VisQuaternion_cl &q, float t);
  // void SquadSpline4 (const VisQuaternion_cl &p1, const VisQuaternion_cl &p2, const VisQuaternion_cl &p3, const VisQuaternion_cl &p4, float t);
  // void SquadSpline4Robust (const VisQuaternion_cl &p1, const VisQuaternion_cl &p2, const VisQuaternion_cl &p3, const VisQuaternion_cl &p4, float t);
};

// Allows to serialize the hkvQuat to a VArchiv using >> and <<
V_DECLARE_SERIALX_NONINTRUSIVE (hkvQuat, VBASE_IMPEXP);

/// \brief Rotates the vector rhs by the quaternion lhs.
HKV_FORCE_INLINE const hkvVec3 operator* (const hkvQuat& lhs, const hkvVec3& rhs);

#ifdef HKVMATH_ENABLE_NEW_OPERATORS
  /// \brief Concatenates the rotations of lhs and rhs.
  HKV_FORCE_INLINE const hkvQuat operator* (const hkvQuat& lhs, const hkvQuat& rhs);
#endif

/// \brief Compares the two quaternions for equality.
HKV_FORCE_INLINE bool operator== (const hkvQuat& lhs, const hkvQuat& rhs);

/// \brief Compares the two quaternions for inequality.
HKV_FORCE_INLINE bool operator!= (const hkvQuat& lhs, const hkvQuat& rhs);

#include <Vision/Runtime/Base/Math/Quaternion/hkvQuat.inl>

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
