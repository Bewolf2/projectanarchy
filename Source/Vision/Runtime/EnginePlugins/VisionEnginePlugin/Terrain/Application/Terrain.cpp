/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainVisibilityCollector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationEntityModel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Math/LargePosition.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenerator.hpp>

// only used in rare places:
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>


#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


// helper macro
#define FOREACH_SECTOR \
  for (int sy=0;sy<m_Config.m_iSectorCount[1];sy++)\
    for (int sx=0;sx<m_Config.m_iSectorCount[0];sx++)\
    {\
      VTerrainSector *pSector = m_SectorManager.GetSector(sx,sy);

                        
#define FOREACH_TERRAIN \
   for (int i=0;i<GetResourceCount();i++)\
   {\
     VTerrain *pTerrain = (VTerrain *)GetResourceByIndex(i);\
     if (!pTerrain) continue;\


VTerrainManager VTerrainManager::g_TerrainManager;
VisCallback_cl VTerrainManager::OnSectorLoaded;
VisCallback_cl VTerrainManager::OnSectorUnloaded;
VisCallback_cl VTerrainManager::OnSectorCreated;


VTerrainManager::VTerrainManager() : VisResourceManager_cl("TerrainManager",VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER, 0)
{
  m_pCustomBaker = NULL;
}

void VTerrainManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    // only purge terrains while unloading the level
    FOREACH_TERRAIN
      pTerrain->RemoveResourceFlag(VRESOURCEFLAG_NOPURGING);
    }
    PurgeUnusedResources();
    VTerrainDecorationModelManager::GlobalManager().PurgeUnusedResources();
    FOREACH_TERRAIN
      pTerrain->SetResourceFlag(VRESOURCEFLAG_NOPURGING);
    }
    VEditableTerrain::m_MeshPaintingComponents.Clear();

    VASSERT(VBillboardDecorationSectorInstance::g_Instances.Count()==0); // all should be disposed already
    VBillboardDecorationSectorInstance::g_Instances.Clear();
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    FOREACH_TERRAIN
      pTerrain->SetDefaultShaderEffect(pTerrain->SectorManager().m_spTerrainEffect);
    }
  }
}


void VTerrainManager::InitOneTime()
{
  VTerrainSectorManager::InitProfiling();
  Vision::ResourceSystem.RegisterResourceManager(this, VColorRef(100,40,0));

  VTerrainDecorationModelManager::GlobalManager().OneTimeInit();
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnReassignShaders += this;

  // entity model factory class (for .model vegetation types) is always there
  VTerrainDecorationModelManager::GlobalManager().RegisterFactory(&VTerrainDecorationEntityModel::g_EntityModelFactory);
}

void VTerrainManager::DeInitOneTime()
{
  VTerrainSectorManager::PROFILING_VISIBILITY_OVERALL = 0; // profiling De-init
  Vision::ResourceSystem.UnregisterResourceManager(this);
  VTerrainDecorationModelManager::GlobalManager().OneTimeDeInit();
  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::Callbacks.OnReassignShaders -= this;
}


