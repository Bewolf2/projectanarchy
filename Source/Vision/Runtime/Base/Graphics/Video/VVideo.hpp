/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VVideo.hpp

#ifndef DC_VHAL_VVIDEO_HPP
#define DC_VHAL_VVIDEO_HPP

#define MAX_SECONDARY_WINDOW 4

#if defined(_VISION_PS3)
#ifndef SPU
// GCM
#include <cell/gcm.h>
#include <cell/resc.h>
#include <Vision/Runtime/Base/System/Memory/VMemoryBasePS3.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VGraphicsMemoryPS3.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VSuspendedRenderingPS3.hpp>
#endif
#endif

#if defined(WIN32) && !defined(_VISION_WINRT) 

#include <Vision/Runtime/Base/Graphics/Video/VWindow.hpp>

#define VVIDEO_DEFAULTWIDTH 1024
#define VVIDEO_DEFAULTHEIGHT 768 

#ifdef _VR_DX9

struct D3DDevice;
struct Direct3D;

#elif defined(_VR_DX11)
#define MAX_SWAP_CHAINS 64

#endif  

#elif defined(_VISION_WINRT) 
#define VVIDEO_DEFAULTWIDTH 1280
#define VVIDEO_DEFAULTHEIGHT 720
  #define MAX_SWAP_CHAINS 2  // Two different overlays possible

#elif defined (_VISION_XENON)
// Xenon default is 720p HDTV
#define VVIDEO_DEFAULTWIDTH 1280
#define VVIDEO_DEFAULTHEIGHT 720

#elif defined (_VISION_PS3)
#define VVIDEO_DEFAULTWIDTH 1280
#define VVIDEO_DEFAULTHEIGHT 720
#define VVIDEO_PRESENTPARAMFLAGS 0



#elif defined (_VISION_LINUX)
#define VVIDEO_DEFAULTWIDTH 1024
#define VVIDEO_DEFAULTHEIGHT 768
#define VVIDEO_PRESENTPARAMFLAGS 0
//@@@L  const unsigned int g_MultiSampleTypes[5]={GL_MULTISAMPLING_NONE_SCE, GL_MULTISAMPLING_2X_DIAGONAL_CENTERED_SCE, GL_MULTISAMPLING_4X_SQUARE_CENTERED_SCE, GL_MULTISAMPLING_4X_SQUARE_CENTERED_SCE, GL_MULTISAMPLING_4X_SQUARE_CENTERED_SCE};


#elif defined(_VISION_PSP2)
#define VVIDEO_DEFAULTWIDTH 480
#define VVIDEO_DEFAULTHEIGHT 272
#define VVIDEO_DISPLAY_STRIDE_IN_PIXELS 512
/*
#define VVIDEO_DEFAULTWIDTH 960
#define VVIDEO_DEFAULTHEIGHT 544
#define VVIDEO_DISPLAY_STRIDE_IN_PIXELS 1024
*/
#define VVIDEO_PRESENTPARAMFLAGS 0

#elif defined (_VISION_IOS)
#define VVIDEO_DEFAULTWIDTH 1024
#define VVIDEO_DEFAULTHEIGHT 768
#define VVIDEO_PRESENTPARAMFLAGS 0

#elif defined(_VISION_ANDROID)
#define VVIDEO_DEFAULTWIDTH 1024
#define VVIDEO_DEFAULTHEIGHT 600
#define VVIDEO_PRESENTPARAMFLAGS 0

#elif defined (_VISION_WIIU)
#define VVIDEO_DEFAULTWIDTH 1280
#define VVIDEO_DEFAULTHEIGHT 720
#define VVIDEO_PRESENTPARAMFLAGS 0

#else
#error Undefined platform!
#endif

class VVideo;

// flags for locking buffers
#define VIS_LOCKFLAG_NONE               0x00000000
#define VIS_LOCKFLAG_DISCARDABLE        0x00000001
#define VIS_LOCKFLAG_READONLY           0x00000002
#define VIS_LOCKFLAG_NOOVERWRITE        0x00000004
#define VIS_LOCKFLAG_REPLACE_ALL        0x00000008

#if defined (WIN32) ||defined (_VISION_XENON) ||defined (_VISION_PS3)

#ifdef _VISION_WINRT

struct VWinRTConfig
{
  VWinRTConfig()
  {
    ViewWidth = ViewHeight = 0;
    DisplayDensity = 1.0f;
    ScreenRotation = 0; //0,90,180,270
    CoreWindow = NULL;
    #ifdef _VISION_METRO
      SwapChainBackgroundPanelNative = NULL;
   #endif
 }
  
  IUnknown* CoreWindow;
  #ifdef _VISION_METRO
      IUnknown* SwapChainBackgroundPanelNative; // Only set this for hybrid XAML/D3D apps
  #endif

  unsigned int ViewWidth;
  unsigned int ViewHeight;
  float DisplayDensity;
  unsigned int ScreenRotation;
};
#endif

#ifdef _VR_GCM

/// \brief
///   Provides access to the basic initialization data for GCM.
struct VGcmConfig
{

  /// \brief
  ///   Constructor. Sets values to their default settings.
  VGcmConfig()
  {
    // default and minimum command buffer size
    uiCommandBufferSize = 1024*1024 - 16384;//0x10000;
    uiMappedMainMemorySize = 6 * 1024 * 1024;
    #ifdef HK_DEBUG_SLOW
      bUseOptimizedFifo = false;
    #else
      bUseOptimizedFifo = true;
    #endif
  }

  uint32_t uiCommandBufferSize;     ///< Sets the command buffer size, this has to be a at least 64k (see cellGcmInit documentation; default is 1 MB minus 16 KB)
  uint32_t uiMappedMainMemorySize;  ///< Size of the buffer for mapped main memory (default is 6 MB)
  bool     bUseOptimizedFifo;       ///< true to use CELL_GCM_DEFAULT_FIFO_MODE_OPTIMIZE, false to use CELL_GCM_DEFAULT_FIFO_MODE_TRADITIONAL (default is true in release build, false in debug build)
};

/// \brief
///   Structure describing frame buffer properties for initialization
struct VGcmFrameBuffer
{

  /// \brief
  ///   Unused.
  VGcmFrameBuffer()
  {
  }

  /// \brief
  ///   Sets the frame buffer properties.
  void Set( void* nAddress, uint32_t nOffset, uint32_t nPitch, uint32_t nFormat )
  {
    Address = nAddress;
    Offset = nOffset;
    Pitch = nPitch;
    Format = nFormat;
  }

  void* Address;        ///< The address of the framebuffer
  uint32_t Offset;      ///< Offset corresponding to the address of the framebuffer
  uint32_t Pitch;       ///< Pitch of the framebuffer
  uint32_t Format;      ///< The format used for the framebuffer (one of the legal GCM framebuffer format constants, see Sony PS3 documentation for further details)
};

/// \brief
///   This enum holds the indices for the different frame buffers the application may create.
enum VVIDEO_FrameBufferIndex
{
  VVIDEO_FRAME_BUFFER_COLOR1 = 0,       ///< Front Buffer
  VVIDEO_FRAME_BUFFER_COLOR2,           ///< Back Buffer
  VVIDEO_FRAME_BUFFER_DEPTHSTENCIL,     ///< Depth-Stencil Buffer
  VVIDEO_FRAME_BUFFER_AA,               ///< Anti-aliasing buffer

  VVIDEO_FRAME_BUFFER_MAX
};

/// \brief
///   This structure stores the display configuration of the display used by gcm. It will be filled by the
///   GCM/screen initialization functions.
struct VGCMDisplayConfig
{

  /// \brief
  ///   Default constructor. 
  VGCMDisplayConfig()
  {
    CurrentBuffer = 0;
    Width = Height = 0;
    WidthAA = HeightAA = 0;
    AAMode = CELL_GCM_SURFACE_CENTER_1;
    BackBufferCount = 0;

    for( uint32_t i = 0; i < VVIDEO_FRAME_BUFFER_MAX; i++ )
      FrameBufferUsed[i] = false;
  }

  void NextBuffer()
  {
    CurrentBuffer = (CurrentBuffer == VVIDEO_FRAME_BUFFER_COLOR1) ? VVIDEO_FRAME_BUFFER_COLOR2 : VVIDEO_FRAME_BUFFER_COLOR1;
  }

  uint32_t CurrentBuffer;         ///< The currently used render buffer (index into the FrameBuffers table).
  uint32_t Width;                 ///< Width of the display mode.
  uint32_t Height;                ///< Height of the display mode.

  uint32_t WidthAA;               ///< Width of the anti-aliased buffer. If no anti-aliasing is used, this is equal to Width
  uint32_t HeightAA;              ///< Height of the anti-aliased buffer. If no anti-aliasing is used, this is equal to Height

  uint32_t AAMode;                ///< Stores the anti-aliasing mode (CELL_GCM_SURFACE_CENTER_1 = none, CELL_GCM_SURFACE_DIAGONAL_CENTERED_2 = 2x, CELL_GCM_SURFACE_SQUARE_ROTATED_4 = 4x)


  uint32_t BackBufferCount;       ///< Stores the backbuffer count.

  VGcmFrameBuffer FrameBuffers[VVIDEO_FRAME_BUFFER_MAX];      ///< Array of render targets (color/depth stencil). Use the VVIDEO_FrameBufferIndex to index.
  bool FrameBufferUsed[VVIDEO_FRAME_BUFFER_MAX];              ///< Array indicating which of the render targets are actually used.
};

#endif

#elif defined(_VISION_IOS)

class CAEAGLLayer;

/// \brief
///   This structure stores the display configuration of the display used by GLES2 on iOS. It will be filled by the
///   GLES2/screen initialization functions.
/// \note
///   Changing the backbuffer format (R5G6B5 or RGBA8) can be done by modifying the view properties (default implemention is in VisEAGLViewIOS.m)
struct VIOSGLES2Config
{
  VIOSGLES2Config()
  {
    ViewWidth = ViewHeight = 0;
    DisplayDensity = 1.0f;
    DisplayDensityDPI = -1;
    LandscapeMode = false;
    LazyShaderCompilation = true;
    GLLayer = NULL;
  }

  unsigned int ViewWidth;             ///< Stores the display width in pixels
  unsigned int ViewHeight;            ///< Stores the display height in Pixels
  float DisplayDensity;               ///< Stores the display density as content scale factor (1.0f on non retina devices, 2.0f on retina devices)
  int DisplayDensityDPI;              ///< Stores the display density in DPI
  bool LandscapeMode;                 ///< Stores if the device is currently in landscape mode or not.
  bool LazyShaderCompilation;         ///< If true, GLES2 shader programs are created when they are first used rather than at load time. This reduces load time and saves memory, but can result in runtime lags.

  CAEAGLLayer* GLLayer;               ///< Stores the eagl layer used for rendering
};

#elif defined(_VISION_PSP2)
class VRazorLiveMetrics;

/// \brief
///   This structure stores the display configuration of the display used by GXM. It will be filled by the
///   GCM/screen initialization functions.  See also the SceGxmContextParams chapter in the GXM documentation 
//    in the PSP2 SDK.
struct VGXMConfig
{
  /// \brief
  ///   Constructor. Sets values to their default settings.
  VGXMConfig();

  unsigned int uiParameterBufferSize;         ///< The parameter buffer size (SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE by default)
  bool bEnableRazorHUD;                       ///< Enables Razor HUD when true
  bool bEnableRazorCapture;                   ///< Enables Razor capture when true
  bool bUseExplicitDepthBuffer;               ///< An explicit depth buffer is allocated and preserved between scenes
  VRazorLiveMetrics *pLiveMetrics;            ///< Receives live metrics when non-null

