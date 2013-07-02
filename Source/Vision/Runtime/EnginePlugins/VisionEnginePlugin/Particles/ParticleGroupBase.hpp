/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ParticleGroupBase.hpp

#ifndef PARTICLEGROUPBASE_HPP_INCLUDED
#define PARTICLEGROUPBASE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiParticleGroup.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiBitmap.hpp>
#include <Vision/Runtime/Engine/System/VisApiCallbacks.hpp>
#include <Vision/Runtime/Base/Math/Random/VRandom.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleDescriptor.hpp>

// group flags
#define PGROUPFLAGS_NONE        0x00000000

// maximum recursion depth for particle groups that have a child group for destroyed particles
#define MAX_ONDESTROY_GROUPDEPTH    8

// minimum cache size, also used for static member size
#define PARTICLECACHESIZE_NOALLOC   32

// known trigger target constants
#define PARTICLETRIGGER_PAUSE     "Pause"
#define PARTICLETRIGGER_RESUME    "Resume"
#define PARTICLETRIGGER_RESTART   "Restart"

// types of particle distortions - replace by VIS_PARTICLE_TOPOLOGY_e
#define DISTORTION_TYPE_NONE      0
#define DISTORTION_TYPE_VELOCITY  1
#define DISTORTION_TYPE_FIXLEN    2
#define DISTORTION_TYPE_SIZEMODE  3
#define DISTORTION_TYPE_TRAIL     5

#define DISTORTION_TYPE_CUSTOM    100 // Particle system will not modify a custom distortion vector supplied by application code


// smart pointer def.
class TiXmlNode;
class TiXmlElement;
class ParticleGroupBase_cl;
class VisParticleGroupDescriptor_cl;
class VisParticleGroupManager_cl;
class VisParticleEffect_cl;
class VisParticleEffectFile_cl;
typedef VSmartPtr<ParticleGroupBase_cl> ParticleGroupBasePtr;
typedef VSmartPtr<VisParticleEffect_cl> VisParticleEffectPtr;
typedef VSmartPtr<VisParticleEffectFile_cl> VisParticleEffectFilePtr;

extern VModule g_VisionEngineModule;


///\brief
///Threaded task class used by ParticleGroupBase_cl to perform asynchronous simulation
class HandleParticlesTask_cl : public VThreadedTask
{
public:
  ///\brief
  ///Constructor
  HandleParticlesTask_cl(ParticleGroupBase_cl *pGroup);

  ///\brief
  ///Overridden Run function that performs the actual simulation
  virtual void Run(VManagedThread *pThread);

  ParticleGroupBase_cl *m_pParticleGroup; ///< owner group
  float m_fTimeDelta; ///< time delta used for simulation

public:
  //type management
  inline HandleParticlesTask_cl() {}
  V_DECLARE_DYNCREATE_DLLEXP( HandleParticlesTask_cl,  PARTICLE_IMPEXP );
};


///\brief
///Structure that represents a single particle of the extended particle system. Adds more particle specific properties
struct ParticleExt_t : public Particle_t
{
  float m_fDistortionMult;  ///< scaling of distortion length
  float m_fLifeTimeCounter; ///< for handling lifetime
  float m_fLifeTimeInc;     ///< for handling lifetime
  float m_fAnimPos,m_fAnimInc; ///< for handling animation
  float m_fRotationParam0;  ///< speed for mode PARTICLE_ROTATION_CONSTSPEED or curve pos for PARTICLE_ROTATION_ANGLECURVE
  float m_fRotationParam1;  ///< curve pos incrementor for PARTICLE_ROTATION_ANGLECURVE
  float m_fSizeGrowth;      ///< growth of the particle
  float m_fInertiaFactor;   ///< inertia value [0..1]
  VColorRef m_ModColor;     ///< color for modulating with the current frame color
};


///\brief
///Generic interface to access a particle array. Used for the constraint simulation. Implemented by particle effects and cloth
class IVPhysicsParticleCollection_cl
{
public:
  virtual ~IVPhysicsParticleCollection_cl(){}

  ///\brief
  ///  Must be implemented to return the array size (number of particles)
  PARTICLE_IMPEXP virtual int GetPhysicsParticleCount() const = 0;

  ///\brief
  ///  Must be implemented to return the stride in bytes between two particles in the array, e.g. sizeof(ParticleExt_t)
  PARTICLE_IMPEXP virtual int GetPhysicsParticleStride() const = 0;

  ///\brief
  ///  Must be implemented to return the pointer to the array itself
  PARTICLE_IMPEXP virtual Particle_t *GetPhysicsParticleArray() const = 0;

  ///\brief
  ///  Must be implemented to process particles that should be destroyed
  PARTICLE_IMPEXP virtual void DestroyParticle(Particle_t *pParticle,float fTimeDelta) = 0;
  
  /// \brief
  ///  Updates the seed offset using the previously set base seed and current engine
  ///  frame information. The application can control this behavior by calling 
  ///  Vision::Game.GetUpdateSceneCount().
  inline void UpdateSeed();

  /// \brief
  ///  Get const access to the generator 
  /// \note
  ///  By only allowing const access, application cannot call the seed functions
  ///  directly, which would break the guaranteed synchronization of pseudo-random
  ///  numbers between runs of the engine.
  inline const VRandom& GetRandom() const { return m_generator; }

  /// \brief
  ///  Get the base random number seed
  inline unsigned int GetBaseSeed() const { return m_uiBaseSeed; }

protected:
  IVPhysicsParticleCollection_cl(unsigned int uiBaseSeed=0);

private:
  unsigned int m_uiBaseSeed; ///< Set in vForge using RandomBaseSeed in the properties
  VRandom m_generator;
};


///\brief
/// This callback data object type is fired by the VisParticleGroupManager_cl::OnLayerEvent event. It holds the particle effect layer instance and the event
class VisParticleLayerEventData_cl : public IVisCallbackDataObject_cl
{
public:
  VisParticleLayerEventData_cl(ParticleGroupBase_cl *pLayer, VisAnimEvent_cl *pEvent);

