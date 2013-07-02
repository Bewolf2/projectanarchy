/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_TRACKED_FILE_DUMMY_HPP
#define HKV_TRACKED_FILE_DUMMY_HPP

#include <Common/Base/Object/hkReferencedObject.h>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>

class hkvTrackedFileDummy : public hkReferencedObject, public hkvIProperties
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

  const char* getRelativePath() const;
  void setRelativePath(const char* relativePath);

  const hkFileSystem::TimeStamp& getLastEditTime() const;
  void setLastEditTime(const hkFileSystem::TimeStamp& time);

  ASSETFRAMEWORK_IMPEXP virtual const char* getTypeName() const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP virtual void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP virtual void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

private:
  hkStringPtr m_relativePath;
  hkFileSystem::TimeStamp m_lastEditTime;
  hkvPropertyList m_properties;
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
