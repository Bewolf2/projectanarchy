/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorMesh.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>


const VisMBVertexDescriptor_t& VTerrainVertexLowRes_t::GetDesc()
{
  g_Desc.m_iStride = sizeof(VTerrainVertexLowRes_t);
  g_Desc.m_iPosOfs = offsetof(VTerrainVertexLowRes_t,vPos);
  return g_Desc;
}

VisMBVertexDescriptor_t VTerrainVertexLowRes_t::g_Desc;


void VTerrainSectorMeshPageInfo::Init(VTerrainSector* pOwner, int x, int y, bool bFirstTime)
{
  m_pOwner = pOwner;

  const VTerrainConfig &cfg = m_pOwner->m_Config;
  m_iGlobalIndex = m_pOwner->GetGlobalPageIndex(x,y);

  float fInvMeshCountX = 1.f / (float)cfg.m_iSectorMeshesPerSector[0];
  float fInvMeshCountY = 1.f / (float)cfg.m_iSectorMeshesPerSector[1];
  float fSizeX = cfg.m_vSectorSize.x*fInvMeshCountX;
  float fSizeY = cfg.m_vSectorSize.y*fInvMeshCountY;
  m_AbsBoundingBox = m_pOwner->GetBoundingBox();
  hkvVec3 vOrigin = m_pOwner->GetSectorOrigin();
  if (bFirstTime)
    m_AbsBoundingBox.m_vMin.z = m_AbsBoundingBox.m_vMax.z = vOrigin.z;
  m_AbsBoundingBox.m_vMin.x = vOrigin.x + (float)x*fSizeX;
  m_AbsBoundingBox.m_vMin.y = vOrigin.y + (float)y*fSizeY;
  m_AbsBoundingBox.m_vMax.x = m_AbsBoundingBox.m_vMin.x + fSizeX;
  m_AbsBoundingBox.m_vMax.y = m_AbsBoundingBox.m_vMin.y + fSizeY;

#if defined(_VR_DX11)
  m_PageVSConstBuffer.AllocateBuffer(0,sizeof(hkvVec4)*3);
  hkvVec4* pReg = (hkvVec4*) m_PageVSConstBuffer.Lock (0,sizeof(hkvVec4)*3, VIS_LOCKFLAG_DISCARDABLE);
#else
  hkvVec4* pReg = (hkvVec4*) m_vVertexShaderConst;
#endif


  // reg 38
  pReg[0].x = cfg.m_vSectorSize.x*fInvMeshCountX;
  pReg[0].y = cfg.m_vSectorSize.y*fInvMeshCountY;
  pReg[0].z = m_AbsBoundingBox.m_vMin.x;
  pReg[0].w = m_AbsBoundingBox.m_vMin.y;

  // reg 39
  float fInvCountX = fInvMeshCountX/(float)cfg.m_iSectorCount[0];
  float fInvCountY = fInvMeshCountY/(float)cfg.m_iSectorCount[1];
  pReg[1].x = fInvCountX;
  pReg[1].y = fInvCountY;
  pReg[1].z = fInvCountX*((float)(m_pOwner->m_iIndexX*cfg.m_iSectorMeshesPerSector[0]) + (float)x);
  pReg[1].w = fInvCountY*((float)(m_pOwner->m_iIndexY*cfg.m_iSectorMeshesPerSector[1]) + (float)y);

  // reg 40: Transformation from Mesh UV to Sector UV, used for sampling the weightmap

  // Slightly offset the transform so that we don't filter across page borders in the weightmap,
  // as different components can refer to different channels
  float w = m_pOwner->m_Config.m_iDefaultWeightmapResolution[0] * fInvMeshCountX;
  float h = m_pOwner->m_Config.m_iDefaultWeightmapResolution[1] * fInvMeshCountY;
  hkvVec4 adjustedModelToSectorUV;
  adjustedModelToSectorUV.x = fInvMeshCountX * (w - 2) / w;
  adjustedModelToSectorUV.y = fInvMeshCountY * (h - 2) / h;
  adjustedModelToSectorUV.z = (x + 1.0f / w) * fInvMeshCountX;
  adjustedModelToSectorUV.w = (y + 1.0f / h) * fInvMeshCountY;

  // As some UVs (lightmap, normalmap) are computed from the sector UV, we need to make sure that they aren't affected by the offsetting,
  // so compute a transform that can be used to map back to the full range
  hkvVec4 rawModelToSectorUV;
  rawModelToSectorUV.x = fInvMeshCountX;
  rawModelToSectorUV.y = fInvMeshCountY;
  rawModelToSectorUV.z = (x + 0.0f) * fInvMeshCountX;
  rawModelToSectorUV.w = (y + 0.0f) * fInvMeshCountY;

  m_unclampUVTransform.x = rawModelToSectorUV.x / adjustedModelToSectorUV.x;
  m_unclampUVTransform.y = rawModelToSectorUV.y / adjustedModelToSectorUV.y;
  m_unclampUVTransform.z = rawModelToSectorUV.z - adjustedModelToSectorUV.z / adjustedModelToSectorUV.x * rawModelToSectorUV.x;
  m_unclampUVTransform.w = rawModelToSectorUV.w - adjustedModelToSectorUV.w / adjustedModelToSectorUV.y * rawModelToSectorUV.y;

  pReg[2] = adjustedModelToSectorUV;


#if defined(_VR_DX11)
  m_PageVSConstBuffer.Unlock();
#endif
}

