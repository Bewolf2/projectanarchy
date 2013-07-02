/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRendererNodeCommon.hpp

#ifndef VRENDERER_NODE_COMMON_HPP_INCLUDED
#define VRENDERER_NODE_COMMON_HPP_INCLUDED

#include <Vision/Runtime/Engine/Renderer/VisApiRendererNode.hpp>
#if defined(_VISION_PS3) || defined(_VISION_XENON)
  #include <Vision/Runtime/Engine/Renderer/RenderLoop/VisApiHiZHelper.hpp>
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/SimpleCopyPostprocess.hpp>

class VBufferResolver;

/// \brief
///   Enumeration listing the supported particle rendering modes
enum VParticleRenderingMode_e
{
  PARTICLE_RENDERING_MODE_AUTO = 0,         ///< Platform-specific default is chosen for particle rendering (quarter-size on PS3 and Xbox360, full-size on PC)
  PARTICLE_RENDERING_MODE_FULLSIZE = 1,     ///< Particles are rendered at full render-target resolution
  PARTICLE_RENDERING_MODE_QUARTERSIZE = 2   ///< Particles are rendered at a quarter of the render-target resolution, potentially resulting in a significant performance gain in scene with lots of overdraw resulting from particles
};

#ifdef WIN32
#define PARTICLE_RENDERING_MODE_PLATFORMDEFAULT PARTICLE_RENDERING_MODE_FULLSIZE
#elif defined _VISION_WIIU
// TODO WIIU: fix quarter-size rendering
#define PARTICLE_RENDERING_MODE_PLATFORMDEFAULT PARTICLE_RENDERING_MODE_FULLSIZE
#else
#define PARTICLE_RENDERING_MODE_PLATFORMDEFAULT PARTICLE_RENDERING_MODE_QUARTERSIZE
#endif

/// \brief
///   Gamma correction mode
enum VGammaCorrection_e
{
  VGC_None = 0,
  VGC_PreTransform,
  VGC_PostTransform
};

/// \brief
///  Base class for Vision forward and deferred rendering systems which includes common functionality.
///
/// This base class mainly handles the buffer resolver as well has post processor assignment.
///
/// This class (and all derived implementations) are designed to hold the following invariants:
///  - If the renderer node is initialized, all activated post processors must be initialized.
///  - If the renderer node is not initialized, all post processors must be not initialized.
class VRendererNodeCommon : public IVRendererNode, public IVisCallbackHandler_cl
{
public:
  /// @name Constructor
  /// @{

  /// \brief
  ///  Constructor.
  EFFECTS_IMPEXP VRendererNodeCommon();

  /// \brief
  ///  Constructor.
  EFFECTS_IMPEXP VRendererNodeCommon(VisRenderContext_cl* pTargetContext);

  /// \brief
  ///  Destructor.
  EFFECTS_IMPEXP virtual ~VRendererNodeCommon();

  /// @}


  /// @name Rendering properties
  /// @{

  /// \brief
  ///   Use this method to change the eMSAA setting passed to the constructor.
  ///
  /// The renderer node may not be initialized when changing this setting.
  /// 
  /// \param eMultiSampleMode
  ///   The new VVIDEO_Multisample Multisample Mode
  EFFECTS_IMPEXP virtual void SetMultisampleMode(VVIDEO_Multisample eMultiSampleMode) {}

  /// \brief
  ///   Overridable function to return the multisampling mode used by the renderer node.
  EFFECTS_IMPEXP virtual VVIDEO_Multisample GetMultisampleMode() const { return VVIDEO_MULTISAMPLE_OFF; }

  /// \brief
  ///   Override this function to handle gamma correction.
  ///
  /// \note
  ///   See the standard Vision final stage postprocessors for a reference implementation.
  ///
  /// \param gammaCorrection
  ///   Requested gamma correction mode.
  EFFECTS_IMPEXP virtual void SetGammaCorrection(VGammaCorrection_e gammaCorrection) { };

  /// \brief
  ///   Returns the gamma correction mode.
  EFFECTS_IMPEXP virtual VGammaCorrection_e GetGammaCorrection() { return VGC_None; }

  /// \brief
  ///   Use this method to set whether the renderer node should use HDR rendering. Implementations may choose to ignore this.
  ///
  EFFECTS_IMPEXP virtual void SetUseHDR(bool bHDR) {};

