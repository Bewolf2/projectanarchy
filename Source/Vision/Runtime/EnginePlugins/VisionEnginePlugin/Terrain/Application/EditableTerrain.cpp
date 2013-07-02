/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Bitmap/BlittingHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/ITerrainFilter.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
#endif



// no own Serialize function though
V_IMPLEMENT_SERIAL( VEditableTerrain, VTerrain, 0, &g_VisionEngineModule );

unsigned int VEditableTerrain::m_iFrameRelevantMeshesDetected = 0xffffffff;
VObjectComponentCollection VEditableTerrain::m_MeshPaintingComponents;
VisCallback_cl VEditableTerrain::OnDecorationDirtyRangeChanged;

// helper macro
#define FOREACH_SECTOR \
  for (int sy=0;sy<m_Config.m_iSectorCount[1];sy++)\
    for (int sx=0;sx<m_Config.m_iSectorCount[0];sx++)\
    {\
      VEditableTerrainSector *pSector = GetSector(sx,sy);


/////////////////////////////////////////////////////////////////////////////////////////////
// editable terrain
/////////////////////////////////////////////////////////////////////////////////////////////

VEditableTerrain::VEditableTerrain() : 
  m_fBrushBuffer(0,0.f), m_fSourceBuffer(0,0.f), m_fDestinationBuffer(0,0.f), m_LockMem(0,0)
{
  m_bShow3DCursor = false;
  m_bFailedLoadingCursorFXLib = false;
  m_pSectorAction = NULL;
  m_bUseBakedTextures = false;
  if (!Vision::Editor.IsInEditor()) // prefer this method in exported versions
    m_bUseBakedTextures = true;
  m_iLightmapPageSize = 0;// no lightmapping
  m_bPerSectorLightmaps = false;
  m_bCastStaticShadows = true;
  m_bPreviewDirectionalLight = true;
  m_bPreviewHoleOverlay = false;
  m_bShowMiniMap = false;
  m_fBlinkDuration = 0.f;
  m_fCursorRotationAngle = 0.f;
  m_fCursorWSRadius = 0.f;
  m_bSaveModifiedFiles = true;
  m_bCheckSectorLocks = true;
  m_iHighlightMaterialID = -1;

  // listen to the following callbacks
  Vision::Callbacks.OnRenderHook += this;
  Vision::Callbacks.OnGatherLightmapInfo += this;
  Vision::Callbacks.OnLightmapFileLoaded += this;
  Vision::Callbacks.OnResetStaticLighting += this;
  Vision::Callbacks.OnRebuiltVisibility += this;
  Vision::Callbacks.OnUpdateSceneFinished += this;
  Vision::Callbacks.OnRendererNodeChanged += this;
}

VEditableTerrain::~VEditableTerrain()
{
  Vision::Callbacks.OnRenderHook -= this;
  Vision::Callbacks.OnGatherLightmapInfo -= this;
  Vision::Callbacks.OnLightmapFileLoaded -= this;
  Vision::Callbacks.OnResetStaticLighting -= this;
  Vision::Callbacks.OnRebuiltVisibility -= this;
  Vision::Callbacks.OnUpdateSceneFinished -= this;
  Vision::Callbacks.OnRendererNodeChanged -= this;
}



void VEditableTerrain::SetSaveModifiedFiles(bool bStatus)
{
  m_bSaveModifiedFiles = bStatus;
  m_SectorManager.SetAllowPurging(bStatus);
}

void VEditableTerrain::OnNewTerrainCreated()
{
  VTerrain::OnNewTerrainCreated();
  ReloadEditingShaders();
}


void VEditableTerrain::FreeTerrain()
{
  // release all file locks
  ReleaseAllSectorFileLocks(false); // save?
  V_SAFE_DELETE_ARRAY(m_pSectorAction);
  VTerrain::FreeTerrain();

  m_spCursorTexture = NULL;
  m_spDefaultCursorTexture = NULL;

  // free effects
  m_spDetailTexPass = NULL;
  m_spDetailTexPass3way = NULL;
  m_spDirLightPass = NULL;
  m_spLightmapPass = NULL;
  m_spFogPass = NULL;
  m_spClearAlphaPass = NULL;

  m_spWeightmapPass = NULL;
  m_spHoleOverlayPass = NULL;
  m_spLockedOverlayPass = NULL;
  m_spHighlightMaterialIDPass = NULL;
  m_spMiniMapPass = NULL;
}


bool VEditableTerrain::LoadFromFile(const char *szFilename)
{
  if (!VTerrain::LoadFromFile(szFilename))
    return false;

  // load editor specific data here

  return true;
}
 
bool VEditableTerrain::SaveToFile(const char *szAbsFolder)
{
  if (!VTerrain::SaveToFile(szAbsFolder))
    return false;

  // save editor specific data here

  return true;
}


// sets the parts of the config that can be changed with no major recalculations
void VEditableTerrain::SetModifiableConfig(VTerrainConfig &cfg)
{
  SetTerrainOrigin(cfg.m_vTerrainPos);

  bool bConfigChanged = false;
  bool bForceUpdateAllSectors = false;
  bool bForceRebakeWeightmaps = false;
  bool bNeedsDecorationUpdate = false;
  bool bSectorsMoved = cfg.IsMovedSectorStartPosition();

  /// Setting
  if (cfg.m_iOldSectorStartIndex[0] !=m_Config.m_iOldSectorStartIndex[0] || cfg.m_iOldSectorStartIndex[1] != m_Config.m_iOldSectorStartIndex[1])
  {
	  m_Config.m_iOldSectorStartIndex[0] = cfg.m_iOldSectorStartIndex[0];
	  m_Config.m_iOldSectorStartIndex[1] = cfg.m_iOldSectorStartIndex[1];
  }

  if (cfg.m_iNewSectorStartIndex[0] !=m_Config.m_iNewSectorStartIndex[0] || cfg.m_iNewSectorStartIndex[1] != m_Config.m_iNewSectorStartIndex[1])
  {
	  m_Config.m_iNewSectorStartIndex[0] = cfg.m_iNewSectorStartIndex[0];
	  m_Config.m_iNewSectorStartIndex[1] = cfg.m_iNewSectorStartIndex[1];
  }

  if (cfg.m_iOldSectorCount[0] !=m_Config.m_iOldSectorCount[0] || cfg.m_iOldSectorCount[1] != m_Config.m_iOldSectorCount[1])
  {
	  m_Config.m_iOldSectorCount[0] = cfg.m_iOldSectorCount[0];
	  m_Config.m_iOldSectorCount[1] = cfg.m_iOldSectorCount[1];
  }

  if (cfg.m_iSectorCount[0]!=m_Config.m_iSectorCount[0] || cfg.m_iSectorCount[1]!=m_Config.m_iSectorCount[1])
  {
    ChangeSectorCount(cfg.m_iSectorCount[0],cfg.m_iSectorCount[1]);
    bConfigChanged = bForceUpdateAllSectors = true;
  }

  if (cfg.m_vSectorSize!=m_Config.m_vSectorSize)
  {
    bForceUpdateAllSectors = true;
    bConfigChanged = true;
    bNeedsDecorationUpdate = true;
    m_Config.m_vSectorSize = cfg.m_vSectorSize;
  }
  if (cfg.m_fVisibilityHeightOverTerrain!=m_Config.m_fVisibilityHeightOverTerrain)
  {
    bConfigChanged = true;
    m_Config.m_fVisibilityHeightOverTerrain = cfg.m_fVisibilityHeightOverTerrain;
  }
  if (cfg.m_iDensityMapSamplesPerSector[0]!=m_Config.m_iDensityMapSamplesPerSector[0] || cfg.m_iDensityMapSamplesPerSector[1]!=m_Config.m_iDensityMapSamplesPerSector[1])
  {
    m_Config.m_iDensityMapSamplesPerSector[0] = cfg.m_iDensityMapSamplesPerSector[0];
    m_Config.m_iDensityMapSamplesPerSector[1] = cfg.m_iDensityMapSamplesPerSector[1];

    bConfigChanged = true;
    bForceUpdateAllSectors = true;
    bNeedsDecorationUpdate = true;
  }
  if (cfg.m_bNormalmapUsesBorder != m_Config.m_bNormalmapUsesBorder)
  {
    bForceUpdateAllSectors = true;
    bConfigChanged = true;
    m_Config.m_bNormalmapUsesBorder = cfg.m_bNormalmapUsesBorder;
  }
  if (cfg.m_iMaterialMapResolution[0]!=m_Config.m_iMaterialMapResolution[0] || cfg.m_iMaterialMapResolution[1]!=m_Config.m_iMaterialMapResolution[1])
  {
    m_Config.m_iMaterialMapResolution[0] = cfg.m_iMaterialMapResolution[0];
    m_Config.m_iMaterialMapResolution[1] = cfg.m_iMaterialMapResolution[1];

    bConfigChanged = true;
    bForceUpdateAllSectors = true;
    bForceRebakeWeightmaps = true;
  }
  if (cfg.m_iDefaultWeightmapResolution[0]!=m_Config.m_iDefaultWeightmapResolution[0] || cfg.m_iDefaultWeightmapResolution[1]!=m_Config.m_iDefaultWeightmapResolution[1])
  {
    m_Config.m_iDefaultWeightmapResolution[0] = cfg.m_iDefaultWeightmapResolution[0];
    m_Config.m_iDefaultWeightmapResolution[1] = cfg.m_iDefaultWeightmapResolution[1];

    bConfigChanged = true;
    bForceUpdateAllSectors = true;
    bForceRebakeWeightmaps = true;
  }

  // here are a few settings that can be set without breaking anything:
  m_Config.m_fPurgeInterval = cfg.m_fPurgeInterval;
  m_Config.m_fPrecacheMargin = cfg.m_fPrecacheMargin;
  m_Config.m_iMemoryLimit = cfg.m_iMemoryLimit;

  if (bConfigChanged)
  {
    m_Config.Finalize();
    FOREACH_SECTOR
      BOOL bOldLoaded = pSector->IsLoaded();
      if (!bOldLoaded && !bForceUpdateAllSectors)
        continue;
      pSector->OnConfigChanged();
      if (bNeedsDecorationUpdate)
        pSector->UpdateDecoration(true);
      if (bForceRebakeWeightmaps)
        pSector->SetEditorFlag(SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY);
      if (!bOldLoaded)
        pSector->EnsureUnloaded();
    }
  }
  if (bSectorsMoved)
  {
    // toggle baked state to get everything up-to date
    SetUseBakedTextures(!m_bUseBakedTextures, true);
    SetUseBakedTextures(!m_bUseBakedTextures, true);
  }
}


