/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTex.hpp

/**************************************************************************/
/*                                                                        */
/*                         VTex - Texture file support                    */
/*                                                                        */
/**************************************************************************/
/* The VTex library currently has support for loading the following       */
/* image file formats:                                                    */
/*    -Microsoft DirectDraw Surface (DDS)                                 */
/*    -Vision Texture Format (TEX)                                        */
/*    -Truevision Targa (TGA)                                             */
/*    -JPEG File Interchange Format (JFIF)                                */
/*   Legacy formats                                                       */
/*    -Microsoft Bitmap (except 1-bit and 4-bit) (BMP)                    */
/*    -Tagged Image File Format (uncompressed only) (TIFF)                */
/*    -Portable Network Graphics Format (PNG)                             */
/*    -Digital Elevation Map (Bryce only) (DEM)                           */
/**************************************************************************/
/* The VTex library is using the following third-party libraries:          */
/*    -libjpeg of the Independent JPEG Group                              */
/*    -libpng of the PNG Development Group                                */
/*    -zlib by Jean-loup Gailly and Mark Adler                            */
/**************************************************************************/

#ifndef _VTEX_HPP
#define _VTEX_HPP

#define VTEX_EXPFUNC VBASE_IMPEXP

// general errors
#define VTEX_ERR_NOMEM                 -20001
#define VTEX_ERR_INDEXOUTOFBOUNDS      -20002

// file read errors
#define VTEX_ERR_WRONGFILETYPE         -20101
#define VTEX_ERR_CANTOPEN              -20102
#define VTEX_ERR_CANTWRITE             -20103
#define VTEX_ERR_FILETYPENOTSUPPORTED  -20104
#define VTEX_ERR_FILEBROKEN            -20105

// file write errors
#define VTEX_ERR_IMAGENOTVALID         -20106

// colormap errors
#define VTEX_ERR_WRONGDIMENSIONS       -20201
#define VTEX_ERR_WRONGCOLORDEPTH       -20202

#define VTEX_ERR_NODEM                 -20203
#define VTEX_ERR_DEM_ALREADYSCALED     -20204

#ifndef _VISION_DOC

enum ColorDepth_e
{
  COLORDEPTH_8BPP  = 1,   ///< 8-bit greyscale heightmap/opacity/bump/glow
  COLORDEPTH_16BPP = 2,   ///< 16-bit greyscale heightmap/5-6-5 RGB
  COLORDEPTH_24BPP = 3,   ///< 8-8-8 RGB
  COLORDEPTH_32BPP = 4    ///< 32-bit greyscale heightmap
};

enum ImageFileType_e
{
  FILETYPE_NONE,
  FILETYPE_TEX,
  FILETYPE_T24,
  FILETYPE_BMP,
  FILETYPE_PCX_OBSOLETE,
  FILETYPE_TGA,
  FILETYPE_TIF, FILETYPE_TIFF_OBSOLETE,
  FILETYPE_JPG, FILETYPE_JPEG_OBSOLETE, FILETYPE_JFIF_OBSOLETE, FILETYPE_JIF_OBSOLETE,
  FILETYPE_PNG,
  FILETYPE_DEM,
  FILETYPE_DDS, FILETYPE_DDS_PAL8, FILETYPE_DDS_NORMAL8,
  FILETYPE_TEXANIM_REFS,
  FILETYPE_ANY = 0xFF
};

enum ScalePower2_e
{
  SCALE_UP,
  SCALE_DOWN,
  SCALE_NEAREST
};

enum AnimType_e
{
  ANIMTYPE_NOANIM = 0,
  ANIMTYPE_PERM_ROT = 1,
  ANIMTYPE_PERM_PINGPONG = 2,
  ANIMTYPE_SINGLE_ROTSEQ = 3,
  ANIMTYPE_SINGLE_PINGPONGSEQ = 4,
  ANIMTYPE_SINGLE_ROTSTEP = 5,
  ANIMTYPE_SINGLE_PINGPONGSTEP = 6,
  ANIMTYPE_ONEWAYSTEP = 7,
  ANIMTYPE_PERM_RANDOMSEQ = 8,
  ANIMTYPE_SINGLE_RANDOMSTEP = 9,
  ANIMTYPE_DIRECTDEFINITION = 10
};

enum ScaleToBPP_e
{
  SCALE_DOWN_TO_BPP,
  SCALE_TO_BOUNDS,
  CUT_HIGHER_BITS,
  SHIFT_TO_ZERO
};

// hint on what multiple maps in one image specify
enum ImageRawMapType_e
{
  RAWMAP_TEXTURE,
  RAWMAP_CUBEMAP,
  RAWMAP_VOLUMETEXTURE,
  RAWMAP_NORMALMAP,  
  RAWMAP_LOOKUP
};

// hint on what OpenGL image upload functions to use
enum ImageRawMapHint_e
{
  RAWMAP_RGB_PLAIN,  
  RAWMAP_DXT_COMPRESSED,
  RAWMAP_256_PALETTED,
  RAWMAP_LUMINANCE
};

