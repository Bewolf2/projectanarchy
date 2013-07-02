/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessTranslucencies.hpp>

V_IMPLEMENT_DYNAMIC(VPostProcessTranslucencies, VPostProcessingBaseComponent, &g_VisionEngineModule);

VPostProcessTranslucencies::VPostProcessTranslucencies(bool bQuarterSizeParticles)
{
  m_bQuarterSizeParticles = bQuarterSizeParticles;

#if defined (WIN32)
  if (m_bQuarterSizeParticles)
  {
    Vision::Error.Warning("Win32 version does not support quarter-size particle rendering.");
  }
  m_bQuarterSizeParticles = false;
#endif

  m_pOldContext = NULL;

  m_fPriority = VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR_RESOLVED;

  m_iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE;
  m_bufferUsageFlags = USES_BLENDING;
}

VPostProcessTranslucencies::~VPostProcessTranslucencies()
{
}

void VPostProcessTranslucencies::InitializePostProcessor()
{
  if (m_bIsInitialized)
    return;

  GetTargetContext()->SetRenderFlags(VIS_RENDERCONTEXT_FLAG_SHOW_DEBUGOUTPUT|VIS_RENDERCONTEXT_FLAG_RENDER_CORONAS|VIS_RENDERCONTEXT_FLAG_RENDER_LENSFLARES);

  GetTargetContext()->SetName("Translucency");

#ifdef _VISION_PSP2
  GetTargetContext()->ForceDepthStencilSurfaceLoad(true);
#endif

  GetOwner()->AddContext(GetTargetContext());

  m_TempParticleGroupCollection = new VisParticleGroupCollection_cl(512, 512);
  m_TempMeshBufferObjectCollection = new VisMeshBufferObjectCollection_cl(512, 512);

  Vision::Shaders.LoadShaderLibrary("\\Shaders\\BaseShaders.ShaderLib", SHADERLIBFLAG_HIDDEN);
  m_spForegroundMaskTechnique = Vision::Shaders.CreateTechnique("BS_ForeGroundMask", NULL);
  VASSERT(m_spForegroundMaskTechnique!=NULL);

  m_spForegroundFillPassTechnique = Vision::Shaders.CreateTechnique("BS_ForeGroundFill",NULL);
  VASSERT(m_spForegroundFillPassTechnique!=NULL);

#if defined(_VISION_MOBILE) || defined(_VISION_PSP2)
  if(m_bQuarterSizeParticles)
    Vision::Error.Warning("Quarter size particles are not supported on mobile platforms");
  m_bQuarterSizeParticles = false;
#else
  if (m_bQuarterSizeParticles)
  {
    CreateQuarterSizeShadersAndContext();
  }
#endif

  m_bIsInitialized = true;
}

void VPostProcessTranslucencies::DeInitializePostProcessor()
{
  if (!m_bIsInitialized)
    return;

  V_SAFE_DELETE(m_TempParticleGroupCollection);
  V_SAFE_DELETE(m_TempMeshBufferObjectCollection);

#if !defined(_VISION_MOBILE) && !defined(_VISION_PSP2)
  m_spLowResContext = NULL;
  m_spLowResDepthTexture = NULL;
  m_spLowResColorTexture = NULL;
#endif

  GetOwner()->RemoveContext(GetTargetContext());

  m_bIsInitialized = false;
}