  ParticleGroupBase_cl *m_pLayer; ///< The particle effect layer instance that triggered the event
  VisAnimEvent_cl *m_pEvent;      ///< the event (see class VisAnimEvent_cl)
};


///\brief
/// This class corresponds to the instance of a particle layer
///
///This class is usually not directly instantiated but rather part of a full effect instance which is represented
///by class VisParticleEffect_cl 
class ParticleGroupBase_cl : public VisParticleGroup_cl, public IVPhysicsParticleCollection_cl
{
public:
  ///\brief
  ///Constructor
  PARTICLE_IMPEXP ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, unsigned int uiRandomSeed);
  ///\brief
  ///Constructor
  PARTICLE_IMPEXP ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, bool bSpawnParticles, unsigned int uiRandomSeed);
  ///\brief
  ///Constructor
  PARTICLE_IMPEXP ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, VisParticleEmitter_cl *pEmitter, const hkvVec3& vSpawnPos, const hkvVec3& vOrientation, float fScaling, bool bSpawnParticles, unsigned int uiRandomSeed);
  ///\brief
  ///Destructor
  PARTICLE_IMPEXP virtual ~ParticleGroupBase_cl();

  ///\brief
  ///Associates an emitter instance with this layer
  inline void SetEmitter(VisParticleEmitter_cl *pEmitter);
  ///\brief
  ///Returns the current emitter instance of this layer
  inline VisParticleEmitter_cl* GetEmitter() const;

  ///\brief
  ///Returns the descriptor that provides this layer's properties
  inline VisParticleGroupDescriptor_cl *GetDescriptor() const;

  ///\brief
  ///Internal - do not use
  ParticleGroupBase_cl *GetOnDestroyCreateGroup() const {return m_spOnDestroyCreateGroup;}

  ///\brief
  ///Internal - do not use
  inline VisParticleEffect_cl *GetParentEffect() const
  {
    if (VISION_UNLIKELY(m_pParentGroup!=NULL)) 
      return m_pParentGroup->GetParentEffect();
    return m_pParentEffect;
  }
  ///\brief
  ///Internal - do not use
  void SetInitialTransformation();

  ///\brief
  ///Overridden VisParticleGroup_cl function to calculate a distance to camera that is appropriate for sorting. It uses the parent's effect center
  virtual float GetSortingDistance(const VisObject3D_cl *pCamera) HKV_OVERRIDE;
  
  ///\brief
  ///Accesses the array as an array of ParticleExt_t
  inline ParticleExt_t* GetParticlesExt() const {return (ParticleExt_t *)GetParticles();}

  ///\brief
  ///Implements the IVPhysicsParticleCollection_cl interface
  virtual int GetPhysicsParticleStride() const HKV_OVERRIDE {return sizeof(ParticleExt_t);}
  ///\brief
  ///Implements the IVPhysicsParticleCollection_cl interface
  virtual int GetPhysicsParticleCount() const HKV_OVERRIDE {return m_iHighWaterMark;}
  ///\brief
  ///Implements the IVPhysicsParticleCollection_cl interface
  virtual Particle_t *GetPhysicsParticleArray() const HKV_OVERRIDE {return (ParticleExt_t *)GetParticles();}
  ///\brief
  ///Implements the IVPhysicsParticleCollection_cl interface
  PARTICLE_IMPEXP virtual void DestroyParticle(Particle_t *pParticle,float fTimeDelta) HKV_OVERRIDE;

  ///\brief
  ///Orientation helper function
  inline void SetOrientation(float fYaw,float fPitch,float fRoll);
  ///\brief
  ///Orientation helper function
  inline void SetOrientation(const hkvMat3 &mRotMat);
  ///\brief
  ///Orientation helper function
  inline void SetOrientation(const hkvVec3& vDir, const hkvVec3& vUp, const hkvVec3& vRight);
  ///\brief
  ///Moves this layer by certain position delta
  PARTICLE_IMPEXP void IncPosition(const hkvVec3& vDelta);
  ///\brief
  ///Returns the current direction
  PARTICLE_IMPEXP hkvVec3 GetDirection();

  ///\brief
  ///Deprecated
  PARTICLE_IMPEXP void AttachToCamera(const hkvVec3& vRelPos)   {m_bAttachedToCam=true;m_vCamRelPos=vRelPos;}
  ///\brief
  ///Deprecated
  PARTICLE_IMPEXP void DetachFromCamera() {m_bAttachedToCam=false;}
  ///\brief
  ///Updates the visibility bounding box by evaluating the particle's box
  PARTICLE_IMPEXP void UpdateVisibilityObject();

  ///\brief
  ///Applies a uniform scaling factor
  inline void SetScaling(float fScale=1.f);
  ///\brief
  ///Return current scaling factor
  inline float GetScaling() const;

  ///\brief
  ///Moves all particle positions by specified position delta
  PARTICLE_IMPEXP void MoveParticles(const hkvVec3& vDelta);
  ///\brief
  ///Simulation properties as defined in vForge
  PARTICLE_IMPEXP void SetLocalFactors(float fAtLifetimeStart, float fAtLifetimeEnd);

  ///\brief
  ///Sets whether to add the time-of-day light to the particles color.
  inline void SetApplyTimeOfDayLight(bool bApply) {m_bApplyTimeOfDayLight = bApply; EvaluateSceneBrightness ();}
  ///\brief
  ///Returns whether to add the time-of-day light to the particles color.
  inline bool GetApplyTimeOfDayLight() const {return (m_bApplyTimeOfDayLight);}

  ///\brief
  ///Sets the pause status. If paused, the emitter does not spawn new particles
  inline void SetPause(bool bStatus);
  ///\brief
  ///Returns the current pause status.
  inline bool IsPaused() const;

  ///\brief
  ///Sets the halted status. When halted, existing particles do not move anymore
  inline void SetHalted(bool bStatus);
  ///\brief
  ///Returns the current halted status.
  inline bool IsHalted() const;

  ///\brief
  ///If true, the particle lifetime is still updated while particles are invisible (default: false)
  ///This makes particles to get destroyed even if they are invisible (and thus not simulated).
  ///This can be used in combination with SetHandleWhenVisible(true).
  ///
  ///\param bStatus
  ///  If true, life is drained even if particles are invisible.
  inline void SetUpdateLifetimeIfInvisible(bool bStatus) { m_bUpdateLifetimeIfInvisible = bStatus; }

  ///\brief
  ///Returns true if particles lifetime is updated even while they are invisible.
  inline bool IsLifetimeUpdatedIfInvisible() const  { return m_bUpdateLifetimeIfInvisible; }

  ///\brief
  ///A "dead" group just waits until there are no refs anymore to delete itself
  inline bool IsDead() const {return m_bIsDead;}
  ///\brief
  ///Indicates whether the layer's lifetime is used up
  inline bool IsLifeTimeOver() {if (m_bInfiniteLifeTime) return false;return (m_fLifeTime<0.f);}
  ///\brief
  ///Marks the layer as finished, i.e. no new particles are spawned. As soon as all particles have disappeared, the layer can be destroyed.
  inline void SetFinished() 
  {
    m_bRepeatLifetime=false;
    m_bInfiniteLifeTime=false;
    m_fLifeTime=-1.f;
  }

  ///\brief
  ///Returns the remaining lifetime of the layer in seconds. If the layer has infinite lifetime, this function returns -1.0f
  inline float GetRemainingLifeTime() const
  {
    if (m_bInfiniteLifeTime) 
      return -1.f;
    return m_fLifeTime / m_fTimeScale;
  }

  ///\brief
  ///Returns a new particle from the layer. The result can be NULL
  PARTICLE_IMPEXP ParticleExt_t* GetFreeParticle();
  ///\brief
  ///Cache helper function
  PARTICLE_IMPEXP bool FillFreeParticleCache();

  ///\brief
  ///Simulation function; called by the handling task
  PARTICLE_IMPEXP void HandleParticles(float dtime);

  ///\brief
  ///Simulation function of a single particle
  inline bool HandleSingleParticle(ParticleExt_t *pParticle, float fDeltaTime);

  ///\brief
  ///Respawns all particles
  ///
  ///\param bUseOldCount
  ///  if true, the current number of active particles is used, otherwise the predicted count at spawn time
  PARTICLE_IMPEXP void RespawnAllParticles(bool bUseOldCount);

  ///\brief
  ///   Moves the position of the emitter to the current position of particle group
  ///
  /// The particle system does by default interpolate the emitter position over time. This means that moving
  /// the particle group in one frame will by default still cause particles to be emitted at the old position and
  /// possibly between the old and new position.
  ///
  /// If you want to move a particle effect to a new position, and want to make sure that all newly created
  /// particles of this effect are created at the new position as well, then simply call this method after you have
  /// moved the particle group.
  PARTICLE_IMPEXP void TeleportSpawnPosition();


  ///\brief
  ///Loops through the particle array and counts the alive particles
  PARTICLE_IMPEXP int CountActiveParticles() const;

  ///\brief
  ///Initializes a single new particle (called by the emitter)
  inline void InitSingleParticle(ParticleExt_t *pParticle);

  ///\brief
  ///Copies over properties from source particle
  inline bool CreateParticleFromSource(const ParticleExt_t *pSrcParticle, float fTimeDelta, int iCopyFlags);

  ///\brief
  ///Recompute the bounding box using all particles
  PARTICLE_IMPEXP void InflateBoundingBox(bool bForceValid);

  ///\brief
  ///Waits for the simulation task (if there is a task)
  inline void EnsureUpdaterTaskFinished()
  {
    VThreadedTask *pUpdaterTask = GetUpdateTask(); // this is not necessarily m_pHandlingTask
    if (pUpdaterTask != NULL && pUpdaterTask->GetState() != TASKSTATE_UNASSIGNED)
      Vision::GetThreadManager()->WaitForTask(pUpdaterTask, true);
  }

  ///\brief
  ///Ensures the visibility bounding box is updated
  inline void UpdateVisibilityBoundingBox() 
  {
    // make sure the task has finished because it might also affect the bounding box
    EnsureUpdaterTaskFinished();
    m_BoundingBox.setInvalid();
    InflateBoundingBox(true);
  }

  ///\brief
  ///Returns a reference to the current world space bounding box
  inline const hkvAlignedBBox& BoundingBox() const  
  {
    return m_BoundingBox;
  }

  ///\brief
  ///Render helper function
  inline void RenderBoundingBox(VColorRef iColor = V_RGBA_WHITE);
  
  ///\brief
  ///Debug render helper
  PARTICLE_IMPEXP void RenderParticleBoundingBoxes();

  ///\brief
  ///Returns the highwater mark in the particle array
  inline int GetActiveParticleCount() const {return m_iValidCount;}

  ///\brief
  ///Prepares some values for faster access
  inline void SetPerFrameConstants(float dtime);

  ///\brief
  ///Sets the animation frame and weighting for a single particle
  inline void SetParticleAnimFrame(ParticleExt_t *pParticle, float fFrame, bool bWrapAround = false);

  ///\brief
  ///Sets the visible status of this layer (obsolete since SetVisibleBitmask)
  inline void SetVisible(bool bStatus);

  /// \brief
  ///   Explicitly sets the particle (DISTORTION_TYPE_*) distortion type, which determines how the .
  ///   distortion vector in the particle effect is handled.
  /// \param eDistortionType
  ///   Distortion Type to set. Can be any of the valid options such as DISTORTION_TYPE_VELOCITY,
  ///   DISTORTION_TYPE_CUSTOM, etc.
  inline void SetDistortionType(unsigned char eDistortionType);

  /// \brief
  ///   Returns the current Distortion Type.
  ///   
  /// \returns
  ///   unsigned char: Current Distortion Type
  inline unsigned char GetDistortionType() const;

  ///\brief
  ///Sets the visible filter bitmask for this layer and for on-destroy children
  PARTICLE_IMPEXP void SetVisibleBitmask(unsigned int iMask);

  ///\brief
  ///Returns the updated bounding box. The return pointer can be NULL
  inline const hkvAlignedBBox *GetCurrentBoundingBox();

  ///\brief
  ///Sets simulation flag. If false, the layer is also simulated when it is not visible; If true, the layer will only be simulated when visible.
  inline void SetHandleWhenVisible(bool bStatus) {m_bHandleWhenVisible=bStatus;}

  ///\brief
  ///Gets simulation flag. If false, the layer is also simulated when it is not visible; If true, the layer is only simulated when it is visible.
  inline bool GetHandleWhenVisible() { return m_bHandleWhenVisible; }

  ///\brief
  ///Returns true if the particle group was rendered in the last frame. Returns false otherwise.
  inline bool WasRecentlyRendered() { return (m_iLastRenderFrame == Vision::Game.GetUpdateSceneCount() - 1); }

  ///\brief
  ///Sets an ambient color that might contribute to the per-frame color
  PARTICLE_IMPEXP void SetAmbientColor (VColorRef iColor);

  ///\brief
  ///Re-evaluates the scene brightness, e.g. from the lightgrid
  PARTICLE_IMPEXP void EvaluateSceneBrightness();

  ///\brief
  ///Adds constraints from the passed source list and optionally performs early out using the constraint's Influences function
  PARTICLE_IMPEXP int AddRelevantConstraints(const VisParticleConstraintList_cl *pSrcList, bool bCheckInfluence=true);

  ///\brief
  ///Adds a single constraint and optionally performs early out using the constraint's Influences function
  PARTICLE_IMPEXP bool AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence=true);

  ///\brief
  ///Detach the passed constraint from this layer
  inline bool RemoveConstraint(VisParticleConstraint_cl *pConstraint);

  ///\brief
  ///Detach all constraints from this layer
  inline void RemoveAllConstraints();

  ///\brief
  ///Perform constraint simulation
  PARTICLE_IMPEXP void HandleAllConstraints(float dtime);

  ///\brief
  ///Return list of all attached constraints
  inline const VisParticleConstraintList_cl* GetConstraintList() const {return &m_Constraints;}
  ///\brief
  ///Copy passed constraint list
  inline void CopyConstraintList(const VisParticleConstraintList_cl* pSrc) {m_Constraints.CopyFrom(pSrc);}

  ///\brief
  ///Sets a wind speed vector. Simple wind can be applied with no performance impact
  PARTICLE_IMPEXP void SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace=false);

  ///\brief
  ///Debug flag
  inline void SetRenderConstraints(bool bStatus) {m_bRenderConstraints=bStatus;}

  ///\brief
  /// Internal function triggered by the engine callback
  void ReassignShader(bool bRecreateFX);

