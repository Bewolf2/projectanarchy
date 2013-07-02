/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiResource.hpp

#ifndef _VISAPIRESOURCE_HPP_INCLUDED
#define _VISAPIRESOURCE_HPP_INCLUDED

class VConnection;

//
// Synopsis:
//  - System resource manager name string constants
//
#define VIS_RESOURCEMANAGER_BITMAPS               "Bitmaps"
#define VIS_RESOURCEMANAGER_MESHBUFFER            "MeshBuffer"
#define VIS_RESOURCEMANAGER_EFFECTLIBS            "EffectLibs"
#define VIS_RESOURCEMANAGER_TEXTURES              "Textures"
#define VIS_RESOURCEMANAGER_LIGHTMASKS            "LightMasks"
#define VIS_RESOURCEMANAGER_COLLISIONMESHES       "CollisionMeshes"
#define VIS_RESOURCEMANAGER_ANIMATIONS            "Animations"
#define VIS_RESOURCEMANAGER_SYSTEMMEMORYBUFFER    "SystemMemory"
#define VIS_RESOURCEMANAGER_INDEXBUFFER           "IndexBuffers"
#define VIS_RESOURCEMANAGER_VERTEXBUFFER          "VertexBuffers"
#define VIS_RESOURCEMANAGER_LIGHTGRIDS            "Lightgrids"
#define VIS_RESOURCEMANAGER_MATERIALTEMPLATES     "MaterialTemplates"
#define VIS_RESOURCEMANAGER_MESHES                "Static/Dynamic Meshes"
#define VIS_RESOURCEMANAGER_SURFACELIBRARIES      "SurfaceLibraries"


/// \brief
///   Vision resource manager base class.
/// 
/// An instance of this class represents a resource manager for a single resource type.
/// 
/// This class is derived from VResourceManager and implements the file IO overridables.
/// 
/// A resource manager owns a list of resources of type VManagedResource and provides functionality
/// to handle the resources, e.g. checking for memory limits etc.
/// 
/// The engine provides resource manager for each type of managed resource, e.g. models, textures,
/// skeletal  animations etc.
/// 
/// The engine resource manager can be accessed via the VisResourceSystem_cl class
/// (Vision::ResourceSystem). The VisResourceSystem_cl class also allows the user to register 
/// custom resource managers which will show up in the resource viewer as well.
/// 
/// To add resources to the manager, derive your own resource class from VManagedResource and pass
/// a pointer to the manager in the constructor.
class VisResourceManager_cl : public VResourceManager
{
public:

  ///
  /// @name Constructors / Destructor
  /// @{
  ///


  /// \brief
  ///   Constructor of the VisResourceManager_cl class
  /// 
  /// \param szManagerName
  ///   The name of the resource manager. If the manager is registered with the resource system, this 
  ///   name will show up in the resource viewer.
  /// 
  /// \param iFlags
  ///   Flags for the resource manager. See VResourceManager::VResourceManager() and VResourceFlag
  ///   for supported bit flag constants.
  /// 
  /// \param iMemLimit
  ///   The memory limit of this resource manager. Once the memory limit is exceeded the manager
  ///   tries to unload or remove unused resources. The memory limit is checked for each call of
  ///   the resource manager's TickFunction. (VResourceManager::TickFunction). If the manager is
  ///   registered via the VisResourceSystem_cl class, the TickFunction will be called
  ///   automatically.
  /// 
  /// \sa class VResourceManager
  /// \sa class VisResourceSystem_cl
  /// \sa VisResourceSystem_cl::RegisterResourceManager
  VISION_APIFUNC VisResourceManager_cl(const char *szManagerName, int iFlags=VRESOURCEMANAGERFLAG_NONE, int iMemLimit=0);

  /// \brief
  ///   Destructor of the VisResourceManager_cl class
  /// 
  /// When a resource manager instance is destroyed, remaining resources in the manager are
  /// reported as leaks.
  /// 
  /// Thus PurgeUnusedResources should be called before deleting it (this is however automatically
  /// done when the manager is registered). If there are still resources left, an assertion will be
  /// triggered.
  VISION_APIFUNC virtual ~VisResourceManager_cl();


  ///
  /// @}
  ///

  ///
  /// @name Information Functions
  /// @{
  ///

