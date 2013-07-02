/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Math/LargePosition.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


void VTerrainConfig::Finalize()
{
  m_iSectorMeshesPerSector[0] = hkvMath::Max(m_iHeightSamplesPerSector[0]/TERRAIN_SECTORMESH_IDEAL_SIZE,1);
  m_iSectorMeshesPerSector[1] = hkvMath::Max(m_iHeightSamplesPerSector[1]/TERRAIN_SECTORMESH_IDEAL_SIZE,1);
  m_iSectorMeshCount = m_iSectorMeshesPerSector[0]*m_iSectorMeshesPerSector[1];
  m_iSectorMeshVertexCount[0] = m_iHeightSamplesPerSector[0]/m_iSectorMeshesPerSector[0];
  m_iSectorMeshVertexCount[1] = m_iHeightSamplesPerSector[1]/m_iSectorMeshesPerSector[1];
  m_iVerticesPerMesh = (m_iSectorMeshVertexCount[0]+1)*(m_iSectorMeshVertexCount[0]+1);
  m_iHeightSamplesPerTile[0] = m_iHeightSamplesPerSector[0]/m_iTilesPerSector[0];
  m_iHeightSamplesPerTile[1] = m_iHeightSamplesPerSector[1]/m_iTilesPerSector[1];
  m_iOverallHeightSampleCount[0] = m_iHeightSamplesPerSector[0]*m_iSectorCount[0];
  m_iOverallHeightSampleCount[1] = m_iHeightSamplesPerSector[1]*m_iSectorCount[1];
  m_iTilesPerSectorMesh[0] = m_iTilesPerSector[0] / m_iSectorMeshesPerSector[0];
  m_iTilesPerSectorMesh[1] = m_iTilesPerSector[1] / m_iSectorMeshesPerSector[1];
  m_iOverallMeshPageCount[0] = m_iSectorCount[0]*m_iSectorMeshesPerSector[0];
  m_iOverallMeshPageCount[1] = m_iSectorCount[1]*m_iSectorMeshesPerSector[1];
  m_iTilesPerSectorCount = m_iTilesPerSector[0] * m_iTilesPerSector[1];

  // additional initialisation
  m_vInvSectorSize.set(1.f/m_vSectorSize.x,1.f/m_vSectorSize.y);

  m_vWorld2Sample.x = (float)m_iHeightSamplesPerSector[0] / m_vSectorSize.x;
  m_vWorld2Sample.y = (float)m_iHeightSamplesPerSector[1] / m_vSectorSize.y;
  m_vSampleSpacing.x = 1.f/m_vWorld2Sample.x;
  m_vSampleSpacing.y = 1.f/m_vWorld2Sample.y; 
  m_vTileSize.x = m_vSectorSize.x/(float)m_iTilesPerSector[0];
  m_vTileSize.y = m_vSectorSize.y/(float)m_iTilesPerSector[1];
  m_vWorld2Tile.x = (float)m_iTilesPerSector[0]/m_vSectorSize.x;
  m_vWorld2Tile.y = (float)m_iTilesPerSector[1]/m_vSectorSize.y;

  m_vWorld2DensityMap.x = (float)m_iDensityMapSamplesPerSector[0] / m_vSectorSize.x;
  m_vWorld2DensityMap.y = (float)m_iDensityMapSamplesPerSector[1] / m_vSectorSize.y;
  m_vDensitySampleSpacing.x = 1.f/m_vWorld2DensityMap.x;
  m_vDensitySampleSpacing.y = 1.f/m_vWorld2DensityMap.y;
  m_vNormalGradientSpacing = m_vSampleSpacing*1.f;

  m_iMaxMeshLOD = m_iMaxMeshLODHoles = 0;
  int iEdge = hkvMath::Min(m_iSectorMeshVertexCount[0],m_iSectorMeshVertexCount[1]);
  int iTile = hkvMath::Min(m_iHeightSamplesPerTile[0],m_iHeightSamplesPerTile[1]);

  while (iEdge>2 && m_iMaxMeshLOD<MAX_MESH_LOD) // smallest is a 4x4 tile
  {
    m_iMaxMeshLOD++;
    if ((1<<m_iMaxMeshLOD)<=iTile)
      m_iMaxMeshLODHoles = m_iMaxMeshLOD;
    iEdge/=2;
  }

  m_iReplacementLOD = hkvMath::Min(m_iReplacementLOD,m_iMaxMeshLODHoles);
  m_iSamplesPerReplacementMesh[0] = m_iSamplesPerReplacementMesh[1] = 0;

  if (m_iReplacementLOD>0)
  {
    m_iSamplesPerReplacementMesh[0] = m_iHeightSamplesPerSector[0] >> m_iReplacementLOD;
    m_iSamplesPerReplacementMesh[1] = m_iHeightSamplesPerSector[1] >> m_iReplacementLOD;
    while (m_iSamplesPerReplacementMesh[0]>128 || m_iSamplesPerReplacementMesh[1]>128)
    {
      m_iReplacementLOD++;
      m_iSamplesPerReplacementMesh[0] /= 2;
      m_iSamplesPerReplacementMesh[1] /= 2;
    }
  }

  m_bSupportsReplacement = m_iReplacementLOD>0 && !Vision::Editor.IsInEditor();

  // calc optimal replacement mesh distance
  if (m_bSupportsReplacement && m_fReplacementDistance<0.f)
    m_fReplacementDistance = hkvMath::Max(m_vSectorSize.x,m_vSectorSize.y) * ((float)m_iReplacementLOD+0.1f);

  m_bFinalized = true;
}


