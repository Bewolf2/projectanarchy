/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenSpotDir.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/IVShadowMapComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VBaseShadowMapComponentSpotDirectional.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
#endif

int VShadowMapGenerator::PROFILING_RENDER_SHADOWMAP = 0;
int VShadowMapGenerator::PROFILING_POSTPROCESS_SHADOWMAP = 0;
int VShadowMapGenerator::PROFILING_SORT_SCENEELEMENTS = 0;
int VShadowMapGenerator::PROFILING_POSTFILER_SCENEELEMENTS = 0;


V_IMPLEMENT_DYNAMIC(VShadowmapVisibilityCollector,VisionVisibilityCollector_cl, &g_VisionEngineModule);


// ================================================================================
// VShadowMapGeneratorBase
// ================================================================================

EFFECTS_IMPEXP VCallback VShadowMapGenerator::OnRenderShadowMap;

VShadowMapGenerator::VShadowMapGenerator(IVRendererNode *pRendererNode, VisLightSource_cl *pRefLight, 
  IVShadowMapFormat* pShadowMapFormat, IVShadowMapComponent* pComponent, int iNumCascades, int iMaxNumCascades) :
  m_ShadowCasterEntities(512, 256)
{
  m_eProjectionType = SHADOW_PROJECTION_ORTHOGRAPHIC;  
  m_pLightSource = pRefLight;
  m_pRendererNode = pRendererNode;
  m_spShadowMapFormat = pShadowMapFormat;
  VASSERT(pComponent);
  m_pShadowComponent = pComponent;
  m_bPropertiesDirty = false;
  m_iNumParts = iNumCascades;
  m_pParts = new VShadowMapPart[iMaxNumCascades];
  m_bUpdateEnabled = false;
  m_bIsRenderedInterleaved = false;
  m_bIsInitialized = false;
  m_iPartsRendered = 0;  
  m_bRenderShadowVolumes = false;
  m_bShowProfilingData = false;
  m_bConsiderCastShadowFlag = true;

  Vision::Callbacks.OnUpdateSceneFinished += this;
  Vision::Callbacks.OnUpdateSceneBegin += this;
  
  // The OnContextSwitching callback is used to skip rendering in the first place and do it later in the light pass
  // (see VDeferredShadingLights::RenderLightPass)
  Vision::Callbacks.OnContextSwitching += this;
  
  #if defined (HK_DEBUG_SLOW)
    Vision::Callbacks.OnRenderHook += this;
  #endif

  for (int i=0; i<iMaxNumCascades; i++)
    m_pParts[i].SetShadowMapGenerator(this);

  // Force update by invalidating the last stored values
  m_vLastLightPos.set(FLT_MAX,FLT_MAX,FLT_MAX);
  m_fLastRadius = FLT_MAX;
}

VShadowMapGenerator::~VShadowMapGenerator()
{
  Vision::Callbacks.OnUpdateSceneFinished -= this;
  Vision::Callbacks.OnUpdateSceneBegin -= this;
  
  Vision::Callbacks.OnContextSwitching -= this;
  
  #if defined (HK_DEBUG_SLOW)
    Vision::Callbacks.OnRenderHook -= this;
  #endif

  V_SAFE_DELETE_ARRAY(m_pParts);
  m_pLightSource = NULL;
  m_pRendererNode = NULL;
}

void VShadowMapGenerator::DeInitialize()
{
  if (!m_bIsInitialized)
    return;

  for (int i=0; i<m_iNumParts; i++)
  {
    m_pRendererNode->RemoveContext(m_pParts[i].GetRenderContext());
    m_pParts[i].SetRenderContext(NULL);
  }

#if defined(WIN32)
  m_spShadowMap = NULL;
#elif defined (_VISION_PS3)
  m_spPatchedShadowMapDepthStencil = NULL;
#endif

  m_spShadowMapDepthStencil = NULL;

  m_spDebugMask = NULL;

  m_bIsRenderedInterleaved = false;
  m_bIsInitialized = false;
}


