/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VResource.hpp

#ifndef _VRESOURCE_HPP_INCLUDED
#define _VRESOURCE_HPP_INCLUDED

#include <Vision/Runtime/Base/System/IO/Serialization/VSerializationProxy.hpp>

/// \brief Flags for resources.
enum VResourceFlag
{
  VRESOURCEFLAG_NONE                     = 0,
  VRESOURCEFLAG_ISLOADED                 = V_BIT(0),
  VRESOURCEFLAG_ISLOADING                = V_BIT(1),
  VRESOURCEFLAG_ISQUEUED                 = V_BIT(2),
  VRESOURCEFLAG_ISMISSING                = V_BIT(3),
  VRESOURCEFLAG_DESTROYING               = V_BIT(4),
  VRESOURCEFLAG_ISREPLACEMENT            = V_BIT(5),
  VRESOURCEFLAG_FIRSTTIME                = V_BIT(6),
  VRESOURCEFLAG_ALLOWHANDLEBACKGROUNDING = V_BIT(7),

  VRESOURCEFLAG_ALLOWUNLOAD              = V_BIT(8),
  VRESOURCEFLAG_AUTODELETE               = V_BIT(9),
  VRESOURCEFLAG_AUTOUNLOAD               = V_BIT(10),
  VRESOURCEFLAG_IGNORE_SURVIVING         = V_BIT(11),
  VRESOURCEFLAG_MODIFIABLEFLAGS          = (VRESOURCEFLAG_IGNORE_SURVIVING | VRESOURCEFLAG_AUTOUNLOAD | VRESOURCEFLAG_AUTODELETE | VRESOURCEFLAG_ALLOWUNLOAD),
  VRESOURCEFLAG_NOPURGING                = V_BIT(12),
  VRESOURCEFLAG_VIEWER_WANTSINFO         = V_BIT(13),
  VRESOURCEFLAG_USERFLAG                 = V_BIT(14),
  VRESOURCEFLAG_TEMPTAG                  = V_BIT(15),

  VRESOURCEFLAG_LOADING_RELATED        = (VRESOURCEFLAG_ISLOADED|VRESOURCEFLAG_ISLOADING|VRESOURCEFLAG_ISQUEUED|VRESOURCEFLAG_ISMISSING),
};


class VManagedResource;
class VResourceManager;
struct VFileTime;
class IVFileInStream;
class IVFileOutStream;
class IVFileStreamManager;
class VResourceSnapshot;
class VResourceSnapshotEntry;
class VThreadedTask;
class IVResourcePreview;

typedef VSmartPtr<VManagedResource> VManagedResourcePtr;

// resource helpers:
#define VSAFE_UNTAG_RES(res) if (res!=NULL) {res->RemoveResourceFlag(VRESOURCEFLAG_TEMPTAG);}
#define VSAFE_COUNT_RESMEM(res,counter_sys,counter_gpu) \
  if (res!=NULL && !res->IsResourceFlagSet(VRESOURCEFLAG_TEMPTAG)) \
    {counter_sys+=res->GetSysMemSize();counter_gpu+=res->GetGPUMemSize();res->SetResourceFlag(VRESOURCEFLAG_TEMPTAG);}\

/// \brief
///   Helper macro to lock a resource
/// 
/// This macro locks the resource within the variable scope of this macro, e.g. within a code block
/// or a whole function.
#define V_LOCK_RESOURCE(pResource) VResourceLock LockInstance_##pRes(pResource)

/// \brief
///   Streamable Resource class that is attached to a resource manager (class VResourceManager)
class VManagedResource : public VRefCounter, public IVSerializationProxyCreator
{
public:


  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Resource constructor
  /// 
  /// \param pParentManager
  ///   The resource manager this resource is attached to.
  VBASE_IMPEXP VManagedResource(VResourceManager *pParentManager);

  /// \brief
  ///   Resource destructor
  VBASE_IMPEXP virtual ~VManagedResource();

  ///
  /// @}
  ///

  ///
  /// @name Resource Name
  /// @{
  ///

  /// \brief
  ///   Returns the filename of the resource
  /// 
  /// This filename can be used to load a resource from file in the Reload() function.
  /// 
  /// This version removes a '\' at the beginning (which marks the resource filename as absolute)
  /// so this version can be used for loading the resource.
  /// 
  /// To actually get the '\' at the beginning, use the GetOriginalFilename function
  /// 
  /// A new filename can be set via SetFilename
  ///
  /// UNC style paths are handled correctly, ex. \\[server]\[dir]
  /// Currently also treats /\[server]\[dir] as UNC-path
  /// 
  /// \return
  ///   const char *szFilename : Current filename string. Can be NULL.
  /// 
  /// \sa VManagedResource::SetFilename()
  inline const char *GetFilename() const
  {
#if defined(_VISION_PS3)
    // do not remove the starting "/" on the PS3
    if (m_szFilename && (_strnicmp(m_szFilename, "/app_home/", 10) == 0))
      return m_szFilename;
#elif defined(_VISION_WIIU)
    // do not remove the starting "/" on the WIIU
    if (m_szFilename && (_strnicmp(m_szFilename, "/vol/content/", 13) == 0))
      return m_szFilename;
#elif defined(_VISION_ANDROID)
    // do not remove the starting "/" on Android when accessing the data directory
    if (m_szFilename && 
      (_strnicmp(m_szFilename, "/data/", 6) == 0 ||
      _strnicmp(m_szFilename, "/storage/", 9) == 0 ||
      _strnicmp(m_szFilename, "/mnt/sdcard/", 12) == 0))
      return m_szFilename;
#elif defined(_VISION_IOS)
    // do not remove the starting "/" on iOS when accessing the var directory
    if (m_szFilename && (_strnicmp(m_szFilename, "/var/", 5) == 0))
      return m_szFilename;
#endif

    if (m_szFilename && ( m_szFilename[0]=='\\' || m_szFilename[0]=='/'))
    {
#ifdef WIN32
      // Decide if filename is UNC-path style filename
      if(m_szFilename[1] != '\\')
#endif
        return &m_szFilename[1];
    }
    return m_szFilename;
  }


