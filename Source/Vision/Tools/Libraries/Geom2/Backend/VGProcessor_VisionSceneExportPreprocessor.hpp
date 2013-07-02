/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once



/// \brief
///   Vision scene preprocessor
///
/// This is a very specific processor that preprocesses a vGeom2 scene for later exports. It basically runs
/// over a vGeom2 scene and alters it in ways to prepare it for Vision file format specific exports. It was
/// written mainly to translate custom user properties to node-data (filling a vGeom2 scene proved to be
/// easier across multiple tool environments this way, for some options).
///
/// Furthermore it checks whether texture paths have to be altered (e.g. for texture animations), and will
/// probably be extended every once in a while, as it is a nice abstraction for specific scene details, etc..
/// It is a good idea to keep this on your processor stack as first processor, if you intend to use the
/// VGProcessor_VisionExporter processor, as well.
///
/// This processor uses qhull for some computations, thus introducing an external dependency on qhull.
class VGEOM2_IMPEXP_CLASS VGProcessor_VisionSceneExportPreprocessor : public VGDynamicProcessor<VGProcessor_VisionSceneExportPreprocessor>
{
public:
  VGProcessor_VisionSceneExportPreprocessor();
  virtual ~VGProcessor_VisionSceneExportPreprocessor()   { }



  /// \brief
  ///   Processes a scene
  ///
  /// \param scene
  ///   Scene data to be preprocess for Vision specific exports
  ///
  /// \return
  ///   False if preprocessing failed
  virtual bool Process(VGScene& scene) const;//@@@ test
};

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
