/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodIncludes.hpp

#ifndef VISION_FMOD_INCLUDED_HPP
#define VISION_FMOD_INCLUDED_HPP

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_event.hpp>
#if !defined(_VISION_WIIU)
#include <fmod_event_net.hpp>
#endif

#if defined(_VISION_XENON)
#include <fmodxbox360.h>
#elif defined(_VISION_PS3)
#include <fmodps3.h>
#elif defined(_VISION_PSP2)
#include <fmodngp.h>
#elif defined(_VISION_IOS)
#include <fmodiphone.h>
#endif


// module for serialization
extern VModule g_FmodModule;


#ifdef FMODENGINEPLUGIN_EXPORTS
#define FMOD_IMPEXP __declspec(dllexport)
#elif defined (FMODENGINEPLUGIN_IMPORTS)
#define FMOD_IMPEXP __declspec(dllimport)
#else
#define FMOD_IMPEXP
#endif


// forward declarations
class VFmodManager;
class VFmodSoundResource;
class VFmodSoundObject;
class VFmodSoundObjectCollection;
class VFmodEventGroup;
class VFmodEvent;
class VFmodEventCollection;
class VFmodCollisionMeshInstance;
class VFmodCollisionMeshInstanceCollection;
class VFmodReverb;
class VFmodReverbCollection;


// typedefs 
typedef VSmartPtr<VFmodSoundResource> VFmodSoundResourcePtr;
typedef VSmartPtr<VFmodSoundObject> VFmodSoundObjectPtr;
typedef VSmartPtr<VFmodEventGroup> VFmodEventGroupPtr;
typedef VSmartPtr<VFmodEvent> VFmodEventPtr;
typedef VSmartPtr<VFmodCollisionMeshInstance> VFmodCollisionMeshInstancePtr;
typedef VSmartPtr<VFmodReverb> VFmodReverbPtr;


// defines

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Fmod Designer tool specific
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define VFMOD_DESIGNER_DIR "\\redistsdks\\fmod\\"
#define VFMOD_DESIGNER_INSTALLED_VISION_DIR "..\\..\\ThirdParty\\fmod\\"
#define VFMOD_DESIGNER_EXE_FOLDER "\\tools\\"
#define VFMOD_DESIGNER_EXE "fmod_designer.exe"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform specific
/////////////////////////////////////////////////////////////////////////////////////////////////////

// -------------------------------------------------------------------------- //
// currently supported platforms: PC/ Xbox360/ PS3/ PSP2/ Android/ iOS
//
// THINGS TO CONSIDER WHEN ADDING/ REMOVING A PLATFORM:
// - add/ remove VFMOD_PLATFORM_STR for the respective platform
// - adjust VFMOD_ALL_PLATFORM_STRS
// - add/ remove VFMOD_PLATFORM_STR define for the respective platform
// - add define VFMOD_SUPPORTS_NETWORK, if platform supports Fmod Designer 
//   network API
//
// -------------------------------------------------------------------------- //
#define VFMOD_NUM_SUPPORTED_PLATFORMS 6 

// platform-specific strings (used for event-projects)
#define VFMOD_PLATFORM_STR_PC       "pc"
#define VFMOD_PLATFORM_STR_XENON    "xbox360"
#define VFMOD_PLATFORM_STR_PS3      "ps3"
#define VFMOD_PLATFORM_STR_PSP2     "vita"
#define VFMOD_PLATFORM_STR_ANDROID  "android"
#define VFMOD_PLATFORM_STR_IOS      "ios"
#define VFMOD_PLATFORM_STR_WIIU     "wiiu"

// VFMOD_SUPPORTS_NETWORK : support for Fmod Designer Network API 