// decoder to use
enum ImageRawDecoder_e
{
  RAWDECODER_TGA,
  RAWDECODER_TEX,
  RAWDECODER_DDS,
  RAWDECODER_NONE
};

enum RawExternalFormal_e 
{
  RAWEXTERNAL_UNKNOWN       = 0,
  RAWEXTERNAL_R8G8B8        = 1,
  RAWEXTERNAL_R8G8B8A8      = 2,
  RAWEXTERNAL_B8G8R8        = 3,
  RAWEXTERNAL_B8G8R8A8      = 4,
  RAWEXTERNAL_DXT1A         = 5,
  RAWEXTERNAL_DXT1          = 6,
  RAWEXTERNAL_DXT3          = 7,
  RAWEXTERNAL_DXT5          = 8,
  RAWEXTERNAL_INDEXED8      = 9,
  RAWEXTERNAL_B5G5R5        = 10,
  RAWEXTERNAL_B5G6R5        = 11,
  RAWEXTERNAL_B5G5R5A1      = 12,
  RAWEXTERNAL_B4G4R4A4      = 13,
  RAWEXTERNAL_L8            = 14,
  RAWEXTERNAL_A8            = 15,
  RAWEXTERNAL_R16G16B16A16F = 16,
  RAWEXTERNAL_R32G32B32A32F = 17,
  RAWEXTERNAL_R16F          = 18,
  RAWEXTERNAL_R32F          = 19,
  RAWEXTERNAL_L8A8          = 20
};

class ImageMap_cl
{
public:
  // A new ImageMap_cl object is created with dimensions width, height and depth (in bits), and image data data.
  VTEX_EXPFUNC ImageMap_cl(SLONG width, SLONG height, SLONG depth, const UBYTE *data);

  // A new ImageMap_cl object is created with dimensions width, height and depth (in ColorDepth_e enums),
  // and image data data.
  VTEX_EXPFUNC ImageMap_cl(SLONG width, SLONG height, ColorDepth_e depth, const UBYTE *data);

  // A new uninitialized ImageMap is created. This constructor should only be used when you are planning to use
  // the assignment operator of this object.
  // WARNING: When supplying uninitialized ImageMap objects to TEX API functions your application will crash.
  VTEX_EXPFUNC ImageMap_cl();

  // Copy constructor
  VTEX_EXPFUNC ImageMap_cl(const ImageMap_cl &map);

  // Destructor
  VTEX_EXPFUNC ~ImageMap_cl();

  // Get the width of this image map
  VTEX_EXPFUNC SLONG GetWidth() const;

  // Get the height of this image map
  VTEX_EXPFUNC SLONG GetHeight() const;

  // Get the height of this image map
  VTEX_EXPFUNC ColorDepth_e GetDepth() const;

  // Get the image data of this image map
  // NOTE: you can change the image data, but you can't alter the pointer
  VTEX_EXPFUNC UBYTE * GetData() const;

  // Assignment operator
  VTEX_EXPFUNC ImageMap_cl &operator=(const ImageMap_cl &map);

private:
  SLONG width;
  SLONG height;
  ColorDepth_e depth;
  UBYTE *data;

  friend class Image_cl;
};

class IVFileInStream;
class IVFileStreamManager;

class Image_cl
{
public:
  // A new empty Image_cl is created with undefined width and height. These will be defined when either the Load or
  // one of the Add... or Insert... functions is called for the first time.
  // The autoScale parameter determines whether every imagemap that is inserted (and width and height are already defined),
  // will automatically be scaled to the width and height of this object when this is necessary. if this parameter is
  // FALSE, the Add... and Insert... functions will return an error when the inserted imagemap has a different width or
  // height than those of this object.
  VTEX_EXPFUNC Image_cl(BOOL autoScale = TRUE);

  // As the first constructor, except that the width and height of this Image_cl object already have to be defined.
  VTEX_EXPFUNC Image_cl(SLONG width, SLONG height, BOOL autoScale = TRUE);
  
  // Copy constructor
  VTEX_EXPFUNC Image_cl(const Image_cl &img);

  // Destructor. Every imagemap will be removed from memory and this Image_cl object will be destroyed.
  VTEX_EXPFUNC virtual ~Image_cl();
  
  //Opens a file with the specified stream manager (or default vBase stream manager)
  VTEX_EXPFUNC static IVFileInStream* OpenFile(const char* pszFileName , IVFileStreamManager* pMan = NULL);

  //Loads an image from the specified stream
  // All formats, except DEM and TEX usually yield one colormap, with an additional opacitymap if an alpha-channel
  // is present. The DEM format yields one 32-bit heightmap. The TEX format can yield any possible configuration.
  // WARNING: every imagemap and image data in this object will be deleted before the new data is loaded.
  //Loads an image from the specified stream
  //New function
  VTEX_EXPFUNC RETVAL Load(IVFileInStream* pStream);

