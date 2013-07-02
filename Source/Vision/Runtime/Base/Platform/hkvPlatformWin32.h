/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPLATFORMDEFINESWIN32_H
#define HKVPLATFORMDEFINESWIN32_H

  #define RESTRICT
  #define X86CPU
#ifndef _VISION_WINRT
	#define SUPPORTS_VLOGWIN
	#define SUPPORTS_REPORTDIALOG
#endif
#if !defined(_VISION_WINRT) || defined(_VISION_METRO)
	#define SUPPORTS_KEYBOARD
	#define SUPPORTS_MOUSE
#endif
  #define SUPPORTS_RELATIVE_PATHS
  #define SUPPORTS_REPORTGROUPS
  #define SUPPORTS_BROWSER
  #define SUPPORTS_SKINNED_TANGENTS
  #define SUPPORTS_HARDWARE_SKINNING
  #define SUPPORTS_RENDERABLE_CUBEMAPS
  #define SUPPORTS_SHADOW_MAPS
  #define SUPPORTS_MODULATEXN_LIGHTMAPPING
  #define SUPPORTS_FLOATINGPOINT_BLENDING
  #define SCREEN_MESSAGE_DEFAULT_SCALING 1.0f
  #define RESOURCE_MANAGER_DEFAULT_MEMORY_TEXTURES (256*1024*1024)
  #define SUPPORTS_OCCLUSION_QUERY
  #define SUPPORTS_PARTIAL_BUFFER_LOCKS
  #define SUPPORTS_RENDERERNODES
  #define SUPPORTS_TERRAIN
  #define SUPPORTS_BORDERCOLOR
  #define SUPPORTS_POSTPROCESSING
  #define SUPPORTS_LOD_HYSTERESIS_THRESHOLDING

  #define SUPPORTS_MULTITHREADING
  #define SUPPORTS_RESOURCEVIEWER
  #define SUPPORTS_RESOURCE_TIMESTAMP
  #define SUPPORTS_LOCALES
  #define SUPPORTS_VTHREAD
  #define SUPPORTS_RELATIVE_DIRECTORIES
#ifndef _VISION_WINRT
  #define SUPPORTS_WINDOWED_MODES

  #ifndef IGNORE_LIBPNG
    #define SUPPORTS_LIBPNG
  #endif

  #ifndef IGNORE_LIBJPG
    #define SUPPORTS_LIBJPG
  #endif

  #define SUPPORTS_ADVANCED_FILEOPS 
#endif  
  #define SUPPORTS_RESOLUTION_CHANGE
  #define SUPPORTS_MULTIPLE_DEVICES
  #define SUPPORTS_8_AND_24BPP_MODES
  #define SUPPORTS_SNAPSHOT_CREATION
  #define SUPPORTS_LIT_FILE_LOADING
  #define VSTREAMPROCESS_RUN_IN_THREADMANAGER
  #define V_IS_DIRECTX_PLATFORM
  #define V_IS_WINDOWS_PLATFORM
  #define SUPPORTS_ORTHOGONAL_LIGHTGRID
  #define NATIVE_PATH_SEPARATOR_BACKSLASH
#if !defined(_VISION_WINRT) || defined(_VISION_METRO)
  #define SUPPORTS_KEYBOARD
  #define SUPPORTS_MOUSE
  #define SUPPORTS_MULTITOUCH
#endif  
  // on Windows-based platforms, we support the file serve client for
  // vanilla win32 and x64, as well as Apollo
#if defined( _VISION_APOLLO ) || !defined( _VISION_WINRT )
  #define SUPPORTS_FILESERVE_CLIENT
#endif
  
  #define SUPPORTS_VTARGET
  #define V_HAS_VARIABLE_NUMBER_OF_CPU_CORES
  #define V_HAS_STACKWALKER
  #define V_SUPPORTS_EARLY_STATICS

  #include <mmintrin.h>
  #if defined( _MSC_VER) && _MSC_VER >= 1700
    #include <xmmintrin.h>
  #endif
  #define V_PRECACHE(offset, ptr) _mm_prefetch(((char*)ptr)+offset,_MM_HINT_T1)
  #define V_CACHE_LINE_SIZE 128

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
