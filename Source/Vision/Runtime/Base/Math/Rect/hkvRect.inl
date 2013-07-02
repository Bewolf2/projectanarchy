/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVRECT_INL
#define VBASE_HKVMATH_HKVRECT_INL

#include <Vision/Runtime/Base/Math/Rect/hkvRect.h>
 
int hkvRect::getLeft() const
{
  return m_left;
}

int hkvRect::getX() const
{
  return m_left;
}

int hkvRect::getTop() const
{
  return m_top;
}

int hkvRect::getY() const
{
  return m_top;
}

int hkvRect::getRight() const
{
  return m_right;
}

int hkvRect::getBottom() const
{
  return m_bottom;
}

int hkvRect::getWidth() const
{
  return m_right - m_left;
}

int hkvRect::getHeight() const
{
  return m_bottom - m_top;
}

void hkvRect::setLeft(int left)
{
  m_left = left;
}

void hkvRect::setX(int x)
{
  m_left = x;
}

void hkvRect::setTop(int top)
{
  m_top = top;
}

void hkvRect::setY(int y)
{
  m_top = y;
}


void hkvRect::setRight(int right)
{
  m_right = right;
}

void hkvRect::setBottom(int bottom)
{
  m_bottom = bottom;
}

void hkvRect::setWidth(int width)
{
  m_right = m_left + width;
}

void hkvRect::setHeight(int height)
{
  m_bottom = m_top + height;
}

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
