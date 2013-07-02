/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationGroup.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenerator.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapReceiverRenderer.hpp>
#endif

#include <Vision/Runtime/Base/Graphics/VLightmapHelper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


DynArray_cl<VTerrainDecorationInstance *> VTerrainDecorationGroup::g_VisibleInstances(0,NULL); // only needed while rendering
hkvVec3 VTerrainDecorationGroup::g_vGlobalRenderOffset; // for repositioning


VTerrainDecorationGroup::VTerrainDecorationGroup(bool bRegister)
{
  RemoveObjectFlag(VObjectFlag_AutoDispose); // this group should behave like a game object
  m_bIsRegistered = bRegister;
  m_iVisStateCount = 0;
  if (m_bIsRegistered)
    VTerrainDecorationGroupManager::g_GlobalManager.AddGroup(this);

  int iFlags = VISTESTFLAGS_PERFORM_ALL_TESTS & (~VISTESTFLAGS_HARDWAREOCCLUSIONQUERY);
  SetVisTestFlags(iFlags | VISTESTFLAGS_ACTIVE);
  m_AmbientColor = V_RGBA_BLACK;
  m_bUseLightgrid = true;
  m_fMax3DDistance = m_fUnscaledMaxDist = -1;
  m_pDecoOfTerrain = NULL;
  m_bUseCollisions = m_bCastLightmapShadows = true;
}


VTerrainDecorationGroup::~VTerrainDecorationGroup()
{
  // notify physics
  IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
  if (pPM)
    pPM->OnDecorationRemoved(m_Instances);

  FreeAllVisStates();
}

void VTerrainDecorationGroup::DisposeObject()
{
  // the order inside this function is critical because there are two references held
  if (IsObjectFlagSet(VObjectFlag_Disposed))
    return;

  // notify physics
  IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
  if (pPM)
    pPM->OnDecorationRemoved(m_Instances);

  VisVisibilityObjectAABox_cl::DisposeObject();
  FreeAllVisStates();
  m_Instances.Free();

  bool bOldRegistered = m_bIsRegistered;
  m_bIsRegistered = false;
  if (bOldRegistered)
    VTerrainDecorationGroupManager::g_GlobalManager.RemoveGroup(this);
}




void VTerrainDecorationGroup::SetFarClipDistance(float fFar)
{
  VVisibilityData::SetFarClipDistance(fFar);
  if (fFar<=0.f)
    SetClipMode(VIS_LOD_TEST_NONE);
  else
    SetClipMode(VIS_LOD_TEST_BOUNDINGBOX|VIS_LOD_TEST_APPLYLODSCALING);
  CalcMax3DDistance();
}


VTerrainDecorationGroup::VCollectorVisibleState* VTerrainDecorationGroup::FindVisStateForCollector(
  IVisVisibilityCollector_cl* pColl)
{
  VCollectorVisibleState** ppVisStates = static_cast<VCollectorVisibleState**>(m_VisStates.GetBuffer());
  for (int i=0;i<m_iVisStateCount;i++)
  {
    if (ppVisStates[i]->m_pCollector==pColl)
      return ppVisStates[i];
  }
  return NULL;
}

VTerrainDecorationGroup::VCollectorVisibleState* VTerrainDecorationGroup::CreateVisStateForCollector(
  IVisVisibilityCollector_cl* pColl)
{
  VCollectorVisibleState *pNewState = FindVisStateForCollector(pColl);
  if (pNewState != NULL)
    return pNewState;

  m_VisibilityMutex.Lock();

  int iIntCount = (m_Instances.m_iCount+31)/32;
  int iSize = offsetof(VCollectorVisibleState,m_iFirstMask) + iIntCount*sizeof(unsigned int); // structure + bitmask

  pNewState = (VCollectorVisibleState *)vMemAlloc(iSize);
  pNewState->m_pCollector = pColl;
  pNewState->m_bVerticalBBVisible = false;
  pNewState->m_iLastFrameChecked = static_cast<unsigned char>(VisRenderContext_cl::GetGlobalTickCount() - 1);
  pNewState->m_iFirstRelevantBit = m_Instances.GetDecorationObjectCount();
  pNewState->m_iLastRelevantBit = -1;
  memset(&pNewState->m_iFirstMask, 0, iIntCount*sizeof(unsigned int));

  // Make sure array is large enough and preserve content
  m_VisStates.EnsureCapacity((m_iVisStateCount + 1) * sizeof(VCollectorVisibleState*), true);
  VCollectorVisibleState** ppVisStates = static_cast<VCollectorVisibleState**>(m_VisStates.GetBuffer());
  ppVisStates[m_iVisStateCount++] = pNewState;

  m_VisibilityMutex.Unlock();
  return pNewState;
}


