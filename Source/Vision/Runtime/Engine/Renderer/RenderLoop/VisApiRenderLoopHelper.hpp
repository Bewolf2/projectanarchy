/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiRenderLoopHelper.hpp

#ifndef DEFINE_VISAPIRENDERLOOPHELPER
#define DEFINE_VISAPIRENDERLOOPHELPER

#define VIS_GEOMSTYLE_OPAQUE                    1
#define VIS_GEOMSTYLE_TRANSLUCENT               2
#define VIS_GEOMSTYLE_TRANSLUCENT_LIT           3

// Shadows: Shadow Casters
#define VIS_LIGHTSRCVIS_PRIMITIVES              1
#define VIS_LIGHTSRCVIS_MODELS                  2
#define VIS_LIGHTSRCVIS_VISIBILITYOBJECTS       4
#define VIS_LIGHTSRCVIS_ALLGEOMTYPES           15
#define VIS_LIGHTSRCVIS_ONLYSHADOWS            32
#define VIS_LIGHTSRCVIS_ALLINFLUENCEDGEOMETRY  64

#define VIS_LIGHTFLAGS_DYNAMICLIGHTS                0x00000001
#define VIS_LIGHTFLAGS_STATICLIGHTS                 0x00000002
#define VIS_LIGHTFLAGS_ALL_LIGHTS                   (VIS_LIGHTFLAGS_DYNAMICLIGHTS|VIS_LIGHTFLAGS_STATICLIGHTS)
#define VIS_LIGHTFLAGS_RECOMPUTE                    0x00000004
#define VIS_LIGHTFLAGS_CONSIDERWORLDINFLUENCEMASK   0x00000010
#define VIS_LIGHTFLAGS_CONSIDERENTITYINFLUENCEMASK  0x00000020

#define VIS_RLP_MODELSTATE_UNINITIALISED        0
#define VIS_RLP_MODELSTATE_BASE                 1
#define VIS_RLP_MODELSTATE_SHADER               2

#define VIS_PROFILINGINFO_TEXT         0x01
#define VIS_PROFILINGINFO_GEOMETRY     0x02
#define VIS_PROFILINGINFO_ALL          0xFF

#include <Vision/Runtime/Engine/SceneElements/VisApiSortedRenderCollection.hpp>

#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>

#ifndef _VISION_DOC

class VDynamicMeshStreamState
{
public:
  VDynamicMeshStreamState()
  {
    Reset();
  }

  void Reset()
  {
    bStreamsReset = true;
    bBoneWeightStreamSet = false;
    bSkinnedVertexStreamSet = false;
    pLastMesh = NULL;
    pLastAnimConfig = NULL;
    iLastShaderStreamMask = 0;
  }

  bool bStreamsReset;
  bool bBoneWeightStreamSet;
  bool bSkinnedVertexStreamSet;
  VDynamicMesh *pLastMesh;
  VisAnimConfig_cl *pLastAnimConfig;
  int iLastShaderStreamMask;
};

#endif


/// \brief
///   Render Loop Helper class
/// 
/// This class provides helper functionality for writing custom render loops. All methods defined
/// in this class may only be used inside render loops (not e.g. in entity code).
/// 
/// The default VisionRenderLoop_cl implementation provides a good insight into how these methods
/// are typically used.
class VisRenderLoopHelper_cl
{

  public:
    VISION_APIFUNC VisRenderLoopHelper_cl();
    VISION_APIFUNC ~VisRenderLoopHelper_cl();

    ///
    /// @name Render Helper Functions
    /// @{
    ///


    /// \brief Flags that specify which render-targets should be cleared.
    enum VClearTargetFlags_e
    {
      VCTF_Color        = 1 << 0,                                     ///< Clears all color render-targets
      VCTF_Stencil      = 1 << 1,                                     ///< Clears the stencil buffer
      VCTF_Depth        = 1 << 2,                                     ///< Clears the depth buffer
      VCTF_DepthStencil = VCTF_Stencil | VCTF_Depth,                  ///< Clears the depth/stencil buffer (usually faster than just clearing one of them)
      VCTF_All          = VCTF_Color | VCTF_Stencil | VCTF_Depth,     ///< Clears all buffers (color + depth + stencil)
    };
  
    /// \brief
    ///   Clears the depth, stencil, and/or color buffers
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// \param uiBufferFlags
    ///   One or more of the flags defined in VClearTargetFlags_e can be specified.
    ///   For performance reasons it is strongly recommended to clear depth and stencil buffers together, instead of separately.
    /// 
    /// \param iClearColor
    ///   Color with which to clear the color buffer.
    /// 
    /// \param fDepthRef
    ///   Depth value with which to clear the depth buffer.
    /// 
    /// \param iStencilRef
    ///   Stencil value with which to clear the stencil buffer.
    /// 
    /// \param iRTMask
    ///   Mask indicating which color render targets to clear. Bit 0 represents RT 0, Bit 1
    ///   represents RT 1, and so on. This does not have any effect if the VIS_COLORBUFFER flag is
    ///   not set in the iBufferFlags, or if the respective render target is not set.
    VISION_APIFUNC void ClearScreen(unsigned int uiBufferFlags = VCTF_All, VColorRef iClearColor = VColorRef(0,0,0,0), float fDepthRef = 1.0f, int iStencilRef = 0, int iRTMask = 1) const;

    /// \brief
    ///   Renders a collection of particle groups
    /// 
    /// The groups in this collection will neither be sorted nor checked for visibility.
    /// 
    /// To sort the particle groups (e.g. according to their distance from the camera), use the
    /// VisParticleGroupCollection_cl::SortParticleGroups function.
    /// 
    /// Only the particle groups in the collection that have the correct render order and bitmask
    /// will be rendered by this function.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// \param pGroups
    ///   Pointer to the particle group collection.
    /// 
    /// \param iOrder
    ///   Only the particle groups with this render order will be rendered.
    /// 
    /// \param iFilterMask
    ///   Filter bitmask for the AND comparison.
    /// 
    /// \sa VisParticleGroupCollection_cl
    /// \sa VisParticleGroupCollection_cl::SortParticleGroups
    VISION_APIFUNC void RenderParticleSystems(const VisParticleGroupCollection_cl *pGroups, unsigned int iOrder, unsigned int iFilterMask);

    /// \brief
    ///   This version of RenderParticleSystems renders the passed collection without further filtering
    VISION_APIFUNC void RenderParticleSystems(const VisParticleGroupCollection_cl *pGroups);

    /// \brief
    ///   Renders a collection of particle groups with a specific shader
    /// 
    /// The groups in this collection will neither be sorted nor checked for visibility.
    /// 
    /// To sort the particle groups, use the VisParticleGroupCollection_cl::SortParticleGroups
    /// function.
    /// 
    /// \param pGroups
    ///   Pointer to particle group collection.
    /// 
    /// \param Shader
    ///   Shader to use for rendering.
    /// 
    /// \sa VisParticleGroupCollection_cl
    /// \sa VisParticleGroupCollection_cl::SortParticleGroups
    /// \sa VCompiledShaderPass
    VISION_APIFUNC void RenderParticleSystemsWithShader(const VisParticleGroupCollection_cl *pGroups, VCompiledShaderPass &Shader);

    /// \brief
    ///   Renders the current sky.
    /// 
    ///  For a detailed reference of sky definitions, please refer to the VSky  class.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// \param pSky
    ///   The sky to be rendered. Can be NULL to use VIsion::World.GetActiveSky()
    VISION_APIFUNC void RenderSky(IVSky *pSky=NULL);

    /// \brief
    ///   Render all visibility portals for debug purposes. Called when DEBUGRENDERFLAG_PORTALS is
    ///   specified for VisProfiling_cl::SetDebugRenderFlags
    VISION_APIFUNC void RenderPortals();

    /// \brief
    ///   Render all visibility zones for debug purposes. Called when
    ///   DEBUGRENDERFLAG_VISIBILITYZONES is specified for VisProfiling_cl::SetDebugRenderFlags
    VISION_APIFUNC void RenderVisibilityZones();

    /// \brief
    ///   Renders profiling information.
    /// 
    /// This includes all entity-specific profiling data (e.g. individual triangle counts) and the profiling charts.
    /// 
    /// This functionality is used for profiling and debugging purposes only and should not be used
    /// in release versions.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// In general, this method should be called after the regular scene elements have been
    /// rendered.
    ///
    /// \param iInfoType
    ///   Specifies the type of profiling information to render. Basically, there is textual output (e.g.
    ///   lists, per-object information, etc.), and geometric output (e.g. lines, bounding boxes). Since
    ///   some renderer implementation may require separate rendering of textual and geometric output (e.g.
    ///   since the geometric output requires a depth buffer), this parameter can be used to specify what
    ///   should be rendererd. Allowed inputs are VIS_PROFILINGINFO_ALL (default), VIS_PROFILINGINFO_TEXT,
    ///   and VIS_PROFILINGINFO_GEOMETRY.
    VISION_APIFUNC void RenderProfilingInfo(int iInfoType = VIS_PROFILINGINFO_ALL);

    /// \brief
    ///   Renders all visible screen masks.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// The visibility filter bitmasks are taken from VisScreenMask_cl::GetVisibleBitmask and
    /// compared to the filter mask of the current render context
    /// (VisRenderContext_cl::GetRenderFilterMask)
    /// 
    /// The visible screen masks are sorted by their sorting order (VisScreenMask_cl::SetOrder).
    /// 
    /// \sa VisScreenMask_cl::SetOrder
    /// \sa VisScreenMask_cl::GetVisibleBitmask
    /// \sa VisRenderContext_cl::GetRenderFilterMask
    VISION_APIFUNC void RenderScreenMasks();

    /// \brief
    ///   Renders screen masks with a specified filtering bitmask.
    /// 
    /// This version of RenderScreenMasks allows specifying a filtering bitmask rather than using
    /// the render context's filtering bitmask.
    /// 
    /// Apart from that, this function works the same way as the RenderScreenMasks version without
    /// parameters.
    /// 
    /// \param iVisFilter
    ///   Filter bitmask for the AND comparison.
    VISION_APIFUNC void RenderScreenMasks(unsigned int iVisFilter);

    /// \brief
    ///   Renders a collection of screen masks.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// This version does not perform sorting of the screen masks, but rather renders them in the
    /// order in which they are stored in the collection.
    /// 
    /// If sorting is desired, the VisScreenMaskCollection_cl::SortByOrder method may be called
    /// before rendering.
    /// 
    /// \param masks
    ///   collection of screen masks to render
    /// 
    /// \param pShader
    ///   If not NULL, this shader will be applied to the rendered screen masks.
    /// 
    /// \sa VisScreenMaskCollection_cl
    /// \sa VisScreenMaskCollection_cl::SortByOrder
    inline VISION_APIFUNC void RenderScreenMasks(const VisScreenMaskCollection_cl &masks, VCompiledShaderPass *pShader = NULL)
    {
      RenderScreenMasks(masks.GetDataPtr(), masks.GetNumEntries(), pShader);
    }

    /// \brief
    ///   Renders a collection of screen masks.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// This version does not perform sorting of the screen masks, but rather renders them in the
    /// order in which they are stored in the collection.
    /// 
    /// 
    /// \param ppMasks
    ///   Pointer to an array of screen masks to render.
    ///
    /// \param iNumMasks
    ///  The number of screen masks in the array.
    /// 
    /// \param pShader
    ///   If not NULL, this shader will be applied to the rendered screen masks.
    VISION_APIFUNC void RenderScreenMasks(VisScreenMask_cl** ppMasks, int iNumMasks, VCompiledShaderPass *pShader);

    /// \brief
    ///   Renders an entity with a set of shaders.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// \param pEntity
    ///   Pointer to entity that should be rendered
    /// 
    /// \param iNumShaders
    ///   Number of shaders with which the entity is to be rendered
    /// 
    /// \param pShaderList
    ///   Pointer to the list of shaders with which the entity is to be rendered
    VISION_APIFUNC void RenderEntityWithShaders(VisBaseEntity_cl *pEntity, unsigned int iNumShaders, VCompiledShaderPass **pShaderList);