#define SAVE_TEXTURE(_ptr) \
{\
  if (_ptr && _ptr!=Vision::TextureManager.GetPlainWhiteTexture() && _ptr!=Vision::TextureManager.GetPlainBlackTexture() && _ptr!=m_pOwner->GetSectorManager()->GetPlainBlackTexture())\
{\
  VTextureObject::DoArchiveExchange(ar, _ptr);\
}\
  else\
  VTextureObject::DoArchiveExchange(ar, NULL);\
}

void VTerrainSectorMeshPageInfo::SerializeX(VTerrainSector *pOwner, VArchive &ar, int iLocalVersion)
{
  if (ar.IsLoading())
  {
    if(iLocalVersion >= SECTOR_FILE_VERSION_21)
    {
      char magic[4];
      ar.Read(magic, 4);

      VASSERT(magic[0] == 0x50);
      VASSERT(magic[1] == 0x61);
      VASSERT(magic[2] == 0x67);
      VASSERT(magic[3] == 0x65);
    }

    ar.Read(m_fLODThreshold,(MAX_MESH_LOD-1)*sizeof(float),"f",(MAX_MESH_LOD-1));
    ar >> m_bHasHoles >> m_bAllHoles;
    if (m_bHasHoles && !m_bAllHoles)
    {
      short iLevels;
      ar >> iLevels;
      VASSERT(iLevels == pOwner->m_Config.m_iMaxMeshLODHoles+1);
      for (int i=0;i<iLevels;i++)
        m_HoleLODInfo[i].SerializeX(ar,iLocalVersion);
      m_spHoleIndexBuffer = new VisMeshBuffer_cl();
      m_spHoleIndexBuffer->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
      #ifdef HK_DEBUG
      m_spHoleIndexBuffer->SetFilename("<TerrainHoleIndexBuffer>");
#endif
      m_spHoleIndexBuffer->SerializeX(ar);
    }

    if(iLocalVersion >= SECTOR_FILE_VERSION_21)
    {
      char iTextureType;
      VTextureObject* pLayer[16];
      for (int i=0;i<16;i++)
      {
        ar >> iTextureType;
        switch (iTextureType)
        {
        case SECTOR_TEXTURETYPE_NULL : pLayer[i] = pOwner->GetSectorManager()->GetPlainBlackTexture();break; // this is a "real" black texture with alpha==0
        case SECTOR_TEXTURETYPE_WHITE : pLayer[i] = Vision::TextureManager.GetPlainWhiteTexture();break;
        case SECTOR_TEXTURETYPE_BLACK : pLayer[i] = Vision::TextureManager.GetPlainBlackTexture();break; // this one has alpha==255
        case SECTOR_TEXTURETYPE_FILE :   pLayer[i] = VTextureObject::DoArchiveExchange(ar, NULL);break;
        case SECTOR_TEXTURETYPE_NEUTRALNORMAL: pLayer[i] = Vision::TextureManager.GetNeutralNormalMapTexture();break;
        default:
          VASSERT(!"Invalid texture type. File corrupt?");
        }
        if (pLayer[i]) // valid for all types of texture
          pLayer[i]->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
        SetTextureBySamplerIndex(pLayer[i], i);
      }

      ar >> m_iSamplerClampMask;

      int iRegCountInFile;
      ar >> iRegCountInFile;


      m_iPixelShaderRegCount = hkvMath::Max(iRegCountInFile,6);

#if defined(_VR_DX11)
      m_SectorPSConstBuffer.AllocateBuffer(0,m_iPixelShaderRegCount*sizeof(hkvVec4));
#endif
      hkvVec4* pPixelConst = LockPixelConst();
      ar.Read(pPixelConst,iRegCountInFile*sizeof(hkvVec4),"ffff",iRegCountInFile);
      UpdateNormalmapRegister (pPixelConst);
      UnLockPixelConst();

      ar >> m_iNumThreeWayMappingLayer;
      if (m_iNumThreeWayMappingLayer > 0)
      {
        ar >> m_iThreeWayMappingLayerMask;
      }
      {
        VTextureObjectPtr *pSceneLightmaps = m_pOwner->GetSectorManager()->m_pTerrain->m_spSceneLightmaps;

        ar.Read(m_iLightmapSamplerIndex,4,"cccc"); // 4 chars
        for (int i=0;i<4;i++)
        {
          VTextureObject *pTex = NULL;
          if (m_iLightmapSamplerIndex[i]>=0)
          {
            // scene specific lightmaps?
            if (pSceneLightmaps!=NULL)
            {
              pTex = pSceneLightmaps[(m_pOwner->m_iIndexY*m_pOwner->m_Config.m_iSectorCount[0]+m_pOwner->m_iIndexX) * 4 + i];
              SetTextureBySamplerIndex(pTex, m_iLightmapSamplerIndex[i]);
            }
            else
            {
              pTex = GetTextureBySamplerIndex(m_iLightmapSamplerIndex[i]);
            }

          }

          GetSurfaceSafe().m_spModelLightmaps[i] = pTex;
        }
      }

      ar.Read(m_MaterialIDs, 9 * sizeof(short));
    }
  }
  else
  {
    char magic[4];

    magic[0] = 0x50;
    magic[1] = 0x61;
    magic[2] = 0x67;
    magic[3] = 0x65;

    ar.Write(magic, 4);


    // version 7:
    ar.Write(m_fLODThreshold,(MAX_MESH_LOD-1)*sizeof(float));
    ar << m_bHasHoles<< m_bAllHoles;
    if (m_bHasHoles && !m_bAllHoles)
    {
      short iLevels = pOwner->m_Config.m_iMaxMeshLODHoles+1;
      ar << iLevels;
      for (int i=0;i<iLevels;i++)
        m_HoleLODInfo[i].SerializeX(ar,iLocalVersion);
      VASSERT(m_spHoleIndexBuffer);
      m_spHoleIndexBuffer->SerializeX(ar);
    }

    VTextureObject* pNullTex = pOwner->GetSectorManager()->GetPlainBlackTexture();

    char iTextureType[16];
    for (int i=0;i<16;i++)
    {
      VTextureObject* pTex = GetTextureBySamplerIndex(i);
      if (pTex==NULL || pTex==pNullTex)
      {
        iTextureType[i] = SECTOR_TEXTURETYPE_NULL;
      }
      else if (pTex==Vision::TextureManager.GetPlainWhiteTexture())
      {
        iTextureType[i] = SECTOR_TEXTURETYPE_WHITE;
      }
      else if (pTex==Vision::TextureManager.GetPlainBlackTexture())
      {
        iTextureType[i] = SECTOR_TEXTURETYPE_BLACK;
      }
      else if (pTex==Vision::TextureManager.GetNeutralNormalMapTexture())
      {
        iTextureType[i] = SECTOR_TEXTURETYPE_NEUTRALNORMAL;
      }
      else
      {
        iTextureType[i] = SECTOR_TEXTURETYPE_FILE;
      }

      if (pOwner->m_Config.m_bSceneSpecificLightmaps)
      {
        // when using scene specific lightmaps, then save lightmap samplers as plain white and patch them at loading time
        for (int j=0;j<4;j++)
          if (i==m_iLightmapSamplerIndex[j])
            iTextureType[i] = SECTOR_TEXTURETYPE_WHITE;
      }

      ar << iTextureType[i];
      if (iTextureType[i] == SECTOR_TEXTURETYPE_FILE)
      {
        ANALYSIS_IGNORE_WARNING_ONCE(6246)
          VTextureObject *pTex = GetTextureBySamplerIndex(i);
        VStaticString<FS_MAX_PATH> sOrigName = pTex->GetFilename();
        const char *szFixedName = pTex->GetFilename();
        bool bNameChanged = false;
        if (!pOwner->m_Config.m_bUseTempFolder) // when we are saving to final directory, make sure that all filenames are correctly resolved
        {
          char szBuffer[FS_MAX_PATH];
          szFixedName = pOwner->m_Config.FixTempDirFile(sOrigName,szBuffer);
          bNameChanged = strcmp(sOrigName,szFixedName)!=0;
          if (bNameChanged)
            pTex->SetFilename(szFixedName);
        }
        VTextureObject::DoArchiveExchange(ar, pTex);
        if (bNameChanged)
          pTex->SetFilename(sOrigName);
      }

    }

    ar << m_iSamplerClampMask;

    ar << m_iPixelShaderRegCount;
    int iSize = sizeof(hkvVec4)*m_iPixelShaderRegCount;
    if (iSize>0)
    {
#if defined(_VR_DX11)
      hkvVec4* pPixelConst = (hkvVec4*) m_SectorPSConstBuffer.Lock(0,iSize,VIS_LOCKFLAG_READONLY);
#else
      hkvVec4* pPixelConst = (hkvVec4*) m_vPixelShaderConst;
#endif
      ar.Write(pPixelConst,iSize);
#if defined(_VR_DX11)
      m_SectorPSConstBuffer.Unlock();
#endif
    }

    ar << m_iNumThreeWayMappingLayer;
    if (m_iNumThreeWayMappingLayer > 0)
    {
      ar << m_iThreeWayMappingLayerMask;
    }

    ar.Write(m_iLightmapSamplerIndex,4); // 4 chars

    // exposed material IDs
    ar.Write(m_MaterialIDs, 9 * sizeof(short));
  }
}


