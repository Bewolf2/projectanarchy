/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiTextureManager.hpp

#ifndef DEFINE_VISAPITEXTUREMANAGER
#define DEFINE_VISAPITEXTUREMANAGER

#include <Vision/Runtime/Engine/Renderer/Shader/VisApiShaders.hpp>

class VisRenderableTexture_cl;
class VisRenderableCubeMap_cl;
class VTextureObject;
typedef VSmartPtr<VTextureObject>  VTextureObjectPtr;


/// \brief
///   Config structure for defining a renderable texture's properties. 
/// 
/// A renderable texture can be created by passing this structure to
/// VisTextureManager_cl::CreateRenderableTexture. The returned renderable texture
/// is derived from the common VTextureObject and can directly be used as a texture.
/// 
/// \sa VisTextureManager_cl::CreateRenderableTexture
struct VisRenderableTextureConfig_t
{
  /// \brief
  ///   constructor : sets useful default values
  inline VisRenderableTextureConfig_t() 
  {
    // Zero memory
    memset(this,0,sizeof(VisRenderableTextureConfig_t));

    m_iStructVersion = 2;
    m_iHeight = 256;
    m_iWidth = 256;
    m_bCreateMipmaps = FALSE;
    m_eFormat = VTextureLoader::UNKNOWN;
    m_bIsUsableAsVertexTexture = false;
    m_bRenderTargetOnly = false;
    m_bIsDepthStencilTarget = false;
    m_iMultiSampling = 0;
    m_bSRGB = false;

    #ifdef SUPPORTS_MANUAL_HIZSTENCIL_SETUP
      m_iHiZOffset = 0;
    #endif
    
    #ifdef _VISION_XENON
      m_iEdramOffset = -1;
      m_bUsesPredicatedTiling = FALSE;
      m_iTileCount = 0;
      m_bExpand = false;
    #endif
    
    #ifdef _VISION_PS3
      m_bUseXDRMemory = false;           ///< use XDR memory for the texture?
      m_bUseCompression = true; 
      m_bUseTiledMemory = true;
      m_bSwizzled = false;

      m_bResolveTargetSwizzled = false;
      m_bResolveTargetUseXDRMemory = false;
    #endif
    
    #if defined (WIN32) || defined (_VISION_PS3) 
      m_bResolve = false;
    
    #elif defined (_VISION_XENON)
      m_bResolve = true;                ///< On Xbox360, we always need resolve (due to EDRAM)
     
    #endif

    #if defined(_VR_DX11)
      m_bUnorderedAccessView = false;
      m_bRenderTargetArray = false;
      m_iSliceRange[0] = 0;
      m_iSliceRange[1] = -1;
      m_iDepth = 256;
    #endif

    #if defined(_VR_GX2)
      m_bPreferFastMemory = true;
      m_bUseHiZ = true;
    #endif
  }

  // assignment operator
  inline VisRenderableTextureConfig_t &operator = (const VisRenderableTextureConfig_t &other)
  {
    memcpy(this, &other, sizeof(VisRenderableTextureConfig_t));
    return *this;
  }

  // equal operator
  inline BOOL operator == (const VisRenderableTextureConfig_t &other) const
  {
    return memcmp(this, &other, sizeof(VisRenderableTextureConfig_t))==0;
  }
  inline BOOL operator != (const VisRenderableTextureConfig_t &other) const
  {
    return memcmp(this, &other, sizeof(VisRenderableTextureConfig_t))!=0;
  }

  int m_iStructVersion;     ///< version of the structure, recent version is 2

  // texture properties
  int m_iWidth;             ///< width of the texture
  int m_iHeight;            ///< height of the texture

  // renderable texture properties
  BOOL m_bCreateMipmaps;    ///< automatically create mipmaps

  VTextureLoader::VTextureFormat_e m_eFormat; ///< Texture Format

  bool m_bSRGB;
  
