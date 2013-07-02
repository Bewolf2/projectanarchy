/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainConfig.hpp

#ifndef TERRAINCONFIG_HPP_INCLUDED
#define TERRAINCONFIG_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>

#define MAX_MESH_LOD 8

#define TERRAIN_COLLISIONMESHES_SECTORLOCALSPACE
#undef TERRAIN_COLLISIONMESHES_SECTORLOCALSPACE

#define TERRAIN_SECTORMESH_IDEAL_SIZE   128

class VLargePosition;
class VLargeBoundingBox;

#define COPY_MEMBER(m) {this->m = other.m;}

//Constants for use with GetDirectory
#define VTC_DIR_SECTORS  0x000  // Directory where sector files go for engine
#define VTC_DIR_TEXTURE  0x100  // Directory where auxiliary texture files go (normalmaps, weightmaps)
#define VTC_DIR_EDITING  0x200  // Directory where editing related sector files go
#define VTC_DIR_MESH     0x300  // Directory where mobile mesh data files go

//Constants for use with GetFilename and GetSectorFile
#define VTC_FILE_SECTORS_MESH       (VTC_DIR_SECTORS | 0)   // Mesh file for engine
#define VTC_FILE_SECTORS_XML        (VTC_DIR_SECTORS | 1)   // XML file for engine
#define VTC_FILE_SECTORS_HMAP       (VTC_DIR_SECTORS | 2)   // HMAP file for engine
#define VTC_FILE_SECTORS_VRES       (VTC_DIR_SECTORS | 3)   // VRES file for engine
                                                        
#define VTC_FILE_EDITING_DATA       (VTC_DIR_EDITING | 0)   // Data file for editor
                                                        
#define VTC_FILE_TEXTURES_NORMAL    (VTC_DIR_TEXTURE | 0)   // Normalmap texture
#define VTC_FILE_TEXTURES_DIFFUSE   (VTC_DIR_TEXTURE | 1)   // Diffuse replacement texture
#define VTC_FILE_TEXTURES_WEIGHT    (VTC_DIR_TEXTURE | 2)   // Weightmap texture. Contains %s so we can substitute "*" or int for weight #

#define VTC_FILE_MESHES_VMESH       (VTC_DIR_MESH | 0)      // Mesh file representing a sector when using mesh export.
#define VTC_FILE_MESHES_VCOLMESH    (VTC_DIR_MESH | 1)      // Collision file for mesh export.


/// \brief
///   Config structure that holds the properties of a single terrain. See VTerrain::Config()
class VTerrainConfig
{
public:
  ///
  /// @name Mandatory base members
  /// @{
  ///

  /// \brief
  ///   Constructor that initializes defaults
  inline VTerrainConfig() {m_bFinalized=false;InitDefaults();}

  VString m_sTerrainFolder;             ///< folder for storing terrain related files
  VString m_sAbsTerrainDir;             ///< terrain directory. Only valid inside vForge, filled in by EngineInstanceTerrain::SetConfig
  VString m_sAbsProjectDir;             ///< project directory. Only valid inside vForge, filled in by EngineInstanceTerrain::SetConfig
  mutable bool m_bUseTempFolder;        ///< internal use
  bool m_bSortTextureChannels;          ///< internal use
  bool m_bDistanceBasedLODOnly;         ///< if enabled, the LOD computation is only distance based, not terrain noise based
  bool m_bIgnoreHeightForLOD;           ///< if enabled (not default), then terrain LOD will only consider XY of the camera distance, not the height
  bool m_bNormalmapUsesBorder;          ///< if specified, the worldspace sector normalmaps are generated with a filtering border
  bool m_bSceneSpecificLightmaps;       ///< if enabled, lightmaps are saved per scene, not per terrain
  bool m_bUseThreeWayMapping;           ///< if enabled the terrain uses three way mapping
  bool m_bUseNormalMapping;             ///< if enabled the terrain uses normal mapping
  bool m_bUseLightMapping;              ///< if enabled the terrain uses light mapping

  // terrain specific
  int m_iSectorCount[2];                ///< number of sectors in x-y direction
  hkvVec2 m_vSectorSize;                ///< x-y dimension of a sector
  hkvVec3 m_vTerrainPos;                ///< x-y-z offset of the terrain

