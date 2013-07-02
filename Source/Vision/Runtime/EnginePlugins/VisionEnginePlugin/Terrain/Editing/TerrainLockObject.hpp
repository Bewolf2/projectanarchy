/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainLockObject.hpp

#ifndef TERRAINLOCKOBJECT_HPP_INCLUDED
#define TERRAINLOCKOBJECT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainModule.hpp>

class VTerrainSector;
class VEditableTerrain;
class VEditableTerrainSector;


/// \brief
///   Helper class for locking a region in the terrain to modify height values, vegetation density maps detail textures or holes. See respective Lock functions on editable terrain class.
class VTerrainLockObject
{
public:
  /// \brief
  ///   Constructor that takes the coordinate region to update. For height map update the region is defined in sample indices
  inline VTerrainLockObject(int x,int y, int iSizeX, int iSizeY, int iFlags)
  {
    Init(x,y, iSizeX,iSizeY, iFlags);
  }

  inline VTerrainLockObject()
  {
    Init(0,0,-1,-1,0);
  }

  TERRAIN_IMPEXP VTerrainLockObject(VEditableTerrainSector *pSector,int iFlags);

  void Init(int x,int y, int iSizeX, int iSizeY, int iFlags)
  {
    m_iPos[0] = x;
    m_iPos[1] = y;
    m_iSize[0] = iSizeX;
    m_iSize[1] = iSizeY;
    m_iComponentSize = m_iStride = -1;
    m_pData = NULL;
    m_bOwnsData = m_bLocked = false;
    m_iLockFlags = iFlags;
  }

  inline ~VTerrainLockObject()
  {
    if (m_bOwnsData)
      V_SAFE_FREE(m_pData);
  }

  inline bool IsValid() const {return (m_iSize[0]>0) && (m_iSize[1]>0);}
  inline int GetArea() const {VASSERT(IsValid());return m_iSize[0]*m_iSize[1];}
  void *GetData() const {return m_pData;}
  TERRAIN_IMPEXP void SetSectorRange(int x1,int y1,int x2,int y2);

  inline void CloneData(VTerrainLockObject &target)
  {
    target.Init(m_iPos[0],m_iPos[1],m_iSize[0],m_iSize[1],0);
    if (!IsValid())
      return;
    target.m_iStride = m_iStride;
    target.m_iComponentSize = m_iComponentSize;
    target.m_bOwnsData = true;
    int iByteCount = m_iStride*m_iSize[1]*m_iComponentSize;
    target.m_pData = vMemAlloc(iByteCount);
    memcpy(target.m_pData,m_pData,iByteCount);

    // copy additional properties (important since resource infos might be stored)
    target.m_pTerrain = m_pTerrain;
    memcpy(target.m_iSectorRect,m_iSectorRect,sizeof(m_iSectorRect));
    memcpy(target.m_iReserved,m_iReserved,sizeof(m_iReserved));
    memcpy(target.m_pReserved,m_pReserved,sizeof(m_pReserved));
  }

  int m_iPos[2];
  int m_iSize[2];
  int m_iStride; ///< stride in components
  int m_iComponentSize; ///< e.g. sizeof(float)
  bool m_bOwnsData,m_bLocked;
  void *m_pData;
  int m_iLockFlags;
  VEditableTerrain *m_pTerrain;
  int m_iSectorRect[4]; ///< only valid after lock operation

  //reserved:
  int m_iReserved[4];
  void *m_pReserved[4];
};


#endif

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
