/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/TimeOfDay.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/ToneMapping.hpp> 
#include <Vision/Runtime/Engine/Renderer/Shader/VisionMobileShaderProvider.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderLoop.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessScreenMasks.hpp>
#include <Vision/Runtime/Engine/Renderer/Lighting/VisApiLightGridManager.hpp>

VCallback VMobileForwardRenderingSystem::OnSetResolution;

/// \brief
///   Simple render loop class for upscaling the rendering results of the VMobileForwardRenderingSystem to the full display output resolution.
class VUpscaleRenderLoop : public IVisRenderLoop_cl
{
public:

  /// \brief
  ///   Constructor. Input texture should be the render target of the renderer node's final target context.
  VUpscaleRenderLoop(VTextureObject *pSourceTexture, bool bFiltering)
  {
    m_spSourceTexture = pSourceTexture;
    m_bFiltering = bFiltering;
  }

  /// \brief
  ///   Upscale render loop implementation. Renders the upscaled image and the 2D overlay geometry.
  virtual void OnDoRenderLoop(void *pUserData)
  {
    INSERT_PERF_MARKER_SCOPE("VUpscaleRenderLoop::OnDoRenderLoop");

    int iWidth, iHeight;
    VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
    pContext->GetSize(iWidth, iHeight);

    VSimpleRenderState_t iState(VIS_TRANSP_NONE,RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_NOMULTISAMPLING);
    if (m_bFiltering)
      iState.SetFlag(RENDERSTATEFLAG_FILTERING);
    IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
    pRI->DrawTexturedQuad(hkvVec2(0.f,0.f), hkvVec2((float)iWidth, (float)iHeight), m_spSourceTexture, hkvVec2(0.0f,0.0f), hkvVec2(1.0f,1.0f), V_RGBA_WHITE, iState);
    Vision::RenderLoopHelper.EndOverlayRendering();

    // Only render 2d overlay geometry. 3d requires a depth buffer, which we don't have at this resolution.
    VRendererNodeCommon::RenderOverlays(true, false);
  }

protected:
  bool m_bFiltering;                        ///< True to enable bilinear filtering, false to use point sampling.
  VTextureObjectPtr m_spSourceTexture;      ///< Input texture to be upscaled.
};


VMobileForwardRenderingSystem::VMobileForwardRenderingSystem()
{
  GammaCorrection = VGC_None;
  RenderingResolutionMode = VRSM_FULL_RESOLUTION;
  DesiredRenderingDpi = 150.f;
  UseUpscaleFiltering = FALSE;
  ResolutionTolerance = 30.f;
  m_bIsInitialized = false;  
  m_spUpscaleTargetContext = NULL;
  m_spOriginalSizeTargetContext = NULL;
  m_spStoreFinalTargetContext = NULL;

  Vision::Callbacks.OnUpdateSceneFinished += this;
  Vision::Callbacks.OnEnterForeground += this;
}

VMobileForwardRenderingSystem::VMobileForwardRenderingSystem(VisRenderContext_cl *pTargetContext) : VRendererNodeCommon(pTargetContext)
{
  GammaCorrection = VGC_None;
  RenderingResolutionMode = VRSM_FULL_RESOLUTION;
  DesiredRenderingDpi = 150.f;
  UseUpscaleFiltering = FALSE;
  ResolutionTolerance = 30.f;
  m_bIsInitialized = false;

  Vision::Callbacks.OnUpdateSceneFinished += this;
  Vision::Callbacks.OnEnterForeground += this;
}

VMobileForwardRenderingSystem::~VMobileForwardRenderingSystem()
{
  DeInitializeRenderer();
  ScratchTexturePool_cl::GlobalManager().PurgeUnusedTextures();

  Vision::Callbacks.OnUpdateSceneFinished -= this;
  Vision::Callbacks.OnEnterForeground -= this;
}


void VMobileForwardRenderingSystem::SetResolutionMode(VRendererResolutionMode_e eResMode, float fDesiredDpi, bool bUseFiltering, float fResolutionTolerance)
{
  if (RenderingResolutionMode != eResMode || UseUpscaleFiltering != (bUseFiltering ? TRUE: FALSE) || fDesiredDpi != DesiredRenderingDpi || fResolutionTolerance != ResolutionTolerance)
  {
    RenderingResolutionMode = eResMode;
    DesiredRenderingDpi = fDesiredDpi;
    UseUpscaleFiltering = bUseFiltering ? TRUE : FALSE;
    ResolutionTolerance = fResolutionTolerance;

    ReInitializeRenderer();
  }
}



