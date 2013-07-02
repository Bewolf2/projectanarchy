/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionRenderLoop.hpp

#ifndef VISION_RENDERLOOP_HPP_INCLUDED
#define VISION_RENDERLOOP_HPP_INCLUDED

class hkvAlignedBBox;
class IVisRenderLoop_cl;

#include <Vision/Runtime/Engine/System/VisApiCallbacks.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisApiShaderProvider.hpp>

// TODO: Maybe get rid of these at some time :).
#define RLP_MAX_ENTITY_SURFACESHADERS 1024
#define RLP_MAX_ENTITY_SURFACES       1024

typedef VSmartPtr<IVisRenderLoop_cl> IVisRenderLoopPtr; 

class VisRenderCollection_cl;
class VisStaticGeometryInstanceCollection_cl;
class VLightGrid_cl;
class VisMeshBufferObjectCollection_cl;



/// \brief
///   This class implements the engine's default shader provider.
/// 
/// The class is used by the renderloop to manage the various lighting shaders.
class VisionShaderProvider_cl : public IVisShaderProvider_cl, public IVisCallbackHandler_cl
{
public:
  VISION_APIFUNC VisionShaderProvider_cl();
  VISION_APIFUNC virtual ~VisionShaderProvider_cl();

  V_DECLARE_DYNCREATE_DLLEXP(VisionShaderProvider_cl, VISION_APIDATA);


  ///
  /// @name Overridden Functions
  /// @{
  ///


  /// \brief
  ///   Overridden function. See base class.
  VISION_APIFUNC VOVERRIDE VCompiledEffect* CreateMaterialEffect(VisSurface_cl *pSurface, int iFlags=0);
  

  /// \brief
  ///   Overridden function. This implementation loops through all models in the scene and calls
  ///   CreateLightingShaderForModel.
  VISION_APIFUNC VOVERRIDE void CreateLightingShaderForAllModels(bool bIncludeManualAssignments);


  /// \brief
  ///   Overridden function. This implementation creates a shader set and gathers the default
  ///   effect for each surface (using CreateMaterialEffect).
  VISION_APIFUNC VOVERRIDE void CreateLightingShaderForModel(VDynamicMesh *pModel, bool bIncludeManualAssignments);


  /// \brief
  ///   Overridden function. This implementation loops through all world surfaces and gathers the
  ///   default effect for each surface (using CreateMaterialEffect)
  VISION_APIFUNC VOVERRIDE void CreateLightingShaderForWorld(bool bIncludeManualAssignments);


  /// \brief
  ///   Returns the dynamic light shader that matches the parameters passed to this method.
  ///   Additionally sets various parameters in the respective shader according to the provided
  ///   parameters.
  VISION_APIFUNC VOVERRIDE VCompiledTechnique *GetDynamicLightShader(const VisLightSource_cl *pLight, 
    const VisSurface_cl *pSurface, bool bForEntity);


  /// \brief
  ///   Initializes a new frame. Currently just clears a few values.
  VISION_APIFUNC VOVERRIDE void ResetCache();


  /// \brief
  ///   Sets the default lighting color.
  VISION_APIFUNC VOVERRIDE void SetDefaultLightingColor(VColorRef iColor);


  ///
  /// @}
  ///


  ///
  /// @name Specific Functions
  /// @{
  ///
  

  /// \brief
  ///   Implements IVisRenderLoop_cl
  VISION_APIFUNC VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  
    
  /// \brief
  ///   Currently nothing is loaded here, instead all textures are created on demand
  ///   (GetDefaultAttenuationTexture/GetDefaultSpotlightTexture)
  VISION_APIFUNC void LoadDefaultTextures();


  /// \brief
  ///   Releases the texture smart pointers
  VISION_APIFUNC void ReleaseDefaultTextures();


  /// \brief
  ///   Returns the default smooth attenuation curve texture 
  VISION_APIFUNC VTextureObject* GetDefaultAttenuationTexture();


  /// \brief
  ///   Returns the default projected texture
  VISION_APIFUNC VTextureObject* GetDefaultSpotlightTexture();


