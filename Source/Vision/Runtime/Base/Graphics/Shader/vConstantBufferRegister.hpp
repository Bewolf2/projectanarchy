/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vConstantBufferRegister.hpp

#ifndef VCONSTANTBUFFERREGISTER_HPP_INCLUDED
#define VCONSTANTBUFFERREGISTER_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vConstantBuffer.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VVideo.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vCompiledShaderPass.hpp>

/// \brief
///   Helper class to store and conveniently access register values for shader constant buffers.
/// 
/// Assumes that a variable name is only used in one shader stage.
/// 
/// The register name can also represent a texture sampler; in this case, the SetSamplerTexture function can
/// be used.
/// 
/// \example
///   \code
///   VConstantBufferRegister colorReg(pShader,"TintColor");
///   float pColor[4] = {1,1,1,1};
///   colorReg.SetRegisterValueSafe(pShader,pColors); // can be either vertex or pixel shader constant
///   \endcode
struct VConstantBufferRegister
{
public:

  /// \brief
  ///   Empty Constructor
  VConstantBufferRegister()
  {
    m_iRegister = 0;
    m_iBuffer = VSS_Invalid;
  }

  /// \brief
  ///   Constructor that calls Init: Initializes the register using a shader and a variable name as input
  VConstantBufferRegister(VCompiledShaderPass *pSrc, const char *szRegname)
  {
    Init(pSrc,szRegname);
  }

  /// \brief
  ///   Initializes the register using a shader and a variable name as input
  /// 
  /// \param pSrc
  ///   Source shader used to look up the register in the shader constant buffers.
  ///   Can be NULL to reset the register.
  /// 
  /// \param szRegname
  ///   Variable name (case sensitive) to lookup
  /// 
  /// \return
  ///   bool bResult : true, if the variable could be found in the constant buffers of one of the shader stages.
  VBASE_IMPEXP bool Init(VCompiledShaderPass *pSrc, const char *szRegname);

  /// \brief
  ///   Resets the register so that IsValid returns false
  void Reset()
  {
    m_iBuffer = -1;
    m_iRegister = 0;
  }

  /// \brief
  ///   Indicates whether the Init function was successful (i.e., the register object is correctly initialized)
  bool IsValid() const
  {
    return m_iBuffer>=0;
  }

  /// \brief
  ///   Indicates whether this represents a valid register in vertex or pixel shader constant table
  bool IsValidRegister() const
  {
    return m_iBuffer != VSS_Invalid;
  }

  /// \brief
  ///   Indicates whether this represents a valid sampler texture
  bool IsValidSampler() const
  {
    return (m_iBuffer != VSS_Invalid) && (m_iBuffer&256)>0;
  }

  /// \brief
  ///   Sets multiple 4-component float values on the target register. Requires IsValidRegister()==true
  void SetRegisterValuesF(VCompiledShaderPass *pSrc, const float *pValues, int iNumRegs)
  {
    VASSERT_MSG(IsValidRegister(), "Invalid register.");
    void *pTrg = pSrc->GetConstantBuffer((VShaderStage_e) m_iBuffer)->Lock(m_iRegister, iNumRegs, VIS_LOCKFLAG_DISCARDABLE);
    VASSERT(pTrg != NULL);
    memcpy(pTrg, pValues, iNumRegs * 4 * sizeof(float));
    pSrc->GetConstantBuffer ((VShaderStage_e) m_iBuffer)->Unlock();
  }

  /// \brief
  ///   Uploads an array of matrices into a constant buffer.
  ///
  /// The uploaded matrices are automatically converted to row major or column major layout
  /// so that they may be used without further corrections in the shader.
  ///
  /// Always prefer this function over 'SetRegisterValuesF'.
  ///
  /// \param pSrc
  ///  The shader pass to set the register value on.
  ///
  /// \param pMatrices
  ///  A pointer to the array of matrices to upload.
  ///
  /// \param iNumMatrices
  ///  The number of matrices to set.
  void SetRegisterValuesMatrix(VCompiledShaderPass *pSrc, const hkvMat4* pMatrices, int iNumMatrices = 1)
  {
    VASSERT_MSG(IsValidRegister(), "Invalid register.");
    pSrc->GetConstantBuffer((VShaderStage_e) m_iBuffer)->SetRegisterRangeM(m_iRegister, pMatrices, iNumMatrices);
  }

  /// \brief
  ///   Sets a 4-component float value on the target register. Requires IsValidRegister()==true
  void SetRegisterValueF(VCompiledShaderPass *pSrc, const float *pValues)
  {
    VASSERT_MSG(IsValidRegister(), "Use SetRegisterValueSafe instead");
    pSrc->GetConstantBuffer((VShaderStage_e) m_iBuffer)->SetSingleRegisterF(m_iRegister, pValues);
  }

  /// \brief
  ///   Sets a 4-component float value on the target register if the register is valid
  ///   (IsValidRegister()==true)
  void SetRegisterValueSafeF(VCompiledShaderPass *pSrc, const float *pValues)
  {
    if (IsValidRegister())
      SetRegisterValueF(pSrc, pValues);
  }

  /// \brief
  ///   Sets the texture object on the sampler. Requires IsValidSampler()==true
  void SetSamplerTexture(VCompiledShaderPass *pSrc, VTextureObject *pTexture)
  {
    VASSERT_MSG(IsValidSampler(), "Use SetSamplerTextureSafe instead");
    VStateGroupTexture *pStateGroupTexture = pSrc->GetStateGroupTexture(VSS_PixelShader, m_iRegister);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = pTexture;
  }

  /// \brief
  ///   Sets the texture object on the sampler if IsValidSampler()==true
  void SetSamplerTextureSafe(VCompiledShaderPass *pSrc, VTextureObject *pTexture)
  {
    if (IsValidSampler())
      SetSamplerTexture(pSrc,pTexture);
  }

  short GetRegister (void) const
  {
    return (m_iRegister);
  }

private:
  short m_iRegister;
  short m_iBuffer;
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
