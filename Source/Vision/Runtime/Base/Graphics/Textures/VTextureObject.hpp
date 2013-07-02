/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTextureObject.hpp

#ifndef _VTEXTUREOBJECT_HPP_INCLUDED
#define _VTEXTUREOBJECT_HPP_INCLUDED

#include <Vision/Runtime/Base/System/Resource/VResource.hpp>
#include <Vision/Runtime/Base/System/Resource/VResourceManager.hpp>
#include <Vision/Runtime/Base/Graphics/Textures/VTextureLoader.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroupSampler.hpp>

#if defined( _VISION_DOC )
  #define SUPPORTS_GPU_RESOURCE_VIEWS
#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )
  #include <Vision/Runtime/Base/System/Resource/VGpuResourceView.hpp>
#endif

// forward declarations (engine):
class VisPBuffer_cl;
class VTextureManager;
class VisTextureAnimInstance_cl;
class VisTextureManager_cl;
class VTextureObject;
struct TextureAnimInfo_t;

#ifndef VGCM_IS_SWIZZLED
  #define VGCM_IS_SWIZZLED(pGcmTex) ((pGcmTex->format & CELL_GCM_TEXTURE_LN) ? false : true)
#endif


#ifdef _VR_DX9
// forward declarations (Direct3D):
struct IDirect3DTexture9;
typedef VSmartPtr<IDirect3DBaseTexture9>   IInternalTextureObjectPtr;

#elif defined(_VR_DX11)

// forward declarations (Direct3D):
typedef VSmartPtr<D3DResource>   D3DResourcePtr;
typedef VSmartPtr<D3DShaderResourceView>   D3DShaderResourcePtr;


#elif defined(_VR_GCM)

  #include <cell/gcm.h>
  #include <Vision/Runtime/Base/System/Memory/VMemoryBasePS3.hpp>
  #include <Vision/Runtime/Base/Graphics/Video/VGraphicsMemoryPS3.hpp>

#elif defined(_VR_GXM)
  #include <Vision/Runtime/Base/System/Memory/VLockableMemoryPSP2.inl>

#elif defined(_VR_OPENGL) && defined(_VISION_LINUX)
  #include <GL/gl.h>

#endif


// cubemap faces
#define VTM_CUBEMAPFACE_POS_X               0
#define VTM_CUBEMAPFACE_NEG_X               1
#define VTM_CUBEMAPFACE_POS_Y               2
#define VTM_CUBEMAPFACE_NEG_Y               3
#define VTM_CUBEMAPFACE_POS_Z               4
#define VTM_CUBEMAPFACE_NEG_Z               5

// used by flags parameter of Load/Modify
// these flags can be ORed
#define VTM_DEFAULT_FLAGS                   0x00000000
#define VTM_FLAG_NO_MIPMAPS                 0x00000001
#define VTM_FLAG_FORCE_UNIQUE               0x00000002
#define VTM_FLAG_NO_UPLOAD                  0x00000004
#define VTM_FLAG_NO_DOWNSCALE               0x00000008
#define VTM_FLAG_XDRMEMORY                  0x00000010    /* PS3 only */
#define VTM_FLAG_DYNAMIC                    0x00000020
#define VTM_FLAG_ASSUME_SRGB                0x00000040
#define VTM_FLAG_NO_UPLOAD_ANIMFRAMES       0x00000080
#define VTM_FLAG_AUTOGENMIPMAPS             0x00000100
#define VTM_FLAG_VERTEXTEXTURE              0x00000200
#define VTM_FLAG_NONMANAGEDPOOL             0x00000400    /* PC DX9 only */
#define VTM_FLAG_FORCELINEAR                0x00000800    /* PS3 only */
#define VTM_FLAG_GPU_UPLOADFORMAT           0x00001000
#define VTM_FLAG_UNORDEREDACCESSVIEW        0x00002000    /* DX11 only */
#define VTM_FLAG_RESOLVETARGET              0x00004000    /* DX9 only */
#define VTM_FLAG_FORCETYPEDSURFACE          0x00008000    /* DX11 only */
#define VTM_FLAG_LOAD_AS_SRGB               0x00010000

// commonly used flag combinations
#define VTM_FLAG_DEFAULT_MIPMAPPED      (0)
#define VTM_FLAG_DEFAULT_NON_MIPMAPPED  (VTM_FLAG_NO_MIPMAPS)

// Lockflags
#define V_TEXTURE_LOCKFLAG_NONE                 0x0
#define V_TEXTURE_LOCKFLAG_DISCARDABLE          0x1
#define V_TEXTURE_LOCKFLAG_READONLY             0x2
#define V_TEXTURE_LOCKFLAG_NOOVERWRITE          0x4
#define V_TEXTURE_LOCKFLAG_NOCOMPONENTSWIZZLE   0x100
#define V_TEXTURE_LOCKFLAG_NATIVEDATA           0x200

#ifdef WIN32
  #ifdef _VR_DX9
    #define VIS_TEX_DEFAULTPOOLTYPE   D3DPOOL_MANAGED
    #define VIS_TEX_RTPOOLTYPE        D3DPOOL_DEFAULT
  #endif

#elif defined(_VISION_XENON)
  #define VIS_TEX_DEFAULTPOOLTYPE   0
  #define VIS_TEX_RTPOOLTYPE        0

#endif

/// \brief
///   Pure virtual interface for texture resource loading. Implemented by VTextureManager in the engine.
class IVTextureLoader
{
public:
  /// \brief
  ///   Destructor
  virtual ~IVTextureLoader() {}

  /// \brief
  ///   Load a 2D texture from file and return the texture object
  virtual VTextureObject* Load2DTextureFromFile(const char *szFilename, int iFlags=0) = 0;

  /// \brief
  ///   Load a 3D texture from file and return the texture object
  virtual VTextureObject* Load3DTextureFromFile(const char *szFilename, int iFlags=0) = 0;
  
