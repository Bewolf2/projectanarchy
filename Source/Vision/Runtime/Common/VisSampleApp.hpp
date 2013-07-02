/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisSampleApp.hpp

#ifndef __VISION_SAMPLEAPP_HPP
#define __VISION_SAMPLEAPP_HPP

// Include required headers
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Engine/Application/VisionApp.hpp>
#include <Vision/Runtime/Common/iOS/VisAppSettings.h>
#include <Vision/Runtime/Engine/Application/VisProfilingMenu.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VMouseCamera.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Profiling/VGraphObject.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
class VBufferResolver;

#define VISION_INPUT_CONTROL_USER_SPACE 64
#define VISION_INPUT_CONTROL_ALTERNATIVES 20

/// \brief
///   Flag values for configuring the behavior of a VisSampleApp.
/// \note
///   Not all flags are applicable to all platforms.
enum VSampleFlags
{
  VSAMPLE_FORCEFULLSCREEN           = V_BIT(0),  ///< Start in full-screen mode. Unless VSAMPLE_ASKFULLSCREEN is specified, no display mode prompt is shown. (Windows desktop only)
  VSAMPLE_ASKFULLSCREEN             = V_BIT(1),  ///< Ask about the display mode. The answer to this prompt will override VSAMPLE_FORCEFULLSCREEN. (Windows desktop only)
  VSAMPLE_MULTISAMPLE2X             = V_BIT(2),  ///< Use multisampling: 2 samples.
  VSAMPLE_MULTISAMPLE4X             = V_BIT(3),  ///< Use multisampling: 4 samples.
  VSAMPLE_WAITRETRACE               = V_BIT(4),  ///< Wait for vertical sync before swapping buffers.
  VSAMPLE_USEDESKTOPRESOLUTION      = V_BIT(5),  ///< In full-screen mode, use the current screen resolution instead of the resolution passed to VisSampleApp::InitSample().
  VSAMPLE_SPLASHSCREEN              = V_BIT(6),  ///< Show a loading screen with a progress bar, that cross-fades into the loaded scene once loading is complete.
  VSAMPLE_HAVOKLOGO                 = V_BIT(7),  ///< Show the Havok logo as an overlay in the lower right corner of the screen or window
  VSAMPLE_ALIGNLOGOALTERNATIVE      = V_BIT(8),  ///< If VSAMPLE_HAVOKLOGO is specified as well, shows the Havok logo in the lower left corner instead.
  VSAMPLE_MULTISAMPLE8X             = V_BIT(10), ///< Use multisampling: 4 samples.
  VSAMPLE_CUSTOMDATADIRECTORIES     = V_BIT(11), ///< Don't add default data dirs for Vision samples (see VisSampleApp::SetupSampleData()).
  VSAMPLE_DISABLEDEFAULTKEYS        = V_BIT(12), ///< Don't process default sample hotkeys for showing debug information and changing rendering modes.
  VSAMPLE_ENABLE_IHV_PROFILING      = V_BIT(13), ///< Enable hardware vendor-specific profiling (Windows only; this enables nVidia NVPerfHUD).
  VSAMPLE_XBOX360_BUFFER_2_FRAMES   = V_BIT(14), ///< Allow commands of two frames to queue up in the command buffer (Xbox360 only).
  VSAMPLE_NO_DEPTHBUFFER            = V_BIT(15), ///< Don't create a depth/stencil buffer.
  VSAMPLE_DX11_0_ON_DX11            = V_BIT(16), ///< Attempt to create the DirectX 11.0 feature level (DirectX 11 only).
  VSAMPLE_DX10_1_ON_DX11            = V_BIT(17), ///< Attempt to create the DirectX 10.1 feature level (DirectX 11 only).
  VSAMPLE_DX10_0_ON_DX11            = V_BIT(18), ///< Attempt to create the DirectX 10.0 feature level (DirectX 11 only).
  VSAMPLE_DX9_3_ON_DX11             = V_BIT(19), ///< Attempt to create the DirectX 9.3 feature level (DirectX 11 only).
  VSAMPLE_DX9_2_ON_DX11             = V_BIT(20), ///< Attempt to create the DirectX 9.2 feature level (DirectX 11 only).
  VSAMPLE_DX9_1_ON_DX11             = V_BIT(21), ///< Attempt to create the DirectX 9.1 feature level (DirectX 11 only).
  VSAMPLE_FORCEMOBILEMODE           = V_BIT(22), ///< Use the mobile shader provider when loading and setting up a scene.
  VSAMPLE_FLIPMODE30FPS             = V_BIT(23), ///< Perform 30 Hz frame sync (Playstation 3 only).
  VSAMPLE_FLIPMODE60FPS             = V_BIT(24), ///< Perform 60 Hz frame sync (Playstation 3 only).
  VSAMPLE_CHECKFULLSCREENMODE       = V_BIT(25), ///< Check whether the resolution passed to VisSampleApp::InitSample() is actually supported; use closest supported alternative if it isn't (Windows desktop only).
  VSAMPLE_WIIU_DRCDEMO              = V_BIT(26), ///< WiiU only: Show a demo animation on the DRC.
  VSAMPLE_WIIU_DRCCOPY              = V_BIT(27), ///< WiiU only: Show the rendered scene on the DRC also.
  VSAMPLE_HEADLESS                  = V_BIT(28), ///< Run in headless mode (see VVideo::SetHeadlessModeEnabled() for more details; Windows only).
  VSAMPLE_SHOWEXITPROMPT            = V_BIT(29), ///< Prompt before exiting the application (Android only).
  VSAMPLE_NO_FALLBACK_ASSETPROFILE  = V_BIT(30), ///< Don't allow selection of a different asset profile as a fallback in case the specified profile doesn't exist. See LoadScene() for more details.
  VSAMPLE_DONT_LOAD_MANIFESTFILE    = V_BIT(31), ///< Don't evaluate the project's manifest file for additional data directories to add.

#if defined(_VISION_WIIU) // on WIIU we currently need WAITRETRACE
  VSAMPLE_INIT_DEFAULTS = VSAMPLE_ASKFULLSCREEN|VSAMPLE_SPLASHSCREEN|VSAMPLE_USEDESKTOPRESOLUTION|VSAMPLE_HAVOKLOGO|VSAMPLE_WIIU_DRCDEMO|VSAMPLE_WAITRETRACE,
#elif defined(HK_ANARCHY) && !defined( _VISION_MOBILE )
  VSAMPLE_INIT_DEFAULTS = VSAMPLE_ASKFULLSCREEN|VSAMPLE_SPLASHSCREEN|VSAMPLE_USEDESKTOPRESOLUTION|VSAMPLE_SHOWEXITPROMPT,
#else
  VSAMPLE_INIT_DEFAULTS = VSAMPLE_ASKFULLSCREEN|VSAMPLE_SPLASHSCREEN|VSAMPLE_USEDESKTOPRESOLUTION|VSAMPLE_HAVOKLOGO|VSAMPLE_SHOWEXITPROMPT,
#endif

