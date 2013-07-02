/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiRendererNode.hpp
#ifndef DEFINE_VISAPIRENDERERNODE
#define DEFINE_VISAPIRENDERERNODE




#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>

#include <Vision/Runtime/Engine/Renderer/Context/VisApiContext.hpp>

class IVRendererNode;
typedef VSmartPtr<IVRendererNode> IVRendererNodePtr;

/// \brief
/// Simple overlay vertex for the VRendererNodeHelper class.
struct OverlayVertex_t
{
  float fPos[2];  ///< Vertex position
  float fUV[3];   ///< Vertex UV
};

/// \brief
///   post processing target version
enum VRenderTargetVersion_e
{
  VRTV_RESOLVED, ///< get the resolved version of the render target
  VRTV_MSAA      ///< get the MSAA version of the render target (if it does not exist, returns the resolved target instead)
};

/// \brief
///   Helper class providing functionality commonly used in renderer node and post-processor implementations.
///
/// A VRendererNodeHelper instance is specific to a single renderer node. The various methods of this class return
/// information specific to this renderer node once the renderer node helper has been initialized.
class VRendererNodeHelper
{
public:

  /// \brief
  /// Virtual destructor of the VRendererNodeHelper class. Internal use only.
  virtual VISION_APIFUNC ~VRendererNodeHelper() {}

  ///
  /// @name INITIALIZATION AND DEINITIALIZATION
  /// @{
  ///

  /// \brief
  ///   Intializes a VRendererNodeHelper instance for a specific renderer node.
  ///
  /// Call this method before using any of the helper functions contained in this class. 
  /// 
  /// \param pRendererNode
  ///   Renderer node instance this helper class instance is going to be used for.
  VISION_APIFUNC void Init(IVRendererNode *pRendererNode);

  /// \brief
  /// DeIntializes the VRendererNodeHelper instance. 
  ///
  /// This method will clean up all resources allocated within the VRendererNodeHelper instance.
  VISION_APIFUNC void DeInit();

  ///
  /// @}
  ///

  ///
  /// @name COMMONLY USED MESH/VERTEX DATA
  /// @{
  ///

  /// \brief
  /// Returns the far corners of a context's view frustum in eye space.
  /// 
  /// \param pVectors
  /// Out: Pointer to an array of four vectors the frustum corners will be stored in. The corner vertices will be
  /// stored in the following order: top-left / bottom-left / bottom-right / top-right.
  VISION_APIFUNC virtual void GetFrustumFarCorners(hkvVec3* pVectors);

  /// \brief
  /// Returns a mesh buffer defining the view frustum properties for the renderer node.
  ///
  /// The returned mesh buffer has four vertices and consists of two triangles whose position values describe
  /// the device coordinates (-1 to +1 on screen), and whose 3D texture coordinates define the far corners of
  /// the view frustum. This mesh buffer is typically used in postprocessors to reconstruct the position
  /// in world space from the pixel depth in pixel shaders.
  /// If the mesh buffer does not yet exist, it is created when this function is first called.
  /// 
  /// \returns
  /// VisMeshBuffer_cl *pMeshBuffer: Returned mesh buffer.
  VISION_APIFUNC virtual VisMeshBuffer_cl *GetFrustumMeshBuffer();

  /// \brief
  /// Returns a mesh buffer defining a unit sphere.
  ///
  /// This mesh buffer is used e.g. in deferred lighting to render point light sources.
  /// 
  /// \returns
  /// VisMeshBuffer_cl *pMeshBuffer: Returned mesh buffer.
  VISION_APIFUNC virtual VisMeshBuffer_cl *GetSphereMeshBuffer();

  /// \brief
  /// Returns a mesh buffer defining a unit cone.
  ///
  /// This mesh buffer is used e.g. in deferred lighting to render spotlights.
  /// 
  /// \returns
  /// VisMeshBuffer_cl *pMeshBuffer: Returned mesh buffer.
  VISION_APIFUNC virtual VisMeshBuffer_cl *GetConeMeshBuffer();