  unsigned int m_iMultiSampling;  ///< multisampling type (may be 0, 2, 4, or 8)

#ifdef SUPPORTS_MANUAL_HIZSTENCIL_SETUP
  int  m_iHiZOffset;              ///< offset of the HiZ/ZCull buffer in HiZ/ZCull Memory. Supported on Xbox360 and Playstation 3.
#endif

#ifdef _VISION_XENON
  int  m_iEdramOffset;            ///< offset of the surface in EDRAM in number of EDRAM tiles
  bool m_bUsesPredicatedTiling;   ///< use predicated tiling?
  bool m_bExpand;                 ///< if true, expand texture to fixed-point format when resolving in order to allow filtering
  int  m_iTileCount;              ///< number of tiles
  D3DRECT m_TilingRects[16];      ///< tiling rects for predicated tiling
#endif

#ifdef _VISION_PS3
  bool m_bUseXDRMemory;           ///< use XDR memory for the render target
  bool m_bUseCompression;         ///< use compression for the render target (only allowed if m_bUseTiledMemory == true)
  bool m_bUseTiledMemory;         ///< allocate render target in tiled memory
  bool m_bSwizzled;               ///< swizzled render target (warning: may degrade performance; swizzled targets do not support anti-aliasing)
  bool m_bResolveTargetSwizzled;  ///< if resolve is used, use this to specfy whether the resolve target should be swizzled.
  bool m_bResolveTargetUseXDRMemory; ///< if resolve is used, use this to specify whether the resolve target should be in XDR memory rather than local memory.
  bool m_bPad1;                   ///< padding for ensure the data structures is 4 bytes aligned
  bool m_bPad2;                   ///< padding for ensure the data structures is 4 bytes aligned 
#endif

  
  bool m_bResolve;                  ///< has to be set to true for multisample render targets on PC, Xbox360 and PS3. This will result in renderable textures created with this config not being usable directly as input for texture-mapping, but instead requiring a separate target texture and resolve operation.
  bool m_bIsUsableAsVertexTexture;  ///< has to be set if the texture is intended to be used as a vertex texture. Note that this may degrade performance when sampling this texture in a pixel shader!
  bool m_bRenderTargetOnly;         ///< if set to true, only a color or depth stencil target will be created. In this case, it will not be possible to use the texture as input for shaders.
  bool m_bIsDepthStencilTarget;     ///< if set to true, a depth stencil target will be created instead of a color render target target.
  
#if defined(_VR_DX11)
  
  bool m_bUnorderedAccessView;    ///< DX11 only: True to create an unordered access view of this renderable texture

  bool m_bRenderTargetArray;        ///< Cubemaps on DX10/11 only: if true, this renderable cubemap will be treated as a single DX10/11 render target array (rather than six separate 2D render targets).
  union
  {
    int  m_iSliceRange[2];            ///< 3D Textures on DX10/11 only: First slice and number of slices to use in the render target view.
    int  m_iArrayRange[2];            ///< 2D Array Textures on DX10/11 only: First array index and number of textures in the array to use in the render target view.
  };
  union
  {
    int  m_iArraySize;                ///< 2D Array Textures on DX10/11 only: Number of array elements in the texture array.
    int  m_iDepth;                    ///< 3D Textures on DX10/11 only: Depth of the texture
  };
#endif

#if defined (_VR_DX11) || defined (_VR_DX9) && !defined (_VISION_XENON)
  bool m_bCreateSharedResource;       ///< Indicates that this resource is created as shared.
  HANDLE m_hSharedRenderable;         ///< Shared cross-process handle for render targets.
#endif

#if defined(_VR_GX2)
  bool m_bPreferFastMemory; ///< WiiU only - if true the rendertarget will try to allocate its memory in MEM1 and falls back to MEM2 if no memory is left.
  bool m_bUseHiZ; ///< WiiU only - if true and the rendertarget is a depth stencil target a HiZ buffer will be allocated
#endif
};



