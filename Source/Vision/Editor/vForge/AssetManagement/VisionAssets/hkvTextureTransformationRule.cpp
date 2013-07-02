/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvExternalToolWiiUTexConv2.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvExternalToolNvDxt.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvExternalToolPvrTexTool.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvExternalToolTexConv.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvImageFileProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationSettings.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTransformationStepImageToDds.hpp>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRuleTypeManager.hpp>

#include <Common/Base/Container/String/hkUtf8.h>
#include <Common/Base/Math/hkMath.h>


static const char VARIANT_ANDROID_DEFAULT[] = "android-default";
static const char VARIANT_ANDROID_PVR[] = "android-pvr";
static const char VARIANT_VFORGE[] = "vforge";



hkvTextureTransformationRule::TransformationStepInfo::TransformationStepInfo()
: m_stepType(STEP_TYPE_INVALID), m_targetExtension(NULL), m_settings(HKV_TARGET_PLATFORM_ANY)
{
}

hkvTextureTransformationRule::TransformationStepInfo::TransformationStepInfo(
  TransformationStepType stepType, const char* targetExtension, const hkvTextureTransformationSettings& settings)
: m_stepType(stepType), m_targetExtension(targetExtension), m_settings(settings)
{
}

hkvTextureTransformationRule::TransformationStepInfo::TransformationStepInfo(
  const TransformationStepInfo& other)
: m_stepType(other.m_stepType), m_targetExtension(other.m_targetExtension), m_settings(other.m_settings)
{
}

hkvTextureTransformationRule::TransformationStepInfo& hkvTextureTransformationRule::TransformationStepInfo::operator=(
  const hkvTextureTransformationRule::TransformationStepInfo& other)
{
  m_stepType = other.m_stepType;
  m_targetExtension = other.m_targetExtension;
  m_settings = other.m_settings;
  return *this;
}


hkUint32 hkvTextureTransformationRule::s_ruleTypeIndex = HKV_INVALID_INDEX;

hkvEnumDefinition hkvTextureTransformationRule::s_compressionDefinition(HKV_TEXTURE_COMPRESSION_COUNT, hkvTextureCompressionNames);

hkvEnumDefinition hkvTextureTransformationRule::s_usageDefinition(HKV_TEXTURE_USAGE_COUNT, hkvTextureUsageNames);

CRITICAL_SECTION hkvTextureTransformationRule::s_protect;


/////////////////////////////////////////////////////////////////////////////
// hkvTextureTransformationRule static functions
/////////////////////////////////////////////////////////////////////////////

void hkvTextureTransformationRule::staticInit()
{
  VASSERT(s_ruleTypeIndex == HKV_INVALID_INDEX);

  hkvTransformationRuleTypeInfo ti;
  ti.m_name = "TextureTransformationRule";
  ti.m_createFunc = &createTransformationRule;
  ti.m_supportsAllTypes = false;
  ti.m_supportedAssetTypes.pushBack("Texture");
  ti.m_priority = 100;

  // register at the hkvTransformationRuleTypeManager and store the type index in static variable.
  s_ruleTypeIndex = hkvTransformationRuleTypeManager::getGlobalInstance()->addType(ti);

  InitializeCriticalSection(&s_protect);
}


void hkvTextureTransformationRule::staticDeInit()
{
  if (s_ruleTypeIndex == HKV_INVALID_INDEX)
    return;

  // de-register at the hkvTransformationRuleTypeManager
  hkvTransformationRuleTypeManager::getGlobalInstance()->removeType(s_ruleTypeIndex);
  s_ruleTypeIndex = HKV_INVALID_INDEX;

  DeleteCriticalSection(&s_protect);
}


hkvTransformationRule* hkvTextureTransformationRule::createTransformationRule(hkvTargetPlatform targetPlatform)
{
  hkvTransformationRule* pRule = new hkvTextureTransformationRule(targetPlatform);
  VASSERT(pRule);
  return pRule;
}


/////////////////////////////////////////////////////////////////////////////
// hkvTextureTransformationRule public functions
/////////////////////////////////////////////////////////////////////////////

static hkvTextureCompression compressionDefs[] = {HKV_TEXTURE_COMPRESSION_QUALITY, HKV_TEXTURE_COMPRESSION_NONE};

hkvTextureTransformationRule::hkvTextureTransformationRule(hkvTargetPlatform platform) :
  hkvTransformationRule(platform),
  m_compressionInstance(s_compressionDefinition, sizeof(compressionDefs) / sizeof(hkvTextureCompression), (const hkUint32*) compressionDefs),
  m_removeAlphaChannel(false), m_createMipMaps(true), m_downscaleLevel(0), m_minSize(0), m_maxSize(0)
{
  m_compressionInstance.setByDefinitionId(HKV_TEXTURE_COMPRESSION_QUALITY);
}