  /// \brief
  /// Returns an array of six vertices defining a full-screen quad for a viewport which has half the size
  /// of the renderer node's reference context in each direction.
  /// 
  /// The vertex properties will be set so that the position value covers the complete lower-resolution render context (in number of pixels),
  /// while the 2D texture coordinates range from 0 to 1.
  /// This function can be used in post-processing effects to render a full-screen quad in a downscale render context.
  ///
  /// \returns
  /// Overlay2DVertex_t *pVertices: Pointer to an array of six vertices.
  VISION_APIFUNC virtual Overlay2DVertex_t *GetOverlayVerticesHalfSize();

  /// \brief
  /// Returns an array of six vertices defining a "full-screen" quad (i.e., covering the complete reference render context of the renderer node).
  /// 
  /// The vertex properties will be set so that the position value cover the complete screen (in number of pixels),
  /// while the 2D texture coordinates range from 0 to 1.
  /// This function can be used in post-processing effects to render a full-screen quad.
  ///
  /// \returns
  /// Overlay2DVertex_t *pVertices: Pointer to an array of six vertices.
  VISION_APIFUNC virtual Overlay2DVertex_t *GetOverlayVertices();


  /// \brief
  /// Should be called whenever the view frustum properties of the renderer node have changed.
  ///
  /// This function should be called by renderer node implementations whenever their view frustum properties (i.e. FOV,
  /// clip planes, render target size) change.
  VISION_APIFUNC void InvalidateFrustum();

  ///
  /// @}
  ///

private:

  void ComputeOverlayVertices(int iWidth, int iHeight, Overlay2DVertex_t *pVertices);

  VisMeshBufferPtr m_spMeshBuffer;
  bool m_bFrustumMeshBufferDirty;

  VisMeshBufferPtr m_spSphereMeshBuffer;
  VisMeshBufferPtr m_spConeMeshBuffer;
  Overlay2DVertex_t *m_pOverlayVertices;
  Overlay2DVertex_t *m_pOverlayVerticesHalfSize;
  IVRendererNode *m_pRendererNode;
};

/// \brief
///   Interface that defines the rendering paradigm associated with a specific view. 
/// 
/// The renderer node represents a specific renderer (e.g. forward or deferred rendering) used for a specific view.
/// Most games and applications will typically only use a single renderer node, but it is possible to globally
/// register multiple concurrent renderer node instances via Vision::Renderer.SetRendererNode. This is particularly
/// useful e.g. for split-screen or multi-view applications. Custom effects should make sure that they can handle
/// multiple concurrent renderer nodes.
/// 
/// Rendererer Nodes serve as containers for render contexts. Additionally, post-processing effects (derived from
/// VPostProcessingBaseComponent) can be attached to renderer nodes. Ideally, post-processing effects should not
/// need to know which type of renderer node they are attached to.
/// 
/// The Vision Engine ships with two default implementations of this interface: The VDeferredRenderingSystem and the
/// VForwardRenderingSystem classes. It is relatively straightforward to either derive custom renderer implementations
/// from these classes, or write a new renderer node class from scratch. That way, custom render pipelines - including,
/// but not limited to, Light-prepass renderers or inferred lighting - can be implemented without the need to touch
/// other parts of the rendering code.
///
/// For reasons of backwards compatibility, Vision also supports running without any renderer node installed. This means that
/// the default forward renderer is used - a single VisionRenderLoop_cl instance is used for the main render context.
/// Note that Postprocessors and advanced effects requiring renderer node-related data can not be used in this case.
class IVRendererNode : public VisTypedEngineObject_cl, public VRefCounter, public IVRenderContextContainer
{
public:

  /// \brief
  ///   Constructor of the renderer node.
  ///
  /// Creates a new renderer node. Renderer nodes will have to be registered using VisRenderer_cl::SetRendererNode();
  /// otherwise, they will not be taken into account by the engine.
  ///
  /// \param pTargetContext
  ///   The target render context of the renderer node. This can be the main render context (if you want the renderer
  ///   node to output to the backbuffer), or some intermediate render context. Unless the renderer node explicitly
  ///   supports it, renderer node implementations assume that their viewport covers the whole target context.
  VISION_APIFUNC IVRendererNode(VisRenderContext_cl *pTargetContext = NULL);