void VMobileForwardRenderingSystem::GetTargetSizeFromDeviceDPI(const int *pOriginalSize, int *pTargetSize) const
{
  float fDisplayDpi = Vision::Video.GetDeviceDpi();

  // Platforms without support for returning dpi will return 0, so we simply assume 72dpi here.
  if (fDisplayDpi == 0.0f)
    fDisplayDpi = 72;

  int iTargetDpi = hkvMath::Max(1, (int)DesiredRenderingDpi);

  float fDpiDeviation = (fabsf(fDisplayDpi - (float)iTargetDpi) * 100.f) / fDisplayDpi;
  if (fDpiDeviation < ResolutionTolerance)
  {
    for (int i=0; i<2; i++)
      pTargetSize[i] = pOriginalSize[i];
  }
  else
  {
    for (int i=0; i<2; i++)
      pTargetSize[i] = (iTargetDpi * pOriginalSize[i]) / (int)fDisplayDpi;
  }

  #ifdef HK_DEBUG_SLOW
    Vision::Error.SystemMessage("Adaptive resolution - display density: %.1f, target density: %.1f, render resolution; %i/%i", fDisplayDpi, DesiredRenderingDpi, pTargetSize[0], pTargetSize[1]);
  #endif
}

bool VMobileForwardRenderingSystem::DetermineRenderResolution(int *pTargetSize)
{
  int iOriginalSize[2] = {-1, -1};
  if (m_spUpscaleTargetContext != NULL)
    m_spUpscaleTargetContext->GetSize(iOriginalSize[0], iOriginalSize[1]);
  else if (GetFinalTargetContext() != NULL)
    GetFinalTargetContext()->GetSize(iOriginalSize[0], iOriginalSize[1]);

  VASSERT_MSG(iOriginalSize[0] >= 0 && iOriginalSize[1] >= 0, "Invalid original size!");

  switch (RenderingResolutionMode)
  {
  case VRSM_FULL_RESOLUTION:
    pTargetSize[0] = iOriginalSize[0]; pTargetSize[1] = iOriginalSize[1];
    break;
  case VRSM_QUARTER_RESOLUTION:
    pTargetSize[0] = iOriginalSize[0] >> 1; pTargetSize[1] = iOriginalSize[1] >> 1;
    break;
  case VRSM_USE_TARGET_DPI:
    GetTargetSizeFromDeviceDPI(iOriginalSize, pTargetSize);
    break;
  default:
    VASSERT_MSG(false, "Invalid rendering resolution mode!");
    break;
  };
   
  VSetResolutionCallbackObject resolutionCallbackObject(&OnSetResolution, pTargetSize, this);
  OnSetResolution.TriggerCallbacks(&resolutionCallbackObject);

  // Size of the render target should never exceed the size of the original target buffer
  pTargetSize[0] = resolutionCallbackObject.m_iTargetSize[0] > iOriginalSize[0] ? iOriginalSize[0] : resolutionCallbackObject.m_iTargetSize[0];
  pTargetSize[1] = resolutionCallbackObject.m_iTargetSize[1] > iOriginalSize[1] ? iOriginalSize[1] : resolutionCallbackObject.m_iTargetSize[1];

  return (pTargetSize[0] != iOriginalSize[0]) || (pTargetSize[1] != iOriginalSize[1]);
}


void VMobileForwardRenderingSystem::InitializeRenderer()
{
  if (m_bIsInitialized)
    return;

  int iSize[2];
  bool bUseUpscaling = DetermineRenderResolution(iSize);
  SetUpscaling(bUseUpscaling, iSize[0], iSize[1]);

  VRendererNodeCommon::InitializeRenderer();

  VisLightGridManager_cl::GlobalManager().SetLightGridMode(VisLightGridManager_cl::Simple);

  if(Components().GetComponentOfType<VPostProcessScreenMasks>() == NULL)
  {
    AddComponent(new VPostProcessScreenMasks());
  }

  CreateShaderProvider();

#if defined(_VISION_MOBILE) || defined(WIN32)
  // Only create offscreen context if we have PPs other than the Screen Mask PP
  m_bUsesDirectRenderToFinalTargetContext = (m_Components.Count() <= 1) && (!bUseUpscaling);
#else
  // Direct rendering to final target context isn't implemented/tested on consoles
  m_bUsesDirectRenderToFinalTargetContext = false;
#endif
  CreateOffscreenContext();

  // Reapply FOV and clip planes
  OnViewPropertiesChanged();

  m_bIsInitialized = true;

  InitializePostProcessors();
}

