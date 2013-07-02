/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/Engine/Renderer/Shader/VisionMobileShaderProvider.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VMobileShadowMapComponentSpotDirectional.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenSpotDir.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderLoop.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VLightClippingVolumeComponent.hpp>


VMobileForwardRenderLoop::VMobileForwardRenderLoop(VMobileForwardRenderingSystem *pRenderer): 
  VisionRenderLoop_cl(),
  m_DynamicLightCollection(64, 64),
  m_AdditiveLitGeoInstanceCollection(4096, 2048),
  m_AdditiveLitEntityCollection(2048, 1024)
{
  m_pRenderer = pRenderer;
  m_pBasePassLight = NULL;
  m_iBasePassLightPriority = 0;
}

void VMobileForwardRenderLoop::InitRenderLoop()
{
  VisionRenderLoop_cl::InitRenderLoop();

  VLightClippingVolumeRenderer::PatchReadDepthStencilState(m_dynLightDefaultState, m_lightClippingStencilStatesRead);
  for (unsigned int i=0;i<2;i++)
    m_lightClippingStencilStatesRead[i].ComputeHash();

  m_lightClippingVolumeRenderer.Init();
}

void VMobileForwardRenderLoop::DeInitRenderLoop()
{
  VisionRenderLoop_cl::DeInitRenderLoop();

  m_lightClippingVolumeRenderer.DeInit();
}

IVShadowMapComponent* VMobileForwardRenderLoop::GetCompatibleShadowMapComponent(VisLightSource_cl *pLight)
{
#ifdef SUPPORTS_SHADOW_MAPS
  return vdynamic_cast<VMobileShadowMapComponentSpotDirectional*>(IVShadowMapComponent::GetShadowMapComponent(pLight, m_pRenderer));
#else
  return NULL;
#endif
}

int VMobileForwardRenderLoop::GetLightInfluenceArea(VisLightSource_cl *pLight)
{
  VASSERT(pLight != NULL);

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  int iScreenWidth,iScreenHeight;
  pContext->GetSize(iScreenWidth, iScreenHeight);
  if (pLight->GetType() == VIS_LIGHT_DIRECTED)
  {
    // directional lights influence the whole screen
    return (iScreenWidth*iScreenHeight); 
  }

  hkvMat4 projMatrix = pContext->GetViewProperties()->getProjectionMatrix(hkvClipSpaceYRange::MinusOneToOne);
  hkvMat4 viewMatrix = pContext->GetCamera()->GetWorldToCameraTransformation();

  // get position/ radius of bounding sphere
  hkvVec3 vPosition;
  float fRadius = 0.0f;
  if (pLight->GetType() == VIS_LIGHT_POINT)
  {
    vPosition = pLight->GetPosition();
    fRadius = pLight->GetRadius();
  }
  else if (pLight->GetType() == VIS_LIGHT_SPOTLIGHT)
  {
    hkvAlignedBBox bBox;
    pLight->GetBoundingBox(bBox);
    vPosition = bBox.getBoundingSphere().m_vCenter;
    fRadius = bBox.getBoundingSphere().m_fRadius;
  }
  else
    VASSERT_MSG(false, "Unsupported light type"); 
  
  // get corners of bounding rectangle in view space
  hkvVec4 vPositionVS = viewMatrix*vPosition.getAsVec4(1.0f);
  hkvVec4 vCorners[4];
  vCorners[0] = vPositionVS+hkvVec4(-fRadius, -fRadius, 0.0f, 0.0f);
  vCorners[1] = vPositionVS+hkvVec4(fRadius, -fRadius, 0.0f, 0.0f);
  vCorners[2] = vPositionVS+hkvVec4(fRadius, fRadius, 0.0f, 0.0f);
  vCorners[3] = vPositionVS+hkvVec4(-fRadius, fRadius, 0.0f, 0.0f); 

  // get corners of bounding rectangle in normalized device coordinates
  for (int i=0;i<4;i++)
  {
    vCorners[i] = projMatrix*vCorners[i];
    vCorners[i] /= vCorners[i].w;
  }

  // clip corners of bounding rectangle
  hkvVec2 vMin(vCorners[0].x, vCorners[0].y); 
  vMin.clampTo(hkvVec2(-1.0f, -1.0f), hkvVec2(1.0f, 1.0f));
  hkvVec2 vMax(vCorners[2].x, vCorners[2].y); 
  vMax.clampTo(hkvVec2(-1.0f, -1.0f), hkvVec2(1.0f, 1.0f));

  // calculate influence area 
  int iWidth = (int)((vMax.x-vMin.x)*0.5f*iScreenWidth);
  int iHeight = (int)((vMax.y-vMin.y)*0.5f*iScreenHeight);
  return (iWidth*iHeight);
}

