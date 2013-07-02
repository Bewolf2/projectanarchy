/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <D3D11.h>

#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

#define D3D_FEATURE_LEVEL_HEADLESS D3D_FEATURE_LEVEL_11_0

class D3D11DeviceDummy : public ID3D11Device, public UnknownDummy
{
public:
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID,__RPC__deref_out void __RPC_FAR *__RPC_FAR *) HKV_OVERRIDE { return E_NOTIMPL; }
  virtual ULONG STDMETHODCALLTYPE AddRef( void) HKV_OVERRIDE { return UnknownDummy::AddRef(); }
  virtual ULONG STDMETHODCALLTYPE Release( void) HKV_OVERRIDE { return UnknownDummy::Release(); }

  virtual HRESULT STDMETHODCALLTYPE CreateTexture1D( 
    /* [annotation] */ 
    __in  const D3D11_TEXTURE1D_DESC *pDesc,
    /* [annotation] */ 
    __in_xcount_opt(pDesc->MipLevels * pDesc->ArraySize)  const D3D11_SUBRESOURCE_DATA *pInitialData,
    /* [annotation] */ 
    __out_opt  ID3D11Texture1D **ppTexture1D) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE CreateHullShader( 
    /* [annotation] */ 
    __in  const void *pShaderBytecode,
    /* [annotation] */ 
    __in  SIZE_T BytecodeLength,
    /* [annotation] */ 
    __in_opt  ID3D11ClassLinkage *pClassLinkage,
    /* [annotation] */ 
    __out_opt  ID3D11HullShader **ppHullShader) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE CreateDomainShader( 
    /* [annotation] */ 
    __in  const void *pShaderBytecode,
    /* [annotation] */ 
    __in  SIZE_T BytecodeLength,
    /* [annotation] */ 
    __in_opt  ID3D11ClassLinkage *pClassLinkage,
    /* [annotation] */ 
    __out_opt  ID3D11DomainShader **ppDomainShader) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage( 
    /* [annotation] */ 
    __out  ID3D11ClassLinkage **ppLinkage) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE CreatePredicate( 
    /* [annotation] */ 
    __in  const D3D11_QUERY_DESC *pPredicateDesc,
    /* [annotation] */ 
    __out_opt  ID3D11Predicate **ppPredicate) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE CreateCounter( 
    /* [annotation] */ 
    __in  const D3D11_COUNTER_DESC *pCounterDesc,
    /* [annotation] */ 
    __out_opt  ID3D11Counter **ppCounter) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext( 
    UINT ContextFlags,
    /* [annotation] */ 
    __out_opt  ID3D11DeviceContext **ppDeferredContext) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE OpenSharedResource( 
    /* [annotation] */ 
    __in  HANDLE hResource,
    /* [annotation] */ 
    __in  REFIID ReturnedInterface,
    /* [annotation] */ 
    __out_opt  void **ppResource) HKV_NR_UNIMPLEMENTED

    virtual void STDMETHODCALLTYPE CheckCounterInfo( 
    /* [annotation] */ 
    __out  D3D11_COUNTER_INFO *pCounterInfo) HKV_NR_UNIMPLEMENTED_VOID

    virtual HRESULT STDMETHODCALLTYPE CheckCounter( 
    /* [annotation] */ 
    __in  const D3D11_COUNTER_DESC *pDesc,
    /* [annotation] */ 
    __out  D3D11_COUNTER_TYPE *pType,
    /* [annotation] */ 
    __out  UINT *pActiveCounters,
    /* [annotation] */ 
    __out_ecount_opt(*pNameLength)  LPSTR szName,
    /* [annotation] */ 
    __inout_opt  UINT *pNameLength,
    /* [annotation] */ 
    __out_ecount_opt(*pUnitsLength)  LPSTR szUnits,
    /* [annotation] */ 
    __inout_opt  UINT *pUnitsLength,
    /* [annotation] */ 
    __out_ecount_opt(*pDescriptionLength)  LPSTR szDescription,
    /* [annotation] */ 
    __inout_opt  UINT *pDescriptionLength) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport( 
    D3D11_FEATURE Feature,
    /* [annotation] */ 
    __out_bcount(FeatureSupportDataSize)  void *pFeatureSupportData,
    UINT FeatureSupportDataSize) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
    /* [annotation] */ 
    __in  REFGUID guid,
    /* [annotation] */ 
    __inout  UINT *pDataSize,
    /* [annotation] */ 
    __out_bcount_opt(*pDataSize)  void *pData) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
    /* [annotation] */ 
    __in  REFGUID guid,
    /* [annotation] */ 
    __in  UINT DataSize,
    /* [annotation] */ 
    __in_bcount_opt(DataSize)  const void *pData) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
    /* [annotation] */ 
    __in  REFGUID guid,
    /* [annotation] */ 
    __in_opt  const IUnknown *pData) HKV_NR_UNIMPLEMENTED

    virtual UINT STDMETHODCALLTYPE GetCreationFlags( void) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason( void) HKV_NR_UNIMPLEMENTED

    virtual void STDMETHODCALLTYPE GetImmediateContext( 
    /* [annotation] */ 
    __out  ID3D11DeviceContext **ppImmediateContext) HKV_NR_UNIMPLEMENTED_VOID

    virtual HRESULT STDMETHODCALLTYPE SetExceptionMode( 
    UINT RaiseFlags) HKV_NR_UNIMPLEMENTED

    virtual UINT STDMETHODCALLTYPE GetExceptionMode( void) HKV_NR_UNIMPLEMENTED

    // Implemented:
    virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport(DXGI_FORMAT Format, UINT *pFormatSupport) HKV_OVERRIDE;
    virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateBlendState(const D3D11_BLEND_DESC *pBlendStateDesc, ID3D11BlendState **ppBlendState) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateBuffer(const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateComputeShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11ComputeShader **ppComputeShader) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D11DepthStencilState **ppDepthStencilState) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView(ID3D11Resource *pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D11DepthStencilView **ppDepthStencilView) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput(const void *pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreatePixelShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateQuery(const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery);
  virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState(const D3D11_RASTERIZER_DESC *pRasterizerDesc, ID3D11RasterizerState **ppRasterizerState) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView(ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC *pDesc, ID3D11RenderTargetView **ppRTView) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateSamplerState(const D3D11_SAMPLER_DESC *pSamplerDesc, ID3D11SamplerState **ppSamplerState) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView(ID3D11Resource *pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D11ShaderResourceView **ppSRView);
  virtual HRESULT STDMETHODCALLTYPE CreateTexture2D(const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateTexture3D(const D3D11_TEXTURE3D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture3D **ppTexture3D) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView(ID3D11Resource *pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc, ID3D11UnorderedAccessView **ppUAView) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE CreateVertexShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader) HKV_OVERRIDE;
  virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel() HKV_OVERRIDE;
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
