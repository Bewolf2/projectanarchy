/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/Glow.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisApiShaderProvider.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#if defined( _VISION_XENON )
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
#endif

//-----------------------------------------------------------------------------------

VPostProcessGlow::VPostProcessGlow(GlowDownscale_e eDownScaleMode) 
  : DownscaleMode(eDownScaleMode)
  , BlurValue(1.0f)
  , Bias(0.0f)
  , Exponent(4.0f)
  , Scale(4.0f)
  , BlurPasses(2)
  , m_iScreenX(0), m_iScreenY(0)
  , m_iScreenTexX(0), m_iScreenTexY(0)
  , m_iBlurX(0), m_iBlurY(0)
  , m_iBlurTexX(0), m_iBlurTexY(0)
  , m_fHorBlurStep(0.0f), m_fVertBlurStep(0.0f)
{
  // base class values
  m_iRequiredBufferFlags = VBUFFERFLAG_FINALCOLOR;
  m_fPriority = VPOSTPROCESSOR_PRIORITY_GLOW;
  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER | DRAWS_FULLSCREEN_QUAD | USES_BLENDING;
}

VPostProcessGlow::VPostProcessGlow() 
  : DownscaleMode(GLOW_DOWNSCALE_2X)
  , BlurValue(1.0f)
  , Bias(0.0f)
  , Exponent(4.0f)
  , Scale(4.0f)
  , BlurPasses(2)
  , m_iScreenX(0), m_iScreenY(0)
  , m_iScreenTexX(0), m_iScreenTexY(0)
  , m_iBlurX(0), m_iBlurY(0)
  , m_iBlurTexX(0), m_iBlurTexY(0)
  , m_fHorBlurStep(0.0f), m_fVertBlurStep(0.0f)
{
  // base class values
  m_iRequiredBufferFlags = VBUFFERFLAG_FINALCOLOR;
  m_fPriority = VPOSTPROCESSOR_PRIORITY_GLOW;
  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER | DRAWS_FULLSCREEN_QUAD | USES_BLENDING;
}

VPostProcessGlow::~VPostProcessGlow()
{
}

void VPostProcessGlow::SetBlurPasses(int iNumPasses)
{
  BlurPasses = iNumPasses;
}

int VPostProcessGlow::GetBlurPasses() const
{
  return BlurPasses;
}

void VPostProcessGlow::SetBlurOffset(float fPixels)
{
  // compute blur steps in texture coordinates
  m_fHorBlurStep = fPixels/static_cast<float>(m_iBlurTexX);
  m_fVertBlurStep = fPixels/static_cast<float>(m_iBlurTexY);

  BlurValue = fPixels;
}

float VPostProcessGlow::GetBlurOffset() const
{
  return BlurValue;
}

void VPostProcessGlow::SetGlowParameters(float fBias, float fPow, float fScale)
{
  Bias = fBias;
  Exponent = fPow;
  Scale = fScale;
}

void VPostProcessGlow::GetGlowParameters(float &fBias, float &fPow, float &fScale) const
{
  fBias = Bias;
  fPow = Exponent;
  fScale = Scale;
}

void VPostProcessGlow::SetDownScaleMode(GlowDownscale_e downScaleMode)
{
  if(DownscaleMode != downScaleMode)
  {
    DownscaleMode = downScaleMode;

    ScheduleReinitialization();
  }
}

void VPostProcessGlow::RemoveContext()
{
  GetOwner()->RemoveContext(GetTargetContext());
}

void VPostProcessGlow::CreateContext()
{
  VASSERT_MSG(GetOwner() != NULL, "Postprocessing effects require a valid renderer node!");
  VisRenderContext_cl *pRefContext = GetOwner()->GetReferenceContext();

  // ============ Glow Render Context ==============

  GetTargetContext()->SetRenderFlags(VIS_RENDERCONTEXT_FLAG_SHOW_DEBUGOUTPUT);
  GetTargetContext()->SetCamera(pRefContext->GetCamera());

  GetTargetContext()->SetName("PostProcessGlow");

  GetTargetContext()->SetUserData(this);
  GetTargetContext()->SetPriority(m_fPriority);

  GetOwner()->AddContext(GetTargetContext());

  GetTargetContext()->SetRenderLoop(new PostProcessRenderLoop_cl(this));
  GetTargetContext()->SetVisibilityCollector(NULL);

  m_spSourceTextures[0] = GetOwner()->GetGBuffer(IVRendererNode::VGBT_Accumulation);
  m_iNumSourceTextures = 1;
}



