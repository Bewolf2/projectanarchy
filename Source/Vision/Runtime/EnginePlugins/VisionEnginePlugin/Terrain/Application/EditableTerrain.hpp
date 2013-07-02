/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file EditableTerrain.hpp

#ifndef EDITABLETERRAIN_HPP_INCLUDED
#define EDITABLETERRAIN_HPP_INCLUDED

#include <Vision/Runtime/Base/System/IO/System/FileLock.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/UndoableSectorActions.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Bitmap/BlittingHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/EditableTerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TextureWeightmapChannel.hpp>

class VCompiledShaderPass;

/// \brief
///   Derived VTerrain class that implements editing functionality on top (used inside vForge, not
///   inside the game application)
class VEditableTerrain : public VTerrain
{
public:

  /// \brief
  ///   Enum that defines the terrain editin mode
  enum HMBlitMode_e
  {
    None = 0,     ///< None
    Elevate = 1,  ///< Elevate terrain
    Subtract = 2, ///< Negative elevate
    Flatten = 3,  ///< Flatten with abs height
    Smooth = 4,   ///< Smoth terrain
    AddHoles = 5, ///< Add holes
    RemoveHoles = 6,  ///< Remove holes
  };

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor
  TERRAIN_IMPEXP VEditableTerrain();

  /// \brief
  ///   Destructor
  TERRAIN_IMPEXP virtual ~VEditableTerrain();

  ///
  /// @}
  ///

  ///
  /// @name Creation
  /// @{
  ///

  /// \brief
  ///   Overridden listener function
  ///
  /// \param pData
  ///   data object
  TERRAIN_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  /// \brief
  ///   Overridden function
  TERRAIN_IMPEXP virtual void OnNewTerrainCreated();

  /// \brief
  ///   Overridden function to load a terrain from file
  ///
  /// \param szFilename
  ///   filename of the config file
  TERRAIN_IMPEXP virtual bool LoadFromFile(const char *szFilename);

  /// \brief
  ///   Overridden function to save terrain to a folder
  ///
  /// \param szAbsFolder
  ///   absolute path to folder
  TERRAIN_IMPEXP virtual bool SaveToFile(const char *szAbsFolder);

  /// \brief
  ///   Overridden function to free terrain resources
  TERRAIN_IMPEXP virtual void FreeTerrain();

  /// \brief
  ///   Called by vForge to apply the modifyable parts of the config
  ///
  /// \param cfg
  ///   config that contains new properties
  TERRAIN_IMPEXP void SetModifiableConfig(VTerrainConfig &cfg);

  /// \brief
  ///   Changes number of sectors
  ///
  /// \param iNewCountX
  ///   New number of sectors
  /// \param iNewCountY
  ///   New number of sectors
  TERRAIN_IMPEXP void ChangeSectorCount(int iNewCountX, int iNewCountY);

  /// \brief
  ///   Internal function
  /// \internal
  TERRAIN_IMPEXP void ResetEditableFlags();

  /// \brief
  ///   Overridden factory function to create sectors of specific class
  ///
  /// \param iIndexX
  ///   Sector index
  /// \param iIndexY
  ///   Sector index
  TERRAIN_IMPEXP virtual VTerrainSector* CreateSectorInstance(int iIndexX, int iIndexY);

  /// \brief
  ///   Access sector as instance of VEditableTerrainSector
  ///
  /// \param iIndexX
  ///   Sector index
  /// \param iIndexY
  ///   Sector index
  inline VEditableTerrainSector* GetSector(int iIndexX, int iIndexY) {return (VEditableTerrainSector *)m_SectorManager.GetSector(iIndexX,iIndexY);}

  /// \brief
  ///   Sets a new terrain origin
  ///
  /// \param vNewPos
  ///   New position
  TERRAIN_IMPEXP void SetTerrainOrigin(const hkvVec3& vNewPos);


  ///
  /// @}
  ///

  ///
  /// @name Editing
  /// @{
  ///

  /// \brief
  ///   Locks height values for editing
  ///
  /// Locks the height values of the terrain. This mechansim is very similar to texture locking
  ///
  /// \param lockObj
  ///   Lock object that holds the lock range and lock flags
  TERRAIN_IMPEXP void LockHeightValues(VTerrainLockObject &lockObj);