    /// \brief
    ///   Renders an entity with a set of surface-specific shaders
    /// 
    /// Since entities may have multiple surfaces, this method provides a way to dynamically render
    /// shaders on specific surfaces of an entity. For this purpose, the VisDrawCallInfo_t
    /// provides a way to combine sets of shaders and entity surfaces arbitrarily.
    /// 
    /// This method has a certain performance impact associated with it, in comparison with the
    /// RenderEntityWithShaders method. Keeping the number of shader/surface combinations as low as
    /// possible (e.g. by simply using as few entity surfaces as possible) will greatly help
    /// performance.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// \param pEntity
    ///   Pointer to entity that should be rendered
    /// 
    /// \param iNumModelSurfaceShaders
    ///   Number of VisDrawCallInfo_t structures passed in the respective array
    /// 
    /// \param pModelSurfaceShaderList
    ///   Pointer to the array of surface/shader assignments for this entity.
    VISION_APIFUNC void RenderEntityWithSurfaceShaderList(VisBaseEntity_cl *pEntity, unsigned int iNumModelSurfaceShaders, const VisDrawCallInfo_t *pModelSurfaceShaderList);

    /// \brief
    ///   Render a collection of entities with the default material shaders or with the assigned shader set respectively
    VISION_APIFUNC void RenderEntitiesWithDefaultShaders(const VisEntityCollection_cl &EntityCollection, bool bPreBasepass, int iPreferEntityThreshold);

    /// \brief
    ///   Render a collection of entities (all materials with the same shader pass)
    ///
    /// This function is useful to render entities into a context with the same shader pass, e.g. depth fill pass.
    /// This function calls BeginEntityRendering/EndEntityRendering internally.
    ///
    /// \param EntityCollection
    ///   The input collection of objects to render
    ///
    /// \param ShaderPass
    ///   The shader pass used for rendering
    ///
    /// \param iFirstEntity
    ///   The first entity in the collection to render. 0 to start at the beginning of the collection.
    ///
    /// \param iNumEntities
    ///   The number of entities in the collection to render. -1 to render all entities in the collection.
    VISION_APIFUNC void RenderEntitiesWithShader(const VisEntityCollection_cl &EntityCollection, VCompiledShaderPass &ShaderPass, int iFirstEntity = 0, int iNumEntities = -1);

    /// \brief
    ///   Opens a BeginEntityRendering/EndEntityRendering bracket. Some functions may only be called inside the scope of that bracket
    VISION_APIFUNC void BeginEntityRendering();

    /// \brief
    ///   Ends a BeginEntityRendering/EndEntityRendering bracket. Some functions may only be called inside the scope of that bracket
    VISION_APIFUNC void EndEntityRendering();

    /// \brief
    ///   Renders a collection of mesh buffer objects
    /// 
    /// The mesh buffer objects are additionally filtered using a bitmask which is compared to the
    /// order value.
    /// 
    /// In case the mesh buffer object should not be rendered in the standard render loop, but
    /// solely by calls to RenderMeshBufferObjects, the VRH_CUSTOM order constant can
    /// be used for the mesh buffer objects.
    /// 
    /// Also see VisMeshBufferObject_cl::SetOrder.
    /// 
    /// \param objectlist
    ///   Collection of mesh buffer objects  to be rendered
    /// 
    /// \param iFilterMask
    ///   Filter bitmask for the order bitmask. See VisMeshBufferObject_cl::SetOrder
    /// 
    /// \sa VisRenderLoopHelper_cl::RenderMeshBufferObjectsWithShader
    /// \sa VisMeshBufferObject_cl::SetOrder
    VISION_APIFUNC void RenderMeshBufferObjects(const VisMeshBufferObjectCollection_cl &objectlist, unsigned int iFilterMask=0xffffffff);

    /// \brief
    ///   Same as RenderMeshBufferObjects, but uses a shader for rendering all objects
    /// 
    /// \param objectlist
    ///   Collection of mesh buffer objects to be rendered
    /// 
    /// \param shader
    ///   Shader used for rendering the objects in the collection
    /// 
    /// \param iFilterMask
    ///   Filter bitmask for the order bitmask. See VisMeshBufferObject_cl::SetOrder
    /// 
    /// \sa VisRenderLoopHelper_cl::RenderMeshBufferObjects
    /// \sa VisMeshBufferObject_cl::SetOrder
    VISION_APIFUNC void RenderMeshBufferObjectsWithShader(const VisMeshBufferObjectCollection_cl &objectlist, VCompiledShaderPass &shader, unsigned int iFilterMask=0xffffffff);


    /// \brief Computes the far frustum corners in world space. DEPRECATED: USe 'ComputeFrustumFarCorners' instead.
    /// 
    /// Far Corners are returned in this order:
    /// Bottom Left, Top Left, Top Right, Bottom Right
    ///
    /// \param pNode
    ///   The render node to take the frustum from.
    ///
    /// \param vFarCorners
    ///   out: pointer to a hkvVec3[4] array which will contain the results at the end of the function
    HKV_DEPRECATED_2012_2 static VISION_APIFUNC void ComputeFarFrustumCorners(IVRendererNode *pNode, hkvVec3* vFarCorners);

    /// \brief 
    ///   Computes the corners of the view-frustum at the far-plane. Optionally also returns the right-direction and up-direction vector.
    static VISION_APIFUNC void ComputeFrustumFarCorners (IVRendererNode *pNode, hkvVec3* out_vTopLeft = NULL, hkvVec3* out_vBottomLeft = NULL, hkvVec3* out_vBottomRight = NULL, hkvVec3* out_vTopRight = NULL, hkvVec3* out_vRightDir = NULL, hkvVec3* out_vDownDir = NULL);

    ///
    /// @}
    ///

    ///
    /// @name Light Influence Helper Functions
    /// @{
    ///
  
    /// \brief
    ///   Fills the passed collections of static geometry instances, entities and visibility
    ///   objects with lists of geometry that lies within a light source's range.
    /// 
    /// Using a simple region-of-influence test, this method adds all static geometry instances,
    /// entities, and/or visibility objects within the influence of the passed light source to the
    /// respective collections.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// \param pStaticGeomInstances
    ///   If NULL, static geometry instances will be ignored in the influence test. Otherwise, this
    ///   has to be the pointer to a valid VisStaticGeometryInstanceCollection_cl instance to which
    ///   affected static geometry instances are added.
    /// 
    /// \param pEntities
    ///   If NULL, entities will be ignored in the influence test. Otherwise, this has to be the
    ///   pointer to a valid VisEntityCollection_cl instance to which affected entities are added.
    /// 
    /// \param pVisObjects
    ///   If NULL, visibility objects will be ignored in the influence test. Otherwise, this has to
    ///   be the pointer to a valid VisVisibilityObjectCollection_cl instance to which affected
    ///   visibility objects are added.
    /// 
    /// \param LightSource
    ///   Reference to the light for which the affected geometry is to be determined.
    ///
    /// \param bConsiderCastShadowFlag
    ///   True if the cast shadow flag of scene elements should be considered in the test, false if not.
    VISION_APIFUNC void GetGeometryInLightsourceRange(VisStaticGeometryInstanceCollection_cl *pStaticGeomInstances, VisEntityCollection_cl *pEntities, VisVisibilityObjectCollection_cl *pVisObjects, VisLightSource_cl &LightSource, bool bConsiderCastShadowFlag=false);

    /// \brief
    ///   Fills the passed collections of scene elements with elements that lie within a light
    ///   source's range and can serve as occluders for the given frustum.
    /// 
    /// This is the case if, seen from the light source, this scene element may occlude any of the
    /// geometry inside the frustum.
    /// 
    /// This method is useful for implementing shadow-casting algorithms.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// \param pViewFrustum
    ///   View frustum for which the occlusion test should be performed.
    /// 
    /// \param pStaticGeomInstances
    ///   If NULL, static geometry instances will be ignored in the influence test. Otherwise, this
    ///   has to be the pointer to a valid VisStaticGeometryInstanceCollection_cl instance to which
    ///   relevant static geometry instances are added.
    /// 
    /// \param pEntities
    ///   If NULL, entities will be ignored in the influence test. Otherwise, this has to be the
    ///   pointer to a valid VisEntityCollection_cl instance to which relevant entities are added.
    /// 
    /// \param pVisObjects
    ///   If NULL, visibility objects will be ignored in the influence test. Otherwise, this has to
    ///   be the pointer to a valid VisVisibilityObjectCollection_cl instance to which relevant
    ///   visibility objects are added.
    /// 
    /// \param lightSource
    ///   Reference to the light for which the relevant occluder geometry is to be determined.
    VISION_APIFUNC void GetOccludersInLightsourceRange(const VisFrustum_cl *pViewFrustum, VisStaticGeometryInstanceCollection_cl *pStaticGeomInstances, VisEntityCollection_cl *pEntities, VisVisibilityObjectCollection_cl *pVisObjects, VisLightSource_cl &lightSource);

    /// \brief
    ///   Fills the passed collections of scene elements with lists of elements that lie within a
    ///   light source's range.
    /// 
    /// Using a simple region-of-influence test, this method adds all scene elements within the
    /// influence of the passed light source to the respective collections.
    /// 
    /// Unlike GetGeometryInLightsourceRange, this method will only return scene elements which are
    /// classified as visible in the current render context's visibility collector.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// \param pStaticGeomInstances
    ///   If NULL, static geometry instances will be ignored in the influence test. Otherwise, this
    ///   has to be the pointer to a valid VisStaticGeometryInstanceCollection_cl instance to which
    ///   affected static geometry instances are added.
    /// 
    /// \param pEntities
    ///   If NULL, entities will be ignored in the influence test. Otherwise, this has to be the
    ///   pointer to a valid VisEntityCollection_cl instance to which affected entities are added.
    /// 
    /// \param pVisObjects
    ///   If NULL, visibility objects will be ignored in the influence test. Otherwise, this has to
    ///   be the pointer to a valid VisVisibilityObjectCollection_cl instance to which affected
    ///   visibility objects are added.
    /// 
    /// \param lightSource
    ///   Reference to the light for which the affected geometry is to be determined.
    /// 
    /// \param bConsiderCastShadowFlag
    ///   If true, only candidates with SetCastShadows flag enabled go into the lists, otherwise
    ///   the light influence bitflags are compared
    VISION_APIFUNC void GetVisibleGeometryInLightsourceRange(VisStaticGeometryInstanceCollection_cl *pStaticGeomInstances, VisEntityCollection_cl *pEntities, VisVisibilityObjectCollection_cl *pVisObjects, const VisLightSource_cl &lightSource, bool bConsiderCastShadowFlag=false);

    ///
    /// @}
    ///

    ///
    /// @name Visibility Helper Functions
    /// @{
    ///

    /// \brief
    ///   Renders scheduled hardware occlusion queries.
    /// 
    /// This method temporarily disables multi-sample anti-aliasing (if it was previously enabled)
    /// and calls VisRenderContext_cl::RenderScheduledOcclusionTests.
    /// 
    /// \sa VisRenderContext_cl::RenderScheduledOcclusionTests
    VISION_APIFUNC void PerformHardwareOcclusionQuery();

    /// \brief
    ///   Renders scheduled hardware occlusion queries for pixel counters.
    /// 
    /// This method temporarily disables multi-sample anti-aliasing (if it was previously enabled)
    /// and calls VisRenderContext_cl::RenderScheduledPixelCounterTests.
    /// 
    /// \sa VisRenderContext_cl::RenderScheduledPixelCounterTests
    VISION_APIFUNC void PerformHardwarePixelCounterQuery();


    ///
    /// @}
    ///

    ///
    /// @name Custom Lighting Helper Functions
    /// @{
    ///

