/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSurface.hpp

#ifndef VISAPISURFACE_HPP_INCLUDED
#define VISAPISURFACE_HPP_INCLUDED

#include <Vision/Runtime/Engine/Material/VisApiMaterialTemplateManager.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiTextureManager.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisApiShaderEffect.hpp>
#include <Vision/Runtime/Engine/System/VisApiCallbacks.hpp>

class VisSurfaceTextureSet_cl;
typedef VSmartPtr<VisSurfaceTextureSet_cl> VisSurfaceTextureSetPtr;

class VisSurfaceLibrary_cl;
typedef VSmartPtr<VisSurfaceLibrary_cl> VisSurfaceLibraryPtr;

#define MAX_AUXILIARY_TEXTURES    128

/// \brief
///   Class that provides material textures for surfaces.
/// 
/// The VisSurface_cl class is derived from this class to inherit the material textures.
/// 
/// Furthermore, entities can replace material sets via the VisSurfaceTextureSet_cl class.
/// 
/// See VisBaseEntity_cl::SetCustomTextureSet
class VisSurfaceTextures_cl
{
public:

  /// \brief
  ///   Constructor
  VisSurfaceTextures_cl() : m_vLightmapScaleOfs(1.0f, 1.0f, 0.0f, 0.0f)
  {
    m_iIndex = 0;
    m_spAuxiliaryTextures = NULL;
    m_iAuxiliaryTextureCount = 0;
  }

  ~VisSurfaceTextures_cl()
  {
    V_SAFE_DELETE_ARRAY(m_spAuxiliaryTextures);
  }

  /// \brief Enum specifying the usage of a texture.
  enum VTextureType_e
  {
    VTT_Diffuse,      ///< The texture is used as a diffuse color map.
    VTT_NormalMap,    ///< The texture is used as a normal map.
    VTT_SpecularMap   ///< The texture is used as a specular map.
  };
 
  ///
  /// @name Access Textures
  /// @{
  ///
   
  /// \brief
  ///   Helper function to load and set a specific texture in this set.
  VISION_APIFUNC void SetTextureFile (VTextureType_e eType, const char *pszFileName);

  /// \brief
  ///   Helper function to return the texture filename string (or NULL) of the texture in the specified texture slot.
  VISION_APIFUNC const char *GetTextureFile(VTextureType_e eType) const;

  /// \brief
  ///   Sets a specific texture object in the passed slot (diffuse/normalmap/specularmap). If the diffuse texture is changed (VTT_Diffuse), then the texture animation instance is changed.
  VISION_APIFUNC void SetTexture (VTextureType_e eType, VTextureObject *pTex);

  /// \brief
  ///   Return the texture object in the specified texture slot.
  VISION_APIFUNC VTextureObject* GetTexture (VTextureType_e eType) const;


  /// \brief
  ///   Returns the file name of the base texture.
  inline const char *GetBaseTexture() const
  {
    if (m_spDiffuseTexture)
      return m_spDiffuseTexture->GetOriginalFilename();
    return NULL;
  }

  /// \brief
  ///   Returns the file name of the normal map texture.
  inline const char *GetNormalMapTexture() const
  {
    if (m_spNormalMap)
      return m_spNormalMap->GetOriginalFilename();
    return NULL;
  }


  /// \brief
  ///   Returns the file name of the specular texture.
  inline const char *GetSpecularMapTexture() const
  {
    if (m_spSpecularMap)
      return m_spSpecularMap->GetOriginalFilename();
    return NULL;
  }


  /// \brief
  ///   Returns a pointer to the base texture object that represents the diffuse base texture of the surface.
  inline VTextureObject *GetBaseTextureObject() const 
  {
    return m_spDiffuseTexture;
  }

  /// \brief
  ///   Sets the diffuse base texture on this material and creates a texture animation instance if the texture is animated.
  VISION_APIFUNC void SetBaseTexture(VTextureObject *pTex);

  /// \brief
  ///   Returns a pointer to the texture object that represents the normal map texture of the
  ///   surface.
  inline VTextureObject *GetNormalMapTextureObject() const 
  {
    return m_spNormalMap;
  }

  /// \brief
  ///   Returns the specular map texture object.
  inline VTextureObject *GetSpecularMapTextureObject() const 
  {
    return m_spSpecularMap;
  }


  /// \brief
  ///   Return the animation instance that is associated with the diffuse texture. If the diffuse texture is not animated, this function returns NULL.
  VisTextureAnimInstance_cl *GetDiffuseTextureAnimation() const
  {
    return m_spDiffuseAnim;
  }

  /// \brief
  ///   Returns the number of additional auxiliary textures associated with this material.
  inline int GetAuxiliaryTextureCount() const {return m_iAuxiliaryTextureCount;}

  /// \brief
  ///   Returns the additional auxiliary texture with index iIndex. The index must be in valid
  ///   range [0..GetAuxiliaryTextureCount()-1].
  ///
  /// \param iIndex
  ///  The index of the auxiliary texture.
  ///   
  /// The return value can be NULL is no auxiliary texture was set at the given index.
  VTextureObject *GetAuxiliaryTexture(int iIndex) const 
  {
    VASSERT(m_spAuxiliaryTextures!=NULL && iIndex>=0 && iIndex<m_iAuxiliaryTextureCount);
    return m_spAuxiliaryTextures[iIndex];
  }

  /// \brief
  ///   Allocates an array of iCount auxiliary textures for this material. See GetAuxiliaryTexture or SetAuxiliaryTexture.
  ///
  /// Auxiliary textures can be used to associate an arbitrary number of custom textures with a material without any hardcoded
  /// semantic (e.g. specular). Since auxiliary textures can be addressed in shader texture samplers, this is a convenient way
  /// to render geometry with identical shader setup but different texture sets.
  /// Allocating a new array does not preserve the old array in this material.
  ///
  /// \param iCount
  ///   new number of auxiliary textures. Can be 0
  VISION_APIFUNC void AllocateAuxiliaryTextures(int iCount);

  /// \brief
  ///   Associates an auxiliary texture with this material. The index must be in valid range defined by GetAuxiliaryTextureCount (which is the number of allocated textures via AllocateAuxiliaryTextures).
  void SetAuxiliaryTexture(int iIndex, VTextureObject *pTexture) const 
  {
    VASSERT(m_spAuxiliaryTextures!=NULL && iIndex>=0 && iIndex<m_iAuxiliaryTextureCount);
    m_spAuxiliaryTextures[iIndex] = pTexture;
  }

  ///
  /// @}
  ///

