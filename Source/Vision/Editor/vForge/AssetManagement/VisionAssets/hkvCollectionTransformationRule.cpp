/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvCollectionTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Collection/hkvCollection.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRuleTypeManager.hpp>

#include <Common/Base/Container/String/hkUtf8.h>


hkUint32 hkvCollectionTransformationRule::s_ruleTypeIndex = HKV_INVALID_INDEX;


/////////////////////////////////////////////////////////////////////////////
// hkvCollectionTransformationRule static functions
/////////////////////////////////////////////////////////////////////////////

void hkvCollectionTransformationRule::staticInit()
{
  VASSERT(s_ruleTypeIndex == HKV_INVALID_INDEX);

  hkvTransformationRuleTypeInfo ti;
  ti.m_name = "CollectionTransformationRule";
  ti.m_createFunc = &createTransformationRule;
  ti.m_supportsAllTypes = false;
  ti.m_supportedAssetTypes.pushBack("Collection");
  ti.m_priority = 100;

  // register at the hkvTransformationRuleTypeManager and store the type index in static variable.
  s_ruleTypeIndex = hkvTransformationRuleTypeManager::getGlobalInstance()->addType(ti);
}


void hkvCollectionTransformationRule::staticDeInit()
{
  if (s_ruleTypeIndex == HKV_INVALID_INDEX)
    return;

  // de-register at the hkvTransformationRuleTypeManager
  hkvTransformationRuleTypeManager::getGlobalInstance()->removeType(s_ruleTypeIndex);
  s_ruleTypeIndex = HKV_INVALID_INDEX;
}


hkvTransformationRule* hkvCollectionTransformationRule::createTransformationRule(hkvTargetPlatform targetPlatform)
{
  hkvTransformationRule* pRule = new hkvCollectionTransformationRule(targetPlatform);
  VASSERT(pRule);
  return pRule;
}


/////////////////////////////////////////////////////////////////////////////
// hkvCollectionTransformationRule public functions
/////////////////////////////////////////////////////////////////////////////

hkvCollectionTransformationRule::hkvCollectionTransformationRule(hkvTargetPlatform platform)
: hkvTransformationRule(platform)
{
}


hkvCollectionTransformationRule::~hkvCollectionTransformationRule()
{
}


void hkvCollectionTransformationRule::getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
}


void hkvCollectionTransformationRule::setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int iStackIndex, hkvProperty::Purpose purpose)
{
}

unsigned int hkvCollectionTransformationRule::getTypeIndex() const
{
  return s_ruleTypeIndex;
}


const char* hkvCollectionTransformationRule::getTypeName() const
{
  return "CollectionTransformationRule";
}


bool hkvCollectionTransformationRule::executeTransformation(const hkvTransformationInput& input, hkvTransformationOutput& output) const
{
  Context context(input, output);

  hkStringBuf filePathBuf;
  context.m_targetFile = makeAbsoluteTargetFilePath(context, ".vres", filePathBuf);

  bool ok = (runConversion(context) == HK_SUCCESS);

  if (ok)
  {
    notifyOutputWritten(context, NULL);
    context.m_output.m_messages.pushBack(hkvTransformationMessage(
      HKV_MESSAGE_SEVERITY_INFO, "Collection transformation succeeded."));
  }
  else
  {
    context.m_output.m_messages.pushBack(hkvTransformationMessage(
      HKV_MESSAGE_SEVERITY_ERROR, "Collection transformation failed!"));
  }

  return ok;
}


bool hkvCollectionTransformationRule::queryOutputFileSpecs(const hkvTransformationInput& input, hkvTransformationOutput& output) const
{
  Context context(input, output);

  // Our only output is a .vres file.
  hkvTransformationOutputFileSpec outSpec;
  hkStringBuf fileNameBuf;
  outSpec.m_fileName = makeTargetFileName(context, ".vres", fileNameBuf);

  output.m_outputFileSpecs.pushBack(outSpec);

  return true;
}


hkResult hkvCollectionTransformationRule::runConversion(Context& context) const
{
  context.m_stepSourceFile = context.m_input.m_sourceFile;
  context.m_stepTargetFile = context.m_targetFile;

  hkStringBuf targetDirBuf(context.m_stepTargetFile);
  targetDirBuf.pathDirname();

  if (hkvFileHelper::createDirectoryRecursive(targetDirBuf) != HK_SUCCESS)
  {
    return HK_FAILURE;
  }

  hkvCollection collection;
  hkResult res = collection.loadCollection(context.m_stepSourceFile);
  if (res == HK_SUCCESS)
  {
    res = collection.saveResourceSnapshot(context.m_stepTargetFile);
  }

  return res;
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
