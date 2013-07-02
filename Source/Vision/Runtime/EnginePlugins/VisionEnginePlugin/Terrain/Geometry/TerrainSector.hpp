/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainSector.hpp

#ifndef TERRAINSECTOR_HPP_INCLUDED
#define TERRAINSECTOR_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/Base/System/IO/System/FileLock.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/SectorTile.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Math/LargePosition.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/SectorVisibilityZone.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorMesh.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/LuminanceChannel.hpp>


class VTerrainLockObject;
class VTerrainSector;
class VTerrainVisibilityInfo;
typedef VSmartPtr<VTerrainSector> VTerrainSectorPtr;
class VTerrainSectorManager;
class VSectorCollisionMeshes;
typedef VSmartPtr<VSectorCollisionMeshes> VSectorCollisionMeshesPtr;
class VTerrainVisibilityCollectorComponent;


#define WORLDX_2_TILE(_x) (int)(((_x)-m_vSectorOrigin.x)*m_Config.m_vWorld2Tile.x)
#define WORLDY_2_TILE(_y) (int)(((_y)-m_vSectorOrigin.y)*m_Config.m_vWorld2Tile.y)

/// \brief
///   Internal class for streaming a terrain sector snapshot
class VSectorResourceSnapshot : public VResourceSnapshot
{
public:
  VSmartPtr<VLoadingTask> m_spResFileLoadingTask;
};

#define SECTOR_FILE_VERSION_8       8
#define SECTOR_FILE_VERSION_9       9
#define SECTOR_FILE_VERSION_10      10
#define SECTOR_FILE_VERSION_11      11
// version 14: decoration with 32bit references 8.1 
#define SECTOR_FILE_VERSION_14      14
// version 15: material ID map 8.1 
#define SECTOR_FILE_VERSION_15      15
// version 16: three way mapping
#define SECTOR_FILE_VERSION_16      16
// version 17: decoration SerializeX change
#define SECTOR_FILE_VERSION_17      17
// version 18: exposed material IDs
#define SECTOR_FILE_VERSION_18      18
// version 19: save texture flags
#define SECTOR_FILE_VERSION_19      19
// version 20: physics type
#define SECTOR_FILE_VERSION_20      20
// version 21: texture limits per page instead of per sector
#define SECTOR_FILE_VERSION_21      21
// version 21: added decoration version number
#define SECTOR_FILE_VERSION_22      22

#define SECTOR_CURRENT_FILE_VERSION SECTOR_FILE_VERSION_22

#define SECTOR_TEXTURETYPE_NULL  0
#define SECTOR_TEXTURETYPE_WHITE 1
#define SECTOR_TEXTURETYPE_BLACK 2
#define SECTOR_TEXTURETYPE_FILE  3
#define SECTOR_TEXTURETYPE_NEUTRALNORMAL  4



/// \brief
///   Represents a large sector inside the terrain. It keeps its own copy of the relevant heightmap data, decoration objects etc.
///
/// A sector can be streamed in as it represented as separate physical files on disk.
/// A sector is derived from VisStaticGeometryInstance_cl so it represents a renderable mesh in the engine (e.g. in the visibility pipeline).
/// However, for rendering it uses a finer grained approach: A sector that has more than 128x128 height samples is subdivided into multiple draw calls
/// of 128x128 each. This draw call is represented by class VTerrainSectorMeshPageInfo. For instance, if the terrain has been created with 512x512 height samples
/// per sector, a sector holds an array of 4x4 instances of VTerrainSectorMeshPageInfo that can be rendered independently. LOD is also computed on
/// VTerrainSectorMeshPageInfo-basis
class VTerrainSector : public VManagedResource, public VisStaticGeometryInstance_cl
{
public:

  /// 
  /// \brief
  ///   Enumeration of physics representations for terrain sectors.
  ///
  enum VPhysicsType_e
  {
    VPHYSICSTYPE_APPROXIMATE = 0,              ///< approximated/ fast physics representation (can't handle holes)
    VPHYSICSTYPE_PRECISE = 1,                  ///< precise/ slower physics representation
    VPHYSICSTYPE_PRECISE_OFFLINE_WELDING = 2   ///< precise/ slower physics representation with contact point welding (required information created offline, increased memory footprint)
  };