  //New helper function that opens and closes the file stream for you
  VTEX_EXPFUNC RETVAL Load(const char *pszFileName, IVFileStreamManager* pMan = NULL);

  //Old function for backwards compatibility. Will be removed
  VTEX_EXPFUNC RETVAL Load(const char *imgFilename, ImageFileType_e imgFileType/* = FILETYPE_ANY*/);


  // This function saves the image data in this object to the TEX file with the name in imgFilename.
  VTEX_EXPFUNC RETVAL SaveTEX(IVFileOutStream* pOut);

  // This function saves the first colormap frame in this object to the BMP file with the name in imgFilename.
  VTEX_EXPFUNC RETVAL SaveBMP(IVFileOutStream* pOut);

  // This function saves the first colormap frame in this object to the TGA file with the name in imgFilename.
  VTEX_EXPFUNC RETVAL SaveTGA(IVFileOutStream* pOut);

  // This function saves the first colormap frame in this object to uncompressed DDS format. Either 24 bit or 32 bit if alpha is specified
  VTEX_EXPFUNC RETVAL SaveUncompressedDDS(IVFileOutStream* pOut);

  // This function saves the first colormap frame in this object to the 8 bit paletted dds file with the name in imgFilename.
  VTEX_EXPFUNC RETVAL Save8BitPalettedDDS(IVFileOutStream* pOut, bool bNormalize);

  // This function saves the first colormap frame in this object to the 8 bit paletted tga file with the name in imgFilename.
  VTEX_EXPFUNC RETVAL Save8BitPalettedTGA(IVFileOutStream* pOut, bool bNormalize);

  // This function saves the first colormap frame in this object to the JPEG file with the name in imgFilename.
  // The jpegQuality parameter determines the quality of the saved JPEG image (scale 1-100, default 75).
  VTEX_EXPFUNC RETVAL SaveJPEG(IVFileOutStream* pOut, int jpegQuality = 75);

  // This function saves the first heightmap frame in this object to the DEM file with the name in imgFilename.
  // The DEM file that this function writes has the same layout as a Bryce DEM.
  VTEX_EXPFUNC RETVAL SaveDEM(const char *imgFilename);

  // This function saves the colormaps/heightmaps in this Image splitted up in multiple TEX files if
  // the width exceeds max_x and the height exceeds max_y. the separate image files are named as follows:
  // <imgFilename>_<x tile nr>_<y tile nr><extension>
  // WARNING: opacitymaps aren't supported by this function
  // EXAMPLE: ret = img->SaveSplittedTex("image", ".tex", 32, 64);
  VTEX_EXPFUNC RETVAL SaveSplittedTEX(const char *imgFilename, char *extention, int max_x, int max_y, IVFileStreamManager* pMan = NULL);

  // WARNING: This is an old function that might be removed in the future. Do not use it!
  // This function saves the image data in this object to the TEX file with the name in imgFilename.
  // With the rest of the parameters, the following TEX file features can be set:
  // genMipMaps:      if TRUE, Vision will generate mipmaps for this texture
  // sprite:          if TRUE, this image may be used as a sprite in Vision
  // texMorphing:     if TRUE, Vision interpolates between the subsequent animation frames
  // animStartStatus: if TRUE, the texture starts in animated status in Vision. if FALSE, the 1st frame is showed
  VTEX_EXPFUNC RETVAL SaveTEX(IVFileOutStream* pOut, BOOL genMipMaps, BOOL sprite, BOOL texMorphing, BOOL animStartStatus);


  /////////////////////////////////////////////////////////////////////////////////////////
  //Helpers
  /////////////////////////////////////////////////////////////////////////////////////////
  RETVAL SaveTEX(const char* pszFilename, IVFileStreamManager* pMan = NULL)
  {
    if (!pMan) pMan = VBase_GetFileStreamManager();
    IVFileOutStream* pOut = pMan->Create(pszFilename);
    int res = SaveTEX( pOut, genMipMaps, sprite, texMorphing, animStartStatus);
    if (pOut) pOut->Close();
    return res;
  }

  RETVAL SaveTEX(const char* pszFilename, BOOL genMipMaps, BOOL sprite, BOOL texMorphing, BOOL animStartStatus, IVFileStreamManager* pMan = NULL)
  {
    if (!pMan) pMan = VBase_GetFileStreamManager();
    IVFileOutStream* pOut = pMan->Create(pszFilename);
    int res = SaveTEX( pOut, genMipMaps, sprite, texMorphing, animStartStatus);
    if (pOut) pOut->Close();
    return res;
  }

  RETVAL SaveJPEG(const char* pszFilename, int jpegQuality = 75, IVFileStreamManager* pMan = NULL)
  {
    if (!pMan) pMan = VBase_GetFileStreamManager();
    IVFileOutStream* pOut = pMan->Create(pszFilename);
    int res = SaveJPEG( pOut, jpegQuality);
    if (pOut) pOut->Close();
    return res;
  }

