/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Glow.hpp

#ifndef GLOWEFFECT_HPP_INCLUDED
#define GLOWEFFECT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>


/// \brief
///   Glow downscale factor
enum GlowDownscale_e
{
  GLOW_DOWNSCALE_2X = 0,      ///< Glow texture has 1/4 of the size of the input texture (looks slightly better)
  GLOW_DOWNSCALE_4X = 1       ///< Glow texture has 1/16 of the size of the input texture (faster)
};


/// \brief
///   Glow/Bloom post-processing effect
/// 
/// This post-processing effect provides functionality for glow and bloom.
class VPostProcessGlow : public VPostProcessingBaseComponent
{
public:
  
  /// \brief
  ///   Constructor of the Glow/Bloom post-processing effect. 
  /// 
  /// \param eDownScaleMode
  ///   Downscaling mode for the post-processing effect. May be GLOW_DOWNSCALE_2X or GLOW_DOWNSCALE_4X.
  EFFECTS_IMPEXP VPostProcessGlow(GlowDownscale_e eDownScaleMode);

  /// \brief
  ///   Destructor of the Glow/Bloom post-processing effect.
  EFFECTS_IMPEXP ~VPostProcessGlow();

  /// \brief
  ///   Sets the number of blur passes.
  EFFECTS_IMPEXP void SetBlurPasses(int iNumPasses);

  /// \brief
  ///   Returns the number of blur passes.
  EFFECTS_IMPEXP int GetBlurPasses() const;

  /// \brief
  ///   Sets the blur offset.
  EFFECTS_IMPEXP void SetBlurOffset(float fBlurOffset);

  /// \brief
  ///   Returns the blur offset.
  EFFECTS_IMPEXP float GetBlurOffset() const;

  /// \brief
  ///   Sets the glow/bloom parameters: bias, exponent, and scale.
  EFFECTS_IMPEXP void SetGlowParameters(float fBias, float fPow, float fScale);

  /// \brief
  ///   Returns the glow/bloom parameters: bias, exponent, and scale.
  EFFECTS_IMPEXP void GetGlowParameters(float &fBias, float &fPow, float &fScale) const;

  /// \brief
  ///   Sets the current glow/bloom downscale mode.
  EFFECTS_IMPEXP void SetDownScaleMode(GlowDownscale_e downScaleMode);

  /// \brief
  ///   Returns the current glow/bloom downscale mode.
  inline GlowDownscale_e GetDownScaleMode() {return DownscaleMode;}

  /// \brief
  ///   Internal function to set up the required target render context
  EFFECTS_IMPEXP void CreateContext();

  /// \brief
  ///   Internal function to tear down the required target render context
  EFFECTS_IMPEXP void RemoveContext();


#ifndef _VISION_DOC
  V_DECLARE_VARTABLE(VPostProcessGlow, EFFECTS_IMPEXP);
  V_DECLARE_SERIAL_DLLEXP( VPostProcessGlow,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
  EFFECTS_IMPEXP VPostProcessGlow();

  // VARTABLE variables
  GlowDownscale_e DownscaleMode;
  float BlurValue;
  float Bias;
  float Exponent;
  float Scale;
  int BlurPasses;

#endif

protected:
  virtual void Execute() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void InitializePostProcessor() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DeInitializePostProcessor() HKV_OVERRIDE;

  void SetDownsampleRegisters();
  void ExecuteWithCopy();

  VisScreenMask_cl *CreateScreenMask(int iSizeX, int iSizeY, VTextureObject *pTexture);

  // Member variabless
  VisRenderContextPtr m_spPingPongContext[2];
  VTextureObjectPtr m_spSceneTexture;
  VTextureObjectPtr m_spGlowTexture;
  VTextureObjectPtr m_spBlurTexture;

  VisRenderableTexturePtr m_spPRT[2];

  VisScreenMaskPtr m_spDownsampleMask;
 
  VisScreenMaskPtr m_spHorBlurMask;
  VisScreenMaskPtr m_spVertBlurMask;

  VisScreenMaskPtr m_spSceneMask;
  VisScreenMaskPtr m_spFinalMask;

  // Technique used to emulate bilinear filtering (if not supported)
  // when drawing the glow mask in the final pass.
  VCompiledTechniquePtr m_spPositionTexBilinearTechnique;

  // dimensions
  int m_iScreenX, m_iScreenY;
  int m_iScreenTexX, m_iScreenTexY;
  int m_iBlurX, m_iBlurY;
  int m_iBlurTexX, m_iBlurTexY;
  float m_fHorBlurStep, m_fVertBlurStep;
  VConstantBufferRegister m_regBlurStep;

  VConstantBufferRegister m_regDownsampleGlowparams;
  VConstantBufferRegister m_regDownsampleStepSize;
};

#endif //GLOWEFFECT_HPP_INCLUDED

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