  TERRAIN_IMPEXP VTerrainSector(VTerrainSectorManager *pManager, const VTerrainConfig &config, int iIndexX, int iIndexY);
  TERRAIN_IMPEXP ~VTerrainSector();

  /// \brief
  ///   Returns the owner sector manager (which is part of the terrain instance)
  inline VTerrainSectorManager* GetSectorManager() const {return (VTerrainSectorManager *)GetParentManager();}

  // lookup functions
  inline int GetHeightmapSampleCount() const {return m_iSampleStrideX*(m_Config.m_iHeightSamplesPerSector[1]+2);}

  /// \brief
  ///   Returns a raw array of heightmap values. Ensures that the heightmap is loaded for this sector
  inline float *GetHeightmapValues() {if (!m_pHeight) LoadHeightmap(); return m_pHeight;}

  /// \brief
  ///   Returns the non-interpolated height value at specified sector-relative sample position. Ensures that the heightmap is loaded
  inline float *GetHeightmapValuesAt(int x, int y) 
  {
    float *fVal = GetHeightmapValues();
    VASSERT(x>=0 && x<=m_Config.m_iHeightSamplesPerSector[0]+1); ///< also allow overlapping part
    VASSERT(y>=0 && y<=m_Config.m_iHeightSamplesPerSector[1]+1);
    return &fVal[y*m_iSampleStrideX+x];
  }

  /// \brief
  ///   Same as GetHeightmapValuesAt but assumes that the sector is loaded (must be tested outside)
  inline float GetHeightAt(int x, int y) const
  {
    VASSERT(m_pHeight != NULL); ///< make sure GetHeightmapValues() has been called
    VASSERT(x>=0 && x<=m_Config.m_iHeightSamplesPerSector[0]+1); ///< also allow overlapping part
    VASSERT(y>=0 && y<=m_Config.m_iHeightSamplesPerSector[1]+1);
    return m_pHeight[y*m_iSampleStrideX+x];
  }
  /// \brief
  ///   Returns interpolated height value at sample position that is a world position made relative to the sector's origin
  TERRAIN_IMPEXP float GetHeightAtRelPos(const hkvVec3& vPos) const;

  /// \brief
  ///   Returns the material ID at specified relative sector position
  TERRAIN_IMPEXP int GetMaterialID(float fRelPosX, float fRelPosY);

  TERRAIN_IMPEXP void PerformVisibility(VTerrainVisibilityCollectorComponent &infoComp);

  /// \brief
  ///   Returns the world space position of the sector's origin
  inline const hkvVec3& GetSectorOrigin() const {return m_vSectorOrigin;}

  /// \brief
  ///   Get sector physics representation type
  inline const VPhysicsType_e GetPhysicsType() const { return m_ePhysicsType; }

  /// \brief
  ///   Set sector physics representation type
  inline void SetPhysicsType(VPhysicsType_e ePhysicsType) { m_ePhysicsType = ePhysicsType; }

  /// \brief
  ///   Returns the default bounding box of the sector (z-extents might not be accurate)
  TERRAIN_IMPEXP void GetDefaultSectorBoundingBox(hkvAlignedBBox &destBox) const;
  TERRAIN_IMPEXP void SetSectorBoundingBox(const hkvAlignedBBox &box, bool bInflateByTiles=false);
  /// \brief
  ///   Returns the bounding box of the visibility zone that is associated with this sector
  TERRAIN_IMPEXP const hkvAlignedBBox& GetVisZoneBoundingBox() const;

  /// \brief
  ///  Accesses the mesh page for rendering. If the sector has only 128x128 samples then there is only one mesh page per sector
  inline VTerrainSectorMeshPageInfo *GetMeshPageInfo(int x, int y) const 
  {
    VASSERT(x>=0 && y>=0 && x<m_Config.m_iSectorMeshesPerSector[0] && y<m_Config.m_iSectorMeshesPerSector[1]);
    if (m_pMeshPage)
      return &m_pMeshPage[y*m_Config.m_iSectorMeshesPerSector[0]+x];
    return NULL;
  }

  /// \brief
  ///  Returns an index that represents the page with the passed relative indices inside this sector. This global index is used for visibility computation.
  inline int GetGlobalPageIndex(int px, int py)
  {
    int xx = m_iIndexX*m_Config.m_iSectorMeshesPerSector[0]+px;
    int yy = m_iIndexY*m_Config.m_iSectorMeshesPerSector[1]+py;
    return yy*m_Config.m_iOverallMeshPageCount[0] + xx;
  }