public:
  V_DECLARE_SERIALX( ParticleGroupBase_cl, PARTICLE_IMPEXP );
  ///\brief
  ///Binary serialization
  PARTICLE_IMPEXP void SerializeX( VArchive &ar );
  PARTICLE_IMPEXP void OnDeserializationCallback(const VSerializationContext &context);

  ///\brief
  ///Set the mesh emitter to the passed mesh
  bool SetMeshEmitterEntity(VisBaseEntity_cl* pEntity);

  ///\brief
  ///Set the SizeMultiplier. If DistortionSizeMode is used then this is the distortion vector. The distortion vector is typically orthogonal to the normal
  ///The w component is not used
  inline void SetSizeMultiplier(const hkvVec4& vSizeMultiplier) {m_vSizeMultiplier = vSizeMultiplier;}

  ///\brief
  ///Get the SizeMultiplier. If DistortionSizeMode is used then this is the distortion vector. The distortion vector is typically orthogonal to the normal
  ///The w component is not used
  inline hkvVec4 GetSizeMultiplier() {return m_vSizeMultiplier;}

protected:
  // VisObject3D_cl class overrides
  virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;
#ifdef SUPPORTS_SNAPSHOT_CREATION
  virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif
  virtual void ModSysNotifyFunctionCommand(int command) HKV_OVERRIDE;

