/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Bitmap/BlittingHelpers.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>



bool BitmapInfo_t::ScissorRect(int *pRect, int &iOfsX, int &iOfsY) const
{
  // completely outside?
  if (pRect[0]>=m_iSize[0] || pRect[1]>=m_iSize[1]) 
    return false;
  if (pRect[2]<0 || pRect[3]<0) 
    return false;

  // clamp
  if (pRect[2]>=m_iSize[0]) pRect[2]=m_iSize[0]-1;
  if (pRect[3]>=m_iSize[1]) pRect[3]=m_iSize[1]-1;
  if (pRect[0]<0) {iOfsX=-pRect[0];pRect[0]=0;} else iOfsX=0;
  if (pRect[1]<0) {iOfsY=-pRect[1];pRect[1]=0;} else iOfsY=0;

  return true;
}

hkvVec4 BitmapInfo_t::GetColorAt(int iX, int iY) const
{
  VASSERT(iX>=0 && iX<m_iSize[0]);
  VASSERT(iY>=0 && iY<m_iSize[1]);
  float *pAsFloats;
  UBYTE *pAsBytes;
  VColorRef *pAsCRef;
  hkvVec4* pAsVector;

  switch (m_eComponentType)
  {
    case ByteLuminance:
      pAsBytes = (UBYTE *)m_pData;
      pAsBytes += m_iStride*iY + iX;
      return hkvVec4 (pAsBytes[0]*(1.f/255.f),pAsBytes[0]*(1.f/255.f),pAsBytes[0]*(1.f/255.f),1.f);

    case FloatLuminance:
      pAsFloats = (float *)m_pData;
      pAsFloats += m_iStride*iY + iX;
      return hkvVec4 (pAsFloats[0],pAsFloats[1],pAsFloats[2],1.f);

    case ByteRGBA:
      pAsCRef = (VColorRef *)m_pData;
      pAsCRef += m_iStride*iY + iX;
      return hkvVec4 (pAsCRef->r*(1.f/255.f),pAsCRef->g*(1.f/255.f),pAsCRef->b*(1.f/255.f),pAsCRef->a*(1.f/255.f));

    case FloatRGBA:
      pAsVector = (hkvVec4*) m_pData;
      return pAsVector[m_iStride*iY + iX];
    default: 
      VASSERT(!"Unsupported component type");
  }

  return hkvVec4();
}

void BitmapInfo_t::SetColorAt(int iX, int iY, const hkvVec4& vColor)
{
  VASSERT(iX>=0 && iX<m_iSize[0]);
  VASSERT(iY>=0 && iY<m_iSize[1]);
  float *pAsFloats;
  UBYTE *pAsBytes;
  VColorRef *pAsCRef;
  hkvVec4* pAsVector;

  switch (m_eComponentType)
  {
    case ByteLuminance:
      pAsBytes = (UBYTE *)m_pData;
      pAsBytes += m_iStride*iY + iX;
      pAsBytes[0] = BlittingHelpers::ClampInt((int)(vColor.r*255.999f),0,255);
      break;
    case FloatLuminance:
      pAsFloats = (float *)m_pData;
      pAsFloats += m_iStride*iY + iX;
      pAsFloats[0] = vColor.x;
      break;
    case ByteRGBA:
      pAsCRef = (VColorRef *)m_pData;
      pAsCRef += m_iStride*iY + iX;
      pAsCRef->r = BlittingHelpers::ClampInt((int)(vColor.r*255.999f),0,255);
      pAsCRef->g = BlittingHelpers::ClampInt((int)(vColor.g*255.999f),0,255);
      pAsCRef->b = BlittingHelpers::ClampInt((int)(vColor.b*255.999f),0,255);
      pAsCRef->a = BlittingHelpers::ClampInt((int)(vColor.a*255.999f),0,255);
      break;
    case FloatRGBA:
      pAsVector = (hkvVec4*) m_pData;
      pAsVector[m_iStride*iY + iX] = vColor;
      break;
    default: 
      VASSERT(!"Unsupported component type");
  }
}



  // replace (no blending)
hkvVec4 VISION_FASTCALL BlittingHelpers::CombineReplace(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor)
{
  return hkvVec4(
    srcColor.x*auxColor.x,
    srcColor.y*auxColor.y,
    srcColor.z*auxColor.z,
    srcColor.w*auxColor.w);
}

  // boolean mask
hkvVec4 VISION_FASTCALL BlittingHelpers::BooleanMask(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor)
{
  return hkvVec4(
    srcColor.x>0.5f ? auxColor.x : dstColor.x,
    srcColor.y>0.5f ? auxColor.y : dstColor.y,
    srcColor.z>0.5f ? auxColor.z : dstColor.z,
    srcColor.w>0.5f ? auxColor.w : dstColor.w);
}

