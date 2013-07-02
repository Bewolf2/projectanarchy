/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiRenderableTexture.hpp

#ifndef VIS_CORE_PBUFFER_HPP_INCLUDED
#define VIS_CORE_PBUFFER_HPP_INCLUDED

class VisRenderableTexture_cl;
typedef VSmartPtr<VisRenderableTexture_cl> VisRenderableTexturePtr;

#if defined(_VR_DX11)
  typedef VSmartPtr<D3DRenderTargetView> D3DRenderTargetViewPtr;
  typedef VSmartPtr<D3DDepthStencilView> D3DDepthStencilViewPtr;
#endif

/// \brief
///   Class for offscreen 2D render/depth stencil targets
/// 
/// A renderable texture is a special type of texture object: Unlike regular VTextureObjects, it
/// can be a render target or depth stencil target that can e.g. be passed to a VisRenderContext_cl
/// object using the VisRenderContext_cl::SetRenderTarget method. Otherwise, renderable textures
/// can be used just like normal textures.
/// 
/// Note that a renderable texture that is the current render or depth stencil target can not be
/// bound as a texture at the same time (in other words, don't render a shader/scene elementh which
/// has the render target of the current render context as input texture).
/// 
/// A VisRenderableTexture_cl object has to be set up using the VisRenderableTextureConfig_t
/// structure.
/// 
/// On Xbox360, a VisRenderableTexture_cl allocates memory in the internal EDRAM buffer as a render
/// target. In order to set up data for predicated tiling, use the appropriate members of the
/// VisRenderableTextureConfig_t structure.
/// 
/// VisRenderableTextureConfig_t::m_bRenderTargetOnly can be used to specify whether a
/// VisRenderableTexture_cl is intended to be used as a a texture, or whether it is simply a
/// temporary surface. The latter is often desired for depth-stencil targets, for instance.
/// 
/// VisRenderableTextureConfig_t::m_bResolve is useful e.g. for multisampled render targets which
/// can't be directly used as textures on many platforms. VisRenderableTexture_cl objects created
/// with m_bResolve set to true will internally create a non-multisampled resolve target for the
/// texture. If the associated render context uses the automatic resolve feature, the Vision Engine
/// will handle all necessary resolve operations for you. Note that there may be limitations
/// regarding resolve operations depending on the graphics hardware. 
/// 
/// m_bResolve is enabled by default for Xbox360, since these platforms can't render
/// directly to a texture.
/// 
/// Using m_bIsDepthStencilTarget and an appropriate surface format, you can specify whether the
/// VisRenderableTexture_cl is to be used as a color or depth stencil target.
class VisRenderableTexture_cl : public VTextureObject
{
public:

  /// \brief
  ///   Constructor of the renderable texture class
  /// 
  /// Creates a new renderable texture based on the data stored in the VisRenderableTextureConfig_t
  /// structure. 
  /// 
  /// \param pParentManager
  ///   Resource Manager this texture will be registered with
  /// 
  /// \param config
  ///   Renderable texture configuration for this texture object
  ///
  /// \param szFileName
  ///   Optional resource name for this texture object
  VISION_APIFUNC VisRenderableTexture_cl(VResourceManager *pParentManager, VisRenderableTextureConfig_t &config, const char* szFileName=NULL);

  /// \brief
  ///   Destructor of the VisRenderableTexture_cl class
  VISION_APIFUNC ~VisRenderableTexture_cl();

  /// \brief
  ///   Returns the configuration settings of this renderable texture object.
  /// 
  /// \return
  ///   VisRenderableTextureConfig_t *pConfig: Pointer to configuration settings of this
  ///   renderable texture object
  VISION_APIFUNC const VisRenderableTextureConfig_t *GetConfig() const { return &m_config; }

  /// \brief
  ///   Returns the configuration settings of this renderable texture object.
  /// 
  /// \return
  ///   VisRenderableTextureConfig_t *pConfig: Pointer to configuration settings of this
  ///   renderable texture object
  VISION_APIFUNC VisRenderableTextureConfig_t *GetConfig() { return &m_config; }