#define TERRAINMESH_NORMALMAP_SAMPLER 5

void VTerrainSectorMeshPageInfo::UpdateNormalmapRegister(hkvVec4* pPixelConst)
{
  bool bOwnLock = pPixelConst==NULL;
  if (bOwnLock)
    pPixelConst = LockPixelConst();

  m_iPixelShaderRegCount = hkvMath::Max(m_iPixelShaderRegCount,TERRAINMESH_NORMALMAP_SAMPLER+1);

  // store the normalmap transformation in this (unused) register c7 (DX9) / c5 (DX11)

  hkvVec4 normalMapRegister;

  if (m_pOwner->m_Config.m_bNormalmapUsesBorder)
  {
    float fSizeX = static_cast<float>(m_pOwner->m_Config.m_iHeightSamplesPerSector[0]);
    float fSizeY = static_cast<float>(m_pOwner->m_Config.m_iHeightSamplesPerSector[1]);
    normalMapRegister.set(
      fSizeX/(fSizeX+4.f),
      fSizeY/(fSizeY+4.f),
      2.f/fSizeX,2.f/fSizeY);
  }
  else
  {
    normalMapRegister.set(1,1,0.0f,0.0f);
  }

  normalMapRegister = AdjustTransformForUnclampedUVs(normalMapRegister);

  pPixelConst[TERRAINMESH_NORMALMAP_SAMPLER] = normalMapRegister;

  if (bOwnLock)
    UnLockPixelConst();
}

