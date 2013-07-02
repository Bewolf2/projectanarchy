/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvExternalToolWiiUTexConv2.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationSettings.hpp>

#include <Common/Base/Container/String/hkUtf8.h>


hkvExternalToolWiiUTexConv2::hkvExternalToolWiiUTexConv2(
  const hkvTextureTransformationSettings& settings, 
  const char* sourceFile, const char* targetFile)
: hkvExternalToolTransformation(sourceFile, targetFile)
{
  determineExecutablePath();
  determineCommandLineParameters(settings, sourceFile, targetFile);
}


const char* hkvExternalToolWiiUTexConv2::getToolName() const
{
  return "WiiUTexConv2";
}


void hkvExternalToolWiiUTexConv2::determineExecutablePath()
{
  wchar_t envBuf[MAX_PATH + 1];
  DWORD numChars = GetEnvironmentVariableW(L"CAFE_ROOT", envBuf, MAX_PATH + 1);
  if ((numChars == 0) || (numChars > MAX_PATH + 1))
  {
    return;
  }

  hkStringBuf pathBuf(hkUtf8::Utf8FromWide(envBuf).cString());
  pathBuf.replace('\\', '/');
  pathBuf.pathAppend("system/bin/win32/TexConv2.exe");

  m_executable = pathBuf;
}


void hkvExternalToolWiiUTexConv2::determineCommandLineParameters(
  const hkvTextureTransformationSettings& settings, const char* sourceFile, const char* targetFile)
{
  // Build the command line argument string
  hkStringBuf sourceFileBuf(sourceFile);
  sourceFileBuf.replace('/', '\\');

  hkStringBuf targetFileBuf(targetFile);
  targetFileBuf.replace('/', '\\');

  hkStringBuf paramBuf;

  paramBuf.appendJoin(" -i \"", sourceFileBuf.cString(), "\"");
  paramBuf.appendJoin(" -o \"", targetFileBuf.cString(), "\"");

  if (settings.getCreateMipMaps())
  {
    paramBuf.append(" -minmip 1 -mipFilter box");
  }

  paramBuf.append(" -align");
  
  // Enable the following line to get extended information back 
  // from the conversion tool
  //paramBuf.append(" -printinfo");

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
