/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Terrain.hpp

#ifndef TERRAIN_HPP_INCLUDED
#define TERRAIN_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>


class VTerrain;
class VTerrainSector;
typedef VSmartPtr<VTerrain> VTerrainPtr;
extern VModule g_VisionEngineModule;

/// \brief
///   Interface that can be globally installed via VTerrainManager::GlobalManager().SetCustomWeightmapBaker() or VTerrainManager::GlobalManager().SetCustomNormalmapBaker() to control texture baking (e.g. compress output textures)
class IVTerrainTextureBaker
{
public:

  /// \brief
  ///   Destructor
  virtual ~IVTerrainTextureBaker()
  {
  }

  /// \brief
  ///   Function called when a weightmap bake is requested
  ///
  /// This function has to perform two things : a) Save the texture to file and b) upload to GPU texture (see pExistingTex)
  ///
  /// \param pSourceData
  ///   The uncompressed source data of the weightmap   
  ///
  /// \param szFilename
  ///   The project relative target filename of the texture to save
  ///
  /// \param iComponents
  ///   The number of used components in the source bitmap. Can be an integer between 1 (red only) and 4 (RGBA). Unused components are 0
  ///
  /// \param pExistingTex
  ///   A pointer reference to the exising texture. If this pointer is NULL this function should create and assign a new texture via Vision::TextureManager.Create2DTextureObject
  ///
  /// \returns
  ///   The result of the operation. Return false to fall back to default saving as uncompressed RGBA
  ///
  virtual bool SaveWeightmap(VisBitmap_cl *pSourceData, const char *szFilename, int iComponents, VTextureObject *&pExistingTex) = 0;

  
  /// \brief
  ///   Function called when a normal map bake is requested
  ///
  /// This function has to perform two things : a) Save the texture to file and b) upload to GPU texture (see pExistingTex)
  ///
  /// \param pSourceData
  ///   The uncompressed source data of the normal map   
  ///
  /// \param szFilename
  ///   The project relative target filename of the texture to save
  ///
  /// \param iComponents
  ///   The number of used components in the source bitmap. Can be an integer between 1 (red only) and 4 (RGBA). Unused components are 0
  ///
  /// \param pExistingTex
  ///   A pointer reference to the exising texture. If this pointer is NULL this function should create and assign a new texture via Vision::TextureManager.Create2DTextureObject
  ///
  /// \returns
  ///   The result of the operation. Return false to fall back to default saving as uncompressed RGBA
  ///
  virtual bool SaveNormalmap(VisBitmap_cl *pSourceData, const char *szFilename, int iComponents, VTextureObject *&pExistingTex) = 0;
};



/// \brief
///   Callback data object used for various terrain related callbacks
class VTerrainDataObject : public IVisCallbackDataObject_cl
{
public:
  inline VTerrainDataObject(VCallback *pSender, VTerrain *pTerrain, VTerrainSector *pSector) : IVisCallbackDataObject_cl(pSender)
  {
    m_pTerrain = pTerrain;
    m_pSector = pSector;
  }

  VTerrain *m_pTerrain;       ///< terrain that triggered the callback
  VTerrainSector *m_pSector;  ///< sector that triggered the callback
};



/// \brief
///   This class represents one single terrain instance.
///
/// This is the native counterpart of the terrain shape in vForge.
class VTerrain : public VisTypedEngineObject_cl, public VManagedResource, public IVisCallbackHandler_cl
{
public:

  /// \brief
  ///   Enum that is used for the VTerrain::SetLODMetric function
  enum VTerrainLODMode_e
  {
    VLODMODE_MODE_MASK          = 0x0f, ///< and mask to filter the LOD type 
    VLODMODE_FLAG_XYONLY        = 0x10, ///< flag that can be added to any mode to indicate that only xy of th distance should be used

