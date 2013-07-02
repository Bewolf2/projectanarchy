/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

typedef VSmartPtr<D3DBlendState> D3DBlendStatePtr;
typedef VSmartPtr<D3DDepthStencilState> D3DDepthStencilStatePtr;
typedef VSmartPtr<D3DRasterizerState> D3DRasterizerStatePtr;
typedef VSmartPtr<D3DSamplerState> D3DSamplerStatePtr;

VBASE_IMPEXP extern D3D_BLEND d3dBlendTable[];
VBASE_IMPEXP extern D3D_BLEND d3dBlendAlphaTable[];
VBASE_IMPEXP extern D3D_BLEND_OP d3dBlendOpTable[];
VBASE_IMPEXP extern D3D_COMPARISON_FUNC d3dComparisonTable[];
VBASE_IMPEXP extern D3D_STENCIL_OP d3dStencilOpTable[];
VBASE_IMPEXP extern D3D_FILL_MODE d3dFillModeTable[];
VBASE_IMPEXP extern D3D_CULL_MODE d3dCullModeTable[];
VBASE_IMPEXP extern D3D_FILTER d3dFilterModeTable[];
VBASE_IMPEXP extern D3D_TEXTURE_ADDRESS_MODE d3dTextureAddressTable[];

class VStateGroupSamplerDX11 : public VStateGroupSamplerBase
{
  public:

    VStateGroupSamplerDX11()
      : VStateGroupSamplerBase()
    {
    }

    /// \brief
    ///   Assignment operator
    inline VStateGroupSamplerDX11& operator = (const VStateGroupSamplerDX11& other)
    {
      VStateGroupSamplerBase::operator=(other);
      m_spSamplerState = NULL;

      return *this;
    }

    /// \brief
    ///   Cleans up internally allocated data. 
    inline void Cleanup()
    {
      VStateGroupSamplerBase::Cleanup();
      m_spSamplerState = NULL;
    }

    /// \brief
    ///   Computes a state group hash value for faster comparisons.
    /// 
    /// It is absolutely necessary to call this method before using a state group.
    /// 
    /// On DX10/DX11, this method also creates the actual state object.
    inline void ComputeHash() 
    { 
      VStateGroupSamplerBase::ComputeHash();

      m_spSamplerState = NULL;
      if (VVideo::GetD3DDevice() != NULL)
      {
        D3D_SAMPLER_DESC samplerDesc;
        GetD3DDesc(samplerDesc);
        VVideo::GetD3DDevice()->CreateSamplerState(&samplerDesc, &m_spSamplerState.m_pPtr);
      }
    }

    /// \brief
    ///   DX10/DX11 only: Translates the state group to a DX10/DX11 state descriptor.
    inline void GetD3DDesc(D3D_SAMPLER_DESC &samplerDesc)
    {
      samplerDesc.Filter = d3dFilterModeTable[m_cFilterMode];
      samplerDesc.AddressU = d3dTextureAddressTable[m_cTextureAddressMode[0]];
      samplerDesc.AddressV = d3dTextureAddressTable[m_cTextureAddressMode[1]];
      samplerDesc.AddressW = d3dTextureAddressTable[m_cTextureAddressMode[2]];
      samplerDesc.MipLODBias = m_fLodBias;
      // Hack for old shaders
      samplerDesc.MaxAnisotropy = (m_iMaxAnisotropy<=1 && samplerDesc.Filter == D3D11_FILTER_ANISOTROPIC) ? 16 : m_iMaxAnisotropy;
      samplerDesc.ComparisonFunc = d3dComparisonTable[m_cComparisonFunc];
      for (int i=0; i<4; i++)
        samplerDesc.BorderColor[i] = m_fBorderColor.data[i];
      samplerDesc.MinLOD = 0.0f;//FIXME m_fMinLod;
      samplerDesc.MaxLOD = FLT_MAX;//FIXME m_fMaxLod;
    }

    static inline void SetDefaultFilterMode(VFilterMode defaultFilterMode)
    {
      VASSERT(defaultFilterMode < FILTER_DEFAULT);
      d3dFilterModeTable[FILTER_DEFAULT] = d3dFilterModeTable[defaultFilterMode];
    }

    /// \brief
    ///   Invalidates all states (for internal use)
    inline void Invalidate() 
    { 
      m_iHash = 0xABCDEF;
      m_iTextureSizeConstantNameHash = 0;
      m_sTextureSizeConstantName = NULL;
      m_iTextureSizeRegister = -1;
      m_iLastTextureHeight = -1;
      m_iLastTextureWidth = -1;
      m_iLastTextureDepth = -1;
    }

    D3DSamplerStatePtr m_spSamplerState;
};

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
