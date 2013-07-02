/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#include <Vision/Runtime/Base/VBase.hpp>

#include <Vision/Runtime/Common/VisSampleApp.hpp>

#include <Vision/Runtime/Base/System/IO/Stream/IVFileStream.hpp>
#include <Vision/Runtime/Base/System/Memory/Manager/VSmallBlockMemoryManager.hpp>
#include <Vision/Runtime/Base/System/Threading/StreamProcessor/vstreamprocessor.hpp>

#include <Vision/Runtime/Engine/Application/VisionAppHelpers.hpp>
#include <Vision/Runtime/Engine/Application/VisProfilingMenu.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiSimpleRendererNode.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiScreenMask.hpp>
#include <Vision/Runtime/Engine/System/VisApiManifest.hpp>
#include <Vision/Runtime/Engine/System/Resource/IVisApiBackgroundResourceRestorer.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/BufferResolver.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/ToneMapping.hpp>

#if defined(_VISION_MOBILE)
  #include <Vision/Runtime/Common/VisMobileExitDialog.hpp>
#endif

#ifdef SUPPORTS_SHADOW_MAPS
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/IVShadowMapComponent.hpp>
  #if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
    #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VMobileShadowMapComponentSpotDirectional.hpp>
  #else
    #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentPoint.hpp>
    #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentSpotDirectional.hpp>
#endif
#endif

#if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>
#elif defined( SUPPORTS_RENDERERNODES )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ForwardRenderer/ForwardRenderer.hpp>
#endif

#if defined( SUPPORTS_FILESERVE_CLIENT )
  #include <Vision/Runtime/Base/System/IO/Stream/VFileServeStreamManager.hpp> 
#endif

#ifdef WIN32
  #include <direct.h>
  #ifdef _VISION_WINRT
    char VisSampleApp::m_szCacheDirectory[FS_MAX_PATH];
    char VisSampleApp::m_szSDCardDirectory[FS_MAX_PATH];
  #endif
#elif defined(_VISION_XENON)
#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Common/VisSampleAppPS3.inl>
#elif defined(_VISION_IOS)
  char VisSampleApp::m_szRootDirectory[FS_MAX_PATH];
  char VisSampleApp::m_szBaseDataDirectory[FS_MAX_PATH];
  char VisSampleApp::m_szCommonDataDirectory[FS_MAX_PATH];
  char VisSampleApp::m_szCachesDirectory[FS_MAX_PATH];
  char VisSampleApp::m_szDocumentsDirectory[FS_MAX_PATH];
  //#include <VisSampleAppiOS.inl>
#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Common/VisSampleAppPSP2.inl>
#elif defined(_VISION_ANDROID)
char VisSampleApp::m_szBaseDataDirectory[FS_MAX_PATH];
char VisSampleApp::m_szCommonDataDirectory[FS_MAX_PATH];
char VisSampleApp::m_szCacheDirectory[FS_MAX_PATH];
char VisSampleApp::m_szSDCardDirectory[FS_MAX_PATH];
char VisSampleApp::m_szAPKDirectory[FS_MAX_PATH];
#include <Vision/Runtime/Common/VisSampleAppAndroid.inl>
#elif defined(_VISION_WIIU)
#else
  #error "Missing platform!"
#endif

// Define the memory manager.
DEFINE_MEMORYMANAGER(GetDefaultVMemoryManager()); 

VInputMap* VisSampleApp::s_pInputMap; 
VColorRef HAVOK_COLOR_BACK = VColorRef(35, 31, 32);
VColorRef HAVOK_COLOR_FORE = VColorRef(253,184,19);

bool VisSampleApp::s_bIsFileserverInstalled = false;


#define SAFE_DEINIT_MOUSE() \
if(VInputManagerPC::IsInitialized()) V_PC_MOUSE.DeInit();

#define SAFE_INIT_MOUSE() \
if(VInputManagerPC::IsInitialized()) V_PC_MOUSE.Init( VInputManagerPC::GetInputMethodMouse(), VInputManagerPC::IsMouseCursorHidden());

// internal forward declaration functions-------------------------------------------------
// Just check if the desired resolution in fullscreen is supported by the adapter.
// It returns false if not supported or cannot check it
// It returns true if it is supported
// In both cases, it always returns in selectedX and selectedY the correct resolution the application should have in fullscreen
#if defined(WIN32)
static bool CheckFullscreenResolution( int iAdapter, int desiredX, int desiredY, int* selectedX, int* selectedY );
#endif

// public functions ---------------------------------------------------------------------
VisSampleApp::VisSampleApp()
  : m_bShowFrameRate(false)
  , m_bShowHelp(false)
  , m_spCamera(NULL)
  , m_iLastCameraPositionEntityIndex(-1)
  , m_pszHelpText(16,NULL)
  , m_iNumHelpLines(0)
  , m_fFPSPos(0.f)
  , m_fCurrentFPS(0.f)
  , m_fCurrentFrameTime(0.f)
  , m_sceneLoader()
  , m_pBackgroundResourceRestorer(NULL)
  , m_appState(AS_UNKNOWN)
  , m_bScreenshotPending(false)
#if defined(_VISION_IOS)
  , m_bScreenshotForBackgroundPending(false)
#endif
  , m_bShowLoadingProgress(true)
  , m_iInsideLoadWorldCounter(0)
  , m_loadingScreenSettings()
  , m_loadingScreenSettingsBackup()
  , m_pTmpBackgroundScreenshot(NULL)
  , m_spBackground(NULL)
  , m_spBackgroundColor(NULL)
  , m_fFadeOutPos(0.f)
  , m_iFrameCtr(0)
  , m_iSampleFlags(0)
  , m_spLogoOverlay(NULL)
  , m_fCurrentFadeOutTime(1.f)
  , m_fFadeInTime(0.f)
  , m_iXDist(10)
  , m_bLogoFadeInDone(false)
  , m_bSampleInitialized(false)
  , m_bTripleHead(false)
  , m_spProgressBar(NULL)
  , m_spProgressBack(NULL)
  , m_pTimeStepGraph(NULL)
  , m_iUnfilteredTimeStepCurveIndex(-1)
  , m_iTimeStepCurveIndex(-1)
{
#ifdef _VR_DX11
  memset( m_FeatureLevels, 0, sizeof( m_FeatureLevels ) );
#endif

  VISION_INTER_LINKING_CHECK
  SetHelpTextPosition(9,16);

  m_szCurrentDir[0] = 0;
#ifdef SUPPORTS_RELATIVE_PATHS
  // Save the current directory, so the application can reset it on exit  
  VFileHelper::GetCurrentWorkingDir( m_szCurrentDir, FS_MAX_PATH );
#endif

  // initialize default help
#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
  Vision::Message.Add(1,"Press 'F1' for help.");

#elif defined (_VISION_XENON) || defined(_VISION_WINRT)
  Vision::Message.Add(1,"Press 'Pad1 BACK' for help.");

#elif defined (_VISION_PS3)
  Vision::Message.Add(1,"Press 'Pad1 SELECT' for help.");

#endif

  #ifdef PLATFORM_STARTUP
    PLATFORM_STARTUP();
  #endif

#ifdef _VISION_WIIU
    m_spDRCContext = NULL;
    m_pDRCCopyBuffer = NULL;
#endif

  Vision::Callbacks.OnVideoChanged += this;
  Vision::Callbacks.OnBeforeSwapBuffers += this;
  
#if defined(_VISION_MOBILE)
  Vision::Callbacks.OnLeaveForeground += this;
  Vision::Callbacks.OnEnterBackground += this;
  Vision::Callbacks.OnBackgroundRestore += this;
  Vision::Callbacks.OnEnterForeground += this;
#endif
}

VisSampleApp::~VisSampleApp()
{ 
  Vision::Callbacks.OnVideoChanged -= this;
  Vision::Callbacks.OnBeforeSwapBuffers -= this;

#if defined(_VISION_MOBILE)
  Vision::Callbacks.OnLeaveForeground -= this;
  Vision::Callbacks.OnEnterBackground -= this;
  Vision::Callbacks.OnBackgroundRestore -= this;
  Vision::Callbacks.OnEnterForeground -= this;
#endif

  //Can't do the above here because we need explicit InitSample/DeInitSample calls.
  //This is because the sample is installed as the engine application and will
  //not get destroyed until after the engine is de-initialized and then it is too late.
  FreeHelpText();

  VASSERT(!m_bSampleInitialized); // Please call DeInitSample();

  #if defined(SUPPORTS_RELATIVE_PATHS) && !defined(_VISION_WINRT)
    // restore the path the application started in
    if (m_szCurrentDir[0])
      VFileHelper::ChDir ( m_szCurrentDir );    
  #endif

  #ifdef PLATFORM_SHUTDOWN
    PLATFORM_SHUTDOWN();
  #endif

  V_SAFE_DELETE_ARRAY(m_pTmpBackgroundScreenshot);
}

//////////////////////////////////////////////////////////////////////////////
// Init/Deinit
//////////////////////////////////////////////////////////////////////////////


bool VisSampleApp::InitSample( const char *pszSampleDataDir, const char *pszSampleScene, int iSampleFlags, int iX, int iY, int iEngineFlags, int iNumThreads)
{
  FORCE_LINKDYNCLASS(VisMouseCamera_cl);

  Vision::SetApplication(this);

  VDiskFileStreamCacheManager::SetReadCacheSize(256*1024);
  
  m_iSampleFlags = iSampleFlags;

  if(iSampleFlags & VSAMPLE_HEADLESS)
  {
    VVideo::SetHeadlessModeEnabled(true);
  }

  LoadHelpFile(NULL, (m_iSampleFlags&VSAMPLE_DISABLEDEFAULTKEYS) ? false : true);  

  // Set the current directory to the EXE dir so the sample can find it's data files
  //(We need to do this since VC2003 sets the project file dir as the default
  // dir and not the EXE. We can change the startup directory in the settings, but it
  // is in a user specific (SUO) file and not in the SLN/VCPROJ files we ship)
  VisionAppHelpers::MakeEXEDirCurrent();
  
  // ensure the vision engine plugin is loaded
  LoadVisionEnginePlugin();

  iSampleFlags |= VISION_MANDATORY_SAMPLE_FLAGS;

#if defined(WIN32) && !defined(_VISION_WINRT)
  const char *szCaption = pszSampleDataDir ? pszSampleDataDir : "Engine Sample";

  if(iSampleFlags & VSAMPLE_HEADLESS)
  {
    iSampleFlags &= ~VSAMPLE_ASKFULLSCREEN;
  }

  // Show dialog asking the user if he wants to go fullscreen
  if (iSampleFlags & VSAMPLE_ASKFULLSCREEN)
  {
    // the question is negated, so that just pressing enter will stay in windowed mode, the return value is true
    if ( MessageBoxA (NULL, "Do you want to run in windowed mode?",szCaption, MB_ICONQUESTION | MB_YESNO ) == IDNO) 
    {
      iSampleFlags |= VSAMPLE_FORCEFULLSCREEN;
    }
    else
    {
      iSampleFlags &= ~VSAMPLE_FORCEFULLSCREEN;
    }

  }
  //m_appConfig.m_videoConfig.m_iAdapter = 0;

  if ((iSampleFlags & VSAMPLE_FORCEFULLSCREEN) && (iSampleFlags & VSAMPLE_USEDESKTOPRESOLUTION))
  {
    DEVMODEA deviceMode;

    deviceMode = Vision::Video.GetAdapterMode(m_appConfig.m_videoConfig.m_iAdapter);

    iX = deviceMode.dmPelsWidth;
    iY = deviceMode.dmPelsHeight;
  }
  
  //Detect TripleHead display resolutions
  if (iY>0)
  {
    float fAspect = (float)iX/(float)iY;
    m_bTripleHead = (fAspect > 3.f);  //More than a 3:1 ratio ?
  }

  if (iSampleFlags & VSAMPLE_ENABLE_IHV_PROFILING)
    m_appConfig.m_videoConfig.m_bEnableNVPerfHUD = true;

  #ifdef _VR_DX11
    // For DX11, we allow manually selecting lower feature levels
    if (iSampleFlags & VSAMPLE_DX11_FEATURE_LEVEL_MASK)
    {
      int iNumFeatureLevels = 0;
      if ( iSampleFlags & VSAMPLE_DX11_0_ON_DX11 )
        m_FeatureLevels[ iNumFeatureLevels++ ] = D3D_FEATURE_LEVEL_11_0;
      if ( iSampleFlags & VSAMPLE_DX10_1_ON_DX11 )
        m_FeatureLevels[ iNumFeatureLevels++ ] = D3D_FEATURE_LEVEL_10_1;
      if ( iSampleFlags & VSAMPLE_DX10_0_ON_DX11 )
        m_FeatureLevels[ iNumFeatureLevels++ ] = D3D_FEATURE_LEVEL_10_0;
      if ( iSampleFlags & VSAMPLE_DX9_3_ON_DX11 )
        m_FeatureLevels[ iNumFeatureLevels++ ] = D3D_FEATURE_LEVEL_9_3;
      if ( iSampleFlags & VSAMPLE_DX9_2_ON_DX11 )
        m_FeatureLevels[ iNumFeatureLevels++ ] = D3D_FEATURE_LEVEL_9_2;
      if ( iSampleFlags & VSAMPLE_DX9_1_ON_DX11 )
        m_FeatureLevels[ iNumFeatureLevels++ ] = D3D_FEATURE_LEVEL_9_1;

      m_appConfig.m_videoConfig.m_iNumFeatureLevels = iNumFeatureLevels;
      m_appConfig.m_videoConfig.m_pFeatureLevels = m_FeatureLevels;
    }
  #endif
 
#elif defined(_VISION_APOLLO) && !defined(_M_ARM)

  iSampleFlags |= VSAMPLE_DX9_ON_DX11; // Force on X86 emu so that more like current h/w
  m_appConfig.m_videoConfig.m_iNumFeatureLevels = 1;
  m_appConfig.m_videoConfig.m_pFeatureLevels = &m_FeatureLevel;
  m_FeatureLevel = D3D_FEATURE_LEVEL_9_3; 

#elif defined (_VISION_XENON) 
  
  if (iSampleFlags & VSAMPLE_XBOX360_BUFFER_2_FRAMES)
    m_appConfig.m_videoConfig.m_iCreateDeviceBehaviorFlags |= D3DCREATE_BUFFER_2_FRAMES;

#elif defined (_VISION_WINRT)
  
	if (iSampleFlags & (VSAMPLE_DX10_ON_DX11 | VSAMPLE_DX9_ON_DX11))
	{
		m_appConfig.m_videoConfig.m_iNumFeatureLevels = 1;
		m_appConfig.m_videoConfig.m_pFeatureLevels = &m_FeatureLevel;
		if (iSampleFlags & VSAMPLE_DX10_ON_DX11)
			m_FeatureLevel = D3D_FEATURE_LEVEL_10_0;
		if (iSampleFlags & VSAMPLE_DX9_ON_DX11)
			m_FeatureLevel = D3D_FEATURE_LEVEL_9_3;
	}
#endif

  if (iSampleFlags & VSAMPLE_NO_DEPTHBUFFER)
    m_appConfig.m_videoConfig.m_iFBDepthStencilBits = 0;

  if (iSampleFlags & VSAMPLE_MULTISAMPLE8X)
    m_appConfig.m_videoConfig.m_eMultiSample = VVIDEO_MULTISAMPLE_8TIMES;
  else if (iSampleFlags & VSAMPLE_MULTISAMPLE4X)
    m_appConfig.m_videoConfig.m_eMultiSample = VVIDEO_MULTISAMPLE_4TIMES;
  else if (iSampleFlags & VSAMPLE_MULTISAMPLE2X)
    m_appConfig.m_videoConfig.m_eMultiSample = VVIDEO_MULTISAMPLE_2TIMES;
  else
    m_appConfig.m_videoConfig.m_eMultiSample = VVIDEO_MULTISAMPLE_OFF;

  m_appConfig.m_videoConfig.m_bFullScreen = (iSampleFlags & VSAMPLE_FORCEFULLSCREEN) != 0;
  m_appConfig.m_videoConfig.m_iXRes = iX;
  m_appConfig.m_videoConfig.m_iYRes = iY;  

  if (iSampleFlags & VSAMPLE_WAITRETRACE)
    m_appConfig.m_videoConfig.m_bWaitVRetrace = true;

#if defined(_VISION_PS3)
  if (iSampleFlags & VSAMPLE_FLIPMODE30FPS)
	  m_appConfig.m_videoConfig.m_AdaptiveFlipMode = VVideoConfig::FlipMode30FPS;
  else if (iSampleFlags & VSAMPLE_FLIPMODE60FPS)
	  m_appConfig.m_videoConfig.m_AdaptiveFlipMode = VVideoConfig::FlipMode60FPS;
#endif

  m_appConfig.m_iInitFlags = iEngineFlags;  

#if defined(WIN32) || defined(_VISION_XENON)
  VStreamProcessor::Initialize(1);


#elif defined(_VISION_PS3)

  VSpursHandler *pSpurs = Vision::GetSpursHandler();
  if(pSpurs == NULL)
  {    
    pSpurs = new VSpursHandler();
    bool bRet = pSpurs->InitializeSPUs();  
    VASSERT(bRet); 
    bRet = pSpurs->InitializeSpurs();     
    VASSERT(bRet); 
    #ifdef HK_DEBUG
      int iPrio; 
      VisSampleSpursUtilGetSecondaryPpuThreadPriority(&iPrio);  
      VSpuPrintfService::Initialize(pSpurs->GetSpurs(), iPrio);  
    #endif
    Vision::SetSpursHandler(pSpurs);  
  }
  #ifdef VSTREAMPROCESS_RUN_IN_SPURS
    VStreamProcessor::Initialize(1, 5, pSpurs->GetSpurs());
  #else
    VStreamProcessor::Initialize(1);
  #endif

#endif

  // Base data files
  SetupBaseDataDir();

#if defined(WIN32) && !defined(_VISION_WINRT)

  // Enum adapter modes to check the best fit in fullscreen
  if ( m_appConfig.m_videoConfig.m_bFullScreen && (iSampleFlags & VSAMPLE_CHECKFULLSCREENMODE)!=0 )
  {
    int newX, newY;
    bool supported = CheckFullscreenResolution( m_appConfig.m_videoConfig.m_iAdapter, m_appConfig.m_videoConfig.m_iXRes, 
                                                m_appConfig.m_videoConfig.m_iYRes, &newX, &newY );
    if ( !supported )
      Vision::Error.Warning( "The selected resolution (%dx%d) isn't supported in fullscreen, using %dx%d instead.", 
                                m_appConfig.m_videoConfig.m_iXRes, m_appConfig.m_videoConfig.m_iYRes, newX, newY );
    m_appConfig.m_videoConfig.m_iXRes = newX;
    m_appConfig.m_videoConfig.m_iYRes = newY;
  }
#endif

  //Initialize the engine
  if (!InitEngine())
  {  
#if defined(WIN32) && !defined(_VISION_WINRT)
    SAFE_DEINIT_MOUSE();

    MessageBoxA(NULL, "Failed to initialize the engine", szCaption, MB_ICONEXCLAMATION );
#endif
    return false;
  }

#ifdef _VISION_PSP2
  Vision::Renderer.SetDefaultIndexBufferSize(USHRT_MAX);
#endif

  if (!(iSampleFlags & VSAMPLE_CUSTOMDATADIRECTORIES))
  {
    //Setup the data directories
    if (!SetupSampleData(pszSampleDataDir))
    {
  #if defined(WIN32) && !defined(_VISION_WINRT)
      SAFE_DEINIT_MOUSE();
      MessageBoxA(NULL, "Failed to initialize the data directories", szCaption, MB_ICONEXCLAMATION );
      SAFE_INIT_MOUSE();
  #endif
      return false;
    }
  }

  if (iSampleFlags & VSAMPLE_SPLASHSCREEN)
  {
    LoadingScreenSettings settings;
    settings.fFadeOutTime = 2.0f;

#if defined( HK_ANARCHY )
    settings.backgroundColor = HAVOK_COLOR_BACK;
    settings.eAspectRatioAlignment = LoadingScreenSettings::ALIGN_VERTICAL;
    settings.bForceFullScreenTexture = true;
    if(Vision::File.Exists("Anarchy_Splash_1024x512.dds", "..\\Textures"))
      settings.sBackgroundImagePath = "..\\Textures\\Anarchy_Splash_1024x512.dds";
    else if (Vision::File.Exists("Anarchy_Splash_1024x512.dds", "Textures"))
      settings.sBackgroundImagePath = "Textures\\Anarchy_Splash_1024x512.dds";
#else
    settings.backgroundColor = HAVOK_COLOR_BACK;
    settings.eAspectRatioAlignment = LoadingScreenSettings::ALIGN_HORIZONTAL;
    if(Vision::File.Exists("Havok_Logo_1024x512.dds", "..\\Textures"))
      settings.sBackgroundImagePath = "..\\Textures\\Havok_Logo_1024x512.dds";
    else if (Vision::File.Exists("Havok_Logo_1024x512.dds", "Textures"))
      settings.sBackgroundImagePath = "Textures\\Havok_Logo_1024x512.dds";
#endif

    SetupLoadingScreen( true, settings );
  }

  //create input map with free slots for all samples (user_space)
  s_pInputMap = new VInputMap(VISION_INPUT_CONTROL_LAST_ELEMENT+1+VISION_INPUT_CONTROL_USER_SPACE, VISION_INPUT_CONTROL_ALTERNATIVES);

  // Define the keys we want to have available
  DefineKeys();

  // Create additional Threads. The passed iNumThreads count includes the main thread, so we subtract one here.
  InitThreadManager(iNumThreads - 1, VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT);

#if !defined(_VR_GLES2) || defined(FORCE_TRILINEAR_FILTERING)
  Vision::Renderer.SetDefaultTextureFilterMode(FILTER_MIN_MAG_MIP_LINEAR);
#else
  Vision::Renderer.SetDefaultTextureFilterMode(FILTER_MIN_MAG_LINEAR_MIP_POINT);
#endif

  // Check whether the graphics card supports at least SM 1.1
  VisionAppHelpers::CheckShaderCompatibility(); 

  if (m_bTripleHead)
  {
    //Set the vertical FOV to the typical one and calculate the horizontal one
    //(The typical vertical FOV is from when we have a 90deg horizontal and a 4:3 aspect.
    // The computed horizontal FOV will be around 140)
    VisRenderContext_cl::GetMainRenderContext()->SetFOV(0.0f, 73.739792f);
  }
 
  m_bSampleInitialized = true;

  Vision::Renderer.SetShowBenignRuntimeWarnings(false);

#if (defined(_VISION_MOBILE) && defined(HK_DEBUG)) || defined(_VISION_ANDROID)
  // force the resources for the mobile menus to be loaded right here
  VGUIManager::GlobalManager().LoadResourceFile("GUI/MenuSystemMobile.xml");
#endif

#if defined(_VISION_WIIU)

  if((iSampleFlags & VSAMPLE_WIIU_DRCDEMO) != 0 && (iSampleFlags & VSAMPLE_WIIU_DRCCOPY) != 0)
  {
    Vision::Error.Warning("Both DRC demo and copy not possible at the same time, falling back to DRC demo!");
    iSampleFlags &= ~VSAMPLE_WIIU_DRCCOPY;
  }
  
  // Show something on the DRC controller
  if((iSampleFlags & VSAMPLE_WIIU_DRCDEMO) != 0)
  {
    InitDRCDemo();
  }
  else if((iSampleFlags & VSAMPLE_WIIU_DRCCOPY) != 0)
  {
    InitDRCCopyMode();
  }

#endif

  return LoadScene(pszSampleScene, 0, (iSampleFlags & VSAMPLE_NO_FALLBACK_ASSETPROFILE) == 0);
}

