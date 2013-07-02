/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/EditableTerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>
#include <Vision/Runtime/Base/Math/Random/VRandom.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
#endif

enum { MAX_THREE_WAY_MAPPING_LAYER = 4 };

VEditableTerrainSector::VEditableTerrainSector(VTerrainSectorManager *pManager, const VTerrainConfig &config, int iIndexX, int iIndexY)
  : VTerrainSector(pManager,config,iIndexX,iIndexY), m_TempDecorationObj(0)
{
  m_iEditorFlags = SECTOR_EDITORFLAG_NONE;
  m_iUpdateDecorationCounter = m_iTempDecorationCount = 0;
  m_iRandomSeed = iIndexX*3 + iIndexY*991;
  m_bHoleTextureDirty = true;
}



VEditableTerrain *VEditableTerrainSector::GetTerrain() const
{
  return (VEditableTerrain *)GetSectorManager()->m_pTerrain;
}



void VEditableTerrainSector::SetEditorFlag(int iFlag) 
{
  m_iEditorFlags|=iFlag;
  if (m_iEditorFlags&SECTOR_EDITORFLAG_MESHDIRTY)
    m_bPrepared=false;

  //  if (m_iEditorFlags&SECTOR_EDITORFLAG_ANYTHINGDIRTY)
  //    RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);

}


void VEditableTerrainSector::RemoveEditorFlag(int iFlag) 
{
  m_iEditorFlags&=(~iFlag);

  //  if ((m_iEditorFlags&SECTOR_EDITORFLAG_ANYTHINGDIRTY)==0)
  //    SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
}


// sanity check of the sector
void VEditableTerrainSector::AssertValid()
{
#ifdef HK_DEBUG_SLOW
  int i;

  for (i=0;i<m_iReferencedDecorationCount;i++)
  {
    VASSERT(/*m_piReferencedDecoration[i]>=0 &&*/ m_piReferencedDecoration[i]<m_Decoration.m_iCount);
  }

  if (m_pTile)
  {
    VSectorTile *pTile = m_pTile;
    for (i=0;i<m_Config.m_iTilesPerSectorCount;i++,pTile++) 
      if (pTile->m_iReferencedDecorationCount)
      {
        int iFirst = pTile->m_iFirstReferencedDecoration;
        int iLast = iFirst+pTile->m_iReferencedDecorationCount-1;
        VASSERT(iFirst>=0 && iFirst<m_iReferencedDecorationCount);
        VASSERT(iLast>=0 && iLast<m_iReferencedDecorationCount);
      }
  }
#endif
}


void VEditableTerrainSector::ResetLightMasks()
{
  EnsureLoaded();
  m_iNumLightMaskEntries = 0;
  V_SAFE_DELETE_ARRAY(m_pLightMaskEntries);
  SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
}

bool VEditableTerrainSector::GetNeedsThreeWayMapping(const VTextureWeightmapChannel* pChannel) const
{
  const bool bChannelWantsThreeWayMapping = pChannel->m_spResource->m_Properties.m_bUseThreeWayMapping;
  if (!bChannelWantsThreeWayMapping)
    return false;

  const int iOfsX = m_iIndexX * m_Config.m_iHeightSamplesPerSector[0];
  const int iOfsY = m_iIndexY * m_Config.m_iHeightSamplesPerSector[1];
  const int iNrmlMapSizeX = m_Config.m_iHeightSamplesPerSector[0];
  const int iNrmlMapSizeY = m_Config.m_iHeightSamplesPerSector[1];

  const int iSizeX = pChannel->m_iSizeX;
  const int iSizeY = pChannel->m_iSizeY;
  const UBYTE* pWeight = pChannel->GetValuePtr(0, 0);

  const int iBlockSizeX = iSizeX / iNrmlMapSizeX;
  const int iBlockSizeY = iSizeY / iNrmlMapSizeY;
  const float fNormalZThreshold = 0.916f;
  hkvVec3 vNormal(hkvNoInitialization);
  VTerrainSectorManager* pSM = GetSectorManager();

  for (int y = 0; y < iNrmlMapSizeY; y++)
  {
    for (int x = 0; x < iNrmlMapSizeX; x++)
    {
      pSM->GetNormalAtFace(iOfsX+x,iOfsY+y,vNormal);
      if (vNormal.z < fNormalZThreshold)
      {
        // get maximum blend weight blockwise
        UBYTE maxWeight = 0;
        for (int j = 0; j < iBlockSizeY; j++)
        {
          for (int i = 0; i < iBlockSizeX; i++)
          {
            const int iIndex = (y * iBlockSizeY + j) * iSizeX + (x * iBlockSizeX + i);
            maxWeight = hkvMath::Max(maxWeight, pWeight[iIndex]);
          }
        }
        if (maxWeight > 7)
          return true;
      }
    }
  }

  return false;
}

bool VEditableTerrainSector::SaveWeightmapTextures()
{
  //Nothing to do. Don't force load. Outside code can do that if it really wants to save unloaded sectors
  if (!IsLoaded())
  {
    return true;
  }

  char szPath[FS_MAX_PATH];

  if (!m_Config.GetAuxiliaryTextureDir(szPath,true))
    return false;

  char szFormat[FS_MAX_PATH];
  VFileHelper::CombineDirAndFile(szFormat,szPath,"Weightmap");
  strcat(szFormat,"%i_");
  sprintf(szPath,"%02i_%02i.dds",m_iIndexX,m_iIndexY);
  strcat(szFormat,szPath);

  if(m_UsedWeightmapChannels.Count() < 1)
  {
    return true;
  }


  int weightMapWidth = m_UsedWeightmapChannels.GetAt(0)->m_iSizeX;
  int weightMapHeight = m_UsedWeightmapChannels.GetAt(0)->m_iSizeY;

  int weightMapCount = 1;

  // Number of weight maps used is the largest number of supported weightmaps across all pages
  VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
  for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
  {
    for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
    {
      weightMapCount = hkvMath::Max(weightMapCount, pPage->GetWeightMapSamplerCount());
    }
  }

  VRefCountedCollection<VisBitmap_cl> weightMaps;

  for(int weightMapIdx = 0; weightMapIdx < weightMapCount; weightMapIdx++)
  {
    VisBitmap_cl* bitmap = new VisBitmap_cl(NULL, weightMapWidth, weightMapHeight);
    bitmap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    weightMaps.Add(bitmap);
  }

  int perPageWeightMapWidth = weightMapWidth / m_Config.m_iSectorMeshesPerSector[0];
  int perPageWeightMapHeight = weightMapHeight / m_Config.m_iSectorMeshesPerSector[1];

  int numPages = m_Config.m_iSectorMeshesPerSector[0] * m_Config.m_iSectorMeshesPerSector[1];

  VArray<int> usedChannelsPerPage(numPages);

  pPage = m_pMeshPage;
  for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
  {
    for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
    {
      int pageWeightMapCount = pPage->GetWeightMapSamplerCount();
      int maxUsedChannels = pageWeightMapCount * 4 + 1;

      VArray<VTextureWeightmapChannel*> usedChannels;
      usedChannels.SetSize(maxUsedChannels);

      hkvRect pageRegion(iPageX * perPageWeightMapWidth, iPageY * perPageWeightMapHeight, perPageWeightMapWidth, perPageWeightMapHeight);

      int numUsedChannels = m_UsedWeightmapChannels.BakeWeightMaps(maxUsedChannels, 1.0f / 255.0f, pageRegion, weightMaps.GetPtrs(), usedChannels.GetData());
      usedChannelsPerPage.Append(numUsedChannels);

      // handle three way mapping
      char channelRemapping[256];
      VASSERT(numUsedChannels < V_ARRAY_SIZE(channelRemapping));
      memset(channelRemapping, 0, sizeof(channelRemapping));

      pPage->m_iNumThreeWayMappingLayer = 0;
      pPage->m_iThreeWayMappingLayerMask = 0;
      for (int i = 1; i < numUsedChannels; i++)
      {
        if (GetNeedsThreeWayMapping(usedChannels[i]))
        {
          ++pPage->m_iNumThreeWayMappingLayer;
          channelRemapping[i] = pPage->m_iNumThreeWayMappingLayer;
          pPage->m_iThreeWayMappingLayerMask |= V_BIT(i);
        }
      }

      int iTmpCounter = pPage->m_iNumThreeWayMappingLayer;
      for (int i = 1; i < numUsedChannels; i++)
      {
        if (!(pPage->m_iThreeWayMappingLayerMask & V_BIT(i)))
        {
          ++iTmpCounter;
          channelRemapping[i] = iTmpCounter;
        }
      }

      // the first relevant channel is the base texture
      hkvVec4 vBaseScalingOfs(1.f,1.f,0.f,0.f);
      pPage->GetSurfaceSafe().m_spDiffuseTexture = Vision::TextureManager.GetPlainWhiteTexture();
      VTextureObject *pBaseNrml = Vision::TextureManager.GetNeutralNormalMapTexture();
      VTextureObject *pBaseSpec = NULL;
      bool bBaseClamping = false;

      if (numUsedChannels > 0)
      {
        VTextureWeightmapChannelResource *pRes = usedChannels[0]->m_spResource;
        pPage->GetSurfaceSafe().m_spDiffuseTexture = pRes->m_Properties.GetDiffuseTexture(m_iIndexX,m_iIndexY);
        pBaseNrml = pRes->m_Properties.GetNormalmapTexture(m_iIndexX,m_iIndexY);
        if (!pBaseNrml)
        {
          pBaseNrml = Vision::TextureManager.GetNeutralNormalMapTexture();
        }
        pBaseSpec = pRes->m_Properties.GetSpecularTexture(m_iIndexX,m_iIndexY);
        pRes->m_Properties.GetFinalTileAndOfs(m_Config, vBaseScalingOfs,m_iIndexX,m_iIndexY);
        bBaseClamping = pRes->m_Properties.m_bUseClamping;

        pPage->m_MaterialIDs[0] = pRes->m_Properties.m_iMaterialID;
      }

      int iSampler = pPage->GetSamplerIndex("DiffuseReplacement");
      if (iSampler>=0)
      {
        char szPathname[FS_MAX_PATH];
        if (m_Config.GetSectorDiffuseReplacementFilename(szPathname,m_iIndexX,m_iIndexY))
          pPage->SetTextureBySamplerIndex(Vision::TextureManager.Load2DTexture(szPathname), iSampler);
      }

      pPage->m_iSamplerClampMask = 0;
      pPage->m_iPixelShaderRegCount = 0;

      // now set the relevant mapped sampler texture on the base terrain sectors
      iSampler = pPage->GetSamplerIndex("Normalmap");
      if (iSampler>=0)
        pPage->SetTextureBySamplerIndex(pPage->GetSurfaceSafe().m_spNormalMap, iSampler);

      // setup the samplers for the base pass
      iSampler = pPage->GetSamplerIndex("DetailBase");
      if (iSampler>=0)
        pPage->SetDetailTexture(iSampler,iSampler,pPage->GetSurfaceSafe().m_spDiffuseTexture,vBaseScalingOfs,bBaseClamping);
      iSampler = pPage->GetSamplerIndex("NormalmapBase");
      if (iSampler>=0)
        pPage->SetDetailTexture(iSampler,-1,pBaseNrml,vBaseScalingOfs,bBaseClamping);
      iSampler = pPage->GetSamplerIndex("SpecularmapBase");
      if (iSampler>=0)
        pPage->SetDetailTexture(iSampler,-1,pBaseSpec,vBaseScalingOfs,bBaseClamping);

      // set the mapped samplers on the sector:
      for (int iChannelIndex=0;iChannelIndex < numUsedChannels - 1;iChannelIndex++)
      {
        // detail textures
        VTextureObject *pDetailTex = NULL; // fill up the unused samplers with NULL textures
        VTextureObject *pNormalmapTex = Vision::TextureManager.GetNeutralNormalMapTexture();
        VTextureObject *pSpecularTex = NULL;

        hkvVec4 vTiling(1.f,1.f,0.f,0.f);
        bool bUseClamping = false;
        if (iChannelIndex<numUsedChannels-1)
        {
          VTextureWeightmapChannelResource *pRes = usedChannels[iChannelIndex+1]->m_spResource;
          pDetailTex = pRes->m_Properties.GetDiffuseTexture(m_iIndexX,m_iIndexY);
          pNormalmapTex = pRes->m_Properties.GetNormalmapTexture(m_iIndexX,m_iIndexY);
          if (!pNormalmapTex)
            pNormalmapTex = Vision::TextureManager.GetNeutralNormalMapTexture();
          pSpecularTex = pRes->m_Properties.GetSpecularTexture(m_iIndexX,m_iIndexY);
          pRes->m_Properties.GetFinalTileAndOfs(m_Config, vTiling,m_iIndexX,m_iIndexY);;
          bUseClamping = pRes->m_Properties.m_bUseClamping;

          // store material ID so it is available on native side
          if ((iChannelIndex + 1) < 9)
            pPage->m_MaterialIDs[iChannelIndex + 1] = pRes->m_Properties.m_iMaterialID;
        }
        const int iTexSamplerIndex = channelRemapping[iChannelIndex+1] - 1;

        char szSamplerName[FS_MAX_PATH];
        sprintf(szSamplerName,"DetailTexture%i", iTexSamplerIndex);
        iSampler = pPage->GetSamplerIndex(szSamplerName);
        if (iSampler>=0) // diffuse texture also defines the tiling factor
          pPage->SetDetailTexture(iSampler, iSampler, pDetailTex, vTiling, bUseClamping);

        sprintf(szSamplerName,"DetailNormalmap%i", iTexSamplerIndex);
        iSampler = pPage->GetSamplerIndex(szSamplerName);
        if (iSampler>=0)
          pPage->SetDetailTexture(iSampler, -1, pNormalmapTex, vTiling, bUseClamping);

        sprintf(szSamplerName,"DetailSpecularmap%i", iTexSamplerIndex);
        iSampler = pPage->GetSamplerIndex(szSamplerName);
        if (iSampler>=0)
          pPage->SetDetailTexture(iSampler, -1, pSpecularTex, vTiling, bUseClamping);

      }

      if (pPage->m_iNumThreeWayMappingLayer > 0)
      {

        pPage->m_iPixelShaderRegCount = hkvMath::Max(pPage->m_iPixelShaderRegCount, 11);

        hkvVec4* vPixelConstants = (hkvVec4*) pPage->LockPixelConst();
        for (int i = 1; i <= MAX_THREE_WAY_MAPPING_LAYER; i++)
        {
          hkvVec4 tmp(false);
          tmp.x = (i == 1) ? 1.0f : 0.0f;
          tmp.y = (i == 2) ? 1.0f : 0.0f;
          tmp.z = (i == 3) ? 1.0f : 0.0f;
          tmp.w = (i == 4) ? 1.0f : 0.0f;
          char mapping = channelRemapping[i] + 6;
          vPixelConstants[mapping] = tmp;
        }
        pPage->UnLockPixelConst();
      }

      pPage->UpdateNormalmapRegister(NULL);
    }
  }

  // the first channel is always the solid one
  if (m_UsedWeightmapChannels.Count()>0)
    m_UsedWeightmapChannels.GetAt(0)->Clear(255);

  const char* pszProjectDir = (m_Config.m_bUseTempFolder) ? NULL : m_Config.m_sAbsProjectDir.AsChar();

  for(int weightMapIdx = 0; weightMapIdx < weightMapCount; weightMapIdx++)
  {
    char szFileName[FS_MAX_PATH];
    sprintf(szFileName,szFormat,weightMapIdx);

    VisBitmap_cl* bitMap = weightMaps.GetAt(weightMapIdx);
    if (pszProjectDir)
    {
      VFileAccessManager::RCSPerformAction(szFileName, RCS_EDIT, pszProjectDir); //make sure we can write to it
    }

    // upload texture in-place
    VTextureObject *pExistingTex = Vision::TextureManager.GetTextureObjectByName(szFileName);

    // after rescaling of the resolution the following might happen:
    if (pExistingTex!=NULL && (
      pExistingTex->GetTextureWidth() != weightMapWidth ||
      pExistingTex->GetTextureHeight() != weightMapHeight ||
      pExistingTex->GetTextureFormat() != VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP
      ))
    {
      pExistingTex->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
      pExistingTex->SetFilename("<garbage>");
      pExistingTex->Purge();
      pExistingTex = NULL;
    }

    IVTerrainTextureBaker* pCustomBaker = VTerrainManager::GlobalManager().GetCustomTextureBaker();
    if (pCustomBaker==NULL || !pCustomBaker->SaveWeightmap(bitMap, szFileName, 4, pExistingTex))
    {
      if (pExistingTex==NULL)
      {
        pExistingTex = Vision::TextureManager.Create2DTextureObject(szFileName,weightMapWidth,weightMapHeight,1,VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP);
      }
      else
      {
        VASSERT_MSG(pExistingTex->GetTextureFormat() == VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP, "Existing weightmap should be in %s format, but was %s",
          VTextureLoader::GetTextureFormatString(VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP, NULL),
          VTextureLoader::GetTextureFormatString(pExistingTex->GetTextureFormat(), NULL));
      }

      pExistingTex->UpdateRect(0, 0,0,weightMapWidth,weightMapHeight,weightMapWidth*4,bitMap->GetDataPtr(),V_TEXTURE_LOCKFLAG_DISCARDABLE);
      const bool bSaveModifiedWeightmaps = true;
      if (bSaveModifiedWeightmaps)
      {
        bitMap->SaveToFile(szFileName);
      }

    }

    if (pszProjectDir)
    {
      VFileAccessManager::RCSPerformAction(szFileName, RCS_ADD, pszProjectDir); //make sure it's added to RCS
    }
  }

  // Set weightmap on sectors - do this now since the texture pointer may have changed
  pPage = m_pMeshPage;
  int iPageIdx = 0;
  for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
  {
    for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++,iPageIdx++)
    {
      // rounded number of weightmaps (note the first one is basetexture and does not count)
      int iNumWeightMaps = (usedChannelsPerPage[iPageIdx]+2)/4;

      for(int iWeightMapIdx = 0; iWeightMapIdx < 4; iWeightMapIdx++)
      {
        // weightmap texture file
        char szSamplerName[FS_MAX_PATH];
        sprintf(szSamplerName,"Weightmap%i",iWeightMapIdx);
        int iSampler = pPage->GetSamplerIndex(szSamplerName);
        if (iSampler<0)
          continue;

        VTextureObject *pWeightTex = NULL;

        if(iWeightMapIdx < iNumWeightMaps)
        {
          sprintf(szPath,szFormat,iWeightMapIdx);
          pWeightTex = Vision::TextureManager.Load2DTexture(szPath);
          if (pWeightTex==NULL)
          {
            pWeightTex = GetSectorManager()->GetPlainBlackTexture(); // we need real black textures here (a=0)
          }
          else
          {
            pWeightTex->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
          }
        }
        else
        {
          // we want plain black here (note that the engine's plainblack has a==255)
          pWeightTex = GetSectorManager()->GetPlainBlackTexture();
        }

        pPage->SetTextureBySamplerIndex(pWeightTex, iSampler);
      }
    }
  }

  OnDefaultShaderEffectChanged();

  // generate material map
  m_spMaterialIDMap = m_UsedWeightmapChannels.CreateMaterialIDMap(m_Config, m_spMaterialIDMap);


  // since sampler layout changed:
  SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
  return true;
}

