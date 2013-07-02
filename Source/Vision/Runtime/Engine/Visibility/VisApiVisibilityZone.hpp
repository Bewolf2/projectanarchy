/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVisibilityZone.hpp

#ifndef DEFINE_VISAPIVISIBILITYZONE
#define DEFINE_VISAPIVISIBILITYZONE

#include <Vision/Runtime/Engine/Renderer/OcclusionQuery/VisApiOcclusionQueryObject.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiPortal.hpp>

typedef VSmartPtr<VisVisibilityZone_cl> VisVisibilityZonePtr;

class VisLightSrcCollection_cl;
class VisEntityCollection_cl;
class VisVisibilityObjectCollection_cl;
class VisStaticGeometryInstanceCollection_cl;
class IVisSceneManager_cl;
class VisVisibilityZone_cl;


#define VIS_VISIBILITYZONEFLAGS_NONE                          0x00000000
#define VIS_VISIBILITYZONEFLAGS_FALLBACKZONE                  V_BIT(0)
#define VIS_VISIBILITYZONEFLAGS_CALLVIRTUALTAGFUNCTION        V_BIT(1)
#define VIS_VISIBILITYZONEFLAGS_INFINITEHEIGHTFORASSIGNMENT   V_BIT(2)
#define VIS_VISIBILITYZONEFLAGS_CUSTOMASSIGNMENTBOUNDINGBOX   V_BIT(3)
#define VIS_VISIBILITYZONEFLAGS_DEFAULT                       (VIS_VISIBILITYZONEFLAGS_CALLVIRTUALTAGFUNCTION)

  /// \brief
  ///   Enum used for function VisVisibilityZone_cl::SetVisibilityZoneSerializationMode
enum VisVisibilityZoneSerializationMode_e
{
  VIS_SERIALIZE_NONE = 0,     ///< No visibility related information is saved
  VIS_SERIALIZE_AS_PROXIES,   ///< Visibility zones are referenced by their unique ID
  VIS_SERIALIZE_FULL,         ///< Full object serialization
};


/// \brief
///   Class describing a visibility zone.
/// 
/// A visibility zone is basically a volume containing scene elements. During visibility
/// determination, the visibility collector (see IVisVisibilityCollector_cl) traverses the
/// visibility zones and determines which zones and which scene elements inside these zones are
/// visible. Visibility zones are either connected through portals or directly. Portals are
/// typically used in indoor scenes, where a large part of the scene is always occluded. Direct
/// connections between visibility zones can be used to subdivide large open spaces and therefore
/// improve visibility determination performance.
/// 
/// Scene elements can be in zero, one or multiple visibility zones at the same time. Transitions
/// of scene elements through portals or into neighboring visibility zones are handled
/// automatically using the functionality provided by the VisObject3DVisData_cl class.
/// 
/// Visibility zones have to be added to the active scene manager (see IVisSceneManager_cl) before
/// they are used.
/// 
/// Visibility zones are always represented by an axis-aligned bounding box. While a visibility
/// zone MAY NOT overlap any of the visibility zones directly connected to it (though it has to
/// touch them), it MAY overlap visibility zones connected through portals. This is due to the fact
/// that portals are basically determined by the scene geometry and can be at arbitrary locations
/// and arbitrary angles.
/// 
/// Note that all portals connected with a visibility zone have to be completely inside the
/// bounding box of that visibility zone.
/// 
/// Custom implementations may derive classes from VisVisibilityZone_cl and implement some of the
/// virtual functions.
class VisVisibilityZone_cl : public VRefCounter, public VisTypedEngineObject_cl, public VUserDataObj, public IVSerializationProxyCreator
{
public:

  ///
  /// @name Construction / Destruction
  /// @{
  ///

  /// \brief
  ///   Constructs a new visibility zones with the given bounding box.
  VISION_APIFUNC VisVisibilityZone_cl(const hkvAlignedBBox &bbox);

  /// \brief
  ///   Default constructor. Initializes the visibility zone's bounding box as empty.
  VISION_APIFUNC VisVisibilityZone_cl();

  /// \brief
  ///   Destructor of the VisVisibilityZone_cl.
  VISION_APIFUNC ~VisVisibilityZone_cl();

  /// \brief
  ///   Checks whether this zone is still valid and not deleted already. For debug purposes
  VISION_APIFUNC void AssertValid();

  ///
  /// @}
  ///

  ///
  /// @name Flags
  /// @{
  ///

