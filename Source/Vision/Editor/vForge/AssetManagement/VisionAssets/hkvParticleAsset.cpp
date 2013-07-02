/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvParticleAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>

unsigned int hkvParticleAsset::s_iAssetTypeIndex = HKV_INVALID_INDEX;

/////////////////////////////////////////////////////////////////////////////
// hkvParticleAssetType static functions
/////////////////////////////////////////////////////////////////////////////

void hkvParticleAsset::StaticInit()
{
  hkvAssetTypeInfo ti;
  ti.m_name = "Particle";
  ti.m_createFunc = &CreateAsset;
  ti.m_supportedFileExtensions.pushBack("pfx");
  ti.m_szTypeIconQt = ":/Icons/Icons/ParticleEffectAsset.png";

  ti.m_resourceManagerName = "Particles";
  ti.m_useEngineForDependencies = true;
  ti.m_useEngineForThumbnails = true;

  // register at the hkvAssetTypeManager and store the asset type index in static variable.
  //s_iAssetTypeIndex = hkvAssetTypeManager::getGlobalInstance()->addAssetType(ti);
}


void hkvParticleAsset::StaticDeInit()
{
  // de-register at the hkvAssetTypeManager
  //hkvAssetTypeManager::getGlobalInstance()->removeAssetType(s_iAssetTypeIndex);
  s_iAssetTypeIndex = HKV_INVALID_INDEX;
}


hkvAsset* hkvParticleAsset::CreateAsset()
{
  hkvAsset* pAsset = new hkvParticleAsset;
  VASSERT(pAsset)

  return pAsset;
}



/////////////////////////////////////////////////////////////////////////////
// hkvParticleAssetType public functions
/////////////////////////////////////////////////////////////////////////////

hkvParticleAsset::hkvParticleAsset()
{

}


hkvParticleAsset::~hkvParticleAsset()
{

}


/////////////////////////////////////////////////////////////////////////////
// hkvParticleAssetType public override functions
/////////////////////////////////////////////////////////////////////////////

unsigned int hkvParticleAsset::getTypeIndex() const
{
  return s_iAssetTypeIndex;
}


const char* hkvParticleAsset::getTypeName() const
{
  return "Particle";
}


void hkvParticleAsset::getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  // TODO
}

void hkvParticleAsset::setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int iStackIndex, hkvProperty::Purpose purpose)
{
  // TODO
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