bool VShadowMapGenerator::Initialize()
{
  VASSERT(!m_bIsInitialized);
  VASSERT(m_pRendererNode);
  VASSERT(m_spShadowMapFormat.GetPtr());

  // All shadow maps are stored within a single renderable texture for faster access in the pixel shader
  int iShadowMapSize = m_pShadowComponent->GetShadowMapSize();
  switch(m_iNumParts)
  {
  case 1:
    m_iTextureSize[0] = iShadowMapSize;
    m_iTextureSize[1] = iShadowMapSize;
    m_pParts[0].m_mTexMatrix.m_Column[0][0] = 0.5f;
    m_pParts[0].m_mTexMatrix.m_Column[1][1] = -0.5f;
    m_pParts[0].m_mTexMatrix.m_Column[3][0] = m_pParts[0].m_mTexMatrix.m_Column[3][1] = 0.5f;
    break;
  case 2:
    m_iTextureSize[0] = iShadowMapSize * 2;
    m_iTextureSize[1] = iShadowMapSize;
    for (int i = 0; i < 2; i++)
    {
      m_pParts[i].m_mTexMatrix.m_Column[0][0] = 0.25f;
      m_pParts[i].m_mTexMatrix.m_Column[1][1] = -0.5f;
      m_pParts[i].m_mTexMatrix.m_Column[3][0] = 0.25f + i * 0.5f;    
      m_pParts[i].m_mTexMatrix.m_Column[3][1] = 0.5f;
    }
    break;
  case 3:
  case 4:
    m_iTextureSize[0] = iShadowMapSize * 2;
    m_iTextureSize[1] = iShadowMapSize * 2;
    for (int i = 0; i < 4; i++)
    {
      m_pParts[i].m_mTexMatrix.m_Column[0][0] = 0.25f;
      m_pParts[i].m_mTexMatrix.m_Column[1][1] = -0.25f;
    }
    m_pParts[0].m_mTexMatrix.m_Column[3][0] = 0.25f;
    m_pParts[0].m_mTexMatrix.m_Column[3][1] = 0.25f;
    m_pParts[1].m_mTexMatrix.m_Column[3][0] = 0.75f;
    m_pParts[1].m_mTexMatrix.m_Column[3][1] = 0.25f;
    m_pParts[2].m_mTexMatrix.m_Column[3][0] = 0.25f;
    m_pParts[2].m_mTexMatrix.m_Column[3][1] = 0.75f;
    m_pParts[3].m_mTexMatrix.m_Column[3][0] = 0.75f;
    m_pParts[3].m_mTexMatrix.m_Column[3][1] = 0.75f;  
    break;
  default:
    VASSERT(FALSE && "Unsupported number of cascades!");
  }

#ifndef _VISION_XENON
  int iMaxTextureSize = 4096;
#else
  int iMaxTextureSize = 1500;
#endif

  while ((m_iTextureSize[0] > iMaxTextureSize) || (m_iTextureSize[1] > iMaxTextureSize))
  {
    Vision::Error.Warning("Specified shadow map size/cascade count is too large for current hardware. Reducing resolution.");
    m_iTextureSize[0] >>= 1;
    m_iTextureSize[1] >>= 1;
    iShadowMapSize >>= 1;
  }
  m_pShadowComponent->ShadowMapSize = iShadowMapSize;
 
  for (int i = 0; i < m_iNumParts; i++)
  {
    VShadowMapPart& part = m_pParts[i];
    part.m_vTextureOffset.x = part.m_mTexMatrix.m_Column[3][0] - part.m_mTexMatrix.m_Column[0][0];
    part.m_vTextureOffset.y = part.m_mTexMatrix.m_Column[1][1] + part.m_mTexMatrix.m_Column[3][1]; 
#if defined(_VR_DX9) || defined(_VISION_XENON) 
    part.m_mTexMatrix.m_Column[3][0] += 0.5f / m_iTextureSize[0];
    part.m_mTexMatrix.m_Column[3][1] += 0.5f / m_iTextureSize[1];
#endif
  }

  // Create shadow map
  VisRenderableTextureConfig_t config;
  config.m_iWidth = m_iTextureSize[0];
  config.m_iHeight = m_iTextureSize[1];

#if !defined( _VISION_MOBILE ) && !defined( HK_ANARCHY )
  // Check if interleaved shadow map rendering should be used
  m_bIsRenderedInterleaved = false;
  if(VDeferredRenderingSystem* pDeferred = vdynamic_cast<VDeferredRenderingSystem*>(m_pRendererNode))
  {
    m_bIsRenderedInterleaved = pDeferred->IsUsingInterleavedShadowRendering();
  }
#endif

#if defined(WIN32) && defined(_VR_DX9)
  config.m_eFormat = m_spShadowMapFormat->GetShadowMapFormat();
  VASSERT(config.m_eFormat != VTextureLoader::NONE);

  if (!Vision::Video.IsRenderTargetFormatSupported(config.m_eFormat, *Vision::Video.GetCurrentConfig()))
    return false;

  // In case of interleaved shadow map rendering, we only need as much different textures as there are unique shadow maps,
  // so we can use the scratch texture pool...
  if (m_bIsRenderedInterleaved)
  {
    m_spShadowMap = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(config);
  }
  else
  {
    m_spShadowMap = Vision::TextureManager.CreateRenderableTexture("<ShadowMap>", config);
    m_spShadowMap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  }

  config.m_eFormat = m_spShadowMapFormat->GetShadowMapFormatDepthStencil();

  // Check if depth stencil format is readable as a texture
  if (Vision::Video.IsDepthStencilFormatSupported(config.m_eFormat, *Vision::Video.GetCurrentConfig()))
  {
    m_spShadowMapDepthStencil = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(config.m_iWidth, config.m_iHeight, config.m_eFormat, 0);
  }
  else
  {
    config.m_bIsDepthStencilTarget = true;
    config.m_bRenderTargetOnly = true;
    config.m_bResolve = true;
    m_spShadowMapDepthStencil = Vision::TextureManager.CreateRenderableTexture("<ShadowMapDepthStencil>", config);
    m_spShadowMapDepthStencil->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  }
#else

  bool bUseScratchTextureForDepthStencilTarget = m_bIsRenderedInterleaved;
#if defined( _VR_DX11 )
  if (VVideo::GetDXFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
  {
    config.m_eFormat = m_spShadowMapFormat->GetShadowMapFormat();
  VASSERT(config.m_eFormat != VTextureLoader::NONE);

    if (!Vision::Video.IsRenderTargetFormatSupported(config.m_eFormat, *Vision::Video.GetCurrentConfig()))
      return false;

    // In case of interleaved shadow map rendering, we only need as much different textures as there are unique shadow maps,
    // so we can use the scratch texture pool...
    if (m_bIsRenderedInterleaved)
    {
      m_spShadowMap = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(config);
    }
    else
    {
      m_spShadowMap = Vision::TextureManager.CreateRenderableTexture("<ShadowMap>", config);
      m_spShadowMap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    }

    bUseScratchTextureForDepthStencilTarget = false;

    // continue with the configuration for the actual depth stencil render target
    config.m_bIsDepthStencilTarget = true;
    config.m_bRenderTargetOnly = true;
  }
  else
#endif
  {
  config.m_bIsDepthStencilTarget = true;
  config.m_bRenderTargetOnly = false;
  }

  config.m_eFormat = m_spShadowMapFormat->GetShadowMapFormatDepthStencil();
  VASSERT(config.m_eFormat != VTextureLoader::NONE);

  #if defined (_VISION_PS3)
    // For point lights and spot lights, we don't want to waste a precious tile area - the overdraw is typically very
    // limited there.
    if (m_pLightSource->GetType() != VIS_LIGHT_DIRECTED)
    {
      config.m_bUseTiledMemory = false;
      config.m_bUseCompression = false;
    }
  #endif

  #if defined (_VISION_XENON)
    // In case of deferred rendering and interleaved shadow map rendering, the shadow map is at the same location in EDRAM
    // as the depth buffer, the deferred rendering system ensures that all shadow maps and lights are rendered in a correct order
    // and ensures that the depth buffer is restored after the light pass (see VDeferredShadingLights::Execute)
    if (m_bIsRenderedInterleaved)
    {
      int iWidth, iHeight;
      VisRenderContext_cl *pRefContext = m_pShadowComponent->GetRendererNode()->GetReferenceContext();
      pRefContext->GetSize(iWidth, iHeight);

      // Place after color buffer, exactly where the depth buffer is located
      config.m_iEdramOffset = XGSurfaceSize(iWidth, iHeight, 
        VTextureObject::ToD3DRenderTargetFormat(VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP), D3DMULTISAMPLE_NONE);
    }
    else
    {
      config.m_iEdramOffset = 0;
    }
  #endif

  #if defined (_VISION_WIIU)
    config.m_bResolve = true;
  #endif

  #if defined (_VR_GLES2)
    if (config.m_eFormat == VTextureLoader::UNKNOWN)
    {
      Vision::Error.Warning("Shadow mapping not supported on this device!");
      return false;
    }
  #endif

  #ifdef SUPPORTS_MANUAL_HIZSTENCIL_SETUP
    config.m_iHiZOffset = 0;
  #endif

  // In case of interleaved shadow map rendering, we only need as much different textures as there are unique shadow maps,
  // so we can use the scratch texture pool...
  if (bUseScratchTextureForDepthStencilTarget)
  {
    m_spShadowMapDepthStencil = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(config);
  }
  else
  {
    m_spShadowMapDepthStencil = Vision::TextureManager.CreateRenderableTexture("<ShadowMapDepthStencil>", config);
    m_spShadowMapDepthStencil->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  }

  #if defined (_VISION_PS3)
    m_spPatchedShadowMapDepthStencil = RenderingOptimizationHelpers_cl::CreateAndPatchDummyTexture(m_spShadowMapDepthStencil);
  #endif
#endif

  // The render loop is shared between the cascades.
  ANALYSIS_IGNORE_WARNING_ONCE(6211)
  VShadowMapRenderLoop *pShadowMapRenderLoop = new VShadowMapRenderLoop(this);
  const bool bFrontFacingShadows = m_pShadowComponent->GetFrontFacingShadows() ? true : false;

  // Set up remaining properties in Shadow Cascades
  for (int i=0; i<m_iNumParts; i++)
  {
    // Reset cascade
    m_pParts[i].Reset();
    m_pParts[i].SetShadowMapGenerator(this);

    // Create render context for cascade
    VisRenderContext_cl *pContext = new VisRenderContext_cl();
    pContext->SetName("ShadowmapGenerator");

    m_pParts[i].SetRenderContext(pContext);

    // Set up render context properties
    pContext->SetRenderFlags(VIS_RENDERCONTEXT_FLAG_NO_LIGHTS);    // All scene elements except lights, no masks/debug rendering, no occlusion query
    pContext->SetShaderFlags(VIS_RENDERSHADER_ALL_TEXTURE);
    pContext->SetRenderFilterMask(m_pShadowComponent->GetFilterBitmask());

    
#if defined (_VISION_PS3)
#pragma diag_push
#pragma diag_suppress=1787
#endif
    

    pContext->SetUsageHint((VisContextUsageHint_e)m_spShadowMapFormat->GetUsageHint());

    
#if defined (_VISION_PS3)
#pragma diag_pop
#endif
    

    pContext->SetLODReferenceContext(m_pRendererNode->GetReferenceContext());
    pContext->SetRenderLoop(pShadowMapRenderLoop);
    pContext->SetRenderingEnabled(m_bUpdateEnabled);
    pContext->SetUserData(this);

    // Set Render Targets to the shared shadow map / depth stencil map
#if defined(WIN32)
    VASSERT( VVideo::GetDXFeatureLevel() < D3D_FEATURE_LEVEL_10_0 || m_spShadowMap.GetPtr() == NULL );
    pContext->SetRenderTarget(0, m_spShadowMap);
#else
    pContext->SetRenderTarget(0, NULL);
#endif
    pContext->SetDepthStencilTarget(m_spShadowMapDepthStencil);

    // Set Context priority to ensure that the contexts are rendered in a defined order (important for
    // parallelization with visibility determination). Also, we need to process the context light source-wise to
    // ensure that the stencil buffer setup remains intact.
    pContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_SHADOWS + (float)m_pLightSource->GetNumber() + ((float)i / 6.f));
    
    // Create and set camera in cascade and context
    VisContextCamera_cl *pCam = new VisContextCamera_cl();
    m_pParts[i].SetCamera(pCam);
    pContext->SetCamera(pCam);
    if (m_pLightSource->GetType() != VIS_LIGHT_DIRECTED)
    {
      pCam->SetPosition(m_pLightSource->GetPosition());
    }
    pCam->ReComputeVisibility();

    // Finally, create a visibility collector
    VShadowmapVisibilityCollector *pVisCollector = new VShadowmapVisibilityCollector();
    pVisCollector->m_pSMGenerator = this;

    // If forward-facing geometry casts shadows, we can simply use the regular visibility determination algorithm.
    // Otherwise, we need to tell the visibility collector to ignore portals, because we might otherwise miss
    // critical geometry.
    if (bFrontFacingShadows)
      pVisCollector->SetBehaviorFlags(pVisCollector->GetBehaviorFlags() | VIS_VISCOLLECTOR_USEPORTALS);
    else
      pVisCollector->SetBehaviorFlags(pVisCollector->GetBehaviorFlags() & (~VIS_VISCOLLECTOR_USEPORTALS));

    pContext->SetVisibilityCollector(pVisCollector);

    // Set this cascade's viewport to the correct part of the texture
    int iPosX, iPosY, iWidth, iHeight;
    m_pParts[i].GetViewport(iPosX, iPosY, iWidth, iHeight);
    pContext->SetViewport(iPosX, iPosY, iWidth, iHeight);
    
#if defined (_VISION_PSP2)
    if (m_iNumParts > 1)
    {
      pContext->ForceDepthStencilSurfaceLoad(true);
    }
#endif

    // And register the context.
    m_pRendererNode->AddContext(pContext);
  }

  VASSERT(m_pRendererNode->GetReferenceContext() != NULL);
  m_pRendererNode->GetReferenceContext()->GetViewFrustum(m_MainViewFrustum);
  SetGeometryTypes(m_pShadowComponent->GetGeometryTypes());

  UpdateLightSourceProperties();

  InitProfiling();

  m_spShadowMapFormat->Initialize(this);

  int iStateGroupMask = STATEGROUP_ALL & ~STATEGROUP_DEPTHSTENCIL;
  m_spShadowMapFormat->GetTerrainFillTechnique()->GetShader(0)->m_cStateGroupMask = iStateGroupMask;
  m_spShadowMapFormat->GetOpaqueFillTechnique(false)->GetShader(0)->m_cStateGroupMask = iStateGroupMask;
  m_spShadowMapFormat->GetAlphaFillTechnique(false)->GetShader(0)->m_cStateGroupMask = iStateGroupMask;
  m_spShadowMapFormat->GetOpaqueFillTechnique(true)->GetShader(0)->m_cStateGroupMask = iStateGroupMask;
  m_spShadowMapFormat->GetAlphaFillTechnique(true)->GetShader(0)->m_cStateGroupMask = iStateGroupMask;

  UpdateFacing(m_spShadowMapFormat->GetTerrainFillTechnique(), bFrontFacingShadows);
  UpdateFacing(m_spShadowMapFormat->GetOpaqueFillTechnique(false), bFrontFacingShadows);
  UpdateFacing(m_spShadowMapFormat->GetAlphaFillTechnique(false), bFrontFacingShadows);

  m_spClearTechnique = Vision::Shaders.CreateTechnique("ClearShadowMap", NULL, NULL, EFFECTCREATEFLAG_FORCEUNIQUE);
  m_spClearTechnique->GetShader(0)->DisablePixelShader();
#if defined(_VR_DX11) || defined(_VISION_PS3) 
  m_spClearTechnique->GetShader(0)->GetRenderState ()->GetBlendState ().m_iRenderTargetWriteMask = 0;
  m_spClearTechnique->GetShader(0)->GetRenderState ()->GetBlendState ().ComputeHash();
#endif

  m_bIsInitialized = true;
  return true;
}

