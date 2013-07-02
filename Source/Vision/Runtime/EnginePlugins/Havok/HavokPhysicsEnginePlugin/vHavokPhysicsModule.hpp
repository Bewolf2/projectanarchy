/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokPhysicsModule.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Havok. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKPHYSICSMODULE_HPP_INCLUDED
#define VHAVOKPHYSICSMODULE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStaticMesh.hpp>
#ifdef SUPPORTS_TERRAIN
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokTerrain.hpp>
#endif
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokTriggerVolume.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokVisualDebugger.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokDisplayHandler.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokContactListener.hpp>

// Reduce this to 0 if you are not using the VDB stats view etc
#define HKVIS_MONITOR_STREAM_BYTES_PER_THREAD 1024*1024

/// Serialization versions of global world params
#define VHAVOKWORLDPARAMS_VERSION_0        1 // Initial version (8.2, not supported anymore, reexport of scene required)
#define VHAVOKWORLDPARAMS_VERSION_1        2 // Added StaticMesh-Mode, CollisionLayer-Setup (>=8.3) 
#define VHAVOKWORLDPARAMS_VERSION_2        3 // Added Disable Constrained bodies collisions 
#define VHAVOKWORLDPARAMS_VERSION_3        4 // Added Enable Legacy Compound Shapes
#define VHAVOKWORLDPARAMS_VERSION_4        5 // Added native serialization of Static Mesh Caching
#define VHAVOKWORLDPARAMS_VERSION_5        6 // This is a dead version introduced in Restructuring -> 2013.1 merge, should never be referenced
#define VHAVOKWORLDPARAMS_VERSION_6        7 // Added welding type for merged static meshes
#define VHAVOKWORLDPARAMS_VERSION_7        8 // Added solver iterations and hardness
#define VHAVOKWORLDPARAMS_VERSION_CURRENT  VHAVOKWORLDPARAMS_VERSION_7


const unsigned int VIS_MSG_HAVOK_ONCOLLISION   = VIS_MSG_PHYSICS,   ///< iParamA = Pointer to vHavokCollisionInfo_t, iParamB = sizeof(vHavokCollisionInfo_t)
                   VIS_MSG_HAVOK_ONJOINTCREATE = VIS_MSG_PHYSICS+1, ///< iParamA = pointer to first linked entity, iParamB = pointer to second linked entity
                   VIS_MSG_HAVOK_ONJOINTBREAK  = VIS_MSG_PHYSICS+2; ///< iParamA = pointer to vHavokConstraintBreakInfo structure, iParamB = pointer to linked Vision entity

VHAVOK_IMPEXP const char* vHavokPhysicsModule_GetVersionString();

/// profiling IDs
static int PROFILING_HAVOK_PHYSICS_SIMULATION;
static int PROFILING_HAVOK_PHYSICS_FETCH_RESULTS;

/// \brief
///   World setup settings. If you change these parameters you will have to reload the scene in vForge to apply the changes.  
struct vHavokPhysicsModuleDefaultSetupWorldParams 
{
  float m_fHavokToVisionScale; 
  int m_iStaticGeomMode; // Specifies how the static geometry is represented/ optimized within Havok (vHavokPhysicsModule::SGM_SINGLE_INSTANCES or vHavokPhysicsModule::SGM_MERGED_INSTANCES)
  int m_iMergedStaticWeldingType; // Specifies the welding type for merged static mesh instances (VisWeldingType_e)
};

// Any edit to these will apply next scene load
VHAVOK_IMPEXP vHavokPhysicsModuleDefaultSetupWorldParams& vHavokPhysicsModule_GetWorldSetupSettings();

/// \brief
///   World runtime settings. Most of these have Set/Get methods on the module for runtime use, this struct is to set them pre world create.
struct vHavokPhysicsModuleDefaultRuntimeWorldParams
{
  BOOL m_bBroadphaseAutoCompute;
  float m_fBroadphaseManualSize; // In Vision Units
  hkvVec3 m_vGravity; // in Vision units
  BOOL m_bEnableShapeCaching;
  BOOL m_bEnableConstraintCollisionFilter;
  BOOL m_bEnableLegacyCompoundShapes;		// Affects Ps3 builds. If true, it will load the EMS / CMS collide elf instead of the one using static compound shape.
  unsigned int m_iCollisionGroupMasks[32];	// Please note: after changing the collision group masks, UpdateGroupsCollision() has to be called on vHavokPhysicsModule 

  int m_solverIterations; // Defaults to 4. [2,4,8] allowed
  int m_solverHardness; // Defaults to 1. [0,1,2] allowed, where 0 is soft, 1 medium, 2 is hard
};

// Any edit to these will apply next world create. If you have a vHavokPhysicsModule instance already
// then you can just set the desired parameter directly
VHAVOK_IMPEXP vHavokPhysicsModuleDefaultRuntimeWorldParams& vHavokPhysicsModule_GetDefaultWorldRuntimeSettings();

// Restore default world setup/ runtime settings
VHAVOK_IMPEXP void vHavokPhysicsModule_ResetWorldSettings();

class hkpWorld;
class hkThreadMemory;
struct hkpWorldRayCastOutput;
class vHavokCharacterController;
class vHavokRigidBody;
class vHavokStaticMesh;
class vHavokRagdoll;
class vHavokTerrain; // declare it even when it does not exist
class vHavokConstraint;
class vHavokConstraintChain;
class vHavokContactListener;
class hkJobQueue;
class hkSpuUtil;
class hkJobThreadPool;
class VHavokDecorationGroup;
class vHavokProfiler;

class vHavokConstraintListener;
class vHavokPhysicsModule;
class vHavokConstraint;
class vHavokConstraintChain;

struct vHavokSweepResult;
class vHavokPhysicsRaycaster;

typedef VSmartPtr<vHavokVisualDebugger> vHavokVisualDebuggerPtr;
typedef VSmartPtr<vHavokDisplayHandler> vHavokDisplayHandlerPtr;
typedef VSmartPtr<vHavokConstraintListener> vHavokConstraintListenerPtr;
typedef VSmartPtr<vHavokContactListener> vHavokContactListenerPtr;


//
// Important Note:
// All functions in the binding use the Vision world scale as defined by the user
// (typically 50 units = 1 meter).
// 
// All Havok objects (e.g. the hkRigidBody instances) are in the recommended Havok
// world space, which is 1 unit = 1 meter.
// 
// This means that you can use the Vision world scale when accessing methods on
// the vHavok* classes (the binding), but have to use downscaled values when 
// directly accessing Havok methods (e.g. on the hkRigidBody)
//


/// \brief
///   Task object for processing physics asynchronously (i.e. in parallel with rendering).
class VHavokTask : public VThreadedTask
{
  V_DECLARE_DYNCREATE_DLLEXP( VHavokTask,  VHAVOK_IMPEXP )

private:
  vHavokPhysicsModule *m_pModule;

public:
  inline VHavokTask() {}
  VHavokTask(vHavokPhysicsModule *pModule);

  VOVERRIDE void Run(VManagedThread *pThread);
};