    /// \brief
    ///   Determines the currently most relevant lights from a collection of input light sources.
    /// 
    /// Using a heuristic, this method determines the lights from the input collection which are
    /// most relevant for the visible part of the scene, and adds them to the output collection.
    /// The maximum number of lights to be returned is specified as a parameter to this method.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// Note that iMaxLights is currently limited to eight light sources.
    /// 
    /// \param InputLightSources
    ///   Input light source collection
    /// 
    /// \param OutputLightSources
    ///   Output light source collection
    /// 
    /// \param iMaxLights
    ///   Maximum number of lights to be added to the output collection
    /// 
    /// \param bIncludeProjected
    ///   If set to FALSE, projected lights are ignored
    /// 
    /// \param iFlags
    ///   Determines which types of lights are to be considered. The following flags are valid:
    ///   \li VIS_LIGHTFLAGS_DYNAMICLIGHTS : dynamic lights are considered.
    /// 
    ///   \li VIS_LIGHTFLAGS_STATICLIGHTS : static lights are considered.
    /// 
    ///   \li VIS_LIGHTFLAGS_ALL_LIGHTS : both dynamic and static lights are considered
    ///     (combination of previous constants).
    /// 
    ///   \li VIS_LIGHTFLAGS_CONSIDERWORLDINFLUENCEMASK : If specified, the light's world influence
    ///     bitmask is checked to be !=0.
    /// 
    ///   \li VIS_LIGHTFLAGS_CONSIDERENTITYINFLUENCEMASK : If specified, the light's object
    ///     influence bitmask is checked to be !=0.
    VISION_APIFUNC void GetMostRelevantLights(const VisLightSrcCollection_cl &InputLightSources, VisLightSrcCollection_cl &OutputLightSources, unsigned int iMaxLights, BOOL bIncludeProjected, int iFlags);


    /// \brief
    ///   Determines the currently most relevant lights for the bounding box.
    /// 
    /// Using a heuristic, this method determines the lights from the scene which are most relevant
    /// for the bounds, and adds them to the output collection. The maximum number of lights to be
    /// returned is specified as a parameter to this method.
    /// 
    /// This method may exclusively be called in render loop implementations.
    /// 
    /// Note that iMaxLights is currently limited to eight light sources.
    /// 
    /// \param bounds
    ///   The bounding box to determine the lights for
    /// 
    /// \param outputLightSources
    ///   Output light source collection
    /// 
    /// \param maxLights
    ///   Maximum number of lights to be added to the output collection
    VISION_APIFUNC void GetMostRelevantLightsForBoundingBox(const hkvAlignedBBox& bounds, VisLightSrcCollection_cl& outputLightSources, int maxLights);


    ///
    /// @}
    ///

    ///
    /// @name Tagging Functions
    /// @{
    ///


    /// \brief
    ///   Resets the tags on all lightsources.
    /// 
    /// This method has very little overhead.
    VISION_APIFUNC void ResetLightSourceTags();

    /// \brief
    ///   Returns whether a light source has been tagged
    /// 
    /// since the last call to ResetLightSourceTags.
    /// 
    /// \param pLight
    ///   pointer to light source
    /// 
    /// \return
    ///   BOOL: TRUE if the light has been tagged, otherwise FALSE.
    VISION_APIFUNC BOOL IsLightSourceTagged(const VisLightSource_cl *pLight);

    /// \brief
    ///   Tags a light source
    /// 
    ///  The tag is removed in the next call to ResetLightSourceTags.
    /// 
    /// \param pLight
    ///   Pointer to light source to be tagged
    VISION_APIFUNC void TagLightSource(VisLightSource_cl *pLight);

    /// \brief
    ///   Resets the tags on all entities.
    /// 
    /// Calling this method takes virtually no time.
    VISION_APIFUNC void ResetEntityTags();

    /// \brief
    ///   Tags an entity
    /// 
    ///  The tag is removed in the next call to ResetEntityTags.
    /// 
    /// \param pEntity
    ///   Pointer to entity to be tagged
    VISION_APIFUNC void TagEntity(VisBaseEntity_cl *pEntity);

    /// \brief
    ///   Returns whether an entity has been tagged
    /// 
    /// since the last call to ResetEntityTags.
    /// 
    /// \param pEntity
    ///   pointer to entity.
    /// 
    /// \return
    ///   BOOL: TRUE if the entity has been tagged, otherwise FALSE.
    VISION_APIFUNC BOOL IsEntityTagged(const VisBaseEntity_cl *pEntity);

    /// \brief
    ///   Resets the tags on all visibility objects.
    /// 
    /// Calling this method takes virtually no time.
    VISION_APIFUNC void ResetVisibilityObjectTags();

    /// \brief
    ///   Tags a visibility object
    /// 
    ///  The tag is removed in the next call to ResetVisibilityObjectTags.
    /// 
    /// \param pVisObj
    ///   Pointer to Visibility object to be tagged.
    VISION_APIFUNC void TagVisibilityObject(VisVisibilityObject_cl *pVisObj);

    /// \brief
    ///   Returns whether a visibility object has been tagged
    /// 
    /// since the last call to ResetVisibilityObjectTags.
    /// 
    /// \param pVisObj
    ///   Pointer to visibility object.
    /// 
    /// \return
    ///   BOOL: TRUE if the visibility object has been tagged, otherwise FALSE.
    VISION_APIFUNC BOOL IsVisibilityObjectTagged(const VisVisibilityObject_cl *pVisObj);


    ///
    /// @}
    ///

    ///
    /// @name Lightgrid Functions
    /// @{
    ///


    /// \brief
    ///   Access the global light grid of the scene
    /// 
    /// This can be a NULL pointer if the scene has no lightgrid information.
    /// 
    /// The lightgrid can be used to do some custom light color lookups.
    /// 
    /// \return
    ///   VLightGrid_cl *pGrid : A pointer to the lightgrid
    /// 
    /// \sa VLightGrid_cl
    /// \sa VisRenderLoopHelper_cl::SetLightGrid
    inline VLightGrid_cl *GetLightGrid() const {return m_spLightGrid;}

    /// \brief
    ///   Sets the light grid used for scene lighting
    /// 
    /// Usually it isn't needed to set the light grid manually, since the engine will automatically
    /// load the light grid file corresponding to the loaded .lit file.
    /// 
    /// \param pGrid
    ///   The light grid instance to set
    /// 
    /// \sa VLightGrid_cl
    /// \sa VisRenderLoopHelper_cl::GetLightGrid
    VISION_APIFUNC void SetLightGrid(VLightGrid_cl * pGrid);

    /// \brief
    ///   Tracks the light grid colors of an entity into shader constants
    /// 
    /// The entity's position is used to perform the lightgrid lookup in the global lightgrid. The
    /// function takes care of caching entity light grid colors in case the entity does not change
    /// its position, because lightgrid lookups are relatively expensive.
    /// 
    /// The resulting colors are stored internally. The next time a shader is set which uses light
    /// grid tracking, the  colors will automatically be uploaded into the correct shader
    /// constants.
    /// 
    /// \param pEntity
    ///   Entity for which the lightgrid colors should be tracked.
    /// 
    /// \sa VLightGrid_cl
    /// \sa VisRenderLoopHelper_cl::GetLightGrid
    VISION_APIFUNC void TrackLightGridInfo(VisBaseEntity_cl *pEntity);

    /// \brief
    ///   Tracks custom lightgrid colors into shader constants so the colors are available in the shader.
    /// 
    /// The colors are stored internally. The next time a shader is set which uses light grid
    /// tracking, the  colors will automatically be uploaded into the correct shader constants.
    /// 
    /// \param pColors
    ///   A pointer to 6 vectors which represent the 6 colors of a lightgrid sample. The order of
    ///   cube directions is +x,-x,+y,-y,+z,-z 
    /// 
    /// \sa VLightGrid_cl
    /// \sa VisRenderLoopHelper_cl::GetLightGrid
    VISION_APIFUNC void TrackLightGridInfo(const hkvVec3* pColors);

    /// \brief
    ///   Tracks custom lightgrid colors into shader constants so the colors are available in the shader.
    /// 
    /// This version performs a lightgrid lookup of the specified position in the global lightgrid.
    /// 
    /// However, if lightgrid information of entities are supposed to be tracked, the version of
    /// TrackLightGridInfo that takes an entity as input is much faster, since it can efficiently
    /// cache results on a per-entity basis.
    /// 
    /// The colors are stored internally. The next time a shader is set which uses light grid
    /// tracking, the  colors will automatically be uploaded into the correct shader constants.
    /// 
    /// \param vPos
    ///   a position used for lightgrid lookup.
    /// 
    /// \sa VLightGrid_cl
    /// \sa VisRenderLoopHelper_cl::GetLightGrid
    VISION_APIFUNC void TrackLightGridInfo(const hkvVec3& vPos);



    /// \brief
    ///   Evaluates the light contribution of all active dynamic light sources to a world space
    ///   position
    /// 
    /// This function is very useful to simulate dynamic lights on objects with light grid based
    /// lighting.
    /// 
    /// For instance, the static light grid colors can be cached per object and the dynamic
    /// contribution is added when tracking the object colors.
    /// 
    /// This function uses the the object influence bitmask of the dynamic lights (see iLightMask
    /// parameter).
    /// 
    /// As an optimization, this function only considers dynamic lights that actually have an
    /// impact on the visual range, i.e. those whose radius of influence intersect with the view
    /// frustum.
    /// 
    /// \param vPos
    ///   The world position to evaluate the colors for
    /// 
    /// \param pDestColors
    ///   Destination array of 6 colors. The dynamic contribution is added to the passed colors
    ///   without initially resetting them.
    /// 
    /// \param iLightMask
    ///   A bitmask that is used for filtering out relevant dynamic light. This bitmask is
    ///   logically AND-ed with the object bitmask of the dynamic light
    ///   (VisLightSource_cl::GetLightInfluenceBitMaskEntity).
    /// 
    /// \sa TrackLightGridInfo
    VISION_APIFUNC void GetDynamicLightContribution(const hkvVec3& vPos, hkvVec3* pDestColors, int iLightMask=0xffffffff);

    /// \brief
    ///   Forces cached light grid data to be updated for all relevant scene objects by invalidating all cached values.
    /// 
    /// This method has to be called if the light grid is modified. 
    /// 
    /// Entity or static meshes using cached light grid data will subsequently recompute their
    /// illumination data the next frame they are rendered.
    /// 
    /// \sa VLightGrid_cl
    /// \sa VisRenderLoopHelper_cl::TrackLightGridInfo
    VISION_APIFUNC void InvalidateLightgrid();

    /// \brief
    ///   Indicates whether the scene has valid lightmaps. Lightmap textures are available in all
    ///   cases
    inline bool HasLightmaps() const
    {
      return m_bHasLightmaps;
    }

    /// \brief
    ///   Indicates whether a light grid file has been loaded for the scene. If not, a dummy light
    ///   grid object is created.
    inline bool HasLightGrid() const
    {
      return m_spLightGrid != NULL;
    }

    ///
    /// @}
    ///

    ///
    /// @name 2d Overlay Rendering
    /// @{
    ///

    /// \brief
    ///   Begins a block of 2D overlay rendering, e.g. for GUI
    /// 
    /// This function can be called at any time inside the render loop, but not outside.
    /// 
    /// The returned IVRender2DInterface object can be used to render 2D triangles. This interface
    /// can be passed to GUI components that can render themselves.
    /// 
    /// A block of 2D overlay rendering must be closed again with EndOverlayRendering before
    /// issuing any other rendering calls.
    /// 
    /// \return
    ///   IVRender2DInterface *pRenderer : Interface that provides low-level funcionality to render
    ///   2D vertex buffers.
    /// 
    /// \sa VisRenderLoopHelper_cl::EndOverlayRendering
    /// \sa IVRender2DInterface
    VISION_APIFUNC IVRender2DInterface *BeginOverlayRendering();

    /// \brief
    ///   Closes a block of 2D rendering that has been opened with BeginOverlayRendering
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginOverlayRendering
    /// \sa IVRender2DInterface
    VISION_APIFUNC void EndOverlayRendering();

    /// \brief
    ///   Returns whether this function is called from within a Begin/EndOverlayRendering bracket
    VISION_APIFUNC bool IsInOverlayRenderingBracket();

    /// \brief
    ///   Sets a 2D hardware clipping rectangle defined in screen coordinates.
    /// 
    /// This can be useful to either clip GUI elements at the client area of a dialog or to reduce
    /// the fillrate impact of pixel shaders that only affect a small area on-screen but need to
    /// render large parts of the geometry. 
    /// 
    /// Note that the scissor rect requires the scissor test in the shader or in the simple state
    /// structure (depending on the rendering command used) to be enabled in order to have an
    /// effect!
    /// 
    /// See e.g. the Blob Shadow Sample in the Vision SDK as a reference.
    /// 
    /// \param pScreenRect
    ///   Clipping rectangle in screen coordinates. Can be NULL to set the clipping rectangle to
    ///   cover the complete viewport.
    VISION_APIFUNC void SetScissorRect(const VRectanglef *pScreenRect);

