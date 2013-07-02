/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/ToneMapping.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>

VPostProcessToneMapping::VPostProcessToneMapping(ToneMapType_e toneMapType, float fSaturation, float fContrast, float fBrightness, float fMotionBlurFeedback) :
    m_bAdaptive(false),
    tempMasks(2,2)
{
  m_iRequiredBufferFlags = VBUFFERFLAG_FINALCOLOR;
  m_bRenderFirstTime = true;

  ToneMapType = toneMapType;
  Saturation = fSaturation;
  Contrast = fContrast;
  Brightness = fBrightness;
  MotionBlurFeedback = fMotionBlurFeedback;

  m_fPriority = VPOSTPROCESSOR_PRIORITY_TONEMAPPING;
  TintColor = V_RGBA_WHITE;

  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER | DRAWS_FULLSCREEN_QUAD;
}

VPostProcessToneMapping::VPostProcessToneMapping() :
  m_bAdaptive(false),
  tempMasks(2,2)
{
  TintColor = V_RGBA_WHITE;
  m_iRequiredBufferFlags = VBUFFERFLAG_FINALCOLOR;
  m_bRenderFirstTime = true;
  m_fPriority = VPOSTPROCESSOR_PRIORITY_TONEMAPPING;

  ToneMapType = TONEMAP_SCALE;
  Saturation = 1.f;
  Contrast = 1.f;
  Brightness = 0.f;
  MotionBlurFeedback = 0.f;

  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER | DRAWS_FULLSCREEN_QUAD;
}


VPostProcessToneMapping::~VPostProcessToneMapping()
{
}

//-----------------------------------------------------------------------------------

void VPostProcessToneMapping::RemoveContext()
{
  GetOwner()->RemoveContext(GetTargetContext());
}

void VPostProcessToneMapping::SetupContext()
{
  IVRendererNode *pRendererNode = GetOwner();
  VASSERT(pRendererNode != NULL && "Postprocessing effects require a valid renderer node!");

  pRendererNode->AddContext(GetTargetContext());

  m_spSourceTextures[0] = pRendererNode->GetGBuffer(IVRendererNode::VGBT_Accumulation);
  m_iNumSourceTextures = 1;
  VASSERT(m_spSourceTextures[0] != NULL);
}


bool VPostProcessToneMapping::SetVariable(const char *szName, const char *szValue)
{
  bool bRes = VPostProcessingBaseComponent::SetVariable(szName, szValue);

  if (szName && !strcmp(szName,"Grading3DTexture"))
  {
    if (!szValue || !szValue[0])
      m_spGradingLUT = NULL;
    else
      m_spGradingLUT = Vision::TextureManager.Load3DTexture(szValue);
  }

  ScheduleReinitialization();

  return bRes;
}

void VPostProcessToneMapping::InitializePostProcessor()
{
  if (m_bIsInitialized)
    return;

  VASSERT(GetOwner()->IsInitialized());

  SetupContext();

  // Load PostProcess shader library
  BOOL bResult = Vision::Shaders.LoadShaderLibrary("\\Shaders\\PostProcess.ShaderLib", SHADERLIBFLAG_HIDDEN) != NULL;
  VASSERT(bResult); // file not found?

  GetTargetContext()->GetSize(m_iWidth, m_iHeight);
  const float fWidth = static_cast<float>(m_iWidth);
  const float fHeight = static_cast<float>(m_iHeight);

  CreateScreenMasks(fWidth, fHeight);

  SetToneMapping(ToneMapType, Saturation, Contrast, Brightness, TintColor, MotionBlurFeedback);

  const bool bUseMotionBlur = MotionBlurFeedback>0.f;
  VCompiledTechnique* pTech = GetTechnique(bUseMotionBlur);

  m_spMask->SetTechnique(pTech);

  m_spMask->SetTransparency((bUseMotionBlur && IsActive()) ? VIS_TRANSP_ALPHA:VIS_TRANSP_NONE); // set in the shader though

  VCompiledShaderPass *pPass = pTech->GetShader(0);

  m_motionBlurReg.Init(pPass, "BlurFactor");
  m_gradingSamplerReg.Init(pPass, "GradingLUT");
  m_colorTransformReg.Init(pPass, "ColorTransform");

  m_bIsInitialized = true;

  m_bRenderFirstTime = true;
}

