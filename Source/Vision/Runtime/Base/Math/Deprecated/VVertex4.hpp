/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VVertex4.hpp

#ifndef DC_VVERTEX4_HPP
#define DC_VVERTEX4_HPP

#include <Vision/Runtime/Base/System/IO/Serialization/VArchive.hpp>

#include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>

// since the engine rather uses VisVector_cl ignore this class
#ifndef _VISION_DOC

HKVMATH_DEPRECATED_STAGE1 class VVertex4f
{
public:
  V_DECLARE_SERIALX( VVertex4f, VBASE_IMPEXP )
  inline void SerializeX( VArchive& ar )
  {
    if (ar.IsLoading())
      ar >> x >> y >> z >> w;
    else
      ar << x << y << z << w;
  }


  union
  {
    struct { float x,y,z,w; };
    struct { float data[4]; };
  };

  HKVMATH_DEPRECATED_STAGE1 inline VVertex4f()                                                            { Zero(); }
  HKVMATH_DEPRECATED_STAGE1 inline VVertex4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
  HKVMATH_DEPRECATED_STAGE1 inline VVertex4f(const VVertex4f &v) : x(v.x), y(v.y), z(v.z), w(v.w)         { }

  HKV_DISABLE_DEPRECATION

  inline void Zero()                                                            { x = y = z = w = 0.f; }

  inline float & operator[] (const int i)                                       { return data[i]; }
  inline VVertex4f operator -() const                                           { return VVertex4f(-x,-y,-z,-w); }

  inline VVertex4f operator *(float f) const                                    { return VVertex4f(x*f, y*f, z*f, w*f); }
  inline VVertex4f operator /(float f) const                                    { return VVertex4f(x/f, y/f, z/f, w/f); }
  inline VVertex4f operator +(const VVertex4f& rhs) const                       { return VVertex4f(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w); }
  inline VVertex4f operator -(const VVertex4f& rhs) const                       { return VVertex4f(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w); }

  inline VVertex4f& operator += (float f)                                       { x+=f; y+=f; z+=f; w+=f; return *this; }
  inline VVertex4f& operator -= (float f)                                       { x-=f; y-=f; z-=f; w-=f; return *this; }
  inline VVertex4f& operator *= (float f)                                       { x*=f; y*=f; z*=f; w*=f; return *this; }
  inline VVertex4f& operator /= (float f)                                       { x/=f; y/=f; z/=f; w/=f; return *this; }

  inline VVertex4f& operator += (const VVertex4f& rhs)                          { x+=rhs.x; y+=rhs.y; z+=rhs.z; w+=rhs.w; return *this; }
  inline VVertex4f& operator -= (const VVertex4f& rhs)                          { x-=rhs.x; y-=rhs.y; z-=rhs.z; w-=rhs.w; return *this; }
 
  inline bool operator == (const VVertex4f& rhs) const                          { return  IsEqual(rhs); }
  inline bool operator != (const VVertex4f& rhs) const                          { return !IsEqual(rhs); }

  inline void Set     (float _x, float _y, float _z, float _w)                  { x = _x; y = _y; z = _z; w = _w; }
  inline void SetValue(float _x, float _y, float _z, float _w)                  { Set(_x,_y,_z,_w); }

  inline bool IsEqual(const VVertex4f &rhs, float eps) const
  {
    return (data[0] <= rhs.data[0] + eps) &&
           (data[0] >= rhs.data[0] - eps) &&
           (data[1] <= rhs.data[1] + eps) &&
           (data[1] >= rhs.data[1] - eps) &&
           (data[2] <= rhs.data[2] + eps) &&
           (data[2] >= rhs.data[2] - eps) &&
           (data[3] <= rhs.data[3] + eps) &&
           (data[3] >= rhs.data[3] - eps);
  }
  
  inline bool IsEqual(const VVertex4f &rhs) const
  {
    return (data[0] == rhs.data[0]) &&
           (data[1] == rhs.data[1]) &&
           (data[2] == rhs.data[2]) &&
           (data[3] == rhs.data[3]);
  }
  
  inline void Min(const VVertex4f &vClamp)
  {
    if (x>vClamp.x) x=vClamp.x;
    if (y>vClamp.y) y=vClamp.y;
    if (z>vClamp.z) z=vClamp.z;
    if (w>vClamp.w) w=vClamp.w;
  }

  inline void Max(const VVertex4f &vClamp)
  {
    if (x<vClamp.x) x=vClamp.x;
    if (y<vClamp.y) y=vClamp.y;
    if (z<vClamp.z) z=vClamp.z;
    if (w<vClamp.w) w=vClamp.w;
  }

  inline float GetSquaredLen() const
  {
    return (x*x+y*y+z*z+w*w);
  }

  inline float GetLen() const
  {
    return hkvMath::sqrt (x*x+y*y+z*z+w*w);
  }

  inline void Normalize()
  {
    float fLength = GetLen();
    if(fLength==0) 
      return;
    float fInvLength = 1.0f/fLength;
    if(x!=0) x *= fInvLength;
    if(y!=0) y *= fInvLength;
    if(z!=0) z *= fInvLength;
    if(w!=0) w *= fInvLength;
  }

  HKV_ENABLE_DEPRECATION
};

#endif // _VISION_DOC


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
