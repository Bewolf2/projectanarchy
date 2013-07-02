/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetManagerCommands.hpp

#ifndef HKV_ASSET_MANAGER_COMMANDS_HPP_INCLUDED
#define HKV_ASSET_MANAGER_COMMANDS_HPP_INCLUDED

#pragma warning(push)
#pragma warning(disable: 6211 6001 6326 6011 6385 6031 6387 6246 6386)
//ANALYSIS_IGNORE_ALL_START
#include <Common/Base/Types/hkBaseTypes.h>
//ANALYSIS_IGNORE_ALL_END
#pragma warning(pop)
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>


struct hkvAssetCommandSetEnableBackgroundProcessing
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() {}
    Options(bool enable)
      : m_enable(enable) {}

    hkBool m_enable;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvAssetCommandAddDataDirectory
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() {}
    Options(const char* dataDirectory, const char* libraryName, bool create, bool isProjectRoot)
      : m_create(create), m_dataDirectory(dataDirectory), m_libraryName(libraryName), m_isProjectRoot(isProjectRoot) {}

    hkStringPtr m_dataDirectory;
    hkStringPtr m_libraryName;
    hkStringPtr m_localFileHint;
    hkBool m_create;
    hkBool m_isProjectRoot;
  };

  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    hkvAssetLibraryLoadResult m_loadResult;
    hkvAssetLibrary::RefPtr m_library;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts, Results& results);
};


struct hkvAssetCommandSaveLibraries
{
  //TODO:TOC HK_TOC_COMMAND();

  ASSETFRAMEWORK_IMPEXP static hkResult execute();
};


struct hkvAssetCommandWriteLUTs
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() {}
  };

  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    std::vector<hkStringPtr> m_failedFiles;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts, Results& res);
};


struct hkvAssetCommandCleanUpAssetCache
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options() {}
  };


  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    hkUint32 m_numFilesDeleted;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts, Results& res);
};


struct hkvAssetCommandGetTypesInfo
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    hkArray<hkvAssetTypeInfo> m_typesInfo;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(Results& results);
};


struct hkvAssetCommandGetTagsInfo
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Results
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Results);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Results() {}

    const hkArray<hkStringPtr>* m_tagsInfo;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(Results& results);
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
