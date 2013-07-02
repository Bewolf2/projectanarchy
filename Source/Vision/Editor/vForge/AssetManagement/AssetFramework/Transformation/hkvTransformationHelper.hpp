/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTextureTransformationHelper.hpp

#ifndef HKV_TEXTURE_TRANSFORMATION_HELPER_HPP_INCLUDED
#define HKV_TEXTURE_TRANSFORMATION_HELPER_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>

#include <deque>

class hkvAsset;
class hkvAssetLibrary;
class hkvTransformationRule;

class hkvTransformationHelper
{
private:
  hkvTransformationHelper();
  hkvTransformationHelper(const hkvTransformationHelper&);
  hkvTransformationHelper& operator=(const hkvTransformationHelper&);

public:
  ASSETFRAMEWORK_IMPEXP static bool appendLookupTableEntries(const hkvAssetLibrary& library, const hkvAsset& asset, const hkvTransformationRule& rule, bool forEditor, bool assetPreviewMode, std::vector<hkvLookupTableEntry>& out_entries);
  ASSETFRAMEWORK_IMPEXP static bool queryOutputFiles(const hkvAssetLibrary& library, const hkvAsset& asset, const hkvTransformationRule& rule, hkArray<hkStringPtr>& out_files);
  ASSETFRAMEWORK_IMPEXP static hkvAssetOperationResult transformAsset(const hkvAssetLibrary& library, const hkvAsset& asset, 
    const hkvTransformationRule& rule, hkArray<hkvTransformationMessage>& messages, 
    hkvITransformationControlHost* controlHost = NULL, bool addTransformedToRCS = false);

public:
  ASSETFRAMEWORK_IMPEXP static void filterRelevantProperties(const hkvPropertyList& properties, hkvPropertyList& out_properties);

private:
  static bool assetNeedsTransformation(const hkvAssetLibrary& library, const hkvAsset& asset, const hkvTransformationRule& rule);
  static bool fillTargetFileInformation(const hkvAssetLibrary& library, const hkvAsset& asset, const hkvTransformationRule& rule, hkvTransformationInput& transformationInput);
  static bool performTransformation(const hkvAssetLibrary& library, const hkvAsset& asset, const hkvTransformationRule& rule, 
    hkArray<hkvTransformationMessage>& messages, hkvITransformationControlHost* controlHost, bool addTransformedToRCS);
  static bool queryOutputFileSpecs(const hkvAssetLibrary& library, const hkvAsset& asset, const hkvTransformationRule& rule, bool includeEditorPreview, hkArray<hkvTransformationOutputFileSpec>& out_specs);
  static bool staticTargetExists(const hkvAssetLibrary& library, const hkvAsset& asset, const hkvTransformationRule& rule);
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
