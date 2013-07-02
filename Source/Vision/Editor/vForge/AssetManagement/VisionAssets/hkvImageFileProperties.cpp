/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvImageFileProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvDds.hpp>

#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/System/Io/IArchive/hkIArchive.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>


#define BMP_TYPE_BITMAP 0x4d42


#pragma pack(push, 1)
struct hkvPngHeader
{
  hkUint32 magic;
  hkUint16 crlf;
  hkUint16 byteOrder;
  hkUint32 ihdrLen;
  hkUint32 ihdrId;
  hkUint32 ihdrWidth;
  hkUint32 ihdrHeight;
  hkUint8  ihdrBitDepth;
  hkUint8  ihdrColorType;
  hkUint8  ihdrCompressionMethod;
  hkUint8  ihdrFilterMethod;
  hkUint8  ihdrInterlaceMethod;
};
#pragma pack(pop)

#define PNG_MAGIC 0x89504e47u
#define PNG_IHDR_ID 0x49484452u


hkvImageFileProperties::hkvImageFileProperties()
: m_imageFormatInstance(getImageFileFormatDefinition())
{
  clear();
}


hkvImageFileProperties::hkvImageFileProperties(const hkvImageFileProperties& rhs)
: m_imageFormatInstance(getImageFileFormatDefinition())
{
  *this = rhs;
}


hkvImageFileProperties& hkvImageFileProperties::operator=(const hkvImageFileProperties& rhs)
{
  m_imageFormatInstance.setByAvailableElementsId(rhs.m_imageFormatInstance.getAvailableElementsId());
  m_hasAlpha = rhs.m_hasAlpha;
  m_width = rhs.m_width;
  m_height = rhs.m_height;

  return *this;
}


const char* hkvImageFileProperties::getTypeName() const
{
  return "ImageFileProperties";
}


void hkvImageFileProperties::getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  int flags = hkvProperty::FLAG_READ_ONLY;

  properties.reserve(properties.size() + 5);

  bool valid = m_imageFormatInstance.getDefinitionId() != HKV_IMAGE_FILE_FORMAT_INVALID;

  if (purpose != hkvProperty::PURPOSE_SERIALIZATION)
  {
    properties.push_back(hkvProperty("Valid", valid, flags | hkvProperty::FLAG_READ_ONLY, "If an image is not valid, either the image file is damaged or the file format is not supported. Check the file and save it with a different format."));
  }

  if (valid)
  {
    properties.push_back(hkvProperty("FileFormat", m_imageFormatInstance.getString(), hkvProperty::TYPE_STRING, flags | hkvProperty::FLAG_READ_ONLY, "Which file format the source texture uses."));
    properties.push_back(hkvProperty("Width", m_width, flags | hkvProperty::FLAG_READ_ONLY, "The width of the texture in pixels."));
    properties.push_back(hkvProperty("Height", m_height, flags | hkvProperty::FLAG_READ_ONLY, "The height of the texture in pixels."));
    properties.push_back(hkvProperty("HasAlpha", m_hasAlpha, flags | hkvProperty::FLAG_READ_ONLY, "Whether the texture contains an Alpha-channel."));
  }
}


void hkvImageFileProperties::setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose)
{
  int stackSize = (path.getSize() - stackIndex);
  
  if ((stackSize != 0))
    return;

  if (hkvStringHelper::safeCompare(prop.getName(), "FileFormat") == 0)
  {
    if (m_imageFormatInstance.setByString(prop.getString()) != HK_SUCCESS)
    {
      m_imageFormatInstance.setByDefinitionId(HKV_IMAGE_FILE_FORMAT_INVALID);
    }
  }
  else if (hkvStringHelper::safeCompare(prop.getName(), "Width") == 0)
  {
    m_width = prop.getUint();
  }
  else if (hkvStringHelper::safeCompare(prop.getName(), "Height") == 0)
  {
    m_height = prop.getUint();
  }
  else if (hkvStringHelper::safeCompare(prop.getName(), "HasAlpha") == 0)
  {
    m_hasAlpha = prop.getBool();
  }
}