  uint32_t uiDisplayBufferCount;              ///< Number of display buffers
  uint32_t uiMaxPendingSwaps;                 ///< Maximum pending display swaps
  uint32_t uiContextHostMemSize;              ///< The size of the host memory for the SceGxmContext structure
  uint32_t uiVDMRingBufferMemSize;            ///< VDM ring buffer size
  uint32_t uiVertexRingBufferMemSize;         ///< Vertex ring buffer size
  uint32_t uiFragmentRingBufferMemSize;       ///< Fragment ring buffer size
  uint32_t uiFragmentUsseRingBufferMemSize;   ///< USSE fragment ring buffer size
  uint32_t patcherBufferSize;                 ///< The size of the buffer used by the shader patcher
  uint32_t patcherVertexUsseSize;             ///< Shader patcher USSE vertex heap size
  uint32_t patcherFragmentUsseSize;           ///< Shader patcher USSE fragment heap size
};

/// \brief
///   The data passed to the display callback.
struct VGXMDisplayCallbackData
{
  SceDisplayFrameBuf m_frameBuf;              ///< Frame buffer
  bool m_bUseVSync;                           ///< V-sync is used when true
};

/// \brief
///   The GXM display buffer
class VGXMDisplayBuffer
{
public:
  /// \brief
  ///    Default constructor
  VGXMDisplayBuffer()
    : m_pData(0)
    , m_pDepthData(0)
    , m_ID(0)
    , m_DepthID(0)
    , m_pFragmentNotifications(0)
    , m_nMaxNotification(0)
    , m_nCurrentNotification(0)
  {

  }


  /// \brief
  ///   Initializes the display buffer object
  /// 
  /// \param width
  ///   Display buffer width
  /// 
  /// \param height
  ///   Display buffer height
  ///
  /// \param stride
  ///   Display buffer stride
  ///
  /// \param multiSample
  ///   Multisampling flag
  ///
  /// \param pFragmentNotifications
  ///   The address of end scene fragment notifications
  ///
  /// \param notificationCount
  ///   Number of notifications to reserve
  ///
  /// \param bUseExplicitDepth
  ///   When true, forces the main context to create an explicit depth buffer
  ///
  void Initialize(unsigned int width, unsigned int height, unsigned int stride, bool multiSample, volatile uint32_t *pFragmentNotifications, uint32_t notificationCount, bool bUseExplicitDepth = false);

  /// \brief
  ///   Destroys the display buffer object
  void Destroy();

  /// \brief
  ///   Returns the sync object
  SceGxmSyncObject *GetSyncObject() const;

  /// \brief
  ///   Returns the display render surface
  SceGxmColorSurface *GetRenderSurface();

  /// \brief
  ///   Returns the display depth stencil surface
  SceGxmDepthStencilSurface *GetDepthStencilSurface();

  /// \brief
  ///   Returns the display texture object
  SceGxmTexture &GetTexture();

  /// \brief
  ///   Returns the display buffer data
  void *GetData() const;

  /// \brief
  ///   Returns the id associated with the display buffer data
  SceUID GetID();

  /// \brief
  ///   Gets the current fragment notification address
  volatile uint32_t *GetFragmentNotification() const;

  /// \brief
  ///   Gets a new fragment notification address
  volatile uint32_t *NextFragmentNotification();

  /// \brief
  ///   Gets the display buffer width
  uint32_t GetWidth();

  /// \brief
  ///   Gets the display buffer height
  uint32_t GetHeight();

protected:
  void *m_pData, *m_pDepthData;
  SceUID m_ID, m_DepthID;
  SceGxmColorSurface m_ColorSurface;
  SceGxmDepthStencilSurface m_DepthStencilSurface;
  SceGxmTexture m_Texture;
  SceGxmSyncObject* m_pSyncObject;

  volatile uint32_t *m_pFragmentNotifications;
  uint32_t m_nMaxNotification;
  uint32_t m_nCurrentNotification;
  uint32_t m_nWidth, m_nHeight;
};

#elif defined(_VISION_ANDROID)
#include <EGL/egl.h>

/// \brief
///   This structure stores the display orientation from the Android Surface
enum EDisplayRotation
{
  ROTATION_0 = 0,
  ROTATION_90 = 1,
  ROTATION_180 = 2,
  ROTATION_270 = 3,
};

/// \brief
///   This structure stores the display configuration of the display used by GLES2 on Android. It will be filled by the
///   GLES2/screen initialization functions.
struct VAndroidGLES2Config
{
  VAndroidGLES2Config()
  {
    ViewWidth = ViewHeight = 0;
    DisplayDensity = 1.0f;
    DisplayDensityDPI = -1;
    DisplayXDPI = -1.0f;
    DisplayYDPI = -1.0f;
    Display = EGL_NO_DISPLAY;
    Context = EGL_NO_CONTEXT;
    Surface = EGL_NO_SURFACE;
    Config = EGLConfig(0);
    Transparent = false;
    UseR5G6B5Format = false;
    bEglSurfaceInitialized = false;
    bEglWindowInitialized = false;
    LazyShaderCompilation = true;
    bRunWhileSleeping = false;
    bDisablePVRUsage = false;
  }

  EDisplayRotation DisplayRotation; ///< Stores the currently rotation of the display of the device (used by the input system)
  unsigned int ViewWidth;           ///< Stores the view width of the buffer given by the OS specified GLES2 surface
  unsigned int ViewHeight;          ///< Stores the view height of the buffer given by the OS specified GLES2 surface

  float DisplayDensity;             ///< Stores the logical density of the display as reported by Android (see http://developer.android.com/reference/android/util/DisplayMetrics.html - density)
  int DisplayDensityDPI;            ///< Stores the density in DPI of the display
  float DisplayXDPI;                ///< Stores the exact DPI in the x dimension of the display
  float DisplayYDPI;                ///< Stores the exact DPI in the y dimension of the display

  EGLDisplay Display;               ///< Stores the used egl display context
  EGLSurface Surface;               ///< Stores the used egl surface
  EGLContext Context;               ///< Stores the egl context
  EGLConfig Config;                 ///< Stores the chosen egl config (based on the parameters below)

  bool Transparent;                 ///< Configuration value which allows to specify that the backing EGL surface should be created transparent or not (for overlay rendering for example), default = false
  bool UseR5G6B5Format;             ///< Internal value which stores whether a RGBA8 or R5G6B5 backing surface was created
  bool bEglSurfaceInitialized;      ///< Internal value which stores if there currently is a egl surface correctly initialized
  bool bEglWindowInitialized;       ///< Internal value which stores if there currently is a egl window correctly initialized
  bool LazyShaderCompilation;       ///< Configuration value which allows to enable / disable lazy shader compilation (speeds up loading times but may produce small stalls when spawning new models), default = true
  bool bRunWhileSleeping;           ///< Configuration value which allows the app to run even when in background (not recommended due to battery consumption - but may be useful for automated tests), default = false
  bool bDisablePVRUsage;            ///< Configuration value which allows to disable PVR texture usage (useful when stripping PVR textures from the APK for example), default = false
};

#elif defined(_VISION_WIIU)

/// \brief
///   This structure stores the display configuration of the display used by the WiiU. It is also possible to
///   configure the remote controller rendering settings here.
struct VWiiUDisplayConfig
{

  VWiiUDisplayConfig()
  {
    pfnGX2RAlloc = NULL;
    pfnGX2RFree = NULL;

    uiCommandBufferSize = 1024 * 1024 * sizeof(u32);

    uiSupportedDRCCount = 1;
    bDRCHiZEnabled = true;
    b30HzDRCMode = false;

    uiDRCRenderWidth = 854;
    uiDRCRenderHeight = 480;

    uiDRCDepthBits = 24;
    bDRCNeedsStencilBuffer = true;

    drcMultiSampleMode = 0;
  }


  
  GX2RAllocateFunc pfnGX2RAlloc;        ///< Custom GX2R allocation functions (default = Vision provided functions)
  GX2RFreeFunc pfnGX2RFree;             ///< Custom GX2R allocation functions (default = Vision provided functions)
  u32 uiCommandBufferSize;              ///< Command buffer size (default = 4 Mb)
  u32 uiSupportedDRCCount;              ///< How many drcs does the title support, set to 0 if no drc support is wished to save some memory (default = 1)
  u32 uiDRCRenderWidth; u32 uiDRCRenderHeight;    ///< DRC render resolution (default = native drc resolution of 854 * 480)
  u32 drcMultiSampleMode;               ///< DRC msaa (default = off)
  u32 uiDRCDepthBits;                   ///< DRC depth bits (default = 24)
  bool bDRCNeedsStencilBuffer;          ///< true if the DRC depth buffer should have stencil bits (default = true), note that this will lead to D24_S8 usage (ignoring lower settings for uiDRCDepthBits)
  bool b30HzDRCMode;                    ///< Should the DRCs run in 30Hz mode (default = false). Note that with 2 DRCs it will always be 30Hz - this has only an effect with single mode.
  bool bDRCHiZEnabled;                  ///< Do the DRC depth buffers (if used) want to use HiZ (default = true)

};

#else
#warning "Probably missing platform!"
#endif

/// \brief
///   Describes the multisampling type
enum VVIDEO_Multisample
{
  VVIDEO_MULTISAMPLE_OFF = 0,       ///< no multisampling
  VVIDEO_MULTISAMPLE_2TIMES = 1,    ///< 2x multisampling 
  VVIDEO_MULTISAMPLE_4TIMES = 2,    ///< 4x multisampling
  VVIDEO_MULTISAMPLE_8TIMES = 3,    ///< 8x multisampling
  VVIDEO_MULTISAMPLE_16TIMES = 4    ///< 16x multisampling
};


/// \brief
///   Short description of a video mode
/// 
/// This class represents a compact form of specifying a video mode. It can be used as an input to
/// the constructor of VVideoConfig.
struct VVideoMode 
{
  int m_iXRes;                  ///< Horizontal resolution 
  int m_iYRes;                  ///< Vertical resolution
  int m_iBPP;                   ///< Backbuffer color depth in bits per pixel
  DWORD m_Format;               ///< Desired back buffer format (one of the types in VTextureLoader::VTextureFormat_e)
  int m_iRefreshRate;           ///< Monitor refresh rate in Hz (use 0 for default)
  unsigned int m_iXboxFlags;    ///< Reserved
};

#ifdef WIN32

#ifdef _VR_DX9

enum D3D9ExtendedVersionFlags // Internal flags
{
  ExtVersionPreferred = 1,
  ExtVersionUsed = 2
};

/// \brief
///   Stores a device configuration. Internal use only, Win32 DX9 only.
struct DeviceConfig_t
{
  DEVMODEA m_OrigDevMode;                     ///< internal
};

#endif

#if defined(_VR_DX11)

/// \brief
///   Stores an output/adapter configuration. Internal use only, Win32 DX10/DX11 only.
struct OutputConfig_t
{
  VSmartPtr<IDXGIOutput> pOutput;
  VSmartPtr<IDXGIAdapter> pAdapter;
};
#endif

#endif

/// \brief
///   Class for specifying and storing video configurations.
/// 
/// This class stores a specific video configuration, typically used for a single screen or window (or swap chain in DirectX terminology).
/// 
/// It does not only contain information about the video mode itself, but also about the used
/// device and the screen or window to render to.
class VVideoConfig
{
public:

  /// \brief
  ///   Constructor for the VVideoConfig class
  /// 
  /// Creates a VVideoConfig instance from width and height settings.
  /// For all other parameters, the default data is used. The color depth will be set to the
  /// current desktop color depth, or the default color depth of the platform for non-Windows platforms.
  /// 
  /// \param iWidth
  ///   width of the screen or window
  /// 
  /// \param iHeight
  ///   height of the screen or window
  VBASE_IMPEXP VVideoConfig(int iWidth = VVIDEO_DEFAULTWIDTH, int iHeight = VVIDEO_DEFAULTHEIGHT);

  /// \brief
  ///   Constructor for the VVideoConfig class
  /// 
  /// Creates a VVideoConfig instance from a VVideoMode instance.
  /// 
  /// The parameters are set to the respective values in the VVideoMode structure. If the color
  /// depth in the VVideoMode structure is set to 0, the color depth will be set to the current
  /// desktop/screen color depth.
  /// 
  /// For all remaining parameters, the default data is used. 
  /// 
  /// \param vm
  ///   Reference to a valid VVideoMode structure
  VBASE_IMPEXP VVideoConfig(const VVideoMode &vm);

  /// \brief
  ///  Returns the ratio of physical width to height of the back buffer as displayed on the output device.
  inline float GetDisplayAspectRatio() const
  {
    return float(m_iPhysicalAspectRatio[0]) / float(m_iPhysicalAspectRatio[1]);
  }

  /// \brief
  ///  Returns the ratio of physical width to height of a single pixel of the back buffer as displayed on the output device.
  inline float GetPixelAspectRatio() const
  {
    float fStorageAspectRatio = float(m_iXRes) / float(m_iYRes);
    return GetDisplayAspectRatio() / fStorageAspectRatio;
  }

  int m_iXPos;                          ///< Horizontal position of the window (in windowed mode, Windows only - irrelevant for other platforms)
  int m_iYPos;                          ///< Vertical position of the window (in windowed mode, Windows only - irrelevant for other platforms)
  int m_iXRes;                          ///< Horizontal size of the target buffer
  int m_iYRes;                          ///< Vertical size of the target buffer
  int m_iRefreshRate;                   ///< Refresh rate in Hz, 0 if default refresh rate is to be used

  // Monitor
  int m_iPhysicalAspectRatio[2];        ///< A fraction representing the monitor's physical width to height ratio

  // Front Buffer / Depth-stencil buffer
  int m_iFBColorBits;                   ///< Number of front-buffer color bits
  int m_iFBDepthStencilBits;            ///< Number of depth+stencil bits
  bool m_bFBUseStencilBuffer;           ///< True if a stencil buffer should be used

  DWORD m_iFBD3DFormat;                 ///< Front Buffer Pixel Format (of type D3DFORMAT). (Only used in Windows DirectX9 version)

  // Back Buffer
  int m_iBBColorBits;                   ///< Number of color bits for the back buffer
  bool m_bBBFloat;                      ///< True if floating-point backbuffer is desired
  DWORD m_iBBD3DFormat;                 ///< Back Buffer Pixel Format (of type D3DFORMAT), overrides m_iBBColorBits and m_bBBFloat. (Only used in Windows DirectX9 version)

  int m_iBBCount;                       ///< Number of back buffers, usually 1 (for double-buffering) or 2 (for triple-buffering).

  int m_iAdapter;                       ///< Device index; all monitors that belong to the Windows Desktop can be accessed via this index. (Windows only)

  unsigned int m_iFlags;                ///< Reserved.

  bool m_bWaitVRetrace;                 ///< true: Perform vsync; false: don't perform vsync.

  bool m_bHideWindow;                   ///< true to keep the Vision window hidden (Windows only)
  bool m_bFullScreen;                   ///< true for fullscreen, false for windowed (Windows only)
  VString m_szWindowTitle;              ///< Window Title (Windows only)

  bool m_bOpenGL;                       ///< Reserved.

  VVIDEO_Multisample m_eMultiSample;    ///< Multisampling type

#if ( defined (WIN32)  || defined (_VISION_XENON) ) && !defined( _VISION_WINRT )
  HWND m_hWnd;                          ///(deprecated, use VWindow::CreatedWindowHandle)
  HWND m_hParentWnd;                    ///(deprecated, use VWindow::ParentWindowHandle)
#endif

#if defined (WIN32)
  bool m_bEnableNVPerfHUD;              ///< Initializes the graphics device with NV Performance HUD enabled. Requires running the application from NVPerfHUD. (Windows only)
  bool m_bForceReferenceRasterizer;     ///< Initializes the graphics device with the software reference rasterizer. Useful for automating vForge exports in virtual machines. (Windows only)

  // The following parameters are not user-definable, but are set by VVideo. The user shouldn't change them.
  int   m_iSecHandle;                   ///< Set internally by the VVideo class. Do not modify.
  #if !defined( _VISION_WINRT )
     HDC   m_hDC;                       ///< Set internally by the VVideo class. Do not modify.
     HGLRC m_hGLRC;                     ///< Set internally by the VVideo class. Do not modify.
     bool  m_bAutoWindow;               ///< Set internally by the VVideo class. Do not modify.
  #endif
#endif


#ifdef _VISION_PS3
  /// \brief
  ///   Flip mode for adaptive hsync/vsync switching
  ///
  /// \sa m_VideoSyncMode
  enum FlipMode
  {
    FlipModeNonAdaptive = 0,
    FlipMode30FPS       = 1,
    FlipMode60FPS       = 2,
  };

  bool  m_bUseTiledMemoryForRescaleTarget; ///< PS3 only: If true, tiled memory will be used for the rescale target (default is false)
  bool  m_bRescaleResolution;           ///< PS3 only: If true, libresc will be used to rescale the passed render buffer resolution to the video resolution
  int   m_iVideoWidth;                  ///< PS3 only: In case rescaling is used, the width of the actual video mode 
  int   m_iVideoHeight;                 ///< PS3 only: In case rescaling is used, the height of the actual video mode
  FlipMode m_AdaptiveFlipMode;          ///< PS3 only: If non-zero while m_bWaitVRetrace is true, either horizontal or vertical sync will be used, depending on the framerate.

  unsigned int m_iRescaleRatio;         ///< PS3 only: Either CELL_RESC_FULLSCREEN, CELL_RESC_LETTERBOX, or CELL_RESC_PANSCAN
  unsigned int m_iRescalePalTemporalMode; ///< PS3 only: Either CELL_RESC_PAL_50, CELL_RESC_PAL_60_DROP, CELL_RESC_PAL_60_INTERPOLATE, CELL_RESC_PAL_60_INTERPOLATE_30_DROP, CELL_RESC_PAL_60_INTERPOLATE_DROP_FLEXIBLE, or CELL_RESC_PAL_60_FOR_HSYNC
  unsigned int m_iRescaleInterlaceMode; ///< PS3 only: Either CELL_RESC_NORMAL_BILINEAR, CELL_RESC_INTERLACE_FILTER, CELL_RESC_3X3_GAUSSIAN, CELL_RESC_2X3_QUINCUNX, or CELL_RESC_2X3_QUINCUNX_ALT
  unsigned int m_iRescaleFlipMode;      ///< PS3 only: Either CELL_RESC_DISPLAY_VSYNC or CELL_RESC_DISPLAY_HSYNC

  unsigned char m_iAATileIndex;         ///< PS3 only, internal use only: Tile index for anti-aliasing target
  unsigned short m_iAATileTagBase;      ///< PS3 only, internal use only: Tagbase for anti-aliasing target
  unsigned char m_iRescTileIndex;       ///< PS3 only, internal use only: Tile index for rescale target
  unsigned short m_iRescTileTagBase;    ///< PS3 only, internal use only: Tagbase for rescale target
  unsigned char m_iDepthTileIndex;      ///< PS3 only, internal use only: Tile index for depth/stencil target
  unsigned short m_iDepthTileTagBase;   ///< PS3 only, internal use only: Tagbase for depth/stencil target
  unsigned char m_iTileIndex;           ///< PS3 only, internal use only: Tile index for backbuffer and frontbuffer
  unsigned short m_iTileTagBase;        ///< PS3 only, internal use only: Tagbase for backbuffer and frontbuffer
  unsigned char m_iZCullIndex;          ///< PS3 only, internal use only: ZCull Index
#endif



#ifdef _VISION_LINUX
  //@@@L fill in
#endif


#if defined(_VR_DX9)
  DWORD m_iCreateDeviceBehaviorFlags;           ///< WIN32 DX9 only: behavior flags for device creation
  DWORD m_iCreateDeviceFallbackBehaviorFlags;   ///< WIN32 DX9 only: fallback behavior flags for device creation (if the device could not be created using the primary behavior flags)
#endif

#if defined(_VR_DX11)
  DXGI_MODE_SCANLINE_ORDER m_iScanlineOrder;    ///< WIN32 DX10/DX11 only: Scanline order for the video mode
  DXGI_MODE_SCALING m_iScaling;                 ///< WIN32 DX10/DX11 only: Scaling for the video mode
  bool m_bUseDebugRuntime;                      ///< WIN32 DX10/DX11 only: Set to true to enable debug runtime
  int m_iWindowAssociationFlags;                ///< WIN32 DX10/DX11 only: Window association flags
  bool m_bIgnoreFullScreenErrors;               ///< WIN32 DX10/DX11 only: if fullscreen mode is not applied, keep throguth with windoe mode.

  D3D_DRIVER_TYPE m_iDriverType;                ///< WIN32 DX11 only: Driver Type
  D3D_FEATURE_LEVEL *m_pFeatureLevels;          ///< WIN32 DX11 only: NULL, or a list of feature levels to attempt to create.
  int m_iNumFeatureLevels;                      ///< WIN32 DX11 only: The number of feature levels in the m_pFeatureLevels list.
#endif

#ifdef _VR_GX2

  bool m_bBackBufferInMEM1; ///< This can be used to move the backbuffer to MEM2 if other rendertargets are more important (e.g. with deferred shading)

#endif

private:
  void SetDefaults(const VVideoMode &vm);

  friend class VVideo;
};





/// \brief
///   Base class for managing video-related functions.
/// 
/// This class offers video related-functions like device and video initialization, buffer swapping,
/// window creation etc.
///
/// The Vision Engine's VisVideo_cl class is derived from this class. For most applications, it is sufficient to work with the
/// higher-level functionality exposed by the VisVideo_cl class (e.g. SetMode) rather than directly calling into the low-level
/// methods of the VVideo class.
class VVideo
{

public:

  inline virtual ~VVideo()
  {
    //Nothing to do here  
  }

#ifdef WIN32

  /// \brief
  ///   Returns the handle of the application instance.
  /// 
  /// Windows PCs only.
  /// 
  /// \return
  ///   HINSTANCE hInstance: Handle to the application instance
  /// 
  /// \note
  ///   This function is only valid on Windows. It would not make sense to use it on any other
  ///   platform. Since you don't need to define WinMain, this function can be useful if
  ///   you want to perform e.g. Windows API calls.
  static VBASE_IMPEXP HINSTANCE GetInstance();

#endif

  /// \brief
  ///  Configures Vision for headless mode.
  /// 
  /// Setting this flag to true will cause Vision to skip creating windows, input devices
  /// or rendering devices as appropriate for running Vision on a server. Internal calls
  /// are redirected to dummy implementations, so it remains possible to use most graphics
  /// features just like in a standard environment.
  ///
  /// This feature only removes any GPU-side rendering overhead, but does not disable
  /// animations, skinning, particle simulation etc. See the section "General.TutorialsForProgrammers.HeadlessMode"
  /// in the Vision Engine documentation for information on how to set up Vision for server mode.
  ///
  /// This flag needs be set very early in your application (i.e., before initialization of vBase),
  /// as changing this flag after the first call that creates a window, rendering device
  /// or input device handle is not supported and will lead to undefined behavior.
  ///
  /// \param bEnabled
  ///   True to enable headless mode.
  ///
  /// \note
  ///  This function is only supported on Windows. On other platforms, calling this function
  ///  is silently ignored.
  static VBASE_IMPEXP void SetHeadlessModeEnabled(bool bEnabled);