BOOL VTerrainDecorationGroup::OnTestVisible(IVisVisibilityCollector_cl *pCollector, const VisFrustum_cl *pFrustum)
{
  // VISION_PROFILE_FUNCTION(VSpeedTreeManager::PROFILING_VISIBILITY);
  if (m_Instances.m_iCount==0)
    return TRUE;

  // the bounding box has been determined visible already so perform some per-instance tests
  VCollectorVisibleState* pState = CreateVisStateForCollector(pCollector);
  if (!pState)
    return TRUE;

  // Clear state structure only if this is the first visibility check this frame with
  // this visibility collector.
  // If portals are used there may be several checks with the same collector after the test with the actual camera view frustum.
  const unsigned char iCurrentTickCount = static_cast<unsigned char>(VisRenderContext_cl::GetGlobalTickCount());
  if (pState->m_iLastFrameChecked == iCurrentTickCount)
  {
    // Already updated this frame.
    return TRUE;
  }

  unsigned int *pMask = &pState->m_iFirstMask;
  int iIntCount = (m_Instances.m_iCount + 31) / 32;

  // clear values
  memset(pMask, 0, iIntCount * sizeof(unsigned int));
  pState->m_iLastFrameChecked = iCurrentTickCount;

  pState->m_iFirstRelevantBit = m_Instances.m_iCount;
  pState->m_iLastRelevantBit = -1;
  pState->m_bVerticalBBVisible = true;

  // early out also when group is too far away:
  VASSERT(pCollector->GetLODReferenceRenderContext()!=NULL);
  VisRenderContext_cl *pLODContext = pCollector->GetLODReferenceRenderContext();
  const hkvVec3 vLODPos = pLODContext->GetCamera()->GetPosition();

  // early out also when group is too far away:
  const float fDistanceScale =  pState->m_pCollector->GetLODReferenceRenderContext()->GetLODDistanceScaling();
  const float fGroupDist = this->GetWorldSpaceBoundingBox().getDistanceTo(vLODPos);
  if (fGroupDist*fDistanceScale > m_fMax3DDistance)
    return TRUE;

  const hkvVec3 vTranslate = GetPosition() - m_vPositionBackup;
  VTerrainDecorationInstance *pInst = m_Instances.m_pInstances;
  
  for (int i = 0; i < m_Instances.m_iCount; i++, pInst++)
  {
    // farclip distance check
    const float fDistance = hkvMath::sqrt (pInst->GetRenderDistanceSqr(vLODPos, vTranslate));

  #ifdef DECORATION_SCALE_INDEPENDENT_LOD
    if (pInst->m_spModel == NULL || fDistance*fDistanceScale > m_fUnscaledMaxDist)
      continue;
  #else
    if (pInst->m_spModel == NULL || fDistance*fDistanceScale > m_fUnscaledMaxDist*pInst->GetScaling())
      continue;
  #endif
  
    // frustum check
    hkvAlignedBBox absBBox(hkvNoInitialization);
    pInst->GetRenderBoundingBox(absBBox,vTranslate);
    //Vision::Game.DrawBoundingBox(absBBox);

    if (pFrustum && !pFrustum->Overlaps(absBBox))
      continue;

    // object is visible:
    pMask[i / 32] |= (1 << (i & 31));
    pState->m_iFirstRelevantBit = hkvMath::Min(pState->m_iFirstRelevantBit, i);
    pState->m_iLastRelevantBit = i;
  }

  return TRUE;
}


