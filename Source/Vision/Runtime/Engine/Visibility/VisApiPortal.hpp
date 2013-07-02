/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiPortal.hpp

#ifndef DEFINE_VISAPIPORTAL
#define DEFINE_VISAPIPORTAL

#ifndef SPU
#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQueryObject.hpp>
#endif

class VisVisibilityZone_cl;
class VisFrustum_cl;
class VisOcclusionQueryObject_cl;
class VisOcclusionQueryObjectPortal_cl;
class VArchive;
class IVRenderInterface;


// number of vertices without allocation:
#define VPORTAL_STATIC_VERTEX_COUNT 4


/// \brief
///   Portal class used for describing portal-based visibility relations in a scene.
/// 
/// A portal can be used to link two VisVisibilityZones_cl objects together. If this is the case,
/// and the eye point is in the source zone, it is only possible to see objects in the target zone
/// through the portal.
/// 
/// Portal-based visibility is typically well-suited for indoor scenes.
/// 
/// The portal's vertices have to be provided in counter-clockwise order; the portal plane's normal
/// has to point in the view direction. Portals are unidirectional, so if it should be possible to
/// see visibility zone A from zone B and vice versa, you need to set up two portals (one with A as
/// source and B as target, and one with B as source and A as target). Make sure to specify the
/// vertices in correct (counter-clockwise) order.
/// 
/// Valid portals have to contain at least three vertices. Note that it is not recommended to
/// create portals with very high vertex counts, since this will adversely affect performance.
class VisPortal_cl
{
public:

  /// \brief
  ///   Constructs a new empty portal. If bTemporary is set to true, no occlusion query information
  ///   is created for this portal.
  VISION_APIFUNC VisPortal_cl(bool bTemporary = false);

  /// \brief
  ///   Creates a temporary portal with external memory. Used inside visibility computation to avoid dynamic allocations
  VISION_APIFUNC VisPortal_cl(hkvVec3* pVertexMem, hkvPlane* pPlaneMem, int iMemElements);

  /// \brief
  ///   Destructor of the VisPortal_cl class.
  VISION_APIFUNC ~VisPortal_cl();

  /// \brief
  ///   Notifies the Vision engine that the portal will now be modified (by
  ///   adding/removing/altering vertices).
  /// 
  /// Calling either CreateVertices or SetVertex is only allowed inside a BeginUpdate/EndUpdate
  /// pair.
  inline void BeginUpdate()
  {
    m_iUpdateCtr++;
  }

  /// \brief
  ///   Notifies the Vision engine that the portal will now be modified (by
  ///   adding/removing/altering vertices).
  /// 
  /// Calling either CreateVertices or SetVertex is only allowed inside a BeginUpdate/EndUpdate
  /// pair.
  VISION_APIFUNC void EndUpdate();

  /// \brief
  ///   Clips the portal by the passed frustum, returning the clipping result as a new portal.
  /// 
  /// \param clipFrustum
  ///   Frustum by which to clip the portal.
  /// 
  /// \param targetPortal
  ///   Out: Portal object in which to store the clipped portal.
  /// 
  /// \return
  ///   VisClippingResult_e: The clipping operation that was performed. See the VisClippingResult_e
  ///   enumeration for details.
  VISION_APIFUNC VisClippingResult_e Clip(const VisFrustum_cl &clipFrustum, VisPortal_cl &targetPortal) const;

  /// \brief
  ///   Clips the portal by the passed plane, returning the clipping result as a new portal.
  /// 
  /// \param clipPlane
  ///   Plane by which to clip the portal.
  /// 
  /// \param targetPortal
  ///   Out: Portal object in which to store the clipped portal.
  /// 
  /// \return
  ///   VisClippingResult_e: The clipping operation that was performed. See the VisClippingResult_e
  ///   enumeration for details.
  VISION_APIFUNC VisClippingResult_e Clip(const hkvPlane& clipPlane, VisPortal_cl &targetPortal) const;

  /// \brief
  ///   Initializes (clears) the portal.
  VISION_APIFUNC void Init();