  /// \brief
  ///   Unlocks height values after editing
  ///
  /// Unlocks the height values of the terrain. This mechansim is very similar to texture un-locking. If the locking
  /// was performed without read-only flag, then this operation causes the heights to be updated (including everything
  /// that depends on it.
  ///
  /// \param lockObj
  ///   Lock object that was used to lock the terrain
  TERRAIN_IMPEXP void UnlockHeightValues(VTerrainLockObject &lockObj);

  /// \brief
  ///   Updates physics (terrain mesh) of all pending sectors that have been updated since the last call of this function
  TERRAIN_IMPEXP void UpdatePhysics();
  
  /// \brief
  ///   Locks the luminance values of a detail texture channel
  ///
  /// Locks the luminance values of a detail texture channel. This mechansim is very similar to texture locking
  ///
  /// \param lockObj
  ///   Lock object that holds the lock range and lock flags
  ///
  /// \param pTexture
  ///   Texture resource to lock
  TERRAIN_IMPEXP void LockDetailTexture(VTerrainLockObject &lockObj, VTextureWeightmapChannelResource *pTexture);

  /// \brief
  ///   Unlocks luminance values of a detail texture channel after editing
  ///
  /// Unlocks the luminance values of a detail texture channel. If the locking
  /// was performed without read-only flag, then this operation causes the detail texture to update. Does not affect baked textures.
  ///
  /// \param lockObj
  ///   Lock object that was used to lock the texture channel
  TERRAIN_IMPEXP void UnlockDetailTexture(VTerrainLockObject &lockObj);

  /// \brief
  ///   Locks the luminance values of vegetation distribution
  ///
  /// Locks the luminance values of vegetation distribution. This mechansim is very similar to texture locking
  ///
  /// \param lockObj
  ///   Lock object that holds the lock range and lock flags
  ///
  /// \param pModelResource
  ///   Decoration resource to lock
  TERRAIN_IMPEXP void LockDecorationChannel(VTerrainLockObject &lockObj, IVTerrainDecorationModel *pModelResource);

  /// \brief
  ///   Unlocks luminance values of vegetation distribution after editing
  ///
  /// Unlocks the luminance values of vegetation distribution. If the locking
  /// was performed without read-only flag, then this operation causes the vegetation to update.
  ///
  /// \param lockObj
  ///   Lock object that was used to lock the decoration channel
  /// \param bFinalUpdate
  ///   if true, a more sophisticated positioning on the ground is used. false is used duriong painting
  TERRAIN_IMPEXP void UnlockDecorationChannel(VTerrainLockObject &lockObj, bool bFinalUpdate);

  /// \brief
  ///   Manually triggers decoration update of all channels. If bFullTerrain is false, only modified sectors (in m_DirtyVegetationSectors list) are updated
  TERRAIN_IMPEXP void UpdateDecoration(bool bFullTerrain);

  /// \brief
  ///   Locks the tile hole grid (nxn array of bool values) for editing
  ///
  /// Locks the tile hole grid (nxn array of bool values) for editing. This mechansim is very similar to texture locking
  ///
  /// \param lockObj
  ///   Lock object that holds the lock range and lock flags
  ///
  TERRAIN_IMPEXP void LockHoleMask(VTerrainLockObject &lockObj);

  /// \brief
  ///   Unlocks the tile hole grid after editing
  ///
  /// Unlocks the tile hole grid after editing. If the locking
  /// was performed without read-only flag, then this operation causes the holes to be updates, i.e. new index buffers are generated.
  ///
  /// \param lockObj
  ///   Lock object that was used to lock the hole mask
  TERRAIN_IMPEXP void UnlockHoleMask(VTerrainLockObject &lockObj);
 
  /// \brief
  ///   Shows or hides the 3D cursor overlay at current position
  ///
  /// \param bStatus
  ///   New status
  inline void SetEnable3DCursor(bool bStatus) {m_bShow3DCursor=bStatus;}

  /// \brief
  ///   Sets the projected cursor texture
  ///
  /// \param pTex
  ///   Texture to use
  inline void SetCursorTexture(VTextureObject *pTex) {m_spCursorTexture=pTex;}

