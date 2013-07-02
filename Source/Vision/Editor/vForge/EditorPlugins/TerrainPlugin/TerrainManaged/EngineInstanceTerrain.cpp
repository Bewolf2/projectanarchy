/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/EngineInstanceTerrain.hpp>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainConversionHelpers.h>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainChangedAction.hpp>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainUtil/TerrainMeshExporter.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/MeshPaintingComponent.hpp>

#include <vector>

#using <mscorlib.dll>
using namespace ManagedBase;
using namespace ManagedFramework;
using namespace System;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace System::Windows::Forms;
using namespace CSharpFramework::Actions;
using namespace CSharpFramework::Helper;

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif

#ifdef VGEOM2_LIB
#include <Vision/Tools/Libraries/Geom2/vGeom2.hpp>
#endif

namespace TerrainManaged
{
  ///////////////////////////////////////////////////////////////////////////////////////////////
  // Native listener class
  ///////////////////////////////////////////////////////////////////////////////////////////////

  NativeTerrainListener::NativeTerrainListener(EngineInstanceTerrain ^pOwner)
  {
    m_pOwner = pOwner;
    VTerrainManager::OnSectorLoaded += this;
    VTerrainManager::OnSectorUnloaded += this;
    VEditableTerrain::OnDecorationDirtyRangeChanged += this;
    VTerrainDecorationModelManager::GlobalManager().OnGatherModelProperties += this;
  }

  NativeTerrainListener::~NativeTerrainListener()
  {
    VTerrainManager::OnSectorLoaded -= this;
    VTerrainManager::OnSectorUnloaded -= this;
    VEditableTerrain::OnDecorationDirtyRangeChanged -= this;
    VTerrainDecorationModelManager::GlobalManager().OnGatherModelProperties -= this;
  }


  void NativeTerrainListener::OnHandleCallback(IVisCallbackDataObject_cl *pData)
  {
    VTerrainDataObject *pTerrainData = (VTerrainDataObject *)pData;
    if (pData->m_pSender==&VTerrainManager::OnSectorLoaded)
      m_pOwner->OnSectorReloaded(pTerrainData->m_pSector->m_iIndexX,pTerrainData->m_pSector->m_iIndexY);
    else if (pData->m_pSender==&VTerrainManager::OnSectorUnloaded)
      m_pOwner->OnSectorUnloaded(pTerrainData->m_pSector->m_iIndexX,pTerrainData->m_pSector->m_iIndexY);
    else if (pData->m_pSender==&VTerrainDecorationModelManager::GlobalManager().OnGatherModelProperties)
    {
      VTerrainDecorationModelDataObject *pModelData = (VTerrainDecorationModelDataObject *)pData;
      m_pOwner->SetNativeModelProperties(nullptr,pModelData->m_pModel);
    }
    else if (pData->m_pSender==&VEditableTerrain::OnDecorationDirtyRangeChanged)
      m_pOwner->OnDecorationDirtyRegionChanged();

  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // Height field shape
  ///////////////////////////////////////////////////////////////////////////////////////////////

  EngineInstanceTerrain::EngineInstanceTerrain(ITerrainCallback ^pSectorCallback)
  {
    m_bIsReference = false;
    m_bExportAsEditable = false;
    m_pSectorCallback = pSectorCallback;
    m_pTerrain = NULL;
    m_pCurrentDecoModel = NULL;
    m_pCurrentDetailTexture = NULL;
    m_bVisible = true;
    m_iVisibilityBitmask = 0xffffffff;
    m_iDirtyDecorationSectorCount = 0;
    m_pListener = new NativeTerrainListener(this);
  }

  void EngineInstanceTerrain::DisposeObject()
  {
    //TODO: Cleanup _temp directory?

    if (m_pTerrain)
      m_pTerrain->ResetEditableFlags(); // it will not uncache to file

    V_SAFE_RELEASE(m_pTerrain);
    V_SAFE_RELEASE(m_pCurrentDecoModel);
    V_SAFE_RELEASE(m_pCurrentDetailTexture);
    V_SAFE_DELETE(m_pListener);
  }

  void EngineInstanceTerrain::SetUniqueID(unsigned __int64 iID)
  {
    if (m_pTerrain)
      m_pTerrain->SetUniqueID(iID);
  }

  void EngineInstanceTerrain::SetZoneUniqueID(int x, int y, unsigned __int64 iID)
  {
    if (m_pTerrain)
    {
     VisVisibilityZone_cl *pZone = m_pTerrain->GetSector(x,y)->m_spSectorZone;
     if (pZone)
       pZone->SetUniqueID(iID);
    }
  }


  void EngineInstanceTerrain::SetPosition(float x,float y,float z)
  {
    if (m_pTerrain && !m_bIsReference)
      m_pTerrain->SetTerrainOrigin(hkvVec3 (x,y,z));
  }

    
  void EngineInstanceTerrain::UpdateVisibleStatus()
  {
    unsigned int iMask = m_bVisible ? m_iVisibilityBitmask:0;
    if (m_pTerrain && !m_bIsReference)
      m_pTerrain->SetVisibleBitmask(iMask);
  }


  void EngineInstanceTerrain::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    if (!m_pTerrain)
      return;

    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);

    static VisTraceLineInfo_t hitHinfo;//@@@@
    VLargePosition lStart(m_pTerrain->m_Config,vStart);
    VLargePosition lEnd(m_pTerrain->m_Config,vEnd);
    if (m_pTerrain->m_SectorManager.GetAccurateTraceIntersection(lStart,lEnd,1,&hitHinfo)==0)
      return;