  /// \brief
  ///   Set the bit flag constants for this zone. These constants are of type
  ///   VIS_VISIBILITYZONEFLAGS_xyz)
  inline void SetZoneFlags(unsigned int iFlags) {m_iZoneFlags=iFlags;}

  /// \brief
  ///   Get the bit flag constants for this zone. These constants are of type
  ///   VIS_VISIBILITYZONEFLAGS_xyz)
  inline unsigned int GetZoneFlags() const {return m_iZoneFlags;}

  ///
  /// @}
  ///

  ///
  /// @name Scene Element Handling
  /// @{
  ///

  /// \brief
  ///   Returns the minimum distance of the passed bounding box to the static geometry in the
  ///   visibility zone.
  /// 
  /// Since this method actually computes the distance between the box and the indvidual static
  /// geometry inside the visibility zone, this method may take some time to finish. It is,
  /// however, required for accurate assigment of dynamic scene elements to visibility zones.
  /// 
  /// \param RefBox
  ///   The bounding box to test.
  /// 
  /// \param pBestGeoInstance
  ///   Pointer to the closest geometry instance found.
  /// 
  /// \return
  ///   float: Distance of the closest static geometry instance to the center of the box in world
  ///   units.
  VISION_APIFUNC float GetDistanceFromGeometry(const hkvAlignedBBox &RefBox, VisStaticGeometryInstance_cl *&pBestGeoInstance) const;

  /// \brief
  ///   Returns a collection containing all light sources in this visibility zone.
  /// 
  /// Note that scene elements may be in multiple visibility zones at the same time. 
  /// 
  /// \return
  ///   VisLightSrcCollection_cl *: Collection of all lights in this zone.
  VISION_APIFUNC inline const VisLightSrcCollection_cl *GetLightSources() { return m_pLightSources;}

  /// \brief
  ///   Returns a collection containing all entities in this visibility zone.
  /// 
  /// Note that scene elements may be in multiple visibility zones at the same time. 
  /// 
  /// \return
  ///   VisEntityCollection_cl *: Collection of all entities in this zone.
  VISION_APIFUNC inline const VisEntityCollection_cl *GetEntities() { return m_pEntities;}

  /// \brief
  ///   Returns a collection containing all visibility objects in this visibility zone.
  /// 
  /// Note that scene elements may be in multiple visibility zones at the same time. 
  /// 
  /// \return
  ///   VisVisibilityObjectCollection_cl *: Collection of all visibility objects in this zone.
  VISION_APIFUNC inline const VisVisibilityObjectCollection_cl *GetVisObjects() { return m_pVisibilityObjects;}

  /// \brief
  ///   Returns a collection containing all static geometry instances in this visibility zone
  /// 
  /// Note that scene elements may be in multiple visibility zones at the same time. 
  /// 
  /// \return
  ///   VisStaticGeometryInstanceCollection_cl *: Collection of all static geometry instances in
  ///   this zone.
  VISION_APIFUNC inline const VisStaticGeometryInstanceCollection_cl* GetStaticGeometryInstances() 
  { 
    return m_pStaticGeometryInstances; 
  }


  ///
  /// @}
  ///

  ///
  /// @name Portals and Connected Zones
  /// @{
  ///

  /// \brief
  ///   Creates the specified number of portals for this zone.
  /// 
  /// The created portals belong to this visibility zone. They can be queried with GetPortal and
  /// filled with data.
  /// 
  /// \param iCount
  ///   Number of portals to create.
  VISION_APIFUNC void CreatePortals(int iCount);

  /// \brief
  ///   Returns the number of portals in this visibility zone.
  inline unsigned int GetPortalCount() const { return m_iNumPortals;}

  /// \brief
  ///   Returns a reference to the portal with the specified index. The index has to be >=0 and
  ///   less than the number of portals in the zone.
  inline VisPortal_cl& GetPortal(unsigned int iPortal) const
  {
    VASSERT(iPortal<m_iNumPortals);
    return m_pPortals[iPortal];
  }

  /// \brief
  ///   Connects a neighboring visibility zone to a visibility zone.
  /// 
  /// Directly connects a neighboring visibility zone to a visibility zone (without a portal).
  /// 
  /// This mechanism allows scene traversal during visibility determination to progress from the
  /// visibility zone to one or more connected visibility zones without clipping the active view
  /// frustum (like it would be the case when connecting zones via portals).
  /// 
  /// Note that when adding zone A to zone B, zone B should also be added to zone A (zone relations
  /// should be reciprocal).
  /// 
  /// The neighboring zones connected with this mechanism may not overlap, but they have to touch
  /// each other.
  /// 
  /// Subdividing large open spaces holding many scene elements with a grid of visibility zones can
  /// significantly enhance performance of the visibility determination.
  /// 
  /// Do NOT connect a zone to another zone through this mechanism and a portal at the same time.
  /// This will cause unpredictable results.
  /// 
  /// \param pZone
  ///   Visibility zone to connect.
  VISION_APIFUNC void AddVisibilityZone(VisVisibilityZone_cl *pZone);