  /// \brief
  ///  Queries whether Vision is configured to run in headless mode.
  ///
  /// \return
  ///  True or false.
  static VBASE_IMPEXP bool IsHeadlessModeEnabled();

  /// \brief
  ///   Gets the current device gamma ramp.
  /// 
  /// The passed device gamma ramp is written to the three specified arrays. Every array contains
  /// 256 values that map input colors to gamma corrected values. Valid gamma corrected values are
  /// in the range [0..1].
  /// 
  /// Currently only supported on PC DX9 and Xbox360.
  /// 
  /// \param iAdapter
  ///   Index of the Swap Chain/Device (0 for the primary device, 1 for the first secondary device,
  ///   and so on)
  /// 
  /// \param pfr
  ///   pointer to an array of 256 float entries to receive the red gamma ramp
  /// 
  /// \param pfg
  ///   pointer to an array of 256 float entries to receive the green gamma ramp
  /// 
  /// \param pfb
  ///   pointer to an array of 256 float entries to receive the blue gamma ramp
  /// 
  /// \return
  ///   bool: true if success, false if failed (the device does not support gamma correction)
  /// 
  /// \example
  ///   \code
  ///   float r[256], g[256], b[256];
  ///   Vision::Video.GetGammaRamp ( r,g,b );
  ///   \endcode
  static VBASE_IMPEXP bool GetGammaRamp ( unsigned int iAdapter, float *pfr, float *pfg, float *pfb );


  /// \brief
  ///   Sets the current device gamma ramp.
  /// 
  /// The passed device gamma ramp is set from the three specified arrays. Every array contains 256
  /// values that map input colors to gamma corrected values. Valid gamma corrected values are in
  /// the range [0..1].
  /// 
  /// An identity mapping would be <tt>for (i=0; i<256; i++) x[i] = (float)i / 255.0f;</tt>
  /// 
  /// The input values are clamped into the range [0..1].
  /// 
  /// Note that you will have to restore the gamma value later before exiting your application.
  /// 
  /// Currently only supported on PC DX9 and Xbox360.
  /// 
  /// \param iAdapter
  ///   Index of the Swap Chain/Device (0 for the primary device, 1 for the first secondary device,
  ///   and so on)
  /// 
  /// \param pfr
  ///   pointer to an array of 256 float containing the red gamma ramp
  /// 
  /// \param pfg
  ///   pointer to an array of 256 float containing the green gamma ramp
  /// 
  /// \param pfb
  ///   pointer to an array of 256 float containing the blue gamma ramp
  /// 
  /// \return
  ///   bool: true if success, false if failed (the device does not support gamma correction)
  /// 
  /// \sa Vision::Video.SaveGamma()
  /// \sa Vision::Video.RestoreGamma()
  /// 
  /// \example
  ///   \code
  ///   float r[256], g[256], b[256]; 
  ///   for ( i=0; i<256; i++ ) r[i] = g[i] = b[i] = (float)i/255.0f; //set identity gamma
  ///   Vision::Video.SetGammaRamp ( r,g,b );
  ///   \endcode
  static VBASE_IMPEXP bool SetGammaRamp ( unsigned int iAdapter, const float *pfr, const float *pfg, const float *pfb );


  /// \brief
  ///   Sets the current device gamma ramp via control values.
  /// 
  /// The passed device gamma ramp is set to a pow ramp controlled by three exponents, one for each
  /// color channel.  
  /// 
  /// The ramp function is: <tt>for (i=0; i<256; i++) x[i] = pow ((float)(i+1)/256.0f, ctrl );</tt>
  /// 
  /// The input values are clamped into the range [0.25..4.0]
  /// 
  /// Note that you will have to restore the gamma value later before exiting your application.
  /// 
  /// Currently only supported on PC DX9 and Xbox360.
  /// 
  /// \param iAdapter
  ///   Index of the Swap Chain/Device (0 for the primary device, 1 for the first secondary device,
  ///   and so on)
  /// 
  /// \param fr
  ///   red exponent
  /// 
  /// \param fg
  ///   green exponent
  /// 
  /// \param fb
  ///   blue exponent
  /// 
  /// \return
  ///   bool: true if success, false if failed (the device does not support gamma correction)
  /// 
  /// \note
  ///   The result is only visible in fullscreen mode
  /// 
  /// \sa Vision::Video.SaveGamma()
  /// \sa Vision::Video.RestoreGamma()
  /// 
  /// \example
  ///   \code
  ///   Vision::Video.SetGamma ( 0.9f,1.0f,1.1f );
  ///   \endcode
  static VBASE_IMPEXP bool SetGamma ( unsigned int iAdapter, float fr, float fg, float fb );


  /// \brief
  ///   Saves the current device gamma to a one slot storage
  /// 
  /// Gets the current device gamma ramp and saves it into a local one slot storage for  later
  /// restore operations.
  /// 
  /// It is recommended to call this function before changing gamma ramps in order to be able to
  /// restore the old user settings at application exit
  /// 
  /// Currently only supported on PC DX9 and Xbox360.
  /// 
  /// \param iAdapter
  ///   Index of the Swap Chain/Device (0 for the primary device, 1 for the first secondary device,
  ///   and so on)
  /// 
  /// \return
  ///   bool: true if success, false if failed (the device does not support gamma correction)
  /// 
  /// \example
  ///   \code
  ///   Vision::Video.SaveGamma ();
  ///   // change gamma
  ///   while ( pApplication->Run () ) ;
  ///     // run program
  ///   Vision::Video.RestoreGamma ();
  ///   \endcode
  VBASE_IMPEXP bool SaveGamma (unsigned int iAdapter);


  /// \brief
  ///   Restores the current device gamma from a one slot storage.
  /// 
  /// The local one slot storage was previously filled by the SaveGamma function.
  /// 
  /// It is recommended to call this function at application exit to restore user gamma.
  /// 
  /// You must call SaveGamma before calling this function. Otherwise the result is undefined.
  /// 
  /// Currently only supported on PC DX9 and Xbox360.
  /// 
  /// \param iAdapter
  ///   Index of the Swap Chain/Device (0 for the primary device, 1 for the first secondary device,
  ///   and so on)
  /// 
  /// \return
  ///   bool: true if success, false if failed (the device does not support gamma correction)
  /// 
  /// \example
  ///   \code
  ///   Vision::Video.SaveGamma ();
  ///   // change gamma
  ///   while ( pApplication->Run () ) ;
  ///     // run program
  ///   Vision::Video.RestoreGamma ();
  ///   \endcode
  VBASE_IMPEXP bool RestoreGamma(unsigned int iAdapter);

  /// \brief
  ///   For devices supporting querying device display resolutions, this function returns the display resolution in dots per inch (dpi).
  /// 
  /// Supported on Android, iOS, and Windows RT. Returns a default value of 96 on other platforms, which is the value used by Windows.
  VBASE_IMPEXP float GetDeviceDpi() const;

  /// \brief
  ///   Suspends rendering for automatic flipping of the screen.
  /// 
  /// While rendering is suspended, the system will automatically take care of flipping the
  /// rendering buffers at least 15 times a second. This facilitates passing certification
  /// requirements from console manufacturers.
  /// 
  /// While rendering is suspended, do not call any rendering-related functions.
  /// 
  /// This method is currently only supported on Xbox360 and Playstation 3.
  /// 
  /// Call ResumeRendering to resume rendering again.
  /// 
  /// \param bGrab
  ///   On Playstation 3, set this to true to indicate that the current back buffer (!) contents
  ///   should be copied to the internal blit texture for displaying during the suspended state. If
  ///   bGrab ist false, the contents of the internal blit texture won't be overwritten. This does
  ///   not affect any other platform.
  VBASE_IMPEXP static void StartSuspendedRendering(bool bGrab = true);

  /// \brief
  ///   Resumes rendering if it was previously suspended with VisVideo_cl::SuspendRendering.
  VBASE_IMPEXP static void StopSuspendedRendering();

  /// \brief
  ///   Returns whether rendering is currently suspended.
  inline static bool IsRenderingSuspended() { return m_bRenderingIsSuspended; }


#ifdef _VISION_PS3

  /// \brief
  ///   Specifies a temporary blit texture for suspended rendering on Playstation 3.
  /// 
  /// Using this method, you can specify a custom texture that will be used for screen
  /// flipping/copying. For instance, you can use a temporary texture that your application will
  /// need anyway (but not during loading), and thus save memory. If no blit texture is specified
  /// explicitly, calling StartSuspendedRendering will generate an internal blit texture.
  /// 
  /// When specifying a custom blit texture, its width, height, pitch, and format settings need to
  /// match those of the back buffer
  /// 
  /// \param pBlitTarget
  ///   Texture object to use as blit texture.
  VBASE_IMPEXP static void SetSuspendedRenderingBlitTexture(const CellGcmTexture *pBlitTarget);

  /// \brief
  ///   Deletes the suspended rendering blit texture and frees the allocated memory. Call this
  ///   method when using an auto-generated blit texture and you don't need suspended rendering any
  ///   more.
  VBASE_IMPEXP static void FreeSuspendedRenderingBlitTexture();

  /// \brief
  ///   Switches between v-sync and adaptive v-sync.  Has no effect if VVideoConfig::m_bWaitVRetrace 
  /// was set to false when creating the device.
  ///
  /// \param flipMode
  ///   Use VVideoConfig::FlipModeNonAdaptive to switch to v-sync, and the other values to switch to
  /// 30- and 60-fps adaptive sync, respectively.
  ///
  /// \sa VVideoConfig::FlipMode
  /// \sa VVideoConfig::m_VideoSyncMode
  /// \sa VVideoConfig::m_bWaitRetrace
  VBASE_IMPEXP static void SetAdaptiveFlipMode(VVideoConfig::FlipMode flipMode);
#endif



#if defined(_VR_DX9) || defined(_VR_DX11)

  /// \brief
  ///   Returns the currently active D3D Instance.
  /// 
  /// Only available on Xbox360 and WIN32 DirectX9. Will return NULL for WIN32 DirectX10/DirectX11.
  /// 
  /// \return
  ///   D3DDevice* or NULL
  static VBASE_IMPEXP Direct3D* GetD3DInstance();

  /// \brief
  ///   Returns the currently active D3D Device.
  /// 
  /// Only available on Xbox360 and WIN32 (DX9, DX10, and DX11).
  /// 
  /// \return
  ///   D3DDevice* or NULL
  static VBASE_IMPEXP D3DDevice* GetD3DDevice();

#if defined(_VR_DX11)

  /// \brief
  ///   Returns currently active D3D Device Context used for immediate rendering.
  static VBASE_IMPEXP D3DDeviceContext* GetD3DDeviceContext();

#endif

  /// \brief
  ///   Initializes Direct3D version of VVideo (Xbox360/DirectX9 only)
  /// 
  /// You will need to call this function on Xbox360 and PC DirectX9 prior to calling InitializeScreen.
  /// 
  /// \return
  ///   bool: true if initialization suceeded, otherwise false. Use VVideo::GetLastError to get additional
  ///   information.
  static VBASE_IMPEXP bool InitializeD3D();

