/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTextureObject.inl



  
inline VTextureLoader::VTextureType_e VTextureObject::GetTextureType() const 
{
  return static_cast<VTextureLoader::VTextureType_e>(m_eTextureType);
}


// ---------------------------------------------------------------------------------
// Method: GetTextureColorDepth
// Author: Florian Born
// Notes: Returns the colordepth (in bits) of the internal format
//        compressed formats may return 4 or 8 (storage for each pixel)
// ---------------------------------------------------------------------------------
inline int VTextureObject::GetTextureColorDepth() const 
{
  return GetColorDepthFromTextureFormat((VTextureLoader::VTextureFormat_e)m_eTextureFormat);
}


// ---------------------------------------------------------------------------------
// Method: GetTextureColorDepthUncompressed
// Author: Florian Born
// Notes: Returns the colordepth (in bits) of the internal format
//        compressed formats return 24 or 32 (RGB or RGBA)
// ---------------------------------------------------------------------------------
inline int VTextureObject::GetTextureColorDepthUncompressed() const 
{
  switch (m_eTextureFormat)
  {
    case VTextureLoader::DXT1:
    case VTextureLoader::DXT1A:
      return 24;

    case VTextureLoader::DXT3:
    case VTextureLoader::DXT5:
      return 32;
  }

  return GetColorDepthFromTextureFormat((VTextureLoader::VTextureFormat_e)m_eTextureFormat);
}


// ---------------------------------------------------------------------------------
// Method: GetTextureFormat
// Author: Florian Born
// Notes: Get the internal format constant
// ---------------------------------------------------------------------------------
inline VTextureLoader::VTextureFormat_e VTextureObject::GetTextureFormat() const 
{
  return static_cast<VTextureLoader::VTextureFormat_e>(m_eTextureFormat);
}


// ---------------------------------------------------------------------------------
// Method: IsTexture2D
// Author: Florian Born
// Notes: Indicates if texture is a 2D texture
// ---------------------------------------------------------------------------------
inline bool VTextureObject::IsTexture2D() const
{
  return m_eTextureType == VTextureLoader::Texture2D;
}


// ---------------------------------------------------------------------------------
// Method: IsTexture3D
// Author: Florian Born
// Notes: Indicates if texture is a 3D texture
// ---------------------------------------------------------------------------------
inline bool VTextureObject::IsTexture3D() const
{
  return m_eTextureType == VTextureLoader::Texture3D;
}

#if defined(SUPPORTS_2D_TEXTURE_ARRAYS)

// ---------------------------------------------------------------------------------
// Method: IsTexture2DArray
// Author: Dag Frommhold
// Notes: Indicates if texture is a 2D texture array
// ---------------------------------------------------------------------------------
inline bool VTextureObject::IsTexture2DArray() const
{
  return m_eTextureType == VTextureLoader::Texture2DArray;
}

#endif

// ---------------------------------------------------------------------------------
// Method: IsCubemap
// Author: Florian Born
// Notes: Indicates if texture is a cubemap texture
// ---------------------------------------------------------------------------------
inline bool VTextureObject::IsCubemap() const
{
  return m_eTextureType == VTextureLoader::Cubemap;
}


// ---------------------------------------------------------------------------------
// Method: IsHeightMap
// Author: Florian Born
// Notes: Indicates if texture is a heightmap
// ---------------------------------------------------------------------------------
inline bool VTextureObject::IsHeightMap()  const 
{
  return m_eTextureType == VTextureLoader::Heightmap16bpp;
}



inline int VTextureObject::GetMipMapLevelCount() const
{
  return m_iMipLevels;
}


// ---------------------------------------------------------------------------------
// Method: IsAnimated
// Author: Florian Born
// Notes: Returns 1 if texture is animated
// ---------------------------------------------------------------------------------
inline bool VTextureObject::IsAnimated() const 
{
  return (m_pAnimInfo!=NULL);
}