void VTerrainSectorMeshPageInfo::BindSectorShaderTextures(VCompiledShaderPass *pShader,unsigned int iSamplerClampMask)
{
  // bind shader specific samplers

  VTextureObject *pTex=NULL;
  bool bLMRegTracked = false;
  VisSurface_cl &srf(GetSurfaceSafe());
  VStateGroupSampler *pClampSampler = (VStateGroupSampler *)VisRenderStates_cl::GetSamplerStateGroup((VisSamplerState_e)(VIS_SAMPLER_CLAMP|VIS_SAMPLER_FILTERING));
  for (unsigned int iSamplerIndex = 0; iSamplerIndex < pShader->GetActiveSamplerCount(VSS_PixelShader); iSamplerIndex++, iSamplerClampMask >>= 1)
  {
    VStateGroupSampler *pSampler = (iSamplerClampMask&1) ? pClampSampler : pShader->GetStateGroupSampler(VSS_PixelShader, iSamplerIndex);
    VStateGroupTexture* pTexture = pShader->GetStateGroupTexture(VSS_PixelShader, iSamplerIndex);

    switch (pTexture->m_cTextureType)
    {
    case TEXTURETYPE_BASE_TEXTURE:
    case TEXTURETYPE_REFMAP:
      pTex = pTexture->m_spCustomTex ? pTexture->m_spCustomTex.GetPtr() : GetTextureBySamplerIndex(iSamplerIndex);
      break;

    case TEXTURETYPE_NORMALMAP:
      pTex = srf.m_spNormalMap;
      break;
    case TEXTURETYPE_LIGHTMAP:
      if (!bLMRegTracked)
      {
        VisRenderStates_cl::VSSetModelUVToLightmap(AdjustTransformForUnclampedUVs(m_pOwner->GetLightmapScaleOffset()).data);
        bLMRegTracked = true;
      }
      pTex = srf.m_spModelLightmaps[pTexture->m_iTextureIndex];
      if (!pTex)
        pTex = Vision::TextureManager.GetSceneColorTexture();

      break;

    case TEXTURETYPE_LIGHTMASK:
      {
        if (!bLMRegTracked)
        {
          VisRenderStates_cl::VSSetModelUVToLightmap(AdjustTransformForUnclampedUVs(m_pOwner->GetLightmapScaleOffset()).data);
          bLMRegTracked = true;
        }
        VisLightMask_cl *pEntry = m_pOwner->GetLightMaskEntry(VisLightSource_cl::ElementManagerGet(pTexture->m_iTextureIndex));
        if (pEntry)
        {
          VisRenderStates_cl::VSSetLightmapToLightMaskTransform(pEntry->m_vUVScaleOfs.data);
          pTex = pEntry->m_spLightMask;
          //Vision::Shaders.DisplayDebugTexture(pTex);
        } else
          pTex = NULL;
      } break;
    case TEXTURETYPE_AUXILIARY_TEXTURE:
    case TEXTURETYPE_CUSTOM_TEXTURE:
    case TEXTURETYPE_CUBEMAP:
    case TEXTURETYPE_TEXTURE3D:
    case TEXTURETYPE_TEXTURE2DARRAY:
      pTex = pTexture->m_spCustomTex;
      break;
    default:
      VASSERT(FALSE);
    }

    Vision::RenderLoopHelper.BindMeshTexture(pTex,iSamplerIndex,pSampler); // for some samplers NULL==black is intended here
  }
}



