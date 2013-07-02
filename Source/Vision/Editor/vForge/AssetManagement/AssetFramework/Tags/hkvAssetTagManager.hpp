/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetTagManager.hpp

#ifndef VASSETTAGMANAGER_HPP_INCLUDED
#define VASSETTAGMANAGER_HPP_INCLUDED

#include "../Base/hkvCallback.hpp"

/// \brief
///   Data object for the OnTagDataChanged callback in the hkvAssetTagManager.
class hkvTagDataChangedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvTagDataChangedData(hkvCallback* sender) : hkvCallbackData(sender)
  {
    m_tags = NULL;
  }
  const hkArray<hkStringPtr>* m_tags;
};


/// \brief
///   The hkvAssetTagManager handles all tags that are loaded from assets via an asset library and
///   tags that are dynamically added at runtime. Each tag is assigned a unique number during runtime
///   which is used to represent used tags as a bitfield.
class hkvAssetTagManager
{
public:
  ASSETFRAMEWORK_IMPEXP static hkvAssetTagManager* getGlobalInstance();
  ASSETFRAMEWORK_IMPEXP static void deInitAssetTagManager();

public:
  ASSETFRAMEWORK_IMPEXP hkvAssetTagManager();
  ASSETFRAMEWORK_IMPEXP ~hkvAssetTagManager();
  ASSETFRAMEWORK_IMPEXP void clear();

  // Tag Handling
  ASSETFRAMEWORK_IMPEXP hkUint32 getTagCount() const;
  ASSETFRAMEWORK_IMPEXP const char* getTag(hkUint32 tagIndex) const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getTagIndex(const char* tagName) const;
  ASSETFRAMEWORK_IMPEXP hkUint32 addTag(const char* tagName);
  ASSETFRAMEWORK_IMPEXP hkBool removeTag(hkUint32 tagIndex);
  ASSETFRAMEWORK_IMPEXP const hkArray<hkStringPtr>& getTags() const;


public: // Data
  hkvCallback OnTagDataChanged;

private:
  void callOnTagDataChanged();

private:
  hkArray<hkStringPtr> m_tags;

private:
  static hkvAssetTagManager* s_globalInstance;
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
