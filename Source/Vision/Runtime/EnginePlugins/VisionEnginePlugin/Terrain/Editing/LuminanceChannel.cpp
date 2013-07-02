/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/LuminanceChannel.hpp>


VLuminanceChannel::VLuminanceChannel(int iSizeX, int iSizeY, UBYTE fillVal)
{
  m_iSizeX = iSizeX;
  m_iSizeY = iSizeY;
  m_pBuffer = NULL;
  if (m_iSizeX>0 && m_iSizeY>0)
  {
    m_pBuffer = new UBYTE[m_iSizeX*m_iSizeY];
    memset(m_pBuffer,fillVal,m_iSizeX*m_iSizeY);
  }
  m_bDirty = true;
  m_bIsZero = fillVal==0;
}

void VLuminanceChannel::CopyFrom(const VLuminanceChannel *pOther)
{
  if (pOther==this || pOther==NULL)
    return;
  if (m_iSizeX!=pOther->m_iSizeX || m_iSizeY!=pOther->m_iSizeY)
  {
    V_SAFE_DELETE_ARRAY(m_pBuffer);
    m_iSizeX=pOther->m_iSizeX;
    m_iSizeY=pOther->m_iSizeY;
    if (m_iSizeX>0 && m_iSizeY>0)
      m_pBuffer = new UBYTE[m_iSizeX*m_iSizeY];
    m_spLuminanceTexture = NULL;
  }
  if (m_iSizeX>0 && m_iSizeY>0)
    memcpy(m_pBuffer,pOther->m_pBuffer,m_iSizeX*m_iSizeY);

  SetDirtyFlag();
}


bool VLuminanceChannel::ReadScaled(VChunkFile &file, int iSizeXInFile, int iSizeYInFile)
{
  if (m_iSizeX==iSizeXInFile && m_iSizeY==iSizeYInFile)
  {
    file.Read(GetValuePtr(0,0),iSizeXInFile*iSizeYInFile);
    return false;
  } 

  // rescale 
  VMemoryTempBuffer<128*128> buffer(iSizeXInFile*iSizeYInFile);
  UBYTE *pSrc = (UBYTE *)buffer.GetBuffer();
  file.Read(pSrc,iSizeXInFile*iSizeYInFile);
  int iWantedSizeX = m_iSizeX;
  int iWantedSizeY = m_iSizeY;
  for (int y=0;y<iWantedSizeY;y++)
  {
    UBYTE *pDest = GetValuePtr(0,y);
    for (int x=0;x<iWantedSizeX;x++,pDest++)
    {
      int xs = x*iSizeXInFile/iWantedSizeX;
      int ys = y*iSizeYInFile/iWantedSizeY;
      *pDest = pSrc[ys*iSizeXInFile+xs];
    }
  }
  return true; 
}

void VLuminanceChannel::Rescale(int iNewSizeX, int iNewSizeY)
{
  if (m_iSizeX==iNewSizeX && m_iSizeY==iNewSizeY)
    return;
  VASSERT(m_iSizeX>0 && m_iSizeY>0 && m_pBuffer!=NULL)
  UBYTE *pNewBuffer = new UBYTE[iNewSizeX*iNewSizeY];

  // stretch (non-interpolated)
  UBYTE *pDest = pNewBuffer;
  for (int y=0;y<iNewSizeY;y++)
    for (int x=0;x<iNewSizeX;x++,pDest++)
    {
      int xs = x*m_iSizeX/iNewSizeX;
      int ys = y*m_iSizeY/iNewSizeY;
      *pDest = m_pBuffer[ys*m_iSizeX+xs];
    }

    // set new dimension
  m_iSizeX = iNewSizeX;
  m_iSizeY = iNewSizeY;
  V_SAFE_DELETE_ARRAY(m_pBuffer);
  m_pBuffer = pNewBuffer;
  m_spLuminanceTexture = NULL;
  m_bDirty = true;
}