    ///
    /// @}
    ///


    ///
    /// @name Model Rendering
    /// @{
    ///

    /// \brief
    ///   Renders a (static) model with a specific shader
    /// 
    /// This method provides functionality for rendering a static model without requiring an
    /// entity. In many cases, this allows for more streamlined rendering code. 
    /// 
    /// It is recommended to use this method in cases where you have many instances of the same
    /// model, or where you would otherwise have "dumb" entities without any logic code. In these
    /// cases, the RenderModel* method will yield significantly better performance.
    /// 
    /// This function may only be called inside a renderloop, inside a BeginEntityRendering/EndEntityRendering
    /// 
    /// \param pModel
    ///   Model to render
    /// 
    /// \param pMatrix
    ///   Transformation matrix with which to render the model
    /// 
    /// \param pShader
    ///   Shader with which to render the model
    /// 
    /// \param pAnimConfig
    ///   Optional animation config that is used to render the mesh. The anim config must be compatible with the mesh (no validation takes place).
    ///   Furthermore this config must be prepared for rendering which is always the case if the config is associated with a visible entity of the scene. If not bound to an entity,
    ///   the VisAnimConfig_cl::StartComputeCurrentVertexResult function must be called manually in the update loop, otherwise the result might be undefined.
    ///   Also note that this function does not 'tick' the anim config, so standalone anim config instances must call VisAnimConfig_cl::UpdateAnimState during scene update.
    VISION_APIFUNC void RenderModelWithShader(VDynamicMesh *pModel, const hkvMat4* pMatrix, VCompiledShaderPass *pShader, VisAnimConfig_cl *pAnimConfig = NULL);

    /// \brief
    ///   Renders a (static) model with a shader list
    /// 
    /// This method provides functionality for rendering a model without requiring an entity. In
    /// many cases, this allows for more streamlined rendering code. 
    /// 
    /// It is recommended to use this method in cases where you have many instances of the same
    /// model, or where you would otherwise have "dumb" entities without any logic code. In these
    /// cases, the RenderModel* method will yield significantly better performance.
    /// 
    /// This function may only be called inside a renderloop, inside a BeginEntityRendering/EndEntityRendering
    ///
    /// \param pModel
    ///   Model to render
    /// 
    /// \param pMatrix
    ///   Transformation matrix with which to render the model
    /// 
    /// \param iNumModelSurfaceShaders
    ///   Number of surface shader assignments to use
    /// 
    /// \param pModelSurfaceShaderList
    ///   List of surface shaders for this model
    /// 
    /// \param pCustomTextureSet
    ///   Optional set of texture replacements. See also VisBaseEntity_cl::SetCustomTextureSet. Can
    ///   be NULL to use model textures.
    /// 
    /// \param pAnimConfig
    ///   Optional animation config that is used to render the mesh. The anim config must be compatible with the mesh (no validation takes place).
    ///   Furthermore this config must be prepared for rendering which is always the case if the config is associated with a visible entity of the scene. If not bound to an entity,
    ///   the VisAnimConfig_cl::StartComputeCurrentVertexResult function must be called manually in the update loop, otherwise the result might be undefined.
    ///   Also note that this function does not 'tick' the anim config, so standalone anim config instances must call VisAnimConfig_cl::UpdateAnimState during scene update.
    VISION_APIFUNC void RenderModelWithSurfaceShaderList(VDynamicMesh *pModel, const hkvMat4* pMatrix, unsigned int iNumModelSurfaceShaders, const VisDrawCallInfo_t *pModelSurfaceShaderList, VisSurfaceTextureSet_cl *pCustomTextureSet=NULL, VisAnimConfig_cl *pAnimConfig=NULL);

    ///
    /// @}
    ///

    ///
    /// @name Mesh Buffer Rendering
    /// @{
    ///

    /// \brief
    ///   Starts a block inside a render loop for low level mesh rendering without mesh buffer
    ///   object instances
    /// 
    /// Inside a BeginMeshRendering/EndMeshRendering block, mesh buffers can be rendered directly
    /// by binding streams and texture samplers.
    /// 
    /// The relevant functions inside a block are ResetMeshStreams, AddMeshStreams, BindMeshTexture
    /// and RenderMeshes.
    /// 
    /// However, no other rendering functions might be called inside this block, e.g.
    /// RenderEntityWithShaders etc. Also, this block is not for rendering mesh buffer objects via
    /// RenderMeshBufferObjects
    /// 
    /// This functionality is very useful for modular plugins that render geometry in the scene
    /// without creating instances of mesh buffer objects.
    /// 
    /// To render a block of meshes the plugin code can hook into existing render loops via the
    /// Vision::Callbacks.OnRenderHook callback.
    /// 
    /// For an implementation sample, please refer to the Volumetric Cone rendering in the
    /// VisionEnginePlugin (file VolumetricCone.cpp).
    /// 
    /// \sa VisRenderLoopHelper_cl::EndMeshRendering
    /// \sa VisRenderLoopHelper_cl::ResetMeshStreams
    /// \sa VisRenderLoopHelper_cl::AddMeshStreams
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    VISION_APIFUNC void BeginMeshRendering();

    /// \brief
    ///   Ends a block of direct mesh rendering. See BeginMeshRendering.
    VISION_APIFUNC void EndMeshRendering();

    /// \brief
    ///   Returns whether we are currently inside a BeginMeshRendering/EndMeshRendering bracket.
    VISION_APIFUNC bool IsInMeshRenderingBracket();

    /// \brief
    ///   Resets all vertex streams set via AddMeshStreams inside a
    ///   BeginMeshRendering/EndMeshRendering block
    /// 
    /// This function should be called when the setup of new mesh types starts.
    /// 
    /// It is also possible to overwrite streams by calling AddMeshStreams with new mesh data.
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::AddMeshStreams
    VISION_APIFUNC void ResetMeshStreams();

    /// \brief
    ///   Sets one or more mesh vertex streams from a mesh buffer inside a
    ///   BeginMeshRendering/EndMeshRendering block.
    /// 
    /// These streams are used when rendering the meshes via RenderMeshes. Distinct streams can be
    /// set from more than on mesh buffer (e.g. using the color stream from another mesh buffer)
    /// 
    /// Beginning with Vision 7.2, streams do NOT automatically overwrite previously streams. Use
    /// the stream masks to ensure that you are not duplicating stream components if you bind
    /// multiple mesh buffers, and call ResetMeshStreams to restart binding mesh buffers.
    /// 
    /// \param pMesh
    ///   The source mesh buffer that contains the streams.
    /// 
    /// \param iStreamMasks
    ///   A bitmask that defines which streams should be set. It only sets the streams if the
    ///   respective stream is actually defined in the mesh, so the VERTEX_STREAM_ALL (defined to
    ///   0xffffffff)  can always be used to bind all available streams.The following constants are
    ///   defined:
    ///   \li VERTEX_STREAM_POSITION : Bind the position vertex stream,
    ///   \li VERTEX_STREAM_NORMAL : Bind the normal vector vertex stream,
    ///   \li VERTEX_STREAM_COLOR : Bind the color vertex stream,
    ///   \li VERTEX_STREAM_SECONDARYCOLOR : Bind the secondary vertex stream (not supported on all
    ///     platforms),
    ///   \li VERTEX_STREAM_TEX0 : Bind the texture coordinate vertex stream of stage 0,
    ///   \li VERTEX_STREAM_TEX1 : Bind the texture coordinate vertex stream of stage 1,
    ///   \li VERTEX_STREAM_TEX2 : Bind the texture coordinate vertex stream of stage 2,
    ///   \li VERTEX_STREAM_TEX3 : Bind the texture coordinate vertex stream of stage 3,
    ///   \li VERTEX_STREAM_TEX4 : Bind the texture coordinate vertex stream of stage 4,
    ///   \li VERTEX_STREAM_TEX5 : Bind the texture coordinate vertex stream of stage 5,
    ///   \li VERTEX_STREAM_TEX6 : Bind the texture coordinate vertex stream of stage 6,
    ///   \li VERTEX_STREAM_TEX7 : Bind the texture coordinate vertex stream of stage 7,
    ///   \li VERTEX_STREAM_INDEXBUFFER : Use the index buffer of this mesh buffer
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::ResetMeshStreams
    VISION_APIFUNC void AddMeshStreams(VisMeshBuffer_cl *pMesh, int iStreamMasks=VERTEX_STREAM_ALL);

    /// \brief
    ///   Sets one or more mesh vertex streams from a mesh buffer inside a
    ///   BeginMeshRendering/EndMeshRendering block.
    /// 
    /// These streams are used when rendering the meshes via RenderMeshes. Distinct streams can be
    /// set from more than on mesh buffer (e.g. using the color stream from another mesh buffer)
    /// 
    /// This function supports instancing, i.e. providing distinct geometry streams and
    /// per-instance streams.
    /// 
    /// Beginning with Vision 7.2, streams do NOT automatically overwrite previously streams. Use
    /// the stream masks to ensure that you are not duplicating stream components if you bind
    /// multiple mesh buffers, and call ResetMeshStreams to restart binding mesh buffers.
    /// 
    /// When migrating to Vision 7.2 from earlier versions, note that the meaning of the
    /// iInstanceCount parameter has changed, and that there is a new method called
    /// VisRenderLoopHelper_cl::SetMeshInstanceCount.
    /// 
    /// Note that Xbox360 requires manual handling of instancing in the shader using the vfetch
    /// instruction.
    /// 
    /// Note that Playstation 3 uses the instancing information to set up dividers/modulos for the
    /// vertex indices in the following way: If iInstanceValue==0, the operation used is "modulo
    /// the total vertex count". If iInstanceValue>0, the operation used is "divided by
    /// (iInstanceValue*total vertex count)". Take into account that the PS3 still requirs you to
    /// set up an appropriately sized index buffer. Refer to the hardware instancing chapter of the
    /// PS3 SDK documentation for further details.
    /// 
    /// \param pMesh
    ///   The source mesh buffer that contains the streams.
    /// 
    /// \param iStreamMasks
    ///   A bitmask that defines which streams should be set. It only sets the streams if the
    ///   respective stream is actually defined in the mesh, so the VERTEX_STREAM_ALL (defined to
    ///   0xffffffff)  can always be used to bind all available streams.The following constants are
    ///   defined:
    ///   \li VERTEX_STREAM_POSITION : Bind the position vertex stream,
    ///   \li VERTEX_STREAM_NORMAL : Bind the normal vector vertex stream,
    ///   \li VERTEX_STREAM_COLOR : Bind the color vertex stream,
    ///   \li VERTEX_STREAM_SECONDARYCOLOR : Bind the secondary vertex stream (not supported on all
    ///     platforms),
    ///   \li VERTEX_STREAM_TEX0 : Bind the texture coordinate vertex stream of stage 0,
    ///   \li VERTEX_STREAM_TEX1 : Bind the texture coordinate vertex stream of stage 1,
    ///   \li VERTEX_STREAM_TEX2 : Bind the texture coordinate vertex stream of stage 2,
    ///   \li VERTEX_STREAM_TEX3 : Bind the texture coordinate vertex stream of stage 3,
    ///   \li VERTEX_STREAM_TEX4 : Bind the texture coordinate vertex stream of stage 4,
    ///   \li VERTEX_STREAM_TEX5 : Bind the texture coordinate vertex stream of stage 5,
    ///   \li VERTEX_STREAM_TEX6 : Bind the texture coordinate vertex stream of stage 6,
    ///   \li VERTEX_STREAM_TEX7 : Bind the texture coordinate vertex stream of stage 7,
    ///   \li VERTEX_STREAM_INDEXBUFFER : Use the index buffer of this mesh buffer
    ///
    /// \param iFirstVertex: 
    ///   The index of the first vertex (internally calculates the byte
    ///   offset inside the vertex buffer as iFirstVertex*stride)
    /// 
    /// \param iInstanceCount: 
    ///   The number of instances of this geometry to be rendered. The
    ///   engine uses the following convention:
    ///   \li If this value is 0, this stream defines the instanced mesh geometry (if the instance
    ///     count set with SetMeshInstanceCount is >1), or instancing is off (if the instance count
    ///     set with SetMeshInstanceCount is <=1),
    ///   \li If this value is >0, this stream defince per-instance data, and the value defines the
    ///     step rate for the mesh data. Note that DX9 only supports a step rate of 1.
    ///
    /// \param iStreamIndex:
    ///   On DirectX9/10/11-based platforms (PC, Xbox360), this value can be used to specify the
    ///   stream index for the mesh buffer. By default, this parameter is -1, indicating that the
    ///   Vision Engine will use the next free mesh buffer. When using values other than -1, the
    ///   mesh buffers have to be added in proper order, meaning that you first have to set stream
    ///   index 0, then 1, then 2, and so on. However, you can also use this parameter to overwrite
    ///   previously set stream information (mesh buffer, first vertex, instance count) at a specific
    ///   index. When replacing streams, the order of the replacement does not matter (e.g. you can
    ///   replace stream index 1, then 0, then 2). Replacing streams is only possible when the
    ///   stream mask of the newly set stream matches that of the stream previusly set at the same
    ///   index.
    ///   Stream Replacement can be used to reduce CPU overhead during rendering, since it reduces
    ///   the need for calls to ResetMeshStreams().
    ///
    /// \returns
    ///   int: The stream index that was actually used. If a stream index is explicitly specified, this
    ///   stream index will be returned.
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::ResetMeshStreams
    VISION_APIFUNC int AddMeshStreamsEx(VisMeshBuffer_cl *pMesh, int iStreamMasks=VERTEX_STREAM_ALL, int iFirstVertex=0, int iInstanceCount=0, int iStreamIndex=-1);

