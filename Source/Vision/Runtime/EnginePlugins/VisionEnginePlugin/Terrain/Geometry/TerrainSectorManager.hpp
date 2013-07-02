/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainSectorManager.hpp

#ifndef TERRAINSECTORMANAGER_HPP_INCLUDED
#define TERRAINSECTORMANAGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainCollisionMesh.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Math/LargePosition.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainVisibilityCollector.hpp>
#include <Vision/Runtime/Base/Container/VElementCache.hpp>

class VTerrainDecorationInstance;
class VTerrainSectorManager;
class VTerrainSector;
class VSectorTile;
class VTerrain;
class VTerrainLockObject;
class VTerrainSectorMeshPageInfo;
typedef VSmartPtr<VTerrainSector> VTerrainSectorPtr;

// two footstep constraints - if this changes, it must also be changed in the shaders
#define MAX_SPHERE_CONSTRAINTS  2


/// \brief
///   Implements a small cache of reusable surface instances
/// \internal
class VTerrainSurfaceCache : public IVElementCache<VisSurface_cl *, 64>
{
protected:
  virtual void DeleteObject(VisSurface_cl *pElem) 
  {
    delete pElem;
  }
  virtual VisSurface_cl* AllocateObject() 
  {
    return new VisSurface_cl();
  }

  virtual void OnAddedToCache(VisSurface_cl *pElem)
  {
    // give up texture and effect references
    pElem->ClearReferences();
  }
};

  
#if defined(_VR_DX11)
/// \brief
///   Structure that holds DX10 specific shader registers
/// \internal
struct VTerrainConstantBuffer_Globals
{
  hkvVec4 m_vSphereConstraint[MAX_SPHERE_CONSTRAINTS];
  hkvVec4 m_vWorldSpace2Sector; ///< changes for every sector
};

typedef VTypedConstantBuffer<VTerrainConstantBuffer_Globals> VTerrainConstantBufferGlobals;

// slots use
#define VTERRAIN_CB_GLOBALS    5
#define VTERRAIN_CB_DECOMODEL  6

#endif


/// \brief
///   Helper structure to track directional light relevant shader parameter ("TerrainDirectional" shader)
/// \internal
struct VTerrainDirectionalLightConfig
{
  hkvVec4 m_vLightDirection;
  hkvVec4 m_vDirectionalLightColor;
  hkvVec4 m_vAmbientColor;
};

typedef VTypedConstantBuffer<VTerrainDirectionalLightConfig> VTerrainConstantBufferDirectionalLightConfig;

/// \brief
///   Helper structure that contains information about LOD index range
/// \internal
struct VSectorMeshRenderRange_t
{
  VSectorMeshRenderRange_t() {m_iFirstPrim=m_iPrimCount=0;}
  VSectorMeshRenderRange_t operator = (const VSectorMeshRenderRange_t& other)
  {
    m_iFirstPrim = other.m_iFirstPrim;
    m_iPrimCount = other.m_iPrimCount;
    return *this;
  }
  inline void SetRange(int iFirstPrim, int iCount) {m_iFirstPrim=iFirstPrim;m_iPrimCount=iCount;}
  inline void SerializeX(VArchive &ar, int iLocalVersion)
  {
    if (ar.IsLoading())
      ar >> m_iFirstPrim >> m_iPrimCount;
    else
      ar << m_iFirstPrim << m_iPrimCount;
  }

  int m_iFirstPrim, m_iPrimCount;
};

/// \brief
///   Helper structure that holds all edge permutations of VSectorMeshRenderRange_t
/// \internal
struct VSectorMeshLODLevelInfo_t
{
  inline void SerializeX(VArchive &ar, int iLocalVersion)
  {
    for (int a=0;a<2;a++)
      for (int b=0;b<2;b++)
        for (int c=0;c<2;c++)
          for (int d=0;d<2;d++)
            m_EdgeInfo[a][b][c][d].SerializeX(ar,iLocalVersion);
  }

  // indexing order: top,right,bottom,left
  VSectorMeshRenderRange_t m_EdgeInfo[2][2][2][2];
};

//LOD constants
const char PAGE_LOD_NOTVISIBLE        = (char)-1;
const char PAGE_LOD_MASK              = 0x0f;
const char PAGE_LOD_CONSTRAINED       = 0x10;
const char PAGE_LOD_FORCEFIXED        = 0x20;



/// \brief
///   Represents the terrain, i.e. keeps a grid of n x n sector pointers that are loaded/unloaded
///   dynamically. A VTerrain has one instance of this manager.
class VTerrainSectorManager : public VisResourceManager_cl
{
public:
  TERRAIN_IMPEXP VTerrainSectorManager(VTerrainConfig &config);
  TERRAIN_IMPEXP virtual ~VTerrainSectorManager();