/// \brief
///   Texture Manager class for accessing textures in the engine. The global instance of the
///   texture manager can be accessed via Vision::TextureManager.
/// 
/// This class provides useful functionality to load texture files or upload texture data from
/// system memory.
/// 
/// This class is however not a resource manager. To access the global texture resource manager,
/// use Vision::ResourceSystem.GetResourceManagerByName(VIS_RESOURCEMANAGER_TEXTURES).
class VisTextureManager_cl
{
public:
  VisTextureManager_cl();
  ~VisTextureManager_cl();

  ///
  /// @name Render Target Related
  /// @{
  ///


  /// \brief
  ///   Creates a renderable 2D texture that can be used as a render target
  /// 
  /// Since a renderable texture object is derived from VTextureObject, it can also be used as a
  /// standard texture.
  /// 
  /// The texture properties are passed through the VisRenderableTextureConfig_t structure.
  /// 
  /// By default the returned texture has the VRESOURCEFLAG_AUTODELETE flag set so it gets deleted
  /// as soon as there is no reference to it anymore.
  /// 
  /// \param szName
  ///   Internal key of the renderable texture. Shows up in the resource viewer and is also used to
  ///   find a texture.
  /// 
  /// \param config
  ///   Config structure that defines the renderable texture properties
  ///
  /// \param iLoadingFlags
  ///   Optional loading flags
  /// 
  /// \return
  ///   VisRenderableTexture_cl* pRTObject: Pointer to renderable texture object, derived from
  ///   VTextureObject. NULL if creation failed.
  /// 
  /// \sa VisRenderableTextureConfig_t
  /// \sa VisTextureManager_cl::FindRenderableTexture
  /// \sa VisRenderContext_cl
  /// \sa VisRenderableTexture_cl
  VISION_APIFUNC VisRenderableTexture_cl* CreateRenderableTexture(const char *szName, VisRenderableTextureConfig_t &config,
                                                                  unsigned int iLoadingFlags = VTM_DEFAULT_FLAGS);

  /// \brief
  ///   Creates a renderable cube map that can be used as a render target
  /// 
  /// Since a renderable cube map object is derived from VTextureObject, it can also be used as a
  /// regular texture.
  /// 
  /// The texture properties are passed through the VisRenderableTextureConfig_t structure.
  /// 
  /// By default the returned texture has the VRESOURCEFLAG_AUTODELETE flag set so it gets deleted
  /// as soon as there is no reference to it anymore.
  /// 
  /// \param szName
  ///   Internal key of the renderable cube map. Shows up in the resource viewer and may also be
  ///   used to find a texture again.
  /// 
  /// \param config
  ///   Config structure that defines the renderable texture's properties
  /// 
  /// \param iLoadingFlags
  ///   Optional loading flags
  /// 
  /// \return
  ///   VisRenderableCubeMap_cl* pRTObject: Pointer to renderable cube map object, derived from
  ///   VTextureObject. NULL if creation failed.
  /// 
  /// \sa VisRenderableTextureConfig_t
  /// \sa VisTextureManager_cl::FindRenderableTexture
  /// \sa VisRenderContext_cl
  VISION_APIFUNC VisRenderableCubeMap_cl* CreateRenderableCubeMap(const char *szName, VisRenderableTextureConfig_t &config,
                                                                  unsigned int iLoadingFlags = VTM_DEFAULT_FLAGS);

  ///
  /// @}
  ///

  ///
  /// @name Texture Object Functions
  /// @{
  ///

  /// \brief
  ///   Creates a debug output string with a list of all used textures, e.g. for console output
  ///
  /// \param pBuffer
  ///   The output string with the list of used textures.
  /// 
  /// \param maxchar
  ///   Maximum number of characters that pBuffer can hold.
  VISION_APIFUNC void GetCurrentUsedTextureTableString(char *pBuffer, int maxchar);