  VSAMPLE_DX11_FEATURE_LEVEL_MASK   = VSAMPLE_DX11_0_ON_DX11 | VSAMPLE_DX10_1_ON_DX11 | VSAMPLE_DX10_0_ON_DX11 | VSAMPLE_DX9_3_ON_DX11 | VSAMPLE_DX9_2_ON_DX11 | VSAMPLE_DX9_1_ON_DX11,
  VSAMPLE_DX11_FEATURE_LEVEL_COUNT  = 6,
};

//defines platform specific macros to make the sample source more readable
//  - VISION_MAIN: the main function signature
//  - VISION_BASE_DATA: the root path to the Base Data directory
//  - VISION_COMMON_DATA: the root path to the Sample Data directory
//  - VISION_ROOT_DATA: the root path (on PC the current directory, on console the root path)
//  - VISION_MANDATORY_SAMPLE_FLAGS: Sample flags that are mandatory (e.g. fullscreen for consoles)
//  - VISION_DEFAULT_WORKERTHREADCOUNT: Default worker thread count for the platform
//  - VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT: If true, the worker threads will be assigned to specific processors
//  - VISION_OUTPUT_DIR: Default output directory for the platform. If not set, the application directory will be used
#if defined(_VISION_WINRT)

  #ifndef _VISION_WINRT_HYBRID
    #define VISION_MAIN extern int defaultVisionWinRtMain(Platform::Array<Platform::String^>^ args); \
      [Platform::MTAThread] \
      int main(Platform::Array<Platform::String^>^ args) \
      {	return defaultVisionWinRtMain( args ); } \
      extern "C" int VisionMainLine(int argc, wchar_t* argv[]) 
  #else
    #define VISION_MAIN extern "C" int VisionMainLine(int argc, wchar_t* argv[]) 
  #endif

  #define VISION_ROOT_DATA ""
  #define VISION_BASE_DATA "Data\\Vision\\Base"
  #define VISION_COMMON_DATA "Data\\Vision\\Samples\\Engine"
  #define VISION_MANDATORY_SAMPLE_FLAGS 0
  #define VISION_DEFAULT_WORKERTHREADCOUNT 3
  
  #define VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT false  //ignored 

#elif defined(WIN32)

  #define VISION_MAIN int APIENTRY WinMain( HINSTANCE hInstance, \
                                            HINSTANCE hPrevInstance, \
                                            LPSTR     lpCmdLine, \
                                            int       nCmdShow )

  #define VISION_ROOT_DATA ".\\"
  #define VISION_BASE_DATA "..\\..\\..\\..\\Data\\Vision\\Base"
  #define VISION_COMMON_DATA "..\\..\\..\\..\\Data\\Vision\\Samples\\Engine"
  #define VISION_MANDATORY_SAMPLE_FLAGS 0
  #define VISION_DEFAULT_WORKERTHREADCOUNT 3
  
  #define VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT false

  // enable theming on sample application
  #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#elif defined(_VISION_XENON)
  #define VISION_MAIN int main(int argc, char* argv[])
  #define VISION_ROOT_DATA "D:\\"
  #define VISION_BASE_DATA "D:\\Data\\Vision\\Base"
  #define VISION_COMMON_DATA "D:\\Data\\Vision\\Samples\\Engine"
  #define VISION_MANDATORY_SAMPLE_FLAGS VSAMPLE_FORCEFULLSCREEN
  #define VISION_DEFAULT_WORKERTHREADCOUNT 4
  #define VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT true
  #define VISION_OUTPUT_DIR "D:\\"

#elif defined(_VISION_PS3)
  #define VISION_MAIN SYS_PROCESS_PARAM (1001, 512*1024) \
                    int main(int argc, char* argv[])

  #if defined(BD_EMULATION)
    #define VISION_ROOT_DATA "/dev_bdvd/PS3_GAME/USRDIR/"
    #define VISION_BASE_DATA "/dev_bdvd/PS3_GAME/USRDIR/base"
    #define VISION_COMMON_DATA "/dev_bdvd/PS3_GAME/USRDIR"
  #else
    #define VISION_ROOT_DATA "/app_home/"
    #define VISION_BASE_DATA "/app_home/Data/Vision/Base"
    #define VISION_COMMON_DATA "/app_home/Data/Vision/Samples/Engine"
  #endif

  #define VISION_MANDATORY_SAMPLE_FLAGS VSAMPLE_FORCEFULLSCREEN
  #define VISION_DEFAULT_WORKERTHREADCOUNT 1
  #define VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT false
  #define VISION_OUTPUT_DIR "/app_home/Bin"

#elif defined(_VISION_IOS)
  #define VISION_MAIN extern "C" int VisionMainFunction(const char* pDocumentsDirectory)
  #define VISION_ROOT_DATA VisSampleApp::GetRootDirectory()
  #define VISION_BASE_DATA VisSampleApp::GetBaseDataDirectory()
  #define VISION_COMMON_DATA VisSampleApp::GetCommonDataDirectory()
  #define VISION_OUTPUT_DIR VisSampleApp::GetDocumentsDirectory()
  #define VISION_MANDATORY_SAMPLE_FLAGS VSAMPLE_FORCEFULLSCREEN
  #define VISION_DEFAULT_WORKERTHREADCOUNT 0
  #define VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT false

#elif defined(_VISION_PSP2)
  #ifndef VISION_PSP2_MAIN_STACK_SIZE
    #define VISION_PSP2_MAIN_STACK_SIZE SCE_KERNEL_STACK_SIZE_DEFAULT_USER_MAIN
  #endif
  #ifndef VISION_PSP2_MAIN_THREAD_NAME
    #define VISION_PSP2_MAIN_THREAD_NAME "VisionMainThread"
  #endif
  #ifndef VISION_PSP2_LIBC_HEAP_SIZE
    #pragma warning("VISION_PSP2_LIBC_HEAP_SIZE not defined, using default (230mb)")

    #ifdef HK_DEBUG
      #define VISION_PSP2_LIBC_HEAP_SIZE (230 * 1024 * 1024)
    #else
      #define VISION_PSP2_LIBC_HEAP_SIZE (190 * 1024 * 1024)
    #endif
    
  #endif

