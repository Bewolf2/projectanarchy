/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>



float VTerrainSector::Noise2D(int x, int y)
{
  double fx = fmod((double)x*0.001,(double) (hkvMath::pi () * 2.0f));
  double fy = fmod((double)y*0.001,(double) (hkvMath::pi () * 2.0f));
  return (float)(sin(fx*4.0)*cos(fy*5.0) + 0.5 * sin(fx*9.0)*cos(fy*11.0) + 0.25 * sin(fx*17.0)*cos(fy*19.0)  + 0.13 * sin(fx*31.0)*cos(fy*33.0));
}


VTerrainSector::VTerrainSector(VTerrainSectorManager *pManager, const VTerrainConfig &config, int iIndexX, int iIndexY)
  : VManagedResource(pManager), m_Config(config), m_PerSectorObjects(0,NULL)
{
  //  SetResourceFlag(VRESOURCEFLAG_AUTODELETE|VRESOURCEFLAG_ALLOWUNLOAD);
  SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD); // no auto delete, currently messes with visibility when destroying terrain

  m_pSnapshot = NULL;
  m_PhysicsUserData = NULL;
  m_vSectorOrigin = config.GetSectorOrigin(iIndexX,iIndexY);
  m_piReferencedDecoration = NULL;
  m_iReferencedDecorationCount = 0;
  m_eGeometryType = STATIC_GEOMETRY_TYPE_TERRAIN;
  m_iIndexX = iIndexX;
  m_iIndexY = iIndexY;
  char szBuffer[128];
  sprintf(szBuffer,"Sector_%02i_%02i",m_iIndexX,m_iIndexY);
  SetFilename(szBuffer);
  m_fMinHeightValue = 0.f;
  m_fMaxHeightValue = 0.f;
  m_fMaxTileHandlingDistance = 0.f;
  m_pHeight = NULL;
  m_pTile = NULL;
  m_bPrepared = m_bSectorFileLoaded = m_bHasAdditionalDecoration = m_bFailedLoadingReplacementMesh = false;
  m_pMeshPage = NULL;
  m_pFirstDecoVisInfo = NULL;
  m_ePhysicsType = VPHYSICSTYPE_PRECISE;

  m_iSampleStrideX = config.m_iHeightSamplesPerSector[0]+2;

  // initialize visibility bounding box
  hkvAlignedBBox bbox(hkvNoInitialization);
  GetDefaultSectorBoundingBox(bbox);
  bbox.m_vMin.z = -10.f;
  bbox.m_vMax.z = 10.f;
  SetSectorBoundingBox(bbox); // geometry's bounding box, might change

  m_iPerSectorObjectCount = 0;
}


VTerrainSector::~VTerrainSector()
{
  V_SAFE_DELETE(m_pSnapshot)
    VASSERT(!IsLoaded());
  V_SAFE_DELETE_ARRAY(m_piReferencedDecoration);
  V_SAFE_DELETE_ARRAY(m_pHeight);
}

#define ADDREMOVENEIGHBOR(iIX,iIY) \
  if ((iIX)>=0 && (iIY)>=0 && (iIX)<m_Config.m_iSectorCount[0] && (iIY)<m_Config.m_iSectorCount[1])\
{\
  VTerrainSector *pOther = pSectors->GetSector(iIX,iIY);\
  bool bConnected = m_spSectorZone->IsConnected(pOther->m_spSectorZone);\
  if (bStatus && !bConnected)\
  m_spSectorZone->AddVisibilityZone(pOther->m_spSectorZone);\
    else if (!bStatus && bConnected)\
    m_spSectorZone->RemoveVisibilityZone(pOther->m_spSectorZone);\
}



void VTerrainSector::AssignVisibility(bool bStatus)
{
  VASSERT(!bStatus || m_spSectorZone);

  // remove from old
  IVisSceneManager_cl *pSM = Vision::GetSceneManager();
  VASSERT(pSM);

  RemoveFromAllVisibilityZones();
  VASSERT(m_pVisibilityZone==NULL);
  if (!m_spSectorZone)
    return;

  VTerrainSectorManager *pSectors = GetSectorManager();

  // assign to new
  if (!bStatus && m_spSectorZone->GetIndex()>=0)
    pSM->RemoveVisibilityZone(m_spSectorZone);
  else if (bStatus)
  {
    m_spSectorZone->AddStaticGeometryInstance(this);
    if (m_spSectorZone->GetIndex()<0)
      pSM->AddVisibilityZone(m_spSectorZone);

    if (pSectors->m_pTerrain->m_bVisibleInVisZones)
    {
      // also add to all other visibility zones
      for (int i=0;i<pSM->GetNumVisibilityZones();i++)
      {
        VisVisibilityZone_cl *pZone = pSM->GetVisibilityZone(i);
        if (pZone==NULL || pZone->GetTypeId()==V_RUNTIME_CLASS(VSectorVisibilityZone)) // don't assign to its own or sector neightbor zones
          continue;
        if (m_BoundingBox.overlaps(pZone->GetAssignmentBoundingBox()))
          pZone->AddStaticGeometryInstance(this);

      }
    }
  }

  ConnectNeighborZones(bStatus);
}

void VTerrainSector::ConnectNeighborZones(bool bStatus)
{
  // neighbor status: Connect visibility zone to its neighbors
  VTerrainSectorManager *pSectors = GetSectorManager();
  ADDREMOVENEIGHBOR(m_iIndexX-1,m_iIndexY);
  ADDREMOVENEIGHBOR(m_iIndexX+1,m_iIndexY);
  ADDREMOVENEIGHBOR(m_iIndexX,m_iIndexY-1);
  ADDREMOVENEIGHBOR(m_iIndexX,m_iIndexY+1);
}

float* VTerrainSector::AllocateHeightMap()
{
  if (!m_pHeight)
  {
    // allocate height values
    const int iSampleCount = GetHeightmapSampleCount();
    m_pHeight = new float[iSampleCount];
  }
  return m_pHeight;
}

bool VTerrainSector::SaveHeightmap()
{
  if (!m_pHeight)
    return false;

  char szFilename[FS_MAX_PATH];
  if (!m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"hmap",true))
    return false;

  m_Config.RCSPerformAction(szFilename, RCS_EDIT); //make sure we can write to it

  IVFileOutStream *pOut = GetParentManager()->CreateFileOutStream(szFilename,this);
  if (!pOut)
    return false;

  int iVersion=0;
  int iSampleCount = GetHeightmapSampleCount();

  pOut->Write(&iVersion,sizeof(iVersion),"i");
  pOut->Write(&iSampleCount,sizeof(iSampleCount),"i");

  bool bResult = pOut->Write(m_pHeight,iSampleCount*sizeof(float),"f",iSampleCount) == iSampleCount*sizeof(float);
  pOut->Close();
  m_Config.RCSPerformAction(szFilename, RCS_ADD);

  if (!m_Config.m_bUseTempFolder)
  {
    IVisPhysicsModule_cl *pModule = Vision::GetApplication()->GetPhysicsModule();
    if (pModule)
      pModule->OnTerrainSectorSaved(this);
  }

  return bResult;
}


void VTerrainSector::LoadHeightmap()
{
  if (m_pHeight==NULL)
    AllocateHeightMap();
  const int iSampleCount = GetHeightmapSampleCount();
  VASSERT(iSampleCount>0 && m_pHeight);

  bool bLoaded = false;
  char szFilename[FS_MAX_PATH];
  if (m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"hmap",false))
  {
    IVFileInStream *pIn = GetParentManager()->CreateFileInStream(szFilename,this);
    if (pIn)
    {
      int iVersion=0;
      int iFileSampleCount = 0;

      pIn->Read(&iVersion,sizeof(iVersion),"i");
      pIn->Read(&iFileSampleCount,sizeof(iFileSampleCount),"i");
      if (iFileSampleCount==iSampleCount)
      {
        // load block of height values
        bLoaded = pIn->Read(m_pHeight,iSampleCount*sizeof(float),"f",iSampleCount) == iSampleCount*sizeof(float);
      }
      pIn->Close();
    }
  }

  // procedurally generate a heightmap
  if (!bLoaded)
  {
    memset(m_pHeight,0,iSampleCount*sizeof(float));
  }


  // recalc min/max. Note that the extra border must not be considered
  m_fMaxHeightValue = m_fMinHeightValue = m_pHeight[0];
  for (int y=0;y<=m_Config.m_iHeightSamplesPerSector[1];y++)
  {
    float *pHeight = &m_pHeight[y*m_iSampleStrideX];
    for (int x=0;x<=m_Config.m_iHeightSamplesPerSector[0];x++)
    {
      m_fMaxHeightValue = hkvMath::Max(m_fMaxHeightValue, pHeight[x]);
      m_fMinHeightValue = hkvMath::Min(m_fMinHeightValue, pHeight[x]);
    }
  }

  // update bounding box:
  hkvAlignedBBox bbox(hkvNoInitialization);
  GetDefaultSectorBoundingBox(bbox);
  SetSectorBoundingBox(bbox);
}


float VTerrainSector::ComputeMaxErrorForLOD(int iLod,int x1,int y1,int x2,int y2)
{
  EnsureLoaded();
  VASSERT(iLod>0);
  const int iStep = 1<<iLod;
  const int iHalfStep = iStep/2;
  float fMax = 0.f;
  float d;
  for (int y=y1;y<y2;y+=iStep)
    for (int x=x1;x<x2;x+=iStep)
    {
      float fH = GetHeightAt(x,y);
      float fHdx = GetHeightAt(x+iStep,y);
      float fHdy = GetHeightAt(x,y+iStep);
      d = hkvMath::Abs (GetHeightAt(x+iHalfStep,y)-((fH+fHdx)*0.5f)); // error on dx edge
      fMax = hkvMath::Max(fMax,d);
      d = hkvMath::Abs (GetHeightAt(x,y+iHalfStep)-((fH+fHdy)*0.5f)); // error on dy edge
      fMax = hkvMath::Max(fMax,d);
      d = hkvMath::Abs (GetHeightAt(x+iHalfStep,y+iHalfStep)-((fHdx+fHdy)*0.5f)); // error on diagonal edge
      fMax = hkvMath::Max(fMax,d);
    }

    return fMax;
}


void VTerrainSector::ComputeLODDistanceTable()
{
  VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
  for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      for (int i=1;i<m_Config.m_iMaxMeshLOD;i++)
      {
        int x1 = m_Config.m_iSectorMeshVertexCount[0]*iPageX;
        int y1 = m_Config.m_iSectorMeshVertexCount[1]*iPageY;
        float fError = ComputeMaxErrorForLOD(i,x1,y1,x1+m_Config.m_iSectorMeshVertexCount[0],y1+m_Config.m_iSectorMeshVertexCount[1]);
        pPage->m_fLODThreshold[i-1] = fError;
      }
}

#define LOAD_TEXTURE(_ptr)\
{\
  if (iLocalVersion>=SECTOR_FILE_VERSION_19)\
{\
  _ptr = VTextureObject::DoArchiveExchange(ar, NULL);\
} else\
{\
  ar.ReadStringBinary(szFilename,FS_MAX_PATH);\
  if (szFilename[0]) _ptr = Vision::TextureManager.Load2DTexture(szFilename, iTextureFlags); else _ptr = NULL;\
}\
  if (_ptr && _ptr->IsMissing()) _ptr = NULL;\
}

