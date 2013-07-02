/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_CONSTRAINT_CHAIN_HPP_INCLUDED
#define V_HAVOK_CONSTRAINT_CHAIN_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokElementManager.hpp>

#pragma managed(push, off)
#include <Common/Base/hkBase.h>
#include <Physics2012/Dynamics/Constraint/hkpConstraintListener.h>
#pragma managed(pop)


class hkpConstraintChainData;
class hkpConstraintChainInstance;
class hkpRigidBody;

class vHavokPhysicsModule;
class vHavokRigidBody;
class vHavokConstraintChain;

HAVOK_ELEMENTMANAGER_TEMPLATE_DECL(vHavokConstraintChain);

// ----------------------------------------------------------------------------
// vHavokConstraintChainDesc
// ----------------------------------------------------------------------------

/// \brief
///   Base class for descriptors of Havok constraint chains
///
/// This base class holds all the properties that are common to all 
/// constraint chain types in vHavok. A subclass exists for each concrete
/// Havok constraint chain type, which may contain additional properties.
class vHavokConstraintChainDesc : public VTypedObject
{
public:
  /// \brief
  ///   Default constructor; initializes all fields to default settings.
  VHAVOK_IMPEXP vHavokConstraintChainDesc();
protected:
  /// \brief
  ///   No-init constructor; leaves the fields uninitialized.
  /// \param bDummyNoInit
  ///   value is ignored
  vHavokConstraintChainDesc(bool bDummyNoInit);
public:
  /// \brief
  ///   Destructor.
  VHAVOK_IMPEXP virtual ~vHavokConstraintChainDesc();

public:
  /// \brief
  ///   Resets all fields to their default values.
  VHAVOK_IMPEXP virtual void Reset();
  
  /// \brief
  ///   Gets the key of the path to fit the constraint chain to.
  /// \return
  ///   the path key, or \c null, if none is set
  VHAVOK_IMPEXP const char *GetPathKey() const;

  /// \brief
  ///   Sets the key of the path to fit the constraint chain to.
  ///
  /// If no path is set, the constraint chain will be fitted to the line
  /// between the pivot points of the two anchor bodies. 
  ///
  /// \param pszPathKey
  ///   the path key, or \c null to set an empty key
  VHAVOK_IMPEXP void SetPathKey(const char* pszPathKey);

  V_DECLARE_SERIAL_DLLEXP( vHavokConstraintChainDesc, VHAVOK_IMPEXP )
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

public:
  VSmartPtr<vHavokRigidBody> m_spAnchorBodies[2]; ///< The vHavok rigid bodies to anchor the ends of the constraint chain to
  hkvVec3 m_vAnchorPivots[2];                ///< The pivot points of the anchor bodies, in body space

  float m_fLinkLength;  ///< The length of each chain link
  float m_fLinkGap;     ///< The gap between two chain links
  float m_fDiameter;    ///< The diameter of the chain
  float m_fMaterialDensity; ///< The density of the link material (in kg/m^3)
  float m_fMaterialStretchingConstant; ///< The stretching constant (in N)

  float m_fChainDamping;       ///< The damping the solver should apply when solving the constraint chain. Ranges from 0.0 (no damping) to 1.0 (strong damping)
  float m_fLinkLinearDamping;  ///< The linear damping for each chain link
  float m_fLinkAngularDamping; ///< The angular damping for each chain link

  bool m_bCalculateLinkInertia; ///< Whether to let Havok calculate an inertia tensor for the chain links
  hkvVec3 m_vInertiaTensorAxisScaling; ///< Scaling of the chain link inertia tensor axes
  float m_fInertiaTensorUniformScaling; ///< Uniform scaling of the chain link inertia tensor

  int m_iLinkCollisionLayer;
  int m_iLinkCollisionGroup;
  int m_iLinkSubSystemId;
  int m_iLinkSubSystemDontCollideWith;

  bool m_bComputeCfm; ///< Whether to compute the CFM (or to use a fixed CFM otherwise)
  float m_fCfmFactor; ///< Factor to scale computed CFM (constraint force mixing) with. 0.f: don't compute CFM and use Havok default.
  float m_fFixedCfm; ///< The fixed CFM to use when the CFM is not computed

