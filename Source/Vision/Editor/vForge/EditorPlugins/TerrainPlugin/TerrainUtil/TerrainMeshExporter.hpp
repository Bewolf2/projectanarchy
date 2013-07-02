/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __MOBILE_TERRAIN_MESH_EXPORTER_HPP__
#define __MOBILE_TERRAIN_MESH_EXPORTER_HPP__

#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainUtil/TerrainUtilModule.hpp>

#include <Vision/Tools/Libraries/Geom2/vGeom2.hpp>

class VEditableTerrain;
class VEditableTerrainSector;
class VTerrainSectorRenderer;

class hkMeshSystem;
class hkMeshShape;

struct hkGeometry;

class TerrainMeshExporter
{
public:
  struct MeshExportParameters
  {
    VEditableTerrain* pTerrain;

    bool bBakedLightmaps;
    bool bSimplifyMesh;
    bool bUseLegacySolver;
    int iTextureResolution;
    float fMaxErrorDistance;

    // Internally used only
    int iBorderWidth;
  };

  /// \brief
  ///   VThreadedTask class that performs sector export
  class VExportSectorTask : public VThreadedTask
  {
  public:
    TERRAINUTIL_IMPEXP VExportSectorTask() : m_bResult(true), m_bFinished(true), m_pParameters(NULL), m_iSectorX(-1), m_iSectorY(-1) {}
    TERRAINUTIL_IMPEXP ~VExportSectorTask() {}

    // This is the key override of VThreadedTask. The Run function is called to process this job in a separate thread.
    // This function is called automatically when the task is scheduled via Vision::GetThreadManager()->ScheduleTask.
    // Note that everything inside this function must be thread safe.
    VOVERRIDE void Run(VManagedThread *pThread);

    // Task members
    bool m_bResult;
    bool m_bFinished;
    const MeshExportParameters* m_pParameters;
    int m_iSectorX, m_iSectorY;
  };

  // Export baked texture for given sector (Note: not multithread-safe)
  TERRAINUTIL_IMPEXP static bool ExportSectorTexture(const MeshExportParameters& parameters, int iSectorX, int iSectorY);

  // Export .vmesh for given sector
  TERRAINUTIL_IMPEXP static bool ExportSectorMesh(const MeshExportParameters& parameters, int iSectorX, int iSectorY);

private:
  static void CreateSectorMesh(VEditableTerrainSector *pSector, hkGeometry* pGeometry, float bSimplifyMesh, float fMaxErrorDistance);
  static void SimplifySectorMesh(hkGeometry* pGeometry, float fMaxErrorDistance, bool bUseLegacySolver);
#ifdef VGEOM2_LIB
  static void CreateSectorMaterial(VEditableTerrainSector *pSector, VGScene& scene, bool bBakedLightmaps);
  static void CreateSectorExportMesh(const MeshExportParameters& parameters, VEditableTerrainSector *pSector, VGScene& scene, hkGeometry* pGeometry, bool bBakedLightmaps);
#endif
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