  // single sector related
  int m_iHeightSamplesPerSector[2];     ///< number of height map samples per sector
  int m_iTilesPerSector[2];             ///< number of tile subdivisions for each sector
  int m_iDensityMapSamplesPerSector[2]; ///< number of luminance samples for vegetation density maps. TODO: maybe per sector or even per channel?
  int m_iDefaultWeightmapResolution[2]; ///< default resolution for a sector's weightmap texture
  int m_iMaterialMapResolution[2];      ///< if larger than 0 then this defines the resolution for a material ID map (See VTerrainSectorManager::GetMaterialIDAtWorldPos)

  // texturing
  hkvVec2 m_vBaseTextureTiling;         ///< tiling number of base texture on sector
  VString m_sBaseTextureFile;           ///< only for creation

  // replacement meshes
  int m_iReplacementLOD;                ///< future use
  float m_fReplacementDistance;         ///< future use

  // others
  float m_fPurgeInterval;               ///< purge interval in seconds for uncaching sectors
  float m_fVisibilityHeightOverTerrain; ///< height offset of visibility zones over the terrain
  float m_fPrecacheMargin;              ///< number of sectors to cache around the actual visible distance. The default is 2.0 (sectors)
  int   m_iMemoryLimit;                 ///< memory limit for terrain sectors in megabytes

  // For Terrain sector expansion
  int m_iOldSectorCount[2];	
  int m_iOldSectorStartIndex[2]; // in order to determine sector position when sector is expanded
  int m_iNewSectorStartIndex[2];

  inline bool IsMovedSectorStartPosition() const
  {
	  if ( m_iOldSectorStartIndex[0] != 0 || m_iOldSectorStartIndex[1] != 0 )
		  return true;
	  return false;
  }

  /// \brief
  ///   Initializes useful default
  inline void InitDefaults()
  {
    m_bSortTextureChannels = false;
    m_bUseTempFolder = false;
    m_sTerrainFolder = "TerrainScene";
    m_iSectorCount[0] = m_iSectorCount[1] = 16;
    m_fViewDistance = 100000.f;
    m_iHeightSamplesPerSector[0] = m_iHeightSamplesPerSector[1] = 128; ///< ideal size
    m_vSectorSize.set(10000.f,10000.f);
    m_iTilesPerSector[0] = m_iTilesPerSector[1] = 16;
    m_iDensityMapSamplesPerSector[0] = m_iDensityMapSamplesPerSector[1] = 32;
    m_vTerrainPos.set(0.f,0.f,0.f);
    m_fPurgeInterval = 100.f;
    m_iMemoryLimit = 500;
    m_iDefaultWeightmapResolution[0] = m_iDefaultWeightmapResolution[1] = 512;
    m_iMaterialMapResolution[0] = m_iMaterialMapResolution[1] = 0; // no material map by default

    m_fVisibilityHeightOverTerrain = 2000.f;
    m_iReplacementLOD = -1;
    m_fReplacementDistance = -1.f;
    m_bDistanceBasedLODOnly = false;
    m_bIgnoreHeightForLOD = false;
    m_bNormalmapUsesBorder = false;
    m_bSceneSpecificLightmaps = false;
    m_bUseThreeWayMapping = false;
    m_bUseNormalMapping = false;
    m_bUseLightMapping = false;
    m_fPrecacheMargin = 1.0f;

	m_iOldSectorCount[0] = 0;	
	m_iOldSectorCount[1] = 0;	

	m_iOldSectorStartIndex[0] = 0;	// For Terrain sector expansion
	m_iOldSectorStartIndex[1] = 0;

	m_iNewSectorStartIndex[0] = 0;	// For Terrain sector expansion
	m_iNewSectorStartIndex[1] = 0;

    Finalize();
  }

  /// \brief
  ///   Must be called after a config has been setup. This function calculates all the members that are implicitly given.
  TERRAIN_IMPEXP void Finalize();