  /// \brief
  ///   Returns the unmodified filename of the resource, i.e. the returned path might start with a slash
  inline const char *GetOriginalFilename() const
  {
    return m_szFilename;
  }

  /// \brief
  ///   Compares the hash of the internally stored filename with the given hash. If both hashes are identical, the filenames are additionally compared case-insensitive. Returns false if any of those comparisons fails.
  /// \param szFileName
  ///   File to be compared with internal filename
  /// \param filenameHash
  ///   Hash value to be compared, if you need to pass this value directly, you can use VPathHelper::GetHash
  /// \sa VPathHelper::GetHash()
  /// \sa VPathHelper::CompareNoSlashes()
  VBASE_IMPEXP BOOL CompareFileName(const char * szFileName, unsigned int filenameHash) const;

  /// \brief
  ///   Sets a new filename for the resource. Note that this function will not call any loading routines.
  /// 
  /// \param szFilename
  ///   New filename string. Can be NULL.
  /// 
  /// \sa VManagedResource::GetFilename()
  VBASE_IMPEXP void SetFilename(const char *szFilename);

  ///
  /// @}
  ///

  ///
  /// @name Memory
  /// @{
  ///

  /// \brief
  ///   Returns the number of bytes used by this resource.
  /// 
  /// Each resource class has to estimate the amount of memory used and set it via SetNewMemSize
  /// whenever the amount changes.
  ///
  /// \param eTypeBits
  ///   The allocated memory types to include in the computation. See VResourceMemoryType_e for available flags.
  /// 
  /// \return
  ///   int iMemSize: The amount of memory, in bytes.
  /// 
  /// \sa VManagedResource::SetNewMemSize()
  inline int GetMemSize(VResourceMemoryType_e eTypeBits) const 
  {
    int iSum = 0;
    if  (eTypeBits & VRESOURCEMEMORY_SYSTEM)            iSum += m_iUniqueSysMemSize;
    if  (eTypeBits & VRESOURCEMEMORY_GPU)               iSum += m_iUniqueGPUMemSize;
    if  (eTypeBits & VRESOURCEMEMORY_DEPENDENT_SYSTEM)  iSum += m_iDependentSysMemSize;
    if  (eTypeBits & VRESOURCEMEMORY_DEPENDENT_GPU)     iSum += m_iDependentGPUMemSize;
    return iSum;
  }

  /// \brief
  ///   Returns the sum of all memory types (unique and dependent, system mem and GPU). This number is not very meaningful,
  ///   instead it is for backwards compatibility
  inline int GetMemSize() const 
  {
    return m_iUniqueGPUMemSize + m_iUniqueSysMemSize + m_iDependentSysMemSize + m_iDependentGPUMemSize;
  }

  //// \brief Returns the amount of memory used on the GPU.
  inline int GetGPUMemSize() const {return m_iUniqueGPUMemSize;}

  //// \brief Returns the amount of memory used in system RAM.
  inline int GetSysMemSize() const {return m_iUniqueSysMemSize;}

  //// \brief Returns the amount of system memory used by resources that this resource depends on.
  inline int GetDependentSysMemSize() const {return m_iDependentSysMemSize;}

  //// \brief Returns the amount of GPU memory used by resources that this resource depends on.
  inline int GetDependentGPUMemSize() const {return m_iDependentGPUMemSize;}

  /// \brief Adds the used system and GPU memory (own and dependent) to the given variables iDestSys and iDestGPU.
  inline void IncOverallMemSizes(size_t &iDestSys, size_t &iDestGPU) const 
  {
    iDestSys += m_iUniqueSysMemSize + m_iDependentSysMemSize;
    iDestGPU += m_iUniqueGPUMemSize + m_iDependentGPUMemSize;
  }

  ///
  /// @}
  ///

  ///
  /// @name Time Stamp
  /// @{
  ///

  /// \brief
  ///   Sets the resource's file time stamp value. The time stamp is retrieved from the passed stream.
  VBASE_IMPEXP BOOL SetTimeStamp(IVFileInStream *pStream);

  /// \brief
  ///   Sets the resource's file time stamp value. The time stamp is retrieved from the passed stream.
  VBASE_IMPEXP BOOL SetTimeStamp(IVFileOutStream *pStream);

#ifdef SUPPORTS_RESOURCE_TIMESTAMP

