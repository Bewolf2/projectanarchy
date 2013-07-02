/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiRenderer.hpp

#ifndef DEFINE_VISAPIRENDERER
#define DEFINE_VISAPIRENDERER

#include <Vision/Runtime/Engine/Renderer/Texture/VisApiTextureManager.hpp>
#include <Vision/Runtime/Engine/Renderer/Context/VisApiContext.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiRendererNode.hpp>


// bitmask constants to enable occlusion query only for some features (see VisRenderer_cl::SetUseHardwareOcclusionFlags)
#define HARDWAREOCCLUSION_ENTITIES      0x00000001
#define HARDWAREOCCLUSION_PORTALS       0x00000002
#define HARDWAREOCCLUSION_ALL           0x0000FFFF


// evaluation flags for VisRenderer_cl::EvaluateBrightnessAtPointEx function
#define EVALUATELIGHTINGFLAG_USE_AFFECTMODELS_FLAG        0x00000001
#define EVALUATELIGHTINGFLAG_USE_AFFECTWORLD_FLAG         0x00000002
#define EVALUATELIGHTINGFLAG_STATICLIGHTS                 0x00000004
#define EVALUATELIGHTINGFLAG_DYNAMICLIGHTS                0x00000008
#define EVALUATELIGHTINGFLAG_ALL_LIGHTS                   (EVALUATELIGHTINGFLAG_STATICLIGHTS|EVALUATELIGHTINGFLAG_DYNAMICLIGHTS)

#define EVALUATELIGHTINGFLAG_CLAMP_RESULT_COLOR           0x00000100
#define EVALUATELIGHTINGFLAG_CLAMP_EACH_COLOR             0x00000200
#define EVALUATELIGHTINGFLAG_NORMALIZECOLOR               0x00000400
#define EVALUATELIGHTINGFLAG_ADD_AMBIENT                  0x00000800

#define EVALUATELIGHTINGFLAG_TRACELINE_MODELS             0x00010000
#define EVALUATELIGHTINGFLAG_TRACELINE_WORLD              0x00020000
#define EVALUATELIGHTINGFLAG_TRACELINES                   0x000f0000


#define V_MAX_RENDERER_NODES            16


class IVTimeOfDay;
typedef VSmartPtr<IVTimeOfDay> IVTimeOfDayPtr;

/// \brief
///   Interface for time of day properties
/// 
/// This class is the base class for time of day implementations. It provides a general interface for commonly used
/// time of day properties. Notice that specific renderer node implementations may require additional features, so it
/// is not safe to assume that every renderer node type can be used with every time of day implementation. However,
/// various engine features (e.g. VSunGlare, Water rendering) use the time of day interface to query common time of
/// day properties.
/// 
/// The time of day instance can be registered using Vision::Renderer.SetTimeOfDayHandler().
///
/// The data returned by the time of day handler is also used to set up the engine's global time of day shader constants.
class IVTimeOfDay : public VisTypedEngineObject_cl, public VRefCounter
{
public:

  /// \brief
  ///   Constructor of the time of day interface.
  VISION_APIFUNC IVTimeOfDay() {m_bLightgridNeedsUpdate=true;}

  /// \brief
  ///   Virtual destructor of the time of day interface.
  VISION_APIFUNC virtual ~IVTimeOfDay() {}

  /// \brief
  ///   Virtual function to set the time of day. Time of day may range from 0.0f (0:00) to 1.0f (24:00).
  VISION_APIFUNC virtual void SetDayTime(float fTime) = 0;

  /// \brief
  ///   Returns the current time of day.
  VISION_APIFUNC virtual float GetDayTime() = 0;

  /// \brief
  ///   Increments the current time of day.
	/// 
	/// \param fTimeDiff
  /// Time difference to apply
  VISION_APIFUNC virtual void IncDayTime(float fTimeDiff) = 0;

  /// \brief
  ///   Returns the direction of incoming sun light for the current time of day.
  VISION_APIFUNC virtual void GetSunDirection(hkvVec3& vDirection) = 0;

  /// \brief
  ///   Returns the direction of incoming sun light for a given day time.
  VISION_APIFUNC virtual void GetSunDirectionAtDayTime(hkvVec3& vDirection, float fTime) = 0;

  /// \brief
  ///   Returns the sun color for the current time of day.
  VISION_APIFUNC virtual VColorRef GetSunColor() = 0;

  /// \brief
  ///   Returns the ambient color for the current time of day.
  VISION_APIFUNC virtual VColorRef GetAmbientColor() = 0;

  /// \brief
  ///   Static function to read a time of day instance from a binary stream. The file is compatible with files written by WriteToStream and with single time of day instances exported by vForge
  VISION_APIFUNC static IVTimeOfDay* ReadFromStream(IVFileInStream *pIn);

  /// \brief
  ///   Writes this time of day instance to a binary file (see ReadFromStream). This function is used by vForge when a single instance is exported.
  VISION_APIFUNC void WriteToStream(IVFileOutStream *pOut);

  /// \brief
  ///   Fill out the lightgrid colors with current sun direction and ambient
  VISION_APIFUNC const hkvVec3* GetLightGridColors();

  /// \brief
  ///   Must be called by custom implementations whenever the daytime changes
  inline void InvalidateLightGridColors()
  {
    m_bLightgridNeedsUpdate = true;
  }

  bool m_bLightgridNeedsUpdate;
  hkvVec3 m_CachedLightgridColors[6];

  V_DECLARE_DYNAMIC_DLLEXP( IVTimeOfDay, VISION_APIDATA );
};

/// \brief
///   A structure describing the global fog parameters.
///
struct VFogParameters
{
  /// \brief
  ///    Constructor
  inline VFogParameters()
    : depthMode(Off)
    , fDepthStart(0)
    , fDepthEnd(32000.f)
    , iDepthColor(100,120,140,255)

    , heightFogMode(Off)
    , fHeightFogDensity(0.01f)
    , iHeightFogColor(255,255,255,255)
    , iHeightFogColorOffset(255,255,255,255)
    , fHeightFogHalfDensityHeight(1000.0f)
    , fHeightFogOffset(0)
    , fHeightFogStart(0)
    , fHeightFogEnd(10000)
    , bHeightFogAddScattering(false)
    , bMaskSky(true)
  {
  }

  inline bool operator == (const VFogParameters &other)
  {
    return depthMode == other.depthMode && fDepthStart == other.fDepthStart && fDepthEnd == other.fDepthEnd && iDepthColor == other.iDepthColor
      && heightFogMode == other.heightFogMode && fHeightFogDensity == other.fHeightFogDensity && iHeightFogColor == other.iHeightFogColor
      && iHeightFogColorOffset == other.iHeightFogColorOffset && fHeightFogHalfDensityHeight == other.fHeightFogHalfDensityHeight
      && fHeightFogOffset == other.fHeightFogOffset && fHeightFogStart == other.fHeightFogStart && fHeightFogEnd == other.fHeightFogEnd
      && bHeightFogAddScattering == other.bHeightFogAddScattering && bMaskSky == other.bMaskSky;
  }

  inline bool operator != (const VFogParameters& other)
  {
    return !(*this == other);
  }

  enum Mode
  {
    Off = 0,
    On = 1,
    TimeOfDay = 2
  };

  Mode depthMode;                     ///< Determines whether the depth fog is disabled, enabled, or driven by the time of day system.
  float fDepthStart;                  ///< Near z value for depth fog.
  float fDepthEnd;                    ///< Far z value for depth fog.
  VColorRef iDepthColor;              ///< Depth fog color

  Mode heightFogMode;                 ///< Determines whether the depth fog is disabled, enabled, or driven by the time of day system.
  float fHeightFogDensity;            ///< Ground fog density
  VColorRef iHeightFogColor;          ///< Ground fog color
  VColorRef iHeightFogColorOffset;    ///< Ground fog color offset (driven by e.g. atmospheric scattering)
  float fHeightFogHalfDensityHeight;  ///< The height at which ground fog falls off to half the initial value.
  float fHeightFogOffset;             ///< Height offset from 0
  float fHeightFogStart;              ///< Near z value for ground fog
  float fHeightFogEnd;                ///< Far z value for ground fog
  bool bHeightFogAddScattering;       ///< If true, ground fog will receive contribution from atmospheric scattering
  bool bMaskSky;                      ///< If true, sky is masked out for fog rendering 
};

extern VFogParameters g_fogParameters;


