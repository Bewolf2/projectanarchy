/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <d3d9.h>

#include <Vision/Runtime/Base/Graphics/Headless/Direct3D9Dummy.h>
#include <Vision/Runtime/Base/Graphics/Headless/Direct3DSurface9Dummy.h>
#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

#include <Vision/Runtime/Base/VBase.hpp>

class Direct3DDevice9Dummy : public IDirect3DDevice9, public UnknownDummy
{
public:
  Direct3DDevice9Dummy(Direct3D9Dummy* pDirect3D9) : m_spDirect3D9(pDirect3D9) {}

  HKV_NR_IMPLEMENT_IUNKNOWN;

  /*** IDirect3DDevice9 methods ***/
  STDMETHOD_(UINT, GetAvailableTextureMem)(THIS) HKV_NR_UNIMPLEMENTED
  STDMETHOD(EvictManagedResources)(THIS) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetDisplayMode)(THIS_ UINT iSwapChain,D3DDISPLAYMODE* pMode) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetCreationParameters)(THIS_ D3DDEVICE_CREATION_PARAMETERS *pParameters) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetCursorProperties)(THIS_ UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap) HKV_NR_UNIMPLEMENTED
  STDMETHOD_(void, SetCursorPosition)(THIS_ int X,int Y,DWORD Flags) {}
  STDMETHOD_(BOOL, ShowCursor)(THIS_ BOOL bShow) HKV_NR_UNIMPLEMENTED_BOOL
  STDMETHOD(CreateAdditionalSwapChain)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetSwapChain)(THIS_ UINT iSwapChain,IDirect3DSwapChain9** pSwapChain) HKV_NR_UNIMPLEMENTED
  STDMETHOD_(UINT, GetNumberOfSwapChains)(THIS) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetBackBuffer)(THIS_ UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetRasterStatus)(THIS_ UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetDialogBoxMode)(THIS_ BOOL bEnableDialogs) HKV_NR_UNIMPLEMENTED
  STDMETHOD(UpdateSurface)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) HKV_NR_UNIMPLEMENTED
  STDMETHOD(UpdateTexture)(THIS_ IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetRenderTargetData)(THIS_ IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetFrontBufferData)(THIS_ UINT iSwapChain,IDirect3DSurface9* pDestSurface) HKV_NR_UNIMPLEMENTED
  STDMETHOD(ColorFill)(THIS_ IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color) HKV_NR_UNIMPLEMENTED
  STDMETHOD(CreateOffscreenPlainSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetTransform)(THIS_ D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix) HKV_NR_UNIMPLEMENTED
  STDMETHOD(MultiplyTransform)(THIS_ D3DTRANSFORMSTATETYPE,CONST D3DMATRIX*) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetViewport)(THIS_ D3DVIEWPORT9* pViewport) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9* pMaterial) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetMaterial)(THIS_ D3DMATERIAL9* pMaterial) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetLight)(THIS_ DWORD Index,CONST D3DLIGHT9*) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetLight)(THIS_ DWORD Index,D3DLIGHT9*) HKV_NR_UNIMPLEMENTED
  STDMETHOD(LightEnable)(THIS_ DWORD Index,BOOL Enable) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetLightEnable)(THIS_ DWORD Index,BOOL* pEnable) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetClipPlane)(THIS_ DWORD Index,CONST float* pPlane) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetClipPlane)(THIS_ DWORD Index,float* pPlane) HKV_NR_UNIMPLEMENTED
  STDMETHOD(CreateStateBlock)(THIS_ D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB) HKV_NR_UNIMPLEMENTED
  STDMETHOD(BeginStateBlock)(THIS) HKV_NR_UNIMPLEMENTED
  STDMETHOD(EndStateBlock)(THIS_ IDirect3DStateBlock9** ppSB) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetClipStatus)(THIS_ CONST D3DCLIPSTATUS9* pClipStatus) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetClipStatus)(THIS_ D3DCLIPSTATUS9* pClipStatus) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue) HKV_NR_UNIMPLEMENTED
  STDMETHOD(ValidateDevice)(THIS_ DWORD* pNumPasses) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetPaletteEntries)(THIS_ UINT PaletteNumber,CONST PALETTEENTRY* pEntries) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetPaletteEntries)(THIS_ UINT PaletteNumber,PALETTEENTRY* pEntries) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetCurrentTexturePalette)(THIS_ UINT PaletteNumber) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetCurrentTexturePalette)(THIS_ UINT *PaletteNumber) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetScissorRect)(THIS_ RECT* pRect) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetSoftwareVertexProcessing)(THIS_ BOOL bSoftware) HKV_NR_UNIMPLEMENTED
  STDMETHOD_(BOOL, GetSoftwareVertexProcessing)(THIS) HKV_NR_UNIMPLEMENTED_BOOL
  STDMETHOD(SetNPatchMode)(THIS_ float nSegments) HKV_NR_UNIMPLEMENTED
  STDMETHOD_(float, GetNPatchMode)(THIS) HKV_NR_UNIMPLEMENTED_FLOAT
  STDMETHOD(ProcessVertices)(THIS_ UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9** ppDecl) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetVertexShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetVertexShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetVertexShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT* pSetting) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetPixelShaderConstantF)(THIS_ UINT StartRegister,float* pConstantData,UINT Vector4fCount) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetPixelShaderConstantI)(THIS_ UINT StartRegister,int* pConstantData,UINT Vector4iCount) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetPixelShaderConstantB)(THIS_ UINT StartRegister,BOOL* pConstantData,UINT BoolCount) HKV_NR_UNIMPLEMENTED
  STDMETHOD(DrawRectPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo) HKV_NR_UNIMPLEMENTED
  STDMETHOD(DrawTriPatch)(THIS_ UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo) HKV_NR_UNIMPLEMENTED
  STDMETHOD(DeletePatch)(THIS_ UINT Handle) HKV_NR_UNIMPLEMENTED   

  // Implemented
  STDMETHOD(BeginScene)(THIS) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(Clear)(THIS_ DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(CreateCubeTexture)(THIS_ UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
  STDMETHOD(CreateDepthStencilSurface)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
  STDMETHOD(CreateIndexBuffer)(THIS_ UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle);
  STDMETHOD(CreateVertexShader)(THIS_ CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
  STDMETHOD(CreateVolumeTexture)(THIS_ UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle);
  STDMETHOD(CreatePixelShader)(THIS_ CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);
  STDMETHOD(CreateQuery)(THIS_ D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery);
  STDMETHOD(CreateRenderTarget)(THIS_ UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
  STDMETHOD(CreateTexture)(THIS_ UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
  STDMETHOD(CreateVertexBuffer)(THIS_ UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
  STDMETHOD(CreateVertexDeclaration)(THIS_ CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl);
  STDMETHOD(DrawIndexedPrimitive)(THIS_ D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(DrawIndexedPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(DrawPrimitive)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(DrawPrimitiveUP)(THIS_ D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(EndScene)(THIS) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(GetDirect3D)(THIS_ IDirect3D9** ppD3D9) HKV_OVERRIDE;
  STDMETHOD(GetDepthStencilSurface)(THIS_ IDirect3DSurface9** ppZStencilSurface) HKV_OVERRIDE;
  STDMETHOD(GetDeviceCaps)(THIS_ D3DCAPS9* pCaps) HKV_OVERRIDE;
  STDMETHOD(GetFVF)(THIS_ DWORD* pFVF) HKV_OVERRIDE { return D3DERR_INVALIDCALL; }
  STDMETHOD_(void, GetGammaRamp)(THIS_ UINT iSwapChain,D3DGAMMARAMP* pRamp) HKV_OVERRIDE { *pRamp = gammaRamp; }
  STDMETHOD(GetIndices)(THIS_ IDirect3DIndexBuffer9** ppIndexData) HKV_OVERRIDE { return D3DERR_INVALIDCALL; }
  STDMETHOD(GetPixelShader)(THIS_ IDirect3DPixelShader9** ppShader) HKV_OVERRIDE { return D3DERR_INVALIDCALL; }
    STDMETHOD(GetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD* pValue) HKV_OVERRIDE { return D3DERR_INVALIDCALL; }
  STDMETHOD(GetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget) HKV_OVERRIDE;
  STDMETHOD(GetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride) HKV_OVERRIDE { return D3DERR_INVALIDCALL; }
  STDMETHOD(GetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9** ppTexture) HKV_OVERRIDE { return D3DERR_INVALIDCALL; }
  STDMETHOD(GetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue) HKV_OVERRIDE { return D3DERR_INVALIDCALL; }
  STDMETHOD(GetVertexShader)(THIS_ IDirect3DVertexShader9** ppShader) HKV_OVERRIDE { return D3DERR_INVALIDCALL; }
  STDMETHOD(Present)(THIS_ CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(Reset)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetDepthStencilSurface)(THIS_ IDirect3DSurface9* pNewZStencil) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetFVF)(THIS_ DWORD FVF) HKV_OVERRIDE { return S_OK; }
  STDMETHOD_(void, SetGammaRamp)(THIS_ UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp) { gammaRamp = *pRamp; }
  STDMETHOD(SetIndices)(THIS_ IDirect3DIndexBuffer9* pIndexData) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetPixelShader)(THIS_ IDirect3DPixelShader9* pShader) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD Value) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetRenderTarget)(THIS_ DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetSamplerState)(THIS_ DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetScissorRect)(THIS_ CONST RECT* pRect) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetStreamSource)(THIS_ UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetStreamSourceFreq)(THIS_ UINT StreamNumber,UINT Setting) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetTexture)(THIS_ DWORD Stage,IDirect3DBaseTexture9* pTexture) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetVertexDeclaration)(THIS_ IDirect3DVertexDeclaration9* pDecl) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetVertexShader)(THIS_ IDirect3DVertexShader9* pShader) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetViewport)(THIS_ CONST D3DVIEWPORT9* pViewport) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(StretchRect)(THIS_ IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter) { return S_OK; }
  STDMETHOD(TestCooperativeLevel)(THIS) HKV_OVERRIDE { return S_OK; }

  private:
    VSmartPtr<Direct3D9Dummy> m_spDirect3D9;
    VSmartPtr<Direct3DSurface9Dummy> m_spDepthStencilSurface;
    VSmartPtr<Direct3DSurface9Dummy> m_spDefaultRenderTarget;
    D3DGAMMARAMP gammaRamp;
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
