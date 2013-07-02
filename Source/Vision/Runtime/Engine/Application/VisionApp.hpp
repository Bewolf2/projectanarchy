/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionApp.hpp

#ifndef VISION_VISION_APP_HPP
#define VISION_VISION_APP_HPP

#if defined( HK_ANARCHY )
  #define VISION_AUTHENTICITY_KEY      "NO_KEY_REQUIRED"
#else
  //so we can get VISION_AUTHENTICITY_KEY
  #include <Vision/Runtime/Engine/System/Authenticity/VisionAuthenticity.hpp>
#endif

#include <Vision/Runtime/Engine/Application/IVisApp.hpp>
#include <Vision/Runtime/Engine/Application/VisionAppHelpers.hpp>

/// \brief
///   Implements VProgressStatus and propagates all progress changes to the virtual
///   IVisApp_cl::OnLoadSceneStatus
class VAppProgressStatus : public VProgressStatus
{
public:
  VISION_APIFUNC VAppProgressStatus();
  VISION_APIFUNC virtual ~VAppProgressStatus();

  VISION_APIFUNC virtual void OnStart();
  VISION_APIFUNC virtual void OnFinish();
  VISION_APIFUNC virtual void OnProgressChanged();
  VISION_APIFUNC virtual void OnStatusStringChanged();

#ifdef HK_DEBUG_SLOW
  float m_fOldPercentage;
#endif
  int m_iStartCtr;
  IVisApp_cl *m_pApp;
};


/// \brief
///   A reference implementation of the IVisApp_cl interface that is used by the samples in Vision.
/// 
/// It implements the pure virtual IVisApp_cl interface and runs the engine in a standard way (e.g.
/// runs the game loop, simulates the physics and renders all contexts).
/// 
/// This class can thus be used as a starting point for custom application classes and modified
/// where necessary. The source code of this class is available in the SDK folder. Please note that
/// you need to set the application in the engine via Vision::SetApplication before running the
/// engine main loop.
/// 
/// \sa IVisApp_cl
/// \sa Vision::GetApplication()
/// \sa Vision::SetApplication()
/// 
/// \example
///   \code
///   {
///     // Create our application
///     VSmartPtr<IVisApp_cl> spApp = new VisionApp_cl();
///     // Make sure the engine knows about this application instance
///     Vision::SetApplication(spApp)
///     // Init our application with the default settings
///     if (!spApp->InitEngine())
///       return -1;
///     //Setup the data directories and load a map
///     ...
///     // Run the application until we quit
///     while (spApp->Run()) {}
///     // Unload the current scene
///     spApp->UnloadScene();
///     // Deinit the engine
///     spApp->DeInitEngine());
///     return 0; 
///   }
///   \endcode
class VisionApp_cl : public IVisApp_cl
{
public:


  ///
  /// @name Constructors / Destructor
  /// @{
  ///


  /// \brief
  ///   Creates and initializes a new Vision application
  /// 
  /// Does NOT initialize the engine.
  /// 
  /// \param pszAuthKey
  ///   Your authenticity key.
  /// 
  /// \sa VisionApp_cl::InitEngine
  /// \sa VisionApp_cl::DeInitEngine
  VISION_APIFUNC VisionApp_cl(const char *pszAuthKey = VISION_AUTHENTICITY_KEY);
  

  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisionApp_cl();


  ///
  /// @}
  ///


  ///
  /// @name Engine Init/Deinit
  /// @{
  ///
  

  /// \brief
  ///   Initializes the engine and video mode with the specified configuration
  /// 
  /// \param pConfig
  ///   Application configuration The m_appConfig member will be used if you don't specify a
  ///   configuration.
  /// 
  /// \sa VisionApp_cl::DeInitEngine
  VISION_APIFUNC virtual bool InitEngine(VisAppConfig_cl *pConfig = NULL);
  

  /// \brief
  ///   Deinitializes the engine
  /// 
  /// \sa VisionApp_cl::InitEngine
  VISION_APIFUNC virtual void DeInitEngine();
  

  /// \brief
  ///   Returns the engine initialization status
  /// 
  /// \return
  ///   bool bStatus: Whether the engine is initialized.
  /// 
  /// \sa VisionApp_cl::InitEngine
  /// \sa VisionApp_cl::DeInitEngine
  VISION_APIFUNC virtual bool IsInitialized() const;