  /// \brief
  ///   Virtual destructor of the renderer node.
  VISION_APIFUNC virtual ~IVRendererNode();

  /// \brief
  ///  Sets this as the current renderer node, triggers switching callbacks, and performs visibility determination and rendering.
  VISION_APIFUNC virtual void Execute();

  /// \brief
  ///   Overridden base function.
  ///
  /// Recreates the renderer node after setting the variable, unless called inside a Begin/EndPropertyUpdate block.
  ///
  /// \param name
  ///   The name of the variable to update.
  ///
  /// \param value
  ///   A value string that represents the new variable value.
  ///
  VISION_APIFUNC virtual bool SetVariable(const char * name, const char * value) HKV_OVERRIDE;

  /// \brief
  ///   Tells the renderer node that multiple properties will be updated inside a BeginPropertyUpdate/EndPropertyUpdate pair.
  /// 
  /// A BeginPropertyUpdate/EndPropertyUpdate bracket can be used to avoid unnecessary memory allocations and
  /// reduce resource recreation when updating multiple renderer node properties in a row. 
  VISION_APIFUNC virtual void BeginPropertyUpdate();

  /// \brief
  ///   Ends the property update initiated by a call to BeginPropertyUpdate.
  ///
  /// By default, ReInitializeRenderer is called by this method if any properties have been changed inside the update block.
  VISION_APIFUNC virtual void EndPropertyUpdate();

  /// \brief
  ///   Render contexts / special effects can inform the renderer node that they require a linear depth texture for
  ///   the current frame contents using this function.
  ///
  /// Implementations of IVRendererNode need to handle this function. Through this function, rendering effects and
  /// post-processors can inform the renderer node that they require access to a linear depth texture for the current
  /// scene. This linear depth textures has to be in a format which can be accessed inside pixel shaders. Once this
  /// function has been called with bStatus set to true, the IVRendererNode implementation has to return a valid depth
  /// texture (with one frame delay) when GetDepthTexture() is called. Effects and post-processors can indicate that they do not
  /// need a depth texture any more by calling this function with bStatus set to false.
  ///
  /// The bounding box parameter is an optional parameter to indicate that a depth buffer is only needed for a
  /// part of the scene. IVRendererNode implementations can freely decide whether they want to use this information
  /// or not.
  ///
  /// Requesting depth textures is tied to a render context, since effects and post-processors typically use a depth
  /// texture to render to a specific target context. This target context should be passed to SetRequiresDepthTexture.
  ///
  /// \param pObject
  ///   The object which indicates that it needs/does not need a depth buffer.
  ///
  /// \param bStatus
  ///   True to enable depth buffer generation, false to disable it. If multiple contexts indicate that they need
  ///   a depth buffer, it will still only be generated once.
  ///
  /// \param pBox
  ///   Optional bounding box specifying for which part of the scene to generate the depth texture. Can be NULL.
  ///
  /// \returns
  ///   true if the renderer node supports generating a depth texture, otherwise false.
  VISION_APIFUNC virtual bool SetRequiresDepthTexture(void* pObject, bool bStatus, const hkvAlignedBBox *pBox) { return false; }

  /// \brief
  ///   Returns the value previously set for the passed context using SetRequiresDepthTexture.
  VISION_APIFUNC virtual bool GetRequiresDepthTexture(void* pObject, hkvAlignedBBox* pBox) { return false; }

  /// \brief
  ///   Returns a depth texture for the current scene. Implementations of the renderer node should implement this function
  ///   so that it returns a valid depth texture in a shader-accessible format (typically at least 16 or 32 bit floating-point
  ///   or fixed-point accuracy for the red component). The renderer node should take the requests made via SetRequiresDepthTexture
  ///   into account.
  VISION_APIFUNC virtual VTextureObject *GetDepthTexture(VRenderTargetVersion_e eVersion) { return NULL; }

