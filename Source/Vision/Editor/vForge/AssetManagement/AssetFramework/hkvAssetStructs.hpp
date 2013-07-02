/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetStructs.hpp

#ifndef HKV_ASSET_STRUCTS_HPP_INCLUDED
#define HKV_ASSET_STRUCTS_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Tags/hkvAssetTag.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>

#include <Common/Base/Types/Color/hkColor.h>

#include <vector>

class hkvAsset;
class hkvIProperties;
class hkvITransformationControlHost;

/////////////////////////////////////////////////////////////////////////////
// hkvTargetPlatform related
/////////////////////////////////////////////////////////////////////////////
enum hkvTargetPlatform
{
  HKV_TARGET_PLATFORM_DX9 = 0,
  HKV_TARGET_PLATFORM_DX11,
  HKV_TARGET_PLATFORM_XBOX360,
  HKV_TARGET_PLATFORM_PS3,
  HKV_TARGET_PLATFORM_PSVITA,
  HKV_TARGET_PLATFORM_IOS,
  HKV_TARGET_PLATFORM_ANDROID,
  HKV_TARGET_PLATFORM_WIIU,

  HKV_TARGET_PLATFORM_COUNT,

  HKV_TARGET_PLATFORM_ANY,
};

extern const char* hkvTargetPlatformNames[];

ASSETFRAMEWORK_IMPEXP const hkvEnumDefinition& hkvGetTargetPlatformDefinition();


/////////////////////////////////////////////////////////////////////////////
// Asset look-up table related
/////////////////////////////////////////////////////////////////////////////

/// \brief Represents a single entry in the lookup table.
struct hkvLookupTableEntry
{
  hkStringPtr m_sSourcePath;  ///< The original file path (should be all lower case)
  hkStringPtr m_sVariantKey;  ///< The variant key of this entry (optional)
  hkStringPtr m_sTargetPath;  ///< The file to which to map the source path
  hkStringPtr m_sMetaInfo;    ///< Meta info string

  /// \brief Combines the entry data into a single string, as it will be stored in the lookup table.
  ASSETFRAMEWORK_IMPEXP const char* getCombinedString(hkStringBuf& sBuffer) const;
};



/////////////////////////////////////////////////////////////////////////////
// hkvAssetTypeInfo related
/////////////////////////////////////////////////////////////////////////////
typedef hkvAsset* (*hkvCreateAssetFunc)(void);
typedef bool (*hkvDetermineDependenciesFunc)(const char* libraryPath, const char* assetPath, std::vector<hkStringPtr>& out_dependencies);
typedef bool (*hkvGenerateThumbnailFunc)(const char* libraryPath, const char* assetPath, const char* thumbnailPath);

/// \brief
///   The hkvAssetTypeInfo struct is used to register a new asset type
///   at the hkvAssetTypeManager.
struct hkvAssetTypeInfo
{
  hkvAssetTypeInfo() 
  : m_createFunc(NULL), m_determineDependenciesFunc(NULL), m_generateThumbnailFunc(NULL), 
    m_resourceManagerName(NULL), 
    m_useEngineForDependencies(false), m_useEngineForThumbnails(false)
  {
    m_szTypeIconQt = "";
  }

  hkvAssetTypeInfo(const hkvAssetTypeInfo& rhs)
  {
    *this = rhs;
  }

  ~hkvAssetTypeInfo() {}
  void operator= (const hkvAssetTypeInfo& rhs)
  {
    m_name = rhs.m_name;
    m_createFunc = rhs.m_createFunc;
    m_determineDependenciesFunc = rhs.m_determineDependenciesFunc;
    m_generateThumbnailFunc = rhs.m_generateThumbnailFunc;
    m_supportedFileExtensions = rhs.m_supportedFileExtensions;
    m_defaultThumbnailPath = rhs.m_defaultThumbnailPath;
    m_resourceManagerName = rhs.m_resourceManagerName;
    m_useEngineForDependencies = rhs.m_useEngineForDependencies;
    m_useEngineForThumbnails = rhs.m_useEngineForThumbnails;
    m_szTypeIconQt = rhs.m_szTypeIconQt;
  }

  bool supportsFile(const char* fileName) const
  {
    const char* extension = hkvStringHelper::getExtension(fileName);
    if (extension[0] == '.')
    {
      extension++;
    }

    const int numSupported = m_supportedFileExtensions.getSize();
    for (int supportedIdx = 0; supportedIdx < numSupported; ++supportedIdx)
    {
      if (hkvStringHelper::safeCompare(extension, m_supportedFileExtensions[supportedIdx].cString(), true) == 0)
      {
        return true;
      }
    }

    return false;
  }

  hkStringPtr m_name;
  hkvCreateAssetFunc m_createFunc;
  hkvDetermineDependenciesFunc m_determineDependenciesFunc;
  hkvGenerateThumbnailFunc m_generateThumbnailFunc;
  hkArray<hkStringPtr> m_supportedFileExtensions;