  /// \brief
  /// \brief
  ///   Overridden function; see base class VTextureObject.
  virtual VISION_APIFUNC bool IsRenderable() const;

  /// \brief
  ///   Performs the resolve operation for this renderable texture.
  /// 
  /// For renderable textures created with m_bResolve=true, this method resolves the (typically
  /// multisampled) render target to the internal (non-multisampled) target texture.
  /// 
  /// Note that in order for the resolve operation to function properly, the renderable texture
  /// must be set as one of the render targets in the currently active render context.
  VISION_APIFUNC void Resolve();

  /// \brief
  ///   Returns the format for the resolved texture.
  ///
  /// This is the format used when sampling the renderable texture.
  inline VTextureLoader::VTextureFormat_e GetResolvedTextureFormat() const
  {
#if defined(_VISION_XENON)
    return static_cast<VTextureLoader::VTextureFormat_e>(m_eResolvedTextureFormat);
#else
    return GetTextureFormat();
#endif
  }

#ifdef _VISION_XENON

  /// \brief
  ///   Performs the resolve operation for this renderable texture (Xbox360 version).
  /// 
  /// For renderable textures created with m_bResolve=true, this method resolves the (typically
  /// multisampled) render target to the internal (non-multisampled) target texture.
  /// 
  /// For predicated tiling, this method provides a D3DRECT structure specifying the tiling rect to
  /// resolve.
  /// 
  /// Note that in order for the resolve operation to function properly, the renderable texture
  /// must be set as one of the render targets in the currently active render context.
  ///
  /// \param tile
  ///   A D3DRECT structure specifying the tiling rect to resolve.
  ///
  /// \param pDestObject
  ///   If non-NULL, this specifies the target texture to resolve to. Otherwise, the render target contents are
  ///   resolved to the renderable texture's internal resolve target.
  VISION_APIFUNC void Resolve(const D3DRECT &tile, VTextureObject *pDestObject = NULL);

  /// \brief
  ///   Returns the size in EDRAM tiles allocated for a surface when using predicated tiling.
  ///
  /// \param config
  ///   The respective render target configuration.
  VISION_APIFUNC static int GetPredicatedEDRAMTileSize(const VisRenderableTextureConfig_t &config);

#endif


  /// \brief
  ///   Returns the size of the renderable texture in ZCull/HiZ units (Playstation 3, Xbox360).
  unsigned int GetHiZSize();

  // Methods for internal use:

  VISION_APIFUNC virtual BOOL Unload() HKV_OVERRIDE;
  VISION_APIFUNC virtual BOOL Reload() HKV_OVERRIDE;
  VISION_APIFUNC virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU) HKV_OVERRIDE;

  static int CalculateSize(const VTextureObject *pTex, const VisRenderableTextureConfig_t &config);

#if defined(_VR_DX9)
  VISION_APIFUNC IDirect3DSurface9 *GetRenderTarget() const;
  static D3DMULTISAMPLE_TYPE GetMultisampleType(int iMS);

  IDirect3DSurface9 *m_pSurface;
  D3DFORMAT m_BufferFormat;
  #if defined(_VISION_XENON)
    VTextureLoader::VTextureFormat_e m_eResolvedTextureFormat;
  #endif

#elif defined(_VR_DX11)
  VISION_APIFUNC D3DView *GetRenderTarget();
  static DXGI_SAMPLE_DESC GetMultisampleType(int iMS, int iQuality = 0);

  DXGI_FORMAT m_BufferFormat;
  D3DRenderTargetViewPtr m_spD3DRenderTargetView;
  D3DDepthStencilViewPtr m_spD3DDepthStencilView;
  D3DResourcePtr m_spD3DResolvedResource;

#elif defined(_VR_OPENGL) || defined(_VR_GLES2)
  static VVIDEO_Multisample GetMultisampleType(int iMS);
  static GLenum GetGLMultisampleType(int iMS);
  static int GetXScale(int iMS);
  static int GetYScale(int iMS);

  VISION_APIFUNC unsigned int GetRenderTarget() const;

  unsigned int m_GLTargetHandle;
  