  RETVAL SaveBMP(const char* pszFilename, IVFileStreamManager* pMan = NULL)
  {
    if (!pMan) pMan = VBase_GetFileStreamManager();
    IVFileOutStream* pOut = pMan->Create(pszFilename);
    int res = SaveBMP( pOut );
    if (pOut) pOut->Close();
    return res;

  }

  RETVAL SaveTGA(const char* pszFilename, IVFileStreamManager* pMan = NULL)
  {
    if (!pMan) pMan = VBase_GetFileStreamManager();
    IVFileOutStream* pOut = pMan->Create(pszFilename);
    int res = SaveTGA( pOut );
    if (pOut) pOut->Close();
    return res;

  }

  RETVAL SaveUncompressedDDS(const char* pszFilename, IVFileStreamManager* pMan = NULL)
  {
    if (!pMan) pMan = VBase_GetFileStreamManager();
    IVFileOutStream* pOut = pMan->Create(pszFilename);
    int res = SaveUncompressedDDS( pOut );
    if (pOut) pOut->Close();
    return res;    
  }

  RETVAL Save8BitPalettedDDS(const char* pszFilename, bool bNormalize, IVFileStreamManager* pMan = NULL)
  {
    if (!pMan) pMan = VBase_GetFileStreamManager();
    IVFileOutStream* pOut = pMan->Create(pszFilename);
    int res = Save8BitPalettedDDS( pOut, bNormalize );
    if (pOut) pOut->Close();
    return res;

  }

  RETVAL Save8BitPalettedTGA(const char* pszFilename, bool bNormalize, IVFileStreamManager* pMan = NULL)
  {
    if (!pMan) pMan = VBase_GetFileStreamManager();
    IVFileOutStream* pOut = pMan->Create(pszFilename);
    int res = Save8BitPalettedTGA( pOut, bNormalize );
    if (pOut) pOut->Close();
    return res;

  }

  // saves the image to known format according to file extension (bmp, tga, dds, ...)
  RETVAL Save(const char* pszFilename, IVFileStreamManager* pMan = NULL)
  {
    char szExt[FS_MAX_FILE];
    if (!VFileHelper::GetExtension(szExt,pszFilename))
      return VTEX_ERR_FILETYPENOTSUPPORTED;

    if (!_stricmp(szExt,"bmp"))  return SaveBMP(pszFilename,pMan);
    if (!_stricmp(szExt,"tga"))  return SaveTGA(pszFilename,pMan);
    if (!_stricmp(szExt,"dds"))  return SaveUncompressedDDS(pszFilename,pMan);
    if (!_stricmp(szExt,"jpg"))  return SaveJPEG(pszFilename,75,pMan);
    if (!_stricmp(szExt,"jpeg")) return SaveJPEG(pszFilename,75,pMan);
    if (!_stricmp(szExt,"tex"))  return SaveTEX(pszFilename,pMan);
    return VTEX_ERR_FILETYPENOTSUPPORTED;
  }

  /////////////////////////////////////////////////////////////////////////////////////////

  // The Add... functions will add a specific imagemap as a new frame to this Image.
  // WARNING: The AddColorMap functions need 24-bit image data, and AddHeightMap either needs 32-bit or 16-bit data
  // (the latter only when the ScaleHeightMapsTo16BPP has been called before). The other functions take any image data.
  // It is illegal to add both colormaps and heightmaps to an Image. Such an Image object can't be saved.
  // EXAMPLE: ret = img->AddColorMap(32, 64, COLORDEPTH_24BPP, data);
  VTEX_EXPFUNC RETVAL AddColorMap(ImageMap_cl &colorMap);
  VTEX_EXPFUNC RETVAL AddColorMap(SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);
  VTEX_EXPFUNC RETVAL AddOpacityMap(ImageMap_cl &opacityMap);
  VTEX_EXPFUNC RETVAL AddOpacityMap(SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);
  VTEX_EXPFUNC RETVAL AddHeightMap(ImageMap_cl &heightMap);
  VTEX_EXPFUNC RETVAL AddHeightMap(SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);

  //Invert a channel 0=R, 1=G, 2=B, 3=A
  VTEX_EXPFUNC bool InvertChannel(int iChannel); // used in the TexView project

  // The Insert... functions will insert a specific imagemap at a frame position with the number frameNr to this Image.
  // WARNING: The InsertColorMap functions need 24-bit image data, and InsertHeightMap either needs 32-bit or 16-bit data
  // (the latter only when the ScaleHeightMapsTo16BPP has been called before). The other functions take any image data.
  // It is illegal to add both colormaps and heightmaps to an Image. Such an Image object can't be saved.
  // EXAMPLE: ret = img->InsertColorMap(2, 32, 64, COLORDEPTH_24BPP, data);
  VTEX_EXPFUNC RETVAL InsertColorMap(SLONG frameNr, ImageMap_cl colorMap);  ///< TODO: map taken by value ?
  VTEX_EXPFUNC RETVAL InsertColorMap(SLONG frameNr, SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);
  VTEX_EXPFUNC RETVAL InsertOpacityMap(SLONG frameNr, ImageMap_cl opacityMap);  ///< TODO: map taken by value ?
  VTEX_EXPFUNC RETVAL InsertOpacityMap(SLONG frameNr, SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);
  VTEX_EXPFUNC RETVAL InsertHeightMap(SLONG frameNr, ImageMap_cl heightMap);  ///< TODO: map taken by value ?
  VTEX_EXPFUNC RETVAL InsertHeightMap(SLONG frameNr, SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);

