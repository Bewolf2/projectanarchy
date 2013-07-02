/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <Unknwn.h>

#include <Vision/Runtime/Base/System/VRefCounter.hpp>

#define HKV_NR_IMPLEMENT_IUNKNOWN \
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID,__RPC__deref_out void __RPC_FAR *__RPC_FAR *) HKV_OVERRIDE { VASSERT_ALWAYS_MSG(false, "Unimplemented null renderer function called"); return E_NOTIMPL; } \
  virtual ULONG STDMETHODCALLTYPE AddRef( void) HKV_OVERRIDE { return UnknownDummy::AddRef(); } \
  virtual ULONG STDMETHODCALLTYPE Release( void) HKV_OVERRIDE { return UnknownDummy::Release(); }

#define HKV_NR_UNIMPLEMENTED HKV_OVERRIDE { VASSERT_ALWAYS_MSG(false, "Unimplemented null renderer function called"); return E_NOTIMPL; }
#define HKV_NR_UNIMPLEMENTED_BOOL HKV_OVERRIDE { VASSERT_ALWAYS_MSG(false, "Unimplemented null renderer function called"); return FALSE; }
#define HKV_NR_UNIMPLEMENTED_VOID HKV_OVERRIDE { VASSERT_ALWAYS_MSG(false, "Unimplemented null renderer function called"); }
#define HKV_NR_UNIMPLEMENTED_FLOAT HKV_OVERRIDE { VASSERT_ALWAYS_MSG(false, "Unimplemented null renderer function called"); return 0.0f; }

typedef VRefCounter UnknownDummy;

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