hkvTextureTransformationRule::~hkvTextureTransformationRule()
{
}


void hkvTextureTransformationRule::getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  int iFlags = (purpose == hkvProperty::PURPOSE_USER_INTERFACE_READ_ONLY) ? hkvProperty::FLAG_READ_ONLY : 0;

  if (purpose == hkvProperty::PURPOSE_SERIALIZATION)
  {
    properties.push_back(hkvProperty("Compression", m_compressionInstance.getString(), hkvProperty::TYPE_STRING, 0));
    properties.back().setDefaultValue(m_compressionInstance.getStringForAvailableElementsId(0), false);
  }
  else
  {
    properties.push_back(hkvProperty("Compression", m_compressionInstance.getAvailableElementsId(), m_compressionInstance.getAvailableElementStrings(), iFlags, "Use 'uncompressed' for all textures that must not change due to compression, for example font textures and lookup tables. Use 'Quality' for all other textures, as this will use texture compression but with the best possible quality."));
  }

  properties.push_back(hkvProperty("RemoveAlphaChannel", m_removeAlphaChannel, iFlags, "If enabled, the transformed texture will never have an alpha channel. Use this, if you have textures that have an alpha channel in their source data, which is not used at runtime, and you want to get rid of it. Note that this might or might not reduce the size of the transformed file, as this depends on the (platform specific) file format. So this is not a guarantee to save space."));
  properties.push_back(hkvProperty("CreateMipMaps", m_createMipMaps, iFlags, "If enabled, the transformed file will contain mipmaps. Enable this for all standard textures. However for textures that are used for splash-screens, UI, lookup tables and basically everything that will or should never get scaled, you might need to disable this."));
  properties.back().setDefaultValue(true);
  properties.push_back(hkvProperty("DownscaleLevel", m_downscaleLevel, iFlags, "Specifies how many mipmap levels to discard. Zero means the texture is used in full resolution. One means the highest mipmap will be discarded. Two means the two highest mipmaps will be discarded, and so on. Use this to scale down textures for example for devices that do not have enough RAM or processing power, or that do not require the visual fidelity due to smaller screens."));
  properties.push_back(hkvProperty("MinimumSize", m_minSize, iFlags, "Specify the minimum length of the longer edge of the target texture. Set to 0 for no user-defined limit (target device/format limits may still apply)."));
  properties.push_back(hkvProperty("MaximumSize", m_maxSize, iFlags, "Specify the maximum length of the longer edge of the target texture. Set to 0 for no user-defined limit (target device/format limits may still apply)."));
}


void hkvTextureTransformationRule::setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int iStackIndex, hkvProperty::Purpose purpose)
{
  int iStackSize = (path.getSize() - iStackIndex);

  if (iStackSize == 0)
  {
    if (hkvStringHelper::safeCompare(prop.getName(), "Compression") == 0)
    {
      if (purpose == hkvProperty::PURPOSE_SERIALIZATION)
      {
        if (m_compressionInstance.setByString(prop.getString()) != HK_SUCCESS)
        {
          m_compressionInstance.setByAvailableElementsId(0);
        }
      }
      else
      {
        m_compressionInstance.setByAvailableElementsId(prop.getEnumValue());
      }
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "RemoveAlphaChannel") == 0)
    {
      m_removeAlphaChannel = prop.getBool();
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "CreateMipMaps") == 0)
    {
      m_createMipMaps = prop.getBool();
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "DownscaleLevel") == 0)
    {
      m_downscaleLevel = prop.getUint();
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "MinimumSize") == 0)
    {
      m_minSize = prop.getUint();
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "MaximumSize") == 0)
    {
      m_maxSize = prop.getUint();
    }
  }
}


unsigned int hkvTextureTransformationRule::getTypeIndex() const
{
  return s_ruleTypeIndex;
}


const char* hkvTextureTransformationRule::getTypeName() const
{
  return "TextureTransformationRule";
}


