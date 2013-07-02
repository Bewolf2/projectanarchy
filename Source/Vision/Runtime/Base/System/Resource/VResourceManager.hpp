/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VResourceManager.hpp

#ifndef _VRESOURCEMANAGER_HPP_INCLUDED
#define _VRESOURCEMANAGER_HPP_INCLUDED

#include <Vision/Runtime/Base/System/VCallbacks.hpp>
#include <Vision/Runtime/Base/Container/VMapStrToPtr.hpp>

// user manager flags
#define VRESOURCEMANAGERFLAG_NONE                         0x00000000
#define VRESOURCEMANAGERFLAG_UNLOADUNUSED                 0x00000001
#define VRESOURCEMANAGERFLAG_ALLOW_ORDER_CHANGE           0x00000002
#define VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER               0x00000004
#define VRESOURCEMANAGERFLAG_PURGE_AFTER_LOADWORLD        0x00000008
#define VRESOURCEMANAGERFLAG_IGNORE_SURVIVING_RESOURCES   0x00000010
#define VRESOURCEMANAGERFLAG_NO_LOADWORLD_PURGING         0x00000020
#define VRESOURCEMANAGERFLAG_SUSPEND_PURGING              0x00000040

// memory limit flags
#define VRESOURCEMANAGERFLAG_MEMLIMIT_SOFT          0x00000100
#define VRESOURCEMANAGERFLAG_MEMLIMIT_HARD          0x00000200
#define VRESOURCEMANAGERFLAG_MEMLIMIT_MEDIUM        0x00000400
#define VRESOURCEMANAGERFLAG_MEMLIMIT_MASK          (VRESOURCEMANAGERFLAG_MEMLIMIT_SOFT|VRESOURCEMANAGERFLAG_MEMLIMIT_MEDIUM|VRESOURCEMANAGERFLAG_MEMLIMIT_HARD)
#define VRESOURCEMANAGERFLAG_MEMLIMIT_INTERVALCHECK 0x00001000


// internal manager flags, do not set!
#define VRESOURCEMANAGERFLAG_CUSTOMTAG              0x20000000
#define VRESOURCEMANAGERFLAG_REGISTERED             0x40000000
#define VRESOURCEMANAGERFLAG_SYSTEM_MANAGER         0x80000000

// flags returned by VResourceManager::GetChangedFlags
#define VRESOURCECHANGEDFLAG_NONE                   0x00000000
#define VRESOURCECHANGEDFLAG_ADDEDRESOURCE          0x00000001
#define VRESOURCECHANGEDFLAG_REMOVEDRESOURCE        0x00000002
#define VRESOURCECHANGEDFLAG_MEMSIZECHANGED         0x00000004
#define VRESOURCECHANGEDFLAG_MEMLIMITCHANGED        0x00000008
#define VRESOURCECHANGEDFLAG_LOADEDRESOURCE         0x00000010
#define VRESOURCECHANGEDFLAG_UNLOADEDRESOURCE       0x00000020
#define VRESOURCECHANGEDFLAG_MEMSIZECHANGING        0x00000040
#define VRESOURCECHANGEDFLAG_RESOURCEMISSING        0x00000080
#define VRESOURCECHANGEDFLAG_RESOURCE_ADDREF        0x00000100
#define VRESOURCECHANGEDFLAG_RESOURCE_RELEASE       0x00000200
#define VRESOURCECHANGEDFLAG_LOADINGRESOURCE        0x00000400
#define VRESOURCECHANGEDFLAG_UNLOADINGRESOURCE      0x00000800
#define VRESOURCEACTION_GATHER_PREVIEW              0x00001000
#define VRESOURCECHANGEDFLAG_BEFORE_FILEMODIFIED    0x00002000
#define VRESOURCECHANGEDFLAG_AFTER_FILEMODIFIED     0x00004000
#define VRESOURCECHANGEDFLAG_FORCEVIEWERUPDATE      0x00010000

#define VRESOURCEMANAGER_PURGEMASK_AFTERSCENELOAD     V_BIT(0)
#define VRESOURCEMANAGER_PURGEMASK_AFTERSCENEUNLOAD   V_BIT(1)
#define VRESOURCEMANAGER_PURGEMASK_STREAMINGCLEANUP   V_BIT(2)
#define VRESOURCEMANAGER_PURGEMASK_FORCEALL           V_BIT(31)

class VManagedResource;
DECLARE_LIST(VResourceList,VManagedResource,VBASE_IMPEXP)
class VResourceSnapshotEntry;
class VResourceManager;
class IVResourcePreview;

class VConnection;

/// \brief
///   This enum defines the different memory pool types as used by VManagedResource::SetNewMemSize for instance
///
/// \sa VManagedResource::SetNewMemSize
/// \sa VManagedResource::GetNewMemSize
enum VResourceMemoryType_e
{
  VRESOURCEMEMORY_SYSTEM            = V_BIT(0), ///< Unique system memory used by the resource
  VRESOURCEMEMORY_GPU               = V_BIT(1), ///< Unique GPU memory used by the resource
  VRESOURCEMEMORY_DEPENDENT_SYSTEM  = V_BIT(2), ///< System memory of other resources that this resource depends on
  VRESOURCEMEMORY_DEPENDENT_GPU     = V_BIT(3), ///< GPU memory of other resources that this resource depends on

  VRESOURCEMEMORY_GPU_DOUBLEBUFFERED  = VRESOURCEMEMORY_SYSTEM|VRESOURCEMEMORY_GPU,   ///< Bitflag combinations for both unique memory types
  VRESOURCEMEMORY_ALLTYPES            = VRESOURCEMEMORY_SYSTEM|VRESOURCEMEMORY_GPU|VRESOURCEMEMORY_DEPENDENT_SYSTEM|VRESOURCEMEMORY_DEPENDENT_GPU ///< Bitflag combinations for all memory types
};

