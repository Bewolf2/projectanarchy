/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiBaseEntity.hpp

#ifndef FR_DEFINE_VISAPIBASEENTITY
#define FR_DEFINE_VISAPIBASEENTITY



#include <Vision/Runtime/Engine/System/ElementManager/VisApiElementManager.hpp>
#include <Vision/Runtime/Engine/Physics/VisApiCollisionMesh.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimConfig.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiElementVisData.hpp>

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
#include <Vision/Runtime/Engine/Visibility/VisApiPortal.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiFrustum.hpp>
#include <Vision/Runtime/Engine/Renderer/Material/VisApiSurface.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiDynamicMesh.hpp>

//Flags for entity settings 
#define VISENTFLAG_UPDATE_ON_ANIM  1


#define VISENTFLAG_DEFAULT  (VISENTFLAG_UPDATE_ON_ANIM) ///< default flags


#include <Vision/Runtime/Engine/Physics/IVisApiPhysicsObject.hpp>

#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQueryObject.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityData.hpp>

struct VisTraceLineInfo_t;
class VisRenderContext_cl;
class VLightmapPrimitive;
class VLightmapSceneInfo;

class VisFrustum_cl;


/// \brief
///   Base class for dynamic objects in the scene
/// 
/// The VisBaseEntity_cl class is the base class for entities in the Vision Engine. Entities are
/// programmable objects in the scene, defined by position, orientation, animation status and
/// various  additional properties. Entities can reference static meshes (and therefore have a visual
/// representation), essentially in the same way as a VisStaticMeshInstance_cl references a VisStaticMesh_cl.
/// 
/// The VisBaseEntity_cl class provides access to the entity properties and provides virtual
/// functions that can be overridden. Programmer's can derive their own entity classes from this
/// base class and implement game specific code in their derived classes.
/// 
/// Entities may either be created dynamically (using VisGame_cl::CreateEntity) or placed in the
/// scene via vForge (WYSIWYG editing tool). The dynamic variable system (see START_VAR_TABLE
/// macro) can be used to define dynamic variables that are exposed to the level designer (via
/// vForge). 
/// 
/// Please refer to the Entities chapter in the Programmer's Documentation for more details about
/// the entity system in Vision.
class VisBaseEntity_cl : public VisObject3D_cl, public VVisibilityData,
  public VisElementManager_cl<class VisBaseEntity_cl *>
{
public:
  V_DECLARE_SERIAL_DLLEXP( VisBaseEntity_cl, VISION_APIDATA )

  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Default constructor (used for serialization purposes only).
  /// 
  /// This constructor is used for serialization purposes only. Please refer to the
  /// VisGame_cl::CreateEntity function instead for dynamically creating entities within your
  /// application.
  /// 
  /// Do not initialize your entity in the constructor since the entity properties might not be set
  /// yet.  Override the virtual VisBaseEntity_cl::InitFunction instead.
  /// 
  /// \sa VisGame_cl::CreateEntity
  VISION_APIFUNC VisBaseEntity_cl();


  /// \brief
  ///   Destructor (for internal usage only)
  /// 
  /// This destructor is for internal usage only. Your application has to use the
  /// VisBaseEntity_cl::Remove or VisBaseEntity_cl::Free functions instead to safely remove the
  /// object.
  /// 
  /// \sa VisBaseEntity_cl::Remove
  /// \sa VisBaseEntity_cl::Free
  VISION_APIFUNC virtual ~VisBaseEntity_cl();


  /// \brief
  ///   Marks the entity as deleted.  
  /// 
  /// The scene manager will take care of deleting the entity (by default
  /// within the VisionApp_cl::OnUpdateScene function).
  /// 
  /// Use this function to safely remove the entity while the scene is being updated. This might
  /// defer the entity deletion up to the beginning of the next scene update.
  /// 
  /// \sa VisBaseEntity_cl::Free
  VISION_APIFUNC void Remove();


  /// \brief
  ///   DEPRECATED - use DisposeObject instead
  /// 
  HKV_DEPRECATED_2012_1 inline void Free()
  {
    DisposeObject();
  }

  /// \brief
  ///   Overridden VisTypedEngineObject_cl function to remove this instance from the scene. Just
  ///   calls VisBaseEntity_cl::Free
  VISION_APIFUNC virtual void DisposeObject() HKV_OVERRIDE;



  ///
  /// @}
  ///

  ///
  /// @name Virtual Entity Functions
  /// @{
  ///

  /// \brief
  ///   Override this function to initialize the entity
  /// 
  /// Use this function instead of the constructor to initialize your entity. It is guaranteed that
  /// all entity properties are set when this function is called.
  /// 
  /// The virtual InitFunction is called for dynamically created entities. It is NOT called for
  /// entities loaded from a VSCENE file, the constructor and Serialize function will be called
  /// instead. For your own entity classes you can implement the the virtual
  /// VTypedObject::OnDeserializationCallback function and call the InitFunction from there. This
  /// function gets called by the VArchive class after de-serializing all typed objects.
  /// 
  /// vForge recreates the entity instances when switching between edit and play-the-game mode.
  /// This will also trigger the InitFunction of the entities from within vForge.
  /// 
  /// Custom entity classes should call this base implementation.
  /// 
  /// \sa VisBaseEntity_cl::MessageFunction
  /// \sa VisBaseEntity_cl::ThinkFunction
  /// \sa VisBaseEntity_cl::DeInitFunction
  VISION_APIFUNC virtual void InitFunction();


  /// \brief
  ///   Override this function to deinitialize the entity.
  /// 
  /// Use this function instead of the destructor to deinitalize your entity. It is guaranteed that
  /// the virtual DeInitFunction gets called before the entity is deleted. You can still access all
  /// entity properties within this function.
  /// 
  /// Custom entity classes should call this base implementation.
  /// 
  /// The virtual DeInitFunction is not called when deleting an entity with the delete operator.
  /// Use the VisBaseEntity_cl::Remove and VisBaseEntity_cl::Free functions instead to delete
  /// entities.
  /// 
  /// \sa VisBaseEntity_cl::InitFunction
  /// \sa VisBaseEntity_cl::MessageFunction
  /// \sa VisBaseEntity_cl::ThinkFunction
  VISION_APIFUNC virtual void DeInitFunction();

  
  /// \brief
  ///   Override this function to process your entity once per frame, after animations and physics
  ///   are applied
  /// 
  /// The virtual ThinkFunction is called by the scene manager once per frame. Override this method
  /// to process your entity game logic. Both animations and physics have already been updated at
  /// this point.
  /// 
  /// The ThinkFunction handling is part of the scene manager implementation. You can modify this
  /// behaviour or completely discard the ThinkFunction by customizing our default scene manager
  /// (VisionSceneManager_cl) or  implementing your own one (IVisSceneManager_cl interface). Please
  /// refer to the "Customizing Game Loops"  chapter in the Programmer's Documentation for more
  /// details.
  /// 
  /// There is also a virtual PreThinkFunction that you can override. This function gets called
  /// BEFORE physics and animations are updated.
  /// 
  /// Please note that vForge only calls the ThinkFunction when running the "Play-the-Game" edit
  /// mode. There is an alternative function EditorThinkFunction that is called by vForge for every tick.
  /// 
  /// \note
  ///   Do NOT call the base implementation when overriding the ThinkFunction. This would
  ///   deactivate the ThinkFunction by calling the VisBaseEntity_cl::SetThinkFunctionStatus.
  /// 
  /// \sa VisBaseEntity_cl::PreThinkFunction
  /// \sa VisBaseEntity_cl::SetThinkFunctionStatus
  /// \sa VisBaseEntity_cl::GetThinkFunctionStatus
  /// \sa VisBaseEntity_cl::EditorThinkFunction
  VISION_APIFUNC virtual void ThinkFunction() { SetThinkFunctionStatus(FALSE); }
  

  /// \brief
  ///   Override this function to process your entity once per frame, before animations and physics
  ///   are applied
  /// 
  /// The virtual PreThinkFunction is called by the scene manager once per frame. Override this
  /// method to process your entity game logic. Neither animations nor physics have been updated at
  /// this point.
  /// 
  /// The PreThinkFunction handling is part of the scene manager implementation. You can modify
  /// this behaviour or completely discard the PreThinkFunction by customizing our default scene
  /// manager (VisionSceneManager_cl) or  implementing your own one (IVisSceneManager_cl
  /// interface). Please refer to the "Customizing Game Loops"  chapter in the Programmer's
  /// Documentation for more details.
  /// 
  /// There is also a virtual ThinkFunction that you can override. This function gets called AFTER
  /// physics and animations are updated.
  /// 
  /// Please note that vForge only calls the PreThinkFunction when running the "Play-the-Game" edit
  /// mode.
  /// 
  /// \note
  ///   Do NOT call the base implementation when overriding the ThinkFunction. This would
  ///   deactivate the ThinkFunction by calling the VisBaseEntity_cl::SetThinkFunctionStatus.
  /// 
  /// \sa VisBaseEntity_cl::ThinkFunction
  /// \sa VisBaseEntity_cl::SetPreThinkFunctionStatus
  /// \sa VisBaseEntity_cl::GetPreThinkFunctionStatus
  VISION_APIFUNC virtual void PreThinkFunction() { SetPreThinkFunctionStatus(FALSE); }

  /// \brief
  ///   Overridable that is called for every entity every frame when inside the editor, regardless of Play-the-game status.
  ///
  /// Unlike the ThinkFunction, this function is also called when the editor is not in playback mode. Accordingly,
  /// it is quite suitable for editor preview rendering using debug line rendering functions etc.
  /// This function is never called for standalone applications outside the editor and it does not even exist on platforms other 
  /// than PC.
  /// The default implementation is empty.
  /// 
  /// \sa VisBaseEntity_cl::ThinkFunction
  VISION_APIFUNC virtual void EditorThinkFunction() {}


  /// \brief
  ///   Overridden base class method to handle resource reloading messages.
  VISION_APIFUNC virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;


  ///
  /// @}
  ///

  ///
  /// @name Entity Properties
  /// @{
  ///
  
  
  /// \brief
  ///   DEPRECATED, use RTTI instead
  inline const char *GetClassFullName() const
  {
    return this->GetTypeId()->m_lpszClassName;
  }
  
  
  /// \brief
  ///   Gets the current entity key. 
  /// 
  /// The entity key is an arbitrary string that can be set for the entity. Setting the key can
  /// either be done within vForge or with the VisBaseEntity_cl::SetEntityKey function.
  /// 
  /// This function wraps around GetObjectKey of the base class.
  /// 
  /// \return
  ///   const char *entityKey: current entity key of this entity. NULL if not specified.
  inline const char *GetEntityKey() const
  {
    return GetObjectKey();
  }
  
  /// \brief
  ///   Get the current entity key (safe access)
  /// 
  /// If the entity key is not specified, an empty string is returned.
  /// 
  /// The entity key is an arbitrary string that can be set for the entity. Setting the key can
  /// either be done within vForge or with the VisBaseEntity_cl::SetEntityKey function.
  /// 
  /// This function wraps around GetObjectKeySafe of the base class.
  /// 
  /// \return
  ///   const char *entityKey: current entity key of this entity. Empty string if not specified.
  inline const char *GetEntityKeySafe() const
  {
    return GetObjectKeySafe();
  }

  

  /// \brief
  ///   Get the entity key as string (as VString). This overload should not be used as it allocates
  ///   string memory.
  inline void GetEntityKey(VString &res) const
  {
    res = m_sObjectKey;
  }


  /// \brief
  ///   Sets the entity key for this entity.
  /// 
  /// The entity key is an arbitrary string that can be set on the entity. Setting the key can
  /// either be done within vForge or with this function.
  /// 
  /// This function wraps around SetObjectKey of the base class.
  /// 
  /// \param key
  ///   new entity key (or NULL to clear the key)
  inline void SetEntityKey(const char *key)
  {
    SetObjectKey(key);
  }
  




  ///
  /// @}
  ///

  ///
  /// @name Mesh-related Functions
  /// @{
  ///

  /// \brief
  ///   Set the mesh for this  entity (by filename)
  /// 
  /// The SetMesh function loads the specified dynamic mesh (.model file) and sets it on the entity. Use this function
  /// to change the mesh of an entity while the application is running.
  /// 
  /// Note that the animation configuration is set to NULL by this function. It is within the
  /// responsibility of the game developer to set a new matching animation configuration or to keep
  /// a reference to the old one.
  /// 
  /// If a default mesh file is specified in the START_VAR_TABLE macro then the engine will load
  /// and set this mesh when creating the entity instance. The same applies if a mesh file (.model) has
  /// been set in vForge.
  /// 
  /// This function is not virtual but it wraps around the virtual version of SetMesh that takes a VDynamicMesh pointer as input. Basically
  /// this function is the same as SetMesh(Vision::Game.LoadMesh(szMeshFile, true)).
  /// 
  /// \param szMeshFile
  ///   filename of the new dynamic mesh (e.g. "scientist.model")
  /// 
  /// \return
  ///   BOOL success: TRUE if the mesh could be successfully loaded.
  /// 
  /// \sa VisBaseEntity_cl::GetMesh
  /// \sa VisBaseEntity_cl::HasMesh
  /// 
  /// \example
  ///   \code
  ///   pEnt->SetMesh( "scientist.model" );
  ///   \endcode
  VISION_APIFUNC virtual BOOL SetMesh( const char *szMeshFile);

  /// \brief
  ///   Set the mesh for this entity (by pointer)
  /// 
  /// This function sets the passed mesh for the entity. Use this function to change the
  /// mesh of an entity while the application is running.
  /// 
  /// Note that the animation configuration is set to NULL by this function. It is within the
  /// responsibility of the game developer to set a new matching animation configuration or to keep
  /// a reference to the old one.
  /// This function is virtual so custom entity classes can respond to changes.
  /// 
  /// \param pMesh
  ///   pointer to the new mesh
  /// 
  /// \return
  ///   Nothing.
  /// 
  /// \sa VisBaseEntity_cl::GetMesh
  /// \sa VisBaseEntity_cl::HasMesh
  VISION_APIFUNC virtual void SetMesh(VDynamicMesh* pMesh);

  /// \brief
  ///   Indicates whether a mesh is assigned to this entity
  /// 
  /// \return
  ///   BOOL: TRUE if the entity has a mesh, otherwise FALSE.
  /// 
  /// \sa VisBaseEntity_cl::SetMesh
  /// \sa VisBaseEntity_cl::GetMesh
  VISION_APIFUNC BOOL HasMesh() const;

 
  /// \brief
  ///   Gets the current mesh of the entity
  /// 
  /// This function provides access to the entity's mesh, which contains vertex data, skeleton information,
  /// material data, and allows accessing animation data. Note that a single mesh may be shared by an arbitrary
  /// number of entities.
  /// 
  /// This method will always return a valid VDynamicMesh instance. If the mesh could not be loaded,
  /// the engine will transparently have loaded the MissingModel.model from the basedata directory
  /// instead.
  ///
  /// If you want to be able to determine whether a mesh was successfully loaded then you need to
  /// remove the MissingModel.model file from the basedata. You can then call IsResourceFlagSet(VRESOURCEFLAG_ISLOADED)
  /// on the VDynamicMesh to see whether it was loaded or not.
  /// 
  /// Use the START_VAR_TABLE macro to specify the default mesh file (.model) for an entity class or use
  /// vForge to set the mesh for the entity. Alternatively you can call the
  /// VisBaseEntity_cl::SetMesh function for dynamically setting a mesh for the entity.
  /// 
  /// \return
  ///   VDynamicMesh*:  Pointer to the mesh of this entity
  /// 
  /// \sa VisBaseEntity_cl::SetMesh
  /// \sa VisBaseEntity_cl::HasMesh
  inline VDynamicMesh* GetMesh() const
  {
    return m_spMesh;
  }

 
  ///
  /// @}
  ///

  ///
  /// @name Bounding Box Related Functions
  /// @{
  ///


  /// \brief
  ///   Draw the bounding boxes for all bones of this entity
  /// 
  /// The rendering is done with a line buffer and will thus be visible at the next frame.
  /// 
  /// \param iColor
  ///   color, the default RGB value is 255,255,255
  /// 
  /// \param lineWidth
  ///   width of the line, the default value is 1.0f
  /// 
  /// \note
  ///   This function draws the bounding boxes once. You can call this function each frame  to get
  ///   a continuous rendering of the bone bounding boxes.
  /// 
  /// \sa VisBaseEntity_cl::DrawBoneBoundingBox
  /// \sa VisBaseEntity_cl::DrawBoundingBox
  VISION_APIFUNC void DrawBoneBoundingBoxes(VColorRef iColor = VColorRef(255,255,255), float lineWidth = 1.0f);


  /// \brief
  ///   Draw the bounding box for a single bone
  /// 
  /// The rendering is done with a line buffer and will thus be visible at the next frame.
  /// 
  /// \param boneName
  ///   name of the bone to be drawn
  /// 
  /// \param iColor
  ///   color, the default RGB value is 255,255,255
  /// 
  /// \param lineWidth
  ///   width of the line, the default value is 1.0f
  /// 
  /// \note
  ///   This function draws the bounding box once. You can call this function each  frame to get a
  ///   continuous rendering of the bone bounding box.
  /// 
  /// \sa VisBaseEntity_cl::DrawBoneBoundingBoxes
  /// \sa VisBaseEntity_cl::DrawBoundingBox
  VISION_APIFUNC void DrawBoneBoundingBox(const char *boneName, VColorRef iColor = VColorRef(255, 255, 255), float lineWidth = 1.0f);


  /// \brief
  ///   Draw the collision bounding box of the entity
  /// 
  /// The rendering is done with a line buffer and will thus be visible at the next frame.
  /// 
  /// Uses the VisBaseEntity_cl::GetCollisionBoundingBox function to query the bounding box and
  /// VisGame_cl::DrawBoundingBox or VisGame_cl::DrawOrientedBoundingBox to perform the rendering.
  /// 
  /// \param bOriented
  ///   rendering mode. TRUE for oriented bounding box, FALSE for axis aligned bounding box
  /// 
  /// \param iColor
  ///   color, the default RGB value is 255,255,255
  /// 
  /// \param lineWidth
  ///   width of the line, the default value is 1.0f
  ///
  /// \return
  ///   BOOL: Returns TRUE if successful or FALSE if the mesh bounding box could not be gathered.
  ///   Possible reason: entity doens't have a mesh assigned.
  /// 
  /// \sa VisBaseEntity_cl::DrawBoneBoundingBoxes
  /// \sa VisBaseEntity_cl::DrawBoneBoundingBox
  VISION_APIFUNC BOOL DrawBoundingBox(BOOL bOriented, VColorRef iColor = VColorRef(255,255,255), float lineWidth = 1.0f);





  ///
  /// @}
  ///

  ///
  /// @name Entity Setup and Initialisation
  /// @{
  ///

  /// \brief
  ///   Enables or disables the virtual ThinkFunction call for the entity.
  /// 
  /// The default scene manager
  /// implementation (VisionSceneManager_cl) takes this property into account to decide whether the
  /// ThinkFunction should be called.
  /// 
  /// \param bStatus
  ///   TRUE for enabling the ThinkFunction, FALSE for disabling it
  /// 
  /// \note
  ///   Use this function to reduce the number of virtual function calls when working with large
  ///   numbers of entities.
  /// 
  /// \sa VisBaseEntity_cl::GetThinkFunctionStatus
  /// \sa VisBaseEntity_cl::SetPreThinkFunctionStatus
  /// \sa VisBaseEntity_cl::GetPreThinkFunctionStatus
  VISION_APIFUNC void SetThinkFunctionStatus(BOOL bStatus);

  /// \brief
  ///   Gets the current status of the ThinkFunction.
  /// 
  /// \return
  ///   BOOL: TRUE if the ThinkFunction is currently enabled, FALSE if it is disabled
  /// 
  /// \sa VisBaseEntity_cl::SetThinkFunctionStatus
  /// \sa VisBaseEntity_cl::SetPreThinkFunctionStatus
  /// \sa VisBaseEntity_cl::GetPreThinkFunctionStatus
  VISION_APIFUNC BOOL GetThinkFunctionStatus() const;

  /// \brief
  ///   Enables or disables the virtual PreThinkFunction call for the entity.
  /// 
  /// The default scene
  /// manager  implementation (VisionSceneManager_cl) takes this property into account to decide
  /// whether the PreThinkFunction should be called.
  /// 
  /// \param bStatus
  ///   TRUE for enabling the PreThinkFunction, FALSE for disabling it
  /// 
  /// \note
  ///   Use this function to reduce the number of virtual function calls when working with large
  ///   numbers of entities.
  /// 
  /// \sa VisBaseEntity_cl::SetThinkFunctionStatus
  /// \sa VisBaseEntity_cl::GetThinkFunctionStatus
  /// \sa VisBaseEntity_cl::GetPreThinkFunctionStatus
  VISION_APIFUNC void SetPreThinkFunctionStatus(BOOL bStatus);
  
  /// \brief
  ///   Gets the current status of the PreThinkFunction
  /// 
  /// \return
  ///   BOOL: TRUE if PreThinkFunction is currently enabled, FALSE if it is disabled
  /// 
  /// \sa VisBaseEntity_cl::SetThinkFunctionStatus
  /// \sa VisBaseEntity_cl::GetThinkFunctionStatus  
  /// \sa VisBaseEntity_cl::SetPreThinkFunctionStatus
  VISION_APIFUNC BOOL GetPreThinkFunctionStatus() const;

  /// \brief
  ///   Returns a pointer to the collision mesh of an entity.
  /// 
  /// Unlike trace meshes, collision meshes are never animated, so this method will never return an animated
  /// collision mesh. In order to obtain an animated collision mesh (generated from the render mesh) for animated
  /// entities, use the GetTraceMesh() function.
  ///
  /// \param bForceCreate
  ///   If set to true, the collision mesh will be created if it has not been generated yet. In this case, the
  ///   method will always return a valid collision mesh as long as the entity has a mesh. If bForceCreate is 
  ///   false, this method will return NULL.
  ///
  /// \param bUseTraceMeshIfLoadingFailed
  ///   If set to true, the function may return the trace mesh if loading the collision mesh failed. See
  ///   VBaseMesh::GetCollisionMesh for details.
  /// 
  /// \return
  ///   IVCollisionMesh *pCollisionMesh : Pointer to collision mesh or NULL
  ///
  /// \sa VBaseMesh::GetCollisionMesh
  VISION_APIFUNC IVCollisionMesh *GetCollisionMesh(bool bForceCreate, bool bUseTraceMeshIfLoadingFailed);

  /// \brief
  ///   Returns a pointer to the trace mesh of an entity.
  /// 
  /// If the entity is not animated (i.e., does not have a VisAnimConfig_cl object attached to it), the trace
  /// mesh returned by this function will be the trace mesh of the entity's VDynamicMesh object. In this case,
  /// the returned trace mesh will be of type VRigidCollisionMesh.
  /// 
  /// If the entity is animated, the returned trace mesh will be of type VAnimatedCollisionMesh. In this case,
  /// calling this function is equivalent to calling VisAnimConfig_cl::GetTraceMesh(). Note that there is no
  /// differentiation between trace meshes and collision meshes for animated entities, since there is no
  /// separate skinned representation for collision meshes.
  ///
  /// \param bForceCreate
  ///   If set to true, the trace mesh will be created if it has not been generated yet. In this case, the
  ///   method will always return a valid trace mesh as long as the entity has a mesh. If bForceCreate is 
  ///   false, this method will return NULL.
  ///
  /// \return
  ///   IVCollisionMesh *pCollisionMesh : Pointer to trace mesh or NULL
  ///
  /// \sa VBaseMesh::GetTraceMesh
  VISION_APIFUNC IVCollisionMesh *GetTraceMesh(bool bForceCreate);

  /// \brief
  ///   Overrides the collision mesh of the entity.
  ///
  /// If you set a custom collision mesh using this function, the engine will use the specified collision mesh
  /// instead of an internally generated one.
  /// 
  /// \sa VisBaseEntity_cl::GetCollisionMesh
  /// \sa VisBaseEntity_cl::SetTraceMesh
  ///
  /// \param pMesh
  ///   Pointer to the new collision mesh
  VISION_APIFUNC void SetCollisionMesh(VRigidCollisionMesh *pMesh);

  /// \brief
  ///   Overrides the trace mesh of the entity.
  ///
  /// If you set a custom trace mesh using this function, the engine will use the specified trace mesh
  /// instead of an internally generated one.
  /// 
  /// \sa VisBaseEntity_cl::GetTraceMesh
  /// \sa VisBaseEntity_cl::SetCollisionMesh
  ///
  /// \param pMesh
  ///   Pointer to the new trace mesh
  VISION_APIFUNC void SetTraceMesh(VRigidCollisionMesh *pMesh);

  /// \brief
  ///   Specifies whether the entity should always be rendered in foreground
  /// 
  /// This flag should e.g. be set for first person weapon overlays. By default
  /// SetAlwaysInForeGround is disabled.
  /// 
  /// \param status
  ///   if true then the entity will always be rendered in foreground. All entities which are
  ///   rendered in foreground will clip each other again.
  /// 
  /// \note
  ///   Rendering foreground objects is significantly slower than rendering normal objects, since
  ///   an additional rendering pass is requiered.
  /// 
  /// \note
  ///   Foreground objects may not contain any translucent surfaces.
  VISION_APIFUNC void SetAlwaysInForeGround(bool status = true);



  /// \brief
  ///   Sets the visible state of a specific submesh
  ///
  /// This feature allows for masking out specific submeshes on a per-instance(=entity) basis. Unlike for static mesh instances, there is no submesh instance
  /// representation on a per-entity basis to set a visibility bitmask, so this is the only possibility to disable entity submeshes for rendering.
  /// The SetVisibleBitmask function can be used to enable/disable the whole entity in specific render contexts. This function on the other hand additionally sets a visible enable state
  /// for specific submeshes (which is applied for all contexts).
  /// Do not use this feature to disable the whole entity - rather use SetVisibleBitmask(0) in that case, because the submesh masking will still cause the entity to run through the
  /// full visibility and even rendering pipeline, whereas SetVisibleBitmask(0) is very efficient.
  /// By default all submesh states of an entity are set. Also, when the model changes, the submeshes will be enabled again. To set the state of all submeshes, the
  /// function VisBaseEntity_cl::SetSubmeshVisibleStateMask can be used.
  /// These flags affect the following rendering functions: VisRenderLoopHelper_cl::RenderEntitiesWithShader, VisRenderLoopHelper_cl::RenderEntityWithShaders and
  /// VisRenderLoopHelper_cl::RenderEntityWithSurfaceShaderList
  ///
  /// \param iSubmesh
  ///   The submesh index to set. This index must be in valid range [0..GetMesh()->GetSubmeshCount()-1]
  ///
  /// \param bState
  ///   The new visible state for the submesh
  ///
  /// \sa VisBaseEntity_cl::SetVisibleBitmask
  /// \sa VisBaseEntity_cl::SetSubmeshVisibleStateMask
  /// \sa VisRenderLoopHelper_cl::RenderEntitiesWithShader
  /// \sa VisRenderLoopHelper_cl::RenderEntityWithShaders
  /// \sa VisRenderLoopHelper_cl::RenderEntityWithSurfaceShaderList
  VISION_APIFUNC void SetSubmeshVisibleState(int iSubmesh, bool bState);

  /// \brief
  ///   Returns the visible state of a specific submesh in this entity
  ///
  /// \param iSubmesh
  ///   The submesh index to query. This index must be in valid range [0..GetMesh()->GetSubmeshCount()-1]
  ///
  /// \returns
  ///   The visible state that has been set via SetSubmeshVisibleState
  ///
  /// \sa VisBaseEntity_cl::SetSubmeshVisibleState
  inline bool GetSubmeshVisibleState(int iSubmesh) const
  {
    VASSERT(m_spMesh!=NULL && iSubmesh>=0 && iSubmesh<m_spMesh->GetSubmeshCount());
    return (m_pSubmeshVisMaskPtr[iSubmesh>>5] & (1<<(iSubmesh&31)))>0;
  }

  /// \brief
  ///   Sets the visible state of all submeshes at the same time.
  ///
  /// The passed bitmask can be a NULL pointer to set all bits or it must hold GetMesh()->GetSubmeshCount() bits. So the passed array must
  /// hold at least (GetMesh()->GetSubmeshCount()+31)/32 integer values (32 bit each). The bitmask will be copied so temporary values can be passed.
  /// For more information, please refer to VisBaseEntity_cl::SetSubmeshVisibleState.
  ///
  /// \param piMask
  ///   Pointer to a bitmask. Can be NULL to enable the visibility of all submeshes. Otherwise it must hold enough bits (see description).
  ///
  /// \sa VisBaseEntity_cl::SetSubmeshVisibleState
  VISION_APIFUNC void SetSubmeshVisibleStateMask(const unsigned int* piMask);

  /// \brief
  ///   Returns a pointer to a bitmask that represents the visible state of each submesh. So the returned bitmask provides GetMesh()->GetSubmeshCount() bits
  inline const unsigned int* GetSubmeshVisibleStateMask() const
  {
    return m_pSubmeshVisMaskPtr;
  }

  /// \brief
  ///   Indicates whether any submesh on this instance is disabled. This flag might be used for rendering optimizations
  inline bool IsAnySubmeshDisabled() const
  {
    return (statusFlags & VIS_STATUS_DISABLED_ANY_SUBMESH)>0;
  }


  /// \brief
  ///   Specifies whether this entity casts a dynamic shadow.
  /// 
  /// This includes both shadows from dynamic lightsources (VisLightSource_cl) and static
  /// lightsources (placed in vLux).
  /// 
  /// Dynamic shadows are by default disabled for new entities. Use this function to enable them.
  /// 
  /// Dynamic shadows from static lightsources will only be applied if the precomputed static
  /// lighting (generated via vLux) includes light masks. Please execute vLux with enabled "light 
  /// masks" option on your scene in case you don't get any shadows from static lightsources.
  /// 
  /// \param bStatus
  ///   The new status. If FALSE, the entity will not cast shadows
  /// 
  /// \sa VisBaseEntity_cl::GetCastShadows
  VISION_APIFUNC void SetCastShadows(BOOL bStatus);

  /// \brief
  ///   Get the dynamic shadow status of this entity.
  /// 
  /// \return
  ///   BOOL bStatus: The current status. If FALSE, the entity does not cast shadows
  /// 
  /// \sa VisBaseEntity_cl::SetCastShadows
  VISION_APIFUNC BOOL GetCastShadows() const;

  /// \brief
  ///   Internal flag used by vForge. Does not affect realtime rendering at all.
  inline void SetCastStaticShadows(bool bStatus) {m_bCastStaticShadows=bStatus;}

  /// \brief
  ///   Internal flag used by vForge. Does not affect realtime rendering at all.
  inline bool GetCastStaticShadows() const {return m_bCastStaticShadows;}

  /// \brief
  ///   Sets the collision accuracy for traceline operations against this entity.
  /// 
  /// Note that this value is by default set to VIS_TRACEACC_NOCOLLISION, which causes the
  /// entity to be ignored during traceline operations.
  /// 
  /// Please refer to the VisTraceAccuracy_e enumeration for more details about the available
  /// traceline collision accuracies. The VIS_TRACEACC_POLYGONS flag is only valid for
  /// entities which have a mesh assigned. Entities without a mesh will fall back to AABOX
  /// accuracy.
  /// 
  /// \param accuracy
  ///   collision accuracy used for traceline operations against the entity
  /// 
  /// \sa VisCollisionToolkit_cl::TraceLine
  /// \sa VisBaseEntity_cl::SetCustomTraceBBox
  /// \sa VisBaseEntity_cl::GetTraceAccuracy
  /// 
  /// \example
  ///   \code
  ///   entity->SetTraceAccuracy(VIS_TRACEACC_POLYGONS);
  ///   \endcode
  VISION_APIFUNC void SetTraceAccuracy(VisTraceAccuracy_e accuracy);


  /// \brief
  ///   Gets the current collision accuracy for traceline operations against this entity
  /// 
  /// \return
  ///   VisTraceAccuracy_e: current trace accuracy
  /// 
  /// \sa VisCollisionToolkit_cl::TraceLine
  /// \sa VisBaseEntity_cl::SetCustomTraceBBox
  /// \sa VisBaseEntity_cl::SetTraceAccuracy
  VISION_APIFUNC VisTraceAccuracy_e GetTraceAccuracy() const;

  
  /// \brief
  ///   Sets the custom trace bounding box of the entity.
  /// 
  /// Any  upcoming traceline operations against the entity will then use this custom bounding box
  /// instead of the collision bounding box.
  /// 
  /// The passed bounding box will be copied by the engine.
  /// 
  /// The custom trace bounding box can be cleared by passing NULL as parameter.
  /// 
  /// \param pCustomTraceBBox
  ///   pointer to the new bounding box. The data gets copied by this function
  /// 
  /// \note
  ///   The custom bounding box for traceline operations is not relevant if the traceline accuracy
  ///   is set to VIS_TRACEACC_NOCOLLISION.
  /// 
  /// \sa VisCollisionToolkit_cl::TraceLine
  /// \sa VisBaseEntity_cl::SetTraceAccuracy
  /// \sa VisBaseEntity_cl::GetTraceAccuracy
  /// \sa VisBaseEntity_cl::GetCustomTraceBBox
  VISION_APIFUNC void SetCustomTraceBBox(const hkvAlignedBBox *pCustomTraceBBox);


  /// \brief
  ///   Gets the custom trace bounding box of this entity
  /// 
  /// Can also return NULL if the custom trace bounding box is not specified. See
  /// VisBaseEntity_cl::SetCustomTraceBBox for more details.
  /// 
  /// \return
  ///   const hkvAlignedBBox *: pointer to the custom trace bounding box (NULL if not set)
  /// 
  /// \sa VisCollisionToolkit_cl::TraceLine
  /// \sa VisBaseEntity_cl::SetTraceAccuracy
  /// \sa VisBaseEntity_cl::GetTraceAccuracy
  /// \sa VisBaseEntity_cl::SetCustomTraceBBox
  VISION_APIFUNC const hkvAlignedBBox *GetCustomTraceBBox() const;

  /// \brief
  ///   Sets the trace bitmask for this entity
  /// 
  /// The bitmask is considered for traceline operations when using
  /// VisCollisionToolkit_cl::TraceLine
  /// 
  /// The bitmask value will be logically ANDed with the validEntities parameter of the traceline
  /// function. Thus, if the bitmask is set to 0, the entity will never be traced.
  /// 
  /// The default bitmask value of an entity is 0xffffffff
  /// 
  /// \param iBitMask
  ///   The 32 bit bitmask
  /// 
  /// \sa VisBaseEntity_cl::GetTracelineBitmask
  /// \sa VisCollisionToolkit_cl::TraceLine
  /// \sa VisBaseEntity_cl::SetTraceAccuracy
  /// \sa VisBaseEntity_cl::GetTraceAccuracy
  /// \sa VisBaseEntity_cl::SetCustomTraceBBox
  inline void SetTracelineBitmask(unsigned int iBitMask);

  /// \brief
  ///   Gets the trace bitmask value of this entity
  /// 
  /// To set the bitmask, use the VisBaseEntity_cl::SetTracelineBitmask function
  /// 
  /// \return
  ///   unsigned int iBitMask : The current bitmask
  /// 
  /// \sa VisBaseEntity_cl::SetTracelineBitmask
  /// \sa VisCollisionToolkit_cl::TraceLine
  /// \sa VisBaseEntity_cl::SetTraceAccuracy
  /// \sa VisBaseEntity_cl::GetTraceAccuracy
  /// \sa VisBaseEntity_cl::SetCustomTraceBBox
  inline unsigned int GetTracelineBitmask() const;


  /// \brief
  ///   Set the primary sorting key for the entity
  /// 
  /// SetPrimarySortingKey sets a value that can be used for fine-tuning the sorting order of
  /// entities. A larger value will cause the entity to be rendered earlier than other entities.
  /// 
  /// This value overrides the render state and distance properties.
  /// 
  /// This flag only affects the order in case VIS_SORTING_OBJECTKEY is globally set via
  /// VisRenderer_cl::SetEntitySortingCriterion, which is default.
  /// 
  /// \param key
  ///   primary sorting key
  VISION_APIFUNC void SetPrimarySortingKey(signed char key);
  
  /// \brief
  ///   Gets the primary sorting key for the entity.
  /// 
  /// See VisBaseEntity_cl::SetPrimarySortingKey for more details
  /// 
  /// \return
  ///   signed char key: current primary sorting key
  VISION_APIFUNC signed char GetPrimarySortingKey();

  /// \brief
  ///   Defines whether the bounding volume information of the entity will be recomputed when its
  ///   animations status changes
  /// 
  /// If SetUpdateOnAnimation is false, the bounding volume information will not be updated when
  /// the animation status of the entity changes. This can be used to improve performance.
  ///
  /// \param bStatus
  ///   Should the bounding volume information be updated when the animation status changes
  VISION_APIFUNC void SetUpdateOnAnimation(bool bStatus);

  /// \brief
  ///   Set the uniform scaling factor for the entity
  /// 
  /// It is possible to uniformly scale entities by using this function,
  /// both the rendering and the collision meshes get updated accordingly.
  /// The collision bounding boxes also get multiplied according to the specified
  /// scaling factor before they are used in the physics system
  /// 
  /// \param fScaling
  ///   scaling value (1.0f = default size) May NOT be negative
  /// 
  /// \return
  ///   BOOL bResult: TRUE if scaling value could be set.
  /// 
  /// \sa VisBaseEntity_cl::GetScaling
  inline BOOL SetScaling(float fScaling)
  {
    hkvVec3 scale(fScaling,fScaling,fScaling);
    return SetScaling(scale);
  }

  /// \brief
  ///   Sets the non-uniform scaling factor of the entity
  /// 
  /// The rendering and the collision meshes get updated accordingly.
  /// The collision bounding boxes also get multiplied according to the
  /// specified scaling factor before they are used in the physics system
  /// 
  /// IMPORTANT: Visibility tests for objects may use spheres. So using a scaling with a large
  /// difference in components (ex. 5,1,1) can lead to large spheres and overestimated visibility.
  /// 
  /// \param scale
  ///   scaling vector (1.f, 1.f, 1.f = default size) Components may NOT be negative
  /// 
  /// \return
  ///   BOOL bResult: Always TRUE.
  /// 
  /// \sa VisBaseEntity_cl::GetScaling
  VISION_APIFUNC BOOL SetScaling(const hkvVec3& scale);

  /// \brief
  ///   Gets the non-uniform scaling of the entity
  /// 
  /// See VisBaseEntity_cl::SetScaling for more information about scaling entities.
  /// 
  /// \return
  ///   const hkvVec3& scale: non-uniform scaling factor of the entity (x,y,z component)
  /// 
  /// \sa VisBaseEntity_cl::SetScaling
  VISION_APIFUNC const hkvVec3& GetScaling() const;

  
  /// \brief
  ///   Computes and returns the matrix transforming this entity to world space.
  /// 
  /// \param WorldMatrix
  ///   gets filled with the transformation matrix
  VISION_APIFUNC void GetWorldMatrix( hkvMat4& WorldMatrix); 

  /// \brief
  ///   Computes and returns the matrix transforming this entity to world space as a pointer.
  /// 
  /// \returns
  ///   Pointer to the transformation matrix
  inline const hkvMat4& GetWorldMatrix () { UpdateWorldMatrix(); return m_CachedTransform; }

  ///
  /// @}
  ///

  ///
  /// @name Flag-related Functions
  /// @{
  ///
 

  /// \brief
  ///   Sets the custom flags of the entity
  /// 
  /// These flags are not used by the engine. Feel free to store any game specific information in
  /// them. The custom flags are set to 0 for new entities.
  /// 
  /// \param flags
  ///   new custom flags
  VISION_APIFUNC void SetCustomFlags(int flags);
  

  /// \brief
  ///   Gets the custom flags of the entity
  /// 
  /// Refer to VisBaseEntity_cl::SetCustomFlags for more details.
  /// 
  /// \return
  ///   int flags: current custom flags
  VISION_APIFUNC int GetCustomFlags() const;
  

  /// \brief
  ///   Gets the status flags of the entity
  /// 
  /// \return
  ///   int statusFlags: internal status flags, available values are:
  ///    \li VIS_STATUS_SKELETALANIMATION_CHANGED: skeletal animation as modified within this frame
  ///    \li VIS_STATUS_ANIMATION_CHANGED: skeletal or vertex animation was modified within this frame
  ///    \li VIS_STATUS_DISABLED_THINKFUNCTION: ThinkFunction is disabled and thus won't be called
  ///    \li VIS_STATUS_FORCE_COLMESH_UPDATE: enforces a collision mesh update on the next request
  ///    \li VIS_STATUS_DISABLED_PRETHINKFUNCTION: prethinkfunction is disabled, won't be called anymore
  VISION_APIFUNC int GetStatusFlags() const;


  /// \brief
  ///   Set the status flags of the entity
  /// 
  /// Please note that the engine takes care of updating the
  /// status flags. Modifying them manually can lead to unexpected results.
  /// 
  /// \param iStatusFlags
  ///   internal status flags (See GetStatusFlags())
  ///
  /// \sa VisBaseEntity_cl::GetStatusFlags
  inline void SetStatusFlags(int iStatusFlags)
  {
    int r0 = (statusFlags &(VIS_STATUS_DISABLED_THINKFUNCTION|VIS_STATUS_DISABLED_PRETHINKFUNCTION));
    int r1 = (iStatusFlags &(VIS_STATUS_DISABLED_THINKFUNCTION|VIS_STATUS_DISABLED_PRETHINKFUNCTION));
    statusFlags = iStatusFlags;
    if (r0!=r1)
      CheckCollectionUpdate();
  }

  ///
  /// @}
  ///

  ///
  /// @name Miscellaneous
  /// @{
  ///

  /// \brief
  ///   Enforce an entity status update in the core engine
  /// 
  /// UpdateEntityChangedStatus enforces an update of the entity position, orientation and bounding
  /// box in the core engine. Use this function to update the core data when modifying the entity
  /// outside the  game loop. 
  VISION_APIFUNC void UpdateEntityChangedStatus();



  /// \brief
  ///   Parses the variable string and updates the respective variable(s).
  /// 
  /// The passed string must consist of variable=value pairs, each pair separated with comma. 
  /// The value part must be inside quotes. Valid variables are specified inside the START_VAR_TABLE/END_VAR_TABLE macro pairs.
  /// If a non-existing variable is specified, then this function returns FALSE.
  /// 
  /// \param szVarString
  ///   Variable string to parse.
  /// 
  /// \param bUpdateEngine
  ///   Not used anymore
  /// 
  /// \return
  ///   BOOL: FALSE if at least one variable could not be set
  /// 
  /// \example
  ///   \code
  ///   pEntity->SetVariablesByString("Radius=\"20.000\", Height=\"160.0000\" ");
  ///   \endcode
  VISION_APIFUNC BOOL SetVariablesByString(const char *szVarString, BOOL bUpdateEngine = TRUE);




  ///
  /// @}
  ///

  ///
  /// @name Physics
  /// @{
  ///

  /// \brief
  ///   Create a new physics object via the registered physics module
  /// 
  /// CreatePhysicsObject creates a new physics object by calling the
  /// IVisPhysicsModule_cl::CreatePhysicsObject function on the currently registered physics
  /// module. Use this function during the initialisation of your entity to setup the physics
  /// properties.
  /// 
  /// Each entity can have exactly one physics object. Additional calls of this function on the
  /// same entity will not recreate the physics object but simply return the existing one.
  /// 
  /// \param bStatic
  ///   Indicates whether the physics object will be created as as static one. Static physics
  ///   objects may not move.
  /// 
  /// \return
  ///   IVisPhysicsObject_cl *: pointer to the physics object interface
  /// 
  /// \sa VisBaseEntity_cl::DeletePhysicsObject
  /// \sa VisBaseEntity_cl::GetPhysicsObject
  /// \sa IVisPhysicsObject_cl
  /// \sa IVisPhysicsModule_cl
  /// 
  VISION_APIFUNC IVisPhysicsObject_cl *CreatePhysicsObject(bool bStatic=false);


  /// \brief
  ///   Releases the physics object of the entity
  /// 
  /// Not strictly required because this happens automatically at entity destruction time
  /// 
  /// \sa VisBaseEntity_cl::CreatePhysicsObject
  /// \sa VisBaseEntity_cl::GetPhysicsObject
  /// \sa VisBaseEntity_cl::SetPhysicsObject
  /// \sa IVisPhysicsObject_cl
  /// \sa IVisPhysicsModule_cl
  VISION_APIFUNC void DeletePhysicsObject();


  /// \brief
  ///   Get the physics object of the entity
  /// 
  /// If the physics object was not created then NULL is returned.
  /// 
  /// \return
  ///   IVisPhysicsObject_cl *: pointer to the physics object interface
  /// 
  /// \sa VisBaseEntity_cl::CreatePhysicsObject
  /// \sa VisBaseEntity_cl::SetPhysicsObject
  /// \sa IVisPhysicsObject_cl
  /// \sa IVisPhysicsModule_cl
  /// 
  VISION_APIFUNC IVisPhysicsObject_cl *GetPhysicsObject() const;



  /// \brief
  ///   Sets the physics object of the entity.
  /// 
  /// \param pPhysObj
  ///   pointer to the physics object interface. If NULL is passed the current physics object will
  ///   be released
  /// 
  /// \sa VisBaseEntity_cl::CreatePhysicsObject
  /// \sa VisBaseEntity_cl::GetPhysicsObject
  /// \sa IVisPhysicsObject_cl
  /// \sa IVisPhysicsModule_cl
  VISION_APIFUNC void SetPhysicsObject(IVisPhysicsObject_cl *pPhysObj);

  
  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///



  /// \brief
  ///   Indicates whether the entity is marked as deleted.
  /// 
  /// The application will then take care of deleting the entity (by default within
  /// the VisionApp_cl::OnUpdateScene function). 
  ///
  /// The "Remove" functionality is used to safely remove the entity while the scene is  being
  /// updated. This might defer the entity deletion up to the beginning of the next  scene update.
  /// 
  /// \return
  ///   BOOL status : TRUE if the entity is marked as deleted
  inline BOOL IsRemoved() const;


  /// \brief
  ///   Gets the visibility status of this entity in the last rendered frame of the given render context.
  ///
  /// If the object was rendered in the last frame, the function returns TRUE. 
  /// 
  /// \param pContext
  ///   Render Context for which the visibility status is to be queried. If the context is NULL,
  ///   the currently active render context is used.
  /// 
  /// \return
  ///   BOOL status : The visibility status of the object in the last frame
  /// 
  /// \note
  ///   Calling this function is rather cheap, since the visibility status has already been
  ///   calculated
  VISION_APIFUNC BOOL WasVisibleLastFrame(VisRenderContext_cl *pContext=NULL) const;

  /// \brief
  ///   Gets the visibility status of this entity in the last rendered frame of all active render
  ///   contexts.
  /// 
  /// If the object has been visible in the last frame rendered in any of the contexts, the function
  /// returns TRUE. 
  /// 
  /// \return
  ///   BOOL status : The visibility status of the object
  /// 
  /// \note
  ///   Calling this function is rather cheap, since the visibility status has already been
  ///   calculated
  VISION_APIFUNC BOOL WasVisibleInAnyLastFrame() const;


  /// \brief
  ///   Serializes the entity to the passed archive.
  /// 
  /// The serialization covers all persistent elements of the entity including the animation
  /// configuration. 
  /// 
  /// The Serialize function supports both reading from and writing to the archive. Use
  /// VArchive::IsLoading and VArchive::IsSaving to determine the current operation.
  /// 
  /// Override this function in derived classes to serialize their persistent state. Note that the
  /// base function has to be called when overriding this function.
  /// 
  /// Please refer to the Programmer's Documentation for more information about serialization.
  /// 
  /// \param ar
  ///   The archive object to read from or write to
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );

#ifdef SUPPORTS_SNAPSHOT_CREATION
  
  /// \brief
  ///   Overridden function that gets the resources used for this instance.
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  /// \brief
  ///   Get the current visibility bounding box of the entity in world space
  /// 
  /// The visibility bounding box is used for visibilty calculation, but not for physics.
  /// 
  /// The bounding box may change each frame because the entity moves or because of animation
  /// 
  /// \param Box
  ///   Reference of a bounding box instance that will receive the entity bounding box.
  /// 
  /// \example
  ///   \code
  ///   hkvAlignedBBox bBox;
  ///   pEntity->GetCurrentVisBoundingBox(bBox);
  ///   Vision::Game.DrawBoundingBox(bBox);
  ///   \endcode
  inline void GetCurrentVisBoundingBox(hkvAlignedBBox &Box) const { Box = m_BoundingBox; }

  /// \brief
  ///   Similar to GetCurrentVisBoundingBox, reducing overhead by returning a pointer to the
  ///   bounding box.
  inline const hkvAlignedBBox *GetCurrentVisBoundingBoxPtr() const { return &m_BoundingBox; }


  


  /// \brief
  ///   Sets the visibility bounding box of the entity for the next frame until RemoveCustomVisBoundingBox is called.
  /// 
  /// Use this function to set a custom visibility bounding box.
  /// You may call this function each frame. A custom box remains active until SetCurrentVisBoundingBox
  /// is called with a new box or RemoveCustomVisBoundingBox is called.
  /// To enable automatic update of the visibility bounding box again, call RemoveCustomVisBoundingBox.
  /// 
  /// The visibility bounding box is used for visibilty calculation, but not for physics.
  /// 
  /// Bounding box coordinates are specified in world coordinates, NOT relative to entity's
  /// 
  /// \param bBox
  ///   Reference to a bounding box instance that will be copied to the entity bounding box.
  /// 
  /// \example
  ///   \code
  ///   hkvAlignedBBox bBox;
  ///   pEntity->GetCurrentVisBoundingBox(bBox);
  ///   pEntity->SetCurrentVisBoundingBox(bBox);
  ///   Vision::Game.DrawBoundingBox(bBox);
  ///   \endcode
  VISION_APIFUNC void SetCurrentVisBoundingBox(const hkvAlignedBBox &bBox);

  /// \brief
  ///   Re-activates automatic bounding box update if it was in custom bounding box mode before (via SetCurrentVisBoundingBox)
  inline void RemoveCustomVisBoundingBox()
  {
    m_iVisBoundingBoxComputed = 0;
  }

  /// \brief
  ///   Returns the current collision bounding box of this entity in world space.
  VISION_APIFUNC bool GetCollisionBoundingBox(hkvAlignedBBox &bBox, bool bTranslated=true);
  
  /// \brief
  ///   Sets a custom projection matrix for foreground objects
  /// 
  /// This function only affects entities that are flagged as SetAlwaysInForeGround(TRUE).
  /// 
  /// \param pMatrix
  ///   pointer to a 4x4 projection matrix. If NULL, the global projection matrix will be used
  ///   again. The passed matrix (if not NULL) will be copied.
  /// 
  /// \example
  ///   \code
  ///   hkvMat4 customProj;
  ///   customProj.CreateProjectionMatrix(200, 32000.f);
  ///   pEntity->SetCustomProjectionMatrixForForegroundObject(&customProj);
  ///   \endcode
  VISION_APIFUNC void SetCustomProjectionMatrixForForegroundObject(const hkvMat4* pMatrix);

  /// \brief
  ///   Returns the matrix that has been set via SetCustomProjectionMatrixForForegroundObject, or
  ///   NULL
  VISION_APIFUNC const hkvMat4* GetCustomProjectionMatrixForForegroundObject() const;


  ///
  /// @}
  ///

  ///
  /// @name Object Lighting Related Functions
  /// @{
  ///

  /// \brief
  ///   Sets a bitmask that defines the light influence behaviour
  /// 
  /// This value allows you to configure which dynamic light sources affect this entity. At runtime, the
  /// light's bitmask (set via VisLightSource_cl::SetLightInfluenceBitMasks will be AND-ed with the entity's
  /// bitmask, and if the result is nonzero, the light will affect the entity.
  /// 
  /// \param iLightMask
  ///   The light influence bitmask for this entity. The entity will only be influenced by
  ///   light sources that have a matching bit in their bitmask.
  /// 
  /// \note
  ///   The default bitmask for lights and entities is 1.
  /// 
  /// \sa VisBaseEntity_cl::GetLightInfluenceBitMask
  /// \sa VisLightSource_cl::SetLightInfluenceBitMasks
  /// \sa VisLightSource_cl::GetLightInfluenceBitMasks
  VISION_APIFUNC void SetLightInfluenceBitMask(unsigned int iLightMask);

  /// \brief
  ///   Returns the bitmask that defines the light influence behaviour
  /// 
  /// The bitmask is set via VisBaseEntity_cl::SetLightInfluenceBitMask.
  /// 
  /// \return
  ///   unsigned int iLightMask : The light influence bitmask for this entity. The entity will only be
  ///   influenced by light sources that have a matching bit in their bitmask
  /// 
  /// \sa VisBaseEntity_cl::SetLightInfluenceBitMask
  /// \sa VisLightSource_cl::SetLightInfluenceBitMasks
  /// \sa VisLightSource_cl::GetLightInfluenceBitMasks
  VISION_APIFUNC unsigned int GetLightInfluenceBitMask() const;

  /// \brief
  ///   Sets the ambient color of this entity for rendering the entity.
  /// 
  /// The entity ambient color is added to the light grid color when tracking the light grid color
  /// for this entity.
  /// 
  /// Calling this function will thus invalidate the cached light grid color for a static entity.
  /// 
  /// \param iColor
  ///   ambient color value, use the V_RGB macro to create a color. The alpha portion is ignored.
  /// 
  /// \sa VisBaseEntity_cl::GetAmbientColor
  VISION_APIFUNC void SetAmbientColor(VColorRef iColor);
  

  /// \brief
  ///   Gets the ambient color of the entity.
  /// 
  /// This function returns the color set via VisBaseEntity_cl::SetAmbientColor and does not
  /// consider any global or surface ambient color values.
  /// 
  /// \return
  ///   VColorRef iColor : color value. Alpha part is not defined.
  /// 
  /// \sa VisBaseEntity_cl::SetAmbientColor
  VISION_APIFUNC VColorRef GetAmbientColor() const;


  /// \brief
  ///   Sets the 6 light grid colors for this entity
  /// 
  /// With this function, custom light grid colors can be tracked per entity. This can be more
  /// efficient than using the light grid.
  /// 
  /// This function only has an effect if light grid lighting is active, i.e. there is a light grid
  /// in this map.
  /// 
  /// The colors passed to this function are stored into internal members and uploaded when
  /// required by a shader.
  /// 
  /// Moving the entity (SetPosition etc.) will not invalidate the colors.
  /// 
  /// To go back to tracking from lightgrid when position changes, a NULL pointer can be passed.
  /// 
  /// \param pColors
  ///   A pointer to 6 colors that represent the light grid information, or NULL.
  /// 
  /// \sa VisRenderLoopHelper_cl::TrackLightGridInfo
  VISION_APIFUNC void SetLightGridColors(const hkvVec3* pColors);

  /// \brief
  ///   Returns the absolute light grid sample position in world space
  /// 
  /// This position is used to lookup the light grid color if light grid lighting is used in the
  /// scene.
  /// 
  /// \return
  ///   hkvVec3 vPos : Position vector for sampling the light grid
  /// 
  /// \sa VisBaseEntity_cl::SetRelativeLightGridSamplePos
  inline hkvVec3 GetLightGridSamplePos() const {return GetPosition()+ (hkvVec3) m_vLightgridOfs;}

  /// \brief
  ///   Sets the relative light grid sample offset for this entity
  /// 
  /// This can be useful to tweak the light grid lighting for meshes that have their
  /// pivot at the fringe or even outside of the bounding box and would thus use a light grid color
  /// from that position rather than the box center.
  /// 
  /// The passed offset is relative to the entity's pivot position and will not be rotated with the
  /// entity.
  /// 
  /// The default offset for an entity is (0,0,0) which means it uses its pivot position as a light
  /// grid sample position.
  /// 
  /// \param vRelPos
  ///   relative position offset
  /// 
  /// \sa VisBaseEntity_cl::GetLightGridSamplePos
  VISION_APIFUNC void SetRelativeLightGridSamplePos(const hkvVec3& vRelPos);

  /// \brief
  ///   Returns the relative light grid sample offset for this entity
  /// 
  /// set via SetRelativeLightGridSamplePos
  /// 
  /// \return
  ///   const hkvVec3& vRelPos : relative position offset set via the
  ///   SetRelativeLightGridSamplePos function
  /// 
  /// \sa VisBaseEntity_cl::SetRelativeLightGridSamplePos
  inline const hkvVec3& GetRelativeLightGridSamplePos() const
  {
    return m_vLightgridOfs;
  }

  /// \brief
  ///   Invalidates the lightgrid data so lightgrid colors are looked up in the lightgrid again in the next frame
  inline void InvalidateLightgridData() 
  { 
    m_LightGridData.iLastUpdate = 0; 
  }

  /// \brief
  ///   Return the data structure that holds the current lightgrid data of this entity instance
  inline LightGridData_t *GetLightGridData() 
  { 
    return &m_LightGridData; 
  }

  ///
  /// @}
  ///

  ///
  /// @name Visibility Related Functions
  /// @{
  ///

  /// \brief
  ///   Overrides VVisibilityData::SetVisibleBitmask and additionally propagates message ID to all attached components
  VISION_APIFUNC VOVERRIDE void SetVisibleBitmask(unsigned int iMask);
   
  /// \brief
  ///   Non virtual override of VVisibilityData::SetFarClipDistance. For backwards compatibility it sets the LOD flag as well
  inline void SetFarClipDistance(float fClipDistance)
  {
    VVisibilityData::SetFarClipDistance(fClipDistance);
    SetClipMode((m_fNearClipDistance>0.f || m_fFarClipDistance>0.f) ? VIS_LOD_TEST_BOUNDINGBOX : VIS_LOD_TEST_NONE);
  }



  /// \brief
  ///   With this method, an entity can explicitly be excluded from the visibility test.
  /// 
  /// This also means that the entity will never be reported as visible by visibility collectors
  /// taking this setting into account.
  /// 
  /// \param status
  ///   If TRUE, the entity is excluded from the visibility test. Passing FALSE includes it again.
  inline void SetExcludeFromVisTest(BOOL status)
  {
    if (status)
      m_iPerformTestFlags |= VIS_EXCLUDED_FROM_VISTEST;
    else
      m_iPerformTestFlags &= ~VIS_EXCLUDED_FROM_VISTEST;
  }

  /// \brief
  ///   This method returns whether the entity is currently excluded from the visibility test.
  /// 
  /// \return
  ///   BOOL: If TRUE, the entity is excluded from the visibility test.
  inline BOOL GetExcludeFromVisTest() const
  {
    return (m_iPerformTestFlags&VIS_EXCLUDED_FROM_VISTEST)!=0 ? TRUE : FALSE;
  }


  /// \brief
  ///   Enables or disables hardware occlusion query for this entity.
  ///
  /// \param status: New status; FALSE to disable occlusion query
  ///
  /// The default value is TRUE for all entities. This does however not mean that all these entities are considered for
  /// occlusion query as other criteria might exclude specific entities. Naturally entities without a mesh or a polygon count below a threshold value
  /// are excluded as well. See actual implementation in VisionVisibilityCollector_cl::PostProcessVisibilityResults().
  /// Other visibility tests (portals, frustum culling) are not affected by this flag.
  inline void SetExcludeFromOcclusionQueryTest(BOOL status)
  {
    if (status)
      m_iPerformTestFlags |= VIS_EXCLUDED_FROM_OCCLUSIONQUERY;
    else
      m_iPerformTestFlags &= ~VIS_EXCLUDED_FROM_OCCLUSIONQUERY;
  }


  /// \brief
  ///   Returns the status whether occlusion query is enabled for this entity. The return value does not consider other criteria such as polygon count
  inline BOOL GetExcludeFromOcclusionQueryTest() const
  {
    return (m_iPerformTestFlags&VIS_EXCLUDED_FROM_OCCLUSIONQUERY)>0;
  }

  /// \brief
  ///   Returns the frame number in which the entity was updated for the last time.
  ///   This can be used to compare if an entity was updated in the current frame:
  ///   if (GetLastUpdateCount() == Vision::Game.GetUpdateSceneCount()) { entity moved ... }
  inline int GetLastUpdateCount() const
  {
    return m_iEntityAddedToUpdateList;
  }

  /// \brief
  ///   Returns whether an entity is flagged as always in the foreground via
  ///   SetAlwaysInForeGround(true)
  /// 
  /// \return
  ///   bool: true if the entity is always in the foreground.
  /// 
  /// \sa SetAlwaysInForeGround
  inline bool IsObjectAlwaysInForegroundEnabled() const { return m_bAlwaysInForeground; }

  /// \brief
  ///   Returns whether there are shaders for the given pass type assigned to the entity
  ///
  /// The same information can be gathered from the active VisShaderSet_cl instance assigned to the this entity.
  ///
  /// \param ePassType
  ///   The pass type to test for
  ///
  /// \return
  ///   bool: true if there are shaders with the given pass type, false otherwise
  VISION_APIFUNC bool HasShadersForPass(VPassType_e ePassType) const;


  IMPLEMENT_OBJ_CLASS(VisBaseEntity_cl);


  ///
  /// @}
  ///

  ///
  /// @name Functions For the Scene Update Loop
  /// @{
  ///

  
  /// \brief
  ///   Processes the VisObject3D_cl state flags and calls the ModSysNotifyFunctionHandle functions
  ///   on the children.
  inline void Handle();


  /// \brief
  ///   Resets the status flags
  inline void ResetStatusFlags();


  /// \brief
  ///   Updates the animation state of the animation configuration and applies the motion delta
  ///   caused by the animations.
  VISION_APIFUNC void HandleAnimations(float fTimeDelta);

  /// \brief
  ///   Check whether the core engine states need to be updated.
  /// 
  /// This is the case if either the position, orientation or animation changed.
  VISION_APIFUNC BOOL IsCoreUpdateRequired();


  ///
  /// @}
  ///

  ///
  /// @name Weak Reference Support
  /// @{
  ///

  /// \brief
  ///   Returns a weak reference target for setting up a weak reference to the entity
  /// 
  /// In comparison to usual pointers a weak pointer returns NULL if the entity has been
  /// destroyed.
  /// 
  /// \sa VWeakReference
  /// 
  /// \example
  ///   \code
  ///   // get a weak pointer to an entity and use it as a usual pointer
  ///   VWeakPtr<VisBaseEntity_cl> m_wpRef = pEntity->GetWeakReference();
  ///   m_wpRef->SetVisibleBitmask(0);
  ///   \endcode
  VISION_APIFUNC VWeakRefTarget<VisBaseEntity_cl>* GetWeakReference();
 

  ///
  /// @}
  ///

  ///
  /// @name Visobject3d_cl Overrides
  /// @{
  ///

  /// \brief
  ///   Override this function to keep track of position or orientation modifications.
  /// 
  /// This virtual function (defined in VisObject3D_cl) gets called whenever the position,
  /// orientation or motion/rotation delta of the object has been modified. Please refer to
  /// VisObject3D_cl::OnObject3DChanged for more details.
  /// 
  /// \note
  ///   Don't forget to call the base implementation when overriding this function. 
  /// 
  /// \param iO3DFlags
  ///   VisObject3D_cl::VISOBJECT3D_FLAGS flags indicating the modified components
  VISION_APIFUNC VOVERRIDE void OnObject3DChanged(int iO3DFlags);

  
  ///
  /// @}
  ///

  /*
  /// \brief
  ///   Non-virtual override of VRefCounter::Release to Free the entity rather than deleting it.
  VISION_APIFUNC void Release()
  {
    VASSERT(m_iRefCount>0);
    m_iRefCount--;
    if (m_iRefCount==0)
      Free();
  }
*/

  /// \brief
  ///   SetAnimConfig sets an animation config on the entity.
  /// 
  /// Use it to replace a config, share configs between several entities or remove it with a NULL
  /// pointer.
  /// 
  /// \param pAnimConfig
  ///   Pointer to a new config or NULL.
  /// 
  /// \return
  ///   bool: True if setting the config was successful.
  VISION_APIFUNC bool SetAnimConfig(VisAnimConfig_cl* pAnimConfig);


  /// \brief
  ///   GetAnimConfig returns the currently set animation config of the entity.
  /// 
  /// If you set the returned pointer on another entity it will be animated sychronized and use the
  /// same memory buffers.
  /// 
  /// \return
  ///   VisAnimConfig_cl* pAnimConfig: Pointer to the current config or NULL.
  inline VisAnimConfig_cl* GetAnimConfig() const {return m_spAnimConfig;}

  /// \brief  Query if this entity uses vertex shader skinning. 
  ///
  /// \return true if vertex shader skinned, false if not. 
  ///
  inline bool IsVertexShaderSkinned() const { return m_spAnimConfig ? (m_spAnimConfig->GetSkinningMode() == VIS_SKINNINGMODE_HARDWARE) : false; }


  /// \brief
  ///   Renders the current animated normals of the entity.
  /// 
  /// \param color
  ///   color used for drawing
  /// 
  /// \param fLength
  ///   length of the rendered normals
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void DebugRenderNormals(VColorRef color=V_RGBA_YELLOW, float fLength=5.f);

  /// \brief
  ///   Renders the current animated tangents of the entity.
  /// 
  /// \param color
  ///   color used for drawing
  /// 
  /// \param fLength
  ///   length of the rendered tangents
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void DebugRenderTangents(VColorRef color=V_RGBA_YELLOW, float fLength=5.f);


  /// \brief
  ///   Draw the animated mesh in the current lod level
  /// 
  /// \param iMeshColor
  ///   color used for drawing
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void DebugRenderMesh(VColorRef iMeshColor);


  /// \brief
  ///   Helper function to calculate the bone translation and rotation in worldspace for this
  ///   entity.
  /// 
  /// Since the bone result is in object space, this function is useful to transform the bone
  /// position into world space.
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
  ///   bool bResult: returns true for a valid bone transformation.
  VISION_APIFUNC bool GetBoneCurrentWorldSpaceTransformation(int iBoneIndex, hkvVec3& boneTranslation, hkvQuat &boneRotation);


  /// \brief
  ///   Helper function to calculate the bone translation and rotation in objectspace for this
  ///   entity.
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
  ///   bool bResult: returns true for a valid bone transformation.
  VISION_APIFUNC bool GetBoneCurrentObjectSpaceTransformation(int iBoneIndex, hkvVec3& boneTranslation, hkvQuat &boneRotation);


  /// \brief
  ///   Helper function to calculate the bone translation and rotation in localspace for this
  ///   entity.
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
  ///   bool bResult: returns true for a valid bone transformation.
  VISION_APIFUNC bool GetBoneCurrentLocalSpaceTransformation(int iBoneIndex, hkvVec3& boneTranslation, hkvQuat &boneRotation);


  /// \brief
  ///   Assigns a shader set to this entity
  /// 
  /// A VisShaderSet_cl object contains a set of assignments of shader technique to surfaces. Using
  /// this method, a shader set can be assigned to an entity.
  /// 
  /// If no shader set is assigned to the entity, the entity uses the mesh's default shader set
  /// (VDynamicMesh::GetShaderSet).
  /// 
  /// Shader sets can be shared across multiple entities.
  /// 
  /// \param pShaderSet
  ///   Pointer to the shader set to assign to this entity. Can be NULL to use the mesh's set.
  VISION_APIFUNC void SetShaderSet(VisShaderSet_cl *pShaderSet);

  /// \brief
  ///   Returns the shader set currently assigned to this entity
  /// 
  /// The return value can be NULL if no set is assigned to this entity via SetShaderSet.
  /// 
  /// \return
  ///   VisShaderSet_cl *pShaderSet: Pointer to the shader set assigned to this entity
  VISION_APIFUNC VisShaderSet_cl *GetShaderSet() const;

  /// \brief
  ///   Returns the shader set active for this entity
  /// 
  /// While GetShaderSet always returns NULL when there is no shader set assigned to the entity,
  /// GetActiveShaderSet returns the corresponding mesh's shader set in this case. 
  /// 
  /// \return
  ///   VisShaderSet_cl *pShaderSet: Pointer to the shader set assigned to this entity
  VISION_APIFUNC VisShaderSet_cl *GetActiveShaderSet() const;


  /// \brief
  ///   Specifies an asynchronous preparation task for this entity.
  /// 
  /// Before the entity is animated and rendered, the Vision engine will wait for the preparation
  /// task to complete.
  /// 
  /// Typical preparation task include e.g. vertex animations for cloth simulation (take a look at
  /// the sample cloth simulation shipped with the Vision SDK).
  /// 
  /// \param pTask
  ///   Preparation task for this entity
  VISION_APIFUNC void SetPreparationTask(VThreadedTask *pTask) { m_pPreparationTask = pTask; }

  /// \brief
  ///   Returns the preparation task previously set with SetPreparationTask
  /// 
  /// \return
  ///   VThreadedTask *pTask: Preparation task for this entity
  VISION_APIFUNC VThreadedTask *GetPreparationTask() const { return m_pPreparationTask; }

  /// \brief
  ///   Specifies whether an entity's animation should be updated even when the entity is not
  ///   visible
  /// 
  /// It is recommended to set this value to true if the entity can be moved or turned by an
  /// animation. This is also the default behavior.
  /// 
  /// For performance optimisation reasons, it is strongly recommended to set this value to false
  /// in all cases where an animation can't alter the entity's position or orientation.
  /// 
  /// \param bAlwaysUpdateAnimation
  ///   true if the animation should always be updated, false if the update should only be done
  ///   when the entity is visible.
  VISION_APIFUNC void SetAlwaysUpdateAnimations(bool bAlwaysUpdateAnimation) { m_bAlwaysUpdateAnimation = bAlwaysUpdateAnimation; }

  /// \brief
  ///   Returns the value set with SetAlwaysUpdateAnimations
  /// 
  /// \return
  ///   bool bAlwaysUpdateAnimation: true if the animation is always updated, false if the update
  ///   is only done when the entity is visible.
  VISION_APIFUNC bool GetAlwaysUpdateAnimations() { return m_bAlwaysUpdateAnimation; }

  ///
  /// @name Custom Materials
  /// @{
  ///

  /// \brief
  ///   Sets an instance of custom material textures on the entity
  /// 
  /// This texture set replaces the textures defined in the mesh resource so every entity can use
  /// its own material textures without replacing the material shaders via a shader set.
  /// 
  /// Since the VisSurfaceTextureSet_cl class is reference counted, an instance can be shared
  /// across multiple entities. But note that the number of surfaces in the set has to match
  /// exactly the number of surfaces used by the mesh resource.
  /// 
  /// This set allows programmers to exchange the individual textures of an entity. In default mode, the texture set only replaces
  /// the texture slots (diffuse, normalmap etc.), but not the shaders and other material properties. Accordingly, if the mesh's surface 
  /// doesn't have a normalmap specified, setting a normalmap on the replacement set won't change the outcome, because the applied shader 
  /// has been created for the surface without a normalmap.
  /// A texture set can also allocate an array of VisSurface_cl (se parameter for CreateCustomTextureSet); In that case it even replaces
  /// the shader assignments including custom dynamic light shaders.
  ///
  /// A custom texture set can have an animated texture (.texanim) for the diffuse texture slot. This is
  /// gracefully handled as long as SetCustomTextureSet is called after the animated texture is set in the
  /// structure. If a new animated texture should be used for a surface replacement the same texture set has to be
  /// re-assigned via SetCustomTextureSet because this function initializes the per-entity animation instances.
  /// SetCustomTextureSet must also be called when any surface shader effect has changed in the custom set in extended surface aray mode.
  ///
  /// \param pSet
  ///   The new replacement texture set (or NULL to use the mesh's surface textures again)
  /// 
  /// \note
  ///   Make sure you also change the replacement texture set when you change the entity's mesh
  VISION_APIFUNC void SetCustomTextureSet(VisSurfaceTextureSet_cl *pSet);

  /// \brief
  ///   Returns the current texture replacement set used by this entity (NULL by default)
  inline VisSurfaceTextureSet_cl* GetCustomTextureSet() const
  {
    return m_spTextureSet;
  }

  /// \brief
  ///   Creates and sets a custom texture set that is compatible with the entity's mesh
  /// 
  /// If there is already a custom set, that one is returned (same as GetCustomTextureSet())
  /// 
  /// The returned set is compatible with the current mesh, i.e. it has the same number of
  /// surfaces allocated and each entry is initialized with the respective textures of the mesh
  /// surfaces.
  /// 
  /// \param bCreateSrfInstances
  ///    If true, An array of VisSurface_cl instances is creates rather than just surface textures. This allows for applying distinct shader
  ///    effects as well, including custom dynamic light shader. This feature makes a custom shader set (VisShaderSet_cl) obsolete and in fact
  ///    internally uses that class.
  /// 
  /// \return
  ///   VisSurfaceTextureSet_cl *pSet : The new (or cached) replacement texture set that is
  ///   compatible with the current mesh
  /// 
  /// \sa VisBaseEntity_cl::SetCustomTextureSet
  /// \sa VisBaseEntity_cl::GetCustomTextureSet
  /// \sa VisSurfaceTextureSet_cl
  /// \sa VisSurfaceTextures_cl
  /// 
  /// \note
  ///   Each time a new shader effect is assigned to any of the surfaces (bCreateSrfInstances=true), function SetCustomTextureSet must be called again
  ///   to refresh the shader assignment of the surfaces on the entity.
  /// 
  /// \example
  ///   \code
  ///   // replace the diffuse base texture of material #0 for this entity
  ///   pEntity->CreateCustomTextureSet()->GetTextures(0)->pTexture = Vision::TextureManager.Load2DTexture("newtex.dds");
  ///   \endcode
  VISION_APIFUNC VisSurfaceTextureSet_cl* CreateCustomTextureSet(bool bCreateSrfInstances=false);

  /// \brief
  ///   Access the array of surface pointers that is relevant for rendering. This is either a custom surface set (CreateCustomTextureSet(true)) or the surface array of the mesh.
  VISION_APIFUNC VisSurface_cl** GetSurfaceArray() const;

  ///
  /// @}
  ///

  ///
  /// @name Tagging
  /// @{
  ///

  /// \brief
  ///   Tags the entity. 
  /// 
  /// Tagging can be used to quickly flag a number of entities for a specific operation. For instance, if you
  /// have multiple potentially overlapping collections of entities, you can use tagging to generate a single
  /// list that contains all entities from the collection, but no duplicates. Tagging (including querying and
  /// resetting tags) is a very fast operation.
  ///
  /// \sa VisEntityCollection_cl::GetTaggedEntries
  inline void Tag() { m_iTagged = s_iEntityTagCtr; }


  /// \brief
  ///   Returns whether an entity is tagged. This is a low-overhead operation.
  inline bool IsTagged() const { return m_iTagged == s_iEntityTagCtr; }

  /// \brief
  ///   Resets all entity tags. This is a low-overhead operation.
  static inline void ResetTags() { s_iEntityTagCtr++; }

  ///
  /// @}
  ///

  ///
  /// @name Debug Rendering
  /// @{
  ///

  /// \brief
  ///   Renders the entity's collision mesh as a wireframe.
  /// 
  /// This function is intended for debugging purposes only and may be rather slow.
  /// 
  /// \param iMeshColor
  ///   Color of the wireframe representing the mesh
  ///
  /// \param iNormalColor
  ///   Color of the normal vectors
  ///
  /// \param fNormalLength
  ///   Length of the normal vectors in units
  ///
  /// \sa VisBaseEntity_cl::DebugRenderTraceMesh
  VISION_APIFUNC virtual void DebugRenderCollisionMesh(VColorRef iMeshColor, VColorRef iNormalColor, float fNormalLength);

  /// \brief
  ///   Renders the entity's trace mesh using lines.
  /// 
  /// This function is intended for debugging purposes only and may be rather slow.
  /// 
  /// \param iMeshColor
  ///   Color of the wireframe representing the mesh
  ///
  /// \param iNormalColor
  ///   Color of the normal vectors
  ///
  /// \param fNormalLength
  ///   Length of the normal vectors in units
  ///
  /// \sa VisBaseEntity_cl::DebugRenderCollisionMesh
  VISION_APIFUNC virtual void DebugRenderTraceMesh(VColorRef iMeshColor, VColorRef iNormalColor, float fNormalLength);

  ///
  /// @}
  ///


  ///
  /// @name Lightmapping-related Functions
  /// @{
  ///

  /// \brief
  ///   Fills the passed info with lightmap related information
  VISION_APIFUNC bool GatherLightmapInfo(VLightmapSceneInfo &sceneInfo);

  /// \brief
  ///   Applies lightmap information to this instance
  VISION_APIFUNC bool ApplyLightmapInfo(VLightmapSceneInfo &sceneInfo, VLightmapPrimitive *pPrim);

  /// \brief
  ///   Removes static lightmap information from this
  VISION_APIFUNC void ResetStaticLighting();

  /// \brief
  ///   Returns the occlusion query object for this entity. Used for visibility determination.
  VISION_APIFUNC inline const VisOcclusionQueryObject_cl &GetOccQueryObject() const { return m_OccQueryObject; }

  /// \brief
  ///   Sets the expected number of entities in your scene.
  /// 
  /// Setting this to an adequate number will avoid unneccessary reallocations.
  /// 
  /// This method only has an effect if called prior to loading a scene.
  /// 
  /// \param iCount
  ///   Expected number of entities in the scene.
  VISION_APIFUNC static void SetExpectedCount(int iCount);

  /// \brief
  ///   Returns the expected number of entities previously set with SetExpectedCount.
  VISION_APIFUNC static int GetExpectedCount();

  /// \brief
  ///   Updates the bounding box used for visibility determination
  VISION_APIFUNC void UpdateVisBoundingBox();

  /// \brief
  ///   Updates the worldspace transformation matrix
  VISION_APIFUNC virtual void UpdateWorldMatrix();

  /// \brief
  ///   Overridden method. Clears the motion delta of the entity and its current animation.
  VISION_APIFUNC virtual void ResetMotionDelta() HKV_OVERRIDE;

  inline int GetLastTraceHit() { return lastTraceHit;}
  inline void SetLastTraceHit(int iLastHit) { lastTraceHit = iLastHit;}
  inline static unsigned int GetVisDataOffset() { return offsetof(VisBaseEntity_cl, m_BoundingBox); }

  inline unsigned int GetRenderedTriangleCount() const { return m_iRenderedTriangleCount; }
  inline void ResetRenderedTriangleCount() { m_iRenderedTriangleCount = 0; }
  inline void AddRenderedTriangleCount(unsigned int iNum) { m_iRenderedTriangleCount += iNum; }


  // internal use only
  inline void SetTraceCount(unsigned int iCount) { m_iTraceCount = iCount;}
  inline unsigned int GetTraceCount() const { return m_iTraceCount;}
  inline unsigned int GetLastMoved() const { return m_iLastMoved; }
  inline unsigned int GetLastChanged() const { return m_iLastChanged; }
  VISION_APIFUNC void PrepareForRendering();

  #if defined(_VR_DX11)
    inline VShaderConstantBuffer *GetLightGridConstantBuffer() { return &m_LightGridConstantBuffer; }
  #endif


  ///
  /// @}
  ///

  ///
  /// @name Network related
  /// @{
  ///

  VISION_APIFUNC int GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList);

  ///
  /// @}
  ///