void VTerrainSectorMeshPageInfo::UpdateHoleMask(VTerrainSector *pOwner, int px, int py)
{
  const VTerrainConfig &cfg = pOwner->m_Config;
  // check for holes:
  m_bHasHoles = false;
  m_bAllHoles = true;
  bool bHoleArray[4096];
  int iIndex=0;
  for (int ty=0;ty<cfg.m_iTilesPerSectorMesh[1];ty++)
    for (int tx=0;tx<cfg.m_iTilesPerSectorMesh[0];tx++,iIndex++)
    {
      VSectorTile *pTile = pOwner->GetTile(px*cfg.m_iTilesPerSectorMesh[0]+tx, py*cfg.m_iTilesPerSectorMesh[1]+ty);
      bHoleArray[iIndex] = pTile->IsHole();
      if (bHoleArray[iIndex])
        m_bHasHoles = true;
      else
        m_bAllHoles = false;
    }

    if (!m_bHasHoles || m_bAllHoles)
    {
      //m_spHoleIndexBuffer = NULL;
      return;
    }

    // rebuild index buffer
    int iIndexCount = 0;
    int iEstCount = 38 * (cfg.m_iHeightSamplesPerSector[0] / cfg.m_iSectorMeshesPerSector[0]) * (cfg.m_iHeightSamplesPerSector[1] / cfg.m_iSectorMeshesPerSector[1]);
    DynArray_cl<unsigned short> indices(iEstCount,0);

    // highest resolution does not have increased edge res.
    int iCount = VTerrainSectorManager::CreateLODIndexBuffer(cfg, 0,0,0,0,0,indices,iIndexCount,bHoleArray);
    for (int up=0;up<2;up++)
      for (int right=0;right<2;right++)
        for (int btm=0;btm<2;btm++)
          for (int left=0;left<2;left++)
            m_HoleLODInfo[0].m_EdgeInfo[up][right][btm][left].SetRange(iIndexCount/3,iCount/3);
    iIndexCount+=iCount;

    // higher LOD levels have edge resolution permutations
    for (int i=1;i<=cfg.m_iMaxMeshLODHoles;i++)
    {
      VSectorMeshLODLevelInfo_t &info = m_HoleLODInfo[i];
      // edges
      for (int up=0;up<2;up++)
        for (int right=0;right<2;right++)
          for (int btm=0;btm<2;btm++)
            for (int left=0;left<2;left++)
            {
              VSectorMeshRenderRange_t &range = info.m_EdgeInfo[up][right][btm][left];
              ANALYSIS_IGNORE_WARNING_ONCE(6246)
                int iCount = VTerrainSectorManager::CreateLODIndexBuffer(cfg, i,up,right,btm,left,indices,iIndexCount,bHoleArray);
              range.SetRange(iIndexCount/3,iCount/3);
              iIndexCount+=iCount;
            }
    }

    VASSERT(iIndexCount>0);

  if (!m_spHoleIndexBuffer)
  {
    m_spHoleIndexBuffer = new VisMeshBuffer_cl();
    m_spHoleIndexBuffer->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    #ifdef HK_DEBUG
    m_spHoleIndexBuffer->SetFilename("<TerrainHoleIndexBuffer>");
    #endif
  }

    m_spHoleIndexBuffer->AllocateIndexList(iIndexCount);
    unsigned short *pDest = (unsigned short *)m_spHoleIndexBuffer->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
    memcpy(pDest,indices.GetDataPtr(),iIndexCount*sizeof(unsigned short));
    m_spHoleIndexBuffer->UnLockIndices();

}