/// \brief
///   Generic callback data object for callbacks triggered by a vHavok physics
//    module.
class vHavokPhysicsModuleCallbackData : public IVisCallbackDataObject_cl
{
public:
  VHAVOK_IMPEXP vHavokPhysicsModuleCallbackData(VisCallback_cl *pSender, 
    vHavokPhysicsModule *pHavokModule);

public:
  VHAVOK_IMPEXP vHavokPhysicsModule *GetHavokModule() const;

private:
  vHavokPhysicsModule *m_pHavokModule;
};


/// \brief
///   Derived callback data object class that is used for the OnBeforeWorldCreated callback function.
/// 
/// \sa vHavokPhysicsModule::OnBeforeWorldCreated
class vHavokBeforeWorldCreateDataObject_cl : public vHavokPhysicsModuleCallbackData
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance and a model pointer
  vHavokBeforeWorldCreateDataObject_cl(VisCallback_cl *pSender, 
    vHavokPhysicsModule *pHavokModule, hkpWorldCinfo &worldConfig) 
  : vHavokPhysicsModuleCallbackData(pSender, pHavokModule), m_worldConfig(worldConfig)
  {
  }

  hkpWorldCinfo &m_worldConfig;      ///< Havok world config structure. Can be modified by the listener.
};


/// \brief
///   A structure to describe a single sweep command.
///
/// \sa vHavokPhysicsModule::PerformSweep
/// \sa vHavokPhysicsModule::PerformSweepBatched
struct vHavokSweepCommand
{
public:
  /// Constructor
  vHavokSweepCommand() : m_pResults(NULL), m_iNumMaxResults(0), m_iNumResultsOut(-1), m_pRigidBody(NULL), m_pCharacterController(NULL), m_vDir(0.0f), m_fDistance(1.0f)
  {
  }

  /// A user-allocated buffer in which the results will be placed.
  vHavokSweepResult* m_pResults;

  /// The maximum number of results that can be placed into m_pResults.
  int m_iNumMaxResults;

  /// The number of results after the sweep has been completed.
  int m_iNumResultsOut;

  /// The rigid body to perform the sweep with. Mutually exclusive with m_pCharacterController.
  vHavokRigidBody* m_pRigidBody;

  /// The character controller to perform the sweep with. Mutually exclusive with m_pRigidBody.
  vHavokCharacterController* m_pCharacterController;
  
  /// The direction of the sweep.
  hkvVec3 m_vDir;

  /// The distance of the sweep.
  float m_fDistance;
};


/// 
/// \brief
///   Module responsible for the physics simulation and the conversion from  Vision => Havok => Vision.
/// 
/// Write detailed description for VHavokPhysicsModule here.
/// 
class vHavokPhysicsModule : public IVisPhysicsModule_cl, public IVisCallbackHandler_cl 
{
public:

  /// 
  /// \brief
  ///   Enumeration of static geometry modes (single or merged).
  ///
  enum StaticGeomMode
  {
    SGM_SINGLE_INSTANCES,       ///< Each static mesh is represented as one Havok rigid body (editiable, but slow)
    SGM_MERGED_INSTANCES        ///< All static meshes in the scene file are baked into one Havok rigid body (not editable, but fast)
  };

  ///
  /// \brief
  ///   Enumeration of collision layers to which Havok Collidables can be assigned. 
  ///   
  /// Please note: When changing this enum, keep it in sync with:
  ///   HavokCollisionGroups_e      in      Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokManaged.cpp
  ///   CollisionLayerNames         in      Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionEditorPlugin/Shapes/StaticMeshShape.cs
  ///   cmbPhyGroup                 in      ContentTools/Max/MaxSceneExport/Scripts/HavokVision/Vision_TaggingDialog.ms
  ///   colLayers                   in      ContentTools/Maya/MayaSceneExport/Scripts/Vision/editVisionNodeProperties.mel
  enum CollisionGroups
  {
    HK_LAYER_ALL = 0,
    HK_LAYER_COLLIDABLE_DYNAMIC = 1,
    HK_LAYER_COLLIDABLE_STATIC = 2,
    HK_LAYER_COLLIDABLE_TERRAIN = 3,
    HK_LAYER_COLLIDABLE_CONTROLLER = 4,
    HK_LAYER_COLLIDABLE_TERRAIN_HOLE = 5,
    HK_LAYER_COLLIDABLE_DISABLED = 6,
    HK_LAYER_COLLIDABLE_RAGDOLL = 7,
    HK_LAYER_COLLIDABLE_ATTACHMENTS = 8,
    HK_LAYER_COLLIDABLE_FOOT_IK = 9,
    HK_LAYER_COLLIDABLE_DEBRIS	= 11,
    HK_LAYER_COLLIDABLE_CUSTOM0 = 27,
    HK_LAYER_COLLIDABLE_CUSTOM1 = 28,
    HK_LAYER_COLLIDABLE_CUSTOM2 = 29,
    HK_LAYER_COLLIDABLE_CUSTOM3 = 30,
    HK_LAYER_COLLIDABLE_CUSTOM4 = 31
  };

  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// 
  /// \brief
  ///   Constructor of the Havok Physics Module.
  /// 
  vHavokPhysicsModule();


  /// 
  /// \brief
  ///   Destructor of the Havok Physics Module.
  /// 
  VHAVOK_IMPEXP virtual ~vHavokPhysicsModule(void);


  /// 
  /// \brief
  ///   Returns an instance of the Havok physics module
  ///
  /// The physics module is retrieved from the active Vision application.
  /// If a non Havok physics module is set, then NULL will be returned.
  /// 
  /// \returns
  ///   An instance of the havok physics module (or NULL if no Havok physics module installed).
  /// 
  VHAVOK_IMPEXP static vHavokPhysicsModule *GetInstance();


  ///
  /// @}
  ///


  ///
  /// @name IVisPhysicsModule_cl Virtual Overrides
  /// @{
  ///


  /// 
  /// \brief
  ///   Initializes the Havok physics module.
  /// 
  /// \returns
  ///   TRUE if initialization was successful, FALSE otherwise.
  /// 
  virtual BOOL OnInitPhysics();


  /// 
  /// \brief
  ///   Deinitializes the Havok physics module.
  /// 
  /// \returns
  ///   TRUE if deinitialisation was successful, FALSE otherwise.
  /// 
  virtual BOOL OnDeInitPhysics();
  

  /// 
  /// \brief
  ///   Called when a new world has been loaded (DEPRECATED).
  /// 
  virtual void OnNewWorldLoaded();


  /// 
  /// \brief
  ///   Run the physics simulation.
  /// 
  /// \param fDuration
  ///   Time the physics simulation should be processed.
  /// 
  virtual void OnRunPhysics( float fDuration );


  /// 
  /// \brief
  ///   Fetches new physics results after a call to OnRunPhysics. 
  /// 
  virtual void FetchPhysicsResults();


  /// 
  /// \brief
  ///   Returns the physics module type.
  /// 
  /// \returns
  ///   The physics module type.
  /// 
  virtual eMODULETYPE GetType() { return HAVOK; }

  
  /// 
  /// \brief
  ///   Creates a physics object (not implemented)
  /// 
  /// \param pObject3D
  ///   Owner object a physics object should be created for.
  /// 
  /// \param bStatic
  ///   If TRUE a static physics object will be created for its owner object.
  /// 
  /// \returns
  ///   IVisPhysicsObject_cl: Pointer to the created physics object.
  /// 
  virtual IVisPhysicsObject_cl *CreatePhysicsObject( VisObject3D_cl *pObject3D, bool bStatic = false );