protected:

  friend class VisGame_cl;
  friend class VisProfiling_cl;  
  friend class VisSurface_cl;
  friend class VisRenderStates_cl;
  friend void GetObjectSimpleBoundingBox(int object, hkvAlignedBBox &boundingBox);

  // former entity loader functions:
  static void SplitVariable(char *input, char *variable, char *value);
  
  ///
  /// @name Entity Init / Deinit Functions
  /// @{
  ///

  
  /// \brief
  ///   Initialises the entity variables
  void InitVars(const hkvVec3& origin, const hkvVec3& orientation);


  /// \brief
  ///   Initialises the entity. Called from VisGame_cl::CreateEntity
  void Init(const hkvVec3& origin, const char *customMeshFile = NULL);



  /// \brief
  ///   Returns a new and free internal entity number
  int GetNewNumber();
  


  /// \brief
  ///   Update attached children and the entity core status
  void UpdateDependencies();

  /// \brief
  ///   Adds the entity to the collection of updated entities.
  /// 
  /// This function ensures that the object isn't added to the collection multiple times per frame.
  void MarkAsUpdated();

  /// \brief
  ///   Get the number of the portal which is nearest to the entity
  int GetClosestPortalNumber() const;







  ///
  /// @}
  ///

  ///
  /// @name Entity Update / Handle Functions
  /// @{
  ///

  /// \brief
  ///   Recalculates the bone bounding boxes of this entity
  void UpdateBoneBoundingBoxes();
  
  /// \brief
  ///   Initialise all entity variables which contain information about the assigned mesh
  void InitModelRelatedVars();
  
  
  /// \brief
  ///   Frees all entity variables which contain information about the assigned mesh
  void FreeModelRelatedVars();


  /// \brief
  ///   Internal function
  VISION_APIFUNC void CheckCollectionUpdate();

  ///
  /// @}
  ///

  ///
  /// @name Currently Disabled Enitity Functions
  /// @{
  ///
  

  /// \brief
  /// 
  /// \return
  ///   BOOL: true, if the Think-function is enabled
  /// 
  /// \sa SetThinkFunctionStatus
  BOOL IsThinkFunctionEnabled();


  /// \brief
  /// 
  /// \return
  ///   BOOL: true, if the PreThink-function is enabled
  /// 
  /// \sa SetPreThinkFunctionStatus
  BOOL IsPreThinkFunctionEnabled();


 
  /// \brief
  ///   Draw the specified bone bounding boxes
  BOOL RenderCoreBoneBoundingBox(const char *boneName, VColorRef iColor, float lineWidth = 1.0f);



  void UpdateVisTraceRadius();
