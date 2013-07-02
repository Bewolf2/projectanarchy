/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAsset.hpp

#ifndef HKV_ASSET_HPP_INCLUDED
#define HKV_ASSET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Tags/hkvAssetTag.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>

#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

#include <deque>

class hkStreamReader;
class hkvAssetLibrary;
class hkvAssetLibraryReader;
class hkvAssetTrackedFile;
class hkvProfile;
class hkvTransformationRule;
struct hkvTransformationRuleTypeInfo;



/// \brief
///   This is the base class of all asset types. It provides an interface that needs
///   to be implemented by any derived asset type.
///
/// Each derived class needs to provide a static function that passes its hkvAssetTypeInfo
/// to the hkvAssetTypeManager that is called on plugin load and another function should
/// be called on plugin unload to remove the asset type from the hkvAssetTypeManager.
class hkvAsset : public hkReferencedObject, public hkvIProperties
{
  friend class hkvTransformationRule;

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef hkRefNew<hkvAsset> RefNew;
  typedef hkRefPtr<hkvAsset> RefPtr;
  typedef hkRefPtr<const hkvAsset> RefCPtr;


public:
  class Variant
  {
  public:
    Variant(const char* key, const char* target);

    ASSETFRAMEWORK_IMPEXP bool operator<(const Variant& other) const;

    ASSETFRAMEWORK_IMPEXP const char* getKey() const;
    ASSETFRAMEWORK_IMPEXP const char* getTarget() const;
    ASSETFRAMEWORK_IMPEXP bool isValid() const;
    
    void setKey(const char* key);
    void setTarget(const char* target);
    void setValid(bool valid);

  private:
    hkStringPtr m_key;
    hkStringPtr m_target;
    bool m_valid;
  };

  class Variants
  {
  public:
    ASSETFRAMEWORK_IMPEXP void clear();

    ASSETFRAMEWORK_IMPEXP int getCount() const;
    ASSETFRAMEWORK_IMPEXP const Variant* getVariant(hkUint32 index) const;

    ASSETFRAMEWORK_IMPEXP int indexOf(const char* key) const;

    ASSETFRAMEWORK_IMPEXP int setVariant(const char* key, const char* target);

    ASSETFRAMEWORK_IMPEXP bool renameVariant(hkUint32 index, const char* newName);
    ASSETFRAMEWORK_IMPEXP void setVariantTarget(hkUint32 index, const char* target, bool valid);
    ASSETFRAMEWORK_IMPEXP void remove(hkUint32 index);

    ASSETFRAMEWORK_IMPEXP void sort();
    ASSETFRAMEWORK_IMPEXP void removeEmptyTargets();
    ASSETFRAMEWORK_IMPEXP void validate();

    ASSETFRAMEWORK_IMPEXP void setFromProperty(const hkvProperty& prop);
    ASSETFRAMEWORK_IMPEXP hkvProperty toProperty(const char* propertyName, hkUint32 flags) const;

  public:
    ASSETFRAMEWORK_IMPEXP static const char* s_propTypeVariants;

  private:
    hkArray<Variant> m_variants;
  };


public:
  ASSETFRAMEWORK_IMPEXP hkvAsset();
  ASSETFRAMEWORK_IMPEXP virtual ~hkvAsset();

public:
  static int getNumInstances() { return s_numInstances; }

public: // Interface
  ASSETFRAMEWORK_IMPEXP virtual bool queryByString(const char* query) const;
  ASSETFRAMEWORK_IMPEXP virtual hkUint32 getTypeIndex() const = 0;
  ASSETFRAMEWORK_IMPEXP const hkvAssetTypeInfo* getType() const;

public: // Member functions
  ASSETFRAMEWORK_IMPEXP virtual hkFileSystem::TimeStamp getLastEditTime() const;
  ASSETFRAMEWORK_IMPEXP virtual void touch();

  ASSETFRAMEWORK_IMPEXP hkvAssetLibrary* getOwnerLibrary() const { return m_ownerLibrary; }
  ASSETFRAMEWORK_IMPEXP int getIndexInLibrary() const { return m_indexInLibrary; }
  ASSETFRAMEWORK_IMPEXP void setOwnerLibrary(hkvAssetLibrary* library, int indexInLibrary = HKV_INVALID_INDEX);

