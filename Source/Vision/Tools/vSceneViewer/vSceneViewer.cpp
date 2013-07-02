/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Scene Viewer
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#include <Vision/Tools/vSceneViewer/vSceneViewerPCH.h>

#include <Vision/Runtime/Base/Container/VScopedPtr.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VPrefab.hpp>
#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiSimpleRendererNode.hpp>

#if !defined (WIN32) || defined(_VISION_WINRT)
#include <Vision/Tools/vSceneViewer/GUI/GUISystem.hpp>
#endif

#ifdef _VISION_MOBILE
#include <Vision/Runtime/Common/VSampleTouchMenu.hpp>
#endif

#if defined( USE_FILESERVE )
  #include <Vision/Runtime/Base/System/IO/Stream/VFileServeStreamManager.hpp> 
#endif

#define ENABLE_VISUAL_DEBUGGER
#ifdef ENABLE_VISUAL_DEBUGGER
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp> 
#endif

// Include key-codes last so they are not disabled by some other product includes
#include <Common/Base/KeyCode.h>

hkvResult LoadSceneConfig (VTargetDevice_e CurPlatform, const char* szSceneConfigFile, VListControl* pList);

VisBaseEntity_cl *g_pCamera = NULL;

#if !defined(WIN32) || defined(_VISION_MOBILE)

  SceneViewerMainMenuPtr g_spMainDlg;
  VGUIMainContextPtr g_spGUIContext;

  #ifdef _VISION_IOS
    VScopedPtr<VSampleTouchMenu> g_spTouchMenu;
    VSimpleSampleTouchButton* g_pMenuToggleTouch;
  #endif

#endif

// custom overloads of the scene viewer go into this class
class VSceneViewerApp : public VisSampleApp
{
public:
  VOVERRIDE void OnLoadSceneStatus(int iStatus, float fPercentage, const char* pszStatus)
  {
#if defined(WIN32) && !defined(_VISION_WINRT)
    // ESC key can abort the loading
    if (Vision::Key.IsPressed(VGLK_ESC))
    {
      GetLoadingProgress().SetAbort();
      return;
    }
#endif

    VisSampleApp::OnLoadSceneStatus(iStatus, fPercentage, pszStatus); // important! call the base implementation
  }

  VSceneLoader* GetSceneLoader()
  {
    return &m_sceneLoader;
  }

#if defined(_VISION_MOBILE)
  void SetGUIContext(VGUIMainContext* pGUIContext)
  {
    VASSERT(m_spGUIContext == NULL);
    m_spGUIContext = pGUIContext;
  }
#endif

protected:

#if defined(_VISION_ANDROID)
  // override back button behavior -> show scene list.
  virtual bool OnAndroidBackButtonPressed() HKV_OVERRIDE
  {
    // Go back to menu if currently viewing a scene.
    if (g_pCamera != NULL && !g_spMainDlg->IsVisible())
    {
      g_spMainDlg->ToggleView(g_pCamera);

      // Input has been handled -> Do not show Exit Dialog yet.
      return true; 
    }

    return false;
  }
#endif
};

VSmartPtr<VSceneViewerApp> g_spApp;

//-----------------------------------------------------------------------------------

#define DEINIT_MOUSE_IF_AVAILABLE() \
  if(VInputManagerPC::IsInitialized() && (&VInputManagerPC::GetMouse())!=NULL) { \
  VInputManagerPC::GetMouse().DeInit(); \
  } \

#define INIT_MOUSE_IF_AVAILABLE() \
  if(VInputManagerPC::IsInitialized() && (&VInputManagerPC::GetMouse())!=NULL) { \
  VInputManagerPC::GetMouse().Init(); \
  } \


#if defined(WIN32) && !defined(_VISION_WINRT)

////////////////////////////////////////////////////////////////////////////
// SceneViewConfig_t: structure that provides scene viewer properties
////////////////////////////////////////////////////////////////////////////
struct SceneViewConfig_t
{
  SceneViewConfig_t()
  {
    bForceFreeCam = false;
    bForceNoCam = false;
    bFullScreen = false;
    bAskFullScreen = false;    
    iResX = V_DEFAULT_XRES;
    iResY = V_DEFAULT_YRES;
    iAnisotropicFilteringLevel = 0;
    bShowHelp = false;
    bHelpOnly = false;
    bIsVScene = false;
    bIsVMesh = false;
    bIsVZone = false;
    bIsVPrefab = false;
    bIsLITFile  = false;
    bWaitForVSync = false;
    iAdapter = 0;
  }

  VString GetSceneFileNameFromArgvW(LPWSTR argvW)
  {
    return VString(argvW);
  }  

  void SetFileTypeFromSceneFileName()
  {
    const char *szFileExt = VFileHelper::GetExtension(sSceneName);

    // set config according to file extension
    if (szFileExt)
    {
      if( !stricmp(szFileExt, "VSCENE"))          bIsVScene = true;
      else if( !stricmp(szFileExt, "VMESH"))      bIsVMesh  = true; 
      else if( !stricmp(szFileExt, "VZONE"))      bIsVZone  = true; 
      else if( !stricmp(szFileExt, "VPREFAB"))    bIsVPrefab = true; 
      else if( !stricmp(szFileExt, "LIT"))         bIsLITFile = true;
    }
  }

  void ParseParams()
  {
    LPWSTR *argv;
    int argc;
    int i;

    bMouseReleased = false;

    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argc==1) // no arguments -> show help
      bShowHelp = true;

    for (i=1;i<argc;i++)
    {
      bool bScenefileRequired = false;

      LPWSTR pArg = argv[i];
      VASSERT(pArg);
      if (pArg[0]=='?')
        bShowHelp = true;

      if (pArg[0]=='-' || pArg[0]=='/')
      {
        pArg++;
        if (!_wcsicmp(pArg,L"help")) 
        {
          bShowHelp = true; 
          continue;
        }

        if (!_wcsicmp(pArg,L"?")) 
        {
          bShowHelp = true; 
          continue;
        }

        if (!_wcsicmp(pArg,L"freecam")) bForceFreeCam = true; 
        else if (!_wcsicmp(pArg,L"nocam")) bForceNoCam = true; 
        else if (!_wcsicmp(pArg,L"askfullscreen")) bAskFullScreen = true; 
        else if (!_wcsicmp(pArg,L"fullscreen")) bFullScreen = true; 
        else if (!_wcsicmp(pArg,L"af"))
        {
          i++;
          if (i>=argc) // next one is anisotropy filtering level
          {
            VASSERT(FALSE && "Missing anisotropy level parameter")
            continue;
          }
          iAnisotropicFilteringLevel = _wtoi(argv[i]);          
          VASSERT(iAnisotropicFilteringLevel>0 && iAnisotropicFilteringLevel<=256);          
        }
        else if (!_wcsicmp(pArg,L"windowed")) bFullScreen = false;
        else if(!_wcsicmp(pArg,L"vsync")) bWaitForVSync = true;
        else if (!_wcsicmp(pArg,L"scene"))
        {
          bIsVScene = true;
          bScenefileRequired = true;
        }
        else if(!_wcsicmp(pArg,L"mesh"))
        {
          bIsVMesh = true;
          bScenefileRequired = true;
        }
        else if(!_wcsicmp(pArg,L"zone"))
        {
          bIsVZone = true;
          bScenefileRequired = true;
        }
        else if(!_wcsicmp(pArg,L"prefab"))
        {
          bScenefileRequired = true;
        }
        else if (!_wcsicmp(pArg,L"res"))
        {
          if (i>=argc-2) // next two ones are x resp. y res
          {
            VASSERT(FALSE);
            continue;
          }
          iResX = _wtoi(argv[i+1]);
          iResY = _wtoi(argv[i+2]);
          VASSERT(iResX>0 && iResX<=4096);
          VASSERT(iResY>0 && iResY<=4096);
          i+=2;
        }
        else if(!_wcsicmp(pArg,L"cleancopy")) // WIN32 only
        {
          i++;
          VASSERT(i<argc); // next one is directory name
          sCleanCopyFolder = GetSceneFileNameFromArgvW(argv[i]);
        }
        else if (!_wcsicmp(pArg,L"adapter"))
        {
          i++;
          VASSERT(i<argc); // next one is adapter index
          iAdapter = _wtoi(argv[i]);
          VASSERT(iAdapter>=0 && iAdapter<=16);
        }
        else if (!_wcsicmp(pArg,L"varkeys"))
        {
          i++;
          VASSERT(i<argc); // next one is variant key list
          sVariantKeys  = argv[i];
        }

        if(bScenefileRequired)
        {
          i++;
          VASSERT(i<argc); // next one is VPrefab name
          sSceneName = GetSceneFileNameFromArgvW(argv[i]);
        }
      }
      else if(i==1) //hand for filename without type separator
      {
        sSceneName = GetSceneFileNameFromArgvW(argv[i]);
        SetFileTypeFromSceneFileName();
      }
    }

