/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvExternalToolTexConv.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationSettings.hpp>

#include <Common/Base/Container/String/hkUtf8.h>

const hkvConversionToolFormatString hkvTexConvFormatStrings[] =
{
  {HKV_TEXTURE_DATA_FORMAT_PVRTC2, ""},
  {HKV_TEXTURE_DATA_FORMAT_PVRTC4, ""},

  {HKV_TEXTURE_DATA_FORMAT_ETC1, ""},

  {HKV_TEXTURE_DATA_FORMAT_DXT1, "BC1_UNORM"},
  {HKV_TEXTURE_DATA_FORMAT_DXT3, "BC2_UNORM"},
  {HKV_TEXTURE_DATA_FORMAT_DXT5, "BC3_UNORM"},

  {HKV_TEXTURE_DATA_FORMAT_A4R4G4B4, ""},
  {HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL, ""},
  {HKV_TEXTURE_DATA_FORMAT_A1R5G5B5, "B5G5R5A1_UNORM"},
  {HKV_TEXTURE_DATA_FORMAT_R5G6B5, "B5G6R5_UNORM"},
  {HKV_TEXTURE_DATA_FORMAT_A8R8G8B8, "B8G8R8A8_UNORM"},
  {HKV_TEXTURE_DATA_FORMAT_X8R8G8B8, "B8G8R8X8_UNORM"},
  {HKV_TEXTURE_DATA_FORMAT_R8G8B8, ""},
};
HK_COMPILE_TIME_ASSERT(sizeof(hkvTexConvFormatStrings) / sizeof(hkvConversionToolFormatString) == HKV_TEXTURE_DATA_FORMAT_COUNT);


hkvExternalToolTexConv::hkvExternalToolTexConv(const hkvTextureTransformationSettings& settings, 
  const char* sourceFile, const char* targetFile)
: hkvExternalToolTransformation(sourceFile, targetFile)
{
  determineExecutablePath();
  determineCommandLineParameters(settings, sourceFile, targetFile);
}


const char* hkvExternalToolTexConv::getToolName() const
{
  return "TexConv";
}


void hkvExternalToolTexConv::determineExecutablePath()
{
  hkStringBuf pathBuf;
  if (hkvFileHelper::getMainModuleBasePath(pathBuf) == HK_SUCCESS)
  {
    pathBuf.pathAppend("../../../Tools/texconv.exe");
    m_executable = pathBuf;
  }
}


void hkvExternalToolTexConv::determineCommandLineParameters(
  const hkvTextureTransformationSettings& settings, const char* sourceFile, const char* targetFile)
{
  // Build the command line argument string
  hkStringBuf sourceFileBuf(sourceFile);
  sourceFileBuf.replace('/', '\\');

  hkStringBuf outFileDir(sourceFile);
  outFileDir.pathDirname();
  outFileDir.replace('/', '\\');

  hkStringBuf sourceFileBaseName(sourceFile);
  sourceFileBaseName.pathBasename();
  hkvStringHelper::pathNoExtension(sourceFileBaseName);

  hkStringBuf outFileName(outFileDir);
  outFileName.appendJoin("/", sourceFileBaseName, "_out.dds");
  setTargetFile(outFileName);

  hkStringBuf paramBuf;
  paramBuf.append(" -nologo");

  if (settings.getTargetWidth() != settings.getSourceWidth())
  {
    paramBuf.appendPrintf(" -w %i", settings.getTargetWidth());
  }
  if (settings.getTargetHeight() != settings.getSourceHeight())
  {
    paramBuf.appendPrintf(" -h %i", settings.getTargetHeight());
  }

  if (settings.getCreateMipMaps())
  {
    paramBuf.append(" -m 0");
  }
  else
  {
    paramBuf.append(" -m 1");
  }

  paramBuf.append(" -sepalpha");

  hkvTextureDataFormat dataFormat = settings.getTargetDataFormat();
  VASSERT_MSG(hkvTexConvFormatStrings[dataFormat].m_format == dataFormat, "TexConv format string lookup is inconsistent with defined texture formats!");
  paramBuf.appendJoin(" -f ", hkvTexConvFormatStrings[dataFormat].m_string);

  paramBuf.append(" -if CUBIC");

  paramBuf.append(" -sx _out");

  paramBuf.appendJoin(" -o \"", outFileDir.cString(), "\"");

  paramBuf.append(" -ft DDS");

  paramBuf.appendJoin(" \"", sourceFileBuf.cString(), "\"");

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
