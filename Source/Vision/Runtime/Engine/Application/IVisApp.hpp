/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVisApp.hpp

#ifndef VISION_IVISAPP_HPP
#define VISION_IVISAPP_HPP


#include <Vision/Runtime/Engine/Physics/IVisApiPhysicsModule.hpp>
#include <Vision/Runtime/Engine/Application/IVisApp.hpp>

#include <Vision/Runtime/Engine/Renderer/RenderLoop/VisionRenderLoop.hpp>

#include <Vision/Runtime/Engine/SceneManagement/VisionScene.hpp>


/// \brief Flags for VisionApp_cl::InitEngine
enum VAppFlags
{
  VAPP_FULLSCREEN                   =   1,   ///< Go into full screen mode. Overrides m_videoConfig.m_bFullScreen if specified.
  VAPP_INIT_INPUT                   =   2,   ///< Initialize the input system.
  VAPP_USE_NVPERFHUD                =   4,   ///< Enables support for NVIDIA PerfHUD.
  VAPP_MOUSE_HIDECURSOR             =   8,   ///< Hide the mouse cursor for Windows Input.
  VAPP_HIDE_MOUSECURSOR             =  VAPP_MOUSE_HIDECURSOR, ///< Hide the mouse cursor for Windows Input.
  VAPP_MOUSE_NONEXCLUSIVE           =  16,   ///< Non-exclusive mouse mode. Unhides the cursor and allows you to click on other windows. Not suitable for FPS controls in a window.
  VAPP_KEYBOARD_NONEXCLUSIVE        =  32,   ///< Non-exclusive keyboard mode in DirectInput
  VAPP_USE_DINPUT_KEYBOARD          =  64,   ///< Use DirectInput for the keyboard.
  VAPP_USE_DINPUT_MOUSE             = 128,   ///< Use DirectInput for the mouse.
  VAPP_USE_DINPUT                   = VAPP_USE_DINPUT_KEYBOARD | VAPP_USE_DINPUT_MOUSE,   ///< Use DirectInput for mouse and keyboard.
  VAPP_PEEK_ALL_MESSAGES_DEPRECATED = 256,   ///< Deprecated, use VWindow::SetPickAllMessage() instead.
  VAPP_DEFER_IM_SHADER_CREATION     = 512,   ///< Don't create immediate mode shaders.
  VAPP_INIT_DEFAULTS_DEBUG          = VAPP_INIT_INPUT | VAPP_USE_DINPUT, ///< Defaults for initializing the engine in debug: Init the input and use DirectInput and show the cursor.
  VAPP_INIT_DEFAULTS_RELEASE        = VAPP_INIT_INPUT | VAPP_USE_DINPUT | VAPP_MOUSE_HIDECURSOR ///< Defaults for initializing the engine in release: Init the input and use DirectInput and hide the cursor.
};

HKV_DEPRECATED_2013_1 static const VAppFlags VAPP_PEEK_ALL_MESSAGES = VAPP_PEEK_ALL_MESSAGES_DEPRECATED; ///< Deprecated, use VWindow::SetPickAllMessage() instead.

#ifdef HK_DEBUG
  /// Defaults for initializing the engine:  Init the input and use DirectInput and show the cursor
  #define VAPP_INIT_DEFAULTS   VAPP_INIT_DEFAULTS_DEBUG
#else
  /// Defaults for initializing the engine:  Init the input and use DirectInput and hide the cursor
  #define VAPP_INIT_DEFAULTS   VAPP_INIT_DEFAULTS_RELEASE
#endif

/// \brief
///   Class that contains the settings used to initialize an instance of VisionApp_cl.
/// 
/// This class contains settings such as video mode, window procedure, engine configuration or input settings.
/// 
/// The video configuration can be accessed via the public member m_videoConfig. The engine configuration can be accessed via the
/// public member m_engineConfig.
/// 
/// \sa VisionApp_cl
class VisAppConfig_cl
{
public:

  /// \brief
  ///   Constructor that initializes the settings
  /// 
  /// \param iXRes
  ///   Horizontal screen resolution
  /// 
  /// \param iYRes
  ///   Vertical screen resolution
  /// 
  /// \param iInitFlags
  ///   A logical combination of flags listed in the VAppFlags enum.
  VisAppConfig_cl(int iXRes = VVIDEO_DEFAULTWIDTH, int iYRes = VVIDEO_DEFAULTHEIGHT, int iInitFlags = VAPP_INIT_DEFAULTS) :
    m_videoConfig(iXRes, iYRes)
  {
    m_iInitFlags = iInitFlags;
#ifdef WIN32
    m_videoConfig.m_szWindowTitle = "Vision Application";
#endif    
  }