  /// \brief
  ///   Performs sanity checks on the config
  ///
  /// \return
  ///   true if the config seems OK
  inline bool IsValid() const
  {
    VASSERT(m_bFinalized);
    if (m_iSectorCount[0]<=0 || m_iSectorCount[1]<=0) return false;
    if (m_iSectorCount[0]>1024 || m_iSectorCount[1]>1024) return false; ///< sanity check
    if (m_iHeightSamplesPerSector[0]<=4 || m_iHeightSamplesPerSector[1]<=4) return false;
    if (m_iTilesPerSector[0]<=0 || m_iTilesPerSector[1]<=0) return false;
    if (m_iTilesPerSector[0]>m_iHeightSamplesPerSector[0] || m_iTilesPerSector[1]>m_iHeightSamplesPerSector[1]) return false;
    if (m_fViewDistance<1.f) return false;
    if (m_fVisibilityHeightOverTerrain<0.f) return false;
    return true;
  }
  
  ///
  /// @}
  ///

  ///
  /// @name Calculation Helper functions
  /// @{
  ///

  /// \brief
  ///   Returns the sector index for given height map sample
  ///
  /// \param iHMSampleX
  ///   height map sample index in x-direction
  /// \return
  ///   sector index (not clamped)
  inline int GetSectorIndexAtSamplePosX(int iHMSampleX) const {return iHMSampleX/m_iHeightSamplesPerSector[0];}

  /// \brief
  ///   Returns the sector index for given height map sample
  ///
  /// \param iHMSampleY
  ///   height map sample index in y-direction
  /// \return
  ///   sector index (not clamped)
  inline int GetSectorIndexAtSamplePosY(int iHMSampleY) const {return iHMSampleY/m_iHeightSamplesPerSector[1];}

  /// \brief
  ///   Returns the height map sample offset inside its sector
  ///
  /// \param iHMSampleX
  ///   height map sample index in x-direction
  /// \return
  ///   offset index
  inline int GetSectorSampleOfsX(int iHMSampleX) const {return iHMSampleX%m_iHeightSamplesPerSector[0];}

  /// \brief
  ///   Returns the height map sample offset inside its sector
  ///
  /// \param iHMSampleY
  ///   height map sample index in y-direction
  /// \return
  ///   offset index
  inline int GetSectorSampleOfsY(int iHMSampleY) const {return iHMSampleY%m_iHeightSamplesPerSector[1];}

  /// \brief
  ///   Returns the sector index for given tile index
  ///
  /// \param iTileX
  ///   height map sample index in x-direction
  /// \return
  ///   sector index (not clamped)
  inline int GetSectorIndexAtTilePosX(int iTileX) const {return iTileX/m_iTilesPerSector[0];}

  /// \brief
  ///   Returns the sector index for given tile index
  ///
  /// \param iTileY
  ///   height map sample index in y-direction
  /// \return
  ///   sector index (not clamped)
  inline int GetSectorIndexAtTilePosY(int iTileY) const {return iTileY/m_iTilesPerSector[1];}

  /// \brief
  ///   Helper function to clamp incoming sector indices to valid range
  ///
  /// \param x
  ///   sector index to clamp
  /// \param y
  ///   sector index to clamp
  inline void ClampSectorIndices(int &x,int &y) const
  {
    if (x<0) x=0; else if (x>=m_iSectorCount[0]) x=m_iSectorCount[0]-1;
    if (y<0) y=0; else if (y>=m_iSectorCount[1]) y=m_iSectorCount[1]-1;
  }

  /// \brief
  ///   Helper function to clamp an incoming sector rectangle to valid range.
  ///
  /// x1 can be larger than x2 afterwards if the rect is completely outside
  ///
  /// \param x1
  ///   sector index to clamp
  /// \param y1
  ///   sector index to clamp
  /// \param x2
  ///   sector index to clamp
  /// \param y2
  ///   sector index to clamp
  inline void ClampSectorRectangle(int &x1,int &y1,int &x2,int &y2) const
  {
    if (x1<0) x1=0;
    if (y1<0) y1=0;
    if (x2>=m_iSectorCount[0]) x2=m_iSectorCount[0]-1;
    if (y2>=m_iSectorCount[1]) y2=m_iSectorCount[1]-1;
  }