void VPostProcessTranslucencies::Execute()
{
  INSERT_PERF_MARKER_SCOPE("VPostProcessTranslucencies");

  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  IVisVisibilityCollector_cl *pVisCollector = pContext->GetVisibilityCollector();
  VASSERT(pVisCollector != NULL);

  const VisStaticGeometryInstanceCollection_cl *pVisibleTransparentGeoInstances = pVisCollector->GetVisibleStaticGeometryInstancesForPass(VPT_TransparentPass);
  const VisEntityCollection_cl *pVisibleEntities = pVisCollector->GetVisibleEntitiesForPass(VPT_TransparentPass);
  const VisEntityCollection_cl *pVisibleForeGroundEntities = pVisCollector->GetVisibleForeGroundEntities();

  m_VisibilityObjectCollector.HandleVisibleVisibilityObjects();

#ifndef _VISION_MOBILE
  RenderingOptimizationHelpers_cl::SetShaderPreference(96);
#endif

  // Get a pointer to the collection of visible mesh buffer objects
  const VisMeshBufferObjectCollection_cl *pVisibleMeshBuffer = &m_VisibilityObjectCollector.GetMeshBufferObjectCollection();

  // Get a pointer to the collection of visible particle groups
  const VisParticleGroupCollection_cl *pVisibleParticleGroups = &m_VisibilityObjectCollector.GetParticleGroupCollection();

  // Mask out entities which are "always in foreground"
  MaskOutForegroundEntities(*pVisibleForeGroundEntities);

  VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_TRANSPARENT_PASS_GEOMETRY, true);

  // render transparent pass surface shaders on translucent static geometry instances
  Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(*pVisibleTransparentGeoInstances, VPT_TransparentPass);

  VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PRE_TRANSPARENT_PASS_ENTITIES, true);

  // Render transparent pass shaders on entities
  DrawEntitiesShaders(*pVisibleEntities, VPT_TransparentPass);

  VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_POST_TRANSPARENT_PASS_GEOMETRY, true);

  VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_DECALS, true);

  RenderParticles(pVisibleMeshBuffer, pVisibleParticleGroups);

   // Render visible foreground entities (see DrawForegroundEntities)
  DrawTransparentForegroundEntities(*pVisibleForeGroundEntities);

  VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_CORONAS_AND_FLARES, true);
}

// Render shaders on entities
void VPostProcessTranslucencies::DrawEntitiesShaders(const VisEntityCollection_cl &EntityCollection, VPassType_e ePassType)
{
  VisDrawCallInfo_t SurfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];
  unsigned int iNumEntities = EntityCollection.GetNumEntries();
  unsigned int i;

  Vision::RenderLoopHelper.BeginEntityRendering();

  for (i=0; i<iNumEntities; i++)
  {
    VisBaseEntity_cl *pEntity = EntityCollection.GetEntry(i);
    // Foreground entities will be handled separately
    if (pEntity->IsObjectAlwaysInForegroundEnabled()) continue;
    if ( pEntity->HasShadersForPass(ePassType) )
    {
      // Get a list of the corresponding pass type surface shaders
      VisShaderSet_cl *pShaderSet = pEntity->GetActiveShaderSet();
      if (pShaderSet == NULL) continue;
      int iNumSurfaceShaders = pShaderSet->GetShaderAssignmentList(SurfaceShaderList, ePassType, RLP_MAX_ENTITY_SURFACESHADERS);
      VASSERT(iNumSurfaceShaders < RLP_MAX_ENTITY_SURFACESHADERS);
      if (iNumSurfaceShaders == 0) continue;

      VDynamicMesh *pMesh = pEntity->GetMesh();

      // If the model has lit surfaces, and if the shaders makes use of the lighting information, we need to set up
      // the global lights.
      if (pMesh != NULL && pMesh->HasLitSurfaces() && (pShaderSet->GetCombinedTrackingMask()&(VSHADER_TRACKING_LIGHTGRID_PS|VSHADER_TRACKING_LIGHTGRID_GS|VSHADER_TRACKING_LIGHTGRID_VS)) )
      {
        Vision::RenderLoopHelper.TrackLightGridInfo(pEntity);
      }
      // Render the entity with the surface shader list
      Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, SurfaceShaderList);
    } 
  }

  Vision::RenderLoopHelper.EndEntityRendering();

} 

void VPostProcessTranslucencies::RenderDeferredParticles(const VisParticleGroupCollection_cl *pVisibleParticleGroups, unsigned int iOrder)
{
  Vision::RenderLoopHelper.RenderParticleSystems(pVisibleParticleGroups, iOrder, Vision::Contexts.GetCurrentContext()->GetRenderFilterMask() );
}


