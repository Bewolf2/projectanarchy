/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCompressionHelper.hpp

#ifndef VCOMPRESSIONHELPER_HPP_INCLUDED
#define VCOMPRESSIONHELPER_HPP_INCLUDED

// Note: Functions in this class use VColorRef because they need to know the
// order of the components in each specific platform.
namespace VCompressionHelper
{
  VBASE_IMPEXP void DecodeDXT1A(VColorRef *pDest, const UBYTE *src, int w, int h);
  VBASE_IMPEXP void DecodeDXT3(VColorRef *pDest, const UBYTE *src, int w, int h);
  VBASE_IMPEXP void DecodeDXT5(VColorRef *pDest, const UBYTE *src, int w, int h);

  VBASE_IMPEXP void DecodeBGR555(VColorRef *pDest, const unsigned short *src, int w, int h);
  VBASE_IMPEXP void DecodeBGR565(VColorRef *pDest, const unsigned short *src, int w, int h);
  VBASE_IMPEXP void DecodeBGRA5551(VColorRef *pDest, const unsigned short *src, int w, int h);
  VBASE_IMPEXP void DecodeBGRA4444(VColorRef *pDest, const unsigned short *src, int w, int h);

  VBASE_IMPEXP void DecodeRLE(UBYTE *pDest, const UBYTE *pSrc, int iDestTexelCount, int iTexelStride);
  VBASE_IMPEXP void DecodeRLE(UBYTE *pDest, IVFileInStream *pSrcStream, int iDestTexelCount, int iTexelStride);

#if defined(SUPPORTS_TPL)
  VBASE_IMPEXP void DecodeTPL_CMPR(VColorRef *pDest, const UBYTE *src, int w, int h);
#endif

  // Adds an alpha component without modifying the layout in memory of the rest of the components
  VBASE_IMPEXP void AddAlphaComponent(UBYTE *pDest, const UBYTE* pSrc, int iTexelCount);

  VBASE_IMPEXP void UnSwizzleData(VColorRef *pDest, const UBYTE* pSrc, int iTexelCount, const char mask[4], int iSrcComponents);

  VBASE_IMPEXP void ReorderDXT1Morton(DWORD *pDest, DWORD *pSrc, int iWidth, int iHeight);
  VBASE_IMPEXP void ReorderDXT3Morton(DWORD *pDest, DWORD *pSrc, int iWidth, int iHeight);
  VBASE_IMPEXP void ReorderDXT5Morton(DWORD *pDest, DWORD *pSrc, int iWidth, int iHeight);
  VBASE_IMPEXP void ReorderRGBAMorton(DWORD *pDest, DWORD *pSrc, int iWidth, int iHeight);
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