void VShadowMapGenerator::InitProfiling()
{
  if (!PROFILING_RENDER_SHADOWMAP)
  {
    PROFILING_RENDER_SHADOWMAP = Vision::Profiling.GetFreeElementID();
    PROFILING_POSTPROCESS_SHADOWMAP = Vision::Profiling.GetFreeElementID();
    PROFILING_SORT_SCENEELEMENTS = Vision::Profiling.GetFreeElementID();
    PROFILING_POSTFILER_SCENEELEMENTS = Vision::Profiling.GetFreeElementID();

    Vision::Profiling.AddGroup("Shadow Maps");
    VProfilingNode *pRenderMaps = Vision::Profiling.AddElement(PROFILING_RENDER_SHADOWMAP, "Render Shadow Maps", TRUE);
    Vision::Profiling.AddElement(PROFILING_POSTPROCESS_SHADOWMAP, "Postprocess Shadow Maps", TRUE, pRenderMaps);
    Vision::Profiling.AddElement(PROFILING_SORT_SCENEELEMENTS, "Sort Scene Elements", TRUE, pRenderMaps);
    Vision::Profiling.AddElement(PROFILING_POSTFILER_SCENEELEMENTS, "Post-filter Scene Elements", TRUE, pRenderMaps);
  }
}


void VShadowMapGenerator::SetupDebugScreenMask()
{
  // create a screen mask to view the pbuffer
  m_spDebugMask = new VisScreenMask_cl(); 
  m_spDebugMask->SetPos( 0.0f, 0.0f);

  m_spDebugMask->SetTextureObject(GetShadowMap());
 
  m_spDebugMask->SetTargetSize((float)384 * (m_spShadowMapDepthStencil->GetTextureWidth() / m_spShadowMapDepthStencil->GetTextureHeight()), (float)384);
  m_spDebugMask->SetVisible(TRUE);
}

void VShadowMapGenerator::UpdateLightShader(VShadowMapApplyShaderPass *pPass, hkvMat4 (&mLightProjection)[4], hkvVec4& vCascadeScale)
{
  VisContextCamera_cl* pCamera = m_pRendererNode->GetReferenceContext()->GetCamera();

  hkvMat3 mWorldToCameraRot = pCamera->GetWorldToCameraRotation ();

  pPass->SetLightPosition(GetPosition(), mWorldToCameraRot);
  pPass->SetLightDirection(GetDirection());
  pPass->SetTextureSize(m_iTextureSize[0], m_iTextureSize[1]);

  hkvVec4 vCascadeEnd;

  ComputeCascadeParams(mLightProjection, vCascadeScale, vCascadeEnd);

  pPass->SetCascadeParameters(vCascadeEnd, vCascadeScale);
  pPass->SetLightProjection(m_iNumParts, mLightProjection);
}

void VShadowMapGenerator::ComputeCascadeParams(hkvMat4 (&mLightProjection)[4], hkvVec4& vCascadeScale, hkvVec4& vCascadeEnd)
{
  VisContextCamera_cl* pCamera = m_pRendererNode->GetReferenceContext()->GetCamera();

  vCascadeEnd = hkvVec4(false);

  float fEnd0 = m_pParts[0].m_fCullDistance;

  VASSERT(m_iNumParts <= 4);
  for (int i=0; i<m_iNumParts; i++)
  {
    // Cascade Splits
    float fEnd = m_pParts[i].m_fCullDistance;
    vCascadeScale.data[i] = fEnd0 / fEnd;
    vCascadeEnd.data[i] = fEnd;
    
      //light projection matrix
    hkvMat4 mWorldToCam = m_pParts[i].GetCamera()->GetWorldToCameraTransformation();
    const hkvMat4 mTmp = m_pParts[i].GetRenderContext()->GetViewProperties()->getProjectionMatrix(hkvClipSpaceYRange::MinusOneToOne).multiply(mWorldToCam);
    mLightProjection[i] = m_pParts[i].m_mTexMatrix.multiply (mTmp);
  }

  for (int i=m_iNumParts; i<4; i++)
  {
    vCascadeScale.data[i] = 1.0f;
    vCascadeEnd.data[i] = FLT_MAX;
  }
}

void VShadowMapGenerator::UpdateLightSourceProperties()
{
  VASSERT(m_pLightSource != NULL);
  if (m_pLightSource->GetType()==VIS_LIGHT_DIRECTED)
    m_eProjectionType = SHADOW_PROJECTION_ORTHOGRAPHIC;
  else if (m_pLightSource->GetType() == VIS_LIGHT_SPOTLIGHT || m_pLightSource->GetType() == VIS_LIGHT_POINT)
    m_eProjectionType = SHADOW_PROJECTION_PERSPECTIVE;
  else
  {
    VASSERT(FALSE && "The current light source is not supported by this shadow map generator!");
  }

  for (int i = 0; i < m_iNumParts; i++)
  {
    VisRenderContext_cl *pContext = m_pParts[i].GetRenderContext();
    VASSERT(pContext != NULL);
    pContext->SetProjectionType(m_eProjectionType == SHADOW_PROJECTION_ORTHOGRAPHIC ? VIS_PROJECTIONTYPE_ORTHOGRAPHIC : VIS_PROJECTIONTYPE_PERSPECTIVE);
  }
}

void VShadowMapGenerator::SetLightSource(VisLightSource_cl *pLight)
{
  if (m_pLightSource != pLight)
  {
    m_pLightSource = pLight;
    UpdateLightSourceProperties();
  }
}

int VShadowMapGenerator::GetShadowMapSize() const 
{ 
  return m_pShadowComponent->GetShadowMapSize(); 
}

void VShadowMapGenerator::SetCascadeCount(int iNumParts)
{
  if (iNumParts > 1 && m_pLightSource->GetType() != VIS_LIGHT_DIRECTED)
  {
    Vision::Error.Warning("More than one cascade is only supported for directional lights. Forcing iNumCascades to 1!");
    iNumParts = 1;
  }
  m_iNumParts = iNumParts;
  m_bPropertiesDirty = true;
}

void VShadowMapGenerator::SetUpdateEnabled(bool bStatus)
{
  for (int i=0; i<m_iNumParts; i++)
  {
    VisRenderContext_cl *pContext = m_pParts[i].GetRenderContext();
    VASSERT(pContext != NULL);
    pContext->SetRenderingEnabled(bStatus);
  }
  m_bUpdateEnabled = bStatus;
}

void VShadowMapGenerator::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if ( (m_bRenderShadowVolumes || m_bShowProfilingData) && pData->m_pSender==&Vision::Callbacks.OnRenderHook)
  {
    VisRenderHookDataObject_cl *pRenderHookData = (VisRenderHookDataObject_cl *)pData;

    if (pRenderHookData->m_iEntryConst == VRH_CORONAS_AND_FLARES
      && Vision::Contexts.GetCurrentContext() == m_pRendererNode->GetTranslucencyReferenceContext())
    {
      static VColorRef iCascadeColors[4] = {V_RGBA_BLUE, V_RGBA_GREEN, V_RGBA_RED, V_RGBA_YELLOW};
      for (int i=0; i<m_iNumParts; i++)
      {
        iCascadeColors[i].a = 128;
        m_pParts[i].RenderFrustumMesh(iCascadeColors[i]);
      }
    }
    else if (pRenderHookData->m_iEntryConst == VRH_GUI)
    {
      if (m_bShowProfilingData)
        RenderProfilingData();
    }

    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneBegin)
  {
    if (m_bPropertiesDirty)
    {
      DeInitialize();
      Initialize();
      m_bPropertiesDirty = false;
    }
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    Update();
    return;
  }

  // Skip rendering in case of deferred rendering and interleaved shadow map rendering
  if (pData->m_pSender==&Vision::Callbacks.OnContextSwitching && m_bIsRenderedInterleaved)
  {
	  VisContextDataObject_cl *pContextData = (VisContextDataObject_cl *)pData;
	  for (int i=0; i<m_iNumParts; i++)
	  {
		  if (m_pParts[i].GetRenderContext() == pContextData->m_pContext)
		  {
			  pContextData->m_bSkipRendering = true;
			  return;
		  }
	  }
  }

}

void VShadowMapGenerator::GetDepthFillShaderConstantValues(int iCascade, float &fDepthBias, float &fSlopScaleDepthBias, hkvVec3& vClipPlanes)
{
  VisRenderContext_cl *pContext = m_pParts[iCascade].GetRenderContext();
  float fNear, fFar;
  pContext->GetClipPlanes(fNear, fFar);

  const float fCascadeBias = m_pShadowComponent->GetBias(iCascade);
  const float fCascadeSlopeScale = m_pShadowComponent->GetSlopeScaled(iCascade);

  if (m_eProjectionType == SHADOW_PROJECTION_ORTHOGRAPHIC)
  {
    vClipPlanes.x = 0.0f;
    vClipPlanes.y = 1.0f;
    vClipPlanes.z = 1.0f;
  }
  else
  {
    vClipPlanes.x = fNear;
    vClipPlanes.y = fFar;
    vClipPlanes.z = 1.0f / (fFar - fNear);
  }

  fDepthBias = fCascadeBias / (fFar-fNear);
  fSlopScaleDepthBias = fCascadeSlopeScale;
}

void VShadowMapGenerator::UpdateDepthFillTechniques(int iCascade)
{
  VASSERT(m_spShadowMapFormat->GetTerrainFillTechnique()->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));
  VASSERT(m_spShadowMapFormat->GetOpaqueFillTechnique(false)->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));
  VASSERT(m_spShadowMapFormat->GetAlphaFillTechnique(false)->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));
  VASSERT(m_spShadowMapFormat->GetOpaqueFillTechnique(true)->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));
  VASSERT(m_spShadowMapFormat->GetAlphaFillTechnique(true)->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));

  float fBias, fSlopeScale;
  hkvVec3 vClipPlanes(hkvNoInitialization);
  GetDepthFillShaderConstantValues(iCascade, fBias, fSlopeScale, vClipPlanes);
  
  VShadowMapFillShaderPass* pPass[5];
  pPass[0] = (VShadowMapFillShaderPass*)m_spShadowMapFormat->GetTerrainFillTechnique()->GetShader(0);
  pPass[1] = (VShadowMapFillShaderPass*)m_spShadowMapFormat->GetOpaqueFillTechnique(false)->GetShader(0);
  pPass[2] = (VShadowMapFillShaderPass*)m_spShadowMapFormat->GetAlphaFillTechnique(false)->GetShader(0);
  pPass[3] = (VShadowMapFillShaderPass*)m_spShadowMapFormat->GetOpaqueFillTechnique(true)->GetShader(0);
  pPass[4] = (VShadowMapFillShaderPass*)m_spShadowMapFormat->GetAlphaFillTechnique(true)->GetShader(0);

  for (int i = 0; i < 5; i++)
  {
    pPass[i]->SetDepthBias(fBias, fSlopeScale, m_pLightSource->GetType() == VIS_LIGHT_DIRECTED);
    pPass[i]->SetClipPlanes(vClipPlanes.x, vClipPlanes.y, vClipPlanes.z);
  }
   
  for (int i = 0; i < 3; i++)
  {
    // Updating cull mode because the shadow technique is shared before to render.
    pPass[i]->GetRenderState()->GetRasterizerState().m_cCullMode = m_pShadowComponent->GetFrontFacingShadows() ? CULL_BACK : CULL_FRONT; 
    pPass[i]->GetRenderState()->GetRasterizerState().ComputeHash();
    pPass[i]->m_bModified = true;
  }

  if (m_pParts[iCascade].m_sgDepthStencil != NULL)
  {
    VisRenderStates_cl::SetDepthStencilState (*m_pParts[iCascade].m_sgDepthStencil);
  }
  else
  {
    VisRenderStates_cl::SetDepthStencilState (pPass[1]->GetRenderState ()->GetDepthStencilState ());
  }
}