  /// \brief
  ///   Performs a deep copy of this portal from another portal.
  VISION_APIFUNC void DeepCopyFrom(const VisPortal_cl &Other);

  /// \brief
  ///   Identical to DeepCopyFrom.
  inline VisPortal_cl &operator = (const VisPortal_cl &Other) {DeepCopyFrom(Other);return *this;}

  /// \brief
  ///   Reserves the specified number of vertices for this portal. May only be called inside a
  ///   BeginUpdate/EndUpdate pair.
  VISION_APIFUNC void CreateVertices(int iNewCount);

  /// \brief
  ///   Sets a portal vertex with the specified index to the given position.
  /// 
  /// May only be called inside a BeginUpdate/EndUpdate block.
  /// 
  /// Vertices have to be provided in counter-clockwise order. In order for a portal to be valid,
  /// it has to contain at least three non-colinear vertices.
  /// 
  /// \param iIndex
  ///   Vertex index. Has to be inside the range from 0 to the number of vertices reserved with
  ///   CreateVertices.
  ///
  /// \param vVertex
  ///   Position of the portal vertex.
  inline void SetVertex(int iIndex, const hkvVec3& vVertex)
  {
    VASSERT_MSG(m_iUpdateCtr>0, "only allowed inside BeginUpdate/EndUpdate block");
    VASSERT(iIndex>=0 && iIndex<(int)m_iVertexCount);
    m_pVertices[iIndex] = vVertex;
  }

  /// \brief
  ///   Returns the number of vertices in this portal.
  inline int GetNumVertices() const { return m_iVertexCount; }

