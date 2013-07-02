/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetLibrary.hpp

#ifndef HKV_ASSET_LIBRARY_HPP_INCLUDED
#define HKV_ASSET_LIBRARY_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetQuery.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvDummyAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Profiles/hkvProfile.hpp>

#include <vector>
#include <map>
#include <set>
#include <deque>

#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>
#include <Common/Base/Thread/Semaphore/hkSemaphore.h>
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>

class hkBitField;

class hkvAssetManager;
class hkvAssetTrackedDirectory;


class hkvAssetLibraryConfig : public hkvIProperties
{
  friend class hkvAssetLibrary;

public:
  ASSETFRAMEWORK_IMPEXP hkvAssetLibraryConfig();
  ASSETFRAMEWORK_IMPEXP hkvAssetLibraryConfig(const hkvAssetLibraryConfig& config);
  ASSETFRAMEWORK_IMPEXP hkvAssetLibraryConfig(const char* name);

  ASSETFRAMEWORK_IMPEXP void operator=(const hkvAssetLibraryConfig& rhs);

  ASSETFRAMEWORK_IMPEXP const char* getName() const;

  ASSETFRAMEWORK_IMPEXP hkUint32 getFormatVersion() const;
  ASSETFRAMEWORK_IMPEXP void setFormatVersionCurrent();

  ASSETFRAMEWORK_IMPEXP hkFileSystem::TimeStamp getLastEditTime() const;
  ASSETFRAMEWORK_IMPEXP void touch();

  ASSETFRAMEWORK_IMPEXP VOVERRIDE const char* getTypeName() const;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

public:
  ASSETFRAMEWORK_IMPEXP static hkUint32 getCurrentFormatVersion();

private:
  hkStringPtr m_name;
  hkUint32 m_formatVersion;
  hkFileSystem::TimeStamp m_lastEditTime;

private:
  static hkUint32 s_currentFormatVersion; ///< Increment this value for all internal changes that make an update of assets necessary!
};


class hkvAssetLibraryReader : public hkReferencedObject, public hkvPropertyReader
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef hkRefNew<hkvAssetLibraryReader> RefNew;
  typedef hkRefPtr<hkvAssetLibraryReader> RefPtr;
  typedef hkRefPtr<const hkvAssetLibraryReader> RefCPtr;

public:
  hkvAssetLibraryReader() {}
private:
  hkvAssetLibraryReader(const hkvAssetLibraryReader&);
  hkvAssetLibraryReader& operator=(const hkvAssetLibraryReader&);
public:
  ~hkvAssetLibraryReader() {}

public:
  const hkvAssetLibraryConfig& getConfig() const { return m_config; }
  
  hkBool read(const char* filePath);

  VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseString(const unsigned char* pszValue, size_t len);
  VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseMapKey(const unsigned char* pszValue, size_t len);

  bool popFirstAsset(hkStringBuf& out_key, hkvDummyAsset::RefPtr& out_asset);
  bool popSpecificAsset(hkStringBuf& key, hkvDummyAsset::RefPtr& out_asset);

  bool popFirstTrackedFile(hkStringBuf& out_key, hkRefPtr<hkvTrackedFileDummy>& out_file);
  bool popSpecificTrackedFile(hkStringBuf& key, hkRefPtr<hkvTrackedFileDummy>& out_file);

private:
  typedef std::map<const hkStringPtr, hkvDummyAsset::RefPtr, hkvStringSafeLessI<hkStringPtr> > AssetMap;
  typedef std::map<const hkStringPtr, hkRefPtr<hkvTrackedFileDummy>, hkvStringSafeLessI<hkStringPtr> > TrackedFileDummyMap;

private:
  /// Json parser stuff
  hkStringPtr m_loaderPath;

  hkvAssetLibraryConfig m_config;
  AssetMap m_assets;
  TrackedFileDummyMap m_trackedFiles;
};


