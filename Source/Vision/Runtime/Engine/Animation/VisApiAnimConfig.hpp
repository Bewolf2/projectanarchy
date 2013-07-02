/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimConfig.hpp

#ifndef VIS_ANIM_CONFIG_HPP_INCLUDED
#define VIS_ANIM_CONFIG_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimFinalSkeletalResult.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexDeformerStack.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimControl.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiSkeletalAnimControl.hpp>


// predefines
class VisAnimConfig_cl;
typedef VSmartPtr<VisAnimConfig_cl>  VisAnimConfigPtr;

class VDynamicMesh;
typedef VSmartPtr<VDynamicMesh> VDynamicMeshPtr;

class VAnimatedCollisionMesh;
typedef VSmartPtr<VAnimatedCollisionMesh> VAnimatedCollisionMeshPtr;

// config flag defines
#define APPLY_MOTION_DELTA          0x00000001
#define REUSE_VERTEX_POS_NORM_TAN   0x00000002 ///< keep an extra copy of vertex position, normal and tangent(all vertices)
#define KEEP_EXTRA_SYSTEMBUFFER     0x00000002
#define REUSE_VERTEX_POS            0x00000004 ///< keep an extra copy of vertex positions(only identity vertices)
#define KEEP_EXTRA_COLLISIONBUFFER  0x00000004
#define MULTITHREADED_ANIMATION     0x00000008
#define HINT_FREQUENT_REUSE         0x00000010
#define USE_BLENDPOINTS_FOR_NORMALS 0x00000020
#define RENORMALIZE_NORMALS         0x00000040

#define VSPURS_SKINNING_JOB_PRIORITY 1

// GetCurrentVertexResult flags
#define UPDATE_VERTEX_POSITION                0x00000001 ///< uses the same buffer as the collision mesh
#define UPDATE_VERTEX_POSITION_NORMAL_TANGENT 0x00000002
#define COPY_RESULT_TO_RENDERBUFFER           0x00000004
#define COPY_RESULT_TO_COLLISIONBUFFER        0x00000001
#define ANIMCONFIG_UNKNOWN_USAGE              -1

class VisVertexAnimDeformer_cl; class VisMorphingDeformer_cl; class VisAnimFinalSkeletalResult_cl; class VisVertexAnimControl_cl; class VisSkeletalAnimControl_cl; class VisVertexAnimationTask_cl;

typedef VSmartPtr<VisVertexAnimationTask_cl> VisVertexAnimationTaskPtr;

//Bone limitation for Hardware skinning
#define HARDWARESKINNING_TRACKREGISTERSTART_DX9 70
#define HARDWARESKINNING_MAX_BONECOUNT_PC_DX9 (256-HARDWARESKINNING_TRACKREGISTERSTART_DX9)/3 // = 62
#define HARDWARESKINNING_MAX_BONECOUNT_PC_DX11 256
#define HARDWARESKINNING_MAX_BONECOUNT_XBOX HARDWARESKINNING_MAX_BONECOUNT_PC_DX9
#define HARDWARESKINNING_MAX_BONECOUNT_XBOX_MEMEXPORT 82
#define HARDWARESKINNING_MAX_BONECOUNT_XBOX_MEMEXPORT_QUAT 123
#define SPUSKINNING_MAX_BONECOUNT_PS3 256
#define HARDWARESKINNING_MAX_BONECOUNT_WIIU (256-HARDWARESKINNING_TRACKREGISTERSTART_DX9)/3 // = 62