// ----------------------------------------------------------------------------------------
// Free all resources again
// ----------------------------------------------------------------------------------------
void VPostProcessToneMapping::DeInitializePostProcessor()
{
  if (!m_bIsInitialized)
    return;

  m_spMask = NULL;
  m_spPreviousFrameTexture = NULL;
  m_spPreviousFrameMask = NULL;
  m_motionBlurReg.Reset();
  m_gradingSamplerReg.Reset();
  m_colorTransformReg.Reset();
  RemoveContext();

  m_bIsInitialized = false;

}

void VPostProcessToneMapping::Execute()
{
  if (!IsActive() || !m_bIsInitialized)
    return;

  INSERT_PERF_MARKER_SCOPE("VPostProcessToneMapping");

  RenderingOptimizationHelpers_cl::SetShaderPreference(112);

  VCompiledShaderPass* pPass = m_spMask->GetTechnique()->GetShader(0);

  if(m_colorTransformReg.IsValid())
  {
    m_colorTransformReg.SetRegisterValuesMatrix(m_spMask->GetTechnique()->GetShader(0), &m_mColorTransform, 1);
  }

  m_gradingSamplerReg.SetSamplerTextureSafe(pPass, m_spGradingLUT);

  {
    INSERT_PERF_MARKER_SCOPE("Tone Map");
  
    tempMasks.Clear();

    if (m_spFrameCopyTexture != NULL)
    {
      Vision::Renderer.CopyToTexture(m_spFrameCopyTexture, 0, 0, m_iWidth, m_iHeight);
    }

    if (MotionBlurFeedback > 0)
    {
      // with motion blur
      if(!m_bRenderFirstTime)
      {
        tempMasks.AppendEntryFast(m_spPreviousFrameMask);
      }
      tempMasks.AppendEntryFast(m_spMask);

      float dtime = Vision::GetTimer()->GetTimeDifference();
      // calculate frame independent blend value

      float fMotionBlurParams[] =
      {
        0, 0, 0, 0
      };

      float& fAlpha = fMotionBlurParams[0];
      fAlpha = 1.f - hkvMath::pow(0.5f, dtime / MotionBlurFeedback);

      // clamp the alpha so that screen mask is not rendered opaque
      const float fMinAlpha = 0.1f; // in theory 1/255, but graphics cards switch to opaque for some thresholds
      fAlpha = hkvMath::Max(fAlpha, fMinAlpha);

      if (m_bRenderFirstTime)
      {
        fAlpha = 1.f; // initialize frame buffer properly
        m_bRenderFirstTime = false;
      }

      m_motionBlurReg.SetRegisterValuesF(pPass, fMotionBlurParams, 1);

      Vision::RenderLoopHelper.RenderScreenMasks(tempMasks);

      Vision::Renderer.CopyToTexture(m_spPreviousFrameTexture, 0, 0, m_iWidth, m_iHeight);
    }
    else
    {
      // no motion blur
      tempMasks.AppendEntryFast(m_spMask);

      // render
      Vision::RenderLoopHelper.RenderScreenMasks(tempMasks);
    }
  } 
}

void VPostProcessToneMapping::SetToneMapping(ToneMapType_e toneMapType, float fSaturation, 
  float fContrast, float fBrightness, VColorRef iTintColor, float fMotionBlurFeedback)
{
  hkvMat4 colorMatrix;
  CreateColorMatrixFromValues(fSaturation,fContrast,fBrightness,iTintColor,colorMatrix);
  SetToneMapping(toneMapType, colorMatrix, fMotionBlurFeedback);
}

