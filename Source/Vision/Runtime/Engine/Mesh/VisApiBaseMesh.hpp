/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiBaseMesh.hpp

#ifndef DEFINE_VISAPIBASEMESH
#define DEFINE_VISAPIBASEMESH

#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>

#define VMESH_VERSION_1           1
#define CURRENT_VMESH_VERSION     VMESH_VERSION_1
#define VMESH_VERSION_UNDEFINED   -1

class VBaseSubmesh;

/// \brief
///   Enum that defines the mesh resource type as returned by VBaseMesh::GetMeshType
enum VMeshType_e
{
  VMESH_BASEMESH = 0,   ///< Not used by the engine - can be used for custom mesh types
  VMESH_STATICMESH = 1, ///< Resource is a static mesh resource (VisStaticMesh_cl)
  VMESH_DYNAMICMESH = 2 ///< Resource is a dynamic mesh resource (VDynamicMesh)
};

// internal flags - correspond to vGeom2 flags
#define VMAT_FLAG_DOUBLESIDED                 V_BIT(0)
#define VMAT_FLAG_VERTEXSHADE                 V_BIT(1)
#define VMAT_FLAG_LIGHTMAPPED                 V_BIT(2)
#define VMAT_FLAG_FORCEPRIMARYOPAQUEPASS      V_BIT(3)
#define VMAT_FLAG_FORCETRANSPARENTPASS        V_BIT(4)
#define VMAT_FLAG_FORCESECONDARYOPAQUEPASS    V_BIT(5)
#define VMAT_FLAG_DISABLE_CAST_STATIC_SHADOWS V_BIT(6)
#define VMAT_FLAG_DISABLE_DEPTH_WRITE         V_BIT(7)

//Geometry Info flags
#define VGIF_CAST_DYNAMIC_SHADOWS 1
#define VGIF_CAST_STATIC_SHADOWS  2
#define VGIF_DEFAULT  (VGIF_CAST_DYNAMIC_SHADOWS | VGIF_CAST_STATIC_SHADOWS)

/// \brief
///   Class representing geometry info of a submesh (VBaseSubmesh).
/// 
/// Each SubMesh contains an index linking it to its geometry info in the mesh. This way multiple submeshes 
//  can reference the same geometry info. The Geometry Info can be used to identify the geometry by index
//  or by tag, and provides additional information on how the submesh should be used (LOD, lighting, tracing,
//  visibility, etc)
class VBaseGeometryInfo
{
public:
  VBaseGeometryInfo();
  
  unsigned int m_iTag;          ///< custom user tag
  unsigned int m_iVisibleMask;  ///< The default visible bitmask for geometry
  unsigned short m_iLightMask;  ///< The default bitmask for receiving dynamic lights
  unsigned short m_iTraceMask;  ///< The default traceline bimask

  unsigned short m_sFlags;      ///< additional geometry flags. See VGIF_xyz bit constants
  unsigned short m_sUserFlags;  ///< one more value for user flags
    
  short m_iLODIndex;            ///< The LOD index that this geometry is associated with
  float m_fNearClipDistance;    ///< Default near clip distance of the geometry (0.0f for no clipping)
  float m_fFarClipDistance;     ///< Default far clip distance of the geometry (0.0f for no clipping)
  hkvVec3 m_vClipReference;   ///< Relative position that is used to measure the camera distance for clipping

  VString m_name;               ///< User definable name of the geometry group
  
  /// \brief
  ///   Helper function that tests the VGIF_CAST_DYNAMIC_SHADOWS bit on the m_sFlags member
  inline bool GetCastDynamicShadows() 
  { 
    return (m_sFlags & VGIF_CAST_DYNAMIC_SHADOWS) !=0;
  }

  /// \brief
  ///   Helper function that tests the VGIF_CAST_STATIC_SHADOWS bit on the m_sFlags member
  inline bool GetCastStaticShadows()  
  { 
    return (m_sFlags & VGIF_CAST_STATIC_SHADOWS) !=0;
  }

  /// \brief
  ///   Helper function that indicates whether this group supports clipping 
  inline bool ContainsClipDistance() const
  {
    return m_fNearClipDistance>0.f || m_fFarClipDistance>0.f;
  }

  /// \brief
  ///   Helper function to determine whether the object is clipped given the passed camera position
  bool IsNearOrFarClipped(const hkvVec3& vCameraPos) const
  {
    float fDistSqr = GetClipDistanceSqr(vCameraPos);
    return ((m_fNearClipDistance>0.f) && (fDistSqr<(m_fNearClipDistance*m_fNearClipDistance))) 
        || ((m_fFarClipDistance>0.f) && (fDistSqr>=(m_fFarClipDistance*m_fFarClipDistance)));
  }

  /// \brief
  ///   Helper function to return the square of the camera distance
  inline float GetClipDistanceSqr(const hkvVec3& vCameraPos)  const
  {
    hkvVec3 vDiff = m_vClipReference - (const hkvVec3& )vCameraPos;
    return vDiff.dot (vDiff);
  }

  /// \brief
  ///   Helper function to return the camera distance
  inline float GetClipDistance(const hkvVec3& vCameraPos)  const
  {
    return hkvMath::sqrt (GetClipDistanceSqr(vCameraPos));
  }

};



/// \brief
///   Shared base class for static meshes (VisStaticMesh_cl) and dynamic meshes (VDynamicMesh). 
/// 
/// Since most of the code between static and dynamic meshes is shared, it is contained in this abstract
/// base class. 
class VBaseMesh : public VManagedResource
{
public:

  ///
  /// @name Construction and Destruction; Allocation/deallocation
  /// @{
  ///

