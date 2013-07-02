/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiFrustum.hpp

#ifndef DEFINE_VISAPIFRUSTUM
#define DEFINE_VISAPIFRUSTUM

#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>
#include <Vision/Runtime/Base/Math/Plane/hkvPlane.h>

// Forward declarations
class VisPortal_cl;

#define GETPLANE(PLANE) (((hkvPlane* )&m_Planes)[PLANE])
#define GETPLANEPTR(PLANE) ((hkvPlane* )&(m_Planes[PLANE*4]))
  
#define VIS_FRUSTUM_MAX_PLANES 32


/// \brief
///   Frustum class for n-sided frustums (perspective/orthographic), optionally supporting capping
///   planes.
/// 
/// This class provides functionality for easily creating n-sided frustums
/// (perspective/orthographic) and using them for overlap checks (e.g. visibility tests).
/// 
/// The render collection classes provide functionality for filtering collections of entities,
/// static scene elements etc. according to the spatial relation to a frustum.
/// 
/// Frustum planes have to point outside of the frustum. For performance reasons, no sanity check
/// is performed; the best way to verify the orientation of frustum planes is to draw the frustum
/// using the Vision::Game.DrawFrustum method with normals enabled. The normals have to point away
/// from the frustum.
/// 
/// If a near clap plane is contained in the frustum, it is always at plane index 0.
/// 
/// If a far cap plane is contained in the frustum, it is at plane index 0 if no near cap plane is
/// present; otherwise, it is at plane index 1.
class VisFrustum_cl
{
  public:

    /// \brief
    ///   Constructor for an empty frustum instance
    VISION_APIFUNC VisFrustum_cl();

    /// \brief
    ///   Destructor of the frustum class
    VISION_APIFUNC ~VisFrustum_cl() {}

    /// \brief
    ///   Generates a perspective frustum based on a bounding box
    /// 
    /// Creates four frustum planes that pass through the origin and fully include the passed
    /// bounding box.
    /// 
    /// Optionally creates a near cap plane that separates the origin from the bounding box and
    /// touches the bounding box
    /// 
    /// Optionally creates a far cap plane at a user-specified distance from the origin.
    /// 
    /// \param origin
    ///   Reference to the origin vector
    /// 
    /// \param box
    ///   Reference to the bounding box
    /// 
    /// \param bCreateNearCapPlane
    ///   If set to true, a near cap plane (as defined above) will be created
    /// 
    /// \param fFarCapDistance
    ///   Distance from the origin to the far cap plane. If this value is 0.f, no far clip plane
    ///   will be generated.
    /// 
    /// \return
    ///   bool: true if the frustum could be generated, otherwise false (e.g. because the origin is
    ///   too close to the bounding box).
    VISION_APIFUNC bool Set(const hkvVec3& origin, const hkvAlignedBBox &box, bool bCreateNearCapPlane, float fFarCapDistance=0.f);

    /// \brief
    ///   Generates a perspective frustum based on position, orientation, and field of view angle 
    /// 
    /// Generates a frustum based on position, orientation, and field of view angle 
    /// 
    /// Creates four frustum planes that pass through the specified position. The frustum's
    /// orientation is specified by a 3x3 matrix, and the frustum's field of view values are passed
    /// as scalars.
    /// 
    /// Optionally creates a near cap plane at a user-specified distance from the origin.
    /// 
    /// Optionally creates a far cap plane at a user-specified distance from the origin.
    /// 
    /// \param origin
    ///   Reference to the position vector
    /// 
    /// \param rotMatrix
    ///   Orientation of the frustum
    /// 
    /// \param fFovX
    ///   Horizontal field of view
    /// 
    /// \param fFovY
    ///   Vertical field of view
    /// 
    /// \param fNearCapDistance
    ///   Distance from the position to the near cap plane. If this value is 0.f, no near clip
    ///   plane will be generated.
    /// 
    /// \param fFarCapDistance
    ///   Distance from the position to the far cap plane. If this value is 0.f, no far clip plane
    ///   will be generated.
    ///
    /// \param bFlipped
    ///   Set to true to flip all frustum planes if matrix is mirrored.
    /// 
    /// \return
    ///   bool: true if the frustum could successfully be generated.
    VISION_APIFUNC bool Set(const hkvVec3& origin, const hkvMat3& rotMatrix, float fFovX, float fFovY, float fNearCapDistance = 5.0f, float fFarCapDistance = 32000.0f, bool bFlipped = false);

