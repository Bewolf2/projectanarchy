/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiPhysics.hpp

#ifndef FR_DEFINE_VISAPIPHYSICS
#define FR_DEFINE_VISAPIPHYSICS


#define VISCOLMESH_FLAG_NONE           0 
#define VISCOLMESH_FLAG_DOUBLESIDED    1
#define VISCOLMESH_FLAG_TRANSPARENT    2

class hkvAlignedBBox;
struct VisCollisionInfo_t;
class IVCollisionMeshBase;
class IVCollisionMesh;
class VisStaticMesh_cl;


/// \brief
///   Enumeration to define from which data source trace meshes will be generated.
enum VisTraceMeshBehavior_e
{
  VIS_USE_RENDERMESHES_FOR_TRACELINES = 0,        ///< Generate trace meshes from render meshes (default). Note that using this setting has a significant memory footprint, since it requires double-buffering of render mesh data.
  VIS_USE_COLLISIONMESHES_FOR_TRACELINES = 1      ///< Use collision meshes as trace meshes. This will require less memory, but you won't be able to obtain UV coordinates from trace results.
};

/// \brief
///   Collision mesh structure for indexed triangle sets (used by models).
/// 
/// The collision mesh access is provided with a stride based system, which allows the physics
/// module to share the collision mesh data with the engine.
/// 
/// Alternatively, the collision mesh resources can also be accessed directly.
struct VisColMeshIndexedTriSet_t
{
  hkvVec3* pVertex;                   ///< pointer to the first vertex
  int iVertexStride;                    ///< stride to the next vertex
  int iNumVertices;                     ///< number of vertices
  void *pIndex;                         ///< pointer to the first vertex indices (for the first triangle)
  int iIndexStride;                     ///< stride to the next vertex indices in bytes (for the next triangle)
  int iIndexType;                       ///< Either VIS_INDEXFORMAT_16 or VIS_INDEXFORMAT_32, depending on the index format used
  int iNumTriangles;                    ///< number of triangles
  hkvPlane* pPlane;                  ///< pointer to the first triangle plane equation
  int iPlaneStride;                     ///< stride in bytes to the plane equation for the next triangle in
  unsigned char *pFlags;                ///< pointer to the collision flags of the first triangle (VISCOLMESH_FLAG_NONE, VISCOLMESH_FLAG_DOUBLESIDED, VISCOLMESH_FLAG_TRANSPARENT)
  int iFlagsStride;                     ///< stride in bytes to the collision flags of the next triangle
};


/// \brief
///   Collision mesh structure for triangle arrays (Bezier patches, height fields, triangle lists).
/// 
/// The collision mesh access is provided with a stride based system, which allows the physics
/// module to share the collision mesh data with the engine.
/// 
/// Alternatively, the collision mesh resources can also be accessed directly.
struct VisColMeshTriArray_t
{
  hkvVec3* pVertex;                 ///< pointer to the first three of vertices
  int iTriangleStride;                ///< stride to the next triangle in bytes (pNextVertexSet = pVertex + iTriangleStride)
  int iNumTriangles;                  ///< number of triangles
  hkvPlane* pPlane;                ///< pointer to the first plane
  int iPlaneStride;                   ///< stride to the next plane in bytes
  hkvAlignedBBox* pBox;                       ///< pointer to the first bounding box
  int iBoxStride;                     ///< stride to the next bounding box in bytes
  unsigned short iFlags;              ///< collision flags for the primitive (VISCOLMESH_FLAG_NONE, VISCOLMESH_FLAG_DOUBLESIDED, VISCOLMESH_FLAG_TRANSPARENT)
};


/// \brief
///   Collision mesh class for a physics material
/// 
/// The collision mesh access is provided with a stride based system, which allows the physics
/// module to share the collision mesh data with the engine.
/// 
/// Alternatively, the collision mesh resources can also be accessed directly.
class VColMeshMaterial
{
public: 
  VColMeshMaterial()
  {
    iFlags = 0;
    fStaticFriction = fDynamicFriction = fRestitution = 0.0f;
    fAnisoStaticFriction = fAnisoDynamicFriction = 0.0f;
  }

  int iFlags;                         ///< currently available: Bit 0 (set for anisotropic friction)
  float fStaticFriction;              ///< coefficient for static friction
  float fDynamicFriction;             ///< coefficient for dynamic friction 
  float fRestitution;                 ///< coefficient for restitution
  hkvVec3 vAnisoFrictionDir;          ///< direction of anisotropic friction (if material has anisotropic friction)
  float fAnisoStaticFriction;         ///< static friction in secondary axis of anisotropy
  float fAnisoDynamicFriction;        ///< dynamic friction in secondary axis of anisotropy 
  VString szUserData;                 ///< material user data string
};