// NOTE: When changing the main thread priority and/or the CPU affinity mask, 
//       certain limitations apply. Please refer to the SDK documentation,
//       section 5.6 "Thread Scheduling" in the System chapter, for further details.
  #define VISION_MAIN                                                                \
    int       sceUserMainThreadPriority	       = SCE_KERNEL_DEFAULT_PRIORITY_USER;   \
    int       sceUserMainThreadCpuAffinityMask = SCE_KERNEL_THREAD_CPU_AFFINITY_MASK_DEFAULT; \
    unsigned int sceUserMainStackSize          = VISION_PSP2_MAIN_STACK_SIZE;        \
    unsigned int sceLibcHeapSize               = VISION_PSP2_LIBC_HEAP_SIZE;         \
    char      sceUserMainThreadName[]          = VISION_PSP2_MAIN_THREAD_NAME;       \
    int main(int argc, char* argv[])

  #define VISION_ROOT_DATA "app0:"
  #define VISION_BASE_DATA "app0:Data/Vision/Base"
  #define VISION_COMMON_DATA "app0:Data/Vision/Samples/Engine"
  #define VISION_OUTPUT_DIR "app0:Bin"

  #define VISION_MANDATORY_SAMPLE_FLAGS VSAMPLE_FORCEFULLSCREEN
  #define VISION_DEFAULT_WORKERTHREADCOUNT 2
  #define VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT true

#elif defined(_VISION_ANDROID)

  #define VISION_MAIN extern "C" void android_main(struct android_app* state)
  #define VISION_ROOT_DATA ""  
  #define VISION_BASE_DATA VisSampleApp::GetBaseDataDirectory()
  #define VISION_COMMON_DATA VisSampleApp::GetCommonDataDirectory()
  #define VISION_OUTPUT_DIR VisSampleApp::GetCacheDirectory()

  #define VISION_MANDATORY_SAMPLE_FLAGS VSAMPLE_FORCEFULLSCREEN
  #define VISION_DEFAULT_WORKERTHREADCOUNT 0
  #define VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT false

#elif defined(_VISION_WIIU)

  #define VISION_MAIN int main(int argc, char* argv[])

  #define VISION_ROOT_DATA "/vol/content/VisionRoot"
  #define VISION_BASE_DATA "/vol/content/VisionRoot/Data/Vision/Base"
  #define VISION_COMMON_DATA "/vol/content/VisionRoot/Data/Vision/Samples/Engine"
  #define VISION_OUTPUT_DIR "/vol/save"
  
  #define VISION_MANDATORY_SAMPLE_FLAGS VSAMPLE_FORCEFULLSCREEN
  #define VISION_DEFAULT_WORKERTHREADCOUNT 2
  #define VISION_FIXED_WORKERTHREAD_CPU_ASSIGNMENT true
#else
  #error "Missing platform!"
#endif
// VISION_MAIN

// entry points for a callback based app
#ifdef _VISION_IOS
#define VISION_CALLBACK extern "C"
#else
#define VISION_CALLBACK 
#endif

VISION_CALLBACK bool VisionInitFunction();
VISION_CALLBACK bool VisionDeInitFunction();
VISION_CALLBACK bool VisionRunFunction();

#define VISION_INIT   VISION_CALLBACK bool VisionInitFunction()
#define VISION_DEINIT VISION_CALLBACK bool VisionDeInitFunction()
#define VISION_RUN    VISION_CALLBACK bool VisionRunFunction()

#if (defined(_VISION_WINRT) || defined(_VISION_IOS) || defined(_VISION_ANDROID)) && !defined(NO_VISION_SAMPLEAPP_CALLBACKS) && !defined(VISION_SAMPLEAPP_CALLBACKS)
  #define VISION_SAMPLEAPP_CALLBACKS
#endif

#ifdef VISION_SAMPLEAPP_CALLBACKS
#define VISION_SAMPLEAPP_AFTER_LOADING VISION_CALLBACK void VisionSampleAppAfterLoadingFunction()
#define VISION_SAMPLEAPP_RUN           VISION_CALLBACK bool VisionSampleAppRunFunction()
#endif

#ifdef HK_DEBUG_SLOW
#define VISION_INTER_LINKING_CHECK VisionInterDebugReleaseLinkingCheck(); vBaseInterDebugReleaseLinkingCheck();
#else
#define VISION_INTER_LINKING_CHECK VisionInterReleaseDebugLinkingCheck(); vBaseInterReleaseDebugLinkingCheck();
#endif

#if defined(_VISION_ANDROID)
#define VISION_MAIN_DEFAULT VISION_MAIN { \
  printf("android_main\n"); \
  app_dummy(); \
  AndroidApplication = state; \
  char apkDir[FS_MAX_PATH]; \
  char sdDir[FS_MAX_PATH]; \
  char cacheDir[FS_MAX_PATH]; \
  InitAndroidNativeEnvironment(apkDir, sdDir, cacheDir); \
  VisSampleApp::SetAndroidDirectories(apkDir, sdDir, cacheDir); \
  if (!VisionInitFunction()) { VisionDeInitFunction(); DeinitAndroidNativeEnvironment(); return; } \
  while (VisionRunFunction()) { if(!PollAndroidNativeEnvironment()) break; } \
  VisionDeInitFunction(); \
  DeinitAndroidNativeEnvironment(); }
#elif defined( _VISION_IOS )
#define VISION_MAIN_DEFAULT
#else
#define VISION_MAIN_DEFAULT VISION_MAIN { \
  if (!VisionInitFunction()) { VisionDeInitFunction(); return -1; } \
  while (VisionRunFunction()) { } \
  VisionDeInitFunction(); \
  return 0; }
#endif

#if defined(_VISION_IOS) || defined(_VISION_METRO) || defined(_VISION_APOLLO)
  #define VISION_SET_DIRECTORIES(bLoadFromDocuments)  \
    VisSampleApp::SetupDirectories(bLoadFromDocuments);
#else
  #define VISION_SET_DIRECTORIES(bLoadFromDocuments)
#endif


//Vision related input triggers
enum VISION_INPUT_CONTROL 
{
  EXIT = 0,
  EXIT_COMBO,

  SHOW_HELP,
  SHOW_FRAME_RATE,

#if defined(_VISION_PSP2)
  PSP2_OPTION,            // multi-key combos
#endif

  WIREFRAME_MODE,
  SHOW_ENTITY_DEBUG_OUTPUT,
  RELOAD_MODIFIED_RESOURCES,
  SHOW_POLYGON_COUNT,
  SAVE_SCREEN_SHOT,
  SHOW_PREV_PROFILING_CHART,
  SHOW_NEXT_PROFILING_CHART,
  RESET_MAX_PROFILING_VALUES,
  SHOW_TIME_STEPS,
  
#if defined(_VISION_MOBILE)
  SHOW_PROFILING_MENU1,
  SHOW_PROFILING_MENU2,
#endif