  /// 
  /// \brief
  ///   Review whether physics module uses asynchronous physics.
  /// 
  /// \returns
  ///   bool: TRUE if physics module uses asynchronous physics, FALSE otherwise.
  /// 
  VHAVOK_IMPEXP virtual bool GetUseAsynchronousPhysics();

  /// \brief
  ///   Sets whether to uses asynchronous physics processing.
  /// \param bAsyncPhysics
  ///   whether to uses asynchronous physics processing
  VHAVOK_IMPEXP virtual void SetUseAsynchronousPhysics(bool bAsyncPhysics);

  /// 
  /// \brief
  ///   Immediately performs a raycast operation. 
  /// 
  /// \param pRaycastData
  ///   Raycast implementation defining the raycast behavior. This object is also used for storing the results.
  ///   Please note: use hkpGroupFilter::calcFilterInfo() to setup the iCollisionBitmask member 
  /// 
  VHAVOK_IMPEXP virtual void PerformRaycast(VisPhysicsRaycastBase_cl *pRaycastData);
  
  
  /// 
  /// \brief
  ///   Enqueues a raycast operation for asynchronous execution. 
  /// 
  /// \param pRaycastData
  ///   Raycast implementation defining the raycast behavior. This object is also used for storing the results.
  ///   Please note: use hkpGroupFilter::calcFilterInfo() to setup the iCollisionBitmask member 
  /// 
  VHAVOK_IMPEXP virtual void EnqueueRaycast(VisPhysicsRaycastBase_cl *pRaycastData);

  /// 
  /// \brief
  ///   Method called by the Vision engine whenever a new static mesh instance has been created.
  /// 
  /// \param pMeshInstance
  ///   Pointer to static mesh instance that has just been created.
  /// 
  virtual void OnStaticMeshInstanceCreated(VisStaticMeshInstance_cl *pMeshInstance);
  

  /// 
  /// \brief
  ///   Method called by the Vision engine whenever a static mesh instance has been removed. 
  /// 
  /// \param pMeshInstance
  ///   Pointer to static mesh instance that has just been removed.
  /// 
  virtual void OnStaticMeshInstanceRemoved(VisStaticMeshInstance_cl *pMeshInstance);
  

  /// 
  /// \brief
  ///   Method called by the Vision engine whenever a static mesh instance has moved. 
  ///   Typically only used by vForge.
  /// 
  /// \param pMeshInstance
  ///   Pointer to static mesh instance that has just been moved.
  /// 
  virtual void OnStaticMeshInstanceMoved(VisStaticMeshInstance_cl *pMeshInstance);


  /// 
  /// \brief
  ///   Method called by the Vision engine whenever the collision bitmask of a static 
  ///   mesh instance has changed. Typically only used by vForge. 
  /// 
  /// \param pMeshInstance
  ///   Pointer to static mesh instance the collision bitmask has just been changed.
  /// 
  virtual void OnStaticMeshInstanceCollisionBitmaskChanged(VisStaticMeshInstance_cl *pMeshInstance);

  /// 
  /// \brief
  ///   Returns the group collision filter set into the physics world.
  /// 
  virtual hkpGroupFilter* GetGroupCollisionFilter();
    
#ifdef SUPPORTS_TERRAIN

  ///
  /// \brief
  ///   Method called by the Vision engine when a new terrain sector has been created.
  ///
  virtual void OnTerrainSectorCreated(VTerrainSector *pTerrainSector);
  
  
  ///
  /// \brief
  ///   Method called by the Vision engine when a terrain sector has been removed.
  ///
  virtual void OnTerrainSectorRemoved(VTerrainSector *pTerrainSector);

  ///
  /// \brief
  ///   Method called by the Vision engine when a terrain sector has been saved.
  ///
  virtual void OnTerrainSectorSaved(VTerrainSector *pTerrainSector);

  ///
  /// \brief
  ///   Method called by the Vision engine when a new decoration object has been created.
  ///
  virtual void OnDecorationCreated(IVisDecorationGroup_cl &group);
 
    
  ///  
  /// \brief
  ///   Method called by the Vision engine when a decoration object has been removed.
  ///
  virtual void OnDecorationRemoved(IVisDecorationGroup_cl &group);

#endif

#ifdef SUPPORTS_SNAPSHOT_CREATION

  ///  
  /// \brief
  ///   Method called by the Vision engine to retrieve additional file dependencies for streaming for a instance.
  virtual void GetDependenciesForObject(VResourceSnapshot &snapshot, VTypedObject *pInstance);

  ///  
  /// \brief
  ///   Overridable called by the Vision engine to retrieve additional resource/file dependencies for streaming for a resource.
  virtual void GetDependenciesForResource(VResourceSnapshot &snapshot, VManagedResource *pResource);

#endif

  ///
  /// @}
  ///


  ///
  /// @name Sweep Testing
  /// @{
  ///

  /// \brief
  ///   Performs a linear sweep through space with the specified rigid body.
  ///
  /// \param pResults           Array of iNumResults elements to receive hit results.
  /// \param iNumResults        Number of hit results to get in array pResults. 
  /// \param pRigidBody         Rigid Body for which to perform sweep test. 
  /// \param vDir               Normalized motion vector for the sweep.
  /// \param fDistance          Distance along the vDir vector.
  /// 
  /// \return                   The number of hits found or -1 if the sweep test was not possible.
  VHAVOK_IMPEXP int PerformSweep(vHavokSweepResult *pResults, int iNumResults, vHavokRigidBody *pRigidBody, const hkvVec3& vDir, float fDistance);

  /// \brief
  ///   Performs a linear sweep through space with the specified character controller.
  ///
  /// \param pResults           Array of iNumResults elements to receive hit results.
  /// \param iNumResults        Number of hit results to get in array pResults. 
  /// \param pCharacterCtrl     Character Controller for which to perform sweep test. 
  /// \param vDir               Normalized motion vector for the sweep.
  /// \param fDistance          Distance along the vDir vector.
  /// 
  /// \return                   The number of hits found or -1 if the sweep test was not possible.
  VHAVOK_IMPEXP int PerformSweep(vHavokSweepResult *pResults, int iNumResults, vHavokCharacterController *pCharacterCtrl, const hkvVec3& vDir, float fDistance);

  /// \brief
  ///  Performs a number of multi-threaded linear sweeps through space with the given list of sweep commands.
  ///
  /// This function uses multi-threading, but is not thread-safe itself.
  ///
  /// \param pCommands          An array of sweep commands to be executed
  /// \param iNumCommands       Number of commands to be batched
  ///
  /// \sa vHavokSweepCommand
  VHAVOK_IMPEXP void PerformSweepBatched(vHavokSweepCommand* pCommands, int iNumCommands);

