/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/Engine/SceneManagement/VisApiZone.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneExporter.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VPrefab.hpp>

#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityLODHysteresis.hpp>

#ifdef WIN32

VSceneExporterFactory VSceneExporterFactory::g_ExporterFactory;


VSceneExporterFactory *VSceneExporterFactory::GetFactory()
{
  return &g_ExporterFactory;
}


// helper class to patch data later
class VShapesMemoryOutStream : public VMemoryOutStream
{
public:
  VShapesMemoryOutStream() : VMemoryOutStream(NULL, new VMemoryStream(NULL,64*1024))
  {
    m_iStoredPosition = -1;
  }

  ~VShapesMemoryOutStream()
  {
  }

  void ReservePatchedIntegers(VArchive &ar, int iCount)
  {
    VASSERT(m_iStoredPosition==-1);
    m_iStoredPosition = ar.GetPos();
    int iDummy = 0;
    for (int i=0;i<iCount;i++)
      ar.Write(&iDummy,sizeof(int));
  }

  int *GetPatchedIntegerPtr(int iCount)
  {
    if (m_iStoredPosition<0)
      return NULL;
    char *c = (char *)GetStream()->GetDataPtr();
    int *pData = (int *)&(c[m_iStoredPosition]);
    for (int i=0;i<iCount;i++)
    {
      VASSERT(pData[i]==0);
    }
    return pData;
  }

  int m_iStoredPosition;
};



VSceneExporter::VSceneExporter()
{
  m_pArchive = NULL;
  m_pShapesOut = NULL;
  m_pEmbeddedZones = NULL;
  m_iEmbeddedZoneCount = 0;
  m_eExportFlags = VExport_All;
}

VSceneExporter::~VSceneExporter()
{
  VASSERT(m_pArchive==NULL);
  VASSERT(m_pEmbeddedZones==NULL);

}