  // The Replace... functions will replace a specific imagemap at a frame position with the number frameNr in this Image.
  // WARNING: The ReplaceColorMap functions need 24-bit image data, and ReplaceHeightMap either needs 32-bit or 16-bit data
  // (the latter only when the ScaleHeightMapsTo16BPP has been called before). The other functions take any image data.
  // It is illegal to add both colormaps and heightmaps to an Image. Such an Image object can't be saved.
  // EXAMPLE: ret = img->ReplaceColorMap(2, 32, 64, COLORDEPTH_24BPP, data);
  VTEX_EXPFUNC RETVAL ReplaceColorMap(SLONG frameNr, ImageMap_cl colorMap);  ///< TODO: map taken by value ?
  VTEX_EXPFUNC RETVAL ReplaceColorMap(SLONG frameNr, SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);
  VTEX_EXPFUNC RETVAL ReplaceOpacityMap(SLONG frameNr, ImageMap_cl opacityMap);  ///< TODO: map taken by value ?
  VTEX_EXPFUNC RETVAL ReplaceOpacityMap(SLONG frameNr, SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);
  VTEX_EXPFUNC RETVAL ReplaceHeightMap(SLONG frameNr, ImageMap_cl heightMap);  ///< TODO: map taken by value ?
  VTEX_EXPFUNC RETVAL ReplaceHeightMap(SLONG frameNr, SLONG width, SLONG height, ColorDepth_e depth, UBYTE *imgdata);

  // The Get...Map functions return a certain imagemap at frame position frameNr if an imagemap exists at that position.
  // WARNING: these functions have no error-checking
  VTEX_EXPFUNC ImageMap_cl GetColorMap(SLONG frameNr); ///< removed const, because auto-decoding of rawdata sure is not a const operation
  VTEX_EXPFUNC ImageMap_cl GetOpacityMap(SLONG frameNr);
  VTEX_EXPFUNC ImageMap_cl GetHeightMap(SLONG frameNr) const;

  // The Remove... functions remove a certain imagemap at frame position frameNr if an imagemap exists at that position.
  // If not, an error code is returned.
  // Calling these functions with -1 as frameNr has the same effect as the Delete... functions.
  VTEX_EXPFUNC RETVAL RemoveColorMap(SLONG frameNr);
  VTEX_EXPFUNC RETVAL RemoveOpacityMap(SLONG frameNr);
  VTEX_EXPFUNC RETVAL RemoveHeightMap(SLONG frameNr);

  // The Delete... functions delete all imagemaps of a certain time (opacity, bump, etc).
  VTEX_EXPFUNC void DeleteColorMaps();
  VTEX_EXPFUNC void DeleteOpacityMaps();
  VTEX_EXPFUNC void DeleteHeightMaps();

  // This function deletes all imagemaps in this object and sets width and height to uninitialized
  VTEX_EXPFUNC void ClearImage();

  // The Has... functions return TRUE if one or more imagemaps of a certain type are present in this Image.
  // If not, FALSE is returned.
  VTEX_EXPFUNC BOOL HasColorMap() const;
  VTEX_EXPFUNC BOOL HasOpacityMap() const;
  VTEX_EXPFUNC BOOL HasHeightMap() const;
  VTEX_EXPFUNC BOOL HasRawMap() const;

  // The Nr... functions return the number of imagemaps of a certain type that are present in this Image.
  VTEX_EXPFUNC SLONG NrColorMaps() const;
  VTEX_EXPFUNC SLONG NrOpacityMaps() const;
  VTEX_EXPFUNC SLONG NrHeightMaps() const;
  VTEX_EXPFUNC SLONG NrRawMaps() const;

  // GetMaxFrameCount returns the maximum amount of animated frames can be made out of the different imagemaps present
  // in this Image. For example, when there are three colormaps, four opacitymaps, one bumpmap and two glowmaps, and
  // the colormap and opacitymap are animated, GetMaxFrameCount is three.
  VTEX_EXPFUNC SLONG GetMaxFrameCount();

  // With these functions one can get or set the animated status of the different types of imagemaps. TRUE means that an
  // imagemap type is animated, FALSE means that it's not animated, in which case only the first imagemap of that type
  // will be used in the Image when it is exported as a file. Default is FALSE.
  VTEX_EXPFUNC BOOL &ColorMapAnimated();
  VTEX_EXPFUNC BOOL &OpacityMapAnimated();
  VTEX_EXPFUNC BOOL &HeightMapAnimated();