void VTerrainDecorationGroup::RenderVisibleInstancesToShadowMap(IVisVisibilityCollector_cl *pCollector, 
  VShadowMapGenerator *pShadowMapGenerator, float *pfLightFrustumDistances)
{
//  VISION_PROFILE_FUNCTION(VSpeedTreeManager::PROFILING_RENDERGROUPINSTANCES);
  VCollectorVisibleState *pState = FindVisStateForCollector(pCollector);
  if (pState == NULL || !pState->IsAnyInstanceVisible())
    return;

  // build batch
  const hkvVec3 vTranslate = GetPosition() - m_vPositionBackup;
  g_VisibleInstances.EnsureSize(pState->m_iLastRelevantBit - pState->m_iFirstRelevantBit + 1);
  int iInstCount = 0;
  const unsigned int* pMask = &pState->m_iFirstMask;

  // loop through all relevant integers
  const int iFirstRelevantInt = pState->m_iFirstRelevantBit / 32;
  const int iLastRelevantInt = pState->m_iLastRelevantBit / 32;

  for (int i = iFirstRelevantInt; i <= iLastRelevantInt; i++)
  {
    const unsigned int iMask32 = pMask[i];

    // No bits set for this integer? -> Move to the next integer.
    if (iMask32 == 0) 
      continue;

    // loop through all bits of the integer value
    const int iFirstBit = hkvMath::Max(i*32, pState->m_iFirstRelevantBit);
    const int iLastBit = hkvMath::Min(i*32 + 31, pState->m_iLastRelevantBit);

    for (int b = iFirstBit; b <= iLastBit; b++)
    {
      if ((iMask32 & (1 << (b & 31))) == 0)
        continue;

      hkvAlignedBBox instanceBox(hkvNoInitialization);
      m_Instances.m_pInstances[b].GetRenderBoundingBox(instanceBox, vTranslate);
      if (!Vision::RenderLoopHelper.CompareLightFrustumDistances(instanceBox, 
        pShadowMapGenerator->GetMainFrustum(), pfLightFrustumDistances))
      {
        continue;
      }
      
      g_VisibleInstances.GetDataPtr()[iInstCount++] = &m_Instances.m_pInstances[b];
    }
  }

  if (iInstCount == 0)
    return;

  m_Instances.m_pInstances[0].m_spModel->RenderBatch(NULL, g_VisibleInstances.GetDataPtr(), iInstCount);
}


void VTerrainDecorationGroup::RenderVisibleInstances(IVisVisibilityCollector_cl *pCollector)
{
//  VISION_PROFILE_FUNCTION(VSpeedTreeManager::PROFILING_RENDERGROUPINSTANCES);
  VCollectorVisibleState *pState = FindVisStateForCollector(pCollector);

  if (pState==NULL || !pState->IsAnyInstanceVisible())
    return;

  // build batch
  g_VisibleInstances.EnsureSize(pState->m_iLastRelevantBit - pState->m_iFirstRelevantBit + 1);
  int iInstCount = 0;
  const unsigned int* pMask = &pState->m_iFirstMask;

  // loop through all relevant integers
  const int iFirstRelevantInt = pState->m_iFirstRelevantBit / 32;
  const int iLastRelevantInt = pState->m_iLastRelevantBit / 32;

  for (int i = iFirstRelevantInt; i <= iLastRelevantInt; i++)
  {
    const unsigned int iMask32 = pMask[i];

    // No bits set for this integer? -> Move to the next integer.
    if (iMask32 == 0) 
      continue;

    // loop through all bits of the integer value
    const int iFirstBit = hkvMath::Max(i*32, pState->m_iFirstRelevantBit);
    const int iLastBit = hkvMath::Min(i*32 + 31, pState->m_iLastRelevantBit);

    for (int b = iFirstBit; b <= iLastBit; b++)
    {
      if ((iMask32 & (1 << (b & 31))) != 0)
        g_VisibleInstances.GetDataPtr()[iInstCount++] = &m_Instances.m_pInstances[b];
    }
  }

  if (iInstCount == 0)
    return;

  // track light grid colors (global for the group):
  TrackLightgridColors();

  // render
//  VSpeedTreeManager::GlobalManager().UpdateGlobals(GetPosition() - m_vPositionBackup);
  m_Instances.m_pInstances[0].m_spModel->RenderBatch(NULL, g_VisibleInstances.GetDataPtr(), iInstCount);

#ifdef PROFILING
  if (Vision::Profiling.GetDebugRenderFlags()&DEBUGRENDERFLAG_OBJECT_VISBBOX)
    Vision::Game.DrawBoundingBox(this->GetWorldSpaceBoundingBox(), VColorRef(0, 90, 0, 255));
#endif

#if defined(_VR_DX11)
  VisRenderStates_cl::SetVSConstantBuffer(VTERRAIN_CB_DECOMODEL, NULL);
#endif

}