    (*result).SetShapeHit_IfCloser(ownerShape,hitHinfo.distance);

  }

  bool EngineInstanceTerrain::TraceTerrain(Vector3F start, Vector3F end, Vector3F %touchPoint)
  {
    if (!m_pTerrain)
      return false;

    hkvVec3 vStart(start.X,start.Y,start.Z);
    hkvVec3 vEnd(end.X,end.Y,end.Z);
    VLargePosition vLStart(m_pTerrain->m_Config,vStart);
    VLargePosition vLEnd(m_pTerrain->m_Config,vEnd);

    static VisTraceLineInfo_t result;//@@@@
    if (!m_pTerrain->SectorManager().GetAccurateTraceIntersection(vLStart,vLEnd,1,&result))
      return false;

    (touchPoint) = Vector3F(result.touchPoint.x, result.touchPoint.y, result.touchPoint.z);
    return true;
  }

  float EngineInstanceTerrain::TerrainHeightAtPosition(Vector3F pos)
  {
    if (!m_pTerrain)
      return 0.f;
    hkvVec3 vPos(pos.X,pos.Y,pos.Z);
    VLargePosition largePos(m_pTerrain->m_Config,vPos);
    return m_pTerrain->SectorManager().GetHeightAtWorldPos(largePos);
  }

  float EngineInstanceTerrain::TerrainHeightAtSamplePos(int x, int y)
  {
    if (!m_pTerrain)
      return 0.f;
    return m_pTerrain->SectorManager().GetHeightAtSamplePosClamped(x,y);
  }

  void EngineInstanceTerrain::SaveToFile()
  {
    if (m_pTerrain && !m_bIsReference)
      m_pTerrain->SaveToFile(NULL);
  }
  
  void EngineInstanceTerrain::GetDependencies(ResourceSnapshotInfo ^info)
  {
    if (!m_pTerrain)
      return;

    VResourceSnapshot *snapshot = (VResourceSnapshot *)info->NativeResourceSnapshotPtr.ToPointer();
    m_pTerrain->GetDependencies(*snapshot);
  }

  bool EngineInstanceTerrain::OnExport(SceneExportInfo ^info)
  {
    if (!m_pTerrain)
      return true;

    if (!m_bIsReference)
    m_pTerrain->SaveToFile(NULL);

    // in case VEditableTerrain has its own Serialize function, add a flag to the terrain as well.
    VType *pSaveAsType = V_RUNTIME_CLASS(VTerrain);
    if (m_bExportAsEditable)
      pSaveAsType = V_RUNTIME_CLASS(VEditableTerrain);

    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar.WriteObject(m_pTerrain,pSaveAsType);
    return true;
  }


  void EngineInstanceTerrain::SetConfig(TerrainBase::Config::TerrainConfig ^pConfig, ArrayList ^pDetailTextures, ArrayList ^pDecoModels)
  {
    VTerrainConfig cfg;
    TerrainConversionHelpers::ConfigToNative(pConfig,&cfg);
    V_SAFE_RELEASE(m_pTerrain);
    m_pTerrain = new VEditableTerrain();
    m_pTerrain->AddRef();
    
    //Fill in project directory
    ConversionUtils::StringToVString(EditorManager::Project->ProjectDir, m_pTerrain->m_Config.m_sAbsProjectDir);
    if(cfg.m_sAbsProjectDir.IsEmpty())
      cfg.m_sAbsProjectDir = m_pTerrain->m_Config.m_sAbsProjectDir;

    //RecursiveCreateDir
    // first synchronize detail textures & models
    for (int i=0;i<pDetailTextures->Count;i++)
    {
      DetailTextureResource ^pSrc = static_cast<DetailTextureResource ^>(pDetailTextures[i]);
      VTextureWeightmapChannelResource::DetailTextureProperties_t props;
      TerrainConversionHelpers::DetailTexturePropertiesToNative(&props,pSrc);
      VTextureWeightmapChannelResource *pRes = m_pTerrain->m_WeightmapChannels.CreateDetailTexture(props);
      if (pRes)
        pRes->m_iSortingKey = i; // ensure same order as in vForge list
    }
    m_pTerrain->m_WeightmapChannels.SortByResourceSortingKey();

    // TODO: same for models

    VString absDir;
    ConversionUtils::StringToVString(EditorManager::Project->MakeAbsolute(pConfig->TerrainFolder), absDir);
    m_pTerrain->m_Config.m_sAbsTerrainDir = absDir;
    
    
    if (m_pTerrain->LoadFromFile(absDir))
    {
      // copy back to C#
      TerrainConversionHelpers::ConfigFromNative(&m_pTerrain->m_Config, pConfig);
      ConversionUtils::StringToVString(EditorManager::Project->ProjectDir,absDir);
      m_pTerrain->m_Config.MakeRelevantDirectories(absDir, true);  //make sure the folder for temp files exists
    } 
    else
    {
      if (VFileHelper::ExistsDir(absDir))
      {
        //Folder exists but we failed to load
        VASSERT_MSG(false,"Failed to load terrain config file");
        return; // Don't continue. This might overwrite existing data
      }
      
      //no directory on disc, so create it
      ConversionUtils::StringToVString(EditorManager::Project->ProjectDir,absDir);
      m_pTerrain->CreateTerrain(&cfg,absDir,true);

      float fProgessStep = 100.f/(float)(m_pTerrain->m_Config.m_iSectorCount[0]*m_pTerrain->m_Config.m_iSectorCount[1]);
      int iIndex = 0;
      EditorManager::Progress->ShowProgressDialog("Create Terrain");

      // Make sure all sectors get saved so we have the files on disk
      for (int sy=0;sy<m_pTerrain->m_Config.m_iSectorCount[1];sy++)
        for (int sx=0;sx<m_pTerrain->m_Config.m_iSectorCount[0];sx++)
        {
          VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
          pSector->EnsureLoaded();
          pSector->SetEditorFlag(SECTOR_EDITORFLAG_ANYTHINGDIRTY);
          pSector->SaveSector();   //Save the sector to final directory so it creates real files
          pSector->EnsureUnloaded();
          iIndex++;
          EditorManager::Progress->Percentage = (float)iIndex*fProgessStep;
        }

      EditorManager::Progress->HideProgressDialog();
    }

  }

  void EngineInstanceTerrain::SaveAndUnload(ProgressStatus ^progress, float fProgressScale)
  {
    if (!m_pTerrain)
      return;
    SaveToFile(); // bring latest versions to disk

    m_pTerrain->ReleaseAllSectorFileLocks(true); //Release the locks

    VTerrainConfig &cfg(m_pTerrain->m_Config);
    float fStep = 1.f / (cfg.m_iSectorCount[0]*cfg.m_iSectorCount[1]);
    for (int sy=0;sy<cfg.m_iSectorCount[1];sy++)
      for (int sx=0;sx<cfg.m_iSectorCount[0];sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        pSector->EnsureUnloaded();
        progress->Percentage += fStep*fProgressScale; // first 20%
      }
  }

  void EngineInstanceTerrain::OnTerrainFolderChanged(String ^newFolder, ProgressStatus ^progress, float fProgressScale)
  {
    if (!m_pTerrain)
      return;

    VTerrainConfig &cfg(m_pTerrain->m_Config);
    float fStep = 1.f / (cfg.m_iSectorCount[0]*cfg.m_iSectorCount[1]);

    // both change filename in the config and the resource name
    ConversionUtils::StringToVString(newFolder,cfg.m_sTerrainFolder);
    m_pTerrain->SetFilename(cfg.m_sTerrainFolder);

    cfg.MakeRelevantDirectories(cfg.m_sAbsProjectDir, false); //normal files
    cfg.MakeRelevantDirectories(cfg.m_sAbsProjectDir, true);  //temp files  
    
    // save config
    if (!m_bIsReference)
    m_pTerrain->SaveToFile(NULL);

    // re-save all sectors to fixup texture references
    for (int sy=0;sy<cfg.m_iSectorCount[1];sy++)
      for (int sx=0;sx<cfg.m_iSectorCount[0];sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        pSector->EnsureLoaded();
        pSector->SetEditorFlag(SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY); // fixup texture references upon next saving
        pSector->SaveSectorFinal(true);  // do real saving, so we don't get swap files on unload
        pSector->EnsureUnloaded();
        progress->Percentage += fStep*fProgressScale; // remaining 80%
      }

  }

  // sets the modifiable parts of the config
  void EngineInstanceTerrain::SetModifiableConfig(TerrainBase::Config::TerrainConfig ^pConfig)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    VTerrainConfig cfg;
    TerrainConversionHelpers::ConfigToNative(pConfig,&cfg);
    m_pTerrain->SetModifiableConfig(cfg);
  }


  bool EngineInstanceTerrain::GetConfig(TerrainBase::Config::TerrainConfig ^pConfig)
  {
    if (!m_pTerrain)
      return false;
    TerrainConversionHelpers::ConfigFromNative(&m_pTerrain->m_Config,pConfig);
    return true;
  }

    
  bool EngineInstanceTerrain::GetSectorBoundingBox(int x, int y, BoundingBox ^%bbox)
  {
    if (!m_pTerrain)
      return false;
    VTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    //if (!pSector->IsLoaded())
    //  return false;
    const hkvAlignedBBox &temp = pSector->GetVisZoneBoundingBox();
    (*bbox).Set(temp.m_vMin.x,temp.m_vMin.y,temp.m_vMin.z, temp.m_vMax.x,temp.m_vMax.y,temp.m_vMax.z );
    return true;
  }

  
  void EngineInstanceTerrain::SetSectorBoundingBox(int x, int y, BoundingBox ^bbox)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    VTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    hkvAlignedBBox box (hkvVec3 (bbox->X1,bbox->Y1,bbox->Z1), hkvVec3 (bbox->X2,bbox->Y2,bbox->Z2));
    pSector->SetSectorBoundingBox(box);
  }

  void EngineInstanceTerrain::SetSectorVisibleBitmask(int x, int y, unsigned int iMask)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    VTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    pSector->SetVisibleBitmask(iMask);
  }

  unsigned int EngineInstanceTerrain::GetSectorVisibleBitmask(int x, int y)
  {
    if (!m_pTerrain)
      return 0xffffffff;
    VTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    return pSector->GetVisibleBitmask();
  }

  void EngineInstanceTerrain::SetSectorCollisionBitmask(int x, int y, unsigned short iMask)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    VTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    //if (pSector->GetCollisionBitmask()==iMask) //TODO
    if (pSector->GetTraceBitmask()==iMask)  //currently using trace flags for this, since submeshes don't have collision flags
      return;

    //pSector->SetCollisionBitmask(iMask); //TODO
    pSector->SetTraceBitmask(iMask);  //currently using trace flags for this, since submeshes don't have collision flags
    if (pSector->m_pTile)
    {
      IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
      if (pPM!=NULL)
      {
        pPM->OnTerrainSectorRemoved(pSector);
        pPM->OnTerrainSectorCreated(pSector);
      }
    }
  }

  unsigned short EngineInstanceTerrain::GetSectorCollisionBitmask(int x, int y)
  {
    if (!m_pTerrain)
      return 0xffff;
    VTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    //return pSector->GetCollisionBitmask();
    return pSector->GetTraceBitmask(); //currently using trace flags for this, since submeshs don't have collision flags    
  }

  void EngineInstanceTerrain::SetSectorPhysicsType(int x, int y, SectorPhysicsType_e ePhysicsType)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    
    VEditableTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    if(pSector->GetPhysicsType() == (VTerrainSector::VPhysicsType_e)ePhysicsType)
      return;

    pSector->SetPhysicsType((VTerrainSector::VPhysicsType_e)ePhysicsType);
    pSector->SetEditorFlag(SECTOR_EDITORFLAG_HEIGHTMAPDIRTY | SECTOR_EDITORFLAG_SECTORFILEDIRTY);
    if (pSector->m_pTile)
    {
      IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
      if (pPM!=NULL)
      {
        pPM->OnTerrainSectorRemoved(pSector);
        pPM->OnTerrainSectorCreated(pSector);
      }
    }
  }

  SectorPhysicsType_e EngineInstanceTerrain::GetSectorPhysicsType(int x, int y)
  {
    if (!m_pTerrain)
      return SectorPhysicsType_e::Precise;
    VEditableTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    return (SectorPhysicsType_e)pSector->GetPhysicsType();
  }

  void EngineInstanceTerrain::AssignGeometryToVisibiltyZone(int iSectorX, int iSectorY, VisibilityZoneShape ^pZone)
  {
    if (!m_pTerrain)
      return;
    VEditableTerrainSector *pSector = m_pTerrain->GetSector(iSectorX,iSectorY);

    // should be in there already
    if (!pSector->m_spSectorZone->GetStaticGeometryInstances()->Contains(pSector))
      pSector->m_spSectorZone->AddStaticGeometryInstance(pSector);
  }

  System::IntPtr EngineInstanceTerrain::GetNativeVisibiltyZone(int iSectorX, int iSectorY)
  {
    if (!m_pTerrain)
      return System::IntPtr((void *)NULL);
    VEditableTerrainSector *pSector = m_pTerrain->GetSector(iSectorX,iSectorY);
    return System::IntPtr((void *)pSector->m_spSectorZone.m_pPtr);
  }


  bool EngineInstanceTerrain::CreateFromCurrentV3D(TerrainBase::Config::TerrainConfig ^pConfig)
  {
      return false;
    }

  
  bool EngineInstanceTerrain::ApplyHeightmapFilter(IHeightmapFilter ^filter, ProgressStatus ^progress)
  {
    if (!m_pTerrain || m_bIsReference)
      return false;

    int x1 = filter->X1;
    int y1 = filter->Y1;
    int x2 = filter->X2;
    int y2 = filter->Y2;
   
    if (x2<=x1) x2 = m_pTerrain->m_Config.m_iOverallHeightSampleCount[0]-1-x1;
    if (y2<=y1) y2 = m_pTerrain->m_Config.m_iOverallHeightSampleCount[1]-1-y1;

    int sx1 = m_pTerrain->m_Config.GetSectorIndexAtSamplePosX(x1-1); // compensate for overlapping
    int sy1 = m_pTerrain->m_Config.GetSectorIndexAtSamplePosY(y1-1);
    int sx2 = m_pTerrain->m_Config.GetSectorIndexAtSamplePosX(x2+1);
    int sy2 = m_pTerrain->m_Config.GetSectorIndexAtSamplePosY(y2+1);
    m_pTerrain->m_Config.ClampSectorIndices(sx1,sy1);
    m_pTerrain->m_Config.ClampSectorIndices(sx2,sy2);

    int iSamplesX = m_pTerrain->m_Config.m_iHeightSamplesPerSector[0];
    int iSamplesY = m_pTerrain->m_Config.m_iHeightSamplesPerSector[1];
    float *pDataBlock = (float *)m_pTerrain->GetLockBuffer((iSamplesX+2)*(iSamplesY+2)*sizeof(float));
    
    float fOverallSectorCount = (float)((sx2-sx1+1)*(sy2-sy1+1));
    float fSectorProgress = 0;
    int iSamplesXStride = iSamplesX+1;

    // iterate via sectors
    for (int sy=sy1;sy<=sy2;sy++)
      for (int sx=sx1;sx<=sx2;sx++,fSectorProgress+=1.f)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bOldLoaded = pSector->IsLoaded();
        pSector->EnsureLoaded();

        // backup height values
        VUndoableSectorActionPtr &action = m_pTerrain->SectorAction(sx,sy);
        action = new VSectorActionHeightfieldModified(pSector);

        int iOfsX = iSamplesX*sx;
        int iOfsY = iSamplesY*sy;
        int iSizeX = iSamplesX+1;
        int iSizeY = iSamplesY+1;

        // clamp rectangle
        if (iOfsX<x1)
        {
          iSizeX -= x1-iOfsX;
          iOfsX = x1;
        }
        if (iOfsY<y1)
        {
          iSizeY -= y1-iOfsY;
          iOfsY = y1;
        }
        if (iOfsX+iSizeX-1 > x2)
          iSizeX = x2-iOfsX+1;
        if (iOfsY+iSizeY-1 > y2)
          iSizeY = y2-iOfsY+1;

        VTerrainLockObject lockObj(pSector,0);
        lockObj.Init(iOfsX,iOfsY,iSizeX,iSizeY,0);
        lockObj.m_bLocked = true;
        lockObj.m_bOwnsData = false;
        lockObj.m_pData = pDataBlock;
        lockObj.m_iStride = iSamplesXStride;

        // fill the sector rectangle
        for (int y=0;y<iSizeY;y++)
          for (int x=0;x<iSizeX;x++)
            pDataBlock[y*iSamplesXStride+x] = filter->GetHeightAt(x+iOfsX,y+iOfsY);

        pSector->GetRelevantHeightValues(lockObj);
        
        if (!bOldLoaded && pSector->CanUnload())
          pSector->EnsureUnloaded();

        if (progress)
          progress->Percentage = fSectorProgress*100.f/fOverallSectorCount;
      }

    return true;
  }


  bool EngineInstanceTerrain::ApplyDecorationFilter(IDecorationFilter ^filter, ProgressStatus ^progress)
  {
    if (!m_pTerrain || m_bIsReference)
      return false;

    IVTerrainDecorationModel *pRes = SetNativeModelProperties(filter->DecorationModel,NULL);
    if (!pRes)
      return false;

    const VTerrainConfig &cfg(m_pTerrain->m_Config);

    int iResX = cfg.m_iDensityMapSamplesPerSector[0];
    int iResY = cfg.m_iDensityMapSamplesPerSector[1];
    int iCountX = iResX*cfg.m_iSectorCount[0];
    int iCountY = iResY*cfg.m_iSectorCount[1];

    // make it undoable
    for (int sy=0;sy<cfg.m_iSectorCount[1];sy++)
      for (int sx=0;sx<cfg.m_iSectorCount[0];sx++)
      {
        VUndoableSectorActionPtr &action = m_pTerrain->SectorAction(sx,sy);
        action = new VSectorActionDecoChannelModified(m_pTerrain->GetSector(sx,sy), pRes);
      }

    // lock area and run filter on it
    VTerrainLockObject lockObj(0,0,iCountX,iCountY,VIS_LOCKFLAG_DISCARDABLE);
    m_pTerrain->LockDecorationChannel(lockObj,pRes);
    bool bConsiderHoles = filter->ConsiderTerrainHoles;

    UBYTE *pDest = (UBYTE *)lockObj.m_pData;
    int iSectorCount = cfg.m_iSectorCount[0]*cfg.m_iSectorCount[1];
    int iSectorCounter = 0;
    for (int sy=0;sy<cfg.m_iSectorCount[1];sy++)
      for (int sx=0;sx<cfg.m_iSectorCount[0];sx++, iSectorCounter++)
      {
        VTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bOldLoaded = pSector->IsLoaded();
        if (bConsiderHoles)
          pSector->EnsureLoaded();

        for (int ry=0;ry<iResY;ry++)
          for (int rx=0;rx<iResX;rx++)
          {
            int x = sx*iResX+rx;
            int y = sy*iResY+ry;
            if (bConsiderHoles)
            {
              int tx = rx*cfg.m_iTilesPerSector[0]/iResX;
              int ty = ry*cfg.m_iTilesPerSector[1]/iResY;
              if (pSector->GetTile(tx,ty)->IsHole())
              {
                pDest[y*lockObj.m_iStride+x] = 0;
                continue;
              }
            }
            int iVal = (int)(filter->GetDensityAt(x,y) * 255.99f);
            if (iVal<0) iVal=0; else if (iVal>255) iVal=255;
            pDest[y*lockObj.m_iStride+x] = iVal;
          }

        if (!bOldLoaded)
          pSector->EnsureUnloaded();
        progress->Percentage = 100.f/(float)iSectorCount*(float)iSectorCounter; 
      }

    m_pTerrain->UnlockDecorationChannel(lockObj,true);

    // re-new the affected area
    filter->DecorationModel->ResetAffectedSectors();
    for (int sy=0;sy<cfg.m_iSectorCount[1];sy++)
      for (int sx=0;sx<cfg.m_iSectorCount[0];sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        pSector->EnsureLoaded();
        VDecorationDensityChannel *pChannel = pSector->m_DecorationChannels.FindModelChannel(pRes);
        if (!pChannel||pChannel->IsBlack())
          continue;
        filter->DecorationModel->AddAffectedSector(sx,sy);
      }

    return true;
  }

  bool EngineInstanceTerrain::GetDecorationModelDensityResolution(DecorationModelResource ^pModel)
  {
    const VTerrainConfig &cfg(m_pTerrain->m_Config);
    pModel->iDensityResX = cfg.m_iDensityMapSamplesPerSector[0];
    pModel->iDensityResY = cfg.m_iDensityMapSamplesPerSector[1];
    return true;
  }

    
  float EngineInstanceTerrain::GetAverageNormalZ(int x, int y, int iTargetResX, int iTargetResY)
  {
    if (!m_pTerrain)
      return 1.0f;

    // map to sample positions
    const VTerrainConfig &cfg(m_pTerrain->m_Config);
    int x1 = x*cfg.m_iHeightSamplesPerSector[0]/iTargetResX;
    int y1 = y*cfg.m_iHeightSamplesPerSector[0]/iTargetResY;
    int x2 = (x+1)*cfg.m_iHeightSamplesPerSector[0]/iTargetResX;
    int y2 = (y+1)*cfg.m_iHeightSamplesPerSector[1]/iTargetResY;
    x2 = hkvMath::Max(x2,x1+1);
    y2 = hkvMath::Max(y2,y1+1);
    hkvVec3 vNrml;
    hkvVec3 vTempNrml(hkvNoInitialization);
    for (int y=y1;y<y2;y++)
      for (int x=x1;x<x2;x++)
      {
        m_pTerrain->m_SectorManager.GetNormalAtSamplePos(x,y,vTempNrml);
        vNrml += vTempNrml;
      }

    if (vNrml.z<HKVMATH_LARGE_EPSILON)
      return 0.f;

    vNrml.normalizeIfNotZero();
    return vNrml.z;
  }


  float EngineInstanceTerrain::GetDetailTextureDensity(DetailTextureResource ^pSrc, int x, int y, int iTargetResX, int iTargetResY)
  {
    if (!m_pTerrain)
      return 0.0f;
    VTextureWeightmapChannelResource *pTexture = m_pTerrain->m_WeightmapChannels.FindByID(pSrc->ID);
    if (!pTexture)
      return 0.0f;

    int sx = x/iTargetResX;
    int sy = y/iTargetResY;
    x -= (sx*iTargetResX); // offset inside sector
    y -= (sy*iTargetResY); // offset inside sector
    VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
    pSector->EnsureLoaded();

    int iIndex = pSector->m_UsedWeightmapChannels.FindChannelIndex(pTexture);
    if (iIndex<0)
      return 0.0f;


    VTextureWeightmapChannel* pThisChannel = pSector->m_UsedWeightmapChannels.GetAt(iIndex);
    if (pThisChannel->IsBlack())
      return 0.0f;

    int x1 = x*pThisChannel->m_iSizeX/iTargetResX;
    int y1 = y*pThisChannel->m_iSizeY/iTargetResY;
    int x2 = (x+1)*pThisChannel->m_iSizeX/iTargetResX;
    int y2 = (y+1)*pThisChannel->m_iSizeY/iTargetResY;
    x2 = hkvMath::Max(x2,x1+1);
    y2 = hkvMath::Max(y2,y1+1);

    int iArea = (x2-x1)*(y2-y1);

    // filter across a full block 
    float fSum = 0.0f;
    for (int y=y1;y<y2;y++)
      for (int x=x1;x<x2;x++)
      {
        float fVal = (float)pThisChannel->GetValue(x,y) * (1.f/255.f);

        // mask out for detail textures that are painted on top
        float fBlend = 1.0f;
        for(int i=iIndex+1;i<pSector->m_UsedWeightmapChannels.Count();i++)
        {
          float fOtherVal = (float)pSector->m_UsedWeightmapChannels.GetAt(i)->GetValue(x,y) * (1.f/255.f);
          fBlend *= (1.f-fOtherVal);
        }
        fSum += fVal*fBlend;
      }

    fSum /= (float)iArea;
    return fSum;
  }

   bool EngineInstanceTerrain::WantsViewUpdate()
   {
     return m_pTerrain!=NULL && m_pTerrain->WantsViewUpdate();
   }

   
  void EngineInstanceTerrain::SelectTerrainBrush(TerrainBrush ^pBrush)
  {
    if (!m_pTerrain)
      return;
    VTextureObject *pTex = NULL;
    if (pBrush)
      pTex = (VTextureObject *)pBrush->NativeTextureObject.ToPointer();
    m_pTerrain->SetCursorTexture(pTex);
  }


  void EngineInstanceTerrain::Update3DCursor(Cursor3DProperties ^pProperties, float fMouseX, float fMouseY)
  {
    m_pTerrain->SetEnable3DCursor(pProperties->Enabled);
    if (!pProperties->Enabled || fMouseX<0.f || fMouseY<0.f)
      return;  

  //VisContextCamera_cl *pCamera = Vision::Contexts.GetMainRenderContext()->GetCamera();
  VisBaseEntity_cl *pCamera =(static_cast<VisionView ^>(EditorManager::ActiveView))->m_pCamera;

  if ( pCamera == NULL )
    return;

    hkvVec3 vPos = pCamera->GetPosition();
    hkvVec3 vEnd;  

  if ( Vision::Contexts.GetCurrentContext()->GetProjectionType() == VIS_PROJECTIONTYPE_ORTHOGRAPHIC )
  {
    hkvVec3 vDir;
    hkvVec3 vStartPos;
    Vision::Contexts.GetCurrentContext()->GetTraceDirFromScreenPos(fMouseX,fMouseY,vDir,EditorManager::Settings->MaxPickingDistance,&vStartPos);

    vPos = vStartPos;
    vEnd = vStartPos;

    vPos.z += EditorManager::Settings->MaxPickingDistance;
    vEnd.z -= EditorManager::Settings->MaxPickingDistance;
  }
  else
  {
    Vision::Contexts.GetCurrentContext()->GetTraceDirFromScreenPos(fMouseX,fMouseY,vEnd,EditorManager::Settings->MaxPickingDistance );
    vEnd += vPos;
  }

    m_pTerrain->SetCursorPosFromTraceResult(vPos,vEnd,pProperties->Radius,pProperties->Rotation);
  }

  Vector3F EngineInstanceTerrain::GetCurrentCursorCenter()
  {
    hkvVec3 p = m_pTerrain->m_vCursorCenter.ToRenderSpace(m_pTerrain->m_Config);
    return Vector3F(p.x,p.y,p.z);
  }


  void EngineInstanceTerrain::BlitHeightBrush(TerrainBrush ^pBrush, HeightmapCursorProperties ^pProperties, HeightmapEditMode_e mode)
  {
    if (!pBrush || pBrush->NativePtr==System::IntPtr::Zero || m_bIsReference)
      return;

    float fWeight = 1.0f;
    if (EditorManager::Tablet->IsAvailable && EditorManager::Tablet->IsActive && EditorManager::Tablet->IsPressed)
      fWeight = EditorManager::Tablet->Pressure;

    VisBitmap_cl *pBmp = (VisBitmap_cl *)pBrush->NativePtr.ToPointer();
    float fInt = pProperties->Intensity * Vision::GetTimer()->GetTimeDifference() * fWeight;

    if (mode==HeightmapEditMode_e::Elevate || mode==HeightmapEditMode_e::Subtract)
      fInt *= EditorManager::Settings->GlobalUnitScaling;

    //m_pTerrain->SetCursorTexture((VTextureObject *)pBrush->NativeTextureObject.ToPointer());
    m_pTerrain->BlitHeightBrush(pBmp,fInt,(VEditableTerrain::HMBlitMode_e)mode,pProperties->AbsoluteHeight);
  }


  void EngineInstanceTerrain::ReplaceDecorationModel(DecorationModelResource ^pModel, IVTerrainDecorationModel *pOldModel, IVTerrainDecorationModel *pNewModel)
  {
    if (pOldModel==pNewModel || m_bIsReference)
      return;
    for (int sy=pModel->_iSectorY1;sy<=pModel->_iSectorY2;sy++)
      for (int sx=pModel->_iSectorX1;sx<=pModel->_iSectorX2;sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bOldLoaded = pSector->IsLoaded();
        pSector->EnsureLoaded();
        VDecorationDensityChannel *pChannel = pSector->m_DecorationChannels.FindModelChannel(pOldModel);
        if (!pChannel)
          continue;

        pOldModel->OnEditorEvent(IVTerrainDecorationModel::SectorDecorationDeleted,m_pTerrain,pSector,NULL);
        pChannel->m_spSource = pNewModel;
        //pSector->UpdateDecoration(true); // will be updated later
        pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
        if (!bOldLoaded)
          pSector->EnsureUnloaded();
      }
  }

  void UpdatePivotHeight(IVTerrainDecorationModel *pNativeModel, float fPivotHeight)
  {
    if (pNativeModel==NULL || !pNativeModel->m_LocalBBox.isValid())
      return;
    float fBoxMin = pNativeModel->m_LocalBBox.m_vMin.z;
    pNativeModel->m_Properties.m_fRelHeight = fBoxMin*fPivotHeight; // 0=pivot
  }
    
  IVTerrainDecorationModel* EngineInstanceTerrain::SetNativeModelProperties(DecorationModelResource ^pModel, IVTerrainDecorationModel *pNativeModel)
  {
    if (m_bIsReference)
      return NULL;

    if (!pModel) // find the other way around
    {
      if (pNativeModel)
        pModel = m_pSectorCallback->GetDecorationModelByID(pNativeModel->m_Properties.m_iModelID);
      if (!pModel)
        return NULL;
    }

    IVTerrainDecorationModel::ModelProperties_t newProps;
    IVTerrainDecorationModel::ModelProperties_t &props = pNativeModel ? pNativeModel->m_Properties : newProps;

    // fill out props here
    props.m_iModelID = pModel->ID;
    props.m_fFarClip = pModel->FarClipDistance;
    props.m_fNearClip = pModel->NearClipDistance;
    props.m_fRelFadeStart = pModel->RelativeFadeOutStart;
    props.m_iVisibleBitmask = (unsigned int)pModel->VisibleBitmask;
    props.m_fAverageScale = pModel->AverageScale;
    props.m_fScaleVariation = pModel->SizeVariation*0.01f; // percentage
    props.m_fRelativeDensity = pModel->Density;
    props.m_fRandomness = pModel->DistributionRandomness;
    props.m_fApplyWind = pModel->ApplyWind*0.01f; // percentage
    props.m_fApplyConstraint = pModel->ApplyConstraint*0.01f; // percentage
    props.m_fCollisionCapsuleRadius = pModel->CollisionCapsuleRadius;

    //props.m_fRelHeight = fBoxMin*pModel->PivotHeight; // set via UpdatePivotHeight
    props.m_eModelFlags = (IVTerrainDecorationModel::ModelFlags_e)pModel->ModelFlags;
    UpdatePivotHeight(pNativeModel, pModel->PivotHeight);

    if (pNativeModel==NULL)
    {
      VString sFilename;
      ConversionUtils::StringToVString(pModel->Filename,sFilename);
      IVTerrainDecorationModel::ModelType_e eForcedType = IVTerrainDecorationModel::AnyType; // no filter

      // first see whether we need to re-create decoration with a new model class (patching filename is not possible)
      IVTerrainDecorationModelPtr spOldModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(pModel->ID);
      if (spOldModel!=NULL)
      {
        bool bSameExtension = VFileHelper::HasExtension(spOldModel->GetFilename(), VFileHelper::GetExtension(sFilename));
        if (!bSameExtension)
        {
          spOldModel->m_bIgnore = true; // mark as not used anymore
          pNativeModel = VTerrainDecorationModelManager::GlobalManager().CreateModel(sFilename,props,true,eForcedType);
          ReplaceDecorationModel(pModel, spOldModel, pNativeModel);
        }
      }

      if (pNativeModel==NULL)
        pNativeModel = VTerrainDecorationModelManager::GlobalManager().CreateModel(sFilename,props,true,eForcedType);
      if (pNativeModel==NULL)
        return NULL;
    }

    // fixup native far clip
    float fNativeFarClip = pNativeModel->GetNativeFarClipDistance();
    if (fNativeFarClip>0.f)
    {
      // since the terrain rendering system does not consider individual scaling, we have get the upper limit by using the maximum possible scale which is:
      float fMaxScale = props.m_fAverageScale * (1.f+props.m_fScaleVariation);
      fNativeFarClip *= fMaxScale;
      props.m_fFarClip = fNativeFarClip;
      pModel->SetFarClipDistanceInternal(fNativeFarClip); // write back to C# properties
    }

    // ensure the LOD versions are there:
    if (pModel && pModel->_LODVersions)
    {
      IVTerrainDecorationModel* pParent = pNativeModel;
      for (int i=0;i<pModel->_LODVersions->Count;i++)
      {
        IVTerrainDecorationModel *pChildModel = SetNativeModelProperties(pModel->_LODVersions[i],NULL);
        pParent->m_pNextLOD = pChildModel;
        pParent = pChildModel;
      }
    }

    VString sLMCasterFilename;
    ConversionUtils::StringToVString(pModel->CustomLightmapCasterMesh,sLMCasterFilename);
    pNativeModel->m_spCustomLightmapMesh = NULL;
    if (!sLMCasterFilename.IsEmpty())
    {
      pNativeModel->m_spCustomLightmapMesh = VMeshManager::GetMeshManager()->LoadDynamicMeshFile(sLMCasterFilename);
    }

    UpdatePivotHeight(pNativeModel, pModel->PivotHeight);

    if (pNativeModel->m_LocalBBox.isValid())
    {
      // find the suggested radius from the smallest absolute component
      float d1 = hkvMath::Min(-pNativeModel->m_LocalBBox.m_vMin.x, -pNativeModel->m_LocalBBox.m_vMin.y);
      float d2 = hkvMath::Min( pNativeModel->m_LocalBBox.m_vMax.x,  pNativeModel->m_LocalBBox.m_vMax.y);
      pModel->SuggestedCapsuleRadius = hkvMath::Max(hkvMath::Min(d1,d2), 0.0f);
    } 
    else
      pModel->SuggestedCapsuleRadius =  0.0f;

    return pNativeModel;
  }


  void EngineInstanceTerrain::SetCurrentVegetationModel(DecorationModelResource ^pModel)
  {
    if (m_bIsReference)
      return;
    V_SAFE_RELEASE(m_pCurrentDecoModel);
    m_pCurrentDecoModel = SetNativeModelProperties(pModel,NULL);
    if (!m_pCurrentDecoModel)
      return;
    m_pCurrentDecoModel->AddRef();
  }

  void EngineInstanceTerrain::BlitVegetationBrush(TerrainBrush ^pBrush, Cursor3DProperties ^pProperties, bool bSubtract)
  {
    if (!m_pCurrentDecoModel || m_bIsReference)
      return;

    float fWeight = 1.0f; 
    if (EditorManager::Tablet && EditorManager::Tablet->IsAvailable && EditorManager::Tablet->IsActive && EditorManager::Tablet->IsPressed)
      fWeight = EditorManager::Tablet->Pressure;

    VisBitmap_cl *pBmp = (VisBitmap_cl *)pBrush->NativePtr.ToPointer();
    float fInt = pProperties->Intensity*Vision::GetTimer()->GetTimeDifference() * 0.1f * fWeight; // tweaked intensity
    //m_pTerrain->SetCursorTexture((VTextureObject *)pBrush->NativeTextureObject.ToPointer());
    m_pTerrain->BlitVegetationMap(m_pCurrentDecoModel,pBmp,fInt,bSubtract);
  }

  void EngineInstanceTerrain::SetPreviewVegetationModelWeightmap(DecorationModelResource ^pModel)
  {
    IVTerrainDecorationModel *pNativeModel = NULL;
    if (pModel)
      pNativeModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(pModel->ID);
    m_pTerrain->SetDecorationModelPreviewWeightmap(pNativeModel);
  }

  VTextureWeightmapChannelResource* EngineInstanceTerrain::SetNativeTextureProperties(DetailTextureResource ^pTexture)
  {
    if (!pTexture || m_bIsReference)
      return NULL;
    VTextureWeightmapChannelResource::DetailTextureProperties_t props;
    TerrainConversionHelpers::DetailTexturePropertiesToNative(&props,pTexture);
    VTextureWeightmapChannelResource *pNativeRes = m_pTerrain->m_WeightmapChannels.CreateDetailTexture(props);
    return pNativeRes;
  }


  void EngineInstanceTerrain::SetCurrentDetailTexture(DetailTextureResource ^pTexture)
  {
    if (m_bIsReference)
      return;
    V_SAFE_RELEASE(m_pCurrentDetailTexture);
    m_pCurrentDetailTexture = SetNativeTextureProperties(pTexture);
    if (!m_pCurrentDetailTexture)
      return;
    m_pCurrentDetailTexture->AddRef();
  }


  void EngineInstanceTerrain::UpdateDecorationModelProperties(DecorationModelResource ^pModel, bool bCreateWhenMissing)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    IVTerrainDecorationModel *pNativeModel = SetNativeModelProperties(pModel,NULL);
    if (!pNativeModel)
      return;

    for (int sy=pModel->_iSectorY1;sy<=pModel->_iSectorY2;sy++)
      for (int sx=pModel->_iSectorX1;sx<=pModel->_iSectorX2;sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bOldLoaded = pSector->IsLoaded();
        pSector->EnsureLoaded();
        VDecorationDensityChannel *pChannel = pSector->m_DecorationChannels.FindModelChannel(pNativeModel);
        if (!pChannel || pChannel->IsBlack())
          continue;

        pSector->UpdateDecoration(true);
        pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
      }
  }

  void EngineInstanceTerrain::DeleteDecorationModel(DecorationModelResource ^pModel)
  {
    if (m_bIsReference)
      return;

    IVTerrainDecorationModel *pNativeModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(pModel->ID);

    // fixup LOD references if this was a LOD version
    if (pNativeModel!=NULL && pModel->_LODOrigin!=nullptr)
    {
      IVTerrainDecorationModel *pParentModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(pModel->_LODOrigin->ID);
      VASSERT(pParentModel!=NULL && pParentModel->m_pNextLOD==pNativeModel);
      if (pParentModel!=NULL)
        pParentModel->m_pNextLOD = pNativeModel->m_pNextLOD; // close linked list
      return; // no need to modify sectors now.  Updating the origin is triggered separately from the editor
    }

    // not undoable yet
    for (int sy=pModel->_iSectorY1;sy<=pModel->_iSectorY2;sy++)
      for (int sx=pModel->_iSectorX1;sx<=pModel->_iSectorX2;sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bOldLoaded = pSector->IsLoaded();
        pSector->EnsureLoaded();
        VDecorationDensityChannel *pChannel = pSector->m_DecorationChannels.FindModelChannelByID(pModel->ID);
        if (pChannel)
        {
          // this event is important for those models, that do not have an instance representation (billboards)
          if (pNativeModel)
            pNativeModel->OnEditorEvent(IVTerrainDecorationModel::SectorDecorationDeleted,m_pTerrain,pSector,NULL);
          pSector->m_DecorationChannels.Remove(pChannel);
          pSector->UpdateDecoration(true);
          pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
        }
        if (!bOldLoaded)
          pSector->EnsureUnloaded();
      }
  }

  void EngineInstanceTerrain::ReloadDecorationModel(DecorationModelResource ^pModel)
  {
    IVTerrainDecorationModel *pNativeModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(pModel->ID);
    if (pNativeModel==NULL)
      return;
    pNativeModel->SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
    pNativeModel->EnsureUnloaded();
    pNativeModel->EnsureLoaded();
    pNativeModel->RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
    m_pTerrain->UpdateDecoration(true);
  }

  void EngineInstanceTerrain::UpdateDecoration(bool bFullTerrain)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    m_pTerrain->UpdateDecoration(bFullTerrain);
  }


  void EngineInstanceTerrain::SynchronizeDecorationModels(ArrayList ^pModels)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    // first, flag all as invalid
    VTerrainDecorationModelManager::GlobalManager().FlagValidState(false);

    // then repair all models that are actually there
    for (int i=0;i<pModels->Count;i++)
    {
      
      DecorationModelResource ^pModel = static_cast<DecorationModelResource ^>(pModels[i]);
      IVTerrainDecorationModel *pNativeModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(pModel->ID);
      if (!pNativeModel) // create?
        continue;

      pNativeModel->m_bValidState = true;
    }
  }


  void EngineInstanceTerrain::BlitDetailTexture(TerrainBrush ^pBrush, Cursor3DProperties ^pProperties, bool bSubtract)
  {
    if (!m_pCurrentDetailTexture || m_bIsReference)
      return;

    float fWeight = 1.0f;
    if (EditorManager::Tablet->IsAvailable && EditorManager::Tablet->IsActive && EditorManager::Tablet->IsPressed)
      fWeight = EditorManager::Tablet->Pressure;
     
    VisBitmap_cl *pBmp = (VisBitmap_cl *)pBrush->NativePtr.ToPointer();
    float fInt = pProperties->Intensity*Vision::GetTimer()->GetTimeDifference()* fWeight;
    //m_pTerrain->SetCursorTexture((VTextureObject *)pBrush->NativeTextureObject.ToPointer());
    m_pTerrain->BlitDetailTexture(m_pCurrentDetailTexture,pBmp,fInt,bSubtract);
  }

  void EngineInstanceTerrain::BlinkDetailTexture(DetailTextureResource ^pTexture, float fDuration)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    VTextureWeightmapChannelResource *pRes = m_pTerrain->m_WeightmapChannels.FindByID(pTexture->ID);
    m_pTerrain->BlinkDetailTexture(pRes,fDuration);
  }

  void EngineInstanceTerrain::SetHighlightMaterialID(int iID)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    m_pTerrain->SetHighlightMaterialID(iID);
  }


  void EngineInstanceTerrain::SetUseBakedTextures(bool bStatus, ShaderEffectConfig^ pFXConfig)
  {
    if (m_pTerrain)
    {
      if (bStatus && pFXConfig->_shadermode==ISurface::SHADERMODE::AUTO)
      {
        m_pTerrain->m_SectorManager.m_spTerrainEffect = NULL;
      }

      m_pTerrain->SetUseBakedTextures(bStatus, !m_bIsReference);

      if (bStatus)
      {
        // get back result for the C# node
        VCompiledEffect* pFX = m_pTerrain->m_SectorManager.GetTerrainEffect();
        ConversionUtils::UpdateShaderEffectConfig(NULL, NULL, pFX, pFXConfig);
      }
    }
  }


  void EngineInstanceTerrain::SetPreviewDirectionalLight(bool bStatus)
  {
    if (m_pTerrain)
      m_pTerrain->SetPreviewDirectionalLight(bStatus);
  }

  void EngineInstanceTerrain::SetDirectionalLightProperties(Vector3F lightDir,Color ambColor, Color dirColor)
  {
    if (!m_pTerrain)
      return;
    hkvVec3 dir(lightDir.X,lightDir.Y,lightDir.Z);
    dir.normalizeIfNotZero();
    hkvVec4 dirCol(dirColor.R,dirColor.G,dirColor.B,dirColor.A);
    hkvVec4 ambCol(ambColor.R,ambColor.G,ambColor.B,ambColor.A);

    dirCol *= (1.f/255.f);
    ambCol *= (1.f/255.f);

    m_pTerrain->SetDirectionalLightProperties(dir,dirCol,ambCol);
  }


  void EngineInstanceTerrain::SetDisplayHoleOverlay(bool bStatus)
  {
    if (m_pTerrain)
      m_pTerrain->m_bPreviewHoleOverlay = bStatus;
  }

  void EngineInstanceTerrain::SetDisplayMiniMap(bool bStatus)
  {
    if (m_pTerrain)
      m_pTerrain->m_bShowMiniMap = bStatus;
  }

  int EngineInstanceTerrain::GetDecorationObjectCount()
  {
    if (!m_pTerrain)
      return 0;

    return m_pTerrain->SectorManager().GetDecorationObjectCount();
  } 

  void EngineInstanceTerrain::SetLODScaling(TerrainLODMetric_e metric, float fValue, float fMirrorBias)
  {
    if (m_pTerrain==NULL)
      return;
    if (!m_bIsReference)
      m_pTerrain->SetLODMetric((VTerrain::VTerrainLODMode_e)metric);

    // these two properties can be set per instance (not stored in the config)
    m_pTerrain->SetLODScaling(fValue);
    m_pTerrain->SetLODMirrorBias(fMirrorBias);
  }

  TerrainLODMetric_e EngineInstanceTerrain::GetLODMetric()
  {
    if (m_pTerrain==NULL)
      return (TerrainLODMetric_e)m_pTerrain->GetLODMetric();
    return TerrainLODMetric_e::DistanceAndNoise;
  }

  void EngineInstanceTerrain::SetShaderEffect(ShaderEffectConfig ^pFXConfig)
  {
    // also m_bIsReference can change it
    if (!m_pTerrain)
      return;

    VCompiledEffect *pOldFX = m_pTerrain->m_SectorManager.m_spTerrainEffect;
    if (pFXConfig->_shadermode==ISurface::SHADERMODE::AUTO)
    {
      m_pTerrain->SetDefaultShaderEffect(NULL);
      // get back result for the C# node
      VCompiledEffect* pFX = m_pTerrain->m_SectorManager.GetTerrainEffect();
      ConversionUtils::UpdateShaderEffectConfig(NULL,NULL,pFX, pFXConfig);
    }
    else
    {
      VCompiledEffect *pFX = ConversionUtils::GetShaderEffect(pFXConfig,true,false);
      m_pTerrain->SetDefaultShaderEffect(pFX);
    }

    VCompiledEffect *pNewFX = m_pTerrain->m_SectorManager.m_spTerrainEffect;
    bool bLayoutChanged = pOldFX!=NULL && pNewFX!=NULL && pOldFX!=pNewFX;
    if (bLayoutChanged && !m_bIsReference)
    {
      const VTerrainConfig &cfg(m_pTerrain->m_Config);
      for (int sy=0;sy<cfg.m_iSectorCount[1];sy++)
        for (int sx=0;sx<cfg.m_iSectorCount[0];sx++)
          m_pTerrain->GetSector(sx,sy)->SetEditorFlag(SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY); // rebind sampler textures
    }
  }
