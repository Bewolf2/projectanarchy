/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_CONSTRAINT_HPP_INCLUDED
#define V_HAVOK_CONSTRAINT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokElementManager.hpp>

#pragma managed(push, off)
#include <Common/Base/hkBase.h>
#include <Physics2012/Dynamics/Constraint/hkpConstraintListener.h>
#pragma managed(pop)

class hkpBreakableConstraintData;
class hkpConstraintData;
class hkpConstraintInstance;
class hkpMalleableConstraintData;

class vHavokPhysicsModule;
class vHavokRigidBody;
class vHavokConstraint;

struct vHavokConstraintBreakInfo_t;

HAVOK_ELEMENTMANAGER_TEMPLATE_DECL(vHavokConstraint);

/// \brief
///   Base class for descriptors of Havok constraints
///
/// This base class holds all the properties that are common to all 
/// constraint types in vHavok. A subclass exists for each concrete
/// Havok constraint type, which may contain additional properties.
class vHavokConstraintDesc : public VTypedObject
{
public:
  /// \brief
  ///   Default constructor; initializes all fields to default settings.
  vHavokConstraintDesc();
protected:
  /// \brief
  ///   No-init constructor; leaves the fields uninitialized.
  /// \param bDummyNoInit
  ///   value is ignored
  vHavokConstraintDesc(bool bDummyNoInit);

public:
  /// \brief
  ///   Resets all fields to their default values.
  VHAVOK_IMPEXP virtual void Reset();

  V_DECLARE_SERIAL_DLLEXP( vHavokConstraintDesc, VHAVOK_IMPEXP )
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

public:
  VSmartPtr<vHavokRigidBody> m_spBodies[2]; ///< The bodies used as anchors. The second one may be \c null to use static anchor.
  hkvVec3 m_vPivots[2];                ///< The pivot points of the anchors in body/world space depending on the constraint implementation. If a body is \c null, the pivot coordinates must be in world space.

  bool m_bBreakable;                        ///< Whether this constraint is breakable
  hkReal m_fBreakThreshold;                  ///< The force impulse threshold for breaking the constraint
  bool m_bRevertVelocityOnBreak;            ///< Whether the velocity of the bodies is reset when the constraint breaks

  bool m_bMalleable;                        ///< Whether this constraint is malleable
  hkReal m_fConstraintStrength;              ///< How much of the constraint forces become effective (factor; range. 0.0 - 1.0)

private:
  static const unsigned int s_iSerialVersion; ///< The current serialization version
};

/// \brief
///   Helper class for vHavok event handling - internal use only!
class vHavokConstraintListenerProxy : public hkpConstraintListener
{
public:
	vHavokConstraintListenerProxy() { }
	void setOwner(vHavokConstraint* o ) { m_owner = o; }
	virtual void constraintDeletedCallback(hkpConstraintInstance* constraint);
	vHavokConstraint* m_owner;
};