VTerrainManager& VTerrainManager::GlobalManager()
{
  return g_TerrainManager;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Terrain class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

VTerrain::VTerrain() : VManagedResource(&VTerrainManager::GlobalManager()), m_SectorManager(m_Config)
{
  m_spSceneLightmaps = NULL;
  m_iDecorationRenderHookConstant = VRH_PRE_OCCLUSION_TESTS;
  m_SectorManager.m_pTerrain = this;
  SetResourceFlag(VRESOURCEFLAG_AUTODELETE|VRESOURCEFLAG_NOPURGING);
  m_bAddedToSceneManager = false;
  m_bVisibleInVisZones = false;
  m_bFastUpdate = false;
  Vision::ResourceSystem.RegisterResourceManager(&m_SectorManager, VColorRef(40,100,0));
  Vision::ResourceSystem.RegisterResourceManager(&m_SectorManager.CollisionMeshManager(), VColorRef(0,0,100));

  Vision::Callbacks.OnRenderHook += this;
  VShadowMapGenerator::OnRenderShadowMap += this;
  IVisVisibilityCollector_cl::OnVisibilityCollectorCreated += this;
  Vision::Callbacks.OnVisibilityPerformed += this;
}


VTerrain::~VTerrain()
{
  FreeTerrain();

  // before deregistering the resource manager, update the res viewer panel inside the editor, so it shows the empty lists [HS#6152]
  if (Vision::Editor.IsInEditor())
    Vision::ResourceSystem.UpdateResourceViewer();

  Vision::ResourceSystem.UnregisterResourceManager(&m_SectorManager);
  Vision::ResourceSystem.UnregisterResourceManager(&m_SectorManager.CollisionMeshManager());
  Vision::Callbacks.OnRenderHook -= this;
  VShadowMapGenerator::OnRenderShadowMap -= this;
  IVisVisibilityCollector_cl::OnVisibilityCollectorCreated -= this;
  Vision::Callbacks.OnVisibilityPerformed -= this;
}



void VTerrain::FreeTerrain()
{
  V_SAFE_DELETE_ARRAY(m_spSceneLightmaps);
  RemoveFromSceneManager();
  m_SectorManager.DeInitTerrain();
}

void VTerrain::EnsureCollectorComponentStatus(bool bAdd)
{
  VisRenderContext_cl *pContexts[1024];
  const int iCount = Vision::Renderer.GetAllRenderContexts(pContexts,1024);
  for (int i=0;i<iCount;i++)
    if (bAdd)
      EnsureCollectorComponentAdded(pContexts[i]->GetVisibilityCollector());
    else
      EnsureCollectorComponentRemoved(pContexts[i]->GetVisibilityCollector());
}


void VTerrain::AddToSceneManager()
{
  if (m_bAddedToSceneManager)
    return;
  IVisSceneManager_cl *pSM = Vision::GetSceneManager();

  // first make sure every sector has it svisibility zone
  FOREACH_SECTOR
    if (!pSector->m_spSectorZone)
      pSector->m_spSectorZone = new VSectorVisibilityZone(pSector);
  }

  // then connect them
  FOREACH_SECTOR
    pSector->AssignVisibility(true);
  }

  m_bAddedToSceneManager = true;

  EnsureCollectorComponentStatus(true);
}

void VTerrain::RemoveFromSceneManager()
{
  if (!m_bAddedToSceneManager)
    return;
  for (int i=0;i<m_SectorManager.m_iSectorCount;i++)
    m_SectorManager.m_pSector[i]->AssignVisibility(false);
  m_bAddedToSceneManager = false;

  EnsureCollectorComponentStatus(false);
}


BOOL VTerrain::Unload()
{
  FreeTerrain();
  return TRUE;
}


static const int TERRAIN_FILE_VERSION_0 = 0;
static const int TERRAIN_FILE_VERSION_CURRENT = TERRAIN_FILE_VERSION_0;

bool VTerrain::LoadFromFile(const char *szFilename)
{
  EnsureUnloaded();
  if (!szFilename)
    szFilename = m_Config.m_sTerrainFolder;
  if (!szFilename || !szFilename[0])
    return false;
  SetFilename(szFilename);
  EnsureLoaded();
  return IsLoaded()==TRUE;
}