  /// \brief
  ///   Returns whether the renderer node uses HDR rendering. Implementations should return the correct value here.
  EFFECTS_IMPEXP virtual bool GetUseHDR() const { return false; }

  /// @}

  /// @name IVRendererNode overrides
  /// @{

  /// \brief
  ///  Renders the registered contexts.
  EFFECTS_IMPEXP virtual void Execute() HKV_OVERRIDE;

  /// \brief
  ///  Overridden method. Propagates the change to all attached post processors.
  EFFECTS_IMPEXP virtual void OnViewPropertiesChanged() HKV_OVERRIDE;

  /// @}

  /// @name Post processor and component handling
  /// @{

  /// \brief
  ///  Overridden method. Automatically handles post processor attachment and context reassignment.
  EFFECTS_IMPEXP virtual BOOL AddComponent(IVObjectComponent *pComponent) HKV_OVERRIDE;

  /// \brief
  ///  Overridden method. Automatically handles post processor attachment and context reassignment.
  EFFECTS_IMPEXP virtual BOOL RemoveComponent(IVObjectComponent *pComponent) HKV_OVERRIDE;

  /// \brief
  ///  Prevent post processor reinitialization inside a Begin/EndUpdate block.
  EFFECTS_IMPEXP virtual void BeginPostProcessorUpdate();

  /// \brief
  ///  Notifies the renderer node that reassigning the post processor contexts is required.
  EFFECTS_IMPEXP virtual void OnPostProcessorChanged();

  /// \brief
  ///  Prevent post processor reinitialization inside a Begin/EndUpdate block.
  EFFECTS_IMPEXP virtual void EndPostProcessorUpdate();

  /// \brief
  ///   Overridable function for removing all custom postprocessors from the renderer node
  EFFECTS_IMPEXP virtual void RemovePostprocessors();

  /// \brief
  ///   Function that returns whether this post processor was created internally by this renderer node.
  ///   This can be useful when looping through renderer's components e.g. to detect duplication.
  EFFECTS_IMPEXP virtual bool IsInternalPostProcessor(VTypedObject *pPostprocessor) { return false; }

  /// \brief
  ///   Used internally; called by vForge to check whether the passed post processor type should be attached by default.
  EFFECTS_IMPEXP virtual bool WantsDefaultPostprocessor(VType *pPostprocessorClass) { return false; }

  /// \brief
  ///   Return true if this renderer node has any render context that outputs directly into the backbuffer
  EFFECTS_IMPEXP bool RendersIntoBackBuffer();

  /// @}


  /// @name Post processor target accessors
  /// @{

  /// \brief
  /// Returns the texture within which color information is accumulated during rendering.
  ///
  /// The color target is the texture (typically a VisRenderableTexture_cl instance) within which color information
  /// is accumulated in the process of rendering the various render contexts in a renderer node. For instance, the
  /// renderer may first render the scene to this buffer, and may then perform a post-processing pass over it.
  /// It is not guaranteed that it is possible to read from this texture without resolving it first; likewise,
  /// custom renderer node implementations do not have to guarantee that it is possible to directly read from this texture.
  ///
  /// \param eVersion
  ///   which version of the post processing color target is wanted (resolved or MSAA)
  ///
  /// \returns
  ///   A pointer to the color target.
  EFFECTS_IMPEXP virtual VTextureObject *GetPostProcessColorTarget(VRenderTargetVersion_e eVersion) { return NULL; }

  /// \brief
  /// Returns the texture within which depth/stencil information is accumulated during rendering.
  ///
  /// The depth/stencil target is the texture (typically a VisRenderableTexture_cl instance) within which depth/stencil information
  /// is accumulated in the process of rendering the various render contexts in a renderer node. 
  /// It is not guaranteed that it is possible to read from this texture without resolving it first; likewise,
  /// custom renderer node implementations do not have to guarantee that it is possible to directly read from this texture.
  ///
  /// \param eVersion
  ///   which version of the post processing color target is wanted (resolved or MSAA)
  ///
  /// \returns
  ///   A pointer to the depth/stencil target.
  EFFECTS_IMPEXP virtual VTextureObject *GetPostProcessDepthStencilTarget(VRenderTargetVersion_e eVersion) { return NULL; }

  /// @}