// ---------------------------------------------------------------------------------
// Method: GetNumAnimPhases
// Author: Florian Born
// Notes: Get the number of animation phases of this texture
// ---------------------------------------------------------------------------------
inline int VTextureObject::GetNumAnimPhases() const
{
  if (m_pAnimInfo)
    return m_pAnimInfo->numFrames;
  return 0;
}

// ---------------------------------------------------------------------------------
// Method: IsCompressed
// Author: Florian Born
// Notes: Returns 1 if the internal format of the texture is compressed
// ---------------------------------------------------------------------------------
inline bool VTextureObject::IsCompressed() const
{
  if (IsHeightMap()) return false; ///< internal format is just 8/16/32bpp
  return IsFormatCompressed(GetTextureFormat());
}


// ---------------------------------------------------------------------------------
// Method: IsTexturePowerOfTwo
// Author: Florian Born
// Notes: Returns 1 if all dimensions of the texture are a power of 2
// ---------------------------------------------------------------------------------
inline bool VTextureObject::IsTexturePowerOfTwo() const
{
  return (hkvMath::isPowerOf2 (m_iSizeX) && hkvMath::isPowerOf2 (m_iSizeY));
}


// ---------------------------------------------------------------------------------
// Method: GetTextureAnimFrame
// Author: Florian Born
// Notes: Get the pointer to the n-th animation frame texture
// ---------------------------------------------------------------------------------
inline VTextureObject *VTextureObject::GetTextureAnimFrame(int framenum)
{
  VASSERT(framenum>=0 && framenum<m_pAnimInfo->numFrames);

  return m_pAnimInfo->spFrame[framenum];
}

#ifdef _VR_DX9

IDirect3DBaseTexture9* VTextureObject::GetD3DInterface()
{
  return m_spD3Dinterface;
}

#elif defined(_VR_DX11)


D3DResource* VTextureObject::GetD3DResource()
{
  return m_spD3DResource;
}

/*
D3DShaderResourceView* VTextureObject::GetD3DShaderResourceView()
{
  return m_spD3DShaderResourceView;
}
*/

#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )
bool VTextureObject::CreateShaderResourceView(D3DResource *pResource, D3D_SHADER_RESOURCE_VIEW_DESC *pResourceViewDesc)
{
  if (!pResource)
    return false;
  m_spShaderResourceView = NULL;
  D3DShaderResourceView *pResourceView = NULL;
  HRESULT res = VVideo::GetD3DDevice()->CreateShaderResourceView(pResource, pResourceViewDesc, &pResourceView);
  if (res != S_OK)
    return false;
  m_spShaderResourceView = new VShaderResourceView( pResourceViewDesc );
  m_spShaderResourceView->SetView( pResourceView );
  pResourceView->Release();
  return true;
}

bool VTextureObject::CreateUnorderedAccessView(D3DResource *pResource, D3D_UNORDERED_ACCESS_VIEW_DESC *pUavDesc)
{
  if (!pResource)
    return false;
  m_spUnorderedAccessView = NULL;
  D3DUnorderedAccessView *pUav = NULL;
  HRESULT res = VVideo::GetD3DDevice()->CreateUnorderedAccessView(pResource, pUavDesc, &pUav);
  if (res != S_OK)
    return false;
  m_spUnorderedAccessView = new VUnorderedAccessView( pUavDesc );
  m_spUnorderedAccessView->SetView( pUav );
  pUav->Release();
  return true;
}

VShaderResourceView* VTextureObject::GetShaderResourceView() const
{
  return m_spShaderResourceView.GetPtr();
}

VUnorderedAccessView* VTextureObject::GetUnorderedAccessView() const
{
  return m_spUnorderedAccessView.GetPtr();
}
#endif



#ifdef _VR_GCM
CellGcmTexture* VTextureObject::GetGCMTexture()
{
  return &m_GcmTexture;
}
#endif


#ifdef _VR_GXM
SceGxmTexture* VTextureObject::GetGXMTexture()
{
  if (!HasDeviceHandle())
  {
    return 0;
  }
  return &m_GxmTexture;
}
#endif


//@@@L opengl?

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