  /// \brief
  ///   Constructor of the mesh class. Do not call (class is abstract).
  VISION_APIFUNC VBaseMesh(VMeshType_e eMeshType=VMESH_BASEMESH);

  /// \brief
  ///   Constructor of the mesh class. Do not call (class is abstract).
  VISION_APIFUNC VBaseMesh(const char *szFileName, VMeshType_e eMeshType=VMESH_BASEMESH);

  VISION_APIFUNC ~VBaseMesh();

  /// \brief
  ///   Allocates the specified number of submeshes for a mesh.
  /// 
  /// Note that you will only have to call this function if you create static/dynamic meshes in
  /// code; it is not required to call this method for meshes loaded from a .vmesh/.model file.
  /// 
  /// \param iNumMeshes
  ///   Number of submeshes to allocate.
  VISION_APIFUNC virtual void AllocateSubmeshes(int iNumMeshes) = 0;

  /// \brief
  ///   Frees the submeshes allocated for a mesh.
  VISION_APIFUNC virtual void FreeSubmeshes() = 0;

  /// \brief
  ///   Allocates the specified number of geometry info entries for a mesh.
  /// 
  /// Note that you will only have to call this function if you create static/dynamic meshes in
  /// code; it is not required to call this method for meshes loaded from a file.
  /// 
  /// \param iNumGeomInfo
  ///   Number of geometry info entries to allocate.
  VISION_APIFUNC void AllocateGeometryInfo(int iNumGeomInfo);

  /// \brief
  ///   Frees the geometry info allocated for a mesh.
  VISION_APIFUNC void FreeGeometryInfo();
  
  ///
  /// @}
  ///

  ///
  /// @name Bounding Box Access
  /// @{
  ///

  /// \brief
  ///   Sets the bounding box for this mesh.
  /// 
  /// This bounding box is in local space. When an instance (entity/static mesh instance) of a mesh is created, the bounding box
  /// of this instance will be computed by taking the mesh bounding box into world space using the
  /// instance's transform.
  /// 
  /// Note that you will only have to call this function if you create mesh objects in
  /// code; it is not required to call this method for meshes loaded from a vmesh file.
  /// 
  /// \param box
  ///   New local space bounding box of the whole mesh.
  inline void SetBoundingBox(const hkvAlignedBBox &box) { m_BoundingBox = box; }

  /// \brief
  ///   Returns the bounding box for this mesh.
  inline const hkvAlignedBBox &GetBoundingBox() const { return m_BoundingBox; }

  ///
  /// @}
  ///

  ///
  /// @name Mesh Buffer Access
  /// @{
  ///

  /// \brief
  ///   Returns this mesh's VisMeshBuffer_cl object.
  /// 
  /// Internally, mesh rendering is built around the VisMeshBuffer_cl class. All render
  /// mesh data is stored in a VisMeshBuffer_cl object. Using this method, you can get direct access to
  /// the VisMeshBuffer_cl object (and, therefore, to the mesh data).
  /// 
  /// \return
  ///   VisMeshBuffer_cl *: Pointer to the VisMeshBuffer_cl object containing this mesh's vertex
  ///   and index data.
  inline VisMeshBuffer_cl *GetMeshBuffer() const
  { 
    return m_spMeshBuffer;
  }

  /// \brief
  ///   Sets this mesh's VisMeshBuffer_cl object.
  /// 
  /// Internally, mesh rendering is built around the VisMeshBuffer_cl class. All its render
  /// mesh data is stored in a VisMeshBuffer_cl object. 
  /// 
  /// Using this method, you can set the VisMeshBuffer_cl object. This is typically only useful for
  /// meshes which are created in runtime through code.
  /// 
  /// This function calls UpdateStreamMasks.
  /// 
  /// \param pMesh
  ///   Pointer to the VisMeshBuffer_cl object to set.
  inline void SetMeshBuffer(VisMeshBuffer_cl *pMesh)
  {
    m_spMeshBuffer = pMesh;
    UpdateStreamMasks();
  }

  /// \brief
  ///   This function must be called after the stream layout of the underlying mesh buffer has been
  ///   changed by user code.
  inline virtual void UpdateStreamMasks()
  {
    if (m_spMeshBuffer)
    {
      m_iMeshBufferStreamMask = m_spMeshBuffer->GetStreamMask();
    } 
    else
      m_iMeshBufferStreamMask = 0;
  }

  /// \brief
  ///   Internal function
  VISION_APIFUNC void EnsureMeshCreated();

  /// \brief
  ///   Returns the stream mask for this mesh.
  inline int GetMeshBufferStreamMask() const 
  {
    return m_iMeshBufferStreamMask;
  }
  
  /// \brief
  ///   Ensures that the mesh buffer can be read. This is necessary on platforms that can not read from single buffered vertex/index buffers.
  VISION_APIFUNC void EnsureReadableBuffer();

  /// \brief
  ///   Copies all or some of the mesh's vertices into a user-defined format.
  /// 
  /// The target format is specified through the passed VisMBVertexDescriptor_t. Note that the
  /// destination buffer has to be large enough to store all the resulting data (the size of the
  /// vertex format times the number of vertices).
  /// 
  /// Note that this function can be rather slow, especially on console versions of the Vision
  /// engine.
  /// 
  /// \param pDestBuffer
  ///   Destination buffer.
  /// 
  /// \param descr
  ///   Vertex descriptor defining the target format.
  /// 
  /// \param iFirstVertex
  ///   First vertex to copy (default is 0).
  /// 
  /// \param iVertexCount
  ///   Number of vertices to copy (default is -1). A value of -1 means that all remaining vertices
  ///   in the mesh are to be copied.
  /// 
  /// \return
  ///   int: Number of vertices copied.
  VISION_APIFUNC int CopyMeshVertices(void *pDestBuffer, const VisMBVertexDescriptor_t &descr, int iFirstVertex=0, int iVertexCount=-1) const;

