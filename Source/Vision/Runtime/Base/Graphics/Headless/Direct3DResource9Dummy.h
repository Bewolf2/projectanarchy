/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#define HKV_NR_IMPLEMENT_IDIRECT3DRESOURCE9 \
  STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) HKV_NR_UNIMPLEMENTED \
  STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) HKV_NR_UNIMPLEMENTED \
  STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) HKV_NR_UNIMPLEMENTED \
  STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) HKV_NR_UNIMPLEMENTED \
  STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) HKV_NR_UNIMPLEMENTED \
  STDMETHOD_(DWORD, GetPriority)(THIS) HKV_NR_UNIMPLEMENTED \
  STDMETHOD_(void, PreLoad)(THIS) HKV_NR_UNIMPLEMENTED_VOID \
  STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) HKV_OVERRIDE {VASSERT_MSG(false, "Unimplemented null renderer function called"); return D3DRESOURCETYPE();} \
  STDMETHOD(GetContainer)(THIS_ REFIID riid,void** ppContainer) HKV_NR_UNIMPLEMENTED \
  STDMETHOD(GetDesc)(THIS_ D3DSURFACE_DESC *pDesc) HKV_NR_UNIMPLEMENTED \
  STDMETHOD(LockRect)(THIS_ D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags) HKV_NR_UNIMPLEMENTED \
  STDMETHOD(UnlockRect)(THIS) HKV_NR_UNIMPLEMENTED \
  STDMETHOD(GetDC)(THIS_ HDC *phdc) HKV_NR_UNIMPLEMENTED \
  STDMETHOD(ReleaseDC)(THIS_ HDC hdc) HKV_NR_UNIMPLEMENTED

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