  /// \brief
  ///   Drops the specified rigid body to the floor by performing a linear sweep.
  /// 
  /// \param pRigidBody         Rigid body which should be dropped to floor.
  /// \param fDist              Test for a floor in fDist units below the specified rigid body.
  ///
  /// \return                   True on success, otherwise false.
  VHAVOK_IMPEXP bool DropToFloor(vHavokRigidBody *pRigidBody, float fDist=10000.0f);

  /// \brief
  ///   Drops the specified character controller to the floor by performing a linear sweep.
  /// 
  /// \param pCharacterCtrl     Character Controller which should be dropped to floor.
  /// \param fDist              Test for a floor in fDist units below the specified character controller.
  ///
  /// \return                   True on success, otherwise false.
  VHAVOK_IMPEXP bool DropToFloor(vHavokCharacterController *pCharacterCtrl, float fDist=10000.0f);

  ///
  /// @}
  ///


  ///
  /// @name Simulation 
  /// @{
  ///

  /// 
  /// \brief
  ///   Set the number of physics ticks per second
  /// 
  /// \param iTickCount
  ///   Value how often the physics is stepped in one second. 
  ///   Set to zero if you want to use variable time stepping, which makes the simulation indeterministic.
  /// 
  /// \param iMaxTicksPerFrame
  ///   Value that is used to clamp the number of ticks performed in one frame. For instance, if simulation is set to 60Hz and the game renders with 10fps
  ///   then 6 ticks would be performed. 6 ticks however can be too much and cause further slowdown (negative feedback). So it is important to clamp this value
  ///   which however results in physics running slower.
  /// 
  /// \param bFixedTicksPerFrame
  ///   If true, the value of \c iMaxTicksPerFrame is used as a fixed number of physics ticks
  ///   in each frame. This can be useful if the Vision frame rate is fixed, e.g., by some
  ///   frame-lock mechanism.
  ///   Ignored when variable time stepping is enabled.
  ///
  /// \param fMinPhysicsTimeStep
  ///   If running with variable time stepping, this defines the minimum time step size with which the simulation is ticked. Set this to 0 in order to not limit
  ///   the time step size. Note, that this can lead to severe simulation instabilities if your application's frame rate varies wildly.
  ///
  /// \param fMaxPhysicsTimeStep
  ///   If running with variable time stepping, this defines the maximum time step size with which the simulation is ticked. Set this to 0 in order to not limit
  ///   the time step size. Note, that this can lead to severe simulation instabilities if your application's frame rate is low.
  ///
  /// \remarks
  ///   A default value (60 Hz = 1/60 sec) is set during initialization on non-mobile platforms. Mobile platforms default to variable time stepping with a
  ///   minimum time step size of 1/60 sec and a maximum time step of 1/30 sec.
  ///
  /// \note
  ///   On mobile it is advised to only use one simulation step per frame for performance reasons. 
  ///   Thus, to account for devices with lower frame rates one should consider setting the tick count
  ///   adaptively. Alternatively variable time stepping can be used.
  /// 
  /// \see
  ///   vHavokPhysicsModule::OnRunPhysics
  /// 
  VHAVOK_IMPEXP void SetPhysicsTickCount(int iTickCount,
      int iMaxTicksPerFrame=4, bool bFixedTicksPerFrame=false, float fMinPhysicsTimeStep = 0.0f, float fMaxPhysicsTimeStep=1.0/10.0f);

  /// \brief
  ///   Gets the configured number of physics ticks per second
  inline int GetPhysicsTickCount() const 
  {
    if (m_bAllowVariableStepRate) // variable time stepping
      return 0;
    else
      return (int) (1.0f / m_fTimeStep + 0.5f); 
  }


  /// \brief
  ///   Gets the configured fixed physics time step.
  inline float GetPhysicsTimeStep() const { return m_fTimeStep; }

  /// \brief
  ///   Gets the maximum number of physics updates per time step.
  inline int GetMaxTicksPerFrame() const { return m_iMaxTicksPerFrame; }

  /// \brief
  ///   Gets the configured minimum physics time step.
  inline float GetMinPhysicsTimeStep() const { return m_fMinTimeStep; }

  /// \brief
  ///   Gets the configured maximum physics time step.
  inline float GetMaxPhysicsTimeStep() const { return m_fMaxTimeStep; }

  /// 
  /// \brief
  ///   Set the paused state of the physics simulation.
  /// 
  /// \param bStatus
  ///   TRUE if simulation should be paused, FALSE otherwise.
  /// 
  inline void SetPaused(bool bStatus) { m_bPaused = bStatus; } ;
  

  /// 
  /// \brief
  ///   Review whether the simulation is current paused.
  /// 
  /// \returns 
  ///   TRUE if simulation is paused, FALSE otherwise.
  /// 
  inline bool IsPaused() const { return m_bPaused; } ;

  /// 
  /// \brief
  ///   Allow the physics system to be stepped by another module (eg Behavior)
  /// 
  /// \param steppedExternally
  ///   TRUE if simulation will be stepped externally, FALSE if Vision will step physics.
  /// 
  inline void SetSteppedExternally(bool steppedExternally ) { m_steppedExternally = steppedExternally; }

  /// 
  /// \brief
  ///   Allow the VDB to be stepped by another module (eg Behavior)
  /// 
  /// \param steppedExternally
  ///   TRUE if vdb will be stepped externally, FALSE if Vision will step vdb after physics step (default).
  /// 
  inline void SetVdbSteppedExternally(bool steppedExternally ) { m_vdbSteppedExternally = steppedExternally; }


  /// \brief
  ///   Sets the number of threads to use for physics simulation.
  /// \param iNumThreads
  ///   the number of threads to use
  VHAVOK_IMPEXP void SetThreadCount(unsigned int iNumThreads);

  ///
  /// @}
  ///

  ///
  /// @name Debug Rendering
  /// @{
  ///

  /// 
  /// \brief
  ///   Enable / Disable the Vision specific implementation of the Havok Visual Debugger.
  /// 
  /// This method initializes / deinitializes a connection to the Visual Debugger Tool that
  /// can be found in the Havok SDK.
  ///
  /// \param bEnabled
  ///   TRUE if visual debugger should be enabled, FALSE otherwise.
  /// 
  VHAVOK_IMPEXP void SetEnabledVisualDebugger(BOOL bEnabled);

  /// 
  /// \brief
  ///   Review whether the Vision specific implementation of the Havok Visual Debugger is currently enabled.
  /// 
  /// \returns
  ///   TRUE if visual debugger is enabled, FALSE otherwise.
  /// 
  VHAVOK_IMPEXP BOOL IsEnabledVisualDebugger();

  /// \brief
  ///   Returns the Vision specific implementation of the Havok Visual Debugger.
  /// 
  /// \returns
  ///   vHavokVisualDebugger: Pointer to the Vision specific implementation of the Havok Visual Debugger.
  /// 
  VHAVOK_IMPEXP vHavokVisualDebugger* GetHavokVisualDebugger() { return m_spVisualDebugger; }

  /// 
  /// \brief
  ///   Enable / Disable the Vision specific implementation of the Havok Debug Display Handler.
  /// 
  /// \param bEnabled
  ///   TRUE if display handler should be enabled, FALSE otherwise.
  /// 
  VHAVOK_IMPEXP void SetEnabledDebug(BOOL bEnabled);

