/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ParticleConstraint.hpp

#ifndef PARTICLECONSTRAINT_HPP_INCLUDED
#define PARTICLECONSTRAINT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>

class ParticleGroupBase_cl;
class VisParticleConstraintList_cl;
class IVPhysicsParticleCollection_cl;
class VTerrain;

///\brief
///Defines the interaction mode of particles with a constraint
///
///\see
///VisParticleConstraint_cl::HandleParticles
enum VIS_CONSTRAINT_REFLECT_BEHAVIOR_e
{
  CONSTRAINT_REFLECT_DEFAULT = -1,  ///< Fall back to default mode (if defined)
  CONSTRAINT_REFLECT_NOTHING = 0,   ///< Nothing happens except for posiotn constraining
  CONSTRAINT_REFLECT_BOUNCE  = 1,   ///< The particle bounces, i.e. its velocity is reflected at the normal
  CONSTRAINT_REFLECT_GLIDE   = 2,   ///< The particle valocity is aligned to the surface normal
  CONSTRAINT_REFLECT_REMOVE  = 3,   ///< The particle dies
};

///\brief
///Axis enum, used by some constraint types
///
enum VIS_AXIS_e
{
  AXIS_NONE = -1, ///< No axis
  AXIS_X = 0,     ///< x-axis
  AXIS_Y = 1,     ///< y-axis
  AXIS_Z = 2,     ///< z-axis
};


class TiXmlElement;
class IVRenderInterface;
class VisParticleConstraint_cl;
typedef VSmartPtr<VisParticleConstraint_cl> VisParticleConstraintPtr;

///\brief
///Base class for particle constraint
///
///A constraint is an object that can influence particle movements of effects that are linked to this constraint.
///The interface is kept rather abstract so it can be used for other types of particles. For instance, cloth particles
///are simulated using this interface as well.
///Constraints can constrain particle's positons (e.g. bouncing at a plane) or also its velocities, e.g. wind, turbulences etc.
class VisParticleConstraint_cl : public VisObject3D_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraint_cl,  PARTICLE_IMPEXP )

  ///
  /// @name Constructor and Init
  /// @{
  ///

  ///\brief
	///Default constructor
	///
	///\param eReflectMode
	///Defines the default constraint behavior associated with this constraint object
	///
	///\param fPersistance
  ///Effect amount for eReflectMode. For instance, for CONSTRAINT_REFLECT_BOUNCE it defines the reflectivity [0..1]
	///
	PARTICLE_IMPEXP VisParticleConstraint_cl(VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode, float fPersistance);

  ///\brief
	///Destructor
	PARTICLE_IMPEXP virtual ~VisParticleConstraint_cl();

  ///\brief
	///Overridden dispose function that removes this instance from the global list
	///
  PARTICLE_IMPEXP virtual void DisposeObject() HKV_OVERRIDE; ///< object3d override

  ///
  /// @}
  ///

  ///
  /// @name Activation
  /// @{
  ///

  ///\brief
	///Sets the activation status for this constraint. Inactive constraints do not influence particles
	///
	///\param bStatus
	///New status
	///
	inline void SetActive(bool bStatus=true) {m_bActive=bStatus;}

  ///\brief
	///Returns the current activation status
	///
	///\returns
	///Activation status
	///
	///\see
	///SetActive
	inline bool IsActive() const {return m_bActive;}

  ///\brief
	///Internal function that flags this object so that it will be removed in the next simulation iteration
	///
	inline void FlagForRemoval() {m_bIsDead=true;}
  ///\brief
	///Returns the state that has been set via FlagForRemoval
	///
	///\returns
	///true if FlagForRemoval has been called
	///
	inline bool IsFlaggedForRemoval() const {return m_bIsDead;}


  ///
  /// @}
  ///

  ///
  /// @name Reflection properties
  /// @{
  ///

  ///\brief
	///Sets reflection mode and intensity for this object
	///
	///\param eMode
	///New mode
	///
	///\param fPersistance
  ///New intensity (unused for some of the modes)
	///
	inline void SetReflection(VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eMode, float fPersistance) {m_eReflectBehavior=eMode;m_fPersistance=fPersistance;}

  ///\brief
	///Returns the current reflection mode as passed to the constructor or SetReflection
	///
	///\returns
	///Current reflection mode
	///
	///\see
	///SetReflection
	inline VIS_CONSTRAINT_REFLECT_BEHAVIOR_e GetReflectionMode() const {return m_eReflectBehavior;}

  ///\brief
	///Returns the current reflection intensity as passed to the constructor or SetReflection
	///
	///\returns
	///Current reflection intensity
	///
	///\see
	///SetReflection
	inline float GetReflectionPersistance() const {return m_fPersistance;}

  ///\brief
	///Sets the noise parameter which is used by a few constraint types/reflection modes. The default value is 0
	///
	///\param fVal
  ///Noise randomness [0..1]
	///
	inline void SetReflectionNoise(float fVal) {m_fReflectionNoise = fVal;}

  ///\brief
	///Returns the value previously set with SetReflectionNoise
	///
	///\returns
	///Current noise value
	///
	///\see
	///SetReflectionNoise
	inline float GetReflectionNoise() const {return m_fReflectionNoise;}


  ///
  /// @}
  ///

  ///
  /// @name Debugging
  /// @{
  ///

  ///\brief
	///Associates a debug rendering color with this instance
	///
	///\param iColor
	///New color
	///
	inline void SetDebugColor(VColorRef iColor) {m_iDebugColor=iColor;}

  ///\brief
	///Returns the current debug color
	///
	///\returns
	///Current color
	///
	inline VColorRef GetDebugColor() const {return m_iDebugColor;}


  ///
  /// @}
  ///

  ///
  /// @name Overridables
  /// @{
  ///

  ///\brief
	///Pure virtual function to apply this constraint to an array of particles
	///
	///This is one of the key functions of this interface. It must be overridden by constraint implementations to perform
  ///the actual simulation. The simulation is multithreaded so this function must be thread safe.
	///
	///\param pGroup
	///Interface that provides the particle array and array stride
	///
	///\param fTimeDelta
	///Time delta since last simulation - used by some constraints
	///
	///\param eForceBehavior
	///If not CONSTRAINT_REFLECT_DEFAULT then this value overrides the constrain's own mode
	///
	///\see
	///IVPhysicsParticleCollection_cl
	PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior = CONSTRAINT_REFLECT_DEFAULT) {}

  ///\brief
	///Overridable to determine whether this constraint potentially affects anything in the passed bounding box.
  ///
  ///The result might be a rough estimate. It can be used to early-out constraint simulation on a particle group
	///
	///\param bbox
	///The world-space bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) { return false; }

  ///\brief
	///Overridable to render a preview of this constraint. Used by vForge
	///
	///\param pRenderer
	///The renderer interface that can be used to render lines etc.
	///
	///\param vHelperPos
	///Helper pivot position, i.e. position of the underlying VisObject3D_cl
	///
	PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const {}

  ///\brief
  ///Helper function to serialize (read/write) constraint properties to XML file. Currently not used
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
	///
	PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) { return false; }

  ///
  /// @}
  ///


  ///
  /// @name Creation and Serialization
  /// @{
  ///

  ///\brief
	///Static helper function to create a constraint instance from XML node
	///
	///\param pNode
	///XML node that constains the definition
	///
	///\returns
	///Pointer to new created base class
	///
	PARTICLE_IMPEXP static VisParticleConstraint_cl *CreateConstraint(TiXmlElement *pNode);

  ///\brief
	///Base implementation for binary serialization. Serialization is used for vscene exports
	///
	///\param ar
	///Binary archive
	///
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;


  ///
  /// @}
  ///

  ///
  /// @name Global List
  /// @{
  ///

  ///\brief
	///Global function to access the list of all constraints in the scene
	///
	///\returns
	///Reference to the collection
	///
	PARTICLE_IMPEXP static VisParticleConstraintList_cl &GlobalConstraintList();

  ///
  /// @}
  ///