  /// \brief
  ///   Moves the 3D cursor to first trace hit
  ///
  /// \param vTraceStart
  ///   Trace start position
  ///
  /// \param vTraceEnd
  ///   Trace end position
  ///
  /// \param fCursorRadius
  ///   Target cursor size
  ///
  /// \param fAngle
  ///   Target cursor rotation angle (deg)
  TERRAIN_IMPEXP void SetCursorPosFromTraceResult(const hkvVec3& vTraceStart,const hkvVec3& vTraceEnd, float fCursorRadius, float fAngle=0.f);

  /// \brief
  ///   Retrieves the terrain height at the current cursor 3D position
  ///
  /// \return
  ///   Terrain height in units
  TERRAIN_IMPEXP float GetHeightAtCursorPos() const;

  /// \brief
  ///   Internal render function
  /// \internal
  TERRAIN_IMPEXP void RenderCursor();

  /// \brief
  ///   Internal function to update the cursor shader
  ///
  /// \param shader
  ///   shader to update
  /// \param vCenter
  ///   new center
  /// \param fRadius
  ///   new radius
  /// \param fAngle
  ///   new angle
  /// \param pTexture
  ///   new cursor texture
  /// \param iColor
  ///   new color
  /// \internal
  TERRAIN_IMPEXP void UpdateCursorShader(VCompiledShaderPass *shader, const hkvVec3& vCenter, float fRadius, float fAngle, VTextureObject* pTexture, VColorRef iColor);

  /// \brief
  ///   Return a technique that can be used for rendering the cursor overlay
  ///
  /// \return
  ///   technique pointer
  TERRAIN_IMPEXP VCompiledTechnique *GetCursorEffect();

  /// \brief
  ///   Return the default cusror texture
  ///
  /// \return
  ///   cursor texture
  TERRAIN_IMPEXP VTextureObject *GetDefaultCursorTexture();

  /// \brief
  ///   Selects a decoration model that should display its density overlay
  ///
  /// \param pModel
  ///   The model resource. NULL to deactivate the feature
  inline void SetDecorationModelPreviewWeightmap(IVTerrainDecorationModel *pModel) {m_spPreviewModelWeightmap=pModel;}

  /// \brief
  ///   Switches between baked and non-baked texturing mode.
  ///
  /// In non-baked mode all detail textures are rendered as single passes
  ///
  /// \param bStatus
  ///   New status
  /// \param bAllowRebaking
  ///   If true then bStatus==true will trigger a rebaking of modified sectors, otherwise only the internal status is set
  TERRAIN_IMPEXP void SetUseBakedTextures(bool bStatus, bool bAllowRebaking);

  /// \brief
  ///   Enables/Disables previewing the terrain with a directional light
  ///
  /// If disabled, the terrain is lit with default lighting, e.g. lightmaps
  ///
  /// \param bStatus
  ///   New status
  TERRAIN_IMPEXP void SetPreviewDirectionalLight(bool bStatus);

  /// \brief
  ///   Set all parameters that are relevant for directional lighting
  ///
  /// \param vDir
  ///   normalized light direction
  /// \param vLightColor
  ///   light color (each component 0..1)
  /// \param vAmbientColor
  ///   ambient color (each component 0..1)
  TERRAIN_IMPEXP void SetDirectionalLightProperties(const hkvVec3& vDir, const hkvVec4& vLightColor, const hkvVec4& vAmbientColor);

  /// \brief
  ///   Triggers blinking of the passed detail texture for a certain amount of time. Only works in non-baked mode
  ///
  /// \param pRes
  ///   The texture to blink
  /// \param fDuration
  ///   duration in seconds
  TERRAIN_IMPEXP void BlinkDetailTexture(VTextureWeightmapChannelResource *pRes, float fDuration);

  /// \brief
  ///   Triggers white highlighting of terrain parts with specified material ID
  inline void SetHighlightMaterialID(int iID)
  {
    m_iHighlightMaterialID = iID;
  }

  /// \brief
  ///   Interal function to notify vForge
  ///
  /// \return
  ///   true if the view should be refreshed
  inline bool WantsViewUpdate() const {return m_fBlinkDuration>0.f;}

  /// \brief
  ///   Enables rendering of a minimap texture overlay
  ///
  /// \param pMiniMap
  ///   The texture to overlay. NULL to deactivate
  inline void SetMinimapTexture(VTextureObject *pMiniMap) {m_spMiniMapTexture=pMiniMap;}