VisScreenMask_cl *VPostProcessGlow::CreateScreenMask(int iSizeX, int iSizeY, VTextureObject *pTexture)
{
  VisScreenMask_cl *pNewMask = new VisScreenMask_cl();
  pNewMask->SetTextureObject(pTexture);

  pNewMask->SetPos(0,0);
  pNewMask->SetTargetSize((float)iSizeX,(float)iSizeY);
  pNewMask->SetTextureRange(0.f, (float)iSizeY, (float)iSizeX, 0.f);
  pNewMask->SetTransparency(VIS_TRANSP_NONE);
  pNewMask->SetVisible(FALSE);
  pNewMask->SetUseOpenGLTexelShift(FALSE);
  pNewMask->SetDepthWrite(FALSE);
  pNewMask->SetWrapping(FALSE, FALSE);

  // no wireframe for this mask
  VSimpleRenderState_t s = pNewMask->GetRenderState();
  s.SetFlag(RENDERSTATEFLAG_NOWIREFRAME);
  pNewMask->SetRenderState(s);

  return pNewMask;
}

void VPostProcessGlow::InitializePostProcessor()
{
  if (m_bIsInitialized)
    return;

  CreateContext();

  // Load glow shader library
  BOOL bResult = Vision::Shaders.LoadShaderLibrary("\\Shaders\\PostProcess.ShaderLib", SHADERLIBFLAG_HIDDEN) != NULL;
  VASSERT(bResult); // file not found?
  
  GetTargetContext()->GetSize(m_iScreenX,m_iScreenY);
  m_iScreenTexX = m_iScreenX;
  m_iScreenTexY = m_iScreenY;
  
  int iSizeDivider, iSizeRemainderMask; 
  if (DownscaleMode == GLOW_DOWNSCALE_4X)
  {
    iSizeDivider = 4; iSizeRemainderMask = 3;
  }
  else
  {
    iSizeDivider = 2; iSizeRemainderMask = 1;
  }

  m_iBlurX = (m_iScreenX+(iSizeDivider>>1)) / iSizeDivider;
  m_iBlurY = (m_iScreenY+(iSizeDivider>>1)) / iSizeDivider;

  // make sure resolution is multiple of iSizeDivider
  m_iBlurY &= ~iSizeRemainderMask;
  m_iBlurX &= ~iSizeRemainderMask;
  m_iBlurTexX = m_iBlurX;
  m_iBlurTexY = m_iBlurY;

  VTextureLoader::VTextureFormat_e eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
  if (m_spSourceTextures[0]->IsRenderable())
  {
    const VisRenderableTextureConfig_t *pConfig = 
      static_cast<VisRenderableTexture_cl*>(m_spSourceTextures[0].m_pPtr)->GetConfig();
    eFormat = pConfig->m_eFormat;
  }
    
  m_spPRT[0] = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(
    m_iBlurTexX, m_iBlurTexY, eFormat, 0, 0, false, 0, 0, false, false);
  m_spPRT[1] = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(
    m_iBlurTexX, m_iBlurTexY, eFormat, 1, 0, false, 0, 0, false, false);
    
  VASSERT(m_spPRT[0]!=NULL && m_spPRT[1]!=NULL);
  int i;
  for (i=0; i<2; i++)
  {
    m_spPingPongContext[i] = new VisRenderContext_cl();
    m_spPingPongContext[i]->SetCamera(GetTargetContext()->GetCamera());
    m_spPingPongContext[i]->SetUsageHint(VIS_CONTEXTUSAGE_NONE);
    m_spPingPongContext[i]->SetRenderFlags(VIS_RENDERCONTEXT_FLAG_NO_SCENEELEMENTS);
    m_spPingPongContext[i]->SetRenderTarget(0, m_spPRT[i]);
    m_spPingPongContext[i]->SetDepthStencilTarget(NULL);
    m_spPingPongContext[i]->SetName("PostProcessGlow:PingPong");
  }

  // Next, create the screen masks we'll use for the glow effect:
  // - m_spDownsampleMask: Mask used to perform the downsampling step of the glow geometry
  // - m_spHorBlurMask, m_pVertBlurMask: Masks used for a box blur of the downsampled glow mask
  // - m_spSceneMask: Mask used to draw the main scene previously copied into the SceneTexture
  // - m_spFinalMask: Used for an additive pass of the finished blurred glow mask over the scene mask
  m_spDownsampleMask = CreateScreenMask(m_iBlurTexX, m_iBlurTexY, m_spSourceTextures[0]); 
  m_spHorBlurMask = CreateScreenMask(m_iBlurTexX, m_iBlurTexY, m_spPRT[0]);
  m_spVertBlurMask = CreateScreenMask(m_iBlurTexX, m_iBlurTexY, m_spPRT[1]);
  m_spSceneMask = CreateScreenMask(m_iScreenX, m_iScreenY, m_spSourceTextures[0]);
  m_spFinalMask = CreateScreenMask(m_iScreenX, m_iScreenY, m_spPRT[0]);

  m_spSceneMask->SetTextureRange(0.0f, 0.0f, (float)m_iScreenX, (float)m_iScreenY);
  m_spDownsampleMask->SetTextureRange(0.0f, 0.0f, (float)m_iScreenX, (float)m_iScreenY);
  m_spHorBlurMask->SetTextureRange(0.0f, 0.0f, (float)m_iBlurX, (float)m_iBlurY);
  m_spVertBlurMask->SetTextureRange(0.0f, 0.0f, (float)m_iBlurX, (float)m_iBlurY);
  m_spFinalMask->SetTextureRange(0.0f, 0.0f, (float)m_iBlurX, (float)m_iBlurY);

#if defined(_VR_DX9)
  m_spSceneMask->SetUseOpenGLTexelShift(TRUE);
  m_spDownsampleMask->SetUseOpenGLTexelShift(TRUE);
  m_spHorBlurMask->SetUseOpenGLTexelShift(TRUE);
  m_spVertBlurMask->SetUseOpenGLTexelShift(TRUE);
  m_spFinalMask->SetUseOpenGLTexelShift(TRUE);
#endif

  m_spSceneMask->SetFiltering(FALSE);

  //m_spBlurMask->SetTransparency(VIS_TRANSP_ADDITIVE);
  m_spHorBlurMask->SetTransparency(VIS_TRANSP_NONE);
  m_spVertBlurMask->SetTransparency(VIS_TRANSP_NONE);
  m_spDownsampleMask->SetTransparency(VIS_TRANSP_NONE);

  VSimpleRenderState_t maskState(VIS_TRANSP_ADDITIVE, 
    RENDERSTATEFLAG_DOUBLESIDED | RENDERSTATEFLAG_ALWAYSVISIBLE);
  m_spFinalMask->SetRenderState(maskState);

  m_bIsInitialized = true;

  // Set default offset for the box blur in pixels
  SetBlurOffset(BlurValue);

  // Set bias, exponent and scale variables for the glow effect.
  // The operation performed per pixel is: 
  // fragment = (color + bias)^exponent * scale
  SetGlowParameters(Bias, Exponent, Scale);

  // Create PositionTextureEmulateBilinear effect of fixed function shaders
  bool bFilteringSupported = Vision::Video.IsTextureFilteringSupported(
    m_spPRT[0]->GetResolvedTextureFormat(), *Vision::Video.GetCurrentConfig());
#if defined(_VISION_XENON)
  bFilteringSupported = (bFilteringSupported || m_spPRT[0]->GetConfig()->m_bExpand);
#endif

  if (!bFilteringSupported)
  {
    // bilinear filtering not supported for the given texture format
    char szParamString[128];
    sprintf(szParamString, "invTexSize=%f,%f;", 
      1.0f / static_cast<float>(m_spPRT[0]->GetTextureWidth()),
      1.0f / static_cast<float>(m_spPRT[0]->GetTextureHeight()));

    IVisShaderProvider_cl* pProvider = Vision::GetApplication()->GetShaderProvider();
    VASSERT_MSG(pProvider != NULL, "A shader provider must be set");

    // TODO: do this automatically when rendering screen masks and bilinear filtering is
    // required, but not supported for specific texture formats. (see HS#7308)
    // (Only if RENDERSTATEFLAG_FILTERING is enabled)
    m_spPositionTexBilinearTechnique = Vision::Shaders.CreateTechnique(
      "PositionTextureEmulateBilinear", szParamString, NULL, EFFECTCREATEFLAG_FORCEUNIQUE,
      pProvider->LoadFixedFunctionShaderLib());

    m_spFinalMask->SetTechnique(m_spPositionTexBilinearTechnique);

    // Blend state needs to be overridden 
    // (may be removed if screen masks take care of this automatically)
    VASSERT(m_spPositionTexBilinearTechnique->GetShaderCount() > 0);
    VStateGroupBlend& blendState = 
      m_spPositionTexBilinearTechnique->m_Shaders.GetAt(0)->GetRenderState()->GetBlendState();
    blendState.m_cBlendEnable = 1;
    blendState.m_cBlendOp = BLEND_OP_ADD;
    blendState.m_cSrcBlend = BLEND_ONE;
    blendState.m_cDestBlend = BLEND_ONE;
    blendState.ComputeHash();
  }

  VCompiledTechnique *pTech;
  if (DownscaleMode == GLOW_DOWNSCALE_4X)
  {
    pTech = Vision::Shaders.CreateTechnique(
      bFilteringSupported ? "GlowDownsample4x4": "GlowDownsample4x4Point", NULL);
  }
  else
  {
    pTech = Vision::Shaders.CreateTechnique(
      bFilteringSupported ? "GlowDownsample2x2" : "GlowDownsample2x2Point", NULL);
  }

  if (!pTech)
  {
    m_bValid = false;
    return;
  }

  m_spDownsampleMask->SetTechnique(pTech);

  m_regDownsampleGlowparams.Init(pTech->GetShader(0), "GlowParams");
  m_regDownsampleStepSize.Init(pTech->GetShader(0), "StepSize");

  // Create blur technique
  pTech = Vision::Shaders.CreateTechnique(
    bFilteringSupported ? "Blur" : "BlurPoint", "", NULL);
  if (!pTech)
  {
    m_bValid = false;
    return;
  }
  m_spHorBlurMask->SetTechnique(pTech);
  m_spVertBlurMask->SetTechnique(pTech);

  // Get constant register for step size
  VASSERT(pTech->GetShaderCount() > 0);
  VCompiledShaderPass* pPass = pTech->GetShader(0);
  VASSERT(pPass != NULL);
  m_regBlurStep.Init(pPass, "StepSize");
  VASSERT(m_regBlurStep.IsValid());
}