void VEditableTerrain::ChangeSectorCount(int iNewCountX, int iNewCountY)
{
  RemoveFromSceneManager();
  V_SAFE_DELETE_ARRAY(m_spSceneLightmaps);

  int iOldCountX = m_Config.m_iSectorCount[0];
  int iOldCountY = m_Config.m_iSectorCount[1];

  // First destroy old instances if one of the new dimension is smaller than the old one
  if (iOldCountX > iNewCountX || iOldCountY > iNewCountY)
  {
    for (int y=0;y<iOldCountY;y++)
    {
      for (int x=0;x<iOldCountX;x++)
      {
        if (x>=iNewCountX || y>=iNewCountY)
        {
          VTerrainSector *pSector = m_SectorManager.m_pSector[y*iOldCountX+x];
          pSector->SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD|VRESOURCEFLAG_AUTODELETE);
          pSector->EnsureUnloaded();
          
          // delete sector files out of both the temp folder and the permanent one
          pSector->DeleteSectorData(true);
          pSector->DeleteSectorData(false);
        }
      }
    }
  }

  // next, swap out old sectors
  VTerrainSectorPtr *pOldSectorList =  m_SectorManager.m_pSector;
  m_SectorManager.m_pSector = NULL;

  // allocate new arrays:
  int iNewSectorCount = iNewCountX*iNewCountY;
  V_SAFE_DELETE_ARRAY(m_pSectorAction); // gets allocated on demand
  m_SectorManager.m_pSector = new VTerrainSectorPtr[iNewSectorCount];

  // fill up with new instances
  for (int y=0;y<iNewCountY;y++)
    for (int x=0;x<iNewCountX;x++)
    {
      VEditableTerrainSector *pNewSector;
      if (x<iOldCountX && y<iOldCountY)
        pNewSector = (VEditableTerrainSector*)pOldSectorList[y*iOldCountX+x].GetPtr();
      else
      {
        pNewSector = (VEditableTerrainSector*)CreateSectorInstance(x,y);
        pNewSector->SetEditorFlag(SECTOR_EDITORFLAG_ANYTHINGDIRTY | SECTOR_EDITORFLAG_TEMPDIR);
      }
      m_SectorManager.m_pSector[y*iNewCountX+x] = pNewSector;
    }

  V_SAFE_DELETE_ARRAY(pOldSectorList); // this destroys unused

  // update config and re-save
  m_SectorManager.m_iSectorCount = iNewSectorCount;
  m_SectorManager.PackPointers();

  m_Config.m_iSectorCount[0] = iNewCountX;
  m_Config.m_iSectorCount[1] = iNewCountY;
  m_Config.Finalize();

  AddToSceneManager();
  SaveToFile(NULL);

  // lock/the edges so there are no gaps:
  int iEdgeX = iOldCountX*m_Config.m_iHeightSamplesPerSector[0];
  int iEdgeY = iOldCountY*m_Config.m_iHeightSamplesPerSector[1];
  if (iNewCountX>iOldCountX)
  {
    VTerrainLockObject lockObj(iEdgeX,0,iEdgeX+2,iEdgeY+2, 0);
    LockHeightValues(lockObj);
    UnlockHeightValues(lockObj);
  }
  if (iNewCountY>iOldCountY)
  {
    VTerrainLockObject lockObj(0,iEdgeY,iEdgeX+2,iEdgeY+2, 0);
    LockHeightValues(lockObj);
    UnlockHeightValues(lockObj);
  }

}


// resets the flags on each sector that would cause to save sector data
void VEditableTerrain::ResetEditableFlags()
{
  FOREACH_SECTOR
    pSector->RemoveEditorFlag(SECTOR_EDITORFLAG_ANYTHINGDIRTY);
  }
}


VCompiledTechnique* CreateSpecificTechnique(const char *szFXName, BOOL bUseDeferred, VTechniqueConfig* pConfig = NULL)
{
  char szBuffer[256];
  if (bUseDeferred)
  {
    sprintf(szBuffer,"%s_Deferred",szFXName);
    szFXName = szBuffer;
  }
  return Vision::Shaders.CreateTechnique(szFXName, NULL, pConfig);
}


void VEditableTerrain::ReloadEditingShaders()
{
  // if deferred shading is used a postfix is appended to all effect names

#if defined( HK_ANARCHY )
  BOOL bUseDeferred = FALSE;
#else
  BOOL bUseDeferred = Vision::Renderer.IsRendererNodeOfType(V_RUNTIME_CLASS(VDeferredRenderingSystem));
#endif

#if defined(_VISION_PS3) || defined(_VISION_PSP2)
#pragma diag_push
#pragma diag_suppress=552
#endif

  // Load default shaders
  if (bUseDeferred)
    Vision::Shaders.LoadShaderLibrary("TerrainPlugin\\TerrainCursorDeferred.ShaderLib", SHADERLIBFLAG_HIDDEN);
  else
    Vision::Shaders.LoadShaderLibrary("TerrainPlugin\\TerrainCursor.ShaderLib", SHADERLIBFLAG_HIDDEN);

#ifdef _VISION_PS3
#pragma diag_pop
#endif

  g_spCursorFX = CreateSpecificTechnique("TerrainCursor",bUseDeferred);
  VASSERT(g_spCursorFX && "failed to create terrain effect");

  m_spDetailTexPass = CreateSpecificTechnique("TerrainDetailPass",bUseDeferred);
  VASSERT(m_spDetailTexPass && "failed to create terrain effect");

  VTechniqueConfig config("USE_3WAY", NULL);
  m_spDetailTexPass3way = CreateSpecificTechnique("TerrainDetailPass", bUseDeferred, &config);
  VASSERT(m_spDetailTexPass3way && "failed to create 3 way mapping terrain effect");

  config.SetInclusionTags("DIRLIGHT");
  m_spDirLightPass = CreateSpecificTechnique("TerrainDetailPass", bUseDeferred, &config);
  VASSERT(m_spDirLightPass && "failed to create directional light terrain effect");

  config.SetInclusionTags("LIGHTMAP");
  m_spLightmapPass = CreateSpecificTechnique("TerrainDetailPass", bUseDeferred, &config);
  VASSERT(m_spLightmapPass && "failed to create lightmap terrain effect");

  config.SetInclusionTags("FOG");
  m_spFogPass = CreateSpecificTechnique("TerrainDetailPass", bUseDeferred, &config);
  VASSERT(m_spFogPass && "failed to create fog terrain effect");

  if (bUseDeferred) //in deferred we need a final clear pass that finally resets the alpha value
  {
    config.SetInclusionTags("CLEAR");
    m_spClearAlphaPass = CreateSpecificTechnique("TerrainDetailPass", bUseDeferred, &config);
    VASSERT(m_spClearAlphaPass && "failed to create clear terrain effect");
  }

  m_spWeightmapPass = CreateSpecificTechnique("TerrainWeightmapPass",bUseDeferred);
  VASSERT(m_spWeightmapPass && "failed to create terrain effect");

  m_spHoleOverlayPass = CreateSpecificTechnique("TerrainHoleOverlay",bUseDeferred);
  VASSERT(m_spHoleOverlayPass && "failed to create terrain effect");

  m_spLockedOverlayPass = CreateSpecificTechnique("TerrainLockedOverlay",bUseDeferred);
  VASSERT(m_spLockedOverlayPass && "failed to create terrain effect");

  m_spMiniMapPass = CreateSpecificTechnique("TerrainMiniMapPass",bUseDeferred);
  VASSERT(m_spMiniMapPass && "failed to create terrain effect");

  m_spHighlightMaterialIDPass = CreateSpecificTechnique("TerrainHighlightMaterialID",bUseDeferred);
  VASSERT(m_spHighlightMaterialIDPass && "failed to create terrain effect");
}

void VEditableTerrain::SetTerrainOrigin(const hkvVec3& vNewPos)
{
  if (vNewPos == m_Config.m_vTerrainPos)
    return;

  hkvVec3 vOldPos = m_Config.m_vTerrainPos;
  m_Config.m_vTerrainPos = vNewPos;
  m_SectorManager.m_bGlobalsCBDirty = true; // update constant buffer (DX10)
  FOREACH_SECTOR
    pSector->OnPositionChanged(vOldPos,vNewPos);
  }
}


VTerrainSector* VEditableTerrain::CreateSectorInstance(int iIndexX, int iIndexY)
{
  return new VEditableTerrainSector(&m_SectorManager,m_Config,iIndexX,iIndexY);
}



void VEditableTerrain::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnRenderHook)
  {
    VTerrain::OnHandleCallback(pData); // render terrain
    if (((VisRenderHookDataObject_cl *)pData)->m_iEntryConst==VRH_POST_TRANSPARENT_PASS_GEOMETRY)
    {
      if (m_bShow3DCursor)
        RenderCursor();
    }
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    VTerrain::OnHandleCallback(pData);
    if (m_fBlinkDuration>0.f)
    {
      m_fBlinkDuration -= Vision::GetUITimer()->GetTimeDifference();
      if (m_fBlinkDuration<=0.f)
        m_spBlinkChannel = NULL;
    }
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnGatherLightmapInfo)
  {
    if (m_iLightmapPageSize>0 || m_bCastStaticShadows)
      GatherLightmapInfo(((VLightmapInfoDataObject_cl *)pData)->m_SceneInfo);
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnLightmapFileLoaded)
  {
    if (m_iLightmapPageSize>0)
      GetLightmapInfo(((VLightmapInfoDataObject_cl *)pData)->m_SceneInfo);
//    else
//      SetFullbrightLightmaps(); // this would be correct but fatal for all large scenes with no lightmaps

    m_SectorManager.m_bNeedsLightgridUpdate = true;
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnResetStaticLighting)
  {
    if (m_iLightmapPageSize>0)
      SetFullbrightLightmaps();
    m_SectorManager.m_bNeedsLightgridUpdate = true;
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnRebuiltVisibility)
  {
    AddToSceneManager();
    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnRendererNodeChanged)
  {
    m_SectorManager.m_bNeedsLightgridUpdate = true;
    ReloadEditingShaders();
    return;
  }

  VTerrain::OnHandleCallback(pData);
}



VCompiledTechnique *VEditableTerrain::GetCursorEffect()
{
  return g_spCursorFX;
}


VTextureObject *VEditableTerrain::GetDefaultCursorTexture()
{
  if (!m_spDefaultCursorTexture)
    m_spDefaultCursorTexture = Vision::TextureManager.Load2DTexture("TerrainPlugin\\Cursor3D.dds",VTM_FLAG_DEFAULT_NON_MIPMAPPED);
  return m_spDefaultCursorTexture;
}




////////////////////////////////////////////////////////////////////////////////
// Update the cursor center for the projection shader
////////////////////////////////////////////////////////////////////////////////
void VEditableTerrain::UpdateCursorShader(VCompiledShaderPass *shader, const hkvVec3& vCenter, float fRadius, float fAngle, VTextureObject* pTexture, VColorRef iColor)
{
  float fSin = hkvMath::sinDeg (fAngle);
  float fCos = hkvMath::cosDeg (fAngle);
  hkvVec3 vRight(fCos,fSin,0.f);
  hkvVec3 vFront(fSin,-fCos,0.f);

  hkvPlane vPlaneX;
  vPlaneX.m_vNormal = vRight*(0.5f/fRadius);
  vPlaneX.m_fNegDist = -vPlaneX.m_vNormal.dot(vCenter-vRight*fRadius);
  hkvPlane vPlaneY;
  vPlaneY.m_vNormal = vFront*(0.5f/fRadius);
  vPlaneY.m_fNegDist = -vPlaneY.m_vNormal.dot(vCenter-vFront*fRadius);

  VShaderConstantBuffer *pVertexConst = shader->GetConstantBuffer(VSS_VertexShader);
  pVertexConst->SetSingleParameterF("CursorPlaneX", &vPlaneX.m_vNormal.x);
  pVertexConst->SetSingleParameterF("CursorPlaneY", &vPlaneY.m_vNormal.x);

  hkvVec4 vColor;
  iColor.ToFloat((hkvVec3&) vColor);
  vColor.w = 1.f;

  VShaderConstantBuffer *pPixelConst = shader->GetConstantBuffer(VSS_PixelShader);
  pPixelConst->SetSingleParameterF("CursorColor", vColor.data);

  // update texture
  if (!pTexture)
    pTexture = GetDefaultCursorTexture();
  int iSampler = shader->GetSamplerIndexByName(VSS_PixelShader, "CursorTexture");
  if (iSampler>=0)
  {
    VStateGroupTexture *pStateGroupTexture = shader->GetStateGroupTexture(VSS_PixelShader, iSampler);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = pTexture;
  }
}




void VEditableTerrain::RenderCursor()
{
  VCompiledTechnique *pFX = GetCursorEffect();
  if (!m_CursorBBox.IsValid() || !pFX || !m_SectorManager.m_pRenderCollector)
    return;


  // get the affected sectors from the cursor bounding box
  int x1,y1,x2,y2;
  m_CursorBBox.GetSectorIndices_Clamped(m_Config,x1,y1,x2,y2);

  m_AffectedByCursor.Clear();

  // collect affected geometry
  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
    {
      VTerrainSector *pSector = m_SectorManager.GetSector(x,y);
      if (!m_SectorManager.m_pRenderCollector->IsSectorVisible(pSector->VManagedResource::GetNumber()))
        continue;
      m_AffectedByCursor.AppendEntry(pSector);
    }

  VTextureObject* pDisplayTexture = m_spCursorTexture;
  if (!pDisplayTexture)
    pDisplayTexture = GetDefaultCursorTexture();

  // render again using Render function with specific shader
  for (int j=0;j<pFX->GetShaderCount();j++)
  {
    VCompiledShaderPass *pShader = pFX->GetShader(j);
    UpdateCursorShader(pShader,m_vCursorCenter.ToRenderSpace(m_Config),m_fCursorWSRadius, m_fCursorRotationAngle, pDisplayTexture, VColorRef(255,0,0,180));
    Vision::RenderLoopHelper.RenderStaticGeometryWithShader(m_AffectedByCursor,*pShader);

  }

/*
  int iMatID = this->m_SectorManager.GetMaterialIDAtWorldPos(m_vCursorCenter);
  Vision::Message.Print(1,100,100,"Mat ID: %i",iMatID);
*/
}


