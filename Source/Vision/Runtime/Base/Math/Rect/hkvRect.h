/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVRECT_H
#define VBASE_HKVMATH_HKVRECT_H

#include <Vision/Runtime/Base/Math/hkvMath.h>

/// \brief An integer rectangle in the plane.
///
/// The bounds of the rectangle are stored as its top left (inclusive) and
/// bottom right (exclusive) corners.
///
/// The origin of the plane is considered to be in the top left so that
/// valid coordinate values satisfy top < bottom and left < right.
///
/// The top left is considered as the reference position. Methods that modify
/// width and height will leave this corner unchanged and move the bottom right
/// instead.
///
/// This class is intended to be compatible with the WinApi RECT and Direct3D's D3DRECT struct.
class hkvRect
{
public:
  /// \brief Constructs a rectangle without initialization.
  hkvRect()
  {
  }

  /// \brief Constructs the rectangle from a given top left corner and width/height.
  hkvRect(int x, int y, int width, int height) : m_left(x), m_top(y), m_right(x + width), m_bottom(y + height)
  {
  }
  
  /// \brief Returns the x-coordinate of the rectangle's left edge.
  HKV_FORCE_INLINE int getLeft() const;

  /// \brief Alias for getLeft().
  HKV_FORCE_INLINE int getX() const;

  /// \brief Returns the y-coordinate of the rectangle's top edge.
  HKV_FORCE_INLINE int getTop() const;

  /// \brief Alias for getTop().
  HKV_FORCE_INLINE int getY() const;

  /// \brief Returns the x-coordinate of the rectangle's right edge.
  HKV_FORCE_INLINE int getRight() const;

  /// \brief Returns the y-coordinate of the rectangle's bottom edge.
  HKV_FORCE_INLINE int getBottom() const;

  /// \brief Returns the width of the rectangle as right - left.
  HKV_FORCE_INLINE int getWidth() const;

  /// \brief Returns the height of the rectangle as bottom - top.
  HKV_FORCE_INLINE int getHeight() const;


  /// \brief Sets the x-coordinate of the rectangle's left edge.
  HKV_FORCE_INLINE void setLeft(int left);

  /// \brief Alias for setLeft().
  HKV_FORCE_INLINE void setX(int x);

  /// \brief Sets the y-coordinate of the rectangle's top edge.
  HKV_FORCE_INLINE void setTop(int top);

  /// \brief Alias for setTop().
  HKV_FORCE_INLINE void setY(int y);

  /// \brief Sets the x-coordinate of the rectangle's right edge.
  HKV_FORCE_INLINE void setRight(int right);

  /// \brief Sets the y-coordinate of the rectangle's bottom edge.
  HKV_FORCE_INLINE void setBottom(int bottom);

  /// \brief Sets the width of the rectangle. This modifies the right edge, the left is unchanged.
  HKV_FORCE_INLINE void setWidth(int width);

  /// \brief Sets the height of the rectangle. This modifies the bottom edge, the top is unchanged.
  HKV_FORCE_INLINE void setHeight(int height);

private:
  int m_left;
  int m_top;
  int m_right;
  int m_bottom;
};

#include <Vision/Runtime/Base/Math/Rect/hkvRect.inl>

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