  /// \brief
  ///   Render contexts / special effects can inform the renderer node that they require a resolved color texture for
  ///   the current frame contents using this function.
  ///
  /// Implementations of IVRendererNode need to handle this function. Through this function, rendering effects and
  /// post-processors can inform the renderer node that they require access to a resolved color texture for the current
  /// scene. This color texture has to be in a format which can be accessed inside pixel shaders. Once this
  /// function has been called with bStatus set to true, the IVRendererNode implementation has to return a valid color
  /// texture (with one frame delay) when GetResolvedColorBuffer() is called. Effects and post-processors can indicate that they do not
  /// need a color texture any more by calling this function with bStatus set to false.
  ///
  /// \param pObject
  ///   The object which indicates that it needs/does not need a resolved color buffer.
  ///
  /// \param bStatus
  ///   True to enable color buffer generation, false to disable it. If multiple contexts indicate that they need
  ///   a resolved color buffer, it will still only be generated once.
  ///
  /// \returns
  ///   true if the renderer node supports generating a resolved color texture, otherwise false.
  VISION_APIFUNC virtual bool SetRequiresResolvedColorBuffer(void* pObject, bool bStatus) { return false; }

  /// \brief
  ///   Tells the render node to allocate a resolved color buffer so that manually resolving into it becomes possible.
  ///   
  /// If all objects requiring a resolved color buffer (see SetRequiresResolvedColorBuffer) specify that they are manually
  /// resolving the color buffer, the renderer node should not perform an automatic resolve any more. This is also the behavior
  /// of Vision's standard deferred and forward rendering systems.
  ///
  /// \param pObject
  ///   The object which indicates that it resolved the color buffer.
  ///
  /// \param bStatus
  ///   True to tell the renderer node that the object resolves the color buffer, false to indicate that this is not the case.
  ///
  ///  Call this function to register a object for manual color buffer resolving. After calling this method you can manually
  ///  resolve the color buffer at any point within the rendering of this node by calling ResolveColorBufferManually()
  ///
  /// \param pObject
  ///   The object which indicates that it needs/does not need a resolved color buffer.
  ///
  /// \param bStatus
  ///   True to enable color buffer allocation, false to disable it. If multiple contexts indicate that they need
  ///   a resolved color buffer, it will still only be allocated once.
  ///
  /// \returns
  ///  True if the renderer node supports manual color buffer resolving, false otherwise
  VISION_APIFUNC virtual bool SetResolvesColorBufferManually(void* pObject, bool bStatus) { return false; }

  /// \brief
  ///   Returns the value previously set for the passed context using SetRequiresResolvedColorBuffer.
  VISION_APIFUNC virtual bool GetRequiresResolvedColorBuffer(void* pObject) { VASSERT_MSG(0, "not implemented"); return false; }

  /// \brief
  ///   Returns a resolved color texture for the current scene. Implementations of the renderer node should implement this function
  ///   so that it returns a valid depth texture in a shader-accessible format. The renderer node should take the requests made via 
  ///   SetRequiresResolvedColorBuffer into account.
  VISION_APIFUNC virtual VTextureObject* GetResolvedColorBuffer() { return NULL; }

  /// \brief
  ///  Specifies the render hook which should be used to resolve the color buffer
  VISION_APIFUNC virtual void SetResolveColorBufferRenderHook(unsigned int uiRenderHook) {}

  /// \brief
  ///   Manually resolves the color buffer
  VISION_APIFUNC virtual bool ResolveColorBufferManually() { VASSERT_MSG(0, "not implemented"); return false; }

  /// \brief
  /// Returns the reference context for this renderer node.
  ///
  /// The "reference context" for a renderer node is the render context to which the opaque scene geometry gets
  /// rendered. For deferred rendering, this is the initial g-buffer render context, for forward rendering, this is
  /// the offscreen render target. Initial FOV information and clip planes may be taken from this context, and 
  /// rendering effects may choose to use Render Hooks to render into this context.
  ///
  /// \returns
  ///   A pointer to the reference context for opaque geometry rendering.
  VISION_APIFUNC virtual VisRenderContext_cl *GetReferenceContext() { return NULL; }