  /// \brief
  ///   Deinitializes the Direct3D version of VVideo (Xbox360/DirectX9 only)
  /// 
  /// Frees memory allocated by InitializeD3D.
  /// Only supported on Xbox360 and WIN32 DX9.
  /// 
  /// \return
  ///   bool: true if the deinitialization suceeded, otherwise false. Use VVideo::GetLastError to get
  ///   additional information.
  static VBASE_IMPEXP void DeInitializeD3D();


  static VBASE_IMPEXP bool SetNewDeviceWindow(VVideoConfig &vc, HWND hNewWindow, HWND hNewParentWindow);

  #ifdef _VR_DX9
    static VBASE_IMPEXP void GetD3DPPFromVideoConfig(VVideoConfig &vc, D3DPRESENT_PARAMETERS &d3dpp);
    #ifndef _VISION_XENON
    static VBASE_IMPEXP void GetD3DExDisplayModeFromVideoConfig(VVideoConfig &vc, D3DDISPLAYMODEEX &d3ddmex);
    #endif
  #elif defined(_VR_DX11)
    static VBASE_IMPEXP bool CreateSwapChain(VVideoConfig&vc, D3DSwapChain** pSwapChain, bool bForceWindowMode = false);
    static VBASE_IMPEXP void GetSwapChainDescFromVideoConfig(VVideoConfig &vc, D3DSwapChainDesc &swapChainDesc);
    static VBASE_IMPEXP void GetModeDescFromVideoConfig(VVideoConfig &vc, DXGI_MODE_DESC &modeDesc);
	#ifndef _VISION_METRO
		static VBASE_IMPEXP const OutputConfig_t *GetOutputConfig(int iAdapter);
	#endif
  #endif

#endif

#if defined(_VR_GCM)

  /// \brief
  ///   Initializes libgcm (e.g. command buffer space, GCM context)
  static VBASE_IMPEXP bool InitializeGCM( const VGcmConfig& GcmConfig );

  /// \brief
  ///   Deinitializes libgcm
  static VBASE_IMPEXP bool DeInitializeGCM();


  /// \brief
  ///   Sets the libgcm allocator to use by the engine for all libgcm-related allocations
  /// 
  /// Only available on PS3.
  /// 
  /// \param Allocator
  ///   The new allocator
  /// 
  /// \note
  ///   The allocator has to be initialized by the user before passing it in here
  static VBASE_IMPEXP void SetGCMAllocator( IVGcmAllocator* Allocator )
  {
    // if there is already an allocator deinitialize it and release it
    if( m_GcmAllocator )
    {
      m_GcmAllocator->DeInit();
      delete m_GcmAllocator;
    }

    m_GcmAllocator = Allocator;
  }

  /// \brief
  ///   Returns the currently used libgcm allocator
  /// 
  /// Only available on PS3.
  /// 
  /// \return
  ///   IVGcmAllocator*: A pointer to the currently used allocator, if no allocator has been set
  ///   yet: NULL
  static VBASE_IMPEXP IVGcmAllocator* GetGCMAllocator()
  {
    return m_GcmAllocator;
  }


  static VBASE_IMPEXP void FillGcmSurfaceWithRenderTargetInfo(VVIDEO_FrameBufferIndex FrameBufferIndex, CellGcmSurface &sf);

  static VBASE_IMPEXP void SetGCMRendertarget( VVIDEO_FrameBufferIndex FrameBufferIndex );

  static VBASE_IMPEXP VVIDEO_FrameBufferIndex GetCurrentFramebufferIndex() { return (VVIDEO_FrameBufferIndex)m_GCMDisplayConf.CurrentBuffer; }
  static VBASE_IMPEXP VGCMDisplayConfig *GetGcmDisplayConfig() { return &m_GCMDisplayConf; }
  static VBASE_IMPEXP void SetTransferScaleBlit( uint8_t mode, uint32_t dstOffset, uint32_t dstPitch, uint32_t srcOffset, uint32_t srcPitch,
    uint32_t dstWidth, uint32_t dstHeight, uint32_t bytesPerPixel, int32_t log2XScale, int32_t log2YScale, bool bFilter );
  static VBASE_IMPEXP void CopySystemBuffer(VVIDEO_FrameBufferIndex Source, VVIDEO_FrameBufferIndex Target);

  static inline VSuspendedRendering* GetSuspendedRenderer() { return &m_SuspendedRendering; }
  static inline VVideoConfig* GetVideoConfig() { return m_pVideoConfig; }

  static inline uint8_t GetFirstFreeRSXLabel() { return LABEL_ID_LAST; } // retrieve the first RSX label not used by Vision

  static inline uint32_t GCMGetCurrentFrame() { return m_uiGcmCurrentFrame; }
  static inline uint32_t GCMGetLastFlushedFrame() { return m_uiGcmLastFlushedFrame; }
  static inline volatile uint32_t GCMGetLastFinishedFrame() { return *m_pGcmLastFinishedFrameAddress; }

  enum RSXLabelId
  {
    LABEL_ID_FLIP_CONTROL = 64,
    LABEL_ID_FRAME_COUNT,
    LABEL_ID_LAST
  };

#endif


#if defined(_VR_GLES2)

  /// \brief
  ///   Initializes OpenGLES 2.0. This function needs to be called before calling InitializeScreen().
  ///
  /// This method is only available on OpenGLES 2.0 platforms, e.g. Android and iOS.
  static VBASE_IMPEXP bool InitializeGLES2();

  /// \brief
  ///   Deinitializes OpenGLES 2.0. Only available on OpenGLES 2.0 platforms, e.g. Android and iOS.
  static VBASE_IMPEXP bool DeInitializeGLES2();

  /// \brief
  ///   Returns whether PVR textures are supported on the OpenGL ES 2.0 device. Needs to be called after screen initialization.
  static inline bool IsPVRTCSupported() { return m_bPVRTCSupported; }

  /// \brief
  ///   Returns whether ETC textures are supported on the OpenGL ES 2.0 device. Needs to be called after screen initialization.
  static inline bool IsETCSupported() { return m_bETCSupported; }

  /// \brief
  ///   Returns whether the packed depth-stencil extension is supported on the OpenGL ES 2.0 device. Needs to be called after screen initialization.
  ///
  /// Vision will automatically use packed depth-stencil targets if they are supported. They are required for shadow map support.
  ///
  /// \return
  ///  bool: true if packed depth-stencil is supported.
  static inline bool IsPackedDepthStencilSupported() { return m_bPackedDepthStencilSupported; }

  /// \brief
  ///   Returns the max texture size supported by the devices.
  ///
  /// \return
  ///   int: The max texture size (width / height) of a texture in pixels supported on the device.
  static inline int GetMaxTextureSizeSupported() { return m_iMaxGLTextureSize; }

#endif

#if defined(_VISION_IOS)

  static VBASE_IMPEXP VIOSGLES2Config* GetVideoConfig() { return &m_IOSGLES2Config; }

#endif

 #if defined(_VISION_WINRT)

  static VBASE_IMPEXP VWinRTConfig* GetVideoConfig() { return &m_winRTConfig; }
  static VBASE_IMPEXP void SetVideoConfig(VWinRTConfig& newCfg); 

#endif

#if defined(_VISION_WIIU)

  /// \brief
  ///   Initializes GX2
  static VBASE_IMPEXP bool InitializeGX2();

  /// \brief
  ///   Deinitializes GX2
  static VBASE_IMPEXP bool DeInitializeGX2();

  static VWiiUDisplayConfig* GetVideoConfig() { return &m_WiiUDisplayConfig; }

  static void* AllocMEM1(u32 uiSize, u32 uiAlignment, bool bCheckForValidPtr = true);

  static void FreeMEM1(void* pPointer);

  static void* AllocFGBucket(u32 uiSize, u32 uiAlignment);

  static void FreeFGBucket(void* pPointer);

  static void* AllocMEM2(u32 uiSize, u32 uiAlignment);

  static void FreeMEM2(void* pPointer);

  // Looks which Free* function has to be used
  static void FreeMEM(void* pPointer);

  static bool IsMEM1Pointer(void* pPointer);

  static bool IsMEM2Pointer(void* pPointer);

  static bool IsMEMFGPointer(void* pPointer);

  static void* AllocGraphicsMemory(u32 uiSize, u32 uiAlignment, bool bPreferFastMem, GX2InvalidateType eInvalidateType = (GX2InvalidateType)0);

  static GX2ContextState* GetDefaultContextState() { return m_pDefaultContextState; }

  static void RestoreDefaultContextState();

  static GX2AAMode ConvertToNativeMSType(VVIDEO_Multisample VisMSType);

  static void* VisGX2RAlloc(GX2RResourceFlags flags, u32 uiByteCount, u32 uiAlignment);

  static void VisGX2RFree(GX2RResourceFlags flags, void* pPointer);

  static GX2ColorBuffer* GetBackbuffer() { return &m_GX2BackBuffer; }

  static GX2DepthBuffer* GetDepthStencilBuffer() { return &m_GX2DepthStencilBuffer; }


  static GX2ColorBuffer* GetDRCColorBuffer(VWiiUDRC drc)
  {
    return &m_DRCColorBuffers[drc];
  }

  static GX2DepthBuffer* GetDRCDepthBuffer(VWiiUDRC drc)
  {
    return &m_DRCDepthBuffers[drc];
  }

  static inline bool IsDRCConnected(VWiiUDRC drc)
  {
    return m_bDRCConnected[drc];
  }

#endif

#if defined(_VISION_ANDROID)

  static VBASE_IMPEXP VAndroidGLES2Config* GetVideoConfig() { return &m_AndroidGLES2Config; }

  static VBASE_IMPEXP bool IsGLViewRecreationWished() { bool bRetVal = m_bGLViewRecreationWished; m_bGLViewRecreationWished = false; return bRetVal; }

#endif


#if defined(_VR_GXM)

  static VBASE_IMPEXP bool InitializeGXM(const VGXMConfig& GxmConfig);
  static VBASE_IMPEXP bool DeInitializeGXM();

  static VBASE_IMPEXP void* GXMGraphicsAlloc(SceKernelMemBlockType Type, uint32_t size, uint32_t alignment, uint32_t attribs, int32_t *uid);

  static VBASE_IMPEXP void *GXMVertexUSSEAlloc(uint32_t size, SceUID *uid, uint32_t *usseOffset);
  static VBASE_IMPEXP void *GXMFragmentUSSEAlloc(uint32_t size, SceUID *uid, uint32_t *usseOffset);
  static VBASE_IMPEXP void GXMVertexUSSEFree(SceUID uid);
  static VBASE_IMPEXP void GXMFragmentUSSEFree(SceUID uid);

  static VBASE_IMPEXP SceGxmRenderTarget *GXMCreateRenderTarget(uint16_t resX, uint16_t resY, VVIDEO_Multisample ms);

  static VBASE_IMPEXP void  GXMGraphicsFree(SceUID uiID);
  static VBASE_IMPEXP uint32_t GXMAlignSize(uint32_t Size, uint32_t Alignment);

  static VBASE_IMPEXP void GXMGetGraphicsMemStats( VMemoryStatistics_t &dest );

  static inline VBASE_IMPEXP SceGxmShaderPatcher* GXMGetShaderPatcher() { return m_pGxmShaderPatcher;} 
  static inline VBASE_IMPEXP SceGxmContext* GXMGetContext() { return m_pGxmContext; }

  static VBASE_IMPEXP VGXMConfig *GXMGetConfig() { return &m_GxmConfig; }

  static VBASE_IMPEXP VGXMDisplayBuffer* GXMGetCurrentBackbuffer() { return &m_pDisplayBuffers[m_uiCurrentBackBuffer]; }
  static VBASE_IMPEXP VGXMDisplayBuffer* GXMGetCurrentFrontbuffer()  { return &m_pDisplayBuffers[m_uiCurrentFrontBuffer]; }