void VEditableTerrainSector::EnsureEditingDataLoaded()
{
	if (m_bEditingDataLoaded)
		return;
	m_bEditingDataLoaded = true;

	LoadSectorEditingInformation();

	// add the base texture channel if there is none yet:
	if (m_UsedWeightmapChannels.Count()==0 && GetTerrain()->m_WeightmapChannels.Count()>0)
	{
		int iResX = m_Config.m_iDefaultWeightmapResolution[0]; // this might become sector specific
		int iResY = m_Config.m_iDefaultWeightmapResolution[1];

		VTextureWeightmapChannel *pChannel = m_UsedWeightmapChannels.CreateChannel(m_Config,GetTerrain()->m_WeightmapChannels.GetAt(0),iResX,iResY);
		pChannel->Clear(255); // the first channel starts with white

		//Don't do saving in reload anymore
		// SaveWeightmapTextures();
		SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);
	}

	// fixup sectors saved in wrong order...
	if (m_Config.m_bSortTextureChannels)
  {
		m_UsedWeightmapChannels.SortByResourceSortingKey(false);
  }
}

bool VEditableTerrainSector::LoadSectorEditingInformation()
{
  char szFilename[FS_MAX_PATH];
  if (!m_Config.GetSectorEditingFilename(szFilename,m_iIndexX,m_iIndexY,false))
    return false;

  VChunkFile file;
  if (!file.Open(szFilename,Vision::File.GetManager()))
    return false;

  int iVersion,iCount;
  // detail textures
  CHUNKIDTYPE chunkID;

  while (!file.IsEOF())
  {
    if (!file.OpenChunk(&chunkID))
      break;

    if (chunkID=='DTEX')
    {
      file.ReadInt(iVersion); VASSERT(iVersion==0);
      file.ReadInt(iCount);
      m_UsedWeightmapChannels.Clear();
      m_UsedWeightmapChannels.EnsureCapacity(iCount);
      for (int i=0;i<iCount;i++)
      {
        VTextureWeightmapChannel *pChannel = VTextureWeightmapChannel::ChunkFileExchange(file,NULL,GetTerrain());
        if (pChannel) // only add the channels that exist as resource
          m_UsedWeightmapChannels.Add(pChannel);
      }
    }

    if (chunkID=='DECO')
    {
      file.ReadInt(iVersion); VASSERT(iVersion==0);
      file.ReadInt(iCount);
      m_DecorationChannels.Clear();
      m_DecorationChannels.EnsureCapacity(iCount);
      for (int i=0;i<iCount;i++)
      {
        VDecorationDensityChannel *pChannel = VDecorationDensityChannel::ChunkFileExchange(file,NULL,GetTerrain());
        if (pChannel)
          m_DecorationChannels.Add(pChannel);

      }
    }

    file.EndChunk();
  }
  file.Close();

  // Update timestamps of lock targets
  UpdateFileLockTimeStamp(VTC_FILE_EDITING_DATA);

  return !file.IsInErrorState();
}

bool VEditableTerrainSector::SaveSectorEditingInformation()
{
  char szFilename[FS_MAX_PATH];
  if (!m_Config.GetSectorEditingFilename(szFilename,m_iIndexX,m_iIndexY,true))
    return false;

  {
    // Create Editing dir if it doesn't exist yet
    char szFileDir[FS_MAX_PATH];
    VFileHelper::GetFileDir(szFilename, szFileDir);
    VString sAbspath = VFileHelper::CombineDirAndDir(m_Config.m_sAbsProjectDir, szFileDir);
    if (!VFileHelper::ExistsDir(sAbspath))
    {
      VFileHelper::MkDir(sAbspath);
    }
  }

  m_Config.RCSPerformAction(szFilename, RCS_EDIT); //make sure we can write to it

  VChunkFile file;
  if (!file.Create(szFilename,Vision::File.GetOutputManager()))
    return false;

  // detail textures
  file.StartChunk('DTEX');
  {
    file.WriteInt(0); // version

    int iCount = 0;
    //    m_UsedWeightmaps.SortByResourceSortingKey(false); // should be sorted already though

    // first count used ones:
    for (int i=0;i<m_UsedWeightmapChannels.Count();i++)
    {
      VTextureWeightmapChannel *pChannel = m_UsedWeightmapChannels.GetAt(i);
      if (i==0) // the first one is always solid
        pChannel->Clear(255);

      if (pChannel->IsRelevantForSaving())
        iCount++;
    }

    // write out channel luminance values:
    file.WriteInt(iCount);
    for (int i=0;i<m_UsedWeightmapChannels.Count();i++)
      if (m_UsedWeightmapChannels.GetAt(i)->IsRelevantForSaving())
        VTextureWeightmapChannel::ChunkFileExchange(file,m_UsedWeightmapChannels.GetAt(i),GetTerrain());
  }
  file.EndChunk();


  // decoration channels
  file.StartChunk('DECO');
  {
    file.WriteInt(0); // version

    int iCount = 0;
    for (int i=0;i<m_DecorationChannels.Count();i++)
      if (m_DecorationChannels.GetAt(i)->IsRelevantForSaving())
        iCount++;

    // write out channel luminance values:
    file.WriteInt(iCount);
    for (int i=0;i<m_DecorationChannels.Count();i++)
      if (m_DecorationChannels.GetAt(i)->IsRelevantForSaving())
        VDecorationDensityChannel::ChunkFileExchange(file,m_DecorationChannels.GetAt(i),GetTerrain());
  }
  file.EndChunk();

  file.Close();

  m_Config.RCSPerformAction(szFilename, RCS_ADD);
  UpdateFileLockTimeStamp(VTC_FILE_EDITING_DATA);

  return !file.IsInErrorState();
}


bool VEditableTerrainSector::SaveReplacementMesh()
{
  EnsureLoaded();

  char szFilename[FS_MAX_PATH];
  if (!m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"lowres",true))
    return false;

  m_Config.RCSPerformAction(szFilename, RCS_EDIT); //make sure we can write to it

  IVFileOutStream *pOut = GetParentManager()->CreateFileOutStream(szFilename,this);
  if (!pOut)
    return false;

  VArchive ar(NULL,pOut,Vision::GetTypeManager());

  int iLocalVersion = 0;
  ar << Vision::GetArchiveVersion();
  ar << iLocalVersion; // local version

  // save only height values
  ar << m_Config.m_iSamplesPerReplacementMesh[0] << m_Config.m_iSamplesPerReplacementMesh[1];

  int iStepX = m_Config.m_iHeightSamplesPerSector[0] / m_Config.m_iSamplesPerReplacementMesh[0];
  int iStepY = m_Config.m_iHeightSamplesPerSector[1] / m_Config.m_iSamplesPerReplacementMesh[1];
  for (int y=0;y<=m_Config.m_iSamplesPerReplacementMesh[1];y++)
    for (int x=0;x<=m_Config.m_iSamplesPerReplacementMesh[0];x++)
    {
      float fH = GetHeightAt(x*iStepX, y*iStepY);
      ar << fH;
    }

    ar.Close();
    pOut->Close();
    m_Config.RCSPerformAction(szFilename, RCS_ADD);

    return true;
}



/*
void VEditableTerrainSector::UpdateReplacementMesh()
{
EnsureLoaded();

if (!m_spReplacementMesh)
m_spReplacementMesh = new VisMeshBuffer_cl();

int iVertexCount = (m_Config.m_iSamplesPerReplacementMesh[0]+1)*(m_Config.m_iSamplesPerReplacementMesh[1]+1);
int iIndexCount = m_Config.m_iSamplesPerReplacementMesh[0]*m_Config.m_iSamplesPerReplacementMesh[1]*6;
m_spReplacementMesh->AllocateVertices(VTerrainVertexLowRes_t::GetDesc(), iVertexCount, VIS_MEMUSAGE_STATIC);

VTerrainVertexLowRes_t *pV = (VTerrainVertexLowRes_t *)m_spReplacementMesh->LockVertices(VIS_LOCKFLAG_DISCARDABLE);

int iStepX = m_Config.m_iHeightSamplesPerSector[0] / m_Config.m_iSamplesPerReplacementMesh[0];
int iStepY = m_Config.m_iHeightSamplesPerSector[1] / m_Config.m_iSamplesPerReplacementMesh[1];
int iOfsX = m_iIndexX * m_Config.m_iHeightSamplesPerSector[0];
int iOfsY = m_iIndexY * m_Config.m_iHeightSamplesPerSector[1];

for (int y=0;y<=m_Config.m_iSamplesPerReplacementMesh[1];y++)
for (int x=0;x<=m_Config.m_iSamplesPerReplacementMesh[0];x++,pV++)
{
int iSampleX = x*iStepX;
int iSampleY = y*iStepY;
pV->vPos.x = m_Config.GetHeightmapSamplePosX((float)(iSampleX+iOfsX));
pV->vPos.y = m_Config.GetHeightmapSamplePosX((float)(iSampleY+iOfsY));
pV->vPos.z = GetHeightAt(iSampleX, iSampleY) + 50.f;
}

m_spReplacementMesh->UnLockVertices();

m_spReplacementMesh->AllocateIndexList(iIndexCount,VIS_INDEXFORMAT_16);
unsigned short *pInd = (unsigned short *)m_spReplacementMesh->LockIndices(VIS_LOCKFLAG_DISCARDABLE);
int iRowStride = m_Config.m_iSamplesPerReplacementMesh[0]+1;
for (int y=0;y<m_Config.m_iSamplesPerReplacementMesh[1];y++)
for (int x=0;x<m_Config.m_iSamplesPerReplacementMesh[0];x++,pInd+=6)
{
int iFirst = y*iRowStride+x;
pInd[0] = iFirst+iRowStride+1;
pInd[1] = iFirst+iRowStride;
pInd[2] = iFirst;
pInd[3] = iFirst+1;
pInd[4] = iFirst+iRowStride+1;
pInd[5] = iFirst;
}

m_spReplacementMesh->UnLockIndices();

m_spReplacementMesh->SetPrimitiveType(VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST);
m_spReplacementMesh->SetPrimitiveCount(iIndexCount/3);
}
*/


void VEditableTerrainSector::AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
{
  VTerrainSector::AccumulateMemoryFootprint(iUniqueSys,iUniqueGPU,iDependentSys,iDependentGPU);

  if (m_spNormalmapSysMem)
    m_spNormalmapSysMem->IncOverallMemSizes(iDependentSys,iDependentGPU);

  for (int i=0;i<m_DecorationChannels.Count();i++)
    iUniqueSys += m_DecorationChannels.GetAt(i)->GetSampleCount() + sizeof (*m_DecorationChannels.GetAt(i));

  for (int i=0;i<m_UsedWeightmapChannels.Count();i++)
    iUniqueSys += m_UsedWeightmapChannels.GetAt(i)->GetSampleCount() + sizeof (*m_UsedWeightmapChannels.GetAt(i));

}

void VEditableTerrainSector::PrepareSector()
{
  if (!m_bPrepared)
  {
    VTerrainSector::PrepareSector();
    bool bNiceUpdate = true; // isn't much slower

    // update the terrain mesh
    if (IsEditorFlagSet(SECTOR_EDITORFLAG_MESHDIRTY))
    {
      UpdateMesh(false);
      UpdateNormalmap(bNiceUpdate);
      RemoveEditorFlag(SECTOR_EDITORFLAG_MESHDIRTY);
    }

    if (!m_pMeshPage[0].GetSurfaceSafe().m_spNormalMap)
    {
      UpdateNormalmap(bNiceUpdate);
    }

    m_bPrepared=true;
  }
}


BOOL VEditableTerrainSector::Reload()
{
  m_Config.m_bUseTempFolder = IsEditorFlagSet(SECTOR_EDITORFLAG_TEMPDIR);
  BOOL bResult = VTerrainSector::Reload();
  m_Config.m_bUseTempFolder = IsEditorFlagSet(SECTOR_EDITORFLAG_TEMPDIR); // set again (might recurse)

  //Don't do saving in reload anymore
  /*
  // save the heightmap file if not already saved. We need it for some operations such as file locking
  char szFilename[FS_MAX_PATH];
  m_Config.GetSectorCacheFilename(szFilename,m_iIndexX,m_iIndexY,"hmap",false);
  if (!Vision::File.Exists(szFilename))
  SaveHeightmap();
  */

  LoadSectorEditingInformation();

  // add the base texture channel if there is none yet:
  if (m_UsedWeightmapChannels.Count()==0 && GetTerrain()->m_WeightmapChannels.Count()>0)
  {
    int iResX = m_Config.m_iDefaultWeightmapResolution[0]; // this might become sector specific
    int iResY = m_Config.m_iDefaultWeightmapResolution[1];

    VTextureWeightmapChannel *pChannel = m_UsedWeightmapChannels.CreateChannel(m_Config,GetTerrain()->m_WeightmapChannels.GetAt(0),iResX,iResY);
    pChannel->Clear(255); // the first channel starts with white

    //Don't do saving in reload anymore
    // SaveWeightmapTextures();
    SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);
  }

  // fixup sectors saved in wrong order...
  if (m_Config.m_bSortTextureChannels)
    m_UsedWeightmapChannels.SortByResourceSortingKey(false);

  if (!m_bSectorFileLoaded)
    SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);

  //Make sure everything is up to date
  m_bPrepared=false;
  PrepareSector();
  // 'VTC_FILE_EDITING_DATA' is updated in 'LoadSectorEditingInformation'.
  UpdateFileLockTimeStamp(VTC_FILE_SECTORS_HMAP);
  UpdateFileLockTimeStamp(VTC_FILE_SECTORS_MESH);

  // In case that we are reloading a sector that was previously locked by others,
  // we reset the flag as in may no longer be valid.
  RemoveEditorFlag(SECTOR_EDITORFLAG_NETWORKLOCK);

  return bResult;
}


