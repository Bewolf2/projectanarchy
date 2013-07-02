/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



#ifndef VISION_EXPORT_PROCESSOR__H
#define VISION_EXPORT_PROCESSOR__H

#include <Vision/Tools/SceneDataConverter/Processor/hkVisionProcessorOptions.h>

class hkVisionProcessor : public hkReferencedObject
{

public: 

  hkVisionProcessor();

  // Full export
  hkResult processAndExport(VGScene* scene, VGVisionImporterExporter::DataFormat format, const char* filename, const hkvMat4& pivot);

  // Prefabs have a reduced processing set
  hkResult processAndExportPrefab(VGScene* scene, const char* filename, const hkvMat4& pivot);

  VExporterOptions m_options;

  struct InputOptions
  {
    bool m_rotateYtoZup; // We shouldn't even need that, in here, as hkx should guarantee the conversion, beforehand, to sync filters, etc.. --> see EXP-2351 for lengthy discussion.
  } m_inputOptions;      // Note that this should NOT be moved to or merged with the general Vision export options as it is unneeded for GUI, scripting and config presets.

};


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
