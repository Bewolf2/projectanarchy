/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMAT3_H
#define VBASE_HKVMATH_HKVMAT3_H

#include <Vision/Runtime/Base/Math/hkvMath.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat3Helpers.h>

/// \brief
///   A helper struct for returning matrices as contiguous array on the stack.
struct hkvMat3_AsArray
{
  float data[9];
};

/// \brief
///   A 3x3 matrix that can be used to represent rotations and scalings.
///
/// The elements are stored in column-major order. 
/// That means first the data of column 0 is stored, then column 1, column 2.
///
/// Thus the elements are stored like this in memory:
/// 00 03 06
/// 01 04 07
/// 02 05 08
class hkvMat3
{
public:

  /// \brief
  ///   Describes the memory layout in which matrix data is stored.
  enum Layout
  {
    ColumnMajor,      ///< The data is stored in column-major format, e.g. a float[9] array contains a matrix column by column.
    RowMajor          ///< The data is stored in row-major format, e.g. a float[9] array contains a matrix row by row
  };


public:

  ///
  /// @name Constructors
  /// @{
  ///

  #ifdef HKVMATH_DEFAULTCONSTRUCTORS_INITIALIZEDATA
    /// \brief
    ///   DEPRECATED: Initializes the matrix to the identity.
    ///
    /// Prefer to (not) initialize the matrix with hkvNoInitialization.
    ///
    /// Note: At some point the Vision Engine will deactivate this old behavior and use the uninitialized version instead.
    /// At that time you need to make sure that whenever you default construct a matrix, you do not rely on it being the identity.
    ///
    /// You can find all the places where you use the default constructor by defining 
    /// HKVMATH_DEPRECATE_DEFAULT_CONSTRUCTOR in hkvMathConfig.h and compiling your code for Windows.
    /// Then the compiler will generate a warning for every location where you use the default constructor.
    /// Use the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION to prevent that warning
    /// for code that cannot be changed to use a non default constructor (such as for arrays).
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvMat3 () { setIdentity (); }

  #else

    /// \brief
    ///   FUTURE BEHAVIOR: Keeps the matrix uninitialized.
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvMat3 () {}
  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvMat3 (hkvInit_None init /* = hkvNoInitialization */) { }

  /// \brief
  ///   Allows to set each element directly.
  ///
  /// This constructor allows to write a matrix in a formatted way in code.
  /// For example you could write hkvMat3 m (\n
  /// 1, 1, 1,\n
  /// 2, 2, 2,\n
  /// 3, 3, 3);\n
  /// \n
  /// Note that this is in 'row-major' format (since you write it row by row),
  /// therefore the default value for 'layout' is RowMajor.
  /// Internally data is stored in column-major format.
  ///
  /// \param c0r0
  ///   This is the value for column 0, row 0.
  ///
  /// \param c1r0
  ///   If layout is RowMajor, this is the value for column 1, row 0.
  ///   If layout is ColumnMajor, this is the value for row 1, column 0.
  ///
  /// \param c2r0
  ///   If layout is RowMajor, this is the value for column 2, row 0.
  ///   If layout is ColumnMajor, this is the value for row 2, column 0.
  ///
  /// \param c0r1
  ///   If layout is RowMajor, this is the value for column 0, row 1.
  ///   If layout is ColumnMajor, this is the value for row 0, column 1.
  ///
  /// \param c1r1
  ///   This is the value for column 1, row 1.
  ///
  /// \param c2r1
  ///   If layout is RowMajor, this is the value for column 2, row 1.
  ///   If layout is ColumnMajor, this is the value for row 2, column 1.
  ///
  /// \param c0r2
  ///   If layout is RowMajor, this is the value for column 0, row 2.
  ///   If layout is ColumnMajor, this is the value for row 0, column 2.
  ///
  /// \param c1r2
  ///   If layout is RowMajor, this is the value for column 1, row 2.
  ///   If layout is ColumnMajor, this is the value for row 1, column 2.
  ///
  /// \param c2r2
  ///   This is the value for column 2, row 2.
  ///
  /// \param layout
  ///   If layout is RowMajor, the other parameters are considered to be in row-major order (the expected default).
  ///   This allows to write matrices naturally in a code editor.
  ///   If layout is ColumnMajor, the other parameters will be transposed. Ie. parameter c1r0 will be used as if 
  ///   it was actually c0r1, etc.
  HKV_FORCE_INLINE hkvMat3 (float c0r0, float c1r0, float c2r0,
                           float c0r1, float c1r1, float c2r1,
                           float c0r2, float c1r2, float c2r2, Layout layout = RowMajor);