void VEditableTerrain::SetCursorPosFromTraceResult(const hkvVec3& vTraceStart,const hkvVec3& vTraceEnd, float fCursorRadius, float fAngle)
{
  m_CursorBBox.Reset();
  VisTraceLineInfo_t hitInfo;
  VLargePosition vAbsStart(m_Config,(hkvVec3& )vTraceStart);
  VLargePosition vAbsEnd(m_Config,(hkvVec3& )vTraceEnd);
  if (!m_SectorManager.GetAccurateTraceIntersection(vAbsStart,vAbsEnd, 1, &hitInfo))
    return;

  m_fCursorWSRadius = fCursorRadius;
  m_fCursorRotationAngle = fAngle;

  hkvVec3 vRad(fCursorRadius,fCursorRadius,1000.f);
  m_vCursorCenter.FromRenderSpace(m_Config,(hkvVec3& )hitInfo.touchPoint);
  m_CursorBBox.m_vMin = m_CursorBBox.m_vMax = m_vCursorCenter;
  m_CursorBBox.m_vMin.IncPos(m_Config,-vRad);
  m_CursorBBox.m_vMax.IncPos(m_Config,vRad);
}


float VEditableTerrain::GetHeightAtCursorPos() const
{
  return m_SectorManager.GetHeightAtWorldPos(m_vCursorCenter);
}


// flatten blending function
static hkvVec4 VISION_FASTCALL CombineFlatten(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor) 
{
  float fInt = hkvMath::Min(auxColor.r*srcColor.r,1.f);
  return hkvVec4(
    dstColor.r*(1.f-fInt)+auxColor.g*fInt, // lerp between current and destination height
    0,0,0);
}


void VEditableTerrain::CreateRotatedSourceCoordinates(float fAngle, hkvVec2 targetCoords[4])
{
  // y: mirrored
  float fSin = hkvMath::sinDeg (fAngle) * 0.5f;
  float fCos = hkvMath::cosDeg (fAngle) * 0.5f;
  hkvVec2 vRight(fCos,fSin);
  hkvVec2 vFront(fSin,-fCos);
  hkvVec2 vCenter(0.5f,0.5f);

  targetCoords[0] = vCenter - vRight - vFront;
  targetCoords[1] = vCenter + vRight - vFront;
  targetCoords[2] = vCenter - vRight + vFront;
  targetCoords[3] = vCenter + vRight + vFront;
}


void VEditableTerrain::BlitHeightBrush(VisBitmap_cl *pBitmap,float fIntensity, HMBlitMode_e eMode, float fHeight)
{
  if (eMode==Smooth)
  {
    BlitHeightBrushSmooth(pBitmap,fIntensity);
    return;
  }

  if (eMode==AddHoles)
  {
    BlitHoleBrush(pBitmap, false);
    return;
  }

  if (eMode==RemoveHoles)
  {
    BlitHoleBrush(pBitmap, true);
    return;
  }

  if (!m_CursorBBox.IsValid())
    return;

  // we need to ensure that we can write into this sector...
  if (!GetFileLocksOnSectorRange(m_CursorBBox,true,false))
    return;

  const int iCountX = m_Config.m_iOverallHeightSampleCount[0];
  const int iCountY = m_Config.m_iOverallHeightSampleCount[1];
  int x1,y1,x2,y2;
  m_Config.GetHeightSampleIndicesAtPos(m_CursorBBox.m_vMin,x1,y1);
  m_Config.GetHeightSampleIndicesAtPos(m_CursorBBox.m_vMax,x2,y2);
  if (x2<0 || y2<0 || x1>=iCountX || y1>=iCountY)
    return;

  int iOfsX = 0;
  int iOfsY = 0;
  int iOrigSizeX = x2-x1+1;
  int iOrigSizeY = y2-y1+1;
  if (x1<0) {iOfsX=x1;x1=0;}
  if (y1<0) {iOfsY=y1;y1=0;}
  if (x2>iCountX) x2=iCountX; // also allow last row
  if (y2>iCountY) y2=iCountY;

  VTerrainLockObject lock(x1,y1,x2-x1+1,y2-y1+1, 0);
  LockHeightValues(lock);

  // backup all affected sectors (if not already) to make the action undoable
  for (int y=lock.m_iSectorRect[1];y<=lock.m_iSectorRect[3];y++)
    for (int x=lock.m_iSectorRect[0];x<=lock.m_iSectorRect[2];x++)
    {
      VUndoableSectorActionPtr &action = SectorAction(x,y);
      if (!action.m_pPtr)
        action = new VSectorActionHeightfieldModified(GetSector(x,y));
    }

  hkvVec4 auxColor (0.0f);

  PixelCombineCallback combinemode;
  switch (eMode)
  {
    case Elevate:
      combinemode = BlittingHelpers::CombineAdditive;
      auxColor.r = fIntensity*100.f; // tweaked world units/s
      break;
    case Subtract:
      combinemode = BlittingHelpers::CombineAdditive;
      auxColor.r = -fIntensity*100.f; // tweaked world units/s
      break;
    case Flatten:
      combinemode = CombineFlatten;
      auxColor.r = fIntensity;
      auxColor.g = fHeight;
      break;

    default:
      return;
  }

  BitmapInfo_t targetInfo(BitmapInfo_t::FloatLuminance, lock.m_iSize[0], lock.m_iSize[1], lock.m_pData, lock.m_iStride);

  hkvVec2 texCoords[4];
  CreateRotatedSourceCoordinates(m_fCursorRotationAngle,texCoords);
  BlittingHelpers::StretchBlitBitmap(pBitmap, texCoords, targetInfo,iOfsX,iOfsY,iOrigSizeX,iOrigSizeY, auxColor, combinemode);
  UnlockHeightValues(lock);
}

void VEditableTerrain::BlitHeightBrushSmooth(VisBitmap_cl *pBitmap, float fIntensity)
{
  if (!m_CursorBBox.IsValid())
    return;

  // we need to ensure that we can write into this sector...
  if (!GetFileLocksOnSectorRange(m_CursorBBox,true,false))
    return;

  float fWeight = 1.f - hkvMath::pow (0.5f,fIntensity);
  
  // to scale from color to height
  static const float fScale = 1.f/255.f;

  // the maximum height value
  static const float fMaximum = 1.0f;

  // the maximum radius 
  static const int MASKRADIUSMAX = 5;

  // The max mask 'radius' for pure white in the brush
  static const float fGrayValueStep =  fMaximum / (float)MASKRADIUSMAX;

  // get and check the necessary parameters
  if (!m_CursorBBox.IsValid())
    return;
  const int iCountX = m_Config.m_iOverallHeightSampleCount[0] + 1;
  const int iCountY = m_Config.m_iOverallHeightSampleCount[1] + 1;
  int x1,y1,x2,y2;
  m_Config.GetHeightSampleIndicesAtPos(m_CursorBBox.m_vMin,x1,y1);
  m_Config.GetHeightSampleIndicesAtPos(m_CursorBBox.m_vMax,x2,y2);
  if (x2<0 || y2<0 || x1>=iCountX || y1>=iCountY)
    return;

  int xBrushOffset = 0;
  int yBrushOffset = 0;
  int xBrushSize = x2-x1+1;
  int yBrushSize = y2-y1+1;
  if (x1<0) {xBrushOffset=-x1;x1=0;}
  if (y1<0) {yBrushOffset=-y1;y1=0;}
  if (x2>=iCountX) x2=iCountX-1;
  if (y2>=iCountY) y2=iCountY-1;

  int iRect[4] = {x1,y1,x2-x1+1,y2-y1+1};
  int iSourceRect[4] = {iRect[0]-MASKRADIUSMAX,iRect[1]-MASKRADIUSMAX,iRect[2]+(2*MASKRADIUSMAX),iRect[3]+(2*MASKRADIUSMAX)};
  int iReadRect[4] = {iSourceRect[0],iSourceRect[1],iSourceRect[2],iSourceRect[3]};

  // copy the part that will be edited from the height field
  // to the source buffer

  // we can not read the part that is below x=0 of the height map
  if (iReadRect[0] < 0)
  {
    iReadRect[2] += iReadRect[0];
    iReadRect[0]=0;
  }
  // we can not read the part that is outside of the height map
  int iOversize = ((iSourceRect[0]+iSourceRect[2]) - iCountX);
  if (iOversize > 0)
  {
    iReadRect[2] -= iOversize;
  }
  // we can not read the part that is below y=0 of the height map
  if (iReadRect[1] < 0)
  {
    iReadRect[3] += iReadRect[1];
    iReadRect[1] = 0;
  }
  // we can not read the part that is outside of the height map
  iOversize = ((iSourceRect[1]+iSourceRect[3]) - iCountY);
  if (iOversize > 0)
  {
    iReadRect[3] -= iOversize;
  }

  m_fSourceBuffer.Resize(iSourceRect[2]*iSourceRect[3]);
  m_fDestinationBuffer.Resize(iRect[2]*iRect[3]);

  // lock the part in the height field that will be edited
  VTerrainLockObject lock(iReadRect[0],iReadRect[1],iReadRect[2],iReadRect[3], 0);
  LockHeightValues(lock);
  // backup all affected sectors (if not already) to make the action undoable
  for (int y=lock.m_iSectorRect[1];y<=lock.m_iSectorRect[3];y++)
    for (int x=lock.m_iSectorRect[0];x<=lock.m_iSectorRect[2];x++)
    {
      VUndoableSectorActionPtr &action = SectorAction(x,y);
      if (!action.m_pPtr)
        action = new VSectorActionHeightfieldModified(GetSector(x,y));
    }



  BitmapInfo_t targetInfo(BitmapInfo_t::FloatLuminance, lock.m_iSize[0], lock.m_iSize[1], lock.m_pData, lock.m_iStride);

  // get the first row of the height-field and initialize the index
  unsigned int iRunIdx = 0;
  for (int v=iSourceRect[1]; v < iSourceRect[1]+iSourceRect[3]; ++v)
  {
    // init the row index
    for (int u=iSourceRect[0]; u < iSourceRect[0]+iSourceRect[2]; ++u)
    {
      if (u < 0)
      {
        if (v < 0)
        {
          // use the value at the beginning of this row for all
          // indices outside the height field on the negative side
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(0,0).x;
        }
        else if (v >= iCountY)
        {
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(0,iReadRect[3]-1).x;
        }
        else
        {
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(0,v-iReadRect[1]).x;
        }
      }
      else if (u >= iCountX)
      {
        if (v < 0)
        {
          // use the value at the end of this row for all
          // indices outside the height field on the negative side
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(iReadRect[2]-1,0).x;
        }
        else if (v >= iCountY)
        {
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(iReadRect[2]-1,iReadRect[3]-1).x;
        }
        else
        {
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(iReadRect[2]-1,v-iReadRect[1]).x;
        }
      }
      else
      {
        if (v < 0)
        {
          // use the value at the end of this row for all
          // indices outside the height field on the negative side
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(u-iReadRect[0],0).x;
        }
        else if (v >= iCountY)
        {
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(u-iReadRect[0],iReadRect[3]-1).x;
        }
        else
        {
          // write all read indices to the source buffer
          m_fSourceBuffer[iRunIdx] = targetInfo.GetColorAt(u-iReadRect[0],v-iReadRect[1]).x;
        }
      }
      ++iRunIdx;
    }
  }


  // copy the part of the brush which is over the height field
  const float fScaleU = (float)pBitmap->GetWidth()/(float)xBrushSize;
  const float fScaleV = (float)pBitmap->GetHeight()/(float)yBrushSize;
  iRunIdx = 0;
  m_fBrushBuffer.Resize(xBrushSize*yBrushSize);
  for (int v=yBrushOffset; v < yBrushOffset+iRect[3]; ++v)
  {
    for (int u=xBrushOffset; u < xBrushOffset+iRect[2]; ++u, ++iRunIdx)
    {
      // copy the values
      m_fBrushBuffer[iRunIdx] = (float)pBitmap->LookupTexelColor((int)(u*fScaleU),(int)(v*fScaleV)).r * fScale;     
    }
  }

	// now the smoothing starts

  // start at the first position of the source buffer which is
  // copied from the height field
	int iSourceIdx = MASKRADIUSMAX*(iSourceRect[2]+1);
	int iDestinationIdx = 0;
  for (int y=0;y<iRect[3];++y, iSourceIdx += 2*MASKRADIUSMAX)
  {
    for (int x=0; x<iRect[2]; ++x, ++iDestinationIdx, ++iSourceIdx)
	  {
      // if the value in the brush is to small -> keep the height
      if (m_fBrushBuffer[iDestinationIdx] < fGrayValueStep)
      {
        m_fDestinationBuffer[iDestinationIdx] = m_fSourceBuffer[iSourceIdx];
        continue;
      }

      // start with zero
      m_fDestinationBuffer[iDestinationIdx] = 0.0f;

      // initialize the mask parameters dependent on the value of the brush
      // we use the red color channel to get the radius of the smoothing
      unsigned int uiMaskSize = (int)(m_fBrushBuffer[iDestinationIdx] / fGrayValueStep);
      unsigned int uiMaskSideLength = (2*uiMaskSize)+1;
      unsigned int uiMaskIdx = 0;

      // start at the top left corner of the mask
      unsigned int uiMaskSourceIdx = iSourceIdx - (uiMaskSize * (iSourceRect[2]+1));
      for (unsigned int u=0; u<uiMaskSideLength; ++u, uiMaskSourceIdx += (iSourceRect[2]-uiMaskSideLength))
      {
        // TODO:is it cheaper to remember the beginning of the row
        // and add v each time?
        for (unsigned int v=0; v<uiMaskSideLength; ++v, ++uiMaskIdx, ++uiMaskSourceIdx)
        {
          // sum all the values of the source buffer under the mask
          m_fDestinationBuffer[iDestinationIdx] += m_fSourceBuffer[uiMaskSourceIdx];
        }
      }
      // and take the arithmetic mean of the mask
      m_fDestinationBuffer[iDestinationIdx] /= (uiMaskSideLength * uiMaskSideLength);

      // dependent on the time, move a step to the destination height
      m_fDestinationBuffer[iDestinationIdx] = m_fSourceBuffer[iSourceIdx] + ((m_fDestinationBuffer[iDestinationIdx]-m_fSourceBuffer[iSourceIdx]) * fWeight);
    }
  }

  // we need to write just the part of the height field
  // that lies under the brush and therefore might has been edited
  int iYOffset = MASKRADIUSMAX;
  if (iSourceRect[1] < 0)
  {
    // if part of the source buffer lies outside the height field on
    // the negative y side the offset has to be reduced
    iYOffset = (MASKRADIUSMAX+iSourceRect[1]);
  }
  int iXOffset = MASKRADIUSMAX;
  if (iSourceRect[0] < 0)
  {
    // if part of the source buffer lies outside the height field on
    // the negative x side the offset has to be reduced 
    iXOffset += iSourceRect[0];
  }

  // write the part which is modified back to the height field
  iRunIdx = 0;
  for (int v=iYOffset; v < iRect[3]+iYOffset; ++v)
  {
    for (int u=iXOffset; u < iRect[2]+iXOffset; ++u, ++iRunIdx)
    {
      float fH = m_fDestinationBuffer[iRunIdx];
      targetInfo.SetColorAt(u,v,hkvVec4 (fH,fH,fH, 1.0f));
    }
  }

  // unlock the height field
 UnlockHeightValues(lock);

  // put decoration on ground
//  m_SectorManager.UpdateDecorationChannels(m_CursorBBox,-1,NULL); 
}



