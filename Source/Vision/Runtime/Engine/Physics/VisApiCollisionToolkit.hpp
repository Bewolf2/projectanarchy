/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiCollisionToolkit.hpp

#ifndef DEFINE_VisApiCollisionToolkit
#define DEFINE_VisApiCollisionToolkit
#include <string.h>

#include <Vision/Runtime/Engine/System/VisApiDefs.hpp>
#include <Vision/Runtime/Base/Math/Plane/hkvPlane.h>
#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>



class VisBaseEntity_cl;


/// \brief
///   Enumeration which describes the available hit types of traceline results.
enum VisTraceHitType_e
{
  VIS_TRACETYPE_UNKNOWN = 0,        ///< Collider is unknown.
  VIS_TRACETYPE_ENTITYBOX = 2,      ///< Collider is an entity, collision was against its bounding box.
  VIS_TRACETYPE_ENTITYPOLY = 3,     ///< Collider is an entity, collision was against its polygonal mesh.
  VIS_TRACETYPE_STATICGEOMETRY = 4, ///< Collider is a static geometry instance (typically mesh instance).
  VIS_TRACETYPE_TERRAIN = 5,        ///< Collider is a heightfield.
  VIS_TRACETYPE_DECORATION = 6      ///< Collider is a terrain decoration or SpeedTree group object.
};


/// \brief
///   Structure containing information about a traceline hit.
struct VisTraceLineInfo_t
{
  inline VisTraceLineInfo_t() : touchPoint(hkvNoInitialization), primitivePlane(hkvNoInitialization), primitiveVertex(hkvNoInitialization), baseUV(hkvNoInitialization)
  {
    detected = FALSE;
  }

  BOOL detected;                              ///< Will be TRUE if the traceline hit something.

  // Hit type
  VisTraceHitType_e hitType;                  ///< Collision type of the geometry that was hit.
  VColMeshSceneElement_e sceneElementType;    ///< Scene element type of the geometry that was hit.
  VisBaseEntity_cl *colliderEntity;           ///< The collider entity (only valid for hits against entities).
  IVCollisionMesh *pCollisionMesh;            ///< Collision mesh that was hit.

  float distance;                             ///< Distance to the hit in units.
  hkvVec3 touchPoint;                         ///< TouchPoint (point of impact) of the hit.

  // More hit information
  int primitiveNumber;                        ///< Triangle index in collision mesh that was hit.
  VisSurface_cl *pSurface;                    ///< Pointer to the surface of the primitive.
  hkvPlane primitivePlane;                    ///< Hit primitive plane.
  hkvVec3 primitiveVertex;                    ///< One vertex on the primitive that was hit.
  hkvVec2 baseUV;                             ///< Base texture uv coordinates. Only valid for polygon accurate hits. The VIS_TRACESETTINGS_STOREBASEUV needs to be globally enabled.
  VisStaticGeometryInstance_cl *pGeoInstance; ///< Geometry instance pointer if hit scene element is a static geometry instance.
  VBaseSubmesh *pSubmesh;                     ///< The submesh that was hit (pSurface is the same as pSubmesh->GetSurface()).

  /// \brief
  ///   Static helper function to sort the trace result into a table.
  /// 
  /// This function takes an existing table (array of VisTraceLineInfo_t) as input and returns the
  /// table entry where a trace hit with specified fDistance is inserted.
  /// 
  /// This function actually inserts the new entry and moves up all entries that are further away.
  /// 
  /// The returned table entry pointer can be NULL in case all entries in the table are "better",
  /// i.e have a smaller distance value.
  /// 
  /// If the returned pointer is not NULL, the VisTraceLineInfo_t structure needs to be filled out
  /// by custom code. It is not properly initialized; instead it still has the values of the old
  /// entry.
  /// 
  /// \example
  ///   \code
  ///   VisTraceLineInfo_t *pMyEntry = VisTraceLineInfo_t::InsertTraceResult(iTableSize,pFirstStoreResult,fMyHitDistance);
  ///   if (pMyEntry) // we have a better hit than currently in the table
  ///   {
  ///      pMyEntry->detected = TRUE;
  ///      pMyEntry->distance = fMyHitDistance;
  ///      [..] fill in the other members
  ///   }
  ///   \endcode
  ///
  /// \param iCount
  ///   Trace Result Count
  ///
  /// \param pTable
  ///   Pointer to the table to insert into.
  ///
  /// \param fDistance
  ///   Distance at which to insert a new hit.
  VISION_APIFUNC static VisTraceLineInfo_t* InsertTraceResult(int iCount, VisTraceLineInfo_t *pTable, float fDistance);
};

