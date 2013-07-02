/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VResourceSnapshot.hpp

#ifndef _VRESOURCESNAPSHOT_HPP_INCLUDED
#define _VRESOURCESNAPSHOT_HPP_INCLUDED

class TiXmlElement;
class IVResourceCreator;
class VResourceSnapshotQueue;

/// \brief
///   Class that represents a single entry inside the VResourceSnapshot class
/// 
/// This class has public members that describe the resource, e.g. filename and resource manager
/// pool name.
/// 
/// \sa VResourceSnapshot
class VResourceSnapshotEntry
{
public:

  /// \brief
  ///   Constructor
  VResourceSnapshotEntry(VResourceSnapshot *pOwner=NULL)
  {
    m_pOwner = pOwner;
    m_pManager = NULL;
    m_fPriority = 0.f;
    m_iDependRefs = m_iFileSize = 0;
    m_iRawDataEntry = -1;
    m_iOwnerResourceEntryIndex = -1;
  }

  virtual ~VResourceSnapshotEntry() {}

  /// \brief
  ///   Clears all information in this entry (internal use)
  VBASE_IMPEXP virtual void Reset()
  {
    m_pManager = NULL;
    m_spResource = NULL;
    m_iRawDataEntry = -1;
  }

  /// \brief
  ///   Returns the name of the manager. Returns NULL if no manager is specified.
  VBASE_IMPEXP virtual const char* GetManagerName() const = 0;

  /// \brief
  ///   Returns the file name of the resource. Returns NULL if no name is specified.
  VBASE_IMPEXP virtual const char* GetFileName() const = 0;

  /// \brief
  ///   Internally used when building snapshots
  VBASE_IMPEXP virtual void SetManagerName(const char *szName) = 0;

  /// \brief
  ///   Internally used when building snapshots
  VBASE_IMPEXP virtual void SetFileName(const char *szName) = 0;

  /// \brief
  ///   Associates a block of binary data with this entry
  VBASE_IMPEXP void SetBinaryBlock(const void *pData, int iSize);

  /// \brief
  ///   Returns the binary data that is associated with his entry
  VBASE_IMPEXP const void* GetBinaryBlock(int &iSize) const;

  /// \brief
  ///   Indicates whether this entry represents a resource
  inline bool IsResource() const
  {
    return (m_spResource!=NULL) || (GetManagerName()!=NULL);
  }

  /// \brief
  ///   Indicates whether this entry represents a simple file
  inline bool IsFile() const
  {
    return GetManagerName()==NULL;
  }

  /// \brief
  ///   Indicates whether this entry has a binary block for low-res replacement creation
  inline bool HasReplacementData() const
  {
    return m_iRawDataEntry>=0;
  }

  /// \brief
  ///   Associates custom int values with this entry. Can be used to store extra information per
  ///   entry (e.g. texture type, creation flags...)
  /// 
  /// There can be up to 4 values for one entry. By default, all custom values are set to 0
  /// 
  /// These values are stored along with the file, so they are available at loading time and can be
  /// used to re-create the resource.
  /// 
  /// \param iIndex
  ///   Index to set (must be 0..3)
  /// 
  /// \param iValue
  ///   Custom arbitrary Value to set at index.
  VBASE_IMPEXP virtual void SetCustomIntValue(int iIndex, int iValue) = 0;

  /// \brief See SetCustomIntValue() documentation.
  VBASE_IMPEXP virtual void SetCustomIntValues(int *piList) = 0;

  /// \brief
  ///   Returns the custom int value at specified index. A default value can be passed in case the
  ///   index is larger than the available indices.
  VBASE_IMPEXP virtual int GetCustomIntValue(int iIndex, int iDefaultValue=0) const = 0;

  /// \brief
  ///   Returns the number of custom int values
  VBASE_IMPEXP virtual int GetCustomIntValueCount() const = 0;


  /// \brief
  ///   Assignment operator
  inline const VResourceSnapshotEntry& operator= (const VResourceSnapshotEntry& other)
  {
    (*this).CopyFrom(other);
    return *this;
  }