VCompiledTechnique *VPostProcessToneMapping::GetTechnique(bool bMotionBlur) const
{
  const char *szEffectName = "PlainCopy"; // inactive tone mapper

  if (IsActive())
  {
    switch (ToneMapType)
    {
    case TONEMAP_NONE:
      if (bMotionBlur)
        szEffectName = "SimpleMotionBlur";
      break;
    case TONEMAP_SCALE:
      szEffectName = bMotionBlur ? "ColorScaleMotionBlur" : "ColorScale";
      break;
    case TONEMAP_SCALE_MAP:
      szEffectName = bMotionBlur ? "ToneMappingBlur" : "ToneMapping";
      break;
    case TONEMAP_GRADING:
      if (m_spGradingLUT!=NULL) // else fall back to PlainCopy
        szEffectName = bMotionBlur ? "ToneMappingGradingBlur" : "ToneMappingGrading";
      break;
    }
  }

  VTechniqueConfig config;
  switch (GetOwner()->GetGammaCorrection())
  {
  case VGC_PreTransform:
    config.SetInclusionTags("GAMMA_CORRECTION");
    break;
  case VGC_PostTransform:
    config.SetInclusionTags("GAMMA_CORRECTION;GAMMA_CORRECTION_POST_TRANSFORM");
    break;
  default:
    break;
  }

  VCompiledTechnique *pTech = (szEffectName != NULL) ? 
    Vision::Shaders.CreateTechnique(szEffectName, NULL, &config, EFFECTFLAGS_FORCEUNIQUE) : NULL;
  VASSERT((szEffectName==NULL || pTech!=NULL) && "Could not create technique for tone mapping");
  return pTech;
}

void VPostProcessToneMapping::SetToneMapping(ToneMapType_e toneMapType, 
  const hkvMat4& colorTransform, float fMotionBlurFeedback)
{
  ToneMapType = toneMapType;

  const bool bOldEnabled = MotionBlurFeedback>0.f;
  const bool bUseMotionBlur = fMotionBlurFeedback>0.f;
  MotionBlurFeedback = fMotionBlurFeedback;
  m_bRenderFirstTime |= (bOldEnabled!=bUseMotionBlur);


  SetColorTransform(colorTransform);

  ScheduleReinitialization();
  }

void VPostProcessToneMapping::CreateScreenMasks( const float fWidth, const float fHeight )
{
  if (MotionBlurFeedback > 0)
  {
    // This screenmask is used to store the previous rendering result. This is necessary since the backbuffers are
    // flipped, and you therefore can't rely on the results of the previous frame being available in the current
    // backbuffer.
    m_spPreviousFrameTexture = Vision::TextureManager.CreateEmpty2DTextureObject(
      "<MotionBlurTempTexture>", m_iWidth, m_iHeight, VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP);
    m_spPreviousFrameTexture->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    m_spPreviousFrameMask = new VisScreenMask_cl();
    m_spPreviousFrameMask->SetTextureObject(m_spPreviousFrameTexture);
    m_spPreviousFrameMask->SetPos(0,0);
    m_spPreviousFrameMask->SetTargetSize(fWidth, fHeight);
    m_spPreviousFrameMask->SetTextureRange(0.0f, 0.0f, fWidth, fHeight);
#ifdef _VR_DX9
    m_spPreviousFrameMask->SetUseOpenGLTexelShift(TRUE);
#else
    m_spPreviousFrameMask->SetUseOpenGLTexelShift(FALSE);
#endif
    m_spPreviousFrameMask->SetTransparency(VIS_TRANSP_NONE);
    m_spPreviousFrameMask->SetVisible(FALSE);
    m_spPreviousFrameMask->SetDepthWrite(FALSE);
    m_spPreviousFrameMask->SetWrapping(FALSE, FALSE);
    m_spPreviousFrameMask->SetVisibleBitmask(0); // this mask is rendered manually via a collection
    VSimpleRenderState_t s1 = m_spPreviousFrameMask->GetRenderState();
    s1.SetFlag(RENDERSTATEFLAG_NOWIREFRAME);
    m_spPreviousFrameMask->SetRenderState(s1);
  }

  m_spMask = new VisScreenMask_cl();
  if (GetTargetContext()->GetRenderTarget() == m_spSourceTextures[0])
  {
    m_spFrameCopyTexture = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(m_iWidth, m_iHeight, m_spSourceTextures[0]->GetTextureFormat(), 0);
    m_spMask->SetTextureObject(m_spFrameCopyTexture);
  }
  else
  {
    m_spFrameCopyTexture = NULL;
    m_spMask->SetTextureObject(m_spSourceTextures[0]);
  }

  m_spMask->SetPos(0,0);
  m_spMask->SetTargetSize(fWidth, fHeight);
  m_spMask->SetTextureRange(0.0f, 0.0f, fWidth, fHeight);
#if defined(_VR_DX9)
  m_spMask->SetUseOpenGLTexelShift(TRUE);
#else
  m_spMask->SetUseOpenGLTexelShift(FALSE);
#endif
  m_spMask->SetTransparency(VIS_TRANSP_NONE);
  m_spMask->SetVisible(FALSE);
  m_spMask->SetDepthWrite(FALSE);
  m_spMask->SetWrapping(FALSE, FALSE);
  m_spMask->SetVisibleBitmask(0); // this mask is rendered manually via a collection

  // no wireframe for this mask
  VSimpleRenderState_t s2 = m_spMask->GetRenderState();
  s2.SetFlag(RENDERSTATEFLAG_NOWIREFRAME);
  m_spMask->SetRenderState(s2);
}