  ATTACH_TO_NEXT_CAMERA_POSITION,

  FOV_DECREASE,
  FOV_INCREASE,
  
  VISION_INPUT_CONTROL_LAST_ELEMENT = FOV_INCREASE
};

class IVisApiBackgroundResourceRestorer;

/// \brief
///   This class is provided to help you use Vision features, and to give a unified framework for
///   samples.
/// 
/// It extends the VisionApp_cl class by adding hotkeys for debugging (e.g. F3 for wireframe mode
/// etc.)
/// 
/// \example
///   \code
///   See Tutorial 01 for an explanation of the simplest possible sample:
///   {
///     VisSampleAppPtr spApp = new VisSampleApp();
///     if (!spApp->InitSample("SampleDataDir"))
///       return -1;
///     {
///       VSceneLoader loader;
///       if (!loader.LoadScene("ViewerMap.vscene"))
///         Vision::Error.FatalError(loader.GetLastError());
///     }
///     spApp->EnableMouseCamera(false);
///     while (spApp->Run()) {}
///     spApp->DeInitSample();
///     return 0;
///   }
///   \endcode
class VisSampleApp : public VisionApp_cl, public IVisCallbackHandler_cl
{
public:

  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor. Initializes the basic variables for a sample application, but does not
  ///   initialize the engine.
  /// 
  /// \sa VisSampleApp::~VisSampleApp();
  /// \sa VisSampleApp::InitSample
  VisSampleApp();
  
  /// \brief
  ///   Destructor of the VisSampleApp class
  /// 
  /// Cleans up the variables of the sample application.
  /// 
  /// \sa VisSampleApp::VisSampleApp();  
  ~VisSampleApp();


  ///
  /// @}
  ///

  ///
  /// @name Init / Deinit
  /// @{
  ///
  
  /// \brief
  ///   Initializes the sample application (and the engine)
  /// 
  /// You can change the engine settings by changing 'm_config' before calling this
  /// 
  /// Also see the VisionApp_cl::InitEngine documentation.
  /// 
  /// \param pszSampleDataDir
  ///   Directory where sample data will be found. May be \c NULL. (Passed to SetupSampleData() and
  ///   assumed to be in \c Data/Vision/Engine/Samples/)
  /// 
  /// \param pszSampleScene
  ///   Scene file to load at sample startup. May be \c NULL. (\c .vscene will be added)
  /// 
  /// \param iSampleFlags
  ///   Initialization flags for the sample. See the VSampleFlags enum for available flags.
  ///   The default (\c VSAMPLE_INIT_DEFAULTS) provides reasonable defaults. On Windows desktop,
  ///   it always asks about switching to full-screen mode.
  /// 
  /// \param iX
  ///   X resolution for the frame buffer, e.g. 1280
  /// 
  /// \param iY
  ///   Y resolution for the frame buffer, e.g. 720
  /// 
  /// \param iInitFlags
  ///   Engine initialization flags (see VisionApp_cl::InitEngine)
  /// 
  /// \param iNumThreads
  ///   Number of threads to be used. A value of 0 means that a suitable number of threads will be
  ///   used, typically (but not necessarily) equal to the number of hardware threads in the
  ///   system.
  /// 
  /// \sa VisionApp_cl::InitEngine
  /// \sa VisSampleApp::SetupSampleData
  bool InitSample( const char *pszSampleDataDir = NULL, const char *pszSampleScene = NULL, int iSampleFlags = VSAMPLE_INIT_DEFAULTS, int iX=VVIDEO_DEFAULTWIDTH, int iY=VVIDEO_DEFAULTHEIGHT , int iInitFlags = VAPP_INIT_DEFAULTS, int iNumThreads = 0);

#if defined(WIN32) && !defined(_VISION_WINRT)
  bool InitSampleForBrowser( HWND hWnd, WNDPROC wndProc, VVIDEO_Multisample msaa = VVIDEO_MULTISAMPLE_OFF);
#endif
  
#if defined(_VISION_IOS)
  static void SetupDirectories(bool bLoadFromDocuments = false);
  inline static const char* GetRootDirectory() { return m_szRootDirectory; }
  inline static const char* GetBaseDataDirectory() { return m_szBaseDataDirectory; }
  inline static const char* GetCommonDataDirectory() { return m_szCommonDataDirectory; }
  inline static const char* GetCacheDirectory() { return m_szCachesDirectory; }
  inline static const char* GetDocumentsDirectory() { return m_szDocumentsDirectory; }
#endif

#if defined(_VISION_ANDROID)
  inline static const char* GetBaseDataDirectory() { return m_szBaseDataDirectory; }
  inline static const char* GetCommonDataDirectory() { return m_szCommonDataDirectory; }
  inline static const char* GetSDCardDirectory() { return m_szSDCardDirectory; }
  inline static const char* GetCacheDirectory() { return m_szCacheDirectory; }
  inline static const char* GetApkDirectory() { return m_szAPKDirectory; }

  static void SetAndroidDirectories(const char* szAPKDirectory, const char* szSDCardDirectory, const char* szCacheDirectory);

  static bool ExtractFileFromAPKToCache(const char* pszFileName);
#endif

#if defined(_VISION_WINRT)
  static void SetupDirectories(bool bLoadFromDocuments = false);
  inline static const char* GetSDCardDirectory() { return m_szSDCardDirectory; }
  inline static const char* GetCacheDirectory() { return m_szCacheDirectory; }
#endif

#if defined(_VISION_WIIU)
  inline VisRenderContext_cl *GetDRCRenderContext() const { return m_spDRCContext; }
#endif

  /// \brief
  ///   Deinitializes the sample application (and the engine)
  void DeInitSample();

  /// \brief
  ///   Static helper function to load the VisionEnginePlugin
  /// 
  /// This function should be called before the application/engine is initialized.
  static void LoadVisionEnginePlugin();

  ///
  /// @}
  ///

  ///
  /// @name Run
  /// @{
  ///

