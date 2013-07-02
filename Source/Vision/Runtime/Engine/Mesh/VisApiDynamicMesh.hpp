/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiDynamicMesh.hpp

#ifndef DEFINE_VISAPIDYNAMICMESH
#define DEFINE_VISAPIDYNAMICMESH

#include <Vision/Runtime/Engine/Mesh/VisApiBaseMesh.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimSequenceSet.hpp>

class VDynamicSubmesh;
class VDynamicMesh;
typedef VSmartPtr<VDynamicMesh> VDynamicMeshPtr;

class VMeshManager;

#ifdef SUPPORTS_HARDWARE_SKINNING
  #define VDYNAMICMESH_HAS_GPU_BONEWEIGHT_BUFFER
#endif

/// \brief Maximum number of index/weight pairs in a VVertexBoneReference_t
#define MAX_BONEREFS_PER_VERTEX 4

/// \brief
///   Data structure containing the bone references (indices) and the respective weights for a single vertex
struct VVertexBoneReference_t
{
  short iBoneIndex[MAX_BONEREFS_PER_VERTEX];          ///< Bone index in the skeleton
  float fBoneWeight[MAX_BONEREFS_PER_VERTEX];         ///< Bone weight (between 0 and 1)
};

#ifndef _VISION_DOC

struct VCSSkinningConfigStruct_t
{
  int iSrcOffsets[3];
  int iSrcStride;
};


struct SurfaceSubmeshRef_t
{
  inline SurfaceSubmeshRef_t() {m_iFirstSMRef=m_iSMRefCount=0;}
  unsigned short m_iFirstSMRef, m_iSMRefCount;
};

#endif


/// \brief
///   Derived class for loading a VDynamicMesh resource 
class VModelSerializationProxy : public IVSerializationProxy
{
public:
  inline VModelSerializationProxy(VDynamicMesh *pDynamicMesh) {VASSERT(pDynamicMesh); m_pDynamicMesh=pDynamicMesh;}
  VOVERRIDE IVSerializationProxyCreator *GetInstance();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

protected:
  // serialization
  inline VModelSerializationProxy() {m_pDynamicMesh=NULL;}
  V_DECLARE_SERIAL_DLLEXP( VModelSerializationProxy, VISION_APIDATA )
  VOVERRIDE void Serialize( VArchive &ar );
private:
  VDynamicMesh *m_pDynamicMesh;
};




/// \brief
///   Mesh class that can be used for dynamic, animated entities
///
/// Dynamic meshes inherit from class VBaseMesh and add skeletal animation capabilities.
/// Dynamic meshes are typically referenced by entities, but they can also be rendered through low level rendering functions.
///
/// \sa VisBaseEntity_cl::SetMesh
/// \sa VisRenderLoopHelper_cl::RenderModelWithShader
class VDynamicMesh : public VBaseMesh
{
public:

  ///
  /// @name Initialization/De-initialization
  /// @{
  ///

  /// \brief
  ///   Constructor; do not use. Create dynamic meshes using the VDynamicMeshBuilder or LoadDynamicMesh instead.
  VISION_APIFUNC VDynamicMesh();

  /// \brief
  ///   Constructor; do not use. Create dynamic meshes using the VDynamicMeshBuilder or LoadDynamicMesh instead.
  VISION_APIFUNC VDynamicMesh(const char *szFileName);

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VDynamicMesh();

  /// \brief
  ///   Internal allocation function; do not use
  VISION_APIFUNC VOVERRIDE void AllocateSubmeshes(int iNumMeshes);

  /// \brief
  ///   Internal de-allocation function; do not use
  VISION_APIFUNC VOVERRIDE void FreeSubmeshes();

  ///
  /// @}
  ///


  ///
  /// @name Submesh Access
  /// @{
  ///

  /// \brief
  ///   Returns the submesh resource with the specified index.
  VISION_APIFUNC VOVERRIDE VBaseSubmesh *GetBaseSubmesh(int iMeshIndex) const;

  /// \brief
  ///   Returns the submesh resource with the specified index.
  inline VDynamicSubmesh *GetSubmesh(int iMeshIndex) const;