float GetXYDistanceToAABB (const hkvAlignedBBox& aabb, const hkvVec3& vPos);

void VTerrainSectorMeshPageInfo::PerformVisibility(VTerrainVisibilityCollectorComponent &infoComp, const VTerrainConfig &cfg)
{
  VTerrainVisibilityInfo &info(infoComp.m_VisibilityInfo);
  if (m_bAllHoles || !info.m_pCamFrustum->Overlaps(m_AbsBoundingBox))
    return;

  // clip in shadow context when not visible in main context
  if (info.m_pSMGenerator!=NULL && (!info.m_bCastDynamicShadows || !info.Helper_IsShadowBoxVisible(m_AbsBoundingBox)))
    return;

  // calculate LOD value
  float fDistance = cfg.m_bIgnoreHeightForLOD ? GetXYDistanceToAABB (m_AbsBoundingBox, info.m_vCamLODPos) : m_AbsBoundingBox.getDistanceTo(info.m_vCamLODPos);
  float fThreshold = fDistance * infoComp.m_fSectorDistLODMult;
  const int iMaxLod = m_bHasHoles ? (cfg.m_iMaxMeshLODHoles) : (cfg.m_iMaxMeshLOD-1);

  if (cfg.m_bDistanceBasedLODOnly)
  {
    int iLOD = (int)(fThreshold + 0.5f + infoComp.m_fLODBias);
    infoComp.m_pPageLOD[m_iGlobalIndex] = hkvMath::Min(iLOD,iMaxLod);
  }
  else
  {
    char &iCurrentLOD = infoComp.m_pPageLOD[m_iGlobalIndex];
    iCurrentLOD = (int)infoComp.m_fLODBias; // >=0 makes it visible at the same time
    while (fThreshold>=m_fLODThreshold[iCurrentLOD] && iCurrentLOD<iMaxLod)
      iCurrentLOD++;
  }
}