  /// \brief
  ///   Copies the data from \a other into this object.
  inline void CopyFrom(const VResourceSnapshotEntry &other, IVFilePathResolver* pResolver = NULL)
  {
    char szBuffer[1024];
    m_pOwner = other.m_pOwner;
    m_spResource = other.m_spResource;
    m_pManager = other.m_pManager;
    m_fPriority = other.m_fPriority;
    m_iDependRefs = other.m_iDependRefs;
    m_iFileSize = other.m_iFileSize;
    m_iRawDataEntry = other.m_iRawDataEntry;
    m_iOwnerResourceEntryIndex = other.m_iOwnerResourceEntryIndex;

    if (pResolver!=NULL)
      SetFileName(pResolver->ResolvePath(other.GetFileName(),szBuffer));
    else
      SetFileName(other.GetFileName());
    SetManagerName(other.GetManagerName());
    int iCount = other.GetCustomIntValueCount();
    VMemoryTempBuffer<1024> buffer((iCount+1)*sizeof(int));
    int *pInt = (int *)buffer.GetBuffer();
    pInt[0] = iCount;
    for (int i=0;i<iCount;i++) 
      pInt[i+1] = other.GetCustomIntValue(i);
    SetCustomIntValues(pInt);

    int iRawSize;
    const void *pRaw = other.GetBinaryBlock(iRawSize);
    SetBinaryBlock(pRaw,iRawSize);
  }


  /// \brief
  ///   Helper function to evaluate the resource manager's name
  VBASE_IMPEXP const char *GetFinalManagerName() const;

  /// \brief
  ///   Returns a non-NULL string pointer
  inline const char *GetFileNameSafe() const
  {
    const char *szFilename = GetFileName();
    if (szFilename==NULL)
      return "";
    return szFilename;
  }

  VResourceSnapshot *m_pOwner;
  VManagedResourcePtr m_spResource; ///< smart pointer to the resource
  VResourceManager *m_pManager;     ///< Manager that the resource is bound to. This member is usually NULL but can be used by implementations to cache the manager that has been looked up by name
  float m_fPriority;                ///< not yet used
  int m_iDependRefs;                ///< internal counter that counts the dependencies for this resource
  int m_iFileSize;                  ///< rough size value used to weighten the percentage progress when loading
  int m_iRawDataEntry;              ///< entry handle in raw data block
  short m_iOwnerResourceEntryIndex; ///< only for file types
};


/// \brief
///   Internal class that implements a pure virtual VResourceSnapshotEntry interface.
class VResourceSnapshotEntryXML : public VResourceSnapshotEntry
{
public:

  inline VResourceSnapshotEntryXML(VResourceSnapshot *pOwner=NULL)
  {
    m_pOwner = pOwner;
    m_iCustomValueCount = 0;
    m_iCustomValue[0] = m_iCustomValue[1] = m_iCustomValue[2] = m_iCustomValue[3] = 0;
  }

  virtual ~VResourceSnapshotEntryXML() {}

  VBASE_IMPEXP virtual void Reset();
  VBASE_IMPEXP virtual void SetManagerName(const char *szName);
  VBASE_IMPEXP virtual const char* GetManagerName() const;
  VBASE_IMPEXP virtual void SetFileName(const char *szName);
  VBASE_IMPEXP virtual const char* GetFileName() const;
  VBASE_IMPEXP virtual void SetCustomIntValue(int iIndex, int iValue);
  VBASE_IMPEXP virtual int GetCustomIntValue(int iIndex, int iDefaultValue=0) const;
  VBASE_IMPEXP virtual int GetCustomIntValueCount() const;
  VBASE_IMPEXP virtual void SetCustomIntValues(int *piList);

  /// \brief
  ///   Internal function to read/write information from/to XML.
  VBASE_IMPEXP void XMLExchange(VResourceSnapshot &snapshot, TiXmlElement *pNode, bool bWrite, IVFilePathResolver *pResolver);

private:
  friend class VResourceSnapshot;
  VStaticString<270> m_sFilename;   ///< Filename of the resource (only for loading). Since filenames are project relative, use reasonable size here rather than FS_MAX_PATH.
  VStaticString<64> m_sResourceManagerName; ///< Resource manager pool name (VResourceManager::GetManagerName(), only for loading).
  short m_iCustomValueCount;
  short m_iOrigIndex;   // temp value
  int m_iCustomValue[4];
};