/// \brief
///   Flags for the VisGlobalRendererSettingsDataObject_cl callback. Used to indicate which renderer properties changed.
enum VGlobalRenderPropertyFlags
{
  VGRP_NONE                         = 0x0,              ///< No changes
  VGRP_AMBIENT_COLOR                = 0x00000001,       ///< Indicates that the global ambient color changed
  VGRP_FOG_PROPERTIES               = 0x00000002,       ///< Indicates that the global fog properties changed
  VGRP_MAX_ANISOTROPY               = 0x00000004,       ///< Indicates that the global maximum anisotropy changed
  VGRP_TEXTURE_DETAIL               = 0x00000008,       ///< Indicates that the texture detail changed
  VGRP_TEXTURE_LOD_BIAS             = 0x00000010,       ///< Indicates that the texture LOD bias changed
  VGRP_ALPHA_THRESHOLD              = 0x00000020,       ///< Indicates that the alpha threshold changed
  VGRP_DEFAULT_CLEAR_COLOR          = 0x00000040,       ///< Indicates that the default clear color changes.
  VGRP_SRGB_MODE                    = 0x00000080,       ///< Indicates that the global SRGB mode changed
  VGRP_WIREFRAME_MODE               = 0x00000100,       ///< Indicates that wireframe rendering was globally enabled/disabled.
  VGRP_DEFAULT_LIGHTING_COLOR       = 0x00000200,       ///< Indicates that the default lighting color changed.
  VGRP_LIGHTING_MODE                = 0x00000400,       ///< Indicates that the lighting mode changed.
  VGRP_LIGHT_GRID_MULTIPLIER        = 0x00000800,       ///< Indicates that the global light grid multiplier changed.
  VGRP_DEFAULT_TEXTURE_FILTER_MODE  = 0x00001000,       ///< Indicates that the default texture filter mode changed.
  VGRP_FOG_TOGGLE                   = 0x00002000,       ///< Indicates that fog was enabled or disabled.
  VGRP_NEW_TIMEOFDAY_HANDLER        = 0x00004000        ///< Indicates that a new time of day handler has been set (can also be NULL)
};





/// \brief
///   Class for general renderer settings
/// 
/// This class contains general renderer settings for the Vision Engine, including features like
/// debug output, geometry settings, and the like.
class VisRenderer_cl
{
public:


  VISION_APIFUNC VisRenderer_cl();

  VISION_APIFUNC virtual ~VisRenderer_cl() { }

  ///
  /// @name Renderer Node
  /// @{
  ///

  /// \brief
  ///   Registers/unregisters a new renderer node.
  ///
  /// The Vision engine supports up to V_MAX_RENDERER_NODES different renderer nodes to be registered at the same time.
  /// Each renderer node can, for instance, represent a separate view in a split-screen application.
  /// Every renderer node that has been registered using the SetRendererNode function will be processed by the VisionApp_cl::Run()
  /// function, meaning that all renderer contexts which are registered with these renderer nodes will have their
  /// visibility collectors processed, and will then be rendered. The renderer nodes are processed in-order by the
  /// default implementation of the VisionApp, i.e. renderer node 0 will be processed first, then node 1, and so on.
  /// A renderer node can be registered by setting it to a specific node index using SetRendererNode. Setting this index
  /// to NULL will unregister the renderer node again. Whenever a context is registered or unregistered, the OnRendererNodeChanged
  /// callback is triggered. It is allowed to leave gaps (i.e., entries set to NULL) in the list of registered renderer nodes.
  /// 
  /// At the moment, Vision only allows one global Shader Provider implementation. This means that you should not register
  /// renderer nodes using different shader providers.
  ///
  /// If no renderer node is registered, only render contexts globally registered with the VisRenderContextManager_cl class
  /// will be handled by the VisionApp_cl::Run function.
  ///
  /// \param iNodeIndex
  ///   Index at which to set the renderer node. Must be between 0 and V_MAX_RENDERER_NODES.
  ///
  /// \param pRendererNode
  ///   New renderer node. Can be NULL to deregister a renderer node.
  ///
  /// \sa
  ///   IVRendererNode
  VISION_APIFUNC void SetRendererNode(int iNodeIndex, IVRendererNode* pRendererNode);

  /// \brief
  ///   Returns the current render node that has been set via SetRendererNode.
  inline IVRendererNode* GetRendererNode(int iNodeIndex) const 
  {
    VASSERT(iNodeIndex>=0 && iNodeIndex < V_MAX_RENDERER_NODES);
    return m_spRendererNode[iNodeIndex];
  }

  /// \brief Returns the index at which a renderer node is registered, or -1.
  inline int GetRendererNodeIndex(IVRendererNode* pRendererNode) const 
  {
    for(int iNodeIdx = 0; iNodeIdx < V_MAX_RENDERER_NODES; iNodeIdx++)
    {
      if(m_spRendererNode[iNodeIdx] == pRendererNode)
      {
        return iNodeIdx;
      }
    }

    return -1;
  }

  /// \brief
  ///   Returns the active renderer node that is currently being rendered. It is set via SetCurrentRendererNode from inside the application loop
  inline IVRendererNode* GetCurrentRendererNode() const 
  { 
    return m_spCurrentRendererNode; 
  }

  /// \brief
  ///   Sets the currently active renderer node. Actual application Run loop implementations must call this when looping through all renderer nodes. See VisionApp_cl::Run() implementation.
  inline void SetCurrentRendererNode(IVRendererNode *pNode) 
  { 
    m_spCurrentRendererNode = pNode; 
  }


  /// \brief
  ///   Helper function to determine whether the first installed renderer node is not NULL and derived from the passed class type
  inline BOOL IsRendererNodeOfType(VType *pType, int iNodeIndex=0) const 
  {
    VASSERT(iNodeIndex>=0 && iNodeIndex < V_MAX_RENDERER_NODES);
    return (m_spRendererNode[iNodeIndex]!=NULL) && m_spRendererNode[iNodeIndex]->IsOfType(pType);
  }

  /// \brief
  ///   Helper function to retrieve a flat list of all used render contexts in the engine.
  ///
  /// To retrieve all render contexts includes the contexts added to the Vision::Contexts container plus the contexts in all registered
  /// renderer nodes. This convenience function builds a flat list out of all these contexts.
  ///
  /// \param pDestList
  ///   A list of pointers that receive the context pointers
  ///
  /// \param iMaxCount
  ///   The maximum amount of elements to be stored in the list (array size)
  ///
  /// \param iRenderFlagFilterMask
  ///   A context is only added to the list if all passed flags are in the VisRenderContext_cl::GetRenderFlags bitmask. For instance, VIS_RENDERCONTEXT_FLAG_VIEWCONTEXT
  ///   can be passed to filter the view contexts.
  ///
  /// \return
  ///   The number of elements in the destination list
  VISION_APIFUNC int GetAllRenderContexts(VisRenderContext_cl **pDestList, int iMaxCount, int iRenderFlagFilterMask=0);


  ///
  /// @}
  ///

  ///
  /// @name Time of Day
  /// @{
  ///


  /// \brief
  ///   Sets the global time of day handler
  ///
  /// This function registers the global time of day handler, which can be used by renderer node implementations, and which
  /// is also used to obtain global sun direction and sun color data to update the global shader constants.
  /// Note that specific renderer node implementations may require specific time of day handler implementations.
  ///
  /// \param pTimeOfDay
  ///   New time of day handler. May be NULL.
  VISION_APIFUNC void SetTimeOfDayHandler(IVTimeOfDay *pTimeOfDay);

  /// \brief
  ///   Returns the global time of day handler.
  inline IVTimeOfDay *GetTimeOfDayHandler() const
  {
    return m_spTimeOfDay;
  }

  /// \brief
  ///   User can set a custom factory class to create custom instances of class IVTimeOfDay
  ///
  /// Note that this factory is only required inside the editor, because time of day instances loaded from exported scenes are de-serialized and thus
  /// use the class that was used at export time. vForge uses this function whenever a new instance must be created.
  /// The VisionEnginePlugin installs a factory that creates instances VTimeOfDay, as the engine itself does not provide any time of day implementation
  /// beside the interface.
  ///
  /// \param pFactory
  ///   The factory instance (or NULL)
  inline void SetTimeOfDayFactory(IVObjectInstanceFactory *pFactory)
  {
    m_pTimeOfDayFactory = pFactory;
  }

  /// \brief
  ///   Returns the global factory instance (or NULL)
  inline IVObjectInstanceFactory* GetTimeOfDayFactory() const
  {
    return m_pTimeOfDayFactory;
  }

  /// \brief
  ///   Used by the editor to create a time of day instance via the installed factory (using GetTimeOfDayFactory)
  VISION_APIFUNC IVTimeOfDay* CreateTimeOfDayInstance();

  ///
  /// @}
  ///


  ///
  /// @name Global Settings
  /// @{
  ///
    
