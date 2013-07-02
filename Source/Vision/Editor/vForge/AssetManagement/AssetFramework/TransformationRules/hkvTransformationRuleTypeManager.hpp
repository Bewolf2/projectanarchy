/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTransformationRuleTypeManager.hpp

#ifndef HKV_TRANSFORMATION_RULE_TYPE_MANAGER_HPP_INCLUDED
#define HKV_TRANSFORMATION_RULE_TYPE_MANAGER_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>

#include <Common/Base/Container/Queue/hkQueue.h>
#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

class hkvProfile;

class hkvTransformationRuleTypeManager
{
public:
  ASSETFRAMEWORK_IMPEXP static hkvTransformationRuleTypeManager* getGlobalInstance();
  ASSETFRAMEWORK_IMPEXP static void deInitTransformationRuleTypeManager();

public:
  ASSETFRAMEWORK_IMPEXP hkvTransformationRuleTypeManager();
  ASSETFRAMEWORK_IMPEXP ~hkvTransformationRuleTypeManager();
  ASSETFRAMEWORK_IMPEXP void clear();

  // Transformation Type Handling
  ASSETFRAMEWORK_IMPEXP hkUint32 getTypeCount() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 addType(const hkvTransformationRuleTypeInfo& typeInfo);
  ASSETFRAMEWORK_IMPEXP void removeType(hkUint32 typeIndex);
  ASSETFRAMEWORK_IMPEXP const hkvTransformationRuleTypeInfo* getTypeInfo(hkUint32 typeIndex) const;
  ASSETFRAMEWORK_IMPEXP const hkvTransformationRuleTypeInfo* getTypeInfo(const char* name) const;
  ASSETFRAMEWORK_IMPEXP const hkArray<hkvTransformationRuleTypeInfo>& getTypeInfos() const;
 
  ASSETFRAMEWORK_IMPEXP hkResult getTypeInfosForAsset(const char* assetType, hkvEnumInstance& out_ruleTypes) const;
  ASSETFRAMEWORK_IMPEXP hkResult getTypeInfosForAsset(const char* assetType, hkArray<unsigned int>& out_ruleTypes) const;
  ASSETFRAMEWORK_IMPEXP const hkvTransformationRuleTypeInfo* getDefaultTypeInfoForAsset(const char* assetType) const;
  
  // Transformation Type Helper Functions
  ASSETFRAMEWORK_IMPEXP hkvTransformationRule* createTransformationRule(const char* profileName, const char* ruleName) const;
  ASSETFRAMEWORK_IMPEXP hkvTransformationRule* recreateTransformationRule(hkvTargetPlatform platform, const hkvTransformationRuleTypeInfo* destInfo, hkvTransformationRule* existingRule) const;

private:
  void clearAssetTypeLookup();
  void addToAssetTypeLookup(const hkvTransformationRuleTypeInfo& typeInfo, hkUint32 typeIndex);
  void updateEnumDefinition();

private: // Data
  struct TypeList
  {
    hkArray<hkUint32> m_typeIndices;
  };

  hkArray<hkvTransformationRuleTypeInfo> m_types;
  hkQueue<hkUint32> m_freelist;

  hkStorageStringMap<TypeList*> m_assetTypeLookup;
  hkvEnumDefinition m_transformRuleTypeDefinition;

private: // static members
  static hkvTransformationRuleTypeManager* s_globalInstance;
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