#ifdef SUPPORTS_RESOURCEVIEWER

/// \brief Helper to generate data for the vResViewer. For internal use.
struct VResourceViewerData
{
private:

  int m_iDataSize;
  DynArray_cl<char> m_dataBlock;
  int m_iManagerBlockSizePos;

public:

  /// \brief constructor
  VResourceViewerData()  :
  m_iDataSize(0),
  m_dataBlock(0,0),
  m_iManagerBlockSizePos(0)
  {}

  /// \brief resets the internal counters
  HKV_FORCE_INLINE void Reset()
  {
    m_iDataSize = 0;
    m_iManagerBlockSizePos = 0;
  }

  /// \brief 
  ///   starts a new manager block
  ///   has to be called before any write operations take place
  ///
  /// \param pszManagerName
  ///   the name of the manager
  ///
  /// \param iColor
  ///   Color for the manager
  ///
  /// \param iMemlimit
  ///   memory limit for the manager
  HKV_FORCE_INLINE void StartManager(const char* pszManagerName, int iColor, __int64 iMemlimit)
  {
    WriteString(pszManagerName);
    WriteInt(iColor); //resource manager color
    WriteInt((int)(iMemlimit & 0xffffffff)); //resource manager memory limit
    WriteInt((int)(iMemlimit >> 32)); //resource manager memory limit; for backwards compatibility dont use WriteInt64
    WriteInt(0); // reserved

    m_iManagerBlockSizePos = m_iDataSize; // save the position of the size field
    WriteInt(-1); //will be replaced by data size later
  } 

  /// \brief
  ///   Finishes a manager block. Has to be called after all write operations are complete
  HKV_FORCE_INLINE void FinishManager()
  {
    int iManagerByteCount = m_iDataSize - m_iManagerBlockSizePos - sizeof(int);
    VASSERT(iManagerByteCount>=0);
    int *pSizePos = (int *)&m_dataBlock.GetDataPtr()[m_iManagerBlockSizePos];
    VASSERT(*pSizePos==-1);
    #ifndef VISION_LITTLE_ENDIAN
    EndianSwitchDWords((DWORD*)&iManagerByteCount, 1);
    #endif
    *pSizePos = iManagerByteCount;
  }

  /// \brief copies a block of data
  ///
  /// \param data
  ///   data to copy
  ///
  /// \param size
  ///   size of the data to copy
  HKV_FORCE_INLINE void CopyBlock(const void* data, int size)
  {
    VASSERT(size > 0);
    m_dataBlock[m_iDataSize + size] = 0;
    memcpy(&m_dataBlock.GetDataPtr()[m_iDataSize], data, size);
    m_iDataSize += size;
  }

  /// \brief Writes a int
  HKV_FORCE_INLINE void WriteInt(int value)
  {
    #ifndef VISION_LITTLE_ENDIAN
    EndianSwitchDWords((DWORD*)&value, 1);
    #endif
    CopyBlock(&value, sizeof(int));
  }

  /// \brief writes a int64
  HKV_FORCE_INLINE void WriteInt64(__int64 value)
  {
    #ifndef VISION_LITTLE_ENDIAN
    EndianSwitchQWords((uint64*)&value, 1);
    #endif
    CopyBlock(&value, sizeof(__int64));
  }

  /// \brief writes a float
  HKV_FORCE_INLINE void WriteFloat(float value)
  {
    #ifndef VISION_LITTLE_ENDIAN
    EndianSwitchDWords((DWORD*)&value, 1);
    #endif
    CopyBlock(&value, sizeof(float));
  }

  /// \brief writes a string
  HKV_FORCE_INLINE void WriteString(const char* str)
  {
    char blank = 0;
    if(!str)
      str = &blank;
    CopyBlock(str, static_cast<int>(strlen(str))+1);
  }

  /// \brief returns the size of the cached data
  inline int GetSize() const
  {
    return m_iDataSize;
  }

  /// \brief returns a pointer to the cached data
  inline const char* GetData() const
  {
    return m_dataBlock.GetDataPtr();
  }
};

/// \brief data object for the VResourceManager::OnUpdateResourceViewer callback
class VUpdateResourceViewerObject : public IVisCallbackDataObject_cl
{
public:
  VResourceViewerData& m_data; ///< data object to write to

  /// \brief constructor
  VUpdateResourceViewerObject(VCallback *pSender, VResourceViewerData& data) : IVisCallbackDataObject_cl(pSender),
    m_data(data)
  { }
};
#endif


/// \brief
///   Derived callback data object class that is used for the VResourceManager::OnResourceChanged event.
/// 
/// The action member is an integer constant that describes the reason for the event. Can be a
/// constant of type VRESOURCECHANGEDFLAG_XYZ.
/// 
/// Currently the following actions are supported:
/// \li VRESOURCECHANGEDFLAG_LOADINGRESOURCE    : Triggered before the Reload function has been called.
/// \li VRESOURCECHANGEDFLAG_UNLOADINGRESOURCE  : Triggered before the Unload function has been called
/// \li VRESOURCECHANGEDFLAG_LOADEDRESOURCE     : Triggered after the Reload function has been called.
/// \li VRESOURCECHANGEDFLAG_UNLOADEDRESOURCE   : Triggered after the Unload function has been called.
/// \li VRESOURCECHANGEDFLAG_MEMSIZECHANGING    : Triggered before the memory usage is about to change
/// \li VRESOURCECHANGEDFLAG_MEMSIZECHANGED     : Triggered after the memory usage has changed
/// \li VRESOURCECHANGEDFLAG_RESOURCEMISSING    : Triggered for every resource that is flagged as missing (via VManagedResource::FlagAsMissing())
///
/// \sa class IVisCallbackHandler_cl
/// \sa VResourceManager::OnResourceChanged
class VisResourceInfoDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that initializes information about this event (resource, action)
  inline VisResourceInfoDataObject_cl(VCallback *pSender, VResourceManager *pManager, VManagedResource *pResource, int iAction) : IVisCallbackDataObject_cl(pSender)
  {
    m_pManager = pManager;
    m_pResource = pResource;
    m_iAction = iAction;
  }
  
  /// \brief
  ///   The owner resource manager
  VResourceManager *m_pManager;

  /// \brief
  ///   The resource itself
  VManagedResource *m_pResource;

  /// \brief
  ///   Integer constant that describes the reason for change. Can be a constant of type
  ///   VRESOURCECHANGEDFLAG_XYZ. See class description
  int m_iAction;

};