  /// \brief
  ///   Sets the texture detail to be used.
  /// 
  /// VisRenderer_cl::SetTextureDetail allows you to specify whether the Vision Engine should use lower-resolution
  /// versions of textures for rendering. VIS_TEXDETAIL_FULL means that all mip levels of each
  /// texture will be stored in memory and used for rendering. VIS_TEXDETAIL_REDUCED only uses mip
  /// levels 1..n (rather than 0..n) and therefore requires less system and video memory. The
  /// maximum texture resolution will be one fourth of the full resolution. VIS_TEXDETAIL_LOW only
  /// uses mip levels 2..n (rather than 0..n) and therefore requires less system and video memory.
  /// The maximum texture resolution will be one sixteenth of the full resolution.
  /// 
  /// \param detail
  ///   detail level to be used (see description)
  /// 
  /// \note
  ///   This settings only afects textures that are loaded afterwards. It does not affect textures that are loaded already.
  /// 
  /// \note
  ///   The behavior can still be controlled on a per texture level: If specific textures should not be reduced, they can be loaded in full resolution by passing the VTM_FLAG_NO_DOWNSCALE
  ///   flag to Vision::TextureManager.Load2DTexture.
  /// 
  /// \note
  ///   This method should primarily be used for testing purposes. For best loading performance, it
  ///   is recommended to have separate texture sets for systems with different amounts of texture
  ///   memory.
  VISION_APIFUNC void SetTextureDetail(VIS_CFG_TextureDetail detail);

  /// \brief
  ///   Returns the texture detail previously set with SetTextureDetail. The default value is 
  VISION_APIFUNC VIS_CFG_TextureDetail GetTextureDetail();

  /// \brief
  ///   Sets the global maximum anisotropy of textures
  /// 
  /// This method will only have an effect on textures whose sampler uses anisotropic filtering
  /// (this is a property of the shader system).
  /// 
  /// The value passed reflects the maximum anisotropy. The higher the maximum anisotropy, the
  /// higher the fillrate impact associated with anisotropic filtering.
  /// 
  /// If the graphics card used does not support anisotropic filtering, it will simply not be used.
  /// Additionally, the maximum anisotropy value will be clamped to the supported range of the
  /// graphics hardware. The default value is 1.0f, which corresponds to no anisotropic filtering.
  ///
  /// This feature is not supported in DirectX11; calling this function will have no effect.
  /// 
  /// \param max_anisotropy
  ///   values between 1.0f and 256.0f are allowed.
  VISION_APIFUNC void SetMaxAnisotropy(float max_anisotropy);


  /// \brief
  ///   Gets the global maximum anisotropy of textures.
  /// 
  /// This function returns the value set with VisRenderer_cl::SetMaxAnisotropy.
  /// 
  /// Since the anisotropy value set with VisRenderer_cl::SetMaxAnisotropy is clamped to the graphics card's
  /// supported range, the value returned by this method is not necessarily the same value
  /// previously set with SetMaxAnisotropy.
  /// 
  /// \return
  ///   float: Maximum anisotropy
  VISION_APIFUNC float GetMaxAnisotropy();


  /// \brief
  ///   Sets the global texture LOD bias
  /// 
  /// When mipmapping is used, this function can be used to globally change the texture detail
  /// bias. Values above 0 lead to lower-resolution mipmap levels being used, thus making the image
  /// more blurry. Values below 0 lead to a more crisp look, but are also more likely to cause
  /// undersampling artifacts and may have a negative effect on texture read performance.
  /// 
  /// Note that the effect of this setting depends on the platform and graphics chip. Some
  /// implementations may, for instance, truncate the passed float values to integer values.
  ///
  /// This feature is not supported in DirectX11; calling this function will have no effect.
  /// 
  /// \param lod_bias
  ///   global texture LOD bias, between -8.0f and +8.0f. 
  VISION_APIFUNC void SetTextureLODBias(float lod_bias);

  /// \brief
  ///   Returns the global texture LOD bias.
  /// 
  /// \return
  ///   float lod_bias: Current texture LOD bias.
  ///
  /// \see VisRenderer_cl::SetTextureLODBias
  VISION_APIFUNC float GetTextureLODBias();

  /// \brief
  ///   Sets the global default threshold for alpha tests.
  /// 
  /// For an incoming fragment (i.e. texel), the alpha value in the range 0..1 is compared to
  /// this threshold value. If the test fails (i.e. incoming fragment alpha < threshold), then
  /// the fragment is eliminated, and no write operation to frame or depth buffer is performed.
  /// 
  /// This alpha test value can be overriden by shaders.
  /// 
  /// Alpha Test is not necessarily supported on floating-point frame buffers. The details depend
  /// on the platform and graphics card used.
  /// 
  /// This value can be overridden on a per-surface basis. See
  /// VisSurface_cl::GetAlphaTestThreshold.
  /// 
  /// \param value
  ///   Alpha Threshold (0.0f...1.0f)
  /// 
  /// \note
  ///   This function may be called at startup or runtime of the engine.
  VISION_APIFUNC void SetGlobalAlphaTestThreshold(float value);
  
  /// \brief
  ///   Returns the global threshold for alpha tests
  /// 
  /// \sa VisRenderer_cl::SetGlobalAlphaTestThreshold
  VISION_APIFUNC float GetGlobalAlphaTestThreshold() const;

  /// \brief
  ///   Sets the default clear color that should be used by all renderloop implementations 
  VISION_APIFUNC void SetDefaultClearColor(VColorRef iColor);

  /// \brief
  ///   Returns the default clear color that has previously been set via SetDefaultClearColor
  inline VColorRef GetDefaultClearColor() const
  {
    return m_iDefaultClearColor;
  }

  /// \brief
  ///   Sets how textures are handled in the rendering pipeline with regard to SRGB.
  /// \param eMode
  ///   the mode to set
  VISION_APIFUNC void SetSRGBMode(VSRGBMode eMode);

  /// \brief
  ///   Returns how textures are handled in the rendering pipeline with regard to SRGB.
  /// \return
  ///   the currently set SRGB mode
  VISION_APIFUNC VSRGBMode GetSRGBMode() const;

  ///
  /// @}
  ///

  ///
  /// @name Primitive/entity Sorting
  /// @{
  ///
  
  /// \brief
  ///   Enables/Disables sorting of transparent static geometry
  /// 
  /// SetSortTranslucentPrimitives allows you to enable and disable sorting of transparent static
  /// geometry in the engine. Translucent primitives are always sorted in back-to-front order.
  /// 
  /// It is strongly recommended to enable this feature in order to avoid visual artifacts.
  /// 
  /// \param bStatus
  ///  Set to true/false to enabled/disable sorting. true is default.
  inline void SetSortTranslucentPrimitives(bool bStatus) { m_bSortTranslucentPrimitives = bStatus; }

  /// \brief
  ///   Returns the value previously set with SetSortTranslucentPrimitives.
  inline bool GetSortTranslucentPrimitives() { return m_bSortTranslucentPrimitives; }

  /// \brief
  ///   Sets the criterion for sorting opaque and alpha-tested static geometry.
  /// 
  /// Use this if you want to change the sorting order in which opaque static geometry is rendered. The most
  /// efficient sorting option strongly depends on the scene: Scenes that are heavily pixel-bound often
  /// run faster with PROXIMITY set as the sorting criterion (i.e., front to back sorting), since this
  /// allows early z-cull hardware to cull pixels early in the pipeline.
  /// On the other hand, sorting by proximity means that there will be a lot of state changes which typically
  /// have a CPU impact. Scenes with a heavy CPU impact should therefore prefer RENDERSTATE as sorting criterion.
  /// In any case, it is usually a good idea to profile a scene to determine which sorting criterion is most
  /// useful for your scene.
  ///
  /// The following sorting flags are supported:
  /// VIS_API_SORTING_RENDERSTATE: Sort by render state (material, vertex/index buffer, etc.)
  /// VIS_API_SORTING_PROXIMITY: Sort front-to-back
  /// VIS_API_SORTING_DISTANCE: Sort back-to-front
  /// 
  /// Sorting flags can be combined, with the exception of VIS_API_SORTING_PROXIMITY and VIS_API_SORTING_DISTANCE,
  /// which are mutually exclusive. All other combinations are valid. In this case, the flag with the highest
  /// priority is followed by VIS_API_SORTING_RENDERSTATE, and finally
  /// VIS_API_SORTING_PROXIMITY or VIS_API_SORTING_DISTANCE.
  ///
  /// The actual sorting occurs in the VisionVisibilityCollector_cl class, so if you implement a custom visibility
  /// collector, you need to make sure you call the appropriate sorting methods on the collections of visible scene
  /// elements.
  /// 
  /// \param iCriterionFlags
  ///   Sorting key. Can be a combination of the following flags:   VIS_API_SORTING_RENDERSTATE,
  ///   VIS_API_SORTING_PROXIMITY, VIS_API_SORTING_DISTANCE. 0 can be used if no
  ///   sorting is desired. The default is VIS_API_SORTING_RENDERSTATE.
  VISION_APIFUNC void SetStaticGeometrySortingCriterion (int iCriterionFlags);

