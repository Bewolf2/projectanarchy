/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetManager.hpp

#ifndef HKV_ASSET_MANAGER_HPP_INCLUDED
#define HKV_ASSET_MANAGER_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetLibrary.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvBackgroundAssetHandling.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBackgroundProcessor.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Profiles/hkvProfile.hpp>

ANALYSIS_IGNORE_ALL_START
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Memory/Allocator/LargeBlock/hkLargeBlockAllocator.h>
#include <Common/Base/Memory/System/FreeList/hkFreeListMemorySystem.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/DebugUtil/MultiThreadCheck/hkMultiThreadCheck.h>
ANALYSIS_IGNORE_ALL_END

#include <vector>

class hkvActiveProfileChangedData;
class hkvAssetUpdateBackgroundProcessor;
class hkvSubtreeMonitor;
class hkvTrackedDirectory;
class hkvTransformationBackgroundProcessor;

/// \brief
///   Data object for the OnQueryInvalidated callback in the hkvAssetManager.
class hkvQueryInvalidatedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvQueryInvalidatedData(hkvCallback* sender) : hkvCallbackData(sender)
  {
  }
};


/// \brief
///   Data object for the OnAssetChanged callback in the hkvAssetManager.
class hkvAssetChangedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvAssetChangedData(hkvCallback* sender, const hkvAsset& asset)
  : hkvCallbackData(sender), m_asset(&asset)
  {
  }

  const hkvAsset::RefCPtr m_asset;
};


/// \brief
///   Entry struct for the hkvReloadResources data callback data object
struct hkvReloadResourcesEntry
{
  hkvReloadResourcesEntry(const char* resourceManager, const char* resourceId) :
    m_resourceManager(resourceManager), m_resourceId(resourceId)
  {
  }
  
  hkvReloadResourcesEntry(const hkvReloadResourcesEntry& rhs) :
    m_resourceManager(rhs.m_resourceManager), m_resourceId(rhs.m_resourceId)
  {
  }

  hkvReloadResourcesEntry& operator=(const hkvReloadResourcesEntry& rhs)
  {
    m_resourceManager = rhs.m_resourceManager;
    m_resourceId = rhs.m_resourceId;
  }

  hkStringPtr m_resourceManager;
  hkStringPtr m_resourceId;
};


/// \brief
///   Data object for the OnReloadResources callback in the hkvAssetManager
class hkvReloadResourcesData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvReloadResourcesData(const hkvCallback* sender,
    const std::vector<hkStringPtr>& lookupTables, const std::vector<hkvReloadResourcesEntry>& resources) 
  : hkvCallbackData(sender), m_lookupTables(lookupTables), m_resources(resources), m_reloadAllResources(false)
  {
  }

  ASSETFRAMEWORK_IMPEXP hkvReloadResourcesData(const hkvCallback* sender,
    const std::vector<hkStringPtr>& lookupTables) 
  : hkvCallbackData(sender), m_lookupTables(lookupTables), m_reloadAllResources(true)
  {
  }

  std::vector<hkStringPtr> m_lookupTables;
  std::vector<hkvReloadResourcesEntry> m_resources;
  bool m_reloadAllResources;
};


/// \brief
///   Data object for the OnEngineMessage callback in the hkvAssetManager
class hkvEngineMessageData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvEngineMessageData(const hkvCallback* sender,
    hkvMessageSeverity severity, const char* message)
  : hkvCallbackData(sender), m_severity(severity), m_message(message)
  {
  }

  hkvMessageSeverity m_severity;
  hkStringPtr m_message;
};


/// \brief
///   Data object for the OnQueryEditorStatus callback in the hkvAssetManager
class hkvQueryEditorStatusData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvQueryEditorStatusData(const hkvCallback* sender)
  : hkvCallbackData(sender), m_valid(false), m_isDevelopmentMachine(false)
  {
  }

  bool m_valid;
  bool m_isDevelopmentMachine;
};