  int m_iInitFlags;
  
#if defined(WIN32) && !defined(_VISION_WINRT)
  VWindowConfig m_windowConfig; ///< Window configuration
#endif

  VVideoConfig m_videoConfig;   ///< Video configuration
  VisConfig_t m_engineConfig;  ///< Engine configuration


#ifdef _VISION_PS3
  VGcmConfig m_gcmConfig;
#endif
};

/// \brief
///   Interface class that allows for customization of the simulation update behavior
///
/// An instance of a custom implementation can be installed via IVisApp_cl::SetSceneUpdateController to control the scene update ticks inside an application heartbeat (IVisApp_cl::Run).
/// A custom controller typically comes in pair with a custom timer implementation to return custom time steps during scene update.
/// Class VFixStepSceneUpdateController is a predefined implementation of it which allows for running simulation with a fixed tick rate (e.g. 30Hz).
/// Typical use cases are applications that require deterministic rendering through fix simulation update frequency such as multichannel applications.
class IVisUpdateSceneController_cl : public VRefCounter
{
public:
  /// \brief
  ///   Constructor
  ///
  /// \param bDeleteObjectUponUnref
  ///   if false, the instance will not be destroyed when dereferenced, so also static instances can be used
  VISION_APIFUNC IVisUpdateSceneController_cl(bool bDeleteObjectUponUnref=true);

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual  ~IVisUpdateSceneController_cl();


  /// \brief
  ///   This function is called by the engine when a new scene has been loaded
  VISION_APIFUNC virtual void Reset() = 0;

  /// \brief
  ///   This is the key function of this interface. It is called at the beginning of the VisionApp_cl::Run function to determine the number of simulation update ticks that should be executed
  ///
  /// The tick count can be 0 to skip simulation in one frame. Otherwise, for each tick, the OnUpdateScene()/OnFinishScene() functions are called, including the callbacks
  /// Vision::Callbacks.OnUpdateSceneBegin/Finished so external code can respond to each tick.
  /// This function can also be used to wait for a synchronization event to move on to the next frame, e.g. in a multichannel context.
  VISION_APIFUNC virtual int GetUpdateTickCount() = 0;

  /// \brief
  /// Overridden function called upon deletion time
  VISION_APIFUNC virtual void DeleteThis() HKV_OVERRIDE;
protected:
  bool m_bDeleteObjectUponUnref;
};

typedef VSmartPtr<IVisUpdateSceneController_cl> IVisUpdateSceneControllerPtr;


/// \brief
///   Interface class that controls the main application loop.
///
/// Besides executing the game loop, this class provides several scene update related callbacks
/// and holds references to the used physics module and shader provider.
/// 
/// The reference implementation used in Vision is the class VisionApp_cl, and it is assumed that custom implementations
/// implement their game loop such that scene and rendering updates are performed in the same order.
/// 
class IVisApp_cl : public VRefCounter
{
public:

  /// \brief
  ///   Constructor.
  /// 
  /// Does NOT initialize the engine.
  /// 
  /// \param pszAuthKey
  ///   The application authenticity key. This will be compiled in automatically.
  /// 
  /// \sa IVisApp_cl::InitEngine
  /// \sa IVisApp_cl::DeInitEngine
  VISION_APIFUNC IVisApp_cl(const char *pszAuthKey = VISION_AUTHENTICITY_KEY);
  
  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~IVisApp_cl();


  ///
  /// @name Scene Callbacks
  /// @{
  ///

  /// \brief
  ///   Renders the scene.
  /// 
  /// This is triggered by each VisRenderContext_cl when it is executed. The reference implementation
  /// calls Vision::RenderSceneHelper(), which forwards execution to the context's render loop.
  VISION_APIFUNC virtual void OnRenderScene() = 0;
  
  /// \brief
  ///   Updates the scene, i.e performs a single game loop simulation tick.
  /// 
  /// This function is responsible for updating the scene (entity thinking, animations, physics,
  /// events). In the reference implementation, it can be called multiple times per game loop, but strictly in pairs with OnFinishScene.
  VISION_APIFUNC virtual void OnUpdateScene() = 0;

