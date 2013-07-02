/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiZone.hpp

#ifndef VISAPIZONE_HPP_INCLUDED
#define VISAPIZONE_HPP_INCLUDED

#include <Vision/Runtime/Engine/System/VisApiTypedEngineObject.hpp>

class VisZoneResource_cl;
class VZoneShapesArchive;



class VisZoneResourceManager_cl;

// initial zone file version (was shared with VSCENE version number)
#define VZONE_FILE_VERSION_9      9

// SDK version 7.6: Optional zone pivot in vzone files
#define VZONE_FILE_VERSION_10     10

// always latest version:
#define VZONE_CURRENT_FILE_VERSION VZONE_FILE_VERSION_10

/// \brief
///   Data structure that holds information about re-positioning. An instance of this structure 
///   can be obtained from the scene manager via IVisSceneManager_cl::GetZoneRepositionInfo
struct VisZoneRepositionInfo_t
{
  VisZoneRepositionInfo_t();

  /// \brief
  ///   Internal function
  VISION_APIFUNC void SetRepositionInterval(float fRadius);

  /// \brief
  ///   Internal function
  VISION_APIFUNC void SetGlobalPivotPos(const hkvVec3d& vZonePivot );

  /// \brief
  ///   Internal function
  VISION_APIFUNC void GetGlobalPivotPos(hkvVec3d& vGlobalPivot) const;

  /// \brief
  ///   Indicates whether this scene wants to use the repositioning mechanism.
  inline bool SupportsRepositioning() const
  {
    return m_fLocalRange>0.f;
  }

  /// \brief
  ///   Internal function called by the scene manager
  VISION_APIFUNC bool HandleRepositioning(VisObject3D_cl *pCamera);

  /// \brief
  ///   Internal function
  inline void SetActive( bool bActive )
  {
    m_bActive = bActive;
  }

  /// \brief
  ///   Internal function
  inline void SetUpdateCamera( bool bUpdate )
  {
    m_bUpdateCamera = bUpdate;
  }

  /// \brief 
  ///   Increased support for pivoting along the Z axis, but it is not fully tested.
  inline void SetCanPivotOnZ( bool bCanPivot )
  {
    m_bCanPivotOnZ = bCanPivot;
  }

  /// \brief
  ///   Internal function
  inline bool GetActive() const
  {
    return m_bActive;
  }

  /// \brief
  ///   Internal function
  inline bool GetUpdateCamera() const
  {
    return m_bUpdateCamera;
  }

  /// \brief
  ///   Increased support for pivoting along the Z axis, but it is not fully tested.
  inline bool GetCanPivotOnZ() const
  {
    return m_bCanPivotOnZ;
  }

  /// \brief
  ///   Helper function to generate render space coordinates out of local position and zone pivot coordinates
  inline void Helper_MakeAbsolute(hkvVec3& dest, const hkvVec3& src, const hkvVec3d& vZonePivot ) const
  {
    dest = hkvVec3((vZonePivot + src) - m_vGlobalPivotPos);
  }

  /// \brief
  ///   Helper function to generate render space coordinates out of local position and a zone. The zone pointer can be NULL
  VISION_APIFUNC void Helper_MakeAbsolute(hkvVec3& dest, const hkvVec3& src, VisZoneResource_cl *pZone) const;

  float m_fLocalRange, m_fInvLocalRange;

  int m_iGlobalSectorX;
  int m_iGlobalSectorY;
  int m_iGlobalSectorZ;

  hkvVec3d m_vGlobalPivotPos;
  hkvVec3d m_vLastGlobalPivot;

  bool m_bActive;
  bool m_bUpdateCamera;
  bool m_bCanPivotOnZ;
};

/// \brief
///   This data object is passed to the static IVisSceneManager_cl::OnReposition callback to provide the re-positioning info
class VisZoneRepositionDataObject_cl : public IVisCallbackDataObject_cl
{
public:
  VisZoneRepositionDataObject_cl(VCallback *pSender, const VisZoneRepositionInfo_t &info) :
      IVisCallbackDataObject_cl(pSender), m_Info(info)
  {
  }

  const VisZoneRepositionInfo_t &m_Info; ///< The info member that holds information of the new pivot
};