public:
  VColorRef m_iDebugColor;  ///< Debug color
  VIS_CONSTRAINT_REFLECT_BEHAVIOR_e m_eReflectBehavior; ///< Reflection behavior
  bool m_bActive; ///< current active status
  bool m_bIsDead; ///< current flag-for-removal status
  char m_iLocalLoadingVersion; ///< only relevant during serialization
  float m_fPersistance;  ///< Reflection mode intensity
  unsigned int m_iAffectBitMask;  ///< constraint filter bitmask
  float m_fReflectionNoise; ///< Noise value

protected:
  ///\brief
	///Empty constructor for serialization
	PARTICLE_IMPEXP VisParticleConstraint_cl();

  ///\brief
	///Base implementation for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param szType
  ///constraint type (short name)
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
	///
	PARTICLE_IMPEXP bool DataExchangeXML_Base(TiXmlElement *pNode, const char *szType, bool bWrite);

  static VisParticleConstraintList_cl g_GlobalConstraintList; ///< Global list (static)
};


///\brief
///Collection class for constraints. Elements are reference counted
///
class VisParticleConstraintList_cl
{
public:
  V_DECLARE_SERIALX( VisParticleConstraintList_cl, PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	PARTICLE_IMPEXP VisParticleConstraintList_cl();

  ///\brief
	///Destructor
  PARTICLE_IMPEXP ~VisParticleConstraintList_cl();


  ///
  /// @}
  ///

  ///
  /// @name Add/Remove
  /// @{
  ///

  ///\brief
	///Returns the current number of constraints in this list
	///
	///\returns
	///Number of constraints
	///
	inline int GetConstraintCount() const;

  ///\brief
	///Adds a constraint to the list. The refcounter is increased
	///
	///\param pConstraint
	///Constraint to add
	///
	///\param eForceBehavior
	///Associates an override behavior with this instance
	///
	///\returns
	///Index at insertion
	///
	PARTICLE_IMPEXP int AddConstraint(VisParticleConstraint_cl *pConstraint, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior = CONSTRAINT_REFLECT_DEFAULT);

  ///\brief
	///Returns the constraint at specified index
	///
	///\param iIndex
  ///Index, must be [0..GetConstraintCount()-1]
	///
	///\returns
	///The constraint instance
	///
	inline VisParticleConstraint_cl *GetConstraint(int iIndex) const;

  ///\brief
	///Removes the constraint at specified index. The refcounter of that object is decremented
	///
	///\param iIndex
  ///Index, must be [0..GetConstraintCount()-1]
	///
	///\returns
	///true if constraint could be removed
	///
	PARTICLE_IMPEXP bool RemoveConstraint(int iIndex);

  ///\brief
	///Remove constraint from the list. The refcounter of that object is decremented
	///
	///\param pConstraint
	///Constraint to remove
	///
	///\returns
	///true if constraint could be removed
	///
	PARTICLE_IMPEXP bool RemoveConstraint(VisParticleConstraint_cl *pConstraint);

  ///\brief
	///Clears the collection and decrements the refcounter of all objects in it
	PARTICLE_IMPEXP void ReleaseAllConstraints();

  ///\brief
	///Copies the content from another list. Clears this list beforehand. Reference counting is handled gracefully for the objects
	///
	///\param pSrcList
	///List to copy
	PARTICLE_IMPEXP void CopyFrom(const VisParticleConstraintList_cl* pSrcList);

  ///\brief
  ///Add the content from another list if not already in the list of constraints
  ///
  ///\param pSrcList
  ///List to add
  PARTICLE_IMPEXP void AddFrom(const VisParticleConstraintList_cl* pSrcList);

  ///
  /// @}
  ///


  ///
  /// @name Constraint handling
  /// @{
  ///

  ///\brief
	///Calls the HandleParticles function on all collection members. 'Dead' constraints will be removed from the collection
	///
	///\param pGroup
	///Particles to simulate
	///
	///\param fTimeDelta
	///Time delta forwared to constraints
	///
	///\param iAffectMask
	///Filter mask to test on each constraint in the list
	///
	inline void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, unsigned int iAffectMask=0xffffffff);

