
#ifndef VLUA_APIDOC

%nodefaultctor vHavokRagdoll;
%nodefaultdtor vHavokRagdoll;

class vHavokRagdoll : public IVObjectComponent
{
public:

  %rename(Remove) DisposeObject();
  void DisposeObject();

  %extend {
	VSWIG_CONVERT_BOOL_SETTER(SetEnabled);
    VSWIG_CONVERT_BOOL_SETTER(SetDebugRendering);

    VSWIG_CREATE_CAST(vHavokRagdoll)
  }

  bool IsEnabled() const;
  
  void SetDebugColor(VColorRef color);

  const char* GetRagdollCollisionFile();
  void SetRagdollCollisionFile(const char* val);
  int GetRigidBodyIndex(const char* val) const;
  void ApplyForceToRigidBody(int boneIndex, hkvVec3& value, float deltaT);
  void ApplyLinearImpulseToRigidBody(int boneIndex, hkvVec3& value);
  hkvVec3 GetPositionOfRigidBody(int boneIndex) const;
};

#else

/// \brief Physics module class (Havok): Object component wrapper class for a rag doll component.
class vHavokRagdoll : public IVObjectComponent
{
public:
  
  /// @name Property Wrapper Functions
  /// @{

  /// \brief
  ///   Enables / Disables the rag doll component.
  void SetEnabled(boolean bStatus);

  /// \brief
  ///   Returns whether the rag doll component is enabled.
  boolean IsEnabled();

  /// \brief
  ///   Returns the path to the Havok collision file containing the
  ///   rag doll's rigid body hierarchy (.hkt file).
  const char* GetRagdollCollisionFile();

  /// \brief
  ///   Sets the path to the Havok collision file containing the
  ///   rag doll's rigid body hierarchy (.hkt file).
  void SetRagdollCollisionFile(const char* val);
  
  /// @}
  /// @name Debug Rendering
  /// @{
  
  /// \brief
  ///   Enable / Disable debug rendering for this rag doll's shapes.
  ///
  /// \param enable
  ///   If true, the shapes of this rag doll will be displayed in Vision.
  void SetDebugRendering(boolean enable);
  
  /// \brief
  ///   Set the color of the debug rendering representation of this rag doll's shapes.
  ///
  /// \param color
  ///   New Color of the debug rendering representation.
  void SetDebugColor(VColorRef color);

  /// \brief
  ///   Returns the index of this bone in the ragdoll or returns -1 if not found
  ///
  /// \param val
  ///   RigidBody bone name as set in the export pipeline
  int GetRigidBodyIndex(const char* val) const;

  /// \brief
  ///   Applies a force to the bone at index i. Use GetRigidBodyIndex to retrieve the rigid body index by name.
  ///
  /// \param iBoneIndex
  ///   Rigid body index to apply force to.
  ///
  /// \param force
  ///   Force vector to apply.
  ///
  /// \param deltaT
  ///   The time interval over which the force is applied.
  ///
  /// See the documentation for vHavokRigidBody::ApplyForce or vHavokRigidBody::ApplyLinearImpulse for more information
  /// on the distinction between applying forces or impulses.
  void ApplyForceToRigidBody(int iBoneIndex, const hkvVec3& force, float deltaT);

  /// \brief
  ///   Applies a linear impulse to the bone at index i. Use GetRigidBodyIndex to retrieve the rigid body index by name.
  ///
  /// \param iBoneIndex
  ///   Rigid body index to apply force to.
  ///
  /// \param impulse
  ///   Impulse vector to apply.
  ///
  ///
  /// See the documentation for vHavokRigidBody::ApplyForce or vHavokRigidBody::ApplyLinearImpulse for more information
  /// on the distinction between applying forces or impulses.
  void ApplyLinearImpulseToRigidBody(int iBoneIndex, const hkvVec3& impulse);

  /// \brief
  ///   Returns the position of a given rigidbody in the ragdoll
  ///
  /// \param boneIndex
  ///   Index of the rigidbody 
  hkvVec3 GetPositionOfRigidBody(int boneIndex) const;

  /// @}
};

#endif