    if (bShowHelp && argc==2) // e.g. vsceneviewer -help
      bHelpOnly = true;

    LocalFree(argv);
  }

  inline bool HasSceneName() const {return !sSceneName.IsEmpty();}
  inline int GetSampleFlags() const
  {
    int iSampleFlags = 0;
    if (bFullScreen) iSampleFlags|=VSAMPLE_FORCEFULLSCREEN;
    if (bAskFullScreen) iSampleFlags|=VSAMPLE_ASKFULLSCREEN;
    if (bWaitForVSync) iSampleFlags|=VSAMPLE_WAITRETRACE;
    return iSampleFlags;
  }

  void ToLog()
  {
    Vision::Error.SystemMessage("Scene viewer Settings:");
    Vision::Error.SystemMessage("----------------------");
    Vision::Error.SystemMessage("Scene name : '%s'",sSceneName.AsChar());
    Vision::Error.SystemMessage("Resolution : %ix%i",iResX,iResY);
    Vision::Error.SystemMessage("FullScreen : %s",bFullScreen?"On":"Off");
    Vision::Error.SystemMessage("Ask fullscreen : %s",bAskFullScreen?"On":"Off");
    Vision::Error.SystemMessage("Force free camera : %s",bForceFreeCam?"Yes":"No");
    Vision::Error.SystemMessage("Force no camera : %s",bForceNoCam?"Yes":"No");
    Vision::Error.SystemMessage("Anisotropic filtering: %i",iAnisotropicFilteringLevel);
    Vision::Error.SystemMessage("Vertical Sync : %s",bWaitForVSync?"Enabled":"Disabled");
    if (bForceFreeCam && bForceNoCam)
      Vision::Error.SystemMessage("bForceFreeCam flag conflicts with bForceNoCam flag!");
    Vision::Error.SystemMessage("----------------------");
  }

  // members:
  bool bForceFreeCam, bForceNoCam;
  bool bFullScreen;
  bool bAskFullScreen;  
  bool bShowHelp,bHelpOnly;
  bool bIsVScene;
  bool bIsVMesh;
  bool bIsVZone;
  bool bIsVPrefab;
  bool bIsLITFile;
  bool bMouseReleased;
  bool bWaitForVSync;
  int iResX, iResY;
  int iAnisotropicFilteringLevel;
  VString sSceneName;
  VString sCleanCopyFolder;
  int iAdapter;
  VString sVariantKeys;
};

// global instance
SceneViewConfig_t g_Config;

// standard file dialog to load a world file
bool LoadWorldDialog(char *destName, char *destPath)
{
  // convert to wchar_t and build path
  wchar_t wszDestName[MAX_PATH];
  wchar_t wszDestPath[MAX_PATH];
  wchar_t wszPathName[MAX_PATH];
  V_UTF8_TO_WCHAR(destName, wszDestName, MAX_PATH);
  V_UTF8_TO_WCHAR(destPath, wszDestPath, MAX_PATH);
  swprintf(wszPathName, MAX_PATH, L"%s\\%s", wszDestPath, wszDestName);

  OPENFILENAMEW openfile;
  memset(&openfile, 0, sizeof(openfile));
  openfile.lStructSize = sizeof(openfile);
  openfile.lpstrFilter = L"Vision scene files\0*.vscene;*.vzone;*.vprefab;*.vmesh;*.lit\0";
  openfile.lpstrCustomFilter  = NULL;

  openfile.lpstrFile  = wszPathName;
  openfile.nMaxFile   = MAX_PATH-1;
  openfile.lpstrFileTitle   = wszDestName;
  openfile.nMaxFileTitle    = MAX_PATH-1;
  openfile.lpstrInitialDir  = wszDestPath;
  openfile.lpstrTitle = L"Select a map file";
  openfile.Flags = OFN_ENABLESIZING|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;

  DEINIT_MOUSE_IF_AVAILABLE();
  if (!GetOpenFileNameW(&openfile)) 
    return false;
  INIT_MOUSE_IF_AVAILABLE();

  if (openfile.nFileOffset>0)
    wszPathName[openfile.nFileOffset-1] = 0; // separate path from filename

  wcscpy(wszDestPath, wszPathName);
  wcscpy(wszDestName, &wszPathName[openfile.nFileOffset]);

  // convert back to UTF-8
  V_WCHAR_TO_UTF8(wszDestPath, destPath, MAX_PATH);
  V_WCHAR_TO_UTF8(wszDestName, destName, MAX_PATH);

  return true;
}
#endif


void LookAtBox(const hkvAlignedBBox &bbox, hkvVec3 &destPos)
{
  // calculate bounding box radius
  hkvVec3 vCenter = bbox.getCenter();
  float BBRadius = bbox.getMaxExtent()*0.5f;

  float Dist = 90.0f;
  float Fov, Dummy;
  Vision::Contexts.GetMainRenderContext()->GetFOV( Fov, Dummy );
  float Angle = Fov * 0.5f;
  Dist = BBRadius / hkvMath::tanDeg (Angle);
  destPos.set( -Dist, 0.0f, 0.0f );
  destPos += vCenter;
}

#if defined (WIN32)

void LoadLitFile(const char *szFilename)
{
  VLightmapSceneInfo meshes,lit;
  if (!lit.LoadOutputFile(szFilename,Vision::File.GetManager(),&Vision::TextureManager.GetManager()))
    return;
  char szMeshFile[FS_MAX_PATH];
  VFileHelper::AddExtension(szMeshFile,szFilename,"temp");
  if (!meshes.LoadMeshFile(szMeshFile,Vision::File.GetManager()))
    return;

  Vision::InitWorld();
  VisionAppHelpers::CreateLightmapMeshes(meshes,&lit);
}

#endif // WIN32


#if defined(WIN32) && !defined(_VISION_WINRT)

// This filemanager has been designed to hook into file accesses and create a batch file for copying all
// files that it touches. It is linked to the "cleancopy" command line parameter (followed by an absolute
// path that defines the target folder)
class CopyFileManager : public IVFileStreamManager
{
public:
  CopyFileManager()
  {
    m_spManager = Vision::File.GetManager();
    m_pBatchFile = NULL;
    char szEXEDir[FS_MAX_PATH];
    VFileHelper::GetCurrentWorkingDir( szEXEDir, FS_MAX_PATH );
    VFileHelper::CombineDirAndFile(m_BaseDataDir,szEXEDir,VISION_BASE_DATA);
  }

  ~CopyFileManager()
  {
    if (m_pBatchFile)
      m_pBatchFile->Close();
  }

  VOVERRIDE const char* GetName() {return m_spManager->GetName();}

  VOVERRIDE IVFileInStream* InternalOpen(const char* pszFileName, int iFlags)
  {
    if (!m_pBatchFile)
    {
      char szBatchName[FS_MAX_PATH];
      VFileHelper::CombineDirAndFile(szBatchName,g_Config.sCleanCopyFolder,VFileHelper::GetFilename(g_Config.sSceneName));
      int iExtPos = VFileHelper::GetExtensionPos(szBatchName);
      VASSERT(iExtPos>0);
      strcpy(&szBatchName[iExtPos],"_CopyFiles.bat");
      m_pBatchFile = m_spManager->Create(szBatchName);
    }

    IVFileInStream* pIn = CallInternalOpen(*m_spManager, pszFileName, iFlags);
    if (!pIn) return NULL;
    const char *szSrcFile = pIn->GetFileName();

    // ignore files that come from base data
    if (_strnicmp(m_BaseDataDir,szSrcFile,m_BaseDataDir.GetLength())==0)
      return pIn;

    if (m_CopiedFiles.FindString(szSrcFile)<0 && m_pBatchFile!=NULL)
    {
      m_CopiedFiles.AddString(szSrcFile);
      char szDstFile[FS_MAX_PATH];
      VFileHelper::CombineDirAndFile(szDstFile,g_Config.sCleanCopyFolder,pszFileName);
      char szLine[1024];
      sprintf(szLine,"xcopy \"%s\" \"%s\" /s\r\n", szSrcFile, szDstFile); // copy with subdir
      //res = WinExec(szLine,0);
      m_pBatchFile->WriteText(szLine);
    }

    return pIn;
  }

