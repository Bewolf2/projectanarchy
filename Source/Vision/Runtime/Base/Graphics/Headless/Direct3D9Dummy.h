/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <d3d9.h>

#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

class Direct3D9Dummy : public IDirect3D9, public UnknownDummy
{
public:
  HKV_NR_IMPLEMENT_IUNKNOWN;

  /*** IDirect3D9 methods ***/
  STDMETHOD(RegisterSoftwareDevice)(THIS_ void* pInitializeFunction) HKV_NR_UNIMPLEMENTED
  STDMETHOD_(UINT, GetAdapterCount)(THIS) HKV_NR_UNIMPLEMENTED
  STDMETHOD_(UINT, GetAdapterModeCount)(THIS_ UINT Adapter,D3DFORMAT Format) HKV_NR_UNIMPLEMENTED
  STDMETHOD(EnumAdapterModes)(THIS_ UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetAdapterDisplayMode)(THIS_ UINT Adapter,D3DDISPLAYMODE* pMode) HKV_NR_UNIMPLEMENTED
  STDMETHOD(CheckDeviceType)(THIS_ UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed) HKV_NR_UNIMPLEMENTED
  STDMETHOD(CheckDeviceMultiSampleType)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels) HKV_NR_UNIMPLEMENTED
  STDMETHOD(CheckDepthStencilMatch)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat) HKV_NR_UNIMPLEMENTED
  STDMETHOD(CheckDeviceFormatConversion)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetDeviceCaps)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps) HKV_NR_UNIMPLEMENTED
  STDMETHOD_(HMONITOR, GetAdapterMonitor)(THIS_ UINT Adapter) HKV_OVERRIDE {VASSERT_MSG(false, "Unimplemented null renderer function called"); return HMONITOR();}

  // Implemented:
  STDMETHOD(CheckDeviceFormat)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(CreateDevice)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface) HKV_OVERRIDE;
  STDMETHOD(GetAdapterIdentifier)(THIS_ UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier) HKV_OVERRIDE;
};

IDirect3D9   * WINAPI Direct3DCreate9Dummy(UINT SDKVersion);
int WINAPI D3DPERF_BeginEventDummy( D3DCOLOR col, LPCWSTR wszName );
int WINAPI D3DPERF_EndEventDummy( void );
void WINAPI D3DPERF_SetMarkerDummy( D3DCOLOR col, LPCWSTR wszName );

HRESULT WINAPI D3DReflectDummy(LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void **ppReflector);

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
