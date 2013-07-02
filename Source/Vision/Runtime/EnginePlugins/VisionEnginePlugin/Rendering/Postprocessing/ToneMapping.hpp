/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ToneMapping.hpp

#ifndef TONEMAPPING_HPP_INCLUDED
#define TONEMAPPING_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>

enum ToneMapType_e
{
  TONEMAP_NONE = 0,
  TONEMAP_SCALE = 1,
  TONEMAP_SCALE_MAP = 2,
  TONEMAP_GRADING = 3,
};

/// \brief
///   Tone Mapping post-processing effect
/// 
/// This post-processing effect provides functionality for color control like saturation, contrast,
/// and brightness settings (mode TONEMAP_SCALE), and optionally also tone-mapping for HDR render
/// targets (mode TONEMAP_SCALE_MAP). It also supports a simple motion blur effect.
class VPostProcessToneMapping : public VPostProcessingBaseComponent
{
public:

  /// \brief
  ///   Constructor of the Tone Mapping class.
  /// 
  /// \param toneMapType
  ///   The tone mapping type to use (TONEMAP_NONE, TONEMAP_SCALE, TONEMAP_SCALE_MAP or TONEMAP_GRADING)
  /// 
  /// \param fSaturation
  ///   The Saturation to use. Values below 1.0 will desaturate the image, values above 1.0 will increase the saturation.
  /// 
  /// \param fContrast
  ///   The Contrast to use. Values below 1.0 will reduce the contrast, values above 1.0 will increase it.
  /// 
  /// \param fBrightness
  ///   The Brightness to use. Values below 0.0 will reduce the contrast, values above 0.0 will increase it.
  /// 
  /// \param fMotionBlurFeedback
  ///   If this value is set to 0, motion blur is disabled (default). Values above 0 specify a feedback time for the motion blur effect.
  EFFECTS_IMPEXP VPostProcessToneMapping(ToneMapType_e toneMapType, float fSaturation, float fContrast, float fBrightness, float fMotionBlurFeedback=0.f);

  /// \brief
  ///   Destructor of the Tone Mapping class.
  EFFECTS_IMPEXP ~VPostProcessToneMapping();

  /// \brief
  ///   Sets the tone mapping parameters (tone mapping type, color correction matrix, motion blur feedback value)
  EFFECTS_IMPEXP void SetToneMapping(ToneMapType_e toneMapType, const hkvMat4& colorTransform, float fMotionBlurFeedback=0.f);

  /// \brief
  ///   Sets the tone mapping parameters (tone mapping type, color correction parameters, motion blur feedback value)
  /// 
  /// \param toneMapType
  ///   The tone mapping type to use (TONEMAP_NONE, TONEMAP_SCALE, TONEMAP_SCALE_MAP or TONEMAP_GRADING)
  /// 
  /// \param fSaturation
  ///   The Saturation to use. Values below 1.0 will desaturate the image, values above 1.0 will increase the saturation.
  /// 
  /// \param fContrast
  ///   The Contrast to use. Values below 1.0 will reduce the contrast, values above 1.0 will increase it.
  /// 
  /// \param fBrightness
  ///   The Brightness to use. Values below 0.0 will reduce the contrast, values above 0.0 will increase it.
  ///
  /// \param iTintColor
  ///   Additional color to tint the postprocessing
  ///
  /// \param fMotionBlurFeedback
  ///   If this value is set to 0, motion blur is disabled (default). Values above 0 specify a feedback time for the motion blur effect.
  EFFECTS_IMPEXP void SetToneMapping(ToneMapType_e toneMapType, float fSaturation, float fContrast, float fBrightness, VColorRef iTintColor, float fMotionBlurFeedback=0.f);

  /// \brief
  ///   Returns the tone mapping type
  ToneMapType_e GetToneMapType() const { return ToneMapType; }

  /// \brief
  ///   Returns the color correction matrix
  void GetColorTransform(hkvMat4& colorTransform) const { colorTransform = m_mColorTransform; }

  /// \brief
  ///   Returns the saturation
  float GetSaturation() { return Saturation; }

  /// \brief
  ///   Returns the contrast
  float GetContrast() { return Contrast; }

  /// \brief
  ///   Returns the brightness
  float GetBrightness() { return Brightness; }

  /// \brief
  ///   Returns the motion blur feedback
  float GetMotionBlurFeedback() { return MotionBlurFeedback; }

  /// \brief
  ///   Internal function to set up the required target render context
  EFFECTS_IMPEXP void SetupContext();

  /// \brief
  ///   Internal function to tear down the required target render context
  EFFECTS_IMPEXP void RemoveContext();

  /// \brief
  ///   Overridden virtual function.
  EFFECTS_IMPEXP virtual bool IsIdentity() const HKV_OVERRIDE;

  /// \brief
  ///   Overridden virtual function.
  EFFECTS_IMPEXP virtual void SetActive(bool bStatus) HKV_OVERRIDE;

  /// \brief
  ///   Sets color in runtime for tonemapping.
  ///   It calls SetColorTransform internally.
  EFFECTS_IMPEXP virtual void SetColor(float fSaturation, float fContrast, float fBrightness, VColorRef iTintColor);

  /// \brief
  ///   Sets color transform in runtime for tonemapping
  EFFECTS_IMPEXP virtual void SetColorTransform(const hkvMat4& colorTransform);

  #ifndef _VISION_DOC
    EFFECTS_IMPEXP virtual bool SetVariable(const char *szName, const char *szValue) HKV_OVERRIDE;
    V_DECLARE_VARTABLE(VPostProcessToneMapping, EFFECTS_IMPEXP);

    V_DECLARE_SERIAL_DLLEXP( VPostProcessToneMapping,  EFFECTS_IMPEXP );
    EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
    EFFECTS_IMPEXP VPostProcessToneMapping();

    // VARTABLE variables
    float Saturation;
    VColorRef TintColor;
    float Contrast;
    float Brightness;
    float MotionBlurFeedback;
    ToneMapType_e ToneMapType;

  #endif

protected:
  EFFECTS_IMPEXP virtual void InitializePostProcessor() HKV_OVERRIDE;
  void CreateScreenMasks( const float fWidth, const float fHeight );

  EFFECTS_IMPEXP virtual void DeInitializePostProcessor() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void Execute() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void CreateColorMatrixFromValues(float fSaturation, float fContrast, float fBrightness, VColorRef iTintColor, hkvMat4& colorMatrix);

  EFFECTS_IMPEXP virtual VCompiledTechnique *GetTechnique(bool bMotionBlur) const;
    
  VTextureObjectPtr m_spPreviousFrameTexture, m_spFrameCopyTexture;
  VisScreenMaskPtr m_spPreviousFrameMask;

  int m_iWidth, m_iHeight;
  hkvMat4 m_mColorTransform;
  VisScreenMaskPtr m_spMask;
  bool m_bRenderFirstTime;
  bool m_bAdaptive;

  VConstantBufferRegister m_motionBlurReg;
  VConstantBufferRegister m_colorTransformReg;
  VConstantBufferRegister m_gradingSamplerReg;

  VTexture3DObjectPtr m_spGradingLUT;
  VisScreenMaskCollection_cl tempMasks;
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