/// \brief
///   Structure containing information about a trace-line query
struct VisTraceLineQuery_t
{
  hkvVec3 m_start;                      ///< Start point.
  hkvVec3 m_end;                        ///< End point.
  int m_flags;						            	///< Flags of type VTraceSettings.

  int m_traceCount;                     ///< Number of entities that were hit.
  VisTraceLineInfo_t m_traceInfo;       ///< Hit result.
};

/// \brief
///   Toolkit class with helper functions for tracelines/line-of-sight tests. Can be accessed via
///   Vision::CollisionToolkit.
///
/// \note
///  Several parts of the Vision Physics API are obsolete without explicitly being marked as such
///  and should not be used directly. When in doubt, use the functionality provided by the
///  vHavokPhysicsPlugin directly instead of relying on the generic interface.
///
class VisCollisionToolkit_cl
{
public :
  VisCollisionToolkit_cl();
  
  /// \brief
  ///   Performs a traceline check between two world positions and check for intersections with
  ///   scene elements.
  /// 
  /// The TraceLine function performs a traceline operation between two given world positions and
  /// returns whether the traceline whether an intersection occurred. The various parameters of
  /// this function allow you to specify whether entities and/or static geometry should be
  /// considered during the intersection computations. Additionally, detailed information about the
  /// trace hit (such as touch point and distance) can be retrieved by passing a VisTraceLineInfo_t
  /// structure.
  /// 
  /// Note that this method provides a lot of features, but has a relatively high overhead and
  /// can't be multithreaded. Especially when using a physics module with support for asynchronous
  /// physics (e.g. Havok Physics), it is strongly suggested to use the
  /// IVisPhysicsModule_cl::EnqueueRaycast functionality instead.
  /// 
  /// \param startOfLine
  ///   Position in world space where the traceline starts.
  /// 
  /// \param endOfLine
  ///   Position in world space where the traceline ends.
  /// 
  /// \param validStaticGeometry
  ///   integer value which defines the static geometry which will be considered during the
  ///   traceline operation. The following values are currently available:
  ///   \li VIS_TRACE_NONE: Don't check against static geometry.
  ///   \li VIS_TRACE_ALL:  Check against static geometry.
  /// 
  /// \param validEntities
  ///   Integer value which defines the entities which will be considered during the traceline
  ///   operation. The value is interpreted as a bitmask and compares the logical AND result with
  ///   the bitmask set on entity basis  via VisBaseEntity_cl::SetTracelineBitmask The following
  ///   bitmask constants are pre-defined:
  ///   \li VIS_TRACE_NONE: don't check against any entities
  ///   \li VIS_TRACE_ALL: check against all entities (bitmask 0xffffffff). Apart from the bitmask,
  ///     you can exclude separate entities from the traceline test by either using the next two
  ///     parameters or by setting the traceline accuracy of these entities to
  ///     VIS_TRACEACC_NOCOLLISION (see VisBaseEntity_cl::SetTraceAccuracy)
  /// 
  /// \param ignoreEnt1
  ///   If defined then this entity will be ignored during the collision detection. This parameter
  ///   is often used to exclude the entity where the trace starts at.
  /// 
  /// \param ignoreEnt2
  ///   If defined then this entity will be ignored during the collision detection
  /// 
  /// \param storeFirstTraceInfo
  ///   If defined then the collision information about the closest intersection will be stored in
  ///   the given structure. You can use this parameter if you want to get detailed information
  ///   about the collision. The structure is the same one which is used for the virtual collision
  ///   function. You can get various information like colliderType, primitiveType, touchPoint,
  ///   distance, plane, ... If no further information about the trace hit is required, then a NULL
  ///   pointer should be passed here to improve performance.
  /// 
  /// \param traceInfoTableFlags
  ///   Currently the following collision info table related flags are available:
  ///   \li VIS_TRACE_STOREENTITIES: all ray-entity intersections will be stored in an internal
  ///     collision table.
  /// 
  ///   \li VIS_TRACE_STOREPRIMITIVES: all static geometry intersections will be stored in an
  ///     internal collision table.
  /// 
  ///   \li VIS_TRACE_STOREALL: all world and entity intersections will be stored in an internal
  ///     collision table The collision info table can then be requested with the
  ///     VisCollisionToolkit_cl::GetTraceInfoTable function. You can use this feature if you need
  ///     to know all the entities and/or static geometry which intersect with a given ray.
  /// 
  /// \return
  ///   BOOL: This function will return TRUE if there was NO intersection between the start and the
  ///   end position, in any other case FALSE will be returned.
  /// 
  /// \note
  ///   Please note that tracelines will not trace for alpha transparencies in textures whatsoever!
  /// 
  /// \note
  ///   In case you perform a traceline operation against entities you should make sure that the
  ///   traceline accuracy of the entities are set.
  /// 
  /// \note
  ///   Not requesting the collision information (storeFirstTraceInfo parameter) significantly
  ///   improves the performance of this function. Whenever you just need to know whether an
  ///   intersection has happened you should pass a NULL pointer to the storeFirstTraceInfo
  ///   parameter. The engine will then not check for the nearest intersection and use some
  ///   additional caching algorithms for best performance.
  /// 
  /// \note
  ///   Generating a collision information table (traceInfoTableFlags parameter) is also quite
  ///   expensive.
  /// 
  /// \note
  ///   Polygon accurate traceline computations for entities are rather expensive, you should use
  ///   bounding box approximations for entities whenever possible.
  /// 
  /// \note
  ///   Skeletal animations will be considered during traceline computations.
  /// 
  /// \sa VisBaseEntity_cl::SetTraceAccuracy
  /// \sa VisBaseEntity_cl::SetTracelineBitmask
  /// 
  /// \example
  ///   \code
  ///   VisTraceLineInfo_t traceInfo;
  ///   BOOL noHit = Vision::CollisionToolkit.TraceLine(start, end, VIS_TRACE_ALL, VIS_TRACE_ALL, this, NULL, &traceInfo);
  ///   \endcode
  VISION_APIFUNC BOOL TraceLine(const hkvVec3& startOfLine, const hkvVec3& endOfLine, ULONG validStaticGeometry = (ULONG)VIS_TRACE_ALL, ULONG validEntities = (ULONG)VIS_TRACE_ALL, VisBaseEntity_cl *ignoreEnt1 = NULL, 
                                VisBaseEntity_cl *ignoreEnt2 = NULL, VisTraceLineInfo_t *storeFirstTraceInfo = NULL, ULONG traceInfoTableFlags = 0);