  /// \brief
  ///   Overridable callback that the application has to call AFTER the engine has been initialized
  /// 
  /// You can override this to initialize some of your engine data (like fonts).
  /// 
  /// The default implementation does nothing at the moment, but you have to call it when you
  /// override this function to prevent a problem in future versions.
  /// 
  /// \sa VisionApp_cl::OnDeInitEngine
  VISION_APIFUNC virtual void OnInitEngine();
  

  /// \brief
  ///   Overridable callback that the application has to call BEFORE the engine gets deinitialized
  /// 
  /// You can override this to clean up some of your engine data (like fonts).
  /// 
  /// The default implementation does nothing at the moment, but you have to call it when you
  /// override this function to prevent a problem in future versions.
  /// 
  /// \sa VisionApp_cl::OnInitEngine
  VISION_APIFUNC virtual void OnDeInitEngine();


  ///
  /// @}
  ///


  ///
  /// @name Scene Callbacks
  /// @{
  ///


  /// \brief
  ///   Default render scene implementation. Just calls Vision::RenderSceneHelper().
  VISION_APIFUNC virtual void OnRenderScene();
  

  /// \brief
  ///   Default update scene implementation
  /// 
  /// This function represents one game loop tick of the application and is responsible for calling
  /// the ThinkFunctions etc.
  /// 
  /// See source code of this function.
  VISION_APIFUNC virtual void OnUpdateScene();


  /// \brief
  ///   Default finish scene implementation. Fetches physics results when in async mode and updates the device inputs.
  VISION_APIFUNC virtual void OnFinishScene();


  /// \copydoc IVisApp_cl::OnFrameUpdatePreRender
  ///  Zone handling and UI update is performed here.
  VISION_APIFUNC virtual void OnFrameUpdatePreRender();

  /// \copydoc IVisApp_cl::OnFrameUpdatePostRender
  ///  Resource manager handling is performed here.
  VISION_APIFUNC virtual void OnFrameUpdatePostRender();

  ///
  /// @}
  ///


  ///
  /// @name Input Handling
  /// @{
  ///


  /// \brief
  ///   Initializes the input handler (platform dependent: keyboard and mouse)
  /// 
  /// Called by InitEngine and various other times (error dialog, game editor) to initialize the
  /// input handler.
  /// 
  /// \sa VisionApp_cl::InitEngine
  /// \sa VisionApp_cl::DeInitInput
  VISION_APIFUNC virtual bool InitInput();


  /// \brief
  ///   Deinitializes the input handler
  /// 
  /// Called by DeInitEngine and various other times (error dialog, game editor) to deinitialize
  /// the input handler.
  /// 
  /// \sa VisionApp_cl::InitInput
  /// \sa VisionApp_cl::DeInitEngine
  VISION_APIFUNC virtual bool DeInitInput();


  /// \brief
  ///   Returns whether the input handler is initialized
  /// 
  /// \sa VisionApp_cl::InitInput
  /// \sa VisionApp_cl::DeInitInput
  VISION_APIFUNC virtual bool IsInputInitialized();


  ///
  /// @}
  ///


  ///
  /// @name Loading
  /// @{
  ///
  

  /// \brief
  ///   Overridable callback that the engine calls to update the progress of scene loading
  /// 
  /// Callback that the engine calls to update the progress of scene loading.
  /// 
  /// This function also initializes the physics module after a new world is loaded, so make sure
  /// to call this base function if you override it in a derived class. See IVisApp_cl::OnLoadSceneStatus 
  /// for full description. See VisSampleApp::OnLoadSceneStatus implementation which displays a title
  /// screen and progress bar. A more flexible way is provided through the VProgressStatus class 
  /// (see GetLoadingProgress).
  ///
  /// \param iStatus
  ///  The current status (symbolic constant).
  ///
  /// \param fPercentage
  ///  Current status percentage.
  ///
  /// \param pszStatus
  ///  Status message.
  /// 
  /// \sa IVisApp_cl::OnLoadSceneStatus
  /// \sa VisSampleApp::OnLoadSceneStatus
  VISION_APIFUNC virtual void OnLoadSceneStatus(int iStatus, float fPercentage, const char* pszStatus);


  /// \brief
  ///   Implements the pure virtual IVisApp_cl::GetLoadingProgress. It returns an object that
  ///   forwards all progress changes to OnLoadSceneStatus for backwards compatibility.
  VISION_APIFUNC virtual VProgressStatus& GetLoadingProgress();


  ///
  /// @}
  ///