void VisSampleApp::InitThreadManager(int iWorkerThreadCount, bool bFixedCPUAssignment)
{
  const int iCores = Vision::GetThreadManager()->GetNumberOfProcessors();
#ifdef _VISION_XENON
  // Don't assign worker threads to HW threads 0 and 1, since the main thread is running on HW thread 0,
  // and we want all resources of CPU 0 to be available for it.
  const int iSkipCores = 2;
#else
  const int iSkipCores = 1;
#endif

  // Compute maximum amount of worker threads.
  if (iWorkerThreadCount <= -1)
  {
    iWorkerThreadCount = iCores - iSkipCores;
  }

  Vision::GetThreadManager()->SetThreadCount(iWorkerThreadCount);

  if (bFixedCPUAssignment)
  {
    if (iCores > iSkipCores)
    {
      for (int iThread=0; iThread < iWorkerThreadCount; iThread++)
      {
        Vision::GetThreadManager()->AssignThreadToProcessor(iThread, iThread % (iCores - iSkipCores) + iSkipCores);
      }
    }
  }
}


#if defined(WIN32) && !defined(_VISION_WINRT)

bool VisSampleApp::InitSampleForBrowser( HWND hWnd, WNDPROC wndProc, VVIDEO_Multisample msaa)
{
  FORCE_LINKDYNCLASS(VisMouseCamera_cl);

  Vision::SetApplication(this);

  // Set the current directory to the EXE dir so the sample can find it's data files
  //(We need to do this since VC2003 sets the project file dir as the default
  // dir and not the EXE. We can change the startup directory in the settings, but it
  // is in a user specific (SUO) file and not in the SLN/VCPROJ files we ship)
  VisionAppHelpers::MakeEXEDirCurrent();

  WINDOWINFO WindowInfo;
  GetWindowInfo(hWnd, &WindowInfo);

  m_appConfig.m_windowConfig.ParentWindowHandle = hWnd;
  m_appConfig.m_windowConfig.WindowCreationFlags = WS_VISIBLE | WS_CLIPSIBLINGS;
  m_appConfig.m_windowConfig.SetWindowProc(wndProc);

  
  m_appConfig.m_videoConfig.m_iXRes = WindowInfo.rcClient.right - WindowInfo.rcClient.left;
  m_appConfig.m_videoConfig.m_iYRes = WindowInfo.rcClient.bottom - WindowInfo.rcClient.top; 
  m_appConfig.m_videoConfig.m_iXPos = 0;
  m_appConfig.m_videoConfig.m_iYPos = 0;
  m_appConfig.m_videoConfig.m_eMultiSample = msaa;
  m_appConfig.m_videoConfig.m_bFullScreen = false;
  m_appConfig.m_iInitFlags &= ~VAPP_INIT_INPUT;
  m_appConfig.m_iInitFlags &= ~VAPP_MOUSE_NONEXCLUSIVE; //Windows Input isn't deeded to use this option for browser.
  m_appConfig.m_iInitFlags |= VAPP_KEYBOARD_NONEXCLUSIVE;
  m_appConfig.m_iInitFlags &= ~VAPP_USE_DINPUT_MOUSE;
  

  VStreamProcessor::Initialize(1);

  //Initialize the engine
  if (!InitEngine())
  {  
    SAFE_DEINIT_MOUSE();

    MessageBoxA(NULL, "Failed to initialize the engine", "", MB_ICONEXCLAMATION );
    return false;
  }

  //create input map with free slots for all samples (user_space)
  s_pInputMap = new VInputMap(VISION_INPUT_CONTROL_LAST_ELEMENT+1+VISION_INPUT_CONTROL_USER_SPACE, VISION_INPUT_CONTROL_ALTERNATIVES);

  // Define the keys we want to have available
  DefineKeys();

  // Create additional Threads. The passed iNumThreads count includes the main thread!
  /*if (iNumThreads==0)
    iNumThreads = Vision::GetThreadManager()->GetNumberOfProcessors(); 
  Vision::GetThreadManager()->SetThreadCount(iNumThreads-1); */

  Vision::Renderer.SetDefaultTextureFilterMode(FILTER_MIN_MAG_MIP_LINEAR);  

  // Check whether the graphics card supports at least SM 1.1
  VisionAppHelpers::CheckShaderCompatibility(); 

  m_bSampleInitialized = true;

  return true;
}

#endif 

#ifdef _VISION_IOS
//static
void VisSampleApp::SetupDirectories(bool bLoadFromDocuments)
{
  strcpy(m_szRootDirectory, g_VisionAppSettings.szAppDirectory);
  strcat(m_szRootDirectory, "/");  
  strcpy(m_szCachesDirectory, g_VisionAppSettings.szCachesDirectory);
  strcpy(m_szDocumentsDirectory, g_VisionAppSettings.szDocumentsDirectory);
  
  if (bLoadFromDocuments)
  {
    sprintf(m_szBaseDataDirectory, "%s/BaseData.zip", m_szDocumentsDirectory);
    sprintf(m_szCommonDataDirectory, "%s/Data.zip", m_szDocumentsDirectory);    
  }
  else
  {
    sprintf(m_szBaseDataDirectory, "%sData/Vision/Base/", m_szRootDirectory);
    sprintf(m_szCommonDataDirectory, "%sData/Vision/Samples/Engine/", m_szRootDirectory);
  }
}
#endif

#ifdef _VISION_ANDROID

// static
void VisSampleApp::SetAndroidDirectories(const char* szAPKDirectory, const char* szSDCardDirectory, const char* szCacheDirectory)
{
  sprintf(m_szBaseDataDirectory, "%s?assets/Data/Vision/Base/", szAPKDirectory);
  sprintf(m_szCommonDataDirectory, "%s?assets/Data/Vision/Samples/Engine/", szAPKDirectory); 

  strcpy(m_szAPKDirectory, szAPKDirectory);
  strcpy(m_szSDCardDirectory, szSDCardDirectory);
  strcpy(m_szCacheDirectory, szCacheDirectory);
}

// static
bool VisSampleApp::ExtractFileFromAPKToCache(const char* pszFileName)
{
  VASSERT(pszFileName);

  char szInCachePath[FS_MAX_PATH];
  sprintf(szInCachePath, "%s/%s", GetCacheDirectory(), VPathHelper::GetFilename(pszFileName));

  IVFileInStream* pIn = Vision::File.Open(pszFileName);
  VASSERT(pIn);
  
  if(!pIn)
    return false;

  IVFileOutStream* pOut = Vision::File.Create(szInCachePath);
  VASSERT(pOut);

  printf("ExtractFileFromAPKToCache() writing to %s", szInCachePath);

  if(!pOut)
  {
    pIn->Close();
    return false;
  }

  unsigned char buff[128];
  while(!pIn->IsEOF())
  {
    size_t uiBytesRead = pIn->Read(buff, sizeof(buff));
    pOut->Write(buff, uiBytesRead);
  }

  pIn->Close();
  pOut->Close();

  return true;
}

#endif

#if defined(_VISION_APOLLO) || defined(_VISION_METRO)
void VisSampleApp::SetupDirectories(bool bLoadFromDocuments)
{
	  
#ifdef _VISION_WINRT
   // Should go somewhere better xx
  try
  {
	  auto localFolderPath =  Windows::Storage::ApplicationData::Current->LocalFolder->Path;
      V_WCHAR_TO_UTF8( localFolderPath->Data(), m_szCacheDirectory, MAX_PATH)
  }
  catch(...)
  {
	  strcpy( m_szCacheDirectory, "" );
  }

#ifdef _VISION_APOLLO
  try
  {
	  //auto externalDevicesPath = Windows::Storage::KnownFolders::RemovableDevices->Path; // Unimpl exception on Apollo..
	  //auto documentsPath = Windows::Storage::KnownFolders::DocumentsLibrary->Path;       // Unimpl exception on Apollo..
	  //strcpy( m_szSDCardDirectory, "D:" ); // There, but not allowed to access from Native..
	  auto installedPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;       // Unimpl exception on Apollo..
	  V_WCHAR_TO_UTF8( installedPath->Data(), m_szSDCardDirectory, MAX_PATH)
  }
  catch(...)
#endif
  {
	  strcpy( m_szSDCardDirectory, "" );
  }

#endif

}
#endif

void VisSampleApp::DeInitSample()
{
  if (!m_bSampleInitialized)
    return;

  DisableMouseCamera();

#if defined(_VISION_MOBILE)
  m_spProfilingDlg = NULL;
  m_spExitDlg = NULL;
  if (m_spGUIContext != NULL)
  {
    m_spGUIContext->SetActivate(false);
    m_spGUIContext = NULL;
  }
  VGUIManager::GlobalManager().CleanupResources();
#endif

  V_SAFE_DELETE(s_pInputMap);
  m_spBackgroundColor = NULL;
  m_spBackground = NULL;
  m_spLogoOverlay = NULL;
  m_spProgressBar = NULL;
  m_spProgressBack = NULL;

  V_SAFE_DELETE(m_pTimeStepGraph);

#if defined(_VISION_WIIU)
  CleanupDRC();
#endif

  m_bSampleInitialized = false; 

  DeInitEngine();
  Vision::SetApplication(NULL);
  
  
  #ifdef _VISION_PS3
    #ifdef VSTREAMPROCESS_RUN_IN_SPURS
      VStreamProcessor::DeInitialize();
    #endif

    VSpursHandler *pSpursHandler = Vision::GetSpursHandler();
    #ifdef HK_DEBUG
      VSpuPrintfService::Finalize();
    #endif
    bool bRet = pSpursHandler->DeInitializeSpurs();
    VASSERT(bRet);
    V_SAFE_DELETE(pSpursHandler);
  #endif
  

  //Unload any plugins
  //Remember: This can unload the application object's code if it's in a plugin.
  //It's OK to call from here (since we link this class into the application),
  //but not from the virtual VisionApp::DeInitEngine while we're deinitializing
  Vision::Shutdown();
}

void VisSampleApp::UnloadScene()
{
  DisableMouseCamera();

  m_iLastCameraPositionEntityIndex = -1;

  // removes the scene
  VSceneLoader::UnloadScene();
}

void VisSampleApp::ClearScene()
{
  DisableMouseCamera();

  m_iLastCameraPositionEntityIndex = -1;

  // removes the scene and prepares a clean world
  VSceneLoader::ClearScene();
}

bool VisSampleApp::LoadScene(const char *pszSampleScene, int iAdditionalLoadingFlags, bool bAllowProfileFallback)
{
  SelectAssetProfile(pszSampleScene, bAllowProfileFallback);

  if (VStringUtil::IsEmpty(pszSampleScene))
  {
    // Empty string is considered valid, as in: empty scene
    return true;
  }

  char szSceneFile[FS_MAX_PATH];
  m_appState = AS_UNKNOWN;

  if (VFileHelper::HasExtension(pszSampleScene, "vscene"))
  {
    sprintf(szSceneFile,"%s", pszSampleScene);
  }
  else
  {
    sprintf(szSceneFile,"%s.vscene", pszSampleScene);
  }

  int iLoadingFlags = VSceneLoader::LF_PlatformDefault | iAdditionalLoadingFlags;
  if (m_iSampleFlags & VSAMPLE_FORCEMOBILEMODE)
    iLoadingFlags |= VSceneLoader::LF_ForceMobileMode;
  
  if (!m_sceneLoader.LoadScene(szSceneFile, iLoadingFlags))
  {
    Vision::Error.FatalError(m_sceneLoader.GetLastError());
    return false;
  }
  if(m_sceneLoader.IsAborted())
  {
    return false;
  }

  return true;
}