/// \brief
///   The hkvAssetLibrary class represents a single asset library that was loaded by adding a data directory.
///   The library manages all assets under its relative path. Asset configurations are stored in the .assetlib file
///   in the root of the data directory.
///
/// The asset library also creates the lookup table for the engine that maps assets to actual files.
class hkvAssetLibrary : public hkReferencedObject
{
  friend class hkvAssetManager;
  friend class hkvAssetTrackedFile;

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef hkRefNew<hkvAssetLibrary> RefNew;
  typedef hkRefPtr<hkvAssetLibrary> RefPtr;
  typedef hkRefPtr<const hkvAssetLibrary> RefCPtr;

public:
  ASSETFRAMEWORK_IMPEXP hkvAssetLibrary(const char* libraryPath, bool create, bool isProjectRoot);
  ASSETFRAMEWORK_IMPEXP ~hkvAssetLibrary();

  ASSETFRAMEWORK_IMPEXP hkvCriticalSectionLock acquireLock() const;

  ASSETFRAMEWORK_IMPEXP void clear();
  ASSETFRAMEWORK_IMPEXP hkvAssetLibraryLoadResult loadLibrary(const char* localFileHint = NULL);
  ASSETFRAMEWORK_IMPEXP bool saveLibrary(const char* targetFile = NULL);

  ASSETFRAMEWORK_IMPEXP const char* getName() const;
  ASSETFRAMEWORK_IMPEXP void setName(const char* name);

  // Returns the absolute path to the asset library.
  ASSETFRAMEWORK_IMPEXP const char* getPath() const;
  
  ASSETFRAMEWORK_IMPEXP void getRelativeAssetCachePath(hkStringBuf& out_path) const;
  ASSETFRAMEWORK_IMPEXP void getAbsoluteAssetCachePath(hkStringBuf& out_path) const;
  ASSETFRAMEWORK_IMPEXP void getRelativeThumbnailCachePath(hkStringBuf& out_path) const;
  ASSETFRAMEWORK_IMPEXP void getAbsoluteThumbnailCachePath(hkStringBuf& out_path) const;

  ASSETFRAMEWORK_IMPEXP bool isDirty() const { return m_isDirty; }
  ASSETFRAMEWORK_IMPEXP bool isLoading() const  { return m_isLoading; }

  ASSETFRAMEWORK_IMPEXP bool isProjectRoot() const  { return m_isProjectRoot; }
  ASSETFRAMEWORK_IMPEXP void setProjectRoot(bool root) { m_isProjectRoot = root; }

  // Asset Handling
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumberOfAssets() const;
  ASSETFRAMEWORK_IMPEXP hkvAsset::RefPtr getAssetByIndex(hkUint32 assetIndex) const;

  /// \brief Searches for an asset which was created from the given file. Returns NULL if nothing could be found.
  ASSETFRAMEWORK_IMPEXP hkvAsset::RefPtr getAssetByPath(const char* assetPath) const;

  ASSETFRAMEWORK_IMPEXP hkvAssetTrackedFile* getTrackedFileByPath(const char* relativePath) const;
  ASSETFRAMEWORK_IMPEXP hkvAssetTrackedFile* createMissingTrackedFile(const char* relativePath);

  ASSETFRAMEWORK_IMPEXP void finalizeDependencies();

  /// \brief
  ///   Changes the modification time of this library and all assets contained therein by the specified number
  ///   of milliseconds.
  /// \param offsetMs
  ///   the offset in milliseconds.
  ASSETFRAMEWORK_IMPEXP void changeModificationTimes(int offsetMs);

  // Queries
  ASSETFRAMEWORK_IMPEXP void query(hkvAssetQuery& query) const;

  // Inventory management
  ASSETFRAMEWORK_IMPEXP hkvAssetTrackedDirectory& getRootDirectory();
  ASSETFRAMEWORK_IMPEXP const hkArray<hkStringPtr>& getIgnoreList() const;

  ASSETFRAMEWORK_IMPEXP void invalidateAssetTransforms();
  ASSETFRAMEWORK_IMPEXP void setAssetNeedsTransformation(hkvAsset& asset, bool needsTransformation);
  ASSETFRAMEWORK_IMPEXP void setAssetNeedsTransformation(hkUint32 assetIndex, bool needsTransformation);
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumberOfAssetsNeedingTransformation() const;
  ASSETFRAMEWORK_IMPEXP bool getAssetNeedsTransformation(hkvAsset& asset) const;
  ASSETFRAMEWORK_IMPEXP bool getAssetNeedsTransformation(hkUint32 assetIndex) const;