void VLuminanceChannel::Blend(const hkvRect& destRect, const hkvRect& srcRect, const VLuminanceChannel *pOther)
{
  VASSERT_MSG(destRect.getHeight() == srcRect.getHeight() && destRect.getWidth() == srcRect.getWidth(), "Mismatching blend regions");

  for(int rowIdx = 0; rowIdx < destRect.getHeight(); rowIdx++)
  {
    const UBYTE *pSrc = pOther->GetValuePtr(srcRect.getX(), srcRect.getY() + rowIdx);
    UBYTE *pDest = this->GetValuePtr(destRect.getX(), destRect.getY() + rowIdx);

    for (int colIdx = 0; colIdx < destRect.getWidth(); colIdx++)
    {
      float fWgt = (float)pSrc[colIdx] * (1.f/255.f);
      *pDest = (int)((float)pDest[colIdx] * (1.f-fWgt) + 255.999f*fWgt);
    }
  }
  
  SetDirtyFlag();
}


float VLuminanceChannel::AlphaMaskCompare(const hkvRect& destRect, const hkvRect& srcRect, const VLuminanceChannel *pOther) const
{
  float fVal = 0.0f;
  
  for(int rowIdx = 0; rowIdx < destRect.getHeight(); rowIdx++)
  {
    const UBYTE *pSrc = pOther->GetValuePtr(srcRect.getX(), srcRect.getY() + rowIdx);
    const UBYTE *pDest = this->GetValuePtr(destRect.getX(), destRect.getY() + rowIdx);

    for (int colIdx = 0; colIdx < destRect.getWidth(); colIdx++)
    {
        float fShineThrough = float(pDest[colIdx] * (255 - pSrc[colIdx])) / (255.0f * 255.0f);
        fVal += fShineThrough*fShineThrough; // square the result to give extra penalty for opaque parts
    }
  }

  return fVal*255.f;
}


float VLuminanceChannel::GetValueI(float x, float y) const
{
  float xb = x*(float)m_iSizeX;
  float yb = y*(float)m_iSizeY;
  float rx = xb - hkvMath::floor(xb);
  float ry = yb - hkvMath::floor(yb);
  int x0 = (int)xb;
  int y0 = (int)yb;
  if (x0<0) x0=0; else if (x0>=m_iSizeX) x0=m_iSizeX-1;
  if (y0<0) y0=0; else if (y0>=m_iSizeY) y0=m_iSizeY-1;
  int x1 = hkvMath::Min(x0+1, m_iSizeX-1);
  int y1 = hkvMath::Min(y0+1, m_iSizeY-1);

  float v00 = (float)GetValue(x0,y0);
  float v10 = (float)GetValue(x1,y0);
  float v01 = (float)GetValue(x0,y1);
  float v11 = (float)GetValue(x1,y1);

  return ((v11*rx+v01*(1.f-rx))*ry + (v10*rx+v00*(1.f-rx))*(1-ry))*(1.f/255.f);
}



VTextureObject *VLuminanceChannel::GetLuminanceTexture(int iSectorX, int iSectorY, int iDbgID)
{
  if (!m_spLuminanceTexture)
  {
    char szName[128];
    sprintf(szName,"<Luminance_S%ix%i_ID%i>",iSectorX,iSectorY,iDbgID);
    m_spLuminanceTexture = Vision::TextureManager.Create2DTextureObject(szName,m_iSizeX,m_iSizeY,1,VTextureLoader::L8);
    m_spLuminanceTexture->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    m_bDirty = true;
  }

  if (m_bDirty)
  {
    //m_spLuminanceTexture->UploadDataMipmap(0,(const char *)m_pBuffer);
    m_spLuminanceTexture->UpdateRect(0, 0, 0, 0, 0, -1, (const char *)m_pBuffer, V_TEXTURE_LOCKFLAG_DISCARDABLE);
    m_bDirty = false;
  }
  return m_spLuminanceTexture;
}


