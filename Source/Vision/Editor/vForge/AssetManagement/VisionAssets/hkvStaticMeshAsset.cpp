/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvStaticMeshAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Engine/System/Resource/VisApiResource.hpp>

unsigned int hkvStaticMeshAsset::s_iAssetTypeIndex = HKV_INVALID_INDEX;

/////////////////////////////////////////////////////////////////////////////
// hkvStaticMeshAsset static functions
/////////////////////////////////////////////////////////////////////////////

void hkvStaticMeshAsset::StaticInit()
{
  hkvAssetTypeInfo ti;
  ti.m_name = "StaticMesh";
  ti.m_createFunc = &CreateAsset;
  ti.m_supportedFileExtensions.pushBack("vmesh");
  ti.m_szTypeIconQt = ":/Icons/Icons/StaticMeshAsset.png";

  ti.m_resourceManagerName = VIS_RESOURCEMANAGER_MESHES;
  ti.m_useEngineForDependencies = true;
  ti.m_useEngineForThumbnails = true;

  // register at the hkvAssetTypeManager and store the asset type index in static variable.
  s_iAssetTypeIndex = hkvAssetTypeManager::getGlobalInstance()->addAssetType(ti);
}


void hkvStaticMeshAsset::StaticDeInit()
{
  // de-register at the hkvAssetTypeManager
  hkvAssetTypeManager::getGlobalInstance()->removeAssetType(s_iAssetTypeIndex);
  s_iAssetTypeIndex = HKV_INVALID_INDEX;
}


hkvAsset* hkvStaticMeshAsset::CreateAsset()
{
  hkvAsset* pAsset = new hkvStaticMeshAsset;
  VASSERT(pAsset)

  return pAsset;
}



/////////////////////////////////////////////////////////////////////////////
// hkvStaticMeshAsset public functions
/////////////////////////////////////////////////////////////////////////////

hkvStaticMeshAsset::hkvStaticMeshAsset()
{

}


hkvStaticMeshAsset::~hkvStaticMeshAsset()
{

}


/////////////////////////////////////////////////////////////////////////////
// hkvStaticMeshAsset public override functions
/////////////////////////////////////////////////////////////////////////////

unsigned int hkvStaticMeshAsset::getTypeIndex() const
{
  return s_iAssetTypeIndex;
}


const char* hkvStaticMeshAsset::getTypeName() const
{
  return "StaticMesh";
}


void hkvStaticMeshAsset::getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  // TODO
}

void hkvStaticMeshAsset::setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int iStackIndex, hkvProperty::Purpose purpose)
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