  /// \brief
  ///   Creates an empty 2D-texture object.
  /// 
  /// This function does not allocate texture memory. You will have to call VTextureObject::EnsureLoaded in order to
  /// allocate device-specific data for the texture, and VTextureObject::UpdateRect to update its content. Note
  /// that there may be limitations regarding texture properties, e.g. some devices may only support power-of-two
  /// textures.
  /// 
  /// \param szInternalName
  ///   Internal texture name to appear in resource viewer
  /// 
  /// \param iSizeX
  ///   Width of the texture.
  /// 
  /// \param iSizeY
  ///   Height of the texture.
  /// 
  /// \param eTextureFormat
  ///   VTextureLoader::VTextureFormat_e eTextureFormat : Color format of the texture. Must be a format
  ///   supported by the graphics hardware.
  /// 
  /// \return
  ///   VTextureObject* pTexture : Pointer to texture object
  VISION_APIFUNC VTextureObject* CreateEmpty2DTextureObject(const char *szInternalName, int iSizeX, int iSizeY,
    VTextureLoader::VTextureFormat_e eTextureFormat = VTextureLoader::DEFAULT_TEXTURE_FORMAT_32BPP);

  /// \brief
  ///   Creates a 2D texture object that can be modified via VTextureObject::UpdateRect.
  ///
  /// Unlike CreateEmpty2DTextureObject, this function allocates texture memory. Note that there may be limitations 
  /// regarding texture properties, e.g. some devices may only support power-of-two textures.
  /// 
  /// \param szInternalName
  ///   Internal texture name to appear in resource viewer
  /// 
  /// \param iSizeX
  ///   Width of the texture. 
  /// 
  /// \param iSizeY
  ///   Height of the texture. 
  /// 
  /// \param iMipLevels
  ///   The number of mipmap levels (1 for no mipmaps)
  /// 
  /// \param eTextureFormat
  ///   VTextureLoader::VTextureFormat_e eTextureFormat : Color format of the texture. Must be a format
  ///   supported by the graphics hardware.
  /// 
  /// \param iFlags
  ///   Additional creation flags. The following flags are supported: 
  ///   \li VTM_FLAG_AUTOGENMIPMAPS : Generate mipmaps on the hardware.
  ///   \li VTM_FLAG_VERTEXTEXTURE : The texture can also be bound as a vertex shader sampler.
  ///   \li VTM_FLAG_NONMANAGEDPOOL : PC DX9 only. If set, texture will be created in default pool,
  ///     and no lost device handling will be performed.
  ///   \li VTM_FLAG_XDRMEMORY : PS3 only. If set, texture will be created in main memory.
  ///   \li VTM_FLAG_FORCELINEAR : PS3 only. If set, texture will be linear.
  ///   \li VTM_FLAG_DYNAMIC : Texture is dynamic. Recommended for frequent CPU-side writes.
  ///   \li VTM_FLAG_UNORDEREDACCESSVIEW : PC DX11 only. If this flag is specified, the engine will generate an unordered
  ///       access view (byte-address view) for this texture so that it can be used as an output target for compute shaders.
  ///   \li VTM_FLAG_FORCETYPEDSURFACE : PC DX11 only. If this flag is specified, the engine will create a typed surface
  ///       instead of _TYPELESS. This is often less efficient and can prevent surface sharing, but allows debugging surfaces
  ///       in tools like PIX.
  ///   \li VTM_FLAG_SRGB : If specified, the texture will be assumed to be in sRGB space.
  /// 
  /// \return
  ///   VTextureObject* pTexture : Pointer to texture object
  /// 
  /// \sa VTextureObject
  /// \sa VTextureObject::UploadDataMipmap
  VISION_APIFUNC VTextureObject* Create2DTextureObject(const char *szInternalName, int iSizeX, int iSizeY, int iMipLevels, 
    VTextureLoader::VTextureFormat_e eTextureFormat, int iFlags=VTM_FLAG_DEFAULT_NON_MIPMAPPED);