  /// \brief
  ///   Performs a traceline check between two world positions on a collection of entities
  /// 
  /// In case there are many entities in the scene, this version should be preferred rather than
  /// TraceLine, because the TraceLine function has to test all entities. This is a performance
  /// issue even when there are many early-outs (bitflags)
  /// 
  /// This function can also override the traceline accuracy of each entity in the collection.
  /// 
  /// Note that this method provides a lot of features, but has a relatively high overhead and
  /// can't be multithreaded. Especially when using a physics module with support for asynchronous
  /// physics (e.g. Havok Physics), it is strongly suggested to use the
  /// IVisPhysicsModule_cl::EnqueueRaycast functionality instead.
  /// 
  /// \param startOfLine
  ///   position in world space where the traceline starts
  /// 
  /// \param endOfLine
  ///   position in world space where the traceline ends
  /// 
  /// \param entityCollection
  ///   Reference to an entity collection which contains all entities to be traced
  /// 
  /// \param storeFirstTraceInfo
  ///   If defined then the collision information about the closest intersection will be stored in
  ///   the given structure. You can use this parameter if you want to get detailed information
  ///   about the collision. The structure is the same one which is used for the virtual collision
  ///   function. You can get various information like colliderType, primitiveType, touchPoint,
  ///   distance, plane, ... If no further information about the trace hit is required, then a NULL
  ///   pointer should be passed here to improve performance.
  /// 
  /// \param iTraceFlags
  ///   The flags used for tracing. For valid flags see function SetTraceLineSettings. If this
  ///   value is != -1, it will override the flags set in the SetTraceLineSettings function,
  ///   otherwise the global flags will be used.
  /// 
  /// \param traceInfoTableFlags
  ///   Currently the following collision info table related flags are available:
  ///   \li VIS_TRACE_STOREENTITIES: all ray-entity intersections which happened will be stored in
  ///     an internal collision table.
  /// 
  ///   \li VIS_TRACE_STOREPRIMITIVES: ignored in this version
  /// 
  ///   \li VIS_TRACE_STOREALL: ignored in this version The collision info table can then be
  ///     requested with the VisCollisionToolkit_cl::GetTraceInfoTable function. You can use this
  ///     feature if you need to know all the entities and/or static geometry which intersect with
  ///     a given ray.
  /// 
  /// \return
  ///   int iTraceCount : Number of entities successfully traced
  /// 
  /// \sa VisCollisionToolkit_cl::TraceLine
  VISION_APIFUNC int TraceLineEntities(const hkvVec3& startOfLine, const hkvVec3& endOfLine, VisEntityCollection_cl &entityCollection, VisTraceLineInfo_t *storeFirstTraceInfo = NULL, int iTraceFlags=-1, ULONG traceInfoTableFlags = 0);