  /// \brief
  ///   Returns the technique that simply tints with a default color
  VISION_APIFUNC VCompiledEffect* GetDefaultLightingColorEffect(bool bUseAlphaTest = false, 
    float fAlphaThreshold = 0.0f, bool bIsDoubleSided = false, bool bUsesDepthWrite = true);
  
  VTextureObjectPtr m_spAttenSmoothTex;         ///< The default smooth attenuation curve texture.
  VTextureObjectPtr m_spSpotlightTexture;       ///< The default projected texture.
  
  VCompiledTechnique *m_pLastLightTechnique;    
  VisLightSource_cl *m_pLastDynamicLight;
  bool m_bLastDynamicLightForEntity;

  VShaderEffectLibPtr m_spBaseShaderLib;
  VRefCountedCollection<VCompiledEffect> m_DefaultLightingColorEffects;
  
  bool m_bInitialized;


  ///
  /// @}
  ///

};


/// \brief
///   Interface for handling the engine's render loop callback
class IVisRenderLoop_cl : public VRefCounter, public VisTypedEngineObject_cl
{
public:
  V_DECLARE_DYNAMIC_DLLEXP(IVisRenderLoop_cl, VISION_APIDATA);

  /// \brief
  ///   Execute the render loop
  /// 
  /// \param pUserData
  ///   The user data of the render context that calls the rendering. See
  ///   VisRenderContext_cl::GetUserData.
  /// 
  /// \sa VisRenderContext_cl
  /// \sa VisRenderContext_cl::GetUserData
  VISION_APIFUNC virtual void OnDoRenderLoop(void *pUserData) = 0;

};

/// \brief
///   Helper class to collect all visible visibility objects. E.g. particles and mesh buffers
class VVisibilityObjectCollector
{
public:
  /// \brief default constructor
  VISION_APIFUNC VVisibilityObjectCollector();

  /// \brief default destructor
  VISION_APIFUNC ~VVisibilityObjectCollector();

  /// \brief Fills the internal collections with new data from the visibility collector
  VISION_APIFUNC void HandleVisibleVisibilityObjects();

  /// \brief returns a collection of all particle groups
  inline VisParticleGroupCollection_cl& GetParticleGroupCollection() { return m_ParticleGroupCollection; }

  /// \brief returns a collection of all mesh buffer objects
  inline VisMeshBufferObjectCollection_cl& GetMeshBufferObjectCollection() { return m_MeshBufferObjectCollection; }

private:
  VisParticleGroupCollection_cl m_ParticleGroupCollection;
  VisMeshBufferObjectCollection_cl m_MeshBufferObjectCollection;
};


/// \brief
///   This class implements the engine's default render loop. It also serves as a base class for
///   custom render loop implementations.
/// 
/// The protected member functions of the VisionRenderLoop_cl class additionally provide some
/// useful (though not required) helper functionality.
/// 
/// Custom Render Loop implementations can easily be derived from this class (or IVisRenderLoop_cl)
/// and assigned to render contexts using the VisRenderContext_cl::SetRenderLoop method.
class VisionRenderLoop_cl : public IVisRenderLoop_cl, public IVisCallbackHandler_cl
{
public:

  /// \brief
  ///   Render Loop constructor
  /// 
  /// Constructor of the render loop class. 
  VISION_APIFUNC VisionRenderLoop_cl();


  /// \brief
  ///   Render Loop destructor
  /// 
  /// Destructor of the render loop class. 
  VISION_APIFUNC ~VisionRenderLoop_cl();


  /// \brief
  ///   Initializes the render loop
  /// 
  /// Initialization method for the render loop (in case you're writing a custom render loop, take
  /// the base class implementation as a sample).
  /// 
  /// This is the recommended place for allocating memory and defining settings that will be used
  /// over the course of all frames to be rendered with this render loop.
  VISION_APIFUNC virtual void InitRenderLoop();


  /// \brief
  ///   De-initializes the render loop
  /// 
  /// De-initialization method for the render loop (in case you're writing a custom render loop,
  /// take the base class implementation as a sample).
  VISION_APIFUNC virtual void DeInitRenderLoop();