#elif defined(_VR_GCM)

  VISION_APIFUNC static unsigned int GetRequiredSize(const CellGcmTexture &refTex);
  VISION_APIFUNC static unsigned int CountMipMaps(const CellGcmTexture &refTex);

  static VVIDEO_Multisample GetMultisampleType(int iMS);

  VISION_APIFUNC const CellGcmTexture *GetRenderTarget() const;

  static unsigned int  GetGCMMultisampleType(int iMS);
  static unsigned char GetCompressionMode(const VisRenderableTextureConfig_t &config);
  static unsigned int  GetZCullFormat(const VisRenderableTextureConfig_t &config);

  CellGcmTexture m_RenderTargetGcmTexture;
  VGcmMemory m_RenderTargetGcmMemory;

  bool m_bHasTile;
  bool m_bIsBoundToTile;
  unsigned short m_iTileTagBase;
  unsigned char m_iTileIndex;
  signed char m_iZCullIndex;

#elif defined(_VR_GXM)

  SceGxmColorSurface m_ColorSurface;
  SceGxmDepthStencilSurface m_DepthStencilSurface;

  SceUID m_UID;

  VISION_APIFUNC SceGxmTexture* GetRenderTarget();

  VISION_APIFUNC SceGxmColorSurface* GetColorSurface()
  {
    return &m_ColorSurface;
  }

  VISION_APIFUNC SceGxmDepthStencilSurface* GetDepthStencilSurface()
  {
    return &m_DepthStencilSurface;
  }

#elif defined(_VR_GX2)
    
  mutable GX2ColorBuffer m_ColorBuffer;

  mutable GX2DepthBuffer m_DepthStencilBuffer;

  mutable GX2Texture m_Texture;

  mutable GX2ColorBuffer* m_pExistingColorBuffer;

  void* m_pGX2ColorBufferPtr;
  void* m_pGX2ColorBufferAuxPtr;

  void* m_pGX2DepthBufferPtr;
  void* m_pGX2DepthBufferHiZPtr;

  GX2SurfaceFormat m_GX2SurfaceFormat;

  bool m_bCreatedFromExistingBuffer;

  static VVIDEO_Multisample GetMultisampleType(int iMS);

  inline VISION_APIFUNC GX2SurfaceFormat GetSurfaceFormat() const
  {
    return m_GX2SurfaceFormat;
  }


  inline VISION_APIFUNC GX2ColorBuffer* GetColorBuffer() const
  {
    if(m_bCreatedFromExistingBuffer)
      return m_pExistingColorBuffer;
    else
      return &m_ColorBuffer;
  }

  inline VISION_APIFUNC GX2DepthBuffer* GetDepthStencilBuffer() const
  {
    return &m_DepthStencilBuffer;
  }

  inline VISION_APIFUNC GX2Texture* GetRenderTarget() const
  {
    return &m_Texture;
  }

private:

  friend class VisRenderer_cl;

  VisRenderableTexture_cl(VResourceManager *pParentManager, GX2ColorBuffer* pExistingColorBuffer, const char* szFileName);

  VisRenderableTexture_cl(VResourceManager *pParentManager, GX2DepthBuffer* pExistingDepthBuffer, const char* szFileName);


#endif

private:
  VisRenderableTextureConfig_t m_config;

protected:

  // Protected members ...

#if defined (_VR_DX9) || defined (_VR_DX11) && !defined (_VISION_XENON)
  HANDLE m_hShared;
  public:
    HANDLE GetSharedHandle() { return m_hShared; }
  protected:
#endif

#ifdef _VISION_XENON
  static void GetFinalTileSize(const VisRenderableTextureConfig_t &config, int &iWidth, int &iHeight);
  static void GetLargestTileRectSize( const VisRenderableTextureConfig_t &config, D3DPOINT* pLargestTile );
#endif


  friend class VisRenderContext_cl;
  friend class VisRenderableCubeMap_cl;
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