  /// \brief
  ///   Performs a traceline check between two world positions and check for intersections with a
  ///   specified list of static geometry instances.
  /// 
  /// The TraceLine function performs a traceline operation between two given world positions and
  /// returns whether the traceline whether an intersection with the passed static geometry
  /// occurred. Additionally, detailed information about the trace hit (such as touch point and
  /// distance) can be retrieved by passing a VisTraceLineInfo_t structure.
  /// 
  /// Note that this method provides a lot of features, but has a relatively high overhead and
  /// can't be multithreaded. Especially when using a physics module with support for asynchronous
  /// physics (e.g. Havok Physics), it is strongly suggested to use the
  /// IVisPhysicsModule_cl::EnqueueRaycast functionality instead.
  /// 
  /// \param startOfLine
  ///   position in world space where the traceline starts
  /// 
  /// \param endOfLine
  ///   position in world space where the traceline ends
  /// 
  /// \param geomCollection
  ///   Collection of static geometry instances to take into account.
  /// 
  /// \param iStoreResultCount
  ///   Maximum number of hits for which to store the results.
  /// 
  /// \param pFirstTraceInfo
  ///   Array of VisTraceLineInfo_t structures in which to store the hits.
  /// 
  /// \return
  ///   int: This function will return the number of intersections (hits) found.
  /// 
  /// \note
  ///   Please note that tracelines will not trace for alpha transparencies in textures whatsoever!
  /// 
  /// \sa VisCollisionToolkit_cl::TraceLine
  /// \sa VisCollisionToolkit_cl::TraceLineEntities
  VISION_APIFUNC int TraceLineStaticGeometry(const hkvVec3& startOfLine, const hkvVec3& endOfLine, const VisStaticGeometryInstanceCollection_cl &geomCollection, int iStoreResultCount=0, VisTraceLineInfo_t *pFirstTraceInfo = NULL);

  /// \brief
  ///   Wraps around info.baseUV.
  VISION_APIFUNC BOOL GetModelBaseUVFromTraceHit(const VisTraceLineInfo_t &info, float &u, float &v);

  /// \brief
  ///   Specifies the global traceline settings.
  /// 
  /// The flags can be changed at any time.
  /// 
  /// \param iFlags
  ///   A combination of bit flags of type VTraceSettings.
  /// 
  /// \note
  ///   By default the VIS_TRACESETTINGS_IGNORETRANSP flag is set (transparent geometry is by
  ///   default ignored during tracelines).
  /// 
  /// \note
  ///   If VIS_TRACESETTINGS_IGNORETRANSP is NOT set, then transparent geometry is treated like
  ///   normal primitives during a traceline. A traceline does not detect the transparent parts of
  ///   a primitive - a primitive is either fully an occluder or not an occluder.
  /// 
  /// \sa VisCollisionToolkit_cl::GetTraceLineSettings
  /// \sa VisCollisionToolkit_cl::TraceLine
  /// 
  /// \example
  ///   \code
  ///   Vision::CollisionToolkit.SetTraceLineSettings( VIS_TRACESETTINGS_IGNORETRANSP );
  ///   \endcode
  VISION_APIFUNC void SetTraceLineSettings(int iFlags = VIS_TRACESETTINGS_DEFAULT);