  ///\brief
	///Similar to HandleParticles except that it does not remove dead constraints
	inline void HandleParticlesNoRemove(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, unsigned int iAffectMask=0xffffffff);

  ///\brief
	///Helper function to call the DebugRender function on each constraint in the list
	///
	///\param pRenderer
	///Forwarded to DebugRender
	///
	///\param bActiveOnly
	///if true, it renders only the active constraints
	///
	inline void RenderConstraints(IVRenderInterface *pRenderer, bool bActiveOnly=true) const;

  ///
  /// @}
  ///


  ///
  /// @name IO
  /// @{
  ///

  ///\brief
	///Not used
	///
	///\param szFilename
	///do not use
	///
	///\returns
	///do not use
	///
	PARTICLE_IMPEXP bool LoadFromFile(const char *szFilename);

  ///\brief
	///Not used
	///
	///\param szFilename
	///do not use
	///
	///\returns
	///do not use
	PARTICLE_IMPEXP bool SaveToFile(const char *szFilename);

  ///\brief
	///XML serialization function for the full list
	///
	///\param pNode
	///Parent node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
	///
	PARTICLE_IMPEXP bool DataExchangeXML(TiXmlElement *pNode, bool bWrite);

  ///\brief
	///Static function that returns the last error string in case loading/saving returned false
	///
	///\returns
	///Error string
	///
	static inline const char *GetLastError() {return g_sLastError;}

  ///\brief
	///Serialization of the constraint collection
	///
	///\param ar
	///Binary archive
	PARTICLE_IMPEXP void SerializeX( VArchive &ar );
  
  ///
  /// @}
  ///

protected:
  int m_iConstraintCount; ///< number of elements
  DynArray_cl<VisParticleConstraint_cl *>m_Constraint;  ///< constraint pointers
  DynArray_cl<VIS_CONSTRAINT_REFLECT_BEHAVIOR_e>m_ForceBehavior; ///< forced behavior (same indexing as m_Constraint)
  static VString g_sLastError; ///< Static error string
};


