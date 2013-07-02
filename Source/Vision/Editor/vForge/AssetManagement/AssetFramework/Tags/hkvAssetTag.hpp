/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VAssetTag.hpp

#ifndef VASSETTAG_HPP_INCLUDED
#define VASSETTAG_HPP_INCLUDED

ANALYSIS_IGNORE_ALL_START
#include <Common/Base/Container/BitField/hkBitField.h>
ANALYSIS_IGNORE_ALL_END

class hkvAssetTagsHelper;

/// \brief
///   The VAssetTag class stores for each tag defined in the hkvAssetTagManager
///   whether it is set or not.
///
/// The tags are stored via a bitfield that maps to the tags defined in the hkvAssetTagManager.
/// Adding a tag will automatically add it to the hkvAssetTagManager if it is not 
/// present there already.
class hkvAssetTags
{
  friend class hkvAssetTagsHelper;
public:
  ASSETFRAMEWORK_IMPEXP hkvAssetTags();
  ASSETFRAMEWORK_IMPEXP hkvAssetTags(const char* szTags);
  ASSETFRAMEWORK_IMPEXP hkvAssetTags(const hkvAssetTags& rhs);
  ASSETFRAMEWORK_IMPEXP ~hkvAssetTags();
  ASSETFRAMEWORK_IMPEXP void operator= (const hkvAssetTags& rhs);
  ASSETFRAMEWORK_IMPEXP hkBool operator< (const hkvAssetTags& rhs) const;

  ASSETFRAMEWORK_IMPEXP void clearTags();
  ASSETFRAMEWORK_IMPEXP void setTags(const char* tags);
  ASSETFRAMEWORK_IMPEXP void addTags(const char* tags);
  ASSETFRAMEWORK_IMPEXP void addTag(const char* tag);
  ASSETFRAMEWORK_IMPEXP void addTag(hkUint32 tagIndex);
  ASSETFRAMEWORK_IMPEXP void removeTag(const char* tag);
  ASSETFRAMEWORK_IMPEXP void removeTag(hkUint32 tagIndex);
  ASSETFRAMEWORK_IMPEXP hkBool isTagSet(const char* tag) const;
  ASSETFRAMEWORK_IMPEXP hkBool isTagSet(hkUint32 tagIndex) const;
  ASSETFRAMEWORK_IMPEXP hkBool isAnyBitSet() const;

  ASSETFRAMEWORK_IMPEXP hkBool contains(const hkvAssetTags& rhs) const;
  ASSETFRAMEWORK_IMPEXP void writeTags(hkStringBuf& out_tags) const;

private:
  void parseTagString (const char* szTags);

private:
  hkBitField m_bits;
};


class hkvAssetTagsHelper
{
public:
  static void logicalAndTags(const hkvAssetTags& operand, hkvAssetTags& inout_result);
  static void logicalOrTags(const hkvAssetTags& operand, hkvAssetTags& inout_result);

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
