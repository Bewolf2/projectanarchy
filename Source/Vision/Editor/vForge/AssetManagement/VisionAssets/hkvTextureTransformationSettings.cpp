/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationSettings.hpp>

hkvTextureTransformationSettings::hkvTextureTransformationSettings(hkvTargetPlatform platform)
: m_platform(platform), m_sourceHasAlpha(false), m_sourceWidth(0), m_sourceHeight(0),
  m_sourceUsage(HKV_TEXTURE_USAGE_DIFFUSE_MAP), m_sourceSrgb(false),
  m_targetDataFormat(HKV_TEXTURE_DATA_FORMAT_A8R8G8B8), m_targetFileFormat(HKV_TEXTURE_FILE_FORMAT_DDS),
  m_discardAlpha(false), m_createMipMaps(false), m_downscaleLevel(0),
  m_userMinSize(0), m_userMaxSize(0),
  m_explicitTargetSize(false), m_explicitWidth(0), m_explicitHeight(0),
  m_applyMipMapRestrictions(false), m_ignoreTargetFormatRestrictions(false),
  m_valid(false),
  m_targetHasAlpha(false), m_targetWidth(0), m_targetHeight(0)
{
}


void hkvTextureTransformationSettings::assignFrom(const hkvTextureTransformationSettings& other, bool explicitSize)
{
  m_sourceHasAlpha = other.m_sourceHasAlpha;
  m_sourceWidth = other.m_sourceWidth;
  m_sourceHeight = other.m_sourceHeight;
  m_sourceUsage = other.m_sourceUsage;
  m_sourceSrgb = other.m_sourceSrgb;

  m_targetDataFormat = other.m_targetDataFormat;
  m_targetFileFormat = other.m_targetFileFormat;
  m_discardAlpha = other.m_discardAlpha;
  m_createMipMaps = other.m_createMipMaps;
  m_downscaleLevel = other.m_downscaleLevel;
  m_userMinSize = other.m_userMinSize;
  m_userMaxSize = other.m_userMaxSize;

  if (explicitSize)
  {
    setExplicitTargetSize(other.getTargetWidth(), other.getTargetHeight());
  }

  m_valid = false;
}


void hkvTextureTransformationSettings::setSourceProperties(hkvTextureUsage usage, bool srgb, bool hasAlpha, hkUint32 width, hkUint32 height)
{
  m_sourceUsage = usage;
  m_sourceSrgb = srgb;
  m_sourceHasAlpha = hasAlpha;
  m_sourceWidth = width;
  m_sourceHeight = height;
  m_valid = false;
}


void hkvTextureTransformationSettings::setTargetFormat(hkvTextureDataFormat dataFormat, hkvTextureFileFormat fileFormat)
{
  if (m_targetDataFormat < HKV_TEXTURE_DATA_FORMAT_COUNT)
  {
    m_targetDataFormat = dataFormat;
  }
  if (m_targetFileFormat < HKV_TEXTURE_FILE_FORMAT_COUNT)
  {
    m_targetFileFormat = fileFormat;
  }
  m_valid = false;
}


void hkvTextureTransformationSettings::setDiscardAlpha(bool discard)
{
  if (discard != m_discardAlpha)
  {
    m_discardAlpha = discard;
    m_valid = false;
  }
}


void hkvTextureTransformationSettings::setCreateMipMaps(bool create)
{
  if (create != m_createMipMaps)
  {
    m_createMipMaps = create;
    m_valid = false;
  }
}


void hkvTextureTransformationSettings::setDownscaleLevel(hkUint32 level)
{
  if (level != m_downscaleLevel)
  {
    m_downscaleLevel = level;
    m_valid = false;
  }
}


void hkvTextureTransformationSettings::setUserMaxSize(hkUint32 size)
{
  if (size != m_userMaxSize)
  {
    m_userMaxSize = size;
    m_valid = false;
  }
}


void hkvTextureTransformationSettings::setUserMinSize(hkUint32 size)
{
  if (size != m_userMinSize)
  {
    m_userMinSize = size;
    m_valid = false;
  }
}


void hkvTextureTransformationSettings::setExplicitTargetSize(hkUint32 width, hkUint32 height)
{
  m_explicitTargetSize = true;
  m_explicitWidth = width;
  m_explicitHeight = height;
  m_valid = false;
}


void hkvTextureTransformationSettings::clearExplicitTargetSize()
{
  m_explicitTargetSize = false;
  m_explicitWidth = 0;
  m_explicitHeight = 0;
  m_valid = false;
}