    /// \brief
    ///   Extracts a frustum from a modelview-projection matrix.
    ///
    /// \param projMatrix
    ///   The projection matrix.  This can be either a simple projection, or a modelview-projection matrix.
    ///
    /// \param bFlipped
    ///   Set to true to flip all frustum planes if matrix is mirrored.
    /// 
    /// \return
    ///   bool: true if the frustum could successfully be generated.
    VISION_APIFUNC bool set(const hkvMat4 &projMatrix, bool bFlipped = false);

    /// \brief DEPRECATED: Use 'set' instead, but keep in mind that 'projMatrix' has been transposed.
    HKV_DEPRECATED_2012_3 VISION_APIFUNC bool Set(const hkvMat4 &projMatrix, bool bFlipped = false)
    {
      return set(projMatrix.getTransposed(), bFlipped);
    }

    /// \brief
    ///   Extracts a frustum from a camera matrix and projection matrix.
    ///
    /// \param cameraMatrix
    ///   The camera rotation matrix.
    ///
    /// \param cameraPos
    ///   The camera position.
    ///
    /// \param projMatrix
    ///   The projection matrix.
    ///
    /// \param bFlipped
    ///   Set to true to flip all frustum planes if matrix is mirrored.
    /// 
    /// \return
    ///   bool: true if the frustum could successfully be generated.
    VISION_APIFUNC bool set(const hkvVec3 &cameraPos, const hkvMat3 &cameraMatrix, const hkvMat4 &projMatrix, bool bFlipped = false);

    /// \brief DEPRECATED: Use 'set' instead, but keep in mind that the projection matrix has been transposed.
    HKV_DEPRECATED_2012_3 VISION_APIFUNC bool Set(const hkvVec3 &cameraPos, const hkvMat3 &cameraMatrix, const hkvMat4 &projMatrix, bool bFlipped = false)
    {
      return set(cameraPos, cameraMatrix, projMatrix.getTransposed(), bFlipped);
    }

    

    /// \brief
    ///   Generates an orthographic frustum based on position, orientation, width/height and
    ///   near/far clip planes
    /// 
    /// Creates four frustum planes centered around the specified position. The frustum's
    /// orientation is specified by a 3x3 matrix, and the frustum's width and height are passed as
    /// scalars.
    /// 
    /// Optionally creates a near cap plane at a user-specified distance from the origin.
    /// 
    /// Optionally creates a far cap plane at a user-specified distance from the origin.
    /// 
    /// \param origin
    ///   Reference to the position vector. Note that this is not a position the frustum planes
    ///   pass through, since this method generates an orthographic frustum
    /// 
    /// \param rotMatrix
    ///   Orientation of the frustum
    /// 
    /// \param fWidth
    ///   Width of the frustum, in units.
    /// 
    /// \param fHeight
    ///   Height of the frustum, in units.
    /// 
    /// \param nearCapDistance
    ///   Distance from the position to the near cap plane. If this value is 0.f, no near clip
    ///   plane will be generated.
    /// 
    /// \param farCapDistance
    ///   Distance from the position to the far cap plane. If this value is 0.f, no far clip plane
    ///   will be generated.
    /// 
    /// \param bFlipped
    ///   Set to true to generate a flipped (inside-out) frustum.
    /// 
    /// \return
    ///   bool: true if the frustum could successfully be generated.
    VISION_APIFUNC bool SetOrthographic(const hkvVec3& origin, const hkvMat3& rotMatrix, float fWidth, float fHeight, float nearCapDistance = 5.0f, float farCapDistance = 32000.0f, bool bFlipped = false);