///\brief
///Simple point constraint. Has no effect for particles but it can pinpoint cloth objects as it is associated with a specific particle index
///
class VisParticleConstraintPoint_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintPoint_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param vWorldPos
	///point's world position
	///
	///\param fRadius
	///Point's search radius for cloth vertices
	///
	PARTICLE_IMPEXP VisParticleConstraintPoint_cl(const hkvVec3& vWorldPos, float fRadius=4.f)
    : VisParticleConstraint_cl(CONSTRAINT_REFLECT_NOTHING,0.f)
  {
    m_fRadius = fRadius;
    SetPosition( vWorldPos );
    m_iDebugColor.SetRGBA(255,0,0,180);
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
	PARTICLE_IMPEXP VisParticleConstraintPoint_cl(TiXmlElement *pNode)
  {
    DataExchangeXML(pNode,false);
  }


  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
	PARTICLE_IMPEXP static const char *GetShortName() {return "point";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;


  ///
  /// @}
  ///

  ///
  /// @name Custom properties
  /// @{
  ///

  ///\brief
	///Sets the search radius for the point
	///
	///\param fRad
	///New radius
	///
	inline void SetRadius(float fRad) {m_fRadius=fRad;}

  ///\brief
  ///Returns the search radius (see SetRadius)
	///
	///\returns
	///The current search radius
	///
	inline float GetRadius() const {return m_fRadius;}

  ///
  /// @}
  ///

protected:

  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleConstraintPoint_cl() {}
  float m_fRadius; ///< tolerance to assign to cloth for instance
};


///\brief
///Simple non-oriented ground plane constraint. Particles below that z-level are constrained
///
class VisParticleConstraintGroundPlane_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintGroundPlane_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param fHeight
	///z-level of the ground plane
	///
	///\param eReflectMode
	///Reflection mode
	///
	///\param fPersistance
	///Reflection intensity
	///
	PARTICLE_IMPEXP VisParticleConstraintGroundPlane_cl(float fHeight, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode=CONSTRAINT_REFLECT_NOTHING, float fPersistance=0.2f)
    : VisParticleConstraint_cl(eReflectMode,fPersistance)
  {
    m_iDebugColor.SetRGBA(100,180,255,64);
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleConstraintGroundPlane_cl(TiXmlElement *pNode)
  {
    DataExchangeXML(pNode,false);
  }

  
  ///
  /// @}
  ///


  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "groundplane";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  PARTICLE_IMPEXP virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;

  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleConstraintGroundPlane_cl() {}

};

///\brief
///Generic plane constraint. Particles are constrained to the positive side of the plane
///
///For ground planes please use class VisParticleConstraintGroundPlane_cl
///
class VisParticleConstraintPlane_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintPlane_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param eReflectMode
	///Reflection mode
	///
	///\param fPersistance
	///Reflection intensity
	///
	PARTICLE_IMPEXP VisParticleConstraintPlane_cl(VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode=CONSTRAINT_REFLECT_NOTHING, float fPersistance=0.2f)
    : VisParticleConstraint_cl(eReflectMode, fPersistance)
  {
    m_bInfinite = true;
    m_iDebugColor.SetRGBA(100,255,180,64);
    m_fSizeX = m_fSizeY = m_fThickness =  0.f;
    m_Plane.setNormalAndDistance(0,0,1.f,0);
  }

  ///\brief
	///Constructor that takes a source plane
	///
	///\param plane
	///Plane
	///
	///\param eReflectMode
	///Reflection mode
	///
	///\param fPersistance
	///Reflection intensity
	PARTICLE_IMPEXP VisParticleConstraintPlane_cl(const hkvPlane& plane, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode=CONSTRAINT_REFLECT_NOTHING, float fPersistance=0.2f)
    : VisParticleConstraint_cl(eReflectMode, fPersistance)
  {
    m_bInfinite = true;
    m_iDebugColor.SetRGBA(100,255,180,64);
    m_fSizeX = m_fSizeY = m_fThickness =  0.f;
    m_Plane = plane;
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleConstraintPlane_cl(TiXmlElement *pNode) {DataExchangeXML(pNode,false);}

  ///
  /// @}
  ///

  ///
  /// @name Custom properties
  /// @{
  ///

  ///\brief
	///Optionally limits the size of the plane around its pivot
	///
	///\param bInfinite
	///false to limit the plane
	///
	///\param fSizeX
  ///New size around the origin (ignored if bInfinite=true)
	///
	///\param fSizeY
  ///New size around the origin (ignored if bInfinite=true)
	///
	///\param fThickness
  ///Thickness for testing particle penetration nto plane. Particles below this plane margin are not influenced. Ignored if bInfinite=true
	inline void SetPlaneExtent(bool bInfinite, float fSizeX, float fSizeY, float fThickness)
  {
    m_bInfinite = bInfinite || (fSizeX<=0.f) || (fSizeY<=0.f);
    m_fSizeX = fSizeX;
    m_fSizeY = fSizeY;
    m_fThickness = fThickness;
  }

  ///\brief
  ///Returns the current plane constraint
  PARTICLE_IMPEXP hkvPlane* GetPlane( ){ return &m_Plane; }

  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "plane";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  PARTICLE_IMPEXP virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;

  hkvPlane m_Plane;  ///< Current Plane
  bool m_bInfinite;     ///< indicates whether an infinite plane should be used
  float m_fSizeX;       ///< Plane size if not infinite
  float m_fSizeY;       ///< Plane size if not infinite
  float m_fThickness;   ///< Plane test thickness if not infinite
};


///\brief
///Sphere constraint. Particles can be constrained to the inside or outside of the sphere.
class VisParticleConstraintSphere_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintSphere_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param center
	///Sphere center
	///
	///\param fRadius
	///Sphere radius
	///
	///\param bInside
	///if true, particles are kept inside the sphere, otherwise outside
	///
	///\param eReflectMode
	///Reflection mode
	///
	///\param fPersistance
	///Reflection intensity
	PARTICLE_IMPEXP VisParticleConstraintSphere_cl(const hkvVec3& center, float fRadius, bool bInside, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode=CONSTRAINT_REFLECT_NOTHING, float fPersistance=0.2f)
    : VisParticleConstraint_cl(eReflectMode, fPersistance)
  {
    m_iDebugColor.SetRGBA(255,240,10,64);
    SetPosition(center);
    m_fRadius = fRadius;
    m_bInside = bInside;
    m_eAxis = AXIS_NONE;
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleConstraintSphere_cl(TiXmlElement *pNode) 
  {
    m_fRadius = 100.f;
    m_bInside = false;
    m_eAxis = AXIS_NONE;
    DataExchangeXML(pNode,false);
  }


  ///
  /// @}
  ///

  ///
  /// @name Custom properties
  /// @{
  ///

  ///\brief
	///Sets the radious of the sphere.
	///
	///\param fRadius
	///New radius
	inline void SetRadius(float fRadius) {m_fRadius=fRadius;}

  ///\brief
	///Returns the current radius
	///
	///\returns
	///Radius
	///
	inline float GetRadius() const {return m_fRadius;}

  ///\brief
	///Defines whether particles are constrained to the inside or the outside of the sphere
	///
	///\param bStatus
	///true to keep the particles inside
	inline void SetInside(bool bStatus) {m_bInside=bStatus;}

  ///\brief
	///Returns the status that has been set with SetInside
	///
	///\returns
	///true to keep the particles inside
	inline bool GetInside() const {return m_bInside;}

  ///
  /// @}
  ///


  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "sphere";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:

  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleConstraintSphere_cl() {}
  float m_fRadius;    ///< Sphere radius
  bool m_bInside;     ///< Inside flag
  VIS_AXIS_e m_eAxis; ///< Used by cylinder constraints
};


///\brief
///Infinite cylinder along aligned axis. Particles can be constrained to the inside our outside
class VisParticleConstraintInfCylinder_cl : public VisParticleConstraintSphere_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintInfCylinder_cl,  PARTICLE_IMPEXP )

  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param center
  ///Cylinder center (point on axis)
	///
	///\param fRadius
	///Radius
	///
	///\param bInside
	///Inside flag (see sphere=
	///
	///\param eAxis
	///Axis constant
	///
	///\param eReflectMode
	///Reflection mode
	///
	///\param fPersistance
	///Reflection intensity
	///
	PARTICLE_IMPEXP VisParticleConstraintInfCylinder_cl(const hkvVec3& center, float fRadius, bool bInside, VIS_AXIS_e eAxis, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode=CONSTRAINT_REFLECT_NOTHING, float fPersistance=0.2f)
    : VisParticleConstraintSphere_cl(center,fRadius, bInside, eReflectMode, fPersistance)
  {
    m_iDebugColor.SetRGBA(200,50,255,64);
    m_eAxis = eAxis; ///< this makes a sphere a cylinder :-)
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleConstraintInfCylinder_cl(TiXmlElement *pNode) 
    : VisParticleConstraintSphere_cl(hkvVec3::ZeroVector (),100.f,false)
  {
    m_eAxis = AXIS_X;
    DataExchangeXML(pNode,false);
  }

  ///
  /// @}
  ///

  ///
  /// @name Custom Properties
  /// @{
  ///

  ///\brief
	///Returns the direction of the cylinder
	///
	///\returns
	///Normalized direction (e.g. hkvVec3(1,0,0) for AXIS_X
	///
	inline hkvVec3 GetDirection() const
  {
    switch (m_eAxis)
    {
    case AXIS_X:return hkvVec3(1,0,0);
    case AXIS_Y:return hkvVec3(0,1,0);
    case AXIS_Z:return hkvVec3(0,0,1);
    default:VASSERT(FALSE);
    }
    return hkvVec3::ZeroVector ();
  }

  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "infcylinder";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleConstraintInfCylinder_cl() {}

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///
  /// @}
  ///
};