void VTerrainConfig::GetViewDistanceBox(hkvAlignedBBox &destbox, const hkvVec3& vCamPos, float fViewDistance) const
{
  const hkvVec3 vRad(fViewDistance+m_vSectorSize.x*m_fPrecacheMargin,fViewDistance+m_vSectorSize.y*m_fPrecacheMargin,1000000.f);
  destbox.m_vMin = (hkvVec3) vCamPos - vRad;
  destbox.m_vMax = (hkvVec3) vCamPos + vRad;
}


void VTerrainConfig::GetHeightSampleIndicesAtPos(const VLargePosition &vPos, int &x, int &y) const
{
  VASSERT(vPos.IsValid(*this));
  x = vPos.m_iSectorX*m_iHeightSamplesPerSector[0] + (int)(vPos.m_vSectorOfs.x*m_vWorld2Sample.x);
  y = vPos.m_iSectorY*m_iHeightSamplesPerSector[1] + (int)(vPos.m_vSectorOfs.y*m_vWorld2Sample.y);
}

void VTerrainConfig::GetTileIndicesAtPos(const VLargePosition &vPos, int &x, int &y) const
{
  VASSERT(vPos.IsValid(*this));
  x = vPos.m_iSectorX*m_iTilesPerSector[0] + (int)(vPos.m_vSectorOfs.x*m_vWorld2Tile.x);
  y = vPos.m_iSectorY*m_iTilesPerSector[1] + (int)(vPos.m_vSectorOfs.y*m_vWorld2Tile.y);
}

void VTerrainConfig::GetDensitySampleIndicesAtPos(const VLargePosition &vPos, int &x, int &y) const
{
  VASSERT(vPos.IsValid(*this));
  x = vPos.m_iSectorX*m_iDensityMapSamplesPerSector[0] + (int)(vPos.m_vSectorOfs.x*m_vWorld2DensityMap.x);
  y = vPos.m_iSectorY*m_iDensityMapSamplesPerSector[1] + (int)(vPos.m_vSectorOfs.y*m_vWorld2DensityMap.y);
}

void VTerrainConfig::GetWeightmapSampleIndicesAtPos(const VLargePosition &vPos, int &x, int &y) const
{
  VASSERT(vPos.IsValid(*this));
  x = vPos.m_iSectorX*m_iDefaultWeightmapResolution[0] + (int)(vPos.m_vSectorOfs.x*m_vInvSectorSize.x*(float)m_iDefaultWeightmapResolution[0]);
  y = vPos.m_iSectorY*m_iDefaultWeightmapResolution[1] + (int)(vPos.m_vSectorOfs.y*m_vInvSectorSize.y*(float)m_iDefaultWeightmapResolution[1]);
}