  bool m_bUseAngularFriction; ///< Whether to use angular friction
  float m_fFrictionFactorMin; ///< The minimum internal friction factor (in the middle of the chain)
  float m_fFrictionFactorMax; ///< The maximum internal friction factor (at the ends of the chain)
  float m_fBaseTension; ///< The minimum tension the rope is assumed to always have (used for internal friction calculations)

private:
  static const unsigned int s_iSerialVersion; ///< The current serialization version for objects of this class.
  char *m_pszPathKey;   ///< The path key
};


/// \brief
///   Helper class for vHavok event handling - internal use only!
class vHavokConstraintChainListenerProxy : public hkpConstraintListener
{
public:
	vHavokConstraintChainListenerProxy() { }
	void setOwner(vHavokConstraintChain* o ) { m_owner = o; }
	virtual void constraintDeletedCallback(hkpConstraintInstance* constraint);
	vHavokConstraintChain* m_owner;
};


// ----------------------------------------------------------------------------
// vHavokRagdollConstraintInfo
// ----------------------------------------------------------------------------
/// \brief
///   Helper struct for keeping track of the ragdoll extension for 
///   constraint chains.
struct vHavokRagdollConstraintInfo
{
  HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO, vHavokRagdollConstraintInfo);
  hkpConstraintInstance* m_instance;

  // Source constraint. Used to compute max friction
  hkpConstraintChainInstance* m_sourceChain;
  int m_sourceInfo;
};


// ----------------------------------------------------------------------------
// vHavokConstraintChain
// ----------------------------------------------------------------------------

