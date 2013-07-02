/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VVertex2.hpp

#ifndef DC_VVERTEX2_HPP
#define DC_VVERTEX2_HPP

class VArchive;

/// \brief
///   This is the engine's default class for 2 component vectors.
HKVMATH_DEPRECATED_STAGE1 class VVertex2f
{
public:
  V_DECLARE_SERIALX( VVertex2f, VBASE_IMPEXP )

  /// \brief
  ///   Serializes the 2 components.
  inline void SerializeX( VArchive& ar )
  {
    if (ar.IsLoading())
      ar >> x >> y;
    else
      ar << x << y;
  }


  union
  {
    struct { float x,y; };
    struct { float u,v; };
    struct { float uv[2]; };
    struct { float data[2]; };
  };

  /// \brief
  ///   Constructor that initializes (0,0).
  HKVMATH_DEPRECATED_STAGE1 inline VVertex2f()
  {
    Zero();
  }

  /// \brief
  ///   Constructor that leaves the members uninitialized.
  HKVMATH_DEPRECATED_STAGE1 explicit inline VVertex2f(bool bUninitialized) {}

  /// \brief
  ///   Constructor
  HKVMATH_DEPRECATED_STAGE1 inline VVertex2f(float _x, float _y)
  {
    x = _x;
    y = _y;
  }

  /// \brief
  ///   Copy constructor.
  HKVMATH_DEPRECATED_STAGE1 inline VVertex2f(const VVertex2f &copy)
  {
    x = copy.x;
    y = copy.y;
  }

  /// \brief
  ///   Sets (0,0).
  inline void Zero()
  {
    x = y = 0.f;
  }

  HKV_DISABLE_DEPRECATION
  
  /// \brief
  ///   Multiplies with a scalar.
  inline VVertex2f operator *(float f) const
  {
    return VVertex2f( x*f, y*f);  
  }

  /// \brief
  ///   Adds another vector.
  inline VVertex2f operator +(const VVertex2f &rhs) const
  {
    return VVertex2f( x+rhs.x, y+rhs.y);
  }
   
  /// \brief
  ///   Subtracts another vector.
  inline VVertex2f operator -(const VVertex2f& rhs) const
  {
    return VVertex2f( x-rhs.x, y-rhs.y);  
  }
  
  /// \brief
  ///   Negates the vector.
  inline VVertex2f operator -() const
  {
    return VVertex2f(-x,-y);
  }

  /// \brief
  ///   Accesses the component (index must be 0 or 1).
  inline float & operator[] (const int i)
  {
    VASSERT(i>=0 && i<2);
    return uv[i];
  }

  /// \brief
  ///   Accesses the component (index must be 0 or 1).
  inline float operator[] (const int i) const
  {
    return uv[i];
  }

  /// \brief
  ///   Adds a scalar to both components.
  inline VVertex2f& operator += (float f)
  {
    x += f;
    y += f;
    return *this;
  }

  /// \brief
  ///   Subtracts a scalar to both components.
  inline VVertex2f& operator -= (float f)
  {
    x -= f;
    y -= f;
    return *this;
  }

  /// \brief
  ///   Adds another vector.
  inline VVertex2f& operator += (const VVertex2f& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  /// \brief
  ///   Subtracts another vector.
  inline VVertex2f& operator -= (const VVertex2f& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }
 
  /// \brief
  ///   Divides by a scalar.
  inline VVertex2f& operator /= (float f)
  {
    float fScalar = 1.f/f;
    x *= fScalar;
    y *= fScalar;
    return *this;
  }

  /// \brief
  ///   Multiplies with a scalar.
  inline VVertex2f& operator *= (float f)
  {
    x *= f;
    y *= f;
    return *this;
  }

  /// \brief
  ///   Equal operator, uses IsEqual function (no epsilon).
  inline bool operator == (const VVertex2f& rhs) const
  {
    return IsEqual(rhs);
  }

  /// \brief
  ///   Equal operator, uses IsEqual function (no epsilon).
  inline bool operator != (const VVertex2f& rhs) const
  {
    return !IsEqual(rhs);
  }

  /// \brief
  ///   Sets both components.
  inline void Set(float _x, float _y)
  {
    x = _x;
    y = _y;
  }

  /// \brief
  ///   Sets both components.
  inline void SetValue(float _x, float _y) { Set(_x,_y); }

  /// \brief
  ///   Compares two vectors (using component wise epsilon).
  inline bool IsEqual(const VVertex2f &rhs, float eps) const
  {
    return  (uv[0] <= rhs.uv[0] + eps) &&
            (uv[0] >= rhs.uv[0] - eps) &&
            (uv[1] <= rhs.uv[1] + eps) &&
            (uv[1] >= rhs.uv[1] - eps);
  }
  
  /// \brief
  ///   Compares two vectors (exact match).
  inline bool IsEqual(const VVertex2f &rhs) const
  {
    return  (uv[0] == rhs.uv[0]) && (uv[1] == rhs.uv[1]);
  }
  
  /// \brief
  ///   Sets componentwise minimum.
  inline void Min(const VVertex2f &vClamp)
  {
    if (x>vClamp.x) x=vClamp.x;
    if (y>vClamp.y) y=vClamp.y;
  }

  /// \brief
  ///   Sets componentwise maximum.
  inline void Max(const VVertex2f &vClamp)
  {
    if (x<vClamp.x) x=vClamp.x;
    if (y<vClamp.y) y=vClamp.y;
  }

  /// \brief
  ///   Returns the squared length of this vector.
  inline float GetLengthSqr() const
  {
    return x*x+y*y;
  }

  /// \brief
  ///   Returns the length of this vector.
  inline float GetLength() const
  {
    return hkvMath::sqrt (x*x+y*y);
  }

  /// \brief
  ///   Normalizes this vector. Leaves the vector unmodified if its length is zero.
  inline void Normalize()
  {
    float len = GetLength();
    if (len<FLT_EPSILON)
      return;
    len=1.f/len;
    x*=len;
    y*=len;
  }

  HKV_ENABLE_DEPRECATION
};

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
