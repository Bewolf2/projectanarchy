/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainVisibilityCollector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenerator.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/IVShadowMapComponent.hpp>

void VTerrainVisibilityInfo::Helper_GetShadowBox(hkvAlignedBBox &result, const hkvAlignedBBox &sourceBox) const
{
  result = sourceBox;
  hkvVec3 vShadowOfs = m_vShadowExtrusion * sourceBox.getSizeZ();
  result.translate(vShadowOfs);
  result.expandToInclude(sourceBox);
}



bool VTerrainVisibilityInfo::Helper_IsShadowBoxVisible(const hkvAlignedBBox &sourceBox) const
{
  hkvAlignedBBox shadowBox(hkvNoInitialization);
  Helper_GetShadowBox(shadowBox, sourceBox);
  VASSERT(m_pSMGenerator!=NULL && m_pSMGenerator->GetMainFrustum()!=NULL);
  bool bResult = m_pSMGenerator->GetMainFrustum()->Overlaps(shadowBox);
  return bResult;
}



void VTerrainVisibilityInfo::Set(IVisVisibilityCollector_cl *pCollector,const VTerrainConfig &config)
{
  hkvVec3 vCamPos(hkvNoInitialization), vCamDir(hkvNoInitialization);
  
  pCollector->GetSourceObject()->GetPosition(vCamPos);
  vCamDir = pCollector->GetSourceObject()->GetDirection();

  VisRenderContext_cl *pLODContext = pCollector->GetLODReferenceRenderContext();
  VASSERT(pLODContext!=NULL && pLODContext->GetCamera()!=NULL);
  pLODContext->GetCamera()->GetPosition(m_vCamLODPos);

  m_vCamPos.FromRenderSpace(config,(const hkvVec3& )vCamPos);
  m_vCamVisPos = vCamPos;
  m_CameraPlane.setFromPointAndNormal(vCamPos,vCamDir);
  VASSERT(m_vCamPos.IsValid(config));

  // compute the rest
  float fNear,fFar;
  pCollector->GetClipPlanes(fNear,fFar);
  m_fMaxViewDistance = fFar;

  m_fLODDistanceInvScale = pLODContext->GetLODDistanceScaling() * VTerrainSectorManager::g_fDecorationDistanceInvScaling;
  if (m_fLODDistanceInvScale>0.f)
    m_fLODDistanceScale = 1.f/m_fLODDistanceInvScale;
  else 
    m_fLODDistanceScale = 0.f;

  // overestimate
  config.GetViewDistanceBox(m_CamBBox, m_vCamVisPos, fFar);
  m_VisibleRangeBox.FromRenderSpace(config,m_CamBBox);
  m_iContextFilterMask = pCollector->GetFilterBitmask();

  // reset min/max
  m_iVisibleSectorRange[0] = m_iVisibleSectorRange[1] = 32000;
  m_iVisibleSectorRange[2] = m_iVisibleSectorRange[3] = -32000;

  // reset some of the values:
  m_iVisibleDecorationCount = 0;
  m_iEstimatedDecorationCount = 0;

  static bool bEnableOpt = true;

  // shadowmap related:
  if (bEnableOpt && pCollector->GetTypeId()==V_RUNTIME_CLASS(VShadowmapVisibilityCollector))
  {
    m_pSMGenerator = ((VShadowmapVisibilityCollector *)pCollector)->m_pSMGenerator;
    float fShadowExtrusionFactor = m_pSMGenerator->GetShadowMapComponent()->GetShadowBoxExtrudeMultiplier();
    m_vShadowExtrusion = m_pSMGenerator->GetDirection();
    if (m_vShadowExtrusion.z<-0.01f) // normalize height
      m_vShadowExtrusion.z *= (-1.f/m_vShadowExtrusion.z);
    m_vShadowExtrusion *= fShadowExtrusionFactor;
    m_bCastDynamicShadows = (m_pSMGenerator->GetShadowMapComponent()->GetGeometryTypes()&SHADOW_CASTER_TERRAIN)>0;
  }
  else
    m_pSMGenerator = NULL;

}



// register the class
V_IMPLEMENT_DYNAMIC(VTerrainVisibilityCollectorComponent,IVisVisibilityCollectorComponent_cl,&g_VisionEngineModule);


VTerrainVisibilityCollectorComponent::VTerrainVisibilityCollectorComponent(IVisVisibilityCollector_cl *pOwnerCollector, VTerrain *pOwnerTerrain)
  : m_pPendingSector(64,NULL)
{
  VASSERT(pOwnerTerrain);
  m_pCollector = pOwnerCollector;
  m_pTerrain = pOwnerTerrain;
  m_iPendingSectorCount = 0;
  m_bHasVisibleReplacementMeshes = false;
  m_fLODBias = 0.f;
  m_fSectorDistLODMult = 0.f;

  const VTerrainConfig &cfg(pOwnerTerrain->m_Config);

  // allocate arrays
  m_iOverallMeshPageCount = cfg.m_iOverallMeshPageCount[0]*cfg.m_iOverallMeshPageCount[1];
  m_PageLODBuffer.EnsureCapacity(m_iOverallMeshPageCount);
  m_pPageLOD = (char *)m_PageLODBuffer.GetBuffer();
  memset(m_pPageLOD,-1,m_iOverallMeshPageCount); // flag invisible

  m_PageRangeBuffer.EnsureCapacity(m_iOverallMeshPageCount * sizeof(VSectorMeshRenderRange_t *));
  m_PageRangeNoHoleBuffer.EnsureCapacity(m_iOverallMeshPageCount * sizeof(VSectorMeshRenderRange_t *));
  m_pPageRenderRange = (VSectorMeshRenderRange_t **)m_PageRangeBuffer.GetBuffer();
  m_pPageRenderRangeNoHoles = (VSectorMeshRenderRange_t **)m_PageRangeNoHoleBuffer.GetBuffer();
  memset(m_pPageRenderRange,0,m_iOverallMeshPageCount * sizeof(VSectorMeshRenderRange_t *));
  memset(m_pPageRenderRangeNoHoles,0,m_iOverallMeshPageCount * sizeof(VSectorMeshRenderRange_t *));

  m_SectorVisible.AllocateBitfield(cfg.m_iSectorCount[0]*cfg.m_iSectorCount[1]);
  m_SectorReplacementVisible.AllocateBitfield(cfg.m_iSectorCount[0]*cfg.m_iSectorCount[1]);
}