  /// \brief
  ///   Returns a list of submesh indices that reference the surface with the specified surface index.
  ///
  /// Since multiple submeshes can reference the same material it is sometimes required to retrieve all submeshes that reference a material.
  /// This function provides a fast way to retrieve this (precalculated) list.
  /// The requested surface is passed as an index to cover also cases where specific surfaces might be replaced by external ones (material library)
  ///
  /// \param iSurfaceIndex
  ///   The requested surface index - must be in valid range [0..GetSurfaceCount()-1]
  ///
  /// \param iDestCount
  ///   Receives the number of list entries
  ///
  /// \return
  ///   An array of indices. The length of the array will be written to iDestCount. Each value in the array specifies a submesh index that can be passed to GetSubmesh
  inline unsigned short *GetSurfaceSubmeshIndices(int iSurfaceIndex, int &iDestCount) const
  {
    VASSERT(iSurfaceIndex>=0 && iSurfaceIndex<GetSurfaceCount() && m_piSurfaceSubmeshRefList!=NULL);
    const SurfaceSubmeshRef_t &ref(m_pSurfaceSubmeshRefs[iSurfaceIndex]);
    iDestCount = ref.m_iSMRefCount;
    return &m_piSurfaceSubmeshRefList[ref.m_iFirstSMRef];
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

  /// \brief
  ///   Obsolete function. Please use GetDependencies directly instead
  VISION_APIFUNC bool SaveResourceDependencyXML(VString *psErrorRes);
#endif

  /// \brief
  ///   Called internally to update the memory consumption for this mesh
  VISION_APIFUNC VOVERRIDE void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);

  /// \brief
  ///   Overridden resource function. Use EnsureUnloaded instead
  VISION_APIFUNC BOOL Unload();

  /// \brief
  ///   Overridden resource function to load the mesh from file. Use EnsureLoaded instead
  VISION_APIFUNC BOOL Reload();

  /// \brief
  ///   Static function to load a dynamic mesh file.
  /// 
  /// This method first looks up a resource in the mesh resource manager and returns that instance if a mesh
  /// with the same file name is available (see VDynamicMesh::FindDynamicMesh). Otherwise it loads a new resource
  /// from file. If loading fails (e.g. because the file is not available), this function returns the mesh
  /// resource of the "MissingModel.model" file
  /// 
  /// \param szFilename
  ///   The filename of the mesh file
  /// 
  /// \return 
  ///   A new or existing dynamic mesh resource
  VISION_APIFUNC static VDynamicMesh *LoadDynamicMesh(const char *szFilename);

  /// \brief
  ///   Static function that looks up a mesh resource by filename match in the resource manager. If not found, NULL is returned.
  VISION_APIFUNC static VDynamicMesh *FindDynamicMesh(const char *szFilename);

  /// \brief
  ///   Returns the global resource manager responsible for handling static meshes.
  VISION_APIFUNC static VMeshManager &GetResourceManager();

  /// \brief
  ///   Static function to load a dynamic mesh file.
  /// 
  /// This method first looks up a resource in the mesh resource manager and returns that instance if a mesh
  /// with the same file name is available (see VDynamicMesh::FindDynamicMesh). Otherwise it loads a new resource
  /// from file. If loading fails (e.g. because the file is not available), this function returns the mesh
  /// resource of the "MissingModel.model" file
  /// 
  /// \param pInStream
  ///   The input stream to load the file from
  /// 
  /// \param szName
  ///   The name of the mesh
  /// 
  /// \return 
  ///   A new or existing dynamic mesh resource
  VISION_APIFUNC static VDynamicMesh *LoadDynamicMesh(IVFileInStream& pInStream, const char *szName);

  ///
  /// @}
  ///

  ///
  /// @name Skeleton and Bone Handling
  /// @{
  ///

  /// \brief
  ///   Gets the skeleton instance that is associated with this model.
  /// 
  /// \return
  ///   VisSkeleton_cl *pSkeleton : The skeleton instance. NULL for non-skeletally animated models
  /// 
  /// \sa VisSkeleton_cl
  inline VisSkeleton_cl* GetSkeleton() const
  {
    return m_spSkeleton;
  }

  /// \brief
  ///   Sets a skeleton instance on this model. Do not use this function
  inline void SetSkeleton(VisSkeleton_cl *pSkeleton)
  {
    m_spSkeleton = pSkeleton;
  }

  /// \brief
  ///   Finds the animation sequence by name and type
  /// 
  /// This function loops through all sequence sets that are attached to this dynamic mesh. By default a dynamic mesh
  /// file loads the sequence set that is the .anim counterpart of the model filename.
  /// 
  /// To globally find a loaded animation sequence, the dedicated resource manager can be used. See
  /// VisAnimManager_cl::GetSequenceSetManager().
  /// 
  /// \param szSequenceName
  ///   Name of the sequence.
  ///
  /// \param eType
  ///   The animation type (either VIS_MODELANIM_SKELETAL or VIS_MODELANIM_VERTEX).
  /// 
  /// \return
  ///   VisAnimSequence_cl* pSeq: found sequence or NULL.
  inline VisAnimSequence_cl* GetSequence(const char *szSequenceName, VisModelAnimType_e eType = VIS_MODELANIM_SKELETAL)
  {
    return m_SequenceSetCollection.GetSequence(szSequenceName, eType);
  }