    /// \brief
    ///   Sets the total number of mesh instances to be rendered, and the number of vertices in the
    ///   original mesh vertex buffer.
    /// 
    /// The number of vertices is only relevant for the PS3 version in order to set up index
    /// modulo/division values.
    /// 
    /// The number of instances is only relevant for the XBOX360, PS3, Vita, DX9 and DX11 versions in order to render
    /// the right number of instances.
    /// 
    /// \param iInstanceCount
    ///   Number of instances to render.
    /// 
    /// \param iNumVertices
    ///   Number of vertices in the "original" vertex buffer (i.e., the mesh stream).
    VISION_APIFUNC void SetMeshInstanceCount(int iInstanceCount, int iNumVertices);

    /// \brief
    ///   Binds a texture object to a pixel shader sampler stage for mesh rendering inside a
    ///   BeginMeshRendering/EndMeshRendering block.
    /// 
    /// Textures that are set this way can be referenced by the shader as "BaseTexture" of the
    /// respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// \param pTexture
    ///   The texture object to bind
    /// 
    /// \param iSamplerStage
    ///   The sampler stage to bind the texture. Must be in the range
    ///   [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \param pSampler
    ///   Sampler properties to set
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureVS
    VISION_APIFUNC void BindMeshTexture(VTextureObject *pTexture, int iSamplerStage, const VStateGroupSampler *pSampler = NULL);

    /// \brief
    ///   Binds a texture object to a vertex shader sampler stage for mesh rendering inside a
    ///   BeginMeshRendering/EndMeshRendering block.
    /// 
    /// Textures that are set this way can be referenced by the shader as "BaseTexture" of the
    /// respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// \param pTexture
    ///   The texture object to bind
    /// 
    /// \param iSamplerStage
    ///   The sampler stage to bind the texture. Must be in the range
    ///   [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \param pSampler
    ///   Sampler properties to set
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    VISION_APIFUNC void BindMeshTextureVS(VTextureObject *pTexture, int iSamplerStage, const VStateGroupSampler *pSampler);

    /// \brief
    ///   Helper function to call VisRenderLoopHelper_cl::BindMeshTexture for all textures that are used by the passed shader
    /// 
    /// This function may only be used inside a BeginMeshRendering/EndMeshRendering block.
    /// 
    /// \param pSurface
    ///   The surface that provides base textures
    /// 
    /// \param pPass
    ///   The shader pass that defines the binding for the textures
    /// 
    /// \param pDiffuseAnim
    ///   Optional animation instance for the diffuse texture animation
    ///
    VISION_APIFUNC void BindSurfaceTextures(VisSurface_cl *pSurface, VCompiledShaderPass *pPass, VisTextureAnimInstance_cl *pDiffuseAnim=NULL);

    /// \brief
    ///   Helper function to bind appropriate material state groups if the passed shader uses the m_bBlendingFromSurface resp. the m_bRasterizerStyleFromSurface flag
    VISION_APIFUNC void BindDefaultStateGroups(VisSurface_cl *pSurface, VCompiledShaderPass *pPass);

#if defined(_VR_DX11)

    /// \brief
    ///   Binds a texture object to a geometry shader sampler stage for mesh rendering inside a
    ///   BeginMeshRendering/EndMeshRendering block.
    /// 
    /// Textures that are set this way can be referenced by the shader as "BaseTexture" of the
    /// respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// This method is only available on PC DX10/DX11.
    /// 
    /// \param pTexture
    ///   The texture object to bind
    /// 
    /// \param iSamplerStage
    ///   The sampler stage to bind the texture. Must be in the range
    ///   [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \param pSampler
    ///   Sampler properties to set
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureVS
    VISION_APIFUNC void BindMeshTextureGS(VTextureObject *pTexture, int iSamplerStage, const VStateGroupSampler *pSampler);

   #ifdef _VR_DX11

    /// \brief
    ///   Binds a texture object to a hull shader sampler stage for mesh rendering inside a
    ///   BeginMeshRendering/EndMeshRendering block.
    /// 
    /// Textures that are set this way can be referenced by the shader as "BaseTexture" of the
    /// respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// This method is only available on PC DX11.
    /// 
    /// \param pTexture
    ///   The texture object to bind
    /// 
    /// \param iSamplerStage
    ///   The sampler stage to bind the texture. Must be in the range
    ///   [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \param pSampler
    ///   Sampler properties to set
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureVS
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureGS
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureDS
    VISION_APIFUNC void BindMeshTextureHS(VTextureObject *pTexture, int iSamplerStage, const VStateGroupSampler *pSampler);

    /// \brief
    ///   Binds a shader resource to a hull shader stage for mesh rendering.
    /// 
    /// Unlike the BindMeshTextureXX methods, this method does not take a sample state as input,
    /// and it can not only be used to bind textures, but also vertex or index buffers as shader
    /// resources.
    /// 
    /// Note that vertex or index buffers intended to be used as shader resources need to have been
    /// created with the appropriate bind flag (VIS_BIND_SHADER_RESOURCE).
    /// 
    /// Shader Resources that are set this way can be referenced by the shader as "BaseTexture" of
    /// the respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// This method is supported on PC DX11 only.
    /// 
    /// \note
    ///   This function is typically used for mesh rendering inside a BeginMeshRendering/EndMeshRendering
    ///   block. But in some situations like in overwritten VisMeshBufferObject_cl::OnRender functions it
    ///   is necessary to bind a shader resource outside a Begin/End block. Note that it's up to the
    ///   developer to make sure that the texture stage is not colliding with a stage defined in a shader
    ///   sampler.
    ///
    /// \param pResourceView
    ///   The shader resource view to bind
    /// 
    /// \param iStage
    ///   The stage to bind the resource to. Must be in the range [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::BindShaderResourcePS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceVS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceGS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceDS
    VISION_APIFUNC void BindShaderResourceHS(VShaderResourceView *pResourceView, int iStage);

    /// \brief
    ///   Binds a texture object to a domain shader sampler stage for mesh rendering inside a
    ///   BeginMeshRendering/EndMeshRendering block.
    /// 
    /// Textures that are set this way can be referenced by the shader as "BaseTexture" of the
    /// respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// This method is only available on PC DX11.
    /// 
    /// \param pTexture
    ///   The texture object to bind
    /// 
    /// \param iSamplerStage
    ///   The sampler stage to bind the texture. Must be in the range
    ///   [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \param pSampler
    ///   Sampler properties to set
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureVS
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureGS
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureHS
    VISION_APIFUNC void BindMeshTextureDS(VTextureObject *pTexture, int iSamplerStage, const VStateGroupSampler *pSampler);

    /// \brief
    ///   Binds a shader resource to a domain shader stage for mesh rendering.
    /// 
    /// Unlike the BindMeshTextureXX methods, this method does not take a sample state as input,
    /// and it can not only be used to bind textures, but also vertex or index buffers as shader
    /// resources.
    /// 
    /// Note that vertex or index buffers intended to be used as shader resources need to have been
    /// created with the appropriate bind flag (VIS_BIND_SHADER_RESOURCE).
    /// 
    /// Shader Resources that are set this way can be referenced by the shader as "BaseTexture" of
    /// the respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// This method is supported on PC DX11 only.
    /// 
    /// \note
    ///   This function is typically used for mesh rendering inside a BeginMeshRendering/EndMeshRendering
    ///   block. But in some situations like in overwritten VisMeshBufferObject_cl::OnRender functions it
    ///   is necessary to bind a shader resource outside a Begin/End block. Note that it's up to the
    ///   developer to make sure that the texture stage is not colliding with a stage defined in a shader
    ///   sampler.
    ///
    /// \param pResourceView
    ///   The shader resource view to bind
    /// 
    /// \param iStage
    ///   The stage to bind the resource to. Must be in the range [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::BindShaderResourcePS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceVS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceGS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceHS
    VISION_APIFUNC void BindShaderResourceDS(VShaderResourceView *pResourceView, int iStage);

    #endif //_VR_DX11


    /// \brief
    ///   Binds a shader resource to a pixel shader stage for mesh rendering.
    /// 
    /// Unlike the BindMeshTextureXX methods, this method does not take a sample state as input,
    /// and it can not only be used to bind textures, but also vertex or index buffers as shader
    /// resources.
    /// 
    /// Note that vertex or index buffers intended to be used as shader resources need to have been
    /// created with the appropriate bind flag (VIS_BIND_SHADER_RESOURCE).
    /// 
    /// Shader Resources that are set this way can be referenced by the shader as "BaseTexture" of
    /// the respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// This method is supported on PC DX10/DX11 only.
    /// 
    /// \note
    ///   This function is typically used for mesh rendering inside a BeginMeshRendering/EndMeshRendering
    ///   block. But in some situations like in overwritten VisMeshBufferObject_cl::OnRender functions it
    ///   is necessary to bind a shader resource outside a Begin/End block. Note that it's up to the
    ///   developer to make sure that the texture stage is not colliding with a stage defined in a shader
    ///   sampler.
    ///
    /// \param pResourceView
    ///   The shader resource view to bind
    /// 
    /// \param iStage
    ///   The stage to bind the resource to. Must be in the range [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceVS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceGS
    VISION_APIFUNC void BindShaderResourcePS(VShaderResourceView *pResourceView, int iStage);

    /// \brief
    ///   Binds a shader resource to a vertex shader stage for mesh rendering.
    /// 
    /// Unlike the BindMeshTextureXX methods, this method does not take a sample state as input,
    /// and it can not only be used to bind textures, but also vertex or index buffers as shader
    /// resources.
    /// 
    /// Note that vertex or index buffers intended to be used as shader resources need to have been
    /// created with the appropriate bind flag (VIS_BIND_SHADER_RESOURCE).
    /// 
    /// Shader Resources that are set this way can be referenced by the shader as "BaseTexture" of
    /// the respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// This method is supported on PC DX10/DX11 only.
    /// 
    /// \note
    ///   This function is typically used for mesh rendering inside a BeginMeshRendering/EndMeshRendering
    ///   block. But in some situations like in overwritten VisMeshBufferObject_cl::OnRender functions it
    ///   is necessary to bind a shader resource outside a Begin/End block. Note that it's up to the
    ///   developer to make sure that the texture stage is not colliding with a stage defined in a shader
    ///   sampler.
    ///
    /// \param pResourceView
    ///   The shader resource view to bind
    /// 
    /// \param iStage
    ///   The stage to bind the resource to. Must be in the range [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::BindShaderResourcePS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceGS
    VISION_APIFUNC void BindShaderResourceVS(VShaderResourceView *pResourceView, int iStage);