void VPostProcessTranslucencies::MaskOutForegroundEntities(const VisEntityCollection_cl &EntityCollection)
{
  unsigned int iNumEntities = EntityCollection.GetNumEntries(); // this collection only contains foreground objects
  if (m_spForegroundMaskTechnique==NULL || iNumEntities==0)
    return;

  unsigned int i;
  const hkvMat4* pLastProj = NULL;
  INSERT_PERF_MARKER_SCOPE("VPostProcessTranslucencies::MaskOutForegroundEntities");

  Vision::RenderLoopHelper.BeginEntityRendering();

  for (i=0; i<iNumEntities; i++)
  {
    VisBaseEntity_cl *pEntity = EntityCollection.GetEntry(i);
    VASSERT_MSG(pEntity->IsObjectAlwaysInForegroundEnabled(), "Only entities with this flag should be passed to this function");
    if (!pEntity->HasShadersForPass(VPT_PrimaryOpaquePass))
      continue;

    const hkvMat4* pThisProj = pEntity->GetCustomProjectionMatrixForForegroundObject();
    if (pThisProj!=pLastProj)
    {
      VisRenderStates_cl::SetCurrentProjectionMatrix(pThisProj);
      pLastProj = pThisProj;
    }

    // depth fill pass
    Vision::RenderLoopHelper.RenderEntityWithShaders(pEntity, m_spForegroundMaskTechnique->GetShaderCount(), m_spForegroundMaskTechnique->GetShaderList());
  }

  Vision::RenderLoopHelper.EndEntityRendering();

  // reset to context projection matrix
  if (pLastProj)
  {
    VisRenderStates_cl::SetCurrentProjectionMatrix(NULL);
  }
}

// Renders foreground entities (i.e. entities which have been flagged as "always in foreground")
void VPostProcessTranslucencies::DrawTransparentForegroundEntities(const VisEntityCollection_cl &EntityCollection)
{
  unsigned int iNumEntities = EntityCollection.GetNumEntries(); // this collection only contains foreground objects
  if (iNumEntities==0 || m_spForegroundFillPassTechnique==NULL)
    return;

  INSERT_PERF_MARKER_SCOPE("VisionRenderLoop_cl::DrawForegroundEntities");

  unsigned int i;
  const hkvMat4* pLastProj = NULL;

  Vision::RenderLoopHelper.BeginEntityRendering();
  const int iPassCount = m_spForegroundFillPassTechnique->GetShaderCount();
  for (int iPass=0;iPass<=iPassCount;iPass++) // +1 passes, where the last one is the actual material pass
  {
    for (i=0; i<iNumEntities; i++)
    {
      VisBaseEntity_cl *pEntity = EntityCollection.GetEntry(i);

      // Render only Entities that are flagged as "always in foreground"
      VASSERT_MSG(pEntity->IsObjectAlwaysInForegroundEnabled(), "Only entities with this flag should be passed to this function");

      if (pEntity->HasShadersForPass(VPT_TransparentPass))
      {
        VDynamicMesh *pMesh = pEntity->GetMesh();
        VisShaderSet_cl *pShaderSet = pEntity->GetActiveShaderSet();

        VASSERT(pMesh && pShaderSet);

        const hkvMat4* pThisProj = pEntity->GetCustomProjectionMatrixForForegroundObject();

        if (pThisProj != pLastProj)
        {
          VisRenderStates_cl::SetCurrentProjectionMatrix(pThisProj);
          pLastProj = pThisProj;
        }

        if (iPass<iPassCount) // depth fill pass
        {
          VCompiledShaderPass *pPass = m_spForegroundFillPassTechnique->GetShader(iPass);
          Vision::RenderLoopHelper.RenderEntityWithShaders(pEntity, 1, &pPass);
        }
        else // material pass
        {
          const VisDrawCallInfo_t *pAssignment;

          int iNumSurfaceShaders = pShaderSet->GetShaderAssignmentList(&pAssignment);

          // If the shaders make use of the lighting information, we need to track the light grid
          if (pMesh != NULL && pMesh->HasLitSurfaces() && 
            (pShaderSet->GetCombinedTrackingMask() & (VSHADER_TRACKING_LIGHTGRID_PS|VSHADER_TRACKING_LIGHTGRID_GS|VSHADER_TRACKING_LIGHTGRID_VS)) )
          {
            Vision::RenderLoopHelper.TrackLightGridInfo(pEntity);
          }

          // Render the entity with the surface shader list
          Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, pAssignment);
        }
      }
    }
  }

  Vision::RenderLoopHelper.EndEntityRendering();

  // reset to context projection matrix
  if (pLastProj)
  {
    VisRenderStates_cl::SetCurrentProjectionMatrix(NULL);
  }
}

