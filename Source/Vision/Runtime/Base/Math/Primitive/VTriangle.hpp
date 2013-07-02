/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTriangle.hpp

#ifndef VBASE_VTRIANGLE_HPP
#define VBASE_VTRIANGLE_HPP

/// \brief
///   Triangle representation that uses 3 pointers to 3 hkvVec3 objects.
class VTriangle
{
public:

  /// \brief
  ///   Contructor. Sets all pointer to NULL.
  VTriangle() { m_pVertex[0] = m_pVertex[1] = m_pVertex[2] = NULL; }

  /// \brief
  ///   Checks whether all vertices are set
  inline bool IsValid() const;

  /// \brief
  ///   Sets the vertex pointers. Points are not copied, so do not use local variables.
  inline void SetPoints(const hkvVec3* p0,const hkvVec3* p1,const hkvVec3* p2);

  /// \brief
  ///   Gets position of vertex #0
  inline const hkvVec3& GetVertex0() const {return *m_pVertex[0];}

  /// \brief
  ///   Gets position of vertex #1
  inline const hkvVec3& GetVertex1() const {return *m_pVertex[1];}

  /// \brief
  ///   Gets position of vertex #2
  inline const hkvVec3& GetVertex2() const {return *m_pVertex[2];}

  /// \brief
  ///   Gets pointer to vertex #0
  inline const hkvVec3* GetVertexPtr0() const {return m_pVertex[0];}

  /// \brief
  ///   Gets pointer to vertex #1
  inline const hkvVec3* GetVertexPtr1() const {return m_pVertex[1];}

  /// \brief
  ///   Gets pointer to vertex #2
  inline const hkvVec3* GetVertexPtr2() const {return m_pVertex[2];}


  /// \brief
  ///   Gets the triangle edge. iIndex must be 0 or 1.
  inline hkvVec3 VISION_FASTCALL GetEdge(int iIndex) const;

  /// \brief
  ///   Computes the normal of the triangle by its cross product
  inline hkvVec3 GetNormal() const;

  /// \brief
  ///   Computes the area of this triangle (square units)
  inline float GetArea() const;

  /// \brief
  ///   Performs a trace intersection test on this triangle. For a hit, it stores the distance in
  ///   fDist.
  VBASE_IMPEXP bool GetTraceIntersection(const hkvVec3& traceStart, const hkvVec3& rayDir, bool bDoubleSided, float &fDist) const;

  /// \brief
  ///   Computes the mapping parameter for a position projected onto triangle
  VBASE_IMPEXP bool GetParameter(const hkvVec3& pos, hkvVec2& destUV) const;

  /// \brief
  ///   Computes the mapping parameter for a position projected onto triangle and uses the result
  ///   to interpolate between the three uv-coordinates.
  VBASE_IMPEXP bool GetMappingCoords(const hkvVec3& pos, hkvVec2& destUV, const hkvVec2& uv0,const hkvVec2& uv1, const hkvVec2& uv2) const;

  /// \brief
  ///   Helper function to render this triangle. Should be used for debugging only.
  VBASE_IMPEXP void Render(IVRenderInterface *pRenderer, VColorRef iColor, const VSimpleRenderState_t &state, int iFlags, const hkvMat4 *pCustomTransform) const;

  /// \brief
  ///   Returns the plane of this triangle
  inline hkvPlane GetPlane (bool bFlipNormal = false) const { hkvPlane p; p.setFromPoints (*m_pVertex[0], *m_pVertex[1], *m_pVertex[2], hkvTriangleOrientation::ClockWise); if (bFlipNormal) p.flip (); return p;}

private:
  const hkvVec3* m_pVertex[3]; ///< just a pointer to vectors
};


//////////////////////////////////////////////////////////////////////////////
// VTriangle : Inline functions
//////////////////////////////////////////////////////////////////////////////

inline bool VTriangle::IsValid() const 
{
  return (m_pVertex[0]!=NULL && m_pVertex[1]!=NULL && m_pVertex[2]!=NULL);
}

inline void VTriangle::SetPoints(const hkvVec3* p0,const hkvVec3* p1,const hkvVec3* p2)
{
  m_pVertex[0]=p0;
  m_pVertex[1]=p1;
  m_pVertex[2]=p2;
}

inline hkvVec3 VISION_FASTCALL VTriangle::GetEdge(int iIndex) const
{
  VASSERT(iIndex==0 || iIndex==1);
  return (*m_pVertex[iIndex+1])-(*m_pVertex[0]);
}

inline hkvVec3 VTriangle::GetNormal() const
{
  VASSERT(IsValid()); ///< all pointers must be set
  hkvVec3 normal = GetEdge(0);
  normal = normal.cross (GetEdge(1));
  normal.normalize();
  return normal;
}

inline float VTriangle::GetArea() const
{
  VASSERT(IsValid()); ///< all pointers must be set
  hkvVec3 normal = GetEdge(0);
  normal = normal.cross(GetEdge(1));
  return normal.getLength() * 0.5f;
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
