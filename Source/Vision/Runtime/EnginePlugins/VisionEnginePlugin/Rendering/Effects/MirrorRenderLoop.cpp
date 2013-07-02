/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Mirror.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/MirrorRenderLoop.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Sky/Sky.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>

// TODO:
// - Sinnvolle shader modes (always simple, mirror shader with fallback, scene shader)
// - Make collections static so they can be shared across mirror objects???



V_IMPLEMENT_DYNAMIC(MirrorRenderLoop_cl,VisTypedEngineObject_cl, &g_VisionEngineModule);


MirrorRenderLoop_cl::MirrorRenderLoop_cl(VisMirror_cl *pMirror) : 
   m_visiblePrimaryOpaquePassGeoInstances(1024, 1024),
   m_visibleSecondaryOpaquePassGeoInstances(256, 256),
   m_visibleTransparentOpaquePassGeoInstances(256, 256),
   m_CustomGeoInstances(0,1024),
   m_visEntities(1024, 1024),
   m_dynLightDefaultState()
{
  m_bTriggerCallbacks = false; // no OnRenderHook callbacks
  m_pMirror = pMirror;

  CreateSimpleShaders();

  Vision::Callbacks.OnReassignShaders += this;
}

MirrorRenderLoop_cl::~MirrorRenderLoop_cl()
{
  Vision::Callbacks.OnReassignShaders -= this;
}

void MirrorRenderLoop_cl::CreateSimpleShaders()
{
  // create a dummy diffuse-only surface
  m_dummySurface.SetEffect(NULL); // make sure all lighting shaders are set properly
  VCompiledEffect *pFX = m_dummySurface.m_spCurrentEffect;
  VVERIFY_MSG(pFX != NULL,"failed to initialize mirror shaders");

  VTechniqueConfig *pGlobalConfig = Vision::Shaders.GetGlobalTechniqueConfig();
  VTechniqueConfig usageConfig;

  // default technique to render lightmapped geometry
  usageConfig.SetInclusionTags("MIRROR;LIGHTMAP");
  m_spDefaultLightMapping = pFX->FindCompatibleTechnique(&usageConfig, pGlobalConfig);
  if (!m_spDefaultLightMapping)
    m_spDefaultLightMapping = pFX->GetDefaultTechnique();

  // technique to render geometry with light grid
  usageConfig.SetInclusionTags("MIRROR;LIGHTGRID");
  m_spDefaultLightGrid = pFX->FindCompatibleTechnique(&usageConfig, pGlobalConfig);
  if (!m_spDefaultLightGrid)
    m_spDefaultLightGrid = pFX->GetDefaultTechnique();

  // depth stencil state
  m_dynLightDefaultState = *VisRenderStates_cl::GetDepthStencilDefaultState();
  m_dynLightDefaultState.m_cDepthComparisonFunc = COMPARISON_EQUAL;
  m_dynLightDefaultState.m_iStencilReadMask = 0;
  m_dynLightDefaultState.m_bDepthWriteEnabled = false;
  m_dynLightDefaultState.ComputeHash();

  VASSERT(m_spDefaultLightMapping && m_spDefaultLightGrid);
}

VCompiledShaderPass *MirrorRenderLoop_cl::GetMirrorShader (const VisSurface_cl* pMaterial, VisMirror_cl::VReflectionShaderSets_e Mode)
{
  VASSERT (pMaterial != NULL);

  bool bUseSimple = true;

  if (Mode == VisMirror_cl::AlwaysSurfaceShaders)
    bUseSimple = false;

  if ((Mode == VisMirror_cl::SimpleForAUTO) && (pMaterial->GetShaderMode () == VisSurface_cl::VSM_Manual))
    bUseSimple = false;

  if (!bUseSimple)
  {
    if ((pMaterial->m_spMirror != NULL) && (pMaterial->m_spMirror->GetShaderCount () > 0))
      return (pMaterial->m_spMirror->GetShader (0));
  }

  if (pMaterial->m_spMirrorSimple!=NULL && pMaterial->m_spMirrorSimple->GetShaderCount()>0)
    return pMaterial->m_spMirrorSimple->GetShader(0);

  VASSERT_MSG (false, "No Mirror Shader available.");

  return (NULL);
}