bool hkvTextureTransformationRule::executeTransformation(const hkvTransformationInput& input, hkvTransformationOutput& output) const
{
  Context context(input, output);

  bool ok = !context.m_canceled && (transferSourceProperties(context) == HK_SUCCESS);
  ok = ok && !context.m_canceled && (examineSourceFile(context) == HK_SUCCESS);
  ok = ok && !context.m_canceled && (determineOutputs(context) == HK_SUCCESS);
  ok = ok && !context.m_canceled && (makeIntermediateBaseName(context) == HK_SUCCESS);
  ok = ok && !context.m_canceled && (copySourceFileToTemp(context) == HK_SUCCESS);

  int numVariants = context.m_outputVariants.getSize();
  for (int variantIdx = 0; ok && (variantIdx < numVariants); ++variantIdx)
  {
    const hkvTextureVariant& variant = context.m_outputVariants[variantIdx];
    ok = ok && !context.m_canceled && (prepareTransformationSettings(context, variant) == HK_SUCCESS);

    if (ok && hkvFileHelper::fileExists(context.m_targetFile))
    {
      continue;
    }

    ok = ok && !context.m_canceled && (runConversion(context) == HK_SUCCESS);
    ok = ok && !context.m_canceled && (moveTempFileToTarget(context) == HK_SUCCESS);

    if (ok)
    {
      notifyOutputWritten(context, variant.m_variantKey.cString());
    }
  }

  cleanUpTempFiles(context);

  if (ok)
  {
    context.m_output.m_messages.pushBack(hkvTransformationMessage(
      HKV_MESSAGE_SEVERITY_INFO, "Texture transformed successfully."));
  }
  else if (context.m_canceled)
  {
    deleteTargetFile(context);
    context.m_output.m_messages.pushBack(hkvTransformationMessage(
      HKV_MESSAGE_SEVERITY_WARNING, "Transformation of texture was canceled."));
  }
  else
  {
    context.m_output.m_messages.pushBack(hkvTransformationMessage(
      HKV_MESSAGE_SEVERITY_ERROR, "Transformation of texture failed!"));
  }

  return ok;
}


bool hkvTextureTransformationRule::queryOutputFileSpecs(const hkvTransformationInput& input, hkvTransformationOutput& output) const
{
  Context context(input, output);

  bool ok = (transferSourceProperties(context) == HK_SUCCESS);
  ok = ok && (determineOutputs(context) == HK_SUCCESS);
  if (!ok)
  {
    return HK_FAILURE;
  }

  int numVariants = context.m_outputVariants.getSize();
  for (int variantIdx = 0; variantIdx < numVariants; ++variantIdx)
  {
    const hkvTextureVariant& variant = context.m_outputVariants[variantIdx];
    if (variant.m_isEditorPreview && !input.m_includeEditorPreview)
    {
      continue;
    }

    const char* extension = hkvTextureFileFormatExtensions[variant.m_fileFormat];

    hkStringBuf fileNameAddition(variant.m_filenameAddition, ".", extension);

    hkvTransformationOutputFileSpec outSpec;
    outSpec.m_variantKey = variant.m_variantKey;
    hkStringBuf fileNameBuf;
    outSpec.m_fileName = makeTargetFileName(context, fileNameAddition, fileNameBuf);
    output.m_outputFileSpecs.pushBack(outSpec);
  }

  return true;
}


hkResult hkvTextureTransformationRule::determineOutputs(Context& context) const
{
  switch (getPlatform())
  {
  case HKV_TARGET_PLATFORM_DX9:
  case HKV_TARGET_PLATFORM_DX11:
  case HKV_TARGET_PLATFORM_XBOX360:
  case HKV_TARGET_PLATFORM_PS3:
  case HKV_TARGET_PLATFORM_PSVITA:
    {
      return determineOutputsDxt(context, false);
    }
  case HKV_TARGET_PLATFORM_ANDROID:
    {
      return determineOutputsAndroid(context);
    }
  case HKV_TARGET_PLATFORM_IOS:
    {
      return determineOutputsIos(context);
    }
  case HKV_TARGET_PLATFORM_WIIU:
    {
      return determineOutputsWiiU(context);
    }
  }

  VASSERT_MSG(FALSE, "Unhandled platform!");
  return HK_FAILURE;
}