void VMobileForwardRenderingSystem::DeInitializeRenderer()
{
  if (!m_bIsInitialized)
    return;

  DeInitializePostProcessors();

  RemoveOffscreenContext();
  ResetShaderProvider();

  // In case we changed the render targets of the main render contexts, set them back to their original state again (backbuffer).
  if (m_spFinalTargetContext == Vision::Contexts.GetMainRenderContext())
  {
    VisRenderContext_cl::ResetMainRenderContext();
  }

  Vision::Renderer.SetGlobalAmbientColor(hkvVec4::ZeroVector());

  m_bIsInitialized = false;

  // Needs to be called after de-initializing the renderer; otherwise, it would re-enter DeInitializeRenderer().
  SetUpscaling(false, 0, 0);

  VRendererNodeCommon::DeInitializeRenderer();
}

void VMobileForwardRenderingSystem::CreateShaderProvider()
{
  // set the mobile forward shader provider
  if (mobileShaderProvider==NULL)
  {
    oldShaderProvider = Vision::GetApplication()->GetShaderProvider();

    if(oldShaderProvider == NULL || (oldShaderProvider != NULL && !oldShaderProvider->IsOfType(V_RUNTIME_CLASS(VisionMobileShaderProvider))))  
    {
      mobileShaderProvider = new VisionMobileShaderProvider();
      Vision::GetApplication()->SetShaderProvider(mobileShaderProvider);
      mobileShaderProvider->CreateLightingShaderForWorld(true);
      mobileShaderProvider->CreateLightingShaderForAllModels(true);
    }
    else
    {
      mobileShaderProvider = oldShaderProvider;
    }
  }
}

void VMobileForwardRenderingSystem::ResetShaderProvider()
{
  IVisShaderProviderPtr currentShaderProvider = Vision::GetApplication()->GetShaderProvider();
  if (currentShaderProvider == mobileShaderProvider)
  {
    Vision::GetApplication()->SetShaderProvider(oldShaderProvider);
    if (oldShaderProvider)
    {
      oldShaderProvider->CreateLightingShaderForWorld(true);
      oldShaderProvider->CreateLightingShaderForAllModels(true);
      oldShaderProvider = NULL;
    }
  }
  mobileShaderProvider = NULL;
}

VType *VMobileForwardRenderingSystem::GetSupportedTimeOfDaySystem()
{
  return V_RUNTIME_CLASS(VTimeOfDay);
}

void VMobileForwardRenderingSystem::UpdateTimeOfDay()
{
  IVSky *pSky = m_spSky;
  if (!pSky)
  {
    pSky = Vision::World.GetActiveSky();
  }
  if (pSky)
  {
    pSky->Tick(0);
  }

  IVTimeOfDay *pTimeOfDayInterface = Vision::Renderer.GetTimeOfDayHandler();
  if (pTimeOfDayInterface == NULL)
    return;

  VASSERT(pTimeOfDayInterface->IsOfType(V_RUNTIME_CLASS(VTimeOfDay)) && "Incompatible time of day handler installed - has to be VTimeOfDay or a subclass of it!");
  VTimeOfDay *pTimeOfDay = (VTimeOfDay *)pTimeOfDayInterface;

  // Make sure to disable height fog in the forward renderer
  VFogParameters fog = Vision::World.GetFogParameters();
  fog.heightFogMode = VFogParameters::Off;
  Vision::World.SetFogParameters(fog);

  pTimeOfDay->UpdateFogParameters();

  VColorRef vAmbientColor(false);
  float fDawnWeight, fDuskWeight, fNightWeight;
  pTimeOfDay->EvaluateColorValue(0.1f, vAmbientColor, fDawnWeight, fDuskWeight, fNightWeight);
  Vision::Renderer.SetGlobalAmbientColor(vAmbientColor.ToFloat().getAsVec4 (1.0f));
}

void VMobileForwardRenderingSystem::DestroySky()
{
  if (m_spSky == NULL)
    return;

  m_spSky->SetEffect(NULL);
  m_spSky = NULL;
}