/// \brief
///   Base class for all constraint chain implementations
class vHavokConstraintChain 
: public VRefCounter, 
  public VisTypedEngineObject_cl, 
  public VisObjectKey_cl,
  public vHavokElementManager<class vHavokConstraintChain*>,
  public IHavokStepper
{

  ///@name Inner Types
  //@{
private:
  class TempRemoveFromWorld;
  //@}

  /// @name Creation/Initialization/Destruction
  //@{
public:

  /// \brief
  ///   Default constructor.
  VHAVOK_IMPEXP vHavokConstraintChain();

  /// \brief
  ///   Destructor.
  VHAVOK_IMPEXP VOVERRIDE ~vHavokConstraintChain();

  /// \brief
  ///   Initializes this constraint chain from the provided descriptor.
  ///
  /// Although this is a non-virtual method taking a base class reference for
  /// the constraint descriptor as parameter, this should only be called on
  /// subclasses of this class, and a reference to a constraint chain 
  /// descriptor of the correct type should be passed.
  ///
  /// \param desc
  ///   the vHavok constraint chain descriptor
  VHAVOK_IMPEXP bool Init(vHavokConstraintChainDesc &desc);

protected:
  /// \brief
  ///   Resets this instance to an uninitialized state. If any Havok objects have
  ///   been created, they are removed from the world and released.
  void Reset();

  //@}

  /// @name Base Class Overrides
  //@{
public:
  VHAVOK_IMPEXP VOVERRIDE void DisposeObject();
  VHAVOK_IMPEXP VOVERRIDE void Step(float dt);
  //@}

  /// @name Callbacks
  //@{
  VHAVOK_IMPEXP void OnEstablish();
  //@}

  /// \brief
  ///   Indicates whether the constraint chain was successfully created.
  /// \return
  ///   \c false if the creation of the Havok constraint chain failed.
  VHAVOK_IMPEXP bool IsValid() const { return m_pConstraintChain != NULL; }

  /// \brief
  ///   Cleanup function for broken constraint chains.
  static void RemoveBrokenConstraintChains();


  /// @name Serialization and Type Management
  //@{
  V_DECLARE_SERIAL_DLLEXP( vHavokConstraintChain, VHAVOK_IMPEXP )
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
  VHAVOK_IMPEXP VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);
  VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context) { return context.m_eType != VSerializationContext::VSERIALIZATION_EDITOR; }
  //@}
  

  /// @name Internals
  //@{
  
  /// \brief
  ///   Returns the diameter of the chain
  VHAVOK_IMPEXP float GetDiameter() const { return m_fDiameter; }

  /// \brief
  ///   Returns the object used internally by Havok to manage the constraint chain.
  VHAVOK_IMPEXP hkpConstraintChainInstance *GetHkConstraintChainInstance() const { return m_pConstraintChain; }
  
  /// \brief
  ///   Returns the length of each link of the constraint chain
  VHAVOK_IMPEXP float GetLinkLength() const { return m_fLinkLength; }

  /// \brief
  ///   Returns the gap between the links of the constraint chain
  VHAVOK_IMPEXP float GetLinkGap() const { return m_fLinkGap; }

  /// \brief
  ///   Returns the number of links of this constraint chain.
  VHAVOK_IMPEXP unsigned int GetNumLinks() { return m_iNumLinks; }

  /// \brief
  ///   Returns the amount (in world units) by which the last chain link is 
  ///   shorter or longer than required. The value is negative if the link
  ///   is longer, and positive if the link is shorter.
  VHAVOK_IMPEXP float GetLastLinkExtra() const { return m_fLastLinkExtra; }

  /// \brief
  ///   Inquires the transformation of the specified link in world space.
  /// \param iIndex
  ///   index of the link whose transformation to get
  /// \param mRotation
  ///   output parameter; receives the rotation of the link
  /// \param vTranslation
  ///   output parameter; receives the translation of the link
  /// \return
  ///   \c true if a valid transformation could be determined, \c false 
  ///   if not. In the latter case, the contents of mRotation and vTranslation
  ///   are undefined.
  VHAVOK_IMPEXP bool GetLinkTransform(unsigned int iIndex, 
    hkvMat3 &mRotation, hkvVec3& vTranslation) const;

  /// \brief
  ///   Inquires the transformation of all links in world space, optionally
  ///   removing the roll values or reversing the link order.
  /// \param pRotations
  ///   pointer to an array of rotation matrices. This array must be allocated
  ///   by the caller and be at least of the size returned by GetNumLinks().
  /// \param pTranslations
  ///   pointer to an array of translation vectors. This array must be allocated
  ///   by the caller and be at least of the size returned by GetNumLinks().
  /// \param bRemoveRoll
  ///   whether to remove the roll component in the output transforms
  /// \param bReverseOrder
  ///   whether to output the link transforms in reverse order, and with a yaw
  ///   of 180 degrees applied
  VHAVOK_IMPEXP void GetLinkTransforms(hkvMat3 *pRotations, 
    hkvVec3* pTranslations, bool bRemoveRoll, bool bReverseOrder) const;


  /// \brief
  ///   Sets the transform of the specified link in world space.
  /// \param iIndex
  ///   index of the link whose transformation to set
  /// \param mRotation
  ///   new rotation of the link
  /// \param vTranslation
  ///   new translation of the link
  /// \return
  ///   \c true if the transform could be set; \c false if not. The latter can
  ///   happen if the link index is invalid.
  VHAVOK_IMPEXP bool SetLinkTransform(unsigned int iIndex, 
    hkvMat3 &mRotation, hkvVec3& vTranslation);

  /// \brief
  ///   Returns one of the anchor pivots (in local space)
  /// \param iIndex
  ///   the index of the anchor pivot to return
  /// \return
  ///   the requested anchor pivot
  VHAVOK_IMPEXP hkvVec3 GetAnchorPivot(unsigned int iIndex) const;

  /// \brief
  ///   Returns pointer to one of the chains anchor RigidBodys
  /// \param iIndex
  ///   the index of the RigidBody to return
  /// \return
  ///   the requested RigidBody
  VHAVOK_IMPEXP vHavokRigidBody* GetAnchorBody(unsigned int iIndex) const;

  /// \brief
  ///   Returns the rigid body of a chain link
  /// \param iIndex
  ///   the index of the chain link whose body to return
  /// \return
  ///   the rigid body of the chain link, or \c null, if the link index is
  ///   invalid for the current chain
  VHAVOK_IMPEXP hkpRigidBody* GetLinkBody(unsigned int iIndex) const;

  /// \brief
  ///   Returns the average error of approximating the constraint chain path with the
  ///   links of the constraint chain.
  /// \return
  ///   the error value, expressed in average length difference between the links and
  ///   the path segments they approximate. Scaled so that 1 = link length.
  VHAVOK_IMPEXP float GetApproximationError() { return m_fApproximationError; }

  /// \brief
  ///   Returns the force currently in effect on this constraint chain.
  /// \return
  ///   The force on the constraint chain, or \c (0,0,0,0), if the constraint chain
  ///   is not initialized
  VHAVOK_IMPEXP hkvVec3 GetChainForceVec() { return m_vChainForce; }

  /// \brief
  ///   Returns the norm of the force currently in effect on this constraint chain.
  /// \return
  ///   The norm of the force on the constraint chain, or \c 0, if the constraint chain
  ///   is not initialized
  VHAVOK_IMPEXP float GetChainForce() { return m_vChainForce.getLength(); }

