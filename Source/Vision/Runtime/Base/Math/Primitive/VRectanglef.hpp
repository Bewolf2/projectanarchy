/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRectanglef.hpp

#ifndef DC_VRECTANGLEF_HPP
#define DC_VRECTANGLEF_HPP

/// \brief
///   Class that represents a 2D rectangle with float coordinates
class VRectanglef
{
public:
  hkvVec2 m_vMin;
  hkvVec2 m_vMax;

  V_DECLARE_SERIALX( VRectanglef, VBASE_IMPEXP )
  inline void SerializeX( VArchive& ar )
  {
    if (ar.IsLoading())
      ar >> m_vMin >> m_vMax;
    else
      ar << m_vMin << m_vMax;
  }

  /// \brief
  ///   Contructor. Resets the rectangle
  inline VRectanglef()
  {
    Reset();
  }

  /// \brief
  ///   Contructor that copies from another rectangle
  inline VRectanglef(const VRectanglef &copy)
  {
    m_vMin = copy.m_vMin;
    m_vMax = copy.m_vMax;
  }

  /// \brief
  ///   Contructor that copies from min/max coordinates
  inline VRectanglef(const hkvVec2& vmin, const hkvVec2& vmax)
  {
    m_vMin = vmin;
    m_vMax = vmax;
  }

  /// \brief
  ///   Contructor that copies from 4 coordinates
  inline VRectanglef(float x1, float y1, float x2, float y2)
  {
    m_vMin.set(x1,y1);
    m_vMax.set(x2,y2);
  }

  /// \brief
  ///   This constructor version creates an uninitialized rectangle
  explicit inline VRectanglef(bool bUninitialized) : m_vMin(hkvNoInitialization), m_vMax(hkvNoInitialization)
  {
  }

  /// \brief
  ///   Resets the rectangle so that min>max. IsValid will return false
  inline void Reset()
  {
    m_vMax.set(-FLT_MAX,-FLT_MAX);
    m_vMin.set(FLT_MAX,FLT_MAX);
  }
  
  /// \brief
  ///   Offsets the rectangle by a 2D vector
  inline VRectanglef operator +(const hkvVec2& rhs) const
  {
    return VRectanglef( m_vMin+rhs, m_vMax+rhs);
  }
   
  /// \brief
  ///   Offsets the rectangle by a 2D vector
  inline VRectanglef operator -(const hkvVec2& rhs) const
  {
    return VRectanglef( m_vMin-rhs, m_vMax-rhs);
  }
  
  /// \brief
  ///   Offsets the rectangle by a 2D vector
  inline VRectanglef& operator += (const hkvVec2& rhs)
  {
    m_vMin += rhs;
    m_vMax += rhs;
    return *this;
  }

  /// \brief
  ///   Offsets the rectangle by a 2D vector
  inline VRectanglef& operator -= (const hkvVec2& rhs)
  {
    m_vMin -= rhs;
    m_vMax -= rhs;
    return *this;
  }
 
  /// \brief
  ///   Scales the rectangle by a 2D vector componentwise
  inline VRectanglef& operator *= (const hkvVec2& rhs)
  {
    m_vMin.x *= rhs.x;
    m_vMin.y *= rhs.y;
    m_vMax.x *= rhs.x;
    m_vMax.y *= rhs.y;
    return *this;
  }

  /// \brief
  ///   Scales the rectangle by a scalar
  inline VRectanglef& operator *= (float fScalar)
  {
    m_vMin.x *= fScalar;
    m_vMin.y *= fScalar;
    m_vMax.x *= fScalar;
    m_vMax.y *= fScalar;
    return *this;
  }

  /// \brief
  ///   Sets the 2 corners of the rectangle
  inline void Set(float x1, float y1, float x2, float y2)
  {
    m_vMin.set(x1,y1);
    m_vMax.set(x2,y2);
  }

  /// \brief
  ///   Sets the 2 corners of the rectangle
  inline void Set(const hkvVec2& vmin, const hkvVec2& vmax)
  {
    m_vMin = vmin;
    m_vMax = vmax;
  }
  

  /// \brief
  ///   Inflates the rectangle by the passed point. The point will be inside afterwards.
  inline void Add(const hkvVec2& p) 
  {
    m_vMin.setMin(p);
    m_vMax.setMax(p);
  }