VisSampleApp::ApplicationState VisSampleApp::GetApplicationState()
{
  if (m_appState == AS_SUSPENDED)
    return m_appState;

  if (m_appState >= AS_AFTER_LOADING)
{
    m_appState = AS_RUNNING;
    return m_appState;
  }
  
  // Restore background resources first before continuing the loading process
  if (m_pBackgroundResourceRestorer != NULL)
  {
    VASSERT(m_appState == AS_LOADING);
    m_pBackgroundResourceRestorer->Tick();
  }
  // Check scene loading state
  else if (!m_sceneLoader.IsFinished())
{
    m_sceneLoader.Tick();
    m_appState = AS_LOADING;
  }
  else if (m_appState == AS_UNKNOWN || m_appState == AS_LOADING)
  {
    if (m_sceneLoader.IsInErrorState())
      m_appState = AS_LOADING_ERROR;
    else
      m_appState = AS_AFTER_LOADING;
  }
  
  // Refresh screen when loading
  if (m_appState == AS_LOADING)
  {
#if defined(_VISION_ANDROID)
    // Always allow the application to exit on android if the app is loading
    if (GetInputMap()->GetTrigger(EXIT)) 
    {
      m_sceneLoader.Close();
      m_sceneLoader.OnAbort();
      m_appState = AS_LOADING_ERROR;
      return m_appState;
    }

    // We have to draw the screen masks here to ensure that we actually fill the 
    // frame buffer every time the API is called since the OS clears the frame 
    // buffer itself.
    Vision::Video.ResumeRendering();

    VisRenderContext_cl::GetMainRenderContext()->Activate();
    Vision::RenderScreenMasks();
	#endif
    Vision::Video.UpdateScreen();
  }

  return m_appState;
}

bool VisSampleApp::AddSampleDataDir(const char *pszDataDir)
{
  if (pszDataDir && pszDataDir[0])
  {
    char szPlatformDataDir[FS_MAX_PATH];
    char szFilename[FS_MAX_PATH];
    char szTemp[FS_MAX_PATH];

    strcpy(szPlatformDataDir, pszDataDir);

    #ifdef NATIVE_PATH_SEPARATOR_BACKSLASH
      VPathHelper::FrontToBackSlash(szPlatformDataDir);
    #else
      VPathHelper::BackToFrontSlash(szPlatformDataDir);
    #endif

    // The common/root paths used in AppData mode are not compatible with the paths the file server uses.
    // In fact, the file server prepends the standard data directories anyway.
    if (s_bIsFileserverInstalled)
    {
      bool bRes = Vision::File.AddDataDirectory(pszDataDir);
      return bRes;
    }

    //trim ? away for:
    //blah?SubDir
    char * szSubArchiveMark = strchr(szPlatformDataDir, '?');
    if(szSubArchiveMark!=NULL)
    {
      *szSubArchiveMark = 0;
      //go on with search process...
    }

    // is there a .v file in the root directory
    sprintf(szFilename, "%s.v", szPlatformDataDir);
    VPathHelper::CombineDirAndFile(szTemp, VISION_ROOT_DATA, szFilename);
    if (VFileHelper::Exists(szTemp))
    {
      if(szSubArchiveMark!=NULL)
        sprintf(szTemp, "%s?%s", szTemp, &szSubArchiveMark[1]);
      Vision::File.AddDataDirectory(szTemp, true);
      Vision::Error.SystemMessage("Adding data directory \"%s\"", szTemp);
      return true;
    }

    // is there a .zip file in the root directory
    sprintf(szFilename, "%s.zip", szPlatformDataDir);
    VPathHelper::CombineDirAndFile(szTemp, VISION_ROOT_DATA, szFilename);
    if (VFileHelper::Exists(szTemp))
    {
      if(szSubArchiveMark!=NULL)
        sprintf(szTemp, "%s?%s", szTemp, &szSubArchiveMark[1]);
      Vision::File.AddDataDirectory(szTemp, true);
      Vision::Error.SystemMessage("Adding data directory \"%s\"", szTemp);
      return true;
    }

    // is there a .varc file in the root directory
    sprintf(szFilename, "%s.varc", szPlatformDataDir);
    VPathHelper::CombineDirAndFile(szTemp, VISION_ROOT_DATA, szFilename);
    if (VFileHelper::Exists(szTemp))
    {
      if(szSubArchiveMark!=NULL)
        sprintf(szTemp, "%s?%s", szTemp, &szSubArchiveMark[1]);
      Vision::File.AddDataDirectory(szTemp, true);
      Vision::Error.SystemMessage("Adding data directory \"%s\"", szTemp);
      return true;
    }

    // is there a directory in the root directory
    VPathHelper::CombineDirAndDir(szTemp, VISION_ROOT_DATA, szPlatformDataDir);
    if (VFileHelper::ExistsDir(szTemp))
    {
      if(szSubArchiveMark!=NULL)
        sprintf(szTemp, "%s?%s", szTemp, &szSubArchiveMark[1]);
      Vision::File.AddDataDirectory(szTemp, true);
      Vision::Error.SystemMessage("Adding data directory \"%s\"", szTemp);
      return true;
    }

    // is there a .v file in the common sample directory
    sprintf(szFilename, "%s.v", szPlatformDataDir);
    VPathHelper::CombineDirAndFile(szTemp, VISION_COMMON_DATA, szFilename);
    if (VFileHelper::Exists(szTemp))
    {
      if(szSubArchiveMark!=NULL)
        sprintf(szTemp, "%s?%s", szTemp, &szSubArchiveMark[1]);
      Vision::File.AddDataDirectory(szTemp, true);
      Vision::Error.SystemMessage("Adding data directory \"%s\"", szTemp);
      return true;
    }

    // is there a .zip file in the common sample directory
    sprintf(szFilename, "%s.zip", szPlatformDataDir);
    VPathHelper::CombineDirAndFile(szTemp, VISION_COMMON_DATA, szFilename);
    if (VFileHelper::Exists(szTemp))
    {
      if(szSubArchiveMark!=NULL)
        sprintf(szTemp, "%s?%s", szTemp, &szSubArchiveMark[1]);
      Vision::File.AddDataDirectory(szTemp, true);
      Vision::Error.SystemMessage("Adding data directory \"%s\"", szTemp);
      return true;
    }

    // is there a .varc file in the common sample directory
    sprintf(szFilename, "%s.varc", szPlatformDataDir);
    VPathHelper::CombineDirAndFile(szTemp, VISION_COMMON_DATA, szFilename);
    if (VFileHelper::Exists(szTemp))
    {
      if(szSubArchiveMark!=NULL)
        sprintf(szTemp, "%s?%s", szTemp, &szSubArchiveMark[1]);
      Vision::File.AddDataDirectory(szTemp, true);
      Vision::Error.SystemMessage("Adding data directory \"%s\"", szTemp);
      return true;
    }

    // is there a directory in the common sample directory
    VPathHelper::CombineDirAndDir(szTemp, VISION_COMMON_DATA, szPlatformDataDir);
#if !defined(_VISION_ANDROID) // Problematic with apk loading
    if (VFileHelper::ExistsDir(szTemp))
#endif
    {
      if(szSubArchiveMark!=NULL)
        sprintf(szTemp,"%s?%s", szTemp, &szSubArchiveMark[1]);
      Vision::File.AddDataDirectory(szTemp, true);
      Vision::Error.SystemMessage("Adding data directory \"%s\"", szTemp);
      return true;
    }
  }

  // failed to add the data directory
  return false;
}

bool VisSampleApp::AddSampleDataDirPackage(const char* szPackageName)
{
  // combine the package name with the current Asset profile name (such as 'DX9', 'Android', etc.)
  VString sPackageNameWithProfile;
  sPackageNameWithProfile.Format("%s.%s", szPackageName, AssetProfile::GetProfileName());

  // Try to add that as a Data Directory
  if (!AddSampleDataDir(sPackageNameWithProfile.AsChar()))
    return false;
  
  // Get the Name of the package (without and subfolder names at the beginning), combine that with the current asset profile name and the '.PackDep' extension
  VString sPackageDependencyFileName;
  sPackageDependencyFileName.Format("%s.%s.PackDep", VFileHelper::GetFilename(szPackageName), AssetProfile::GetProfileName());

  // try to read the package dependencies file, which contains the information about which other packages are required to use this package
  IVFileInStream* pFile = Vision::File.Open(sPackageDependencyFileName.AsChar());

  // file not found, so assume there are no additional dependencies
  if (pFile == NULL)
    return true;

  // the folder in which this package resides.
  // all the dependencies are given relative to this folder, so we need to combine those paths
  const VString sPackageFolder = VPathHelper::GetFileDir(szPackageName);

  // read the number of dependencies that are stored in the file
  hkUint8 uiDependencies = 0;
  pFile->Read(&uiDependencies, sizeof(hkUint8));

  // for each dependency
  for (hkUint32 dep = 0; dep < uiDependencies; ++dep)
  {
    // read the string length
    hkUint16 uiStrLen = 0;
    pFile->Read(&uiStrLen, sizeof(hkUint16));

    // read the string, terminate it with 0
    char szRelPath[FS_MAX_PATH];
    pFile->Read(&szRelPath[0], sizeof(char) * uiStrLen);
    szRelPath[uiStrLen] = '\0';

    // combine the folder to this package with the relative path to the dependent package
    const VString sOtherPackagePath = VPathHelper::CombineDirAndFile(sPackageFolder.AsChar(), szRelPath);

    // try to mount that package as a data directory as well (recursive)
    AddSampleDataDirPackage(sOtherPackagePath.AsChar());
  }

  pFile->Close();

  return true;
}

bool VisSampleApp::RemoveSampleDataDir(const char *pszDataDir)
{
  if (pszDataDir && pszDataDir[0])
  {
    char szPlatformDataDir[FS_MAX_PATH];
    char szTemp[FS_MAX_PATH];
    char szTempFullPath[FS_MAX_PATH];


    strcpy(szPlatformDataDir, pszDataDir);

    #ifdef NATIVE_PATH_SEPARATOR_BACKSLASH
      VPathHelper::FrontToBackSlash(szPlatformDataDir);
    #else
      VPathHelper::BackToFrontSlash(szPlatformDataDir);
    #endif

    // is there a .v file in the root directory
    sprintf(szTemp,"%s%s.v",VISION_ROOT_DATA, szPlatformDataDir);
    if (VFileHelper::Exists(szTemp))
    {
      Vision::File.GetAbsoluteDirectory(szTempFullPath, szTemp);
      Vision::File.RemoveDataDirectory(szTempFullPath);
      Vision::Error.SystemMessage("Remove data directory \"%s\"", szTempFullPath);
      return true;
    }

    // is there a .zip file in the root directory
    sprintf(szTemp,"%s%s.zip",VISION_ROOT_DATA, szPlatformDataDir);
    if (VFileHelper::Exists(szTemp))
    {
      Vision::File.GetAbsoluteDirectory(szTempFullPath, szTemp);
      Vision::File.RemoveDataDirectory(szTempFullPath);
      Vision::Error.SystemMessage("Remove data directory \"%s\"", szTempFullPath);
      return true;
    }

    // is there a .varc file in the root directory
    sprintf(szTemp,"%s%s.varc",VISION_ROOT_DATA, szPlatformDataDir);
    if (VFileHelper::Exists(szTemp))
    {
      Vision::File.GetAbsoluteDirectory(szTempFullPath, szTemp);
      Vision::File.RemoveDataDirectory(szTempFullPath);
      Vision::Error.SystemMessage("Remove data directory \"%s\"", szTempFullPath);
      return true;
    }

    // is there a directory in the root directory
    sprintf(szTemp,"%s%s",VISION_ROOT_DATA, szPlatformDataDir);
    if (VFileHelper::ExistsDir(szTemp))
    {
      Vision::File.GetAbsoluteDirectory(szTempFullPath, szTemp);
      Vision::File.RemoveDataDirectory(szTempFullPath);
      Vision::Error.SystemMessage("Remove data directory \"%s\"", szTempFullPath);
      return true;
    }

    // is there a .v file in the common sample directory
    sprintf(szTemp,"%s%s%s.v",VISION_COMMON_DATA, VFILE_STR_SEPARATOR, szPlatformDataDir);
    if (VFileHelper::Exists(szTemp))
    {
      Vision::File.GetAbsoluteDirectory(szTempFullPath, szTemp);
      Vision::File.RemoveDataDirectory(szTempFullPath);
      Vision::Error.SystemMessage("Remove data directory \"%s\"", szTempFullPath);
      return true;
    }

    // is there a .zip file in the common sample directory
    sprintf(szTemp,"%s%s%s.zip",VISION_COMMON_DATA, VFILE_STR_SEPARATOR, szPlatformDataDir);
    if (VFileHelper::Exists(szTemp))
    {
      Vision::File.GetAbsoluteDirectory(szTempFullPath, szTemp);
      Vision::File.RemoveDataDirectory(szTempFullPath);
      Vision::Error.SystemMessage("Remove data directory \"%s\"", szTempFullPath);
      return true;
    }

    // is there a .zip file in the common sample directory
    sprintf(szTemp,"%s%s%s.varc",VISION_COMMON_DATA, VFILE_STR_SEPARATOR, szPlatformDataDir);
    if (VFileHelper::Exists(szTemp))
    {
      Vision::File.GetAbsoluteDirectory(szTempFullPath, szTemp);
      Vision::File.RemoveDataDirectory(szTempFullPath);
      Vision::Error.SystemMessage("Remove data directory \"%s\"", szTempFullPath);
      return true;
    }

    // is there a directory in the common sample directory
    sprintf(szTemp,"%s%s%s",VISION_COMMON_DATA, VFILE_STR_SEPARATOR, szPlatformDataDir);
    if (VFileHelper::ExistsDir(szTemp))
    {
      Vision::File.GetAbsoluteDirectory(szTempFullPath, szTemp);
      Vision::File.RemoveDataDirectory(szTempFullPath);
      Vision::Error.SystemMessage("Remove data directory \"%s\"", szTempFullPath);
      return true;
    }
  }
  
  // failed to add the data directory
  return false;
}

void VisSampleApp::SetupBaseDataDir()
{
  //Engine Base Data
/*  char szBaseData[FS_MAX_PATH];
  sprintf(szBaseData,"%s.v",VISION_BASE_DATA);
  // see if we have a .v archive file for base data
  if (VFileHelper::Exists(szBaseData))
  {
    Vision::File.AddDataDirectory(szBaseData);
    Vision::Error.SystemMessage("Adding base data directory \"%s\"", szBaseData);
  }
  else*/
  {
    // No .v file, use the directory instead
    #ifndef _VISION_ANDROID // Problematic with apk loading
    if (VFileHelper::ExistsDir(VISION_BASE_DATA) || VFileHelper::Exists(VISION_BASE_DATA))
    #endif
    {
      Vision::File.AddDataDirectory(VISION_BASE_DATA, true);
      Vision::Error.SystemMessage("Adding base data directory \"%s\"", VISION_BASE_DATA);
    }
    #ifndef _VISION_ANDROID // Problematic with apk loading
    else
    {
      // No Base Data directory found (make this fatal?)
      Vision::Error.Warning("Base Data directory (%s) not found", VISION_BASE_DATA);
    }
    #endif

    #ifdef WIN32
    {
      // on PC platform, also add the Data\Vision\Simulation data folder, if available
      char szDir[FS_MAX_PATH];
      VFileHelper::CombineDirAndDir(szDir, VISION_BASE_DATA, "..\\Simulation");
      if (VFileHelper::ExistsDir(szDir))
      {
        Vision::File.AddDataDirectory(szDir, false);
        Vision::Error.SystemMessage("Adding simulation data directory \"%s\"", szDir);
      }
    }
    #endif
  }
}

void VisSampleApp::SelectAssetProfile(const char* szScene, bool bAllowFallbackProfile)
{
  Vision::File.SetAssetProfile(VTargetDeviceName[TARGETDEVICE_THIS]); // set the intended default asset profile

  if (!bAllowFallbackProfile)
    return;

  if (!szScene)
    return;
  
  char szAbsPath[FS_MAX_PATH];
  char szTemp[FS_MAX_PATH];

  // we try to find a valid scene name, because VFileHelper::GetAbsolutePath will only return an absolute path, if the given file really exists
  VPathHelper::GetFilenameNoExt(szTemp, szScene);

  // first try it with "bla.vscene"
  VString sTestName = szTemp;
  sTestName += ".vscene";

  if (VFileHelper::GetAbsolutePath(sTestName.AsChar(), szAbsPath, Vision::File.GetManager()))
    szScene = szAbsPath;
  else
  {
    // if that did not succeed, try it with every variant of "bla.profilexyz.vscene"
    // we do not care which file is really found, any of those files will do, we only need the absolute path to a valid scene folder

    for (int i = 0; i < TARGETDEVICE_COUNT; ++i)
    {
      sTestName = szTemp;
      sTestName += ".";
      sTestName += VTargetDeviceName[i];
      sTestName += ".vscene";

      if (VFileHelper::GetAbsolutePath(sTestName.AsChar(), szAbsPath, Vision::File.GetManager()))
      {
        szScene = szAbsPath;
        break;
      }
    }
  }

  char szProjectDir[FS_MAX_PATH];
  strcpy(szProjectDir, szScene);

  // now we have the scene folder (probably), but we need the project directory
  // this can be any number of folders up from where the scene is located
  // so we will just try every possible parent directory
  while (true)
  {
    VFileHelper::GetParentDir(szProjectDir);

    if (szProjectDir[0] == '\0')
      break;

    // build the path to the default aidlt file in the project directory

    VString sResult = VFileHelper::CombineDirAndDir(szProjectDir, "AssetMgmt_data");
    sResult = VFileHelper::CombineDirAndDir(sResult.AsChar(), AssetProfile::GetProfileName());
    sResult += ".aidlt";

    // if we can open that file, all is fine
    if (Vision::File.Exists(sResult.AsChar()) == TRUE)
      return;

    // otherwise, if this is one of the platforms that is compatible with the pcdx9 profile, try that instead

    if ((VStringHelper::SafeCompare(AssetProfile::GetProfileName(), VTargetDeviceName[TARGETDEVICE_DX11]) == 0) ||
        (VStringHelper::SafeCompare(AssetProfile::GetProfileName(), VTargetDeviceName[TARGETDEVICE_PS3]) == 0) ||
        (VStringHelper::SafeCompare(AssetProfile::GetProfileName(), VTargetDeviceName[TARGETDEVICE_XBOX360]) == 0) ||
        (VStringHelper::SafeCompare(AssetProfile::GetProfileName(), VTargetDeviceName[TARGETDEVICE_PSP2]) == 0) ||
        (VStringHelper::SafeCompare(AssetProfile::GetProfileName(), VTargetDeviceName[TARGETDEVICE_WIIU]) == 0))
    {
      sResult = VFileHelper::CombineDirAndDir(szProjectDir, "AssetMgmt_data");
      sResult = VFileHelper::CombineDirAndDir(sResult.AsChar(), VTargetDeviceName[TARGETDEVICE_DX9]);
      sResult += ".aidlt";

      // if the pcdx9 profile exists, switch to that
      if (Vision::File.Exists(sResult.AsChar()) == TRUE)
      {
        Vision::File.SetAssetProfile(VTargetDeviceName[TARGETDEVICE_DX9]);
        return;
      }
    }
  }

  // if this is any other platform, just stick with the default profile, even though we did not find any aidlt file
}