void VShadowMapGenerator::PreProcess()
{
  int iOldX, iOldY, iOldW, iOldH;
  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
  pContext->GetViewport(iOldX, iOldY, iOldW, iOldH);

  pContext->SetViewport(0, 0, m_iTextureSize[0], m_iTextureSize[1]);
  Vision::RenderLoopHelper.SetScissorRect(NULL);
  VColorRef iClearColor = m_spShadowMapFormat->GetClearColor();

#if defined (_VISION_XENON)  
  if (m_bIsRenderedInterleaved)
  {
    // In case of interleaved rendering we only clear the depth, otherwise we would destroy the stencil buffer
    Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_Depth, iClearColor);
  }
  else
#endif
  {
    Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, iClearColor);
  }

  pContext->SetViewport(iOldX, iOldY, iOldW, iOldH);
}

int VShadowMapGenerator::GetCascadeIndexFromRenderContext(VisRenderContext_cl *pContext) const
{
  for (int i=0; i<m_iNumParts; i++)
  {
    if (m_pParts[i].GetRenderContext() == pContext)
      return i;
  }

  VASSERT(FALSE && "GetCascadeIndexFromRenderContext: Render Context not found in any of the Parts!");
  return -1;
}

void VShadowMapGenerator::SetGeometryTypes(int iFlags)
{
  for (int i=0; i<m_iNumParts; i++)
  {
    VisRenderContext_cl *pContext = m_pParts[i].GetRenderContext();
    VASSERT(pContext != NULL);
    int iRenderFlags = VIS_RENDERCONTEXT_FLAG_NO_LIGHTS;
    if (!(iFlags & SHADOW_CASTER_ENTITIES))
      iRenderFlags |= VIS_RENDERCONTEXT_FLAG_NO_ENTITIES;
    if (!(iFlags & (SHADOW_CASTER_TERRAIN | SHADOW_CASTER_STATICMESHES)))
      iRenderFlags |= VIS_RENDERCONTEXT_FLAG_NO_WORLDGEOM;
    pContext->SetRenderFlags(iRenderFlags);   
  }
}

void VShadowMapGenerator::SetFilterBitmask(int iMask)
{
  for (int i=0; i<m_iNumParts; i++)
  {
    VisRenderContext_cl *pContext = m_pParts[i].GetRenderContext();
    VASSERT(pContext != NULL);
    pContext->SetRenderFilterMask(iMask);
  }
}

bool VShadowMapGenerator::AreAllCascadeRendered() const
{
  for (int i=0; i<m_iNumParts; i++)
    if (! (m_iPartsRendered & (1<<i)))
      return false;
  return true;
}

void VShadowMapGenerator::DrawTextHelper(IVRenderInterface* pRi, float x, float y, const char* msg, ...)
{
  char msgBuffer[4096];
  va_list args;
  va_start(args, msg);
  vsprintf(msgBuffer, msg, args);
  va_end(args);
  pRi->DrawText2D((float)x,(float)y, msgBuffer, V_RGBA_WHITE);
  y += 12;
}

void VShadowMapGenerator::RenderProfilingData()
{
  IVRenderInterface *pRI = Vision::Contexts.GetCurrentContext()->GetRenderInterface();

  DrawTextHelper(pRI, 10.f, 10.f, "VShadowMapGeneratorBase: Statistics");
  for (int i=0; i<m_iNumParts; i++)
  {
    DrawTextHelper(pRI, 16.f, 26.f + i*56.f, "Cascade #%i:", i);
    DrawTextHelper(pRI, 16.f, 42.f + i*56.f, "Entities (post-visibility): %i, Rendered: %i, Culled %i, Post-visibility Culling Rate %.1f%",
      m_ShadowProfilingData.iEntitiesPassedToRenderLoop[i], m_ShadowProfilingData.iEntitiesRendered[i],
      m_ShadowProfilingData.iEntitiesPassedToRenderLoop[i] - m_ShadowProfilingData.iEntitiesRendered[i],
      ((float)(m_ShadowProfilingData.iEntitiesPassedToRenderLoop[i] - m_ShadowProfilingData.iEntitiesRendered[i])/(float)m_ShadowProfilingData.iEntitiesPassedToRenderLoop[i])*100.f);
    DrawTextHelper(pRI, 16.f, 58.f + i*56, "StaticGIs (post-visibility): %i, Rendered: %i, Culled %i, Post-visibility Culling Rate %.1f%",
      m_ShadowProfilingData.iStaticGIPassedToRenderLoop[i], m_ShadowProfilingData.iStaticGIRendered[i],
      m_ShadowProfilingData.iStaticGIPassedToRenderLoop[i] - m_ShadowProfilingData.iStaticGIRendered[i],
      ((float)(m_ShadowProfilingData.iStaticGIPassedToRenderLoop[i] - m_ShadowProfilingData.iStaticGIRendered[i])/(float)m_ShadowProfilingData.iStaticGIPassedToRenderLoop[i])*100.f);
  }
}

void VShadowMapGenerator::UpdateFacing(VCompiledTechnique *pTech, bool bFrontFace)
{
  pTech->GetShader(0)->GetRenderState()->GetRasterizerState().m_cCullMode = bFrontFace ? CULL_BACK : CULL_FRONT;
  pTech->GetShader(0)->GetRenderState()->GetRasterizerState().ComputeHash();
}

void VShadowMapGenerator::SetFrontFacesCastShadows(bool bStatus)
{
  UpdateFacing(m_spShadowMapFormat->GetTerrainFillTechnique(), bStatus);
  UpdateFacing(m_spShadowMapFormat->GetOpaqueFillTechnique(false), bStatus);
  UpdateFacing(m_spShadowMapFormat->GetAlphaFillTechnique(false), bStatus);

  // If forward-facing geometry casts shadows, we can simply use the regular visibility determination algorithm.
  // Otherwise, we need to tell the visibility collector to ignore portals, because we might otherwise miss
  // critical geometry.
  for (int i=0; i<m_iNumParts; i++)
  {
    VisRenderContext_cl *pContext = m_pParts[i].GetRenderContext();
    VASSERT(pContext->GetVisibilityCollector()->IsOfType(V_RUNTIME_CLASS(VisionVisibilityCollector_cl)));
    VisionVisibilityCollector_cl *pVisCollector = vstatic_cast<VisionVisibilityCollector_cl *>(pContext->GetVisibilityCollector());
    if (bStatus)
      pVisCollector->SetBehaviorFlags(pVisCollector->GetBehaviorFlags() | VIS_VISCOLLECTOR_USEPORTALS);
    else
      pVisCollector->SetBehaviorFlags(pVisCollector->GetBehaviorFlags() & (~VIS_VISCOLLECTOR_USEPORTALS));
  }
}


// ================================================================================
// VShadowMapFormatDepthOnly
// ================================================================================

VShadowMapFormatDepthOnly::VShadowMapFormatDepthOnly()
{
}

VShadowMapFormatDepthOnly::~VShadowMapFormatDepthOnly()
{
 
}

void VShadowMapFormatDepthOnly::Initialize(VShadowMapGenerator* pGenerator)
{
  IVShadowMapFormat::Initialize(pGenerator);

#if defined( _VR_DX11 )
  const char* szShaderFileName = VVideo::GetDXFeatureLevel() < D3D_FEATURE_LEVEL_10_0 ? "\\Shaders/ShadowMapShadersDX11L93.ShaderLib" : "\\Shaders\\ShadowMapShaders.ShaderLib";
#else
  const char* szShaderFileName = "\\Shaders\\ShadowMapShaders.ShaderLib";
#endif
  if (!Vision::Shaders.LoadShaderLibrary(szShaderFileName, SHADERLIBFLAG_HIDDEN))
  {
    Vision::Error.Warning("Shader lib file for shadowing could not be loaded (file '%s')", szShaderFileName);
    return;
  }

  m_spTerrainFillTechnique = Vision::Shaders.CreateTechnique("FillTerrainDepth", NULL, NULL, EFFECTCREATEFLAG_FORCEUNIQUE);
  m_spOpaqueFillTechnique = Vision::Shaders.CreateTechnique("FillOpaqueDepth", NULL, NULL, EFFECTCREATEFLAG_FORCEUNIQUE);
  m_spAlphatestFillTechnique = Vision::Shaders.CreateTechnique("FillAlphatestDepth", NULL, NULL, EFFECTCREATEFLAG_FORCEUNIQUE);
  m_spOpaqueFillTechniqueDoubleSided = Vision::Shaders.CreateTechnique("FillOpaqueDepthDoubleSided", NULL, NULL, EFFECTCREATEFLAG_FORCEUNIQUE);
  m_spAlphatestFillTechniqueDoubleSided = Vision::Shaders.CreateTechnique("FillAlphatestDepthDoubleSided", NULL, NULL, EFFECTCREATEFLAG_FORCEUNIQUE);

  VASSERT(m_spTerrainFillTechnique);
  VASSERT(m_spOpaqueFillTechnique);
  VASSERT(m_spAlphatestFillTechnique);
  VASSERT(m_spOpaqueFillTechniqueDoubleSided);
  VASSERT(m_spAlphatestFillTechniqueDoubleSided);
  
  VASSERT(m_spTerrainFillTechnique->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));
  VASSERT(m_spOpaqueFillTechnique->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));
  VASSERT(m_spAlphatestFillTechnique->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));
  VASSERT(m_spOpaqueFillTechniqueDoubleSided->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));
  VASSERT(m_spAlphatestFillTechniqueDoubleSided->GetShader(0)->IsOfType(VShadowMapFillShaderPass::GetClassTypeId()));

  // Note: disabling of a pixel shader generally works on WiiU, but using the clip instruction inside the pixel shader as done by the alpha-test
  //       fill techniques causes problems on the WiiU, because the alpha-test pixel shader would be still active after it has been used once.
  //       Thus we do not disable the pixel shader for opaque fill techniques to ensure that a valid shader is bound (without alpha test).
