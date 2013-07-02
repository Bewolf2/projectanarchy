/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/TerrainManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/TerrainPlugin/TerrainManaged/HeightmapImporters.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;



namespace TerrainManaged
{

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // class IHeightmapImporterFromFile
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  IHeightmapImporterFromFile::IHeightmapImporterFromFile()
  {
    m_iSizeX = m_iSizeY = 0;
    m_pLastError = nullptr;
    m_pHeightData = NULL;
  }

  bool IHeightmapImporterFromFile::OnStart(IValueProvider ^originalValues, int x1, int y1, int x2, int y2)
  {
    if (!IHeightmapImporter::OnStart(originalValues,x1,y1,x2,y2))
      return false;

    return m_pHeightData!=NULL;
  }

  void IHeightmapImporterFromFile::OnFinish()
  {
    V_SAFE_DELETE_ARRAY(m_pHeightData);
  }

  float IHeightmapImporterFromFile::GetHeightAtInternal(int x, int y)
  {
    VASSERT(m_pHeightData);
    if (x<0) x=0; else if (x>=m_iSizeX) x = m_iSizeX-1;
    if (y<0) y=0; else if (y>=m_iSizeY) y = m_iSizeY-1;
    return m_pHeightData[y*m_iSizeX+x] * _fScaling; // apply scaling here so we don't have to reload every time it changes
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // class IHeightmapImporterFromTextureFile
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void IHeightmapImporterFromTextureFile::LoadFromFile(String ^value)
  {
    V_SAFE_DELETE_ARRAY(m_pHeightData);
    if (String::IsNullOrEmpty(value))
      return;

    Image_cl image;
    VString sFilename;
    ConversionUtils::StringToVString(FileName,sFilename);
    RETVAL ret = image.Load(sFilename,Vision::File.GetManager());
    if (ret!=VERR_NOERROR)
    {
      switch (ret)
      {
        case VERR_FILENOTFOUND:m_pLastError="File not found";break;
      }
      return;
    }

    if (image.HasHeightMap())
    {
      ImageMap_cl hmap = image.GetHeightMap(0);
      m_iSizeX = image.GetWidth();
      m_iSizeY = image.GetHeight();
      m_pHeightData = new float[m_iSizeX*m_iSizeY];
      float *pDst = m_pHeightData; 
      unsigned short *pSrc = (unsigned short *)hmap.GetData();
      for (int y=0;y<m_iSizeY;y++)
        for (int x=0;x<m_iSizeX;x++,pSrc++,pDst++)
          *pDst = (float)(*pSrc);
    }
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // class IHeightmapImporterFromRawFile
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void IHeightmapImporterFromRawFile::LoadFromFile(String ^value)
  {
    V_SAFE_DELETE_ARRAY(m_pHeightData);
    VString sFilename;
    ConversionUtils::StringToVString(FileName,sFilename);
    if (sFilename.IsEmpty())
      return;

    IVFileInStream *pIn = Vision::File.Open(sFilename);
    if (pIn==NULL)
    {
      m_pLastError="File not found";
      return;
    }

    int iSize = pIn->GetSize() - m_iHeaderSize - m_iOutroSize;
    int iSamples = iSize/(m_iComponentSize/8);
    if (iSamples<(16*16))
    {
      m_pLastError="File is too small";
      pIn->Close();
      return;
    }

    m_iSizeX = m_iSizeY = 0; // Reset to calculate new values

    if (m_iSizeX==0 && m_iSizeY==0)
    {
      if (m_iRowStride>=0)
      {
        m_iSizeX = hkvMath::Max(m_iRowStride/(m_iComponentSize/8),1);
        m_iSizeY = iSamples / m_iSizeX;
      } else
      {
        int iEdge = (int)hkvMath::sqrt((float)iSamples);
        m_iSizeX = m_iSizeY = iEdge;
      }
    }

    if (m_iSizeY==0)
      m_iSizeY = iSamples/m_iSizeX;
    else if (m_iSizeX==0)
      m_iSizeX = iSamples/m_iSizeY;

    if ((m_iSizeX*m_iSizeY)!=iSamples)
    {
      m_pLastError="File size does not match expected sample count";
      pIn->Close();
      return;
    }

    m_pHeightData = new float[iSamples];
    memset(m_pHeightData,0,sizeof(float)*iSamples);

    // read row-wise
    int iRowBytes = m_iSizeX*m_iComponentSize/8;
    int iWantedStride = iRowBytes;
    if (m_iRowStride>=0)
      iWantedStride = m_iRowStride;

    DynArray_cl<char> rb(iRowBytes,0);
    void *pRowBuffer = rb.GetDataPtr();
    int dummy;

    // discard header bytes:
    for (int q=0;q<m_iHeaderSize;q++)
      pIn->Read(&dummy,1);

    float *pDest = m_pHeightData;
    for (int y=0;y<m_iSizeY;y++)
    {
      pIn->Read(pRowBuffer,iRowBytes);
      unsigned char *pValue8 = (unsigned char *)pRowBuffer;
      unsigned short *pValue16 = (unsigned short *)pRowBuffer;
      unsigned int *pValue32 = (unsigned int *)pRowBuffer;
      for (int x=0;x<m_iSizeX;x++,pDest++)
      {
        // map every component type to 65000
        if (m_iComponentSize==8)
          *pDest = (float)pValue8[x]*256.f;
        else if (m_iComponentSize==16)
          *pDest = (float)pValue16[x];
        else if (m_iComponentSize==32)
          *pDest = (float)pValue32[x]*(1.f/65535.f);
      }

      // discard bytes (only works for stride>width)
      for (int q=iRowBytes;q<iWantedStride;q++)
        pIn->Read(&dummy,1);

    }

    pIn->Close();
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // class IHeightmapImporterFromDDSFile
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  void IHeightmapImporterFromDDSFile::LoadFromFile(String ^value)
  {
    V_SAFE_DELETE_ARRAY(m_pHeightData);
    if (String::IsNullOrEmpty(value))
      return;

    VString sFilename;
    ConversionUtils::StringToVString(FileName,sFilename);
    VTextureLoader loader;
    // Load the texture in GPU format because we'd rather have a raw buffer.
    if (!loader.Open(sFilename,Vision::File.GetManager(),VTM_FLAG_NO_MIPMAPS|VTM_FLAG_GPU_UPLOADFORMAT))
    {
      m_pLastError = "Failed to open file";
      return;
    }

    VASSERT(loader.HasRawData());
    
    if ((loader.m_eRawFormat!=VTextureLoader::R32F) &&
        (loader.m_eRawFormat!=VTextureLoader::B8G8R8A8) && (loader.m_eRawFormat!=VTextureLoader::B8G8R8) &&
        (loader.m_eRawFormat!=VTextureLoader::R8G8B8A8) && (loader.m_eRawFormat!=VTextureLoader::R8G8B8))
    {
      m_pLastError = "This DDS format is not supported for import. Please use format R32F for instance.";
      return;
    }

    m_iSizeX = loader.m_Header.dwWidth;
    m_iSizeY = loader.m_Header.dwHeight;
    m_pHeightData = new float[m_iSizeX*m_iSizeY];
    float *pDst = m_pHeightData;
    UBYTE *pSrc = loader.NextRawImage();
    int iNumComponents = 1;

    switch (loader.m_eRawFormat)
    {
    case VTextureLoader::R32F:      
      memcpy(pDst,pSrc,m_iSizeX*m_iSizeY*sizeof(float));
      break;
    case VTextureLoader::B8G8R8A8:
    case VTextureLoader::R8G8B8A8:
      iNumComponents = 4;  
    case VTextureLoader::B8G8R8:
    case VTextureLoader::R8G8B8:
      if (iNumComponents==1) iNumComponents=3;
      for (int y=0; y<m_iSizeY; y++)
      {
        for (int x=0; x<m_iSizeX; x++, pDst++, pSrc+=iNumComponents)
        {
          *pDst = ((float)pSrc[0] + (float)pSrc[1] + (float)pSrc[2]) * (1.f/765.f); // map to [0..1] range
        }
      }
      break;
    }
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
