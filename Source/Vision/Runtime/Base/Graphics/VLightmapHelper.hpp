/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLightmapHelper.hpp

#ifndef _VLIGHTMAPHELPER_HPP_INCLUDED
#define _VLIGHTMAPHELPER_HPP_INCLUDED


#define PRIMITIVEFLAG_CASTSHADOWS         V_BIT(0)
#define PRIMITIVEFLAG_RECEIVESHADOWS      V_BIT(1)
#define PRIMITIVEFLAG_FULLPAGE            V_BIT(2)
#define PRIMITIVEFLAG_DEFAULT             (PRIMITIVEFLAG_CASTSHADOWS|PRIMITIVEFLAG_RECEIVESHADOWS)
#define PRIMITIVEFLAG_ALL                 (PRIMITIVEFLAG_CASTSHADOWS|PRIMITIVEFLAG_RECEIVESHADOWS|PRIMITIVEFLAG_FULLPAGE)

class VLightmapSceneInfo;
class VLightmapPrimitive;
class VisZoneResource_cl; ///< from engine
typedef VSmartPtr<VLightmapPrimitive> VLightmapPrimitivePtr;


/// \brief
///   Single geometry primitive that gets supplied as occluder/receiver triangle mesh to vLux
/// 
/// See class VLightmapSceneInfo.
class VLightmapPrimitive : public VRefCounter
{
public:

  /// \brief
  ///   Enum for the geometry type of the primitive
  /// 
  /// Can be MESH (3 vertices per triangle) or INDEXED_MESH. If type INDEXED_MESH is used as a
  /// lightmap receiver, it has to provide pre-defined lightmap UV coordinates, because vLux cannot
  /// unwrap indexed meshes itself (since it needs to create unique lightmap coordinates).
  enum PrimitiveType_e
  {
    MESH = 0,         ///< vertex buffer (triangle list)
    INDEXED_MESH = 1  ///< indexed triangle list
  };


  /// \brief
  ///   Enum that categorizes the geometry provider
  /// 
  /// Use OWNERTYPE_FIRSTCUSTOMTYPE (or larger values) to identify custom geometry. Type
  /// OWNERTYPE_OBJECT3D is reserved for classes derived from VisObject3D_cl, as these classes can
  /// use their unique ID for identification.
  enum OwnerType_e
  {
    OWNERTYPE_OBJECT3D = 0,         ///< should be used for all VisObject3D_cl instances
    OWNERTYPE_STATICMESH = 1,       ///< used for static meshes
    OWNERTYPE_FIRSTCUSTOMTYPE = 16  ///< should be used for custom object types (default)
  };

  /// \brief
  ///   Mesh vertex structure, as used in VLightmapPrimitive::AllocateVertices()
  struct MeshVertex_t
  {
    VBASE_IMPEXP static void GetDescriptor(VisMBVertexDescriptor_t &desc);
    hkvVec3 vPos;       ///< Position of the vertex in world space
    hkvVec3 vNormal;    ///< World space unit length normal vector
    hkvVec3 vTangent;   ///< World space unit length tangent vector
    hkvVec3 vBiTangent; ///< World space unit length bi-tangent vector
    hkvVec2 vBaseUV;    ///< Diffuse base texture coordinates. Used for tracing transparent materials
  };

  /// \brief
  ///   Structure that describes a single material inside a VLightmapPrimitive.
  /// 
  /// If the primitive type is INDEXED_MESH, the materials have index lists.
  struct MeshMaterial_t
  {
    MeshMaterial_t() : 
      m_RenderState(VIS_TRANSP_NONE, RENDERSTATEFLAG_FRONTFACE), 
      m_vUVOffset(0.f,0.f), m_vUVScaling(1.f,1.f)
    {
      m_iUserData = 0;
      m_fGranularity = -1.f;
      m_AmbientColor.SetRGBA(0,0,0,0);
      m_iLightInfluenceBitmask = 0xffffffff;
      m_iIndexCount = 0;
      m_pIndices = NULL;
      m_pIndices32 = NULL;
      m_iPageIndex = 0;
      m_iWantedLightmapSize[0] = m_iWantedLightmapSize[1] = 0;
      m_iLightmapMergeID = 0;
      m_pSharedUVMaterial = NULL;
      m_pInternal = NULL;
      m_iOverrideFlags = -1;
      m_iNumLightMaskEntries = 0;
      m_pLightMaskEntryIndex = NULL;
    }

