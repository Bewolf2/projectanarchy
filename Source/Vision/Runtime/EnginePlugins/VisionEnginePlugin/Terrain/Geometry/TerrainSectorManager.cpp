/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/SectorTile.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>
#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
#endif





int VTerrainSectorManager::PROFILING_UNLOAD       = 0;
int VTerrainSectorManager::PROFILING_RELOAD       = 0;
int VTerrainSectorManager::PROFILING_STREAMING       = 0;
int VTerrainSectorManager::PROFILING_HEIGHTLOOKUP       = 0;
int VTerrainSectorManager::PROFILING_UPDATE_MESH        = 0;
int VTerrainSectorManager::PROFILING_UPLOAD_MESH        = 0;
int VTerrainSectorManager::PROFILING_UPDATE_NORMALMAP   = 0;
int VTerrainSectorManager::PROFILING_UPLOAD_NORMALMAP   = 0;
int VTerrainSectorManager::PROFILING_TRACING            = 0;
int VTerrainSectorManager::PROFILING_TRACING_GETRELEVANTMESHES = 0;
int VTerrainSectorManager::PROFILING_RENDERTERRAINMESHES  = 0;
int VTerrainSectorManager::PROFILING_RENDERTERRAINREPLACEMENTMESHES  = 0;
int VTerrainSectorManager::PROFILING_RENDERDECORATION     = 0;
int VTerrainSectorManager::PROFILING_RENDERDECORARION_INSTANCE_SETUP = 0;
int VTerrainSectorManager::PROFILING_VISIBILITY_OVERALL = 0;
int VTerrainSectorManager::PROFILING_VISIBILITY_SECTORVISIBILITY = 0;
int VTerrainSectorManager::PROFILING_VISIBILITY_TILEVISIBILITY = 0;
int VTerrainSectorManager::PROFILING_VISIBILITY_STARTVISIBILITY = 0;
int VTerrainSectorManager::PROFILING_VISIBILITY_ENDVISIBILITY = 0;

float VTerrainSectorManager::g_fDecorationDistanceScaling = 1.f;
float VTerrainSectorManager::g_fDecorationDistanceInvScaling = 1.f;


void VTerrainSectorManager::InitProfiling()
{
  VASSERT(PROFILING_VISIBILITY_OVERALL==0);
  PROFILING_HEIGHTLOOKUP = Vision::Profiling.GetFreeElementID();
  PROFILING_UPDATE_MESH = Vision::Profiling.GetFreeElementID();
  PROFILING_UPLOAD_MESH = Vision::Profiling.GetFreeElementID();
  PROFILING_UPDATE_NORMALMAP = Vision::Profiling.GetFreeElementID();
  PROFILING_UPLOAD_NORMALMAP = Vision::Profiling.GetFreeElementID();
  PROFILING_TRACING = Vision::Profiling.GetFreeElementID();
  PROFILING_TRACING_GETRELEVANTMESHES = Vision::Profiling.GetFreeElementID();
  PROFILING_RENDERTERRAINMESHES  = Vision::Profiling.GetFreeElementID();
  PROFILING_RENDERTERRAINREPLACEMENTMESHES = Vision::Profiling.GetFreeElementID();
  PROFILING_RENDERDECORATION     = Vision::Profiling.GetFreeElementID();
  PROFILING_RENDERDECORARION_INSTANCE_SETUP = Vision::Profiling.GetFreeElementID();
  PROFILING_VISIBILITY_OVERALL = Vision::Profiling.GetFreeElementID();
  PROFILING_VISIBILITY_SECTORVISIBILITY = Vision::Profiling.GetFreeElementID();
  PROFILING_VISIBILITY_TILEVISIBILITY = Vision::Profiling.GetFreeElementID();
  PROFILING_VISIBILITY_STARTVISIBILITY = Vision::Profiling.GetFreeElementID();
  PROFILING_VISIBILITY_ENDVISIBILITY = Vision::Profiling.GetFreeElementID();
  PROFILING_UNLOAD = Vision::Profiling.GetFreeElementID();
  PROFILING_RELOAD = Vision::Profiling.GetFreeElementID();
  PROFILING_STREAMING = Vision::Profiling.GetFreeElementID();
  Vision::Profiling.AddGroup("Terrain");
  Vision::Profiling.AddElement(PROFILING_RELOAD,          "Sector: Reload",TRUE);
  Vision::Profiling.AddElement(PROFILING_UNLOAD,          "Sector: Unload",TRUE);
  Vision::Profiling.AddElement(PROFILING_STREAMING,       "Sector: Caching",TRUE);
  VProfilingNode *pVisibility = Vision::Profiling.AddElement(PROFILING_VISIBILITY_OVERALL,    "Terrain visibility overall",TRUE);
  Vision::Profiling.AddElement(PROFILING_VISIBILITY_STARTVISIBILITY,  "Start visibility",TRUE, pVisibility);
  Vision::Profiling.AddElement(PROFILING_VISIBILITY_SECTORVISIBILITY, "Sector visibility",TRUE, pVisibility);
  Vision::Profiling.AddElement(PROFILING_VISIBILITY_TILEVISIBILITY,   "Tile visibility",TRUE, pVisibility);
  Vision::Profiling.AddElement(PROFILING_VISIBILITY_ENDVISIBILITY,    "End visibility",TRUE, pVisibility);
  Vision::Profiling.AddElement(PROFILING_HEIGHTLOOKUP,          "Height lookups",TRUE);
  VProfilingNode *pUpdateMesh = Vision::Profiling.AddElement(PROFILING_UPDATE_MESH,           "Update mesh",TRUE);
  Vision::Profiling.AddElement(PROFILING_UPLOAD_MESH,           "Upload mesh",TRUE,pUpdateMesh);
  VProfilingNode *pNrml = Vision::Profiling.AddElement(PROFILING_UPDATE_NORMALMAP,           "Update Normalmap",TRUE);
  Vision::Profiling.AddElement(PROFILING_UPLOAD_NORMALMAP,           "Upload Normalmap",TRUE, pNrml);
  Vision::Profiling.AddElement(PROFILING_RENDERTERRAINMESHES,   "Rendering: terrain meshes",TRUE);
  Vision::Profiling.AddElement(PROFILING_RENDERTERRAINREPLACEMENTMESHES,   "Rendering: replacement meshes",TRUE);
  Vision::Profiling.AddElement(PROFILING_RENDERDECORATION,      "Rendering: decoration",TRUE);
  Vision::Profiling.AddElement(PROFILING_RENDERDECORARION_INSTANCE_SETUP, "Rendering: decoration fill instances streams",TRUE);
  Vision::Profiling.AddElement(PROFILING_TRACING,               "Tracing (overall)",TRUE);
  Vision::Profiling.AddElement(PROFILING_TRACING_GETRELEVANTMESHES,"  Tracing (get relevant meshes)",TRUE);
}


VTerrainSectorManager::VTerrainSectorManager(VTerrainConfig &config)
: VisResourceManager_cl("TerrainSectors",VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER, 0),
  m_Config(config)
{
  m_iSectorCount = 0;
  m_pSector = NULL;
  m_iVisibleBitmask = 0xffffffff;
  m_fLODScaling = 1.0f;
  m_fMirrorLODBias = 1.0f; // one level down
  m_bLastMeshHasHoles = false;
  m_bForceFullLOD = false;
  m_pRenderCollector = NULL;
  m_iPickingMeshDetail = 2; // use only every 4th vertex for picking

  memset(m_fSphereConstraintRadius,0,sizeof(m_fSphereConstraintRadius));
  m_bSphereConstraintConstantsDirty = true;

  SetAllowPurging(true);//!Vision::Editor.IsInEditor(); // in editor mode, don't purge in-between

  m_iIndexBufferStreamMask = 0;

  // Init fallback lightgrid colors
  hkvVec3 lightDir(1.f,1.f,1.f);
  lightDir.normalizeIfNotZero();
  VLightGrid_cl::GetDirectionalLightColors(m_DecorationLightgridColors,lightDir,hkvVec3(1.f,0.8f,0.8f),hkvVec3(0.2f,0.3f,0.4f));
  m_bNeedsLightgridUpdate = true;

  m_bShaderUsesDirectionalLight = false;
#ifndef SUPPORTS_SHADER_CONSTANT_BUFFERS
  m_DirLightConfig.m_iFirstRegister = 26;
#endif
  VTerrainDirectionalLightConfig &dir(m_DirLightConfig.BeginUpdate());
    dir.m_vDirectionalLightColor.set(1.f,1.f,1.f,1.0f);
    dir.m_vAmbientColor.set(0.2f,0.21f,0.24f, 1.0f);
    dir.m_vLightDirection.set(1.f,1.f,-1.f,0.f);
    dir.m_vLightDirection.normalizeIfNotZero();
  m_DirLightConfig.EndUpdate();

  m_bTerrainMeshExport = false;
  m_bExportRendering = false;
}

VTerrainSectorManager::~VTerrainSectorManager()
{
  // Unset PS Constant buffer used by the terrain rendering.
  VisRenderStates_cl::SetPSConstantBuffer(3, NULL);
  VisRenderStates_cl::SetPSConstantBuffer(5, NULL);
  VASSERT(m_iSectorCount==0);
}

