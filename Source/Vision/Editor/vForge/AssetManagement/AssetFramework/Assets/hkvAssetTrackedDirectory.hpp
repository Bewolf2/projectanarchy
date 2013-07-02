/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_ASSET_TRACKED_DIRECTORY_HPP
#define HKV_ASSET_TRACKED_DIRECTORY_HPP

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvTrackedDirectory.hpp>


class hkvAssetLibrary;
class hkvAssetTrackedTreeInfo;


class hkvAssetTrackedDirectory : public hkvTrackedDirectory
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  ASSETFRAMEWORK_IMPEXP hkvAssetTrackedDirectory(hkvAssetTrackedTreeInfo& treeInfo, hkvAssetTrackedDirectory* parent, const char* name);
protected:
  hkvAssetTrackedDirectory(const hkvAssetTrackedDirectory&);
  hkvAssetTrackedDirectory& operator=(const hkvAssetTrackedDirectory&);

public:
  ASSETFRAMEWORK_IMPEXP virtual const char* getDisplayName() const HKV_OVERRIDE;

  ASSETFRAMEWORK_IMPEXP bool containsAssets() const;

protected:
  virtual hkvTrackedDirectory* createDirectory(const char* name) HKV_OVERRIDE;
  virtual hkvTrackedFile* createFile(const char* name) HKV_OVERRIDE;
};


class hkvAssetTrackedTreeInfo : public hkvTrackedTreeInfo
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  hkvAssetTrackedTreeInfo(hkvAssetLibrary& owner);
private:
  hkvAssetTrackedTreeInfo(const hkvAssetTrackedTreeInfo&);
  hkvAssetTrackedTreeInfo& operator=(const hkvAssetTrackedTreeInfo&);

public:
  virtual hkvCriticalSectionLock acquireLock() HKV_OVERRIDE;

  virtual bool canApplyChanges() const HKV_OVERRIDE;
  virtual bool canChangeStructure() const HKV_OVERRIDE;

  virtual bool shouldIgnorePath(const char* path) const HKV_OVERRIDE;

  hkvAssetLibrary& getAssetLibrary() const;

protected:
  virtual void doSendUpdateNotification() HKV_OVERRIDE;

private:
  hkvAssetLibrary& m_owner;
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