  /// \brief
  ///   Main render loop callback function
  /// 
  /// This is the actual render loop implementation. It serves as a callback function which will be
  /// executed by the engine.
  ///
  /// \param pUserData
  ///   The user data of the render context that calls the rendering. See VisRenderContext_cl::GetUserData.
  /// 
  /// \note
  ///   There are a number of API methods (mostly in the Vision::RenderLoopHelper class) which may
  ///   only be called from this method.
  /// 
  /// \note
  ///   Certain operations are illegal in this method (e.g. creating and deleting entities, lights,
  ///   or similar scene elements; changing basic renderer settings).
  VISION_APIFUNC virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE;

  /// \brief
  ///   Returns the default smooth attenuation curve texture 
  VISION_APIFUNC VTextureObject* GetDefaultAttenuationTexture();


  /// \brief
  ///   Returns the default projected texture
  VISION_APIFUNC VTextureObject* GetDefaultSpotlightTexture();

  /// \brief
  ///   Helper function to trigger a render hook and render all particle groups and mesh buffers associated with it
  ///
  /// \param visibleMeshBuffer
  ///   a collection of all mesh buffers that are visible
  ///
  /// \param visibleParticleGroups
  ///   a collection of all visible particle groups
  ///
  /// \param eRenderHook
  ///   the render hook to trigger
  ///
  /// \param bTriggerCallbacks
  ///   true if the render hook should be triggered, false if only particle systems and mesh buffers should be rendered
  static VISION_APIFUNC void RenderHook(const VisMeshBufferObjectCollection_cl &visibleMeshBuffer, const VisParticleGroupCollection_cl *visibleParticleGroups, VRenderHook_e eRenderHook, bool bTriggerCallbacks);

  /// \brief
  ///   Helper function to trigger a renderhook and render all particle groups and mesh buffers associated with it
  ///
  /// \param visibleMeshBuffer
  ///   a collection of all mesh buffers that are visible
  ///
  /// \param visibleParticleGroups
  ///   a collection of all visible particle groups
  ///
  /// \param eRenderHook
  ///   the render hook to trigger
  inline void RenderHook(const VisMeshBufferObjectCollection_cl &visibleMeshBuffer, const VisParticleGroupCollection_cl *visibleParticleGroups, VRenderHook_e eRenderHook)
  {
    RenderHook(visibleMeshBuffer, visibleParticleGroups, eRenderHook, m_bTriggerCallbacks);
  }

protected:

  /// \brief
  ///   Draws all visible entity-specific shaders
  /// 
  /// This method is responsible for rendering entity-specific shaders on all visible entities.
  /// 
  /// The collection of visible entities is passed to this method.
  /// 
  /// \param EntityCollection
  ///   Reference to a collection of entities to be rendered.
  /// 
  /// \param ePassType
  ///   The Pass type of the shaders that should be drawn
  /// 
  /// \param iTagFilter
  ///   If iTagFilter==VTF_IGNORE_TAGGED_ENTRIES, tagged entries inside the specified collection will not be rendered.
  VISION_APIFUNC virtual void DrawEntitiesShaders(const VisEntityCollection_cl &EntityCollection, VPassType_e ePassType, VTagFilter_e iTagFilter=VTF_IGNORE_NONE);


  /// \brief
  ///   Draws entities which are flagged as always in foreground
  /// 
  /// This method is responsible for rendering all visible foreground entities (i.e. entities which
  /// are always in the foreground due to VisBaseEntity_cl::SetAlwaysInForeground having been
  /// called).
  /// 
  /// The collection of visible entities is passed to this method.
  /// 
  /// \param EntityCollection
  ///   Reference to a collection of entities to be rendered.
  VISION_APIFUNC void DrawForegroundEntities(const VisEntityCollection_cl &EntityCollection);