void VTerrainSectorManager::SetAllowPurging(bool bStatus)
{
  if (bStatus && m_Config.m_fPurgeInterval>0.f)
  {
    SetAutoUnloadInterval(m_Config.m_fPurgeInterval*0.25f,m_Config.m_fPurgeInterval, false);
    int iLimit = m_Config.m_iMemoryLimit*1024*1024; // megabytes to bytes
    VASSERT(iLimit>=0);
    SetMemoryLimit(iLimit, VRESOURCEMANAGERFLAG_MEMLIMIT_SOFT);
  }
  else
  {
    SetAutoUnloadInterval(-1.f,-1.f);
    SetMemoryLimit(0);
  }
}

void VTerrainSectorManager::InitTerrain()
{
  VASSERT(m_Config.IsValid());

  // create all sector instances
  m_iSectorCount = m_Config.m_iSectorCount[0] * m_Config.m_iSectorCount[1];
  m_pSector = new VTerrainSectorPtr[m_iSectorCount];

  int iIndex = 0;
  int x,y;
  for (y=0;y<m_Config.m_iSectorCount[1];y++)
    for (x=0;x<m_Config.m_iSectorCount[0];x++,iIndex++)
      m_pSector[iIndex] = m_pTerrain->CreateSectorInstance(x,y);

  m_spTerrainEffect = NULL;
  m_spTerrainEffectLib = NULL;
  m_bGlobalsCBDirty = true;

  // increase picking detail if needed (tile count)
  while (m_iPickingMeshDetail>0 
    && (m_Config.m_iHeightSamplesPerSector[0]<(m_Config.m_iTilesPerSector[0]<<m_iPickingMeshDetail))
     || m_Config.m_iHeightSamplesPerSector[1]<(m_Config.m_iTilesPerSector[1]<<m_iPickingMeshDetail))
     m_iPickingMeshDetail--;

//  EnsureSharedMeshesCreated();
}


void VTerrainSectorManager::DeInitTerrain()
{
  for (int i=0;i<m_iSectorCount;i++)
    m_pSector[i]->OnDestroying();

  V_SAFE_DELETE_ARRAY(m_pSector);
  m_iSectorCount = 0;
  PurgeUnusedResources();
  m_spTerrainEffect = NULL;
  m_spTerrainEffectLib = NULL;
  m_spTerrainIndexBuffer = NULL;

  m_CollisionMeshManager.PurgeUnusedResources();
  int iMeshCount = m_CollisionMeshManager.GetResourceCount();
  VASSERT(iMeshCount==0);

  // clear caches:
  m_SurfaceCache.Clear();

  // clear resources allocated by model manager:
  VTerrainDecorationModelManager::GlobalManager().ReleaseInstanceBuffer();
}


void VTerrainSectorManager::EnsureSharedMeshesCreated()
{
  if (m_spTerrainIndexBuffer)
    return;

  // prepare index buffers
  int iIndexCount = 0;
  int iEstCount = 38 * (m_Config.m_iHeightSamplesPerSector[0] / m_Config.m_iSectorMeshesPerSector[0]) * (m_Config.m_iHeightSamplesPerSector[1] / m_Config.m_iSectorMeshesPerSector[1]);
  DynArray_cl<unsigned short> indices(iEstCount,0);
  int x,y;

  // highest resolution does not have increased edge res.
  int iCount = CreateLODIndexBuffer(m_Config, 0,0,0,0,0,indices,iIndexCount,NULL);
  for (int up=0;up<2;up++)
    for (int right=0;right<2;right++)
      for (int btm=0;btm<2;btm++)
        for (int left=0;left<2;left++)
          m_LODInfo[0].m_EdgeInfo[up][right][btm][left].SetRange(iIndexCount/3,iCount/3);
  iIndexCount+=iCount;

  // higher LOD levels have edge resolution permutations
  for (int i=1;i<m_Config.m_iMaxMeshLOD;i++)
  {
    VSectorMeshLODLevelInfo_t &info = m_LODInfo[i];
    // edges
    for (int up=0;up<2;up++)
      for (int right=0;right<2;right++)
        for (int btm=0;btm<2;btm++)
          for (int left=0;left<2;left++)
          {
            VSectorMeshRenderRange_t &range = info.m_EdgeInfo[up][right][btm][left];
            ANALYSIS_IGNORE_WARNING_ONCE(6246)
            int iCount = CreateLODIndexBuffer(m_Config, i,up,right,btm,left,indices,iIndexCount,NULL);
            range.SetRange(iIndexCount/3,iCount/3);
            iIndexCount+=iCount;
          }
  }


  // build shared vertex & index buffer (shared for all meshes)
  const int iSamplesX = m_Config.m_iHeightSamplesPerSector[0] / m_Config.m_iSectorMeshesPerSector[0];
  const int iSamplesY = m_Config.m_iHeightSamplesPerSector[1] / m_Config.m_iSectorMeshesPerSector[1];
  const int iVertCount = (iSamplesX+1)*(iSamplesY+1);
  VisMBVertexDescriptor_t desc;
  desc.m_iStride = sizeof(hkvVec2);
  desc.m_iPosOfs = 0 | VERTEXDESC_FORMAT_FLOAT2;
  m_spTerrainIndexBuffer = new VisMeshBuffer_cl(desc,iVertCount,VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST,iIndexCount,-1,VIS_MEMUSAGE_STATIC, true,true);
  m_spTerrainIndexBuffer->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  #ifdef HK_DEBUG
  m_spTerrainIndexBuffer->SetFilename("<TerrainIndexBuffer>");
  #endif
  m_iIndexBufferStreamMask = m_spTerrainIndexBuffer->GetStreamMask();

  // vertex XY resp. UV position
  hkvVec2 vInvSize(1.f/(float)m_Config.m_iSectorMeshVertexCount[0], 1.f/(float)m_Config.m_iSectorMeshVertexCount[1]);
  hkvVec2 *pVert = (hkvVec2 *)m_spTerrainIndexBuffer->LockVertices(VIS_LOCKFLAG_DISCARDABLE);
  for (y=0;y<=iSamplesY;y++)
    for (x=0;x<=iSamplesX;x++, pVert++)
      pVert->set((float)x*vInvSize.x, (float)y*vInvSize.y);
  m_spTerrainIndexBuffer->UnLockVertices();


  // index buffer (all LOD levels)
  unsigned short *pIndex = (unsigned short *)m_spTerrainIndexBuffer->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
  memcpy(pIndex,indices.GetDataPtr(),iIndexCount*sizeof(unsigned short));
  m_spTerrainIndexBuffer->UnLockIndices();
}



#define VERTEXINDEX(ix,iy)    ((iy)*iSingleRow+(ix))
#define ADDTRIANGLE(v0,v1,v2) \
{\
  VASSERT((v0)>=0 && (v0)<iVertexCount);\
  VASSERT((v1)>=0 && (v1)<iVertexCount);\
  VASSERT((v2)>=0 && (v2)<iVertexCount);\
  bool bAddTriangle = true;\
  if (pTileHoleMask)\
  {\
    int vx = ((v0)%iSingleRow);\
    int vy = ((v0)/iSingleRow);\
    vx = hkvMath::Min(vx,(v1)%iSingleRow);\
    vy = hkvMath::Min(vy,(v1)/iSingleRow);\
    vx = hkvMath::Min(vx,(v2)%iSingleRow);\
    vy = hkvMath::Min(vy,(v2)/iSingleRow);\
    int tx = vx / cfg.m_iHeightSamplesPerTile[0];\
    int ty = vy / cfg.m_iHeightSamplesPerTile[1];\
    bAddTriangle = !pTileHoleMask[ty*cfg.m_iTilesPerSectorMesh[0]+tx];\
  }\
  if (bAddTriangle)\
  {\
    pDestBuffer[iStartIndex+iIndex+0] = v0;\
    pDestBuffer[iStartIndex+iIndex+1] = v1;\
    pDestBuffer[iStartIndex+iIndex+2] = v2;\
    iIndex += 3;\
  }\
}


#if defined(_VISION_PS3) || defined(_VISION_PSP2)
#pragma diag_push
#pragma diag_suppress=178
#endif