#if !defined(_VISION_WIIU)
  m_spTerrainFillTechnique->GetShader(0)->DisablePixelShader();
  m_spOpaqueFillTechnique->GetShader(0)->DisablePixelShader();
  m_spOpaqueFillTechniqueDoubleSided->GetShader(0)->DisablePixelShader();
#endif

#if defined(_VISION_PSP2)  || defined(_VISION_PS3) 
  m_spTerrainFillTechnique->GetShader(0)->GetRenderState ()->GetBlendState ().m_iRenderTargetWriteMask = 0;
  m_spTerrainFillTechnique->GetShader(0)->GetRenderState ()->GetBlendState ().ComputeHash();
  m_spOpaqueFillTechnique->GetShader(0)->GetRenderState ()->GetBlendState ().m_iRenderTargetWriteMask = 0;
  m_spOpaqueFillTechnique->GetShader(0)->GetRenderState ()->GetBlendState ().ComputeHash();
#endif
}

VTextureLoader::VTextureFormat_e VShadowMapFormatDepthOnly::GetShadowMapFormat()
{
#if defined( _VR_DX11 )
  if (VVideo::GetDXFeatureLevel() >= D3D_FEATURE_LEVEL_10_0)
    return VTextureLoader::NONE;
  else
    return VTextureLoader::R32F;
#elif defined(_VISION_XENON)  || defined(_VISION_PS3)  || defined(_VISION_PSP2) || defined(_VISION_WIIU)
  return VTextureLoader::NONE;
#else
  return VTextureLoader::R32F;
#endif
}

VTextureLoader::VTextureFormat_e VShadowMapFormatDepthOnly::GetShadowMapFormatDepthStencil()
{
#if defined(_VR_GLES2)
  return VTextureObject::GetCompatibleDepthTextureFormat(*Vision::Video.GetCurrentConfig());
#else
  return VTextureLoader::D24S8;
#endif
}

int VShadowMapFormatDepthOnly::GetUsageHint() const
{
  return VIS_CONTEXTUSAGE_DEPTHSHADOW | VIS_CONTEXTUSAGEFLAG_DEPTHSTENCILONLY; 
}

void VShadowMapFormatDepthOnly::PostProcess()
{
#if defined (_VISION_XENON) || defined(_VISION_WIIU)
  m_pGenerator->GetShadowMapDepthStencil()->Resolve();
#endif
}


// ================================================================================
// VShadowCascade
// ================================================================================

VShadowMapPart::VShadowMapPart()
{
  Reset();
}

VShadowMapPart::~VShadowMapPart()
{
}

void VShadowMapPart::Reset()
{
  m_spRenderContext = NULL;
  m_pShadowMapGenerator = NULL;

  m_fCenterOffset = 0.0f;
  m_fRadius = 0.0f;
  m_fCullDistance = 0.0f;
  m_sgDepthStencil = NULL;
}


void VShadowMapPart::GetViewport(int &iPosX, int &iPosY, int &iWidth, int &iHeight) const 
{
  int iTextureSize[2]; 
  m_pShadowMapGenerator->GetTextureSize(iTextureSize[0], iTextureSize[1]);
  int iShadowMapSize = m_pShadowMapGenerator->GetShadowMapSize();
  iWidth  = (int) (iTextureSize[0] *  m_mTexMatrix.m_Column[0][0] * 2.0f);
  iHeight = (int) (iTextureSize[1] * -m_mTexMatrix.m_Column[1][1] * 2.0f);
  iPosX   = (int) (iTextureSize[0] * (m_mTexMatrix.m_Column[3][0] - m_mTexMatrix.m_Column[0][0]));
  iPosY   = (int) (iTextureSize[1] * (m_mTexMatrix.m_Column[1][1] + m_mTexMatrix.m_Column[3][1]));
}

//static
void VShadowMapPart::GetViewFrustumCornersAtDistance(const VisRenderContext_cl *pContext, float fDistance, hkvVec3* pVectors)
{
  int iWidth, iHeight;
  pContext->GetSize(iWidth, iHeight);
  float fFovX, fFovY;
  pContext->GetFinalFOV(fFovX, fFovY);

  float fHalfWidth = hkvMath::tanDeg (fFovX * 0.5f) * fDistance;
  float fHalfHeight = hkvMath::tanDeg (fFovY * 0.5f) * fDistance;

  VisContextCamera_cl *pCam = pContext->GetCamera();
  const hkvVec3 vCamPos = pCam->GetPosition();
  pVectors[0] = vCamPos + (pCam->GetDirection()*fDistance) - pCam->GetObjDir_Right()*fHalfWidth - pCam->GetObjDir_Up()*fHalfHeight;
  pVectors[1] = vCamPos + (pCam->GetDirection()*fDistance) - pCam->GetObjDir_Right()*fHalfWidth + pCam->GetObjDir_Up()*fHalfHeight;
  pVectors[2] = vCamPos + (pCam->GetDirection()*fDistance) + pCam->GetObjDir_Right()*fHalfWidth + pCam->GetObjDir_Up()*fHalfHeight;
  pVectors[3] = vCamPos + (pCam->GetDirection()*fDistance) + pCam->GetObjDir_Right()*fHalfWidth - pCam->GetObjDir_Up()*fHalfHeight;
}

void VShadowMapPart::RenderFrustumMesh(VColorRef iColor)
{
  VSimpleRenderState_t iRenderState(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_DOUBLESIDED);
  IVRenderInterface *pRI = Vision::Contexts.GetCurrentContext()->GetRenderInterface();
  pRI->DrawTriangle(m_vFrustumMesh[0], m_vFrustumMesh[4], m_vFrustumMesh[5], iColor * 0.7f, iRenderState);
  pRI->DrawTriangle(m_vFrustumMesh[0], m_vFrustumMesh[5], m_vFrustumMesh[1], iColor * 0.7f, iRenderState);
  pRI->DrawTriangle(m_vFrustumMesh[1], m_vFrustumMesh[5], m_vFrustumMesh[6], iColor * 0.9f, iRenderState);
  pRI->DrawTriangle(m_vFrustumMesh[1], m_vFrustumMesh[6], m_vFrustumMesh[2], iColor * 0.9f, iRenderState);
  pRI->DrawTriangle(m_vFrustumMesh[2], m_vFrustumMesh[6], m_vFrustumMesh[7], iColor * 0.8f, iRenderState);
  pRI->DrawTriangle(m_vFrustumMesh[2], m_vFrustumMesh[7], m_vFrustumMesh[3], iColor * 0.8f, iRenderState);
  pRI->DrawTriangle(m_vFrustumMesh[3], m_vFrustumMesh[7], m_vFrustumMesh[4], iColor, iRenderState);
  pRI->DrawTriangle(m_vFrustumMesh[3], m_vFrustumMesh[4], m_vFrustumMesh[0], iColor, iRenderState);
}

void VShadowMapPart::ComputeFrustumMesh()
{
  VisContextCamera_cl *pCam = m_spRenderContext->GetCamera();

  hkvVec3 vCamPos = pCam->GetPosition();
  hkvVec3 vDirection = pCam->GetDirection();
  hkvVec3 vDirectionRight = pCam->GetObjDir_Right();
  hkvVec3 vDirectionUp = pCam->GetObjDir_Up();

  hkvVec3 vFrustumMesh[8];

  if (m_spRenderContext->GetProjectionType() == VIS_PROJECTIONTYPE_ORTHOGRAPHIC)
  {
    float fOrthoWidth, fOrthoHeight;
    m_spRenderContext->GetOrthographicSize(fOrthoWidth, fOrthoHeight);
    fOrthoHeight *= 0.5f;
    fOrthoWidth *= 0.5f;

    float fNear, fFar;
    m_spRenderContext->GetClipPlanes(fNear, fFar);
    hkvVec3 vStartPos = vCamPos + vDirection * fNear;
    hkvVec3 vEndPos = vCamPos + vDirection * fFar;

    vFrustumMesh[0] = vStartPos - vDirectionRight * fOrthoWidth - vDirectionUp * fOrthoHeight;
    vFrustumMesh[1] = vStartPos - vDirectionRight * fOrthoWidth + vDirectionUp * fOrthoHeight;
    vFrustumMesh[2] = vStartPos + vDirectionRight * fOrthoWidth + vDirectionUp * fOrthoHeight;
    vFrustumMesh[3] = vStartPos + vDirectionRight * fOrthoWidth - vDirectionUp * fOrthoHeight;

    vFrustumMesh[4] = vEndPos - vDirectionRight * fOrthoWidth - vDirectionUp * fOrthoHeight;
    vFrustumMesh[5] = vEndPos - vDirectionRight * fOrthoWidth + vDirectionUp * fOrthoHeight;
    vFrustumMesh[6] = vEndPos + vDirectionRight * fOrthoWidth + vDirectionUp * fOrthoHeight;
    vFrustumMesh[7] = vEndPos + vDirectionRight * fOrthoWidth - vDirectionUp * fOrthoHeight;
  }
  if (m_spRenderContext->GetProjectionType() == VIS_PROJECTIONTYPE_PERSPECTIVE)
  {
    float fNear, fFar;
    m_spRenderContext->GetClipPlanes(fNear, fFar);

    float fFovX, fFovY;
    m_spRenderContext->GetFinalFOV(fFovX, fFovY);
    float fStartWidth = fNear * hkvMath::tanDeg (fFovX * 0.5f);
    float fStartHeight = fNear * hkvMath::tanDeg (fFovY * 0.5f);
    float fEndWidth = fFar * hkvMath::tanDeg (fFovX * 0.5f);
    float fEndHeight = fFar * hkvMath::tanDeg (fFovY * 0.5f);

    vFrustumMesh[0] = vCamPos - vDirectionRight * fStartWidth - vDirectionUp * fStartHeight + vDirection * fNear;
    vFrustumMesh[1] = vCamPos - vDirectionRight * fStartWidth + vDirectionUp * fStartHeight + vDirection * fNear;
    vFrustumMesh[2] = vCamPos + vDirectionRight * fStartWidth + vDirectionUp * fStartHeight + vDirection * fNear;
    vFrustumMesh[3] = vCamPos + vDirectionRight * fStartWidth - vDirectionUp * fStartHeight + vDirection * fNear;

    vFrustumMesh[4] = vCamPos - vDirectionRight * fEndWidth - vDirectionUp * fEndHeight + vDirection * fFar;
    vFrustumMesh[5] = vCamPos - vDirectionRight * fEndWidth + vDirectionUp * fEndHeight + vDirection * fFar;
    vFrustumMesh[6] = vCamPos + vDirectionRight * fEndWidth + vDirectionUp * fEndHeight + vDirection * fFar;
    vFrustumMesh[7] = vCamPos + vDirectionRight * fEndWidth - vDirectionUp * fEndHeight + vDirection * fFar;
  }

  for (int i=0; i<8; i++)
    m_vFrustumMesh[i].set(vFrustumMesh[i].x, vFrustumMesh[i].y, vFrustumMesh[i].z);
}


