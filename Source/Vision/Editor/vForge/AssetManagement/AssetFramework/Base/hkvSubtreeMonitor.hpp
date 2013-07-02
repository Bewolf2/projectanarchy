/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_SUBTREE_MONITOR_HPP
#define HKV_SUBTREE_MONITOR_HPP

#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

#include <deque>
#include <vector>

class hkvTrackedDirectory;


class hkvSubtreeMonitor
{
public:
  class SubtreeEntry;
  class FileSystemChange;
  class DeferUpdates;

private:
  typedef std::vector<hkFileSystem::Entry> FileEntryList;
  typedef std::vector<SubtreeEntry*> SubtreeEntryList;

public:
  class DirScanEntry
  {
  public:
    DirScanEntry(SubtreeEntry& subtree, const char* relativePath);

  public:
    SubtreeEntry& getSubtreeEntry();
    const char* getRelativePath();

  private:
    SubtreeEntry* m_subtreeEntry;
    hkStringPtr m_relativePath;
  };

public:
  hkvSubtreeMonitor();
private:
  hkvSubtreeMonitor(const hkvSubtreeMonitor&);
  hkvSubtreeMonitor& operator=(const hkvSubtreeMonitor&);
public:
  ~hkvSubtreeMonitor();


public:
  void start(bool scanAllSubtreesNow = false);
  void stop(bool keepCurrentWork);

  bool isRunning();
  bool join(DWORD timeout);

  void addSubtree(hkvTrackedDirectory& rootDirectory, bool monitorChanges, float scanIntervalSeconds);
  void clear();

  void queueScan(hkvTrackedDirectory& rootDirectory, const char* relativePath);

  void queryProgress(hkUint32& out_scansRemaining, hkUint32& out_changesRemaining);

protected:
  void queueScan(SubtreeEntry& subtree, const char* relativePath);

private:
  static DWORD WINAPI monitorThreadProcStatic(LPVOID data);
  void monitorThreadProc();

  void initMonitoring();
  void monitorLoop();
  void deinitMonitoring();

  void handleSignaledSubtree(int subtreeIndex);
  void handleTimedScans();
  void handleApplyResults();

  static DWORD WINAPI scanThreadProcStatic(LPVOID data);
  void scanThreadProc();
  void scanThreadWork();
  bool handleSingleScanJob();

  void clearSubtreeChanges();

private:
  // General runtime behavior
  bool m_keepCurrentWorkOnStop;

  // Directory Monitoring
  HANDLE m_monitorThread;
  HANDLE m_monitorThreadNotify;
  hkCriticalSection m_monitorProtect;
  hkFileSystem::TimeStamp m_lastMonitorLoopTime;
  SubtreeEntryList m_monitoredSubtrees;
  bool m_monitorThreadShouldExit;

  // Directory scanning
  HANDLE m_scanThread;
  HANDLE m_scanThreadNotify;
  hkCriticalSection m_scanProtect;
  std::deque<DirScanEntry> m_scanQueue;
  bool m_scanThreadShouldExit;
  bool m_busyScanning;

  // Change processing
  float m_minChangeApplyIntervalSeconds;
  hkFileSystem::TimeStamp m_lastApplyChangesTime;
};


class hkvSubtreeMonitor::SubtreeEntry
{
private:
  typedef std::vector<FileSystemChange> FileSystemChangeList;

  enum Constants {
    NotificationBufferSize = 64*1024
  };
  
public:
  SubtreeEntry(hkvSubtreeMonitor& owner, hkvTrackedDirectory& rootDirectory, 
    bool monitorChanges, float scanIntervalSeconds);
private:
  SubtreeEntry(const SubtreeEntry&);
  SubtreeEntry& operator=(const SubtreeEntry&);
public:
  ~SubtreeEntry();

public:
  bool initMonitoring();
  void deinitMonitoring();

  bool startMonitoring();
  void stopMonitoring();

  bool isMonitoringInitialized();

  hkvTrackedDirectory& getRootDirectory();
  HANDLE getWaitHandle();

  bool processMonitoringResults();
  void processScanResults(const char* relativePath, const FileEntryList& foundFiles);

  void applyTimeDifference(float differenceSeconds);
  float getSecondsToNextScan() const;
  void resetTimeToNextScan();

  bool hasChanges() const;
  bool applyChanges();
  void clearChanges();
  size_t getNumChanges();

private:
  void applyDirectoryCreated(const FileSystemChange& change);
  void applyDirectoryRenamed(const FileSystemChange& change);
  void applyFileCreatedOrModified(const FileSystemChange& change);
  void applyFileOrDirectoryDeleted(const FileSystemChange& change);
  void applyFileRenamed(const FileSystemChange& change);

private:
  hkvSubtreeMonitor& m_owner;

  // Config data
  hkRefPtr<hkvTrackedDirectory> m_rootDirectory;
  float m_scanIntervalSeconds;

  // Runtime data
  HANDLE m_monitoredDirectory;
  HANDLE m_waitEvent;
  hkArray<hkUint8> m_notificationBuffer;
  OVERLAPPED m_overlapped;
  float m_secondsToNextScan;
  FileSystemChangeList m_changes;
  hkCriticalSection m_changesProtect;
};


class hkvSubtreeMonitor::FileSystemChange
{
public:
  enum ChangeType
  {
    ChangeUnspecified,
    ChangeCreated,
    ChangeModified,
    ChangeRenamed,
    ChangeDeleted
  };

  enum ObjectType
  {
    ObjectUnknown,
    ObjectDirectory,
    ObjectFile
  };

public:
  FileSystemChange(ObjectType objectType, ChangeType changeType, const char* firstPath, 
    const char* secondPath, const hkFileSystem::TimeStamp& timeStamp);

  ObjectType getObjectType() const;
  ChangeType getChangeType() const;
  const char* getFirstPath() const;
  const char* getSecondPath() const;
  const hkFileSystem::TimeStamp& getTimeStamp() const;

  static ChangeType changeTypeFromNotificationAction(DWORD action);

private:
  ObjectType m_objectType;
  ChangeType m_changeType;
  hkStringPtr m_firstPath;
  hkStringPtr m_secondPath;
  hkFileSystem::TimeStamp m_timeStamp;
};


class hkvSubtreeMonitor::DeferUpdates
{
public:
  DeferUpdates(const SubtreeEntryList& subtrees);
private:
  DeferUpdates(const DeferUpdates&);
  DeferUpdates& operator=(const DeferUpdates&);
public:
  ~DeferUpdates();

private:
  const SubtreeEntryList& m_subtrees;
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