void VTerrainConfig::GetRelativeTerrainPos(const VLargePosition &vPos, float &x, float &y) const
{
  VASSERT(vPos.IsValid(*this));
  x = ((float)vPos.m_iSectorX + vPos.m_vSectorOfs.x*m_vInvSectorSize.x) / (float)m_iSectorCount[0] ;
  y = ((float)vPos.m_iSectorY + vPos.m_vSectorOfs.y*m_vInvSectorSize.y) / (float)m_iSectorCount[1] ;
}

void VTerrainConfig::GetTerrainBBox(VLargeBoundingBox &bbox) const
{
  bbox.m_vMin.m_iSectorX = 0;
  bbox.m_vMin.m_iSectorY = 0;
  bbox.m_vMax.m_iSectorX = m_iSectorCount[0];
  bbox.m_vMax.m_iSectorY = m_iSectorCount[1];
  bbox.m_vMin.m_vSectorOfs.set(0,0,0);
  bbox.m_vMax.m_vSectorOfs.set(0,0,0);
}

void VTerrainConfig::GetSectorBBox(VLargeBoundingBox &bbox,int x, int y) const
{
  bbox.m_vMin.m_iSectorX = x;
  bbox.m_vMin.m_iSectorY = y;
  bbox.m_vMax.m_iSectorX = x+1;
  bbox.m_vMax.m_iSectorY = y+1;
  bbox.m_vMin.m_vSectorOfs.set(0,0,0);
  bbox.m_vMax.m_vSectorOfs.set(0,0,0);
}

void VTerrainConfig::GetWorldSpaceToSectorTransform(int iSectorX, int iSectorY, hkvVec4& dest) const
{
  hkvVec2 vTemp = GetSectorOrigin(iSectorX,iSectorY).getAsVec2 ();
  dest.x = m_vInvSectorSize.x;
  dest.y = m_vInvSectorSize.y;
  dest.z = -dest.x * vTemp.x;
  dest.w = -dest.y * vTemp.y;
}

//Returns project local directory
bool VTerrainConfig::GetDirectory(char *szPath, const char *szFolder, bool bTempDir) const
{
  VASSERT(szFolder);
  VASSERT(!m_sTerrainFolder.IsEmpty());
  if (m_sTerrainFolder.IsEmpty()) 
    return false;

  char szRoot[FS_MAX_PATH];
  strcpy(szRoot,m_sTerrainFolder);
  if (bTempDir)
    strcat(szRoot,"_temp");   //TODO: "\\.temp\\"  ?

  VFileHelper::CombineDirAndFile(szPath, szRoot, szFolder);

  return true;
}

bool VTerrainConfig::GetDirectory(char *szPath, int eDirType, bool bTemp) const
{
  
  switch(eDirType & 0xff00) //The high word is the directory type
  {
    case VTC_DIR_EDITING: return GetDirectory(szPath, "Editing", bTemp);
    case VTC_DIR_TEXTURE: return GetDirectory(szPath, "AuxiliaryTextures", bTemp);
    case VTC_DIR_SECTORS: return GetDirectory(szPath, "Sectors", bTemp);
    case VTC_DIR_MESH:    return GetDirectory(szPath, "Meshes", bTemp);
  }
  
  return false;
}


bool VTerrainConfig::IsTempDirFile(const char *szFilename) const
{
  char szFolder[FS_MAX_PATH];
  sprintf(szFolder,"%s_temp\\",m_sTerrainFolder.AsChar());
  int iLen = (int)strlen(szFolder);
  bool bCmp = _strnicmp(szFilename,szFolder,iLen)==0;
  return bCmp;
}

  
const char* VTerrainConfig::FixTempDirFile(const char *szFilename, char *szBuffer) const
{
  char szFolder[FS_MAX_PATH];
  sprintf(szFolder,"%s_temp\\",m_sTerrainFolder.AsChar());
  int iLen = (int)strlen(szFolder);
  if (_strnicmp(szFilename,szFolder,iLen)!=0)
    return szFilename; // this is not a temp dir file

  VFileHelper::CombineDirAndFile(szBuffer,m_sTerrainFolder,&szFilename[iLen]);

  return szBuffer;
}


