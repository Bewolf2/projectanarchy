/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokRigidBody.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKRIGIDBODY_HPP_INCLUDED
#define VHAVOKRIGIDBODY_HPP_INCLUDED

#pragma managed(push, off)
#include <Physics2012/Dynamics/Entity/hkpRigidBody.h>
#pragma managed(pop)

//#include <vHavok/vHavokPhysicsModule.hpp>

struct vHavokColliderInfo_t;
class vHavokPhysicsModule;
class hkResource;

/// Serialization versions
#define VHAVOKRIGIDBODY_VERSION_0        0     // Initial version
#define VHAVOKRIGIDBODY_VERSION_1        1     // Havok Date From File
#define VHAVOKRIGIDBODY_VERSION_2        2     // Havok Motion Type and Auto Pivot Align
#define VHAVOKRIGIDBODY_VERSION_3        3     // Added CollisionLayer, SystemId. Serializing Velocity
#define VHAVOKRIGIDBODY_VERSION_4        4     // Extended Havok Motion Type
#define VHAVOKRIGIDBODY_VERSION_5        5     // Inertia tensor and activation parameters
#define VHAVOKRIGIDBODY_VERSION_6        6     // Added Motion Quality property
#define VHAVOKRIGIDBODY_VERSION_7        7     // Max Kinematic Velocity for kinematic objects
#define VHAVOKRIGIDBODY_VERSION_8        8     // Dynamic init possible, see Havok_AlwaysInitAsDynamic
#define VHAVOKRIGIDBODY_VERSION_9        9     // Activation/deactivation of collision
#define VHAVOKRIGIDBODY_VERSION_10       10    // Welding type for mesh rigid bodies
#define VHAVOKRIGIDBODY_VERSION_11       11    // Shape shrinker support on convex shapes
#define VHAVOKRIGIDBODY_VERSION_CURRENT  11    // Current version

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
typedef hkMassProperties hkpMassProperties;
#endif

///
/// \brief
///   Enumeration to define which shape type is used for the rigid body.
///
enum ShapeType_e
{
  ShapeType_BOX = 0,          ///< Simple box shape
  ShapeType_SPHERE = 1,       ///< Simple sphere shape
  ShapeType_CONVEX = 2,       ///< Convex shape
  ShapeType_FILE = 3,         ///< Collision geometry is loaded from external file
  ShapeType_CAPSULE = 4,      ///< Simple capsule shape
  ShapeType_CYLINDER = 5,     ///< Simple cylinder shape
  ShapeType_MESH = 6          ///< Polygon Mesh
};


///
/// \brief
///   Enumeration to define which motion type is used for the rigid body.
///
enum MotionType_e
{
  MotionType_DYNAMIC = 0,          ///< Driven by Havok Physics Simulation
  MotionType_KEYFRAMED = 1,        ///< Can be moved manually by code
  MotionType_FIXED = 2,            ///< Collides but doesn't move
  MotionType_SPHERE_INERTIA = 3,   ///< Driven by Havok Physics Simulation
  MotionType_BOX_INERTIA = 4,      ///< Driven by Havok Physics Simulation
  MotionType_THIN_BOX_INERTIA = 5, ///< Driven by Havok Physics Simulation
  MotionType_CHARACTER = 6         ///< Driven by Havok Physics Simulation
};

///
/// \brief
///   Enumeration to define the quality type for collidables.
///   This is mainly used to identify objects, which require the more expensive continuous simulation (compared to the faster, Havok2 style discrete) simulation.
///
enum QualityType_e
{
  QualityType_AUTO = 0,               ///< Automatic assignment
  QualityType_FIXED = 1,              ///< Use this for fixed bodies.
  QualityType_KEYFRAMED = 2,          ///< Use this for moving objects with infinite mass.
  QualityType_DEBRIS = 3,             ///< Use this for all your debris objects.
  QualityType_DEBRIS_SIMPLE_TOI = 4,  ///< Use this for debris objects that should have simplified Toi collisions with fixed/landscape objects.
  QualityType_MOVING = 5,             ///< Use this for moving bodies, which should not leave the world, but you rather prefer those objects to tunnel through the world than dropping frames because the engine.
  QualityType_CRITICAL = 6,           ///< Use this for all objects, which you cannot afford to tunnel through the world at all.
  QualityType_BULLET = 7,             ///< Use this for very fast objects.
  QualityType_CHARACTER = 8,          ///< Use this for rigid body character controllers.
  QualityType_KEYFRAMED_REPORTING = 9 ///< Use this for moving objects with infinite mass which should report contact points and Toi-collisions against all other bodies, including other fixed and keyframed bodies.   
};

