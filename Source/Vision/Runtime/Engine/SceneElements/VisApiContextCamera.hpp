/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiContextCamera.hpp

#ifndef DEFINE_VISAPICONTEXTCAMERA
#define DEFINE_VISAPICONTEXTCAMERA

#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiElementVisData.hpp>


// flags for (camera) position update, see VisCamera_cl::IncCameraPosition
#define INCPOSFLAG_NONE               0
#define INCPOSFLAG_PORTALCHECK        1

class VisContextCamera_cl;
class VisVisibilityZone_cl;
typedef VSmartPtr<VisContextCamera_cl> VisContextCameraPtr;


/// \brief
///   Class for setting cameras for invidual render contexts
/// 
/// The VisCameraContext_cl class provides functionality to create and handle cameras. Each context
/// camera has to be attached to one or more render contexts in order to take effect. See the
/// VisRenderContext_cl class as a reference. You can either attach the camera to an existing
/// VisObject3D_cl (e.g. entity) or set the camera position and orientation manually.
class VisContextCamera_cl : public VisObject3D_cl, public VisElementManager_cl<VisContextCamera_cl*>
{
public:

  /// \brief
  ///   Default Constructor for the VisContextCamera_cl class
  VISION_APIFUNC VisContextCamera_cl();

  /// \brief
  ///   Destructor for the VisContextCamera_cl class
  VISION_APIFUNC virtual ~VisContextCamera_cl();

  /// \brief
  ///   Dispose the object and remove it from scene
  VISION_APIFUNC VOVERRIDE void DisposeObject();

  /// \brief
  ///   Sets the position and orientation of the context camera in world-space.
  /// 
  /// \param cameraMatrix
  ///   Rotation of the camera in world-space.
  /// 
  /// \param position
  ///   Position of the camera in world space.
  VISION_APIFUNC void Set(const hkvMat3& cameraMatrix, const hkvVec3& position);  

  /// \brief
  ///   Attaches the camera to a specific entity.
  /// 
  /// It also sets a position and orientation
  /// relative to the entity which will be used to calculate the final position of the camera.
  /// 
  /// Internally, this function uses the AttachToParent, SetLocalPosition and SetLocalOrientation
  /// methods exposed by the VisObject3D_cl class.
  /// 
  /// \param entity
  ///   entity to which the camera will be attached
  /// 
  /// \param spawnPos
  ///   position relative to the entity's origin
  /// 
  /// \param rotationMatrix
  ///   orientation relative to the entity's orientation
  VISION_APIFUNC void AttachToEntity(VisBaseEntity_cl *entity, const hkvVec3& spawnPos, const hkvMat3* rotationMatrix = NULL);

  /// \brief
  ///   Returns the current view direction vector of the camera. DEPRECATED: Use the variant with the return value instead.
  /// 
  /// This function stores the current view direction of the camera.
  /// 
  /// The view direction is a normalized vector in world space.
  /// 
  /// \param direction
  ///   vector reference to store the direction
  /// 
  /// \sa VisCamera_cl::GetPosition
  /// \sa VisCamera_cl::GetRotation
  HKV_DEPRECATED_2012_2 VISION_APIFUNC void GetDirection(hkvVec3& direction) const;

  /// \brief
  ///   Returns the direction of the camera
  /// 
  /// This function returns the current world space direction of the camera
  /// 
  /// The view direction is a normalized vector in world space.
  /// 
  /// \return
  ///   hkvVec3& direction: current camera direction vector
  VISION_APIFUNC const hkvVec3 GetDirection() const;

  /// \brief
  ///   Returns the visibility zone the camera is currently in.
  VISION_APIFUNC VisVisibilityZone_cl *GetVisibilityZone() const;

  /// \brief
  ///   Returns whether this context camera is the currently active camera
  /// 
  /// \return
  ///   BOOL bActive: TRUE if this context camera is the currently active camera
  /// 
  /// \sa VisRenderContext_cl::Activate()
  VISION_APIFUNC bool IsActive() const;

  /// \brief
  ///   Gets the current inverse eye matrix of the camera. DEPRECATED: Use 'GetWorldToCameraTransformation' or 'GetCameraToWorldTransformation' instead.
  HKV_DEPRECATED_2012_2 VISION_APIFUNC void GetInverseEyeMatrix(hkvMat4& ieMatrix) const;

  /// \brief
  ///   Updates the camera matrix. Must be called by before the scene is rendered.
  VISION_APIFUNC void Update();


  /// \brief
  ///   Gets the current 3x3 rotation matrix of the camera. DEPRECATED: Use 'GetWorldToCameraRotation' instead.
  HKV_DEPRECATED_2012_2 VISION_APIFUNC void GetWorldRotationMatrix(hkvMat3& wrMatrix) const;

  /// \brief
  ///   Gets the current 3x3 rotation matrix that rotates direction vectors from camera space to world space.
  VISION_APIFUNC hkvMat3 GetCameraToWorldRotation() const;

  /// \brief
  ///   Gets the current 3x3 rotation matrix that rotates direction vectors from world space to camera space.
  VISION_APIFUNC hkvMat3 GetWorldToCameraRotation() const;

  /// \brief
  ///   Gets the current 4x4 transformation matrix that transforms points from camera space to world space.
  VISION_APIFUNC hkvMat4 GetCameraToWorldTransformation() const;

  /// \brief
  ///   Gets the current 4x4 transformation matrix that transforms points from world space to camera space.
  VISION_APIFUNC hkvMat4 GetWorldToCameraTransformation() const;

  /// \brief
  ///   Gets the current 4x4 transformation matrix of the camera. DEPRECATED: Use 'GetCameraToWorldTransformation' or 'GetWorldToCameraTransformation' instead.
  HKV_DEPRECATED_2012_2 VISION_APIFUNC void GetWorldMatrix (hkvMat4& wMatrix) const;

  /// \brief
  ///   Returns the frame during which the last teleportation (i.e. call to ReComputeVisibility)
  ///   occurred.
  VISION_APIFUNC unsigned int GetLastTeleported() const;

  VISION_APIFUNC VOVERRIDE void ModSysNotifyFunctionParentDestroyed();
  VISION_APIFUNC VOVERRIDE void ModSysNotifyFunctionOnDisconnect(BOOL bDestroy);

private:
  void SetCoreCamera();

  friend class VisRenderContext_cl;
  friend class Vision;
  friend class VisGame_cl;
};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisContextCamera_cl)

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