void hkvTextureTransformationSettings::setApplyMipMapRestrictions(bool apply)
{
  if (apply != m_applyMipMapRestrictions)
  {
    m_applyMipMapRestrictions = apply;
    m_valid = false;
  }
}


void hkvTextureTransformationSettings::setIgnoreTargetFormatRestrictions(bool ignore)
{
  if (ignore != m_ignoreTargetFormatRestrictions)
  {
    m_ignoreTargetFormatRestrictions = ignore;
    m_valid = false;
  }
}


hkResult hkvTextureTransformationSettings::validate(hkArray<hkvTransformationMessage>& out_messages)
{
  if (m_valid)
  {
    return HK_SUCCESS;
  }

  m_validationMinSize = 0;
  m_validationWarnSize = 0;
  m_validationMaxSize = 0;
  m_validationNeedsPowerOfTwo = false;
  m_validationNeedsSquare = false;
  m_validationNeedsMultipleOf = 1;

  bool ok = true;
  if (!m_ignoreTargetFormatRestrictions)
  {
    ok &= checkFormatCompatibility(out_messages);
  }
  ok &= determineRestrictions(out_messages);
  ok &= checkDimensions(out_messages);
  ok &= checkAlpha(out_messages);

  if (!ok)
  {
    return HK_FAILURE;
  }

  m_valid = true;
  return HK_SUCCESS;
}


hkUint32 hkvTextureTransformationSettings::adjustToNearestPowerOfTwo(hkUint32 i)
{
  hkUint32 nextPo2 = hkNextPowerOf2(i);
  hkUint32 prevPo2 = nextPo2 >> 1;
  if (prevPo2 == 0 || i == nextPo2)
  {
    return nextPo2;
  }

  hkUint32 dPrev = i - prevPo2;
  hkUint32 dNext = nextPo2 - i;

  return (dPrev < dNext) ? prevPo2 : nextPo2;
}


bool hkvTextureTransformationSettings::checkFormatCompatibility(
  hkArray<hkvTransformationMessage>& out_messages)
{
  // Check if the file and data formats are compatible
  if (!hkvTextureFileToDataFormatMapping::getInstance().isMapped(m_targetFileFormat, m_targetDataFormat))
  {
    hkStringBuf msg;
    msg.printf("The target file format (%s) is not compatible with the target data format (%s)",
      hkvTextureFileFormatNames[m_targetFileFormat], hkvTextureDataFormatNames[m_targetDataFormat]);
    out_messages.pushBack(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_ERROR, msg));
  }

  // Check if the platform can handle the target data format
  if (m_platform != HKV_TARGET_PLATFORM_ANY)
  {
    if (!hkvPlatformToTextureDataFormatMapping::getInstance().isMapped(m_platform, m_targetDataFormat))
    {
      const char* platformName = "";
      hkvGetTargetPlatformDefinition().idToString(m_platform, platformName);
      
      hkStringBuf msg;
      msg.printf("The target data format (%s) is not compatible with the target platform (%s)",
        hkvTextureDataFormatNames[m_targetDataFormat], platformName);
      out_messages.pushBack(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_ERROR, msg));
    }
  }

  // Check if the platform can handle the target file format
  if (m_platform != HKV_TARGET_PLATFORM_ANY)
  {
    if (!hkvPlatformToTextureFileFormatMapping::getInstance().isMapped(m_platform, m_targetFileFormat))
    {
      const char* platformName = "";
      hkvGetTargetPlatformDefinition().idToString(m_platform, platformName);

      hkStringBuf msg;
      msg.printf("The target file format (%s) is not compatible with the target platform (%s)",
        hkvTextureFileFormatNames[m_targetFileFormat], platformName);
      out_messages.pushBack(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_ERROR, msg));
    }
  }

  return true;
}