/// \brief
///   Base class for all vHavok constraint implementations
///
/// Although this class cannot be abstract, it should not be instantiated directly. A
/// subclass of this exists for each type of Havok constraints that vHavok supports.
class vHavokConstraint
: public VRefCounter, 
  public VisTypedEngineObject_cl, 
  public VisObjectKey_cl,
  public vHavokElementManager<class vHavokConstraint*>
{
public:
  /// @name Creation/Initialization/Destruction
  //@{
  /// \brief
  ///   Default constructor.
  VHAVOK_IMPEXP vHavokConstraint();

  /// \brief
  ///   Destructor.
  VHAVOK_IMPEXP VOVERRIDE ~vHavokConstraint();
  
  /// \brief
  ///   Initializes this constraint from the provided constraint descriptor.
  ///
  /// Although this is a non-virtual method taking a base class reference for
  /// the constraint descriptor as parameter, this should only be called on
  /// subclasses of this class, and a reference to a constraint descriptor of
  /// the correct type should be passed.
  ///
  /// \param desc
  ///   the vHavok constraint descriptor
  VHAVOK_IMPEXP bool Init(vHavokConstraintDesc &desc);
  //@}


  /// @name Base Class Overrides
  //@{
  VHAVOK_IMPEXP VOVERRIDE void DisposeObject();
  //@}


  /// @name Callbacks
  //@{
  /// \brief
  ///   Called by the vHavok module when this constraint breaks.
  /// \param pBreakInfo
  ///   Information about the constraint break event, such as the actual
  ///   impulse that cause the breaking
  VHAVOK_IMPEXP void OnBreak(vHavokConstraintBreakInfo_t *pBreakInfo);
  
  /// \brief
  ///   Called when this constraint is established in the simulation.
  VHAVOK_IMPEXP void OnEstablish();
  //@}


  /// \brief
  ///   Indicates whether the constraint was successfully created.
  /// \return
  ///   False if the creation of the Havok constraint failed.
  VHAVOK_IMPEXP bool IsValid() const { return m_pConstraint != NULL; }

  
  /// @name Breakable Constraint Functionality
  //@{
  
  /// \brief
  ///   Returns whether this constraint is breakable.
  /// \return  
  ///   \c true if this constraint is breakable; \c false if not
  VHAVOK_IMPEXP bool IsBreakable() const;
  
  /// \brief
  ///   Returns whether this constraint has been broken.
  /// \return
  ///   \c true if this constraint is breakable and has been broken;
  ///   \c false otherwise.
  VHAVOK_IMPEXP bool IsBroken() const;
  
  /// \brief
  ///   Sets the impulse threshold above which this constraint will break.
  ///   
  /// If this constraint is not breakable, the call has no effect.
  ///   
  /// \param fThreshold
  ///   the new break threshold
  VHAVOK_IMPEXP void SetBreakThreshold(float fThreshold);
  
  //@}
  
  /// \brief
  ///   Cleanup function for removing broken constraints.
  static void RemoveBrokenConstraints();

  // serialization and type management
  V_DECLARE_SERIAL_DLLEXP( vHavokConstraint, VHAVOK_IMPEXP )
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
  VHAVOK_IMPEXP VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);
  VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context) { return context.m_eType != VSerializationContext::VSERIALIZATION_EDITOR; }

  /// @name Internals
  //@{
  
  /// \brief
  ///   Returns the internal Havok constraint instance.
  /// \return
  ///   a hkpConstraintInstance, if the constraint has been successfully 
  ///   established; \c null otherwise
  inline hkpConstraintInstance *GetHkConstraintInstance() const { return m_pConstraint; }

  /// \brief
  ///   Returns the internal Havok constraint runtime.
  /// \return
  ///   a hkpConstraintRuntime, if the constraint has been successfully 
  ///   established; \c null otherwise
  inline hkpConstraintRuntime *GetHkConstraintRuntime() const { return m_pConstraint->getRuntime(); }
  
  /// \brief
  ///   Returns the internal Havok constraint data
  /// \return
  ///   the constraint data, if the constraint has been successfully established;
  ///   \c null otherwise
  inline hkpConstraintData *GetHkConstraintData() const { return m_pBaseConstraintData; }

  //@}


  /// @name Havok callbacks
  //@{
  VHAVOK_IMPEXP VOVERRIDE void constraintDeletedCallback(hkpConstraintInstance* constraint);
  //@}