BOOL VTerrain::Reload()
{
  char szPathname[FS_MAX_PATH];
  VFileHelper::CombineDirAndFile(szPathname,GetFilename(),"Config.vtc");
  VChunkFile file;
  if (!file.Open(szPathname,Vision::File.GetManager()))
    return FALSE;


  const char *szAbsProjDir = NULL;
  char szAbsSceneDir[FS_MAX_PATH];
  VFileHelper::GetFileDir(file.GetFilename(),szAbsSceneDir);
  if (EndsWithI(szAbsSceneDir,GetFilename()))
  {
    // remove the scene subdirectory
    int i1 = (int)strlen(szAbsSceneDir);
    int i2 = (int)strlen(GetFilename());
    if (i1>i2)
    {
      szAbsSceneDir[i1-i2-1] = 0;
      szAbsProjDir = szAbsSceneDir;
    }
  }

  CHUNKIDTYPE chunkID;
  int iFileVersion = TERRAIN_FILE_VERSION_0;
  while (!file.IsEOF())
  {
    if (!file.OpenChunk(&chunkID,NULL))
      break;
    switch (chunkID)
    {
      case 'TVRS':
        file.ReadInt(iFileVersion);
        VASSERT(iFileVersion<=TERRAIN_FILE_VERSION_CURRENT);
        break;
      case 'CONF':
        m_Config.ChunkFileSerialize(file);
        CreateTerrain(NULL,szAbsProjDir,false); // do not save
        break;
      case 'GEOM':
        m_SectorManager.LoadGeometryChunk(file);
        break;
      default:
        VASSERT(!"unsupported chunk ID");
    }
    file.EndChunk();
  }

  file.Close();
  m_SectorManager.EnsureSharedMeshesCreated();

  return TRUE;
}


void VTerrain::AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
{
  VManagedResource::AccumulateMemoryFootprint(iUniqueSys,iUniqueGPU,iDependentSys,iDependentGPU);

  // the terrain itself only depends on the shared mesh buffer. The rest is in the sectors
  if (m_SectorManager.m_spTerrainIndexBuffer)
    m_SectorManager.m_spTerrainIndexBuffer->IncOverallMemSizes(iDependentSys,iDependentGPU);

  int iSectorCount = m_Config.m_iSectorCount[0] * m_Config.m_iSectorCount[1];
  iUniqueSys += sizeof(VTerrainSector)*iSectorCount;
  iUniqueSys += sizeof(m_SectorManager);
}

#ifdef SUPPORTS_SNAPSHOT_CREATION



void VTerrain::GetDependencies(VResourceSnapshot &snapshot)
{
  VManagedResource::GetDependencies(snapshot);

  char szPathname[FS_MAX_PATH];
  VFileHelper::CombineDirAndFile(szPathname,GetFilename(),"Config.vtc");
  IVFileInStream *pIn = Vision::File.Open(szPathname);
  if (pIn)
  {
    snapshot.AddFileDependency(this, szPathname, pIn->GetSize());
    pIn->Close();
  }


  if (snapshot.GetUsageType()&VResourceSnapshot::VSNAPSHOTTYPE_DEPENDENT_MEMORY)
  {
    if (m_SectorManager.m_spTerrainIndexBuffer)
      m_SectorManager.m_spTerrainIndexBuffer->GetDependencies(snapshot);
  }
}

#endif


/*
Called from:
-VTerrain::CreateTerrain
-VEditableTerrain::ChangeSectorCount
-EngineInstanceTerrain::SaveToFile
-EngineInstanceTerrain::OnExport
*/
bool VTerrain::SaveToFile(const char *szFolder)
{
  bool bOK = SaveConfigFile(szFolder);  //Save the config
  m_SectorManager.SaveAllSectors();  //Save the sectors
  
  return bOK;
}

bool VTerrain::SaveConfigFile(const char *szFolder)
{
  if (!szFolder)
    szFolder = GetFilename();
  if (!szFolder || !szFolder[0])
    return false;

 //NB. szFolder can be Absolute or Project Relative (depending on what was passed in or how it was loaded)

  VStaticString<FS_MAX_PATH> szConfigFile;
  VFileHelper::CombineDirAndFile(szConfigFile,szFolder,"Config.vtc");

  m_Config.RCSPerformAction(szConfigFile, RCS_EDIT); //make sure we can write to it

  // write out terrain file
  VChunkFile file;
  if (!file.Create(szConfigFile,Vision::File.GetManager()))
    return false;

  // file version
  file.StartChunk('TVRS');
    file.WriteInt(TERRAIN_FILE_VERSION_CURRENT);
  file.EndChunk();

  // write out terrain config
  file.StartChunk('CONF');
    m_Config.ChunkFileSerialize(file);
  file.EndChunk();

  // write shared geometry data (meshes)
  file.StartChunk('GEOM');
    m_SectorManager.SaveGeometryChunk(file);
  file.EndChunk();
  
  file.Close();
  if (file.IsInErrorState())
    return false;
  m_Config.RCSPerformAction(szConfigFile, RCS_ADD);
  
  return true;
}