/// \brief
///   Modes for inertia tensor computation
enum InertiaTensorComputeMode_e
{
  InertiaTensorComputeMode_NONE = 0,    ///< Don't compute, use default
  InertiaTensorComputeMode_SURFACE = 1, ///< Compute the inertia tensor of a surface with the given shape and thickness
  InertiaTensorComputeMode_VOLUME = 2,  ///< Compute the inertia tensor of a volume with the given shape
};

/// \brief
///   Parameters needed to create a Havok box shape
struct HkBoxGeometry
{
  hkVector4 m_HalfExtents; ///< Extents in each direction from the center
};

/// \brief
///   Parameters needed to create a Havok sphere shape
struct HkSphereGeometry
{
  hkReal m_Radius; ///< Radius of the sphere
};

/// \brief
///   Parameters needed to create a Havok capsule shape
struct HkCapsuleGeometry
{
  hkVector4 m_StartAxis; ///< Start point of the axis
  hkVector4 m_EndAxis; ///< End point of the axis
  hkReal m_Radius; ///< Radius of the capsule
};

/// \brief
///   Parameters needed to create a Havok cylinder shape
struct HkCylinderGeometry
{
  hkVector4 m_StartAxis; ///< Start point of the axis
  hkVector4 m_EndAxis; ///< End point of the axis
  hkReal m_Radius; ///< Radius of the cylinder
};


/// 
/// \brief
///   This class represents a rigid body object with a configurable shape. It is 
///   a component that can be attached to objects of class VisObject3D_cl.
/// 
/// The vHavokRigidBody object is the Vision representation of the hkpRigidBody 
/// in Havok that are the basic building blocks of Havok simulations. It contains
/// both dynamics (rigid body) and collision (shape) information.
///
class vHavokRigidBody : public IVObjectComponent
{
public:

  ///
  /// \brief
  ///   Class for storing common physics properties used for creating physics objects.
  ///
  class InitTemplate
  {
  public:

    InitTemplate()
    {
      m_motionType = hkpMotion::MOTION_DYNAMIC;
      m_qualityType = HK_COLLIDABLE_QUALITY_MOVING;
      m_restitution = -1;
    }
    
    hkpMotion::MotionType m_motionType;       ///< Important! By default set to MOTION_DYNAMIC.
    hkpCollidableQualityType m_qualityType;   ///< Important! By default set to HK_COLLIDABLE_QUALITY_MOVING.
    float m_restitution;                      ///< Set to -1 for Havok default.
  };


  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// 
  /// \brief
  ///   Constructor for the Havok Rigid Body Component.
  /// 
  /// \param iComponentFlags
  ///   Component bitflags that define the behavior of the component.
  /// 
  /// \see
  ///   IVObjectComponent::IVObjectComponent
  /// 
  VHAVOK_IMPEXP vHavokRigidBody(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);
  

  /// 
  /// \brief
  ///   Destructor of the Havok Rigid Body Component. Deletes the Havok rigid body if still alive.
  /// 
  VHAVOK_IMPEXP virtual ~vHavokRigidBody();


  ///
  /// \brief
  ///   Removes the rigid body from the simulation without necessarily deleting this instance.
  ///
  VHAVOK_IMPEXP VOVERRIDE void DisposeObject();


  ///
  /// @}
  ///


  ///
  /// @name Shape Configuration
  /// @{
  ///


  ///
  /// \brief
  ///   Init a rigid body that uses a box as shape.
  ///
  /// \param vBoxSize
  ///   Size of the box shape.
  ///
  /// \param vScale
  ///   Vector that defines the scaling of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitBoxRb(const hkvVec3& vBoxSize, const hkvVec3& vScale, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a box as shape generated from the model's bounding box.
  ///
  /// \param pMesh
  ///   Pointer to mesh data.
  ///
  /// \param vScale
  ///   Vector that defines the scaling of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitBoxRb(const VDynamicMesh *pMesh, const hkvVec3& vScale, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a box as shape.
  ///
  /// \param geometry
  ///   Havok geometry information needed to create the box.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitBoxRb(HkBoxGeometry const& geometry, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a sphere as shape.
  ///
  /// \param fRadius
  ///   Radius of the sphere shape.
  ///
  /// \param fScale
  ///   Uniform scaling value of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitSphereRb(float fRadius, float fScale, vHavokRigidBody::InitTemplate &initTempl);
  

