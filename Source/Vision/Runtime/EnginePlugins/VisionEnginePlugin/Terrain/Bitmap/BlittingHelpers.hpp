/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file BlittingHelpers.hpp

#ifndef BLITTINGHELPERS_HPP_INCLUDED
#define BLITTINGHELPERS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainIncludes.hpp>

///\brief
///Describes the data alignment of a system memory bitmap. Used for blitting 
///
struct BitmapInfo_t
{
  ///\brief
	///Enum for blitting component formats
	enum VComponentType
  {
    Unknown = -1,       ///< not used
    ByteLuminance = 0,  ///< 8-bit values (0..255)
    FloatLuminance,     ///< single float values : can be used for height fields
    ByteRGBA,           ///< VColorRef / VisBitmap_cl
    FloatRGBA           ///< hkvVec4: 4-component float values
  };

  ///\brief
	///Constructor
	///
	///\param components
	///Component format
	///
	///\param iSizeX
	///Size of the rectangle in components
	///
	///\param iSizeY
	///Size of the rectangle in components
	///
	///\param pData
	///Raw data pointer
	///
	///\param iStride
	///Row stride in components
	BitmapInfo_t(VComponentType components, int iSizeX, int iSizeY, void *pData, int iStride=-1)
  {
    m_iSize[0] = iSizeX;
    m_iSize[1] = iSizeY;
    m_eComponentType = components;
    m_iStride = iStride>0 ? iStride:iSizeX;
    m_pData=pData;
  }

  ///\brief
  ///Access color value as float4 at specified pixel position
  ///
  ///\param iX
  ///Pixel position
  ///
  ///\param iY
  ///Pixel position
  ///
  ///\returns
  ///float based color vector
  ///
  TERRAIN_IMPEXP hkvVec4 GetColorAt(int iX, int iY) const;

  ///\brief
  ///Set color values at specified pixel position (will be mapped to target component format)
  ///
  ///\param iX
  ///Pixel position
  ///
  ///\param iY
  ///Pixel position
  ///
  ///\param vColor
  ///float based color vector
  TERRAIN_IMPEXP void SetColorAt(int iX, int iY, const hkvVec4& vColor);

  ///\brief
  ///Helper function to evaluate a new rectangle that is clipped against the extents
  ///
  ///\param pRect
  ///Source and dest rectangle
  ///
  ///\param iOfsX
  ///New offset in rectangle
  ///
  ///\param iOfsY
  ///New offset in rectangle
  ///
  ///\returns
  ///false if the rectangle is completely outside
  TERRAIN_IMPEXP bool ScissorRect(int *pRect, int &iOfsX, int &iOfsY) const;

  int m_iSize[2]; ///< size of the bitmap
  VComponentType m_eComponentType;  ///<component format
  int m_iStride;  ///< stride (in multiples of components)
  void *m_pData;  ///< pointer to raw data
};

#if defined _VISION_DOC

// PixelCombineCallback : callback type of a pixel blend function
typedef hkvVec4 (*PixelCombineCallback) (const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);
#else
// this is necessary since doxygen does not like the __fastcall in the function pointer declaration
typedef hkvVec4 (VISION_FASTCALL * PixelCombineCallback) (const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);
#endif

///\brief
///Provides helper functions for software blitting in system memory bitmaps
namespace BlittingHelpers
{

  ///\brief
	///Enum that is used for pixel read- and write masks
	enum VColorComponentBits
  {
    None  = 0, ///< None
    Red   = 1, ///< Red bit
    Green = 2, ///< Green bit
    Blue  = 4, ///< Blue bit
    Alpha = 8, ///< Alpha bit
    RGB   = Red|Green|Blue, ///< Combination of RGB bits
    RGBA  = RGB|Alpha, ///< Combination of RGBA bits
  };

  
  ///
  /// @name Blitting
  /// @{
  ///

  ///\brief
	///Performs an actual blitting from source bitmap into passed target structure
	///
	///\param pSrcBitmap
	///Source image to blit
	///
	///\param targetInfo
	///Target memory info
	///
	///\param iX
	///Upper left corner coordinate
	///
	///\param iY
	///Upper left corner coordinate
	///
	///\param auxColor
	///Auxiliary color, usage depends on the pixel callback 
	///
  ///\param combine
  ///Callback function pointer that defines the "blendmode". There are predefined functions such as CombineSrcAlpha.
	///
  ///\param eWriteMask
	///Target write mask
	TERRAIN_IMPEXP void BlitBitmap(const VisBitmap_cl *pSrcBitmap, BitmapInfo_t &targetInfo, int iX, int iY, const hkvVec4& auxColor, PixelCombineCallback combine=NULL, VColorComponentBits eWriteMask=RGBA);