    VLODMODE_NOISE_AND_DISTANCE = 0, ///< LOD metric is based on distance and terrain noise (default)
    VLODMODE_DISTANCE_ONLY      = 1,  ///< LOD metric is based on distance only
    VLODMODE_NOISE_AND_DISTANCE_XY  = VLODMODE_NOISE_AND_DISTANCE | VLODMODE_FLAG_XYONLY, ///< same as VLODMODE_NOISE_AND_DISTANCE but only considers xy distance
    VLODMODE_DISTANCE_ONLY_XY       = VLODMODE_DISTANCE_ONLY | VLODMODE_FLAG_XYONLY, ///< same as VLODMODE_NOISE_AND_DISTANCE but only considers xy distance
  };

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor
  TERRAIN_IMPEXP VTerrain();

  /// \brief
  ///   Destructor
  TERRAIN_IMPEXP virtual ~VTerrain();

  
  ///
  /// @}
  ///

  ///
  /// @name Load/create Terrain
  /// @{
  ///

  /// \brief
  ///   Creates the terrain using the passed config and saving the files to the absolute directory
  TERRAIN_IMPEXP void CreateTerrain(VTerrainConfig *pConfig, const char *szAbsProjectDir, bool bSave=true);

  /// \brief
  ///   Loads a terrain file from file. The filename must direct to the Config.vtc file
  TERRAIN_IMPEXP virtual bool LoadFromFile(const char *szFolder);

  /// \brief
  ///   Save a terrain config to a folder. The absolute folder must contain a Config.vtc file
  TERRAIN_IMPEXP virtual bool SaveToFile(const char *szAbsFolder=NULL);

  //Saves the terrain config
  TERRAIN_IMPEXP bool SaveConfigFile(const char *szFolder);

  /// \brief
  ///   Accesses the terrain's config
  inline const VTerrainConfig& Config() const {return m_Config;}

  /// \brief
  ///   Overidable function that releases all resources associated with the terrain
  TERRAIN_IMPEXP virtual void FreeTerrain();

  /// \brief
  ///   Virtual overridable that is called after the the new terrain has been created. Called by
  ///   CreateTerrain
  TERRAIN_IMPEXP virtual void OnNewTerrainCreated() {}

  /// \brief
  ///   Internal function that is called to add all terrain zones to the scene manager
  TERRAIN_IMPEXP void AddToSceneManager();

  /// \brief
  ///   Internal function that is called remove all zones from the scene manager
  TERRAIN_IMPEXP void RemoveFromSceneManager();

  /// \brief
  ///   Guarantees that the specified sector range is loaded
  TERRAIN_IMPEXP void EnsureSectorRangeLoaded(int iX1=0,int iY1=0, int iCountX=-1,int iCountY=-1);

  ///
  /// @}
  ///

  ///
  /// @name Per-instance Properties
  /// @{
  ///

  /// \brief
  ///   Associates a new shader effect with the terrain. Used for rendering the terrain
  TERRAIN_IMPEXP void SetDefaultShaderEffect(VCompiledEffect *pFX);

  /// \brief
  ///   Sets the visibility bitmask filter for this terrain to render it into specific contexts
  inline void SetVisibleBitmask(unsigned int iMask) {m_SectorManager.m_iVisibleBitmask=iMask;}

  /// \brief
  ///   Returns the current visibility bitmask. See SetVisibleBitmask
  inline unsigned int GetVisibleBitmask() const {return m_SectorManager.m_iVisibleBitmask;}

  /// \brief
  ///   Sets the LOD calculation mode. The default mode is VLODMODE_NOISE_AND_DISTANCE which means both distance and heightmap noise is considered.
  inline void SetLODMetric(VTerrainLODMode_e eMode) 
  {
    m_Config.m_bDistanceBasedLODOnly = (eMode&VLODMODE_MODE_MASK)==VLODMODE_DISTANCE_ONLY;
    m_Config.m_bIgnoreHeightForLOD = (eMode&VLODMODE_FLAG_XYONLY)!=0;
  }

  /// \brief
  ///   Return the current LOD metric
  inline VTerrainLODMode_e GetLODMetric() const
  {
    int eMode = 0;
    if (m_Config.m_bDistanceBasedLODOnly) eMode|=VLODMODE_DISTANCE_ONLY;
    if (m_Config.m_bIgnoreHeightForLOD) eMode|=VLODMODE_FLAG_XYONLY;
    return (VTerrainLODMode_e)eMode;
  }
  