  VOVERRIDE IVFileOutStream* Create(const char* pszFileName,int iFlags=0)
  {
    return m_spManager->Create(pszFileName,iFlags);
  }

  VOVERRIDE BOOL InternalExists(const char* pszFileName)
  {
    return CallInternalExists(*m_spManager, pszFileName);
  }

  VOVERRIDE BOOL InternalGetTimeStamp(const char *pszFileName, VFileTime &destTime)
  {
    return CallInternalGetTimeStamp(*m_spManager, pszFileName, destTime);
  }

  VOVERRIDE int GetNumDataDirectories()
  {
    return m_spManager->GetNumDataDirectories();
  }

  VOVERRIDE bool InternalSetDataDirectory(int idx, const char* pszPath)
  {
    return m_spManager->SetDataDirectory(idx,pszPath);
  }

  VOVERRIDE const char* GetDataDirectory(int idx = 0)
  {
    return m_spManager->GetDataDirectory(idx);
  }


  IVFileStreamManagerPtr m_spManager;
  IVFileOutStream *m_pBatchFile;
  VStrList m_CopiedFiles;
  VStaticString<FS_MAX_PATH> m_BaseDataDir;
};

#endif // WIN32


bool g_bIsFirstLoad = true;

char g_sceneName[FS_MAX_PATH];
char g_scenePath[FS_MAX_PATH];
char g_projectPath[FS_MAX_PATH];

// vSceneViewer Data Path
#if defined(WIN32) && !defined(_VISION_WINRT)
  #define VSCENEVIEWER_DATA_PATH "Data\\Vision\\Tools\\vSceneViewer"
#elif defined (_VISION_XENON)
  #define VSCENEVIEWER_DATA_PATH  "D:\\Data\\Vision\\Tools\\vSceneViewer"
#elif defined (_VISION_PS3)
  #define VSCENEVIEWER_DATA_PATH  "/app_home/Data/Vision/Tools/vSceneViewer"
#elif defined (_VISION_PSP2)
  #define VSCENEVIEWER_DATA_PATH  "app0:Data/Vision/Tools/vSceneViewer"
#elif defined(_VISION_ANDROID)
  static char g_szSceneViewerDataDir[FS_MAX_PATH];
  inline const char *GetSceneViewerDataDir()
  {
    sprintf(g_szSceneViewerDataDir, "%s?assets/Data/Vision/Tools/vSceneViewer/", VisSampleApp::GetApkDirectory()); 
    return g_szSceneViewerDataDir;
  }
  #define VSCENEVIEWER_DATA_PATH  GetSceneViewerDataDir()
#elif defined(_VISION_IOS)
  static char g_szSceneViewerDataDir[FS_MAX_PATH];
  inline const char *GetSceneViewerDataDir()
  {
    sprintf(g_szSceneViewerDataDir, "%s/Data/Vision/Tools/vSceneViewer/", VisSampleApp::GetRootDirectory());
    return g_szSceneViewerDataDir;
  }
  #define VSCENEVIEWER_DATA_PATH  GetSceneViewerDataDir()
#elif defined (_VISION_WIIU)
  #define VSCENEVIEWER_DATA_PATH  "/vol/content/VisionRoot/Data/Vision/Tools/vSceneViewer"
#else
  #error "Undefined platform!"
#endif

hkvVec3 g_vDefaultCamPos;
hkvMat3 g_mDefaultCamRot;

bool g_bRenderCollisionMesh = false;

#if defined (WIN32) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU) && !defined(_VISION_WINRT)

  // shading mode related:
  // backup renderer node and prepare shading modes:
  IVRendererNodePtr g_spRendererNode;
  VRefCountedCollection<VCompiledEffect> g_shadingEffects;
  int g_iCurrentShadingMode = -1;
  VSmartPtr<VForgeDebugRenderLoop> g_spShadingModeRenderLoop;
  VSmartPtr<VShaderEffectLib> g_spShadingShaderLib;

  void EnumShadingModes(VRefCountedCollection<VCompiledEffect> &effects)
  {
    // add the vForge data directory because it contains the shader lib with shading modes
    char szForgePath[FS_MAX_PATH];
    char szExeDir[FS_MAX_PATH];
    if (!VBaseAppHelpers::GetApplicationDir(szExeDir))
      return;
#if defined(WIN32)
    VFileHelper::CombineDirAndFile(szForgePath, szExeDir, "..\\..\\..\\..\\Data\\Vision\\Editor\\vForge");
#elif defined(_VISION_XENON)
    VFileHelper::CombineDirAndFile(szForgePath, szExeDir, "Data\\Vision\\Editor\\vForge");
#else
    VFileHelper::CombineDirAndFile(szForgePath, szExeDir, "Data\\Vision\\Editor\\vForge");
#endif
    if (!Vision::File.AddDataDirectory(szForgePath))
      return;

    VisShaderFXLibManager_cl &manager(Vision::Shaders.GetShaderFXLibManager());

#if defined(WIN32)
    // we have to load this way to be able to load the non-binary version (as we need the effect description)
    g_spShadingShaderLib = new VShaderEffectLib(&manager,"Shaders\\vForgeShadingEffects.ShaderLib");
    g_spShadingShaderLib->m_iLoadingFlags |= VSF_LOAD_IGNOREPLATFORMBINARIES;
    g_spShadingShaderLib->EnsureLoaded();
    if (!g_spShadingShaderLib->IsLoaded())
      return;
#else
    g_spShadingShaderLib = Vision::Shaders.LoadShaderLibrary("Shaders\\vForgeShadingEffects.ShaderLib");
    if ((g_spShadingShaderLib == NULL) || (g_spShadingShaderLib && !g_spShadingShaderLib->IsLoaded()))
      return;
#endif

    for (int i=0;i<g_spShadingShaderLib->m_Effects.Count();i++)
    {
      VShaderEffectResource *pFXRes = g_spShadingShaderLib->m_Effects.GetAt(i);

#if defined(WIN32)
      if (pFXRes->m_sDescription.IsEmpty() || pFXRes->m_sDescription=="<internal>")
        continue;

      VCompiledEffect *pFX = pFXRes->CompileEffect(NULL,manager.m_ShaderInstances);
      if (pFX==NULL)
        continue;
      pFX->SetUserData((void *)pFXRes->m_sDescription.AsChar());
#else
      // we do not have the description on consoles, thus we use the source effect's name
      VCompiledEffect *pFX = pFXRes->CompileEffect(NULL,manager.m_ShaderInstances);
      // we also do not want to expose the vForge_NotAvailable shader as a option
      if ((pFX==NULL) || (pFX && VStringHelper::SafeCompare(pFX->GetSourceEffect()->GetName(), "vForge_NotAvailable") == 0))
        continue;
      pFX->SetUserData((void *)pFX->GetSourceEffect()->GetName());
#endif
      effects.Add(pFX);
    }
  }

#endif

static unsigned int g_uiUsedDirectories = 0;
static void RecordDataDirectoryConfiguration()
{
  int iMaxDirectories = hkvMath::Min( Vision::File.GetMaxDataDirectoryCount(), int ( sizeof( unsigned int ) * 8 ) );

  // record, which directories are currently in use
  g_uiUsedDirectories = 0;
  unsigned int uiMask = 1;
  for ( int i = 0; i < iMaxDirectories; ++i )
  {
    if ( Vision::File.GetDataDirectory( i ) != NULL )
    {
      g_uiUsedDirectories |= uiMask;
    }
    uiMask = uiMask << 1;
  }
}

static void RestoreDataDirectoryConfiguration()
{
  int iMaxDirectories = hkvMath::Min( Vision::File.GetMaxDataDirectoryCount(), int ( sizeof( unsigned int ) * 8 ) );

  // get rid of all directories, which haven't been recorded as being in use
  unsigned int uiMask = 1;
  for ( int i = 0; i < iMaxDirectories; ++i )
  {
    if ( 0 == ( g_uiUsedDirectories & uiMask ) )
    {
      Vision::File.SetDataDirectory( i, NULL );
    }
    uiMask = uiMask << 1;
  }
}

static void SetVariantKeys(const VString& sVarKeys)
{
  VNameValueListParser<';', '='> parser(sVarKeys.AsChar(), false);
  while (parser.next())
  {
    const char* szVarKey = parser.name();

    AssetVariantKeys::Add(szVarKey);
  }
}