void VMobileForwardRenderingSystem::CreateSky(const char *szPrefixNoon, const char *szPrefixDawn, const char *szPrefixDusk, const char *szPrefixNight, const char *szExtension, bool bUseBottom)
{
  DestroySky();

  BOOL bResult = Vision::Shaders.LoadShaderLibrary("\\Shaders\\DefaultSky.ShaderLib", SHADERLIBFLAG_HIDDEN) != NULL;
  VASSERT(bResult);

  VCompiledEffect *pFX = Vision::Shaders.CreateEffect("DefaultSky", NULL, EFFECTCREATEFLAG_NOCONSTANTBUFFER);
  m_spSky = new VSky(szPrefixNoon, szPrefixDawn, szPrefixDusk, szPrefixNight, szExtension, bUseBottom, pFX);
}

void VMobileForwardRenderingSystem::SetUpscaling(bool bStatus, int iWidth, int iHeight)
{
  VASSERT(!IsInitialized());

  if (IsUsingUpscaling() == bStatus)
    return;

  if (bStatus)
  {
    // When upscaling is enabled, create a lower-res intermediate render target/render context and set it as the renderer node's final target context.
    // The final upscale operation happens in a new render context with a VUpscaleRenderLoop, taking the intermediate render target as input.

    // Create intermediate render target (size corresponds to the desired rendering resolution)
    VisRenderableTextureConfig_t config;
    GetRenderTargetConfig(config);
    int iFullWidth = config.m_iWidth;
    int iFullHeight = config.m_iHeight;
    config.m_iWidth = iWidth;
    config.m_iHeight = iHeight;
    VisRenderableTexture_cl *pIntermediateRenderTarget = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(config);
    
    // Create a copy of the final target context. This is going to be our context for upscaling.
    m_spUpscaleTargetContext = new VisRenderContext_cl();
    m_spUpscaleTargetContext->SetCamera(GetFinalTargetContext()->GetCamera());
    m_spUpscaleTargetContext->SetViewProperties(GetFinalTargetContext()->GetViewProperties());
    m_spUpscaleTargetContext->SetViewport(0, 0, iFullWidth, iFullHeight);
    m_spUpscaleTargetContext->SetRenderAndDepthStencilTargets(GetFinalTargetContext());
    m_spUpscaleTargetContext->SetVisibilityCollector(GetFinalTargetContext()->GetVisibilityCollector());
    m_spUpscaleTargetContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_DISPLAY * 10.f);
    m_spUpscaleTargetContext->SetName("MobileForwardRenderer_UpscaleTarget");
    VUpscaleRenderLoop *pUpscaleRenderLoop = new VUpscaleRenderLoop(pIntermediateRenderTarget, UseUpscaleFiltering ? true : false);
    m_spUpscaleTargetContext->SetRenderLoop(pUpscaleRenderLoop);

    // Create another copy of the final target context. This is going to be our new final target context. We can't repurpose the
    // main render context directly, as there is some special-case handling code implemented for it.
    m_spStoreFinalTargetContext = GetFinalTargetContext();
    m_spOriginalSizeTargetContext = new VisRenderContext_cl();
    m_spOriginalSizeTargetContext->SetCamera(GetFinalTargetContext()->GetCamera());
    m_spOriginalSizeTargetContext->SetViewProperties(GetFinalTargetContext()->GetViewProperties());
    m_spOriginalSizeTargetContext->SetVisibilityCollector(NULL);
    m_spOriginalSizeTargetContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_DISPLAY);
    m_spOriginalSizeTargetContext->SetRenderFlags(m_spOriginalSizeTargetContext->GetRenderFlags() | VIS_RENDERCONTEXT_FLAG_SHOW_DEBUGOUTPUT);
    m_spOriginalSizeTargetContext->SetName("MobileForwardRenderer_UpscaleTarget");

    // Disable 2D rendering in screen mask postprocessor. We need to render 2D overlay geometry in the upscale render loop.
    // 3D overlay geometry is rendered in the upscale render loop, as it requires a depth buffer.
    VPostProcessScreenMasks *pPostProcessScreenMasks = (VPostProcessScreenMasks *)m_Components.GetComponentOfType(V_RUNTIME_CLASS(VPostProcessScreenMasks));
    if (pPostProcessScreenMasks)
      pPostProcessScreenMasks->SetRender2dElements(false);

    // Redirect final target context to output to intermediate render target
    m_spOriginalSizeTargetContext->SetRenderTarget(0, pIntermediateRenderTarget);
    SetFinalTargetContext(m_spOriginalSizeTargetContext);

    // Add the upscale render context to our render context list
    AddContext(m_spUpscaleTargetContext);
  }
  else
  {
    // Re-enable 2D rendering in the screen mask postprocessor.
    VPostProcessScreenMasks *pPostProcessScreenMasks = (VPostProcessScreenMasks *)m_Components.GetComponentOfType(V_RUNTIME_CLASS(VPostProcessScreenMasks));
    if (pPostProcessScreenMasks)
      pPostProcessScreenMasks->SetRender2dElements(true);

    // Undo the changes made when enabling upscaling
    VASSERT(m_spStoreFinalTargetContext != NULL);
    SetFinalTargetContext(m_spStoreFinalTargetContext);
    RemoveContext(m_spUpscaleTargetContext);
    m_spUpscaleTargetContext = NULL;
    m_spOriginalSizeTargetContext = NULL;
    m_spStoreFinalTargetContext = NULL;
  }

}