void VTerrain::CreateTerrain(VTerrainConfig *pConfig, const char *szAbsProjectDir, bool bSave)
{
  // copy config to current
  if (pConfig)
    m_Config = *pConfig; // copy the config

  const char *szFilename = m_Config.m_sTerrainFolder;
  SetFilename(szFilename);
  FlagAsLoaded();

  FreeTerrain();
  if (!m_Config.IsValid())
  {
    VASSERT(!"Terrain config is not valid");
    return;
  }

  if (bSave)
  {
    VASSERT(szAbsProjectDir);
    VASSERT(VPathHelper::IsAbsolutePath(szAbsProjectDir));

    // create all the directories
    VASSERT( !m_Config.m_sAbsProjectDir.IsEmpty() );
    m_Config.MakeRelevantDirectories(szAbsProjectDir, false); //normal files
    m_Config.MakeRelevantDirectories(szAbsProjectDir, true);  //temp files  
  }

  // prepare directory
  VASSERT(szAbsProjectDir==NULL || VFileHelper::ExistsDir(szAbsProjectDir));
  m_SectorManager.InitTerrain(/*szAbsProjectDir*/);

  if (bSave)
    SaveToFile(szFilename);

//  AddToSceneManager(); // this should be done from outside now
  m_SectorManager.SetAllowPurging(true); // confirm new purge time
  OnNewTerrainCreated();
  UpdateMemoryFootprint();
}



