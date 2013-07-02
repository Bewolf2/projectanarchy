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

class Direct3DVertexBuffer9Dummy : public IDirect3DVertexBuffer9, public UnknownDummy
{
public:
  Direct3DVertexBuffer9Dummy(int iByteSize) : m_iByteSize(iByteSize), m_pLockedData(NULL), m_iLockCount(0)
  {
  }

  ~Direct3DVertexBuffer9Dummy();

  HKV_NR_IMPLEMENT_IUNKNOWN;

  /*** IDirect3DResource9 methods ***/
  STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice) HKV_NR_UNIMPLEMENTED
    STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags) HKV_NR_UNIMPLEMENTED
    STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData) HKV_NR_UNIMPLEMENTED
    STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid) HKV_NR_UNIMPLEMENTED
    STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew) HKV_NR_UNIMPLEMENTED
    STDMETHOD_(DWORD, GetPriority)(THIS) HKV_NR_UNIMPLEMENTED
    STDMETHOD_(void, PreLoad)(THIS) {}
  STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS) {return D3DRESOURCETYPE();}
  STDMETHOD(GetDesc)(THIS_ D3DVERTEXBUFFER_DESC *pDesc) HKV_NR_UNIMPLEMENTED;

  // Implemented:
  STDMETHOD(Lock)(THIS_ UINT OffsetToLock,UINT SizeToLock,void** ppbData,DWORD Flags) HKV_OVERRIDE;
  STDMETHOD(Unlock)(THIS) HKV_OVERRIDE;

private:
  Direct3DVertexBuffer9Dummy();

  char* m_pLockedData;
  int m_iLockCount;
  int m_iByteSize;
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