void MirrorRenderLoop_cl::GetLightShader (const VisLightSource_cl *pLight, VisStaticGeometryInstance_cl *pGI, VisMirror_cl::VReflectionShaderSets_e Mode,
                                          VisSurface_cl* pLastSurface, VCompiledTechnique* pLastTechnique, VisSurface_cl*& out_pNewSurface, VCompiledTechnique*& out_pNewTechnique)
{
  VASSERT (pLight != NULL);
  VASSERT (pGI != NULL);

  bool bUseSimple = true;

  if (Mode == VisMirror_cl::AlwaysSurfaceShaders)
    bUseSimple = false;

  if ((Mode == VisMirror_cl::SimpleForAUTO) && (pGI->GetSurface()->GetShaderMode () == VisSurface_cl::VSM_Manual))
    bUseSimple = false;

  if (pGI->GetGeometryType() == STATIC_GEOMETRY_TYPE_TERRAIN)
    bUseSimple = false;

  out_pNewSurface = &m_dummySurface;

  if (!bUseSimple)
  {
    out_pNewSurface = pGI->GetSurface();
  }
  
  // Reuse old technique as the surface hasn't changed.
  if (out_pNewSurface == pLastSurface)
  {
    out_pNewTechnique = pLastTechnique;
  }
  else
  {
    out_pNewTechnique = Vision::GetApplication()->GetShaderProvider()->GetDynamicLightShader(pLight, out_pNewSurface, false);
  }
}