V_IMPLEMENT_SERIAL( VTerrain, VisTypedEngineObject_cl, 0, &g_VisionEngineModule );
void VTerrain::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    int iVersion;
    ar >> iVersion; VASSERT_MSG(iVersion>=0 && iVersion<=8, "Incompatible terrain version number");
    if (iVersion>=7)
      VisTypedEngineObject_cl::Serialize(ar); // components, ID
    else
      ar >> m_iUniqueID64;

    ar >> m_SectorManager.m_iVisibleBitmask;
    if (iVersion>=5)
      ar >> m_bVisibleInVisZones;
    if (iVersion>=8)
    {
      bool bState;
      ar >> bState;
      SetFastUpdate(bState);
    }

    char szPath[FS_MAX_PATH];
    ar.ReadStringBinary(szPath,FS_MAX_PATH);
    if (!LoadFromFile(szPath)) // loads the config
    {
      Vision::Error.FatalError("Failed to load Config.vtc which is mandatory for the terrain");
    }
      

    VisEffectConfig_cl fxConfig;
    fxConfig.SerializeX(ar);
    if (iVersion>=2)
    {
      VTerrainDirectionalLightConfig &dir(m_SectorManager.m_DirLightConfig.BeginUpdate());
        ar >> dir.m_vLightDirection;
        ar >> dir.m_vDirectionalLightColor;
        ar >> dir.m_vAmbientColor;
      m_SectorManager.m_DirLightConfig.EndUpdate();
    }
    SetDefaultShaderEffect(fxConfig.GetEffect());

    m_bAddedToSceneManager = false;
    // read sector information, basically just the viszone as the rest streams in for every sector
    int iCountX,iCountY;
    ar >> iCountX >> iCountY;
    VASSERT_MSG(iCountX==m_Config.m_iSectorCount[0] && iCountY==m_Config.m_iSectorCount[1],"Please re-export terrain");
    FOREACH_SECTOR
      VSectorVisibilityZone *pZone = (VSectorVisibilityZone *)ar.ReadObject(V_RUNTIME_CLASS(VSectorVisibilityZone));
      pSector->m_spSectorZone = pZone;
      pZone->m_pTerrain = this;
      pZone->m_pSector = pSector;
    }
    AddToSceneManager();

    float fLOD = 1.f;
    float fMirrorBias = 1.f;
    if (iVersion>=1)
    {
      ar >> fLOD;
      ar >> fMirrorBias;
    }
    SetLODScaling(fLOD);
    SetLODMirrorBias(fMirrorBias);
    // version 3
    bool bSceneLightmaps = false;
    if (iVersion>=3)
      ar >> bSceneLightmaps;
    V_SAFE_DELETE_ARRAY(m_spSceneLightmaps);
    if (bSceneLightmaps)
    {
      const int iTexCount = m_Config.m_iSectorCount[0]*m_Config.m_iSectorCount[1]*4;
      m_spSceneLightmaps = new VTextureObjectPtr[iTexCount];
      for (int i=0;i<iTexCount;i++)
      {
        VMemoryTempBuffer<FS_MAX_PATH> strbuffer;
        const char *szFilename = strbuffer.ReadStringBinary(ar);
        if (szFilename && szFilename[0])
          m_spSceneLightmaps[i] = Vision::TextureManager.Load2DTexture(szFilename, VTM_FLAG_DEFAULT_MIPMAPPED|VTM_FLAG_NO_DOWNSCALE|VTM_FLAG_NO_UPLOAD);
      }
      if (iVersion>=4)
      {
        FOREACH_SECTOR
          hkvVec4 vLM(false);
          ar >> vLM;
          pSector->SetLightmapScaleOffset(vLM);
        }
      }
    }

    // we dont have to call ConnectNeighborZones(true) here because it happens in AddToSceneManager already
  } else
  {
    FOREACH_SECTOR
      pSector->ConnectNeighborZones(false); // version 6 does not save the connections to neighbor zones to avoid recursion
    }

    ar << (int)8; // version
    VisTypedEngineObject_cl::Serialize(ar); // components (version 7)

    ar << m_SectorManager.m_iVisibleBitmask;
    ar << m_bVisibleInVisZones; // version 5
    ar << GetFastUpdate(); // version 8

    ar.WriteStringBinary(GetFilename());

    // write out default effect
    VisEffectConfig_cl fxConfig;
    VCompiledEffect *pFX = m_SectorManager.GetTerrainEffect();
    if (pFX != NULL && pFX->m_pSourceEffect && pFX->m_pSourceEffect->m_pOwnerLib==NULL)
      pFX->m_pSourceEffect->m_pOwnerLib = m_SectorManager.m_spTerrainEffectLib; // fixup
    fxConfig.AddEffect(pFX);
    fxConfig.SerializeX(ar);

    VTerrainDirectionalLightConfig &dir(m_SectorManager.m_DirLightConfig.BeginUpdate());
      ar << dir.m_vLightDirection;
      ar << dir.m_vDirectionalLightColor;
      ar << dir.m_vAmbientColor;
    m_SectorManager.m_DirLightConfig.EndUpdate();

    // sector information
    ar << m_Config.m_iSectorCount[0] << m_Config.m_iSectorCount[1];
    FOREACH_SECTOR
      ar.WriteObject(pSector->m_spSectorZone);
    }

    ar << GetLODScaling();
    ar << GetLODMirrorBias();

    // version 3
    bool bSceneLightmaps = m_spSceneLightmaps!=NULL;
    ar << bSceneLightmaps;
    if (bSceneLightmaps)
    {
      const int iTexCount = m_Config.m_iSectorCount[0]*m_Config.m_iSectorCount[1]*4;
      for (int i=0;i<iTexCount;i++)
      {
        if (m_spSceneLightmaps[i]==NULL)
          ar.WriteStringBinary(NULL);
        else
          ar.WriteStringBinary(m_spSceneLightmaps[i]->GetFilename());
      }
      // lightmap scale/offset
      FOREACH_SECTOR
        hkvVec4 vLM = pSector->GetLightmapScaleOffset();
        ar << vLM;
      }
    }

    FOREACH_SECTOR
      pSector->ConnectNeighborZones(true); // version 6 does not save the connections to neighbor zones to avoid recursion
    }

  }
}