///\brief
///Constraint for axis aligned bounding box. Particles can be constrained to the inside or outside of the box
class VisParticleConstraintAABox_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintAABox_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param bbox
	///source box
	///
	///\param bInside
	///Inside flag. If true, particles are kept inside the box, otherwise outside
	///
	///\param eReflectMode
	///Reflection mode
	///
	///\param fPersistance
	///Reflection intensity
	PARTICLE_IMPEXP VisParticleConstraintAABox_cl(const hkvAlignedBBox &bbox, bool bInside, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode=CONSTRAINT_REFLECT_NOTHING, float fPersistance=0.2f)
    : VisParticleConstraint_cl(eReflectMode, fPersistance)
  {
    m_iDebugColor.SetRGBA(255,50,30,64);
    VASSERT(bbox.isValid());
    m_Box = bbox;
    SetPosition( m_Box.getCenter() );
    m_bInside = bInside;
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleConstraintAABox_cl(TiXmlElement *pNode) 
  {
    m_bInside = true;
    DataExchangeXML(pNode,false);
  }


  ///
  /// @}
  ///

  ///
  /// @name Custom properties
  /// @{
  ///

 ///\brief
	///Defines whether particles are constrained to the inside or the outside of the box
	///
	///\param bStatus
	///true to keep the particles inside
	inline void SetInside(bool bStatus) {m_bInside=bStatus;}

  ///\brief
	///Returns the status that has been set with SetInside
	///
	///\returns
	///true to keep the particles inside
	inline bool GetInside() const {return m_bInside;}

  ///\brief
	///Sets a new box size around the current box's center
	///
	///\param x
	///New x-extent of the box
	///
	///\param y
	///New y-extent of the box
	///
	///\param z
	///New z-extent of the box
	///
	inline void SetBoxSize(float x, float y, float z)
  {
    hkvVec3 vCenter = m_Box.getCenter();
    hkvVec3 vRadius(x*0.5f,y*0.5f,z*0.5f);
    m_Box.m_vMin = vCenter-vRadius;
    m_Box.m_vMax = vCenter+vRadius;
  }

  ///
  /// @}
  ///


  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "aabox";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  PARTICLE_IMPEXP virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;

  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleConstraintAABox_cl() {}

  hkvAlignedBBox m_Box; ///< Current box
  bool m_bInside; ///< Inside flag
};



