/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTextureLoader.hpp

#ifndef VTEXTURELOADER_HPP_INCLUDED
#define VTEXTURELOADER_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Textures/VTextureFileHeader.hpp>
#include <Vision/Runtime/Base/Graphics/Textures/VCompressionHelper.hpp>

class VTextureLoader;

/// \brief
///   Helper class to read RGBA colors within a VTextureLoader. This class implements the IVFileInStream interface
class RGBAStream : public IVFileInStream
{
public:
// not documented
#ifndef _VISION_DOC
  VBASE_IMPEXP RGBAStream() : IVFileInStream(NULL) {m_pLoader=NULL;m_pUnpackedStreamPos=NULL;}
  VBASE_IMPEXP void SetLoader(VTextureLoader *pLoader);
  VBASE_IMPEXP VOVERRIDE void Close();
  VBASE_IMPEXP VOVERRIDE size_t Read(void* pBuffer,int iLen);
  VBASE_IMPEXP VOVERRIDE BOOL SetPos(LONG iPos, int iMode);
  VBASE_IMPEXP VOVERRIDE LONG GetPos();
  VBASE_IMPEXP VOVERRIDE LONG GetSize();
  VBASE_IMPEXP VOVERRIDE const char* GetFileName();
  VTextureLoader *m_pLoader;
  UBYTE *m_pUnpackedStreamPos;
#endif // _VISION_DOC
};

/// \brief
///   Generic low level texture loading class.
///
/// This class is used internally by all texture loading operations (VTextureObject, VisBitmap_cl) of the engine.
class VTextureLoader : public VBaseObject
{
public:

  /// \brief
  ///   Enum defining the texture resource types supported by Vision.
  enum VTextureType_e
  {
    Unknown        = 0,  ///< Unknown type
    Texture2D      = 1,  ///< Standard 2D texture
    Texture3D      = 2,  ///< Volume texture
    Cubemap        = 3,  ///< Cubemap texture
    Heightmap16bpp = 4,  ///< Not used
    Texture2DArray = 5,  ///< DX10 (and up): 2D texture array
    CubemapArray   = 6   ///< DX11 (and up): Cubemap Array (currently unsupported)
  };