  /// \brief
  ///   Implements the SerializeX function to serialize the content of this set.
  VISION_APIFUNC void SerializeX( VArchive &ar );
  V_DECLARE_SERIALX( VisSurfaceTextures_cl, VISION_APIFUNC );


  /// \brief
  ///   Assignment operator; assigns the texture smart pointers componentwise.
  VisSurfaceTextures_cl &operator = (const VisSurfaceTextures_cl &other)
  {
    if(this == &other)
      return *this;
  
    m_iIndex = other.m_iIndex;
    m_spDiffuseTexture = other.m_spDiffuseTexture;
    m_spNormalMap = other.m_spNormalMap;
    m_spSpecularMap = other.m_spSpecularMap;
    m_spModelLightmaps[0] = other.m_spModelLightmaps[0];
    m_spModelLightmaps[1] = other.m_spModelLightmaps[1];
    m_spModelLightmaps[2] = other.m_spModelLightmaps[2];
    m_spModelLightmaps[3] = other.m_spModelLightmaps[3];
    m_vLightmapScaleOfs = other.m_vLightmapScaleOfs;

    //copy auxiliary textures
    V_SAFE_DELETE_ARRAY(m_spAuxiliaryTextures);
    m_iAuxiliaryTextureCount = other.m_iAuxiliaryTextureCount;
    if (m_iAuxiliaryTextureCount>0)
    {
      int i;
      m_spAuxiliaryTextures = new VTextureObjectPtr[m_iAuxiliaryTextureCount];
      for (i=0;i<m_iAuxiliaryTextureCount;i++)
        m_spAuxiliaryTextures[i] = other.m_spAuxiliaryTextures[i];
    }


    return *this;
  }

  /// \brief
  ///   Get the 0-based index of this entry in the owner's list.
  inline int GetIndex() const {return m_iIndex;}

  /// \brief
  ///   Internal function - do not use
  inline void SetIndex(int iIndex) {m_iIndex=iIndex;}

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Gather relevant resource information that pOwner relies on. In this case, add all textures.
  VISION_APIFUNC void GetDependencies(VResourceSnapshot &snapshot, VManagedResource *pOwner);
#endif
  int m_iIndex; // index

  hkvVec4 m_vLightmapScaleOfs;               ///< xy = scale, zw = ofsset for uv => lightmap uv transform
  VTextureObjectPtr m_spDiffuseTexture;      ///< the diffuse base texture of the material
  VTextureObjectPtr m_spNormalMap;           ///< the normalmap texture of the material
  VTextureObjectPtr m_spSpecularMap;         ///< the reflection map texture of the material
  VTextureObjectPtr m_spModelLightmaps[4];   ///< 0: diffuse, 1-3: dot3 lightmaps

  VisTextureAnimInstancePtr m_spDiffuseAnim; ///< texture animation instance for diffuse texture animation

  short m_iAuxiliaryTextureCount;
  VTextureObjectPtr *m_spAuxiliaryTextures;
};


/// \brief
///   Provides an array of VisSurfaceTextures_cl instances to match the material set of a model.
/// 
/// An instance of this class can be used to override the model material textures on a per-entity basis; see VisBaseEntity_cl::SetCustomTextureSet.
/// 
/// Once a set is initialized for a model, each entry in the m_pSurfaceTextures array corresponds
/// to a surface in the mesh, i.e. pSet->GetTextures(i) <-> pMesh->GetSurfacebyIndex(i).
/// This has to be taken into account when modifying the replacement textures.
class VisSurfaceTextureSet_cl : public IVSerializationProxyCreator, public VRefCounter
{
public:

  /// \brief
  ///   Constructor that initializes an empty set. Should not be used, use one of the other constructors instead.
  VisSurfaceTextureSet_cl()
  {
    m_iSurfaceCount = 0;
    m_pSurfaceTextureArray = NULL;
    m_pSurfaceArray = NULL;
    m_pSurfaceRefArray = NULL;
  }

  /// \brief
  ///   Constructor that initializes this set with the mesh's surface array.
  VISION_APIFUNC VisSurfaceTextureSet_cl(VBaseMesh *pModel, bool bCreateSrfInstances=false);

  /// \brief
  ///   Constructor that initializes this set with the entity model's surface array.
  VISION_APIFUNC VisSurfaceTextureSet_cl(VisBaseEntity_cl *pEntity, bool bCreateSrfInstances=false);

  /// \brief
  ///   Constructor that initializes this set with the static mesh's surface array.
  VISION_APIFUNC VisSurfaceTextureSet_cl(VisStaticMeshInstance_cl *pMeshInst);

  /// \brief
  ///   Destructor. Frees the allocated array.
  VISION_APIFUNC VOVERRIDE ~VisSurfaceTextureSet_cl();

  /// \brief
  ///   Initializes this set with the mesh's surface array.
  /// 
  /// It allocates an array of VisSurfaceTextures_cl or VisSurface_cl instances using the same amount as the mesh surfaces.
  /// Each array element gets properly cloned from the original mesh materials.
  /// A set that has been created through this function can be used for functions VisBaseEntity_cl::SetCustomTextureSet or 
  /// VisStaticMeshInstance_cl::SetCustomSurfaceSet. If used for static meshes, the bCreateSrfInstances parameter has to be true.
  ///
  /// \param pMesh
  ///   Mesh to use.
  ///
  /// \param bCreateSrfInstances
  ///   If true, an array of VisSurface_cl is created, an array of the lightweight parent class VisSurfaceTextures_cl otherwise.
  ///   Static meshes can only work on VisSurface_cl so in this case true has to be passed here.
  ///
  /// \return
  ///   true if successful.
  VISION_APIFUNC bool CreateForMesh(VBaseMesh *pMesh, bool bCreateSrfInstances);

  /// \brief
  ///   Wrapper function for backwards compatibility. Simply calls CreateForMesh.
  VISION_APIFUNC bool CreateForModel(VDynamicMesh *pMesh, bool bCreateSrfInstances=false);

  /// \brief
  ///   Implements the SerializeX function to serialize the content of this set.
  VISION_APIFUNC void SerializeX( VArchive &ar );
  V_DECLARE_SERIALX( VisSurfaceTextureSet_cl, VISION_APIFUNC );

  /// \brief
  ///   Implements IVSerializationProxyCreator.
  VISION_APIFUNC VOVERRIDE IVSerializationProxy *CreateProxy();