  /// \brief
  ///   Indicates whether this manager is registered in the resource system (see
  ///   VisResourceSystem_cl class)
  /// 
  /// Registered resource manager can show up in the resource viewer and are updated by the engine
  /// each tick.
  /// 
  /// \return
  ///   \c TRUE if the manager is registered
  /// 
  /// \sa VisResourceSystem_cl::RegisterResourceManager()
  /// \sa VisResourceSystem_cl::UnegisterResourceManager()
  VISION_APIFUNC BOOL IsRegistered() const;


  ///
  /// @}
  ///

  ///
  /// @name Overridden File Stream Functions
  /// @{
  ///

  /// \brief
  ///   Creates a file in stream for loading resources in this manager.
  /// 
  /// The default implementation of this function wraps around Vision::File.Open
  /// 
  /// \param szFilename
  ///   The filename of the file to open.
  /// 
  /// \param pStoreTimeStampRes
  ///   a pointer to a resource that will receive the time stamp of the opened file. Can be \c NULL.
  /// 
  /// \return
  ///   pointer to a stream for reading (or \c NULL if file could not be opened)
  /// 
  /// \note
  ///   The resources in this manager should use the
  ///   GetParentManager()->CreateFileInStream(GetFilename(), this) function rather than opening a
  ///   file directly with Vision::File.Open
  /// 
  /// \sa VStreamResource::Reload()
  VISION_APIFUNC virtual IVFileInStream *CreateFileInStream(const char *szFilename, VManagedResource *pStoreTimeStampRes) HKV_OVERRIDE;

  /// \brief
  ///   Creates a file out stream.
  /// 
  /// The default implementation of this function wraps around Vision::File.Create.
  /// 
  /// \param szFilename
  ///   the filename of the file to be created
  /// 
  /// \param pStoreTimeStampRes
  ///   a pointer to a resource that will receive the time stamp of the created file. Can be \c NULL.
  /// 
  /// \return
  ///   pointer to a stream for writing (or \c NULL if file could not be created)
  /// 
  /// \note
  ///   The resources in this manager should use the
  ///   GetParentManager()->CreateFileOutStream(GetFilename()) function rather than creating a file
  ///   directly with Vision::File.Create
  VISION_APIFUNC virtual IVFileOutStream *CreateFileOutStream(const char *szFilename, VManagedResource *pStoreTimeStampRes) HKV_OVERRIDE;

  /// \brief
  ///   Returns Vision's standard file manager.
  /// \sa Vision::File.GetManager()
  VISION_APIFUNC VOVERRIDE IVFileStreamManager* GetFileManager();

  ///
  /// @}
  ///

  ///
  /// @name Debug Output Functions
  /// @{
  ///

  /// \brief
  ///   Overridable function that outputs the manager information and resource list on the screen
  /// 
  /// The default implementation of this function writes the list of resources to the screen,
  /// calling VManagedResource::GetDebugOutputString for each resource.
  /// 
  /// This function should be used for debugging purposes only.
  VISION_APIFUNC virtual void DebugScreenOutput() const;


  /// \brief
  ///   Overridable function that dumps out resource leaks.
  /// 
  /// In the default implementation of this function, the resource leaks are dumped to the log
  /// output.
  /// 
  /// The DumpMemoryLeaks function is called for each registered resource manager at engine
  /// deinitialization time.
  /// 
  /// \param bDeleteObjects
  ///   pass \c TRUE to delete the leaked resources
  /// 
  /// \return
  ///   the number of remaining resources (resource leaks)
  VISION_APIFUNC virtual int DumpMemoryLeaks(BOOL bDeleteObjects=FALSE);

  ///
  /// @}
  ///

private:
  friend class VisResourceSystem_cl;
};