// Simplified version of dynamic light rendering for mirrors
void MirrorRenderLoop_cl::DrawDynamicLight()
{
  INSERT_PERF_MARKER_SCOPE("MirrorRenderLoop_cl::DrawDynamicLight");

  // Some local variables for storing surfaces, shaders, surface shaders, and the like.
  VisDrawCallInfo_t SurfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];
  VCompiledTechnique *pTechnique = NULL;

  VisMirror_cl::VReflectionShaderSets_e shaderMode = m_pMirror->m_eReflectionShaderMode;

  // Get all visible light sources
  IVisVisibilityCollector_cl *pVisColl = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  if (pVisColl == NULL)
    return;

  const VisLightSrcCollection_cl *pLightSourceCollection = pVisColl->GetVisibleLights();
  unsigned int i;
  unsigned int iNumLights = pLightSourceCollection->GetNumEntries();
  if (iNumLights == 0)
    return;

  // Set depth-stencil state
  VisRenderStates_cl::SetDepthStencilState(m_dynLightDefaultState);
  
  // For all visible lights...
  for (i=0; i<iNumLights; i++)
  {
    VisLightSource_cl *pLight = pLightSourceCollection->GetEntry(i);
    // We're only interested in dynamic lights
    if (!pLight->IsDynamic()) 
      continue;

    // Clear the collections of geo instances and entities, since we want to build them from scratch for each light
    s_LitEntityCollection.Clear();
    s_LitGeoInstanceCollection.Clear();

    // See which geometry types have to cast shadows
    int iReceiverFlags = GetLightReceiverFlags(pLight);

    // If nothing receives light from this light source, we can proceed to the next light.
    if (!iReceiverFlags)
      continue;

    // ***************** Create lists of illuminated scene elements *****************

    // If no shadows are cast, we simply illuminate all visible geometry within the range (spherical) of the light.
    VisEntityCollection_cl *pEntColl = NULL;
    if (iReceiverFlags & VIS_LIGHTSRCVIS_MODELS)
      pEntColl = &s_LitEntityCollection;
    VisStaticGeometryInstanceCollection_cl *pGeoInstanceColl = NULL;
    if (iReceiverFlags & VIS_LIGHTSRCVIS_PRIMITIVES)
    {
      pGeoInstanceColl = &s_LitGeoInstanceCollection;
    }

    Vision::RenderLoopHelper.GetVisibleGeometryInLightsourceRange(pGeoInstanceColl, pEntColl, NULL, *pLight);

    // For all illuminated entities: Render a dynamic lighting pass now.
    if (pLight->GetLightInfluenceBitMaskEntity())
    {
      int j;
      
      int iNumLitEntities = s_LitEntityCollection.GetNumEntries();

      Vision::RenderLoopHelper.BeginEntityRendering();

      for (j=0; j<iNumLitEntities; j++)
      {
        VisBaseEntity_cl *pEntity = s_LitEntityCollection.GetEntry(j);
        // Ignore foreground entities (they don't trivially support additive lighting)
        if (pEntity->IsObjectAlwaysInForegroundEnabled())
          continue;
        if (!(pEntity->GetLightInfluenceBitMask() & pLight->GetLightInfluenceBitMaskEntity()))
          continue;
        if (!pVisColl->IsEntityVisible(pEntity))
          continue;
          
        VDynamicMesh *pMesh = pEntity->GetMesh();
          
        // Get list of all the surfaces in the model
        int iNumSubmeshes = pMesh->GetSubmeshCount();
        int iNumSurfaceShaders = 0;
        VisSurface_cl **ppSurfaceArray = pEntity->GetSurfaceArray();

        // For all the surfaces...
        for (int k=0; k<iNumSubmeshes; k++)
        {
          VDynamicSubmesh *pSubmesh = pMesh->GetSubmesh(k);
          VASSERT(pSubmesh != NULL);
          VisSurface_cl* pSurface = &m_dummySurface; 
          VisSurface_cl* pMeshSurface = pSubmesh->m_pSurface;
          VASSERT(pMeshSurface != NULL);

          bool bHasManualTemplateShaderAssignment = pMeshSurface->GetShaderMode() == VisSurface_cl::VSM_Template 
            && pMeshSurface->GetMaterialTemplate() != NULL && pMeshSurface->GetMaterialTemplate()->HasManualAssignment();

          if (shaderMode == VisMirror_cl::AlwaysSurfaceShaders || 
            (shaderMode == VisMirror_cl::SimpleForAUTO && ( (pMeshSurface->GetShaderMode() == VisSurface_cl::VSM_Manual) || bHasManualTemplateShaderAssignment) ) )
          {
            pSurface = ppSurfaceArray[pSubmesh->m_iMaterialIndex]; // use the real surface
          }
          pTechnique = Vision::GetApplication()->GetShaderProvider()->GetDynamicLightShader(pLight, pSurface, true);
          if (pTechnique==NULL)
            continue;
            
          VisDrawCallInfo_t &info(SurfaceShaderList[iNumSurfaceShaders++]);
          info.Set(pSubmesh, pSurface, pTechnique->m_Shaders.GetAt(0));
        }
        // Finally, render the entity with a surface shader list.
        if (iNumSurfaceShaders>0)
          Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, SurfaceShaderList);
      }

      Vision::RenderLoopHelper.EndEntityRendering();
    }

    // For all illuminated world primitives: Render a dynamic lighting pass now
    if (pLight->GetLightInfluenceBitMaskWorld() > 0)
    {
      // For all illuminated static geometry instances: Render a dynamic lighting pass now.
      int iNumLitGeoInstances = s_LitGeoInstanceCollection.GetNumEntries();

      s_RenderGeoInstanceCollection.Clear();

      // Render illuminated geometry instances. 
      for (int j=0; j < iNumLitGeoInstances; j++)
      {
        VisStaticGeometryInstance_cl *pGI = s_LitGeoInstanceCollection.GetEntry(j);

        if (pGI->GetSurface()==NULL || pGI->GetSurface()->IsFullbright())
          continue;
       
        // We have to append the primitive to our collection
        s_RenderGeoInstanceCollection.AppendEntry(pGI);
      }

      // render the collection
      const int iLitGeoCount = s_RenderGeoInstanceCollection.GetNumEntries();
      if (iLitGeoCount > 0)
      {
        VCompiledTechnique *pLastTech = NULL;
        VisSurface_cl* pLastSurface = NULL;
        m_CustomGeoInstances.EnsureSize(iLitGeoCount);
        m_CustomGeoInstances.Clear();

        for (int j=0; j < iLitGeoCount; j++)
        {
          VisStaticGeometryInstance_cl *pGI = s_RenderGeoInstanceCollection.GetEntry(j);

          GetLightShader (pLight, pGI, m_pMirror->m_eReflectionShaderMode, pLastSurface, pLastTech, pLastSurface, pTechnique);
          
          // The current technique has changed, so we have to render the previously gathered geometry.
          if (pLastTech != pTechnique)
          {
            if ((m_CustomGeoInstances.GetNumEntries() > 0) && (pLastTech != NULL) && (pLastTech->GetShaderCount() > 0))
            {
              Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_CustomGeoInstances, *pLastTech->m_Shaders.GetAt(0) );
              m_CustomGeoInstances.Clear();
            }
            pLastTech = pTechnique;
          }
          m_CustomGeoInstances.AppendEntryFast(pGI);
        }
        // Render remaining geometry
        if ((m_CustomGeoInstances.GetNumEntries() > 0) && (pLastTech != NULL) && (pLastTech->GetShaderCount() > 0))
        {
          Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_CustomGeoInstances, *pTechnique->m_Shaders.GetAt(0) );
        }

        s_RenderGeoInstanceCollection.Clear();
      }
    }
  }

  // Restore default render state
  VisRenderStates_cl::SetDepthStencilState(*VisRenderStates_cl::GetDepthStencilDefaultState());
}