  TERRAIN_IMPEXP void UpdateHoleMask();

  // tile lookup functions

  /// \brief
  ///  Maps a global world position to VSectorTile index inside this sector
  inline int GetTileIndexX(const VLargePosition &vPos) const 
  {
    return (int)(vPos.m_vSectorOfs.x*m_Config.m_vWorld2Tile.x) + m_Config.m_iTilesPerSector[0]*(vPos.m_iSectorX-m_iIndexX);
  }
  /// \brief
  ///  Maps a global world position to VSectorTile index inside this sector
  inline int GetTileIndexY(const VLargePosition &vPos) const 
  {
    return (int)(vPos.m_vSectorOfs.y*m_Config.m_vWorld2Tile.y) + m_Config.m_iTilesPerSector[1]*(vPos.m_iSectorY-m_iIndexY);
  }
  /// \brief
  ///  Maps a global world position to VSectorTile index inside this sector. Clamps to valid tile range
  inline void GetTileIndices_Clamped(const VLargePosition &vPos, int &x, int &y) const 
  {
    x = GetTileIndexX(vPos);
    if (x<0) x=0; else if (x>=m_Config.m_iTilesPerSector[0]) x=m_Config.m_iTilesPerSector[0]-1;
    y = GetTileIndexY(vPos);
    if (y<0) y=0; else if (y>=m_Config.m_iTilesPerSector[1]) y=m_Config.m_iTilesPerSector[1]-1;
  }
  /// \brief
  ///  Accesses tile by tile indices inside this sector. Indices must be in valid range
  inline VSectorTile *GetTile(int x, int y) const
  {
    VASSERT(x>=0 && x<m_Config.m_iTilesPerSector[0]);
    VASSERT(y>=0 && y<m_Config.m_iTilesPerSector[1]);
    return &m_pTile[y*m_Config.m_iTilesPerSector[0]+x];
  }
  /// \brief
  ///  Accesses tile by height sample indices inside this sector. Indices must be in valid range
  inline VSectorTile *GetTileAtSampleIndices(int x,int y) const
  {
    x/=m_Config.m_iHeightSamplesPerTile[0];
    y/=m_Config.m_iHeightSamplesPerTile[1];
    // the height samples must be in range, but clamp at max edge because of extra overlapping values
    return GetTile(hkvMath::Min(x,m_Config.m_iTilesPerSector[0]-1),hkvMath::Min(y,m_Config.m_iTilesPerSector[1]-1));
  }

  /// \brief
  ///  Returns the default bounding box of the indexed tile. z extent might not be accurate
  TERRAIN_IMPEXP void GetDefaultTileBoundingBox(hkvAlignedBBox &destBox, int iTileX, int iTileY) const;

  /// \brief
  ///  Evaluates whether this sector has any holes. The terrain sector must be loaded when calling this function
  TERRAIN_IMPEXP bool HasHoles() const;

  /// \brief
  ///  Evaluates whether all tiles of this sectors are holes. The terrain sector must be loaded when calling this function
  TERRAIN_IMPEXP bool IsAllHoles() const;

  /// \brief
  ///  Generic mapping helper function
  inline void GetTexelIndices(const VLargePosition &vPos, int &x, int &y, int iSizeX, int iSizeY)
  {
    x = (vPos.m_iSectorX - m_iIndexX) * iSizeX;
    y = (vPos.m_iSectorY - m_iIndexY) * iSizeY;
    x += (int)(vPos.m_vSectorOfs.x * m_Config.m_vInvSectorSize.x * (float)iSizeX);
    y += (int)(vPos.m_vSectorOfs.y * m_Config.m_vInvSectorSize.y * (float)iSizeY);
  }

  /// \brief
  ///  Accesses the collection of decoration instances in this sector
  VTerrainDecorationInstanceCollection &Decoration() {return m_Decoration;}

  /// \brief
	///   Selectively removes the decoration that is marked in the passed bit field. The bits in the passed bit field correspond to indices from the sector's Decoration collection
  TERRAIN_IMPEXP void RemoveDecorationInstances(const VTBitfield<1024> &removeIndices);