  #if defined(SUPPORTS_2D_TEXTURE_ARRAYS)
    /// \brief
    ///   Load a 2D texture array from file and return the texture object
    virtual VTextureObject* Load2DArrayTextureFromFile(const char *szFilename, int iFlags=0) = 0;
  #endif

  /// \brief
  ///   Load a cubemap texture from file and return the texture object
  virtual VTextureObject* LoadCubemapTextureFromFile(const char *szFilename, int iFlags=0) = 0;

  /// \brief
  ///   Generate a renderable cubemap proxy with the specified key
  virtual VTextureObject* GetRenderableCubemap(const char *szKey, bool bForceCreate=true) = 0;
};


/// \brief
///   Derived proxy class for loading a VTextureObject resource. Used internally.
class VTextureSerializationProxy : public IVSerializationProxy
{
public:
  inline VTextureSerializationProxy(VTextureObject *pTexture) {m_pTexture=pTexture;}
  VBASE_IMPEXP VOVERRIDE IVSerializationProxyCreator *GetInstance();

protected:
  // serialization
  inline VTextureSerializationProxy() {m_pTexture=NULL;}
  V_DECLARE_SERIAL_DLLEXP( VTextureSerializationProxy, VBASE_IMPEXP )

  void Serialize( VArchive &ar, int iAdditionalFlags);
  virtual void Serialize( VArchive &ar) HKV_OVERRIDE { Serialize (ar, 0); }

private:
  VTextureObject *m_pTexture;
};


/// \brief
///   Texture object
/// 
/// Class used for a representing textures in the Vision Engine. VTextureObject is the base class for all types of texture
/// objects, and all other texture types (both renderable and non-renderable) are derived from it. At the same time,
/// a VTextureObject instance represents a regular 2D texture.
class VTextureObject : public VManagedResource
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///
 
  /// \brief
  ///   Constructor. Should not be used directly.
  VBASE_IMPEXP VTextureObject(VResourceManager *pParentManager);

  /// \brief
  ///   Destructor. Should not be used directly.
  VBASE_IMPEXP virtual ~VTextureObject();
  

  ///
  /// @}
  ///

  ///
  /// @name Texture Properties
  /// @{
  ///

  /// \brief
  ///   Returns the type of this object (i.e. 2D Texture, Cube Map, ...)
  inline VTextureLoader::VTextureType_e GetTextureType() const;

  /// \brief
  ///   Adjust downscale factor to not exceed mip-level count minus one.
  VBASE_IMPEXP void AdjustDownScaleFactor();
  
  /// \brief
  ///   Indicates whether the texture is renderable (i.e., can be used as a render target). 
  /// 
  /// Renderable texture objects are of types VisRenderableTexture_cl, VisRenderableCubeMap_cl etc. and derived from VTextureObjects. "Regular"
  /// VTextureObject instances can not be used as render targets.
  ///
  /// \returns
  ///   bool: true if the texture is renderable.
  VBASE_IMPEXP virtual bool IsRenderable() const;

  /// \brief
  ///   Returns the width of the texture
  inline int GetTextureWidth() const {return m_iSizeX;}

  /// \brief
  ///   Returns the height of the texture
  inline int GetTextureHeight() const {return m_iSizeY;}

  /// \brief
  ///   Returns the depth of the texture (slice count for volume textures, or number of array indices for 2D Array Textures)
  inline int GetTextureDepth() const { return m_iDepth; }

  /// \brief
  ///   Returns the internal color depth of the texture. This does not necessarily correspond to the color depth
  ///   defined in a texture file, since e.g. RGB textures are typically padded to 32bpp for the GPU.
  inline int GetTextureColorDepth() const;

  /// \brief
  ///   Gets the uncompressed color depth of the texture. This will return 24 or 32 for block-compressed formats, depending
  ///   on whether they are compressed RGBA or RGB formats.
  inline int GetTextureColorDepthUncompressed() const;

  /// \brief
  ///   Gets the width, height and color depth of the texture
  VBASE_IMPEXP void GetTextureDimensions(int &size_x, int &size_y, int &depth) const;

  /// \brief
  ///   Gets the downscaled texture dimensions (if downscaling is globally enabled using VisTextureManager_cl::SetDownScale())
  VBASE_IMPEXP void GetRealTextureDimensions(int &size_x, int &size_y, int &depth) const;

  /// \brief
  ///   Gets the texture format of the texture
  inline VTextureLoader::VTextureFormat_e GetTextureFormat() const;

  /// \brief
  ///   Indicates whether texture is a heightmap. Heightmaps are special 16bpp image formats used by the Vision Terrain System.
  inline bool IsHeightMap() const;

  /// \brief
  ///   Indicates whether the texture is a 2D texture
  inline bool IsTexture2D() const;

  /// \brief
  ///   Indicates whether the texture is a 3D texture
  inline bool IsTexture3D() const;

  #if defined(SUPPORTS_2D_TEXTURE_ARRAYS)

  /// \brief
  ///   Indicates whether the texture is a 2D texture array (PC DirectX10/11 and up only)
  inline bool IsTexture2DArray() const;
  #endif

  /// \brief
  ///   Indicates whether the texture is a cubemap texture.
  inline bool IsCubemap() const;

  /// \brief
  ///   Indicates whether the texture is an animated texture.
  inline bool IsAnimated() const;

  /// \brief
  ///   Returns the number of textures in a texture animation (or 1 for non-animated textures)
  inline int GetNumAnimPhases() const;

  /// \brief
  ///   Indicates whether the texture format is a block-compressed format (DXT etc.)
  inline bool IsCompressed() const;

  /// \brief
  ///   Returns the number of mipmap levels used by this texture (returns 1 for no mip maps)
  inline int GetMipMapLevelCount() const;

  /// \brief
  ///   Indicates whether this texture has mipmaps or not
  inline bool HasMipMaps() const
  {
    return GetMipMapLevelCount()>1;
  }

  /// \brief
  ///   Returns whether the texture has an alpha channel
  VBASE_IMPEXP bool HasAlphaChannel() const;

  /// \brief
  ///   Returns a texture object pointer that represents the n-th texture animation frame in an animated texture.
  inline VTextureObject *GetTextureAnimFrame(int framenum);

