/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisAppSettings.h

#ifndef __VISION_APPSETTINGS_H
#define __VISION_APPSETTINGS_H

#ifdef _VISION_IOS

#ifndef _VISION_DOC

typedef enum TVisionOrientations
{
  VisOrientationPortrait           = (1 << 0),
  VisOrientationPortraitUpsideDown = (1 << 1),
  VisOrientationLandscapeRight     = (1 << 2),
  VisOrientationLandscapeLeft      = (1 << 3),
  VisOrientationAllPortrait        = VisOrientationPortrait | VisOrientationPortraitUpsideDown,
  VisOrientationAllLandscape       = VisOrientationLandscapeRight | VisOrientationLandscapeLeft,
  VisOrientationAll                = VisOrientationAllPortrait | VisOrientationAllLandscape
} VisionOrientations;

typedef struct TVisionAppSettings
{
  const char* szAppDirectory;
  const char* szDocumentsDirectory;
  const char* szCachesDirectory;
  
  int iSyncFrameRate;
  int iSupportedOrientations;
} VisionAppSettings;

extern VisionAppSettings g_VisionAppSettings;


#if defined (__cplusplus)

static int SetSupportedOrientations(int orientations)
{
  g_VisionAppSettings.iSupportedOrientations = orientations;
  return 0;
}

static int SetSyncFrameRate(int frameRate)
{
  g_VisionAppSettings.iSyncFrameRate = frameRate;
  return 0;
}

#define VISION_SET_SUPPORTED_ORIENTATIONS(orientations) static int __orientDummy = SetSupportedOrientations(orientations);
#define VISION_SET_SYNC_FRAMERATE(frameRate) static int __syncFRDummy = SetSyncFrameRate(frameRate);

#endif //__cplusplus

#endif //!_VISION_DOC

#else //_VISION_IOS

  #define VISION_SET_SUPPORTED_ORIENTATIONS(orientations)
  #define VISION_SET_SYNC_FRAMERATE(frameRate)

#endif //_VISION_IOS
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