/// \brief
///   Internal class that implements pure virtual VResourceSnapshotEntry interface.
class VResourceSnapshotEntryRetail : public VResourceSnapshotEntry
{
public:

  inline VResourceSnapshotEntryRetail(VResourceSnapshot *pOwner=NULL)
  {
    m_pOwner = pOwner;
    m_iNameHandle = m_iManagerHandle = m_iCustomIntHandle = -1;
  }

  virtual ~VResourceSnapshotEntryRetail(){}

  VBASE_IMPEXP virtual void SetManagerName(const char *szName);
  VBASE_IMPEXP virtual const char* GetManagerName() const;
  VBASE_IMPEXP virtual void SetFileName(const char *szName);
  VBASE_IMPEXP virtual const char* GetFileName() const;
  VBASE_IMPEXP virtual void SetCustomIntValue(int iIndex, int iValue);
  VBASE_IMPEXP virtual int GetCustomIntValue(int iIndex, int iDefaultValue=0) const;
  VBASE_IMPEXP virtual int GetCustomIntValueCount() const;
  VBASE_IMPEXP virtual void SetCustomIntValues(int *piList);

  /// \brief
  ///   Internal function to read/write information from/to binary file.
  VBASE_IMPEXP void ChunkFileExchange(VResourceSnapshot &snapshot, VChunkFile &file);

private:
  int m_iNameHandle, m_iManagerHandle, m_iCustomIntHandle;
};


/// \brief
///   Class that represents a graph of resource dependencies. This class can be used for resource
///   streaming.
/// 
/// An instance of this class can be passed to the VManagedResource::GetDependencies resp.
/// VTypedObject::GetDependencies function to gather relvant resource information.
/// 
/// Once this list is built, it can be saved to file.
/// 
/// This class provides functionality to pre-cache resources in the order they are needed (for
/// instance models rely on textures) using a background thread for loading.
/// 
/// \sa VManagedResource::GetDependencies
/// \sa VTypedObject::GetDependencies
/// \sa VResourceSnapshotEntry
class VResourceSnapshot : public VBaseObject
{
public:

  /// \brief
  ///   Enum that determines the directory type of all resources inside this graph
  enum VFilePathType_e
  {
    PATHTYPE_PROJECT,   ///< filenames are project-relative
    PATHTYPE_RELATIVE   ///< filenames are relative to some owner (e.g. model directory)
  };

  /// \brief
  ///   Enum that defines distinct usage types for snapshots
  enum VUsageType_e
  {
    VSNAPSHOTTYPE_EXPORT_STREAMING = 1, ///< the snapshot wants all resources that are relevant for streaming
    VSNAPSHOTTYPE_DEPENDENT_MEMORY = 2  ///< the snapshot wants all resources to evaluate memory dependencies
  };

  ///
  /// @name Init / Deinit
  /// @{
  ///

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VResourceSnapshot(VUsageType_e usage=VSNAPSHOTTYPE_EXPORT_STREAMING);

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VResourceSnapshot();

  /// \brief
  ///   Resets the snapshot and frees all elements. If the snapshot is attached to a queue (class VResourceSnapshotQueue), it will be removed from it.
  VBASE_IMPEXP void Reset();


  /// \brief
  ///   Defines the usage of this snapshot which might influence which type of resources are added
  /// 
  /// All GetDependencies functions should query this type from the passed snapshot and add
  /// relevant resources.
  inline VUsageType_e GetUsageType() const
  {
    return m_eSnapshotUsage;
  }

  ///
  /// @}
  ///

  ///
  /// @name Resource Dependencies
  /// @{
  ///

#ifdef SUPPORTS_SNAPSHOT_CREATION