hkResult hkvTextureTransformationRule::determineOutputsAndroid(Context& context) const
{
  hkvTextureCompression compression = (hkvTextureCompression)m_compressionInstance.getDefinitionId();

  switch (compression)
  {
  case HKV_TEXTURE_COMPRESSION_NONE:
    {
      if (context.m_sourceHasAlpha && !m_removeAlphaChannel)
      {
      context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_DEFAULT, 
        HKV_TEXTURE_DATA_FORMAT_A8R8G8B8, HKV_TEXTURE_FILE_FORMAT_RGBA, "", false));
      context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_PVR,
        HKV_TEXTURE_DATA_FORMAT_A8R8G8B8, HKV_TEXTURE_FILE_FORMAT_RGBA, "", false));
      }
      else
      {
        context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_DEFAULT, 
          HKV_TEXTURE_DATA_FORMAT_X8R8G8B8, HKV_TEXTURE_FILE_FORMAT_RGBA, "", false));
        context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_PVR,
          HKV_TEXTURE_DATA_FORMAT_X8R8G8B8, HKV_TEXTURE_FILE_FORMAT_RGBA, "", false));
      }
      break;
    }
  case HKV_TEXTURE_COMPRESSION_QUALITY:
    {
      if (context.m_sourceHasAlpha && !m_removeAlphaChannel)
      {
        context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_DEFAULT, 
          HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL, HKV_TEXTURE_FILE_FORMAT_RGBA, "", false));
        context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_PVR,
          HKV_TEXTURE_DATA_FORMAT_PVRTC4, HKV_TEXTURE_FILE_FORMAT_PVR, "", false));
      }
      else
      {
        if (context.m_sourceUsage == HKV_TEXTURE_USAGE_NORMAL_MAP)
        {
          context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_DEFAULT,
            HKV_TEXTURE_DATA_FORMAT_R5G6B5, HKV_TEXTURE_FILE_FORMAT_RGBA, "", false));
        }
        else
        {
          context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_DEFAULT,
            HKV_TEXTURE_DATA_FORMAT_ETC1, HKV_TEXTURE_FILE_FORMAT_ETC, "", false));
        }

        context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_PVR,
          HKV_TEXTURE_DATA_FORMAT_PVRTC4, HKV_TEXTURE_FILE_FORMAT_PVR, "", false));
      }
      break;
    }
  //case HKV_TEXTURE_COMPRESSION_SIZE:
  //  {
  //    if (context.m_sourceHasAlpha && !m_removeAlphaChannel)
  //    {
  //      context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_DEFAULT, 
  //        HKV_TEXTURE_DATA_FORMAT_A4R4G4B4, HKV_TEXTURE_FILE_FORMAT_RGBA, "", false));
  //      context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_PVR,
  //        HKV_TEXTURE_DATA_FORMAT_PVRTC2, HKV_TEXTURE_FILE_FORMAT_PVR, "", false));
  //    }
  //    else
  //    {
  //      context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_DEFAULT, 
  //        HKV_TEXTURE_DATA_FORMAT_ETC1, HKV_TEXTURE_FILE_FORMAT_ETC, "", false));
  //      context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_ANDROID_PVR,
  //        HKV_TEXTURE_DATA_FORMAT_PVRTC2, HKV_TEXTURE_FILE_FORMAT_PVR, "", false));
  //    }
  //    break;
  //  }
  default:
    {
      VASSERT_MSG(FALSE, "Missing compression case!");
      return HK_FAILURE;
    }
  }

  // Add a DXT/RAW variant as editor preview
  determineOutputsDxt(context, true);

  return HK_SUCCESS;
}


hkResult hkvTextureTransformationRule::determineOutputsDxt(Context& context, bool isEditorPreview) const
{
  hkvTextureCompression compression = (hkvTextureCompression)m_compressionInstance.getDefinitionId();

  hkvTextureDataFormat dataFormat;
  switch (compression)
  {
  case HKV_TEXTURE_COMPRESSION_NONE:
    {
      dataFormat = (context.m_sourceHasAlpha && !m_removeAlphaChannel) ? HKV_TEXTURE_DATA_FORMAT_A8R8G8B8 : HKV_TEXTURE_DATA_FORMAT_X8R8G8B8;
      break;
    }
  case HKV_TEXTURE_COMPRESSION_QUALITY:
    {
      dataFormat = (context.m_sourceHasAlpha && !m_removeAlphaChannel) ? HKV_TEXTURE_DATA_FORMAT_DXT5 : HKV_TEXTURE_DATA_FORMAT_DXT1;
      break;
    }
  //case HKV_TEXTURE_COMPRESSION_SIZE:
  //  {
  //    dataFormat = (context.m_sourceHasAlpha && !m_removeAlphaChannel) ? HKV_TEXTURE_DATA_FORMAT_DXT3 : HKV_TEXTURE_DATA_FORMAT_DXT1;
  //    break;
  //  }
  default:
    {
      VASSERT_MSG(FALSE, "Missing compression case!");
      return HK_FAILURE;
    }
  }

  // If this is for editor preview, check if we already have a format that we can use. If this is
  // the case, just create another variant pointing to the same file (the actual target file won't
  // be created twice then).
  if (isEditorPreview)
  {
    for (hkArray<hkvTextureVariant>::iterator iter = context.m_outputVariants.begin(); iter != context.m_outputVariants.end(); ++iter)
    {
      if ((iter->m_dataFormat == dataFormat) && (iter->m_fileFormat == HKV_TEXTURE_FILE_FORMAT_DDS))
      {
        context.m_outputVariants.pushBack(hkvTextureVariant(VARIANT_VFORGE, 
          dataFormat, HKV_TEXTURE_FILE_FORMAT_DDS, "", isEditorPreview));
        return HK_SUCCESS;
      }
    }
  }

  // If we get here, it's either not for editor preview, or we didn't find a suitable
  // transformation output in those already queued. Add a new entry with the necessary
  // variant name and filename addition (empty for regular output, "v" for editor preview).
  const char* variantName = isEditorPreview ? VARIANT_VFORGE : "";
  const char* nameAddition = isEditorPreview ? "v" : "";
  context.m_outputVariants.pushBack(hkvTextureVariant(variantName, 
    dataFormat, HKV_TEXTURE_FILE_FORMAT_DDS, nameAddition, isEditorPreview));

  return HK_SUCCESS;
}