  /// \brief
  ///   Overridable to gather a new file time stamp. Must be overridden in case a different file
  ///   than GetFilename() is opened.
  VBASE_IMPEXP virtual BOOL GatherTimeStamp(IVFileStreamManager *pManager, VFileTime &destTime);
#endif

  /// \brief
  ///   Sets the resource's file stamp value directly.
  VBASE_IMPEXP BOOL SetTimeStamp(const VFileTime& timeVal);

  /// \brief
  ///   Returns the resource's file time stamp.
  /// \return
  ///   the resource's file time stamp. The returned time may be invalid if no 
  ///   file time is known or resource timestamps are not supported on the target platform.
  VBASE_IMPEXP VFileTime GetFileTimeStamp() const;

  /// \brief
  ///   Sets the resource's asset lookup hash directly.
  /// \param iHashValue
  ///   the new hash value to set
  VBASE_IMPEXP void SetAssetLookupHash(unsigned int iHashValue);

  /// \brief
  ///   Checks if the resource has been modified.
  /// 
  /// The stream manager is used to retrieve the new time stamp.
  /// 
  /// For further details on the passed parameters, see VResourceManager::ReloadModifiedResourceFiles.
  /// 
  /// \param pManager
  ///   The file stream manager used for retrieving the time stamp. Should be
  ///   Vision::File.GetManager() when used in the engine.
  /// 
  /// \param bUnload
  ///   If TRUE, the resource will be unloaded in case it has been modified (calls EnsureUnloaded
  ///   on the resource).
  /// 
  /// \param bReload
  ///   If TRUE, the resource will be reloaded in case it has been modified (calls EnsureLoaded on
  ///   the resource).
  /// 
  /// \return
  ///   TRUE if the resource has been detected to be modified
  VBASE_IMPEXP BOOL CheckFileModified(IVFileStreamManager* pManager, BOOL bUnload = TRUE, BOOL bReload = TRUE);

  /// \brief
  ///   Ignores file changes, so that the resource doesn't get reloaded the next time CheckFileModified()
  ///   is called.
  ///
  /// Effectively sets the timestamp to the current value.
  ///
  /// \param pManager
  ///   The file stream manager used for retrieving the time stamp. Should be
  ///   Vision::File.GetManager() when used in the engine.
  VBASE_IMPEXP void IgnoreFileChanges(IVFileStreamManager* pManager);

  ///
  /// @}
  ///

  ///
  /// @name Resource Flags
  /// @{
  ///

  /// \brief
  ///   Sets a resource flag on this resource.
  /// 
  /// The internal flags will be OR'd with the specified mask. No flag will be removed.
  /// See the VResourceFlag enum for details on the available flags.
  /// 
  /// Valid flags for resources are:
  /// \li VRESOURCEFLAG_ALLOWUNLOAD : The resource is allowed to unload itself, e.g. when the
  ///   memory limits are exceeded. This flag is only relevant for managed resources
  ///   (VManagedResource)
  /// 
  /// \li VRESOURCEFLAG_AUTODELETE : The resource will delete itself once the reference counter
  ///   reaches zero. Unlike simple refcounted objects, managed resources can actually "decide"
  ///   what to do when getting unreferenced. This flag is only relevant for VManagedResource
  /// 
  /// \li VRESOURCEFLAG_AUTOUNLOAD : The resource will unload itself once the reference counter
  ///   reaches zero, but the resource object remains valid. This flag is only relevant for
  ///   VManagedResource.
  /// 
  /// \li VRESOURCEFLAG_IGNORE_SURVIVING : If specified, this single resource will not be reported
  ///   as surviving a load world. There is also a per manager flag
  ///   VRESOURCEMANAGERFLAG_IGNORE_SURVIVING_RESOURCES that ignores all resources in the manager.
  /// 
  /// \li VRESOURCEFLAG_NOPURGING : If specified, this resource is ignored during all
  ///   VResourceManager::PurgeUnusedResources operations. This flag must thus be removed again
  ///   before deinitializing the application, otherwise it causes a leak/crash. 
  /// 
  /// \li VRESOURCEFLAG_USERFLAG : A flag that can be specified by the user. The engine ignores it.
  ///
  /// \param iMask
  ///   bitmask to add to the flags. See Description for valid bitmasks.
  /// 
  /// \sa VManagedResource
  /// \sa VResourceManager
  inline void SetResourceFlag(int iMask)          {m_iResourceFlag |= iMask;}

  /// \brief
  ///   Removes specified bits from the internal flags mask.
  /// 
  /// For valid flags, see description of VResource::SetResourceFlag.
  /// 
  /// \param iMask
  ///   bitmask to remove from the flags.
  /// 
  /// \sa VResource::SetResourceFlag
  /// \sa VManagedResource
  /// \sa VResourceManager
  inline void RemoveResourceFlag(int iMask)       {m_iResourceFlag &= (~iMask);}