  /// \brief
  ///   Returns the vertex with the specified index.
  inline const hkvVec3& GetVertex(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<(int)m_iVertexCount)
    return (hkvVec3&) m_pVertices[iIndex];
  }

  /// \brief
  ///   Returns the vertex with the specified index.
  inline void GetVertex(int iIndex, hkvVec3& Vertex) const
  {
    Vertex = GetVertex(iIndex);
  }

  /// \brief
  ///   Returns the status of the portal. If a portal is not active, it is not possible to see
  ///   through it.
  inline bool IsActive() const { return m_bActive && HasTarget(); }

  /// \brief
  ///   Sets the status of a portal to active/inactive. If a portal is not active, it is not
  ///   possible to see through it.
  inline void SetActive(bool bActive) { m_bActive = bActive; }

  /// \brief
  ///   Indicates whether the portal has a valid source zone
  inline bool HasSource() const {return m_pSource!=NULL;}

  /// \brief
  ///   Returns the source visibility zone of this portal. Asserts if source is NULL, use HasSource
  ///   to test.
  inline VisVisibilityZone_cl *GetSource() const {VASSERT(m_pSource);return m_pSource;}

  /// \brief
  ///   Sets the source visibility zone of this portal.
  inline void SetSource(VisVisibilityZone_cl *pRoom) 
  {
    m_pSource = pRoom;
//    AssertValid();
  }

  /// \brief
  ///   Indicates whether the portal has a valid target zone
  inline bool HasTarget() const {return m_pTarget!=NULL;}

  /// \brief
  ///   Returns the target visibility zone of this portal. Asserts if target is NULL, use HasTarget
  ///   to test.
  inline VisVisibilityZone_cl *GetTarget() const {VASSERT(m_pTarget);return m_pTarget;}

  /// \brief
  ///   Sets the target visibility zone of this portal.
  inline void SetTarget(VisVisibilityZone_cl *pRoom) 
  {
    m_pTarget = pRoom;
//    AssertValid();
  }

  /// \brief
  ///   Returns the plane of the portal.
  inline const hkvPlane& GetPlane() const { return m_vPlane; }

  /// \brief
  ///   Returns one of the planes describing the portal area.
  /// 
  /// Each of these planes is defined by two of the portal vertices (side plane 0 is defined by
  /// vertices 0 and 1, and so on) and is orthogonal to the portal plane.
  /// 
  /// Side planes are convenient for classifying other geometry in respect to the portal.
  /// 
  /// \param iIndex
  ///   Number of the side plane to return. The number of side planes is always identical to the
  ///   number of portal vertices.
  /// 
  /// \return
  ///   const hkvPlane&: side plane with the specified index.
  inline const hkvPlane& GetSidePlane(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<(int)m_iVertexCount)
    return m_pSidePlanes[iIndex];
  }

  /// \brief
  ///   Returns whether a line segment specified by startPos and endPos intersects the portal.
  /// 
  /// It also returns the intersection position in intersectionPos if this is the case.
  /// 
  /// \param startPos
  ///   Starting position of the ray
  /// 
  /// \param endPos
  ///   End point of the ray
  /// 
  /// \param intersectionPos
  ///   vector in which the intersection point will be returned. Undefined if no intersection is
  ///   found.
  /// 
  /// \return
  ///   bool: true if an intersection was found, otherwise false.
  VISION_APIFUNC bool Intersects(const hkvVec3& startPos, const hkvVec3& endPos, hkvVec3& intersectionPos);

  /// \brief
  ///   Returns whether a bounding sphere intersects the portal.
  /// 
  /// \param boundingSphere
  ///   Bounding Sphere to test.
  /// 
  /// \return
  ///   bool: true if an intersection was found, otherwise false.
  VISION_APIFUNC bool Intersects(const hkvBoundingSphere& boundingSphere);

  /// \brief
  ///   Returns the reverse portal for a specified portal.
  /// 
  /// Returns the reverse portal for a specified portal, i.e. if this portal leads from visibility
  /// zone A to zone B, this function will return the portal leading from B to A.
  /// 
  /// \return
  ///   VisPortal_cl *: Reverse portal
  VISION_APIFUNC VisPortal_cl *GetReversePortal() const;


  /// \brief
  ///   Returns the occlusion query object for this portal. Used for visibility determination.
  VISION_APIFUNC inline const VisOcclusionQueryObject_cl *GetOccQueryObject() const { return (VisOcclusionQueryObject_cl*)m_pOccQueryObject; }


  /// \brief
  ///   Serializes the portal data
  VISION_APIFUNC void SerializeX( VArchive &ar );

  V_DECLARE_SERIALX( VisPortal_cl, VISION_APIFUNC )

  /// \brief
  ///   Renders the portal
  VISION_APIFUNC void DebugRender(IVRenderInterface *pRI) const;

  /// \brief
  ///   Checks whether portal properties are valid
  VISION_APIFUNC void AssertValid();

protected:
  void ComputeSidePlanes();
  void ComputePlane();
  void FreeVertices();
  void CopyDataFrom(const VisPortal_cl &refPortal);

  friend class VisFrustum_cl;

  short m_iVertexCount, m_iAllocatedVertices;
  hkvVec3* m_pVertices;          ///< always point to a valid array of m_iVertexCount vertices
  hkvPlane* m_pSidePlanes;         ///< always point to a valid array of m_iVertexCount planes
  hkvVec3 m_StaticVertices[VPORTAL_STATIC_VERTEX_COUNT];   ///< m_pVertices points to this array as long as only VPORTAL_STATIC_VERTEX_COUNT vertices are used
  hkvPlane m_StaticSidePlanes[VPORTAL_STATIC_VERTEX_COUNT];  ///< m_pSidePlanes points to this array as long as only VPORTAL_STATIC_VERTEX_COUNT vertices are used

  hkvPlane m_vPlane;       ///< plane of the potal (taken from the first three vertices)
  VisVisibilityZone_cl *m_pSource; ///< source visibility zone
  VisVisibilityZone_cl *m_pTarget; ///< target visibility zone (where the plane normal points)
  bool m_bActive, m_bOwnsAllocatedData;
  short m_iUpdateCtr; ///< for modifying the vertices

  unsigned int m_iLastFullyVisited;
  unsigned int m_iLastVisible;

  VisOcclusionQueryObjectPortal_cl *m_pOccQueryObject;      ///< This NEEDS to be a pointer!

  //unsigned int m_iPortalID;

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