#ifdef HK_DEBUG_SLOW

void VEditableTerrainSector::AssertCorrectFileRefs(const VResourceSnapshot &snapshot, const char *szFilename)
{
  if (m_Config.IsTempDirFile(szFilename))
    return;

  VASSERT(!m_Config.IsTempDirFile(szFilename));

  int iCount = snapshot.GetResourceEntryCount();
  for (int i=0;i<iCount;i++)
  {
    VResourceSnapshotEntry& entry(snapshot.GetResourceEntry(i));
    const char *szFile = entry.GetFileName();
    VASSERT(!m_Config.IsTempDirFile(szFile));
  }
}

#endif

BOOL VEditableTerrainSector::Unload()
{
  //reset decorations
  m_iTempDecorationCount = 0;
  m_TempDecorationObj.Reset();

  //we don't want this sector to update anymore
  GetTerrain()->m_PendingPhysicsUpdateSectors.SafeRemove(this);

  if (GetTerrain()->m_bSaveModifiedFiles) // should we swap out modified sectors?
  {
    // SECTOR_EDITORFLAG_MESHDIRTY is ignored here because it has no effect on the files written to disk
    // and is only used to update the rendering mesh at runtime which is derived from the data on disk.
    if (IsAnyEditorFlagSet(SECTOR_EDITORFLAG_ANYTHINGDIRTY & ~SECTOR_EDITORFLAG_MESHDIRTY))
    {
      // if not yet swapped to temp dir, then make sure everything gets saved
      if (!IsEditorFlagSet(SECTOR_EDITORFLAG_TEMPDIR))
      {
        SetEditorFlag(SECTOR_EDITORFLAG_ANYTHINGDIRTY);
        SetEditorFlag(SECTOR_EDITORFLAG_TEMPDIR); // from now on, load from this dir
      }

      // swap modified data to temp folder
      SaveSector(true);
    }
  }

  // base implementation does not save anything
  BOOL bResult = VTerrainSector::Unload();

  m_Config.m_bUseTempFolder = false;

  m_spHoleOverlay = NULL;
  m_spNormalmapSysMem = NULL;

  m_UsedWeightmapChannels.Clear();
  m_DecorationChannels.Clear();

  // Reset timestamps as we have just unloaded the sector and thus we no
  // longer have a valid state in order to lock the sector.
  m_SectorInfoFileTime = VFileTime();
  m_SectorHMapFileTime = VFileTime();
  m_SectorMeshFileTime = VFileTime();

  return bResult;
}

void VEditableTerrainSector::OnConfigChanged()
{
  // set flags that guarantee that the changed info gets saved, because VTerrainSector::OnConfigChanged() calls EnsureUnloaded
  SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
  SetEditorFlag(SECTOR_EDITORFLAG_MESHDIRTY);

  // re-scale the weightmaps before unloading in VTerrainSector::OnConfigChanged() because there they get baked with new sizes
  for (int i=0;i<m_UsedWeightmapChannels.Count();i++)
    m_UsedWeightmapChannels.GetAt(i)->Rescale(m_Config.m_iDefaultWeightmapResolution[0],m_Config.m_iDefaultWeightmapResolution[1]);

  UpdateDecoration(true);
  VTerrainSector::OnConfigChanged();
}


//Used by final save to save edited or swapped sectors over final sectors and to generate the resource snapshot
//Always saves to the final directory. 
//Only saves modified or swapped out sectors
bool VEditableTerrainSector::SaveSectorFinal(bool bSaveSnapshot)
{
  BOOL bOldStatus = IsLoaded();

  bool bForceResave = false;
#ifdef SUPPORTS_SNAPSHOT_CREATION
  // vres file does not exist? Then force re-saving it
  char szVRESFilename[FS_MAX_PATH];
  if (bSaveSnapshot && !IsLoaded() 
    && m_Config.GetSectorCacheFilename(szVRESFilename,m_iIndexX,m_iIndexY,"vres",true) 
    && !Vision::File.Exists(szVRESFilename))
    bForceResave = true;
#endif

  if (bForceResave || IsEditorFlagSet(SECTOR_EDITORFLAG_TEMPDIR) ) //swapped to temp folder?
  {
    m_Config.m_bUseTempFolder = true; // ...load from temp dir
    EnsureLoaded();   //Load it if it isn't loaded yet. 
    RemoveEditorFlag(SECTOR_EDITORFLAG_TEMPDIR);  //no need to use temp dir anymore, it's going to be saved in the normal dir
    SetEditorFlag(SECTOR_EDITORFLAG_ANYTHINGDIRTY);  //we want to save out everything
  }

  if (IsLoaded())
  {
    PrepareSector();
    bool bRes = VTerrainSector::SaveSectorFinal(bSaveSnapshot);

    if (!bOldStatus)   //Wasn't loaded before?
      EnsureUnloaded();  //Unload it again
    return bRes;
  }

  return true;    // nothing loaded, so nothing to save
}


class VUpdateNormalmapTask : public VThreadedTask
{
public:
  VUpdateNormalmapTask() {}
  virtual ~VUpdateNormalmapTask() {}

  VEditableTerrainSector *pSector;
  int iPageX, iPageY;
  VisBitmap_cl *pNormalmapSysMem;

  virtual void Run(VManagedThread *pThread) HKV_OVERRIDE
  {
    const VTerrainConfig &config(pSector->m_Config);
    const int iSamplesPerPageX = config.m_iHeightSamplesPerSector[0] / config.m_iSectorMeshesPerSector[0];
    const int iSamplesPerPageY = config.m_iHeightSamplesPerSector[1] / config.m_iSectorMeshesPerSector[1];
    hkvVec3 vDirX(config.m_vSampleSpacing.x,0.f,0.f);
    hkvVec3 vDirY(0.f,config.m_vSampleSpacing.y,0.f);

    for (int y=0;y<iSamplesPerPageY;y++)
    {
      int xx = iPageX*iSamplesPerPageX;
      int yy = iPageY*iSamplesPerPageY+y;
      VColorRef *pDst = pNormalmapSysMem->GetDataPtr(xx,yy);
      for (int x=0;x<iSamplesPerPageX;x++,pDst++,xx++)
      {
        // use a faster method here since we never access neighbor sectors
        float fH00 = pSector->GetHeightAt(xx,yy);
        float fH10 = pSector->GetHeightAt(xx+1,yy);
        float fH01 = pSector->GetHeightAt(xx,yy+1);

        vDirX.z = fH10-fH00;
        vDirY.z = fH01-fH00;
        hkvVec3 vNrml = vDirX.cross(vDirY);
        vNrml.setLength(127.f);

        VASSERT(vNrml.z>0.f);
        pDst->r = (int)(127.5f+vNrml.x);
        pDst->g = (int)(127.5f+vNrml.y);
        pDst->b = (int)(127.5f+vNrml.z);
        pDst->a = 255;
      }
    }
  }
};



void VEditableTerrainSector::UpdateNormalmap(bool bNice)
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_UPDATE_NORMALMAP);

  char name[FS_MAX_PATH];
  // Add 2 pixels border on each side (overlapping with neighbor sectors.)
  // In theory a 1 pixel border would be sufficient, but then the textures cannot be compressed anymore (4x4 block restriction)
  int iBorder = m_Config.m_bNormalmapUsesBorder ? 2 : 0;
  int iNrmlMapSizeX = m_Config.m_iHeightSamplesPerSector[0] + iBorder*2;
  int iNrmlMapSizeY = m_Config.m_iHeightSamplesPerSector[1] + iBorder*2;
  if (!m_spNormalmapSysMem)
  {
    sprintf(name,"<Normalmap_%02i_%02i>",m_iIndexX,m_iIndexY);
    m_spNormalmapSysMem = new VisBitmap_cl(name,iNrmlMapSizeX,iNrmlMapSizeY);
    m_spNormalmapSysMem->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  }

  VTextureObjectPtr spNormalMap;

  {
    VisSurface_cl &srf(m_pMeshPage[0].GetSurfaceSafe());
    spNormalMap = srf.m_spNormalMap;
  }

  bool bForceRecreate =
    spNormalMap == NULL ||
    spNormalMap->GetTextureWidth() != iNrmlMapSizeX ||
    spNormalMap->GetTextureHeight() != iNrmlMapSizeY ||
    spNormalMap->GetTextureFormat() != VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP;
  if (bForceRecreate && m_Config.GetSectorNormalmapFilename(name,m_iIndexX,m_iIndexY,true))
  {
    if (spNormalMap!=NULL && spNormalMap!=Vision::TextureManager.GetNeutralNormalMapTexture()) // delete old texture
    {
      spNormalMap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
      spNormalMap = NULL;
    }

    // we use the real filename here so GetDependencies gives us correct references
    spNormalMap = Vision::TextureManager.Create2DTextureObject(name,iNrmlMapSizeX,iNrmlMapSizeY,1,VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP);
    spNormalMap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
    {
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        pPage->GetSurfaceSafe().m_spNormalMap = spNormalMap;
      }
    }
  }

  VTerrainSectorManager *pSM = GetSectorManager();
  hkvVec3 vNrml(hkvNoInitialization);
  const bool bFastUpdate = GetTerrain()->GetFastUpdate() && iBorder==0;

  if (bFastUpdate)
  {
    const int TASK_COUNT = 32;
    VUpdateNormalmapTask tasks[TASK_COUNT];
    int iTaskIndex = 0;

    // update page-wise, only dirty pages

    // 1) kick off update tasks
    VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        if ((pPage->m_iDirtyFlags&MESHPAGEFLAG_NORMALMAP_DIRTY)==0)
          continue;
        VUpdateNormalmapTask &task(tasks[iTaskIndex]);
        Vision::GetThreadManager()->WaitForTask(&task, true);
        // set up task
        task.pSector = this;
        task.iPageX = iPageX; task.iPageY = iPageY;
        task.pNormalmapSysMem = m_spNormalmapSysMem.GetPtr();
        Vision::GetThreadManager()->ScheduleTask(&task);
        iTaskIndex = (iTaskIndex+1)%TASK_COUNT;
      }

    // upload task result
    iTaskIndex = 0;
    pPage = m_pMeshPage;
    for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
      {
        if ((pPage->m_iDirtyFlags&MESHPAGEFLAG_NORMALMAP_DIRTY)==0)
          continue;
        pPage->m_iDirtyFlags &= ~MESHPAGEFLAG_NORMALMAP_DIRTY;
        Vision::GetThreadManager()->WaitForTask(&tasks[iTaskIndex], true);
        iTaskIndex = (iTaskIndex+1)%TASK_COUNT;
        
        // partial upload of texture
        const int iSamplesPerPageX = m_Config.m_iHeightSamplesPerSector[0] / m_Config.m_iSectorMeshesPerSector[0];
        const int iSamplesPerPageY = m_Config.m_iHeightSamplesPerSector[1] / m_Config.m_iSectorMeshesPerSector[1];
        int iox = iPageX*iSamplesPerPageX;
        int ioy = iPageY*iSamplesPerPageY;
        VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_UPLOAD_NORMALMAP);
        spNormalMap->UpdateRect(0, 
          iox,ioy, iSamplesPerPageX, iSamplesPerPageY, // ofs and size
          iNrmlMapSizeX*sizeof(VColorRef),             // stride
          (const char *)m_spNormalmapSysMem->GetDataPtr(iox,ioy), V_TEXTURE_LOCKFLAG_DISCARDABLE);
      }

  } 
  else
  {
    VColorRef *pDst = m_spNormalmapSysMem->GetDataPtr();
    // with a border the normalmap's size is 132x132 for a 128x128 heightfield. The overlapping parts are because of filtering. Use samples in [-2..129]
    const int iOfsX = m_iIndexX * m_Config.m_iHeightSamplesPerSector[0] - iBorder;
    const int iOfsY = m_iIndexY * m_Config.m_iHeightSamplesPerSector[1] - iBorder;
    for (int y=0;y<iNrmlMapSizeY;y++)
      for (int x=0;x<iNrmlMapSizeX;x++,pDst++)
      {
        // we want per face normals, as we have a non-power-of-2 number of vertices
        pSM->GetNormalAtFace(iOfsX+x,iOfsY+y,vNrml,127.f);
        VASSERT(vNrml.z>0.f);
        pDst->r = (int)(127.5f+vNrml.x);
        pDst->g = (int)(127.5f+vNrml.y);
        pDst->b = (int)(127.5f+vNrml.z);
        pDst->a = 255;
      }

    // upload new normalmap
    spNormalMap->UpdateRect(0, 0, 0, 0, 0, -1, (const char *)m_spNormalmapSysMem->GetDataPtr(), V_TEXTURE_LOCKFLAG_DISCARDABLE);
  }
}


IVTerrainDecorationModel *g_pEditedModel = NULL; // set in BeginUpdateDecoration, used in EndUpdateDecoration

void VEditableTerrainSector::BeginUpdateDecoration(bool bDiscardOld, IVTerrainDecorationModel *pIgnoreModel)
{
  AssertValid();
  g_pEditedModel = pIgnoreModel;

  if (m_iUpdateDecorationCounter==0)
  {
    EnsureLoaded();
    AddLock(); // on sector

    VTerrainDecorationInstanceCollection &deco = Decoration();
    // callback on all old model references
    VEditableTerrain* pTerrain = GetTerrain();
    VTerrainDecorationModelManager &models(VTerrainDecorationModelManager::GlobalManager());
    models.SetTempFlag(false);
    VTerrainDecorationInstance *pOldInst = deco.m_pInstances;
    for (int i=0;i<deco.m_iCount;i++,pOldInst++)
      if (pOldInst->m_spModel)
        pOldInst->m_spModel->m_bTempFlag = true;
    models.EditorEventOnTempFlagStatus(true, IVTerrainDecorationModel::SectorDecorationUpdateBegin,pTerrain,this,NULL);


    if (bDiscardOld)
    {
      m_iTempDecorationCount = 0;

      // additionally discard per-sector objects that are related to vegetation
      int iCount = m_iPerSectorObjectCount;
      m_iPerSectorObjectCount = 0;
      for (int i=0;i<iCount;i++)
      {
        VisTypedEngineObject_cl *pObj = m_PerSectorObjects.GetDataPtr()[i];
        if (pObj==NULL || models.DisposePerSectorObject(pObj))
          continue;

        // leave in list
        m_PerSectorObjects.GetDataPtr()[m_iPerSectorObjectCount++] = pObj;
      }
    }
    else
    {
      // copy from sector
      m_iTempDecorationCount = 0;
      m_TempDecorationObj.EnsureSize(deco.m_iCount);
      for (int i=0;i<deco.m_iCount;i++)
        if (deco.m_pInstances[i].m_spModel!=pIgnoreModel)
          m_TempDecorationObj.GetDataPtr()[m_iTempDecorationCount++] = deco.m_pInstances[i];
    }
  }
  m_iUpdateDecorationCounter++;
}

