/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ScratchTexturePool.hpp

#ifndef SCRATCH_TEXTURE_POOL_HPP_INCLUDED
#define SCRATCH_TEXTURE_POOL_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>

#define MAX_NUM_SCRATCH_TEXTURES 128

/// \brief
///   ScratchTexture Pool for creating and storing temporary render target textures
///
/// The scratch texture pool is a useful helper class for storing temporary render target textures which
/// can be used across post-processors, renderer nodes, rendering effects, and so on. Whenever a scratch texture
/// is requested from the pool using the GetScratchTexture function, the pool looks for a previously created
/// scratch texture with compatible properties (e.g. color format and size) and, if found, returns this texture.
/// Otherwise, it creates a new texture using the passed properties and keeps an internal reference to it, so
/// that the texture can be returned the next time a scratch texture with compatible properties is requested.
/// 
/// Note that the scratch texture pool keeps a reference to its textures, so the textures it references will not
/// be deleted as long as this reference is present. You can call PurgeUnusedTextures() to release scratch textures
/// which only have the pool's reference (and no external references) left.
/// 
/// The scratch texture pool listens to the OnVideoDeinitializing callback and releases all texture references when
/// this callback is triggered.
///
/// You can access a global instance of the scratch texture pool using the static GlobalManager() member.
class ScratchTexturePool_cl : public IVisCallbackHandler_cl
{
public:

  /// \brief
  ///   One-time initialization function of the scratch texture pool. 
  EFFECTS_IMPEXP void OneTimeInit();

  /// \brief
  ///   One-time de-initialization function of the scratch texture pool. 
  EFFECTS_IMPEXP void OneTimeDeInit();

  /// \brief
  ///   Accesses the global scratch texture pool instance.
  EFFECTS_IMPEXP static ScratchTexturePool_cl& GlobalManager();

  /// \brief
  /// Returns a temporary texture for rendering effects.
  ///
  /// Many rendering effects need temporary textures of specific sizes. In order for these effects not to have to
  /// allocate each of these temporary textures separately, this class provides a convenient function which allows
  /// rendering effects to share temporary textures with identical properties.
  /// If width, height, format, and index are identical to an already existing texture, this
  /// method will simply return the existing scratch textures. If no texture of the required properties exists yet,
  /// a new one with these settings will be created.
  /// 
  /// \param iWidth
  /// Width of the texture in texels.
  /// 
  /// \param iHeight
  /// Height of the texture in texels.
  /// 
  /// \param eFormat
  /// Texture format.
  /// 
  /// \param iIndex
  /// Texture index. This can be used to ensure that two scratch textures can be separate physical textures even
  /// though they have identical width, height, and format properties.
  /// 
  /// \param iEdramOffset
  /// Xbox360 only: EDRAM offset (in EDRAM tiles) of this render target. Default is 0.
  /// 
  /// \param bPreferSwizzled
  /// Playstation 3 only: true if swizzled texture format is preferred over linear format (default is false).
  ///
  /// \param iMSAASamples
  /// How many MSAA samples the texture should have. Valid values: 0, 2, 4, 8, 16. Default is 0 (no multisampling).
  /// 
  /// \param iHiZOffset
  ///   Offset of the texture in HiZ/ZCull memory. Only relevant for PS3 and Xbox360. Only relevant for depth-stencil formats.
  ///
  /// \param bUseTiledMemoryOnPS3
  ///   If set to true, the scratch texture will used tiled memory on the PS3. Otherwise it won't.
  ///   Note that there is a limit of tiled textures that can be used (usually 16), so only use this on textures that benefit from it.
  ///   Also note that not using tiled memory also disables the use of texture compression.
  ///   This parameter has no effect on platforms other then the PS3.
  ///
  /// \param bUseExpandFormatOnXbox360
  ///   Tries to use the expanded format on Xbox360. This can be used to enable linear filtering for floating point textures,
  ///   at the cost of a lower sampling rate.
  /// 
  /// \returns
  ///   VisRenderableTexture_cl *pRT: Pointer to the renderable texture object.
  /// 
  EFFECTS_IMPEXP VisRenderableTexture_cl *GetScratchTexture(int iWidth, int iHeight, VTextureLoader::VTextureFormat_e eFormat, 
    int iIndex = -1, int iEdramOffset = 0, bool bPreferSwizzled = false, int iMSAASamples = 0, int iHiZOffset = 0, 
    bool bUseTiledMemoryOnPS3 = true, bool bUseExpandFormatOnXbox360=false);

  /// \brief
  /// Returns a temporary texture for rendering effects.
  /// 
  /// \param config
  /// Config structure that defines the renderable texture properties
  ///
  /// \param iIndex
  /// Texture index. This can be used to ensure that two scratch textures can be separate physical textures even
  /// though they have identical width, height, and format properties.
  /// 
  /// \returns
  /// VisRenderableTexture_cl *pRT: Pointer to the renderable texture object.
  /// 
  EFFECTS_IMPEXP VisRenderableTexture_cl *GetScratchTexture(VisRenderableTextureConfig_t &config, int iIndex = -1);

  /// \brief
	/// Removes all texture references of the scratch texture pool.
  EFFECTS_IMPEXP void ReleaseAllScratchTextures();

  /// \brief
  /// Purges unused textures (i.e. textures which are only references by the scratch texture pool).
  EFFECTS_IMPEXP void PurgeUnusedTextures();

protected:

  ScratchTexturePool_cl();
  virtual ~ScratchTexturePool_cl();

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  int m_iNumScratchTextures;
  int m_iScratchIndex[MAX_NUM_SCRATCH_TEXTURES];
  VisRenderableTexturePtr m_spScratchTextures[MAX_NUM_SCRATCH_TEXTURES];
  bool m_bCallbackRegistered;

  static ScratchTexturePool_cl g_GlobalManager;

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