  /// \brief
  ///   Enum that defines texture data formats. Note that not all of these formats are supported on all platforms.
  enum VTextureFormat_e
  {
    UNKNOWN         = 0,  ///< Unknown format
    R8G8B8          = 1,  ///< 24bit RGB
    R8G8B8A8        = 2,  ///< 32bit RGBA
    B8G8R8          = 3,  ///< 24bit BGR  
    B8G8R8A8        = 4,  ///< 32bit BGRA    
    DXT1A           = 5,  ///< DXT1 (1bit alpha) compressed format
    DXT1            = 6,  ///< technically the same as DXT1A
    DXT3            = 7,  ///< DXT3 compressed format
    DXT5            = 8,  ///< DXT5 compressed format
    B5G5R5          = 10, ///< 15bit BGR
    B5G6R5          = 11, ///< 16bit BGR
    B5G5R5A1        = 12, ///< 16bit BGRA (1 bit alpha)
    B4G4R4A4        = 13, ///< 16bit BGRA (4 bit each)    
    A8              = 14, ///< 8bit alpha texture (RGB=black)
    L8              = 15, ///< 8bit luminance texture
    L16             = 16, ///< 16bit luminance texture
    R16G16B16A16F   = 17, ///< 16bit floating point accuracy for each component (RGBA)
    R32G32B32A32F   = 18, ///< 32bit floating point accuracy for each component (RGBA)
    R16F            = 19, ///< single component with 16bit floating point accuracy
    R32F            = 20, ///< single component with 32bit floating point accuracy
    A1B5G5R5        = 30, ///< 16bit ABGR (1 bit alpha) (GLES and WiiU only)
    A4B4G4R4        = 31, ///< 16bit ABGR (4 bit each) (GLES only)
    R16G16F         = 60, ///< two components (RG) with 16bit floating point accuracy each
    R32G32F         = 61, ///< two components (RG) with 32bit floating point accuracy each
    R8G8            = 62, ///< two components (RG) with 8bit each
    R16G16          = 63, ///< two components (RG) with 16bit each
    R32G32          = 64, ///< two components (RG) with 32bit each
    R16G16B16A16    = 66, ///< 16bits fixed-point RGBA format
    R32G32B32A32    = 67, ///< 32bits fixed-point RGBA format
    D24S8           = 68, ///< Render target format with 24bit depth buffer and 8bit stencil buffer
    D24X8           = 69, ///< Render target format with 24bit depth buffer and padding
    D32             = 70, ///< Render target format with 32bit depth buffer
    D16             = 71, ///< Render target format with 16bit depth buffer
    L8A8            = 73, ///< 8bit each for luminance and alpha
    B10G10R10A2     = 74, ///< 10bit for each component (BGR) and 2bit alpha
    R11G11B10F      = 75, ///< Render target format with 10/11bit floating point components
    R9G9B9E5        = 76, ///< Floating point format with shared exponent. DX10 (and up)
    D24FS8          = 77, ///< Render target format with 24bit floating point depth buffer and 8bit stencil buffer
    R24X8           = 78, ///< R24X8 texture format
    D32F            = 79, ///< 32 bit floating-point depth format
    R10G10B10A2     = 80, ///< 10bit for each component (RGB) and 2bit alpha

#if defined(_VR_DX9)
    DF16        = 90, ///< ATI specific DX9 format (depth 16)
    DF24        = 91, ///< ATI specific DX9 format (depth 24)
    INTZ        = 92, ///< DX9 format supported by GeForce8+ and Radeon 4000+ (depth 24)
    RAWZ        = 93, ///< DX9 format for older GeForce cards (depth 24)
#endif

#ifdef _VISION_XENON
    R10G10B10A2F_RT   = 127, ///< Xbox360 specific: Render target-only 10bit floating point RGB components and 2 bits of alpha
    LIN_DXT1          = 128, ///< Xbox360 specific: DXT1 format in linear (untiled) layout. 
    LIN_DXT3          = 129, ///< Xbox360 specific: DXT3 format in linear (untiled) layout. 
    LIN_DXT5          = 130, ///< Xbox360 specific: DXT5 format in linear (untiled) layout. 
    LIN_A8            = 131, ///< Xbox360 specific: A8 format in linear (untiled) layout.
    LIN_L8            = 132, ///< Xbox360 specific: L8 format in linear (untiled) layout.
    LIN_L16           = 133, ///< Xbox360 specific: L16 format in linear (untiled) layout.
    LIN_B5G6R5        = 134, ///< Xbox360 specific: B5G6R5 format in linear (untiled) layout.
    LIN_B5G5R5A1      = 135, ///< Xbox360 specific: B5G5R5A1 format in linear (untiled) layout.
    LIN_B4G4R4A4      = 136, ///< Xbox360 specific: B4G4R4A4 format in linear (untiled) layout.
    LIN_B8G8R8A8      = 137, ///< Xbox360 specific: B8G8R8A8 format in linear (untiled) layout.
    LIN_R16G16B16A16F = 143, ///< Xbox360 specific: R16G16B16A16F format in linear (untiled) layout.
    LIN_R32G32B32A32F = 144, ///< Xbox360 specific: R32G32B32A32F format in linear (untiled) layout.
    LIN_R16F          = 145, ///< Xbox360 specific: R16F format in linear (untiled) layout.
    LIN_R32F          = 146, ///< Xbox360 specific: R32F format in linear (untiled) layout.
    LIN_R16G16F       = 147, ///< Xbox360 specific: R16G16F format in linear (untiled) layout.
    LIN_R32G32F       = 148, ///< Xbox360 specific: R32G32F format in linear (untiled) layout.
    LIN_R8G8          = 149, ///< Xbox360 specific: R8G8 format in linear (untiled) layout.
    LIN_R16G16        = 150, ///< Xbox360 specific: R16G16 format in linear (untiled) layout.
    LIN_R32G32        = 151, ///< Xbox360 specific: R32G32 format in linear (untiled) layout.
    LIN_R16G16B16A16  = 152, ///< Xbox360 specific: R16G16B16A16 format in linear (untiled) layout.
    LIN_R32G32B32A32  = 153, ///< Xbox360 specific: R32G32B32A32 format in linear (untiled) layout.
    LIN_D24S8         = 154, ///< Xbox360 specific: D24S8 format in linear (untiled) layout.
    LIN_D24X8         = 155, ///< Xbox360 specific: D24X8 format in linear (untiled) layout.
    LIN_D32           = 156, ///< Xbox360 specific: D32 format in linear (untiled) layout.
    LIN_D16           = 157, ///< Xbox360 specific: D16 format in linear (untiled) layout.
    LIN_L8A8          = 158, ///< Xbox360 specific: L8A8 format in linear (untiled) layout.
    R16G16B16A16_RT   = 159, ///< Xbox360 specific: Render target-only 16bits fixed-point RGBA format
    R16G16_RT         = 160, ///< Xbox360 specific: Render target-only 16bits fixed-point two-component (RG) format
    LIN_D24FS8        = 161, ///< Xbox360 specific: D24FS8 format in linear (untiled) layout.
    CTX1              = 162, ///< Xbox360 specific: Normalmap compression.
    DXN               = 163, ///< Xbox360 specific: Normalmap compression.
#endif
    
#ifdef _VR_GLES2
    PVRTC_RGBA_4BPP = 21,   ///< OpenGL ES 2.0 specific: PVRTC RGBA Block-compression format with 4 bits per pixel.
    PVRTC_RGBA_2BPP = 22,   ///< OpenGL ES 2.0 specific: PVRTC RGBA Block-compression format with 2 bits per pixel.
    PVRTC_RGB_4BPP = 23,    ///< OpenGL ES 2.0 specific: PVRTC RGB Block-compression format with 4 bits per pixel.
    PVRTC_RGB_2BPP = 24,    ///< OpenGL ES 2.0 specific: PVRTC RGB Block-compression format with 2 bits per pixel.
    ETC_RGB_4BPP = 25,      ///< OpenGL ES 2.0 specific: ETC RGB Block-compression format with 4 bits per pixel.
#endif

#ifdef _VISION_PSP2
    PVRT1_2BPP = 162,       ///< Playstation Vita specific: PVRT1 RGBA Block-compression format with 2 bits per pixel.
    PVRT1_4BPP = 163,       ///< Playstation Vita specific: PVRT1 RGBA Block-compression format with 4 bits per pixel.
    PVRT2_2BPP = 164,       ///< Playstation Vita specific: PVRT2 RGBA Block-compression format with 2 bits per pixel.
    PVRT2_4BPP = 165,       ///< Playstation Vita specific: PVRT2 RGBA Block-compression format with 4 bits per pixel.
#endif

#if defined(_VR_DX11) || defined(_VR_GX2)
    BC5S  = 200,     ///< DX11 specific: signed BC5
    BC5U  = 201,     ///< DX11 specific: unsigned BC5
    R8I   = 202,     ///< DX11 specific: 8-bit signed short
    R8UI  = 203,     ///< DX11 specific: 8-bit unsigned short
    R16I  = 204,     ///< DX11 specific: 16-bit signed short
    R16UI = 205,     ///< DX11 specific: 16-bit unsigned short
    R32I  = 206,     ///< DX11 specific: 32-bit signed int
    R32UI = 207,     ///< DX11 specific: 32-bit unsigned int
    R8G8B8A8S = 208, ///< DX11 specific: 32-bit RGBA (signed)
#endif

#if defined(_VR_DX9)
    DEFAULT_RENDERTARGET_FORMAT_24BPP  = B8G8R8,
    DEFAULT_RENDERTARGET_FORMAT_32BPP  = B8G8R8A8,
    DEFAULT_TEXTURE_FORMAT_24BPP       = B8G8R8,
    DEFAULT_TEXTURE_FORMAT_32BPP       = B8G8R8A8,
#else
    DEFAULT_RENDERTARGET_FORMAT_24BPP  = R8G8B8,
    DEFAULT_RENDERTARGET_FORMAT_32BPP  = R8G8B8A8,
    DEFAULT_TEXTURE_FORMAT_24BPP       = R8G8B8,
    DEFAULT_TEXTURE_FORMAT_32BPP       = R8G8B8A8,
#endif