  /// \brief
  ///   Sets a relative LOD scaling factor that determines how aggressive the LOD should be during
  ///   runtime. The default is 1.0
  inline void SetLODScaling(float fValue=1.0f) 
  {
    m_SectorManager.m_fLODScaling=fValue;
  }

  /// \brief
  ///   Sets a LOD bias sepcifically for mirror/water rendering. If this is 1.0 (default) terrain will be rendered with one LOD level up
  ///   runtime. The default is 1.0
  inline void SetLODMirrorBias(float fValue=1.0f) 
  {
    VASSERT(fValue>=0.f);
    fValue = hkvMath::Min(fValue,(float)m_Config.m_iMaxMeshLODHoles-0.5f);
    m_SectorManager.m_fMirrorLODBias = fValue;
  }

  /// \brief
  ///   Sets the relative far clip distance for all decoration models. Used for all terrrains.
  ///   Default is 1.0, larger values cause vegetation to be clipped further away.
  static TERRAIN_IMPEXP void SetDecorationFarClipScaling(float fValue=1.0f);

  /// \brief
  ///   Returns the current LOD scaling factor. The default is 1.0
  inline float GetLODScaling() const {return m_SectorManager.m_fLODScaling;}
  
  /// \brief
  ///   Returns the value that has been set via SetDecorationFarClipScaling
  TERRAIN_IMPEXP static float GetGlobalLODScaling();

  /// \brief
  ///   Returns the inverse value that has been set via SetDecorationFarClipScaling
  TERRAIN_IMPEXP static float GetGlobalInvLODScaling();

  /// \brief
  ///   Returns the current mirror LOD bias. The default is 1.0
  inline float GetLODMirrorBias() const {return m_SectorManager.m_fMirrorLODBias;}

  /// \brief
  ///   Sets a bitmask for dynamic light influence
  TERRAIN_IMPEXP void SetLightInfluenceBitmask(unsigned short iMask);

  inline void SetVisibleInVisZones(bool bStatus)
  {
    m_bVisibleInVisZones = bStatus;
  }

  /// \brief
  ///   If this flag is enabled, realtime update of the terrain is significantly faster, but loaded sectors have a higher memory footprint. The default state is false
  inline void SetFastUpdate(bool bStatus)
  {
    m_bFastUpdate = bStatus;
  }

  /// \brief
  ///   Returns the status that has beeb set via SetFastUpdate. The default state is false.
  inline bool GetFastUpdate() const
  {
    return m_bFastUpdate;
  }


  ///
  /// @}
  ///

  ///
  /// @name Sectors
  /// @{
  ///

  /// \brief
  ///   Accesses the resource manager for the terrain sectors. Important terrain functions are on the sector manager.
  inline VTerrainSectorManager& SectorManager() {return m_SectorManager;}

  ///
  /// @}
  ///

  ///
  /// @name Editing
  /// @{
  ///


  /// \brief
  ///   This overridable is called at startup for every sector. Custom terrain implementations can create derived sector instances.
  TERRAIN_IMPEXP virtual VTerrainSector* CreateSectorInstance(int iIndexX, int iIndexY);

  // rendering
  TERRAIN_IMPEXP void RenderTerrain(IVisCallbackDataObject_cl* pCallBackData, IVTerrainDecorationModel::RenderMode_e eRenderMode = IVTerrainDecorationModel::RENDER_MODE_OTW);
  TERRAIN_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  TERRAIN_IMPEXP void DebugRender();

  /// \brief
  ///   Sets the renderhook callback entry point that defines when decoration is rendered. Can be set to 0 to disable decoration rendering
  inline void SetDecorationRenderHookConstant(int iConst = VRH_PRE_OCCLUSION_TESTS)
  {
    m_iDecorationRenderHookConstant = iConst;
  }

  /// \brief
  ///   Removes decoration that is inside the passed bounding box (center of the decoration instance)
  ///
  /// This function provides the possibility to remove decoration from the terrain during runtime. It can be used to remove decoration dynamically when the terrain is deformed etc.
  /// Compared to painting with vegetation, there is not much overhead associated with this function since instance references are just removed from the visibility queue.
  /// The decoration instances itself still persist.
  ///
  /// \param bbox
  ///   The bounding box. Every decoration instance with its pivot inside this box will be removed
  ///
  /// \sa VTerrainSector::RemoveDecorationInstances
  TERRAIN_IMPEXP void RemoveDecorationInstances(const VLargeBoundingBox &bbox);


