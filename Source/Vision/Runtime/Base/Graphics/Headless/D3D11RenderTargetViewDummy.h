/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

#include <D3D11.h>

#include <Vision/Runtime/Base/Graphics/Headless/D3D11DeviceChildDummy.h>
#include <Vision/Runtime/Base/VBase.hpp>

class D3D11RenderTargetViewDummy : public ID3D11RenderTargetView, public UnknownDummy
{
public:
  D3D11RenderTargetViewDummy(ID3D11Resource* pResource) : m_pResource(pResource) {}

  HKV_NR_IMPLEMENT_IUNKNOWN;
  HKV_NR_IMPLEMENT_ID3D11DEVICECHILD;

    virtual void STDMETHODCALLTYPE GetDesc( 
    /* [annotation] */ 
    __out  D3D11_RENDER_TARGET_VIEW_DESC *pDesc) HKV_NR_UNIMPLEMENTED_VOID

    // Implemented:
    virtual void STDMETHODCALLTYPE GetResource(ID3D11Resource **ppResource) HKV_OVERRIDE;

private:
  D3D11RenderTargetViewDummy();

  VSmartPtr<ID3D11Resource> m_pResource;
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