  /// \brief
  ///   Clamping version of GetSectorIndexAtSamplePosX/GetSectorIndexAtSamplePosY  
  ///
  /// \param iHMSampleX
  ///   height sample index
  /// \param iHMSampleY
  ///   height sample index
  /// \param x
  ///   target sector index (clamped)
  /// \param y
  ///   target sector index (clamped)
  inline void GetSectorIndices_Clamped(int iHMSampleX, int iHMSampleY, int &x,int &y) const
  {
    x = GetSectorIndexAtSamplePosX(iHMSampleX);
    if (x<0) x=0; else if (x>=m_iSectorCount[0]) x=m_iSectorCount[0]-1;
    y = GetSectorIndexAtSamplePosY(iHMSampleY);
    if (y<0) y=0; else if (y>=m_iSectorCount[1]) y=m_iSectorCount[1]-1;
  }

  /// \brief
  ///   Returns the pivot position of a sector  
  ///
  /// \param iIndexX
  ///   sector index
  /// \param iIndexY
  ///   sector index
  /// \return
  ///   world space position of sector
  inline hkvVec3 GetSectorOrigin(int iIndexX, int iIndexY) const {return hkvVec3 (m_vSectorSize.x*(float)iIndexX+m_vTerrainPos.x, m_vSectorSize.y*(float)iIndexY+m_vTerrainPos.y, m_vTerrainPos.z);}

  /// \brief
  ///   Calculates relative position inside a terrain (0..1 target range)
  ///
  /// \param vPos
  ///   world space position
  /// \param x
  ///   relative x value (0..1)
  /// \param y
  ///   relative y value (0..1)
  TERRAIN_IMPEXP void GetRelativeTerrainPos(const VLargePosition &vPos, float &x, float &y) const;

  /// \brief
  ///   Transforms world space position to sample indices
  ///
  /// \param vPos
  ///   world space position
  /// \param x
  ///   sample index
  /// \param y
  ///   sample index
  TERRAIN_IMPEXP void GetHeightSampleIndicesAtPos(const VLargePosition &vPos, int &x, int &y) const;

  /// \brief
  ///   Transforms world space position to tile indices
  ///
  /// \param vPos
  ///   world space position
  /// \param x
  ///   tile index
  /// \param y
  ///   tile index
  TERRAIN_IMPEXP void GetTileIndicesAtPos(const VLargePosition &vPos, int &x, int &y) const;

  /// \brief
  ///   Transforms world space position to default density map resolution indices
  ///
  /// \param vPos
  ///   world space position
  /// \param x
  ///   sample index
  /// \param y
  ///   sample index
  TERRAIN_IMPEXP void GetDensitySampleIndicesAtPos(const VLargePosition &vPos, int &x, int &y) const;

  /// \brief
  ///   Transforms world space position to default weightmap map resolution indices
  ///
  /// \param vPos
  ///   world space position
  /// \param x
  ///   sample index
  /// \param y
  ///   sample index
  TERRAIN_IMPEXP void GetWeightmapSampleIndicesAtPos(const VLargePosition &vPos, int &x, int &y) const;

  /// \brief
  ///   Computes absolute bounding box of the terrain
  ///
  /// \param bbox
  ///   absolute bounding box
  TERRAIN_IMPEXP void GetTerrainBBox(VLargeBoundingBox &bbox) const;

  /// \brief
  ///   Computes absolute default bounding box of a sector
  ///
  /// \param bbox
  ///   absolute bounding box
  /// \param x
  ///   sector index
  /// \param y
  ///   sector index
  TERRAIN_IMPEXP void GetSectorBBox(VLargeBoundingBox &bbox, int x, int y) const;

  /// \brief
  ///   Computes a bounding box around the passed camera position that represents the visible distance. This box is overestimated by m_fPrecacheMargin*SectorSize
  TERRAIN_IMPEXP void GetViewDistanceBox(hkvAlignedBBox &destbox, const hkvVec3& vCamPos, float fViewDistance) const;

  /// \brief
  ///   Computes absolute position of a height sample (also taking fractional part into account)
  ///
  /// \param fSampleIndexX
  ///   sample index
  /// \return
  ///   world space x coordinate of this sample
  inline float GetHeightmapSamplePosX(float fSampleIndexX) const {return m_vTerrainPos.x + fSampleIndexX*m_vSampleSpacing.x;}

  /// \brief
  ///   Computes absolute position of a height sample (also taking fractional part into account)
  ///
  /// \param fSampleIndexY
  ///   sample index
  /// \return
  ///   world space y coordinate of this sample
  inline float GetHeightmapSamplePosY(float fSampleIndexY) const {return m_vTerrainPos.y + fSampleIndexY*m_vSampleSpacing.y;}

