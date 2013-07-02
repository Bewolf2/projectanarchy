/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#pragma warning(push)
#pragma warning(disable: 28204)  // Ignore mismatch between static analysis annotations

#include <D3D11.h>

#include <Vision/Runtime/Base/Graphics/Headless/D3D11DeviceChildDummy.h>

class D3D11DeviceContextDummy : public ID3D11DeviceContext, public UnknownDummy
{
public:
  HKV_NR_IMPLEMENT_IUNKNOWN;
  HKV_NR_IMPLEMENT_ID3D11DEVICECHILD;

  virtual void STDMETHODCALLTYPE DrawInstanced( 
    /* [annotation] */ 
    __in  UINT VertexCountPerInstance,
    /* [annotation] */ 
    __in  UINT InstanceCount,
    /* [annotation] */ 
    __in  UINT StartVertexLocation,
    /* [annotation] */ 
    __in  UINT StartInstanceLocation) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE SetPredication( 
    /* [annotation] */ 
    __in_opt  ID3D11Predicate *pPredicate,
    /* [annotation] */ 
    __in  BOOL PredicateValue) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE GSSetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews( 
    /* [annotation] */ 
    __in  UINT NumRTVs,
    /* [annotation] */ 
    __in_ecount_opt(NumRTVs)  ID3D11RenderTargetView *const *ppRenderTargetViews,
    /* [annotation] */ 
    __in_opt  ID3D11DepthStencilView *pDepthStencilView,
    /* [annotation] */ 
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
    /* [annotation] */ 
    __in  UINT NumUAVs,
    /* [annotation] */ 
    __in_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
    /* [annotation] */ 
    __in_ecount_opt(NumUAVs)  const UINT *pUAVInitialCounts) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DrawAuto( void) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DrawIndexedInstancedIndirect( 
    /* [annotation] */ 
    __in  ID3D11Buffer *pBufferForArgs,
    /* [annotation] */ 
    __in  UINT AlignedByteOffsetForArgs) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DrawInstancedIndirect( 
    /* [annotation] */ 
    __in  ID3D11Buffer *pBufferForArgs,
    /* [annotation] */ 
    __in  UINT AlignedByteOffsetForArgs) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewUint( 
    /* [annotation] */ 
    __in  ID3D11UnorderedAccessView *pUnorderedAccessView,
    /* [annotation] */ 
    __in  const UINT Values[ 4 ]) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat( 
    /* [annotation] */ 
    __in  ID3D11UnorderedAccessView *pUnorderedAccessView,
    /* [annotation] */ 
    __in  const FLOAT Values[ 4 ]) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE SetResourceMinLOD( 
    /* [annotation] */ 
    __in  ID3D11Resource *pResource,
    FLOAT MinLOD) HKV_NR_UNIMPLEMENTED_VOID

    virtual FLOAT STDMETHODCALLTYPE GetResourceMinLOD( 
    /* [annotation] */ 
    __in  ID3D11Resource *pResource) HKV_NR_UNIMPLEMENTED_FLOAT

