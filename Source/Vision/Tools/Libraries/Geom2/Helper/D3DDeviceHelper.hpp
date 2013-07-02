/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


#if !defined(_VISION_DOC)

// Helper providing a pointer to an existing, or newly created D3D device (multiple instances
// can be used at the same time, but the class itself is not thread-safe).
class D3DDeviceHelper
{
public:
#if defined(_VR_DX9)
  VGEOM2_IMPEXP_CLASS D3DDeviceHelper(IDirect3DDevice9*(*customD3DDeviceGetter)() = &VVideo::GetD3DDevice);  // Reuse (d3d9) device, if a vBase one is already in use - only for DX9 as vGeom2 depends specifically on D3DX9 functions.
#else
  VGEOM2_IMPEXP_CLASS D3DDeviceHelper(IDirect3DDevice9*(*customD3DDeviceGetter)() = NULL);
#endif
  VGEOM2_IMPEXP_CLASS ~D3DDeviceHelper();

  VGEOM2_IMPEXP_CLASS IDirect3DDevice9* GetD3DDevice() const throw();

private:
  D3DDeviceHelper(const D3DDeviceHelper&);  // Disallow copies.
  IDirect3DDevice9*   m_d3dDevice;
  IDirect3DDevice9Ex* m_d3dDeviceEx;
  HWND m_hWnd;  // Optionally used if device needs a window.
  IDirect3DDevice9*(*m_customD3DDeviceGetter)();
};

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