  /// \brief
  ///   Returns the index type of the mesh (either VIS_INDEXFORMAT_32 or VIS_INDEXFORMAT_16).
  inline int GetMeshIndexType() const 
  {
    return m_spMeshBuffer ? m_spMeshBuffer->GetIndexType() : VIS_INDEXFORMAT_16; 
  }

  /// \brief
  ///    Copies a range of mesh indices into unsigned short (16 Bit) format, regardless of the
  ///    original format.
  /// 
  /// Note that this function can be rather slow, especially on console versions of the Vision
  /// engine.
  /// 
  /// \param pDestBuffer
  ///   Destination buffer.
  /// 
  /// \param iFirstIndex
  ///   First index to copy (default is 0).
  /// 
  /// \param iIndexCount
  ///   Number of indices to copy (default is -1). A value of -1 means that all remaining indices
  ///   in the mesh are to be copied.
  /// 
  /// \return
  ///   int: Number of indices copied.
  VISION_APIFUNC int CopyMeshIndices(unsigned short *pDestBuffer, int iFirstIndex=0, int iIndexCount=-1) const;

  /// \brief
  ///   Copies a range of mesh indices into unsigned int (32 Bit) format, regardless of the original format.
  /// 
  /// Note that this function can be rather slow, especially on console versions of the Vision
  /// engine.
  /// 
  /// \param pDestBuffer
  ///   Destination buffer.
  /// 
  /// \param iFirstIndex
  ///   First index to copy (default is 0).
  /// 
  /// \param iIndexCount
  ///   Number of indices to copy (default is -1). A value of -1 means that all remaining indices
  ///   in the mesh are to be copied.
  /// 
  /// \return
  ///   int: Number of indices copied.
  VISION_APIFUNC int CopyMeshIndices32(unsigned int *pDestBuffer, int iFirstIndex=0, int iIndexCount=-1) const;

  /// \brief
  ///    Copies all mesh indices belonging to a specific surface into unsigned short (16 Bit) format, regardless of the
  ///    original format.
  /// 
  /// Note that this function can be rather slow, especially on console versions of the Vision
  /// engine.
  /// 
  /// \param pDestBuffer
  ///   Destination buffer.
  /// 
  /// \param pSurface
  ///   If non-NULL, all mesh indices belonging to triangles referencing the specified surface will be copied.
  ///   If NULL, all mesh indices will be copied.
  /// 
  /// \return
  ///   int: Number of indices copied.
  VISION_APIFUNC int CopyMeshIndices(unsigned short *pDestBuffer, const VisSurface_cl *pSurface) const;

  /// \brief
  ///    Copies all mesh indices belonging to a specific surface into unsigned int (32 Bit) format, regardless of the
  ///    original format.
  /// 
  /// Note that this function can be rather slow, especially on console versions of the Vision
  /// engine.
  /// 
  /// \param pDestBuffer
  ///   Destination buffer.
  /// 
  /// \param pSurface
  ///   If non-NULL, all mesh indices belonging to triangles referencing the specified surface will be copied.
  ///   If NULL, all mesh indices will be copied.
  /// 
  /// \return
  ///   int: Number of indices copied.
  VISION_APIFUNC int CopyMeshIndices32(unsigned int *pDestBuffer, const VisSurface_cl *pSurface) const;

  /// \brief
  ///    Returns the number of mesh indices belonging to triangles which reference the specified surface. If pSurface is NULL, the total number of mesh indices for this mesh will be returned.
  VISION_APIFUNC int CountMeshIndices(const VisSurface_cl *pSurface=NULL) const;

  /// \brief
  ///    Internal function
  VISION_APIFUNC void WriteLightmapPrimitivesToBuffer(const hkvMat4& transform, VLightmapPrimitive::MeshVertex_t* pArray, int iIndexOfs, unsigned int *pIndices32, unsigned short *pIndices16);

  /// \brief
  ///    Returns whether the vertex/index data for the mesh will be double-buffered or not. Derived classes may set this according
  ///    to their specific requirements.
  VISION_APIFUNC virtual bool GetUseDoubleBuffering() const = 0;

  /// \brief
  ///   Returns true if this static mesh provides vertex streams that are required for lightmapping (UV1).
  VISION_APIFUNC bool IsLightmappingSupported() const;

  ///
  /// @}
  ///

  ///
  /// @name Submesh Access
  /// @{
  ///

  /// \brief
  ///   Returns the number of submeshes in this mesh.
  inline int GetSubmeshCount() const 
  { 
    return m_iNumSubmeshes; 
  }

  /// \brief
  ///   Returns the submesh with the specified index.
  VISION_APIFUNC virtual VBaseSubmesh *GetBaseSubmesh(int iMeshIndex) const = 0;

  /// \brief
  ///   Returns the Geometry Info specified by the index. (The index isn't the submesh index, instead a submesh can return a geometry info index). -1 returns the mesh's default geometry info for submeshes that don't have any info.
  VISION_APIFUNC VBaseGeometryInfo& GetGeometryInfo(int iGeometryInfoIndex);
  
  /// \brief
  ///   Returns the number of Geometry Info entries in the mesh
  inline int GetGeometryInfoCount() const
  { 
    return m_iGeometryInfoCount;
  }

  ///
  /// @}
  ///

