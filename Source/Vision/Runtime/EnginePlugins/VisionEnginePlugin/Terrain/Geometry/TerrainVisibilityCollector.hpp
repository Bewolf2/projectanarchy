/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainVisibilityCollector.hpp

#ifndef TERRAINVISIBILITYCOLLECTOR_HPP_INCLUDED
#define TERRAINVISIBILITYCOLLECTOR_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>

class VTerrain;
class VShadowMapGenerator;
struct VSectorMeshRenderRange_t;

/// \brief
///   Helper class used in the visibility calculation. Holds information about the current camera
///   position, frustum etc.
class VTerrainVisibilityInfo
{
public:
  VTerrainVisibilityInfo() : m_VisibleDecoration(1024,NULL)
  {
    m_bAddDecoration = true;
    m_iVisibleDecorationCount = 0;
    m_iContextFilterMask = 0;
    m_bVisibleInContext = m_bCastDynamicShadows = false;
    m_fLODDistanceScale = m_fLODDistanceInvScale = 1.0f;
    m_pSMGenerator = NULL;

    // reset min/max
    m_iVisibleSectorRange[0] = m_iVisibleSectorRange[1] = 32000;
    m_iVisibleSectorRange[2] = m_iVisibleSectorRange[3] = -32000;
  }

  // set by terrain manager
  void Set(IVisVisibilityCollector_cl *pCollector,const VTerrainConfig &config);

  // called by sectors:
  inline void AddVisibleDecorationInstance(VTerrainDecorationInstance *pInstance)
  {
    IVTerrainDecorationModel *pModel = pInstance->m_spModel;
    VASSERT(pModel!=NULL);
    if (VISION_UNLIKELY(!pModel->IsLoaded()))
    {
      if (pModel->IsMissing())
        return;
      pModel->EnsureLoaded();
      if (pModel->IsMissing() || !pModel->IsValid())
        return;
    }
    pModel->UpdateTimeStamp(); // for resource management
    m_VisibleDecoration[m_iVisibleDecorationCount++] = pInstance;
  }

  TERRAIN_IMPEXP void Helper_GetShadowBox(hkvAlignedBBox &result, const hkvAlignedBBox &sourceBox) const;
  TERRAIN_IMPEXP bool Helper_IsShadowBoxVisible(const hkvAlignedBBox &sourceBox) const;

  // information
  VLargePosition m_vCamPos;
  hkvVec3 m_vCamVisPos;
  hkvVec3 m_vCamLODPos;
  hkvPlane m_CameraPlane;
  const VisFrustum_cl *m_pCamFrustum;
  VShadowMapGenerator *m_pSMGenerator;

  hkvAlignedBBox m_CamBBox;
  VLargeBoundingBox m_VisibleRangeBox; ///< same as m_CamBBox
  short m_iVisibleSectorRange[4]; ///< min/max index of visible sectors

  bool m_bVisibleInContext, m_bCastDynamicShadows;
  unsigned int m_iContextFilterMask;

  // filled in by sectors:
  bool m_bAddDecoration;
  int m_iVisibleDecorationCount, m_iEstimatedDecorationCount;
  DynArray_cl<VTerrainDecorationInstance *>m_VisibleDecoration;

  float m_fMaxViewDistance;
  float m_fLODDistanceScale, m_fLODDistanceInvScale;
  hkvVec3 m_vShadowExtrusion;

};



/// \brief
///   Implements the IVisVisibilityCollectorComponent_cl component to hook into default visibility
///   process
/// 
/// This instance stores per-collector specific terrain visibility, so visibility can be computed 
/// 
/// The terrain registers itself to every new collector.
class VTerrainVisibilityCollectorComponent : public IVisVisibilityCollectorComponent_cl
{
public:
  TERRAIN_IMPEXP VTerrainVisibilityCollectorComponent(IVisVisibilityCollector_cl *pOwnerCollector, VTerrain *pOwnerTerrain);
  TERRAIN_IMPEXP virtual ~VTerrainVisibilityCollectorComponent();

  // overridden IVisVisibilityCollectorComponent_cl functions:
  TERRAIN_IMPEXP virtual void BeginVisibility(IVisVisibilityCollector_cl *pCollector);
  TERRAIN_IMPEXP virtual void EndVisibility(IVisVisibilityCollector_cl *pCollector);

  TERRAIN_IMPEXP void AddPendingSector(VTerrainSector *pSector);
  TERRAIN_IMPEXP void ProcessPendingSectors();

  inline bool IsSectorVisible(int iSectorIndex) const {return m_SectorVisible.IsBitSet(iSectorIndex);}
  inline bool IsSectorReplacementVisible(int iSectorIndex) const {return m_SectorReplacementVisible.IsBitSet(iSectorIndex);}
  void WaitForVisibilityTasks();
public:
  V_DECLARE_DYNAMIC_DLLEXP(VTerrainVisibilityCollectorComponent,TERRAIN_IMPEXP)

  IVisVisibilityCollector_cl *m_pCollector;
  VTerrain *m_pTerrain;

  VTerrainVisibilityInfo m_VisibilityInfo;

