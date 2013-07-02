/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvQueryCommands.hpp

#ifndef HKV_QUERY_COMMANDS_HPP_INCLUDED
#define HKV_QUERY_COMMANDS_HPP_INCLUDED

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
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetQuery.hpp>

struct hkvQueryCommandSetProperty
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options(const hkvAssetQuery& query, const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex)
      : m_query(query), m_prop(prop), m_path(path), m_stackIndex(stackIndex) {}

    const hkvAssetQuery& m_query;
    const hkvProperty& m_prop;
    const hkArray<hkStringPtr>& m_path;
    hkUint32 m_stackIndex;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvQueryCommandSetTag
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options(const hkvAssetQuery& query, bool state, hkUint32 tagIdx)
      : m_query(query), m_state(state), m_tagIdx(tagIdx) {}

    const hkvAssetQuery& m_query;
    hkBool m_state;
    hkUint32 m_tagIdx;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvQueryCommandAddTag
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options(const hkvAssetQuery& query, const char* tagName)
      : m_query(query), m_tagName(tagName) {}

    const hkvAssetQuery& m_query;
    hkStringPtr m_tagName;
  };

  ASSETFRAMEWORK_IMPEXP static hkResult execute(const Options& opts);
};


struct hkvQueryCommandDeleteTag
{
  //TODO:TOC HK_TOC_COMMAND();

  struct Options
  {
    //TODO:TOC HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, Options);
    //TODO:TOC HK_DECLARE_REFLECTION();
    Options(hkUint32 tagIdx)
      : m_tagIdx(tagIdx) {}

    hkUint32 m_tagIdx;
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
