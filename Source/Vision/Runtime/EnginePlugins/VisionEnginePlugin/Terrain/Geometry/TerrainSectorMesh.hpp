/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainSectorMesh.hpp

#ifndef TERRAINSECTORMESH_HPP_INCLUDED
#define TERRAINSECTORMESH_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>

class VTerrainSector;
class VTerrainVisibilityInfo;
struct VSectorMeshRenderRange_t;


#ifndef _VISION_DOC
struct VTerrainVertexLowRes_t
{
  hkvVec3 vPos;
  static const VisMBVertexDescriptor_t& GetDesc();

private:
  static VisMBVertexDescriptor_t g_Desc;
};
#endif // _VISION_DOC


/// \brief
///   Holds the sector specific mesh and data associated with it
class VTerrainSectorMeshBuffer : public VisMeshBuffer_cl
{
public:
  VTerrainSectorMeshBuffer() : VisMeshBuffer_cl()
  {
    m_pOwnerSector = NULL;
    m_iMeshStreamMask = 0;
  }

  VTerrainSector *m_pOwnerSector;

  // streams defined by this mesh
  unsigned int m_iMeshStreamMask;

private:
  // The following properties have been moved to page granularity and have been hidden as private here to avoid accidental use.
  // Since these are non-virtual overrides, the actual methods can still be accessed by casting to the base class.

  void SetChannelTexture(VTextureObject *pTextureObject, int iChannel)
  {
    VASSERT_MSG(false, "The terrain sector mesh's channel textures shouldn't be used - did you mean to use the channel textures of one of the sector's pages instead?");
  }

  VTextureObject* SetChannelTexture(const char *szFilename, int iChannel)
  {
    VASSERT_MSG(false, "The terrain sector mesh's channel textures shouldn't be used - did you mean to use the channel textures of one of the sector's pages instead?");
    return NULL;
  }

  VTextureObject* GetChannelTexture(int iChannel) const
  {
    VASSERT_MSG(false, "The terrain sector mesh's channel textures shouldn't be used - did you mean use to the channel textures of one of the sector's pages instead?");
    return NULL;
  }

  VCompiledTechnique *GetDefaultTechnique() const
  {
    VASSERT_MSG(false, "The terrain sector mesh's default technique shouldn't be used - did you mean use to the technique of one of the sector's pages instead?");
    return NULL;
  }
  
  void SetDefaultTechnique(VCompiledTechnique *pTechnique)
  {
    VASSERT_MSG(false, "The terrain sector mesh's default technique shouldn't be used - did you mean use to the technique of one of the sector's pages instead?");
  }
};

// VTerrainSectorMeshPageInfo::m_iDirtyFlags
#define MESHPAGEFLAG_HEIGHTMAP_DIRTY  V_BIT(0)
#define MESHPAGEFLAG_NORMALMAP_DIRTY  V_BIT(1)


/// \brief
///   Represents a info for a single draw call in a sector. In case the sector is too large to be rendered as a single mesh, the sector holds an array of n x m instances of this class
///
/// A sector draw call usually renders a page of 128x128 height samples. Accordingly, if the terrain has been created with 512x512 height samples per sector,
/// a sector holds an array of 4x4 instances of VTerrainSectorMeshPageInfo. Visibility and LOD is calculated independently on each of the draw calls through the
/// VTerrainVisibilityCollectorComponent component. Note that the engine itself computes visibility only based on VisStaticGeometryInstance_cl, which is a full terrain sector.
///
/// Detail textures are baked on a page granularity, so each page may be rendered using a different set of detail textures, even though all pages in a sector use different regions 
/// of the same weightmap texture.
class VTerrainSectorMeshPageInfo
{
public:
  /// Constructor.
  TERRAIN_IMPEXP VTerrainSectorMeshPageInfo();

  /// Destructor.
  TERRAIN_IMPEXP ~VTerrainSectorMeshPageInfo();

  TERRAIN_IMPEXP void Init(VTerrainSector *pOwner, int x, int y, bool bFirstTime);
  TERRAIN_IMPEXP void PerformVisibility(VTerrainVisibilityCollectorComponent &infoComp, const VTerrainConfig &cfg);
  TERRAIN_IMPEXP void SerializeX(VTerrainSector *pOwner, VArchive &ar, int iLocalVersion);

  TERRAIN_IMPEXP void UpdateHoleMask(VTerrainSector *pOwner, int px, int py);

  // Shader related methods

  /// Locks the pixel shader constant buffer for writing.
  hkvVec4* LockPixelConst();

  /// Unlocks the pixel shader constant buffer.
  void UnLockPixelConst();

  /// Binds the textures referenced by this mesh page to the given shader.
  TERRAIN_IMPEXP void BindSectorShaderTextures(VCompiledShaderPass *pShader,unsigned int iSamplerClampMask);

  /// Retrieves the sampler index for a given sampler name.
  TERRAIN_IMPEXP int GetSamplerIndex(const char *szSamplerName) const;

  /// Retrieves the number of weightmaps supported by the current technique.
  TERRAIN_IMPEXP int GetWeightMapSamplerCount() const;

