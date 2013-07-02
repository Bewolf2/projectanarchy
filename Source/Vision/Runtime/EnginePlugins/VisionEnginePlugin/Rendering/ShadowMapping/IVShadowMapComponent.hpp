/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef IVSHADOWMAPCOMPONENT_HPP_INCLUDED
#define IVSHADOWMAPCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenerator.hpp>

#define SHOW_SHADOW_MAP
#undef SHOW_SHADOW_MAP

#define SHOW_SHADOW_MASK
#undef SHOW_SHADOW_MASK

#define SHADOWMAP_COMPONENT_VERSION_0                                   0     // Initial version
#define SHADOWMAP_COMPONENT_VERSION_1                                   1     // Version 1
#define SHADOWMAP_COMPONENT_VERSION_2                                   2     // Version 2 : Geometry types
#define SHADOWMAP_COMPONENT_VERSION_3                                   3     // Version 3 : Front-facing shadows option
#define SHADOWMAP_COMPONENT_VERSION_4                                   4     // Version 4 : Fadeout parameter
#define SHADOWMAP_COMPONENT_VERSION_5                                   5     // Version 5 : Unsupported members removed
#define SHADOWMAP_COMPONENT_VERSION_6                                   6     // Version 6 : SampleRadiusScaleWithDistance
#define SHADOWMAP_COMPONENT_VERSION_7                                   7     // Version 7 : MaxDistanceToBlocker
#define SHADOWMAP_COMPONENT_CURRENT_VERSION SHADOWMAP_COMPONENT_VERSION_7     // Current version


/// \brief
///   Enum that holds supported shadow mapping filtering modes
enum VShadowMappingMode_e 
{
  SHADOW_MAPPING_MODE_PCF4 = 0,                   ///< Four-sample Percentage Closer filtering
  SHADOW_MAPPING_MODE_PCF8 = 1,                   ///< Eight-sample Percentage Closer filtering
  SHADOW_MAPPING_MODE_PCF8_RANDOMIZED = 2,        ///< Eight-sample Percentage Closer filtering with randomized sample positions
  SHADOW_MAPPING_MODE_PCF16 = 3,                  ///< Sixteen-sample Percentage Closer filtering
  SHADOW_MAPPING_MODE_PCF16_RANDOMIZED = 4,       ///< Sixteen-sample Percentage Closer filtering with randomized sample positions
  SHADOW_MAPPING_MODE_PCSS16 = 5,                 ///< Sixteen-sample Percentage Closer Soft Shadows (PCSS)
  SHADOW_MAPPING_MODE_PCSS16_RANDOMIZED = 6,      ///< Sixteen-sample Percentage Closer Soft Shadows (PCSS) with randomized sample positions
  SHADOW_MAPPING_MODE_CHS = 7,                    ///< Contact-hardening shadows
  SHADOW_MAPPING_MODE_LAST_VALID = SHADOW_MAPPING_MODE_CHS,
  SHADOW_MAPPING_MODE_DEBUG_SHOW_CASCADES = 8     ///< Debugging Mode: Show shadow cascades in different colors.
};

/// \brief
///   Base class for all shadow map components that can be attached to light sources.
///
/// By attaching a shadow map component to a dynamic light source, dynamic shadowing using shadow
/// maps is enabled for that light source. 
///
/// Depending on the type of light source (directional vs. spotlight vs. point light), you will
/// need a different specialization of this class (VShadowMapComponentPoint vs. VShadowMapComponentSpotDirectional/ 
/// VMobileShadowMapComponentSpotDirectional).
///
/// Shadow Maps are generated using a "deferred" approach for forward as well as for deferred rendering, except 
/// for the mobile forward renderer: 
/// First, the depth texture is rendered from the light's point of view. Then, this information
/// in conjunction with the scene depth in view space is used to compute shadowing values for all
/// relevant pixels, which are stored in a single texture (typically the size of the render target,
/// or a quarter of that size) which is called the "shadow mask". This texture is generated prior to applying the actual lighting
/// information. The lighting pass itself then only needs to take the shadowing information from
/// this texture into account (a simple lookup and multiplication to mask the light intensity).
/// 
/// For mobile forward rendering the shadow map will be directly used in the lighting pass.
///
/// Note that shadow map components are specific to a single renderer node: If you have multiple renderer nodes, you will have
/// to attach multiple shadow map components - one per renderer node - to your light sources.
class IVShadowMapComponent : public IVObjectComponent, public IVisCallbackHandler_cl
{
public:

  ///
  /// @name Initialization
  /// @{
  ///

  /// \brief
  ///   Constructor
  EFFECTS_IMPEXP IVShadowMapComponent();

  /// \brief
  ///   Constructor taking the associated renderer node index as input.
  EFFECTS_IMPEXP IVShadowMapComponent(int iRendererNode);

  /// \brief
  ///   Destructor
  EFFECTS_IMPEXP virtual ~IVShadowMapComponent();

  /// \brief
  ///   Same as SetEnabled(TRUE);
  EFFECTS_IMPEXP void Enable();

  /// \brief
  ///   Same as SetEnabled(FALSE);
  EFFECTS_IMPEXP void Disable();

  /// \brief
  ///   Enables or disables the shadows of this component. If disabled, all rendering operations related to this shadow component are skipped.
  ///   Disabled Shadow Components have a very small performance overhead.
  EFFECTS_IMPEXP void SetEnabled(BOOL enabled = TRUE);

  /// \brief
  ///   Returns whether the shadow component is currently enabled.
  inline bool IsEnabled() const
  { 
    return m_bIsInitialized; 
  }

  ///
  /// @}
  ///

  ///
  /// @name Shadow Rendering
  /// @{
  ///

  /// \brief
  ///   Key function of this interface that renders the shadows. 
  EFFECTS_IMPEXP virtual void Render(bool bUseLightClippingVolume, bool bClearTexture) {};

  /// \brief
  ///   Return the renderer node that this component is associated with.
  ///
  /// Note that each shadow map component is tied to a specific renderer node index. Thus, if you want to use multiple renderer nodes with
  /// shadow maps, you will have to assign multiple shadow map components to each relevant light source (i.e. one per renderer node).
  /// Since the shadow map component is tied to a renderer node index rather than a renderer node object, exchanging
  /// renderer nodes can change the renderer node returned by this function-
  ///
  /// \returns
  ///  The renderer node this shadow map component is associated with.
  inline IVRendererNode *GetRendererNode() const
  { 
    return m_pRendererNode; 
  }

  /// \brief
  ///   Return the renderer node index that this component is associated with.
  ///
  /// Note that each shadow map component is tied to a specific renderer node index. Thus, if you want to use multiple renderer nodes with
  /// shadow maps, you will have to assign multiple shadow map components to each relevant light source (i.e. one per renderer node).
  ///
  /// \returns
  ///  The renderer node index this shadow map component is associated with.
  inline int GetRendererNodeIndex() const
  { 
    return m_iRendererNodeIndex; 
  }

  /// \brief
  ///   Returns the screen-space shadow mask texture or the shadow map depending on the shadow map component implementation.
  EFFECTS_IMPEXP virtual VTextureObject *GetShadowTexture() const
  {
    return NULL;
  }

  /// \brief
  ///   Returns the light source that this component is attached to (basically owner cast to VisLightSource_cl pointer)
  inline VisLightSource_cl *GetLightSource() const
  { 
    return m_pLightSource; 
  }

  /// \brief
  ///   Returns the shadow map generator.
  ///
  /// The Shadow Map generator is the class responsible for generating the light-space depth texture used by the shadow map renderer
  /// to generate the full-screen shadow mask.
  ///
  /// \returns
  ///   The Shadow Map Generator.
  inline VShadowMapGenerator *GetShadowMapGenerator() const
  { 
    return m_spShadowMapGenerator; 
  }

  /// \brief
  ///   Internal function to return a mesh that represents the light's bounding volume (sphere for point lights, cone for spotlights)
  EFFECTS_IMPEXP virtual VisMeshBuffer_cl* GetLightVolumeMeshBuffer();

  /// \brief
  ///   Internal function to return the draw call range for a specified cascade when rendering the mesh retrieved via GetLightVolumeMeshBuffer
  EFFECTS_IMPEXP virtual void GetLightVolumeMeshBufferIndexAndCount(int iCascade, int& iIndex, int& iCount);

