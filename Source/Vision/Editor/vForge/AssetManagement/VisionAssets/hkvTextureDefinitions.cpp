/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>

const char* hkvTextureDataFormatNames[] = 
{
  "PVRTC2",
  "PVRTC4",

  "ETC1",

  "DXT1",
  "DXT3",
  "DXT5",

  "A4R4G4B4",
  "R4G4B4A4_GL",
  "A1R5G5B5",
  "R5G6B5",
  "A8R8G8B8",
  "X8R8G8B8",
  "R8G8B8"
};
HK_COMPILE_TIME_ASSERT(sizeof(hkvTextureDataFormatNames) / sizeof(char*) == HKV_TEXTURE_DATA_FORMAT_COUNT);


const char* hkvTextureFileFormatNames[] =
{
  "DDS",
  "RGBA (DDS)",
  "ETC",
  "PVR",
  "GTX"
};
HK_COMPILE_TIME_ASSERT(sizeof(hkvTextureFileFormatNames) / sizeof(char*) == HKV_TEXTURE_FILE_FORMAT_COUNT);


const char* hkvTextureFileFormatExtensions[] =
{
  "dds",
  "rgba",
  "etc",
  "pvr",
  "gtx"
};
HK_COMPILE_TIME_ASSERT(sizeof(hkvTextureFileFormatExtensions) / sizeof(char*) == HKV_TEXTURE_FILE_FORMAT_COUNT);


const char* hkvTextureUsageNames[] =
{
  "Undefined",
  "DiffuseMap",
  "NormalMap"/*,
  "SpecularMap"*/
};
HK_COMPILE_TIME_ASSERT(sizeof(hkvTextureUsageNames) / sizeof(char*) == HKV_TEXTURE_USAGE_COUNT);


const char* hkvTextureCompressionNames[] = 
{
  "Uncompressed",
  "Quality"/*,
  "Size"*/
};
HK_COMPILE_TIME_ASSERT(sizeof(hkvTextureCompressionNames) / sizeof(char*) == HKV_TEXTURE_COMPRESSION_COUNT);


HK_SINGLETON_IMPLEMENTATION(hkvPlatformToTextureDataFormatMapping);

hkvPlatformToTextureDataFormatMapping::hkvPlatformToTextureDataFormatMapping()
: hkReferencedObject(), hkSingleton(), hkvPlatformToTextureDataFormatMappingBase()
{
  hkvTextureDataFormat rawFormats[] = {
    HKV_TEXTURE_DATA_FORMAT_A4R4G4B4,
    HKV_TEXTURE_DATA_FORMAT_A1R5G5B5,
    HKV_TEXTURE_DATA_FORMAT_R5G6B5,
    HKV_TEXTURE_DATA_FORMAT_A8R8G8B8,
    HKV_TEXTURE_DATA_FORMAT_X8R8G8B8,
    HKV_TEXTURE_DATA_FORMAT_R8G8B8
  };
  addMappings(HKV_TARGET_PLATFORM_DX9, rawFormats);
  addMappings(HKV_TARGET_PLATFORM_DX11, rawFormats);
  addMappings(HKV_TARGET_PLATFORM_XBOX360, rawFormats);
  addMappings(HKV_TARGET_PLATFORM_PS3, rawFormats);
  addMappings(HKV_TARGET_PLATFORM_PSVITA, rawFormats);
  addMappings(HKV_TARGET_PLATFORM_IOS, rawFormats);
  addMappings(HKV_TARGET_PLATFORM_ANDROID, rawFormats);
  addMappings(HKV_TARGET_PLATFORM_WIIU, rawFormats);

  addMapping(HKV_TARGET_PLATFORM_ANDROID, HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL);
  addMapping(HKV_TARGET_PLATFORM_IOS, HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL);

  hkvTextureDataFormat bcFormats[] = {
    HKV_TEXTURE_DATA_FORMAT_DXT1,
    HKV_TEXTURE_DATA_FORMAT_DXT3,
    HKV_TEXTURE_DATA_FORMAT_DXT5,
  };
  addMappings(HKV_TARGET_PLATFORM_DX9, bcFormats);
  addMappings(HKV_TARGET_PLATFORM_DX11, bcFormats);
  addMappings(HKV_TARGET_PLATFORM_XBOX360, bcFormats);
  addMappings(HKV_TARGET_PLATFORM_PS3, bcFormats);
  addMappings(HKV_TARGET_PLATFORM_PSVITA, bcFormats);
  addMappings(HKV_TARGET_PLATFORM_WIIU, bcFormats);

  hkvTextureDataFormat pvrFormats[] = {
    HKV_TEXTURE_DATA_FORMAT_PVRTC2,
    HKV_TEXTURE_DATA_FORMAT_PVRTC4,
  };
  addMappings(HKV_TARGET_PLATFORM_IOS, pvrFormats);
  addMappings(HKV_TARGET_PLATFORM_ANDROID, pvrFormats);

  hkvTextureDataFormat etcFormats[] = {
    HKV_TEXTURE_DATA_FORMAT_ETC1,
  };
  addMappings(HKV_TARGET_PLATFORM_ANDROID, etcFormats);
}