protected:
  /// \brief
  ///   Returns the impulse currently applied on the constraint chain.
  /// \return
  ///   The impulse for each direction in space
  /// \note
  ///   Must be overwritten in derived classes!
  VHAVOK_IMPEXP virtual hkVector4 GetImpulseApplied() const;

  /// \brief
  ///   Returns the maximum number of links that this constraint chain may
  ///   have.
  VHAVOK_IMPEXP virtual unsigned int GetMaxNumLinks() const { return 0; }

  /// \brief
  ///   Removes the roll (relative to the transformation of the previous chain link)
  ///   from the transformation of a chain link.
  ///
  /// This is used to avoid chain links being rotated against each other, which would
  /// give unwanted skinning or animation results.
  ///
  /// \param mPrevRot
  ///   rotation matrix of the previous chain link (will not be modified)
  /// \param mRot
  ///   rotation of the current chain link (will be modified)
  /// \param bDirection
  ///   state variable denoting odd or even number of direction changes
  ///   of the constraint chain (will be modified; always pass in the result
  ///   of the previous call when going along the chain)
  void RemoveLinkRoll(hkvMat3 const& mPrevRot, hkvMat3& mRot,
    bool &bDirection) const;
  //@}


  ///@name Chain Length
  //@{
public:

  /// \brief
  ///   Returns the current length of the (unstretched) chain.
  /// \return
  ///   the current length of the chain
  VHAVOK_IMPEXP float GetChainLength() const { return m_fUnstretchedLength; }
  
  /// \brief
  ///   Changes the (unstretched) length of the constraint chain.
  /// \param fLength
  ///   the new length
  /// \return
  ///   \c true if successful; \c false if not
  VHAVOK_IMPEXP bool SetChainLength(float fLength);

  //@}


  /// @name Havok callbacks
  //@{
public:
  VHAVOK_IMPEXP VOVERRIDE void constraintDeletedCallback(hkpConstraintInstance* constraint);
  //@}

protected:
  ///@name Pivot Calculations
  //@{

  /// \brief
  ///   Calculates the error between the end of the last link and the
  ///   end anchor.
  /// \param vEndAnchor
  ///   the location of the end anchor
  /// \param pivots
  ///   the list of pivot points calculated for the chain link end points
  /// \param iNumPivots
  ///   the number of pivot points
  void CalcLinkError(hkvVec3 const& vEndAnchor,
    DynObjArray_cl<hkvVec3> const& pivots, unsigned int iNumPivots);

  /// \brief
  ///   Calculates the pivot points for this constraint chain.
  /// \param pivots
  ///   a dynamic array to receive the calculated pivot points
  /// \return  
  ///   the number of pivot points calculated
  unsigned int CalcPivotPoints(DynObjArray_cl<hkvVec3>& pivots);

  /// \brief
  ///   Calculates the pivot points for this constraint chain based on
  ///   the line between start and end anchor.
  /// \param pivots
  ///   a dynamic array to receive the calculated pivot points
  /// \return  
  ///   the number of pivot points calculated
  unsigned int CalcPivotPointsFromAnchors(DynObjArray_cl<hkvVec3>& pivots);

  /// \brief
  ///   Calculates the pivot points for this constraint chain, based on a path.
  ///
  /// \param pivots
  ///   a dynamic array to receive the calculated pivot points
  ///
  /// \param pPath
  ///   the path to calculate the pivot points from
  ///
  /// \return  
  ///   the number of pivot points calculated
  unsigned int CalcPivotPointsFromPath(DynObjArray_cl<hkvVec3>& pivots, VisPath_cl* pPath);

  /// \brief
  ///   Calculates a pivot point for chain links (in link space)
  /// \param iIndex
  ///   which pivot point to calculate; only 0 and 1 are allowed
  /// \return
  ///   the calculated pivot point
  virtual hkvVec3 CalcLinkPivot(unsigned int iIndex);

  /// \brief
  ///   Adjusts the pivot coordinates of the last chain link to make the
  ///   chain exactly fit the required length.
  void AdjustLastChainLinkPivot();

  //@}

  ///@name ConstraintInfo Handling
  //@{