  /// \brief
  ///   Access the collection of animation sequence sets
  /// 
  /// This collection can be used to add or remove sequence sets to this model. See
  /// VisAnimSequenceSetCollection_cl::AddSequenceSet and
  /// VisAnimSequenceSetCollection_cl::RemoveSequenceSet.
  /// 
  /// A single set of animations (i.e. one instance of VisAnimSequenceSet_cl) corresponds to a
  /// .anim file which contains an arbitrary number of skeletal- and/or vertex animations.
  /// 
  /// By default an animated model contains one instance in the collection, that is the .anim file
  /// that matches the .model filename. 
  /// 
  /// To play an animation sequence, the sequence does not necessarily have to be contained in any
  /// set attached to this model; instead, playing animations works also directly with pointers to
  /// sequences.
  /// 
  /// However, most animation helper functions, such as the static
  /// VisAnimConfig_cl::StartSkeletalAnimation function, work with animation names. To find the
  /// sequence by name for an entity mesh, the helper functions only search in the attached
  /// collection.
  /// 
  /// To access all sequences being loaded, the dedicated resource manager can be used. See
  /// VisAnimManager_cl::GetSequenceSetManager().
  /// 
  /// \return
  ///   VisAnimSequenceSetCollection_cl *pSets : The collection of sets attached to this model
  /// 
  /// \sa VisAnimSequenceSet_cl
  /// \sa VisAnimSequenceSetCollection_cl
  /// \sa VisAnimManager_cl
  inline VisAnimSequenceSetCollection_cl* GetSequenceSetCollection()
  {
    return &m_SequenceSetCollection;
  }

  /// \brief
  ///   Internal function to associate all vertices with new bone references.
  ///
  /// \param pBoneReferences
  ///   New array to set. This array must hold vertex count elements. Each element represents the bone weightings for the respective vertex
  ///   The array can be NULL to free the data. Otherwise the array is copied by this function
  VISION_APIFUNC void SetBoneReferences(const VVertexBoneReference_t *pBoneReferences);

  /// \brief
  ///   Internal function to ensure that the array of bone references is allocated. Does not need to be called
  VISION_APIFUNC VVertexBoneReference_t* CreateBoneReferences();

  /// \brief
  ///   Returns a pointer to the bone weighting array. Each element in the array represents the bone weighting for the respective vertex
  inline const VVertexBoneReference_t *GetBoneReferencePtr() { return m_pBoneReferences; }


  ///
  /// @}
  ///

  
  /// @name Surface Handling
  //@{

  /// \brief
  ///   Internal function that copies the data of the surface \c srcSurface into the surface of this mesh
  ///   at position \c iDestIndex.
  /// \param iDestIndex
  ///   index of the surface in this mesh to receive the copied data
  /// \param srcSurface
  ///   the surface to copy data from
  VISION_APIFUNC void CopySurfaceFrom(int iDestIndex, VisSurface_cl const& srcSurface);

  //@}


  ///
  /// @name Bounding Volumes
  /// @{
  ///

  /// \brief
  ///   Sets a collision bounding box for this mesh resource. By default the box is provided by the mesh file so this setter should not be used
  ///
  /// \param boundingBox
  ///   New bounding box in mesh local space
  inline void SetCollisionBoundingBox(const hkvAlignedBBox &boundingBox) 
  { 
    m_CollisionBoundingBox = boundingBox; 
  }

  /// \brief
  ///   Returns the collision bounding box for this mesh resource. This bounding box can be used as default for specific physics implementations
  inline const hkvAlignedBBox &GetCollisionBoundingBox() const 
  { 
    return m_CollisionBoundingBox; 
  }

  /// \brief
  ///   Obsolete overload for GetCollisionBoundingBox. Should not be used anymore.
  inline bool GetCollisionBoundingBox(hkvAlignedBBox &bbox) const 
  { 
    bbox = m_CollisionBoundingBox; return true; 
  }

  /// \brief
  ///   Sets the visibility sphere radius for this mesh resource. By default the radius is provided by the mesh file so this setter should not be used
  inline void SetBoundingSphereRadius(float fRadius) 
  { 
    m_fBoundingSphereRadius = fRadius; 
  }

  /// \brief
  ///   Returns the visibility sphere radius for this mesh resource. This radius might be useful for visibility calculations.
  inline float GetBoundingSphereRadius() const 
  { 
    return m_fBoundingSphereRadius; 
  }

  /// \brief
  ///   Sets a visibility bounding box for this mesh resource. By default the box is provided by the mesh file so this setter should not be used
  inline void SetVisibilityBoundingBox(const hkvAlignedBBox& bbox) 
  { 
    VASSERT(bbox.isValid());
    m_BoundingBox = bbox; 
  }

  /// \brief
  ///   Returns a tight visibility bounding box around the model
  inline const hkvAlignedBBox& GetVisibilityBoundingBox() const 
  { 
    return m_BoundingBox; 
  }

  /// \brief
  ///   Obsolete overload for GetVisibilityBoundingBox. Should not be used anymore.
  inline bool GetVisibilityBoundingBox(hkvAlignedBBox &bbox) const 
  { 
    bbox = m_BoundingBox; 
    return true; 
  }