VExportShapesArchive* VSceneExporter::StartVSceneExport(IVFileOutStream *pOut, bool bCloseFile, const VZoneInfo_t *pEmbeddedZones, int iEmbeddedZoneCount, VSceneExportFlags_e eFlags)
{
  VASSERT(m_pArchive==NULL && pOut!=NULL);
  VASSERT(m_pEmbeddedZones==NULL);

  m_eExportFlags = eFlags;
  m_eType = EXPORTTYPE_VSCENE;
  m_pOut = pOut;
  m_bCloseFile = bCloseFile;

  VisVisibilityZone_cl::SetVisibilityZoneSerializationMode(VIS_SERIALIZE_FULL); // non-default
  PrepareShapesArchive(true);

  m_iEmbeddedZoneCount = iEmbeddedZoneCount;
  if (m_iEmbeddedZoneCount>0)
  {
    m_pEmbeddedZones = new VZoneInfo_t[m_iEmbeddedZoneCount];
    for (int i=0;i<m_iEmbeddedZoneCount;i++)
      m_pEmbeddedZones[i] = pEmbeddedZones[i];
  }
  // write some count statistics into the file. Since we have detailed information only later, write
  // placeholders and patch later (it is a memory stream)
  int iDummy = 0;
  (*m_pArchive) << Vision::GetArchiveVersion();
  m_pShapesOut->ReservePatchedIntegers(*m_pArchive,4); // used to patch memory stream later

  // scene version 7 : count statistics
  VStaticIntDictionary<1024> statistics;
    statistics.Set('_ENT',VisBaseEntity_cl::ElementManagerGetUsedCount());
    statistics.Set('_LGT',VisLightSource_cl::ElementManagerGetUsedCount());
    statistics.Set('_PTG',VisParticleGroup_cl::ElementManagerGetUsedCount());
    statistics.Set('_VOB',VisVisibilityObject_cl::ElementManagerGetUsedCount());
    statistics.Set('_SMI',VisStaticMeshInstance_cl::ElementManagerGetUsedCount());
    statistics.Set('_SGI',VisStaticGeometryInstance_cl::ElementManagerGetUsedCount());
    statistics.Set('_MBO',VisMeshBufferObject_cl::ElementManagerGetUsedCount());
    statistics.Set('_PTH',VisPath_cl::ElementManagerGetUsedCount());
  statistics.SerializeX(*m_pArchive);

  //active object size
  m_pArchive->m_bUseObjectLengths = true;

  // write out reserved shapes:

  // #1 : all visibility zones (just standard shapes). At loading time we need to add each to the scene manager
  // Scene version 6
  IVisSceneManager_cl *pSM = Vision::GetSceneManager();
  if (pSM)
  {
    int iCount = pSM->GetNumVisibilityZones();
    (*m_pArchive) << iCount;
    for (int i=0;i<iCount;i++)
      (*m_pArchive) << pSM->GetVisibilityZone(i);
  }
  else
    (*m_pArchive) << (int)0;


  // #2 : the first shape is the scene's script object:
  IVScriptInstance *pScriptObj = (Vision::GetScriptManager() && (m_eExportFlags&VExport_SceneScript)) ? Vision::GetScriptManager()->GetSceneScript() : NULL;
  (*m_pArchive) << pScriptObj;

  // #14 : Global script think interval
  (*m_pArchive) << (Vision::GetScriptManager()!=NULL ? Vision::GetScriptManager()->GetThinkInterval() : 0.0f);

  // #3 : current sky object
  IVSky *pSky = (m_eExportFlags & VExport_Sky) ? Vision::World.GetActiveSky() : NULL;
  (*m_pArchive) << pSky;

  // #4 : current renderer
  IVRendererNode *pRenderer = (m_eExportFlags & VExport_RendererNode) ? Vision::Renderer.GetRendererNode(0) : NULL;
  (*m_pArchive) << pRenderer;

  // #5 : time of day node
  IVTimeOfDay *pToD = (m_eExportFlags & VExport_TimeOfDay) ? Vision::Renderer.GetTimeOfDayHandler() : NULL;
  if (pRenderer==NULL) // no time of day when renderer node is not exported (Wii)
    pToD = NULL; 
  (*m_pArchive) << pToD;

  // #15: Serialize global LOD Hysteresis settings
  VLODHysteresisManager::SerializeX(*m_pArchive);

  return m_pArchive;
}

VExportShapesArchive* VSceneExporter::StartVZoneExport(IVFileOutStream *pOut, bool bCloseFile, VZoneInfo_t &info)
{
  m_eType = EXPORTTYPE_VZONE;
  m_pOut = pOut;
  m_bCloseFile = bCloseFile;
  VisVisibilityZone_cl::SetVisibilityZoneSerializationMode(VIS_SERIALIZE_AS_PROXIES);
  PrepareShapesArchive(false);

  int iLocalVersion = VZONE_CURRENT_FILE_VERSION;
  (*m_pArchive) << Vision::GetArchiveVersion() << iLocalVersion;

  // VZONE_FILE_VERSION_10: Zone pivot:
  (*m_pArchive) << info.m_dPivot[0];
  (*m_pArchive) << info.m_dPivot[1];
  (*m_pArchive) << info.m_dPivot[2];

  m_pShapesOut->ReservePatchedIntegers(*m_pArchive,4); // used to patch memory stream later

  return m_pArchive;
}

VExportShapesArchive* VSceneExporter::StartVPrefabExport(IVFileOutStream *pOut, bool bCloseFile)
{
  m_eType = EXPORTTYPE_VPREFAB;
  m_pOut = pOut;
  m_bCloseFile = bCloseFile;
  VisVisibilityZone_cl::SetVisibilityZoneSerializationMode(VIS_SERIALIZE_NONE); // no visibility connection whatsoever for prefabs
  PrepareShapesArchive(false);

  int iLocalVersion = VPREFAB_BINARY_VERSION_CURRENT;
  (*m_pArchive) << Vision::GetArchiveVersion() << iLocalVersion;

  m_pShapesOut->ReservePatchedIntegers(*m_pArchive, 4); // patched in the end

  return m_pArchive;
}