    virtual void STDMETHODCALLTYPE ExecuteCommandList( 
    /* [annotation] */ 
    __in  ID3D11CommandList *pCommandList,
    BOOL RestoreContextState) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE HSSetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE HSSetShader( 
    /* [annotation] */ 
    __in_opt  ID3D11HullShader *pHullShader,
    /* [annotation] */ 
    __in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
    UINT NumClassInstances) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DSSetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __in_ecount(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DSSetShader( 
    /* [annotation] */ 
    __in_opt  ID3D11DomainShader *pDomainShader,
    /* [annotation] */ 
    __in_ecount_opt(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
    UINT NumClassInstances) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE VSGetConstantBuffers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */ 
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE PSGetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE PSGetShader( 
    /* [annotation] */ 
    __out  ID3D11PixelShader **ppPixelShader,
    /* [annotation] */ 
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */ 
    __inout_opt  UINT *pNumClassInstances) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE PSGetSamplers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */ 
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE VSGetShader( 
    /* [annotation] */ 
    __out  ID3D11VertexShader **ppVertexShader,
    /* [annotation] */ 
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */ 
    __inout_opt  UINT *pNumClassInstances) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE PSGetConstantBuffers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */ 
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE IAGetInputLayout( 
    /* [annotation] */ 
    __out  ID3D11InputLayout **ppInputLayout) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE IAGetVertexBuffers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */ 
    __out_ecount_opt(NumBuffers)  ID3D11Buffer **ppVertexBuffers,
    /* [annotation] */ 
    __out_ecount_opt(NumBuffers)  UINT *pStrides,
    /* [annotation] */ 
    __out_ecount_opt(NumBuffers)  UINT *pOffsets) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE IAGetIndexBuffer( 
    /* [annotation] */ 
    __out_opt  ID3D11Buffer **pIndexBuffer,
    /* [annotation] */ 
    __out_opt  DXGI_FORMAT *Format,
    /* [annotation] */ 
    __out_opt  UINT *Offset) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE GSGetConstantBuffers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */ 
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE GSGetShader( 
    /* [annotation] */ 
    __out  ID3D11GeometryShader **ppGeometryShader,
    /* [annotation] */ 
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */ 
    __inout_opt  UINT *pNumClassInstances) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology( 
    /* [annotation] */ 
    __out  D3D11_PRIMITIVE_TOPOLOGY *pTopology) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE VSGetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE VSGetSamplers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */ 
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE GetPredication( 
    /* [annotation] */ 
    __out_opt  ID3D11Predicate **ppPredicate,
    /* [annotation] */ 
    __out_opt  BOOL *pPredicateValue) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE GSGetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE GSGetSamplers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */ 
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE OMGetRenderTargets( 
    /* [annotation] */ 
    __in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
    /* [annotation] */ 
    __out_ecount_opt(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews,
    /* [annotation] */ 
    __out_opt  ID3D11DepthStencilView **ppDepthStencilView) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews( 
    /* [annotation] */ 
    __in_range( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumRTVs,
    /* [annotation] */ 
    __out_ecount_opt(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews,
    /* [annotation] */ 
    __out_opt  ID3D11DepthStencilView **ppDepthStencilView,
    /* [annotation] */ 
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot )  UINT NumUAVs,
    /* [annotation] */ 
    __out_ecount_opt(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE OMGetBlendState( 
    /* [annotation] */ 
    __out_opt  ID3D11BlendState **ppBlendState,
    /* [annotation] */ 
    __out_opt  FLOAT BlendFactor[ 4 ],
    /* [annotation] */ 
    __out_opt  UINT *pSampleMask) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE OMGetDepthStencilState( 
    /* [annotation] */ 
    __out_opt  ID3D11DepthStencilState **ppDepthStencilState,
    /* [annotation] */ 
    __out_opt  UINT *pStencilRef) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE SOGetTargets( 
    /* [annotation] */ 
    __in_range( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumBuffers,
    /* [annotation] */ 
    __out_ecount(NumBuffers)  ID3D11Buffer **ppSOTargets) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE RSGetState( 
    /* [annotation] */ 
    __out  ID3D11RasterizerState **ppRasterizerState) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE RSGetViewports( 
    /* [annotation] */ 
    __inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports,
    /* [annotation] */ 
    __out_ecount_opt(*pNumViewports)  D3D11_VIEWPORT *pViewports) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE RSGetScissorRects( 
    /* [annotation] */ 
    __inout /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects,
    /* [annotation] */ 
    __out_ecount_opt(*pNumRects)  D3D11_RECT *pRects) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE HSGetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE HSGetShader( 
    /* [annotation] */ 
    __out  ID3D11HullShader **ppHullShader,
    /* [annotation] */ 
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */ 
    __inout_opt  UINT *pNumClassInstances) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE HSGetSamplers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */ 
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE HSGetConstantBuffers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */ 
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DSGetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DSGetShader( 
    /* [annotation] */ 
    __out  ID3D11DomainShader **ppDomainShader,
    /* [annotation] */ 
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */ 
    __inout_opt  UINT *pNumClassInstances) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DSGetSamplers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */ 
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE DSGetConstantBuffers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */ 
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE CSGetShaderResources( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
    /* [annotation] */ 
    __out_ecount(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE CSGetUnorderedAccessViews( 
    /* [annotation] */ 
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - StartSlot )  UINT NumUAVs,
    /* [annotation] */ 
    __out_ecount(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE CSGetShader( 
    /* [annotation] */ 
    __out  ID3D11ComputeShader **ppComputeShader,
    /* [annotation] */ 
    __out_ecount_opt(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
    /* [annotation] */ 
    __inout_opt  UINT *pNumClassInstances) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE CSGetSamplers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
    /* [annotation] */ 
    __out_ecount(NumSamplers)  ID3D11SamplerState **ppSamplers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE CSGetConstantBuffers( 
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
    /* [annotation] */ 
    __in_range( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
    /* [annotation] */ 
    __out_ecount(NumBuffers)  ID3D11Buffer **ppConstantBuffers) HKV_NR_UNIMPLEMENTED_VOID

    virtual void STDMETHODCALLTYPE Flush( void) HKV_NR_UNIMPLEMENTED_VOID

    virtual UINT STDMETHODCALLTYPE GetContextFlags( void) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE FinishCommandList( 
    BOOL RestoreDeferredContextState,
    /* [annotation] */ 
    __out_opt  ID3D11CommandList **ppCommandList) HKV_NR_UNIMPLEMENTED

    // Implemented:
    virtual void STDMETHODCALLTYPE Begin(__in ID3D11Asynchronous *pAsync) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE ClearDepthStencilView(__in ID3D11DepthStencilView *pDepthStencilView, __in UINT ClearFlags, __in FLOAT Depth, __in UINT8 Stencil) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE ClearRenderTargetView(__in ID3D11RenderTargetView *pRenderTargetView, __in const FLOAT ColorRGBA[4]) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE ClearState() HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE CopyResource(__in  ID3D11Resource *pDstResource, __in  ID3D11Resource *pSrcResource) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE CopyStructureCount(ID3D11Buffer *pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView *pSrcView) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE CopySubresourceRegion(ID3D11Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource *pSrcResource, UINT SrcSubresource, const D3D11_BOX *pSrcBox) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE CSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE CSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE CSSetShader(ID3D11ComputeShader *pComputeShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE CSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE CSSetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE DispatchIndirect(ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE Draw(UINT VertexCount, UINT StartVertexLocation) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE DSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE DSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE End(ID3D11Asynchronous *pAsync) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE GenerateMips(ID3D11ShaderResourceView *pShaderResourceView) HKV_OVERRIDE {}
  virtual HRESULT STDMETHODCALLTYPE GetData(ID3D11Asynchronous *pAsync, void *pData, UINT DataSize, UINT GetDataFlags) HKV_OVERRIDE;
  virtual D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType() HKV_OVERRIDE;
  virtual void STDMETHODCALLTYPE GSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE GSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE GSSetShader(ID3D11GeometryShader *pShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE HSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE HSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE IASetIndexBuffer(ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset) {}
  virtual void STDMETHODCALLTYPE IASetInputLayout(ID3D11InputLayout *pInputLayout) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets) HKV_OVERRIDE {}
  virtual HRESULT STDMETHODCALLTYPE Map(ID3D11Resource *pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE *pMappedResource) HKV_OVERRIDE;
  virtual void STDMETHODCALLTYPE OMSetBlendState(ID3D11BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE OMSetDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE OMSetRenderTargets(UINT NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE PSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE PSSetShader(ID3D11PixelShader *pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE ResolveSubresource(ID3D11Resource *pDstResource, UINT DstSubresource, ID3D11Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE RSSetScissorRects(UINT NumRects, const D3D11_RECT *pRects) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE RSSetState(ID3D11RasterizerState *pRasterizerState) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE RSSetViewports(UINT NumViewports, const D3D11_VIEWPORT *pViewports) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE SOSetTargets(UINT NumBuffers, ID3D11Buffer *const *ppSOTargets, const UINT *pOffsets) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE Unmap(ID3D11Resource *pResource, UINT Subresource) HKV_OVERRIDE;
  virtual void STDMETHODCALLTYPE UpdateSubresource(ID3D11Resource *pDstResource, UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE VSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE VSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE VSSetShader(ID3D11VertexShader *pVertexShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) HKV_OVERRIDE {}
  virtual void STDMETHODCALLTYPE VSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) HKV_OVERRIDE {}
};

#pragma warning(pop)

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