  /// \brief
  ///   Internal name that is serialized and which can be used to identify a variant by name. Used by vForge.
  VISION_APIFUNC void SetVariantName(const VString& sName)
  {
    m_sVariantName = sName;
  }

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Gather relevant resource information that pOwner relies on. In this case, add all textures
  ///   from all sets.
  void GetDependencies(VResourceSnapshot &snapshot, VManagedResource *pOwner)
  {
    for (int i=0;i<m_iSurfaceCount;i++)
      GetTextures(i)->GetDependencies(snapshot,pOwner);
  }
#endif

  /// \brief
  ///   Returns the number of allocated textures / surfaces in this set. This matches pMesh->GetSurfaceCount of the owner mesh.
  int GetSurfaceCount() const
  {
    return m_iSurfaceCount;
  }

  /// \brief
  ///   Returns the textures of surface with specified index, where index must be in valid range [0..GetSurfaceCount()-1].
  VisSurfaceTextures_cl* GetTextures(int iIndex) const;

  /// \brief
  ///   Indicates whether the array has been allocated with VisSurface_cl instances, i.e. bCreateSrfInstances = true was passed during creation.
  bool UsesSurfaces() const
  {
    return m_pSurfaceRefArray!=NULL;
  }

  /// \brief
  ///   Returns the array as an array of VisSurface_cl instances. This function requires that UsesSurfaces() == true.
  inline VisSurface_cl** AsSurfaceArray() const
  {
    VASSERT_MSG(UsesSurfaces(), "Must not call this function when UsesSurfaces indicates false");
    return m_pSurfaceRefArray;
  }

protected:
  void DeleteArray();
  short m_iSurfaceCount; ///< Number of materials in this set. Automatically allocated by constructor
  VisSurfaceTextures_cl *m_pSurfaceTextureArray;  ///< Pointer to the array of material textures. The array holds m_iSurfaceCount elements
  VisSurface_cl *m_pSurfaceArray;  ///< Pointer to the array of surfaces. The array holds m_iSurfaceCount elements
  VisSurface_cl **m_pSurfaceRefArray;  ///< Pointer to the array of surface pointers. The array holds m_iSurfaceCount elements
  VString m_sVariantName;
};

/// \brief
///   Material class used by the Vision engine.
/// 
/// An instance of this class can represent a material used for either world primitives or entity
/// models.
/// 
/// Use this class to obtain information about material-specific data in the Vision engine.
/// 
/// This class inherits the material textures it uses from the VisSurfaceTextures_cl.
/// 
class VisSurface_cl : public VBaseObject, public VUserDataObj, public VisSurfaceTextures_cl 
{
  // non-copyable (using standard operators)
  VisSurface_cl(const VisSurface_cl &surface);
  VisSurface_cl &operator=(const VisSurface_cl &surface);

public:
  /// \brief
  ///   Enum that defines the effect assignment mode for surfaces. See VisSurface_cl::GetShaderMode.
  enum VShaderMode_e
  {
    VSM_Auto     = 0,    ///< Shader selected automatically. 
    VSM_Manual   = 1,    ///< Shader selected manually.
    VSM_Template = 2     ///< Shader applied via an asset profile dependent shader assignment template.
  };

  /// \brief
  ///  Flags passed to the shader provider to control automatic shader assignment.
  enum VSurfaceFlags_e
  {
    VSF_None                       = 0x00000000,   ///< No flags.
    VSF_ObjectSpaceCoordinates     = 0x00000001,   ///< Unused.
    VSF_VertexColor                = 0x00000010,   ///< Select a shader that uses vertex colors.
    VSF_NoDefaultShaders           = 0x00001000,   ///< Don't fall back to default material shaders.
    VSF_NoDynamicLightShaders      = 0x00002000,   ///< Don't select dynamic lighting shaders.
    VSF_TriggerCallback_Internal   = 0x10000000,   ///< Used internally, do not use.
    VSF_HasAdditionalForwardPass   = 0x20000000,   ///< Select a shader that supports additional forward passes.
    VSF_Default                    = VSF_None
  };

  /// \brief
  ///   Topology of the geometry referencing this surface.
  enum VGeometryTopology_e
  {
    VGT_3DMesh = 0,                 ///< 3D mesh that refers to this surface contains positions that can be straightly multiplied with MVP matrix in vertex shader.
    VGT_TerrainMesh,                ///< Similar to VGT_3DMesh, but a terrain mesh is referencing to this surface.
    VGT_Billboard,                  ///< Billboard which needs special handling in vertex shader: POSITION=center, COLOR=RGBA color, TEXCOORD0=UVs, TEXCOORD1=stretch x/y.
    VGT_BillboardVerticalAligned,   ///< Similar to VGT_Billboard, but the billboard is vertically aligned.

    VGT_FirstCustom = 100           ///< First slot that can be used for custom topologies.
  };

  /// \brief Constructor.
  VISION_APIFUNC VisSurface_cl();

  /// \brief Destructor.
  VISION_APIFUNC virtual ~VisSurface_cl();

  /// \brief
  ///   Returns true if both surfaces equal each other.
  VISION_APIFUNC bool Equals(VisSurface_cl *pOtherSurface) const;

  /// \brief
  ///   Sets all smart pointers to NULL so this surface does not hold any references anymore.
  VISION_APIFUNC void ClearReferences();

  ///
  /// @name Surface Name
  /// @{
  ///


  /// \brief
  ///   Returns the surface name.
  inline const char* GetName() const {return (m_sName.AsChar ());}

  /// \brief
  ///   Sets the name of this surface.
  inline void SetName(const char *szNewString) { m_sName = szNewString; }

  /// \brief
  ///   Returns the surface name as a VHashString.
  inline const VHashString& GetNameString() const {return (m_sName);}

  ///
  /// @}


  ///
  /// @name User Data
  /// @{
  ///

  /// \brief
  ///   Returns the user data string set in editing tools (e.g. vEdit, Maya). If not set, the return value is NULL.
  VISION_APIFUNC const char *GetEditorUserDataString() const 
  {
    return m_sUserData.IsEmpty() ? NULL : m_sUserData.AsChar(); // Explicitly return NULL here.
  }


  /// \brief
  ///   Sets a new user data string.
  VISION_APIFUNC void SetEditorUserDataString(const char *szNewString) { m_sUserData = szNewString; }


  ///
  /// @}
  ///

  ///
  /// @name Surface Properties
  /// @{
  ///

  /// \brief
  ///   Returns the transparency type of the surface.
  /// 
  /// \return
  ///   VIS_TransparencyType: The transparency type of the surface
  /// 
  /// \sa VIS_TransparencyType
  VISION_APIFUNC VIS_TransparencyType GetTransparencyType () const {
    return static_cast<VIS_TransparencyType>(m_uiTransparencyType);
  }