// ----------------------------------------------------------------------------------------
// Free all resources again
// ----------------------------------------------------------------------------------------

void VPostProcessGlow::DeInitializePostProcessor()
{
  if (!m_bIsInitialized)
    return;

  // free screen masks
  m_spSceneMask = NULL;
  m_spHorBlurMask = NULL;
  m_spVertBlurMask = NULL;

  m_spDownsampleMask = NULL;
  m_spFinalMask = NULL;

  m_spPRT[0] = NULL;
  m_spPRT[1] = NULL;
  m_spPingPongContext[0] = NULL;
  m_spPingPongContext[1] = NULL;

  m_spPositionTexBilinearTechnique = NULL;

  m_bIsInitialized = false;

  RemoveContext();
}


void VPostProcessGlow::Execute()
{
  INSERT_PERF_MARKER_SCOPE("VPostProcessGlow");

  RenderingOptimizationHelpers_cl::SetShaderPreference(112);
  
  // Downsample the color buffer
  {
  m_spPingPongContext[0]->Activate();

  VisRenderContext_cl::PerformPendingContextSwitch();

    SetDownsampleRegisters();

    VisScreenMask_cl* pMask = m_spDownsampleMask;

    Vision::RenderLoopHelper.RenderScreenMasks(&pMask, 1, NULL);

#if defined(_VISION_XENON)
  m_spPRT[0]->Resolve();
#endif
  }

  VCompiledShaderPass* pPass = m_spHorBlurMask->GetTechnique()->GetShader(0);
  VASSERT(pPass != NULL);

  const float horBlurStep[4] = { m_fHorBlurStep, 0.0f, 0.0f, 0.0f };
  const float vertBlurStep[4] = { 0.0f, m_fVertBlurStep, 0.0f, 0.0f };

  for (int i = 0; i < BlurPasses; i++)
  {
    // Horizontal blur pass
    m_regBlurStep.SetRegisterValueF(pPass, horBlurStep);
    m_spPingPongContext[1]->Activate();
    VisRenderContext_cl::PerformPendingContextSwitch();
    
    VisScreenMask_cl* pMask = m_spHorBlurMask;

    Vision::RenderLoopHelper.RenderScreenMasks(&pMask, 1, NULL);
#if defined(_VISION_XENON)
    m_spPRT[1]->Resolve();
#endif
    
    // Vertical blur pass
    m_regBlurStep.SetRegisterValueF(pPass, vertBlurStep);
    m_spPingPongContext[0]->Activate();
    VisRenderContext_cl::PerformPendingContextSwitch();
    
    pMask = m_spVertBlurMask;

    Vision::RenderLoopHelper.RenderScreenMasks(&pMask, 1, NULL);
#if defined(_VISION_XENON)
    m_spPRT[0]->Resolve();
#endif
  }

  // render over screen
  GetTargetContext()->Activate();
  VisRenderContext_cl::PerformPendingContextSwitch();

#if defined(_VISION_XENON)
  VisScreenMask_cl* ppMasks[] = 
{
    m_spSceneMask,
    m_spFinalMask
  };
#else
  VisScreenMask_cl* ppMasks[] = 
{
    m_spFinalMask
  };
#endif
  Vision::RenderLoopHelper.RenderScreenMasks(ppMasks, V_ARRAY_SIZE(ppMasks), NULL);
}