  /// \brief
  ///   Removes the specified visibility zone from the list of neighboring zones.
  VISION_APIFUNC void RemoveVisibilityZone(VisVisibilityZone_cl *pZone);

  /// \brief
  ///   Reset the list of connected zones
  VISION_APIFUNC void RemoveAllVisibilityZones();

  /// \brief
  ///   Returns the number of neighboring (i.e., directly connected) visibility zones.
  inline unsigned int GetVisibilityZoneCount() const { return m_iNumVisibilityZones; }

  /// \brief
  ///   Returns the neighboring (i.e., directly connected) visibility zone with the specified
  ///   index.
  inline VisVisibilityZone_cl *GetVisibilityZone(unsigned int iZoneIndex) const
  {
    VASSERT(iZoneIndex < m_iNumVisibilityZones);
    return m_VisibilityZones.GetDataPtr()[iZoneIndex];
  }

  /// \brief
  ///   Indicates whether this zone is directly connected to the passed zone.
  inline bool IsConnected(VisVisibilityZone_cl *pZone) const
  {
    return m_VisibilityZones.GetElementPos(pZone, m_iNumVisibilityZones)>=0;
  }


  ///
  /// @}
  ///

  ///
  /// @name Bounding Box
  /// @{
  ///

  /// \brief
  ///   Returns the bounding box of this visibility zone.
  inline const hkvAlignedBBox &GetBoundingBox() const { return m_BoundingBox; }

  /// \brief
  ///   Sets the bounding box of this visibility zone.
  VISION_APIFUNC void SetBoundingBox(const hkvAlignedBBox &bbox);

  /// \brief
  ///   Optionally sets a different bounding box that is used for assigning geometry
  /// 
  /// Quite naturally, by default the same bounding box as set via SetBoundingBox is used.
  /// 
  /// This function can be called with NULL to reset the default behavior again.
  /// 
  /// \param pBBox
  ///   Custom bounding box (will be copied). If NULL the zone's bounding box is used
  VISION_APIFUNC void SetAssignmentBoundingBox(const hkvAlignedBBox *pBBox);

  /// \brief
  ///   Returns a bounding box reference that should be used for all geometry assignment relevant
  ///   functions.
  /// 
  /// This box is the same as the zone's visibility box (GetBoundingBox()) but can be repaced by a
  /// different box via SetAssignmentBoundingBox.
  /// 
  /// If the VIS_VISIBILITYZONEFLAGS_INFINITEHEIGHTFORASSIGNMENT flag is set for this zone, this
  /// box has very large z-extents so it passes every overlapping test.
  /// 
  /// \return
  ///   const hkvAlignedBBox &bbox : Bounding box used for geometry assignment.
  inline const hkvAlignedBBox &GetAssignmentBoundingBox() const { return m_AssignmentBoundingBox; }

  /// \brief
  ///   Returns the bounding sphere of this visibility zone.
  VISION_APIFUNC inline const hkvBoundingSphere& GetBoundingSphere() const { return m_BoundingSphere; }


  ///
  /// @}
  ///

  ///
  /// @name Adding/removing Scene Elements
  /// @{
  ///

  /// \brief
  ///   Manually adds a dynamic scene element to this visibility zone.
  /// 
  /// Dynamic scene elements are derived from VisObject3D_cl. Note that in order to be handled
  /// correctly, they need to have a VisObject3DVisData_cl object attached to them.
  /// 
  /// Never call this method for objects using automatic status tracking (see
  /// VisObject3DVisData_cl::GetAutomaticUpdate and VisObject3DVisData_cl::SetAutomaticUpdate).
  /// Also note that automatic status tracking is enabled by default!
  /// 
  /// When using this method, take into account that scene elements have to be added to visibility
  /// zones even if only parts of them extend into the respective visibility zone.
  /// 
  /// \param pObject
  ///   scene element to add.
  /// 
  /// \sa VisObject3DVisData_cl::SetAutomaticUpdate
  /// \sa VisObject3DVisData_cl::GetAutomaticUpdate
  VISION_APIFUNC void AddDynamicSceneElement(VisObject3D_cl *pObject);