  // Init/deinit
  TERRAIN_IMPEXP void InitTerrain();
  TERRAIN_IMPEXP void DeInitTerrain();
  TERRAIN_IMPEXP static int CreateLODIndexBuffer(const VTerrainConfig &cfg, int iLOD,  int iUp, int iRight, int iBottom, int iLeft, DynArray_cl<unsigned short> &pDestBuffer, int iStartIndex, const bool* pTileHoleMask=NULL);
  TERRAIN_IMPEXP void EnsureSharedMeshesCreated();
  TERRAIN_IMPEXP void SetAllowPurging(bool bStatus);

  ///\brief
	///Returns the terrain sector with specified indices. Indices must be in valid range
	///
	///\param x
  ///Sector index (must be 0..sectorcountX-1)
	///
	///\param y
  ///Sector index (must be 0..sectorcountY-1)
	///
	///\returns
	///The sector instance. This is always !=NULL but sector might not be loaded
	///
	inline VTerrainSector *GetSector(int x,int y) const
  {
    VASSERT(m_pSector && x>=0 && x<m_Config.m_iSectorCount[0] && y>=0 && y<m_Config.m_iSectorCount[1]);
    return m_pSector[y*m_Config.m_iSectorCount[0]+x];
  }

  ///\brief
	///Returns a sector's sub-tile using global indexing
  TERRAIN_IMPEXP VSectorTile *GetTile(int x,int y) const;
  ///\brief
	///Returns a sector's render mesh page using global indexing
  TERRAIN_IMPEXP VTerrainSectorMeshPageInfo *GetMeshPageInfo(int x, int y);
  TERRAIN_IMPEXP bool GetMeshPageInfo(int x, int y, int &iGlobalIndex) const;



  ///\brief
	///Interpolated height lookup at a specified world position. Performs clamping for positions outside the terrain
	///
	///\param vPos
	///The world position to test
	///
	///\returns
	///Interpolated height of the terrain. The terrain's z-position must be added to make it an absolute height
	///
	TERRAIN_IMPEXP float GetHeightAtWorldPos(const VLargePosition &vPos) const;

  ///\brief
	///Non-interpolated height lookup at specified global index position. Indices must be in valid range
	///
	///\param iSampleX
  ///Sample x index (must be in range 0..overallSamplesX)
	///
	///\param iSampleY
  ///Sample y index (must be in range 0..overallSamplesY)
	///
	///\returns
	///Terrain height. The terrain's z-position must be added to make it an absolute height
	TERRAIN_IMPEXP float GetHeightAtSamplePos(int iSampleX, int iSampleY) const;
  ///\brief
	///Uses GetHeightAtSamplePos but clamps samples to valid range beforehand
  TERRAIN_IMPEXP float GetHeightAtSamplePosClamped(int iSampleX, int iSampleY) const;
  ///\brief
	///Returns an interpolated, normalized normal vector at specified world position
  TERRAIN_IMPEXP hkvVec3 GetNormalAtWorldPos(const VLargePosition &vPos) const;
  ///\brief
	///Returns a normalized normal vector at specified sample indices. Indices must be in valid range
  TERRAIN_IMPEXP void GetNormalAtSamplePos(int iSampleX, int iSampleY, hkvVec3& destNrml, float fNrmlLen=1.0f) const;
  ///\brief
	///This version is slightly faster than GetNormalAtSamplePos but also less accurate
  TERRAIN_IMPEXP void GetNormalAtSamplePosFast(int iSampleX, int iSampleY, hkvVec3& destNrml, float fNrmlLen=1.0f) const;
  ///\brief
	///Returns the average normal vector of the two triangles that make up the quad at the specified sample position
  TERRAIN_IMPEXP void GetNormalAtFace(int iSampleX, int iSampleY, hkvVec3& destNrml, float fNrmlLen=1.0f) const;

  /// \brief
  ///   Returns the material ID at specified world position. Material IDs can be assigned to painted textures in vForge. Only the most significant ID (most opaque texture) is saved per position.
  TERRAIN_IMPEXP int GetMaterialIDAtWorldPos(const VLargePosition &vPos) const;

  inline float GetMinZ() const {return -32000.f;}
  inline float GetMaxZ() const {return 32000.f;}

  ///\brief
  ///Transforms the passed position to relative (0..1) range in xy direction
  TERRAIN_IMPEXP hkvVec2 GetRelativeTerrainPos(const VLargePosition &vPos) const;