  /// 
  /// \brief
  ///   Review whether the Vision specific implementation of the Havok Debug Display Handler is current enabled.
  /// 
  /// \returns
  ///   TRUE if display handler is enabled, FALSE otherwise.
  ///  
  VHAVOK_IMPEXP BOOL IsEnabledDebug();

  ///
  /// \brief
  ///   Returns the Vision specific implementation of the Havok Debug Display Handler.
  /// 
  /// \returns
  ///   vHavokDisplayHandler: Pointer to the Vision specific implementation of the Havok Debug Display Handler.
  /// 
  VHAVOK_IMPEXP vHavokDisplayHandler* GetHavokDisplayHandler() 
  { 
    return m_spDisplayHandler; 
  }

  ///
  /// @}
  ///


  ///
  /// @name Collision Filtering
  /// @{
  /// 

  /// \brief
  ///   Sets, whether group1 should collide with group2.
  /// 
  /// This method is slow. It involves re-evaluating all the broadphase AABBs in the system. 
  /// You should normally only call this at startup time. 
  ///
  /// \param group1
  ///   First collision group. 0..32
  ///
  /// \param group2
  ///   Second collision group. 0..32
  /// 
  /// \param bEnable
  ///   True, if collision between specified groups should be enabled, otherwise false.
  ///
  VHAVOK_IMPEXP void SetGroupsCollision(int group1, int group2, bool bEnable);

  /// \brief
  ///   Applies the currently set vHavokPhysicsModuleDefaultRuntimeWorldParams.m_iCollisionGroupMasks to the Havok physics world.
  /// 
  /// This method is slow. It involves re-evaluating all the broadphase AABBs in the system. 
  /// You should normally only call this at startup time. 
  /// 
  VHAVOK_IMPEXP void UpdateGroupsCollision();

  ///
  /// @}
  ///


  ///
  /// @name Module Configuration Options
  /// @{
  /// 
  

  ///
  /// \brief
  ///   Set scaling of the Vision world relative to the Havok world.
  /// 
  /// \param fUnitsPerMeter
  ///   Relative scaling value specified by units per meter.
  /// 
  VHAVOK_IMPEXP void SetVisionWorldScale(hkReal fUnitsPerMeter);
  
  
  ///
  /// \brief
  ///   Get scaling of the Vision world relative to the Havok world.
  /// 
  /// \return
  ///   float: Relative scaling value specified by units per meter.
  /// 
  VHAVOK_IMPEXP hkReal GetVisionWorldScale();


  ///
  /// \brief
  ///   Set the static geometry type (single or merged).
  /// 
  /// \param mode
  ///   Static geometry type (single or merged).
  /// 
  VHAVOK_IMPEXP void SetStaticGeometryMode(StaticGeomMode mode);

  /// \brief
  ///   Will enable or disable debug rendering on all instances of the different types.
  VHAVOK_IMPEXP void EnableDebugRendering(bool bRigidBodies, bool bRagdolls, 
    bool bCharacterControlers, bool bTriggerVolumes, bool bStaticMeshes);

  /// \brief
  ///   Returns whether it is enforced to cache physics shapes to HKT files even when operating outside vForge.
  inline bool IsHktShapeCachingEnforced() const
  {
    return m_bForceHktShapeCaching;
  }

  /// \brief
  ///   Force caching of physics shapes to HKT files even when operating outside vForge.
  /// 
  /// \param bForceHktShapeCaching
  ///   Toggles forcing of caching physics shapes to HKT files.  
  /// 
  /// \note
  ///   When forcing is enabled, all warnings about related missing or outdated cached HKT files are suppressed.
  ///   This setting will have no influence on caching trigger volume or terrain sector shapes to HKT files.
  inline void ForceHktShapeCaching(bool bForceHktShapeCaching)
  {
    m_bForceHktShapeCaching = bForceHktShapeCaching;
  }


  ///
  /// @}
  ///


  ///
  /// @name Access to Havok Internals
  /// @{


  ///
  /// \brief
  ///   Returns the internal Havok wold.
  /// 
  /// \return
  ///   hkpWorld: Pointer to the Havok world.
  /// 
  inline hkpWorld* GetPhysicsWorld() { return m_pPhysicsWorld; }
  inline hkJobThreadPool* GetThreadPool() { return m_pJobThreadPool; }
  inline hkJobQueue* GetJobQueue() { return m_pJobQueue; }

  /// \brief
  ///   Lock the internal Havok world.
  VHAVOK_IMPEXP void LockWorld();

  /// \brief
  ///   Unlock the internal Havok world.
  VHAVOK_IMPEXP void UnlockWorld();

  /// \brief
  ///   Mark the internal Havok world for write access in this thread.
  VHAVOK_IMPEXP void MarkForWrite();

  /// \brief
  ///   Mark the internal Havok world for read access in this thread.
  VHAVOK_IMPEXP void MarkForRead();

  /// \brief
  ///   Remove the write mark from the internal Havok world.
  VHAVOK_IMPEXP void UnmarkForWrite();

  /// \brief
  ///   Remove the read mark from the internal Havok world.
  VHAVOK_IMPEXP void UnmarkForRead();

  /// \brief
  ///   Sets manually the solver iterations (2,4,8) and hardness (0 (soft),1 (medium), 2 (hard) )
  VHAVOK_IMPEXP void SetSolver(int iters, int hardness); 

  /// \brief
  ///   returns the number of solver iterations per physics step.
  ///
  /// See the Havok SDK documentation of hkpWorldCInfo::m_solverIteration
  /// for more information about this value.
  ///
  VHAVOK_IMPEXP int GetNumSolverIterations() const;

  /// \brief
  ///   returns the number of solver micro-steps per physics step.
  ///
  /// See the Havok SDK documentation of hkpWorldCInfo::m_solverMicrosteps
  /// for more information about this value.
  ///
  VHAVOK_IMPEXP int GetNumSolverMicrosteps() const;

  /// \brief
  ///   returns the solver tau of the Havok physics solver.
  ///
  /// See the Havok SDK documentation of hkpWorldCInfo::m_solverTau
  /// for more information about this value.
  ///
  VHAVOK_IMPEXP hkReal GetSolverTau() const;

  ///
  /// @}
  ///

  VHAVOK_IMPEXP void SetGravity(const hkvVec3& gravity);
  VHAVOK_IMPEXP void GetGravity(hkvVec3& gravity);


  ///
  /// @name Broadphase setup 
  /// @{

  /// \brief
  ///   Automatically generates and sets broadphase size from all static physics objects.
  VHAVOK_IMPEXP bool SetBroadphaseSizeAuto();


  /// \brief
  ///   Sets manually broadphase size.
  VHAVOK_IMPEXP void SetBroadphaseSizeManual(float fBroadphaseManualSize); 


  /// \brief
  ///   Dynamic resizing of the broadphase involves removing bodies from the world and re-adding them.
  ///   This function allows you to specify a listener that should not be called when this happens.
  VHAVOK_IMPEXP void AddEntityListenerToIgnore(hkpEntityListener* listener);

  ///
  /// @}
  ///