private:
  friend class VisParticleEffect_cl;
  friend class VisParticleGroupManager_cl;
  PARTICLE_IMPEXP ParticleGroupBase_cl(VisParticleGroupDescriptor_cl *pDescr, ParticleGroupBase_cl *pParent, int iGeneration, unsigned int uiRandomSeed);
  friend class VisParticleGroupDescriptor_cl;
  friend class VisParticleEmitter_cl;
  friend class VisParticleEffectFile_cl;
  friend class HandleParticlesTask_cl;

  void InitGroup(bool bSpawnParticles, int iGeneration=0);
  void CopyParentPosition();
  void OnDescriptorChanged();
  void FadeDistancesFromDesc();
  void Finalize();
  void RemoveUpdaterTaskRecursive(ParticleGroupBase_cl *pGroup);

  inline bool AddParticleToCache(ParticleExt_t *pParticle);

  ParticleGroupBase_cl *m_pParentGroup; ///< no smart pointer because of dead-lock

  hkvVec3 m_vDirection;
  hkvVec3 m_vUpDir;
  hkvVec3 m_vRightDir;                  ///< cached coordinate system (important if cone angle>0)
  float m_fScaling;                     ///< scaling of the group
  float m_fAnimFrameCount;              ///< number of (used) animation frames

  // lookup texture
  VColorRef *m_pColorLookup;            ///< points to m_spColorLookup->GetDataPtr for faster lookup
  VisBitmapPtr m_spColorLookup;         ///< copy of VisParticleGroupDescriptor_cl::m_spColorLookup for faster access
  VCurve2DPtr m_spSizeCurve;            ///< particle size curve [0..1] maximum value = maximum size
  VCurve2DPtr m_spAnimCurve;            ///< particle animation curve

  float m_fColorBitmapSizeX;            ///< bitmap X size for faster lookups
  float m_fColorBitmapSizeY;            ///< bitmap Y size for faster lookups

  // animation
  VIS_PARTICLE_ANIMATION_MODE_e m_eParticleAnimMode; ///< animation mode of particles
  short m_iMaxAnimFrame;                ///< from descriptor m_iUsedAnimFrames-1, similar to m_fAnimFrameCount
  short m_iChildIndex;                  ///< index within parent's collection

  // rotation
  VIS_PARTICLE_ROTATION_MODE_e m_eRotationMode; ///< particle rotation behaviour
  VCurve2DPtr m_spRotationCurve;        ///< rotation angle over time, mode PARTICLE_ROTATION_ANGLECURVE

  // lighting
  float m_fLastTimeOfDayUpdate;         ///< stores when the particle was re-lit using the time-of-day to check when to do it again
  bool m_bEvaluateBrightnessNextFrame;  ///< flag to evaluate the brightness next frame
  VColorRef m_InstanceColor;            ///< internal final color result
  VColorRef m_AmbientColor;             ///< color set via VisParticleEffect_cl::SetAmbientColor

  // cache free particles
  short m_iCachedParticleCount, m_iMaxCachedParticleCount;  ///< number of currently cached free particles
  unsigned short *m_piCachedParticle;   ///< pointer to cached particles
  unsigned short m_iCachedParticleNoAlloc[PARTICLECACHESIZE_NOALLOC];

  // group instance vars
  float m_fLifeTime;                    ///< group lifetime
  float m_fTimeScale;                   ///< time scaling value
  float m_fInitialDelay;                ///< delay before simulation starts

  bool m_bApplyTimeOfDayLight;          ///< if true the time-of-day light will be added to the particles color
  int m_iRemainingParticleCount;        ///< if greater or equal to 0, this group has a restricted amount of particles
  bool m_bInfiniteLifeTime;             ///< indicates infinite lifetime of group
  
  bool m_bIsDead;                       ///< marks the group as dead (can be removed if no more references)
  bool m_bPaused;                       ///< marks the group as paused, i.e. no more new particles are spawned
  bool m_bHalted;                       ///< marks the group as halted, i.e. even the particles do not move
  bool m_bUpdateLifetimeIfInvisible;

  hkvVec3 m_vFrameWind;
  hkvVec3 m_vWindSpeed;
  hkvVec3 m_vFrameWindNoInertia;        ///< wind parameters
  float m_fFrameFriction;               ///< the friction multiplier in this frame
  short m_iGeneration;                  ///< recursion depth for recursively created groups (m_spOnDestroyCreateGroup)
  bool m_bBBoxValid, m_bVisibilityUpdate; ///< internal visibility status flags
  hkvAlignedBBox m_BoundingBox;         ///< absolute world space bounding box
  float m_fBBoxUpdateTimePos;           ///< current timer for updating the bounding box (related to VisParticleGroupDescriptor_cl::m_fDynamicInflateInterval)
  hkvVec4 m_vSizeMultiplier;            ///< for the distortion size mode

  // group descriptor
  VisParticleEffect_cl *m_pParentEffect;         ///< pointer to the particle effect this groups belongs to
  VisParticleGroupDescriptorPtr m_spDescriptor;  ///< pointer to group descriptor
  ParticleGroupBasePtr m_spOnDestroyCreateGroup; ///< group to create destroyed particles

  // constraints
  bool m_bHandleConstraints;                     ///< indicates whether constraints should be handled
  bool m_bRenderConstraints;                     ///< render constraints for debuging
  bool m_bDistortionPlaneAligned;                ///< calculate particle normals from direction

  unsigned int m_iConstraintAffectBitMask;       ///< Constraint filter bitmask
  VisParticleConstraintList_cl m_Constraints;    ///< List of attached constraints

  // emitter
  VisParticleEmitterPtr m_spEmitter;             ///< Pointer to the emitter
  VisBaseEntity_cl* m_pEmitterMeshEntity;        ///< Temporary variable use when deserialization

  // misc
  bool m_bHandleWhenVisible;                     ///< if enabled, simulation will only be performed if group is visible
  bool m_bMovesWithEmitter;                      ///< internal flag evaluated from SetLocalFactors
  bool m_bRepeatLifetime;                        ///< from descriptor
  bool m_bInertiaAffectsGravity;                 ///< from descriptor

  hkvVec3 m_vOldPos;                             ///< cached old position to calculate the delta when new position is set
  hkvVec3 m_vGroupMoveDelta;
  hkvVec3 m_vGroupMoveDeltaAccum;                ///< internal accumulation vectors
  float m_fLocalFactorStart,m_fLocalFactorDiff;  ///< amount of how much particles move with the emitter at start and end of lifecycle
  volatile int m_iValidCount;                    ///< current number of valid particles

  float m_fTransformationCurveTime, m_fTransformationCurveSpeed;
  char m_eTopology;                              ///< casted to VIS_PARTICLE_TOPOLOGY_e
  bool m_bHasTransformationCurves, m_bHasEvents;
  bool m_bWindInLocalSpace;                      ///< apply wind in local space (not default)
  hkvAlignedBBox m_MeshBoundingBox;              ///< usually set zero, but maybe larger for geometry particles

  short m_iTrailIndex;
  
  // camera
  bool m_bAttachedToCam; ///< Deprecated
  hkvVec3 m_vCamRelPos;  ///< Deprecated

  // task
  HandleParticlesTask_cl *m_pHandlingTask;       ///< pointer to simulation task
};



