/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

#define HKV_NR_IMPLEMENT_ID3D11DEVICECHILD \
  virtual void STDMETHODCALLTYPE GetDevice(__out ID3D11Device**) HKV_NR_UNIMPLEMENTED_VOID \
  virtual HRESULT STDMETHODCALLTYPE GetPrivateData(__in REFGUID, __inout UINT* pDataSize, __out_bcount_opt( *pDataSize ) void*) HKV_NR_UNIMPLEMENTED \
  virtual HRESULT STDMETHODCALLTYPE SetPrivateData(__in  REFGUID, __in  UINT DataSize, __in_bcount_opt( DataSize ) const void*) HKV_OVERRIDE { return S_OK; } \
  virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(__in REFGUID, __in_opt const IUnknown*) HKV_NR_UNIMPLEMENTED

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
