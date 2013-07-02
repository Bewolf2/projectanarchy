/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file StreamProcessVisibilityJob.hpp

#ifndef DEFINE_STREAMPROCESSVISIBILITYJOB
#define DEFINE_STREAMPROCESSVISIBILITYJOB

#ifdef SPU
#include <Vision/Runtime/Base/System/Threading/StreamProcessor/VStreamProcessingTask.hpp>
#endif

#ifndef _VISION_DOC

// Forward declarations
class VisVisibilityZone_cl;

// Structure used to pass visibility information which is consistent across a stream to the SPUs
struct VisVisibilityStreamConfig_t
{
  hkvVec3 cameraPos;
  hkvVec3 cameraDir;
  float fLODScaleSqr;
  int iPlaneFlags;
  int iVisDataBlockOffset;
  int iFilterMask;
  int eSceneElements;
  VisVisibilityZone_cl *pVisZone;
};

// Visibility data block for each stream element
struct VisVisibilityDataBlock_t
{
  hkvAlignedBBox boundingBox;
  int   iVisibleMask;
  float fFarClipDistance;
  int   iPerformTestFlags;
  float fNearClipDistance;
  hkvVec3 vClipReference;
};

class VStreamProcessVisibilityJob : public VStreamProcessingKernel
{
public:
  void Execute(VStreamProcessingTask *pTask);
};

#endif

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