  /// \brief
  ///   Computes a vector that can be used to transform from world space into the sector's space (0..1)
  ///
  /// \param iSectorX
  ///   index of target sector
  /// \param iSectorY
  ///   index of target sector
  /// \param dest
  ///   transformation vector: xy=scale, zw=ofs
  TERRAIN_IMPEXP void GetWorldSpaceToSectorTransform(int iSectorX, int iSectorY, hkvVec4& dest) const;

  /// \brief
  ///   Ensures relevant sub-directories are created. Requires m_sTerrainFolder to be setup
  ///
  /// \param szProjectDir
  ///   absolute path to project
  /// \param bTemp
  ///   determines whether the temp directory should be used
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool MakeRelevantDirectories(const char *szProjectDir, bool bTemp = false) const;

  /// \brief
  ///   Retrieves the project local directory where terrain files go
  ///
  /// \param szPath
  ///   target string
  /// \param szFolder
  ///   name of subfolder
  /// \param bTemp
  ///   determines whether the temp directory should be used
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool GetDirectory(char *szPath, const char *szFolder, bool bTemp) const;

  /// \brief
  ///   Retrieves the project local directory where terrain files go
  ///
  /// \param szDir
  ///   target string
  /// \param eDirType
  ///   Directory type. See VTC_DIR_xyz or VTC_FILE_xyz
  /// \param bTemp
  ///   determines whether the temp directory should be used
  ///
  /// \return
  ///   true if successful  
  TERRAIN_IMPEXP bool GetDirectory(char *szDir, int eDirType, bool bTemp) const;

  //TODO: Deprecate
  TERRAIN_IMPEXP bool GetSectorCacheDir(char *szPath, bool bTempDir) const
  {    return GetDirectory(szPath, VTC_DIR_SECTORS, m_bUseTempFolder & bTempDir);  }
  TERRAIN_IMPEXP bool GetAuxiliaryTextureDir(char *szPath, bool bTempDir) const
  {    return GetDirectory(szPath, VTC_DIR_TEXTURE, m_bUseTempFolder & bTempDir);  }
  TERRAIN_IMPEXP bool GetEditingDir(char *szPath, bool bTempDir) const
  {    return GetDirectory(szPath, VTC_DIR_EDITING, m_bUseTempFolder & bTempDir);  }  

  //Returns only the filename
  TERRAIN_IMPEXP bool GetFilename(char *szFilename, int iSectorX, int iSectorY, int iFileType) const;

  // Returns the sector file's path. Doesn't return temp folder
  TERRAIN_IMPEXP bool GetSectorFile(char *szPath, int iSectorX, int iSectorY, int iFileType) const;

  // Returns the sector filename
  TERRAIN_IMPEXP bool GetSectorFilename(char *szPath, int iSectorX, int iSectorY, const char *szExt, bool bTemp = false) const;

  // Returns the moved sector filename
  TERRAIN_IMPEXP bool GetMovedSectorFilename(char *szPath, int iSectorX, int iSectorY, const char *szExt, bool bTemp ) const;

  /// \brief
  ///   Builds a full path name of a sector specific file. Returns the optional temp directory if it exists or if you're saving
  ///
  /// \param szPath
  ///   target string
  /// \param iSectorX
  ///   sector index
  /// \param iSectorY
  ///   sector index
  /// \param szExt
  ///   file extension
  /// \param bForSaving
  ///   true if the file is to be saved
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool GetSectorCacheFilename(char *szPath, int iSectorX, int iSectorY, const char *szExt, bool bForSaving) const;

  /// \brief
  ///   Builds a full path name for an auxiliary texture
  ///
  /// \param szPath
  ///   target string
  /// \param iSectorX
  ///   sector index
  /// \param iSectorY
  ///   sector index
  /// \param szNameBase
  ///   base filename
  /// \param bForSaving
  ///   true if the file is to be saved
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool GetSectorAuxiliaryTextureFilename(char *szPath, int iSectorX, int iSectorY, const char *szNameBase, bool bForSaving) const;