  ///
  /// \brief
  ///   Init a rigid body that uses a sphere as shape generated from the model's bounding box.
  ///
  /// \param pMesh
  ///   Pointer to mesh data.
  ///
  /// \param fScale
  ///   Uniform scaling value of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitSphereRb(const VDynamicMesh *pMesh, float fScale, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a sphere as shape.
  ///
  /// \param geometry
  ///   Havok geometry information needed to create the sphere
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitSphereRb(HkSphereGeometry const& geometry, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a capsule as shape.
  ///
  /// \param vVertexA
  ///   Position of the lower end of the capsule shape.
  ///
  /// \param vVertexB
  ///   Position of the upper end of the capsule shape.
  ///
  /// \param fRadius
  ///   Radius of the capsule shape.
  ///
  /// \param fScale
  ///   Uniform scaling value of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitCapsuleRb(const hkvVec3& vVertexA, const hkvVec3& vVertexB, float fRadius, float fScale, vHavokRigidBody::InitTemplate &initTempl);

  ///
  /// \brief
  ///   Init a rigid body that uses a capsule as shape generated from the model's bounding box.
  ///
  /// \param pMesh
  ///   Pointer to mesh data.
  ///
  /// \param fScale
  ///   Uniform scaling value of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitCapsuleRb(const VDynamicMesh *pMesh, float fScale, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a capsule as shape.
  ///
  /// \param geometry
  ///   Havok geometry information needed to create the capsule
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitCapsuleRb(HkCapsuleGeometry const& geometry, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a cylinder as shape.
  ///
  /// \param vVertexA
  ///   Position of the lower end of the cylinder shape.
  ///
  /// \param vVertexB
  ///   Position of the upper end of the cylinder shape.
  ///
  /// \param fRadius
  ///   Radius of the cylinder shape.
  ///
  /// \param fScale
  ///   Uniform scaling value of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitCylinderRb(const hkvVec3& vVertexA, const hkvVec3& vVertexB, float fRadius, float fScale, vHavokRigidBody::InitTemplate &initTempl);

  ///
  /// \brief
  ///   Init a rigid body that uses a cylinder as shape generated from the model's bounding box.
  ///
  /// \param pMesh
  ///   Pointer to mesh data.
  ///
  /// \param fScale
  ///   Uniform scaling value of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitCylinderRb(const VDynamicMesh *pMesh, float fScale, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a cylinder as shape.
  ///
  /// \param geometry
  ///   Havok geometry information needed to create the cylinder
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitCylinderRb(HkCylinderGeometry const& geometry, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a convex hull as shape.
  ///
  /// \param pMesh
  ///   Pointer to mesh data.
  ///
  /// \param vScale
  ///   Vector that defines the scaling of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitConvexRb(VDynamicMesh *pMesh, const hkvVec3& vScale, vHavokRigidBody::InitTemplate &initTempl);

  ///
  /// \brief
  ///   Init a rigid body that uses the model's mesh as shape.
  ///
  /// \param pMesh
  ///   Pointer to mesh data.
  ///
  /// \param vScale
  ///   Vector that defines the scaling of this rigid body.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP void InitMeshRb(VDynamicMesh* pMesh, const hkvVec3& vScale, vHavokRigidBody::InitTemplate &initTempl);


  ///
  /// \brief
  ///   Init a rigid body that uses a shape defined in a Havok file.
  ///
  /// Returns true if a rigid body was successfully assigned.
  ///
  /// \param filename
  ///   Filename of the Havok file.
  ///
  /// \param takeRbDataFromFile
  ///   If this is TRUE, the mass etc. of the file based RB will be used along with the shape.
  ///   If this is FALSE, just the shape will be used
  ///
  /// \param scale
  ///   Uniform scaling factor.
  ///
  /// \param initTempl
  ///   Template that contains the physics properties of this rigid body.
  ///
  VHAVOK_IMPEXP bool InitRbFromFile(const char* filename, BOOL takeRbDataFromFile = true, float scale = 1.0f, vHavokRigidBody::InitTemplate *initTempl = NULL);


  ///
  /// \brief
  ///   Initializes a custom rigid body with the given construction info.
  ///
  /// \param cInfo
  ///   Construction info for this rigid body.
  ///
  /// \param massProperties
  ///   Mass properties (inertia tensor etc.) for this rigid body. Will 
  ///   only be considered if an inertia tensor was computed by Havok.
  ///
  VHAVOK_IMPEXP void InitCustomRb(hkpRigidBodyCinfo &cInfo, 
    hkpMassProperties const& massProperties);


  ///
  /// @}
  ///


  ///
  /// @name IVObjectComponent Virtual Overrides
  /// @{
  ///


  ///
  /// \brief
  ///   Overridden function to respond to owner changes.
  ///
  /// By setting the owner of this component the HavokRigidBody object will be 
  /// added to the Havok World and gets registered in the Havok module. 
  ///
  /// \param pOwner
  ///   The owner of this component.
  /// 
  /// \remarks
  ///   SetOwner(NULL) removes the HavokRigidBody object from the Havok World and
  ///   gets unregistered in the Havok module.
  /// 
  VHAVOK_IMPEXP VOVERRIDE void SetOwner(VisTypedEngineObject_cl *pOwner);


  ///
  /// \brief
  ///   Overridden function to determine if this component can be attached to a given object.
  ///
  /// The vHavokRigidBody component can be attached to VisObject3D_cl instances.
  /// 
  /// \param pObject
  ///   Possible owner candidate.
  /// 
  /// \param sErrorMsgOut
  ///   Reference to error message string.
  /// 
  /// \returns
  ///   TRUE if this component can be attached to the given object, FALSE otherwise.
  /// 
  VHAVOK_IMPEXP VOVERRIDE BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);
  

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
  VHAVOK_IMPEXP VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);


  ///
  /// \brief
  ///   Overridden function to process incoming messages, such as collision events and property changes
  ///
  VHAVOK_IMPEXP VOVERRIDE void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);