  ///
  /// @name Resource Handling
  /// @{
  ///
#ifdef SUPPORTS_SNAPSHOT_CREATION

  /// \brief
  ///   Overridden function. See base class for details.
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  /// \brief
  ///   Called internally to update the memory consumption for this mesh
  VISION_APIFUNC VOVERRIDE void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);

  /// \brief
  ///   Unloads the mesh.
  VISION_APIFUNC VOVERRIDE BOOL Unload();

  /// \brief
  ///   Reloads the mesh.
  /// 
  /// Overridden function; If the mesh was originally loaded from a .vmesh/.model file, the file will be reloaded if
  /// the mesh is currently not loaded.
  /// 
  /// \return
  ///   BOOL: TRUE if the Reload was successful.
  VISION_APIFUNC VOVERRIDE BOOL Reload();

  /// \brief
  ///   Reloads the associated materials.xml file and reassigns the shaders in it
  VISION_APIFUNC BOOL ReloadShaderAssignmentFile(const char *szFilename);

  /// \brief
  ///   Enables/Disables the loading of materials from the materials.xml file for all meshes.
  ///   It is enabled by default.
  VISION_APIFUNC static void EnableLoadingMaterialsFromXML(bool enable);

  ///
  /// @}
  ///

  ///
  /// @name Surface Access
  /// @{
  ///

  /// \brief
  ///   Returns the number of surfaces referenced by the mesh. The array can be accessed via GetSurface(iIndex). Note that this is not the same indexing as for submeshes.
  inline int GetSurfaceCount() const 
  {
    return m_iNumSurfaces;
  }

  /// \brief
  ///   Returns a pointer to the mesh's material (surface) with the specified index. The index must be in valid range [0..GetSurfaceCount()-1]. The returned pointer already considers replacement materials from libraries
  inline VisSurface_cl* GetSurface(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<m_iNumSurfaces);
    return m_pSurfaceRefs[iIndex];
  }

  /// \brief
  ///   Returns a pointer to the array of the mesh's own material block. 
  ///
  /// The array holds GetSurfaceCount() elements. Not all of them are necessarily used for rendering as the relevant array is retrieved via GetSurfaceArray.
  /// To access a specific material, use GetSurface(iIndex)
  ///
  /// \returns
  ///    An array of GetSurfaceCount() instances of VisSurface_cl
  inline VisSurface_cl* GetOwnMaterialList() const
  {
    return m_pSurfaces;
  }

  /// \brief
  ///   Returns a pointer to the array of the materials that are used by the mesh.
  ///
  /// The array holds GetSurfaceCount() pointers to materials which are relevant for rendering. To access a specific material, use GetSurface(iIndex).
  /// The pointers in this array do not necessarily correspond to the list retrieved via GetOwnMaterialList, as surface pointers can point to external surfaces
  /// that are not part of the mesh (e.g. material library)
  ///
  /// \returns
  ///    An array of GetSurfaceCount() pointers to VisSurface_cl instances
  inline VisSurface_cl** GetSurfaceArray() const
  {
    return m_pSurfaceRefs;
  }

  /// \brief
  ///   Replaces a material (surface) in the mesh's palette of available materials. All submeshes that use this material entry will change accordingly.
  ///
  /// \param iSurfaceIndex
  ///   The material index to replace. Must be in valid range [0..GetSurfaceCount()-1]
  ///
  /// \param pSurface
  ///   The material pointer. Can be any material. This mesh does not take ownership of this surface (note VisSurface_cl is not reference counted). NULL can be passed to use the original material.
  VISION_APIFUNC void SetSurface(int iSurfaceIndex, VisSurface_cl *pSurface);

  /// \brief
  ///   Returns the 0-based surface index specified by szSurfaceName (case insensitive). If no matching surface is found, this function will return -1. This function uses the GetSurfaceArray function to access materials.
  VISION_APIFUNC int GetSurfaceIndexByName( const char* szSurfaceName ) const;

  /// \brief
  ///   Returns the surface specified by szSurfaceName (case insensitive). If no matching surface is found, this function will return NULL. This function uses the GetSurfaceArray function to access materials.
  inline VisSurface_cl* GetSurfaceByName( const char* szSurfaceName ) const
  {
    int iIndex = GetSurfaceIndexByName(szSurfaceName);
    return (iIndex<0) ? NULL : GetSurface(iIndex);
  }

  /// \brief
  ///   Returns the index (or -1) of the surface with the specified pointer. Can be used to determine whether the passed surface is used by this mesh. This function uses the GetSurfaceArray function to access materials.
  inline int GetSurfaceIndex(VisSurface_cl* pSurface)
  {
    for (int i = 0; i < m_iNumSurfaces; i++ )
    {
      if (m_pSurfaceRefs[i]==pSurface)
        return i;
    }
    return -1;
  }

  /// \brief
  ///   Allocates an array of iCount surfaces. These materials can be referenced by submeshes. This
  ///   function is typically only required when building custom meshes in code.
  VISION_APIFUNC void AllocateSurfaces(int iCount);


  /// \brief
  ///   Updates all surface-references to the most recent changes. Mostly useful in vForge during editing.
  ///
  /// \param bUpdateInstances
  /// Whether to update all instances of this base-mesh (mostly static mesh instances).
  /// Leave it to false during startup, to prevent excessive updates, but set it to true at runtime to make sure
  /// all instances are updated.
  ///
  /// If a Surface uses a Material override, that override will be copied into the meshes internal surface pointer cache.
  VISION_APIFUNC void UpdateSurfaceOverrides (bool bUpdateInstances);

  ///
  /// @}
  ///

  ///
  /// @name Trace Mesh / Collision Mesh Access
  /// @{
  ///

  /// \brief
  ///   Returns the trace mesh of this mesh.
  /// 
  /// Trace meshes and collision meshes differ insofar as collision meshes are the meshes the
  /// physics module operates on, while the Vision engine's internal TraceLine functionality (in
  /// the collision toolkit) operates on trace meshes. Collision meshes can be completely separate
  /// from the render geometry, while trace meshes are typically generated from the render
  /// geometry. For best performance, physics trace lines (against collision meshes) should be
  /// preferred; however, some features (e.g. getting UV positions for trace hits) are only
  /// possible with Vision trace lines.
  /// 
  /// Note that Vision collision toolkit trace lines call this method. Since it requires data being
  /// copied from the mesh buffer, this might have a significant performance and memory impact,
  /// especially on consoles. It is recommended to avoid these trace lines (and directly calling
  /// GetTraceMesh) altogether, or enable VIS_USE_COLLISIONMESHES_FOR_TRACELINES through
  /// Vision::Physics.SetStaticMeshTraceMeshBehavior(). This will result in collision meshes
  /// instead of trace meshes being used for the VisCollisionToolkit_cl's trace functionality,
  /// resulting in better memory/performance behavior, but without support for UV traces, and using
  /// the collision geometry rather than the render geometry.
  /// 
  /// \param bForceCreate
  ///   If true, the trace mesh will be created if it does not exist yet. If false, this function
  ///   may return NULL.
  /// 
  /// \param bForeTraceMesh
  ///   If true, this method will never return the collision geometry. Instead depend upon the first
  ///   param it will either return the trace mesh or NULL.
  /// 
  /// \return
  ///   IVCollisionMesh *: The trace mesh.
  VISION_APIFUNC IVCollisionMesh *GetTraceMesh(bool bForceCreate = true, bool bForeTraceMesh = false);

  /// \brief
  ///   Returns the collision mesh of the mesh.
  /// 
  /// Trace meshes and collision meshes differ insofar as collision meshes are the meshes the
  /// physics module operates on, while the Vision engine's internal TraceLine functionality (in
  /// the collision toolkit) operates on trace meshes. Collision meshes can be completely separate
  /// from the render geometry, while trace meshes are typically generated from the render
  /// geometry. For best performance, physics trace lines (against collision meshes) should be
  /// preferred; however, some features (e.g. getting UV positions for trace hits) are only
  /// possible with Vision trace lines.
  /// 
  /// When a collision mesh is requested through this method, the engine tries to load a fitting
  /// collision mesh (.vcolmesh) file from disk if the static mesh/ model itself was originally 
  /// loaded from disk. The .vcolmesh file is expected in the same directory as the .vmesh/ .model
  /// file. If the file can't be found and bUseTraceMeshIfLoadingFailed is set, the trace mesh will 
  /// be used as the collision mesh for this static mesh/ model; however, this may result in lower 
  /// performance and higher memory requirements.
  /// 
  /// \param bForceCreate
  ///   If true, the collision mesh will be created if it does not exist yet. If false, this
  ///   function may return NULL.
  /// 
  /// \param bUseTraceMeshIfLoadingFailed
  ///   If the file can't be found (or the mesh was generated through code) and this value is true,
  ///   the trace mesh will be used as the collision mesh for this mesh.
  /// 
  /// \return
  ///   IVCollisionMesh *: The collision mesh.
  VISION_APIFUNC IVCollisionMesh *GetCollisionMesh(bool bForceCreate = true, bool bUseTraceMeshIfLoadingFailed = false);


  /// \brief
  ///   Sets the collision mesh of the mesh manually.
  /// 
  /// \param pCollMesh
  ///   The collision mesh to use.
  inline void SetCollisionMesh(IVCollisionMesh *pCollMesh) 
  { 
    m_spCollisionMesh = pCollMesh;
  }

  /// \brief
  ///   Sets the trace mesh of the mesh manually.
  /// 
  /// \param pTraceMesh
  ///   The trace mesh to use.
  inline void SetTraceMesh(IVCollisionMesh *pTraceMesh) 
  { 
    m_spTraceMesh = pTraceMesh; 
  }

  /// \brief
  ///   Sets a new trace bitmask of all the submesh resources. This means that every new instance created with this resource receives the mask by default (but it does not change existing instances).
  VISION_APIFUNC void SetTraceBitmask(unsigned short iMask);

  /// \brief
  ///   Specifies the collision bitmask for the static mesh resource. This is used as the default value for newly created mesh instances
  inline void SetCollisionBitmask(unsigned int iCollisionMask) 
  { 
    m_iCollisionMask = iCollisionMask; 
  }

  /// \brief
  ///   Returns the collision bitmask for the static mesh resource. This is determined by the collision usage specfied at export time.
  inline unsigned int GetCollisionBitmask() const 
  { 
    return m_iCollisionMask;
  }
  
  ///
  /// @}
  ///

  ///
  /// @name Miscellaneous
  /// @{
  ///

  /// \brief
  ///   Saves the full material setup information to an XML file. Uses
  ///   name.model_data/materials.xml if the filename is not specified
  VISION_APIFUNC virtual bool SaveMaterialXML(const char *szFilename=NULL, VString *psErrorRes = NULL);