/// \brief
///   Base resource class for zones in the application.
/// 
/// This zone class is the native counterpart for zones inside vForge.
/// 
/// It is implemented as a resource so it can load and unload itself.
/// 
/// When a zone is loaded, it opens the zone specific archive file (.vzone), de-serializes the
/// objects in the file and attaches all objects to this zone. 
/// 
/// Objects that are assigned to zones are automatically cleaned up when the zone is unloaded (See
/// VisTypedEngineObject_cl::DisposeObject function).
/// 
/// Besides objects such as entities, lights etc. a zone can have its own lightgrid which is used
/// automatically for all objects inside the zone. See
/// VisTypedEngineObject_cl::GetRelevantLightGrid 
class VisZoneResource_cl : public VManagedResource, public VUserDataObj
{
public:
  /// \brief Possible values for the loading/unloading state of a zone.
  enum VZoneState_e
  {
    ZONESTATE_UNLOADED, ///< The zone is not loaded
    ZONESTATE_STREAMINGRESOURCES, ///< The zone is streaming resources in preparation for creating instances
    ZONESTATE_CREATINGINSTANCES, ///< The zone is creating instances of the objects within
    ZONESTATE_LOADED, ///< The zone is fully loaded
    ZONESTATE_DESTROYINGINSTANCES ///< The zone is destroying instances while being unloaded
  };

  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// \brief
  ///   Constructor that attaches this zone to its parent manager
  VISION_APIFUNC VisZoneResource_cl(VisZoneResourceManager_cl *pManager, IVisSceneManager_cl *pSceneManager);

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisZoneResource_cl();

  VISION_APIFUNC virtual void OnRemoveResource() HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name Zone Properties
  /// @{
  ///

  /// \brief
  ///   Copies the zone name into szDestBuffer.
  inline const char* GetZoneName(char *szDestBuffer) const
  {
    VFileHelper::GetFilenameNoExt(szDestBuffer,VFileHelper::GetFilename(GetFilename()));
    return szDestBuffer;
  }

  ///
  /// @}
  ///

  ///
  /// @name Resource Functions/overrides
  /// @{
  ///


  /// \brief
  ///   Removes this zone's objects from the scene by calling DisposeObject on each.
  VISION_APIFUNC virtual BOOL Unload() HKV_OVERRIDE;

  /// \brief
  ///   De-serializes all objects from the corresponding .vzone file.
  VISION_APIFUNC virtual BOOL Reload() HKV_OVERRIDE;

#ifdef SUPPORTS_SNAPSHOT_CREATION
  VISION_APIFUNC virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
  VISION_APIFUNC virtual BOOL IsRelevantForSnapshot(VResourceSnapshot &snapshot) const  HKV_OVERRIDE
  {
    return FALSE; // would cause recursion otherwise
  }

#endif

  /// \brief
  ///   Writes the lightgrid filename into the resource viewer.
  VISION_APIFUNC virtual int GetAdditionalOutputString(char *szDestBuffer, int iMaxChars) HKV_OVERRIDE;

  /// \brief
  ///   Starts loading all required resources in the background. The per-frame update of the
  ///   snapshot is then handled by VisionSceneManager_cl::HandleZones
  VISION_APIFUNC virtual void ScheduleLoading();

  /// \brief
  ///   Loads (and optionally starts) the vres file that references resources of this zone.
  VISION_APIFUNC bool LoadSnapshotFile(bool bStart);

  /// \brief
  ///   Virtual function that is called by the zone handling code to perform the next loading tick if the zone is in streaming state
  ///
  /// The default implementation calls the tick function on m_Snapshot in the ZONESTATE_STREAMINGRESOURCES state and creates the next
  /// object when the zone is in ZONESTATE_CREATINGINSTANCES state.
  ///
  /// \param fTimeDelta
  ///   Time delta passed since the last simulation tick
  /// \returns
  ///    TRUE if streaming has finished, FALSE if this function must be called again in the next tick
  VISION_APIFUNC virtual BOOL LoadingTick(float fTimeDelta);