/// \brief
///   Helper template to provide the global ElementManager functionality using the parent resource
///   manager
/// 
/// A custom resource class can be derived from this template class. The list of resources of this
/// type can then be accessed via MyManagedResource_cl::ElementManagerGet(iIndex).
/// 
/// This allows the programmer to get a per-class resource manager.
/// 
/// This is just a helper class for compatibility reasons.
template<class ELEMTYPE> class VisRMElementManager_cl
{
public:
  inline virtual ~VisRMElementManager_cl(){;}

  /// \brief
  ///   Gets the number of elements (=number of resources) in the resource manager.
  /// 
  /// \return
  ///   the number of resources (i.e. maximum valid index +1).
  /// 
  /// \sa class VisElementManager_cl
  static int ElementManagerGetSize()               {VASSERT(g_pResourceManager); return g_pResourceManager->GetResourceCount();}

  /// \brief
  ///   Counts the number of used elements (=number of used resources) in the resource manager.
  /// 
  /// \return
  ///   the number of used resources.
  /// 
  /// \sa class VisElementManager_cl
  static int ElementManagerGetUsedCount()          {VASSERT(g_pResourceManager); return g_pResourceManager->GetUsedResourceCount();}

  /// \brief
  ///   Gets the resource at specified index.
  /// 
  /// The index must be a valid index [0..ElementManagerGetSize()-1].
  /// 
  /// \param iIndex
  ///   resource index in the list. Must be within the [0..ElementManagerGetSize()-1] range.
  /// 
  /// \return
  ///   resource pointer at index \c iIndex. Can be \c NULL.
  /// 
  /// \sa class VisElementManager_cl
  static ELEMTYPE ElementManagerGet(int iIndex)    {VASSERT(g_pResourceManager); return (ELEMTYPE)g_pResourceManager->GetResourceByIndex(iIndex);}

  /// \brief
  ///   Static function to return the global resource manager that is associated with this resource type
  static inline VisResourceManager_cl& GetResourceManager() {return *g_pResourceManager;}
protected:
  VISION_APIDATA static VisResourceManager_cl *g_pResourceManager; ///< Points to the individual class specific resource manager.
};



/// \brief
///   This class implements the IVResourceCreator interface. It uses the VisResourceSystem_cl class
///   to create resources
class VisionResourceCreator_cl : public IVResourceCreator
{
public:

  /// \brief
  ///   Looks up the specified resource in the global resource system.
  /// 
  /// \param snapshot
  ///   reference to resource snapshot in whose context this function is called
  /// 
  /// \param resourceDesc
  ///   resource entry structure that describes the resource to find
  /// 
  /// \return
  ///   pointer to the resource, if it exists; \c NULL otherwise.
  VISION_APIFUNC virtual VManagedResource *FindExistingResource(VResourceSnapshot &snapshot, VResourceSnapshotEntry &resourceDesc) HKV_OVERRIDE;

  /// \brief
  ///   Creates a resource in the appropriate resource manager.
  /// 
  /// \param snapshot
  ///   reference to resource snapshot in whose context this function is called
  /// 
  /// \param resourceDesc
  ///   resource entry structure that describes the resource to create
  /// 
  /// \return
  ///   pointer to the resource, if it could be created; \c NULL otherwise.
  VISION_APIFUNC virtual VManagedResource *CreateResource(VResourceSnapshot &snapshot, VResourceSnapshotEntry &resourceDesc) HKV_OVERRIDE;

  /// \brief
  ///   Patches platform specific filenames (needed for shader library bin files)
  VISION_APIFUNC const char *GetReplacementFilename(VResourceSnapshotEntry &resourceDesc, const char *szResolvedFilename, char *szBuffer);

  /// \brief
  ///   Looks up the manager by name or uses the cached VResourceSnapshotEntry::m_pManager member
  VISION_APIFUNC VResourceManager *GetResourceManager(VResourceSnapshotEntry &resourceDesc);
};

/// \brief
///   Vision resource manager system interface, accessed via Vision::ResourceSystem.
/// 
/// The resource system holds and updates the list of registered resource managers.
/// 
/// For each registered manager, the resource system calls the TickFunction each tick, and the
/// purge functions after LoadWorld resp. when deinitializing a world.
/// 
/// The resource system additionally updates the registered managers in the resource viewer.
class VisResourceSystem_cl
{
public:

  ///
  /// @name Access Registered Resource Managers
  /// @{
  ///

  /// \brief
  ///   Returns the number of registered resource managers.
  /// 
  /// \return
  ///   the number of resource manager entries in the list. Note that the list can contain
  ///   \c NULL entries.
  /// 
  /// \sa VisResourceSystem_cl::GetResourceManagerByIndex()
  /// \sa VisResourceSystem_cl::GetResourceManagerByName()
  /// \sa VisResourceSystem_cl::RegisterResourceManager()
  /// \sa VisResourceSystem_cl::UnregisterResourceManager()
  VISION_APIFUNC int GetResourceManagerCount() const;

  /// \brief
  ///   Gets a registered resource manager.
  /// 
  /// \param iIndex
  ///   index of the resource manager; must be in valid range [0..GetResourceManagerCount()-1]
  /// 
  /// \return
  ///   a pointer to the resource manager. Can be \c NULL.
  /// 
  /// \sa VisResourceSystem_cl::GetResourceManagerCount()
  /// \sa VisResourceSystem_cl::GetResourceManagerByName()
  /// \sa VisResourceSystem_cl::RegisterResourceManager()
  /// \sa VisResourceSystem_cl::UnregisterResourceManager()
  VISION_APIFUNC VResourceManager *GetResourceManagerByIndex(int iIndex) const;