  /// \brief
  ///   Finalizes the scene update.
  /// 
  /// This function is the counterpart for OnUpdateScene and has to be called for each time OnUpdateScene is called
  /// If there is only one call of OnUpdateScene per game loop, this function should be called after rendering, because asynchronous
  /// processes can read back their results here.
  VISION_APIFUNC virtual void OnFinishScene() = 0;

  /// \brief
  ///  Updates once per frame before rendering, as opposed to OnUpdateScene/OnFinishScene which can be called zero, one or more times per game loop.
  ///
  /// This is called from within the game loop in the reference implementation.
  VISION_APIFUNC virtual void OnFrameUpdatePreRender() = 0;

  /// \brief
  ///  Updates once per frame after rendering, as opposed to OnUpdateScene/OnFinishScene which can be called zero, one or more times per game loop.
  ///
  /// This is called from within the game loop in the reference implementation.
  VISION_APIFUNC virtual void OnFrameUpdatePostRender() = 0;

  /// \brief
  ///   Sets an instance of IVisUpdateSceneController_cl on this scene to customize the simulation tick handling.
  ///
  /// By default, no controller is installed. In the reference implementationthis implies that there is a 1:1 ratio between simulation update and rendering,
  /// where simulation time step is the actual time passed since the last frame. For fixed time steps, a controller of existing type VFixStepSceneUpdateController can be installed.
  ///
  /// \param pController
  ///   The update scene controller that should be used.
  inline void SetSceneUpdateController(IVisUpdateSceneController_cl* pController)
  {
    m_spUpdateSceneController = pController;
  }

  /// \brief
  ///   Returns the currently installed custom controller. By default this is NULL.
  inline IVisUpdateSceneController_cl* GetSceneUpdateController() const
  {
    return m_spUpdateSceneController;
  }


  /// \brief
  ///   Returns the number of update ticks in this current frame. This value is constant for the rest of the frame after it has been initialized through the IVisUpdateSceneController_cl interface.
  inline int GetUpdateSceneTickCount() const
  {
    return m_iUpdateSceneTickCount;
  }

  ///
  /// @}
  ///

  ///
  /// @name Scene Loading Status Callbacks
  /// @{
  ///
  
  /// \brief
  ///   Callback that the engine calls to update the progress of LoadWorld
  /// 
  /// This function also initializes the physics module after a new world is loaded, so make sure
  /// to call this function if you override it.
  /// 
  /// The incoming percentage value should be remapped using the following formula: fPercentage =
  /// (m_fProgressRangeEnd-m_fProgressRangeStart) * fPercentage + m_fProgressRangeStart*100.f;
  /// 
  /// See VisSampleApp::OnLoadSceneStatus implementation which shows a splash screen and a progress
  /// bar.
  /// 
  /// \param iStatus
  ///   is one of the following:
  ///   \li VIS_API_LOADSCENESTATUS_START : Triggered by the scene loader when a new scene starts
  ///     loading; pszStatus contains the vscene filename,
  /// 
  ///   \li VIS_API_LOADSCENESTATUS_PROGRESS : Called while loading the scene file; pszStatus might
  ///     contain some status text or NULL,
  /// 
  ///   \li VIS_API_LOADSCENESTATUS_FINISHED : Triggered by the scene loader when the scene
  ///     finished loading; pszStatus contains the vscene filename,
  ///
  /// \param fPercentage
  ///   Current status (percentage finished).
  ///
  /// \param pszStatus
  ///   Status string.
  /// 
  /// \sa VisionApp_cl::OnLoadSceneStatus
  /// \sa VisSampleApp::OnLoadSceneStatus
  virtual void OnLoadSceneStatus(int iStatus, float fPercentage, const char* pszStatus) = 0; 

  /// \brief
  ///   Helper function to call the virtual OnLoadSceneStatus function and the callback.
  VISION_APIFUNC void TriggerLoadSceneStatus(int iStatus, float fPercentage, const char* pszStatus);

  /// \brief
  ///   Overridable that must return a valid VProgressStatus object that is updated by the engine
  ///   for various loading processes. See class VProgressStatus.
  virtual VProgressStatus& GetLoadingProgress() = 0;

  ///
  /// @}
  ///

  ///
  /// @name Entity Creation Callback
  /// @{
  ///