  /// \brief
  ///   Virtual function that is called by the zone handling code to perform the next unloading tick if the zone is in streaming state
  ///
  /// \param fTimeDelta
  ///   Time delta passed since the last simulation tick
  /// \returns
  ///    TRUE if streaming has finished, FALSE if this function must be called again in the next tick
  VISION_APIFUNC virtual BOOL UnLoadingTick(float fTimeDelta);

  /// \brief
  ///   Virtual overridable to retrieve the priority value for steaming in the resources. By default this function returns the 
  ///   negative camera distance so that close zones are handled first.
  VISION_APIFUNC virtual float GetStreamingPriority(VisObject3D_cl *pRefObject);


  /// \brief
  ///   Overridable convenience function to perform the actual instantiation after streaming.
  ///
  /// The only function that should be called inside this function is ar.ReadZoneObjects on the passed archive.
  /// The current implementation is return ar.ReadZoneObjects(1)==0; (one object 1 read from archive per tick).
  /// This tick function might be called multiple times per simulation tick, depending on how much time may be spent on streaming.
  /// Accordingly reading only one object (default) is a good solution for smoothest streaming granularity.
  ///
  /// \param ar
  ///   The archive used to instantiate objects.
  /// \param fTimeDelta
  ///   Time delta passed since the last simulation tick
  /// \returns
  ///    TRUE if instantiation has finished, FALSE if this function must be called again in the net tick. This corresponds to
  ///    the condition that ar.ReadZoneObjects returns a smaller object count than passed to it.
  ///
  VISION_APIFUNC virtual BOOL CreateInstanceTick(VZoneShapesArchive &ar, float fTimeDelta);

  /// \brief
  ///   Indicates whether this zone is currently in streaming state, If not, it is either loaded or unloaded.
  inline BOOL IsStreaming() const
  {
    return m_ZoneState==ZONESTATE_STREAMINGRESOURCES || m_ZoneState==ZONESTATE_CREATINGINSTANCES;
  }

  /// \brief
  ///   Indicates whether this zone is currently unloading.
  inline BOOL IsDestroyingInstances() const
  {
    return m_ZoneState==ZONESTATE_DESTROYINGINSTANCES;
  }

  /// \brief
  ///   Returns the current zone state
  inline VZoneState_e GetStreamingState() const
  {
    return m_ZoneState;
  }

  /// \brief
  ///   Deprecated, has no effect anymore
  static HKV_DEPRECATED_2012_1 VISION_APIFUNC void SetObjectCreationSpeed(float fObjectsPerSecond);

  ///
  /// @}
  ///

  ///
  /// @name Add/remove Objects
  /// @{
  ///

  /// \brief
  ///   Overridable that is called for every object that changes its zone during runtime via
  ///   VisTypedEngineObject_cl::SetParentZone
  VISION_APIFUNC virtual void OnAddEngineObject(VisTypedEngineObject_cl *pObject);

  /// \brief
  ///   Overridable that is called for every object that changes its zone during runtime via
  ///   VisTypedEngineObject_cl::SetParentZone
  VISION_APIFUNC virtual void OnRemoveEngineObject(VisTypedEngineObject_cl *pObject);

  /// \brief
  ///   Overridable that is called for every object that is de-serialized during zone streaming. The base implementation is empty.
  VISION_APIFUNC virtual void OnEngineObjectCreated(VisTypedEngineObject_cl *pObject)
  {
  }

  /// \brief
  ///   Overridable that is called for every object before it is disposed during zone unloading. The base implementation is empty.
  VISION_APIFUNC virtual void OnEngineObjectDisposing(VisTypedEngineObject_cl *pObject)
  {
  }

  /// \brief
  ///   Overridable that is called when repositioning is triggered.
  VISION_APIFUNC virtual void OnReposition(const VisZoneRepositionInfo_t &info)
  {
  }

  /// \brief
  ///   Removes all objects from this zone without destroying them.
  VISION_APIFUNC void UnAttachAllEngineObjects();

  /// \brief
  ///   (Re-)loads the zone lightgrid.
  VISION_APIFUNC BOOL LoadZoneLightGrid(BOOL bForceReload=FALSE);

  /// \brief
  ///   Internal function.
  VISION_APIFUNC BOOL EnsureShapesArchiveCreated();

