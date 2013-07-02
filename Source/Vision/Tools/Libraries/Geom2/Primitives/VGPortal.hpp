/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Portal class
///
/// A portal is basically a polygon describing the shape of the portal, and a
/// front / back ID pair. IDs matching the triangle properties' visibilityID
/// are considered being in the same visibility zone as the portal's front or
/// back. A portal is thus bidirectional.
/// Again, it's up to the user to define the space the portal's shape will be
/// in, however, the Vision bakend code interprets it as parent space (equals
/// world space in this case).
class VGEOM2_IMPEXP_CLASS VGPortal : public VGNameable/*@@@ replace with VGNode*/
{
public:
  VGPortal() : VGNameable(""), m_visibilityID_front(NO_VIS_ID_ASSIGNED), m_visibilityID_back(NO_VIS_ID_ASSIGNED), m_leadsToOutside(false), m_singleSided(false) { }
  virtual ~VGPortal()                                                                                                                     { }


  /// \brief
  ///   Spartan polygon used by this portal class
  typedef VArray<hkvVec3, const hkvVec3&> Polygon;

  /// \brief
  ///   Visibility zone ID for "not assigned to zone"
  static const int NO_VIS_ID_ASSIGNED = INT_MIN;



  /// \brief
  ///   Gets the visibility ID for zone this portal connects to (on the front side)
  ///
  /// \return
  ///   Visibility ID
  inline int            GetVisibilityID_Front() const throw()   { return m_visibilityID_front; }

  /// \brief
  ///   Sets the visibility ID for zone this portal connects to (on the front side)
  ///
  /// \param id
  ///   Visibility ID
  inline void           SetVisibilityID_Front(int id) throw()   { m_visibilityID_front = id; }

  /// \brief
  ///   Gets the visibility ID for zone this portal connects to (on the back side)
  ///
  /// \return
  ///   Visibility ID
  inline int            GetVisibilityID_Back() const throw()    { return m_visibilityID_back; }

  /// \brief
  ///   Sets the visibility ID for zone this portal connects to (on the back side)
  ///
  /// \param id
  ///   Visibility ID
  inline void           SetVisibilityID_Back(int id) throw()    { m_visibilityID_back = id; }



  /// \brief
  ///   Sets custom flag whether the portal leads to outside or not
  ///
  /// \param b
  ///   Flag
  inline void           SetLeadsToOutside(bool b) throw()       { m_leadsToOutside = b; }

  /// \brief
  ///   Gets custom flag whether the portal leads to outside or not
  ///
  /// \return
  ///   Flag
  inline bool           GetLeadsToOutside() const throw()       { return m_leadsToOutside; }


  /// \brief
  ///   Sets whether the portal is single sided or not
  ///
  /// \param b
  ///   Flag
  inline void           SetSingleSided(bool b) throw()       { m_singleSided = b; }

  /// \brief
  ///   Gets whether the portal is single sided or not
  ///
  /// \return
  ///   Flag
  inline bool           GetSingleSided() const throw()       { return m_singleSided; }


  /// \brief
  ///   Gets the portal's shape (polygon) - should be convex and flat
  ///
  /// \return
  ///   Portal's shape (polygon)
  inline const Polygon& GetShape() const                        { return m_shape; }
  inline       Polygon& GetShape()                              { return m_shape; }

  /// \brief
  ///   Sets the portal's shape (polygon)
  ///
  /// \param s
  ///   Portal's shape (polygon)
  inline void           SetShape(const Polygon& s)              { m_shape = s; }

  /// \brief
  ///   Checks if portal polygon's shape is valid (meaning if points lie in a plane, more than 3 points - no checking for convexivity, though)
  ///
  /// \return
  ///   Validity of shape
  inline bool           IsValidShape() const                    { bool b=true; int n=m_shape.GetSize(); if(n<3) b=false; else { hkvPlane p (hkvNoInitialization); p.setFromPoints (m_shape[0], m_shape[1], m_shape[2], hkvTriangleOrientation::CounterClockWise); for(int i=3; (i<n)&&b; ++i) { if(p.getPointPosition (m_shape[i], HKVMATH_LARGE_EPSILON) != hkvPlanePosition::Coplanar) b=false; } } return b; }



private:
  int m_visibilityID_front, m_visibilityID_back;
  bool m_leadsToOutside, m_singleSided;
  Polygon m_shape;
};

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