hkResult hkvTextureTransformationRule::determineOutputsIos(Context& context) const
{
  hkvTextureCompression compression = (hkvTextureCompression)m_compressionInstance.getDefinitionId();

  hkvTextureDataFormat dataFormat;
  hkvTextureFileFormat fileFormat;
  switch (compression)
  {
  case HKV_TEXTURE_COMPRESSION_NONE:
    {
      dataFormat = (context.m_sourceHasAlpha && !m_removeAlphaChannel) ? HKV_TEXTURE_DATA_FORMAT_A8R8G8B8 : HKV_TEXTURE_DATA_FORMAT_X8R8G8B8;
      fileFormat = HKV_TEXTURE_FILE_FORMAT_RGBA;
      break;
    }
  case HKV_TEXTURE_COMPRESSION_QUALITY:
    {
      dataFormat = HKV_TEXTURE_DATA_FORMAT_PVRTC4;
      fileFormat = HKV_TEXTURE_FILE_FORMAT_PVR;
      break;
    }
  //case HKV_TEXTURE_COMPRESSION_SIZE:
  //  {
  //    dataFormat = HKV_TEXTURE_DATA_FORMAT_PVRTC2;
  //    fileFormat = HKV_TEXTURE_FILE_FORMAT_PVR;
  //    break;
  //  }
  default:
    {
      VASSERT_MSG(FALSE, "Missing compression case!");
      return HK_FAILURE;
    }
  }

  context.m_outputVariants.pushBack(hkvTextureVariant("", dataFormat, fileFormat, "", false));

  // Add a DXT/RAW variant as editor preview
  determineOutputsDxt(context, true);

  return HK_SUCCESS;
}


hkResult hkvTextureTransformationRule::determineOutputsWiiU(Context& context) const
{
  hkvTextureCompression compression = (hkvTextureCompression)m_compressionInstance.getDefinitionId();

  hkvTextureDataFormat dataFormat;
  switch (compression)
  {
  case HKV_TEXTURE_COMPRESSION_NONE:
    {
      dataFormat = (context.m_sourceHasAlpha && !m_removeAlphaChannel) ? HKV_TEXTURE_DATA_FORMAT_A8R8G8B8 : HKV_TEXTURE_DATA_FORMAT_X8R8G8B8;
      break;
    }
  case HKV_TEXTURE_COMPRESSION_QUALITY:
    {
      dataFormat = (context.m_sourceHasAlpha && !m_removeAlphaChannel) ? HKV_TEXTURE_DATA_FORMAT_DXT5 : HKV_TEXTURE_DATA_FORMAT_DXT1;
      break;
    }
  //case HKV_TEXTURE_COMPRESSION_SIZE:
  //  {
  //    dataFormat = (context.m_sourceHasAlpha && !m_removeAlphaChannel) ? HKV_TEXTURE_DATA_FORMAT_DXT3 : HKV_TEXTURE_DATA_FORMAT_DXT1;
  //    break;
  //  }
  default:
    {
      VASSERT_MSG(FALSE, "Missing compression case!");
      return HK_FAILURE;
    }
  }

  context.m_outputVariants.pushBack(hkvTextureVariant("", dataFormat, HKV_TEXTURE_FILE_FORMAT_GTX, "", false));

  // Add a DXT/RAW variant as editor preview
  determineOutputsDxt(context, true);

  return HK_SUCCESS;
}


hkResult hkvTextureTransformationRule::examineSourceFile(Context& context) const
{
  hkvImageFileProperties fileProperties;
  if (fileProperties.examineFile(context.m_input.m_sourceFile) != HK_SUCCESS)
  {
    return HK_FAILURE;
  }

  context.m_sourceFormat = fileProperties.getImageFileFormat();
  context.m_sourceHasAlpha = fileProperties.hasAlpha();
  context.m_sourceWidth = fileProperties.getWidth();
  context.m_sourceHeight = fileProperties.getHeight();

  return HK_SUCCESS;
}


