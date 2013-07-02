/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSceneExporter.hpp

#ifndef VSCENEEXPORTER_HPP_INCLUDED
#define VSCENEEXPORTER_HPP_INCLUDED

#ifdef WIN32

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>


/// \brief
///   Implements IVisSceneExporter_cl for standard vscene/vzone/vprefab export
class VSceneExporter : public IVisSceneExporter_cl
{
public:
  SCENE_IMPEXP VSceneExporter();
  SCENE_IMPEXP virtual ~VSceneExporter();
  SCENE_IMPEXP virtual VExportShapesArchive* StartVSceneExport(IVFileOutStream *pOut, bool bCloseFile, const VZoneInfo_t *pEmbeddedZones, int iEmbeddedZoneCount, VSceneExportFlags_e eFlags=VExport_All) HKV_OVERRIDE;
  SCENE_IMPEXP virtual VExportShapesArchive* StartVZoneExport(IVFileOutStream *pOut, bool bCloseFile, VZoneInfo_t &info) HKV_OVERRIDE;
  SCENE_IMPEXP virtual VExportShapesArchive* StartVPrefabExport(IVFileOutStream *pOut, bool bCloseFile) HKV_OVERRIDE;
  SCENE_IMPEXP virtual void EndExport() HKV_OVERRIDE;

protected:
  enum VisSceneExportType_e
  {
    EXPORTTYPE_VSCENE,
    EXPORTTYPE_VZONE,
    EXPORTTYPE_VPREFAB
  };

  void WriteVSceneFile();
  void WriteVZoneFile();
  void WriteVPrefabFile();
  void PrepareShapesArchive(bool bWithRangeInfo);

  bool m_bCloseFile;
  VisSceneExportType_e m_eType;
  VExportShapesArchive *m_pArchive;
  VMemoryStreamPtr m_spMemStream;
  class VShapesMemoryOutStream *m_pShapesOut;
  IVFileOutStream *m_pOut;
  VZoneInfo_t *m_pEmbeddedZones;
  int m_iEmbeddedZoneCount;
  int m_eExportFlags; // VSceneExportFlags_e
 // int m_iFilePosMarker;
};


/// \brief
///   Factory that creates exporters of type VSceneExporter. One global instance is available through VSceneExporterFactory::GetFactory() and installed so vForge can use it.
class VSceneExporterFactory : public IVisSceneExporterFactory_cl
{
public:
  SCENE_IMPEXP virtual IVisSceneExporter_cl *CreateExporter() HKV_OVERRIDE
  {
    return new VSceneExporter();
  }
  static SCENE_IMPEXP VSceneExporterFactory *GetFactory();
  static VSceneExporterFactory g_ExporterFactory;
};


#endif // WIN32


#endif // VSCENEEXPORTER_HPP_INCLUDED

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