void VTerrain::SetDefaultShaderEffect(VCompiledEffect *pFX)
{
  if (pFX!=NULL)
  {
    if (pFX->m_pSourceEffect!=NULL)
      m_SectorManager.m_spTerrainEffectLib = pFX->m_pSourceEffect->m_pOwnerLib;
    else
    {
      VASSERT(pFX->m_pSourceEffect!=NULL);
      m_SectorManager.m_spTerrainEffectLib = NULL;
    }

    m_SectorManager.m_spTerrainEffect = pFX;
  }
  else
  {
    // un-cache
    m_SectorManager.m_spTerrainEffect = NULL;
    m_SectorManager.GetTerrainEffect(); // load default effect
    VASSERT(m_SectorManager.m_spTerrainEffect!=NULL);
  }

  FOREACH_SECTOR
    if (pSector->IsLoaded())
      pSector->OnDefaultShaderEffectChanged();
  }
  CheckUsesDirectionalLight();
}

VTerrainVisibilityCollectorComponent* VTerrain::FindTerrainComponent(IVisVisibilityCollector_cl *pCollector)
{
  VASSERT(pCollector);
  for (int i=0;i<pCollector->Components().Count();i++)
  {
    IVObjectComponent *pComponent = pCollector->Components().GetAt(i);
    if (!pComponent->IsOfType(V_RUNTIME_CLASS(VTerrainVisibilityCollectorComponent)))
      continue;
    if (((VTerrainVisibilityCollectorComponent *)pComponent)->m_pTerrain==this)
      return (VTerrainVisibilityCollectorComponent *)pComponent;
  }
  return NULL;
}

void VTerrain::EnsureCollectorComponentAdded(IVisVisibilityCollector_cl *pCollector)
{
  if (!pCollector)
    return;

  for (int i=0;i<pCollector->Components().Count();i++)
  {
    IVObjectComponent *pComponent = pCollector->Components().GetAt(i);
    if (!pComponent->IsOfType(V_RUNTIME_CLASS(VTerrainVisibilityCollectorComponent)))
      continue;
    if (((VTerrainVisibilityCollectorComponent *)pComponent)->m_pTerrain==this)
      return; // already in there
  }

  pCollector->AddComponent(new VTerrainVisibilityCollectorComponent(pCollector, this));
}

void VTerrain::EnsureCollectorComponentRemoved(IVisVisibilityCollector_cl *pCollector)
{
  if (!pCollector)
    return;

  for (int i=0;i<pCollector->Components().Count();i++)
  {
    IVObjectComponent *pComponent = pCollector->Components().GetAt(i);
    if (!pComponent->IsOfType(V_RUNTIME_CLASS(VTerrainVisibilityCollectorComponent)))
      continue;
    if (((VTerrainVisibilityCollectorComponent *)pComponent)->m_pTerrain==this)
    {
      pCollector->RemoveComponent(pComponent);
      return;
    }
  }
}