  VHAVOK_IMPEXP const VArray<IHavokStepper*, IHavokStepper*> GetSteppers() const;
  VHAVOK_IMPEXP void AddStepper(IHavokStepper* s);
  VHAVOK_IMPEXP void RemoveStepper(IHavokStepper* s);
  VHAVOK_IMPEXP void WaitForSimulationToComplete();

  /// \name Static Callbacks
  //@{

  VHAVOK_IMPEXP static VisCallback_cl OnBeforeInitializePhysics;
  VHAVOK_IMPEXP static VisCallback_cl OnAfterInitializePhysics;
  VHAVOK_IMPEXP static VisCallback_cl OnBeforeDeInitializePhysics;
  VHAVOK_IMPEXP static VisCallback_cl OnAfterDeInitializePhysics;
  VHAVOK_IMPEXP static VisCallback_cl OnUnsyncHavokStatics;

  /// \brief
  ///   This callback gets called before the Havok world (hkpWorld) is created
  /// 
  /// The passed data object can be casted to vHavokBeforeWorldCreateDataObject_cl and provides
  /// access to the hkpWorldCinfo object that will be used for creating the Havok world. You
  /// can thus use this callback to modify the hkpWorldCinfo according to your own requirements.
  ///
  /// If you access this callback outside the Havok plugin then you need to compile your project
  /// with the VHAVOKMODULE_IMPORTS preprocessor definition. Otherwise you might get an unresolved
  /// external for this callback.
  ///
  /// \note
  ///   You must not change the m_simulationType member in passed the hkpWorldCinfo object.
  ///   The simulation type depends on other configurations, such as the job thread pool. Thus
  ///   any change in the multi-threading behaviour has to be changed in the module's source code
  ///   instead.
  /// 
  /// \sa vHavokBeforeWorldCreateDataObject_cl
  VHAVOK_IMPEXP static VisCallback_cl OnBeforeWorldCreated;
  VHAVOK_IMPEXP static VisCallback_cl OnAfterWorldCreated;
  VHAVOK_IMPEXP static VisCallback_cl OnWorkerThreadCreated;
  VHAVOK_IMPEXP static VisCallback_cl OnWorkerThreadFinished;
  VHAVOK_IMPEXP static VisCallback_cl OnFetchPhysicsResults;

  //@}

  /// \brief
  ///   Saves or loads world global setting to/ from the passed chunk file.
  VHAVOK_IMPEXP void GlobalsChunkFileExchange(VChunkFile &file, CHUNKIDTYPE iID);

  /// \name Determinism Checking
  /// \brief Determinism checking utility functions
  ///
  /// The hkCheckDeterminismUtil is a Havok utility devised to check whether physics
  /// execution in a certain demo is deterministic. Several sources of non-determinism
  /// can exist in a game (multithreading, floating point precision, etc...), and this
  /// utility can help identify those sources.
  ///
  /// A static instance of this utility will be created on Havok initialization
  /// and destroyed on havok deinitialization. This instance will usually be idle and
  /// will not perform anything. If havok is compiled without the HK_WANT_DETERMINISM_CHECKS
  /// flag, nothing will happen.
  ///
  /// When havok is compiled with HK_WANT_DETERMINISM_CHECKS, the physics engine will
  /// perform checking call during simulation. If the hkCheckDeterminismUtil is
  /// idle nothing will happen, but if the user configures write or check mode, then
  /// a log will be written or opened for checking during the physics simulation.
  /// 
  /// If the determinism check fails, execution will pause and a breakpoint will
  /// be triggered as soon as the failure was detected.
  ///
  /// NOTES:
  /// - Currently, the determinism util has only been tested with the asynchronous physics vision mode
  /// - The user should make sure that the physics stepping happens in the same thread that initialized
  ///   the determinism util, that is the main thread usually. This is in contrast with the default
  ///   behaviour of the vision engine. Future revisions will support a wider range of determinism testing.
  //@{

  /// \brief
  ///   Called to start writing the determinism log file.
  ///
  /// Determinism testing works by executing the exact same code two times, the
  /// first time it will write out a log file and the second time will check whether
  /// the local results match the log.
  /// \param filename
  ///   file where to save the determinism log.
  VHAVOK_IMPEXP void StartDeterminismWriteMode(const char* filename = "hkDeterminismCheckfile.bin");

  /// \brief
  ///   Called to start checking the determinism log file.
  ///
  /// Determinism testing works by executing the exact same code two times, the
  /// first time it will write out a log file and the second time will check whether
  /// the local results match the log.
  /// \param filename
  ///   file containing the determinism log used for checking.
  VHAVOK_IMPEXP void StartDeterminismCheckMode(const char* filename = "hkDeterminismCheckfile.bin");

  /// \brief
  ///   Finish any outstanding determinism check. Return to IDLE mode.
  VHAVOK_IMPEXP void FinishDeterminismTest();

  /// \brief
  ///   Step the Visual Debugger. Normally you would only call this if you set the VdbExternallyStep to true.
  VHAVOK_IMPEXP void StepVisualDebugger();
  
  /// \brief
  ///   When you step the Visual Debugger externally, you also probably control when the timers are cleared. Normally you would only call this if you set the VdbExternallyStep to true.
  VHAVOK_IMPEXP void ClearVisualDebuggerTimerData();

  //@}

protected:

  int PerformSweepInternal(vHavokSweepResult *pResults, int iNumResults, const hkpCollidable *pCollidable, const hkvVec3& vPos, const hkvVec3& vDir, float fDistance);
  
  bool AutoComputeBroadphaseSize(hkAabb &worldBounds);
  void SetBroadphaseSize(const hkAabb &worldBounds);

  /// \brief
  ///   Checks the solver buffer is big enough.
  bool CheckSolverBufferSize();

  // collision notification queue
  void OnObjectCollision(vHavokCollisionInfo_t& info);
  void CallObjectCollisionFunction(vHavokCollisionInfo_t& info);  
  void RemoveObjectFromQueues(vHavokRigidBody *pRB, vHavokCharacterController *pController, vHavokStaticMesh *pSM, vHavokTerrain *pTerrain);

  // Workaround for [HVK-6398]
  // This function is basically a copy of hkpWorld::removePhysicsSystem with the difference that
  // actions are removed before constraints, which avoids an assert that would happen
  // if a constraint chain is contained in the physics system.
  // The workaround should be removed as soon as the Jira Issue in Havok Physics is resolved
  void hkpWorldRemovePhysicsSystem(hkpWorld* pWorld, hkpPhysicsSystem* pSystem);

  // trigger notification queue
  void OnVolumeTrigger(vHavokTriggerInfo &info);
  void CallVolumeTriggerFunction(vHavokTriggerInfo &info);
  void RemoveVolumeFromQueues(vHavokTriggerVolume *pTriggerVolume);

  void SendEnqueuedNotifications();
  static void TriggerCollisionScriptFunction(IVObjectComponent *pPhysComponent, vHavokCollisionInfo_t *pCollInfo); 
  VMutex m_NotificationQueueMutex;
  DynObjArray_cl<vHavokCollisionInfo_t> m_CollisionNotificationQueue;
  int m_iCollisionNotificationQueueSize;
  DynObjArray_cl<vHavokTriggerInfo> m_TriggerNotificationQueue;
  int m_iTriggerNotificationQueueSize;

