/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVMAT4_H
#define VBASE_HKVMATH_HKVMAT4_H

#include <Vision/Runtime/Base/Math/hkvMath.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4Helpers.h>
/// \brief
///   A helper struct for returning matrices as contiguous array on the stack.
struct hkvMat4_AsArray
{
  float data[16];
};

/// \brief
///   Describes which depth-range to use. 'MinusOneToOne' is used by OpenGL. 'ZeroToOne' is used by DirectX.
struct hkvClipSpaceDepthRange
{
  /// \brief
  ///   Describes which depth-range to use. 'MinusOneToOne' is used by OpenGL. 'ZeroToOne' is used by DirectX.
  enum Enum
  {
    MinusOneToOne,  ///< This is the convention used by OpenGL
    ZeroToOne,      ///< This is the convention used by Direct3D

    #ifdef CLIPSPACE_DEPTH_IS_MINUSONE_TO_ONE
      Native = MinusOneToOne, ///< This is the convention used by the platform that the code has been compiled for
    #else
      Native = ZeroToOne,     ///< This is the convention used by the platform that the code has been compiled for
    #endif
  };
};

/// \brief
///  Describes whether to map the y-Range to [-1, 1], or to flip upside down and map it to [1, -1].
struct hkvClipSpaceYRange
{
  /// \brief
  ///  Describes whether to map the y-Range to [-1, 1], or to flip upside down and map it to [1, -1].
  enum Enum
  {
    MinusOneToOne,    ///< Creates a standard projection matrix.
    OneToMinusOne,    ///< Creates a projection matrix used to flip the resulting image upside down.

#if defined(_VR_GLES2)
    ForRenderToTexture    = OneToMinusOne,  ///< This is the convention suitable for rendering to a texture on the platform the code has been compiled for.
    ForRenderToBackBuffer = MinusOneToOne,  ///< This is the convention suitable for rendering into the back buffer on the platform  the code has been compiled for.
#else
    ForRenderToTexture    = MinusOneToOne,  ///< This is the convention suitable for rendering to a texture on the platform the code has been compiled for.
    ForRenderToBackBuffer = MinusOneToOne,  ///< This is the convention suitable for rendering into the back buffer on the platform  the code has been compiled for.
#endif
  };
};