///\brief
/// This class represents a single particle effect instance. It is the native counterpart to vForge's Particle System shape
///
///An effect instance can be instantiated via VisParticleEffectFile_cl::CreateParticleEffectInstance.
///An instance of this class holds an array of active effect layer instances (each of type ParticleGroupBase_cl)
///which correspond to instances of particle layers in the editor.
///This class inherits positioning etc. from its parent VisObject3D_cl class.
///
///\see
///  VisParticleEffectFile_cl
class VisParticleEffect_cl : public VisObject3D_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisParticleEffect_cl,  PARTICLE_IMPEXP );

  ///\brief
  ///Constructor, same as VisParticleEffectFile_cl::CreateParticleEffectInstance
  PARTICLE_IMPEXP VisParticleEffect_cl(VisParticleEffectFile_cl *pEffect, const hkvVec3& vPos, const hkvVec3& vOri, unsigned int uiRandomSeed);
  ///\brief
  ///Constructor, same as VisParticleEffectFile_cl::CreateParticleEffectInstance
  PARTICLE_IMPEXP VisParticleEffect_cl(VisParticleEffectFile_cl *pEffect, const hkvVec3& vPos, const hkvVec3& vOri, float fScaling, unsigned int uiRandomSeed);

  ///\brief
  ///Internal creation helper
  ///\internal
  PARTICLE_IMPEXP void CreateFromDescriptors(VisParticleGroupDescriptor_cl **pDescList,int iDescCount);

  ///\brief
  ///Destructor
  PARTICLE_IMPEXP virtual ~VisParticleEffect_cl();

  ///\brief
  ///Sets the visible status. Obsolete since SetVisibleBitmask
  PARTICLE_IMPEXP void SetVisible(bool bStatus);
 
  ///\brief
  ///Sets the visible filter bitmask (on all layers)
  PARTICLE_IMPEXP void SetVisibleBitmask(unsigned int iMask);

  ///\brief
  ///Returns the visible state set via SetVisible
  inline bool IsVisible() const {return m_bVisible;}
  
  ///\brief
  ///Obsolete; Wraps around GetObjectKey
  inline const char *GetEffectKey() const {return GetObjectKey();}
  ///\brief
  ///Obsolete; Wraps around SetObjectKey
  inline void SetEffectKey(const char *szKey) {SetObjectKey(szKey);}

  ///\brief
  /// Gets the random number generator base seed for this particle effect. This can
  /// be defined in vForge. If 0, a true random seed will be used (default).
  inline unsigned int GetRandomBaseSeed() const { return m_uiRandomBaseSeed; }

  ///\brief
  /// Moves this instance by specified delta. Optionally moves all particle positions (used by vForge positioning)
  PARTICLE_IMPEXP void IncPosition(const hkvVec3& vDelta, bool bMoveParticles = false);

  ///\brief
  /// Set scaling factor for all layers
  PARTICLE_IMPEXP void SetScaling(float fScale);

  ///\brief
  // set additive ambient color
  PARTICLE_IMPEXP void SetAmbientColor(VColorRef iColor);

  ///\brief
  // Re-evaluate the scene brightness
  PARTICLE_IMPEXP void EvaluateSceneBrightness();

  ///\brief
  // Calls respective function on all layers
  PARTICLE_IMPEXP void UpdateVisibilityBoundingBox();

  ///\brief
  ///Sets the pause status. If paused, the emitter does not spawn new particles
  PARTICLE_IMPEXP void SetPause(bool bStatus);

  ///\brief
  ///Returns the current pause status.
  inline bool IsPaused() const {return m_bPaused;}

  ///\brief
  ///Sets the halted status. When halted, existing particles do not move anymore
  PARTICLE_IMPEXP void SetHalted(bool bStatus);

  ///\brief
  ///Returns the current halted status.
  inline bool IsHalted() const {return m_bHalted;}

  ///\brief
  /// Stops the emitters and waits until all particles have disappeared. Then removes the particle effect
  PARTICLE_IMPEXP void SetFinished();

  ///\brief
  /// If set to true, the light of the time-of-day component will be added to the particles color.
  PARTICLE_IMPEXP void SetApplyTimeOfDayLight(bool bApply);

  ///\brief
  /// Returns whether time-of-day light is supposed to be applied on this effect.
  PARTICLE_IMPEXP bool GetApplyTimeOfDayLight(void) const;

  ///\brief
  /// When this flag is enabled (default) then this instance is deleted as soon as it is dead (all layers finished).
  ///
  ///Disable this flag to be able to call Restart()
  ///
  ///\param bStatus
  /// New status 
  inline void SetRemoveWhenFinished(bool bStatus) {m_bRemoveDeadLayers=bStatus;}

  ///\brief
  /// Returns the flag that has been set via SetRemoveWhenFinished. Default is true
  inline bool GetRemoveWhenFinished() const {return m_bRemoveDeadLayers;}

  ///\brief
  /// Overridden function. Removes this instance from the simulation collection
  PARTICLE_IMPEXP virtual void DisposeObject() HKV_OVERRIDE;

  ///\brief
  /// Obsolete; Wraps around DisposeObject
  inline void Dispose() {DisposeObject();}

  ///\brief
  /// Changes the simulation state for all layers. If false, the particle effect is also simulated when not visible; If true, the particle effect is simulated only when visible.
  PARTICLE_IMPEXP void SetHandleWhenVisible(bool bStatus);

  ///\brief
  /// Returns true if ALL of its particle groups:
  ///  - are marked as handled only when visible (m_bHandleWhenVisible==true)
  ///  - do not have to update their lifetime if not visible (m_bUpdateLifetimeIfInvisible==false)
  bool IsUpdatedOnlyWhenVisible();

  ///\brief
  /// Returns true if any of its particle groups was rendered in the last frame. Returns false otherwise.
  bool WasRecentlyRendered();

  ///\brief
  /// Restarts the particle effect. Requires SetRemoveWhenFinished(false)
  PARTICLE_IMPEXP void Restart();

  ///\brief
  /// Sets the emitter intensity on all layers. This factor modulates the spawn frequency of the emitters  
  ///
  ///\param fIntensity
  /// The new intensity value. Should be in 0..1 range
  ///
  ///An intensity of 1.0f (default) spawns particles at full frequency as defined in the editor. An intensity of 0.0f
  ///pauses the emitter. However, in that case SetPaused(true) should be used.
  PARTICLE_IMPEXP void SetIntensity(float fIntensity);

  ///\brief
  ///Re-spawns all particles in all layers
  ///
  ///\param bUseOldCount
  ///  if true, the current number of active particles is used, otherwise the predicted count at spawn time
  PARTICLE_IMPEXP void RespawnAllParticles(bool bUseOldCount=true);
  
  ///\brief
  ///   Moves the position of the emitters to the current position of the corresponding particle groups
  ///
  /// The particle system does by default interpolate the emitter position over time. This means that moving
  /// the particle group in one frame will by default still cause particles to be emitted at the old position and
  /// possibly between the old and new position.
  ///
  /// If you want to move a particle effect to a new position, and want to make sure that all newly created
  /// particles of this effect are created at the new position as well, then simply call this method after you have
  /// moved the particle effect.
  PARTICLE_IMPEXP void TeleportSpawnPosition();

  ///\brief
  /// Get the local bounding box (merged from all children)
  PARTICLE_IMPEXP bool GetLocalBoundingBox(hkvAlignedBBox &destBox) const;

  ///\brief
  /// Get the updated bounding box (merged from all children)
  PARTICLE_IMPEXP bool GetCurrentBoundingBox(hkvAlignedBBox &destBox) const;

  ///\brief
  ///Adds constraints from the passed source list to all layers and optionally performs early out using the constraint's Influences function
  PARTICLE_IMPEXP void AddRelevantConstraints(const VisParticleConstraintList_cl *pSrcList, bool bCheckInfluence=true);

  ///\brief
  ///Adds a single constraint to all layers and optionally performs early out using the constraint's Influences function
  PARTICLE_IMPEXP void AddConstraint(VisParticleConstraint_cl *pConstraint, bool bCheckInfluence=true);

  ///\brief
  ///Detach the passed constraint from all layers
  PARTICLE_IMPEXP void RemoveConstraint(VisParticleConstraint_cl *pConstraint);
  ///\brief
  ///Detach all constraints from all layers
  PARTICLE_IMPEXP void RemoveAllConstraints();

  ///\brief
  ///Sets a wind speed vector for all layers. Simple wind can be applied with no performance impact
  PARTICLE_IMPEXP void SetWindSpeed(const hkvVec3& vWind, bool bApplyInLocalSpace=false);

  ///\brief
  ///Associate all layers with emitter type 'mesh' with the passed entity instance
  PARTICLE_IMPEXP void SetMeshEmitterEntity(VisBaseEntity_cl *pEntity);

  ///\brief
  /// Performs debug rendering
  PARTICLE_IMPEXP void RenderParticleBoundingBoxes();

  ///\brief
  /// Performs the simulation for all layers
  PARTICLE_IMPEXP void Tick(float fTimeDiff);

  ///\brief
  ///Indicates whether the lifetime of all layers is used up
  inline bool IsLifeTimeOver() const
  {
    for (int i = 0; i < m_iGroupCount; ++i)
    {
      ParticleGroupBase_cl *pLayer = m_spGroups[i];
      if (pLayer == NULL || pLayer->IsDead())
        continue;

      if (!pLayer->IsLifeTimeOver())
        return false;
    }
    return true;
  }

  ///\brief
  /// Indicates whether this instance is ready for removal
  inline bool IsDead() const {return m_bRemoveDeadLayers && (m_iGroupCount==0);}

  ///\brief
  /// Return the (remaining) number of layers in this effect
  inline int GetParticleGroupCount() const {return m_iGroupCount;}

  ///\brief
  /// Return the layer with specified index
  inline ParticleGroupBase_cl *GetParticleGroup(int iIndex) const {return m_spGroups[iIndex];}

  ///\brief
  ///Returns the remaining lifetime of this effect in seconds. If any layer has infinite lifetime, this function returns -1.0f
  inline float GetRemainingLifeTime() const
  {
    float fMax = 0.f;
    for (int i = 0; i < m_iGroupCount; ++i)
    {
      ParticleGroupBase_cl *pLayer = m_spGroups[i];
      if (pLayer==NULL || pLayer->IsDead())
        continue;

      if (pLayer->m_bInfiniteLifeTime)
        return -1.f;

      float fLayerLifeTime = pLayer->GetRemainingLifeTime();
      fMax = hkvMath::Max(fMax,fLayerLifeTime);
    }
    return fMax;
  }

  ///\brief
  /// Adds remaining layers to the passed collection
  inline void AddGroupsToCollection(VisParticleGroupCollection_cl &dest)
  {
    dest.EnsureSize(dest.GetSize()+m_iGroupCount);
    for (int i = 0; i < m_iGroupCount; ++i)
    {
      ParticleGroupBase_cl *pLayer = m_spGroups[i];
      if (pLayer==NULL || pLayer->IsDead())
        continue;

      dest.AppendEntryFast(pLayer);
    }
  }

  ///\brief
  /// Returns the particle effect resource that was used to create this instance
  ///
  ///\returns
  /// The particle effect resource 
  inline VisParticleEffectFile_cl* GetSourceEffect() const {return m_spSourceFXFile;}

  ///
  /// @name Network related
  /// @{
  ///

  PARTICLE_IMPEXP int GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList);

  ///
  /// @}
  ///
  

