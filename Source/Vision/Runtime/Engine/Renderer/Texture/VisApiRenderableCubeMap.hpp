/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiRenderableCubeMap.hpp

#ifndef VIS_API_RENDERABLECUBEMAP_HPP_INCLUDED
#define VIS_API_RENDERABLECUBEMAP_HPP_INCLUDED

class VisRenderableCubeMap_cl;
typedef VSmartPtr<VisRenderableCubeMap_cl> VisRenderableCubeMapPtr;


/// \brief
///   Class for offscreen cube map render/depth stencil targets
/// 
/// A renderable cube map is a special type of cube map texture object: Unlike regular
/// VTextureCubeObjects, its faces (in DX10/11 also the complete cube) can be render or depth stencil
/// targets which can e.g. be passed to a VisRenderContext_cl object using the
/// VisRenderContext_cl::SetRenderTargetCubemap/SetDepthStencilTargetCubemap methods. Otherwise,
/// renderable cube map textures can be used just like normal cube maps.
/// 
/// Note that a renderable cube map texture that is the current render or depth stencil target can
/// not be bound as a texture at the same time (in other words, don't render a shader/scene
/// element which has the render target of the current render context as input texture).
/// 
/// A VisRenderableCubeMap_cl object has to be set up using the VisRenderableTextureConfig_t
/// structure.
/// 
/// On Xbox360, a VisRenderableCubeMap_cl allocates memory in the internal EDRAM buffer as a render
/// target. In order to set up data for predicated tiling, use the appropriate members of the
/// VisRenderableTextureConfig_t structure.
/// 
/// VisRenderableTextureConfig_t::m_bRenderTargetOnly can be used to specify whether a
/// VisRenderableCubeMap_cl is intended to be used as a a texture, or whether it is simply a
/// temporary surface. The latter is often desired for depth-stencil targets, for instance, and
/// typically consumes less memory.
/// 
/// VisRenderableTextureConfig_t::m_bResolve is useful e.g. for multisampled render targets which
/// can't be directly used as textures on many platforms. VisRenderableCubeMap_cl objects created
/// with m_bResolve set to true will internally create a non-multisampled resolve target for the
/// texture. If the associated render context uses the automatic resolve feature, the Vision Engine
/// will handle all necessary resolve operations for you. Note that there may be limitations
/// regarding resolve operations depending on the graphics hardware.
/// 
/// m_bResolve is enabled by default for Xbox360, since this platform can't render
/// directly to a texture.
/// 
/// Using VisRenderableTextureConfig_t::m_bIsDepthStencilTarget and an appropriate surface format,
/// you can specify whether the VisRenderableCubeMap_cl is to be used as a color or depth stencil
/// target.
/// 
/// In DX10/11, you can use the VisRenderableTextureConfig_t::m_bRenderTargetArray member to specify
/// whether the renderable cube map is to be used as a render target array, or a set of six
/// separate 2D render targets. The former is useful if you e.g. want to select the render target
/// in the geometry shader.
class VisRenderableCubeMap_cl : public VTextureCubeObject
{
public:

  /// \brief
  ///   Constructor of the renderable cube map class
  /// 
  /// Creates a new renderable cube map based on the data stored in the
  /// VisRenderableTextureConfig_t structure. 
  /// 
  /// \param pParentManager
  ///   Resource Manager this texture will be registered with
  /// 
  /// \param config
  ///   Renderable texture configuration for this texture object
  ///
  /// \param szFileName
  ///   Optional resource name for this texture object
  VISION_APIFUNC VisRenderableCubeMap_cl(VResourceManager *pParentManager, 
    VisRenderableTextureConfig_t &config, const char* szFileName=NULL);

  /// \brief
  ///   Destructor of the VisRenderableCubeMap_cl class
  VISION_APIFUNC ~VisRenderableCubeMap_cl();

  /// \brief
  ///   Returns the configuration settings of this renderable texture object.
  /// 
  /// \return
  ///   VisRenderableTextureConfig_t *pConfig: Pointer to configuration settings of this
  ///   renderable texture object
  VISION_APIFUNC const VisRenderableTextureConfig_t *GetConfig() { return &m_config; }

  /// \brief
  ///   Overridden function; see base class VTextureObject.
  VOVERRIDE VISION_APIFUNC bool IsRenderable() const;

  /// \brief
  ///   Performs the resolve operation for this renderable cube map.
  /// 
  /// For renderable cube maps created with m_bResolve=true, this method resolves the (typically
  /// multisampled) render target to the internal (non-multisampled) target texture.
  /// 
  /// Note that in order for the resolve operation to function properly, the renderable cube map
  /// must be set as one of the render targets in the currently active render context.
  /// 
  /// \param iFace
  ///   cube map face to resolve
  VISION_APIFUNC void Resolve(int iFace);

  /// \brief
  ///   Returns the format for the resolved texture.
  ///
  /// This is the format used when sampling the renderable texture.
  inline VTextureLoader::VTextureFormat_e GetResolvedTextureFormat() const
  {
#if defined(_VISION_XENON)
    return m_eResolvedTextureFormat;
#else
    return GetTextureFormat();
#endif
  }

#ifdef _VISION_XENON