  ///\brief
  ///Returns the Decoration Object Count in all currently loaded Sectors
  TERRAIN_IMPEXP int GetDecorationObjectCount() const;

  ///\brief
  ///Accesses the terrain config that is associated with the owner terrain
  inline const VTerrainConfig& Config() const {return m_Config;}

  ///\brief
  ///Accesses the collision mesh manager that holds all collision meshes
  inline VTerrainCollisionMeshManager& CollisionMeshManager() {return m_CollisionMeshManager;}

  ///\brief
  ///Retrieves the global bounding box of the terrain. The z-extent might not be accurate though
  TERRAIN_IMPEXP void GetTerrainBBox(VLargeBoundingBox &bbox) const;

  ///\brief
	///Performs a trace line test on all relevant sectors
	///
	///\param vTraceStart
	///Tranceline start position
	///
	///\param vTraceEnd
	///Tranceline end position
	///
	///\param iStoreCount
	///Number of elements that should be stored in the pStore. If 0, the return value is just an indicator if there was an intersection or not
	///
	///\param pStore
  ///Array to store the trace results (sorted by distance)
	///
	///\returns
	///The number of trace intersections. At most iStoreCount unless iStoreCount is 0.
	TERRAIN_IMPEXP int GetTraceIntersection(const VLargePosition &vTraceStart, const VLargePosition &vTraceEnd, int iStoreCount=0, VisTraceLineInfo_t *pStore=NULL);

  ///\brief
	///Fills a list with relevant collision meshes that are inside a bounding box
  TERRAIN_IMPEXP int GetRelevantCollisionMeshesInBoundingBox(const VLargeBoundingBox &bbox, DynArray_cl<VSimpleCollisionMesh *> &meshes);
 
  ///\brief
	///This version traces at full mesh resolution. It bypasses the collision meshes. It is significantly slower than GetTraceIntersection
  TERRAIN_IMPEXP int GetAccurateTraceIntersection(const VLargePosition &vStart, const VLargePosition &vEnd, int iStoreCount=0, VisTraceLineInfo_t *pStore=NULL, bool bSortedHits=true);

  ///\brief
	///Returns the shader effect that is used for rendering the terrain
  TERRAIN_IMPEXP VCompiledEffect* GetTerrainEffect();
  ///\brief
  ///Sets one of the sphere constraints that is used by the vegetation objects (vertex shader)
  TERRAIN_IMPEXP void SetSphereConstraint(int iIndex, const VLargePosition &vPos, float fRadius);

  TERRAIN_IMPEXP void UpdateSphereConstraintConstants();
  TERRAIN_IMPEXP VTextureObject* GetPlainBlackTexture(); ///< RGBA(0,0,0,0) and not RGBA(0,0,0,255)

  // visibility
  TERRAIN_IMPEXP int GetNewVisTag();
  TERRAIN_IMPEXP void BeginVisibilityUpdate(VTerrainVisibilityCollectorComponent &infoComp);
  TERRAIN_IMPEXP void EndVisibilityUpdate(VTerrainVisibilityCollectorComponent &infoComp);

  TERRAIN_IMPEXP void RenderDecoration(VTerrainVisibilityCollectorComponent &infoComp, IVisCallbackDataObject_cl* pCallbackData, IVTerrainDecorationModel::RenderMode_e eRenderMode = IVTerrainDecorationModel::RENDER_MODE_OTW);
  TERRAIN_IMPEXP void RenderReplacementMeshes(VTerrainVisibilityCollectorComponent &infoComp, IVisCallbackDataObject_cl* pCallbackData, IVTerrainDecorationModel::RenderMode_e eRenderMode = IVTerrainDecorationModel::RENDER_MODE_OTW);

  // file related
  TERRAIN_IMPEXP void SaveAllSectors();
  TERRAIN_IMPEXP void LoadGeometryChunk(VChunkFile &file);
  TERRAIN_IMPEXP void SaveGeometryChunk(VChunkFile &file);

  ///\brief
  ///Schedule pre-caching of sectors that are inside the passed area
  TERRAIN_IMPEXP void PreCacheRegion(const VLargeBoundingBox &bbox);
public:
  // config
  VTerrain *m_pTerrain;           ///< owner terrain
  VTerrainConfig &m_Config;
  int m_iSectorCount;             ///< m_Config.m_iSectorCount[0]*m_Config.m_iSectorCount[1]
  VTerrainSectorPtr *m_pSector;   ///< array of m_iSectorCount smart pointers
  VTerrainCollisionMeshManager m_CollisionMeshManager;
  int m_iPickingMeshDetail;

