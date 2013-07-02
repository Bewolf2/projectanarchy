/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef DXWRAPPERS_H_INCLUDED
#define DXWRAPPERS_H_INCLUDED

#if defined(WIN32) && !defined(_VISION_WINRT)
#ifdef _VISION_DIRECTX_2010
    #pragma warning(push)
    #pragma warning(disable: 4005)
#endif
#include <d3d11.h>
#include <dinput.h>
#include <dxgi.h>
#ifdef _VISION_DIRECTX_2010
    #pragma warning(pop)
#endif
LONG WINAPI WrapChangeDisplaySettingsEx(LPCTSTR lpszDeviceName, DEVMODE *lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam);
HRESULT WINAPI WrapCreateDXGIFactory(REFIID riid, void **ppFactory);
HRESULT WINAPI WrapD3D11CreateDevice(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, CONST D3D_FEATURE_LEVEL* pFeatureLevels,
                                     UINT FeatureLevels, UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext );
int VBASE_IMPEXP WINAPI WrapD3DPERF_BeginEvent( D3DCOLOR col, LPCWSTR wszName );
int VBASE_IMPEXP WINAPI WrapD3DPERF_EndEvent( void );
void VBASE_IMPEXP WINAPI WrapD3DPERF_SetMarker( D3DCOLOR col, LPCWSTR wszName );
HRESULT VBASE_IMPEXP WINAPI WrapD3DReflect(LPCVOID pSrcData, SIZE_T SrcDataSize, REFIID pInterface, void** ppReflector);
HRESULT WINAPI WrapDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
BOOL WINAPI WrapEnumDisplayDevices(LPCTSTR lpDevice, DWORD iDevNum, PDISPLAY_DEVICE lpDisplayDevice, DWORD dwFlags);
BOOL WINAPI WrapEnumDisplayMonitors(HDC hdc, LPCRECT lprcClip, MONITORENUMPROC lpfnEnum, LPARAM dwData);
BOOL WINAPI WrapEnumDisplaySettings(LPCTSTR lpszDeviceName, DWORD iModeNum, DEVMODE *lpDevMode);
DWORD WINAPI WrapXInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
DWORD WINAPI WrapXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);

#else

#define WrapXInputSetState XInputSetState
#define WrapXInputGetState XInputGetState

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
