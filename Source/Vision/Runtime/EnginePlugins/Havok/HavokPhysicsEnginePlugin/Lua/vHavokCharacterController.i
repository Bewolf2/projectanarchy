
#ifndef VLUA_APIDOC

%nodefaultctor vHavokCharacterController;
%nodefaultdtor vHavokCharacterController;

class vHavokCharacterController : public IVObjectComponent
{
public:

  %extend{
	  VSWIG_CONVERT_BOOL_SETTER(SetEnabled);
    VSWIG_CONVERT_BOOL_SETTER(SetDebugRendering);

    void SetCapsuleHeight(float fHeight)
    {
      self->Character_Top.z = self->Character_Bottom.z+fHeight;
    }

    float GetCapsuleHeight()
    {
      return self->Character_Top.z-self->Character_Bottom.z;
    }

    void SetCapsuleOffset(float fOffsetZ)
    {
      float fHeight = self->Character_Top.z-self->Character_Bottom.z;
      self->Character_Bottom.z = fOffsetZ;
      self->Character_Top.z = self->Character_Bottom.z+fHeight;
    }

    float GetCapsuleOffset()
    {
      return self->Character_Bottom.z;
    }

    void SetCapsuleTop(hkvVec3* pTop)
    {
      if(pTop) self->Character_Top = hkvVec3(pTop->x,pTop->y,pTop->z);
    }

    hkvVec3 GetCapsuleTop()
    {
      return hkvVec3(self->Character_Top.x, self->Character_Top.y, self->Character_Top.z);
    }

    void SetCapsuleBottom(hkvVec3* pBottom)
    {
      if(pBottom) self->Character_Bottom = hkvVec3(pBottom->x,pBottom->y,pBottom->z);
    }

    hkvVec3 GetCapsuleBottom()
    {
      return hkvVec3(self->Character_Bottom.x, self->Character_Bottom.y, self->Character_Bottom.z);
    }

    void SetCapsuleRadius(float fRadius)
    {
      self->Capsule_Radius = fRadius;
    }

    float GetCapsuleRadius()
    {
      return self->Capsule_Radius;
    }

    hkvVec3 GetCurrentLinearVelocity()
    {
      hkvVec3 velocity;
      self->GetCurrentLinearVelocity(velocity);
      return velocity;
    }

    hkReal PerformSweep(const hkvVec3& vDir, float fDistance)
    {
      vHavokSweepResult result;
      int iNumHits = ((vHavokPhysicsModule*)Vision::GetApplication()->GetPhysicsModule())->PerformSweep(&result, 1, self, vDir, fDistance);
      if(iNumHits > 0)
        return result.m_fDistance; // return distance of first hit 
      return hkReal(-1);
    }

    bool DropToFloor(float fDistance=10000.0f)
    {
      return ((vHavokPhysicsModule*)Vision::GetApplication()->GetPhysicsModule())->DropToFloor(self, fDistance);
    }

    VSWIG_CREATE_CAST(vHavokCharacterController)
  }
  
  bool IsEnabled() const;

  void SetDebugColor(VColorRef color);

  void GetCurrentLinearVelocity(hkvVec3& currentVelocity) const;

  void SetPosition(const hkvVec3& position);
  
  void SetFlyState(bool bIsFlying);
  void SetWantJump(bool wantJump);

  bool IsStanding() const;
  bool CheckSupport(const hkvVec3& vec) const;

  float GetGravityScaling() const;
  inline void SetGravityScaling(float fValue);
  
  void SetCollisionInfo(int iLayer, int iGroup, int iSubsystem, int iSubsystemDontCollideWith);

  float Static_Friction;                 ///< Default static friction for surfaces hit.
  float Dynamic_Friction;                ///< Default dynamic friction for surfaces hit.

  float Max_Slope;                       ///< Maximum slope that the character can walk up (In radians, Havok clamp: PI/2).
  float Character_Mass;                  ///< Mass of the character, standing on things.
  float Character_Strength;              ///< Maximum const force that the character controller can impart onto moving objects.

  float Gravity_Scale;                   ///< Scalar factor to scale the global gravity strength. Default is 1.0
};

#else

/// \brief Physics module class (Havok): Object component wrapper class for a character controller.
class vHavokCharacterController : public IVObjectComponent
{
public:

  /// @}
  /// @name Capsule Setup
  /// @{
  
  /// \brief Adjust the height of the capsule.
  /// \param height The new height (number of units point B's Z value will be higher than point A's)
  void SetCapsuleHeight(number height);
  
  /// \brief Calculate the height of the capsule.
  /// \return Returns the Z distance between point A and point B of the capsule.
  number GetCapsuleHeight();
  
  /// \brief Set the distance of the bottom point (point A) to the floor (keeping the distance to point B).
  /// \param offsetZ The offset to the floor.
  void SetCapsuleOffset(number offsetZ);

  /// \brief Get the curren offset to the floor (0 level).
  /// \return The offset measured from the bottom point (point A).
  number GetCapsuleOffset();

  /// \brief Set the top point of the capsule (point A)
  /// \param  top The new top point of the controller.
  void SetCapsuleTop(hkvVec3 top);

