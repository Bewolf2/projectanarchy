/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file EditableTerrainSector.hpp

#ifndef EDITABLETERRAINSECTOR_HPP_INCLUDED
#define EDITABLETERRAINSECTOR_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/DecorationDensityChannel.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TextureWeightmapChannel.hpp>

class VDecorationDensityChannel;

#define SECTOR_EDITORFLAG_NONE              0x00000000
#define SECTOR_EDITORFLAG_MESHDIRTY         V_BIT(0)
#define SECTOR_EDITORFLAG_HEIGHTMAPDIRTY    V_BIT(1)
#define SECTOR_EDITORFLAG_SECTORFILEDIRTY   V_BIT(2)
#define SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY   V_BIT(3)
#define SECTOR_EDITORFLAG_TEMPDIR           V_BIT(8)


#define SECTOR_EDITORFLAG_OWNSLOCK          V_BIT(16)
#define SECTOR_EDITORFLAG_NETWORKLOCK       V_BIT(17)

#define SECTOR_EDITORFLAG_ANYTHINGDIRTY     (SECTOR_EDITORFLAG_MESHDIRTY|SECTOR_EDITORFLAG_HEIGHTMAPDIRTY|SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY)




/// \brief
///   Derived class that adds editing capabilities to the VTerrainSector class. Created by
///   VEditableTerrain class
class VEditableTerrainSector : public VTerrainSector
{
public:
  TERRAIN_IMPEXP VEditableTerrainSector(VTerrainSectorManager *pManager, const VTerrainConfig &config, int iIndexX, int iIndexY);

  TERRAIN_IMPEXP VEditableTerrain *GetTerrain() const;

  // Editor flags
  TERRAIN_IMPEXP void SetEditorFlag(int iFlag);
  TERRAIN_IMPEXP void RemoveEditorFlag(int iFlag);
  inline bool IsEditorFlagSet(int iFlag) const {return (m_iEditorFlags&iFlag)==iFlag;}
  inline bool IsAnyEditorFlagSet(int iFlag) const {return (m_iEditorFlags&iFlag)!=0;}
  inline void InvalidateMesh() {SetEditorFlag(SECTOR_EDITORFLAG_HEIGHTMAPDIRTY|SECTOR_EDITORFLAG_MESHDIRTY);}
  TERRAIN_IMPEXP virtual void OnPositionChanged(const hkvVec3& vOldPos,const hkvVec3& vNewPos);
  TERRAIN_IMPEXP void ResetLightMasks();

  // rendering (in editor)
  TERRAIN_IMPEXP virtual void RenderTerrainMesh(VTerrainSectorMeshPageInfo* pPage, const VTerrainVisibilityCollectorComponent &visInfoComp);
  VCompiledShaderPass *PrepareShader(VTerrainSectorMeshPageInfo* pPage, VCompiledShaderPass *pShader);

  // locking/modifying height values
  TERRAIN_IMPEXP bool CopyRelevantHeightValues(VTerrainLockObject &destData);
  TERRAIN_IMPEXP bool GetRelevantHeightValues(const VTerrainLockObject &srcData);
  TERRAIN_IMPEXP bool CopyRelevantHoleMask(VTerrainLockObject &destData);
  TERRAIN_IMPEXP bool GetRelevantHoleMask(const VTerrainLockObject &srcData);
  TERRAIN_IMPEXP void InvalidateRelevantCollisionMeshes(const VTerrainLockObject *pSrcData);
  inline void SetHeightAt(int x, int y, float fVal)
  {
    VASSERT(m_pHeight); ///< make sure GetHeightmapValues() has been called
    VASSERT(x>=0 && x<=m_Config.m_iHeightSamplesPerSector[0]+1); ///< also allow overlapping part
    VASSERT(y>=0 && y<=m_Config.m_iHeightSamplesPerSector[1]+1);
    m_pHeight[y*m_iSampleStrideX+x] = fVal;
    InvalidateMesh();
    SetEditorFlag(SECTOR_EDITORFLAG_HEIGHTMAPDIRTY);
  }
  TERRAIN_IMPEXP void UpdateNormalmap(bool bNice);

  ///
  /// @name Decoration update
  /// @{
  ///

  /// \brief
  ///   Initiates vegetation update on this sector. Function AddSingleDecorationObject can be called inside BeginUpdateDecoration/EndUpdateDecoration brackets
  ///
  /// \param bDiscardOld
  ///    If true, all vegetation will be cleared. If false, the vegetation is retained and new one can be added
  /// \param pIgnoreModel
  ///    If bDiscardOld is false, and this model pointer is not NULL, the passed model will not be retained. Can be used to filter out models of a kind
  TERRAIN_IMPEXP void BeginUpdateDecoration(bool bDiscardOld=false,IVTerrainDecorationModel *pIgnoreModel=NULL);

  /// \brief
  ///   Adds the passed decoration instance to the sector. This function may only be called inside BeginUpdateDecoration/EndUpdateDecoration brackets
  TERRAIN_IMPEXP void AddSingleDecorationObject(const VTerrainDecorationInstance &objDesc);

  /// \brief
  ///   Ends a decoration update bracket (see BeginUpdateDecoration). This function sorts all decoration instances into the grid
  TERRAIN_IMPEXP void EndUpdateDecoration(bool bFinalUpdate);

  /// \brief
  ///   Removes all decoration from the sector
  TERRAIN_IMPEXP void RemoveDecoration();

  /// \brief
  ///   Removes all decoration of the passed model from the sector
  TERRAIN_IMPEXP void RemoveDecorationOfType(IVTerrainDecorationModel *pModel);