bool VTerrainSector::LoadSectorInformation()
{
  char szFilename[FS_MAX_PATH];
  hkvAlignedBBox dummyBox(hkvNoInitialization);
  VTextureObjectPtr *pSceneLightmaps = GetSectorManager()->m_pTerrain->m_spSceneLightmaps;

  if (!m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"mesh",false))
    return false;

  IVFileInStream *pIn = GetParentManager()->CreateFileInStream(szFilename,this);
  if (!pIn)
    return false;

  bool bResult = true;
  VArchive ar(NULL,pIn,Vision::GetTypeManager());

  int iLocalVersion = 0;
  int iArchiveVersion, iDecoCount;
  short iTileX,iTileY;
  ar >> iArchiveVersion;
  ar.SetLoadingVersion(iArchiveVersion);
  ar >> iLocalVersion; // local version
  VASSERT(iLocalVersion<=SECTOR_CURRENT_FILE_VERSION);
  VASSERT_ALWAYS_MSG(iLocalVersion >= SECTOR_FILE_VERSION_8, "This terrain version isn't supported anymore. Please re-export the terrain with a current version.");

  ar >> iTileX >> iTileY; // sanity check
  if (iTileX!=m_Config.m_iTilesPerSector[0] || iTileY!=m_Config.m_iTilesPerSector[1]) 
  {
    bResult=false;
  }
  else
  {

    dummyBox.SerializeAs_VisBoundingBox (ar);
    if (!m_BoundingBox.isValid())
      SetSectorBoundingBox(dummyBox);
    ar >> m_fMaxTileHandlingDistance;

    // decoration objects
    char iDecoVersion = iLocalVersion>=SECTOR_FILE_VERSION_17 ? DECORATIONINSTANCE_VERSION_20 : iLocalVersion;
    if (iLocalVersion>=SECTOR_FILE_VERSION_22)
      ar >> iDecoVersion;

    ar >> iDecoCount;
    m_Decoration.Allocate(this, iDecoCount);
    for (int i=0;i<iDecoCount;i++)
      m_Decoration.m_pInstances[i].SerializeX(ar,iDecoVersion);

    // decoration refs:
    V_SAFE_DELETE_ARRAY(m_piReferencedDecoration);
    ar >> m_iReferencedDecorationCount;
    if (m_iReferencedDecorationCount>0)
    {
      m_piReferencedDecoration = new int[m_iReferencedDecorationCount];
      if (iLocalVersion>=SECTOR_FILE_VERSION_14)
        ar.Read(m_piReferencedDecoration,m_iReferencedDecorationCount*sizeof(m_piReferencedDecoration[0]),"i",m_iReferencedDecorationCount);
      else
      {
        // old path with 16bits
        for (int i=0;i<m_iReferencedDecorationCount;i++)
        {
          unsigned short iTempIdx;
          ar >> iTempIdx;
          m_piReferencedDecoration[i] = iTempIdx;
        }
      }
    }

    // tile information (including deco refs)
    VSectorTile *pTile = m_pTile;
    for (int i=0;i<m_Config.m_iTilesPerSectorCount;i++,pTile++)
    {
      pTile->SerializeX(ar, iLocalVersion);
    }

    // page information (holes)
    {
      VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
      for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      {
        for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
        {
          pPage->SerializeX(this,ar,iLocalVersion);
        }
      }
    }

    const int iTextureFlags = VTM_FLAG_DEFAULT_MIPMAPPED;

    // texture layer
    {
      VTextureObjectPtr spWeightMap;
      LOAD_TEXTURE(spWeightMap); // weightmap texture

      VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
      for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      {
        for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
        {
          pPage->GetSurfaceSafe().m_spDiffuseTexture = spWeightMap;
        }
      }
    }

    // Texture channels are now stored per page
    if(iLocalVersion < SECTOR_FILE_VERSION_21)
    {
      VTextureObject *pLayer[16];

      char iTextureType;
      for (int i=0;i<16;i++)
      {
        ar >> iTextureType;
        switch (iTextureType)
        {
        case SECTOR_TEXTURETYPE_NULL : pLayer[i] = GetSectorManager()->GetPlainBlackTexture();break; // this is a "real" black texture with alpha==0
        case SECTOR_TEXTURETYPE_WHITE : pLayer[i] = Vision::TextureManager.GetPlainWhiteTexture();break;
        case SECTOR_TEXTURETYPE_BLACK : pLayer[i] = Vision::TextureManager.GetPlainBlackTexture();break; // this one has alpha==255
        case SECTOR_TEXTURETYPE_FILE : LOAD_TEXTURE(pLayer[i]);break;
        case SECTOR_TEXTURETYPE_NEUTRALNORMAL: pLayer[i] = Vision::TextureManager.GetNeutralNormalMapTexture();break;
        default:
          VASSERT(!"Invalid texture type. File corrupt?");
        }
        if (pLayer[i]) // valid for all types of texture
          pLayer[i]->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

        VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
        for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
        {
          for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
          {
            pPage->SetTextureBySamplerIndex(pLayer[i],i);
          }
        }
      }

      if (iLocalVersion>=SECTOR_FILE_VERSION_10)
      {
        int iReserved;
        int iSamplerClampMask;
        ar >> iSamplerClampMask;
        VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
        for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
        {
          for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
          {
            pPage->m_iSamplerClampMask = iSamplerClampMask;
          }
        }
        ar >> iReserved; VASSERT(iReserved==0);
      }
    } 
  }

  hkvVec4 vLM(false);
  // lightmap transformation (version 9)
  if (iLocalVersion>=SECTOR_FILE_VERSION_9)
    ar >> vLM;
  else
  {
    // for backwards compatibility rather use global UV
    float fInvSectorCountX = 1.f/(float)m_Config.m_iSectorCount[0];
    float fInvSectorCountY = 1.f/(float)m_Config.m_iSectorCount[1];
    vLM.set(fInvSectorCountX,fInvSectorCountY,(float)m_iIndexX*fInvSectorCountX,(float)m_iIndexY*fInvSectorCountY);
  }
  if (pSceneLightmaps==NULL)
    SetLightmapScaleOffset(vLM); // otherwise retain values loaded through VTerrain::Serialize

  // matchup lightmap textures
  if(iLocalVersion < SECTOR_FILE_VERSION_21)
  {
    char iLightmapSamplerIndex[4];
    ar.Read(iLightmapSamplerIndex, 4, "cccc"); // 4 chars
    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        memcpy(pPage->m_iLightmapSamplerIndex, iLightmapSamplerIndex, sizeof(iLightmapSamplerIndex));
        for (int i=0;i<4;i++)
        {
          VTextureObject *pTex = NULL;
          if (iLightmapSamplerIndex[i] >= 0)
          {
            // scene specific lightmaps?
            if (pSceneLightmaps!=NULL)
            {
              pTex = pSceneLightmaps[(m_iIndexY*m_Config.m_iSectorCount[0]+m_iIndexX) * 4 + i];
              pPage->SetTextureBySamplerIndex(pTex, iLightmapSamplerIndex[i]);
            }
            else
            {
              pTex = pPage->GetTextureBySamplerIndex((int)pPage->m_iLightmapSamplerIndex[i]);
            }
          }

          pPage->GetSurfaceSafe().m_spModelLightmaps[i] = pTex;
        }
      }
    }
  }

  {
    // shadow map references (vers. 5)
    VASSERT(m_pLightMaskEntries==NULL);
    ar >> m_iNumLightMaskEntries;
    if (m_iNumLightMaskEntries>0)
    {
      m_pLightMaskEntries = new VisLightMask_cl[m_iNumLightMaskEntries];
      for (int i=0;i<m_iNumLightMaskEntries;i++)
      {
        m_pLightMaskEntries[i].SerializeX(ar,true); // use light ID here to not fully reconstruct the light objects
        // fixup the pointers in the light source. It might be in the list already if the sector is loaded/unloaded multiple times
        VisLightSource_cl *pLight = m_pLightMaskEntries[i].m_pLight;
        if (pLight && !Vision::Editor.IsInEditor() && !pLight->AffectsStaticGeometryInstance(this))
          pLight->AddAffectedStaticGeometryInstance(this);
      }
    }
  }

  if(iLocalVersion < SECTOR_FILE_VERSION_21)
  {
    int iRegCountInFile;
    ar >> iRegCountInFile;
    int iRegCountClamped = hkvMath::Max(iRegCountInFile,6);

    int iSize = sizeof(hkvVec4) * iRegCountClamped;
    VMemoryTempBuffer<256> tempMem(iSize);
    ar.Read(tempMem.GetBuffer(),iRegCountInFile*sizeof(hkvVec4),"ffff",iRegCountInFile);

    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        pPage->m_iPixelShaderRegCount = iRegCountClamped;

#if defined(_VR_DX11)
        pPage->m_SectorPSConstBuffer.AllocateBuffer(0,iSize);
#endif
        hkvVec4* pPixelConst = pPage->LockPixelConst();
        memcpy(pPixelConst, tempMem.GetBuffer(), iRegCountInFile*sizeof(hkvVec4));
        pPage->UpdateNormalmapRegister (pPixelConst);
        pPage->UnLockPixelConst();
      }
    }
  }


  // version 11: per sector objects
  DisposePerSectorObjects();
  VASSERT(m_iPerSectorObjectCount==0);
  if (iLocalVersion>=SECTOR_FILE_VERSION_11)
  {
    int iCount;
    ar >> iCount;
    m_PerSectorObjects.EnsureSize(iCount);
    for (int i=0;i<iCount;i++)
    {
      VisTypedEngineObject_cl *pObj = (VisTypedEngineObject_cl *)ar.ReadObject(NULL);
      if (pObj!=NULL)
        m_PerSectorObjects.GetDataPtr()[m_iPerSectorObjectCount++] = pObj;
    }
  }

  // version 15: material map
  if (iLocalVersion>=SECTOR_FILE_VERSION_15)
  {
    bool bHasMaterialMap = false;
    ar >> bHasMaterialMap;
    if (bHasMaterialMap)
    {
      m_spMaterialIDMap = new VLuminanceChannel(0,0); // will be allocated in serialization
      m_spMaterialIDMap->SerializeX(ar);
    }
  }

  // version 16: three way mapping
  if (iLocalVersion >= SECTOR_FILE_VERSION_16 && iLocalVersion < SECTOR_FILE_VERSION_21)
  {
    int iNumThreeWayMappingLayer;
    ar >> iNumThreeWayMappingLayer;
    int iThreeWayMappingLayerMask;
    if (iNumThreeWayMappingLayer > 0)
    {
      ar >> iThreeWayMappingLayerMask;
    }

    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        pPage->m_iNumThreeWayMappingLayer = iNumThreeWayMappingLayer;
        pPage->m_iThreeWayMappingLayerMask = iThreeWayMappingLayerMask;
      }
    }
  }

  // version 18: exposed material IDs
  if (iLocalVersion >= SECTOR_FILE_VERSION_18 && iLocalVersion < SECTOR_FILE_VERSION_21)
  {
    short materialIDs[9];
    ar.Read(materialIDs, 9 * sizeof(short));

    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        memcpy(pPage->m_MaterialIDs, materialIDs, 9 * sizeof(short));
      }
    }
  }

  // version 20: physics type
  if (iLocalVersion >= SECTOR_FILE_VERSION_20)
  {
    int iTemp;
    ar >> iTemp;
    m_ePhysicsType = (VPhysicsType_e)iTemp;
  }

  ar.Close();
  pIn->Close();

  return bResult;
}