/// \brief
///   A 4x4 matrix.
///
/// The elements are stored in column-major order. 
/// That means first the data of column 0 is stored, then column 1, column 2 then column 3.
///
/// Thus the elements are stored like this in memory:
/// 00 04 08 12
/// 01 05 09 13
/// 02 06 10 14
/// 03 07 11 15
class hkvMat4
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
    /// Prefer to initialize the matrix with hkvMat4::Identity if you want an identity matrix.
    /// Prefer to initialize the matrix with hkvNoInitialization if it does not need to be initialized.
    ///
    /// Note: At some point the Vision Engine will deactivate this old behavior and use the uninitialized version instead.
    /// At that time you need to make sure that whenever you default construct a matrix, you do not rely on it being the identity.
    ///
    /// You can find all the places where you use the default constructor by defining 
    /// HKVMATH_DEPRECATE_DEFAULT_CONSTRUCTOR in hkvMathConfig.h and compiling your code for Windows.
    /// Then the compiler will generate a warning for every location where you use the default constructor.
    /// Use the macros HKV_DISABLE_DEPRECATION and HKV_ENABLE_DEPRECATION to prevent that warning
    /// for code that cannot be changed to use a non default constructor (such as for arrays).
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvMat4 () { setIdentity (); }

  #else

    /// \brief
    ///   FUTURE BEHAVIOR: Keeps the matrix uninitialized.
    HKVMATH_DEFAULT_CONSTRUCTOR HKV_FORCE_INLINE hkvMat4 () {}
  #endif


  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvMat4 (hkvInit_None init /* = hkvNoInitialization */) { }


  /// \brief
  ///   Allows to set each element directly.
  ///
  /// This constructor allows to write a matrix in a formatted way in code.
  /// For example you could write hkvMat4 m (\n
  /// 1, 1, 1, 1,\n
  /// 2, 2, 2, 2,\n
  /// 3, 3, 3, 3,\n
  /// 4, 4, 4, 4);\n
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
  /// \param c3r0
  ///   If layout is RowMajor, this is the value for column 3, row 0.
  ///   If layout is ColumnMajor, this is the value for row 3, column 0.
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
  /// \param c3r1
  ///   If layout is RowMajor, this is the value for column 3, row 1.
  ///   If layout is ColumnMajor, this is the value for row 3, column 1.
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
  /// \param c3r2
  ///   If layout is RowMajor, this is the value for column 3, row 2.
  ///   If layout is ColumnMajor, this is the value for row 3, column 2.
  ///
  /// \param c0r3
  ///   If layout is RowMajor, this is the value for column 0, row 3.
  ///   If layout is ColumnMajor, this is the value for row 0, column 3.
  ///
  /// \param c1r3
  ///   If layout is RowMajor, this is the value for column 1, row 3.
  ///   If layout is ColumnMajor, this is the value for row 1, column 3.
  ///
  /// \param c2r3
  ///   If layout is RowMajor, this is the value for column 2, row 3.
  ///   If layout is ColumnMajor, this is the value for row 2, column 3.
  ///
  /// \param c3r3
  ///   This is the value for column 3, row 3.
  ///
  /// \param layout
  ///   If layout is RowMajor, the other parameters are considered to be in row-major order (the expected default).
  ///   This allows to write matrices naturally in a code editor.
  ///   If layout is ColumnMajor, the other parameters will be transposed. Ie. parameter c1r0 will be used as if 
  ///   it was actually c0r1, etc.
  HKV_FORCE_INLINE hkvMat4 (float c0r0, float c1r0, float c2r0, float c3r0,
                            float c0r1, float c1r1, float c2r1, float c3r1,
                            float c0r2, float c1r2, float c2r2, float c3r2,
                            float c0r3, float c1r3, float c2r3, float c3r3, Layout layout = RowMajor);


  /// \brief
  ///   Creates a matrix by combining a 3x3 matrix and a translation vector.
  HKV_FORCE_INLINE hkvMat4 (const hkvMat3& m3x3, const hkvVec3& vTranslation);

  ///
  /// @}
  ///

  ///
  /// @name Static Functions for Default Matrices
  /// @{
  ///

  /// \brief
  ///   Returns an identity matrix.
  HKV_FORCE_INLINE static const hkvMat4 IdentityMatrix () { return hkvMat4 (1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1); }

  /// \brief
  ///   Returns a zero matrix.
  HKV_FORCE_INLINE static const hkvMat4 ZeroMatrix () { return hkvMat4 (0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0); }

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
  ///   Creates a matrix by combining a 3x3 matrix and a translation vector.
  HKV_FORCE_INLINE void set (const hkvMat3& m3x3, const hkvVec3& vTranslation);

  /// \brief
  ///   Allows to set each element directly.
  ///
  /// This functions allows to write a matrix in a formatted way in code.
  /// For example you could write hkvMat4 m; m.set (\n
  /// 1, 1, 1, 1,\n
  /// 2, 2, 2, 2,\n
  /// 3, 3, 3, 3,\n
  /// 4, 4, 4, 4);\n
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
  /// \param c3r0
  ///   If layout is RowMajor, this is the value for column 3, row 0.
  ///   If layout is ColumnMajor, this is the value for row 3, column 0.
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
  /// \param c3r1
  ///   If layout is RowMajor, this is the value for column 3, row 1.
  ///   If layout is ColumnMajor, this is the value for row 3, column 1.
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
  /// \param c3r2
  ///   If layout is RowMajor, this is the value for column 3, row 2.
  ///   If layout is ColumnMajor, this is the value for row 3, column 2.
  ///
  /// \param c0r3
  ///   If layout is RowMajor, this is the value for column 0, row 3.
  ///   If layout is ColumnMajor, this is the value for row 0, column 3.
  ///
  /// \param c1r3
  ///   If layout is RowMajor, this is the value for column 1, row 3.
  ///   If layout is ColumnMajor, this is the value for row 1, column 3.
  ///
  /// \param c2r3
  ///   If layout is RowMajor, this is the value for column 2, row 3.
  ///   If layout is ColumnMajor, this is the value for row 2, column 3.
  ///
  /// \param c3r3
  ///   This is the value for column 3, row 3.
  ///
  /// \param layout
  ///   If layout is RowMajor, the other parameters are considered to be in row-major order (the expected default).
  ///   This allows to write matrices naturally in a code editor.
  ///   If layout is ColumnMajor, the other parameters will be transposed. Ie. parameter c1r0 will be used as if 
  ///   it was actually c0r1, etc.
  HKV_FORCE_INLINE void set (float c0r0, float c1r0, float c2r0, float c3r0,
                             float c0r1, float c1r1, float c2r1, float c3r1,
                             float c0r2, float c1r2, float c2r2, float c3r2,
                             float c0r3, float c1r3, float c2r3, float c3r3, Layout layout = RowMajor);


  /// \brief
  ///   Sets the matrix by copying the data from an array.
  ///
  /// \param pCopyFrom
  ///   The array from which to copy the matrix data. Exactly 16 elements are read.
  ///
  /// \param layout
  ///   If layout is ColumnMajor the pCopyFrom array is considered to be in ColumnMajor format.\n
  ///   Ie. c0r0, c0r1, c0r2, c0r3, c1r0, c1r1, c1r2, c1r3, c2r0, c2r1, c2r2, c2r3, c3r0, c3r1, c3r2, c3r3\n
  ///   If layout is RowMajor, the pCopyFrom array is considered to be in RowMajor format.\n
  ///   Ie. c0r0, c1r0, c2r0, c3r0, c0r1, c1r1, c2r1, c3r1, c0r2, c1r2, c2r2, c3r2, c0r3, c1r3, c2r3, c3r3\n
  HKV_FORCE_INLINE void set (const float* pCopyFrom, Layout layout);

  /// \brief
  ///   Copies the matrix data to a float array. Transposes it, if desired.
  ///
  /// \param pCopyTo
  ///   The array to copy the data to. Must have 16 elements.
  ///
  /// \param layout
  ///   Specifies the data layout in which the data will be written to the array.
  ///   ColumnMajor means the data is written as\n
  ///   Ie. c0r0, c0r1, c0r2, c0r3, c1r0, c1r1, c1r2, c1r3, c2r0, c2r1, c2r2, c2r3, c3r0, c3r1, c3r2, c3r3\n
  ///   RowMajor means the data is written as\n
  ///   Ie. c0r0, c1r0, c2r0, c3r0, c0r1, c1r1, c2r1, c3r1, c0r2, c1r2, c2r2, c3r2, c0r3, c1r3, c2r3, c3r3\n
  HKV_FORCE_INLINE void get (float* HKV_RESTRICT pCopyTo, Layout layout) const;

  /// \brief
  ///   Sets all the values in the specified row.
  HKV_FORCE_INLINE void setRow (hkUint32 iRow, const hkvVec4& values);

  /// \brief
  ///   Returns all the values in the specified row.
  HKV_FORCE_INLINE const hkvVec4 getRow (hkUint32 iRow) const;

  /// \brief
  ///   Sets all the values in the specified column.
  HKV_FORCE_INLINE void setColumn (hkUint32 iColumn, const hkvVec4& values);

  /// \brief
  ///   Returns all the values in the specified column.
  HKV_FORCE_INLINE const hkvVec4 getColumn (hkUint32 iColumn) const;

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
  ///   The value to set.
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
  HKV_FORCE_INLINE void setDiagonal (const hkvVec4& values);

  /// \brief
  ///   Returns all the values on the matrices diagonal.
  HKV_FORCE_INLINE const hkvVec4 getDiagonal () const;

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
  ///   Returns the 3x3 part of the 4x4 matrix, which represents rotation and scaling.
  HKV_FORCE_INLINE const hkvMat3 getRotationalPart () const;

  /// \brief
  ///   Sets the 3x3 part of the 4x4 matrix, which represents rotation and scaling. Leaves all other data unchanged.
  HKV_FORCE_INLINE void setRotationalPart (const hkvMat3& mRot);

  /// \brief
  ///   Sets this matrix to a rotation matrix defined by the given euler angles.
  HKV_FORCE_INLINE void setFromEulerAngles (float fRoll, float fPitch, float fYaw);

  /// \brief
  ///   Returns the euler angles which represent the rotation which is stored in the 3x3 part of this matrix.
  HKV_FORCE_INLINE void getAsEulerAngles (float& out_fRoll, float& out_fPitch, float& out_fYaw) const;

  /// \brief
  ///   Sets this matrix to a rotation matrix which represents the same rotation as the given quaternion.
  HKV_FORCE_INLINE void setFromQuaternion (const hkvQuat& q);

  /// \brief
  ///   Returns a quaternion which represent the rotation which is stored in the 3x3 part of this matrix.
  HKV_FORCE_INLINE hkvQuat getAsQuaternion () const;

  /// \brief
  ///   Returns the matrix as one contiguous array for reading in either row-major or column-major order.
  HKV_FORCE_INLINE const hkvMat4_AsArray getAsArray (Layout layout) const;

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
  HKV_FORCE_INLINE bool isIdentical (const hkvMat4& rhs) const;

  /// \brief
  ///   Checks whether the two matrices are equal within some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvMat4& rhs, float fEpsilon) const;
  
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
  ///   Creates a pure translation matrix.
  HKV_FORCE_INLINE void setTranslationMatrix (const hkvVec3& vTranslation);

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
  HKV_FORCE_INLINE const hkvMat4 getTransposed () const;

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
  /// Internally this creates a copy of the matrix and then calls hkvMat4::invert on that.
  /// As such it has the same limitations when the inversion fails.
  HKV_FORCE_INLINE const hkvMat4 getInverse () const;

  /// \brief
  ///   Inverts the matrix. Only works on orthogonal and uniformly scaled matrices. Faster than 'invert'.
  VBASE_IMPEXP hkvResult invertOrthogonal ();

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
  /// \sa hkvMat4::multiplyReverse
  VBASE_IMPEXP const hkvMat4 multiply (const hkvMat4& rhs) const;

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
  /// \sa hkvMat4::multiply
  HKV_FORCE_INLINE const hkvMat4 multiplyReverse (const hkvMat4& rhs) const;

  /// \brief
  ///   Returns a transformed copy of v, assuming v to be a point.
  ///
  /// This function uses 1 as the w-component of v and then multiplies v with this matrix.
  /// The result is that the translation part of this matrix is added to the resulting vector,
  /// which means the point is not only rotated but also moved around.
  /// This is the default method of transformation, which is also used in 
  /// operator* (hkvMat4, hkvVec3)
  /// 
  /// \param v
  ///   The position vector to be transformed.
  HKV_FORCE_INLINE const hkvVec3 transformPosition (const hkvVec3& v) const;

  /// \brief
  ///   Returns a transformed copy of v, assuming v to be a point.
  ///
  /// This function uses 1 as the w-component of v and then multiplies v with this matrix.
  /// The result is that the translation part of this matrix is added to the resulting vector,
  /// which means the point is not only rotated but also moved around.
  /// This is the default method of transformation, which is also used in 
  /// operator* (hkvMat4, hkvVec3)
  /// 
  /// \param v
  ///   The position vector to be transformed.
  VBASE_IMPEXP const hkvVec3d transformPosition (const hkvVec3d& v) const;

  /// \brief
  ///   Transforms a whole set of points, which are handled like position vectors.
  VBASE_IMPEXP void transformPositions (hkvVec3* HKV_RESTRICT inout_vPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   Returns a transformed copy of v, assuming v to be a direction vector.
  ///
  /// This function uses 0 as the w-component of v and then multiplies v with this matrix.
  /// The result is that the translation part of this matrix is ignored,
  /// which means that v is only rotated and scaled but not moved around.
  /// This is useful to transform normals and tangents etc.
  ///
  /// \param v
  ///   The direction vector to be transformed.
  HKV_FORCE_INLINE const hkvVec3 transformDirection (const hkvVec3& v) const;

  /// \brief
  ///   Returns a transformed copy of v, assuming v to be a direction vector.
  ///
  /// This function uses 0 as the w-component of v and then multiplies v with this matrix.
  /// The result is that the translation part of this matrix is ignored,
  /// which means that v is only rotated and scaled but not moved around.
  /// This is useful to transform normals and tangents etc.
  ///
  /// \param v
  ///   The direction vector to be transformed.
  VBASE_IMPEXP const hkvVec3d transformDirection (const hkvVec3d& v) const;

  /// \brief
  ///   Transforms a whole set of points, which are handled like direction vectors.
  VBASE_IMPEXP void transformDirections (hkvVec3* HKV_RESTRICT inout_vPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   Transforms a 4 component vector.
  HKV_FORCE_INLINE const hkvVec4 transform (const hkvVec4& v) const;

  ///
  /// @}
  ///

  ///
  /// @name Scaling & Translation
  /// @{
  ///

  /// \brief
  ///   Sets the scaling of the x,y,z axis. Returns HKV_FAILURE if the scaling could not be set, because the matrix is too close to a zero matrix.
  HKV_FORCE_INLINE hkvResult setScalingFactors (const hkvVec3& vXYZ, float fEpsilon = HKVMATH_SMALL_EPSILON);
  
  /// \brief
  ///   Returns the scaling of the x,y,z axis.
  HKV_FORCE_INLINE hkvVec3 getScalingFactors () const;
  
  /// \brief
  ///   Normalizes each axis (ie. the 3x3 rotational part of the matrix).
  ///
  /// Returns HKV_FAILURE if the matrix is too close to a zero matrix and could not be normalized
  /// or if any value is invalid (ie. NaN or +/- infinity).
  /// Does not modify the matrix in case of failure.
  ///
  /// \param fEpsilon
  ///   If the length of an axis is below this value, normalization fails.
  HKV_FORCE_INLINE hkvResult normalize (float fEpsilon = HKVMATH_SMALL_EPSILON);

  /// \brief
  ///   Returns the translational part of the matrix.
  HKV_FORCE_INLINE const hkvVec3 getTranslation () const;

  /// \brief
  ///   Sets the translational part of the matrix.
  HKV_FORCE_INLINE void setTranslation (const hkvVec3& vTranslation);
  
  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///

  /// \brief
  ///   Adds rhs component-wise to this matrix.
  HKV_FORCE_INLINE void operator+= (const hkvMat4& rhs);

  /// \brief
  ///   Subtracts rhs component-wise from this matrix.
  HKV_FORCE_INLINE void operator-= (const hkvMat4& rhs);

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
  /// @name Utility Functions
  /// @{
  ///

  /// \brief
  ///   Creates a 'look-at' matrix, which will transform objects such that they are located at vStartPos 'looking at' vTargetPos.
  ///
  /// The Vision Engine uses by default the X-axis as the 'forward' direction of objects, +Y as 'left' and +Z as 'up'.
  /// In case that vLookDir and vUpDir are nearly equal an arbitrary up vector is used to create a valid matrix.
  ///
  /// \param vStartPos
  ///   The position at which the object should be located.
  ///
  /// \param vTargetPos
  ///   The position at which the object should 'look'.
  ///
  /// \param vUpDir
  ///   An optional vector to describe where 'up' is. By default (0,0,1) is used. If the look direction coincides with vUpDir,
  ///   an arbitrary other up direction is used, meaning the matrix will be valid, but it will be arbitrary rotated around the look direction.
  VBASE_IMPEXP void setLookAtMatrix (const hkvVec3& vStartPos, const hkvVec3& vTargetPos, const hkvVec3& vUpDir = hkvVec3 (0, 0, 1));


  /// \brief
  ///   Creates a symmetric perspective projection matrix. See the off-center variant of this function for more detailed documentation.
  VBASE_IMPEXP void setPerspectiveProjectionMatrix (float fViewWidth, float fViewHeight, float fNearZ, float fFarZ, hkvClipSpaceDepthRange::Enum DepthRange = hkvClipSpaceDepthRange::Native, hkvClipSpaceYRange::Enum YRange = hkvClipSpaceYRange::MinusOneToOne);

  /// \brief Creates an off-center perspective projection matrix.
  ///
  /// \param fLeft
  ///   Specifies the coordinate for the left vertical clipping plane. For symmetric projection matrices 'fLeft == -fRight'
  ///
  /// \param fRight
  ///   Specifies the coordinate for the right vertical clipping plane. For symmetric projection matrices 'fLeft == -fRight'
  ///
  /// \param fBottom
  ///   Specifies the coordinate for the bottom horizontal clipping plane. For symmetric projection matrices 'fBottom == -fTop'
  ///
  /// \param fTop
  ///   Specifies the coordinate for the top horizontal clipping plane. For symmetric projection matrices 'fBottom == -fTop'
  ///
  /// \param fNearZ
  ///   The distance of the camera to the near clipping plane. Please note that for good depth buffer precision the ratio
  ///   of the (fFarZ / fNearZ) should be as low as possible. This is best achieved by pushing the near plane out as much as possible,
  ///   but also make sure to not set the far clipping plane too far out.
  ///   Since the Vision Engine by default uses 1 unit == 1 centimeter, values for fNearZ should always be 10 or larger.
  ///
  /// \param fFarZ
  ///   The distance of the camera to the far clipping plane.
  ///
  /// \param DepthRange
  ///   Describes for which platform the projection matrix should be generated.
  ///   'MinusOneToOne': In OpenGL the depth range in clipping space is in range [-1; 1]. With -1 being at the near plane and 1 being at the far plane. Thus the projection matrix is built to transform points into this space.
  ///   'ZeroToOne':  In Direct3D the depth range in clipping space is in range [ 0; 1]. With  0 being at the near plane and 1 being at the far plane. Thus the projection matrix is built to transform points into this space.
  ///
  /// \param YRange
  ///   Describes for whether the projection matrix should flip the y-Range upside down.
  ///   'MinusOneToOne': Creates a standard projection matrix.
  ///   'OneToMinusOne': Creates a projection matrix used to flip the resulting image upside down. Note that this reverses the orientation of polygons transformed with this matrix.
  ///   Use 'ForRenderToTexture' or 'ForRenderToBackBuffer' to create a matrix which automatically flips the texture for rendering to a texture or into the back buffer.
  VBASE_IMPEXP void setPerspectiveProjectionMatrix (float fLeft, float fRight, float fBottom, float fTop, float fNearZ, float fFarZ, hkvClipSpaceDepthRange::Enum DepthRange = hkvClipSpaceDepthRange::Native, hkvClipSpaceYRange::Enum YRange = hkvClipSpaceYRange::MinusOneToOne);

  /// brief
  ///   Creates a symmetric perspective projection matrix. See the off-center variant of this function for more detailed documentation.
  ///
  /// \param fFieldOfViewX
  ///   The opening angle of the camera along the X axis, given in degree.
  ///   For example 45. The opening angle along the Y axis is computed from this value and fAspectRatioWidthDivHeight.
  ///
  /// \param fAspectRatioWidthDivHeight
  ///   The aspect ratio should match the aspect ratio of the viewport.
  ///   For example, fAspectRatioWidthDivHeight = 2.0 means the camera's angle of view is twice as wide in X as it is in Y.
  ///
  /// \param fNearZ
  ///   The distance of the camera to the near clipping plane. Please note that for good depth buffer precision the ratio
  ///   of the (fFarZ / fNearZ) should be as low as possible. This is best achieved by pushing the near plane out as much as possible,
  ///   but also make sure to not set the far clipping plane too far out.
  ///   Since the Vision Engine by default uses 1 unit == 1 centimeter, values for fNearZ should always be 10 or larger.
  ///
  /// \param fFarZ
  ///   The distance of the camera to the far clipping plane.
  ///
  /// \param DepthRange
  ///   Describes for which platform the projection matrix should be generated.
  ///   'MinusOneToOne': In OpenGL the depth range in clipping space is in range [-1; 1]. With -1 being at the near plane and 1 being at the far plane. Thus the projection matrix is built to transform points into this space.
  ///   'ZeroToOne':  In Direct3D the depth range in clipping space is in range [ 0; 1]. With  0 being at the near plane and 1 being at the far plane. Thus the projection matrix is built to transform points into this space.
  ///
  /// \param YRange
  ///   Describes for whether the projection matrix should flip the y-Range upside down.
  ///   'MinusOneToOne': Creates a standard projection matrix.
  ///   'OneToMinusOne': Creates a projection matrix used to flip the resulting image upside down. Note that this reverses the orientation of polygons transformed with this matrix.
  ///   Use 'ForRenderToTexture' or 'ForRenderToBackBuffer' to create a matrix which automatically flips the texture for rendering to a texture or into the back buffer.
  VBASE_IMPEXP void setPerspectiveProjectionMatrixFromFovX (float fFieldOfViewX, float fAspectRatioWidthDivHeight, float fNearZ, float fFarZ, hkvClipSpaceDepthRange::Enum DepthRange = hkvClipSpaceDepthRange::Native, hkvClipSpaceYRange::Enum YRange = hkvClipSpaceYRange::MinusOneToOne);

  /// brief
  ///   Creates a symmetric perspective projection matrix. See the off-center variant of this function for more detailed documentation.
  ///
  /// \param fFieldOfViewY
  ///   The opening angle of the camera along the Y axis, given in degree.
  ///   For example 45. The opening angle along the X axis is computed from this value and fAspectRatioWidthDivHeight.
  ///
  /// \param fAspectRatioWidthDivHeight
  ///   The aspect ratio should match the aspect ratio of the viewport.
  ///   For example, fAspectRatioWidthDivHeight = 2.0 means the camera's angle of view is twice as wide in X as it is in Y.
  ///
  /// \param fNearZ
  ///   The distance of the camera to the near clipping plane. Please note that for good depth buffer precision the ratio
  ///   of the (fFarZ / fNearZ) should be as low as possible. This is best achieved by pushing the near plane out as much as possible,
  ///   but also make sure to not set the far clipping plane too far out.
  ///   Since the Vision Engine by default uses 1 unit == 1 centimeter, values for fNearZ should always be 10 or larger.
  ///
  /// \param fFarZ
  ///   The distance of the camera to the far clipping plane.
  ///
  /// \param DepthRange
  ///   Describes for which platform the projection matrix should be generated.
  ///   MinusOneToOne: In OpenGL the depth range in clipping space is in range [-1; 1]. With -1 being at the near plane and 1 being at the far plane. Thus the projection matrix is built to transform points into this space.
  ///   ZeroToOne:  In Direct3D the depth range in clipping space is in range [ 0; 1]. With  0 being at the near plane and 1 being at the far plane. Thus the projection matrix is built to transform points into this space.
  ///
  /// \param YRange
  ///   Describes for whether the projection matrix should flip the y-Range upside down.
  ///   'MinusOneToOne': Creates a standard projection matrix.
  ///   'OneToMinusOne': Creates a projection matrix used to flip the resulting image upside down. Note that this reverses the orientation of polygons transformed with this matrix.
  ///   Use 'ForRenderToTexture' or 'ForRenderToBackBuffer' to create a matrix which automatically flips the texture for rendering to a texture or into the back buffer.
  VBASE_IMPEXP void setPerspectiveProjectionMatrixFromFovY (float fFieldOfViewY, float fAspectRatioWidthDivHeight, float fNearZ, float fFarZ, hkvClipSpaceDepthRange::Enum DepthRange = hkvClipSpaceDepthRange::Native, hkvClipSpaceYRange::Enum YRange = hkvClipSpaceYRange::MinusOneToOne);

  /// \brief
  ///   Creates a symmetric orthographic projection matrix. See the off-center variant of this function for more detailed documentation.
  VBASE_IMPEXP void setOrthographicProjectionMatrix (float fViewWidth, float fViewHeight, float fNearZ, float fFarZ, hkvClipSpaceDepthRange::Enum DepthRange = hkvClipSpaceDepthRange::Native, hkvClipSpaceYRange::Enum YRange = hkvClipSpaceYRange::MinusOneToOne);

  /// \brief Creates an off-center orthographic projection matrix.
  ///
  /// \param fLeft
  ///   Specifies the coordinate for the left vertical clipping plane. For symmetric projection matrices 'fLeft == -fRight'
  ///
  /// \param fRight
  ///   Specifies the coordinate for the right vertical clipping plane. For symmetric projection matrices 'fLeft == -fRight'
  ///
  /// \param fBottom
  ///   Specifies the coordinate for the bottom horizontal clipping plane. For symmetric projection matrices 'fBottom == -fTop'
  ///
  /// \param fTop
  ///   Specifies the coordinate for the top horizontal clipping plane. For symmetric projection matrices 'fBottom == -fTop'
  ///
  /// \param fNearZ
  ///   The distance of the camera to the near clipping plane.
  ///   For orthographic projections it is possible to set fNearZ to a negative value to position the near plane behind the camera.
  ///   For example in an editor it is common to position the camera somewhere around the origin, but set the clipping planes to
  ///   -1000000 and +1000000 such that everything inside this slab will be rendered.
  ///
  /// \param fFarZ
  ///   The distance of the camera to the far clipping plane.
  ///
  /// \param DepthRange
  ///   Describes for which platform the projection matrix should be generated.
  ///   MinusOneToOne: In OpenGL the depth range in clipping space is in range [-1; 1]. With -1 being at the near plane and 1 being at the far plane. Thus the projection matrix is built to transform points into this space.
  ///   ZeroToOne:  In Direct3D the depth range in clipping space is in range [ 0; 1]. With  0 being at the near plane and 1 being at the far plane. Thus the projection matrix is built to transform points into this space.
  ///
  /// \param YRange
  ///   Describes for whether the projection matrix should flip the y-Range upside down.
  ///   'MinusOneToOne': Creates a standard projection matrix.
  ///   'OneToMinusOne': Creates a projection matrix used to flip the resulting image upside down. Note that this reverses the orientation of polygons transformed with this matrix.
  ///   Use 'ForRenderToTexture' or 'ForRenderToBackBuffer' to create a matrix which automatically flips the texture for rendering to a texture or into the back buffer.
  VBASE_IMPEXP void setOrthographicProjectionMatrix (float fLeft, float fRight, float fBottom, float fTop, float fNearZ, float fFarZ, hkvClipSpaceDepthRange::Enum DepthRange = hkvClipSpaceDepthRange::Native, hkvClipSpaceYRange::Enum YRange = hkvClipSpaceYRange::MinusOneToOne);

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
  const hkvMat4* getPointer () const { return this; }

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
  hkvMat4* getPointer () { return this; }

  ///
  /// @}
  ///

public:
  
  union
  {
    float m_ElementsCM[16];             ///< Data is stored column-major
    float m_Column[4][4];               ///< Column-major 2D array

    hkvMat4_ElementSwizzle m_ElementsRM;///< Swizzle object to access data, as if it were stored row-major
    hkvMat4_ColumnSwizzle  m_Row;       ///< Swizzle object to access data with 2D indices, as if it were stored row-major
  };

  // Missing Functions:



  // Not going to be added:

  // void CreateProjectionMatrix(float perspective, float farclip, float nearclip=5.f);
  // void CreateProjectionMatrixFromFovXY(float fovX, float fovY, float farclip, float nearclip);
  // void AddObliqueClippingPlane(const hkvPlane& eyeSpacePlane);
  // bool IsProjectionMatrixSheared() const; 
};

V_DECLARE_SERIALX_NONINTRUSIVE (hkvMat4, VBASE_IMPEXP);

/// \brief
///   Checks whether the two matrices are identical.
HKV_FORCE_INLINE bool operator== (const hkvMat4& lhs, const hkvMat4& rhs);

/// \brief
///   Checks whether the two matrices are not identical.
HKV_FORCE_INLINE bool operator!= (const hkvMat4& lhs, const hkvMat4& rhs);

/// \brief
///   Adds the two matrices component-wise.
HKV_FORCE_INLINE const hkvMat4 operator+ (const hkvMat4& lhs, const hkvMat4& rhs);

/// \brief
///   Subtracts the two matrices component-wise.
HKV_FORCE_INLINE const hkvMat4 operator- (const hkvMat4& lhs, const hkvMat4& rhs);

/// \brief 
///   Returns a temporary vector that is the result of multiplying lhs with the vector rhs.
HKV_FORCE_INLINE const hkvVec3 operator* (const hkvMat4& lhs, const hkvVec3& rhs);

/// \brief 
///   Returns a temporary vector that is the result of multiplying lhs with the vector rhs.
VBASE_IMPEXP const hkvVec3d operator* (const hkvMat4& lhs, const hkvVec3d& rhs);

/// \brief 
///   Returns a temporary vector that is the result of multiplying lhs with the vector rhs.
HKV_FORCE_INLINE const hkvVec4 operator* (const hkvMat4& lhs, const hkvVec4& rhs);

#ifdef HKVMATH_ENABLE_NEW_OPERATORS
  /// \brief Multiplication Operator
  HKV_FORCE_INLINE const hkvMat4 operator* (const hkvMat4& lhs, const hkvMat4& rhs);
#else
  const hkvMat4 operator* (const hkvMat4& lhs, const hkvMat4& rhs);
#endif

/// \brief 
///   Returns a temporary matrix that has all elements of lhs multiplied by f.
HKV_FORCE_INLINE const hkvMat4 operator* (const hkvMat4& lhs, float f);

/// \brief
///   Returns a temporary matrix that has all elements of lhs multiplied by f.
HKV_FORCE_INLINE const hkvMat4 operator* (float f, const hkvMat4& rhs);

/// \brief
///   Returns a temporary matrix that has all elements of lhs divided by f.
HKV_FORCE_INLINE const hkvMat4 operator/ (const hkvMat4& lhs, float f);


#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.inl>

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
