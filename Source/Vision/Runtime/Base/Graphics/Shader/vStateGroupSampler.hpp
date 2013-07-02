/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vStateGroupSampler.hpp

#ifndef VSTATEGROUPSAMPLER_HPP_INCLUDED
#define VSTATEGROUPSAMPLER_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vConstantBuffer.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VVideo.hpp>

/// \brief
///   State group that controls the states of a single texture sampler stage. In a shader, there is
///   a 1:1 mapping between VStateGroupTexture and VStateGroupSampler
struct VStateGroupSamplerBase
{
  /// \brief
  ///   Constructor; Sets defaults
  inline VStateGroupSamplerBase()
  {
    memset(this, 0, sizeof(VStateGroupSamplerBase)); ///< turn off everything by default
    m_cFilterMode = FILTER_DEFAULT;//_MIN_MAG_MIP_POINT;
    m_cTextureAddressMode[0] = TEXTURE_ADDRESS_WRAP;
    m_cTextureAddressMode[1] = TEXTURE_ADDRESS_WRAP;
    m_cTextureAddressMode[2] = TEXTURE_ADDRESS_WRAP;
    m_fLodBias = 0.f;
    m_iMaxAnisotropy = 1;
    m_cComparisonFunc = COMPARISON_LESS;
    m_fMinLod = 0.f;
    m_fMaxLod = 1024.f;
    m_iHash = 0;
    m_bIsVertexTexture = false;
    m_iTextureSizeRegister = -1;
    m_iTextureSizeConstantNameHash = 0;
    m_iLastTextureHeight= -1;
    m_iLastTextureWidth = -1;
    m_iLastTextureDepth = -1;
  }

  /// \brief
  ///   Assignment operator
  inline VStateGroupSamplerBase &operator = (const VStateGroupSamplerBase &other)
  {
    memcpy(this, &other, offsetof(VStateGroupSamplerBase, m_sTextureSizeConstantName));
    m_sTextureSizeConstantName = other.m_sTextureSizeConstantName;
    m_iLastTextureWidth = -1;
    m_iLastTextureHeight = -1;
    m_iLastTextureDepth = -1;
    m_iHash = other.m_iHash;

    return *this;
  }

  /// \brief
  ///   Comparison operator
  inline bool operator == (const VStateGroupSamplerBase &other) const
  {
    if (m_iHash!=other.m_iHash) return false;
    return memcmp(this, &other, offsetof(VStateGroupSamplerBase, m_sTextureSizeConstantName)) == 0;
  }

  /// \brief
  ///   Comparison operator
  inline bool operator != (const VStateGroupSamplerBase &other) const
  {
    return !(*this == other);
  }  

  /// \brief
  ///   Sets all sampler values to specific minimum/maximum (platform dependent)
  VBASE_IMPEXP void ClampValuesToValidRange();

  /// \brief
  ///   Cleans up internally allocated data. 
  inline void Cleanup()
  {
    m_sTextureSizeConstantName = NULL;
  }

  /// \brief
  ///   Computes the hash value for this state group. This function has to be called prior to using the state group.
  ///
  /// This function always has to be called after setting up a new state group or after modifying an existing state group.
  /// It computes a hash which is internally used for optimization purposes, and additionally generates platforms-specific
  /// data which is critical to the use of the state group.
  inline void ComputeHash()
  {
    ClampValuesToValidRange(); ///< TODO: Remove once this check is performed upon loading of binary files
    m_iHash = ComputeHashInternal();
  }

  /// \brief
  ///   Invalidates all states (for internal use)
  inline void Invalidate() 
  {
    ::InvalidateStates(this, offsetof(VStateGroupSamplerBase, m_sTextureSizeConstantName));
    m_sTextureSizeConstantName = NULL;
    m_iTextureSizeRegister = -1;
    m_iLastTextureHeight = -1;
    m_iLastTextureWidth = -1;
    m_iLastTextureDepth = -1;
    m_iHash = 0;
  }

  /// \brief
  ///   Verifies the correctness of the hash. Only used for debugging purposes.
  inline bool IsHashValid() const
  {
    return m_iHash == ComputeHashInternal();
  }

  /// \brief
  ///   Helper function to dump the current states into a text file in a readable format
  VBASE_IMPEXP void DumpToFile(IVFileOutStream *pOut) const;

  /// \brief
  ///   Binary file serialization
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

private:
  inline unsigned int ComputeHashInternal() const
  {  
    return ::ComputeHash(this, offsetof(VStateGroupSamplerBase, m_sTextureSizeConstantName));
  }

public:
  unsigned char m_cFilterMode;                   ///< Sample filtering mode to use for this sample. Must be one of the members of the VFilterMode enumeration.
  unsigned char m_cTextureAddressMode[3];        ///< Texture address mode. Must be one of the members of the VTextureAddressMode enumeration.
  float m_fLodBias;                              ///< Mip Map LOD Bias (negative values prefer higher-resolution mip levels)
  unsigned int m_iMaxAnisotropy;                 ///< Maximum anisotropy for this sampler (only used if filter mode is set to anisotropic)
  unsigned char m_cComparisonFunc;               ///< Sampler comparison function. Must be one of the members of the VComparisonFunc enumeration.
  hkvVec4 m_fBorderColor;                        ///< Border color (if border clamping is used as address mode)
  float m_fMinLod;                               ///< Minimum LOD value (i.e., Mip Level) to use
  float m_fMaxLod;                               ///< Maximum LOD value (i.e., Mip Level) to use
  bool m_bIsVertexTexture;                       ///< true to indicate that this sampler will be used to sample in a vertex shader. Not serialized.
  int m_iTextureSizeRegister;                    ///< Shader constant register in which the texture size is stored. Internal use only; not serialized.
  unsigned int m_iTextureSizeConstantNameHash;   ///< Hash of name of the shader constant in which to store the texture size. Internal use only, but hashed and serialized.
  VString m_sTextureSizeConstantName;            ///< Name of the shader constant in which to store the texture size. Internal use only, not hashed but serialized in shaderlib only.
  short m_iLastTextureWidth, m_iLastTextureHeight, m_iLastTextureDepth; ///< Internal use only. Not hashed, not serialized.

  unsigned int m_iHash;
};

#if defined(_VR_DX11)
// Special code for DX11 state groups (creating state objects etc.)
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroupSamplerDX11.inl>

typedef VStateGroupSamplerDX11 VStateGroupSampler;

#elif defined(_VR_GX2)
// Special code for GX2 state groups (creating state objects etc.)
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroupSamplerGX2.inl>

typedef VStateGroupSamplerGX2 VStateGroupSampler;

#else
typedef VStateGroupSamplerBase VStateGroupSampler;

#endif


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