public:
  // VisObject3D_cl class overrides
  PARTICLE_IMPEXP virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual BOOL AddComponent(IVObjectComponent *pComponent) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void OnReposition(const VisZoneRepositionInfo_t &info, const hkvVec3 &vLocalPos) HKV_OVERRIDE;
#ifdef SUPPORTS_SNAPSHOT_CREATION
  PARTICLE_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif
  PARTICLE_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;
  PARTICLE_IMPEXP VisParticleEffect_cl();

private:
  PARTICLE_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void OnSerialized(VArchive &ar) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual VBool WantsDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE
  {
    // only call it in this case as the emitter mesh gets lost otherwise
    return context.m_eType == VSerializationContext::VSERIALIZATION_ARCHIVE ? TRUE : FALSE;
  }
  PARTICLE_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE;
  void CheckForRestartComponent();
  void EnsureComponentIDsRegistered();

  inline void UpdateLightGrid()
  {
    if (!m_bUseLightgrid) return;
    hkvVec3 vPos = GetPosition();
    VLightGrid_cl *pLightGrid = this->GetRelevantLightGrid();
    if (pLightGrid)
      pLightGrid->GetColorsAtPositionI((hkvVec3& )vPos,m_OwnLGColors);
  }

  friend class HandleParticlesTask_cl;
  friend class ParticleGroupBase_cl;
  friend class VisParticleEffectFile_cl;
  friend class VisParticleGroupDescriptor_cl;

  void OnSingleGroupFinished(ParticleGroupBase_cl *pFinishedGroup);
  VisParticleEffectFilePtr m_spSourceFXFile;

  int m_iGroupCount;                ///< remaining number of groups
  unsigned int m_uiRandomBaseSeed;  ///< if non-zero, contains the custom based seed for random number generation for this particle effect
  ParticleGroupBasePtr *m_spGroups; ///< array of smart pointers
  bool m_bPaused, m_bHalted, m_bVisible, m_bUseLightgrid; ///< status flags
  bool m_bRemoveDeadLayers;         ///< if enabled (default), all layers that have finished their simulation will be removed
  bool m_bAnyMeshEmitter;           ///< indicates whwther any layer uses an entity mesh emitter (special treatment in the main thread update)
  hkvVec3 m_OwnLGColors[6];         ///< own cached light grid colors
};