    NONE        = 255
  };

#define ENUM_2_STRING(val) {if (format==val) return #val;}

  ///\brief
  ///Helper function that returns a string that matches the format enum value. szBuffer should hold enough characters 
  static inline const char *GetTextureFormatString(VTextureLoader::VTextureFormat_e format, char *szBuffer)
  {
    ENUM_2_STRING(UNKNOWN);
    ENUM_2_STRING(R8G8B8);
    ENUM_2_STRING(R8G8B8A8);
    ENUM_2_STRING(B8G8R8);
    ENUM_2_STRING(B8G8R8A8);
    ENUM_2_STRING(DXT1A);
    ENUM_2_STRING(DXT1);
    ENUM_2_STRING(DXT3);
    ENUM_2_STRING(DXT5);
    ENUM_2_STRING(B5G5R5);
    ENUM_2_STRING(B5G6R5);
    ENUM_2_STRING(B5G5R5A1);
    ENUM_2_STRING(B4G4R4A4);
    ENUM_2_STRING(A1B5G5R5);
    ENUM_2_STRING(A4B4G4R4);
    ENUM_2_STRING(A8);
    ENUM_2_STRING(L8);
    ENUM_2_STRING(L16);
    ENUM_2_STRING(R16G16B16A16F);
    ENUM_2_STRING(R32G32B32A32F);
    ENUM_2_STRING(R16F);
    ENUM_2_STRING(R32F);
    ENUM_2_STRING(R16G16F);
    ENUM_2_STRING(R32G32F);
    ENUM_2_STRING(R8G8);
    ENUM_2_STRING(R16G16);
    ENUM_2_STRING(R32G32);
    ENUM_2_STRING(R16G16B16A16);
    ENUM_2_STRING(R32G32B32A32);
    ENUM_2_STRING(D24S8);
    ENUM_2_STRING(R24X8);
    ENUM_2_STRING(D24FS8);
    ENUM_2_STRING(D24X8);
    ENUM_2_STRING(D32);
    ENUM_2_STRING(D16);
    ENUM_2_STRING(L8A8);
    ENUM_2_STRING(R10G10B10A2);
    ENUM_2_STRING(B10G10R10A2);
    ENUM_2_STRING(R11G11B10F);
    ENUM_2_STRING(R9G9B9E5);
    ENUM_2_STRING(D24FS8);
    ENUM_2_STRING(R24X8);
    ENUM_2_STRING(D32F);

#if defined(_VR_DX9)
    ENUM_2_STRING(DF16);
    ENUM_2_STRING(DF24);
    ENUM_2_STRING(INTZ);
    ENUM_2_STRING(RAWZ);
#endif

#ifdef _VISION_XENON
    ENUM_2_STRING(R10G10B10A2F_RT);
    ENUM_2_STRING(R16G16B16A16_RT);
    ENUM_2_STRING(R16G16_RT);
    ENUM_2_STRING(LIN_DXT1);
    ENUM_2_STRING(LIN_DXT3);
    ENUM_2_STRING(LIN_DXT5);
    ENUM_2_STRING(LIN_A8);
    ENUM_2_STRING(LIN_L8);
    ENUM_2_STRING(LIN_L16);
    ENUM_2_STRING(LIN_B5G6R5);
    ENUM_2_STRING(LIN_B5G5R5A1);
    ENUM_2_STRING(LIN_B4G4R4A4);
    ENUM_2_STRING(LIN_B8G8R8A8);
    ENUM_2_STRING(LIN_R16G16B16A16F);
    ENUM_2_STRING(LIN_R32G32B32A32F);
    ENUM_2_STRING(LIN_R16F);
    ENUM_2_STRING(LIN_R32F);
    ENUM_2_STRING(LIN_R16G16F);
    ENUM_2_STRING(LIN_R32G32F);
    ENUM_2_STRING(LIN_R8G8);
    ENUM_2_STRING(LIN_R16G16);
    ENUM_2_STRING(LIN_R32G32);
    ENUM_2_STRING(LIN_R16G16B16A16);
    ENUM_2_STRING(LIN_R32G32B32A32);
    ENUM_2_STRING(LIN_D24S8);
    ENUM_2_STRING(LIN_D24FS8);
    ENUM_2_STRING(LIN_D24X8);
    ENUM_2_STRING(LIN_D32);
    ENUM_2_STRING(LIN_D16);
    ENUM_2_STRING(LIN_L8A8);
    ENUM_2_STRING(CTX1);
    ENUM_2_STRING(DXN);
#endif

#ifdef _VR_GLES2
    ENUM_2_STRING(PVRTC_RGBA_4BPP);
    ENUM_2_STRING(PVRTC_RGBA_2BPP);
    ENUM_2_STRING(PVRTC_RGB_4BPP);
    ENUM_2_STRING(PVRTC_RGB_2BPP);   
    ENUM_2_STRING(ETC_RGB_4BPP);
#endif

#ifdef _VISION_PSP2
    ENUM_2_STRING(PVRT1_2BPP);
    ENUM_2_STRING(PVRT1_4BPP);
    ENUM_2_STRING(PVRT2_2BPP);
    ENUM_2_STRING(PVRT2_4BPP);
#endif

#if defined(_VR_DX11)
    ENUM_2_STRING(BC5S);
    ENUM_2_STRING(BC5U);
    ENUM_2_STRING(R8I);
    ENUM_2_STRING(R8UI);
    ENUM_2_STRING(R16I);
    ENUM_2_STRING(R16UI);
    ENUM_2_STRING(R32I);
    ENUM_2_STRING(R32UI);
    ENUM_2_STRING(R8G8B8A8S);
#endif

    ENUM_2_STRING(NONE);

    if(szBuffer)
    {
#pragma warning(push)
#pragma warning(disable:4996)
      sprintf(szBuffer,"0x%X",(int)format);
#pragma warning(pop)
    }
    return szBuffer;
  }