  /// \brief
  ///   Sets the transparency type of the surface.
  VISION_APIFUNC void SetTransparencyType (VIS_TransparencyType tt) {
    m_uiTransparencyType = static_cast<UBYTE>(tt);
    ResolvePassType();
  }

  /// \brief
  ///   Returns whether the surface is translucent, i.e. it uses a blend mode other than VIS_TRANSP_NONE or VIS_TRANSP_ALPHATEST.
  VISION_APIFUNC bool IsTranslucent() const {
    return (m_uiTransparencyType != VIS_TRANSP_NONE) && (m_uiTransparencyType != VIS_TRANSP_ALPHATEST);
  }

  /// \brief
  ///   Returns whether the surface is light-mapped.
  VISION_APIFUNC bool IsLightMapped() const;

  /// \brief
  ///   Determines whether this material satisfies a few properties required to support lightmapping.
  /// 
  /// This test does not completely test whether lightmapping is supported; e.g. it does not check whether the mesh
  /// has a required UV1 coordinate channel.
  VISION_APIFUNC bool IsLightmappingSupported() const;


  /// \brief
  ///   Deprecated; not used anymore.
  HKV_DEPRECATED_2012_1 bool AreSurfaceManipsAllowed() const {
    return true;
  }

  /// \brief
  ///   Returns whether the surface is double-sided.
  /// 
  /// No backface culling will be performed for primitives using a double-sided surface.
  /// 
  /// \return
  ///   bool: true if the surface is double-sided
  VISION_APIFUNC bool IsDoubleSided() const {
    return m_bDoubleSided;
  }

  /// \brief
  ///   Sets whether the surface is double-sided.
  VISION_APIFUNC void SetDoubleSided (bool b) {m_bDoubleSided = b;}

  /// \brief
  ///   Determines whether the default shader of this material supports depth writing.
  VISION_APIFUNC bool IsDepthWriteEnabled() const {return m_bDepthWrite;}

  /// \brief
  ///   Returns the pass type assigned by the material provider.
  ///
  /// \return
  ///   The pass type.
  ///
  /// The default is VPT_AUTO, which means the shader provider uses information such as depth write and transparency settings to determine the effective pass type.
  VISION_APIFUNC VPassType_e GetPassType() const {return (VPassType_e)m_uiPassType;}

  /// \brief
  ///   Sets a new pass type. A mode other than VPT_AUTO (default) enforces the material shader to use the given pass type regardless of depth write flag or transparency mode.
  VISION_APIFUNC void SetPassType(VPassType_e eType) {m_uiPassType = eType;}

  /// \brief
  ///   Returns the automatically resolved pass type. When a pass type is forced on the surface, that one is used. Otherwise the pass type from the shader is used.
  ///   If there is no pass type set on the shader, the transparency settings are used to determine the pass type
  VISION_APIFUNC VPassType_e GetResolvedPassType() const { return (VPassType_e)m_uiResolvedPassType; }

  /// \brief
  ///   Sets whether the default shader of this material will use depth writing.
  VISION_APIFUNC void SetDepthWriteEnabled (bool b) {m_bDepthWrite = b;}

  /// \brief
  ///   Returns the z-offset value that is passed to the shader.
  VISION_APIFUNC float GetDepthBias() const {return m_fDepthBias;}

  /// \brief
  ///   Sets the z-offset value that is passed to the shader.
  VISION_APIFUNC void SetDepthBias(float f) {m_fDepthBias = f;}

  /// \brief
  ///   Returns the z-offset clamping value that is passed to the shader.
  VISION_APIFUNC float GetDepthBiasClamp() const {return m_fDepthBiasClamp;}

  /// \brief
  ///   Sets the z-offset clamping value that is passed to the shader. 
  VISION_APIFUNC void SetDepthBiasClamp(float f) {m_fDepthBiasClamp = f;}

  /// \brief
  ///   Returns the slope dependent z-offset value that is passed to the shader.
  VISION_APIFUNC float GetSlopeScaledDepthBias() const {return m_fSlopeScaledDepthBias;}

  /// \brief
  ///   Sets the slope dependent z-offset value that is passed to the shader.
  VISION_APIFUNC void SetSlopeScaledDepthBias(float f) {m_fSlopeScaledDepthBias = f;}

  /// \brief
  ///   Returns the topology of the geometry referencing this surface.
  ///
  /// \see VisSurfaceTextures_cl::VGeometryTopology_e
  VISION_APIFUNC VGeometryTopology_e GetGeometryTopology() const {
    return static_cast<VGeometryTopology_e>(m_uiGeometryTopology);
  }

  /// \brief
  ///   Sets the geometry topology.
  ///
  /// \param eType
  ///   Geometry topology of the geometry referencing this surface.
  ///
  /// \see VisSurfaceTextures_cl::VGeometryTopology_e
  VISION_APIFUNC void SetGeometryTopology(VGeometryTopology_e eType) {
    m_uiGeometryTopology = eType;
  }

  /// \brief
  ///   Returns whether the surface is fullbright.
  /// 
  /// Fullbright surfaces are not affected by any lighting.
  /// 
  /// \return
  ///   bool: true if the surface is fullbright.
  VISION_APIFUNC bool IsFullbright() const {
    return m_uiLightingMethod == VIS_LIGHTING_FULLBRIGHT;
  }

  /// \brief
  ///   Returns the ambient color of the surface.
  VISION_APIFUNC VColorRef GetAmbientColor() const {return m_AmbientColor;}

  /// \brief
  ///   Sets the ambient color of the surface.
  VISION_APIFUNC void SetAmbientColor(const VColorRef& color) {m_AmbientColor = color;}

  /// \brief
  ///   Returns the specular multiplier.
  /// 
  /// A return value of 0.0 means that the surface does not have specularity.
  /// 
  /// \return
  ///   float : The specular multiplier
  VISION_APIFUNC float GetSpecularMultiplier() const { return m_fSpecMul;}
  
  /// \brief
  ///   Sets the specular multiplier.
  VISION_APIFUNC void  SetSpecularMultiplier (float f) {m_fSpecMul = f;}

  
  /// \brief
  ///   Returns the specular exponent.
  /// 
  /// A return value of 0.0 means that the surface does not have specularity.
  /// 
  /// \return
  ///   float : The specular exponent
  VISION_APIFUNC float GetSpecularExponent() const   { return m_fSpecExp;}