  /// \brief
  ///   Adds a resource dependency to this list
  /// 
  /// A dependency might consist of a pair, i.e. one resource that should be added and one resource
  /// that it depends on.
  /// 
  /// Both resources are uniquely added to the list.
  /// 
  /// Typically this function is called multiple times for the same pThisRes resource to add all
  /// resources it depends on.
  /// 
  /// For example, a model resource (pThisRes) calls this function for every texture resource
  /// (pDependsOnRes) that it finds in all its materials.
  /// 
  /// \param pThisRes
  ///   The resource that should be added to the list.
  /// 
  /// \param pDependsOnRes
  ///   If !=NULL, this resource will also be added to the list and additionally increases its
  ///   "dependency counter" which means it must be higher up in the list than pThisRes.
  /// 
  /// \param bRecurse
  ///   If true (and pDependsOnRes!=NULL) then this function recursively calls
  ///   pDependsOnRes->GetDependencies(*this). It however does not recurse on pThisRes since
  ///   pThisRes->GetDependencies is the function call where we come from.
  /// 
  /// \sa VManagedResource::GetDependencies
  /// \sa VTypedObject::GetDependencies
  VBASE_IMPEXP void AddDependency(VManagedResource *pThisRes, VManagedResource *pDependsOnRes, bool bRecurse=true);

  /// \brief
  ///   Adds a file dependency to this list
  /// 
  /// The first parameter is a valid resource and the second one is a filename of a file that needs
  /// to be pre-cached so the resource loads properly. 
  /// 
  /// As an example, a model resource depends on its external material.xml file
  /// 
  /// \param pThisRes
  ///   The resource that should be added to the list. Can be a NULL pointer to add a plain file to the snapshot
  /// 
  /// \param szFilename
  ///   Filename of the external file. The path must be project relative.
  /// 
  /// \param iFileSize
  ///   Defines the file size of the external file in bytes. It is not important that this value is
  ///   correct, instead it is just used for statistics
  VBASE_IMPEXP void AddFileDependency(VManagedResource *pThisRes, const char *szFilename, int iFileSize);

  /// \brief
  ///   Internal function
  VBASE_IMPEXP VResourceSnapshotEntry& AppendEntry();

#endif // SUPPORTS_SNAPSHOT_CREATION

  /// \brief
  ///   Looks up a list entry by resource pointer and returns the entry (or NULL if the resource is
  ///   not in the list yet)
  VBASE_IMPEXP VResourceSnapshotEntry* FindResourceEntry(VManagedResource *pRes) const;

  /// \brief
  ///   Looks up a list entry by filename string comparison and returns the entry (or NULL if the
  ///   filename is not in the list yet)
  VBASE_IMPEXP VResourceSnapshotEntry* FindFileEntry(const char *szFilename) const;

  /// \brief
  ///   Looks up a list entry by the properties specified by the passed (external) entries. Handles
  ///   files and resources.
  VBASE_IMPEXP VResourceSnapshotEntry* FindEntry(const VResourceSnapshotEntry &entry) const;

  /// \brief
  ///   Returns the index of the passed item, if it is in the list or -1 otherwise
  inline int GetEntryIndex(const VResourceSnapshotEntry *pEntry) const
  {
    int iCount = GetResourceEntryCount();
    for (int i=0;i<iCount;i++)
      if (&GetResourceEntry(i)==pEntry)
        return i;
    return -1;
  }

  /// \brief
  ///   Returns the number of resource list entries. This count includes simple file entries
  inline int GetResourceEntryCount() const 
  {
    return m_iEntryCount;
  }

  /// \brief
  ///   Returns the resource entry at specified index, where iIndex must be in valid range
  ///   [0..GetResourceEntryCount()-1]
  inline VResourceSnapshotEntry& GetResourceEntry(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<m_iEntryCount);
    if (m_pRetailEntries)
      return m_pRetailEntries[iIndex];
    return m_Entries.GetDataPtr()[iIndex];
  }


  ///
  /// @}
  ///

  ///
  /// @name Load and Save
  /// @{
  ///

#ifdef SUPPORTS_SNAPSHOT_CREATION

  /// \brief
  ///   Saves the list to XML file using the filename and the passed file stream manager (which can
  ///   be NULL)
  VBASE_IMPEXP bool SaveToFile(const char *szFilename,IVFileStreamManager* pManager, IVFilePathResolver *pResolver=NULL);

  /// \brief
  ///   Saves the list to binary file using the filename and the passed file stream manager (which
  ///   can be NULL)
  VBASE_IMPEXP bool SaveToBinaryFile(const char *szFilename,IVFileStreamManager* pManager, IVFilePathResolver *pResolver=NULL);

  
  /// \brief
  ///   Merges all resources from the passed snapshot into this one
  VBASE_IMPEXP void MergeSnapshot(const VResourceSnapshot &other);

