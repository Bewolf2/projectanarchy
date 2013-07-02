/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef _HKV_TRACKED_FILE_HPP_
#define _HKV_TRACKED_FILE_HPP_

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>

#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

#include <set>
#include <vector>

class hkvTrackedDirectory;

class hkvTrackedFile : public hkReferencedObject, public hkvIProperties
{
public:
  friend class hkvTrackedDirectory;

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef std::vector<hkRefPtr<hkvTrackedFile> > TrackedFileList;
  typedef std::set<hkRefPtr<hkvTrackedFile> > TrackedFileSet;

protected:
  ASSETFRAMEWORK_IMPEXP hkvTrackedFile(hkvTrackedDirectory& parent, const char* name);
private:
  hkvTrackedFile(const hkvTrackedFile&);
  hkvTrackedFile& operator=(const hkvTrackedFile&);
public:
  ASSETFRAMEWORK_IMPEXP virtual ~hkvTrackedFile() = 0;

public:
  static int getNumInstances() { return s_numInstances; }

public:
  ASSETFRAMEWORK_IMPEXP hkvTrackedDirectory* getParent() const;
  ASSETFRAMEWORK_IMPEXP const char* getName() const;

  ASSETFRAMEWORK_IMPEXP const char* getRelativePath(hkStringBuf& out_path) const;
  ASSETFRAMEWORK_IMPEXP const char* getAbsolutePath(hkStringBuf& out_path) const;

  ASSETFRAMEWORK_IMPEXP bool rename(const char* newName);
  ASSETFRAMEWORK_IMPEXP bool remove(bool force);
  ASSETFRAMEWORK_IMPEXP void clear();

  ASSETFRAMEWORK_IMPEXP void touch();
  ASSETFRAMEWORK_IMPEXP hkFileSystem::TimeStamp getLastEditTime() const;

  ASSETFRAMEWORK_IMPEXP bool isChanged() const;
  ASSETFRAMEWORK_IMPEXP void setChanged(bool changed, hkFileSystem::TimeStamp newTimestamp = hkFileSystem::TimeStamp::TIMESTAMP_UNAVAILABLE);

  ASSETFRAMEWORK_IMPEXP bool hasFile() const;
  ASSETFRAMEWORK_IMPEXP void setHasFile(bool hasFile, hkFileSystem::TimeStamp newTimestamp = hkFileSystem::TimeStamp::TIMESTAMP_UNAVAILABLE);

  ASSETFRAMEWORK_IMPEXP bool isOrphaned() const;

  ASSETFRAMEWORK_IMPEXP hkUint32 getStoredHash() const;
  ASSETFRAMEWORK_IMPEXP hkFileSystem::TimeStamp getStoredTimeStamp() const;

  ASSETFRAMEWORK_IMPEXP hkvAssetOperationResult updateFileState();
  ASSETFRAMEWORK_IMPEXP hkvAssetOperationResult updateFileState(hkStreamReader& reader, const hkFileSystem::Entry& fileEntry);
  ASSETFRAMEWORK_IMPEXP void clearStoredFileState();

  ASSETFRAMEWORK_IMPEXP hkRefPtr<hkStreamReader> openReader() const;

  ASSETFRAMEWORK_IMPEXP void addDependency(hkvTrackedFile& dependency);
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumDependencies() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumDependentFiles() const;
  ASSETFRAMEWORK_IMPEXP void removeDependency(hkvTrackedFile& dependency);
  ASSETFRAMEWORK_IMPEXP void removeAllDependencies();
  ASSETFRAMEWORK_IMPEXP void updateDependencies(const TrackedFileSet& incoming);
  ASSETFRAMEWORK_IMPEXP void makeDependencyPathList(std::vector<hkStringPtr>& out_list) const;

  ASSETFRAMEWORK_IMPEXP void resetSeen();
  ASSETFRAMEWORK_IMPEXP void setSeen();
  ASSETFRAMEWORK_IMPEXP bool wasSeen() const;

  ASSETFRAMEWORK_IMPEXP virtual const char* getTypeName() const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP virtual void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP virtual void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

protected:
  ///
  /// \return
  ///   true if further processing is being done and the file should remain in modified state. False
  ///   to reset the modified state of this file immediately.
  virtual bool onChanged() { return false; }
  virtual void onRemove() {}

  virtual hkvAssetOperationResult onUpdateData(hkStreamReader& reader) { return HKV_AOR_SUCCESS; }
  virtual void onClearData() {};

  virtual bool canRemove();

  virtual hkvCriticalSectionLock acquireDependencyLock() const = 0;

private:
  void callOnChanged(const hkFileSystem::TimeStamp timeStamp);
  
  void removeFromDependency(hkvTrackedFile& dependency);
  void removeFromDependentFiles();

private:
  hkRefPtr<hkvTrackedDirectory> m_parent;
  hkStringPtr m_name;

  hkUint32 m_fileHash;
  hkFileSystem::TimeStamp m_fileTimeStamp;

  hkFileSystem::TimeStamp m_lastEditTime;

  TrackedFileSet m_dependencies; ///< The files on which this one depends
  TrackedFileSet m_dependentFiles; ///< The files that depend on this files

  bool m_changed;
  mutable bool m_hasFile;
  bool m_seen;

private:
  static int s_numInstances;
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