  /// \brief
  ///   Builds a full path name for a sector's normalmap texture
  ///
  /// \param szPath
  ///   target string
  /// \param iSectorX
  ///   sector index
  /// \param iSectorY
  ///   sector index
  /// \param bForSaving
  ///   true if the file is to be saved
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool GetSectorNormalmapFilename(char *szPath, int iSectorX, int iSectorY, bool bForSaving) const
  {
    return GetSectorAuxiliaryTextureFilename(szPath,iSectorX,iSectorY,"Normalmap",bForSaving);
  }
  
  /// \brief
  ///   Builds a full path name for a sector's diffuse replacement texture
  ///
  /// \param szPath
  ///   target string
  /// \param iSectorX
  ///   sector index
  /// \param iSectorY
  ///   sector index
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool GetSectorDiffuseReplacementFilename(char *szPath, int iSectorX, int iSectorY) const
  {
    char szDir[FS_MAX_PATH];
    char szExtName[FS_MAX_PATH];

    if (!GetAuxiliaryTextureDir(szDir, false))
      return false;
    
    sprintf(szExtName, "%s_%02i_%02i.dds", "DiffuseReplacement", iSectorX, iSectorY);
    VFileHelper::CombineDirAndFile(szPath, szDir, szExtName);

    return true;
  }

  /// \brief
  ///   Builds a full path name for a sector's editing data file
  ///
  /// \param szPath
  ///   target string
  /// \param iSectorX
  ///   sector index
  /// \param iSectorY
  ///   sector index
  /// \param bForSaving
  ///   true if the file is to be saved
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool GetSectorEditingFilename(char *szPath, int iSectorX, int iSectorY, bool bForSaving) const;

  /// \brief
  ///   Builds a full path name for a sector's mesh data file (used for terrain mesh export)
  ///
  /// \param szPath
  ///   target string
  /// \param iSectorX
  ///   sector index
  /// \param iSectorY
  ///   sector index
  /// \param bForSaving
  ///   true if the file is to be saved
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool GetSectorMeshFilename(char *szPath, int iSectorX, int iSectorY, bool bForSaving) const;

  /// \brief
  ///   Builds a full path name for a sector's collision mesh data file (used for terrain mesh export)
  ///
  /// \param szPath
  ///   target string
  /// \param iSectorX
  ///   sector index
  /// \param iSectorY
  ///   sector index
  /// \param bForSaving
  ///   true if the file is to be saved
  ///
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool GetSectorCollisionFilename(char *szPath, int iSectorX, int iSectorY, bool bForSaving) const;

  /// \brief
  ///   Indicates whether the passed filename is located in the temporary directory
  TERRAIN_IMPEXP bool IsTempDirFile(const char *szFilename) const;

  /// \brief
  ///   Converts a temp dir file into the neighbor retail directory name. Handles other filenames gracefully 
  TERRAIN_IMPEXP const char* FixTempDirFile(const char *szFilename, char *szBuffer) const;

  /// \brief
  ///   Saves or load config from chunk file
  ///
  /// \param file
  ///   chunk file
  TERRAIN_IMPEXP void ChunkFileSerialize(VChunkFile &file);

  /// \brief
  ///   IO helper function
  ///
  /// \internal
  /// \param szProjRelpath
  ///   do not use
  /// \return
  ///   true if successful
  TERRAIN_IMPEXP bool RecursiveCreateDir(const char *szProjRelpath) const;

  //Perform a RCS operation on terrain files
  //-For use inside vForge
  //-Accepts absolute or project relative filenames
  //-Checks that we don't accidentally use temp files
  TERRAIN_IMPEXP int RCSPerformAction(const char *szFilename, int eAction) const;

