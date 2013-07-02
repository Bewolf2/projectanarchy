/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTextureTransformationRule.hpp

#ifndef HKV_TEXTURE_TRANSFORMATION_RULE_HPP_INCLUDED
#define HKV_TEXTURE_TRANSFORMATION_RULE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Transformation/hkvFileTransformationStep.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRule.hpp>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationSettings.hpp>

#include <Common/Base/Types/hkBaseTypes.h>


struct hkvTextureVariant
{
  hkvTextureVariant()
  : m_dataFormat(HKV_TEXTURE_DATA_FORMAT_A8R8G8B8), m_fileFormat(HKV_TEXTURE_FILE_FORMAT_DDS)
  {
  }

  hkvTextureVariant(const char* key, hkvTextureDataFormat dataFormat, 
    hkvTextureFileFormat fileFormat, const char* filenameAddition, bool isEditorPreview)
  : m_variantKey(key), m_dataFormat(dataFormat), m_fileFormat(fileFormat), 
    m_filenameAddition(filenameAddition), m_isEditorPreview(isEditorPreview)
  {
  }

  hkvTextureVariant(const hkvTextureVariant& rhs)
  : m_variantKey(rhs.m_variantKey), m_dataFormat(rhs.m_dataFormat), m_fileFormat(rhs.m_fileFormat), 
    m_filenameAddition(rhs.m_filenameAddition), m_isEditorPreview(rhs.m_isEditorPreview)
  {
  }

  hkvTextureVariant& operator=(const hkvTextureVariant& rhs)
  {
    m_variantKey = rhs.m_variantKey;
    m_dataFormat = rhs.m_dataFormat;
    m_fileFormat = rhs.m_fileFormat;
    m_filenameAddition = rhs.m_filenameAddition;
    m_isEditorPreview = rhs.m_isEditorPreview;
    return *this;
  }

  hkStringPtr m_variantKey;
  hkvTextureDataFormat m_dataFormat;
  hkvTextureFileFormat m_fileFormat;
  hkStringPtr m_filenameAddition;
  hkBool m_isEditorPreview;
};


class hkvIProperties;
class hkvProcessRunner;
struct hkvTransformationInput;
struct hkvTransformationOutput;

class hkvTextureTransformationRule : public hkvTransformationRule
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

private:
  enum TransformationStepType
  {
    STEP_TYPE_INVALID,
    STEP_TYPE_WIIU_TEXCONV2,
    STEP_TYPE_IMAGE_TO_DDS,
    STEP_TYPE_NVDXT,
    STEP_TYPE_PVRTEXTOOL,
    STEP_TYPE_TEXCONV
  };

  struct TransformationStepInfo
  {
  public:
    TransformationStepInfo();
    TransformationStepInfo(TransformationStepType tool, const char* targetExtension, const hkvTextureTransformationSettings& settings);
    TransformationStepInfo(const TransformationStepInfo& other);
    TransformationStepInfo& operator=(const TransformationStepInfo& other);

    TransformationStepType getStepType() const { return m_stepType; }
    const char* getTargetExtension() const { return m_targetExtension; }
    const hkvTextureTransformationSettings& getSettings() const { return m_settings; }
    hkvTextureTransformationSettings& getSettings() { return m_settings; }

  private:
    TransformationStepType m_stepType;
    hkStringPtr m_targetExtension;
    hkvTextureTransformationSettings m_settings;
  };


  class Context : public hkvTransformationRule::Context, public hkvITransformationControl
  {
  public:
    Context(const hkvTransformationInput& input, hkvTransformationOutput& output);
    ~Context();

    virtual void cancelTransformation() HKV_OVERRIDE;

    hkvImageFileFormat m_sourceFormat;
    hkBool m_sourceHasAlpha;
    hkUint32 m_sourceWidth;
    hkUint32 m_sourceHeight;
    hkvTextureUsage m_sourceUsage;
    hkBool m_sourceSrgb;

    hkArray<hkvTextureVariant> m_outputVariants;

    hkArray<TransformationStepInfo> m_transformationSteps;

    bool m_canceled;
    hkRefPtr<hkvFileTransformationStep> m_currentTransformationStep;
  };

public: // static functions
  static void staticInit();
  static void staticDeInit();
  static hkvTransformationRule* createTransformationRule(hkvTargetPlatform targetPlatform);

public:
  hkvTextureTransformationRule (hkvTargetPlatform platform);
  ~hkvTextureTransformationRule();

  // IProperties implementation
  virtual void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  virtual void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

public: // Interface
  virtual hkUint32 getTypeIndex() const HKV_OVERRIDE;
  virtual const char* getTypeName() const HKV_OVERRIDE;

  virtual bool executeTransformation(const hkvTransformationInput& input, hkvTransformationOutput& output) const HKV_OVERRIDE;
  virtual bool queryOutputFileSpecs(const hkvTransformationInput& input, hkvTransformationOutput& output) const HKV_OVERRIDE;

private:
  hkResult determineOutputs(Context& context) const;
  hkResult determineOutputsAndroid(Context& context) const;
  hkResult determineOutputsDxt(Context& context, bool isEditorPreview) const;
  hkResult determineOutputsIos(Context& context) const;
  hkResult determineOutputsWiiU(Context& context) const;
  hkResult examineSourceFile(Context& context) const;
  hkResult prepareTransformationSettings(Context& context, const hkvTextureVariant& variant) const;
  hkResult runConversion(Context& context) const;
  hkResult transferSourceProperties(Context& context) const;

private:
  static hkUint32 s_ruleTypeIndex;
  static hkvEnumDefinition s_compressionDefinition;
  static hkvEnumDefinition s_usageDefinition;
  static CRITICAL_SECTION s_protect;

private:
  hkvEnumInstance m_compressionInstance;
  bool m_removeAlphaChannel;
  bool m_createMipMaps;
  hkUint32 m_downscaleLevel;
  hkUint32 m_minSize;
  hkUint32 m_maxSize;
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