void VMobileForwardRenderingSystem::CreateOffscreenContext()
{
  if (!m_bUsesDirectRenderToFinalTargetContext)
  {
    // Create offscreen render target
    VisRenderableTextureConfig_t config;
    GetRenderTargetConfig(config);
    m_spOffscreenRenderTarget = Vision::TextureManager.CreateRenderableTexture("<OffscreenBuffer>", config, VTM_FLAG_FORCETYPEDSURFACE);
    VASSERT(m_spOffscreenRenderTarget);
    m_spOffscreenRenderTarget->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

    // Create offscreen depth stencil target
    // TODO: If the final target context has a depth-stencil target, we could use that one to save some space!
    GetDepthStencilConfig(config);
    m_spOffscreenDepthStencilTarget = Vision::TextureManager.CreateRenderableTexture("<OffscreenDepthStencilBuffer>", config);
    VASSERT(m_spOffscreenDepthStencilTarget);
    m_spOffscreenDepthStencilTarget->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  }

  // Create offscreen render context
  m_spOffscreenContext = new VisRenderContext_cl();

  int iFlags = VIS_RENDERCONTEXT_FLAGS_ALLFEATURES;
  m_spOffscreenContext->SetRenderFlags(iFlags);

  m_spOffscreenContext->SetCamera(GetFinalTargetContext()->GetCamera());
  m_spOffscreenContext->SetViewProperties(GetFinalTargetContext()->GetViewProperties());
  int iViewportPosX, iViewportPosY, iViewportSizeX, iViewportSizeY;
  GetFinalTargetContext()->GetViewport(iViewportPosX, iViewportPosY, iViewportSizeX, iViewportSizeY);
  m_spOffscreenContext->SetViewport(iViewportPosX, iViewportPosY, iViewportSizeX, iViewportSizeY);

  if (m_bUsesDirectRenderToFinalTargetContext)
  {
    m_spOffscreenContext->SetRenderAndDepthStencilTargets(GetFinalTargetContext());
  }
  else
  {
    m_spOffscreenContext->SetRenderTarget(0, m_spOffscreenRenderTarget);
    m_spOffscreenContext->SetDepthStencilTarget(m_spOffscreenDepthStencilTarget); 
  }

  VMobileForwardRenderLoop *pRenderLoop = new VMobileForwardRenderLoop(this);
  pRenderLoop->InitRenderLoop();
  m_spOffscreenContext->SetRenderLoop(pRenderLoop);

  IVisVisibilityCollector_cl *pVisColl = GetFinalTargetContext()->GetVisibilityCollector();
  if (!pVisColl)
  {
    pVisColl = new VisionVisibilityCollector_cl;
    static_cast<VisionVisibilityCollector_cl*>(pVisColl)->SetBehaviorFlags(VIS_VISCOLLECTOR_USEPORTALS | VIS_VISCOLLECTOR_USEFOV); 
  }

  GetFinalTargetContext()->SetVisibilityCollector(pVisColl, false);
  m_spOffscreenContext->SetVisibilityCollector(pVisColl, true);
  pVisColl->SetOcclusionQueryRenderContext(m_spOffscreenContext);

  m_spOffscreenContext->SetUserData(this); 
  m_spOffscreenContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_SCENE);
  m_spOffscreenContext->SetUsageHint((VisContextUsageHint_e)VIS_CONTEXTUSAGE_VIEWCONTEXT);

  m_spOffscreenContext->SetName("MobileForwardRenderer");

  this->AddContext(m_spOffscreenContext);
}