int VTerrainSectorManager::CreateLODIndexBuffer(const VTerrainConfig &cfg, int iLOD, int iUp, int iRight, int iBottom, int iLeft, DynArray_cl<unsigned short> &pDestBuffer, int iStartIndex, const bool* pTileHoleMask)
{
  const int iSamplesX = cfg.m_iHeightSamplesPerSector[0] / cfg.m_iSectorMeshesPerSector[0];
  const int iSamplesY = cfg.m_iHeightSamplesPerSector[1] / cfg.m_iSectorMeshesPerSector[1];
  const int iVertexCount = (iSamplesX+1) * (iSamplesY+1);
  const int iStep = 1<<iLOD; // LOD==0 -> use all vertices
  const int iSingleRow = (iSamplesX+1);
  const int iVertexRowStride = iSingleRow*iStep; // num vertices per row
  const int iBorder = (iLOD==0) ? 0 : iStep;
  int iIndex = 0;
  int j,x,y,iLast,iV0,iV1=0;

  // in highest resolution, there are no subdivisions for the edges
  if (iLOD==0)
  {
    VASSERT(iUp==0 && iRight==0 && iBottom==0 && iLeft==0 && "Cannot use increased edge resolution in highest resolution");
  }

  for (y=iBottom*iStep;y<iSamplesY-iUp*iStep;y+=iStep)
  {
    int iRow = y*iSingleRow;
    int iNextRow = iRow+iVertexRowStride;
    for (x=iLeft*iStep;x<iSamplesX-iRight*iStep;x+=iStep)
    {
      ADDTRIANGLE(iRow+x,iRow+x+iStep,iNextRow+x+iStep);
      ADDTRIANGLE(iRow+x,iNextRow+x+iStep,iNextRow+x);
    }
  }

  const int iHalfStep = iStep/2;

  // horizontal border
  iLast = iSamplesX-iBorder;
  for (x=iBorder;x<=iLast;x+=iStep)
  {
    const int iFanStart = (x==iBorder) ? iStep : iHalfStep;
    const int iFanEnd   = (x==iLast) ? iStep : iHalfStep;
    if (iBottom)
    {
      iV0 = VERTEXINDEX(x,iBorder);
      for (j=-iFanStart;j<iFanEnd;j+=iHalfStep)
      {
        iV1 = VERTEXINDEX(x+j,0);
        ADDTRIANGLE(iV0,iV1,iV1+iHalfStep);
      }
      if (x<iLast)
        ADDTRIANGLE(iV0,iV1+iHalfStep,iV0+iStep);
    }
    if (iUp)
    {
      iV0 = VERTEXINDEX(x,iSamplesY-iBorder);
      for (j=-iFanStart;j<iFanEnd;j+=iHalfStep)
      {
        iV1 = VERTEXINDEX(x+j,iSamplesY);
        ADDTRIANGLE(iV0,iV1+iHalfStep,iV1);
      }
      if (x<iLast)
        ADDTRIANGLE(iV0,iV0+iStep,iV1+iHalfStep);
    }
  }

  // vertical border
  iLast = iSamplesY-iBorder;
  for (y=iBorder;y<=iLast;y+=iStep)
  {
    const int iFanStart = (y==iBorder) ? iStep : iHalfStep;
    const int iFanEnd   = (y==iLast) ? iStep : iHalfStep;
    if (iLeft)
    {
      iV0 = VERTEXINDEX(iBorder,y);
      for (j=-iFanStart;j<iFanEnd;j+=iHalfStep)
      {
        iV1 = VERTEXINDEX(0,y+j);
        ADDTRIANGLE(iV1,iV0,iV1+iSingleRow*iHalfStep);
      }
      if (y<iLast)
        ADDTRIANGLE(iV1+iSingleRow*iHalfStep,iV0,iV0+iStep*iSingleRow);
    }

    if (iRight)
    {
      iV0 = VERTEXINDEX(iSamplesX-iBorder,y);
      for (j=-iFanStart;j<iFanEnd;j+=iHalfStep)
      {
        iV1 = VERTEXINDEX(iSamplesX,y+j);
        ADDTRIANGLE(iV1+iSingleRow*iHalfStep,iV0,iV1);
      }
      if (y<iLast)
        ADDTRIANGLE(iV0+iStep*iSingleRow,iV0,iV1+iSingleRow*iHalfStep);
    }
  }

  // corners:
  if (iBottom && !iLeft)
    ADDTRIANGLE(VERTEXINDEX(0,0),VERTEXINDEX(iStep,iStep),VERTEXINDEX(0,iStep))
  else if (!iBottom && iLeft)
    ADDTRIANGLE(VERTEXINDEX(0,0),VERTEXINDEX(iStep,0),VERTEXINDEX(iStep,iStep))

  if (iLeft && !iUp)
    ADDTRIANGLE(VERTEXINDEX(0,iSamplesY),VERTEXINDEX(iStep,iSamplesY-iStep),VERTEXINDEX(iStep,iSamplesY))
  else if (!iLeft && iUp)
    ADDTRIANGLE(VERTEXINDEX(0,iSamplesY),VERTEXINDEX(0,iSamplesY-iStep),VERTEXINDEX(iStep,iSamplesY-iStep))

  if (iRight && !iUp)
    ADDTRIANGLE(VERTEXINDEX(iSamplesX-iStep,iSamplesY),VERTEXINDEX(iSamplesX-iStep,iSamplesY-iStep),VERTEXINDEX(iSamplesX,iSamplesY))
  else if (!iRight && iUp)
    ADDTRIANGLE(VERTEXINDEX(iSamplesX,iSamplesY),VERTEXINDEX(iSamplesX-iStep,iSamplesY-iStep),VERTEXINDEX(iSamplesX,iSamplesY-iStep))

  if (iRight && !iBottom)
    ADDTRIANGLE(VERTEXINDEX(iSamplesX-iStep,0),VERTEXINDEX(iSamplesX,0),VERTEXINDEX(iSamplesX-iStep,iStep))
  else if (!iRight && iBottom)
    ADDTRIANGLE(VERTEXINDEX(iSamplesX-iStep,iStep),VERTEXINDEX(iSamplesX,0),VERTEXINDEX(iSamplesX,iStep))

  return iIndex;
}


#if defined(_VISION_PS3) || defined(_VISION_PSP2)
#pragma diag_pop
#endif



inline VTerrainSectorMeshPageInfo *VTerrainSectorManager::GetMeshPageInfo(int x, int y)
{
  int sx = x/m_Config.m_iSectorMeshesPerSector[0];
  int sy = y/m_Config.m_iSectorMeshesPerSector[1];
  if (x<0 || y<0 || sx>=m_Config.m_iSectorCount[0] || sy>=m_Config.m_iSectorCount[1])
    return NULL;
  return GetSector(sx,sy)->GetMeshPageInfo(x%m_Config.m_iSectorMeshesPerSector[0],y%m_Config.m_iSectorMeshesPerSector[1]);
}

bool VTerrainSectorManager::GetMeshPageInfo(int x, int y, int &iGlobalIndex) const
{
  if (x<0 || y<0 || x>=m_Config.m_iOverallMeshPageCount[0] || y>=m_Config.m_iOverallMeshPageCount[1])
    return false;

  iGlobalIndex = y*m_Config.m_iOverallMeshPageCount[0]+x;
  return true;
}

void VTerrainSectorManager::GetTerrainBBox(VLargeBoundingBox &bbox) const
{
  bbox.m_vMin.m_iSectorX = 0;
  bbox.m_vMin.m_iSectorY = 0;
  bbox.m_vMax.m_iSectorX = m_Config.m_iSectorCount[0];
  bbox.m_vMax.m_iSectorY = m_Config.m_iSectorCount[1];
  bbox.m_vMin.m_vSectorOfs.set(0,0,GetMinZ());
  bbox.m_vMax.m_vSectorOfs.set(0,0,GetMaxZ());
}


hkvVec2 VTerrainSectorManager::GetRelativeTerrainPos(const VLargePosition &vPos) const
{
  VASSERT(!"[TODO]");
  hkvVec2 vRel((float)vPos.m_iSectorX,(float)vPos.m_iSectorY);
  return vRel;
}


int VTerrainSectorManager::GetDecorationObjectCount() const
{
  int iDecorationObjectCount = 0;

  for (int i = 0; i < m_iSectorCount; ++i)
    iDecorationObjectCount += m_pSector[i]->Decoration().m_iCount;

  return iDecorationObjectCount;
}


float VTerrainSectorManager::GetHeightAtWorldPos(const VLargePosition &vPos) const
{
  // make sure we access height in correct sector
  VASSERT(vPos.IsValid(m_Config));
  if (vPos.m_iSectorX<0 || vPos.m_iSectorY<0 || vPos.m_iSectorX>=m_Config.m_iSectorCount[0] || vPos.m_iSectorY>=m_Config.m_iSectorCount[1])
    return 0.f;
  VTerrainSector *pSector = GetSector(vPos.m_iSectorX,vPos.m_iSectorY);
  pSector->EnsureLoaded();
  return pSector->GetHeightAtRelPos(vPos.m_vSectorOfs);
}


float VTerrainSectorManager::GetHeightAtSamplePos(int iSampleX, int iSampleY) const
{
  // make sure we access height in correct sector
  const int iSectorX = m_Config.GetSectorIndexAtSamplePosX(iSampleX);
  const int iSectorY = m_Config.GetSectorIndexAtSamplePosY(iSampleY);
  VASSERT(iSectorX>=0 && iSectorY>=0 && iSectorX<m_Config.m_iSectorCount[0] && iSectorY<m_Config.m_iSectorCount[1]);
//  if (iSectorX<0 || iSectorY<0 || iSectorX>=m_Config.m_iSectorCount[0] || iSectorY>=m_Config.m_iSectorCount[1])
//    return 0.f;
  VTerrainSector *pSector = GetSector(iSectorX,iSectorY);
  pSector->GetHeightmapValues();
  return pSector->GetHeightAt(m_Config.GetSectorSampleOfsX(iSampleX),m_Config.GetSectorSampleOfsY(iSampleY));
}