void VEditableTerrainSector::EndUpdateDecoration(bool bFinalUpdate)
{
  m_iUpdateDecorationCounter--;
  if (m_iUpdateDecorationCounter>0)
    return;

  IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
  if (pPM!=NULL && bFinalUpdate)
    pPM->OnDecorationRemoved(m_Decoration);
  SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY); // mark data as dirty

  m_bHasAdditionalDecoration = !bFinalUpdate;
  if (m_bHasAdditionalDecoration) // in this mode, don't write back to sector. Instead GetAdditionalDecoration is called
  {
    ReleaseLock(); // on sector
    AssertValid();
    return;
  }

  VEditableTerrain* pTerrain = GetTerrain();
  VTerrainDecorationModelManager &models(VTerrainDecorationModelManager::GlobalManager());
  models.SetTempFlag(false);


  // count relevant decoration instances
  int iRelevantCount = 0;
  for (int i=0;i<m_iTempDecorationCount;i++)
  {
    VTerrainDecorationInstance &inst(m_TempDecorationObj.GetDataPtr()[i]);
    if (inst.m_spModel && inst.m_spModel->GenerateInstances())
    {
      int iLODCount = inst.m_spModel->GetLODCount(); // there will be one object per LOD
      iRelevantCount+=iLODCount;
    }
  }

  // copy back to sector
  int tx,ty;
  VLargePosition vCenter;
  hkvVec3 vLocalCenter;
  VTerrainDecorationInstanceCollection &deco = Decoration();
  deco.Allocate(this,iRelevantCount);
  hkvAlignedBBox sectorBBox(hkvNoInitialization);
  GetDefaultSectorBoundingBox(sectorBBox); // uses height value's min/max
  m_fMaxTileHandlingDistance = 0.f;

  int iActualCount = 0;
  for (int i=0;i<m_iTempDecorationCount;i++)
  {
    VTerrainDecorationInstance &srcInst(m_TempDecorationObj.GetDataPtr()[i]);
    IVTerrainDecorationModel *pModel = srcInst.m_spModel;
    if (pModel==NULL)
      continue;
    pModel->m_bTempFlag = true; // call the event callback later
    if (!pModel->GenerateInstances())
      continue;

    int iLODCount = pModel->GetLODCount(); // there will be one object per LOD
    while (pModel) // LOD
    {
      VTerrainDecorationInstance &dstInst(deco.m_pInstances[iActualCount++]);
      dstInst = srcInst;
      dstInst.m_spModel = pModel;
      dstInst.GetPosition(vLocalCenter);
      vCenter.FromRenderSpace(m_Config,(hkvVec3& )vLocalCenter);
      GetTileIndices_Clamped(vCenter,tx,ty);
      dstInst.ComputeSortingKey((ty<<8)|tx);

      pModel = pModel->m_pNextLOD;
      iLODCount--;
    }
    VASSERT(iLODCount==0); // loop count must match GetLODCount
  }
  VASSERT(iActualCount==iRelevantCount);

  if (iActualCount>0)
    qsort(deco.m_pInstances,iActualCount,sizeof(VTerrainDecorationInstance),VTerrainDecorationInstance::SortObjects);

  // assign decoration to tiles
  DynArray_cl<int> overAllRefs(iActualCount*2,0);
  V_SAFE_DELETE_ARRAY(m_piReferencedDecoration);

  // pre-compute bounding boxes
  DynArray_cl<hkvAlignedBBox> boundingBoxes(iActualCount);
  for (int i=0;i<iActualCount;i++)
  {
     deco.m_pInstances[i].GetBoundingBox(boundingBoxes[i]);
  }

  // count refs on this tile
  hkvAlignedBBox bbox(hkvNoInitialization);
  m_iReferencedDecorationCount = 0;
  VSectorTile *pTile = m_pTile;
  for (int iTile=0;iTile<m_Config.m_iTilesPerSectorCount;iTile++,pTile++)
  {
    hkvAlignedBBox tileDecoBBox;
    pTile->m_fMaxDecorationFarClip = 0.f;
    pTile->m_iFirstReferencedDecoration = m_iReferencedDecorationCount;
    pTile->m_iReferencedDecorationCount = 0;
    // first, make a valid default bbox
    GetDefaultTileBoundingBox(pTile->m_TileBBox,pTile->m_iIndexX,pTile->m_iIndexY);

    for (int i=0;i<iActualCount;i++)
    {
      bbox = boundingBoxes[i];

      // make it a 2D intersection test
      bbox.m_vMin.z = hkvMath::Min(bbox.m_vMin.z, pTile->m_TileBBox.m_vMin.z);
      bbox.m_vMax.z = hkvMath::Max(bbox.m_vMax.z, pTile->m_TileBBox.m_vMax.z);
      if (!pTile->m_TileBBox.overlaps(bbox))
        continue;

      pTile->m_iReferencedDecorationCount++;
      pTile->m_fMaxDecorationFarClip = hkvMath::Max(pTile->m_fMaxDecorationFarClip,deco.m_pInstances[i].m_spModel->m_Properties.m_fFarClip);
      overAllRefs[m_iReferencedDecorationCount++] = i;
      tileDecoBBox.expandToInclude(bbox);
    }

    // update the tile's bounding box
    if (tileDecoBBox.isValid()) // update z-extent of tile bounding box
    {
      pTile->m_TileBBox.m_vMin.z = tileDecoBBox.m_vMin.z;
      pTile->m_TileBBox.m_vMax.z = tileDecoBBox.m_vMax.z;

      // update the edges:
      if (pTile->m_iIndexX==0) // left edge
      {pTile->m_TileBBox.m_vMin.x = hkvMath::Min(pTile->m_TileBBox.m_vMin.x,tileDecoBBox.m_vMin.x);}
      if (pTile->m_iIndexX==m_Config.m_iTilesPerSector[0]-1) // right edge
      {pTile->m_TileBBox.m_vMax.x = hkvMath::Max(pTile->m_TileBBox.m_vMax.x,tileDecoBBox.m_vMax.x);}
      if (pTile->m_iIndexY==0) // back edge
      {pTile->m_TileBBox.m_vMin.y = hkvMath::Min(pTile->m_TileBBox.m_vMin.y,tileDecoBBox.m_vMin.y);}
      if (pTile->m_iIndexY==m_Config.m_iTilesPerSector[1]-1) // front edge
      {pTile->m_TileBBox.m_vMax.y = hkvMath::Max(pTile->m_TileBBox.m_vMax.y,tileDecoBBox.m_vMax.y);}
    }
    VASSERT(pTile->m_TileBBox.isValid());
    sectorBBox.expandToInclude(pTile->m_TileBBox);
    m_fMaxTileHandlingDistance = hkvMath::Max(m_fMaxTileHandlingDistance, pTile->m_fMaxDecorationFarClip);
  }

  // allocate overall reference list
  if (m_iReferencedDecorationCount>0)
  {
    // copy pointers
    m_piReferencedDecoration = new int[m_iReferencedDecorationCount];
    memcpy(m_piReferencedDecoration,overAllRefs.GetDataPtr(),m_iReferencedDecorationCount*sizeof(m_piReferencedDecoration[0]));
  }

  SetSectorBoundingBox(sectorBBox);
  ReleaseLock(); // on sector

  // release all bitmasks for decoration
  VTerrainSectorDecorationVisibilityMask::DeleteRecursive(m_pFirstDecoVisInfo);
  m_pFirstDecoVisInfo = NULL;

  // mark the edited model as modified. Important for billboards so they receive a callback when they are fully removed
  if (g_pEditedModel!=NULL)
  {
    g_pEditedModel->m_bTempFlag = true;
    g_pEditedModel = NULL;
  }

  // callback on affected models
  models.EditorEventOnTempFlagStatus(true, IVTerrainDecorationModel::SectorDecorationUpdateFinished,pTerrain,this,NULL);

  if (pPM!=NULL && bFinalUpdate)
    pPM->OnDecorationCreated(m_Decoration);

  AssertValid();
}


VTerrainDecorationInstance* VEditableTerrainSector::GetAdditionalDecoration(int &iCount)
{
  iCount = m_iTempDecorationCount;
  return m_TempDecorationObj.GetDataPtr();
}


void VEditableTerrainSector::RemoveDecorationOfType(IVTerrainDecorationModel *pModel)
{
  VASSERT(m_iUpdateDecorationCounter>0 && "This must be called inside a BeginUpdateDecoration/EndUpdateDecoration block");

  int iCount = m_iTempDecorationCount;
  m_iTempDecorationCount = 0;
  VTerrainDecorationInstance *pSrc = m_TempDecorationObj.GetDataPtr();
  VTerrainDecorationInstance *pDest = m_TempDecorationObj.GetDataPtr();
  for (int i=0;i<iCount;i++,pSrc++)
  {
    if (pSrc->m_spModel==pModel)
      continue;
    if (pSrc!=&pDest[m_iTempDecorationCount])
      pDest[m_iTempDecorationCount++] = *pSrc;
  }

}


void VEditableTerrainSector::ReplaceDecorationOfType(IVTerrainDecorationModel *pOldModel, IVTerrainDecorationModel *pNewModel)
{
  if (pNewModel==NULL)
  {
    RemoveDecorationOfType(pOldModel);
    return;
  }
  VASSERT(m_iUpdateDecorationCounter>0 && "This must be called inside a BeginUpdateDecoration/EndUpdateDecoration block");
  VTerrainDecorationInstance *pSrc = m_TempDecorationObj.GetDataPtr();
  for (int i=0;i<m_iTempDecorationCount;i++,pSrc++)
    if (pSrc->m_spModel==pOldModel)
      pSrc->m_spModel = pNewModel;
}


void VEditableTerrainSector::RemoveDecoration()
{
  VASSERT(m_iUpdateDecorationCounter>0 && "This must be called inside a BeginUpdateDecoration/EndUpdateDecoration block");
  m_iTempDecorationCount = 0;
}

void VEditableTerrainSector::AddSingleDecorationObject(const VTerrainDecorationInstance &objDesc)
{
  VASSERT(m_iUpdateDecorationCounter>0 && "This must be called inside a BeginUpdateDecoration/EndUpdateDecoration block");

  m_TempDecorationObj[m_iTempDecorationCount++] = objDesc; // uses = operator
}

#define DECORATION_ON_MESHES

bool TraceTerrainAndMeshes(VTerrainSectorManager *pTerrain, const VLargePosition& vStart, const VLargePosition &vEnd, const VisStaticGeometryInstanceCollection_cl &colliders, VisTraceLineInfo_t &info)
{
  bool bRes = pTerrain->GetAccurateTraceIntersection(vStart,vEnd,1,&info)>0;
#ifdef DECORATION_ON_MESHES
  hkvVec3 v1 = vStart.ToRenderSpace(pTerrain->m_Config);
  hkvVec3 v2 = vEnd.ToRenderSpace(pTerrain->m_Config);

  for (int i=0;i<(int)colliders.GetNumEntries();i++)
  {
    VisStaticGeometryInstance_cl *pInst = colliders.GetEntry(i);
    VASSERT(pInst!=NULL && pInst->GetGeometryType()==STATIC_GEOMETRY_TYPE_MESHINSTANCE);
    bRes |= pInst->TraceTest(v1,v2,1,&info)>0;
  }
#endif
  return bRes;
}

#ifdef DECORATION_ON_MESHES

float GetMeshDropHeight(const hkvVec3& vPos, const VisStaticGeometryInstanceCollection_cl &colliders)
{
  VisTraceLineInfo_t info;
  hkvVec3 vStart(vPos.x,vPos.y,vPos.z+1.f);
  hkvVec3 vEnd(vPos.x,vPos.y,vPos.z-1000.f);
  float fMax = vEnd.z - 1.0f; 
  for (int i=0;i<(int)colliders.GetNumEntries();i++)
  {
    VisStaticGeometryInstance_cl *pInst = colliders.GetEntry(i);
    VASSERT(pInst!=NULL && pInst->GetGeometryType()==STATIC_GEOMETRY_TYPE_MESHINSTANCE);
    if (pInst->TraceTest(vStart,vEnd,1,&info)==0)
      continue;
    fMax = hkvMath::Max(fMax,info.touchPoint.z);
  }
  return fMax;
}

#define DROPVECTOR(v) \
{\
  v = rotMat * v; v += vObjPos;\
  VLargePosition tempLargePos(pTerrain->m_Config,(hkvVec3& )v);\
  float fTerrainTrace = pTerrain->GetHeightAtWorldPos(tempLargePos) + fTerrainHeight;\
  float fMeshTrace = bHasColliderGeom ? GetMeshDropHeight(v, colliders) : fTerrainTrace;\
  float fTraceHeight = hkvMath::Max(fTerrainTrace,fMeshTrace);\
  float fHeightDiff = v.z - fTraceHeight;\
  if (fHeightDiff>fMaxDist)\
  fMaxDist = fHeightDiff;\
}

#else

#define DROPVECTOR(v) \
{\
  v *= rotMat; v += vObjPos;\
  VLargePosition tempLargePos(pTerrain->m_Config,(hkvVec3& )v);\
  float fTerrainTrace = pTerrain->GetHeightAtWorldPos(tempLargePos) + fTerrainHeight;\
  float fHeightDiff = v.z - fTerrainTrace;\
  if (fHeightDiff>fMaxDist)\
  fMaxDist = fHeightDiff;\
}

#endif // DECORATION_ON_MESHES






bool DropObjectToFloor(VTerrainDecorationInstance *pInst, VTerrainSectorManager *pTerrain, float fScale, float fYaw, bool bFinal, const VisStaticGeometryInstanceCollection_cl &colliders, float fMinHeight, float fMaxHeight)
{
  const bool bHasColliderGeom = colliders.GetNumEntries()>0;
  hkvVec3 vObjPos(hkvNoInitialization);
  pInst->GetPosition(vObjPos);
  vObjPos.z = fMaxHeight;
  VisTraceLineInfo_t traceInfo;
  VLargePosition vInitPos(pTerrain->m_Config,(hkvVec3& )vObjPos);
  VLargePosition vEnd(vInitPos);
  vEnd.m_vSectorOfs.z = fMinHeight - pTerrain->m_Config.m_vTerrainPos.z;

  if (!TraceTerrainAndMeshes(pTerrain,vInitPos,vEnd,colliders,traceInfo))
    return false;

  const hkvAlignedBBox &localBox = pInst->m_spModel->m_LocalBBox;
  vObjPos = traceInfo.touchPoint;
  hkvMat3 rotMat(hkvNoInitialization);

  IVTerrainDecorationModel::ModelFlags_e mode = (IVTerrainDecorationModel::ModelFlags_e)((int)pInst->m_spModel->m_Properties.m_eModelFlags & IVTerrainDecorationModel::AlignmentModes);

  if (mode==IVTerrainDecorationModel::AlwaysVertical)
  {
    vObjPos.z -= pInst->m_spModel->m_Properties.m_fRelHeight*fScale;
    rotMat.setFromEulerAngles (0, 0, fYaw*360.0f);
    rotMat *= fScale;
    pInst->SetTransformation(vObjPos,rotMat,fScale); // this maps to different transformation types
    return true;
  }

  
  // make a new rotation matrix
  fYaw *= (hkvMath::pi () * 2.0f);
  hkvVec3 vDir(hkvMath::cosRad (fYaw),hkvMath::sinRad (fYaw),0.f); // rotation in xy plane
  hkvVec3 vNormal = traceInfo.primitivePlane.m_vNormal;
	vNormal = vNormal * fScale;
  hkvVec3 vRight = vDir.cross(vNormal);
  vDir = vNormal.cross(vRight);
  vDir.setLength(fScale);
  vRight.setLength(fScale);

  // in sheared mode, z-coordinate is always straight up
  if (mode==IVTerrainDecorationModel::SmoothSheared)
    vNormal.set(0,0,fScale);

	rotMat.setAxisXYZ(vDir,-vRight,vNormal);
  vObjPos -= vNormal*pInst->m_spModel->m_Properties.m_fRelHeight;

  // accurate update - this would be too slow to perform every frame
  if (bFinal)
  {
    // get the bottom four positions of model bounding box
    hkvVec3 c00(localBox.m_vMin.x, localBox.m_vMin.y, localBox.m_vMin.z);
    hkvVec3 c01(localBox.m_vMin.x, localBox.m_vMax.y, localBox.m_vMin.z);
    hkvVec3 c10(localBox.m_vMax.x, localBox.m_vMin.y, localBox.m_vMin.z);
    hkvVec3 c11(localBox.m_vMax.x, localBox.m_vMax.y, localBox.m_vMin.z);
    float fMaxDist = -1000.f;
    const float fTerrainHeight = pTerrain->m_Config.m_vTerrainPos.z;
    DROPVECTOR(c00);
    DROPVECTOR(c01);
    DROPVECTOR(c10);
    DROPVECTOR(c11);

    // stick the object into the ground so no corner floats in the air
    if (fMaxDist>-1000.f)
      vObjPos.z -= fMaxDist;
  }

  pInst->SetTransformation(vObjPos,rotMat,fScale); // this maps to different transformation types

  return true;
}

// Due to a longstanding bug in the terrain random number generator, it was always
// using 0 to 0.5 instead of 0 to 1. This bug was fixed, but to keep terrain working
// as before, we need to mod the results here.
#define LEGACY_TERRAIN_RAND_MOD 0.5f