/*
  // this function is called in referenced mode
  ShaderEffectConfig^ EngineInstanceTerrain::GetShaderEffectConfig()
  {
    if (!m_pTerrain)
      return nullptr;

    // in referenced mode, we have to acquire the shader information from any sector and set it globally
    VCompiledEffect *pCurrentFX = m_pTerrain->m_SectorManager.m_spTerrainEffect;
    if (pCurrentFX==NULL)
      return nullptr;
    ShaderEffectConfig ^fxConf = ConversionUtils::GetShaderEffectConfig(pCurrentFX->m_pSourceEffect->m_pOwner, pCurrentFX->m_pSourceEffect, pCurrentFX);
    return fxConf;
  }

*/

  void EngineInstanceTerrain::UpdatePhysicsMeshes()
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    for (int sy=0;sy<m_pTerrain->m_Config.m_iSectorCount[1];sy++)
      for (int sx=0;sx<m_pTerrain->m_Config.m_iSectorCount[0];sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        if (!pSector->IsLoaded())
          continue;

        Vision::GetApplication()->GetPhysicsModule()->OnTerrainSectorRemoved(pSector);
        Vision::GetApplication()->GetPhysicsModule()->OnTerrainSectorCreated(pSector);
      }
  }

  // this function is rather dangerous for large terrains (out of memory). So set bUnload to true
  void EngineInstanceTerrain::EnsureSectorRangeLoaded(int x1,int y1,int x2,int y2, int iAddEditorFlags, bool bUnload, bool bRepair, ProgressStatus ^pProgress)
  {
    int iArea = (x2-x1+1) * (y2-y1+1);
    float fStep = 100.f/(float)iArea;
    float fPercentage = 0.0f;

    for (int sy=y1;sy<=y2;sy++)
      for (int sx=x1;sx<=x2;sx++,fPercentage+=fStep)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bOldLoaded = pSector->IsLoaded();
        pSector->EnsureLoaded();
        if (iAddEditorFlags)
          pSector->SetEditorFlag(iAddEditorFlags);

        if (bRepair)
        {
          pSector->SaveSectorFinal(true);
        }

        if (!bOldLoaded && bUnload)
          pSector->EnsureUnloaded();

        if (pProgress)
          pProgress->Percentage = fPercentage;
      }
  }

  void EngineInstanceTerrain::ReloadSectorRange(int x1,int y1,int x2,int y2, bool bLoadUnloadedSectors, ProgressStatus ^pProgress)
  {
    int iArea = (x2-x1+1) * (y2-y1+1);
    float fStep = 100.f/(float)iArea;
    float fPercentage = 0.0f;

    for (int sy=y1;sy<=y2;sy++)
    {
      for (int sx=x1;sx<=x2;sx++,fPercentage+=fStep)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bIsLoaded = pSector->IsLoaded();

        if (bIsLoaded)
        {
          // In case the sector is dirty it will be written to the temp folder while unloading
          pSector->EnsureUnloaded();
          pSector->EnsureLoaded();
        }
        else if (bLoadUnloadedSectors)
        {
          pSector->EnsureLoaded();
        }

        if (pProgress)
        {
          pProgress->Percentage = fPercentage;
        }
      }
    }
  }

  bool EngineInstanceTerrain::AllSectorsLoaded(int x1,int y1,int x2,int y2)
  {
    for (int sy=y1;sy<=y2;sy++)
      for (int sx=x1;sx<=x2;sx++)
        if (!m_pTerrain->GetSector(sx,sy)->IsLoaded())
          return false;
    return true;
  }


  void EngineInstanceTerrain::UpdateDetailTextureProperties(DetailTextureResource ^pTexture, bool bCreateWhenMissing, ProgressStatus ^pProgress, bool bUpdateMaterialID)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    VTextureWeightmapChannelResource *pRes = m_pTerrain->m_WeightmapChannels.FindByID(pTexture->ID);
    if (!pRes)
    {
      if (bCreateWhenMissing)
      {
        VTextureWeightmapChannelResource::DetailTextureProperties_t props;
        TerrainConversionHelpers::DetailTexturePropertiesToNative(&props,pTexture);
        pRes = m_pTerrain->m_WeightmapChannels.CreateDetailTexture(props);
      }
    }
    else
    {
      TerrainConversionHelpers::DetailTexturePropertiesToNative(&pRes->m_Properties,pTexture);
    }

    static bool bShowAlphaBlendingWarning = true;
    if (pTexture->UseThreeWayMapping && bShowAlphaBlendingWarning)
    {
      for (int i = 0; i < m_pTerrain->m_WeightmapChannels.Count(); i++)
      {
        const VTextureWeightmapChannelResource::DetailTextureProperties_t& props = m_pTerrain->m_WeightmapChannels.GetAt(i)->m_Properties;
        if (props.GetDiffuseTexture(0, 0)->HasAlphaChannel())
        {
          Vision::Error.Warning("A terrain layer uses a texture with an alpha channel. Alpha-Blending will not work when using three-way-mapping.");
          bShowAlphaBlendingWarning = false;
        }
      }
    }

    int x1,y1,x2,y2;
    if (pTexture->_bIsBaseTexture)
    {
      x1=y1=0; // should be same as affected sector range though
      x2=m_pTerrain->m_Config.m_iSectorCount[0]-1;
      y2=m_pTerrain->m_Config.m_iSectorCount[1]-1;
    } 
    else
    {
      // C# holds the affected sector range      
      x1 = pTexture->_iSectorX1;
      y1 = pTexture->_iSectorY1;
      x2 = pTexture->_iSectorX2;
      y2 = pTexture->_iSectorY2;
    }
    
    int iArea = (x2-x1+1) * (y2-y1+1);
    float fStep = 100.f/(float)iArea;
    float fPercentage = 0.0f;

    for (int sy=y1;sy<=y2;sy++)
    {
      for (int sx=x1;sx<=x2;sx++,fPercentage+=fStep)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bOldLoaded = pSector->IsLoaded();
        pSector->EnsureLoaded();
        pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);

        // for the base texture we have to take care that every sector uses at least that texture:
        if (pTexture->_bIsBaseTexture && !pSector->m_UsedWeightmapChannels.FindChannel(pRes))
        {
          // create detail texture:
          int iResX = m_pTerrain->m_Config.m_iDefaultWeightmapResolution[0]; // this might become sector specific
          int iResY = m_pTerrain->m_Config.m_iDefaultWeightmapResolution[1];
          VTextureWeightmapChannel *pChannel = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pRes,iResX,iResY);
          pChannel->Clear(255);
        }

        if (bOldLoaded)
        {
          // Update the per-sector texture resources for sectors that were loaded before and thus 
          // remain loaded. This is necessary to see the changes after updating a split texture
          // when only the file contents, but not the file name did change.
          const int numWeightmaps = pSector->m_UsedWeightmapChannels.Count();
          for (int weightmapIdx = 0; weightmapIdx < numWeightmaps; ++weightmapIdx)
          {
            VTextureWeightmapChannel* pChannel = pSector->m_UsedWeightmapChannels.GetAt(weightmapIdx);
            if (pChannel == NULL || pChannel->m_spResource == NULL)
              continue;
            VTextureWeightmapChannelResource::DetailTextureProperties_t& channelProps = 
              pChannel->m_spResource->m_Properties;

            VTextureObject* pDiffuseTexture = channelProps.GetDiffuseTexture(pSector->m_iIndexX, pSector->m_iIndexY);
            if (pDiffuseTexture != NULL)
              pDiffuseTexture->CheckFileModified(Vision::File.GetManager());

            VTextureObject* pNormalmapTexture = channelProps.GetNormalmapTexture(pSector->m_iIndexX, pSector->m_iIndexY);
            if (pNormalmapTexture != NULL)
              pNormalmapTexture->CheckFileModified(Vision::File.GetManager());

            VTextureObject* pSpecularTexture = channelProps.GetSpecularTexture(pSector->m_iIndexX, pSector->m_iIndexY);
            if (pSpecularTexture != NULL)
              pSpecularTexture->CheckFileModified(Vision::File.GetManager());
          }
        }

        // unload the sector again, otherwise we easily run out of memory
        if (!bOldLoaded)
          pSector->EnsureUnloaded();

        if (pProgress)
          pProgress->Percentage = fPercentage;

        if (bUpdateMaterialID)
          pSector->UpdateMaterialID();
      }
      Vision::ResourceSystem.UpdateResourceViewer();
    }
    
    // Update terrain config
    m_pTerrain->UpdateDefaultEffectSettingsInConfig();
  }

  // this function covers all combinations of added textures, removed textures and order change
  void EngineInstanceTerrain::UpdateDetailTextureOrder(ArrayList ^list, bool bModifySectors, ProgressStatus ^pProgress)
  {
    if (!m_pTerrain || m_bIsReference)
      return;

    for (int i=0;i<m_pTerrain->m_WeightmapChannels.Count();i++)
      m_pTerrain->m_WeightmapChannels.GetAt(i)->m_iSortingKey = -1; // flag for removal

    for (int i=0;i<list->Count;i++)
    {
      DetailTextureResource ^pTexture = static_cast<DetailTextureResource ^>(list[i]);

      VTextureWeightmapChannelResource::DetailTextureProperties_t props;
      TerrainConversionHelpers::DetailTexturePropertiesToNative(&props,pTexture);
      VTextureWeightmapChannelResource *pRes = m_pTerrain->m_WeightmapChannels.CreateDetailTexture(props);
      pRes->m_iSortingKey = i; // mark as used and set order
    }

    m_pTerrain->m_WeightmapChannels.SortByResourceSortingKey();
    VTextureWeightmapChannelResource **pSorted = m_pTerrain->m_WeightmapChannels.GetPtrs(); 

    if (bModifySectors)
    {
      int x1,y1,x2,y2;
      x1=y1=0;
      x2=m_pTerrain->m_Config.m_iSectorCount[0]-1;
      y2=m_pTerrain->m_Config.m_iSectorCount[1]-1;
      int iArea = (x2-x1+1) * (y2-y1+1);
      float fStep = 100.f/(float)iArea;
      float fPercentage = 0.0f;

      for (int sy=y1;sy<=y2;sy++)
        for (int sx=x1;sx<=x2;sx++,fPercentage+=fStep)
        {
          VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
          BOOL bOldLoaded = pSector->IsLoaded();
          pSector->EnsureLoaded();

          pSector->m_UsedWeightmapChannels.SortByResourceSortingKey(true); // this also removes channels with resource flagged for removal
          pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);

          // unload the sector again, otherwise we easily run out of memory
          if (!bOldLoaded)
            pSector->EnsureUnloaded();

          if (pProgress)
            pProgress->Percentage = fPercentage;

        }
    }


    // hide unused resources
    for (int i=0;i<m_pTerrain->m_WeightmapChannels.Count();i++)
      m_pTerrain->m_WeightmapChannels.GetAt(i)->SetHidden(m_pTerrain->m_WeightmapChannels.GetAt(i)->m_iSortingKey == -1);

  }


  void EngineInstanceTerrain::DeleteDetailTexture(DetailTextureResource ^pTexture, ProgressStatus ^pProgress)
  {
    if (!m_pTerrain || m_bIsReference)
      return;
    VTextureWeightmapChannelResource *pRes = m_pTerrain->m_WeightmapChannels.FindByID(pTexture->ID);
    if (!pRes)
      return;

/*
    for (int sy=pTexture->_iSectorY1;sy<=pTexture->_iSectorY2;sy++)
      for (int sx=pTexture->_iSectorX1;sx<=pTexture->_iSectorX2;sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        pSector->EnsureLoaded();
        pSector->m_UsedWeightmaps.RemoveChannel(pRes); // note that this is not undoable
      }
*/
    pRes->SetHidden(true);
    //m_pTerrain->m_WeightmapChannels.Remove(pRes);

    // C# holds the affected sector range
    // Reloading should be enough as it discards the resources that are flagged for removal
    EnsureSectorRangeLoaded(
      pTexture->_iSectorX1, pTexture->_iSectorY1, pTexture->_iSectorX2, pTexture->_iSectorY2, 
      SECTOR_EDITORFLAG_SECTORFILEDIRTY | SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY,
      true, false, pProgress);
  
    // Update terrain config
    m_pTerrain->UpdateDefaultEffectSettingsInConfig();
  }


  void EngineInstanceTerrain::SetLightmapPageSize(int iSize,bool bPerSector)
  {
    if (m_pTerrain)
    {
      m_pTerrain->SetLightmapPageSize(iSize,bPerSector);
      if (!m_bIsReference)
        m_pTerrain->UpdateDefaultEffectSettingsInConfig();
    }
  }

  void EngineInstanceTerrain::SetFullbrightLightmaps()
  {
    if (m_pTerrain)
      m_pTerrain->SetFullbrightLightmaps();
  }

  void EngineInstanceTerrain::SetCastStaticShadows(bool bStatus)
  {
    if (m_pTerrain)
      m_pTerrain->SetCastStaticShadows(bStatus);
  }

  void EngineInstanceTerrain::SetLightInfluenceBitmask(unsigned short iMask)
  {
    if (m_pTerrain)
      m_pTerrain->SetLightInfluenceBitmask(iMask);
  }

  void EngineInstanceTerrain::OnSectorUnloaded(int x, int y)
  {
    m_pSectorCallback->OnSectorChanged(SectorAction::Unloaded,x,y);
  }

  void EngineInstanceTerrain::OnSectorReloaded(int x, int y)
  {
    m_pSectorCallback->OnSectorChanged(SectorAction::Reloaded,x,y);
  }

  void EngineInstanceTerrain::OnDecorationDirtyRegionChanged()
  {
    m_iDirtyDecorationSectorCount = m_pTerrain ? m_pTerrain->GetDirtySectorCount() : 0;
    m_pSectorCallback->OnDirtyDecorationRegionChanged(m_iDirtyDecorationSectorCount);
  }

  void EngineInstanceTerrain::SetComponentUpdateThroughUI(bool bStatus)
  {
    // we must differentiate between components that come from file and components that are added later through UI
    VMeshPaintingComponent::g_bComponentsThroughUI = bStatus;
  }

  
  void EngineInstanceTerrain::CreateSectorMapPreview(int x, int y, BitmapData ^pLockData)
  {
    if (!m_pTerrain)
      return;
    VEditableTerrainSector *pSector = m_pTerrain->GetSector(x,y);
    if (!pSector->IsLoaded())
      return;

    hkvVec3 vDirX(m_pTerrain->m_Config.m_vSampleSpacing.x,0.f,0.f);
    hkvVec3 vDirY(0.f,m_pTerrain->m_Config.m_vSampleSpacing.y,0.f);

    hkvVec3 vLightDir(1.f,1.f,1.f);
    vLightDir.normalizeIfNotZero();

    VColorRef *pDest = (VColorRef *)pLockData->Scan0.ToPointer();
    for (int y = 0;y<pLockData->Height;y++, pDest += pLockData->Stride/4)
      for (int x = 0;x<pLockData->Width;x++)
      {
        int sx = x * m_pTerrain->m_Config.m_iHeightSamplesPerSector[0] / pLockData->Width;
        int sy = y * m_pTerrain->m_Config.m_iHeightSamplesPerSector[1] / pLockData->Height;

        float fH = pSector->GetHeightAt(sx,sy);
        vDirX.z = pSector->GetHeightAt(sx+1,sy)-fH;
        vDirY.z = pSector->GetHeightAt(sx,sy+1)-fH;
        hkvVec3 vNrml = vDirX.cross(vDirY);
        vNrml.normalizeIfNotZero();
        float fDot = vNrml.dot (vLightDir);
        int its = (int)(fDot*256.f);
        if (its>255)
          its=255;
        else if (its<40)
          its=40;
        pDest[x].SetRGBA(its,its,its,255);
      }
  }


  String^ EngineInstanceTerrain::QueryTextureSwizzleMask(String^ filename)
  {
    VString sFilename;
    ConversionUtils::StringToVString(filename,sFilename);
    VisBitmap_cl *pBmp = VisBitmap_cl::LoadBitmapFromFile(sFilename);
    if (!pBmp || !pBmp->IsLoaded())
      return "";

    return ConversionUtils::VStringToString(pBmp->GetSwizzleMask());
  }

  void EngineInstanceTerrain::ImportWeightmap(String ^filename, DetailTextureResource ^pDestR, DetailTextureResource ^pDestG, DetailTextureResource ^pDestB, DetailTextureResource ^pDestA, ProgressStatus ^pProgress)
  {
    if (!m_pTerrain || m_bIsReference)
      return;

    VString sFilename;
    ConversionUtils::StringToVString(filename,sFilename);
    VisBitmap_cl *pBmp = VisBitmap_cl::LoadBitmapFromFile(sFilename);
    if (!pBmp || !pBmp->IsLoaded())
      return;

    pBmp->CheckFileModified(Vision::File.GetManager()); // in case the bitmap has changed
    VTextureWeightmapChannelResource *pResR = pDestR ? m_pTerrain->m_WeightmapChannels.FindByID(pDestR->ID) : NULL;
    VTextureWeightmapChannelResource *pResG = pDestG ? m_pTerrain->m_WeightmapChannels.FindByID(pDestG->ID) : NULL;
    VTextureWeightmapChannelResource *pResB = pDestB ? m_pTerrain->m_WeightmapChannels.FindByID(pDestB->ID) : NULL;
    VTextureWeightmapChannelResource *pResA = pDestA ? m_pTerrain->m_WeightmapChannels.FindByID(pDestA->ID) : NULL;

    hkvVec4 vCol;
    float fInvX = 1.f/(float)m_pTerrain->m_Config.m_iSectorCount[0];
    float fInvY = 1.f/(float)m_pTerrain->m_Config.m_iSectorCount[1];
    int iResX = m_pTerrain->m_Config.m_iDefaultWeightmapResolution[0]; // this might become sector specific
    int iResY = m_pTerrain->m_Config.m_iDefaultWeightmapResolution[1];

    float fPercentageStep = 100.0f / (m_pTerrain->m_Config.m_iSectorCount[0]*m_pTerrain->m_Config.m_iSectorCount[1]*iResY);
    float fPercentage = 0.f;

    for (int sy=0;sy<m_pTerrain->m_Config.m_iSectorCount[1];sy++)
      for (int sx=0;sx<m_pTerrain->m_Config.m_iSectorCount[0];sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        pSector->EnsureLoaded();
        VTextureWeightmapChannel *pChannelR = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pResR,iResX,iResY);
        VTextureWeightmapChannel *pChannelG = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pResG,iResX,iResY);
        VTextureWeightmapChannel *pChannelB = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pResB,iResX,iResY);
        VTextureWeightmapChannel *pChannelA = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pResA,iResX,iResY);

        UBYTE *pR = pChannelR ? pChannelR->GetValuePtr(0,0): NULL;
        UBYTE *pG = pChannelG ? pChannelG->GetValuePtr(0,0): NULL;
        UBYTE *pB = pChannelB ? pChannelB->GetValuePtr(0,0): NULL;
        UBYTE *pA = pChannelA ? pChannelA->GetValuePtr(0,0): NULL;

        bool bSetR = false;
        bool bSetG = false;
        bool bSetB = false;
        bool bSetA = false;
        for (int y=0;y<iResY;y++,fPercentage+=fPercentageStep)
        {
          for (int x=0;x<iResX;x++,pR++,pG++,pB++,pA++)
          {
            float u = ((float)sx+x/(float)iResX)*fInvX;
            float v = ((float)sy+(y+1)/(float)iResY)*fInvY;
            pBmp->LookupTexelColor(vCol,u,1.0f-v);
            if (pChannelR) {*pR = (int)vCol.r; if (*pR) bSetR=true;}
            if (pChannelG) {*pG = (int)vCol.g; if (*pG) bSetG=true;}
            if (pChannelB) {*pB = (int)vCol.b; if (*pB) bSetB=true;}
            if (pChannelA) {*pA = (int)vCol.a; if (*pA) bSetA=true;}
          }
          pProgress->Percentage = fPercentage;
        }

        if (pChannelR) pChannelR->SetDirtyFlag();
        if (pChannelG) pChannelG->SetDirtyFlag();
        if (pChannelB) pChannelB->SetDirtyFlag();
        if (pChannelA) pChannelA->SetDirtyFlag();
        if (bSetR) pDestR->AddAffectedSector(sx,sy);
        if (bSetG) pDestG->AddAffectedSector(sx,sy);
        if (bSetB) pDestB->AddAffectedSector(sx,sy);
        if (bSetA) pDestA->AddAffectedSector(sx,sy);
        pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);

      }

  }

   
  void EngineInstanceTerrain::DetailTexturesFromSlope(String ^filename, DetailTextureResource ^pDestR, DetailTextureResource ^pDestG, DetailTextureResource ^pDestB, DetailTextureResource ^pDestA, float fNormalNoise, ProgressStatus ^pProgress)
  {
    if (!m_pTerrain || m_bIsReference)
      return;

    VString sFilename;
    ConversionUtils::StringToVString(filename,sFilename);
    VisBitmap_cl *pSlopeBmp = VisBitmap_cl::LoadBitmapFromFile(sFilename);
    if (!pSlopeBmp)
      return;

    pSlopeBmp->EnsureLoaded();
    if (!pSlopeBmp->IsLoaded())
      return;

    VTextureWeightmapChannelResource *pResR = pDestR ? m_pTerrain->m_WeightmapChannels.FindByID(pDestR->ID) : NULL;
    VTextureWeightmapChannelResource *pResG = pDestG ? m_pTerrain->m_WeightmapChannels.FindByID(pDestG->ID) : NULL;
    VTextureWeightmapChannelResource *pResB = pDestB ? m_pTerrain->m_WeightmapChannels.FindByID(pDestB->ID) : NULL;
    VTextureWeightmapChannelResource *pResA = pDestA ? m_pTerrain->m_WeightmapChannels.FindByID(pDestA->ID) : NULL;
    hkvVec4 vCol;
    float fInvX = 1.f/(float)m_pTerrain->m_Config.m_iSectorCount[0];
    float fInvY = 1.f/(float)m_pTerrain->m_Config.m_iSectorCount[1];
    int iResX = m_pTerrain->m_Config.m_iDefaultWeightmapResolution[0]; // this might become sector specific
    int iResY = m_pTerrain->m_Config.m_iDefaultWeightmapResolution[1];

    float fPercentageStep = 100.0f / (m_pTerrain->m_Config.m_iSectorCount[0]*m_pTerrain->m_Config.m_iSectorCount[1]*iResY);
    float fPercentage = 0.f;

    for (int sy=0;sy<m_pTerrain->m_Config.m_iSectorCount[1];sy++)
      for (int sx=0;sx<m_pTerrain->m_Config.m_iSectorCount[0];sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        pSector->EnsureLoaded();
        pSector->AddLock();
        VTextureWeightmapChannel *pChannelR = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pResR,iResX,iResY);
        VTextureWeightmapChannel *pChannelG = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pResG,iResX,iResY);
        VTextureWeightmapChannel *pChannelB = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pResB,iResX,iResY);
        VTextureWeightmapChannel *pChannelA = pSector->m_UsedWeightmapChannels.CreateChannel(m_pTerrain->m_Config,pResA,iResX,iResY);

        UBYTE *pR = pChannelR ? pChannelR->GetValuePtr(0,0): NULL;
        UBYTE *pG = pChannelG ? pChannelG->GetValuePtr(0,0): NULL;
        UBYTE *pB = pChannelB ? pChannelB->GetValuePtr(0,0): NULL;
        UBYTE *pA = pChannelA ? pChannelA->GetValuePtr(0,0): NULL;

        // find out the minimum index
        int iMinIndex = 10000000;
        if (pChannelR) iMinIndex = hkvMath::Min(pSector->m_UsedWeightmapChannels.Find(pChannelR),iMinIndex);
        if (pChannelG) iMinIndex = hkvMath::Min(pSector->m_UsedWeightmapChannels.Find(pChannelG),iMinIndex);
        if (pChannelB) iMinIndex = hkvMath::Min(pSector->m_UsedWeightmapChannels.Find(pChannelB),iMinIndex);
        if (pChannelA) iMinIndex = hkvMath::Min(pSector->m_UsedWeightmapChannels.Find(pChannelA),iMinIndex);

        const float fSpacingX = m_pTerrain->m_Config.m_vSectorSize.x / (float)iResX;
        const float fSpacingY = m_pTerrain->m_Config.m_vSectorSize.y / (float)iResY;
        bool bSetR = false;
        bool bSetG = false;
        bool bSetB = false;
        bool bSetA = false;

        for (int y=0;y<iResY;y++,fPercentage+=fPercentageStep)
        {
          for (int x=0;x<iResX;x++,pR++,pG++,pB++,pA++)
          {
            hkvVec3 vRelPos((float)x*fSpacingX,(float)y*fSpacingY,0.0f);
            VLargePosition vPos(sx,sy,vRelPos);
            hkvVec3 vNrml = m_pTerrain->m_SectorManager.GetNormalAtWorldPos(vPos);
            if (fNormalNoise!=0.0f)
              vNrml.z += VTerrainSector::Noise2D(sx*iResX+x,sy*iResY+y)*fNormalNoise;
            pSlopeBmp->LookupTexelColor(vCol,1.f - vNrml.z,0.f);
            if (pChannelR) {*pR = (int)vCol.r; if (*pR) bSetR=true;}
            if (pChannelG) {*pG = (int)vCol.g; if (*pG) bSetG=true;}
            if (pChannelB) {*pB = (int)vCol.b; if (*pB) bSetB=true;}
            if (pChannelA) {*pA = (int)vCol.a; if (*pA) bSetA=true;}
            
          }
          if ((fPercentage-pProgress->Percentage)>0.5f)
            pProgress->Percentage = fPercentage;
        }

        // all other layers (starting from minindex) should be cleared, otherwise they might cover our result
        for (int i=iMinIndex+1;i<pSector->m_UsedWeightmapChannels.Count();i++)
        {
          VTextureWeightmapChannel *pChannel = pSector->m_UsedWeightmapChannels.GetAt(i);
          if (pChannel==pChannelR || pChannel==pChannelG || pChannel==pChannelB || pChannel==pChannelA)
            continue;
          pSector->m_UsedWeightmapChannels.RemoveAt(i);
          i--;
        }

        pSector->ReleaseLock();
        if (pChannelR) pChannelR->SetDirtyFlag();
        if (pChannelG) pChannelG->SetDirtyFlag();
        if (pChannelB) pChannelB->SetDirtyFlag();
        if (pChannelA) pChannelA->SetDirtyFlag();
        if (bSetR) pDestR->AddAffectedSector(sx,sy);
        if (bSetG) pDestG->AddAffectedSector(sx,sy);
        if (bSetB) pDestB->AddAffectedSector(sx,sy);
        if (bSetA) pDestA->AddAffectedSector(sx,sy);
        pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY|SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);

      }

  }
  
  
  void EngineInstanceTerrain::PurgeAll()
  {
    VTerrainManager &tm = VTerrainManager::GlobalManager();
    for (int i=0;i<tm.GetResourceCount();i++)
    {
      VTerrain *pTerrain = (VTerrain *)tm.GetResourceByIndex(i);
      if (!pTerrain)
        continue;
      pTerrain->m_SectorManager.UnloadUnusedResources(1.f,false);
    }

  }

  String ^ EngineInstanceTerrain::SplitPerSectorTexture(String ^filename, String ^*targetname, int *piSize, bool bAddBorder)
  {
    if (!m_pTerrain)
      return "Terrain not available";
    if (m_bIsReference)
      return "Terrain is a reference and cannot be modified";
    VTerrainConfig &cfg = m_pTerrain->m_Config;
    char szTargetDir[FS_MAX_PATH];
    if (!cfg.GetAuxiliaryTextureDir(szTargetDir,false))
      return "Terrain auxiliary texture directory not available";

    VString sFilename;
    ConversionUtils::StringToVString(filename,sFilename);

    VisBitmapPtr spLargeBitmap = new VisBitmap_cl(sFilename);
    spLargeBitmap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    spLargeBitmap->EnsureLoaded();
    if (!spLargeBitmap->IsLoaded())
      return "Failed to load source bitmap";

    int iSizeX = spLargeBitmap->GetWidth();
    int iSizeY = spLargeBitmap->GetHeight();
    int iSmallX = iSizeX / cfg.m_iSectorCount[0];
    int iSmallY = iSizeY / cfg.m_iSectorCount[1];
    int iBorder = bAddBorder ? 1 : 0;
    int iSizeSX = iSmallX + iBorder*2;
    int iSizeSY = iSmallY + iBorder*2;
    if (piSize)
    {
      piSize[0] = iSizeSX; 
      piSize[1] = iSizeSY; 
    }

    if ((iSizeX%cfg.m_iSectorCount[0])!=0 || (iSizeY%cfg.m_iSectorCount[1])!=0)
      return "Size of source texture is not a multiple of sector count";
    if (bAddBorder)
    {
      if (!hkvMath::isPowerOf2 (iSizeSX) || !hkvMath::isPowerOf2(iSizeSY))
        return "Target per sector texture size plus border is not a power of two size";
    } else
    {
    if (!hkvMath::isPowerOf2(iSmallX) || !hkvMath::isPowerOf2(iSmallY))
        return "Target per sector texture size is not a power of two size";
    }

    // flip source image vertically:
    int iHalfY = iSizeY/2;
    for (int y=0;y<iHalfY;y++)
    {
      VColorRef *pRow1 = spLargeBitmap->GetDataPtr(0,y);
      VColorRef *pRow2 = spLargeBitmap->GetDataPtr(0,iSizeY-1-y);
      int iHalfX = iSizeX/2;
      for (int x=0;x<iSizeX;x++)
      {
        VColorRef m = pRow1[x];
        pRow1[x] = pRow2[x];
        pRow2[x] = m;
      }
    }

    char szError[FS_MAX_PATH+128];
    char szTemp[FS_MAX_PATH];
    char szBaseName[FS_MAX_PATH];
    VFileHelper::GetFilenameNoExt(szTemp,VFileHelper::GetFilename(sFilename));
    VFileHelper::CombineDirAndFile(szBaseName,szTargetDir,szTemp);

    VisBitmapPtr spSmallBitmap = new VisBitmap_cl(NULL,iSizeSX,iSizeSY);
    spSmallBitmap->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

    for (int sy=0;sy<cfg.m_iSectorCount[1];sy++)
      for (int sx=0;sx<cfg.m_iSectorCount[0];sx++)
      {
        for (int y=0;y<iSizeSY;y++)
          for (int x=0;x<iSizeSX;x++)
        {
            VColorRef iColor = spLargeBitmap->LookupTexelColor(sx*iSmallX+x-iBorder,sy*iSmallY+y-iBorder, BITMAPLOOKUPFLAG_NONE); // clamp
            VColorRef *pDst = spSmallBitmap->GetDataPtr(x,y);
            *pDst = iColor;
        }

        char szTargetName[FS_MAX_PATH];
        sprintf(szTargetName,"%s_%02i_%02i.dds",szBaseName,sx,sy);
        if (!spSmallBitmap->SaveToFile(szTargetName))
        {
          sprintf(szError,"Failed to save '%s'",szTargetName);
          return ConversionUtils::VStringToString(szError);
        }

      }

     
    // modify texture properties
    if (VFileHelper::IsAbsolutePath(szBaseName))
      *targetname = EditorManager::Project->MakeRelative(ConversionUtils::VStringToString(szBaseName));
    else
      *targetname = ConversionUtils::VStringToString(szBaseName);

    return nullptr;
  }



  void EngineInstanceTerrain::SetMinimapTexture(String ^filename)
  {
    if (!m_pTerrain)
      return;
    VString sFilename;
    ConversionUtils::StringToVString(filename,sFilename);
    VTextureObject *pMiniMap = NULL;
    if (!sFilename.IsEmpty())
      pMiniMap = Vision::TextureManager.Load2DTexture(sFilename);
    m_pTerrain->SetMinimapTexture(pMiniMap);
    
  }


  String ^ EngineInstanceTerrain::SplitPerSectorTextures(String ^diffuse,String ^normal,String ^specular, bool bAddBorder, DetailTextureResource ^pTarget,ProgressStatus ^pProgress)
  {
    if (!m_pTerrain)
      return "Terrain not available";
    if (m_bIsReference)
      return "Terrain is a reference and cannot be modified";

    String ^name = nullptr;
    VTerrainConfig &cfg = m_pTerrain->m_Config;

    int iDiffuseSize[2] = {4,4};

    // split diffuse texture
    if (!String::IsNullOrEmpty(diffuse))
    {
      String ^error = SplitPerSectorTexture(diffuse,&name,iDiffuseSize,bAddBorder);
      if (error!=nullptr)
        return error;
      pTarget->DiffuseFilename = String::Format("{0}_%02i_%02i.dds",name);
    }

    // split diffuse texture
    if (!String::IsNullOrEmpty(normal))
    {
      String ^error = SplitPerSectorTexture(normal,&name,NULL,bAddBorder);
      if (error!=nullptr)
        return error;
      pTarget->NormalmapFilename = String::Format("{0}_%02i_%02i.dds",name);
    }

    // split diffuse texture
    if (!String::IsNullOrEmpty(specular))
    {
      String ^error = SplitPerSectorTexture(specular,&name,NULL,bAddBorder);
      if (error!=nullptr)
        return error;
      pTarget->SpecularmapFilename = String::Format("{0}_%02i_%02i.dds",name);
    }

    pTarget->TilingOffsetX = 0.f;
    pTarget->TilingOffsetY = 0.f;
    pTarget->TilingX = (float)cfg.m_iSectorCount[0];
    pTarget->TilingY = (float)cfg.m_iSectorCount[1];

    if (bAddBorder)
    {
      float fTexSizeX = (float)iDiffuseSize[0];
      float fTexSizeY = (float)iDiffuseSize[1];
      pTarget->TilingX *= (fTexSizeX-2.f) / fTexSizeX;
      pTarget->TilingY *= (fTexSizeY-2.f) / fTexSizeY;
      pTarget->TilingOffsetX = 1.f / fTexSizeX;
      pTarget->TilingOffsetY = 1.f / fTexSizeY;
    }

    pTarget->_bUseClamping = true; // should be set already by DiffuseFilename setter

    UpdateDetailTextureProperties(pTarget,true,pProgress,false);

    return nullptr;
  }


  bool EngineInstanceTerrain::ExportHeightmapAsDDS(String ^filename, bool bNormalize)
  {
    if (!m_pTerrain)
      return false;

    const VTerrainConfig &cfg(m_pTerrain->m_Config);
    int iSizeX = cfg.m_iOverallHeightSampleCount[0];
    int iSizeY = cfg.m_iOverallHeightSampleCount[1];
    VTerrainLockObject lockObj(0,0,iSizeX,iSizeY, VIS_LOCKFLAG_READONLY);

    float *pNormalized = NULL;

    bool bResult = false;
    m_pTerrain->LockHeightValues(lockObj);

    float *pData = (float *)lockObj.GetData();
    int iStride = lockObj.m_iStride;
    if (bNormalize)
    {
      iStride = iSizeX;
      pNormalized = new float[iSizeX*iSizeY];
      float fMin = FLT_MAX;
      float fMax = -FLT_MAX;
      float *src = pData;
      for (int y=0;y<iSizeY;y++)
        for (int x=0;x<iSizeX;x++,src++)
        {
          fMin = hkvMath::Min(fMin,*src);
          fMax = hkvMath::Max(fMax,*src);
        }
      float fScale = 0.f;
      if (fMax>fMin)
        fScale = 1.f/(fMax-fMin);

      src = pData;
      float *tgt = pNormalized;
      for (int y=0;y<iSizeY;y++)
        for (int x=0;x<iSizeX;x++,src++,tgt++)
          *tgt = (*src - fMin) * fScale;

      pData = pNormalized;
    }

    if (pData!=NULL)
      bResult = DDSSaver::SaveDDS(filename,VTextureLoader::R32F,iSizeX,-iSizeY,iStride*sizeof(float),pData);
    m_pTerrain->UnlockHeightValues(lockObj);

    V_SAFE_DELETE_ARRAY(pNormalized);
    return bResult;
  }

  bool EngineInstanceTerrain::ExportDetailTextureAsDDS(String ^filename, DetailTextureResource ^pTexture)
  {
    if (!m_pTerrain)
      return false;
    VTextureWeightmapChannelResource *pNativeTex = m_pTerrain->m_WeightmapChannels.FindByID(pTexture->ID);
    const VTerrainConfig &cfg(m_pTerrain->m_Config);
    int iSizeX = cfg.m_iDefaultWeightmapResolution[0] * cfg.m_iSectorCount[0];
    int iSizeY = cfg.m_iDefaultWeightmapResolution[1] * cfg.m_iSectorCount[1];
    VTerrainLockObject lockObj(0,0,iSizeX,iSizeY, VIS_LOCKFLAG_READONLY);

    bool bResult = false;
    m_pTerrain->LockDetailTexture(lockObj,pNativeTex); // works for pNativeTex==NULL as well
    if (lockObj.GetData()!=NULL) // flip vertically
      bResult = DDSSaver::SaveDDS(filename,VTextureLoader::L8,iSizeX,-iSizeY,lockObj.m_iStride*sizeof(UBYTE),lockObj.GetData());
    m_pTerrain->UnlockDetailTexture(lockObj);

    return true;
  }

  bool EngineInstanceTerrain::ExportDensityMapAsDDS(String ^filename, DecorationModelResource ^pModel)
  {
    if (!m_pTerrain)
      return false;
    IVTerrainDecorationModel *pNativeModel = VTerrainDecorationModelManager::GlobalManager().FindModelByID(pModel->ID);
    const VTerrainConfig &cfg(m_pTerrain->m_Config);
    int iSizeX = cfg.m_iDensityMapSamplesPerSector[0] * cfg.m_iSectorCount[0];
    int iSizeY = cfg.m_iDensityMapSamplesPerSector[1] * cfg.m_iSectorCount[1];
    VTerrainLockObject lockObj(0,0,iSizeX,iSizeY, VIS_LOCKFLAG_READONLY);

    bool bResult = false;
    m_pTerrain->LockDecorationChannel(lockObj,pNativeModel); // works for pNativeModel==NULL as well
    if (lockObj.GetData()!=NULL)
      bResult = DDSSaver::SaveDDS(filename,VTextureLoader::L8,iSizeX,-iSizeY,lockObj.m_iStride*sizeof(UBYTE),lockObj.GetData());
    m_pTerrain->UnlockDecorationChannel(lockObj,false);

    return true;
  }

  ITerrainClipboardObject ^ EngineInstanceTerrain::CopySelection(TerrainSelection ^pSelection)
  {
    if (pSelection==nullptr || !pSelection->Valid || m_pTerrain==NULL)
      return nullptr;
    // get relevant sector range
    const VTerrainConfig &cfg(m_pTerrain->m_Config);
    hkvVec3 vMin(pSelection->WorldSpaceExtent->X1, pSelection->WorldSpaceExtent->Y1,-1.f);
    hkvVec3 vMax(pSelection->WorldSpaceExtent->X2, pSelection->WorldSpaceExtent->Y2, 1.f);
    VLargeBoundingBox bbox(cfg,vMin,vMax);

    int sx1,sy1,sx2,sy2;
    bbox.GetSectorIndices_Clamped(cfg,sx1,sy1,sx2,sy2);

    TerrainClipboardObject ^ obj = gcnew TerrainClipboardObject(nullptr, "Terrain Selection");
    //obj->m_pTerrain = this;
    obj->OriginalExtent = (Rectangle2D ^)pSelection->WorldSpaceExtent->Clone();

    // evaluates which textures and models are to be copied
    VRefCountedCollection<VTextureWeightmapChannelResource> UsedTextures;
    VRefCountedCollection<IVTerrainDecorationModel> UsedModels;

    // analyse what is in the sectors
    for (int sy=sy1;sy<=sy2;sy++)
      for (int sx=sx1;sx<=sx2;sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        BOOL bOldLoaded = pSector->IsLoaded();
        pSector->EnsureLoaded();

        // all weightmaps
        if ((int)pSelection->SelectionFilter & (int)TerrainSelection::SelectionFilter_e::Textures)
          for (int i=0;i<pSector->m_UsedWeightmapChannels.Count();i++)
            if (!pSector->m_UsedWeightmapChannels.GetAt(i)->IsBlack())
              UsedTextures.AddUnique(pSector->m_UsedWeightmapChannels.GetAt(i)->m_spResource);

        // all decoration
        if ((int)pSelection->SelectionFilter & (int)TerrainSelection::SelectionFilter_e::Decoration)
          for (int i=0;i<pSector->m_DecorationChannels.Count();i++)
            if (!pSector->m_DecorationChannels.GetAt(i)->IsBlack())
              UsedModels.AddUnique(pSector->m_DecorationChannels.GetAt(i)->m_spSource);
      }

    if ((int)pSelection->SelectionFilter & (int)TerrainSelection::SelectionFilter_e::Heightmap) // copy height values?
    {
      int x1,y1,x2,y2;
      cfg.GetHeightSampleIndicesAtPos(bbox.m_vMin, x1,y1);
      cfg.GetHeightSampleIndicesAtPos(bbox.m_vMax, x2,y2);

      obj->m_pHeightValues = new VTerrainLockObject();
      VTerrainLockObject lockHeight(x1,y1,x2-x1+1,y2-y1+1,VIS_LOCKFLAG_READONLY);
      m_pTerrain->LockHeightValues(lockHeight);
       lockHeight.CloneData(*obj->m_pHeightValues);
      m_pTerrain->UnlockHeightValues(lockHeight);
    }

    if (UsedTextures.Count()>0) // copy texture layers?
    {
      int iCount = UsedTextures.Count();
      obj->m_iDetailTextureCount = iCount;
      obj->m_pDetailTextures = new VTerrainLockObject[iCount];
      obj->m_psDetailTextureNames = new VString[iCount];

      int x1,y1,x2,y2;
      cfg.GetWeightmapSampleIndicesAtPos(bbox.m_vMin, x1,y1);
      cfg.GetWeightmapSampleIndicesAtPos(bbox.m_vMax, x2,y2);
      VTerrainLockObject lockTex(x1,y1,x2-x1+1,y2-y1+1,VIS_LOCKFLAG_READONLY);
      for (int i=0;i<iCount;i++)
      {
        VTextureWeightmapChannelResource *pRes = UsedTextures.GetAt(i);
        DetailTextureResource ^pDetailTex = m_pSectorCallback->GetDetailTextureByID(pRes->m_Properties.m_iTextureID);
        if (pDetailTex!=nullptr)
          ConversionUtils::StringToVString(pDetailTex->Name, obj->m_psDetailTextureNames[i]);
        m_pTerrain->LockDetailTexture(lockTex, pRes);
         lockTex.CloneData(obj->m_pDetailTextures[i]);
        m_pTerrain->UnlockDetailTexture(lockTex);
      }
    }

    if (UsedModels.Count()>0) // copy decoration?
    {
      int iCount = UsedModels.Count();
      obj->m_iDecorationChannelCount = iCount;
      obj->m_pDecorationChannels = new VTerrainLockObject[iCount];
      obj->m_psDecorationChannelNames = new VString[iCount];

      int x1,y1,x2,y2;
      cfg.GetDensitySampleIndicesAtPos(bbox.m_vMin, x1,y1);
      cfg.GetDensitySampleIndicesAtPos(bbox.m_vMax, x2,y2);
      VTerrainLockObject lockDeco(x1,y1,x2-x1+1,y2-y1+1,VIS_LOCKFLAG_READONLY);
      for (int i=0;i<iCount;i++)
      {
        IVTerrainDecorationModel *pRes = UsedModels.GetAt(i);
        DecorationModelResource ^pDecoRes = m_pSectorCallback->GetDecorationModelByID(pRes->m_Properties.m_iModelID);
        if (pDecoRes!=nullptr)
          ConversionUtils::StringToVString(pDecoRes->Name, obj->m_psDecorationChannelNames[i]);
        m_pTerrain->LockDecorationChannel(lockDeco, pRes);
          lockDeco.CloneData(obj->m_pDecorationChannels[i]);
        m_pTerrain->UnlockDecorationChannel(lockDeco,true);
      }
    }

    return obj;
  }



  void EngineInstanceTerrain::PasteSelection(TerrainSelection ^pSelection, ITerrainClipboardObject ^ pData)
  {
    if (pSelection==nullptr || !pSelection->Valid || m_pTerrain==NULL || m_bIsReference)
      return;

    TerrainClipboardObject ^ obj = static_cast<TerrainClipboardObject ^>(pData);

    // get relevant sector range
    const VTerrainConfig &cfg(m_pTerrain->m_Config);
    hkvVec3 vMin(pSelection->WorldSpaceExtent->X1, pSelection->WorldSpaceExtent->Y1,-1.f);
    hkvVec3 vMax(pSelection->WorldSpaceExtent->X2, pSelection->WorldSpaceExtent->Y2, 1.f);
    int sx1,sy1,sx2,sy2;
    VLargeBoundingBox bbox(cfg,vMin,vMax);
    bbox.GetSectorIndices_Clamped(cfg,sx1,sy1,sx2,sy2);
    // we need to ensure that we can write into this sector...
    if (!m_pTerrain->GetFileLocksOnSectorRange(bbox,true,false))
      return;

    TerrainClipboardAction ^pAction = gcnew TerrainClipboardAction(this,"Paste Terrain");
    pAction->sx1 = sx1;
    pAction->sy1 = sy1;
    pAction->sx2 = sx2;
    pAction->sy2 = sy2;

    if (obj->m_pHeightValues && ((int)pSelection->SelectionFilter & (int)TerrainSelection::SelectionFilter_e::Heightmap))
    {
      pAction->m_pNewHeightValues = new VTerrainLockObject();

      // source data
      VTerrainLockObject &src(*obj->m_pHeightValues);
      src.m_pTerrain = this->m_pTerrain; // fixup (old one might not be necesary anymore)
      src.CloneData(*pAction->m_pNewHeightValues);

      // backup target data
      int x1,y1,x2,y2;
      cfg.GetHeightSampleIndicesAtPos(bbox.m_vMin, x1,y1);
      cfg.GetHeightSampleIndicesAtPos(bbox.m_vMax, x2,y2);
      VTerrainLockObject tgt(x1,y1,x2-x1+1,y2-y1+1,VIS_LOCKFLAG_READONLY);
      m_pTerrain->LockHeightValues(tgt);
        pAction->m_pOldHeightValues = new VTerrainLockObject();
        tgt.CloneData(*pAction->m_pOldHeightValues);
      m_pTerrain->UnlockHeightValues(tgt);
    }

    if (obj->m_pDetailTextures && ((int)pSelection->SelectionFilter & (int)TerrainSelection::SelectionFilter_e::Textures)) // paste textures?
    {
      pAction->m_iDetailTextureCount = obj->m_iDetailTextureCount;
      pAction->m_pNewDetailTextures = new VTerrainLockObject[obj->m_iDetailTextureCount];
      pAction->m_pOldDetailTextures = new VTerrainLockObject[obj->m_iDetailTextureCount];

      // source data
      for (int i=0;i<obj->m_iDetailTextureCount;i++)
      {
        VTerrainLockObject &src(obj->m_pDetailTextures[i]);
        src.m_pTerrain = this->m_pTerrain; // fixup (old one might not be necesary anymore)

        //int iID = src.m_iReserved[0]; // see also VEditableTerrainSector::GetRelevantDetailTextureValues

        // lookup detail textures by name
        DetailTextureResource ^pDetailTex = m_pSectorCallback->GetDetailTextureByName(
          ConversionUtils::VStringToString(obj->m_psDetailTextureNames[i]));
        if (pDetailTex==nullptr)
          continue;
        int iID = pDetailTex->ID;
        VTextureWeightmapChannelResource *pRes = SetNativeTextureProperties(pDetailTex); // make sure it is there
        if (pRes==NULL)
          continue;
        src.m_iReserved[0] = iID; // write back ID
        src.CloneData(pAction->m_pNewDetailTextures[i]);

        // backup target data
        int x1,y1,x2,y2;
        cfg.GetWeightmapSampleIndicesAtPos(bbox.m_vMin, x1,y1);
        cfg.GetWeightmapSampleIndicesAtPos(bbox.m_vMax, x2,y2);
        VTerrainLockObject tgt(x1,y1,x2-x1+1,y2-y1+1,VIS_LOCKFLAG_READONLY);
        m_pTerrain->LockDetailTexture(tgt, pRes);
          tgt.CloneData(pAction->m_pOldDetailTextures[i]);
        m_pTerrain->UnlockDetailTexture(tgt);

      }
    }

    if (obj->m_pDecorationChannels && ((int)pSelection->SelectionFilter & (int)TerrainSelection::SelectionFilter_e::Decoration)) // paste decoration?
    {
      pAction->m_iDecorationChannelCount = obj->m_iDecorationChannelCount;
      pAction->m_pNewDecorationChannels = new VTerrainLockObject[obj->m_iDecorationChannelCount];
      pAction->m_pOldDecorationChannels = new VTerrainLockObject[obj->m_iDecorationChannelCount];

      // source data
      for (int i=0;i<obj->m_iDecorationChannelCount;i++)
      {
        VTerrainLockObject &src(obj->m_pDecorationChannels[i]);
        src.m_pTerrain = this->m_pTerrain; // fixup (old one might not be necesary anymore)

        // lookup detail textures by name
        DecorationModelResource ^pDecoRes = m_pSectorCallback->GetDecorationModelByName(
          ConversionUtils::VStringToString(obj->m_psDecorationChannelNames[i]));
        if (pDecoRes==nullptr)
          continue;
        int iID = pDecoRes->ID;
        IVTerrainDecorationModel *pRes = SetNativeModelProperties(pDecoRes,NULL); // make sure it is there
        if (pRes==NULL)
          continue;
        src.m_iReserved[0] = iID; // write back ID
        src.CloneData(pAction->m_pNewDecorationChannels[i]);

        // backup target data
        int x1,y1,x2,y2;
        cfg.GetDensitySampleIndicesAtPos(bbox.m_vMin, x1,y1);
        cfg.GetDensitySampleIndicesAtPos(bbox.m_vMax, x2,y2);
        VTerrainLockObject tgt(x1,y1,x2-x1+1,y2-y1+1,VIS_LOCKFLAG_READONLY);
        m_pTerrain->LockDecorationChannel(tgt, pRes);
          tgt.CloneData(pAction->m_pOldDecorationChannels[i]);
        m_pTerrain->UnlockDecorationChannel(tgt,true);
      }
    }

    EditorManager::Actions->Add(pAction);

    if (obj->ShapesToPaste!=nullptr && ((int)pSelection->SelectionFilter & (int)TerrainSelection::SelectionFilter_e::Shapes)>0) // paste shapes?
    {
      ShapeCollection ^pShapes = obj->ShapesToPaste->CloneForClipboard();
      for (int i=0;i<pShapes->Count;i++)
      {
        Shape3D ^pShape = safe_cast<Shape3D ^>(pShapes[i]);
        // map to world space (shape xy was 0..1, see TerrainShape.CopySelection). z might not be correct though
        pShape->Position = Vector3F(
          pSelection->WorldSpaceExtent->X1 + pShape->x * pSelection->WorldSpaceExtent->GetSizeX(),
          pSelection->WorldSpaceExtent->Y1 + pShape->y * pSelection->WorldSpaceExtent->GetSizeY(),
          pShape->z);
      }
      // add thse shapes in a separate action
      EditorManager::Actions->Add(gcnew AddShapesAction(pShapes,nullptr, EditorManager::Scene->ActiveLayer, true,"Paste shapes on terrain"));
    }


  }

  bool EngineInstanceTerrain::GetSectorOwnsLock(int iSectorX, int iSectorY)
  {
    VVERIFY_OR_RET_VAL(m_pTerrain, false);
    //return m_pTerrain->GetSectorFileLockObject(iSectorX,iSectorY)->OwnsLock();
    return m_pTerrain->GetSector(iSectorX,iSectorY)->IsEditorFlagSet(SECTOR_EDITORFLAG_OWNSLOCK);
  }

  bool EngineInstanceTerrain::GetSectorNetworkLock(int iSectorX, int iSectorY)
  {
    VVERIFY_OR_RET_VAL(m_pTerrain, false);
    return m_pTerrain->GetSector(iSectorX,iSectorY)->IsEditorFlagSet(SECTOR_EDITORFLAG_NETWORKLOCK);
  }
  
  // Update the RCS status of the terrain
  void EngineInstanceTerrain::UpdateRCSFiles(int iActions, int iX, int iY, int iW, int iH)
  {
    VVERIFY_OR_RET(m_pTerrain);
    
    //negative pos means update the whole terrain
    if (iX<0 && iY<0)
    {
      //Update config.vtc
      VFileAccessManager::RCSUpdateFile(m_pTerrain->m_Config.m_sTerrainFolder);
      if (iActions & RCS_EDIT)
        VFileAccessManager::RCSEditFile(m_pTerrain->m_Config.m_sTerrainFolder); //make sure we can write to it
    }
    
    if (iX<0) { iX = 0; iW = m_pTerrain->m_Config.m_iSectorCount[0]; }
    if (iY<0) { iY = 0; iH = m_pTerrain->m_Config.m_iSectorCount[1]; }
    
    //Update all the sectors of the terrain
    for (int sy=iY;sy<iH;sy++)
      for (int sx=iX;sx<iW;sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        pSector->UpdateRCSFiles(iActions);
      }
    
    //Reload possibly loaded sectors after an update
    if (iActions & RCS_UPDATE)
    {
//TODO      m_pTerrain->Reload();
      
      //Update all the sectors of the terrain
      for (int sy=iY;sy<iH;sy++)
        for (int sx=iX;sx<iW;sx++)
        {
          VTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
          if (pSector->IsLoaded())
            pSector->Reload();
        }
    }
  }
  
  void EngineInstanceTerrain::ExportTerrainMeshs(TerrainMeshExportSettings^ pSettings, ProgressStatus^ pProgress, float fProgressRange, bool bExportPrefab)
  {
    if (m_pTerrain==nullptr || pSettings==nullptr)
      return;

    // Ensure everything is saved
    SaveToFile();

    // Ensure export folders exist
    char szAbsPath[FS_MAX_PATH];
    m_pTerrain->m_Config.GetDirectory(szAbsPath, VTC_DIR_MESH, false);
    VString sAbspath = VFileHelper::CombineDirAndDir(m_pTerrain->m_Config.m_sAbsProjectDir, szAbsPath);
    if (!VFileHelper::ExistsDir(sAbspath))
      VFileHelper::MkDir(sAbspath);

    m_pTerrain->m_Config.GetDirectory(szAbsPath, VTC_DIR_TEXTURE, false);
    sAbspath = VFileHelper::CombineDirAndDir(m_pTerrain->m_Config.m_sAbsProjectDir, szAbsPath);
    if (!VFileHelper::ExistsDir(sAbspath))
      VFileHelper::MkDir(sAbspath);

    m_pTerrain->SectorManager().SetUseExportRendering(true);

    // Track view settings
    bool bVisible = m_bVisible;
    SetVisible(true);

    bool bWireframe = Vision::Renderer.GetWireframeMode();
    Vision::Renderer.SetWireframeMode(false);

    // Load settings of currently saved .vmeshes
    TerrainMeshExportSettings^ pLoadedSettings = LoadMeshExportSettings();

    TerrainMeshExporter::MeshExportParameters exportParameters;
    exportParameters.bBakedLightmaps = pSettings->_bBakedLightmaps;
    exportParameters.bSimplifyMesh = pSettings->_bSimplifyMesh;
    exportParameters.bUseLegacySolver = pSettings->_bUseLegacySolver;
    exportParameters.iTextureResolution = pSettings->_iTextureResolution;
    exportParameters.fMaxErrorDistance = pSettings->_fMaxErrorDistance;
    exportParameters.pTerrain = m_pTerrain;

    // Use 4 pixels border to ensure proper texture filtering between sectors
    exportParameters.iBorderWidth = 4;

    float fStep = (fProgressRange*0.25f) / (float)(m_pTerrain->m_Config.m_iSectorCount[0]*m_pTerrain->m_Config.m_iSectorCount[1]);
    float fPercentage = 0.0f;

    if (pProgress)
      pProgress->StatusString = gcnew String("Exporting diffuse replacement textures.");

    // Vector for storing the sector mesh filenames, used by RCS to add files after the export
    std::vector<VString> sectorMeshFilenames;
    std::vector<VString> sectorCollisionFilenames;

    // Loop sectors and decide if export is required
    std::vector<std::pair<int, int>> Sectors;
    for (int sy=0; sy<m_pTerrain->m_Config.m_iSectorCount[1]; ++sy)
    {
      for (int sx=0; sx<m_pTerrain->m_Config.m_iSectorCount[0]; ++sx)
      {
        char szFilename[FS_MAX_PATH];
        VFileTime vmeshFileTime, fileTime;

        bool bExport = false;
        bool bExportSectorTexture = false;

        // Macro to get absolute filename of a specific terrain file type
        #define GET_SECTOR_FILENAME(fileType) \
          m_pTerrain->m_Config.GetSectorFile(szFilename, sx, sy, fileType);\
          if (!VFileHelper::IsAbsolutePath(szFilename))\
            VFileHelper::GetAbsolutePath(szFilename, szFilename, Vision::File.GetManager());

        // Export texture when diffuse replacement texture is missing
        GET_SECTOR_FILENAME(VTC_FILE_TEXTURES_DIFFUSE);
        if (!VFileHelper::Exists(szFilename) || (pLoadedSettings == nullptr) || (pLoadedSettings->_iTextureResolution != pSettings->_iTextureResolution))
            bExportSectorTexture = true;

        // export sector when settings have been changed
        if ((pLoadedSettings == nullptr) ||
            (pLoadedSettings->_fMaxErrorDistance != pSettings->_fMaxErrorDistance && pSettings->_bSimplifyMesh) ||
            (pLoadedSettings->_bSimplifyMesh != pSettings->_bSimplifyMesh) ||
            (pLoadedSettings->_bUseLegacySolver != pSettings->_bUseLegacySolver) ||
            (pLoadedSettings->_bBakedLightmaps != pSettings->_bBakedLightmaps) ||
            (pLoadedSettings->_iTextureResolution != pSettings->_iTextureResolution))
        {
          bExport = true;
        }

        // Export when .vmesh is missing
        if (!bExport)
        {
          GET_SECTOR_FILENAME(VTC_FILE_MESHES_VMESH);
          if (!VFileHelper::GetModifyTime(szFilename, vmeshFileTime))
            bExport = true;
        }

        // Export when .vcolmesh is missing
        if (!bExport)
        {
          GET_SECTOR_FILENAME(VTC_FILE_MESHES_VCOLMESH);
          if (!VFileHelper::Exists(szFilename))
            bExport = true;
        }

        // Macro to compare the timestamp of the .vmesh to the timestamp of another terrain file type
        #define CHECK_TIME_STAMP(fileType) \
          if (!bExport)\
          {\
            GET_SECTOR_FILENAME(fileType)\
            if (VFileHelper::GetModifyTime(szFilename, fileTime) && (vmeshFileTime < fileTime))\
              bExport = true;\
          }

        // Compare timestamps
        CHECK_TIME_STAMP(VTC_FILE_EDITING_DATA)
        CHECK_TIME_STAMP(VTC_FILE_SECTORS_HMAP)
        CHECK_TIME_STAMP(VTC_FILE_SECTORS_MESH)

        // Add to export sectors if required
        if (bExport)
        {
          // Build .vmesh filename
          m_pTerrain->m_Config.GetSectorMeshFilename(szFilename, sx, sy, false);
          VString sectorMeshFileName(m_pTerrain->m_Config.m_sAbsProjectDir);
          sectorMeshFileName += VString(szFilename);

          // Build .vcolmesh filename
          m_pTerrain->m_Config.GetSectorCollisionFilename(szFilename, sx, sy, false);
          VString sectorCollisionFileName(m_pTerrain->m_Config.m_sAbsProjectDir);
          sectorCollisionFileName += VString(szFilename);

          m_pTerrain->GetSector(sx, sy)->EnsureLoaded();
          if (m_pTerrain->GetSector(sx, sy)->IsAllHoles() || (m_pTerrain->GetSector(sx, sy)->GetVisibleBitmask() == 0))
          {
            if (VFileHelper::Exists(sectorMeshFileName))
              VFileAccessManager::RCSPerformAction(sectorMeshFileName, RCS_REMOVE);
            if (VFileHelper::Exists(sectorCollisionFileName))
              VFileAccessManager::RCSPerformAction(sectorCollisionFileName, RCS_REMOVE);
          }
          else
          {
            Sectors.push_back(std::pair<int, int>(sx, sy));

            sectorMeshFilenames.push_back(sectorMeshFileName);
            sectorCollisionFilenames.push_back(sectorCollisionFileName);

            if (VFileHelper::Exists(sectorMeshFileName))
              VFileAccessManager::RCSEditFile(sectorMeshFileName);
            if (VFileHelper::Exists(sectorCollisionFileName))
              VFileAccessManager::RCSEditFile(sectorCollisionFileName);
          }
          m_pTerrain->GetSector(sx, sy)->EnsureUnloaded();
        }

        // Export sector textures (does render the terrain into a texture if required)
        // Note: rendering must be done on the main thread, thus this task can't be done in parallel
        if (bExport || bExportSectorTexture)
          TerrainMeshExporter::ExportSectorTexture(exportParameters, sx, sy);

        fPercentage += fStep;
        if (pProgress)
          pProgress->Percentage = fPercentage;

        Application::DoEvents();
      }      
    }

    // Restore view settings
    SetVisible(bVisible);
    Vision::Renderer.SetWireframeMode(bWireframe);
    
    m_pTerrain->SectorManager().SetUseExportRendering(false);

    fStep = (fProgressRange*0.75f) / (float)Sectors.size();
    if (pProgress)
      pProgress->StatusString = gcnew String("Exporting sector geometry data.");

    // Export .vmesh file for each sector of the terrain
#if defined(SUPPORTS_MULTITHREADING)

    VThreadManager *pManager = VThreadManager::GetManager();

    int iProcessorCount = pManager->GetNumberOfProcessors();
    int iThreadCount = pManager->GetThreadCount();

    // Use all available processors for processing
    pManager->SetThreadCount(iProcessorCount);

    // We use as much tasks as there are sectors in the terrain
    DynObjArray_cl<TerrainMeshExporter::VExportSectorTask> Tasks((unsigned int)iProcessorCount);

    while (Sectors.size() > 0)
    {
      int i;
      for (i=0; i<iProcessorCount; ++i)
      {
        // Abort scheduling once there are no sectors anymore
        if (Sectors.size() == 0)
          break;

        TerrainMeshExporter::VExportSectorTask& task = Tasks[i];
        VASSERT(task.GetState() == TASKSTATE_FINISHED || task.GetState() == TASKSTATE_UNASSIGNED);

        std::pair<int, int> info = Sectors.back();
        Sectors.pop_back();

        // Provide task with data
        task.m_pParameters = &exportParameters;
        task.m_iSectorX = info.first;
        task.m_iSectorY = info.second;

        const int iCountX = (m_pTerrain->m_Config.m_iSectorCount[0] == 1) ? 1 : (((task.m_iSectorX > 0) && (task.m_iSectorX < m_pTerrain->m_Config.m_iSectorCount[0] - 1)) ? 3 : 2);
        const int iCountY = (m_pTerrain->m_Config.m_iSectorCount[1] == 1) ? 1 : (((task.m_iSectorY > 0) && (task.m_iSectorY < m_pTerrain->m_Config.m_iSectorCount[1] - 1)) ? 3 : 2);

        // Load sectors
        m_pTerrain->EnsureSectorRangeLoaded(hkvMath::Max(0, task.m_iSectorX-1), hkvMath::Max(0, task.m_iSectorY-1), iCountX, iCountY);

        // Let the task run
        pManager->ScheduleTask(&task);
      }

      pManager->WaitForAllThreads();

      // Unloading the sectors after each packet of processed sectors ensures that memory does not get filled too much
      for (int sy=0; sy<m_pTerrain->m_Config.m_iSectorCount[1]; ++sy)
        for (int sx=0; sx<m_pTerrain->m_Config.m_iSectorCount[0]; ++sx)
          m_pTerrain->GetSector(sx, sy)->EnsureUnloaded();

      fPercentage += fStep*(float)iProcessorCount;
      if (pProgress)
        pProgress->Percentage = fPercentage;

      Application::DoEvents();
    }

    // Wait until everything is finished
    pManager->WaitForAllThreads();

    // Restore thread count
    pManager->SetThreadCount(iThreadCount);

#else

    // Unload all terrain sectors, so that during processing only those sectors are loaded which are actually required
    for (int i=0; i<(int)Sectors.size(); ++i)
      m_pTerrain->GetSector(Sectors[i].first, Sectors[i].second)->EnsureUnloaded();

    for (int i=0; i<(int)Sectors.size(); ++i)
    {
      const int sx = Sectors[i].first;
      const int sy = Sectors[i].second;

      const int iCountX = (m_pTerrain->m_Config.m_iSectorCount[0] == 1) ? 1 : (((sx > 0) && (sx < m_pTerrain->m_Config.m_iSectorCount[0] - 1)) ? 3 : 2);
      const int iCountY = (m_pTerrain->m_Config.m_iSectorCount[1] == 1) ? 1 : (((sy > 0) && (sy < m_pTerrain->m_Config.m_iSectorCount[1] - 1)) ? 3 : 2);

      m_pTerrain->EnsureSectorRangeLoaded(hkvMath::Max(0, sx-1), hkvMath::Max(0, sy-1), iCountX, iCountY);
      {
        TerrainMeshExporter::ExportSectorMesh(exportParameters, sx, sy);
      }
      
      for (int sy=0; sy<m_pTerrain->m_Config.m_iSectorCount[1]; ++sy)
        for (int sx=0; sx<m_pTerrain->m_Config.m_iSectorCount[0]; ++sx)
          m_pTerrain->GetSector(sx, sy)->EnsureUnloaded();

      if (pProgress)
        pProgress->Percentage = fPercentage;
      fPercentage+=fStep;

      Application::DoEvents();
    }

#endif

    // Add files to RCS (done for all files, even if they already have been opened for edit)
    for (int i=0; i<(int)sectorMeshFilenames.size(); ++i)
      VFileAccessManager::RCSAddFile(sectorMeshFilenames[i], true);
    for (int i=0; i<(int)sectorCollisionFilenames.size(); ++i)
      VFileAccessManager::RCSAddFile(sectorCollisionFilenames[i], true);

    // Save new settings to file
    SaveMeshExportSettings(pSettings);

    // Export prefab if required
    if (bExportPrefab)
      ExportPrefab();
  }

  void EngineInstanceTerrain::ExportPrefab()
  {
    // Create mesh shape for each exported .vmesh file
    ShapeCollection^ sectorShapes = gcnew ShapeCollection();
    for (int sy=0; sy<m_pTerrain->m_Config.m_iSectorCount[1]; ++sy)
          {
      for (int sx=0; sx<m_pTerrain->m_Config.m_iSectorCount[0]; ++sx)
      {
        char szFilename[FS_MAX_PATH];
        if (!m_pTerrain->m_Config.GetSectorMeshFilename(szFilename, sx, sy, false))
          continue;

        String^ absSectorFileName = gcnew String(m_pTerrain->m_Config.m_sAbsProjectDir);
        absSectorFileName += gcnew String(szFilename);
        if (!System::IO::File::Exists(absSectorFileName))
          continue;

        String^ sectorFileName = gcnew String(szFilename);
        StaticMeshShape^ shape = gcnew StaticMeshShape(Path::GetFileNameWithoutExtension(sectorFileName));
        shape->MeshFileName = sectorFileName;

        hkvVec2 vOrigin = m_pTerrain->GetSector(sx, sy)->GetSectorOrigin().getAsVec2();
        shape->Position = Vector3F(vOrigin.x, vOrigin.y, 0);

        sectorShapes->Add(shape);
      }
    }

    String^ proj = EditorManager::Project->ProjectDir;
    String^ prefabDir = Path::Combine(proj, EditorManager::Settings->PrefabDirectory);
    if (!Directory::Exists(prefabDir))
    {
      prefabDir = proj;
    }

    // Create prefab
    String^ prefabFilename = Path::Combine(prefabDir, ConversionUtils::VStringToString(m_pTerrain->m_Config.m_sTerrainFolder) + ".prefab");
    PrefabDesc^ prefab = gcnew PrefabDesc(prefabFilename);

    // If the file exists, load it to get the properties
    if (File::Exists(prefab->Filename))
    {
      prefab->Load();
    }

    if (!prefab->CreateFromInstances(sectorShapes, Vector3F(0.0f, 0.0f, 0.0f), true, true))
    {
      EditorManager::ShowMessageBox("Failed to create prefab. Detailed message:\n\n" + prefab->LastError, "Creation Failed", MessageBoxButtons::OK, MessageBoxIcon::Error);
      return;
    }

    if (!prefab->SaveToFile(prefab->Filename))
    {
      EditorManager::ShowMessageBox("Failed to save prefab. Detailed message:\n\n" + prefab->LastError, "Saving Failed", MessageBoxButtons::OK, MessageBoxIcon::Error);
      return;
    }
  }

  String^ EngineInstanceTerrain::GetSectorMeshFileName(int iSectorX, int iSectorY)
  {
    char szFilename[FS_MAX_PATH];
    if (!m_pTerrain->m_Config.GetSectorMeshFilename(szFilename, iSectorX, iSectorY, false))
      return nullptr;
            
    return ConversionUtils::VStringToString(szFilename);
  }

  bool EngineInstanceTerrain::GetSectorMeshCount(int% iCountX, int% iCountY)
  {
    if (!m_pTerrain)
      return false;

    iCountX = m_pTerrain->m_Config.m_iSectorCount[0];
    iCountY = m_pTerrain->m_Config.m_iSectorCount[1];
    return true;
  }

  Vector3F^ EngineInstanceTerrain::GetSectorOrigin(int iSectorX, int iSectorY)
  {
    if (!m_pTerrain)
      return gcnew Vector3F(0.0f, 0.0f, 0.0f);

    const hkvVec2 vOrigin = m_pTerrain->GetSector(iSectorX, iSectorY)->GetSectorOrigin().getAsVec2();
    return gcnew Vector3F(vOrigin.x, vOrigin.y, 0.0f);
  }

  String^ EngineInstanceTerrain::GetSectorLightmapName(int iSectorX, int iSectorY)
  {
    if (!m_pTerrain)
      return nullptr;

    VTextureObject* pTexture = m_pTerrain->GetSector(iSectorX, iSectorY)->GetLightmapTexture(0);

    if (pTexture != NULL)
      return gcnew String(pTexture->GetFilename());
    else
      return nullptr;
  }

  void EngineInstanceTerrain::SetTerrainMeshExportFlag(bool bFlag)
  {
    if (m_pTerrain)
      m_pTerrain->SectorManager().SetTerrainMeshExportInProgress(bFlag);
  }

  String^ EngineInstanceTerrain::GetMeshExportSettingsFilename()
  {
    char szAbsFilename[FS_MAX_PATH];
    m_pTerrain->m_Config.GetDirectory(szAbsFilename, VTC_DIR_MESH, false);
    VFileHelper::CombineDirAndFile(szAbsFilename, szAbsFilename, "ExportSettings.xml");
    return (EditorManager::Project->ProjectDir + gcnew String(szAbsFilename));
  }

  void EngineInstanceTerrain::SaveMeshExportSettings(TerrainMeshExportSettings^ pSettings)
  {
    String^ szAbsFilename = GetMeshExportSettingsFilename();

    VString sFilename;
    ConversionUtils::StringToVString(szAbsFilename, sFilename);
    VFileAccessManager::RCSEditFile(sFilename);

    // we don't have access to the derived class of TerrainMeshExportSettings, thus we have to use the base type and copy the data
    TerrainMeshExportSettings^ pData = gcnew TerrainMeshExportSettings(pSettings);

    System::Xml::Serialization::XmlSerializer^ ser = gcnew System::Xml::Serialization::XmlSerializer(pData->GetType());
    System::IO::TextWriter^ writer = gcnew System::IO::StreamWriter(gcnew String(szAbsFilename));
    ser->Serialize(writer, pData);
    writer->Close();

    VFileAccessManager::RCSAddFile(sFilename, false);
  }

  TerrainMeshExportSettings^ EngineInstanceTerrain::LoadMeshExportSettings()
  {
    String^ szAbsFilename = GetMeshExportSettingsFilename();
    if (!System::IO::File::Exists(szAbsFilename))
      return nullptr;

    System::IO::FileStream^ fs = gcnew System::IO::FileStream(szAbsFilename, System::IO::FileMode::Open, System::IO::FileAccess::Read);

    TerrainMeshExportSettings^ pSettings = gcnew TerrainMeshExportSettings();
    
    System::Xml::Serialization::XmlSerializer^ ser = gcnew System::Xml::Serialization::XmlSerializer(pSettings->GetType());
    pSettings = (TerrainMeshExportSettings^) ser->Deserialize(fs);
    fs->Close();

    return pSettings;
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