float VTerrainSectorManager::GetHeightAtSamplePosClamped(int iSampleX, int iSampleY) const
{
  if (iSampleX<0) iSampleX=0; 
    else if (iSampleX>=m_Config.m_iOverallHeightSampleCount[0])
      iSampleX=m_Config.m_iOverallHeightSampleCount[0]-1;
  if (iSampleY<0) iSampleY=0; 
    else if (iSampleY>=m_Config.m_iOverallHeightSampleCount[1])
      iSampleY=m_Config.m_iOverallHeightSampleCount[1]-1;
  return GetHeightAtSamplePos(iSampleX,iSampleY);
}



hkvVec3 VTerrainSectorManager::GetNormalAtWorldPos(const VLargePosition &vPos) const
{
  float fH = GetHeightAtWorldPos(vPos);
  hkvVec3 vDirX(m_Config.m_vNormalGradientSpacing.x,0.f,0.f);
  hkvVec3 vDirY(0.f,m_Config.m_vNormalGradientSpacing.y,0.f);

  vDirX.z = GetHeightAtWorldPos(VLargePosition::Sum(m_Config,vPos,vDirX)) - fH;
  vDirY.z = GetHeightAtWorldPos(VLargePosition::Sum(m_Config,vPos,vDirY)) - fH;
  vDirX = vDirX.cross(vDirY);
  vDirX.normalizeIfNotZero();
  VASSERT(vDirX.z>0.f);
  return vDirX;
}



void VTerrainSectorManager::GetNormalAtSamplePos(int iSampleX, int iSampleY, hkvVec3& destNrml, float fNrmlLen) const
{
  hkvVec3 vDirX(m_Config.m_vSampleSpacing.x,0.f,0.f);
  hkvVec3 vDirY(0.f,m_Config.m_vSampleSpacing.y,0.f);

  float fH = GetHeightAtSamplePosClamped(iSampleX,iSampleY);
  float dright = GetHeightAtSamplePosClamped(iSampleX+1,iSampleY)-fH;
  float dleft = GetHeightAtSamplePosClamped(iSampleX-1,iSampleY)-fH;
  float dup = GetHeightAtSamplePosClamped(iSampleX,iSampleY+1)-fH;
  float ddown = GetHeightAtSamplePosClamped(iSampleX,iSampleY-1)-fH;

  // take the average normal from 4 directions
  vDirX.z = dright;
  vDirY.z = dup;
  destNrml = vDirX.cross(vDirY);

  vDirX.z = dright;
  vDirY.z = -ddown;
  destNrml += vDirX.cross(vDirY);

  vDirX.z = -dleft;
  vDirY.z = dup;
  destNrml += vDirX.cross(vDirY);

  vDirX.z = -dleft;
  vDirY.z = -ddown;
  destNrml += vDirX.cross(vDirY);

  destNrml.setLength(fNrmlLen);
}

void VTerrainSectorManager::GetNormalAtSamplePosFast(int iSampleX, int iSampleY, hkvVec3& destNrml, float fNrmlLen) const
{
  hkvVec3 vDirX(m_Config.m_vSampleSpacing.x,0.f,0.f);
  hkvVec3 vDirY(0.f,m_Config.m_vSampleSpacing.y,0.f);
  float fH = GetHeightAtSamplePosClamped(iSampleX,iSampleY);
  vDirX.z = GetHeightAtSamplePosClamped(iSampleX+1,iSampleY)-fH;
  vDirY.z = GetHeightAtSamplePosClamped(iSampleX,iSampleY+1)-fH;
  destNrml = vDirX.cross(vDirY);
  destNrml.setLength(fNrmlLen);
}

void VTerrainSectorManager::GetNormalAtFace(int iSampleX, int iSampleY, hkvVec3& destNrml, float fNrmlLen) const
{
  hkvVec3 vDirX(m_Config.m_vSampleSpacing.x,0.f,0.f);
  hkvVec3 vDirY(0.f,m_Config.m_vSampleSpacing.y,0.f);

  // this is allowed as we can access the overlapping samples
  float fH00 = GetHeightAtSamplePosClamped(iSampleX,iSampleY);
  float fH10 = GetHeightAtSamplePosClamped(iSampleX+1,iSampleY);
  float fH01 = GetHeightAtSamplePosClamped(iSampleX,iSampleY+1);
  float fH11 = GetHeightAtSamplePosClamped(iSampleX+1,iSampleY+1);

  vDirX.z = fH10-fH00;
  vDirY.z = fH01-fH00;
  destNrml = vDirX.cross(vDirY);

  vDirX.z = fH11-fH01;
  vDirY.z = fH11-fH10;
  destNrml += vDirX.cross(vDirY);

  destNrml.setLength(fNrmlLen);
}



int VTerrainSectorManager::GetMaterialIDAtWorldPos(const VLargePosition &vPos) const
{
  VASSERT(vPos.IsValid(m_Config));
  if (vPos.m_iSectorX<0 || vPos.m_iSectorY<0 || vPos.m_iSectorX>=m_Config.m_iSectorCount[0] || vPos.m_iSectorY>=m_Config.m_iSectorCount[1])
    return -1;
  VTerrainSector *pSector = GetSector(vPos.m_iSectorX,vPos.m_iSectorY);
  pSector->EnsureLoaded();
  return pSector->GetMaterialID(vPos.m_vSectorOfs.x,vPos.m_vSectorOfs.y);

}


void VTerrainSectorManager::RenderReplacementMeshes(VTerrainVisibilityCollectorComponent &infoComp, IVisCallbackDataObject_cl* pCallbackData, IVTerrainDecorationModel::RenderMode_e eRenderMode)
{
  VISION_PROFILE_FUNCTION(PROFILING_RENDERTERRAINREPLACEMENTMESHES);
  // replacement meshes:
  if (!infoComp.m_bHasVisibleReplacementMeshes || eRenderMode != IVTerrainDecorationModel::RENDER_MODE_OTW)
    return;

  VTerrainVisibilityInfo &info(infoComp.m_VisibilityInfo);
  Vision::RenderLoopHelper.BeginMeshRendering();
  Vision::RenderLoopHelper.SetMeshTransformationMatrix(hkvMat4::IdentityMatrix());

  for (int y=info.m_iVisibleSectorRange[1]; y<=info.m_iVisibleSectorRange[3]; y++)
  {
    for (int x=info.m_iVisibleSectorRange[0]; x<=info.m_iVisibleSectorRange[2]; x++)
    {
      VTerrainSector *pSector = GetSector(x,y);
      VCompiledTechnique* pReplTech = pSector->GetReplacementTechnique();
      if (!pReplTech)
        continue;  
      
      if (!infoComp.IsSectorReplacementVisible(pSector->VManagedResource::GetNumber()))
        continue;

      pSector->EnsureReplacementMeshLoaded();
      if (!pSector->m_spReplacementMesh)
        continue;

      VisMeshBuffer_cl *pMesh = pSector->m_spReplacementMesh;
      VCompiledShaderPass *pShader = pReplTech->GetShader(0);
      Vision::RenderLoopHelper.ResetMeshStreams();
      Vision::RenderLoopHelper.AddMeshStreams(pMesh);
      Vision::RenderLoopHelper.RenderMeshes(pShader,pMesh->GetPrimitiveType(), 0, 
        pMesh->GetCurrentPrimitiveCount(), pMesh->GetVertexCount());
    }
  }

  Vision::RenderLoopHelper.EndMeshRendering();
}