  /// \brief
  ///   Internal structure to hold a single mip level's size
  struct ImageInfo_t
  {
    short iSizeX, iSizeY, iSizeZ;
    int iDataSize, iDataOfs;
  };

  ///
  /// @name Initialization and Loading
  /// @{
  ///

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VTextureLoader();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP ~VTextureLoader();

  /// \brief
  ///  Opens a texture file for reading data. Called by the various texture loading functions in the Vision Engine.
  ///
  /// \param szFilename
  ///   Texture filename
  ///
  /// \param pManager
  ///   File manager that is used to load the texture
  ///
  /// \param iFlags
  ///   Additional loading flags. Same constants as used for loading textures in the engine (e.g. VTM_FLAG_NO_MIPMAPS for instance)
  ///
  /// \param eExpectedType
  ///   Expected texture type (e.g. 2D texture, volume texture, ...)
  ///
  /// \param pLog
  ///   Optional log that receives errors, warnings and info strings
  ///
  /// \returns
  ///   true if opening the file was successful
  ///
  VBASE_IMPEXP bool Open(const char *szFilename, IVFileStreamManager *pManager, int iFlags, VTextureType_e eExpectedType=Unknown, IVLog *pLog=NULL);

  /// \brief
  ///   This overload of Open operates on a previously opened file stream
  VBASE_IMPEXP bool Open(IVFileInStream *pStream, bool bCloseStream, int iFlags, VTextureType_e eExpectedType=Unknown, IVLog *pLog=NULL);