VTerrainDecorationInstanceCollection& VTerrainDecorationGroup::BeginUpdateInstances()
{
  // notify physics
  IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
  if (pPM)
    pPM->OnDecorationRemoved(m_Instances);

  FreeAllVisStates();
  return m_Instances;
}

void VTerrainDecorationGroup::EndUpdateInstances()
{
  VTerrainDecorationInstance *pInst = m_Instances.m_pInstances;
  hkvAlignedBBox bbox;
  IVTerrainDecorationModel *pModel = (m_Instances.m_iCount>0) ? pInst[0].m_spModel : NULL;
  m_spModel = pModel;

  if (pModel!=NULL && pModel->IsValid() && pModel->m_LocalBBox.isValid())
  {
    for (int i=0;i<m_Instances.m_iCount;i++,pInst++)
    {
      hkvAlignedBBox temp = pModel->m_LocalBBox;
      temp.transformFromOrigin (hkvMat4 (pInst->m_Orientation,pInst->m_vPosition));
      bbox.expandToInclude (temp);
    }
  }

  if (bbox.isValid())
  {
    SetVisibleBitmask(0xffffffff);
    SetActivate(true);
  }
  else
  {
    bbox.set(hkvVec3 (-1.f,-1.f,-1.f), hkvVec3 (1.f,1.f,1.f)); // dummy bbox
    SetVisibleBitmask(0);
    SetActivate(false);
  }
  SetWorldSpaceBoundingBox(bbox);
  m_bIsWorldSpaceBBox = false; // make it movable for re-positioning
  ReComputeVisibility();
  if (Vision::Editor.IsInEditor())
    BackupPosition();
  CalcMax3DDistance();

  // notify physics
  IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
  if ((pPM != NULL)&& m_bUseCollisions)
  {
    pPM->OnDecorationCreated(m_Instances);
  }
}


void VTerrainDecorationGroup::ClearInstances()
{
  VTerrainDecorationInstanceCollection &coll = BeginUpdateInstances();
  coll.Allocate(NULL,0);
  EndUpdateInstances();
}



void VTerrainDecorationGroup::GetDependencies(VResourceSnapshot &snapshot)
{
  VisVisibilityObjectAABox_cl::GetDependencies(snapshot);
  IVTerrainDecorationModel *pModel = GetModel();
  if (pModel)
    pModel->GetDependencies(snapshot);
}


V_IMPLEMENT_SERIAL( VTerrainDecorationGroup, VisObject3D_cl, 0, &g_VisionEngineModule );
void VTerrainDecorationGroup::Serialize( VArchive &ar )
{
  VisVisibilityObjectAABox_cl::Serialize(ar);
  char iLocalVersion = 11;
  char iDecoVersion = DECORATIONINSTANCE_CURRENT_VERSION;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT(iLocalVersion>=10 && iLocalVersion<=11);

    // source model
    IVTerrainDecorationModel *pModel = (IVTerrainDecorationModel *)ar.ReadProxyObject();
    m_spModel = pModel;
    ar >> m_fMax3DDistance;

    // instances
    int iCount;
    VTerrainDecorationInstanceCollection& list = m_Instances;

    if (iLocalVersion>=11)
      ar >> iDecoVersion;
    else iDecoVersion = DECORATIONINSTANCE_VERSION_20;

    ar >> iCount;
    list.Allocate(NULL, iCount);
    VTerrainDecorationInstance *pInst = list.m_pInstances;
    for (int i=0;i<iCount;i++,pInst++)
    {
      pInst->SerializeX(ar,iDecoVersion,false);
      pInst->m_spModel = m_spModel;
      pInst->ComputeSortingKey(0);
    }

    // When there are no instances, assume this group is just the billboard group of painted terrain vegetation. In that case we need a
    // back reference to the terrain:
    if (iCount==0 && VTerrainManager::GlobalManager().GetResourceCount()==1)
      m_pDecoOfTerrain = (VTerrain *)VTerrainManager::GlobalManager().GetResourceByIndex(0);

    // more properties:
    ar >> m_bUseLightgrid;
    ar >> m_bUseCollisions;
    ar >> m_bCastLightmapShadows;
    m_vLightgridSamplePosition.SerializeAsVec3(ar);
    ar >> m_AmbientColor;
    for (int i=0;i<6;i++)
      m_LightGridColors[i].SerializeAsVec3(ar);

    // notify physics
    IVisPhysicsModule_cl *pPM = Vision::GetApplication()->GetPhysicsModule();
    if ((pPM != NULL) && iCount>0 && m_bUseCollisions)
    {
      pPM->OnDecorationCreated(m_Instances);
    }
    BackupPosition();

    if (m_spModel!=NULL)
    {
      m_fUnscaledMaxDist = GetFarClipDistance();
      if (m_fUnscaledMaxDist<=0.f)
        m_fUnscaledMaxDist = m_spModel->GetNativeFarClipDistance();
      if (m_fUnscaledMaxDist<0.f)
        m_fUnscaledMaxDist = 999999999999.f;
    }
    if (m_fMax3DDistance<0.f) // not serialized? Then test now
      CalcMax3DDistance();
    m_bIsWorldSpaceBBox = false; // make it movable for re-positioning
  }
  else
  {
    ar << iLocalVersion;

    // source model:
    ar.WriteProxyObject(m_spModel);
    ar << m_fMax3DDistance;

    // serialize instances
    ar << iDecoVersion; // vers 11
    ar << m_Instances.m_iCount;
    for (int i=0;i<m_Instances.m_iCount;i++)
      m_Instances.m_pInstances[i].SerializeX(ar,iDecoVersion,false); // version 7 serializes without per-instance model pointer (Serialization time!)

    // light grid colors
    ar << m_bUseLightgrid;
    ar << m_bUseCollisions;
    ar << m_bCastLightmapShadows;
    m_vLightgridSamplePosition.SerializeAsVec3 (ar);
    ar << m_AmbientColor;
    for (int i=0;i<6;i++)
      m_LightGridColors[i].SerializeAsVec3 (ar);
  }
}