  ASSETFRAMEWORK_IMPEXP hkvAssetTrackedFile* getFile() const { return m_file; }
  ASSETFRAMEWORK_IMPEXP void setFile(hkvAssetTrackedFile* file);

  ASSETFRAMEWORK_IMPEXP const char* getAbsoluteFilePath(hkStringBuf& out_path) const;
  ASSETFRAMEWORK_IMPEXP const char* getRelativeFilePath(hkStringBuf& out_path) const;

  ASSETFRAMEWORK_IMPEXP hkBool queryByTags(const hkvAssetTags& tags) const;
  ASSETFRAMEWORK_IMPEXP hkvAssetTags& getTags();
  ASSETFRAMEWORK_IMPEXP const hkvAssetTags& getTags() const;

  ASSETFRAMEWORK_IMPEXP virtual void makeDependencyList(std::vector<hkStringPtr>& out_list) const;
  ASSETFRAMEWORK_IMPEXP void setDependencyList(const std::vector<hkStringPtr>& dependencies);
  ASSETFRAMEWORK_IMPEXP virtual void updateDependenciesFromList();

  ASSETFRAMEWORK_IMPEXP const char* getName() const;

  ASSETFRAMEWORK_IMPEXP bool canCreateThumbnail() const;
  ASSETFRAMEWORK_IMPEXP const char* makeThumbnailKey(hkStringBuf& out_key) const;
  ASSETFRAMEWORK_IMPEXP const char* makeAbsoluteThumbnailPath(hkStringBuf& out_path) const;
  ASSETFRAMEWORK_IMPEXP void notifyMissingThumbnail();

  ASSETFRAMEWORK_IMPEXP virtual void clearAssetSpecificData() const {}
  ASSETFRAMEWORK_IMPEXP virtual hkvAssetOperationResult updateAssetSpecificData(hkStreamReader& fileData) const { return HKV_AOR_SUCCESS; }

  // Variants
  ASSETFRAMEWORK_IMPEXP Variants& getVariants() { return m_variants; }
  ASSETFRAMEWORK_IMPEXP const Variants& getVariants() const { return m_variants; }
  ASSETFRAMEWORK_IMPEXP void appendVariantLookupEntries(std::vector<hkvLookupTableEntry>& out_entries) const;

  // IProperties implementation
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

  ASSETFRAMEWORK_IMPEXP virtual void getCommonProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const;
  ASSETFRAMEWORK_IMPEXP virtual void setCommonProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose);

  ASSETFRAMEWORK_IMPEXP virtual void getFileProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const;
  ASSETFRAMEWORK_IMPEXP virtual void setFileProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose);

  virtual void getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const {}
  virtual void setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) {}

  ASSETFRAMEWORK_IMPEXP void makeSpecificPropertiesString(const char* entrySeparator, 
    const char* nameValueSeparator, bool flatten, hkStringBuf& out_string) const;

  ASSETFRAMEWORK_IMPEXP const hkvTransformationRule* getTransformRule(const char* profileName) const;
  hkBool hasCustomTransformRule() const;
  hkUint32 getTransformTemplate() const;
  void setTransformTemplate(const char* name, bool allowCreateTemplate);
  void setTransformTemplate(hkUint32 newTemplateIndex);

  ASSETFRAMEWORK_IMPEXP void dropUnusedProfiles();

private:
  hkvTransformationRule* getCustomTransformRule(const char* profileName) const;
  hkvTransformationRule* recreateCustomTransformRule(const char* profileName, const hkvTransformationRuleTypeInfo* destInfo) const;
  void createCustomTransformRules(hkUint32 sourceTemplateIndex);
  void deleteCustomTransformRules();

  // Property handling
  void setTransformationsProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose);


protected:
  hkvAssetTags m_tags;

  Variants m_variants;
  std::vector<hkStringPtr> m_dependencyList;

  hkvAssetLibrary* m_ownerLibrary;
  hkUint32 m_indexInLibrary;

  hkRefPtr<hkvAssetTrackedFile> m_file;

  mutable hkBool m_sourceFileChanged;

  mutable hkStringPtr m_templateName;

  typedef hkStorageStringMap<hkvTransformationRule*> TransformationRulesMap;
  mutable TransformationRulesMap m_transformationRules;

  hkUint32 m_lastMissingThumbnailHash;

private:
  static int s_numInstances;
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