VisSurface_cl& VTerrainSectorMeshPageInfo::GetSurfaceSafe()
{
  if (!m_pSurface)
  {
    m_pSurface = m_pOwner->GetSectorManager()->m_SurfaceCache.GetNewInstance();
    m_pSurface->OnCreated(false); ///< no native callback
    m_pSurface->SetGeometryTopology(VisSurface_cl::VGT_TerrainMesh);
  }
  return *m_pSurface;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Editing only:
///////////////////////////////////////////////////////////////////////////////////////////////////


void VTerrainSectorMeshPageInfo::SetDetailTexture (int iSampler, int iTexCoordStream, VTextureObject *pTex, const hkvVec4& vTilingAndOfs, bool bUseClamping)
{
  hkvVec4 vFinalTileOfs = vTilingAndOfs;
  unsigned int iMask = 1<<iSampler;
  if (bUseClamping)
    m_iSamplerClampMask |= iMask;
  else
    m_iSamplerClampMask &= (~iMask);

  SetTextureBySamplerIndex(pTex,iSampler);
  if (iTexCoordStream>=2) // start pixel shader register at 2
  {
    iTexCoordStream -= 2;
#if defined(_VR_DX11)
    int iByteStart = iTexCoordStream*sizeof(hkvVec4);
    int iByteOverallSize = iByteStart + sizeof(hkvVec4);
    if (m_SectorPSConstBuffer.GetByteCount()<iByteOverallSize)
      ReallocPixelBuffer();
    hkvVec4* pReg = (hkvVec4*) m_SectorPSConstBuffer.Lock(iByteStart,sizeof(hkvVec4),VIS_LOCKFLAG_DISCARDABLE);
    *pReg = vFinalTileOfs;
    m_SectorPSConstBuffer.Unlock();
#else
    m_vPixelShaderConst[iTexCoordStream] = vFinalTileOfs;
#endif
    m_iPixelShaderRegCount = hkvMath::Max(m_iPixelShaderRegCount,iTexCoordStream+1);
  }
}

VTerrainSectorMeshPageInfo::~VTerrainSectorMeshPageInfo()
{
  if (m_pSurface)
  {
    m_pOwner->GetSectorManager()->m_SurfaceCache.RemoveInstance(m_pSurface);
    m_pSurface = NULL;
  }
}

int VTerrainSectorMeshPageInfo::GetSamplerIndex(const char *szSamplerName) const
{
  VCompiledTechnique *pTerrainFX = const_cast<VTerrainSectorMeshPageInfo*>(this)->GetSurfaceSafe().GetTechnique();
  if (!pTerrainFX || pTerrainFX->GetShaderCount()<1)
    return -1;

  // use the first shader to identify samplers by name
  VCompiledShaderPass *pShader = pTerrainFX->GetShader(0);
  return pShader->GetSamplerIndexByName(VSS_PixelShader, szSamplerName);
}

int VTerrainSectorMeshPageInfo::GetWeightMapSamplerCount() const
{
  int weightMapCount = 1; // minimum 1
  char szSamplerName[FS_MAX_PATH];
  for (int i = 1; i < 4; i++)
  {
    // weightmap texture file
    sprintf(szSamplerName, "Weightmap%i", i);
    if (GetSamplerIndex(szSamplerName) < 0)
    {
      break;
    }
    weightMapCount++;
  }

  return weightMapCount;
}

TERRAIN_IMPEXP hkvVec4 VTerrainSectorMeshPageInfo::AdjustTransformForUnclampedUVs(hkvVec4 uvTransform) const
{
  hkvVec4 result;

  result.x = m_unclampUVTransform.x * uvTransform.x;
  result.y = m_unclampUVTransform.y * uvTransform.y;
  result.z = uvTransform.z + m_unclampUVTransform.z * uvTransform.x;
  result.w = uvTransform.w + m_unclampUVTransform.w * uvTransform.y;

  return result;
}

VTerrainSectorMeshPageInfo::VTerrainSectorMeshPageInfo()
{
  memset(m_fLODThreshold,0,sizeof(m_fLODThreshold));
  m_bHasHoles = m_bAllHoles = false;
  m_iGlobalIndex = -1;
  m_iDirtyFlags = 0;
  m_iSamplerClampMask = 0;
  m_iPixelShaderRegCount = 0;
  m_iNumThreeWayMappingLayer = 0;
  m_iThreeWayMappingLayerMask = 0;
  m_pSurface = NULL;
  m_pOwner = NULL;
  m_iLightmapSamplerIndex[0] = m_iLightmapSamplerIndex[1] = m_iLightmapSamplerIndex[2] = m_iLightmapSamplerIndex[3] = -1;

  memset(m_MaterialIDs, 0, sizeof(m_MaterialIDs));
}


#if defined(_VR_DX11)

#define SAFE_PIXELREGISTER_COUNT  32

void VTerrainSectorMeshPageInfo::ReallocPixelBuffer()
{
  hkvVec4 szTemp[SAFE_PIXELREGISTER_COUNT];
  int iOldBytes = m_SectorPSConstBuffer.GetByteCount();
  VASSERT(iOldBytes<=(SAFE_PIXELREGISTER_COUNT*sizeof(hkvVec4)));
  if (iOldBytes>0)
  {
    memcpy(szTemp, m_SectorPSConstBuffer.Lock(0,iOldBytes,VIS_LOCKFLAG_READONLY), iOldBytes);
    m_SectorPSConstBuffer.Unlock();
  }

  m_SectorPSConstBuffer.AllocateBuffer(0,sizeof(hkvVec4)*SAFE_PIXELREGISTER_COUNT); // inside vForge, allocate enough

  if (iOldBytes>0)
  {
    memcpy(m_SectorPSConstBuffer.Lock(0,iOldBytes,VIS_LOCKFLAG_NONE), szTemp, iOldBytes);
    m_SectorPSConstBuffer.Unlock();
  }

}

#endif

hkvVec4* VTerrainSectorMeshPageInfo::LockPixelConst()
{
#if defined(_VR_DX11)
  VASSERT(m_iPixelShaderRegCount<=SAFE_PIXELREGISTER_COUNT);
  int iByteOverallSize = sizeof(hkvVec4)*m_iPixelShaderRegCount;
  if (m_SectorPSConstBuffer.GetByteCount()<iByteOverallSize)
    ReallocPixelBuffer();
  return (hkvVec4*) m_SectorPSConstBuffer.Lock(0,iByteOverallSize,VIS_LOCKFLAG_NONE);
#else
  return m_vPixelShaderConst;
#endif
}

void VTerrainSectorMeshPageInfo::UnLockPixelConst()
{
#if defined(_VR_DX11)
  m_SectorPSConstBuffer.Unlock();
#endif
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