void VEditableTerrain::BlitVegetationMap(IVTerrainDecorationModel *pModel, VisBitmap_cl *pBrush, float fIntensity, bool bSubtract)
{

  if (!m_CursorBBox.IsValid() || !pBrush || !pModel)
    return;

  // we need to ensure that we can write into this sector...
  if (!GetFileLocksOnSectorRange(m_CursorBBox,true,false))
    return;

  // we assume that one channel type has constant density map over the terrain
  int iResX = m_Config.m_iDensityMapSamplesPerSector[0];
  int iResY = m_Config.m_iDensityMapSamplesPerSector[1];

  const int iCountX = iResX*m_Config.m_iSectorCount[0];
  const int iCountY = iResY*m_Config.m_iSectorCount[1];
  int x1,y1,x2,y2;
  pModel->GetDensitySampleIndicesAtPos(m_Config, m_CursorBBox.m_vMin, x1,y1);
  pModel->GetDensitySampleIndicesAtPos(m_Config, m_CursorBBox.m_vMax, x2,y2);
  if (x2<0 || y2<0 || x1>=iCountX || y1>=iCountY)
    return;

//  int iOfsX = 0;
//  int iOfsY = 0;
  if (x1<0) {/*iOfsX=-x1;*/x1=0;}
  if (y1<0) {/*iOfsY=-y1;*/y1=0;}
  if (x2>=iCountX) x2=iCountX-1;
  if (y2>=iCountY) y2=iCountY-1;

  VTerrainLockObject lock(x1,y1,x2-x1+1,y2-y1+1, 0);
  LockDecorationChannel(lock,pModel);

  // backup all affected sectors (if not already) to make the action undoable
  for (int y=lock.m_iSectorRect[1];y<=lock.m_iSectorRect[3];y++)
    for (int x=lock.m_iSectorRect[0];x<=lock.m_iSectorRect[2];x++)
    {
      VUndoableSectorActionPtr &action = SectorAction(x,y);
      if (!action.m_pPtr)
        action = new VSectorActionDecoChannelModified(GetSector(x,y), pModel);
    }


  // update target
  if (bSubtract)
    fIntensity = -fIntensity;
  BitmapInfo_t targetInfo(BitmapInfo_t::ByteLuminance, lock.m_iSize[0], lock.m_iSize[1], lock.m_pData, lock.m_iStride);

  hkvVec4 auxColor(fIntensity,fIntensity,fIntensity,fIntensity); // uses red component only
  hkvVec2 texCoords[4];
  CreateRotatedSourceCoordinates(m_fCursorRotationAngle,texCoords);
  BlittingHelpers::StretchBlitBitmap(pBrush, texCoords, targetInfo,0,0,-1,-1, auxColor, BlittingHelpers::CombineAdditive);

  UnlockDecorationChannel(lock, false);
}


void VEditableTerrain::BlitDetailTexture(VTextureWeightmapChannelResource *pTarget, VisBitmap_cl *pBrush, float fIntensity, bool bSubtract)
{
  if (!m_CursorBBox.IsValid() || !pBrush || !pTarget)
    return;

  // we need to ensure that we can write into this sector...
  if (!GetFileLocksOnSectorRange(m_CursorBBox,true,false))
    return;

  int sx1,sy1,sx2,sy2;
  m_CursorBBox.GetSectorIndices_Clamped(m_Config,sx1,sy1,sx2,sy2);

  hkvVec4 auxColorAdd(fIntensity,fIntensity,fIntensity,0.f);
  hkvVec4 auxColorSub(-fIntensity,-fIntensity,-fIntensity,0.f);

  // backup all affected sectors (if not already) to make the action undoable
  for (int y=sy1;y<=sy2;y++)
    for (int x=sx1;x<=sx2;x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      pSector->EnsureLoaded();

      // make sure the sector has this channel allocated:
      int iResX = m_Config.m_iDefaultWeightmapResolution[0]; // this might become sector specific
      int iResY = m_Config.m_iDefaultWeightmapResolution[1];
      VTextureWeightmapChannel *pFirst = pSector->m_UsedWeightmapChannels.CreateChannel(m_Config,pTarget,iResX,iResY); // this keeps the list sorted
      VASSERT(pFirst);

      // backup all channels
      VUndoableSectorActionPtr &action = SectorAction(x,y);
      if (!action.m_pPtr)
      {
        action = new VWeightmapChannelsModified(pSector,pSector->m_UsedWeightmapChannels);
      }

      // now blit into targets; start with the one we want to edit
      int iFirst = pSector->m_UsedWeightmapChannels.Find(pFirst);
      hkvVec2 texCoords[4];
      CreateRotatedSourceCoordinates(m_fCursorRotationAngle,texCoords);

      // blit into all channels: Use additive blending for the channel itself and subtractive for all others
      for (int i=iFirst;i<pSector->m_UsedWeightmapChannels.Count();i++)
      {
        VTextureWeightmapChannel *pCurrent = pSector->m_UsedWeightmapChannels.GetAt(i);
        
        // evaluate affected region:
        int x1,y1,x2,y2;
        pSector->GetTexelIndices(m_CursorBBox.m_vMin, x1,y1, pCurrent->m_iSizeX, pCurrent->m_iSizeY);
        pSector->GetTexelIndices(m_CursorBBox.m_vMax, x2,y2, pCurrent->m_iSizeX, pCurrent->m_iSizeY);

        const hkvVec4& auxColor = (i==iFirst && !bSubtract) ? auxColorAdd : auxColorSub;
        BitmapInfo_t targetInfo(BitmapInfo_t::ByteLuminance, pCurrent->m_iSizeX, pCurrent->m_iSizeY, pCurrent->GetValuePtr(0,0), pCurrent->m_iSizeX);
        BlittingHelpers::StretchBlitBitmap(pBrush, texCoords, targetInfo,x1,y1,x2-x1+1,y2-y1+1, auxColor, BlittingHelpers::CombineAddSrcLuminance);
        pCurrent->SetDirtyFlag();
        if (bSubtract) // don't update other layers when we subtract anyway
          break;
      }

      pSector->SetEditorFlag(SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY|SECTOR_EDITORFLAG_SECTORFILEDIRTY);

    }
}



void VEditableTerrain::BlitHoleBrush(VisBitmap_cl *pBrush, bool bCloseHoles)
{
  if (!m_CursorBBox.IsValid() || !pBrush)
    return;

  // we need to ensure that we can write into this sector...
  if (!GetFileLocksOnSectorRange(m_CursorBBox,true,false))
    return;

  int x1,y1,x2,y2;
  m_Config.GetTileIndicesAtPos(m_CursorBBox.m_vMin,x1,y1);
  m_Config.GetTileIndicesAtPos(m_CursorBBox.m_vMax,x2,y2);
  VTerrainLockObject lock(x1,y1,x2-x1+1,y2-y1+1, 0);
  LockHoleMask(lock);

  // backup all affected sectors (if not already) to make the action undoable
  for (int y=lock.m_iSectorRect[1];y<=lock.m_iSectorRect[3];y++)
    for (int x=lock.m_iSectorRect[0];x<=lock.m_iSectorRect[2];x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      pSector->EnsureLoaded();

      // backup all channels
      VUndoableSectorActionPtr &action = SectorAction(x,y);
      if (!action.m_pPtr)
        action = new VSectorActionHolesModified(pSector);

      pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
      pSector->InvalidateMesh();
    }

  BitmapInfo_t targetInfo(BitmapInfo_t::ByteLuminance, lock.m_iSize[0], lock.m_iSize[1], lock.m_pData, lock.m_iStride);

  float fIntensity = bCloseHoles ? 0.f : 1.f;
  hkvVec4 auxColor(fIntensity,fIntensity,fIntensity,fIntensity); // uses red component only
  hkvVec2 texCoords[4];
  CreateRotatedSourceCoordinates(m_fCursorRotationAngle,texCoords);
  BlittingHelpers::StretchBlitBitmap(pBrush, texCoords, targetInfo,0,0,-1,-1, auxColor, BlittingHelpers::BooleanMask);

  UnlockHoleMask(lock);

}


