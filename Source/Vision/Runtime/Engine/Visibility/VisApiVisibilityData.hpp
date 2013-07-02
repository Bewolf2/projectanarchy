/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//! \file VisApiVisibilityData.hpp

#ifndef VISAPIVISIBILITYDATA_HPP_INCLUDED
#define VISAPIVISIBILITYDATA_HPP_INCLUDED

/// \brief
///   Class that houses members and functions for visiblity clipping: bounding box and near/far clip distances.
///
/// This class is used as common base class for VisBaseEntity_cl or VisStaticGeometryInstance_cl to add
/// visibility capabilities. The visibility collector uses convenience functions (such as IsClipped) to
/// perform visibility tests in a unified way.
class VVisibilityData
{
public:
  /// \brief
  ///   Constructor
  VISION_APIFUNC VVisibilityData();

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VVisibilityData();

  /// \brief
  ///   Sets the absolute bounding box (in world space) for this instance.
  inline void SetBoundingBox(const hkvAlignedBBox &box) 
  { 
    m_BoundingBox = box; 
  }

  /// \brief
  ///   Returns the previously set bounding box.
  inline const hkvAlignedBBox &GetBoundingBox() const 
  { 
    return m_BoundingBox; 
  }

  /// \brief
  ///   Sets the far clip distance (in world space) for this instance. A value of <=0 means no far clipping.
  inline void SetFarClipDistance(float fClipDistance)
  {
    m_fFarClipDistance = fClipDistance;
  }

    /// \brief
  ///   Returns the previously set far clip distance.
  inline float GetFarClipDistance() const
  { 
    return m_fFarClipDistance;
  }

  /// \brief
  ///   Sets the near clip distance (in world space) for this instance. A value of <=0 means no near clipping.
  inline void SetNearClipDistance(float fClipDistance)
  {
    m_fNearClipDistance = fClipDistance;
  }

  /// \brief
  ///   Returns the previously set near clip distance.
  inline float GetNearClipDistance() const
  { 
    return m_fNearClipDistance;
  }

  /// \brief
  ///   Sets the visibility bitmask for this instance. This 
  ///
  /// An object will only be rendered into a  render contexts, if the logical AND result of this
  /// bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) is not zero.
  /// By default, all bits are set.
  /// To hide an object completely in all contexts, use SetVisibleBitmask(0)
  /// 
  /// \param iMask
  ///   The new filter bitmask
  VISION_APIFUNC virtual void SetVisibleBitmask(unsigned int iMask);

  /// \brief
  ///   Returns the previously set visibility bitmask for this geometry instance.
  inline unsigned int GetVisibleBitmask() const 
  { 
    return m_iVisibleMask; 
  }

  /// \brief
  ///   Sets the near and far clip distance (in world units) and a clipping reference position to measure the distance against camera
  ///
  /// This function does not set the clipping mode. That can be set by SetClipMode after calling this function.
  /// This function sets the clipping mode to either VIS_LOD_TEST_CLIPPOSITION or VIS_LOD_TEST_NONE depending on parameter
  /// pvClipRefPosition being NULL or not.
  ///
  /// \param fNear
  ///   Near clipping distance in world units. 0 for no near clipping
  ///
  /// \param fFar
  ///   Far clipping distance in world units. 0 for no far clipping
  ///
  /// \param pvClipRefPosition
  ///   Clipping reference position. Pass NULL to disable clipping. To enable bounding box clipping, call SetClipMode afterwards
  VISION_APIFUNC void SetClipSettings(float fNear, float fFar, const hkvVec3* pvClipRefPosition);

  /// \brief
  ///   Defines how near/far clipping is determined on this instance
  ///
  /// \param iFlags
  ///   Determines the clipping behavior. There are 5 valid combinations:
  ///    \li VIS_LOD_TEST_NONE: No clipping is performed 
  ///    \li VIS_LOD_TEST_CLIPPOSITION: Clipping is performed relative to clip position (passed to SetClipSettings)
  ///    \li VIS_LOD_TEST_CLIPPOSITION|VIS_LOD_TEST_APPLYLODSCALING: Additionally applies the context's LOD scaling factor to the distance
  ///    \li VIS_LOD_TEST_BOUNDINGBOX: Distance is measured against the bounding biox
  ///    \li VIS_LOD_TEST_BOUNDINGBOX|VIS_LOD_TEST_APPLYLODSCALING: Additionally applies the context's LOD scaling factor to the distance
  inline void SetClipMode(int iFlags=VIS_LOD_TEST_CLIPPOSITION)
  {
    VASSERT_MSG((iFlags&VIS_PERFORM_LODTEST)==iFlags,"Invalid constants passed to this function");
    m_iPerformTestFlags &= (~VIS_PERFORM_LODTEST);
    m_iPerformTestFlags |= iFlags;
  }

  /// \brief
  ///   Returns bit constants of type VIS_LOD_TEST_NONE, VIS_LOD_TEST_CLIPPOSITION, VIS_LOD_TEST_BOUNDINGBOX and VIS_LOD_TEST_APPLYLODSCALING. Valid combinations are described in SetClipMode
  inline int GetClipMode(int iMask = VIS_PERFORM_LODTEST) const
  {
    return m_iPerformTestFlags & iMask;
  }

  /// \brief
  ///    Returns the clipping reference position which is used if clipping mode is set to VIS_LOD_TEST_CLIPPOSITION
  inline const hkvVec3& GetClipReference() const
  {
    return m_vClipReference;
  }