  /// \brief
  ///   Removes decoration that is inside the passed bounding box (center of the decoration instance). See VTerrain::RemoveDecorationInstances
  TERRAIN_IMPEXP void RemoveDecorationInstances(const VLargeBoundingBox &bbox);

  /// \brief
  ///  Only used while editing
  TERRAIN_IMPEXP virtual VTerrainDecorationInstance* GetAdditionalDecoration(int &iCount) {iCount=0;return NULL;}

  TERRAIN_IMPEXP virtual void OnDestroying();

  // resource functions
  TERRAIN_IMPEXP virtual BOOL Reload();
  TERRAIN_IMPEXP virtual BOOL Unload();
#if ( defined(_VR_DX9) && defined(WIN32) )
  TERRAIN_IMPEXP virtual void OnEnterBackground();
  TERRAIN_IMPEXP virtual void OnLeaveBackground();
#endif
  virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);

  TERRAIN_IMPEXP float* AllocateHeightMap();
  TERRAIN_IMPEXP void LoadHeightmap();
  TERRAIN_IMPEXP bool SaveHeightmap();
  TERRAIN_IMPEXP bool LoadSectorInformation();
  TERRAIN_IMPEXP bool SaveSectorInformation();
  void DisposePerSectorObjects();

  inline void AddPerSectorObject(VisTypedEngineObject_cl *pObj)
  {
    VASSERT(pObj);
    m_PerSectorObjects[m_iPerSectorObjectCount++] = pObj;
  }
  TERRAIN_IMPEXP void RemovePerSectorObject(VisTypedEngineObject_cl *pObj);


  TERRAIN_IMPEXP virtual bool SaveSector(bool bUseTemp=false);
  TERRAIN_IMPEXP virtual void PrepareSector() 
  {
    m_bPrepared=true;
    EnsureLoaded();
  }
  TERRAIN_IMPEXP virtual bool DeleteSectorData(bool bUseTemp=false);
  TERRAIN_IMPEXP bool EnsureReplacementMeshLoaded();

  TERRAIN_IMPEXP virtual void OnPositionChanged(const hkvVec3& vOldPos,const hkvVec3& vNewPos);
  //TERRAIN_IMPEXP void UpdateShaderConstants();
  TERRAIN_IMPEXP virtual void OnConfigChanged();
  TERRAIN_IMPEXP virtual void OnDefaultShaderEffectChanged();

  /// \brief
  ///  This function is called when the mesh should be refreshed
  TERRAIN_IMPEXP void UpdateMesh(bool bForceAllPages);

  /// \brief
  ///  Accesses the collision mesh portion that is associated with the tile
  TERRAIN_IMPEXP VSimpleCollisionMesh* GetCollisionMesh(int iTileX, int iTileY);
  /// \brief
  ///  Fills a list with relevant collision mehes
  TERRAIN_IMPEXP int GetRelevantCollisionMeshesInBoundingBox(const VLargeBoundingBox &bbox, DynArray_cl<VSimpleCollisionMesh *> &meshes, int iFirst=0);

  inline bool IsPreCaching() const {return this->m_pSnapshot!=NULL;}
  TERRAIN_IMPEXP void PreCache();
#ifdef SUPPORTS_SNAPSHOT_CREATION
  TERRAIN_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot);