  /// \brief
  ///   Modify height values with the passed bitmap brush at the current cursor position
  ///
  /// Used by vForge while painting
  ///
  /// \param pBitmap
  ///   bitmap to use as a brush
  /// \param fIntensity
  ///   intensity for the blitting
  /// \param eMode
  ///   blitting mode (elevate, flatten, ...)
  /// \param fHeight
  ///   absolute height for flatten mode
  TERRAIN_IMPEXP void BlitHeightBrush(VisBitmap_cl *pBitmap, float fIntensity, HMBlitMode_e eMode, float fHeight);

  /// \brief
  ///   Smooth painting
  ///
  /// Used by vForge while painting
  ///
  /// \param pBitmap
  ///   bitmap to use as a brush
  /// \param fIntensity
  ///   intensity for the blitting
  TERRAIN_IMPEXP void BlitHeightBrushSmooth(VisBitmap_cl *pBitmap, float fIntensity);

  /// \brief
  ///   Paints decoration with the passed bitmap brush at the current cursor position
  ///
  /// Used by vForge while painting
  ///
  /// \param pModel
  ///   target decoration channel
  /// \param pBrush
  ///   bitmap to use as a brush
  /// \param fIntensity
  ///   intensity for the blitting
  /// \param bSubtract
  ///   true for removing decoration
  TERRAIN_IMPEXP void BlitVegetationMap(IVTerrainDecorationModel *pModel, VisBitmap_cl *pBrush, float fIntensity, bool bSubtract);

  /// \brief
  ///   Paints a detail texture with the passed bitmap brush at the current cursor position
  ///
  /// Used by vForge while painting
  ///
  /// \param pTarget
  ///   target detail texture
  /// \param pBrush
  ///   bitmap to use as a brush
  /// \param fIntensity
  ///   intensity for the blitting
  /// \param bSubtract
  ///   true for removing this channel
  TERRAIN_IMPEXP void BlitDetailTexture(VTextureWeightmapChannelResource *pTarget, VisBitmap_cl *pBrush, float fIntensity, bool bSubtract);

  /// \brief
  ///   Modifies holes with the passed bitmap brush at the current cursor position
  ///
  /// Used by vForge while painting
  ///
  /// \param pBrush
  ///   bitmap to use as a brush
  /// \param bCloseHoles
  ///   true to close holes
  TERRAIN_IMPEXP void BlitHoleBrush(VisBitmap_cl *pBrush, bool bCloseHoles);

  /// \brief
  ///   Evaluates cursor extents
  /// \internal
  void CreateRotatedSourceCoordinates(float fAngle, hkvVec2 targetCoords[4]);

  /// \brief
  ///   Runs a heightmap filter on the passed rectangle
  ///
  /// Used by vForge
  ///
  /// \param pFilter
  ///   filter interface to run
  /// \param affectedArea
  ///   bounding box on the terrain
  /// \param pUserData
  ///   unused data that is forwarded to the filter
  /// \param pLog
  ///   optional log output target (errors etc.)
  TERRAIN_IMPEXP bool ApplyFilter(IVTerrainFilter *pFilter, const VLargeBoundingBox& affectedArea, void *pUserData, IVLog *pLog=NULL);

  /// \brief
  ///   Called internally to reload editing related shaders
  TERRAIN_IMPEXP void ReloadEditingShaders();

  /// \brief
  ///   Sets the global status that determines whether modified terrain files should be saved back to file when unloaded (default: enabled when inside the editor)
  TERRAIN_IMPEXP void SetSaveModifiedFiles(bool bStatus);

  /// \brief
  ///   Updates the variables responsible for finding the default effect in the terrain config.
  TERRAIN_IMPEXP void UpdateDefaultEffectSettingsInConfig();

  ///
  /// @}
  ///
  
  ///
  /// @name File Locks
  /// @{
  ///

  /// \brief
  ///   Iterates over sectors specified by the range box and tries to get a file lock
  ///
  /// Used by vForge
  ///
  /// \param range
  ///   The bounding box to test
  /// \param bReturnOnFail
  ///   if true, return immediately after the first sector that fails
  /// \param bTestOnly
  ///   if true then test only without actual locking
  /// \return
  ///   true if operation was successful
  TERRAIN_IMPEXP bool GetFileLocksOnSectorRange(const VLargeBoundingBox &range, bool bReturnOnFail, bool bTestOnly);