  /// \brief
  ///   Returns the static geometry sorting criteria as set with SetStaticGeometrySortingCriterion.
  VISION_APIFUNC int GetStaticGeometrySortingCriterion();

  /// \brief
  ///   Sets the criterion for sorting entities.
  /// 
  /// Use this if you want to change the sorting order of entities when rendering. The most
  /// efficient sorting option strongly depends on the scene: Scenes that are heavily pixel-bound often
  /// run faster with PROXIMITY set as the sorting criterion (i.e., front to back sorting), since this
  /// allows early z-cull hardware to cull pixels early in the pipeline.
  /// On the other hand, sorting by proximity means that there will be a lot of state changes which typically
  /// have a CPU impact. Scenes with a heavy CPU impact should therefore prefer RENDERSTATE as sorting criterion.
  /// In any case, it is usually a good idea to profile a scene to determine which sorting criterion is most
  /// useful for your scene.
  ///
  /// The following sorting flags are supported:
  /// VIS_API_SORTING_RENDERSTATE: Sort by render state (material, vertex/index buffer, etc.)
  /// VIS_API_SORTING_PROXIMITY: Sort front-to-back
  /// VIS_API_SORTING_DISTANCE: Sort back-to-front
  /// VIS_API_SORTING_OBJECTKEY: Sort according to the object key (see VisBaseEntity_cl::SetPrimarySortingKey).
  /// 
  /// Sorting flags can be combined, with the exception of VIS_API_SORTING_PROXIMITY and VIS_API_SORTING_DISTANCE,
  /// which are mutually exclusive. All other combinations are valid. In this case, the flag with the highest
  /// priority is VIS_API_SORTING_OBJECTKEY, followed by VIS_API_SORTING_RENDERSTATE, and finally
  /// VIS_API_SORTING_PROXIMITY or VIS_API_SORTING_DISTANCE.
  ///
  /// The actual sorting occurs in the VisionVisibilityCollector_cl class, so if you implement a custom visibility
  /// collector, you need to make sure you call the appropriate sorting methods on the collections of visible scene
  /// elements.
  /// 
  /// \param iCriterionFlags
  ///   Sorting key. Can be a combination of the following flags:   VIS_API_SORTING_RENDERSTATE,
  ///   VIS_API_SORTING_PROXIMITY, VIS_API_SORTING_DISTANCE, VIS_API_SORTING_OBJECTKEY. 0 can be used if no
  ///   sorting is desired. The default is VIS_API_SORTING_RENDERSTATE|VIS_API_SORTING_OBJECTKEY.
  VISION_APIFUNC void SetEntitySortingCriterion (int iCriterionFlags);

  /// \brief
  ///   Returns the entity sorting criteria as set with SetEntitySortingCriterion.
  VISION_APIFUNC int GetEntitySortingCriterion();

  /// \brief
  ///   Enables/disables automatic visibility zone creation for empty worlds.
  /// 
  /// If this feature is enabled (which is also the default setting), calling
  /// Vision::InitWorld() will automatically create a large empty visibility zone. This
  /// effectively emulates the behavior of engine versions 6.3 and below.
  /// 
  /// This feature should be disabled if you want to create the visibility setup for your scene
  /// manually.
  /// 
  /// Note that this method has to be called before calling Vision::InitWorld(), or using
  /// a Vision scene loader.
  /// 
  /// \param bStatus
  ///   true to enable, false to disable visibility zone creation for empty worlds.
  VISION_APIFUNC void SetCreateVisibilityForEmptyWorld(bool bStatus) { m_bCreateVisForEmptyWorld = bStatus; }

  /// \brief
  ///   Returns whether automatic visibility zone creation for empty worlds is enabled or disabled.
  VISION_APIFUNC bool GetCreateVisibilityForEmptyWorld() { return m_bCreateVisForEmptyWorld; }

  ///
  /// @}
  ///


  ///
  /// @name Wireframe Mode
  /// @{
  ///
  
  /// \brief
  ///   Enables/Disables wireframe mode
  /// 
  /// If wireframe mode is enabled, all scene elements will be rendered as wireframes instead of
  /// regular solid geometry. This feature is often useful for debugging and/or optimization
  /// purposes.
  /// 
  /// Note that features that required a fully set depth buffer (e.g. visibility query) might not
  /// work while in wireframe mode.
  /// 
  /// \param status
  ///   true to enable wireframe rendering
  VISION_APIFUNC void SetWireframeMode(bool status);

  /// \brief
  ///   Returns whether wireframe mode is currently enabled
  /// 
  /// \return
  ///   bool: true if wireframe mode is currently enabled
  VISION_APIFUNC bool GetWireframeMode();


  ///
  /// @}
  ///

  ///
  /// @name Geometry Cache Settings
  /// @{
  ///

  /// \brief
  ///   Sets a memory limit (in bytes) for the animation system resource manager (system memory
  ///   copy) and a time interval for automatically unloading unused resources.
  /// 
  /// Each time the limit is exceeded, the resource manager will try to unload data (e.g. cached
  /// skinning results).
  /// 
  /// These settings will only affect the system memory copy of the cached skinning results. The
  /// system memory copy is kept for
  /// \li a.) better performancy when copying data to GPU-accessible
  /// memory, and
  /// \li b.) collision meshes.
  /// 
  /// \param iSize
  ///   Memory limit this memory manager should use.
  /// 
  /// \param fUnloadInterval
  ///   The time interval to check. If for instance 10.0f, the resource manager will check every
  ///   ten seconds whether resource should be unloaded (e.g. because they are unused or exceed the
  ///   reserved memory limit).
  /// 
  /// \param fUnloadTimeout
  ///   Timeout value for unused resources.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetAnimationSystemMemory(unsigned int iSize, float fUnloadInterval, float fUnloadTimeout);



  ///
  /// @}
  ///

  ///
  /// @name Lightmapping Settings
  /// @{
  ///

  /// \brief
  ///   Sets the default color to use for lighting if a .LIT file or light grid isn't available
  /// 
  /// The default value for the lighting color is white.
  /// 
  /// \param color
  ///   New default lighting color
  /// 
  /// \note
  ///   This function has to be called before loading the world/scene, but after Vision::Init. 
  VISION_APIFUNC void SetDefaultLightingColor( VColorRef color );
  
  /// \brief
  ///   Returns the default color used for lighting if a .LIT file or light grid isn't available
  /// 
  /// The default value for the lightmap color is white.
  /// 
  /// \return
  ///   VColorRef color: Lightmap color
  VISION_APIFUNC VColorRef GetDefaultLightingColor( );
  
  /// \brief
  ///   Sets the lighting mode used for lightmaps
  /// 
  /// By default, lightmaps are modulated with the base texture. This function provides some
  /// additional modes to combine base texture and lightmap.
  /// 
  /// A disadvantage of using standard color modulation for lightmaps is that the result is always
  /// darker than the unlit texture, regardless of the light's intensity.
  /// 
  /// The modulate2x/4x modes allow to get overbright effects by additionally multiplying the
  /// result by 2 or 4.
  /// 
  /// For instance, in modulate2x mode, a grey lightmap value of (128,128,128) represents the
  /// fullbright level of the base texture.
  /// 
  /// The vLux tool allows selecting the mode and saves it to the .lit file. In case the run-time
  /// mode set using this method is left to  VIS_LIGHTING_DEFAULT, the engine will use the setting
  /// from the lit file.
  /// 
  /// Using a mode other than VIS_LIGHTING_DEFAULT will force the engine to use the specified mode
  /// and override the setting in the .lit file
  /// 
  /// \param lightingMode
  ///   The new lighting mode. The following modes are supported: VIS_LIGHTING_DEFAULT : Default,
  ///   use the settings from the .lit file VIS_LIGHTING_MODULATE : Use standard color modulation
  ///   for lightmaps (lightmap*base texture)  VIS_LIGHTING_MODULATE2X : Use color modulation x2
  ///   for overbright effects (2*lightmap*base texture)  VIS_LIGHTING_MODULATE4X : use color
  ///   modulation x4 for overbright effects (4*lightmap*base texture)
  /// 
  /// \returns
  ///   BOOL bResult: Usually TRUE. FALSE if this mode is not supported on the installed graphics
  ///   hardware.
  /// 
  /// \note
  ///   This mode will NOT affect any other type of illumination like dynamic lights or light-grid
  ///   lighting!
  VISION_APIFUNC BOOL SetLightingMode(VIS_CFG_LightingMode lightingMode);