  bool m_bDebugRenderRigidBodies;
  bool m_bDebugRenderRagdolls;
  bool m_bDebugRenderCharacterControllers;
  bool m_bDebugRenderTriggerVolumes;
  bool m_bDebugRenderStaticMeshes;

  friend class vHavokRigidBody;
  friend class vHavokRagdoll;
  friend class vHavokCharacterController;
  friend class vHavokConstraint;
  friend class vHavokConstraintChain;
  friend class vHavokStaticMesh;
  friend class vHavokContactListener;
#ifdef SUPPORTS_TERRAIN
  friend class vHavokTerrain;
  friend class VHavokDecorationGroup;
#endif
  friend class vHavokTriggerVolumeInternal;
  friend class vHavokTriggerVolume;
  friend class VHavokTask;
  friend class vHavokPhysicsRaycaster;
  friend class vHavokError;

  friend void HK_CALL errorReport(const char* msg, void* errorReportObject);

  ///
  /// \brief
  ///   Adds a havok rigid body to the simulation world.
  ///   
  /// \param pRigidBody
  ///   Pointer of havok rigid body to be added.
  ///
  void AddRigidBody(vHavokRigidBody* pRigidBody);

  ///
  /// \brief
  ///   Removes a havok rigid body from the simulation world.
  ///   
  /// \param pRigidBody
  ///   Pointer of havok rigid body to be removed.
  ///
  void RemoveRigidBody(vHavokRigidBody* pRigidBody);

  ///
  /// \brief
  ///   Adds a havok rag doll to the simulation world:
  ///
  /// \param pRagdoll
  ///   Pointer of the havok rag doll to be added.
  ///
  void AddRagdoll(vHavokRagdoll* pRagdoll);

  ///
  /// \brief
  ///   Removes a havok rag doll from the simulation world:
  ///
  /// \param pRagdoll
  ///   Pointer of the havok rag doll to be removed.
  ///
  void RemoveRagdoll(vHavokRagdoll* pRagdoll);

  ///
  /// \brief
  ///   Adds a havok character controller to the simulation world.
  ///   
  /// \param pCharacterController
  ///   Pointer of havok character controller to be added.
  ///
  void AddCharacterController(vHavokCharacterController *pCharacterController);


  ///
  /// \brief
  ///   Removes a havok character controller from the simulation world.
  ///   
  /// \param pCharacterController
  ///   Pointer of havok character controller to be removed.
  ///
  void RemoveCharacterController(vHavokCharacterController *pCharacterController);


  ///
  /// \brief
  ///   Adds a havok static mesh to the simulation world.
  ///   
  /// \param pStaticMesh
  ///   Pointer of havok static mesh to be added.
  ///
  void AddStaticMesh(vHavokStaticMesh *pStaticMesh);
  
  
  ///
  /// \brief
  ///   Removes a havok static mesh from the simulation world.
  ///   
  /// \param pStaticMesh
  ///   Pointer of havok static mesh to be removed.
  ///
  void RemoveStaticMesh(vHavokStaticMesh *pStaticMesh);

#ifdef SUPPORTS_TERRAIN

  ///
  /// \brief
  ///   Adds a havok terrain heightfield to the simulation world.
  ///   
  /// \param pTerrain
  ///   Pointer of havok terrain heightfield mesh to be added.
  ///
  void AddTerrain(vHavokTerrain *pTerrain);


  ///
  /// \brief
  ///   Removes a havok terrain heightfield from the simulation world.
  ///   
  /// \param pTerrain
  ///   Pointer of havok terrain heightfield to be removed.
  ///
  void RemoveTerrain(vHavokTerrain *pTerrain);

    
  ///
  /// \brief
  ///   Adds a decorations (trees etc in a static mesh) to the simulation world.
  ///   
  /// \param pDecoration
  ///   Pointer of havok decorations to be added.
  ///
  void AddDecoration(VHavokDecorationGroup *pDecoration);


  ///
  /// \brief
  ///   Removes a havok decorations from the simulation world.
  ///   
  /// \param pDecoration
  ///   Pointer of havok decorations to be removed.
  ///
  void RemoveDecoration(VHavokDecorationGroup *pDecoration);

#endif

  ///
  /// \brief
  ///   Adds a havok constraint to the simulation world.
  ///   
  /// \param pConstraint
  ///   Pointer to the havok constraint to be added.
  ///
  void AddConstraint(vHavokConstraint *pConstraint);


  ///
  /// \brief
  ///   Removes a havok constraint from the simulation world.
  ///   
  /// \param pConstraint
  ///   Pointer to the havok constraint to be removed.
  ///
  void RemoveConstraint(vHavokConstraint *pConstraint);

  ///
  /// \brief
  ///   Adds a havok constraint chain to the simulation world.
  ///   
  /// \param pConstraintChain
  ///   Pointer to the havok constraint chain to be added.
  ///
  void AddConstraintChain(vHavokConstraintChain *pConstraintChain);


  ///
  /// \brief
  ///   Removes a havok constraint chain from the simulation world.
  ///   
  /// \param pConstraintChain
  ///   Pointer to the havok constraint chain to be removed.
  ///
  void RemoveConstraintChain(vHavokConstraintChain *pConstraintChain);


  ///
  /// \brief
  ///   Adds a havok trigger volume to the simulation world.
  ///   
  /// \param pTriggerVolume
  ///   Pointer of havok trigger volume to be added.
  ///
  void AddTriggerVolume(vHavokTriggerVolume *pTriggerVolume);


  ///
  /// \brief
  ///   Removes a havok trigger volume from the simulation world.
  ///   
  /// \param pTriggerVolume
  ///   Pointer of havok trigger volume to be removed.
  ///
  void RemoveTriggerVolume(vHavokTriggerVolume *pTriggerVolume);

  ///
  /// \brief
  ///   Creates one Havok shape for all static meshes that have been collected during
  ///   scene loading (only relevant in SGM_MERGED_INSTANCES mode).
  ///
  void CreatePendingStaticMeshes();


  ///
  /// \brief
  ///   Updates transforms of all havok rigid bodies in the simulation.
  ///
  void UpdateHavok2Vision();

  /// \brief
  ///   Performs the simulation for one frame
  /// \param bTask
  ///   whether this is performed by a Vision task (rather than calling
  ///   it directly from the physics module)
  /// \param bWorkerThread
  ///   whether this is executed in a worker thread (rather than in the
  ///   main thread)
  void PerformSimulation(bool bTask, bool bWorkerThread);