  /// \brief Get the top point of the capsule (point A)
  /// \return The top point of the controller.
  hkvVec3 GetCapsuleTop();
  
  /// \brief Set the bottom point of the capsule (point B)
  /// \param bottom The new bottom point of the controller.
  void SetCapsuleBottom(hkvVec3 bottom);

  /// \brief Get the bottom point of the capsule (point B)
  /// \return The bottom point of the controller.
  hkvVec3 GetCapsuleBottom();

  /// \brief Set the radius of the capsule controller
  /// \param radius The new radius to set.
  void SetCapsuleRadius(float radius);
 
  /// \brief calculate the hight of the capsule.
  /// \return The current height (distance on the Z axis bewtween point A and B)
  number GetCapsuleRadius();
 
  /// \brief Setup whether apply the Z value of the motion delta or not.
  /// \param use Set to true in order to apply the Z value of the motion delta.
  void SetUseMotionDeltaZ(boolean use);
  
  /// \brief Check if the Z value of the motion delta is currently applied.
  /// \return true if the Z delta is getting applied, false otherwise.
  boolean GetUseMotionDeltaZ();
  
  /// @}
  /// @name Velocity & Property Wrapper Functions
  /// @{

  /// \brief
  ///   Enables / Disables simulation.
  /// \param enable Enabled state to set.
  void SetEnabled(boolean enable);

  ///
  /// \brief
  ///   Returns whether simulation is enabled;
  boolean IsEnabled();

  /// \brief Directly sets a position (use with caution)
  /// \param position The new position.
  void SetPosition(hkvVec3 position);
  
  /// \brief Checks if the character is standing on the ground.
  /// \return true if standing on the ground, false otherwise.
  boolean IsStanding();
  
  /// \brief After step, get the actual linear velocity achieved.
  /// This includes effect of gravity if unsupported.
  /// \return currentVelocity The current velocity vector.
  hkvVec3 GetCurrentLinearVelocity();
  
  /// \brief Checks if the character is supported by world geometry in a given direction.
  /// \param location The location to check.
  boolean CheckSupport(hkvVec3 location);

  /// \brief Returns the current gravity scaling factor. 
  /// \return The current gravity scaling. The default value is 1.0f (normal gravity of the world).
  number GetGravityScaling();
  
  /// \brief Sets the scaling of the gravity relatively to the world's default gravity.
  /// \param gravityScaling The default value is 1.0f (normal gravity of the world). Passing 0.0f disables the gravity.
  void SetGravityScaling(number gravityScaling);

  /// \brief Sets the collision parameters of this rigid body.
  /// \param layer The collision layer as integer number.
  /// \param group The collision group.
  /// \param subsystem The collision subsystem.
  /// \param subsystemDontCollideWith The collision subsystem this body shouldn't collide with.
  /// \note
  ///   See the Havok documentation on rigid body collisions for more 
  ///   information about what values to specify for these parameters.
  void SetCollisionInfo(number layer, number group, number subsystem, number subsystemDontCollideWith);
  
  ///
  /// \brief
  ///   Set if the character should jump or not
  ///
  void SetWantJump(bool wantJump);

  ///
  /// \brief
  ///   Set to true if the character should start flying. Set to false if the character should stop flying
  ///
  void SetFlyState(bool bIsFlying);
  
  /// @}
  /// @name Debug Rendering
  /// @{
  
  /// \brief Enable / Disable debug rendering for this rigid body's shape.
  /// \param enable If true, the shape of this rigid body will be displayed in Vision.
  void SetDebugRendering(boolean enable);
  
  /// \brief Set the color of the debug rendering representation of this rigid body's shape.
  /// \param color New Color of the debug rendering representation.
  void SetDebugColor(VColorRef color);

  /// @}
  /// @name Sweep Testing
  /// @{

  /// \brief Performs a linear sweep through space with the character controller. 
  /// \param dir Normalized motion vector for the sweep.
  /// \param distance  Distance along the dir vector.
  /// \return A positive number (= distance to collider), if the object would collide with another object within the specified distance, 
  ///         or a negative number, if no collision would occur.
  number PerformSweep(hkvVec3 dir, number distance);

  /// \brief Drops the specified character controller to the floor by performing a linear sweep.
  /// \param distance Test for a floor in distance units below the character controller.
  /// \return Returns true on success, otherwise false.
  boolean DropToFloor(number distance=10000);
      
  /// @}
  /// @name Public Members
  /// @{
  
  /// \brief Default static friction for surfaces hit.
  number Static_Friction;
  
  /// \brief Default dynamic friction for surfaces hit.
  number Dynamic_Friction;

  /// \brief Maximum slope that the character can walk up (In radians, Havok clamp: PI/2).
  number Max_Slope;
  
  /// \brief Mass of the character, standing on things.
  number Character_Mass;
  
  /// \brief Maximum const force that the character controller can impart onto moving objects.
  number Character_Strength;

  /// \brief Scalar factor to scale the global gravity strength. Default is 1.0.
  number Gravity_Scale;
  
  /// @}
};

#endif