  /// \brief
  /// Returns which G-buffers are supported by this renderer node.
  ///
  /// Renderer nodes should implement this function to return which g-buffer textures can be expected from the
  /// renderer node. By default, the result is a combination of the bitmask values defined in VBufferFlags.
  ///
  /// \returns
  ///   int: Set of flags representing the supported buffers
  EFFECTS_IMPEXP virtual int GetSupportedBufferFlags() { return 0; }

  /// @name Scene depth and color rendering
  /// @{

  /// \brief
  ///   Renders the accumulation buffer into the current render context.
  ///
  /// \param bHalfSize
  ///   If true, the target context is assumed to be half the size of the full deferred rendering resolution in
  ///   each direction.
  EFFECTS_IMPEXP void RenderSceneTexture(bool bHalfSize = false);

  /// \brief
  ///   Renders the accumulation buffer and its associated depth texture into the current render context.
  ///
  /// This not only renders the accumulation buffer into the current render context, but it also copies the
  /// current depth-stencil target's depth values into the depth buffer of the current render context.
  ///
  /// \platforms
  ///   Xbox360, PS3, DX10, DX11
  ///
  /// \param bHalfSize
  ///   If true, the target context is assumed to be half the size of the full deferred rendering resolution in
  ///   each direction.
  EFFECTS_IMPEXP void RenderSceneTextureWithDepth(bool bHalfSize = false);

  /// \brief
  ///   Renders the depth texture into the current render context.
  ///
  /// This method copies the current depth-stencil target's depth values into the depth buffer of the
  /// current render context.
  ///
  /// \platforms
  ///   Xbox360, PS3, DX10, DX11
  ///
  /// \param bHalfSize
  ///   If true, the target context is assumed to be half the size of the full deferred rendering resolution in
  ///   each direction.
  EFFECTS_IMPEXP void RenderSceneDepth(bool bHalfSize);

#ifdef _VISION_XENON
  /// \brief renders the depth texture into the current render context
  ///
  ///   Special version of RenderSceneDepth for xbox 360 with parameters only available on xbox 360
  ///
  /// \param eRestoreHiZ
  ///   If HiZ should be restored or not
  ///
  /// \param fMinY
  ///   minimal Y to restore (in screen coordinates)
  ///
  /// \param fMaxY
  ///   maximal Y to restore (in screen coordinates)
  EFFECTS_IMPEXP void RenderSceneDepthXbox360(VisHiZHelper_cl::VRestoreHiZ_e eRestoreHiZ, float fMinY, float fMaxY);
#endif

  /// @}


  /// @name Rendering overlays and debug output
  /// @{

  /// \brief 
  ///   Renders overlay geometry, such as screen masks, debug text, debug lines, etc.
  ///
  /// \param b2dObjects
  ///   If true, renders 2D overlay geometry (UI elements, screen masks, 2D lines, text messages, etc.)
  ///
  /// \param b3dObjects
  ///   If true, renders 3D overlay geometry (3D lines, debug triangles, etc.)
  EFFECTS_IMPEXP static void RenderOverlays(bool b2dObjects, bool b3dObjects);

  /// \brief
  ///   Draws visible mesh buffer objects with the specified render order flag.
  EFFECTS_IMPEXP static void DrawMeshBufferObjects(unsigned int iRenderOrder);

  /// @}


  /// @name Buffer resolving
  /// @{

  /// \brief internal flags
  enum VResolvedBufferUsage_e
  {
    VRBU_REQUIRES_RESOLVED = 0x1,
    VRBU_MANUAL_RESOLVE = 0x2
  };

  /// \brief internal method to update the color buffer resolver
  EFFECTS_IMPEXP void UpdateColorBufferResolver();

  /// \brief internal method to set color buffer resolver flags
  EFFECTS_IMPEXP bool SetResolveColorBufferFlag(void* pObject, bool bStatus, VResolvedBufferUsage_e flag);


  /// \brief
  ///   Manually resolves the color buffer
  EFFECTS_IMPEXP bool ResolveColorBufferManually();

  /// \brief
  ///  internal method
  EFFECTS_IMPEXP bool GetRequiresResolvedColorBuffer(void* pObject);

  /// \brief
  ///  internal method
  EFFECTS_IMPEXP VTextureObject* GetResolvedColorBuffer();

  /// \brief
  ///  internal method
  EFFECTS_IMPEXP void SetResolveColorBufferRenderHook(unsigned int uiRenderHook);