int VMobileForwardRenderLoop::GetLightPriority(VisLightSource_cl *pLight)
{
  int iLightPriority = 0;
  if (pLight->IsDynamic())
  {
    iLightPriority = GetLightInfluenceArea(pLight);

    // lights with attached shadow map component have higher priority
    if (GetCompatibleShadowMapComponent(pLight))
      iLightPriority *= 2;
  }

  // static lights with attached shadow map component (subtractive shadows) have highest priority
  else 
  {
    VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
    int iScreenWidth,iScreenHeight;
    pContext->GetSize(iScreenWidth, iScreenHeight);
    iLightPriority = iScreenWidth*iScreenHeight*3;
  }

  return iLightPriority;
}

void VMobileForwardRenderLoop::DetermineRelevantLights()
{
  m_DynamicLightCollection.Clear();
  m_pBasePassLight = NULL;
  m_iBasePassLightPriority = 0;

  // Get all visible light sources
  IVisVisibilityCollector_cl *pVisColl = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  if (pVisColl == NULL)
    return;
  const VisLightSrcCollection_cl *pLightSourceCollection = pVisColl->GetVisibleLights();
  if (pLightSourceCollection == NULL)
    return;

  unsigned int iNumLights = pLightSourceCollection->GetNumEntries();
  if (iNumLights == 0)
    return;
  
  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  const hkvVec3 &vCamPos = pContext->GetCamera()->GetPosition();

  for (unsigned i=0;i<iNumLights;i++)
  {
    VisLightSource_cl *pLight = pLightSourceCollection->GetEntry(i);

    // We are only interested in dynamic lights or static lights with attached shadow map component
    if ((!pLight->IsDynamic() && !GetCompatibleShadowMapComponent(pLight)) || pLight->GetRadius()<=HKVMATH_LARGE_EPSILON)  
      continue;

    const float fFade = pLight->GetMultiplier()*pLight->GetFadeWeight(vCamPos);
    if (fFade <= HKVMATH_LARGE_EPSILON)
      continue;

    // See which geometry types have to cast shadows
    int iReceiverFlags = GetLightReceiverFlags(pLight);

    // If nothing receives light from this light source, we can proceed to the next light.
    if (!iReceiverFlags)
      continue;

    // Find light with highest priority. This light will be rendered in the base pass, in contrast to all 
    // additional lights that are rendered additively after the base pass. The search ignores lights with 
    // attached light clipping volume, since light clipping volumes can't be rendered before the base pass.
    if (!pLight->HasClipVolumeComponent())
    {
      // get the light with highest priority (largest influence area in screen space combined with weighting factor)
      int iLightPriority = GetLightPriority(pLight);
      if (iLightPriority > m_iBasePassLightPriority)
      {
        m_pBasePassLight = pLight;
        m_iBasePassLightPriority = iLightPriority;
      }
    } 
   
    if (pLight->IsDynamic())
      m_DynamicLightCollection.AppendEntry(pLight); 
  }
}


