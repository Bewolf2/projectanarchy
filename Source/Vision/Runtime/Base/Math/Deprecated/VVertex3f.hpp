/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VVertex3f.hpp

#ifndef VVERTEX3F_HPP_INCLUDED
#define VVERTEX3F_HPP_INCLUDED

#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>


/// \brief
///   This is the engine's default class for 3 component vectors.
HKVMATH_DEPRECATED_STAGE1 class VVertex3f
{
  V_DECLARE_SERIALX( VVertex3f, VBASE_IMPEXP )

public:
  /// \brief
  ///   Constructor that initializes (0,0,0)
  HKVMATH_DEPRECATED_STAGE1 inline VVertex3f()  {x=y=z=0.f;}
  
  /// \brief
  ///   Constructor that leaves the members uninitialized
  HKVMATH_DEPRECATED_STAGE1 explicit inline VVertex3f(bool bUninitialized) {}
  
  /// \brief
  ///   Constructor
  HKVMATH_DEPRECATED_STAGE1 inline VVertex3f(float _x,float _y,float _z) : x(_x),y(_y),z(_z)  
  {
  }

  /// \brief
  ///   Copy constructor
  HKVMATH_DEPRECATED_STAGE1 inline VVertex3f(const VVertex3f &p): x(p.x),y(p.y),z(p.z) {}

  HKV_DISABLE_DEPRECATION

  /// \brief
  ///   Sets (0,0,0)
  inline void Zero()
  {
    x=y=z=0.f;
  }

  /// \brief
  ///   Indicates whether all components are zero using hkvMath::isZero
  inline VBool IsZero() const
  { 
    return hkvMath::isZero(x) && hkvMath::isZero(y) && hkvMath::isZero(z); 
  }

  /// \brief
  ///   Comparison operator (no epsilon)
  inline VBool operator== ( const VVertex3f& rhs) const
  {
    return IsEqual(rhs);
  }

  /// \brief
  ///   Comparison operator (no epsilon)
  inline VBool operator!= ( const VVertex3f& rhs) const
  {
    return !IsEqual(rhs);
  }

  /// \brief
  ///   Assignment operator that can assign from other vector classes
  ///
  /// <tt>C</tt> must be a structure/class containing the attributes <tt>x</tt>, <tt>y</tt> and <tt>z</tt>.
  ///
  ///  For example:
  ///  \code
  ///  // [...]
  ///  struct{float x,y,z;}myvec;
  ///  VVertex3f vvec;
  ///
  ///  myvec.x = myvec.y = myvec.z = 42.f;
  ///
  ///  vvec = myvec;
  ///  // [...]
  ///  \endcode
  ///
  /// \param rhs
  ///   The vertex from which to assign
  template<class C> VVertex3f& operator=(const C& rhs)
  {
    x = (float)rhs.x;
    y = (float)rhs.y;
    z = (float)rhs.z;

    return *this;    
  }
  
  /// \brief
  ///   Adds another vector
  inline void operator+= ( const VVertex3f& l)
  {
	  x += l.x;
	  y += l.y;
	  z += l.z;
  }

  /// \brief
  ///   Subtracts another vector
  inline void operator-= ( const VVertex3f& l)
  {
	  x -= l.x;
	  y -= l.y;
	  z -= l.z;
  }

  
  /// \brief
  ///   Divides by a scalar
  inline VVertex3f& operator /=( float fScalar )
  {
    fScalar = 1.f/fScalar;
		x *= fScalar;
		y *= fScalar;
		z *= fScalar;
		return *this;
  }

  /// \brief
  ///   Multiplies with a scalar
  inline VVertex3f& operator *=( float fScalar )
  {
		x *= fScalar;
		y *= fScalar;
		z *= fScalar;
		return *this;
  }
  
  /// \brief
  ///   Adds a vector
  inline VVertex3f operator+(const VVertex3f &p ) const
  {
	  return VVertex3f(x + p.x, y + p.y, z + p.z );
  }

  /// \brief
  ///   Subtracts a vector
  inline VVertex3f operator-(const VVertex3f &p ) const
  {
	  return VVertex3f(x - p.x, y - p.y, z - p.z );
  }

  /// \brief
  ///   Multiplies with a scalar
  inline VVertex3f operator*(const float fScalar ) const
  {
	  return VVertex3f(x*fScalar,y*fScalar,z*fScalar);
  }

  /// \brief
  ///   Divides by a scalar
  inline VVertex3f operator/(const float fScalar ) const
  {
    float f = 1.f/fScalar;
	  return VVertex3f(x*f,y*f,z*f);
  }
  
  /// \brief
  ///   Negates the vector
  inline VVertex3f operator-() const
  {
	  return VVertex3f(-x,-y,-z);
  }
  
  /// \brief
  ///   float pointer cast operator
  inline operator float *()
  {
    return &x;
  }

  /// \brief
  ///   Sets all components
  inline void SetValue(float _x,float _y,float _z)
  {
    x = _x; y = _y; z = _z;
  }

  /// \brief
  ///   Sets all components
  inline void Set(float _x,float _y,float _z)
  {
    SetValue(_x, _y, _z);
  }
  
  /// \brief
  ///   Sets all components
  inline void SetValue(const VVertex3f &rhs)
  {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
  }
  
  /// \brief
  ///   Returns the dot product
  inline float Dot(const VVertex3f &p) const
  {
    return (x * p.x + y * p.y + z * p.z);
  }

  /// \brief
  ///   Sets this vector to the cross product of this vector and the other vector using right hand rule (a.Cross(b) => a = a x b)
  inline void Cross( const VVertex3f &other)
  {
	  float _x = y*other.z - z*other.y;
	  float _y = z*other.x - x*other.z;
	  float _z = x*other.y - y*other.x;
	  x = _x;
	  y = _y;
	  z = _z;
  }

  /// \brief
  ///   Tries to normalize this vector. Returns TRUE if successful
  inline VBool Normalize()
  {
	  float len = GetLength();

	  if (len<FLT_EPSILON)
      return FALSE;

    len = 1.f/len;
    x*=len;
    y*=len;
    z*=len;

    return TRUE;
  }

  /// \brief
  ///   Returns the length of the vector
  inline float Length() const
  {
    return hkvMath::sqrt (x*x + y*y + z*z);
  }

  /// \brief
  ///   Returns the length of the vector
  inline float GetLength() const { return Length(); }

  /// \brief
  ///  Returns the squared length of the vector
  inline float LengthSqr() const
  {
    return (x*x + y*y + z*z);
  }


  /// \brief
  ///  Sets the length of the vector. Returns TRUE if successful (i.e. length was not zero)
  VBool SetLength(float fNewLen)
  {
    float fLen = Length();
    if (fLen<=HKVMATH_LARGE_EPSILON)
      return FALSE;
    fLen = fNewLen/fLen;
    x*=fLen; y*=fLen; z*=fLen;
    return TRUE;
  }

  /// \brief
  ///   Returns the distance between this vector and the passed vector
  inline float Dist(const VVertex3f &v2) const
  {
    return hkvMath::sqrt (DistSqr(v2));
  }
  
  /// \brief
  ///   Returns the squared distance between this vector and the passed vector
  inline float DistSqr(const VVertex3f &v2) const
  {
    float a = v2.x - x;
    float r = a*a;
    
    a = v2.y - y;
    r += a*a;
    
    a = v2.z - z;
    return  r + a*a;
  }

  /// \brief
  ///   '*' operator is overloaded as the dot product
  float operator *(const VVertex3f &other) const
  {
    return Dot(other);
  }
  
  /// \brief
  ///   Compares two vectors (using component wise epsilon)
  inline bool IsEqual(const VVertex3f &p, float delta) const
  {
    float t = x - p.x; if (t<0) t = -t;
    if (t > delta) return false;
    
    t = y - p.y; if (t<0) t = -t;
    if (t > delta) return false;
    
    t = z - p.z; if (t<0) t = -t;
    if (t > delta) return false;
  
    return true;
  }

  /// \brief
  ///   Compares two vectors (exact match)
  inline bool IsEqual(const VVertex3f &rhs) const
  {
    return  (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
  }


  /// \brief
  ///   Accesses component by index (0..2).
  inline float &operator[] (int i)
  { 
    VASSERT_PARANOID(i>=0 && i<3);
    return data[i];
  }

  /// \brief
  ///   Accesses component by index (0..2).
  const float& operator[] (int i) const
  { 
    VASSERT_PARANOID(i>=0 && i<3);
    return data[i];
  } 

  /// \brief
  ///   Makes the passed vector orthogonal to this vector and normalizes it.
  inline void MakeOrthogonal(VVertex3f &v)
  {
    // get the distance v is along this vector
    float dot = v.x*x + v.y*y + v.z*z;

    //subtract the distance to make it ortogonal
    v.x -= (float) (dot*x);
    v.y -= (float) (dot*y);
    v.z -= (float) (dot*z);

    v.Normalize();
  }

  union
  {
    struct {float x,y,z;};
    float data[3];
    float m[3];
  };


  // Multiply the current vector with a 4x4 matrix
  //Input  : const VisMatrix4x4_cl &matrix: the matrix will be used for the multiplication.
  //Output : Nothing
  inline void MultiplyWith4x4Matrix(const float *pfMatrixScalars)
  {
    float oldX=x, oldY=y, oldZ=z;
    x = oldX*pfMatrixScalars[0]+oldY*pfMatrixScalars[1]+oldZ*pfMatrixScalars[2]+pfMatrixScalars[3];
    y = oldX*pfMatrixScalars[4]+oldY*pfMatrixScalars[5]+oldZ*pfMatrixScalars[6]+pfMatrixScalars[7];
    z = oldX*pfMatrixScalars[8]+oldY*pfMatrixScalars[9]+oldZ*pfMatrixScalars[10]+pfMatrixScalars[11];
  }

  // Multiply the current vector with the rotational part of the 4x4 matrix
  //Input  : const VisMatrix4x4_cl &matrix: the rotational part of this matrix will be used for the multiplication.
  //Output : Nothing
  inline void MultiplyWithRotationalPart(const float *pfMatrixScalars)
  {
    float oldX=x, oldY=y, oldZ=z;
    x = oldX*pfMatrixScalars[0]+oldY*pfMatrixScalars[1]+oldZ*pfMatrixScalars[2];
    y = oldX*pfMatrixScalars[4]+oldY*pfMatrixScalars[5]+oldZ*pfMatrixScalars[6];
    z = oldX*pfMatrixScalars[8]+oldY*pfMatrixScalars[9]+oldZ*pfMatrixScalars[10];
  }

  HKV_ENABLE_DEPRECATION
  
  typedef float COMPONENT_TYPE;  //so we can detect the type (float/double) from outside
  /// \brief
  ///   Serializes the 3 components
  VBASE_IMPEXP void SerializeX( VArchive& ar );
protected:

};

HKV_DISABLE_DEPRECATION

inline VVertex3f operator *( const float f, const VVertex3f &v )
{
  return v * f;
}

inline VVertex3f Cross( const VVertex3f& v1, const VVertex3f& v2 )
{
  VVertex3f v( v1 );
  v.Cross( v2 );
  return v;
}

HKV_ENABLE_DEPRECATION

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
