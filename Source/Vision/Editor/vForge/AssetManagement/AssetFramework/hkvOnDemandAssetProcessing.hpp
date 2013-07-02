/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_ON_DEMAND_ASSET_PROCESSING_HPP
#define HKV_ON_DEMAND_ASSET_PROCESSING_HPP

#include <Common/Base/Thread/Thread/hkThread.h>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvBackgroundAssetHandling.hpp>

class hkvOnDemandAssetProcessing : public hkReferencedObject
{
public:
  enum Status
  {
    STATUS_READY,
    STATUS_PREPARING,
    STATUS_WORKING,
    STATUS_FINISHING,
    STATUS_FINISHED
  };

public:
  ASSETFRAMEWORK_IMPEXP hkvOnDemandAssetProcessing();
private:
  hkvOnDemandAssetProcessing(const hkvOnDemandAssetProcessing&);
  hkvOnDemandAssetProcessing& operator=(const hkvOnDemandAssetProcessing&);
public:
  ASSETFRAMEWORK_IMPEXP ~hkvOnDemandAssetProcessing();

public:
  /// \brief
  ///   Sets an asset library (and optionally a relative path inside that library) to
  ///   be processed by this class.
  ///
  /// If both parameters are NULL (which is also the default when this class is initialized),
  /// all asset libraries will be processed.
  ///
  /// \note
  ///   If any assed is queued for processing, only the assets in the queue will be processed.
  ///   Any libraries (and also the state denoting that all libraries should be processed) will
  ///   be ignored in this case.
  ///
  /// \param library
  ///   the asset library to process (NULL for all libraries)
  /// \param relativePath
  ///   the relative path within the library to be processed. This is only meaningful if the library
  ///   is not NULL.
  ASSETFRAMEWORK_IMPEXP void setPathToProcess(hkvAssetLibrary* library, const char* relativePath = NULL);

  /// \brief
  ///   Queues an asset to be processed by this class.
  ///
  /// Multiple assets can be queued; these assets need not be within the same
  /// asset library. See the documentation to setPathToProcess() about the relation between
  /// assets and libraries queued for processing.
  ///
  /// \param asset
  ///   the asset to queue for processing
  ASSETFRAMEWORK_IMPEXP void addAssetToProcess(hkvAsset& asset);

  /// \brief
  ///   Sets whether transformed assets should be added to RCS.
  ///
  /// \param addTransformed
  ///   \c true to add transformed assets to RCS
  ASSETFRAMEWORK_IMPEXP void setAddTransformedToRCS(bool addTransformed);

  ASSETFRAMEWORK_IMPEXP Status getStatus();

  ASSETFRAMEWORK_IMPEXP void start();
  ASSETFRAMEWORK_IMPEXP void cancel();

  ASSETFRAMEWORK_IMPEXP float estimateProgress();

private:
  static void* HK_CALL threadProcStatic(void* param);
  void threadProc();

  void replaceBackgroundHandling();
  void restoreBackgroundHandling();

  void prepareBackgroundWork();
  void queueLibrary(hkvAssetLibrary& library, const char* relativePath);

private:
  hkRefPtr<hkvBackgroundAssetHandling> m_savedBackgroundHandling;
  hkRefPtr<hkvBackgroundAssetHandling> m_myBackgroundHandling;

  hkThread m_thread;
  hkCriticalSection m_protect;

  bool m_canceled;
  Status m_status;

  hkvAssetLibrary::RefPtr m_library;
  hkStringPtr m_relativePath;
  std::vector<hkvAsset::RefPtr> m_assets;
  bool m_addTransformedToRCS;
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