  /// \brief
  ///   Loads a 2D texture from file, or returns an already loaded texture from the texture manager.
  /// 
  /// The flags specify additional loading properties, e.g. for creating mipmaps. Textures can later on be updated
  /// using VTextureObject::UpdateRect().
  /// 
  /// \param szFilename
  ///   The 2D texture filename to load.
  /// 
  /// \param iFlags
  ///   Loading flags for creating the texture. Some of the flags will be ignored for DDS texture
  ///   files. The following flags are supported:
  ///   \li VTM_FLAG_FORCE_UNIQUE : If specified, the texture manager will not check for occurances
  ///     of the texture with the same filename. This flag should not be used.
  ///   \li VTM_FLAG_NO_UPLOAD : The texture will not be uploaded at creation time, but just when used the first time. 
  ///   \li VTM_FLAG_NO_DOWNSCALE : If specified, the texture remains unaffected by global texture detail.
  ///     Useful for loading GUI textures if the global texture detail is reduced.
  ///   \li VTM_FLAG_NO_MIPMAPS : Ignore the mipmaps in the (dds-) file and create a texture without mipmaps.
  ///   \li VTM_FLAG_AUTOGENMIPMAPS : Generate mipmaps on the hardware. Mutually exclusive with VTM_FLAG_NO_MIPMAPS flag. Usually this flag does not work with DXT compressed textures.
  ///   \li VTM_FLAG_VERTEXTEXTURE : This flag should be specified if the texture might be used in a vertex shader.
  ///   \li VTM_FLAG_UNORDEREDACCESSVIEW : PC DX11 only. If this flag is specified, the engine will generate an unordered
  ///       access view (byte-address view) for this texture so that it can be used as an output target for compute shaders.
  ///   \li VTM_FLAG_FORCETYPEDSURFACE : PC DX11 only. If this flag is specified, the engine will create a typed surface
  ///       instead of _TYPELESS. This is often less efficient and can prevent surface sharing, but allows debugging surfaces
  ///       in tools like PIX.
  ///   \li VTM_FLAG_DEFAULT_MIPMAPPED : standard bit combination for loading mipmapped textures
  ///   \li VTM_FLAG_DEFAULT_NON_MIPMAPPED : standard bit combination for loading non-mipmapped
  ///     textures
  ///   \li VTM_FLAG_SRGB : If specified, the texture will be assumed to be in sRGB space.
  /// 
  /// \return
  ///   VTextureObject* pTexture : Pointer to texture object. If the texture could not be loaded, a
  ///   report entry is created and this function returns the "missing" texture.
  VISION_APIFUNC VTextureObject* Load2DTexture(const char *szFilename, int iFlags=VTM_FLAG_DEFAULT_MIPMAPPED);


  /// \brief
  ///   Creates a Cube Map texture object that can be modified via VTextureCubeObject::UpdateRect.
  ///
  /// \param szInternalName
  ///   Internal texture name to appear in resource viewer
  /// 
  /// \param iSize
  ///   The width and height of the texture. Must be a power of two.
  /// 
  /// \param iMipLevels
  ///   The number of mipmap levels (1 for no mipmaps).
  /// 
  /// \param eDataFormat
  ///   Format of the texture. Must be a format supported by the graphics hardware.
  /// 
  /// \param iFlags
  ///   Additional creation flags. The following flags are supported: 
  ///   \li VTM_FLAG_AUTOGENMIPMAPS : Generate mipmaps on the hardware.
  ///   \li VTM_FLAG_NONMANAGEDPOOL : PC DX9 only. If set, texture will be created in default pool,
  ///     and no lost device handling will be performed.
  ///   \li VTM_FLAG_XDRMEMORY : PS3 only. If set, texture will be created in main memory.
  ///   \li VTM_FLAG_FORCELINEAR : PS3 only. If set, texture will be linear.
  ///   \li VTM_FLAG_DYNAMIC : Texture is dynamic. Recommended for frequent CPU-side writes.
  ///   \li VTM_FLAG_UNORDEREDACCESSVIEW : PC DX11 only. If this flag is specified, the engine will generate an unordered
  ///       access view (byte-address view) for this texture so that it can be used as an output target for compute shaders.
  ///   \li VTM_FLAG_FORCETYPEDSURFACE : PC DX11 only. If this flag is specified, the engine will create a typed surface
  ///       instead of _TYPELESS. This is often less efficient and can prevent surface sharing, but allows debugging surfaces
  ///       in tools like PIX.
  ///   \li VTM_FLAG_SRGB : If specified, the texture will be assumed to be in sRGB space.
  /// 
  /// \return
  ///   VTextureObject* pTexture : Pointer to texture object
  /// 
  /// \sa VTextureObject
  /// \sa VTextureObject::UploadDataMipmap
  VISION_APIFUNC VTextureCubeObject* CreateCubemapTextureObject(const char *szInternalName, int iSize, int iMipLevels,
    VTextureLoader::VTextureFormat_e eDataFormat, int iFlags);