  /// \brief
  ///   Finds a registered resource manager by its name.
  /// 
  /// For finding the system managers, the following string constants are defined:
  /// \li \c VIS_RESOURCEMANAGER_TEXTURES : Resource manager that handles the textures
  /// \li \c VIS_RESOURCEMANAGER_MESHES : Resource manager that handles the mesh files (dynamic meshes/static meshes)
  /// \li \c VIS_RESOURCEMANAGER_BITMAPS : Resource manager that handles VisBitmap_cl instances (not
  /// textures!)
  /// \li \c VIS_RESOURCEMANAGER_MESHBUFFER : Resource manager that handles mesh buffers
  /// \li \c VIS_RESOURCEMANAGER_EFFECTLIBS : Resource manager that handles loaded effect libraries
  /// (ShaderLib files)
  /// \li \c VIS_RESOURCEMANAGER_LIGHTMASKS : Resource manager that handles the lightmask cache
  /// \li \c VIS_RESOURCEMANAGER_COLLISIONMESHES : Resource manager that handles the
  /// collision mesh cache (both world primitives and entities)
  /// \li \c VIS_RESOURCEMANAGER_ANIMATIONS :
  /// Resource manager that handles the animation sequences (.anim files)
  /// \li \c VIS_RESOURCEMANAGER_SYSTEMMEMORYBUFFER : Resource manager that handles system memory buffer
  /// \li \c VIS_RESOURCEMANAGER_INDEXBUFFER : Resource manager that handles index buffers used for
  /// rendering
  /// \li \c VIS_RESOURCEMANAGER_VERTEXBUFFER : Resource manager that handles vertex buffers
  /// used for rendering
  /// \li \c VIS_RESOURCEMANAGER_LIGHTGRIDS: Resource manager that handles light grids
  /// \li \c VIS_RESOURCEMANAGER_MATERIALTEMPLATES: Resource manager that handles material templates
  /// \li \c VIS_RESOURCEMANAGER_SURFACELIBRARIES: Resource manager that handles material libraries
  /// 
  /// \param szName
  ///   unique name of the resource manager to find. Specify either a custom manager or a system
  ///   resource manager (see name constants for system managers in the description).
  /// 
  /// \return
  ///   a pointer to the resource manager with specified name, or \c NULL if no such resource
  ///   manager exists. For a custom registered manager the return value can be cast into a 
  ///   VisResourceManager_cl instance.
  /// 
  /// \sa VisResourceSystem_cl::GetResourceManagerCount()
  /// \sa VisResourceSystem_cl::GetResourceManagerByIndex()
  /// \sa VisResourceSystem_cl::RegisterResourceManager()
  /// \sa VisResourceSystem_cl::UnregisterResourceManager()
  VISION_APIFUNC VResourceManager *GetResourceManagerByName(const char *szName) const;

  ///
  /// @}
  ///

  ///
  /// @name Purging / Reloading
  /// @{
  ///

  /// \brief
  ///   Purges unused resources in all registered resource managers.
  /// 
  /// This function calls VResourceManager::PurgeUnusedResources on each registered resource manager.
  /// 
  /// Calling this function can take a significant amount of time.
  /// 
  /// Alternatively, each resource manager can purge automatically from time to time (defined via
  /// VResourceManager::SetAutoPurgeInterval).
  /// 
  /// \return
  ///   the number of purged resources.
  /// 
  /// \sa VisResourceSystem_cl::RegisterResourceManagers()
  /// \sa VisResourceSystem_cl::UnregisterResourceManagers()
  VISION_APIFUNC int PurgeAllResourceManagers();

  /// \brief
  ///   Deprecated version of PurgeAllResourceManagers.
  HKV_DEPRECATED_2013_2 inline int PurgeAllResourceManager()
  {
    return PurgeAllResourceManagers();
  }