bool VTerrainConfig::GetSectorAuxiliaryTextureFilename(char *szPath, int iSectorX, int iSectorY, const char *szNameBase, bool bForSaving) const
{
  char szDir[FS_MAX_PATH];
  char szExtName[FS_MAX_PATH];
  // check in both directories
  int iStartDir = Vision::Editor.IsInEditor() ? 0 : 1;

  for (int iDir=iStartDir;iDir<2;iDir++)
  {
    if (!GetAuxiliaryTextureDir(szDir,iDir==0))
      return false;
    sprintf(szExtName,"%s_%02i_%02i.dds",szNameBase,iSectorX,iSectorY);
    VFileHelper::CombineDirAndFile(szPath,szDir,szExtName);
    if (bForSaving || !Vision::Editor.IsInEditor())
      return true;
    if (Vision::File.Exists(szPath))
      return true;
  }
  return false;
}


bool VTerrainConfig::GetSectorEditingFilename(char *szPath, int iSectorX, int iSectorY, bool bForSaving) const
{
  char szDir[FS_MAX_PATH];
  char szExtName[FS_MAX_PATH];
  // check in both directories
  for (int iDir=0;iDir<2;iDir++)
  {
    if (!GetEditingDir(szDir,iDir==0))
      return false;
    sprintf(szExtName,"SectorInfo_%02i_%02i.dat",iSectorX,iSectorY);
    VFileHelper::CombineDirAndFile(szPath,szDir,szExtName);
    if (bForSaving)
      return true;
    if (Vision::File.Exists(szPath))
      return true;
  }
  return false;
}


bool VTerrainConfig::GetSectorMeshFilename(char *szPath, int iSectorX, int iSectorY, bool bForSaving) const
{
  char szDir[FS_MAX_PATH];
  char szExtName[FS_MAX_PATH];
  // check in both directories
  for (int iDir=0;iDir<2;iDir++)
  {
    if (!GetDirectory(szDir, VTC_DIR_MESH, m_bUseTempFolder & (iDir==0)))
      return false;
    sprintf(szExtName,"Sector_%02i_%02i.vmesh",iSectorX,iSectorY);
    VFileHelper::CombineDirAndFile(szPath,szDir,szExtName);
    if (bForSaving)
      return true;
    if (Vision::File.Exists(szPath))
      return true;
  }
  return false;
}


bool VTerrainConfig::GetSectorCollisionFilename(char *szPath, int iSectorX, int iSectorY, bool bForSaving) const
{
  char szDir[FS_MAX_PATH];
  char szExtName[FS_MAX_PATH];
  // check in both directories
  for (int iDir=0;iDir<2;iDir++)
  {
    if (!GetDirectory(szDir, VTC_DIR_MESH, m_bUseTempFolder & (iDir==0)))
      return false;
    sprintf(szExtName,"Sector_%02i_%02i.vcolmesh",iSectorX,iSectorY);
    VFileHelper::CombineDirAndFile(szPath,szDir,szExtName);
    if (bForSaving)
      return true;
    if (Vision::File.Exists(szPath))
      return true;
  }
  return false;
}


bool VTerrainConfig::GetFilename(char *szFilename, int iSectorX, int iSectorY, int iFileType) const
{
  switch (iFileType)
  {
    case VTC_FILE_SECTORS_MESH:  sprintf(szFilename, "Sector_%02i_%02i.mesh", iSectorX, iSectorY); return true;
    case VTC_FILE_SECTORS_XML:   sprintf(szFilename, "Sector_%02i_%02i.xml",   iSectorX, iSectorY); return true;
    case VTC_FILE_SECTORS_HMAP:  sprintf(szFilename, "Sector_%02i_%02i.hmap",  iSectorX, iSectorY); return true;
    case VTC_FILE_SECTORS_VRES:  sprintf(szFilename, "Sector_%02i_%02i.vres",  iSectorX, iSectorY); return true;

    case VTC_FILE_EDITING_DATA:  sprintf(szFilename, "SectorInfo_%02i_%02i.dat",  iSectorX, iSectorY); return true;

    case VTC_FILE_TEXTURES_NORMAL:   sprintf(szFilename, "Normalmap_%02i_%02i.dds",  iSectorX, iSectorY); return true;
    case VTC_FILE_TEXTURES_DIFFUSE:  sprintf(szFilename, "DiffuseReplacement_%02i_%02i.dds",  iSectorX, iSectorY); return true;
    case VTC_FILE_TEXTURES_WEIGHT:   sprintf(szFilename, "Weightmap%%s_%02i_%02i.dds",  iSectorX, iSectorY); return true;  //%s so we can substitute "*" or int

    case VTC_FILE_MESHES_VMESH:     sprintf(szFilename, "Sector_%02i_%02i.vmesh", iSectorX, iSectorY); return true;
    case VTC_FILE_MESHES_VCOLMESH:  sprintf(szFilename, "Sector_%02i_%02i.vcolmesh", iSectorX, iSectorY); return true;
  }
  
  return false;
}

