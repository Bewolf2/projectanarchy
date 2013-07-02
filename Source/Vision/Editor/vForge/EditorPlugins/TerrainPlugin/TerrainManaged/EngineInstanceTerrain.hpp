/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::ComponentModel;
using namespace System::IO;

using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace VisionEditorPlugin::Shapes;
using namespace ManagedFramework;

using namespace TerrainBase;
using namespace TerrainBase::Editing;
using namespace TerrainBase::Terrain;

class TerrainConfig;
class VEditableTerrain;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>
#using <mscorlib.dll>
#include <vcclr.h>


namespace TerrainManaged
{

  public enum class HeightmapEditMode_e
  {
    None = VEditableTerrain::None,
    Elevate = VEditableTerrain::Elevate,
    Subtract = VEditableTerrain::Subtract,
    Flatten = VEditableTerrain::Flatten,
		Smooth = VEditableTerrain::Smooth,
    AddHoles = VEditableTerrain::AddHoles,
    RemoveHoles = VEditableTerrain::RemoveHoles,
  };

  public enum class TerrainLODMetric_e
  {
    [Description("Both distance and terrain noise is used to calculate the LOD of a terrain sector")]
    DistanceAndNoise = VTerrain::VLODMODE_NOISE_AND_DISTANCE,
    [Description("The LOD is distance based only")]
    DistanceOnly = VTerrain::VLODMODE_DISTANCE_ONLY,
    [Description("Same as DistanceAndNoise but ignores the camera height (just uses x and y of the distance)")]
    DistanceAndNoise_NoHeight = VTerrain::VLODMODE_NOISE_AND_DISTANCE_XY,
    [Description("Same as DistanceOnly but ignores the camera height (just uses x and y of the distance)")]
    DistanceOnly_NoHeight = VTerrain::VLODMODE_DISTANCE_ONLY_XY,
  };

  [Flags]
  public enum class SectorEditorFlags_e
  {
    None            = SECTOR_EDITORFLAG_NONE,
    MeshDirty       = SECTOR_EDITORFLAG_MESHDIRTY,
    HeightmapDirty  = SECTOR_EDITORFLAG_HEIGHTMAPDIRTY,
    SecorFileDirty  = SECTOR_EDITORFLAG_SECTORFILEDIRTY,
    WeightmapsDirty = SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY,
    UseTempDir      = SECTOR_EDITORFLAG_TEMPDIR,
    OwnsLock        = SECTOR_EDITORFLAG_OWNSLOCK,
    NetworkLock     = SECTOR_EDITORFLAG_NETWORKLOCK,

    AnythingDirty   = SECTOR_EDITORFLAG_ANYTHINGDIRTY
  };

  public enum class SectorPhysicsType_e
  {
    Approximate = VTerrainSector::VPHYSICSTYPE_APPROXIMATE,
    Precise = VTerrainSector::VPHYSICSTYPE_PRECISE,
    PreciseOfflineWelding = VTerrainSector::VPHYSICSTYPE_PRECISE_OFFLINE_WELDING
  };

  [Serializable]
  public ref class TerrainMeshExportSettings
  {
  public:
    TerrainMeshExportSettings()
    {
      _bAutomaticExport = false;
      _bSimplifyMesh = true;
      _bUseLegacySolver = true;
      _bBakedLightmaps = true;
      
      _iTextureResolution = 512;
      _fMaxErrorDistance = 25.0f * EditorManager::Settings->GlobalUnitScaling;

      _fLoadedDistance = -1;
      _fCacheInMargin = 1000;
      _fCacheOutMargin = 1000;
    }

    TerrainMeshExportSettings(TerrainMeshExportSettings^ other)
    {
      _bAutomaticExport = other->_bAutomaticExport;
      _bSimplifyMesh = other->_bSimplifyMesh;
      _bUseLegacySolver = other->_bUseLegacySolver;
      _bBakedLightmaps = other->_bBakedLightmaps;
      _iTextureResolution = other->_iTextureResolution;
      _fMaxErrorDistance = other->_fMaxErrorDistance;

      _fLoadedDistance = other->_fLoadedDistance;
      _fCacheInMargin = other->_fCacheInMargin;
      _fCacheOutMargin = other->_fCacheOutMargin;
    }

    bool _bAutomaticExport;
    bool _bSimplifyMesh;
    bool _bUseLegacySolver;
    bool _bBakedLightmaps;
    int _iTextureResolution;
    float _fMaxErrorDistance;

    float _fLoadedDistance;
    float _fCacheInMargin;
    float _fCacheOutMargin;
  };