#if defined(_VR_DX9)

  /// \brief
  ///   Only available on DX9 (PC/Xbox360): Returns the D3D interface for this texture.
  inline IDirect3DBaseTexture9* GetD3DInterface();

#endif

#if defined(_VR_DX11)

  /// \brief
  ///   Only available on DX10/DX11: Returns the D3D resource for this texture. In order to get the
  ///   shader resource view, use GetShaderResourceView.
  inline D3DResource* GetD3DResource();

#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )
  // Temporary functions for resource view management. (To be deprecated soon and moved into the Engine.)
  inline bool CreateShaderResourceView( D3DResource *pResource, D3D_SHADER_RESOURCE_VIEW_DESC *pResourceViewDesc );
  inline bool CreateUnorderedAccessView( D3DResource *pResource, D3D_UNORDERED_ACCESS_VIEW_DESC *pUavDesc );
  inline VShaderResourceView* GetShaderResourceView() const;
  inline VUnorderedAccessView* GetUnorderedAccessView() const;
#endif

#if defined(_VR_GCM)

  /// \brief
  ///   Playstation 3 (GCM) only: Returns the Gcm texture structure
  inline CellGcmTexture* GetGCMTexture();

  /// \brief
  ///   Playstation 3 (GCM) only: Uses the RSX to efficiently compute mipmaps for 32bpp linear
  ///   textures.
  VBASE_IMPEXP bool ComputeMipmaps();

#endif

#if defined(_VR_GXM)

  inline SceGxmTexture* GetGXMTexture();
  inline void* GetGXMMemory() { return m_GxmMemory.GetAddress(); }
  inline bool IsValid() { return m_bUsesExternalMemory || ( m_GxmMemory.GetAddress() != NULL ); }
  inline SceGxmTextureFormat GetGXMTextureFormat() { return m_GXMFormat; }

  /// \brief
  ///    Sets the internal sampler state. Only supported on Playstation Vita.
  void SetSamplerStateGXM(const VStateGroupSampler &state);

  /// \brief
  ///    Set the texture wrap mode. Only supported on Playstation Vita.
  bool SetWrapModeGXM(VTextureAddressMode wrapS, VTextureAddressMode wrapT);

  /// \brief
  ///    Set texture filter mode. Only supported on Playstation Vita.
  bool SetFilterModeGXM(VFilterMode filterMode, int iAnisoLevel);

  /// \brief
  ///    Set LOD bias. Only supported on Playstation Vita.
  bool SetLODBiasGXM(float bias);

  /// \brief
  ///    Translates VTextureAddressMode to GXMTexWrapMode. Only supported on Playstation Vita.
  static SceGxmTextureAddrMode ToGXMTexWrapMode(VTextureAddressMode mode);

  /// \brief
  ///    Obtains the minification filter corresponding to a given VFilterMode. Only supported on Playstation Vita.
  static SceGxmTextureFilter ToGXMMinFilter(VFilterMode filterMode, int &iAnisoLevel);

  /// \brief
  ///    Obtains the magnification filter corresponding to a given VFilterMode. Only supported on Playstation Vita.
  static SceGxmTextureFilter ToGXMMagFilter(VFilterMode filterMode, int &iAnisoLevel);

  /// \brief
  ///   Creates the internal device-specific texture for a VTextureObject using the given memory for storing the
  ///   actual texture data. [Playstation Vita only]
  ///
  /// This method is responsible for creating the internal device-specific representation of a texture. When calling 
  /// VisTextureManager_cl::CreateEmpty2DTextureObject or when directly constructing a VTextureObject, you need to call
  /// CreateDeviceHandle before you can use the texture for rendering. 
  /// When calling this method all responsibility for managing the given memory lies with the caller - including
  /// allocation, deallocation, locking etc.
  /// This method allows utilizing the same chunk of memory for retrieving camera data and texturing objects with it
  /// without the need for an additional memory copy.
  ///
  /// \param eOverrideFormat
  ///   Texture format to use. Use VTextureLoader::UNKNOWN if the format has previously been set and you don't want to
  ///   change it.
  ///
  /// \param pTextureMemory
  ///   Pointer to a chunk of memory to be used as texture memory. The caller is responsible for all memory management.
  ///
  /// \param bRenderTarget
  ///   Pass true if you want to create a render target texture. Note that simply setting this to true won't allow you
  ///   to use the texture as a render target; for this purpose, you should always use a VisRenderableTexture_cl object.
  ///
  /// \param bAutoGenMipmaps
  ///   True if automatic generation of mipmaps is desired. Note that this is currently only fully supported on PC.
  /// 
  /// \returns
  ///   TRUE if creation of the device-specific object was successful, otherwise FALSE.
  VBASE_IMPEXP BOOL CreateDeviceHandle(VTextureLoader::VTextureFormat_e eOverrideFormat, void *pTextureMemory, 
    bool bRenderTarget = false, bool bAutoGenMipmaps = false);