void hkvImageFileProperties::clear()
{
  m_imageFormatInstance.setByDefinitionId(HKV_IMAGE_FILE_FORMAT_INVALID);
  m_width = 0;
  m_height = 0;
  m_hasAlpha = false;
}


hkResult hkvImageFileProperties::examineFile(const char* fileName)
{
  clear();

  hkvImageFileFormat format = hkvImageFile::guessFormatFromFileName(fileName);
  if (format == HKV_IMAGE_FILE_FORMAT_INVALID)
  {
    return HK_FAILURE;
  }

  // If the file type is recognized, open a stream of its contents and
  // examine the image properties
  hkRefPtr<hkStreamReader> reader(hkFileSystem::getInstance().openReader(fileName));
  
  if (reader == NULL || !reader->isOk())
  {
    return HK_FAILURE;
  }

  hkBufferedStreamReader bufferedReader(reader);
  return examineStream(bufferedReader, format);
}


hkResult hkvImageFileProperties::examineStream(hkStreamReader& reader, hkvImageFileFormat format)
{
  VASSERT_MSG((format > HKV_IMAGE_FILE_FORMAT_INVALID) && (format < HKV_IMAGE_FILE_FORMAT_COUNT), "A valid image file format must be specified!");
  
  hkResult examineRes = HK_FAILURE;
  switch (format)
  {
  case HKV_IMAGE_FILE_FORMAT_BMP:
    {
      examineRes = examineBmp(reader);
      break;
    }
  case HKV_IMAGE_FILE_FORMAT_DDS:
    {
      examineRes = examineDds(reader);
      break;
    }
  case HKV_IMAGE_FILE_FORMAT_JPG:
    {
      examineRes = examineJpg(reader);
      break;
    }
  case HKV_IMAGE_FILE_FORMAT_PNG:
    {
      examineRes = examinePng(reader);
      break;
    }
  case HKV_IMAGE_FILE_FORMAT_TGA:
    {
      examineRes = examineTga(reader);
      break;
    }
  }

  if (examineRes == HK_SUCCESS)
  {
    m_imageFormatInstance.setByDefinitionId(format);
  }
  else
  {
    clear();
  }

  return examineRes;
}


hkvImageFileFormat hkvImageFileProperties::getImageFileFormat() const
{
  return (hkvImageFileFormat)m_imageFormatInstance.getDefinitionId();
}


hkUint32 hkvImageFileProperties::getWidth() const
{
  return m_width;
}


hkUint32 hkvImageFileProperties::getHeight() const
{
  return m_height;
}


hkBool hkvImageFileProperties::hasAlpha() const
{
  return m_hasAlpha;
}


hkResult hkvImageFileProperties::examineBmp(hkStreamReader& reader)
{
  hkIArchive archive(&reader, hkBool(HK_ENDIAN_BIG));

  BITMAPFILEHEADER fileHeader;
  fileHeader.bfType = archive.read16u();
  fileHeader.bfSize = archive.read32u();
  archive.readArray16u(&fileHeader.bfReserved1, 2);
  //WORD    bfReserved1;
  //WORD    bfReserved2;
  fileHeader.bfOffBits = archive.read32u();

  BITMAPINFOHEADER bitmapHeader;
  archive.readArray32u(reinterpret_cast<hkUint32*>(&bitmapHeader.biSize), 3);
  //DWORD      biSize;
  //LONG       biWidth;
  //LONG       biHeight;
  archive.readArray16u(&bitmapHeader.biPlanes, 2);
  //WORD       biPlanes;
  //WORD       biBitCount;
  archive.readArray32u(reinterpret_cast<hkUint32*>(&bitmapHeader.biCompression), 6);
  //DWORD      biCompression;
  //DWORD      biSizeImage;
  //LONG       biXPelsPerMeter;
  //LONG       biYPelsPerMeter;
  //DWORD      biClrUsed;
  //DWORD      biClrImportant;

  if (!archive.isOk() || fileHeader.bfType != BMP_TYPE_BITMAP)
  {
    return HK_FAILURE;
  }

  m_width = bitmapHeader.biWidth;
  m_height = bitmapHeader.biHeight;

  // The following is only a heuristic, and does not cover some special cases. However,
  // we assume that every format below 32bit has no alpha channel, and 32bit always
  // has one.
  m_hasAlpha = (bitmapHeader.biBitCount == 32);

  return HK_SUCCESS;
}