  /// \brief
  ///   Get the current global traceline settings.
  /// 
  /// GetTraceLineSettings returns the current traceline settings flags of type VTraceSettings.
  /// 
  /// \return
  ///   Current traceline settings flags.
  /// 
  /// \sa VisCollisionToolkit_cl::SetTraceLineSettings
  /// \sa VisCollisionToolkit_cl::TraceLine
  VISION_APIFUNC int GetTraceLineSettings() const;

  /// \brief
  ///   Fetches the current traceline hit info table which has been generated during the last call of
  ///   the VisCollisionToolkit_cl::TraceLine function.
  /// 
  /// The results in the table are sorted by distance.
  /// 
  /// \param traceInfoTable
  ///   Pointer to an user-allocated array that will be filled with VisTraceLineInfo_t pointers. Call this function with NULL
  ///   to query the required size of the table.
  /// 
  /// \return
  ///   Number of hits which have been stored in the traceline hit info table.
  /// 
  /// \note
  ///   The traceline info table will only be valid until the TraceLine call.
  /// 
  /// \example
  ///   \code
  ///     VisTraceLineInfo_t** traceInfoTable = new VisTraceLineInfo_t*[Vision::CollisionToolkit.GetTraceInfoTable()];
  ///     Vision::CollisionToolkit.GetTraceInfoTable(traceInfoTable);
  ///   \endcode
  VISION_APIFUNC int GetTraceInfoTable(VisTraceLineInfo_t **traceInfoTable = NULL);

  /// \brief
  ///   Checks whether an entity is still within a certain angle to another entity.
  /// 
  /// InSight checks whether the destination entity is still within a certain angle to the source
  /// entity.
  /// 
  /// \param sourceObj
  ///   Source entity.
  /// 
  /// \param destObj
  ///   Destination entity.
  /// 
  /// \param maxAngle
  ///   Maximum angle from the source entities view direction which is allowed.
  /// 
  /// \note
  ///   Since this function uses the origin of the entities for the calculations, you will not get
  ///   accurate results in case your entities have their local centers at different places.
  VISION_APIFUNC BOOL InSight(const VisBaseEntity_cl *sourceObj, const VisBaseEntity_cl *destObj, int maxAngle = 72);


  /// \brief
  ///   Check whether a given entity can see another entity.
  /// 
  /// IsVisible checks whether the given source entity is actually possible to see the the
  /// destination entity. The check is based on a fast angular check (see InSight) and an
  /// additional trace line check.
  /// 
  /// This function calls the VisCollisionToolkit_cl::InSight function at first, in case of a
  /// positive return value the VisCollisionToolkit_cl::TraceLine function is additionally called.
  /// 
  /// \param sourceObj
  ///   Source entity.
  /// 
  /// \param destObj
  ///   Destination entity.
  /// 
  /// \param maxAngle
  ///   Maximum angle which is allowed during the angular check (see InSight function)
  /// 
  /// \param checkWorld
  ///   If TRUE then static geometry is used as occluders.
  /// 
  /// \param flags
  ///   reserved
  /// 
  /// \return
  ///   TRUE if the source entity is currently able to see the destination entity.
  /// 
  /// \note
  ///   Since this function uses the origin of the entities for the calculations, you will not get
  ///   accurate results in case your entities have their local centers at different places.
  VISION_APIFUNC BOOL IsVisible(const VisBaseEntity_cl *sourceObj, const VisBaseEntity_cl *destObj, int maxAngle = 72, BOOL checkWorld = FALSE , ULONG flags = 0);
    

  
private:
  friend class VisPhysics_cl;
  friend class VisGame_cl;

  // Trace info table:
  void SetTraceInfoTableGeneration(VisTraceLineInfo_t **newStoreTraceInfoTable, int validElements);
  void PrepareTraceInfoTable();

