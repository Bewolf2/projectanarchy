/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/SectorTile.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenerator.hpp>

VSectorTile::VSectorTile()
{
  m_iTileFlags = VSECTORTILEFLAGS_NONE;
  m_fMaxDecorationFarClip = 0.f;
  m_iFirstReferencedDecoration = m_iReferencedDecorationCount = 0;
}

VSectorTile::~VSectorTile()
{
}


void VSectorTile::Init(VTerrainSector *pOwner, int iIndexX, int iIndexY, const hkvAlignedBBox &bbox)
{
  m_pOwner = pOwner;
  m_iIndexX = iIndexX;
  m_iIndexY = iIndexY;
  m_TileBBox = bbox;
}

void VSectorTile::SerializeX(VArchive &ar, int iLocalVersion)
{
  if (ar.IsLoading())
  {
    m_TileBBox.SerializeAs_VisBoundingBox (ar);
    ar >> m_fMaxDecorationFarClip;
    if (iLocalVersion>=SECTOR_FILE_VERSION_14)
    {
    ar >> m_iFirstReferencedDecoration >> m_iReferencedDecorationCount;
    } else
    {
      short iStart,iCount;
      ar >> iStart >> iCount;
      m_iFirstReferencedDecoration = iStart;
      m_iReferencedDecorationCount = iCount;
    }
    ar >> m_iTileFlags;
  } else
  {
    m_TileBBox.SerializeAs_VisBoundingBox (ar);
    ar << m_fMaxDecorationFarClip;
    ar << m_iFirstReferencedDecoration << m_iReferencedDecorationCount; // 32bit (version 14)
    ar << m_iTileFlags;
  }
  VASSERT(m_iFirstReferencedDecoration+m_iReferencedDecorationCount<=m_pOwner->m_iReferencedDecorationCount)
}

float GetXYDistanceToAABB (const hkvAlignedBBox& aabb, const hkvVec3& vPos);

void VSectorTile::PerformVisibility(VTerrainVisibilityCollectorComponent &infoComp, VTerrainSectorDecorationVisibilityMask *pDecoVisMask)
{
  VISION_PROFILE_FUNCTION(VTerrainSectorManager::PROFILING_VISIBILITY_TILEVISIBILITY);
  VTerrainVisibilityInfo &visInfo(infoComp.m_VisibilityInfo);

  const float fDistanceToThisTile = GetXYDistanceToAABB (m_TileBBox, visInfo.m_vCamLODPos);

  //Vision::Game.DrawBoundingBox(m_TileBBox);
  if ((visInfo.m_bAddDecoration && fDistanceToThisTile<m_fMaxDecorationFarClip*visInfo.m_fLODDistanceScale))
  {
    // clip in shadow context when not visible in main context
    const bool bShadowContext = visInfo.m_pSMGenerator!=NULL;
    if (bShadowContext && !visInfo.Helper_IsShadowBoxVisible(m_TileBBox))
      return;

    // add the referenced decoration to the queue
    int *pInstIndex = &m_pOwner->m_piReferencedDecoration[m_iFirstReferencedDecoration];
    for (int i=0;i<m_iReferencedDecorationCount;i++,pInstIndex++)
    {
      if (pDecoVisMask->IsVisible(*pInstIndex)) // this object is tagged already
        continue;

      VTerrainDecorationInstance *pInst = &m_pOwner->m_Decoration.m_pInstances[*pInstIndex];
      IVTerrainDecorationModel *pThisModel = pInst->m_spModel;
      const float fModelFarDist = pThisModel->m_Properties.m_fFarClip*visInfo.m_fLODDistanceScale;
      if (fDistanceToThisTile>fModelFarDist || (visInfo.m_iContextFilterMask&pThisModel->m_Properties.m_iVisibleBitmask)==0 || (bShadowContext && ((pThisModel->m_Properties.m_eModelFlags&IVTerrainDecorationModel::CastDynamicShadows)==0)))
      {
        // we can now fast forward to the next model type
        while (i<m_iReferencedDecorationCount-1 && m_pOwner->m_Decoration.m_pInstances[pInstIndex[1]].m_spModel==pThisModel)
        {
          i++;
          pInstIndex++;
        }
        continue; // but don't tag it since it might be visible on other tiles
      }
      const float fModelNearDist = pThisModel->m_Properties.m_fNearClip*visInfo.m_fLODDistanceScale;
      float fDistSqr = pInst->GetDistanceSqr(visInfo.m_vCamLODPos);
      if ((fDistSqr>=fModelNearDist*fModelNearDist) && (fDistSqr<fModelFarDist*fModelFarDist))
      {
        // mark instance as visible; sorting key must be set right before sorting (context specific)
        pDecoVisMask->SetVisible(*pInstIndex);
        visInfo.m_iEstimatedDecorationCount++; // this would conflict with MT, but this value is a rough estimate anyway
        continue;
      }
      // We could tag the instance here to ignore testing next time, but we'd another bitfield
    }
  }
}



void VSectorTile::RemoveDecorationInstanceReference(int iIndex)
{
  int *pInstIndex = &m_pOwner->m_piReferencedDecoration[m_iFirstReferencedDecoration];
  int iOldCount = m_iReferencedDecorationCount;
  m_iReferencedDecorationCount = 0;
  for (int i=0;i<iOldCount;i++)
    if (pInstIndex[i] != iIndex)
      pInstIndex[m_iReferencedDecorationCount++] = pInstIndex[i];

}


void VSectorTile::RemoveDecorationInstanceReferences(const VTBitfield<1024> &removeIndices)
{
  int *pInstIndex = &m_pOwner->m_piReferencedDecoration[m_iFirstReferencedDecoration];
  int iOldCount = m_iReferencedDecorationCount;
  m_iReferencedDecorationCount = 0;
  for (int i=0;i<iOldCount;i++)
    if (!removeIndices.IsBitSet(pInstIndex[i]))
      pInstIndex[m_iReferencedDecorationCount++] = pInstIndex[i];
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