  /// \brief
  ///   Creates a 3D-texture object that can be modified via VTexture3DObject::UploadDataMipmap
  /// 
  /// Note that 3D textures are not supported on some platforms. Furthermore, there may be other limitations
  /// regarding volume texture properties, e.g. some devices may only support power-of-two texture sizes.
  /// 
  /// \param szInternalName
  ///   Internal texture name to appear in resource viewer
  /// 
  /// \param iSizeX
  ///   Width of the texture. Must be power-of-two size
  /// 
  /// \param iSizeY
  ///   Height of the texture. Must be power-of-two size
  /// 
  /// \param iSizeZ
  ///   Depth of the volume texture. Must be power-of-two size
  /// 
  /// \param iMipLevels
  ///   The number of mipmap levels (1 for no mipmaps)
  /// 
  /// \param eTextureFormat
  ///   VTextureLoader::VTextureFormat_e eTextureFormat : Color format of the texture. Must be a format
  ///   supported by the graphics hardware.
  /// 
  /// \param iFlags
  ///   Additional creation flags. The following flags are supported: 
  ///   \li VTM_FLAG_NONMANAGEDPOOL : PC DX9 only. If set, texture will be created in default pool,
  ///     and no lost device handling will be performed.
  ///   \li VTM_FLAG_XDRMEMORY : PS3 only. If set, texture will be created in main memory.
  ///   \li VTM_FLAG_DYNAMIC : Texture is dynamic. Recommended for frequent CPU-side writes.
  ///   \li VTM_FLAG_UNORDEREDACCESSVIEW : PC DX11 only. If this flag is specified, the engine will generate an unordered
  ///       access view (byte-address view) for this texture so that it can be used as an output target for compute shaders.
  ///   \li VTM_FLAG_SRGB : If specified, the texture will be assumed to be in sRGB space.
  /// 
  /// \return
  ///   VTexture3DObject* pTexture : Pointer to texture object
  /// 
  /// \sa VTexture3DObject
  /// \sa VTexture3DObject::UploadDataMipmap
  VISION_APIFUNC VTexture3DObject* Create3DTextureObject(const char *szInternalName, int iSizeX, int iSizeY, int iSizeZ, int iMipLevels,
    VTextureLoader::VTextureFormat_e eTextureFormat, int iFlags=VTM_FLAG_DEFAULT_NON_MIPMAPPED);

  /// \brief
  ///   Loads a 3D texture from a file, or returns an already loaded texture from the texture manager.
  /// 
  /// Textures can later on be updated using VTexture3DObject::UploadDataMipmap.
  /// 
  /// \param szFilename
  ///   The 3D texture filename to load.
  /// 
  /// \param iFlags
  ///   The flags specify additional loading properties, e.g. for creating mipmaps.
  ///   The supported flags correspond to flags supported in Load2DTexture.
  ///
  /// \return
  ///   VTexture3DObject* pTexture : Pointer to the 3D texture.
  VISION_APIFUNC VTexture3DObject* Load3DTexture(const char *szFilename, int iFlags=VTM_FLAG_DEFAULT_MIPMAPPED);

#if defined(_VR_DX11)