void VisSampleApp::ProcessManifestFile(bool bAddDataDirs, bool bLoadEnginePlugins)
{
  // Add all data directories from the manifest file
  char szManifestPath[FS_MAX_PATH];
  char szProjectDir[FS_MAX_PATH];

  if (!VFileHelper::GetAbsolutePath("vForgeManifest.txt", szManifestPath, Vision::File.GetManager()))
    return;

  VFileHelper::GetFileDir(szManifestPath, szProjectDir);

  VManifest manifest;
  if(manifest.LoadManifest(szManifestPath))
  {
    int iNumEntries;
    VManifest::VManifestEntry* pEntries;
    manifest.GetEntries(iNumEntries, pEntries);

    for(int iEntryIdx = 0; iEntryIdx < iNumEntries; iEntryIdx++)
    {
      VManifest::VManifestEntry& entry = pEntries[iEntryIdx];

      if (bAddDataDirs && (entry.m_command == "AddDataDir"))
      {
        char dataDir[FS_MAX_PATH];
        VFileHelper::CombineDirAndDir(dataDir, szProjectDir, entry.m_value);
        Vision::File.AddDataDirectory(dataDir);
      }
      else if (bLoadEnginePlugins && (entry.m_command == "LoadEnginePlugin"))
      {
        bool pluginLoaded = Vision::Plugins.IsEnginePluginLoaded(entry.m_value) 
          || Vision::Plugins.GetRegisteredPlugin(entry.m_value.AsChar())
          || Vision::Plugins.LoadEnginePlugin(entry.m_value);

        if (!pluginLoaded)
        {
          Vision::Error.Warning("Engine plugin '%s' referenced in vForgeManifest.txt file failed to load", entry.m_value.AsChar());
        }
      }
    }
  }
}


//Sets up the data directories for an engine sample
//This will use a V file for basedata or sample instead of the directory if it exists
bool VisSampleApp::SetupSampleData(const char *pszDataDir)
{
#ifndef WIN32
  // Required for consoles that run many times rather than closing the application.
  Vision::File.RemoveAllDataDirectories(); //start without any directories
#endif

  // Base data files
  SetupBaseDataDir();

  //The common sample data files
  #if !defined(_VISION_ANDROID) // Problematic with apk loading
  if (VFileHelper::ExistsDir(VISION_COMMON_DATA) || VFileHelper::Exists(VISION_COMMON_DATA))
  #endif
  {
    Vision::File.AddDataDirectory(VISION_COMMON_DATA);
    Vision::Error.SystemMessage("Adding common data directory \"%s\"", VISION_COMMON_DATA);
  }
  #if !defined(_VISION_ANDROID) // Problematic with apk loading
  else
  {
    // No common data directory found (make this fatal?)
    Vision::Error.Warning("Common Data directory (%s) not found", VISION_COMMON_DATA);
  }
  #endif
  
  //The sample specific data files
  if (pszDataDir && pszDataDir[0])
  {
    AddSampleDataDir(pszDataDir);
  }
  else
  {
    Vision::File.AddDataDirectory(VISION_ROOT_DATA);
  }

  // output files (esp. screenshots) should go into the application directory by default
  char szEXEDir[FS_MAX_PATH];
  #ifdef VISION_OUTPUT_DIR
    strcpy(szEXEDir, VISION_OUTPUT_DIR);
  #else
    VBaseAppHelpers::GetApplicationDir(szEXEDir);
  #endif
  if (Vision::File.SetOutputDirectory(szEXEDir))
  {
    Vision::Error.SystemMessage("Output directory is \"%s\"", szEXEDir);
    // also set it as the last input directory because some samples rely on saving & loading
    Vision::File.AddDataDirectory(szEXEDir);
  }
  else
    Vision::Error.SystemMessage("Failed to set output directory; Defaulting to standard output dir");
  
  if ((m_iSampleFlags & VSAMPLE_DONT_LOAD_MANIFESTFILE) == 0)
  {
    // Add the data directories from the manifest file (the engine plugins must have been
    // handled earlier; at this point, it's too late to initialize them properly.
    ProcessManifestFile(true, false);
  }

  return true;
}

/////////////////////////////////////////////////////////////////////

void VisSampleApp::UpdateFPS()
{
  m_iFrameCtr++;
  m_fFPSPos += Vision::GetUITimer()->GetTimeDifference();
  if (m_fFPSPos>=1.f)
  {
    m_fCurrentFrameTime = (float)m_fFPSPos/m_iFrameCtr;
    m_fCurrentFPS = (float)m_iFrameCtr/m_fFPSPos;
    m_fFPSPos     = 0.f;
    m_iFrameCtr   = 0;
  }
}

/////////////////////////////////////////////////////////////////////


//Update the screen overlays like the splash screen and text
void VisSampleApp::UpdateOverlays()
{
  float fDelta = Vision::GetTimer()->GetTimeDifference();

  //Update the splash screen fading
  if (m_fCurrentFadeOutTime > 0.0f && m_fFadeOutPos > 0.0f)
  {    
    float fDiff = hkvMath::Min(0.2f, fDelta/m_fCurrentFadeOutTime);
    m_fFadeOutPos -= fDiff;
  }

  if (m_fCurrentFadeOutTime <= 0.0f || m_fFadeOutPos <= 0.0f)
  {
    m_spBackgroundColor = NULL;
    m_spBackground = NULL;

    // Fade in smaller logo
    if (m_spLogoOverlay && !m_bLogoFadeInDone)
    { 
      if(m_fFadeInTime<1.f)
      {
        const int iFadeInAlpha = hkvMath::Min(255, static_cast<int>(m_fFadeInTime*255.0f));

        m_fFadeInTime += fDelta;
        m_spLogoOverlay->SetColor(VColorRef(255, 255, 255, iFadeInAlpha));
      } 
      else if(m_fFadeInTime>=1.f)
      {      
        m_spLogoOverlay->SetColor(VColorRef(255,255,255,255));
        m_bLogoFadeInDone = true;
      }
    }
  }
  else
  {
    // Fade out large logo
    const int iFadeOutAlpha = hkvMath::Min(255, static_cast<int>(m_fFadeOutPos*255.0f));

    if (m_spBackgroundColor)
    {
      m_spBackgroundColor->SetColor(VColorRef(
        m_loadingScreenSettings.backgroundColor.r, m_loadingScreenSettings.backgroundColor.g, m_loadingScreenSettings.backgroundColor.b, iFadeOutAlpha));
    }
    if (m_spBackground)
    {
      m_spBackground->SetColor(VColorRef(255, 255, 255, iFadeOutAlpha));
    }
  }

  //Queue the output text for rendering in render scene
  
  //Frames-per-second display  
  UpdateFPS();
  if (m_bShowFrameRate)
    Vision::Message.Print(1, m_iXDist, Vision::Video.GetYRes()-30 ,"FPS : %.1f\nFrame Time : %.2f", GetCurrentFPS(), m_fCurrentFrameTime * 1000.0f);

  // Help text overlay
  if (m_bShowHelp)
    PrintHelpText();

}


void VisSampleApp::CreateForwardRenderer()
{
#if defined( SUPPORTS_RENDERERNODES ) || defined( _VISION_MOBILE )

#if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
  VRendererNodeCommon *pRenderer = new VMobileForwardRenderingSystem(Vision::Contexts.GetMainRenderContext());
#else
  VRendererNodeCommon *pRenderer = new VForwardRenderingSystem(Vision::Contexts.GetMainRenderContext());
#endif

  VASSERT(pRenderer);
  VPostProcessToneMapping *pToneMapper = new VPostProcessToneMapping(TONEMAP_NONE, 1.0f, 1.0f, 0.0f);
  VASSERT(pToneMapper);
  pRenderer->AddComponent(pToneMapper);

  pRenderer->InitializeRenderer();

  // Apply renderer node at last
  Vision::Renderer.SetRendererNode(0, pRenderer);

  // Disable depth fog
  VFogParameters fog = Vision::World.GetFogParameters();
  fog.depthMode = VFogParameters::Off;
  Vision::World.SetFogParameters(fog);
#endif
}

void VisSampleApp::DestroyForwardRenderer()
{
#if defined( SUPPORTS_RENDERERNODES ) || defined( _VISION_MOBILE )
  VSimpleRendererNode* pRenderer = new VSimpleRendererNode(Vision::Contexts.GetMainRenderContext());
  pRenderer->InitializeRenderer();
  Vision::Renderer.SetRendererNode(0, pRenderer);
#endif
}


void VisSampleApp::SetShadowsForLight(VisLightSource_cl *pLight, bool bStatus)
{
#ifdef SUPPORTS_SHADOW_MAPS

  VASSERT(pLight);

  // Convenience function only handles renderer node 0!
  VRendererNodeCommon *pRendererNode = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(0));
  if (pRendererNode == NULL)
  {
    Vision::Error.Warning("Renderer node 0 not set. VisSampleApp::SetShadowsForLight can not be used.");
    return;
  }

  IVShadowMapComponent *pComponent = IVShadowMapComponent::GetShadowMapComponent(pLight, pRendererNode);

  if (!bStatus)
  {
    if (pComponent != NULL)
      pLight->RemoveComponent(pComponent);
  }
  else
  {
    unsigned int nShadowMapSize = 1024;
    VShadowMappingMode_e shadowMapMode = SHADOW_MAPPING_MODE_PCF8;

#if defined(_VISION_PSP2) || defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
    nShadowMapSize = 512;
    shadowMapMode = SHADOW_MAPPING_MODE_PCF4;
#endif

    if (pComponent == NULL)
    {
      if (pLight->GetType() == VIS_LIGHT_DIRECTED)
      {
#if defined(  _VISION_MOBILE ) || defined( HK_ANARCHY )
        VMobileShadowMapComponentSpotDirectional *pSpotDirComponent = new VMobileShadowMapComponentSpotDirectional(0);
#else
        VShadowMapComponentSpotDirectional *pSpotDirComponent = new VShadowMapComponentSpotDirectional(0);
        pSpotDirComponent->SetCascadeCount(2);
        pSpotDirComponent->SetCascadeRange(0, 512);
        pSpotDirComponent->SetCascadeRange(1, 2048);
#endif
        pSpotDirComponent->SetShadowMapSize(nShadowMapSize);
        pComponent = pSpotDirComponent;
      }
      else if (pLight->GetType() == VIS_LIGHT_POINT)
      {
#if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
        pComponent = NULL;
#else
        pComponent = new VShadowMapComponentPoint(0);
        pComponent->SetShadowMapSize(nShadowMapSize);
#endif
      }
      else if (pLight->GetType() == VIS_LIGHT_SPOTLIGHT)
      {
#if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
        pComponent = new VMobileShadowMapComponentSpotDirectional(0);
#else
        pComponent = new VShadowMapComponentSpotDirectional(0);
#endif
        pComponent->SetShadowMapSize(nShadowMapSize);
      }

      if (pComponent) 
      {
        pComponent->SetShadowMappingMode(shadowMapMode);
        pLight->AddComponent(pComponent);
      }
    }
  }

#endif //SUPPORTS_SHADOW_MAPS
}

bool VisSampleApp::Run()
{
  VASSERT(m_bSampleInitialized);
  if (!m_bSampleInitialized)
    return false;

  // handle default keys (if not disabled)
  ProcessKeys();

  if (m_bQuit)
  {
    m_spBackgroundColor = NULL;
    m_spBackground = NULL;
    m_spLogoOverlay = NULL;
    m_spProgressBar = NULL;
    m_spProgressBack = NULL;
    return false;
  }
  
  UpdateOverlays();
  
  if (m_pTimeStepGraph && m_pTimeStepGraph->IsVisible())
  {
    m_pTimeStepGraph->Update(m_iUnfilteredTimeStepCurveIndex, Vision::GetTimer()->GetUnfilteredTimeDifference());
    m_pTimeStepGraph->Update(m_iTimeStepCurveIndex, Vision::GetTimer()->GetTimeDifference());
  }
  
#ifdef _VISION_WIIU
  if(m_spDRCContext)
  {
    if(VVideo::IsDRCConnected(V_DRC_FIRST))
      m_spDRCContext->SetRenderingEnabled(true);
    else
      m_spDRCContext->SetRenderingEnabled(false);
  }
#endif

  // Render and swap the buffers
  return VisionApp_cl::Run();
}


////////////////////////////////////////////////////////
//  Mouse camera functions
////////////////////////////////////////////////////////

VisBaseEntity_cl* VisSampleApp::EnableMouseCamera(const char* szCameraRefObjectKey, float fMoveSpeed)
{
  m_spCamera = (VisMouseCamera_cl*)Vision::Game.CreateEntity("VisMouseCamera_cl", hkvVec3(0.0f, 0.0f, 0.0f));

  if (m_spCamera != NULL)
  {
    if (!VStringUtil::IsEmpty(szCameraRefObjectKey))
    {
      VisBaseEntity_cl* pCameraRef = Vision::Game.SearchEntity(szCameraRefObjectKey);
      if (pCameraRef != NULL)
      {
        m_spCamera->SetPosition(pCameraRef->GetPosition());
        m_spCamera->SetOrientation(pCameraRef->GetOrientation());
      }
    }

    if (fMoveSpeed > 0.0f)
    {
      m_spCamera->m_fMoveSpeed = fMoveSpeed;
    }
  }

  return m_spCamera;
}

void VisSampleApp::DisableMouseCamera()
{
  if (m_spCamera)
    m_spCamera->DetachFromParent();
  m_spCamera = NULL;
}

VisBaseEntity_cl *VisSampleApp::GetMouseCamera() const
{
  return m_spCamera;
}


////////////////////////////////////////////////////////
// Help text functions
////////////////////////////////////////////////////////

void VisSampleApp::FreeHelpText ()
{
  int count = m_pszHelpText.GetSize();
  for ( int i=0; i<count; i++ )
    V_SAFE_FREE(m_pszHelpText[i]);
  m_iNumHelpLines = 0;
}

void VisSampleApp::AddHelpText( const char *szLine )
{
  if ( szLine )
    m_pszHelpText[m_iNumHelpLines] = vStrDup(szLine);
    
  m_iNumHelpLines++;
}

void VisSampleApp::SetHelpText( int index, const char *szLine )
{
  VVERIFY_OR_RET(index>=0);
  if (index>=m_iNumHelpLines)
    m_iNumHelpLines = index+1;

  V_SAFE_FREE(m_pszHelpText[index]);
  if(szLine)
    m_pszHelpText[index] = vStrDup(szLine);
}

void VisSampleApp::PrintHelpText()
{
  for (int i=0;i<m_iNumHelpLines;i++)
    if (m_pszHelpText[i])
      Vision::Message.Print(1, m_iHelpTextOfs[0], i*12+m_iHelpTextOfs[1], m_pszHelpText[i]);
}