  /// \brief
  ///   Removes a dynamic scene element from the zone. Never call this method for objects using
  ///   automatic status tracking!
  VISION_APIFUNC void RemoveDynamicSceneElement(VisObject3D_cl *pObject);

  /// \brief
  ///   Adds a static geometry instance to this visibility zone
  /// 
  /// Scene elements may be assigned to multiple visibility zones.
  /// 
  /// \param pGeomInstance
  ///   Static geometry instance to add.
  VISION_APIFUNC void AddStaticGeometryInstance(VisStaticGeometryInstance_cl *pGeomInstance);

  /// \brief
  ///   Removes a static geometry instance from the visibility zone
  VISION_APIFUNC void RemoveGeometryInstance(VisStaticGeometryInstance_cl *pGeomInstance);

  /// \brief
  ///   Optimized function that removes all static geometry instances from the visibility zone that are currently tagged
  VISION_APIFUNC void RemoveTaggedGeometryInstances();

  ///
  /// @}
  ///

  ///
  /// @name Overridables
  /// @{
  ///
  
  /// \brief
  ///   Virtual overridable that is called when this zone is added to the passed scene manager.
  VISION_APIFUNC virtual void OnAddedToSceneManager(IVisSceneManager_cl *pManager);

  /// \brief
  ///   Virtual overridable that is called after this zone has been removed from the passed scene
  ///   manager.
  VISION_APIFUNC virtual void OnRemovedFromSceneManager(IVisSceneManager_cl *pManager);

  /// \brief
  ///   Fills the passed collection with all dynamic scene objects referenced by this zone
  VISION_APIFUNC virtual void GetAllDynamicSceneObjects(VisObject3DCollection_cl *pColl);

  /// \brief
  ///   Overridable that is called by the visibility system every frame that it is detected to be
  ///   visible. Derived classes should not call the base implementation as this deactivates this
  ///   feature.
  /// 
  /// Note that this function is not necessarily called by the main thread. Furthermore it might be
  /// called multiple times during the visibility computation process. For instance if the zone is
  /// traversed through different portals using different frustum.
  /// 
  /// Thread-safety is a must for for proper impelemtations.
  ///
  /// \param pCollector
  ///   Visibility collector this function is called from.
  ///
  /// \param pFrustum
  ///   The current view frustum.
  VISION_APIFUNC virtual void OnTagVisible(IVisVisibilityCollector_cl *pCollector, VisFrustum_cl *pFrustum)
  {
    m_iZoneFlags&=(~VIS_VISIBILITYZONEFLAGS_CALLVIRTUALTAGFUNCTION);
  }

  /// \brief
  ///   Overridable that is called by vForge after visibility info has been generated. Custom
  ///   classes can do some initialization here.
  VISION_APIFUNC virtual void OnFinishVisibilityBuilt(IVisSceneManager_cl *pSceneManager);

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  
  /// \brief
  ///   Overridden IVSerializationProxyCreator function. Used internally.
  VISION_APIFUNC VOVERRIDE IVSerializationProxy *CreateProxy();

  /// \brief
  ///   Static helper function to load or save a visibility zone. This function should be used
  ///   rather than << and >> operators
  /// 
  /// Dependent on the archive's status it loads or saves a visibility zone. In case of loading it
  /// returns the new zone (the passed zone can be NULL then).
  /// 
  /// Dependent on the status defined via SetSaveVisibilityZonesAsProxies a zone is either saved
  /// via proxy (only its unique ID) or via full object serialization. It is also stored in the
  /// file which type of serialization has been used.
  ///
  /// \param ar
  ///   Archive to serialize to/deserialize from.
  ///
  /// \param pZone
  ///   Visibility zone to serialize/deserialize.
  VISION_APIFUNC static VisVisibilityZone_cl *DoArchiveExchange(VArchive &ar, VisVisibilityZone_cl *pZone);