    /// \brief
    ///   Binds a shader resource to a geometry shader stage for mesh rendering.
    /// 
    /// Unlike the BindMeshTextureXX methods, this method does not take a sample state as input,
    /// and it can not only be used to bind textures, but also vertex or index buffers as shader
    /// resources.
    /// 
    /// Note that vertex or index buffers intended to be used as shader resources need to have been
    /// created with the appropriate bind flag (VIS_BIND_SHADER_RESOURCE).
    /// 
    /// Shader Resources that are set this way can be referenced by the shader as "BaseTexture" of
    /// the respective sampler stage.
    /// 
    /// The textures set on the mesh via SetChannelTexture are ignored.
    /// 
    /// This method is supported on PC DX10/DX11 only.
    /// 
    /// \note
    ///   This function is typically used for mesh rendering inside a BeginMeshRendering/EndMeshRendering
    ///   block. But in some situations like in overwritten VisMeshBufferObject_cl::OnRender functions it
    ///   is necessary to bind a shader resource outside a Begin/End block. Note that it's up to the
    ///   developer to make sure that the texture stage is not colliding with a stage defined in a shader
    ///   sampler.
    ///
    /// \param pResourceView
    ///   The shader resource view to bind
    /// 
    /// \param iStage
    ///   The stage to bind the resource to. Must be in the range [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::BindShaderResourcePS
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceVS
    VISION_APIFUNC void BindShaderResourceGS(VShaderResourceView *pResourceView, int iStage);

#endif

    /// \brief
    ///   Triggers a rendering call of one or more mesh instances inside a
    ///   BeginMeshRendering/EndMeshRendering block.
    /// 
    /// This function uses the vertex streams (and optionally index buffer) of the meshes set via
    /// AddMeshStreams.
    /// 
    /// Textures are set via BindMeshTexture.
    /// 
    /// Except from the streams this function does not use any properties of the added mesh
    /// buffers, i.e. it ignores the mesh's own textures, render state, rendering technique,
    /// primitive type and primitive count.
    /// 
    /// \param pShader
    ///   A valid shader to render the meshes. Must not be NULL.
    /// 
    /// \param iCount
    ///   The number of mesh instances to render
    /// 
    /// \param pFirstMatrix
    ///   Pointer to the first transformation matrix (rotation and translation) that transforms the
    ///   mesh in 3D space. The rotation part can also contain scaling. if iCount is set to 1,
    ///   pFirstMatrix just points the transformation matrix.
    /// 
    /// \param iMatrixStride
    ///   Defines the matrix stride (in Bytes) if iCount>1. If an array of hkvMat4 matrices
    ///   is passed as pFirstMatrix this value must be sizeof (hkvMat4).
    /// 
    /// \param ePrimType
    ///   VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType: Defines the primitive type to render. See
    ///   VisMeshBuffer_cl::MB_PrimitiveType_e.
    /// 
    /// \param iFirstPrimitive
    ///   0-based index of the first primitive(=triangle) to render. For indexed tri-strips this is actually the index offset
    /// 
    /// \param iPrimitiveCount
    ///   number of primitives(=triangles) to render.
    /// 
    /// \param iVertexCount
    ///   The number of vertices used by the rendering. Should be the minimum of all streams set.
    /// 
    /// \param bForSkinning
    ///   If true, the skinning vertex shader contained in pShader will be used instead of the regular, non-skinning vertex
    ///   shader. If pShader does not contain a skinning vertex shader, setting bForSkinning to true will have no effect.
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::AddMeshStreams
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    VISION_APIFUNC void RenderMeshes(VCompiledShaderPass *pShader, int iCount, const hkvMat4* pFirstMatrix, int iMatrixStride, VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType, int iFirstPrimitive, int iPrimitiveCount, int iVertexCount, bool bForSkinning = false);

    /// \brief
    ///   This version of RenderMeshes uses the current transformation matrix (see
    ///   VisRenderLoopHelper_cl::SetMeshTransformationMatrix). Apart from that, the function is identical to the other
    ///   RenderMeshes version
    /// 
    /// \param pShader
    ///   A valid shader to render the meshes. Must not be NULL.
    /// 
    /// \param ePrimType
    ///   VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType: Defines the primitive type to render. See
    ///   VisMeshBuffer_cl::MB_PrimitiveType_e.
    /// 
    /// \param iFirstPrimitive
    ///   0-based index of the first primitive(=triangle) to render. For indexed tri-strips this is actually the index offset
    /// 
    /// \param iPrimitiveCount
    ///   number of primitives(=triangles) to render.
    /// 
    /// \param iVertexCount
    ///   The number of vertices used by the rendering. Should be the minimum of all streams set.
    /// 
    /// \param bForSkinning
    ///   If true, the skinning vertex shader contained in pShader will be used instead of the regular, non-skinning vertex
    ///   shader. If pShader does not contain a skinning vertex shader, setting bForSkinning to true will have no effect.
    /// 
    /// \sa VisRenderLoopHelper_cl::SetMeshTransformationMatrix
    VISION_APIFUNC void RenderMeshes(VCompiledShaderPass *pShader, VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType, int iFirstPrimitive, int iPrimitiveCount, int iVertexCount, bool bForSkinning = false);

    /// \brief
    ///   Sets a new mesh rendering transformation matrix to render meshes via the RenderMeshes
    ///   function
    /// 
    /// This function may only be called inside a BeginMeshRendering/EndMeshRendering block.
    /// 
    /// This matrix only affects mesh rendering via RenderMeshes (the version without
    /// transformation matrices).
    /// 
    /// Using this function, multiple different mesh rendering calls can share the same
    /// transformation matrix without setting vertex shader constants in between.
    /// 
    /// \param transform
    ///   The new transformation matrix. Applies rotation, scaling and position of the meshes.
    /// 
    /// \param bUseViewTransform
    ///   If set to true, the passed transformation matrix is concatenated with the current  view
    ///   transform (default behavior). 
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMeshRendering
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    /// \sa VisRenderLoopHelper_cl::AddMeshStreams
    /// \sa VisRenderLoopHelper_cl::BindMeshTexture
    /// \sa VisRenderLoopHelper_cl::RenderMeshes
    VISION_APIFUNC void SetMeshTransformationMatrix(const hkvMat4& transform, bool bUseViewTransform = true);

#if defined(_VR_DX11) || defined(_VISION_DOC)

    ///
    /// @name Compute Shader Support
    /// @{
    ///

    /// \brief
    ///   Returns whether we are currently inside a BeginCompute/EndCompute bracket. This method is only available on PC DX11.
    VISION_APIFUNC bool IsInComputeBracket();
    
    /// \brief
    ///   Initiates a compute shader bracket.
    ///
    /// Functions which use compute shader-specific properties - such as BindMeshTextureCS, BindShaderResourceCS,
    /// BindUnorderedAccessViewCS, and RunComputeShader - may only be called inside BeginCompute/EndCompute brackets.
    ///
    /// This method is only available on PC DX11.
    VISION_APIFUNC void BeginCompute();

    /// \brief
    ///   Ends a compute shader bracket.
    ///
    /// Functions which use compute shader-specific properties - such as BindMeshTextureCS, BindShaderResourceCS,
    /// BindUnorderedAccessViewCS, and RunComputeShader - may only be called inside BeginCompute/EndCompute brackets.
    ///
    /// This method is only available on PC DX11.
    VISION_APIFUNC void EndCompute();

    /// \brief
    ///   Binds a texture object to a compute shader texture stage (shader resource view).
    /// 
    /// Textures that are set this way can be referenced by the shader as "BaseTexture" of the
    /// respective sampler stage. 
    /// 
    /// This method is only available on PC DX11.
    /// 
    /// \param pTexture
    ///   The texture object to bind
    /// 
    /// \param iSamplerStage
    ///   The sampler stage to bind the texture. Must be in the range
    ///   [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \param pSampler
    ///   Sampler properties to set
    /// 
    /// \sa VisRenderLoopHelper_cl::RunComputeShader
    /// \sa VisRenderLoopHelper_cl::BindShaderResourceCS
    /// \sa VisRenderLoopHelper_cl::BindUnorderedAccessViewCS
    VISION_APIFUNC void BindMeshTextureCS(VTextureObject *pTexture, int iSamplerStage, const VStateGroupSampler *pSampler);

    /// \brief
    ///   Binds a shader resource to a compute shader stage for compute shader execution inside a
    ///   BeginCompute/EndCompute block.
    /// 
    /// Unlike the BindMeshTextureCS method, this method does not take a sample state as input,
    /// and it can not only be used to bind textures, but also vertex or index buffers as shader
    /// resources.
    /// 
    /// Note that vertex or index buffers intended to be used as shader resources need to have been
    /// created with the appropriate bind and usage flags (VIS_BIND_SHADER_RESOURCE and 
    /// VIS_MEMUSAGE_UAV_BYTEADDRESS). 
    /// 
    /// Shader Resources that are set this way can be referenced by the shader as "BaseTexture" of
    /// the respective sampler stage.
    /// 
    /// This method is supported on PC DX11 only.
    /// 
    /// \param pResourceView
    ///   The shader resource view to bind
    /// 
    /// \param iStage
    ///   The stage to bind the resource to. Must be in the range [0..MAX_MESHBUFFER_TEXCOORDS-1]
    /// 
    /// \sa VisRenderLoopHelper_cl::RunComputeShader
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureCS
    /// \sa VisRenderLoopHelper_cl::BindUnorderedAccessViewCS
    VISION_APIFUNC void BindShaderResourceCS(VShaderResourceView *pResourceView, int iStage);

    /// \brief
    ///   Binds an unordered access view of a resource to a compute shader stage for compute shader execution inside a
    ///   BeginCompute/EndCompute block.
    /// 
    /// Note that vertex or index buffers intended to be used as unordered access resources need to have been
    /// created with the appropriate bind and usage flags (VIS_BIND_SHADER_RESOURCE and 
    /// VIS_MEMUSAGE_UNORDERED_ACCESS_BYTEADDRESS). 
    /// Textures intended to be used as unordered access resources need to
    /// have been created with the flag VTM_FLAG_UNORDEREDACCESSVIEW.
    /// 
    /// This method is supported on PC DX11 only.
    /// 
    /// \param pUAV
    ///   The unordered access view to bind
    /// 
    /// \param iStage
    ///   The stage to bind the resource to. Must be in the range [0..MAX_UAV_COUNT-1]
    /// 
    /// \sa VisRenderLoopHelper_cl::RunComputeShader
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureCS
    /// \sa VisRenderLoopHelper_cl::BindUnorderedAccessViewCS
    VISION_APIFUNC void BindUnorderedAccessViewCS(VUnorderedAccessView *pUAV, int iStage);

    /// \brief
    ///   Executes a compute shader.
    /// 
    /// Executes a compute shader, using the currently set shader resource views and unordered access views. the passed
    /// thread group counts correspond to the parameters of the Dispatch() function in DirectX11; please refer to the
    /// DirectX documentation for details.
    /// 
    /// This method is supported on PC DX11 only.
    /// 
    /// \param pShader
    ///   The compiled shader pass containing the compute shader to execute.
    /// 
    /// \param iThreadGroupCountX
    ///   The number of thread groups dispatched in the x direction. Must be less than 64000.
    /// 
    /// \param iThreadGroupCountY
    ///   The number of thread groups dispatched in the y direction. Must be less than 64000.
    /// 
    /// \param iThreadGroupCountZ
    ///   The number of thread groups dispatched in the z direction. Must be less than 64000.
    /// 
    /// \sa VisRenderLoopHelper_cl::RunComputeShader
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureCS
    /// \sa VisRenderLoopHelper_cl::BindUnorderedAccessViewCS
    VISION_APIFUNC void RunComputeShader(VCompiledShaderPass *pShader, int iThreadGroupCountX, int iThreadGroupCountY, int iThreadGroupCountZ);

