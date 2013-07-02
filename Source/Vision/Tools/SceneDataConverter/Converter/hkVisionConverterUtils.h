/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef VISION_EXPORT_CONVERTER_UTILS__H
#define VISION_EXPORT_CONVERTER_UTILS__H

#include <Vision/Tools/SceneDataConverter/Processor/hkVisionProcessorOptions.h>
class VGScene;
class VGTransformable;

struct hkVisionConverterUtils
{
  
  static void HK_CALL mergeSceneAndRebaseAnimations( hkxScene* destScene, hkxScene* srcScene, hkArray<VExporterOptions::AnimInfo*>& anims );
  
  static const VGTransformable* HK_CALL findTransformable( const VGScene* scene, const VString& nameStr );

  // @@@ DEPRECATED: remove following function... it tries to lookup by name things for all the code that sits on top of these utils, that can't be looked up, generically,
  // but is rather dependent on the specific application. Use next function with specific code per use-case along with the recenterer processor, instead.
  static void HK_CALL getPivotTransformUsingOptions(const VGScene* scene, const VGTransformable* selectedNode, const VExporterOptions& options, hkvMat4& matrixOut);

  // Returns 0 for special token other than <selection>, 1 for <selection> and 2 for custom recentering (usually a node name).
  static int HK_CALL getPivotTransformType(const VExporterOptions& options);

};


#endif //VISION_EXPORT_CONVERTER_UTILS__H

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