#define SAVE_TEXTURE(_ptr) \
{\
  if (_ptr && _ptr!=Vision::TextureManager.GetPlainWhiteTexture() && _ptr!=Vision::TextureManager.GetPlainBlackTexture() && _ptr!=GetSectorManager()->GetPlainBlackTexture())\
{\
  VTextureObject::DoArchiveExchange(ar, _ptr);\
}\
  else\
  VTextureObject::DoArchiveExchange(ar, NULL);\
}


bool VTerrainSector::SaveSectorInformation()
{
  char szFilename[FS_MAX_PATH];
  if (!m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"mesh",true))
    return false;

  m_Config.RCSPerformAction(szFilename, RCS_EDIT); //make sure we can write to it

  IVFileOutStream *pOut = GetParentManager()->CreateFileOutStream(szFilename,this);
  if (!pOut)
    return false;
    
  ComputeLODDistanceTable(); // keep them up to date
  VArchive ar(NULL,pOut,Vision::GetTypeManager());

  int iLocalVersion = SECTOR_CURRENT_FILE_VERSION;
  ar << Vision::GetArchiveVersion();
  ar << iLocalVersion; // local version

  short iTileX = m_Config.m_iTilesPerSector[0];
  short iTileY = m_Config.m_iTilesPerSector[1];
  ar << iTileX << iTileY; // for sanity

  m_BoundingBox.SerializeAs_VisBoundingBox (ar);
  ar << m_fMaxTileHandlingDistance;

  // decoration objects
  char iDecoVersion = DECORATIONINSTANCE_CURRENT_VERSION;
  ar << iDecoVersion; // SECTOR_FILE_VERSION_22
  ar << m_Decoration.m_iCount;
  for (int i=0;i<m_Decoration.m_iCount;i++)
    m_Decoration.m_pInstances[i].SerializeX(ar, iDecoVersion);

  // decoration refs:
  ar << m_iReferencedDecorationCount;
  if (m_iReferencedDecorationCount>0)
    ar.Write(m_piReferencedDecoration,m_iReferencedDecorationCount*sizeof(m_piReferencedDecoration[0]));

  // tile information (including deco refs)
  VSectorTile *pTile = m_pTile;
  for (int i=0;i<m_Config.m_iTilesPerSectorCount;i++,pTile++)
    pTile->SerializeX(ar, iLocalVersion);

  // version 7 page information (holes)
  VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
  for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
  {
    for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
    {
      pPage->SerializeX(this,ar,iLocalVersion);
    }
  }

  // texture layer (version 1)
  VASSERT(m_spMesh);
  SAVE_TEXTURE(m_pMeshPage[0].GetSurfaceSafe().m_spDiffuseTexture);
  VTextureObject *pNullTex = GetSectorManager()->GetPlainBlackTexture();

  // lightmap transformation (version 9)
  hkvVec4 vLM = GetLightmapScaleOffset();
  ar << vLM;

  // shadow map references (vers. 5)
  ar << m_iNumLightMaskEntries;
  for (int i=0;i<m_iNumLightMaskEntries;i++)
    m_pLightMaskEntries[i].SerializeX(ar,true); // use light ID here to not fully reconstruct the light objects

  // version 11: per sector objects
  ar << m_iPerSectorObjectCount;
  for (int i=0;i<m_iPerSectorObjectCount;i++)
    ar << m_PerSectorObjects.GetDataPtr()[i];

  // version 15: material map
  ar << (bool)(m_spMaterialIDMap!=NULL);
  if (m_spMaterialIDMap!=NULL)
    m_spMaterialIDMap->SerializeX(ar);

  // version 20: physics type
  ar << (int)m_ePhysicsType;

  ar.Close();
  pOut->Close();
  m_Config.RCSPerformAction(szFilename, RCS_ADD);

  return true;
}



void VTerrainSector::OnPositionChanged(const hkvVec3& vOldPos,const hkvVec3& vNewPos)
{
  m_vSectorOrigin =  m_Config.GetSectorOrigin(m_iIndexX,m_iIndexY);
  hkvVec3 vDelta = vNewPos-vOldPos;
  if (!vDelta.isZero ())
  {
    EnsureLoaded(); // this is necessary because all bounding boxes need to update

    // translate tile boxes
    if (m_pTile)
    {
      VSectorTile *pTile = m_pTile;
      for (int y=0;y<m_Config.m_iTilesPerSector[1];y++)
        for (int x=0;x<m_Config.m_iTilesPerSector[0];x++,pTile++)
          pTile->m_TileBBox.translate (vDelta);
    }

    // translate the visibility/sector box
    hkvAlignedBBox oldBox = GetBoundingBox();
    oldBox.translate (vDelta);
    SetSectorBoundingBox(oldBox, true);

    // update decoration
    for (int i=0;i<m_Decoration.m_iCount;i++)
      m_Decoration.m_pInstances[i].IncPosition(vDelta);

#ifndef TERRAIN_COLLISIONMESHES_SECTORLOCALSPACE
    if (m_spCollisionMeshes)
      m_spCollisionMeshes->InvalidateAllMeshes();
#endif
  }

  VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
  if (pPage)
  {
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        pPage->Init(this, iPageX,iPageY,false);
      }
    }
  }
}


bool VTerrainSector::SaveSector(bool bUseTemp)
{
  m_Config.m_bUseTempFolder = bUseTemp; // temp or final directory?
  bool bSuccess = true;
  bSuccess &= SaveSectorInformation();
  bSuccess &= SaveHeightmap();

  //NB. If you add anything here, you also have to add it to VEditableTerrainSector::SaveSector

  return bSuccess;
}

bool VTerrainSector::DeleteSectorData(bool bUseTemp)
{
  bool bSuccess = true;
  bool bOldUseTemp = m_Config.m_bUseTempFolder;
  m_Config.m_bUseTempFolder = bUseTemp; // temp or final directory?

  const char *szDataDir = GetSectorManager()->GetFileManager()->GetDataDirectory();

  const char *relevantExt[] = {"mesh","hmap",NULL};
  for (int i = 0; relevantExt[i] != NULL; i++)
  {
    char szFilename[FS_MAX_PATH];
    if (!m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,relevantExt[i],true))
      return false;

    m_Config.RCSPerformAction(szFilename, RCS_REMOVE); // open for delete

    char szFileAndPath[FS_MAX_PATH];
    VFileHelper::CombineDirAndFile(szFileAndPath,szDataDir,szFilename);
    VFileHelper::Delete(szFileAndPath, true);
  }

  m_Config.m_bUseTempFolder = bOldUseTemp;
  return bSuccess;
}

void VTerrainSector::OnConfigChanged()
{
  m_vSectorOrigin = m_Config.GetSectorOrigin(m_iIndexX,m_iIndexY);
  hkvAlignedBBox bbox(hkvNoInitialization);
  GetDefaultSectorBoundingBox(bbox);
  SetSectorBoundingBox(bbox);

  // this is the safest way to guarantee that new properties are applied
  if (IsLoaded() && CanUnload()) 
  {
    // update tile bboxes
    VSectorTile *pTile = m_pTile;
    for (int y=0;y<m_Config.m_iTilesPerSector[1];y++)
      for (int x=0;x<m_Config.m_iTilesPerSector[0];x++,pTile++)
      {
        GetDefaultTileBoundingBox(bbox,x,y);
        pTile->Init(this,x,y,bbox);
      }

      EnsureUnloaded();
      EnsureLoaded();
  }
}



void VTerrainSector::OnDefaultShaderEffectChanged()
{
  if (m_spMesh)
  {
    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        VTechniqueConfig config;

        if (pPage->m_iNumThreeWayMappingLayer > 0)
        {
          config.AddInclusionTagInt("NUM_3WAY_LAYERS", pPage->m_iNumThreeWayMappingLayer);
        }

        VCompiledEffect* pTerrainFX = GetSectorManager()->GetTerrainEffect();
        VASSERT_MSG(pTerrainFX, "Failed to get terrain effect");
        VisSurface_cl &surface = pPage->GetSurfaceSafe();

        VTextureObjectPtr spBackup = surface.m_spNormalMap;
        surface.m_spNormalMap = NULL; // pretend that we don't have a normalmap because the normal map has a completely different semantic here
        surface.SetEffect(pTerrainFX, &config);
        surface.m_spNormalMap = spBackup;

        /*
        TODO

        VCompiledTechnique *pTech = surface.GetTechnique();
        VASSERT_MSG(pTech, "Terrain effect does not have valid technique");
        m_spMesh->SetDefaultTechnique(pTech);*/
      }
    }
  }
}


