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

class DirectInputDevice8Dummy : public IDirectInputDevice8, public UnknownDummy
{
public:
  HKV_NR_IMPLEMENT_IUNKNOWN;

  /*** IDirectInputDevice8 methods ***/
  STDMETHOD(GetCapabilities)(THIS_ LPDIDEVCAPS) HKV_NR_UNIMPLEMENTED
  STDMETHOD(EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACK,LPVOID,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetProperty)(THIS_ REFGUID,LPDIPROPHEADER) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetDeviceState)(THIS_ DWORD,LPVOID) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCE,DWORD,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCE) HKV_NR_UNIMPLEMENTED
  STDMETHOD(RunControlPanel)(THIS_ HWND,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(Initialize)(THIS_ HINSTANCE,DWORD,REFGUID) HKV_NR_UNIMPLEMENTED
  STDMETHOD(CreateEffect)(THIS_ REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN) HKV_NR_UNIMPLEMENTED
  STDMETHOD(EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACK,LPVOID,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetEffectInfo)(THIS_ LPDIEFFECTINFO,REFGUID) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetForceFeedbackState)(THIS_ LPDWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SendForceFeedbackCommand)(THIS_ DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(EnumCreatedEffectObjects)(THIS_ LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(Escape)(THIS_ LPDIEFFESCAPE) HKV_NR_UNIMPLEMENTED
  STDMETHOD(Poll)(THIS) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SendDeviceData)(THIS_ DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(EnumEffectsInFile)(THIS_ LPCSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(WriteEffectToFile)(THIS_ LPCSTR,DWORD,LPDIFILEEFFECT,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(BuildActionMap)(THIS_ LPDIACTIONFORMAT,LPCSTR,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(SetActionMap)(THIS_ LPDIACTIONFORMAT,LPCSTR,DWORD) HKV_NR_UNIMPLEMENTED
  STDMETHOD(GetImageInfo)(THIS_ LPDIDEVICEIMAGEINFOHEADER) HKV_NR_UNIMPLEMENTED

  // Implemented
  STDMETHOD(Acquire)(THIS) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(GetDeviceData)(THIS_ DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD);
  STDMETHOD(SetCooperativeLevel)(THIS_ HWND,DWORD) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetDataFormat)(THIS_ LPCDIDATAFORMAT) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetEventNotification)(THIS_ HANDLE) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(SetProperty)(THIS_ REFGUID,LPCDIPROPHEADER) HKV_OVERRIDE { return S_OK; }
  STDMETHOD(Unacquire)(THIS) HKV_OVERRIDE { return S_OK; }
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