  /// \brief
  ///   Sets the specular exponent.
  /// \param f The new specular exponent.
  VISION_APIFUNC void  SetSpecularExponent (float f) {m_fSpecExp = f;}
  
  /// \brief
  ///   Returns the alpha test threshold value [0..1] of this surface.
  /// 
  /// If the m_fCustomAlphaThreshold member of this surface is left to default -1 then this
  /// function returns the global alpha threshold.
  /// 
  /// The global alpha test threshold value is defined via
  /// VisRenderer_cl::SetGlobalAlphaTestThreshold.
  VISION_APIFUNC float GetAlphaTestThreshold() const;

  /// \brief
  ///   Sets the alpha test threshold value [0..1] of this surface.
  ///
  /// \param fAlphaThreshold
  ///   The new alpha test threshold.
  ///
  /// \sa GetAlphaTestThreshold
  VISION_APIFUNC void  SetAlphaTestThreshold(float fAlphaThreshold) {
    m_fCustomAlphaThreshold = fAlphaThreshold;
  }

  /// \brief Sets the static mesh pointer of this resource.
  VISION_APIFUNC void SetMesh (VBaseMesh* pMesh) {m_pOwnerMesh = pMesh;}

  /// \brief Returns the static mesh pointer of this resource.
  VISION_APIFUNC VBaseMesh* GetMesh (void) const {return (m_pOwnerMesh);}

  /// \brief
  ///   Sets the internal sorting key (does not trigger re-sorting).
  VISION_APIFUNC void SetSortingKey (unsigned char iKey) { m_uiSortingKey = iKey ; }

  /// \brief
  ///   Returns the sorting key for this surface.
  VISION_APIFUNC unsigned char GetSortingKey() const { return m_uiSortingKey; }

  /// \brief
  ///   Returns whether submeshes with this material should cast static shadows.
  VISION_APIFUNC bool GetCastStaticShadows (void) const {return (m_bCastStaticShadows);}

  /// \brief
  ///   Sets whether submeshes with this material should cast static shadows.
  VISION_APIFUNC void SetCastStaticShadows (bool b) {m_bCastStaticShadows = b;}

  /// \brief
  ///   Sets all material flags.
  ///
  /// \param Flags
  ///  The new material flags.
  ///
  /// \sa VSurfaceFlags_e
  VISION_APIFUNC void SetMaterialFlags (int Flags) {m_iMaterialFlags = Flags; ResolvePassType(); }

  /// \brief
  ///   Returns whether some material flag is set.
  ///
  /// \param Flag
  ///  The material flag to test.
  ///
  /// \sa VSurfaceFlags_e
  VISION_APIFUNC bool IsMaterialFlagSet (int Flag) const {return ((m_iMaterialFlags & Flag) != 0);}

  /// \brief
  ///   Raises some material flag.
  ///
  /// \param Flag
  ///  The material flag to raise.
  /// 
  /// \sa VSurfaceFlags_e
  VISION_APIFUNC void RaiseMaterialFlag (int Flag) {m_iMaterialFlags |= Flag; ResolvePassType(); }

  /// \brief
  ///   Clears some material flag.
  ///
  /// \param Flag
  ///  The material flag to clear.
  ///
  /// \sa VSurfaceFlags_e
  VISION_APIFUNC void ClearMaterialFlag (int Flag) {m_iMaterialFlags &= ~Flag; ResolvePassType(); }


  /// \brief
  ///   Sets all user flags.
  VISION_APIFUNC void SetUserFlags (int Flags) {m_iUserFlags = Flags;}

  /// \brief
  ///   Returns all user flags.
  VISION_APIFUNC int GetUserFlags (void) const {return (m_iUserFlags);}

  /// \brief
  ///   Returns whether some user flag is set.
  VISION_APIFUNC bool IsUserFlagSet (int Flag) const {return ((m_iUserFlags & Flag) != 0);}

  /// \brief
  ///   Raises some user flag.
  VISION_APIFUNC void RaiseUserFlag (int Flag) {m_iUserFlags |= Flag;}

  /// \brief
  ///   Clears some user flag.
  VISION_APIFUNC void ClearUserFlag (int Flag) {m_iUserFlags &= ~Flag;}


  /// \brief
  ///   Returns the deferred ID of this surface.
  VISION_APIFUNC UBYTE GetDeferredID (void) const {return (m_uiDeferredID);}

  /// \brief
  ///   Sets the deferred ID of this surface.
  VISION_APIFUNC void SetDeferredID (UBYTE id) {m_uiDeferredID = id;}

  /// \brief
  ///   Returns the parallax scale value.
  VISION_APIFUNC float GetParallaxScale (void) const {return (m_fParallaxScale);}

  /// \brief
  ///   Sets the parallax scale value.
  VISION_APIFUNC void  SetParallaxScale (float f) {m_fParallaxScale = f;}

  /// \brief
  ///   Returns the parallax bias value.
  VISION_APIFUNC float GetParallaxBias (void) const {return (m_fParallaxBias);}

  /// \brief
  ///   Sets the parallax bias value.
  VISION_APIFUNC void  SetParallaxBias (float f) {m_fParallaxBias = f;}


  ///
  /// @}
  ///

  ///
  /// @name Lightmap related
  /// @{
  ///

  /// \brief
  ///   Returns the lightmap's granularity.
  VISION_APIFUNC float GetLightMapGranularity (void) const {return (m_fLightmapGranularity);}

  /// \brief
  ///   Sets the lightmap's granularity.
  VISION_APIFUNC void  SetLightMapGranularity (float f) {m_fLightmapGranularity = f;}

  /// \brief
  ///   Returns the page-id of the used lightmap.
  VISION_APIFUNC int  GetLightMapPageID (void) const {return (m_iLightmapPageID);}

  /// \brief
  ///   Sets the page-id of the used lightmap.
  VISION_APIFUNC void SetLightMapPageID (int id) {m_iLightmapPageID = id;}

  /// \brief
  ///   Returns the size of the lightmap along the x-axis.
  VISION_APIFUNC int  GetLightMapPageSizeX (void) const {return (m_iLightmapPageSize[0]);}

  /// \brief
  ///   Returns the size of the lightmap along the y-axis.
  VISION_APIFUNC int  GetLightMapPageSizeY (void) const {return (m_iLightmapPageSize[1]);}

  /// \brief
  ///   Sets the size of the lightmap along the x-axis.
  VISION_APIFUNC void SetLightMapPageSizeX (int size) {m_iLightmapPageSize[0] = size;}

