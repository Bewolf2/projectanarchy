/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainCollisionMesh.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VTerrainCollisionMeshManager::VTerrainCollisionMeshManager() 
: VisResourceManager_cl("TerrainCollisionMeshes",VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER, 0)
{
  SetAutoUnloadInterval(30.f,60.f,false);
}



VSectorCollisionMeshes::VSectorCollisionMeshes(VTerrainSector *pOwnerSector)
: VManagedResource(&pOwnerSector->GetSectorManager()->CollisionMeshManager())
{
  const VTerrainConfig &config = pOwnerSector->GetSectorManager()->Config();
  SetResourceFlag(VRESOURCEFLAG_AUTODELETE|VRESOURCEFLAG_ALLOWUNLOAD);
  m_iCountX = config.m_iTilesPerSector[0];
  m_iCountY = config.m_iTilesPerSector[1];
  m_pCollMesh = new VSimpleCollisionMesh[m_iCountX*m_iCountY];
  SetFilename(pOwnerSector->GetFilename());
  FlagAsLoaded();
  EvaluateMemSize();
}


VSectorCollisionMeshes::~VSectorCollisionMeshes()
{
  V_SAFE_DELETE_ARRAY(m_pCollMesh);
}


BOOL VSectorCollisionMeshes::Unload() 
{
  const int iCount = m_iCountX*m_iCountY;

  SetNewMemSize(VRESOURCEMEMORY_ALLTYPES,0);

  VSimpleCollisionMesh *pMesh = m_pCollMesh;
  for (int i=0;i<iCount;i++,pMesh++)
  {
    pMesh->FreeVertices();
    pMesh->FreeIndices();
  }

  return TRUE;
}


void VSectorCollisionMeshes::EvaluateMemSize()
{
  const int iCount = m_iCountX*m_iCountY;
  int iSize = iCount*sizeof(VSimpleCollisionMesh);
  VSimpleCollisionMesh *pMesh = m_pCollMesh;
  for (int i=0;i<iCount;i++,pMesh++)
    iSize += pMesh->GetMemSize();
  SetNewMemSize(VRESOURCEMEMORY_SYSTEM,iSize);
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