  class NativeTerrainListener;

  public ref class EngineInstanceTerrain : public IEngineShapeInstance
  {
  public:
    EngineInstanceTerrain(ITerrainCallback^ pSectorCallback);
    VOVERRIDE void DisposeObject() override;
    VOVERRIDE IntPtr GetNativeObject() override {return System::IntPtr((void *)m_pTerrain);}

     // overridden IEngineShapeInstance functions
    VOVERRIDE void SetVisible(bool bStatus) override {m_bVisible=bStatus;UpdateVisibleStatus();}
    VOVERRIDE void SetPosition(float x,float y,float z) override;
    VOVERRIDE void SetOrientation(float yaw,float pitch,float roll) override {}
    VOVERRIDE void SetScaling(float x,float y, float z) override {}
    VOVERRIDE void SetUniqueID(unsigned __int64 iID) override;

    VOVERRIDE void TraceShape(Shape3D^ ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult^ %result) override;
    VOVERRIDE bool GetLocalBoundingBox(BoundingBox^% bbox) override {return false;}
    VOVERRIDE bool OnExport(SceneExportInfo^ info) override;
    VOVERRIDE void GetDependencies(ResourceSnapshotInfo^ info) override;
    VOVERRIDE void OnBeforeExport(SceneExportInfo^ info) override {if (m_pTerrain!=NULL) m_pTerrain->SetMarkInsideSerializationSession(true);}
    VOVERRIDE void OnAfterExport(SceneExportInfo^ info) override {if (m_pTerrain!=NULL) m_pTerrain->SetMarkInsideSerializationSession(false);}
    void SaveToFile();

    // components:
    //VOVERRIDE bool get_SupportsComponents() override {return true;}
    VOVERRIDE property bool SupportsComponents { bool get() override {return true;} }

    VOVERRIDE bool CanAttachComponent(ShapeComponent^ component, String^% sError) override {return ConversionUtils::CanAttachComponent(m_pTerrain,component,sError);}
    VOVERRIDE void OnAttachComponent(ShapeComponent^ component) override {ConversionUtils::OnAttachComponent(m_pTerrain,component);}
    VOVERRIDE void OnRemoveComponent(ShapeComponent^ component) override {ConversionUtils::OnRemoveComponent(m_pTerrain,component);}
    VOVERRIDE void OnPostEngineInstanceCreation() override {ConversionUtils::CallOnDeserializationFunction(m_pTerrain);}

    // creation/import:
    void SetConfig(TerrainBase::Config::TerrainConfig^ pConfig, ArrayList^ pDetailTextures, ArrayList^ pDecoModels);
    void SetModifiableConfig(TerrainBase::Config::TerrainConfig^ pConfig);
    bool GetConfig(TerrainBase::Config::TerrainConfig^ pConfig);
    bool CreateFromCurrentV3D(TerrainBase::Config::TerrainConfig^ pConfig);
    // visibility
    void SetVisibleBitmask(unsigned int iMask) {m_iVisibilityBitmask=iMask;UpdateVisibleStatus();}
    void SetSectorVisibleBitmask(int x, int y, unsigned int iMask);
    unsigned int GetSectorVisibleBitmask(int x, int y);
    void SetSectorCollisionBitmask(int x, int y, unsigned short iMask);
    unsigned short GetSectorCollisionBitmask(int x, int y);
    void SetSectorPhysicsType(int x, int y, SectorPhysicsType_e ePhysicsType);
    SectorPhysicsType_e GetSectorPhysicsType(int x, int y);
    void SetZoneUniqueID(int x, int y, unsigned __int64 iID);
    void SetVisibleInVisZones(bool bStatus) {m_pTerrain->SetVisibleInVisZones(bStatus);}
    void SetFastUpdate(bool bStatus) {m_pTerrain->SetFastUpdate(bStatus);}
    void UpdateVisibleStatus();
    bool GetSectorBoundingBox(int x, int y, BoundingBox^% bbox);
    void SetSectorBoundingBox(int x, int y, BoundingBox^ bbox);
    void AssignGeometryToVisibiltyZone(int iSectorX, int iSectorY, VisibilityZoneShape^ pZone);
    System::IntPtr GetNativeVisibiltyZone(int iSectorX, int iSectorY);
    String^ SplitPerSectorTextures(String^ diffuse,String^ normal,String^ specular, bool bAddBorder, DetailTextureResource^ pTarget, ProgressStatus^ pProgress);
    String^ SplitPerSectorTexture(String^ filename, String^ *targetname, int *piSize, bool bAddBorder);

    void SaveAndUnload(ProgressStatus^ progress, float fProgressScale);
    void OnTerrainFolderChanged(String^ newFolder, ProgressStatus^ progress, float fProgressScale);

    // shader
    void SetShaderEffect(ShaderEffectConfig^ pFXConfig);

    // LOD
    void SetLODScaling(TerrainLODMetric_e metric, float fValue, float fMirrorBias);
    TerrainLODMetric_e GetLODMetric();
    void SetForceFullLOD(bool bStatus)
    {
      if (m_pTerrain!=NULL)
        m_pTerrain->m_SectorManager.m_bForceFullLOD = bStatus;
    }

    // lighting
    void SetLightmapPageSize(int iSize,bool bPerSector);
    void SetCastStaticShadows(bool bStatus);
    void SetLightInfluenceBitmask(unsigned short iMask);
    void SetFullbrightLightmaps();
    bool UsesSceneSpecificLightmaps() {return m_pTerrain->m_Config.m_bSceneSpecificLightmaps;}
    void SetUseSceneSpecificLightmaps(bool bStatus) {m_pTerrain->m_Config.m_bSceneSpecificLightmaps = bStatus;}

    // tracing/physics
    bool TraceTerrain(Vector3F start, Vector3F end, Vector3F %touchPoint);
    float TerrainHeightAtPosition(Vector3F pos);
    float TerrainHeightAtSamplePos(int x, int y);
    void UpdatePhysicsMeshes();

    //editing
    void SetReferenceStatus(bool bIsReference) {m_bIsReference=bIsReference;}
    bool WantsViewUpdate();
    void SetExportAsEditable(bool bStatus) {m_bExportAsEditable = bStatus;}
    void SelectTerrainBrush(TerrainBrush^ pBrush);
    void Update3DCursor(Cursor3DProperties^ pProperties, float fMouseX, float fMouseY);
    Vector3F GetCurrentCursorCenter();
    void BlitHeightBrush(TerrainBrush^ pBrush, HeightmapCursorProperties^ pProperties, HeightmapEditMode_e mode);
    void SetCurrentVegetationModel(DecorationModelResource^ pModel);
    bool GetDecorationModelDensityResolution(DecorationModelResource^ pModel);
    void BlitVegetationBrush(TerrainBrush^ pBrush, Cursor3DProperties^ pProperties, bool bSubtract);
    void SetCurrentDetailTexture(DetailTextureResource^ pTexture);
    void BlinkDetailTexture(DetailTextureResource^ pTexture, float fDuration);
    void SetHighlightMaterialID(int iID);
    void BlitDetailTexture(TerrainBrush^ pBrush, Cursor3DProperties^ pProperties, bool bSubtract);
    void SetPreviewVegetationModelWeightmap(DecorationModelResource^ pModel);
    void SetUseBakedTextures(bool bStatus, ShaderEffectConfig^ pFXConfig);
    void SetPreviewDirectionalLight(bool bStatus);
    void SetDirectionalLightProperties(Vector3F lightDir,Color ambColor, Color dirColor);
    void SetDisplayHoleOverlay(bool bStatus);
    void SetDisplayMiniMap(bool bStatus);
    int GetDecorationObjectCount();
    void ReplaceDecorationModel(DecorationModelResource^ pModel, IVTerrainDecorationModel *pOldModel, IVTerrainDecorationModel *pNewModel);

    VTextureWeightmapChannelResource* SetNativeTextureProperties(DetailTextureResource^ pTexture);
    void UpdateDetailTextureProperties(DetailTextureResource^ pTexture, bool bCreateWhenMissing, ProgressStatus^ pProgress, bool bUpdateMaterialID);
    void DeleteDetailTexture(DetailTextureResource^ pTexture, ProgressStatus^ pProgress);
    void UpdateDetailTextureOrder(ArrayList^ list,bool bModifySectors, ProgressStatus^ pProgress);

    // filters:
    bool ApplyHeightmapFilter(IHeightmapFilter^ filter, ProgressStatus^ progress);
    bool ApplyDecorationFilter(IDecorationFilter^ filter, ProgressStatus^ progress);

    float GetDetailTextureDensity(DetailTextureResource^ pSrc, int x, int y, int iTargetResX, int iTargetResY);
    float GetAverageNormalZ(int x, int y, int iTargetResX, int iTargetResY);
    
    static String^ QueryTextureSwizzleMask(String^ filename);
    void ImportWeightmap(String^ filename, DetailTextureResource^ pDestR, DetailTextureResource^ pDestG, DetailTextureResource^ pDestB, DetailTextureResource^ pDestA, ProgressStatus^ pProgress);
    void DetailTexturesFromSlope(String^ filename, DetailTextureResource^ pDestR, DetailTextureResource^ pDestG, DetailTextureResource^ pDestB, DetailTextureResource^ pDestA, float fNormalNoise, ProgressStatus^ pProgress);

    IVTerrainDecorationModel* SetNativeModelProperties(DecorationModelResource^ pModel, IVTerrainDecorationModel* pNativeModel);
    void UpdateDecorationModelProperties(DecorationModelResource^ pModel, bool bCreateWhenMissing);
    void DeleteDecorationModel(DecorationModelResource^ pModel);
    void ReloadDecorationModel(DecorationModelResource^ pModel);

    void UpdateDecoration(bool bFullTerrain);
    void SynchronizeDecorationModels(ArrayList^ pModels);

    // selection
    ITerrainClipboardObject^ CopySelection(TerrainSelection^ pSelection);
    void PasteSelection(TerrainSelection^ pSelection, ITerrainClipboardObject^ pData);

    void CreateSectorMapPreview(int x, int y, BitmapData^ pLockData);
    void OnSectorUnloaded(int x, int y);
    void OnSectorReloaded(int x, int y);

    void EnsureSectorRangeLoaded(int x1,int y1,int x2,int y2, int iAddEditorFlags, bool bUnload, bool bRepair, ProgressStatus^ pProgress);
    void ReloadSectorRange(int x1,int y1,int x2,int y2, bool bLoadUnloadedSectors, ProgressStatus^ pProgress);
    bool AllSectorsLoaded(int x1,int y1,int x2,int y2);

    void SetMinimapTexture(String^ filename);

    // bitmap export functions
    bool ExportHeightmapAsDDS(String^ filename, bool bNormalize);
    bool ExportDetailTextureAsDDS(String^ filename, DetailTextureResource^ pTexture);
    bool ExportDensityMapAsDDS(String^ filename, DecorationModelResource^ pModel);

    // geometry export functions
    void ExportTerrainMeshs(TerrainMeshExportSettings^ pSettings, ProgressStatus^ pProgress, float fProgressRange, bool bExportPrefab);
    String^ GetSectorMeshFileName(int iSectorX, int iSectorY);
    bool GetSectorMeshCount(int% iCountX, int% iCountY);
    Vector3F^ GetSectorOrigin(int iSectorX, int iSectorY);
    String^ GetSectorLightmapName(int iSectorX, int iSectorY);
    
    void SetTerrainMeshExportFlag(bool bFlag);

    // compatibility
    void SetSortDetailTextures(bool bStatus)
    {
      if (m_pTerrain!=NULL && !m_bIsReference)
        m_pTerrain->m_Config.m_bSortTextureChannels = bStatus;
    }
  
    bool GetSectorOwnsLock(int iSectorX, int iSectorY);
    bool GetSectorNetworkLock(int iSectorX, int iSectorY);
    void UpdateRCSFiles(int iActions, int iX, int iY, int iW, int iH);
    void UpdateRCSFiles(int iActions, int iX, int iY) { UpdateRCSFiles(iActions,iX,iY,1,1);}
    void UpdateRCSFiles(int iActions) { UpdateRCSFiles(iActions,-1,-1,1,1);} 

    void OnDecorationDirtyRegionChanged();

    ITerrainCallback^ m_pSectorCallback; // represents the C# shape
    bool m_bVisible, m_bIsReference;
    unsigned int m_iVisibilityBitmask;
    VEditableTerrain *m_pTerrain;
    IVTerrainDecorationModel *m_pCurrentDecoModel;
    VTextureWeightmapChannelResource *m_pCurrentDetailTexture;
    int m_iDirtyDecorationSectorCount;

    // static functions:
    static void PurgeAll();
    static void SetComponentUpdateThroughUI(bool bStatus);

    bool m_bExportAsEditable;
    NativeTerrainListener *m_pListener;

  private:
    void ExportPrefab();
    String^ GetMeshExportSettingsFilename();
    void SaveMeshExportSettings(TerrainMeshExportSettings^ pSettings);
    TerrainMeshExportSettings^ LoadMeshExportSettings();
  };



  class NativeTerrainListener : public IVisCallbackHandler_cl
  {
  public:
    NativeTerrainListener(EngineInstanceTerrain^ pOwner);
    ~NativeTerrainListener();

    VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData) override;
    gcroot<EngineInstanceTerrain^ >m_pOwner;
  };

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