void VEditableTerrainSector::CreateFromDensity(VDecorationDensityChannel *pPlane, bool bFinalUpdate)
{
  VASSERT(m_iUpdateDecorationCounter>0 && "This must be called inside a BeginUpdateDecoration/EndUpdateDecoration block");
  //  int iSubDiv = pPlane->m_spSource->m_Properties.m_iDensitySubDiv;
  IVTerrainDecorationModel *pModel = pPlane->m_spSource;
  if (!pModel || !pModel->IsValid())
  {
    //pPlane->Clear();
    return;
  }


  float fDensity = pModel->m_Properties.m_fRelativeDensity / pModel->m_Properties.m_fAverageScale;
  hkvAlignedBBox &bbox = pModel->m_LocalBBox;
  int iSubDivX = (int)(fDensity * m_Config.m_vSectorSize.x / bbox.getSizeX());
  int iSubDivY = (int)(fDensity * m_Config.m_vSectorSize.y / bbox.getSizeY());
  int iSubDiv = hkvMath::Max(iSubDivX,iSubDivY); // objects are rotated anyway
  if (iSubDiv<1) iSubDiv = 1;
  float fInvSubDivX = 1.f/(float)iSubDiv;
  float fInvSubDivY = 1.f/(float)iSubDiv;
  float fDistribution = (1.f + pModel->m_Properties.m_fRandomness*3.f) * 0.5f;

  VEditableTerrain *pTerrain = GetTerrain();

  VisStaticGeometryInstanceCollection_cl &colliderGeom(pTerrain->DetermineRelevantDecorationReceiverMeshes(this));

  VTerrainDecorationInstance tempInst;
  tempInst.m_pOwnerSector = this;
  VRandom rnd( (m_iRandomSeed + pPlane->m_spSource->m_Properties.m_iModelID) & (RANDOM_TABLE_SIZE-1) );
  hkvVec3 vPos;
  int iCreated = 0;
  int iOverall = 0;

  float z = pTerrain->Config().m_vTerrainPos.z;
  float fEps = Vision::World.GetGlobalUnitScaling() * 50.f;
  float fMaxHeight = (m_fMaxHeightValue+fEps) + z;
  float fMinHeight = (m_fMinHeightValue-fEps) + z;

  hkvVec3 vOrigin = GetSectorOrigin();
  VLargePosition largePos(m_iIndexX,m_iIndexY,hkvVec3(0.f,0.f,0.f));
  const int iMaxCount = 200000; // arbitrary limit
  for (float y=0.f;y<1.f;y+=fInvSubDivY)
    for (float x=0.f;x<1.f;x+=fInvSubDivX,iOverall++)
    {
      if (iCreated>=iMaxCount)
        break;
      float fVal = pPlane->GetValueI(x,y);
      if (fVal<=rnd.GetFloat())
      {
        rnd.SkipFloats(4); // keep in-sync
        continue;
      }
      float fScale = pModel->m_Properties.m_fAverageScale * (1.f+(rnd.GetFloat()-1.f)*pModel->m_Properties.m_fScaleVariation);
      if (fScale<=HKVMATH_LARGE_EPSILON)
      {
        rnd.SkipFloats(3); // keep in-sync
        continue;
      }
      largePos.m_iSectorX = m_iIndexX;
      largePos.m_iSectorY = m_iIndexY;
      largePos.m_vSectorOfs.x = (x+rnd.GetFloat()*LEGACY_TERRAIN_RAND_MOD*fInvSubDivX*fDistribution) * m_Config.m_vSectorSize.x;
      largePos.m_vSectorOfs.y = (y+rnd.GetFloat()*LEGACY_TERRAIN_RAND_MOD*fInvSubDivY*fDistribution) * m_Config.m_vSectorSize.y;
      vPos.x = vOrigin.x + largePos.m_vSectorOfs.x;
      vPos.y = vOrigin.y + largePos.m_vSectorOfs.y;
      largePos.Validate(m_Config);
      vPos.z = pTerrain->m_SectorManager.GetHeightAtWorldPos(largePos);

      tempInst.m_vPosition = vPos;
      tempInst.m_spModel = pModel;
      if (!DropObjectToFloor(&tempInst, this->GetSectorManager(), fScale, rnd.GetFloat()*LEGACY_TERRAIN_RAND_MOD,bFinalUpdate,colliderGeom, fMinHeight,fMaxHeight))
        continue;
      AddSingleDecorationObject(tempInst);
      iCreated++;
    }
}


void VEditableTerrainSector::UpdateDecoration(bool bFinalUpdate)
{
  GetTerrain()->m_DirtyVegetationSectors.SafeRemove(this);
  GetTerrain()->TriggerDirtyRangeChanged();

  VASSERT(m_iUpdateDecorationCounter==0 && "This should not be called inside a BeginUpdateDecoration/EndUpdateDecoration block");
  BeginUpdateDecoration(true);
  for (int i=0;i<m_DecorationChannels.Count();i++)
    CreateFromDensity(m_DecorationChannels.GetAt(i),bFinalUpdate);
  EndUpdateDecoration(bFinalUpdate);
}





#if defined (_VISION_PS3) || defined (_VISION_PSP2)
#pragma diag_push
#pragma diag_suppress=178
#endif


inline static void VerifyValidFloats(const float *pBlock, int iCount, float fMax=1000000.f)
{
  for (int i=0; i < iCount; i++)
  {
    VASSERT_MSG (hkvMath::isValidFloat (pBlock[i]), "Value is not a valid float.");
    VASSERT_MSG ((pBlock[i] >= -fMax) && (pBlock[i] <= fMax), "Value is out of range.");
  }
}


#if defined (_VISION_PS3) || defined (_VISION_PSP2)
#pragma diag_pop
#endif



bool VEditableTerrainSector::CopyRelevantHeightValues(VTerrainLockObject &destData)
{
  const int iHMOfsX = m_iIndexX*m_Config.m_iHeightSamplesPerSector[0];
  const int iHMOfsY = m_iIndexY*m_Config.m_iHeightSamplesPerSector[1];

  int x1 = destData.m_iPos[0] - iHMOfsX;
  int y1 = destData.m_iPos[1] - iHMOfsY;
  int x2 = destData.m_iPos[0]+destData.m_iSize[0] - iHMOfsX - 1;
  int y2 = destData.m_iPos[1]+destData.m_iSize[1] - iHMOfsY - 1;

  // completely outside?
  if (x2<0 || y2<0 || x1>=m_Config.m_iHeightSamplesPerSector[0] || y1>=m_Config.m_iHeightSamplesPerSector[1])
    return false;

  // clamp relevant part
  float *pDest = (float *)destData.GetData();
  if (x1<0) {pDest-=x1;x1=0;}
  if (y1<0) {pDest-=y1*destData.m_iStride;y1=0;}
  // the last sector may also use its extra data row:
  int iMaxX = (m_iIndexX==m_Config.m_iSectorCount[0]-1) ?  m_Config.m_iHeightSamplesPerSector[0] : m_Config.m_iHeightSamplesPerSector[0]-1;
  int iMaxY = (m_iIndexY==m_Config.m_iSectorCount[1]-1) ?  m_Config.m_iHeightSamplesPerSector[1] : m_Config.m_iHeightSamplesPerSector[1]-1;
  if (x2>iMaxX) x2=iMaxX;
  if (y2>iMaxY) y2=iMaxY;
  int iRowSize = (x2-x1+1)*sizeof(float);

  float *pSrc = GetHeightmapValuesAt(x1,y1);
  for (int y=y1;y<=y2;y++,pSrc+=m_iSampleStrideX,pDest+=destData.m_iStride)
  {
    memcpy(pDest,pSrc,iRowSize);
#ifdef HK_DEBUG_SLOW
    VerifyValidFloats(pDest,iRowSize/sizeof(float));
#endif
  }
  return true;
}


bool VEditableTerrainSector::GetRelevantHeightValues(const VTerrainLockObject &srcData)
{
  const int iHMOfsX = m_iIndexX*m_Config.m_iHeightSamplesPerSector[0];
  const int iHMOfsY = m_iIndexY*m_Config.m_iHeightSamplesPerSector[1];

  int x1 = srcData.m_iPos[0] - iHMOfsX;
  int y1 = srcData.m_iPos[1] - iHMOfsY;
  int x2 = srcData.m_iPos[0]+srcData.m_iSize[0] - iHMOfsX - 1;
  int y2 = srcData.m_iPos[1]+srcData.m_iSize[1] - iHMOfsY - 1;

  // completely outside?
  const int iCountX = m_iSampleStrideX; // also copy into overlapping part; very important!
  const int iCountY = m_Config.m_iHeightSamplesPerSector[1]+2; // also copy into overlapping part; very important!
  if (x2<0 || y2<0 || x1>=iCountX || y1>=iCountY)
    return false;

  // clamp relevant part
  float *pSrc = (float *)srcData.GetData();
  if (x1<0) {pSrc-=x1;x1=0;}
  if (y1<0) {pSrc-=y1*srcData.m_iStride;y1=0;}
  if (x2>=iCountX) x2=iCountX-1;
  if (y2>=iCountY) y2=iCountY-1;
  const int iRowElements = x2-x1+1;
  const int iRowSize = iRowElements*sizeof(float);
  float *pDest = (float *)GetHeightmapValuesAt(x1,y1);

  // if we cover the full area, rebuild min/max
  if ((x2-x1+1)==iCountX && (y2-y1+1)==iCountY)
    m_fMinHeightValue = m_fMaxHeightValue = pSrc[0];

  for (int y=y1;y<=y2;y++,pSrc+=srcData.m_iStride,pDest+=m_iSampleStrideX)
  {
    memcpy(pDest,pSrc,iRowSize);

    // new maximum?
    for (int i=0;i<iRowElements;i++)
    {
      m_fMaxHeightValue = hkvMath::Max(m_fMaxHeightValue, pSrc[i]);
      m_fMinHeightValue = hkvMath::Min(m_fMinHeightValue, pSrc[i]);
    }

#ifdef HK_DEBUG_SLOW
    VerifyValidFloats(pDest,iRowElements);
#endif
  }

  // mark sector as dirty for swapping
  InvalidateMesh();
  // flag the relevant pages:
  const int iSamplesPerPageX = m_Config.m_iHeightSamplesPerSector[0]/m_Config.m_iSectorMeshesPerSector[0];
  const int iSamplesPerPageY = m_Config.m_iHeightSamplesPerSector[1]/m_Config.m_iSectorMeshesPerSector[1];
  int px1 = x1/iSamplesPerPageX;
  int py1 = y1/iSamplesPerPageY;
  int px2 = hkvMath::Min(x2/iSamplesPerPageX, m_Config.m_iSectorMeshesPerSector[0]-1);
  int py2 = hkvMath::Min(y2/iSamplesPerPageY, m_Config.m_iSectorMeshesPerSector[1]-1);
  for (int py=py1;py<=py2;py++)
    for (int px=px1;px<=px2;px++)
      this->GetMeshPageInfo(px,py)->m_iDirtyFlags |= (MESHPAGEFLAG_HEIGHTMAP_DIRTY|MESHPAGEFLAG_NORMALMAP_DIRTY);

  VASSERT(m_bPrepared==false);

  // update bounding box
  const float fHOfs = m_Config.m_vTerrainPos.z;
  hkvAlignedBBox bbox = GetBoundingBox();
  bbox.m_vMin.z = hkvMath::Min(m_fMinHeightValue+fHOfs,bbox.m_vMin.z);
  bbox.m_vMax.z = hkvMath::Max(m_fMaxHeightValue+fHOfs,bbox.m_vMax.z);
  SetSectorBoundingBox(bbox);

  return true;
}


bool VEditableTerrainSector::CopyRelevantHoleMask(VTerrainLockObject &destData)
{
  const int iTileOfsX = m_iIndexX*m_Config.m_iTilesPerSector[0];
  const int iTileOfsY = m_iIndexY*m_Config.m_iTilesPerSector[1];

  int x1 = destData.m_iPos[0] - iTileOfsX;
  int y1 = destData.m_iPos[1] - iTileOfsY;
  int x2 = destData.m_iPos[0]+destData.m_iSize[0] - iTileOfsX - 1;
  int y2 = destData.m_iPos[1]+destData.m_iSize[1] - iTileOfsY - 1;

  // completely outside?
  if (x2<0 || y2<0 || x1>=m_Config.m_iTilesPerSector[0] || y1>=m_Config.m_iTilesPerSector[1])
    return false;

  // clamp relevant part
  bool *pDest = (bool *)destData.GetData();
  if (x1<0) {pDest-=x1;x1=0;}
  if (y1<0) {pDest-=y1*destData.m_iStride;y1=0;}
  if (x2>=m_Config.m_iTilesPerSector[0]) x2=m_Config.m_iTilesPerSector[0]-1;
  if (y2>=m_Config.m_iTilesPerSector[1]) y2=m_Config.m_iTilesPerSector[1]-1;

  for (int y=y1;y<=y2;y++,pDest+=destData.m_iStride)
    for (int x=x1;x<=x2;x++)
      pDest[x-x1] = GetTile(x,y)->IsHole();

  return true;
}

bool VEditableTerrainSector::GetRelevantHoleMask(const VTerrainLockObject &srcData)
{
  const int iTileOfsX = m_iIndexX*m_Config.m_iTilesPerSector[0];
  const int iTileOfsY = m_iIndexY*m_Config.m_iTilesPerSector[1];

  int x1 = srcData.m_iPos[0] - iTileOfsX;
  int y1 = srcData.m_iPos[1] - iTileOfsY;
  int x2 = srcData.m_iPos[0]+srcData.m_iSize[0] - iTileOfsX - 1;
  int y2 = srcData.m_iPos[1]+srcData.m_iSize[1] - iTileOfsY - 1;

  // completely outside?
  if (x2<0 || y2<0 || x1>=m_Config.m_iTilesPerSector[0] || y1>=m_Config.m_iTilesPerSector[1])
    return false;

  // clamp relevant part
  bool *pSrc = (bool *)srcData.GetData();
  if (x1<0) {pSrc-=x1;x1=0;}
  if (y1<0) {pSrc-=y1*srcData.m_iStride;y1=0;}
  if (x2>=m_Config.m_iTilesPerSector[0]) x2=m_Config.m_iTilesPerSector[0]-1;
  if (y2>=m_Config.m_iTilesPerSector[1]) y2=m_Config.m_iTilesPerSector[1]-1;

  for (int y=y1;y<=y2;y++,pSrc+=srcData.m_iStride)
    for (int x=x1;x<=x2;x++)
    {
      if (pSrc[x-x1])
        GetTile(x,y)->m_iTileFlags |= VSECTORTILEFLAGS_HOLE;
      else
        GetTile(x,y)->m_iTileFlags &= (~VSECTORTILEFLAGS_HOLE);
    }

    // mark sector as dirty for swapping
    InvalidateMesh();
    if (m_spCollisionMeshes)
      m_spCollisionMeshes->InvalidateMeshes(x1,y1,x2,y2);
    m_bHoleTextureDirty = true;
    GetTerrain()->m_PendingPhysicsUpdateSectors.AddUnique(this); // for real physics update


    VASSERT(m_bPrepared==false);
    return true;
}


bool VEditableTerrainSector::CopyRelevantDensityMap(VTerrainLockObject &destData)
{
  EnsureLoaded();
  VDecorationDensityChannel *pChannel = m_DecorationChannels.FindModelChannelByID(destData.m_iReserved[0]);
  if (!pChannel)
    return false;

  const int iOfsX = m_iIndexX*pChannel->m_iSizeX;
  const int iOfsY = m_iIndexY*pChannel->m_iSizeY;

  int x1 = destData.m_iPos[0] - iOfsX;
  int y1 = destData.m_iPos[1] - iOfsY;
  int x2 = destData.m_iPos[0]+destData.m_iSize[0] - iOfsX - 1;
  int y2 = destData.m_iPos[1]+destData.m_iSize[1] - iOfsY - 1;

  // completely outside?
  if (x2<0 || y2<0 || x1>=pChannel->m_iSizeX || y1>=pChannel->m_iSizeY)
    return false;

  // clamp relevant part
  int iDstIndex = 0;
  if (x1<0) {iDstIndex-=x1;x1=0;}
  if (y1<0) {iDstIndex-=y1*destData.m_iStride;y1=0;}
  if (x2>=pChannel->m_iSizeX) x2=pChannel->m_iSizeX-1;
  if (y2>=pChannel->m_iSizeY) y2=pChannel->m_iSizeY-1;
  int iRowSize = (x2-x1+1)*sizeof(UBYTE);

  VASSERT(iDstIndex>=0);
  UBYTE *pStartDest = (UBYTE *)destData.GetData();
  UBYTE *pDest = &pStartDest[iDstIndex];

  UBYTE *pSrc = pChannel->GetValuePtr(x1,y1);
  for (int y=y1;y<=y2;y++,pSrc+=pChannel->m_iSizeX,pDest+=destData.m_iStride)
  {
    VASSERT((pDest-pStartDest+iRowSize)<=destData.GetArea());
    VASSERT((pSrc-pChannel->GetValuePtr(0,0)+iRowSize)<=pChannel->GetSampleCount());
    memcpy(pDest,pSrc,iRowSize);
  }
  return true;
}