void VMobileForwardRenderLoop::OnDoRenderLoop(void *pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VMobileForwardRenderLoop::OnDoRenderLoop");

  m_iFrameCounter++; // just for arbitrary custom purposes

#ifdef WIN32
  // vForge specific:
  if (Vision::RenderLoopHelper.GetReplacementRenderLoop())
  {
    // render with this render-loop instead
    Vision::RenderLoopHelper.GetReplacementRenderLoop()->OnDoRenderLoop(pUserData);
    return;
  }
#endif

  m_pShaderProvider = Vision::GetApplication()->GetShaderProvider();
  VASSERT(m_pShaderProvider);
  m_pShaderProvider->ResetCache();

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  IVisVisibilityCollector_cl *pVisCollector = pContext->GetVisibilityCollector();
  if (pVisCollector==NULL)
    return; 

  const int iRenderFlags = pContext->GetRenderFlags();
  m_pCameraFrustum = pVisCollector->GetBaseFrustum(); 

  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesPrimaryOpaquePass = pVisCollector->GetVisibleStaticGeometryInstancesForPass(VPT_PrimaryOpaquePass);
  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesSecondaryOpaquePass = pVisCollector->GetVisibleStaticGeometryInstancesForPass(VPT_SecondaryOpaquePass);
  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesTransparentPass = pVisCollector->GetVisibleStaticGeometryInstancesForPass(VPT_TransparentPass);
  const VisEntityCollection_cl *pVisibleEntitiesPrimaryOpaquePass = pVisCollector->GetVisibleEntitiesForPass(VPT_PrimaryOpaquePass);
  const VisEntityCollection_cl *pVisibleEntitiesSecondaryOpaquePass = pVisCollector->GetVisibleEntitiesForPass(VPT_SecondaryOpaquePass);
  const VisEntityCollection_cl *pVisibleEntitiesTransparentPass = pVisCollector->GetVisibleEntitiesForPass(VPT_TransparentPass);
  const VisEntityCollection_cl *pVisibleForeGroundEntities = pVisCollector->GetVisibleForeGroundEntities();
  HandleVisibleVisibilityObjects();

  // Clear the screen
  if ((iRenderFlags&VIS_RENDERCONTEXT_FLAG_NO_CLEARSCREEN)==0)
  {
    const VFogParameters &fog = Vision::World.GetFogParameters();
    VColorRef clearColor = fog.depthMode != VFogParameters::Off ? fog.iDepthColor : Vision::Renderer.GetDefaultClearColor();
    Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, clearColor);
  }

  m_bHasRenderHookCallbacks = m_bTriggerCallbacks && Vision::Callbacks.OnRenderHook.HasCallbacks();

  // Get a pointer to the collection of visible mesh buffer objects
  const VisMeshBufferObjectCollection_cl *pVisibleMeshBuffer = &m_VisibilityObjectCollector.GetMeshBufferObjectCollection();

  // Get a pointer to the collection of visible particle groups
  const VisParticleGroupCollection_cl *pVisibleParticleGroups = &m_VisibilityObjectCollector.GetParticleGroupCollection();

  // Determine which lights have to rendered in the current frame
  DetermineRelevantLights();

  // Render all mesh buffer objects with the render order flag "VRH_PRE_RENDERING".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_RENDERING, m_bTriggerCallbacks);

  // Render all mesh buffer objects with the render order flag "VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY, m_bTriggerCallbacks);

  // Reset tags
  VisStaticGeometryInstance_cl::ResetTags();
  VisBaseEntity_cl::ResetTags();
  
  // Clear temporary collections for geometry that is lit by base pass light, but rendered in additive lighting pass
  m_AdditiveLitGeoInstanceCollection.Clear();
  m_AdditiveLitEntityCollection.Clear();

  // Prepare the initial lighting pass (one light collapsed with base lighting contribution)
  bool bUsesLightClippingVolume = false;
  IVShadowMapComponent *pShadowMap = PrepareLightingPass(m_pBasePassLight, true, bUsesLightClippingVolume);

  // Render lit geometry before actual base pass, whereby the geometry which has been rendered here will be tagged, in order
  // to avoid re-rendering later on. We first render static meshes lit base the base pass light, then static meshes not lit by the base pass light,
  // and then entities (with/without base pass light, respectively).
  {
    RenderLitGeometry(m_pBasePassLight, pShadowMap, true, bUsesLightClippingVolume, false, true);

    // Render all primary opaque pass surface shaders on opaque world geometry
    Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(*pVisibleGeoInstancesPrimaryOpaquePass, VPT_PrimaryOpaquePass, VTF_IGNORE_TAGGED_ENTRIES); 

    // Render all mesh buffer objects with the render order flag "VRH_PRE_PRIMARY_OPAQUE_PASS_ENTITIES".
    RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_PRIMARY_OPAQUE_PASS_ENTITIES, m_bTriggerCallbacks);

    RenderLitGeometry(m_pBasePassLight, pShadowMap, true, bUsesLightClippingVolume, true, false);

    // Render all primary opaque pass shaders on entities (see "DrawEntitiesShaders")
    DrawEntitiesShaders(*pVisibleEntitiesPrimaryOpaquePass, VPT_PrimaryOpaquePass, VTF_IGNORE_TAGGED_ENTRIES);
  }

  // Finalize the initial pass
  FinalizeLightingPass(m_pBasePassLight, bUsesLightClippingVolume);

  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_SECONDARY_OPAQUE_PASS_GEOMETRY, m_bTriggerCallbacks);

  // Render static geometry instances for secondary opaque pass
  Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(*pVisibleGeoInstancesSecondaryOpaquePass, VPT_SecondaryOpaquePass, VTF_IGNORE_TAGGED_ENTRIES);
  
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_SECONDARY_OPAQUE_PASS_ENTITIES, m_bTriggerCallbacks);

  // Render entities for secondary opaque pass
  DrawEntitiesShaders(*pVisibleEntitiesSecondaryOpaquePass, VPT_SecondaryOpaquePass, VTF_IGNORE_TAGGED_ENTRIES);

  // Start the hardware occlusion query. Note that this function always has to be called in render loops.
  // Also, the position of this call in the OnDoRenderLoop is important: The zBuffer contents at this stage of rendering will
  // act as occluders in the hardware occlusion queries.
  Vision::RenderLoopHelper.PerformHardwareOcclusionQuery();

  // Render sky
  Vision::RenderLoopHelper.RenderSky();

  // Render all mesh buffer objects with the render order flag "VRH_PRE_OCCLUSION_TESTS".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_OCCLUSION_TESTS, m_bTriggerCallbacks);

  Vision::RenderLoopHelper.PerformHardwarePixelCounterQuery();

  // Render all mesh buffer objects with the render order flag "VRH_POST_OCCLUSION_TESTS".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_POST_OCCLUSION_TESTS, m_bTriggerCallbacks);

  // Draw dynamic light 
  DrawDynamicLight();

  // Render all mesh buffer objects with the render order flag "VRH_PRE_TRANSPARENT_PASS_GEOMETRY".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_TRANSPARENT_PASS_GEOMETRY, m_bTriggerCallbacks); 
  
  // Render transparent pass surface shaders on translucent lit world primitives
  Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(*pVisibleGeoInstancesTransparentPass, VPT_TransparentPass, VTF_IGNORE_TAGGED_ENTRIES);

  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_TRANSPARENT_PASS_ENTITIES, m_bTriggerCallbacks);

  // Render transparent pass shaders on entities
  DrawEntitiesShaders(*pVisibleEntitiesTransparentPass, VPT_TransparentPass, VTF_IGNORE_TAGGED_ENTRIES);

  // Render all mesh buffer objects with the render order flag "VRH_POST_TRANSPARENT_PASS_GEOMETRY".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_POST_TRANSPARENT_PASS_GEOMETRY, m_bTriggerCallbacks);

  // Render all mesh buffer objects and particle systems with the render order flag "VRH_DECALS".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_DECALS, m_bTriggerCallbacks);

  // Render all mesh buffer objects and particle systems with the render order flag "VRH_PARTICLES".
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PARTICLES, m_bTriggerCallbacks);

  // Render all mesh buffer objects with the render order flag "VRH_ADDITIVE_PARTICLES"
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_ADDITIVE_PARTICLES, m_bTriggerCallbacks);

  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_TRANSLUCENT_VOLUMES, m_bTriggerCallbacks);

  // Render visible foreground entities (see DrawForegroundEntities)
  DrawForegroundEntities(*pVisibleForeGroundEntities);

  // Render all mesh buffer objects with the render order flag "VRH_CORONAS_AND_FLARES"
  RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_CORONAS_AND_FLARES, m_bTriggerCallbacks);

  m_pShaderProvider = NULL;
}