void VTerrainSectorManager::RenderDecoration(VTerrainVisibilityCollectorComponent &infoComp, IVisCallbackDataObject_cl* pCallbackData, IVTerrainDecorationModel::RenderMode_e eRenderMode)
{
  VISION_PROFILE_FUNCTION(PROFILING_RENDERDECORATION);
  m_pRenderCollector = &infoComp;
    
  VTerrainVisibilityInfo &info(infoComp.m_VisibilityInfo);
  VTerrainDecorationInstance **pInst = info.m_VisibleDecoration.GetDataPtr();

  // choose lightgrid color for all objects
  if (m_bNeedsLightgridUpdate && eRenderMode == IVTerrainDecorationModel::RENDER_MODE_OTW)
  {
    m_bNeedsLightgridUpdate = false;
    VLightGrid_cl *pGrid = m_pTerrain->GetRelevantLightGrid();
    bool bUseLightgrid = true;
    bool bIsForward = true;

    // we check the renderer nodes flag whether it wants static lighting or not. This is analogue to entity lighting
    IVRendererNode *pRenderer = Vision::Renderer.GetRendererNode(0);
    IVTimeOfDay *pToD = Vision::Renderer.GetTimeOfDayHandler();
    if (pRenderer!=NULL)
    {
#if !defined( HK_ANARCHY )
      if (pRenderer->IsOfType(VDeferredRenderingSystem::GetClassTypeId()))
      {
        bIsForward = false;
        VDeferredRenderingSystem *pDeferred = (VDeferredRenderingSystem *)pRenderer;
        if (!pDeferred->IsUsingLightmaps())
          bUseLightgrid = false;
      } 
#endif
    }

    if (bUseLightgrid && pGrid!=NULL)
    {
      // take the upper side's center of the lightgrid to look up colors
      hkvAlignedBBox bbox(hkvNoInitialization);
      pGrid->GetBoundingBox(bbox);
      hkvVec3 vCenter = bbox.getCenter();
      vCenter.z = bbox.m_vMax.z;
      pGrid->GetColorsAtPositionI(vCenter,m_DecorationLightgridColors);
    } 
    else
    {
      // black (deferred)
      memset(m_DecorationLightgridColors,0,sizeof(m_DecorationLightgridColors));
    }

    if (bIsForward)
    {
      // use time of day to create directional lightgrid information. Add this to existing colors
      if (pToD!=NULL)
      {
        // add time of day colors to lightgrid
        const hkvVec3* pToDColors = pToD->GetLightGridColors(); // cached
        for (int i=0;i<6;i++)
          m_DecorationLightgridColors[i] += pToDColors[i];
        m_bNeedsLightgridUpdate = true; // daytime may change so update next time again...
      } else
      {
        // initialize with white as the last option (forward, no timeofday, no lightgrid)
        if (pGrid==NULL)
        {
          for (int i=0;i<6;i++)
            m_DecorationLightgridColors[i].set(1.f,1.f,1.f);
        }
      }
    } 

  }

  if (eRenderMode == IVTerrainDecorationModel::RENDER_MODE_OTW)
  Vision::RenderLoopHelper.TrackLightGridInfo(m_DecorationLightgridColors);

  #if defined(_VR_DX11)
    // for sphere constraints
    VisRenderStates_cl::SetVSConstantBuffer(VTERRAIN_CB_GLOBALS,GetGlobalsConstantBuffer());
  #endif

  IVTerrainDecorationModel *pLastModel = NULL;

  // we have to check context type here (and not in visibility) because the collector can be shared for
  // different types of contexts [#25701]
  const VisContextUsageHint_e eContextType = (VisContextUsageHint_e)(Vision::Contexts.GetCurrentContext()->GetUsageHint() & VIS_CONTEXTUSAGE_TYPEENUMMASK);

  const int iDecoCount = info.m_iVisibleDecorationCount;
  int iStart = 0;
  for (int i=0;i<iDecoCount;i++)
  {
    IVTerrainDecorationModel *pThisModel = pInst[i]->m_spModel;
    VASSERT(pThisModel!=NULL && pThisModel->IsValid());

    // setup model specific data:
    if (pThisModel!=pLastModel)
    {
      if (pLastModel && pLastModel->SupportsContextType(eContextType))
        pLastModel->RenderBatch(&infoComp,&pInst[iStart],i-iStart, eRenderMode);
      iStart = i;
      pLastModel = pThisModel;
    }
  }
  if (pLastModel && pLastModel->SupportsContextType(eContextType))
    pLastModel->RenderBatch(&infoComp, &pInst[iStart],iDecoCount-iStart, eRenderMode);

  #if defined(_VR_DX11)
    VisRenderStates_cl::SetVSConstantBuffer(VTERRAIN_CB_GLOBALS,NULL);
    VisRenderStates_cl::SetVSConstantBuffer(VTERRAIN_CB_DECOMODEL,NULL);
  #endif
}

float GetXYDistanceToAABB (const hkvAlignedBBox& aabb, const hkvVec3& vPos);

// called by VTerrainVisibilityCollectorComponent::BeginVisibility from the main (rendering) thread
void VTerrainSectorManager::BeginVisibilityUpdate(VTerrainVisibilityCollectorComponent &infoComp)
{
  VISION_PROFILE_FUNCTION(PROFILING_VISIBILITY_OVERALL);

  { // profiling block
    VISION_PROFILE_FUNCTION(PROFILING_VISIBILITY_STARTVISIBILITY);

    float fLODScale = infoComp.m_pCollector->GetLODReferenceRenderContext()->GetLODDistanceScaling();
    float fDetail = m_bForceFullLOD ? 0.f : m_fLODScaling;
    int hint = infoComp.m_pCollector->GetSourceContextUsageHint() & VIS_CONTEXTUSAGE_TYPEENUMMASK;
    infoComp.m_fLODBias = (hint==VIS_CONTEXTUSAGE_MIRROR) ? m_fMirrorLODBias : 0.f;

    if (m_Config.m_bDistanceBasedLODOnly)
    {
      // in this mode a detail of 1.0 means that LOD is roughly increased by one every sector
      hkvVec2 vScale(m_Config.m_vInvSectorSize.x*(float)m_Config.m_iSectorMeshesPerSector[0],m_Config.m_vInvSectorSize.y*(float)m_Config.m_iSectorMeshesPerSector[1]);
      infoComp.m_fSectorDistLODMult = (fDetail>0.f) ? (fLODScale*hkvMath::Min(vScale.x,vScale.y)/fDetail) : 0.f;
    }
    else
    {
      const float fTweak = 0.20f * fLODScale; // maybe also screen size?
      float fSpacingAtDist = hkvMath::Max(m_Config.m_vSampleSpacing.x,m_Config.m_vSampleSpacing.y);
      infoComp.m_fSectorDistLODMult = (fDetail>0.f) ? fTweak/(fSpacingAtDist*fDetail) : 0.f;
    }

    VTerrainVisibilityInfo &info(infoComp.m_VisibilityInfo);
    info.Set(infoComp.m_pCollector,m_Config);
    info.m_bVisibleInContext = (m_iVisibleBitmask&info.m_iContextFilterMask)!=0;
    if (!info.m_bVisibleInContext)
      return;

    // make sure the following sectors are loaded so they have a valid bounding box etc.
    // the depthshadow context itself should not precache because its radius is too large
    bool bPrecache = hint!=VIS_CONTEXTUSAGE_DEPTHSHADOW && !Vision::Editor.IsInEditor(); 
    if (bPrecache)
    {
      int x1,y1,x2,y2;
      info.m_VisibleRangeBox.GetSectorIndices_Clamped(m_Config,x1,y1,x2,y2);
      for (int y=y1;y<=y2;y++)
        for (int x=x1;x<=x2;x++)
        {
          VTerrainSector *pSector = GetSector(x,y);
          if (pSector->m_bPrepared || pSector->GetVisibleBitmask()==0)
          {
            pSector->UpdateTimeStamp();
            continue;
          }
          float fDist = GetXYDistanceToAABB (pSector->GetBoundingBox(), info.m_vCamVisPos);
          if (m_Config.m_bSupportsReplacement && fDist>m_Config.m_fReplacementDistance)
          {
            pSector->EnsureReplacementMeshLoaded();
            if (fDist>m_Config.m_fReplacementDistance*1.5f)
              continue;
          }
          pSector->PreCache(); // precache real sector data
        }
    }
    
  } // profiling block
}