VISION_INIT
{

  VISION_SET_DIRECTORIES(false);

#ifdef USE_FILESERVE
  VisSampleApp::SetupFileServeClient(true);
#endif

  g_bIsFirstLoad = true; // Re-init global variable for Android

#if defined(WIN32) && !defined(_VISION_WINRT)
  char oldPath[FS_MAX_PATH];
  g_Config.ParseParams();
  if (g_Config.bShowHelp) // output help
  {
    printf("vSceneviewer parameters :\n");
    printf("  -help or ? : show this help text\n");
    printf("  -scene filename.vscene : the scene file to view. If not specified, a file open dialog appears\n");
    printf("  -mesh filename.vmesh : the vmesh file to view. If not specified, a file open dialog appears\n");
    printf("  -zone filename.vzone : the vzone file to view. If not specified, a file open dialog appears\n");
    printf("  -prefab filename.vprefab : the binary vprefab file to view. If not specified, a file open dialog appears\n");
    printf("  -lit filename.lit : views the static lighting info as passed to vLux. If not specified, a file open dialog appears\n");
    printf("  -res x y : screen resolution (x;y)\n");
    printf("  -fullscreen : run in fullscreen mode\n");
    printf("  -windowed : run in windowed mode (default)\n");
    printf("  -askfullscreen : prompts for 'run in windowed mode'\n");
    printf("  -freecam : forces to use a free camera\n");
    printf("  -vsync : lock framerate to vertical sync\n");
    printf("  -nocam : forces to use no free camera\n");
    printf("           If neither -freecam nor -nocam is specified the scene viewer creates\n"); 
    printf("           a free camera if no custom camera is attached after scene loading\n");
    printf("  -af x: enable anisotropic filtering with the specified filtering level\n");
    printf("  -cleancopy targetfolder: write a .bat file that enumerates all files with copy operations to the targetfolder that have been accessed through the file manager\n");
    printf("  -varkeys semi;colon;sepa;rated;list : Adds all the provided Variant Keys, for testing scenes with custom Variant Keys.\n");

    if (g_Config.bHelpOnly)
      return 0; // exit application
  }

  // install a file manager that protocols the file accesses and writes unique files into a batch file
  if (!g_Config.sCleanCopyFolder.IsEmpty())
    Vision::File.SetManager(new CopyFileManager());

#endif //WIN32
  // Create an application
  g_spApp = new VSceneViewerApp();

  strcpy(g_sceneName, "dummy.vscene");
   
  if (VisSampleApp::IsFileServerInstalled())
  {
    strcpy(g_scenePath, "~/Data/Vision/Tools/vSceneViewer");
  }
  else
  {
    strcpy(g_scenePath, VSCENEVIEWER_DATA_PATH);
  }


#if defined(WIN32) && !defined(_VISION_WINRT)

  if (g_Config.HasSceneName())
  {
    VFileHelper::GetFileDirEx(g_Config.sSceneName,g_scenePath);
    strcpy(g_sceneName,VFileHelper::GetFilename(g_Config.sSceneName)); 
  } 
  else
  {
    strcpy(oldPath,".");
    strcpy(g_scenePath,oldPath);
    strcpy(g_sceneName,"*.vscene;*.vmesh;*.vzone;*.vprefab;*.lit");
    if (!LoadWorldDialog(g_sceneName, g_scenePath))
    {
      Vision::Plugins.UnloadAllEnginePlugins();
      return 0;
    }
    g_Config.sSceneName = g_sceneName;
    g_Config.SetFileTypeFromSceneFileName();
  }

  // output log settings
  g_Config.ToLog();
#endif

  if (VFileHelper::FindProjectDir(g_scenePath,g_projectPath))
  {
    // make the scene name relative to project path
    char szAbsFile[FS_MAX_PATH];
    VFileHelper::CombineDirAndFile(szAbsFile,g_scenePath,g_sceneName);
    strcpy(g_sceneName,&szAbsFile[strlen(g_projectPath)+1]);
  }
  else
  {
    strcpy(g_projectPath,g_scenePath);
  }

  // Setup data dir so it's valid for plugin and scene loading
  AssetSettings::SetUseAlternativeFiles(false); //Don't use old file name hacks; use only variant keys
  Vision::File.SetAssetProfile(NULL); //Use default profile to start with
  Vision::File.AddDataDirectory("~");
  Vision::File.AddDataDirectory( g_projectPath );
  Vision::Error.SystemMessage("vSceneViewer: Using project directory %s", &g_projectPath);
  Vision::Editor.SetProjectPath(g_projectPath); // useful to globally access project path

#if defined(WIN32) && !defined(_VISION_WINRT)
  SetVariantKeys(g_Config.sVariantKeys);
#endif

#if defined(_DLL) && !defined(VBASE_LIB)
  //Load plugins if we're loading a scene file and not just a mesh
  if (g_Config.bIsVScene || g_Config.bIsVZone || g_Config.bIsVPrefab)
  {
    if (g_Config.bIsVScene)
    {
      // Determine the actual asset profile in order to find the scene file for plugin loading
      VisSampleApp::SelectAssetProfile(g_sceneName, true);

      //Load the plugins that the scene referenced
      g_spApp->GetSceneLoader()->LoadEnginePlugins(g_sceneName,Vision::File.GetManager());

      // Process the manifest file, loading the plugins referenced there
      VisSampleApp::ProcessManifestFile(false, true);
    }

    //Always load the remote input plugin to enable starting the input server from scene scripts
    VISION_PLUGIN_ENSURE_LOADED(vRemoteInput);
  }
#else

  //Link these plugins so we can view the sample maps
  VISION_PLUGIN_ENSURE_LOADED(VisionEnginePlugin);

  #if !defined(_VISION_WINRT)
    VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);
  #endif

  // Plugins currently not available on Vision Mobile
  #if !defined(_VISION_MOBILE)
    VISION_PLUGIN_ENSURE_LOADED(SampleGamePlugin); 
    #pragma message ( "If you do not have any samples installed, please remove the following library dependencies (and this line): SampleGamePlugin*.lib" )
  #endif
  VISION_PLUGIN_ENSURE_LOADED(vHavok);

  #if defined ( HAVOK_AI_KEYCODE )
    VISION_PLUGIN_ENSURE_LOADED(vHavokAi);
  #else
    #pragma message ( "No Havok AI Keycode found. If you do not have a license for Havok AI, please remove the following library dependencies (and this line): vHavokAi*.lib, hkaiPathfinding.lib, hkaiInternal.lib, hkaiVisualize.lib, hkaiSpatialAnalysis.lib, hkaiPhysics2012Bridge.lib, as well as any platform-specific hkai* libraries.")
  #endif

  #if defined ( HAVOK_BEHAVIOR_KEYCODE )
    VISION_PLUGIN_ENSURE_LOADED(vHavokBehavior);
  #else
    #pragma message ( "No Havok Behavior Keycode found. If you do not have a license for Havok Behavior, please remove the following library dependencies (and this line): vHavokBehavior*.lib, hkbBehavior.lib, hkbInternal.lib, hkbUtilities.lib, hkbPhysics2012Bridge.lib, hkbScript.lib, as well as any platform-specific hkb* libraries.")
  #endif

#if !defined( HK_ANARCHY )
  #if defined ( HAVOK_DESTRUCTION_2012_KEYCODE )
    VISION_PLUGIN_ENSURE_LOADED(vHavokDestruction);
  #else
    #pragma message ( "No Havok Destruction Keycode found. If you do not have a license for Havok Destruction, please remove the following library dependencies (and this line): vHavokDestruction*.lib, hkdDestruction.lib, hkdInternal.lib, hkgpConvexDecomposition.lib, as well as any platform-specific hkd* libraries.")
  #endif

  #if defined ( HAVOK_CLOTH_KEYCODE )
    VISION_PLUGIN_ENSURE_LOADED(vHavokCloth);
  #else
    #pragma message ( "No Havok Cloth Keycode found. If you do not have a license for Havok Cloth, please remove the following library dependencies (and this line): vHavokCloth*.lib, hclCloth.lib, hclInternal.lib, hclSetup.lib, hclAnimationBridge.lib, hclPhysics2012Bridge.lib, as well as any platform-specific hcl* libraries.")
  #endif
   
  #if defined(WIN32)
    #if defined ( HAVOK_SIMULATION_KEYCODE )
      VISION_PLUGIN_ENSURE_LOADED(vHavokVehicle);
      VISION_PLUGIN_ENSURE_LOADED(vHavokCharacter);
    #else
      #pragma message ( "No Havok Simulation found. If you do not have a license for Havok Simulation, please remove the following library dependencies (and this line): hkmsCharacter.lib, hkmsVehicle.lib, hkmsCommon.lib" )
   #endif
  #endif   
    
  // Plugins currently not available on Vision Mobile and WinRT
  #if !defined(_VISION_MOBILE) && !defined(_VISION_WINRT) && !defined(_VISION_PSP2)
    VISION_PLUGIN_ENSURE_LOADED(Speedtree5EnginePlugin);
    #pragma message ( "If you do not have a license for SpeedTree, please remove the following library dependencies (and this line): Speedtree5EnginePlugin*.lib" )
  #endif   