  static VBASE_IMPEXP uint32_t GXMGetCurrentBackbufferIndex() { return m_uiCurrentBackBuffer; }

  static VBASE_IMPEXP VGXMDisplayBuffer* GXMGetBackbuffers() 
  { 
    return m_pDisplayBuffers; 
  }

  static VBASE_IMPEXP SceGxmRenderTarget* GXMGetBackbufferRenderTarget() { return m_pMainRenderTarget; }

  static VBASE_IMPEXP unsigned int GXMGetFrameCount() { return m_uiGXMFrameCount; }
#endif


#if defined(_VR_OPENGL)

  static VBASE_IMPEXP bool InitializeOpenGL( const VOpenGLConfig& openGLConfig );
  static VBASE_IMPEXP bool DeInitializeOpenGL();

#endif


  /// \brief
  ///   Returns the last error that occurred during any call to any VVideo method.
  /// 
  /// The error is returned as a string describing the problem that was encountered.
  /// 
  /// \return
  ///   VString: The respective error message.
  static VBASE_IMPEXP VString GetLastError();

  /// \brief
  ///   Initializes the device context.
  /// 
  /// This method has to be called in order to create a device context that can be rendered to. The
  /// settings of this device context are specified through the VVideoConfig structure passed to this
  /// method. InitializeScreen() will allocate front and back buffers, command buffer structures, and
  /// various platform-specific data structures. On Windows, it will additionally perform the following operations:
  ///
  /// \li It adjusts the screen resolution if the mode is a full-screen mode
  /// \li If the VVideoConfig structure does not contain a window handle, a window is created (and
  ///   stored in the VVideoConfig structure)
  /// \li The context configuration data (e.g. pixel format) is set, and a swap chain is
  ///   created
  /// 
  /// \param vc
  ///   Reference to a VVideoConfig structure describing the mode to be initialized
  /// 
  /// \return
  ///   bool: true if the initialization succeeded, otherwise false. Use VVideo::GetLastError to get
  ///   additional information.
  static VBASE_IMPEXP bool InitializeScreen(VVideoConfig &vc);

  /// \brief
  ///   DeInitializes the device context.
  /// 
  /// Memory allocated by InitializeScreen is freed again.
  /// 
  /// \param vc
  ///   Reference to the VVideoConfig structure that was previously used to initialize the screen.
  /// 
  /// \return
  ///   bool: true if the deinitialization succeeded, otherwise false. Use VVideo::GetLastError to get
  ///   additional information.
  static VBASE_IMPEXP bool DeInitializeScreen(VVideoConfig &vc);

  /// \brief
  ///   Restores the specified adapter's original screen mode.
  /// 
  /// The original screen mode is the screen mode that was set before the last
  /// InitializeScreen call. Only relevant on Windows PCs.
  /// 
  /// This method is called by DeInitializeScreen. It is usually not necessary to call this function directly.
  /// 
  /// \param iAdapter
  ///   Index of the adapter/device to be used
  /// 
  /// \return
  ///   bool: true if restoring succeeded, otherwise false. Use VVideo::GetLastError to get additional
  ///   information.
  static VBASE_IMPEXP bool RestoreScreen(int iAdapter=0);

  /// \internal
  /// \brief
  ///   For internal use only
  ///
  /// This is necessary since there is some weird behavior
  /// on Windows XP (DX9) when switching into fullscreen on with a different
  /// resolution then the desktop, then switching to the desktop
  /// resolution in fullscreen and finally switching back to windowed mode.
  /// 
  /// The workaround resets the screen when the application is deactivated in fullscreen.
  ///
  /// \param vc
  ///   Reference to the VVideoConfig structure that is currently used.
  /// \endinternal
  static VBASE_IMPEXP bool ResetScreen(const VVideoConfig &vc);


  /// \brief
  ///   Returns a string containing the description of the specified adapter.
  /// 
  /// On console platforms, iAdapter has to be 0.
  /// 
  /// \param iAdapter
  ///   Index of the adapter/device to be used
  /// 
  /// \return
  ///   VString: description of the specified adapter.
  static VBASE_IMPEXP VString GetAdapterDesc(int iAdapter);

  /// \brief
  ///   Changes the settings of an already initialized screen mode on the fly.
  /// 
  /// The properties of the VVideoConfig structure that are actually taken into account when
  /// switching screen modes may vary from platform to platform. Typically, m_iXRes, m_iYRes,
  /// m_bFullScreen, and m_iRefreshRate will be taken into account.
  /// 
  /// This feature is only supported on Windows PCs and Xbox360.
  ///
  /// \note
  ///   This method internally resets the device, which requires ALL graphics resources to be
  ///   destroyed. When using this low-level function instead of Vision::Video.ChangeScreenResolution(),
  ///   you need to take care of this yourself (otherwise, you will get an assertion).
  /// 
  /// \param vc
  ///   New properties for the screen mode
  /// 
  /// \return
  ///   bool: true if changing the screen settings succeeded, otherwise false. Use VVideo::GetLastError to
  ///   get additional information.
  static VBASE_IMPEXP bool ChangeScreenSettings(VVideoConfig &vc);

  /// \brief
  ///   Returns the index of the default display device. Will be zero on all devices with only one monitor.
  /// 
  /// DX9: Index of the default monitor in m_MonitorList, which is in the order determined by windows.
  /// DX11: The default monitor is always 0 as iterated by IDXGIAdapter::EnumOutputs.
  /// Always 0 on non-Windows platforms.
  ///
  /// \return
  ///   int: index of the default system adapter.
  static VBASE_IMPEXP int GetDefaultDisplayDevice();

  /// \brief
  ///   Returns the number of supported devices on the system
  /// 
  /// The devices (adapters) can later on be accessed through their index.
  /// 
  /// \return
  ///   bool: true if changing the screen settings succeeded, otherwise false. Use VVideo::GetLastError to
  ///   get additional information.
  static VBASE_IMPEXP int GetAdapterCount();

  /// \brief
  ///   Returns the number of supported display modes for a specific device.
  /// 
  /// Only adapters with the specified front buffer color depth will be counted.
  /// 
  /// Typically, supported front buffer color depths are 16 and 32 bits.
  /// 
  /// \param iAdapter
  ///   Index of the adapter/device to be used. Always 0 on non-Windows platforms.
  /// 
  /// \param iFrontBufferBPP
  ///   Front buffer color depth
  /// 
  /// \return
  ///   int: Number of supported adapter modes
  static VBASE_IMPEXP int  GetAdapterModeCount(int iAdapter, int iFrontBufferBPP);

  /// \brief
  ///   Enumerates all supported adapter modes matching the specified settings.
  /// 
  /// The adapters supported the specified device with the specified front buffer color depth will
  /// be enumerated.
  /// 
  /// Typically, supported front buffer color depths are 16 and 32 bits.
  /// 
  /// \param iAdapter
  ///   Index of the adapter/device to be used. Always 0 on non-Windows platforms.
  /// 
  /// \param iFrontBufferBPP
  ///   Front buffer color depth
  /// 
  /// \param pModes
  ///   An array of VVideoMode structures containing the supported video modes. You have to take
  ///   care of the memory handling and ensure that the array is large enough to contain all video
  ///   modes.
  /// 
  /// \return
  ///   int: Number of supported adapter modes
  static VBASE_IMPEXP int  EnumAdapterModes(int iAdapter, int iFrontBufferBPP, VVideoMode *pModes);

  /// \brief
  ///   Performs a present operation (buffer flipping or copying)
  ///
  /// In order to display the rendered content, an present operation will be performed, ie. switching
  /// front and back buffer.
  /// 
  /// The actual present operation performed (eg. flip vs. copy) is handled transparently.
  /// 
  /// \param vc
  ///   Reference to the VVideoConfig structure that was previously used to initialize the screen.
  /// 
  /// \return
  ///   bool bDeviceLost: false if the device is lost, true if this is not the case. For semantics
  ///   regarding the device lost state, please refer to the DirectX9 SDK documentation. Note that
  ///   this state can only be returned on WIN32 DX9.
  static VBASE_IMPEXP bool Present(const VVideoConfig &vc);

  /// \brief
  ///   Returns the system's default color depth
  /// 
  /// On WIN32 systems, this is the desktop color depth.
  /// 
  /// On Xbox360, this will always be 32.
  /// 
  /// On PS3, this will always be 32.
  /// 
  /// \return
  ///   int: Default color depth.
  static VBASE_IMPEXP int  GetDefaultBPP();


#if defined(WIN32) && defined(_VR_DX9)
  /// \brief
  ///   Returns the current display mode for a specific adapter
  /// 
  /// Available on Win32 only.
  /// 
  /// \param iAdapter
  ///   Index of the adapter/device to be used
  /// 
  /// \param Mode
  ///   Current Video mode (output)
  /// 
  /// \return
  ///   bool: true if operation successfully completed, false if the adapter does not support any
  ///   video modes.
  static VBASE_IMPEXP bool GetCurrentAdapterMode(int iAdapter, VVideoMode &Mode);

#endif

#if defined(WIN32) && !defined(_VISION_WINRT)

  /// \brief
  ///   Returns the current display mode for a specific device
  ///
  /// Available on Win32 only.
  /// 
  /// \param iAdapter
  ///   Index of the monitor adapter/device to be used
  /// 
  /// \return
  ///   DeviceConfig_t: Struct that contains information about the device's display mode
  static VBASE_IMPEXP DEVMODEA GetAdapterMode(int iAdapter);
#endif

  /// \brief
  ///   Indicates whether the passed texture format is supported by a specific video device. From the VVideoConfig parameter, only the adapter information is used.
  static VBASE_IMPEXP bool IsTextureFormatSupported(VTextureLoader::VTextureFormat_e eFormat, const VVideoConfig &videoConfig);

  /// \brief
  ///   Indicates whether the passed render target format is supported by a specific video device (when creating a renderable texture)
  ///
  /// \param eFormat
  ///   The texture format that should be queried for render target support.
  ///
  /// \param videoConfig
  ///   Only the adapter information inside this VVideoConfig is used.
  ///
  /// \note
  ///   If a renderable texture with bRenderTargetOnly=false is created, IsTextureFormatSupported() should also be called in order
  ///   to check whether the texture format can also be sampled.
  static VBASE_IMPEXP bool IsRenderTargetFormatSupported(VTextureLoader::VTextureFormat_e eFormat, const VVideoConfig &videoConfig);

  /// \brief
  ///   Indicates whether the passed texture format is supported by a specific video device (when creating a renderable texture)
  ///
  /// \param eFormat
  ///   The texture format that should be queried for depth stencil support.
  ///
  /// \param videoConfig
  ///   Only the adapter information inside this VVideoConfig is used.
  ///
  /// \note
  ///   If a renderable texture with bRenderTargetOnly=false is created, IsTextureFormatSupported() should also be called in order
  ///   to check whether the texture format can also be sampled.
  static VBASE_IMPEXP bool IsDepthStencilFormatSupported(VTextureLoader::VTextureFormat_e eFormat, const VVideoConfig &videoConfig);

  /// \brief
  ///   Indicates whether the passed texture format supports linear filtering by a specific video device (in the pixel shader).
  ///
  /// \param eFormat
  ///   The texture format that should be queried for linear filtering support.
  ///
  /// \param videoConfig
  ///   Only the adapter information inside this VVideoConfig is used.
  static VBASE_IMPEXP bool IsTextureFilteringSupported(VTextureLoader::VTextureFormat_e eFormat, const VVideoConfig &videoConfig);