///\brief
/// Collection class for refcounted particle effect instances
///
///This class adds convenience functions to perform simulation (Tick) or Pruge the list
class VisParticleEffectCollection_cl : public VRefCountedCollection<VisParticleEffect_cl>
{
public:

  ///\brief
  /// Performs a simulation tick on all instances. 
  PARTICLE_IMPEXP void Tick(float fTimeDelta, bool bPurgeDead=true);

  ///\brief
  /// Re-evaluate scene brightness for all instances
  PARTICLE_IMPEXP void EvaluateSceneBrightness();

  ///\brief
  /// Purges all instances that have finished simulation
  PARTICLE_IMPEXP void Purge(bool bDeadOnly=false);

  ///\brief
  /// Reassign all the shaders
  PARTICLE_IMPEXP void ReassignShader(bool bRecreateFX);

  ///\brief
  /// return the first occurance of an effect with specified key in this collection
  ///
  ///This is very similar to VisGame_cl::SearchEntity etc.
  ///
  ///\param szKey
  ///  Key to search for
  ///
  ///\param pStoreArray
  ///  Optional array that receives all hits (otherwise just the first hit is returned)
  ///
  ///\return
  ///  First hit with specified key, or NULL if key wasn't found
  PARTICLE_IMPEXP VisParticleEffect_cl *FindByKey(const char *szKey, DynArray_cl<VisParticleEffect_cl *> *pStoreArray=NULL) const;

};