#endif // !defined( HK_ANARCHY )
    
#endif // _DLL

#if defined(WIN32) && !defined(_VISION_WINRT)
  char szTitle[FS_MAX_PATH];

#ifdef _VR_DX9
  sprintf(szTitle,"Scene Viewer (DX9) [%s]",g_Config.sSceneName.AsChar());
#elif defined(_VR_DX11)
  sprintf(szTitle,"Scene Viewer (DX11) [%s]",g_Config.sSceneName.AsChar());
#else
  sprintf(szTitle,"Scene Viewer [%s]",g_Config.sSceneName.AsChar());
#endif
  g_spApp->m_appConfig.m_videoConfig.m_szWindowTitle = szTitle;
  g_spApp->m_appConfig.m_videoConfig.m_iAdapter = g_Config.iAdapter;

  // Force to single threaded for testing here by setting 0 to 1
  if (!g_spApp->InitSample("Scene Viewer", NULL, g_Config.GetSampleFlags() | VSAMPLE_NO_FALLBACK_ASSETPROFILE, g_Config.iResX,g_Config.iResY, VAPP_INIT_DEFAULTS, 0))
#elif defined(_VISION_PSP2)
  if (!g_spApp->InitSample(NULL, NULL, VSAMPLE_INIT_DEFAULTS | VSAMPLE_WAITRETRACE | VSAMPLE_NO_FALLBACK_ASSETPROFILE))
#elif defined(_VISION_MOBILE)
  if (!g_spApp->InitSample(NULL, NULL, VSAMPLE_INIT_DEFAULTS | VSAMPLE_NO_FALLBACK_ASSETPROFILE))
#else
  if(!g_spApp->InitSample())
#endif
  {
    // Unload all plugins manually as sample application was not initialized
    // and DeinitSample(() can thus not be called.
    Vision::Plugins.UnloadAllEnginePlugins();
    return false;
  }

#if defined(WIN32) && !defined(_VISION_WINRT)
  if (g_Config.iAnisotropicFilteringLevel > 0)
  {
    Vision::Renderer.SetDefaultTextureFilterMode(FILTER_ANISOTROPIC);
    Vision::Renderer.SetMaxAnisotropy((float)g_Config.iAnisotropicFilteringLevel);
  }
#endif

  // remove the common data dir since it should not be necessary for the scene viewer
  if (VFileHelper::ExistsDir(VISION_COMMON_DATA))
  {
    char szCommonDir[FS_MAX_PATH];
    Vision::File.GetAbsoluteDirectory(szCommonDir, VISION_COMMON_DATA);
    Vision::File.RemoveDataDirectory( szCommonDir );
  }

  // Data dir is reset by engine init. Lets add it again
  Vision::File.AddDataDirectory( g_projectPath );

#if defined(WIN32) && !defined(_VISION_WINRT) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
  // shading mode related - prepare shading modes:
  EnumShadingModes(g_shadingEffects);
#endif

  // Record the base data directory configuration in order to restore it before loading a new scene.
  RecordDataDirectoryConfiguration();


#if defined(WIN32) && !defined(_VISION_WINRT)
  if(g_Config.bIsVMesh )
  {
    // load vmesh
    VSceneLoader::ClearScene();
    VisStaticMesh_cl *pRes = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(g_sceneName);
    if (!pRes)
    {
      Vision::Error.FatalError("Error opening vmesh file '%s'!",g_sceneName);
      g_spApp->DeInitSample();
      return false;
    }
    else
    { //the mesh was loaded successfully

      //force loading of .colmesh file if it exists
      if (pRes->GetCollisionMesh(true)) 
        Vision::Error.SystemMessage("Loaded collision mesh");
      else
        Vision::Error.SystemMessage("No collision mesh loaded");

      // add a visibility zone and set the camera correctly

      // get a instance
      hkvMat4 mTransform;
      VisStaticMeshInstance_cl *pInst = pRes->CreateInstance(mTransform);

      // create the visibility zone
      hkvAlignedBBox bbox( pInst->GetBoundingBox().m_vMin, pInst->GetBoundingBox().m_vMax);
      int iZones = Vision::GetSceneManager()->GetNumVisibilityZones();
      VisVisibilityZone_cl* pVisZone;
      if (iZones>0)
      {
        pVisZone = Vision::GetSceneManager()->GetVisibilityZone(0);
      } else
      {
        pVisZone = new VisVisibilityZone_cl( bbox );
        Vision::GetSceneManager()->AddVisibilityZone( pVisZone );
      }
      // add submeshes to vis zone
      for (int i=0;i<pInst->GetSubmeshInstanceCount();i++)
      {
        pVisZone->AddStaticGeometryInstance(pInst->GetSubmeshInstance(i));
      }

      LookAtBox(bbox,g_vDefaultCamPos);
    }
  }
  else if(g_Config.bIsVZone )
  {
    VSceneLoader::ClearScene(); // start with empty world
    float fLarge = 100000000.f;
    hkvAlignedBBox large (hkvVec3 (-fLarge), hkvVec3 (fLarge));
    VisVisibilityZone_cl *pVisZone = new VisVisibilityZone_cl( large );
    Vision::GetSceneManager()->AddVisibilityZone( pVisZone );

    VisZoneResource_cl *pZone = VisZoneResourceManager_cl::GlobalManager().CreateZone(g_sceneName,large);
    pZone->EnsureLoaded();
    pZone->m_bHandleZone = false; // don't uncache

    hkvAlignedBBox scenebox;
    Vision::GetSceneManager()->GetSceneExtents(scenebox);
    if (scenebox.isValid())
      LookAtBox(scenebox,g_vDefaultCamPos);

  }
  else if(g_Config.bIsVPrefab )
  {
    VSceneLoader::ClearScene(); // start with empty world
    float fLarge = 100000000.f;
    hkvAlignedBBox large (hkvVec3 (-fLarge), hkvVec3 (fLarge));
    VisVisibilityZone_cl *pVisZone = new VisVisibilityZone_cl( large );
    Vision::GetSceneManager()->AddVisibilityZone( pVisZone );

    // load the prefab file and instantiate once
    VPrefab *pPrefab = VPrefabManager::GlobalManager().LoadPrefab(g_sceneName);
    VPrefabInstanceInfo info;
    if (!pPrefab->Instantiate(info))
      Vision::Error.SystemMessage("Failed to instantiate prefab");

    const float r = 100.f;
    hkvAlignedBBox scenebox;
    Vision::GetSceneManager()->GetSceneExtents(scenebox);
    if (!scenebox.isValid())
      scenebox.set(hkvVec3 (-r), hkvVec3 (r));
    LookAtBox(scenebox,g_vDefaultCamPos);

  }
  else if (g_Config.bIsLITFile)
  {
    LoadLitFile(g_sceneName);
  }   
  else if(g_Config.bIsVScene )
  {
#endif

    Vision::World.SetUseGeometryForVisibilityClassification(true);//FIXME

    if (!g_spApp->LoadScene(g_sceneName, 0, true))
    {
      if(!g_spApp->GetSceneLoader()->IsAborted())
      {
        Vision::Error.FatalError("Error opening scene file '%s' :\n%s",g_sceneName,g_spApp->GetSceneLoader()->GetLastError());
      }

      g_spApp->DeInitSample();
    
      return false;
    }

#if defined(WIN32) && !defined(_VISION_WINRT)
  }
  else
  {
    Vision::Error.FatalError("Unsupported file format!");
    return false;
  }
#endif

  // Always enable the VDB 
#ifdef ENABLE_VISUAL_DEBUGGER
  if(vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>(Vision::GetApplication()->GetPhysicsModule()))
  {
    pPhysicsModule->SetEnabledVisualDebugger(TRUE);
  }
#endif