  /// \brief
  ///   Checks if a specified flag is set.
  /// 
  /// If more than one flag is specified, it tests whether all flags are set.
  /// 
  /// \param iMask
  ///   bitmask to check, usually a one bit constant
  /// 
  /// \return
  ///   BOOL bResult : TRUE, if the passed bit is set
  /// 
  /// \sa VResource::SetResourceFlag
  /// \sa VResource::GetResourceFlag
  /// \sa VManagedResource
  /// \sa VResourceManager
  inline BOOL IsResourceFlagSet(int iMask) const  {return (m_iResourceFlag & iMask)==iMask;}

  ///
  /// @}
  ///

  ///
  /// @name Resource States
  /// @{
  ///

  /// \brief
  ///   Returns true, if the resource is currently loaded.
  /// 
  /// \sa VManagedResource
  /// \sa VResourceManager
  inline BOOL IsLoaded() const          {return IsResourceFlagSet(VRESOURCEFLAG_ISLOADED);}

  /// \brief
  ///   Returns TRUE if the resource is currently loading.
  inline BOOL IsLoading() const         {return IsResourceFlagSet(VRESOURCEFLAG_ISLOADING);}

  /// \brief
  ///   Returns TRUE if the resource is currently queued for loading.
  inline BOOL IsQueued() const         {return IsResourceFlagSet(VRESOURCEFLAG_ISQUEUED);}

  /// \brief
  ///   Indicates whether resource (file) is marked as missing.
  inline BOOL IsMissing() const         {return IsResourceFlagSet(VRESOURCEFLAG_ISMISSING);}

  /// \brief
  ///   Indicates whether the resource is currently in low-res replacement state.
  inline BOOL IsReplacement() const         {return IsResourceFlagSet(VRESOURCEFLAG_ISREPLACEMENT);}

  /// \brief
  ///   Flag that determines whether the resource can currently unload itself. Used by the resource manager.
  inline BOOL CanUnload() const         {return IsResourceFlagSet(VRESOURCEFLAG_ALLOWUNLOAD) && !IsLocked();}

  /// \brief
  ///   Returns the time stamp (in seconds) at which the resource has been used the last time
  /// 
  /// \sa VManagedResource::GetLastTimeUsedDiff
  /// \sa VManagedResource::EnsureLoaded
  /// \sa VManagedResource
  /// \sa VResourceManager
  inline float GetLastTimeUsed() const  {return m_fLastTimeUsed;}

  /// \brief
  ///   Returns how long this resource has not been used (in seconds).
  /// 
  /// \sa VManagedResource::GetLastTimeUsed
  /// \sa VManagedResource::EnsureLoaded
  /// \sa VManagedResource
  /// \sa VResourceManager
  inline float GetLastTimeUsedDiff() const  {return GetGlobalTime() - m_fLastTimeUsed;}

  /// \brief
  ///   Sets the "loaded" flag of the resource, so IsLoaded() returns TRUE. Useful for some
  ///   implementations of custom resources.
  inline void FlagAsLoaded()    {m_fLastTimeUsed = GetGlobalTime();SetResourceFlag(VRESOURCEFLAG_ISLOADED);}

  /// \brief
  ///   Removes all "loaded" relating flags from the resource, so IsLoaded() returns FALSE. Useful
  ///   for some implementations of custom resources.
  inline void FlagAsUnLoaded()  {RemoveResourceFlag(VRESOURCEFLAG_LOADING_RELATED);}

  /// \brief
  ///   Sets the "missing" flag. This flag can be combined with the loaded flag so a resource can both return IsLoaded and IsMissing
  VBASE_IMPEXP void FlagAsMissing();

  /// \brief
  ///   Removes the "missing" flag that was previously set via FlagAsMissing()
  ///
  /// This might be used to indicate that the resource files are now available. However, this call does not trigger any reload of the resource.
  inline void RemoveMissingFlag()  {RemoveResourceFlag(VRESOURCEFLAG_ISMISSING);}

  ///
  /// @}
  ///

  ///
  /// @name Load/unload
  /// @{
  ///

  /// \brief
  ///   Guarantees that the resource is loaded
  /// 
  /// One of the key functions of resource management. After calling this function it is
  /// guaranteed, that the resource is loaded. If the resource wasn't already loaded, the virtual
  /// Reload() function is called. Furthermore it updates the time stamp for the last accessed
  /// time. Call this function before any custom resource functions are called that rely on data
  /// being allocated in the Reload() function.
  inline void EnsureLoaded()
  {
    m_fLastTimeUsed = GetGlobalTime();
    if (IsLoaded()) return;
    DoReload();
  }

  /// \brief
  ///   Updates the time stamp of the resource
  inline void UpdateTimeStamp() { m_fLastTimeUsed = GetGlobalTime(); }

  /// \brief
  ///   Ensures that the resource is unloaded
  inline void EnsureUnloaded()
  {
    if (!IsLoaded()) return;
    VASSERT(CanUnload()); ///< this should be tested outside this function!
    DoUnload();
  }

  /// \brief
  ///   Unloads and reloads the resource, based on the flags passed. Additionally triggers any
  ///   registered callbacks for before/after file modified.
  VBASE_IMPEXP virtual void UnloadAndReload(BOOL bUnload, BOOL bReload);