  /// \brief
  ///   Called by the engine to create a new entity.
  /// 
  /// This method creates the entity instance. It does not prepare it for use by the engine
  /// yet. See the VisionApp_cl class implementation for a reference implementation.
  ///
  /// \param pszClassName
  ///   Class name of the entity instance to create
  ///
  /// \returns
  ///   VisBaseEntity_cl *pEntity: Entity that has been created.
  ///
  virtual VisBaseEntity_cl *OnCreateEntity(const char *pszClassName) = 0;  
  
  ///
  /// @}
  ///

  ///
  /// @name Engine Init/Deinit Callbacks
  /// @{
  ///

  /// \brief
  ///   Initializes the engine with the provided configuration
  /// 
  /// \param pConfig
  ///   Application configuration (optional) The m_appConfig member will be used if a NULL pointer
  ///   is passed here
  /// 
  /// \sa IVisApp_cl::InitEngine
  /// \sa IVisApp_cl::DeInitEngine
  virtual bool InitEngine(VisAppConfig_cl *pConfig = NULL) = 0;

  /// \brief
  ///   Deinitializes the engine
  /// 
  /// \sa IVisApp_cl::InitEngine
  /// \sa IVisApp_cl::DeInitEngine
  virtual void DeInitEngine() = 0;

  /// \brief
  ///   Returns the engine initialization status
  /// 
  /// \return
  ///   bool bInitialized: Whether the engine is initialized
  /// 
  /// \sa IVisApp_cl::InitEngine
  /// \sa IVisApp_cl::DeInitEngine
  virtual bool IsInitialized() const = 0;
  
  /// \brief
  ///   Callback that the InitEngine function calls AFTER the engine has been initialized
  /// 
  /// You can override this to initialize some of your engine data (like fonts).
  /// 
  /// Default implementation does nothing at the moment, but you have to call it when you override
  /// the function to prevent a problem in future versions.
  /// 
  /// \sa IVisApp_cl::OnInitEngine
  /// \sa IVisApp_cl::OnDeInitEngine
  virtual void OnInitEngine() = 0;
  
  /// \brief
  ///   Callback that the InitEngine function calls BEFORE the engine gets deinitialized
  /// 
  /// You can override this to clean up some of your engine data (like fonts).
  /// 
  /// Default implementation does nothing at the moment, but you have to call it when you override
  /// the function to prevent a problem in future versions.
  /// 
  /// \sa IVisApp_cl::OnInitEngine
  /// \sa IVisApp_cl::OnDeInitEngine
  virtual void OnDeInitEngine() = 0;

  ///
  /// @}
  ///

  ///
  /// @name Input Handling
  /// @{
  ///

  /// \brief
  ///   Initializes the input handler
  /// 
  /// Called by InitEngine and at various other times  (error dialog, report dialog, game editor)
  /// to  deinitialize and reinitialize the input handler.
  /// 
  /// \sa IVisApp_cl::InitEngine
  /// \sa IVisApp_cl::DeInitInput
  virtual bool InitInput() = 0;

  /// \brief
  ///   Deinitializes the input handler
  /// 
  /// Called by DeInitEngine and at various other times  (error dialog, report dialog, game editor)
  /// to deinitialize and reinitialize the input handler.
  /// 
  /// \sa IVisApp_cl::InitInput
  /// \sa IVisApp_cl::DeInitEngine
  virtual bool DeInitInput() = 0;

  /// \brief
  ///   Returns whether the input handler is initialized
  /// 
  /// \sa IVisApp_cl::InitInput
  /// \sa IVisApp_cl::DeInitInput
  virtual bool IsInputInitialized() = 0;

  
  ///
  /// @}
  ///

  ///
  /// @name Main Application Loop
  /// @{
  ///

  /// \brief
  ///   Runs one update of the main application loop.
  /// 
  /// Implementations of this function should update the Scene, render all the render contexts  and
  /// finally call Vision::Video.UpdateScreen().
  /// 
  /// You can override this to implement your own main loop.
  /// 
  /// See VisionApp_cl::Run() implementation.
  /// 
  /// \return
  ///   bool bQuit: Whether the application wants to terminate
  /// 
  /// \sa IVisApp_cl::Quit
  /// \sa IVisApp_cl::WantsToQuit
  virtual bool Run() = 0;