//  void UpdateTextureAnimations();
  void OnSubmeshVisibleStateMaskChanged();


  ///
  /// @}
  ///

  ///
  /// @name Entity Variables
  /// @{
  ///

  VDynamicMeshPtr m_spMesh;

  static int g_iExpectedEntityCount;
 
  // structures and class instances
  hkvAlignedBBox *m_pCustomTraceBBox;                    ///< custom trace bounding box (used by tracelines)
  
  // lighting
  hkvVec3 m_vLightgridOfs;                                ///< xyz offset to the entity pivot for looking up the lightgrid color

  // internal caching variables
  int lastTraceHit;

  // physics
  IVisPhysicsObjectPtr m_spPhysicsObject;                   ///< physics object of the entity
  IVCollisionMeshPtr m_spCollisionMesh;                     ///< pointer to (manually overridden) collision mesh
  IVCollisionMeshPtr m_spTraceMesh;                         ///< pointer to (manually overridden) trace mesh
  hkvVec3 m_vScaling;

  // weak reference support
  VSmartPtr<VWeakRefTarget<VisBaseEntity_cl> > m_spWeakRefTarget;
  unsigned int *m_pSubmeshVisMaskPtr;
  unsigned int m_iSubmeshVisMaskNoAlloc;

  VisAnimConfigPtr m_spAnimConfig;

  VisShaderSetPtr m_spShaderSet;

  VThreadedTask *m_pPreparationTask;                        ///< pointer to the currently set preparation task of this entity

  VisSurfaceTextureSetPtr m_spTextureSet;
  int m_iEntityAddedToUpdateList;
  VisOcclusionQueryObjectEntity_cl m_OccQueryObject;

  #if defined(_VR_DX11)
    VShaderConstantBuffer m_LightGridConstantBuffer;
  #endif


  // Make sure the size of the following data is larger than sizeof(float) + sizeof(hkvVec3)
  // flags

  // TODO: Actually, the VisObject3D_cl class should have a cached 4x4 transform. Move it (and the GetWorldMatrix method) there.
  hkvMat4 m_CachedTransform;

  int statusFlags;                                          ///< status flags
  unsigned int m_iEntityFlags;                              ///< entity settings
  ULONG customFlags;                                        ///< custom flags
  VisTraceAccuracy_e traceAccuracy;                         ///< accuracy for traceline operations during API tracline calls
  unsigned int m_iTracelineBitMask;                         ///< bitmask for tracing

  unsigned int m_iTraceCount;
  unsigned int m_iVisBoundingBoxComputed;
  unsigned int m_iTransformationMatrixComputed;
  unsigned int m_iLastChanged;
  unsigned int m_iLastMoved;
  unsigned int m_iRenderedTriangleCount;
  unsigned int m_iTagged;

  VColorRef    m_iAmbientColor;           
  hkvMat4* m_pCustomProjection;
  LightGridData_t m_LightGridData;        // TODO: Make this optional
