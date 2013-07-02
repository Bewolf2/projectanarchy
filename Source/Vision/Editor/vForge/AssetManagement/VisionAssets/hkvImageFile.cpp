/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvDds.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvImageFile.hpp>

#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/System/Io/IArchive/hkIArchive.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>


const char* hkvImageFileFormatNames[HKV_IMAGE_FILE_FORMAT_COUNT] =
{
  "(Invalid)",
  "BMP",
  "DDS",
  "JPG",
  "PNG",
  "TGA"
};
HK_COMPILE_TIME_ASSERT((sizeof(hkvImageFileFormatNames) / sizeof(char*)) == HKV_IMAGE_FILE_FORMAT_COUNT);

const hkvEnumDefinition& getImageFileFormatDefinition()
{
  static hkvEnumDefinition def(HKV_IMAGE_FILE_FORMAT_COUNT, hkvImageFileFormatNames);
  return def;
}


//-----------------------------------------------------------------------------
// hkvMipImage
//-----------------------------------------------------------------------------
hkvMipImage::hkvMipImage(hkUint32 width, hkUint32 height)
: m_width(width), m_height(height), m_data(NULL)
{
  hkUint32 dataSize = m_width * m_height * 4;
  VASSERT_MSG(dataSize > 0, "hkvMipImage: width and height must both be greater than 0");
  if (dataSize > 0)
  {
    m_data = new hkUint8[dataSize];
  }
}

hkvMipImage::~hkvMipImage()
{
  delete[] m_data;
}


//-----------------------------------------------------------------------------
// hkvImageFileDds
//-----------------------------------------------------------------------------
class hkvImageFileDds : public hkvImageFile
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  hkvImageFileDds(hkStreamReader& reader, bool readData);

private:
  hkvImageFileDds(const hkvImageFileDds&);
  hkvImageFileDds& operator=(const hkvImageFileDds&);

public:
  virtual hkvImageFileFormat getFileFormat() const HKV_OVERRIDE;
  virtual hkUint32 getWidth() const HKV_OVERRIDE;
  virtual hkUint32 getHeight() const HKV_OVERRIDE;
  virtual bool hasAlpha() const HKV_OVERRIDE;

private:
  hkResult readHeader(hkStreamReader& reader);
  hkResult readImageData(hkStreamReader& reader);
  hkResult readImageBlock(hkStreamReader& reader, hkUint8* target, hkUint32 width, hkUint32 height);

private:
  DDS_HEADER m_header;
};


//-----------------------------------------------------------------------------
// hkvImageFileTga
//-----------------------------------------------------------------------------
class hkvImageFileTga : public hkvImageFile
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

private:
#pragma pack(push, 1)
  struct Header
  {
    hkUint8 identSize;          // size of ID field that follows 18 byte header (0 usually)
    hkUint8 colorMapType;      // type of color map 0=none, 1=has palette
    hkUint8 imageType;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

    hkUint16 colorMapStart;     // first color map entry in palette
    hkUint16 colorMapLength;    // number of colors in palette
    hkUint8 colorMapBits;      // number of bits per palette entry 15,16,24,32

    hkUint16 xStart;             // image x origin
    hkUint16 yStart;             // image y origin
    hkUint16 width;              // image width in pixels
    hkUint16 height;             // image height in pixels
    hkUint8 bits;               // image bits per pixel 8,16,24,32
    hkUint8 descriptor;         // image descriptor bits (vh flip bits)
  };
#pragma pack(pop)

public:
  hkvImageFileTga(hkStreamReader& reader, bool readData);

private:
  hkvImageFileTga(const hkvImageFileTga&);
  hkvImageFileTga& operator=(const hkvImageFileTga&);

public:
  virtual hkvImageFileFormat getFileFormat() const HKV_OVERRIDE;
  virtual hkUint32 getWidth() const HKV_OVERRIDE;
  virtual hkUint32 getHeight() const HKV_OVERRIDE;
  virtual bool hasAlpha() const HKV_OVERRIDE;