  ///
  /// @}
  ///

  ///
  /// @name Rendering-related
  /// @{
  ///

  /// \brief
  ///   Overridden function. Uses the global VisRenderer_cl::GetForceSingleBufferedModels() status
  VISION_APIFUNC VOVERRIDE bool GetUseDoubleBuffering() const; 

  /// \brief
  ///   Returns a bitmask that indicates which vertex streams are provided for this mesh. The bits
  ///   are constants of type VERTEX_STREAM_xyz.
  /// 
  /// Supported bit constants are:  
  ///   \li VERTEX_STREAM_POSITION: always provided
  ///   \li VERTEX_STREAM_NORMAL: always provided
  ///   \li VERTEX_STREAM_COLOR: vertex colors. Exported but by default not used by the default shaders
  ///   \li VERTEX_STREAM_TEX0: always provided (base texture coordinates)
  ///   \li VERTEX_STREAM_TEX1: pre-baked lightmap coordinates (exporter option), 
  ///   \li VERTEX_STREAM_TEX2: always provided (tangent vector)
  /// 
  /// \return
  ///   int iSupportedStreamMask : bitmask that specifies which streams are supported
  VISION_APIFUNC int GetSupportedVertexStreams() const;

  ///
  /// @}
  ///



  ///
  /// @name Backwards-compatibility Functions for CPU-side data access
  /// @{
  ///

  /// \brief
  ///   Provides the pointer to original vertex positions.
  /// 
  /// The pointer reference will be set to the first element. For the next element, increase the
  /// pointer by iStride (=return value) bytes.
  /// 
  /// Thus the position of the first vertex is
  /// (pVertexPosition[0],pVertexPosition[1],pVertexPosition[2]).
  /// This function can only be used when the mesh data is double buffered, which is default on PC platforms.
  /// 
  /// \param pVertexPosition
  ///   pointer reference that will receive the address of the first vertex position
  /// 
  /// \return
  ///   int iStride: Stride (in bytes) between the vertices, -1 if there is no position information in the vertex.
  VISION_APIFUNC int GetVertexPosition(float* &pVertexPosition) const;

  /// \brief
  ///   Allows to check if the dynamic mesh has position information in the vertex description.
  /// 
  /// \return
  ///   true if the mesh data contains position information, false otherwise.
  inline bool HasVertexPosition() const
  {
    float* pTemp = NULL;
    return GetVertexPosition(pTemp) != -1;
  }

  /// \brief
  ///   Provides the pointer to vertex texture coordinates of specified stream index.
  /// 
  /// The pointer reference will be set to the first element. For the next element, increase the
  /// pointer by iStride (=return value) bytes.
  /// Thus the UV of the first vertex is (pVertexTexCoord[0],pVertexTexCoord[1]).
  /// This function can only be used when the mesh data is double buffered, which is default on PC platforms.
  /// 
  /// \param iTexCoordIndex
  ///   Index of the texture coordinate slot. 0 is the diffuse texture, 1 the lightmap UV etc.
  ///
  /// \param pVertexTexCoord
  ///   pointer reference that will receive the address of the first texture coordinate (float[2])
  /// 
  /// \return
  ///   int iStride: Stride (in bytes) between the vertices, or -1 if the mesh does not support this stream
  VISION_APIFUNC int GetVertexTexCoord(int iTexCoordIndex, float* &pVertexTexCoord) const;

  /// \brief
  ///   Allows to check if the dynamic mesh has normal information in the vertex description.
  /// 
  /// \param iTexCoordIndex
  ///   Index of the texture coordinate slot. 0 is the diffuse texture, 1 the lightmap UV etc.
  ///
  /// \return
  ///   true if the mesh data contains normal information, false otherwise.
  inline bool HasVertexTexCoord(int iTexCoordIndex) const
  {
    float* pTemp = NULL;
    return GetVertexTexCoord(iTexCoordIndex, pTemp) != -1;
  }

  /// \brief
  ///   Provides the pointer to the array of normal vectors
  /// 
  /// The pointer reference will be set to the first element. For the next element, increase the
  /// pointer by iStride (=return value) bytes.
  /// Thus the Normal of the first vertex is (pVertexNormal[0],pVertexNormal[1],pVertexNormal[2]).
  /// This function can only be used when the mesh data is double buffered, which is default on PC platforms.
  /// 
  /// \param pVertexNormal
  ///   pointer reference that will receive the address of the first normal vector (float[3])
  /// 
  /// \return
  ///   int iStride: Stride (in bytes) between the vertices
  VISION_APIFUNC int GetVertexNormal(float* &pVertexNormal) const;

  /// \brief
  ///   Allows to check if the dynamic mesh has normal information in the vertex description.
  /// 
  /// \return
  ///   true if the mesh data contains normal information, false otherwise.
  inline bool HasVertexNormal() const
  {
    float* pTemp = NULL;
    return GetVertexNormal(pTemp) != -1;
  }