  // These functions returns the current dimensions of the Image. If the dimensions of the Image aren't defined yet,
  // these functions return -1
  VTEX_EXPFUNC SLONG GetWidth() const;
  VTEX_EXPFUNC SLONG GetHeight() const;

  // With these functions one can set the width or height of the Image. All imagemaps present in the Image will be scaled
  // to the new width or height
  VTEX_EXPFUNC RETVAL SetWidth(SLONG width);
  VTEX_EXPFUNC RETVAL SetHeight(SLONG height);

  // As above, except that the width and height can be set at once
  VTEX_EXPFUNC RETVAL Scale(SLONG width, SLONG height);

  // With this function one can scale the image dimensions to the nearest powers of 2. When scaling is:
  // SCALE_UP: Image will be scaled up to the next powers of 2
  // SCALE_DOWN: Image will be scaled down to the previous powers of 2
  // SCALE_NEAREST: Image will be scaled to the nearest powers of 2
  VTEX_EXPFUNC RETVAL ScalePower2(ScalePower2_e scaling);

  // This function converts the color map to a normal map
  // storing the height value in the alpha channel is very useful for parallax mapping
  // Notes: Does not support animated textures or heightmaps
  VTEX_EXPFUNC bool CreateNormalMap(float fBumpiness, BOOL bStoreHeightInAlpha=FALSE);

  // This function converts the color map to a alpha map,
  // alpha values are calculated from RGB values average
  // Notes: Does not support animated textures or heightmaps
  VTEX_EXPFUNC bool CreateAlphaMap(Image_cl &AlphaSourceImage);

  // This function returns the current heightmap bit depth (standard is COLORDEPTH_16BPP)
  VTEX_EXPFUNC ColorDepth_e GetHeightMapDepth();

  // This function resamples all heightmaps in this Image to targetDepth, and further heightmaps that are added
  // to this Image will be automatically sampled to this bit depth as well
  // WARNING: The standard heightmap depth is set to 16 BPP, so if you want to add 32 BPP heightmaps and keep
  // their bit depth, the first thing you should do is calling for example img->SetHeightMapDepth(COLORDEPTH_32BPP);
  VTEX_EXPFUNC RETVAL SetHeightMapDepth(ColorDepth_e targetDepth);

  // This function converts all heightmaps in this Image to 16-bit, and further heightmaps that are added to this
  // Image are automatically converted as well.
  // WARNING: Old function. Use SetHeightMapDepth instead.
  VTEX_EXPFUNC RETVAL ScaleHeightMapsTo16BPP();

  // Gets/Sets the time between animation frames. Default is 0.
  VTEX_EXPFUNC SLONG &AnimTiming();

  // Gets/Sets the current animation type of the Image. Default is ANIMTYPE_NOANIM
  VTEX_EXPFUNC AnimType_e &AnimType();

  // Gets/Sets the texture morphing status.
  // If TRUE, Vision interpolates between the subsequent animation frames. Default is FALSE.
  VTEX_EXPFUNC BOOL &TexMorphing();

  // Gets/Sets the starting state of an animated Image.
  // If TRUE, the texture starts in animated status in Vision. if FALSE, the 1st frame is showed. Default is FALSE.
  VTEX_EXPFUNC BOOL &AnimStartStatus();

  // Gets/Sets the variable which determines whether mipmaps should be generated for this Image.
  // If TRUE, Vision will generate mipmaps for this texture. Default is FALSE.
  VTEX_EXPFUNC BOOL &GenMipMaps();

  // Gets/Sets the variable which determines whether this Image is going to be used as a sprite.
  // if TRUE, this image may be used as a sprite in Vision. Default is FALSE.
  VTEX_EXPFUNC BOOL &Sprite();

  // Gets the time between animation frames
  // WARNING: Old function. Do not use!
  VTEX_EXPFUNC SLONG GetAnimTiming() const;

  // Gets the current animation type of the Image
  // WARNING: Old function. Do not use!
  VTEX_EXPFUNC AnimType_e GetAnimType() const;

  // Sets the time between animation frames
  // WARNING: Old function. Do not use!
  VTEX_EXPFUNC void SetAnimTiming(SLONG animTiming);

  // Sets the animation type of the Image
  // WARNING: Old function. Do not use!
  VTEX_EXPFUNC void SetAnimType(AnimType_e animType);

  // Assignment operator
  VTEX_EXPFUNC Image_cl &operator=(const Image_cl &img);

  // Clone function (creates a new Image_cl object with this Image's data and returns it)
  VTEX_EXPFUNC virtual Image_cl clone();