bool VisSampleApp::LoadHelpFile( const char * szHelpfile, bool bAddDefaultKeys )
{
  bool retval = true;

  FreeHelpText ();

  FILE *f = NULL;

  if ( szHelpfile ) 
  {
    IVFileStreamManager* pMan = Vision::File.GetManager();
    const int iMax = pMan->GetNumDataDirectories();
    for (int i=0; i<iMax; i++)
    {
      if (pMan->GetDataDirectory(i))
      {
        char szFileAndPath[FS_MAX_PATH];
        VFileHelper::CombineDirAndFile(szFileAndPath,pMan->GetDataDirectory(i),szHelpfile);
        f = fopen ( szFileAndPath, "rt" );
        if (f)
          break;
      }
    }

    if ( f ) 
    {      
      while ( !feof ( f ) )
      {
        char buffer[1024];
        if ( fgets ( buffer, 1024, f ) ) 
          AddHelpText ( buffer );
      }
      fclose ( f );
    } else {
      retval = false;
    }
  }
  
#if (defined (WIN32) || defined (_VISION_LINUX)) && ( !defined(__VISION_WINRT) || defined(__VISION_METRO ) )
  if ( bAddDefaultKeys ) 
  {    
    AddHelpText ("F1 - Help");               
    AddHelpText ("F2 - Toggle fps display");
    AddHelpText ("F3 - Toggle wireframe");   
    AddHelpText ("F4 - Cycle through debug rendering info");
    AddHelpText ("F5 - Reload modified resources");
    AddHelpText ("F6 - Toggle polygon count");
    if ((m_spPhysicsModule != NULL) && (m_spPhysicsModule->GetType() != IVisPhysicsModule_cl::VISION))
    {
      AddHelpText ("F7 - Toggle gravity");
    }
    AddHelpText ("F8 - Save screenshot");
    AddHelpText ("F9 - Previous profiling chart");
    AddHelpText ("F10 - Next profiling chart");
    AddHelpText ("F11 - Reset profiling max values");
    AddHelpText ("KP -/+ - Increase/decrease fov");       
    AddHelpText ("KP . - Cycle through exported camera locations");
    AddHelpText ("ESC - Quit");
  }  


#elif defined(_VISION_XENON) || defined(_VISION_WINRT)
  if ( bAddDefaultKeys ) 
  {    
    AddHelpText ("PAD1 - BACK   : Help");               
    AddHelpText ("PAD1 - START  : Enable Console (debug version only)");   
    AddHelpText ("");
    AddHelpText ("PAD2 - BACK   : Toggle frame rate");
    AddHelpText ("PAD2 - START  : Toggle Wireframe");
    AddHelpText ("");
    AddHelpText ("PAD2 - A : Toggle polygon count");
    AddHelpText ("PAD2 - B : Save Screenshot");
    AddHelpText ("PAD2 - X : Next profiling page");
    AddHelpText ("PAD2 - Y : Previous profiling page");
    AddHelpText ("");
    AddHelpText ("PAD2 - Digital DOWN  : Toggle object debug info");
    AddHelpText ("PAD2 - Digital UP    : Reload modified resources");
    AddHelpText ("PAD2 - Digital RIGHT : Toggle polygon count");
    if ((m_spPhysicsModule != NULL) && (m_spPhysicsModule->GetType() != IVisPhysicsModule_cl::VISION))
    {
      AddHelpText ("PAD2 - Digital LEFT  : Toggle polygon count");
    }
    AddHelpText ("");
    AddHelpText ("PAD2 - Left Anlog Thumbstick: Increase/decrease fov");

    //AddHelpText ("");
    //AddHelpText ("PAD1 - Left Trigger + Right Trigger + Left Shoulder + (A|B|Y): Profiling");

  }



#elif defined(_VISION_PS3)
  if ( bAddDefaultKeys ) 
  {    
    AddHelpText ("PAD1 - SELECT : Help");               
    AddHelpText ("PAD1 - START  : Enable Console (debug version only)");   
    AddHelpText ("");
    AddHelpText ("PAD2 - SELECT : Toggle frame rate");
    AddHelpText ("PAD2 - START  : Toggle Wireframe");
    AddHelpText ("");
    AddHelpText ("PAD2 - CROSS    : Toggle polygon count");
    AddHelpText ("PAD2 - CIRCLE   : Save Screenshot");
    AddHelpText ("PAD2 - SQUARE   : Next profiling page");
    AddHelpText ("PAD2 - TRIANGLE : Previous profiling page");
    AddHelpText ("");
    AddHelpText ("PAD2 - Digital DOWN  : Toggle object debug info");
    AddHelpText ("PAD2 - Digital UP    : Reload modified resources");
    AddHelpText ("PAD2 - Digital RIGHT : Toggle polygon count");
    if ((m_spPhysicsModule != NULL) && (m_spPhysicsModule->GetType() != IVisPhysicsModule_cl::VISION))
    {
      AddHelpText ("PAD2 - Digital LEFT  : Toggle polygon count");
    }
    AddHelpText ("");
    AddHelpText ("PAD2 - Left Anlog Thumbstick: Increase/decrease fov");

    //AddHelpText ("");
    //AddHelpText ("PAD1 - Left Trigger + Right Trigger + Left Shoulder + (A|B|Y): Profiling");
  }

#elif defined(_VISION_WIIU)
  if ( bAddDefaultKeys )
  {
    AddHelpText ("DRC  - SELECT : Help");               
    AddHelpText ("DRC  - START  : Enable Console (debug version only)");   
    AddHelpText ("");
    AddHelpText ("DRC  - SELECT : Toggle frame rate");
    AddHelpText ("DRC  - START  : Toggle Wireframe");
    AddHelpText ("");
    AddHelpText ("PAD1 - B      : Toggle polygon count");
    AddHelpText ("PAD1 - A      : Save Screenshot");
    AddHelpText ("PAD1 - Y      : Next profiling page");
    AddHelpText ("PAD1 - X      : Previous profiling page");
    AddHelpText ("");
    AddHelpText ("PAD1 - Digital DOWN  : Toggle object debug info");
    AddHelpText ("PAD1 - Digital UP    : Reload modified resources");
    AddHelpText ("PAD1 - Digital RIGHT : Toggle polygon count");
    if ((m_spPhysicsModule != NULL) && (m_spPhysicsModule->GetType() != IVisPhysicsModule_cl::VISION))
    {
      AddHelpText ("PAD1 - Digital LEFT  : Toggle polygon count");
    }
    AddHelpText ("");
    AddHelpText ("PAD1 - Left Anlog Thumbstick: Increase/decrease fov");
  }

#endif
 
  return retval;
}







/////////////////////////////////////////////////////////////////////////////////
// Input functions
/////////////////////////////////////////////////////////////////////////////////

#if defined(_VISION_MOBILE)

#include <Vision/Runtime/Common/VisMobileExitDialog.inl>
#include <Vision/Runtime/Engine/Application/VisProfilingMenu.inl>

void VisSampleApp::ShowProfilingMenu()
{
  if (m_spProfilingDlg == NULL)
  {
    EnsureGUIContextCreated();
    
    m_spProfilingDlg = m_spGUIContext->GetManager()->CreateDialogInstance(
      "GUI/ProfilingMenu.xml", m_spGUIContext);
    VASSERT(m_spProfilingDlg);    
    m_spProfilingDlg->SetVisible(false); // track visibility state
  }

  // Show Dialog
  if (m_spProfilingDlg != NULL && !m_spProfilingDlg->IsVisible())
  {
    m_spProfilingDlg->SetVisible(true);
    m_spGUIContext->SetActivate(true);
    m_spGUIContext->ShowDialog(m_spProfilingDlg);
  }
}

void VisSampleApp::ShowExitDialog()
{
  if (m_spExitDlg == NULL)
  {
    EnsureGUIContextCreated();

    m_spExitDlg = m_spGUIContext->GetManager()->CreateDialogInstance(
      "GUI/MobileExitDialog.xml", m_spGUIContext);
    VASSERT(m_spExitDlg); 
    m_spExitDlg->SetVisible(false); // track visibility state
  }

  // Show Dialog
  if (m_spExitDlg != NULL)
  {
    m_spExitDlg->SetVisible(true);
    m_spGUIContext->SetActivate(true);
    m_spGUIContext->ShowDialog(m_spExitDlg);
  }
}

void VisSampleApp::EnsureGUIContextCreated()
{
  if (m_spGUIContext == NULL)
    m_spGUIContext = new VGUIMainContext(NULL);
}

#endif

void VisSampleApp::ProcessKeys ()
{
  // F1: Toggle help
  if (GetInputMap()->GetTrigger(SHOW_HELP))
  {
    m_bShowHelp = !m_bShowHelp;    
  }

  if (!(m_iSampleFlags&VSAMPLE_DISABLEDEFAULTKEYS))
  {
    // F2: Frame rate
    if (GetInputMap()->GetTrigger(SHOW_FRAME_RATE))
      m_bShowFrameRate = !m_bShowFrameRate;

    // F3: Line mode
#ifdef _VISION_PSP2
    if (GetInputMap()->GetTrigger(WIREFRAME_MODE) && !GetInputMap()->GetTrigger(EXIT))
#else
    if (GetInputMap()->GetTrigger(WIREFRAME_MODE))
#endif
    {
      if (Vision::Renderer.GetWireframeMode())
        Vision::Renderer.SetWireframeMode(false);
      else
        Vision::Renderer.SetWireframeMode(true);
    }
    
    // F4: Entity debug output : Cycle through the flags
    if (GetInputMap()->GetTrigger(SHOW_ENTITY_DEBUG_OUTPUT))
    {
      static int iFlags = 0; //no flags on
      iFlags <<=1 ; //move on to next flag
      if (iFlags==0)
        iFlags = 1;
      if (iFlags>=DEBUGRENDERFLAG_ALL)
        iFlags = 0; //start at the beginning again without any flag

      if (iFlags&DEBUGRENDERFLAG_VISION_VERSION)        
        Vision::Message.Add("Display vision version");
      if (iFlags&DEBUGRENDERFLAG_OBJECT_TRIANGLECOUNT)  
        Vision::Message.Add("Display object triangle count");
      if (iFlags&DEBUGRENDERFLAG_OBJECT_VISBBOX)        
        Vision::Message.Add("Display object visibility bounding box");
      if (iFlags&DEBUGRENDERFLAG_OBJECT_VISIBILITYZONES) 
        Vision::Message.Add("Display object vis area assignment");
      if (iFlags&DEBUGRENDERFLAG_LIGHT_INFLUENCEBOX)    
        Vision::Message.Add("Display light influence bounding box");
      if (iFlags&DEBUGRENDERFLAG_TRACELINES)            
        Vision::Message.Add("Display trace lines");
      if (iFlags&DEBUGRENDERFLAG_VISIBILITYOBJECTS)     
        Vision::Message.Add("Display visibility objects");
      if (iFlags&DEBUGRENDERFLAG_PORTALS)               
        Vision::Message.Add("Display portals");
      if (iFlags&DEBUGRENDERFLAG_VISIBILITYZONES)       
        Vision::Message.Add("Display visibility zones");
      if (iFlags&DEBUGRENDERFLAG_SCRIPTSTATISTICS)
      {
        if (Vision::GetScriptManager()!=NULL)
          Vision::Message.Add("Scripting statistics");
        else
          Vision::Message.Add("Scripting statistics (no script manager installed)");
      }        
      if (iFlags&DEBUGRENDERFLAG_OBJECTRENDERORDER)     
        Vision::Message.Add("Display object render order"); 
      if (iFlags&DEBUGRENDERFLAG_THREADWORKLOAD)        
        Vision::Message.Add("Display thread workload"); 
      if (iFlags&DEBUGRENDERFLAG_ZONES)                 
        Vision::Message.Add("Display streaming zones"); 

      Vision::Profiling.SetDebugRenderFlags(iFlags); 
    }

    // F5: Reload modified resources
    if (GetInputMap()->GetTrigger(RELOAD_MODIFIED_RESOURCES))
    {
      int iCount = Vision::ResourceSystem.ReloadModifiedResourceFiles(NULL, TRUE,TRUE);       

      // reassign all shaders
      Vision::Shaders.GetShaderFXLibManager().ResetCompiledEffectCaches();
      Vision::Shaders.ReloadAllShaderAssignmentFiles();

      Vision::Message.Add(1, "%i resources were outdated and have been reloaded",iCount);
    }

    // F6: Show the polygon count
    if (GetInputMap()->GetTrigger(SHOW_POLYGON_COUNT))
    {
      Vision::Profiling.ToggleObjectTriangleCounts();
      Vision::Profiling.ToggleOverallPolygons();
    }
    
    // F8: Save screenshot
    if (GetInputMap()->GetTrigger(SAVE_SCREEN_SHOT))
    {
      // Trigger taking a screenshot before the next back buffer swap
      m_bScreenshotPending = true;
    }
    
    // F9: previous profiling chart
    if (GetInputMap()->GetTrigger(SHOW_PREV_PROFILING_CHART))
    {
      Vision::Profiling.PreviousPage();    
    }

    // F10: next profiling chart
    if (GetInputMap()->GetTrigger(SHOW_NEXT_PROFILING_CHART))
    {
      Vision::Profiling.NextPage();
    }

    // F11: Reset profiling values
    if (GetInputMap()->GetTrigger(RESET_MAX_PROFILING_VALUES))
    {
      Vision::Profiling.ResetProfilingMaxValues();
    }
    
    // Tab: Show time step graph
    if (GetInputMap()->GetTrigger(SHOW_TIME_STEPS))
    {
      if (!m_pTimeStepGraph)
      {
        // Setup graph for displaying un-/ filtered time steps
        VGraphProps timeStepGraphProps;
        timeStepGraphProps.vPosition.set(-0.65f, 0.0f);  
        timeStepGraphProps.fWidth = 0.25f;
        timeStepGraphProps.fHeight = 0.25f;  
        timeStepGraphProps.iResolution = 64; 
        timeStepGraphProps.bRangeAdaptation = true;
        m_pTimeStepGraph = new VGraphObject(timeStepGraphProps);
        m_iUnfilteredTimeStepCurveIndex = m_pTimeStepGraph->AddCurve("Unfiltered Time Step", VColorRef(0, 255, 0));
        m_iTimeStepCurveIndex = m_pTimeStepGraph->AddCurve("Filtered Time Step", VColorRef(255, 0, 0));
        m_pTimeStepGraph->Init();
      }
      else
      {
        m_pTimeStepGraph->SetVisible(!m_pTimeStepGraph->IsVisible());
      }
    }

    // keypad  . : Attach the mouse camera (if it is initialized) to the location of
	//             the next CameraPositionEntity in the scene, if any are found.
    if (m_spCamera && GetInputMap()->GetTrigger(ATTACH_TO_NEXT_CAMERA_POSITION))
    {
      const int iCount = VisBaseEntity_cl::ElementManagerGetSize();
      if (m_iLastCameraPositionEntityIndex>=iCount)
        m_iLastCameraPositionEntityIndex = -1; // in case the scene changes, or out of range, just start over

      const int iStartingCamPosition = m_iLastCameraPositionEntityIndex;
      const VType* pCamPosType = Vision::GetTypeManager()->GetType("CameraPositionEntity");
      for (int i=m_iLastCameraPositionEntityIndex+1; i<=iCount; ++i)
      {
        // Release Camera when we wrap
        if (i>=iCount)
        {
          m_spCamera->DetachFromParent();
          m_iLastCameraPositionEntityIndex = -1;
          Vision::Message.Add(1, "Free Camera");
          break;
        }

        // Find the next camera entity in the scene and attach to it
        VisBaseEntity_cl *pInst = VisBaseEntity_cl::ElementManagerGet(i);
        if (pInst && pInst->GetTypeId()==pCamPosType)
        {
          m_spCamera->SetPosition(pInst->GetPosition());
          m_spCamera->SetOrientation(pInst->GetOrientation());
          m_spCamera->AttachToParent(pInst);

          m_iLastCameraPositionEntityIndex = i;

          Vision::Message.Add(1, "Attached to Camera: %s", pInst->GetObjectKeySafe());
          break;
        }
      }
      if (iStartingCamPosition==m_iLastCameraPositionEntityIndex)
        Vision::Message.Add(1, "No camera entities found in scene");
    }

    // keypad  + : Decrease perspective (Zoom in)
    if (GetInputMap()->GetTrigger(FOV_DECREASE))
      AdjustFOV(-1.f);

    // keypad  - : Increase perspective (Zoom out)
    if (GetInputMap()->GetTrigger(FOV_INCREASE))
      AdjustFOV(+1.f);
  }
  
  // Profiling Menu for TouchScreen only devices
#if defined(_VISION_MOBILE) && defined(HK_DEBUG)
  if (GetInputMap()->GetTrigger(SHOW_PROFILING_MENU1) && GetInputMap()->GetTrigger(SHOW_PROFILING_MENU2))
  {
    ShowProfilingMenu();
  }
#endif

  // Exit
#if (defined(WIN32) && !defined(_VISION_WINRT)) || defined(_VISION_ANDROID) || defined(_VISION_IOS)
  if (GetInputMap()->GetTrigger(EXIT)) 
#elif defined(_VISION_XENON) || defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU) || defined(_VISION_WINRT)
  if (GetInputMap()->GetTrigger(EXIT_COMBO) && GetInputMap()->GetTrigger(EXIT)) 
#else
  #error Undefined platform!
#endif
  {
#if defined(_VISION_ANDROID)
    // Check if button is handled by a derived class
    if (!OnAndroidBackButtonPressed())
    {
      // On Android, show exit dialog first.
      if ((m_iSampleFlags & VSAMPLE_SHOWEXITPROMPT) != 0 && m_appState == AS_RUNNING)
        ShowExitDialog();
      else
        Quit();
    }
  }

  // Check exit dialog on Android
  if (m_spGUIContext != NULL && m_spGUIContext->IsActive() &&
    m_spExitDlg != NULL && static_cast<VisMobileExitDialog*>(m_spExitDlg.GetPtr())->IsExitTriggered())
  {
#endif
    Quit();
  }

#ifdef _VISION_PS3
  // On PS3, check whether an error occurred, or whether the player closed the game
  if(cellSysutilCheckCallback() < 0) 
  {
    Quit();
  }
#endif

}

