/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/CommonTerrainFilters.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/TerrainLockObject.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VImportHeightmapRAWFilter::VImportHeightmapRAWFilter(const char *szRawFile, float fScale)
{
  m_fScale = fScale;
  VASSERT(szRawFile != NULL);
  strcpy(m_szRawFile,szRawFile);
}


bool VImportHeightmapRAWFilter::Init(const VLargeBoundingBox& affectedArea, void *pUserData)
{
  m_pRawStream = Vision::File.Open(m_szRawFile);
  if (!m_pRawStream)
    return false;

  int iSize = m_pRawStream->GetSize();
  int iSamples = (int)hkvMath::sqrt((float)iSize/sizeof(short));
  m_iSrcSizeX = m_iSrcSizeY = iSamples;
  m_iRemainingSampleCount = iSamples;

  // store enough memory for one row of sectors, but stride of the file
  m_iBufferSize = m_iSrcSizeX*m_pTerrain->Config().m_iHeightSamplesPerSector[1];
  
  m_iCurrentSectorRow = -1;
  return true;
}


bool VImportHeightmapRAWFilter::Do(void *pUserData)
{
  int iSamplesX = m_pTerrain->Config().m_iHeightSamplesPerSector[0];
  int iSamplesY = m_pTerrain->Config().m_iHeightSamplesPerSector[1];

  int x1,y1,x2,y2;
  m_pTerrain->Config().GetHeightSampleIndicesAtPos(m_AffectedArea.m_vMin,x1,y1);
  m_pTerrain->Config().GetHeightSampleIndicesAtPos(m_AffectedArea.m_vMax,x2,y2);

  int iWidth = hkvMath::Min(m_iSrcSizeX, x2-x1+1);
  int iHeight = hkvMath::Min(m_iSrcSizeY, y2-y1+1);
  for (int y=0;y<iHeight;y++)
  {
    // lock row-wise
    VTerrainLockObject lockObj(x1,y1+y,iWidth,1, 0);
    m_pTerrain->LockHeightValues(lockObj);
    for (int x=0;x<m_iSrcSizeX;x++)
    {
      unsigned short iValue;
      m_pRawStream->Read(&iValue, sizeof(short));
      float *pDest = (float *)lockObj.m_pData;
      if (x<iWidth)
        pDest[x] = (float)iValue * m_fScale;
    }
    m_pTerrain->UnlockHeightValues(lockObj);

    // do the following at sector row change to purge old rows
    if ((y%iSamplesY)==0)
    {
      m_pTerrain->m_SectorManager.UnloadUnusedResources(0.5f, false);
      VManagedResource::IncGlobalTime(1.f);
      Vision::ResourceSystem.UpdateResourceViewer();
    }
  }

  return true;
}

bool VImportHeightmapRAWFilter::DeInit(void *pUserData)
{
  m_pRawStream->Close();
  m_pRawStream = NULL;
  return true;
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