  ///
  /// \brief
  ///   Overridable function that gets called by all callbacks that this handler is registered to. 
  ///
  VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);


	///
	/// \brief
	///   Converts a successful hit from a havok result into a vision raycast onHit callback 
	///
	void ForwardRaycastData(VisPhysicsRaycastBase_cl *pRaycastData, const hkpWorldRayCastOutput* pHavokResult );

  /// \brief
  ///   Dumps the cached messages and warnings to the log output (e.g. resource viewer)
  void DumpCachedMessagesToLog();

  /// \brief
  ///   Create the bodies (usually as late as possible so that world not needed until last minute in setup / load)
  void CreateCachedBodies();
  
  /// \brief
  ///   Create the world (usually as late as possible so that bounds of world etc known)
  void CreateWorld();

  void StartSimulation(float dt);
  void WaitForForegroundSimulationToComplete();

  hkJobQueue* m_pJobQueue;                            ///< Havok job queue
  hkSpuUtil* m_pSpuUtil;                              ///< Havok SPU utilities for PS3;
  hkJobThreadPool* m_pJobThreadPool;                  ///< Pool of threads to take jobs from the Havok job queue;
  VArray<IHavokStepper*, IHavokStepper*> m_steppers;  ///< List of available Havok steppers.
  bool m_steppedExternally;							              ///< Whether physics is stepped externally
  bool m_vdbSteppedExternally;							              ///< Whether VDB is stepped externally
  int m_iMinSolverBufferIncreaseStep;                 ///< The minimum step size to enlarge the solver buffer if it becomes too small (= start size)

  vHavokProfiler* m_pProfiler;                        ///< Profiler to redirect havok profiling to vision's profiling

  // Havok world
  hkpWorld* m_pPhysicsWorld;                          ///< Havok physics world object; Container for the simulation's physical objects

  // Listeners
  vHavokConstraintListenerPtr m_spConstraintListener; ///< Vision specific implementation of the Havok Constraint Listener 
  vHavokContactListenerPtr m_spContactListener;       ///< Vision specific implementation of the Havok Collision Listener 

  // Simulation stepping
  float m_fLeftOver;                                  ///< Time passed since last time step. 
  float m_fTimeStep;                                  ///< Time value how frequent the physics simulation gets updated. Caution: this is not the frame time. If you need the frame time use m_fCurrentTimeDelta instead.
  float m_fMinTimeStep;                               ///< Minimum size of the time step with which the physics simulation is ticked.
  float m_fMaxTimeStep;                               ///< Maximum size of the time step with which the physics simulation is ticked.
  int m_iMaxTicksPerFrame;                            ///< clamp value for ticks per frame
  int m_iNumTicksThisFrame;                           ///< Number of times the physics world will be stepped this frame
  bool m_bAllowVariableStepRate;                      ///< True to allow a variable step size between m_fMinTimeStep and m_fMaxTimeStep
  bool m_bFixedTicksPerFrame;                         ///< Always do iMaxTicksPerFrame in each frame
  bool m_bPaused;                                     ///< Determines whether the physics simulation is paused or not
  bool m_bSimulationStarted;                          ///< State whether simulation has already started
  bool m_bNewResultsAvailable;                        ///< Flag whether new simulation results can be fetched in this frame
  bool m_bCurrentRunIsAsync;                          ///< True if the current physics run is asynchronous (using Vision task)

  // Asynchronous processing
  bool m_bAsyncPhysics;                               ///< Whether to use asynchronous processing for physics
  VHavokTask* m_pTask;                                ///< Task for asynchronously handling Havok physics
  
  // HKT shape caching
  bool m_bForceHktShapeCaching;                       ///< Force caching physics shapes to HKT files

  // Havok Debug Rendering
  vHavokVisualDebuggerPtr m_spVisualDebugger;         ///< Smart Pointer to the Havok Visual Debugger interface
  vHavokDisplayHandlerPtr m_spDisplayHandler;         ///< Smart Pointer to the Havok Debug Display Handler
  BOOL m_bVisualDebugger;                             ///< Determines whether physics should be displayed in visual debugger
  BOOL m_bDebugDisplay;                               ///< Determines whether visual debugging should be enabled in Vision

  // Collision Filtering
  bool m_bUpdateFilter;                               ///< Determines whether world collision filter should be updated

  // World configuration
  StaticGeomMode m_staticGeomMode;                    ///< Specifies how the static geometry is represented/optimized within Havok (merged, single instances)
  VisWeldingType_e m_mergedStaticWeldingType;         ///< Specifies the welding type for merged static mesh instances
  
  // Scene loading
  bool m_bIsSceneLoading;                             ///< Set to TRUE if the scene is currently being loaded
  bool m_bResultsExpected;                            ///< State wether it is safe to fetch results

  // Cached log output
  VMutex m_errorReportMutex;                          ///< Mutex for synchronizing access to the m_cachedWarnings and m_cachedMessages variables
  VStrList m_cachedWarnings;                          ///< List of Havok warnings and errors reported during simulation. Can be report from within worker threads.
  VStrList m_cachedMessages;                          ///< List of Havok messages reported during simulation. Can be report from within worker threads.

  // Raycaster
  vHavokPhysicsRaycaster* m_pRayCasterSingle;         ///< Collects the closest contact point for each ray.
  vHavokPhysicsRaycaster* m_pRayCasterMultiple;       ///< Collects maximum RAYCAST_THREAD_RESULTS_PER_CMD contact points for each ray.

  // Object lists
  VRefCountedCollection<vHavokRigidBody> m_simulatedRigidBodies;             ///< Collection of all Havok rigid bodies in the simulation
  VRefCountedCollection<vHavokRagdoll> m_simulatedRagdolls;                  ///< Collection of all Havok rag dolls in the simulation
  VRefCountedCollection<vHavokCharacterController> m_simulatedControllers;   ///< Collection of all Havok character controllers in the simulation
  VRefCountedCollection<vHavokStaticMesh> m_simulatedStaticMeshes;           ///< Collection of all Havok static meshes in the simulation 
  VisStaticMeshInstCollection m_pendingStaticMeshes;                         ///< (Non ref-counted) collection of static meshes that still need to be added to Havok once the scene is loaded (relevant SGM_MERGED_INSTANCES mode)

#ifdef SUPPORTS_TERRAIN
  VRefCountedCollection<vHavokTerrain> m_simulatedTerrainSectors;            ///< Collection of all Havok static height fields in the simulation 
  VRefCountedCollection<VHavokDecorationGroup> m_simulatedDecorations;       ///< Collection of all Havok decorations in the simulation 
#endif

  VPListT<vHavokConstraint> m_simulatedConstraints;                          ///< Collection of all Havok constraints
  VRefCountedCollection<vHavokTriggerVolume> m_simulatedTriggerVolumes;      ///< Collection of all Havok trigger volumes in the simulation

  VArray<hkpEntityListener*> m_entityListenersToIgnore;

protected:
  hkSemaphoreBusyWait* m_pSweepSemaphore;                                   ///< Internally used to synchronized batched sweep tests
};  

/// \brief
///   Marks the physics world for read or write for as long an instance
///   of this class exists.
class vHavokMarkWorld
{
public:
  /// If a physics world has been created, marks the world for either
  /// reading or writing, depending on \c bForWrite.
  /// \param bForWrite
  ///   if \c true, marks the world for writing; if \c false, for reading.
  vHavokMarkWorld(bool bForWrite = false);
private:
  vHavokMarkWorld(vHavokMarkWorld const&);
  vHavokMarkWorld& operator=(vHavokMarkWorld const&);
public:
  /// If the physics world has been marked, unmarks it again.
  ~vHavokMarkWorld();

private:
  hkpWorld *pWorld;
  bool bForWrite;
};

#endif // VHAVOKPHYSICSMODULE_HPP_INCLUDED

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