  /// \brief
  ///   Initializes a secondary screen.
  /// 
  /// On Windows DX9/11, this method will create a new swap chain using the specified VVideoConfig
  /// settings.
  /// 
  /// This method can be used to create one or more additional windows the Vision engine can render
  /// to. The HWNDs for these windows can either be passed through the VVideoConfig structure, or a
  /// new window can be created by the engine. Please refer to the description of the VVideoConfig
  /// structure for further details.
  /// 
  /// Note that you always have to clean up all secondary screens using DeInitializeSecondaryScreen
  /// before deinitializing the primary screen.
  /// 
  /// Note that, for secondary screens, you will have to perform lost device handling manually when
  /// using DX9. When the OnEnterBackground event is triggered by the Vision engine on DX9 devices,
  /// delete all secondary screens after releasing all previously acquired secondary screen back
  /// buffers. Recreate them in the OnLeaveBackground event.
  /// 
  /// This method has no relevance on non-Windows platforms.
  /// 
  /// \param vc
  ///   configuration/screen parameters for the secondary screen.
  /// 
  /// \return
  ///   bool: true if the operation has been successful.
  static VBASE_IMPEXP bool InitializeSecondaryScreen(VVideoConfig &vc);

  /// \brief
  ///   DeInitializes a secondary screen.
  /// 
  /// On PC DX9/11, this method will remove a previously created swap chain using the specified
  /// VVideoConfig settings.
  /// 
  /// Note that InitializeSecondaryScreen stores information in the VVideoConfig object which is
  /// needed to correctly deinitialize the screen, so make sure that you always pass the same
  /// object to InitializeSecondaryScreen and DeInitializeSecondaryScreen.
  /// 
  /// Note that, for secondary screens, you will have to perform lost device handling manually.
  /// When the OnEnterBackground event is triggered by the Vision engine, delete all secondary screens
  /// after releasing all previously acquired secondary screen back buffers. Recreate them in the
  /// OnLeaveBackground event.
  /// 
  /// This method has no relevance on console platforms.
  /// 
  /// \param vc
  ///   configuration/screen parameter object for the secondary screen.
  /// 
  /// \return
  ///   bool: true if the operation has been successful.
  static VBASE_IMPEXP bool DeInitializeSecondaryScreen(VVideoConfig &vc);

  /// \brief
  ///   Returns whether the specified device supports hardware vertex processing.
  static VBASE_IMPEXP bool CheckHardwareProcessingSupport(VVideoConfig &vc);

  /// \brief
  ///   Changes the settings of an already initialized secondary screen on the fly.
  /// 
  /// This method works analogous to VVideo::ChangeScreenSettings.
  /// 
  /// Note that InitializeSecondaryScreen stores information in the VVideoConfig object which is
  /// needed to correctly change the screen settings, so make sure that you always pass the same
  /// object to InitializeSecondaryScreen and ChangeSecondaryScreenSettings.
  /// 
  /// This method has no relevance on non-Windows platforms.
  /// 
  /// \param vc
  ///   New properties for the secondary screen.
  /// 
  /// \return
  ///   bool: true if changing the screen settings succeeded, otherwise false. Use VVideo::GetLastError to
  ///   get additional information.
  static VBASE_IMPEXP bool ChangeSecondaryScreenSettings(VVideoConfig &vc);


#if defined(WIN32) && defined(_VR_DX9)

  // Internal use!
  static VBASE_IMPEXP IDirect3DSwapChain9 *GetSecondaryScreenSwapChain(VVideoConfig &vc);


  /// \brief
  ///   Returns the back buffer of the specified secondary screen
  /// 
  /// Note that calling this method internally increases the reference counter of the back buffer
  /// object, so you will have to call ReleaseSecondaryScreenBackBuffer before calling Reset,
  /// ChangeSecondaryScreenSettings, or DeInitializeSecondaryScreen. Also note that a lost device
  /// event invalidates the back buffer.
  /// 
  /// Note that InitializeSecondaryScreen stores information in the VVideoConfig object which is
  /// needed to correctly change the screen settings, so make sure that you always pass the same
  /// object to InitializeSecondaryScreen and GetSecondaryScreenBackBuffer.
  /// 
  /// Only available on PC DX9. For DX10/DX11, use GetDepthStencilView/GetRenderTargetView instead.
  /// 
  /// \param vc
  ///   VVideoConfig generated for the secondary screen.
  /// 
  /// \param iBuffer
  ///   Index of the back buffer to return.
  /// 
  /// \return
  ///   IDirect3DSurface9 *: Pointer to the back buffer surface
  static VBASE_IMPEXP IDirect3DSurface9 *GetSecondaryScreenBackBuffer(VVideoConfig &vc, int iBuffer);

  /// \brief
  ///   Releases a secondary screen back buffer previously acquired with
  ///   GetSecondaryScreenBackBuffer
  /// 
  /// \param vc
  ///   Data about the secondary screen.
  /// 
  /// \param iBuffer
  ///   Index of the back buffer to return.
  /// 
  /// \platforms
  ///   only DX9
  static VBASE_IMPEXP void ReleaseSecondaryScreenBackBuffer(VVideoConfig &vc, int iBuffer);

  /// \brief
  ///   Set to true to tell Video to prefer Direct3D9 Extended (D3D9Ex) devices if available.
  ///
  /// D3D9Ex is only available on Windows Vista and newer. It provides some additional functionality that is not supported in
  /// regular DirectX9, such as sharing resources between processes.
  /// After video initialization, you can use GetD3D9ExtendedUsage() to test whether the DirectX9 device in use is an extended device or not.
  /// 
  /// \param bPrefer
  ///   True to prefer D3D9 extended device, false to use the legacy -Windows XP compatible- device.
  /// 
  /// \platforms
  ///   only DX9
  static VBASE_IMPEXP void SetPreferD3D9ExtendedVersion(bool bPrefer) { if(bPrefer) m_iD3DExtendedVersionFlags |= ExtVersionPreferred; else m_iD3DExtendedVersionFlags &= ~ExtVersionPreferred; }

  /// \brief
  ///   Get preference of Direct3D9 extended version (true if extended version is preferred, false otherwise).
  /// 
  /// \platforms
  ///   only DX9
  static VBASE_IMPEXP bool GetPreferredD3D9ExtendedVersion() { return (m_iD3DExtendedVersionFlags & ExtVersionPreferred) != 0; }

  /// \brief
  ///   Return true if extended D3D9 device is used, false otherwise.
  /// 
  /// \platforms
  ///   only DX9
  static VBASE_IMPEXP bool GetD3D9ExtendedUsage() { return (m_iD3DExtendedVersionFlags & ExtVersionUsed) != 0; }

#endif

#if defined(_VR_DX11) || ( defined(_VR_DX9) && defined(WIN32) )

  /// \brief
  ///   Returns the D3D feature level of the current device.
  ///
  /// \return
  ///   \li D3D_FEATURE_LEVEL_9_3: DirectX9-level (always returned on DX9, regardless of the actual shader model supported
  ///       by the graphics card). Returned by DX11 on SM3.0 graphics cards.
  ///   \li D3D_FEATURE_LEVEL_10_0: DX10/11, Shader Model 4.0. Returned by DX11 on SM4.0 graphics cards.
  ///   \li D3D_FEATURE_LEVEL_10_1: DX10/11, Shader Model 4.1. Returned by DX11 on SM4.1 graphics cards.
  ///   \li D3D_FEATURE_LEVEL_11_0: DX11, Shader Model 5.0. Returned by DX11 on SM5.0 graphics cards.
#endif
#if defined(_VR_DX11)
  static VBASE_IMPEXP D3D_FEATURE_LEVEL GetDXFeatureLevel();
#elif defined(_VR_DX9) && defined(WIN32)
  static inline D3D_FEATURE_LEVEL GetDXFeatureLevel() { return D3D_FEATURE_LEVEL_9_3; }
#endif


#if defined(_VR_DX9)

  /// \brief
  ///   Returns the device's depth-stencil surface
  /// 
  /// \platforms
  ///   only DX9 and Xbox360
  static inline D3DSurface *GetDeviceDepthStencilSurface() { return s_pDeviceDepthStencilSurface; }

  /// \brief
  ///   Returns the device's color surface (back buffer)
  /// 
  /// \platforms
  ///   only DX9 and Xbox360
  static inline D3DSurface *GetDeviceColorSurface() { return s_pDeviceColorSurface; }

#endif


#if defined(_VR_DX11)

  /// \brief
  ///   Returns the device's depth-stencil view
  /// 
  /// \platforms
  ///   only DX11
  static inline D3DDepthStencilView *GetDeviceDepthStencilView() { return m_pDepthStencilView[0]; }

  /// \brief
  ///   Returns the device's color view (back buffer)
  /// 
  /// \platforms
  ///   only DX11
  static inline D3DRenderTargetView *GetDeviceColorView() { return m_pRenderTargetView[0]; }


  /// \brief
  ///   Returns the DXGI swap chain for a given VVideoConfig object.
  /// 
  /// Note that it is only valid to call this method for VVideoConfig objects which have previously
  /// been initialized using InitializeScreen or InitializeSecondaryScreen.
  /// 
  /// Only available on PC DX10/DX11.
  /// 
  /// \param vc
  ///   VVideoConfig object to return the swap chain for.
  /// 
  /// \return
  ///   D3DSwapChain *: Pointer to the D3D swap chain object.
  static VBASE_IMPEXP D3DSwapChain *GetSwapChain(const VVideoConfig &vc);

  /// \brief
  ///   Returns the DX10/DX11 depth stencil view for a given VVideoConfig object.
  /// 
  /// Note that it is only valid to call this method for VVideoConfig objects which have previously
  /// been initialized using InitializeScreen or InitializeSecondaryScreen.
  /// 
  /// Only available on PC DX10/DX11.
  /// 
  /// \param vc
  ///   VVideoConfig object to return the depth stencil view for.
  /// 
  /// \return
  ///   D3DDepthStencilView *: Pointer to the D3D depth stencil view.
  static VBASE_IMPEXP D3DDepthStencilView *GetDepthStencilView(const VVideoConfig &vc);

  /// \brief
  ///   Returns the DX10/DX11 render target view for a given VVideoConfig object.
  /// 
  /// Note that it is only valid to call this method for VVideoConfig objects which have previously
  /// been initialized using InitializeScreen or InitializeSecondaryScreen.
  /// 
  /// Only available on PC DX10/DX11.
  /// 
  /// \param vc
  ///   VVideoConfig object to return the render target view for.
  /// 
  /// \return
  ///   D3DRenderTargetView *: Pointer to the D3D render target view.
  static VBASE_IMPEXP D3DRenderTargetView *GetRenderTargetView(const VVideoConfig &vc);

  #ifndef _VISION_WINRT
    static VBASE_IMPEXP IDXGIFactory *GetDXGIFactory();
  #endif
#endif // dx11

#if defined(_VR_DX9)
  static bool ObtainDefaultRenderTargets(const VVideoConfig &vc);
#endif


#ifdef _VISION_XENON
  static inline bool CanSuspend() { return m_bHasPresentBeenCalled; }
#endif


#ifdef _VISION_PSP2
  static void FlushFragmentProcessing();
  static SceGxmNotification& NextFragmentNotification();
  static SceGxmNotification& LastFragmentNotification();
#endif

