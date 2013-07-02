/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainConversionHelpers.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>

using namespace ManagedFramework;
using namespace System::Drawing;
using namespace CSharpFramework::Math;

namespace TerrainManaged
{

  TerrainConversionHelpers::TerrainConversionHelpers()
  {
  }

  void TerrainConversionHelpers::ConfigToNative(TerrainConfig ^pSrc, VTerrainConfig *pDest)
  {
    ConversionUtils::StringToVString(pSrc->TerrainFolder,pDest->m_sTerrainFolder);
    pDest->m_iSectorCount[0] = pSrc->SectorCount.Width;
    pDest->m_iSectorCount[1] = pSrc->SectorCount.Height;
    pDest->m_vSectorSize.x = pSrc->SectorSize.X;
    pDest->m_vSectorSize.y = pSrc->SectorSize.Y;
    pDest->m_vTerrainPos.x = pSrc->TerrainPos.X;
    pDest->m_vTerrainPos.y = pSrc->TerrainPos.Y;
    pDest->m_vTerrainPos.z = pSrc->TerrainPos.Z;
    pDest->m_iHeightSamplesPerSector[0] = pSrc->SamplesPerSector.Width;
    pDest->m_iHeightSamplesPerSector[1] = pSrc->SamplesPerSector.Height;
    pDest->m_iTilesPerSector[0] = pSrc->TilesPerSector.Width;
    pDest->m_iTilesPerSector[1] = pSrc->TilesPerSector.Height;
    ConversionUtils::StringToVString(pSrc->BaseTextureFile,pDest->m_sBaseTextureFile);
    pDest->m_iDefaultWeightmapResolution[0] = pSrc->_defaultWeightmapResolution.Width;
    pDest->m_iDefaultWeightmapResolution[1] = pSrc->_defaultWeightmapResolution.Height;
    pDest->m_iDensityMapSamplesPerSector[0] = pSrc->_defaultDensitymapResolution.Width;
    pDest->m_iDensityMapSamplesPerSector[1] = pSrc->_defaultDensitymapResolution.Height;
    pDest->m_iMaterialMapResolution[0] = pSrc->_materialmapResolution.Width;
    pDest->m_iMaterialMapResolution[1] = pSrc->_materialmapResolution.Height;
    pDest->m_fVisibilityHeightOverTerrain = pSrc->_visibilityHeightOverTerrain;
    pDest->m_fPurgeInterval = pSrc->_fSectorPurgeInterval;
    pDest->m_iMemoryLimit = pSrc->_iMemoryLimit;
    pDest->m_bNormalmapUsesBorder = pSrc->NormalmapUsesBorder;

    pDest->m_iOldSectorStartIndex[0] = pSrc->_oldSectorIndexX;
    pDest->m_iOldSectorStartIndex[1] = pSrc->_oldSectorIndexY;

    pDest->m_iNewSectorStartIndex[0] = pSrc->_newSectorIndexX;
    pDest->m_iNewSectorStartIndex[1] = pSrc->_newSectorIndexY;

    pDest->m_iOldSectorCount[0] = pSrc->_oldSectorCountX;
    pDest->m_iOldSectorCount[1] = pSrc->_oldSectorCountY;

    pDest->Finalize();
  }

  void TerrainConversionHelpers::ConfigFromNative(VTerrainConfig *pSrc, TerrainConfig ^pDest)
  {
    pDest->Centered = false;
    pDest->TerrainFolder = ConversionUtils::VStringToString(pSrc->m_sTerrainFolder.AsChar());
    pDest->SectorCount = Size(pSrc->m_iSectorCount[0],pSrc->m_iSectorCount[1]);
    pDest->SectorSize = Vector2F(pSrc->m_vSectorSize.x,pSrc->m_vSectorSize.y);
    pDest->TerrainPos = Vector3F(pSrc->m_vTerrainPos.x,pSrc->m_vTerrainPos.y,pSrc->m_vTerrainPos.z);
    pDest->SamplesPerSector = Size(pSrc->m_iHeightSamplesPerSector[0],pSrc->m_iHeightSamplesPerSector[1]);
    pDest->TilesPerSector = Size(pSrc->m_iTilesPerSector[0],pSrc->m_iTilesPerSector[1]);
    pDest->BaseTextureFile = ConversionUtils::VStringToString(pSrc->m_sBaseTextureFile.AsChar());
    pDest->_defaultWeightmapResolution.Width = pSrc->m_iDefaultWeightmapResolution[0];
    pDest->_defaultWeightmapResolution.Height = pSrc->m_iDefaultWeightmapResolution[1];
    pDest->_defaultDensitymapResolution.Width = pSrc->m_iDensityMapSamplesPerSector[0];
    pDest->_defaultDensitymapResolution.Height = pSrc->m_iDensityMapSamplesPerSector[1];
    pDest->_materialmapResolution.Width = pSrc->m_iMaterialMapResolution[0];
    pDest->_materialmapResolution.Height = pSrc->m_iMaterialMapResolution[1];

    pDest->_visibilityHeightOverTerrain = pSrc->m_fVisibilityHeightOverTerrain;
    pDest->_fSectorPurgeInterval = pSrc->m_fPurgeInterval;
    pDest->_iMemoryLimit = pSrc->m_iMemoryLimit;
    pDest->_bNormalmapUsesBorder = pSrc->m_bNormalmapUsesBorder;

    pDest->_oldSectorIndexX = pSrc->m_iOldSectorStartIndex[0];
    pDest->_oldSectorIndexY = pSrc->m_iOldSectorStartIndex[1];

    pDest->_newSectorIndexX = pSrc->m_iNewSectorStartIndex[0];
    pDest->_newSectorIndexY = pSrc->m_iNewSectorStartIndex[1];

    pDest->_oldSectorCountX = pSrc->m_iOldSectorCount[0];
    pDest->_oldSectorCountY = pSrc->m_iOldSectorCount[1];

  }

