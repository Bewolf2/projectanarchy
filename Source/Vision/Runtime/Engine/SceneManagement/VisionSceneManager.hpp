/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionSceneManager.hpp

#ifndef DEFINE_VISIONSCENEMANAGER
#define DEFINE_VISIONSCENEMANAGER

#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityZone.hpp>
#include <Vision/Runtime/Engine/SceneManagement/VisApiZone.hpp>

class VisVisibilityZone_cl;

/// \brief
///   Refcounted collection of VisVisibilityZone_cl pointers. Same as
///   VRefCountedCollection<VisVisibilityZone_cl>
class VisRCVisibilityZoneCollection_cl : public VRefCountedCollection<VisVisibilityZone_cl>
{
};


/// \brief
///   Interface providing the Vision engine with data about the scene. 
/// 
/// The scene manager knowns about all visibility zones in the scene and provides functionality for
/// managing and organizing the visibility nodes.
/// 
/// Registering a scene manager (either the default implementation or a custom solution) is
/// required to make the Vision engine work correctly.
/// 
/// By default, the VisionSceneManager_cl class is used.
class IVisSceneManager_cl : public VRefCounter
{
public:


  /// \brief
  ///   Constructor of the IVisSceneManager_cl interface.
  VISION_APIFUNC IVisSceneManager_cl();


  /// \brief
  ///   Virtual Destructor of the IVisSceneManager_cl interface.
  VISION_APIFUNC virtual ~IVisSceneManager_cl();


  ///
  /// @name Visibility Zone Management
  /// @{
  ///


  /// \brief
  ///   Returns a list of all visibility zones overlapping a bounding box.
  /// 
  /// Has to be implemented in derived classes. See VisionSceneManager_cl for a reference
  /// implementation.
  /// 
  /// \param bbox
  ///   Bounding box to test against.
  /// 
  /// \param ppZones
  ///   List of pointers to visibility zones (output).
  /// 
  /// \param iMaxZones
  ///   Maximum number of visibility zones to return. Typically the allocated size of the array
  ///   passed in ppZones.
  /// 
  /// \return
  ///   the number of visibility zones overlapping the bounding box, i.e. the number of
  ///   entries in the ppZones list.
  VISION_APIFUNC virtual unsigned int FindVisibilityZones(const hkvAlignedBBox &bbox, VisVisibilityZone_cl **ppZones, unsigned int iMaxZones) = 0;


  /// \brief
  ///   Returns the closest visibility zone for a bounding box.
  /// 
  /// This method is required by the Vision engine in order to classify scene elements according to
  /// the visibility zone they belong to.
  /// 
  /// In cases where the passed bounding box overlaps/spans multiple visibility zones, this
  /// function has to return the visibility zone the center of the bounding box is in.
  /// 
  /// Has to be implemented in derived classes. See VisionSceneManager_cl for a reference
  /// implementation.
  /// 
  /// \param bbox
  ///   Bounding box to test against.
  /// 
  /// \param pOrigin
  ///   If not null, this parameter is used to specify a custom origin (in world space) for a scene
  ///   element. The function should then use this position to identify the best-fitting visibility
  ///   zone rather than the center of the bounding box.
  /// 
  /// \return
  ///   the closest visibility zone for the passed bounding box and position.
  VISION_APIFUNC virtual VisVisibilityZone_cl *FindClosestVisibilityZone(const hkvAlignedBBox &bbox, const hkvVec3* pOrigin = NULL) = 0;
  

  /// \brief
  ///   Adds a visibility zone to the scene manager. The scene manager performs reference counting,
  ///   so the refcount of the added zone is increased.
  VISION_APIFUNC void AddVisibilityZone(VisVisibilityZone_cl *pZone);


  /// \brief
  ///   Removes a visibility zone from the scene manager. The scene manager performs reference
  ///   counting, so the refcount of the removed zone is decreased.
  VISION_APIFUNC void RemoveVisibilityZone(VisVisibilityZone_cl *pZone);


  /// \brief
  ///   Removes all visibility zones from the scene manager.
  VISION_APIFUNC void RemoveAllVisibilityZones();


  /// \brief
  ///   Returns the number of visibility zones registered with the scene manager.
  inline int GetNumVisibilityZones() const { return m_VisibilityZones.Count(); }