  /// \brief
  /// Returns the translucency reference context for this renderer node.
  ///
  /// The "translucency reference context" for a renderer node is the render context to which the translucent scene geometry gets
  /// rendered. For deferred rendering, this is the translucency render context, for forward rendering, this is
  /// typically the same offscreen render target that gets used for opaque geometry. Initial FOV information and clip planes may
  /// be taken from this context, and rendering effects may choose to use Render Hooks to render into this context.
  ///
  /// \returns
  ///   A pointer to the reference context for translucent geometry rendering.
  VISION_APIFUNC virtual VisRenderContext_cl *GetTranslucencyReferenceContext() { return NULL; }

  /// \brief
  /// Returns the target context for this renderer node.
  ///
  /// The target render context for a renderer node is specified as a parameter to the constructor.
  ///
  /// \returns
  ///   A pointer to the target context.
  VISION_APIFUNC virtual VisRenderContext_cl *GetFinalTargetContext() { return m_spFinalTargetContext; }

  /// \brief
  /// Sets the final target context for this renderer node.
  ///
  /// You must explicitly deinitialize the renderer node before calling this method, and initialize the renderer node
  /// afterwards.
  ///
  /// The target render context for a renderer node is set to the passed target context. If the target context
  /// has a visibility collector, it will be used for all contexts that require one. This may set the render context's
  /// property of being the reference context for the visibility collector.
  ///
  /// Implementations of this class should also make sure to forward properties such as cull mode settings and filter mask
  /// from the final target context to all relevant internal contexts.
  ///
  /// \param pNewContext
  ///   A pointer to the target context that should be used as the new final target context.
  VISION_APIFUNC virtual void SetFinalTargetContext(VisRenderContext_cl *pNewContext) {}

  /// \brief
  /// \brief
  /// Enum that defines the accesible G-Buffer render targets.
  enum VGBufferTarget_e
  {
    VGBT_Accumulation     = 0,    ///< The buffer in which the results are accumulated. E.g. all light-sources.
    VGBT_Diffuse          = 1,    ///< Contains only the diffuse color.
    VGBT_Normal           = 2,    ///< Contains the normals of each pixel.
    VGBT_DepthSpec        = 3,    ///< Contains the linear depth of a pixel, and its specular factor.
    VGBT_DepthStencil     = 4,    ///< The depth-stencil render-target. Contains the non-linear depth.
  };

  /// \brief
  /// Returns a G-buffer texture.
  ///
  /// "G-Buffer" (geometry buffer) textures are typically used as an input for postprocessors and rendering effects. They represent
  /// the state of rendering after the initial geometry pass(es). These textures have to be readable in pixel shaders
  /// at all times. Which g-buffer index corresponds to which type of buffer is not fixed, since various renderer
  /// implementations can have different requirements. However, the default setup shared by both the deferred and
  /// forward rendering systems of the Vision Engine is as follows:
  /// \li VGBT_Accumulation: Accumulation Buffer (forward + deferred rendering)
  /// \li VGBT_Diffuse: Diffuse Color Buffer (deferred rendering only; alpha value is specular exponent)
  /// \li VGBT_Normal: Normal Buffer (deferred rendering only; alpha value is material index)
  /// \li VGBT_DepthSpec: Depth Buffer (deferred rendering and forward rendering if depth texture generation is enabled; in
  ///              deferred mode, the green value contains the specular color encoded in a floating-point value). This
  ///              has to be a floating point or fixed point texture with the depth ranging from 0 (near clip plane) to
  ///              1 (far clip plane).
  /// \li VGBT_DepthStencil: (forward + deferred) The depth-stencil render-target used for normal rendering.
  ///
  /// Some effects provided as part of the VisionEnginePlugin may expect the returned buffers to match this data.
  /// If you choose to use a different g-buffer layout, you may have to adapt these effects.
  ///
  /// \param eTarget
  ///   Index of the g-buffer texture to return.
  /// 
  /// \returns
  ///   A pointer to the respective texture.
  VISION_APIFUNC virtual VTextureObject *GetGBuffer(VGBufferTarget_e eTarget) { return NULL; }