BOOL VTerrainSector::Reload()
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_RELOAD);
  int x,y;

  // we don't need it anymore
  V_SAFE_DELETE(m_pSnapshot)

    // init the tiles
    VASSERT(m_pTile==NULL);
  m_pTile = new VSectorTile[m_Config.m_iTilesPerSectorCount];
  VSectorTile *pTile = m_pTile;
  hkvAlignedBBox bbox = GetBoundingBox();
  //  float fStepX = m_Config.m_vTileSize.x;
  //  float fStepY = m_Config.m_vTileSize.y;

  for (y=0;y<m_Config.m_iTilesPerSector[1];y++)
  {
    for (x=0;x<m_Config.m_iTilesPerSector[0];x++,pTile++)
    {
      GetDefaultTileBoundingBox(bbox,x,y);
      pTile->Init(this,x,y,bbox);
    }
  }

  //init the pages
  VASSERT(m_pMeshPage==NULL);
  m_pMeshPage = new VTerrainSectorMeshPageInfo[m_Config.m_iSectorMeshCount];
  VTerrainSectorMeshPageInfo* pPage = m_pMeshPage;
  for (y=0;y<m_Config.m_iSectorMeshesPerSector[1];y++)
  {
    for (x=0;x<m_Config.m_iSectorMeshesPerSector[0];x++,pPage++)
    {
      pPage->Init(this,x,y,true);
    }
  }

  SetSurface(&m_pMeshPage[0].GetSurfaceSafe());

  LoadHeightmap();

  bool bNeedsNormalmap = Vision::Editor.IsInEditor(); // always use in editor

  // create the vertex streams
  if (!m_spMesh)
  {
    m_spMesh = new VTerrainSectorMeshBuffer();
    m_spMesh->m_pOwnerSector = this;
    m_spMesh->EnsureLoaded();
    m_spMesh->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

#ifdef HK_DEBUG
    m_spMesh->SetFilename("<VTerrainSectorMeshBuffer>");
#endif
  }

  m_bSectorFileLoaded = LoadSectorInformation(); 

  // remap texture sampler (currently only supports single pass terrain rendering)
  OnDefaultShaderEffectChanged();

  // any sampler references normalmap?
  pPage = m_pMeshPage;
  for (y=0;y<m_Config.m_iSectorMeshesPerSector[1];y++)
  {
    for (x=0;x<m_Config.m_iSectorMeshesPerSector[0];x++,pPage++)
    {
      VCompiledTechnique* pTech = pPage->GetSurfaceSafe().GetTechnique();
      for (int i=0;i<pTech->GetShaderCount();i++)
      {
        const unsigned int iActiveSamplerCount = pTech->GetShader(i)->GetActiveSamplerCount(VSS_PixelShader);
        for (unsigned int j=0;j<iActiveSamplerCount;j++)
        {
          // any sampler references normalmap?
          if (pTech->GetShader(i)->GetStateGroupTexture(VSS_PixelShader, j)->m_cTextureType == TEXTURETYPE_NORMALMAP)
          {
            bNeedsNormalmap = true;  
          }
        }
      }
    }
  }

  int iNrmlLoadingFlags = VTM_FLAG_DEFAULT_MIPMAPPED;
  // in case the normalmap texture isn't needed by the normal terrain shader, upload it on demand
  if (!bNeedsNormalmap)
    iNrmlLoadingFlags |= VTM_FLAG_NO_UPLOAD;

  // load normal map texture.
  char szFilename[FS_MAX_PATH];
  if (m_Config.GetSectorNormalmapFilename(szFilename,m_iIndexX,m_iIndexY,false))
  {
    VTextureObjectPtr spNormalMap;

    spNormalMap = Vision::TextureManager.GetTextureObjectByName(szFilename); // might be streamed already, then do not call Vision::File.Exists
    if (spNormalMap == NULL)
    {
      if (Vision::File.Exists(szFilename))
      {
        spNormalMap = Vision::TextureManager.Load2DTexture(szFilename,iNrmlLoadingFlags);
        spNormalMap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
      }
      else if (!Vision::Editor.IsInEditor())
      {
        spNormalMap = Vision::TextureManager.GetNeutralNormalMapTexture(); // share blank normalmap for runtime
      }
    }

    pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        pPage->GetSurfaceSafe().m_spNormalMap = spNormalMap;
      }
    }
  }

  OnDefaultShaderEffectChanged();
  FlagAsLoaded(); // otherwise UpdateMesh would call this function again
  UpdateMesh(true);
  m_bPrepared = true;

  // make sure everything is inflated properly
  SetSectorBoundingBox(GetBoundingBox(),true);

  // trigger callback
  VisCallback_cl &cbLoaded = VTerrainManager::OnSectorLoaded;
  VTerrainDataObject dataLoaded(&cbLoaded, GetSectorManager()->m_pTerrain,this);
  cbLoaded.TriggerCallbacks(&dataLoaded);

  // and notify physics:
  IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
  if (pPM)
  {
    pPM->OnTerrainSectorCreated(this);
    // decoration:
    pPM->OnDecorationCreated(m_Decoration);
  }

  // workaround for [HS#10086], broken tracing (tile bb) when new sectors are added later and existing terrain is moved by a sector offset
  if (m_Config.IsMovedSectorStartPosition())
  {
    m_vSectorOrigin = m_Config.GetSectorOrigin(m_iIndexX,m_iIndexY);
    hkvAlignedBBox bbox(hkvNoInitialization);
    GetDefaultSectorBoundingBox(bbox);
    SetSectorBoundingBox(bbox);

    VSectorTile *pTile = m_pTile;
    for (int y=0;y<m_Config.m_iTilesPerSector[1];y++)
    {
      for (int x=0;x<m_Config.m_iTilesPerSector[0];x++,pTile++)
      {
        GetDefaultTileBoundingBox(bbox,x,y);
        pTile->Init(this,x,y,bbox);
      }
    }
  }

  // trigger callback
  VisCallback_cl &cbCreated = VTerrainManager::OnSectorCreated;
  VTerrainDataObject dataCreated(&cbCreated, GetSectorManager()->m_pTerrain,this);
  cbCreated.TriggerCallbacks(&dataCreated);

  this->m_bCastDynamicShadows = true; // need to be enabled by default now - the shadow component checks the geometry flags

  return TRUE;
}



bool VTerrainSector::EnsureReplacementMeshLoaded()
{
  if (m_spReplacementMesh)
    return true;

  if (m_bFailedLoadingReplacementMesh)
    return false;

  char szFilename[FS_MAX_PATH];
  if (!m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"lowres",true))
  {
    m_bFailedLoadingReplacementMesh = true;
    return false;
  }

  IVFileInStream *pIn = GetParentManager()->CreateFileInStream(szFilename,this);
  if (!pIn)
  {
    m_bFailedLoadingReplacementMesh = true;
    return false;
  }

  VArchive ar(NULL,pIn,Vision::GetTypeManager());

  int iArchiveVersion = 0;
  int iLocalVersion = 0;
  ar >> iArchiveVersion;
  ar.SetLoadingVersion(iArchiveVersion);
  ar >> iLocalVersion; // local version

  // the file contains only height values
  int iCountX,iCountY;
  ar >> iCountX >> iCountY;
  VASSERT(iCountX==m_Config.m_iSamplesPerReplacementMesh[0] && iCountY==m_Config.m_iSamplesPerReplacementMesh[1]);

  m_spReplacementMesh = new VisMeshBuffer_cl();

#ifdef HK_DEBUG
  m_spReplacementMesh->SetFilename(szFilename);
#endif

  int iStepX = m_Config.m_iHeightSamplesPerSector[0] / iCountX;
  int iStepY = m_Config.m_iHeightSamplesPerSector[1] / iCountY;
  int iOfsX = m_iIndexX * m_Config.m_iHeightSamplesPerSector[0];
  int iOfsY = m_iIndexY * m_Config.m_iHeightSamplesPerSector[1];

  int iVertexCount = (iCountX+1)*(iCountY+1);
  int iIndexCount = iCountX*iCountY*6;
  bool bDoubleBuffered = false;
  m_spReplacementMesh->AllocateVertices(VTerrainVertexLowRes_t::GetDesc(), iVertexCount, VIS_MEMUSAGE_STATIC, bDoubleBuffered);

  VMemoryTempBuffer<65*65*sizeof(VTerrainVertexLowRes_t)> tempVertices((int)hkvMath::Max(iVertexCount*sizeof(VTerrainVertexLowRes_t),iIndexCount*sizeof(short)));
  VTerrainVertexLowRes_t *pV = (VTerrainVertexLowRes_t *)tempVertices.GetBuffer();

  for (int y=0;y<=iCountY;y++)
    for (int x=0;x<=iCountX;x++,pV++)
    {
      int iSampleX = x*iStepX;
      int iSampleY = y*iStepY;
      pV->vPos.x = m_Config.GetHeightmapSamplePosX((float)(iSampleX+iOfsX));
      pV->vPos.y = m_Config.GetHeightmapSamplePosX((float)(iSampleY+iOfsY));
      float fH;
      ar >> fH;
      pV->vPos.z = fH + m_Config.m_vTerrainPos.z;
    }

    ar.Close();
    pIn->Close();

    m_spReplacementMesh->FillVerticesWithData(tempVertices.GetBuffer(),iVertexCount*sizeof(VTerrainVertexLowRes_t));

    m_spReplacementMesh->AllocateIndexList(iIndexCount,VIS_MEMUSAGE_STATIC, VIS_INDEXFORMAT_16);
    unsigned short *pInd = (unsigned short *)tempVertices.GetBuffer();
    int iRowStride = iCountX+1;
    for (int y=0;y<iCountY;y++)
      for (int x=0;x<iCountX;x++,pInd+=6)
      {
        int iFirst = y*iRowStride+x;
        pInd[0] = iFirst+iRowStride+1;
        pInd[1] = iFirst+iRowStride;
        pInd[2] = iFirst;
        pInd[3] = iFirst+1;
        pInd[4] = iFirst+iRowStride+1;
        pInd[5] = iFirst;
      }

      m_spReplacementMesh->FillIndicesWithData(tempVertices.GetBuffer(),iIndexCount*sizeof(short));

      m_spReplacementMesh->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
      m_spReplacementMesh->SetPrimitiveCount(iIndexCount/3);

      return true;
}



void VTerrainSector::UpdateHoleMask()
{
  VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
  for (int y=0;y<m_Config.m_iSectorMeshesPerSector[1];y++)
    for (int x=0;x<m_Config.m_iSectorMeshesPerSector[0];x++,pPage++)
      pPage->UpdateHoleMask(this,x,y);
}


void VTerrainSector::RemovePerSectorObject(VisTypedEngineObject_cl *pObj)
{
  int iCount = m_iPerSectorObjectCount;
  m_iPerSectorObjectCount = 0;
  for (int i=0;i<iCount;i++)
    if (m_PerSectorObjects.GetDataPtr()[i]!=NULL && m_PerSectorObjects.GetDataPtr()[i]!=pObj)
      m_PerSectorObjects.GetDataPtr()[m_iPerSectorObjectCount++] = m_PerSectorObjects.GetDataPtr()[i];
}


void VTerrainSector::DisposePerSectorObjects()
{
  VisTypedEngineObject_cl **pSectorObj = m_PerSectorObjects.GetDataPtr();
  for (int i=0;i<m_iPerSectorObjectCount;i++)
    V_SAFE_DISPOSEOBJECT(pSectorObj[i]);
  m_iPerSectorObjectCount = 0;
  m_PerSectorObjects.Reset();
}


void VTerrainSector::OnDestroying()
{
  m_spCollisionMeshes = NULL;
  V_SAFE_DELETE(m_pSnapshot);
}


BOOL VTerrainSector::Unload()
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_UNLOAD);
  V_SAFE_DELETE(m_pSnapshot)
    m_spMaterialIDMap = NULL;
  V_SAFE_DELETE_ARRAY(m_pHeight);
  V_SAFE_DELETE_ARRAY(m_pTile);
  V_SAFE_DELETE_ARRAY(m_pMeshPage);
  VTerrainSectorDecorationVisibilityMask::DeleteRecursive(m_pFirstDecoVisInfo);
  m_pFirstDecoVisInfo = NULL;

  V_SAFE_DELETE_ARRAY(m_piReferencedDecoration);
  m_iReferencedDecorationCount = 0;

  // stored per sector
  m_iNumLightMaskEntries = 0;
  V_SAFE_DELETE_ARRAY(m_pLightMaskEntries);

  SetNewMemSize(VRESOURCEMEMORY_ALLTYPES,0);
  m_spMesh = NULL;
  m_bPrepared = false;
  m_bSectorFileLoaded = false;

  // trigger callback
  VisCallback_cl &cb = VTerrainManager::OnSectorUnloaded;
  VTerrainDataObject data(&cb, GetSectorManager()->m_pTerrain,this);
  cb.TriggerCallbacks(&data);

  // notify physics:
  IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
  if (pPM)
  {
    pPM->OnTerrainSectorRemoved(this);
    // decoration:
    pPM->OnDecorationRemoved(m_Decoration);
  }

  m_spCollisionMeshes = NULL;
  m_spReplacementMesh = NULL;
  m_Decoration.Free();
  DisposePerSectorObjects();

  m_spTerrainReplacementTechnique = NULL;

  return TRUE;
}

#if ( defined(_VR_DX9) && defined(WIN32) )

void VTerrainSector::OnEnterBackground()
{
  VManagedResource::OnEnterBackground();

  if (m_spMesh)
    m_spMesh->FreeVertices();
  m_spReplacementMesh = NULL;
}

void VTerrainSector::OnLeaveBackground()
{
  VManagedResource::OnLeaveBackground();
  if (m_spMesh)
    UpdateMesh(true);

}

#endif


#ifdef SUPPORTS_SNAPSHOT_CREATION