hkResult hkvImageFileProperties::examineDds(hkStreamReader& reader)
{
  hkvImageFile::RefPtr imageFile = hkvImageFile::open(reader, HKV_IMAGE_FILE_FORMAT_DDS, false);
  if ((imageFile == NULL) || !imageFile->isHeaderValid())
  {
    return HK_FAILURE;
  }

  m_width = imageFile->getWidth();
  m_height = imageFile->getHeight();
  m_hasAlpha = imageFile->hasAlpha();

  return HK_SUCCESS;
}


hkResult hkvImageFileProperties::examineJpg(hkStreamReader& reader)
{
  hkIArchive archive(&reader, hkBool(HK_ENDIAN_LITTLE));
  
  hkUint16 soiMark = archive.read16u();
  if (!archive.isOk() || soiMark != 0xffd8)
  {
    return HK_FAILURE;
  }

  // Search for the first frame start marker
  bool hasFf = false;
  while (true)
  {
    hkUint8 byte = archive.read8u();
    if (!archive.isOk())
    {
      return HK_FAILURE;
    }
    
    if (byte == 0xff)
    {
      hasFf = true;
      continue;
    }

    if (hasFf && (byte == 0xc0 || byte == 0xc1))
    {
      // We found the frame start marker
      break;
    }

    hasFf = false;
  }
  
  // Skip to the two size values, and read them
  archive.getStreamReader()->skip(3);
  hkUint16 height = archive.read16u();
  hkUint16 width = archive.read16u();
  if (!archive.isOk())
  {
    return HK_FAILURE;
  }

  m_height = height;
  m_width = width;

  return HK_SUCCESS;
}


hkResult hkvImageFileProperties::examinePng(hkStreamReader& reader)
{
  hkIArchive archive(&reader, hkBool(HK_ENDIAN_LITTLE));

  hkvPngHeader pngHeader;
  pngHeader.magic = archive.read32u();
  archive.readArray16u(&pngHeader.crlf, 2);
  //hkUint16 crlf;
  //hkUint16 byteOrder;
  archive.readArray32u(&pngHeader.ihdrLen, 4);
  //hkUint32 ihdrLen;
  //hkUint32 ihdrId;
  //hkUint32 ihdrWidth;
  //hkUint32 ihdrHeight;
  archive.readArray8u(&pngHeader.ihdrBitDepth, 5);
  //hkUint8  ihdrBitDepth;
  //hkUint8  ihdrColorType;
  //hkUint8  ihdrCompressionMethod;
  //hkUint8  ihdrFilterMethod;
  //hkUint8  ihdrInterlaceMethod;

  if (!archive.isOk() || pngHeader.magic != PNG_MAGIC || pngHeader.ihdrId != PNG_IHDR_ID)
  {
    return HK_FAILURE;
  }

  m_width = pngHeader.ihdrWidth;
  m_height = pngHeader.ihdrHeight;

  m_hasAlpha = (pngHeader.ihdrColorType == 4 || pngHeader.ihdrColorType == 6);

  return HK_SUCCESS;
}


hkResult hkvImageFileProperties::examineTga(hkStreamReader& reader)
{
  hkvImageFile::RefPtr imageFile = hkvImageFile::open(reader, HKV_IMAGE_FILE_FORMAT_TGA, false);
  if ((imageFile == NULL) || !imageFile->isHeaderValid())
  {
    return HK_FAILURE;
  }

  m_width = imageFile->getWidth();
  m_height = imageFile->getHeight();
  m_hasAlpha = imageFile->hasAlpha();

  return HK_SUCCESS;
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