  ///
  /// @}
  ///

  ///
  /// @name Static Functions for Default Matrices
  /// @{
  ///

  /// \brief
  ///   Returns an identity matrix.
  HKV_FORCE_INLINE static const hkvMat3 IdentityMatrix () { return hkvMat3 (1,0,0,  0,1,0,  0,0,1); }

  /// \brief
  ///   Returns a zero matrix.
  HKV_FORCE_INLINE static const hkvMat3 ZeroMatrix () { return hkvMat3 (0,0,0, 0,0,0, 0,0,0); }

  ///
  /// @}
  ///

  ///
  /// @name Setting / Getting Data
  /// @{
  ///

  /// \brief
  ///   Sets the matrix to all zero.
  HKV_FORCE_INLINE void setZero ();

  /// \brief
  ///   Sets the matrix to the identity matrix.
  HKV_FORCE_INLINE void setIdentity ();

  /// \brief
  ///   Allows to set each element directly.
  ///
  /// This function allows to write a matrix in a formatted way in code.
  /// For example you could write hkvMat3 m; m.set (\n
  /// 1, 1, 1,\n
  /// 2, 2, 2,\n
  /// 3, 3, 3);\n
  /// \n
  /// Note that this is in 'row-major' format (since you write it row by row),
  /// therefore the default value for 'layout' is RowMajor.
  /// Internally data is stored in column-major format.
  ///
  /// \param c0r0
  ///   This is the value for column 0, row 0.
  ///
  /// \param c1r0
  ///   If layout is RowMajor, this is the value for column 1, row 0.
  ///   If layout is ColumnMajor, this is the value for row 1, column 0.
  ///
  /// \param c2r0
  ///   If layout is RowMajor, this is the value for column 2, row 0.
  ///   If layout is ColumnMajor, this is the value for row 2, column 0.
  ///
  /// \param c0r1
  ///   If layout is RowMajor, this is the value for column 0, row 1.
  ///   If layout is ColumnMajor, this is the value for row 0, column 1.
  ///
  /// \param c1r1
  ///   This is the value for column 1, row 1.
  ///
  /// \param c2r1
  ///   If layout is RowMajor, this is the value for column 2, row 1.
  ///   If layout is ColumnMajor, this is the value for row 2, column 1.
  ///
  /// \param c0r2
  ///   If layout is RowMajor, this is the value for column 0, row 2.
  ///   If layout is ColumnMajor, this is the value for row 0, column 2.
  ///
  /// \param c1r2
  ///   If layout is RowMajor, this is the value for column 1, row 2.
  ///   If layout is ColumnMajor, this is the value for row 1, column 2.
  ///
  /// \param c2r2
  ///   This is the value for column 2, row 2.
  ///
  /// \param layout
  ///   If layout is RowMajor, the other parameters are considered to be in row-major order (the expected default).
  ///   This allows to write matrices naturally in a code editor.
  ///   If layout is ColumnMajor, the other parameters will be transposed. Ie. parameter c1r0 will be used as if 
  ///   it was actually c0r1, etc.
  HKV_FORCE_INLINE void set (float c0r0, float c1r0, float c2r0,
                             float c0r1, float c1r1, float c2r1,
                             float c0r2, float c1r2, float c2r2, Layout layout = RowMajor);