/// \brief
///   Class for a physics submesh 
class VPhysicsSubmesh
{
public:
  VPhysicsSubmesh() { iStartIndex = iNumIndices = iFirstVertexIndex = iLastVertexIndex = iCollisionBitmask = iGroupFilter = 0; }       

  int iStartIndex;                    ///< first index in index buffer 
  int iNumIndices;                    ///< number of indices in index buffer
  int iFirstVertexIndex;              ///< index of first vertex in index buffer
  int iLastVertexIndex;               ///< index of last vertex in index buffer
  int iCollisionBitmask;              ///< used at runtime to filter out unwanted collisions
  int iGroupFilter;                   ///< used at runtime to filter out unwanted collisions (mostly for Havok)
};



/// \brief
///   Functionality for accessing the collision meshes of static geometry instances and models
///
/// \note
///  Several parts of the Vision Physics API are obsolete without explicitly being marked as such
///  and should not be used directly. When in doubt, use the functionality provided by the
///  vHavokPhysicsPlugin directly instead of relying on the generic interface.
/// 
/// The VisPhysics_cl class provides functionality for enumerating world primitives, querying the
/// different kinds of collision meshes and computing the relevant primitives for a specified
/// region. This class has been designed as a function set for the physics module in order to get
/// the necessary collision mesh information. One global instance of this class can be accessed via
/// Vision::Physics.
class VisPhysics_cl
{
public:
  VisPhysics_cl();
  ~VisPhysics_cl();

  ///
  /// @name Relevant Primitive Queries
  /// @{
  ///

  /// \brief
  ///   Returns all static geometry instances a bounding box might touch if it moves from Start to
  ///   End.
  /// 
  /// GetRelevantStaticGeometry determines the relevant static geometry for a specified region,
  /// using the internally available visibility information in order to speed up the calculation. 
  /// 
  /// The method basically returns all static geometry instances a bounding box might touch if it
  /// moves from Start to End.
  /// 
  /// \param pGeoColl
  ///   Collection in which to return the static geometry instances
  /// 
  /// \param Start
  ///   start position of the box
  /// 
  /// \param End
  ///   end position of the box
  /// 
  /// \param Box
  ///   bounding box
  /// 
  /// \param iCollFlags
  ///   Collision Mask used for testing.
  /// 
  /// \param bUseCollFlags
  ///   if TRUE, the collision flags of the static geometry instances are compared against the
  ///   passed bitmask before testing, otherwise all static geometry instances are tested.
  /// 
  /// \return
  ///   bool: true if at least one relevant static geometry instance was found.
  bool GetRelevantStaticGeometry(VisStaticGeometryInstanceCollection_cl *pGeoColl, const hkvVec3& Start, const hkvVec3& End, const hkvAlignedBBox &Box, int iCollFlags, bool bUseCollFlags);

  /// \brief
  ///   Gets the relevant static geometry in the specified bounding box.
  /// 
  /// GetRelevantStaticGeometry determines the relevant static geometry for a specified region,
  /// using the internally available visibility information in order to speed up the calculation.
  /// 
  /// \param pGeoColl
  ///   Collection in which to return the static geometry instances
  /// 
  /// \param Box
  ///   bounding box
  /// 
  /// \param iCollFlags
  ///   Collision Mask used for testing.
  /// 
  /// \param bUseCollFlags
  ///   if TRUE, the collision flags of the static geometry instances are compared against the
  ///   passed bitmask before testing, otherwise all static geometry instances are tested.
  /// 
  /// \return
  ///   bool: true if at least one relevant static geometry instance was found.
  bool GetRelevantStaticGeometry(VisStaticGeometryInstanceCollection_cl *pGeoColl, const hkvAlignedBBox &Box, int iCollFlags, bool bUseCollFlags);
 
  /// \brief
  ///   Checks whether any static geometry collides with the passed sphere.
  /// 
  /// The test is accurate on the collision mesh of the primitives.
  /// 
  /// \param vSpherePos
  ///   Sphere center position in world space of the sphere to test.
  /// 
  /// \param fSphereRadius
  ///   Sphere radius (world units)
  /// 
  /// \param useCollFlags
  ///   Flag that determines whether the primitive collision flags should be considered
  /// 
  /// \param pStoreInfo
  ///   Optional structure to be filled with information about the closest touch point
  /// 
  /// \return
  ///   BOOL bResult : TRUE, if any geometry intersects, FALSE if not.
  /// 
  /// \note
  ///   This function is useful to do some AI (pre-)calculation.
  /// 
  /// \note
  ///   The algorithm is reasonably accurate, but also quite performance critical.
  /// 
  /// \note
  ///   The function is faster if no VisCollisionInfo_t structure is passed. This should be done in
  ///   cases where the  BOOL return value is sufficient.
  VISION_APIFUNC BOOL SphereCollidesWithStaticGeometry(const hkvVec3& vSpherePos, float fSphereRadius, BOOL useCollFlags=TRUE, VisCollisionInfo_t *pStoreInfo=NULL);
  
  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Sets the memory limit for collision meshes in bytes.
  /// 
  /// If a limit is set, the collision meshes will not use more than this limit.
  /// 
  /// The limit should not be too small since purging and recreating the meshes will cost signifant
  /// performance in case the limit is exceeded often.
  /// 
  /// The collision meshes memory usage can be profiled in the resource viewer tool.
  /// 
  /// \param iMemSize
  ///   Memory limit in bytes used for collision meshes. A size of 0 means that there is no limit.
  /// 
  /// \sa VisPhysics_cl::GetCollisionMeshCacheSize
  VISION_APIFUNC void SetCollisionMeshCacheSize(__int64 iMemSize);