  /// \brief triggers a resource changed callback
  /// \param changedFlag
  ///    the resource changed flag to trigger (one of VRESOURCECHANGEDFLAG_...)
  VBASE_IMPEXP void TriggerResourceChangedCallback(int changedFlag);

  /// \brief
  ///   Increases the lock counter of the resource. 
  /// 
  /// A locked resource cannot be unloaded. Calling EnsureUnloaded on a locked resource will cause
  /// an assertion. This function will also call EnsureLoaded() which is useful in most cases.
  /// 
  /// \sa V_LOCK_RESOURCE
  /// \sa VManagedResource::CanUnload()
  /// \sa VManagedResource::EnsureUnloaded()
  /// \sa VManagedResource::AddLockNoLoad()
  /// \sa VManagedResource::ReleaseLock()
  /// \sa VManagedResource::IsLocked()
  inline void AddLock()         {m_iLockCounter++;EnsureLoaded();}

  /// \brief
  ///   Same as VManagedResource::AddLock() but does not call EnsureLoaded()
  /// 
  /// \sa V_LOCK_RESOURCE
  /// \sa VManagedResource::AddLock()
  /// \sa VManagedResource::CanUnload()
  /// \sa VManagedResource::EnsureUnloaded()
  /// \sa VManagedResource::ReleaseLock()
  /// \sa VManagedResource::IsLocked()
  inline void AddLockNoLoad()   {m_iLockCounter++;}

  /// \brief
  ///   Decrements the lock counter
  /// 
  /// \sa V_LOCK_RESOURCE
  /// \sa VManagedResource::AddLock()
  /// \sa VManagedResource::CanUnload()
  /// \sa VManagedResource::EnsureUnloaded()
  /// \sa VManagedResource::ReleaseLock()
  /// \sa VManagedResource::IsLocked()
  inline void ReleaseLock()     {VASSERT(m_iLockCounter>0);m_iLockCounter--;}

  /// \brief
  ///   Determines the lock status
  /// 
  /// \return
  ///   BOOL bLocked: TRUE, if the lock counter is larger than zero
  /// 
  /// \sa V_LOCK_RESOURCE
  /// \sa VManagedResource::AddLock()
  /// \sa VManagedResource::CanUnload()
  /// \sa VManagedResource::EnsureUnloaded()
  /// \sa VManagedResource::ReleaseLock()
  inline BOOL IsLocked() const  {return m_iLockCounter>0;}


  ///
  /// @}
  ///

  ///
  /// @name Overridables
  /// @{
  ///

  /// \brief
  ///   Overridable function to reload the resource
  /// 
  /// Resource implementations should do the reloading in this function, e.g. loading from file or
  /// allocating data.
  /// 
  /// Use the GetFilename() function to access the resource's filename.
  /// 
  /// After successfully returning from this function, the engine will:
  ///   \li flag this resource as loaded (set time stamp and set VRESOURCEFLAG_ISLOADED flag),
  ///   \li call the UpdateMemoryFootprint to evaluate a new memory footprint (which is also virtual).
  ///   \li finally remove the VRESOURCEFLAG_FIRSTTIME flag. That means, that inside the first call of Reload() the VRESOURCEFLAG_FIRSTTIME flag is still set.
  /// 
  /// \return
  ///   BOOL bResult: Return TRUE, if loading the resource was successful.
  /// 
  /// \sa VManagedResource::Unload()
  /// \sa VManagedResource::EnsureLoaded()
  /// \sa VManagedResource::UpdateMemoryFootprint()
  /// \sa VManagedResource::AccumulateMemoryFootprint()
  VBASE_IMPEXP virtual BOOL Reload() = 0;

  /// \brief
  ///   Overridable function to unload the resource
  /// 
  /// Resource implementations should do the deinitialization in this function, e.g. deallocating
  /// the memory.
  /// 
  /// At the end of the Unload() function the new memory size should be set to zero via
  /// SetNewMemSize.
  /// 
  /// \return
  ///   BOOL bResult: Return TRUE, if unloading the resource was successful.
  /// 
  /// \sa VManagedResource::SetNewMemSize()
  /// \sa VManagedResource::Reload()
  /// \sa VManagedResource::EnsureLoaded()
  /// \sa VManagedResource::EnsureUnloaded()
  VBASE_IMPEXP virtual BOOL Unload() = 0;

  /// \brief
  ///   Overridable function to gather the footprint of all memory types for this resource.
  ///
  /// This function should be overridden. Implementations should add the actual footprints to the different types. The values
  /// might be set by base implementations, accordingly it is crucial to call the base implementation.
  ///
  /// \param iUniqueSys
  ///   Reference target to add the number of bytes of system memory that are uniquely used by this resource.
  /// \param iUniqueGPU
  ///   Reference target to add the number of bytes of GPU memory that are uniquely used by this resource.
  /// \param iDependentSys
  ///   Reference target to add the number of bytes that other resources consume that this resource depends on.
  ///   For example, model resources should add the unique memory of their material textures here.
  /// \param iDependentGPU
  ///   Reference target to add the number of bytes that other resources consume that this resource depends on.
  ///   For example, model resources should add the unique memory of their material textures here.
  VBASE_IMPEXP virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
  {
  }