#ifdef SUPPORTS_SNAPSHOT_CREATION

  /// \brief
  ///   Saves a resources.xml file into the model's subfolder. This file can be loaded by the
  ///   VResourceSnapshot class
  VISION_APIFUNC virtual bool SaveResourceDependencyXML(VString *psErrorRes = NULL);

#endif

  // Compatibility:

  /// \brief
  ///   Indicates whether this mesh has at least one translucent surface
  /// 
  /// \return
  ///   true if the model has one or more translucent surfaces
  inline bool HasTranslucentSurfaces() const 
  { 
    return m_bContainsTranslucentSurfaces;
  }

  /// \brief
  ///   Indicates whether this mesh has at least one double-sided surface
  /// 
  /// \return
  ///   true if the model has one or more double-sided surfaces
  inline bool HasDoubleSidedSurfaces() const 
  { 
    return m_bContainsDoubleSidedSurfaces;
  }

  /// \brief
  ///   Returns whether this model has lit surfaces
  /// 
  /// \return
  ///   true if the model has lit (i.e. non-fullbright) surfaces
  inline bool HasLitSurfaces() const
  { 
    return m_bContainsLitSurfaces;
  }

  /// \brief
  ///   Updates internal information about the mesh's material data.
  /// 
  ///   This method has to be called whenever one of the mesh's materials (VisSurface_cl objects) has been modified.
  VISION_APIFUNC void UpdateMaterialFlags();

  /// \brief
  ///   Returns the number of vertices in the mesh.
  inline int GetNumOfVertices() const 
  { 
    return m_spMeshBuffer ? m_spMeshBuffer->GetVertexCount() : 0; 
  }

  /// \brief
  ///   Returns the number of triangles in the mesh.
  inline int GetNumOfTriangles() const 
  { 
    return m_spMeshBuffer ? m_spMeshBuffer->GetCurrentPrimitiveCount() : 0; 
  }

  ///
  /// @}
  ///

  ///
  /// @name Mesh Type and Storage Mode
  /// @{
  ///

  /// \brief
  ///   Returns the type of mesh. 
  ///
  /// May either be VMESH_STATICMESH or VMESH_DYNAMICMESH for Vision's standard mesh types. VMESH_BASEMESH should be used for custom mesh types.
  ///
  /// \returns
  ///  The mesh type of the mesh (VMESH_STATICMESH, VMESH_DYNAMICMESH, or VMESH_BASEMESH).
  inline VMeshType_e GetMeshType() const 
  { 
    return m_eMeshType; 
  }

  /// \brief
  ///   Returns the storage mode to be used by the mesh.
  ///
  /// The storage mode is used internally to provide derived classes with control of how the VisMeshBuffer_cl object containing the mesh's
  /// vertex and index information stores its data. V_MBSTORAGE_FROMFILE lets the mesh file (.vmesh/.model) decide whether the data
  /// is single-buffered or double-buffered (depending on what was stored as part of the VisMeshBuffer_cl serialization).
  /// V_MBSTORAGE_FORCESINGLEBUFFER forces the data to be single-buffered and V_MBSTORAGE_FORCEDOUBLEBUFFER forces it to
  /// be double-buffered, regardless of what is stored in the file.
  ///
  /// This method will only be used for meshes loaded from a file.
  ///
  /// \returns
  ///   The storage mode.
  VISION_APIFUNC virtual VMeshBufferStorageMode_e GetStorageMode() const;

  ///
  /// @}
  ///

  ///
  /// @name Shader assignment control
  /// @{
  ///

  /// \brief
  ///   Specifies whether it is allowed to assign shaders to this mesh. Assigning shaders is typically disabled for special objects like the vForge Gizmo.
  inline void SetAllowShaderAssignment(bool bStatus)
  {
    m_bAllowShaderAssignment = bStatus;
  }

  /// \brief
  ///   Returns whether it is allowed to assign shaders to this mesh. Assigning shaders is typically disabled for special objects like the vForge Gizmo.
  inline bool GetAllowShaderAssignment() const
  {
    return m_bAllowShaderAssignment;
  }

  /// \brief
  ///   Set this flag to hide this model in the vForge material editor
  inline void SetSupportMaterialEditing(bool bStatus)
  {
    m_bSupportMaterialEditing = bStatus;
  }

  /// \brief
  ///   Returns the state that has been set via SetSupportMaterialEditing. The default is true
  inline bool GetSupportMaterialEditing() const
  {
    return m_bSupportMaterialEditing;
  }

  ///
  /// @}
  ///