/// \brief
///   Status of the automatic asset transformation pipeline.
struct hkvTransformStatus
{
  ASSETFRAMEWORK_IMPEXP hkvTransformStatus()
    : m_automaticTransform(true), m_totalAssets(0), m_totalAssetsNeedingUpdate(0), m_totalAssetsNeedingTransform(0) {}
  ASSETFRAMEWORK_IMPEXP hkvTransformStatus(bool automaticTransform, hkUint32 totalAssets, hkUint32 totalAssetsNeedingUpdate, hkUint32 totalAssetsNeedingTransform)
    : m_automaticTransform(automaticTransform), m_totalAssets(totalAssets), m_totalAssetsNeedingUpdate(totalAssetsNeedingUpdate), m_totalAssetsNeedingTransform(totalAssetsNeedingTransform) {}

  ASSETFRAMEWORK_IMPEXP bool operator==(const hkvTransformStatus& rhs) const
  {
    return m_automaticTransform == rhs.m_automaticTransform && m_totalAssets == rhs.m_totalAssets &&
      m_totalAssetsNeedingUpdate == rhs.m_totalAssetsNeedingUpdate && m_totalAssetsNeedingTransform == rhs.m_totalAssetsNeedingTransform;
  }

  bool m_automaticTransform;
  hkUint32 m_totalAssets;
  hkUint32 m_totalAssetsNeedingUpdate;
  hkUint32 m_totalAssetsNeedingTransform;
};


/// \brief
///   Data object for the OnTransformStatusChanged callback
class hkvTransformStatusChangedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvTransformStatusChangedData(const hkvCallback* sender, const hkvTransformStatus& status)
    : hkvCallbackData(sender), m_status(status) {}

public:
  hkvTransformStatus m_status;
};

/// \brief
///   The hkvAssetManager is the central manager of the asset management system.
///   It handles asset libraries, queries and asset selections.
class hkvAssetManager
{
private:
  class ProfileManagerEventHandler : public hkvCallbackHandler
  {
  public:
    ProfileManagerEventHandler(hkvAssetManager& owner);
    virtual void handleCallback(hkvCallbackData* data) HKV_OVERRIDE;
  private:
    hkvAssetManager& m_owner;
  };

private:
  typedef std::vector<hkRefPtr<hkvAssetLibrary> > LibraryList;

public:
  ASSETFRAMEWORK_IMPEXP static hkvAssetManager* getGlobalInstance();
  ASSETFRAMEWORK_IMPEXP static void initAssetManager();
  ASSETFRAMEWORK_IMPEXP static void deInitAssetManager();
  ASSETFRAMEWORK_IMPEXP static bool isAssetManagerInitialized();

public:
  ASSETFRAMEWORK_IMPEXP hkvAssetManager();
  ASSETFRAMEWORK_IMPEXP ~hkvAssetManager();

  ASSETFRAMEWORK_IMPEXP hkvCallbackHandler& getProfileManagerEventHandler();

  ASSETFRAMEWORK_IMPEXP hkvCriticalSectionLock acquireLock();

  // Asset Handling

  /// \brief Returns true if the given path is relative to one of the asset libraries (a.k.a. data directories).
  ASSETFRAMEWORK_IMPEXP bool makePathRelative(const char* absolutePath, hkStringBuf& out_relativePath, hkvAssetLibrary::RefPtr& out_library) const;
  /// \brief Searches all loaded libraries for an asset which was created from the given file. Returns NULL if nothing could be found.
  ASSETFRAMEWORK_IMPEXP hkvAsset::RefPtr getAssetByPath(const char* szPathToFile) const;

  ASSETFRAMEWORK_IMPEXP bool canApplyAssetChanges() const;

  // Asset Library Handling
  ASSETFRAMEWORK_IMPEXP void clear();
  ASSETFRAMEWORK_IMPEXP hkUint32 addAssetLibrary(hkvAssetLibrary* library);
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumberOfAssetLibraries() const;
  ASSETFRAMEWORK_IMPEXP hkvAssetLibrary::RefPtr getAssetLibrary(hkUint32 libraryIndex) const;
  ASSETFRAMEWORK_IMPEXP hkvAssetLibrary::RefPtr getAssetLibraryByName(const char* libraryName) const;
  ASSETFRAMEWORK_IMPEXP hkvAssetLibrary::RefPtr getAssetLibraryByAbsolutePath(const char* absolutePath) const;
  ASSETFRAMEWORK_IMPEXP hkvAssetLibrary::RefPtr getProjectRootLibrary() const;
  ASSETFRAMEWORK_IMPEXP void refreshAssetLibrary(const char* libraryName, const char* relativePath = NULL);
  ASSETFRAMEWORK_IMPEXP void query(hkvAssetQuery& query) const;
  ASSETFRAMEWORK_IMPEXP void callOnQueryInvalidated();
  ASSETFRAMEWORK_IMPEXP bool saveLibraries();
  ASSETFRAMEWORK_IMPEXP bool writeLookupTables(hkvProfile::RefCPtr profile, bool forEditor, bool assetPreviewMode, std::vector<hkStringPtr>* out_failedFiles = NULL) const;

