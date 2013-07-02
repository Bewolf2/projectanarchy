
#ifndef VLUA_APIDOC

%nodefaultctor VisContextCamera_cl;
%nodefaultdtor VisContextCamera_cl;

class VisContextCamera_cl : public VisObject3D_cl
{
public:

  void Set(const hkvMat3 &cameraMatrix, const hkvVec3& position);  

  void AttachToEntity(VisBaseEntity_cl *entity, const hkvVec3& spawnPos, const hkvMat3 *rotationMatrix);

  const hkvVec3 GetDirection() const;

  bool IsActive() const;
  
  %extend{
    hkvMat3 GetWorldRotationMatrix() const
    {
      hkvMat3 matrix;
      HKV_DISABLE_DEPRECATION
      self->GetWorldRotationMatrix(matrix);
      HKV_ENABLE_DEPRECATION
      return hkvMat3 (matrix);
    }

    hkvMat3 GetCameraToWorldRotation() const
    {
      hkvMat3 matrix = self->GetCameraToWorldRotation();
      return hkvMat3 (matrix);
    }
  
    hkvMat3 GetWorldToCameraRotation() const
    {
      hkvMat3 matrix = self->GetWorldToCameraRotation();
      return hkvMat3 (matrix);
    }

    void LookAt(hkvVec3* pVector)
    {
      if(pVector)
      {
        hkvMat3 matrix;
        hkvVec3 pos = self->GetPosition();
        matrix.setLookInDirectionMatrix (*pVector - pos);
        self->Set(matrix, pos);
      }
    }
    
    void ActivateInMainRenderContext()
    {
      VisRenderContext_cl::GetMainRenderContext()->SetCamera(self);
    }
    
    void AttachToEntity(VisBaseEntity_cl *pEntity)
    {
      hkvMat3 matrix;
      hkvVec3 pos = self->GetPosition();
      
      if(pEntity)
        matrix.setLookInDirectionMatrix (pEntity->GetPosition() - pos);
        
      self->AttachToEntity(pEntity, pos, matrix.getPointer ()); //NULL for the entity is allowed
    }
    
    void AttachToEntity(VisBaseEntity_cl *pEntity, const hkvVec3& spawnPos)
    {
      hkvMat3 matrix;
      
      if(pEntity)
        matrix.setLookInDirectionMatrix (pEntity->GetPosition() - spawnPos);
        
      self->AttachToEntity(pEntity, spawnPos, matrix.getPointer ());
    }
  }

};

VSWIG_CREATE_TOSTRING(VisContextCamera_cl, "VisContextCamera_cl: '%s' [%s : %1.2f,%1.2f,%1.2f]", self->GetObjectKey(), self->IsActive()?"active":"inactive", self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)

#else

/// \brief Context camera wrapper class.
/// \note If you would like to control the camera via VisObject3D_cl::IncOrientation / VisObject3D_cl::SetOrientation,
///   you have to enable Euler angles using \b VisObject3D_cl::SetUseEulerAngles(true) \b !
/// \see VisGame_cl::GetCamera
/// \see VisGame_cl::CreateCamera
/// \see VisObject3D_cl::SetUseEulerAngles
/// \see VisObject3D_cl::IncOrientation
/// \see VisObject3D_cl::SetOrientation
class VisContextCamera_cl : public VisObject3D_cl {
public:

  /// @name Common Context Camera Functions
  /// @{
  
  /// \brief Set the rotation matrix and position of the conext camera.
  /// \param rotationMatrix The new rotation matrix.
  /// \param position The new position.
  void Set(hkvMat3 rotationMatrix, hkvVec3 position);  

  /// \brief Attach the conext camera to an entity (Keep distance and orientation of the entity).
  /// \param entity The as parent intended entity.
  /// \param spawnPos (\b optional) The new position relative to the entity's origin (if not specified the current position will be used).
  /// \param rotationMatrix (\b optional) The new rotation matrix relative to the entity's orientation
  ///       (if not specified the camera will look from it's position to the target entity).
  void AttachToEntity(VisBaseEntity_cl entity, hkvVec3 spawnPos = nil, hkvMat3 rotationMatrix = nil);

  /// \brief Returns the current view direction vector of the camera.
  /// \details
  ///   This function stores the current view direction of the camera.
  ///   The view direction is a normalized vector in world space.
  /// \return Copy of the stored direction vector.
  hkvVec3 GetDirection();
  
  /// \brief Gets the inverse of the current 3x3 rotation matrix of the camera. Same as GetWorldToCameraRotation. DEPRECATED: Please do not use anymore.
  hkvMat3 GetWorldRotationMatrix();
  
  /// \brief Gets the current 3x3 rotation matrix of the camera.
  hkvMat3 GetCameraToWorldRotation() const;

  /// \brief Gets the inverse of the current 3x3 rotation matrix of the camera.
  hkvMat3 GetWorldToCameraRotation() const;
  
  /// \brief Let the camera look at a specified point.
  /// \param position The point to look at.
  void LookAt(hkvVec3 position);
      
  /// \brief Returns whether this context camera is the currently active camera.
  /// \return true if this context camera is the currently active camera, otherwise false.
  boolean IsActive();
  
  /// \brief Set this camera to be used for the main render context.
  void ActivateInMainRenderContext();
  
  /// @}
};

#endif