VTerrainVisibilityCollectorComponent::~VTerrainVisibilityCollectorComponent()
{
}


void VTerrainVisibilityCollectorComponent::AddPendingSector(VTerrainSector *pSector)
{
  // the following must be thread-safe
  m_pTerrain->m_SectorManager.m_VisibilityMutex.Lock();
  if (m_pPendingSector.GetElementPos(pSector)<0)
  {
    m_pPendingSector[m_iPendingSectorCount++] = pSector;
/* this isn't even necessary as it is done in ProcessPendingSectors, which is the first thing called in EndVisibilityUpdate
    m_VisibilityInfo.m_iVisibleSectorRange[0] = hkvMath::Min(m_VisibilityInfo.m_iVisibleSectorRange[0],pSector->m_iIndexX);
    m_VisibilityInfo.m_iVisibleSectorRange[1] = hkvMath::Min(m_VisibilityInfo.m_iVisibleSectorRange[1],pSector->m_iIndexY);
    m_VisibilityInfo.m_iVisibleSectorRange[2] = hkvMath::Max(m_VisibilityInfo.m_iVisibleSectorRange[2],pSector->m_iIndexX);
    m_VisibilityInfo.m_iVisibleSectorRange[3] = hkvMath::Max(m_VisibilityInfo.m_iVisibleSectorRange[3],pSector->m_iIndexY);
*/
  }
  m_pTerrain->m_SectorManager.m_VisibilityMutex.Unlock();
}



void VTerrainVisibilityCollectorComponent::WaitForVisibilityTasks()
{
  VisRenderContext_cl *pContexts[1024];
  const int iCount = Vision::Renderer.GetAllRenderContexts(pContexts,1024);
  for (int i=0;i<iCount;i++)
    if (pContexts[i]->GetVisibilityCollector()!=NULL)
      pContexts[i]->GetVisibilityCollector()->WaitForAllTasks();
}

float GetXYDistanceToAABB (const hkvAlignedBBox& aabb, const hkvVec3& vPos)
{
  hkvVec2 clamped (hkvMath::Min (vPos.x, aabb.m_vMax.x), hkvMath::Min (vPos.y, aabb.m_vMax.y));
  clamped.x = hkvMath::Max (clamped.x, aabb.m_vMin.x) - vPos.x;
  clamped.y = hkvMath::Max (clamped.y, aabb.m_vMin.y) - vPos.y;
  return hkvMath::sqrt (clamped.x*clamped.x + clamped.y*clamped.y);
}


void VTerrainVisibilityCollectorComponent::ProcessPendingSectors()
{
  if (!m_iPendingSectorCount)
    return;

  // we have to stop all tasks here, otherwise the IsLoaded flag of sectors assert&crash
  WaitForVisibilityTasks();

  const VTerrainConfig &cfg(this->m_pTerrain->m_Config);

  //const VTerrainConfig &config(m_pTerrain->m_Config);
  //m_VisibilityInfo.Set(pContext,config,false); // this is not necessary here
  m_VisibilityInfo.m_pCamFrustum = m_pCollector->GetBaseFrustum();
  
  // perform a visibility on all pending sectors using the base frustum
  for (int i=0;i<m_iPendingSectorCount;i++)
  {
    VTerrainSector *pSector = m_pPendingSector.GetDataPtr()[i];
    m_pPendingSector.GetDataPtr()[i] = NULL;

    // this must be the same condition as used in VTerrainSector::PerformVisibility
    float fDistanceToThisSector = GetXYDistanceToAABB (pSector->GetBoundingBox(), m_VisibilityInfo.m_vCamLODPos);
    const bool bUseReplacement = cfg.m_bSupportsReplacement && fDistanceToThisSector>=cfg.m_fReplacementDistance;

    // ensure the sector is loaded (only when not using replacement)
    if (!bUseReplacement)
      pSector->PrepareSector();
    pSector->PerformVisibility(*this);
  }
  m_iPendingSectorCount = 0;
}



void VTerrainVisibilityCollectorComponent::BeginVisibility(IVisVisibilityCollector_cl *pCollector)
{
  memset(m_pPageLOD,-1,m_iOverallMeshPageCount); // flag invisible
  m_SectorVisible.Clear();
  m_SectorReplacementVisible.Clear();
  m_bHasVisibleReplacementMeshes = false;

  m_pTerrain->m_SectorManager.BeginVisibilityUpdate(*this);
}

void VTerrainVisibilityCollectorComponent::EndVisibility(IVisVisibilityCollector_cl *pCollector)
{
  m_pTerrain->m_SectorManager.EndVisibilityUpdate(*this);
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
