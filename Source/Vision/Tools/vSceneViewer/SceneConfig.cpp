/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vSceneViewer/vSceneViewerPCH.h>

#include <Vision/Runtime/Common/VisSampleApp.hpp>

static VTargetDevice_e s_CurPlatform;
static VListControl* s_pSceneList;
static VString s_sHomeDir;

int LUA_AddScene (lua_State* state)
{
  vLuaScript s (state);

  const hkUint32 uiPlatforms = (hkUint32) s.getIntParameter (1);

  // if this scene is not supported on this platform, ignore it
  if ((uiPlatforms & (1 << s_CurPlatform)) == 0)
    return 0;

  const char* szDisplayName  = s.getStringParameter (2);
  VString sSceneFile         = s.getStringParameter (3);
  
  // fix path separators for different platforms
  {
    // first ensure all path separators are /
    sSceneFile.ReplaceAll ("\\\\", "/");
    sSceneFile.ReplaceAll ("\\", "/");

    char szFileName[FS_MAX_PATH] = "";
    VPathHelper::ConvertFilename (szFileName, sSceneFile.AsChar (), NULL);
    sSceneFile = szFileName;
  }

  VString sTooltip;

  // Check if we have a thumbnail filename
  const char* szThumbnailFileName = "default_thumbnail.tga";

  // The first parameter may be a thumbnail parameter
  if(s.getNumberOfFunctionParameters() >= 4)
  {
    const char* szPotentialThumbnailName = s.getStringParameter(4);

    // Is this a tga filename?
    if(ContainsI(szPotentialThumbnailName, ".tga") != NULL)
    {
      szThumbnailFileName = szPotentialThumbnailName;
    }
  }

  if(s.getNumberOfFunctionParameters() >= 5)
  {
    VNameValueListParser<';', '=', 1024> parser(s.getStringParameter (5), false);
    while (parser.next())
    {
      const char* szDataDir = parser.name();

      if(szDataDir != NULL && szDataDir[0] != '~')
        sTooltip += s_sHomeDir;

      sTooltip += szDataDir;
      sTooltip += ";";
    }
  }

  // add the scene file name to the menu string

  if(sSceneFile.GetSafeStr()[0] != '~')
    sTooltip += s_sHomeDir;

  sTooltip += sSceneFile;

  VListControlItem* pItem = s_pSceneList->AddItem (szDisplayName);
  pItem->SetTooltipText (sTooltip.AsChar ());
  
  char szFileName[256];
  sprintf(szFileName, "Thumbnails/%s", szThumbnailFileName);
  VTextureObjectPtr spThumbnail = Vision::TextureManager.Load2DTexture(szFileName, VTM_FLAG_DEFAULT_NON_MIPMAPPED|VTM_FLAG_NO_DOWNSCALE);

  pItem->Icon().m_States[VWindowBase::NORMAL].SetTexture(spThumbnail);

  return 0;
}

int LUA_SetHomeDir (lua_State* state)
{
  vLuaScript s (state);

  const hkUint32 uiPlatforms = (hkUint32) s.getIntParameter (1);

  // if this base dir is not meant for this platform, ignore it
  if ((uiPlatforms & (1 << s_CurPlatform)) == 0)
    return 0;

  const char* szHomeDir = s.getStringParameter (2);
  VASSERT(szHomeDir != NULL);

   // If the incoming path is a file server path (prepended with ~), we need to replace it with the
   // common data directory (used for 
  if ( (szHomeDir[0] == '~') && (!VisSampleApp::IsFileServerInstalled()) )
  {
    char szAppDirPath[FS_MAX_PATH];
    #ifdef _VISION_ANDROID
      sprintf(szAppDirPath, "%s?assets/%s", VisSampleApp::GetApkDirectory(), &(szHomeDir[2])); 
    #elif defined(_VISION_IOS)
      sprintf(szAppDirPath, "%s/%s", VisSampleApp::GetRootDirectory(), &(szHomeDir[2]));
    #endif
    s_sHomeDir = szAppDirPath;
  }
  else
  {
    s_sHomeDir = szHomeDir;
  }

  return 0;
}


hkvResult LoadSceneConfig (VTargetDevice_e CurPlatform, const char* szSceneConfigFile, VListControl* pList)
{
  s_CurPlatform = CurPlatform;
  s_pSceneList = pList;

  int iWidth, iHeight;
  VisRenderContext_cl::GetMainRenderContext()->GetSize(iWidth, iHeight);
  float fIconSize = hkvMath::floor(hkvMath::clamp(iHeight / 12.0f, 24.0f, 128.0f));

  s_pSceneList->SetIconSize(fIconSize, hkvVec2(0.0f));

  vLuaScript s;

  // register all available platforms as bitmasks
  for (hkInt32 i = 0; i < VShaderEnum::g_devices.GetNumEntries (); ++i)
  {
    VString sDeviceName = VShaderEnum::g_devices.GetNameFromIndex (i);

    // remove all white spaces
    sDeviceName.ReplaceAll (" ", "");
    sDeviceName.ReplaceAll ("\t", "");

    sDeviceName.ToUpper ();

    s.setVariable (sDeviceName.AsChar (), 1 << i);
  }

#ifdef HK_ANARCHY
  s.setVariable("HK_ANARCHY", true);
#else
  s.setVariable("HK_ANARCHY", false);
#endif

#ifdef _VISION_WINRT // all just DX11 set

  s.setVariable ("APOLLO", 1 << TARGETDEVICE_COUNT );
  s.setVariable ("METRO", 1 << TARGETDEVICE_COUNT );
  s_CurPlatform = TARGETDEVICE_COUNT;

#endif

  // register the callback functions
  s.RegisterCFunction ("AddScene", LUA_AddScene);
  s.RegisterCFunction ("SetHomeDir", LUA_SetHomeDir);

  // this will write the result into s_sMenuScenes
  VString sError;
  if (!s.ExecuteFile (szSceneConfigFile, sError))
    return HKV_FAILURE;

  return HKV_SUCCESS;
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