/// \brief
///   This data object is sent for the VRESOURCEACTION_GATHER_PREVIEW event in the VResourceManager::OnResourceChanged callback
///
/// This structure has an additional m_pPreviewObj member that can be set by listeners. This callback is triggered by
/// the default implementation of VResourceManager::OnResourceChanged to query the preview object.
///
/// \remarks
///   This is an alternative mechanism to VManagedResource::CreateResourcePreview so that the preview object code
///   can be separated from the class itself (to keep the native side compact).
///
class VisResourcePreviewDataObject_cl : public VisResourceInfoDataObject_cl
{
public:
  inline VisResourcePreviewDataObject_cl(VCallback *pSender, VResourceManager *pManager, VManagedResource *pResource, int iAction)
    : VisResourceInfoDataObject_cl(pSender,pManager,pResource,iAction)
  {
    m_pPreviewObj = NULL;
  }
  
  IVResourcePreview *m_pPreviewObj; ///< set this member
};


/// \brief
///   Interface that can be globally installed to resolve file paths before loading a resource. See
///   VResourceManager::SetFilePathResolver
class IVFilePathResolver
{
public:
  VBASE_IMPEXP virtual ~IVFilePathResolver() {;}

  /// \brief
  ///   Virtual overridable to return a replacement file path for the incoming path (e.g. turn
  ///   relative into absolute)
  /// 
  /// One global resolver can be installed by the static VResourceManager::SetFilePathResolver
  /// function to re-direct resource filenames.
  /// 
  /// \param szPath
  ///   The incoming file path
  /// 
  /// \param szBuffer
  ///   A Buffer that can be used to concatenate a custom path and then return it. This buffer has
  ///   size FS_MAX_PATH thus enough for file paths
  /// 
  /// \return
  ///   const char *szNewPath: The new replacement path. This can be the incoming szPath (default
  ///   behavior), szBuffer or any other string that persists the call of this function
  VBASE_IMPEXP virtual const char *ResolvePath(const char *szPath, char *szBuffer)
  {
    return szPath;
  }
};




