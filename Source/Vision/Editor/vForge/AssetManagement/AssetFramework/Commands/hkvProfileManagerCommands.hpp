/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvProfileManagerCommands.hpp

#ifndef HKV_PROFILE_MANAGER_COMMANDS_HPP_INCLUDED
#define HKV_PROFILE_MANAGER_COMMANDS_HPP_INCLUDED

#pragma warning(push)
#pragma warning(disable: 6211 6001 6326 6011 6385 6031 6387 6246 6386)

#include <Common/Base/Types/hkBaseTypes.h>

#ifdef WIN32
#pragma managed(push, off)
#endif

#include <Common/Base/hkBase.h>

#ifdef WIN32
#pragma managed(pop)
#endif

#pragma warning(pop)

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Profiles/hkvProfileManager.hpp>

struct hkvProfileCommandUnloadAll
{
  //TODO:TOC HK_TOC_COMMAND();

  ASSETFRAMEWORK_IMPEXP static hkResult execute();
};


struct hkvProfileCommandLoadManifest
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() {}
    Options(const char* projectPath) : m_projectPath(projectPath) {}

    hkStringPtr m_projectPath;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult HK_CALL execute(const Options& opts);
};


struct hkvProfileCommandSaveManifest
{
  //TODO:TOC HK_TOC_COMMAND();

  ASSETFRAMEWORK_IMPEXP static hkResult HK_CALL execute();
};


struct hkvProfileCommandGetProfileList
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    const std::vector<hkvProfile::RefPtr>* m_profiles;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(Results& results);
};


struct hkvProfileCommandGetTransformRuleTemplates
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    const hkArray<hkvTransformRuleTemplate>* m_transformRuleTemplates;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(Results& results);
};


struct hkvProfileCommandGetActiveProfile
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    hkUint32 m_activeProfile;
    hkBool m_assetPreviewMode;
    hkStringPtr m_name;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(Results& results);
};


struct hkvProfileCommandSetActiveProfile
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options()
      : m_activeProfile(HKV_INVALID_INDEX), m_assetPreviewMode(false) {}
    
    hkUint32 m_activeProfile;
    hkStringPtr m_name;
    hkBool m_assetPreviewMode;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvProfileCommandAddProfile
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options()
      : m_platform(HKV_TARGET_PLATFORM_COUNT), m_copySettingsFromProfileIndex(HKV_INVALID_INDEX) {}

    hkvTargetPlatform m_platform;
    hkStringPtr m_name;
    hkUint32 m_copySettingsFromProfileIndex;

  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvProfileCommandDeleteProfile
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options()
      : m_profileIndex(HKV_INVALID_INDEX) {}

    hkUint32 m_profileIndex;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvProfileCommandGetProfileByName
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() {}

    hkStringPtr m_profileName;
  };

  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    hkUint32 m_index;
    const hkvProfile* m_profile;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts, Results& results);
};


struct hkvProfileCommandAddTemplate
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() {}

    hkStringPtr m_newTemplateName; ///< Name of the new template
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvProfileCommandDuplicateTemplate
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() {}

    unsigned int m_templateIndex;
    hkStringPtr m_newTemplateName;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvProfileCommandDeleteTemplate
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() :
      m_deleteTemplateIndex(HKV_INVALID_INDEX), m_replaceTemplateIndex(HKV_INVALID_INDEX) {}
    Options(hkUint32 deleteTemplateIndex, hkUint32 replaceTemplateIndex) :
      m_deleteTemplateIndex(deleteTemplateIndex), m_replaceTemplateIndex(replaceTemplateIndex) {}

    hkUint32 m_deleteTemplateIndex;
    hkUint32 m_replaceTemplateIndex;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
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