  /// \brief
  ///   Provides the pointer to the array of tangent vectors vectors
  /// 
  /// The pointer reference will be set to the first element. For the next element, increase the
  /// pointer by iStride (=return value) bytes.
  /// Thus the Tangent of the first vertex is
  /// (pVertexTangent[0],pVertexTangent[1],pVertexTangent[2]).
  /// This function can only be used when the mesh data is double buffered, which is default on PC platforms.
  /// 
  /// \param pVertexTangent
  ///   pointer reference that will receive the address of the first tangent vector (float[3])
  /// 
  /// \return
  ///   int iStride: Stride (in bytes) between the vertices, -1 if there is no vertex tangent
  VISION_APIFUNC int GetVertexTangent(float* &pVertexTangent) const;

  /// \brief
  ///   Allows to check if the dynamic mesh has tangent information in the vertex description.
  /// 
  /// \return
  ///   true if the mesh data contains tangent information, false otherwise.
  inline bool HasVertexTangent() const
  {
    float* pTemp = NULL;
    return GetVertexTangent(pTemp) != -1;
  }

  /// \brief
  ///   Provides the pointer to the array of vertex bone indices
  /// 
  /// The pointer reference will be set to the first element. For the next element, increase the
  /// pointer by iStride (=return value) bytes.
  /// 
  /// Thus the first vertex is influenced by the following bone indices:
  /// pVertexBoneIndices[0],pVertexBoneIndices[1],pVertexBoneIndices[2], pVertexBoneIndices[3]. Unused
  /// bones have the weighting -1. 
  /// 
  /// \param pVertexBoneIndices
  ///   pointer reference that will receive the address of the first index (short)
  /// 
  /// \return
  ///   int iStride: Stride (in bytes) between the vertices
  VISION_APIFUNC int GetVertexBoneIndices(short* &pVertexBoneIndices) const;


  /// \brief
  ///   Provides the pointer to the array of vertex bone weights
  /// 
  /// The pointer reference will be set to the first element. For the next element, increase the
  /// pointer by iStride (=return value) bytes.
  /// 
  /// Thus the first vertex is influenced by the bones with the following weightings:
  /// pVertexBoneWeights[0],pVertexBoneWeights[1],pVertexBoneWeights[2], pVertexBoneWeights[3]. Unused
  /// bones have the weighting -1. 
  /// 
  /// \param pVertexBoneWeights
  ///   pointer reference that will receive the address of the first weight (float)
  /// 
  /// \return
  ///   int iStride: Stride (in bytes) between the vertices
  VISION_APIFUNC int GetVertexBoneWeights(float* &pVertexBoneWeights) const;

  /// \brief
  ///   Provides the pointer to the array of triangle vertex indices
  /// 
  /// The pointer reference will be set to the first element. For the next element, increase the
  /// pointer by iStride (=return value) bytes.
  /// 
  /// Each triangle is defined by 3 subsequent vertex indices, thus the first triangle uses vertex
  /// indices (pVertexIndices[0],pVertexIndices[1],pVertexIndices[2]).
  /// This function can only be used when the mesh data is double buffered, which is default on PC platforms.
  /// 
  /// \param pVertexIndices
  ///   pointer reference that will receive the address of the first index triples (short[3])
  ///
  /// \param iIndexType
  ///   reference that receives the index type which is VIS_INDEXFORMAT_16 or VIS_INDEXFORMAT_32
  /// 
  /// \return
  ///   int iStride: Stride (in bytes) between the indices. For iIndexType==VIS_INDEXFORMAT_16 this is 2 and
  ///   for VIS_INDEXFORMAT_32 this is 4
  VISION_APIFUNC int GetVertexIndices(void* &pVertexIndices, int &iIndexType) const;


  /// \brief
  ///   Gets the Shader Set for this model.
  /// 
  /// A shader set can either be assigned to the model using the SetShaderSet method, or by loading
  /// a shader assignment file.
  /// 
  /// By default, all entities that use this model also share this shader set.
  /// 
  /// \return
  ///   VisShaderSet_cl *pSet: The Shader Set assigned to this model
  inline VisShaderSet_cl *GetShaderSet() const
  {
    return m_spShaderSet;
  }


  /// \brief
  ///   Sets the Shader Set for this model
  /// 
  /// This will directly influence entities that share this shader set (default).
  /// 
  /// \param pSet
  ///   VisShaderSet_cl *pSet: The Shader Set to assign to this model
  inline void SetShaderSet(VisShaderSet_cl *pSet)
  {
    m_spShaderSet = pSet;
  }


  /// \brief
  ///   Internal function
  VISION_APIFUNC void UpdateShaderSet();

  /// \brief
  ///   Internal function that returns the pointer to the raw vertex data in system memory, in case it is double buffered
  VISION_APIFUNC void *GetMeshBufferPtr() const;

