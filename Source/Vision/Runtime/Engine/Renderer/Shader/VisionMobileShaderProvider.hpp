/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionMobileShaderProvider.hpp

#ifndef VISION_MOBILE_SHADERPROVIDER_HPP_INCLUDED
#define VISION_MOBILE_SHADERPROVIDER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Renderer/Shader/VisApiShaderProvider.hpp>

/// \brief
///   Class for mobile dynamic light shaders.
///
/// This class extends the base class functionality with some specific methods used for mobile dynamic lighting.
///
class VMobileDynamicLightShader : public VDynamicLightShaderBase
{
public:
  inline void SetLightProjection(int iNumCascades, const hkvMat4* lightProjection)
  {
    if (m_RegLightProjection.IsValid())
      m_RegLightProjection.SetRegisterValuesMatrix(this, lightProjection, iNumCascades);
  }

  inline void SetShadowParameters(float fFadeoutStart, float fFadeoutEnd, float fSubtractiveThreshold)
  {
    float pParams[4] = { fFadeoutEnd, 1.0f / (fFadeoutEnd - fFadeoutStart), fSubtractiveThreshold, 0.0f };
    m_RegShadowParams.SetRegisterValueSafeF(this, pParams);
  }

#ifndef _VISION_DOC

  VConstantBufferRegister m_RegLightProjection, m_RegShadowParams;
 
  // shader overrides
  V_DECLARE_SERIAL_DLLEXP(VMobileDynamicLightShader, VISION_APIDATA)
  VISION_APIDATA VOVERRIDE void PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader);

#endif
};


// Specific mobile custom techniques for rendering light/ shadow in base pass
enum VMobileBasePassTechnique_e
{
  VMOBILE_BASEPASS_TECHNIQUE_DYN_POINTLIGHT=0,
  VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT,
  VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT,
  VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT_SHADOW,
  VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT_SHADOW,
  VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT_SUBTRACTIVE_SHADOW,
  VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT_SUBTRACTIVE_SHADOW
}; 

// Flags for specifying mobile lighting techniques    
enum VMobileLightingFlag_e
{
  VMOBILE_LIGHTING_FLAG_BASEPASS=1,   // lighting performed in base pass
  VMOBILE_LIGHTING_FLAG_DYNAMIC=2,    // dynamic lighting
  VMOBILE_LIGHTING_FLAG_SHADOW=4      // use shadow maps
};

/// \class  VisionMobileShaderProvider
///
/// \brief  A special shader provider for mobile devices.
///
class VisionMobileShaderProvider : public VisionShaderProvider_cl
{
public:
  // Constructor / Destructor
  VISION_APIFUNC VisionMobileShaderProvider();
  VISION_APIFUNC virtual ~VisionMobileShaderProvider();

  V_DECLARE_DYNCREATE_DLLEXP(VisionMobileShaderProvider, VISION_APIDATA);

  ///
  /// @name Overridden Functions
  /// @{
  ///

  VISION_APIFUNC VOVERRIDE void SetEffect(VisSurface_cl *pSurface, 
    VCompiledEffect *pFX, VTechniqueConfig* pDefaultConfig = NULL);

  /// \brief
  ///   Overridden function for static lighting. See base class.
  VISION_APIFUNC VOVERRIDE VCompiledEffect* CreateMaterialEffect(
    VisSurface_cl *pSurface, int iFlags=0);

  /// \brief
  ///   Overridden function for dynamic lighting. See base class.
  VISION_APIFUNC VOVERRIDE VCompiledTechnique* GetDynamicLightShader(
    const VisLightSource_cl *pLight, const VisSurface_cl *pSurface, bool bForEntity);

  /// \brief
  ///   Implements IVisRenderLoop_cl
  VISION_APIFUNC VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  ///
  /// @}
  ///

private:
  // Private functions
  VCompiledEffect* CreateEffect(const VisSurface_cl *pSurface, const char* szEffectName, char* szParamStr, int iCreateFlags, 
                                bool bDynamicLighting);

  VCompiledTechnique* CreateLightingTechnique(VisSurface_cl *pSurface,VCompiledEffect *pFX, const VTechniqueConfig* pConfigChain, 
                                              int iNumConfigsInChain, const VTechniqueConfig* pGlobalConfig, bool bBasepass);

  void SetDynamicLightingTechniques(VisSurface_cl *pSurface, VCompiledEffect *pFX, const VTechniqueConfig &defaultConfig, 
                                    int iLightingFlags);

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