void VMobileForwardRenderingSystem::RemoveOffscreenContext()
{
  this->RemoveContext(m_spOffscreenContext);
  m_spOffscreenContext = NULL;
  m_spOffscreenRenderTarget = NULL;
  m_spOffscreenDepthStencilTarget = NULL;
}

void VMobileForwardRenderingSystem::GetRenderTargetConfig(VisRenderableTextureConfig_t &config)
{
  VASSERT_MSG(GetFinalTargetContext() != NULL, "The final target context must be set before retrieving render target information.");

  int iWidth, iHeight;
  GetFinalTargetContext()->GetSize(iWidth, iHeight);
  config.m_iWidth = iWidth;
  config.m_iHeight = iHeight;
  config.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
  config.m_iMultiSampling = 0;
  config.m_bResolve = true;
}

void VMobileForwardRenderingSystem::GetDepthStencilConfig(VisRenderableTextureConfig_t &config)
{
  GetRenderTargetConfig(config);
  config.m_bRenderTargetOnly = true;
  config.m_bResolve = false;
  config.m_bIsDepthStencilTarget = true;
  config.m_eFormat = VVideo::GetSupportedDepthStencilFormat(VTextureLoader::D24S8, *Vision::Video.GetCurrentConfig());
}

void VMobileForwardRenderingSystem::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    UpdateTimeOfDay();
  }
  else if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    DestroySky();
    DeInitializeRenderer();
  }
  else if(pData->m_pSender == &Vision::Callbacks.OnEnterForeground)
  {
    if (m_bUsesDirectRenderToFinalTargetContext)
    {
      // Reassign render targets (in case they're copied from the main render context).
      if (m_spOffscreenContext != NULL)
        m_spOffscreenContext->SetRenderAndDepthStencilTargets(GetFinalTargetContext());
    }
  }

  VRendererNodeCommon::OnHandleCallback(pData);
}

void VMobileForwardRenderingSystem::Resize(int iNewWidth, int iNewHeight)
{
  if (IsInitialized())
    ReInitializeRenderer();
}

VisRenderContext_cl *VMobileForwardRenderingSystem::GetReferenceContext()
{
  return m_spOffscreenContext;
}

VisRenderContext_cl *VMobileForwardRenderingSystem::GetTranslucencyReferenceContext()
{
  return GetReferenceContext();
}

VisRenderContext_cl *VMobileForwardRenderingSystem::GetFinalTargetContext()
{
  return m_spFinalTargetContext;
}

void VMobileForwardRenderingSystem::SetFinalTargetContext(VisRenderContext_cl *pContext)
{
  VASSERT_MSG(!IsInitialized(), "The mobile forward renderer must be explicitly deinitialized before changing the final target context.");

  ScratchTexturePool_cl::GlobalManager().PurgeUnusedTextures();
  m_spFinalTargetContext = pContext;
}

VTextureObject *VMobileForwardRenderingSystem::GetPostProcessColorTarget(VRenderTargetVersion_e eVersion) 
{ 
  return GetReferenceContext()->GetRenderTarget(0);
}

VTextureObject *VMobileForwardRenderingSystem::GetPostProcessDepthStencilTarget(VRenderTargetVersion_e eVersion) 
{
  return GetReferenceContext()->GetDepthStencilTarget();
}

VTextureObject *VMobileForwardRenderingSystem::GetGBuffer(VGBufferTarget_e eTarget) 
{
  switch (eTarget)
  {
  case VGBT_Accumulation:
  case VGBT_Diffuse:
    return GetReferenceContext()->GetRenderTarget(0);
  case VGBT_Normal:
    return NULL;
  case VGBT_DepthSpec:
    return GetDepthTexture(VRTV_RESOLVED); // currently returns NULL
  case VGBT_DepthStencil:
    return GetReferenceContext()->GetDepthStencilTarget();
  };
  return NULL; 
}

int VMobileForwardRenderingSystem::GetSupportedBufferFlags()
{
  return VBUFFERFLAG_FINALCOLOR;
}