  static inline void SetMaxLoadedBoneCount(int newBoneCount) { s_iMaxLoadedBoneCount = newBoneCount; }
  static inline int GetMaxLoadedBoneCount() { return s_iMaxLoadedBoneCount; }
  static inline int GetMaxLoadedVertexCount() { return s_iMaxLoadedVertexCount; }

#ifdef VDYNAMICMESH_HAS_GPU_BONEWEIGHT_BUFFER
  void FillBoneWeightStreams();
  void EnsureBoneWeightBufferFilled();
  inline VisMeshBuffer_cl *GetBoneWeightBuffer() { return m_spBoneWeightBuffer; }
#else
  inline VisMeshBuffer_cl *GetBoneWeightBuffer() { return 0; }
#endif

  VOVERRIDE IVSerializationProxy *CreateProxy() {return new VModelSerializationProxy(this);}


  #if defined(_VR_DX11)
    VShaderConstantBuffer *GetSkinningConstantBuffer(); 
    void ReleaseSkinnngConstantBuffer();
    VShaderConstantBuffer *m_pSkinningConstantBuffer;
    VOVERRIDE void UpdateStreamMasks();
  #endif

  #ifdef _VR_DX11
    VShaderConstantBuffer *GetCSSkinningConfigBuffer() const { return (VShaderConstantBuffer*)&m_SkinningConfig; }
    void UpdateSkinningConfig();
    VTypedConstantBuffer <VCSSkinningConfigStruct_t> m_SkinningConfig;
  #endif

	VISION_APIFUNC void CreateBoneWeightStreams();
	VISION_APIFUNC void FinishLoading();

protected:
  friend class VDynamicMeshLoader;
  friend class VMeshManager;
  friend class VDynamicMeshBuilder;
  friend class VisRenderLoopHelper_cl;

  // Internal functions
  VISION_APIFUNC void Init();

  VISION_APIFUNC virtual bool LoadFromFile(const char *szFilename) HKV_OVERRIDE; 
  VISION_APIFUNC virtual bool LoadFromStream(IVFileInStream& rInStream, const char *szName); 
  
  void CreateShaderSet(unsigned int iCapacity);

  // === Mesh-related Data ===
  VDynamicSubmesh *m_pSubmeshes;
  unsigned short *m_piSurfaceSubmeshRefList;
  SurfaceSubmeshRef_t *m_pSurfaceSubmeshRefs;

  // === Skeleton and Animation-related Data ===
  VisSkeletonPtr m_spSkeleton;
  VisAnimSequenceSetCollection_cl m_SequenceSetCollection;
  VisAnimSequenceSetCollection_cl m_SavedAnimSequences; ///< Collection that holds animation sequences that were present when the resource was unloaded.
  VVertexBoneReference_t *m_pBoneReferences;

  // === Bounding Volume Data ===
  hkvAlignedBBox m_CollisionBoundingBox;         ///< Collision Bounding Box. May differ from m_BoundingBox member declared in base class (used for visibility determination)
  float m_fBoundingSphereRadius;                    ///< Bounding Sphere Radius

  // === Rendering-related Data ===
  VisShaderSetPtr m_spShaderSet;
  #ifdef VDYNAMICMESH_HAS_GPU_BONEWEIGHT_BUFFER
    VisMeshBufferPtr m_spBoneWeightBuffer;
  #endif

  // === Static members ===
  // keep max bone and vertex count for all models that were loaded as static class variable
  static VISION_APIDATA int s_iMaxLoadedBoneCount;
  static VISION_APIDATA int s_iMaxLoadedVertexCount;

};

/// \brief
///   Class representing a subset of a dynamic mesh (VDynamicMesh).
/// 
/// Submeshes are a subset of a mesh. They only have a single material ("surface"). 
/// Each dynamic submesh may only reference a range of up to 32767 vertices in the mesh.
/// 
/// VDynamicSubmesh objects can not be created directly, but are always created through the functions
/// provided by the VDynamicMesh class.
/// 
/// Dynamic submeshes don't have their own vertex and index information, but reference the render mesh /
/// shadow mesh data in the VDynamicMesh they are part of.
/// 
/// Writing data to submeshes is typically only required when creating VDynamicMesh objects
/// manually.
class VDynamicSubmesh : public VBaseSubmesh
{
public:

  /// \brief
  ///   Virtual destructor
  virtual ~VDynamicSubmesh();

  /// \brief
  ///   Returns the dynamic mesh the submesh is part of.
  inline VDynamicMesh *GetMesh() const { return (VDynamicMesh *)m_pMesh; }

protected:
  friend class VDynamicMesh;

  VDynamicSubmesh();
};



/// \brief
///   Internal loader class for loading .vmesh files.
class VDynamicMeshLoader : public VBaseMeshLoader
{
public:
  VISION_APIFUNC VDynamicMeshLoader(VDynamicMesh *pGroup);