    /// \brief
    ///   Executes a Compute Shader with the thread count values provided by a GPU buffer.
    /// 
    /// Executes a compute shader, using the currently set shader resource views and unordered access views. 
    /// The thread group counts are provided by a buffer instead of directly by this call. 
    /// This is allows a compute shader to be driven by a previous compute shader output.
    /// This function corresponds to the parameters of the DispatchIndirect() function 
    /// in DirectX11; please refer to the DirectX documentation for details.
    /// 
    /// \platforms
    ///   DX11
    /// 
    /// \param pShader
    ///   The compiled shader pass containing the compute shader to execute.
    /// 
    /// \param pThreadCountBuffer
    ///   Buffer containing 3 unsigned int counts. This can be filled by the CPU, by
    ///   a previous Compute Shader, or by a call to CopyStructureCount()
    ///
    /// \param uiArgAlignedByteOffset
    ///   DWORD-aligned byte office from beginning of thread count buffer where the
    ///   three counts (ThreadGroupCountX, iThreadGroupCountY, iThreadGroupCountZ) 
    ///   are to be read by the DispatchIndirect() call.
    /// 
    /// \sa VisRenderLoopHelper_cl::BindMeshTextureCS
    /// \sa VisRenderLoopHelper_cl::BindUnorderedAccessViewCS
    /// \sa VisRenderLoopHelper_cl::CopyStructureCount
    VISION_APIFUNC void RunComputeShaderIndirect(VCompiledShaderPass *pShader, VGpuBuffer *pThreadCountBuffer, unsigned int uiArgAlignedByteOffset=0);

    /// \brief
    ///   Copies the structure counts from a D3D11 Append/Consume Structured Buffer
    /// 
    /// When a Compute Shader is used to fill an AppendStructuredBuffer, this function
    /// is used to copy out the total count of added structures into another buffer
    /// so it can be used in a subsequent Compute Shader, or as the group count in
    /// a call to DispatchIndirect() (RunComputeShaderIndirect()).
    /// 
    /// \platforms
    ///   DX11
    /// 
    /// \param pDestBuffer
    ///   Buffer to receive count
    /// \param uiDestAlignedByteOffset
    ///   DWORD-aligned byte office from beginning of dest buffer to put the count.
    ///   This can be used to copy any ONE of the 3 counts needed to use 
    ///   DispatchIndirect() with the Dest buffer.
    /// \param pSrcUAV
    ///   UAV of the buffer that has structures added in a Compute Shader using the
    ///   AppendStructureBuffer::Append() HLSL function.
    /// 
    /// \sa VisRenderLoopHelper_cl::RunComputeShader
    /// \sa VisRenderLoopHelper_cl::RunComputeShaderIndirect
    VISION_APIFUNC void CopyStructureCount(VGpuBuffer *pDestBuffer, unsigned int uiDestAlignedByteOffset, VUnorderedAccessView *pSrcUAV);

    /// \brief
    ///   Sets the shader for matrix-based compute shader skinning
    /// 
    /// This shader is responsible for skinning animated models using matrices, and exporting
    /// them to memory using DX11's compute shader functionality.
    /// 
    /// All compute shader-based skinning operations will use this shader. By default, the VisionRenderLoop
    /// loads the default implementation of the shader and sets it using this method. It is
    /// normally not required to modify it or to use a different shader.
    /// 
    /// \param pDefaultShader
    ///   The desired shader.
    /// 
    /// \platforms
    ///   DX11
    ///
    /// \sa VisRenderLoopHelper_cl::GetMemexportMatrixSkinningShader
    /// \sa VisRenderLoopHelper_cl::SetMemexportQuaternionSkinningShader
    /// \sa VisAnimConfig_cl::SetSkinningMode
    inline void SetComputeMatrixSkinningShader(VCompiledShaderPass *pDefaultShader) { m_spComputeSkinningShaderMatrix = pDefaultShader; }

    /// \brief
    ///   Returns the shader previously set with SetComputeMatrixSkinningShader.
    /// 
    /// \return
    ///   VCompiledShaderPass *: The shader currently used for compute shader skinning.
    /// 
    /// \platforms
    ///   DX11
    ///
    /// \sa VisRenderLoopHelper_cl::SetComputeMatrixSkinningShader
    inline VCompiledShaderPass *GetComputeMatrixSkinningShader() const { return m_spComputeSkinningShaderMatrix; }

    ///
    /// @}
    ///

#endif

    ///
    /// @name Miscellaneous standard shaders
    /// @{
    ///

    /// \brief
    ///   Sets the shader effect used for non-standard clear screen operations (e.g. FP16/32
    ///   clears, MRT clears). Whether this effect is used or not depends on the respective
    ///   platform.
    VISION_APIFUNC void SetClearScreenEffect(VCompiledEffect *pClearEffect);

    /// \brief
    ///   Sets the shader effect used for non-standard clear screen operations (e.g. FP16/32
    ///   clears, MRT clears). Whether this effect is used or not depends on the respective
    ///   platform.
    inline VCompiledEffect *GetClearScreenEffect() { return m_spClearEffect; }

    /// \brief
    ///   Sets the shader effect used for non-standard clear screen operations (e.g. FP16/32
    ///   clears, MRT clears). Whether this effect is used or not depends on the respective
    ///   platform.
    VISION_APIFUNC VCompiledTechnique *GetClearScreenTechnique(int iNumRTs);

    /// \brief
    ///   Sets the default shader effect for particle groups
    /// 
    /// When using hardware spanning for particle groups, the vertex shader is responsible for
    /// computing the final vertex positions for each corner vertex of a particle. If no shaders
    /// are assigned to a particle group using hardware spanning, the default shader (set with this
    /// method) will be used.
    /// 
    /// By default, the engine loads the default implementation of the shader and sets it using
    /// this method. It is normally not required to modify it or to use a different shader.
    /// 
    /// \param pParticleEffect
    ///   The shader effect
    /// 
    /// \sa VisParticleGroup_cl::SetHardwareSpanning
    /// \sa VisRenderLoopHelper_cl::GetDefaultParticleShader
    VISION_APIFUNC void SetDefaultParticleEffect(VCompiledEffect *pParticleEffect);

    /// \brief
    ///   Returns one of the shaders in the effect previously set with SetDefaultParticleEffect.
    /// 
    /// \param iFlags
    ///   The shader type to obtain from the effect. The flags can be combined. Valid flags are:
    ///   \li PARTICLESHADERFLAGS_HARDWARESPANNING: The shader must provide hardware vertex spanning
    ///   \li PARTICLESHADERFLAGS_SMOOTHANIMATION: The shader uses animation and smooth transition between the frames.
    ///
    /// \return
    ///   VCompiledShaderPass *pShader: The particle shader, or NULL if the requested type is not available in the default particle effect.
    /// 
    /// \sa VisParticleGroup_cl::SetHardwareSpanning
    /// \sa VisRenderLoopHelper_cl::SetDefaultParticleShader
    inline VCompiledShaderPass *GetDefaultParticleShader(VisParticleGroup_cl::ParticleShaderFlags_e iFlags=VisParticleGroup_cl::PARTICLESHADERFLAGS_NONE) const 
    { 
      return m_spParticleShader[iFlags];  ///< direct lookup in the array
    }

    /// \brief
    ///   Sets up a shader technique config (inclusion/exclusion flags) that can be used to get the
    ///   appropriate particle shader technique
    /// 
    /// \param iFlags
    ///   Supported bitflags are:
    ///   \li PARTICLESHADERFLAGS_HARDWARESPANNING: The engine uses hardware particle spanning (not
    ///     supported on PC)
    ///   \li PARTICLESHADERFLAGS_SMOOTHANIMATION: The particle group uses a smooth fading between
    ///     animated frames
    /// 
    /// \param config
    ///   Out: The destination config. Inclusion and exclusion tags are applied here.
    VISION_APIFUNC void GetParticleEffectConfig(VisParticleGroup_cl::ParticleShaderFlags_e iFlags, VTechniqueConfig &config);

    /// \brief
    ///   Sets the start register for tracking per instance vertex shader constants. The default start register is 38 (see constant table layout in vForge). Only relevant for DX9-style vertex shader constant tables.
    VISION_APIFUNC void SetParticlePropertiesVSRegister(int iRegister=38);

    /// \brief
    ///   Returns the current start register for tracking per instance vertex shader constants. The default start register is 38 (see constant table layout in vForge)
    VISION_APIFUNC int GetParticlePropertiesVSRegister() const;

    /// \brief
    ///   Returns a shader pass instance that outputs plain diffuse color in the pixel shader
    VISION_APIFUNC VCompiledShaderPass *GetFullbrightShader();

    ///
    /// @}
    ///

    ///
    /// @name Vforge
    /// @{
    ///

    /// \brief
    ///   Sets a global renderloop that replaces the main renderloop
    /// 
    /// This feature is used by vForge to render the geometry with a custom shader effect (e.g.
    /// directional lighting).
    /// 
    /// vForge calls this function when the user selects a different shading mode in the view.
    /// 
    /// Custom main renderloop implementations have to take this setting into account and render
    /// with this renderloop and return immediately. See OnDoRenderLoop implementation of
    /// VisionRenderLoop_cl class.
    /// 
    /// \param pRenderLoop
    ///   Renderloop instance that should be used as a main renderloop replacement
    /// 
    /// \sa VisRenderLoopHelper_cl::GetReplacementRenderLoop
    /// \sa IVisShaderProvider_cl::GetReplacementEffect
    inline void SetReplacementRenderLoop(IVisRenderLoop_cl *pRenderLoop)
    {
      m_spReplacementRenderLoop = pRenderLoop;
    }

    /// \brief
    ///   Returns the replacement renderloop instance. This is NULL for normal rendering and
    ///   outside vForge
    inline IVisRenderLoop_cl *GetReplacementRenderLoop() const
    {
      return m_spReplacementRenderLoop;
    }

    ///
    /// @}
    ///

#if defined(_VR_DX11) || defined(_VISION_XENON) || defined(_VISION_WIIU)

    ///
    /// @name Memexport/stream Out Support (Xbox360, PC DX10, PC DX11)
    /// @{
    ///

    /// \brief
    ///   Sets the shader for quaternion-based memexport vertex shader skinning
    /// 
    /// This shader will be responsible for skinning animated models using quaternions, and
    /// exporting them to memory using Xbox360's memexport feature or DX10 Stream Out.
    /// 
    /// All memexport skinning operations will use this shader. By default, the VisionRenderLoop
    /// loads the default implementation of the shader and sets it using this method. It is
    /// normally not required to modify it or to use a different shader.
    /// 
    /// On PC DX10/DX11, this shader only handles export for position and normals. There is a separate
    /// shader for tangent skinning.
    /// 
    /// \param pDefaultShader
    ///   The desired memexport quaternion skinning shader.
    /// 
    /// \platforms
    ///   DX10, DX11, Xbox360
    ///
    /// \sa VisRenderLoopHelper_cl::GetMemexportQuaternionSkinningShader
    /// \sa VisRenderLoopHelper_cl::SetMemexportMatrixSkinningShader
    /// \sa VisAnimConfig_cl::SetSkinningMode
    inline void SetMemexportQuaternionSkinningShader(VCompiledShaderPass *pDefaultShader) { m_spMemexportSkinningShaderQuat = pDefaultShader; }

    /// \brief
    ///   Returns the shader previously set with SetMemexportQuaternionSkinningShader.
    /// 
    /// \return
    ///   VCompiledShaderPass *: The shader currently used for memexport quaternion skinning.
    /// 
    /// \platforms
    ///   DX10, DX11, Xbox360
    ///
    /// \sa VisRenderLoopHelper_cl::GetMemexportQuaternionSkinningShader
    inline VCompiledShaderPass *GetMemexportQuaternionSkinningShader() const { return m_spMemexportSkinningShaderQuat; }