    ~MeshMaterial_t()
    {
      V_SAFE_DELETE_ARRAY(m_pLightMaskEntryIndex);
      V_SAFE_DELETE_ARRAY(m_pIndices);
      V_SAFE_DELETE_ARRAY(m_pIndices32);
    }

    ///
    /// @name Index List
    /// @{
    ///


    /// \brief
    ///   Allocate the index list for this material (triangle count *3 indices). The owner
    ///   primitive type must be INDEXED_MESH
    VBASE_IMPEXP void AllocateIndices(int iIndexCount);

    /// \brief
    ///   Allocate a 32bit index list for this material (triangle count *3 indices). The owner
    ///   primitive type must be INDEXED_MESH
    VBASE_IMPEXP void AllocateIndices32(int iIndexCount);

    /// \brief
    ///   Indicates whether this material has an index list (16 or 32 bit)
    inline bool HasIndices() const {return m_iIndexCount>0;}

    /// \brief
    ///   Returns the number of indices allocated by this material
    inline int GetIndexCount() const {return m_iIndexCount;}

    /// \brief
    ///   Returns a pointer to the index list
    inline unsigned short *GetIndexList() const {return m_pIndices;}

    /// \brief
    ///   Returns a pointer to the 32 bit index list
    inline unsigned int *GetIndexList32() const {return m_pIndices32;}

    /// \brief
    ///   Returns a 32 bit index regardless of source type
    inline int GetIndexAt(int i) const 
    {
      VASSERT(i>=0 && i<m_iIndexCount);
      if (m_pIndices) return m_pIndices[i];
      VASSERT(m_pIndices32);
      return m_pIndices32[i];
    }

    ///
    /// @}
    ///

    ///
    /// @name Lightmap Page
    /// @{
    ///


    /// \brief
    ///   Returns the 0-based index of the resulting lightmap page. See VLightmapSceneInfo::GetLightmapTexture for information on how to access textures
    int GetPageIndex() const {return m_iPageIndex;}


    inline void SetLightMaskIndexList(int iCount, const int *pList)
    {
      V_SAFE_DELETE_ARRAY(m_pLightMaskEntryIndex);
      m_iNumLightMaskEntries = iCount;
      if (iCount>0)
      {
        m_pLightMaskEntryIndex = new int[iCount];
        if (pList)
          memcpy(m_pLightMaskEntryIndex,pList,sizeof(int)*iCount);
        else
          memset(m_pLightMaskEntryIndex,0,sizeof(int)*iCount);
      }
    }
    inline int GetNumLightMaskEntries() const {return m_iNumLightMaskEntries;}
    inline int GetLightMaskEntryIndex(int i) const {VASSERT(i>=0 && i<m_iNumLightMaskEntries); return m_pLightMaskEntryIndex[i];}
    inline void SetLightMaskEntryIndex(int i, int iVal) {VASSERT(i>=0 && i<m_iNumLightMaskEntries);m_pLightMaskEntryIndex[i]=iVal;}

    inline void SetCustomRadiosityShader(const char *szLibFile, const char *szEffectName, const char *szParam);
    inline bool GetCustomRadiosityShader(VString &sLibFile, VString &sEffectName, VString &sParam) const;

    ///
    /// @}
    ///

    ///
    /// @name Public Member Variables
    /// @{
    ///