#endif
  // physics:
  inline void SetPhysicsUserData(void *pData) {m_PhysicsUserData=pData;}
  inline void* GetPhysicsUserData() const {return m_PhysicsUserData;}

  // debugging
  void DebugRender();

  // VisStaticGeometryInstance_cl implementation:
  TERRAIN_IMPEXP virtual void ComputeSortingKey() HKV_OVERRIDE
  {
    /*PSVita compiler doesn't like 'TERR', so this is done by hand */ 
    m_iSortingKey= (((int) 'T') << 24) | (((int) 'E') << 16) | (((int) 'R') << 8) | 'R';
  }

  /// \brief
  ///  Overridden render function. Renders the sector meshes but not the decoration
  TERRAIN_IMPEXP virtual void Render(VCompiledShaderPass *pShader, const VisStaticGeometryInstanceCollection_cl &allInstances, int iThisIndex, VPassType_e ePassType) HKV_OVERRIDE;
  /// \brief
  ///  Overridden function to perform a traceline test on the terrain
  TERRAIN_IMPEXP virtual int TraceTest(const hkvVec3& startOfLine, const hkvVec3& endOfLine, int iStoreResultCount=0, VisTraceLineInfo_t *pFirstTraceInfo=NULL);
  TERRAIN_IMPEXP virtual int GetNumPrimitives();
  TERRAIN_IMPEXP virtual int GetSupportedStreamMask();

  //Used by final save to save edited or swapped sectors over final sectors and to genenerate the resource snapshot
  TERRAIN_IMPEXP virtual bool SaveSectorFinal(bool bSaveSnapshot=false);

  /// \brief
  ///  Function VTerrainSector::Render calls this function to render a single page. This function is overloaded by editable terrain sectors
  TERRAIN_IMPEXP virtual void RenderTerrainMesh(VTerrainSectorMeshPageInfo* pPage, const VTerrainVisibilityCollectorComponent &visInfoComp);
  TERRAIN_IMPEXP float ComputeMaxErrorForLOD(int iLod, int x1,int y1,int x2,int y2);
  TERRAIN_IMPEXP void ComputeLODDistanceTable();
  TERRAIN_IMPEXP virtual VisZoneResource_cl *GetParentZone(); 

  TERRAIN_IMPEXP void AssignVisibility(bool bStatus);
  void ConnectNeighborZones(bool bStatus);

  TERRAIN_IMPEXP static float Noise2D(int x, int y);

  TERRAIN_IMPEXP VCompiledTechnique* GetReplacementTechnique();

private:
  // As each page has its own surface, this function is overridden to prevent accidentally using the sector's surface (which is just an unused dummy).
  // Since this is a non-virtual override, the actual surface can still be accessed by casting to the base class.
  VisSurface_cl* GetSurface() const
  {
    VASSERT_MSG(false, "The terrain sector's surface shouldn't be used - did you mean to use the surface of one of the sector's mesh pages instead?");
    return NULL;
  }

public:
  const VTerrainConfig &m_Config;     ///< point to terrain's config
  VSectorVisibilityZonePtr m_spSectorZone; ///< the visibility zone

  // sector properties
  hkvVec3 m_vSectorOrigin;          ///< not necessarily boundingbox.min
  int m_iIndexX, m_iIndexY;         ///< sector index in the global terrain
  int m_iSampleStrideX;             ///< 2 additional overlapping height values (m_Config.m_iHeightSamplesPerSector+2)
  float *m_pHeight;                 ///< (m_iSampleCount[0]+2)*(m_iSampleCount[1]+2)
  float m_fMinHeightValue;          ///< the minimum value in m_pHeight array
  float m_fMaxHeightValue;          ///< the maximum value in m_pHeight array
  float m_fMaxTileHandlingDistance; ///< maximum of m_fMaxDecorationFarClip over all tiles
  VPhysicsType_e m_ePhysicsType;    ///< physics representation type

  // sector mesh
  VTerrainSectorMeshBufferPtr m_spMesh;
  VTerrainSectorMeshPageInfo *m_pMeshPage;
  VisMeshBufferPtr m_spReplacementMesh;

  // tile subdivision
  VSectorTile *m_pTile;             ///< array of single tiles
  VSectorCollisionMeshesPtr m_spCollisionMeshes;

  // precaching:
  VSectorResourceSnapshot *m_pSnapshot;

  // decoration
  VTerrainDecorationInstanceCollection m_Decoration;    ///< the decoration instances itself
  int m_iReferencedDecorationCount;
  int *m_piReferencedDecoration; ///< shared by all tiles
  VTerrainSectorDecorationVisibilityMask *m_pFirstDecoVisInfo; ///< first entry of linked list to store collector specific decoration visibility
  int m_iPerSectorObjectCount;
  DynArray_cl<VisTypedEngineObject_cl *> m_PerSectorObjects;

  // meta data:
  VLuminanceChannelPtr m_spMaterialIDMap;

  // flags:
  bool m_bPrepared;  ///< loaded and updated
  bool m_bSectorFileLoaded;
  bool m_bHasAdditionalDecoration; ///< only relevant inside editor
  bool m_bFailedLoadingReplacementMesh;

  // shader:
  VCompiledTechniquePtr m_spTerrainReplacementTechnique;

  // physics
  void *m_PhysicsUserData; ///< store implementation specific data here
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
