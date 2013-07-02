/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#ifdef SUPPORTS_TERRAIN 
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#endif


// -------------------------------------------------------------------------- //
// hkvSampledHeightFieldShape                                                
// -------------------------------------------------------------------------- //

hkvSampledHeightFieldShape::hkvSampledHeightFieldShape(const hkpSampledHeightFieldBaseCinfo &ci, const VTerrainSector *pSector)
 : hkpSampledHeightFieldShape(ci)
{
#ifdef SUPPORTS_TERRAIN
  VASSERT(pSector != NULL);
  m_pSector = pSector;
  m_iTerrainIndex = pSector->GetSectorManager()->m_pTerrain->GetNumber();
  m_iSectorIndexX = pSector->m_iIndexX;
  m_iSectorIndexY = pSector->m_iIndexY;
#endif
}

hkvSampledHeightFieldShape::hkvSampledHeightFieldShape(hkFinishLoadedObjectFlag flag)
 : hkpSampledHeightFieldShape(flag)
{
#ifdef SUPPORTS_TERRAIN
  if (flag.m_finishing == 1)
  {
    // reconstruct pointer to sector
    VTerrain *pTerrain = (VTerrain*)VTerrainManager::GlobalManager().GetResourceByIndex(m_iTerrainIndex);
    VASSERT(pTerrain != NULL);
    m_pSector = pTerrain->SectorManager().GetSector(m_iSectorIndexX, m_iSectorIndexY);
    VASSERT(m_pSector != NULL);
  }
#endif
}

hkReal hkvSampledHeightFieldShape::getHeightAtImpl(int x, int z) const
{
#ifdef SUPPORTS_TERRAIN
  // Note we flip the sampling of (Havok Z) to be (RES - Vision Y)
  // See vHavokTerrain::CreateHkRigidBody for explanation (and other alternatives) 
  return VIS2HK_FLOAT_SCALED(m_pSector->GetHeightAt(x, m_zRes-z-1));
#else
  return 0.0f;
#endif
}

// -------------------------------------------------------------------------- //
// hkvTriSampledHeightFieldCollection                                                
// -------------------------------------------------------------------------- //

#ifdef SUPPORTS_TERRAIN
static int GetLog2(int iValue)
{
  int iBit = 0;
  while (iValue>1) {iValue/=2;iBit++;}
  return iBit;
}
#endif

hkvTriSampledHeightFieldCollection::hkvTriSampledHeightFieldCollection(const VTerrainSector *pSector, const hkpSampledHeightFieldShape *pShape) 
  : hkpTriSampledHeightFieldCollection(pShape), m_tileHoleMask(16*16)
{
#ifdef SUPPORTS_TERRAIN
  // these are the two possible return values
  m_iCollide = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN);
  m_iNonCollide = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN_HOLE);

  // build a bitmask from all terrain tiles so we can perform a fast lookup in getCollisionFilterInfoImpl
  const VTerrainConfig &cfg(pSector->m_Config);
  m_iStrideX = cfg.m_iTilesPerSector[0];
  m_tileHoleMask.setSize(cfg.m_iTilesPerSector[0]*cfg.m_iTilesPerSector[1]);
  int iBit = 0;
  for (int y=0;y<cfg.m_iTilesPerSector[1];y++)
    for (int x=0;x<cfg.m_iTilesPerSector[0];x++,iBit++)
    {
      if (pSector->GetTile(x,cfg.m_iTilesPerSector[1]-1-y)->IsHole())
        m_tileHoleMask.set(iBit);
      else
        m_tileHoleMask.clear(iBit);
    }

  // since both values are power of two, we can store the result as a bit-index for even faster processing in getCollisionFilterInfoImpl
  m_iTileSamplesX = GetLog2(cfg.m_iHeightSamplesPerSector[0] / cfg.m_iTilesPerSector[0]);
  m_iTileSamplesY = GetLog2(cfg.m_iHeightSamplesPerSector[1] / cfg.m_iTilesPerSector[1]);
#endif
}

hkUint32 hkvTriSampledHeightFieldCollection::getCollisionFilterInfo(hkpShapeKey key) const
{
  // look up whether there is a hole at x/z using the bitmask
  int x = getXFromShapeKey(key) >> m_iTileSamplesX;
  int y = getZFromShapeKey(key) >> m_iTileSamplesY;
  const bool bIsHole = m_tileHoleMask.get(x+y*m_iStrideX) > 0;
  return bIsHole ? m_iNonCollide : m_iCollide;
}


// -------------------------------------------------------------------------- //
// hkvTriSampledHeightFieldBvTreeShape                                                
// -------------------------------------------------------------------------- //

#ifdef SUPPORTS_TERRAIN
// Custom raycast collector to filter out holes
class vHavokRayHitCollector_HoleSupport : public hkpRayHitCollector
{
public:
  vHavokRayHitCollector_HoleSupport(hkpRayHitCollector& collector, const hkpShapeRayCastInput& input) 
    : m_collector(collector), m_input(input)
  {
  }

  virtual void addRayHit(const hkpCdBody& cdBody, const hkpShapeRayCastCollectorOutput& hitInfo) HKV_OVERRIDE
  {
    const hkpShapeContainer* container = static_cast<const hkpTriSampledHeightFieldBvTreeShape*>(cdBody.getShape())->getContainer();
    if (m_input.m_rayShapeCollectionFilter != NULL)
    {
      if (m_input.m_rayShapeCollectionFilter->isCollisionEnabled(m_input, *container, (hkpShapeKey)hitInfo.m_extraInfo))
      {
        m_collector.addRayHit(cdBody, hitInfo);
      }
    }
    else
    {
      m_collector.addRayHit(cdBody, hitInfo);
    }
  }

private:
  hkpRayHitCollector& m_collector;
  const hkpShapeRayCastInput& m_input;

};
#endif

void hkvTriSampledHeightFieldBvTreeShape::castRayWithCollector(const hkpShapeRayCastInput& input, const hkpCdBody& cdBody, hkpRayHitCollector& collector) const
{
#ifdef SUPPORTS_TERRAIN
  vHavokRayHitCollector_HoleSupport collectorWithHoleSupport(collector, input);
  hkpTriSampledHeightFieldBvTreeShape::castRayWithCollector(input, cdBody, collectorWithHoleSupport);    
#endif
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
