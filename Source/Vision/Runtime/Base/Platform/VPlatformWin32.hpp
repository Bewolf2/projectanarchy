/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPlatformWin32.hpp

#ifndef VPLATFORM_WIN32_INCLUDED
#define VPLATFORM_WIN32_INCLUDED

////////////////////////////////////////////////////////////////
//  Defines affecting the platform-specific headers
////////////////////////////////////////////////////////////////

#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0501)
  #undef  _WIN32_WINNT
  #define _WIN32_WINNT 0x0501
#endif

#ifdef HK_DEBUG_SLOW
  #define D3D_DEBUG_INFO
#endif

#if !defined(_WIN32_WINDOWS) || (_WIN32_WINDOWS < 0x0500)
  #undef  _WIN32_WINDOWS
  #define _WIN32_WINDOWS 0x0500
#endif



////////////////////////////////////////////////////////////////
//  Platform-specific headers
////////////////////////////////////////////////////////////////

#include <windows.h>

#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/utime.h>

#include <mmsystem.h>
#include <process.h>
#include <shlwapi.h>
#include <winuser.h>

////////////////////////////////////////////////////////////////
//  DirectX headers
////////////////////////////////////////////////////////////////

#if defined(_VR_DX11) || ( (_MSC_VER >= 1700) && !defined(_VR_DX9) && !defined(_VISION_DIRECTX_2010) ) 

  #ifdef _VISION_DIRECTX_2010
     #pragma warning(push)
     #pragma warning(disable: 4005)
  #endif
  #include <dxgi.h>
  #include <initguid.h>
  #include <d3d11.h>
  #include <d3d11shader.h>
  #include <d3d11sdklayers.h>
  #if (_MSC_VER < 1700) || defined(_VISION_DIRECTX_2010)  // VS2012 native Win8 DX sdk has no d3dx etc. We support both builds.
    #include <d3dx11.h>
    #define _VR_DX11_SUPPORTS_D3DX 1
  #else
    #define _VR_DX11_SUPPORTS_D3DX 0
  #endif

  #include <d3d9.h>  //Needed for D3DPERF_xxx (http://discussms.hosting.lsoft.com/SCRIPTS/WA-MSD.EXE?A2=ind0708C&L=directxdev&P=2016)

  #ifdef _VISION_DIRECTX_2010
    #pragma warning(pop)
  #endif 

  #define DDPF_ALPHAPIXELS      0x00000001l
  #define DDPF_ALPHA            0x00000002l
  #define DDPF_RGB              0x00000040l
  #define DDPF_PALETTEINDEXED8  0x00000020l
  #define DDPF_FOURCC           0x00000004l
  #define DDPF_LUMINANCE        0x00020000l

#else
  #include <D3Dcommon.h>
  #include <d3d9.h>
  #include <d3d9types.h>
  #if (_MSC_VER < 1700) || defined(_VISION_DIRECTX_2010)
    #include <dxerr.h>
    #include <d3dx9.h>
    #define _VR_DX9_SUPPORTS_D3DX 1
  #endif
#endif

////////////////////////////////////////////////////////////////
//  DirectInput headers
////////////////////////////////////////////////////////////////

#ifndef DIRECTINPUT_VERSION
  #define DIRECTINPUT_VERSION 0x0800
#endif

//This requires the latest DirectX SDK to be in installed
#include <dinput.h>
#include <dinputd.h>
#include <XInput.h>
  
  
////////////////////////////////////////////////////////////////
//  Misc platform headers
////////////////////////////////////////////////////////////////
  
#if 1
  #include <wbemidl.h>  //default platform SDK
#else
  #include <wbemcli.h>  //latest platform SDK
#endif

#include <gl/gl.h>
#include <winver.h>
  

////////////////////////////////////////////////////////////////
//  Defines for cross-platform interoperability
////////////////////////////////////////////////////////////////

typedef __int64 int64;
typedef unsigned __int64 uint64;


const int FS_MAX_FILE  = 256;
const int FS_MAX_PATH  = 4096;
const int FS_MAX_DRIVE = 256;

#define VFILE_CHAR_SEPARATOR '\\'
#define VFILE_STR_SEPARATOR "\\"

#define FS_STRCMP _stricmp
#define FS_STRNCMP _strnicmp


#define vsnprintf _vsnprintf
#define vsnwprintf _vsnwprintf

#define DECLSPEC_DLLEXPORT __declspec(dllexport)



#define VISION_LIKELY(COND) (COND)
#define VISION_UNLIKELY(COND) (COND)

inline void MicroSleep(unsigned int iMicroSeconds) { /* TODO */ }

#endif //VPLATFORM_WIN32_INCLUDED

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