  static VBASE_IMPEXP bool m_bScreenNeedsReset; //internal use only

#ifndef _VISION_IOS
  /// \brief
  ///   Sets the minimum DPI for a device screen in order for it to be considered a high resolution screen.
  ///
  /// If a device is considered a high-resolution screen and SetAllowAutomaticUseOf2xAssets is enabled, 
  /// asset variants with the \@2x tag will be preferred.
  ///
  /// On iOS, this is handled automatically (retina vs. non-retina displays)
  ///
  /// \param iDPI
  ///   The lowest dpi rate at which a display is considered high resolution
  static VBASE_IMPEXP void SetDensityThresholdForHighResDisplay( int iDPI );

  /// \brief
  ///   Retrieve the minimum DPI for a device screen in order for it to be considered a high resolution screen.
  static VBASE_IMPEXP int GetDensityThresholdForHighResDisplay() { return m_iDensityThresholdForHighResDisplays; }
#endif

  /// \brief
  ///   Sets the whether automatic loading of high-resolution assets is allowed for high-resolution screens.
  static VBASE_IMPEXP void SetAllowAutomaticUseOf2xAssets( bool bAllow );

  /// \brief
  ///   Retrieves whether automatic loading of high-resolution assets is allowed for high-resolution screens.
  static inline bool GetAllowAutomaticUseOf2xAssets() { return m_bAllowAutomaticUseOf2xAssets; }

  /// \brief
  ///   Returns whether 2x assets should be used for the current config, i.e., whether the app
  ///   is running on a high resolution display and automatic loading of high-resolution assets is allowed.
  static inline bool GetUse2xAssetsForCurrentConfig() { return m_bUse2xAssetsForCurrentConfig; }

  /// \brief
  ///   Checks if the specified depth-stencil format is supported. If the format is not supported it will try to find a
  ///   fallback format. Returns the found format (or VTextureFormat_e::UNKNOWN in case no format could be found).
  static VBASE_IMPEXP VTextureLoader::VTextureFormat_e GetSupportedDepthStencilFormat(VTextureLoader::VTextureFormat_e eDepthStencilFormat, 
                                                                                      const VVideoConfig &videoConfig);

#if defined( HK_ANARCHY )
  /// \brief
  ///   Returns whether the Vision Anarchy splash screen is still being displayed.
  #if defined( _VR_GLES2 )
    static bool IsSplashScreenActive();
  #else   // defined( _VR_GLES2 )
    static inline bool IsSplashScreenActive() { return false; }
  #endif  // defined( _VR_GLES2 )
#endif  // defined( HK_ANARCHY )

protected:
  static VBASE_IMPEXP bool m_bRenderingIsSuspended;


#ifdef WIN32
  
  #ifndef _VISION_WINRT
    static bool CreateWindowX(VVideoConfig &vc);
    static bool DestroyWindowX(VVideoConfig &vc);

    static VBASE_IMPEXP bool RegisterWindowClass();
    static VBASE_IMPEXP bool UnregisterWindowClass();
    static VBASE_IMPEXP bool DestroyWindowFromVideoConfig(VVideoConfig &vc);

    static void SetNewWindowProperties(VVideoConfig &vc);
  #endif
  
    static void InitAdapterTable();
  
#ifdef _VR_DX9
  static bool CheckD3D9ExtendedPresence(); // True if present, false otherwise
  static bool CompareDeviceModes(DEVMODEA &devMode1, DEVMODEA &devMode2);
  static D3DFORMAT GetD3DFormat(int iBPP, bool bIsFloat, bool bIsDepthStencil, bool bUseStencil = true);
  static DynArray_cl<DISPLAY_DEVICEA> m_DeviceList;
  static DynArray_cl<MONITORINFOEXA> m_MonitorList;  ///< List is parallel with m_DeviceList!
  static DynArray_cl<DeviceConfig_t> m_DeviceConfig;
  static BOOL CALLBACK MonitorEnumCallback(HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor, LPARAM data);
  static int m_iDeviceCount;
  static unsigned int m_iD3DExtendedVersionFlags;
#elif defined(_VR_DX11)
  static DXGI_FORMAT GetDX10Format(int iBPP, bool bIsFloat, bool bIsDepthStencil, bool bUseStencil = true);
  static void ReQueryRenderBuffers( const VVideoConfig &vc);
  static void CreateRenderBuffers(const VVideoConfig &vc);
  static void DestroyRenderBuffers(const VVideoConfig &vc);
  static int GetFreeSwapChainIndex();

   #ifndef _VISION_WINRT
      static DynObjArray_cl<VSmartPtr<IDXGIAdapter> > m_AdapterList;
      static int m_iAdapterCount;
      static DynObjArray_cl<OutputConfig_t> m_OutputList;
      static int m_iOutputCount;
    #endif   
  static VString m_AdapterDescription;
#endif

#elif defined (_VISION_XENON)
  static D3DFORMAT GetD3DFormat(int iBPP, bool bIsFloat, bool bIsDepthStencil, bool bUseStencil = true);

#endif


  static VString m_szErrorString;
  static bool  m_bInitialized;
  static float m_fGammaStore[256*3];
  static int m_iStateFlags;


#ifdef WIN32

  #if defined(_VR_DX9)
    static DynArray_cl<IDirect3DSwapChain9 *>m_pSecondarySwapChains;
    static VBASE_IMPEXP D3DSurface *s_pDeviceDepthStencilSurface;
    static VBASE_IMPEXP D3DSurface *s_pDeviceColorSurface;
  #elif defined(_VR_DX11)
    static D3DDevice *m_pDevice;
    static D3DSwapChain *m_pSwapChains[MAX_SWAP_CHAINS];
    static VBASE_IMPEXP D3DDepthStencilView *m_pDepthStencilView[MAX_SWAP_CHAINS];
    static VBASE_IMPEXP D3DRenderTargetView *m_pRenderTargetView[MAX_SWAP_CHAINS];
    static D3DTexture2D* m_pDepthStencilBuffer[MAX_SWAP_CHAINS];
    static VBASE_IMPEXP IDXGIFactory* m_pDXGIFactory;
    static D3DDeviceContext *m_pImmediateContext;
    static D3D_FEATURE_LEVEL m_FeatureLevel;

	#ifdef _VISION_WINRT
	   static VWinRTConfig m_winRTConfig; // Current used state
    #endif
  #endif
  #ifndef _VISION_WINRT
    static VBASE_IMPEXP bool s_bHeadless;              ///< Create a null renderer and use dummy input devices
  #endif

#elif defined(_VISION_XENON)
  static IDirect3DTexture9 *m_pSwapTexture;
  static bool m_bHasPresentBeenCalled;
  static VBASE_IMPEXP D3DSurface *s_pDeviceDepthStencilSurface;
  static VBASE_IMPEXP D3DSurface *s_pDeviceColorSurface;



#elif defined(_VISION_PS3)

  friend class VSuspendedRendering;

  static void ResolveMultisampledBuffer();

  static VGCMDisplayConfig m_GCMDisplayConf;
  static VGcmConfig m_GcmConfig;
  static void *m_pMappedMainMemoryAddress;

  static IVGcmAllocator* m_GcmAllocator;

  static uint32_t m_CommandBufferOffset;

  static void* m_CommandBufferAddress;

  static VSuspendedRendering m_SuspendedRendering;
  static VVideoConfig *m_pVideoConfig;


  static uint32_t m_uiGcmCurrentFrame;
  static uint32_t m_uiGcmLastFlushedFrame;
  static volatile uint32_t* m_pGcmLastFinishedFrameAddress;


#elif defined(_VISION_LINUX)

  // @@@L implement
  //



#elif defined(_VISION_IOS)
  static VIOSGLES2Config m_IOSGLES2Config;

#elif defined(_VISION_PSP2)
  static VGXMConfig m_GxmConfig;
  static SceGxmContext* m_pGxmContext;
  static SceGxmShaderPatcher* m_pGxmShaderPatcher;

  static VGXMDisplayBuffer* m_pDisplayBuffers;

  static SceGxmRenderTarget* m_pMainRenderTarget;

  static unsigned int m_uiCurrentBackBuffer;
  static unsigned int m_uiCurrentFrontBuffer;
  static unsigned int m_uiGXMFrameCount;
  static SceGxmNotification g_NextFragmentNotification;
  static SceGxmNotification g_FragmentNotification;

  static SceUID patcherBufferUid;
  static SceUID patcherVertexUsseUid;
  static SceUID patcherFragmentUsseUid;
  static SceUID vdmRingBufferUid;
  static SceUID vertexRingBufferUid;
  static SceUID fragmentRingBufferUid;
  static SceUID fragmentUsseRingBufferUid;
  static void *m_pHostMem;
  static bool m_bFirstPresent;

#elif defined(_VISION_ANDROID)
  static VAndroidGLES2Config m_AndroidGLES2Config;
  static bool m_bGLViewRecreationWished;

#elif defined(_VISION_WIIU)
  static VWiiUDisplayConfig m_WiiUDisplayConfig;

  static void* m_pScanOutBuffer;

  static MEMHeapHandle m_MEM1HeapHandle;
  static u32 m_uiMEM1Size;
  static void* m_pStartOfMEM1;

  static MEMHeapHandle m_FGBucketHeapHandle;
  static u32 m_uiMEMFGSize;
  static void* m_pStartOfMEMFG;

  static GX2ContextState* m_pDefaultContextState;

  static GX2ColorBuffer m_GX2BackBuffer;
  static GX2DepthBuffer m_GX2DepthStencilBuffer;

  static void* m_pBBData;
  static void* m_pBBAuxData;
  static void* m_pDSData;
  static void* m_pDSHiZData;

  static void* m_pCommandBuffer;

  static bool m_bTVOutEnabled;

  static bool m_bDRCOutEnabled;

  static void* m_pDRCScanOutBuffer;

  static GX2ColorBuffer m_DRCColorBuffers[V_DRC_COUNT];
  static GX2DepthBuffer m_DRCDepthBuffers[V_DRC_COUNT];

  static GX2DRCMode m_configuredDRCMode;

  static bool m_bDRCConnected[V_DRC_COUNT];

  static void DRCConnectCallback(s32 channel, s32 reason);

  static u32 ForegroundAcquireCallback(void* userData);

#else 
#error "Missing Platform!"


#endif

#ifdef _VR_GLES2
  static bool m_bPVRTCSupported;
  static bool m_bETCSupported;
  static bool m_bPackedDepthStencilSupported;
  static int m_iMaxGLTextureSize;
#endif

  static VBASE_IMPEXP void DetermineUseOf2xAssetsForCurrentConfig();
#ifndef _VISION_IOS
  static VBASE_IMPEXP int m_iDensityThresholdForHighResDisplays;
#endif

  static VBASE_IMPEXP bool m_bAllowAutomaticUseOf2xAssets;
  static VBASE_IMPEXP bool m_bUse2xAssetsForCurrentConfig;
};


/// \brief
///   Helper class to ensure rendering is not suspended within the current scope. Use through the
///   ENSURE_RENDERING_ALLOWED_IN_SCOPE macro.
class VEnsureRenderingAllowedInScope
{
public:
  VBASE_IMPEXP VEnsureRenderingAllowedInScope();
  VBASE_IMPEXP ~VEnsureRenderingAllowedInScope();
  bool m_bWasSuspended;

private:
#if defined(HK_DEBUG) && defined(_VISION_PS3)
  uint64 m_uiStartSuspendTime;
#endif
};

#ifndef _VISION_WIIU // does not like ___ in names for debugging
#define ENSURE_RENDERING_ALLOWED_IN_SCOPE VEnsureRenderingAllowedInScope ___temp___EnsureRenderingAllowedInScope;
#else
#define ENSURE_RENDERING_ALLOWED_IN_SCOPE VEnsureRenderingAllowedInScope _temp_EnsureRenderingAllowedInScope;
#endif


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