// called by VTerrainVisibilityCollectorComponent::EndVisibility from the main (rendering) thread
void VTerrainSectorManager::EndVisibilityUpdate(VTerrainVisibilityCollectorComponent &infoComp)
{
  VISION_PROFILE_FUNCTION(PROFILING_VISIBILITY_OVERALL);

  { // profiling block
    VISION_PROFILE_FUNCTION(PROFILING_VISIBILITY_ENDVISIBILITY);

    VTerrainVisibilityInfo &info(infoComp.m_VisibilityInfo);
    if (!info.m_bVisibleInContext)
      return;

    // special treatment when the sector might be in visibility zones that are not of type VSectorVisibilityZone
    if (infoComp.m_pTerrain->m_bVisibleInVisZones)
    {
      const VisStaticGeometryInstanceCollection_cl *pVisibleOpaqueGeoInstances = infoComp.m_pCollector->GetVisibleStaticGeometryInstancesForPass(VPT_TransparentPass);
      const int iGeomCount = pVisibleOpaqueGeoInstances->GetNumEntries();
      for (int i=0;i<iGeomCount;i++)
      {
        if (pVisibleOpaqueGeoInstances->GetEntry(i)->GetGeometryType()!=STATIC_GEOMETRY_TYPE_TERRAIN)
          continue;
        VTerrainSector *pSector = (VTerrainSector*)pVisibleOpaqueGeoInstances->GetEntry(i);
        if (!infoComp.IsSectorVisible(pSector->VManagedResource::GetNumber()))
          infoComp.AddPendingSector(pSector);
      }
    }

    // compute visibility for sectors that weren't loaded at visibility computation time
    infoComp.ProcessPendingSectors();

    const int delta[5] = {0,1,0,-1,0};

    // finalize LODs
    bool bChanged = true;
    int iIteration = 0;
    //int iNonConstrained = 0;
    int iEdgeLOD[4] = {0,0,0,0};
    int iNeightborPageIndex=0;
    //int iThisPageIndex=0;

    int rangeX1 = info.m_iVisibleSectorRange[0] * m_Config.m_iSectorMeshesPerSector[0];
    int rangeY1 = info.m_iVisibleSectorRange[1] * m_Config.m_iSectorMeshesPerSector[1];
    int rangeX2 = (info.m_iVisibleSectorRange[2]+1) * m_Config.m_iSectorMeshesPerSector[0];
    int rangeY2 = (info.m_iVisibleSectorRange[3]+1) * m_Config.m_iSectorMeshesPerSector[1];

    while (bChanged)
    {
      //iNonConstrained = 0;
      iIteration++;
      bChanged = false;
      // loop through all pages
      for (int py=rangeY1;py<rangeY2;py++)
        for (int px=rangeX1;px<rangeX2;px++)
        {
          VTerrainSectorMeshPageInfo *pPage = GetMeshPageInfo(px,py);
          if (!pPage) // sector not loaded?
            continue;
          char &iPageLOD = infoComp.m_pPageLOD[pPage->m_iGlobalIndex];
          if ((iPageLOD==PAGE_LOD_NOTVISIBLE) || (iPageLOD&PAGE_LOD_FORCEFIXED)) // page not visible or page/sector uses replacement mesh and thus cannot be changed
            continue;
          int iNeighborConstraintLOD = 0;
          char iOldPageLOD = iPageLOD;
          for (int dir=0;dir<4;dir++)
          {
            int x1 = px+delta[dir];
            int y1 = py+delta[dir+1];
            iEdgeLOD[dir] = 0; // if neighbors aren't visible, use default edge resolution
            if (!GetMeshPageInfo(x1,y1,iNeightborPageIndex))
              continue;
            char &iNeighborLOD = infoComp.m_pPageLOD[iNeightborPageIndex];
            if (iNeighborLOD==PAGE_LOD_NOTVISIBLE) // not visible
              continue;
              
            int iDiff = (int)(iPageLOD&PAGE_LOD_MASK) - (int)(iNeighborLOD&PAGE_LOD_MASK);

            // the neighbor is a replacement mesh and thus cannot change its lod. So this LOD can only go up
            if ((iNeighborLOD&(PAGE_LOD_FORCEFIXED|PAGE_LOD_CONSTRAINED)) || (iPageLOD&PAGE_LOD_CONSTRAINED))
            {
              int iNeighborLODValue = iNeighborLOD&PAGE_LOD_MASK;
              if (iNeighborLOD&PAGE_LOD_FORCEFIXED)
                iNeighborConstraintLOD = iNeighborLODValue; // can't go smaller than this

              if (iDiff<0) // neighbor's LOD is larger
              {
                // if the neighbor does not support higher res edges, then force this to the same res.
                if (iNeighborLOD&PAGE_LOD_FORCEFIXED)
                  iPageLOD = iNeighborLODValue; // can only go up
                else
                  iPageLOD = iNeighborLODValue-1; // otherwise one difference is allowed

                iPageLOD |= PAGE_LOD_CONSTRAINED; // constrain this page
              }
              else if (iDiff>0)
              {
                int iMaxLOD = hkvMath::Max(iNeighborConstraintLOD,iNeighborLODValue+1);
                if ((iPageLOD&PAGE_LOD_CONSTRAINED)==0)
                  iPageLOD = iMaxLOD;
              }
                
              iEdgeLOD[dir] = ((iPageLOD&PAGE_LOD_MASK)>iNeighborLODValue) ? 1:0;
              continue;
            }

            //iNonConstrained++;
            if (iDiff<2) // only one LOD diff? -> everything OK
            {
              iEdgeLOD[dir] = iDiff==1 ? 1:0;
              continue;
            }
            iPageLOD = iNeighborLOD+1; // now iPageLOD is smaller than before
          }

          bChanged |= ((iOldPageLOD&PAGE_LOD_MASK) != (iPageLOD&PAGE_LOD_MASK));

          // seems we have valid sector edge information
          if (!bChanged)
          {
            int iFinalLOD = iPageLOD&PAGE_LOD_MASK;
            VSectorMeshLODLevelInfo_t *pLODInfo = pPage->m_bHasHoles ? pPage->m_HoleLODInfo : m_LODInfo;
            infoComp.m_pPageRenderRange[pPage->m_iGlobalIndex] = &pLODInfo[iFinalLOD].m_EdgeInfo[iEdgeLOD[0]][iEdgeLOD[1]][iEdgeLOD[2]][iEdgeLOD[3]];
            infoComp.m_pPageRenderRangeNoHoles[pPage->m_iGlobalIndex] = &m_LODInfo[iFinalLOD].m_EdgeInfo[iEdgeLOD[0]][iEdgeLOD[1]][iEdgeLOD[2]][iEdgeLOD[3]];
          }
        }
    }
    
    // collect visible vegetation:
    if (info.m_iEstimatedDecorationCount>0)
    {
      info.m_VisibleDecoration.EnsureSize(info.m_iEstimatedDecorationCount);
      for (int sy=info.m_iVisibleSectorRange[1];sy<=info.m_iVisibleSectorRange[3];sy++)
        for (int sx=info.m_iVisibleSectorRange[0];sx<=info.m_iVisibleSectorRange[2];sx++)
        {
          VTerrainSector *pSector = GetSector(sx,sy);
          if (!pSector->IsLoaded())
            continue;

          // in editor mode, there might be additional decoration to render, i.e. while painting:
          #ifdef WIN32
          if (pSector->m_bHasAdditionalDecoration)
          {
            int iAdditionalCount;
            VTerrainDecorationInstance* pDecorationInst = pSector->GetAdditionalDecoration(iAdditionalCount);
            for (int i=0;i<iAdditionalCount;i++,pDecorationInst++)
            {
              // just far clip checks here, no box clipping (performance isn't critical)
              const float fModelFarDist = pDecorationInst->m_spModel->m_Properties.m_fFarClip;
              const float fModelNearDist = pDecorationInst->m_spModel->m_Properties.m_fNearClip;
              float fDistSqr = pDecorationInst->GetDistanceSqr(info.m_vCamLODPos);
              if ((fDistSqr>=fModelNearDist*fModelNearDist) && (fDistSqr<fModelFarDist*fModelFarDist))
                info.AddVisibleDecorationInstance(pDecorationInst);
            }
          }
          #endif

          // add the collected decoration
          VTerrainSectorDecorationVisibilityMask *pDecoVisMask = VTerrainSectorDecorationVisibilityMask::GetForCollector(pSector->m_pFirstDecoVisInfo,&infoComp,-1);
          if (!pDecoVisMask)
            continue;

          // fill the bitmask into the list
          pDecoVisMask->AddInstances(infoComp, pSector->m_Decoration.m_pInstances);

          // clear the bitmask afterwards
          pDecoVisMask->Clear();
        }

      // sort vegetation; cannot set sorting key inside the threads
      VTerrainDecorationInstance **ppInst = info.m_VisibleDecoration.GetDataPtr();
      for (int i=0;i<info.m_iVisibleDecorationCount;i++,ppInst++)
      {
        float fDistSqr = (*ppInst)->GetDistanceSqr(info.m_vCamVisPos); // for sorting we use the real cam pos rather than m_vCamLODPos
        int iSubSort = 0xffff - (int)(hkvMath::sqrt (fDistSqr)*0.5f);
        (*ppInst)->m_iSortingKey = ((*ppInst)->m_iSortingKey & 0xffff0000) | hkvMath::Max(iSubSort,0); 
      }

      qsort(info.m_VisibleDecoration.GetDataPtr(),info.m_iVisibleDecorationCount,sizeof(VTerrainDecorationInstance *),VTerrainDecorationInstance::SortObjectPtrs);
    }

  } // profiling block
}


///////////////////////////////////////////////////////////////////////////////////////////////
// traceline related functions
///////////////////////////////////////////////////////////////////////////////////////////////


int VTerrainSectorManager::GetTraceIntersection(const VLargePosition &vTraceStart, const VLargePosition &vTraceEnd, int iStoreCount, VisTraceLineInfo_t *pStore)
{
  VISION_PROFILE_FUNCTION(PROFILING_TRACING);
  VASSERT(vTraceStart.IsValid(m_Config) && vTraceEnd.IsValid(m_Config));
  int x1 = vTraceStart.m_iSectorX;
  int y1 = vTraceStart.m_iSectorY;
  int x2 = vTraceEnd.m_iSectorX;
  int y2 = vTraceEnd.m_iSectorY;

  if (x1>x2) {int m=x1;x1=x2;x2=m;}
  if (y1>y2) {int m=y1;y1=y2;y2=m;}
  // completely outside?
  if (x1>=m_Config.m_iSectorCount[0] || y1>=m_Config.m_iSectorCount[1]) return false;
  if (x2<0 || y2<0) return false;
  if (x1<0) x1=0;
  if (y1<0) y1=0;
  if (x2>=m_Config.m_iSectorCount[0]) x2=m_Config.m_iSectorCount[0]-1;
  if (y2>=m_Config.m_iSectorCount[1]) y2=m_Config.m_iSectorCount[1]-1;

  hkvVec3 vStart = vTraceStart.ToRenderSpace(m_Config);
  hkvVec3 vEnd = vTraceEnd.ToRenderSpace(m_Config);

  for (int i=0;i<iStoreCount;i++)
    pStore[i].detected = FALSE;

  int iHitCount = 0;
  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
    {
      iHitCount += GetSector(x,y)->TraceTest(vStart,vEnd,iStoreCount,pStore);
      if (iHitCount && iStoreCount<1)
        return 1;
    }

  return iHitCount;
}


