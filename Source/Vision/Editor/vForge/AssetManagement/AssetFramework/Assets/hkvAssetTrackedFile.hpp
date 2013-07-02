/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_ASSET_TRACKED_FILE_HPP
#define HKV_ASSET_TRACKED_FILE_HPP

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvTrackedFile.hpp>

#include <Common/Base/Types/hkRefPtr.h>

class hkvAsset;
class hkvAssetTrackedDirectory;

class hkvAssetTrackedFile : public hkvTrackedFile
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  ASSETFRAMEWORK_IMPEXP hkvAssetTrackedFile(hkvAssetTrackedDirectory& parent, const char* name);
private:
  hkvAssetTrackedFile(const hkvAssetTrackedFile&);
  hkvAssetTrackedFile& operator=(const hkvAssetTrackedFile&);

public:
  hkvAsset* getAsset() const;
  void setAsset(hkvAsset* asset);

  ASSETFRAMEWORK_IMPEXP virtual const char* getTypeName() const HKV_OVERRIDE;

protected:
  virtual bool onChanged() HKV_OVERRIDE;
  virtual void onRemove() HKV_OVERRIDE;

  virtual hkvAssetOperationResult onUpdateData(hkStreamReader& reader) HKV_OVERRIDE;
  virtual void onClearData() HKV_OVERRIDE;

  virtual bool canRemove() HKV_OVERRIDE;

  virtual hkvCriticalSectionLock acquireDependencyLock() const HKV_OVERRIDE;

private:
  void addAsset(hkvAsset& asset);
  void removeAsset();
  void queueAssetForUpdate();

private:
  hkRefPtr<hkvAsset> m_asset;
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