hkResult hkvTextureTransformationRule::prepareTransformationSettings(
  Context& context, const hkvTextureVariant& variant) const
{
  // Construct the settings for the final output.
  hkvTextureTransformationSettings finalSettings(getPlatform());

  finalSettings.setSourceProperties(context.m_sourceUsage, context.m_sourceSrgb,
    context.m_sourceHasAlpha, context.m_sourceWidth, context.m_sourceHeight);

  finalSettings.setTargetFormat(variant.m_dataFormat, variant.m_fileFormat);
  finalSettings.setDiscardAlpha(m_removeAlphaChannel);
  finalSettings.setCreateMipMaps(m_createMipMaps);
  finalSettings.setDownscaleLevel(m_downscaleLevel);
  finalSettings.setUserMinSize(m_minSize);
  finalSettings.setUserMaxSize(m_maxSize);

  if (variant.m_isEditorPreview)
  {
    finalSettings.setIgnoreTargetFormatRestrictions(true);
  }

  if (finalSettings.validate(context.m_output.m_messages) != HK_SUCCESS)
  {
    return HK_FAILURE;
  }

  // Determine all steps that need to be executed to arrive at the target format
  context.m_transformationSteps.clear();

  // First, determine the final step that leads us to our output format.
  TransformationStepInfo finalStep;
  switch (finalSettings.getTargetFileFormat())
  {
  case HKV_TEXTURE_FILE_FORMAT_DDS:
  case HKV_TEXTURE_FILE_FORMAT_RGBA:
    {
      switch (finalSettings.getTargetDataFormat())
      {
      case HKV_TEXTURE_DATA_FORMAT_DXT1:
      case HKV_TEXTURE_DATA_FORMAT_DXT3:
      case HKV_TEXTURE_DATA_FORMAT_DXT5:
      case HKV_TEXTURE_DATA_FORMAT_A1R5G5B5:
      case HKV_TEXTURE_DATA_FORMAT_R5G6B5:
      case HKV_TEXTURE_DATA_FORMAT_A8R8G8B8:
      case HKV_TEXTURE_DATA_FORMAT_X8R8G8B8:
        {
          finalStep = TransformationStepInfo(STEP_TYPE_TEXCONV, "dds", finalSettings);
          break;
        }
      case HKV_TEXTURE_DATA_FORMAT_A4R4G4B4:
      case HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL:
      case HKV_TEXTURE_DATA_FORMAT_R8G8B8:
        {
          finalStep = TransformationStepInfo(STEP_TYPE_IMAGE_TO_DDS, "dds", finalSettings);
          break;
        }
      default:
        {
          VASSERT_MSG(false, "Forgotten uncompressed DDS format?");
          finalStep = TransformationStepInfo(STEP_TYPE_PVRTEXTOOL, "dds", finalSettings);
          break;
        }
      }
      break;
    }
  case HKV_TEXTURE_FILE_FORMAT_ETC:
  case HKV_TEXTURE_FILE_FORMAT_PVR:
    {
      finalStep = TransformationStepInfo(STEP_TYPE_PVRTEXTOOL, "pvr", finalSettings);
      break;
    }
  case HKV_TEXTURE_FILE_FORMAT_GTX:
    {
      finalStep = TransformationStepInfo(STEP_TYPE_WIIU_TEXCONV2, "gtx", finalSettings);
      break;
    }
  default:
    {
      VASSERT_MSG(FALSE, "Missing case for texture file format");
      return HK_FAILURE;
    }
  }

  // In ALL cases, add a step first that converts whatever input format we have to A8R8G8B8 DDS without
  // mipmaps. Yes, even texconv will fail to process a file under certain conditions even if can read the
  // input format and write the output format without problems.
  hkvTextureTransformationSettings preparationSettings(HKV_TARGET_PLATFORM_ANY);
  preparationSettings.setSourceProperties(context.m_sourceUsage, context.m_sourceSrgb,
    context.m_sourceHasAlpha, context.m_sourceWidth, context.m_sourceHeight);
  preparationSettings.setExplicitTargetSize(context.m_sourceWidth, context.m_sourceHeight);
  preparationSettings.setTargetFormat(HKV_TEXTURE_DATA_FORMAT_A8R8G8B8, HKV_TEXTURE_FILE_FORMAT_DDS);
  preparationSettings.setCreateMipMaps(false);
  preparationSettings.setDiscardAlpha(false);
  preparationSettings.setDownscaleLevel(0);
  context.m_transformationSteps.pushBack(TransformationStepInfo(STEP_TYPE_TEXCONV, "dds", preparationSettings));

  // In case of WiiU, the Cafe TexConv2 tool will pass through the input format. So use TexConv again to
  // get to the target format (without mipmaps, though, as the Cafe tool will take care of it)
  if (finalStep.getStepType() == STEP_TYPE_WIIU_TEXCONV2)
  {
    hkvTextureTransformationSettings cafeSettings(HKV_TARGET_PLATFORM_ANY);
    cafeSettings.assignFrom(finalStep.getSettings(), true);
    cafeSettings.setTargetFormat(cafeSettings.getTargetDataFormat(), HKV_TEXTURE_FILE_FORMAT_DDS);
    if (cafeSettings.getCreateMipMaps())
    {
      cafeSettings.setCreateMipMaps(false); // Not needed here, as the final step needs to creates them
      cafeSettings.setApplyMipMapRestrictions(true); // However, take restrictions into account already
    }
    context.m_transformationSteps.pushBack(TransformationStepInfo(STEP_TYPE_TEXCONV, "dds", cafeSettings));
  }

  // If the final step is our internal functionality to write uncompressed DDS files, all the other 
  // operations (Downscaling, MipMaps) still need to be done. Use texconv for that.
  if (finalStep.getStepType() == STEP_TYPE_IMAGE_TO_DDS)
  {
    hkvTextureTransformationSettings texconvSettings(HKV_TARGET_PLATFORM_ANY);
    texconvSettings.assignFrom(finalStep.getSettings(), true);
    texconvSettings.setTargetFormat(HKV_TEXTURE_DATA_FORMAT_A8R8G8B8, HKV_TEXTURE_FILE_FORMAT_DDS);
    context.m_transformationSteps.pushBack(TransformationStepInfo(STEP_TYPE_TEXCONV, "dds", texconvSettings));
  }

  context.m_transformationSteps.pushBack(finalStep);

  // Determine the name of the output file
  const char* extension = hkvTextureFileFormatExtensions[variant.m_fileFormat];
  hkStringBuf nameAdditionBuf(variant.m_filenameAddition, ".", extension);

  hkStringBuf filePathBuf;
  context.m_targetFile = makeAbsoluteTargetFilePath(context, nameAdditionBuf, filePathBuf);

  return HK_SUCCESS;
}