  /// \brief
  ///   Sets the size of the lightmap along the y-axis.
  VISION_APIFUNC void SetLightMapPageSizeY (int size) {m_iLightmapPageSize[1] = size;}

  ///
  /// @}
  ///

  ///
  /// @name Shader Effect Related
  /// @{
  ///


  /// \brief
  ///   Sets the technique to be used for rendering this surface. Use VisSurface_cl::SetEffect instead.
  VISION_APIFUNC void SetTechnique(VCompiledTechnique *pTech);

  /// \brief
  ///   Provided for backwards compatibility. Internally calls the current shader provider's SetEffect() function.
  VISION_APIFUNC void SetEffect(VCompiledEffect *pFX, VTechniqueConfig* pDefaultConfig = NULL);

  /// \brief
  ///   Changes the current shader to the one provided by the given material template for the current asset profile.
  ///
  /// This function will also set the shader mode to VSM_Template and lets the shader provider update the shader assignments.
  /// If szMaterialTemplatePath is NULL, the template will be removed and the shader mode is set to VSM_AUTO.
  ///
  /// \param szMaterialTemplatePath
  ///   The path to the material template (.vmtx) file.
  ///
  /// \return
  ///  True if the template was loaded successfully.
  VISION_APIFUNC bool SetMaterialTemplate(const char* szMaterialTemplatePath);

  /// \brief
  ///   Returns the currently set material template.
  VISION_APIFUNC const VMaterialTemplateResource* GetMaterialTemplate();

  /// \brief
  ///   Returns the correct shader effect instance. This can be the default effect or the custom
  ///   effect assignment assigned using SetEffect.
  VISION_APIFUNC VCompiledEffect* GetEffect() const { return m_spCurrentEffect; }

  /// \brief
  ///   Returns the shader technique for this surface.
  VISION_APIFUNC VCompiledTechnique *GetTechnique() const;


  /// \brief
  ///   Used internally to set VSM_Auto vs. VSM_Manual vs. VSM_Template mode.
  VISION_APIFUNC void SetShaderMode(VShaderMode_e eShaderMode)
  { 
    m_eShaderMode = eShaderMode;
  }
  
  /// \brief
  ///   Returns the current shader effect mode. Can be VSM_Auto, VSM_Manual or VSM_Template.
  /// 
  /// Surfaces marked as VSM_Auto will always get the appropriate surface effect
  /// assigned by the shader provider class (IVisShaderProvider_cl).
  /// 
  /// When manually assigning a shader effect, the surface will be marked as VSM_Manual and when
  /// choosing the shader via a template the surface will be marked as VSM_Template.
  /// 
  /// Only shader assignments of type VSM_Manual are saved into the shader assignment
  /// file (world and models)
  VISION_APIFUNC VShaderMode_e GetShaderMode() const
  {
    return m_eShaderMode;
  }

  /// \brief
  ///   Reapplies the surface shader assignment. This is needed for example if the shader provider changed.
  ///
  /// \param bIncludeManualAssignments
  ///    If true, manual shader assignments are reapplied via the shader provider.
  /// \param iFlags
  ///    These flags (VSurfaceFlags_e) are passed to the shader provider to influence the automatic shader assignment.
  VISION_APIFUNC void ReapplyShader(bool bIncludeManualAssignments, int iFlags = VSF_None);
  
#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Gather relevant resource information that pOwner relies on. In this case, add all textures
  ///   and shader information.
  VISION_APIFUNC void GetDependencies (VResourceSnapshot &snapshot, VManagedResource *pOwner);
#endif

  /// \brief
  ///   Fills the passed structure with relevant information.
  VISION_APIFUNC void GetLightmapInfo (VLightmapPrimitive::MeshMaterial_t &material) const;

  /// \brief
  ///   Returns the lighting mode of this surface.
  VISION_APIFUNC VisLightingMethod_e GetLightingMode () const 
  { return static_cast<VisLightingMethod_e>(m_uiLightingMethod); }

  /// \brief
  ///   Sets the lighting mode of this surface.
  ///
  /// \param mode
  ///   The new lighting mode.
  ///
  /// \sa VisLightingMethod_e
  VISION_APIFUNC void SetLightingMode (VisLightingMethod_e mode) 
  { m_uiLightingMethod = static_cast<UBYTE>(mode); }

  ///
  /// @}
  ///

public:

  /// \brief
  ///   Callback data object class that provides a surface pointer. Sent by the
  ///   OnSurfaceCreated/OnSurfaceDestroyed callbacks.
  class VisSurfaceDataObject_cl : public IVisCallbackDataObject_cl
  {
  public:
    VisSurfaceDataObject_cl(VisCallback_cl *pSender, VisSurface_cl *pSurface) : IVisCallbackDataObject_cl(pSender)
    {
      m_pSurface = pSurface;
    }
    VisSurface_cl *m_pSurface;
  };

  /// \brief
  ///   Callback data object class that is used for the OnXMLDataExchange callback.
  class VisSurfaceXMLExchangeDataObject_cl : public VisSurfaceDataObject_cl
  {
  public:
    VisSurfaceXMLExchangeDataObject_cl(VisCallback_cl *pSender, VisSurface_cl *pSurface, TiXmlElement*pNode, const char *targetFilename, bool bWrite) 
      : VisSurfaceDataObject_cl(pSender,pSurface)
    {
      m_pSurface = pSurface;
      m_pMaterialNode = pNode;
      m_szTargetFilename = targetFilename;
      m_bWrite = bWrite;
    }
    TiXmlElement *m_pMaterialNode;
    const char *m_szTargetFilename;
    bool m_bWrite;
  };

  /// \brief
  ///   Static callback that is triggered when a surface is created. The callback data
  ///   object can be casted to VisSurfaceDataObject_cl to get the surface pointer.
  VISION_APIDATA static VisCallback_cl OnSurfaceCreated;

  /// \brief
  ///   Static callback that is triggered when a surface is destroyed. The callback data
  ///   object can be casted to VisSurfaceDataObject_cl to get the surface pointer.
  VISION_APIDATA static VisCallback_cl OnSurfaceDestroyed;

  /// \brief
  ///   This callback is only called from inside the editor when any surface property was changed by the user in the material editor panel.
  ///   The callback data object can be casted to VisSurfaceDataObject_cl to get the surface pointer.
  VISION_APIDATA static VisCallback_cl OnSurfaceShaderChanged;