#endif // SUPPORTS_SNAPSHOT_CREATION


  /// \brief
  ///   Loads the list from XML file. The loading resolves the filenames using the public m_sPath
  ///   member.
  VBASE_IMPEXP bool LoadFromFile(const char *szFilename,IVFileStreamManager* pManager);

  /// \brief
  ///   Loads the list from XML file. The loading resolves the filenames using the public m_sPath
  ///   member.
  VBASE_IMPEXP bool LoadFromFile(IVFileInStream* pIn);


  /// \brief
  ///   Loads the list from a binary file. The loading resolves the filenames using the public
  ///   m_sPath member.
  VBASE_IMPEXP bool LoadFromBinaryFile(const char *szFilename,IVFileStreamManager* pManager);

  /// \brief
  ///   Loads the list from a binary file. The loading resolves the filenames using the public
  ///   m_sPath member.
  VBASE_IMPEXP bool LoadFromBinaryFile(IVFileInStream* pIn);

  /// \brief
  ///   Returns the raw data block that may contain additional binary data
  inline VRawDataBlock & GetRawDataBlock() {return m_RawDataBlock;}

  ///
  /// @}
  ///

  ///
  /// @name Streaming
  /// @{
  ///

  /// \brief
  ///   Starts the loading process with the current list of resources
  /// 
  /// This function takes some interfaces as input that are used to create the resources on demand.
  /// 
  /// To keep the resource loading busy, the TickFunction has to be called once a tick from the
  /// main thread.
  /// 
  /// \param pCreator
  ///   The resource creator interface that is responsible for creating the resource instances.
  /// 
  /// \param pLoader
  ///   The memory stream manager that provides the background loading capabilities.
  /// 
  /// \param pFileManager
  ///   The filemanager used to open streams for background loading.
  /// 
  /// \param pLog
  ///   Optional log for output messages (e.g. file not found...)
  VBASE_IMPEXP void ScheduleResources(IVResourceCreator *pCreator, VMemoryStreamManager *pLoader, IVFileStreamManager *pFileManager, IVLog *pLog=NULL);

  /// \brief
  ///   Simple loop that finishes loading of all resources in this snapshot in the main thread. ScheduleResources needs to be called to start the process.
  VBASE_IMPEXP void ProcessRemainingResources();

  /// \brief
  ///   Determines whether the snapshot has been loaded
  inline bool IsLoaded() const
  {
    return m_iLoadingVersion>=0;
  }

  /// \brief
  ///   Indicates whether the loading process is finished, i.e. all resources have been loaded
  inline bool IsFinished() const 
  {
    return m_iLoadingIndex==m_iEntryCount;
  }

  /// \brief
  ///   Indicates whether ScheduleResources has been started
  inline bool IsInProgress() const
  {
    return m_iLoadingIndex>=0;
  }

  /// \brief
  ///   Returns the index of the current resource that is processed
  inline int GetCurrentResourceEntry() const
  {
    return m_iLoadingIndex;
  }

  /// \brief
  ///   Returns the total resource footprint
  inline int GetOverallSize() const
  {
    return m_iOverallMemSize;
  }

  /// \brief
  ///   Estimates a percentage progress value
  inline float GetProgress() const
  {
    return (float)((double)m_iLoadedMemSize*100.0/(double)m_iOverallMemSize) + m_fSubProgress;
  }

  /// \brief
  ///   This function must be called once per frame by the main thread while loading is in progress
  ///   (i.e. after ScheduleResources until finished)
  VBASE_IMPEXP virtual void TickFunction(float dtime);

  ///
  /// @}
  ///

  ///
  /// @name Overridables
  /// @{
  ///

  /// \brief
  ///   Overridable that is called every frame to update the progress. The base implementation is
  ///   empty.
  VBASE_IMPEXP virtual void OnProgress(float fPercentage) {}

  /// \brief
  ///   Overridable that is called after a file has been loaded into memory. The base
  ///   implementation is empty.
  VBASE_IMPEXP virtual void OnFileCached(const char *szFilename) {}

  /// \brief
  ///   Overridable that is called after a resource has been created. The base implementation is
  ///   empty.
  VBASE_IMPEXP virtual void OnResourceCreated(VManagedResource *pRes) {}

  /// \brief
  ///   Overridable that is called when the resource loading has finished. The base implementation
  ///   is empty.
  VBASE_IMPEXP virtual void OnFinished() {}
 
  /// \brief
  ///   Overridable that is called to put together path and filename. The base implementation uses
  ///   the public m_ePathType and m_sPath members to concatenate the final filename.
  VBASE_IMPEXP virtual const char* ResolveFilename(const char *szFilename, char *szBuffer);

  /// \brief
  ///   Overridable that is called when an error occurs, e.g. a resource cannot be loaded
  VBASE_IMPEXP virtual void OnError(const char *szErrorMsg) {}

  ///
  /// @}
  ///

  ///
  /// @name Queuing
  /// @{
  ///

  /// \brief
  ///   In case this snapshot instance is scheduled in the scene manager's queue (see IVisSceneManager_cl::ScheduleSnapshot) this value defines the priority
  ///
  /// A new priority value can be set at any time. The snapshots that are used during zone streaming use the negative distance toward the camera as a priority, so closer
  /// zones are streamed in first.
  ///
  /// \param fPriority
  ///   The new priority value. Snapshots with a high priority are handled first
  VBASE_IMPEXP void SetPriority(float fPriority);

  /// \brief
  ///   Get the current priority value that has been set via SetPriority 
  inline float GetPriority() const {return m_fQueuePriority;} 

  /// \brief
  ///   Return the owner queue, if this snapshot is attached to a queue, e.g. via IVisSceneManager_cl::ScheduleSnapshot
  inline VResourceSnapshotQueue* GetQueue() const
  {
    return m_pParentQueue;
  } 

  ///
  /// @}
  ///