  ///
  /// @name Entity Creation
  /// @{
  ///


  /// \brief
  ///   Overridable called by the engine to spawn a new entity
  /// 
  /// This only creates the entity, it does not prepare it for use by the engine yet.
  /// 
  /// The default implementation creates an entity instance via the plugin manager. If the entity
  /// class is not available, it returns a NULL pointer. This function can be overridden to fall
  /// back to VisBaseEntity_cl class if the passed class name is not available in the plug in
  /// manager.
  /// 
  /// See the WorldViewer sample that shows how you can override this function to change the
  /// default behavior.
  /// 
  /// \param pszClassName
  ///   Class name of the entity to create, e.g. "VisBaseEntity_cl".
  /// 
  /// \return
  ///   VisBaseEntity_cl *pEntity : New instance of an entity of specified class.
  VISION_APIFUNC virtual VisBaseEntity_cl *OnCreateEntity(const char *pszClassName);

  
  ///
  /// @}
  ///


  ///
  /// @name Run Loop
  /// @{
  ///


  /// \brief
  ///   Overridable run loop function
  /// 
  /// This function performs a single game tick including updating the scene and rendering.
  /// 
  /// \return
  ///   bool bQuit : If true, the application wants to quit.
  /// 
  /// \example
  ///   \code
  ///   while (pApplication->Run()) {}
  ///   \endcode
  VISION_APIFUNC virtual bool Run();
  /// \brief
  ///   Helper function to call the Update function both on the simulation timer and UI timer
  VISION_APIFUNC void UpdateTimer();

  ///
  /// @}
  ///


  ///
  /// @name Processing Renderer Nodes
  /// @{
  ///

  /// \brief
  ///   Helper function which performs visibility determination for all visibility collectors inside the passed context container.
  /// 
  /// Deprecated. Use pContainer->PerformVisibilityTests instead.
  ///
  /// \param pContainer
  ///  The container to perform visibility tests for.
   inline HKV_DEPRECATED_2013_1 VISION_APIFUNC void PerformVisibilityTestsForContextContainer(IVRenderContextContainer *pContainer)
   {
     pContainer->PerformVisibilityTests();
   }

  /// \brief
  ///   Helper function which renders a range of render contexts inside the passed context container.
  ///
  /// The parameters fMinPriority and fMaxPriority represent the range of render contexts to be rendered. Render contexts
  /// outside of this range will be ignored.
  ///
  /// \param pContainer
  ///   The render context container (e.g. IVRendererNode instance) to process.
  ///
  /// \param fMinPriority
  ///   The minimum priority value (VisRenderContext_cl::Set/GetPriority) to consider. Contexts below this priority will be ignored.
  ///
  /// \param fMaxPriority
  ///   The priority value (VisRenderContext_cl::Set/GetPriority) at and above which not to consider render contexts any more. Contexts at and above this priority will be ignored.
  ///
  /// Deprecated. Use pContainer->RenderContexts(fMinPriority, fMaxPriority) instead.
  inline HKV_DEPRECATED_2013_1 VISION_APIFUNC void RenderContextsForContextContainer(IVRenderContextContainer *pContainer, float fMinPriority, float fMaxPriority)
  {
    pContainer->RenderContexts(fMinPriority, fMaxPriority);
  }

  ///
  /// @}
  ///



  ///
  /// @name vForge Integration Functionality
  /// @{
  ///


  /// \brief
  ///   Indicates whether vForge should abort the Play-the-Game mode
  /// 
  /// Default implementation which uses the ESCAPE key to leave the play-the-game mode (see
  /// IVisApp::WantsToLeaveEditorPlayMode for more details).
  /// 
  /// Can be overridden in own application classes.
  VISION_APIFUNC virtual BOOL WantsToLeaveEditorPlayMode() const;


  /// \brief
  ///   Internal function, do not use
  inline void SetUpdateScreen(bool bStatus)
  {
    m_bUpdateScreen=bStatus;
  }

  /// \brief
  ///   This callback gets triggered by the scene implementation before the beginning of a animation update
  ///   (RunPreThink function)
  /// 
  /// For custom application implementations (classes derived from IVisApp_cl), this callback has
  /// to be triggered manually. See implementation of VisionApp_cl class.
  /// 
  /// \sa VisionApp_cl::RunPreThink()
  VISION_APIDATA static VisCallback_cl OnUpdateAnimatonBegin;