int VTerrainSectorManager::GetRelevantCollisionMeshesInBoundingBox(const VLargeBoundingBox &bbox, DynArray_cl<VSimpleCollisionMesh *> &meshes)
{
  int iCount = 0;
  int x1,y1,x2,y2;
  bbox.GetSectorIndices_Clamped(m_Config,x1,y1,x2,y2);

  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
      iCount += GetSector(x,y)->GetRelevantCollisionMeshesInBoundingBox(bbox,meshes,iCount);

  return iCount;
}

VSectorTile *VTerrainSectorManager::GetTile(int x,int y) const
{
  int sx = x/m_Config.m_iTilesPerSector[0];
  int sy = y/m_Config.m_iTilesPerSector[1];
  VTerrainSector *pSector = GetSector(sx,sy);
  pSector->EnsureLoaded();
  return pSector->GetTile(x-sx*m_Config.m_iTilesPerSector[0],y-sy*m_Config.m_iTilesPerSector[1]);
}


#define GET_VERTEX(p,xx,yy) \
{\
  p.x = m_Config.GetHeightmapSamplePosX((float)((xx)+iSampleOfsX));\
  p.y = m_Config.GetHeightmapSamplePosY((float)((yy)+iSampleOfsY));\
  p.z = GetHeightAtSamplePos(xx+iSampleOfsX,yy+iSampleOfsY)+ fOfsZ;\
}

#define CHECK_INTERSECTION(tri) \
      if (tri.GetTraceIntersection(vRayStart,vRayDir,false, fDistance))\
      {\
        if (iStoreCount<1)\
          return 1;\
        VisTraceLineInfo_t *pStoreInfo = VisTraceLineInfo_t::InsertTraceResult(iStoreCount,pStore, fDistance*fRayLen);\
        if (pStoreInfo)\
        {\
          iHits++;\
          pStoreInfo->detected = TRUE;\
          pStoreInfo->hitType = VIS_TRACETYPE_STATICGEOMETRY;\
          pStoreInfo->sceneElementType = V_COLMESH_SCENEELEMENT_TERRAIN;\
          pStoreInfo->colliderEntity = NULL;\
          pStoreInfo->pCollisionMesh = NULL;\
          pStoreInfo->distance = fDistance*fRayLen;\
          pStoreInfo->touchPoint = vRayStart + fDistance*vRayDir;\
          pStoreInfo->primitivePlane = tri.GetPlane (true);\
          if (!bSortedHits && iHits>=iStoreCount)\
            return iHits;\
        }\
      }

void MakeValid(int &minval, int &maxval)
{
  if (maxval>=minval) return;
  int m = minval;
  minval = maxval;
  maxval = m;
}

