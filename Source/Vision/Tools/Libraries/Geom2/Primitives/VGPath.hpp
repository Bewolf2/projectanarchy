/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Path class (for putting splines/paths in a scene, etc.)
class VGEOM2_IMPEXP_CLASS VGPath : public VGNode
{
public:
  /// \brief
  ///   Tanglent types for control points
  enum TangentType
  {
    VGTT_CUSTOM,
    VGTT_LINEAR,
    VGTT_SMOOTH,
    VGTT_NUM
  };


  /// \brief
  ///   Actual control points of this spline
  struct ControlPoint
  {
    TangentType inType, outType;
    hkvVec3 pos, inHandle, outHandle;

    inline void Set(const hkvVec3& p, const hkvVec3& ih, const hkvVec3& oh, TangentType it, TangentType ot)  { pos = p; inHandle = ih; outHandle = oh; inType = it; outType = ot; }
  };



public:
  VGPath() : m_closed(false)    { }


  /// \brief
  ///   Adds a new control point
  ///
  /// \param p
  ///   Control point to add - will be copied
  inline void                AddControlPoint(const ControlPoint& p)   { m_controlPoints.Add(p); }  // @@@ test

  /// \brief
  ///   Creates a new control point and returns a reference to it (same as implicitly adding a new control point)
  ///
  /// \return
  ///   Reference to newly added control point
  inline ControlPoint&       CreateControlPoint()                     { m_controlPoints.Add(ControlPoint()); return GetControlPoint(GetNumControlPoints()-1); }  // @@@ test

  /// \brief
  ///   Returns number of control points
  ///
  /// \return
  ///   Number of control points
  inline int                 GetNumControlPoints() const              { return m_controlPoints.GetSize(); }  // @@@ test

  /// \brief
  ///   Returns a reference to a control point
  ///
  /// \param i
  ///   Control point index
  ///
  /// \return
  ///   Control point
  inline       ControlPoint& GetControlPoint(int i)                   { return m_controlPoints[i]; }  // @@@ test
  inline const ControlPoint& GetControlPoint(int i) const             { return m_controlPoints[i]; }  // @@@ test

  /// \brief
  ///   Removes a control point (handle with care, b/c removing a control point will change the indices of other control points, of course)
  ///
  /// \param i
  ///   Control point index
  inline void                RemoveControlPoint(int i)                { m_controlPoints[i]; m_controlPoints.RemoveAt(i); }  // @@@ test



  /// \brief
  ///   Gets the path's @@@
  ///
  /// \return
  ///   Currently set path @@@
  inline bool                GetIsClosed() const throw()              { return m_closed; }  // @@@ test

  /// \brief
  ///   Sets the path's @@@
  ///
  /// \param b
  ///   @@@
  inline void                SetIsClosed(bool b)                      { m_closed = b; }  // @@@ test



private:
  VArray<ControlPoint, const ControlPoint&> m_controlPoints;
  bool                                      m_closed;
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