    /// \brief
    ///   Returns a single plane of the frustum
    /// 
    /// \param iIndex
    ///   index of the respective plane (0..GetNumPlanes())
    /// 
    /// \param Plane
    ///   Reference to a hkvPlane object which will be filled with the plane's data.
    inline void GetPlane(unsigned int iIndex, hkvPlane& Plane) const
    {
      VASSERT(iIndex < m_iPlaneCounter)
      Plane = GETPLANE(iIndex);
    }

    /// \brief
    ///   Returns a single plane of the frustum
    /// 
    /// \param iIndex
    ///   index of the respective plane (0..GetNumPlanes())
    /// 
    /// \return
    ///   const hkvPlane* pPlane: Pointer to the plane object.
    inline const hkvPlane* GetPlane(unsigned int iIndex) const
    {
      VASSERT(iIndex < m_iPlaneCounter)
      return GETPLANEPTR(iIndex);
    }

    /// \brief
    ///   Returns the number of planes defining the frustum
    /// 
    /// \return
    ///   int: The number of planes in the frustum
    inline unsigned int GetNumPlanes() const;

    /// \brief
    ///   Returns a pointer to the frustum's near clip plane if it has one, otherwise NULL.
    VISION_APIFUNC hkvPlane* GetNearPlane();

    /// \brief
    ///   Returns a pointer to the frustum's far clip plane if it has one, otherwise NULL.
    VISION_APIFUNC hkvPlane* GetFarPlane();


    /// \brief
    ///   Method for setting the planes in the frustum.
    /// 
    /// Using this method, you can set the planes in the frustum manually.
    /// 
    /// \param origin
    ///   Origin position for frustum
    /// 
    /// \param iPlaneCounter
    ///   number of planes to set
    /// 
    /// \param pPlanes
    ///   Array of hkvPlane objects (will be copied to the frustum)
    /// 
    /// \return
    ///   bool: true if the operation was successful.
    VISION_APIFUNC bool SetPlanes(const hkvVec3& origin, int iPlaneCounter, const hkvPlane* pPlanes);

    /// \brief
    ///   Clears all planes in the frustum.
    inline void Clear() {m_iPlaneCounter = 0; m_pCurrentSourcePortal=0;}

    /// \brief
    ///   Returns whether the passed point lies inside the frustum
    /// 
    /// Returns TRUE if the passed point is inside the frustum.
    /// 
    /// \param point
    ///   Reference to the point
    /// 
    /// \return
    ///   BOOL: TRUE if the point is inside the frustum.
    VISION_APIFUNC inline bool Overlaps(const hkvVec3& point) const;

#ifndef SPU

    /// \brief
    ///   Returns whether the frustum overlaps the passed Bounding Sphere
    /// 
    /// Returns TRUE if the frustum overlaps the bounding sphere.
    /// 
    /// \param sphere
    ///   Reference to the bounding sphere
    /// 
    /// \return
    ///   BOOL: TRUE if the frustum overlaps the bounding sphere.
    VISION_APIFUNC inline bool Overlaps(const hkvBoundingSphere& sphere) const;

#endif

    /// \brief
    ///   Returns whether the frustum overlaps the passed Bounding Box
    /// 
    /// Returns TRUE if the frustum overlaps the bounding box.
    /// 
    /// \param box
    ///   Reference to the bounding box
    /// 
    /// \return
    ///   BOOL: TRUE if the frustum overlaps the bounding box.
    VISION_APIFUNC inline bool Overlaps(const hkvAlignedBBox &box) const;
    