/*VFileLock& VEditableTerrain::GetSectorFileLockObject(int iSectorX, int iSectorY)
{
  return GetSector(iSectorX,iSectorY)->m_SectorFileLock;
}*/


bool VEditableTerrain::GetFileLocksOnSectorRange(const VLargeBoundingBox &range, bool bReturnOnFail, bool bTestOnly)
{
  if (!m_bCheckSectorLocks)
    return true;

  int x1,y1,x2,y2;
  range.GetSectorIndices_Clamped(m_Config,x1,y1,x2,y2);
  
  bool bResult = true;
  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
    {
      VEditableTerrainSector *pSector = (VEditableTerrainSector *)m_SectorManager.GetSector(x,y);
      if (pSector->OwnsFileLock())
        continue; //already locked by us
      if (!pSector->GetFileLock())
      {
        bResult = false;
        VASSERT(!pSector->IsEditorFlagSet(SECTOR_EDITORFLAG_OWNSLOCK));
        pSector->SetEditorFlag(SECTOR_EDITORFLAG_NETWORKLOCK);
        if (bReturnOnFail)
          return bResult;
        continue;
      }

      // release the lock again
      if (bTestOnly)
        pSector->ReleaseFileLock();

      // also set a flag on the sector for vForge
      pSector->SetEditorFlag(SECTOR_EDITORFLAG_OWNSLOCK);
      pSector->RemoveEditorFlag(SECTOR_EDITORFLAG_NETWORKLOCK);
    }
  return bResult;
}



void VEditableTerrain::ReleaseFileLocksOnSectorRange(const VLargeBoundingBox &range, bool bSaveSectors)
{
  if (!m_SectorManager.m_pSector)
    return;
  int x1,y1,x2,y2;
  range.GetSectorIndices_Clamped(m_Config,x1,y1,x2,y2);
  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      if (!pSector->OwnsFileLock())
        continue;

      // save the sector now to disk so it is accessible for other users
      if (bSaveSectors)
        pSector->SaveSectorFinal();

      // after that, release the file lock
      pSector->ReleaseFileLock();
      // also set a flag on the sector for vForge
      pSector->RemoveEditorFlag(SECTOR_EDITORFLAG_OWNSLOCK);

    }
}


void VEditableTerrain::ReleaseAllSectorFileLocks(bool bSaveSectors)
{
  VLargeBoundingBox terrainbox;
  m_Config.GetTerrainBBox(terrainbox);
  ReleaseFileLocksOnSectorRange(terrainbox,bSaveSectors);
}


void VEditableTerrain::AllocateSectorActions()
{
  VASSERT(m_pSectorAction==NULL);
  m_pSectorAction = new VUndoableSectorActionPtr[m_Config.m_iSectorCount[0]*m_Config.m_iSectorCount[1]];
}



void VEditableTerrain::OnSectorUpdateFinished(VUndoableSectorActionCollection* pCollection)
{
  if (!m_pSectorAction)
    return;

  // fill the collection with all affected sectors
  const int iSectorCount = m_Config.m_iSectorCount[0]*m_Config.m_iSectorCount[1];

  for (int i=0;i<iSectorCount;i++)
  {
    if (m_pSectorAction[i])
    {
      m_pSectorAction[i]->GetData();
      if (pCollection)
        pCollection->Add(m_pSectorAction[i]);
      m_pSectorAction[i] = NULL; // remove from list again
    }
  }
}



bool VEditableTerrain::ApplyFilter(IVTerrainFilter *pFilter, const VLargeBoundingBox& affectedArea, void *pUserData, IVLog *pLog)
{
  VASSERT(pFilter && affectedArea.IsValidated(m_Config));

  // initialize the filter members
  pFilter->m_pLog = pLog;
  pFilter->m_pTerrain = this;
  pFilter->m_AffectedArea = affectedArea;

  if (!pFilter->Init(affectedArea,pUserData))
  {
    pFilter->DeInit(pUserData);
    return false;
  }

  // get file locks?


  // clamp the sector area
/*
  int x1,y1,x2,y2;
  affectedArea.GetSectorIndices_Clamped(x1,y1,x2,y2);

  // run the filter over all affected sectors
  for (int y=y1;y<=y2;y++)
    for (int x=x1;x<=x2;x++)
    {
      VTerrainSector *pSector = m_SectorManager.GetSector(x,y);
      pFilter->ApplyToSector(pSector,pUserData); 
    }
  */  
  pFilter->Do(pUserData);
  pFilter->DeInit(pUserData);
  return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// height value related functions
///////////////////////////////////////////////////////////////////////////////////////////////

void VEditableTerrain::LockHeightValues(VTerrainLockObject &lockObj)
{
  VASSERT(lockObj.m_bLocked==false);
  lockObj.m_bLocked = true;
  lockObj.m_pTerrain = this;
  if (!lockObj.IsValid())
    return;

  // setup data pointer
  int iSize = lockObj.GetArea();
  float *pBuffer = (float *)GetLockBuffer(iSize*sizeof(float));
  lockObj.m_iComponentSize = sizeof(float);
  lockObj.m_iStride = lockObj.m_iSize[0];
  lockObj.m_pData = pBuffer;

  // this is not required...
//  VASSERT(lockObj.m_iPos[0]>=0 && lockObj.m_iPos[0]+lockObj.m_iSize[0]<=m_Config.GetOverallHeightSampleCountX());
//  VASSERT(lockObj.m_iPos[1]>=0 && lockObj.m_iPos[1]+lockObj.m_iSize[1]<=m_Config.GetOverallHeightSampleCountY());

  if (lockObj.m_iLockFlags&VIS_LOCKFLAG_DISCARDABLE)
    return;

  // copy height field data
  int sx1,sy1,sx2,sy2;
  m_Config.GetSectorIndices_Clamped(lockObj.m_iPos[0], lockObj.m_iPos[1], sx1,sy1); 
  m_Config.GetSectorIndices_Clamped(lockObj.m_iPos[0]+lockObj.m_iSize[0]-1, lockObj.m_iPos[1]+lockObj.m_iSize[1]-1, sx2,sy2); 
  lockObj.SetSectorRange(sx1,sy1,sx2,sy2);
  bool bReadOnly = (lockObj.m_iLockFlags & VIS_LOCKFLAG_READONLY) > 0;
  for (int y=sy1;y<=sy2;y++)
    for (int x=sx1;x<=sx2;x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      BOOL bOldLoaded = pSector->IsLoaded();
      pSector->EnsureLoaded();
      pSector->CopyRelevantHeightValues(lockObj);
      if (bReadOnly && !bOldLoaded) // be memory friendly
        pSector->EnsureUnloaded();
    }
}

void VEditableTerrain::UnlockHeightValues(VTerrainLockObject &lockObj)
{
  VASSERT(lockObj.m_bLocked==true);
  lockObj.m_bLocked = false;
  if (!lockObj.IsValid() || lockObj.m_iLockFlags&VIS_LOCKFLAG_READONLY)
    return;

  // copy back height map values
  // 2 samples are added in both directions to ensure that we copy the overlapping part into both sectors
  int sx1,sy1,sx2,sy2;
  m_Config.GetSectorIndices_Clamped(lockObj.m_iPos[0]-2, lockObj.m_iPos[1]-2, sx1,sy1); 
  m_Config.GetSectorIndices_Clamped(lockObj.m_iPos[0]+lockObj.m_iSize[0]+1, lockObj.m_iPos[1]+lockObj.m_iSize[1]+1, sx2,sy2); 

  for (int y=sy1;y<=sy2;y++)
    for (int x=sx1;x<=sx2;x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      pSector->EnsureLoaded();
      pSector->GetRelevantHeightValues(lockObj); // copy back the values
      pSector->InvalidateMesh();
      pSector->InvalidateRelevantCollisionMeshes(&lockObj);
    }
}



void VEditableTerrain::UpdatePhysics()
{
  IVisPhysicsModule_cl *pModule = Vision::GetApplication()->GetPhysicsModule();
  for (int i=0;i<m_PendingPhysicsUpdateSectors.Count();i++)
  {
    VEditableTerrainSector *pSector = m_PendingPhysicsUpdateSectors.GetAt(i);
    if (!pSector->IsLoaded() || pModule==NULL)
      continue;
    // TODO: provide a dedicated function on the module that can take an optimized path
    pModule->OnTerrainSectorRemoved(pSector);
    pModule->OnTerrainSectorCreated(pSector);
  }
  m_PendingPhysicsUpdateSectors.Clear();
}


void VEditableTerrain::LockHoleMask(VTerrainLockObject &lockObj)
{
  VASSERT(lockObj.m_bLocked==false);
  lockObj.m_bLocked = true;
  lockObj.m_pTerrain = this;
  if (!lockObj.IsValid())
    return;

  // setup data pointer
  int iSize = lockObj.GetArea();
  bool *pBuffer = (bool *)GetLockBuffer(iSize*sizeof(bool));
  lockObj.m_iComponentSize = sizeof(bool);
  lockObj.m_iStride = lockObj.m_iSize[0];
  lockObj.m_pData = pBuffer;

  if (lockObj.m_iLockFlags&VIS_LOCKFLAG_DISCARDABLE)
    return;

  // copy hole mask
  int sx1,sy1,sx2,sy2;
  sx1 = m_Config.GetSectorIndexAtTilePosX(lockObj.m_iPos[0]);
  sy1 = m_Config.GetSectorIndexAtTilePosY(lockObj.m_iPos[1]);
  sx2 = m_Config.GetSectorIndexAtTilePosX(lockObj.m_iPos[0] + lockObj.m_iSize[0]-1);
  sy2 = m_Config.GetSectorIndexAtTilePosY(lockObj.m_iPos[1] + lockObj.m_iSize[1]-1);
  m_Config.ClampSectorRectangle(sx1,sy1,sx2,sy2);
  lockObj.SetSectorRange(sx1,sy1,sx2,sy2);

  for (int y=sy1;y<=sy2;y++)
    for (int x=sx1;x<=sx2;x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      pSector->EnsureLoaded();
      pSector->CopyRelevantHoleMask(lockObj);
    }
}

void VEditableTerrain::UnlockHoleMask(VTerrainLockObject &lockObj)
{
  VASSERT(lockObj.m_bLocked==true);
  lockObj.m_bLocked = false;
  if (!lockObj.IsValid() || lockObj.m_iLockFlags&VIS_LOCKFLAG_READONLY)
    return;

  // copy back hole mask values
  for (int y=lockObj.m_iSectorRect[1];y<=lockObj.m_iSectorRect[3];y++)
    for (int x=lockObj.m_iSectorRect[0];x<=lockObj.m_iSectorRect[2];x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      pSector->EnsureLoaded();
      pSector->GetRelevantHoleMask(lockObj); // copy back the values
    }
}


void VEditableTerrain::LockDetailTexture(VTerrainLockObject &lockObj, VTextureWeightmapChannelResource *pTexture)
{
  VASSERT(lockObj.m_bLocked==false);
  lockObj.m_bLocked = true;
  lockObj.m_pTerrain = this;
  if (!lockObj.IsValid())
    return;

  // setup data pointer
  int iSize = lockObj.GetArea();
  UBYTE *pBuffer = (UBYTE *)GetLockBuffer(iSize);
  lockObj.m_iComponentSize = sizeof(UBYTE);
  lockObj.m_iStride = lockObj.m_iSize[0];
  lockObj.m_pData = pBuffer;
  lockObj.m_iReserved[0] = pTexture ? pTexture->m_Properties.m_iTextureID : -1;
  int iResX = m_Config.m_iDefaultWeightmapResolution[0];
  int iResY = m_Config.m_iDefaultWeightmapResolution[1];
//  VASSERT(lockObj.m_iPos[0]>=0 && lockObj.m_iPos[0]+lockObj.m_iSize[0]<=iResX*m_Config.m_iSectorCount[0]);
//  VASSERT(lockObj.m_iPos[1]>=0 && lockObj.m_iPos[1]+lockObj.m_iSize[1]<=iResY*m_Config.m_iSectorCount[1]);

  int sx1,sy1,sx2,sy2;
  sx1 = lockObj.m_iPos[0] / iResX;
  sx2 = (lockObj.m_iPos[0]+lockObj.m_iSize[0]-1) / iResX;
  sy1 = lockObj.m_iPos[1] / iResY;
  sy2 = (lockObj.m_iPos[1]+lockObj.m_iSize[1]-1) / iResY;
  m_Config.ClampSectorRectangle(sx1,sy1,sx2,sy2);
  lockObj.SetSectorRange(sx1,sy1,sx2,sy2);

  if (lockObj.m_iLockFlags&VIS_LOCKFLAG_DISCARDABLE)
    return;

  // copy density map data
  memset(pBuffer,0,iSize); // some of the sectors don't write into the buffer
  if (!pTexture)
    return;

  bool bReadOnly = (lockObj.m_iLockFlags&VIS_LOCKFLAG_READONLY)!=0;
  for (int y=sy1;y<=sy2;y++)
    for (int x=sx1;x<=sx2;x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      BOOL bOldLoaded = pSector->IsLoaded();
      pSector->EnsureLoaded();
      pSector->CopyRelevantDetailTextureValues(lockObj);
      if (bReadOnly && !bOldLoaded) // be memory friendly
        pSector->EnsureUnloaded();
    }
}

void VEditableTerrain::UnlockDetailTexture(VTerrainLockObject &lockObj)
{
  VASSERT(lockObj.m_bLocked==true);
  lockObj.m_bLocked = false;
  if (!lockObj.IsValid() || lockObj.m_iLockFlags&VIS_LOCKFLAG_READONLY)
    return;
  for (int y=lockObj.m_iSectorRect[1];y<=lockObj.m_iSectorRect[3];y++)
    for (int x=lockObj.m_iSectorRect[0];x<=lockObj.m_iSectorRect[2];x++)
      GetSector(x,y)->GetRelevantDetailTextureValues(lockObj);
}



///////////////////////////////////////////////////////////////////////////////////////////////
// decoration related functions
///////////////////////////////////////////////////////////////////////////////////////////////

void VEditableTerrain::LockDecorationChannel(VTerrainLockObject &lockObj, IVTerrainDecorationModel *pModelResource)
{
  VASSERT(lockObj.m_bLocked==false);
  lockObj.m_bLocked = true;
  lockObj.m_pTerrain = this;
  if (!lockObj.IsValid())
    return;

  // setup data pointer
  int iSize = lockObj.GetArea();
  UBYTE *pBuffer = (UBYTE *)GetLockBuffer(iSize);
  lockObj.m_iComponentSize = sizeof(UBYTE);
  lockObj.m_iStride = lockObj.m_iSize[0];
  lockObj.m_pData = pBuffer;
  lockObj.m_iReserved[0] = pModelResource ? pModelResource->GetID() : -1;
  int iResX = m_Config.m_iDensityMapSamplesPerSector[0];
  int iResY = m_Config.m_iDensityMapSamplesPerSector[1];
  //VASSERT(lockObj.m_iPos[0]>=0 && lockObj.m_iPos[0]+lockObj.m_iSize[0]<=iResX*m_Config.m_iSectorCount[0]);
  //VASSERT(lockObj.m_iPos[1]>=0 && lockObj.m_iPos[1]+lockObj.m_iSize[1]<=iResY*m_Config.m_iSectorCount[1]);

  int sx1,sy1,sx2,sy2;
  sx1 = lockObj.m_iPos[0] / iResX;
  sx2 = (lockObj.m_iPos[0]+lockObj.m_iSize[0]-1) / iResX;
  sy1 = lockObj.m_iPos[1] / iResY;
  sy2 = (lockObj.m_iPos[1]+lockObj.m_iSize[1]-1) / iResY;
  m_Config.ClampSectorRectangle(sx1,sy1,sx2,sy2);
  lockObj.SetSectorRange(sx1,sy1,sx2,sy2);

  if (lockObj.m_iLockFlags&VIS_LOCKFLAG_DISCARDABLE)
    return;

  // copy density map data
  memset(pBuffer,0,iSize); // some of the sectors don't write into the buffer
  if (!pModelResource)
    return;

  bool bReadOnly = (lockObj.m_iLockFlags&VIS_LOCKFLAG_READONLY)!=0;
  for (int y=sy1;y<=sy2;y++)
    for (int x=sx1;x<=sx2;x++)
    {
      VEditableTerrainSector *pSector = GetSector(x,y);
      BOOL bOldLoaded = pSector->IsLoaded();
      pSector->EnsureLoaded();
      pSector->CopyRelevantDensityMap(lockObj);
      if (bReadOnly && !bOldLoaded) // be memory friendly
        pSector->EnsureUnloaded();
    }
}

void VEditableTerrain::UnlockDecorationChannel(VTerrainLockObject &lockObj, bool bFinalUpdate)
{
  VASSERT(lockObj.m_bLocked==true);
  lockObj.m_bLocked = false;
  if (!lockObj.IsValid() || lockObj.m_iLockFlags&VIS_LOCKFLAG_READONLY)
    return;
  for (int y=lockObj.m_iSectorRect[1];y<=lockObj.m_iSectorRect[3];y++)
    for (int x=lockObj.m_iSectorRect[0];x<=lockObj.m_iSectorRect[2];x++)
      GetSector(x,y)->GetRelevantDensityMap(lockObj, bFinalUpdate);
}



void VEditableTerrain::UpdateDecoration(bool bFullTerrain)
{
  FOREACH_SECTOR
    if (!bFullTerrain && !m_DirtyVegetationSectors.Contains(pSector))
      continue;
    BOOL bOldLoaded = pSector->IsLoaded();
    pSector->EnsureLoaded();
    pSector->UpdateDecoration(true);
    if (!bOldLoaded)
      pSector->EnsureUnloaded();
  }
}