  VOVERRIDE void OnStartLoading();
  VOVERRIDE void OnFinishLoading();

  VOVERRIDE BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen);
  VOVERRIDE void OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs);
  VOVERRIDE void OnWarning(const char *szWarning, CHUNKIDTYPE chunkID, int iChunkOfs);

  //char m_szPath[FS_MAX_PATH]; ///< path for prepending texture loading
  class VModelInfoXMLDocument *m_pInfoDoc;
};






inline VDynamicSubmesh *VDynamicMesh::GetSubmesh(int iMeshIndex) const
{ 
  VASSERT (iMeshIndex>=0 && iMeshIndex < m_iNumSubmeshes); 
  return &(m_pSubmeshes[iMeshIndex]); 
}


/// \brief
///   Helper class to build dynamic meshes from scratch.
///
/// Create an instance of this class for each mesh to build. The counts of vertices,
/// indices, surfaces and bones (if applicable) must be specified at creation time.
///
/// Then, for each vertex, add the vertex first and then any bone weights for that
/// vertex. Triangles and surfaces can be specified any time. When using multiple
/// surfaces, group the vertices and indices by surface, and call \c NextSubmesh()
/// before adding the vertices and triangles of the next surface.
///
/// Note that when using VDynamicMeshBuilder, the number of surfaces in a mesh has to
/// correspond to the number of submeshes. 
///
/// When finished, call \c Finalize() to retrieve the generated dynamic mesh.
class VDynamicMeshBuilder
{
public:
  /// \brief Vertex data structure used by entity's mesh buffer
  struct Vertex_t
  {
    float pos[3];
    float normal[3];
    float tangent[3];
    DWORD color;
    float tex0[2];
  };

public:
  /// \brief
  ///   Initializes a new VDynamicMeshBuilder.
  /// \param iNumVertices
  ///   the number of vertices to allocate
  /// \param iNumTriangles
  ///   the number of triangles to allocate
  /// \param iNumBones
  ///   the number of bones to allocate, if the entity is to be animated. May be \c 0.
  /// \param iNumSurfaces
  ///   the number of surfaces to allocate
  /// \param iUsageFlag
  ///   Can either be VIS_MEMUSAGE_STATIC, VIS_MEMUSAGE_STATIC_UNMANAGED, VIS_MEMUSAGE_STREAM, or
  ///   VIS_MEMUSAGE_DYNAMIC. In general, you should use STATIC for vertex data that is specified
  ///   only once and then never changed. STREAM should be used for vertex data that is modified or
  ///   re-specified in run-time, but used for rendering more often than it is updated. DYNAMIC is
  ///   the best solution for data that has about a 1:1 update to render ratio. The difference
  ///   between VIS_MEMUSAGE_STATIC and VIS_MEMUSAGE_STATIC_UNMANAGED is that an unmanaged buffer
  ///   will never be double-buffered by the graphics API. It may therefore consume less memory on
  ///   some platforms with separate memory pools, but you will have to take care of lost device
  ///   handling etc. yourself. It is generally recommended NOT to use
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED.
  ///   In addition to one of these basic settings (which can't be combined), you can specify 
  ///   VIS_MEMUSAGE_UAV_BYTEADDRESS or VIS_MEMUSAGE_UAV_STRUCTURED to determine the way this buffer
  ///   is addressed when using it as an unordered access resource in a compute shader DX11.
  ///
  /// \param iAdditionalVertexBindFlags
  ///   In DX10/DX11 you can specify additional bind flags for the vertex buffer as follows:
  ///   \li VIS_BIND_STREAM_OUTPUT: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a target for stream output operations.
  ///   \li VIS_BIND_SHADER_RESOURCE: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a shader resource (e.g. in order to perform random-access reads from the
  ///       buffer in a vertex or pixel shader). This can only be used in conjunction with VIS_MEMUSAGE_STATIC, not
  ///       with VIS_MEMUSAGE_STREAM or VIS_MEMUSAGE_DYNAMIC.
  ///   \li VIS_BIND_UNORDERED_ACCESS: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as an unordered access resource for compute shaders.
  ///
  /// \param iAdditionalIndexBindFlags
  ///   In DX10/DX11 you can specify additional bind flags for the index buffer as follows:
  ///   \li VIS_BIND_STREAM_OUTPUT: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a target for stream output operations.
  ///   \li VIS_BIND_SHADER_RESOURCE: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a shader resource (e.g. in order to perform random-access reads from the
  ///       buffer in a vertex or pixel shader). This can only be used in conjunction with VIS_MEMUSAGE_STATIC, not
  ///       with VIS_MEMUSAGE_STREAM or VIS_MEMUSAGE_DYNAMIC.
  ///   \li VIS_BIND_UNORDERED_ACCESS: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as an unordered access resource for compute shaders.
  VISION_APIFUNC VDynamicMeshBuilder(unsigned int iNumVertices, 
    unsigned int iNumTriangles, unsigned int iNumBones, 
    unsigned int iNumSurfaces, int iUsageFlag=VIS_MEMUSAGE_STREAM, 
    int iAdditionalVertexBindFlags=0, int iAdditionalIndexBindFlags=0);
private:
  VDynamicMeshBuilder();
  VDynamicMeshBuilder(VDynamicMeshBuilder const&);
  VDynamicMeshBuilder& operator=(VDynamicMeshBuilder const&);
public:
  VISION_APIFUNC ~VDynamicMeshBuilder();

public:
  /// \brief
  ///   Adds a vertex to the entity's mesh buffer.
  /// \param vPosition
  ///   position of the vertex
  /// \param vNormal
  ///   normal vector
  /// \param vTangent
  ///   tangent vector
  /// \param vTexCoords
  ///   texture offset (u,v)
  /// \param color
  ///   color of the vertex
  /// \return
  ///   the index of the vertex just added
  VISION_APIFUNC int AddVertex(const hkvVec3& vPosition, 
    const hkvVec3& vNormal, const hkvVec3& vTangent, 
    const hkvVec2& vTexCoords, const VColorRef& color);