void VShadowMapPart::Update()
{
  VASSERT(m_pShadowMapGenerator != NULL);

  IVisVisibilityCollector_cl *pVisCollector = m_spRenderContext->GetVisibilityCollector();

  if (pVisCollector->IsOfType(V_RUNTIME_CLASS(VisionVisibilityCollector_cl)))
  {
    VisVisibilityZone_cl *pCameraZone = m_pShadowMapGenerator->GetRendererNode()->GetReferenceContext()->GetCamera()->GetVisData()->GetVisibilityZoneOfOrigin();
    ((VisionVisibilityCollector_cl*)pVisCollector)->SetOverrideStartZone(pCameraZone);
  }

  if (m_pShadowMapGenerator->GetRenderShadowVolumes())
  {
    ComputeFrustumMesh();
  }
}

void VShadowMapPart::ComputeOffset(float fStartDistance, float fEndDistance)
{
  if (fStartDistance >= fEndDistance)
    fEndDistance = fStartDistance + 1.0f;

  float fFovX, fFovY;
  m_pShadowMapGenerator->GetRendererNode()->GetReferenceContext()->GetFinalFOV(fFovX, fFovY);
  float tanX = hkvMath::tanDeg (fFovX * 0.5f);
  float tanY = hkvMath::tanDeg (fFovY * 0.5f);
 
  hkvVec3 nearPoint(tanX * fStartDistance, tanY * fStartDistance, fStartDistance);
  hkvVec3 farPoint(tanX * fEndDistance, tanY * fEndDistance, fEndDistance);
  m_fCullDistance = farPoint.getLength();

  m_fCenterOffset = (farPoint.getLengthSquared () - nearPoint.getLengthSquared ()) / (2.0f * (farPoint.z - nearPoint.z));
  if (m_fCenterOffset > fEndDistance)
  {
    m_fCenterOffset = fEndDistance;
  }
  float fDistToFarCorner = hkvMath::sqrt (farPoint.x * farPoint.x + farPoint.y * farPoint.y + (farPoint.z - m_fCenterOffset) * (farPoint.z - m_fCenterOffset));
  //float fDistToNearCorner = hkvMath::sqrt (nearPoint.x * nearPoint.x + nearPoint.y * nearPoint.y + (nearPoint.z - m_fCenterOffset) * (nearPoint.z - m_fCenterOffset));
  m_fRadius = fDistToFarCorner;
}

// ================================================================================
// VShadowMapFillShaderPass
// ================================================================================

V_IMPLEMENT_SERIAL( VShadowMapFillShaderPass, VCompiledShaderPass, 0, &g_VisionEngineModule );


void VShadowMapFillShaderPass::PostCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader)
{
  VCompiledShaderPass::PostCompileFunction(pSourceFX, pSourceShader);

  m_RegDepthBias.Init(this, "DepthBias");
  if (!m_RegDepthBias.IsValid())
    m_RegDepthBias.Init(this, "g_DepthBias");

  m_RegClipPlanes.Init(this, "ClipPlanes");
  if (!m_RegClipPlanes.IsValid())
    m_RegClipPlanes.Init(this, "g_ShadowClipPlanes");

  m_cStateGroupMask &= ~STATEGROUP_DEPTHSTENCIL;
}


// ================================================================================
// VShadowMapApplyShaderPass
// ================================================================================

V_IMPLEMENT_DYNCREATE( VShadowMapApplyShaderPass, VCompiledShaderPass, &g_VisionEngineModule );


void VShadowMapApplyShaderPass::PostCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader)
{
  VCompiledShaderPass::PostCompileFunction(pSourceFX, pSourceShader);

  // Properties consistent across all cascades
  m_RegInvTexSize.Init(this, "InvShadowMapSize");
  m_RegLightWorldSpacePos.Init(this, "LightWorldSpace");
  m_RegLightEyeSpacePos.Init(this, "LightEyeSpace");
  m_RegLightDirection.Init(this, "LightDirection");
  m_RegFadeoutParams.Init(this, "FadeoutParameters");

  // Each of the following regs must be array[4] - one entry per cascade!
  m_RegCascadeScale.Init(this, "CascadeScale");
  m_RegCascadeSplitsEnd.Init(this, "CascadeTextureSplitsEnd");
  m_RegLightProjection.Init(this, "LightProjectionTex");

  // Shadow Map
  m_RegShadowMap.Init(this, "ShadowMap");
}


// ================================================================================
// VShadowMapRenderLoop
// ================================================================================

V_IMPLEMENT_DYNAMIC(VShadowMapRenderLoop,VisTypedEngineObject_cl, &g_VisionEngineModule);


VShadowMapRenderLoop::VShadowMapRenderLoop(VShadowMapGenerator *pShadowMapGenerator)
  : m_GICollection(512, 256), m_EntityCollection(512, 256), m_AlphaGICollection(512, 256),
    m_OpaqueGICollection(512, 256), 
    m_AlphaGICollectionDoubleSided(512, 256), 
    m_OpaqueGICollectionDoubleSided(512, 256),
    m_TerrainGICollection(512, 256),
    m_MixedEntityCollection(512, 256), m_AlphaEntityCollection(512, 256),
    m_OpaqueEntityCollection(512,256)
{
  VASSERT(pShadowMapGenerator != NULL);
  m_pGenerator = pShadowMapGenerator;
}

