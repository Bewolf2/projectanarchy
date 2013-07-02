/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPLATFORMDEFINESDX9_H
#define HKVPLATFORMDEFINESDX9_H

  // Note that there are additional defines set in here
  #include "hkvPlatformWin32.h"

  #if defined (_WIN64)
    #define VERSION_TYPE "WIN64 DIRECTX9"
  #else
    #define VERSION_TYPE "WIN32 DIRECTX9"
  #endif

  #define MAX_NUM_RENDERTARGETS     4
  #define MAX_TEXTURE_UNIT_COUNT    16
  #define MAX_VS_TEXTURE_UNIT_COUNT 4
  #define USE_MESHBUFFER_DESC_VERTEX_SETUP
  #define SUPPORTS_USERPOINTER_RENDERING
  #define SUPPORTS_VS_SAMPLERS
  #define SUPPORTS_HARDWARESPANNING
  #define SUPPORTS_ALPHATEST_BLENDSTATE
  #define VERTEX_RGBA8_ATTRIB_SWIZZLE_MODE SWIZZLE_VERTEX_RGBA8_TO_RGBA8
  #define SUPPORTS_CLOUD_LAYER

  struct IDirect3DSurface9;
  struct IDirect3DIndexBuffer9;
  #define Direct3D     IDirect3D9
  #define D3DDevice    IDirect3DDevice9
  #define D3DBaseTexture IDirect3DBaseTexture9
  #define D3DTexture IDirect3DTexture9
  #define D3DVertexShader IDirect3DVertexShader9
  #define D3DPixelShader IDirect3DPixelShader9
  #define D3DSurface IDirect3DSurface9
  #define D3DCubeTexture IDirect3DCubeTexture9
  #define D3DVertexBuffer IDirect3DVertexBuffer9
  #define D3DIndexBuffer IDirect3DIndexBuffer9

  #define V_DEVICE_MAX_TEXTURE_SIZE 4096


#endif // HKVPLATFORMDEFINESDX9_H

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