hkResult hkvTextureTransformationRule::runConversion(Context& context) const
{
  context.m_stepSourceFile = context.m_tempOriginalFile;

  int numSteps = context.m_transformationSteps.getSize();
  for (int stepIdx = 0; stepIdx < numSteps; ++stepIdx)
  {
    TransformationStepInfo& stepInfo = context.m_transformationSteps[stepIdx];

    // Check if this step's transformation settings are valid. If not, try to validate them.
    hkvTextureTransformationSettings& settings = stepInfo.getSettings();
    if (!settings.isValid() && (settings.validate(context.m_output.m_messages) != HK_SUCCESS))
    {
      context.m_output.m_messages.pushBack(hkvTransformationMessage(
        HKV_MESSAGE_SEVERITY_ERROR, "Failed to validate transformation settings of a transformation step!"));
      return HK_FAILURE;
    }

    // If this is not the first step, take the output of the previous step as the
    // input of this step
    if (stepIdx > 0)
    {
      context.m_stepSourceFile = context.m_stepTargetFile;
    }

    // The new output file name is a generated base name with the step-specific extension
    hkStringBuf nameBuf;
    context.m_stepTargetFile = makeIntermediateFileName(context, stepInfo.getTargetExtension(), nameBuf);

    hkRefPtr<hkvFileTransformationStep> step;
    switch (stepInfo.getStepType())
    {
    case STEP_TYPE_WIIU_TEXCONV2:
      {
        step = hkRefNew<hkvFileTransformationStep>(
          new hkvExternalToolWiiUTexConv2(stepInfo.getSettings(), context.m_stepSourceFile, context.m_stepTargetFile));
        break;
      }
    case STEP_TYPE_IMAGE_TO_DDS:
      {
        step = hkRefPtr<hkvFileTransformationStep>(
          new hkvTransformationStepImageToDds(stepInfo.getSettings(), context.m_stepSourceFile, context.m_stepTargetFile));
        break;
      }
    case STEP_TYPE_NVDXT:
      {
        step = hkRefNew<hkvFileTransformationStep>(
          new hkvExternalToolNvDxt(stepInfo.getSettings(), context.m_stepSourceFile, context.m_stepTargetFile));
        break;
      }
    case STEP_TYPE_PVRTEXTOOL:
      {
        step = hkRefNew<hkvFileTransformationStep>(
          new hkvExternalToolPvrTexTool(stepInfo.getSettings(), context.m_stepSourceFile, context.m_stepTargetFile));
        break;
      }
    case STEP_TYPE_TEXCONV:
      {
        step = hkRefNew<hkvFileTransformationStep>(
          new hkvExternalToolTexConv(stepInfo.getSettings(), context.m_stepSourceFile, context.m_stepTargetFile));
        break;
      }
    default:
      {
        VASSERT_MSG(FALSE, "Missing tool case!");
        return HK_FAILURE;
      }
    }

    // Some tools want to create output files with a name of their choosing. Now that we
    // know the concrete step to execute, query it for its output file name.
    context.m_stepTargetFile = step->getTargetFile();
    context.m_tempFileNames.pushBack(context.m_stepTargetFile);

    {
      hkvCriticalSectionLock lock(s_protect);
      context.m_currentTransformationStep = step;
    }

    hkResult stepResult = step->run();

    {
      hkvCriticalSectionLock lock(s_protect);
      context.m_currentTransformationStep = NULL;
    }

    context.m_output.m_messages.append(step->getMessages());

    if (stepResult != HK_SUCCESS)
    {
      if (context.m_canceled)
      {
        context.m_output.m_messages.pushBack(hkvTransformationMessage(
          HKV_MESSAGE_SEVERITY_WARNING, "Transformation step was canceled."));
      }
      else
      {
        context.m_output.m_messages.pushBack(hkvTransformationMessage(
          HKV_MESSAGE_SEVERITY_ERROR, "Transformation step failed!"));
      }
      return HK_FAILURE;
    }
  }

  return HK_SUCCESS;
}