  hkStringPtr m_defaultThumbnailPath;
  const char* m_szTypeIconQt;

  const char* m_resourceManagerName;
  hkBool m_useEngineForDependencies;
  hkBool m_useEngineForThumbnails;
};


struct hkvTransformationInput
{
  hkvTransformationInput()
  : m_includeEditorPreview(false), m_sourceProperties(NULL), m_controlHost(NULL)
  {
  }

  hkvTransformationInput(const hkvTransformationInput& rhs) :
    m_sourceFile(rhs.m_sourceFile), m_targetPath(rhs.m_targetPath),
    m_targetBaseName(rhs.m_targetBaseName), m_targetHashString(rhs.m_targetHashString),
    m_includeEditorPreview(rhs.m_includeEditorPreview), m_sourceProperties(rhs.m_sourceProperties),
    m_controlHost(rhs.m_controlHost)
  {
  }

  hkvTransformationInput& operator=(const hkvTransformationInput& rhs)
  {
    m_sourceFile = rhs.m_sourceFile;
    m_targetPath = rhs.m_targetPath;
    m_targetBaseName = rhs.m_targetBaseName;
    m_targetHashString = rhs.m_targetHashString;
    m_includeEditorPreview = rhs.m_includeEditorPreview;
    m_sourceProperties = rhs.m_sourceProperties;
    m_controlHost = rhs.m_controlHost;
    return *this;
  }
  
  hkStringPtr m_sourceFile;
  hkStringPtr m_targetPath;
  hkStringPtr m_targetBaseName;
  hkStringPtr m_targetHashString;
  bool m_includeEditorPreview;
  const hkvIProperties* m_sourceProperties;
  hkvITransformationControlHost* m_controlHost;
};


enum hkvMessageSeverity
{
  HKV_MESSAGE_SEVERITY_INFO,
  HKV_MESSAGE_SEVERITY_WARNING,
  HKV_MESSAGE_SEVERITY_ERROR
};

struct hkvTransformationMessage
{
  hkvTransformationMessage(hkvMessageSeverity severity, const char* message) :
    m_severity(severity), m_message(message)
  {
  }

  hkvMessageSeverity m_severity;
  hkStringPtr m_message;
};


struct hkvTransformationOutputFileSpec
{
  hkvTransformationOutputFileSpec()
  {
  }

  hkvTransformationOutputFileSpec(const hkvTransformationOutputFileSpec& rhs)
    : m_variantKey(rhs.m_variantKey), m_fileName(rhs.m_fileName)
  {
  }

  hkvTransformationOutputFileSpec& operator=(const hkvTransformationOutputFileSpec& rhs)
  {
    m_variantKey = rhs.m_variantKey;
    m_fileName = rhs.m_fileName;
    return *this;
  }

  hkStringPtr m_variantKey;
  hkStringPtr m_fileName;
};


struct hkvTransformationOutput
{
  hkvTransformationOutput()
  {
  }
  
  hkvTransformationOutput(const hkvTransformationOutput& rhs)
  {
    m_messages.append(rhs.m_messages);
    m_outputFileSpecs.append(rhs.m_outputFileSpecs);
  }

  hkvTransformationOutput& operator=(const hkvTransformationOutput& rhs)
  {
    m_messages = rhs.m_messages;
    m_outputFileSpecs = rhs.m_outputFileSpecs;
    return *this;
  }

  hkArray<hkvTransformationMessage> m_messages;
  hkArray<hkvTransformationOutputFileSpec> m_outputFileSpecs;
};

class hkvTransformationRule;

typedef hkvTransformationRule* (*hkvCreateTransformationRuleFunc)(hkvTargetPlatform targetPlatform);

struct hkvTransformationRuleTypeInfo
{
  hkvTransformationRuleTypeInfo() 
  : m_createFunc(NULL), m_supportsAllTypes(false), m_priority(0)
  {
  }

  hkvTransformationRuleTypeInfo(const hkvTransformationRuleTypeInfo& rhs)
  {
    *this = rhs;
  }
  
  ~hkvTransformationRuleTypeInfo() {}
  
  void operator= (const hkvTransformationRuleTypeInfo& rhs)
  {
    m_name = rhs.m_name;
    m_createFunc = rhs.m_createFunc;
    m_supportedAssetTypes = rhs.m_supportedAssetTypes;
    m_supportsAllTypes = rhs.m_supportsAllTypes;
    m_priority = rhs.m_priority;
  }

  hkStringPtr m_name;
  hkvCreateTransformationRuleFunc m_createFunc;
  hkArray<hkStringPtr> m_supportedAssetTypes;
  bool m_supportsAllTypes;
  int m_priority;
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