#if defined(WIN32) && !defined(_VISION_WINRT)

  VisSampleApp::GetInputMap()->MapTrigger(SCENE_RETURN_TO_ORIGIN, VInputManagerPC::GetKeyboard(), CT_KB_HOME, VInputOptions::Once(ONCE_ON_PRESS, 0));
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_TOGGLE_MOUSE_CAPTURE, VInputManagerPC::GetKeyboard(), CT_KB_M, VInputOptions::Once(ONCE_ON_PRESS, 0));
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_TOGGLE_RENDERCOLLISIONMESH, VInputManagerPC::GetKeyboard(), CT_KB_C, VInputOptions::Once(ONCE_ON_PRESS, 0));
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_NEXT_SHADINGMODE, VInputManagerPC::GetKeyboard(), CT_KB_V, VInputOptions::Once(ONCE_ON_PRESS, 0));

  g_spApp->AddHelpText("HOME - Return to scene origin.");
  g_spApp->AddHelpText("M - Release mouse.");
  g_spApp->AddHelpText("C - Render collision mesh");
  g_spApp->AddHelpText("V - Toggle through vForge debug shading modes");

  g_spApp->AddHelpText("");

  g_spApp->AddHelpText("Camera:");
  g_spApp->AddHelpText("  CURSOR KEYS - fly through scene");
  g_spApp->AddHelpText("  MOUSE MOVEMENT - look around");
  g_spApp->AddHelpText("  RIGHT MOUSE BUTTON - move 2 times faster");
  g_spApp->AddHelpText("  PRESS MOUSE WHEEL - move 4 times faster");


#elif (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))

  VisSampleApp::GetInputMap()->MapTrigger(SCENE_TOGGLE_MENU,    VInputManagerXenon::GetPad(0), CT_PAD_Y, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_LOAD_NEW_SCENE, VInputManagerXenon::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());

  // define help text
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("How to use the Vision Scene Viewer:");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("PAD 1 - Y : Shows the available scenes you can view");
  g_spApp->AddHelpText("PAD 1 - B : Select next scene");
  g_spApp->AddHelpText("PAD 1 - X : Select previous scene");
  g_spApp->AddHelpText("PAD 1 - RIGHT SHOULDER : View selected scene");
  

#elif defined(_VISION_XENON)

  VisSampleApp::GetInputMap()->MapTrigger(SCENE_TOGGLE_MENU,    VInputManagerXenon::GetPad(0), CT_PAD_Y, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_LOAD_NEW_SCENE, VInputManagerXenon::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_NEXT_SHADINGMODE, VInputManagerXenon::GetPad(0), CT_PAD_LEFT_TRIGGER, VInputOptions::Once());

  // define help text
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("How to use the Vision Scene Viewer:");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("PAD 1 - Y : Shows the available scenes you can view");
  g_spApp->AddHelpText("PAD 1 - B : Select next scene");
  g_spApp->AddHelpText("PAD 1 - X : Select previous scene");
  g_spApp->AddHelpText("PAD 1 - RIGHT SHOULDER : View selected scene");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("PAD 1 - LEFT TRIGGER : Next shading mode");


#elif defined(_VISION_PS3)

  VisSampleApp::GetInputMap()->MapTrigger(SCENE_TOGGLE_MENU,  VInputManagerPS3::GetPad(0), CT_PAD_TRIANGLE, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_LOAD_NEW_SCENE, VInputManagerPS3::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_NEXT_SHADINGMODE, VInputManagerPS3::GetPad(0), CT_PAD_LEFT_TRIGGER, VInputOptions::Once());

  // define help text
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("How to use the Vision Scene Viewer:");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("PAD 1 - TRIANGLE : Shows the available scenes you can view");
  g_spApp->AddHelpText("PAD 1 - CIRCLE : Select next scene");
  g_spApp->AddHelpText("PAD 1 - SQUARE : Select previous scene");
  g_spApp->AddHelpText("PAD 1 - RIGHT SHOULDER : View selected scene");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("PAD 1 - LEFT TRIGGER : Next shading mode");