  ///
  /// @}
  ///

  ///
  /// @name Component Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden component function
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overridden component function (can attach to VisLightSource_cl instances)
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief
  ///   Overridden IVisCallbackHandler_cl handling function
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  /// \brief
  ///   Overridden component function
  EFFECTS_IMPEXP virtual bool SetVariable(const char *szName, const char *szValue);


#if defined(WIN32) || defined(_VISION_DOC)

  /// \brief
  ///   Overridden component function
  EFFECTS_IMPEXP virtual void GetVariableAttributes(VisVariable_cl* pVariable, VVariableAttributeInfo& destInfo) HKV_OVERRIDE;

#endif

  ///
  /// @}
  ///

  ///
  /// @name RTTI
  /// @{
  ///

  /// \brief
  ///   RTTI macro for the VARTABLE
  V_DECLARE_VARTABLE(IVShadowMapComponent, EFFECTS_IMPEXP);

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(IVShadowMapComponent, EFFECTS_IMPEXP);

  /// \brief
  ///   Serialization
  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar);


  ///
  /// @}
  ///

  ///
  /// @name Shadow Properties
  /// @{
  ///

  /// \brief
  ///   Indicates whether a quarter sized shadow mask texture is used
  inline BOOL GetUseQuarterSizeShadowTexture() const
  { 
    return UseQuarterSizeShadowTexture; 
  }

  /// \brief
  ///   Enables or disables using a quarter-size texture for the screen-space shadow mask.
  ///
  /// By default, a full-size shadow mask is used, i.e. shadowing information is computed at the resolution that is used for
  /// the renderer node's main render target. By calling SetUseQuarterSizeShadowTexture(TRUE) (or setting up a shadow map component
  /// in vForge which has this property enabled), the shadowing information will be computed at a quarter of the resolution of the
  /// main render target. This can result in a very significant speedup, especially with complex shadowing filters, but can produce
  /// undersampling artifacts and small unshadowed seams around objects. Using this feature is a performance versus visual quality
  /// tradeoff.
  /// 
  /// \param bStatus
  ///   TRUE to enab
  EFFECTS_IMPEXP void SetUseQuarterSizeShadowTexture(BOOL bStatus);
  
  /// \brief
  ///   Returns the (quality) mode
  inline VShadowMappingMode_e GetShadowMappingMode() const
  { 
    return ShadowMappingMode;
  }

  /// \brief
  ///   Sets the filtering mode for shadow mapping
  ///
  /// This function specifies the filtering mode used for shadow mapping. The filtering mode greatly effects both the visual quality
  /// and the speed of shadow mapping, and a good choice of shadow map size, filtering mode, and number of cascades is crucial for
  /// a good tradeoff between performance and looks.
  ///
  /// The following filtering types are supported (listed in order of performance impact and, roughly, visual quality):
  /// \li SHADOW_MAPPING_MODE_PCF4: Percentage-closer filtering, taking four samples into account. 
  /// \li SHADOW_MAPPING_MODE_PCF8: Percentage-closer filtering, taking eight samples into account
  /// \li SHADOW_MAPPING_MODE_PCF8_RANDOMIZED: Eight-sample PCF with randomized sample positions.
  /// \li SHADOW_MAPPING_MODE_PCF16: Percentage-closer filtering, taking sixteen samples into account
  /// \li SHADOW_MAPPING_MODE_PCF16_RANDOMIZED: Sixteen-sample PCF with randomized sample positions.
  /// \li SHADOW_MAPPING_MODE_PCSS16: Percentage-closer soft shadows with an eight-sample blocker search and sixteen-sample filtering
  /// \li SHADOW_MAPPING_MODE_PCSS16_RANDOMIZED: Percentage-closer soft shadows with an eight-sample blocker search and sixteen-sample filtering with randomized sample positions.
  /// \li SHADOW_MAPPING_MODE_CHS: Contact-hardening soft shadows with full blocker search.
  ///
  /// Additionally, SHADOW_MAPPING_MODE_DEBUG_SHOW_CASCADES can be specified to visualize the cascades with different colors (for
  /// directional lights and spotlights). For point lights, which use tetrahedron shadow mapping, this mode will visualize the tetrahedron
  /// faces.
  ///
  /// Note that the more "complex" shadowing methods may be too expensive to use on some platforms (e.g. Xbox360, Playstation 3). 
  ///
  /// \param mode
  ///   Shadow Mapping mode to use
  EFFECTS_IMPEXP void SetShadowMappingMode(VShadowMappingMode_e mode);

  /// \brief
  ///   Returns the resolution of the shadow map (per cascade).
  inline unsigned int GetShadowMapSize() const
  { 
    return ShadowMapSize; 
  }

  /// \brief
  ///   Returns the resolution of the shadow map (per cascade).
  ///
  /// The passed value is the shadow map size *per cascade*, so the actual texture size may differ from this value. If two
  /// cascades are used, the created texture will have a width of 2*size and a height of size; if three cascades are used, 
  /// the texture will have a width of 3*size; for four cascades, the texture will have a width of 2*size and a height of
  /// 2*size. This allows some shader optimizations which would otherwise not be possible.
  ///
  /// Note that some platforms may not support textures larger than 4096 on any edge. Furthermore, keep in mind that a renderable
  /// texture has to fit into 10 MB of EDRAM memory in Xbox360, so you should keep this limitation in mind when specifying the
  /// shadow map size. For instance, with two cascades, shadow map sizes larger than 1024 are not allowed on Xbox360.
  ///
  /// \param size
  ///   Size of the shadow map (width and height).
  EFFECTS_IMPEXP void SetShadowMapSize(unsigned int size);

  /// \brief
  ///   Returns the current sample radius
  inline float GetSampleRadius() const
  { 
    return SampleRadius; 
  }

  /// \brief
  ///   Sample radius. 
  ///
  /// Sets the sample radius used for filtering the shadow maps. Typically, a higher sample radius produces softer shadows, but
  /// may produce more rendering artifacts in certain scenarios. Also, a higher samples radius often degrades performance somewhat,
  /// since it usually lowers the texture cache hit rate.
  ///
  /// The sample radius is ignored for PCF4 filtering.
  ///
  /// \param radius
  ///  Sample radius, in units.
  EFFECTS_IMPEXP void SetSampleRadius(float radius);

  /// \brief
  ///   Returns the scale factor which is used to increase the sample radius with the distance
  inline float GetSampleRadiusScaleWithDistance() const 
  {
    return SampleRadiusScaleWithDistance;
  }

  /// \brief
  ///   Sets the scale factor which is used to increase the sample radius with the distance.
  /// 
  /// A scale value of 0 (default) disables the effect. A scale value greater than 0 means that the sample radius is increased with 
  /// the scale value multiplied by the distance of the pixel to the camera. This can be used to reduce aliasing artifacts in the 
  /// distance as well as to smooth the transition between cascades.
  ///
  /// \param radiusScale
  ///  Sample radius scale, this value is added to the sample radius per distance unit.
  inline void SetSampleRadiusScaleWithDistance(float radiusScale)
  {
    SampleRadiusScaleWithDistance = radiusScale;
  }

  /// \brief
  ///   Returns the maximum distance to a blocker for Percentage-Closer-Soft-Shadows in world space.
  inline float GetMaxDistanceToBlocker() const
  {
    return MaxDistanceToBlocker;
  }

  /// \brief
  ///   Returns the maximum distance to a blocker for Percentage-Closer-Soft-Shadows in light space.
  EFFECTS_IMPEXP float GetMaxDistanceToBlockerInLightSpace() const;

  /// \brief
  ///   Sets the maximum distance to a blocker for Percentage-Closer-Soft-Shadows in world space. At this distance 
  ///   the sample radius reaches it's full size and will not grow any further with increasing distance.
  inline void SetMaxDistanceToBlocker(float fMaxDistanceToBlocker)
  {
    MaxDistanceToBlocker = fMaxDistanceToBlocker;
  }

  /// \brief
  ///   Returns the status that has been set via SetUseSurfaceSpecificShadowShaders.
  inline BOOL GetUseSurfaceSpecificShadowShaders() const
  { 
    return UseSurfaceSpecificShadowShaders; 
  }

  /// \brief
  ///   If enabled, shadow map rendering uses shaders that are part of the surface structure (VisSurface_cl::m_spShadowmapFill).
  ///
  /// For custom effects or materials, it may be desirable to use custom shaders for rendering into the shadow map (e.g. because
  /// geometry is generated in a shader, or because the depth values are modified before outputting them). If this is desired, you can
  /// specify a separate shadow map fill technique in your custom shader. This technique has to have the inclusion tag "SpecificShadowmapFill".
  /// In order to enable usage of these surface-specific shaders, you have to call SetUseSurfaceSpecificShadowShaders with bStatus set to
  /// TRUE. Surfaces without a specific shadow map fill technique will still use the custom technique.
  ///
  /// \param bStatus
  ///   TRUE enables support for surface-specific shadow shaders, FALSE (default) disables it. Enabling it may slightly degrade performance
  ///   on the CPU side.
  EFFECTS_IMPEXP void SetUseSurfaceSpecificShadowShaders(BOOL bStatus);

  /// \brief
  ///   Returns the depth bias value for the passed cascade index
  inline float GetBias(int cascade = 0) const 
  { 
    return Bias[cascade]; 
  }

  /// \brief
  ///   Sets the depth bias for the passed cascade index. 
  /// 
  /// The depth bias specifies an offset for the depth values when geometry is rendered into the shadow maps. This bias can be used
  /// to reduce self-shadowing artifacts. Positive values bias the depth away from the light, negative values towards the light.
  /// Typically, it is a good idea to choose larger depth bias values for cascades that are further away.
  /// 
  /// \param iCascade
  ///   Cascade for which to set the depth bias.
  /// 
  /// \param fBias
  ///   Depth bias in units.
  EFFECTS_IMPEXP virtual void SetBias(int iCascade, float fBias);

  /// \brief
  ///   Sets the depth bias for all cascades. The passed array must hold enough elements for the cascades, which is at most 4.
  EFFECTS_IMPEXP virtual void SetBias(float* bias, int size);

  /// \brief
  ///   Returns the render filter bitmask that is used to filter relevant scene elements for shadow casting
  inline unsigned int GetFilterBitmask() const 
  { 
    return FilterBitmask; 
  }

  /// \brief
  ///   Sets supported geometry types, where each bit is a type in defined in VShadowCastingGeometry_e
  EFFECTS_IMPEXP void SetGeometryTypes(int iFlags);


  /// \brief
  ///   Return the value that has been set via SetGeometryTypes. By default, all flags are set
  inline int GetGeometryTypes() const
  {
    return GeometryTypes;
  }

  
  /// \brief
  ///   Sets the render filter bitmask that is used to filter relevant scene elements for shadow casting
  EFFECTS_IMPEXP void SetFilterBitmask(unsigned int bitmask);

  /// \brief
  ///   Returns the near clip distance of the shadow render context
  inline float GetNearClip() const
  { 
    return NearClip; 
  }

  /// \brief
  ///   Sets the near clip distance of the shadow render context for spot / point lights
  /// 
  /// For spot/point lights, a near clip distance has to be specified. The default value is 2.0. This setting will have no
  /// effect for directional lights.
  ///
  /// \param fNearClip
  ///   Near clip distance.
  EFFECTS_IMPEXP void SetNearClip(float fNearClip);

  ///
  /// @}
  ///

  ///
  /// @name Features for shadow map components attached to static lights
  /// @{
  ///

  /// \brief
  ///   Returns the ambient color (see SetAmbientColor for details).
  inline const VColorRef& GetAmbientColor() const
  { 
    return AmbientColor; 
  }

  /// \brief
  ///   Sets the ambient color of shadows.
  ///
  /// When using a shadow map component for a static light in forward-rendering mode, the ambient color will be used as the
  /// intensity of the shadows being cast. The darker the ambient color, the darker the shadows. This setting will have no
  /// effect for dynamic lights.
  ///
  /// \param color
  ///   Ambient color of the shadows.
  inline void SetAmbientColor(const VColorRef& color) 
  { 
    AmbientColor = color; 
  }

  /// \brief
  ///   Returns the heuristic object bounding box extrusion factor for determining the range of the shadow cast by an object.
  inline float GetShadowBoxExtrudeMultiplier() const
  { 
    return ShadowBoxExtrudeMultiplier; 
  }

  /// \brief
  ///   Sets the heuristic object bounding box extrusion factor for determining the range of the shadow cast by an object.
  ///
  /// When using a shadow map component for a static light in forward-rendering mode, the shadow-receiving geometry will have to
  /// be re-rendered for the shadows to become visible. In this case, the receiving geometry is determined from the shadow volumes
  /// of the shadow-casting objects. To compute the shadow volumes, the bounding boxes of the shadow-casting objects are extruded
  /// by a certain range in the light's direction.
  ///
  /// This range is computed as the height of the object's bounding box multiplied by the shadow box extrusion multiplier passed
  /// into this method.
  ///
  /// \param fMultiplier
  ///   Shadow box extrusion multiplier.
  inline void SetShadowBoxExtrudeMultiplier(float fMultiplier) 
  { 
    ShadowBoxExtrudeMultiplier = fMultiplier; 
  }

  /// \brief
  ///   Specifies whether front-facing or back-facing geometry casts shadows
  ///
  /// By default, back-faces (i.e., polygons facing away from the light source) cast shadows. This is often slightly
  /// fast than front-facing shadows, and in most scenes, this makes it easier to avoid artifacts caused by undersampling
  /// the shadow map or lack of precision. However, in some cases where scenes are modelled with single-sided surfaces,
  /// this can result in missing geometry in the shadow map. For these cases, it may be a good idea to switch to
  /// front-facing shadows.
  ///
  /// \param bFrontFacingShadows
  ///   TRUE to enable front-facing shadows, FALSE to use back-facing shadows.
  EFFECTS_IMPEXP void SetFrontFacingShadows(BOOL bFrontFacingShadows);

  /// \brief
  ///   Returns the value previously set with SetFrontFacingShadows().
  inline BOOL GetFrontFacingShadows() const
  {
    return FrontFacingShadows;
  }

  ///
  /// @}
  ///


  ///
  /// @name Unsupported/internal
  /// @{
  ///

  /// \brief
  ///   Static helper function which returns the shadow map component attached to the passed light source for the specified renderer node.
  ///   NULL is returned if no matching combination is found.
  static EFFECTS_IMPEXP IVShadowMapComponent *GetShadowMapComponent(VisLightSource_cl *pLight, VRendererNodeCommon *pRendererNode);

  ///
  /// @}
  ///

  ///
  /// @name Unsupported/internal
  /// @{
  ///

  // Unsupported.
  inline float GetSlopeScaled(int cascade = 0) const
  { 
    return SlopeScaled[cascade]; 
  }

  // Unsupported.
  EFFECTS_IMPEXP void SetSlopeScaled(int cascade, float slope);

  // Unsupported.
  EFFECTS_IMPEXP void SetSlopeScaled(float* slope, int size);

  ///
  /// @}
  ///