    /// \brief
    ///   Returns whether the frustum overlaps the passed Bounding Box, allowing the user to pass
    ///   information about the planes which are relevent for the frustum test.
    /// 
    /// Each bit in the iPlaneFlags parameter represents one of the frustum planes, with the least
    /// significant bit referring to plane index 0. If a plane flag is not set, it is assumed that
    /// the box is guaranteed to be fully on the negative side of the respective frustum plane (so
    /// that a test against this frustum plane can never lead to the bounding box being classified
    /// as lying outside of the frustum).
    /// 
    /// Can be used in Conjunction with the ClassifyPlanes method to optimize frustum overlap
    /// tests.
    /// 
    /// Returns TRUE if the frustum overlaps the bounding box.
    /// 
    /// \param box
    ///   Reference to the bounding box
    /// 
    /// \param iPlaneFlags
    ///   Out: Plane flags, each bit representing one of the frustum planes.
    ///
    /// \return
    ///   BOOL: TRUE if the frustum overlaps the bounding box.
    ///
    /// \sa
    ///   ClassifyPlanes
    VISION_APIFUNC inline bool Overlaps(const hkvAlignedBBox &box, int iPlaneFlags) const;

#ifndef SPU

    /// \brief
    ///   Classifies the frustum planes against the passed bounding box.
    /// 
    /// The resulting flags can be passed to the VisFrustum_cl::Overlaps method in order to speed
    /// up box overlap tests for bounding boxes which are guaranteed to be fully contained in the
    /// box used for classifying the planes.
    /// 
    /// Each bit in the returned iPlaneFlags parameter represents one of the frustum planes, with
    /// the least significant bit referring to plane index 0. An unset plane flag signifies that
    /// the box (and everything inside it) is  fully on the negative side of the respective frustum
    /// plane (so that a test against this frustum plane can never lead to the contents of the box
    /// being classified as lying outside of the frustum).
    /// 
    /// Returns TRUE if the frustum overlaps the bounding box.
    /// 
    /// \param box
    ///   Reference to the bounding box
    ///
    /// \param iPlaneFlags
    ///   Out: Plane flags, each bit representing one of the frustum planes.
    /// 
    /// \return
    ///   BOOL: TRUE if the frustum overlaps the bounding box.
    VISION_APIFUNC VisClippingResult_e ClassifyPlanes(const hkvAlignedBBox &box, int &iPlaneFlags) const;

#endif

    /// \brief
    ///   Creates a deep copy of the passed frustum.
    VISION_APIFUNC void CopyFrom(const VisFrustum_cl &sourceFrustum);


    //Internal use only
    #ifndef SPU
    VISION_APIFUNC bool Set(const hkvVec3& Origin, const VisPortal_cl &Portal);
    VISION_APIFUNC bool Set(const hkvPlane& vFarPlane, const hkvVec3& Origin, const VisPortal_cl &Portal);
    #endif
    VISION_APIFUNC void AddPlane(const hkvPlane& Plane);
    VISION_APIFUNC hkvPlane& NextPlane();
    
    VISION_APIFUNC bool HasFarCapPlane() const { return m_bHasFarPlane; }
    VISION_APIFUNC bool HasNearCapPlane() const { return m_bHasNearPlane; }

    inline const hkvVec3& GetOrigin() const { return m_vOrigin; }

    
  private:

    friend class VisPortal_cl;

    void CreateFrustumPlanesFromBox(const hkvVec3& basePos, const hkvVec3& nearCapPos, const hkvPlane& nearCapPlane, const hkvAlignedBBox &box);
    
    // Note: This array is simply an array of floats to avoid calling the destructor of hkvPlane multiple times!
    float m_Planes[VIS_FRUSTUM_MAX_PLANES * 4];
    unsigned int m_iPlaneCounter;
    VisPortal_cl *m_pCurrentSourcePortal;
    hkvVec3 m_vOrigin;

    bool m_bHasFarPlane;
    bool m_bHasNearPlane;
};

#include <Vision/Runtime/Engine/SceneElements/VisApiFrustum.inl>

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