void VShadowMapRenderLoop::OnDoRenderLoop(void *pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VShadowMapRenderLoop::OnDoRenderLoop");
  VISION_PROFILE_FUNCTION(VShadowMapGenerator::PROFILING_RENDER_SHADOWMAP);

  VisRenderableTexture_cl* renderTarget = static_cast<VisRenderableTexture_cl*>(m_pGenerator->GetShadowMap());

  // Reset GPR allocation etc.
  RenderingOptimizationHelpers_cl::SetShaderPreference(0);

  // Some platforms do not supported to NOT set a pixel shader (=NULL), thus we have to set a valid shader initially
  VisRenderStates_cl::SetDepthFillShader();

  VisEntityCollection_cl* pShadowCasterEntities = &m_pGenerator->GetShadowCasterEntities();

  if (m_pGenerator->AreNoCascadeRendered())
  {
    m_pGenerator->PreProcess();
    pShadowCasterEntities->Clear();

    // reset bounding box for shadow casting geometry
    m_ShadowCasterBBox.setInvalid();
  } 

  int iCurrentCascadeIndex = m_pGenerator->GetCascadeIndexFromRenderContext(VisRenderContext_cl::GetCurrentContext());
  m_pGenerator->UpdateDepthFillTechniques(iCurrentCascadeIndex);
  VCompiledTechnique *pSolidFillTechnique = m_pGenerator->GetOpaqueFillTechnique(false);
  VCompiledTechnique *pAlphaFillTechnique = m_pGenerator->GetAlphaFillTechnique(false);
  VCompiledTechnique *pSolidFillTechniqueDoubleSided = m_pGenerator->GetOpaqueFillTechnique(true);
  VCompiledTechnique *pAlphaFillTechniqueDoubleSided = m_pGenerator->GetAlphaFillTechnique(true);
  VCompiledTechnique *pTerrainFillTechnique = m_pGenerator->GetTerrainFillTechnique();

  IVisVisibilityCollector_cl *pVisColl = VisRenderContext_cl::GetCurrentContext()->GetVisibilityCollector();
  VASSERT(pVisColl != NULL);

  const VisEntityCollection_cl* pEntities = pVisColl->GetVisibleEntities();
  const VisStaticGeometryInstanceCollection_cl *pStaticGI = pVisColl->GetVisibleStaticGeometryInstances();
  
  const int iGeomTypes = m_pGenerator->m_pShadowComponent->GetGeometryTypes();
  const bool bIncludeStaticGeometry = (iGeomTypes&(SHADOW_CASTER_STATICMESHES|SHADOW_CASTER_TERRAIN))!=0;
  const bool bUseSurfaceSpecificShaders = m_pGenerator->m_pShadowComponent->GetUseSurfaceSpecificShadowShaders() ? true : false;

  if (m_pGenerator->GetShowProfilingData())
  {
    m_pGenerator->m_ShadowProfilingData.iStaticGIPassedToRenderLoop[iCurrentCascadeIndex] = pStaticGI->GetNumEntries();
    m_pGenerator->m_ShadowProfilingData.iEntitiesPassedToRenderLoop[iCurrentCascadeIndex] = pEntities->GetNumEntries();
  }

  pEntities = PostFilterByViewFrustum(iCurrentCascadeIndex, pEntities);
  if (bIncludeStaticGeometry)
    pStaticGI = PostFilterByViewFrustum(iCurrentCascadeIndex, pStaticGI);

  // Sort by renderstate to increase efficiency
  SplitByRenderState(pStaticGI);  

  const VisEntityCollection_cl *pMixedEntityCollection = pEntities;
  if (!bUseSurfaceSpecificShaders)
  {
    SplitByRenderState(pEntities);
    pMixedEntityCollection = &m_MixedEntityCollection;
  }

  const VisLightSource_cl *pLightSource = m_pGenerator->GetLightSource();
  VASSERT(pLightSource != NULL);
  const bool bSetScissor = pLightSource->GetType()==VIS_LIGHT_SPOTLIGHT && !pLightSource->IsDynamic();

  // In case of subtractive shadows for spot lights ensure via scissor testing that corresponding shadow map has a 1 pixel border. 
  // In combination with clamping shadow map texture reads to edges, artifacts can be prevented that are caused by sampling outside 
  // the shadow map.
  if (bSetScissor)
  {
    const VShadowMapPart& shadowMapPart = m_pGenerator->GetCascadeInfo(iCurrentCascadeIndex);
    int iPosX, iPosY, iWidth, iHeight;
    shadowMapPart.GetViewport(iPosX, iPosY, iWidth, iHeight);
    VRectanglef clipRect(1.0f,1.0f,float(iWidth-2),float(iHeight-2));
    Vision::RenderLoopHelper.SetScissorRect(&clipRect);
  }
 
  // Render entities  
  Vision::RenderLoopHelper.BeginEntityRendering();

  if (!bUseSurfaceSpecificShaders)
  {
    Vision::RenderLoopHelper.RenderEntitiesWithShader(m_OpaqueEntityCollection, *pSolidFillTechnique->m_Shaders.GetAt(0));
    Vision::RenderLoopHelper.RenderEntitiesWithShader(m_AlphaEntityCollection, *pAlphaFillTechnique->m_Shaders.GetAt(0));
    
    int iNumEntities = m_OpaqueEntityCollection.GetNumEntries();
    VisBaseEntity_cl **ppEntities = m_OpaqueEntityCollection.GetDataPtr();
    for (int i=0; i<iNumEntities; i++, ppEntities++)
      if ((*ppEntities) != NULL)
        pShadowCasterEntities->AppendEntry(*ppEntities);

    iNumEntities = m_AlphaEntityCollection.GetNumEntries();
    ppEntities = m_AlphaEntityCollection.GetDataPtr();
    for (int i=0; i<iNumEntities; i++, ppEntities++)
      if ((*ppEntities) != NULL)
        pShadowCasterEntities->AppendEntry(*ppEntities);
  }
  
  int iMixedCount = pMixedEntityCollection->GetNumEntries();
  for (int i=0; i<iMixedCount; i++)
  {
    VisBaseEntity_cl *pEnt = pMixedEntityCollection->GetEntry(i);
    VisShaderSet_cl *pShaderSet = pEnt->GetShaderSet();
    VDynamicMesh *pMesh = pEnt->GetMesh();

    pShadowCasterEntities->AppendEntry(pEnt);

    if (!pMesh->HasDoubleSidedSurfaces() && (pShaderSet == NULL || !pShaderSet->HasCustomShadowMapFillShader()))
    {
      VCompiledShaderPass *pPass = pEnt->GetMesh()->HasTranslucentSurfaces() ? pAlphaFillTechnique->m_Shaders.GetAt(0) : pSolidFillTechnique->m_Shaders.GetAt(0);
      Vision::RenderLoopHelper.RenderEntityWithShaders(pEnt, 1, &pPass);
    }
    else 
    {
      VisDrawCallInfo_t surfaceShaderList[RLP_MAX_ENTITY_SURFACES];
      ANALYSIS_IGNORE_WARNING_ONCE(6246)
      VDynamicMesh *pMesh = pEnt->GetMesh();
      VisSurface_cl **ppSurfaces = pEnt->GetSurfaceArray();
      int iNumSubmeshes = pMesh->GetSubmeshCount();
      int iNumValidMeshes = 0;
      for (int j=0; j<iNumSubmeshes; j++)
      {
        VDynamicSubmesh *pSubmesh = pMesh->GetSubmesh(j);
        VisSurface_cl *pSurface = ppSurfaces[pSubmesh->m_iMaterialIndex];
        VCompiledShaderPass* pShader = NULL;
        VisDrawCallInfo_t &info(surfaceShaderList[iNumValidMeshes]);

        if (pSurface->m_spShadowmapFill.GetPtr() != NULL)
        {
          pShader = pSurface->m_spShadowmapFill->m_Shaders.GetAt(0);
          VASSERT_ONCE(((pShader->m_cStateGroupMask & STATEGROUP_DEPTHSTENCIL) == 0) && "Set the CompiledClass of your shadowmap shader to VShadowMapFillShaderPass!");
        }
        else
        {
          if (pSurface->IsDoubleSided())
          {
            if (pSurface->GetTransparencyType() == VIS_TRANSP_ALPHATEST || pSurface->GetTransparencyType() == VIS_TRANSP_ALPHA)
              pShader = pAlphaFillTechniqueDoubleSided->m_Shaders.GetAt(0);
            else if (pSurface->GetTransparencyType() == VIS_TRANSP_NONE)
              pShader = pSolidFillTechniqueDoubleSided->m_Shaders.GetAt(0);
          }
          else
          {
            if (pSurface->GetTransparencyType() == VIS_TRANSP_ALPHATEST || pSurface->GetTransparencyType() == VIS_TRANSP_ALPHA)
              pShader = pAlphaFillTechnique->m_Shaders.GetAt(0);
            else if (pSurface->GetTransparencyType() == VIS_TRANSP_NONE)
              pShader = pSolidFillTechnique->m_Shaders.GetAt(0);
          }
        }

        if(pShader != NULL)
        {
          info.Set(pSubmesh, pSurface, pShader);
          iNumValidMeshes++;
      }
      }

      Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEnt, iNumValidMeshes, surfaceShaderList);
    }
  }

  Vision::RenderLoopHelper.EndEntityRendering();

  if (m_pGenerator->GetShowProfilingData())
  {
    m_pGenerator->m_ShadowProfilingData.iStaticGIRendered[iCurrentCascadeIndex] = m_OpaqueGICollection.GetNumEntries() + m_AlphaGICollection.GetNumEntries() + m_TerrainGICollection.GetNumEntries() + m_OpaqueGICollectionDoubleSided.GetNumEntries() + m_AlphaGICollectionDoubleSided.GetNumEntries();
    m_pGenerator->m_ShadowProfilingData.iEntitiesRendered[iCurrentCascadeIndex] = pEntities->GetNumEntries();
  }

  // Render static geometry
  if (bIncludeStaticGeometry)
  {
    Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_OpaqueGICollection, *pSolidFillTechnique->m_Shaders.GetAt(0));
    Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_OpaqueGICollectionDoubleSided, *pSolidFillTechniqueDoubleSided->m_Shaders.GetAt(0));
    Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_TerrainGICollection, *pTerrainFillTechnique->m_Shaders.GetAt(0));
    Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_AlphaGICollection, *pAlphaFillTechnique->m_Shaders.GetAt(0));
    Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_AlphaGICollectionDoubleSided, *pAlphaFillTechniqueDoubleSided->m_Shaders.GetAt(0));

    if (bUseSurfaceSpecificShaders)
    {
      // Render static geometry with surface specific shader
      int iGICount = m_SurfaceSpecificGICollection.GetNumEntries();
      VisStaticGeometryInstanceCollection_cl tmpCollection;
      VCompiledShaderPass *pCurrentPass = NULL;

      for (int i = 0; i < iGICount; i++)
      {
        VisStaticGeometryInstance_cl *pGI = m_SurfaceSpecificGICollection.GetEntry(i);
        VCompiledShaderPass *pPass = pGI->GetSurface()->m_spShadowmapFill->m_Shaders.GetAt(0);
        VASSERT_ONCE(((pPass->m_cStateGroupMask & STATEGROUP_DEPTHSTENCIL) == 0)  && "Set the CompiledClass of your shadowmap shader to VShadowMapFillShaderPass!" );
        if (pPass != pCurrentPass)
        {
          if (tmpCollection.GetNumEntries() > 0)
          {
            Vision::RenderLoopHelper.RenderStaticGeometryWithShader(tmpCollection, *pCurrentPass);
            tmpCollection.Clear();
          }
          pCurrentPass = pPass;
        }
        tmpCollection.AppendEntry(pGI);
      }
      if (tmpCollection.GetNumEntries() > 0)
      {
        Vision::RenderLoopHelper.RenderStaticGeometryWithShader(tmpCollection, *pCurrentPass);
      }
    }
  }

  // Callback for custom geometry that wants to render itself into the shadow map
  VShadowRendererDataObject data(&VShadowMapGenerator::OnRenderShadowMap, m_pGenerator, iCurrentCascadeIndex);
  VShadowMapGenerator::OnRenderShadowMap.TriggerCallbacks(&data);

  // Reset scissor rectangle.
  if (bSetScissor)
    Vision::RenderLoopHelper.SetScissorRect(NULL);

  m_pGenerator->SetCascadeRendered(iCurrentCascadeIndex);
  if (m_pGenerator->AreAllCascadeRendered())
  {
    // Remove duplicates
    if (m_pGenerator->GetCascadeCount() > 1)
    {
      static VisEntityCollection_cl tempEntityCollection(1024, 1024);
      int iShadowCasterEntityCount = pShadowCasterEntities->GetNumEntries();
      VisBaseEntity_cl **ppEntities = pShadowCasterEntities->GetDataPtr();
      VisBaseEntity_cl::ResetTags();
      tempEntityCollection.Clear();
      for (int i=0; i<iShadowCasterEntityCount; i++, ppEntities++)
      {
        if (!(*ppEntities)->IsTagged())
        {
          tempEntityCollection.AppendEntry(*ppEntities);
          {
            (*ppEntities)->Tag();
          }
        }
      }
      pShadowCasterEntities->CopyFrom(tempEntityCollection);
    }

    // Postprocess the generated shadow map (filtering etc.)
    m_pGenerator->PostProcess();

    // Update for orthographic shadows axis-aligned bounding box that represents the shadow volume
    if (m_pGenerator->GetProjectionType()==SHADOW_PROJECTION_ORTHOGRAPHIC)
      ((VShadowMapGenSpotDir*)m_pGenerator)->UpdateOrthoShadowVolumeVS(m_ShadowCasterBBox);
  }
}