  // This function loads the header of a file with the name in imgFilename and which is of image type imgFileType.
  // If the latter parameter is FILETYPE_ANY it will attempt to autodetect the image file type.
  // After loading the header, this function will store the dimensions of the image file in imgWidth, imgHeight
  // and imgDepth (imgDepth can take the values 1, 2, 4, 8, 16, 24 and 32)
  // WARNING: this functions is static and should be called as follows: Image_cl::LoadDimensions(...);
  //NEW function
  VTEX_EXPFUNC static RETVAL LoadDimensions(SLONG *imgWidth, SLONG *imgHeight, SLONG *imgDepth, IVFileInStream* pStream);

  //New helper function that opens and closes the file stream for you
  VTEX_EXPFUNC static RETVAL LoadDimensions(SLONG *imgWidth, SLONG *imgHeight, SLONG *imgDepth, const char* pszFileName, IVFileStreamManager* pMan = NULL);

  //Old function for backwards compatibility
  VTEX_EXPFUNC static RETVAL LoadDimensions(SLONG *imgWidth, SLONG *imgHeight, SLONG *imgDepth, const char* pszFileName ,
    ImageFileType_e imgFileType);


  // Functions for working with powers of 2
  VTEX_EXPFUNC static BOOL IsPowerOf2(SLONG number);
  VTEX_EXPFUNC static SLONG GetNearestPowerOf2(SLONG number);

  // this function combines a colormap (cmap) and opacitymap (omap) to 32-bit RGBA data (rgbamap).
  // imgsize should contain width * height of the cmap and omap. this function automatically allocates memory
  // for the rgbamap
  VTEX_EXPFUNC static RETVAL CombineColorAndOpacityMap(UBYTE **rgbamap, UBYTE *cmap, UBYTE *omap, SLONG imgsize);

  // if the image has raw maps, all rawmaps get converted to color and opacity maps
  // in the process some information might get lost including: -mipmaps -palettes -compression -cubemap ordering
  // existing colormaps/opacitymaps etc are deleted. 
  // if bFreeRawMapMemory is TRUE, the raw maps get deallocated
  // note that this function will be called automatically once somebody tries to access color maps etc on an
  // image that has only rawdata. this is to ensure compatibility with previous versions.
  VTEX_EXPFUNC RETVAL UnpackRawMaps ( BOOL bFreeRawMapMemory );

  // returns true if the image_cl only contains raw map data. 
  // note: if the rawmaps are present but also decoded to image maps, this function will return false
  VTEX_EXPFUNC BOOL IsInRawLoadState () const;

  // return number of mipmaps per raw map. 
  // note: all raw maps in an image_cl must have same number of miplevels.
  // note^2: dds for example does not require that ALL miplevels down to 1x1 are included in the file.
  VTEX_EXPFUNC SLONG GetRawNumMipMaps () const;

  // all rawmaps in an Image_cl have the same format
  VTEX_EXPFUNC RawExternalFormal_e GetRawMapsFormat () const;

  // returns the inherent file specified purpose of the raw maps.
  // this is only a recommendation and can also be ignored. 
  // distinguish between: texture, normalmap, cubemap, volumetexture ... 
  VTEX_EXPFUNC ImageRawMapType_e GetRawMapsType () const;

  // return a hint on which uploading mode to use. this is only a hint and mostly useful for opengl
  VTEX_EXPFUNC ImageRawMapHint_e GetRawMapsUploadHint () const;

  // return a data pointer to rawmap data. the memory is owned by vtex, there is no copy taking place.
  // DO NOT TRY TO FREE/DELETE THIS POINTER. READ ONLY MEMORY.
  // data returned by this call should directly be passed into texture upload functions. format is according to
  // GetRawMapsFormat(). Note: the returned width and height are what should be used for upload, and is not neccesarily
  // the width and height of the data, same goes for size. Example: a dxt1 compressed one but last miplevel would return
  // width and height of 2, but the size of a 4x4 block (8byte) - the same a TexImage2D call expects. 
  VTEX_EXPFUNC const void * GetRawMapData ( int iRawMapNr, int iMipLevel, int &iWidth, int &iHeight, int &iSize ) const;

  // Helper function for determining an image file's type
  static VTEX_EXPFUNC ImageFileType_e DetermineFileType(const char *filename);

private:
  // scaling functions
  RETVAL ScaleMap(UBYTE **map, SLONG width, SLONG height, ColorDepth_e depth, SLONG targetwidth, SLONG targetheight);
  void ScaleX(UBYTE *dest, UBYTE *src, SLONG destxsize, SLONG srcxsize, SLONG ysize, SLONG pixellen);
  void ScaleY(UBYTE *dest, UBYTE *src, SLONG destysize, SLONG srcysize, SLONG xsize, SLONG pixellen);

  // special fprintf function for printing reals in a DEM file
  static void DEMfprintf(FILE *stream, double d);


  // general purpose rounding functions
  static SLONG Round(double number);

  // function for resampling a heightmap to another bit depth
  static RETVAL ResampleHeightMap(ColorDepth_e targetDepth, UBYTE **map, SLONG width, SLONG height, ColorDepth_e depth);