bool VTerrainConfig::GetSectorFile(char *szPath, int iSectorX, int iSectorY, int iFileType) const
{
  char szDir[FS_MAX_PATH];
  char szFilename[FS_MAX_PATH];
  
  if (!GetDirectory(szDir, iFileType, false)) // we don't want the temp folder
    return false;
  
  if (!GetFilename(szFilename, iSectorX, iSectorY, iFileType ))
    return false;

  VFileHelper::CombineDirAndFile(szPath,szDir,szFilename);
  return true;
}

bool VTerrainConfig::GetMovedSectorFilename(char *szPath, int iSectorX, int iSectorY, const char *szExt, bool bTemp ) const
{  
  int iNewSectorX = ( m_iNewSectorStartIndex[0] - m_iOldSectorStartIndex[0] ) + iSectorX;
  int iNewSectorY = ( m_iNewSectorStartIndex[1] - m_iOldSectorStartIndex[1] ) + iSectorY;

  if ( iNewSectorX >= m_iOldSectorCount[0] || iNewSectorY >= m_iOldSectorCount[1] )
    return false;	 

  if ( iNewSectorX < 0 || iNewSectorY < 0 )
    return false;

  return GetSectorFilename( szPath, iNewSectorX, iNewSectorY, szExt, bTemp );
}

bool VTerrainConfig::GetSectorFilename(char *szPath, int iSectorX, int iSectorY, const char *szExt, bool bTemp) const
{
  char szDir[FS_MAX_PATH];
  char szFilename[FS_MAX_PATH];
  
  if (!GetSectorCacheDir(szDir,bTemp))
    return false;
    
  if (szExt)
    sprintf(szFilename,"Sector_%02i_%02i.%s",iSectorX,iSectorY,szExt);
  else
    sprintf(szFilename,"Sector_%02i_%02i",iSectorX,iSectorY);

  VFileHelper::CombineDirAndFile(szPath,szDir,szFilename);
  return true;
}


bool VTerrainConfig::GetSectorCacheFilename(char *szPath, int iSectorX, int iSectorY, const char *szExt, bool bForSaving) const
{
  int iFirstDir = Vision::Editor.IsInEditor() ? 0 : 1; // outside the editor, don't allow access from temp dir

  // check in both directories (temp, then real)
  for (int iDir=iFirstDir;iDir<2;iDir++)
  {
    if ( IsMovedSectorStartPosition() == true && bForSaving == false )
    {
      if (!GetMovedSectorFilename(szPath, iSectorX, iSectorY, szExt, (iDir==0)))
        return false;
    }
    else
    {
      if (!GetSectorFilename(szPath, iSectorX, iSectorY, szExt, (iDir==0) ))
        return false;
    }

    if (bForSaving || !Vision::Editor.IsInEditor())
      return true; 
    if (Vision::File.Exists(szPath))
      return true;
  }
  return false;

  //easier to understand code?
  /*
  if (Vision::Editor.IsInEditor())
  {
    if (!GetSectorFilename(szPath, iSectorX, iSectorY, szExt, true)) //try temp dir
      return false;
    if (bForSaving) return true;   //use temp dir when saving
    if (Vision::File.Exists(szPath))
      return true;  // use temp dir for loading if it exists
  }

  //try real dir        
  if (!GetSectorFilename(szPath, iSectorX, iSectorY, szExt, false)) 
    return false;
  return (Vision::File.Exists(szPath)); // use for loading if it exists 
  */
}

//Helper that makes subdirectories for us
#define _MAKE_DIR(_T)              \
  GetDirectory(szDir,_T, bTemp);   \
  VFileHelper::CombineDirAndFile(szAbsDir, szProjectDir,szDir);\
  if (!VFileHelper::ExistsDir(szAbsDir))            \
    if (!VFileHelper::MkDirRecursive(szAbsDir))     \
      return false;