/// \brief
///   Class that holds an animation setup including skeletal and vertex animations.
/// 
/// The skeletal part consists of the final result, mixer hierarchies and skeletal controls.
/// 
/// The vertex part consists of the modifier stack that performs vertex animations and skinning.
/// 
/// Different entities that are animated exactly synchronously and that use the same dynamic mesh
/// can share the same config, so the video memory is shared and the skinning is only
/// performed once.
/// 
/// Different configs can share the same final result if no remapping is needed (they have the same
/// skeleton).
class VisAnimConfig_cl : public VRefCounter, public VisTypedEngineObject_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the anim config class.
  /// 
  /// \param pMesh
  ///   Mesh the animations are played on.
  /// 
  /// \param iFlags
  ///   configs property bitflags:
  ///   \li APPLY_MOTION_DELTA is used to add motion delta to entity automatically (if available in
  ///     the animation resource).
  /// 
  ///   \li HINT_FREQUENT_REUSE is used as a performance hint. It is disabled by default, which
  ///     allows for best performance in situations where VisAnimConfig_cl objects are unique for
  ///     each character. In situations where you have many characters sharing the same
  ///     VisAnimConfig_cl object, or where the same character will be rendered many times with the
  ///     same animation state (e.g. if there are many render contexts, or animations are updated
  ///     infrequently), it is strongly recommended to set this flag.
  /// 
  ///   \li KEEP_EXTRA_COLLISIONBUFFER should be used where skinning/animation results will have to
  ///     be touched by the CPU during a frame (e.g. for polygon-accurate collision detection).
  ///     This will result in the vertices of an animated object being cached
  ///     in system memory at the same time as the render data is generated. It will result in a
  ///     somewhat slower skinning process, but saves additional skinning passes for shadows or
  ///     collisions. Using this flag will result in all skinning computations being performed on
  ///     the CPU, and will therefore dramatically impact performance in situations where VS, SPU,
  ///     or memexport/stream out skinning is preferred.
  ///
  ///   \li MULTITHREADED_ANIMATION allows the Vision Engine to execute animation computations on multiple
  ///     threads in parallel. This typically results in significant performance gains on multi-core PCs
  ///     and game consoles.
  ///
  ///   \li KEEP_EXTRA_SYSTEMBUFFER tells the engine to always keep a separate system memory copy of the
  ///     transformed vertex data. This is only recommended if you need to query skinned/animated vertex
  ///     data. It will force the engine to use software skinning and require additional CPU-side copy
  ///     operations, so using this flag can dramatically impact performance.
  ///
  ///   \li USE_BLENDPOINTS_FOR_NORMALS tells the engine to use blendpoints for normals and tangents when
  ///     CPU skinning is used (by default, CPU skinning uses only the most significant bone for normal/tangent
  ///     transformations). This has no effect on other skinning modes, since other skinning modes always use
  ///     all bone influences for all vertex components.
  ///     Note that using this option will make skinning operations somewhat slower, so you should only
  ///     us it in cases where it is necessary.
  ///
  ///   \li RENORMALIZE_NORMALS: Only relevant in conjunction with USE_BLENDPOINTS_FOR_NORMALS. If this
  ///     flag is specified, normals and tangents will be renormalized after skinning. If you renormalize normals
  ///     and tangents in the vertex or pixel shader anyway, you will not have to use this flag.
  ///     Note that using this option will make skinning operations significantly slower (skinning will typically
  ///     run at about 30% of the original speed!), so you should only us it in cases where it is absolutely necessary.
  ///     This has no effect on skinning modes other than CPU skinning.
  ///
  VISION_APIFUNC VisAnimConfig_cl(VDynamicMesh* pMesh, int iFlags = APPLY_MOTION_DELTA );

  /// \brief
  ///   This constructor takes a skeleton instead of a mesh as input. Flags are the same as in the mesh variant. This version cannot be used for entities
  /// 
  VISION_APIFUNC VisAnimConfig_cl(VisSkeleton_cl* pSkeleton, int iFlags = APPLY_MOTION_DELTA );

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisAnimConfig_cl();

  ///
  /// @}
  ///

  ///
  /// @name Scene Update
  /// @{
  ///
  
  /// \brief
  ///   Returns frame number the config states have been updated the last time.
  /// 
  /// Compare this value against VisGame_cl::GetUpdateSceneCount.
  /// 
  /// \return
  ///   unsigned int: last updated frame.
  unsigned int GetLastUpdateSceneCount() const
  {
    return m_iLastFrameUpdatedState;
  }
  

  ///
  /// @}
  ///

  ///
  /// @name Mesh / Skeleton Access
  /// @{
  ///

  /// \brief
  ///   Returns the mesh associated with this config.
  /// 
  /// \return
  ///   VDynamicMesh*: used mesh.
  inline VDynamicMesh* GetMesh() const
  {
    return spMesh;
  }

  /// \brief
  ///   Returns the skeleton associated with this config.
  /// 
  /// \return
  ///   VisSkeleton_cl* pSkeleton: used skeleton.
  inline VisSkeleton_cl* GetSkeleton() const
  {
    return spSkeleton;
  }


  /// \brief
  ///   Sets a new mesh on the config.
  /// 
  /// The method only works for dynamic meshes with the same skeleton but different vertex count. Otherwise
  /// it asserts.
  /// 
  /// \param pMesh
  ///   new mesh.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetMesh(VDynamicMesh* pMesh);


  ///
  /// @}
  ///

  ///
  /// @name Input/output
  /// @{
  ///

  /// \brief
  ///   Sets the final result used by the config.
  /// 
  /// It is possible to share the same final result with different config that use the same
  /// skeleton.
  /// 
  /// The final result supports accessing remapped local space, object space and skinning space
  /// results.
  /// 
  /// \param pFinalResult
  ///   remapped result
  /// 
  /// \return
  ///   bool bResult: true if setting was successful.
  VISION_APIFUNC bool SetFinalResult(VisAnimFinalSkeletalResult_cl* pFinalResult);
  

  /// \brief
  ///   Returns the final result used by the config.
  /// 
  /// It can be used to get the result of one config and set it on another to animate them
  /// synchronously.
  /// 
  /// \return
  ///   VisAnimFinalSkeletalResult_cl* pFinalResult: remapped result.
  VISION_APIFUNC inline VisAnimFinalSkeletalResult_cl* GetFinalResult(void) const {return m_spFinalResult;}
  

  /// \brief
  ///   Sets the vertex modifier stack used by the config.
  /// 
  /// \param pVertexDeformerStack
  ///   pointer to modifier stack
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetVertexDeformerStack(VisVertexDeformerStack_cl* pVertexDeformerStack);


  /// \brief
  ///   Returns the modifier stack used by the config.
  /// 
  /// It can be used to get the vertex modifier stack if you want to add or remove modifiers.
  /// 
  /// \return
  ///   VisAnimFinalSkeletalResult_cl* pFinalResult: remapped result.
  VISION_APIFUNC inline VisVertexDeformerStack_cl* GetVertexDeformerStack(void) const {return m_spVertexDeformerStack;}

   
  /// \brief
  ///   Updates the whole state of the config.
  /// 
  /// This function is only called once per simulation frame by the engine. You should not call it
  /// manually.
  /// 
  /// Inside this function, the internal state of the final skeletal result and the vertex modifier
  /// state is updated.
  /// 
  /// \param fTimeDelta
  ///   passed time since last state update
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC virtual void UpdateAnimState(float fTimeDelta);

  /// \brief
  ///   Updates the vertex result of the config and returns the result.
  /// 
  /// The result is cached, i.e. only updated once per frame.
  /// 
  /// It performs skinning and vertex animations and writes the result into video- or system
  /// memory.
  /// 
  /// \param iUsage
  ///   usage flags; can be combined for better performance. The following flags are supported:
  ///   \li UPDATE_VERTEX_POSITION : Writes the result vertex positions to system memory,
  /// 
  ///   \li UPDATE_VERTEX_POSITION_NORMAL_TANGENT : Writes the result vertex normals and tangents
  ///     to system memory,
  /// 
  ///   \li COPY_RESULT_TO_RENDERBUFFER : Writes the result to video memory
  /// 
  ///   \li COPY_RESULT_TO_COLLISIONBUFFER : Same as UPDATE_VERTEX_POSITION since collision meshes
  ///     use the system memory result of the skinning.
  /// 
  /// \param bSync
  ///   synchronize this call with any skinning tasks running. Should be set to TRUE when called
  ///   from the main thread.
  /// 
  /// \return
  ///   VisVertexAnimResult_cl*: pointer to current (and updated) vertex result.
  VISION_APIFUNC VisVertexAnimResult_cl* GetCurrentVertexResult(int iUsage = UPDATE_VERTEX_POSITION, bool bSync = true);

  /// \brief
  ///   Causes the render buffer to be invalidated and thus forces it to be updated.
  VISION_APIFUNC void InvalidateRenderBufferContents();

  /// \brief
  ///   Returns the current skeletal local space result.
  /// 
  /// The result is cached, i.e. only updated once per frame.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to current skeletal local space result.
  VISION_APIFUNC const VisSkeletalAnimResult_cl* GetCurrentLocalSpaceResult();


  /// \brief
  ///   Evaluates a past/future skeletal local space result.
  /// 
  /// The function evaluates the final result with the new time and thus bypasses animation result
  /// caching.
  /// 
  /// The time is relative to the last state time +/- the specified time delta.
  /// 
  /// This function can be used to calculate velocities for bones when  switching to rag-doll mode.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \param fTimeDelta
  ///   time difference to current state time
  /// 
  /// \note
  ///   This function bypasses all animation result caching.
  /// 
  /// \note
  ///   This method will use the currently active settings. Some settings (e.g. ease-in and
  ///   ease-out or custom bones) that have already changed will thus not match.
  VISION_APIFUNC void GetLocalSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);


  /// \brief
  ///   Returns the current skeletal object space result.
  /// 
  /// The result is cached, i.e. only updated once per frame.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to current skeletal object space result.
  VISION_APIFUNC const VisSkeletalAnimResult_cl* GetCurrentObjectSpaceResult();


  /// \brief
  ///   Evaluates a past/future skeletal result in object space.
  /// 
  /// The function evaluates the final result with the new time and thus bypasses animation result
  /// caching.
  /// 
  /// This function can be used to calculate velocities for bones when  switching to rag-doll mode.
  /// 
  /// The time is relative to the last state time +/- the specified time delta.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \param fTimeDelta
  ///   time difference to current state time.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void GetObjectSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);
  
  
  /// \brief
  ///   Returns the current skeletal skinning space result.
  /// 
  /// The result is cached, i.e. only updated once per frame.
  /// 
  /// \return
  ///   VisSkeletalAnimResult_cl* pResult: pointer to current skeletal skinning space result.
  VISION_APIFUNC const VisSkeletalAnimResult_cl* GetCurrentSkinningSpaceResult();


  /// \brief
  ///   Evaluates evaluates a past/future skeletal result in skinning space.
  /// 
  /// The function evaluates the final result with the new time and thus bypasses animation result
  /// caching.
  /// 
  /// The time is relative to the last state time +/- the specified time delta.
  /// 
  /// \param pResult
  ///   Result that gets filled with the new result.
  /// 
  /// \param fTimeDelta
  ///   time difference to current state time.
  /// 
  /// \return
  ///   nothing.
  VISION_APIFUNC void GetSkinningSpaceResult(VisSkeletalAnimResult_cl* pResult, float fTimeDelta);


  /// \brief
  ///   Returns the current offset delta vector.
  /// 
  /// This vector is used to determine the move offset for meshes using offset delta in the current tick.
  /// 
  /// \param OffsetDelta
  ///   reference that receives the offset delta.
  /// 
  /// \return
  ///   bool bResult: returns false if the offset delta already has been updated this frame.
  VISION_APIFUNC bool GetOffsetDelta(hkvVec3& OffsetDelta) const;


  /// \brief
  ///   Returns the current offset rotation delta.
  /// 
  /// The rotation delta is a vector of yaw/pitch/roll Euler angles.
  /// 
  /// This vector is used to determine the rotation delta for meshes using rotation delta in this tick.
  /// 
  /// \param RotationDelta
  ///   reference that receives the rotation delta.
  /// 
  /// \return
  ///   bool bResult: returns false if the rotation delta already has been updated this frame.
  VISION_APIFUNC bool GetRotationDelta(hkvVec3& RotationDelta) const;


  /// \brief
  ///   Gets the current visibility bounding box.
  /// 
  /// This function evaluates the union of all animated visibility bounding boxes from all active
  /// animations.
  /// 
  /// \param VisibilityBoundingBox
  ///   reference that receives the visibility bounding box.
  /// 
  /// \return
  ///   bool bResult: true if the visibility bounding box already has been updated this frame.
  VISION_APIFUNC bool GetVisibilityBoundingBox(hkvAlignedBBox &VisibilityBoundingBox) const;
  
  
  /// \brief
  ///   Returns the current flags of the config (passed in the constructor).
  /// 
  /// \return
  ///   int iFlags: Current flags, see constructor for supported flags.
  inline int GetFlags(void) const 
  {
    return m_iFlags;
  }
  
  
  /// \brief
  ///   Sets the flags of the config.
  /// 
  /// See constructor for supported flags.
  /// 
  /// \param iFlags
  ///   bitflags, see constructor for supported flags.
  /// 
  /// \return
  ///   nothing.
  inline void SetFlags(int iFlags)
  {
    m_iFlags = iFlags;
    if (spMesh!=NULL)
      m_VertexAnimResult.ClearResult(m_iFlags);
  }


  /// \brief
  ///   Indicates whether the visibility bounding box is up-to-date.
  /// 
  /// \return
  ///   bool bValid: true if the visibility bounding box is valid
  VISION_APIFUNC bool IsVisibilityBoundingBoxValid() const
  {
    return m_bVisibilityBoundingBoxValid;
  }

  ///
  /// @}
  ///

  ///
  /// @name Static Helper Functions
  /// @{
  ///
  
  /// \brief
  ///   Static helper function to create a new animation config for playing a vertex animation.
  /// 
  /// You have to add a vertex animation to the VisVertexAnimDeformer_cl instance.
  /// 
  /// The config can be set on one or more entities.
  /// 
  /// \param pMesh
  ///   mesh of the entity the config is created for.
  /// 
  /// \param pVertexAnimDeformer
  ///   optional pointer that gets filled with the new deformer pointer.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pConfig: new created config.
  VISION_APIFUNC static VisAnimConfig_cl* CreateVertexConfig(VDynamicMesh* pMesh, VisVertexAnimDeformer_cl** pVertexAnimDeformer=NULL);


  /// \brief
  ///   Static helper function to create a new animation config for applying morphing deformers.
  /// 
  /// You have to add morph-targets to the VisMorphingDeformer_cl instance.
  /// 
  /// The config can be set on one or more entities.
  /// 
  /// \param pMesh
  ///   mesh of the entity the config is created for.
  /// 
  /// \param pMorphingDeformer
  ///   optional pointer that gets filled with the new deformer pointer.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pConfig: new created config.
  VISION_APIFUNC static VisAnimConfig_cl* CreateMorphingConfig(VDynamicMesh* pMesh, VisMorphingDeformer_cl** pMorphingDeformer=NULL);


  /// \brief
  ///   Static helper function to create a new animation config for applying skeletal deformers.
  /// 
  /// You have to add a skeletal animation control or a whole skeletal animation mixer hierarchy to
  /// the pFinalSkeletalResult instance.
  /// 
  /// The config can be set on one or more entities.
  /// 
  /// \param pMesh
  ///   mesh of the entity the config is created for.
  /// 
  /// \param pFinalSkeletalResult
  ///   optional pointer that gets filled with the new created final skeletal result.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pConfig: new created config.
  VISION_APIFUNC static VisAnimConfig_cl* CreateSkeletalConfig(VDynamicMesh* pMesh, VisAnimFinalSkeletalResult_cl** pFinalSkeletalResult=NULL);


  /// \brief
  ///   Static helper function to create a new animation config for applying skeletal and vertex
  ///   animations.
  /// 
  /// You have to add a vertex animation to the VisVertexAnimDeformer_cl instance.
  /// 
  /// You have to add a skeletal animation control or a whole skeletal animation mixer hierarchy to
  /// the pFinalSkeletalResult instance.
  /// 
  /// The config can be set on one or more entities.
  /// 
  /// \param pMesh
  ///   mesh of the entity the config is created for.
  /// 
  /// \param pFinalSkeletalResult
  ///   optional pointer that gets filled with the new created final skeletal result instance.
  /// 
  /// \param pVertexAnimDeformer
  ///   optional pointer that gets filled with the new deformer instance.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pConfig: new created config.
  VISION_APIFUNC static VisAnimConfig_cl* CreateSkeletalVertexConfig(VDynamicMesh* pMesh, VisAnimFinalSkeletalResult_cl** pFinalSkeletalResult=NULL, VisVertexAnimDeformer_cl** pVertexAnimDeformer=NULL);
  
  /// \brief
  ///   Static helper function to create a new animation config for applying skeletal and morphing
  ///   animations.
  /// 
  /// You have to add a morphing deformer to the VisMorphingDeformer_cl instance.
  /// 
  /// You have to add a skeletal animation control or a whole skeletal animation mixer hierarchy to
  /// the pFinalSkeletalResult instance.
  /// 
  /// The config can be set on one or more entities.
  /// 
  /// \param pMesh
  ///   mesh of the entity the config is created for.
  /// 
  /// \param pFinalSkeletalResult
  ///   optional pointer that gets filled with the new created final skeletal result.
  /// 
  /// \param pMorphingDeformer
  ///   optional pointer that gets filled with the new deformer.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pConfig: new created config.
  VISION_APIFUNC static VisAnimConfig_cl* CreateSkeletalMorphingConfig(VDynamicMesh* pMesh, VisAnimFinalSkeletalResult_cl** pFinalSkeletalResult=NULL, VisMorphingDeformer_cl** pMorphingDeformer=NULL);


  /// \brief
  ///   Static helper function to create a new animation config for applying vertex and morphing
  ///   animations.
  /// 
  /// You have to add a morphing deformer to the VisMorphingDeformer_cl instance.
  /// 
  /// You have to add a vertex animation to the VisVertexAnimDeformer_cl instance.
  /// 
  /// The config can be set on one or more entities.
  /// 
  /// \param pMesh
  ///   mesh of the entity the config is created for.
  /// 
  /// \param pVertexAnimDeformer
  ///   optional pointer that gets filled with the new deformer.
  /// 
  /// \param pMorphingDeformer
  ///   optional pointer that gets filled with the new deformer.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pConfig: new created config.
  VISION_APIFUNC static VisAnimConfig_cl* CreateVertexMorphingConfig(VDynamicMesh* pMesh, VisVertexAnimDeformer_cl** pVertexAnimDeformer=NULL, VisMorphingDeformer_cl** pMorphingDeformer=NULL);


  /// \brief
  ///   Static helper function to create a new animation config for applying vertex, skeletal and
  ///   morphing animations.
  /// 
  /// You have to add a vertex animation to the VisVertexAnimDeformer_cl instance.
  /// 
  /// You have to add a morphing deformer to the VisMorphingDeformer_cl instance.
  /// 
  /// You have to add a skeletal animation control or a whole skeletal animation mixer hierarchy to
  /// the pFinalSkeletalResult instance.
  /// 
  /// The config can be set on one or more entities.
  /// 
  /// \param pMesh
  ///   dynamic mesh of the entity the config is created for.
  /// 
  /// \param pVertexAnimDeformer
  ///   optional pointer that gets filled with the new deformer.
  /// 
  /// \param pFinalSkeletalResult
  ///   optional pointer that gets filled with the new created final skeletal result.
  /// 
  /// \param pMorphingDeformer
  ///   optional pointer that gets filled with the new deformer.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pConfig: new created config.
  VISION_APIFUNC static VisAnimConfig_cl* CreateSkeletalVertexMorphingConfig(VDynamicMesh* pMesh, VisAnimFinalSkeletalResult_cl** pFinalSkeletalResult=NULL, VisVertexAnimDeformer_cl** pVertexAnimDeformer=NULL, VisMorphingDeformer_cl** pMorphingDeformer=NULL);


  /// \brief
  ///   Static helper function to start playing the specified vertex animation on an entity.
  /// 
  /// This is the simplest way to start a single vertex animation on an entity.
  /// 
  /// This method creates a config for the entity that plays one vertex animation.
  /// 
  /// If you call the method again for the same entity the old config gets deleted.
  /// 
  /// \param pEntity
  ///   entity the new config will be set on.
  /// 
  /// \param szAnimSequenceName
  ///   name of the animation sequence that will be played back. This name must be a valid vertex
  ///   animation sequence name of an animation that is attached to the entity's mesh (.model) file.
  /// 
  /// \param iControlFlags
  ///   Flags for the vertex animation control class. Use the VANIMCTRL_LOOP flag to loop the
  ///   animation.
  /// 
  /// \param fControlSpeed
  ///   relative playback speed. 1.0f for normal speed.
  /// 
  /// \return
  ///   VisVertexAnimControl_cl* pControl: new created vertex control.
  VISION_APIFUNC static VisVertexAnimControl_cl* StartVertexAnimation(VisBaseEntity_cl* pEntity, const char* szAnimSequenceName, int iControlFlags = VVERTANIMCTRL_DEFAULTS, float fControlSpeed = 1.f);

  /// \brief
  ///   Static helper function to start playing the specified skeletal animation on an entity (by
  ///   name).
  /// 
  /// This is the simplest way to start a single skeletal animation on an entity.
  /// 
  /// This method creates a config for the entity that plays one skeletal animation.
  /// 
  /// If you call the method again for the same entity the old config gets deleted.
  /// 
  /// \param pEntity
  ///   entity the new config will be set on.
  /// 
  /// \param szAnimSequenceName
  ///   name of the animation sequence that will be played back.
  /// 
  /// \param iControlFlags
  ///   Flags for the skeletal animation control class. Use the VANIMCTRL_LOOP flag to loop the
  ///   animation.
  /// 
  /// \param fControlSpeed
  ///   relative playback speed. 1.0f for normal speed.
  /// 
  /// \return
  ///   VisSkeletalAnimControl_cl*: new created skeletal control.
  VISION_APIFUNC static VisSkeletalAnimControl_cl* StartSkeletalAnimation(VisBaseEntity_cl* pEntity, const char* szAnimSequenceName, int iControlFlags = VSKELANIMCTRL_DEFAULTS, float fControlSpeed = 1.f);

  /// \brief
  ///   Static helper function to start playing the specified skeletal animation on an entity (by
  ///   sequence pointer).
  /// 
  /// This is the simplest way to start a single skeletal animation on an entity.
  /// 
  /// This method creates a config for the entity that plays one skeletal animation.
  /// 
  /// If you call the method again for the same entity the old config gets deleted.
  /// 
  /// \param pEntity
  ///   entity the new config will be set on.
  /// 
  /// \param pAnimSequence
  ///   pointer to the skeletal animation sequence that will be played back.
  /// 
  /// \param iControlFlags
  ///   Flags for the skeletal animation control class. Use the VANIMCTRL_LOOP flag to loop the
  ///   animation.
  /// 
  /// \param fControlSpeed
  ///   relative playback speed. 1.0f for normal speed.
  /// 
  /// \return
  ///   VisSkeletalAnimControl_cl*: new created skeletal control.
  VISION_APIFUNC static VisSkeletalAnimControl_cl* StartSkeletalAnimation(VisBaseEntity_cl* pEntity, VisSkeletalAnimSequence_cl *pAnimSequence, int iControlFlags = VSKELANIMCTRL_DEFAULTS, float fControlSpeed = 1.f);  
   
  /// \brief
  ///   Static helper function to start playing a combined vertex and skeletal animation on an
  ///   entity.
  /// 
  /// This method creates a new config for the entity.
  /// 
  /// If you call the method again for the same entity the old config gets deleted.
  /// 
  /// \param pEntity
  ///   entity the new config will be set on.
  /// 
  /// \param pSkeletalAnimControl
  ///   gets filled with the pointer to the newly created skeletal control.
  /// 
  /// \param pVertexAnimControl
  ///   gets filled with the pointer on the newly created vertex control.
  /// 
  /// \param szSkeletalAnimSequenceName
  ///   name of the skeletal sequence that will be played back.
  /// 
  /// \param szVertexAnimSequenceName
  ///   name of the vertex sequence that will be played back.
  /// 
  /// \param iSkeletalControlFlags
  ///   Flags for the skeletal animation control class. Use the VANIMCTRL_LOOP flag to loop the
  ///   animation.
  /// 
  /// \param iVertexControlFlags
  ///   Flags for the vertex animation control class. Use the VANIMCTRL_LOOP flag to loop the
  ///   animation.
  /// 
  /// \param fSkeletalControlSpeed
  ///   relative playback speed for the skeletal animation playback. 1.0f for normal speed.
  /// 
  /// \param fVertexControlSpeed
  ///   relative playback speed for the vertex animation playback. 1.0f for normal speed.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pConfig: new config.
  VISION_APIFUNC static VisAnimConfig_cl* StartSkeletalAndVertexAnimation(VisBaseEntity_cl* pEntity, VisSkeletalAnimControl_cl** pSkeletalAnimControl, VisVertexAnimControl_cl** pVertexAnimControl, const char* szSkeletalAnimSequenceName, const char* szVertexAnimSequenceName, int iSkeletalControlFlags = VSKELANIMCTRL_DEFAULTS, int iVertexControlFlags = VVERTANIMCTRL_DEFAULTS, float fSkeletalControlSpeed = 1.f, float fVertexControlSpeed = 1.f);


  /// \brief
  ///   Static helper function to start playing a morph animation on an entity.
  /// 
  /// This method creates a new config for the entity.
  /// 
  /// If you call the method again for the same entity the old config gets deleted.
  /// 
  /// \param pEntity
  ///   entity the new config will be set on.
  /// 
  /// \param szAnimSequenceName
  ///   name of the animation sequence, the first frame will be applied as morph target.
  /// 
  /// \param fMorphTargetWeight
  ///   The initial weighting factor. Must be in range [0..1]
  /// 
  /// \return
  ///   VisMorphingDeformer_cl*: new created morphing deformer.
  VISION_APIFUNC static VisMorphingDeformer_cl* StartMorphingAnimation(VisBaseEntity_cl* pEntity, const char* szAnimSequenceName, float fMorphTargetWeight = 1.f);

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  // serialization
  V_DECLARE_SERIAL_DLLEXP( VisAnimConfig_cl, VISION_APIDATA )
 
  /// \brief
  ///   Serializes this config
  VISION_APIFUNC virtual void Serialize( VArchive &ar );

  /// \brief
  ///   Empty constructor for serialization; do not use.
  VISION_APIFUNC VisAnimConfig_cl(); 


  ///
  /// @}
  ///

  ///
  /// @name Caching
  /// @{
  ///

  /// \brief
  ///   Indicates whether the current result has already been calculated and cached.
  /// 
  /// \return
  ///   bool bCached: True for cached results
  inline bool HasVertexResultBeenCached(void) const 
  {
    return m_bCachedVertexResult;
  }


  /// \brief
  ///   Indicates whether the current result has already been copied to the render buffer.
  /// 
  /// \return
  ///   bool bCopied: True for copied results
  inline bool HasCopiedResultToRenderBuffer(void) const
  {
    return m_bCopiedToRenderBuffer;
  }


  /// \brief
  ///   Indicates whether the current result has already been copied to the collision buffer.
  /// 
  /// \return
  ///   bool bCopied: True for copied results
  inline bool HasCopiedResultToCollisionBuffer(void) const 
  {
    return m_bCopiedToCollisionBuffer;
  }

  ///
  /// @}
  ///

  ///
  /// @name Result Update
  /// @{
  ///


  /// \brief
  ///   Returns the number of the last frame the result has been updated.
  /// 
  /// \return
  ///   int: count of the last updated frame.
  inline int GetResultUpdateCount(void) const
  {
    return m_iResultUpdate;
  }

  /// \brief
  ///   Internal function; do not use.
  inline void SetResultUpdateCount(int iResultUpdate) 
  {
    m_iResultUpdate = iResultUpdate;
  }


  /// \brief
  ///   Indicates whether the config is in frozen state.
  /// 
  /// A config in frozen state is not updated anymore and the cached results are reused if
  /// possible.
  /// 
  /// \return
  ///   bool bFrozen: True for frozen configs.
  inline bool GetFrozen(void) const {return m_bFrozen;}


  /// \brief
  ///   SetFrozen sets the frozen state of the config.
  /// 
  /// A config in frozen state is not updated anymore and the cached results are reused if
  /// possible.
  /// 
  /// If all the skeletal and vertex controls and modifiers are paused you should also freeze the
  /// config to cache the results and save significant performance in the animation system.
  /// 
  /// The frozen state has to be set manually. The animation system does not detect automatically
  /// if it is possible to freeze an animation config.
  /// 
  /// \param bFrozen
  ///   true for freezing the config, false for un-freezing.
  /// 
  /// \return
  ///   Nothing.
  inline void SetFrozen(bool bFrozen) {m_bFrozen = bFrozen;}


  ///
  /// @}
  ///

  ///
  /// @name Bone Transformation
  /// @{
  ///

  /// \brief
  ///   Helper function to calculate the bone translation and rotation in world space for this
  ///   config and the specified entity.
  /// 
  /// Since the bone result is in object space, this function is useful to transform the bone
  /// position into world space.
  /// 
  /// \param pEntity
  ///   entity the bone is relative to.
  /// 
  /// \param iBoneIndex
  ///   index of the bone.
  /// 
  /// \param boneTranslation
  ///   vector that receives the translation result.
  /// 
  /// \param boneRotation
  ///   quaternion that receives the rotation result.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void GetBoneCurrentWorldSpaceTransformation(const VisBaseEntity_cl* pEntity, int iBoneIndex, hkvVec3& boneTranslation, hkvQuat &boneRotation);


  /// \brief
  ///   Helper function to calculate the bone translation and rotation in object space for this
  ///   config.
  /// 
  /// \param iBoneIndex
  ///   index of the bone.
  /// 
  /// \param boneTranslation
  ///   vector that receives the translation result.
  /// 
  /// \param boneRotation
  ///   quaternion that receives the rotation result.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void GetBoneCurrentObjectSpaceTransformation(int iBoneIndex, hkvVec3& boneTranslation, hkvQuat &boneRotation);


  /// \brief
  ///   Helper function to calculate the bone translation and rotation in local space for this
  ///   config.
  /// 
  /// \param iBoneIndex
  ///   index of the bone.
  /// 
  /// \param boneTranslation
  ///   vector that receives the translation result.
  /// 
  /// \param boneRotation
  ///   quaternion that receives the rotation result.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void GetBoneCurrentLocalSpaceTransformation(int iBoneIndex, hkvVec3& boneTranslation, hkvQuat &boneRotation);


  ///
  /// @}
  ///

  ///
  /// @name Internal Functions
  /// @{
  ///

  /// \brief
  ///   Debug helper function to show the animation graph as a text overlay
  VISION_APIFUNC void Debug_ShowHierarchy(IVRenderInterface *pRI, float x, float y);

  /// \brief
  ///   Helper output class
  static void Helper_OutputClassName(IVRenderInterface *pRI, VisTypedEngineObject_cl *pClass, const char *szInstanceName, float x, float y, hkvVec2 &vSize, VColorRef iColor=V_RGBA_WHITE);

  ///
  /// @}
  ///

  /// \brief
  ///   Sets the skinning mode for this animation configuration.
  /// 
  /// The Vision engine supports three different skinning modes: VIS_SKINNINGMODE_SOFTWARE (default
  /// on PC DirectX9) performs skinning operations on the CPU. The advantages are that CPU skinning
  /// can be combined with vertex animations or morph targets, and that it doesn't require any
  /// special shaders (it can arbitrarily be combined with any shader effects). Furthermore, CPU
  /// skinning supports caching of skinned vertex data between frames and even across entities
  /// using the same VisAnimConfig_cl object, so it is recommendable when vertex data can be
  /// reused. On the downside, it is relatively slow when vertex data can not be reused.
  /// VIS_SKINNINGMODE_HARDWARE performs skinning operations in the vertex shader. The bone
  /// transformations are passed to the vertex shader as an array of shader constants; the skinning
  /// itself has to be done in every vertex shader that is used to render the respective entity.
  /// This skinning method is very fast when there is a lot of unique vertex data (unshared
  /// VisAnimConfig_cl instances and no frame to frame reuse of skinned data). However, vertex
  /// animations can't be used at the same time as hardware skinning, and all vertex shaders to be
  /// used for skinned entities will have to implement skinning (either using matrices or
  /// quaternions). Hardware skinning is also limited to the number of vertex constant registers
  /// available - quaternion skinning requires two registers per bone, matrix skinning requires
  /// three registers per bone. VIS_SKINNINGMODE_HARDWARE_MEMEXPORT (default on Xbox360, PC DX10, and
  /// PC DX11 with DX10-level graphics cards) is only supported on Xbox360, DX10, and DX11.
  /// It combines hardware skinning with many of the advantages of CPU skinning: Results are cached
  /// and shared within VisAnimConfig_cl instances, and no special shaders are needed; it is, however,
  /// typically significantly faster than CPU skinning. Memexport hardware skinning can't be combined
  /// with vertex animations, though.
  /// Note that there are cases in which VIS_SKINNINGMODE_HARDWARE_MEMEXPORT will automatically be
  /// disabled. This is typically the case in situations where skinned vertex data is read back
  /// with the CPU, when vertex or morph animations are played back on a VisAnimConfig_cl object,
  /// or when specific flags (e.g. KEEP_EXTRA_SYSTEMBUFFER, KEEP_EXTRA_COLLISIONBUFFER) are set in
  /// the VisAnimConfig_cl object. Since memexport skinning is replaced with compute shader skinning on 
  /// DX11 hardware which supports this feature, VIS_SKINNINGMODE_COMPUTE is an alias for VIS_SKINNINGMODE_HARDWARE_MEMEXPORT.
  /// Finally, VIS_SKINNINGMODE_SPU is by far the best choice on Playstation3. It performs skinning on the PS3's
  /// SPUs.
  ///
  /// Setting the skinning method during a render loop is not allowed and will result in undefined behavior.  
  ///
  /// \param eSkinningMode
  ///   The desired skinning mode.
  /// 
  /// \param iTrackingRegisterStart
  ///   For VIS_SKINNINGMODE_HARDWARE and VIS_SKINNINGMODE_HARDWARE_MEMEXPORT, this value provides
  ///   the vertex shader constant at which to start writing matrix/quaternion data for entities.
  ///   Note that, if registers below 64 are used, the "overwrites global constants" flag has to be
  ///   set in the shader. If -1 is passed here, constants will start at 2 if
  ///   VIS_SKINNINGMODE_HARDWARE_MEMEXPORT is used, and at 70 if VIS_SKINNINGMODE_HARDWARE is
  ///   used.
  /// 
  /// \sa VisAnimConfig_cl::SetUseQuaternionSkinning
  /// \sa VisAnimConfig_cl::GetUseQuaternionSkinning
  /// \sa VisAnimConfig_cl::GetShaderConstantRegisterStart
  /// \sa VisAnimConfig_cl::GetSkinningMode
  VISION_APIFUNC void SetSkinningMode(VisSkinningMode_e eSkinningMode, int iTrackingRegisterStart=-1);

  /// \brief
  ///   Returns the current skinning mode previously set with SetSkinningMode.
  /// 
  /// \return
  ///   VisSkinningMode_e: The skinning mode.
  VISION_APIFUNC VisSkinningMode_e GetSkinningMode() const { return m_eSkinningMode; }

  /// \brief
  ///   Specifies whether hardware skinning will use quaternions or matrices.
  /// 
  /// If quaternions are used, only two vertex shader constant registers (rather than three) will
  /// be used per bone. The vertex shaders responsible for skinning will have to take these as
  /// input for the skinning operations. This allows you to store more bone transformation in the
  /// existing shader registers - for instance, you will be able to use 123 bones for memexport
  /// skinning on Xbox360 instead of 82. This way, you can process entities using memexport skinning
  /// with more complex skeletons.
  /// 
  /// Typically, quaternion skinning is somewhat slower than matrix skinning. Additionally, it is only supported
  /// for Vertex Shader skinning (PS3, Xbox360, PC) or memexport skinning (Xbox360).
  /// 
  /// \param bUseQuaternionSkinning
  ///   true if quaternion skinning is to be used, false for matrix skinning.
  inline void SetUseQuaternionSkinning(bool bUseQuaternionSkinning) { m_bQuaternionSkinning=bUseQuaternionSkinning; }

  /// \brief
  ///   Returns whether quaternion-based hardware skinning is currently enabled.
  /// 
  /// \return
  ///   bool: true if quaternion skinning is used, false if matrix skinning is used.
  inline bool GetUseQuaternionSkinning() const { return m_bQuaternionSkinning; }
  
  /// \brief
  ///   Specifies whether tangents should be skinned or not. 
  ///
  /// If your model uses normal mapping then this must be enabled, otherwise it should be turned off
  /// to save performance.
  /// 
  /// By default this is enabled on all platforms
  /// 
  /// \param bSkinTangents
  ///   true if tangents should be skinned
  inline void SetSkinTangents(bool bSkinTangents) { m_VertexAnimResult.m_bSkinTangents = bSkinTangents; }
  
  /// \brief
  ///   Returns whether tangents are skinned or not.
  inline bool GetSkinTangents() const { return m_VertexAnimResult.m_bSkinTangents; }

  /// \brief
  ///   Returns the first vertex shader constant for matrix/quaternion information
  /// 
  /// \return
  ///   int: the first vertex shader constant for matrix/quaternion information
  inline int  GetShaderConstantRegisterStart() const { return m_iTrackingRegister; }

  /// \brief
  ///   Returns the task responsible for multithreaded skinning
  /// 
  /// \return
  ///   VisVertexAnimationTask_cl *: the task responsible for multithreaded skinning
  inline VisVertexAnimationTask_cl *GetVertexAnimationTask() const { return m_spVertexAnimationTask; }

  /// \brief
  ///   Resets the motion delta of the animation results and input network
  /// 
  VISION_APIFUNC void ResetMotionDelta();

  /// \brief
  ///   Returns the trace/collision mesh for this anim config.
  /// 
  /// If the anim config does not yet have a trace/collision mesh, it will create one. Once created, the trace/collision mesh will be
  /// updated automatically if the COPY_RESULT_TO_COLLISIONBUFFER flag is set on the anim config. Otherwise, it
  /// will only be updated when the GetTraceMesh() function is called. Note that if you know you will need the
  /// trace/collision mesh (almost) every frame, it is typically much faster to specify the COPY_RESULT_TO_COLLISIONBUFFER
  /// flag on the anim config, since this avoids performing skinning or copy operations multiple times.
  ///
  /// You can use this method to retrieve the animated mesh of the model, taking both skeletal and vertex animations
  /// into account. A typical use case could be to take this animated mesh and bake it into a new mesh, using the 
  /// VDynamicMeshBuilder class.
  ///
  /// If the anim config does not reference a mesh, this method will return NULL.
  ///
  /// Note that the engine may call this method internally.
  /// 
  /// \returns
  ///   Pointer to a VAnimatedCollisionMesh instance representing the mesh in its current animation state.
  VISION_APIFUNC VAnimatedCollisionMesh *GetTraceMesh();

  /// \brief
  ///   Releases the anim config's trace mesh.
  VISION_APIFUNC void ReleaseTraceMesh();

  /// \brief
  ///   Returns a mesh buffer containing the skinned vertex data generated by this anim config.
  /// 
  /// The returned mesh buffer contains the skinned vertex data generated by this anim config if the skinning mode
  /// is set to VIS_SKINNINGMODE_SPU (PS3 only!) or  VIS_SKINNINGMODE_SOFTWARE. With other skinning modes, this
  /// method should not be used.
  /// The vertex format is contained in the mesh buffer's vertex descriptor; however, it usually contains the
  /// Position as a float3, the Normal as a float3, and the Tangent as a float3. In this case,
  /// the stride will be 3*3*4 Bytes = 36 Bytes.
  ///
  /// \returns
  ///   Pointer to a VisMeshBuffer_cl instance containing the skinned vertex data.
  inline VisMeshBuffer_cl *GetSkinningMeshBuffer() { return m_VertexAnimResult.GetSkinningMeshBuffer(); }

  /// \brief
  /// Set Skinning mode depend on hardware. Default for PC : software skinning.
  VISION_APIFUNC void SetPreferredSkinningMethod();


  /// \brief
  /// Internal function to prepare the anim config for rendering
  ///
  /// This function is implicitly called by the entity's VisBaseEntity_cl::PrepareForRendering() function, so it does not have to be called by user code.
  /// The only reason why this must be called manually is if an anim config that is not bound to an entity is rendered through
  /// VisRenderLoopHelper_cl::RenderModelWithSurfaceShaderList or VisRenderLoopHelper_cl::RenderModelWithShader. In that case this function must be called
  /// during the update loop, not while rendering.
  ///
  /// \param iUsage
  ///   Usually COPY_RESULT_TO_RENDERBUFFER, but also COPY_RESULT_TO_COLLISIONBUFFER is possible
  VISION_APIFUNC void StartComputeCurrentVertexResult(int iUsage);

  // Internal:
  VISION_APIFUNC void TrackShaderConstants(unsigned int iStartingIndex);
  void SyncWithAnimationTask(bool bForceUnlock);
  void CopyToCollisionBuffer();
  void CopyToRenderBuffer();
  VisSkinningMode_e GetEffectiveSkinningMode(int iUsage);
  bool NeedsUpdate(int iUsage);