  /// \brief Modifies a UV transform to revert the UV-clamping used for weightmaps.
  ///
  /// As the components of each weightmap may have different meanings between different pages,
  /// the sector UVs are  slighly offset to avoid sampling the outer half pixel of each weightmap region.
  ///
  /// Other UV coordinates (e.g. for lightmaps) which should be computed from the unclamped sector UV coordinates
  /// (as sectorUV.xy * uvTransfrom.xy + uvTransform.zw) can be computed from the clamped sector UV coordinates
  /// by "undoing" the offset using this method, eg.:
  ///
  ///   *lightmapTransformRegister = AdjustTransformForUnclampedUVs(sectorToLightmapUV);
  ///
  /// \param uvTransform
  ///  The sector UV transformation that should be adjusted to account for the clamping.
  TERRAIN_IMPEXP hkvVec4 AdjustTransformForUnclampedUVs(hkvVec4 uvTransform) const;

  /// Updates the constant register used for transforming sector UV to normalmap UV coordinates.
  TERRAIN_IMPEXP void UpdateNormalmapRegister(hkvVec4* pPixelConst);


  // Texture channel related properties

  /// Lazily creates a VisSurface_cl object used for holding the technique used for rendering the sector's mesh buffer.
  TERRAIN_IMPEXP VisSurface_cl& GetSurfaceSafe();

  /// Sets one of the channel textures used for rendering the sector mesh.
  void SetTextureBySamplerIndex(VTextureObject *pTextureObject, unsigned int iSamplerIdx)
  {
    VASSERT(iSamplerIdx < V_ARRAY_SIZE(m_spSamplerTexture));

    m_spSamplerTexture[iSamplerIdx] = pTextureObject;
  }

  /// Returns one of the textures used for rendering the sector mesh.
  inline VTextureObject* GetTextureBySamplerIndex(unsigned int iSamplerIdx) const
  {
    VASSERT(iSamplerIdx < V_ARRAY_SIZE(m_spSamplerTexture));

    return m_spSamplerTexture[iSamplerIdx];
  }

  /// Returns the sampler index of one of the textures used for rendering the sector mesh.
  inline int GetSamplerIndexByTexture(VTextureObject* pTexture)
  {
    for(int iSamplerIdx = 0; iSamplerIdx < V_ARRAY_SIZE(m_spSamplerTexture); iSamplerIdx++)
    {
      if(m_spSamplerTexture[iSamplerIdx] == pTexture)
      {
        return iSamplerIdx;
      }
    }

    return -1;
  }

  /// Sets one of the detail textures used for rendering the sector mesh, and updates the pixel shader registers.
  TERRAIN_IMPEXP void SetDetailTexture(int iSampler, int iTexCoordStream, VTextureObject *pTex, const hkvVec4& vTilingAndOfs, bool bUseClamping);

  /// Sets one of the lightmap textures used for rendering the sector mesh.
  inline void SetLightmapTexture(int iSampler, VTextureObject *pTex, int iSubIndex)
  {
    SetTextureBySamplerIndex(pTex, iSampler);
    m_iLightmapSamplerIndex[iSubIndex] = iSampler;
  }

  /// The terrain sector owning this mesh page.
  VTerrainSector* m_pOwner;
  
  int m_iGlobalIndex; ///< the global index in the whole terrain
  hkvAlignedBBox m_AbsBoundingBox;

#if defined(_VR_DX11)
  // DX10/11 store a pre-compiled shader constant buffer
  VShaderConstantBuffer m_PageVSConstBuffer;
#else
  // maps to vertex shader register 38..x
  // #38: worldspace vertex position transformation, 
  // #39: UV transformation (global space)
  // #40: UV transformation (sector 0..1)
  hkvVec4 m_vVertexShaderConst[3];
#endif

  int m_iPixelShaderRegCount;
  unsigned int m_iSamplerClampMask; ///< each bit represents a sampler intex to clamp
#if defined(_VR_DX11)
  // DX10/11 store a pre-compiled shader constant buffer
  void ReallocPixelBuffer();
  VShaderConstantBuffer m_SectorPSConstBuffer;
#else
  // maps to pixel shader register 2..x
  // #2..#17: detail texture tilings
  hkvVec4 m_vPixelShaderConst[16];
#endif

  float m_fLODThreshold[MAX_MESH_LOD-1];
  bool m_bHasHoles,m_bAllHoles;
  UBYTE m_iDirtyFlags; // MESHPAGEFLAG_HEIGHTMAP_DIRTY and MESHPAGEFLAG_NORMALMAP_DIRTY

  VSectorMeshLODLevelInfo_t m_HoleLODInfo[MAX_MESH_LOD]; ///< in case it has holes
  VisMeshBufferPtr m_spHoleIndexBuffer; ///< replacement index buffer in case this page has holes.

  /// The number of detail layers rendered using three way mapping.
  int m_iNumThreeWayMappingLayer;

  /// A bit mask indicating the layer indices rendered using three way mapping. Used for debugging purposes.
  int m_iThreeWayMappingLayerMask;

  /// The surface holding the technique for rendering the sector mesh.
  VisSurface_cl* m_pSurface;

  signed char m_iLightmapSamplerIndex[4]; ///< for all 4 lightmap types this is the index where the lightmap is stored.

  short m_MaterialIDs[9]; ///< material IDs exposed from vForge in same order as texture layers

private:
  /// Reverse transform used by AdjustTransformForUnclampedUVs().
  hkvVec4 m_unclampUVTransform;

  VTextureObjectPtr m_spSamplerTexture[16];
};


typedef VSmartPtr<VTerrainSectorMeshBuffer> VTerrainSectorMeshBufferPtr;

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