  /// \brief
  ///    Helper function for visibility loop implementations
  ///
  /// \param vCameraPos
  ///   Camera position to test LOD clipping
  /// \param fLODScaleSqr
  ///   Square of the LOD scaling factor
  ///
  /// \returns
  ///   true if the object is clipped either by near of far clip distance (if specified)
  inline bool IsNearOrFarClipped(const hkvVec3& vCameraPos, float fLODScaleSqr=1.f) const
  {
    // changes in this function have to be reflected in SPU version IsNearOrFarClipped
    int iLODMode = m_iPerformTestFlags&VIS_PERFORM_LODTEST;
    float fDistSqr;
    switch (iLODMode)
    {
    case VIS_LOD_TEST_NONE:
      return false;
    case VIS_LOD_TEST_CLIPPOSITION:
      fDistSqr = vCameraPos.getDistanceToSquared(m_vClipReference);
      break;
    case VIS_LOD_TEST_CLIPPOSITION|VIS_LOD_TEST_APPLYLODSCALING:
      fDistSqr = vCameraPos.getDistanceToSquared(m_vClipReference)*fLODScaleSqr;
      break;
    case VIS_LOD_TEST_BOUNDINGBOX:
      fDistSqr = m_BoundingBox.getDistanceToSquared(vCameraPos);
      break;
    case VIS_LOD_TEST_BOUNDINGBOX|VIS_LOD_TEST_APPLYLODSCALING:
      fDistSqr = m_BoundingBox.getDistanceToSquared(vCameraPos)*fLODScaleSqr;
      break;
    default:
      VASSERT_MSG(false,"Invalid combination of LOD flags");
      return false;
    }
    return ((m_fNearClipDistance>0.f) && (fDistSqr<(m_fNearClipDistance*m_fNearClipDistance))) 
        || ((m_fFarClipDistance>0.f) && (fDistSqr>=(m_fFarClipDistance*m_fFarClipDistance)));
  }

  /// \brief
  ///    Helper function to retrieve current distance to a given plane
  ///
  /// \param vCameraPos
  ///   Camera position to test the distance against
  /// \param fLODScaleSqr
  ///   Square of the LOD scaling factor
  /// \param fPlane
  ///   Reference plane distance, will be subtracted from final result
  ///
  /// \returns
  ///   Relative distance from object to given plane
  inline float GetDistanceToPlane(const hkvVec3& vCameraPos, float fLODScaleSqr=1.f, float fPlane=0.f) const
  {
    int iLODMode = m_iPerformTestFlags&VIS_PERFORM_LODTEST;
    float fDistSqr;
    switch (iLODMode)
    {
    case VIS_LOD_TEST_NONE:
      fDistSqr = 0.0f;
      break;
    case VIS_LOD_TEST_CLIPPOSITION:
      fDistSqr = vCameraPos.getDistanceToSquared(m_vClipReference);
      break;
    case VIS_LOD_TEST_CLIPPOSITION|VIS_LOD_TEST_APPLYLODSCALING:
      fDistSqr = vCameraPos.getDistanceToSquared(m_vClipReference)*fLODScaleSqr;
      break;
    case VIS_LOD_TEST_BOUNDINGBOX:
      fDistSqr = m_BoundingBox.getDistanceToSquared(vCameraPos);
      break;
    case VIS_LOD_TEST_BOUNDINGBOX|VIS_LOD_TEST_APPLYLODSCALING:
      fDistSqr = m_BoundingBox.getDistanceToSquared(vCameraPos)*fLODScaleSqr;
      break;
    default:
      VASSERT_MSG(false,"Invalid combination of LOD flags");
      return false;
    }

    return hkvMath::sqrt(fDistSqr) - fPlane;
  }

  /// \brief
  ///    Helper function for visibility loop implementations
  ///
  /// \param iFilterMask
  ///   Context's filter mask to test against the m_iVisibleMask member
  /// \param vCameraPos
  ///   Camera position to test LOD clipping
  /// \param fLODScaleSqr
  ///   Square of the LOD scaling factor. Pass a value < 0.0 to ignore near or far clipping
  ///
  /// \returns
  ///   true if the object is clipped either by near of far clip distance (if specified)
  inline bool IsClipped(unsigned int iFilterMask, const hkvVec3& vCameraPos, float fLODScaleSqr) const
  {
    if ((m_iVisibleMask&iFilterMask)==0 || ((m_iPerformTestFlags&(VIS_EXCLUDED_FROM_VISTEST|VIS_IS_INACTIVE)))!=0)
      return true;
    return (fLODScaleSqr<0.f) ? false : IsNearOrFarClipped(vCameraPos,fLODScaleSqr);
  }

  /// \brief
  ///    Member serialization function (not called SerializeX to avoid naming collisions in the inheritance)
  VISION_APIFUNC void Serialize_VisData(VArchive &ar);

protected:
  // KEEP THESE ITEMS TOGETHER. IF THIS STRUCTURE IS CHANGED, CHANGE
  // IT EVERYWHERE (entities, static meshes, etc.). ALSO, THE SPU CODE/
  // HEADERS NEED TO BE ADAPTED!
  // =======================================
  hkvAlignedBBox m_BoundingBox;
  unsigned int   m_iVisibleMask;
  float m_fFarClipDistance;
  int   m_iPerformTestFlags;
  float m_fNearClipDistance;
  hkvVec3 m_vClipReference;
  // =======================================
};


#endif // VISAPIVISIBILITYDATA_HPP_INCLUDED

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