private:
  hkResult readHeader(hkStreamReader& reader);
  hkResult readImageData(hkStreamReader& reader);
  hkResult readRleData(hkStreamReader& reader);
  void postprocessData();

private:
  Header m_header;
};


//-----------------------------------------------------------------------------
// hkvImageFileDds
//-----------------------------------------------------------------------------
hkvImageFileDds::hkvImageFileDds(hkStreamReader& reader, bool readData)
{
  if (readHeader(reader) != HK_SUCCESS)
  {
    return;
  }

  if (!readData)
  {
    return;
  }

  if (readImageData(reader) != HK_SUCCESS)
  {
    clearData();
  }
}


hkvImageFileFormat hkvImageFileDds::getFileFormat() const
{
  return HKV_IMAGE_FILE_FORMAT_DDS;
}

hkUint32 hkvImageFileDds::getWidth() const
{
  return m_headerValid ? m_header.dwWidth : 0;
}

hkUint32 hkvImageFileDds::getHeight() const
{
  return m_headerValid ? m_header.dwHeight : 0;
}

bool hkvImageFileDds::hasAlpha() const
{
  if (!m_headerValid)
    return false;

  if (((m_header.ddspf.dwFlags & DDPF_ALPHAPIXELS) != 0) || ((m_header.ddspf.dwFlags & DDPF_ALPHA) != 0))
    return true;

  // DXT formats have alpha as well
  if (((m_header.dwFlags & DDPF_FOURCC) != 0) && 
    (m_header.ddspf.dwFourCC == DDS_FOURCC_DXT1 || m_header.ddspf.dwFourCC == DDS_FOURCC_DXT3 || m_header.ddspf.dwFourCC == DDS_FOURCC_DXT5))
    return true;

  return false;
}

hkResult hkvImageFileDds::readHeader(hkStreamReader& reader)
{
  hkIArchive archive(&reader, hkBool(HK_ENDIAN_BIG));

  // DDS header consists of DWORDs entirely, so read it as one array
  DWORD dwMagic = archive.read32u();
  int numElements = sizeof(DDS_HEADER) / sizeof(DWORD);
  archive.readArray32u(reinterpret_cast<hkUint32*>(&m_header), numElements);

  // TODO: check for and read extended DX10 header

  if (!archive.isOk() || dwMagic != DDS_MAGIC)
    return HK_FAILURE;

  m_headerValid = true;
  return HK_SUCCESS;
}

hkResult hkvImageFileDds::readImageData(hkStreamReader& reader)
{
  if (!isHeaderValid())
    return HK_FAILURE;

  DWORD checkFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
  if ((m_header.ddspf.dwFlags & checkFlags) != checkFlags)
    return HK_FAILURE;

  if (m_header.ddspf.dwRGBBitCount != 32 || m_header.ddspf.dwRBitMask != 0x00ff0000 || m_header.ddspf.dwGBitMask != 0x0000ff00
    || m_header.ddspf.dwBBitMask != 0x000000ff || m_header.ddspf.dwABitMask != 0xff000000)
    return HK_FAILURE;

  DWORD mipLevels = 1;
  if ((m_header.dwFlags & DDSD_MIPMAPCOUNT) != 0)
  {
    mipLevels = hkvMath::Max(1, m_header.dwMipMapCount);
  }

  DWORD mipWidth = m_header.dwWidth;
  DWORD mipHeight = m_header.dwHeight;
  for (DWORD mipLevel = 0; mipLevel < mipLevels; ++mipLevel)
  {
    hkvMipImage* img = new hkvMipImage(mipWidth, mipHeight);
    if (img == NULL || img->getData() == NULL)
    {
      delete img;
      return HK_FAILURE;
    }

    m_mipImages.pushBack(img);
    hkUint8* data = img->getData();

    if (readImageBlock(reader, data, mipWidth, mipHeight) != HK_SUCCESS)
    {
      return HK_FAILURE;
    }

    mipWidth = mipWidth > 1 ? mipWidth >> 1 : 1;
    mipHeight = mipHeight > 1 ? mipHeight >> 1 : 1;
  }

  return HK_SUCCESS;
}