  /// \brief
  ///   Masks out entities which are flagged as always in foreground
  /// 
  /// This method is responsible for masking out all visible foreground entities (i.e. entities which
  /// are always in the foreground due to VisBaseEntity_cl::SetAlwaysInForeground having been
  /// called). This is done by rendering them to the depth buffer with a simple shader.
  /// 
  /// The collection of visible entities is passed to this method.
  /// 
  /// \param EntityCollection
  ///   Reference to a collection of entities to be rendered.
  VISION_APIFUNC void MaskOutForegroundEntities(const VisEntityCollection_cl &EntityCollection);

  /// \brief
  ///   Draws dynamic lighting on entities and static geometry
  /// 
  /// Draws dynamic lighting and shadows for all entities and static geometry instances,
  /// taking the global engine settings into account.
  VISION_APIFUNC virtual void DrawDynamicLight();


  /// \brief
  ///   Overridden IVisCallbackHandler_cl function to listen to some global events such as
  ///   UnloadWorld
  VISION_APIFUNC VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);


  /// \brief
  ///   Loads and creates several commonly used shaders
  VISION_APIFUNC void CreateBaseShaders();

  /// \brief
  ///   Re-Initializes/re-creates shaders after a new world has been loaded
  VISION_APIFUNC void OnWorldInit();

  /// \brief
  ///   Iterates over all visible visibility objects and creates lists of visible mesh buffers and
  ///   particle groups assigned to them.
  VISION_APIFUNC void HandleVisibleVisibilityObjects();


  static VISION_APIFUNC int GetLightReceiverFlags(const VisLightSource_cl *pLight);
  bool m_bInitialized, m_bTriggerCallbacks, m_bHasRenderHookCallbacks, m_bRenderTransparentPass;

  VISION_APIFUNC void CreateMemexportSkinningShaders();
  VISION_APIFUNC void ReleaseMemexportSkinningShaders();
  
  //////////////////////////////////////////////////////////////////////////////////
  // lightgrid/lightmapping initialisation code and helper functions
  //////////////////////////////////////////////////////////////////////////////////  

  VCompiledTechniquePtr m_spStaticLightShadowTechnique;
  VCompiledTechniquePtr m_spForegroundFillPassTechnique;
  VCompiledTechniquePtr m_spForegroundMaskTechnique;

  // Temporary collections for dynamic illumination and static light shadows
  VISION_APIDATA static VisStaticGeometryInstanceCollection_cl s_ShadowsGeoInstanceCollection;
  VISION_APIDATA static VisEntityCollection_cl s_ShadowsEntityCollection;

  VISION_APIDATA static VisStaticGeometryInstanceCollection_cl s_ShadowsRelevantGeoInstanceCollection;
  VISION_APIDATA static VisEntityCollection_cl s_ShadowsRelevantEntityCollection;

  VISION_APIDATA static VisStaticGeometryInstanceCollection_cl s_LitGeoInstanceCollection;
  VISION_APIDATA static VisEntityCollection_cl s_LitEntityCollection;

  VISION_APIDATA static VisStaticGeometryInstanceCollection_cl s_RenderGeoInstanceCollection;

  VVisibilityObjectCollector m_VisibilityObjectCollector;

  IVisShaderProvider_cl *m_pShaderProvider;                             ///< Only valid during OnDoRenderLoop
  VStateGroupDepthStencil m_dynLightDefaultState;                       ///< Default depth stencil state for rendering dynamic lights (depth equal, no write)
  
public:
  const VisFrustum_cl *m_pCameraFrustum;
  VisStaticGeometryInstanceCollection_cl m_TempGeoInstanceCollection;
  unsigned int m_iFrameCounter;                                         ///< just for arbitrary custom purposes
  VisFrustum_cl m_TempFrustum;                                          ///< can be used for custom purposes; avoids re-allocations

  VTextureObjectPtr m_spAttenSmoothTex;                                 ///< The default smooth attenuation curve texture.
  VTextureObjectPtr m_spSpotlightTexture;                               ///< The default projected texture.

  // VTypedObject
  V_DECLARE_DYNAMIC_DLLEXP(VisionRenderLoop_cl,VISION_APIFUNC)
};


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