protected:
  friend class VShadowMapGenerator;

  EFFECTS_IMPEXP virtual bool InitializeRenderer();
  EFFECTS_IMPEXP virtual void DeInitializeRenderer();

  IVShadowMapFormat* GetShadowMapFormat() const;

  //helper function to clamp the near clip value. Returns the clamped value.
  float ClampNearClip(float nearClip);

  char m_iLocalVersion;

  //vartable properties
  BOOL Enabled;
  BOOL UseQuarterSizeShadowTexture;
  VShadowMappingMode_e ShadowMappingMode;
  unsigned int ShadowMapSize;
  float SampleRadius;
  float SampleRadiusScaleWithDistance;
  float MaxDistanceToBlocker;
  BOOL UseSurfaceSpecificShadowShaders;

  float Bias[MAX_SHADOW_PARTS_COUNT];
  float SlopeScaled[MAX_SHADOW_PARTS_COUNT];

  unsigned int FilterBitmask;
  float NearClip;
  VColorRef AmbientColor;
  float ShadowBoxExtrudeMultiplier;
  int GeometryTypes;
  BOOL FrontFacingShadows;

  VisLightSource_cl *m_pLightSource;
  VRendererNodeCommon* m_pRendererNode;
  int m_iRendererNodeIndex;
  VShadowMapGeneratorBasePtr m_spShadowMapGenerator;

  VisMeshBuffer_cl* m_pLightVolumeMeshBuffer;

  bool m_bIsInitialized;
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
