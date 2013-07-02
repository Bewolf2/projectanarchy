/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

class VStateGroupRasterizerDX11 : public VStateGroupRasterizerBase
{
  public:

    VStateGroupRasterizerDX11()
      : VStateGroupRasterizerBase()
    {
    }

    /// \brief
    ///   Assignment operator
    inline VStateGroupRasterizerDX11& operator = (const VStateGroupRasterizerDX11& other)
    {
      VStateGroupRasterizerBase::operator=(other);
      m_spRasterizerState = NULL;//other.m_spRasterizerState;

      return *this;
    }

    /// \brief
    ///   Cleans up internally allocated data. 
    inline void Cleanup()
    {
      VStateGroupRasterizerBase::Cleanup();
      m_spRasterizerState = NULL;
    }

    /// \brief
    ///   Computes a state group hash value for faster comparisons.
    /// 
    /// It is absolutely necessary to call this method before using a state group.
    /// 
    /// On DX10/DX11, this method also creates the actual state object.
    inline void ComputeHash()
    { 
      VStateGroupRasterizerBase::ComputeHash();

      m_spRasterizerState = NULL;
      D3D_RASTERIZER_DESC rasterizerDesc;
      GetD3DDesc(rasterizerDesc);
      VVideo::GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, &m_spRasterizerState.m_pPtr);
    }

    /// \brief
    ///   DX10/DX11 only: Translates the state group to a DX10/DX11 state descriptor.
    inline void GetD3DDesc(D3D_RASTERIZER_DESC& rasterizerDesc)
    {
      rasterizerDesc.FillMode = d3dFillModeTable[m_cFillMode];
      rasterizerDesc.CullMode = d3dCullModeTable[m_cCullMode];
      rasterizerDesc.FrontCounterClockwise = (m_cFlags&VSHADER_RASTERIZERSTATEBIT_FRONTCCW) ? FALSE : TRUE;
      rasterizerDesc.DepthBias = (int)m_fDepthBias;
      rasterizerDesc.DepthBiasClamp = m_fDepthBiasClamp;
      rasterizerDesc.SlopeScaledDepthBias = m_fSlopeScaledDepthBias;
      if (VVideo::GetDXFeatureLevel() >= D3D_FEATURE_LEVEL_10_0)
        rasterizerDesc.DepthClipEnable = (m_cFlags&VSHADER_RASTERIZERSTATEBIT_NODEPTHCLIP) ? FALSE : TRUE;
      else
        rasterizerDesc.DepthClipEnable = TRUE;
      rasterizerDesc.ScissorEnable = (m_cFlags&VSHADER_RASTERIZERSTATEBIT_SCISSORTEST) ? TRUE : FALSE;
      rasterizerDesc.MultisampleEnable = (m_cFlags&VSHADER_RASTERIZERSTATEBIT_MULTISAMPLE) ? TRUE : FALSE;
      #ifdef _VR_DX11
        rasterizerDesc.AntialiasedLineEnable = FALSE;
      #endif
    }

    /// \brief
    ///   Invalidates all states (for internal use)
    inline void Invalidate()
    {
      m_iHash = 0xABCDEF;
    }

    // This has to be the last member!
    D3DRasterizerStatePtr m_spRasterizerState;
};


class VStateGroupDepthStencilDX11 : public VStateGroupDepthStencilBase
{
  public:

    /// \brief
    ///   Assignment operator
    inline VStateGroupDepthStencilDX11 &operator = (const VStateGroupDepthStencilDX11& other)
    {
      VStateGroupDepthStencilBase::operator=(other);

      m_spDepthStencilState = NULL;//other.m_spDepthStencilState;
      return *this;
    }

    /// \brief
    ///   Cleans up internally allocated data. 
    inline void Cleanup()
    {
      VStateGroupDepthStencilBase::Cleanup();

      m_spDepthStencilState = NULL;
    }

