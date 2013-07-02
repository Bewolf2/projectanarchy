/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTextureFileHeader.hpp

#ifndef _TEXTUREFILEHEADER_IMP_HPP
#define _TEXTUREFILEHEADER_IMP_HPP


#ifndef _VISION_DOC


#pragma pack(push)
#pragma pack(1)


#define BMPMAGIC      19778
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

struct BMPFileHeader_t
{
  unsigned short magic;
  unsigned long filesize;
  unsigned short reserved1;
  unsigned short reserved2;
  unsigned long offbits;
};

struct BMPInfoHeader_t
{
  unsigned long  headersize;
  long           width;
  long           height;
  unsigned short planes;
  unsigned short bpp;
  unsigned long  compression;
  unsigned long  imagesize;
  long           xpelspermeter;
  long           ypelspermeter;
  unsigned long  clrused;
  unsigned long  clrimportant;
};

struct BMPCoreHeader_t
{
  unsigned long  headersize;
  short          width;
  short          height;
  unsigned short planes;
  unsigned short bpp;
};
  
union BMPInfo_u
{
  BMPInfoHeader_t infoheader;
  BMPCoreHeader_t coreheader;
};

struct BMP_RGBQuad_t
{
  unsigned char r, g, b, reserved;
};

struct BMP_RGBTriple_t
{
  unsigned char r, g, b;
};

union BMP_RGB_u
{
  BMP_RGBQuad_t   rgbquad[256];
  BMP_RGBTriple_t rgbtriple[256];
};

/*
** PCX
*/

#define PCXHEADERMAGIC  '\x0A'
#define PCXPALETTEMAGIC '\x0C'
#define PCXLOWESTVER    5

struct PCX_RGBTriple
{
  unsigned char b, g, r;
};

struct PCXHeader_t
{
  char           magic;
  char           version;
  char           encoding;
  char           bpp;
  unsigned short xmin, ymin, xmax, ymax;
  unsigned short hres, vres;
  char           obsoletecolormap[48];
  char           reserved;
  char           nplanes;
  unsigned short bytesperline;
  unsigned short paletteinfo;
  char           unused[58];
};

struct PCXPalette_t
{
  char          magic;
  PCX_RGBTriple rgb[256];
};



/*
** TGA
*/

struct ColorMapSpec_t
{
  short         firstentry;
  short         nrentries;
  unsigned char entrysize;
};

struct ImageSpec_t
{
  short         xorigin, yorigin;
  short         xsize, ysize;
  unsigned char bpp;
  unsigned char imagedescriptor;
};

struct TGAHeader_t
{
  unsigned char  idlen;
  unsigned char  colormaptype;
  unsigned char  imagetype;
  ColorMapSpec_t colormapspec;
  ImageSpec_t    imagespec;
};



/*
** TIFF
*/

#define TIFFMAGIC 42

#define TIFF_IMAGEWIDTH                 0x100
#define TIFF_IMAGELENGTH                0x101
#define TIFF_BITSPERSAMPLE              0x102
#define TIFF_COMPRESSION                0x103
#define TIFF_PHOTOMETRICINTERPRETATION  0x106
#define TIFF_STRIPOFFSETS               0x111
#define TIFF_SAMPLESPERPIXEL            0x115
#define TIFF_ROWSPERSTRIP               0x116
#define TIFF_STRIPBYTECOUNTS            0x117
#define TIFF_COLORMAP                   0x140


struct TIFFHeader_t
{
  unsigned short byteorder;
  short          magic;
  unsigned long  ifdoffset;
};


struct TIFFDirEntry_t
{
  unsigned short tag;
  unsigned short type;
  unsigned long  count;
  union
  {
    unsigned long  ulval;
    signed long    slval;
    unsigned short usval;
    signed short   ssval;
    unsigned char  ubval;
    signed char    sbval;
  };
};



#define TEXMAGIC 21590

struct TexHeader_t
{
  short          width;
  short          height;
  char           mip;
  char           sprite;
  char           maptypes;
  char           numframes;
  short          frametiming;
  char           animtype;
  char           colordepth;
  char           morphing;
  char           status;
  char           mapanims;
  char           reserved[2];
  unsigned short magic;
};

//from dds.h to avoid ddraw.h (from the DirectX SDK)
#define V_DDS_ALPHAPIXELS      0x00000001  // DDPF_ALPHAPIXELS
#define V_DDS_ALPHA            0x00000002  // DDPF_ALPHA
#define V_DDS_FOURCC           0x00000004  // DDPF_FOURCC
#define V_DDS_PALETTEINDEXED8  0x00000020  // DDPF_PALETTEINDEXED8
#define V_DDS_RGB              0x00000040  // DDPF_RGB
#define V_DDS_RGBA             0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define V_DDS_LUMINANCE        0x00020000  // DDPF_LUMINANCE

#define DDS_HEADER_FLAGS_TEXTURE  0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE


/*
** DDS [S]
*/


typedef struct {
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwFourCC;
  DWORD dwRGBBitCount;
  DWORD dwRBitMask;
  DWORD dwGBitMask;
  DWORD dwBBitMask;
  DWORD dwRGBAlphaBitMask;
}V_DDSPIXELFORMAT;

typedef struct {
  DWORD dwCaps1;
  DWORD dwCaps2;
  DWORD dwReserved[2];
}V_DDSCAPS;

typedef struct {
  DWORD dwMagic;
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwHeight;
  DWORD dwWidth;
  DWORD dwPitchOrLinearSize;
  DWORD dwDepth;
  DWORD dwMipMapCount;
  DWORD dwReserved1[11];
  V_DDSPIXELFORMAT ddpfPixelFormat;
  V_DDSCAPS ddsCaps;
  DWORD dwReserved2;
}V_DDSHEADER;

typedef struct {
  DWORD dxgiFormat;
  DWORD resourceDimension;
  DWORD miscFlags;
  DWORD arraySize;
  DWORD reserved;
}V_DDSHEADER_DXT10;

#pragma pack(pop)

#endif // _VISION_DOC



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