void MirrorRenderLoop_cl::DrawStaticGeometry(const VisStaticGeometryInstanceCollection_cl &collection, int iPassType)
{
  INSERT_PERF_MARKER_SCOPE("MirrorRenderLoop_cl::DrawStaticGeometry");

  m_CustomGeoInstances.Clear();

  int iNumGI = collection.GetNumEntries();
  VCompiledShaderPass *pCurrentPass = NULL;

  for (int i=0; i<iNumGI; i++)
  {
    VisStaticGeometryInstance_cl *pGI = collection.GetEntry(i);
    if ( pGI->GetGeometryType() == STATIC_GEOMETRY_TYPE_TERRAIN )
      ((VTerrainSector*)pGI)->EnsureLoaded();
    VCompiledShaderPass *pPass = GetMirrorShader (pGI->GetSurface (), m_pMirror->m_eReflectionShaderMode);
    if (pPass != pCurrentPass)
    {
      if (m_CustomGeoInstances.GetNumEntries() > 0)
      {
        VASSERT(pCurrentPass != NULL);
        Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_CustomGeoInstances, *pCurrentPass); // can use simplified lightmap shader
        m_CustomGeoInstances.Clear();
      }
      pCurrentPass = pPass;
    }
    m_CustomGeoInstances.AppendEntry(pGI);
  }

  // Render remaining instances
  if (m_CustomGeoInstances.GetNumEntries() > 0 && pCurrentPass != NULL)
    Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_CustomGeoInstances, *pCurrentPass); // can use simplified lightmap shader
}


#define TRIGGER_MIRROR_HOOK(iEntry) \
  {\
    VisMirrorRenderHookDataObject_cl cb_data(&VisMirrorManager_cl::OnMirrorRenderHook,m_pMirror,iEntry); \
    cb_data.Trigger(); \
  }