/// \brief
///   Resource manager class
/// 
/// A resource manager instance keeps a list of managed resources (VManagedResource class) and
/// "manages" them.
/// 
/// That means the manager keeps track of changing loaded states, sums up the overall memory and
/// cleans up the list if a memory limit is exceeded.
/// 
/// The engine has several instances of resource manager, each responsible for a specific type of
/// resources, such as textures, models, etc.
/// 
/// The resource manager in the engine can be accessed through the VisResourceSystem_cl class.
/// 
/// For implementing a custom resource manager, the derived Vision resource manager class
/// (VisResourceManager_cl) must be used.
class VResourceManager : public VBaseObject
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Resource manager constructor.
  /// 
  /// \param szManagerName
  ///   Manager name. This name appears in the resource viewer TAB.
  /// 
  /// \param iFlags
  ///   resource manager specific flags. The following flags are supported and can be combined:
  ///   \li VRESOURCEMANAGERFLAG_UNLOADUNUSED : The resource manager should unload unused resource
  ///     instead of deleting them. Relevant if a memory limit is specified.
  ///   \li VRESOURCEMANAGERFLAG_ALLOW_ORDER_CHANGE : Allows to resort the array of elements. That
  ///     means that the list index of resources may change.
  ///   \li VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER : Flags the resource manager for showing up in the
  ///     resource viewer. Note that the resource manager has to be registered.
  ///   \li VRESOURCEMANAGERFLAG_PURGE_AFTER_LOADWORLD : If specified, the resources will be purged
  ///     after LoadWorld and not when unloading a world. This feature allows to reuse resources in
  ///     the next scene.
  ///   \li VRESOURCEMANAGERFLAG_IGNORE_SURVIVING_RESOURCES : If specified, the engine will not
  ///     report resources in this manager  that survive a new scene loading. 
  ///   \li VRESOURCEMANAGERFLAG_NO_LOADWORLD_PURGING : If specified, the resource manager will
  ///     only be purged when deinitializing the engine and not before/after LoadWorld. This flag
  ///     supersedes VRESOURCEMANAGERFLAG_PURGE_AFTER_LOADWORLD
  ///
  /// \param iMemLimit
  ///   Memory limit to set for this resource manager.
  VBASE_IMPEXP VResourceManager(const char *szManagerName, int iFlags=VRESOURCEMANAGERFLAG_NONE, __int64 iMemLimit=0);

  /// \brief
  ///   Resource manager destructor
  VBASE_IMPEXP virtual ~VResourceManager();


  ///
  /// @}
  ///

  ///
  /// @name Manager Properties
  /// @{
  ///

  /// \brief
  ///   Returns the manager name
  /// 
  /// \return
  ///   const char *szManagerName : The manager name which has been defined in the constructor
  VBASE_IMPEXP const char *GetManagerName() const;

  /// \brief
  ///   Indicates whether this manager is a system manager, i.e. a resource manager used by the
  ///   engine for textures, models, etc.
  /// 
  /// \return
  ///   BOOL bResult : TRUE, if this manager is a system manager.
  inline BOOL IsSystemManager() const {return (m_iManagerFlags&VRESOURCEMANAGERFLAG_SYSTEM_MANAGER)>0;}

  /// \brief
  ///   Allows the programmer to modify the resource manager flags at runtime.
  /// 
  /// The specified flags will be added without clearing the current flags.
  /// 
  /// \param iMask
  ///   Flags to add. For supported flags, see constructor.
  VBASE_IMPEXP void SetManagerFlag(int iMask);

  /// \brief
  ///   Allows the programmer to modify the resource manager flags at runtime.
  /// 
  /// The specified flags will be removed from the current flags.
  /// 
  /// \param iMask
  ///   Flags to remove. For supported flags, see constructor.
  VBASE_IMPEXP void RemoveManagerFlag(int iMask);

  /// \brief
  ///   Checks whether a manager flag is set.
  /// 
  /// \param iMask
  ///   Flag(s) to check. 
  /// 
  /// \return
  ///   BOOL bResult : TRUE if the flags are set.
  inline BOOL IsManagerFlagSet(int iMask) const  
  {
    return (m_iManagerFlags & iMask)==iMask;
  }

  /// \brief
  ///   Enables unloading of resources when the application goes to the background, and
  ///   reloading when it comes back to the foreground. 
  ///
  /// \param bStatus
  ///   Value to set. Default value is false.
  VBASE_IMPEXP void SetHandlesBackgrounding(bool bStatus);

  /// \brief
  ///   If background handling is enabled, resources are unloaded when the application goes 
  ///   to the background, and are reloaded again when it comes back to the foreground.  
  inline bool GetHandlesBackgrounding() const 
  { 
    return m_bHandlesBackgrounding; 
  }

  /// \brief
  ///   Sets the VRESOURCEMANAGERFLAG_SUSPEND_PURGING flag. This flag causes this manager not to be purged when purge events are triggered
  inline void SuspendPurging()
  {
    SetManagerFlag(VRESOURCEMANAGERFLAG_SUSPEND_PURGING);
  }

  /// \brief
  ///   Removes the VRESOURCEMANAGERFLAG_SUSPEND_PURGING flag. This manager will be purged again during purge events (but it will not be purged immediately)
  inline void ResumePurging()
  {
    RemoveManagerFlag(VRESOURCEMANAGERFLAG_SUSPEND_PURGING);
  }

  ///
  /// @}
  ///

  ///
  /// @name Add/remove Resources
  /// @{
  ///

  /// \brief
  ///   Attaches a managed resource to this manager. For internal use only.
  ///
  /// \param pResource
  ///   The resource to add.
  VBASE_IMPEXP int AddResource(VManagedResource *pResource);

  /// \brief
  ///   Removes a managed resource from this manager. For internal use only.
  ///
  /// \param pResource
  ///   The resource to remove.
  VBASE_IMPEXP void RemoveResource(VManagedResource *pResource);

  /// brief
  ///   Adds the resource to the restore pool of this manager. For internal use only.
  ///
  /// \param pResource
  ///   The resource to add.
  ///
  /// The specified resource will be automatically unloaded/reloaded upon backgrounding event.
  VBASE_IMPEXP void AddResourceToRestorePool(VManagedResource *pResource);

  /// brief
  ///   Removes the resource from the restore pool of this manager. For internal use only.
  ///
  /// \param pResource
  ///   The resource to remove.
  VBASE_IMPEXP void RemoveResourceFromRestorePool(VManagedResource *pResource);


  ///
  /// @}
  ///

  ///
  /// @name Access Resources
  /// @{
  ///

  /// \brief
  ///   Returns the number of resource entries, i.e. the highest index of a used resource minus
  ///   one.
  /// 
  /// This value can be used to iterate through the list of resources, but note that there might
  /// be NULL pointer in the list.
  VBASE_IMPEXP int GetResourceCount() const;

  /// \brief
  ///   Returns the number of actually used resource entries.
  /// 
  /// This value can not be used to iterate through the list of resources.
  VBASE_IMPEXP int GetUsedResourceCount() const;

  /// \brief
  ///   Returns the number of actually loaded resources.
  VBASE_IMPEXP int GetLoadedResourceCount() const;

  /// \brief
  ///   Returns the number of resource entries in the restore pool.
  ///
  /// Resources in the restore pool are unloaded if the application is suspended.
  /// This value can be used to iterate through the list of resources, but note that there might
  /// be NULL pointer in the list. 
  inline int GetRestorePoolResourceCount() const
  { 
    return m_resourcesToRestore.GetLength(); 
  }

  /// \brief
  ///   Returns the resource with the specified index. The index is 0-based and must be in valid range.
  ///
  /// \param iIndex
  ///   The resource index of the resource.
  ///
  /// \sa VResourceManager::GetResourceCount
  inline VManagedResource* GetResourceByIndex(int iIndex) const 
  { 
    return m_resources.Get(iIndex); 
  }

  /// \brief
  ///   Returns the restore-pool resource with the specified index. The index is 0-based and must be in valid range.
  ///
  /// \param iIndex
  ///   The resource index of the resource.
  ///
  /// \sa VResourceManager::GetRestorePoolResourceCount
  inline VManagedResource* GetResourceByRestorePoolIndex(int iIndex) const
  { 
    return m_resourcesToRestore.Get(iIndex); 
  }

  /// \brief
  ///   Finds the first resource with specified name (filename).
  ///   Returns NULL if no resource with specified name has been found.
  VBASE_IMPEXP VManagedResource *GetResourceByName(const char *szFilename) const;

  /// \brief
  ///   Same as pResource->GetNumber()
  VBASE_IMPEXP int GetResourceIndex(VManagedResource *pResource) const;

  ///\ brief
	///   Checks whether this manager contains a resource with the specified pointer
	///
	/// \param pResource
	///   Resource pointer to test
	///
	/// \returns
	///   TRUE if the manager contains the passed pointer.
	inline BOOL IsValidResource(VManagedResource *pResource) const
  {
    const int iCount = GetResourceCount();
    for (int i=0;i<iCount;i++)
      if (GetResourceByIndex(i)==pResource)
        return TRUE;
    return FALSE;
  }


  /// \brief
  ///   Virtual function that can be implemented by each resource manager to load a resource from
  ///   file
  /// 
  /// The base implementation converts the filename to a compatible name, then looks up the
  /// resource by name (GetResourceByName)  and returns CreateResource(), if not found in list.
  /// 
  /// Thus, since the real creation is done in CreateResource, it is enough to override that
  /// function
  /// 
  /// This function is used by the serialization proxy objects.
  ///
  /// \param szFilename
  ///   Filename of the resource to load.
  VBASE_IMPEXP virtual VManagedResource *LoadResource(const char *szFilename);

  /// \brief
  ///   Virtual function that should be implemented by each resource manager to create a resource 
  VBASE_IMPEXP virtual VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);

  /// \brief
  ///   Internal function to handle the resource manager. Gets called by the engine.
  VBASE_IMPEXP void TickFunction(float dtime);

  /// \brief
  ///   Overridable function to do something once per frame. Gets called when manager is registered
  ///   in the engine.
  VBASE_IMPEXP virtual void OnTickFunction(float dtime) {}

  /// \brief
  ///   Used by the streaming system to patch a filename on-the-fly
  VBASE_IMPEXP virtual const char *GetStreamingReplacementFilename(VResourceSnapshotEntry &resourceDesc, 
    const char *szResolvedFilename, char *szBuffer)
  {
    return szResolvedFilename;
  }


  ///
  /// @}
  ///

  ///
  /// @name Purging/reloading
  /// @{
  ///


  /// \brief
  ///   Purges all resources that have no reference. Returns the number of removed resources.
  VBASE_IMPEXP unsigned int PurgeUnusedResources();

  /// \brief
  ///   Purges all resources that have no reference and additionally have not been used for a
  ///   certain amount of time.
  ///   Returns the number of removed resources.
  VBASE_IMPEXP unsigned int PurgeUnusedResources(float fTimeNotUsed);

  /// \brief
  ///   Purges all resources with specific filename. Returns the number of removed resources.
  VBASE_IMPEXP unsigned int PurgeUnusedResources(const char *szResourceName, float fTimeNotUsed);

  /// \brief
  ///   This is called after each purge to allow a derived class to do further purging.
  VBASE_IMPEXP virtual void OnAfterUnusedResourcesPurged() {}

  /// \brief
  ///   Unloads resources that have not been used for a specific time. 
  ///   Optionally allows to ignore the reference counter, i.e. also unloads still referenced
  ///   resources.
  VBASE_IMPEXP unsigned int UnloadUnusedResources(float fTimeNotUsed, bool bUnrefOnly=true);

  /// \brief
  ///   Unloads resources with specific name that have not been used for a specific time.
  ///   Optionally allows to ignore the reference counter, i.e. also unloads still referenced
  ///   resources.
  VBASE_IMPEXP unsigned int UnloadUnusedResources(const char *szResourceName, float fTimeNotUsed, bool bUnrefOnly=true);

  /// \brief
  ///   Deletes all resources by brute force decreasing the ref counter. This function should never
  ///   be used.
  VBASE_IMPEXP unsigned int DeleteAllResources();

  /// \brief
  ///   Reloads all modified resources in the manager.
  /// 
  /// Modified resources are identified by their file time stamp.
  /// 
  /// A file stream manager has to be passed for loading resources.
  /// 
  /// \param pManager
  ///   The file stream manager used for retrieving the time stamp. Should be
  ///   Vision::File.GetManager() when used in the engine.
  /// 
  /// \param bUnload
  ///   If TRUE, the resource will be unloaded in case it has been modified (calls EnsureUnloaded
  ///   for the resource).
  /// 
  /// \param bReload
  ///   If TRUE, the resource will be reloaded in case it has been modified (calls EnsureLoaded for
  ///   the resource).
  /// 
  /// \return
  ///   unsigned int iModifiedCount : Number of resources that have been detected to be modified.
  /// 
  /// \note
  ///   The following combinations of the bUnload,bReload flags actually make sense:
  ///   \li FALSE,FALSE : Nothing happens to the resources, useful for counting the modified
  ///     resources.
  /// 
  ///   \li TRUE,FALSE :  Modified resource are unloaded but not reloaded. Reloading will happen
  ///     the next time the resource is used.
  /// 
  ///   \li TRUE,TRUE :  Modified resource are unloaded and reloaded. The resource is up-to-date
  ///     afterwards.
  VBASE_IMPEXP unsigned int ReloadModifiedResourceFiles(IVFileStreamManager *pManager, BOOL bUnload=TRUE, BOOL bReload=TRUE);

  /// \brief
  ///   Reloads a specific modified resource in the manager.
  /// 
  /// Modified resources are identified by their file time stamp.
  /// 
  /// A file stream manager has to be passed for loading resources.
  ///
  /// \param szResourceId
  ///   The ID of the resource to reload.
  ///
  /// \param pManager
  ///   The file stream manager used for retrieving the time stamp. Should be
  ///   Vision::File.GetManager() when used in the engine.
  ///
  ///
  /// \param bUnload
  ///   If TRUE, the resource will be unloaded in case it has been modified (calls EnsureUnloaded
  ///   for the resource).
  /// 
  /// \param bReload
  ///   If TRUE, the resource will be reloaded in case it has been modified (calls EnsureLoaded for
  ///   the resource).
  /// 
  /// \return
  ///   Returns TRUE when the resource was modified.
  VBASE_IMPEXP BOOL ReloadModifiedResourceFile(const char *szResourceId, IVFileStreamManager *pManager, BOOL bUnload=TRUE, BOOL bReload=TRUE);

  /// \brief
  ///   Reloads a specific resource in this resource manager. The resource is reloaded
  ///   regardless of whether the file has actually been modified or not.
  /// 
  /// \param szResourceId
  ///   The ID of the resource to reload.
  /// 
  /// \param pManager
  ///   The file stream manager used for retrieving the time stamp. Should be
  ///   Vision::File.GetManager() when used in the engine.
  /// 
  /// \param bUnload
  ///   If TRUE, a resource will be unloaded (calls EnsureUnloaded for the resource).
  /// 
  /// \param bReload
  ///   If TRUE, a resource will be reloaded (calls EnsureLoaded for the resource).
  /// 
  /// \return
  ///   TRUE if the resource has been found and was reloaded; false if either the specified resource
  ///   manager was not found or the resource was not found within the resource manager.
  /// 
  /// \note
  ///   The following combinations of the bUnload,bReload flags actually make sense:
  ///   \li FALSE,FALSE : Nothing happens to the resource. Might be useful for checking whether a
  ///     resource can actually be resolved.
  /// 
  ///   \li TRUE,FALSE :  The resource is unloaded, but not reloaded. Reloading will happen
  ///     the next time the resource is used.
  /// 
  ///   \li TRUE,TRUE :  The resource is unloaded and reloaded. It is guaranteed
  ///     to be up-to-date afterwards.
  VBASE_IMPEXP BOOL ReloadSpecificResourceFile(const char *szResourceId, IVFileStreamManager *pManager, BOOL bUnload=TRUE, BOOL bReload=TRUE);
  
  ///
  /// @}
  ///

  ///
  /// @name Auto Unload/purge
  /// @{
  ///

  /// \brief
  ///   Allows to set a time interval for automatically unload unused resources
  /// 
  /// The resource manager has to be registered in the engine, otherwise this won't have any effect
  /// 
  /// \param fCheckInterval
  ///   The time interval to check. If for instance 10.0f, the resource manager will check each 10s
  ///   if any resource can be unloaded. Can be 0.0f to check each frame (though not recommended)
  /// 
  /// \param fTimeNotUsed
  ///   The time the resources to check have not been used
  /// 
  /// \param bUnrefOnly
  ///   If true, only unreferenced resources will be unloaded automatically.
  /// 
  /// \param fCurrentOfs
  ///   (Optional) The time offset value. Allows to specify a different value per resource manager,
  ///   so not all resource managers are tested within the same tick.
  /// 
  /// \sa VResourceManager::UnloadUnusedResources
  /// \sa VResourceManager::SetAutoPurgeInterval
  /// 
  /// \example
  ///   \code
  ///   pMyManager->SetAutoUnloadInterval(10.f, 50.f); // checks all 10sec to unload resources that are unused for at least 50sec
  ///   \endcode
  VBASE_IMPEXP void SetAutoUnloadInterval(float fCheckInterval, float fTimeNotUsed, bool bUnrefOnly=true, float fCurrentOfs=0.f);

  /// \brief
  ///   Allows to set a time interval for automatically purge unused resources
  /// 
  /// The resource manager has to be registered in the engine, otherwise this won't have any effect
  /// 
  /// \param fCheckInterval
  ///   The time interval to check. If for instance 10.0f, the resource manager will check each 10s
  ///   if any resource can be purged. Can be 0.0f to check each frame (though not recommended)
  /// 
  /// \param fTimeNotUsed
  ///   The time the resources to check have not been used
  /// 
  /// \param fCurrentOfs
  ///   (Optional) The time offset value. Allows to specify a different value per resource manager,
  ///   so not all resource managers are tested within the same tick.
  /// 
  /// \sa VResourceManager::PurgeUnusedResources
  /// \sa VResourceManager::SetAutoUnloadInterval
  /// 
  /// \example
  ///   \code
  ///   pMyManager->SetAutoPurgeInterval(10.f, 50.f); // checks all 10sec to purge resources that are unused for at least 50sec
  ///   \endcode
  VBASE_IMPEXP void SetAutoPurgeInterval(float fCheckInterval, float fTimeNotUsed, float fCurrentOfs=0.f);


  /// \brief
  ///   Sets a bitmask that controls the purging behavior of this resource manager
  /// 
  /// This bitmask is compared against the bitmask passed in
  /// VisResourceSystem_cl::PurgeAllResourceManager.
  /// 
  /// \param iMask
  ///   Bitmask value to filter resource manager. The following pre-defined bitmasks are supported
  ///   and used by the engine:
  ///   \li VRESOURCEMANAGER_PURGEMASK_AFTERSCENELOAD: This manager gets cleaned up after a new scene has been loaded.
  ///   \li VRESOURCEMANAGER_PURGEMASK_AFTERSCENEUNLOAD: This manager gets cleaned up at scene unload time.
  ///   \li VRESOURCEMANAGER_PURGEMASK_STREAMINGCLEANUP: This manager gets cleaned up when triggered by zone streaming.
  ///   \li VRESOURCEMANAGER_PURGEMASK_FORCEALL: This manager gets cleaned up at engine de-init time. This bit is always set.
  ///
  /// \sa VisResourceSystem_cl::PurgeAllResourceManager
  inline void SetPurgeMask(unsigned int iMask)
  {
    m_iPurgeMask = iMask|(unsigned int)VRESOURCEMANAGER_PURGEMASK_FORCEALL;
  }

  /// \brief
  ///   Returns the value that has been set via SetPurgeMask
  inline unsigned int GetPurgeMask() const
  {
    return m_iPurgeMask;
  }

  ///
  /// @}
  ///

  ///
  /// @name Memory
  /// @{
  ///

  /// \brief
  ///   Sets a memory limit (in bytes) for the resource manager.
  /// 
  /// Pass 0 for no limitation. Each time the limit is exceeded the resource manager will be
  /// purged, sorting the resources by relevance (i.e. purge not used resources first). The memory usage
  /// is computed with the virtual overridable GetCurrentMemoryUsageForMemLimit function which by default returns
  /// the sum of unique system and GPU memory.
  /// 
  /// \param iByteCount
  ///   New memory limit in bytes
  /// 
  /// \param iLimitType
  ///   Limit type constants; The following constants are supported:
  ///   \li VRESOURCEMANAGERFLAG_MEMLIMIT_SOFT : Soft limit (default); The limit will only be
  ///     checked once a frame and only  resources that are currently not used (time stamp) are
  ///     allowed to be purged.
  /// 
  ///   \li VRESOURCEMANAGERFLAG_MEMLIMIT_MEDIUM : Also checks once a frame, but all resource are
  ///     allowed to be purged
  /// 
  ///   \li VRESOURCEMANAGERFLAG_MEMLIMIT_HARD : Purges the manager as the limit is reached (not
  ///     recommended)
  ///
  /// \sa
  ///   GetCurrentMemoryUsageForMemLimit
  VBASE_IMPEXP void SetMemoryLimit(__int64 iByteCount, int iLimitType=VRESOURCEMANAGERFLAG_MEMLIMIT_SOFT);

  /// \brief
  ///   Returns the current memory limit in bytes
  inline __int64 GetMemoryLimit() const {return m_iMemLimit;}

  /// \brief
  ///   Gets the current memory limit type. See SetMemoryLimit for supported type constants.
  inline int GetMemoryLimitType() const {return m_iManagerFlags&VRESOURCEMANAGERFLAG_MEMLIMIT_MASK;}

  /// \brief
  ///   Returns the number of bytes used by all resources in this manager
  inline __int64 GetCurrentMemoryUsage() const 
  {
    return m_iCurrentSysMemUsage + m_iCurrentGPUMemUsage; // + m_iCurrentDependentSysMemUsage + m_iCurrentDependentGPUMemUsage;
  }

  /// \brief
  ///   Returns the number of bytes used by all resources in this manager. Sums up pool types that are
  ///   specified by the type bits parameter (can be bit-wise combined)
  inline __int64 GetCurrentMemoryUsage(VResourceMemoryType_e eTypeBits) const 
  {
    __int64 iSize = 0;
    if (eTypeBits&VRESOURCEMEMORY_SYSTEM) iSize += m_iCurrentSysMemUsage;
    if (eTypeBits&VRESOURCEMEMORY_GPU) iSize += m_iCurrentGPUMemUsage;
    if (eTypeBits&VRESOURCEMEMORY_DEPENDENT_SYSTEM) iSize += m_iCurrentDependentSysMemUsage;
    if (eTypeBits&VRESOURCEMEMORY_DEPENDENT_GPU) iSize += m_iCurrentDependentGPUMemUsage;
    return iSize;
  }

  /// \brief
  ///   Overridable function to return a memory size that is relevant for checking the memory limit.
  ///
  /// \returns
  ///   Number of bytes used by this resource manager
  ///
  /// This function is only called when a memory limit is actually set via SetMemoryLimit. By default this function returns
  /// the sum of unique system and GPU memory (and not the dependent memory).
  VBASE_IMPEXP virtual __int64 GetCurrentMemoryUsageForMemLimit() const;


  /// \brief
  ///   Returns the percentage of used memory according to the limit. If there is no limit, it
  ///   returns -1.0
  VBASE_IMPEXP float GetCurrentPercentageMemoryUsage() const;

  /// \brief
  ///   Internal function, do not use
  VBASE_IMPEXP void ResourceMemSizeChangedNotify(VManagedResource *pResource, int iDiffSysMem, int iDiffGPUMem, int iDiffDependentSysMem, int iDiffDependentGPUMem);

  ///
  /// @}
  ///

  ///
  /// @name Debugging / Displaying
  /// @{
  ///


  /// \brief
  ///   Sets a color for displaying in the resource viewer.
  inline  void SetDisplayColor(VColorRef iColorVal) {m_iDisplayColor=iColorVal;}

  /// \brief
  ///   Returns the display color.
  inline VColorRef GetDisplayColor() const {return m_iDisplayColor;}

  /// \brief
  ///   Internal function to set change flags.
  inline void SetChangedFlag(int iMask) {m_iChangedFlags|=iMask;}

  /// \brief
  ///   Returns a bitmask according to what changed in the manager since the last call of
  ///   ResetChangedFlags.
  /// 
  /// The return value can be a combination of the following bits:
  /// \li VRESOURCECHANGEDFLAG_ADDEDRESOURCE : One or more resources have been added.
  /// \li VRESOURCECHANGEDFLAG_REMOVEDRESOURCE : One or more resources have been removed.
  /// \li VRESOURCECHANGEDFLAG_MEMSIZECHANGED : The overall memory size has changed.
  /// \li VRESOURCECHANGEDFLAG_MEMLIMITCHANGED : The memory limit has changed.
  /// \li VRESOURCECHANGEDFLAG_LOADEDRESOURCE : One or more resources have been reloaded.
  /// \li VRESOURCECHANGEDFLAG_UNLOADEDRESOURCE : One or more resources have been unloaded.
  /// \li VRESOURCECHANGEDFLAG_RESOURCEMISSING : One or more resources have been marked as missing
  /// \li VRESOURCECHANGEDFLAG_FORCEVIEWERUPDATE : The viewer must be updated for no specific.
  ///   reason
  inline int GetChangedFlags() const {return m_iChangedFlags;}

  /// \brief
  ///   Resets the bitmask of changed properties. See VResourceManager::GetChangedFlags.
  inline void ResetChangedFlags() {m_iChangedFlags=VRESOURCECHANGEDFLAG_NONE;}

  /// \brief
  ///   Overridable function for debug output, e.g. for the console
  VBASE_IMPEXP virtual void DebugOutput(VActionManager *pOutput = NULL);

  /// \brief
  ///   Overridable function for writing an overview string to a string buffer
  VBASE_IMPEXP virtual void GetOverviewOutputString(char *szBuffer) const;

  /// \brief
  ///   Overridable function for dumping out resource leaks
  VBASE_IMPEXP virtual int DumpMemoryLeaks(BOOL bDeleteObjects=FALSE) {return 0;}

  /// \brief
  ///   Helper function that checks whether the sum of all resource mem sizes matches the overall
  ///   amount of memory.
  /// 
  /// If this function fails, maybe calls of VManagedResource::SetNewMemSize() are missing.
  /// 
  /// \example
  ///   \code
  ///   VASSERT(ValidateMemorySum());
  ///   \endcode
  VBASE_IMPEXP bool ValidateMemorySum();

  /// \brief
  ///   Overridable function to create a file in stream for loading resources
  VBASE_IMPEXP virtual IVFileInStream *CreateFileInStream(const char *szFilename, VManagedResource *pStoreTimeStampRes);

  /// \brief
  ///   Overridable function to create a file out stream
  VBASE_IMPEXP virtual IVFileOutStream *CreateFileOutStream(const char *szFilename, VManagedResource *pStoreTimeStampRes);

  /// \brief
  ///   Overridable function to access the file manager for disk operations
  VBASE_IMPEXP virtual IVFileStreamManager* GetFileManager() {return VBase_GetFileStreamManager();}

  /// \brief
  ///   Sets the filename for a resource
  ///
  /// \param pResource
  ///   the resource to set the name for
  ///
  /// \param index
  ///   the index the resource is stored at
  ///
  /// \param szOldName
  ///   The old name of the resource, needed so the old entry in the hashmap can be removed.
  VBASE_IMPEXP void SetResourceFilename(VManagedResource *pResource, int index, const char* szOldName = NULL);