  /// @}


protected:
  /// \brief
  ///   Releases any smart pointers to textures or render targets that are saved within a compiled technique
  EFFECTS_IMPEXP static void FreeCustomTextureRefs(VCompiledTechniquePtr &spTech);

  /// \brief
  ///  Initializes shared features, such as the buffer resolver.
  ///
  /// Call this method from within the InitializeRenderer method of your Renderer Node implementation.
  ///
  /// \param pDepthReadTarget
  ///  Render target for reading the depth buffer.
  ///
  /// \param pColorReadTarget
  ///  Render target for reading the color buffer.
  EFFECTS_IMPEXP void InitializeSharedFeatures(VisRenderableTexture_cl* pDepthReadTarget, VisRenderableTexture_cl* pColorReadTarget);

  /// \brief
  ///  Deinitializes shared features, such as the buffer resolver.
  ///
  /// Call this method from within the DeInitializeRenderer method of your Renderer Node implementation.
  EFFECTS_IMPEXP void DeInitializeSharedFeatures();


  /// \brief
  ///  Initializes all attached and active post processors and assigns them a target context.
  ///
  /// Call this method from within the InitializeRenderer method of your Renderer Node implementation
  /// after setting m_bInitialized = true.
  EFFECTS_IMPEXP virtual void InitializePostProcessors();

  /// \brief
  ///  Deinitializes all post processors and removes their target contexts.
  ///
  /// Call this method from within the DeInitializeRenderer method of your Renderer Node implementation
  /// after setting m_bInitialized = false.
  EFFECTS_IMPEXP virtual void DeInitializePostProcessors();

  /// \brief 
  ///   Saves and disables the global wire frame state.
  EFFECTS_IMPEXP void PushAndDisableGlobalWireframeState();

  /// \brief
  ///   Restores a previously saved wireframe state with PushAndDisableGlobalWireframeState.
  EFFECTS_IMPEXP void PopGlobalWireframeState();

#if !defined(_VISION_DOC)
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
#endif

  /// \brief
  ///   Overwritten callback handler function making sure that renderer nodes always gets
  ///   called first if the video resolution changes (OnVideoChanged). 
  EFFECTS_IMPEXP virtual int GetCallbackSortingKey(VCallback *pCallback) HKV_OVERRIDE
  {
     return (pCallback == &Vision::Callbacks.OnVideoChanged) ? -1000 : 0;
  }

  // mesh buffer collection for rendering mesh buffer objects  
  EFFECTS_IMPEXP static VisMeshBufferObjectCollection_cl s_MeshBufferObjectCollection;

protected:
  bool m_bUsesDirectRenderToFinalTargetContext;              ///< Renderer node implementations can choose to set this to true if they don't have any PP effects which require an offscreen RT. The VRendererNodeCommon will then refrain from creating a simply copy postprocessor for the final copy operation and assume that the renderer node outputs directly to the backbuffer.



private:

  bool m_bSavedGlobalWireframeState;

  VBufferResolver* m_pColorBufferResolver;
  unsigned int m_uiResolveColorBufferRenderHook;
  int m_iAutomaticResolveCounter;
  VMapPtrToInt m_ObjectsUsingResolvedColorBuffer;

  VisRenderableTexturePtr m_spColorReadTarget;
  VisRenderableTexturePtr m_spDepthReadTarget;

  VCompiledTechniquePtr m_spSceneDepthTechnique;
  VCompiledTechniquePtr m_spDepthOnlyTechnique;

  VGammaCorrection_e m_gammaCorrection;

  V_DECLARE_SERIAL_DLLEXP(VRendererNodeCommon, EFFECTS_IMPEXP);
  V_DECLARE_VARTABLE(VRendererNodeCommon, EFFECTS_IMPEXP);

#ifdef _VISION_XENON
  VCompiledTechniquePtr m_spDepthOnlyTechnique360;
  bool m_bDepthRestoreInitialized;
  VisHiZHelper_cl::VFastDepthRestoreData m_fastRestoreData;
#endif

#ifdef _VISION_PS3
  VTextureObjectPtr m_spDummyDepthTexture;
#endif

  int m_iPostProcessorUpdateCounter;
  bool m_bPostProcessorAssignmentDirty;

  VScopedRendererNodeDeinit* m_pDeinitDuringVideoResize;

  VRefCountedCollection<VisRenderContext_cl> m_assignedContexts;
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