    VString m_sMaterialName;            ///< Used for displaying error messages, can be left empty
    unsigned int m_iUserData;           ///< Custom user data, gets serialized
    VString m_sBaseTexture;             ///< Diffuse base texture filename (project-relative) for transparency and radiosity rendering
    VSimpleRenderState_t m_RenderState; ///< Transparency and doublesidedness
    float m_fGranularity;               ///< Receivers only: The granularity value for unwrapping receiver geometry. Primitives with pre-defined UV use it as a tiling factor
    VColorRef m_AmbientColor;           ///< Receivers only: ambient color of the material
    unsigned int m_iLightInfluenceBitmask; ///< receiver only: light filter bitmask
    short m_iWantedLightmapSize[2];     ///< If !=0 this is an alternative way to specify the granularity
    int m_iLightmapMergeID;             ///< Materials with same ID share the same quad on the page. By default, every material has distinct ID
    int m_iOverrideFlags;               ///< if not -1, this overrides the primitive's m_iFlags
    VString m_sCustomRadiosityShader;   ///< Can be used to specify a custom shader for radiosity

    int m_iIndexCount;          ///< internal use; 
    unsigned short *m_pIndices; ///< internal use
    unsigned int *m_pIndices32; ///< internal use
    int m_iNumLightMaskEntries;
    int *m_pLightMaskEntryIndex; ///< in the scene's list

    // output related
    int m_iPageIndex; ///< internal use
    hkvVec2 m_vUVOffset, m_vUVScaling; ///< for primitives with pre-defined lightmap UV, these two vectors specify the 2D scaling and offset transformation from pre-defined UV to lightmap UV
    
    MeshMaterial_t *m_pSharedUVMaterial; ///< internal variable, used by vLux
    void *m_pInternal; ///< do not use

    // internal function
    VBASE_IMPEXP void SerializeMaterialInformation(VChunkFile &file, bool bForOutput, bool bPredefinedUV, int iVersion=-1);
    ///
    /// @}
    ///

  };


  /// \brief
  ///   Structure that holds information about a single lightmask resource (lightmap page/light pair)
  struct VLightMaskEntry_t
  {
  public:
    /// \brief
    ///   Constructor
    VLightMaskEntry_t() : m_vUVScale(1.f,1.f), m_vUVOfs(0.f,0.f)
    {
      m_iLightUniqueID = 0;
      m_pPrimitive = NULL;
      m_pMaterial = NULL;
    }

    /// \brief
    ///   Assignment operator
    inline VLightMaskEntry_t& operator = (const VLightMaskEntry_t &other)
    {
      m_iLightUniqueID = other.m_iLightUniqueID;
      m_pPrimitive = other.m_pPrimitive;
      m_pMaterial = other.m_pMaterial;
      m_vUVScale = other.m_vUVScale;
      m_vUVOfs = other.m_vUVOfs;
      m_sLightMaskFilename = other.m_sLightMaskFilename;
      return *this;
    }

    /// \brief
    ///   Internal IO
    VBASE_IMPEXP void SerializeInformation(VLightmapSceneInfo &info, int iVersion=-1);

    __int64 m_iLightUniqueID;         ///< Unique ID of the light source
    VLightmapPrimitive *m_pPrimitive; ///< Owner primitive
    MeshMaterial_t *m_pMaterial;      ///< Owner material
    hkvVec2 m_vUVScale, m_vUVOfs;   ///< Transformation relative to light map coordinates
    VString m_sLightMaskFilename;     ///< Filename of the texture resource
  };


  /// \brief
  ///   Constructor
  VBASE_IMPEXP VLightmapPrimitive();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VLightmapPrimitive();

  ///
  /// @name Access Geometry/materials
  /// @{
  ///

  /// \brief
  ///   Allocates iMaterialCount materials for this mesh.
  /// 
  /// Each material is of type MeshMaterial_t and defines sub-properties.
  /// 
  /// Primitives of type MESH should have exactly one material, wheres primitives of type
  /// INDEXED_MESH can have more materials. Indexed meshes have a index set per material.
  ///
  /// \param iMaterialCount
  ///   Number of materials to allocate
  VBASE_IMPEXP void AllocateMaterials(int iMaterialCount);