  /// \brief
  ///   Returns the current lighting mode used for lightmaps.
  /// 
  /// \returns
  ///   VIS_CFG_LightingMode eMode: The current lighting mode. See SetLightingMode.
  VISION_APIFUNC VIS_CFG_LightingMode GetLightingMode() const;


  ///
  /// @}
  ///

  ///
  /// @name Hardware Occlusion Query
  /// @{
  ///

  /// \brief
  ///   Globally enables/disables the hardware occlusion test for entities and portals
  /// 
  /// In most cases, it is strongly recommended to enable hardware occlusion query, since it
  /// represents a rather fast and accurate method of determining the visibility of portals and
  /// entities. It should only be disabled in scenarios where you expect rather low culling rates,
  /// and therefore the performance impact of testing the entities/portals would outweigh the
  /// benefit of not rendering a few of them.
  /// 
  /// Note that this method interacts with render flags of the VisRenderContext_cl class and the
  /// render loop. Hardware Occlusion Query will only be performed if the
  /// VIS_RENDERCONTEXT_FLAG_USE_OCCLUSIONQUERY is set in the context and the render loop executes
  /// the VisRenderLoopHelper_cl::PerformHardwareOcclusionQuery method.
  /// 
  /// Additionally, the corresponding flags have to be set using the
  /// VisRenderer_cl::SetUseHardwareOcclusionFlags method.
  /// 
  /// \param status
  ///   If VIS_ENABLED, a hardware occlusion test will be performed for entities and portals. Note
  ///   that the engine may decide not to test entities with very low polygon counts.
  /// 
  /// \sa VisRenderer_cl::GetUseHardwareOcclusion
  VISION_APIFUNC void SetUseHardwareOcclusion(VIS_CFG_OnOff status);

  /// \brief
  ///   Returns the current status of the hardware occlusion test for models
  /// 
  /// \return
  ///   VIS_CFG_OnOff status: VIS_ENABLED if the hardware occlusion test is enabled for entities
  ///   and portals, otherwise VIS_DISABLED.
  /// 
  /// \sa VisRenderer_cl::SetUseHardwareOcclusion
  /// \sa VisRenderer_cl::SetUseHardwareOcclusionFlags
  VISION_APIFUNC VIS_CFG_OnOff GetUseHardwareOcclusion() const; 

  /// \brief
  ///   Copies the current render target contents into a texture
  /// 
  /// Note that this method always reads from render target 0 if multiple render targets are set.
  /// 
  /// No scaling is performed during the CopyToTexture operation, i.e. one pixel in the framebuffer
  /// will translate to one texel on the target texture. The size of the copied area of the frame
  /// buffer is defined by the target texture size. The target texture size may exceed the size of
  /// the frame buffer.
  ///
  /// Note that this is a legacy version of the newer, more flexible VisRenderableTexture_cl::Resolve()
  /// and VisRenderContext_cl::ResolveToTexture() functions. We recommend using these newer methods instead
  /// of CopyToTexture().
  /// 
  /// \param pTexture
  ///   Destination texture to be copied to.
  /// 
  /// \param xoffset
  ///   x offset in the frame buffer.
  /// 
  /// \param yoffset
  ///   y offset in the frame buffer.
  /// 
  /// \param flags
  ///   Ignored, should be 0
  /// 
  /// \return
  ///   BOOL: if TRUE, the call was successful.
  /// 
  /// \sa VisRenderer_cl::RenderToTexture
  VISION_APIFUNC BOOL CopyToTexture(VTextureObject *pTexture, unsigned int xoffset, unsigned int yoffset, int flags=0);

  /// \brief
  ///   Copies the current render target contents into a texture
  /// 
  /// Note that this method always reads from render target 0 if multiple render targets are set.
  /// 
  /// No scaling is performed during the CopyToTexture operation, i.e. one pixel in the framebuffer
  /// will translate to one texel on the target texture. The size of the copied area of the frame
  /// buffer is defined by the target texture size. The target texture size may exceed the size of
  /// the frame buffer.
  ///
  /// Note that this is a legacy version of the newer, more flexible VisRenderableTexture_cl::Resolve()
  /// and VisRenderContext_cl::ResolveToTexture() functions. We recommend using these newer methods instead
  /// of CopyToTexture().
  /// 
  /// \param pTexture
  ///   destination texture to be copied to
  /// 
  /// \param xoffset
  ///   x offset in the frame buffer.
  /// 
  /// \param yoffset
  ///   y offset in the frame buffer.
  /// 
  /// \param width
  ///   Width of the rectangle to be copied. Make sure that the texture is large enough for
  ///   this rectangle.
  /// 
  /// \param height
  ///   Height of the rectangle to be copied. Make sure that the texture is large enough for
  ///   this rectangle.
  /// 
  /// \param flags
  ///   ignored, should be 0
  /// 
  /// \return
  ///   BOOL: if TRUE, the call was successful.
  VISION_APIFUNC BOOL CopyToTexture(VTextureObject *pTexture, unsigned int xoffset, unsigned int yoffset, unsigned int width, unsigned int height, int flags=0);


  ///
  /// @}
  ///

  ///
  /// @name Misc Settings
  /// @{
  ///

  /// \brief
  ///   Specifies how to interpret texture coordinates (currently for screenmasks only).
  /// 
  /// If enabled, the coordinates (0,0) specify the CENTER of the a upper left texel of a texture,
  /// if disabled, the coordinates (0,0) specify the UPPER LEFT corner of that texel. In other
  /// words, this value will - if set - shift the texture coordinates by half a texel. This might
  /// be an issue if you have wrapping and/or filtering enabled for a screenmask. The half texel
  /// shift originally has been the OpenGL standard of adressing the center of a texel, hence the
  /// name.
  /// 
  /// \param status
  ///   TRUE to enable texture shift, FALSE to disable it.
  /// 
  /// \note
  ///   This is a global flag valid for all screenmasks
  /// 
  /// \note
  ///   Currently only screenmasks are affected by this flag
  /// 
  /// \sa VisRenderer_cl::GetUseOpenGLTexelShift()
  /// \sa VisScreenmask_cl::SetWrapping()
  /// \sa VisScreenmask_cl::SetFiltering()
  VISION_APIFUNC void SetUseOpenGLTexelShift(BOOL status);

  /// \brief
  ///   Returns the current status of texel shifting
  /// 
  /// \return
  ///   BOOL  status: The current status 
  /// 
  /// \sa VisRenderer_cl::SetUseOpenGLTexelShift()
  VISION_APIFUNC BOOL GetUseOpenGLTexelShift() const;

  /// \brief
  ///   Sets a global scaling value for the light grid
  /// 
  /// All light grid brightness values are scaled by this value before being returned.
  /// 
  /// The default value is 1.0.
  /// 
  /// \param fMul
  ///   The scaling value
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetLightGridMultiplier(float fMul);

  /// \brief
  ///   Returns the light grid scaling value previously set with SetLightGridMultiplier
  /// 
  /// \return
  ///   float fMul: The scaling value
  VISION_APIFUNC float GetLightGridMultiplier() const;

  /// \brief
  ///   Sets the default texture filtering mode
  /// 
  /// This filter mode will be applied to all samplers which have their filter mode(s) set to
  /// FILTER_DEFAULT. Additionally, it will be applied to all primitives rendered without a shader
  /// technique.
  /// 
  /// For a comprehensive list of supported filter modes, check the VFilterMode enum.
  /// 
  /// For instance, anisotropic filtering can be enabled by passing FILTER_ANISOTROPIC to this
  /// method.
  /// 
  /// Note that this setting does not have any effect on sampler stages with an explicit filter
  /// setting (other than FILTER_DEFAULT).
  ///
  /// In DirectX11, it is required to call this function before a scene is loaded, since it will only
  /// have an effect on sampler state groups created after the call to SetDefaultTextureFilterMode().
  /// 
  /// \param eFilterMode
  ///   The desired default filter mode
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetDefaultTextureFilterMode(VFilterMode eFilterMode);

  /// \brief
  ///   Returns the default texture filter mode.
  /// 
  /// The default texture filter mode can be set with the VisRenderer_cl::SetDefaultTextureFilterMode method.
  /// 
  /// \return
  ///   VFilterMode eFilterMode: The desired default filter mode
  VISION_APIFUNC VFilterMode GetDefaultTextureFilterMode() const;