hkResult hkvTextureTransformationRule::transferSourceProperties(Context& context) const
{
  if (!context.m_input.m_sourceProperties)
  {
    return HK_FAILURE;
  }

  hkvPropertyList sourceProperties;
  context.m_input.m_sourceProperties->getProperties(sourceProperties, hkvProperty::PURPOSE_SERIALIZATION);

  size_t numSourceProperties = sourceProperties.size();
  for (size_t sourcePropertyIdx = 0; sourcePropertyIdx < numSourceProperties; ++sourcePropertyIdx)
  {
    hkvProperty& prop = sourceProperties[sourcePropertyIdx];
    if (hkvStringHelper::safeCompare(prop.getName(), "FileFormat") == 0)
    {
      hkUint32 format = HKV_IMAGE_FILE_FORMAT_INVALID;
      if (getImageFileFormatDefinition().stringToId(prop.getString(), format) == HK_SUCCESS)
      {
        context.m_sourceFormat = (hkvImageFileFormat)format;
      }
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "HasAlpha") == 0)
    {
      context.m_sourceHasAlpha = prop.getBool();
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "Width") == 0)
    {
      context.m_sourceWidth = prop.getUint();
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "Height") == 0)
    {
      context.m_sourceHeight = prop.getUint();
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "Usage") == 0)
    {
      hkUint32 usage = HKV_TEXTURE_USAGE_DIFFUSE_MAP;
      if (s_usageDefinition.stringToId(prop.getString(), usage) == HK_SUCCESS)
      {
        context.m_sourceUsage = (hkvTextureUsage)usage;
      }
    }
    else if (hkvStringHelper::safeCompare(prop.getName(), "sRGB") == 0)
    {
      context.m_sourceSrgb = prop.getBool();
    }
  }

  return HK_SUCCESS;
}


hkvTextureTransformationRule::Context::Context(
  const hkvTransformationInput& input, hkvTransformationOutput& output)
: hkvTransformationRule::Context(input, output),
  m_sourceFormat(HKV_IMAGE_FILE_FORMAT_INVALID), m_sourceHasAlpha(false), 
  m_sourceWidth(0), m_sourceHeight(0), m_sourceUsage(HKV_TEXTURE_USAGE_DIFFUSE_MAP),
  m_canceled(false)
{
  if (m_input.m_controlHost != NULL)
  {
    m_input.m_controlHost->setTransformationControl(this);
  }
}


hkvTextureTransformationRule::Context::~Context()
{
  if (m_input.m_controlHost != NULL)
  {
    m_input.m_controlHost->setTransformationControl(NULL);
  }
}


void hkvTextureTransformationRule::Context::cancelTransformation()
{
  hkvCriticalSectionLock lock(s_protect);
  m_canceled = true;
  if (m_currentTransformationStep != NULL)
  {
    m_currentTransformationStep->cancel();
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