VisEntityCollection_cl *VShadowMapRenderLoop::PostFilterByViewFrustum(int iCascadeIndex, const VisEntityCollection_cl *pInputCollection)
{
  int iCount = pInputCollection->GetNumEntries();
  VisEntityCollection_cl& EntityCollection = m_EntityCollection;
  EntityCollection.EnsureSize(iCount);
  EntityCollection.Clear();

  VisRenderContext_cl *pMainContext = m_pGenerator->GetRendererNode()->GetReferenceContext();
  const bool bConsiderCastShadowFlag = m_pGenerator->GetConsiderCastShadowFlag();
  const bool bBoundingBoxSelection = static_cast<VBaseShadowMapComponentSpotDirectional*>(m_pGenerator->GetShadowMapComponent())->GetCascadeSelection() == VBaseShadowMapComponentSpotDirectional::CSM_SELECT_BY_BOUNDINGBOX;

  const VisFrustum_cl *pFrustum = m_pGenerator->GetMainFrustum();
  VisFrustum_cl clippedFrustum;
  if (m_pGenerator->GetLightSource()->GetType() == VIS_LIGHT_DIRECTED && !bBoundingBoxSelection)
  {
    hkvVec3 vMainCameraPos = pMainContext->GetCamera()->GetPosition();
    hkvVec3 vMainCameraDir = pMainContext->GetCamera()->GetDirection();
    float fRangeFar = static_cast<VShadowMapGenSpotDir*>(m_pGenerator)->GetCascadeCullDistance(iCascadeIndex);

    clippedFrustum.CopyFrom(*pFrustum);
    hkvPlane* pFarPlane = clippedFrustum.GetFarPlane();
    pFarPlane->setFromPointAndNormal(vMainCameraPos + vMainCameraDir * fRangeFar, vMainCameraDir);
    pFrustum = &clippedFrustum;
  }
  
  float fLightToViewFrustumDistances[256];
  Vision::RenderLoopHelper.ComputeLightFrustumDistances(m_pGenerator->GetCascadeLightPosition(iCascadeIndex), pFrustum, fLightToViewFrustumDistances);

  for (int i=0; i<iCount; i++)
  {
    VisBaseEntity_cl *pEnt = pInputCollection->GetEntry(i);
    if (bConsiderCastShadowFlag && !pEnt->GetCastShadows())
      continue;
   
    if (!Vision::RenderLoopHelper.CompareLightFrustumDistances(*pEnt->GetCurrentVisBoundingBoxPtr(), pFrustum, fLightToViewFrustumDistances))
      continue;

    EntityCollection.AppendEntryFast(pEnt);
    m_ShadowCasterBBox.expandToInclude(*pEnt->GetCurrentVisBoundingBoxPtr());
  }

  return &EntityCollection;
}

VisStaticGeometryInstanceCollection_cl *VShadowMapRenderLoop::PostFilterByViewFrustum(int iCascadeIndex, const VisStaticGeometryInstanceCollection_cl *pInputCollection)
{
  int iCount = pInputCollection->GetNumEntries();
  m_GICollection.EnsureSize(iCount);
  m_GICollection.Clear();

  VisRenderContext_cl *pMainContext = m_pGenerator->GetRendererNode()->GetReferenceContext();
  IVisVisibilityCollector_cl *pMainVisCollector = pMainContext->GetVisibilityCollector();
  const bool bConsiderCastShadowFlag = m_pGenerator->GetConsiderCastShadowFlag();
  const bool bBoundingBoxSelection = static_cast<VBaseShadowMapComponentSpotDirectional*>(m_pGenerator->GetShadowMapComponent())->GetCascadeSelection() == VBaseShadowMapComponentSpotDirectional::CSM_SELECT_BY_BOUNDINGBOX;
  
  const VisFrustum_cl *pFrustum = m_pGenerator->GetMainFrustum();
  VisFrustum_cl clippedFrustum;

  if (m_pGenerator->GetLightSource()->GetType() == VIS_LIGHT_DIRECTED && !bBoundingBoxSelection)
  {
    hkvVec3 vMainCameraPos = pMainContext->GetCamera()->GetPosition();
    hkvVec3 vMainCameraDir = pMainContext->GetCamera()->GetDirection();
    float fRangeFar = static_cast<VShadowMapGenSpotDir*>(m_pGenerator)->GetCascadeCullDistance(iCascadeIndex);
    
    clippedFrustum.CopyFrom(*pFrustum);
    hkvPlane* pFarPlane = clippedFrustum.GetFarPlane();
    hkvVec3 vNormal = pFarPlane->m_vNormal;
    pFarPlane->setFromPointAndNormal(vMainCameraPos + vMainCameraDir * fRangeFar, vNormal);
    pFrustum = &clippedFrustum;
  }

  float fLightToViewFrustumDistances[256];
  Vision::RenderLoopHelper.ComputeLightFrustumDistances(m_pGenerator->GetCascadeLightPosition(iCascadeIndex), pFrustum, fLightToViewFrustumDistances);

  const int iGeomTypes = m_pGenerator->m_pShadowComponent->GetGeometryTypes();
  for (int i=0; i<iCount; i++)
  {
    VisStaticGeometryInstance_cl *pGI = pInputCollection->GetEntry(i);
    if (bConsiderCastShadowFlag && !pGI->GetCastDynamicShadows())
      continue;
    if (!Vision::RenderLoopHelper.CompareLightFrustumDistances(pGI->GetBoundingBox(), pFrustum, fLightToViewFrustumDistances))
      continue;

    // filter caster types:
    switch (pGI->GetGeometryType())
    {
      case STATIC_GEOMETRY_TYPE_MESHINSTANCE:
        if ((iGeomTypes&SHADOW_CASTER_STATICMESHES)==0) continue;
        break;
      case STATIC_GEOMETRY_TYPE_TERRAIN:
        if ((iGeomTypes&SHADOW_CASTER_TERRAIN)==0) continue;
        break;
    }
    m_GICollection.AppendEntryFast(pGI);
    m_ShadowCasterBBox.expandToInclude(pGI->GetBoundingBox());
  }

  return &m_GICollection;
}

/*
void VShadowMapRenderLoop::SortByRenderState(VisEntityCollection_cl *pEntities)
{
  int iCount = pEntities->GetNumEntries();
  qsort(pEntities->GetDataPtr(), (size_t)iCount, sizeof(VisBaseEntity_cl *), SortEntityRenderState);
}

int VShadowMapRenderLoop::SortEntityRenderState( const void *elem1, const void *elem2) 
{
  VDynamicMesh* pModel0 = (*(VisBaseEntity_cl **)elem1)->GetMesh();
  VDynamicMesh* pModel1 = (*(VisBaseEntity_cl **)elem2)->GetMesh();

  int iTransp0 = ((int)pModel0->HasTranslucentSurfaces()<<1) + (int)pModel0->HasDoubleSidedSurfaces();
  int iTransp1 = ((int)pModel1->HasTranslucentSurfaces()<<1) + (int)pModel1->HasDoubleSidedSurfaces();
  int iTrans = (iTransp0 - iTransp1) << 29;

  return iTrans + ((int)pModel0->GetNumber() - (int)pModel1->GetNumber());
}
*/

void VShadowMapRenderLoop::SplitByRenderState(const VisStaticGeometryInstanceCollection_cl *pStaticGI)
{
  int iCount = pStaticGI->GetNumEntries();

  // Clear the collections
  m_OpaqueGICollection.Clear();
  m_AlphaGICollection.Clear();
  m_OpaqueGICollectionDoubleSided.Clear();
  m_AlphaGICollectionDoubleSided.Clear();
  m_TerrainGICollection.Clear();
  m_SurfaceSpecificGICollection.Clear();

  // This wastes some memory, but allows us to use AppendEntryFast and avoids fragmentation due to excessive runtime allocations
  m_OpaqueGICollection.EnsureSize(iCount);
  m_AlphaGICollection.EnsureSize(iCount);
  m_OpaqueGICollectionDoubleSided.EnsureSize(iCount);
  m_AlphaGICollectionDoubleSided.EnsureSize(iCount);
  m_TerrainGICollection.EnsureSize(iCount);
  m_SurfaceSpecificGICollection.EnsureSize(iCount);

  // Split by geometry type and translucency
  for (int i=0; i<iCount; i++)
  {
    VisStaticGeometryInstance_cl *pGI = pStaticGI->GetEntry(i);
    if (pGI->GetGeometryType() == STATIC_GEOMETRY_TYPE_MESHINSTANCE)
    {
      if (!m_pGenerator->m_pShadowComponent->GetUseSurfaceSpecificShadowShaders() || pGI->GetSurface()->m_spShadowmapFill.GetPtr() == NULL)
      {
        VisSurface_cl *pSurface = pGI->GetSurface();
        if (pSurface->GetTransparencyType() == VIS_TRANSP_ALPHATEST || pSurface->GetTransparencyType() == VIS_TRANSP_ALPHA)
        {
          if (pSurface->IsDoubleSided())
          {
            m_AlphaGICollectionDoubleSided.AppendEntryFast(pGI);
          }
          else
          {
            m_AlphaGICollection.AppendEntryFast(pGI);
          }
        }
        else if (pSurface->GetTransparencyType() == VIS_TRANSP_NONE)
        {
          if (pSurface->IsDoubleSided())
          {
            m_OpaqueGICollectionDoubleSided.AppendEntryFast(pGI);
          }
          else
          {
            m_OpaqueGICollection.AppendEntryFast(pGI);
          }
        }
      }
      else
      {
        m_SurfaceSpecificGICollection.AppendEntryFast(pGI);
      }
    }
    else if (pGI->GetGeometryType() == STATIC_GEOMETRY_TYPE_TERRAIN)
    {
      m_TerrainGICollection.AppendEntryFast(pGI);
    }
  }
}

void VShadowMapRenderLoop::SplitByRenderState(const VisEntityCollection_cl *pEntities)
 {
  int iCount = pEntities->GetNumEntries();

  // Clear the collections
  m_OpaqueEntityCollection.Clear();
  m_AlphaEntityCollection.Clear();
  m_MixedEntityCollection.Clear();

  // This wastes some memory, but allows us to use AppendEntryFast and avoids fragmentation due to excessive runtime allocations
  m_OpaqueEntityCollection.EnsureSize(iCount);
  m_AlphaEntityCollection.EnsureSize(iCount);
  m_MixedEntityCollection.EnsureSize(iCount);

  // Split by geometry type and translucency
  for (int i=0; i<iCount; i++)
  {
    VisBaseEntity_cl *pEntity = pEntities->GetEntry(i);
    VDynamicMesh *pMesh = pEntity->GetMesh();
    if (pMesh->HasDoubleSidedSurfaces())
      m_MixedEntityCollection.AppendEntryFast(pEntity);
    else if (pMesh->HasTranslucentSurfaces())
      m_AlphaEntityCollection.AppendEntryFast(pEntity);
    else 
      m_OpaqueEntityCollection.AppendEntryFast(pEntity);
  }
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