void VTerrainDecorationGroup::TrackLightgridColors()
{
  VisRenderContext_cl* pContext = Vision::Contexts.GetCurrentContext();
  if (!pContext->IsViewContext())
    return;

  // track light grid colors (global for the group). Billboards might need to use the terrain's colors
  if (m_pDecoOfTerrain)
    Vision::RenderLoopHelper.TrackLightGridInfo(m_pDecoOfTerrain->m_SectorManager.m_DecorationLightgridColors);
  else
  {
    hkvVec3* pFinalColors = m_LightGridColors;
    float fBuffer[6*3];
    IVTimeOfDay *pToD = Vision::Renderer.GetTimeOfDayHandler();

    // consider the time of day?
    if (pToD!=NULL && (pContext->GetUsageHint()&VIS_CONTEXTUSAGEFLAG_DEFERRED)==0) // don't do this for deferred rendering
    {
      pFinalColors = (hkvVec3* )&fBuffer[0];
      const hkvVec3*  pToDColors = pToD->GetLightGridColors();
      for (int i=0;i<6;i++)
        pFinalColors[i] = pToDColors[i] + m_LightGridColors[i];
    }
    Vision::RenderLoopHelper.TrackLightGridInfo(pFinalColors);
  }
}


void VTerrainDecorationGroup::UpdateLightgridColors()
{
  bool bUseLightgrid = m_bUseLightgrid;
  hkvVec3 vAmbient = m_AmbientColor.ToFloat();
  if (GetRelevantLightGrid()==NULL)
  {
    float fInt = bUseLightgrid ? 1.0f : 0.f;
    for (int i=0;i<6;i++)
    {
      m_LightGridColors[i].set(fInt,fInt,fInt);
      m_LightGridColors[i] += vAmbient;
    }
  }
  else
  {
    if (bUseLightgrid)
    {
      GetRelevantLightGrid()->GetColorsAtPositionI(m_vLightgridSamplePosition,m_LightGridColors);
      for (int i=0;i<6;i++)
        m_LightGridColors[i] += vAmbient;
    } else
    {
      for (int i=0;i<6;i++)
        m_LightGridColors[i] = vAmbient;
    }

  }
}