bool VEditableTerrainSector::GetRelevantDensityMap(const VTerrainLockObject &srcData, bool bFinalUpdate)
{
  const bool bForceCreateChannel = true;
  const VTerrainConfig &config(GetSectorManager()->Config());

  EnsureLoaded();
  VDecorationDensityChannel *pChannel = m_DecorationChannels.FindModelChannelByID(srcData.m_iReserved[0]);
  if (!pChannel) // it is possible that this channel is not yet there
  {
    if (!bForceCreateChannel)
      return true;

    // add an instance of the template channel to this collection
    IVTerrainDecorationModel *pModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(srcData.m_iReserved[0]);
    if (!pModel || !pModel->IsValid())
      return false;
    pChannel = new VDecorationDensityChannel(pModel,config);
    m_DecorationChannels.Add(pChannel);
  }

  const int iOfsX = m_iIndexX*pChannel->m_iSizeX;
  const int iOfsY = m_iIndexY*pChannel->m_iSizeY;

  int x1 = srcData.m_iPos[0] - iOfsX;
  int y1 = srcData.m_iPos[1] - iOfsY;
  int x2 = srcData.m_iPos[0]+srcData.m_iSize[0] - iOfsX - 1;
  int y2 = srcData.m_iPos[1]+srcData.m_iSize[1] - iOfsY - 1;

  // completely outside?
  if (x2<0 || y2<0 || x1>=pChannel->m_iSizeX || y1>=pChannel->m_iSizeY)
    return false;

  // clamp relevant part
  UBYTE *pSrc = (UBYTE *)srcData.GetData();
  if (x1<0) {pSrc-=x1;x1=0;}
  if (y1<0) {pSrc-=y1*srcData.m_iStride;y1=0;}
  if (x2>=pChannel->m_iSizeX) x2=pChannel->m_iSizeX-1;
  if (y2>=pChannel->m_iSizeY) y2=pChannel->m_iSizeY-1;
  int iRowSize = (x2-x1+1)*sizeof(UBYTE);
  UBYTE *pDest = pChannel->GetValuePtr(x1,y1);

  //copy row-wise
  for (int y=y1;y<=y2;y++,pSrc+=srcData.m_iStride,pDest+=pChannel->m_iSizeX)
    memcpy(pDest,pSrc,iRowSize);


  // update the objects of this type
  // UpdateDecoration();

  BeginUpdateDecoration(false, pChannel->m_spSource);
  //RemoveDecorationOfType(pChannel->m_spSource); // already filtered out in BeginUpdateDecoration
  CreateFromDensity(pChannel,bFinalUpdate);
  EndUpdateDecoration(bFinalUpdate);


  // mark sector as dirty for swapping
  SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
  pChannel->SetDirtyFlag();
  return true;
}




bool VEditableTerrainSector::CopyRelevantDetailTextureValues(VTerrainLockObject &destData)
{
  EnsureLoaded();
  VTextureWeightmapChannel *pChannel = m_UsedWeightmapChannels.FindChannelByResourceID(destData.m_iReserved[0]);
  if (!pChannel)
    return false;

  const int iOfsX = m_iIndexX*pChannel->m_iSizeX;
  const int iOfsY = m_iIndexY*pChannel->m_iSizeY;

  int x1 = destData.m_iPos[0] - iOfsX;
  int y1 = destData.m_iPos[1] - iOfsY;
  int x2 = destData.m_iPos[0]+destData.m_iSize[0] - iOfsX - 1;
  int y2 = destData.m_iPos[1]+destData.m_iSize[1] - iOfsY - 1;

  // completely outside?
  if (x2<0 || y2<0 || x1>=pChannel->m_iSizeX || y1>=pChannel->m_iSizeY)
    return false;

  // clamp relevant part
  int iDstIndex = 0;
  if (x1<0) {iDstIndex-=x1;x1=0;}
  if (y1<0) {iDstIndex-=y1*destData.m_iStride;y1=0;}
  if (x2>=pChannel->m_iSizeX) x2=pChannel->m_iSizeX-1;
  if (y2>=pChannel->m_iSizeY) y2=pChannel->m_iSizeY-1;
  int iRowSize = (x2-x1+1)*sizeof(UBYTE);

  VASSERT(iDstIndex>=0);
  UBYTE *pStartDest = (UBYTE *)destData.GetData();
  UBYTE *pDest = &pStartDest[iDstIndex];

  UBYTE *pSrc = pChannel->GetValuePtr(x1,y1);
  for (int y=y1;y<=y2;y++,pSrc+=pChannel->m_iSizeX,pDest+=destData.m_iStride)
  {
    VASSERT((pDest-pStartDest+iRowSize)<=destData.GetArea());
    VASSERT((pSrc-pChannel->GetValuePtr(0,0)+iRowSize)<=pChannel->GetSampleCount());
    memcpy(pDest,pSrc,iRowSize);
  }
  return true;
}



bool VEditableTerrainSector::GetRelevantDetailTextureValues(const VTerrainLockObject &srcData)
{
  const VTerrainConfig &config(GetSectorManager()->Config());

  EnsureLoaded();
  VTextureWeightmapChannelResource *pRes = srcData.m_pTerrain->m_WeightmapChannels.FindByID(srcData.m_iReserved[0]);
  VTextureWeightmapChannel *pChannel = m_UsedWeightmapChannels.CreateChannel(config,pRes,config.m_iDefaultWeightmapResolution[0],config.m_iDefaultWeightmapResolution[1]);
  if (!pChannel)
    return false;

  const int iOfsX = m_iIndexX*pChannel->m_iSizeX;
  const int iOfsY = m_iIndexY*pChannel->m_iSizeY;

  int x1 = srcData.m_iPos[0] - iOfsX;
  int y1 = srcData.m_iPos[1] - iOfsY;
  int x2 = srcData.m_iPos[0]+srcData.m_iSize[0] - iOfsX - 1;
  int y2 = srcData.m_iPos[1]+srcData.m_iSize[1] - iOfsY - 1;

  // completely outside?
  if (x2<0 || y2<0 || x1>=pChannel->m_iSizeX || y1>=pChannel->m_iSizeY)
    return false;

  // clamp relevant part
  UBYTE *pSrc = (UBYTE *)srcData.GetData();
  if (x1<0) {pSrc-=x1;x1=0;}
  if (y1<0) {pSrc-=y1*srcData.m_iStride;y1=0;}
  if (x2>=pChannel->m_iSizeX) x2=pChannel->m_iSizeX-1;
  if (y2>=pChannel->m_iSizeY) y2=pChannel->m_iSizeY-1;
  int iRowSize = (x2-x1+1)*sizeof(UBYTE);
  UBYTE *pDest = pChannel->GetValuePtr(x1,y1);

  //copy row-wise
  for (int y=y1;y<=y2;y++,pSrc+=srcData.m_iStride,pDest+=pChannel->m_iSizeX)
    memcpy(pDest,pSrc,iRowSize);

  // mark sector as dirty for swapping
  SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);
  pChannel->SetDirtyFlag();
  return true;
}

void VEditableTerrainSector::UpdateMaterialID()
{
  if (!IsLoaded())
    return;

  int weightMapWidth = m_UsedWeightmapChannels.GetAt(0)->m_iSizeX;
  int weightMapHeight = m_UsedWeightmapChannels.GetAt(0)->m_iSizeY;

  int perPageWeightMapWidth = weightMapWidth / m_Config.m_iSectorMeshesPerSector[0];
  int perPageWeightMapHeight = weightMapHeight / m_Config.m_iSectorMeshesPerSector[1];

  VTerrainSectorMeshPageInfo *pPage = m_pMeshPage;
  for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
  {
    for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
    {
      // find out the number of used weight maps
      int iWMCount = pPage->GetWeightMapSamplerCount();

      hkvRect pageRegion(iPageX * perPageWeightMapWidth, iPageY * perPageWeightMapHeight, perPageWeightMapWidth, perPageWeightMapHeight);

      VTextureWeightmapChannel* pChannel[1024];
      int iRelevant = m_UsedWeightmapChannels.GetMostRelevantChannels(pageRegion, pChannel, iWMCount * 4 + 1, 0.0f);
      if (iRelevant > 0)
        pPage->m_MaterialIDs[0] = pChannel[0]->m_spResource->m_Properties.m_iMaterialID;

      // rounded number of weightmaps (note the first one is basetexture and does not count)
      int iTexCount = (iRelevant + 2) / 4;

      // set the mapped samplers on the sector:
      for (int i = 0; i < iTexCount; i++)
      {
        // every weightmap has up to 4 detail textures:
        for (int iChannel = 0; iChannel < 4; iChannel++)
        {
          int iChannelIndex = i * 4 + iChannel;
          if (iChannelIndex < iRelevant - 1)
          {
            // store material ID so it is available on native side
            if ((iChannelIndex + 1) < 9)
              pPage->m_MaterialIDs[iChannelIndex + 1] = pChannel[iChannelIndex + 1]->m_spResource->m_Properties.m_iMaterialID;
          }
        }
      }
    }
  }
}

void VEditableTerrainSector::InvalidateRelevantCollisionMeshes(const VTerrainLockObject *pSrcData)
{

  GetTerrain()->m_PendingPhysicsUpdateSectors.AddUnique(this); // for real physics update

  if (!m_spCollisionMeshes)
    return;

  const VTerrainConfig &config = GetSectorManager()->Config();
  if (pSrcData==NULL)
  {
    m_spCollisionMeshes->InvalidateMeshes(0,0,config.m_iTilesPerSector[0]-1,config.m_iTilesPerSector[1]-1);
    return;
  }

  const int iHMOfsX = m_iIndexX*config.m_iTilesPerSector[0];
  const int iHMOfsY = m_iIndexY*config.m_iTilesPerSector[1];

  int x1 = pSrcData->m_iPos[0]/config.m_iHeightSamplesPerTile[0] - iHMOfsX;
  int y1 = pSrcData->m_iPos[1]/config.m_iHeightSamplesPerTile[1] - iHMOfsY;
  int x2 = (pSrcData->m_iPos[0]+pSrcData->m_iSize[0]-1)/config.m_iHeightSamplesPerTile[0] - iHMOfsX;
  int y2 = (pSrcData->m_iPos[1]+pSrcData->m_iSize[1]-1)/config.m_iHeightSamplesPerTile[1] - iHMOfsY;

  // completely outside?
  if (x2<0 || y2<0 || x1>=config.m_iTilesPerSector[0] || y1>=config.m_iTilesPerSector[1])
    return;

  // clamp relevant part
  if (x1<0) x1=0;
  if (y1<0) y1=0;
  if (x2>=config.m_iTilesPerSector[0]) x2=config.m_iTilesPerSector[0]-1;
  if (y2>=config.m_iTilesPerSector[1]) y2=config.m_iTilesPerSector[1]-1;

  m_spCollisionMeshes->InvalidateMeshes(x1,y1,x2,y2);    
}


void VEditableTerrainSector::OnPositionChanged(const hkvVec3& vOldPos,const hkvVec3& vNewPos)
{
  VTerrainSector::OnPositionChanged(vOldPos,vNewPos);

  if (IsLoaded())
  {
    // Always invalidate mesh when threshold is exceeded, in order to regenerate cached
    // physics shape files on save. 
    if ((vOldPos-vNewPos).getLengthSquared() > 0.0001f)
      InvalidateMesh();

    // By changing the position the physics representation is no longer valid and has to be updated:
    IVisPhysicsModule_cl *pModule = Vision::GetApplication()->GetPhysicsModule();
    if (pModule == NULL)
      return;
      
    pModule->OnTerrainSectorRemoved(this);
    pModule->OnTerrainSectorCreated(this);
  }
}

// set additional register values
VCompiledShaderPass *VEditableTerrainSector::PrepareShader(VTerrainSectorMeshPageInfo* pPage, VCompiledShaderPass *pShader)
{
  if (pShader)
  {
    VShaderConstantBuffer *pPixelConst = pShader->GetConstantBuffer(VSS_PixelShader);

    hkvVec4 vNormalMapRegister;

    // see UpdateNormalmapRegister:
    float fSizeX = (float)m_Config.m_iHeightSamplesPerSector[0];
    float fSizeY = (float)m_Config.m_iHeightSamplesPerSector[1];

    if (m_Config.m_bNormalmapUsesBorder)
    {
      vNormalMapRegister.set(fSizeX/(fSizeX+4.f), fSizeY/(fSizeY+4.f), 2.f/fSizeX,2.f/fSizeY);
    }
    else
    {
      vNormalMapRegister.set(1.f,1.f,0.f,0.f);
    }

    vNormalMapRegister = pPage->AdjustTransformForUnclampedUVs(vNormalMapRegister);

    pPixelConst->SetSingleParameterF("NormalmapTransform", vNormalMapRegister.data);
  }
  return pShader;
}