void VMobileForwardRenderLoop::DrawDynamicLight()
{
  INSERT_PERF_MARKER_SCOPE("ForwardRenderer::DrawDynamicLight");

  unsigned int iNumLights = m_DynamicLightCollection.GetNumEntries();
  for (unsigned int i=0; i<iNumLights; i++)
  {
    VisLightSource_cl *pLight = m_DynamicLightCollection.GetEntry(i);
    bool bUsesLightClippingVolume = false;
    IVShadowMapComponent *pShadowMapComponent = PrepareLightingPass(pLight, false, bUsesLightClippingVolume);
    RenderLitGeometry(pLight, pShadowMapComponent, false, bUsesLightClippingVolume, true, true);
    FinalizeLightingPass(pLight, bUsesLightClippingVolume);
  }
}

IVShadowMapComponent *VMobileForwardRenderLoop::PrepareLightingPass(VisLightSource_cl *pLight, bool bBasePass, bool &bUsesLightClippingVolume)
{
  bUsesLightClippingVolume = false;

  if (!pLight)
    return NULL;

  // Render the light clipping volume only in additive mode, since it can't be rendered before the base pass.
  if (!bBasePass)
    bUsesLightClippingVolume = m_lightClippingVolumeRenderer.RenderLightClippingVolume(pLight);

  // Render shadow map
  IVShadowMapComponent *pShadowMapComponent = GetCompatibleShadowMapComponent(pLight);
#ifdef SUPPORTS_SHADOW_MAPS
  if (pShadowMapComponent)
  {
    pShadowMapComponent->Render(bUsesLightClippingVolume, false);
    return pShadowMapComponent;
  }
#endif

  return NULL;
}