  /// \brief
  ///   Returns a pointer to the visibility zone with the specified index.
  inline VisVisibilityZone_cl *GetVisibilityZone(int iZoneIndex) const
  { 
    return m_VisibilityZones.GetAt(iZoneIndex); 
  }


  /// \brief
  ///   Checks whether this scene manager contains the passed visibility zone.
  inline bool ContainsVisibilityZone(VisVisibilityZone_cl *pZone)
  {
    return m_VisibilityZones.Contains(pZone);
  }


  /// \brief
  ///   Returns the collection of visibility zones. Should not be used to add zones.
  VisRCVisibilityZoneCollection_cl &VisibilityZones() {return m_VisibilityZones;}


  /// \brief
  ///   Returns the visibility zone with the specified unique ID.
  VISION_APIFUNC VisVisibilityZone_cl *VisibilityZoneForUID(__int64 uid) const;


  /// \brief
  ///   Returns the extents of the currently loaded scene.
  /// 
  /// \param bbox
  ///   Bounding box enclosing scene geometry.
  VISION_APIFUNC virtual void GetSceneExtents(hkvAlignedBBox& bbox) const = 0;

  /// \brief
  ///   Traces a ray into the scene and returns the first visibility zone it hits.
  /// 
  /// Needs to be implemented in derived classes so that visibility determination can handle cases
  /// correctly where the camera is outside of the scene and looks into it.
  /// 
  /// \param vPos
  ///   Starting point of the ray.
  /// 
  /// \param vDir
  ///   Direction and length of the ray.
  /// 
  /// \return
  ///   a pointer to the visibility zone, or NULL if none is hit.
  VISION_APIFUNC virtual VisVisibilityZone_cl *TraceIntoZone(const hkvVec3& vPos, const hkvVec3& vDir) = 0;


  ///
  /// @}
  ///


  ///
  /// @name Zone Management / Streaming
  /// @{
  ///


  /// \brief
  ///   Overridable factory function that can be used to create zone classes of custom
  ///   types (derived from base class VisZoneResource_cl).
  VISION_APIFUNC virtual VisZoneResource_cl *CreateZoneResource(VisZoneResourceManager_cl *pManager) = 0;


  /// \brief
  ///   Overridable that is called every frame by the application's Run function to handle
  ///   all zones.
  /// 
  /// This function is the core of streaming handling as it is responsible for loading/unloading
  /// the zones.
  /// 
  /// See default implementation in the VisionSceneManager_cl class.
  ///
  /// \param fTimeDelta
  ///   Time delta passed since the last tick.
  VISION_APIFUNC virtual void HandleZones(float fTimeDelta) = 0;

  /// \brief
  ///   Overridable to add a resource to a queue for full resolution loading. The queue is processed in HandleZones
  VISION_APIFUNC virtual void ScheduleForFullResolutionLoading(VManagedResource *pResource)
  {
  }

  /// \brief
  ///   Return the collection that holds all resources that are queued for full resolution loading. The queue is processed in HandleZones
  VISION_APIFUNC virtual VResourceCollection* GetResourceLoadingQueue()
  {
    return NULL;
  }

  /// \brief
  ///   Overridable that is called to set the reference object.
  /// 
  /// The position of the reference object is used for distance calculation towards the zones and
  /// thus controls the streaming of zones.
  /// 
  /// By default, the camera of the main render context is used as a reference object. 
  ///
  /// \param pRefObject
  ///   Reference object to set.
  VISION_APIFUNC virtual void SetStreamingReference(VisObject3D_cl *pRefObject);


  /// \brief
  ///   Returns the current reference object. NULL in case the camera of the main render context should be used.
  inline VisObject3D_cl* GetStreamingReference() const {return m_pStreamingReferenceObject;}

  /// \brief
  ///   Same as GetStreamingReference, but the return value is always !=NULL (returns the main camera if no dedicated object is set via SetStreamingReference)
  VISION_APIFUNC VisObject3D_cl* GetStreamingReferenceSafe() const;