    inline void ComputeHash()
    { 
      VStateGroupDepthStencilBase::ComputeHash();

      m_spDepthStencilState = NULL;
      D3D_DEPTH_STENCIL_DESC depthStencilDesc;
      GetD3DDesc(depthStencilDesc);
      VVideo::GetD3DDevice()->CreateDepthStencilState(&depthStencilDesc, &m_spDepthStencilState.m_pPtr);
    }


    /// \brief
    ///   Invalidates all states (for internal use)
    inline void Invalidate() { m_iHash = 0xABCDEF; }

    /// \brief
    ///   DX10/DX11 only: Translates the state group to a DX10/DX11 state descriptor.
    inline void GetD3DDesc(D3D_DEPTH_STENCIL_DESC& depthStencilDesc)
    {
      depthStencilDesc.DepthEnable = m_bDepthTestEnabled ? TRUE : FALSE;
      depthStencilDesc.DepthWriteMask = m_bDepthWriteEnabled ? D3D_DEPTH_WRITE_MASK_ALL : D3D_DEPTH_WRITE_MASK_ZERO;
      depthStencilDesc.DepthFunc = d3dComparisonTable[m_cDepthComparisonFunc];
      depthStencilDesc.StencilEnable = m_bStencilTestEnabled ? TRUE : FALSE;
      depthStencilDesc.StencilReadMask = (UINT8)m_iStencilReadMask;
      depthStencilDesc.StencilWriteMask = (UINT8)m_iStencilWriteMask;
      depthStencilDesc.FrontFace.StencilFailOp = d3dStencilOpTable[m_cStencilFailOp[0]];
      depthStencilDesc.FrontFace.StencilDepthFailOp = d3dStencilOpTable[m_cStencilDepthFailOp[0]];
      depthStencilDesc.FrontFace.StencilPassOp = d3dStencilOpTable[m_cStencilPassOp[0]];
      depthStencilDesc.FrontFace.StencilFunc = d3dComparisonTable[m_cStencilComparisonFunc[0]];

      // if two-sided stencil is disabled, simply use the same values as for the front face:
      const int iIndex = m_bTwoSidedStencil ? 1 : 0;
      depthStencilDesc.BackFace.StencilFailOp = d3dStencilOpTable[m_cStencilFailOp[iIndex]];
      depthStencilDesc.BackFace.StencilDepthFailOp = d3dStencilOpTable[m_cStencilDepthFailOp[iIndex]];
      depthStencilDesc.BackFace.StencilPassOp = d3dStencilOpTable[m_cStencilPassOp[iIndex]];
      depthStencilDesc.BackFace.StencilFunc = d3dComparisonTable[m_cStencilComparisonFunc[iIndex]];
    }

    // This has to be the last member!
    D3DDepthStencilStatePtr m_spDepthStencilState;
};

class VStateGroupBlendDX11 : public VStateGroupBlendBase
{
  public:

    VStateGroupBlendDX11()
      : VStateGroupBlendBase()
    {
      m_spBlendState = NULL;
    }

    /// \brief
    ///   Cleans up internally allocated data. 
    inline void Cleanup()
    {
      m_spBlendState = NULL;
    }

    inline void ComputeHash()
    {
      VStateGroupBlendBase::ComputeHash();

      m_spBlendState = NULL;

      // Fill D3D blend description and create state object
      D3D_BLEND_DESC blendDesc;
      GetD3DDesc(blendDesc);
      VVideo::GetD3DDevice()->CreateBlendState(&blendDesc, &m_spBlendState.m_pPtr);
    }

    /// \brief
    ///   Invalidates all states (for internal use)
    inline void Invalidate()
    {
      m_iHash = 0xABCDEF;
    }

    inline VStateGroupBlendDX11 &operator = (const VStateGroupBlendBase &other)
    {
      VStateGroupBlendBase::operator=(other);
      m_spBlendState = NULL;

      return *this;
    }