  inline void FillTraceInfo (VisTraceLineInfo_t *x) 
  {
    memset (x, 0, sizeof (VisTraceLineInfo_t));
  }

  inline void ResetResults()
  {
    for (unsigned int i=0;i<storeTraceInfoTableSize;i++)
      storeTraceInfoTable[i].detected = FALSE;
  }

  // Helper functions:
  inline char HitBoundingBox(float *minB,float *maxB, const float *origin, const float *dir,float *coord, hkvVec3& outHitPlaneNormalScaled);
  inline BOOL CheckRaySphereIntersection(const hkvVec3& vStart, const hkvVec3& vDir, float fLen, const hkvVec3& vSphereCenter, float fRadius, float &t);
  unsigned char GetTraceFlags();
  inline VisTraceAccuracy_e GetEntityTraceAccuracy(const VisBaseEntity_cl *pEntity, int iTraceFlags);
  inline bool CheckEntityTraceLineFlags(VisBaseEntity_cl *pEntity, int iCollisionBitmask, int iTraceMask);

  // Higher-level trace functions
  BOOL TraceCheckRayWorldIntersection(const hkvVec3& traceOrigin, const hkvVec3& traceDir, float traceLen, VisTraceLineInfo_t *traceInfo, int iCollisionBitMask, int iFlags );
  BOOL TraceCheckRayEntityIntersection(const hkvVec3& traceOrigin, const hkvVec3& traceDir, float traceLen, VisTraceLineInfo_t *traceInfo, int iCollisionBitMask, int iFlags, VisBaseEntity_cl *pIgnoreEnt1,  VisBaseEntity_cl *pIgnoreEnt2);
  BOOL TraceCheckRayEntityIntersection(VisEntityCollection_cl &entityColl, const hkvVec3& traceOrigin, const hkvVec3& traceDir, float traceLen, VisTraceLineInfo_t *traceInfo, int iCollisionBitMask, int iFlags);
  BOOL TraceCheckRayAllIntersection(const hkvVec3& traceOrigin, const hkvVec3& traceDir, float traceLen, VisTraceLineInfo_t *traceInfo, ULONG iStaticGeometryBitmask, ULONG iEntityBitmask, int iFlags, VisBaseEntity_cl *ignoreEnt1, VisBaseEntity_cl *ignoreEnt2);

  // Lower-level trace functions
  bool GetRelevantTraceLineEntities(const hkvVec3& Start, const hkvVec3& Target, VisEntityCollection_cl &entityCollection);
  bool TestEntityIntersection(const hkvVec3& Start, const hkvVec3& Target, float fTraceLen, char iTraceMask, int iCollisionBitmask, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo, VisBaseEntity_cl *pIgnoreEnt1, VisBaseEntity_cl *pIgnoreEnt2);
  bool TestEntityIntersection(VisEntityCollection_cl &entityCollection, const hkvVec3& Start, const hkvVec3& Target, float fTraceLen, char iTraceMask, int iCollisionBitmask, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo);
  bool TestStaticGeometryIntersection(const hkvVec3& Start, const hkvVec3& Target, float fTraceLen, char iTraceMask, int iCollisionBitmask, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo);

  // Physics / Collision support functions:
  int GetStaticGeometryCollisionCandidates(const hkvVec3& Start, const hkvVec3& Target, const hkvAlignedBBox& box, VisStaticGeometryInstanceCollection_cl *pPrimColl, int iMaxPrimCount, bool bUseCollFlags, int iCollisionBitmask);
  int GetStaticGeometryCollisionCandidates(const hkvAlignedBBox &FloatBox, VisStaticGeometryInstanceCollection_cl *pPrimColl, int iMaxPrimCount, bool bUseCollFlags, int iCollisionBitmask);

  // Global settings:
  int m_iTraceFlags;                // traceline settings

  // Global state - formerly contained in VisCoreTraceline_t:
  VisTraceLineInfo_t *storeTraceInfoTable;
  unsigned int storeTraceInfoTableCtr;                   // colInfoTable counter
  unsigned int storeTraceInfoTableSize;                  // colInfoTable size
  unsigned int storeTraceInfoTableElements;              // valid colInfoTable elements (see traceline flags)

  unsigned int iTraceCount;
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