  /// \brief
  ///   Closes the file
  VBASE_IMPEXP void Close();

  /// \brief
  ///   Creates a small plain white texture of specified type. Can be used when loading fails.
  VBASE_IMPEXP void CreatePlainWhiteTexture(VTextureType_e eType = Texture2D);

  /// \brief
  ///   Creates a small texture with solid color of specified type. Can be used when loading fails.
  VBASE_IMPEXP void CreatePlainColorTexture(const VColorRef& vColor,VTextureType_e eType = Texture2D);

  ///
  /// @}
  ///

  ///
  /// @name Image Info
  /// @{
  ///

  ///\brief
  ///  Returns the number of images (e.g. slices in a 3D texture)
  inline int          GetImageCount() const               { return m_iImageCount; }

  ///\brief
  ///  Returns the number of mip maps (1 for no mipmaps)
  inline int          GetMipmapCount() const              { return (int)m_Header.dwMipMapCount; }

  ///\brief
  ///  Indicates whether the texture loader supplies a format that can be uploaded directly to the hardware
  inline bool         HasRawData() const                  { return m_bHasRawData; }

  ///\brief
  ///  Indicates whether the texture file format supports mipmaps
  inline bool         FormatSupportsMipmaps() const       { return m_bSupportsMipmaps; }

  ///\brief
  ///  Indicates whether the texture format is a compressed FourCC type (DDS format only)
  inline bool         IsFourCC() const                    { return ((m_Header.ddpfPixelFormat.dwFlags & V_DDS_FOURCC /*DDPF_FOURCC*/)>0) && (m_iMinMip>1); }

  ///\brief
  ///  Returns lock flags to be used with UpdateRect(), returns 0 if no special lock flags are needed.
  inline unsigned int GetLockFlags() const                { return m_iLockFlags; }

  ///
  /// @}
  ///