  /// \brief
  ///   This callback gets triggered by the scene implementation at the end of a animation update
  ///   (RunPreThink function)
  /// 
  /// For custom application implementations (classes derived from IVisApp_cl), this callback has
  /// to be triggered manually. See implementation of VisionApp_cl class.
  /// 
  /// \sa VisionApp_cl::RunPreThink()
  VISION_APIDATA static VisCallback_cl OnUpdateAnimatonFinished;

  /// \brief
  ///   This callback gets triggered by the scene implementation at the end of a physics update
  ///   (FetchPhysicsResults function).
  /// 
  /// The exact time when this callback is triggered depends on whether you use synchronous or asynchronous
  /// physics. If sync physics is used then the physics results are fetched (and thus this callback is triggered)
  /// just before the ThinkFunctions is called. In case of async physics the results are fetched after the rendering.
  ///
  /// For custom application implementations (classes derived from IVisApp_cl), this callback has
  /// to be triggered manually. See implementation of VisionApp_cl class.
  /// 
  /// \sa VisionApp_cl::FetchPhysicsResults()
  VISION_APIDATA static VisCallback_cl OnUpdatePhysicsFinished;

protected:

  ///
  /// @}
  ///


  ///
  /// @name Protected Helper Functions
  /// @{
  ///
  

  /// \brief
  ///   The update loop is responsible for deleting "dead" entities and updating the engine's core variables
  VISION_APIFUNC virtual void RunUpdateLoop();
 

  /// \brief
  ///   Pre-physics loop: statistics, entity prethink, events and animations
  VISION_APIFUNC virtual void RunPreThink(float fElapsedTime);
  

  /// \brief
  ///   Run the physics simulation
  VISION_APIFUNC virtual void RunPhysics(float fElapsedTime);


  /// \brief
  ///   Fetch the results from the physics simulation
  VISION_APIFUNC virtual void FetchPhysicsResults();
  

  /// \brief
  ///   Post-physics loop: Entity (post-)think & module system-notifications
  VISION_APIFUNC virtual void RunThink(float fElapsedTime);


  //////////////////////////////////////////////////////////////////
  //  Protected members
  //////////////////////////////////////////////////////////////////
  
  int   m_iInitializeCount;
  int   m_iInitFlags;                          ///< Flags that were used to initialize the application.
  
  bool  m_bInputInitialized, m_bUpdateScreen;  ///< Whether input is initialized.

  VAppProgressStatus m_LoadingProgress;

  ///
  /// @}
  ///

};


/// \brief
///   Inherits from IVisUpdateSceneController_cl and implements a scene update controller that performs a constant number of simulation ticks per second.
///
/// Based on a real timer, this class calculates the number of ticks that correspond to the time passed since the previous call of GetUpdateTickCount.
/// This class can be used to run the engine in a fix simulation step mode, which is mandatory for deterministic simulation.
/// For that feature, also the global engine timer has to be aware of the fix time step. An appropriate out-of-the-box timer class is VFixStepTimer,
/// so it makes sense that an instance of VFixStepSceneUpdateController is installed together with VFixStepTimer.
class VFixStepSceneUpdateController : public IVisUpdateSceneController_cl
{
public:
  /// \brief
  ///   Constructor. Sets the initial tick count. These counts can be modified at any time through SetSteps function
  ///
  /// \param iTicksPerSec
  ///   The number of simulation ticks per second
  ///
  /// \param iMaxTickCount
  ///   An upper limit for the returned counts in GetUpdateTickCount. A value of 0 can be passed for no upper limnit
  ///
  /// \param bDeleteObjectUponUnref
  ///   If false, this reference counted instance will not be destroyed when de-referenced.
  ///
  VISION_APIFUNC VFixStepSceneUpdateController(int iTicksPerSec=30, int iMaxTickCount=30, bool bDeleteObjectUponUnref=true);

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VFixStepSceneUpdateController();

  /// \brief
  ///   Overridden function to reset the internal timer
  VISION_APIFUNC VOVERRIDE void Reset();

  /// \brief
  ///   Returns the number of simulation ticks that have to be executed since the last call of this function.
  VISION_APIFUNC VOVERRIDE int GetUpdateTickCount();

  /// \brief
  ///   Modify the number of ticks executed per second. Same as values passed to constructor
  VISION_APIFUNC void SetSteps(int iTicksPerSec, int iMaxTickCount=0);

  int m_iMaxTickCount;
  int m_iTicksPerSecond;

  uint64 m_iLastUpdateTickCount;
};


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