  /// \brief
  /// Returns the MSAA G-buffers if available.
  ///
  /// \param eTarget
  ///   Index of the g-buffer texture to return.
  /// 
  /// \returns
  ///   A pointer to the respective texture.
  ///
  /// \sa IVRendererNode::GetGBuffer
  VISION_APIFUNC virtual VTextureObject *GetGBufferMSAA(VGBufferTarget_e eTarget)
  {
    VASSERT_MSG(false, "GetGBufferMSAA not implemented!");
    return NULL;
  }

  /// \brief Returns the view properties object that should be used to change projection settings (Fov, clip planes, etc.) of the renderer node.
  ///
  /// You will have to notify the renderer node using \sa OnViewPropertiesChanged to propagate the changes.
  VISION_APIFUNC VisContextViewProperties* GetViewProperties()
  {
    VASSERT_MSG(GetFinalTargetContext(), "Cannot access view properties before setting the final target context!");
    return GetFinalTargetContext()->GetViewProperties();
  }

  /// \brief Causes the renderer node to propagate the changes to its view properties to attached post processors etc.
  VISION_APIFUNC virtual void OnViewPropertiesChanged();

  /// \brief
  ///  Sets the pixel aspect ratio used for auto FOV computation.
  /// 
  /// The default is the device's pixel aspect ratio which can be queried by
  /// Vision::Video.GetCurrentConfig()->GetPixelAspectRatio(). This value
  /// is usually 1.0f, but may differ on platforms which scale the back buffer
  /// before displaying it on the output device.
  /// 
  /// When the renderer node's output is not intended to be displayed on the
  /// video device directly, but rather used in the scene (e.g. on a mirror
  /// or to be displayed on an in-game monitor), either set the pixel aspect ratio
  /// accordingly (1.0f in most cases), or always supply the full FOV values.
  /// 
  /// \param fPixelAspectRatio
  ///  The pixel aspect ratio to use for FOV computation when either FOV value is zero.
  VISION_APIFUNC virtual void SetPixelAspectRatio(float fPixelAspectRatio);

  /// \brief
  ///  Returns the pixel aspect ratio used for auto FOV computation.
  VISION_APIFUNC virtual float GetPixelAspectRatio();

  /// \brief
  ///   Called when the renderer node is activated.
  /// 
  /// Renderer nodes can be activated and deactivated. Applications can call OnActivate to tell the renderer
  /// node that it should start updating again after it has been disabled using OnDeactivate(). After a call to
  /// OnActivate, the renderer node has to be in a state where it is fully usable (of course, if this was already
  /// the case prior to the call to OnActivate, the method does not have to do anything).
  VISION_APIFUNC virtual void OnActivate() {}

  /// \brief
  ///   Called when the renderer node is deactivated.
  /// 
  /// Renderer nodes can be activated and deactivated. Applications can call OnDeactivate to tell the renderer
  /// node that it should stop updating itself until OnActivate() is called. Implementations may choose to free up
  /// memory and de-initialize the renderer node. 
  VISION_APIFUNC virtual void OnDeactivate() {}

  /// \brief
  ///   Accessor for the internal renderer node helper instance.
  VISION_APIFUNC virtual VRendererNodeHelper *GetRendererNodeHelper();


  /// \brief
  ///   Overridable that returns the base type of the required time-of-day system. Or NULL if time of day is not supported.
  /// 
  /// It is not guaranteed that each implementation of the IVTimeOfDay interface is compatible with each renderer
  /// node. If a renderer node (such as the deferred rendering system) requires a specific time of day system, it
  /// has to return this type through this function. Subclasses of the type returned here will be considered compatible
  /// as well.
  ///
  /// \return
  ///   VType *: Type of the expected time of day system. NULL if time of day is not supported by the renderer node.
  VISION_APIFUNC virtual VType *GetSupportedTimeOfDaySystem() { return NULL; }