  /// \brief
  ///   Adds a bone weight to the most recently added vertex.
  /// \param iIndex
  ///   index of the bone to add a weight for
  /// \param fWeight
  ///   weight of the specified bone. Must be > 0.
  /// \note
  ///   Only call this method after a vertex has been added.
  VISION_APIFUNC void AddBoneWeight(short iIndex, float fWeight);

  /// \brief
  ///   Adds a triangle to the entity.
  /// \param iIndex1
  ///   the first vertex index
  /// \param iIndex2
  ///   the second vertex index
  /// \param iIndex3
  ///   the third vertex index
  /// \note
  ///   The vertices specified by the indices may be added before or after
  ///   adding the triangle.
  VISION_APIFUNC void AddTriangle(unsigned short iIndex1, 
    unsigned short iIndex2, unsigned short iIndex3);

  /// \brief
  ///   Copies the settings of the specified surface into a surface allocated for
  ///   the entity being built.
  /// \param iDestIndex
  ///   index of the target surface in this VDynamicMeshBuilder
  /// \param srcSurface
  ///   the surface to copy data from
  VISION_APIFUNC void CopySurfaceFrom(unsigned int iDestIndex, VisSurface_cl const& srcSurface);

  /// \brief
  ///   Finalizes the mesh building process and returns the constructed mesh.
  /// \return
  ///   the constructed mesh
  /// \note
  ///   No data can be added or modified after the mesh has been finalized.
  VISION_APIFUNC VDynamicMeshPtr const& Finalize();

  /// \brief
  ///   Returns the index of the first index set by the next call to \c AddTriangle().
  /// \return
  ///   the index of the next index
  VISION_APIFUNC int GetNextIndexIndex() const { return m_iNextIndex; }

  /// \brief
  ///   Returns the index of the next vertex to be set by \c AddVertex().
  /// \return
  ///   the index of the next vertex
  VISION_APIFUNC int GetNextVertexIndex() const { return m_iNextVertex; }

  /// \brief
  ///   Finalizes the current submesh, and starts a new one.
  ///
  /// When creating the VDynamicMeshBuilder, a number of submeshes equal to the number
  /// of surfaces is allocated. The last submesh will be finalized when the dynamic mesh
  /// is finalizes, thus it is not necessary to call \c NextSubmesh() when there is only
  /// one surface.
  VISION_APIFUNC void NextSubmesh();

private:
  static unsigned int s_iMeshCounter; ///< The total number of meshes generated; used to create unique mesh names

  bool m_bFinalized; ///< Whether the dynamic mesh has been finalized

  VDynamicMeshPtr m_spMesh; ///< The dynamic mesh being built
  VisMeshBufferPtr m_spMeshBuffer; ///< The mesh buffer used by the dynamic mesh

  unsigned int m_iNextVertex; ///< The next vertex being set by \c AddVertex()
  Vertex_t *m_pVertices; ///< Pointer to the vertex buffer to be filled

  unsigned int m_iNextIndex; ///< The next index being set by \c AddTriangle()
  unsigned short *m_pIndices; ///< Pointer to the index buffer to be filled

  unsigned int m_iCurrentSubmesh; ///< The index of the current submesh
  unsigned int m_iSubmeshStartVertex; ///< The vertex count at which the current submesh starts
  unsigned int m_iSubmeshStartIndex; ///< The index count at which the current submesh starts

  VVertexBoneReference_t *m_pBoneRefs; ///< The bone reference buffer to be filled

  hkvAlignedBBox m_BoundingBox; ///< The bounding box of the dynamic mesh; updated when vertices are added
};

#endif // DEFINE_VISAPIDYNAMICMESH

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