#if defined (WIN32) 
#define VFMOD_PLATFORM_STR VFMOD_PLATFORM_STR_PC
#define VFMOD_SUPPORTS_NETWORK
#elif defined (_VISION_XENON) 
#define VFMOD_PLATFORM_STR VFMOD_PLATFORM_STR_XENON
#define VFMOD_SUPPORTS_NETWORK
#elif defined (_VISION_PS3) 
#define VFMOD_PLATFORM_STR VFMOD_PLATFORM_STR_PS3
#define VFMOD_SUPPORTS_NETWORK
#elif defined (_VISION_PSP2)
#define VFMOD_PLATFORM_STR VFMOD_PLATFORM_STR_PSP2
#elif defined (_VISION_ANDROID)
#define VFMOD_PLATFORM_STR VFMOD_PLATFORM_STR_ANDROID 
#elif defined (_VISION_IOS)
#define VFMOD_PLATFORM_STR VFMOD_PLATFORM_STR_IOS
#define VFMOD_SUPPORTS_NETWORK
#elif defined (_VISION_WIIU)
#define VFMOD_PLATFORM_STR VFMOD_PLATFORM_STR_WIIU
#undef VFMOD_SUPPORTS_NETWORK
#else
#error Undefined platform!
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
// conversion helpers
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define VFMOD_ASSIGN_VECTOR(_dest, _src) {_dest.x =_src.x;_dest.y =_src.y;_dest.z =_src.z;}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// bitflags used by sound/ event instances
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define VFMOD_FLAG_NONE                         (0)
#define VFMOD_FLAG_LOOPED                       (1<<0)            // sound instance only
#define VFMOD_FLAG_PAUSED                       (1<<2)
#define VFMOD_FLAG_MUTE                         (1<<3)
#define VFMOD_FLAG_NODISPOSE                    (1<<4)
#define VFMOD_FLAG_MUSIC                        (1<<5)            // sound instance only
#define VFMOD_FLAG_SURVIVE_UNLOAD_WORLD         (1<<6)
#define VFMOD_FLAG_DEFAULT                      (VFMOD_FLAG_LOOPED)   // sound instance only
#define VFMOD_FLAG_BITMASK                      0x000000FF

/////////////////////////////////////////////////////////////////////////////////////////////////////
// bitflags used by sound resources
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define VFMOD_RESOURCEFLAG_2D                   (0)
#define VFMOD_RESOURCEFLAG_3D                   (1<<8)
#define VFMOD_RESOURCEFLAG_STREAM               (1<<9)
#define VFMOD_RESOURCEFLAG_LINEAR_ATTENUATION   (1<<10)
#define VFMOD_RESOURCEFLAG_LOG_ATTENUATION      (1<<11)
#define VFMOD_RESOURCEFLAG_ASYNCRONOUS		      (1<<12)
#define VFMOD_RESOURCEFLAG_COMPRESSED_DATA      (1<<13)
#define VFMOD_RESOURCEFLAG_DEFAULT              (VFMOD_RESOURCEFLAG_3D|VFMOD_RESOURCEFLAG_LINEAR_ATTENUATION)
#define VFMOD_RESOURCEFLAG_BITMASK              0x0000FF00

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Known trigger actions for MessageFunction
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define VFMOD_TRIGGER_PAUSE                     "Pause"
#define VFMOD_TRIGGER_RESUME                    "Resume"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// VFmodReverb presets
/////////////////////////////////////////////////////////////////////////////////////////////////////

#define VFMOD_REVERBPRESET_CUSTOM_ENVIRONMENT   25 // use MAX value of the Fmod reverb Environment for the custom preset