  /// \brief
  ///   Purges all registered resource managers to which at least one of the specified flags 
  ///   applies.
  /// 
  /// The passed value is a bitmask that is binary ANDed with the resource manager's
  /// VResourceManager::GetPurgeMask value. The resource manager is eligible for a purge if
  /// the result is not zero.
  /// 
  /// This way a specific subset of resource managers can be purged.
  /// 
  /// There following predefined values (or a combination thereof) can be specified:
  /// \li VRESOURCEMANAGER_PURGEMASK_AFTERSCENELOAD : called after a new scene has been loaded
  /// 
  /// \li VRESOURCEMANAGER_PURGEMASK_AFTERSCENEUNLOAD : called when a scene has been unloaded
  /// 
  /// \li VRESOURCEMANAGER_PURGEMASK_STREAMINGCLEANUP : called by zones to cleanup resources
  /// 
  /// \li VRESOURCEMANAGER_PURGEMASK_FORCEALL : force all managers to be purged (e.g. at engine
  ///   de-init time)
  /// 
  /// \param iPurgeMask
  ///   bitmask to filter the managers (see description).
  /// 
  /// \return
  ///   the number of purged resources.
  VISION_APIFUNC int PurgeAllResourceManagers(unsigned int iPurgeMask);

  /// \brief
  ///   Deprecated version of PurgeAllResourceManagers.
  HKV_DEPRECATED_2013_2 inline int PurgeAllResourceManager(unsigned int iPurgeMask)
  {
    return PurgeAllResourceManagers(iPurgeMask);
  }

  /// \brief
  ///   Reloads all modified resources of a registered resource manager.
  /// 
  /// Modified resources are identified by either a changed file time stamp, or, if asset management
  /// is used, by a changed entry for this resource in the asset lookup table.
  /// 
  /// This feature is very useful to dynamically update changed textures etc.
  /// 
  /// \param szManagerName
  ///   The resource manager name to check. NULL to test all currently registered resource
  ///   managers.
  /// 
  /// \param bUnload
  ///   If TRUE, a resource will be unloaded in case it has been modified (calls EnsureUnloaded for
  ///   the resource).
  /// 
  /// \param bReload
  ///   If TRUE, a resource will be reloaded in case it has been modified (calls EnsureLoaded for
  ///   the resource).
  /// 
  /// \return
  ///   the number of modified resources found.
  /// 
  /// \note
  ///   The following combinations of the bUnload,bReload flags actually make sense:
  ///   \li FALSE,FALSE : Nothing happens to the resources, useful for counting the modified
  ///     resources.
  ///   \li TRUE,FALSE :  Modified resources are unloaded but not reloaded. Reloading will happen
  ///     the next time the resource is used.
  ///   \li TRUE,TRUE :  Modified resources are unloaded and reloaded. The resource is guaranteed
  ///     to be up-to-date afterwards.
  ///
  /// \note
  ///   Not all resources can be reloaded at runtime. Textures, static meshes and dynamic meshes
  ///   for example can be reloaded; animations cannot be reloaded dynamically.
  VISION_APIFUNC int ReloadModifiedResourceFiles(const char *szManagerName, BOOL bUnload=TRUE, BOOL bReload=TRUE);

  /// \brief
  ///   Reloads a specific modified resource of a registered resource manager.
  ///
  /// See VisResourceSystem_cl::ReloadModifiedResourceFiles() for more details about resource reloading.
  ///
  /// \param szResourceId
  ///   The ID of the resource to reload.
  ///
  /// \param szManagerName
  ///   The resource manager name to check. NULL to check all currently registered resource
  ///   managers for the given resource ID.
  /// 
  /// \param bUnload
  ///   If TRUE, a resource will be unloaded in case it has been modified (calls EnsureUnloaded for
  ///   the resource).
  /// 
  /// \param bReload
  ///   If TRUE, a resource will be reloaded in case it has been modified (calls EnsureLoaded for
  ///   the resource).
  /// 
  /// \return
  ///   \c TRUE if the specified resource was found and modified.
  VISION_APIFUNC BOOL ReloadModifiedResourceFile(const char *szResourceId, const char *szManagerName, BOOL bUnload=TRUE, BOOL bReload=TRUE);

  /// \brief
  ///   Reloads a specific resource of a registered resource manager. The resource is reloaded
  ///   regardless of whether the file has actually been modified or not.
  /// 
  /// See VisResourceSystem_cl::ReloadModifiedResourceFiles() for more details about resource reloading.
  ///
  /// \param szManagerName
  ///   The resource manager name to check. NULL to test all currently registered resource
  ///   managers.
  /// 
  /// \param szResourceId
  ///   The ID of the resource to reload.
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
  VISION_APIFUNC BOOL ReloadSpecificResourceFile(const char *szManagerName, const char *szResourceId,
    BOOL bUnload=TRUE, BOOL bReload=TRUE);

  /// \brief
  ///   Calls the per-frame tick function for all resource managers. This function is called by the default VisionApp_cl class.
  VISION_APIFUNC void HandleAllResourceManager(float dtime);

