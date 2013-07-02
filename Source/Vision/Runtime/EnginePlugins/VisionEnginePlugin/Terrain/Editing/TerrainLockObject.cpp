/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VTerrainLockObject::VTerrainLockObject(VEditableTerrainSector *pSector,int iFlags)
{
  int iSizeX = pSector->m_Config.m_iHeightSamplesPerSector[0];
  int iSizeY = pSector->m_Config.m_iHeightSamplesPerSector[1];
  Init(iSizeX*pSector->m_iIndexX,iSizeY*pSector->m_iIndexY,iSizeX+2,iSizeY+2,iFlags);
  m_iComponentSize = sizeof(float);
  m_pTerrain = (VEditableTerrain *)pSector->GetSectorManager()->m_pTerrain;
}



void VTerrainLockObject::SetSectorRange(int x1,int y1,int x2,int y2)
{
  VASSERT(m_pTerrain);
  m_pTerrain->Config().ClampSectorRectangle(x1,y1,x2,y2);
  m_iSectorRect[0] = x1;
  m_iSectorRect[1] = y1;
  m_iSectorRect[2] = x2;
  m_iSectorRect[3] = y2;
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