  /// \brief
  ///   Sets the matrix by copying the data from an array.
  ///
  /// \param pCopyFrom
  ///   The array from which to copy the matrix data. Exactly 9 elements are read.
  ///
  /// \param layout
  ///   If layout is ColumnMajor the pCopyFrom array is considered to be in ColumnMajor format.\n
  ///   Ie. c0r0, c0r1, c0r2, c1r0, c1r1, c1r2, c2r0, c2r1, c2r2\n
  ///   If layout is RowMajor, the pCopyFrom array is considered to be in RowMajor format.\n
  ///   Ie. c0r0, c1r0, c2r0, c0r1, c1r1, c2r1, c0r2, c1r2, c2r2\n
  HKV_FORCE_INLINE void set (const float* pCopyFrom, Layout layout);

  /// \brief
  ///   Copies the matrix data to a float array. Transposes it, if desired.
  ///
  /// \param pCopyTo
  ///   The array to copy the data to. Must have 9 elements.
  ///
  /// \param layout
  ///   Specifies the data layout in which the data will be written to the array.
  ///   ColumnMajor means the data is written as\n
  ///   c0r0, c0r1, c0r2, c1r0, c1r1, c1r2, c2r0, c2r1, c2r2\n
  ///   RowMajor means the data is written as\n
  ///   c0r0, c1r0, c2r0, c0r1, c1r1, c2r1, c0r2, c1r2, c2r2\n
  HKV_FORCE_INLINE void get (float* HKV_RESTRICT pCopyTo, Layout layout) const;

  /// \brief
  ///   Sets all the values in the specified row.
  HKV_FORCE_INLINE void setRow (hkUint32 iRow, const hkvVec3& values);

  /// \brief
  ///   Returns all the values in the specified row.
  HKV_FORCE_INLINE const hkvVec3 getRow (hkUint32 iRow) const;

  /// \brief
  ///   Sets all the values in the specified column.
  HKV_FORCE_INLINE void setColumn (hkUint32 iColumn, const hkvVec3& values);

  /// \brief
  ///   Returns all the values in the specified column.
  HKV_FORCE_INLINE const hkvVec3 getColumn (hkUint32 iColumn) const;

  /// \brief
  ///   Sets the n-th element of the matrix.
  ///
  /// This function can be used to access elements in row-major or column-major fashion.
  /// That can be very useful when working with other libraries that have other conventions.
  /// You can keep the indices identical, and just need to specify, that it is meant to be a row-major
  /// or column-major index. Which element exactly is accessed, will then be handled internally.
  ///
  /// \param iNthElement
  ///   The index, which element should be accessed. If layout is ColumnMajor, this is like accessing m_ElementsCM
  ///   which is a direct array lookup.
  ///   If layout is RowMajor, the index is interpreted as if it is meant for a matrix that is stored in row-major order.
  ///   Since this class stores its data in column-major order, the index is recalculated to access the proper element.
  ///
  /// \param f
  ///   The value with which to set the n-th element.
  ///
  /// \param layout
  ///   Specifies whether the index iNthElement is meant for a row-major or column-major representation.
  ///   That allows to either use it directly or recalculate it first.
  HKV_FORCE_INLINE void setElement (hkUint32 iNthElement, float f, Layout layout = ColumnMajor);

  /// \brief
  ///   Returns the n-th element from the matrix.
  ///
  /// This function can be used to access elements in row-major or column-major fashion.
  /// That can be very useful when working with other libraries that have other conventions.
  /// You can keep the indices identical, and just need to specify, that it is meant to be a row-major
  /// or column-major index. Which element exactly is accessed, will then be handled internally.
  ///
  /// \param iNthElement
  ///   The index, which element should be accessed. If layout is ColumnMajor, this is like accessing m_ElementsCM
  ///   which is a direct array lookup.
  ///   If layout is RowMajor, the index is interpreted as if it is meant for a matrix that is stored in row-major order.
  ///   Since this class stores its data in column-major order, the index is recalculated to access the proper element.
  ///
  /// \param layout
  ///   Specifies whether the index iNthElement is meant for a row-major or column-major representation.
  ///   That allows to either use it directly or recalculate it first.
  HKV_FORCE_INLINE float getElement (hkUint32 iNthElement, Layout layout = ColumnMajor) const;