void VTerrainSector::GetDependencies(VResourceSnapshot &snapshot)
{
  // add all decoration core models
  VTerrainDecorationInstance *pInst = m_Decoration.m_pInstances;
  for (int i=0;i<m_Decoration.m_iCount;i++,pInst++)
    if (pInst->m_spModel)
      pInst->m_spModel->GetDependencies(snapshot);

  // add files
  char szFilename[FS_MAX_PATH];
  const char *relevantExt[] = {"mesh","hmap",NULL};

  for (int i=0;relevantExt[i];i++)
    if (m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY, relevantExt[i],true))
    {
      IVFileInStream *pIn = GetParentManager()->CreateFileInStream(szFilename,this);
      if (pIn)
      {
        snapshot.AddFileDependency(this, szFilename, pIn->GetSize());
        pIn->Close();
      }
    }

    // attached textures
    if (m_spMesh)
    {
      m_spMesh->GetDependencies(snapshot);
    }

    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        pPage->GetSurfaceSafe().GetDependencies(snapshot, this);
      }
    }

    // per sector objects
    for (int i=0;i<m_iPerSectorObjectCount;i++)
      m_PerSectorObjects.GetDataPtr()[i]->GetDependencies(snapshot);

    IVisPhysicsModule_cl *pModule = Vision::GetApplication()->GetPhysicsModule();
    if (pModule != NULL)
      pModule->GetDependenciesForResource(snapshot, this);
}

#endif

static VisionResourceCreator_cl g_ResourceCreator;

void VTerrainSector::PreCache()
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_STREAMING);

  if (IsLoaded() || GetVisibleBitmask()==0)
    return;

  // handle it when it has started already
  if (m_pSnapshot)
  {
    // still streaming the vres file itself?
    if (m_pSnapshot->m_spResFileLoadingTask!=NULL)
    {
      if (!m_pSnapshot->m_spResFileLoadingTask->IsLoaded())
        return; // still caching the vres
      IVFileInStream *pIn = Vision::File.Open(m_pSnapshot->m_spResFileLoadingTask->GetFilename());
      m_pSnapshot->LoadFromBinaryFile(pIn);
      pIn->Close();
      Vision::GetSceneManager()->ScheduleSnapshot(m_pSnapshot);
      m_pSnapshot->m_spResFileLoadingTask = NULL; // destroy mem file
      return; // return for now and continue next frame
    }

    // update priority every frame: Set the negative distance so close sectors are cached first
    const hkvVec3 vCamPos = Vision::GetSceneManager()->GetStreamingReferenceSafe()->GetPosition();
    float fDist = GetBoundingBox().getDistanceTo(vCamPos);
    m_pSnapshot->SetPriority(-fDist);

    if (m_pSnapshot->IsFinished())
    {
      EnsureLoaded();
      return;
    }
    return;
  }

  char szFilename[FS_MAX_PATH];

  // initiate snapshot loading from binary file
  m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"vres",false);
  m_pSnapshot = new VSectorResourceSnapshot();
  m_pSnapshot->m_spResFileLoadingTask = Vision::File.PrecacheFile(szFilename);
  if (m_pSnapshot->m_spResFileLoadingTask==NULL || m_pSnapshot->m_spResFileLoadingTask->IsMissing())
  {
    Vision::Error.Warning("Streaming package '%s' not found", szFilename);
    this->EnsureLoaded();
  }
}


void VTerrainSector::SetSectorBoundingBox(const hkvAlignedBBox &box, bool bInflateByTiles)
{
  // both set on sector and visibility zone:
  VisStaticGeometryInstance_cl::SetBoundingBox(box);
  VASSERT(box.isValid());

  // set a larger bounding box on the visibility zone so camera assignment is correct
  if (m_spSectorZone)
  {
    hkvAlignedBBox zoneBox = m_spSectorZone->GetBoundingBox();
    if (!zoneBox.isValid() || bInflateByTiles)
      zoneBox = box;
    zoneBox.m_vMax.z = box.m_vMax.z + m_Config.m_fVisibilityHeightOverTerrain;
    zoneBox.m_vMin.z = box.m_vMin.z - m_Config.m_fVisibilityHeightOverTerrain;
    if (bInflateByTiles && m_pTile!=NULL)
    {
      VSectorTile *pTile = m_pTile;
      for (int i=0;i<m_Config.m_iTilesPerSectorCount;i++,pTile++)
        zoneBox.expandToInclude(pTile->m_TileBBox);
    }

    // also inflate z by static geometry that is already assigned to it (e.g. cave geometry):
    const VisStaticGeometryInstanceCollection_cl *pInst = m_spSectorZone->GetStaticGeometryInstances();
    if (pInst)
    {
      const int iCount = pInst->GetNumEntries();
      for (int i=0;i<iCount;i++)
      {
        VisStaticGeometryInstance_cl *pGeom = pInst->GetEntry(i);
        const hkvAlignedBBox &bbox = pGeom->GetBoundingBox();
        if(!bbox.isValid())
          continue;
        zoneBox.m_vMin.z = hkvMath::Min(zoneBox.m_vMin.z,bbox.m_vMin.z);
        zoneBox.m_vMax.z = hkvMath::Max(zoneBox.m_vMax.z,bbox.m_vMax.z);
      }
    }

    // the outer zones basically get stretched to infinity:
    float fLarge = 1000000000.f * Vision::World.GetGlobalUnitScaling();
    if (m_iIndexX==0) zoneBox.m_vMin.x -= fLarge;
    if (m_iIndexY==0) zoneBox.m_vMin.y -= fLarge;
    if (m_iIndexX==m_Config.m_iSectorCount[0]-1) zoneBox.m_vMax.x += fLarge;
    if (m_iIndexY==m_Config.m_iSectorCount[1]-1) zoneBox.m_vMax.y += fLarge;

    m_spSectorZone->SetBoundingBox(zoneBox);
  }
}


const hkvAlignedBBox& VTerrainSector::GetVisZoneBoundingBox() const
{
  if (m_spSectorZone)
    return m_spSectorZone->GetBoundingBox();

  return GetBoundingBox();
}




void VTerrainSector::GetDefaultSectorBoundingBox(hkvAlignedBBox &destBox) const
{
  destBox.m_vMax = destBox.m_vMin = m_Config.GetSectorOrigin(m_iIndexX, m_iIndexY);
  destBox.m_vMax.x += m_Config.m_vSectorSize.x;
  destBox.m_vMax.y += m_Config.m_vSectorSize.y;

  const float fHOfs = m_Config.m_vTerrainPos.z;
  destBox.m_vMin.z = m_fMinHeightValue + fHOfs - 5.f;
  destBox.m_vMax.z = m_fMaxHeightValue + fHOfs + 5.f;
}

void VTerrainSector::GetDefaultTileBoundingBox(hkvAlignedBBox &destBox, int iTileX, int iTileY) const
{
  hkvVec3 vOrigin = GetSectorOrigin();
  destBox = GetBoundingBox(); // for min/max z
  destBox.m_vMin.x = vOrigin.x + m_Config.m_vTileSize.x * (float)iTileX;
  destBox.m_vMin.y = vOrigin.y + m_Config.m_vTileSize.y * (float)iTileY;
  destBox.m_vMax.x = destBox.m_vMin.x + m_Config.m_vTileSize.x;
  destBox.m_vMax.y = destBox.m_vMin.y + m_Config.m_vTileSize.y;
}


bool VTerrainSector::HasHoles() const
{
  VASSERT(m_pTile!=NULL);
  VSectorTile *pTile = m_pTile;
  for (int i=0;i<m_Config.m_iTilesPerSectorCount;i++,pTile++)
    if (pTile->IsHole()) 
      return true;
  return false;
}


bool VTerrainSector::IsAllHoles() const
{
  VASSERT(m_pTile!=NULL);
  VSectorTile *pTile = m_pTile;
  for (int i=0; i<m_Config.m_iTilesPerSectorCount; ++i, pTile++)
  {
    if (!pTile->IsHole()) 
      return false;
  }
  return true;
}


float VTerrainSector::GetHeightAtRelPos(const hkvVec3& vPos) const
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_HEIGHTLOOKUP);
  ((VTerrainSector *)this)->GetHeightmapValues();
  float fPosX = vPos.x * m_Config.m_vWorld2Sample.x;
  float fPosY = vPos.y * m_Config.m_vWorld2Sample.y;
  int x = (int)fPosX;
  int y = (int)fPosY;
  float h00 = GetHeightAt(x,y);
  float h01 = GetHeightAt(x,y+1);
  float h10 = GetHeightAt(x+1,y);
  float h11 = GetHeightAt(x+1,y+1);

  float fx1 = hkvMath::mod (fPosX,1.f);
  float fy1 = hkvMath::mod (fPosY,1.f);
  float fx0 = 1.f-fx1;
  float fy0 = 1.f-fy1;
  return (h00*fx0+h10*fx1)*fy0 + (h01*fx0+h11*fx1)*fy1;
}

int VTerrainSector::GetMaterialID(float fRelPosX, float fRelPosY)
{
  if (m_spMaterialIDMap==NULL)
    return -1;
  int x = (int)(fRelPosX*m_Config.m_vInvSectorSize.x*(float)m_spMaterialIDMap->m_iSizeX);
  int y = (int)(fRelPosY*m_Config.m_vInvSectorSize.y*(float)m_spMaterialIDMap->m_iSizeY);
  int iValue = m_spMaterialIDMap->GetValue(x,y);
  return iValue;
}