// TODO: This doesn't handle opaque fullbright surfaces correctly yet, and translucent fullbright surfaces are simply ignored.
void MirrorRenderLoop_cl::OnDoRenderLoop(void *pUserData)
{
  INSERT_PERF_MARKER_SCOPE("MirrorRenderLoop_cl::OnDoRenderLoop");

#if defined (WIN32) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
  if (Vision::RenderLoopHelper.GetReplacementRenderLoop())
  {
    // render with this renderloop instead
    Vision::RenderLoopHelper.GetReplacementRenderLoop()->OnDoRenderLoop(pUserData);
    return;
  }
  if (Vision::Editor.GetIgnoreAdvancedEffects())
  {
    // force a black reflection because it won't work with orthographic views
    Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, V_RGBA_BLACK);
    return;
  }
#endif

  VisRenderContext_cl *pContext = Vision::Contexts.GetCurrentContext();

  const int iRenderFlags = pContext->GetRenderFlags();

  const float fFarClipDist = m_pMirror->GetActualFarClipDistance();

  const VFogParameters &fog = Vision::World.GetFogParameters();
  VColorRef clearColor = (fog.depthMode != VFogParameters::Off) ? fog.iDepthColor : Vision::Renderer.GetDefaultClearColor();
  Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, clearColor);

  // set the oblique clipping plane...
  pContext->SetCustomProjectionMatrix (m_pMirror->GetObliqueClippingProjection().getPointer ());

  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesPrimaryOpaquePass;
  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesSecondaryOpaquePass;
  const VisStaticGeometryInstanceCollection_cl *pVisibleGeoInstancesTransparentPass;
  const VisEntityCollection_cl *pVisEntities;


  // === Visibility Determination ===

  IVisVisibilityCollector_cl *pVisColl = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  if (pVisColl == NULL)
    return;
  const VisVisibilityObjectCollection_cl *pVisObjectCollection = pVisColl->GetVisibleVisObjects();

  hkvAlignedBBox box;
  int iVoCount = m_pMirror->GetVisibilityObjectCount();
  int iFrustumCount = 0;
  bool bUseCommonFrustum = false;

  // === Determine Scissor Rect ===
  hkvVec2 vMinScreenSpace, vMaxScreenSpace;
  const hkvAlignedBBox &worldSpaceBox = m_pMirror->GetBoundingBox();

  hkvVec3 vCorners[8];
  worldSpaceBox.getCorners (vCorners);

  VRectanglef scissorRect;
  bool bUseScissorRect = true;
  for (int i=0; i<8; i++)
  {
    float x2d, y2d;
    BOOL bInFrontOfCamera = pContext->Project2D(vCorners[i], x2d, y2d);
    if (bInFrontOfCamera)
    {
      scissorRect.Add(hkvVec2(x2d, y2d));
    }
    else
    {
      bUseScissorRect = false;
      break;
    }
  }

  if (bUseScissorRect)
    Vision::RenderLoopHelper.SetScissorRect(&scissorRect);

  for (int iVo = 0; iVo < iVoCount; iVo++)
  {
    VisVisibilityObject_cl *pVisObj = m_pMirror->GetVisibilityObject(iVo);
    if (pVisObj != NULL && pVisObj->WasVisibleInAnyLastFrame())
    {
      if (iFrustumCount <= MAX_SEPARATE_FRUSTA)
      {
        const hkvAlignedBBox &voBox = pVisObj->GetWorldSpaceBoundingBox();
        box.expandToInclude(voBox);
        if (m_Frustum[iFrustumCount].Set(pContext->GetCamera()->GetPosition(), voBox, true, fFarClipDist))
        {
          iFrustumCount++;
        }
        else
        {
          bUseCommonFrustum = true;
        }
      }
      else
      {
        const hkvAlignedBBox &voBox = pVisObj->GetWorldSpaceBoundingBox();
        box.expandToInclude(voBox);
        bUseCommonFrustum = true;
      }
    }
  }

  if (bUseCommonFrustum)
  {
    iFrustumCount = 1;
    if (!m_Frustum[0].Set(pContext->GetCamera()->GetPosition(), box, true, fFarClipDist))
      iFrustumCount = 0;
  }

  if (iFrustumCount>0)
  {
    for (int i=0; i<iFrustumCount; i++)
    {
      m_visiblePrimaryOpaquePassGeoInstances.Clear();
      m_visibleSecondaryOpaquePassGeoInstances.Clear();
      m_visibleTransparentOpaquePassGeoInstances.Clear();
      m_visEntities.Clear();
      pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_PrimaryOpaquePass)->DetermineEntriesTouchingFrustum(m_Frustum[i], m_visiblePrimaryOpaquePassGeoInstances);
      pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_SecondaryOpaquePass)->DetermineEntriesTouchingFrustum(m_Frustum[i], m_visibleSecondaryOpaquePassGeoInstances);
      pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_TransparentPass)->DetermineEntriesTouchingFrustum(m_Frustum[i], m_visibleTransparentOpaquePassGeoInstances);
      pVisColl->GetVisibleEntities()->DetermineEntriesTouchingFrustum(m_Frustum[i], m_visEntities);
      if (iFrustumCount == 1)
        break;
      m_visiblePrimaryOpaquePassGeoInstances.TagEntries();
      m_visibleSecondaryOpaquePassGeoInstances.TagEntries();
      m_visibleTransparentOpaquePassGeoInstances.TagEntries();
      m_visEntities.TagEntries();
    }
    if (iFrustumCount > 1)
    {
      m_visiblePrimaryOpaquePassGeoInstances.Clear();
      m_visibleSecondaryOpaquePassGeoInstances.Clear();
      m_visibleTransparentOpaquePassGeoInstances.Clear();
      m_visEntities.Clear();
      pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_PrimaryOpaquePass)->GetTaggedEntries(m_visiblePrimaryOpaquePassGeoInstances);
      pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_SecondaryOpaquePass)->GetTaggedEntries(m_visibleSecondaryOpaquePassGeoInstances);
      pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_TransparentPass)->GetTaggedEntries(m_visibleTransparentOpaquePassGeoInstances);
      pVisColl->GetVisibleEntities()->GetTaggedEntries(m_visEntities);
    }
    pVisibleGeoInstancesPrimaryOpaquePass = &m_visiblePrimaryOpaquePassGeoInstances;
    pVisibleGeoInstancesSecondaryOpaquePass = &m_visibleSecondaryOpaquePassGeoInstances;
    pVisibleGeoInstancesTransparentPass = &m_visibleTransparentOpaquePassGeoInstances;
    pVisEntities = &m_visEntities;
  }
  else
  {
    pVisibleGeoInstancesPrimaryOpaquePass = pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_PrimaryOpaquePass);
    pVisibleGeoInstancesSecondaryOpaquePass = pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_SecondaryOpaquePass);
    pVisibleGeoInstancesTransparentPass = pVisColl->GetVisibleStaticGeometryInstancesForPass(VPT_TransparentPass);
    pVisEntities = pVisColl->GetVisibleEntities();
  }

  // === End Visibility Determination ===

  if (m_pMirror->GetExecuteRenderHooks())
  {
    VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook,VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY);
    Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
  }

  // Render opaque static geometry
  VASSERT(m_spDefaultLightMapping->m_Shaders.Count()==1);

  TRIGGER_MIRROR_HOOK(VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY)
  VisMirror_cl::VReflectionShaderSets_e shaderMode = m_pMirror->m_eReflectionShaderMode;
  DrawStaticGeometry(*pVisibleGeoInstancesPrimaryOpaquePass, VPT_PrimaryOpaquePass);
  DrawStaticGeometry(*pVisibleGeoInstancesSecondaryOpaquePass, VPT_SecondaryOpaquePass);

  // Render entities
  const VisEntityCollection_cl *pEntities = pVisEntities;
  int iCount = pEntities->GetNumEntries();
  VASSERT(m_spDefaultLightGrid->m_Shaders.Count()==1);
  //VCompiledShaderPass *pLightgridShader = m_spDefaultLightGrid->m_Shaders.GetAt(0);
  int i;
  //bool bUseSimpleShader = shaderMode==VisMirror_cl::AlwaysSimple;

  Vision::RenderLoopHelper.BeginEntityRendering();

  for (i=0;i<iCount;i++)
  { 
    VisBaseEntity_cl *pEnt = pEntities->GetEntry(i);

    Vision::RenderLoopHelper.TrackLightGridInfo(pEnt);

    //if (bUseSimpleShader)
    //{
    //  Vision::RenderLoopHelper.RenderEntityWithShaders(pEnt,1,&pLightgridShader);
    //}
    //else
    {
      VisDrawCallInfo_t surfaceShaderList[RLP_MAX_ENTITY_SURFACES];
      VDynamicMesh *pMesh = pEnt->GetMesh();
      VisSurface_cl **ppSurfaces = pEnt->GetSurfaceArray();
      int iNumSubmeshes = pMesh->GetSubmeshCount();
      for (int j=0; j<iNumSubmeshes; j++)
      {
        VisDrawCallInfo_t &info(surfaceShaderList[j]);
        VBaseSubmesh* pSubmesh = pMesh->GetSubmesh(j);
        VisSurface_cl* pSurface = ppSurfaces[pSubmesh->m_iMaterialIndex];
        info.Set(pSubmesh, pSurface, GetMirrorShader (pSurface, shaderMode));
      }

      Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEnt, iNumSubmeshes, surfaceShaderList);
    }
  }

  Vision::RenderLoopHelper.EndEntityRendering();

  // Render Sky
  if (VSky::IsVisible())
  {
    // The sky has to be rendered without oblique clipping
    pContext->SetCustomProjectionMatrix(NULL);
    Vision::RenderLoopHelper.RenderSky();
    // set the oblique clipping plane after sky...
    pContext->SetCustomProjectionMatrix (m_pMirror->GetObliqueClippingProjection().getPointer ());
  }

  if (m_pMirror->GetExecuteRenderHooks())
  {
    VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook,VRH_PRE_OCCLUSION_TESTS);
    Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
  }

  // Render Coronas / Lens Flares
  VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook,VRH_CORONAS_AND_FLARES);
  Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
  TRIGGER_MIRROR_HOOK(VRH_PRE_OCCLUSION_TESTS)  

  if (iRenderFlags&VIS_RENDERCONTEXT_FLAG_USE_OCCLUSIONQUERY)
    Vision::RenderLoopHelper.PerformHardwareOcclusionQuery();

  if (iRenderFlags&VIS_RENDERCONTEXT_FLAG_USE_PIXELCOUNTER)
    Vision::RenderLoopHelper.PerformHardwarePixelCounterQuery();

  DrawDynamicLight();

  TRIGGER_MIRROR_HOOK(VRH_DECALS)
  TRIGGER_MIRROR_HOOK(VRH_CORONAS_AND_FLARES)

  TRIGGER_MIRROR_HOOK(VRH_PRE_TRANSPARENT_PASS_GEOMETRY)
  DrawStaticGeometry(*pVisibleGeoInstancesTransparentPass, VPT_TransparentPass);
  TRIGGER_MIRROR_HOOK(VRH_POST_TRANSPARENT_PASS_GEOMETRY)

  if (bUseScissorRect)
    Vision::RenderLoopHelper.SetScissorRect(NULL);
}

void MirrorRenderLoop_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData) 
{
  if (pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    CreateSimpleShaders();
  }

  // Call base implementation
  VisionRenderLoop_cl::OnHandleCallback(pData);
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