  TerrainBrush ^ TerrainConversionHelpers::CreateBrush(String ^filename)
  {
    TerrainBrush ^pBrush = gcnew TerrainBrush();
    pBrush->Name = filename;
    VString sFilename;
    ConversionUtils::StringToVString(filename,sFilename);
    VisBitmap_cl *pBmp = VisBitmap_cl::LoadBitmapFromFile(sFilename);
    if (pBmp)
      pBmp->AddRef();
    pBrush->NativePtr = System::IntPtr((void *)pBmp);
    VTextureObject *pTex = Vision::TextureManager.Load2DTexture(sFilename);
    if (pTex)
      pTex->AddRef();
    pBrush->NativeTextureObject = System::IntPtr((void *)pTex);

    return pBrush;
  }

  void TerrainConversionHelpers::CheckBrushModified(TerrainBrush ^ brush)
  {
    VisBitmap_cl *pBmp = (VisBitmap_cl *)brush->NativePtr.ToPointer();
    if (pBmp && pBmp->IsLoaded())
    {
      pBmp->EnsureUnloaded();
      pBmp->EnsureLoaded();
    }
    VTextureObject *pTex = (VTextureObject *)brush->NativeTextureObject.ToPointer();
    if (pTex && pTex->IsLoaded())
    {
      pTex->EnsureUnloaded();
      pTex->EnsureLoaded();
    }
  }


  void TerrainConversionHelpers::FreeBrush(TerrainBrush ^ brush)
  {
    VisBitmap_cl *pBmp = (VisBitmap_cl *)brush->NativePtr.ToPointer();
    V_SAFE_RELEASE(pBmp);
    brush->NativePtr = System::IntPtr::Zero;

    VTextureObject *pTex = (VTextureObject *)brush->NativeTextureObject.ToPointer();
    V_SAFE_RELEASE(pTex);
    brush->NativeTextureObject = System::IntPtr::Zero;
  }

  bool TerrainConversionHelpers::V3DHasTerrain()
  {
    return false;
    //return Vision::World.GetHeightmapCount()==1;
  }

  VTextureObject *LoadTextureFile(String ^filename, VString &sDestFilename, int iFlags=VTM_FLAG_DEFAULT_MIPMAPPED)
  {
    if (String::IsNullOrEmpty(filename))
      return NULL;

    if (filename->Contains("%")) // per sector texture
    {
      ConversionUtils::StringToVString(filename,sDestFilename);
      return NULL;
    }

    VString sTempName;
    ConversionUtils::StringToVString(filename,sTempName);

    return Vision::TextureManager.Load2DTexture(sTempName,iFlags);
  }
  
  void TerrainConversionHelpers::DetailTexturePropertiesToNative(VTextureWeightmapChannelResource::DetailTextureProperties_t *pDest, DetailTextureResource ^pSrc)
  {
    // fill out props here
    pDest->m_iTextureID = pSrc->ID;
    pDest->m_iMaterialID = pSrc->MaterialID;
    pDest->m_vTilingAndOfs.set (pSrc->Tiling.X,pSrc->Tiling.Y,pSrc->Offset.X,pSrc->Offset.Y);
    pDest->m_bUseThreeWayMapping = pSrc->UseThreeWayMapping;

    pDest->m_spDiffuseTexture = LoadTextureFile(pSrc->DiffuseFilename, pDest->m_sDiffuseTexFile, VTM_FLAG_DEFAULT_MIPMAPPED | VTM_FLAG_ASSUME_SRGB);
    pDest->m_spNormalmapTexture = LoadTextureFile(pSrc->NormalmapFilename, pDest->m_sNormalmapTexFile);
    pDest->m_spSpecularTexture = LoadTextureFile(pSrc->SpecularmapFilename, pDest->m_sSpecularTexFile);
    pDest->m_fBakePriority = pSrc->_fBakePriority;
    pDest->m_bUseClamping = pSrc->_bUseClamping;
  }


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
