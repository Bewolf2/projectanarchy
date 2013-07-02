/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiCamera.hpp

#ifndef DEFINE_VISAPICAMERA
#define DEFINE_VISAPICAMERA

#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiElementVisData.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiContextCamera.hpp>


// flags for (camera) position update, see VisCamera_cl::IncCameraPosition
#define INCPOSFLAG_NONE               0
#define INCPOSFLAG_PORTALCHECK        1

class VisVisibilityZone_cl;


/// \brief
///   Class for setting the the main camera. 
/// 
/// It is recommended to use the VisContextCamera_cl class instead of this class. VisCamera_cl is
/// still provided for reasons of backwards compatibility, though.
/// 
/// The main camera is essentially just a context camera, and all the methods in this class wrap
/// around the corresponding methods in the VisContextCamera_cl class.
/// 
/// Note that this class will be deprecated in future versions of the Vision engine!
class VisCamera_cl
{
public :

  /// \brief
  ///   Sets the position vector and orientation matrix of the main camera.
  /// 
  /// \param cameraMatrix
  ///   rotation matrix of the camera
  /// 
  /// \param position
  ///   position of the camera in world space
  VISION_APIFUNC void Set(const hkvMat3& cameraMatrix, const hkvVec3& position);  
  
  
  /// \brief
  ///   Attaches the main camera to a given entity and sets a position relative to the entity.
  /// 
  /// The entity will be used to calculate the final position of the camera.
  /// 
  /// \param entity
  ///   entity to which the camera will be attached
  /// 
  /// \param spawnPos
  ///   position relative to the entity
  /// 
  /// \param rotationMatrix
  ///   orientation relative to the entity
  VISION_APIFUNC void AttachToEntity(VisBaseEntity_cl *entity, const hkvVec3& spawnPos, const hkvMat3* rotationMatrix = NULL); 

  /// \brief
  ///   Returns the current position of the main camera.
  /// 
  /// \param position
  ///   vector in which to store the position in world space
  /// 
  /// \sa VisCamera_cl::GetCurrentCameraRotation
  /// \sa VisCamera_cl::GetCurrentCameraDirection
  inline void GetCurrentCameraPosition(hkvVec3& position) const;

  /// \brief
  ///   Returns the current position of the main camera.
  /// 
  /// \return
  ///   hkvVec3& position: camera position in world space
  /// 
  /// \sa VisCamera_cl::GetCurrentCameraRotation
  /// \sa VisCamera_cl::GetCurrentCameraDirection
  inline const hkvVec3& GetCurrentCameraPosition() const {return m_spMainCamera->GetPosition();}

  /// \brief
  ///   Returns the current rotation matrix of the main camera.
  /// 
  /// This method stores the current rotation matrix of the main camera in the specified matrix.
  /// 
  /// \param rotation
  ///   matrix object in which to store the current rotation
  /// 
  /// \sa VisCamera_cl::GetCurrentCameraPosition
  /// \sa VisCamera_cl::GetCurrentCameraDirection
  VISION_APIFUNC void GetCurrentCameraRotation(hkvMat3& rotation) const;

  /// \brief
  ///   Gets the current view direction vector of the main camera.
  /// 
  /// This method stores the current view direction of the camera.
  /// 
  /// The view direction is a normalized vector in world space.
  /// 
  /// \param direction
  ///   vector reference in which to store the direction
  /// 
  /// \sa VisCamera_cl::GetCurrentCameraPosition
  /// \sa VisCamera_cl::GetCurrentCameraRotation
  inline void GetCurrentCameraDirection(hkvVec3& direction) const;

  /// \brief
  ///   Returns the current view direction vector of the main camera.
  /// 
  /// The view direction is a normalized vector in world space.
  /// 
  /// \return
  ///   hkvVec3 direction: current camera direction vector
  /// 
  /// \sa VisCamera_cl::GetCurrentCameraPosition
  /// \sa VisCamera_cl::GetCurrentCameraRotation
  inline const hkvVec3 GetCurrentCameraDirection() const {return m_spMainCamera->GetDirection();}

  /// \brief
  ///   Moves the main camera and its owner entity by a vector.
  /// 
  /// If the flag INCPOSFLAG_PORTALCHECK is used, this method also handles passing through portals.
  /// 
  /// This function is very useful if it is required to move the camera multiple times in a single
  /// frame
  /// 
  /// This function also works if the camera isn't attached to an entity.
  /// 
  /// \param vPos
  ///   The movement delta. This delta will affect both the camera and the owner entity
  /// 
  /// \param iFlags
  ///   a valid combination of moving flags. Currently the following flag is supported:
  ///   INCPOSFLAG_PORTALCHECK : check for portals when moving the camera.
  VISION_APIFUNC void IncCameraPosition( const hkvVec3& vPos, int iFlags=INCPOSFLAG_NONE);

  /// \brief
  ///   Returns the visibility zone the main camera is currently in
  VISION_APIFUNC VisVisibilityZone_cl *GetVisibilityZone() const;

  /// \brief
  ///   Calls ReComputeVisibility on the main camera. See VisObject3D_cl::ReComputeVisibility for
  ///   details.
  VISION_APIFUNC void ReComputeVisibility();

  /// \brief
  ///   Updates the core engine camera matrix. Must be called by OnUpdateScene() once per frame
  VISION_APIFUNC void Update();

  /// \brief
  ///   Returns the main camera's current VisContextCamera_cl object.
  /// 
  /// \return
  ///   VisContextCamera_cl *: Current VisContextCamera_cl object of the main camera
  VISION_APIFUNC VisContextCamera_cl *GetMainCamera() const;

  ~VisCamera_cl() {  }

private:
  friend class Vision;
  
  void CreateMainCamera();

  VisContextCameraPtr m_spMainCamera;        ///< The "main camera" object (for the main render context)
};

#include <Vision/Runtime/Engine/SceneElements/VisApiCamera.inl>

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