#ifdef SUPPORTS_RESOURCEVIEWER
  /// \brief
  ///   Update any possible listening resource viewers.
#if defined(WIN32) && !defined(_VISION_WINRT)
  static BOOL UpdateResourceViewer(HWND hWnd, HWND hWndSender, const VResourceManager **pManagerList, int iListCount, VConnection* pRemoteConnection = NULL);
#else
  static BOOL UpdateResourceViewer(const VResourceManager **pManagerList, int iListCount, VConnection* pRemoteConnection);
#endif

  /// \brief
  ///  Checks if a resource viewer is connected or not
  VBASE_IMPEXP static bool IsResourceViewerConnected();

  /// \brief
  ///   Register to this callback to listen to update resource viewer events
  VBASE_IMPEXP static VCallback OnResourceViewerUpdate;

#endif

  /// \brief Internal function.
  inline void PackPointers()
  {
    m_resources.RemoveFlagged();
    EnumResources();
  }

protected:
  VBASE_IMPEXP void EnumResources();
  VBASE_IMPEXP void AdjustUpperLimit();


  ///
  /// @}
  ///

protected:
  friend class VTextureObject;
  friend class VResourceSystem_cl;

  // internal functions for resources to call
  VBASE_IMPEXP virtual BOOL UnloadSingleResource(VManagedResource *pResource) {return FALSE;}
  VBASE_IMPEXP virtual BOOL ReloadSingleResource(VManagedResource *pResource) {return FALSE;}

  inline void SetSystemManagerFlag() {m_iManagerFlags|=VRESOURCEMANAGERFLAG_SYSTEM_MANAGER;}
  char *m_szManagerName;