#endif

  /// \brief
  ///   Returns whether texture's dimensions are a power of two
  inline bool IsTexturePowerOfTwo() const;

  /// \brief
  ///   Returns the size of a specific mipmap level in bytes.
  VBASE_IMPEXP virtual int GetMipLevelSize(int iMipLevel);

  /// \brief
  ///   Returns the offset of a specific mipmap level (from that start of the texture data in memory) in bytes. 
  ///
  /// This is typically only useful for platforms which allow direct access to the texture memory (consoles).
  ///
  /// \param iMipLevel
  ///   The mip level for which the offset should be retrieved.
  VBASE_IMPEXP virtual int GetMipLevelOffset(int iMipLevel);

  ///
  /// @}
  ///

  ///
  /// @name Updating / modifying textures
  /// @{
  ///

  /// \brief
  ///   Creates the internal device-specific texture for a VTextureObject.
  ///
  /// This method is responsible for creating the internal device-specific representation of a texture. When calling 
  /// VisTextureManager_cl::CreateEmpty2DTextureObject or when directly constructing a VTextureObject, you need to call
  /// CreateDeviceHandle before you can use the texture for rendering. This function is internally called by the
  /// VTextureObject::Reload() method.
  ///
  /// \param eOverrideFormat
  ///   Texture format to use. Use VTextureLoader::UNKNOWN if the format has previously been set and you don't want to
  ///   change it.
  ///
  /// \param bRenderTarget
  ///   Pass true if you want to create a render target texture. Note that simply setting this to true won't allow you
  ///   to use the texture as a render target; for this purpose, you should always use a VisRenderableTexture_cl object.
  ///
  /// \param bAutoGenMipmaps
  ///   True if automatic generation of mipmaps is desired. Note that this is currently only fully supported on PC.
  /// 
  /// \returns
  ///   TRUE if creation of the device-specific object was successful, otherwise FALSE.
  VBASE_IMPEXP BOOL CreateDeviceHandle(VTextureLoader::VTextureFormat_e eOverrideFormat, bool bRenderTarget = false, 
    bool bAutoGenMipmaps = false);

  /// \brief
  ///   Returns whether the texture currently has a device-specific representation.
  VBASE_IMPEXP bool HasDeviceHandle();

  /// \brief
  ///   Releases the device-specific representation of the texture and frees all memory allocated by it.
  VBASE_IMPEXP void ReleaseDeviceHandle();

  /// \brief
  ///   Updates a given mipmap level of the texture.
  ///
  /// This method updates a specific mipmap level of a texture, either partially or completely. It will not touch other
  /// mipmap levels of the texture. The source representation of the image data (texel format) has to match the internal
  /// representation. No endianness adjustments will be made. The source data will be uploaded as-is.
  ///
  /// \param iMipLevel
  ///   The mipmap level to modify. 0 represents the highest mipmap level (at the full size of the texture).
  ///
  /// \param x1
  ///   Horizontal offset of the rectangle of the mipmap level that is to be replaced. For block-compressed formats, this needs
  ///   to be a multiple of 4.
  ///
  /// \param y1
  ///   Vertical offset of the rectangle of the mipmap level that is to be replaced. For block-compressed formats, this needs to
  ///   be a multiple of 4.
  ///
  /// \param iWidth
  ///   Width of the rectangle of the mipmap level that is to be replaced. Note that the rectangle has to fully be
  ///   inside the mipmap level of the texture. For block-compressed formats, this needs to be a multiple of 4.
  ///
  /// \param iHeight
  ///   Height of the rectangle of the mipmap level that is to be replaced. Note that the rectangle has to fully be
  ///   inside the mipmap level of the texture. For block-compressed formats, this needs to be a multiple of 4.
  ///
  /// \param iSrcStride
  ///   Stride (in bytes) between subsequent rows of pixels in the source data. Note that, in block-compressed data
  ///   formats, this is the stride (in bytes) between subsequent rows of blocks of pixels in the source data. A block is typically 4x4
  ///   texels for block-compressed formats.
  ///
  /// \param pData
  ///   Pointer to the source data.
  ///
  /// \param iLockFlags
  ///   Lock flags that specify how to modify the data. The following lock flags are valid:
  ///   \li V_TEXTURE_LOCKFLAG_DISCARDABLE: Set this flag if all the data in the mipmap levels will be overwritten, or
  ///       if the data currently in the texture can be discarded (e.g. because it is already known that it will be
  ///       overwritten later).
  ///   \li V_TEXTURE_LOCKFLAG_NOOVERWRITE: Set this flag if you can guarantee that no data being overwritten
  ///       is still in use by the GPU. This can be used to ensure that the CPU will never wait for the GPU before updating
  ///       the texture (which can drastically increase performance).
  ///   \li V_TEXTURE_LOCKFLAG_NOCOMPONENTSWIZZLE: Specify this flag if component swizzling should never occur. Component swizzling
  ///       may be required in order to pad 24bpp formats like RGB/BGR to the device-specific format; however, it can
  ///       cost a lot of time. By specifying V_TEXTURE_LOCKFLAG_NOCOMPONENTSWIZZLE, you can be sure that the data will be uploaded as-is.
  ///       Setting this flag could be useful to speed up modifications of RGB/BGR textures, by already including the alpha component in
  ///       the source color buffer on platforms that do not support 24bit colors.
  ///
  /// \param iFaceOrSlice
  ///   For cubemaps or texture arrays, this indicates the cubemap face / texture array slice to be updated.
  ///
  /// \returns
  ///   TRUE if updating the texture was successful, otherwise false.
  VBASE_IMPEXP bool UpdateRect(int iMipLevel, int x1, int y1, int iWidth, int iHeight, int iSrcStride, const void *pData, int iLockFlags, int iFaceOrSlice = 0);
  
  /// \brief
  ///   Returns whether this texture has been loaded with VTM_FLAG_ASSUME_SRGB flag
  inline bool IsSRGB() const 
  {
    return (m_iLoadingFlags&VTM_FLAG_ASSUME_SRGB)!=0;
  }

  /// \brief
  ///   Modifies the VTM_FLAG_SRGB flag. If it differs from the current state, the texture is forced to be reloaded.
  VBASE_IMPEXP void SetSRGB(bool bNewStatus);

  #ifdef _VR_GCM

  /// \brief
  ///   Specifies whether GPU Swizzling should be used.
  ///
  /// CPU-side swizzling takes a lot of time. Therefore, conversion to swizzled formats inside the UpdateRect() function
  /// can optionally be performed on the GPU, which is typically about 10 times faster than CPU-side swizzling. Using
  /// GPU swizzling requires a temporary buffer in mapped system memory, which is why you should always keep a sufficient
  /// amount of mapped system memory around when using GPU-side swizzling.
  ///
  /// If GPU-side swizzling fails due to insufficient mapped system memory, swizzling will silently be performed on the
  /// CPU instead.
  ///
  /// Set GPU swizzling to true if you experience bad performance when using UpdateRect() on uncompressed, swizzled textures.
  /// The default is false.
  ///
  /// \param bStatus
  ///   true to enable GPU-side swizzling, false to always perform swizzling on the CPU.
  static inline void SetUseGPUSwizzling(bool bStatus) { s_bUseGPUSwizzling = bStatus; }

  /// \brief
  ///   Returns the value previous set with SetUseGPUSwizzling().
  static inline bool GetUseGPUSwizzling() { return s_bUseGPUSwizzling; }

  #endif
  

  ///
  /// @}
  ///

  ///
  /// @name Resource Manager
  /// @{
  ///

  /// \brief
  ///   Returns the texture manager this resource belongs to.
  inline VTextureManager *GetParentTextureManager() const 
  {
    return (VTextureManager *)VManagedResource::GetParentManager();
  }

  /// \brief
  ///   Implements the IVSerializationProxyCreator interface.
  VBASE_IMPEXP VOVERRIDE IVSerializationProxy *CreateProxy() 
  {
    return new VTextureSerializationProxy(this);
  }

  /// \brief
  ///   Static helper function to (de-)serialize a texture reference. Handles texture types/flags gracefully. The serialized content is compatible with VTextureSerializationProxy
  VBASE_IMPEXP static VTextureObject *DoArchiveExchange(VArchive &ar, VTextureObject* pSource=NULL, int iAdditionalFlags=0);

  /// \brief
  ///   Overridden function that additionally saves out the texture flags in the texture object's VResourceSnapshotEntry. This function ignores
  ///   textures that do not come from a file
  VBASE_IMPEXP VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);

  ///
  /// @}
  ///

  ///
  /// @name Static helper functions
  /// @{
  ///

  /// \brief
  ///   Converts forward slashes to back slashes in the texture filename.
  VBASE_IMPEXP static bool ConvertTextureFilename(char *szDest, const char *szSrc);
  