void VMobileForwardRenderLoop::FinalizeLightingPass(VisLightSource_cl *pLight, bool bUsesLightClippingVolume)
{
  if (bUsesLightClippingVolume)
    m_lightClippingVolumeRenderer.ClearLightClippingVolume(pLight);
}


void VMobileForwardRenderLoop::RenderLitGeometry(VisLightSource_cl *pLight, IVShadowMapComponent *pShadowMapComponent, bool bBasePass, bool bUsesLightClippingVolume, bool bEntities, bool bStaticGeometry)
{
  if (!pLight)
    return;

  // Some local variables for storing surfaces, shaders, surface shaders, and the like.
  VCompiledTechnique *pTechnique = NULL;
  VisDrawCallInfo_t SurfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  const hkvVec3 &vCamPos = pContext->GetCamera()->GetPosition();
 
  const float fFade = pLight->GetFadeWeight(vCamPos);

  VisStaticGeometryInstanceCollection_cl *pLitGeoInstanceCollection = NULL;
  VisEntityCollection_cl *pLitEntityCollection = NULL;

  if (bBasePass || pLight != m_pBasePassLight)
  {
    if (bStaticGeometry)
    {
      pLitGeoInstanceCollection = &s_LitGeoInstanceCollection;
      pLitGeoInstanceCollection->Clear();
    }
    if (bEntities)
    {
      pLitEntityCollection = &s_LitEntityCollection;
      pLitEntityCollection->Clear();
    }

    Vision::RenderLoopHelper.GetVisibleGeometryInLightsourceRange(pLitGeoInstanceCollection, pLitEntityCollection, NULL, *pLight);
  }
  else
  {
    if (bStaticGeometry)
      pLitGeoInstanceCollection = &m_AdditiveLitGeoInstanceCollection;
    if (bEntities)
      pLitEntityCollection = &m_AdditiveLitEntityCollection;
  }

#ifdef SUPPORTS_SHADOW_MAPS
  VShadowMapGenSpotDir *pShadowMapGenDir = NULL;
  if (pShadowMapComponent)
  {
    VShadowMapGenerator *pShadowMapGen = pShadowMapComponent->GetShadowMapGenerator();
    if (pShadowMapGen->GetProjectionType()==SHADOW_PROJECTION_ORTHOGRAPHIC)
      pShadowMapGenDir = static_cast<VShadowMapGenSpotDir*>(pShadowMapGen);
  }
#endif

  // Set the stencil render state for reading light clipping volume information
  if(bUsesLightClippingVolume)
  {
    const VLightClippingVolumeComponent* pLightClippingComponent = pLight->Components().GetComponentOfBaseType<VLightClippingVolumeComponent>();
    VASSERT(pLightClippingComponent != NULL && V_ARRAY_SIZE(m_lightClippingStencilStatesRead)==2);
    VisRenderStates_cl::SetDepthStencilState(m_lightClippingStencilStatesRead[pLightClippingComponent->GetClipHandedness()]);
  }
  else
    VisRenderStates_cl::SetDepthStencilState(m_dynLightDefaultState);

  // For all illuminated entities: Render a dynamic lighting pass now.
  if (pLight->GetLightInfluenceBitMaskEntity() != 0  && bEntities)
  {
    int iNumLitEntities = pLitEntityCollection->GetNumEntries();

    Vision::RenderLoopHelper.BeginEntityRendering();

    for (int j=0; j<iNumLitEntities; j++)
    {
      VisBaseEntity_cl *pEntity = pLitEntityCollection->GetEntry(j);

      if (!(pEntity->GetLightInfluenceBitMask() & pLight->GetLightInfluenceBitMaskEntity()))
        continue;

      VDynamicMesh *pMesh = pEntity->GetMesh();
      VisSurface_cl **ppSurfaces = pEntity->GetSurfaceArray();

      // Get list of all the surfaces in the model
      int iNumSubmeshes = pMesh->GetSubmeshCount();
      int iNumSurfaceShaders = 0;

      // For all the surfaces...
      for (int k=0; k<iNumSubmeshes; k++)
      {
        VDynamicSubmesh *pSubmesh = pMesh->GetSubmesh(k);
        VisSurface_cl *pSurface = ppSurfaces[pSubmesh->m_iMaterialIndex];

        // Full-bright surfaces can't be rendered in the compound base pass, since such surfaces are not illuminated. Since tagging
        // of already rendered geometry happens per entity instance, in case an entity contains full-bright surfaces, all surfaces 
        // of this entity have to be rendered in the normal rendering pipeline.
        const VisLightingMethod_e eLightingMethod = pSurface->GetLightingMode();
        if (bBasePass)
        {
          // Since entities can contain several surfaces with different lighting methods (full-bright, dynamic only, etc.), entities 
          // with full-bright surfaces have to be also added to the additive lit entity collection, in order to ensure rendering of 
          // dynamic only surfaces.
          if (eLightingMethod==VIS_LIGHTING_FULLBRIGHT)
          {
            iNumSurfaceShaders = 0;
            m_AdditiveLitEntityCollection.AppendEntry(pEntity);
            break; 
          }
        }
        else
        {
          if (eLightingMethod == VIS_LIGHTING_FULLBRIGHT)
            continue;
        }

        // If not all surfaces have a primary opaque pass type in the base pass, then render corresponding entity 
        // in the additive lighting pass.
        if (bBasePass && pSurface->GetResolvedPassType()!=VPT_PrimaryOpaquePass)
        {
          iNumSurfaceShaders = 0;
          m_AdditiveLitEntityCollection.AppendEntry(pEntity);
          break; 
        }

        // Check whether entity is in current shadow volume for orthographic shadows. In that case the entity is rendered
        // without shadows since it is not in the relevant shadow volume.
        IVShadowMapComponent *pTmpShadowMapComponent = pShadowMapComponent;
#ifdef SUPPORTS_SHADOW_MAPS
        if (pShadowMapGenDir)
        {
          if (!pShadowMapGenDir->IsEntityInsideOrthoShadowVolume(pEntity))
            pTmpShadowMapComponent = NULL;
        }
#endif

        pTechnique = GetLightShader(pLight, bBasePass, pSurface, fFade, pTmpShadowMapComponent);
        if (!pTechnique || !pTechnique->GetShaderCount()) // do not light this surface
        {
          // If base-pass lighting technique could not be retrieved, render lit entity in the additive pass.
          if (bBasePass && pLight->IsDynamic())
            m_AdditiveLitEntityCollection.AppendEntry(pEntity);
          iNumSurfaceShaders = 0;
          break; 
        }

        // Generate a list of surface shader from the combined surface/shader information
        VisDrawCallInfo_t &info(SurfaceShaderList[iNumSurfaceShaders++]);
        info.Set(pSubmesh, pSurface, pTechnique->m_Shaders.GetAt(0));
      }

      // Finally, render the entity with a surface shader list.
      if (iNumSurfaceShaders>0)
      {
        Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, SurfaceShaderList);
        if (bBasePass)
          pEntity->Tag();
      }
    }

    Vision::RenderLoopHelper.EndEntityRendering();
  }

  // For all illuminated world primitives: Render a dynamic lighting pass now.
  if (pLight->GetLightInfluenceBitMaskWorld() != 0 && bStaticGeometry)
  {
    VisSurface_cl *pSurface;
    VisSurface_cl *pLastSurface = NULL;
    VCompiledTechnique *pLastTechnique = NULL;

    // We start collecting illuminated mesh instances. Whenever a relevant property changes, we set the
    // shader information, render all collected world instances, and start collecting from scratch.
    int iNumLitGeoInstances = pLitGeoInstanceCollection->GetNumEntries();
    pLastSurface = NULL;
    s_RenderGeoInstanceCollection.Clear();
    pLastTechnique = NULL;

    for (int j=0; j<iNumLitGeoInstances; j++)
    {
      VisStaticGeometryInstance_cl *pGI = pLitGeoInstanceCollection->GetEntry(j);

      pSurface = pGI->GetSurface();
      VASSERT(pSurface);
      if (pSurface->IsFullbright()) 
        continue; 

      // If surface does not have a primary opaque pass type in the base pass, then render corresponding static
      // mesh instance in the additive lighting pass.
      if (bBasePass && pSurface->GetResolvedPassType()!=VPT_PrimaryOpaquePass)
      {
        m_AdditiveLitGeoInstanceCollection.AppendEntry(pGI);
        continue;
      }

      // Check whether mesh is in current shadow volume for orthographic shadows. In that case the mesh is rendered
      // without shadows since it is not in the relevant shadow volume.
      IVShadowMapComponent *pTmpShadowMapComponent = pShadowMapComponent;
#ifdef SUPPORTS_SHADOW_MAPS
      if (pShadowMapGenDir)
      {
        if (!pShadowMapGenDir->IsMeshInsideOrthoShadowVolume(pGI))
          pTmpShadowMapComponent = NULL;
      }
#endif

      if (pLastSurface!=pSurface)
      { 
        pTechnique = GetLightShader(pLight, bBasePass, pSurface, fFade, pTmpShadowMapComponent);          
        pLastSurface = pSurface;
      }
      if (pTechnique == NULL || !pTechnique->GetShaderCount())
      {
        // If base-pass lighting technique could not be retrieved, render lit mesh in the additive pass.
        if (bBasePass && pLight->IsDynamic())
          m_AdditiveLitGeoInstanceCollection.AppendEntry(pGI);
        continue; 
      }

      // If the state information is different from the previous one, we have to render the world primitives we
      // have collected so far
      if (pLastTechnique!=pTechnique)
      {
        if (s_RenderGeoInstanceCollection.GetNumEntries()!=0)
        {
          VASSERT(pLastTechnique != NULL);
          Vision::RenderLoopHelper.RenderStaticGeometryWithShader(s_RenderGeoInstanceCollection, *pLastTechnique->m_Shaders.GetAt(0));
          if (bBasePass)
            s_RenderGeoInstanceCollection.TagEntries();
          s_RenderGeoInstanceCollection.Clear();      
        }
        // Update the stored state information
        pLastTechnique = pTechnique;
      }
      // Naturally, we have to append the primitive to our collection (otherwise it won't be collected =) ).
      s_RenderGeoInstanceCollection.AppendEntry(pGI);
    }

    // If there's still something left in the collection, render it as well.
    if (s_RenderGeoInstanceCollection.GetNumEntries()!=0)
    {
      if (pLastTechnique && pLastTechnique->GetShaderCount()>0)
      {
        Vision::RenderLoopHelper.RenderStaticGeometryWithShader(s_RenderGeoInstanceCollection, *pLastTechnique->m_Shaders.GetAt(0));
        if (bBasePass)
          s_RenderGeoInstanceCollection.TagEntries();
      } 
      s_RenderGeoInstanceCollection.Clear();
    }
  }

  // Restore default render state
  VisRenderStates_cl::SetDepthStencilState(*VisRenderStates_cl::GetDepthStencilDefaultState());
}