#elif defined(_VISION_PSP2)

  VisSampleApp::GetInputMap()->MapTrigger(SCENE_TOGGLE_MENU,  VInputManagerPSP2::GetPad(0), CT_PAD_TRIANGLE, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_LOAD_NEW_SCENE, VInputManagerPSP2::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  // Enable this line to allow switching rendering modes on Vita (disabled since some functions like the character controller etc. use the same buttons)
  // VisSampleApp::GetInputMap()->MapTrigger(SCENE_NEXT_SHADINGMODE, VInputManagerPSP2::GetPad(0),   CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  // define help text
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("How to use the Vision Scene Viewer:");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("PAD 1 - TRIANGLE : Shows the available scenes you can view");
  g_spApp->AddHelpText("PAD 1 - CIRCLE : Select next scene");
  g_spApp->AddHelpText("PAD 1 - SQUARE : Select previous scene");
  g_spApp->AddHelpText("PAD 1 - RIGHT SHOULDER : View selected scene");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("PAD 1 - LEFT SHOULDER : Next shading mode");


#elif defined(_VISION_WIIU)

  VisSampleApp::GetInputMap()->MapTrigger(SCENE_TOGGLE_MENU,  VInputManager::GetDRC(V_DRC_FIRST), CT_PAD_X, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_LOAD_NEW_SCENE, VInputManager::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  VisSampleApp::GetInputMap()->MapTrigger(SCENE_NEXT_SHADINGMODE, VInputManager::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_TRIGGER, VInputOptions::Once());

  // define help text
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("How to use the Vision Scene Viewer:");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("DRC - X : Shows the available scenes you can view");
  g_spApp->AddHelpText("DRC - A : Select next scene");
  g_spApp->AddHelpText("DRC - Y : Select previous scene");
  g_spApp->AddHelpText("DRC - RIGHT SHOULDER : View selected scene");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("DRC - LEFT TRIGGER : Next shading mode");


#elif defined(_VISION_MOBILE)

  // TODO: help text

#endif
  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  if(g_bIsFirstLoad)
  {
    g_vDefaultCamPos = g_spApp->GetSceneLoader()->m_vDefaultCamPos;
    g_mDefaultCamRot = g_spApp->GetSceneLoader()->m_DefaultCamRot;

    g_bIsFirstLoad = false;

#if defined(WIN32) && !defined(_VISION_WINRT)
    // create camera (and optional start position)
    bool bCreateCamera = g_Config.bForceFreeCam;
    if (!bCreateCamera && !g_Config.bForceNoCam)
    {
      g_pCamera = vdynamic_cast<VisBaseEntity_cl*>(VisRenderContext_cl::GetMainRenderContext()->GetCamera()->GetParent());
      if (!g_pCamera) // camera not attached to any entity, so allow to create a free camera
        bCreateCamera = true;
    }
    if (bCreateCamera)
    {
      g_pCamera = g_spApp->EnableMouseCamera(); 

      // if a free camera is used, use the camera position & orientation at export time
      g_pCamera->SetPosition(g_vDefaultCamPos);
      g_pCamera->SetRotationMatrix(g_mDefaultCamRot);

      VisMouseCamera_cl* pMouseCam = vdynamic_cast<VisMouseCamera_cl*>(g_pCamera);

      if(pMouseCam)
      {
        //add key events to control the mouseview camera
        pMouseCam->GetInputMap()->MapTrigger(API_CAMERA_MOVE_FORWARD, V_KEYBOARD, CT_KB_W);
        pMouseCam->GetInputMap()->MapTrigger(API_CAMERA_MOVE_BACKWARD,V_KEYBOARD, CT_KB_S);
        pMouseCam->GetInputMap()->MapTrigger(API_CAMERA_MOVE_LEFT,    V_KEYBOARD, CT_KB_A);
        pMouseCam->GetInputMap()->MapTrigger(API_CAMERA_MOVE_RIGHT,   V_KEYBOARD, CT_KB_D);
        pMouseCam->GetInputMap()->MapTrigger(API_CAMERA_MOVE_UP,      V_KEYBOARD, CT_KB_E);
        pMouseCam->GetInputMap()->MapTrigger(API_CAMERA_MOVE_DOWN,    V_KEYBOARD, CT_KB_Q);
      }
    }

#elif defined(_VISION_XENON) 

    // load some GUI resources
    VGUIManager::GlobalManager().LoadResourceFile("Dialogs\\MenuSystem.xml");
    g_spGUIContext = new VGUIMainContext(NULL);
    g_spGUIContext->SetActivate(true);

    // start the main menu
#if defined(_VISION_WINRT)
    g_spMainDlg = (SceneViewerMainMenu *)g_spGUIContext->ShowDialog(".\\EngineSamples\\Data\\vSceneViewer\\Dialogs\\MainMenuXbox360.xml");
#else
    g_spMainDlg = (SceneViewerMainMenu *)g_spGUIContext->ShowDialog("D:\\Data\\Vision\\Tools\\vSceneViewer\\Dialogs\\MainMenuXbox360.xml");
#endif
    VASSERT(g_spMainDlg);

    VListControl *pListCtrl = (VListControl*) g_spMainDlg->Items ().FindItem (g_spMainDlg->GetMenuManager ()->GetID("SCENE_LIST"));
    LoadSceneConfig (TARGETDEVICE_XBOX360, "D:\\Data\\Vision\\Tools\\vSceneViewer\\Dialogs\\SceneConfig.lua", pListCtrl);
    pListCtrl->SetSelectionIndex(0);

#elif defined(_VISION_PS3)

    // load some GUI resources
    VGUIManager::GlobalManager().LoadResourceFile("Dialogs/MenuSystem.xml");
    g_spGUIContext = new VGUIMainContext(NULL);
    g_spGUIContext->SetActivate(true);

    // start the main menu
    g_spMainDlg = (SceneViewerMainMenu *)g_spGUIContext->ShowDialog("/app_home/Data/Vision/Tools/vSceneViewer/Dialogs/MainMenuPS3.xml");
    VASSERT(g_spMainDlg);

    VListControl *pListCtrl = (VListControl*) g_spMainDlg->Items ().FindItem (g_spMainDlg->GetMenuManager ()->GetID("SCENE_LIST"));
    LoadSceneConfig (TARGETDEVICE_PS3, "/app_home/Data/Vision/Tools/vSceneViewer/Dialogs/SceneConfig.lua", pListCtrl);
    pListCtrl->SetSelectionIndex(0);

#elif defined(_VISION_PSP2)

    // load some GUI resources
    VGUIManager::GlobalManager().LoadResourceFile("Dialogs/MenuSystem.xml");
    g_spGUIContext = new VGUIMainContext(NULL);
    g_spGUIContext->SetActivate(true);

    // start the main menu
    g_spMainDlg = (SceneViewerMainMenu *)g_spGUIContext->ShowDialog("app0:Data/Vision/Tools/vSceneViewer/Dialogs/MainMenuPSVita.xml");
    VASSERT(g_spMainDlg);

    VListControl *pListCtrl = (VListControl*) g_spMainDlg->Items ().FindItem (g_spMainDlg->GetMenuManager ()->GetID("SCENE_LIST"));
    LoadSceneConfig (TARGETDEVICE_PSP2, "app0:Data/Vision/Tools/vSceneViewer/Dialogs/SceneConfig.lua", pListCtrl);
    pListCtrl->SetSelectionIndex(0);

#elif defined(_VISION_MOBILE)

    const int width = VVideo::GetVideoConfig()->ViewWidth;
    const int height = VVideo::GetVideoConfig()->ViewHeight;

    // load some GUI resources
    VGUIManager::GlobalManager().LoadResourceFile("Dialogs/MenuSystem.xml");
    g_spGUIContext = new VGUIMainContext(NULL);
    g_spApp->SetGUIContext(g_spGUIContext); // Make sure to use the same GUI context for the whole sample app
    g_spGUIContext->SetActivate(true);  

    // start the main menu
#if defined (_VISION_WINRT) 
    const char* dlgPath = "Data/vSceneViewer/Dialogs/MainMenuMobile.xml";
#else
    const char* dlgPath = "Dialogs/MainMenuMobile.xml";
#endif
    g_spMainDlg = (SceneViewerMainMenu *)g_spGUIContext->ShowDialog(dlgPath);
    VASSERT(g_spMainDlg);

    VListControl *pListCtrl = (VListControl*) g_spMainDlg->Items ().FindItem (g_spMainDlg->GetMenuManager ()->GetID("SCENE_LIST"));

    const char* sceneList = "Dialogs/SceneConfig.lua";

#if defined(_VISION_IOS)
    LoadSceneConfig (TARGETDEVICE_IOS, sceneList, pListCtrl);
#elif defined(_VISION_ANDROID)
    LoadSceneConfig (TARGETDEVICE_ANDROID, sceneList, pListCtrl);
#elif defined (_VISION_WINRT) // Apollo, Metro
	  LoadSceneConfig (TARGETDEVICE_DX11, sceneList, pListCtrl); 
#endif

    // Pull up the list control to full size
    g_spMainDlg->m_pSceneList->SetSize(width - 74, height - 128);

    // Move button to lower left
    if(g_spMainDlg->m_pLoadButton != NULL)
    {
      g_spMainDlg->m_pLoadButton->SetPosition(32, height - 32);
    }

    if(g_spMainDlg->m_pContinueButton != NULL)
    {
      g_spMainDlg->m_pContinueButton->SetPosition(150, height - 32);
      g_spMainDlg->m_pContinueButton->SetVisible(false);
    }

    VSampleTouchMenu* pTouchMenu = NULL;
    VSimpleSampleTouchButton* pMenuToggleTouch = NULL;

#elif defined(_VISION_WIIU)

    // load some GUI resources
    VGUIManager::GlobalManager().LoadResourceFile("Dialogs\\MenuSystem.xml");
    g_spGUIContext = new VGUIMainContext(NULL);

    VisRenderContext_cl* pDRCContext = g_spApp->GetDRCRenderContext();
    int iWidth, iHeight;
    pDRCContext->GetSize(iWidth, iHeight);
    g_spGUIContext->SetClientRect(VRectanglef(0, 0, static_cast<float>(iWidth), static_cast<float>(iHeight)));

    pDRCContext->SetRenderFilterMask(VIS_ENTITY_VISIBLE_IN_TEXTURE);
    g_spGUIContext->SetVisibleBitmask(VIS_ENTITY_VISIBLE_IN_TEXTURE);
    g_spGUIContext->SetActivate(true);

    // start the main menu
    g_spMainDlg = (SceneViewerMainMenu *)g_spGUIContext->ShowDialog("/vol/content/VisionRoot/Data/Vision/Tools/vSceneViewer/Dialogs/MainMenuWiiU.xml");
    VASSERT(g_spMainDlg);

    VListControl *pListCtrl = (VListControl*) g_spMainDlg->Items ().FindItem (g_spMainDlg->GetMenuManager ()->GetID("SCENE_LIST"));
    LoadSceneConfig (TARGETDEVICE_WIIU, "/vol/content/VisionRoot/Data/Vision/Tools/vSceneViewer/Dialogs/SceneConfig.lua", pListCtrl);
    pListCtrl->SetSelectionIndex(0);

#endif

    // switch to zone debug output in case the scene uses streaming zones
    if (VisZoneResourceManager_cl::GlobalManager().GetResourceCount()>0)
      Vision::Profiling.SetDebugRenderFlags(DEBUGRENDERFLAG_ZONES);

  }
  else
  {
    g_pCamera = vdynamic_cast<VisBaseEntity_cl*>(VisRenderContext_cl::GetMainRenderContext()->GetCamera()->GetParent());
    if (!g_pCamera) // camera not attached to any entity, so allow to create a free camera
    {
      g_pCamera = g_spApp->EnableMouseCamera();
      // if a free camera is used, use the camera position & orientation at export time
      g_pCamera->SetPosition(g_spApp->GetSceneLoader()->m_vDefaultCamPos);
      g_pCamera->SetRotationMatrix(g_spApp->GetSceneLoader()->m_DefaultCamRot);
    }

#if !defined(WIN32) || defined(_VISION_WINRT)
    g_spMainDlg->ToggleView(g_pCamera);
#endif
  }

#if defined(WIN32) && !defined(_VISION_WINRT) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
  // shading mode related - backup renderer node:
  g_spRendererNode = Vision::Renderer.GetRendererNode(0);
#endif
}

VISION_SAMPLEAPP_RUN
{
  if(!g_spApp->Run ())
    return false;

  if (g_bRenderCollisionMesh)
    Vision::Physics.RenderAllCollisionMeshes();

#if defined(WIN32) && defined(SUPPORTS_KEYBOARD) && !defined(_VISION_WINRT) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)

  if (
    VisSampleApp::GetInputMap()->GetTrigger(SCENE_NEXT_SHADINGMODE)
    #if !defined(WIN32)
    && !g_spMainDlg->IsVisible()
    #endif
    )
  {
    g_iCurrentShadingMode++;
    if (g_iCurrentShadingMode>=g_shadingEffects.Count())
    {
      // reset to original rendering
      g_iCurrentShadingMode = -1;
      Vision::RenderLoopHelper.SetReplacementRenderLoop(NULL);
      Vision::Renderer.SetRendererNode(0,g_spRendererNode);
      Vision::Message.Add("Switched back to standard scene rendering");
    } 
    else
    {
      if (g_spShadingModeRenderLoop==NULL)
        g_spShadingModeRenderLoop = new VForgeDebugRenderLoop();
      g_spShadingModeRenderLoop->SetEffect(g_shadingEffects.GetAt(g_iCurrentShadingMode));
      VSimpleRendererNode* pSimpleRendererNode = new VSimpleRendererNode(Vision::Contexts.GetMainRenderContext());
      pSimpleRendererNode->InitializeRenderer();
      Vision::Renderer.SetRendererNode(0, pSimpleRendererNode);
      Vision::RenderLoopHelper.SetReplacementRenderLoop(g_spShadingModeRenderLoop);
    }
  }
  if (g_iCurrentShadingMode>=0)
  {
    const char *szNiceName = (const char *)g_shadingEffects.GetAt(g_iCurrentShadingMode)->GetUserData();
    VASSERT(szNiceName);
    Vision::Message.Print(1,10,Vision::Video.GetYRes()-20,"Scene shading mode: %s",szNiceName);
  }
#endif

#if defined(WIN32) && defined(SUPPORTS_KEYBOARD) && !defined(_VISION_WINRT) 
  if (VisSampleApp::GetInputMap()->GetTrigger(SCENE_TOGGLE_RENDERCOLLISIONMESH))
  {
    g_bRenderCollisionMesh = !g_bRenderCollisionMesh;
  }

  if(VisSampleApp::GetInputMap()->GetTrigger(SCENE_RETURN_TO_ORIGIN))
  {
    if(g_pCamera) g_pCamera->SetPosition(0,0,0);
  }

  if(VisSampleApp::GetInputMap()->GetTrigger(SCENE_TOGGLE_MOUSE_CAPTURE))
  {
    g_Config.bMouseReleased = !g_Config.bMouseReleased;

    if(g_Config.bMouseReleased)
    {
      Vision::Message.Add(1," Press 'M' to recapture the mouse.");
      DEINIT_MOUSE_IF_AVAILABLE();
    } else 
    { 
      INIT_MOUSE_IF_AVAILABLE();
    }
  }
#endif

#if !defined(WIN32)
  // Toggling is handled here instead of the tick function of the
  // dialog system since we have to pass the correct camera.
  if (VisSampleApp::GetInputMap()->GetTrigger(SCENE_TOGGLE_MENU))
  {
    if (g_pCamera != NULL)
      g_spMainDlg->ToggleView(g_pCamera);
  }

#ifdef _VISION_IOS
  if(g_spMainDlg->IsVisible())
  {
    // Remove touch menu
    g_spTouchMenu = NULL;
  }
  else
  {
    // Enable touch menu after leaving loading menu
    if(g_spTouchMenu == NULL)
    {
      g_spTouchMenu = new VSampleTouchMenu(*VisSampleApp::GetInputMap(), VSampleTouchMenu::VO_TOP);
      g_pMenuToggleTouch = g_spTouchMenu->AddSimpleButton("Textures/Touch/folder_out.tga", SCENE_TOGGLE_MENU, true);
      g_spTouchMenu->RecalculatePosition();
    }
  }
#endif

  if ( g_spMainDlg->IsLoadNewScene() && g_spMainDlg->GetScenePath() != NULL )
  {
    // Once a scene has been loaded, the continue button should be visible in the menu
    if(g_spMainDlg->m_pContinueButton != NULL)
    {
      g_spMainDlg->m_pContinueButton->SetVisible(true);
    }

#if defined( USE_FILESERVE )
    if (VisSampleApp::IsFileServerInstalled())
    {
      VFileServeStreamManager* pFileServe = (VFileServeStreamManager*) Vision::File.GetManager ();
      pFileServe->SetSceneName(VPathHelper::GetFilename(g_spMainDlg->GetScenePath()));
      pFileServe->SendSceneChangedEvent();
    }
#endif

#ifdef _VISION_IOS
    // Remove touch menu when loading
    g_spTouchMenu = NULL;
#endif

    g_spApp->ClearScene();        // cleanup everything, wait for pending zones

    Vision::Renderer.SetRendererNode(0, NULL);
#if defined (WIN32) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
    g_spRendererNode = NULL;
#endif

    hkvVec3 dummy;
    VisRenderContext_cl::GetMainRenderContext()->GetCamera()->AttachToEntity(NULL, dummy);

    // Restore the initial data directory configuration, i.e. remove all directories which do not belong to the base configuration
    RestoreDataDirectoryConfiguration();

#if defined( USE_FILESERVE )
    if (VisSampleApp::IsFileServerInstalled())
    {
      // re-add vSceneViewer data directory (dialogs / mouse cursors) *after* the data directory has been restored
      Vision::File.AddDataDirectory(VSCENEVIEWER_DATA_PATH); 
    }
#endif

    // get the new scene path
    strcpy(g_scenePath,g_spMainDlg->GetScenePath());

    char path[256];
    char* pCurrentPosition = g_scenePath;
    char* pNextPosition = strchr( pCurrentPosition, ';' );
    while ( pNextPosition != NULL)
    {
      int length = (int)(pNextPosition - pCurrentPosition);
      strncpy(path, pCurrentPosition, length);
      path[length] = 0;
      Vision::File.AddDataDirectory( path );
      pCurrentPosition = pNextPosition + 1;
      pNextPosition = strchr( pCurrentPosition, ';' );
    }

    VFileHelper::GetFileDirEx(pCurrentPosition,path);
    Vision::File.AddDataDirectory( path );

    // Note: also add parent directory, in case if scenes are in sub-folder with respect to .project file
    strcat(path, VFILE_STR_SEPARATOR);
    VFileHelper::GetParentDir(path);
    Vision::File.AddDataDirectory( path );

    // load the scene file
    strcpy(g_sceneName,VFileHelper::GetFilename(pCurrentPosition));



    if (!g_spApp->LoadScene(g_sceneName))
    {
      Vision::Error.FatalError("Error opening scene file '%s' :\n%s",g_sceneName, g_spApp->GetSceneLoader()->GetLastError());
    }

    VListControl *pListCtrl = (VListControl*) g_spMainDlg->Items().FindItem (g_spMainDlg->GetMenuManager ()->GetID("SCENE_LIST"));

    // On mobile we reset the selection so the items don't appear highlighted
    #ifdef _VISION_MOBILE
    if ( pListCtrl )
    {
      pListCtrl->SetSelectionIndex(-1);
    }
    #endif

#ifdef USE_FILESERVE
    if (VisSampleApp::IsFileServerInstalled())
    {
      // Re-add the scene viewer data directory as it has been removed by the change scene event.
      // This only becomes important if the app is suspended and resumed as it will then try to reload
      // the mouse cursor and the sample icons.
      Vision::File.AddDataDirectory( "~/Data/Vision/Tools/vSceneViewer" );
    }
#endif

    g_spMainDlg->ResetLoadNewScene();
  }
#endif
  return true;
}

VISION_DEINIT
{

#ifdef USE_FILESERVE
  if (VisSampleApp::IsFileServerInstalled())
  {
    VFileServeStreamManager* pFileServe = static_cast<VFileServeStreamManager*>(Vision::File.GetManager ());
    pFileServe->SendSceneChangedEvent ();
  }
#endif

#if defined(WIN32) && !defined(_VISION_WINRT) || defined (_VISION_XENON) || defined (_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
  // deinit shading modes related settings
  Vision::RenderLoopHelper.SetReplacementRenderLoop(NULL);
  g_spShadingModeRenderLoop = NULL;
  g_spRendererNode = NULL;
  g_shadingEffects.Clear();
  g_spShadingShaderLib = NULL;
#endif

#ifdef _VISION_IOS
  g_spTouchMenu = NULL;
#endif

#if !defined(WIN32)
  // Deinit GUI
  g_spMainDlg = NULL;
  g_spGUIContext->SetActivate(false);
  g_spGUIContext = NULL;
#endif

  g_pCamera = NULL;

  VGUIManager::GlobalManager().CleanupResources();

  g_spApp->UnloadScene();
  g_spApp->DeInitSample();
  g_spApp = NULL;

  return true;
}

VISION_MAIN_DEFAULT

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
