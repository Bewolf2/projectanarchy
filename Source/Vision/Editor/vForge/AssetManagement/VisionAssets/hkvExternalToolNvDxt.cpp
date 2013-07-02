/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvExternalToolNvDxt.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationSettings.hpp>

#include <Common/Base/Container/String/hkUtf8.h>

const hkvConversionToolFormatString hkvNvDxtFormatStrings[] =
{
  {HKV_TEXTURE_DATA_FORMAT_PVRTC2, ""},
  {HKV_TEXTURE_DATA_FORMAT_PVRTC4, ""},

  {HKV_TEXTURE_DATA_FORMAT_ETC1, ""},

  {HKV_TEXTURE_DATA_FORMAT_DXT1, "dxt1c"},
  {HKV_TEXTURE_DATA_FORMAT_DXT3, "dxt3"},
  {HKV_TEXTURE_DATA_FORMAT_DXT5, "dxt5"},

  {HKV_TEXTURE_DATA_FORMAT_A4R4G4B4, "u4444"},
  {HKV_TEXTURE_DATA_FORMAT_R4G4B4A4_GL, ""},
  {HKV_TEXTURE_DATA_FORMAT_A1R5G5B5, "u1555"},
  {HKV_TEXTURE_DATA_FORMAT_R5G6B5, "u565"},
  {HKV_TEXTURE_DATA_FORMAT_A8R8G8B8, "u8888"},
  {HKV_TEXTURE_DATA_FORMAT_X8R8G8B8, ""},
  {HKV_TEXTURE_DATA_FORMAT_R8G8B8, "u888"},
};
HK_COMPILE_TIME_ASSERT(sizeof(hkvNvDxtFormatStrings) / sizeof(hkvConversionToolFormatString) == HKV_TEXTURE_DATA_FORMAT_COUNT);


hkvExternalToolNvDxt::hkvExternalToolNvDxt(const hkvTextureTransformationSettings& settings, 
  const char* sourceFile, const char* targetFile)
: hkvExternalToolTransformation(sourceFile, targetFile)
{
  determineExecutablePath();
  determineCommandLineParameters(settings, sourceFile, targetFile);
}


const char* hkvExternalToolNvDxt::getToolName() const
{
  return "nvDXT";
}


hkResult hkvExternalToolNvDxt::beforeRun()
{
  hkStringBuf tempDirBuf;
  if (hkvFileHelper::getSystemTempPath(tempDirBuf) != HK_SUCCESS)
  {
    return HK_FAILURE;
  }
  tempDirBuf.appendPrintf("\\NvDxt_tmp_%08X", GetCurrentThreadId());
  m_tempDir = tempDirBuf;

  if (hkvFileHelper::createDirectoryRecursive(m_tempDir) != HK_SUCCESS)
  {
    return HK_FAILURE;
  }

  m_workingDirectory = m_tempDir;

  return HK_SUCCESS;
}


void hkvExternalToolNvDxt::afterRun(bool runWasSuccessful)
{
  if (m_tempDir.getLength() > 0)
  {
    hkUtf8::WideFromUtf8 tempDirW(m_tempDir);
    if (RemoveDirectoryW(tempDirW.cString()) == FALSE)
    {
      DWORD err = GetLastError();
    }
  }
}


void hkvExternalToolNvDxt::determineExecutablePath()
{
  hkStringBuf pathBuf;
  if (hkvFileHelper::getMainModuleBasePath(pathBuf) == HK_SUCCESS)
  {
    pathBuf.pathAppend("../tools/nvDXT/nvdxt.exe");
    m_executable = pathBuf;
  }
}


void hkvExternalToolNvDxt::determineCommandLineParameters(
  const hkvTextureTransformationSettings& settings, const char* sourceFile, const char* targetFile)
{
  // Build the command line argument string
  hkStringBuf sourceFileBuf(sourceFile);
  sourceFileBuf.replace('/', '\\');
  hkStringBuf targetFileBuf(targetFile);
  targetFileBuf.replace('/', '\\');

  hkStringBuf paramBuf;
  paramBuf.appendJoin(" -file \"", sourceFileBuf, "\"");
  paramBuf.appendJoin(" -output \"", targetFileBuf, "\"");

  hkvTextureDataFormat dataFormat = settings.getTargetDataFormat();
  VASSERT_MSG(hkvNvDxtFormatStrings[dataFormat].m_format == dataFormat, "nvDXT format string lookup is inconsistent with defined texture data formats!");
  paramBuf.appendJoin(" -", hkvNvDxtFormatStrings[dataFormat].m_string);

  if (!settings.getCreateMipMaps())
  {
    paramBuf.appendJoin(" -nomipmap");
  }

  if ((settings.getTargetWidth() != settings.getSourceWidth()) || (settings.getTargetHeight() != settings.getSourceHeight()))
  {
    paramBuf.appendPrintf(" -prescale %i %i", settings.getTargetWidth(), settings.getTargetHeight());
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
