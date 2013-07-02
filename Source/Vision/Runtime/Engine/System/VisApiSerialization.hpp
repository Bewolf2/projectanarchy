/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSerialization.hpp

#ifndef FR_DEFINE_SERIALIZATION
#define FR_DEFINE_SERIALIZATION

#include <Vision/Runtime/Engine/System/VisApiDefs.hpp>

// Attention: This header is only for internal use, none of the provided macros
// and defines should be used in the gamecode.
// This header contains information about the serialization archive version of Vision
// In order to get the current serialization archive version of the VISION.DLL which you are
// currently using, call the function Vision::GetArchiveVersion. The DLL which
// you are currently using might already have been compiled with a newer header than this one.


// readable version numbers, also easier to find in the source code as numbers
#define VISION_ARCHIVE_VERSION_030   30
#define VISION_ARCHIVE_VERSION_029   29
#define VISION_ARCHIVE_VERSION_028   28
#define VISION_ARCHIVE_VERSION_025   25
#define VISION_ARCHIVE_VERSION_024   24
#define VISION_ARCHIVE_VERSION_022   22
#define VISION_ARCHIVE_VERSION_021   21
#define VISION_ARCHIVE_VERSION_020   20
#define VISION_ARCHIVE_VERSION_016   16
#define VISION_ARCHIVE_VERSION_015   15
#define VISION_ARCHIVE_VERSION_014   14
#define VISION_ARCHIVE_VERSION_013   13
#define VISION_ARCHIVE_VERSION_012   12
#define VISION_ARCHIVE_VERSION_011   11
#define VISION_ARCHIVE_VERSION_010   10
#define VISION_ARCHIVE_VERSION_009   9
#define VISION_ARCHIVE_VERSION_008   8
#define VISION_ARCHIVE_VERSION_007   7
#define VISION_ARCHIVE_VERSION_006   6
#define VISION_ARCHIVE_VERSION_005   5
#define VISION_ARCHIVE_VERSION_004   4
#define VISION_ARCHIVE_VERSION_003   3
#define VISION_ARCHIVE_VERSION_002   2

#define VISION_ARCHIVE_VERSION  VISION_ARCHIVE_VERSION_030


/*
-> Add the changes of new vision archive version here
  Version 029 (Vision SDK 7.7)
  - Switched PhysX binding to component system. New serialization version of vPhysXJoint class required.

  Version 028 (Vision SDK version 7.6)
  - Cleaned up class structure (VisTypedEngineObject_cl...VisBaseEntity_c)

  Version 025 (Vision SDK version 7.1)
  - Object key in VisObject3D_cl replaces entity key etc.

  Version 024 (Vision SDK version 7.0)
  - AutoRemove property on VisApiAnimEvent

  Version 022 (Vision SDK version 7.0)
  - constraint changes

Version 021 (Vision SDK version 6.3)
  - object3D unique IDs
  - light source version number

Version 020 (Engine Version 6.0)
  - left some space for other versions in 5.5
  - new light attenuation

Version 014 (Engine Version 5.1)
  - entity: new animation state serialization

Version 013 (Engine Version 5.1)
  - entity: shadow casting flag fix

Version 012 (Engine Version 5.1)
  - Scale does not get serialized anymore in VisObject3D_cl (already serialized in VisBaseEntity_cl)
  - VisLightSource_cl class does not serialize position and orientation data anymore (now part of VisObject3D_cl)
  - Introduced new class hierarchy for VisBaseEntity_cl class:
      New Class Structure:                        Old Class Structure:
      ---------------------                       --------------------
      VisTypedEngineObject_cl                     VisTypedEngineObject_cl
      VisBase_cl                                  VarManager_cl
      VisObject3D_cl                              BaseEntity_cl
      VarManager_cl                               VisBase_cl
      BaseEntity_cl                               VisObject3D_cl
      VisBaseEntity_cl                            VisBaseEntity_cl  
  
  - Introduced new class hierarchy for VisLightSource_cl class:
      New Class Structure:                        Old Class Structure:
      ---------------------                       --------------------
      VisTypedEngineObject_cl                     VisTypedEngineObject_cl
      VisBase_cl                                  VisLightSource_cl
      VisObject3D_cl                              
      VisLightSource_cl

Version 010 (Engine Version 4.7.11)
  Use 32bit light masks

Version 011 (Engine Version 4.8)
  Added ProcessZeroWeighting variable to animation component

Version 010 (Engine Version 4.8)
  Serialize parent in VisObject3D_cl class

Version 009 (Engine Version 4.5.14)
  Load time based events in model interface

Version 008 (Engine Version 4.2.0)
  Break compatibility to older versions

Version 007 (Engine Version 4.0.5)
  Added Object3D flag using euler angles

Version 006 (Engine Version 3.6.0):
  Added lightsource color animation custom curves (VisLightSrc_AnimColor_cl class)

Version 005 (Engine Version 3.5.8):
  Added m_bIsStatic member to the VisionPhysicsObject_cl class 

Version 004 (Engine Version 3.5):
  Added pOldAnimSequence member to the VisAnimationUnit_cl class 
  Added pSkeletalAnim/pMuscleAnim member to the VisModelAnimSequence_cl class

Version 003 (Engine Version 2.4):
  Added m_iAbsPhaseTime member to the VisModelAnimPhase_cl class 

Version 002 (Engine Version 2.3):
  First version of the serialization in Vision

Version 001 :
  This version has been skipped since the default value of the loading version is 1.
  By skipping this version we can easily detect whether the game programmer forgot
  to set the loading version for an archive (as done in the following function)
*/


// the following function needs to be updated in case an engine version can't load old
// archives anymore
inline BOOL VisCheckForValidArchive( int iLoadedArchiveVersion, int iEngineArchiveVersion )
{
  // check whether SetLoadingVersion has been called for the archive (which is required)
  if ( iLoadedArchiveVersion == 1 )
    return FALSE;

  // check whether the archive version is higher than the currently supported archive version of the engine
  if ( iLoadedArchiveVersion > iEngineArchiveVersion )
    return FALSE;

  return TRUE;
}


// the following macro can additionally be used for sanity checks within Vision
#define CHECK_FOR_VALID_ARCHIVE( ar ) \
  if ( ar.GetLoadingVersion() == 1 ) \
    VULPERROR_0( FALSE, "Failed on loading an archive due to an missing loading version. VArchive::SetLoadingVersion needs to be called on the archive before the serialization of any engine objects can start." ) \
  if ( !VisCheckForValidArchive( ar.GetLoadingVersion(), Vision::GetArchiveVersion()) ) \
  { \
    VULPERROR_2( FALSE, "Failed on loading an archive of the version %d with an engine which only supports archive versions up to %d. This archive has been saved with a newer engine version. Please update to the latest engine version.", ar.GetLoadingVersion(), Vision::GetArchiveVersion() ); \
  }


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