  /// \brief
  ///   Sets all the values on the matrices diagonal.
  HKV_FORCE_INLINE void setDiagonal (const hkvVec3& values);

  /// \brief
  ///   Returns all the values on the matrices diagonal.
  HKV_FORCE_INLINE const hkvVec3 getDiagonal ();

  /// \brief
  ///   Sets the x,y,z values on the specified axis. iAxisXYZ must be in range [0;2].
  HKV_FORCE_INLINE void setAxis (hkUint32 iAxisXYZ, const hkvVec3& v);

  /// \brief
  ///   Returns the x,y,z values on the specified axis. iAxisXYZ must be in range [0;2].
  HKV_FORCE_INLINE const hkvVec3 getAxis (hkUint32 iAxisXYZ) const;

  /// \brief
  ///   Sets all three axis of the matrix.
  HKV_FORCE_INLINE void setAxisXYZ (const hkvVec3& AxisX, const hkvVec3& AxisY, const hkvVec3& AxisZ);

  /// \brief
  ///   Returns all three axis of the matrix.
  HKV_FORCE_INLINE void getAxisXYZ (hkvVec3* HKV_RESTRICT out_AxisX, hkvVec3* HKV_RESTRICT out_AxisY, hkvVec3* HKV_RESTRICT out_AxisZ) const;


  /// \brief
  ///   Sets this matrix to a rotation matrix defined by the given euler angles.
  HKV_FORCE_INLINE void setFromEulerAngles (float fRoll, float fPitch, float fYaw);

  /// \brief
  ///   Returns the euler angles which represent the rotation of this matrix.
  HKV_FORCE_INLINE void getAsEulerAngles (float& out_fRoll, float& out_fPitch, float& out_fYaw) const;

  /// \brief
  ///   Sets this matrix to a rotation matrix which represents the same rotation as the given quaternion.
  HKV_FORCE_INLINE void setFromQuaternion (const hkvQuat& q);

  /// \brief
  ///   Returns a quaternion which represent the rotation of this matrix.
  HKV_FORCE_INLINE const hkvQuat getAsQuaternion () const;

  /// \brief
  ///   Returns the matrix as one contiguous array for reading in either row-major or column-major order.
  HKV_FORCE_INLINE const hkvMat3_AsArray getAsArray (Layout layout) const;

  ///
  /// @}
  ///

  ///
  /// @name Checks
  /// @{
  ///

