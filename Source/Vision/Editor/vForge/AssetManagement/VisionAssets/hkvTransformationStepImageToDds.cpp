/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvDds.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvImageFile.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationSettings.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTransformationStepImageToDds.hpp>

#include <Common/Base/System/Io/OArchive/hkOArchive.h>

hkvTransformationStepImageToDds::hkvTransformationStepImageToDds(const hkvTextureTransformationSettings& settings,
  const char* sourceFile, const char* targetFile)
: hkvFileTransformationStep(sourceFile, targetFile), m_rgbaBits(0), m_rWidth(0), m_rShift(0), m_gWidth(0), m_gShift(0),
  m_bWidth(0), m_bShift(0), m_aWidth(0), m_aShift(0)
{
  switch (settings.getTargetDataFormat())
  {
  case HKV_TEXTURE_DATA_FORMAT_A4R4G4B4:
    {
      m_rgbaBits = 16;
      m_rWidth = m_gWidth = m_bWidth = m_aWidth = 4;
      m_rShift = 8;
      m_gShift = 4;
      m_bShift = 0;
      m_aShift = 12;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL:
    {
      m_rgbaBits = 16;
      m_rWidth = m_gWidth = m_bWidth = m_aWidth = 4;
      m_rShift = 12;
      m_gShift = 8;
      m_bShift = 4;
      m_aShift = 0;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_A1R5G5B5:
    {
      m_rgbaBits = 16;
      m_rWidth = 5;
      m_gWidth = 5;
      m_bWidth = 5;
      m_aWidth = 1;
      m_rShift = 10;
      m_gShift = 5;
      m_bShift = 0;
      m_aShift = 15;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_R5G6B5:
    {
      m_rgbaBits = 16;
      m_rWidth = 5;
      m_gWidth = 6;
      m_bWidth = 5;
      m_aWidth = 0;
      m_rShift = 11;
      m_gShift = 5;
      m_bShift = 0;
      m_aShift = 0;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_A8R8G8B8:
    {
      m_rgbaBits = 32;
      m_rWidth = m_gWidth = m_bWidth = m_aWidth = 8;
      m_rShift = 16;
      m_gShift = 8;
      m_bShift = 0;
      m_aShift = 24;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_X8R8G8B8:
    {
      m_rgbaBits = 32;
      m_rWidth = m_gWidth = m_bWidth = 8;
      m_aWidth = 0;
      m_rShift = 16;
      m_gShift = 8;
      m_bShift = 0;
      m_aShift = 0;
      break;
    }
  case HKV_TEXTURE_DATA_FORMAT_R8G8B8:
    {
      m_rgbaBits = 24;
      m_rWidth = m_gWidth = m_bWidth = 8;
      m_aWidth = 0;
      m_rShift = 16;
      m_gShift = 8;
      m_bShift = 0;
      m_aShift = 0;
      break;
    }
  }
}


hkResult hkvTransformationStepImageToDds::writeImageBlock(hkStreamWriter& writer, const DDS_HEADER& ddsHeader, hkvMipImage* image)
{
  hkArray<hkUint8> scanLine;

  const hkUint32 width = image->getWidth();
  const hkUint32 height = image->getHeight();

  const DWORD stride = ddsHeader.ddspf.dwRGBBitCount / 8;
  scanLine.setSize(width * stride);

  for (DWORD y = 0; y < height; ++y)
  {
    const hkUint8* imageData = image->getData() + ((height - y - 1) * width * 4);

    int scanLinePos = 0;
    for (DWORD x = 0; x < width; ++x)
    {
      DWORD b = *imageData++;
      DWORD g = *imageData++;
      DWORD r = *imageData++;
      DWORD a = *imageData++;

      b = (b >> (8 - m_bWidth)) << m_bShift;
      g = (g >> (8 - m_gWidth)) << m_gShift;
      r = (r >> (8 - m_rWidth)) << m_rShift;
      a = (a >> (8 - m_aWidth)) << m_aShift;

      DWORD val = b | g | r | a;

      for (DWORD part = 0; part < stride; ++part)
      {
        scanLine[scanLinePos++] = (hkUint8)(val & 0xff);
        val >>= 8;
      }
    }

    if (writer.write(scanLine.begin(), scanLine.getSize()) != scanLine.getSize())
    {
      return HK_FAILURE;
    }
  }

  return HK_SUCCESS;
}


hkResult hkvTransformationStepImageToDds::run()
{
  hkvImageFile::RefPtr image = hkvImageFile::open(getSourceFile(), true);
  if ((image == NULL) || !image->isDataValid())
  {
    addMessage(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_ERROR, "Source image is not valid."));
    return HK_FAILURE;
  }

  if (m_rgbaBits == 0 || (m_rgbaBits % 8) != 0)
  {
    addMessage(hkvTransformationMessage(HKV_MESSAGE_SEVERITY_ERROR, "Target format is not valid."));
    return HK_FAILURE;
  }

  DDS_HEADER ddsHeader;
  memset(&ddsHeader, 0, sizeof(DDS_HEADER));
  ddsHeader.dwSize = sizeof(DDS_HEADER);
  ddsHeader.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
  ddsHeader.dwHeight = image->getHeight();
  ddsHeader.dwWidth = image->getWidth();
  ddsHeader.dwPitchOrLinearSize = image->getWidth() * (m_rgbaBits / 8);
  if (image->getMipImageCount() > 1)
  {
    ddsHeader.dwFlags |= DDSD_MIPMAPCOUNT;
    ddsHeader.dwMipMapCount = image->getMipImageCount();
  }

  ddsHeader.ddspf.dwSize = sizeof(DDS_PIXELFORMAT);
  ddsHeader.ddspf.dwFlags = DDPF_RGB;
  if (m_aWidth > 0)
    ddsHeader.ddspf.dwFlags |= DDPF_ALPHAPIXELS;
  ddsHeader.ddspf.dwRGBBitCount = m_rgbaBits;

  ddsHeader.ddspf.dwRBitMask = makeBitMask(m_rWidth, m_rShift);
  ddsHeader.ddspf.dwGBitMask = makeBitMask(m_gWidth, m_gShift);
  ddsHeader.ddspf.dwBBitMask = makeBitMask(m_bWidth, m_bShift);
  ddsHeader.ddspf.dwABitMask = makeBitMask(m_aWidth, m_aShift);

  ddsHeader.dwCaps = DDSCAPS_TEXTURE;

  hkRefPtr<hkStreamWriter> writer = hkRefNew<hkStreamWriter>(hkFileSystem::getInstance().openWriter(getTargetFile()));
  if ((writer == NULL) || !writer->isOk())
    return HK_FAILURE;

  {
    hkOArchive archive(writer, hkBool(HK_ENDIAN_BIG));

    archive.write32u(DDS_MAGIC);
    archive.writeArray32u((hkUint32*)&ddsHeader, sizeof(ddsHeader) / sizeof(DWORD));
    if (!archive.isOk())
      return HK_FAILURE;
  }

  const hkUint32 numMips = image->getMipImageCount();
  for (hkUint32 mipIdx = 0; mipIdx < numMips; ++mipIdx)
  {
    if (writeImageBlock(*writer, ddsHeader, image->getMipImage(mipIdx)) != HK_SUCCESS)
    {
      return HK_FAILURE;
    }
  }

  return HK_SUCCESS;
}


void hkvTransformationStepImageToDds::cancel()
{
  // Not implemented yet.
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