protected:
  friend class VResourceSnapshotQueue;
  VBASE_IMPEXP VResourceSnapshotEntry* AddResourceEntry(VManagedResource *pRes, float fPriority);
  VBASE_IMPEXP VResourceSnapshotEntry* AddFileEntry(const char *szFilename, int iFileSize, float fPriority);
  VBASE_IMPEXP void Sort();
  VBASE_IMPEXP static int VISION_CDECL ResourcePriorityCompare( const void *arg1, const void *arg2 );
  void ResourceNodeXMLExchange(TiXmlElement *pResNode, bool bWrite);
  void EnsureXMLEntriesCreated();

  bool m_bForceNextRes, m_bOverallMemSizeValid;
  int m_iOverallMemSize, m_iLoadedMemSize;
  int m_iEntryCount;
  DynObjArray_cl<VResourceSnapshotEntryXML>m_Entries;
  VResourceSnapshotEntryRetail *m_pRetailEntries;

  VRawDataBlock m_RawDataBlock;

  IVResourceCreator *m_pCreator;
  VMemoryStreamManager *m_pLoader;
  IVFileStreamManager *m_pFileManager;
  IVLog *m_pLog;

  int m_iLoadingIndex;
  VLoadingTaskPtr m_spCurrentLoadingTask;
  volatile float m_fSubProgress, m_fSubProgressScale;
  
  VResourceSnapshotQueue *m_pParentQueue;
  float m_fQueuePriority;


public:
  VUsageType_e m_eSnapshotUsage; ///< Defines the usage of this snapshot.
  int m_iLoadingVersion;        ///< file version (while loading)
  VFilePathType_e m_ePathType;  ///< file path mode for all resources in this list
  VString m_sPath; ///< path to prepend
};


/// \brief
///   An instance of this class manages a snapshot queue. In a snapshot queue, only the topmost element is handled, because file caching is a linear process
/// When an element in the queue is finished, it will be removed from the queue and the next one is handled. The queue remains sorted when the elements
/// call their VResourceSnapshot::SetPriority value.
class VResourceSnapshotQueue
{
public:
  /// \brief
  ///   Constructor
  VBASE_IMPEXP VResourceSnapshotQueue();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VResourceSnapshotQueue();

  /// \brief
  ///   Indicates whether there is still an element processed or in the queue
  inline bool IsBusy() const
  {
    return m_pCurrent!=NULL || m_iEntryCount>0;
  }

  /// \brief
  ///   Add a snapshot instance to the queue
  VBASE_IMPEXP void AddSnapshot(VResourceSnapshot *pSnapshot);