TERRAIN_IMPEXP void VEditableTerrainSector::RenderTerrainMesh(VTerrainSectorMeshPageInfo* pPage, const VTerrainVisibilityCollectorComponent &visInfoComp)
{
  hkvVec4 vTiling(1.f,1.f,0.f,0.f);
  VEditableTerrain *pTerrain = this->GetTerrain();

  const bool bExportRendering = pTerrain->SectorManager().GetUseExportRendering();

  const VSectorMeshRenderRange_t &lod(bExportRendering ? *visInfoComp.m_pPageRenderRangeNoHoles[pPage->m_iGlobalIndex] : *visInfoComp.m_pPageRenderRange[pPage->m_iGlobalIndex]);

  if ((pTerrain->m_spDetailTexPass==NULL || pTerrain->m_bUseBakedTextures) && !bExportRendering)
  {
    VTerrainSector::RenderTerrainMesh(pPage,visInfoComp); // render default terrain
  }
  else
  {

#if defined( HK_ANARCHY )
    const BOOL bUseDeferred = FALSE;
#else
    const BOOL bUseDeferred = Vision::Renderer.IsRendererNodeOfType(V_RUNTIME_CLASS(VDeferredRenderingSystem));
#endif

    VCompiledShaderPass* pShaderTopMapping = PrepareShader(pPage, pTerrain->m_spDetailTexPass->GetShader(0));
    VCompiledShaderPass* pShader3wayMapping = PrepareShader(pPage, pTerrain->m_spDetailTexPass3way->GetShader(0));

    // directional light, lightmap or no lighting pass
    VCompiledShaderPass* pLightingPass = NULL;
    VCompiledTechnique* pLightingTechnique = NULL;
    if (pTerrain->m_bPreviewDirectionalLight)
      pLightingTechnique = pTerrain->m_spDirLightPass;
    else if (this->GetLightmapTexture(0) != NULL)
      pLightingTechnique = pTerrain->m_spLightmapPass;
    if (pLightingTechnique != NULL && pLightingTechnique->GetShaderCount() > 0)
      pLightingPass = PrepareShader(pPage, pLightingTechnique->GetShader(0));

    VCompiledShaderPass *pFogPass = NULL;
    const VFogParameters &fog = Vision::World.GetFogParameters();

    if ((fog.depthMode != VFogParameters::Off) && !bUseDeferred && pTerrain->m_spFogPass->GetShaderCount() > 0)
      pFogPass = PrepareShader(pPage, pTerrain->m_spFogPass->GetShader(0));
    VCompiledShaderPass *pMinimapPass = NULL;
    if (pTerrain->m_bShowMiniMap && pTerrain->m_spMiniMapPass!=NULL && pTerrain->m_spMiniMapTexture!=NULL)
    {
      pMinimapPass = PrepareShader(pPage, pTerrain->m_spMiniMapPass->GetShader(0));
      VStateGroupTexture *pStateGroupTexture = pMinimapPass->GetStateGroupTexture(VSS_PixelShader, 0);
      if (pStateGroupTexture != NULL)
        pStateGroupTexture->m_spCustomTex = pTerrain->m_spMiniMapTexture;
    }

    VCompiledShaderPass *pClearAlphaPass = NULL;
    if (pTerrain->m_spClearAlphaPass != NULL) //this is null in forward
      pClearAlphaPass = PrepareShader(pPage, pTerrain->m_spClearAlphaPass->GetShader(0));


    VTextureObject *pDetailBaseTex = pPage->GetSurfaceSafe().m_spDiffuseTexture; // base texture
    if (!pDetailBaseTex)
      pDetailBaseTex = Vision::TextureManager.GetPlainWhiteTexture();

    UBYTE iBlending = 0;
    VTextureObject *pBlinkTexture = NULL;
    if (pTerrain->m_fBlinkDuration>0.f)
    {
      pBlinkTexture = (hkvMath::mod (pTerrain->m_fBlinkDuration,0.3f)<0.15f) ? 
        Vision::TextureManager.GetPlainWhiteTexture() : Vision::TextureManager.GetPlainBlackTexture();
    }

    // In (non-baked) editing mode, we render each detail texture separately:
    int iCount = hkvMath::Max(1, m_UsedWeightmapChannels.Count());
    VTextureWeightmapChannel *pChannel = NULL;
    const int SAMPLERINDEX_DETAILTEX = 1; // hardcoded
    const int SAMPLERINDEX_DETAILNORMAL = pShaderTopMapping->GetSamplerIndexByName(VSS_PixelShader, "DetailPassNormalmap");
    const int SAMPLERINDEX_DETAILSPEC = pShaderTopMapping->GetSamplerIndexByName(VSS_PixelShader, "DetailPassSpecmap");
    for (int i=0;i<iCount;i++)
    {
      bool bUseThreeWayMapping = false;
      const VTextureWeightmapChannelResource::DetailTextureProperties_t* prop = NULL;
      if (i<m_UsedWeightmapChannels.Count())
      {
        pChannel = m_UsedWeightmapChannels.GetAt(i);
        pChannel->m_spResource->m_Properties.GetFinalTileAndOfs(m_Config, vTiling,m_iIndexX,m_iIndexY);
        prop = &pChannel->m_spResource->m_Properties;
        bUseThreeWayMapping = prop->m_bUseThreeWayMapping;
      }

      VCompiledShaderPass* pCurrentShader = bUseThreeWayMapping ? pShader3wayMapping : pShaderTopMapping;

      VTextureObject *pDetailNormal = NULL;
      VTextureObject *pDetailSpecmap = NULL;
      if (i==0) // detail base
      {
        VTextureObject *pDiffTex = NULL;
        if (pChannel != NULL)
        {
          VASSERT(prop != NULL);
          pDiffTex = prop->GetDiffuseTexture(m_iIndexX,m_iIndexY);
          pDetailNormal = prop->GetNormalmapTexture(m_iIndexX,m_iIndexY);
          pDetailSpecmap = prop->GetSpecularTexture(m_iIndexX,m_iIndexY);
        }
        if (pChannel && pDiffTex)
          pDetailBaseTex = pDiffTex;

        
        VStateGroupTexture *pStateGroupTexture = pCurrentShader->GetStateGroupTexture(VSS_PixelShader, 0);
        if (pStateGroupTexture != NULL)
          pStateGroupTexture->m_spCustomTex = Vision::TextureManager.GetPlainWhiteTexture();
       
        pStateGroupTexture = pCurrentShader->GetStateGroupTexture(VSS_PixelShader, SAMPLERINDEX_DETAILTEX);
        if (pStateGroupTexture != NULL)
          pStateGroupTexture->m_spCustomTex = (pBlinkTexture!=NULL && pChannel!=NULL && pChannel->m_spResource==pTerrain->m_spBlinkChannel) ? pBlinkTexture : pDetailBaseTex;

        iBlending = 0; // disable blending
      } 
      else
      {
        VASSERT(pChannel != NULL);
        VASSERT(prop != NULL);
        // additional blending passes
        if (pChannel->IsBlack() || !pChannel->IsValid() || pChannel->m_spResource->IsHidden())
          continue;


        
        VStateGroupTexture *pStateGroupTexture = pCurrentShader->GetStateGroupTexture(VSS_PixelShader, 0);
        if (pStateGroupTexture != NULL)
          pStateGroupTexture->m_spCustomTex = pChannel->GetLuminanceTexture(m_iIndexX, m_iIndexY, prop->m_iTextureID); // updates it when dirty

        pStateGroupTexture = pCurrentShader->GetStateGroupTexture(VSS_PixelShader, SAMPLERINDEX_DETAILTEX);
        if (pStateGroupTexture != NULL)
          pStateGroupTexture->m_spCustomTex = (pBlinkTexture!=NULL && pChannel->m_spResource==pTerrain->m_spBlinkChannel) ? pBlinkTexture : prop->GetDiffuseTexture(m_iIndexX,m_iIndexY);
        
        pDetailNormal = prop->GetNormalmapTexture(m_iIndexX,m_iIndexY);
        pDetailSpecmap = prop->GetSpecularTexture(m_iIndexX,m_iIndexY);
        iBlending = 255; // enable blending
      }

      if (pCurrentShader->GetRenderState ()->GetBlendState ().m_cBlendEnable != iBlending)
      {
        pCurrentShader->GetRenderState ()->GetBlendState ().m_cBlendEnable = iBlending;
        pCurrentShader->GetRenderState ()->GetBlendState ().ComputeHash ();
        pCurrentShader->m_bModified  = true;
      }

      // hardcoded register to track tiling factor for single detail passes
#if defined(_VR_DX11)
      VShaderConstantBuffer *pVertexConst = pCurrentShader->GetConstantBuffer(VSS_VertexShader);
      pVertexConst->SetSingleParameterF("SinglePassDetailPassTiling", vTiling.data);
#else
      VisRenderStates_cl::SetVertexShaderConstant(42, vTiling.data, 1);
#endif

      if (bUseDeferred)
      {
        const float fLightMultiplier = pLightingPass != NULL ? 1.0f : 0.0f;
        VShaderConstantBuffer* pPixelConst = pCurrentShader->GetConstantBuffer(VSS_PixelShader);
        pPixelConst->SetSingleParameterF("InitialPassLightMultiplier", fLightMultiplier, fLightMultiplier, fLightMultiplier, fLightMultiplier);
      }

      if (SAMPLERINDEX_DETAILNORMAL>=0)
      {
        if (pDetailNormal==NULL)
          pDetailNormal = Vision::TextureManager.GetNeutralNormalMapTexture();
        VStateGroupTexture *pStateGroupTexture = pCurrentShader->GetStateGroupTexture(VSS_PixelShader, SAMPLERINDEX_DETAILNORMAL);
        if (pStateGroupTexture != NULL)
          pStateGroupTexture->m_spCustomTex = pDetailNormal;
      }
      if (SAMPLERINDEX_DETAILSPEC>=0)
      {
        if (pDetailSpecmap==NULL)
          pDetailSpecmap = Vision::TextureManager.GetPlainBlackTexture();
        VStateGroupTexture *pStateGroupTexture = pCurrentShader->GetStateGroupTexture(VSS_PixelShader, SAMPLERINDEX_DETAILSPEC);
        if (pStateGroupTexture != NULL)
          pStateGroupTexture->m_spCustomTex = pDetailSpecmap;
      }

      unsigned int iForceClamp = (pChannel!=NULL && pChannel->m_spResource->m_Properties.m_bUseClamping) ? (V_BIT(SAMPLERINDEX_DETAILTEX)) : 0;
      pPage->BindSectorShaderTextures(pCurrentShader, iForceClamp);
      Vision::RenderLoopHelper.RenderMeshes(pCurrentShader, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lod.m_iFirstPrim,lod.m_iPrimCount,m_spMesh->GetVertexCount());
    }

    // render again using lighting - either directional or lightmap
    if (pLightingPass)
    {
      pPage->BindSectorShaderTextures(pLightingPass,0);
      Vision::RenderLoopHelper.RenderMeshes(pLightingPass, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lod.m_iFirstPrim,lod.m_iPrimCount,m_spMesh->GetVertexCount());
    }

    // mini map pass
    if (pMinimapPass)
    {
      pPage->BindSectorShaderTextures(pMinimapPass,0);
      Vision::RenderLoopHelper.RenderMeshes(pMinimapPass, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lod.m_iFirstPrim,lod.m_iPrimCount,m_spMesh->GetVertexCount());
    }

    // render fog pass on top
    if (pFogPass)
      Vision::RenderLoopHelper.RenderMeshes(pFogPass, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lod.m_iFirstPrim,lod.m_iPrimCount,m_spMesh->GetVertexCount());

    //finally clear blending value (set it to 1 which is the default value) - happens in deferred only
    if (pClearAlphaPass)
    {
      Vision::RenderLoopHelper.RenderMeshes(pClearAlphaPass, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lod.m_iFirstPrim,lod.m_iPrimCount,m_spMesh->GetVertexCount());
    }


  }

  // Early out when export rendering is enabled to skip all overlay rendering
  if (bExportRendering)
    return;

  // the remaining render calls are all without holes:
  VSectorMeshRenderRange_t &lodClosed(*visInfoComp.m_pPageRenderRangeNoHoles[pPage->m_iGlobalIndex]);
  VCompiledShaderPass* pShader = NULL;

  Vision::RenderLoopHelper.AddMeshStreamsEx(GetSectorManager()->m_spTerrainIndexBuffer, VERTEX_STREAM_INDEXBUFFER);

  if (pTerrain->m_bPreviewHoleOverlay && pTerrain->m_spHoleOverlayPass)
  {
    pShader = PrepareShader(pPage, pTerrain->m_spHoleOverlayPass->GetShader(0));
    VStateGroupTexture *pStateGroupTexture = pShader->GetStateGroupTexture(VSS_PixelShader, 0);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = GetHoleOverlayTexture();
    hkvVec4 vTiles((float)m_Config.m_iTilesPerSector[0],(float)m_Config.m_iTilesPerSector[1],0.f,0.f);
#if defined(_VR_DX11)
    VShaderConstantBuffer *pPixelConst = pShader->GetConstantBuffer(VSS_PixelShader);
    pPixelConst->SetSingleParameterF("TilesPerSector", vTiles.data);
#else
    VisRenderStates_cl::SetPixelShaderConstant(31, vTiles.data, 1);
#endif
    pPage->BindSectorShaderTextures(pShader,0);
    Vision::RenderLoopHelper.RenderMeshes(pShader, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lodClosed.m_iFirstPrim,lodClosed.m_iPrimCount,m_spMesh->GetVertexCount());
  }


  IVTerrainDecorationModel *pPreviewModel = GetTerrain()->m_spPreviewModelWeightmap;
  if (pTerrain->m_spWeightmapPass && pPreviewModel != NULL)
  {
    VDecorationDensityChannel *pDecoChannel = m_DecorationChannels.FindModelChannel(pPreviewModel);
    if (pDecoChannel)
    {
      pShader = PrepareShader(pPage, pTerrain->m_spWeightmapPass->GetShader(0));

      VStateGroupTexture *pStateGroupTexture = pShader->GetStateGroupTexture(VSS_PixelShader, 0);
      if (pStateGroupTexture != NULL)
        pStateGroupTexture->m_spCustomTex = pDecoChannel->GetLuminanceTexture(m_iIndexX,m_iIndexY,pPreviewModel->m_Properties.m_iModelID);

      pPage->BindSectorShaderTextures(pShader,0);

      Vision::RenderLoopHelper.RenderMeshes(pShader, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lodClosed.m_iFirstPrim,lodClosed.m_iPrimCount,m_spMesh->GetVertexCount());
    }
  }

  if (pTerrain->m_spLockedOverlayPass && IsEditorFlagSet(SECTOR_EDITORFLAG_NETWORKLOCK))
  {
    pShader = PrepareShader(pPage, pTerrain->m_spLockedOverlayPass->GetShader(0));

    VStateGroupTexture *pStateGroupTexture = pShader->GetStateGroupTexture(VSS_PixelShader, 0);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = Vision::TextureManager.Load2DTexture("TerrainPlugin\\Icons\\lock128_delete.dds");
    
    pPage->BindSectorShaderTextures(pShader,0);
    Vision::RenderLoopHelper.RenderMeshes(pShader, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lodClosed.m_iFirstPrim,lodClosed.m_iPrimCount,m_spMesh->GetVertexCount());
  }

  if (pTerrain->m_spHighlightMaterialIDPass!=NULL && pTerrain->m_iHighlightMaterialID>=0 && m_spMaterialIDMap!=NULL)
  {
    VTextureObject *pTex = m_spMaterialIDMap->GetLuminanceTexture(m_iIndexX,m_iIndexY,-1);
    pShader = PrepareShader(pPage, pTerrain->m_spHighlightMaterialIDPass->GetShader(0));
    VStateGroupTexture *pStateGroupTexture = pShader->GetStateGroupTexture(VSS_PixelShader, 0);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = pTex; 
    VShaderConstantBuffer *pPixelConst = pShader->GetConstantBuffer(VSS_PixelShader);
    pPixelConst->SetSingleParameterF("HightlightID", (float)pTerrain->m_iHighlightMaterialID * (1.f/255.f),0.f,0.f,0.f);

    pPage->BindSectorShaderTextures(pShader,0);
    Vision::RenderLoopHelper.RenderMeshes(pShader, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, lodClosed.m_iFirstPrim,lodClosed.m_iPrimCount,m_spMesh->GetVertexCount());
  }

}



VTextureObject *VEditableTerrainSector::GetHoleOverlayTexture()
{
  int sx = m_Config.m_iTilesPerSector[0];
  int sy = m_Config.m_iTilesPerSector[1];
  if (!m_spHoleOverlay)
  {
    m_spHoleOverlay = Vision::TextureManager.CreateEmpty2DTextureObject("<HoleOverlay>", sx, sy);
    m_spHoleOverlay->CreateDeviceHandle(VTextureLoader::L8);

    m_bHoleTextureDirty = true;
  }

  if (m_bHoleTextureDirty)
  {
    m_bHoleTextureDirty = false;
    VMemoryTempBuffer<128*128> texData(sx*sy);
    UBYTE *pData = (UBYTE *)texData.GetBuffer();
    for (int y=0;y<sy;y++)
      for (int x=0;x<sx;x++,pData++)
        *pData = GetTile(x,y)->IsHole() ? 0 : 255;
    //Vision::TextureManager.UploadTexture2D(m_spHoleOverlay,texData.GetBuffer(),VTextureLoader::L8);
    m_spHoleOverlay->UpdateRect(0, 0, 0, 0, 0, -1, texData.GetBuffer(), V_TEXTURE_LOCKFLAG_DISCARDABLE);
  }

  return m_spHoleOverlay;
}

bool VEditableTerrainSector::CopyRelevantHoleData(VTerrainLockObject &destData)
{
  return true;
}

bool VEditableTerrainSector::GetRelevantHoleData(const VTerrainLockObject &srcData)
{
  m_bHoleTextureDirty = true;
  return true;
}