  ///
  /// @name Image raw data access
  /// @{
  ///

  ///\brief
  ///  Returns a data buffer that provides the raw data for the next mipmap image. Optionally fills the passed structure with mipmap image info.
  VBASE_IMPEXP UBYTE *NextRawImage(ImageInfo_t *pInfo=NULL);

  ///\brief
  ///  Returns a data buffer that provides the converted 32bit data for the next mipmap image. Optionally fills the passed structure with mipmap image info.
  VBASE_IMPEXP VColorRef *NextRGBAImage(ImageInfo_t *pInfo=NULL);

  ///\brief
  ///  Internal endianness conversion function.
  VBASE_IMPEXP void SwapImageData( void *pData, unsigned int DataSize );

  ///\brief
  ///  Returns a pointer to the image data for the passed mip map level and face/slice.
  ///  It will return a UBYTE raw buffer if the texture loader contains raw data (VTextureLoader::HasRawData returns true). It will return a VColorRef buffer otherwise.
  VBASE_IMPEXP void* GetImageDataPtr(int iMipMapIndex, int iFaceOrSlice, ImageInfo_t *pInfo);

  ///
  /// @}
  ///

  /// \brief
  ///   Maps a DXGI_FORMAT to an equivalent legacy D3D9 FourCC code if possible.
  ///
  /// \param dwDxgiFormat
  ///   The DXGI_FORMAT to map to a FourCC code.
  ///
  /// Returns an equivalent FourCC code, mapped as declared by http://msdn.microsoft.com/en-us/library/windows/desktop/hh308955%28v=vs.85%29.aspx,
  /// or 0xFFFFFFFF when no equivalent is found.
  static DWORD MapDxgiToFourCc(DWORD dwDxgiFormat);

  // public members
  int m_iLoadingFlags;                                   ///< internal flags
  VTextureType_e m_eTextureType;                         ///< texture type
  VTextureFormat_e m_eRawFormat;                         ///< texture format
  short m_iMinMip,m_iBitsPerPixel,m_iImageCount;         ///< dimensions
  int m_iRawDataSize, m_iRGBADataSize;                   ///< data sizes (raw and unpacked)
  bool m_bCompressed;                                    ///< true if data is in a compressed format
  bool m_bHasRawData;                                    ///< true if it has been read into the raw buffer, false if data has been unpacked
  bool m_bSupportsMipmaps;                               ///< true if it supports mipmaps

  VISION_ALIGN(32) V_DDSHEADER m_Header;                 ///< full image header information in DDS compatible format

  char m_SwizzleMask[5];                                 ///< internal component swizzle profile. Used to unpack data.

  friend class RGBAStream;

protected:

  void Clear();
  void FillBitmapInfo();
  bool ParseHeader();
  const char *GetEndiannessFormat(int iByteCount, int &iRepetitions);
  void AdjustEndianness();

  static void CopyLoaderProperties(VTextureLoader &dest, const VTextureLoader &src);
  
  // specific loading
  bool ReadDDS();
  bool ReadTEX();
  bool ReadTGA();
  bool ReadBMP();
  bool ReadJPG();
  bool ReadPNG();
  bool ReadTexAnim();
  bool ReadPVR();
  bool ReadGXT();

  // helper functions on unpack and flip color buffers
  bool ReadIntoBuffer(bool bRLECompressed=false, int iRowStride=-1, bool bFlipV=false, bool bFlipH=false);
  bool ReadIntoUnpackBuffer(bool bRLECompressed=false, int iRowStride=-1, bool flipV=false, bool flipH=false);
  bool ReadIntoRawBuffer(bool bRLECompressed=false, int iRowStride=-1, bool flipV=false, bool flipH=false);
  void FlipBufferHorizontal();
  void FlipBufferVertical();
  bool DecompressImage();

  bool m_bOwnsStream;  
  IVFileInStream *m_pStream;
  IVLog *m_pLog;
  short m_iCurrentRAWImage, m_iCurrentRGBAImage;
  ImageInfo_t m_BitmapInfo[16];
  ImageInfo_t m_BitmapInfoRGBA[16];
  unsigned int m_iLockFlags;
  DWORD m_iUnusedBitsMask;

  VColorRef *m_pUnpacked32;  
  UBYTE *m_pRawBuffer;       

  VTextureLoader *m_pAnimFrameLoader;
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