/*
bool VTerrainSector::CopyRelevantTextureData(VTerrainLockObject &destData)
{
EnsureLoaded();
const VSectorTextureInfo_t* pTemplate = (const VSectorTextureInfo_t *)destData.m_pReserved[0];
VASSERT(pTemplate);
// find the sampler instance of the same name
VSectorTextureInfo_t *pInstance = GetAuxiliaryTextureBySamplerName(pTemplate->m_szSamplerName);
if (!pInstance)
return false;
VisBitmap_cl *pBitmap = pInstance->GetBitmap();
if (!pBitmap)
return false;
VASSERT(pInstance->m_iSizeX==pTemplate->m_iSizeX && pInstance->m_iSizeY==pTemplate->m_iSizeY);

const int iOfsX = m_iIndexX*pInstance->m_iSizeX;
const int iOfsY = m_iIndexY*pInstance->m_iSizeY;

int x1 = destData.m_iPos[0] - iOfsX;
int y1 = destData.m_iPos[1] - iOfsY;
int x2 = destData.m_iPos[0]+destData.m_iSize[0] - iOfsX - 1;
int y2 = destData.m_iPos[1]+destData.m_iSize[1] - iOfsY - 1;

// completely outside?
if (x2<0 || y2<0 || x1>=pInstance->m_iSizeX || y1>=pInstance->m_iSizeY)
return false;

// clamp relevant part
VColorRef *pDest = (VColorRef *)destData.GetData();
if (x1<0) {pDest-=x1;x1=0;}
if (y1<0) {pDest-=y1*destData.m_iStride;y1=0;}
if (x2>=pInstance->m_iSizeX) x2=pInstance->m_iSizeX-1;
if (y2>=pInstance->m_iSizeY) y2=pInstance->m_iSizeY-1;
int iRowSize = (x2-x1+1)*sizeof(VColorRef);

VColorRef *pSrc = pBitmap->GetDataPtr(x1,y1);
for (int y=y1;y<=y2;y++,pSrc+=pInstance->m_iSizeX,pDest+=destData.m_iStride)
memcpy(pDest,pSrc,iRowSize);
return true;
}

bool VTerrainSector::GetRelevantTextureData(const VTerrainLockObject &srcData, bool bUpload)
{
EnsureLoaded();
const VSectorTextureInfo_t* pTemplate = (const VSectorTextureInfo_t *)srcData.m_pReserved[0];
VASSERT(pTemplate);
// find the sampler instance of the same name
VSectorTextureInfo_t *pInstance = VSectorTextureInfo_t::GetAuxiliaryTextureBySamplerName(
pTemplate->m_szSamplerName,m_pAuxiliaryTextures,m_iAuxiliaryTextureCount);
if (!pInstance)
return false;
VisBitmap_cl *pBitmap = pInstance->GetBitmap();
if (!pBitmap)
return false;

VASSERT(pInstance->m_iSizeX==pTemplate->m_iSizeX && pInstance->m_iSizeY==pTemplate->m_iSizeY);

const int iOfsX = m_iIndexX*pInstance->m_iSizeX;
const int iOfsY = m_iIndexY*pInstance->m_iSizeY;

int x1 = srcData.m_iPos[0] - iOfsX;
int y1 = srcData.m_iPos[1] - iOfsY;
int x2 = srcData.m_iPos[0]+srcData.m_iSize[0] - iOfsX - 1;
int y2 = srcData.m_iPos[1]+srcData.m_iSize[1] - iOfsY - 1;

// completely outside?
if (x2<0 || y2<0 || x1>=pInstance->m_iSizeX || y1>=pInstance->m_iSizeY)
return false;

// clamp relevant part
VColorRef *pSrc = (VColorRef *)srcData.GetData();
if (x1<0) {pSrc-=x1;x1=0;}
if (y1<0) {pSrc-=y1*srcData.m_iStride;y1=0;}
if (x2>=pInstance->m_iSizeX) x2=pInstance->m_iSizeX-1;
if (y2>=pInstance->m_iSizeY) y2=pInstance->m_iSizeY-1;
int iRowSize = (x2-x1+1)*sizeof(VColorRef);
VColorRef *pDest = pBitmap->GetDataPtr(x1,y1);

//copy row-wise
for (int y=y1;y<=y2;y++,pSrc+=srcData.m_iStride,pDest+=pInstance->m_iSizeX)
memcpy(pDest,pSrc,iRowSize);

// mark sector as dirty for swapping
SetEditorFlag(SECTOR_EDITORFLAG_MODIFIED);
pInstance->m_bModified = true;

if (bUpload)
pInstance->Upload();

return true;
}
*/


void VTerrainSector::AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
{
  VManagedResource::AccumulateMemoryFootprint(iUniqueSys,iUniqueGPU,iDependentSys,iDependentGPU);

  if (m_pHeight)
    iUniqueSys += m_iSampleStrideX*(m_Config.m_iHeightSamplesPerSector[1]+2)*sizeof(float);

  // tiles
  if (m_pTile)
    iUniqueSys += sizeof(VSectorTile)*m_Config.m_iTilesPerSectorCount;
  if (m_spMesh)
    m_spMesh->IncOverallMemSizes(iDependentSys,iDependentGPU);

  // decoration refs
  iUniqueSys += m_Decoration.m_iCount * sizeof(VTerrainDecorationInstance);
  iUniqueSys += m_iReferencedDecorationCount * sizeof(unsigned short);

  // textures
  if(VTerrainSectorMeshPageInfo *pPage = m_pMeshPage)
  {

    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        if (pPage->m_pSurface)
        {
          pPage->m_pSurface->AddMemFromUnTaggedTextures(iDependentSys,iDependentGPU);
        }
      }
    }
  }


  // other maps:
  if (m_spMaterialIDMap!=NULL)
    iUniqueSys += sizeof(VLuminanceChannel) + m_spMaterialIDMap->GetSampleCount();
}


void VTerrainSector::DebugRender()
{
  // bool bVisible = IsVisibleInThisFrame();
  // Vision::Message.Print(1,m_iIndexX*10+50,m_iIndexY*10+50,bVisible?"V":".");

  Vision::Game.DrawBoundingBox(GetBoundingBox(),V_RGBA_YELLOW,1.f);

  VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
  for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      Vision::Game.DrawBoundingBox(pPage->m_AbsBoundingBox,VColorRef(100,100,255,255));

}


void VTerrainSector::Render(VCompiledShaderPass *pShader,const VisStaticGeometryInstanceCollection_cl &allInstances, int iThisIndex, VPassType_e ePassType)
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_RENDERTERRAINMESHES);

  VisMeshBuffer_cl *pSharedMesh = GetSectorManager()->m_spTerrainIndexBuffer;
  unsigned int iSharedMeshStreams = GetSectorManager()->m_iIndexBufferStreamMask;

  if ((!pShader && ePassType != VPT_PrimaryOpaquePass) || m_spMesh==NULL) // render terrain only in primary opaque pass
    return;

  VTerrainSectorManager &sectorManager(*GetSectorManager());
  VASSERT(sectorManager.m_pRenderCollector);

  VTerrainVisibilityCollectorComponent &infoComp(*sectorManager.m_pRenderCollector);

  const bool bUseReplacement = infoComp.IsSectorReplacementVisible(VManagedResource::GetNumber());
  bool &bLastHasHoles = sectorManager.m_bLastMeshHasHoles;

  /*
  #ifdef HK_DEBUG_SLOW
  int iDebugState = infoComp.m_iDebugState;
  VASSERT_MSG(iDebugState==3,"Terrain visibility consistency check failed"); // EndVisibilityUpdate must be called
  #endif
  */

  // start a new group:
  if (iThisIndex==0 || allInstances.GetEntry(iThisIndex-1)->GetGeometryType()!=STATIC_GEOMETRY_TYPE_TERRAIN)
  {
    Vision::RenderLoopHelper.BeginMeshRendering();

    // bind the default streams that are shared
    Vision::RenderLoopHelper.SetMeshTransformationMatrix(hkvMat4::IdentityMatrix ());
    bLastHasHoles = false;

    if (sectorManager.m_bShaderUsesDirectionalLight)
    {
      // setup directional light
      VisRenderStates_cl::SetPSConstantBuffer(5, &sectorManager.m_DirLightConfig);
    }
  }

  // render this mesh only when it is visible. Everything else has to be setup otherwise
  if (infoComp.IsSectorVisible(VManagedResource::GetNumber()) && !bUseReplacement)
  {
    EnsureLoaded();
    VASSERT(IsLoaded() && m_spMesh);

    int iStartVertex = 0;
    int iStreamMask = m_spMesh->m_iMeshStreamMask & (~iSharedMeshStreams);
    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;

    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++,iStartVertex+=m_Config.m_iVerticesPerMesh)
      {
        if (infoComp.m_pPageLOD[pPage->m_iGlobalIndex]==(char)-1) // page not visible
          continue;

#if defined(_VR_DX11)
        VisRenderStates_cl::SetPSConstantBuffer(3,&pPage->m_SectorPSConstBuffer);
#else
        // bind individual streams and constants (only for some shaders)
        if ((pShader==NULL || (pShader->GetRenderState ()->GetTrackingMask () & VSHADER_TRACKING_USERBIT0)) && pPage->m_iPixelShaderRegCount>0) // ignore register 0 and 1
          VisRenderStates_cl::SetPixelShaderConstant(2, pPage->m_vPixelShaderConst[0].data, pPage->m_iPixelShaderRegCount);
#endif

        Vision::RenderLoopHelper.ResetMeshStreams();

        Vision::RenderLoopHelper.AddMeshStreams(pSharedMesh,iSharedMeshStreams);

        if (pPage->m_bHasHoles && !sectorManager.GetUseExportRendering()) // either use custom or shared index buffer
        {
          Vision::RenderLoopHelper.AddMeshStreamsEx(pPage->m_spHoleIndexBuffer, VERTEX_STREAM_INDEXBUFFER);
          bLastHasHoles = true;
        } 
        else if (bLastHasHoles)
        {
          Vision::RenderLoopHelper.AddMeshStreamsEx(pSharedMesh, VERTEX_STREAM_INDEXBUFFER);
          bLastHasHoles = false;
        }
        VASSERT(infoComp.m_pPageRenderRange[pPage->m_iGlobalIndex]);

        VASSERT(m_spMesh && m_spMesh->IsLoaded());
        VASSERT(m_spMesh->GetVertexBuffer());
        VASSERT(m_spMesh->GetVertexBuffer()->IsLoaded());
        Vision::RenderLoopHelper.AddMeshStreamsEx(m_spMesh, iStreamMask, iStartVertex, 0);

        // individual vertex shader const
#if defined(_VR_DX11)
        VisRenderStates_cl::SetVSConstantBuffer(3,&pPage->m_PageVSConstBuffer);
#else
        VisRenderStates_cl::SetVertexShaderConstant(38, pPage->m_vVertexShaderConst[0].data, sizeof(pPage->m_vVertexShaderConst)/sizeof(hkvVec4));
#endif

        if (pShader)
        {
          if (pShader->GetRenderState ()->IsBlendFromSurfaceEnabled ())
            VisRenderStates_cl::SetBlendState(*VisRenderStates_cl::GetBlendStateGroup(VIS_TRANSP_NONE));
          if (pShader->GetRenderState ()->IsRasterizerStyleFromSurfaceEnabled ())
            VisRenderStates_cl::SetRasterizerState(*VisRenderStates_cl::GetRasterizerStateGroup(VIS_RASTERIZER_CULL_BACK));

          // bind shader specific textures
          pPage->BindSectorShaderTextures(pShader, pPage->m_iSamplerClampMask);
          const VSectorMeshRenderRange_t &range(*infoComp.m_pPageRenderRange[pPage->m_iGlobalIndex]);

          Vision::RenderLoopHelper.RenderMeshes(pShader,
            VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST,
            range.m_iFirstPrim, range.m_iPrimCount,
            m_Config.m_iVerticesPerMesh);
        }
        else
        {
          RenderTerrainMesh(pPage,infoComp);
        }
      }
  }

  //DebugRender(); // remove

  // end terrain group
  if (iThisIndex==allInstances.GetNumEntries()-1 || allInstances.GetEntry(iThisIndex+1)->GetGeometryType()!=STATIC_GEOMETRY_TYPE_TERRAIN)
  {
    Vision::RenderLoopHelper.EndMeshRendering();
  }
}

void VTerrainSector::RenderTerrainMesh(VTerrainSectorMeshPageInfo* pPage, const VTerrainVisibilityCollectorComponent &visInfoComp)
{
  const VSectorMeshRenderRange_t &lod(*visInfoComp.m_pPageRenderRange[pPage->m_iGlobalIndex]);
  VCompiledTechnique *pTech = pPage->GetSurfaceSafe().GetTechnique();
  const int iPasses = pTech->GetShaderCount();
  const int iVertexCount = m_Config.m_iVerticesPerMesh; //  m_spMesh->GetVertexCount()
  for (int iPass=0;iPass<iPasses;iPass++)
  {
    VCompiledShaderPass *pShader = pTech->GetShader(iPass);
    pPage->BindSectorShaderTextures(pShader,pPage->m_iSamplerClampMask);

    // render
    Vision::RenderLoopHelper.RenderMeshes(pShader,
      VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST,
      lod.m_iFirstPrim,lod.m_iPrimCount,iVertexCount);
  }
}

float GetXYDistanceToAABB (const hkvAlignedBBox& aabb, const hkvVec3& vPos);

