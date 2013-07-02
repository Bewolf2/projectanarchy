/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VHAVOKRAGDOLL_HPP_INCLUDED
#define VHAVOKRAGDOLL_HPP_INCLUDED

//-----------------------------------------------------------------------------------

class vHavokCharacterController;

//-----------------------------------------------------------------------------------
// Serialization versions

#define VHAVOKRAGDOLL_VERSION_0         0 // Initial version
#define VHAVOKRAGDOLL_VERSION_CURRENT   0 // Current version

//-----------------------------------------------------------------------------------

///
/// \brief
///   Havok Ragdoll
///
class vHavokRagdoll : public IVObjectComponent
{
public:
  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  ///
  /// \brief
  ///   Default constructor.
  ///
  /// The actual initialization of the component happens in the ::SetOwner function
  VHAVOK_IMPEXP vHavokRagdoll();

  ///
  /// \brief
  ///   Destructor
  VHAVOK_IMPEXP ~vHavokRagdoll();

  ///
  /// @}
  ///

  ///
  /// \brief
  ///   Overridden function to respond to owner changes.
  ///
  /// By setting the owner of this component the HavokRagdoll object will be 
  /// added to the Havok World and gets registered in the Havok module. 
  ///
  /// \param pOwner
  ///   The owner of this component.
  /// 
  /// \remarks
  ///   SetOwner(NULL) removes the HavokRagdoll object from the Havok World and
  ///   gets unregistered in the Havok module.
  VHAVOK_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl* pOwner) HKV_OVERRIDE;

  ///
  /// \brief
  ///   Update the owner's skeleton based on the ragdoll's rigid bodies
  VHAVOK_IMPEXP void UpdateOwner();

  ///
  /// \brief
  ///   Overridden function to determine if this component can be attached to a 
  ///   given object.
  ///
  /// The HavokRagdoll component can be attached to VisObject3D_cl instances 
  /// with a skeleton.
  /// 
  /// \param pObject
  ///   Possible owner candidate.
  /// 
  /// \param sErrorMsgOut
  ///   Reference to error message string.
  /// 
  /// \returns
  ///   TRUE if this component can be attached to the given object, 
  ///   FALSE otherwise.
  VHAVOK_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl* pObject, 
    VString &sErrorMsgOut) HKV_OVERRIDE;

  ///
  /// \brief
  ///   Overridden function to respond to variable changes.
  /// 
  /// \param pVar
  ///   Pointer to the variable object to identify the variable.
  /// 
  /// \param value
  ///   New value of the variable
  /// 
  VHAVOK_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl* pVar,
    const char* value) HKV_OVERRIDE;

  ///
  /// \brief
  ///   The message function has been overridden to forward collision events to a script 
  VHAVOK_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(vHavokRagdoll, VHAVOK_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(vHavokRagdoll, VHAVOK_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   Binary archive
  VHAVOK_IMPEXP virtual void Serialize(VArchive& ar) HKV_OVERRIDE;

  /// \brief
  ///   Virtual overridable that gets called when a loading archive closes.
  VHAVOK_IMPEXP virtual void OnDeserializationCallback(
    const VSerializationContext& context) HKV_OVERRIDE;

  /// \brief
  ///   Virtual overridable that gets called to determine if the object needs the 
  ///   OnDeserializationCallback.
  VHAVOK_IMPEXP virtual VBool WantsDeserializationCallback(
    const VSerializationContext& context) HKV_OVERRIDE
  {
    return TRUE;
  }

  ///
  /// @}
  ///

  ///
  /// @name Parameters
  /// @{
  ///

  /// \brief
  ///   Returns if the rag doll component is enabled.
  inline BOOL IsEnabled() const 
  { 
    return m_bEnabled; 
  }

  /// \brief
  ///   Enables / Disables the rag doll component.
  VHAVOK_IMPEXP void SetEnabled(BOOL bStatus);

  /// \brief
  ///   Returns the path to the Havok collision file containing the
  ///   rag doll's rigid body hierarchy (.hkt file).
  inline const char* GetRagdollCollisionFile() const
  { 
    return m_sFileResourceName.AsChar(); 
  }

  /// \brief
  ///   Sets the path to the Havok collision file containing the
  ///   rag doll's rigid body hierarchy (.hkt file).
  inline void SetRagdollCollisionFile(const char* szFileName)
  { 
    m_sFileResourceName = szFileName; 
    RecreateRagdoll();
  }

  /// \brief
  ///   Helper function for directly retrieving the owner entity.
  inline VisBaseEntity_cl* GetOwnerEntity()
  {
    return static_cast<VisBaseEntity_cl*>(GetOwner());
  }

  ///
  /// @}
  ///

  ///
  /// @name Debug Rendering
  /// @{
  ///

  /// \brief
  ///   Enable / Disable debug rendering for this rag doll's shapes.
  ///
  /// \param bEnable
  ///   If TRUE, the shapes of this rag doll will be displayed in Vision.
  VHAVOK_IMPEXP void SetDebugRendering(BOOL bEnable);

  /// \brief 
  ///   Returns whether debug rendering is enabled.
  inline bool GetDebugRenderEnabled () const 
  { 
    return m_bDebugRenderingEnabled != FALSE; 
  }

  /// \brief
  ///   Set the color of the debug rendering representation of this rag doll's shapes.
  ///
  /// \param color
  ///   New Color of the debug rendering representation.
  VHAVOK_IMPEXP void SetDebugColor(VColorRef color);

  /// \brief
  ///   Returns whether the rag doll component has been successfully initialized.
  inline bool IsInitialized() const
  { 
    return m_bInitialized; 
  }

  /// \brief
  ///   Returns the index of this bone in the ragdoll or returns -1 if not found
  ///
  /// \param szBodyName
  ///   RigidBody bone name as set in the export pipeline
  int GetRigidBodyIndex(const char* szBodyName) const;

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
  /// \param iBoneIndex
  ///   Index of the rigidbody 
  hkvVec3 GetPositionOfRigidBody(int iBoneIndex) const;

  ///
  /// @}
  ///

private:
  // Private functions
  VHAVOK_IMPEXP void CreateRagdoll();
  VHAVOK_IMPEXP void DeleteRagdoll();

  inline void RecreateRagdoll()
  {
    DeleteRagdoll();
    CreateRagdoll();
  }

  // Returns true if loading was successful.
  bool LoadHktFile();

  // Map the owner's bones to rigid bodies.
  //  Returns true if loading was successful.
  bool MapBonesToRigidBodies();

  // Bakes the scaling used by the owner entity
  //  Returns true if loading was successful.
  bool BakeScalingIntoRigidBodies(); 

  // Computes transformations connecting the bones to the rigid bodies.
  //  Returns true if loading was successful.
  bool ComputeTransformMapping();

  // Copies the bones' position and orientation over to the rigid bodies.
  void CopyBoneTransformationToRigidBodies(VisAnimConfig_cl* pAnimConfig);

  // Copies the rigid bodies' position and orientation over to the bones.
  void CopyRigidBodyTransformationToBones();

  // Helpers
  int GetRigidBodyIndexForBone(int iBoneIdx) const; // linear search

  // If the entity is scaled, this is already baked into the object space
  void GetObjectSpaceBone(int iBoneIdx, hkvMat4& boneObjectMatrix, 
    const VisSkeleton_cl* pSkeleton, VisSkeletalAnimResult_cl* pPose);

  // Explicit debug geometry transform update.
  void UpdateDebugGeometry() const;

  void AddToPhysicsWorld();
  void RemoveFromPhysicsWorld();

  // Member variables
  struct RigidBodyInfo
  {
    hkpRigidBody*   pRigidBody;
    // to account for any discrepancy in say Maya or on export for 
    // what is 'local space' (relative to bone)
    hkTransform     relTransform;
    int             iBoneIdx; // of the corresponding bone
  };

  // Structure used for storing relative transformation info
  // for unmapped bones (do not have a corresponding rigid body)
  struct UnmappedBoneInfo
  {
    int     iBoneIdx;
    hkvVec3 localTranslation;
    hkvQuat localRotation;
  };

  hkpWorld* m_pPhysicsWorld;

  hkArray<hkpPhysicsSystem*>  m_physicsSystems;
  hkArray<RigidBodyInfo>      m_rigidBodies;
  hkArray<UnmappedBoneInfo>   m_unmappedBones;
  int                         m_iMappedRootBoneIdx; ///< This bone corresponds to the root rigid body of the rag doll
  float                       m_fScaling;
  bool                        m_bInitialized;
  bool                        m_bAddedToPhysicsWorld;

  // rag doll animation config
  VSmartPtr<VisAnimFinalSkeletalResult_cl> m_spFinalSkeletalResultRagdoll;

  // Smart pointer to increase the reference count of the anim config.
  VSmartPtr<VisAnimConfig_cl> m_spAnimConfig;

  // Var table members
  BOOL        m_bEnabled;
  VString     m_sFileResourceName;
  BOOL        m_bDebugRenderingEnabled;
  VColorRef   m_debugColor;
};

//-----------------------------------------------------------------------------------

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