protected:
  // Protected functions
  VISION_APIFUNC void Init();

  VISION_APIFUNC void UpdateSurfaceReferencesInInstances() const;

  friend class VMeshManager;
  friend class VBaseMeshLoader;
  VISION_APIFUNC virtual bool LoadFromFile(const char *szFilename) = 0; 

  // Protected member variables
  int m_iNumSubmeshes;
  VisMeshBufferPtr m_spMeshBuffer;
  int m_iMeshBufferStreamMask; ///< streams defined in the mesh buffer

  static bool s_bTryLoadXML;
  bool m_bLoadFromFile, m_bTryLoadXML, m_bAllowShaderAssignment, m_bSupportMaterialEditing;

  hkvAlignedBBox m_BoundingBox;

  int m_iNumSurfaces;
  VisSurface_cl *m_pSurfaces;
  VisSurface_cl **m_pSurfaceRefs;

  bool m_bContainsTranslucentSurfaces;
  bool m_bContainsDoubleSidedSurfaces;
  bool m_bContainsLitSurfaces;
  bool m_bContainsClipDistance;

  IVCollisionMeshPtr m_spCollisionMesh;
  IVCollisionMeshPtr m_spTraceMesh;
  unsigned int m_iCollisionMask;
  
  int m_iGeometryInfoCount;
  VBaseGeometryInfo* m_pGeometryInfo;       //array of Geometry Info
  VBaseGeometryInfo m_DefaultGeometryInfo;  //local data instance for models that don't have geometry info (or submeshes with index -1)
  
  VMeshType_e m_eMeshType;
};