  /// \brief
  ///   Globally defines the saving behavior for visibility zones that are saved via DoArchiveExchange
  /// 
  /// The following modes are currently supported:
  ///   \li VIS_SERIALIZE_NONE : No visibility zone assignment information is saved along with object serialization
  ///   \li VIS_SERIALIZE_AS_PROXIES : (default) Only visibility zone IDs are saved so at loading time the zone will be looked
  ///       up in the scene manager by matching ID. This can result in a NULL return value if a zone with
  ///       specified ID is not in the scene manager. 
  ///   \li VIS_SERIALIZE_FULL : Full object serialization (same as ar << pZone;). Might cause zone duplication at loading time
  /// 
  /// While full serialization is suitable for full scene serialization, proxy
  /// serialization is useful when saving game objects that are part of an existing scene. Object archives that can be shared across scenes (vprefabs)
  /// however should use VIS_SERIALIZE_NONE so objects receive fresh visibility assignment after loading.
  /// 
  /// Note that the mode is only relevant for saving visibility zones. When loading zones, the serialization
  /// type at saving time is retrieved from the file as well.
  ///
  /// \param eMode
  ///   The new serialization mode
  VISION_APIFUNC static void SetVisibilityZoneSerializationMode(VisVisibilityZoneSerializationMode_e eMode);

  /// \brief
  ///   Returns the mode that has been set with SetVisibilityZoneSerializationMode
  VISION_APIFUNC static VisVisibilityZoneSerializationMode_e GetVisibilityZoneSerializationMode();


  /// \brief
  ///   For backwards compatibility: Wraps around SetVisibilityZoneSerializationMode and sets VIS_SERIALIZE_AS_PROXIES or VIS_SERIALIZE_FULL
  inline static void SetSaveVisibilityZonesAsProxies(BOOL bStatus)
  {
    SetVisibilityZoneSerializationMode(bStatus ? VIS_SERIALIZE_AS_PROXIES : VIS_SERIALIZE_FULL);
  }


  /// \brief
  ///   For backwards compatibility: Wraps around GetVisibilityZoneSerializationMode and returns GetVisibilityZoneSerializationMode()==VIS_SERIALIZE_AS_PROXIES
  inline static BOOL GetSaveVisibilityZonesAsProxies()
  {
    return GetVisibilityZoneSerializationMode()==VIS_SERIALIZE_AS_PROXIES;
  }


  ///
  /// @}
  ///

  ///
  /// @name Internal Functions
  /// @{
  ///

  // Tagging
  inline void Tag() { m_iTagged = m_iTagCtr; }
  inline void Untag() { m_iTagged = m_iTagCtr-1; }
  inline bool IsTagged() { return (m_iTagged==m_iTagCtr); }
  VISION_APIFUNC static void IncTagCounter();
  
  // Index in Scene Manager
  inline void SetIndex(int iIndex) { m_iIndexInSceneManager = iIndex; }
  inline int GetIndex() const { return m_iIndexInSceneManager; }

  // Serialization (VisTypedEngineObject_cl overrides)
  V_DECLARE_SERIAL_DLLEXP( VisVisibilityZone_cl,  VISION_APIDATA )
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );
#ifdef SUPPORTS_SNAPSHOT_CREATION
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif
  VISION_APIFUNC void DebugRender(IVRenderInterface *pRI, bool bVisible) const;

  /// \brief
  ///   Returns the occlusion query object for this visibility zone. Used for visibility
  ///   determination.
  VISION_APIFUNC inline const VisOcclusionQueryObject_cl &GetOccQueryObject() const { return m_OccQueryObject; }


  VISION_APIFUNC static bool IsPortalInBox(hkvAlignedBBox &box, VisPortal_cl &portal);

protected:

  void Init();

  int m_iIndexInSceneManager;

  static unsigned int m_iTagCtr;
  unsigned int m_iTagged;

  VisLightSrcCollection_cl  *m_pLightSources;
  VisEntityCollection_cl    *m_pEntities;
  VisVisibilityObjectCollection_cl *m_pVisibilityObjects;
  VisStaticGeometryInstanceCollection_cl *m_pStaticGeometryInstances;
  VisObject3DCollection_cl *m_pCustomSceneElements;

  unsigned int m_iZoneFlags;
  unsigned int m_iNumPortals;
  VisPortal_cl *m_pPortals;

  DynArray_cl<VisVisibilityZone_cl *>m_VisibilityZones;
  unsigned int m_iNumVisibilityZones;

  hkvAlignedBBox m_BoundingBox;
  hkvBoundingSphere m_BoundingSphere;

  int m_iSerializationFlags;

  VisOcclusionQueryObjectVisZone_cl m_OccQueryObject;

  ///
  /// @}
  ///

public:
  VColorRef m_iDebugColor; ///< for displaying zones
  hkvAlignedBBox m_AssignmentBoundingBox;

  static VisVisibilityZoneSerializationMode_e g_ZoneSerializationMode;
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