  IVisSceneManager_cl *m_pSceneManager;
  VZoneState_e m_ZoneState;
  VString m_sLightgridFilename;
  bool m_bHandleZone, m_bCollectionChanging;
  UBYTE m_iWantedAction;
  bool m_bAlwaysLoadSnapshot; ///< Set this flag to load all resources always through a snapshot (even when camera is spawned into red ring)
  float m_fCacheDistance,m_fCacheOutDistance,m_fLoadedDistance;
  hkvAlignedBBox m_BoundingBox;
  VResourceSnapshot m_Snapshot;
  VZoneShapesArchive *m_pShapesArchive;
  VSmartPtr<VLoadingTask> m_spResFileLoadingTask;

  VColorRef m_iDebugColor;

  int m_iZoneObjectCount;
  DynArray_cl<VisTypedEngineObject_cl *> m_ZoneObjects;

  hkvVec3d m_vZonePivot; ///< for large coordinate support exported from vForge, usually (0,0,0)
  VLightGridPtr m_spZoneLightGrid;
  float m_fCreationPos;
  float m_fCameraDistance;
  int m_iUnloadCounter;

  ///
  /// @}
  ///

};


/// \brief
///   Implementation of the VisZoneResource_cl class that supports repositioning.
///
/// The IVisSceneManager_cl::CreateZoneResource function creates instances of VisRepositioningZoneResource_cl if repositioning is globally enabled
/// through IVisSceneManager_cl::SetRepositionInterval
class VisRepositioningZoneResource_cl : public VisZoneResource_cl
{
public:
  VisRepositioningZoneResource_cl(VisZoneResourceManager_cl *pManager, IVisSceneManager_cl *pSceneManager)
    : VisZoneResource_cl(pManager, pSceneManager)
  {
    m_bFirstRepositioning = true;
  }

  /// \brief
  ///   Overridden zone function. Repositions all shapes relative to a new global center
  VISION_APIFUNC virtual void OnReposition(const VisZoneRepositionInfo_t &info) HKV_OVERRIDE;

  virtual BOOL Reload() HKV_OVERRIDE;
  virtual BOOL Unload() HKV_OVERRIDE;

  hkvAlignedBBox m_LocalZoneBox; ///< backup of local zone bounding box
  bool m_bFirstRepositioning;       ///< internal flag
};



/// \brief
///   Resource manager class that globally handles all instances of VisZoneResource_cl
/// 
/// The engine's global instance of this manager can be accessed via
/// VisZoneResourceManager_cl::GlobalManager()
class VisZoneResourceManager_cl : public VisResourceManager_cl, public IVisCallbackHandler_cl
{
public:
  /// \brief
  ///   Constructor
  VISION_APIFUNC VisZoneResourceManager_cl();

  /// \brief
  ///   Static function to access the global zone manager 
  VISION_APIFUNC static VisZoneResourceManager_cl &GlobalManager();

  /// \brief
  ///   Internal function, called by the engine
  VISION_APIFUNC void OneTimeInit();

  /// \brief
  ///   Internal function, called by the engine
  VISION_APIFUNC void OneTimeDeInit();

  /// \brief
  ///   Called by the engine every frame in case the DEBUGRENDERFLAG_ZONES is set for
  ///   Vision::Profiling.SetDebugRenderFlags
  VISION_APIFUNC void DebugRender();

  VISION_APIFUNC virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  /// \brief
  ///   Wraps around GetResourceByIndex and casts to VisZoneResource_cl*
  inline VisZoneResource_cl* GetZoneByIndex(int iIndex) {return (VisZoneResource_cl *)GetResourceByIndex(iIndex);}

  /// \brief
  ///   Creates a new zone resource in case a zone with specified filename does not exist yet. Uses
  ///   Vision::GetSceneManager()->CreateZoneResource to create the resource.
  VISION_APIFUNC VisZoneResource_cl *CreateZone(const char *szFilename, const hkvAlignedBBox &bbox);

  /// \brief
  ///   Static helper render function
  VISION_APIFUNC static void DrawRoundedRectangle(const hkvAlignedBBox &bbox, float fRadius, VColorRef iColor);

private:
  static VisZoneResourceManager_cl g_GlobalManager;
};


#endif  // VISAPIZONE_HPP_INCLUDED

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
