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

#include <DXGI.h>

#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

class DXGISwapChainDummy : public IDXGISwapChain, public UnknownDummy
{
public:
  HKV_NR_IMPLEMENT_IUNKNOWN;

  virtual HRESULT STDMETHODCALLTYPE SetPrivateData(
    __in  REFGUID Name,
    /* [in] */ UINT DataSize,
    /* [annotation][in] */ 
    __in_bcount(DataSize)  const void *pData) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
    /* [annotation][in] */ 
    __in  REFGUID Name,
    /* [annotation][in] */ 
    __in  const IUnknown *pUnknown) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
    /* [annotation][in] */ 
    __in  REFGUID Name,
    /* [annotation][out][in] */ 
    __inout  UINT *pDataSize,
    /* [annotation][out] */ 
    __out_bcount(*pDataSize)  void *pData) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetParent( 
    /* [annotation][in] */ 
    __in  REFIID riid,
    /* [annotation][retval][out] */ 
    __out  void **ppParent) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetDevice( 
    /* [annotation][in] */ 
    __in  REFIID riid,
    /* [annotation][retval][out] */ 
    __out  void **ppDevice) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetFullscreenState( 
    /* [annotation][out] */ 
    __out  BOOL *pFullscreen,
    /* [annotation][out] */ 
    __out  IDXGIOutput **ppTarget) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetDesc( 
    /* [annotation][out] */ 
    __out  DXGI_SWAP_CHAIN_DESC *pDesc) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE ResizeTarget( 
    /* [annotation][in] */ 
    __in  const DXGI_MODE_DESC *pNewTargetParameters) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetContainingOutput( 
    /* [annotation][out] */ 
    __out  IDXGIOutput **ppOutput) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics( 
    /* [annotation][out] */ 
    __out  DXGI_FRAME_STATISTICS *pStats) HKV_NR_UNIMPLEMENTED

    virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount( 
    /* [annotation][out] */ 
    __out  UINT *pLastPresentCount) HKV_NR_UNIMPLEMENTED

    // Implemented
    virtual HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, REFIID riid, void **ppSurface) HKV_OVERRIDE;
  virtual HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags) HKV_OVERRIDE { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) HKV_OVERRIDE { return S_OK; }
  virtual HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL Fullscreen, IDXGIOutput *pTarget) HKV_OVERRIDE { return S_OK; }
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