  /// \brief
  ///   Iterates over sectors specified by the range box and releases the file lock if owned by this user
  ///
  /// \param range
  ///   The bounding box to test
  /// \param bSaveSectors
  ///   if true, locked sectors are saved to file
  TERRAIN_IMPEXP void ReleaseFileLocksOnSectorRange(const VLargeBoundingBox &range,bool bSaveSectors);

  /// \brief
  ///   Releases all file locks that are owned by this user
  ///
  /// \param bSaveSectors
  ///   if true, locked sectors are saved to file
  TERRAIN_IMPEXP void ReleaseAllSectorFileLocks(bool bSaveSectors);

  ///
  /// @}
  ///

  ///
  /// @name Actions
  /// @{
  ///

  /// \brief
  ///   Returns an action pointer referencefor the specified sector
  ///
  /// \param iSectorX
  ///   Sector index
  /// \param iSectorY
  ///   Sector index
  /// \return
  ///   The action smart pointer reference - can point to NULL
  inline VUndoableSectorActionPtr &SectorAction(int iSectorX, int iSectorY) 
  {
    if (!m_pSectorAction) AllocateSectorActions();
    VASSERT(iSectorX>=0 && iSectorX<m_Config.m_iSectorCount[0])
    VASSERT(iSectorY>=0 && iSectorY<m_Config.m_iSectorCount[1])
    return m_pSectorAction[iSectorX+iSectorY*m_Config.m_iSectorCount[0]];
  }

  /// \brief
  ///   Allocates the pointer array
  TERRAIN_IMPEXP void AllocateSectorActions();

  /// \brief
  ///   Called after updating to collect all action instances
  ///
  /// \param pCollection
  ///   Collection that will be filled with instances of VUndoableSectorAction
  TERRAIN_IMPEXP void OnSectorUpdateFinished(VUndoableSectorActionCollection* pCollection);


  ///
  /// @}
  ///

  ///
  /// @name Lightmapping
  /// @{
  ///

  /// \brief
  ///   Collect receiver and caster geometry for vLux processing
  ///
  /// \param info
  ///   Collector
  TERRAIN_IMPEXP void GatherLightmapInfo(VLightmapSceneInfo &info);

  /// \brief
  ///   Called after vLux processing to apply lightmapping information
  ///
  /// \param info
  ///   lightmap provider
  TERRAIN_IMPEXP void GetLightmapInfo(VLightmapSceneInfo &info);

  /// \brief
  ///   Helper function to invalidate lightmaps
  TERRAIN_IMPEXP void SetFullbrightLightmaps();

  /// \brief
  ///   Sets lightmapping properties
  ///
  /// \param iSize
  ///   Wanted size of the lightmap textures
  /// \param bPerSector
  ///   either per sector or for the whole terrain
  inline void SetLightmapPageSize(int iSize,bool bPerSector) 
  {
    m_iLightmapPageSize = iSize;
    m_bPerSectorLightmaps = bPerSector;
  }

  /// \brief
  ///   Sets shadow flag for vLux
  ///
  /// \param bStatus
  ///   true to cast static shadows
  inline void SetCastStaticShadows(bool bStatus) {m_bCastStaticShadows=bStatus;}


  ///
  /// @}
  ///

  static inline void ForceUpdateDecorationReceiverList() {m_iFrameRelevantMeshesDetected=0xffffffff;}
  VisStaticGeometryInstanceCollection_cl& DetermineRelevantDecorationReceiverMeshes(VTerrainSector *pSector);
  void AddDirtyVegetationRange(const hkvAlignedBBox &bbox);
  static void AddDirtyVegetationRangeAllTerrains(const hkvAlignedBBox &bbox);
  TERRAIN_IMPEXP static VisCallback_cl OnDecorationDirtyRangeChanged;
  void TriggerDirtyRangeChanged();
  inline int GetDirtySectorCount() const {return m_DirtyVegetationSectors.Count();}

public:
  VLargeBoundingBox m_CursorBBox;  ///< Current world space cursor box
  VLargePosition m_vCursorCenter;  ///< Current world space cursor center
  float m_fCursorWSRadius;         ///< Cursor radius
  float m_fCursorRotationAngle;    ///< Cursor rotation
  VSmartPtr<VCompiledTechnique> g_spCursorFX; ///< Current cursor technique