  /// \brief
  ///   Creates a 2D texture array that can be modified via
  ///   VTexture2DArrayObject::UploadDataMipmap.
  /// 
  /// This functionality is only supported in DX10/DX11.
  /// 
  /// \param szInternalName
  ///   Internal texture name to appear in resource viewer.
  /// 
  /// \param iSizeX
  ///   Width of the texture. Must be power-of-two size.
  /// 
  /// \param iSizeY
  ///   Height of the texture. Must be power-of-two size.
  /// 
  /// \param iArraySize
  ///   Number of slices (array elements) in the 2D texture array.
  /// 
  /// \param iMipLevels
  ///   The number of mipmap levels (1 for no mipmaps).
  /// 
  /// \param eTextureFormat
  ///   VTextureFormat_e eTextureFormat : Color format of the texture. Must be a format
  ///   supported by the graphics hardware.
  /// 
  /// \param iFlags
  ///   Additional creation flags. The following flags are supported: 
  ///   \li VTM_FLAG_DYNAMIC : Texture is dynamic. Recommended for frequent CPU-side writes.
  ///   \li VTM_FLAG_UNORDEREDACCESSVIEW : PC DX11 only. If this flag is specified, the engine will generate an unordered
  ///       access view (byte-address view) for this texture so that it can be used as an output target for compute shaders.
  ///   \li VTM_FLAG_FORCETYPEDSURFACE : PC DX11 only. If this flag is specified, the engine will create a typed surface
  ///       instead of _TYPELESS. This is often less efficient and can prevent surface sharing, but allows debugging surfaces
  ///       in tools like PIX.
  ///   \li VTM_FLAG_SRGB : If specified, the texture will be assumed to be in sRGB space.
  /// 
  /// \return
  ///   VTexture2DArrayObject* pTexture : Pointer to the 2D texture array.
  /// 
  /// \sa VTexture2DArrayObject
  /// \sa VTexture2DArrayObject::UploadDataMipmap
  VISION_APIFUNC VTexture2DArrayObject* Create2DArrayTextureObject(const char *szInternalName, int iSizeX, int iSizeY, int iArraySize,
    int iMipLevels, VTextureLoader::VTextureFormat_e eTextureFormat, int iFlags=VTM_FLAG_DEFAULT_NON_MIPMAPPED);

  /// \brief
  ///   Loads a 2D texture array from a texanim file, or returns an already loaded one from the texture manager.
  /// 
  /// Note that ONLY .texanim files can be loaded as 2D texture arrays.
  /// This functionality is only supported in DX10/DX11.
  /// 
  /// \param szFilename
  ///   Name of the texanim file to load.
  /// 
  /// \param iFlags
  ///   Additional creation flags. The passed flags correspond to the same flags supported by Load2DTexture.
  ///
  /// \return
  ///   VTexture2DArrayObject* pTexture : Pointer to the 2D texture array.
  VISION_APIFUNC VTexture2DArrayObject* Load2DArrayTexture(const char *szFilename, int iFlags=VTM_FLAG_DEFAULT_MIPMAPPED);

#endif

  /// \brief
  ///   Loads a cubemap texture from a .dds file. The passed flags correspond to the flags supported by
  ///   Load2DTexture.
  /// 
  /// The passed filename can also be a cubemap key (without .dds file extension) to lookup a
  /// renderable cubemap proxy by key.
  /// 
  /// Cubemap proxies are represented by class CubeMapHandle_cl and can be placed in vForge.
  ///
  /// \param pszFilename
  ///   Name of the cubemap file to load.
  ///
  /// \param iFlags
  ///   Texture Flags to use when loading the cube map.
  ///
  /// \return
  ///   VTextureCubeObject* pTexture : Pointer to the cubemap texture.
  VISION_APIFUNC VTextureCubeObject* LoadCubemapTexture(const char *pszFilename, int iFlags=VTM_FLAG_DEFAULT_MIPMAPPED);