#ifdef _VISION_XENON

  /// \brief
  ///   Returns whether the passed format is a linear format. Xbox360 only.
  VBASE_IMPEXP static bool IsLinearFormat(VTextureLoader::VTextureFormat_e eUploadFormat);

#endif


  /// \brief
  ///   Queries the hardware for a depth format which can be used as a texture
  ///
  /// Not all depth or depth-stencil formats can be used as textures (i.e., accessed in shaders).
  /// This function returns the platform's default depth texture format or UNKNOWN if no format is supported.
  ///
  /// \param videoConfig
  ///   The parameter may be needed on some platforms, use *Vision::Video.GetCurrentConfig().
  VBASE_IMPEXP static VTextureLoader::VTextureFormat_e GetCompatibleDepthTextureFormat(const VVideoConfig& videoConfig);

  /// \brief
  ///   Returns the number of bits per pixel for a given texture format.
  VBASE_IMPEXP static unsigned int GetBitsPerPixel(VTextureLoader::VTextureFormat_e eFormat);

  /// \brief
  ///   Returns whether the passed format is a depth-stencil format.
  VBASE_IMPEXP static bool IsDepthStencilFormat(VTextureLoader::VTextureFormat_e eFormat);

  /// \brief
  ///   Returns whether the passed format is a block-compression format (e.g. DXT).
  VBASE_IMPEXP static bool IsFormatCompressed(VTextureLoader::VTextureFormat_e eFormat);


  ///
  /// @}
  ///

#ifndef _VISION_DOC

  VBASE_IMPEXP static int CountMipLevels(int iWidth, int iHeight);
  VBASE_IMPEXP static int CountMipLevelsTill4x4(int iWidth, int iHeight);
  VBASE_IMPEXP bool GetReplacementTexture(char *pszFileName, bool &bIsNativeFormat);

  // Custom texture loading which is used on different platforms to directly load
  // platform specific file formats which can't be loaded by the texture loader.
  VBASE_IMPEXP bool LoadCustomTexture(const char* pszFileName);
  
  #if defined(WIN32) && defined(_VR_DX9)
    bool CopyRectWithSysmemProxy(int iMipLevel, int x1, int y1, int iWidth, int iHeight, const char *pSourceData, int iSrcStride, int iFace);
    inline D3DPOOL GetPoolType() const
    {
      if (m_iLoadingFlags & (VTM_FLAG_NONMANAGEDPOOL|VTM_FLAG_DYNAMIC))
        return D3DPOOL_DEFAULT;
      return VVideo::GetD3D9ExtendedUsage() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
    }
  #endif

  VBASE_IMPEXP void CreateMipmaps();
  inline bool IsUpsideDown() const { return m_bIsUpsideDown; }


  /// \brief
  ///   Associates animation info with this texture (for internal use only)
  VBASE_IMPEXP void SetAnimationInfo(TextureAnimInfo_t *pInfo);

  inline void EnableUnloading() {SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);}
  inline void DisableUnloading() {RemoveResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);}

#if defined(_VR_DX9)

  VBASE_IMPEXP DWORD GetD3D9LockFlags(int iLockFlags);
  VBASE_IMPEXP static D3DFORMAT ToD3DDepthStencilFormat(VTextureLoader::VTextureFormat_e eFormat);

  #if defined(_VISION_XENON)
    VBASE_IMPEXP static D3DFORMAT ToD3DTextureFormat(VTextureLoader::VTextureFormat_e eFormat, bool bExpand=false);
    VBASE_IMPEXP static D3DFORMAT ToD3DRenderTargetFormat(VTextureLoader::VTextureFormat_e eFormat);
    VBASE_IMPEXP static VTextureLoader::VTextureFormat_e ToResolvedRenderTargetFormat(VTextureLoader::VTextureFormat_e eFormat);
    VBASE_IMPEXP static bool CheckFilteringSupport(VTextureLoader::VTextureFormat_e eFormat);
  
  #else
    VBASE_IMPEXP static D3DFORMAT ToD3DTextureFormat(VTextureLoader::VTextureFormat_e eFormat);
  
  #endif

