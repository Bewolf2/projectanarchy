/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <dxgi.h>

#include <Vision/Runtime/Base/Graphics/Headless/UnknownDummy.h>

class DXGIOutputDummy : public IDXGIOutput, public UnknownDummy
{
public:
  HKV_NR_IMPLEMENT_IUNKNOWN;

  virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
    /* [annotation][in] */ 
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

  virtual HRESULT STDMETHODCALLTYPE FindClosestMatchingMode( 
    /* [annotation][in] */ 
    __in  const DXGI_MODE_DESC *pModeToMatch,
    /* [annotation][out] */ 
    __out  DXGI_MODE_DESC *pClosestMatch,
    /* [annotation][in] */ 
    __in_opt  IUnknown *pConcernedDevice) HKV_NR_UNIMPLEMENTED

  virtual HRESULT STDMETHODCALLTYPE WaitForVBlank( void) HKV_NR_UNIMPLEMENTED

  virtual HRESULT STDMETHODCALLTYPE TakeOwnership( 
    /* [annotation][in] */ 
    __in  IUnknown *pDevice,
    BOOL Exclusive) HKV_NR_UNIMPLEMENTED

  virtual void STDMETHODCALLTYPE ReleaseOwnership( void) HKV_NR_UNIMPLEMENTED_VOID

  virtual HRESULT STDMETHODCALLTYPE GetGammaControlCapabilities( 
    /* [annotation][out] */ 
    __out  DXGI_GAMMA_CONTROL_CAPABILITIES *pGammaCaps) HKV_NR_UNIMPLEMENTED

  virtual HRESULT STDMETHODCALLTYPE SetGammaControl( 
    /* [annotation][in] */ 
    __in  const DXGI_GAMMA_CONTROL *pArray) HKV_NR_UNIMPLEMENTED

  virtual HRESULT STDMETHODCALLTYPE GetGammaControl( 
    /* [annotation][out] */ 
    __out  DXGI_GAMMA_CONTROL *pArray) HKV_NR_UNIMPLEMENTED

  virtual HRESULT STDMETHODCALLTYPE SetDisplaySurface( 
    /* [annotation][in] */ 
    __in  IDXGISurface *pScanoutSurface) HKV_NR_UNIMPLEMENTED

  virtual HRESULT STDMETHODCALLTYPE GetDisplaySurfaceData( 
    /* [annotation][in] */ 
    __in  IDXGISurface *pDestination) HKV_NR_UNIMPLEMENTED

  virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics( 
    /* [annotation][out] */ 
    __out  DXGI_FRAME_STATISTICS *pStats) HKV_NR_UNIMPLEMENTED

    // Implemented
    virtual HRESULT STDMETHODCALLTYPE GetDesc(__out  DXGI_OUTPUT_DESC *pDesc);
  virtual HRESULT STDMETHODCALLTYPE GetDisplayModeList(DXGI_FORMAT EnumFormat, UINT Flags, UINT *pNumModes, DXGI_MODE_DESC *pDesc);
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
