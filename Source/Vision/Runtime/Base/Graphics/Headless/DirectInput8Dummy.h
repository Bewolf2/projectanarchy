/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <dinput.h>

#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

class DirectInput8Dummy : public IDirectInput8, public UnknownDummy
{
public:
   virtual ULONG STDMETHODCALLTYPE AddRef( void) HKV_OVERRIDE { return UnknownDummy::AddRef(); }
   virtual ULONG STDMETHODCALLTYPE Release( void) HKV_OVERRIDE { return UnknownDummy::Release(); }

   /*** IDirectInput8 methods ***/
   STDMETHOD(GetDeviceStatus)(THIS_ REFGUID) HKV_NR_UNIMPLEMENTED;
   STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) HKV_NR_UNIMPLEMENTED;
   STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD) HKV_NR_UNIMPLEMENTED;
   STDMETHOD(FindDevice)(THIS_ REFGUID,LPCSTR,LPGUID) HKV_NR_UNIMPLEMENTED;
   STDMETHOD(EnumDevicesBySemantics)(THIS_ LPCSTR,LPDIACTIONFORMAT,LPDIENUMDEVICESBYSEMANTICSCBA,LPVOID,DWORD) HKV_NR_UNIMPLEMENTED;
   STDMETHOD(ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK,LPDICONFIGUREDEVICESPARAMS,DWORD,LPVOID) HKV_NR_UNIMPLEMENTED;

  // Implemented
  STDMETHOD(EnumDevices)(THIS_ DWORD,LPDIENUMDEVICESCALLBACK,LPVOID,DWORD) HKV_OVERRIDE;
  STDMETHOD(CreateDevice)(THIS_ REFGUID,LPDIRECTINPUTDEVICE8 *,LPUNKNOWN) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) HKV_OVERRIDE;
};

HRESULT WINAPI DirectInput8CreateDummy(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
DWORD WINAPI XInputSetStateDummy(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
DWORD WINAPI XInputGetStateDummy(DWORD         dwUserIndex, XINPUT_STATE* pState);

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