bool hkvTextureTransformationSettings::determineRestrictions(
  hkArray<hkvTransformationMessage>& out_messages)
{
  switch (m_platform)
  {
  case HKV_TARGET_PLATFORM_ANDROID:
    {
      if (!determineRestrictionsAndroid(out_messages))
      {
        return false;
      }
      break;
    }
  case HKV_TARGET_PLATFORM_IOS:
    {
      if (!determineRestrictionsIos(out_messages))
      {
        return false;
      }
      break;
    }
  case HKV_TARGET_PLATFORM_PSVITA:
    {
      if (!determineRestrictionsPsVita(out_messages))
      {
        return false;
      }
      break;
    }
  }

  // If MipMaps are to be generated, start with Po2 dimensions
  if (m_createMipMaps || m_applyMipMapRestrictions)
  {
    m_validationNeedsPowerOfTwo = true;
  }

  // Apply common restrictions for DXT formats (needs multiple of 4 because of compression block size)
  if (m_targetDataFormat == HKV_TEXTURE_DATA_FORMAT_DXT1 || m_targetDataFormat == HKV_TEXTURE_DATA_FORMAT_DXT3 || m_targetDataFormat == HKV_TEXTURE_DATA_FORMAT_DXT5)
  {
    m_validationMinSize = hkvMath::Max(m_validationMinSize, 4u);
    if (!m_validationNeedsPowerOfTwo)
      m_validationNeedsMultipleOf = 4;
  }

  return true;
}