//Checks out (updates) and optionally marks the terrain sector files for editing
void VEditableTerrainSector::UpdateRCSFiles(int eAction)
{
  VVERIFY_OR_RET( !m_Config.m_sAbsTerrainDir.IsEmpty() );
  IVRevisionControlSystem *pRCS = VFileAccessManager::GetActiveRCS();
  if (!pRCS)
    return;

  //  char szPath[FS_MAX_PATH];
  char szFilename[FS_MAX_PATH];
  char szTemp[FS_MAX_PATH];

  int types[] = {
    VTC_FILE_SECTORS_MESH,
    VTC_FILE_SECTORS_XML,
    VTC_FILE_SECTORS_HMAP,
    VTC_FILE_SECTORS_VRES,
    // VTC_FILE_SECTORS_LOWRES  //???

    VTC_FILE_EDITING_DATA,

    VTC_FILE_TEXTURES_NORMAL,
    VTC_FILE_TEXTURES_DIFFUSE,
    VTC_FILE_TEXTURES_WEIGHT,
  };

  for (int i=0; i<sizeof(types)/sizeof(types[0]); i++)
  {
    if (m_Config.GetSectorFile(szTemp, m_iIndexX, m_iIndexY, types[i]))
    {
      sprintf(szFilename, szTemp, "*"); // weightmap contains %s which needs to be replaced with *
      VFileHelper::CombineDirAndFile(szTemp, m_Config.m_sAbsProjectDir, szFilename); //make absolute path
      pRCS->PerformAction(szTemp, eAction, true /* Binary file */);  
    }
  }

  /*                  
  ////////////////////////////////////////////////////////////////
  // AuxiliaryTextures folder : Normalmaps, weightmaps, diffuse maps,...
  ////////////////////////////////////////////////////////////////

  if (m_Config.GetAuxiliaryTextureDir(szPath,false))
  {
  //Weightmap  
  VFileHelper::CombineDirAndFile(szFilename,szPath,"Weightmap*_"); //all weightmaps
  sprintf(szPath,"%02i_%02i.dds",m_iIndexX,m_iIndexY);  
  strcat(szFilename,szPath);

  pRCS->PerformAction(szFilename,eAction);

  //Normalmap
  if (m_spNormalmapSysMem && m_Config.GetSectorNormalmapFilename(szFilename,m_iIndexX,m_iIndexY,false))
  {
  pRCS->PerformAction(szFilename,eAction);
  }

  //TODO: Diffuse maps

  //TODO: Anything else?

  }

  /////////////////////////////////
  // Editing folder : Editor data
  //
  /////////////////////////////////
  char szDir[FS_MAX_PATH];
  if (m_Config.GetEditingDir(szDir,false))
  {
  // Editing/SectorInfo_0x_0y.dat
  sprintf(szTemp,"SectorInfo_%02i_%02i.dat",m_iIndexX,m_iIndexY);
  VFileHelper::CombineDirAndFile(szFilename,szDir,szTemp);
  pRCS->PerformAction(szFilename,eAction);
  }

  ////////////////////////////////
  //Sectors folder:  Engine data
  ////////////////////////////////

  const char * ext[] = {"hmap","vmesh","vres","xml"};
  for (int i=0; i<sizeof(ext)/sizeof(ext[0]); i++)
  { 
  //get the non-cached file name
  m_Config.GetSectorFilename(szFilename,m_iIndexX,m_iIndexY, ext[i] ,false);
  pRCS->PerformAction(szFilename,eAction);
  }
  */  
}

bool VEditableTerrainSector::OwnsFileLock()
{
  // Only check for locks when editing in vForge.
  if (Vision::Editor.IsInEditor())
  {
    // We only own the lock if we have all the locks. It should be impossible
    // to lock only a part of them, but we check them all anyway.
  return (m_SectorInfoLock.OwnsLock() && m_SectorHMapLock.OwnsLock() && m_SectorMeshLock.OwnsLock());
}
  else
{
    // At runtime we don't want to make unneeded file access calls.
    return true;
  }
}

//*Gets the editing lock for a sector
//*A sector now has multiple locks for the most important files
//*Only succeeds if we can get the *all* the file locks we need for one sector
//*We need multiple locks because having only one lock for multiple files files doesn't
// allow us to safely "unlock unchanged" with RCS. 
// Keeping everything locked until a commit when the sector wasn't modified wasn't practical.
//*We might not get all locks and fail if someone has uncommitted changes in this sector,
// but that's OK because it's the case we're trying to prevent.
//*Locking hmap, sectorinfo and mesh should cover all direct changes the user can make.
// The other files (e.g. normalmaps, weightmaps) are generated from this info.
bool VEditableTerrainSector::GetFileLock()
{
  // Only use locks when in editing in vForge, we do not want to do unneeded file accesses during runtime.
  if (Vision::Editor.IsInEditor())
  {
    EnsureEditingDataLoaded();

    // We need to lock all three files as only some of them may have changed
    // and thus have been committed. Otherwise it would be possible to sneak in
    // changes that are not covered by only a single lock.
  char szFilename[FS_MAX_PATH];

  //Get lock for SectorInfo file
  m_Config.GetSectorFile(szFilename,m_iIndexX,m_iIndexY, VTC_FILE_EDITING_DATA);
    if (!m_SectorInfoLock.LockFile(szFilename, m_SectorInfoFileTime, Vision::File.GetManager()))
    return false;

  //Get lock for hmap file
  m_Config.GetSectorFile(szFilename,m_iIndexX,m_iIndexY, VTC_FILE_SECTORS_HMAP);
    if (!m_SectorHMapLock.LockFile(szFilename, m_SectorHMapFileTime, Vision::File.GetManager()))
  {
    m_SectorInfoLock.UnlockFile();
    return false;
  }

  //Get lock for mesh file (tiles,holes and decorations)
  m_Config.GetSectorFile(szFilename,m_iIndexX,m_iIndexY, VTC_FILE_SECTORS_MESH);
    if (!m_SectorMeshLock.LockFile(szFilename, m_SectorMeshFileTime, Vision::File.GetManager()))
  {
    m_SectorInfoLock.UnlockFile();
    m_SectorHMapLock.UnlockFile();
    return false;
  }
  }

  return true;
}

void VEditableTerrainSector::ReleaseFileLock()
{
  // Only check for locks when editing in vForge.
  if (Vision::Editor.IsInEditor())
  {
    m_SectorInfoLock.UnlockFile();
    m_SectorHMapLock.UnlockFile();
    m_SectorMeshLock.UnlockFile();

    // Reset relevant flags.
    RemoveEditorFlag(SECTOR_EDITORFLAG_NETWORKLOCK);
    RemoveEditorFlag(SECTOR_EDITORFLAG_OWNSLOCK);
  }
}

void VEditableTerrainSector::UpdateFileLockTimeStamp(int iFileType)
{
  // Update timestamps of lock targets
  if (Vision::Editor.IsInEditor())
  {
    const int VTCKeys[3] = {VTC_FILE_EDITING_DATA, VTC_FILE_SECTORS_HMAP, VTC_FILE_SECTORS_MESH};
    VFileTime* Locks[3] = {&m_SectorInfoFileTime, &m_SectorHMapFileTime, &m_SectorMeshFileTime};

    for (int i = 0; i < V_ARRAY_SIZE(VTCKeys); ++i)
    {
      if (iFileType == VTCKeys[i])
      {
        char szFilename[FS_MAX_PATH];
        char szAbsoluteFilename[FS_MAX_PATH];

        m_Config.GetSectorFile(szFilename, m_iIndexX, m_iIndexY, VTCKeys[i]);
        if (VFileHelper::GetAbsolutePath(szFilename, szAbsoluteFilename, Vision::File.GetManager()))
          VFileHelper::GetModifyTime(szAbsoluteFilename, *Locks[i]);
      }
    }
  }
}

bool VEditableTerrainSector::SaveSector(bool bUseTemp)
{
  // As the terrain panel currently ignore locks, we cannot do this.
  /*
  // In case we are saving to the final directory
  // we have to make sure we have the lock for this sector.
  if (!bUseTemp && !OwnsFileLock())
  {
    return false;
  }
  */

  // No need to save if nothing changed.
  if (!IsAnyEditorFlagSet(SECTOR_EDITORFLAG_ANYTHINGDIRTY &~ SECTOR_EDITORFLAG_MESHDIRTY))
  {
    return true;
  }

  //VASSERT(IsLoaded());  Now supports being called for "unloaded" (but initialized) sectors so we can create initial files
  m_Config.m_bUseTempFolder = bUseTemp; // temp directory or final directory?

  // saving the sector information depends on the weightmap texture, so save
  // out these first
  bool bResult;
  bResult = SaveSectorEditingInformation();
  VASSERT_MSG(bResult, "failed to save sector editing data");

  if (IsEditorFlagSet(SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY))
  {
    // Bake the weightmaps:
    bResult = SaveWeightmapTextures();
    VASSERT_MSG(bResult, "failed to save editor sector data");
    if (bResult)
    {
      RemoveEditorFlag(SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);
    }
  }

  bool bHeightmapSaved = false;
  if (IsEditorFlagSet(SECTOR_EDITORFLAG_HEIGHTMAPDIRTY))
  {
    bResult = SaveHeightmap();
    VASSERT_MSG(bResult, "failed to save heightmap data");

    if (m_Config.m_iReplacementLOD>=0)
    {
      bool bResReplacement = SaveReplacementMesh();
      VASSERT_MSG(bResReplacement, "failed to save replacement file");
    }

    if (bResult)
    {
    RemoveEditorFlag(SECTOR_EDITORFLAG_HEIGHTMAPDIRTY);
      UpdateFileLockTimeStamp(VTC_FILE_SECTORS_HMAP);
    bHeightmapSaved = true;
  }
  }

  // save the sector file also when we needed to save the heightmap, otherwise the dirty bounding box remains in the file
  if (IsEditorFlagSet(SECTOR_EDITORFLAG_SECTORFILEDIRTY) || bHeightmapSaved)
  {
    bResult = SaveSectorInformation();
    VASSERT_MSG(bResult, "failed to save sector information");
    if (bResult)
    {
      RemoveEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
      UpdateFileLockTimeStamp(VTC_FILE_SECTORS_MESH);
    }
  }

  char szFilename[FS_MAX_PATH];
  // if there is a system memory version of the normalmap, we assume it to be dirty
  if (m_spNormalmapSysMem && m_Config.GetSectorNormalmapFilename(szFilename,m_iIndexX,m_iIndexY,true))
  {
    if (bHeightmapSaved || !Vision::File.Exists(szFilename))
    {
      if (!bUseTemp) m_Config.RCSPerformAction(szFilename, RCS_EDIT); //make sure we can write to it
			// here we save, so bake but with a file name provided
      VTextureObject * pTexture = NULL; // needed to pass NULL as sixth argument
			if (VTerrainManager::GlobalManager().GetCustomTextureBaker()==NULL || !VTerrainManager::GlobalManager().GetCustomTextureBaker()->SaveNormalmap(m_spNormalmapSysMem, szFilename, 4, pTexture)) // force the creation of a new texture object by passing NULL
			{
				m_spNormalmapSysMem->SaveToFile(szFilename);
				// failed. Classic saving.
			}
      if (!bUseTemp) m_Config.RCSPerformAction(szFilename, RCS_ADD);
    }
  }

  // Only release the file lock if we have saved to the final directory.
  if (!bUseTemp)
  {
    ReleaseFileLock(); 
  }

  return true;
}


#ifdef WIN32


class VTerrainSectorPreview : public IVResourcePreview
{
public:
  inline VTerrainSectorPreview(VManagedResource *pOwner) : IVResourcePreview(pOwner)
  {
  }

  virtual ~VTerrainSectorPreview()
  {
  }

  virtual bool SupportsThumbnails() HKV_OVERRIDE
  {
    return FALSE;
  }


  virtual bool OnUpdate(float fTimeDiff) HKV_OVERRIDE
  {
    // show the bounding box in the view:
    VEditableTerrainSector *pSector = (VEditableTerrainSector *)GetOwner();
    Vision::Game.DrawBoundingBox(pSector->GetBoundingBox(),VColorRef(100,255,100,255));
    return FALSE;
  }

  void RenderTexturedRect(IVRender2DInterface *pRI, float x, float y, float fSize, VTextureObject *pTex)
  {
    VSimpleRenderState_t iState(VIS_TRANSP_NONE, RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_ALWAYSVISIBLE);
    pRI->DrawTexturedQuad(hkvVec2(x,y),hkvVec2(x+fSize,y+fSize),pTex,hkvVec2(0.f,0.f),hkvVec2(1.f,1.f),V_RGBA_WHITE,iState);
  }

  int CountDecorationOfType(IVTerrainDecorationModel *pModel, const VTerrainDecorationInstanceCollection &deco) const
  {
    int iCount = 0;
    for (int i=0;i<deco.m_iCount;i++)
      if (deco.m_pInstances[i].m_spModel==pModel)
        iCount++;
    return iCount;
  }

  virtual void OnRender(int iFlags) HKV_OVERRIDE
  {
    VEditableTerrainSector *pSector = (VEditableTerrainSector *)GetOwner();
    pSector->EnsureLoaded();

    VisFont_cl *pFont = &Vision::Fonts.DebugFont();
    VSimpleRenderState_t iFontState = VisFont_cl::DEFAULT_STATE;
    iFontState.SetFlag(RENDERSTATEFLAG_USETABULATORCHAR);

    char szBuffer[1024];
    IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
    float fHeight = pFont->GetFontHeight();
    float y = 48.f;

    const VColorRef iOverlayBGColor(20,20,20,220);
    const VSimpleRenderState_t iAlphaState(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_ALWAYSVISIBLE);
    pRI->DrawSolidQuad(hkvVec2(10.f,y-4.f),hkvVec2(600.f,800.f), iOverlayBGColor, iAlphaState);

    // used textures
    sprintf(szBuffer,"Baking up-to-date: %s",pSector->IsEditorFlagSet(SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY) ? "No" : "Yes" );
    pFont->PrintText(pRI, hkvVec2(20.f,y), szBuffer, V_RGBA_WHITE);y+=fHeight+2.f;
    pFont->PrintText(pRI, hkvVec2(20.f,y), "Used Textures:", V_RGBA_WHITE);y+=fHeight+4.f;
    pFont->PrintText(pRI, hkvVec2(40.f,y), "Mask@92:Tex@155:Baked@205:3Way@250:Filename", V_RGBA_YELLOW,iFontState);y+=fHeight+6.f;

    const VTextureWeightmapChannelCollection& texColl(pSector->m_UsedWeightmapChannels);
    for (int i=0;i<texColl.Count();i++)
    {
      VTextureWeightmapChannel *pChannel = texColl.GetAt(i);
      if (!pChannel->m_spResource || !pChannel->m_spResource->IsValid() || pChannel->m_spResource->IsHidden())
        continue;
      VTextureObject *pTex = pChannel->m_spResource->m_Properties.m_spDiffuseTexture;
      if (!pTex)
        continue;

      bool textureUsed = false;
      bool bUses3WayMapping = false;

      VTerrainSectorMeshPageInfo *pPage = pSector->m_pMeshPage;
      for (int iPageY=0;iPageY<pSector->m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      {
        for (int iPageX=0;iPageX<pSector->m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
        {
          int iSamplerIdx = pPage->GetSamplerIndexByTexture(pTex);

          if(iSamplerIdx >= 0)
          {
            textureUsed = true;
            if(pPage->m_iThreeWayMappingLayerMask & V_BIT(i))
            {
              bUses3WayMapping = true;
            }
          }
        }
      }

      RenderTexturedRect(pRI,40.f,y,48.f,pChannel->GetLuminanceTexture(pSector->m_iIndexX,pSector->m_iIndexY,pChannel->m_spResource->m_Properties.m_iTextureID));
      RenderTexturedRect(pRI,92.f,y,48.f,pTex);
      sprintf(szBuffer,"%s@209:%s@254:%s", textureUsed ? "Yes" : "No", bUses3WayMapping ? "Yes" : "No",
        pTex->GetFilename());
      pFont->PrintText(pRI, hkvVec2(160.f,y), szBuffer, VColorRef(220,220,220,255),iFontState);
      y += 52;
    }

    y += 8.f;

    // vegetation:
    pFont->PrintText(pRI, hkvVec2(20.f,y), "Vegetation:", V_RGBA_WHITE);y+=fHeight+4.f;
    pFont->PrintText(pRI, hkvVec2(40.f,y), "Mask@92:Count@140:Filename", V_RGBA_YELLOW,iFontState);y+=fHeight+6.f;

    const VDecorationDensityChannelCollection &decoColl(pSector->m_DecorationChannels);
    for (int i=0;i<decoColl.Count();i++)
    {
      VDecorationDensityChannel *pChannel = decoColl.GetAt(i);
      if (!pChannel->m_spSource)
        continue;
      int iDecoCount = CountDecorationOfType(pChannel->m_spSource, pSector->m_Decoration);
      RenderTexturedRect(pRI,40.f,y,48.f,pChannel->GetLuminanceTexture(pSector->m_iIndexX,pSector->m_iIndexY,pChannel->m_spSource->m_Properties.m_iModelID));
      sprintf(szBuffer,"%i@144:%s",iDecoCount,pChannel->m_spSource->GetFilename());
      pFont->PrintText(pRI, hkvVec2(96.f,y), szBuffer, VColorRef(220,220,220,255),iFontState);

      y += 52;
    }


    Vision::RenderLoopHelper.EndOverlayRendering();

  }
};




void VEditableTerrainSector::OnDestroying()
{
  VTerrainSector::OnDestroying();

  // we need to clean up this type manually
  if (Vision::Profiling.GetCurrentResourcePreview()!=NULL && Vision::Profiling.GetCurrentResourcePreview()->GetOwner()==this)
    Vision::Profiling.SetCurrentResourcePreview(NULL);
}


IVResourcePreview *VEditableTerrainSector::CreateResourcePreview()
{
  return new VTerrainSectorPreview(this);
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