  /// \brief
  ///   Remove a snapshot instance from the queue. A snapshot will be automatically removed once it is finished or when VResourceSnapshot::Reset is called
  VBASE_IMPEXP void RemoveSnapshot(VResourceSnapshot *pSnapshot);

  /// \brief
  ///   Internal function to mark the queue sorting as dirty. Does not have to be called by user code
  inline void InvalidateOrder() 
  {
    m_bNeedsResort = true;
  }

  /// \brief
  ///   Return the snapshot element that is currently processed. Can be NULL
  inline VResourceSnapshot *GetCurrent() const
  {
    return m_pCurrent;
  }

  /// \brief
  ///   Returns a sorted list of remaining snapshot elements. This list does not include the current element (GetCurrent)
  VBASE_IMPEXP VResourceSnapshot** GetQueue(int &iCount);

  /// \brief
  ///   This function must be called by the game tick from the main thread to update the queue
  VBASE_IMPEXP void TickFunction(float dtime);

protected:
  static int ComparePriority( const void *elem1, const void *elem2);
  bool m_bNeedsResort;
  int m_iEntryCount;
  DynArray_cl<VResourceSnapshot *>m_Entries;
  VResourceSnapshot *m_pCurrent;
public:
  // must be set when using tickfunction:
  IVResourceCreator *m_pCreator;
  VMemoryStreamManager *m_pLoader;
  IVFileStreamManager *m_pFileManager;
  IVLog *m_pLog;
};



/// \brief
///   Pure virtual interface that can lookup and create resources of arbitrary types.
/// 
/// An instance of this interface is used by the VResourceSnapshot::ScheduleResources function.
/// 
/// \sa VResourceSnapshot
/// \sa VResourceSnapshot::ScheduleResources
/// \sa VResourceSnapshotEntry
class IVResourceCreator : public VBaseObject
{
public:

  /// \brief
  ///   Destructor
  virtual ~IVResourceCreator() {}

  /// \brief
  ///   Pure virtual function to look up a resource that is described by the resourceDesc
  ///   parameter.
  /// 
  /// A straightforward implementation of this function looks up the resource manager in the global
  /// resource system and then looks up the resource by name.
  /// 
  /// \param snapshot
  ///   Reference to the calling snapshot
  /// 
  /// \param resourceDesc
  ///   structure that describes the resource, e.g. filename and resource manager name
  /// 
  /// \return
  ///   VManagedResource *pResource: Resource (or NULL if not created yet)
  virtual VManagedResource *FindExistingResource(VResourceSnapshot &snapshot, VResourceSnapshotEntry &resourceDesc) = 0;

  /// \brief
  ///   Pure virtual function to create a new resource described by the resourceDesc parameter.
  /// 
  /// A straightforward implementation of this function looks up the resource manager in the global
  /// resource system and then calls VResourceManager::CreateResource on it.
  /// 
  /// This function does not need to call FindExistingResource beforehand.
  /// 
  /// \param snapshot
  ///   Reference to the calling snapshot
  /// 
  /// \param resourceDesc
  ///   structure that describes the resource, e.g. filename and resource manager name
  /// 
  /// \return
  ///   VManagedResource *pResource: new resource instance
  virtual VManagedResource *CreateResource(VResourceSnapshot &snapshot, VResourceSnapshotEntry &resourceDesc) = 0;

  /// \brief
  ///   Overridable to return a different filename on the fly for the passed resource/file entry
  /// 
  /// This overridable allows for returning a different file for loading than the actual resource
  /// filename.
  /// 
  /// The default implementation inside the engine for instance uses this function to patch a
  /// shader library's resource name  by its platform specific bin filename.
  /// 
  /// \param resourceDesc
  ///   structure that describes the resource, e.g. filename and resource manager name
  /// 
  /// \param szResolvedFilename
  ///   The filename of the resource that has already been resolved (path prepending)
  /// 
  /// \param szBuffer
  ///   A char buffer that is large enough to store a new filename. This buffer can be filled and
  ///   returned.
  /// 
  /// \return
  ///   const char *szNewFilename: A new filename. Can be NULL to leave the filename as is (default
  ///   implementation).
  virtual const char *GetReplacementFilename(VResourceSnapshotEntry &resourceDesc, const char *szResolvedFilename, char *szBuffer)
  {
    return NULL;
  }

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