///\brief
///Constraint for oriented bounding box. Only implemented for inside
///
class VisParticleConstraintOBox_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintOBox_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param bbox
	///local unrotated box
	///
	///\param vBoxOrient
  ///box rotation (Euler angles)
	///
	///\param vBoxCenter
	///box center
	///
	///\param bInside
	///currently this constraint only checks for particles inside the obox. The bInside parameter is ignored.
	///
	///\param eReflectMode
	///Reflection mode
	///
	///\param fPersistance
	///Reflection intensity
	///
	PARTICLE_IMPEXP VisParticleConstraintOBox_cl(const hkvAlignedBBox &bbox, const hkvVec3& vBoxOrient, const hkvVec3& vBoxCenter, bool bInside, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode=CONSTRAINT_REFLECT_NOTHING, float fPersistance=0.2f)
    : VisParticleConstraint_cl(eReflectMode, fPersistance)
  {
    m_iDebugColor.SetRGBA(255,50,30,64);
    VASSERT(bbox.isValid());
    m_Box = bbox;
    SetOrientation(vBoxOrient);
    SetPosition(vBoxCenter);
    m_bInside = true; /*bInside;*/
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleConstraintOBox_cl(TiXmlElement *pNode) 
  {
    m_bInside = true;
    DataExchangeXML(pNode,false);
  }


  ///
  /// @}
  ///

  
  ///
  /// @name Custom properties
  /// @{
  ///

  ///\brief
	///Currently ignored, inside is always true
	///
	///\param bStatus
	///true to keep the particles inside
  inline void SetInside(bool bStatus) {m_bInside=true; /*bStatus;*/}

  ///\brief
	///Returns the status that has been set with SetInside
	///
	///\returns
	///true to keep the particles inside
	inline bool GetInside() const {return m_bInside;}


  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "obox";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  PARTICLE_IMPEXP virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;

  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleConstraintOBox_cl() {}

  hkvAlignedBBox m_Box; ///< local aabox
  bool m_bInside; ///< Inside flag (ignored, always true)
};


///\brief
///  Constraint that defines a bounding box that is always attached to the main camera. Particles that hit one of the boxes' faces will be wrapped around to the other side
///
class VisParticleConstraintCamBox_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintCamBox_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param vBoxDim
	///Box size around the camera
	///
	PARTICLE_IMPEXP VisParticleConstraintCamBox_cl(const hkvVec3& vBoxDim) 
    : VisParticleConstraint_cl(CONSTRAINT_REFLECT_DEFAULT,0.5f) , m_vBoxDim(vBoxDim)
  {
    m_iDebugColor.SetRGBA(50,255,50,64);
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleConstraintCamBox_cl(TiXmlElement *pNode) : m_vBoxDim(1000.f,1000.f,1000.f)
  {
    DataExchangeXML(pNode,false);
  }


  ///
  /// @}
  ///

  ///
  /// @name Custom properties
  /// @{
  ///

  ///\brief
	///Calcualtes the absolute bounding box using the global camera position
	///
	///\param bbox
	///target box
	///
	inline void GetAbsoluteBBox(hkvAlignedBBox &bbox) const
  {
    hkvVec3 vCamPos;
    Vision::Camera.GetCurrentCameraPosition(vCamPos);
    bbox.m_vMin = vCamPos - m_vBoxDim;
    bbox.m_vMax = vCamPos + m_vBoxDim;
  }

  ///\brief
	///Set the relative size of the box around the camera
  inline void SetBoxRadius(const hkvVec3& radius)
  {
    m_vBoxDim = radius;
  }


  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "cambox";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleConstraintCamBox_cl() {}

  hkvVec3 m_vBoxDim; ///< bounding box dimension relative to current camera position
};


///\brief
///Fan constraint that applies wind speed to particles
class VisParticleAffectorFan_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleAffectorFan_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param vPos
	///Fan origin
	///
	///\param vDirection
	///Fan direction
	///
	///\param fConeAngle
	///Cone angle of the fan
	///
	///\param fIntensity
	///Wind intensity
	///
	PARTICLE_IMPEXP VisParticleAffectorFan_cl(const hkvVec3& vPos, const hkvVec3& vDirection, float fConeAngle, float fIntensity)
    : VisParticleConstraint_cl(CONSTRAINT_REFLECT_DEFAULT,0.5f)
  {
    m_iDebugColor.SetRGBA(50,255,220,64);
    SetPosition(vPos);
    SetDirection(vDirection);
    SetConeAngle(fConeAngle);
    SetIntensity(fIntensity);
    m_fRandomness = 0.f;
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleAffectorFan_cl(TiXmlElement *pNode) 
  {
    m_fConeAngle = 45.f;
    m_fIntensity=100.f;
    DataExchangeXML(pNode,false);
  }

  ///
  /// @}
  ///

  ///
  /// @name Custom properties
  /// @{
  ///

  ///\brief
	///Sets a new cone angle
	///
	///\param fConeAngle
	///New angle in degree
	///
	inline void SetConeAngle(float fConeAngle) {m_fConeAngle = fConeAngle;}

  ///\brief
	///Sets the maximum wind intensity
	///
	///\param fIntensity
	///New intensity
	///
	inline void SetIntensity(float fIntensity) {m_fIntensity = fIntensity;}

  ///\brief
	///Defines the variation of the wind intensity over time
	///
	///\param fVariation
  ///Intensity variation in (0..1) range
	///
	inline void SetRandomness(float fVariation) {m_fRandomness = fVariation;}


  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "fan";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleAffectorFan_cl() {}
  float m_fConeAngle; ///< cone angle
  float m_fIntensity; ///< wind intensity
  float m_fRandomness; ///< wind variation
};