  VTextureObjectPtr m_spDefaultCursorTexture; ///< Default cursor texture
  VTextureObjectPtr m_spCursorTexture;  ///< Cursor texture
  VTextureObjectPtr m_spMiniMapTexture; ///< mini map overlay texture

  bool m_bFailedLoadingCursorFXLib; ///< Internal status flag
  bool m_bShow3DCursor;             ///< show cursor
  bool m_bUseBakedTextures;         ///< bake mode status
  bool m_bCastStaticShadows;        ///< static shadows
  bool m_bPreviewDirectionalLight;  ///< directional preview
  bool m_bPreviewHoleOverlay;       ///< hole overlay status
  bool m_bPerSectorLightmaps;       ///< lightmap property
  bool m_bShowMiniMap;              ///< minimap display status
  bool m_bSaveModifiedFiles;        ///< global flag that determines whether modified terrain files should be saved back to file (default: enabled when inside the editor)
  bool m_bCheckSectorLocks;         ///< If enabled, painting operations will try to lock the affected range throgh file locks. If false, then VEditableTerrain::GetFileLocksOnSectorRange will always return true
  int m_iLightmapPageSize;          ///< lightmap setting

  IVTerrainDecorationModelPtr m_spPreviewModelWeightmap; ///< decoration model to preview density channel

  VTextureWeightmapChannelResourceCollection m_WeightmapChannels; ///< all detail textures used
  
  VTextureWeightmapChannelResourcePtr m_spBlinkChannel; ///< the texture that is currently used for highlighting
  float m_fBlinkDuration; ///< Remaining duration
  int m_iHighlightMaterialID;

  /// \brief
  ///   Internal function to reserve temporary mem for locking
  ///
  /// \param iByteSize
  ///   required size
  ///
  /// \return
  ///   memory pointer
  inline void* GetLockBuffer(int iByteSize) {m_LockMem.EnsureSize(iByteSize);return m_LockMem.GetDataPtr();}
  
protected:
  V_DECLARE_SERIAL_DLLEXP( VEditableTerrain,  TERRAIN_IMPEXP )

  DynArray_cl<float> m_fBrushBuffer;        ///< Internal temp buffer
  DynArray_cl<float> m_fSourceBuffer;       ///< Internal temp buffer
  DynArray_cl<float> m_fDestinationBuffer;  ///< Internal temp buffer

  // IO
  //VFileLock *m_pSectorFileLocks; // sectorcount instances
  VUndoableSectorActionPtr *m_pSectorAction; ///< sectorcount instances

  // locking and editing
  DynArray_cl<char> m_LockMem;  ///< Internal temp buffer
  VisStaticGeometryInstanceCollection_cl m_AffectedByCursor; ///< sectors affected by cursor

  // shader
  VCompiledTechniquePtr m_spDetailTexPass;      ///< Internal technique for rendering
  VCompiledTechniquePtr m_spDetailTexPass3way;  ///< Internal technique for rendering
  VCompiledTechniquePtr m_spDirLightPass;       ///< Internal technique for rendering
  VCompiledTechniquePtr m_spLightmapPass;       ///< Internal technique for rendering
  VCompiledTechniquePtr m_spFogPass;            ///< Internal technique for rendering
  VCompiledTechniquePtr m_spClearAlphaPass;     ///< Internal technique for rendering

  VCompiledTechniquePtr m_spWeightmapPass;      ///< Internal technique for rendering
  VCompiledTechniquePtr m_spHoleOverlayPass;    ///< Internal technique for rendering
  VCompiledTechniquePtr m_spLockedOverlayPass;  ///< Internal technique for rendering
  VCompiledTechniquePtr m_spMiniMapPass;        ///< Internal technique for rendering
  VCompiledTechniquePtr m_spHighlightMaterialIDPass;        ///< Internal technique for rendering

  VRefCountedCollection<VEditableTerrainSector> m_PendingPhysicsUpdateSectors; ///< Collection of sectors with physics out of date

  // painting on meshes
  static unsigned int m_iFrameRelevantMeshesDetected;
  VisStaticGeometryInstanceCollection_cl m_TempRelevantMeshes;
  static VObjectComponentCollection m_MeshPaintingComponents;
  VRefCountedCollection<VEditableTerrainSector> m_DirtyVegetationSectors;
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