  /// \brief
  ///   Static callback that is triggered when a surface is loaded from or saved to XML.
  /// 
  /// This is the callback analogue to the shader provider's virtual MaterialXMLDataExchange
  /// function.
  /// 
  /// The passed data object can be casted to VisSurfaceXMLExchangeDataObject_cl which contains the relevant
  /// data exchange information.
  VISION_APIDATA static VisCallback_cl OnXMLDataExchange;

  VISION_APIFUNC void CopyFrom(const VisSurface_cl &other);
  VISION_APIFUNC void CopyLightTechniques(const VisSurface_cl &other);
  VISION_APIFUNC void DeleteSurfaceData();
  VISION_APIFUNC void OnCreated(bool bTriggerCallback=true);
  VISION_APIFUNC void OnDestroyed();
  VISION_APIFUNC void UnTagTextures();
  VISION_APIFUNC void AddMemFromUnTaggedTextures(size_t &iSysMem,size_t &iGPUMem);
  VISION_APIFUNC int Compare(const VisSurface_cl &other); ///< used for surface sorting

  //Update an xml node for saving the material properties to file
  // szTargetDir is the directory to make texture files relative to
  VISION_APIFUNC void UpdateXML (TiXmlElement* pMaterialNode, const char *szTargetDir, const char* szMaterialFileName = "materials.xml");


  /// \brief
  /// Helper struct to gather all the additional data (e.g. textures) that need to be loaded
  /// after the Surface has been loaded.
  /// 
  /// \sa ReadFromBinaryChunk, UpdateFromXML, LoadData
  struct VisSurface_DataToLoad
  {
    VisSurface_DataToLoad (void)
    {
      m_iAuxTextureCount = 0;
      m_iShaderCount = 0;
      m_sAuxFilenames = m_sAuxFilenamesNoAlloc;
    }

    ~VisSurface_DataToLoad()
    {
      Reset();
    }

    void Reset (void)
    {
      if (m_sAuxFilenames != m_sAuxFilenamesNoAlloc)
        delete[] m_sAuxFilenames;

      m_sAuxFilenames = m_sAuxFilenamesNoAlloc;

      for (int i = 0; i < 4; ++i)
        m_sAuxFilenames[i].Reset ();

      m_iAuxTextureCount = 0;
      m_iShaderCount = 0;

      m_sDiffuseMap.Reset ();
      m_sSpecularMap.Reset ();
      m_sNormalMap.Reset ();
      m_sEffectLibrary.Reset ();
      m_sEffectName.Reset ();
      m_sParamString.Reset ();
      m_sTemplateName.Reset ();
      m_sOverrideLibrary.Reset ();
      m_sOverrideMaterial.Reset ();

    }

    void AllocAuxTextures(int iCount)
    {
      if (m_iAuxTextureCount == iCount)
        return;
      m_iAuxTextureCount = iCount;
      if (m_sAuxFilenames != m_sAuxFilenamesNoAlloc)
        delete[] m_sAuxFilenames;
      m_sAuxFilenames = m_sAuxFilenamesNoAlloc;
      if (m_iAuxTextureCount>4)
        m_sAuxFilenames = new VStaticString<FS_MAX_PATH>[m_iAuxTextureCount];
    }

    // Textures
    int m_iAuxTextureCount;
    VStaticString<FS_MAX_PATH> m_sDiffuseMap;
    VStaticString<FS_MAX_PATH> m_sSpecularMap;
    VStaticString<FS_MAX_PATH> m_sNormalMap;

    VStaticString<FS_MAX_PATH> *m_sAuxFilenames;
    VStaticString<FS_MAX_PATH> m_sAuxFilenamesNoAlloc[4];

    // Shaders
    int m_iShaderCount;
    VStaticString<FS_MAX_PATH> m_sEffectLibrary;
    VStaticString<256> m_sEffectName;
    VStaticString<1024> m_sParamString;
    VStaticString<FS_MAX_PATH> m_sTemplateName;

    // Material Override
    VStaticString<FS_MAX_PATH> m_sOverrideLibrary;
    VStaticString<FS_MAX_PATH> m_sOverrideMaterial;
  };

  /// \brief
  /// Loads the Surface from a binary chunk. 
  ///
  /// \param File
  /// The file-stream from which to load the data.
  ///
  /// \param out_ToLoad
  /// The function fills this helper struct with the information of which textures and shaders need to be loaded.
  ///
  /// \return
  /// True if the data could be read successfully.
  VISION_APIFUNC bool ReadFromBinaryChunk (VChunkFile& File, VisSurface_DataToLoad& out_ToLoad);

  /// \brief
  /// Loads the Surface from an archive chunk. 
  VISION_APIFUNC void SerializeX( VArchive &ar );
  V_DECLARE_SERIALX( VisSurface_cl, VISION_APIFUNC );

  /// \brief
  /// Reads the Surface data from XML and updates all values.
  ///
  /// \param pMaterialNode
  /// The XML node from which to read the data.
  ///
  /// \param inout_ToLoad
  /// The struct that will be updated with the textures etc. that need to be loaded.
  ///
  /// \param szMaterialFileName
  /// The filename used to retrieve the material settings. "materials.xml" is used by default.
  ///
  /// \return
  /// True on success.
  VISION_APIFUNC bool UpdateFromXML (TiXmlElement* pMaterialNode, VisSurface_DataToLoad& inout_ToLoad, const char* szMaterialFileName = "materials.xml");

  /// \brief
  /// Loads the textures / shaders given in "ToLoad"
  ///
  /// \param ToLoad
  ///  The data to be loaded. 
  ///
  /// \param szPath
  ///  The base path from which to load the data.
  /// 
  /// \param bDoNotLoadImmediately
  ///  If set to true, the resources are allocated, but not immediately loaded (ie. texture files).
  /// 
  /// \param Progress
  ///   This object is advanced for each loaded texture, if provided.
  /// 
  /// \param fProgress
  ///   Together with fStep this defines how much to advance the progress object.
  /// 
  /// \param fStep
  ///   Together with fProgress this defines how much to advance the progress object.
  VISION_APIFUNC void LoadData (const VisSurface_DataToLoad& ToLoad, const char *szPath, bool bDoNotLoadImmediately, VProgressStatus* Progress = NULL, float fProgress = 0, float fStep = 0);


  /// \brief Returns how often this surface is referenced by other surfaces.
  VISION_APIFUNC int GetOtherSurfaceReferenceCounter (void) const { return (m_iReferencedByOthers); }

  /// \brief Resolves the pass type and stores the result internally.
  VISION_APIFUNC void ResolvePassType();