void VPostProcessGlow::SetDownsampleRegisters()
{
#if defined(_VISION_MOBILE)
  // Use approximated power function on mobile
  // See: 'A Non-Integer Power Function on the Pixel Shader' @ gamasutra.com
  // (Juan Guardado, Philippe Beaudoin)
  const float fPowApproxFactor = 1.0f / (1.0f - pow(256.0f, -1.0f / Exponent));
  const float fPowApproxBias = 1.0f - fPowApproxFactor;

  const float fGlowParams[] = {
    Bias, fPowApproxFactor, Scale, fPowApproxBias
  };
#else
  const float fGlowParams[] = {
    Bias, Exponent, Scale, 0.0f
  };
#endif

  m_regDownsampleGlowparams.SetRegisterValueF(m_spDownsampleMask->GetTechnique()->GetShader(0), fGlowParams);

  const float fStepSize[] = {
    1.0f/static_cast<float>(m_iScreenX), 1.0f/static_cast<float>(m_iScreenY), 0, 0
  };

  m_regDownsampleStepSize.SetRegisterValueF(m_spDownsampleMask->GetTechnique()->GetShader(0), fStepSize);
}

//-----------------------------------------------------------------------------------

V_IMPLEMENT_SERIAL( VPostProcessGlow, VPostProcessingBaseComponent, 0, &g_VisionEngineModule );