  /// \brief
  ///   Checks whether this matrix is all zero within some epsilon range.
  HKV_FORCE_INLINE bool isZero (float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  /// \brief
  ///   Checks whether this matrix is the identity within some epsilon.
  HKV_FORCE_INLINE bool isIdentity (float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  /// \brief
  ///   Checks whether the two matrices are identical.
  HKV_FORCE_INLINE bool isIdentical (const hkvMat3& rhs) const;

  /// \brief
  ///   Checks whether the two matrices are equal within some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvMat3& rhs, float fEpsilon) const;

  /// \brief
  ///   Checks that no element is NaN or infinity.
  HKV_FORCE_INLINE bool isValid () const;

  ///
  /// @}
  ///

  ///
  /// @name Creating Matrices
  /// @{
  ///

  /// \brief
  ///   Makes this matrix into a scaling matrix.
  HKV_FORCE_INLINE void setScalingMatrix (const hkvVec3& vScaleXYZ);

  /// \brief
  ///   Makes this matrix into a matrix that rotates objects around the X axis.
  VBASE_IMPEXP void setRotationMatrixX (float fDegree);

  /// \brief
  ///   Makes this matrix into a matrix that rotates objects around the Y axis.
  VBASE_IMPEXP void setRotationMatrixY (float fDegree);

  /// \brief
  ///   Makes this matrix into a matrix that rotates objects around the Z axis.
  VBASE_IMPEXP void setRotationMatrixZ (float fDegree);

  /// \brief
  ///   Makes this matrix into a matrix that rotates objects around an arbitrary axis.
  ///
  /// The matrix will be completely reset, so if it had any scaling or translation beforehand, this will be gone.
  ///
  /// \param vAxis
  ///   The rotation axis. Must be normalized. Will assert in debug builds otherwise.
  ///
  /// \param fDegree
  ///   The amount of rotation around vAxis, given in degree.
  VBASE_IMPEXP void setRotationMatrix (const hkvVec3& vAxis, float fDegree);


  /// \brief
  ///   Creates an orthonormal rotation matrix which rotates an object such that it will 'look' in the given direction.
  ///
  /// The Vision Engine uses by default the X-axis as the 'forward' direction of objects, +Y as 'left' and +Z as 'up'.
  /// In case that vLookDir and vUpDir are nearly equal an arbitrary up vector is used to create a valid matrix.
  ///
  /// \param vLookDir
  ///   Look direction. Does not need to be normalized.
  ///
  /// \param vUpDir
  ///   An optional up vector. Must be normalized. By default (0,0,1) is used.
  ///   
  VBASE_IMPEXP void setLookInDirectionMatrix (hkvVec3 vLookDir, hkvVec3 vUpDir = hkvVec3 (0, 0, 1));


  ///
  /// @}
  ///

  ///
  /// @name Inverse / Transpose
  /// @{
  ///

  /// \brief
  ///   Transposes the matrix.
  HKV_FORCE_INLINE void transpose ();

  /// \brief
  ///   Returns a transposed copy of this matrix.
  HKV_FORCE_INLINE const hkvMat3 getTransposed () const;

  /// \brief
  ///   Inverts this matrix. Works for all (mathematically) invertible matrices.
  ///
  /// Will do a full inversion of the whole matrix. Can invert all types of matrices, that are mathematically invertible.
  /// A matrix multiplied with its inverse will always yield the identity (within some epsilon range).
  /// If the matrix could not be inverted the values of the matrix are undefined.
  ///
  /// \return
  ///   If the matrix could not be inverted, HKV_FAILURE will be returned. Otherwise HKV_SUCCESS.
  VBASE_IMPEXP hkvResult invert ();

  /// \brief
  ///   Returns an inverted copy of this matrix.
  ///
  /// Internally this creates a copy of the matrix and then calls hkvMat3::invert on that.
  /// As such it has the same limitations when the inversion fails.
  HKV_FORCE_INLINE const hkvMat3 getInverse () const;

  ///
  /// @}
  ///

  ///
  /// @name Transformations
  /// @{
  ///

  /// \brief
  ///   Returns a matrix that is the result of multiplying this matrix with rhs.
  ///
  /// Prefer this function for matrix multiplications. Try not to use the * operator.
  /// The * operator currently acts like multiplyReverse, since this is the old behavior of Trinigy's math classes.
  /// At some point that operator will become deprecated. You can then use 'multiplyReverse' instead or rewrite formulas to use 'multiply'.
  /// Therefore when writing new code it is recommended to use function calls instead of overloaded operators, at least for now.
  ///
  /// \param rhs
  ///   The right-hand side matrix.
  ///
  /// \sa hkvMat3::multiplyReverse
  VBASE_IMPEXP const hkvMat3 multiply (const hkvMat3& rhs) const;

  /// \brief
  ///   Returns a matrix that is the result of multiplying rhs with this matrix.
  ///
  /// Try not to use the * operator.
  /// The * operator currently acts like multiplyReverse, since this is the old behavior of Trinigy's math classes.
  /// At some point that operator will become deprecated. You can then use 'multiplyReverse' instead or rewrite formulas to use 'multiply'.
  /// Therefore when writing new code it is recommended to use function calls instead of overloaded operators, at least for now.
  ///
  /// \param rhs
  ///   The right-hand side matrix.
  ///
  /// \sa hkvMat3::multiply
  HKV_FORCE_INLINE const hkvMat3 multiplyReverse (const hkvMat3& rhs) const;

  /// \brief
  ///   Returns v transformed by this matrix.
  ///
  /// Since a 3x3 matrix does not have any translation part, it can only rotate or scale a vector.
  /// This is similar to multiplying with a 4x4 matrix but considering the vector to be a direction 
  /// (ie. setting the implied w component to 0).
  /// Therefore in hkvMat3 this function is also called transformDirection, although there is no
  /// transformPosition pendant, as in hkvMat4.
  ///
  /// \param v
  ///   The vector to be multiplied by the matrix.
  HKV_FORCE_INLINE const hkvVec3 transformDirection (const hkvVec3& v) const;

  /// \brief
  ///   Returns v transformed by this matrix.
  ///
  /// Since a 3x3 matrix does not have any translation part, it can only rotate or scale a vector.
  /// This is similar to multiplying with a 4x4 matrix but considering the vector to be a direction 
  /// (ie. setting the implied w component to 0).
  /// Therefore in hkvMat3 this function is also called transformDirection, although there is no
  /// transformPosition pendant, as in hkvMat4.
  ///
  /// \param v
  ///   The vector to be multiplied by the matrix.
  VBASE_IMPEXP const hkvVec3d transformDirection (const hkvVec3d& v) const;

  /// \brief
  ///   Transforms a complete set of points.
  ///
  /// \param inout_vPoints
  ///   Pointer to an array of vectors.
  ///
  /// \param uiNumPoints
  ///   The number of points to use from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the array. Must be at least sizeof (hkvVec3)
  ///   for densely packed arrays.
  ///
  VBASE_IMPEXP void transformDirection (hkvVec3* HKV_RESTRICT inout_vPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  ///
  /// @}
  ///

  ///
  /// @name Scaling
  /// @{
  ///

  /// \brief
  ///   Sets the scaling of the x,y,z axis. Returns HKV_FAILURE if the scaling could not be set, because the matrix is too close to a zero matrix.
  HKV_FORCE_INLINE hkvResult setScalingFactors (const hkvVec3& vXYZ, float fEpsilon = HKVMATH_SMALL_EPSILON);

  /// \brief
  ///   Returns the scaling of the x,y,z axis.
  HKV_FORCE_INLINE hkvVec3 getScalingFactors () const;

  /// \brief
  ///   Normalizes each axis of the matrix.
  ///
  /// Returns HKV_FAILURE if the matrix is too close to a zero matrix and could not be normalized
  /// or if any value is invalid (ie. NaN or +/- infinity).
  /// Does not modify the matrix in case of failure.
  ///
  /// \param fEpsilon
  ///   If the length of an axis is below this value, normalization fails.
  HKV_FORCE_INLINE hkvResult normalize (float fEpsilon = HKVMATH_SMALL_EPSILON);

  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///

  /// \brief
  ///   Adds rhs component-wise to this matrix.
  HKV_FORCE_INLINE void operator+= (const hkvMat3& rhs);

  /// \brief
  ///   Subtracts rhs component-wise from this matrix.
  HKV_FORCE_INLINE void operator-= (const hkvMat3& rhs);

  /// \brief
  ///   Multiplies all components by f.
  HKV_FORCE_INLINE void operator*= (float f);

  /// \brief
  ///   Divides all components by f.
  HKV_FORCE_INLINE void operator/= (float f);

  ///
  /// @}
  ///


  ///
  /// @name Portability Features
  /// @{
  ///

public:

  /// \brief Returns a pointer to this matrix.
  /// 
  /// The operator& was declared deprecated on this class, since the data layout has changed and it is difficult to find operations
  /// such as memcpy (which would work incorrectly). By deprecating operator& all instances in which this class might be
  /// modified through raw memory access can be found.
  /// However, if you know your code is correct, you can use 'getPointer' to get a direct pointer to this object without getting a warning.
  ///
  /// If necessary you can disable a deprecated warning by putting HKV_DISABLE_DEPRECATION / HKV_ENABLE_DEPRECATION around the piece
  /// of code that needs to access the deprecated code.
  ///
  /// In some later update operator& will not be marked deprecated anymore.
  const hkvMat3* getPointer () const { return this; }

  /// \brief Returns a pointer to this matrix.
  /// 
  /// The operator& was declared deprecated on this class, since the data layout has changed and it is difficult to find operations
  /// such as memcpy (which would work incorrectly). By deprecating operator& all instances in which this class might be
  /// modified through raw memory access can be found.
  /// However, if you know your code is correct, you can use 'getPointer' to get a direct pointer to this object without getting a warning.
  ///
  /// If necessary you can disable a deprecated warning by putting HKV_DISABLE_DEPRECATION / HKV_ENABLE_DEPRECATION around the piece
  /// of code that needs to access the deprecated code.
  ///
  /// In some later update operator& will not be marked deprecated anymore.
  hkvMat3* getPointer () { return this; }

  ///
  /// @}
  ///

public:
  
  union
  {
    float                   m_ElementsCM[9];  ///< Data is stored column-major
    float                   m_Column[3][3];   ///< Column-major 2D array

    hkvMat3_ElementSwizzle  m_ElementsRM;     ///< Swizzle object to access data, as if it were stored row-major
    hkvMat3_ColumnSwizzle   m_Row;            ///< Swizzle object to access data with 2D indices, as if it were stored row-major
    };

  };

V_DECLARE_SERIALX_NONINTRUSIVE (hkvMat3, VBASE_IMPEXP);

/// \brief
///   Checks whether the two matrices are identical.
HKV_FORCE_INLINE bool operator== (const hkvMat3& lhs, const hkvMat3& rhs);

/// \brief
///   Checks whether the two matrices are not identical.
HKV_FORCE_INLINE bool operator!= (const hkvMat3& lhs, const hkvMat3& rhs);

/// \brief
///   Adds the two matrices component-wise.
HKV_FORCE_INLINE const hkvMat3 operator+ (const hkvMat3& lhs, const hkvMat3& rhs);

/// \brief
///   Subtracts the two matrices component-wise.
HKV_FORCE_INLINE const hkvMat3 operator- (const hkvMat3& lhs, const hkvMat3& rhs);

/// \brief
///   Multiplication operator.
HKV_FORCE_INLINE const hkvVec3 operator* (const hkvMat3& lhs, const hkvVec3& rhs);

/// \brief
///   Multiplication operator.
VBASE_IMPEXP const hkvVec3d operator* (const hkvMat3& lhs, const hkvVec3d& rhs);

#ifdef HKVMATH_ENABLE_NEW_OPERATORS
  /// \brief
  ///   Multiplication operator.
  HKV_FORCE_INLINE const hkvMat3 operator* (const hkvMat3& lhs, const hkvMat3& rhs);
#else
  const hkvMat3 operator* (const hkvMat3& lhs, const hkvMat3& rhs);
#endif

/// \brief
///   Multiplies all components of the matrix by f.
HKV_FORCE_INLINE const hkvMat3 operator* (const hkvMat3& lhs, float f);

/// \brief
///   Multiplies all components of the matrix by f.
HKV_FORCE_INLINE const hkvMat3 operator* (float f, const hkvMat3& rhs);

/// \brief
///   Divides all components of the matrix by f.
HKV_FORCE_INLINE const hkvMat3 operator/ (const hkvMat3& lhs, float f);


#include <Vision/Runtime/Base/Math/Matrix/hkvMat3.inl>

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