  /// \brief
  ///   Triggers an update of the memory footprint, i.e. calls AccumulateMemoryFootprint.
  ///
  /// This function should be called whenever the memory profile of this resource changes. It is however automatically
  /// called when the resource unloads or reloads.
  VBASE_IMPEXP void UpdateMemoryFootprint();


  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Returns a pointer to the resource manager this resource is attached to
  inline VResourceManager *GetParentManager() const {return m_pParentManager;}


  /// \brief
  ///   Releases one reference (non-virtual overload of VRefCounter function). This function will
  ///   "decide" what to do once the refcounter reaches zero.
  VBASE_IMPEXP void Release();

#ifdef HK_DEBUG
  /// \brief Internal function.
  VBASE_IMPEXP unsigned long AddRef();
#endif

  /// \brief
  ///   Tries to purge a resource. 
  /// 
  /// Purging is only possible if the resource is not referenced anymore and also not locked. This
  /// function returns TRUE if the object has been deleted.
  VBASE_IMPEXP BOOL Purge();

  /// \brief
  ///   Returns the 0-based list index of this resource within the resource manager it is attached to.
  inline unsigned int GetNumber() const {return m_iListIndex;}

  /// \brief
  ///   Indicates whether there are external references to this resource. Can be overridden.
  /// 
  /// The resource manager always has a reference and this reference is not considered in this
  /// function. This overridable is used by the resource manager to determine whether a resource can be purged.
  VBASE_IMPEXP virtual bool IsUsed() const;

  ///
  /// @}
  ///

  ///
  /// @name Overridables
  /// @{
  ///


  /// \brief
  ///   Overridable that adds all dependencies to the snapshot object.
  VBASE_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot);

  /// \brief
  ///   Flag that determines whether this resource should be put into the resource snapshot. By default this function returns TRUE if the resource name is a valid filename
  VBASE_IMPEXP virtual BOOL IsRelevantForSnapshot(VResourceSnapshot &snapshot) const;

  /// \brief
  ///   This function can be used to show some additional information in the resource viewer.
  /// 
  /// It gets called by the engine with a string buffer to fill.
  /// 
  /// This string will be shown in the resource viewer.
  /// 
  /// \param szDestBuffer
  ///   String buffer to fill. Gets passed by the engine.
  /// 
  /// \param iMaxChars
  ///   Maximum number of characters that can be filled into the buffer.
  /// 
  /// \return
  ///   Number of characters written into the buffer. Return -1 to ignore additional info.
  VBASE_IMPEXP virtual int GetAdditionalOutputString(char *szDestBuffer, int iMaxChars) {return -1;}

  /// \brief
  ///   Fills a string buffer with resource information, e.g for the console.
  /// 
  /// \param szBuffer
  ///   String buffer to fill. Gets passed by the engine.
  /// 
  /// \param bUseDecRef
  ///   If TRUE, the output of the refcounter is decremented to compensate for the manager's
  ///   reference to it. Thus refcount=0 means no external reference to the object.
  VBASE_IMPEXP virtual void GetDebugOutputString(char *szBuffer, BOOL bUseDecRef) const;

  ///  \brief
	///    Returns a resource preview object for vForge.
	///
	///  \returns
	///    This function returns NULL on all platforms except for WIN32 since it is only relevant for vForge.
	///
	///  \see
	///    IVResourcePreview
	VBASE_IMPEXP virtual IVResourcePreview *CreateResourcePreview();
 

  /// \brief
  ///   Overridable notification function that gets called when the resource is added to a resource manager
  VBASE_IMPEXP virtual void OnAddResource(){}

  /// \brief
  ///   Overridable notification function that gets called when the resource is removed from a
  ///   resource manager
  VBASE_IMPEXP virtual void OnRemoveResource(){}


  /// \brief
  ///   Overridable function to update the memory of a specific pool type.
  ///   This function is for backwards compatibility. Override AccumulateMemoryFootprint instead.
  VBASE_IMPEXP virtual void SetNewMemSize(VResourceMemoryType_e eMemType, int iNewSize);

  /// \brief
  ///   OnEnterBackground event for the resource. 
  ///
  /// This method will be called for all resources whenever a backgrounding/lost device event 
  /// occurs and allows the  application to react appropriately. Further information about how to 
  /// react to a lost device can be found in the DirectX9 SDK documentation. 
  VBASE_IMPEXP virtual void OnEnterBackground();

  /// \brief
  ///   OnLeaveBackground event for the resource.
  ///
  /// This method will be called for all resources whenever the application is about to come
  /// back to the foreground again. It allows the application to react appropriately to this event, 
  /// for instance by recreating GPU resources deleted as a response to a previous DirectX 9 lost 
  /// device or an Android backgrounding event. Further information about how to react to a lost device 
  /// can be found in the DirectX9 SDK documentation.
  VBASE_IMPEXP virtual void OnLeaveBackground();


  ///
  /// @}
  ///

  ///
  /// @name Static Helper Functions
  /// @{
  ///

  /// \brief
  ///   Sets the global time stamp. Called by the engine.
  static inline void SetGlobalTime(float fTime)   {g_fGlobalTime=fTime;}

  /// \brief
  ///   Increments the global time stamp. Called by the engine.
  static inline void IncGlobalTime(float fDelta)  {g_fGlobalTime+=fDelta;}

  /// \brief
  ///   Gets the global time value. Used for assigning a time stamp in EnsureLoaded()
  static inline float GetGlobalTime()             {return g_fGlobalTime;}

  /// \brief
  ///   Formats a memory size to a nice string, such as 4.1MB
  VBASE_IMPEXP static void Helper_FormatMemSize(__int64 iMem, char *szBuffer);