  ///\brief
  ///Performs an actual blitting from source bitmap into passed target structure. Uses additional stetching.
  ///
  ///\param pSrcBitmap
  ///Source image to blit
  ///
  ///\param targetInfo
	///Target memory info
  ///
  ///\param iX
	///Upper left corner coordinate
  ///
  ///\param iY
	///Upper left corner coordinate
  ///
  ///\param iSizeX
  ///Target size
  ///
  ///\param iSizeY
  ///Target size
  ///
  ///\param auxColor
	///Auxiliary color, usage depends on the pixel callback 
	///
  ///\param combine
  ///Callback function pointer that defines the "blendmode". There are predefined functions such as CombineSrcAlpha.
	///
  ///\param eWriteMask
	///Target write mask
  ///
  TERRAIN_IMPEXP void StretchBlitBitmap(const VisBitmap_cl *pSrcBitmap, BitmapInfo_t &targetInfo, int iX, int iY, int iSizeX, int iSizeY, const hkvVec4& auxColor, PixelCombineCallback combine=NULL, VColorComponentBits eWriteMask=RGBA);

  ///\brief
  ///Performs an actual blitting from source bitmap into passed target structure. Uses additional stetching.
  ///
  ///\param pSrcBitmap
  ///Source image to blit
  ///
  ///\param pSrcVertexCoords
  ///4 source texture coordinates that allow for rotated blitting
  ///
  ///\param targetInfo
	///Set of 4 source UV coordinates in the source image
  ///
  ///\param targetInfo
	///Target memory info
  ///
  ///\param iX
	///Upper left corner coordinate
  ///
  ///\param iY
	///Upper left corner coordinate
  ///
  ///\param iSizeX
  ///Target size
  ///
  ///\param iSizeY
  ///Target size
  ///
  ///\param auxColor
	///Auxiliary color, usage depends on the pixel callback 
	///
  ///\param combine
  ///Callback function pointer that defines the "blendmode". There are predefined functions such as CombineSrcAlpha.
	///
  ///\param eWriteMask
	///Target write mask
  ///
  TERRAIN_IMPEXP void StretchBlitBitmap(const VisBitmap_cl *pSrcBitmap, const hkvVec2 pSrcVertexCoords[4], BitmapInfo_t &targetInfo, int iX, int iY, int iSizeX, int iSizeY, const hkvVec4& auxColor, PixelCombineCallback combine=NULL, VColorComponentBits eWriteMask=RGBA);

  ///
  /// @}
  ///

  ///
  /// @name Math helpers
  /// @{
  ///

  ///\brief
	///Helper function to clamp a float against a min max range
	///
	///\param fIn
	///Input float
	///
	///\param fMin
	///Lower border
	///
	///\param fMax
	///Upper border
	///
	///\returns
	///clamped value
	///
	inline float ClampFloat(float fIn, float fMin, float fMax) {return (fIn<fMin) ? fMin:((fIn>fMax) ? fMax:fIn);}

  ///\brief
	///Helper function to clamp an integer value against a min max range
	///
	///\param iIn
	///Input value
	///
	///\param iMin
	///Lower border
	///
	///\param iMax
	///Upper border
	///
	///\returns
	///clamped value
	inline int ClampInt(int iIn, int iMin, int iMax) {return (iIn<iMin) ? iMin:((iIn>iMax) ? iMax:iIn);}

  ///\brief
	///Helper function to bilinearily interpolate float values
	///
	///\param pSrc
	///Source value data
	///
	///\param iSizeX
	///Source size
	///
	///\param iSizeY
	///Source size
	///
	///\param iSrcStride
  ///Row stride in source (in multiples of float)
	///
	///\param x
	///Coordinate to sample
	///
	///\param y
	///Coordinate to sample
	///
	///\returns
  ///Interpolated value at (x,y)
	inline float BiLerpF(const float *pSrc, int iSizeX, int iSizeY, int iSrcStride, float x, float y);

  ///\brief
	///Helper function to bilinearily interpolate unsigned byte values
	///
	///\param pSrc
	///Source value data
	///
	///\param iSizeX
	///Source size
	///
	///\param iSizeY
	///Source size
	///
	///\param iSrcStride
  ///Row stride in source (in bytes)
	///
	///\param x
	///Coordinate to sample
	///
	///\param y
	///Coordinate to sample
	///
	///\returns
  ///Interpolated value at (x,y). It is a float in range [0..255]
  inline float BiLerpB(const UBYTE *pSrc, int iSizeX, int iSizeY, int iSrcStride, float x, float y);
  
  ///
  /// @}
  ///

  ///
  /// @name Predefined blend functions
  /// @{
  ///

  ///\brief
  ///Replace color (no blending)
  ///
  ///Pre-defined blend function that can be passed to BlitBitmap and its variants
  ///
  ///\param srcColor
  ///Source color
  ///
  ///\param dstColor
  ///Target color
  ///
  ///\param auxColor
  ///Auxiliary color
  ///
  ///\returns
  ///Blended result
  ///
  TERRAIN_IMPEXP hkvVec4 VISION_FASTCALL CombineReplace(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);