  /// \brief
  ///   Main loop of the sample application
  /// 
  /// It overrides the VisionApp_cl::Run implementation and adds support for some keyboard hotkeys.
  /// 
  /// This function must be called once per frame for the application to work. It will return false
  /// if the application wants to quit.  This functions handles input, text display and buffer
  /// swapping. 
  /// 
  /// \return
  ///   \c true if the application should continue running; \c false if the application should exit.
  ///  Run will also abort if the sample wasn't correctly initialized.
  /// 
  /// \sa VisionApp_cl::Run
  /// \sa VisionApp_cl::Quit
  /// \sa VisionApp_cl::WantsToQuit
  /// 
  /// \example
  ///   \code
  ///   while ( spApp->Run () ) { }
  ///   \endcode
  virtual bool Run();


  ///
  /// @}
  ///

  ///
  /// @name Loading Screen
  /// @{
  ///

  /// \brief
  ///   This structure holds the loading screen settings
  struct LoadingScreenSettings
  {
    /// \brief
    ///   Specify if and how aspect ratio of the background texture is maintained.
    enum AspectRatioAlignment
    {
      ALIGN_NONE,         ///< Fullscreen display of the background texture.
      ALIGN_HORIZONTAL,   ///< Keep the texture's aspect ratio and align it horizontally (i.e., pad or cut off the top and bottom part of the texture, if necessary).
      ALIGN_VERTICAL,     ///< Keep the texture's aspect ratio and align it vertically (i.e., pad of cut off the left and right borders of the texture, if necessary).
      ALIGN_FULL,         ///< Keep the texture's aspect ratio and fit the whole texture on screen.
    };

    /// \brief
    ///   Constructor. Initializes the structure with the default values.
    LoadingScreenSettings()
      : sBackgroundImagePath()
      , backgroundColor( 0, 0, 0 )
      , eAspectRatioAlignment( ALIGN_NONE )
      , iProgressBarLeft(-1), iProgressBarRight(-1)
      , iProgressBarTop(-1), iProgressBarBottom(-1)
      , fFadeOutTime(1.f)
      , bForceFullScreenTexture( false )
    {}

    VString sBackgroundImagePath;                 ///< path to the background image to be displayed on the loading screen
    VColorRef backgroundColor;                    ///< color to be displayed 
    AspectRatioAlignment eAspectRatioAlignment;   ///< alignment specification for the background texture
    int iProgressBarLeft, iProgressBarRight;      ///< horizontal placement of the progress bar
    int iProgressBarTop, iProgressBarBottom;      ///< vertical placement of the progress bar
    float fFadeOutTime;                           ///< time for the loading screen to fade out after loading is complete
    bool bForceFullScreenTexture;                 ///< always draw a fullscreen quad for the texture, but modify texture coordinates to preserve the alignment settings as given above
  };

  /// \brief
  ///   Enables or disables display of a progress bar during world loading
  /// 
  /// Call this function before VSceneLoader::LoadScene to setup the display of a loading screen
  /// and progress bar.
  /// 
  /// \param bStatus
  ///   true to enable it, false to disable
  /// 
  /// \param szTextureFile
  ///   Name of texture file to use as background (optional)
  /// 
  /// \param fFadeOutTime
  ///   Time that loading screen takes to fade out after loading
  /// 
  /// \param iProgressBarLeft
  ///   x-coordinate of the upper left corner of the progress bar in pixel coordinates 
  ///   of the background texture. Set to -1 to use default progress bar extends.
  ///
  /// \param iProgressBarTop
  ///   y-coordinate of the upper left corner of the progress bar in pixel coordinates 
  ///   of the background texture.
  ///
  /// \param iProgressBarRight
  ///   x-coordinate of the lower right corner of the progress bar in pixel coordinates 
  ///   of the background texture.
  ///
  /// \param iProgressBarBottom
  ///   y-coordinate of the lower right corner of the progress bar in pixel coordinates 
  ///   of the background texture.
  /// 
  /// \note
  ///   Call this function before calling VSceneLoader::LoadScene
  ///
  /// \note
  ///   Using this function will set the loading screen's alignment parameter to 
  ///   LoadingScreenSettings::ALIGN_HORIZONTAL.
  /// 
  /// \sa VisionApp_cl::OnLoadSceneStatus  
  ///
  /// \sa VisSampleApp::LoadingScreenSettings
  /// 
  /// \example
  ///   \code
  ///   VisSampleApp *pApplication = new VisSampleApp ( );  
  ///   pApplication->SetupLoadingScreen ( true , "Textures\\splash.dds" ); 
  ///   VSceneLoader loader;
  ///   loader.LoadScene("ViewerMap.vscene");
  ///   \endcode
  void SetupLoadingScreen(bool bStatus, const char *szTextureFile=NULL, float fFadeOutTime=2.f,
    int iProgressBarLeft=-1, int iProgressBarTop=-1, int iProgressBarRight=-1, int iProgressBarBottom=-1);

  /// \brief
  ///   Enables or disables display of a progress bar during world loading
  /// 
  /// Call this function before VSceneLoader::LoadScene to setup the display of a loading screen
  /// and progress bar.
  /// 
  /// \param bStatus
  ///   true to enable it, false to disable
  /// 
  /// \param settings
  ///   Additional loading screen settings.
  /// 
  /// \note
  ///   Call this function before calling VSceneLoader::LoadScene
  /// 
  /// \sa VisionApp_cl::OnLoadSceneStatus  
  ///
  /// \sa VisSampleApp::LoadingScreenSettings
  /// 
  /// \example
  ///   \code
  ///   VisSampleApp *pApplication = new VisSampleApp ( );
  ///   VisSampleApp::LoadingScreenSettings settings;
  ///   settings.sBackgroundImagePath = "Textures\\splash.dds";
  ///   settings.eAspectRatioAlignment = LoadingScreenSettings::ALIGN_VERTICAL;
  ///   pApplication->SetupLoadingScreen ( true , settings ); 
  ///   VSceneLoader loader;
  ///   loader.LoadScene("ViewerMap.vscene");
  ///   \endcode
  void SetupLoadingScreen(bool bStatus, const LoadingScreenSettings& settings );


  ///
  /// @}
  ///

  ///
  /// @name File Serve Client
  /// @{
  ///

#if defined( SUPPORTS_FILESERVE_CLIENT ) || defined( _VISION_DOC )
  /// \brief
  ///   Static helper method for setting up a VFileServeClient as global file manager.
  ///
  /// This method automatically sets up host information (taken from vFileServeHost.txt)
  /// and a data cache directory. Furthermore, it allows you to specify what should happen in case
  /// no file server connection could be established: In this case, Vision can either use a fallback
  /// to the cache directory (which will naturally only work if the data has been copied to the cache
  /// in a previous session with file serving enabled and connected), or it can load the data from
  /// the installed AppData (which is an APK file on Android and a the application data folder on iOS).
  ///
  /// \param bUseAppDataFallback
  ///   Defines what should happen if no connection to a file server could be made. If set to false,
  ///   the cache directory will be used. If set to true, Vision will attempt to load from the AppData
  ///   instead.
  ///
  /// \return 
  ///   True if the connection to a file serve host was successful. False if no connection could be made.
  ///   If false is returned, the subsequent behavior depends on the value passed into bUseApkFallback.
  static bool SetupFileServeClient(bool bUseAppDataFallback = false);
#endif