void VEditableTerrain::SetUseBakedTextures(bool bStatus, bool bAllowRebaking)
{
  m_bUseBakedTextures = bStatus;

  if (!m_bUseBakedTextures || !bAllowRebaking)
    return;

  bool bOldUseThreeWayMapping = m_Config.m_bUseThreeWayMapping;
  UpdateDefaultEffectSettingsInConfig();

  // actually perform the baking:
  FOREACH_SECTOR
    if (!pSector->IsEditorFlagSet(SECTOR_EDITORFLAG_WEIGHTMAPSDIRTY) &&
      !pSector->IsEditorFlagSet(SECTOR_EDITORFLAG_HEIGHTMAPDIRTY) &&
      bOldUseThreeWayMapping == m_Config.m_bUseThreeWayMapping)
    {
      continue;
    }
    BOOL bOldStatus = pSector->IsLoaded();
    pSector->EnsureLoaded();
    pSector->SaveSector();
    if (!bOldStatus)
      pSector->EnsureUnloaded();
  }
}



void VEditableTerrain::SetPreviewDirectionalLight(bool bStatus)
{
  m_bPreviewDirectionalLight = bStatus;
}


void VEditableTerrain::SetDirectionalLightProperties(const hkvVec3& vDir, const hkvVec4& vLightColor, const hkvVec4& vAmbientColor)
{
  VTerrain::SetDirectionalLightProperties(vDir,vLightColor,vAmbientColor);
  if (m_spDirLightPass==NULL || m_spDirLightPass->GetShaderCount()==0)
    return;
    
  hkvVec3 ndir(-vDir);
  ndir.normalizeIfNotZero();
  VCompiledShaderPass* pLightingPass = m_spDirLightPass->GetShader(0);
  pLightingPass->GetConstantBuffer(VSS_PixelShader)->SetSingleParameterF("LightDirection",ndir.getAsDirection ().data);
  pLightingPass->GetConstantBuffer(VSS_PixelShader)->SetSingleParameterF("DirectionalLightColor",vLightColor.data);
  pLightingPass->GetConstantBuffer(VSS_PixelShader)->SetSingleParameterF("AmbientColor",vAmbientColor.data);
}


void VEditableTerrain::BlinkDetailTexture(VTextureWeightmapChannelResource *pRes, float fDuration)
{
  if (!pRes)
    fDuration = 0.f;
  m_spBlinkChannel = pRes; // the texture that is currently used for highlighting
  m_fBlinkDuration = fDuration;

}


// Custom lightmap primitive class so that terrains can respond to live lightmap update
class VTerrainLightmapPrimitive : public VLightmapPrimitive
{
public:
  VTerrainLightmapPrimitive(VEditableTerrain *pTerrain)
    : m_pTerrain(pTerrain)
  {
  }

