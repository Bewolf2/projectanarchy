/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/SectorVisibilityZone.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VSectorVisibilityZone::VSectorVisibilityZone(VTerrainSector *pSector)
{
  m_pSector = pSector;
  m_pTerrain = pSector->GetSectorManager()->m_pTerrain;

//  m_pVisibilityInfo = NULL;

  SetZoneFlags(GetZoneFlags() | VIS_VISIBILITYZONEFLAGS_FALLBACKZONE | VIS_VISIBILITYZONEFLAGS_INFINITEHEIGHTFORASSIGNMENT);
  hkvAlignedBBox visBox(m_pSector->GetBoundingBox());
  visBox.m_vMax.z += m_pTerrain->m_Config.m_fVisibilityHeightOverTerrain;
  visBox.m_vMin.z -= m_pTerrain->m_Config.m_fVisibilityHeightOverTerrain;
  SetBoundingBox(visBox);
}

//serialization
VSectorVisibilityZone::VSectorVisibilityZone()
{
  m_pSector = NULL;
  m_pTerrain = NULL;
 // m_pVisibilityInfo = NULL;

  SetZoneFlags(GetZoneFlags() | VIS_VISIBILITYZONEFLAGS_FALLBACKZONE | VIS_VISIBILITYZONEFLAGS_INFINITEHEIGHTFORASSIGNMENT);
}



// serialization
V_IMPLEMENT_SERIAL( VSectorVisibilityZone, VisVisibilityZone_cl, 0, &g_VisionEngineModule );
void VSectorVisibilityZone::Serialize( VArchive &ar )
{
  // version 6 does not save the connections to neighbor zones to avoid recursion
  // Note: when exporting the mobile replacement version of the terrain (one vmesh for each sector) the connection
  //       between neighbor zones must be serialized, because there is actually no terrain in the mobile exported version
  //       from which the neighbor zone information can be re-consturcted.
#if !defined(_VISION_MOBILE)
  if (ar.IsSaving() && !m_pTerrain->SectorManager().IsTerrainMeshExportInProgress())
    m_pSector->ConnectNeighborZones(false);
#endif

  VisVisibilityZone_cl::Serialize(ar);

  if (!(GetZoneFlags()&VIS_VISIBILITYZONEFLAGS_INFINITEHEIGHTFORASSIGNMENT))
  {
    SetZoneFlags(GetZoneFlags() | VIS_VISIBILITYZONEFLAGS_INFINITEHEIGHTFORASSIGNMENT);
    SetBoundingBox(m_BoundingBox);
  }
}


void VSectorVisibilityZone::OnAddedToSceneManager(IVisSceneManager_cl *pManager)
{
  VisVisibilityZone_cl::OnAddedToSceneManager(pManager);
  if (m_pSector && !GetStaticGeometryInstances()->Contains(m_pSector))
    AddStaticGeometryInstance(m_pSector);
}

void VSectorVisibilityZone::OnRemovedFromSceneManager(IVisSceneManager_cl *pManager)
{
  if (m_pSector)
  {
    VTerrain *pTerrain = m_pSector->GetSectorManager()->m_pTerrain;
    
    // mark full terrain as unassigned, so we have to re-assign. This might happen when vForge update visibilty
    // with UseInEngine==false
    pTerrain->m_bAddedToSceneManager = false;

    if (GetStaticGeometryInstances()->Contains(m_pSector))
      RemoveGeometryInstance(m_pSector);
  }
  VisVisibilityZone_cl::OnRemovedFromSceneManager(pManager);
}

void VSectorVisibilityZone::OnTagVisible(IVisVisibilityCollector_cl *pCollector, VisFrustum_cl *pFrustum)
{
#if !defined(_VISION_MOBILE)
  if (VISION_UNLIKELY(m_pSector==NULL)) // this only happens when the scene has been accidentally exported without terrain
    return;

  VASSERT(m_pTerrain);
  VTerrainVisibilityCollectorComponent *pComponent = m_pTerrain->FindTerrainComponent(pCollector);
  if (VISION_UNLIKELY(pComponent==NULL))
  {
    m_pTerrain->EnsureCollectorComponentAdded(pCollector);
    pComponent = m_pTerrain->FindTerrainComponent(pCollector);
  }

  VTerrainVisibilityInfo &info(pComponent->m_VisibilityInfo);
  info.m_pCamFrustum = pFrustum;

  if (!info.m_bVisibleInContext || (m_pSector->GetVisibleBitmask()&info.m_iContextFilterMask)==0)
    return;

  if (!m_pSector->m_bPrepared || !m_pSector->IsLoaded()) // the sector will be loaded while rendering, but then the decoration isn't visible
  {
    pComponent->AddPendingSector(m_pSector);
    return;
  }
  
  m_pSector->PerformVisibility(*pComponent);
#endif
}

#define CONNECT_TO(dx,dy) \
{\
  int sx = m_pSector->m_iIndexX+(dx);\
  int sy = m_pSector->m_iIndexY+(dy);\
  if (sx>=0 && sy>=0 && sx<m_pSector->m_Config.m_iSectorCount[0] && sy<m_pSector->m_Config.m_iSectorCount[1])\
  {\
    VSectorVisibilityZone *pOtherZone = pSectorManager->GetSector(sx,sy)->m_spSectorZone;\
    if (!IsConnected(pOtherZone))\
      this->AddVisibilityZone(pOtherZone);\
  }\
}

void VSectorVisibilityZone::OnFinishVisibilityBuilt(IVisSceneManager_cl *pSceneManager)
{
  VisVisibilityZone_cl::OnFinishVisibilityBuilt(pSceneManager);
  VTerrainSectorManager* pSectorManager = m_pSector->GetSectorManager();

  // see whether we are still connected to our neighbors
  CONNECT_TO(-1,0);
  CONNECT_TO( 1,0);
  CONNECT_TO(0,-1);
  CONNECT_TO(0, 1);

  // inflate the assignment boxes at the edges so that (billboard) geometry outside the terrain is propertly assigned
  const float fLarge = 1000000000.f;
  int sx = m_pSector->m_iIndexX;
  int sy = m_pSector->m_iIndexY;
  if (sx==0)
    m_AssignmentBoundingBox.m_vMin.x -= fLarge;
  if (sy==0)
    m_AssignmentBoundingBox.m_vMin.y -= fLarge;
  if (sx==m_pSector->m_Config.m_iSectorCount[0]-1)
    m_AssignmentBoundingBox.m_vMax.x += fLarge;
  if (sy==m_pSector->m_Config.m_iSectorCount[1]-1)
    m_AssignmentBoundingBox.m_vMax.y += fLarge;

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