// additive blending
hkvVec4 VISION_FASTCALL BlittingHelpers::CombineAdditive(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor) 
{
  return hkvVec4(
    dstColor.x+srcColor.x*auxColor.x,
    dstColor.y+srcColor.y*auxColor.y,
    dstColor.z+srcColor.z*auxColor.z,
    dstColor.w+srcColor.w*auxColor.w);
}

// add source luminance
hkvVec4 VISION_FASTCALL BlittingHelpers::CombineAddSrcLuminance(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor) 
{
  float lum = (srcColor.r+srcColor.g+srcColor.b) * (1.f/3.f);
  return hkvVec4(
    dstColor.x+lum*auxColor.x,
    dstColor.y+lum*auxColor.y,
    dstColor.z+lum*auxColor.z,
    dstColor.w+lum*auxColor.w);
}

// alpha blending
hkvVec4 VISION_FASTCALL BlittingHelpers::CombineSrcAlpha(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor) 
{
  float a0 = ClampFloat(srcColor.a * auxColor.a,0,1); 
  float a1 = 1.f-a0; 
  return hkvVec4(
    dstColor.x*a1+srcColor.x*auxColor.x*a0,
    dstColor.y*a1+srcColor.y*auxColor.y*a0,
    dstColor.z*a1+srcColor.z*auxColor.z*a0,
    dstColor.w*a1+srcColor.w*auxColor.w*a0);
}

// multiplicative blending
hkvVec4 VISION_FASTCALL BlittingHelpers::CombineModulate(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor) 
{
  return hkvVec4(
    dstColor.x*srcColor.x*auxColor.x,
    dstColor.y*srcColor.y*auxColor.y,
    dstColor.z*srcColor.z*auxColor.z,
    dstColor.w*srcColor.w*auxColor.w);
}


// blending using source luminance
hkvVec4 VISION_FASTCALL BlittingHelpers::CombineSrcLuminance(const hkvVec4& srcColor, const hkvVec4& dstColor, const hkvVec4& auxColor) 
{
  float a0 = ClampFloat((srcColor.r+srcColor.g+srcColor.b) * (1.f/3.f) * auxColor.a,0,1); 
  float a1 = 1.f-a0; 
  return hkvVec4(
    dstColor.x*a1+srcColor.x*auxColor.x*a0,
    dstColor.y*a1+srcColor.y*auxColor.y*a0,
    dstColor.z*a1+srcColor.z*auxColor.z*a0,
    dstColor.w*a1+srcColor.w*auxColor.w*a0);
}


void BlittingHelpers::BlitBitmap(const VisBitmap_cl *pSrcBitmap, BitmapInfo_t &targetInfo, int iX, int iY, const hkvVec4& auxColor, PixelCombineCallback combine, VColorComponentBits eWriteMask)
{
  if (!combine)
    combine = CombineReplace;
  
  // clip rectangle
  int iOfsX,iOfsY;
  int iRect[4] = {iX,iY,iX+pSrcBitmap->GetWidth()-1,iY+pSrcBitmap->GetHeight()-1};
  if (!targetInfo.ScissorRect(iRect,iOfsX,iOfsY))
    return;

  iOfsX-=iX;
  iOfsY-=iY;
  const float fScale = 1.f/255.f;
  for (int y=iRect[1];y<=iRect[3];y++)
    for (int x=iRect[0];x<=iRect[2];x++)
    {
      VColorRef iColor = ((VisBitmap_cl *)pSrcBitmap)->LookupTexelColor(x+iOfsX,y+iOfsY);
      hkvVec4 vSrcColor((float)iColor.r*fScale,(float)iColor.g*fScale,(float)iColor.b*fScale,(float)iColor.a*fScale);
      hkvVec4 vDstColor = targetInfo.GetColorAt(x,y);

      hkvVec4 result = combine(vSrcColor,vDstColor,auxColor);
      if (eWriteMask&Red)   vDstColor.r = result.r;
      if (eWriteMask&Green) vDstColor.g = result.g;
      if (eWriteMask&Blue)  vDstColor.b = result.b;
      if (eWriteMask&Alpha) vDstColor.a = result.a;
      targetInfo.SetColorAt(x,y,vDstColor);
    }
}


