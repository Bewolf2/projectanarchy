/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainDecorationInstance.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>




int VTerrainDecorationInstance::SortObjects( const void *elem1, const void *elem2)
{
  VTerrainDecorationInstance *pInst1 = (VTerrainDecorationInstance *)elem1;
  VTerrainDecorationInstance *pInst2 = (VTerrainDecorationInstance *)elem2;
  return pInst1->m_iSortingKey - pInst2->m_iSortingKey;
}

int VTerrainDecorationInstance::SortObjectPtrs( const void *elem1, const void *elem2)
{
  VTerrainDecorationInstance *pInst1 = *(VTerrainDecorationInstance **)elem1;
  VTerrainDecorationInstance *pInst2 = *(VTerrainDecorationInstance **)elem2;
  return pInst1->m_iSortingKey - pInst2->m_iSortingKey;
}

void VTerrainDecorationInstance::ComputeSortingKey(unsigned short iSubKey)
{
  VASSERT(m_spModel);
  int iHighBits = m_spModel->GetNumber();
  if (m_spModel->m_bNeedsLightmap && m_pOwnerSector!=NULL)
  {
    // lightmapped instances should be sorted by sector index
    iHighBits ^= ((((VManagedResource *)m_pOwnerSector)->GetNumber()&0x7f)<<8);
  }

  m_iSortingKey = (iHighBits << 16) | iSubKey;
}


void VTerrainDecorationInstance::SetTransformation(const hkvVec3& vPos, const hkvMat3 &rotation, float fScale)
{
  m_Orientation = rotation;
  m_vPosition = vPos;
  m_fScaling = fScale;
}



void VTerrainDecorationInstance::SerializeX(VArchive &ar, int iLocalVersion, bool bSerializeModel)
{
  if (ar.IsLoading())
  {
    if (iLocalVersion<DECORATIONINSTANCE_VERSION_20)
    {
      // old compatibility (4x4 transform)
      hkvMat4 temp4;
      ar >> temp4;
      m_vPosition = temp4.getTranslation();
      m_Orientation = temp4.getRotationalPart();
      m_fScaling = m_Orientation.getAxis(2).getLength();
    } else
    {
      m_vPosition.SerializeAsVec3(ar);
      ar >> m_Orientation >> m_fScaling;
      if (iLocalVersion>=DECORATIONINSTANCE_VERSION_21)
        ar >> m_InstanceColor;
    }

    if (bSerializeModel)
    {
      m_spModel = (IVTerrainDecorationModel *)ar.ReadProxyObject();
      VASSERT_MSG(m_spModel && m_spModel->GenerateInstances(), "Invalid model type");
      ComputeSortingKey(0); // compute the higher bits
    }
  } else
  {
    VASSERT(iLocalVersion==DECORATIONINSTANCE_CURRENT_VERSION);
    m_vPosition.SerializeAsVec3(ar);
    ar << m_Orientation << m_fScaling << m_InstanceColor;

    if (bSerializeModel)
    {
      VASSERT_MSG(m_spModel && m_spModel->GenerateInstances(), "Invalid model type");
      ar.WriteProxyObject(m_spModel);
    }
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