  /// \brief
  ///   Performs the resolve operation for this renderable cube map.
  /// 
  /// Performs the resolve operation for this renderable cube map (Xbox360 version).
  /// 
  /// For renderable cube maps created with m_bResolve=true, this method resolves the (typically
  /// multisampled) render target to the internal (non-multisampled) target texture.
  /// 
  /// For predicated tiling, this method provides a D3DRECT structure specifying the tiling rect to
  /// resolve.
  /// 
  /// Note that in order for the resolve operation to function properly, the renderable cube map
  /// must be set as one of the render targets in the currently active render context.
  /// 
  /// \param iFace
  ///   cube map face to resolve
  /// 
  /// \param tile
  ///   Tile to resolve
  /// 
  /// \param pDestObject
  ///   If non-NULL, this specifies the target texture to resolve to. Otherwise, the render target contents are
  ///   resolved to the renderable texture's internal resolve target.
  VISION_APIFUNC void Resolve(int iFace, const D3DRECT &tile, VTextureCubeObject *pDestObject = NULL);

#endif


  // Methods for internal use:

  VOVERRIDE BOOL Unload();
  VOVERRIDE BOOL Reload();
  VOVERRIDE void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, 
    size_t &iDependentSys, size_t &iDependentGPU);
  VISION_APIFUNC VisRenderableCubeMap_cl(VResourceManager *pParentManager, const char* szFileName=NULL);
  VISION_APIFUNC void Init(VisRenderableTextureConfig_t *pConfig);
  inline bool HasValidConfig() const {return m_bHasValidConfig;}

#ifdef _VR_DX9
  VISION_APIFUNC IDirect3DSurface9 *GetRenderTarget(int iCubeMapFace) const;
  D3DFORMAT m_BufferFormat;
  IDirect3DSurface9 *m_pSurface[6];
  #if defined(_VISION_XENON)
    VTextureLoader::VTextureFormat_e m_eResolvedTextureFormat;
  #endif

#elif defined(_VR_DX11)
  VISION_APIFUNC D3DView *GetRenderTarget(int iCubeMapFace) const;
  DXGI_FORMAT m_BufferFormat;
  D3DRenderTargetViewPtr m_spD3DRenderTargetView[6];
  D3DDepthStencilViewPtr m_spD3DDepthStencilView[6];
  D3DResourcePtr m_spD3DResolvedResource;
  bool m_bUsing2DTexture;

#elif defined(_VR_GCM)
  VISION_APIFUNC const CellGcmTexture *GetRenderTarget() const;

  CellGcmTexture m_RenderTargetGcmTexture;
  VGcmMemory m_RenderTargetGcmMemory;

  bool m_bHasTile;
  bool m_bIsBoundToTile;
  unsigned short m_iTileTagBase;
  unsigned char m_iTileIndex;
  signed char m_iZCullIndex;


#elif defined(_VR_GXM)
  VISION_APIFUNC SceGxmTexture* GetRenderTarget();
  SceGxmColorSurface m_ColorSurface[6];
  SceGxmDepthStencilSurface m_DepthStencilSurface[6];

  VISION_APIFUNC SceGxmColorSurface *GetColorSurface(int iCubeMapFace)
  {
    return &m_ColorSurface[iCubeMapFace];
  }

  VISION_APIFUNC SceGxmDepthStencilSurface *GetDepthStencilSurface(int iCubeMapFace)
  {
    return &m_DepthStencilSurface[iCubeMapFace];
  }

#elif defined(_VR_OPENGL)
  VISION_APIFUNC unsigned int GetRenderTarget() const;
  unsigned int m_iBufferHandle;
  GLenum m_BufferFormat;

#elif defined(_VR_GX2)

  mutable GX2ColorBuffer m_ColorBuffer[6];
  mutable GX2DepthBuffer m_DepthStencilBuffer[6];

  void* m_pGX2ColorBufferPtr;
  void* m_pGX2ColorBufferAuxPtr;

  void* m_pGX2DepthBufferPtr;
  void* m_pGX2DepthBufferHiZPtr;

  GX2SurfaceFormat m_GX2SurfaceFormat;

  inline VISION_APIFUNC GX2SurfaceFormat GetSurfaceFormat() const
  {
    return m_GX2SurfaceFormat;
  }

  inline VISION_APIFUNC GX2ColorBuffer* GetColorBuffer(int iCubeMapFace) const
  {
    return &m_ColorBuffer[iCubeMapFace];
  }

  inline VISION_APIFUNC GX2DepthBuffer* GetDepthStencilBuffer(int iCubeMapFace) const
  {
    return &m_DepthStencilBuffer[iCubeMapFace];
  }

#endif

private:
  VisRenderableTextureConfig_t m_config;
  bool m_bSeparateDepthStencilBuffers;
  bool m_bHasValidConfig;

protected:
  friend class VisRenderContext_cl;
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
