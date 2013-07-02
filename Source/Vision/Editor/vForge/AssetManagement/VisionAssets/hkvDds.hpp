/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_DDS_HPP_INCLUDED
#define HKV_DDS_HPP_INCLUDED


struct DDS_PIXELFORMAT 
{
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwFourCC;
  DWORD dwRGBBitCount;
  DWORD dwRBitMask;
  DWORD dwGBitMask;
  DWORD dwBBitMask;
  DWORD dwABitMask;
};

struct DDS_HEADER 
{
  DWORD           dwSize;
  DWORD           dwFlags;
  DWORD           dwHeight;
  DWORD           dwWidth;
  DWORD           dwPitchOrLinearSize;
  DWORD           dwDepth;
  DWORD           dwMipMapCount;
  DWORD           dwReserved1[11];
  DDS_PIXELFORMAT ddspf;
  DWORD           dwCaps;
  DWORD           dwCaps2;
  DWORD           dwCaps3;
  DWORD           dwCaps4;
  DWORD           dwReserved2;
};


#define DDSD_CAPS             0x1 //Required in every .dds file.
#define DDSD_HEIGHT           0x2 //Required in every .dds file.
#define DDSD_WIDTH            0x4 //Required in every .dds file.
#define DDSD_PITCH            0x8 //Required when pitch is provided for an uncompressed texture.
#define DDSD_PIXELFORMAT   0x1000 //Required in every .dds file.
#define DDSD_MIPMAPCOUNT  0x20000 //Required in a mipmapped texture.
#define DDSD_LINEARSIZE   0x80000 //Required when pitch is provided for a compressed texture.
#define DDSD_DEPTH       0x800000 //Required in a depth texture.

#define DDSCAPS_COMPLEX      0x8 //Optional; must be used on any file that contains more than one surface (a mipmap, a cubic environment map, or mipmapped volume texture).
#define DDSCAPS_TEXTURE   0x1000 //Required
#define DDSCAPS_MIPMAP  0x400000 //Optional; should be used for a mipmap.

#ifndef DDPF_ALPHAPIXELS
  #define DDPF_ALPHAPIXELS    0x1 //Texture contains alpha data; dwRGBAlphaBitMask contains valid data.
  #define DDPF_ALPHA          0x2 //Used in some older DDS files for alpha channel only uncompressed data (dwRGBBitCount contains the alpha channel bitcount; dwABitMask contains valid data)
  #define DDPF_FOURCC         0x4 //Texture contains compressed RGB data; dwFourCC contains valid data.
  #define DDPF_RGB           0x40 //Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks (dwRBitMask, dwRBitMask, dwRBitMask) contain valid data.
  #define DDPF_YUV          0x200 //Used in some older DDS files for YUV uncompressed data (dwRGBBitCount contains the YUV bit count; dwRBitMask contains the Y mask, dwGBitMask contains the U mask, dwBBitMask contains the V mask)
  #define DDPF_LUMINANCE  0x20000 //Used in some older DDS files for single channel color uncompressed data (dwRGBBitCount contains the luminance channel bit count; dwRBitMask contains the channel mask). Can be combined with DDPF_ALPHAPIXELS for a two channel DDS file.
#endif

#define DDS_MAGIC       0x20534444u //'DDS '
#define DDS_FOURCC_DXT1 0x31545844u //'DXT1'
#define DDS_FOURCC_DXT3 0x33545844u //'DXT3'
#define DDS_FOURCC_DXT5 0x35545844u //'DXT5'


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