  ///
  /// @}
  ///

  ///
  /// @name Data Directories
  /// @{
  ///

  /// \brief
  ///   Helper function to setup the base data directory.
  void SetupBaseDataDir();

  /// \brief
  ///   Called from VisSampleApp::InitSample with the sample name to setup the data directories
  ///   used by the sample to load data from
  /// 
  /// The samples use up to 3 data directories relative to the startup directory (e.g. shared/bin).
  /// \li Engine base data : Data needed by the engine   (Data/Vision/Base) 
  /// \li Sample base data : Data shared by all samples  (Data/Vision/Samples/Engine)
  /// \li Sample data      : Data needed by the sample   (Data/Vision/Samples/Engine/SampleName)
  /// 
  /// This function removes all existing data directories before adding the new ones.
  /// 
  /// \param pszDataDir
  ///   Name of the directory to be used as a data directory. If a .V (encrypted ZIP) file exists
  ///   with the specified name it will be used instead of the directory. This allows packaging of
  ///   sample data for easy distribution.
  /// 
  /// \sa VisSampleApp::InitSample
  bool SetupSampleData(const char *pszDataDir);

  /// \brief
  ///   Adds the given sample data directory.
  /// 
  /// Searches for the given directory in the following order:
  /// \li VISION_ROOT_DATA/(directory).v
  /// \li VISION_ROOT_DATA/(directory)
  /// \li VISION_COMMON_DATA/(directory).v
  /// \li VISION_COMMON_DATA/(directory)
  /// 
  /// Fails if non of the paths exists.
  /// 
  /// This should be transparent for any platform.
  /// 
  /// \param pszDataDir
  ///   Name of the directory to be used as the data directory.
  bool AddSampleDataDir(const char *pszDataDir);
  
  /// \brief
  ///   Adds the given folder or package as a data directory. If it is a package, all dependent packages are automatically added, as well.
  ///
  /// This function calls AddSampleDataDir() internally and thus has the same search criteria.
  /// However, if the given string identifies a package that was generated  with the vPackageTool2,
  /// the information about other required packages is read from that package, and those packages are added as data directories, too.
  /// This is done recursively until all required packages are mounted.
  ///
  /// \param szPackageName
  ///   The path to the package or folder.
  bool AddSampleDataDirPackage(const char* szPackageName);
  
  /// \brief
  ///   Remove the given sample data directory.
  /// 
  /// Searches for the given directory in the following order:
  /// \li VISION_ROOT_DATA/(directory).v
  /// \li VISION_ROOT_DATA/(directory)
  /// \li VISION_COMMON_DATA/(directory).v
  /// \li VISION_COMMON_DATA/(directory)
  /// 
  /// Fails if non of the paths exists.
  /// 
  /// This should be transparent for any platform.
  /// 
  /// \param pszDataDir
  ///   Name of the directory to be removed.
  bool RemoveSampleDataDir(const char *pszDataDir);

  /// \brief
  ///   Sets the default asset profile for this platform. Also falls back to a compatible profile (usually 'pcdx9') if there is no profile specified for this platform.
  static void SelectAssetProfile(const char* szScene, bool bAllowFallbackProfile);

  /// \brief
  ///   Adds data directories and loads plugins defined in vForgeManafest.txt.
  ///
  /// The file \c vForgeManifest.txt is searched in the currently set data directories.
  /// If it is found, the data directories defined in this file are added, and the engine
  /// plugins referenced there are loaded.
  ///
  /// \param bAddDataDirs
  ///   Whether to add any data directories specified in the manifest file
  /// \param bLoadEnginePlugins
  ///   Whether to load any engine plugins referenced in the manifest file
  ///
  /// \note
  ///   Some engine plugins must be loaded before the engine is initialized. If you need
  ///   this functionality, call ProcessManifestFile(false, true) at the appropriate place.
  static void ProcessManifestFile(bool bAddDataDirs, bool bLoadEnginePlugins);

  /// \brief
  ///   Application State
  enum ApplicationState
  {
    AS_UNKNOWN,       ///< Loading has not started
    AS_LOADING,       ///< Loading is not yet finished
    AS_LOADING_ERROR, ///< Error during loading
    AS_SUSPENDED,     ///< Application is currently in background  
    AS_AFTER_LOADING, ///< First frame after loading has finished
    AS_RUNNING        ///< Loading is completed
  };


  /// \brief
  ///   Loads the given scene and return true of the scene was loaded successfully.
  /// 
  /// Adds ".vscene" to the scene name.
  /// 
  /// Failure to load the scene leads to a fatal error.
  /// 
  /// \param pszSampleScene
  ///   The name of the sample scene (.vscene will be added).
  ///
  /// \param iAdditionalLoadingFlags
  ///   Optionally specify additional VSceneLoader flags.
  ///
  /// \param bAllowProfileFallback
  ///   If true LoadScene will try to guess the asset profile to use. By default it will first use the platform specific profile, but it might fall back to 'pcdx9'
  ///   if there is no platform specific profile, but a pcdx9 profile, and the current platform is compatible with that (regarding texture formats).
  bool LoadScene(const char *pszSampleScene, int iAdditionalLoadingFlags=0, bool bAllowProfileFallback=true);

  /// \brief
  ///   Unloads the current scene. Call only when finished rendering( e.g. at shutdown)
  void UnloadScene();

  /// \brief
  ///   Clear the current scene and prepares for loading another scene.
  void ClearScene();

  /// \brief
  ///   Get the current application state. Do NOT call this method more than once per frame, 
  ///   otherwise the AFTER_LOADING state could be missed.
  ApplicationState GetApplicationState();


  ///
  /// @}
  ///

  ///
  /// @name Mouse Camera
  /// @{
  ///