#if !defined(_VISION_MOBILE) && !defined(_VISION_PSP2)
void VPostProcessTranslucencies::CreateQuarterSizeShadersAndContext()
{
  Vision::Shaders.LoadShaderLibrary("\\Shaders\\Particles.ShaderLib", SHADERLIBFLAG_HIDDEN);

  Vision::Shaders.LoadShaderLibrary("\\Shaders\\vDeferredMaterial.ShaderLib", SHADERLIBFLAG_HIDDEN);
  m_spCopyTranslucenciesAlphaTechnique = Vision::Shaders.CreateTechnique("CopyTranslucenciesAlpha", NULL);
  VASSERT(m_spCopyTranslucenciesAlphaTechnique != NULL);
  m_spCopyTranslucenciesAdditiveTechnique = Vision::Shaders.CreateTechnique("CopyTranslucenciesAdditive", NULL);
  VASSERT(m_spCopyTranslucenciesAdditiveTechnique != NULL);

  int iWidth, iHeight;
  GetOwner()->GetReferenceContext()->GetSize(iWidth, iHeight);

  // Create color buffer
#if defined (_VISION_XENON)
  int iEdramOffset = GetOwner()->GetEdramOffset();

  m_spLowResColorTexture = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(iWidth/2, iHeight/2, VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP, 0, iEdramOffset);
  iEdramOffset += XGSurfaceSize(iWidth/2, iHeight/2, VTextureObject::ToD3DRenderTargetFormat(VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP), 
    D3DMULTISAMPLE_NONE);
#else
  m_spLowResColorTexture = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(iWidth/2, iHeight/2, VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP, 0, 0, false, 0, 0, false);
#endif

  // Create depth-stencil
  VisRenderableTextureConfig_t textureConfig;
  textureConfig.m_iWidth = iWidth / 2; 
  textureConfig.m_iHeight = iHeight / 2; 
  textureConfig.m_eFormat = VVideo::GetSupportedDepthStencilFormat(VTextureLoader::D24S8, *Vision::Video.GetCurrentConfig());
  textureConfig.m_bResolve = false;
  textureConfig.m_bRenderTargetOnly = true;
  textureConfig.m_bIsDepthStencilTarget = true;
#ifdef _VISION_XENON
  textureConfig.m_iEdramOffset = iEdramOffset;
  textureConfig.m_iHiZOffset = GetOwner()->GetHiZOffset();
#elif defined(_VISON_PS3)
  textureConfig.m_iHiZOffset = GetOwner()->GetHiZOffset();
#endif
  m_spLowResDepthTexture = Vision::TextureManager.CreateRenderableTexture("AccumulationBuffer", textureConfig);
  VASSERT(m_spLowResColorTexture!=NULL && m_spLowResDepthTexture!=NULL);

  // Create low-res context
  float fFar, fNear;
  VisRenderContext_cl::GetMainRenderContext()->GetClipPlanes(fNear, fFar);

  m_spLowResContext = new VisRenderContext_cl(NULL, 90.f, 90.f, iWidth/2, iHeight/2, fNear, fFar, 0);
  m_spLowResContext->SetCamera(VisRenderContext_cl::GetMainRenderContext()->GetCamera());
  m_spLowResContext->SetVisibilityCollector(GetOwner()->GetTranslucencyReferenceContext()->GetVisibilityCollector(),false);
  m_spLowResContext->SetRenderTarget(0, m_spLowResColorTexture);
  m_spLowResContext->SetDepthStencilTarget(m_spLowResDepthTexture);
  m_spLowResContext->SetName("TranslucencyRenderLoop:LowRes");
}
#endif