//  float        m_fLightmapGranularity;

  int m_iLightBitmask;
  float m_fMaxScaling;
  bool m_bHasScaling;
  bool m_bAlwaysInForeground;
  char m_cCastShadowFlags;
  char m_cPrimarySortingKey;
  bool deadNowFlag;                                        ///< if TRUE entity has to be deleted in next frame
  bool m_bCastStaticShadows;  ///< internal variable
  bool m_bAlwaysUpdateAnimation;

  VISION_APIDATA static unsigned int s_iEntityTagCtr;


  ///
  /// @}
  ///

};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisBaseEntity_cl)


#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.inl>

  
/// \brief
///   Class containing data used as a template when constructing a new entity instance
class VisEntityTemplate_cl : public VisObject3DTemplate_cl
{
public:

  /// \brief
  ///   Constructor for an entity creation template
  VisEntityTemplate_cl()
  {
    m_pszModelFile = NULL;
    m_pszVarString = NULL;
    m_bCallInitFunction = true;
  }

  virtual ~VisEntityTemplate_cl()
  {
    // Nothing to be done
  }
  
  /// \brief
  ///   If a custom mesh file has been defined, then this mesh will be taken instead of the
  ///   default mesh of the class.
  const char *m_pszModelFile;
  
  /// \brief
  ///   Optional string that sets the properties of the entity (see
  ///   VisBaseEntity_cl::SetVariablesByString).
  const char *m_pszVarString;


  /// \brief
  ///   Specifies whether the InitFunction of the entity will be called. This value should
  ///   typically always be set to true. Initializations that need to be done before the
  ///   InitFunction call can still be performed via the virtual OnEntityCreated method. The
  ///   possibility to disable the InitFunction call has been introduced to bypass complicated
  ///   managed/native transition issues in vForge.
  bool m_bCallInitFunction;
  
  /// \brief
  ///   This callback function can be used to set additional entity properties before the
  ///   InitFunction is called.
  ///
  /// \param pObj
  ///   Object on which to set the properties
  virtual void OnEntityCreated(VisObject3D_cl* pObj)
  {
  }

};

typedef VNameValueListParser<',', '=', 1024> VVariableStringParser;

#endif // FR_DEFINE_VISAPIBASEENTITY

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