void VPostProcessToneMapping::CreateColorMatrixFromValues(float fSaturation, 
  float fContrast, float fBrightness, VColorRef iTintColor, hkvMat4& colorMatrix)
{
  // the parameters are already set to this values
  Saturation = fSaturation;
  Contrast = fContrast;
  Brightness = fBrightness;
  TintColor = iTintColor;

  // apply saturation
  const float lumR = 0.3086f;
  const float lumG = 0.6094f;
  const float lumB = 0.0820f;
  float satCompl = 1.0f - fSaturation;
  float satComplR = lumR * satCompl;
  float satComplG = lumG * satCompl;
  float satComplB = lumB * satCompl;
  colorMatrix.setIdentity ();
  colorMatrix.m_Column[0][0] = satComplR + fSaturation;
  colorMatrix.m_Column[1][0] = satComplR;
  colorMatrix.m_Column[2][0] = satComplR;

  colorMatrix.m_Column[0][1] = satComplG;
  colorMatrix.m_Column[1][1] = satComplG + fSaturation;
  colorMatrix.m_Column[2][1] = satComplG;

  colorMatrix.m_Column[0][2] = satComplB;
  colorMatrix.m_Column[1][2] = satComplB;
  colorMatrix.m_Column[2][2] = satComplB + fSaturation;

  // apply brightness and contrast
  float fFactor = 0.5f * (1.0f - fContrast); 
  float fBias = fFactor + fBrightness; 

  for (int i=0; i<16; i++)
    colorMatrix.m_ElementsCM[i] *= fContrast;

  colorMatrix.transpose();

  colorMatrix.m_Column[3][0] = fBias;
  colorMatrix.m_Column[3][1] = fBias;
  colorMatrix.m_Column[3][2] = fBias;

  // tint color
  hkvVec3 tintColorF = TintColor.ToFloat();
  for (int i=0;i<3;i++)
  {
    colorMatrix.m_Column[i][0] *= tintColorF.x;
    colorMatrix.m_Column[i][1] *= tintColorF.y;
    colorMatrix.m_Column[i][2] *= tintColorF.z;
  } 
}

void VPostProcessToneMapping::SetColor(float fSaturation, float fContrast, float fBrightness, VColorRef iTintColor)
{
  // default constructor sets identity
  hkvMat4 colorMatrix;

  CreateColorMatrixFromValues(fSaturation,fContrast,fBrightness,iTintColor,colorMatrix);

  SetColorTransform(colorMatrix);
}

void VPostProcessToneMapping::SetColorTransform(const hkvMat4& colorTransform)
{
  m_mColorTransform = colorTransform;
  m_mColorTransform.transpose();
}