  /// \brief
  ///   Forces DX11 render target textures to be strongly typed
  /// 
  /// When enabled, all offscreen render target textures will be created with a
  /// specific type (e.g. DXGI_FORMAT_R32_FLOAT instead of DXGI_FORMAT_R32_TYPELESS).
  /// This is useful when debugging applications with external tools such as PIX, 
  /// which require typed surfaces to enable surface and shader debugging 
  /// functionality. It should never be used in a shipping application.
  /// 
  /// Set this value prior to initializing the engine to debug primary render targets
  /// and post processing stages. Changing the value does not alter already created
  /// surfaces.
  /// 
  /// The default value is false.
  /// 
  /// \note
  /// This only applies to DX11 devices
  /// 
  /// \warning
  /// Enabling this debugging flag may cause instabilities and other improper 
  /// functioning within the engine. It should only be used during debugging of an
  /// application in tools such as PIX.
  /// 
  /// \param bStatus
  ///   Pass true to use typed formats, false to use typeless.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void SetUseTypedRenderTargets(BOOL bStatus);

  /// \brief
  ///   Returns the status value previously set with SetUseTypedRenderTargets
  /// 
  /// \return
  ///   BOOL bStatus: current value
  VISION_APIFUNC BOOL GetUseTypedRenderTargets() const;

  /// \brief
  ///   Returns the maximum pixel shader version supported on the graphics device
  /// 
  /// \returns 
  ///   VShaderModel_e VShaderModel_e: The highest supported pixel shader version.
  VISION_APIFUNC VShaderModel_e GetPixelShaderVersion();

  /// \brief
  ///   Returns the maximum vertex shader version supported on the graphics device
  /// 
  /// \returns 
  ///   VShaderModel_e VShaderModel_e: The highest supported vertex shader version.
  VISION_APIFUNC VShaderModel_e GetVertexShaderVersion();

  /// \brief
  ///   Returns the maximum geometry shader version supported on the graphics device
  /// 
  /// \returns 
  ///   VShaderModel_e VShaderModel_e: The highest supported geometry shader version.
  VISION_APIFUNC VShaderModel_e GetGeometryShaderVersion();

  /// \brief
  ///   Indicates whether this graphics device supports the instancing API
  /// 
  /// It is always supported for Xbox360, PS3 and all PC systems with support for shader model 3.0
  /// (or up). It is  also supported on some (but not all) shader model 2.0 systems.
  /// 
  /// Instanced rendering is exposed through the low level mesh rendering routines. See
  /// VisRenderLoopHelper_cl::AddMeshStreamsEx function
  /// 
  /// The engine has to be initialized before calling this function.
  /// 
  /// \return
  ///   BOOL bResult : TRUE if instancing is supported on this system.
  VISION_APIFUNC BOOL SupportsInstancing();

  /// \brief
  ///   Indicates whether this graphics device supports texture fetches in vertex shaders.
  /// 
  /// Vertex Texture Fetch is supported on Xbox360, PS3, DX10, and DX11; for DX9, it depends on the
  /// graphics card (typically supported on Geforce 6 and up as well as Radeon HD2400 and up).
  /// 
  /// The engine has to be initialized before calling this function.
  /// 
  /// \return
  ///   BOOL bResult : TRUE if vertex texture fetch is supported on this system.
  VISION_APIFUNC BOOL SupportsVertexTextureFetch();

  /// \brief
  ///   This function invalidates all internal cached render states and rebinds the context.
  /// 
  /// Relevant vertex and pixel shader constants are also re-uploaded.
  /// 
  /// Some bindings to 3rd party middleware might need this function to clean up states after
  /// custom rendering
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void ResetAllStates();



#ifdef _VISION_XENON

  /// \brief
  ///   Describes the primary and secondary ring buffers used by Direct3D 
  /// 
  /// Describes the primary and secondary ring buffers used by Direct3D
  /// 
  /// This is a convenience method wrapping around D3D's SetRingBufferParameters method, only
  /// exposing frequently used parameters. Calling this method using the default parameters will
  /// reset the ring buffer to its default properties.
  /// 
  /// For further details, please refer to the Xbox360 documentation.
  /// 
  /// \param iPrimarySize
  ///   Size of the primary ring buffer. 0 re-sets it to the default size (32kB).
  /// 
  /// \param iSecondarySize
  ///   Size of the secondary ring buffer. 0 re-sets it to the default size (2MB).
  /// 
  /// \param iSegmentCount
  ///   Number of ring buffer segments. 0 re-sets it to the default segment count (32).
  VISION_APIFUNC void SetRingBufferParameters(unsigned int iPrimarySize = 0, unsigned int iSecondarySize = 0, unsigned int iSegmentCount = 0);

  /// \brief
  ///   Returns the properties of the primary and secondary ring buffers used by Direct3D 
  /// 
  /// \param iPrimarySize
  ///   Size of the primary ring buffer.
  /// 
  /// \param iSecondarySize
  ///   Size of the secondary ring buffer.
  /// 
  /// \param iSegmentCount
  ///   Number of ring buffer segments.
  VISION_APIFUNC void GetRingBufferParameters(unsigned int &iPrimarySize, unsigned int &iSecondarySize, unsigned int &iSegmentCount) const;

  /// \brief
  ///   Enables/disables high-precision blending
  /// 
  /// for 2_10_10_10 and 2_10_10_10_FLOAT formats. Each bit of the bitmask represents one render
  /// target (bit 0 stands for RT0, bit 1 for RT1, and so on). A set bit means that high-precision
  /// blending will be enabled for the respective context, an unset bit means it will be disabled.
  /// 
  /// May not be called before initialization of the Direct3D device using
  /// Vision::Video.InitializeScreen.
  /// 
  /// \param iMask
  ///   Bitmask for high-precision blending
  VISION_APIFUNC void SetHighPrecisionBlend(int iMask);
  
  /// \brief
  ///   Returns the high-precision blending mask
  /// 
  /// previously set with SetHighPrecisionBlend
  /// 
  /// \return
  ///   int iMask: Bitmask for high-precision blending
  VISION_APIFUNC int GetHighPrecisionBlend() const;

  /// \brief
  ///   Returns the immediate presentation threshold on Xbox360.
  /// 
  /// Returns the immediate presentation threshold previously set with
  /// SetPresentImmediateThreshold.
  /// 
  /// The default threshold value is 0.
  /// 
  /// \return
  ///   int: immediate presentation threshold
  VISION_APIFUNC int GetPresentImmediateThreshold() const;

  /// \brief
  ///   Sets the immediate presentation threshold on Xbox360.
  /// 
  /// This function wraps around setting the render state D3DRS_PRESENTIMMEDIATETHRESHOLD.
  /// 
  /// It is only available on the Xbox360 platform.
  /// 
  /// \param iPercentageThreshold
  ///   immediate presentation threshold
  VISION_APIFUNC void SetPresentImmediateThreshold(int iPercentageThreshold);

#endif




#if defined(WIN32) || defined(_VISION_PS3) || defined(_VISION_XENON) || defined(_VISION_PSP2) || defined(_VISION_WIIU)

  /// \brief
  ///   Sets the particle ring buffer size on PS3 / PC / PSP2 / Xbox360
  /// 
  /// Particles are rendered using a ring buffer on PS3 / PC / PSP2 / Xbox360. The larger the ring buffer, the
  /// lower the risk of running into (expensive) intra-frame synchronisations of GPU and CPU. The
  /// ideal size depends on the number of particles simultaneously visible on screen.
  /// 
  /// As a rule of thumb, the ring buffer should be able to hold about Particles(max) * 48 Bytes of
  /// storage, where Particles(max) is the maximum number of particles simultaneously visible on
  /// screen.
  /// 
  /// If this function is not called, the Vision engine will use a predefined platform-dependend
  /// default value (typically between 512kB and 4 MB).
  /// 
  /// \param iSizeInBytes
  ///   Size of the ring buffer in bytes.
  VISION_APIFUNC void SetParticleRingBufferSize(int iSizeInBytes);

  /// \brief
  ///   Returns the particle ring buffer size
  VISION_APIFUNC int GetParticleRingBufferSize() const;


#endif

  /// \brief
  ///   Sets the ring buffer size for immediate mode rendering (DX10/11 only).
  /// 
  /// On PC DX10/11, "immediate mode" rendering (e.g. fonts, screen masks,...) is handled using a ring
  /// buffer for vertex data. The larger this ring buffer is, the smaller the chance for expensive
  /// CPU/GPU synchronisation to happen, but the higher the memory impact.
  /// 
  /// \param iSizeInBytes
  ///   Size of the immediate mode ring buffer in bytes
  VISION_APIFUNC void SetImmediateModeRingBufferSize(int iSizeInBytes);

  /// \brief
  ///   Returns the ring buffer size for immediate mode rendering (DX10/11 only).
  VISION_APIFUNC int GetImmediateModeRingBufferSize() const;