protected:
  /// \brief
  ///   Appends a constraint info with the specified pivot points.
  /// \param vPivotA
  ///   the first pivot point
  /// \param vPivotB
  ///   the second pivot point
  /// \param bIsAnchor
  ///   whether the added constraint info connect to an anchor \c (true), or to
  ///   another chain link \c (false), default
  /// \note
  ///   Must be overwritten in derived classes!
  virtual void AppendConstraintInfo( const hkvVec3& vPivotA, const hkvVec3& vPivotB, 
    bool bIsAnchor = false);

  /// \brief
  ///   Updates a constraint info, setting the specified pivot points.
  /// \param iIndex
  ///   index of the constraint info to update
  /// \param vPivotA
  ///   the first pivot point
  /// \param vPivotB
  ///   the second pivot point
  /// \note
  ///   Must be overwritten in derived classes!
  virtual void UpdateConstraintInfo(unsigned int iIndex, 
    const hkvVec3& vPivotA, const hkvVec3& vPivotB);

  /// \brief
  ///   Removes a constraint info.
  /// \param iIndex
  ///   index of the constraint info to remove
  /// \note
  ///   Must be overwritten in derived classes!
  virtual void RemoveConstraintInfo(unsigned int iIndex);

  /// \brief
  ///   Gets the number of constraint infos in this constraint chain.
  /// \return
  ///   the number of constraint infos
  /// \note
  ///   Must be overwritten in derived classes!
  virtual unsigned int GetConstraintInfoCount();
  
  //@}


  ///@name Ragdoll constraint handling
  //@{
protected:
  /// \brief
  ///   Fills the ragdoll constraint info with the parameters of the
  ///   specified constraint info.
  /// \param ragdollInfo
  ///   the ragdoll constraint info struct to fill
  /// \param iConstraintIndex
  ///   the index of the constraint
  VHAVOK_IMPEXP virtual void FillRagdollConstraintInfo(
    vHavokRagdollConstraintInfo &ragdollInfo, 
    unsigned int iConstraintIndex);

  /// \brief
  ///   Returns whether this constraint chain implementation support the
  ///   use of ragdoll constraints for constraint chains.
  /// \return
  ///   whether ragdoll constraints are supported
  VHAVOK_IMPEXP virtual bool SupportsRagdollConstraints() const;

  /// \brief
  ///   Updates the ragdoll constraint states
  VHAVOK_IMPEXP virtual void UpdateRagdollConstraintState();

private:
  /// \brief
  ///   Appends a ragdoll constraint for the specified constraint of 
  ///   this constraint chain.
  /// \param iConstraintIndex
  ///   index of the constraint for which to append a ragdoll constraint
  void AppendRagdollConstraint(unsigned int iConstraintIndex);

  /// \brief
  ///   Removes the ragdoll cosntraint for the specified constraint
  /// \param iConstraintIndex
  ///   index of the constraint for which to remove the ragdoll constraint
  void RemoveRagdollConstraint(unsigned int iConstraintIndex);

  /// \brief
  ///   Updates the ragdoll constraint for the specified constraint
  ///   of this constraint chain.
  /// \param iConstraintIndex
  ///   index of the constraint to update
  void UpdateRagdollConstraint(unsigned int iConstraintIndex);

  //@}


  ///@name Adding/Removing Chain Links
  //@{