  /// \brief
  ///   Returns the number of allocated materials.
  int GetMaterialCount() const {return m_iMaterialCount;}

  /// \brief
  ///   Returns the material by index.
  inline MeshMaterial_t& GetMaterial(int iIndex) const 
  {
    VASSERT(iIndex>=0 && iIndex<m_iMaterialCount);
    return m_pMaterials[iIndex];
  }

  /// \brief
  ///   Returns the material by index.
  inline int GetMaterialIndexByUserData(unsigned int iUserData) const 
  {
    for (int i=0;i<m_iMaterialCount;i++)
      if (m_pMaterials[i].m_iUserData==iUserData)
        return i;
    return -1;
  }

  /// \brief
  ///   Returns the 0-based index of the passed material in this primitive.
  inline int GetMaterialIndex(const MeshMaterial_t *pMat) const
  {
    int iMat = (int)(pMat-m_pMaterials);
    VASSERT(&GetMaterial(iMat) == pMat);
    return iMat;
  }

  /// \brief
  ///   Tests whether this primitive contains the material. For debug purposes only
  inline bool ContainsMaterial(const MeshMaterial_t &mat) const
  {
    for (int i=0;i<m_iMaterialCount;i++)
      if (&m_pMaterials[i] == &mat)
        return true;
    return false;
  }

  /// \brief
  ///   Internal function
  inline MeshMaterial_t *GetSharedUVMaterial(int iMaterialIndex) const
  {
    MeshMaterial_t &mat = GetMaterial(iMaterialIndex);
    if (mat.m_iLightmapMergeID==-1) ///< never share
      return NULL;
    for (int i=0;i<iMaterialIndex;i++)
    {
      MeshMaterial_t &other(GetMaterial(i));
      if (i==iMaterialIndex || other.m_iLightmapMergeID!=mat.m_iLightmapMergeID)
        continue;
      if ((other.m_iOverrideFlags&PRIMITIVEFLAG_RECEIVESHADOWS)==0) // this might be a submesh with collapsed UV - don't consider it
        continue; // also works for m_iOverrideFlags==-1
      return &other;
    }
    return NULL;
  }

  /// \brief
  ///   Allocates iVertexCount vertices for this mesh. Each vertex is of type MeshVertex_t.
  VBASE_IMPEXP void AllocateVertices(int iVertexCount);

  /// \brief
  ///   Returns the number of allocated vertices.
  int GetVertexCount() const {return m_iVertexCount;}

  /// \brief
  ///   Returns the array of vertices
  inline MeshVertex_t *GetVertices() const {return m_pVertex;}

  /// \brief
  ///   Return the number of allocated materials

  ///
  /// @}
  ///

  ///
  /// @name Result Uv / Predefined Uv
  /// @{
  ///

  /// \brief
  ///   Allocates predefined lightmap UV coordinates.
  /// 
  /// This has to be done for primitives of type INDEXED_MESH that are used as shadow receiver.
  /// 
  /// If iCount==-1, the current vertex count is used.
  ///
  ///\param iCount
  ///   Number of UV pairs. Pass -1 to use the vertex count
  VBASE_IMPEXP void AllocateUV(int iCount=-1);

  /// \brief
  ///   Returns the array lightmap UV coordinates. The size of the array matches the vertex count
  inline hkvVec2* GetLightmapUV() {return m_pOutUV;}

  /// \brief
  ///   Indicates whether this primitive uses a predefined set of lightmap coordinates
  inline bool HasPreDefinedLightmapUV() const {return m_eType==INDEXED_MESH;}

  ///
  /// @}
  ///

  ///
  /// @name Lightmap Flags
  /// @{
  ///