void VPostProcessGlow::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    ar >> m_bActive;

    int iDownscaleMode;
    ar >> iDownscaleMode;
    DownscaleMode = static_cast<GlowDownscale_e>(iDownscaleMode);
    ar >> Bias;
    ar >> Exponent;
    ar >> Scale;
    ar >> BlurValue;
    ar >> BlurPasses;

    VRendererNodeCommon* pDummy;
    ar >> pDummy;
  }
  else
  {
    ar << m_bActive;
    ar << static_cast<int>(DownscaleMode);
    ar << Bias;
    ar << Exponent;
    ar << Scale;
    ar << BlurValue;
    ar << BlurPasses;
    ar << GetOwner();
  }
}

//-----------------------------------------------------------------------------------

START_VAR_TABLE(VPostProcessGlow, VPostProcessingBaseComponent, "VPostProcessGlow", 0, "Glow")  
  DEFINE_VAR_ENUM(VPostProcessGlow, DownscaleMode, "Downscale mode (4x is slightly faster, 2x produces higher quality)", "DOWNSCALE_2X", "DOWNSCALE_2X,DOWNSCALE_4X", 0, 0);
  DEFINE_VAR_FLOAT(VPostProcessGlow, Bias, "Glow Bias (higher bias makes overall picture brighter and increases glow)", "0.05", 0, "Slider(0,4)");
  DEFINE_VAR_FLOAT(VPostProcessGlow, Exponent, "Glow exponent (higher exponent results in more contrast and more selective glow)", "1.5", 0, "Slider(0,16)");
  DEFINE_VAR_FLOAT(VPostProcessGlow, Scale, "Glow Scale (higher scale makes bright areas brighter)", "0.3333", 0, "Slider(0,10)");
  DEFINE_VAR_INT(VPostProcessGlow, BlurPasses, "Number of blur passes for the glow effect.", "3", 0, 0);
  DEFINE_VAR_FLOAT(VPostProcessGlow, BlurValue, "Sample texel offset value for blur pass", "1", 0, "Slider(0,4)"); 
END_VAR_TABLE


//-----------------------------------------------------------------------------------

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