protected:

  /// \brief
  ///   Creates a hkRigidBody to be used as a link in the constraint chain
  /// \param vPivot1
  ///   the first pivot point used to determine position and orientation for
  ///   the new body
  /// \param vPivot2
  ///   the second pivot point
  /// \return  
  ///   a hkRigidBody instance (capsule) describing the chain link with 
  ///   its position and orientation
  hkpRigidBody *CreateChainLink(hkvVec3 vPivot1, hkvVec3 vPivot2);

  /// \brief
  ///   Creates a hkRigidBody to be used as a link in the constraint chain
  /// \param vPosition
  ///   the position of the center of the new chain link
  /// \param mRotation
  ///   the rotation of the new chain link
  /// \return  
  ///   a hkRigidBody instance (capsule) describing the chain link with 
  ///   its position and orientation
  hkpRigidBody *CreateChainLink(hkvVec3 const& vPosition, 
    hkvMat3 const& mRotation);

  /// \brief
  ///   Returns the maximum gap allowed for the last link of the chain.
  /// \return
  ///   the maximum gap (0.0 .. 1.0)
  /// \note
  ///   Must be overwritten in derived classes!
  virtual float GetLastLinkAllowedGap() const;

  /// \brief
  ///   Adds a new link at the end of the chain which is attached
  ///   to the second anchor.
  /// \return
  ///   \c true if the link could be added; \c false if not
  bool AddChainLink();

  /// \brief
  ///   Removes a link from the end of the chain which is attached
  ///   to the second anchor.
  /// \return
  ///   \c true if the link could be removed; \c false if not
  bool RemoveChainLink();

  /// \brief
  ///   Calculates the number of links required for a chain of the 
  ///   specified length.
  /// \param fLength
  ///   the desired chain length
  /// \return
  ///   the number of links required
  unsigned int CalcNumLinksRequired(float fLength);

  /// \brief
  ///   Updates the physics properties of the constraint chain; called 
  ///   after the constraint chain's configuration has changed.
  virtual void UpdateChainPhysicsProperties();

  //@}

  /// @name Data Exchange
  //@{
  
  /// \brief
  ///   Creates the native Havok constraint chain data instance for this type
  ///   of constraint chains.
  /// \return  
  ///   an instance of a hkpConstraintChainData subclass
  virtual hkpConstraintChainData* CreateConstraintChainData();
  
  /// \brief
  ///   Creates a vHavok constraint chain descriptor for this type of
  ///   constraint chains.
  /// \return  
  ///   an instance of a vHavokConstraintChainDesc subclass
  virtual vHavokConstraintChainDesc *CreateConstraintChainDesc();

  /// \brief
  ///   Reads and validates the parameters from the passed constraint
  ///   chain description.
  /// \param desc
  ///   the constraint chain description
  /// \return
  ///   \c true if successful, \c false if not
  ///
  /// This method is called form Init() and can be overwritten in 
  /// derived classes to read settings specific to the constraint chain
  /// type. No attempt is made to create the Havok constraint chain before
  /// this method has been called.
  virtual bool GetParametersFromDesc(vHavokConstraintChainDesc const& desc);

  /// \brief
  ///   Initializes a native Havok constraint chain data object from a
  ///   vHavok constraint chain descriptor.
  /// \param data
  ///   the Havok constraint chain data
  /// \param desc
  ///   the vHavok constraint chain descriptor
  virtual void InitConstraintChainDataFromDesc(hkpConstraintChainData& data, 
    vHavokConstraintChainDesc const& desc);
  
public:
  /// \brief
  ///   Saves the configuration of this constraint chain to a vHavok
  ///   constraint chain descriptor.
  /// \param desc
  ///   the vHavok constraint chain descriptor
  VHAVOK_IMPEXP virtual void SaveToDesc(vHavokConstraintChainDesc& desc);
  
  //@}


