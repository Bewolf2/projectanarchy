/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_TRACKED_DIRECTORY_HPP
#define HKV_TRACKED_DIRECTORY_HPP

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>

#include <Common/Base/Container/Array/hkArray.h>
#include <Common/Base/Object/hkReferencedObject.h>

#include <map>
#include <vector>

class hkvTrackedFile;
class hkvTrackedTreeInfo;


class hkvTrackedDirectory : public hkReferencedObject
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  friend class hkvTrackedFile;

public:
  typedef std::vector<hkRefPtr<hkvTrackedFile> > FileList;
  typedef std::vector<hkRefPtr<hkvTrackedDirectory> > DirectoryList;

private:
  typedef std::map<const char*, hkRefPtr<hkvTrackedFile>, hkvStringSafeLessI<const char*> > FileMap;
  typedef std::map<const char*, hkRefPtr<hkvTrackedDirectory>, hkvStringSafeLessI<const char*> > DirectoryMap;

protected:
  hkvTrackedDirectory(hkvTrackedTreeInfo& treeInfo, hkvTrackedDirectory* parent, const char* name);
private:
  hkvTrackedDirectory(const hkvTrackedDirectory&);
  hkvTrackedDirectory& operator=(const hkvTrackedDirectory&);
public:
  virtual ~hkvTrackedDirectory();

public:
  static int getNumInstances() { return s_numInstances; }

public:
  ASSETFRAMEWORK_IMPEXP const char* getAbsolutePath(hkStringBuf& out_path) const;
  ASSETFRAMEWORK_IMPEXP const char* getRelativePath(hkStringBuf& out_path) const;

  ASSETFRAMEWORK_IMPEXP const hkvTrackedDirectory* resolvePath(const char* relativePath) const;
  ASSETFRAMEWORK_IMPEXP hkvTrackedDirectory* resolvePath(const char* relativePath, bool fixCasing, bool createMissing);

  ASSETFRAMEWORK_IMPEXP const hkvTrackedFile* resolveFile(const char* relativePath) const;
  ASSETFRAMEWORK_IMPEXP hkvTrackedFile* resolveFile(const char* relativePath, bool fixCasing, bool createMissing);

  ASSETFRAMEWORK_IMPEXP const char* getName() const;
  ASSETFRAMEWORK_IMPEXP virtual const char* getDisplayName() const;
  ASSETFRAMEWORK_IMPEXP hkvTrackedDirectory* getParent() const;
  ASSETFRAMEWORK_IMPEXP hkvTrackedTreeInfo& getTreeInfo() const;

  ASSETFRAMEWORK_IMPEXP void getDirectories(DirectoryList& out_directories) const;
  ASSETFRAMEWORK_IMPEXP void getFiles(FileList& out_files) const;

  template<typename Function>
  bool forEachFile(Function& function) const;

  ASSETFRAMEWORK_IMPEXP bool isEmpty() const;

  ASSETFRAMEWORK_IMPEXP bool rename(const char* newName);
  ASSETFRAMEWORK_IMPEXP bool remove(bool force);

  // Recurse means upwards in the tree - remove empty folders until we encounter
  // the root or a non-empty folder.
  ASSETFRAMEWORK_IMPEXP bool removeIfEmpty(bool recurse);

  // Only call this when clearing all directories - this method does not unhook dependencies.
  ASSETFRAMEWORK_IMPEXP void clear(); 

protected:
  virtual hkvTrackedDirectory* createDirectory(const char* name) = 0;
  virtual hkvTrackedFile* createFile(const char* name) = 0;

private:
  bool renameFile(hkvTrackedFile& file, const char* newName);
  void fileRemoved(hkvTrackedFile& file);

  bool renameDirectory(hkvTrackedDirectory& directory, const char* newName);
  void directoryRemoved(hkvTrackedDirectory& directory);

private:
  hkStringPtr m_name;
  hkRefPtr<hkvTrackedDirectory> m_parent;
  hkRefPtr<hkvTrackedTreeInfo> m_treeInfo;

  DirectoryMap m_directories;
  FileMap m_files;

private:
  static int s_numInstances;
};


class hkvTrackedTreeInfo : public hkReferencedObject
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

protected:
  hkvTrackedTreeInfo();
private:
  hkvTrackedTreeInfo(const hkvTrackedTreeInfo&);
  hkvTrackedTreeInfo& operator=(const hkvTrackedTreeInfo&);
public:
  virtual ~hkvTrackedTreeInfo();

public:
  virtual hkvCriticalSectionLock acquireLock() = 0;

  virtual bool canApplyChanges() const = 0;
  virtual bool canChangeStructure() const = 0;

  virtual bool shouldIgnorePath(const char* path) const = 0;

  void deferUpdateNotifications();
  void releaseUpdateNotifications();

  void sendUpdateNotification();
  void clearPendingUpdateNotifications();

protected:
  virtual void doSendUpdateNotification() = 0;

private:
  int m_updateNotifyDeferredLevel;
  bool m_havePendingUpdateNotifications;
};


template<typename Function>
bool hkvTrackedDirectory::forEachFile(Function& function) const
{
  for (FileMap::const_iterator iter = m_files.begin(); iter != m_files.end(); ++iter)
  {
    if (!function(*iter->second))
    {
      return false;
    }
  }
  for (DirectoryMap::const_iterator iter = m_directories.begin(); iter != m_directories.end(); ++iter)
  {
    if (!iter->second->forEachFile(function))
    {
      return false;
    }
  }
  return true;
}


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