bool VPostProcessToneMapping::IsIdentity() const
{
  if (!VPostProcessingBaseComponent::IsIdentity())
    return false;
  if (ToneMapType!=TONEMAP_NONE || MotionBlurFeedback!=0.f)
    return false;
  if (!m_mColorTransform.isIdentity ())
    return false;

  return true;
}

void VPostProcessToneMapping::SetActive(bool bStatus)
{
  if (m_bActive == bStatus)
    return;
  VPostProcessingBaseComponent::SetActive(bStatus);
  SetToneMapping(ToneMapType, m_mColorTransform, MotionBlurFeedback); // this applies a new shader to the mask
}

//-----------------------------------------------------------------------------------

V_IMPLEMENT_SERIAL( VPostProcessToneMapping, VPostProcessingBaseComponent, 0, &g_VisionEngineModule );

void VPostProcessToneMapping::Serialize( VArchive &ar )
{
  char iLocalVersion = 1;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; 
    VASSERT_MSG(iLocalVersion<=1, "VPostProcessToneMapping: Invalid version number, please re-export");
    ar >> m_bActive;
    ar >> m_mColorTransform;

    int iToneMapType;
    ar >> iToneMapType;
    ToneMapType = static_cast<ToneMapType_e>(iToneMapType);
    if (iLocalVersion>=1)
      m_spGradingLUT = static_cast<VTexture3DObject*>(ar.ReadProxyObject());

    ar >> MotionBlurFeedback;
    ar >> Saturation;
    ar >> Contrast;
    ar >> Brightness;
    ar >> TintColor;

    VRendererNodeCommon* pDummy;
    ar >> pDummy;
  }
  else
  {
    ar << iLocalVersion;
    ar << m_bActive;
    ar << m_mColorTransform;

    ar << static_cast<int>(ToneMapType);
    ar.WriteProxyObject(m_spGradingLUT); // version 1
    ar << MotionBlurFeedback;    
    ar << Saturation;    
    ar << Contrast;    
    ar << Brightness;
    ar << TintColor;
    ar << GetOwner();
  }
}

//-----------------------------------------------------------------------------------

START_VAR_TABLE(VPostProcessToneMapping, VPostProcessingBaseComponent, "VPostProcessToneMapping", VCOMPONENT_ALLOW_MULTIPLE, "Tone Mapping")  
  DEFINE_VAR_ENUM(VPostProcessToneMapping, ToneMapType, "Tone Mapping Mode (Scale simply adjusts brightness, contrast, and saturation; Scale_Map additionally remaps the color range to 0..1)", "SCALE", "OFF,SCALE,SCALE_MAP,GRADING", 0, 0);
  DEFINE_VAR_STRING_CALLBACK(VPostProcessToneMapping,Grading3DTexture,"3D texture used for the color grading lookup (GRADING mode only)","",DISPLAY_HINT_TEXTUREFILE,0);
  DEFINE_VAR_FLOAT(VPostProcessToneMapping, Saturation, "Saturation (values below 1 decrease saturation, values above 1 increase saturation); ignored for GRADING mode", "1.0", 0, "Slider(0,16)");
  DEFINE_VAR_COLORREF(VPostProcessToneMapping, TintColor, "Color to tint the final result; ignored for GRADING mode", "255/255/255/255", 0, 0);
  DEFINE_VAR_FLOAT(VPostProcessToneMapping, Contrast, "Contrast (values below 1 decrease contrast, values above 1 increase contrast); ignored for GRADING mode", "1.0", 0, "Slider(0,4)");
  DEFINE_VAR_FLOAT(VPostProcessToneMapping, Brightness, "Brightness (values below 0 decrease brightness, values above 0 increase brightness); ignored for GRADING mode", "0.0", 0, "Slider(0,1)");
  DEFINE_VAR_FLOAT(VPostProcessToneMapping, MotionBlurFeedback, "Motion blur feedback strength (higher values result in stronger blur)", "0.0", 0, "Slider(0,10000)");
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