void VSceneExporter::EndExport()
{
  m_pArchive->Close(); // flush archive

  switch(m_eType)
  {
    case EXPORTTYPE_VSCENE:WriteVSceneFile();break;
    case EXPORTTYPE_VZONE:WriteVZoneFile();break;
    case EXPORTTYPE_VPREFAB:WriteVPrefabFile();break;
    default:
      VASSERT(!"Unsupported export format");
  }

  // clean up
  m_pShapesOut->Close();
  m_pShapesOut = NULL;
  V_SAFE_DELETE(m_pArchive);
  m_spMemStream = NULL;
  VisVisibilityZone_cl::SetVisibilityZoneSerializationMode(VIS_SERIALIZE_AS_PROXIES); // reset

  if (m_bCloseFile)
    m_pOut->Close();
  m_pOut = NULL;
  m_iEmbeddedZoneCount = 0;
  V_SAFE_DELETE_ARRAY(m_pEmbeddedZones);
}


void VSceneExporter::PrepareShapesArchive(bool bWithRangeInfo)
{
  // shapes go into a memory stream (not pOut directly)
  m_pShapesOut = new VShapesMemoryOutStream();
  m_spMemStream = m_pShapesOut->GetStream();
  m_pArchive = new VExportShapesArchive(m_pShapesOut);
  m_pArchive->m_bHasPerObjectRangeInfo = bWithRangeInfo;
  m_pArchive->SetLoadingVersion(Vision::GetArchiveVersion());
}