///\brief
///Deprecated
///
class VisParticleAffectorCyclone_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleAffectorCyclone_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param vPos
	///cyclone center
	///
	///\param vDirection
	///Direction of the axis
	///
	///\param fRadius
	///Radius
	///
	///\param fIntensity
	///Intensity
	PARTICLE_IMPEXP VisParticleAffectorCyclone_cl(const hkvVec3& vPos, const hkvVec3& vDirection, float fRadius, float fIntensity)
    : VisParticleConstraint_cl(CONSTRAINT_REFLECT_DEFAULT,0.5f)
  {
    m_iDebugColor.SetRGBA(50,200,255,64);
    SetPosition(vPos);
    SetDirection(vDirection);
    SetRadius(fRadius);
    SetIntensity(fIntensity);
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleAffectorCyclone_cl(TiXmlElement *pNode) 
  {
    m_fAxisLen = 0.f;
    m_fRadius = 50.f;
    m_fIntensity = 100.f;
    DataExchangeXML(pNode,false);
  }

  ///
  /// @}
  ///

  ///
  /// @name Custom properties
  /// @{
  ///

  ///\brief
	///Sets the length of the cylinder.
	///
	///\param fLen
	///New length of the cylinder
	inline void SetLength(float fLen) {m_fAxisLen=fLen;}

  ///\brief
	///Sets the new radius.
	///
	///\param fRadius
	///New radius
	inline void SetRadius(float fRadius) {m_fRadius = fRadius;}

  ///\brief
	///Sets new intensity.
	///
	///\param fIntensity
	///New intensity
	///
	inline void SetIntensity(float fIntensity) {m_fIntensity = fIntensity;}

  
  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "cyclone";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleAffectorCyclone_cl() {}
  float m_fRadius; ///< Radius
  float m_fIntensity; ///< Intensity 
  float m_fAxisLen; ///< Length (multiplier for m_vNormalizedDir)
};


///\brief
///Gravity mass point that applies forces towards the center
///
///The gravity point has a configurable radius and uses the following gravity formula:
///For outside the sphere radius it uses intensity/(r*r) falloff, for inside the sphere it uses 
///intensity*r/radius, i.e. linear falloff towards the center. This is the 'real' gravity profile for spheres.
class VisParticleAffectorGravityPoint_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleAffectorGravityPoint_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param vPos
	///Center of gravity point
	///
	///\param fRadius
	///Radius
	///
	///\param fIntensity
	///Gravity intensity
	///
	PARTICLE_IMPEXP VisParticleAffectorGravityPoint_cl(const hkvVec3& vPos, float fRadius, float fIntensity)
    : VisParticleConstraint_cl(CONSTRAINT_REFLECT_DEFAULT,0.5f)
  {
    m_iDebugColor.SetRGBA(200,0,0,64);
    SetPosition(vPos);
    SetRadius(fRadius);
    SetIntensity(fIntensity);
  }

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleAffectorGravityPoint_cl(TiXmlElement *pNode) 
  {
    m_fRadius = 50.f;
    m_fIntensity = 100.f;
    DataExchangeXML(pNode,false);
  }


  ///
  /// @}
  ///

  ///
  /// @name Custom properties
  /// @{
  ///

  ///\brief
	///Sets a new radius
	///
	///\param fRadius
	///New radius
	///
	inline void SetRadius(float fRadius) {m_fRadius = hkvMath::Max(fRadius,1.f);}

  ///\brief
	///Sets a new intensity
	///
	///\param fIntensity
	///New intensity
	///
	inline void SetIntensity(float fIntensity) {m_fIntensity = fIntensity;}


  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "gravitypoint";}

  ///\brief
	///Empty constructor for serialization
  PARTICLE_IMPEXP VisParticleAffectorGravityPoint_cl() {}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

  float m_fRadius; ///< gravity sphere radius
  float m_fIntensity; ///< gravity intensity
};

#if defined (SUPPORTS_TERRAIN)


///\brief
///Constraint class that allows for particles bouncing off the global terrain
///
///This constraint class uses the global terrain to constraint the particles against the height values. Nice effects 
///such as ground fog can be implemented with it, however, this effect is quite expensive from a computational side.
class VisParticleConstraintTerrain_cl : public VisParticleConstraint_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintTerrain_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Constructor
	///
	///\param pTerrain
	///Terrain to use for collision. Can be NULL to use the first terrain
	///
	///\param eReflectMode
	///bouncing mode
	///
	///\param fPersistance
	///reflectivity for bouncing
	///
	PARTICLE_IMPEXP VisParticleConstraintTerrain_cl(VTerrain *pTerrain=NULL, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode=CONSTRAINT_REFLECT_BOUNCE, float fPersistance=0.2f);



  ///
  /// @}
  ///


  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "terrain";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE 
  {
    return m_pTerrain!=NULL;
  }

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE
  {
  }

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE
  {
    VASSERT_MSG(false, "Not implemented");
    return false;
  }

  ///
  /// @}
  ///

  VTerrain *m_pTerrain;
};