  /// \brief
  ///   Enables or disables strict vertex layout consistency checks (DX10/11 only).
  /// 
  /// Enable this setting to track down Input Assembler - Vertex Shader mismatches.  Whenever the
  /// declaration order of input elements in a vertex shader does not match the required POSITION
  /// -> NORMAL -> COLOR -> TEXCOORD order required by the Vision engine, a debug assertion will
  /// occur. 
  /// 
  /// This setting only works in debug mode. The default is false.
  /// 
  /// Note that enabling this option may cause a significant performance drop in debug builds,
  /// since a validation is performed before every draw call.
  /// 
  /// \param bStatus
  ///   enable/disable the setting
  inline void SetStrictVertexLayoutConsistencyCheck(bool bStatus) { m_bStrictLayoutConsistencyCheck = bStatus; }

  /// \brief
  ///   Returns the value previously set with SetStrictVertexLayoutConsistencyCheck (or the default
  ///   value, which is false). DX10/11 only.
  inline bool GetStrictVertexLayoutConsistencyCheck() const { return m_bStrictLayoutConsistencyCheck; }


  /// \brief
  ///   Optionally forces static meshes (VisStaticMesh_cl) to use single-buffered vertex and index
  ///   buffers.
  /// 
  /// Single-buffered vertex and index buffers require less memory, but they result in a higher
  /// performance impact when read back from GPU memory, e.g. when VisStaticMesh_cl::GetTraceMesh
  /// is called.
  /// 
  /// The default setting on PS3 and Xbox360 is false, on PC, it is true.
  /// 
  /// Call this method before the static meshes are created (e.g. before loading a scene).
  /// 
  /// \param bStatus
  ///   true to force single-buffered static meshes.
  VISION_APIFUNC void SetUseSingleBufferedStaticMeshes(bool bStatus);

  /// \brief
  ///   Returns whether static meshes are force to single-buffered.
  VISION_APIFUNC bool GetUseSingleBufferedStaticMeshes();


#if defined(WIN32) && defined(_VR_DX9)

  /// \brief
  ///   PC DX9 only: Specifies whether vertex and index buffers should preferably reside in the
  ///   managed or the dynamic pool
  /// 
  /// PC DX9 only: Specifies whether vertex and index buffers should preferably reside in the
  /// managed or  the default memory pool. Due to internal limitations of DirectX9, this only
  /// affects buffers with the VIS_MEMUSAGE_STATIC usage flag (VIS_MEMUSAGE_STREAM and
  /// VIS_MEMUSAGE_DYNAMIC buffers always use the default pool).
  /// 
  /// This setting will affect all subsequent buffer creations.
  /// 
  /// \param bStatusSingleBuffered
  ///   true to prefer default pool for single-buffered buffers, false to prefer managed pool
  ///   (default).
  /// 
  /// \param bStatusDoubleBuffered
  ///   true to prefer default pool for double-buffered buffers, false to prefer managed pool
  ///   (default).
  inline void SetUseUnmanagedBufferPool(bool bStatusSingleBuffered, bool bStatusDoubleBuffered)
  {
    m_bSingleBufferedBuffersUnmanaged = bStatusSingleBuffered;
    m_bDoubleBufferedBuffersUnmanaged = bStatusDoubleBuffered;
  }

  /// \brief
  ///   Returns the values previously set with SetUseUnmanagedBufferPool.
  inline void GetUseUnmanagedBufferPool(bool &bStatusSingleBuffered, bool &bStatusDoubleBuffered) const
  {
    bStatusSingleBuffered = m_bSingleBufferedBuffersUnmanaged;
    bStatusDoubleBuffered = m_bDoubleBufferedBuffersUnmanaged;
  }

#endif



  /// \brief
  ///   Sets the global ambient color.
  /// 
  /// On platforms supporting shader model 3.0, the global ambient color is available by default in
  /// one of the reserved constants of the pixel shader. On other platforms, it has to be tracked
  /// manually.
  /// 
  /// \note
  ///   Shaders within the mobile shader libraries do only use the global ambient color when TimeOfDay
  ///   is active. Please check the Mobile Forward Rendering documentation for more information.
  /// 
  /// \param vGlobalAmbient
  ///   Global ambient color.
  VISION_APIFUNC void SetGlobalAmbientColor(const hkvVec4& vGlobalAmbient);

  /// \brief
  ///   Returns the global ambient color previously set with SetGlobalAmbientColor.
  VISION_APIFUNC inline const hkvVec4& GetGlobalAmbientColor() const { return (hkvVec4&) m_vGlobalAmbient; }

  /// \brief
  ///   Enables/disables global constant tracking.
  /// 
  /// This method can be used to disable global constant tracking (i.e. tracking of global shader
  /// constant registers 0 to 31 for pixel shaders and 0 to 63 for vertex shaders). If global
  /// constant tracking is disabled, the Vision engine will not update these registers
  /// automatically, so you will have to do all shader constant updates manually.
  /// 
  /// For DX10/11, disabling global constant tracking results in constant buffers 0 and 1 (for VS, PS,
  /// and GS) not being set any more. However, the internally used buffers 2 to 4 may still be set
  /// and altered by the Vision Engine.
  /// 
  /// \param bStatus
  ///   true to enable global constant tracking (default), false to disable it.
  VISION_APIFUNC inline void SetGlobalConstantTracking(bool bStatus) {
    m_bGlobalConstantTracking = bStatus;
  }

  /// \brief
  ///   Returns whether global constant tracking is enabled or disabled.
  VISION_APIFUNC inline bool GetGlobalConstantTracking() const { 
    return m_bGlobalConstantTracking; 
  }

  /// \brief
  ///   This method has to be called before any rendering is executed. On PC DX9, it internally calls BeginScene().
  VISION_APIFUNC void BeginRendering();

  /// \brief
  ///   This method has to be called after rendering, but before Present()/UpdateScreen() is called. On PC DX9, it internally calls EndScene().
  VISION_APIFUNC void EndRendering();

  /// \brief
  ///   DX10/11 debug builds only: Enable or disable displaying of benign runtime warnings.
  /// 
  /// The Vision Engine generates a few DX10/11 runtime warnings which are benign, i.e. they do not affect the behaviour.
  /// By default, Vision displays these runtime warnings. However, displaying of these warnings can be switched off using
  /// this function in order to remove "garbage" from the output window.
  /// 
  /// \param bStatus
  ///   true to enable displaying of benign runtime warnings, false to disable it.
  VISION_APIFUNC void SetShowBenignRuntimeWarnings(bool bStatus);

  /// \brief
  ///   Returns whether benign runtime warnings are shown.
  inline bool GetShowBenignRuntimeWarnings() const { return m_bShowBenignRuntimeWarnings; }


  
  #ifdef _VISION_PS3

  /// \brief
  ///   Sets the size of the fragment shader ring buffer on Playstation 3
  /// 
  /// On Playstation 3, fragment shaders constant are patched directly into the fragment shader's
  /// byte code in order to speed up rendering and reduce RSX workload. A ring buffer is used to
  /// ensure that RSX/PPU stalls are minimized. The larger the ring buffer, the lower the risk of
  /// stalls.
  /// 
  /// The Vision Profiling pages show the amount of time spent in RSX/PPU stalls for fragment
  /// shader ring buffer synchronisation. If this value is significantly above zero in your
  /// application, it is recommended to increase the fragment shader ring buffer size.
  /// 
  /// This method is only available on Playstation 3.
  /// 
  /// The default size of the buffer is 512K; the default number of ring buffer segments is 4.
  /// 
  /// \param iSizeInBytes
  ///   Size of the ring buffer in bytes.
  /// 
  /// \param iSegmentCount
  ///   Number of ring buffer segments.
  VISION_APIFUNC void SetFragmentShaderRingBufferSize(unsigned int iSizeInBytes, unsigned int iSegmentCount);

  /// \brief
  ///   Returns the fragment shader ring buffer size previously set with
  ///   GetFragmentShaderRingBufferSize.
  VISION_APIFUNC unsigned int GetFragmentShaderRingBufferSize(unsigned int *pSegmentCount = NULL) const;

  /// \brief
  ///   Specifies whether fragment shaders will be put in local or main memory.
  /// 
  /// Typically, it is recommended to use fragment shaders located in main memory, since this will
  /// ensure the highest fragment shader patching performance. However, if main memory is running
  /// low, it can be a viable option to put fragment shaders in local (RSX) memory instead.
  /// 
  /// The default value is true (fragment shaders reside in main memory).
  /// 
  /// \param bStatus
  ///   true to put fragment shaders in main memory, false to put them in local memory.
  VISION_APIFUNC void SetUseMainMemoryForFragmentShaders(bool bStatus);

  /// \brief
  ///   Returns the value previously set with SetUseMainMemoryForFragmentShaders.
  inline bool GetUseMainMemoryForFragmentShaders() const { return m_bUseMainMemoryForFragmentShaders; }