protected:
  /// @name Data Exchange
  //@{

  /// \brief
  ///   Creates the native Havok constraint data instance for this type
  ///   of constraint.
  /// \return  
  ///   an instance of a hkpConstraintData subclass
  virtual hkpConstraintData* CreateConstraintData();

  /// \brief
  ///   Creates a vHavok constraint descriptor for this type of
  ///   constraint.
  /// \return  
  ///   an instance of a vHavokConstraintDesc subclass
  virtual vHavokConstraintDesc *CreateConstraintDesc();
  
  /// \brief
  ///   Initializes a native Havok constraint data object from a
  ///   vHavok constraint descriptor.
  /// \param data
  ///   the Havok constraint data
  /// \param desc
  ///   the vHavok constraint descriptor
  virtual void InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc);  

  /// \brief
  ///   Stores the custom shape transformation of the archive in temporary member variables
  void RememberCustomArchiveTransformation(VArchive &ar);

  /// \brief
  ///   Applies the custom shape transformation, if available, to the passed constraint description.
  ///   Custom shape transformations are typically used by prefab instances.
  VHAVOK_IMPEXP virtual void ApplyCustomArchiveTransformationToJointDesc(vHavokConstraintDesc *pDesc, hkvVec3& vCustomArchivePositionOfs, hkvMat3 &vCustomArchiveRotationOfs);

  /// \brief
	///   Get a double precision pivot point, considering the constraint's zone
	VHAVOK_IMPEXP virtual bool GetZonedPivot(hkvVec3 const& localPivot, hkvVec3d& worldPivot );

	/// \brief
	///   Get a single precision pivot point from a double precision world pivot
	VHAVOK_IMPEXP virtual void GetLocalPivot(hkvVec3d const& zonedPivot, hkvVec3& localPivot );

	/// \brief
	///   Convert between a physics positions and a single precision Vision position, this will also do unit conversion.
	///   Note: this function avoids double precision math if it is unnecessary (e.g., if there's no zones anyway).
	VHAVOK_IMPEXP virtual void GetLocalPivot(hkVector4 const& zonedPivot, hkvVec3& localPivot );


public:
  /// \brief
  ///   Saves the configuration of this constraint to a vHavok
  ///   constraint descriptor.
  /// \param desc
  ///   the vHavok constraint descriptor
  VHAVOK_IMPEXP virtual void SaveToDesc(vHavokConstraintDesc& desc);

  //@}

protected:
  hkvVec3 m_vSavedPivots[2];                 ///< Saved pivot values of the constraint anchors.
  hkvVec3 m_vCustomArchivePositionOfs;       ///< Temporary custom shape transformation (position offset) read during deserialization
  hkvMat3 m_vCustomArchiveRotationOfs;    ///< Temporary custom shape transformation (rotation offset) read during deserialization
  bool m_bApplyCustomArchiveTransform;            ///< Flag, that indicates, whether m_vCustomArchivePositionOfs/ -RotationOfs should be applied

private:
  /// \brief
  ///   Resets this instance to an uninitialized state. 
  ///
  /// If any Havok objects have been created, they are removed from the 
  /// simulation world, and then released.
  void Reset();

private:
  vHavokPhysicsModule *m_pModule;                 ///< The physics module instance that manages this constraint
  vHavokConstraintDesc *m_pSerializationDesc;     ///< Temporary constraint description used during serialization and deserialization.
  
  hkpConstraintData *m_pBaseConstraintData;       ///< The basic Havok constraint data for this constraint. May be modified by wrapping other constraint data objects around it.
  hkpBreakableConstraintData *m_pBreakableData;   ///< Havok constraint data object describing a breakable constraint. Is \c null if the constraint is not breakable.
  hkpMalleableConstraintData *m_pMalleableData;   ///< Havok constraint data object describing a malleable constraint. Is \c null if the constraint is not malleable.
  hkpConstraintData *m_pFinalConstraintData;      ///< The Havok constraint data that is be used for creating the actual constraint instance.

  hkpConstraintInstance *m_pConstraint;           ///< The Havok constraint instance
  
  bool m_bDisposed;                               ///< Whether this object has been disposed.

  vHavokConstraintListenerProxy  m_listener;
  static const unsigned int s_iSerialVersion;     ///< The current serialization version for objects of this class.
};

#endif //V_HAVOK_CONSTRAINT_HPP_INCLUDED

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