#else
  // Terrain not supported on this platform
#endif


////////////////////////////////////////////////////////////////////
// Bones
//
// Example:
//  int indices[256];
//  indices[0] = pEntity->GetMesh()->GetSkeleton()->GetBoneIndexByName("LeftShoulder");
//  indices[1] = pEntity->GetMesh()->GetSkeleton()->GetBoneIndexByName("RightShoulder");
//  indices[2] = pEntity->GetMesh()->GetSkeleton()->GetBoneIndexByName("Head");
//  VisParticleConstraintBones_cl *pBoneConstraint = new VisParticleConstraintBones_cl(pEntity,3,indices, 0.6f);
////////////////////////////////////////////////////////////////////


///\brief
///Constraint that simulates one or more bones of an entity as capsules
///
///This is a rather complex and CPU intensive constraint and should thus not be used
///
class VisParticleConstraintBones_cl : public VisParticleConstraint_cl, public IVisCallbackHandler_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleConstraintBones_cl,  PARTICLE_IMPEXP )


  ///
  /// @name Constructor
  /// @{
  ///

  ///\brief
	///Destructor
  PARTICLE_IMPEXP virtual ~VisParticleConstraintBones_cl();

  ///\brief
	///Constructor
	///
	///\param pEntity
	///Source entity
	///
	///\param iBoneCount
	///Number of bones to test
	///
	///\param pBoneIndices
	///List of iBoneCount bone indices
	///
	///\param fBoneRadiusScale
	///Scaling factor for bone's own radius
	///
	PARTICLE_IMPEXP VisParticleConstraintBones_cl(VisBaseEntity_cl *pEntity, int iBoneCount, const int *pBoneIndices, float fBoneRadiusScale=1.0f);

  ///\brief
	///Constructor that de-serializes from source XML node
	///
	///\param pNode
	///XML node
	///
  PARTICLE_IMPEXP VisParticleConstraintBones_cl(TiXmlElement *pNode)
  {
    m_pEntity=NULL;m_iBoneCount=0;m_pBoneInfo=NULL;
    DataExchangeXML(pNode,false);
    m_bValid=false;
  }

  ///
  /// @}
  ///


  ///
  /// @name Overrides
  /// @{
  ///

  ///\brief
	///Static function to return the type specific 'short name' for this class
	///
	///\returns
	///String that defines the short name
  PARTICLE_IMPEXP static const char *GetShortName() {return "bones";}

  ///\brief
	///Overridden binary serialization function
	///
	///\param ar
	///Binary archive
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  ///\brief
  ///Overridden function to display the constraint (e.g. in vForge)
	///
	///\param pRenderer
	///Renderer interface for line rendering
	///
	///\param vHelperPos
	///Object's pivot
	///
	///\see
  ///VisParticleConstraint_cl::DebugRender
  PARTICLE_IMPEXP virtual void DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const HKV_OVERRIDE;

  ///\brief
	///Empty constructor for serialization
  inline VisParticleConstraintBones_cl() {m_pEntity=NULL;m_iBoneCount=0;m_pBoneInfo=NULL;m_bValid=false;}

  ///\brief
	///Overridden particle simulation function
	///
	///\param pGroup
	///Particle array to simulate
	///
	///\param fTimeDelta
	///Time delta
	///
	///\param eForceBehavior
	///Reflection behavior override
	///
	///\see
  ///VisParticleConstraint_cl::HandleParticles
  PARTICLE_IMPEXP virtual void HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior) HKV_OVERRIDE;

  ///\brief
	///Overridden bounding box early out test
	///
	///\param bbox
	///Bounding box to test
	///
	///\returns
	///true if this constraint affects the bounding box
	///
	///\see
  ///VisParticleConstraint_cl::Influences
  PARTICLE_IMPEXP virtual bool Influences(const hkvAlignedBBox &bbox) HKV_OVERRIDE;

  ///\brief
	///Overridden function for XML serialization
	///
	///\param pNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true if successful
  PARTICLE_IMPEXP virtual bool DataExchangeXML(TiXmlElement *pNode, bool bWrite) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  ///\brief
	///Overridden IVisCallbackHandler_cl function
	///
	///\param pData
	///Data object
	///
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  ///\brief
	///Internal structure to hold bone instance specific data
  struct BoneInfo_t
  {
    int iIndex;
    float fRangeStart,fBoneLength,fRadius;
    PARTICLE_IMPEXP void DebugRender(IVRenderInterface *pRenderer, VisBaseEntity_cl *pEntity) const;

    hkvVec3 m_vWorldStart;
    hkvVec3 m_vWorldDir; ///< computed every frame
  };

  bool m_bValid;  ///< valid flag
  VisBaseEntity_cl *m_pEntity;  ///< source entity
  int m_iBoneCount; ///< Number of bones to test
  BoneInfo_t *m_pBoneInfo;  ///< bone info array
};


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleConstraint.inl>

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
