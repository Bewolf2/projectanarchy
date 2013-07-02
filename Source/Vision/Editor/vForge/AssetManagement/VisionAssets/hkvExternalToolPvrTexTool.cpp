/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvExternalToolPvrTexTool.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationSettings.hpp>

//TODO: Add ETC2 and PVRTC2 support
const hkvConversionToolFormatString hkvPvrTexToolFormatStrings[] =
{
  {HKV_TEXTURE_DATA_FORMAT_PVRTC2, "PVRTC1_2"},
  {HKV_TEXTURE_DATA_FORMAT_PVRTC4, "PVRTC1_4"},

  {HKV_TEXTURE_DATA_FORMAT_ETC1, "ETC1"},

  {HKV_TEXTURE_DATA_FORMAT_DXT1, "BC1"},
  {HKV_TEXTURE_DATA_FORMAT_DXT3, "BC2"},
  {HKV_TEXTURE_DATA_FORMAT_DXT5, "BC3"},

  {HKV_TEXTURE_DATA_FORMAT_A4R4G4B4, "a4r4g4b4"},
  {HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL, "r4g4b4a4"},
  {HKV_TEXTURE_DATA_FORMAT_A1R5G5B5, "a1r5g5b5"},
  {HKV_TEXTURE_DATA_FORMAT_R5G6B5, "r5g6b5"},
  {HKV_TEXTURE_DATA_FORMAT_A8R8G8B8, "a8r8g8b8"},
  {HKV_TEXTURE_DATA_FORMAT_X8R8G8B8, "x8r8g8b8"},
  {HKV_TEXTURE_DATA_FORMAT_R8G8B8, "r8g8b8"},
};
HK_COMPILE_TIME_ASSERT(sizeof(hkvPvrTexToolFormatStrings) / sizeof(hkvConversionToolFormatString) == HKV_TEXTURE_DATA_FORMAT_COUNT);


hkvExternalToolPvrTexTool::hkvExternalToolPvrTexTool(const hkvTextureTransformationSettings& settings, 
  const char* sourceFile, const char* targetFile)
: hkvExternalToolTransformation(sourceFile, targetFile)
{
  determineExecutablePath();
  determineCommandLineParameters(settings, sourceFile, targetFile);
  m_idlePriority = true;
}


const char* hkvExternalToolPvrTexTool::getToolName() const
{
  return "PVRTexTool";
}


void hkvExternalToolPvrTexTool::determineExecutablePath()
{
  hkStringBuf pathBuf;
  if (hkvFileHelper::getMainModuleBasePath(pathBuf) == HK_SUCCESS)
  {
    pathBuf.pathAppend("../../../Tools/PVRTexTool.exe");
    m_executable = pathBuf;
  }
}


void hkvExternalToolPvrTexTool::determineCommandLineParameters(
  const hkvTextureTransformationSettings& settings, const char* sourceFile, const char* targetFile)
{
  // Build the command line argument string
  hkStringBuf sourceFileBuf(sourceFile);
  sourceFileBuf.replace('/', '\\');
  hkStringBuf targetFileBuf(targetFile);
  targetFileBuf.replace('/', '\\');

  hkStringBuf paramBuf;
  paramBuf.appendJoin(" -i \"", sourceFileBuf, "\"");
  paramBuf.appendJoin(" -o \"", targetFileBuf, "\"");

  hkvTextureDataFormat dataFormat = settings.getTargetDataFormat();
  VASSERT_MSG(hkvPvrTexToolFormatStrings[dataFormat].m_format == dataFormat, "PVRTexTool format string lookup is inconsistent with defined texture formats!");
  paramBuf.appendJoin(" -f ", hkvPvrTexToolFormatStrings[dataFormat].m_string);

  switch (settings.getTargetFileFormat())
  {
  case HKV_TEXTURE_FILE_FORMAT_DDS:
    {
      break;
    }
  case HKV_TEXTURE_FILE_FORMAT_ETC:
    {
      paramBuf.append(" -legacypvr");
      break;
    }
  case HKV_TEXTURE_FILE_FORMAT_PVR:
    {
      paramBuf.append(" -legacypvr");
      break;
    }
  }

  if (settings.getCreateMipMaps())
  {
    paramBuf.append(" -m");
    paramBuf.append(" -mfilter cubic");
  }

  if (settings.getTargetWidth() != settings.getSourceWidth() || settings.getTargetHeight() != settings.getSourceHeight())
  {
    paramBuf.appendPrintf(" -r %i,%i", settings.getTargetWidth(), settings.getTargetHeight());
    paramBuf.appendPrintf(" -rfilter cubic");
  }
  
  m_commandLine = paramBuf;
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