  /// \brief
  ///   Gets the current memory limit for collision meshes in bytes.
  /// 
  /// \return
  ///   __int64 iMemSize : The current limit, or 0 if there is no limit
  /// 
  /// \sa VisPhysics_cl::SetCollisionMeshCacheSize
  VISION_APIFUNC __int64 GetCollisionMeshCacheSize() const;

  /// \brief
  ///   Specifies whether trace line operations in the collision toolkit are performed against
  ///   collision meshes or trace meshes.
  /// 
  /// \param eTraceMeshBehavior
  ///   See GetCollsionMesh and VisStaticMesh_cl::GetTraceMesh for further details.
  /// 
  /// \sa VisPhysics_cl::GetStaticMeshTraceMeshBehavior
  /// \sa VisStaticMesh_cl::GetCollsionMesh
  /// \sa VisStaticMesh_cl::GetTraceMesh
  inline void SetStaticMeshTraceMeshBehavior(VisTraceMeshBehavior_e eTraceMeshBehavior) { m_eTraceMeshBehavior = eTraceMeshBehavior; }

  /// \brief
  ///   Returns whether trace line operations in the collision toolkit are performed against
  ///   collision meshes or trace meshes.
  inline VisTraceMeshBehavior_e GetStaticMeshTraceMeshBehavior() const { return m_eTraceMeshBehavior; }

  /// \brief
  ///   Renders all collision meshes for debugging in line mode.
  /// 
  /// \param iMeshColor
  ///   Color value for rendering the mesh
  /// 
  /// \param iNormalColor
  ///   Color value for rendering the normal
  /// 
  /// \param fNormalLength
  ///   Length of the normal vector. If <=0.f, the normal will not be rendered.
  /// 
  /// \param iRenderFlags
  ///   Flags used for rendering. The following bitflags are supported can can be combined:
  ///   COLMESH_RENDERFLAG_MESH : Renders the mesh itself COLMESH_RENDERFLAG_NORMALS : Render the
  ///   triangle normals (using the normal length and color) COLMESH_RENDERFLAG_BOXES : Render the
  ///   triangle boxes             COLMESH_RENDERFLAG_PRIMITIVENUMBER : Display the primitive
  ///   number (DEPRECATED) COLMESH_RENDERFLAG_ENSURELOADED : Make sure the mesh is created; If NOT
  ///   specified, this function will only render the active meshes. COLMESH_RENDERFLAG_DEFAULT :
  ///   Common bit combination : Render mesh and normals
  /// 
  /// \return
  ///   int iRenderedCount : Number of rendered meshes
  /// 
  /// \sa IVCollisionMesh
  /// \sa IVCollisionMesh::DebugRender
  VISION_APIFUNC int RenderAllCollisionMeshes(VColorRef iMeshColor=V_RGBA_WHITE, VColorRef iNormalColor=V_RGBA_GREEN, float fNormalLength=5.f, int iRenderFlags=COLMESH_RENDERFLAG_DEFAULT);

  /// \brief
  ///   Purges the collision meshes that are currently allocated
  /// 
  /// Since the static geometry and entities keep their reference to the collision mesh, this
  /// function additionally removes these references.
  /// 
  /// \param bWorld
  ///   If enabled, this function will remove all references from the world primitives
  /// 
  /// \param bEntities
  ///   If enabled, this function will remove all references from entities
  /// 
  /// \return
  ///   int iPurgedCount : Number of purged collision meshes
  VISION_APIFUNC int PurgeCollisionMeshes(BOOL bWorld=TRUE,BOOL bEntities=TRUE);

  ///
  /// @}
  ///

private:
  VisStaticGeometryInstanceCollection_cl *m_pGeoInstanceColl;
  int m_iMaxRelevantPrimitives;     ///< size of the integer array
  VisTraceMeshBehavior_e m_eTraceMeshBehavior;
};


#endif  // FR_DEFINE_VISAPIPHYSICS

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