  // function for downsampling 16-bit and 24-bit image data to 8-bit
  // (by taking the mean value of the RGB components. this is used for making opacity-, bump- and glowmaps from colormaps)
  static RETVAL ImageDownSampling8BPP(UBYTE **map, SLONG width, SLONG height, ColorDepth_e depth);

  // frees all rawmap pointers
  void FreeRawMapData ();


private:
  friend class DDSLoader_cl;          ///< raw loaders must be friends

  // the width and height of this Image object. a value of -1 means uninitialized.
  SLONG width;
  SLONG height;

  // this variable determines whether every new imagemap should be automatically scaled
  //  to the width and height of this Image object.
  BOOL autoScale;

  // the linked list objects which hold the imagemaps.
  LinkedList_cl<UBYTE *> colorMaps;   ///< color data in the format BGRBGRBGRBGR...
  LinkedList_cl<UBYTE *> opacityMaps; ///< opacity data in the format AAAA...
  LinkedList_cl<UBYTE *> heightMaps;
  UBYTE *rawMapData;                  ///< undecoded map data-use this pointer for deletion only
  UBYTE **rawSubMaps;                 ///< undecoded maps, pointers to submaps (ramapNumMipLevel*numrawmaps)
  int *rawSubMapSizes;                ///< and sizes of the same

  // rawmap state
  // all the following vars are set by loaders that only load rawmaps
  BOOL isInRawLoadState;              ///< only raw maps loaded - other values are only dummies
  ImageRawMapType_e rawmapType;       ///< cubemaps, volume maps, texture maps... 
  ImageRawMapHint_e rawmapHint;
  RawExternalFormal_e rawmapFormat;   ///< direct opengl format (translation table available)
  int numRawMaps;                     ///< number fo rawmaps
  SLONG rawmapNumMipLevels;           ///< number of miplevels
  ImageRawDecoder_e rawDecoder;       ///< raw decoder to use
  int numRawOpacityMaps;              ///< number of opacity maps after decoding the raw data
  int numRawColorMaps;                ///< number of color maps after decoding the raw data

  ColorDepth_e heightMapDepth;        ///< holds the current depth of the height maps

  // these variables determine whether the corresponding imagemaps are animated
  BOOL colorMapAnimated;
  BOOL opacityMapAnimated;
  BOOL heightMapAnimated;

  SLONG animTiming;
  AnimType_e animType;
  BOOL texMorphing;
  BOOL animStartStatus;
  BOOL genMipMaps;
  BOOL sprite;
};

#endif // _VISION_DOC


/// \brief
///   Structure that holds information about a single texture animation frame resource
struct VTexAnimFrame_t
{
  const char *m_szFilename; ///< filename of the texture
  float m_fDelay; ///< Delay in seconds
};

struct TextureAnimInfo_t;
class IVTextureLoader;

/// \brief
///   Class that represents a single texanim file
class VTexAnimInfo
{
public:
  /// \brief
  ///   Constructor
  VTEX_EXPFUNC VTexAnimInfo();
  /// \brief
  ///   Destructor
  VTEX_EXPFUNC ~VTexAnimInfo();

  /// \brief
  ///   Loads the texanim from text-based file
  VTEX_EXPFUNC RETVAL LoadFromFile(const char* pszFileName , IVFileStreamManager* pMan = NULL);
  /// \brief
  ///   Loads the texanim from text-based file
  VTEX_EXPFUNC RETVAL LoadFromFile(IVFileInStream* pStream);

  /// \brief
  ///   Returns the number of texture frames
  VTEX_EXPFUNC inline int GetFrameCount() const {return m_iNumFrames;}

  /// \brief
  ///   Fills the passed structure with information about the frame with specified index
  VTEX_EXPFUNC void GetFrame(VTexAnimFrame_t &frameInfo, int iIndex) const;

  /// \brief
  ///   Helper function to physically load all texture resource using the passed texture loader. The frames are stored in the passed TextureAnimInfo_t structure
  VTEX_EXPFUNC void LoadAllFrames(TextureAnimInfo_t &destInfo, IVTextureLoader *pLoader, const char *szPath, int iTextureFlags);
private:

  /// \brief
  ///   Release all frames
  void FreeAll();
  int m_iNumFrames; ///< Number of frames
  DynArray_cl<char *>m_szFilename; ///< Array that holds the filenames for each frame
  DynArray_cl<float>m_fDelay;      ///< Array that holds the delay for each frame
};

#ifndef _VISION_DOC

namespace VTex
{
  //Some helper functions
  
  //Copy a line of color values and swap Red and Blue
  inline void CopyLineSwapRB(UBYTE *pDest, const UBYTE *pSrc, int width , const int iStride = 3)
  {
    for (int i=width; i>0; i--)
    {
      pDest[0] = pSrc[2];  ///<B
      pDest[1] = pSrc[1];  ///<G
      pDest[2] = pSrc[0];  ///<R
      pDest+=iStride; pSrc+=iStride;
    }
  }  
}

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