  /// \brief
  ///   Xbox360/PS3 only: Minimum Hi-Z/ZCull offset to be used for custom render targets after the initialization of the renderer node.
  /// 
  /// Renderer nodes may desire to reserve a certain amount of Hi-Z/ZCull memory, e.g. for optimization purposes. In order
  /// to tell custom rendering effects which Hi-Z/ZCull range they can still allocate, this function returns the upper bound
  /// of the reserved Hi-Z/ZCull range.
  ///
  /// \return
  ///   int: upper bound of the reserved Hi-Z/ZCull range.
  VISION_APIFUNC virtual int GetHiZOffset() { return 0; }
  
#ifdef _VISION_XENON

  /// \brief
  ///   Xbox360 only: Minimum EDRAM offset to be used for custom render targets after the initialization of the renderer node.
  /// 
  /// Renderer nodes may desire to reserve a certain amount of EDRAM memory, e.g. for optimization purposes. In order
  /// to tell custom rendering effects which EDRAM range they can still allocate, this function returns the upper bound
  /// of the reserved EDRAM range.
  ///
  /// \return
  ///   int: upper bound of the reserved EDRAM range.
  VISION_APIFUNC virtual int GetEdramOffset() { return 0; }

#endif
  ///   Called to initialize the renderer.
  VISION_APIFUNC virtual void InitializeRenderer();

  /// \brief
  ///   Called to de-initialize the renderer.
  VISION_APIFUNC virtual void DeInitializeRenderer();

  /// \brief
  ///  Called to de-initialize and then initialize the renderer
  VISION_APIFUNC virtual void ReInitializeRenderer();

  /// \brief
  ///   Overridable serialization function.
  VISION_APIFUNC virtual void Serialize(VArchive &ar);

  /// \brief
  ///   Static function to read a renderer node instance from a binary stream. The file is compatible with files written by WriteToStream and with renderer node setup exported by vForge
  VISION_APIFUNC static IVRendererNode* ReadFromStream(IVFileInStream *pIn);

  /// \brief
  ///   Writes this renderer node to a binary file (see ReadFromStream). This function is used by vForge when a single instance is exported.
  VISION_APIFUNC void WriteToStream(IVFileOutStream *pOut);

  /// \brief
  ///   Enabled/disables rendering of this renderer node. Lightweight function, use this if you frequently switch between
  ///   renderer nodes without intending to de-initialize them.
  VISION_APIFUNC virtual void SetRenderingEnabled(bool bStatus);

  /// \brief
  ///   Returns whether rendering of this renderer node is currently enabled.
  inline bool GetRenderingEnabled() const { return m_bRenderingEnabled; }

  /// \brief
  ///  Returns whether the renderer node is initialized (ie. has created its contexts and resources)
  inline bool IsInitialized() const
  {
    return m_bIsInitialized;
  }

  /// \brief
  ///   Returns whether a light-source should be exported or discarded.
  VISION_APIFUNC virtual bool IsLightSourceRelevantForExport (const VisLightSource_cl* pLightSource) const {return (false);}

protected:
  VRendererNodeHelper m_RendererNodeHelper;
  VisRenderContextPtr m_spFinalTargetContext;
  bool m_bRenderingEnabled;
  bool m_bIsInitialized;

  float m_fPixelAspectRatio;

  /// \brief
  ///   Flag which signifies whether the renderer properties have changed inside a PropertyUpdate block.
  bool m_bUpdateDirty;

  /// \brief
  ///   Counter to manage nested PropertyUpdate blocks.
  int m_iUpdateCounter;

public:
  V_DECLARE_SERIAL_DLLEXP( IVRendererNode, VISION_APIDATA );
  V_DECLARE_VARTABLE(IVRendererNode, VISION_APIFUNC);
};


/// \brief A helper class that deinitializes and deregisters a renderer node in a given scope and restores it at scope exit.
class VScopedRendererNodeDeinit
{
public:
  /// Constructor. Deregisters and deinitializes pNode.
  VISION_APIFUNC VScopedRendererNodeDeinit(IVRendererNode* pNode);

  /// Destructor. Restores the previous initialization and registration status of the node.
  VISION_APIFUNC ~VScopedRendererNodeDeinit();

private:
  VSmartPtr<IVRendererNode> m_spNode;
  int m_iNodeIdx;
  bool m_bWasInitialized;
};


#endif // DEFINE_VISAPIRENDERERNODE

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