void VTerrain::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnRenderHook)
  {
    int iEntryConst = ((VisRenderHookDataObject_cl *)pData)->m_iEntryConst;

    VASSERT(Vision::Contexts.GetCurrentContext() != NULL);
    if ((Vision::Contexts.GetCurrentContext()->GetUsageHint() & VIS_CONTEXTUSAGEFLAG_INFRARED) != 0)
    {
      if (iEntryConst == VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY)
      {
        INSERT_PERF_MARKER_SCOPE("Terrain Vegetation IR Rendering PrePass (VTerrain::OnHandleCallback)");
        RenderTerrain(pData, IVTerrainDecorationModel::RENDER_MODE_IR_PREPASS);
      }
      else if (iEntryConst == VRH_PRE_TRANSPARENT_PASS_GEOMETRY)
      {
        INSERT_PERF_MARKER_SCOPE("Terrain Vegetation IR Rendering MainPass (VTerrain::OnHandleCallback)");
        RenderTerrain(pData, IVTerrainDecorationModel::RENDER_MODE_IR_MAINPASS);
      }
    }
    else
    {
    if (iEntryConst==m_iDecorationRenderHookConstant)
    {
      INSERT_PERF_MARKER_SCOPE("Terrain Vegetation Rendering (VTerrain::OnHandleCallback)");
      RenderTerrain(pData);
    }
    }

    return;
  }

  if (pData->m_pSender==&VShadowMapGenerator::OnRenderShadowMap)
  {
    INSERT_PERF_MARKER_SCOPE("Terrain Vegetation Shadowmap Rendering (VTerrain::OnHandleCallback)");
    RenderTerrain(pData);
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnVisibilityPerformed)
  {
    // the m_pRenderCollector member is only valid while rendering
    if (Vision::Contexts.GetCurrentContext()->GetVisibilityCollector())
      m_SectorManager.m_pRenderCollector = FindTerrainComponent(Vision::Contexts.GetCurrentContext()->GetVisibilityCollector());
    else
      m_SectorManager.m_pRenderCollector = NULL;

    return;
  }

  if (pData->m_pSender==&IVisVisibilityCollector_cl::OnVisibilityCollectorCreated)
  {
    VisVisibilityCollectorDataObject_cl *pVCData = (VisVisibilityCollectorDataObject_cl *)pData;
    EnsureCollectorComponentAdded(pVCData->m_pCollector);

    return;
  }
}

void VTerrain::SetLightInfluenceBitmask(unsigned short iMask)
{
  // bitmask is part of the geometry base class
  FOREACH_SECTOR
    pSector->SetLightInfluenceBitmask(iMask);
  }
}



void VTerrain::RenderTerrain(IVisCallbackDataObject_cl* pCallBackData , IVTerrainDecorationModel::RenderMode_e eRenderMode)
{
  if (!Vision::Contexts.GetCurrentContext()->GetVisibilityCollector())
    return;

  if (!m_SectorManager.m_pRenderCollector)
    return;

  // render decoration etc.
  m_SectorManager.RenderReplacementMeshes(*m_SectorManager.m_pRenderCollector, pCallBackData, eRenderMode); // once this used, m_iDecorationRenderHookConstant must be separated from this call
  m_SectorManager.RenderDecoration(*m_SectorManager.m_pRenderCollector, pCallBackData, eRenderMode);

#ifdef PROFILING
  if ((Vision::Profiling.GetDebugRenderFlags() & DEBUGRENDERFLAG_OBJECT_VISIBILITYZONES) != 0)
    DebugRender();
#endif
}

VTerrainSector* VTerrain::CreateSectorInstance(int iIndexX, int iIndexY)
{
 /* 
  if (!m_pSectorBlock)
  {
    int iCount = m_Config.m_iSectorCount[0]*m_Config.m_iSectorCount[1];
    m_pSectorBlock = (VTerrainSector*)vMemAlloc(iCount*sizeof(VTerrainSector));

    int i=0;
    for (int y=0;y<m_Config.m_iSectorCount[1];y++)
      for (int x=0;x<m_Config.m_iSectorCount[0];x++,i++)
      {
        ::new (&m_pSectorBlock[i]) VTerrainSector(&m_SectorManager,m_Config,x,y);
      }
  }
  return &m_pSectorBlock[iIndexY*m_Config.m_iSectorCount[0]+iIndexX];
*/
  
  return new VTerrainSector(&m_SectorManager,m_Config,iIndexX,iIndexY);
}


void VTerrain::EnsureSectorRangeLoaded(int iX1,int iY1, int iCountX,int iCountY)
{
  if (iCountX<0) iCountX = m_Config.m_iSectorCount[0] - iX1;
  if (iCountY<0) iCountY = m_Config.m_iSectorCount[1] - iY1;
  
  for (int y=0;y<iCountY;y++)
    for (int x=0;x<iCountX;x++)
      m_SectorManager.GetSector(x+iX1,y+iY1)->EnsureLoaded();
}