hkResult hkvImageFileDds::readImageBlock(hkStreamReader& reader, hkUint8* target, hkUint32 width, hkUint32 height)
{
  const int scanLineSize = width * 4;

  for (DWORD y = 0; y < height; ++y)
  {
    hkUint8* targetPos = target + ((height - y - 1) * scanLineSize);
    if (reader.read(targetPos, scanLineSize) != scanLineSize)
    {
      return HK_FAILURE;
    }
  }

  return HK_SUCCESS;
}


//-----------------------------------------------------------------------------
// hkvImageFileTga
//-----------------------------------------------------------------------------
hkvImageFileTga::hkvImageFileTga(hkStreamReader& reader, bool readData)
{
  if (readHeader(reader) != HK_SUCCESS)
  {
    return;
  }

  if (!readData)
  {
    return;
  }

  if (readImageData(reader) != HK_SUCCESS)
  {
    clearData();
  }
}


hkvImageFileFormat hkvImageFileTga::getFileFormat() const
{
  return HKV_IMAGE_FILE_FORMAT_TGA;
}


hkUint32 hkvImageFileTga::getWidth() const
{
  return m_headerValid ? m_header.width : 0;
}


hkUint32 hkvImageFileTga::getHeight() const
{
  return m_headerValid ? m_header.height : 0;
}


bool hkvImageFileTga::hasAlpha() const
{
  return m_headerValid ? m_header.bits == 32 : false;
}


hkResult hkvImageFileTga::readHeader(hkStreamReader& reader)
{
  hkIArchive archive(&reader, hkBool(HK_ENDIAN_BIG));

  archive.readArray8u(&m_header.identSize, 3);
  //hkUint8 identSize;
  //hkUint8 colorrMapType;
  //hkUint8 imageType;
  archive.readArray16u(&m_header.colorMapStart, 2);
  //hkUint16 colorMapStart;
  //hkUint16 colorMapLength;
  m_header.colorMapBits = archive.read8u();
  archive.readArray16u(&m_header.xStart, 4);
  //hkUint16 xStart;
  //hkUint16 yStart;
  //hkUint16 width;
  //hkUint16 height;
  archive.readArray8u(&m_header.bits, 2);
  //hkUint8 bits;
  //hkUint8 descriptor;

  if (!archive.isOk())
  {
    return HK_FAILURE;
  }

  // Check for plausible size
  if ((m_header.width < 1) || (m_header.height < 1))
  {
    return HK_FAILURE;
  }

  // Check for bit depth and supported image types
  switch (m_header.bits)
  {
  case 8:
    {
      // Image type must be grayscale
      if ((m_header.imageType != 3) && (m_header.imageType != 11)) // 3: grayscale; 11: grayscale RLE
      {
        return HK_FAILURE;
      }
      break;
    }
  case 24:
  case 32:
    {
      // Image type must be truecolor
      if ((m_header.imageType != 2) && (m_header.imageType != 10)) // 2: truecolor; 10: truecolor RLE
      {
        return HK_FAILURE;
      }
      break;
    }
  default:
    {
      // Unsupported bit depth
      return HK_FAILURE;
    }
  }

  m_headerValid = true;
  return HK_SUCCESS;
}


hkResult hkvImageFileTga::readImageData(hkStreamReader& reader)
{
  if (!isHeaderValid())
  {
    return HK_FAILURE;
  }

  hkvMipImage* img = new hkvMipImage(m_header.width, m_header.height);
  if (img == NULL || img->getData() == NULL)
  {
    delete img;
    return HK_FAILURE;
  }

  m_mipImages.pushBack(img);
  hkUint8* data = img->getData();

  switch (m_header.imageType)
  {
  case 2:
  case 3:
    {
      // Uncompressed data
      int toRead = m_header.width * m_header.height * m_header.bits / 8;
      if (reader.read(data, toRead) != toRead)
      {
        return HK_FAILURE;
      }
      break;
    }
  case 10:
  case 11:
    {
      // RLE-compressed data
      if (readRleData(reader) != HK_SUCCESS)
      {
        return HK_FAILURE;
      }
      break;
    }
  }

  postprocessData();

  return HK_SUCCESS;
}


