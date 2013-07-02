/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLine.hpp

#ifndef _VISION_VLINE_HPP
#define _VISION_VLINE_HPP


#ifndef _VISION_DOC

class VLine
{
public:
  VBASE_IMPEXP VLine();
  VBASE_IMPEXP VLine(const hkvVec3& v1, const hkvVec3& v2);

  // Set the line based on two vertices
  VBASE_IMPEXP void SetValue(const hkvVec3& v1, const hkvVec3& v2);

  // Set the line based on one vertex and a direction
  VBASE_IMPEXP void SetValueDir(const hkvVec3& pos, const hkvVec3& dir);

  VBASE_IMPEXP VBool GetClosestPoints(const VLine &line2, hkvVec3& ptOnThis, hkvVec3& ptOnLine2) const;

  /// \brief
  ///   get distance of the given point from this line
  /// 
  /// this method calculates distance of the point from this line
  /// 
  /// \param pt
  ///   [in] point of which distance is being calculated
  /// 
  /// \param pClosestPoint
  ///   [out] if nonzero then after the method finishes this param contains a point on the line
  ///   which has minimum distance from the tested point
  /// 
  /// \return
  ///   distance of the point from this line

  VBASE_IMPEXP float GetDistance( const hkvVec3& pt, hkvVec3* pClosestPoint = 0 ) const;

//data
  hkvVec3 pos;
  hkvVec3 dir;

};


#endif //_VISION_DOC

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