#ifdef _VISION_PS3
  void WaitForSpursJob();
  void WaitForFence();
  void EnsureFenceExists();
  void InsertFence();
  void DeleteFence();
#endif


protected:
  void CommonInit();

  VisSkinningMode_e m_eSkinningMode;
  VisSkinningMode_e m_eCachedEffectiveSkinningMode;
  bool m_bQuaternionSkinning;
  int m_iTrackingRegister;

  unsigned int m_iLastFrameUpdatedState;

  VisAnimFinalSkeletalResultPtr m_spFinalResult;          ///< pointer to (shared) final result
  VisVertexDeformerStackPtr m_spVertexDeformerStack;

  // holds several flags to change config behavior
  int m_iFlags;

  // offset delta
  hkvVec3 m_vOffsetDelta;
  bool m_bOffsetDeltaValid;
  
  // rotation delta
  hkvVec3 m_RotationDelta;
  bool m_bRotationDeltaValid;

  // visibility bounding box, union of all visibility bounding boxes of all animations
  hkvAlignedBBox m_VisibilityBoundingBox;
  bool m_bVisibilityBoundingBoxValid;

  VDynamicMeshPtr spMesh;
  VisSkeletonPtr spSkeleton;

  VisVertexAnimResult_cl m_VertexAnimResult;   ///< used to keep VertexAnimResult

  // new system/video memory handling for vertex animation and skinning
  bool m_bCachedVertexResult;
  bool m_bCopiedToRenderBuffer;     ///< stores if the vertex result already has been copied to fastgeom memory after skinning, vertex animations where applied
  bool m_bCopiedToCollisionBuffer;  ///< stores if vertex positions have been transfered to collision buffer
  int m_iResultUpdate;              ///< for further optimizing the caching of the result, should be not bigger than 1
  bool m_bFrozen;                   ///< tells the update state if it has to do anything


#ifdef _VISION_PS3
  unsigned int m_iFence;
#endif


  VisVertexAnimationTaskPtr m_spVertexAnimationTask; ///< task for multi-threaded CPU skinning computation
  unsigned int m_iVertexResultFrame;

  bool m_bUseMemExportSkinning;    ///< Even if the skinning mode is memexport, there may still be circumstances under which memexport skinning can't be used.

  VAnimatedCollisionMeshPtr m_spCollisionMesh;
};

#ifndef _VISION_DOC

// Internal class
class VisVertexAnimationTask_cl : public VThreadedTask, public VRefCounter
{
public:
  VisVertexAnimationTask_cl();
  virtual ~VisVertexAnimationTask_cl();

  virtual void Run(VManagedThread *pThread);

  /// \brief
  ///   RTTI macro
  V_DECLARE_DYNAMIC_DLLEXP(VisVertexAnimationTask_cl, VISION_APIFUNC);

private:
  friend class VisAnimConfig_cl;
  VisAnimConfig_cl *m_pAnimConfig;
  int m_iUsage;
};

#endif // _VISION_DOC



#endif //VIS_ANIM_STATE_HPP_INCLUDED

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