bool hkvTextureTransformationSettings::determineRestrictionsAndroid(
  hkArray<hkvTransformationMessage>& out_messages)
{
  switch (m_targetDataFormat)
  {
  case HKV_TEXTURE_DATA_FORMAT_ETC1:
    {
      m_validationMinSize = 4;
      m_validationNeedsMultipleOf = 4;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_PVRTC2:
    {
      m_validationMinSize = 16;
      m_validationNeedsPowerOfTwo = true;
      m_validationNeedsSquare = true;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_PVRTC4:
    {
      m_validationMinSize = 8;
      m_validationNeedsPowerOfTwo = true;
      m_validationNeedsSquare = true;
      break;
    }
  }

  m_validationWarnSize = 1024;

  return true;
}


bool hkvTextureTransformationSettings::determineRestrictionsIos(
  hkArray<hkvTransformationMessage>& out_messages)
{
  switch (m_targetDataFormat)
  {
  case HKV_TEXTURE_DATA_FORMAT_PVRTC2:
    {
      m_validationMinSize = 16;
      m_validationNeedsPowerOfTwo = true;
      m_validationNeedsSquare = true;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_PVRTC4:
    {
      m_validationMinSize = 8;
      m_validationNeedsPowerOfTwo = true;
      m_validationNeedsSquare = true;
      break;
    }
  }

  m_validationWarnSize = 1024;
  m_validationMaxSize = 2048;

  return true;
}


bool hkvTextureTransformationSettings::determineRestrictionsPsVita(
  hkArray<hkvTransformationMessage>& out_messages)
{
  switch (m_targetDataFormat)
  {
  case HKV_TEXTURE_DATA_FORMAT_DXT1:
  case HKV_TEXTURE_DATA_FORMAT_DXT3:
  case HKV_TEXTURE_DATA_FORMAT_DXT5:
    {
      m_validationNeedsPowerOfTwo = true;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_A1R5G5B5:
  case HKV_TEXTURE_DATA_FORMAT_A4R4G4B4:
  case HKV_TEXTURE_DATA_FORMAT_A8R8G8B8:
  case HKV_TEXTURE_DATA_FORMAT_R5G6B5:
  case HKV_TEXTURE_DATA_FORMAT_R8G8B8:
    {
      m_validationNeedsMultipleOf = 8;
      break;
    }
  }

  m_validationWarnSize = 1024;
  m_validationMaxSize = 4096;

  return true;
}


bool hkvTextureTransformationSettings::ensureDimensionIntegrity(hkArray<hkvTransformationMessage>& out_messages)
{
  if (m_sourceWidth == 0 || m_sourceHeight == 0)
  {
    out_messages.pushBack(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_ERROR, "Size of source texture is invalid; cannot continue!"));
    return false;
  }

  // Perform sanity checks
  if (m_validationNeedsPowerOfTwo && m_validationNeedsMultipleOf > 1)
  {
    if (hkvMath::isPowerOf2(m_validationNeedsMultipleOf))
    {
      // Both Power-of-Two and Multiple-of restrictions are given. However, multiple-of is a power of two,
      // so we can drop the multiple-of constraint as long as we limit the minimum size.
      m_validationMinSize = hkvMath::Max(m_validationMinSize, m_validationNeedsMultipleOf);
    }
    else
    {
      VASSERT_MSG(false, "Both Power-of-Two and Multiple-of restrictions are specified, but Multiple-of is not a power of two. Ignoring Multiple-of.");
    }
    m_validationNeedsMultipleOf = 1;
  }

  if (m_validationNeedsPowerOfTwo)
  {
    if (m_validationMinSize > 0 && !hkvMath::isPowerOf2(m_validationMinSize))
    {
      VASSERT_MSG(false, "According to set restrictions, system minimum size needs to be Power-of-Two. Adjusting.");
      m_validationMinSize = hkvMath::powerOf2_ceil(m_validationMinSize);
    }

    if (m_userMinSize > 0)
    {
      m_userMinSize = adjustToNearestPowerOfTwo(m_userMinSize);
    }

    if (m_validationMaxSize > 0 && !hkvMath::isPowerOf2(m_validationMaxSize))
    {
      VASSERT_MSG(false, "According to set restrictions, system maximum size needs to be Power-of-Two. Adjusting.");
      m_validationMaxSize = hkvMath::powerOf2_floor(m_validationMaxSize);
    }

    if (m_userMaxSize > 0)
    {
      m_userMaxSize = adjustToNearestPowerOfTwo(m_userMaxSize);
    }
  }

  if (m_validationNeedsMultipleOf > 1)
  {
    if ((m_validationMinSize % m_validationNeedsMultipleOf) != 0)
    {
      VASSERT_MSG(false, "According to set restrictions, system minimum size needs to be Multiple-of. Adjusting.");
      m_validationMinSize = ((m_validationMinSize + m_validationNeedsMultipleOf - 1) / m_validationNeedsMultipleOf) * m_validationNeedsMultipleOf;
    }

    m_userMinSize = ((m_userMinSize + m_validationNeedsMultipleOf - 1) / m_validationNeedsMultipleOf) * m_validationNeedsMultipleOf;

    if (m_validationMaxSize % m_validationNeedsMultipleOf != 0)
    {
      VASSERT_MSG(false, "According to set restrictions, system maximum size needs to be Multiple-of. Adjusting.");
      m_validationMaxSize = (m_validationMaxSize / m_validationNeedsMultipleOf) * m_validationNeedsMultipleOf;
    }

    m_userMaxSize = (m_userMaxSize / m_validationNeedsMultipleOf) * m_validationNeedsMultipleOf;
  }

  if ((m_validationMinSize > 0) && (m_validationMaxSize > 0) && (m_validationMaxSize < m_validationMinSize))
  {
    VASSERT_MSG(false, "System maximum size less than system minimum size; adjusting.");
    m_validationMinSize = m_validationMaxSize;
  }

  if ((m_userMinSize > 0) && (m_userMaxSize > 0) && (m_userMaxSize < m_userMinSize))
  {
    m_userMinSize = m_userMaxSize;
  }

  if ((m_validationMinSize > 0) && (m_userMinSize > 0) && (m_userMinSize < m_validationMinSize))
  {
    m_userMinSize = m_validationMinSize;
  }

  if ((m_validationMaxSize > 0) && (m_userMaxSize > 0) && (m_userMaxSize > m_validationMaxSize))
  {
    m_userMaxSize = m_validationMaxSize;
  }

  return true;
}


bool hkvTextureTransformationSettings::checkDimensions(hkArray<hkvTransformationMessage>& out_messages)
{
  if (!ensureDimensionIntegrity(out_messages))
  {
    return false;
  }

  // Apply either explicit size or downscale
  if (m_explicitTargetSize)
  {
    m_targetWidth = m_explicitWidth;
    m_targetHeight = m_explicitHeight;
  }
  else
  {
    m_targetWidth = m_sourceWidth;
    m_targetHeight = m_sourceHeight;

    for (hkUint32 i = 0; i < m_downscaleLevel; ++i)
    {
      if (m_targetWidth > 1)
      {
        m_targetWidth >>= 1;
      }
      if (m_targetHeight > 1)
      {
        m_targetHeight >>= 1;
      }

      if (m_targetHeight == 1 && m_targetWidth == 1)
      {
        break;
      }
    }
  }

  // Apply square
  if (m_validationNeedsSquare)
  {
    m_targetWidth = hkvMath::Max(m_targetWidth, m_targetHeight);
    m_targetHeight = m_targetWidth;
  }


  const hkUint32& longerSideOrig = (m_sourceWidth > m_sourceHeight) ? m_sourceWidth : m_sourceHeight;
  const hkUint32& shorterSideOrig = (m_sourceWidth > m_sourceHeight) ? m_sourceHeight: m_sourceWidth;
  hkUint32& longerSideTarget = (m_sourceWidth > m_sourceHeight) ? m_targetWidth : m_targetHeight;
  hkUint32& shorterSideTarget = (m_sourceWidth > m_sourceHeight) ? m_targetHeight: m_targetWidth;
  bool userSizeAdjusted = false;

  // Check if we need to apply user-specified maximum size
  if ((m_userMaxSize > 0) && (longerSideTarget > m_userMaxSize))
  {
    longerSideTarget = m_userMaxSize;
    userSizeAdjusted = true;
  }

  // Check if we need to apply user-specified minimum size
  if ((m_userMinSize > 0) && (longerSideTarget < m_userMinSize))
  {
    longerSideTarget = hkvMath::Min(longerSideOrig, m_userMinSize);
    userSizeAdjusted = true;
  }

  // If an adjustment was made, calculate the shorter side as well
  if (userSizeAdjusted)
  {
    if (m_validationNeedsSquare)
    {
      shorterSideTarget = longerSideTarget;
    }
    else
    {
      float aspectRatio = (float)shorterSideOrig / (float)longerSideOrig;
      shorterSideTarget = hkvMath::Max((hkUint32)hkvMath::round(longerSideTarget * aspectRatio), 1u);
    }
  }

  // Apply Power of Two
  if (m_validationNeedsPowerOfTwo)
  {
    m_targetWidth = adjustToNearestPowerOfTwo(m_targetWidth);
    m_targetHeight = adjustToNearestPowerOfTwo(m_targetHeight);
  }

  // Apply Multiple-of (always round up)
  if (m_validationNeedsMultipleOf > 1)
  {
    hkUint32 wRemainder = m_targetWidth % m_validationNeedsMultipleOf;
    if (wRemainder > 0)
    {
      m_targetWidth += (m_validationNeedsMultipleOf - wRemainder);
    }
    hkUint32 hRemainder = m_targetHeight % m_validationNeedsMultipleOf;
    if (hRemainder > 0)
    {
      m_targetHeight += (m_validationNeedsMultipleOf - hRemainder);
    }
  }

  // Adjust to system minimum size
  if (m_validationMinSize > 0)
  {
    m_targetWidth = hkvMath::Max(m_targetWidth, m_validationMinSize);
    m_targetHeight = hkvMath::Max(m_targetHeight, m_validationMinSize);
  }

  // Adjust to maximum size
  if (m_validationMaxSize > 0)
  {
    m_targetWidth = hkvMath::Min(m_targetWidth, m_validationMaxSize);
    m_targetHeight = hkvMath::Min(m_targetHeight, m_validationMaxSize);
  }

  // Emit some information if the size has been changed
  if ((m_targetWidth != m_sourceWidth) || (m_targetHeight != m_sourceHeight))
  {
    hkStringBuf msg;
    msg.printf("Texture size was changed from %i x %i to %i x %i",
      m_sourceWidth, m_sourceHeight, m_targetWidth, m_targetHeight);
    out_messages.pushBack(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_INFO, msg));
  }

  // Check if we need to emit a size warning
  if ((m_validationWarnSize > 0) && ((m_targetWidth > m_validationWarnSize) || (m_targetHeight > m_validationWarnSize)))
  {
    hkStringBuf msg;
    msg.printf("Resulting texture size (%i x %i) is larger than recommended maximum size (%i x %i) of the target platform.",
      m_targetWidth, m_targetHeight, m_validationWarnSize, m_validationWarnSize);
    out_messages.pushBack(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_WARNING, msg));
  }

  return true;
}


bool hkvTextureTransformationSettings::checkAlpha(
  hkArray<hkvTransformationMessage>& out_messages)
{
  bool targetFormatHasAlpha = true;
  switch (m_targetDataFormat)
  {
  case HKV_TEXTURE_DATA_FORMAT_ETC1:
  case HKV_TEXTURE_DATA_FORMAT_DXT1:
  case HKV_TEXTURE_DATA_FORMAT_R5G6B5:
  case HKV_TEXTURE_DATA_FORMAT_R8G8B8:
    {
      targetFormatHasAlpha = false;
      break;
    }
  }

  // Emit a warning if alpha is stripped because of the target format only
  if (m_sourceHasAlpha && !m_discardAlpha && !targetFormatHasAlpha)
  {
    out_messages.pushBack(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_WARNING, "Alpha channel is discarded due to target data format restrictions!"));
  }

  m_targetHasAlpha = (m_sourceHasAlpha && !m_discardAlpha && targetFormatHasAlpha);

  return true;
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