void VTerrainSector::PerformVisibility(VTerrainVisibilityCollectorComponent &infoComp)
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_VISIBILITY_OVERALL);

  VTerrainVisibilityInfo &info(infoComp.m_VisibilityInfo);

  const bool bSectorGeomVisible = info.m_pCamFrustum->Overlaps(m_BoundingBox);
  float fDistanceToThisSector = GetXYDistanceToAABB (m_BoundingBox, info.m_vCamLODPos);
  const bool bUseReplacement = bSectorGeomVisible && m_Config.m_bSupportsReplacement && fDistanceToThisSector>=m_Config.m_fReplacementDistance;

  VMutex &mutex(GetSectorManager()->m_VisibilityMutex); // global visibility mutex for critical operations

  { // profiling block
    VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_VISIBILITY_SECTORVISIBILITY);

    mutex.Lock();
    info.m_iVisibleSectorRange[0] = hkvMath::Min (info.m_iVisibleSectorRange[0],m_iIndexX);
    info.m_iVisibleSectorRange[1] = hkvMath::Min (info.m_iVisibleSectorRange[1],m_iIndexY);
    info.m_iVisibleSectorRange[2] = hkvMath::Max (info.m_iVisibleSectorRange[2],m_iIndexX);
    info.m_iVisibleSectorRange[3] = hkvMath::Max (info.m_iVisibleSectorRange[3],m_iIndexY);
    mutex.Unlock();

    // mark this sector as visible
    if (bSectorGeomVisible)
    {
      infoComp.m_SectorVisible.SetBit(VManagedResource::GetNumber());

      // mesh pages + LOD
      if (bUseReplacement) // far away?
      {
        infoComp.m_SectorReplacementVisible.SetBit(VManagedResource::GetNumber());
        infoComp.m_bHasVisibleReplacementMeshes = true;
        // since we don't have pages yet in replacement mode, we use page index directly
        for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
          for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++)
          {
            int iPageIndex = GetGlobalPageIndex(iPageX,iPageY);
            infoComp.m_pPageLOD[iPageIndex] = PAGE_LOD_FORCEFIXED | m_Config.m_iReplacementLOD;
          }
      } else
      {
        VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
        for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
          for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
            pPage->PerformVisibility(infoComp,m_Config);
      }
    }


    // calculate visibility of each tile. Since tiles might be larger than geometry at the edge, test for bSectorGeomVisible==false as well
    fDistanceToThisSector *= info.m_fLODDistanceInvScale;
    if (info.m_fLODDistanceScale>0.f && fDistanceToThisSector < m_fMaxTileHandlingDistance)
    {
      info.m_bAddDecoration = !m_bHasAdditionalDecoration; // in this mode, only render the custom array, see below

      VTerrainSectorDecorationVisibilityMask *pDecoVisMask = NULL;
      if (m_Decoration.m_iCount>0)
      {
        // the following operation must be thread-safe since it adds something to the list
        mutex.Lock();
        pDecoVisMask = VTerrainSectorDecorationVisibilityMask::GetForCollector(m_pFirstDecoVisInfo,&infoComp,m_Decoration.m_iCount);
        mutex.Unlock();
        VASSERT(pDecoVisMask);
        VSectorTile *pTile = m_pTile;
        for (int i=0;i<m_Config.m_iTilesPerSectorCount;i++,pTile++)
        {
          if (pTile->m_iReferencedDecorationCount==0 || !info.m_pCamFrustum->Overlaps(pTile->m_TileBBox))
            continue;
          pTile->PerformVisibility(infoComp, pDecoVisMask);
        }
      }
    }

    // in editor mode, there might be additional decoration to render, i.e. while painting:
#ifdef WIN32
    if (m_bHasAdditionalDecoration)
    {
      int iAdditionalCount;
      VTerrainDecorationInstance* pDecorationInst = GetAdditionalDecoration(iAdditionalCount);
      info.m_iEstimatedDecorationCount += iAdditionalCount; // the actual instances are added in EndVisibility
    }
#endif
  }
}


struct TerrainVertex_t
{
  float fHeight;        // position z value
};



void VTerrainSector::UpdateMesh(bool bForceAllPages)
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_UPDATE_MESH);

  const bool bFastUpdate = GetSectorManager()->m_pTerrain->GetFastUpdate();
  if (!bFastUpdate)
    bForceAllPages = true;

  GetHeightmapValues();
  VASSERT(m_spMesh && m_pHeight);

  const VTerrainConfig &config = m_Config;
  const int iMeshSamplesX = m_Config.m_iHeightSamplesPerSector[0]/config.m_iSectorMeshesPerSector[0];
  const int iMeshSamplesY = m_Config.m_iHeightSamplesPerSector[1]/config.m_iSectorMeshesPerSector[1];
  const int iVerticesPerPage = (iMeshSamplesX+1)*(iMeshSamplesY+1);
  const int iReqVertCount = iVerticesPerPage * m_Config.m_iSectorMeshCount;
  VisMBVertexDescriptor_t desc;
  
  if (m_spMesh->GetVertexCount()<iReqVertCount || (bFastUpdate!=m_spMesh->GetVertexBuffer()->HasSysmemCopy()))
  {
    ANALYSIS_IGNORE_WARNING_ONCE(6246)
      VisMBVertexDescriptor_t desc;
    desc.m_iStride = sizeof(TerrainVertex_t);
    desc.m_iTexCoordOfs[0] = offsetof(TerrainVertex_t,fHeight) | VERTEXDESC_FORMAT_FLOAT;
    bool bDoubleBuffered = bFastUpdate;
    m_spMesh->AllocateVertices(desc, iReqVertCount, bFastUpdate ? VIS_MEMUSAGE_STATIC : VIS_MEMUSAGE_STATIC, bDoubleBuffered);
    m_spMesh->m_iMeshStreamMask = m_spMesh->GetStreamMask();
    m_spMesh->GetVertexBuffer()->RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
    bForceAllPages = true;
  }

  int x,y;

  // generate vertices
  VMemoryTempBuffer<sizeof(TerrainVertex_t)*129*129> tempVertices;
  TerrainVertex_t *pVert = NULL;

  if (!bFastUpdate)
  {
    tempVertices.EnsureCapacity(iReqVertCount*sizeof(TerrainVertex_t));
    pVert = (TerrainVertex_t *)tempVertices.GetBuffer();
  }

  m_fMinHeightValue = m_pHeight[0];
  m_fMaxHeightValue = m_pHeight[0];
  const float fHOfs = m_Config.m_vTerrainPos.z;

  int iFirstVertex = 0;
  VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
  for (int iPageY=0;iPageY<config.m_iSectorMeshesPerSector[1];iPageY++)
    for (int iPageX=0;iPageX<config.m_iSectorMeshesPerSector[0];iPageX++,pPage++,iFirstVertex+=iVerticesPerPage)
    {
      if (!bForceAllPages && (pPage->m_iDirtyFlags&MESHPAGEFLAG_HEIGHTMAP_DIRTY)==0)
        continue;
      if (bFastUpdate)
        pVert = (TerrainVertex_t *)m_spMesh->LockVertices(VIS_LOCKFLAG_NOOVERWRITE|VIS_LOCKFLAG_DISCARDABLE, iFirstVertex, iVerticesPerPage);

      pPage->m_iDirtyFlags &= ~MESHPAGEFLAG_HEIGHTMAP_DIRTY;
      pPage->m_AbsBoundingBox.m_vMin.z = 10000000.f;
      pPage->m_AbsBoundingBox.m_vMax.z = -10000000.f;
      for (y=0;y<=iMeshSamplesY;y++)
      {
        float *pHVal = &m_pHeight[m_iSampleStrideX*(y+iPageY*iMeshSamplesY) + iPageX*iMeshSamplesX];
        for (x=0;x<=iMeshSamplesX;x++, pVert++, pHVal++)
        {
          float h = *pHVal + fHOfs;
          pVert->fHeight = h;
          m_fMinHeightValue = hkvMath::Min(m_fMinHeightValue,*pHVal); // use h here?
          m_fMaxHeightValue = hkvMath::Max(m_fMaxHeightValue,*pHVal);
          pPage->m_AbsBoundingBox.m_vMin.z = hkvMath::Min(pPage->m_AbsBoundingBox.m_vMin.z,h);
          pPage->m_AbsBoundingBox.m_vMax.z = hkvMath::Max(pPage->m_AbsBoundingBox.m_vMax.z,h);
        }
      }

      if (bFastUpdate)
        m_spMesh->UnLockVertices();
    }

  { // upload vertex data
    VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_UPLOAD_MESH);
    if (!bFastUpdate)
      m_spMesh->FillVerticesWithData(tempVertices.GetBuffer(),iReqVertCount*sizeof(TerrainVertex_t));
  }

  VTerrain::VTerrainLODMode_e lod = GetSectorManager()->m_pTerrain->GetLODMetric();
  if (lod==VTerrain::VLODMODE_NOISE_AND_DISTANCE || lod==VTerrain::VLODMODE_NOISE_AND_DISTANCE_XY)
    ComputeLODDistanceTable();
    UpdateMemoryFootprint();
}


VSimpleCollisionMesh* VTerrainSector::GetCollisionMesh(int iTileX, int iTileY)
{
  // create a resource that keeps all collision meshes in this sector
  if (!m_spCollisionMeshes)
    m_spCollisionMeshes = new VSectorCollisionMeshes(this);
  V_LOCK_RESOURCE(m_spCollisionMeshes);

  VSimpleCollisionMesh* pMesh = m_spCollisionMeshes->GetTileMesh(iTileX,iTileY);

  // collision mesh yet built? (we set the primitive count to 0 to force an update, so no new vertex count is allocated)
  if (!pMesh->GetPrimitiveCount())
  {
    int x,y;
    const int iDetail = 1 << GetSectorManager()->m_iPickingMeshDetail;
    const int iCountX = m_Config.m_iHeightSamplesPerSector[0]/(m_Config.m_iTilesPerSector[0]*iDetail);
    const int iCountY = m_Config.m_iHeightSamplesPerSector[1]/(m_Config.m_iTilesPerSector[1]*iDetail);
    pMesh->AllocateVertices((iCountX+1)*(iCountY+1));
    pMesh->AllocateIndices(iCountX*iCountY*6);
    pMesh->SetPrimitiveCount(iCountX*iCountY*2);
    const float *pHeight = GetHeightmapValues();
    hkvVec3 vPos;
    int iIndex = 0;
    const int y1 = iCountY*iTileY;
    const int y2 = y1+iCountY;
    const int x1 = iCountX*iTileX;
    const int x2 = x1+iCountX;
    const float fStepX = m_Config.m_vSampleSpacing.x*(float)iDetail;
    const float fStepY = m_Config.m_vSampleSpacing.y*(float)iDetail;
#ifdef TERRAIN_COLLISIONMESHES_SECTORLOCALSPACE
    hkvVec3 vPosOfs(0.f,0.f,m_Config.m_vTerrainPos.z);
#else
    hkvVec3 vPosOfs = m_vSectorOrigin;
#endif

    // create vertices
    for (y=y1;y<=y2;y++)
    {
      const float *pHVal = &pHeight[m_iSampleStrideX*(y*iDetail) + x1*iDetail];
      vPos.x = (float)x1*fStepX;
      vPos.y = (float)y*fStepY;
      for (x=x1;x<=x2;x++,iIndex++,pHVal+=iDetail,vPos.x+=fStepX)
      {
        vPos.z = *pHVal;
        pMesh->SetVertex(iIndex,vPos+vPosOfs);
      }
    }

    // create indices
    unsigned short* pIndex = pMesh->GetIndexPtr();
    int iRow = 0;
    const int iRowStride = iCountX+1;
    ANALYSIS_IGNORE_WARNING_ONCE(6246)
      for (int y=0;y<iCountY;y++,iRow+=iRowStride)
      {
        ANALYSIS_IGNORE_WARNING_ONCE(6246)
          for (int x=0;x<iCountX;x++,pIndex+=6)
          {
            const int iPos = iRow+x;
            pIndex[2] = iPos;
            pIndex[1] = iPos+1;
            pIndex[0] = iPos+iRowStride+1;
            pIndex[5] = iPos;
            pIndex[4] = iPos+1+iRowStride;
            pIndex[3] = iPos+iRowStride;
          }
      }

      // calculate bounding box
      const hkvAlignedBBox* pBBox = pMesh->GetBoundingBox();

      pMesh->ComputeTrianglePlanes();

      // new memory size for resource viewer
      m_spCollisionMeshes->EvaluateMemSize();
  }
  return pMesh;
}