hkResult hkvImageFileTga::readRleData(hkStreamReader& reader)
{
  hkUint32 numPixels = m_header.width * m_header.height;
  hkUint32 bytesPerPixel = m_header.bits / 8;

  hkUint32 pixelIdx = 0;

  hkUint8* data = getData();

  while (reader.isOk() && (pixelIdx < numPixels))
  {
    hkUint8 chunkInfo = 0;
    if (reader.read(&chunkInfo, 1) != 1)
    {
      return HK_FAILURE;
    }

    if (chunkInfo < 128)
    {
      // Raw values follow: chunkInfo + 1 values
      hkUint32 numRawValues = hkMath::min2((hkUint32)chunkInfo + 1, numPixels - pixelIdx);
      int toRead = numRawValues * bytesPerPixel;
      if (reader.read(&data[pixelIdx * bytesPerPixel], toRead) != toRead)
      {
        return HK_FAILURE;
      }
      pixelIdx += numRawValues;
    }
    else
    {
      // RLE: next value is repeated n times
      hkUint32 numRepeats = hkMath::min2((hkUint32)chunkInfo - 127, numPixels - pixelIdx);
      hkUint8 rlePixel[4];
      if (reader.read(rlePixel, bytesPerPixel) != bytesPerPixel)
      {
        return HK_FAILURE;
      }

      for (hkUint32 repeatIdx = 0; repeatIdx < numRepeats; ++repeatIdx)
      {
        for (hkUint8 byteIdx = 0; byteIdx < bytesPerPixel; ++byteIdx)
        {
          data[((pixelIdx + repeatIdx) * bytesPerPixel) + byteIdx] = rlePixel[byteIdx];
        }
      }
      pixelIdx += numRepeats;
    }
  }

  return HK_SUCCESS;
}


void hkvImageFileTga::postprocessData()
{
  hkUint32 numPixels = m_header.width * m_header.height;
  hkUint32 bytesPerPixel = m_header.bits / 8;
  hkUint32 srcOffset = (numPixels - 1) * bytesPerPixel;
  hkUint32 destOffset = (numPixels - 1) * 4;

  hkUint8* data = getData();

  hkUint8 srcData[4];
  for (hkInt32 pixelIdx = numPixels - 1; pixelIdx >= 0; --pixelIdx)
  {
    for (hkUint8 byteIdx = 0; byteIdx < bytesPerPixel; ++byteIdx)
    {
      srcData[byteIdx] = data[srcOffset + byteIdx];
    }
    
    switch (bytesPerPixel)
    {
    case 1:
      {
        data[destOffset    ] = data[destOffset + 1] = data[destOffset + 2] = srcData[0];
        data[destOffset + 3] = 255;
        break;
      }
    case 3:
      {
        data[destOffset    ] = srcData[0];
        data[destOffset + 1] = srcData[1];
        data[destOffset + 2] = srcData[2];
        data[destOffset + 3] = 255;
        break;
      }
    case 4:
      {
        data[destOffset    ] = srcData[0];
        data[destOffset + 1] = srcData[1];
        data[destOffset + 2] = srcData[2];
        data[destOffset + 3] = srcData[3];
        break;
      }
    }

    srcOffset -= bytesPerPixel;
    destOffset -= 4;
  }
}


//-----------------------------------------------------------------------------
// hkvImageFile
//-----------------------------------------------------------------------------
hkvImageFile::hkvImageFile()
: m_headerValid(false)
{
}


hkvImageFile::~hkvImageFile()
{
  clearData();
}