  /// \brief
  ///   Builds the union of the two rectangles
  inline void Add(const VRectanglef &rect) 
  {
    VASSERT(rect.IsValid());
    Add(rect.m_vMin);
    Add(rect.m_vMax);
  }

  /// \brief
  ///   Adds a border of fBorder units in all directions
  inline void Add(float fBorder) 
  {
    m_vMin.x -= fBorder;
    m_vMin.y -= fBorder;
    m_vMax.x += fBorder;
    m_vMax.y += fBorder;
  }

  /// \brief
  ///   Indicates whether min<=max
  inline bool IsValid() const
  {
    return (m_vMax.x>=m_vMin.x) && (m_vMax.y>=m_vMin.y);
  }

  /// \brief
  ///   re-arranges min and max values so that max>min
  inline void MakeValid()
  {
    Set(hkvMath::Min(m_vMin.x,m_vMax.x), hkvMath::Min(m_vMin.y,m_vMax.y), hkvMath::Max(m_vMin.x,m_vMax.x), hkvMath::Max(m_vMin.y,m_vMax.y));
  }

  /// \brief
  ///   Returns the x-extent of the rectangle (also for non-valid rects)
  inline float GetSizeX() const 
  {
    return m_vMax.x-m_vMin.x;
  }

  /// \brief
  ///   Returns the y-extent of the rectangle (also for non-valid rects)
  inline float GetSizeY() const
  {
    return m_vMax.y-m_vMin.y;
  }

  /// \brief
  ///   Returns the extent of the rectangle, i.e. its diagonal (also for non-valid rects)
  inline hkvVec2 GetSize() const
  {
    return m_vMax-m_vMin;
  }

  /// \brief
  ///   Returns the center of the rectangle
  inline hkvVec2 GetCenter() const
  {
    return (m_vMin+m_vMax)*0.5f;
  }

  /// \brief
  ///   Calculates the area of the rectangle. Returns -1 for non-valid rectangles (min>max)
  inline float GetArea() const
  {
    if (!IsValid())
      return -1.f;
    return GetSizeX()*GetSizeY();
  }

  /// \brief
  ///   Indicates whether a point (x,y) is inside the rect
  inline bool IsInside(float x, float y) const
  {
    return (x>=m_vMin.x && x<=m_vMax.x && y>=m_vMin.y && y<=m_vMax.y);
  }

  /// \brief
  ///   Indicates whether a point p is inside the rect
  inline bool IsInside(const hkvVec2& p) const
  {
    return IsInside(p.x,p.y);
  }

  /// \brief
  ///   Clamps the point coordinates of p to the rectangle
  inline void Clamp(hkvVec2& p) const
  {
    if (p.x<m_vMin.x) p.x=m_vMin.x;
    if (p.x>m_vMax.x) p.x=m_vMax.x;
    if (p.y<m_vMin.y) p.y=m_vMin.y;
    if (p.y>m_vMax.y) p.y=m_vMax.y;
  }

  /// \brief
  ///   Indicates whether the passed rectangle intersects with this rectangle
  inline bool Touches(const VRectanglef &other) const
  {
    if ( m_vMin.x > other.m_vMax.x || m_vMax.x < other.m_vMin.x  ||
         m_vMin.y > other.m_vMax.y || m_vMax.y < other.m_vMin.y)
      return false;
    return true;
  }

  /// \brief
  ///   Indicates whether the passed rectangle is completely inside this rectangle
  inline bool IsCompletelyInside(const VRectanglef &other) const
  {
    return IsInside(other.m_vMin) && IsInside(other.m_vMax);
  }


  /// \brief
  ///   Returns a rectangle that is the intersection of this and other rectangle. Return rect is
  ///   not valid if there is no intersection.
  inline VRectanglef GetIntersection(const VRectanglef &other) const
  {
    VRectanglef rect(other);
    rect.m_vMin.setMax(m_vMin);
    rect.m_vMax.setMin(m_vMax);
    return rect;
  }
  
  /// \brief
  ///   Equal operator, exact match.
  inline bool operator==(const VRectanglef &other) const
  {
    return m_vMin == other.m_vMin && m_vMax == other.m_vMax;
  }
  
  /// \brief
  ///   Not equal operator, exact match.
  inline bool operator!=(const VRectanglef &other) const
  {
    return m_vMin != other.m_vMin || m_vMax != other.m_vMax;
  }

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