  // visibility
  //VTerrainVisibilityInfo m_VisibilityInfo; // gets updated per frame
  VTerrainVisibilityCollectorComponent *m_pRenderCollector;
  unsigned int m_iVisibleBitmask; ///< terrain's render filter mask
  VMutex m_VisibilityMutex;

  // LOD
  float m_fLODScaling, m_fMirrorLODBias;
  static float g_fDecorationDistanceScaling;
  static float g_fDecorationDistanceInvScaling;
  bool m_bForceFullLOD; // for editing

  // shader
  bool m_bSphereConstraintConstantsDirty, m_bNeedsLightgridUpdate, m_bGlobalsCBDirty, m_bShaderUsesDirectionalLight;
  VLargePosition m_vSphereConstraintPos[MAX_SPHERE_CONSTRAINTS];
  float m_fSphereConstraintRadius[MAX_SPHERE_CONSTRAINTS];
  VShaderEffectLibPtr m_spTerrainEffectLib;
  VCompiledEffectPtr m_spTerrainEffect;
  VTextureObjectPtr m_spPlainBlackTexture;
  hkvVec3 m_DecorationLightgridColors[6];

  VTerrainConstantBufferDirectionalLightConfig m_DirLightConfig;

  inline void SetWorld2SectorTransform(const hkvVec4& transform)
  {
    #if defined(_VR_DX11)
      if (memcmp(&transform,&m_ConstantBufferGlobals.m_Data.m_vWorldSpace2Sector,sizeof(transform)))
      {
        m_ConstantBufferGlobals.m_Data.m_vWorldSpace2Sector = transform;
        m_bGlobalsCBDirty = true;
      }
      VisRenderStates_cl::SetVSConstantBuffer(VTERRAIN_CB_GLOBALS,GetGlobalsConstantBuffer());
    #else
      VisRenderStates_cl::SetVertexShaderConstant(49,transform.data,1);
    #endif
  }


#if defined(_VR_DX11)
  VTerrainConstantBufferGlobals m_ConstantBufferGlobals;
  VTerrainConstantBufferGlobals *GetGlobalsConstantBuffer();
#endif

  // shared meshes:
  VisMeshBufferPtr m_spTerrainIndexBuffer;
  unsigned int m_iIndexBufferStreamMask;
  VSectorMeshLODLevelInfo_t m_LODInfo[MAX_MESH_LOD];
  bool m_bLastMeshHasHoles;

// caches:
  VTerrainSurfaceCache m_SurfaceCache;

  /// \brief
  ///   Sets a flag which indicates if a terrain mesh export is currently in progress
  inline void SetTerrainMeshExportInProgress(bool bFlag) { m_bTerrainMeshExport = bFlag; }

  /// \brief
  ///   Returns a flag which indicates if a terrain mesh export is currently in progress
  inline bool IsTerrainMeshExportInProgress() { return m_bTerrainMeshExport; }

  /// \brief
  ///   Enables or disables the export rendering mode
  inline void SetUseExportRendering(bool bFlag) { m_bExportRendering = bFlag; }

  /// \brief
  ///   Get whether the export rendering mode is enabled or not
  inline bool GetUseExportRendering() { return m_bExportRendering; }

private:
  bool m_bTerrainMeshExport;
  bool m_bExportRendering; ///< If enabled, pure terrain rendering is done without any holes, overlays, or preview light.
                           ///  This mode also forces the non baked version of the terrain to be rendered.

public:
  // profiling
  TERRAIN_IMPEXP static void InitProfiling(); ///< one time
  static int PROFILING_HEIGHTLOOKUP;
  static int PROFILING_UPDATE_MESH;
  static int PROFILING_UPLOAD_MESH;
  static int PROFILING_UPDATE_NORMALMAP;
  static int PROFILING_UPLOAD_NORMALMAP;
  static int PROFILING_TRACING;
  static int PROFILING_TRACING_GETRELEVANTMESHES;
  static int PROFILING_RENDERTERRAINMESHES;
  static int PROFILING_RENDERTERRAINREPLACEMENTMESHES;
  static int PROFILING_RENDERDECORATION;
  static int PROFILING_RENDERDECORARION_INSTANCE_SETUP;
  static int PROFILING_UNLOAD;
  static int PROFILING_RELOAD;
  static int PROFILING_STREAMING;

  static int PROFILING_VISIBILITY_OVERALL;
  static int PROFILING_VISIBILITY_SECTORVISIBILITY;
  static int PROFILING_VISIBILITY_TILEVISIBILITY;
  static int PROFILING_VISIBILITY_STARTVISIBILITY;
  static int PROFILING_VISIBILITY_ENDVISIBILITY;
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