  /// \brief
  ///   Indicates whether this primitive is used as a shadow caster (PRIMITIVEFLAG_CASTSHADOWS
  ///   flag)
  inline bool IsShadowCaster() const {return (m_iFlags&PRIMITIVEFLAG_CASTSHADOWS)>0;}

  /// \brief
  ///   Indicates whether this primitive is used as a shadow receiver (PRIMITIVEFLAG_RECEIVESHADOWS
  ///   flag)
  inline bool IsShadowReceiver() const {return (m_iFlags&PRIMITIVEFLAG_RECEIVESHADOWS)>0;}

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   Internal function: (De-)serializes the mesh information part of this primitive
  VBASE_IMPEXP void SerializeMeshInformation(VChunkFile &file, int iVersion=-1);

  /// \brief
  ///   Internal function: (De-)serializes the result information part of this primitive
  VBASE_IMPEXP void SerializeOutputInformation(VChunkFile &file,int iVersion=-1);

  /// \brief
  ///   Internal function
  VBASE_IMPEXP int FindVertexIDIndex(const MeshVertex_t *pVertex) const;

  /// \brief
  ///   Internal function
  VBASE_IMPEXP bool CheckValid(IVLog *pLog) const;

  /// \brief
  ///   Internal function
  VBASE_IMPEXP bool CheckValidVertex(MeshVertex_t *v, IVLog *pLog, bool &bReport, int iPrimFlags) const;

  /// \brief
  ///   Overridable that is called during lightmap live update. Custom primitive can implement this function and update the lightmap on the fly
  ///
  /// \param iMaterialIndex
  ///   The index of the material that is updated within this primitive
  ///
  /// \param iTextureSubIndex
  ///   A value between 0 and 3 that describes the bump lightmap index. Usually this is 0 for diffuse lightmap only
  ///
  /// \param pLightmap
  ///   The lightmap texture pointer
  ///
  /// \param pAtlasScaleOfs
  ///   An array of 4 floats that describe the atlas transformation of UV lightmap coordinates (xy=scale, zw=offset)
  ///
  VBASE_IMPEXP virtual void OnLiveUpdate(int iMaterialIndex, int iTextureSubIndex, VTextureObject *pLightmap, const float *pAtlasScaleOfs)
  {
  }

  ///
  /// @}
  ///

public:

  // identify primitive
  PrimitiveType_e m_eType;        ///< type of the primitive
  OwnerType_e m_eOwnerType;       ///< used to categorize the objects (engine vs. custom objects)
  __int64 m_iUniqueID;            ///< identifies the object after loading. For instance VisObject3D_cl::GetUniqueID serialized from vForge
  unsigned int m_iUserData;       ///< custom purpose, gets serialized
  int m_iSortingKey;              ///< receiver only: sorting key for charting. Might be a Zone ID

  // primitive features
  unsigned int m_iFlags; ///< see PRIMITIVEFLAG_xyz; doublesided flag is in m_RenderState

  hkvVec3 m_vPrimCenter;
  int m_iVertexCount, m_iMaterialCount;
  MeshVertex_t *m_pVertex;
  MeshMaterial_t *m_pMaterials;

  // output
  hkvVec2* m_pOutUV;

  MeshMaterial_t m_SingleMaterial; ///< do not use
};




/// \brief
///   An instance of VLightmapSceneInfo contains a collection of VLightmapPrimitive instances that
///   supplies vForge with occluder/receiver geometry
/// 
/// A VLightmapSceneInfo instance gets provided by the engine callbacks
/// VisCallbackManager_cl::OnGatherLightmapInfo (triggered by vForge to gather light map
/// information from the scene and supply it to vLux) and
/// VisCallbackManager_cl::OnLightmapFileLoaded (triggered by the scene when a new lit file is
/// loaded.
/// 
/// Both callbacks use the VLightmapInfoDataObject_cl data object that provides a scene object
/// reference.
/// 
/// Depending on whether the scene gathers mesh information or provides lightmap result
/// information, some streams might not be set in the primitives. For instance, mesh information is
/// not available (anymore) when loading the result information that vLux saves out.
class VLightmapSceneInfo : public VChunkFile
{
public:

  /// \brief
  ///   Internal collection class to hold instances of class VLightmapPrimitive
  class VLightmapPrimitiveList : public VRefCountedCollection<VLightmapPrimitive>
  {
  };

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VLightmapSceneInfo();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VLightmapSceneInfo();

  ///
  /// @name Create/access Primitives
  /// @{
  ///

  /// \brief
  ///   Creates and returns a new primitive. The returned primitive structure must be filled in
  ///   properly
  /// 
  /// This should only be done when vForge queries light map information
  /// (VisCallbackManager_cl::OnGatherLightmapInfo callback).
  VBASE_IMPEXP VLightmapPrimitive *CreateNewPrimitive();
 
  /// \brief
  ///   Helper function to find a primitive by its owner type and the unique ID
  /// 
  /// Can be used inside the VisCallbackManager_cl::OnLightmapFileLoaded callback to grab the
  /// lightmap UV set for a specific primitive.
  ///
  /// \param eType
  ///   Type of primitive to filter
  ///
  /// \param iUniqueID
  ///   Unique ID to find
  VBASE_IMPEXP VLightmapPrimitive *GetPrimitiveByID(VLightmapPrimitive::OwnerType_e eType, __int64 iUniqueID);

  /// \brief
  ///   Access the collection of primitives
  VBASE_IMPEXP VLightmapPrimitiveList &Primitives() {return m_PrimitiveList;}

  /// \brief
  ///   Counts the number of vertices used by all primitives
  VBASE_IMPEXP int GetOverallVertexCount() const;


  ///
  /// @}
  ///

  ///
  /// @name Access Lightmap Pages
  /// @{
  ///

  /// \brief
  ///   Returns the resulting number of lightmap pages. See GetLightmapTexture
  inline int GetPageCount() const {return m_iPageCount;}

  /// \brief
  ///   Returns the resulting number of sub-pages. This is either 1 (diffuse only) or 4 (1 diffuse
  ///   +3 dot3 lightmapping pages)
  inline int GetSubPageCount() const {return m_iSubPageCount;}

  /// \brief
  ///   Returns the lightmap page texture object. iPage must be in range [0..GetPageCount()-1] and
  ///   iSubIndex must be in range [0..GetSubPageCount()-1]
  VBASE_IMPEXP VTextureObject *GetLightmapTexture(int iPage, int iSubIndex);


  /// \brief
  ///   Indicates whether GetLightmapTexture may be called
  inline bool HasLightmapTextures() const {return m_spLightmapTextures!=NULL;}


  /// \brief
  ///   Indicates whether GetLightmapTextureName may be called
  inline bool HasLightmapTextureFilenames() const {return m_pLightmapTextureNames!=NULL;}

  ///
  /// @}
  ///

  ///
  /// @name Load / Save
  /// @{
  ///

  /// \brief
  ///   Saves this scene info object to a mesh information file. Can be read by vLux.
  VBASE_IMPEXP bool SaveMeshFile(const char *szFilename, IVFileStreamManager *pFileManager);

  /// \brief
  ///   Saves this scene info object to output file that provides lightmap UV information. Used by
  ///   vLux.
  VBASE_IMPEXP bool SaveOutputFile(const char *szFilename, IVFileStreamManager *pFileManager);

  /// \brief
  ///   Loads mesh information. Used by vLux.
  VBASE_IMPEXP bool LoadMeshFile(const char *szFilename, IVFileStreamManager *pFileManager);

  /// \brief
  ///   Loads the output information file. Used by custom code.
  VBASE_IMPEXP bool LoadOutputFile(const char *szFilename, IVFileStreamManager *pFileManager, IVTextureLoader *pLightmapProvider);
 