protected:
  VBASE_IMPEXP virtual void DoReload(); ///< internal function, do not use
  VBASE_IMPEXP virtual void DoUnload(); ///< internal function, do not use
  VBASE_IMPEXP virtual IVSerializationProxy* CreateProxy() HKV_OVERRIDE {return NULL;} ///< internal function, do not use
  VBASE_IMPEXP virtual void ResetFilename(); ///< internal function, do not use

protected:
  friend class VResourceManager;

  VResourceManager *m_pParentManager;     ///< pointer to parent manager
  int m_iListIndex, m_iRestoreListIndex;  ///< list index in parent manager
  unsigned short m_iResourceFlag;
  unsigned short m_iLockCounter;

  float m_fLastTimeUsed;    ///< time stamp when last used
  unsigned int m_iUniqueSysMemSize, m_iUniqueGPUMemSize;
  unsigned int m_iDependentSysMemSize, m_iDependentGPUMemSize;

#ifdef SUPPORTS_RESOURCE_TIMESTAMP
  vtime_t m_iFileTimeStamp;
#endif

  unsigned int m_uiAssetLookupHash;

  ///
  /// @}
  ///

private:

  ///
  /// @name Private properties, because it need to be changed both filename & hash by calling SetFilename method.
  /// @{
  ///

  // Use following method GetOriginalFilename() / SetFilename() instead
  char *m_szFilename;       ///< filename string
  unsigned int m_filenameHash;

  ///
  /// @}
  ///

  VBASE_IMPEXP static float g_fGlobalTime;

};



/// \brief
///   Guarding class that locks a resource within the class instance's scope. Used by the V_LOCK_RESOURCE macro.
class VResourceLock
{ 
public:
  VResourceLock(VManagedResource *pRes) {m_pRes = pRes;if (m_pRes) m_pRes->AddLock();}
  ~VResourceLock()                      {if (m_pRes) m_pRes->ReleaseLock();}

private:
  VManagedResource *m_pRes;
};




#define RESOURCEPREVIEWFLAG_NONE               0
#define RESOURCEPREVIEWFLAG_FOR_THUMBNAIL      1

/// \brief
///   This is a class that is used by vForge to display resource previews.
///
/// In order to incorporate this feature for custom resources, the VManagedResource::CreateResourcePreview of the resource
/// class must be overridden and return a new instance of a class derived from IVResourcePreview. vForge calls functions
/// of this interface automatically.
/// Furthermore this class can be used to save thumbnails of custom resources so they can be shown by the file open dialog in vForge.
/// Since this class is refcounted the system takes care to release/destroy instances provided by VManagedResource::CreateResourcePreview.
///
/// \see
///   VManagedResource::CreateResourcePreview
class IVResourcePreview : public VRefCounter
{
public:

  /// \brief
	///   Constructor. Takes the owner resource that is associated with this preview object
	inline IVResourcePreview(VManagedResource *pOwner)
  {
    m_spOwner = pOwner;
    m_bRenderMetaData = true;
  }

  /// \brief
	///   Returns the owner resource as passed to the constructor
  inline VManagedResource* GetOwner() const 
  {
    return m_spOwner;
  }

  /// \brief
  ///   Called by vForge to inform this preview if meta data should be rendered or not.
  inline void SetRenderMetaData(bool bRenderMetaData)
  {
    m_bRenderMetaData = bRenderMetaData;
  }

  /// \brief
	///   Overridable that is called by vForge to activate this preview. Graphics resources can be allocated here.
	VBASE_IMPEXP virtual void OnActivate()
  {
  }

  /// \brief
	///   Overridable that is called by vForge to de-activate this preview. Graphics resources should be de-allocated here.
  VBASE_IMPEXP virtual void OnDeActivate()
  {
  }

  /// \brief
	///   Update function that is called every frame when this preview is active. Can be used to show a dynamic preview.
	///
	/// \param fTimeDiff
	///   The time difference in sec since the last update
	///
	/// \returns
	///   \c true to trigger the next view update right after that. Can be used to animate the preview.
	///
	VBASE_IMPEXP virtual bool OnUpdate(float fTimeDiff)
  {
    return false;
  }

  /// \brief
	///   This is the actual render function. It is called from inside a render loop so direct rendering can be performed here.
	///
	/// \param iFlags
	///   Render flags. Currently this is either RESOURCEPREVIEWFLAG_NONE or RESOURCEPREVIEWFLAG_FOR_THUMBNAIL when this is called for thumbnail rendering.
	///
	VBASE_IMPEXP virtual void OnRender(int iFlags)
  {
  }