  // see also VEditableTerrain::GetLightmapInfo
  virtual void OnLiveUpdate(int iMaterialIndex, int iTextureSubIndex, VTextureObject *pLightmap, const float *pAtlasScaleOfs) HKV_OVERRIDE
  {
    VLightmapPrimitive::MeshMaterial_t &mat(GetMaterial(iMaterialIndex));

    unsigned int iUserData = mat.m_iUserData;
    bool bSinglePageLM = iUserData==0xffffffff;
    const VTerrainConfig &cfg(m_pTerrain->Config());

    char szSamplerName[64];
    if (iTextureSubIndex==0)
      sprintf(szSamplerName,"Lightmap");
    else
      sprintf(szSamplerName,"Lightmap%i",iTextureSubIndex);

    // this loop covers both per sector and single page lightmaps:
    for (int sy=0;sy<cfg.m_iSectorCount[1];sy++)
      for (int sx=0;sx<cfg.m_iSectorCount[0];sx++)
      {
        VEditableTerrainSector *pSector = m_pTerrain->GetSector(sx,sy);
        if ((iUserData != ((sy<<16) + sx)) && !bSinglePageLM)
          continue;
        if (!pSector->IsLoaded() || pSector->m_spMesh==NULL)
          break;
       
        VTerrainSectorMeshPageInfo *pPage = pSector->m_pMeshPage;
        for (int iPageY=0;iPageY<pSector->m_Config.m_iSectorMeshesPerSector[1];iPageY++)
        {
          for (int iPageX=0;iPageX<pSector->m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
          {
            int iSampler = pPage->GetSamplerIndex(szSamplerName);
            if (iSampler<0)
              continue;

            pPage->SetLightmapTexture(iSampler,pLightmap,iTextureSubIndex);
            pPage->GetSurfaceSafe().m_spModelLightmaps[iTextureSubIndex] = pLightmap;
          }
        }
        // don't set the incoming atlas UV because the current one is correct for terrains whereas the incoming one does not match here
      }
  }

  VEditableTerrain *m_pTerrain;
};


#define TERRAIN_LM_MODE_SINGLE_PAGE     1
#define TERRAIN_LM_MODE_PERSECTOR_PAGE  2

void VEditableTerrain::GatherLightmapInfo(VLightmapSceneInfo &info)
{
  bool bIsCaster = m_bCastStaticShadows;  
  bool bIsReceiver = m_iLightmapPageSize>0;
  bIsCaster &= info.IsUsedCasterID(GetUniqueID());
  bIsReceiver &= info.IsUsedReceiverID(GetUniqueID());

  int iPageSize = 1024;
  int iSectorX1 = 0;
  int iSectorY1 = 0;
  int iSectorX2 = m_Config.m_iSectorCount[0]-1;
  int iSectorY2 = m_Config.m_iSectorCount[1]-1;
  
  // in this case we can use a subrange of occluder sectors
  if (!bIsReceiver && info.m_CombinedLightReceiverBox.isValid())
  {
    VLargePosition p1(m_Config,info.m_CombinedLightReceiverBox.m_vMin);
    VLargePosition p2(m_Config,info.m_CombinedLightReceiverBox.m_vMax);
    iSectorX1 = p1.m_iSectorX;
    iSectorY1 = p1.m_iSectorY;
    iSectorX2 = p2.m_iSectorX;
    iSectorY2 = p2.m_iSectorY;
    m_Config.ClampSectorRectangle(iSectorX1,iSectorY1,iSectorX2,iSectorY2);
    if (iSectorX1>iSectorX2 || iSectorY1>iSectorY2)
      bIsCaster = false; // this will cause it to exit this function
  }

  // relevant for lighting at all?
  if (!bIsCaster && !bIsReceiver)
    return;

  VLightmapPrimitive *pPrim = new VTerrainLightmapPrimitive(this);
  info.Primitives().Add(pPrim);

  pPrim->m_eOwnerType = VLightmapPrimitive::OWNERTYPE_STATICMESH;
  pPrim->m_eType = VLightmapPrimitive::INDEXED_MESH;
  pPrim->m_iUniqueID = GetUniqueID();
  pPrim->m_iFlags = 0;
  if (bIsCaster)
    pPrim->m_iFlags |= PRIMITIVEFLAG_CASTSHADOWS;
  if (bIsReceiver)
    pPrim->m_iFlags |= PRIMITIVEFLAG_RECEIVESHADOWS;

  int iStep = 1;
  int iSamplesX = (iSectorX2-iSectorX1+1) * m_Config.m_iHeightSamplesPerSector[0];
  int iSamplesY = (iSectorY2-iSectorY1+1) * m_Config.m_iHeightSamplesPerSector[1];
  int iReducedSamplesX = iSamplesX;
  int iReducedSamplesY = iSamplesY;
//  int iSamplesX1 = iSectorX1 * m_Config.m_iHeightSamplesPerSector[0];
//  int iSamplesY1 = iSectorY1 * m_Config.m_iHeightSamplesPerSector[1];
//  int iSamplesX2 = (iSectorX2+1) * m_Config.m_iHeightSamplesPerSector[0];
//  int iSamplesY2 = (iSectorY2+1) * m_Config.m_iHeightSamplesPerSector[1];

  int iSectorCountX = iSectorX2-iSectorX1+1;
  int iSectorCountY = iSectorY2-iSectorY1+1;

  int iRelevantSectorCount = 0;
  for (int sy=iSectorY1;sy<=iSectorY2;sy++)
    for (int sx=iSectorX1;sx<=iSectorX2;sx++)
      if (GetSector(sx,sy)->GetVisibleBitmask()!=0)
        iRelevantSectorCount++;

  // limit the number of triangles passed to vLux:
  const int iMaxLMSamples = hkvMath::Min(1024,iPageSize);

  while (iReducedSamplesX>iMaxLMSamples || iReducedSamplesY>iMaxLMSamples)
  {
    iStep *= 2;
    iReducedSamplesX/=2;
    iReducedSamplesY/=2;
  }
  
  bool bPerSectorLightmaps = m_bPerSectorLightmaps;
  bool bSplitSectors = !bIsReceiver || bPerSectorLightmaps;
  int iMatCount = 1;
  if (bSplitSectors)
  {
    // count the actual number of visible sectors:
    iMatCount = iRelevantSectorCount;
  }
  pPrim->AllocateMaterials(iMatCount);
  pPrim->m_iUserData = bPerSectorLightmaps ? TERRAIN_LM_MODE_PERSECTOR_PAGE : TERRAIN_LM_MODE_SINGLE_PAGE;
    
  VEditableTerrainSector *pReferenceSector = this->GetSector(0,0); // for all properties that are common for all sectors

  pReferenceSector->EnsureLoaded();

  VTerrainSectorMeshPageInfo* pReferencePage = &pReferenceSector->m_pMeshPage[0];

  for (int i=0;i<iMatCount;i++)
  {
    VLightmapPrimitive::MeshMaterial_t &mat = pPrim->GetMaterial(i);
   
    pReferencePage->GetSurfaceSafe().GetLightmapInfo(mat); // some (yet unnecessary) base properties
    if (bIsReceiver)
    {
      iPageSize = m_iLightmapPageSize;
      pPrim->m_iFlags |= PRIMITIVEFLAG_RECEIVESHADOWS|PRIMITIVEFLAG_FULLPAGE;
      mat.m_iLightInfluenceBitmask = pReferenceSector->GetLightInfluenceBitmask();
      mat.m_iWantedLightmapSize[0] = mat.m_iWantedLightmapSize[1] = m_iLightmapPageSize; // page size
    }
    mat.m_sBaseTexture.Reset(); // no well-defined texture here...
    mat.m_iOverrideFlags = pPrim->m_iFlags;
  }

  // create vertices

  // when per-sector lightmaps are used, then add a border vertex row around the sector. This is to duplicate lightmap texels at the edges so we can
  // simulate flawless bilinear filtering across pages
  int iBorder = (bPerSectorLightmaps && bIsReceiver) ? 1 : 0;
  int iBorderStep = iBorder*iStep;
  int iVerticesPerSector = ((m_Config.m_iHeightSamplesPerSector[0]+iBorderStep*2)/iStep+1)*((m_Config.m_iHeightSamplesPerSector[1]+iBorderStep*2)/iStep+1);
  pPrim->AllocateVertices(iRelevantSectorCount*iVerticesPerSector);
  if (bIsReceiver)
    pPrim->AllocateUV();
  hkvVec2 *pUV = pPrim->GetLightmapUV();
  VLightmapPrimitive::MeshVertex_t *pVert = pPrim->GetVertices();
  hkvVec3 vNrml;
  int iFilled = 0;

  hkvVec3 vTranslate = m_Config.GetSectorOrigin(iSectorX1,iSectorY1);
  pPrim->m_vPrimCenter = vTranslate;

  int iFullCountX = m_Config.m_iHeightSamplesPerSector[0]+iBorderStep*2;
  int iFullCountY = m_Config.m_iHeightSamplesPerSector[1]+iBorderStep*2;
  // we need to re-map the UV to [0..1] range, otherwise vLux complains. For the resulting UVScaleOfs it does not make a difference as we use a hardcoded value anyway
  const float fUCorrectionScale = (float)m_Config.m_iHeightSamplesPerSector[0] / (float)(iFullCountX);
  const float fVCorrectionScale = (float)m_Config.m_iHeightSamplesPerSector[1] / (float)(iFullCountY);
  const float fUCorrectionOfs = (float)iBorderStep / (float)m_Config.m_iHeightSamplesPerSector[0];
  const float fVCorrectionOfs = (float)iBorderStep / (float)m_Config.m_iHeightSamplesPerSector[1];

  for (int sy=iSectorY1;sy<=iSectorY2;sy++)
    for (int sx=iSectorX1;sx<=iSectorX2;sx++) if (GetSector(sx,sy)->GetVisibleBitmask()!=0)
    {
      for (int y=-iBorderStep;y<=m_Config.m_iHeightSamplesPerSector[1]+iBorderStep;y+=iStep)
        for (int x=-iBorderStep;x<=m_Config.m_iHeightSamplesPerSector[0]+iBorderStep;x+=iStep,pVert++,pUV++,iFilled++)
        {
          int xx = sx*m_Config.m_iHeightSamplesPerSector[0] + x;
          int yy = sy*m_Config.m_iHeightSamplesPerSector[1] + y;
          if (bIsReceiver)
          {
            if (bPerSectorLightmaps)
              pUV->set((float)x/(float)m_Config.m_iHeightSamplesPerSector[0],(float)y/(float)m_Config.m_iHeightSamplesPerSector[1]);
            else
              pUV->set((float)xx/(float)m_Config.m_iOverallHeightSampleCount[0],(float)yy/(float)m_Config.m_iOverallHeightSampleCount[1]);
            // re-normalize:
            pUV->x = (pUV->x + fUCorrectionOfs) * fUCorrectionScale;
            pUV->y = (pUV->y + fVCorrectionOfs) * fVCorrectionScale;
          }

          float z00 = m_SectorManager.GetHeightAtSamplePosClamped(xx,yy);
          float z10 = m_SectorManager.GetHeightAtSamplePosClamped(xx+1,yy);
          float z01 = m_SectorManager.GetHeightAtSamplePosClamped(xx,yy+1);

          pVert->vPos.x = m_Config.GetHeightmapSamplePosX((float)xx) - vTranslate.x;
          pVert->vPos.y = m_Config.GetHeightmapSamplePosY((float)yy) - vTranslate.y;
          pVert->vPos.z = z00 + m_Config.m_vTerrainPos.z - vTranslate.z;

          pVert->vTangent.x = m_Config.m_vSampleSpacing.x;
          pVert->vTangent.y = 0.f;
          pVert->vTangent.z = z10-z00;
          pVert->vTangent.normalizeIfNotZero();
          pVert->vBiTangent.x = 0.f;
          pVert->vBiTangent.y = m_Config.m_vSampleSpacing.y;
          pVert->vBiTangent.z = z01-z00;
          pVert->vBiTangent.normalizeIfNotZero();

          // calculate a nice&smooth normal instead
          m_SectorManager.GetNormalAtSamplePos(xx,yy,vNrml,1.f);
          pVert->vNormal = vNrml;
          VASSERT(pVert->vNormal.z>0.f);

          pVert->vBaseUV.set((float)xx/(float)m_Config.m_iOverallHeightSampleCount[0],(float)y/(float)m_Config.m_iOverallHeightSampleCount[1]);
        }
    }

  VASSERT(iFilled==pPrim->GetVertexCount());

  // create indices
//  if (bSplitSectors)
  {
    int iMatIndex = 0;
    VASSERT((iReducedSamplesX%iSectorCountX)==0);
    VASSERT((iReducedSamplesY%iSectorCountY)==0);
    iReducedSamplesX /= iSectorCountX;
    iReducedSamplesY /= iSectorCountY;
    int iRowStride = iReducedSamplesX+1+iBorder*2;
    const int iIndicesPerSector = (iReducedSamplesX+iBorder*2)*(iReducedSamplesY+iBorder*2)*6;
    int iIndicesPerMaterial = bSplitSectors ? iIndicesPerSector : (iIndicesPerSector*iRelevantSectorCount);

    const bool bIndex16 = pPrim->GetVertexCount()<0x10000;
    unsigned int *pInd32 = NULL;
    unsigned short *pInd16 = NULL;

    int iSectorCounter = 0;
    for (int sy=0;sy<iSectorCountY;sy++)
      for (int sx=0;sx<iSectorCountX;sx++)
      {
        if (GetSector(sx+iSectorX1,sy+iSectorY1)->GetVisibleBitmask()==0)
          continue;
        if (bSplitSectors || iMatIndex==0)
        {
          VLightmapPrimitive::MeshMaterial_t &mat = pPrim->GetMaterial(iMatIndex++);
          if (bIndex16)
            mat.AllocateIndices(iIndicesPerMaterial);
          else
            mat.AllocateIndices32(iIndicesPerMaterial);
          pInd32 = mat.GetIndexList32();
          pInd16 = mat.GetIndexList();
          if (bSplitSectors)
            mat.m_iUserData = (sy<<16) + sx; // encode sector index
          else 
            mat.m_iUserData = 0xffffffff;
        }

        for (int y=0;y<iReducedSamplesY+iBorder*2;y++)
          for (int x=0;x<iReducedSamplesX+iBorder*2;x++,pInd32+=6,pInd16+=6)
          {
            int iFirst = y*iRowStride+x + iSectorCounter * iVerticesPerSector;
            VASSERT(iFirst+iRowStride+1 < pPrim->GetVertexCount());
            if (bIndex16)
            {
              pInd16[0] = iFirst+iRowStride+1;
              pInd16[1] = iFirst+iRowStride;
              pInd16[2] = iFirst;
              pInd16[3] = iFirst+1;
              pInd16[4] = iFirst+iRowStride+1;
              pInd16[5] = iFirst;
            } else
            {
              pInd32[0] = iFirst+iRowStride+1;
              pInd32[1] = iFirst+iRowStride;
              pInd32[2] = iFirst;
              pInd32[3] = iFirst+1;
              pInd32[4] = iFirst+iRowStride+1;
              pInd32[5] = iFirst;
            }
          }   
        iSectorCounter++;
      }

    VASSERT(iSectorCounter==iRelevantSectorCount);
  }


  bool bAnyShadowCastingDeco = false;
  
  for (int i=0;i<VTerrainDecorationModelManager::GlobalManager().GetResourceCount();i++)
  {
    IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)VTerrainDecorationModelManager::GlobalManager().GetResourceByIndex(i);
    if (pModel==NULL)
      continue;
    pModel->OnEditorEvent(IVTerrainDecorationModel::GatherLightmapBegin,this,&info,NULL);
    if (pModel->m_Properties.m_eModelFlags&IVTerrainDecorationModel::CastLightmapShadows)
      bAnyShadowCastingDeco = true;
  }