/*                                                                Env   Room   RoomHF  RmLF DecTm   DecHF    Refl  RefDel   Revb  RevDel  HFRef    LFRef   Diffus  Density */
#define VFMOD_REVERBPRESET_OFF                  VFmodReverbProps( -1,  -10000, -10000, 0,   1.00f,  1.00f,  -2602, 0.007f,   200, 0.011f, 5000.0f, 250.0f,   0.0f,   0.0f )
#define VFMOD_REVERBPRESET_GENERIC              VFmodReverbProps(  0,  -1000,  -100,   0,   1.49f,  0.83f,  -2602, 0.007f,   200, 0.011f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_PADDEDCELL           VFmodReverbProps(  1,  -1000,  -6000,  0,   0.17f,  0.10f,  -1204, 0.001f,   207, 0.002f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_ROOM                 VFmodReverbProps(  2,  -1000,  -454,   0,   0.40f,  0.83f,  -1646, 0.002f,    53, 0.003f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_BATHROOM             VFmodReverbProps(  3,  -1000,  -1200,  0,   1.49f,  0.54f,   -370, 0.007f,  1030, 0.011f, 5000.0f, 250.0f, 100.0f,  60.0f )
#define VFMOD_REVERBPRESET_LIVINGROOM           VFmodReverbProps(  4,  -1000,  -6000,  0,   0.50f,  0.10f,  -1376, 0.003f, -1104, 0.004f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_STONEROOM            VFmodReverbProps(  5,  -1000,  -300,   0,   2.31f,  0.64f,   -711, 0.012f,    83, 0.017f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_AUDITORIUM           VFmodReverbProps(  6,  -1000,  -476,   0,   4.32f,  0.59f,   -789, 0.020f,  -289, 0.030f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_CONCERTHALL          VFmodReverbProps(  7,  -1000,  -500,   0,   3.92f,  0.70f,  -1230, 0.020f,    -2, 0.029f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_CAVE                 VFmodReverbProps(  8,  -1000,  0,      0,   2.91f,  1.30f,   -602, 0.015f,  -302, 0.022f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_ARENA                VFmodReverbProps(  9,  -1000,  -698,   0,   7.24f,  0.33f,  -1166, 0.020f,    16, 0.030f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_HANGAR               VFmodReverbProps(  10, -1000,  -1000,  0,   10.05f, 0.23f,   -602, 0.020f,   198, 0.030f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_CARPETTEDHALLWAY     VFmodReverbProps(  11, -1000,  -4000,  0,   0.30f,  0.10f,  -1831, 0.002f, -1630, 0.030f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_HALLWAY              VFmodReverbProps(  12, -1000,  -300,   0,   1.49f,  0.59f,  -1219, 0.007f,   441, 0.011f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_STONECORRIDOR        VFmodReverbProps(  13, -1000,  -237,   0,   2.70f,  0.79f,  -1214, 0.013f,   395, 0.020f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_ALLEY                VFmodReverbProps(  14, -1000,  -270,   0,   1.49f,  0.86f,  -1204, 0.007f,    -4, 0.011f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_FOREST               VFmodReverbProps(  15, -1000,  -3300,  0,   1.49f,  0.54f,  -2560, 0.162f,  -229, 0.088f, 5000.0f, 250.0f,  79.0f, 100.0f )
#define VFMOD_REVERBPRESET_CITY                 VFmodReverbProps(  16, -1000,  -800,   0,   1.49f,  0.67f,  -2273, 0.007f, -1691, 0.011f, 5000.0f, 250.0f,  50.0f, 100.0f )
#define VFMOD_REVERBPRESET_MOUNTAINS            VFmodReverbProps(  17, -1000,  -2500,  0,   1.49f,  0.21f,  -2780, 0.300f, -1434, 0.100f, 5000.0f, 250.0f,  27.0f, 100.0f )
#define VFMOD_REVERBPRESET_QUARRY               VFmodReverbProps(  18, -1000,  -1000,  0,   1.49f,  0.83f, -10000, 0.061f,   500, 0.025f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_PLAIN                VFmodReverbProps(  19, -1000,  -2000,  0,   1.49f,  0.50f,  -2466, 0.179f, -1926, 0.100f, 5000.0f, 250.0f,  21.0f, 100.0f )
#define VFMOD_REVERBPRESET_PARKINGLOT           VFmodReverbProps(  20, -1000,  0,      0,   1.65f,  1.50f,  -1363, 0.008f, -1153, 0.012f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_SEWERPIPE            VFmodReverbProps(  21, -1000,  -1000,  0,   2.81f,  0.14f,    429, 0.014f,  1023, 0.021f, 5000.0f, 250.0f,  80.0f,  60.0f )
#define VFMOD_REVERBPRESET_UNDERWATER           VFmodReverbProps(  22, -1000,  -4000,  0,   1.49f,  0.10f,   -449, 0.007f,  1700, 0.011f, 5000.0f, 250.0f, 100.0f, 100.0f )
#define VFMOD_REVERBPRESET_CUSTOM               VFmodReverbProps(  VFMOD_REVERBPRESET_CUSTOM_ENVIRONMENT,  -1000,  -100,   0,   1.49f,  0.83f,  -2602, 0.007f,   200, 0.011f, 5000.0f, 250.0f, 100.0f, 100.0f )

#define VFMOD_ALL_REVERBPRESETS \
  { \
    VFMOD_REVERBPRESET_OFF, VFMOD_REVERBPRESET_GENERIC, VFMOD_REVERBPRESET_PADDEDCELL, VFMOD_REVERBPRESET_ROOM, \
    VFMOD_REVERBPRESET_BATHROOM, VFMOD_REVERBPRESET_LIVINGROOM, VFMOD_REVERBPRESET_STONEROOM, VFMOD_REVERBPRESET_AUDITORIUM, \
    VFMOD_REVERBPRESET_CONCERTHALL, VFMOD_REVERBPRESET_CAVE, VFMOD_REVERBPRESET_ARENA, VFMOD_REVERBPRESET_HANGAR, \
    VFMOD_REVERBPRESET_CARPETTEDHALLWAY, VFMOD_REVERBPRESET_HALLWAY, VFMOD_REVERBPRESET_STONECORRIDOR, VFMOD_REVERBPRESET_ALLEY, \
    VFMOD_REVERBPRESET_FOREST, VFMOD_REVERBPRESET_CITY, VFMOD_REVERBPRESET_MOUNTAINS, VFMOD_REVERBPRESET_QUARRY, \
    VFMOD_REVERBPRESET_PLAIN, VFMOD_REVERBPRESET_PARKINGLOT, VFMOD_REVERBPRESET_SEWERPIPE, VFMOD_REVERBPRESET_UNDERWATER, \
    VFMOD_REVERBPRESET_CUSTOM \
  }

#endif // VISION_FMOD_INCLUDED_HPP

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