  ///
  /// @}
  ///

  
  ///
  /// @name VTypedObject Virtual Overrides
  /// @{
  ///


  /// 
  /// \brief
  ///   Overridable that is called by the editor to retrieve per-instance variable display hints. 
  ///
  /// \param pVariable
  ///   Variable to retrieve dynamic display hints for.
  ///
  /// \param destInfo
  ///   Structure that can be modified to affect the editor's displaying 
  ///   of the respective variable (read-only, hidden).
  ///
  #ifdef WIN32
  VHAVOK_IMPEXP virtual void GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo) HKV_OVERRIDE;
  #endif


  ///
  /// @}
  ///


  /// @name Serialization
  //@{

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP( vHavokRigidBody, VHAVOK_IMPEXP)


  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(vHavokRigidBody, VHAVOK_IMPEXP)


  /// \brief
  ///   Serialization function
  /// \param ar
  ///   Binary archive
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
  
  /// \brief
  ///   Overridden function to finalize the Havok rigid body once the deserialization is finished
  VHAVOK_IMPEXP VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);

  /// \brief
  ///   Overridden function to indicate that we need a deserialization callback
  VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context) {return context.m_eType!=VSerializationContext::VSERIALIZATION_EDITOR;}

  //@}

  ///
  /// @name Synchronization
  /// @{
  ///

  ///
  /// \brief
  ///   Step the rigid body (note this locks the world, so should only do this outside of physics step).
  ///
  /// Will not update the owner, as otherwise this could not be executed in the background.
  /// Update (method for updating the owner) will be called by the Physics Module in the main thread 
  /// of physics handling.
  ///
  /// \param dt
  ///   Time delta of simulation step.
  ///
  VHAVOK_IMPEXP VOVERRIDE void Step(float dt);

  VHAVOK_IMPEXP void UpdateOwner();

  ///
  /// \brief
  ///   Gets the transformation of the rigid body from Havok and sets them on the Vision instances.
  ///
  /// The physics module takes care of updating the transformation during the physics simulation. You
  /// will typically not have to call this function manually.
  ///
  VHAVOK_IMPEXP void UpdateHavok2Vision();
  
  
  ///
  /// \brief
  ///   Gets the transformation of the owner object from Vision and sets them on the Havok instance.
  ///
  /// The physics module takes care of updating the transformation during the physics simulation. You
  /// will typically not have to call this function manually.
  ///
  VHAVOK_IMPEXP void UpdateVision2Havok();

  ///
  /// @}
  ///


  ///
  /// @name Velocity & Property Wrapper Functions
  /// @{
  ///

  /// \brief
  ///   Sets the linear damping of this rigid body
  /// \param fDamping
  ///   the new linear damping
  VHAVOK_IMPEXP void SetLinearDamping(float fDamping);

  /// \brief
  ///   Gets the linear damping of this rigid body
  /// \return
  ///   the linear damping
  VHAVOK_IMPEXP hkReal GetLinearDamping() const;

  /// \brief
  ///   Sets the damage multiplier of this rigid body
  /// \param fMultiplier
  ///   the new damage multiplier
  VHAVOK_IMPEXP void SetDamageMultiplier(float fMultiplier);

  /// \brief
  ///   Gets the damage multiplier of this rigid body
  /// \return
  ///   the damage multiplier
  VHAVOK_IMPEXP hkReal GetDamageMultiplier() const;

  /// \brief
  ///   Sets the angular damping of this rigid body
  /// \param fDamping
  ///   the new angular damping
  VHAVOK_IMPEXP void SetAngularDamping(float fDamping);

  /// \brief
  ///   Gets the angular damping of this rigid body
  /// \return
  ///   the angular damping
  VHAVOK_IMPEXP hkReal GetAngularDamping() const;

  ///
  /// \brief
  ///   Set the position of this rigid body, in world space.
  ///
  /// \param value
  ///   Position vector.
  ///
  VHAVOK_IMPEXP void SetPosition(const hkvVec3& value);

  ///
  /// \brief
  ///   Get the world position of this rigid body.
  ///
  /// \return
  ///   Position vector.
  ///
  VHAVOK_IMPEXP hkvVec3 GetPosition() const;

  ///
  /// \brief
  ///   Set the orientation of this rigid body, in world space.
  ///
  /// \param value
  ///  orientation as euler angles.
  ///
  VHAVOK_IMPEXP void SetOrientation(const hkvVec3& value);


  ///
  /// \brief
  ///   Set the movement velocity of this rigid body.
  ///
  /// \param value
  ///   Linear velocity vector.
  ///
  VHAVOK_IMPEXP void SetLinearVelocity(const hkvVec3& value);


  ///
  /// \brief
  ///   Get the movement velocity of this rigid body.
  ///
  /// \return
  ///   Linear velocity vector.
  ///
  VHAVOK_IMPEXP hkvVec3 GetLinearVelocity() const;


  ///
  /// \brief
  ///   Set the rotation velocity of this rigid body.
  ///
  /// \param value
  ///   Angular velocity vector.
  ///
  VHAVOK_IMPEXP void SetAngularVelocity(const hkvVec3& value);
  

  ///
  /// \brief
  ///   Get the rotation velocity of this rigid body.
  ///
  /// \return
  ///   Angular velocity vector.
  ///
  VHAVOK_IMPEXP hkvVec3 GetAngularVelocity() const;


  /// \brief
  ///   Apply a force to this rigid body for a given time interval.
  ///
  /// \param value
  ///   Force vector.
  ///
  /// \param deltaT
  ///   The time interval over which the force is applied.
  ///
  /// To instantaneously accelerate a rigid body upon a single event (e.g. when struck by an explosion),
  /// it is recommended to use ApplyLinearImpulse. To apply a continuous force to a rigid body (e.g. a changing
  /// gravity which is evaluated every frame), it is recommended to use ApplyForce and pass the time interval
  /// between each application for the deltaT parameter.
  VHAVOK_IMPEXP void ApplyForce(const hkvVec3& value, float deltaT);


  ///
  /// \brief
  ///   Apply a linear impulse to this rigid body.
  ///
  /// \param value
  ///   Impulse vector.
  ///
  /// To instantaneously accelerate a rigid body upon a single event (e.g. when struck by an explosion),
  /// it is recommended to use ApplyLinearImpulse. To apply a continuous force to a rigid body (e.g. a changing
  /// gravity which is evaluated every frame), it is recommended to use ApplyForce and pass the time interval
  /// between each application for the deltaT parameter.
  VHAVOK_IMPEXP void ApplyLinearImpulse(const hkvVec3& value);


  ///
  /// \brief
  ///   Set the mass of this rigid body.
  ///
  /// \param fMass
  ///   Mass specified in kilo.
  ///
  VHAVOK_IMPEXP void SetMass(float fMass);


  /// \brief
  ///   Sets the motion type of this rigid body.
  /// \param type
  ///   the new motion type
  VHAVOK_IMPEXP void SetMotionType(hkpMotion::MotionType type);

  /// \brief
  ///   Sets the quality type of this rigid body.
  /// \param type
  ///   the new quality type
  VHAVOK_IMPEXP void SetQualityType(hkpCollidableQualityType type);

  /// \brief
  ///   Sets the restitution of this rigid body.
  /// \param fRestitution
  ///   the new restitution; should be between 0.0 and 1.0
  VHAVOK_IMPEXP void SetRestitution(float fRestitution);

  /// \brief
  ///   Sets the friction of this rigid body.
  /// \param fFriction
  ///   the new friction; should be between 0.0 and 1.0
  VHAVOK_IMPEXP void SetFriction(float fFriction);

  /// \brief
  ///   Sets the collision parameters of this rigid body.
  /// \param iLayer
  ///   the collision layer
  /// \param iGroup
  ///   the collision group
  /// \param iSubsystem
  ///   the collision subsystem
  /// \param iSubsystemDontCollideWith
  ///   the collision subsystem this body shouldn't collide with
  /// \note
  ///   See the Havok documentation on rigid body collisions for more 
  ///   information about what values to specify for these parameters.
  VHAVOK_IMPEXP void SetCollisionInfo(int iLayer, int iGroup,
    int iSubsystem, int iSubsystemDontCollideWith);

  /// \brief
  ///   Specifies whether the rigid body should be initialized as dynamic and set to the configured
  ///   motion type after initialization only.
  ///
  /// The rigid body object in Havok has an optimization regarding keyframed and fixed motion types. If
  /// a rigid body gets created with the motion type keyframed or fixed then such a rigid body can not be
  /// changed to the motion type dynamic anymore later on.
  ///
  /// If you set this InitAsDynamic property to TRUE then the vHavokRigidBody class will always create the
  /// native rigid body with a dynamic motion type, and switch it to the configured motion type afterwards.
  /// This means that the motion type gets correctly set, but you can still switch to a dynamic motion type
  /// later on without problems.
  ///
  /// You can e.g. use this toggle if you serialized rigid bodies that may switch between keyframed/fixed/dynamic
  /// motion type inside the game. [3950]
  ///
  /// \param bStatus
  ///   If true then the rigid bodies gets temporarily initialized with dynamic motion type
  VHAVOK_IMPEXP void SetAlwaysInitAsDynamic(bool bStatus);

  /// \brief
  ///   Sets the activation status of this object.
  ///
  /// When the status is set to false, the rigid body is removed from the Havok World so that the collision of this
  /// object is ignored. When set to true, the object is added to the Havok World again.
  ///
  /// \param bStatus
  ///   Whether the rigid body should be activated or not
  VHAVOK_IMPEXP void SetActive(bool bStatus);

  /// \brief
  ///   Gets the activation status of this object.
  ///
  /// \return
  ///   Whether the rigid body is active or not
  VHAVOK_IMPEXP bool GetActive() const;

  ///
  /// @}
  ///


  ///
  /// @name Access to Havok Internals
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the Havok internal rigid body instance (can be NULL if not yet initialized).
  ///
  /// \return
  ///   hkpRigidBody: Pointer to the Havok rigid body (or NULL if not yet initialized).
  ///
  inline hkpRigidBody *GetHkRigidBody() { return m_spRigidBody; }


  ///
  /// \brief
  ///   Gets the Havok shape that belongs to the rigid body instance (can be NULL if not yet initialized).
  ///  
  /// \return
  ///   hkpShape: Pointer to the shape of this rigid body (or NULL if not yet initialized).
  ///
  VHAVOK_IMPEXP const hkpShape *GetHkShape() const; 


  /// \brief
  ///   Given a hkpRigidBody, get the vHavokRigidBody associated with it (if any).
  /// \param pHkRigidBody
  ///   the havok rigid body for which to get the associated Vision object
  /// \return
  ///   a pointer to the associated vHavokRigidBody, or NULL, if no such association exists.
  VHAVOK_IMPEXP static vHavokRigidBody *FromHkRigidBody(const hkpRigidBody *pHkRigidBody); 

  ///
  /// @}
  ///


  ///
  /// @name Debug Rendering
  /// @{
  ///


  ///
  /// \brief
  ///   Enable / Disable debug rendering for this rigid body's shape.
  ///
  /// \param bEnable
  ///   If TRUE, the shape of this rigid body will be displayed in Vision.
  ///
  VHAVOK_IMPEXP void SetDebugRendering(BOOL bEnable);


  ///
  /// \brief
  ///   Set the color of the debug rendering representation of this rigid body's shape.
  ///
  /// \param color
  ///   New Color of the debug rendering representation.
  ///
  VHAVOK_IMPEXP void SetDebugColor(VColorRef color);


  ///
  /// @}
  ///