  /// \brief 
  ///   Cleans up stale asset files in this library's asset cache.
  /// \return
  ///   the number of stale files deleted, or -1, if an error occurred
  ASSETFRAMEWORK_IMPEXP hkInt32 cleanUpAssetCache();

  /// \brief Generates the profile specific lookup table and returns a list of all entries.
  ASSETFRAMEWORK_IMPEXP /*const std::vector< std::vector<hkvLookupTableEntry> >&*/ void generateLookupTable(hkvProfile::RefCPtr profile, bool forEditor, bool assetPreviewMode) const;
  /// \brief Generates the profile specific lookup table and returns all entries concatenated into one large string.
  ASSETFRAMEWORK_IMPEXP const char* generateLookupTable(hkvProfile::RefCPtr profile, bool forEditor, bool assetPreviewMode, hkStringBuf& out_buffer) const;

  ASSETFRAMEWORK_IMPEXP bool writeLookupTable(hkvProfile::RefCPtr profile, bool forEditor, bool assetPreviewMode, hkStringPtr* out_fileName = NULL) const;

  ASSETFRAMEWORK_IMPEXP void invalidateLookupTableEntry(hkvAsset* assetPtr) const;

private:
  hkvAssetLibraryLoadResult loadLibraryInternal(const std::vector<hkStringPtr>& libraryFiles, const char* localFileHint);
  void useMostRecentConfiguration(const hkArray<hkvAssetLibraryReader::RefPtr>& libraryReaders);
  void useMostRecentAssets(const hkArray<hkvAssetLibraryReader::RefPtr>& libraryReaders, int localFile);
  void useMostRecentTrackedFiles(const hkArray<hkvAssetLibraryReader::RefPtr>& libraryReaders, int localFile);

  hkvAsset::RefPtr addAssetFromDummy(hkvDummyAsset& dummyAsset);
  void addAssetToLibrary(hkvAsset& asset);
  void removeAssetFromLibrary(hkvAsset& asset);
  void removeAssetFromLibrary(int index);
  void clearLutEntry(int index);

  bool generateTransformedAssetFileNames(hkArray<hkStringPtr>& out_names);
  bool generateTransformedAssetFileNames(const hkvProfile& profile, hkArray<hkStringPtr>& out_names);
  bool generateAssetCacheExistingFilesList(hkArray<hkStringPtr>& out_files);
  bool generateAssetCacheStaleFilesList(hkArray<hkStringPtr>& out_files);

  struct AssetMapComp {
    bool operator() (const char* const lhs, const char* const rhs) const
    {
      return hkvStringHelper::safeCompare(lhs, rhs, true) < 0;
    }
  };

  bool m_isProjectRoot;
  bool m_createLibrary;
  bool m_isLoaded;
  bool m_isDirty;
  bool m_isLoading;

  hkvAssetLibraryConfig m_config;

  hkStringPtr m_absolutePath;
  hkStringPtr m_localFileName;
  hkUint32 m_libraryIndex; ///< Set by the hkvAssetManager

  hkArray<hkvAsset::RefPtr> m_assets;
  hkBitField m_assetsNeedingTransform;

  hkRefPtr<hkvAssetTrackedDirectory> m_rootDirectory;
  hkArray<hkStringPtr> m_ignoreList;


  // LUT caching
  mutable hkStringPtr m_lutEntriesProfile;
  mutable std::vector< std::vector<hkvLookupTableEntry> > m_lutEntriesEngine;
  mutable std::vector< std::vector<std::string> > m_lutStringsEngine;
  mutable hkBitField m_validLutEntriesEngine;

  mutable std::vector< std::vector<hkvLookupTableEntry> > m_lutEntriesEditor;
  mutable std::vector< std::vector<std::string> > m_lutStringsEditor;
  mutable hkBitField m_validLutEntriesEditor;
  mutable bool m_LutEntriesEditorAssetPreviewMode;

public:
  ASSETFRAMEWORK_IMPEXP static const char* s_assetDataDirectory;
  ASSETFRAMEWORK_IMPEXP static const char* s_assetCacheDirectory;
  ASSETFRAMEWORK_IMPEXP static const char* s_thumbnailCacheDirectory;
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