void VSceneExporter::WriteVSceneFile()
{
  VSceneLoader file;
  file.Create(m_pOut);

  // scene chunk
  file.StartChunk('SCNE');
    file.WriteInt(SCENE_MAGIG_NUMBER);
    file.WriteInt(SCENE_FILE_CURRENT_VERSION);
    file.WriteInt(m_eExportFlags);
    file.WriteInt(0); // reserved
  file.EndChunk();

  if (m_eExportFlags&VExport_Plugins)
  {
    // also add the ones with VEXPORTREFERENCE_ALWAYS/VEXPORTREFERENCE_USER_SELECT behavior
    int iCount = Vision::Plugins.GetRegisteredPluginCount();
    for (int i=0;i<iCount;i++)
    {
      IVisPlugin_cl* pPlugin = Vision::Plugins.GetRegisteredPlugin(i);
      if (pPlugin==NULL)
        continue;
      IVisPlugin_cl::VExportReferenceBehavior bh = pPlugin->GetExportBehavior();
      if (bh==IVisPlugin_cl::VEXPORTREFERENCE_ALWAYS || (bh==IVisPlugin_cl::VEXPORTREFERENCE_USER_SELECT && pPlugin->m_bUserSelected))
        m_pArchive->AddPlugin(pPlugin);
    }

    file.StartChunk('EPLG');
      VStrList &plugins(m_pArchive->m_UniquePluginNames);
      iCount = plugins.GetLength();
      file.WriteInt(iCount);
      for (int i=0;i<iCount;i++)
        file.WriteString(plugins.GetString(i));
    file.EndChunk();
  }

  // v3d chunk, currently contains some scene-wide settings
  file.StartChunk('_V3D');

  file.WriteInt(6); // version for this chunk
  hkvVec3d refPos;
  Vision::World.GetCoordinateSystem()->GetSceneReferencePosition(refPos);

    file.WriteFloat(Vision::World.GetGlobalUnitScaling());

    file.Write(refPos.data,sizeof(double)*3,"qqq"); // version 6
    VColorRef iDefaultLightColor = Vision::Renderer.GetDefaultLightingColor();
    int iLightmapEquation = Vision::Renderer.GetLightingMode();
    if (!Vision::RenderLoopHelper.HasLightmaps())
      iLightmapEquation = VIS_LIGHTING_NOLIGHTMAPS;
    file.WriteString(""); // deprecated
    file.WriteColorRef(iDefaultLightColor);
    file.WriteInt(iLightmapEquation);

    VSRGBMode eSRGBMode = Vision::Renderer.GetSRGBMode();
    file.WriteInt((int)eSRGBMode);

    // save a reference to the current light grid
    VLightGrid_cl* pGrid = Vision::RenderLoopHelper.GetLightGrid();
    if (pGrid != NULL && pGrid->GetFilename())
    {
      /*const char* szFilename = pGrid->GetFilename();
      char szExtName[FS_MAX_PATH];
      if (m_pPlatformProfile->m_bSimpleLightGrid)
        VFileHelper::AddExtension(szExtName, szFilename, "vslg");
      else
        VFileHelper::AddExtension(szExtName, szFilename, "vlg");*/

      file.WriteString(pGrid->GetFilename());
    }
    else
      file.WriteString(NULL);

    //save shader provider
    IVisShaderProvider_cl* pShaderProvider = Vision::GetApplication()->GetShaderProvider();
    if (pShaderProvider != NULL)
      file.WriteString(pShaderProvider->GetTypeId()->m_lpszClassName);
    else
      file.WriteString(NULL);
  file.EndChunk();

  // serialize fog config
  if (m_eExportFlags&VExport_Fog)
  {
    file.StartChunk('_FOG');

    VFogParameters fog = Vision::World.GetFogParameters();
    
    file.WriteInt(VSCENE_FOG_CURRENT_VERSION); // version
    file.WriteInt(fog.depthMode);
    file.WriteFloat(fog.fDepthStart);
    file.WriteFloat(fog.fDepthEnd);
    file.WriteColorRef(fog.iDepthColor);
    file.WriteInt(fog.heightFogMode);
    file.WriteFloat(fog.fHeightFogDensity);
    file.WriteColorRef(fog.iHeightFogColor);
    file.WriteFloat(fog.fHeightFogHalfDensityHeight);
    file.WriteFloat(fog.fHeightFogOffset);
    file.WriteFloat(fog.fHeightFogStart);
    file.WriteFloat(fog.fHeightFogEnd);
    file.Writebool(fog.bHeightFogAddScattering);
    file.Writebool(fog.bMaskSky);

    file.EndChunk();
  }

  // view settings
  if (m_eExportFlags&VExport_View)
  {
    file.StartChunk('VIEW'); 
      // clip distances
      IVRendererNode* pNode = Vision::Renderer.GetRendererNode(0);
      VASSERT(pNode != NULL);
      file.WriteFloat(pNode->GetViewProperties()->getNear());
      file.WriteFloat(pNode->GetViewProperties()->getFar());

      // initial camera position (scene version 2)
      hkvVec3 vCamPos = Vision::Camera.GetCurrentCameraPosition();
      hkvMat3 camRot;
      Vision::Camera.GetCurrentCameraRotation(camRot);
      file.Write(vCamPos.data,3*sizeof(float),"3f");

      for (int i = 0; i < 9; ++i)
      {
        float f = camRot.m_ElementsCM[(i/3)+(i%3)*3];
        file.Write(&f,sizeof(float),"1f");
      }

      // Get field of view
      float fovX = 90.0f;
      float fovY = 0.0f;
      Vision::Contexts.GetMainRenderContext()->GetFOV(fovX, fovY);

      file.WriteFloat(fovX);
      file.WriteInt(0); // reserved
      file.WriteInt(0); // reserved
    file.EndChunk();
  }

  if (m_eExportFlags&VExport_ZoneRefs)
  {
    file.StartChunk('ZONE');
      file.WriteInt(1); // local version
      file.WriteInt(m_iEmbeddedZoneCount);
      for (int i=0;i<m_iEmbeddedZoneCount;i++)
      {
        VZoneInfo_t &info(m_pEmbeddedZones[i]);
        file.WriteString(info.m_sZoneName);
        file.WriteString(info.m_sLightgridFilename);

        // zone extents
        file.Write(info.m_ZoneBox.m_vMin.data,sizeof(float)*3,"fff");
        file.Write(info.m_ZoneBox.m_vMax.data,sizeof(float)*3,"fff");
        file.WriteFloat(info.m_fLoadedRadius);
        file.WriteFloat(info.m_fCacheInMargin);
        file.WriteFloat(info.m_fCacheOutMargin);
        file.Write(info.m_dPivot, sizeof(info.m_dPivot), "ddd"); // version 1
        file.WriteInt(0); // reserved
      }
    file.EndChunk();
  }

  // save out before 'SHPS' chunk custom chunks 
  {
    VCustomSceneChunkDataObject data(file, 0, &VSceneLoader::OnCustomChunkBeforeShapesSerialization);
    VSceneLoader::OnCustomChunkBeforeShapesSerialization.TriggerCallbacks(&data);
  }

  // start the shapes chunk which embeds a binary archive
  file.StartChunk('SHPS');  
    char *pMemData = (char *)m_spMemStream->GetDataPtr();
    int *pFirstEntry = m_pShapesOut->GetPatchedIntegerPtr(4);
    pFirstEntry[0] = m_pArchive->m_iObjectCount;
    pFirstEntry[1] = m_pArchive->m_iNonNullCount;
    pFirstEntry[2] = m_pArchive->m_iRootObjectCount;
    pFirstEntry[3] = 0; // reserved

    // now put the memory stream into the chunk file
    int iBytes = m_spMemStream->GetSize();

    double fScaleRoot = m_pArchive->m_iSumNewBytes>0 ? 100.0/(double)m_pArchive->m_iSumNewBytes : 0.0;
    float fRootPercentage = 0.f;
    // Patch the stream with actual per-object progress range
    for (int i=0;i<m_pArchive->m_iObjectsWithRangeCount;i++)
    {
      int iPatchPos = m_pArchive->m_iObjectPatchPos[i];
      if (iPatchPos==0)
        continue;

      if (m_pArchive->m_iObjectRecursionDepth[i]==0) // we haven't set this range value
      {
        float &fPercentage(m_pArchive->m_iObjectRangeLen[i]);
        VASSERT(fPercentage<0.f); // yet undefined
        fPercentage = (float)((double)m_pArchive->m_iObjectsBytesCount[i] * fScaleRoot);
        fRootPercentage += fPercentage;
      }

      float *pProgressValue = (float *)&pMemData[iPatchPos];
      VASSERT(pProgressValue[0] == 11.f); // validate magic values set earlier
      pProgressValue[0] = m_pArchive->m_iObjectRangeLen[i];
      VASSERT(pProgressValue[0]>=0.f && pProgressValue[0]<=100.f);
    }
    VASSERT(hkvMath::Abs (fRootPercentage-100.f)<0.05f);
    // now write the memory stream to file:
    file.Write(pMemData,iBytes);


  file.EndChunk(); // 'SHPS'

  // save out custom chunks
  {
    VCustomSceneChunkDataObject data(file, 0, &VSceneLoader::OnCustomChunkSerialization);
    VSceneLoader::OnCustomChunkSerialization.TriggerCallbacks(&data);
  }

  file.Close();
}


void VSceneExporter::WriteVZoneFile()
{
  int *pHeader = m_pShapesOut->GetPatchedIntegerPtr(4);
  pHeader[0] = m_pArchive->m_iObjectCount;
  pHeader[1] = m_pArchive->m_iNonNullCount;
  pHeader[2] = m_pArchive->m_iRootObjectCount;
  pHeader[3] = 0; // reserved

  m_spMemStream->CopyToStream(m_pOut); // write everything to file
}

void VSceneExporter::WriteVPrefabFile()
{
  int *pHeader = m_pShapesOut->GetPatchedIntegerPtr(4);
  pHeader[0] = m_pArchive->m_iObjectCount;
  pHeader[1] = m_pArchive->m_iNonNullCount;
  pHeader[2] = m_pArchive->m_iRootObjectCount;
  pHeader[3] = 0; // reserved

  m_spMemStream->CopyToStream(m_pOut); // write everything to file
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