protected:

  ///
  /// \brief
  ///   Initializes the rigid body component after it has been added to an owner.
  ///
  void CommonInit();


  ///
  /// \brief
  ///   Deinitialisation that is used both on DisposeObject and on destruction.
  ///
  void CommonDeinit();

  ///
  /// \brief
  ///   Adjust the vertexes that delimits the height of the cylinder part in a capsule shape
  /// in a way the two semi-spheres that delimits the capsule on the top and bottom extremes
  /// fits, or at least fits as much as possible, inside the cylinder part of the capsule.
  ///
  /// \param[in,out]  vA One of the vertex that delimits one of the extremes of the cylinder's part of the capsule.
  /// \param[in,out]  vB The other vertex that delimits one of the extremes of the cylinder's part of the capsule.
  /// \param[in]      fSphereRadius Radius of the two semi-spheres used in the extremes of the capsule.
  ///
  static void AdjustVertexesForCapsuleSemispheres(hkvVec3& vA, hkvVec3& vB, float fSphereRadius);

  ///
  /// \brief
  ///   Creates the actual Havok rigid body and adds it to the simulation.
  ///
  /// \param cInfo
  ///   Construction info for this rigid body.
  ///
  /// \param massProperties
  ///   Mass properties (inertia tensor etc.) for this rigid body. Will 
  ///   only be considered if an inertia tensor was computed by Havok.
  ///
  /// \return
  ///   bool: TRUE if rigid body could be successfully created, FALSE otherwise.
  ///
  bool CreateHkRigidBody(hkpRigidBodyCinfo &cInfo, hkpMassProperties const& massProperties);


  ///
  /// \brief
  ///   Removes object from the simulation and releases the rigid body reference.
  ///
  void RemoveHkRigidBody();


  ///
  /// \brief
  ///   Fills the given construction info with the physics properties specified in the initial template.
  ///
  /// \param cInfo
  ///   Reference to construction info of this rigid body to be filled.
  ///
  /// \param initTempl
  ///   Reference to template that contains the physics properties of this rigid body.
  ///
  void FillConstructionInfo(hkpRigidBodyCinfo &cInfo, const vHavokRigidBody::InitTemplate &initTempl);

  /// 
  /// \brief
  ///   Determines whether specified resource is used by the vHavokRigidBody component.
  bool IsResourceRelevant(const VManagedResource* pResource);
  