void VTerrain::DebugRender()
{
  if (!m_SectorManager.m_pRenderCollector)
    return;

  char szLine[1024];
  Vision::Message.Print(1,50,38,"Terrain : V=Visible, v=replacement visible, L=Loaded, C=Caching");
  
  memset(szLine,0,sizeof(szLine));
  for (int y=0;y<m_Config.m_iSectorCount[1];y++)
  {
    for (int x=0;x<m_Config.m_iSectorCount[0];x++)
    {
      VTerrainSector *pSector = m_SectorManager.GetSector(x,y);
      if (m_SectorManager.m_pRenderCollector->IsSectorReplacementVisible(pSector->VManagedResource::GetNumber()))
        szLine[0] = 'v';
      else if (m_SectorManager.m_pRenderCollector->IsSectorVisible(pSector->VManagedResource::GetNumber()))
        szLine[0] = 'V';
      else if (pSector->IsLoaded())
        szLine[0] = 'L';
      else if (pSector->IsPreCaching())
        szLine[0] = 'C';
      else
        szLine[0] = '.';
      Vision::Message.Print(1,x*12+50,(m_Config.m_iSectorCount[1]-y)*12+50,szLine);
    }
  }
}



void VTerrain::SetDecorationFarClipScaling(float fValue) 
{
  if (VTerrainSectorManager::g_fDecorationDistanceScaling == fValue)
    return;

  VTerrainSectorManager::g_fDecorationDistanceScaling = fValue;
  if (fValue<=0.f)
    return; // defined behavior (= no decoration)

  VTerrainSectorManager::g_fDecorationDistanceInvScaling = 1.f/fValue;

  for (int i=0;i<VTerrainDecorationModelManager::GlobalManager().GetResourceCount();i++)
  {
     IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)VTerrainDecorationModelManager::GlobalManager().GetResourceByIndex(i);
     if (!pModel || !pModel->IsLoaded())
       continue;
     pModel->UpdateParameter();
  }
}


float VTerrain::GetGlobalLODScaling()
{
  return VTerrainSectorManager::g_fDecorationDistanceScaling;
}

float VTerrain::GetGlobalInvLODScaling()
{
  return VTerrainSectorManager::g_fDecorationDistanceInvScaling;
}

void VTerrain::CheckUsesDirectionalLight()
{
  m_SectorManager.m_bShaderUsesDirectionalLight = false;
  if (m_SectorManager.m_spTerrainEffect==NULL)
    return;
  VCompiledTechnique* pTech = m_SectorManager.m_spTerrainEffect->GetDefaultTechnique();
  if (pTech != NULL && pTech->GetShaderCount() > 0)
  {
    VCompiledShaderPass *pShader = pTech->GetShader(0);
//    VConstantBufferRegister reg;
//    reg.Init(pShader, "_NoExport_LightDirection");
//    m_SectorManager.m_bShaderUsesDirectionalLight = reg.IsValid();

    // the UserBit1 in the tracking mask determines whether we want to track direction or not.
    // the commented code above does not work in DX11 because variables in b5 are not reflected
    m_SectorManager.m_bShaderUsesDirectionalLight = (pShader->GetRenderState ()->GetTrackingMask () & VSHADER_TRACKING_USERBIT1)>0;
  }
}


void VTerrain::SetDirectionalLightProperties(const hkvVec3& vDir, const hkvVec4& vLightColor, const hkvVec4& vAmbientColor)
{
  VTerrainDirectionalLightConfig &dir(m_SectorManager.m_DirLightConfig.BeginUpdate());
    dir.m_vLightDirection.set(-vDir.x,-vDir.y,-vDir.z,0.f);
    dir.m_vDirectionalLightColor = vLightColor;
    dir.m_vAmbientColor = vAmbientColor;
    dir.m_vLightDirection.normalizeIfNotZero();
  m_SectorManager.m_DirLightConfig.EndUpdate();
  CheckUsesDirectionalLight();
}


void VTerrain::RemoveDecorationInstances(const VLargeBoundingBox &bbox)
{
  int x1,y1,x2,y2;
  bbox.GetSectorIndices_Clamped(m_Config,x1,y1,x2,y2);
  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
      m_SectorManager.GetSector(x,y)->RemoveDecorationInstances(bbox);
}

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