int VTerrainSectorManager::GetAccurateTraceIntersection(const VLargePosition &vStart, const VLargePosition &vEnd, int iStoreCount, VisTraceLineInfo_t *pStore, bool bSortedHits)
{
  for (int i=0;i<iStoreCount;i++)
    pStore[i].detected = FALSE;

  int x1,y1,x2,y2;
  m_Config.GetHeightSampleIndicesAtPos(vStart, x1,y1);
  m_Config.GetHeightSampleIndicesAtPos(vEnd, x2,y2);
  MakeValid(x1,x2);
  MakeValid(y1,y2);

  // margins:
  x1-=1;y1-=1;
  x2+=1;y2+=1;

  //Clamp
  if (x1<0) x1=0;
  if (y1<0) y1=0;
  if (x2>=m_Config.m_iOverallHeightSampleCount[0]) x2=m_Config.m_iOverallHeightSampleCount[0]-1;
  if (y2>=m_Config.m_iOverallHeightSampleCount[1]) y2=m_Config.m_iOverallHeightSampleCount[1]-1;

  // tile indices
  int tx1 = x1 / m_Config.m_iHeightSamplesPerTile[0];
  int ty1 = y1 / m_Config.m_iHeightSamplesPerTile[1];
  int tx2 = x2 / m_Config.m_iHeightSamplesPerTile[0];
  int ty2 = y2 / m_Config.m_iHeightSamplesPerTile[1];


  hkvVec3 vRayStart = vStart.ToRenderSpace(m_Config);
  hkvVec3 vRayEnd = vEnd.ToRenderSpace(m_Config);
  hkvVec3 vRayDir = vRayEnd - vRayStart;
  const float fRayLen = vRayDir.getLength();
  if (fRayLen<HKVMATH_LARGE_EPSILON)
    return 0;

  float fDistance;
  float fOfsZ = m_Config.m_vTerrainPos.z;

  // test all triangles inside the 2D bounding rect; do early out on tiles
  hkvVec3 p1(hkvNoInitialization),p2(hkvNoInitialization),p3(hkvNoInitialization),p4(hkvNoInitialization);
  VTriangle tri1,tri2;
  tri1.SetPoints(&p1,&p2,&p3);
  tri2.SetPoints(&p1,&p3,&p4);
  int iHits = 0;

  hkvAlignedBBox tilebb;
  for (int ty=ty1;ty<=ty2;ty++)
    for (int tx=tx1;tx<=tx2;tx++)
    {
      VSectorTile *pTile = GetTile(tx,ty);
      tilebb = pTile->m_TileBBox;
      // z-extent of tile bboxes might not be initialized
      tilebb.m_vMin.z = pTile->m_pOwner->m_fMinHeightValue + fOfsZ;
      tilebb.m_vMax.z = pTile->m_pOwner->m_fMaxHeightValue + fOfsZ;
      if (!tilebb.contains (vRayStart) && !tilebb.getLineSegmentIntersection(vRayStart,vRayEnd))
        continue;
      //Vision::Game.DrawBoundingBox(tilebb);

      int iSampleOfsX = tx*m_Config.m_iHeightSamplesPerTile[0];
      int iSampleOfsY = ty*m_Config.m_iHeightSamplesPerTile[1];
      int iCntX = hkvMath::Min(m_Config.m_iHeightSamplesPerTile[0], x2-iSampleOfsX);
      int iCntY = hkvMath::Min(m_Config.m_iHeightSamplesPerTile[1], y2-iSampleOfsY);

      for (int y=0;y<iCntY;y++)
        for (int x=0;x<iCntX;x++)
        {
          GET_VERTEX(p1,x,y);
          GET_VERTEX(p2,x+1,y);
          GET_VERTEX(p3,x+1,y+1);
          CHECK_INTERSECTION(tri1);  // triangle #1
          GET_VERTEX(p4,x,y+1);
          CHECK_INTERSECTION(tri2);  // triangle #2
        }

    }

  return iHits;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// file related functions
///////////////////////////////////////////////////////////////////////////////////////////////

//Called from VTerrain::SaveToFile
void VTerrainSectorManager::SaveAllSectors()
{
  for (int y=0;y<m_Config.m_iSectorCount[1];y++)
    for (int x=0;x<m_Config.m_iSectorCount[0];x++)
    {
      VTerrainSector *pSector = GetSector(x,y);
      pSector->SaveSectorFinal(true);  //save to final directory and generate resource XMLs
    }
}


void VTerrainSectorManager::PreCacheRegion(const VLargeBoundingBox &bbox)
{
  int x1,y1,x2,y2;
  bbox.GetSectorIndices_Clamped(m_Config,x1,y1,x2,y2);

  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
      GetSector(x,y)->PreCache();
}

#define TERRAINGEOMCHUNK_VERSION_1        1
#define TERRAINGEOMCHUNK_VERSION_2        2
#define TERRAINGEOMCHUNK_VERSION_3        3 // added serialization of sector physics type
#define TERRAINGEOMCHUNK_CURRENT_VERSION  TERRAINGEOMCHUNK_VERSION_3

void VTerrainSectorManager::LoadGeometryChunk(VChunkFile &file)
{
  int iVersion = -1;
  file.ReadInt(iVersion); VASSERT(iVersion>=0 && iVersion<=TERRAINGEOMCHUNK_CURRENT_VERSION);

  // load mesh as-is
  IVFileInStream *pIn = file.GetChunkInStream();
    m_spTerrainIndexBuffer = new VisMeshBuffer_cl();
    m_spTerrainIndexBuffer->ReadFromStream(pIn);
    #ifdef HK_DEBUG
    m_spTerrainIndexBuffer->SetFilename("<TerrainGeometryBuffer>");
    #endif
  pIn->Close();
  m_iIndexBufferStreamMask = m_spTerrainIndexBuffer->GetStreamMask();

  VisMBVertexDescriptor_t descr;
  m_spTerrainIndexBuffer->GetVertexDescriptor(descr);
  if (descr.m_iPosOfs != VERTEXDESC_FORMAT_FLOAT2)
  {
    Vision::Error.Warning("Please re-save terrain '%s' since it has outdated vertex buffers",m_pTerrain->GetFilename());
    m_spTerrainIndexBuffer = NULL;
    EnsureSharedMeshesCreated();
  }

  // load render ranges (LOD):
  int iLevels;
  file.ReadInt(iLevels);
  VASSERT(iLevels==m_Config.m_iMaxMeshLOD);
  for (int i=0;i<iLevels;i++)
    file.Read(&m_LODInfo[i],sizeof(VSectorMeshLODLevelInfo_t),"32i");

  if (iVersion>=TERRAINGEOMCHUNK_VERSION_1)
  {
    for (int i=0;i<m_iSectorCount;i++)
    {
      VASSERT(m_pSector!=NULL && m_pSector[i]!=NULL);
      int iMask = 0xffffffff;
      short iTraceMask = -1;
      short iLightMask = -1;
      int iPhysicsType = VTerrainSector::VPHYSICSTYPE_PRECISE;
      file.ReadInt(iMask);
      file.ReadShort(iTraceMask);
      if (iVersion>=TERRAINGEOMCHUNK_VERSION_2)
        file.ReadShort(iLightMask);
      if (iVersion>=TERRAINGEOMCHUNK_VERSION_3)
        file.ReadInt(iPhysicsType);
      m_pSector[i]->SetVisibleBitmask(iMask);
      m_pSector[i]->SetTraceBitmask(iTraceMask);
      m_pSector[i]->SetLightInfluenceBitmask(iLightMask);
      m_pSector[i]->SetPhysicsType((VTerrainSector::VPhysicsType_e)iPhysicsType);
    }
  }

}


void VTerrainSectorManager::SaveGeometryChunk(VChunkFile &file)
{
  EnsureSharedMeshesCreated();
  VASSERT(m_spTerrainIndexBuffer);

  file.WriteInt(TERRAINGEOMCHUNK_CURRENT_VERSION); // version

  // save mesh as-is
  IVFileOutStream *pIn = file.GetChunkOutStream();
    m_spTerrainIndexBuffer->WriteToStream(pIn);
  pIn->Close();

  // save render ranges (LOD):
  V_COMPILE_ASSERT(sizeof(VSectorMeshLODLevelInfo_t)==32*sizeof(int));
  V_COMPILE_ASSERT(sizeof(VSectorMeshRenderRange_t)==2*sizeof(int));
  file.WriteInt(m_Config.m_iMaxMeshLOD);
  for (int i=0;i<m_Config.m_iMaxMeshLOD;i++)
    file.Write(&m_LODInfo[i],sizeof(VSectorMeshLODLevelInfo_t),"32i");

  // version 1: save some per sector info
  for (int i=0;i<m_iSectorCount;i++)
  {
    VASSERT(m_pSector!=NULL && m_pSector[i]!=NULL);
    file.WriteInt(m_pSector[i]->GetVisibleBitmask());
    file.WriteShort(m_pSector[i]->GetTraceBitmask());
    file.WriteShort(m_pSector[i]->GetLightInfluenceBitmask()); // vers.2
    file.WriteInt((int)m_pSector[i]->GetPhysicsType()); // vers.3
  }
}




///////////////////////////////////////////////////////////////////////////////////////////////
// shader related functions
///////////////////////////////////////////////////////////////////////////////////////////////


VTextureObject* VTerrainSectorManager::GetPlainBlackTexture()
{
  if (!m_spPlainBlackTexture)
  {
    m_spPlainBlackTexture = Vision::TextureManager.CreateEmpty2DTextureObject("<RGBA(0,0,0,0)>",4,4,VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP);
    m_spPlainBlackTexture->EnsureLoaded();

    VColorRef pData[4][4];
    memset(pData,0,sizeof(pData));
    m_spPlainBlackTexture->UpdateRect(0, 0, 0, 0, 0, -1, pData, V_TEXTURE_LOCKFLAG_DISCARDABLE);
  }
  return m_spPlainBlackTexture;
}


VCompiledEffect* VTerrainSectorManager::GetTerrainEffect()
{
  if (!m_spTerrainEffect)
  {
    const bool bUseThreeWayMapping = m_pTerrain->m_Config.m_bUseThreeWayMapping;
    const bool bUseNormalMapping = m_pTerrain->m_Config.m_bUseNormalMapping;
    const bool bUseLightMapping = m_pTerrain->m_Config.m_bUseLightMapping &&
      Vision::RenderLoopHelper.HasLightmaps();

#if ( defined(WIN32) && !defined( HK_ANARCHY ) ) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_WIIU)
    BOOL bUseDeferred = Vision::Renderer.IsRendererNodeOfType(V_RUNTIME_CLASS(VDeferredRenderingSystem));
    if (bUseDeferred)
    {
      // Set shader library
      if (bUseThreeWayMapping)
      {
        m_spTerrainEffectLib = Vision::Shaders.LoadShaderLibrary(
          "\\Shaders\\DeferredShadingTerrain3Way", SHADERLIBFLAG_HIDDEN);
      }
      else
      {
        m_spTerrainEffectLib = Vision::Shaders.LoadShaderLibrary(
          "\\Shaders\\DeferredShadingTerrain", SHADERLIBFLAG_HIDDEN);
      }

      // Set effect
      char szParamStr[1024];
      strcpy(szParamStr, "MaterialParams=0,1,0,0;MaterialAmbient=0,0,0;");

      char szEffectName[1024] = "DeferredTerrain";
      if (bUseNormalMapping) strcat_s(szEffectName, "Normal");
      if (bUseLightMapping) strcat_s(szEffectName, "_Lightmap");

      m_spTerrainEffect = Vision::Shaders.CreateEffect(szEffectName,
        szParamStr, EFFECTCREATEFLAG_NONE, m_spTerrainEffectLib);
    }
    else	

#elif defined(_VISION_PSP2) || defined(_VISION_MOBILE) || ( defined( WIN32 ) && defined( HK_ANARCHY ) )
    // No deferred rendering
#else
  #error Undefined platform!
#endif
    {
      // Set shader library
      if (bUseThreeWayMapping)
      {
        m_spTerrainEffectLib = Vision::Shaders.LoadShaderLibrary(
          "\\Shaders\\Terrain3Way.ShaderLib", SHADERLIBFLAG_HIDDEN);
      }
      else
      {
        m_spTerrainEffectLib = Vision::Shaders.LoadShaderLibrary(
          "\\Shaders\\Terrain.ShaderLib", SHADERLIBFLAG_HIDDEN);
      }

      // Set effect
      char szEffectName[1024] = "Terrain";
      if (!bUseLightMapping) strcat_s(szEffectName, "Directional");
      if (bUseNormalMapping) strcat_s(szEffectName, "Normalmap");

      m_spTerrainEffect = Vision::Shaders.CreateEffect(szEffectName, 
        NULL, EFFECTCREATEFLAG_NONE, m_spTerrainEffectLib);
    }    
    VASSERT_MSG(m_spTerrainEffectLib!=NULL && m_spTerrainEffect!=NULL, "failed to create terrain effect");
  }

  return m_spTerrainEffect;
}

void VTerrainSectorManager::SetSphereConstraint(int iIndex, const VLargePosition &vPos, float fRadius)
{
  VASSERT(iIndex>=0 && iIndex<MAX_SPHERE_CONSTRAINTS);
  m_vSphereConstraintPos[iIndex] = vPos;
  m_fSphereConstraintRadius[iIndex] = fRadius;
  m_bSphereConstraintConstantsDirty = m_bGlobalsCBDirty = true;
//  Vision::Game.DrawCube(vPos.ToRenderSpace(),fRadius);
}


void VTerrainSectorManager::UpdateSphereConstraintConstants()
{
#if defined(_VR_DX11)
  hkvVec4* vRenderWorldPos = m_ConstantBufferGlobals.BeginUpdate().m_vSphereConstraint;
#else
  hkvVec4 vRenderWorldPos[MAX_SPHERE_CONSTRAINTS];
#endif
  for (int i=0;i<MAX_SPHERE_CONSTRAINTS;i++)
  {
    vRenderWorldPos[i] = m_vSphereConstraintPos[i].ToRenderSpace(m_Config).getAsVec4 (0.0f);
    vRenderWorldPos[i].w = m_fSphereConstraintRadius[i];
  }

  #if defined(_VR_DX11)
    m_ConstantBufferGlobals.EndUpdate();
  #else
    VisRenderStates_cl::SetVertexShaderConstant(44, vRenderWorldPos[0].data, MAX_SPHERE_CONSTRAINTS);
  #endif

  m_bSphereConstraintConstantsDirty = false;
}


#if defined(_VR_DX11)

VTerrainConstantBufferGlobals *VTerrainSectorManager::GetGlobalsConstantBuffer()
{
  if (m_bGlobalsCBDirty)
  {
    VTerrainConstantBuffer_Globals &data = m_ConstantBufferGlobals.BeginUpdate();
    // nothing to update here...
    m_ConstantBufferGlobals.EndUpdate();
    m_bGlobalsCBDirty = false;
  }
  return &m_ConstantBufferGlobals;
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