  /// \brief
  ///   Adds a snapshot instance to the streaming queue. This function can be used to schedule custom snapshot classes.
  ///
  /// The default scene manager processes this queue in HandleZones. The order in the queue is determined by VResourceSnapshot::SetPriority.
  /// All snapshots used for zone streaming are handled through this queue as well. Their priority is determined by VisZoneResource_cl::GetStreamingPriority.
  /// The time spent for processing the queue is incorporated into the time management (e.g. VisionSceneManager_cl::SetStreamingTimeBudget).
  ///
  /// \param pSnapshot
  ///   The snapshot instance to add
  VISION_APIFUNC virtual void ScheduleSnapshot(VResourceSnapshot* pSnapshot)
  {
    m_SnapshotQueue.AddSnapshot(pSnapshot);
  }

  /// \brief
  ///   Remove a snapshot instance from the queue. If a snapshot is finished (VResourceSnapshot::IsFinished) it will be automatically removed from the queue
  VISION_APIFUNC virtual void RemoveSnapshot(VResourceSnapshot* pSnapshot)
  {
    m_SnapshotQueue.RemoveSnapshot(pSnapshot);
  }

  /// \brief
  ///   Access the underlying snapshot queue that is filled with instances from ScheduleSnapshot
  inline VResourceSnapshotQueue& GetSnapshotQueue()
  {
    return m_SnapshotQueue;
  }

  ///
  /// @}
  ///

  ///
  /// @name Zone Re-positioning
  /// @{
  ///

  /// \brief
  ///   Retrieve the information about the current re-positioning state, e.g. global pivot offset
  inline const VisZoneRepositionInfo_t& GetZoneRepositionInfo() const
  {
    return m_RepositionInfo;
  }

  /// \brief
  ///   Sets a radius in units around the origin inside which the camera can move without re-positioning.
  ///
  /// Once the camera leaves the radius, re-positioning will be triggered on all scene elements that are attached to zones.
  /// A value of 0.0f disables re-positioning (default). This value must be set before a new scene is loaded.
  ///
  /// \param fRadius
  ///   The tile radius (in world units) around the origin inside which the camera can move without re-positioning the scene. This can be a rather large value, e.g. >1km
  inline void SetRepositionInterval(float fRadius)
  {
    m_RepositionInfo.SetRepositionInterval(fRadius);
  }

  /// \brief
  ///   Internal function
  VISION_APIFUNC void SetGlobalPivot(const hkvVec3d& zonePivot);

  /// \brief
  ///   Internal function
  inline VisZoneRepositionInfo_t& GetZoneRepositionInfo_Modify()
  {
    return m_RepositionInfo;
  }

  /// \brief
  ///   This callback is triggered when re-positioning of scene elements occurs. The data object can be casted to VisZoneRepositionDataObject_cl
  ///
  /// Custom scene elements that are not attached to zones should listen to this callback to support re-positioning properly.
  VISION_APIDATA static VisCallback_cl OnReposition;

  ///
  /// @}
  ///


  ///
  /// @name Geometry Query Helpers
  /// @{
  ///


  /// \brief
  ///   Iterates through the scene and adds all static geometry instances to the passed collection that touch the bounding box.
  ///
  /// This function utilizes visibility information for faster processing. 
  ///
  /// \param bbox
  ///   Bounding box against which to test the geometry.
  /// 
  /// \param destList
  ///   This list will be filled with static geometry instances which passed the test.
  ///
  VISION_APIFUNC void GatherStaticGeometryInBoundingBox(const hkvAlignedBBox &bbox, VisStaticGeometryInstanceCollection_cl &destList);

  /// \brief
  ///   Iterates through the scene and adds all entity instances to the passed collection that touch the bounding box.
  ///
  /// This function utilizes visibility information for faster processing. Accordingly it only adds entities that have a model mesh and that
  /// are part of the visibility zones.
  ///
  /// \param bbox
  ///   Bounding box against which to test the entities
  /// 
  /// \param destList
  ///   This list will be filled with entities which passed the test.
  ///
  VISION_APIFUNC void GatherEntitiesInBoundingBox(const hkvAlignedBBox &bbox, VisEntityCollection_cl &destList);

  //Force zone update for handling slew conditions
  VISION_APIFUNC void RepositionAllZones();

protected:

  VisRCVisibilityZoneCollection_cl m_VisibilityZones;      ///< List of visibility zones.
  VisObject3D_cl *m_pStreamingReferenceObject;             ///< Reference object for streaming.

  mutable VisVisibilityZone_cl *m_pLastTestedZone;         ///< Cache for faster access in VisibilityZoneForUID.
  VResourceSnapshotQueue m_SnapshotQueue;
  
  VisZoneRepositionInfo_t m_RepositionInfo;
  
  ///
  /// @}
  ///

};

/// \brief
///   The default scene manager for the Vision engine.
/// 
/// The default scene manager for the Vision engine, handling most common scene types.
/// 
/// Note that the FindClosestVisibilityZone method relies on static geometry instances for
/// classifying scene elements. In situations where the bounding boxes of visibility zones overlap
/// and there is no static geometry present which can be used to classify the visibility zone
/// assignment of scene elements, this implementation may fail. Such scenes have to use custom
/// scene manager implementations.
class VisionSceneManager_cl : public IVisSceneManager_cl
{
public:


  ///
  /// @name Construction / Destruction
  /// @{
  ///


  /// \brief
  ///   Vision scene manager Constructor.
  VISION_APIFUNC VisionSceneManager_cl();


  /// \brief
  ///   Vision scene manager Destructor.
  VISION_APIFUNC ~VisionSceneManager_cl();


  ///
  /// @}
  ///


  ///
  /// @name Visibility Zone Management
  /// @{
  ///


  /// \brief
  ///   Returns a list of all visibility zones overlapping a bounding box.
  /// 
  /// \param bbox
  ///   Bounding box to test.
  /// 
  /// \param ppZones
  ///   List of pointers to visibility zones (output).
  /// 
  /// \param iMaxZones
  ///   Maximum number of visibility zones to return. Typically the allocated size of the array
  ///   passed in ppZones.
  /// 
  /// \return
  ///   the umber of visibility zones overlapping the bounding box, i.e. the number of
  ///   entries in the ppZones list.
  VISION_APIFUNC VOVERRIDE unsigned int FindVisibilityZones(const hkvAlignedBBox &bbox, VisVisibilityZone_cl **ppZones, unsigned int iMaxZones);


  /// \brief
  ///   Returns the closest visibility zone for a bounding box.
  /// 
  /// Returns the closest visibility zone for a bounding box, taking proximity to the static
  /// geometry in visibility nodes and portal relations into account.
  /// 
  /// This method is used by the Vision engine in order to classify scene elements according to the
  /// visibility zone they belong to.
  /// 
  /// In cases where the passed bounding box overlaps/spans multiple visibility zones, this
  /// function returns the visibility zone the center of the bounding box (or, if set, the origin
  /// passed to this function) is in.
  /// 
  /// Note that the this method relies on static geometry instances in the visibility zones for
  /// classifying scene elements. In situations where the bounding boxes of visibility zones
  /// overlap and there is no static geometry present which can be used to classify the visibility
  /// zone assignment of scene elements, this implementation will fail. Such scenes have to use
  /// custom scene manager implementations.
  /// 
  /// \param bbox
  ///   Bounding box to test against.
  /// 
  /// \param pOrigin
  ///   If not null, this parameter is used to specify a custom origin (in world space) for a scene
  ///   element. The function should then use this position to identify the best-fitting visibility
  ///   zone rather than the center of the bounding box.
  /// 
  /// \return
  ///   the closest visibility zone for the passed bounding box and position.
  VISION_APIFUNC VOVERRIDE VisVisibilityZone_cl *FindClosestVisibilityZone(const hkvAlignedBBox &bbox, const hkvVec3* pOrigin = NULL);


  /// \brief
  ///   Returns the closest visibility zone for a bounding box, based only on proximity.
  /// 
  /// No tests against geometry or portals are performed.
  /// 
  /// This is used as a special-cased solution called by FindClosestVisibilityZone in situations
  /// where no visibility zone actually overlaps the passed bounding box.
  /// 
  /// \param bbox
  ///   Bounding box to test against.
  /// 
  /// \return
  ///   the closest visibility zone for the passed bounding box and position.
  VISION_APIFUNC VisVisibilityZone_cl *FindClosestVisibilityZoneSimple(const hkvAlignedBBox &bbox) const;