  /// \brief
  ///   Returns a new instance of a texture animation for the passed texture.
  ///
  /// \param pAnimTex
  ///   The original animated texture object.
  ///
  /// \return
  ///   The new instance for the passed texture. If the passed texture
  ///   is not animated, this function returns NULL.
  VISION_APIFUNC VisTextureAnimInstance_cl* GetAnimationInstance(VTextureObject* pAnimTex);

  /// \brief
  ///   Finds the first texture object with specified name. Same as
  ///   Vision::TextureManager.GetManager().GetResourceByName(szFilename);
  ///
  /// \param szFilename
  ///   The name of the texture.
  ///
  /// \return
  ///   VTextureCubeObject* pTexture : Pointer to the cubemap texture.
  ///   Returns NULL if no resource with the specified name has been found.
  VISION_APIFUNC VTextureObject* GetTextureObjectByName(const char *szFilename);

#ifdef _VR_DX9

  /// \brief
  ///   Returns the Direct3D9 Texture Object for the passed VTextureObject (DirectX9 only). Same as
  ///   pTexture->m_spD3Dinterface
  VISION_APIFUNC IDirect3DTexture9 *GetDirect3DTexture9FromTextureObject(VTextureObject *pTexture) const;

  /// \brief
  ///   This is a PC DX9 specific function to determine which texture pool is used for textures
  ///   loaded from file
  /// 
  /// If left to default (FALSE), textures are created in the managed pool, which means DX holds a
  /// system memory copy to handle lost device gracefully.
  /// 
  /// If set to TRUE, all subsequent textures are loaded into default pool. Lost device in this
  /// mode is handled via unloading/reloading the VTextureObject resource.
  ///
  /// \param bStatus
  ///   Whether to create new textures in the managed (false) or unmanage (true) pool.
  VISION_APIFUNC void SetUseUnmanagedTexturePool(BOOL bStatus);
#endif

  /// \brief
  ///   When the downscale amount is > 0 the texture manager automatically scales down every
  ///   newly created texture by a factor of 2^amount.
  ///
  /// \param iAmount
  ///   Scaledown amount. 0 to disable downscaling. Maximum amount is 3.
  ///
  VISION_APIFUNC void SetDownScale(int iAmount);

#if defined(_VR_DX11)
 
  /// \brief
  ///   Returns the Direct3D Resource for the passed VTextureObject (DirectX10/11 only). Same as
  ///   pTexture->m_spD3DResource
  VISION_APIFUNC D3DResource *GetDirect3DResourceFromTextureObject(VTextureObject *pTexture) const;

#endif


  /// \brief
  ///   Returns a texture object that represents a plain white texture (RGBA=(255,255,255,255))
  VISION_APIFUNC VTextureObject* GetPlainWhiteTexture();

  /// \brief
  ///   Returns a texture object that represents a plain black texture (RGBA=(0,0,0,255))
  VISION_APIFUNC VTextureObject* GetPlainBlackTexture();

  /// \brief
  ///   Returns a texture object that represents a flat normal map texture (RGBA=(128,128,255,255))
  VISION_APIFUNC VTextureObject* GetNeutralNormalMapTexture();

  /// \brief
  ///   Returns a texture object that represents a transparent texture (RGBA=(0,0,0,0))
  VISION_APIFUNC VTextureObject* GetPlainTransparentTexture();

  /// \brief
  ///   Returns a texture object that has the solid color of the scene's default color (VisRenderer_cl::GetDefaultLightingColor)
  VISION_APIFUNC VTextureObject* GetSceneColorTexture();

  /// \brief
  ///   Returns the global instance of the texture resource manager
  VISION_APIFUNC VTextureManager& GetManager();

  /// \brief
  ///   Returns the maximum texture size supported by the hardware. Only valid after engine initialization.
  VISION_APIFUNC int GetMaxTextureSize() const;

  ///
  /// @}
  ///

private:
  friend class Vision;
  void Init ();
  void DeInit ();

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