  /// \brief
  ///   Only loads the header information which contains lighting mode etc. Used internally
  VBASE_IMPEXP bool LoadSettings(const char *szFilename, IVFileStreamManager *pFileManager);


  // internal
  VBASE_IMPEXP void AllocateLightmaps(int iPages, int iSubPages, bool bTexObj, bool bTexNames);
  inline void AllocateLightmaps(int iPages, int iSubPages, bool bTexObj=true) {AllocateLightmaps(iPages,iSubPages,bTexObj,!bTexObj);}
  inline void SetLightmapTextureName(int iPage, int iSubIndex, const char *szFilename)
  {
    VASSERT(m_pLightmapTextureNames && iPage>=0 && iPage<m_iPageCount && iSubIndex>=0 && iSubIndex<m_iSubPageCount);
    m_pLightmapTextureNames[iPage*m_iSubPageCount+iSubIndex] = szFilename;
  }

  inline const char* GetLightmapTextureName(int iPage, int iSubIndex)
  {
    VASSERT(m_pLightmapTextureNames && iPage>=0 && iPage<m_iPageCount && iSubIndex>=0 && iSubIndex<m_iSubPageCount);
    return m_pLightmapTextureNames[iPage*m_iSubPageCount+iSubIndex];
  }

  inline void SetLightmapTexture(int iPage, int iSubIndex, VTextureObject *pTex)
  {
    VASSERT(m_spLightmapTextures && iPage>=0 && iPage<m_iPageCount && iSubIndex>=0 && iSubIndex<m_iSubPageCount);
    m_spLightmapTextures[iPage*m_iSubPageCount+iSubIndex] = pTex;
  }

  /// \brief
  ///   Returns the lightmap equation for this scene. The constants are of type
  ///   VIS_LIGHTING_MODULATE,...
  inline int GetLightingEquation() const {return m_iLightingEquation;}
    
  /// \brief
  ///   Sets the lightmap equation for this scene. The constants are of type
  ///   VIS_LIGHTING_MODULATE,...
  inline void SetLightingEquation(int iValue) {m_iLightingEquation=iValue;}

  /// \brief
  ///   Internal allocation function
  VBASE_IMPEXP void AllocateLightMaskEntries(int iCount);

  /// \brief
  ///   Returns the number allocated via AllocateLightMaskEntries
  inline int GetLightMaskEntryCount() const {return m_iNumLightMaskEntries;}

  /// \brief
  ///   Returns the number of light mask entries (allocated via AllocateShadowMapEntries)
  VLightmapPrimitive::VLightMaskEntry_t &GetLightMaskEntry(int iIndex) const
  {
    VASSERT(iIndex>=0 && iIndex<m_iNumLightMaskEntries);
    return m_pLightMaskEntry[iIndex];
  }

  // Internal
  VBASE_IMPEXP bool CheckValid(IVLog *pLog) const;

  /// \brief
  ///   Used by vForge to set an array of desired caster IDs. Applications should check this via
  ///   IsUsedCasterID
  VBASE_IMPEXP __int64* SetCasterFilterIDs( int iCount, const __int64 *pIDArray);

  /// \brief
  ///   Used by vForge to set an array of wanted receiver IDs. Applications should check this via
  ///   IsUsedReceiverID
  VBASE_IMPEXP __int64* SetReceiverFilterIDs( int iCount, const __int64 *pIDArray);


  /// \brief
  ///   Applications that use custom lightmapping should use this function to check whether their
  ///   ID should be actually used as a caster
  /// 
  /// vForge provides a subset of caster primitives e.g. primitives inside a zone.
  ///
  /// \param iID
  ///   ID to check
  ///
  /// \return
  ///   true if the ID is contained in the caster list
  VBASE_IMPEXP bool IsUsedCasterID(__int64 iID) const;

