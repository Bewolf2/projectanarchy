/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPLATFORMDEFINES_H
#define HKVPLATFORMDEFINES_H

/*

================== FEATURE PREPROCESSOR DEFINITIONS ==================

NOTE: WHEN INSERTING NEW FEATURE DEFINITIONS, DO NOT FORGET TO ADD THEM TO THIS LIST!

_VR_DX9
_VR_GCM
_VR_GLES2
_VR_GXM
MAX_NUM_RENDERTARGETS
MAX_TEXTURE_UNIT_COUNT
MAX_UAV_COUNT
PLATFORM_HOME_DIR
PPCCPU
PROFILING_VIEW_OVER_PAD_0
RESOURCE_MANAGER_DEFAULT_MEMORY_TEXTURES
RESTRICT
SCREEN_MESSAGE_DEFAULT_SCALING
SUPPORTS_BORDERCOLOR
SUPPORTS_BROWSER
SUPPORTS_CS_SAMPLERS
SUPPORTS_DS_SAMPLERS
SUPPORTS_FLOATINGPOINT_BLENDING
SUPPORTS_GS_SAMPLERS
SUPPORTS_HARDWARE_SKINNING
SUPPORTS_HARDWARESPANNING
SUPPORTS_HS_SAMPLERS
SUPPORTS_JPG
SUPPORTS_KEYBOARD
SUPPORTS_MANUAL_HIZSTENCIL_SETUP
SUPPORTS_MEMEXPORT_SKINNING
SUPPORTS_MODULATEXN_LIGHTMAPPING
SUPPORTS_MOUSE
SUPPORTS_OCCLUSION_QUERY
SUPPORTS_DETAILED_OCCLUSION_QUERY
SUPPORTS_PARTIAL_BUFFER_LOCKS
SUPPORTS_PNG
SUPPORTS_PS_SAMPLERS
SUPPORTS_QUADRENDERING
SUPPORTS_RELATIVE_PATHS
SUPPORTS_RENDERABLE_CUBEMAPS
SUPPORTS_REPORTDIALOG
SUPPORTS_REPORTGROUPS
SUPPORTS_SKINNED_TANGENTS
SUPPORTS_SHADOW_MAPS
SUPPORTS_USERPOINTER_RENDERING
SUPPORTS_VLOGWIN
SUPPORTS_VS_SAMPLERS
SUPPORTS_SHADER_CONSTANT_BUFFERS
SUPPORTS_TERRAIN
SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
SUPPORTS_GPU_RESOURCE_VIEWS
SUPPORTS_LOCKABLE_MEMORY                  // uses lockable memory to safely manage GPU memory
SUPPORTS_GPU_DATA_UPLOAD                  // provides methods for uploading data into GPU memory (without using lock, memcpy, unlock)
USE_MESHBUFFER_DESC_VERTEX_SETUP
CLIPSPACE_DEPTH_IS_MINUSONE_TO_ONE        // set this on all OpenGL platforms where the depth range of clip-space is from -1 to 1 instead of 0 to 1, as on DirectX platforms
TRANSPOSE_MATRICES_ON_UPLOAD_TO_SHADER    // set this on platforms that use GLSL, which requires to upload matrices transposed to shaders
VERSION_TYPE "ANDROID"
X86CPU
VERTEX_RGBA8_ATTRIB_SWIZZLE_MODE (defined to one of the following:)
  SWIZZLE_VERTEX_RGBA8_TO_RGBA8
  SWIZZLE_VERTEX_RGBA8_TO_BGRA8
  SWIZZLE_VERTEX_RGBA8_TO_ABGR8

NEEDS_MRT_AWARE_IM_SHADERS                // For platforms that need special shaders versions of the IM shaders for different MRT counts

CELL_OS_FILE_FUNCTIONS
NATIVE_PATH_SEPARATOR_FWDSLASH
NEEDS_SCENE_STREAMING
NEEDS_SCENE_PREWARMING
SUPPORTS_8_AND_24BPP_MODES
SUPPORTS_ADVANCED_FILEOPS 
V_SUPPORTS_EARLY_STATICS    ///< Platforms on which we have control over creating static objects before all others
SUPPORTS_GETFREEMEM
SUPPORTS_GXT
SUPPORTS_LIBJPG
SUPPORTS_LIBPNG
SUPPORTS_LIT_FILE_LOADING
SUPPORTS_LOCALES
SUPPORTS_MULTIPLE_DEVICES
SUPPORTS_MULTITHREADING
SUPPORTS_MULTITOUCH
SUPPORTS_ORTHOGONAL_LIGHTGRID
SUPPORTS_PVR
SUPPORTS_RELATIVE_DIRECTORIES
SUPPORTS_RESOLUTION_CHANGE
SUPPORTS_RESOURCE_TIMESTAMP
SUPPORTS_RESOURCEVIEWER
SUPPORTS_SNAPSHOT_CREATION
SUPPORTS_TPL              ///< Nintendo
SUPPORTS_VTHREAD
SUPPORTS_WINDOWED_MODES
SUPPORTS_ALPHATEST_BLENDSTATE ///< as opposed to a shader instruction for alpha testing (e.g. clip() in dx11)
V_DEVICE_MAX_TEXTURE_SIZE 1024
V_IS_DIRECTX_PLATFORM
V_IS_WINDOWS_PLATFORM
VSTREAMPROCESS_RUN_IN_SPURS
VSTREAMPROCESS_RUN_IN_THREADMANAGER
SUPPORTS_VTARGET
SUPPORTS_2D_TEXTURE_ARRAYS
SUPPORTS_GEOMETRY_SHADER
SUPPORTS_COMPUTE_SHADER
SUPPORTS_TESSELATION_SHADER
SUPPORTS_SOFTKEYBOARD
V_HAS_STACKWALKER         ///< Support for callstack walking
SUPPORTS_SELECTIVE_BLENDING ///< means that blending can be enabled / disabled for each render target (not different blend states) (if you add this to a platform, also add to VisionCommon.inc)
SUPPORTS_CLOUD_LAYER

======================================================================

*/


#include "hkvPlatformCommon.h"

#ifdef WIN32
  #if !defined(_VR_DX11)
    #ifdef _VISION_WINRT
      #error DX9 not suppoted on WinRT or higher builds
    #endif
    #if !defined(_VR_DX9)
      #define _VR_DX9
    #endif
  #endif
  
  // sets common defines that are always set on Win32, independent of the Direct3D version
  #include "hkvPlatformWin32.h"

  #ifdef _VR_DX9
    #include "hkvPlatformDX9.h"
  #elif defined(_VR_DX11)
    #include "hkvPlatformDX11.h"
  #else
    #error Undefined DirectX version.
  #endif
  
#elif defined (_VISION_PS3)  

  #include "hkvPlatformPS3.h"
  
#elif defined (_VISION_XENON)  

  #include "hkvPlatformXenon.h"
  
#elif defined (_VISION_IOS)

  #include "hkvPlatformiOS.h"
  
#elif defined(_VISION_ANDROID)

  #include "hkvPlatformAndroid.h"
  
#elif defined(_VISION_PSP2)

  #include "hkvPlatformPSP2.h"
  
#elif defined(_VISION_WIIU)

  #include "hkvPlatformWiiU.h"
  
#else

  #error Undefined platform.
  
#endif



#ifndef VERTEX_RGBA8_ATTRIB_SWIZZLE_MODE
  #error The unsigned byte vertex attribute swizzle mode is not defined!
#endif




#endif // HKVPLATFORMDEFINES_H

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