hkvImageFile::RefPtr hkvImageFile::open(const char* fileName, bool readData)
{
  if (fileName == NULL)
  {
    return NULL;
  }

  // Extract the file extension, and determine the image file type from it
  int lastPeriod = hkString::lastIndexOf(fileName, '.');
  if (lastPeriod < 0)
  {
    return NULL;
  }
  const char* extension = fileName + lastPeriod + 1;

  hkvImageFileFormat format = HKV_IMAGE_FILE_FORMAT_INVALID;
  /*
  if (hkString::strCasecmp(extension, "bmp") == 0)
  {
    format = HKV_IMAGE_FILE_FORMAT_BMP;
  }
  else*/ if (hkString::strCasecmp(extension, "dds") == 0)
  {
    format = HKV_IMAGE_FILE_FORMAT_DDS;
  }
  /*else if (hkString::strCasecmp(extension, "jpg") == 0) 
  {
    format = HKV_IMAGE_FILE_FORMAT_JPG;
  }
  else if (hkString::strCasecmp(extension, "png") == 0) 
  {
    format = HKV_IMAGE_FILE_FORMAT_PNG;
  }*/
  else if (hkString::strCasecmp(extension, "tga") == 0) 
  {
    format = HKV_IMAGE_FILE_FORMAT_TGA;
  }

  if (format == HKV_IMAGE_FILE_FORMAT_INVALID)
  {
    return NULL;
  }

  // If the file type is recognized, open a stream of its contents and
  // open the image with the appropriate reader class
  hkRefPtr<hkStreamReader> reader(hkFileSystem::getInstance().openReader(fileName));
  if (reader == NULL || !reader->isOk())
  {
    return NULL;
  }
  hkBufferedStreamReader bufferedReader(reader);

  return open(bufferedReader, format, readData);
}


hkvImageFile::RefPtr hkvImageFile::open(hkStreamReader& reader, hkvImageFileFormat format, bool readData)
{
  switch (format)
  {
  case HKV_IMAGE_FILE_FORMAT_DDS:
    {
      return hkvImageFile::RefNew(new hkvImageFileDds(reader, readData));
    }
  case HKV_IMAGE_FILE_FORMAT_TGA:
    {
      return hkvImageFile::RefNew(new hkvImageFileTga(reader, readData));
    }
  default:
    {
      VASSERT_MSG(FALSE, "A valid image file format must be specified!");
      return NULL;
    }
  }
}


hkvImageFileFormat hkvImageFile::guessFormatFromFileName(const char* fileName)
{
  if (fileName == NULL)
  {
    return HKV_IMAGE_FILE_FORMAT_INVALID;
  }

  // Extract the file extension, and determine the image file type from it
  int lastPeriod = hkString::lastIndexOf(fileName, '.');
  if (lastPeriod < 0)
  {
    return HKV_IMAGE_FILE_FORMAT_INVALID;
  }
  const char* extension = fileName + lastPeriod + 1;

  hkvImageFileFormat format = HKV_IMAGE_FILE_FORMAT_INVALID;
  if (hkString::strCasecmp(extension, "bmp") == 0)
  {
    format = HKV_IMAGE_FILE_FORMAT_BMP;
  }
  else if (hkString::strCasecmp(extension, "dds") == 0)
  {
    format = HKV_IMAGE_FILE_FORMAT_DDS;
  }
  else if (hkString::strCasecmp(extension, "jpg") == 0) 
  {
    format = HKV_IMAGE_FILE_FORMAT_JPG;
  }
  else if (hkString::strCasecmp(extension, "png") == 0) 
  {
    format = HKV_IMAGE_FILE_FORMAT_PNG;
  }
  else if (hkString::strCasecmp(extension, "tga") == 0) 
  {
    format = HKV_IMAGE_FILE_FORMAT_TGA;
  }

  return format;
}

void hkvImageFile::clearData()
{
  const int numMips = m_mipImages.getSize();
  for (int mipIdx = 0; mipIdx < numMips; ++mipIdx)
  {
    delete m_mipImages[mipIdx];
  }
  m_mipImages.clear();
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