  /// \brief
  ///   Creates a simple mouse camera entity which optionally uses the position and orientation
  ///   from a given entity, e.g. a camera position shape. Additionally adjusts camera move/strafe
  ///   speed.
  /// 
  /// Convenience function to create a VisMouseCamera_cl entity. This allows you to have an
  /// interactive mouse controlled camera for navigating.
  /// 
  /// A new camera entity has to be created after each new loaded scene since LoadWorld purges all
  /// entities.
  /// 
  /// \param szCameraRefObjectKey
  ///   Entity object key of which the initial camera position and rotation should be taken from.
  ///
  /// \param fMoveSpeed
  ///   Camera move/strafe speed. A value of -1 uses default values based on global unit scaled.
  ///
  /// \return
  ///   VisBaseEntity_cl *pCameraEntity: A reference to the created camera entity. The returned
  ///   instance can be casted to VisMouseCamera_cl* to call specific functions of that class.
  /// 
  /// \sa VisBaseEntity_cl * VisSampleApp::GetMouseCamera ();
  /// 
  /// \example
  ///   \code
  ///   A very basic world viewer:
  ///     VSceneLoader loader;
  ///     loader.LoadScene("ViewerMap.vscene");
  ///     pApplication->EnableMouseCamera("CameraPosition", 500.0f);
  ///     while ( pApplication->Run () ) { }
  ///   \endcode
  VisBaseEntity_cl* EnableMouseCamera (const char* szCameraRefObjectKey = NULL, float fMoveSpeed = -1.0f);

  /// \brief
  ///   Disables the mouse camera.
  void DisableMouseCamera();

  /// \brief
  ///   Returns a pointer to the simple mouse camera entity created via EnableMouseCamera
  /// 
  /// \return
  ///   VisBaseEntity_cl *pCameraEntity: A reference to the created camera entity. The returned
  ///   instance can be casted to VisMouseCamera_cl* to call specific functions of that class.
  /// 
  /// \note
  ///   If there was no camera created with EnableMouseCamera, the return value will be NULL
  /// 
  /// \sa VisBaseEntity_cl * VisSampleApp::EnableMouseCamera();
  VisBaseEntity_cl* GetMouseCamera () const;

  ///
  /// @}
  ///

  ///
  /// @name Display Functions
  /// @{
  ///
  
  /// \brief
  ///   Adds a single line of help text to the help display of the application.
  /// 
  /// The application class displays help text when the user presses F1.
  /// 
  /// This defaults to an explanation of key mappings. With this function you can add single help
  /// items explaining special behavior or key mappings of your application.
  /// 
  /// Lines are displayed in the order they were added.
  /// 
  /// \param pszLine
  ///   The string to be displayed.
  /// 
  /// \sa bool VisSampleApp::LoadHelpFile
  /// 
  /// \example
  ///   \code
  ///   pApplication->AddHelpText ("'X' - Render AI path nodes");
  ///   \endcode
  void AddHelpText ( const char *pszLine );


  /// \brief
  ///   Loads a text file that will be displayed when the user presses F1.
  /// 
  /// Instead of adding single lines (see AddHelpText function) you can load a full detailed help
  /// text from a txt file to be displayed when the user presses F1.
  /// 
  /// \param pszHelpfile
  ///   the filename. file must be in path or working directory.
  /// 
  /// \param bAddDefaultKeys
  ///   if true, the function will append the default key binding info to the help text
  /// 
  /// \return
  ///   bool: false on file open error.
  /// 
  /// \note
  ///   There is not much error checking being done by this function. If your text file is formated
  ///   badly, the behavior is undefined. 
  /// 
  /// \sa void VisSampleApp::AddHelpText ( const char *pszLine );
  /// 
  /// \example
  ///   \code
  ///   pApplication->LoadHelpFile ( "mykeybindings.txt", false );
  ///   \endcode
  bool LoadHelpFile ( const char *pszHelpfile, bool bAddDefaultKeys = true );


  /// \brief
  ///   Replaces a single line in the F1 help text.
  /// 
  /// \param iIndex
  ///   The iIndex of the line to change. Must be in valid range [0..MAX_HELP_LINES-1].
  /// 
  /// \param pszLine
  ///   Replacement Text.
  /// 
  /// \sa void VisSampleApp::AddHelpText ( const char *pszLine );
  /// 
  /// \example
  ///   \code
  ///   pApplication->SetHelpText ( 23 , "this is line 23" );  
  ///   \endcode
  void SetHelpText ( int iIndex, const char *pszLine );

  /// \brief
  ///   Sets the upper corner position for displaying the help text. Default is (9, 16)
  inline void SetHelpTextPosition(int x, int y)
  {
    m_iHelpTextOfs[0] = x;
    m_iHelpTextOfs[1] = y;
  }

  /// \brief
  ///   Clear all lines of the help text that have been added so far.
  void FreeHelpText();

  /// \brief
  ///   manually sets the visible state of the help text
  inline void SetShowHelpText(bool bStatus)
  {
    m_bShowHelp = bStatus;
  }

  /// \brief
  ///   Gets the current FPS (frames per second) count
  /// 
  /// This float value is the average FPS value for the last second.
  /// 
  /// It is updated once per second.
  /// 
  /// \return
  ///   float fFPS : Current FPS
  inline float GetCurrentFPS() const 
  {
    return m_fCurrentFPS;
  }

  /// \brief
  ///   Gets the current frame time 
  ///
  /// This float value is the average frame time for the last second
  ///
  /// It is updated once per second
  ///
  /// \return
  ///   Current frame time
  inline float GetCurrentFrameTime() const
  {
    return m_fCurrentFrameTime;
  }

  /// \brief
  ///   Enables or disables the display of the FPS counter
  /// 
  /// \param bShow
  ///   The new status
  inline void SetShowFrameRate(bool bShow)
  { 
    m_bShowFrameRate = bShow;
  }



  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Gets a pointer to the static input map of the sample app.
  /// 
  /// \return
  ///   Pointer to the input map
  inline static VInputMap * GetInputMap() 
  {
    return s_pInputMap;
  }

  /// \brief
  ///   Static helper function to create and attach a shadow map component to the passed light source using default parameters.
  /// 
  /// When bStatus is set to true, this function will attach a shadow map component of the appropriate type
  /// to the passed light source using some default parameter values. Note that this convenience function
  /// will only support renderer node index 0.
  ///
  /// \param pLight
  ///   Light source to which to attach the shadow map component
  /// 
  /// \param bStatus
  ///   When set to true, a new shadow map component will be created and attached to the light if there
  ///   is no shadow map component attached to the light source yet. If set to false and the light 
  ///   has a shadow map component attached, this shadow map component will be detached from the light source.
  static void SetShadowsForLight(VisLightSource_cl *pLight, bool bStatus);

  /// \brief
  ///   Static helper function to create a forward rendering system with default parameters.
  static void CreateForwardRenderer();

  /// \brief
  ///   Static helper function to destroy the forward rendering system previously created with CreateForwardRenderer().
  static void DestroyForwardRenderer();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Implement VisionApp_cl callback
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;
  virtual void OnLoadSceneStatus(int iStatus, float fPercentage, const char* pszStatus) HKV_OVERRIDE;