  /// \brief
  ///   Call this to make the next call to Run to return false so the application will quit 
  /// 
  /// Sets the protected m_bQuit member variable to true.
  /// 
  /// \sa IVisApp_cl::WantsToQuit
  /// \sa IVisApp_cl::Run
  VISION_APIFUNC void Quit();

  /// \brief
  ///   Returns whether the application wants to terminate
  /// 
  /// Either because the m_bQuit member is true or the window wants to close.
  /// 
  /// \return
  ///   bool bResult: Whether the application wants to terminate
  /// 
  /// \sa IVisApp_cl::Quit
  /// \sa IVisApp_cl::Run
  VISION_APIFUNC bool WantsToQuit();

  ///
  /// @}
  ///

  ///
  /// @name Physics Module Customization Functions
  /// @{
  ///

  /// \brief
  ///   Installs a new physics module and releases the currently installed physics module
  /// 
  /// You can deinstall the current physics module by passing NULL
  /// 
  /// Calls DeInit on the current physics module and Init on the new physics module.
  /// 
  /// \param pPhysicsModule
  ///   New module to install
  /// 
  /// \return
  ///   bool bResult : true if the physics module has been installed and initialized successfully
  /// 
  /// \note
  ///   Refer to the IVisPhysicsModule_cl interface for more information about custom physics
  ///   module
  /// 
  /// \sa IVisApp_cl::GetPhysicsModule
  /// \sa IVisPhysicsModule_cl
  VISION_APIFUNC bool SetPhysicsModule(IVisPhysicsModule_cl *pPhysicsModule);

  /// \brief
  ///   Returns the currently registered physics module
  /// 
  /// \return
  ///   IVisPhysicsModule_cl *pModule: pointer to the currently registered physics module
  /// 
  /// \note
  ///   Refer to the IVisPhysicsModule_cl interface for more information about custom physics
  ///   module.
  /// 
  /// \sa IVisApp_cl::SetPhysicsModule
  inline IVisPhysicsModule_cl* GetPhysicsModule() const
  { 
    return m_spPhysicsModule; 
  }


  ///
  /// @}
  ///

  ///
  /// @name Shader Provider
  /// @{
  ///

  /// \brief
  ///   Sets a new shader provider instance globally for this app that supplies all materials with
  ///   default lighting techniques
  /// 
  /// The default provider class (VisionShaderProvider_cl) is shipped with the VisionRenderLoop
  /// source code.
  /// 
  /// Custom shader provider implementations can change the default material lighting behavior.
  /// 
  /// For detailed information please refer to the documentation.
  /// 
  /// \param pProvider
  ///   new shader provider instance
  /// 
  /// \sa IVisShaderProvider_cl
  VISION_APIFUNC void SetShaderProvider(IVisShaderProvider_cl* pProvider);

  /// \brief
  ///   Returns the current instance of the shader provider
  VISION_APIFUNC IVisShaderProvider_cl* GetShaderProvider() const;

  ///
  /// @}
  ///


  ///
  /// @name vForge Integration Functionality
  /// @{
  ///

  /// \brief
  ///   Indicates whether vForge should abort the play-the-game mode
  /// 
  /// This function defines the cancel behavior of the vForge play-the-game mode.  vForge allows
  /// the user to switch to play mode while providing the game full control over keyboard and mouse
  /// input. This function is used by vForge to determine when the play-the-game mode should be
  /// stopped.
  /// 
  /// Please note that vForge will remain in the play-the-game mode until this function returns
  /// true. Wrong implementations can thus deadlock vForge.
  /// 
  /// \return
  ///   BOOL :  indicates whether vForge should abort the play-the-game mode
  VISION_APIFUNC virtual BOOL WantsToLeaveEditorPlayMode() const = 0;

  ///
  /// @}
  ///

public:

  // Application configuration that will be used if none is provided during InitEngine.
  // You can change the settings before calling InitEngine
  VisAppConfig_cl   m_appConfig;


protected:

  bool              m_bQuit;            ///<whether the application wants to quit
  
  IVisPhysicsModulePtr  m_spPhysicsModule;  ///<smart pointer to the installed physics module
  IVisShaderProviderPtr m_spShaderProvider; ///<smart pointer to the installed shader provider
  IVisUpdateSceneControllerPtr m_spUpdateSceneController;
  int m_iUpdateSceneTickCount;
};

typedef VSmartPtr<IVisApp_cl> IVisAppPtr;
typedef VSmartPtr<IVisSceneManager_cl> IVisSceneManagerPtr;



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