#elif defined(_VR_DX11)

  VBASE_IMPEXP static DXGI_FORMAT ToD3DTextureFormat(VTextureLoader::VTextureFormat_e eFormat, bool bSRGB = false);
  VBASE_IMPEXP static DXGI_FORMAT ToD3DTypelessFormat(VTextureLoader::VTextureFormat_e eFormat, bool bSRGB = false);
  VBASE_IMPEXP static DXGI_FORMAT ToD3DDepthStencilFormat(VTextureLoader::VTextureFormat_e eFormat);

#elif defined( _VR_GCM )

  VBASE_IMPEXP static bool ToGCMTextureFormat(VTextureLoader::VTextureFormat_e eFormat, uint8_t& iGCMFormatOut, 
    uint32_t& iGCMRemapOut);
  VBASE_IMPEXP static bool ToGCMDepthStencilFormat(VTextureLoader::VTextureFormat_e eFormat, uint8_t& iGCMFormatOut);
  VBASE_IMPEXP static bool FindMatchingColorFormatForSurface(uint8_t iGCMTextureFormat, uint8_t& iGCMSurfaceFormatOut);
  VBASE_IMPEXP static bool GetColorRemappingForSurface(uint8_t iGCMFormat, uint32_t& iGCMRemapOut);
  VBASE_IMPEXP static bool CheckFilteringSupport(VTextureLoader::VTextureFormat_e eFormat);
  VBASE_IMPEXP static bool CheckFastClearSupport(VTextureLoader::VTextureFormat_e eFormat);
  inline bool SupportsFiltering() const { return m_bSupportsFiltering; }
  inline bool SupportsFastClear() const { return m_bSupportsFastClear; }

  VBASE_IMPEXP void UpdateGCMTexture( VTextureLoader* TextureLoader = NULL, bool Cubemap = false, uint32_t Depth = 1) { FillGcmTextureInfo( TextureLoader, false, Depth, false ); }

#elif defined(_VR_GXM)

  VBASE_IMPEXP static bool ToGXMTextureFormat(VTextureLoader::VTextureFormat_e eFormat, SceGxmTextureFormat &gxmTextureFormat);
  VBASE_IMPEXP static bool ToGXMDepthStencilFormat(VTextureLoader::VTextureFormat_e eFormat, 
    SceGxmDepthStencilFormat& gxmDepthStencilFormat, SceGxmTextureFormat& gxmTextureFormat, SceGxmDepthStencilSurfaceType& gxmMemoryLayout,
    int &iBytesPerPixel);
  VBASE_IMPEXP static bool ToGXMColorFormat(VTextureLoader::VTextureFormat_e eFormat, SceGxmColorFormat &gxmColorFormat);
  VBASE_IMPEXP static bool CheckFilteringSupport(VTextureLoader::VTextureFormat_e eFormat);
  void *AllocateTextureMemory(SceKernelMemBlockType type, int size, int align, int flags);

  void SetFragmentNotification(volatile uint32_t *address, uint32_t value);
  void WaitForSceneNotification();

#elif defined(_VR_GLES2)

  VBASE_IMPEXP static bool ToGLESTextureFormatAndType(VTextureLoader::VTextureFormat_e eFormat, int& iFormatOut, 
    int& iType, bool& bIsCompressedFormat);  
  
  VBASE_IMPEXP static bool ToGLESRenderTargetFormatAndType(VTextureLoader::VTextureFormat_e eFormat, int& iFormatOut, 
    int& iRenderBufferFormat, int& iType, bool& bIsCompressedFormat);  
  
  VBASE_IMPEXP static bool ToGLESDepthStencilFormatAndType(VTextureLoader::VTextureFormat_e eFormat, int& iFormatOut, 
    int& iRenderBufferFormat, int& iType, bool& bIsCompressedFormat);

  VBASE_IMPEXP static const char* GetGLES2TextureExtensionString(VTextureLoader::VTextureFormat_e eFormat);

  VBASE_IMPEXP static const char* GetGLES2DepthStencilExtensionString(VTextureLoader::VTextureFormat_e eFormat);
 
  VBASE_IMPEXP static bool GetPlatformSpecificTextureFileName(const char* pszOriginalFilename, char* pszOutFilename, 
    IVFileStreamManager* pFileManager, bool &bIsNativeFormat);


#elif defined(_VR_GX2)
  VBASE_IMPEXP static GX2SurfaceFormat ToGX2Format(VTextureLoader::VTextureFormat_e eFormat, bool bsRGB);
  VBASE_IMPEXP static GX2SurfaceFormat ToGX2Format(VTextureLoader::VTextureFormat_e eFormat, bool bsRGB, bool& bTextureFormat, bool& bDepthFormat, bool& bColorBufferFormat);

  VBASE_IMPEXP static bool IsDepthFormatUsableAsTexture(VTextureLoader::VTextureFormat_e eFormat);

  /// \brief
  ///   Forces the component selection of the texture object to be XXXX (required for depth textures which only have a red channel)
  VBASE_IMPEXP void ForceTextureComponentSelection();
#endif

  VBASE_IMPEXP static int GetColorDepthFromTextureFormat(VTextureLoader::VTextureFormat_e eFormat);

protected:

  VBASE_IMPEXP virtual BOOL Load(const char* fileName);

  VBASE_IMPEXP virtual BOOL Reload();

  VBASE_IMPEXP virtual BOOL Unload();

  void AdjustFlagsFromMetadata(const char* szMetadata, unsigned int& iFlags);

  inline void SetUpsideDown(bool bStatus) 
  { 
    m_bIsUpsideDown = bStatus; 
  }

  /// \brief
  ///   Specifies whether this texture has been created "from memory" using custom data rather than from file.
  ///
  /// This tells the texture object that it does not correspond to a texture file.
  /// If this is the case, only a device handle is created using CreateDeviceHandle() when
  /// the object is created or reloaded. The texture data has to be set manually using UpdatedRect().
  inline void SetIsCreatedFromMemory(bool bStatus)
  { 
    m_bIsCreatedFromMemory = bStatus; 
  }

  /// \brief
  ///   Returns whether this texture has been created "from memory" using custom data rather than from file.
  inline bool IsCreatedFromMemory() const
  { 
    return m_bIsCreatedFromMemory; 
  }

  VBASE_IMPEXP void EnableDestroyHandleOnLostDevice();

  inline bool IsDestroyHandleOnLostDeviceEnabled() const
  { 
    return m_bDestroyHandleOnLostDevice; 
  }
  
  friend class VTextureManager;
  friend class VisionTextureManager;
  friend class VisTextureAnimInstance_cl;
  friend class VisTextureManager_cl;
  friend class VisRenderContext_cl;
  friend class VisStateHandler_cl;
  friend class VTextureSerializationProxy;

  // private static helper functions
  static int GetBlockSizeInTexels(VTextureLoader::VTextureFormat_e eFormat);
  static int GetBlockSizeInBytes(VTextureLoader::VTextureFormat_e eFormat);

  void Clear();

  VBASE_IMPEXP virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, 
    size_t &iDependentSys, size_t &iDependentGPU) HKV_OVERRIDE;