int VTerrainSector::GetRelevantCollisionMeshesInBoundingBox(const VLargeBoundingBox &bbox, DynArray_cl<VSimpleCollisionMesh *> &meshes, int iFirst)
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_TRACING_GETRELEVANTMESHES);
  int iRelevantCount = 0;
  int x1 = GetTileIndexX(bbox.m_vMin);
  int y1 = GetTileIndexY(bbox.m_vMin);
  int x2 = GetTileIndexX(bbox.m_vMax);
  int y2 = GetTileIndexY(bbox.m_vMax);

  // completely outside?
  if (x1>=m_Config.m_iTilesPerSector[0] || y1>=m_Config.m_iTilesPerSector[1]) return 0;
  if (x2<0 || y2<0) return 0;

  //clamp
  if (x1<0) x1=0;
  if (y1<0) y1=0;
  if (x2>=m_Config.m_iTilesPerSector[0]) x2=m_Config.m_iTilesPerSector[0]-1;
  if (y2>=m_Config.m_iTilesPerSector[1]) y2=m_Config.m_iTilesPerSector[1]-1;

  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++,iRelevantCount++)
      meshes[iFirst+iRelevantCount] = GetCollisionMesh(x,y);

  return iRelevantCount;
}


int VTerrainSector::TraceTest(const hkvVec3& startOfLine, const hkvVec3& endOfLine, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo)
{
  hkvAlignedBBox rayBox(startOfLine,startOfLine);
  rayBox.expandToInclude(endOfLine);
  const hkvAlignedBBox &sectorBox = GetBoundingBox();
  if (!sectorBox.overlaps(rayBox) || (!sectorBox.contains (startOfLine) && !sectorBox.getLineSegmentIntersection(startOfLine,endOfLine)))
    return 0;

  int x1 = WORLDX_2_TILE(rayBox.m_vMin.x);
  int y1 = WORLDY_2_TILE(rayBox.m_vMin.y);
  int x2 = WORLDX_2_TILE(rayBox.m_vMax.x);
  int y2 = WORLDY_2_TILE(rayBox.m_vMax.y);

  // transformed start/end since collision meshes are sector relative
#ifdef TERRAIN_COLLISIONMESHES_SECTORLOCALSPACE
  hkvVec3 vStart = startOfLine - m_vSectorOrigin;
  hkvVec3 vEnd = endOfLine - m_vSectorOrigin;
#else
  hkvVec3 vStart = startOfLine;
  hkvVec3 vEnd = endOfLine;
#endif

  // completely outside?
  if (x1>=m_Config.m_iTilesPerSector[0] || y1>=m_Config.m_iTilesPerSector[1]) return 0;
  if (x2<0 || y2<0) return 0;

  //clamp
  if (x1<0) x1=0;
  if (y1<0) y1=0;
  if (x2>=m_Config.m_iTilesPerSector[0]) x2=m_Config.m_iTilesPerSector[0]-1;
  if (y2>=m_Config.m_iTilesPerSector[1]) y2=m_Config.m_iTilesPerSector[1]-1;

  hkvVec3 vDir = vEnd-vStart;
  float fLen = vDir.getLength();
  if (fLen<=HKVMATH_LARGE_EPSILON) return 0;
  vDir *= (1.f/fLen);

  VTraceHitInfo hitInfo;
  VTraceHitInfo *pHitInfo = iStoreResultCount>0 ? &hitInfo : NULL;

  // this loop could be optimized...
  int iHitCount = 0;
  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
    {
      VSimpleCollisionMesh* pMesh = GetCollisionMesh(x,y);
      const hkvAlignedBBox* pBBox = pMesh->GetBoundingBox();

      //pMesh->Render(Vision::Contexts.GetMainRenderContext()->GetRenderInterface(), V_RGBA_YELLOW);

      // performs bbox early out.
      // This does however not cover multiple hits per single tile yet...
      // We need the TRACEFLAG_DOUBLESIDED flag as the vertex order is flipped (physics requirement) 
      if (!pMesh->GetTraceIntersection((hkvVec3& )vStart,(hkvVec3& )vEnd,TRACEFLAG_DOUBLESIDED,pHitInfo))
        continue;

      // we have a hit
      if (iStoreResultCount<1)
        return 1;

      // better than any of the current ones?
      hitInfo.m_fDistance *= fLen;
      VisTraceLineInfo_t *pStoreInfo = VisTraceLineInfo_t::InsertTraceResult(iStoreResultCount,pFirstTraceInfo,hitInfo.m_fDistance);
      if (!pStoreInfo)
        continue;

      pStoreInfo->detected = TRUE;
      pStoreInfo->hitType = VIS_TRACETYPE_STATICGEOMETRY;
      pStoreInfo->sceneElementType = V_COLMESH_SCENEELEMENT_TERRAIN; // actually it is VIS_PRIMITIVETYPE_STATICMESH or VIS_PRIMITIVETYPE_CUSTOM
      pStoreInfo->colliderEntity = NULL;
      pStoreInfo->pCollisionMesh = NULL;
      pStoreInfo->distance = hitInfo.m_fDistance;
      pStoreInfo->touchPoint = hitInfo.m_vTouchPoint;
#ifdef TERRAIN_COLLISIONMESHES_SECTORLOCALSPACE
      pStoreInfo->touchPoint += m_vSectorOrigin; // local space collision mesh
#endif
      pStoreInfo->primitiveNumber = (int)GetTile(x,y);
      pStoreInfo->pSurface = &m_pMeshPage[0].GetSurfaceSafe();
      pStoreInfo->primitivePlane = hitInfo.m_MeshTriangle.GetPlane ();
      pStoreInfo->primitiveVertex = hitInfo.m_MeshTriangle.GetVertex0();
      //  pStoreInfo->baseUV; ? 
      pStoreInfo->pGeoInstance = this;
      pStoreInfo->pSubmesh = NULL;
      iHitCount++;

    }

    return iHitCount;
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

class FixCacheDirRefs : public IVFilePathResolver
{
public:
  FixCacheDirRefs(VTerrainSector &sector) : m_Sector(sector)
  {
  }

  virtual const char *ResolvePath(const char *szPath, char *szBuffer) HKV_OVERRIDE
  {
    if (!szPath || !szPath[0])
      return NULL;
    const char *szNewFile = m_Sector.m_Config.FixTempDirFile(szPath,szBuffer);
    VASSERT(!m_Sector.m_Config.IsTempDirFile(szNewFile));
    return szNewFile;
  }

  VTerrainSector &m_Sector;
};


#endif

//Used by final save to save edited or swapped sectors over final sectors and to genenerate the resource snapshot
bool VTerrainSector::SaveSectorFinal(bool bSaveSnapshot)
{
  bool bRes = true;
  if (IsLoaded())
  {
    bRes = SaveSector(false);  //always save to final directory

#ifdef SUPPORTS_SNAPSHOT_CREATION
    char szFilename[FS_MAX_PATH];
    // save resource.xml file
    if (bSaveSnapshot && m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"vres",true))
    {
      FixCacheDirRefs resolver(*this);
      VResourceSnapshot snapshot;
      GetDependencies(snapshot);
      m_Config.RCSPerformAction(szFilename, RCS_EDIT); //make sure we can write to it
      snapshot.SaveToBinaryFile(szFilename, Vision::File.GetOutputManager(),&resolver);
      m_Config.RCSPerformAction(szFilename, RCS_ADD);
    }
#endif  
  }
  return bRes;
}


VisZoneResource_cl *VTerrainSector::GetParentZone()
{
  return GetSectorManager()->m_pTerrain->GetParentZone();
}

int VTerrainSector::GetNumPrimitives()
{
  return GetSectorManager()->m_LODInfo[0].m_EdgeInfo[0][0][0][0].m_iPrimCount;
}

int VTerrainSector::GetSupportedStreamMask()
{
  int iMask = GetSectorManager()->m_iIndexBufferStreamMask;
  if (m_spMesh)
    iMask |= m_spMesh->m_iMeshStreamMask;
  return iMask;
}

void VTerrainSector::RemoveDecorationInstances(const VTBitfield<1024> &removeIndices)
{
  EnsureLoaded();
  VSectorTile *pTile = m_pTile;
  for (int i=0;i<m_Config.m_iTilesPerSectorCount;i++,pTile++)
    pTile->RemoveDecorationInstanceReferences(removeIndices);
}

void VTerrainSector::RemoveDecorationInstances(const VLargeBoundingBox &bbox)
{
  EnsureLoaded();
  int x1,y1,x2,y2;
  GetTileIndices_Clamped(bbox.m_vMin,x1,y1);
  GetTileIndices_Clamped(bbox.m_vMax,x2,y2);

  const int iDecoCount = m_Decoration.GetDecorationObjectCount();
  VTBitfield<1024> removeIndices(iDecoCount);
  hkvVec3 instancePos(hkvNoInitialization);
  hkvAlignedBBox renderBox = bbox.ToRenderSpace(m_Config);
  int iRemoveCount = 0;
  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
    {
      VSectorTile *pTile = GetTile(x,y);
      for (int i=0;i<pTile->m_iReferencedDecorationCount;i++)
      {
        int iIndex = m_piReferencedDecoration[pTile->m_iFirstReferencedDecoration+i];
        if (removeIndices.IsBitSet(iIndex)) // already checked for removal
          continue;
        m_Decoration.GetDecorationObject(iIndex)->GetPosition(instancePos);
        //Vision::Game.DrawCube(instancePos,10.f);
        if (!renderBox.contains(instancePos))
          continue;
        iRemoveCount++;
        removeIndices.SetBit(iIndex);
      }
    }
    if (iRemoveCount>0)
      RemoveDecorationInstances(removeIndices);
}

VCompiledTechnique* VTerrainSector::GetReplacementTechnique()
{
  if (!m_spTerrainReplacementTechnique)
  {
    // create terrain shader FX
    BOOL bResult = Vision::Shaders.LoadShaderLibrary("\\Shaders\\Terrain.ShaderLib",SHADERLIBFLAG_HIDDEN)!=NULL;
    m_spTerrainReplacementTechnique = Vision::Shaders.CreateTechnique("TerrainReplacement",NULL);
    VASSERT(bResult && m_spTerrainReplacementTechnique && "failed to create terrain replacement effect");
  }

  return m_spTerrainReplacementTechnique;
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