  int m_iOverallMeshPageCount;
  char *m_pPageLOD;
  VSectorMeshRenderRange_t **m_pPageRenderRange;
  VSectorMeshRenderRange_t **m_pPageRenderRangeNoHoles;
  bool m_bHasVisibleReplacementMeshes;

  VTBitfield<16*16> m_SectorVisible;
  VTBitfield<16*16> m_SectorReplacementVisible;

  // in case sectors are't loaded at visibility calculation time, they go here and are processed later in ProcessPendingSectors
  int m_iPendingSectorCount;
  DynArray_cl<VTerrainSector *>m_pPendingSector;

  float m_fLODBias, m_fSectorDistLODMult;
//  int m_iDebugState;
private:
  VMemoryTempBuffer<16*16*sizeof(char)> m_PageLODBuffer;
  VMemoryTempBuffer<16*16*sizeof(VSectorMeshRenderRange_t *)> m_PageRangeBuffer;
  VMemoryTempBuffer<16*16*sizeof(VSectorMeshRenderRange_t *)> m_PageRangeNoHoleBuffer;
};


/// \brief
///   Helper struct that stores per sector/per collector visibility information for decoration,
///   represented by a bitmask.
/// 
/// To avoid extra allocation, an instance is allocated as one block of memory containing enough
/// space for all bits.
struct VTerrainSectorDecorationVisibilityMask
{
  // tests a bit in the bitfield
  inline bool IsVisible(int iIndex) const
  {
    VASSERT((iIndex>>5)<m_iIntCount);
    const unsigned int *pBits = &m_iFirstBits;
    return (pBits[iIndex>>5] & (1U<<(iIndex&31))) != 0;
  }

  // sets a bit in the bitfield
  inline void SetVisible(int iIndex)
  {
    VASSERT((iIndex>>5)<m_iIntCount);
    unsigned int *pBits = &m_iFirstBits;
    pBits[iIndex>>5] |= (1U<<(iIndex&31));
  }

  // clears the bitfield
  inline void Clear()
  {
    memset(&m_iFirstBits,0,m_iIntCount*sizeof(int));
  }

  // loops through all bits and adds the set bits to the pointer list for rendering
  inline void AddInstances(VTerrainVisibilityCollectorComponent &dest, VTerrainDecorationInstance *pInstanceList)
  {
    unsigned int *pBits = &m_iFirstBits;
    for (int i=0;i<m_iIntCount;i++)
    {
      unsigned int iMask = pBits[i];
      int iIndex = i<<5;
      while (iMask)
      {
        if (iMask&1)
          dest.m_VisibilityInfo.AddVisibleDecorationInstance(&pInstanceList[iIndex]);
        iMask >>=1;
        iIndex++;
      }
    }
  }

  // static function to create one instance for a specific collector 
  inline static VTerrainSectorDecorationVisibilityMask *Create(VTerrainVisibilityCollectorComponent *pComponent,int iNumObjects)
  {
    int iIntCount = (iNumObjects+31)/32;
    int iAllocCount = iIntCount*sizeof(int) + offsetof(VTerrainSectorDecorationVisibilityMask,m_iFirstBits);
    VTerrainSectorDecorationVisibilityMask *pObj = (VTerrainSectorDecorationVisibilityMask *)vMemAlignedAlloc(iAllocCount,16);
    pObj->m_iIntCount = iIntCount;
    pObj->m_pComponent = pComponent;
    pObj->m_pNext = NULL;
    pObj->Clear();
    return pObj;
  }

  // searches the linked list for a specific collector, and optionally adds a new instance if not found
  static VTerrainSectorDecorationVisibilityMask* GetForCollector(VTerrainSectorDecorationVisibilityMask *&pFirst, VTerrainVisibilityCollectorComponent *pComponent, int iNumObjects)
  {
    VTerrainSectorDecorationVisibilityMask *pCurrent = pFirst;
    VTerrainSectorDecorationVisibilityMask *pLast = pFirst;
    while (pCurrent)
    {
      if (pCurrent->m_pComponent==pComponent)
        return pCurrent;
      pLast = pCurrent;
      pCurrent = pCurrent->m_pNext;
    }
    if (iNumObjects<0) ///< just find?
      return NULL;
    VTerrainSectorDecorationVisibilityMask *pNew = Create(pComponent,iNumObjects); ///< add to the linked list
    if (pFirst==NULL)
      pFirst = pNew;
    if (pLast)
      pLast->m_pNext = pNew;
    return pNew;
  }

  // deletes all instances in the linked list
  static void DeleteRecursive(VTerrainSectorDecorationVisibilityMask *pFirst)
  {
    while (pFirst)
    {
      VTerrainSectorDecorationVisibilityMask *pThis = pFirst;
      pFirst = pThis->m_pNext;
      vMemAlignedFree(pThis);
    }
  }


  VTerrainVisibilityCollectorComponent *m_pComponent; ///< the target compoennt
  VTerrainSectorDecorationVisibilityMask *m_pNext; ///< next instance in the linked list
  int m_iIntCount; ///< number of 32bit integers
  unsigned int m_iFirstBits; ///< this is the first 32 bit value
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