void VisSampleApp::DefineKeys ()
{

#if defined(SUPPORTS_KEYBOARD)

  Vision::GetConsoleManager()->SetAllowed(true); // debug console is always available
  //The following keys should only trigger once (former known as 'single hit') and not repeat when we press them

  GetInputMap()->MapTrigger(EXIT,               V_KEYBOARD, CT_KB_ESC, VInputOptions::Once());
  GetInputMap()->MapTrigger(EXIT_COMBO,         V_KEYBOARD, CT_KB_ESC, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_HELP,          V_KEYBOARD, CT_KB_F1 | EXCLUDE_MODIFIERS, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_FRAME_RATE,    V_KEYBOARD, CT_KB_F2 | EXCLUDE_MODIFIERS, VInputOptions::Once());
  GetInputMap()->MapTrigger(WIREFRAME_MODE,     V_KEYBOARD, CT_KB_F3 | EXCLUDE_MODIFIERS, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_ENTITY_DEBUG_OUTPUT,  V_KEYBOARD, CT_KB_F4 | EXCLUDE_MODIFIERS, VInputOptions::Once());
  GetInputMap()->MapTrigger(RELOAD_MODIFIED_RESOURCES, V_KEYBOARD, CT_KB_F5 | EXCLUDE_MODIFIERS, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_POLYGON_COUNT, V_KEYBOARD, CT_KB_F6 | EXCLUDE_MODIFIERS, VInputOptions::Once());
  GetInputMap()->MapTrigger(SAVE_SCREEN_SHOT,   V_KEYBOARD, CT_KB_F8 | EXCLUDE_MODIFIERS, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_PREV_PROFILING_CHART,  V_KEYBOARD, CT_KB_F9 | EXCLUDE_MODIFIERS,  VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_NEXT_PROFILING_CHART,  V_KEYBOARD, CT_KB_F10 | EXCLUDE_MODIFIERS, VInputOptions::Once());
  GetInputMap()->MapTrigger(RESET_MAX_PROFILING_VALUES, V_KEYBOARD, CT_KB_F11, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_TIME_STEPS, V_KEYBOARD, CT_KB_TAB | EXCLUDE_MODIFIERS, VInputOptions::Once());

  GetInputMap()->MapTrigger(ATTACH_TO_NEXT_CAMERA_POSITION, V_KEYBOARD, CT_KB_KP_PERIOD | EXCLUDE_MODIFIERS, VInputOptions::Once());

  GetInputMap()->MapTrigger(FOV_DECREASE, V_KEYBOARD, CT_KB_KP_PLUS);
  GetInputMap()->MapTrigger(FOV_INCREASE, V_KEYBOARD, CT_KB_KP_MINUS);

#endif
  
#if defined(_VISION_XENON) || ( defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO) )

  GetInputMap()->MapTrigger(EXIT, V_XENON_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  GetInputMap()->MapTrigger(EXIT_COMBO, V_XENON_PAD(0), CT_PAD_RIGHT_TRIGGER, VInputOptions::DeadZone(0.6f));

  GetInputMap()->MapTrigger(SHOW_HELP, V_XENON_PAD(0), CT_PAD_BACK, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_FRAME_RATE, V_XENON_PAD(1), CT_PAD_BACK, VInputOptions::Once());
  GetInputMap()->MapTrigger(WIREFRAME_MODE, V_XENON_PAD(1), CT_PAD_START, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_ENTITY_DEBUG_OUTPUT, V_XENON_PAD(1), CT_PAD_DOWN, VInputOptions::Once());
  GetInputMap()->MapTrigger(RELOAD_MODIFIED_RESOURCES, V_XENON_PAD(1), CT_PAD_UP, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_POLYGON_COUNT, V_XENON_PAD(1), CT_PAD_RIGHT, VInputOptions::Once());
  GetInputMap()->MapTrigger(SAVE_SCREEN_SHOT, V_XENON_PAD(1), CT_PAD_B, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_PREV_PROFILING_CHART, V_XENON_PAD(1), CT_PAD_X, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_NEXT_PROFILING_CHART, V_XENON_PAD(1), CT_PAD_Y, VInputOptions::Once());
  GetInputMap()->MapTrigger(RESET_MAX_PROFILING_VALUES, V_XENON_PAD(1), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_TIME_STEPS, V_XENON_PAD(1), CT_PAD_LEFT, VInputOptions::Once());

  GetInputMap()->MapTrigger(FOV_DECREASE, V_XENON_PAD(1), CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(FOV_INCREASE, V_XENON_PAD(1), CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.4f));
  #if defined(SUPPORTS_KEYBOARD)
    GetInputMap()->MapTrigger(EXIT, VInputManagerXenon::GetKeyboard(), CT_KB_ESC, VInputOptions::Once());
    GetInputMap()->MapTrigger(SHOW_HELP, VInputManagerXenon::GetKeyboard(), CT_KB_F1 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(SHOW_FRAME_RATE, VInputManagerXenon::GetKeyboard(), CT_KB_F2 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(WIREFRAME_MODE, VInputManagerXenon::GetKeyboard(), CT_KB_F3 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(SHOW_ENTITY_DEBUG_OUTPUT, VInputManagerXenon::GetKeyboard(), CT_KB_F4 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(RELOAD_MODIFIED_RESOURCES, VInputManagerXenon::GetKeyboard(), CT_KB_F5 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(SHOW_POLYGON_COUNT, VInputManagerXenon::GetKeyboard(), CT_KB_F6 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(SAVE_SCREEN_SHOT, VInputManagerXenon::GetKeyboard(), CT_KB_F8 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(SHOW_PREV_PROFILING_CHART, VInputManagerXenon::GetKeyboard(), CT_KB_F9 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(SHOW_NEXT_PROFILING_CHART, VInputManagerXenon::GetKeyboard(), CT_KB_F10 | EXCLUDE_MODIFIERS, VInputOptions::Once());
    GetInputMap()->MapTrigger(RESET_MAX_PROFILING_VALUES, VInputManagerXenon::GetKeyboard(), CT_KB_F11, VInputOptions::Once());

    GetInputMap()->MapTrigger(FOV_DECREASE, VInputManagerXenon::GetKeyboard(), CT_KB_KP_PLUS);
    GetInputMap()->MapTrigger(FOV_INCREASE, VInputManagerXenon::GetKeyboard(), CT_KB_KP_MINUS);
  #endif
#endif  

#if defined(_VISION_PS3)

  GetInputMap()->MapTrigger(EXIT, V_PS3_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  GetInputMap()->MapTrigger(EXIT_COMBO, V_PS3_PAD(0), CT_PAD_RIGHT_TRIGGER, VInputOptions::DeadZone(0.6f));

  GetInputMap()->MapTrigger(SHOW_HELP, V_PS3_PAD(0), CT_PAD_SELECT, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_FRAME_RATE, V_PS3_PAD(1), CT_PAD_SELECT, VInputOptions::Once());
  GetInputMap()->MapTrigger(WIREFRAME_MODE, V_PS3_PAD(1), CT_PAD_START, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_ENTITY_DEBUG_OUTPUT, V_PS3_PAD(1), CT_PAD_DOWN, VInputOptions::Once());
  GetInputMap()->MapTrigger(RELOAD_MODIFIED_RESOURCES, V_PS3_PAD(1), CT_PAD_UP, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_POLYGON_COUNT, V_PS3_PAD(1), CT_PAD_RIGHT, VInputOptions::Once());
  GetInputMap()->MapTrigger(SAVE_SCREEN_SHOT, V_PS3_PAD(1), CT_PAD_CIRCLE, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_PREV_PROFILING_CHART, V_PS3_PAD(1), CT_PAD_SQUARE, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_NEXT_PROFILING_CHART, V_PS3_PAD(1), CT_PAD_TRIANGLE, VInputOptions::Once());
  GetInputMap()->MapTrigger(RESET_MAX_PROFILING_VALUES, V_PS3_PAD(1), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_TIME_STEPS, V_PS3_PAD(1), CT_PAD_LEFT, VInputOptions::Once());

  GetInputMap()->MapTrigger(FOV_DECREASE, V_PS3_PAD(1), CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(FOV_INCREASE, V_PS3_PAD(1), CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.4f));
#endif

#if defined(_VISION_PSP2)
  GetInputMap()->MapTrigger(EXIT, V_PSP2_PAD(0), CT_PAD_SELECT);
  GetInputMap()->MapTrigger(EXIT_COMBO, V_PSP2_PAD(0), CT_PAD_START, VInputOptions::Once());

  GetInputMap()->MapTrigger(SHOW_FRAME_RATE, V_PSP2_PAD(0), CT_PAD_SELECT, VInputOptions::Once());
  GetInputMap()->MapTrigger(WIREFRAME_MODE, V_PSP2_PAD(0), CT_PAD_START, VInputOptions::Once());
  //GetInputMap()->MapTrigger(SHOW_TIME_STEPS, V_PSP2_PAD(0), CT_PAD_SQUARE, VInputOptions::Once());  
  
  //GetInputMap()->MapTrigger(SHOW_PREV_PROFILING_CHART, V_PSP2_PAD(0), CT_PAD_SQUARE, VInputOptions::Once());
  //GetInputMap()->MapTrigger(SHOW_NEXT_PROFILING_CHART, V_PSP2_PAD(0), CT_PAD_CIRCLE, VInputOptions::Once());
  //GetInputMap()->MapTrigger(SHOW_ENTITY_DEBUG_OUTPUT, V_PSP2_PAD(0), CT_PAD_TRIANGLE, VInputOptions::Once());
#endif
  
#if defined(_VISION_MOBILE) && defined(HK_DEBUG) 
  
  VTouchArea* profilingMenuArea1 = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(0, 0, 80, 80), -900.0f);
  GetInputMap()->MapTrigger(SHOW_PROFILING_MENU1, profilingMenuArea1, CT_TOUCH_ANY);
  
  int width = Vision::Video.GetXRes();
  int height = Vision::Video.GetYRes();
  
  VTouchArea* profilingMenuArea2 = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef((float)width - 80.f, (float)height - 80.f, (float)width, (float)height), -900.0f);
  GetInputMap()->MapTrigger(SHOW_PROFILING_MENU2, profilingMenuArea2, CT_TOUCH_ANY);
  
#endif

#if defined(_VISION_ANDROID)
  // Back button on android is mapped as the circle button.
  GetInputMap()->MapTrigger(EXIT, VInputManagerAndroid::GetKeyInput(), CT_PAD_ANDROID_BACK, VInputOptions::Once());
#endif

#if defined(_VISION_WIIU)
  GetInputMap()->MapTrigger(EXIT, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());
  GetInputMap()->MapTrigger(EXIT_COMBO, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_TRIGGER, VInputOptions::DeadZone(0.6f));

  GetInputMap()->MapTrigger(SHOW_PREV_PROFILING_CHART, VInputManagerWiiU::GetPad(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_NEXT_PROFILING_CHART, VInputManagerWiiU::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());

  GetInputMap()->MapTrigger(SHOW_HELP,                 VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_SELECT, VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_FRAME_RATE,           VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_SELECT, VInputOptions::Once());
  GetInputMap()->MapTrigger(WIREFRAME_MODE,            VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_START,  VInputOptions::Once());
  
  GetInputMap()->MapTrigger(SHOW_ENTITY_DEBUG_OUTPUT,  VInputManagerWiiU::GetPad(0), CT_PAD_DOWN,     VInputOptions::Once());
  GetInputMap()->MapTrigger(RELOAD_MODIFIED_RESOURCES, VInputManagerWiiU::GetPad(0), CT_PAD_UP,       VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_POLYGON_COUNT,        VInputManagerWiiU::GetPad(0), CT_PAD_RIGHT,    VInputOptions::Once());
  GetInputMap()->MapTrigger(SAVE_SCREEN_SHOT,          VInputManagerWiiU::GetPad(0), CT_PAD_A,   VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_PREV_PROFILING_CHART, VInputManagerWiiU::GetPad(0), CT_PAD_Y,   VInputOptions::Once());
  GetInputMap()->MapTrigger(SHOW_NEXT_PROFILING_CHART, VInputManagerWiiU::GetPad(0), CT_PAD_X, VInputOptions::Once());
  GetInputMap()->MapTrigger(RESET_MAX_PROFILING_VALUES,VInputManagerWiiU::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::Once());

  GetInputMap()->MapTrigger(FOV_DECREASE, VInputManagerWiiU::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP,   VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(FOV_INCREASE, VInputManagerWiiU::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.4f));
#endif

}

//////////////////////////////////////////////////////////////////////////////
//Adjusts the field-of-view  + or -

void VisSampleApp::AdjustFOV(float fDir)
{
  float d = Vision::GetUITimer()->GetTimeDifference() * 10.0f;

  if (Vision::Renderer.GetRendererNode(0) != NULL)
  {
    float h,v;
    Vision::Renderer.GetRendererNode(0)->GetViewProperties()->getFov(h, v);
    h += fDir*d;
    h = hkvMath::clamp(h, 35.0f, 160.0f);
    Vision::Renderer.GetRendererNode(0)->GetViewProperties()->setFov(h, 0.0f);
    Vision::Renderer.GetRendererNode(0)->OnViewPropertiesChanged();
  }
  else
  {
    float h,v;
    VisRenderContext_cl::GetMainRenderContext()->GetFOV(h,v);
    h += fDir*d;
    
    if (h > 160.f) h = 160;
    if (h < 35.f)  h = 35.f;
    VisRenderContext_cl::GetMainRenderContext()->SetFOV(h, 0.0f);
    Vision::Message.Add(1,"FOV is now: %f", h);
  }
}



//////////////////////////////////////////////////////////////////////////////
//  LOADING SCREEN
//////////////////////////////////////////////////////////////////////////////

void VisSampleApp::SetupLoadingScreen(bool bStatus, const char *szTextureFile, float fFadeOutTime,
  int iProgressBarLeft, int iProgressBarTop, int iProgressBarRight, int iProgressBarBottom)
{
  m_bShowLoadingProgress = bStatus;
  m_loadingScreenSettings.sBackgroundImagePath = szTextureFile;
  m_loadingScreenSettings.eAspectRatioAlignment = LoadingScreenSettings::ALIGN_HORIZONTAL;
  m_loadingScreenSettings.backgroundColor = HAVOK_COLOR_BACK;
  m_loadingScreenSettings.fFadeOutTime = fFadeOutTime;
  m_loadingScreenSettings.bForceFullScreenTexture = false;

  m_loadingScreenSettings.iProgressBarLeft = iProgressBarLeft;
  m_loadingScreenSettings.iProgressBarTop = iProgressBarTop;
  m_loadingScreenSettings.iProgressBarRight = iProgressBarRight;
  m_loadingScreenSettings.iProgressBarBottom = iProgressBarBottom;
}

void VisSampleApp::SetupLoadingScreen( bool bStatus, const LoadingScreenSettings& settings )
{
  m_bShowLoadingProgress = bStatus;
  m_loadingScreenSettings = settings;
}

// LoadWorld callback
void VisSampleApp::OnLoadSceneStatus(int iStatus, float fPercentage, const char* pszStatus)
{
  VisionApp_cl::OnLoadSceneStatus(iStatus, fPercentage, pszStatus); // important! call the base implementation

  if (!m_bShowLoadingProgress || m_sceneLoader.IsAborted())
    return;

  static uint64 uiLastTimeRendered = 0;
  static uint64 uiStartTime = 0;
  const uint64 uiCurrentTime = VGLGetTimer();

  switch (iStatus)
  {
  case VIS_API_LOADSCENESTATUS_START:
    // Suspend rendering for frequent updates
    Vision::Video.SuspendRendering();

    m_iInsideLoadWorldCounter++; 
    if (m_iInsideLoadWorldCounter == 1) // this handles the load world inside the load scene
    {
      float fScreenLeft, fScreenTop, fScreenRight, fScreenBottom;
      GetLoadingScreenExtents(fScreenLeft, fScreenTop, fScreenRight, fScreenBottom);

      VTextureObjectPtr spWhite = Vision::TextureManager.Load2DTexture("\\plainwhite.dds");
      spWhite->SetResourceFlag(VRESOURCEFLAG_AUTODELETE); // delete the texture when finished

      // Background for the splash screen
      // Always use background color to hide other menu elements 
      // (also in case no background image is visible)
      m_spBackgroundColor = new VisScreenMask_cl();
      m_spBackgroundColor->SetTextureObject(spWhite);
      m_spBackgroundColor->SetColor(m_loadingScreenSettings.backgroundColor);
      m_spBackgroundColor->SetTransparency(VIS_TRANSP_ALPHA);
      m_spBackgroundColor->SetDepthWrite(FALSE);
      m_spBackgroundColor->SetPos(0.0f, 0.0f);
      m_spBackgroundColor->SetTargetSize(fScreenRight, fScreenBottom);
      m_spBackgroundColor->SetOrder(-50);

      if (!m_loadingScreenSettings.sBackgroundImagePath.IsEmpty())
      {   
        m_spBackground = new VisScreenMask_cl(m_loadingScreenSettings.sBackgroundImagePath);
        m_spBackground->SetDepthWrite(FALSE);
        m_spBackground->SetTransparency(VIS_TRANSP_ALPHA);
        m_spBackground->SetOrder(-60);
        m_spBackground->GetTextureObject()->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

        if ( m_loadingScreenSettings.eAspectRatioAlignment != LoadingScreenSettings::ALIGN_NONE )
        {
          // Set up the splash screen so that the aspect ratio is maintained.
          int iResX, iResY;
          m_spBackground->GetTextureSize(iResX, iResY);

          LoadingScreenSettings::AspectRatioAlignment alignment = m_loadingScreenSettings.eAspectRatioAlignment;
          if ( alignment == LoadingScreenSettings::ALIGN_FULL )
          {
            // Compare the texture's aspect ratio with that of the screen...
            float fRatioScreen = fScreenRight / fScreenBottom;
            float fRatioTexture = iResX / float( iResY );

            // and use that to determine our texture's alignment
            if ( fRatioTexture > fRatioScreen )
            {
              alignment = LoadingScreenSettings::ALIGN_HORIZONTAL;
            }
            else
            {
              alignment = LoadingScreenSettings::ALIGN_VERTICAL;
            }
          }

          float fTargetX = ( alignment == LoadingScreenSettings::ALIGN_HORIZONTAL ) 
            ? fScreenRight : ( fScreenBottom / float( iResY ) * float( iResX ) );
          float fTargetY = ( alignment == LoadingScreenSettings::ALIGN_HORIZONTAL ) 
            ? ( fScreenRight / float( iResX ) * float( iResY ) ) : fScreenBottom;

          float fPosX = (fScreenRight - fTargetX) / 2.0f;
          float fPosY = (fScreenBottom - fTargetY) / 2.0f;

          if ( m_loadingScreenSettings.bForceFullScreenTexture )
          {
            // Just draw a fullscreen quad,...
            m_spBackground->SetPos( 0.0f, 0.0f );
            m_spBackground->SetTargetSize( fScreenRight, fScreenBottom );

            // ...but adjust texture coordinates to cater for the background image placement
            float fDeltaU = fPosX * ( iResX / fTargetX );
            float fDeltaV = fPosY * ( iResY / fTargetY );
            m_spBackground->SetTextureRange( -fDeltaU, -fDeltaV, iResX + fDeltaU, iResY + fDeltaV );

            // Oh, and don't forget to clamp!
            m_spBackground->SetWrapping( FALSE, FALSE );
          }
          else
          {
            m_spBackground->SetTargetSize(fTargetX, fTargetY);
            m_spBackground->SetPos(fPosX, fPosY);
          }
        }
        else
        {
          m_spBackground->SetPos(0.0f, 0.0f);
          m_spBackground->SetTargetSize(fScreenRight, fScreenBottom);
        }
      }
      else
      {
        m_spBackground = NULL;
        m_loadingScreenSettings.fFadeOutTime = 0.0f;
      }

      if (m_iSampleFlags & VSAMPLE_HAVOKLOGO)
      {
#if defined( HK_ANARCHY )
        m_spLogoOverlay = new VisScreenMask_cl("Textures\\Anarchy_Logo_128x128.dds");
#else
        m_spLogoOverlay = new VisScreenMask_cl("Textures\\Havok_Logo_128x64.dds");
#endif
        m_spLogoOverlay->SetDepthWrite(FALSE);
        m_spLogoOverlay->SetTransparency (VIS_TRANSP_ALPHA);
        m_spLogoOverlay->SetFiltering(FALSE);

        float fWidth = 0.0f, fHeight = 0.0f;
        m_spLogoOverlay->GetTargetSize(fWidth, fHeight);
        float fGapBottom = 2.0f;
        if(m_iSampleFlags & VSAMPLE_ALIGNLOGOALTERNATIVE)
        {
          m_spLogoOverlay->SetPos(16.f, fScreenBottom - fGapBottom - fHeight);
          m_iXDist += 19;
        } 
        else
        {
          float fGapRight = 18.0f;
          m_spLogoOverlay->SetPos(fScreenRight - fGapRight - fWidth, fScreenBottom - fGapBottom - fHeight);
        }

        m_spLogoOverlay->SetColor(VColorRef(255, 255, 255, m_bLogoFadeInDone ? 255 : 0));
        m_spLogoOverlay->GetTextureObject()->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
      }

      float fBarLeft, fBarTop, fBarRight, fBarBottom;
      GetProgressBarExtents(fBarLeft, fBarTop, fBarRight, fBarBottom);

      // Progress bar 
      m_spProgressBar = new VisScreenMask_cl();
      m_spProgressBar->SetTextureObject(spWhite);
      m_spProgressBar->SetColor(VColorRef(253,184,19,128));
      m_spProgressBar->SetTransparency(VIS_TRANSP_ADDITIVE);
      m_spProgressBar->SetDepthWrite(FALSE);
      m_spProgressBar->SetPos(fBarLeft + 4.f, fBarTop + 3.f);
      m_spProgressBar->SetTargetSize(2.f, fBarBottom - fBarTop - 6.f);
      m_spProgressBar->SetOrder(-80);

      // Progress bar background
      m_spProgressBack = new VisScreenMask_cl();
      m_spProgressBack->SetTextureObject(spWhite);
      m_spProgressBack->SetColor( VColorRef(100,100,100,128));
      m_spProgressBack->SetTransparency(VIS_TRANSP_ADDITIVE);
      m_spProgressBack->SetDepthWrite(FALSE);
      m_spProgressBack->SetPos(fBarLeft, fBarTop);
      m_spProgressBack->SetTargetSize(fBarRight - fBarLeft, fBarBottom - fBarTop);
      m_spProgressBack->SetOrder(-70);

      uiStartTime = VGLGetTimer(); // get the current time
      uiLastTimeRendered = uiStartTime;
    }
    break;

  case VIS_API_LOADSCENESTATUS_FINISHED:
    VASSERT(m_iInsideLoadWorldCounter > 0);
    m_iInsideLoadWorldCounter--;
    if (m_iInsideLoadWorldCounter==0) // this handles the loadworld inside the load scene
    {
      // don't forget to remove screenmasks otherwise they will cover the world after it is loaded    
      m_spProgressBar = NULL;
      m_spProgressBack = NULL;
      m_fCurrentFadeOutTime = m_loadingScreenSettings.fFadeOutTime;

      if (m_fCurrentFadeOutTime <= 0.f)
      {
        // background not fading out -> not needed anymore
        m_spBackgroundColor = NULL;
        m_spBackground = NULL;
      }
      else
      {
        m_fFadeOutPos = 1.f;
      }

      float fLoadingTime = float( VGLGetTimer() - uiStartTime ) / float( VGLGetTimerResolution() );
      Vision::Error.SystemMessage("Time for loading : %.2fs", fLoadingTime);
    }
    break;

  case VIS_API_LOADSCENESTATUS_PROGRESS:
    {
      // only update the loading screen 5 times a second, otherwise rendering the loading screen slows down
      // performance too much (esp. if vertical sync is enabled)
      const uint64 uiUpdateTime = uint64( ( 1.0f / 5.0f ) * float( VGLGetTimerResolution() ) );
      if ((uiCurrentTime-uiLastTimeRendered)<uiUpdateTime)
        return;

#ifdef _VISION_PS3
      // Check for system callbacks while progressing
      if(sysUtilCheckCallbacks() < 0) 
      {
        Quit();
      }
#endif

      if (m_spProgressBar)
      {
        float fBarLeft, fBarTop, fBarRight, fBarBottom;
        GetProgressBarExtents(fBarLeft, fBarTop, fBarRight, fBarBottom);

        int iBarX,iBarY;
        m_spProgressBar->GetTextureSize(iBarX, iBarY);
        float fScrWidth = (0.01f*fPercentage) * (fBarRight - fBarLeft - 8.f);
        float fTexWidth = (0.01f*fPercentage) * static_cast<float>(iBarX);
        if (fScrWidth < 3.f) fScrWidth = 3.f;
        m_spProgressBar->SetTextureRange(0.f, 0.f, fTexWidth, static_cast<float>(iBarY));
        m_spProgressBar->SetTargetSize(fScrWidth, fBarBottom - fBarTop - 6.f);
      }
      break;
    }
  case VIS_API_LOADMODELSTATUS_START:
  case VIS_API_LOADMODELSTATUS_FINISHED:
  case VIS_API_LOADMODELSTATUS_PROGRESS:
    // this check is important in case an extra status bar for models is to be displayed
    // without this check, the model status bar would also be rendered when loading models within the game
    if (!m_iInsideLoadWorldCounter)
      return;
    // do not do anything while model loading in this implementation anyway, but this can be changed
    return;
  };

  if (m_appState == AS_SUSPENDED)
    return; // Do not render
  
  uiLastTimeRendered = uiCurrentTime;

  // This is a HACK
  // The progress bar has an abort state and VSceneLoader has an abort state, which are not necessarily in sync.
  // The progress bar should not have an abort state, since it is for displaying the loading state only.
  // Instead 'abort' should always be executed on the VSceneLoader (IMO).
  if (!GetLoadingProgress().WantsAbort ())
  {
    // ResumeRendering ensures that we have control over the rendering device (Xbox360/PS3 only)
    Vision::Video.ResumeRendering();

    VisRenderContext_cl::GetMainRenderContext()->Activate();
    Vision::RenderScreenMasks();

    #if defined(WIN32)  || defined(_VISION_XENON)   || defined(_VISION_PSP2) || defined(_VISION_ANDROID) || defined(_VISION_WIIU) || defined(_VISION_IOS)
      // On PS3, don't flip yet, because this would modify the backbuffer when rendering system overlays during a flip.
      Vision::Video.UpdateScreen();
    #elif defined(_VISION_PS3)
    #else
      #error "Missing code!"
    #endif
  }


  // Suspend ensures that buffers are flipped frequently enough to ensure TRC/TCR compliance on consoles (Xbox360/PS3 only)
  if (m_iInsideLoadWorldCounter)
  {
    Vision::Video.SuspendRendering();
  }

#ifdef _VISION_PS3
  // We're finished loading, so we don't need the internally created temporary blit texture any more.
  if (!m_iInsideLoadWorldCounter)
  {
    Vision::Video.UpdateScreen();
    Vision::Video.FreeSuspendedRenderingBlitTexture();
  }
#endif
}

#if defined( SUPPORTS_FILESERVE_CLIENT )
bool VisSampleApp::SetupFileServeClient(bool bUseAppDataFallback)
{
#if defined( _VISION_MOBILE )
  const char* pszCachePath = GetCacheDirectory();
#elif defined( WIN32 )
  char pszCachePath[ FS_MAX_PATH ];
  GetTempPathA( V_ARRAY_SIZE( pszCachePath ), pszCachePath );
#else
#error "Platform not supported!"
#endif

#if defined( _VISION_IOS )
  VStaticString< FS_MAX_PATH > sSettingsFilePath = GetDocumentsDirectory();
#elif defined( _VISION_ANDROID ) || defined( _VISION_APOLLO )
  VStaticString< FS_MAX_PATH > sSettingsFilePath = GetSDCardDirectory();
#else
  VStaticString< FS_MAX_PATH > sSettingsFilePath = ".";
#endif

  sSettingsFilePath += "/vFileServeHost.txt";

  Vision::Error.SystemMessage ("Searching for file-serve configuration in '%s'", sSettingsFilePath.AsChar ());
  
  bool bConnected = false;

  // Try to open the file
  IVFileStreamManager *pFileStreamMgr = VBase_GetFileStreamManager();
  VASSERT(pFileStreamMgr != NULL);
  IVFileInStream *pStream = pFileStreamMgr->Open(sSettingsFilePath);
  if(pStream == NULL)
  {
#if defined(_VISION_IOS)
    // Fall back to the AppDirectory if no config was found in the Documents Directory
    sSettingsFilePath = GetRootDirectory();
    sSettingsFilePath += "/vFileServeHost.txt";
    Vision::Error.SystemMessage ("Searching for file-serve configuration in '%s'", sSettingsFilePath.AsChar ());
    // Try to open the file
    pStream = pFileStreamMgr->Open(sSettingsFilePath);
    if(pStream == NULL)
    {
      Vision::Error.Warning("Unable to find file serve configuration (please look up documentation for details)");
      s_bIsFileserverInstalled = false;
      return false;
    }
#else
    Vision::Error.Warning("Unable to find file serve configuration (please look up documentation for details)");
    s_bIsFileserverInstalled = false;
    return false;
#endif
  }

  // Read string
  char szHostUrl[1024];
  size_t uiLen = pStream->Read(szHostUrl, V_ARRAY_SIZE(szHostUrl)-1);
  pStream->Close();
  pStream = NULL;
  szHostUrl[uiLen] = '\0';

  VFileServeStreamManager *pFSSM = new VFileServeStreamManager( szHostUrl, pszCachePath );
  VASSERT_MSG( pFSSM != NULL, "Unable to create VFileServeStreamManager!" );

  bConnected = pFSSM->IsConnected();
  if ( !bConnected )
  {
    Vision::Error.Warning( "Unable to connect to file server!" );
  }

  if (bConnected || !bUseAppDataFallback)
  {
    // Only set the VFileServeStreamManager if it successfully connected or if AppData fallback is disabled.
    // If it is not connected and AppData fallback is disabled, the cached data will be used.
    Vision::File.SetManager( pFSSM );
    Vision::File.SetUseAbsolutePaths( false );
    s_bIsFileserverInstalled = true;
  }
  else
  {
    s_bIsFileserverInstalled = false;
  }

  return bConnected;
}
#endif

void VisSampleApp::LoadVisionEnginePlugin()
{
#ifndef DISABLE_VISION_ENGINE_PLUGIN
  VISION_PLUGIN_ENSURE_LOADED(VisionEnginePlugin);
#endif
}

void VisSampleApp::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if(pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
    float fScreenLeft, fScreenTop, fScreenRight, fScreenBottom;
    GetLoadingScreenExtents(fScreenLeft, fScreenTop, fScreenRight, fScreenBottom);

    if(m_spLogoOverlay != NULL)
    {
      float fWidth = 0.0f, fHeight = 0.0f;
      m_spLogoOverlay->GetTargetSize(fWidth, fHeight);
      float fGapBottom = 2.0f;
      if(m_iSampleFlags & VSAMPLE_ALIGNLOGOALTERNATIVE)
      {
        m_spLogoOverlay->SetPos(16.f, fScreenBottom - fGapBottom - fHeight );
      } 
      else
      {
        float fGapRight = 18.0f;
        m_spLogoOverlay->SetPos( fScreenRight - fGapRight - fWidth, fScreenBottom - fGapBottom - fHeight );
      }
    }
  }
  // OnLeaveForeground
  else if (pData->m_pSender == &Vision::Callbacks.OnLeaveForeground)
  {
    // Grab screen shot for the next loading screen.
    // Only do this if application is running. If scene loading has been
    // interrupted, grabbing a screen shot doesn't make sense.
    if (m_appState == AS_RUNNING)
    {
#if defined(_VISION_IOS)
      // On iOS we have to redraw the frame and wait for the callback
      // right before the back buffer is presented on the screen.
      // Afterwards the contents of the back buffer are cleared.
      m_bScreenshotForBackgroundPending = true;
      Run(); // <- Vision::Callbacks.OnBeforeSwapBuffers is triggered in here
      m_bScreenshotForBackgroundPending = false;
#else
      StoreScreenShotAsLoadBackgroundStart();
#endif
    }

    // Backup loading screen settings as StoreScreenShotAsLoadBackgroundEnd might
    // modify them for an upcoming restore process.
    m_loadingScreenSettingsBackup = m_loadingScreenSettings;
  }
  // OnEnterBackground
  else if (pData->m_pSender == &Vision::Callbacks.OnEnterBackground)
  {
    // Process and save screen shot only now to prevent Android from
    // detecting the application as not responding.
    // This call will only have an effect if StoreScreenShotAsLoadBackgroundStart 
    // has been executed previously.
    StoreScreenShotAsLoadBackgroundEnd("vision_background.bmp", 0.7f, 0.2f);

    // Disable calling the run function
    m_appState = AS_SUSPENDED;

    // Shut down loading screen (needs to be re-initialized completely after
    // backgrounding)
    while (m_iInsideLoadWorldCounter > 0)
    {
      OnLoadSceneStatus(VIS_API_LOADSCENESTATUS_FINISHED, 100.0f, "");
    }
    m_pBackgroundResourceRestorer = NULL;

    // destroy resources needed for the loading screen to make
    // sure they get recreated as soon as the restoring process starts.
    m_spBackgroundColor = NULL;
    m_spBackground = NULL;
    m_spLogoOverlay = NULL;
    m_spProgressBar = NULL;
    m_spProgressBack = NULL;
  }
  // OnBackgroundRestore
  else if (pData->m_pSender == &Vision::Callbacks.OnBackgroundRestore)
  {
    VisBackgroundRestoreObject_cl* pProgressData = static_cast<VisBackgroundRestoreObject_cl*>(pData);

    if (m_pBackgroundResourceRestorer == NULL) // restoring has started
    {
      // Set background restorer object
      m_pBackgroundResourceRestorer = pProgressData->m_pBackgroundResourceRestorer;
      m_appState = AS_LOADING;

      // set up loading screen (will be re-used by normal loading process 
      // -> if !m_sceneLoader.IsFinished())
      OnLoadSceneStatus(VIS_API_LOADSCENESTATUS_START, pProgressData->m_percentage, "");
    }

    // Update progress bar
    OnLoadSceneStatus(VIS_API_LOADSCENESTATUS_PROGRESS, pProgressData->m_percentage, "");
  }
  // OnEnterForeground
  else if (pData->m_pSender == &Vision::Callbacks.OnEnterForeground)
  {
    // Re-set loading screen settings (only if restoration has finished)
    if (m_pBackgroundResourceRestorer != NULL)
    {
      m_appState = m_sceneLoader.IsFinished() ? AS_RUNNING : AS_LOADING;
      m_pBackgroundResourceRestorer = NULL; // not needed anymore

      // Disable loading screen (only if normal scene loading has finished)
      if (m_sceneLoader.IsFinished())
        OnLoadSceneStatus(VIS_API_LOADSCENESTATUS_FINISHED, 100.0f, "");  
    }

    // Restore loading screen settings
    m_loadingScreenSettings = m_loadingScreenSettingsBackup;
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnBeforeSwapBuffers)
  {
    if (m_bScreenshotPending)
    {
      if (Vision::Game.SaveScreenShot())
        Vision::Message.Add("Saved screenshot ...", 1);
      else
        Vision::Message.Add("Screenshot could not be saved! Write protection?", 1);

      // Re-set
      m_bScreenshotPending = false;
    }

#if defined(_VISION_IOS)
    // Delayed screen shot when backgrounding the app on iOS
    if (m_bScreenshotForBackgroundPending)
    {
      StoreScreenShotAsLoadBackgroundStart();
      m_bScreenshotForBackgroundPending = false;
    }
#endif
  }
}

//-----------------------------------------------------------------------------------
// Loading screen helpers

void VisSampleApp::GetLoadingScreenExtents(float& fLeft, float& fTop, 
  float& fRight, float& fBottom) const
{
  int iScreenLeft = 0;
  int iScreenTop = 0;
  int iScreenRight = Vision::Video.GetXRes();
  int iScreenBottom = Vision::Video.GetYRes();

  if (m_bTripleHead)
  {
    //put it on the center screen
    int iWidth = Vision::Video.GetXRes()/3;
    iScreenLeft = iWidth;
    iScreenRight = iScreenLeft + iWidth;
  }

  fLeft = static_cast<float>(iScreenLeft);
  fTop = static_cast<float>(iScreenTop);
  fRight = static_cast<float>(iScreenRight);
  fBottom = static_cast<float>(iScreenBottom);
}

void VisSampleApp::GetProgressBarExtents(float& fProgressBarLeft, 
  float& fProgressBarTop, float& fProgressBarRight, float& fProgressBarBottom) const
{
  float fScreenLeft, fScreenTop, fScreenRight, fScreenBottom;
  GetLoadingScreenExtents(fScreenLeft, fScreenTop, fScreenRight, fScreenBottom);

  if (m_loadingScreenSettings.iProgressBarLeft == -1 || m_spBackground == NULL)
  {
#ifdef _VISION_PSP2
    float marginX = 50.f * (fScreenRight - fScreenLeft) / 1280.f;
    float marginY = 16.f * (fScreenBottom - fScreenTop) / 960.f;
    float y_align = (fScreenTop + 3.f*fScreenBottom) / 4.f;

    fProgressBarLeft = fScreenLeft + marginX; 
    fProgressBarTop = y_align - 4.f;
    fProgressBarRight = fScreenRight - marginX; 
    fProgressBarBottom = y_align + marginY + 3.f;

#else
    float y_align = (fScreenTop + 3.f*fScreenBottom) / 4.f;

    fProgressBarLeft = fScreenLeft + 100.f; 
    fProgressBarTop = y_align;
    fProgressBarRight = fScreenRight - 100.f; 
    fProgressBarBottom = y_align + 20.f;

#endif
  } 
  else
  {
    int iTexWidth, iTexHeight;
    m_spBackground->GetTextureSize(iTexWidth, iTexHeight);
    float fTexWidth = static_cast<float>(iTexWidth);
    float fTexHeight = static_cast<float>(iTexHeight);

    // transform to screen
    float fLeftFrac = static_cast<float>(m_loadingScreenSettings.iProgressBarLeft) / fTexWidth;
    float fTopFrac = static_cast<float>(m_loadingScreenSettings.iProgressBarTop) / fTexHeight;
    float fRightFrac = static_cast<float>(m_loadingScreenSettings.iProgressBarRight) / fTexWidth;
    float fBottomFrac = static_cast<float>(m_loadingScreenSettings.iProgressBarBottom) / fTexHeight;

    float fBackgroundPosX, fBackgroundPosY;
    float fBackgroundWidth, fBackgroundHeight;
    m_spBackground->GetPos(fBackgroundPosX, fBackgroundPosY);
    m_spBackground->GetTargetSize(fBackgroundWidth, fBackgroundHeight);

    fProgressBarLeft = fBackgroundPosX + fLeftFrac*fBackgroundWidth + 0.5f;
    fProgressBarTop = fBackgroundPosY + fTopFrac*fBackgroundHeight + 0.5f;
    fProgressBarRight= fBackgroundPosX + fRightFrac*fBackgroundWidth + 0.5f;
    fProgressBarBottom = fBackgroundPosY + fBottomFrac*fBackgroundHeight + 0.5f;
  }
}

//-----------------------------------------------------------------------------------
// StoreScreenShotAsBackground

void VisSampleApp::StoreScreenShotAsLoadBackgroundStart()
{
  const int iScreenWidth = Vision::Video.GetXRes();
  const int iScreenHeight = Vision::Video.GetYRes();

  V_SAFE_DELETE_ARRAY(m_pTmpBackgroundScreenshot);
  m_pTmpBackgroundScreenshot = new unsigned char[iScreenWidth*iScreenHeight*3];

  // write the screen buffer to memory buffer
  if (!Vision::Game.WriteScreenToBuffer(0, 0, iScreenWidth, iScreenHeight, 
    (UBYTE*)m_pTmpBackgroundScreenshot))
  {
    V_SAFE_DELETE_ARRAY(m_pTmpBackgroundScreenshot);
  }
}

void VisSampleApp::StoreScreenShotAsLoadBackgroundEnd(const char* szFileName, 
  float fBrightness, float fSaturation)
{
  if (m_pTmpBackgroundScreenshot == NULL)
    return; // no screen shot available to process

  const int iScreenWidth = Vision::Video.GetXRes();
  const int iScreenHeight = Vision::Video.GetYRes();

  // flip buffer vertically and swap red <-> blue channel
  float fLumFactors[3] = { 0.3f * fBrightness, 0.59f * fBrightness, 0.11f * fBrightness };
  const float fInvSaturation = 1.0f - fSaturation;
  for (int y = 0; y < iScreenHeight/2; y++)
  {
    unsigned char *pRow1 = &m_pTmpBackgroundScreenshot[y*iScreenWidth*3];
    unsigned char *pRow2 = &m_pTmpBackgroundScreenshot[(iScreenHeight-y-1)*iScreenWidth*3];

    for (int x = 0; x < iScreenWidth ; x++, pRow1 += 3, pRow2 += 3)
    {
      unsigned char tmp[3] = { pRow1[0], pRow1[1], pRow1[2] };

      const float fLum1 = 
        static_cast<float>(pRow1[0]) * fLumFactors[0] +
        static_cast<float>(pRow1[1]) * fLumFactors[1] +
        static_cast<float>(pRow1[2]) * fLumFactors[2];
      const float fLum2 = 
        static_cast<float>(pRow2[0]) * fLumFactors[0] +
        static_cast<float>(pRow2[1]) * fLumFactors[1] +
        static_cast<float>(pRow2[2]) * fLumFactors[2];

      // flip and swap interpolated value
      float fInvSaturationProduct = fInvSaturation * fLum2;
      pRow1[0] = hkvMath::Min(255, static_cast<unsigned char>(
        fSaturation * static_cast<float>(pRow2[2]) + fInvSaturationProduct));
      pRow1[1] = hkvMath::Min(255, static_cast<unsigned char>(
        fSaturation * static_cast<float>(pRow2[1]) + fInvSaturationProduct));
      pRow1[2] = hkvMath::Min(255, static_cast<unsigned char>(
        fSaturation * static_cast<float>(pRow2[0]) + fInvSaturationProduct));

      fInvSaturationProduct = fInvSaturation * fLum1;
      pRow2[0] = hkvMath::Min(255, static_cast<unsigned char>(
        fSaturation * static_cast<float>(tmp[2]) + fInvSaturationProduct));
      pRow2[1] = hkvMath::Min(255, static_cast<unsigned char>(
        fSaturation * static_cast<float>(tmp[1]) + fInvSaturationProduct));
      pRow2[2] = hkvMath::Min(255, static_cast<unsigned char>(
        fSaturation * static_cast<float>(tmp[0]) + fInvSaturationProduct));
    }
  }

  // use VTEX to scale and save the image
  Image_cl image;
  ImageMap_cl colorMap(iScreenWidth, iScreenHeight, 24, 
    static_cast<UBYTE*>(m_pTmpBackgroundScreenshot));
  image.AddColorMap(colorMap);
  image.Scale(512, 512);

  // build screen shot path
  const char* szScreenShotFileName = "vision_background.bmp";
  char szScreenShotFilePath[FS_MAX_PATH];
#if defined(_VISION_MOBILE)
  strcpy(szScreenShotFilePath, GetCacheDirectory());
  strcat(szScreenShotFilePath, "/");
  strcat(szScreenShotFilePath, szScreenShotFileName);
#else
  strcpy(szScreenShotFilePath, szScreenShotFileName);
#endif

  IVFileOutStream* pOut = Vision::File.Create(szScreenShotFilePath);
  const bool bSaved = (image.SaveBMP(pOut) == VERR_NOERROR);
  if (pOut)
    pOut->Close();
  V_SAFE_DELETE_ARRAY(m_pTmpBackgroundScreenshot);

  if (!bSaved)
    return;

  // Set new background loading image
  m_loadingScreenSettings.sBackgroundImagePath = szScreenShotFilePath; 
  m_loadingScreenSettings.eAspectRatioAlignment = LoadingScreenSettings::ALIGN_NONE; // stretch over the whole screen

  // Disable custom progress bar rectangle and override fade out time
  m_loadingScreenSettings.iProgressBarLeft = -1;
  m_loadingScreenSettings.iProgressBarRight = -1;
  m_loadingScreenSettings.iProgressBarTop = -1;
  m_loadingScreenSettings.iProgressBarBottom = -1;
  m_loadingScreenSettings.fFadeOutTime = 0.5f;
}

// Just check if the desired resolution in fullscreen is supported by the adapter.
// It returns false if not supported or cannot check it
// It returns true if it is supported
// In both cases, it always returns in selectedX and selectedY the correct resolution the application should have in fullscreen
#if defined(WIN32) && !defined(_VISION_WINRT)
bool CheckFullscreenResolution( int iAdapter, int desiredX, int desiredY, int* selectedX, int* selectedY )
{
  VASSERT( selectedX && selectedY );

  // by default is the same
  *selectedX = desiredX;
  *selectedY = desiredY;

  // Getting the number of display modes
  DEVMODEA deviceMode;
  deviceMode = Vision::Video.GetAdapterMode(iAdapter);
  int nModes = VVideo::GetAdapterModeCount( iAdapter, deviceMode.dmBitsPerPel );
  if ( nModes > 0 && nModes < 2048 )// sanity check
  {
    // Retrieving the display modes
    VVideoMode* arrayModes = new VVideoMode[nModes];
    if ( arrayModes != NULL ) // allocated memory check
    {
      VVideo::EnumAdapterModes( iAdapter, deviceMode.dmBitsPerPel, arrayModes );

      // Searching best fit. We compare using the number of pixels (width * height)
      unsigned int desiredNoPixels = desiredX * desiredY;
      unsigned int curNoPixels;
      int found = -1; // indicates which mode in the list will be used
      int i = 0;
      for ( i = 0; i < nModes; ++i )
      {
        curNoPixels = arrayModes[i].m_iXRes*arrayModes[i].m_iYRes;
        if ( desiredNoPixels == curNoPixels ) 
        {
          // same resolution mode exists, returns supported
          return true; 
        }
        else if ( curNoPixels > desiredNoPixels )
        {
          // we reach a mode higher than desired (but no exactly the same was found)
          // use the last one in list (__max is used for lower bound checking)
          found = __max(i-1,i);
          break;
        }    
      }
      // we reach the end of list, our mode is not supported and bigger than any other in the list
      if ( i == nModes )
        found = nModes-1; // set the last one supported

      // if we found some fit, use it
      if ( found >= 0 )
      {
        *selectedX = arrayModes[found].m_iXRes;
        *selectedY = arrayModes[found].m_iYRes;
      }
      // Releasing array
      V_SAFE_DELETE_ARRAY(arrayModes);
    }
  }
  return false;
}
#endif

#if defined(_VISION_WIIU)

class VDRCDemoRenderLoop : public IVisRenderLoop_cl
{
public:

  VDRCDemoRenderLoop()
    : m_spLogoTexture(NULL), m_LogoState(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_ALWAYSVISIBLE),
    m_vLogoPosition(0.0f, 0.0f), m_vLogoMovement(35.0f, 25.0f)
  {
    m_spLogoTexture = Vision::TextureManager.Load2DTexture("Textures\\Havok_Logo_128x64.dds");
  }

  VOVERRIDE void OnDoRenderLoop(void *pUserData);

private:

  VTextureObjectPtr m_spLogoTexture;

  VSimpleRenderState_t m_LogoState;

  hkvVec2 m_vLogoPosition;
  hkvVec2 m_vLogoMovement;
};

void VDRCDemoRenderLoop::OnDoRenderLoop(void *pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VDRCDemoRenderLoop");

  Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, V_RGBA_BLACK);

  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();

  if(m_spLogoTexture != NULL)
  {
    float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
    m_vLogoPosition += (m_vLogoMovement * fTimeDiff);

    hkvVec2 vLogoSize(m_spLogoTexture->GetTextureWidth(), m_spLogoTexture->GetTextureHeight());

    int iMaxX, iMaxY;
    VisRenderContext_cl::GetCurrentContext()->GetSize(iMaxX, iMaxY);

    if(m_vLogoPosition.x <= 0.0f || m_vLogoPosition.x + vLogoSize.x > iMaxX)
      m_vLogoMovement.x = -m_vLogoMovement.x;

    if(m_vLogoPosition.y <= 0.0f || m_vLogoPosition.y + vLogoSize.y > iMaxY)
      m_vLogoMovement.y = -m_vLogoMovement.y;

    pRI->DrawTexturedQuad(m_vLogoPosition, m_vLogoPosition + vLogoSize, m_spLogoTexture, hkvVec2(0.0f, 0.0f), hkvVec2(1.0f, 1.0f), V_RGBA_WHITE, m_LogoState);
  }

  Vision::RenderLoopHelper.EndOverlayRendering();

  // send render callback
  VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, VRH_GUI);
  Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
}

class VDRCCopyRenderLoop : public IVisRenderLoop_cl
{
public:

  VDRCCopyRenderLoop()
    : m_spTexture(NULL), m_State(VIS_TRANSP_NONE, RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_FILTERING)
  {
  }

  VOVERRIDE void OnDoRenderLoop(void *pUserData);

  inline void SetTexture(VTextureObject* pTexture)
  {
    m_spTexture = pTexture;
  }

private:

  VTextureObjectPtr m_spTexture;

  VSimpleRenderState_t m_State;
};

void VDRCCopyRenderLoop::OnDoRenderLoop(void *pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VDRCCopyRenderLoop");

  VisRenderContext_cl::GetMainRenderContext()->ResolveToTexture((VisRenderableTexture_cl*)pUserData);

  Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_All, V_RGBA_BLACK);

  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();

  if(m_spTexture != NULL)
  {
    pRI->DrawTexturedQuad(hkvVec2(0.0f, 0.0f), hkvVec2(VVideo::GetVideoConfig()->uiDRCRenderWidth, VVideo::GetVideoConfig()->uiDRCRenderHeight), m_spTexture, hkvVec2(0.0f, 0.0f), hkvVec2(1.0f, 1.0f), V_RGBA_WHITE, m_State);
  }

  Vision::RenderLoopHelper.EndOverlayRendering();
}


void VisSampleApp::InitDRCWithRenderloop(VWiiUDRC eDrc, IVisRenderLoop_cl* pRenderLoop)
{
  VASSERT(pRenderLoop != NULL);
  VASSERT_MSG(m_spDRCContext == NULL, "You can only initialize the DRC with one renderloop!");

  VisRenderableTexture_cl* pDRCColorTexture = NULL, *pDRCDepthTexture = NULL;

  if(Vision::Renderer.GetDRCRenderableTextures(eDrc, &pDRCColorTexture, &pDRCDepthTexture))
  {
    VisContextCamera_cl *pCamera = new VisContextCamera_cl();
    m_spDRCContext = new VisRenderContext_cl();
    m_spDRCContext->SetCamera(pCamera);
    m_spDRCContext->SetRenderTarget(0, pDRCColorTexture);
    m_spDRCContext->SetDepthStencilTarget(pDRCDepthTexture);
    m_spDRCContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_DISPLAY);
    m_spDRCContext->SetRenderingEnabled(false);
    m_spDRCContext->SetName("DRC");
    IVisVisibilityCollector_cl *pVisColl = new VisionVisibilityCollector_cl();
    m_spDRCContext->SetVisibilityCollector(pVisColl);
    m_spDRCContext->SetRenderLoop(pRenderLoop);
    m_spDRCContext->SetUserData(m_pDRCCopyBuffer);

    pVisColl->SetOcclusionQueryRenderContext(m_spDRCContext);
    Vision::Contexts.AddContext(m_spDRCContext);
  }
}

void VisSampleApp::CleanupDRC()
{
  if (m_spDRCContext != NULL)
  {
    m_spDRCContext->ReleaseContext();
    m_spDRCContext = NULL;
  }

  // DRCCopyBuffer gets deleted when the DRCContext is finally removed
  m_pDRCCopyBuffer = NULL;
}

void VisSampleApp::InitDRCDemo(VWiiUDRC eDrc /*= V_DRC_FIRST*/)
{
  InitDRCWithRenderloop(eDrc, new VDRCDemoRenderLoop());
}

void VisSampleApp::InitDRCCopyMode(VWiiUDRC eDrc /*= V_DRC_FIRST*/)
{
  VisRenderableTextureConfig_t LocalConfig;
  LocalConfig.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
  VisRenderContext_cl::GetMainRenderContext()->GetSize(LocalConfig.m_iWidth, LocalConfig.m_iHeight);
  LocalConfig.m_bCreateMipmaps = false;
  LocalConfig.m_bIsDepthStencilTarget = false;
  LocalConfig.m_bPreferFastMemory = false;
  LocalConfig.m_bSRGB = false;
  LocalConfig.m_iMultiSampling = VVIDEO_MULTISAMPLE_OFF;
  LocalConfig.m_bRenderTargetOnly = false;
  LocalConfig.m_bResolve = false;
  m_pDRCCopyBuffer = Vision::TextureManager.CreateRenderableTexture("<ResolvedColorBuffer>", LocalConfig);

  VDRCCopyRenderLoop* pCopyRenderLoop = new VDRCCopyRenderLoop();
  pCopyRenderLoop->SetTexture(m_pDRCCopyBuffer);

  InitDRCWithRenderloop(eDrc, pCopyRenderLoop);
}

#endif

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