  /// \brief
  ///   Replaces decoration of one model with another model
  TERRAIN_IMPEXP void ReplaceDecorationOfType(IVTerrainDecorationModel *pOldModel, IVTerrainDecorationModel *pNewModel);

  /// \brief
  ///   Generate decoration from a density information
  TERRAIN_IMPEXP void CreateFromDensity(VDecorationDensityChannel *pPlane, bool bFinalUpdate);

  /// \brief
  ///   Internal function
  TERRAIN_IMPEXP bool CopyRelevantDensityMap(VTerrainLockObject &destData);

  /// \brief
  ///   Internal function
  TERRAIN_IMPEXP bool GetRelevantDensityMap(const VTerrainLockObject &srcData, bool bFinalUpdate);

  /// \brief
  ///   Access a collection of density information of all decoration types that are referenced in this sector
  inline VDecorationDensityChannelCollection& DecorationChannels() {return m_DecorationChannels;}

  /// \brief
  ///   Re-generate all decoration from density channels
  TERRAIN_IMPEXP void UpdateDecoration(bool bFinalUpdate);

  /// \brief
  ///   Internal function (return list of decoration that are accumulated inside a decoration update bracket)
  TERRAIN_IMPEXP virtual VTerrainDecorationInstance* GetAdditionalDecoration(int &iCount);

  ///
  /// @}
  ///

  // hole editing
  TERRAIN_IMPEXP VTextureObject *GetHoleOverlayTexture();
  TERRAIN_IMPEXP bool CopyRelevantHoleData(VTerrainLockObject &destData);
  TERRAIN_IMPEXP bool GetRelevantHoleData(const VTerrainLockObject &srcData);

  //detail textures
  TERRAIN_IMPEXP bool CopyRelevantDetailTextureValues(VTerrainLockObject &destData);
  TERRAIN_IMPEXP bool GetRelevantDetailTextureValues(const VTerrainLockObject &srcData);
  TERRAIN_IMPEXP void UpdateMaterialID();

  // resource functions
  TERRAIN_IMPEXP virtual BOOL Reload();
  TERRAIN_IMPEXP virtual BOOL Unload();
  TERRAIN_IMPEXP virtual bool SaveSector(bool bUseTemp=false);
  TERRAIN_IMPEXP virtual void OnConfigChanged();
  virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);
  TERRAIN_IMPEXP bool LoadSectorEditingInformation();
  TERRAIN_IMPEXP bool SaveSectorEditingInformation();
  TERRAIN_IMPEXP bool SaveWeightmapTextures();
  TERRAIN_IMPEXP bool SaveReplacementMesh();

  TERRAIN_IMPEXP bool GetNeedsThreeWayMapping(const VTextureWeightmapChannel* pChannels) const;

  TERRAIN_IMPEXP void AssertValid();
  void AssertCorrectFileRefs(const VResourceSnapshot &snapshot, const char *szFilename);
  
  //Revision control
  TERRAIN_IMPEXP void UpdateRCSFiles(int iActions);
  
  //Used by final save to save edited or swapped sectors over final sectors and to genenerate the resource snapshot
  TERRAIN_IMPEXP virtual bool SaveSectorFinal(bool bSaveSnapshot=false);

  TERRAIN_IMPEXP virtual void PrepareSector();
  
  TERRAIN_IMPEXP bool OwnsFileLock();
  TERRAIN_IMPEXP bool GetFileLock();
  TERRAIN_IMPEXP void ReleaseFileLock();

  /// \brief
  ///   In case a FileLock target was loaded or saved, our internal timestamp must be updated to mirror the file.
  ///   This is necessary in order to prevent locks on files that haven't been updated yet.
  ///
  /// \param iFileType
  ///    The FileLock targets to be updated (VTC_FILE_SECTORS_MESH, VTC_FILE_SECTORS_HMAP or VTC_FILE_EDITING_DATA).
  TERRAIN_IMPEXP void UpdateFileLockTimeStamp(int iFileType);

#ifdef WIN32
  TERRAIN_IMPEXP virtual void OnDestroying();
  TERRAIN_IMPEXP virtual IVResourcePreview *CreateResourcePreview();
#endif
  void EnsureEditingDataLoaded();
  // editing related:
public:
  int m_iEditorFlags; ///< SECTOR_EDITORFLAG_XYZ

  // normal map update
  VisBitmapPtr m_spNormalmapSysMem;

  // temp decoration update buffer
  int m_iUpdateDecorationCounter, m_iTempDecorationCount;
  DynObjArray_cl<VTerrainDecorationInstance> m_TempDecorationObj;

  // decoration source luminance planes
  VDecorationDensityChannelCollection m_DecorationChannels;

  // in case only some data was changed.
  VFileLock m_SectorInfoLock;
  VFileLock m_SectorHMapLock;
  VFileLock m_SectorMeshLock;

  // Timestamps: These are needed to prevent locking of files that have changed on disk but
  // those changes haven't been loaded yet in the code.
  VFileTime m_SectorInfoFileTime;
  VFileTime m_SectorHMapFileTime;
  VFileTime m_SectorMeshFileTime;

  // random generation    
  int m_iRandomSeed;

  VTextureWeightmapChannelCollection m_UsedWeightmapChannels; ///< subset of the terrain's collection of all detail textures

  bool m_bEditingDataLoaded;
  bool m_bHoleTextureDirty;
  VTextureObjectPtr m_spHoleOverlay;
};


typedef VSmartPtr<VEditableTerrainSector> VEditableTerrainSectorPtr;

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