    /// \brief
    ///   Sets the shader for matrix-based memexport vertex shader skinning
    /// 
    /// This shader will be responsible for skinning animated models using matrices, and exporting
    /// them to memory using Xbox360's memexport feature or DX10 stream out.
    /// 
    /// All memexport skinning operations will use this shader. By default, the VisionRenderLoop
    /// loads the default implementation of the shader and sets it using this method. It is
    /// normally not required to modify it or to use a different shader.
    /// 
    /// On PC DX10/DX11, this shader only handles export for position and normals. There is a separate
    /// shader for tangent skinning.
    /// 
    /// \param pDefaultShader
    ///   The desired memexport matrix skinning shader.
    /// 
    /// \platforms
    ///   DX10, DX11, Xbox360
    ///
    /// \sa VisRenderLoopHelper_cl::GetMemexportMatrixSkinningShader
    /// \sa VisRenderLoopHelper_cl::SetMemexportQuaternionSkinningShader
    /// \sa VisAnimConfig_cl::SetSkinningMode
    inline void SetMemexportMatrixSkinningShader(VCompiledShaderPass *pDefaultShader) { m_spMemexportSkinningShaderMatrix = pDefaultShader; }

    /// \brief
    ///   Returns the shader previously set with SetMemexportMatrixSkinningShader.
    /// 
    /// \return
    ///   VCompiledShaderPass *: The shader currently used for memexport matrix skinning.
    /// 
    /// \platforms
    ///   DX10, DX11, Xbox360
    ///
    /// \sa VisRenderLoopHelper_cl::SetMemexportMatrixSkinningShader
    inline VCompiledShaderPass *GetMemexportMatrixSkinningShader() const { return m_spMemexportSkinningShaderMatrix; }

    /// \brief
    ///   Begins a memexport operation
    /// 
    /// This method allows vertex shaders to write back to memory using the memexport microcode
    /// instruction of the Xbox360 GPU or the Stream Out feature of PC DX10/DX11.
    /// 
    /// You have to finish memexport operations using the EndMemExport method.
    /// 
    /// Only single-buffered vertex buffers can be written to. On PC DX10, they additionally have
    /// to have been created with the appropriate usage flag (VIS_MEMUSAGE_STREAM_OUTPUT).
    /// 
    /// For PC DX10/DX11, the following vertex outputs are assumed: iIndex==0 matches the shader output
    /// semantic POSITION, iIndex==1 the semantic NORMAL, iIndex==2 the shader output semantic
    /// TEXCOORD0, and iIndex==3 the shader output semantic TEXCOORD1. All outputs need to be of
    /// type float4 on PC DX10/DX11.
    /// 
    /// \param iIndex
    ///   Index of the memexport operation (0..3, allowing for up to four concurrent memexports)
    /// 
    /// \param pBuffer
    ///   Vertex buffer to be written to
    /// 
    /// \param iVertexCount
    ///   Total number of vertices to be written. Ignored for DX10/DX11.
    /// 
    /// \param iConstantIndex
    ///   Vertex shader constant in which to place memexport offset/address information (take the
    ///   memexport skinning shaders shipped with the Vision SDK as an example). Ignored for DX10/DX11.
    /// 
    /// \param iStride
    ///   Vertex stride within the target vertex buffer.
    /// 
    /// \param iOffset
    ///   Offset in the target vertex buffer.
    /// 
    /// \platforms
    ///   DX10, DX11, Xbox360
    ///
    /// \sa VisRenderLoopHelper_cl::EndMemExport
    VISION_APIFUNC void BeginMemExport(int iIndex, VVertexBuffer *pBuffer, int iVertexCount, int iConstantIndex, int iStride, int iOffset = 0);

    /// \brief
    ///   Ends a memexport operation previously started with BeginMemExport.
    ///

    /// A memexport operation has to be finished using this method before the vertex buffer being
    /// written to can be used in any rendering operations.
    /// 
    /// \param iIndex
    ///   Index of the memexport operation (0..3, allowing for up to four concurrent memexports)
    /// 
    /// \param pBuffer
    ///   vertex buffer to be written to
    /// 
    /// \platforms
    ///   DX10, DX11, Xbox360
    ///
    /// \sa VisRenderLoopHelper_cl::BeginMemExport
    VISION_APIFUNC void EndMemExport(int iIndex, VVertexBuffer *pBuffer);

#endif

#if defined(_VR_DX11)

    /// \brief
    ///   Begins a memexport operation (this version is supported only on PC DX10/DX11)
    /// 
    /// This method allows vertex shaders to write back to a vertex buffer using the Stream Out
    /// feature of PC DX10/DX11.
    /// 
    /// You have to explicitly finish memexport operations using the EndMemExport method.
    /// 
    /// Only single-buffered vertex buffers can be written to. They additionally have to have been
    /// created with the appropriate usage flag (VIS_MEMUSAGE_STREAM_OUTPUT).
    /// 
    /// As a general limitation of DX10, you have to take into account that you can either have one
    /// export target (i.e., one "index") with multiple components, or multiple export targets with
    /// only a single component.
    /// 
    /// In this version, the passed VisMBVertexDescriptor_t defines the structure of the data
    /// generated with this memexport operation.
    /// 
    /// \param iIndex
    ///   Index of the memexport operation (0..3, allowing for up to four concurrent memexports)
    /// 
    /// \param pBuffer
    ///   vertex buffer to be written to
    /// 
    /// \param vertexDescriptor
    ///   descriptor for the data generated in this memexport operation. Has to match the output
    ///   generated by the shader that performs the stream out operation.
    /// 
    /// \param iOffset
    ///   Offset in the target vertex buffer.
    /// 
    /// \sa VisRenderLoopHelper_cl::EndMemExport
    VISION_APIFUNC void BeginMemExport(int iIndex, VVertexBuffer *pBuffer, const VisMBVertexDescriptor_t &vertexDescriptor, int iOffset = 0);

    /// \brief
    ///   Ends a memexport operation previously started with BeginMemExport. (version for DX10/DX11 only)
    /// 
    /// A memexport operation has to be finished using this method before the vertex buffer being
    /// written to can be used in any rendering operations.
    /// 
    /// \param iIndex
    ///   Index of the memexport operation (0..3, allowing for up to four concurrent memexports)
    /// 
    /// \sa VisRenderLoopHelper_cl::BeginMemExport
    VISION_APIFUNC void EndMemExport(int iIndex);


#endif

    ///
    /// @}
    ///

    ///
    /// @name Platform-specific Shaders
    /// @{
    ///

    // internal
    VISION_APIFUNC void FlushParticleBuffer();


    ///
    /// @}
    ///

    ///
    /// @name Rendering Static Geometry Instances
    /// @{
    ///

    /// \brief
    ///   Renders a collection of static geometry instances with the passed shader.
    /// 
    /// For best performance, the order of the static geometry instances in the collection should
    /// be so that render state changes are minimized. The Vision Engine will automatically take
    /// care of minimizing redundant state changes, but does not resort geometry instances on the
    /// fly.
    /// 
    /// Most of all, it should be taken care that VisStaticSubmeshInstance_cl objects are not
    /// interspersed with other static geometry instance types.
    /// 
    /// \param geoInstances
    ///   Collection of static geometry instances to render.
    /// 
    /// \param Shader
    ///   Shader with which to render the static geometry instances.
    /// 
    /// \sa VisRenderLoopHelper_cl::RenderStaticGeometrySurfaceShaders
    VISION_APIFUNC void RenderStaticGeometryWithShader(const VisStaticGeometryInstanceCollection_cl &geoInstances, VCompiledShaderPass &Shader);

    /// \brief
    ///   Renders a collection of static geometry instances with the shaders assigned to their
    ///   respective surfaces.
    /// 
    /// For best performance, the order of the static geometry instances in the collection should
    /// be so that render state changes are minimized. The Vision Engine will automatically take
    /// care of minimizing redundant state changes, but does not resort geometry instances on the
    /// fly.
    /// 
    /// Most of all, it should be taken care that VisStaticSubmeshInstance_cl objects are not
    /// interspersed with other static geometry instance types.
    /// 
    /// \param geoInstances
    ///   Collection of static geometry instances to render.
    /// 
    /// \param ePassType
    ///   The pass type to render
    /// 
    /// \param iTagFilter
    ///   If iTagFilter==VTF_IGNORE_TAGGED_ENTRIES, tagged entries inside the specified collection will not be rendered.
    /// 
    /// \sa VisRenderLoopHelper_cl::RenderStaticGeometryWithShader
    VISION_APIFUNC void RenderStaticGeometrySurfaceShaders(const VisStaticGeometryInstanceCollection_cl &geoInstances, VPassType_e ePassType, VTagFilter_e iTagFilter=VTF_IGNORE_NONE);

    /// \brief
    ///   Returns all static geometry instances affected by a specific light source
    /// 
    /// taking the radius of influence of the light source into account.
    /// 
    /// \param lightSource
    ///   Reference to the light source.
    /// 
    /// \param geoInstances
    ///   Collection of static geometry instances which will be filled with the geometry instances
    ///   potentially affected by the light source.
    /// 
    /// \param bSpecularOnly
    ///   If TRUE, only geometry instances with the specular multiplier set to >0 will be returned.
    ///   FALSE by default.
    /// 
    /// \param bVisibleOnly
    ///   If TRUE, only geometry instances which have successfully passed the visibility test in
    ///   the visibility collector of this render context are returned. FALSE by default.
    VISION_APIFUNC void GetStaticGeometryAffectedByStaticLight(VisLightSource_cl &lightSource, VisStaticGeometryInstanceCollection_cl &geoInstances, BOOL bSpecularOnly=FALSE, BOOL bVisibleOnly=TRUE);

    /// \brief
    ///   Helper function computing the distances of a point to all planes of a frustum and storing
    ///   these values in a array of floats.
    VISION_APIFUNC void ComputeLightFrustumDistances(const hkvVec3& vLightPos, const VisFrustum_cl *pViewFrustum, float *fLightFrustumDistances);

    /// \brief
    ///   Helper function comparing previously computed frustum distance against the frustum
    ///   distance of a bounding box. Used e.g. to check whether an object can possibly cast a
    ///   shadow into the visible area.
    VISION_APIFUNC bool CompareLightFrustumDistances(const hkvAlignedBBox &bbox, const VisFrustum_cl *pViewFrustum, float *fLightFrustumDistances);


    #ifdef WIN32
    
    /// \brief
    ///   Temporarily replaces the render targets that are used on graphics API level. Should not be used.
    VISION_APIFUNC void SetRenderTargetOverride(int iRTCount, VisRenderableTexture_cl **pRT, VisRenderableTexture_cl *pDS);

    #endif

    ///
    /// @}
    ///

  private:

    friend void Vision_GL_RenderWorld();
    friend class VisMain_cl;


    void Init();
    void DeInit();
   
    inline bool SynchronizeAnimation(VisAnimConfig_cl *pAnimConfig);
    int GetIdenticalMeshes(const VisEntityCollection_cl &EntityCollection, int iFirstEntity);
    void SetupDynamicMeshStreams(VDynamicMesh *pMesh, VisAnimConfig_cl *pAnimConfig, const VCompiledShaderPass *pShaderPass, bool bSetupBoneWeightStream, VDynamicMeshStreamState &streamState);
    void SetEntitySurfaceShaderProperties(const VisBaseEntity_cl *pEntity, const VBaseSubmesh *pSubmesh, const VisSurface_cl *pSurface, const VCompiledShaderPass *pShaderPass);
    void SetMeshSurfaceShaderProperties(const VDynamicMesh *pMesh, const VBaseSubmesh *pSubmesh, const VisSurface_cl *pSurface, const VCompiledShaderPass *pShaderPass);
    inline VisSurfaceTextures_cl *GetSurfaceTextures(VBaseSubmesh *pSubmesh, const VisSurfaceTextureSet_cl *pCustomTextureSet);


 
    int m_iCurrentVisQueryHandle;

    VLightGridPtr m_spLightGrid;
    VCompiledShaderPassPtr m_spParticleShader[VisParticleGroup_cl::PARTICLESHADERFLAGS_ALLFLAGS+1];  
    VCompiledShaderPassPtr m_spMemexportSkinningShaderMatrix;
    VCompiledShaderPassPtr m_spMemexportSkinningShaderQuat;
    VCompiledShaderPassPtr m_spComputeSkinningShaderMatrix;

    VCompiledEffectPtr m_spClearEffect;

    IVisRenderLoopPtr m_spReplacementRenderLoop;

    int m_iMeshRenderCtr;
    int m_iComputeCtr;
    int m_iOverlayCtr;

    VDynamicMeshStreamState m_MeshStreamState;
    bool m_bIsInEntityRenderingBracket;


public:
    bool m_bHasLightmaps;
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