VCompiledTechnique *VMobileForwardRenderLoop::GetLightShader(VisLightSource_cl *pLight, bool bBasePass, const VisSurface_cl *pSurface,  
                                                             float fFade, IVShadowMapComponent *pShadowMapComponent)
{
  VASSERT(pLight!=NULL && pSurface!=NULL);

  VCompiledTechnique *pTech = NULL;
  bool bDynamicLighting = pLight->IsDynamic()==TRUE;
  VisLightSourceType_e lightType = pLight->GetType(); 
  VASSERT(lightType==VIS_LIGHT_POINT || lightType==VIS_LIGHT_SPOTLIGHT || lightType==VIS_LIGHT_DIRECTED);

  VTextureObject *pProjTexture = NULL;
  hkvPlane plane_x(hkvNoInitialization);
  hkvPlane plane_y(hkvNoInitialization);
  hkvPlane plane_z(hkvNoInitialization);
  
  int iLightingFlags = 0;
  if (bBasePass)
    iLightingFlags |= VMOBILE_LIGHTING_FLAG_BASEPASS;
  if (bDynamicLighting)
    iLightingFlags |= VMOBILE_LIGHTING_FLAG_DYNAMIC;
  if (pShadowMapComponent)
    iLightingFlags |= VMOBILE_LIGHTING_FLAG_SHADOW;

  // 1: pick light shader from material
  switch (iLightingFlags)
  {
  case VMOBILE_LIGHTING_FLAG_DYNAMIC:
    {
      if (lightType == VIS_LIGHT_POINT)
        pTech = pSurface->m_spDynPointLight;
      else if (lightType == VIS_LIGHT_SPOTLIGHT)
        pTech = pSurface->m_spDynSpotLight;
      else if (lightType == VIS_LIGHT_DIRECTED)
        pTech = pSurface->m_spDynDirectionalLight; 
      break;
    }

  case (VMOBILE_LIGHTING_FLAG_DYNAMIC | VMOBILE_LIGHTING_FLAG_SHADOW):
    {
      if (lightType == VIS_LIGHT_SPOTLIGHT)
        pTech = pSurface->m_spDynSpotLightShadow;
      else if (lightType == VIS_LIGHT_DIRECTED)
        pTech = pSurface->m_spDynDirectionalLightShadow; 
      break;
    }
   
  case (VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_DYNAMIC):
    {
      if (lightType == VIS_LIGHT_POINT)
        pTech = pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_POINTLIGHT];
      else if (lightType == VIS_LIGHT_SPOTLIGHT)
        pTech = pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT]; 
      else if (lightType == VIS_LIGHT_DIRECTED)
        pTech = pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT];
      break;
    }

  case (VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_DYNAMIC | VMOBILE_LIGHTING_FLAG_SHADOW):
    {
      if (lightType == VIS_LIGHT_SPOTLIGHT)
        pTech = pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT_SHADOW];
      else if (lightType == VIS_LIGHT_DIRECTED)
        pTech = pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT_SHADOW];
      break;
    }

  case (VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_SHADOW):
    {
      if (lightType == VIS_LIGHT_SPOTLIGHT)
        pTech = pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT_SUBTRACTIVE_SHADOW];
      else if (lightType == VIS_LIGHT_DIRECTED)
        pTech = pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT_SUBTRACTIVE_SHADOW];
      break;
    }
 
  case VMOBILE_LIGHTING_FLAG_BASEPASS:
    break; // rendering in base pass without lights and shadows results in rendering within the default base pass

  default:
    VASSERT_MSG(false, "Unsupported lighting flags");
  }
  if (!pTech)
    return NULL;

  if (bDynamicLighting && lightType==VIS_LIGHT_SPOTLIGHT)
  {
    pProjTexture = pLight->GetProjectionTexture();
    if (!pProjTexture)
      pProjTexture = GetDefaultSpotlightTexture();
    pLight->GetProjectionPlanes(plane_x, plane_y, plane_z); // this has the cone angle encoded already
  }

  // 2: modify light shader
  hkvVec3 vColor, lightDir;
  float fMultiplier = 0.0f;
  if (bDynamicLighting)
  {
    vColor = pLight->GetColor().ToFloat();
    fMultiplier = pLight->GetMultiplier()*fFade;
    if (lightType == VIS_LIGHT_DIRECTED || lightType == VIS_LIGHT_SPOTLIGHT)
    {
      lightDir = pLight->GetDirection();
      lightDir.normalize();
    }
  }

  // Attenuation texture not used by MobileShaders.ShaderLib, but may be used
  // by manually assigned shader libs.
  VTextureObject *pAttTexture = pLight->GetAttenuationTexture();
  if (pAttTexture == NULL)
    pAttTexture = GetDefaultAttenuationTexture();

  const int iPassCount = pTech->GetShaderCount();
  for (int i=0;i<iPassCount;i++)
  {
    VDynamicLightShaderBase *pPass = vdynamic_cast<VDynamicLightShaderBase*>(pTech->GetShader(i));
    VASSERT_MSG(pPass != NULL, "Dynamic light shaders must be of class VDynamicLightShaderBase");   
    if (!pPass)
    { 
      Vision::Error.Warning("Dynamic light shader is not of class VDynamicLightShaderBase; dynamic light will have no effect.");
      continue;
    }
   
    // If the lighting shader is not mobile specific, we can't use the shadow component, since 
    // it is mobile specific. In this case we recall this method without a valid shadow component,
    // in order to render the lit surfaces without shadows.
    if (!pTech->GetShader(i)->IsOfType(VMobileDynamicLightShader::GetClassTypeId()) && pShadowMapComponent)
      return GetLightShader(pLight, bBasePass, pSurface, fFade, NULL);

    pPass->SetAttenuationTexture(pAttTexture);

    if (bDynamicLighting)
    {
      // set light source properties
      pPass->SetPosition(pLight->GetPosition());
      pPass->SetRadius(pLight->GetRadius());
      pPass->SetColor(vColor, fMultiplier);
      
      // set light direction for directed lights
      if (lightType == VIS_LIGHT_DIRECTED)
        pPass->SetDirection(lightDir);

      // set light direction and cone angle for spot lights
      else if (lightType == VIS_LIGHT_SPOTLIGHT)
      {
        pPass->SetDirection(lightDir);
        pPass->SetProjectionAngle(pLight->GetProjectionAngle());
      }

      if (pProjTexture)
      {
        VDynamicLightShader *pDynamicLightShader = vdynamic_cast<VDynamicLightShader*>(pPass);
        if (pDynamicLightShader)
        {
          pDynamicLightShader->SetProjectedTexture(pProjTexture);
          pDynamicLightShader->SetProjectionPlanes(plane_x,plane_y,plane_z);
        }
      }
    }

#ifdef SUPPORTS_SHADOW_MAPS
    if (pShadowMapComponent)
    {
      VMobileShadowMapComponentSpotDirectional *pMobileSpotDirShadowMapComponent = vstatic_cast<VMobileShadowMapComponentSpotDirectional*>(pShadowMapComponent);
      if (pMobileSpotDirShadowMapComponent)
        pMobileSpotDirShadowMapComponent->UpdateLightShader((VMobileDynamicLightShader*)pPass);
    }
#endif
    
    pPass->m_bModified = true;
  }

  return pTech;
}

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