  ///\brief
  ///boolean mask (either auxColor or dstColor)
  ///
  ///Pre-defined blend function that can be passed to BlitBitmap and its variants
  ///
  ///\param srcColor
  ///Source color
  ///
  ///\param dstColor
  ///Target color
  ///
  ///\param auxColor
  ///Auxiliary color
  ///
  ///\returns
  ///Blended result
  ///
  TERRAIN_IMPEXP hkvVec4 VISION_FASTCALL BooleanMask(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);

  ///\brief
  ///Additive blending (dstColor+srcColor*auxColor)
  ///
  ///Pre-defined blend function that can be passed to BlitBitmap and its variants
  ///
  ///\param srcColor
  ///Source color
  ///
  ///\param dstColor
  ///Target color
  ///
  ///\param auxColor
  ///Auxiliary color
  ///
  ///\returns
  ///Blended result
  ///
  TERRAIN_IMPEXP hkvVec4 VISION_FASTCALL CombineAdditive(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);

  ///\brief
  ///Alpha blending (blending with srcColor.a*auxColor.a)
  ///
  ///Pre-defined blend function that can be passed to BlitBitmap and its variants
  ///
  ///\param srcColor
  ///Source color
  ///
  ///\param dstColor
  ///Target color
  ///
  ///\param auxColor
  ///Auxiliary color
  ///
  ///\returns
  ///Blended result
  ///
  TERRAIN_IMPEXP hkvVec4 VISION_FASTCALL CombineSrcAlpha(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);

  ///\brief
  ///Multiplicative blending (dstColor*srcColor*auxColor)
  ///
  ///Pre-defined blend function that can be passed to BlitBitmap and its variants
  ///
  ///\param srcColor
  ///Source color
  ///
  ///\param dstColor
  ///Target color
  ///
  ///\param auxColor
  ///Auxiliary color
  ///
  ///\returns
  ///Blended result
  ///
  TERRAIN_IMPEXP hkvVec4 VISION_FASTCALL CombineModulate(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);

  ///\brief
  ///Alpha blending using the luminance of source color
  ///
  ///Pre-defined blend function that can be passed to BlitBitmap and its variants
  ///
  ///\param srcColor
  ///Source color
  ///
  ///\param dstColor
  ///Target color
  ///
  ///\param auxColor
  ///Auxiliary color
  ///
  ///\returns
  ///Blended result
  ///
  TERRAIN_IMPEXP hkvVec4 VISION_FASTCALL CombineSrcLuminance(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);

  ///\brief
  ///Additive blending using the luminance of source color
  ///
  ///Pre-defined blend function that can be passed to BlitBitmap and its variants
  ///
  ///\param srcColor
  ///Source color
  ///
  ///\param dstColor
  ///Target color
  ///
  ///\param auxColor
  ///Auxiliary color
  ///
  ///\returns
  ///Blended result
  ///
  TERRAIN_IMPEXP hkvVec4 VISION_FASTCALL CombineAddSrcLuminance(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor);

  ///
  /// @}
  ///
}


inline float BlittingHelpers::BiLerpF(const float *pSrc, int iSizeX, int iSizeY, int iSrcStride, float x, float y)
{
  int x1 = (int)x;
  int y1 = (int)y;
  float fx = x-(float)x1;
  float fy = y-(float)y1;
  if (x1<0) x1=0; else if (x1>=iSizeX) x1=iSizeX-1;
  if (y1<0) y1=0; else if (y1>=iSizeY) y1=iSizeY-1;
  int x2 = hkvMath::Min(x1+1,iSizeX-1);
  int y2 = hkvMath::Min(y1+1,iSizeY-1);

  float row1 = pSrc[y1*iSrcStride+x1] + (pSrc[y1*iSrcStride+x2]-pSrc[y1*iSrcStride+x1])*fx;
  float row2 = pSrc[y2*iSrcStride+x1] + (pSrc[y2*iSrcStride+x2]-pSrc[y2*iSrcStride+x1])*fx;

  return row1 + (row2-row1)*fy;
}


inline float BlittingHelpers::BiLerpB(const UBYTE *pSrc, int iSizeX, int iSizeY, int iSrcStride, float x, float y)
{
  int x1 = (int)x;
  int y1 = (int)y;
  float fx = x-(float)x1;
  float fy = y-(float)y1;
  if (x1<0) x1=0; else if (x1>=iSizeX) x1=iSizeX-1;
  if (y1<0) y1=0; else if (y1>=iSizeY) y1=iSizeY-1;
  int x2 = hkvMath::Min(x1+1,iSizeX-1);
  int y2 = hkvMath::Min(y1+1,iSizeY-1);

  float row1 = (float)pSrc[y1*iSrcStride+x1] + (float)(pSrc[y1*iSrcStride+x2]-pSrc[y1*iSrcStride+x1])*fx;
  float row2 = (float)pSrc[y2*iSrcStride+x1] + (float)(pSrc[y2*iSrcStride+x2]-pSrc[y2*iSrcStride+x1])*fx;

  return row1 + (row2-row1)*fy;
}

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