/// \brief
///   Implements IVNetworkSynchronizationGroup as a synchronization group that synchronizes particle effects. This version supports interpolation
///
/// There is one global instance: g_Instance
class VNetworkParticleEffectGroupI : public IVNetworkSynchronizationGroup
{
public:
  /// \brief
  ///   Data history
  class VHistoryDataParticleEffect : public IVNetworkSynchronizationGroupInstanceData
  {
  public:
    VisDataHistory_cl<BYTE, 3, VStepFunction<BYTE> > m_particleFlagHistory;
  };

  /// \brief
  ///   Flags defining the status of the particle effect group
  enum VParticleFlags
  {
    VCF_PAUSED = V_BIT(0),
    VCF_HALTED = V_BIT(1),
  };

  PARTICLE_IMPEXP virtual const char *GetName() const HKV_OVERRIDE {return "ParticleEffectSettings";}
  PARTICLE_IMPEXP virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_ParticleEffect;}
  PARTICLE_IMPEXP virtual IVNetworkSynchronizationGroupInstanceData* CreatePerInstanceData() const HKV_OVERRIDE {return (new VHistoryDataParticleEffect);}
  PARTICLE_IMPEXP virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta) HKV_OVERRIDE;

  PARTICLE_IMPEXP static VNetworkParticleEffectGroupI g_Instance; ///< paused flag, halted flag
};


/// \brief
///   Similar to VNetworkParticleEffectGroupI but without interpolation
///
/// There is one global instance: g_Instance
class VNetworkParticleEffectGroup : public IVNetworkSynchronizationGroup
{
public:
  /// \brief
  ///   Flags defining the status of the particle effect group
  enum VParticleFlags
  {
    VCF_PAUSED = V_BIT(0),
    VCF_HALTED = V_BIT(1),
  };

  PARTICLE_IMPEXP virtual const char *GetName() const HKV_OVERRIDE {return "ParticleEffectSettings";}
  PARTICLE_IMPEXP virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_ParticleEffect;}
  PARTICLE_IMPEXP virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  PARTICLE_IMPEXP virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;

  PARTICLE_IMPEXP static VNetworkParticleEffectGroup g_Instance; ///< paused flag, halted flag
};


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.inl>

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