public:  
  /// \brief
  ///   Gets the owner of this rigid body as a VisObject3d_cl.
  /// \return
  ///   the owner object of this rigid body.
  VHAVOK_IMPEXP VisObject3D_cl *GetOwner3D();

  /// \brief
  ///   Gets the owner of this rigid body as a VisBaseEntity_cl.
  /// \return
  ///   the owner entity of this rigid body if the owner is an entity; 
  ///   \c null otherwise
  VHAVOK_IMPEXP VisBaseEntity_cl *GetOwnerEntity();


  //
  // Members not exposed to vForge:
  //
protected:
  hkRefPtr<hkpRigidBody> m_spRigidBody; ///< The actual Havok Rigid Body
  vHavokPhysicsModule *m_pModule;       ///< Reference to Havok Physics Module
  hkvVec3 m_vCenterOfMassOffset;        ///< Offset which compensates for collision bbox pivot not being equal to model pivot
  bool m_bAddedToWorld;                 ///< Whether the Rigid Body is currently added to Havok World
  const char *m_szShapeCacheId;         ///< ID of shape of the rigid body in runtime cache table (points to memory in cache table).

public:
  //
  // Members exposed to vForge:
  //
  int Shape_Type;                       ///< Shape type of Havok Rigid Body
  float Shape_Radius;                   ///< Radius for Sphere/Capsule/Cylinder Shape Types.
  hkvVec3 Shape_BoxSize;                ///< Size of box shape (affects only box shapes)
  float Shape_Height;                   ///< Height of the Capsule/Cylinder. The Capsule/Cylinder is fully defined with this parameter and Shape_Radius.
   
  hkvVec3 Shape_PivotOffset;            ///< Initial Offset between Havok shape and its owner object
  hkvVec3 Shape_CenterOfMass;           ///< User defined mass center of this RB in local space. Final center of mass might additional take model pivot into account (see m_vCenterOfMassOffset)
  BOOL Shape_PivotOffsetAutoAlign;      ///< Automatic alignment of the pivot offset

  BOOL Havok_NoDeactivate;              ///< Whether to keep this rigid body always active

  float Havok_Mass;                     ///< The mass of this RB in kilograms
  int Havok_InertiaTensorComputeMode;   ///< Inertia tensor computation mode
  float Havok_SurfaceThickness;         ///< The surface thickness for surface inertia tensor computation
  hkvVec3 Havok_InertiaTensorAxisScaling; ///< Factors to scale the inertia tensor's axes with
  float Havok_InertiaTensorScaling;     ///< Factor to scale all components of the inertia tensor with

  float Havok_Friction;                 ///< Defines the smoothness of the surface of this RB
  float Havok_Restitution;              ///< Defines the bounciness of this RB
  float Havok_LinearDamping;            ///< Defines how strong the movement of this RB is reduced over time
  float Havok_AngularDamping;           ///< Defines how strong the rotation of this RB is reduced over time

  int Havok_CollisionLayer;             ///< Defines the collision layer this RB is assigned to.
  int Havok_CollisionGroup;             ///< Defines the collision group this RB is assigned to.
  int Havok_SubSystemId;                ///< Defines the sub system ID of this RB.
  int Havok_SubSystemDontCollideWith;   ///< Defines the sub system ID this RB should not collide with.

  BOOL Debug_Render;                    ///< Whether debug rendering is active or not
  VColorRef Debug_Color;                ///< Debug color of the physical RB representation

  VString Havok_FileResourceName;       ///< If ShapeType is ShapeType_FILE, then this is the filename to load from
  BOOL Havok_TakeRbDataFromFile;        ///< If ShapeType is ShapeType_FILE, then this indicates whether it is the whole RB (mass, etc) or just ythe Shape that is taken from file.
  int Havok_MotionType;                 ///< Type of Physics: Dynamic, Fixed or Keyframed
  int Havok_QualityType;                ///< Used to specify when to use continuous physics. The default is -Moving-
  int Havok_WeldingType;                ///< Used to specify welding type for mesh rigid bodies

  float Havok_MaxKinematicVelocity;     ///< If the current velocity is below this value a kinematic object is moved by velocity otherwise the object will be repositioned 

  BOOL Havok_AlwaysInitAsDynamic;       ///< Specifies if the rigid body should always be created as dynamic. Only dynamic initialized objects can be changed back from a fixed/keyframed state to become dynamic again (e.G. after loading a savegame)
  BOOL Havok_Active;                    ///< Whether the rigid body is active or not. The object is removed from the Havok World on deactivation so its collision is ignored then.
  BOOL Havok_TightFit;                  ///< Whether to shrink the shape by cvx radius or not (so that graphics aligns with physics in auto gen cases better)

  hkvVec3 m_vTempDeserializedLinearVel;   ///< Deserialized linear velocity. Temporarily stored until deserialization is finished and rigid body is available.
  hkvVec3 m_vTempDeserializedAngularVel;  ///< Deserialized angular velocity. Temporarily stored until deserialization is finished and rigid body is available.
};

#endif // VHAVOKRIGIDBODY_HPP_INCLUDED

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