  if (bAnyShadowCastingDeco)
  {
    DynArray_cl<VTerrainDecorationInstance *> tempList(NULL,0);

    FOREACH_SECTOR
      BOOL bOldLoaded = pSector->IsLoaded();
      pSector->EnsureLoaded();
      VTerrainDecorationModelManager &models(VTerrainDecorationModelManager::GlobalManager());

      // for each model...
      for (int iModel=0;iModel<models.GetResourceCount();iModel++)
      {
        IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)models.GetResourceByIndex(iModel);
        if (pModel==NULL || (pModel->m_Properties.m_eModelFlags & IVTerrainDecorationModel::CastLightmapShadows)==0)
          continue;
        pModel->EnsureLoaded();
        if (!pModel->IsValid())
          continue;

        // ...count the number of instances and add them to a temp list
        int iInstCount = 0;
        VTerrainDecorationInstance *pInst =  pSector->m_Decoration.m_pInstances;
        for (int i=0;i<pSector->m_Decoration.m_iCount;i++,pInst++)
          if (pInst->m_spModel == pModel)
            tempList[iInstCount++] = pInst;

        if (iInstCount>0)
          pModel->GatherLightmapInfo(info,tempList.GetDataPtr(),iInstCount);
      }

      if (!bOldLoaded)
        pSector->EnsureUnloaded();
    }
  }

  for (int i=0;i<VTerrainDecorationModelManager::GlobalManager().GetResourceCount();i++)
  {
    IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)VTerrainDecorationModelManager::GlobalManager().GetResourceByIndex(i);
    if (pModel==NULL)
      continue;
    pModel->OnEditorEvent(IVTerrainDecorationModel::GatherLightmapFinished,this,&info,NULL);
  }

}

void VEditableTerrain::GetLightmapInfo(VLightmapSceneInfo &info)
{
  if (info.m_pCurrentZone!=this->GetParentZone()) // ignore other zones
    return;

  VLightmapPrimitive *pPrim = info.GetPrimitiveByID(VLightmapPrimitive::OWNERTYPE_STATICMESH, GetUniqueID());
  if (!pPrim)
  {
    V_SAFE_DELETE_ARRAY(m_spSceneLightmaps);
    SetFullbrightLightmaps(); // in this case lightmaps are wanted but not available
    return;
  }

  V_SAFE_DELETE_ARRAY(m_spSceneLightmaps);
  if (m_Config.m_bSceneSpecificLightmaps)
    m_spSceneLightmaps = new VTextureObjectPtr[m_Config.m_iSectorCount[0]*m_Config.m_iSectorCount[1]*4];

  VLightmapPrimitive::MeshMaterial_t *pMat = NULL;

  // set samplers on all sectors
  char szSampler[128];
  int iMatIndex = 0;
  bool bSinglePageLM = true;
  float fInvSectorCountX = 1.f/(float)m_Config.m_iSectorCount[0];
  float fInvSectorCountY = 1.f/(float)m_Config.m_iSectorCount[1];

  FOREACH_SECTOR
    pMat = NULL;
    for (iMatIndex=0;iMatIndex<pPrim->GetMaterialCount();iMatIndex++)
    {
      unsigned int iUserData = pPrim->GetMaterial(iMatIndex).m_iUserData;
      bSinglePageLM = iUserData==0xffffffff;
      if ((iUserData != ((sy<<16) + sx)) && !bSinglePageLM)
        continue;
      // found the material
      pMat = &pPrim->GetMaterial(iMatIndex);
      break;
    }
    if (!pMat)
      continue;

    pSector->EnsureLoaded();
    pSector->ApplyLightmapInfo(info, pPrim, iMatIndex); // call VisStaticGeometryInstance_cl::ApplyLightmapInfo for lightmasks etc.
    for (int i=0;i<info.GetSubPageCount();i++)
    {
      if (i==0)
        sprintf(szSampler,"Lightmap");
      else
        sprintf(szSampler,"Lightmap%i",i);

      VTextureObject *pTex = info.GetLightmapTexture(pMat->m_iPageIndex,i);
      if (!pTex)
        continue;

      // assign to surface and texture
      if (m_spSceneLightmaps!=NULL)
        m_spSceneLightmaps[(pSector->m_iIndexY*m_Config.m_iSectorCount[0]+pSector->m_iIndexX)*4 + i] = pTex;

      VTerrainSectorMeshPageInfo *pPage = pSector->m_pMeshPage;
      for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      {
        for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
        {
          pPage->GetSurfaceSafe().m_spModelLightmaps[i] = pTex;
        }
      }

      hkvVec4 scaleOfs = pSector->GetLightmapScaleOffset();
      if (bSinglePageLM)
      {
        scaleOfs.set(fInvSectorCountX,fInvSectorCountY,(float)sx*fInvSectorCountX,(float)sy*fInvSectorCountY);
      }
      else
      {
        // recent versions do export per-sector pages with a border around it. Adjust the mapping:
        if (pPrim->m_iUserData==TERRAIN_LM_MODE_PERSECTOR_PAGE) // old versions did not store a value here
        {
          // note this border is independent of the texture size. Currently its width is one vertex row/column rather than a lightmap texture texel
          float fBorderX = 1.f/(float)m_Config.m_iHeightSamplesPerSector[0];
          float fBorderY = 1.f/(float)m_Config.m_iHeightSamplesPerSector[1];
          scaleOfs.set(1.f-fBorderX*2.f,1.f-fBorderY*2.f,fBorderX,fBorderY);
        }
        else
          scaleOfs.set(1.f,1.f,0.f,0.f); // old style
      }

      // incorporate scale/ofs as provided by Beast
      scaleOfs.x *= pMat->m_vUVScaling.x;
      scaleOfs.y *= pMat->m_vUVScaling.y;
      scaleOfs.z *= pMat->m_vUVScaling.x;
      scaleOfs.w *= pMat->m_vUVScaling.y;
      scaleOfs.z += pMat->m_vUVOffset.x;
      scaleOfs.w += pMat->m_vUVOffset.y;

      pSector->SetLightmapScaleOffset(scaleOfs);

      pPage = pSector->m_pMeshPage;
      for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      {
        for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
        {
          int iSampler = pPage->GetSamplerIndex(szSampler);
          if (iSampler>=0)
          {
            pPage->SetLightmapTexture(iSampler,pTex,i);
            pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
          }
        }
      }
    }
  }
}


void VEditableTerrain::SetFullbrightLightmaps()
{
  char szSampler[128];
  VTextureObject *pTex = Vision::TextureManager.GetPlainWhiteTexture();

  FOREACH_SECTOR
    pSector->EnsureLoaded();
    pSector->ResetLightMasks();
    for (int i=0;i<4;i++)
    {
      if (i==0)
        sprintf(szSampler,"Lightmap");
      else
        sprintf(szSampler,"Lightmap%i",i);

      // assign to surface and texture
      VTerrainSectorMeshPageInfo *pPage = pSector->m_pMeshPage;
      for (int iPageY=0;iPageY<m_Config.m_iSectorMeshesPerSector[1];iPageY++)
      {
        for (int iPageX=0;iPageX<m_Config.m_iSectorMeshesPerSector[0];iPageX++,pPage++)
        {
          pPage->GetSurfaceSafe().m_spModelLightmaps[i] = pTex;

          int iSampler = pPage->GetSamplerIndex(szSampler);
          if (iSampler>=0)
          {
            pPage->SetLightmapTexture(iSampler,pTex,i);
            pSector->SetEditorFlag(SECTOR_EDITORFLAG_SECTORFILEDIRTY);
          }
        }
      }
      pSector->SetLightmapScaleOffset(hkvVec4 (1.f,1.f,0.f,0.f));
    }
  }  
}



void VEditableTerrain::AddDirtyVegetationRange(const hkvAlignedBBox &bbox)
{
  VLargeBoundingBox largeBox(m_Config,bbox);
  int sx1,sy1,sx2,sy2;
  largeBox.GetSectorIndices_Clamped(m_Config,sx1,sy1,sx2,sy2);
  
  int iOldCount = m_DirtyVegetationSectors.Count();
  for (int y=sy1;y<=sy2;y++)
    for (int x=sx1;x<=sx2;x++)
      if (GetSector(x,y)->m_Decoration.GetDecorationObjectCount()>0)
        m_DirtyVegetationSectors.AddUnique(GetSector(x,y));

  if (iOldCount != m_DirtyVegetationSectors.Count())
    TriggerDirtyRangeChanged();
}


void VEditableTerrain::AddDirtyVegetationRangeAllTerrains(const hkvAlignedBBox &bbox)
{
  for (int i=0;i<VTerrainManager::GlobalManager().GetResourceCount();i++)
    ((VEditableTerrain*)VTerrainManager::GlobalManager().GetResourceByIndex(i))->AddDirtyVegetationRange(bbox);
}


VisStaticGeometryInstanceCollection_cl& VEditableTerrain::DetermineRelevantDecorationReceiverMeshes(VTerrainSector *pSector)
{
  m_TempRelevantMeshes.Clear();
  const VObjectComponentCollection &components(VEditableTerrain::m_MeshPaintingComponents);
  const int iCount = components.Count();
  if (iCount==0)
    return m_TempRelevantMeshes;

  // tag geometry that has the component attached (ideally only once per frame)
  if (m_iFrameRelevantMeshesDetected != VisStaticGeometryInstance_cl::GetCurrentGlobalTag())
  {
    VisStaticGeometryInstance_cl::ResetTags();
    m_iFrameRelevantMeshesDetected = VisStaticGeometryInstance_cl::GetCurrentGlobalTag();
    for (int i=0;i<iCount;i++)
    {
      VisStaticMeshInstance_cl *pInst = (VisStaticMeshInstance_cl *)components.GetAt(i)->GetOwner();
      VASSERT(pInst);
      // tag the geometry
      for (int j=0;j<pInst->GetSubmeshInstanceCount();j++)
        pInst->GetSubmeshInstance(j)->Tag();

    }
  }

  // now retrieve the tagged geometry that is assigned to the visibility zone of the sector:
  VASSERT(pSector->m_spSectorZone!=NULL && pSector->m_spSectorZone->GetStaticGeometryInstances()!=NULL);
  const VisStaticGeometryInstanceCollection_cl *pAllGeom = pSector->m_spSectorZone->GetStaticGeometryInstances();
  pAllGeom->GetTaggedEntries(m_TempRelevantMeshes);
  return m_TempRelevantMeshes;
}

void VEditableTerrain::TriggerDirtyRangeChanged()
{
  VTerrainDataObject data(&OnDecorationDirtyRangeChanged,this,NULL);
  data.Trigger();
}

void VEditableTerrain::UpdateDefaultEffectSettingsInConfig()
{
  // check if three way mapping and/or normal mapping is wanted
  m_Config.m_bUseThreeWayMapping = false;
  m_Config.m_bUseNormalMapping = false;

  for (int i = 0; 
    i < m_WeightmapChannels.Count() && 
    (!m_Config.m_bUseThreeWayMapping || !m_Config.m_bUseNormalMapping); 
    i++)
  {
    VTextureWeightmapChannelResource *pRes = m_WeightmapChannels.GetAt(i);
    if (pRes->IsHidden())
      continue;

    if (pRes->m_Properties.m_bUseThreeWayMapping)
    {
      m_Config.m_bUseThreeWayMapping = true;
    }
    if (pRes->m_Properties.m_spNormalmapTexture != NULL)
    {
      m_Config.m_bUseNormalMapping = true;
    }
  }

  // check if light mapping is needed
  m_Config.m_bUseLightMapping = m_iLightmapPageSize > 0;
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