  /// \brief
  ///   Resolves the pass type with a given shader override and returns the result.
  ///
  /// \param pShader
  ///   The shader that should be used. If NULL, the internal shader will be used.
  /// 
  /// \return 
  ///   The resolved pass type.
  VISION_APIFUNC UBYTE ResolvePassType(const VCompiledShaderPass* pShader) const;

private:
  /// \brief Increases the internal counter of how often this surface is referenced by other surfaces.
  VISION_APIFUNC void AddReferenceByOtherSurface (void) { ++m_iReferencedByOthers; }

  /// \brief Decreases the internal counter of how often this surface is referenced by other surfaces.
  VISION_APIFUNC void RemoveReferenceByOtherSurface (void) { --m_iReferencedByOthers; }

private:
  VHashString m_sName;           ///< name of this surface
  VBaseMesh *m_pOwnerMesh;       ///< pointer to the base mesh that refers to this surface

  int   m_iMaterialFlags;        ///< casted to VSurfaceFlags_e (combinable)
  VColorRef m_AmbientColor;      ///< the ambient color of this surface
  
  UBYTE m_uiTransparencyType; 			///< casted to VIS_TransparencyType
  UBYTE m_uiSortingKey;          ///< internal sorting key; has to be in the range 0..15
  UBYTE m_uiLightingMethod; 	      ///< casted to VisLightingMethod_e
  UBYTE m_uiDeferredID;          ///< material ID that is written to G-Buffer in deferred rendering

  bool m_bCastStaticShadows;
  UBYTE m_uiPassType; 
  UBYTE m_uiResolvedPassType; ///< casted to VPassType_e, automatically detected pass type using force, transparency and shader settings
  bool m_bDoubleSided;
  bool m_bDepthWrite;
  UBYTE m_uiGeometryTopology;    ///< casted to VGeometryTopology_e

  float m_fSpecMul;     ///<specular multiplier
  float m_fSpecExp;     ///<specular exponent
  float m_fParallaxScale;  ///< parallax scale
  float m_fParallaxBias;   ///< parallax bias
  float m_fCustomAlphaThreshold; ///< custom alpha threshold; in case it is lower than 0, global alpha threshold is taken
  float m_fDepthBias;            ///< z-offset value that is passed to the shader
  float m_fDepthBiasClamp;       ///< clamped z-offset value that is passed to the shader
  float m_fSlopeScaledDepthBias; ///< slope dependent z-offset value that is passed to the shader

  int m_iUserFlags;              ///< customizable user flags
  int m_iReferencedByOthers;     ///< how often this surface is referenced by other surfaces

  VShaderMode_e m_eShaderMode;   ///< whether shader assignment is performed automatically or manually
  int m_iLightmapPageID;         ///< page-id of the used lightmap
  short m_iLightmapPageSize[2];  ///< size of the lightmap
  float m_fLightmapGranularity;  ///< granularity of the lightmap

  VString  m_sUserData;          ///< user data string set in editing tools (e.g. vEdit, Maya)
  VMaterialTemplateResourcePtr m_spTemplate;

public:

  // later:
#ifdef SUPPORTS_PHYSICS_MATERIALS
  // physics properties:
  unsigned int m_iCollisionBitmask;
  float m_fStaticFriction, m_fDynamicFriction, m_fRestitution;
#endif

  VCompiledEffectPtr m_spCurrentEffect;
  VShaderEffectResourcePtr m_spCurrentEffectResource; ///< owner effect
  VShaderEffectLibPtr m_spCurrentEffectLib; ///< owner lib

  VCompiledTechniquePtr m_spCurrentTechnique;
  VCompiledTechniquePtr m_spDynPointLight;
  VCompiledTechniquePtr m_spDynSpotLight;
  VCompiledTechniquePtr m_spDynProjPointLight;
  VCompiledTechniquePtr m_spDynDirectionalLight;

  VCompiledTechniquePtr m_spDynPointLightShadow;
  VCompiledTechniquePtr m_spDynSpotLightShadow;
  VCompiledTechniquePtr m_spDynProjPointLightShadow;
  VCompiledTechniquePtr m_spDynDirectionalLightShadow;

  VCompiledTechniquePtr m_spShadowmapFill;

  VCompiledTechniquePtr m_spMirror; 
  VCompiledTechniquePtr m_spMirrorSimple;
  VCompiledTechniquePtr m_spDepthFill; 

  VCompiledTechniquePtr m_spCustomTechniques[8];



public:
  /// \brief
  /// Returns the path of the MaterialLibrary that should override this surface.
  VISION_APIFUNC const char* GetOverrideLibraryName (void) const;

  /// \brief
  /// Returns the name of the material that should override this surface.
  VISION_APIFUNC const char* GetOverrideMaterialName (void) const;

  /// \brief
  /// Sets the path of the MaterialLibrary and the name of the material that should override this surface.
  VISION_APIFUNC bool SetMaterialOverride (bool bUpdateInstances, const char* szLibraryFileName, const char* szSurface = NULL);

  /// \brief
  /// Sets the MaterialLibrary and the name of the material that should override this surface.
  VISION_APIFUNC bool SetMaterialOverride (bool bUpdateInstances, VisSurfaceLibrary_cl* pLibrary, const char* szSurface = NULL);

  /// \brief
  ///   Returns a pointer to the override material of this surface, or this if no override material is used.
  VISION_APIFUNC VisSurface_cl* GetOverrideMaterial (void);

private:
  VisSurfaceLibraryPtr m_spOverrideLibrary;  ///< Smart Pointer to the Library that contains the material to override this one. Can be NULL.
  VisSurface_cl* m_pOverrideMaterial;        ///< Dumb Pointer to the Material that overrides this one. Can be NULL.


public:
  #if defined (HK_DEBUG) && defined (WIN32)
    void SetDefaultInclusionTags (const VTechniqueConfig& it) { m_DefaultInclusionTags = it;}
    const VTechniqueConfig& GetDefaultInclusionTags (void) const { return (m_DefaultInclusionTags); }
  #endif

private:
  #if defined (HK_DEBUG) && defined (WIN32)
    VTechniqueConfig m_DefaultInclusionTags;
  #endif
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inlines
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline VisSurfaceTextures_cl* VisSurfaceTextureSet_cl::GetTextures(int iIndex) const
{
  VASSERT(iIndex>=0 && iIndex<m_iSurfaceCount);
  return m_pSurfaceRefArray!=NULL ? m_pSurfaceRefArray[iIndex] : &m_pSurfaceTextureArray[iIndex];
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