V_IMPLEMENT_SERIALX(VLuminanceChannel);
void VLuminanceChannel::SerializeX( VArchive &ar, int iWantedSizeX, int iWantedSizeY)
{
  char iCurrentVersion=0;
  if (ar.IsLoading())
  {
#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=187
#endif

    char iVersion;
    ar >> iVersion; VASSERT(iVersion>=0 && iVersion<=iCurrentVersion);

#if defined(__SNC__)
#pragma diag_pop
#endif

    m_spLuminanceTexture = NULL;
    m_bDirty = true;
    m_bIsZero = -1;

    int iActualSizeX, iActualSizeY;
    ar >> iActualSizeX >> iActualSizeY;
    if (iWantedSizeX<0) iWantedSizeX = iActualSizeX;
    if (iWantedSizeY<0) iWantedSizeY = iActualSizeY;

    V_SAFE_DELETE_ARRAY(m_pBuffer);
    int iCount = iActualSizeX*iActualSizeY;
    if (iCount==0)
    {
      m_iSizeX = m_iSizeY = 0;
      return;
    }

    m_pBuffer = new UBYTE[iWantedSizeX*iWantedSizeY];
    if (iActualSizeX==iWantedSizeX && iActualSizeY==iWantedSizeY)
      ar.Read(m_pBuffer,iCount);
    else
    {
      // rescale:
      VMemoryTempBuffer<128*128> buffer(iCount);
      UBYTE *pSrc = (UBYTE *)buffer.GetBuffer();
      ar.Read(pSrc,iCount);
      for (int y=0;y<iWantedSizeY;y++)
      {
        UBYTE *pDest = GetValuePtr(0,y);
        for (int x=0;x<iWantedSizeX;x++,pDest++)
        {
          int xs = x*iActualSizeX/iWantedSizeX;
          int ys = y*iActualSizeY/iWantedSizeY;
          *pDest = pSrc[ys*iActualSizeX+xs];
        }
      }
    }

    m_iSizeX = iWantedSizeX;
    m_iSizeY = iWantedSizeY;
  } else
  {
    ar << iCurrentVersion;
    ar << m_iSizeX << m_iSizeY;
    ar.Write(m_pBuffer,m_iSizeX*m_iSizeY);
  }
}

bool VLuminanceChannel::CheckSolidColor( UBYTE val ) const
{
  int byteSize = m_iSizeX * m_iSizeY;

  UINT_PTR ptr = reinterpret_cast<UINT_PTR>(m_pBuffer);
  UINT_PTR ptr4 = ((ptr - 1) / 4 + 1) * 4;
  UINT_PTR ptr16 = ((ptr - 1) / 16 + 1) * 16;

  UINT_PTR end = ptr + byteSize;
  UINT_PTR end4 = (end / 4) * 4;

  // Compare bytewise until reaching 32bit alignment
  while(ptr < ptr4)
  {
    if(*reinterpret_cast<UBYTE*>(ptr) != val)
    {
      return false;
    }

    ptr++;
  }

  // Compare 32b at once until reaching 128b alignment
  UINT32 intVal = val * 0x01010101u;
  while(ptr4 < ptr16)
  {
    if (*reinterpret_cast<UINT32*>(ptr4) != intVal)
    {
      return false;
    }

    ptr4 += 4;
  }

/*#if defined(WIN32)
  // Compare 128b at once
  __m128i sseVal = _mm_set1_epi32(intVal);
  UINT_PTR end16 = (end / 16) * 16;
  while(ptr16 < end16)
  {
    if (_mm_movemask_epi8(_mm_cmpeq_epi32(*reinterpret_cast<__m128i*>(ptr16), sseVal)) != 0xffff)
    {
      return false;
    }

    ptr16 += 16;
  }
#endif*/

  // Compare trailing ints
  ptr4 = ptr16;
  while(ptr4 < end4)
  {
    if (*reinterpret_cast<UINT32*>(ptr4) != intVal)
    {
      return false;
    }

    ptr4 += 4;
  }

  // Compare trailing bytes
  ptr = ptr4;
  while(ptr < end)
  {
    if(*reinterpret_cast<UBYTE*>(ptr) != val)
    {
      return false;
    }

    ptr++;
  }

  return true;
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