/// \brief
///   Class representing a subset of a mesh (VBaseMesh).
/// 
/// Submeshes are a part of a mesh. They only have a single material ("surface") and a single
/// lightmap (if any). Each submesh has to be renderable with a single graphics API rendering call and may
/// only reference a range of up to 32767 vertices in the static mesh instance.
/// 
/// Submeshes don't have their own vertex and index information, but reference the render mesh /
/// shadow mesh data in the VBaseMesh they are part of.
/// 
/// Writing data to submeshes is typically only required when creating mesh objects manually.
class VBaseSubmesh
{
public:

  /// \brief
  ///   Destructor of the base submesh class.
  virtual ~VBaseSubmesh();

  /// \brief
  ///   Sets the bounding box for this submesh. Only required when building submeshes via code
  inline void SetBoundingBox(const hkvAlignedBBox &box) 
  { 
    m_BoundingBox = box; 
  }

  /// \brief
  ///   Returns the bounding box for this submesh.
  inline const hkvAlignedBBox &GetBoundingBox() const 
  { 
    return m_BoundingBox; 
  }

  /// \brief
  ///   Specifies the render index range in the mesh's index buffer (first index and number
  ///   of indices). For non-indexed primitive types (MB_PRIMTYPE_TRILIST or MB_PRIMTYPE_TRISTRIP) these numbers are measured in vertex counts
  inline void SetRenderRange(int iStart, int iNumIndices) 
  { 
    m_iRenderFirstIndex=iStart;
    m_iRenderNumIndices=iNumIndices; 
  }

  /// \brief
  ///   Returns the render index range in the mesh's index buffer (first index and number of indices).
  inline void GetRenderRange(int &iStart, int &iNumIndices) const 
  { 
    iStart=m_iRenderFirstIndex;
    iNumIndices=m_iRenderNumIndices;
  }

  /// \brief
  ///   Specifies the render vertex range in the mesh's vertex buffer (first vertex and number of vertices relevant for the submesh).
  inline void SetRenderVertexRange(int iStart, int iNumVertices) 
  { 
    m_iRenderFirstVertex=iStart;
    m_iRenderNumVertices=iNumVertices;
  }

  /// \brief
  ///   Returns the render vertex range in the mesh's vertex buffer (first vertex and number of vertices relevant for the submesh).
  inline void GetRenderVertexRange(int &iStart, int &iNumVertices) const 
  { 
    iStart=m_iRenderFirstVertex;
    iNumVertices=m_iRenderNumVertices;
  }

  /// \brief
  ///   Associates a submesh with a material (surface) so that every instance of this submesh is rendered with this material setup.
  ///
  /// To replace a material inside the mesh's material palette, rather use function VBaseMesh::SetSurface
  ///
  /// \param pSurface
  ///   The surface to set
  ///
  /// \param iMaterialIndex
  ///   The index of the passed surface within the material table of the owner mesh. The index must be in valid range [0..pMesh->GetSurfaceCount()-1]
  inline void SetSurface(VisSurface_cl *pSurface, int iMaterialIndex) 
  { 
    m_pSurface = pSurface;
    m_iMaterialIndex = iMaterialIndex;
  }

  /// \brief
  ///   Returns the submesh's material (surface).
  inline VisSurface_cl *GetSurface() const 
  { 
    return m_pSurface; 
  }

  /// \brief
  ///   Returns the index of the material in the owner mesh's material list
  inline int GetSurfaceIndex() const 
  {
    return m_iMaterialIndex;
  }

  /// \brief
  ///   Returns the mesh the submesh is part of.
  inline VBaseMesh *GetBaseRenderMesh() const 
  { 
    return m_pMesh; 
  }

  /// \brief
  ///   Sets the mesh the submesh is part of.
  inline void SetMesh(VBaseMesh *pMesh) 
  { 
    m_pMesh=pMesh; 
  }

  /// \brief
  ///   Returns the geometry info index of this submesh. Can be -1 to define the default geometry info
  inline int GetGeometryInfoIndex() const
  { 
    return m_iGeometryInfoIndex; 
  }

  /// \brief
  ///   Sets the geometry info index of this submesh. Can be -1 for default geometry info
  inline void SetGeometryInfoIndex(int iIndex) 
  { 
    VASSERT(m_pMesh);
    VASSERT(iIndex<m_pMesh->GetGeometryInfoCount()); 
    m_iGeometryInfoIndex = iIndex; 
  }
  
  /// \brief
  ///   Returns the geometry info of this submesh
  inline VBaseGeometryInfo& GetGeometryInfo() 
  { 
    VASSERT(m_pMesh);
    return m_pMesh->GetGeometryInfo(m_iGeometryInfoIndex); 
  } 


protected:
  VBaseSubmesh();


public:

  hkvAlignedBBox m_BoundingBox;
  VisSurface_cl *m_pSurface;

  int m_iRenderFirstIndex;
  int m_iRenderNumIndices;

  int m_iRenderFirstVertex;
  int m_iRenderNumVertices;

  VBaseMesh *m_pMesh;
  short m_iGeometryInfoIndex;
  short m_iMaterialIndex;

};


/// \brief
///   Internal loader class for loading .vmesh files.
class VBaseMeshLoader : public VChunkFile
{
public:
  VISION_APIFUNC VBaseMeshLoader(VBaseMesh *pBaseMesh);
  VISION_APIFUNC virtual ~VBaseMeshLoader();