bool VTerrainConfig::MakeRelevantDirectories(const char *szProjectDir, bool bTemp) const
{
  VASSERT(!m_sTerrainFolder.IsEmpty());
  VASSERT(VPathHelper::IsAbsolutePath(szProjectDir));
  
  char szAbsDir[FS_MAX_PATH];
  char szDir[FS_MAX_PATH];

  _MAKE_DIR("");  //make sure terrain folder exists
  _MAKE_DIR(VTC_DIR_EDITING);  //make sure editing folder exists
  _MAKE_DIR(VTC_DIR_TEXTURE);  //make sure texture folder exists
  _MAKE_DIR(VTC_DIR_SECTORS);  //make sure sector folder exists
  _MAKE_DIR(VTC_DIR_MESH);     //make sure mesh folder exists

  return true;
}

#undef _MAKE_DIR

void VTerrainConfig::ChunkFileSerialize(VChunkFile &file)
{
  if (file.IsLoading())
  {
    int iVersion=-1;
    file.ReadInt(iVersion);
    file.ReadString(m_sTerrainFolder);
    file.ReadInt(m_iSectorCount[0]);
    file.ReadInt(m_iSectorCount[1]);
    file.ReadFloat(m_vSectorSize.x);
    file.ReadFloat(m_vSectorSize.y);
    if (iVersion >= 2)
    {
      file.ReadFloat(m_vTerrainPos.x);
      file.ReadFloat(m_vTerrainPos.y);
      file.ReadFloat(m_vTerrainPos.z);
    }
    if (iVersion >= 3)
      file.ReadFloat(m_fPurgeInterval);
    if (iVersion >= 5)
      file.ReadFloat(m_fVisibilityHeightOverTerrain);
    if (iVersion <= 1)
      file.ReadFloat(m_fViewDistance);
    file.ReadInt(m_iHeightSamplesPerSector[0]);
    file.ReadInt(m_iHeightSamplesPerSector[1]);
    file.ReadInt(m_iTilesPerSector[0]);
    file.ReadInt(m_iTilesPerSector[1]);
    file.ReadInt(m_iDensityMapSamplesPerSector[0]);
    file.ReadInt(m_iDensityMapSamplesPerSector[1]);
    if (iVersion <= 1)
    {
      float fDummy = 1.0f;
      file.ReadFloat(fDummy);//m_vBaseTextureTiling.x);
      file.ReadFloat(fDummy);//m_vBaseTextureTiling.y);
    }
    m_iDefaultWeightmapResolution[0] = m_iDefaultWeightmapResolution[1] = 512;
    if (iVersion >= 4)
    {
      file.ReadInt(m_iDefaultWeightmapResolution[0]);
      file.ReadInt(m_iDefaultWeightmapResolution[1]);
    }
    if (iVersion >= 6)
    {
      file.ReadInt(m_iReplacementLOD);
      file.ReadFloat(m_fReplacementDistance);
    }
    if (iVersion >= 7)
      file.Readbool(m_bSortTextureChannels);
    if (iVersion >= 8)
      file.Readbool(m_bDistanceBasedLODOnly);
    if (iVersion >= 9)
      file.Readbool(m_bIgnoreHeightForLOD);
    if (iVersion >= 10)
      file.Readbool(m_bSceneSpecificLightmaps);
    if (iVersion >= 14)
      file.Readbool(m_bNormalmapUsesBorder);
    if (iVersion >= 15)
    {
      file.ReadInt(m_iMaterialMapResolution[0]);
      file.ReadInt(m_iMaterialMapResolution[1]);
    }
    if (iVersion == 16)
    {
      // m_bUseThreeWayMapping has been written as an integer in version 16
      int tmp;
      file.ReadInt(tmp);
      m_bUseThreeWayMapping = tmp != 0;
    }
    if (iVersion >= 17)
    {
      file.Readbool(m_bUseThreeWayMapping);
      file.Readbool(m_bUseNormalMapping);
      file.Readbool(m_bUseLightMapping);
    }
    if (iVersion >= 18)
      file.ReadInt(m_iMemoryLimit);

    //if (iVersion>=x)
    //  file.ReadFloat(m_fPrecacheMargin);

    VASSERT(IsValid() && "Terrain config is not valid");
    Finalize();
  }
  else
  {
    file.WriteInt(18); // current version
    file.WriteString(m_sTerrainFolder);
    file.WriteInt(m_iSectorCount[0]);
    file.WriteInt(m_iSectorCount[1]);
    file.WriteFloat(m_vSectorSize.x);
    file.WriteFloat(m_vSectorSize.y);
    file.WriteFloat(m_vTerrainPos.x);                // version 2
    file.WriteFloat(m_vTerrainPos.y);
    file.WriteFloat(m_vTerrainPos.z);
    file.WriteFloat(m_fPurgeInterval);               // version 3
    file.WriteFloat(m_fVisibilityHeightOverTerrain); // version 5

    file.WriteInt(m_iHeightSamplesPerSector[0]);
    file.WriteInt(m_iHeightSamplesPerSector[1]);
    file.WriteInt(m_iTilesPerSector[0]);
    file.WriteInt(m_iTilesPerSector[1]);
    file.WriteInt(m_iDensityMapSamplesPerSector[0]);
    file.WriteInt(m_iDensityMapSamplesPerSector[1]);
    file.WriteInt(m_iDefaultWeightmapResolution[0]); // version 4
    file.WriteInt(m_iDefaultWeightmapResolution[1]);

    file.WriteInt(m_iReplacementLOD);                // version 6
    file.WriteFloat(m_fReplacementDistance);         // version 6

    file.Writebool(m_bSortTextureChannels);          // version 7
    file.Writebool(m_bDistanceBasedLODOnly);         // version 8
    file.Writebool(m_bIgnoreHeightForLOD);           // version 9
    file.Writebool(m_bSceneSpecificLightmaps);       // version 10
    file.Writebool(m_bNormalmapUsesBorder);          // version 14
    file.WriteInt(m_iMaterialMapResolution[0]);      // version 15
    file.WriteInt(m_iMaterialMapResolution[1]);
    file.Writebool(m_bUseThreeWayMapping);           // version 17
    file.Writebool(m_bUseNormalMapping);
    file.Writebool(m_bUseLightMapping);
    file.WriteInt(m_iMemoryLimit);                   // version 18
    //file.WriteFloat(m_fPrecacheMargin);            // version x+1 (not yet exposed in vForge, so no need to save it)
  }
}