  /// \brief
  ///   Return \c true if this type supports saving of thumbnails. vForge will then associate the SaveAsThumbnail 
  ///   with the keyboard shortcut which is CRTL-T by default.
  ///
  /// \returns
  ///   \c true to support thumbnails. In this case, saving of either custom or image thumbnails must be implemented.
  ///
  /// \see
  ///   IVResourcePreview::SupportsCustomThumbnails
  ///   IVResourcePreview::SupportsImageThumbnails
  ///   IVResourcePreview::SaveCustomThumbnail
  ///   IVResourcePreview::SaveImageThumbnail
  ///   IVResourcePreview::QueryImageThumbnailParameters
  VBASE_IMPEXP virtual bool SupportsThumbnails()
  {
    return false;
  }

  /// \brief
  ///   Returns whether this type supports saving custom thumbnails, i.e., thumbnails completely rendered and saved
  ///   by the resource preview's code. If \c true, SaveCustomThumbnail must be implemented.
  /// \returns
  ///   \c true if saving of custom thumbnails is supported; \c false otherwise
  VBASE_IMPEXP virtual bool SupportsCustomThumbnails()
  {
    return false;
  }

  /// \brief
  ///   Returns whether this type supports saving image thumbnails, i.e., thumbnails generated 
  ///   by rendering the resource in the Engine and saving the resulting image afterwards. If \c true,
  ///   SaveImageThumbnail must be implemented. A default implementation is provided in the class
  ///   VResourcePreview in the VisionEnginePlugin.
  /// \returns
  ///   \c true if saving of image thumbnails is supported; \c false otherwise
  VBASE_IMPEXP virtual bool SupportsImageThumbnails()
  {
    return false;
  }

  /// \brief
  ///   Shortcut function to determine whether there is any way to save the preview as a thumbnail, and
  ///   if there is, to use that way to actually save it.
  /// \return
  ///   \c true if there was a way to save this preview and saving succeeded, \c false if there either was
  ///   no way to save it or saving failed.
  VBASE_IMPEXP bool SaveThumbnail()
  {
    if (!SupportsThumbnails())
      return false;
    
    bool bSaved = false;
    if (SupportsCustomThumbnails())
      bSaved = SaveCustomThumbnail();
    if (!bSaved && SupportsImageThumbnails())
      bSaved = SaveImageThumbnail();

    return bSaved;
  }

  /// \brief
  ///   Implement this function to save a custom thumbnail. All rendering and saving must be performed
  ///   in this function.
  ///
  /// \returns
  ///   \c true if saving was successful, \c false otherwise
  ///
  /// \remarks
  ///   If the resource can be rendered in a suitable way using the Engine, you can use SaveImageThumbnail instead.
  VBASE_IMPEXP virtual bool SaveCustomThumbnail()
  {
    return false;
  }

  /// \brief
  ///   Implemented in VResourcePreview to save an image thumbnail. Rendering and saving is performed by the Engine.
  ///
  /// \param szFileName
  ///   the file name under which to save the image. Can be \c NULL, in which case the resource is asked to suggest a name.
  /// \param iResolutionX
  ///   desired X resolution of the thumbnail image. If <= 0, a default value or a value suggested by the resource is used.
  /// \param iResolutionY
  ///   desired Y resolution of the thumbnail image. If <= 0, a default value or a value suggested by the resource is used.
  /// \param backgroundColor
  ///   the background color for rendering the preview image; may be \c NULL to use default.
  /// \returns
  ///   \c true if saving was successful, \c false otherwise
  VBASE_IMPEXP virtual bool SaveImageThumbnail(const char* szFileName = NULL, int iResolutionX = 0, int iResolutionY = 0, VColorRef* backgroundColor = NULL)
  {
    return false;
  }

  /// \brief
  ///   Queries the resource preview for its suggested parameters for image thumbnails. Is only called if both
  ///   SupportsImageThumbnails returns true.
  ///
  /// \param sAbsDestFilenameOut
  ///   receives the absolute file name for writing the thumbnail image to. Can already include a file extension.
  /// \param iPreferredResXOut
  ///   receives the suggested X resolution for the thumbnail image.
  /// \param iPreferredResYOut
  ///   receives the suggested Y resolution for the thumbnail image.
  VBASE_IMPEXP virtual void QueryImageThumbnailParameters(VString &sAbsDestFilenameOut, int &iPreferredResXOut, int &iPreferredResYOut)
  {
  }

  /// \brief
  ///   Called by vForge to retrieve the absolute thumbnail filename.
  /// \returns
  ///   Absolute thumbnail filename
  VBASE_IMPEXP virtual VString GetThumbnailFilename()
  {
    return VString();
  }

  /// \brief
  ///   Called by vForge to determine if a thumbnail has already been saved for this resource preview.
  VBASE_IMPEXP virtual bool ThumbnailAlreadySaved()
  {
    return false;
  }

  /// \brief
  ///   Called by vForge to delete a thumbnail that has been saved for this resource preview.
  VBASE_IMPEXP virtual bool DeleteThumbnail()
  {
    return FALSE;
  }


protected:
  bool m_bRenderMetaData;
private:
  VManagedResourcePtr m_spOwner; ///< Owner as passed to the constructor
};


/// \brief
///   Collection for VManagedResource (reference counted)
class VResourceCollection : public VRefCountedCollection<VManagedResource>
{
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
