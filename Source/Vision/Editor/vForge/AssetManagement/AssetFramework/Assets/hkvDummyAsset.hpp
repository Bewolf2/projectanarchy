/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvDummyAsset.hpp

#ifndef HKV_DUMMY_ASSET_HPP_INCLUDED
#define HKV_DUMMY_ASSET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvTrackedFileDummy.hpp>

class hkvAssetTypeManager;

class hkvDummyAsset : public hkvAsset
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef hkRefNew<hkvDummyAsset> RefNew;
  typedef hkRefPtr<hkvDummyAsset> RefPtr;
  typedef hkRefPtr<const hkvDummyAsset> RefCPtr;

public: // static functions
  static void staticInit();
  static void staticDeInit();
  static hkvAsset* createAsset();

public: // public functions
  hkvDummyAsset();
  ~hkvDummyAsset();

  virtual hkFileSystem::TimeStamp getLastEditTime() const HKV_OVERRIDE;
  virtual void touch() HKV_OVERRIDE;

  const char* getAssetType() const { return m_assetType; }
  void setAssetType(const char* assetType) { m_assetType = assetType; }

  const char* getLoaderPath() const;
  void setLoaderPath(const char* loaderPath);

public: // overrides
  VOVERRIDE hkUint32 getTypeIndex() const;
  VOVERRIDE const char* getTypeName() const;

  virtual void makeDependencyList(std::vector<hkStringPtr>& out_list) const HKV_OVERRIDE;
  virtual void updateDependenciesFromList() HKV_OVERRIDE;

  virtual void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;

  virtual void getFileProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  virtual void setFileProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

  virtual void getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  virtual void setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

private:
  hkStringPtr m_assetType;
  hkRefPtr<hkvTrackedFileDummy> m_trackedFileDummy;
  hkvPropertyList m_properties;

  static hkUint32 s_assetTypeIndex;
  static hkUint32 s_dummyCount;
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