  /// \brief
  ///   Copy assignment operator - copies all properties
  ///
  /// \param other
  ///   source config
  /// \return
  ///   reference to this
  inline VTerrainConfig& operator = (const VTerrainConfig& other)
  {
    COPY_MEMBER(m_sTerrainFolder);
    COPY_MEMBER(m_sAbsTerrainDir);
    COPY_MEMBER(m_sAbsProjectDir);

    COPY_MEMBER(m_iSectorCount[0]);
    COPY_MEMBER(m_iSectorCount[1]);
    COPY_MEMBER(m_vSectorSize);
    COPY_MEMBER(m_iHeightSamplesPerSector[0]);
    COPY_MEMBER(m_iHeightSamplesPerSector[1]);
    COPY_MEMBER(m_iTilesPerSector[0]);
    COPY_MEMBER(m_iTilesPerSector[1]);
    COPY_MEMBER(m_iDensityMapSamplesPerSector[0]);
    COPY_MEMBER(m_iDensityMapSamplesPerSector[1]);
    COPY_MEMBER(m_iDefaultWeightmapResolution[0]);
    COPY_MEMBER(m_iDefaultWeightmapResolution[1]);
    COPY_MEMBER(m_iMaterialMapResolution[0]);
    COPY_MEMBER(m_iMaterialMapResolution[1]);
    COPY_MEMBER(m_vTerrainPos);
    COPY_MEMBER(m_fPurgeInterval);
    COPY_MEMBER(m_iMemoryLimit);
    COPY_MEMBER(m_fVisibilityHeightOverTerrain);
    COPY_MEMBER(m_bSortTextureChannels);
    COPY_MEMBER(m_bDistanceBasedLODOnly);
    COPY_MEMBER(m_bIgnoreHeightForLOD);
    COPY_MEMBER(m_bNormalmapUsesBorder);
    COPY_MEMBER(m_bSceneSpecificLightmaps);
    COPY_MEMBER(m_bUseThreeWayMapping);
    COPY_MEMBER(m_bUseNormalMapping);
    COPY_MEMBER(m_bUseLightMapping);

    COPY_MEMBER(m_fPrecacheMargin);
    COPY_MEMBER(m_iOldSectorStartIndex[0]);
    COPY_MEMBER(m_iOldSectorStartIndex[1]);
    COPY_MEMBER(m_iNewSectorStartIndex[0]);
    COPY_MEMBER(m_iNewSectorStartIndex[1]);
    COPY_MEMBER(m_iOldSectorCount[0]);
    COPY_MEMBER(m_iOldSectorCount[1]);

    Finalize();
    return *this;
  }

  ///
  /// @}
  ///

public:

  ///
  /// @name Members that are implicitly defined
  /// @{
  ///

  float m_fViewDistance;                ///< maximum view distance, updated per frame
  int m_iSectorMeshesPerSector[2];      ///< samples per sector / 128 (ideal mesh size)
  int m_iSectorMeshVertexCount[2];      ///< 128 (ideal mesh size)
  int m_iSectorMeshCount,m_iVerticesPerMesh;
  int m_iHeightSamplesPerTile[2];       ///< number of height map samples per tile, m_iHeightSamplesPerSector/m_iTilesPerSector
  int m_iOverallHeightSampleCount[2];   ///< overall number of heightsamples in both directions
  int m_iOverallMeshPageCount[2];       ///< overall number of pages in both directions
  int m_iSamplesPerReplacementMesh[2];  ///< number of heightmap samples per low res replacement mesh
  int m_iTilesPerSectorMesh[2];         ///< number of tile subdivisions for each sector mesh
  int m_iTilesPerSectorCount;           ///< m_iTilesPerSector[0] * m_iTilesPerSector[1]
  hkvVec2 m_vInvSectorSize;             ///< inverse of terrain sector size
  hkvVec2 m_vWorld2Sample;              ///< scales a world position to height sample index inside a sector
  hkvVec2 m_vTileSize;                  ///< size of a single tile (sectorsize/tiles per sector)
  hkvVec2 m_vWorld2Tile;                ///< scales a world position to tile index inside a sector (1.0/m_vTileSize)
  hkvVec2 m_vWorld2DensityMap;          ///< scales a world position to sample index of the density map
  hkvVec2 m_vSampleSpacing;             ///< x-y spacing between two height map samples
  hkvVec2 m_vDensitySampleSpacing;      ///< x-y spacing between two samples on the density map
  hkvVec2 m_vNormalGradientSpacing;     ///< recommended x-y spacing for making the cross product
  int m_iMaxMeshLOD;                    ///< the usable max (<= MAX_MESH_LOD)
  int m_iMaxMeshLODHoles;               ///< the maximum LOD for sectors that have holes
  bool m_bFinalized;                    ///< future use
  bool m_bSupportsReplacement;          ///< future use
  ///
  /// @}
  ///
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