  /// \brief
  ///   Input map used to map input controls to triggers
  static VInputMap* s_pInputMap;

  /// \brief
  ///   Resets the quit state of the application
  inline void ResetQuitState(){m_bQuit=false;}

  /// \brief
  ///   Returns true if the vFileServe file manager is installed.
  static bool IsFileServerInstalled() { return s_bIsFileserverInstalled; }

#if defined(_VISION_MOBILE)
  /// \brief
  ///   Show exit dialog
  void ShowExitDialog();
#endif

protected:
  /// \brief
  ///   Initializes the ThreadManager and creates worker threads.
  ///
  /// \param iNumWorkerThreads
  ///   Number of threads to be used. A negative value means that a suitable number of threads will be
  ///   used, typically (but not necessarily) equal to the number of hardware threads in the
  ///   system.
  ///
  /// \param bFixedCPUAssignment
  ///   If worker threads should be fixed to a certain CPU.
  void InitThreadManager(int iNumWorkerThreads, bool bFixedCPUAssignment = false);

  /// \brief 
  ///   Only retrieves the current screen content and stores in into a member buffer.
  ///
  /// Call StoreScreenShotAsLoadBackgroundEnd to actually save the screenshot to disk.
  void StoreScreenShotAsLoadBackgroundStart();

  /// \brief
  ///   Stores a screen shot as a new background image for the loading screen
  ///   (if successful).
  ///
  /// \param szFileName
  ///   The file name of the image file
  /// \param fBrightness
  ///   brightness of the screen shot 
  /// \param fSaturation
  ///   The saturation of the screen shot. (0 = gray scale, 1 = no change)
  void StoreScreenShotAsLoadBackgroundEnd(const char* szFileName, float fBrightness, float fSaturation);

#if defined(_VISION_ANDROID)
  /// \brief 
  ///   Optional handling of the Android Back Button event. 
  ///
  /// Return true if you want to override the default behavior, false otherwise.
  virtual bool OnAndroidBackButtonPressed(){ return false; }
#endif

  void GetLoadingScreenExtents(float& fLeft, float& fTop, float& fRight, float& fBottom) const;
  void GetProgressBarExtents(float& fLeft, float& fTop, float& fRight, float& fBottom) const;

  // protected data    
  bool m_bShowFrameRate;
  bool m_bShowHelp; 
 
  VSmartPtr<VisMouseCamera_cl> m_spCamera;
  int m_iLastCameraPositionEntityIndex;
  char m_szCurrentDir[FS_MAX_PATH];

  short m_iHelpTextOfs[2];
  DynArray_cl<char *> m_pszHelpText;
  int m_iNumHelpLines;

  float m_fFPSPos, m_fCurrentFPS, m_fCurrentFrameTime;

  // protected functions
  void ProcessKeys();
  void DefineKeys();

  void UpdateOverlays();
  void UpdateFPS();
  
  void PrintHelpText();
  
  void AdjustFOV(float fDir);


#if defined(_VISION_MOBILE)

  VGUIMainContextPtr m_spGUIContext;
  VDialogPtr m_spProfilingDlg;
  VDialogPtr m_spExitDlg;
  void ShowProfilingMenu();
  void EnsureGUIContextCreated();

#endif

#if defined(_VISION_WIIU)

  void InitDRCDemo(VWiiUDRC eDrc = V_DRC_FIRST);

  void InitDRCCopyMode(VWiiUDRC eDrc = V_DRC_FIRST);

  void InitDRCWithRenderloop(VWiiUDRC eDrc, IVisRenderLoop_cl* pRenderLoop);

  void CleanupDRC();

  VisRenderContextPtr m_spDRCContext;
  VisRenderableTexture_cl* m_pDRCCopyBuffer;

#endif

#if defined(_VISION_IOS)
  static char m_szRootDirectory[FS_MAX_PATH];
  static char m_szBaseDataDirectory[FS_MAX_PATH];
  static char m_szCommonDataDirectory[FS_MAX_PATH];
  static char m_szCachesDirectory[FS_MAX_PATH];
  static char m_szDocumentsDirectory[FS_MAX_PATH];

#elif defined(_VISION_ANDROID)
  static char m_szBaseDataDirectory[FS_MAX_PATH];
  static char m_szCommonDataDirectory[FS_MAX_PATH];

  static char m_szSDCardDirectory[FS_MAX_PATH];
  static char m_szCacheDirectory[FS_MAX_PATH];

  static char m_szAPKDirectory[FS_MAX_PATH];

#elif defined(_VISION_WINRT)

  static char m_szSDCardDirectory[FS_MAX_PATH];
  static char m_szCacheDirectory[FS_MAX_PATH];

#endif

  VSceneLoader m_sceneLoader;
  IVisBackgroundResourceRestorer_cl* m_pBackgroundResourceRestorer;
  ApplicationState m_appState;
  bool m_bScreenshotPending;
#if defined(_VISION_IOS)
  bool m_bScreenshotForBackgroundPending; ///< Needed for iOS in order to get the screen shot before swapping buffers
#endif

  // Loading screen
  bool m_bShowLoadingProgress;
  int m_iInsideLoadWorldCounter;
  
  LoadingScreenSettings m_loadingScreenSettings;
  LoadingScreenSettings m_loadingScreenSettingsBackup; ///< Needed for when the application goes to the background

  unsigned char* m_pTmpBackgroundScreenshot;
  VisScreenMaskPtr m_spBackground;
  VisScreenMaskPtr m_spBackgroundColor;
  float m_fFadeOutPos;

  int m_iFrameCtr;

  int m_iSampleFlags;
  VisScreenMaskPtr m_spLogoOverlay;
  float m_fCurrentFadeOutTime, m_fFadeInTime;
  int m_iXDist;
  bool m_bLogoFadeInDone;
  
  bool m_bSampleInitialized;
  
  bool m_bTripleHead;

  #ifdef _VR_DX11
    D3D_FEATURE_LEVEL m_FeatureLevels[ VSAMPLE_DX11_FEATURE_LEVEL_COUNT ];
  #endif

  VisScreenMaskPtr m_spProgressBar;
  VisScreenMaskPtr m_spProgressBack;

  VGraphObject *m_pTimeStepGraph;
  int m_iUnfilteredTimeStepCurveIndex;
  int m_iTimeStepCurveIndex;

  static bool s_bIsFileserverInstalled;

  ///
  /// @}
  ///

};

typedef VSmartPtr<VisSampleApp> VisSampleAppPtr;



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