HK_SINGLETON_IMPLEMENTATION(hkvPlatformToTextureFileFormatMapping);

hkvPlatformToTextureFileFormatMapping::hkvPlatformToTextureFileFormatMapping()
{
  //DDS
  addMapping(HKV_TARGET_PLATFORM_DX9, HKV_TEXTURE_FILE_FORMAT_DDS);
  addMapping(HKV_TARGET_PLATFORM_DX11, HKV_TEXTURE_FILE_FORMAT_DDS);
  addMapping(HKV_TARGET_PLATFORM_XBOX360, HKV_TEXTURE_FILE_FORMAT_DDS);
  addMapping(HKV_TARGET_PLATFORM_PS3, HKV_TEXTURE_FILE_FORMAT_DDS);
  addMapping(HKV_TARGET_PLATFORM_PSVITA, HKV_TEXTURE_FILE_FORMAT_DDS);
  addMapping(HKV_TARGET_PLATFORM_ANDROID, HKV_TEXTURE_FILE_FORMAT_DDS);
  addMapping(HKV_TARGET_PLATFORM_IOS, HKV_TEXTURE_FILE_FORMAT_DDS);

  //RGBA
  addMapping(HKV_TARGET_PLATFORM_ANDROID, HKV_TEXTURE_FILE_FORMAT_RGBA);
  addMapping(HKV_TARGET_PLATFORM_IOS, HKV_TEXTURE_FILE_FORMAT_RGBA);

  //PVR
  addMapping(HKV_TARGET_PLATFORM_IOS, HKV_TEXTURE_FILE_FORMAT_PVR);
  addMapping(HKV_TARGET_PLATFORM_ANDROID, HKV_TEXTURE_FILE_FORMAT_PVR);

  //ETC
  addMapping(HKV_TARGET_PLATFORM_ANDROID, HKV_TEXTURE_FILE_FORMAT_ETC);

  //GTX
  addMapping(HKV_TARGET_PLATFORM_WIIU, HKV_TEXTURE_FILE_FORMAT_GTX);
}


HK_SINGLETON_IMPLEMENTATION(hkvTextureFileToDataFormatMapping);

hkvTextureFileToDataFormatMapping::hkvTextureFileToDataFormatMapping()
{
  hkvTextureDataFormat rawFormats[] = {
    HKV_TEXTURE_DATA_FORMAT_A4R4G4B4,
    HKV_TEXTURE_DATA_FORMAT_A1R5G5B5,
    HKV_TEXTURE_DATA_FORMAT_R5G6B5,
    HKV_TEXTURE_DATA_FORMAT_A8R8G8B8,
    HKV_TEXTURE_DATA_FORMAT_X8R8G8B8,
    HKV_TEXTURE_DATA_FORMAT_R8G8B8
  };
  addMappings(HKV_TEXTURE_FILE_FORMAT_DDS, rawFormats);
  addMappings(HKV_TEXTURE_FILE_FORMAT_RGBA, rawFormats);
  addMappings(HKV_TEXTURE_FILE_FORMAT_GTX, rawFormats);

  addMapping(HKV_TEXTURE_FILE_FORMAT_RGBA, HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL);

  hkvTextureDataFormat bcFormats[] = {
    HKV_TEXTURE_DATA_FORMAT_DXT1,
    HKV_TEXTURE_DATA_FORMAT_DXT3,
    HKV_TEXTURE_DATA_FORMAT_DXT5,
  };
  addMappings(HKV_TEXTURE_FILE_FORMAT_DDS, bcFormats);
  addMappings(HKV_TEXTURE_FILE_FORMAT_GTX, bcFormats);

  hkvTextureDataFormat pvrFormats[] = {
    HKV_TEXTURE_DATA_FORMAT_PVRTC2,
    HKV_TEXTURE_DATA_FORMAT_PVRTC4,
  };
  addMappings(HKV_TEXTURE_FILE_FORMAT_PVR, pvrFormats);

  hkvTextureDataFormat etcFormats[] = {
    HKV_TEXTURE_DATA_FORMAT_ETC1,
  };
  addMappings(HKV_TEXTURE_FILE_FORMAT_ETC, etcFormats);
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
