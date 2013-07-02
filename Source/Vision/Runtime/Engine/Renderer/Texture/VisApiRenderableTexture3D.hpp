/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiRenderableTexture3D.hpp

#ifndef VIS_API_RENDERABLETEXTURE3D_HPP_INCLUDED
#define VIS_API_RENDERABLETEXTURE3D_HPP_INCLUDED

class VisRenderableTexture3D_cl;
typedef VSmartPtr<VisRenderableTexture3D_cl> VisRenderableTexture3DPtr;



/// \brief
///   Class for offscreen 3D render targets (UNSUPPORTED).
/// 
/// Currently not supported.
class VisRenderableTexture3D_cl : public VTexture3DObject
{
public:

  /// \brief
  ///   Constructor of the renderable 3D texture class
  /// 
  /// Creates a new renderable 3D texture based on the data stored in the
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
  VISION_APIFUNC VisRenderableTexture3D_cl(VResourceManager *pParentManager, VisRenderableTextureConfig_t &config, const char* szFileName=NULL);

  /// \brief
  ///   Destructor of the VisRenderableCubeMap_cl class
  VISION_APIFUNC ~VisRenderableTexture3D_cl();

  /// \brief
  ///   Returns the configuration settings of this renderable texture object.
  /// 
  /// \return
  ///   VisRenderableTextureConfig_t *pConfig: Pointer to configuration settings of this
  ///   renderable texture object
  VISION_APIFUNC const VisRenderableTextureConfig_t *GetConfig() { return &m_config; }

  /// \brief
  ///   Overridden function; see base class VTextureObject.
  virtual VISION_APIFUNC bool IsRenderable() const;


  // Methods for internal use:
  virtual BOOL Unload();
  virtual BOOL Reload();
  virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);
  VISION_APIFUNC VisRenderableTexture3D_cl(VResourceManager *pParentManager, const char* szFileName=NULL);
  VISION_APIFUNC void Init(VisRenderableTextureConfig_t *pConfig);
  inline bool HasValidConfig() const {return m_bHasValidConfig;}

  VISION_APIFUNC void Resolve();

  /// \brief
  ///   Returns the format for the resolved texture.
  ///
  /// This is the format used when sampling the renderable texture.
  inline VTextureLoader::VTextureFormat_e GetResolvedTextureFormat() const
  {
    return GetTextureFormat();
  }

#if defined(_VR_DX11)
  VISION_APIFUNC D3DView *GetRenderTarget() const;
  DXGI_FORMAT m_BufferFormat;
#endif

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