// manager properties
  int m_iManagerFlags;

// resources
  VResourceList m_resources;
  VResourceList m_resourcesToRestore;
  int m_iNumResourcesToRestore;
  //TODO replace this with a real hashmap to avoid ugly casting
  VStrMap<void> m_resourcesByName;

  bool m_bHandlesBackgrounding;

// mem limits
  __int64 m_iMemLimit;
  __int64 m_iCurrentSysMemUsage,m_iCurrentGPUMemUsage,m_iCurrentDependentSysMemUsage,m_iCurrentDependentGPUMemUsage;

// auto unload/purge
  float m_fAutoUnloadTime, m_fAutoPurgeTime;
  float m_fAutoUnloadPos, m_fAutoPurgePos;
  float m_fAutoUnloadTNU, m_fAutoPurgeTNU;
  bool m_bAutoUnloadUnRefOnly;

// debug
  VColorRef m_iDisplayColor;
  int m_iChangedFlags;

  VResourceList *m_pTempSortedList;
  unsigned int m_iPurgeMask;

public:
  /// \brief
  ///   Register to this callback to listen to resource changed events
  VCallback OnResourceChanged;

  VBASE_IMPEXP static VCallback *g_pResChangedCallback;

  /// \brief
  ///   Returns the current resource file resolver. This is always !=NULL. The default resolver
  ///   always returns identity.
  VBASE_IMPEXP static IVFilePathResolver& GetFilePathResolver();

  /// \brief
  ///   Sets a custom resource file resolver. NULL can be passed to restore default behaviour.
  /// 
  /// A resource file resolver is useful to redirect all resource loading operations that actually
  /// load from file.
  /// 
  /// The resource will be stored under the new name - thus resolving a path in the filemanager is
  /// not an alternative to this functionality as this would generate unique resources.
  /// 
  /// \param pResolver
  ///   The new resolver (or NULL)
  VBASE_IMPEXP static void SetFilePathResolver(IVFilePathResolver *pResolver);
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