//#ifdef SUPPORTS_SNAPSHOT_CREATION
//  VBASE_IMPEXP virtual void CreateReplacementBlock(VResourceSnapshot &snapshot, VResourceSnapshotEntry &entry, const char *szReplacementFilename) HKV_OVERRIDE;
//#endif

  void CreateFromReplacementBlock(VResourceSnapshotEntry &entry);
  VBASE_IMPEXP int ComputeDownScaleFactor(); ///< assumes that texture sizes etc. are set
  VBASE_IMPEXP void SetPathname(const char *szName, bool convert=true);
  void Init(const VTextureLoader &loader);

#if defined(_VR_DX9) && defined(WIN32)
  VBASE_IMPEXP virtual void OnEnterBackground() HKV_OVERRIDE;
  VBASE_IMPEXP virtual void OnLeaveBackground() HKV_OVERRIDE;
#endif

#if defined(_VR_GCM)
  VBASE_IMPEXP void FillGcmTextureInfo( VTextureLoader* loader, bool Cubemap=false, 
    uint32_t Depth=1, bool RenderTarget=false);
#endif

#endif // _VISION_DOC

protected:
  // Protected members
  short m_iSizeX, m_iSizeY;
  char m_iDownScale;
  char m_iMipLevels;
  char m_eTextureType;    ///< casted to VTextureLoader::VTextureType_e
  unsigned char m_eTextureFormat;  ///< casted to VTextureLoader::VTextureFormat_e
  TextureAnimInfo_t *m_pAnimInfo;
  unsigned int m_iLoadingFlags;
  short m_iDepth;

  bool m_bDestroyHandleOnLostDevice;
  bool m_bIsUpsideDown;
  bool m_bIsVertexTexture;
  bool m_bIsCreatedFromMemory;

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )
  VShaderResourceViewPtr  m_spShaderResourceView;
  VUnorderedAccessViewPtr m_spUnorderedAccessView;
#endif

public:
  // Platform-specific data
#if defined(_VR_DX9) 
  IInternalTextureObjectPtr m_spD3Dinterface;

#elif defined(_VR_DX11)
  D3DResourcePtr m_spD3DResource;

#elif defined(_VR_GCM)
  bool m_bSupportsFiltering;
  bool m_bSupportsFastClear;

  CellGcmTexture m_GcmTexture;
  VGcmMemory m_GcmMemory;

  static bool s_bUseGPUSwizzling;
    
#elif defined(_VR_GLES2)
  unsigned int m_GLFormat;
  unsigned int m_GLHandle;
  VStateGroupSampler m_sgSampler;
  
#elif defined(_VR_GXM)
  SceGxmTexture m_GxmTexture;
  VGxmLockableMemory m_GxmMemory;
  SceGxmTextureFormat m_GXMFormat;
  bool m_bIsGxt;
  SceGxmNotification m_FragmentNotification;
  VStateGroupSampler m_sgSampler;
  bool m_bUsesExternalMemory;

#elif defined(_VR_GX2)
  VISION_ALIGN(PPC_IO_BUFFER_ALIGN) GX2Texture m_GX2Texture;
  void* m_pTextureMemory;
  void* m_pMipMemory;

#endif
};

typedef VSmartPtr<VTextureObject>   VTextureObjectPtr;


/// \brief
///   Internal structure that describes a set of textures used for texture animations
struct TextureAnimInfo_t
{
public:
  VBASE_IMPEXP TextureAnimInfo_t();
  inline ~TextureAnimInfo_t() {ReleaseFrames();}

  VBASE_IMPEXP void ReleaseFrames();
  VBASE_IMPEXP void AllocFrames(int count);
  VBASE_IMPEXP void SetFrameSpeed(int iFrame, float fTimeMs);
  inline float GetFrameSpeed(int iFrame) const 
  {
    VASSERT(iFrame>=0 && iFrame<numFrames);
    if (pFrameSpeed) return pFrameSpeed[iFrame];
    return speed;
  }

public:
  float speed;
  short numFrames;
  short animtype;   ///< ping pong...
  char  use_morph;
  char start_trigger;
  VTextureObjectPtr *spFrame;
  float *pFrameSpeed;
};




/// \brief
///   Class that represents a cubemap texture loaded from file. 
/// 
/// This class is derived from the texture object's base class VTextureObject.
/// The engine derives another class VisRenderableCubeMap_cl from VTextureCubeObject for renderable
/// cubemaps.
/// 
/// Cubemap textures (.dds) can be loaded from file through the global texture manager.
class VTextureCubeObject : public VTextureObject
{
public:

  /// \brief
  ///   Internal Constructor. Load cubemaps through teh global texture manager instead.
  VBASE_IMPEXP VTextureCubeObject(VResourceManager *pParentManager);

  
  #ifdef _VR_GCM

  /// \brief
  ///   Playstation 3 (GCM) only: Uses the RSX to efficiently compute mipmaps for 32bpp linear
  ///   textures. Pass iFace = -1 to generate mip maps for all faces.
  VBASE_IMPEXP bool ComputeMipmaps(int iFace);