void BlittingHelpers::StretchBlitBitmap(const VisBitmap_cl *pSrcBitmap, BitmapInfo_t &targetInfo, int iX, int iY, int iSizeX, int iSizeY, const hkvVec4& auxColor, PixelCombineCallback combine, VColorComponentBits eWriteMask)
{
  if (!combine)
    combine = CombineReplace;
  
  // adjust to target size
  if (iSizeX<0) iSizeX = targetInfo.m_iSize[0];
  if (iSizeY<0) iSizeY = targetInfo.m_iSize[1];

  // clip rectangle
  int iOfsX,iOfsY;
  int iRect[4] = {iX,iY,iX+iSizeX-1,iY+iSizeY-1};
  if (iSizeX<1 || iSizeY<1 || !targetInfo.ScissorRect(iRect,iOfsX,iOfsY))
    return;

  if (iX>0) iOfsX-=iX;
  if (iY>0) iOfsY-=iY;
  hkvVec4 vSrcColor;
  const float fScaleU = 1.f/(float)iSizeX;
  const float fScaleV = 1.f/(float)iSizeY;
  const float fScale = 1.f/255.f;

  for (int y=iRect[1];y<=iRect[3];y++)
    for (int x=iRect[0];x<=iRect[2];x++)
    {
      ((VisBitmap_cl *)pSrcBitmap)->LookupTexelColor(vSrcColor, ((float)(x+iOfsX)+0.5f)*fScaleU,((float)(y+iOfsY)+0.5f)*fScaleV);
      vSrcColor.r *= fScale;
      vSrcColor.g *= fScale;
      vSrcColor.b *= fScale;
      vSrcColor.a *= fScale;
      hkvVec4 vDstColor = targetInfo.GetColorAt(x,y);

      hkvVec4 result = combine(vSrcColor,vDstColor,auxColor);
      if (eWriteMask&Red)   vDstColor.r = result.r;
      if (eWriteMask&Green) vDstColor.g = result.g;
      if (eWriteMask&Blue)  vDstColor.b = result.b;
      if (eWriteMask&Alpha) vDstColor.a = result.a;
      targetInfo.SetColorAt(x,y,vDstColor);
    }
}


static inline hkvVec2 BiLerp(const hkvVec2 pSrcVertexCoords[4], const hkvVec2 &coord1)
{
  hkvVec2 coord0(1.f-coord1.x, 1.f-coord1.y);

  hkvVec2 row0 = pSrcVertexCoords[0] * coord0.x + pSrcVertexCoords[1] * coord1.x;
  hkvVec2 row1 = pSrcVertexCoords[2] * coord0.x + pSrcVertexCoords[3] * coord1.x;
  return row0*coord0.y + row1*coord1.y;
}

void BlittingHelpers::StretchBlitBitmap(const VisBitmap_cl *pSrcBitmap, const hkvVec2 pSrcVertexCoords[4], BitmapInfo_t &targetInfo, int iX, int iY, int iSizeX, int iSizeY, const hkvVec4& auxColor, PixelCombineCallback combine, VColorComponentBits eWriteMask)
{
  if (pSrcVertexCoords==NULL)
  {
    StretchBlitBitmap(pSrcBitmap, targetInfo, iX,iY,iSizeX,iSizeY,auxColor,combine,eWriteMask);
    return;
  }
  // adjust to target size
  if (iSizeX<0) iSizeX = targetInfo.m_iSize[0];
  if (iSizeY<0) iSizeY = targetInfo.m_iSize[1];

  // clip rectangle
  int iOfsX,iOfsY;
  int iRect[4] = {iX,iY,iX+iSizeX-1,iY+iSizeY-1};
  if (iSizeX<1 || iSizeY<1 || !targetInfo.ScissorRect(iRect,iOfsX,iOfsY))
    return;

  if (iX>0) iOfsX-=iX;
  if (iY>0) iOfsY-=iY;
  hkvVec4 vSrcColor;
  const float fScaleU = 1.f/(float)iSizeX;
  const float fScaleV = 1.f/(float)iSizeY;
  const float fScale = 1.f/255.f;

  hkvVec2 coord;
  for (int y=iRect[1];y<=iRect[3];y++)
    for (int x=iRect[0];x<=iRect[2];x++)
    {
      coord.x = ((float)(x+iOfsX)+0.5f)*fScaleU;
      coord.y = ((float)(y+iOfsY)+0.5f)*fScaleV;
      // lerp coordinates:
      coord = BiLerp(pSrcVertexCoords,coord);
      if (coord.x<0.f || coord.y<0.f || coord.x>=1.0f || coord.y>=1.0f)
        continue;

      ((VisBitmap_cl *)pSrcBitmap)->LookupTexelColor(vSrcColor, coord.x,coord.y);
      vSrcColor.r *= fScale;
      vSrcColor.g *= fScale;
      vSrcColor.b *= fScale;
      vSrcColor.a *= fScale;
      hkvVec4 vDstColor = targetInfo.GetColorAt(x,y);

      hkvVec4 result = combine(vSrcColor,vDstColor,auxColor);
      if (eWriteMask&Red)   vDstColor.r = result.r;
      if (eWriteMask&Green) vDstColor.g = result.g;
      if (eWriteMask&Blue)  vDstColor.b = result.b;
      if (eWriteMask&Alpha) vDstColor.a = result.a;
      targetInfo.SetColorAt(x,y,vDstColor);
    }
}

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
