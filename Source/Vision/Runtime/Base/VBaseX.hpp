/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBaseX.hpp

#ifndef DC_VBASEX_HPP
#define DC_VBASEX_HPP

/*
  Header for including vBase + system specific classes (video + shaders)
  We split vBase like this so all the samples and tools using
  vBase don't need the system classes (e.g. DirectX SDK) installed.
*/
#if defined(_VISION_PSP2)
#include <math.h>
#endif

#include <Vision/Runtime/Base/VBase.hpp>

extern bool g_bVBaseDeinitializing;

#if defined(WIN32) || defined(_VISION_WINRT)

  #ifdef _VR_DX9
    #include <d3d9.h>
    #include <d3d9types.h>
    #include <d3dx9.h>
    #include <dxerr.h>
//    #include <ddraw.h>
  #elif defined _VR_DX11
    #if _VR_DX11_SUPPORTS_D3DX
      #include <d3d11.h>
      #include <d3d11shader.h>
      #include <d3d11sdklayers.h>
      #include <d3dx11.h>
      #include <d3dx11async.h>
      #include <d3dx11core.h>
      #include <d3dx11tex.h>
      #include <d3dx10math.h>

    #else // Newer DX sdks (Win8 etc)
      #include <d3d11_1.h>
    #endif
    #ifndef _VISION_APOLLO
      #include <D3Dcompiler.h>
    #endif
  #endif
  
#endif

#if defined(WIN32) || defined(_VISION_WINRT)

  #ifdef _VR_DX9
      
    #define VVIDEO_DEVTYPE D3DDEVTYPE_HAL

    #if defined(HK_ANARCHY)
      // See HS#10365
      #define VVIDEO_BEHAVIORFLAGS D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE
    #else
      #define VVIDEO_BEHAVIORFLAGS D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE
    #endif

    #define VVIDEO_PRESENTPARAMFLAGS D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL
    #define VVIDEO_DEFAULTMODE32 D3DFMT_A8R8G8B8
    #define VVIDEO_DEFAULTMODE16 D3DFMT_R5G6B5
    
    const D3DMULTISAMPLE_TYPE g_MultiSampleTypes[5]={D3DMULTISAMPLE_NONE, D3DMULTISAMPLE_2_SAMPLES, D3DMULTISAMPLE_4_SAMPLES, D3DMULTISAMPLE_8_SAMPLES, D3DMULTISAMPLE_16_SAMPLES};
    const unsigned int g_PresentationInterval[2]={D3DPRESENT_INTERVAL_IMMEDIATE, D3DPRESENT_INTERVAL_ONE};
  #elif defined(_VR_DX10) || defined(_VR_DX11)
    //#define VVIDEO_DEVTYPE D3DDEVTYPE_HAL
    //#define VVIDEO_BEHAVIORFLAGS D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE
    #define VVIDEO_PRESENTPARAMFLAGS 0
    #ifdef _VISION_WINRT
      #define VVIDEO_DEFAULTMODE32 DXGI_FORMAT_B8G8R8A8_UNORM   // If you want D2D support on top of the window, then BGRA is the native format
    #else
      #define VVIDEO_DEFAULTMODE32 DXGI_FORMAT_R8G8B8A8_UNORM
    #endif
    
    #define VVIDEO_DEFAULTMODE16 DXGI_FORMAT_B5G6R5_UNORM
    const unsigned int g_MultiSampleTypes[5]={1,2,4,8,16};
    //const unsigned int g_PresentationInterval[2]={0, 1};
  #endif
  

#elif defined (_VISION_XENON)
  // Xenon default is 720p HDTV
  #define VVIDEO_DEVTYPE D3DDEVTYPE_HAL
  #define VVIDEO_BEHAVIORFLAGS D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_BUFFER_2_FRAMES/*|D3DCREATE_RELAXEDRAMLIMIT*/
  #define VVIDEO_PRESENTPARAMFLAGS 0
  #define VVIDEO_DEFAULTMODE32 D3DFMT_A8R8G8B8
  #define VVIDEO_DEFAULTMODE16 D3DFMT_R5G6B5
  #define HWND void*
  const D3DMULTISAMPLE_TYPE g_MultiSampleTypes[5]={D3DMULTISAMPLE_NONE, D3DMULTISAMPLE_2_SAMPLES, D3DMULTISAMPLE_4_SAMPLES, D3DMULTISAMPLE_4_SAMPLES, D3DMULTISAMPLE_4_SAMPLES};
  const unsigned int g_PresentationInterval[2]={D3DPRESENT_INTERVAL_IMMEDIATE, D3DPRESENT_INTERVAL_ONE};



#elif defined (_VISION_POSIX)
  // @@@L more to come...
  #define HWND void*

#elif defined(_VISION_PSP2)
  #include <assert.h>

#endif

#ifdef _VISION_IOS

  // Import the OpenGL ES2 headers of the iOS SDK
  #import <OpenGLES/ES2/gl.h>
  #import <OpenGLES/ES2/glext.h>

#endif

#ifdef _VISION_ANDROID

  #include <GLES2/gl2.h>
  #include <GLES2/gl2ext.h>

#endif

  
#include <Vision/Runtime/Base/Graphics/Video/VVideoCaps.hpp>

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludeManager.hpp>

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