  /// \brief
  ///   Applications that use custom lightmapping should use this function to check whether their
  ///   ID should be actually used as a lightmap receiver
  /// 
  /// vForge provides a subset of receiver primitives e.g. primitives inside a zone.
  /// 
  /// \param iID
  ///   ID to check
  ///
  /// \return
  ///   true if the ID is contained in the receiver list
  VBASE_IMPEXP bool IsUsedReceiverID(__int64 iID) const;

  hkvAlignedBBox m_CombinedLightReceiverBox; ///< \brief set by vForge while collecting

  /// \brief
  ///   If set, the engine will ignore this info for entities and static meshes
  inline bool GetIncludeEngineShapes() const
  {
    return m_bIncludeEngineShapes;
  }

  /// \brief
  ///   If set, the engine will ignore this info for entities and static meshes
  inline void SetIncludeEngineShapes(bool bStatus)
  {
    m_bIncludeEngineShapes = bStatus;
  }

  ///
  /// @}
  ///

private:
  VBASE_IMPEXP bool SerializeHeader();
  VBASE_IMPEXP bool SerializePrimitives(bool bOutput);
  VBASE_IMPEXP bool SerializePages();
  VBASE_IMPEXP virtual void OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs);

  // private:
  IVTextureLoader *m_pLightmapProvider;
  VLightmapPrimitiveList m_PrimitiveList;
  int m_iPageCount, m_iSubPageCount;
  int m_iLightingEquation;  ///< output only, set by vLux
  int m_iLoadingVersion;
  VTextureObjectPtr *m_spLightmapTextures;
  VString *m_pLightmapTextureNames;

  int m_iCasterFilterIDCount, m_iReceiverFilterIDCount;
  __int64 *m_pCasterFilterID, *m_pReceiverFilterID;

  int m_iNumLightMaskEntries;
  VLightmapPrimitive::VLightMaskEntry_t *m_pLightMaskEntry;

  bool m_bIncludeEngineShapes;
public:
  VisZoneResource_cl *m_pCurrentZone; ///< just blind data used by vForge
};


/// \brief
///   Derived callback data object class used by the VisCallbackManager_cl::OnGatherLightmapInfo
///   and VisCallbackManager_cl::OnLightmapFileLoaded callback.
class VLightmapInfoDataObject_cl : public IVisCallbackDataObject_cl
{
public:
  inline VLightmapInfoDataObject_cl(VLightmapSceneInfo &info, VCallback *pSender) :
    IVisCallbackDataObject_cl(pSender),m_SceneInfo(info)
  {
  }

  VLightmapSceneInfo &m_SceneInfo;
};




////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline implementations
////////////////////////////////////////////////////////////////////////////////////////////////////

inline void VLightmapPrimitive::MeshMaterial_t::SetCustomRadiosityShader(const char *szLibFile, const char *szEffectName, const char *szParam)
{
  if (!szLibFile || !szLibFile[0] || !szEffectName || !szEffectName[0])
    m_sCustomRadiosityShader = NULL;
  else
  {
    char blank = 0;
    if (szParam==NULL)
      szParam = &blank;
    m_sCustomRadiosityShader.Format("%s:%s:%s",szLibFile,szEffectName,szParam);
  }
}

inline bool VLightmapPrimitive::MeshMaterial_t::GetCustomRadiosityShader(VString &sLibFile, VString &sEffectName, VString &sParam) const
{
  if (m_sCustomRadiosityShader.IsEmpty())
  {
    sLibFile = NULL;
    sEffectName = NULL;
    sParam = NULL;
    return false;
  } else
  {
    VMemoryTempBuffer<4096> buffer(m_sCustomRadiosityShader.AsChar());
    char *szStart = buffer.AsChar();
    char *szEnd = strchr(szStart,':');
    if (szEnd==NULL)
      return false;
    szEnd[0] = 0;
    sLibFile = szStart;
    szStart = szEnd+1;
    szEnd = strchr(szStart,':');
    if (szEnd==NULL)
      return false;
    szEnd[0] = 0;
    sEffectName = szStart;
    sParam = szEnd+1;
  }
  return true;
}

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