protected:
  vHavokPhysicsModule *m_pModule;                 ///< The physics module instance that manages this constraint chain
  hkArray<vHavokRagdollConstraintInfo> m_ragdollConstraintInfos; ///< The ragdoll constraint info created by the ragdoll extension
  unsigned int m_iNumLinks;                       ///< The number of links in this constraint chain
  hkpConstraintChainData *m_pConstraintChainData; ///< The data object that contains the Havok state of this constraint chain.

  VSmartPtr<vHavokRigidBody> m_spAnchorBodies[2]; ///< The rigid bodies used as anchors for the constraint chain ends
  hkvVec3 m_vAnchorPivots[2];                ///< The pivot points of the anchor bodies
  float m_fLinkLength;                            ///< The desired length of each chain link
  float m_fLinkGap;                               ///< The gap between two chain links
  float m_fDiameter;                              ///< The diameter of the chain
  float m_fMaterialDensity;                       ///< The density of the link material
  float m_fMaterialStretchingConstant;            ///< The stretching constant of the link material
  float m_fLinkLinearDamping;                     ///< The linear damping of each chain link
  float m_fLinkAngularDamping;                    ///< The angular damping of each chain link
  bool m_bCalculateLinkInertia;                   ///< Whether to let Havok calculate an inertia tensor for the chain links
  hkvVec3 m_vInertiaTensorAxisScaling;       ///< Scaling of the chain link inertia tensor axes
  float m_fInertiaTensorUniformScaling;           ///< Uniform scaling of the chain link inertia tensor
  int m_iCollisionFilterInfo;                     ///< The collision filter info for the chain links
  bool m_bComputeCfm;                             ///< Whether to compute the CFM (or to use a fixed CFM otherwise)
  float m_fCfmFactor;                             ///< Factor to scale computed CFM (constraint force mixing) with. 0.f: don't compute CFM and use Havok default.
  float m_fFixedCfm;                              ///< The fixed CFM to use when the CFM is not computed
  bool m_bUseAngularFriction;                     ///< Whether to use angular friction
  float m_fFrictionFactorMin;                     ///< The minimum internal friction factor (in the middle of the chain)
  float m_fFrictionFactorMax;                     ///< The maximum internal friction factor (at the ends of the chain)
  float m_fBaseTension;                           ///< The minimum tension the rope is assumed to always have (used for internal friction calculations)

private:
  vHavokConstraintChainDesc *m_pSerializationDesc; ///< Temporary description used for serialization.
  hkpConstraintChainInstance *m_pConstraintChain; ///< The Havok runtime interface to this constraint chain

  VString m_sChainPathKey; ///< The key of the path which the links of the constraint chain must follow.
  DynArray_cl<hkpRigidBody *> m_LinkBodies; ///< The list of Havok rigid bodies that are used for the chain links. This does not include the anchor bodies at either end of the chain.

  float m_fApproximationError; ///< The average error of the segment-wise approximation of the path by the links of the chain.

  float m_fUnstretchedLength; ///< Length of the constraint chain when the chain links are not pulled apart (in Vision units)
  float m_fLastLinkExtra; ///< Amount that needs to be compensated at the last link to make the chain the correct length (in Vision units, may be positive or negative)

  hkvVec3 m_vChainForce; ///< The force currently in effect on the chain

  bool m_bDisposed; ///< Whether this constraint chain has been disposed

  vHavokConstraintChainListenerProxy m_listener;
  
  static const unsigned int s_iSerialVersion;
};


// ----------------------------------------------------------------------------
// vHavokConstraintChain::TempRemoveFromWorld
// ----------------------------------------------------------------------------
/// \brief
///   Internal helper to temporarily remove a constraint chain from the 
///   simulation world.
class vHavokConstraintChain::TempRemoveFromWorld
{
public:
  /// \brief
  ///   Constructor; removes the passed constraint chain from the
  ///   simulation world.
  /// \param chain
  ///   the constraint chain to remove
  TempRemoveFromWorld(hkpConstraintChainInstance &chain);
  
  /// \brief
  ///   Destructor; re-adds the constraint chain to the simulation 
  ///   world if it was removed from there in the constructor.
  ~TempRemoveFromWorld();

private:
  hkpWorld *pWorld; ///< The current physics world
  hkpConstraintChainInstance &chain; ///< The constraint chain
  bool bWasInWorld; ///< Whether the constraint chain was removed from the world in the constructor
};

#endif //V_HAVOK_CONSTRAINT_CHAIN_HPP_INCLUDED

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