void VTerrainDecorationGroup::CalcMax3DDistance()
{
  m_fMax3DDistance = m_fUnscaledMaxDist = 0.f;
  if (m_spModel==NULL)
    return;
  float fNativeFar = GetFarClipDistance();
  if (fNativeFar<=0.f)
    fNativeFar = m_spModel->GetNativeFarClipDistance();
  if (fNativeFar<=0.f)
  {
    m_fMax3DDistance = m_fUnscaledMaxDist = 999999999999.f;
    return;
  }

  m_fUnscaledMaxDist = fNativeFar;
  const int iCount = m_Instances.GetDecorationObjectCount();
  for (int i=0;i<iCount;i++)
  {
    VTerrainDecorationInstance *pInst = m_Instances.GetDecorationObject(i);
#ifdef DECORATION_SCALE_INDEPENDENT_LOD
    const float fScale = 1.0f;
#else
    const float fScale = pInst->GetScaling();
#endif
    m_fMax3DDistance = hkvMath::Max(m_fMax3DDistance, fScale*m_fUnscaledMaxDist);
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VTerrainDecorationGroupManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VTerrainDecorationGroupManager VTerrainDecorationGroupManager::g_GlobalManager;


VTerrainDecorationGroupManager& VTerrainDecorationGroupManager::GlobalManager()
{
  return g_GlobalManager;
}


void VTerrainDecorationGroupManager::AddGroup(VTerrainDecorationGroup *pGroup)
{
  if (Count()==0)
    RegisterCallbacks();
  Add(pGroup);
}

void VTerrainDecorationGroupManager::RemoveGroup(VTerrainDecorationGroup *pGroup)
{
  Remove(pGroup);
  if (Count()==0)
    UnRegisterCallbacks();
}

void VTerrainDecorationGroupManager::RemoveAllGroups()
{
  if (Count()>0)
    UnRegisterCallbacks();

  for (int i=Count()-1;i>=0;i--)
    GetAt(i)->DisposeObject();
  Clear();
}

void VTerrainDecorationGroupManager::RegisterCallbacks()
{
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnRenderHook += this;
  VShadowMapGenerator::OnRenderShadowMap += this;
#if !defined( HK_ANARCHY )
  VShadowMapReceiverRenderer::OnDetermineShadowReceivers += this;
#endif
  IVisVisibilityCollector_cl::OnVisibilityCollectorDestroyed += this;
}

void VTerrainDecorationGroupManager::UnRegisterCallbacks()
{
  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::Callbacks.OnRenderHook -= this;
  VShadowMapGenerator::OnRenderShadowMap -= this;
#if !defined( HK_ANARCHY )
  VShadowMapReceiverRenderer::OnDetermineShadowReceivers -= this;
#endif
  IVisVisibilityCollector_cl::OnVisibilityCollectorDestroyed -= this;
}


void VTerrainDecorationGroupManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  bool bIsRenderHookCallback = pData->m_pSender==&Vision::Callbacks.OnRenderHook;
  bool bIsShadowMapCallback = pData->m_pSender==&VShadowMapGenerator::OnRenderShadowMap;

  if (bIsRenderHookCallback || bIsShadowMapCallback)
  {
    if ( bIsRenderHookCallback && ((VisRenderHookDataObject_cl *)pData)->m_iEntryConst!=VRH_PRE_OCCLUSION_TESTS)
      return;
    IVisVisibilityCollector_cl *pCollector = Vision::Contexts.GetCurrentContext()->GetVisibilityCollector();
    const int iCount = this->Count();
    if (pCollector==NULL || iCount==0)
      return;

    // build a list of visible groups
    m_iVisibleGroupCount = 0;
    m_VisibleGroups.EnsureSize(iCount);

    float fLightToViewFrustumDistances[256];
    VShadowRendererDataObject *pShadowData = (VShadowRendererDataObject *)pData;

    if (bIsShadowMapCallback)
    {
      // For shadow maps, additionally filter by light / view frustum distances
      Vision::RenderLoopHelper.ComputeLightFrustumDistances(pShadowData->m_pGenerator->GetCascadeLightPosition(pShadowData->m_iCascadeIndex), pShadowData->m_pGenerator->GetMainFrustum(), fLightToViewFrustumDistances);
      
      for (int i=0;i<iCount;i++)
      {
        VTerrainDecorationGroup *pTreeGroup = GetAt(i);
        if (pCollector->IsVisObjectVisible(pTreeGroup)) // tree groups are derived from visibility objects
        {
          if (!Vision::RenderLoopHelper.CompareLightFrustumDistances(pTreeGroup->GetWorldSpaceBoundingBox(), pShadowData->m_pGenerator->GetMainFrustum(), fLightToViewFrustumDistances))
            continue;
          m_VisibleGroups[m_iVisibleGroupCount++] = pTreeGroup;
        }
      }
    }
    else
    {
      for (int i=0;i<iCount;i++)
      {
        VTerrainDecorationGroup *pTreeGroup = GetAt(i);
        if (pCollector->IsVisObjectVisible(pTreeGroup)) // tree groups are derived from visibility objects
          m_VisibleGroups[m_iVisibleGroupCount++] = pTreeGroup;
      }
    }
    if (m_iVisibleGroupCount==0)
      return;

//    VISION_PROFILE_FUNCTION(PROFILING_RENDERINGOVERALL)

    {
      // VISION_PROFILE_FUNCTION(PROFILING_RENDERINSTANCES)
      if (bIsShadowMapCallback)
      {
        INSERT_PERF_MARKER_SCOPE("Decoration groups : Render to Shadowmap");
        for (int i=0;i<m_iVisibleGroupCount;i++)
          m_VisibleGroups.GetDataPtr()[i]->RenderVisibleInstancesToShadowMap(pCollector, pShadowData->m_pGenerator, fLightToViewFrustumDistances);
      }
      else
      {
        INSERT_PERF_MARKER_SCOPE("Decoration groups : Render");
        for (int i=0;i<m_iVisibleGroupCount;i++)
          m_VisibleGroups.GetDataPtr()[i]->RenderVisibleInstances(pCollector);
      }
    }
    return;
  }

#if !defined( HK_ANARCHY )
  if ( (pData->m_pSender == &VShadowMapReceiverRenderer::OnDetermineShadowReceivers))
  {
    static VisStaticGeometryInstanceCollection_cl BoxGIs(1024);
    static VisEntityCollection_cl BoxEntities(1024);

    const VisStaticGeometryInstanceCollection_cl * pGICollection = ((VShadowReceiverDataObject_cl *)pData)->m_pGICollection;
    const VisEntityCollection_cl* pEntityCollection = ((VShadowReceiverDataObject_cl *)pData)->m_pEntityCollection;
    const hkvVec3& vExtrudeDir = ((VShadowReceiverDataObject_cl *)pData)->m_vExtrudeDirection;
    
    hkvAlignedBBox shadowBox(hkvNoInitialization);
    hkvAlignedBBox transBox(hkvNoInitialization);

    for (int i = 0; i < m_iVisibleGroupCount; i++)
    {
      VTerrainDecorationGroup* pTreeGroup = m_VisibleGroups[i];
      for (int j = 0; j < pTreeGroup->m_Instances.m_iCount; j++)
      {
        pTreeGroup->m_Instances.m_pInstances[j].GetBoundingBox(shadowBox);
        transBox = shadowBox;
        transBox.translate (vExtrudeDir * shadowBox.getSizeZ());
        shadowBox.expandToInclude(transBox);

        // determine affected primitives in the box and tag them
        BoxGIs.Clear();
        pGICollection->DetermineEntriesTouchingBox(shadowBox, BoxGIs);
        BoxGIs.TagEntries(); // mark these static geometry instances as relevant

        BoxEntities.Clear();
        pEntityCollection->DetermineEntriesTouchingBox(shadowBox, BoxEntities);
        BoxEntities.TagEntries();
      }
    }
    return;
  }
#endif

  // remove a destroyed collector from the  vis state list. It will cause the list to overflow otherwise when a lot of lost devices are triggered.
  if (pData->m_pSender==&IVisVisibilityCollector_cl::OnVisibilityCollectorDestroyed)
  {
    IVisVisibilityCollector_cl *pCollector = ((VisVisibilityCollectorDataObject_cl *)pData)->m_pCollector;
    for (int i=0;i<this->Count();i++)
      this->GetAt(i)->FreeCollectorVisStates(pCollector);
    return;
  }
/*
#ifdef WIN32
  if (pData->m_pSender==&Vision::Callbacks.OnGatherLightmapInfo)
  {
    VLightmapInfoDataObject_cl *pLMData = (VLightmapInfoDataObject_cl *)pData;
    for (int i=0;i<Count();i++)
      GetAt(i)->GatherLightmapInfo(pLMData->m_SceneInfo);
    return;
  }
#endif
*/
  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    RemoveAllGroups();
    return;
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