  #endif
  

  #ifdef _VISION_PSP2

  /// \fn VISION_APIFUNC void SetDefaultIndexBufferSize(unsigned int uiSize)
  ///
  /// \brief  Sets a default index buffer size. You can use this function to change the size of the default index buffer on the PSP2 which is used when drawing unindexed geometry. By default it will be set to USHRT_MAX indices.
  ///
  /// \param  uiSize  Size of the default index buffer. 
  ///
  VISION_APIFUNC void SetDefaultIndexBufferSize(unsigned int uiSize);

  /// \fn inline unsigned int GetDefaultIndexBufferSize() const
  ///
  /// \brief  Gets the default index buffer size. 
  ///
  /// \return The current default index buffer size. 
  ///
  inline unsigned int GetDefaultIndexBufferSize() const { return m_uiDefaultIndexBufferSize; }

  /// \fn inline VisRenderIndexBufferPtr GetDefaultIndexBuffer() const
  ///
  /// \brief  Gets the default index buffer. 
  ///
  /// \return The default index buffer. 
  ///
  inline VIndexBufferPtr GetDefaultIndexBuffer() const { return m_spDefaultIndexBuffer; }

  #endif


  #ifdef _VISION_WIIU

  /// \brief
  ///   Returns the renderable textures usable for rendering to the remote display controller on the WiiU
  ///
  /// \param eDrc
  ///   The drc to get the renderable textures for
  /// \param pColorRenderTarget
  ///   A pointer to a VisRenderableTexture_cl to store the pointer to the color render target into (optional)
  /// \param pDepthRenderTarget
  ///   A pointer to a VisRenderableTexture_cl to store the pointer to the depth/stencil render target into (optional)
  ///
  VISION_APIFUNC bool GetDRCRenderableTextures(VWiiUDRC eDrc, VisRenderableTexture_cl** pColorRenderTarget, VisRenderableTexture_cl** pDepthRenderTarget);

  #endif

  /// \brief
  ///   Enables/disables automatic lost device handling.
  ///
  /// By default, the Vision Engine handles lost device events (which can only occur in DirectX9)
  /// automatically. This involves regularly polling for a change of the device status once the 
  /// device is lost, basically resulting in a loop that can run for milliseconds up to multiple seconds
  /// before it returns. In the case of network-enabled games, this might not always be acceptable, since
  /// a minimum tick frequency must be ensured. In such cases, disable automatic lost device handling.
  /// If lost device handling is disabled, VisRenderer_cl::CheckLostDevice provides a valuable
  /// method for easily handling lost devices.
  ///
  /// \param bStatus
  ///   true to enable automatic lost device handling (default), false to disable it.
  inline void SetAutoLostDeviceHandling(bool bStatus) { m_bAutoLostDeviceHandling = bStatus; }

  /// \brief
  ///   Returns whether automatic lost device handling is currently enabled or disabled.
  inline bool GetAutoLostDeviceHandling() const { return m_bAutoLostDeviceHandling; }

  /// \brief
  ///   Returns whether the device is lost.
  VISION_APIFUNC bool IsDeviceLost() const;

  /// \brief
  ///   Handles lost device transitions.
  ///
  /// If automatic handling of lost devices has been disabled using the VisRenderer_cl::SetAutoLostDeviceHandling
  /// method, this function provides a valuable tool for implementing lost device handling in your game
  /// loop. Its behavior is as follows:
  /// \li If the device is not lost, it will not do anything and simply return true
  /// \li If the device is in lost state (D3DERR_DEVICELOST), it will test the cooperative level of the
  ///   device each time the function is called. It will return false while the device is lost.
  /// \li When the device reaches the "not reset" state (D3DERR_DEVICENOTRESET), all resources will
  ///   be cleaned up, callbacks will be triggered, the device will be reset, and the resources will
  ///   be recreated. This is very similar to what happens in the case of automatic lost device handling.
  ///
  /// \returns
  ///   bool: true while rendering is allowed, false if rendering is not allowed. Do not execute any
  ///   rendering commands as long as this function returns false!
  VISION_APIFUNC bool CheckLostDevice();

  /// \brief
  ///   Returns whether this function is called from within a render loop. Used to assert correct usage of functions inside VisRenderLoopHelper_cl.
  inline bool IsInRenderLoop() { return m_bInRenderLoop; }

  // Not officially supported. Do not use!
  inline void SetForceSingleBufferedModels(bool bStatus)
  {
    m_bForceSingleBufferedModelData = bStatus;
  }

  // Not officially supported. Do not use!
  inline bool GetForceSingleBufferedModels() const
  {
    return m_bForceSingleBufferedModelData;
  }

  // internal methods
  VISION_APIFUNC void SetReverseCullModeFromContext(BOOL bStatus);

  inline void BeginRenderLoop() { m_bInRenderLoop = true; }
  inline void EndRenderLoop() { m_bInRenderLoop = false; }

  #if defined(WIN32) && defined(_VR_DX9)
    inline bool IsInRenderingBracket() const { return m_bIsInRenderingBracket; }
  #endif

  // old methods - still supported, but will be removed
  VISION_APIFUNC void SetHeightfieldForceFixStepTesselation(VIS_CFG_OnOff status);
  VISION_APIFUNC void SetVideoWait(VIS_CFG_OnOff retrace, VIS_CFG_OnOff glFinish);
  VISION_APIFUNC void SetDepthOffsetEpsilon (float epsilon);
  VISION_APIFUNC void SetCheckForGraphicsAPIErrors(VIS_CFG_OnOff status);
  VISION_APIFUNC const char *Get3DAPIErrorString();
  VISION_APIFUNC HKV_DEPRECATED_2012_1 void EvaluateBrightnessAtPoint (const hkvVec3& pos, unsigned char &r, unsigned char &g, unsigned char &b);
  VISION_APIFUNC HKV_DEPRECATED_2012_1 int EvaluateBrightnessAtPointEx (const hkvVec3& pos, float fRadius, hkvVec4 &destColor, int iFlags, hkvVec3* pNormal=NULL);
  VISION_APIFUNC void SetUseHardwareOcclusionFlags(int iFlags = HARDWAREOCCLUSION_ALL); 


  ///
  /// @}
  ///

private:
  friend class VisDisplay_cl;
  friend class VisMessage_cl;
  friend class Vision;
  friend class VisVideo_cl;
  friend void PrepareEntitiesForRendering(const VisEntityCollection_cl *, VisEntityCollection_cl *);

  // INTERNAL FUNCTIONS
  void SetMaxHeightfieldDisplayCacheSize(int size);
  void SetMaxObjectDisplayCacheSize(int size);

#ifdef _VISION_PS3
  bool m_bUseMainMemoryForFragmentShaders;
#endif


#ifdef _VISION_PSP2
  unsigned int m_uiDefaultIndexBufferSize;
  VIndexBufferPtr m_spDefaultIndexBuffer;
#endif

  bool  m_bCreateVisForEmptyWorld;
  bool  m_bGlobalConstantTracking;
  bool  m_bAutoLostDeviceHandling;
  bool  m_bInRenderLoop;

  bool  m_bForceSingleBufferedModelData;

  bool m_bStrictLayoutConsistencyCheck;
  bool m_bRenderingIsSuspended;
  bool m_bShowBenignRuntimeWarnings;

#if defined(_VR_DX9) && defined(WIN32)
  bool m_bSingleBufferedBuffersUnmanaged;
  bool m_bDoubleBufferedBuffersUnmanaged;
  bool m_bIsInRenderingBracket;
#endif

#if defined(_VR_DX11) && defined(WIN32)
  bool m_bUseTypedRenderTargets;
#endif

  hkvVec4 m_vGlobalAmbient;
  VColorRef m_iDefaultClearColor;

  IVRendererNodePtr m_spRendererNode[V_MAX_RENDERER_NODES];
  IVRendererNodePtr m_spCurrentRendererNode;
  IVTimeOfDayPtr m_spTimeOfDay;
  IVObjectInstanceFactory *m_pTimeOfDayFactory;


#ifdef _VISION_XENON
  int m_iHighPrecisionBlendMask;
  int m_iPresentImmediateThreshold;
#endif

#ifdef _VISION_WIIU
  VisRenderableTexture_cl* m_pDRCColorBuffers[V_DRC_COUNT];
  VisRenderableTexture_cl* m_pDRCDepthBuffers[V_DRC_COUNT];
  bool m_bDRCBuffersBound;
#endif


  int m_iEntitySortingCriterion;
  int m_iStaticGeometrySortingCriterion;
  bool m_bSortTranslucentPrimitives;
};

VISION_APIFUNC VRenderHook_e UpdateRenderHook(unsigned int uiOldValue);

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
