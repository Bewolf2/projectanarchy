/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiRenderableTexture2DArray.hpp

#ifndef VIS_API_RENDERABLETEXTURE2DARRAY_HPP_INCLUDED
#define VIS_API_RENDERABLETEXTURE2DARRAY_HPP_INCLUDED

class VisRenderableTexture2DArray_cl;
typedef VSmartPtr<VisRenderableTexture2DArray_cl> VisRenderableTexture2DArrayPtr;


/// \brief
///   Class for offscreen 2D render/depth stencil target arrays
/// 
/// A renderable texture array is a special type of texture array: Unlike regular
/// VTexture2DArrayObjects, it can be a render target or depth stencil target that can e.g. be
/// passed to a VisRenderContext_cl object using the VisRenderContext_cl::SetRenderTarget method.
/// Otherwise, renderable texture arrays can be used just like normal texture arrays.
/// 
/// Note that a renderable texture array that is the current render or depth stencil target can not
/// be bound as a texture at the same time (in other words, don't render a shader/scene elementh
/// which has the render target of the current render context as input texture).
/// 
/// A VisRenderableTexture2DArray_cl object has to be set up using the VisRenderableTextureConfig_t
/// structure.
/// 
/// The VisRenderableTextureConfig_t::m_iArraySize property specifies the number of 2D slices
/// (array elements) in the texture.
/// 
/// On Xbox360, a VisRenderableTexture2DArray_cl allocates memory in the internal EDRAM buffer as a
/// render target. In order to set up data for predicated tiling, use the appropriate members of
/// the VisRenderableTextureConfig_t structure.
/// 
/// VisRenderableTextureConfig_t::m_bRenderTargetOnly can be used to specify whether a
/// VisRenderableTexture2DArray_cl is intended to be used as a a texture, or whether it is simply a
/// temporary surface. The latter is often desired for depth-stencil targets, for instance.
/// 
/// VisRenderableTextureConfig_t::m_bResolve is useful e.g. for multisampled render targets which
/// can't be directly used as textures on many platforms. VisRenderableTexture2DArray_cl objects
/// created with m_bResolve set to true will internally create a non-multisampled resolve target
/// for the texture. If the associated render context uses the automatic resolve feature, the
/// Vision Engine will handle all necessary resolve operations for you. Note that there may be
/// limitations regarding resolve operations depending on the graphics hardware.
/// 
/// m_bResolve is enabled by default for Xbox360, since these platforms can't render
/// directly to a texture.
/// 
/// Using m_bIsDepthStencilTarget and an appropriate surface format, you can specify whether the
/// VisRenderableTexture2DArray_cl is to be used as a color or depth stencil target.
class VisRenderableTexture2DArray_cl : public VTexture2DArrayObject
{
public:

  /// \brief
  ///   Constructor of the renderable texture array class
  /// 
  /// Creates a new renderable texture array based on the data stored in the
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
  VISION_APIFUNC VisRenderableTexture2DArray_cl(VResourceManager *pParentManager, VisRenderableTextureConfig_t &config, const char* szFileName=NULL);

  /// \brief
  ///   Destructor of the VisRenderableCubeMap_cl class
  VISION_APIFUNC ~VisRenderableTexture2DArray_cl();

  /// \brief
  ///   Returns the configuration settings of this renderable texture array object.
  /// 
  /// \return
  ///   VisRenderableTextureConfig_t *pConfig: Pointer to configuration settings of this
  ///   renderable texture array object
  VISION_APIFUNC const VisRenderableTextureConfig_t *GetConfig() { return &m_config; }

  /// \brief
  ///   Overridden function; see base class VTextureObject.
  virtual VISION_APIFUNC bool IsRenderable() const;

  /// \brief
  ///   Performs the resolve operation for this renderable texture array.
  /// 
  /// For renderable texture arrays created with m_bResolve=true, this method resolves the
  /// (typically multisampled) render target to the internal (non-multisampled) target texture.
  /// 
  /// Note that in order for the resolve operation to function properly, the renderable texture
  /// array must be set as one of the render targets in the currently active render context.
  /// 
  /// \param iArrayIndex
  ///   texture array index to resolve
  VISION_APIFUNC void Resolve(int iArrayIndex);


  // Methods for internal use:
  virtual BOOL Unload();
  virtual BOOL Reload();
  virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);
  VISION_APIFUNC VisRenderableTexture2DArray_cl(VResourceManager *pParentManager, const char* szFileName=NULL);
  VISION_APIFUNC void Init(VisRenderableTextureConfig_t *pConfig);
  inline bool HasValidConfig() const {return m_bHasValidConfig;}

  VISION_APIFUNC D3DView *GetRenderTarget() const;
  DXGI_FORMAT m_BufferFormat;

private:
  VisRenderableTextureConfig_t m_config;
  bool m_bSeparateDepthStencilBuffers;
  bool m_bHasValidConfig;

protected:

  D3DRenderTargetViewPtr m_spD3DRenderTargetView;
  D3DDepthStencilViewPtr m_spD3DDepthStencilView;

  D3DResourcePtr m_spD3DResolvedResource;

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