  VOVERRIDE void OnStartLoading();
  VOVERRIDE void OnFinishLoading();

  VOVERRIDE BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen);
  VOVERRIDE void OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs);
  VOVERRIDE void OnWarning(const char *szWarning, CHUNKIDTYPE chunkID, int iChunkOfs);

  /// \brief
  ///   Static callback that is triggered when a new data chunk is found while loading a mesh, before it is parsed.
  ///
  /// Applications can register to this callback to hook into a mesh loading and parse user defined chunks.
  /// The data object can be casted to class VMeshChunkDataObject which provides the chunk file and the chunk ID.
  /// Only custom chunks should be processed by the user, otherwise the mesh will be broken. The user should
  /// ignore all other chunks, which will be processed by the engine.
  VISION_APIDATA static VisCallback_cl OnMeshChunkSerialization;

  bool m_bAlreadyLoading;
  int m_iVersion;
  VBaseMesh *m_pBaseMesh;

  char m_szPath[FS_MAX_PATH]; ///< path for prepending texture loading
  class VModelInfoXMLDocument *m_pInfoDoc;
};

/// \brief
///   Callback data object class that is triggered by the static VBaseMeshLoader::OnMeshChunkSerialization callback
class VMeshChunkDataObject : public IVisCallbackDataObject_cl
{
public:
  inline VMeshChunkDataObject(VBaseMeshLoader &loader, CHUNKIDTYPE chunkID)
    : IVisCallbackDataObject_cl(&VBaseMeshLoader::OnMeshChunkSerialization),
    m_Loader(loader)
  {
    m_iChunkID = chunkID;
    m_bProcessed = false;
    m_bSuccess = true;
  }

  VBaseMeshLoader &m_Loader; ///< The mesh loader that triggered the callback. This is the chunk file with the current chunk open
  CHUNKIDTYPE m_iChunkID;    ///< The chunk ID. May or may not be a custom chunk
  bool m_bSuccess;           ///< Set this value to false to indicate loading failed
  bool m_bProcessed;         ///< Only process the chunk if this member is false, otherwise it might have been processed by another listener
};

/// \brief
///   Resource Manager for meshes
class VMeshManager : public VisResourceManager_cl
{
public:

  /// \brief
  ///   Internal: Constructor of the mesh manager.
  VMeshManager(const char *szManagerName,int iFlags, int iMemLimit) : VisResourceManager_cl(szManagerName,iFlags,iMemLimit) {}

  /// \brief
  ///   Method to load a static mesh from a .vmesh file.
  VISION_APIFUNC VisStaticMesh_cl *LoadStaticMeshFile(const char *szFilename); 

  /// \brief
  ///   Method to load a dynamic mesh from a .model file.
  VISION_APIFUNC VDynamicMesh *LoadDynamicMeshFile(const char *szFilename);

  /// \brief
  ///   Method to find a mesh file of specified type in the manager. Similar to VResourceManager::GetResourceByName but also checks the type
  VISION_APIFUNC VBaseMesh *FindMeshFile(const char *szFilename, VMeshType_e eType);

  /// \brief
  ///   Returns a pointer to the global static mesh manager
  VISION_APIFUNC static VMeshManager* GetMeshManager();

  /// \brief
  ///   Merges a list of existing (dynamic) meshes into a new one
  /// 
  /// The merged mesh can be used for entities. Since the animation system can perform skeletal remapping, the original animation sequences can
  /// be applied to the new mesh. This method is thus very useful to bake the equipment parts (several entities) into a new mesh to reduce the number
  /// of entities in the scene. However, this optimization sacrifices memory, as the new mesh consists of unique geometry.
  /// The new number of vertices and triangles matches the sum of all subparts. No merging of vertices takes place 
  /// (merging of skeletons and materials is optional and defined by the respective MeshMergeInfo_t members).
  /// Merged meshes cannot be saved to file by the runtime. Quite naturally, unloading the resource is not supported.
  /// This function outputs a strict 1:1 relation between surfaces and submeshes, even when the input models have more submeshes per material. All
  /// original submeshes that belong to merged surfaces will be merged as well. This implies that this function does not split submeshes according to primitive counts,
  /// so the new primitive count might exceed the supported draw call sizes on specific platforms (e.g. 32k triangles).
  ///
  /// \param szNewName
  ///   The new name of the mesh to appear it in the resource manager. Must be !=NULL, but does not necessarily have to be unique name
  /// 
  /// \param pInfoList
  ///   A pointer to an array of info structures that hold information about meshes to merge. Each entry in the list represents one mesh to bake
  /// 
  /// \param iInfoCount
  ///   Number of array entries in pInfoList.
  /// 
  /// \param pReferenceSkeleton
  ///   An optional reference skeleton. If not specified, the first skeleton of a mesh in the list is used.
  /// 
  /// \param pLog
  ///   Optional log that receives notifications about errors
  /// 
  /// \return
  ///   VisModel_cl *pNewMesh : A new mesh instance baked form the sources. Can be NULL if an error has occurred (which is reported to pLog instance).
  VISION_APIFUNC VDynamicMesh* MergeDynamicMeshes(const char *szNewName, MeshMergeInfo_t *pInfoList, int iInfoCount, VisSkeleton_cl *pReferenceSkeleton = NULL, IVLog *pLog = NULL);

protected:

  VOVERRIDE VManagedResource *LoadResource(const char *szFilename) { VASSERT_MSG(FALSE, "VMeshmanager::LoadResource not supported."); return NULL; }
  VISION_APIFUNC VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);
};



#endif // DEFINE_VISAPIBASEMESH

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