  #endif
  

protected:
  VBASE_IMPEXP VOVERRIDE BOOL Load(const char* fileName);
};

typedef VSmartPtr<VTextureCubeObject> VTextureCubeObjectPtr;




/// \brief
///   Class that represents a 3D (volume-) texture. This class is derived from the texture object's
///   base class VTextureObject.
class VTexture3DObject : public VTextureObject
{
public:

  /// \brief
  ///   Internal Constructor. Use Vision::TextureManager.Create3DTextureObject or load via texture
  ///   manager instead
  VBASE_IMPEXP VTexture3DObject(VResourceManager *pParentManager);

  /// \brief
  ///   Overridden function. Returns the size of a specific mipmap level of the volume texture in bytes.
  VBASE_IMPEXP VOVERRIDE int GetMipLevelSize(int iMipLevel);

  /// \brief
  ///   Updates a given mipmap level of the texture.
  ///
  /// This method updates a specific mipmap level of a texture, either partially or completely. It will not touch other
  /// mipmap levels of the texture. The source representation of the image data (texel format) has to match the internal
  /// representation.
  ///
  /// \param iMipLevel
  ///   The mipmap level to modify. 0 represents the highest mipmap level (at the full size of the texture).
  ///
  /// \param x1
  ///   Horizontal offset of the rectangle of the mipmap level that is to be replaced. For block-compression formats, this needs
  ///   to be a multiple of 4.
  ///
  /// \param y1
  ///   Vertical offset of the rectangle of the mipmap level that is to be replaced. For block-compression formats, this needs to
  ///   be a multiple of 4.
  ///
  /// \param z1
  ///   Depth offset of the rectangle of the mipmap level that is to be replaced. For block-compression formats, this needs to
  ///   be a multiple of 4.
  ///
  /// \param iWidth
  ///   Width of the rectangle of the mipmap level that is to be replaced. Note that the rectangle has to fully be
  ///   inside the mipmap level of the texture. For block-compression formats, this needs to be a multiple of 4.
  ///
  /// \param iHeight
  ///   Height of the rectangle of the mipmap level that is to be replaced. Note that the rectangle has to fully be
  ///   inside the mipmap level of the texture. For block-compression formats, this needs to be a multiple of 4.
  ///
  /// \param iDepth
  ///   Depth of the rectangle of the mipmap level that is to be replaced. Note that the rectangle has to fully be
  ///   inside the mipmap level of the texture. For block-compression formats, this needs to be a multiple of 4.
  ///
  /// \param iSrcStride
  ///   Stride (in bytes) between subsequent rows of pixels in the source data. Note that, in block-compressed data
  ///   formats, this is the stride (in bytes) between subsequent rows of blocks of pixels in the source data. A block is typically 4x4
  ///   texels for block-compressed formats.
  ///
  /// \param iSliceStride
  ///   Stride (in bytes) between subsequent slices of the volume texture. 
  ///
  /// \param pData
  ///   Pointer to the source data.
  ///
  /// \param iLockFlags
  ///   Lockflags that specify how to modify the data. The following lock flags are valid:
  ///   \li V_TEXTURE_LOCKFLAG_DISCARDABLE: Set this flag if all the data in the mipmap levels will be overwritten, or
  ///       if the data currently in the texture can be discarded (e.g. because it is already known that it will be
  ///       overwritten later).
  ///   \li V_TEXTURE_LOCKFLAG_NOOVERWRITE: Set this flag if you can guarantee that no data being overwritten
  ///       is still in use by the GPU. This can be used to ensure that the CPU will never wait for the GPU before updating
  ///       the texture (which can drastically increase performance).
  ///   \li V_TEXTURE_LOCKFLAG_NOCOMPONENTSWIZZLE: Specify this flag if component swizzling should never occur. Component swizzling
  ///       may be required in order to pad and convert formats like 24bpp BGR to the device-specific format; however, it can
  ///       cost a lot of time. By specifying V_TEXTURE_LOCKFLAG_NOCOMPONENTSWIZZLE, you can be sure that the data will never
  ///       be converted before uploading.
  ///
  /// \returns
  ///   true if updating the texture was successful, otherwise false.
  VBASE_IMPEXP bool UpdateBox(int iMipLevel, int x1, int y1, int z1, int iWidth, int iHeight, int iDepth, int iSrcStride, int iSliceStride, const void *pData, int iLockFlags);

protected:
  friend class VTextureManager;
  friend class VisionTextureManager;
  friend class VisTextureManager_cl;

  inline void Init(const VTextureLoader &loader)
  {
    VTextureObject::Init(loader);
    m_iDepth = (short)loader.m_Header.dwDepth;
  }

  VBASE_IMPEXP VOVERRIDE BOOL Load(const char* fileName);
};


#if defined(SUPPORTS_2D_TEXTURE_ARRAYS) || defined(_VISION_DOC)

/// \brief
///   Class that represents a 2D texture array. This class is derived from the texture object's
///   base class VTextureObject.
/// 
/// 2D Texture Arrays can be loaded through .texanim files using e.g.
/// Vision::TextureManager.Load2DArrayTexture(). 
/// 
/// Only available on PC DX10/DX11.
class VTexture2DArrayObject : public VTextureObject
{
public:

  /// \brief
  ///   Internal Constructor. Use Vision::TextureManager.Create2DArrayTextureObject or load via
  ///   texture manager instead.
  VBASE_IMPEXP VTexture2DArrayObject(VResourceManager *pParentManager);

  /// \brief
  ///   Overridden function that adds the slices as separate streaming files
  VBASE_IMPEXP VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);

protected:
  friend class VTextureManager;
  friend class VisionTextureManager;
  friend class VisTextureManager_cl;

  VBASE_IMPEXP VOVERRIDE BOOL Reload();
};

typedef VSmartPtr<VTexture2DArrayObject> VTexture2DArrayObjectPtr;

#endif
  
#if defined(_VR_GLES2)
  
  inline unsigned int GetGLTexture();
  
#endif


typedef VSmartPtr<VTexture3DObject> VTexture3DObjectPtr;



#include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.inl>


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