  /// \brief 
  ///   Cleans up stale asset files in all known asset libraries.
  /// \return
  ///   the number of stale files deleted, or -1, if an error occurred
  ASSETFRAMEWORK_IMPEXP hkInt32 cleanUpAssetCache();

  ASSETFRAMEWORK_IMPEXP hkvAssetTrackedFile* getTrackedFileByPath(const char* relativePath, hkvAssetLibrary* libraryHint = NULL) const;

  // Background Processing
  ASSETFRAMEWORK_IMPEXP bool isBackgroundProcessingEnabled() const;
  ASSETFRAMEWORK_IMPEXP void setBackgroundProcessingEnabled(bool enabled);

  ASSETFRAMEWORK_IMPEXP void initBackgroundProcessing();
  ASSETFRAMEWORK_IMPEXP void deinitBackgroundProcessing();

  ASSETFRAMEWORK_IMPEXP bool hasBackgroundProcessing() const;

  ASSETFRAMEWORK_IMPEXP hkvBackgroundAssetHandling* getBackgroundAssetHandling() const;
  ASSETFRAMEWORK_IMPEXP void setBackgroundAssetHandling(hkvBackgroundAssetHandling* newHandling);

  ASSETFRAMEWORK_IMPEXP void queueAssetForUpdate(hkvAsset& asset);
  ASSETFRAMEWORK_IMPEXP void queueAssetForTransformation(hkvAsset& asset);
  ASSETFRAMEWORK_IMPEXP void queueAllAssetsForTransformation();

  ASSETFRAMEWORK_IMPEXP void fetchBackgroundTransformationResults();
  ASSETFRAMEWORK_IMPEXP void evaluateBackgroundTransformationResults();
  ASSETFRAMEWORK_IMPEXP bool performMainThreadWork();
  ASSETFRAMEWORK_IMPEXP void gatherBackgroundUpdateStatistics(hkUint32& out_count, hkUint32& out_remaining) const;
  ASSETFRAMEWORK_IMPEXP void gatherBackgroundTransformStatistics(hkUint32& out_count, hkUint32& out_remaining) const;

  ASSETFRAMEWORK_IMPEXP void postEngineMessage(hkvMessageSeverity severity, const char* message);

  ASSETFRAMEWORK_IMPEXP void callOnAssetChanged(const hkvAsset& asset);
  ASSETFRAMEWORK_IMPEXP void callOnLibraryDataChanged();
  ASSETFRAMEWORK_IMPEXP void callOnTransformStatusChanged();

  // Notifications
  ASSETFRAMEWORK_IMPEXP void handleProjectLoaded();
  ASSETFRAMEWORK_IMPEXP void handleProjectUnloaded();

public: // data
  hkvCallback OnLibraryDataChanged;
  hkvCallback OnQueryInvalidated;
  hkvCallback OnAssetChanged;
  hkvCallback OnReloadResources;
  hkvCallback OnEngineMessage;
  hkvCallback OnQueryEditorStatus;
  hkvCallback OnTransformStatusChanged;

private: // functions
  void UpdateLibraryIndices();
  void finalizeDependencies();

  // Callback handling
  void handleActiveProfileChanged(hkvActiveProfileChangedData& data);

private: //data
  mutable hkCriticalSection m_protect;
  LibraryList m_assetLibraries;

  bool m_editorProjectLoaded;

  bool m_backgroundProcessingEnabled;
  hkRefPtr<hkvBackgroundAssetHandling> m_backgroundAssetHandling;

  std::vector<hkvAsset::RefPtr> m_assetsToRefreshInEngine;

  ProfileManagerEventHandler* m_profileManagerEventHandler;

private: // static data
  static hkvAssetManager* s_pGlobalInstance;
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