bool VTerrainConfig::RecursiveCreateDir(const char *szProjRelpath) const
{
  VASSERT(!m_sAbsProjectDir.IsEmpty());
  
  #if 0
  char tmp[FS_MAX_PATH];
  sprintf(tmp, "LOG:RecursiveCreateDir(%s)\r\n", szProjRelpath); //HACK
  OutputDebugString(tmp); 
  #endif
  
  //TODO: Use VFileHelper:: instead?

  const char *szStart = szProjRelpath;
  char szPath[FS_MAX_PATH];
  while (szProjRelpath && szProjRelpath[0])
  {
    const char *szSep = strchr(szProjRelpath,'\\');
    if (szSep)
    {
      int iLen = (int)(szSep-szStart);
      strncpy(szPath,szStart,iLen);
      szPath[iLen] = 0;
      szProjRelpath = &szStart[iLen+1];
    }
    else
    {
      strcpy(szPath,szStart);
      szProjRelpath = NULL;
    }

    if (szPath[0])
    {
      char szAbsPath[FS_MAX_PATH];
      VFileHelper::CombineDirAndFile(szAbsPath,m_sAbsProjectDir,szPath);
      if (!VFileHelper::ExistsDir(szAbsPath))
        if (!VFileHelper::MkDir(szAbsPath))
          return false;
    }
  }

  return true;
}

int VTerrainConfig::RCSPerformAction(const char *szFilename, int eAction) const
{
  if (!m_bUseTempFolder && !m_sAbsProjectDir.IsEmpty())   // saving to real directory and in editor? 
  {
    static bool bCheck = true;
    if (bCheck)
    {
      VASSERT(ContainsI(szFilename, "_temp") == NULL); // should not be called for temp files
    }
    // use RCS
    if (VPathHelper::IsAbsolutePath(szFilename))
      return VFileAccessManager::RCSPerformAction(szFilename, eAction); //use absolute path
    
    return VFileAccessManager::RCSPerformAction(szFilename, eAction, m_sAbsProjectDir); // use project dir
  }    
  return true;
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