V_IMPLEMENT_SERIAL(VMobileForwardRenderingSystem, VRendererNodeCommon, 0, &g_VisionEngineModule );

void VMobileForwardRenderingSystem::Serialize( VArchive &ar )
{
  char iLocalVersion = VMOBILE_FORWARDRENDERER_VERSION_CURRENT;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; 
    VASSERT_MSG(iLocalVersion <= VMOBILE_FORWARDRENDERER_VERSION_CURRENT, "Mobile Forward Rendering: Invalid version number");
  
    int iGammaCorrection;
    ar >> iGammaCorrection;
    GammaCorrection = (VGammaCorrection_e)iGammaCorrection;

    if (iLocalVersion >= VMOBILE_FORWARDRENDERER_VERSION_2)
    {
      int iResMode;
      ar >> iResMode;
      RenderingResolutionMode = (VRendererResolutionMode_e)iResMode;
      ar >> DesiredRenderingDpi;
      ar >> UseUpscaleFiltering;

      if (iLocalVersion >= VMOBILE_FORWARDRENDERER_VERSION_3)
        ar >> ResolutionTolerance;
    }

    if (iLocalVersion >= VMOBILE_FORWARDRENDERER_VERSION_1)
    {
      VRendererNodeCommon::Serialize(ar);
    }
    else
    {
      IVRendererNode::Serialize(ar);
    }
  }
  else
  {
    ar << iLocalVersion;
    ar << (int)GammaCorrection; 
    ar << (int)RenderingResolutionMode;
    ar << DesiredRenderingDpi;
    ar << UseUpscaleFiltering;
    ar << ResolutionTolerance;
    VRendererNodeCommon::Serialize(ar);
  }
}

void VMobileForwardRenderingSystem::OnPostProcessorChanged()
{
  if(m_iPostProcessorUpdateCounter == 0 && IsInitialized())
  {
    ReInitializeRenderer();
  }
  else
  {
    m_bPostProcessorAssignmentDirty = true;
  }

}

void VMobileForwardRenderingSystem::SetGammaCorrection(VGammaCorrection_e gammaCorrection)
{
  VASSERT_MSG(!IsInitialized(), "The mobile forward renderer must be explicitly deinitialized before changing the gamma settings.");
  GammaCorrection = gammaCorrection;
}

START_VAR_TABLE(VMobileForwardRenderingSystem, VRendererNodeCommon, "VMobileForwardRenderingSystem", 0, "")  
  // OpenGL ES 2.0 does only support hardware gamma-correction via the GL_EXT_sRGB extension. Since support for this extension seems to be not common among current mobile devices,
  // all related sRGB conversions would have to be done manually in the shader code. Therefore for time being GammaCorrection is not exposed in vForge. 
  // DEFINE_VAR_ENUM(VMobileForwardRenderingSystem, GammaCorrection, "This setting determines whether gamma correction is applied, and if so, whether it's applied before or after the color transformations (contrast, saturation, etc). 'PreTransform' is the more common option.", "Off", "Off,PreTransform,PostTransform", 0, 0);
  DEFINE_VAR_ENUM(VMobileForwardRenderingSystem, RenderingResolutionMode, "This setting specifies the rendering resolution mode. When FULLSIZE, rendering is always performed at the device's full resolution. When QUARTERSIZE, rendering is performed at a quarter of the display resolution. DPI allows you to specify a desired target resolution.", "FULLSIZE", "FULLSIZE,QUARTERSIZE,DPI", 0, 0);
  DEFINE_VAR_FLOAT(VMobileForwardRenderingSystem, DesiredRenderingDpi, "If the the resolution mode is set to DPI, you can specify the desired target resolution in DPI with this value.", "150", 0, "Clamp(0,600)"); 
  DEFINE_VAR_FLOAT(VMobileForwardRenderingSystem, ResolutionTolerance, "Defines the maximum tolerance for matching rendering and display resolution. If e.g. set to 10%,  and the resolutions differ by less than 10%, the native display resolution will be used.", "30", 0, "Clamp(1,100)"); 
  DEFINE_VAR_BOOL(VMobileForwardRenderingSystem, UseUpscaleFiltering, "If rendering resolution and display resolution differ, this setting controls whether bilinear filtering should be used for scaling the image.", "TRUE", 0, 0);

END_VAR_TABLE

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