void VPostProcessTranslucencies::RenderParticles( const VisMeshBufferObjectCollection_cl * pVisibleMeshBuffer, const VisParticleGroupCollection_cl * pVisibleParticleGroups )
{
  if(!m_bQuarterSizeParticles)
  {
    VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_PARTICLES, true);
    VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_ADDITIVE_PARTICLES, true); 
    VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_TRANSLUCENT_VOLUMES, true);
  }
#if !defined(_VISION_MOBILE) && !defined(_VISION_PSP2)
  else
  {
    {
      // ALPHA-BLENDED PARTICLES + MESH BUFFERS
      INSERT_PERF_MARKER_SCOPE("VPostProcessTranslucencies: Alpha-Blended Particles (quarter-size)");

      SwitchToLowResContext();

      GetOwner()->RenderSceneDepth(true);
      Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_Color, VColorRef(0,0,0,255));

      RenderDeferredParticles(pVisibleParticleGroups, VRH_PARTICLES);
      VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, NULL, VRH_PARTICLES, true);


#if defined (_VISION_XENON)
      // Resolving is only required on Xbox360.
      m_spLowResColorTexture->Resolve();
#endif

      RestorePreviousContext();

      BlendQuarterSizeIntoTarget(m_spCopyTranslucenciesAlphaTechnique);
    }

    {
      // ADDITIVE PARTICLES + MESH BUFFERS
      INSERT_PERF_MARKER_SCOPE("VPostProcessTranslucencies: Additive Particles (quarter-size)");

      SwitchToLowResContext();

      Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_Color, VColorRef(0,0,0,0));

      VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_ADDITIVE_PARTICLES, true);


#if defined (_VISION_XENON)
      // Resolving is only required on Xbox360.
      m_spLowResColorTexture->Resolve();
#endif

      RestorePreviousContext();

      BlendQuarterSizeIntoTarget(m_spCopyTranslucenciesAdditiveTechnique);
    }

#ifdef _VISION_XENON
    //only a few pixels at the lower end of the depth buffer got overwritten
    //so we do a special depth restore only of the invalidated region of the depth buffer
    GetOwner()->RenderSceneDepthXbox360(VisHiZHelper_cl::VR_HIZ_RESTORE, -1.0f, -0.9f);
#endif

    //We can't trigger this render hook between VRH_PARTICLES and VRH_ADDITIVE_PARTICLES because of quarter size rendering
    VisionRenderLoop_cl::RenderHook(*pVisibleMeshBuffer, pVisibleParticleGroups, VRH_TRANSLUCENT_VOLUMES, true);
  }
#endif
}

#if !defined(_VISION_MOBILE) && !defined(_VISION_PSP2)
void VPostProcessTranslucencies::BlendQuarterSizeIntoTarget(VCompiledTechnique* pTechnique)
{
  IVRender2DInterface *pRenderer = Vision::RenderLoopHelper.BeginOverlayRendering();
  VCompiledShaderPass *pPass = pTechnique->GetShader(0);
  VStateGroupTexture *pStateGroupTexture = pPass->GetStateGroupTexture(VSS_PixelShader, 0);
  if (pStateGroupTexture != NULL)
    pStateGroupTexture->m_spCustomTex = m_spLowResColorTexture;
  pRenderer->Draw2DBufferWithShader(6, GetOwner()->GetRendererNodeHelper()->GetOverlayVertices(), m_spLowResColorTexture, *pPass);
  Vision::RenderLoopHelper.EndOverlayRendering();
}

void VPostProcessTranslucencies::SwitchToLowResContext()
{
  m_pOldContext = VisRenderContext_cl::GetCurrentContext();
  m_spLowResContext->SetViewProperties(m_pOldContext->GetViewProperties());
  m_spLowResContext->SetCamera(m_pOldContext->GetCamera());
  m_spLowResContext->Activate();
  VisRenderContext_cl::PerformPendingContextSwitch();
}

void VPostProcessTranslucencies::RestorePreviousContext()
{
  m_pOldContext->Activate();
  m_pOldContext = NULL;
  VisRenderContext_cl::PerformPendingContextSwitch();
}
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
