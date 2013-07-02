/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_CUSTOM_TRANSFORMATION_RULE_HPP_INCLUDED
#define HKV_CUSTOM_TRANSFORMATION_RULE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRule.hpp>


class hkvProcessRunner;
struct hkvTransformationInput;
struct hkvTransformationOutput;

class hkvCustomTransformationRule : public hkvTransformationRule
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public: // static functions
  static void staticInit();
  static void staticDeInit();
  static hkvTransformationRule* createTransformationRule(hkvTargetPlatform targetPlatform);

public:
  ASSETFRAMEWORK_IMPEXP hkvCustomTransformationRule (hkvTargetPlatform platform);
  ASSETFRAMEWORK_IMPEXP ~hkvCustomTransformationRule();

  // IProperties implementation
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

public: // Interface
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkUint32 getTypeIndex() const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE const char* getTypeName() const HKV_OVERRIDE;

  ASSETFRAMEWORK_IMPEXP virtual bool executeTransformation(const hkvTransformationInput& input, hkvTransformationOutput& output) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP virtual bool queryOutputFileSpecs(const hkvTransformationInput& input, hkvTransformationOutput& output) const HKV_OVERRIDE;

private:
  hkResult runConversion(Context& context) const;

  static hkUint32 s_ruleTypeIndex;

  hkStringPtr m_executable;
  hkStringPtr m_arguments;
  hkStringPtr m_targetExtension;
  hkBool m_useTempDirectory;
  hkBool m_checkExitCode;
  hkBool m_checkOutputFile;
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