  ///
  /// @}
  ///

  ///
  /// @name Register and Unregister Resource Managers
  /// @{
  ///

  /// \brief
  ///   Registers a custom resource manager.
  /// 
  /// Any registered manager will be updated, purged and displayed automatically (if
  /// VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER flag is set).
  /// 
  /// \param pManager
  ///   Pointer to a custom manager instance.
  /// 
  /// \param iDisplayColor
  ///   Displaying color. The manager will appear in this color in the engine profiling chart and
  ///   in the resource viewer. The alpha portion is ignored.
  /// 
  /// \return
  ///   \c TRUE if the manager could be registered. The manager cannot be registered if
  ///   there is already a manger registered with the same name.
  /// 
  /// \sa VisResourceSystem_cl::GetResourceManagerByName()
  /// \sa VisResourceSystem_cl::UnregisterResourceManager()
  VISION_APIFUNC BOOL RegisterResourceManager(VisResourceManager_cl *pManager, VColorRef iDisplayColor);

  /// \brief
  ///   Unregisters a custom resource manager.
  /// 
  /// \param pManager
  ///   Pointer to the custom manager instance that should be de-registered.
  /// 
  /// \return
  ///   \c TRUE, if the manager was unregistered. Note, that system manager cannot
  ///   be unregistered.
  /// 
  /// \sa VisResourceSystem_cl::GetResourceManagerByName()
  /// \sa VisResourceSystem_cl::RegisterResourceManager()
  /// \sa VisResourceSystem_cl::UnregisterAllResourceManager()
  VISION_APIFUNC BOOL UnregisterResourceManager(VisResourceManager_cl *pManager);

  /// \brief
  ///   Unregisters all custom resource managers
  /// 
  ///  Does not unregister system resource managers.
  /// 
  /// \return
  ///   the number of unregistered resource managers
  /// 
  /// \sa VisResourceSystem_cl::GetResourceManagerByName()
  /// \sa VisResourceSystem_cl::RegisterResourceManager()
  /// \sa VisResourceSystem_cl::UnregisterResourceManager()
  VISION_APIFUNC int UnregisterAllResourceManagers();

  HKV_DEPRECATED_2013_2 inline int UnregisterAllResourceManager()
  {
    return UnregisterAllResourceManagers();
  }

  ///
  /// @}
  ///

  ///
  /// @name Profiling / Debugging
  /// @{
  ///

  /// \brief
  ///   Enables or disables resource manager chart display in the engine
  /// 
  /// Should be used for debug purposes only.
  /// 
  /// \param bEnable
  ///   If enabled, the profiling chart will be displayed.
  /// 
  /// \param pManager
  ///   Pointer to specific manager to display. If NULL, the overview of all registered managers
  ///   will be displayed.
  /// 
  /// \note
  ///   Displaying the charts is only available in profiling mode of the engine (VisionP resp.
  ///   VisionD library version).
  /// 
  /// \note
  ///   Updating the chart takes a significant amount of time.
  VISION_APIFUNC void ShowProfilingChart(BOOL bEnable, VResourceManager *pManager=NULL);


  /// \brief
  ///   Immediately updates the resource viewer.
  /// 
  /// Updating the resource viewer might be useful to track down reference counter problems.
  /// 
  /// The resource viewer can be manually updated more than once per tick.
  /// 
  /// \note
  ///   Updating the resource viewer is only available in profiling mode of the engine (VisionP
  ///   resp. VisionD library version).
  VISION_APIFUNC void UpdateResourceViewer();

  /// \brief
  ///   Sets the status for reporting surviving resources in the report dialog
  /// 
  /// Usually all resources are purged when unloading a world, so it might be useful to list
  /// unpurged resources in the report dialog.
  /// 
  /// On the other hand, when knowingly keeping a reference to some of the resources, this dialog
  /// might not be useful at all.
  /// 
  /// This global flag defines the default behavior. It can be overridden per resource manager
  /// (VRESOURCEMANAGERFLAG_IGNORE_SURVIVING_RESOURCES flag) or even per resource instance
  /// (VRESOURCEFLAG_IGNORE_SURVIVING flag).
  /// 
  /// \param bStatus
  ///   If enabled, the surviving resources will be listed in the report dialog after resp. before
  ///   LoadWorld.
  VISION_APIFUNC void SetReportSurvivingResources(BOOL bStatus);
  ///
  /// @}
  ///
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