    /// \brief
    ///   DX10/DX11 only: Translates the state group to a DX10/DX11 state descriptor.
    inline void GetD3DDesc(D3D_BLEND_DESC& blendDesc)
    {
      // rely on these defines, because this is how the m_iRenderTargetWriteMask member is stored internally
      V_COMPILE_ASSERT(D3D11_COLOR_WRITE_ENABLE_RED==1);
      V_COMPILE_ASSERT(D3D11_COLOR_WRITE_ENABLE_GREEN==2);
      V_COMPILE_ASSERT(D3D11_COLOR_WRITE_ENABLE_BLUE==4);
      V_COMPILE_ASSERT(D3D11_COLOR_WRITE_ENABLE_ALPHA==8);

      blendDesc.AlphaToCoverageEnable = m_bAlphaToCoverageEnabled ? TRUE : FALSE;

      // force the same blend operation across all render targets for DX11 feature level 9_3 and below
      if ( VVideo::GetDXFeatureLevel() <= D3D_FEATURE_LEVEL_9_3 
        && m_cBlendEnable != 0 && m_cBlendEnable != 255)
      {
#if defined( HK_DEBUG )
        OutputDebugStringA( "WARNING: Forcing same blend operation across all render targets for DX11 feature level 9_3 and below.\n" );
#endif
        m_cBlendEnable = m_cBlendEnable ? 0xFF : 0;
      }

      // TODO [DX11]: Add support for independent blend! (i.e. separate blend settings for each RT)
      blendDesc.IndependentBlendEnable = (m_cBlendEnable == 0 || m_cBlendEnable == 0xFF) ? FALSE : TRUE;

#ifdef _VISION_METRO
      if (VVideo::GetDXFeatureLevel() < D3D_FEATURE_LEVEL_10_0)
      {
          if ( m_iRenderTargetWriteMask != 0)
              m_iRenderTargetWriteMask = 0xffffffff; // not seprable
      }
#endif

      for (int i=0; i<8; i++)
      {
        blendDesc.RenderTarget[i].BlendEnable = ((m_cBlendEnable & (1<<i))>0) ? TRUE : FALSE;
        blendDesc.RenderTarget[i].SrcBlend = d3dBlendTable[m_cSrcBlend];
        blendDesc.RenderTarget[i].DestBlend = d3dBlendTable[m_cDestBlend];
        blendDesc.RenderTarget[i].BlendOp = d3dBlendOpTable[m_cBlendOp]; 
        blendDesc.RenderTarget[i].BlendOpAlpha = d3dBlendOpTable[m_cBlendOpAlpha];
        blendDesc.RenderTarget[i].RenderTargetWriteMask = (m_iRenderTargetWriteMask >> (i*4)) & 15;

        // separate alpha blend
        if (m_bSeparateAlphaBlend)
        {
          blendDesc.RenderTarget[i].SrcBlendAlpha = d3dBlendTable[m_cSrcBlendAlpha];
          blendDesc.RenderTarget[i].DestBlendAlpha = d3dBlendTable[m_cDestBlendAlpha];
        }
        // copy color blend factors if separate alpha blend is disabled
        else
        {
          // Use alpha blend factor table in order to translate color blend factors
          // to its equivalent alpha version, i.e. SRC_COLOR -> SRC_ALPHA.
          // (this is the same behavior as in Dx9: all channels (RGBA) are used if 
          // SRC_COLOR is specified and separate alpha blend is off)
          blendDesc.RenderTarget[i].SrcBlendAlpha = d3dBlendAlphaTable[m_cSrcBlend];
          blendDesc.RenderTarget[i].DestBlendAlpha = d3dBlendAlphaTable[m_cDestBlend];
        }

        // independent blend
        if (blendDesc.RenderTarget[i].BlendEnable != blendDesc.RenderTarget[0].BlendEnable || 
          blendDesc.RenderTarget[i].RenderTargetWriteMask != blendDesc.RenderTarget[0].RenderTargetWriteMask)
        {
          blendDesc.IndependentBlendEnable = TRUE;
        }
      }
    }

    // This has to be the last member!
    D3DBlendStatePtr m_spBlendState;
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