  /// \brief
  ///   Returns the extents of the currently loaded scene.
  /// 
  /// \param bbox
  ///   Bounding box enclosing all scene geometry.
  VISION_APIFUNC VOVERRIDE void GetSceneExtents(hkvAlignedBBox& bbox) const;

  /// \brief
  ///   Traces a ray into the scene and returns the first visibility zone that was hit.
  /// 
  /// This function is useful if a camera is outside of the scene (e.g. in an editor) and a starting point for
  /// visibility determination is required.
  /// 
  /// \param vPos
  ///   Starting position of the ray.
  ///
  /// \param vDir
  ///   Direction and length of the ray.
  ///
  /// \returns
  ///   the first visibility zone that was hit by the ray, or NULL if none was hit.
  ///
  VISION_APIFUNC VOVERRIDE VisVisibilityZone_cl *TraceIntoZone(const hkvVec3& vPos, const hkvVec3& vDir);

  ///
  /// @}
  ///


  ///
  /// @name Zone Management / Streaming
  /// @{
  ///


  /// \brief
  ///   Overridden implementation that returns instances of base class VisZoneResource_cl
  VISION_APIFUNC VOVERRIDE VisZoneResource_cl *CreateZoneResource(VisZoneResourceManager_cl *pManager);


  /// \brief
  ///   Base implementation that controls zone streaming every frame.
  /// 
  /// This implementation uses a simple radius based approach to load, unload resp. triggering the
  /// pre-caching of a zone. The respective distances can be set inside vForge by artists.
  /// See implementation in he cpp file.
  ///
  /// \param fTimeDelta
  ///   Time delta passed since the last tick.
  VISION_APIFUNC virtual void HandleZones(float fTimeDelta) HKV_OVERRIDE;

  /// \brief
  ///   Defines how much time (in milliseconds) is reserved for zone streaming per game tick.
  ///
  /// The HandleZones implementation of this class processes all streaming zones (e.g. object creation loop) as long
  /// as the elapsed time in that function remains smaller than the budget. It is however not guaranteed that zone handling can
  /// always stick to this limit (e.g. uploading big textures etc.)
  ///
  /// \param fMilliSeconds
  ///   Time budget in milliseconds
  inline void SetStreamingTimeBudget(float fMilliSeconds)
  {
    m_fStreamingTimeBudget = fMilliSeconds;
  }

  /// \brief
  ///   Sets the behavior for purging unused resources (delete resources vs. unload only) 
  ///
  /// Unused resources are usually deleted, but with this method the behavior can be switched to unload resources. In that mode
  /// Resources are just unloaded so that the resource object itself remains in the resource manager. This mode might have some advantages
  /// in specific implementations.
  ///
  /// \param bUnload
  ///   if false (default), unused resources are deleted (purged). Pass true to just unload the resources
  inline void SetStreamingPurgeMethod(bool bUnload=false)
  {
    m_bPreferUnload = bUnload;
  }



  /// \brief
  ///   Helper function that purges/unloads unused resources while sticking to the passed time limit
  VISION_APIFUNC bool PurgeResources(__int64 iEndTime);

  VISION_APIFUNC virtual void ScheduleForFullResolutionLoading(VManagedResource *pResource) HKV_OVERRIDE
  {
    m_FullResQueue.AddUnique(pResource);
  }

  VISION_APIFUNC virtual VResourceCollection* GetResourceLoadingQueue() HKV_OVERRIDE
  {
    return &m_FullResQueue;
  }

  /// \brief
  ///   Internal function
  VISION_APIFUNC void HandleFullResLoadingQueue();

protected:
  bool m_bAnyZoneUnloaded, m_bAnyZoneCaching;
  bool m_bPreferUnload;
  // deferred loading of full-res resources
  VResourceCollection m_FullResQueue;
  VSmartPtr<VLoadingTask> m_spLoadingTask;
  DynArray_cl<VisZoneResource_cl *>m_ChangingZones;
  float m_fStreamingTimeBudget;
  int m_iLastManagerIndex;

  ///
  /// @}
  ///

};

typedef VSmartPtr<IVisSceneManager_cl> IVisSceneManagerPtr;

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
