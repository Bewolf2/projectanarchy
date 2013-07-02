/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvCollection.hpp

#ifndef HKV_COLLECTION_HPP_INCLUDED
#define HKV_COLLECTION_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>

#include <map>
#include <vector>

// This class stores a list of files that is stored in a vcollection file.
// This file is considered an asset an will be transformed into a vres file
// that can be used to stream data by the engine.
class hkvCollection : public hkvPropertyReader
{
public:
  ASSETFRAMEWORK_IMPEXP hkvCollection();
  ASSETFRAMEWORK_IMPEXP ~hkvCollection();

  ASSETFRAMEWORK_IMPEXP hkResult loadCollection(const char* absolutePath);
  ASSETFRAMEWORK_IMPEXP hkResult saveCollection(const char* absolutePath) const;

  ASSETFRAMEWORK_IMPEXP hkResult loadResourceSnapshot(const char* absolutePath);
  ASSETFRAMEWORK_IMPEXP hkResult saveResourceSnapshot(const char* absolutePath) const;

  ASSETFRAMEWORK_IMPEXP std::vector<hkStringPtr>& getCollection();
  ASSETFRAMEWORK_IMPEXP const std::vector<hkStringPtr>& getCollection() const;

  ASSETFRAMEWORK_IMPEXP void buildEntryMap(std::map<hkStringBuf, hkUint32>& entryMap, const char* szRelativeCollectionPath) const;

  virtual hkvJsonStreamReadHandler::ParsingResult ParseMapKey(const unsigned char* value, size_t len);

private:
  static void buildEntryMap(std::map<hkStringBuf, hkUint32>& entryMap, std::vector<hkStringPtr>& stack, const char* szParentName, const char* szEntryName);

  class CollectionData : public hkvIProperties
  {
  public:
    ASSETFRAMEWORK_IMPEXP virtual const char* getTypeName() const HKV_OVERRIDE;
    ASSETFRAMEWORK_IMPEXP virtual void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
    ASSETFRAMEWORK_IMPEXP virtual void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

    std::vector<hkStringPtr> m_entries;
  };
  
  CollectionData m_data;
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