  /// \brief
  ///   Sets global directional lighting settings. Used by vForge when the review direction is changed
  TERRAIN_IMPEXP virtual void SetDirectionalLightProperties(const hkvVec3& vDir, const hkvVec4& vLightColor, const hkvVec4& vAmbientColor);
  void CheckUsesDirectionalLight();

  ///
  /// @}
  ///

public:
  // visibility
  void EnsureCollectorComponentAdded(IVisVisibilityCollector_cl *pCollector);
  void EnsureCollectorComponentRemoved(IVisVisibilityCollector_cl *pCollector);
  VTerrainVisibilityCollectorComponent* FindTerrainComponent(IVisVisibilityCollector_cl *pCollector);
  void EnsureCollectorComponentStatus(bool bAdd);
#ifdef SUPPORTS_SNAPSHOT_CREATION
  TERRAIN_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot);
#endif

protected:
  TERRAIN_IMPEXP virtual BOOL Reload();
  TERRAIN_IMPEXP virtual BOOL Unload();
  virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);

  //serialization
  V_DECLARE_SERIAL_DLLEXP( VTerrain,  TERRAIN_IMPEXP )
  TERRAIN_IMPEXP virtual void Serialize( VArchive &ar );

  bool m_bAddedToSceneManager, m_bVisibleInVisZones;
  bool m_bFastUpdate;

  VTerrainConfig m_Config;
  VTerrainSectorManager m_SectorManager;
  int m_iDecorationRenderHookConstant;

  VTextureObjectPtr *m_spSceneLightmaps; // SectorCount*4 texture pointers, in case Config.m_bSceneSpecificLightmaps is set
};


/// \brief
///   Resource manager that keeps track of all terrains (there should only be one, though).
/// 
/// One instance of the terrain manager can be accessed via VTerrainManager::GlobalManager()
class VTerrainManager : public VisResourceManager_cl, public IVisCallbackHandler_cl
{
public:
  TERRAIN_IMPEXP VTerrainManager();
  TERRAIN_IMPEXP void InitOneTime();
  TERRAIN_IMPEXP void DeInitOneTime();

  /// \brief
  ///   Accesses the global instance of the terrain manager
  TERRAIN_IMPEXP static VTerrainManager& GlobalManager();

  ///
  /// @name Callback for terrain load
  /// @{
  ///

  /// \brief
  ///   This callback gets triggered by the terrain manager after a new sector has been loaded
  /// 
  /// \sa VTerrainSector::Reload()
  TERRAIN_IMPEXP static VisCallback_cl OnSectorLoaded;

  /// \brief
  ///   This callback gets triggered by the terrain manager before a new sector is unloaded
  /// 
  /// \sa VTerrainSector::Reload()
  TERRAIN_IMPEXP static VisCallback_cl OnSectorUnloaded;

  /// \brief
  ///   This callback gets triggered by the terrain manager after a new sector has been created
  ///   This is useful for when you need to ensure physics sectors are really created so that physics-objects don't falling through the terrain.
  /// \sa VTerrainSector::Reload()
  TERRAIN_IMPEXP static VisCallback_cl OnSectorCreated;

  ///
  /// @}
  ///

  /// \brief
  ///   Globally install a custom baker to control how weightmap and normal map textures are saved. See IVTerrainMapBaker
  inline void SetCustomTextureBaker(IVTerrainTextureBaker *pCustomBaker)
  {
    m_pCustomBaker = pCustomBaker;
  }

  /// \brief
  ///   Return the baker that is currently installed. Returns NULL for default saving.
  inline IVTerrainTextureBaker * GetCustomTextureBaker() const
  {
    return m_pCustomBaker;
  }

protected:
  TERRAIN_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  static VTerrainManager g_TerrainManager;
  IVTerrainTextureBaker *m_pCustomBaker;
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
