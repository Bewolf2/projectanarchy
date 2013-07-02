/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_TEXTURE_TRANSFORMATION_SETTINGS_HPP_INCLUDED
#define HKV_TEXTURE_TRANSFORMATION_SETTINGS_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvImageFileProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>

class hkvTextureTransformationSettings
{
public:
  hkvTextureTransformationSettings(hkvTargetPlatform platform);


public:
  void assignFrom(const hkvTextureTransformationSettings& other, bool explicitSize);

  void setSourceProperties(hkvTextureUsage usage, bool srgb, bool hasAlpha, hkUint32 width, hkUint32 height);

  void setTargetFormat(hkvTextureDataFormat dataFormat, hkvTextureFileFormat fileFormat);
  void setDiscardAlpha(bool discard);
  void setCreateMipMaps(bool create);
  void setDownscaleLevel(hkUint32 level);
  void setUserMaxSize(hkUint32 size);
  void setUserMinSize(hkUint32 size);
  void setExplicitTargetSize(hkUint32 width, hkUint32 height);
  void clearExplicitTargetSize();
  void setApplyMipMapRestrictions(bool apply);
  void setIgnoreTargetFormatRestrictions(bool ignore);

  hkResult validate(hkArray<hkvTransformationMessage>& out_messages);

  bool isValid() const { return m_valid; }

  hkUint32 getSourceWidth() const { return m_sourceWidth; }
  hkUint32 getSourceHeight() const { return m_sourceHeight; }

  hkvTextureDataFormat getTargetDataFormat() const { return m_targetDataFormat; }
  hkvTextureFileFormat getTargetFileFormat() const { return m_targetFileFormat; }

  bool getCreateMipMaps() const { return m_createMipMaps; }

  bool getTargetHasAlpha() const { return m_targetHasAlpha; }
  hkUint32 getTargetWidth() const { return m_targetWidth; }
  hkUint32 getTargetHeight() const { return m_targetHeight; }


private:
  static hkUint32 adjustToNearestPowerOfTwo(hkUint32 i);

  bool checkFormatCompatibility(hkArray<hkvTransformationMessage>& out_messages);
  bool determineRestrictions(hkArray<hkvTransformationMessage>& out_messages);
  bool determineRestrictionsAndroid(hkArray<hkvTransformationMessage>& out_messages);
  bool determineRestrictionsIos(hkArray<hkvTransformationMessage>& out_messages);
  bool determineRestrictionsPsVita(hkArray<hkvTransformationMessage>& out_messages);
  bool ensureDimensionIntegrity(hkArray<hkvTransformationMessage>& out_messages);
  bool checkDimensions(hkArray<hkvTransformationMessage>& out_messages);
  bool checkAlpha(hkArray<hkvTransformationMessage>& out_messages);


private:
  hkvTargetPlatform m_platform;

  hkBool m_sourceHasAlpha;
  hkUint32 m_sourceWidth;
  hkUint32 m_sourceHeight;
  hkvTextureUsage m_sourceUsage;
  hkBool m_sourceSrgb;

  hkvTextureDataFormat m_targetDataFormat;
  hkvTextureFileFormat m_targetFileFormat;
  hkBool m_discardAlpha;
  hkBool m_createMipMaps;
  hkUint32 m_downscaleLevel;
  hkUint32 m_userMinSize;
  hkUint32 m_userMaxSize;
  hkBool m_explicitTargetSize;
  hkUint32 m_explicitWidth;
  hkUint32 m_explicitHeight;
  hkBool m_applyMipMapRestrictions;
  hkBool m_ignoreTargetFormatRestrictions;

  hkUint32 m_validationMinSize;
  hkUint32 m_validationWarnSize;
  hkUint32 m_validationMaxSize;
  hkBool m_validationNeedsPowerOfTwo;
  hkBool m_validationNeedsSquare;
  hkUint32 m_validationNeedsMultipleOf;

  hkBool m_valid;

  hkBool m_targetHasAlpha;
  hkUint32 m_targetWidth;
  hkUint32 m_targetHeight;
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
