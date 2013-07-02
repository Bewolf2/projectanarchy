/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvCollectionAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Collection/hkvCollection.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>

unsigned int hkvCollectionAsset::s_iAssetTypeIndex = HKV_INVALID_INDEX;

/////////////////////////////////////////////////////////////////////////////
// hkvCollectionAsset static functions
/////////////////////////////////////////////////////////////////////////////

void hkvCollectionAsset::StaticInit()
{
  hkvAssetTypeInfo ti;
  ti.m_name = "Collection";
  ti.m_createFunc = &CreateAsset;
  ti.m_determineDependenciesFunc = &DetermineDependencies;
  ti.m_supportedFileExtensions.pushBack("vcollection");
  ti.m_szTypeIconQt = ":/Icons/Icons/CollectionAsset.png";

   // deactivated for now, will come in a later release

  // register at the hkvAssetTypeManager and store the asset type index in static variable.
  //s_iAssetTypeIndex = hkvAssetTypeManager::getGlobalInstance()->addAssetType(ti);
}


void hkvCollectionAsset::StaticDeInit()
{
   // deactivated for now, will come in a later release

  // de-register at the hkvAssetTypeManager
  //hkvAssetTypeManager::getGlobalInstance()->removeAssetType(s_iAssetTypeIndex);
  s_iAssetTypeIndex = HKV_INVALID_INDEX;
}


hkvAsset* hkvCollectionAsset::CreateAsset()
{
  hkvAsset* pAsset = new hkvCollectionAsset;
  VASSERT(pAsset)

  return pAsset;
}

bool hkvCollectionAsset::DetermineDependencies(const char* libraryPath, const char* assetPath, std::vector<hkStringPtr>& out_dependencies)
{
  hkvCollection collection;
  hkStringBuf sAbsolutePath(libraryPath);
  sAbsolutePath.pathAppend(assetPath);

  hkResult res = collection.loadCollection(sAbsolutePath.cString());
  if (res == HK_FAILURE)
    return false;

  std::map<hkStringBuf, hkUint32> entryMap;
  collection.buildEntryMap(entryMap, assetPath);

  std::map<hkStringBuf, hkUint32>::iterator it = entryMap.begin();
  for (; it != entryMap.end(); ++it)
  {
    out_dependencies.push_back(it->first.cString());
  }

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// hkvCollectionAsset public functions
/////////////////////////////////////////////////////////////////////////////

hkvCollectionAsset::hkvCollectionAsset()
{

}


hkvCollectionAsset::~hkvCollectionAsset()
{

}


/////////////////////////////////////////////////////////////////////////////
// hkvCollectionAsset public override functions
/////////////////////////////////////////////////////////////////////////////

unsigned int hkvCollectionAsset::getTypeIndex() const
{
  return s_iAssetTypeIndex;
}


const char* hkvCollectionAsset::getTypeName() const
{
  return "Collection";
}


void hkvCollectionAsset::getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  // TODO
}

void hkvCollectionAsset::setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int iStackIndex, hkvProperty::Purpose purpose)
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
