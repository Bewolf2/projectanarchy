/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/Engine.hpp> 
#include <Vision/Runtime/Engine/Application/VisionApp.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiSimpleRendererNode.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisionMobileShaderProvider.hpp>
#include <Vision/Runtime/Engine/Physics/IVisApiPhysicsModule.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

// *******************************************************************************
// *  This file is Vision's default Application Class implementation and is
// *  provided in full source code with the Vision SDK to serve as a reference
// *  for custom implementations. Note that you will have to alter class names
// *  and replace the precompiled header reference (Engine.hpp) with custom
// *  #includes in order to get this code to compile.
// *******************************************************************************

const char* s_pszAuthKey = NULL;


IVisApp_cl::IVisApp_cl(const char *pszAuthKey)
{
  m_bQuit = false;
  s_pszAuthKey = pszAuthKey;
  m_iUpdateSceneTickCount = 1;
}

IVisApp_cl::~IVisApp_cl()
{
  SetPhysicsModule(NULL);
}

//Call this to make the next call to Run to return false
//so the application will quit 
void IVisApp_cl::Quit()
{
  m_bQuit = true;
}

//Returns whether the application wants to terminate
bool IVisApp_cl::WantsToQuit()
{
#if defined(WIN32) && !defined(_VISION_WINRT)
  //VGLWantClose will be true if the user pressed Alt-F4 or the Window close button (X)
  return (m_bQuit || VGLWantClose());
#elif defined(_VISION_WIIU)
  // On the WiiU we have to check for the process switching information,
  // once this returns true the application should quit (User select "Exit Game" in the HBM)
  return m_bQuit || VWiiUCheckProcUI();
#else
  return m_bQuit;
#endif
}



void IVisApp_cl::TriggerLoadSceneStatus(int iStatus, float fPercentage, const char* pszStatus)
{
  OnLoadSceneStatus(iStatus, fPercentage, pszStatus);
  VisProgressDataObject_cl data(&Vision::Callbacks.OnProgress,iStatus,GetLoadingProgress());
  Vision::Callbacks.OnProgress.TriggerCallbacks(&data);
}


bool IVisApp_cl::SetPhysicsModule(IVisPhysicsModule_cl *pPhysicsModule)
{
  //Deinitialize current physics module
  if (m_spPhysicsModule)
  {
    m_spPhysicsModule->OnDeInitPhysics();
    m_spPhysicsModule = NULL;
  }

  bool bRes = true;

  //Initialize new physics module
  if (pPhysicsModule)
    bRes = (pPhysicsModule->OnInitPhysics()!=0);

  if (bRes)
    m_spPhysicsModule = pPhysicsModule;

  return bRes;    
}

void IVisApp_cl::SetShaderProvider(IVisShaderProvider_cl* pProvider)
{
  m_spShaderProvider = pProvider;
}

IVisShaderProvider_cl* IVisApp_cl::GetShaderProvider() const
{
  return m_spShaderProvider;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// class IVisUpdateSceneController_cl
///////////////////////////////////////////////////////////////////////////////////////////////////


IVisUpdateSceneController_cl::IVisUpdateSceneController_cl(bool bDeleteObjectUponUnref)
{
  m_bDeleteObjectUponUnref = bDeleteObjectUponUnref;
}

IVisUpdateSceneController_cl::~IVisUpdateSceneController_cl()
{
}



void IVisUpdateSceneController_cl::DeleteThis()
{
  if (m_bDeleteObjectUponUnref)
    VRefCounter::DeleteThis();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// class VFixStepSceneUpdateController
///////////////////////////////////////////////////////////////////////////////////////////////////


VFixStepSceneUpdateController::VFixStepSceneUpdateController(int iTicksPerSec, int iMaxTickCount, bool bDeleteObjectUponUnref)
  : IVisUpdateSceneController_cl(bDeleteObjectUponUnref)
{
  m_iLastUpdateTickCount = 0;
  m_iTicksPerSecond = iTicksPerSec;
  m_iMaxTickCount = iMaxTickCount;
}


void VFixStepSceneUpdateController::SetSteps(int iTicksPerSec, int iMaxTickCount)
{
  m_iTicksPerSecond = iTicksPerSec;
  m_iMaxTickCount = iMaxTickCount;
}


VFixStepSceneUpdateController::~VFixStepSceneUpdateController()
{
}

void VFixStepSceneUpdateController::Reset()
{
  m_iLastUpdateTickCount = 0;
}

int VFixStepSceneUpdateController::GetUpdateTickCount()
{
  uint64 iTimeNow = VGLGetTimer();
  const uint64 iTicksPerSec = VGLGetTimerResolution() / m_iTicksPerSecond;
  if (m_iLastUpdateTickCount==0)
    m_iLastUpdateTickCount = iTimeNow;

  VASSERT(m_iLastUpdateTickCount<=iTimeNow);

  int iCount = (int)((iTimeNow-m_iLastUpdateTickCount) / iTicksPerSec);
  m_iLastUpdateTickCount += iTicksPerSec*iCount;
  if (m_iMaxTickCount>0 && iCount>m_iMaxTickCount)
    return m_iMaxTickCount;
  return iCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// class VAppProgressStatus
///////////////////////////////////////////////////////////////////////////////////////////////////



VAppProgressStatus::VAppProgressStatus()
{
  m_pApp=NULL;
  m_iStartCtr = 0;
#ifdef HK_DEBUG_SLOW
  m_fOldPercentage = 0.f;
#endif
}

VAppProgressStatus::~VAppProgressStatus()
{
}


const bool bShowProgress = true; // FIXME

//int g_iStartTime = 0;
//VProgressStatus g_DiscardProgress;

void VAppProgressStatus::OnStart()
{
  VProgressStatus::OnStart();
  if (bShowProgress)
    m_pApp->TriggerLoadSceneStatus(VIS_API_LOADSCENESTATUS_START,0.f,GetProgressStatusString());
  m_iStartCtr++;
#ifdef HK_DEBUG_SLOW
  m_fOldPercentage = 0.f;
  //  g_iStartTime = GetTickCount(); // time debugging
#endif
}

void VAppProgressStatus::OnFinish()
{
  VProgressStatus::OnFinish();
  if (bShowProgress)
    m_pApp->TriggerLoadSceneStatus(VIS_API_LOADSCENESTATUS_FINISHED,100.f,GetProgressStatusString());
  m_iStartCtr--;
  VASSERT_MSG(m_iStartCtr>=0,"Mismatching number of OnStart/OnFinish calls");
}

void VAppProgressStatus::OnProgressChanged()
{
  VProgressStatus::OnProgressChanged();

  float fProgress = GetCurrentProgress();
#pragma warning(suppress:6239)
  if (bShowProgress && m_iStartCtr>0) // only inside a scene loading block
    m_pApp->TriggerLoadSceneStatus(VIS_API_LOADSCENESTATUS_PROGRESS,fProgress,GetProgressStatusString());

  /* For debugging purposes
  int iDepth = GetStackPos();
  char indent[128];
  for (int i=0;i<iDepth;i++) indent[i] = '_';
  indent[iDepth] = 0;
  float fTimeDiff = (float)(GetTickCount() - g_iStartTime)*0.001f;
  float fSpeed = 0.f;
  if (fTimeDiff>0.f) fSpeed = fProgress/fTimeDiff;
  Vision::Error.SystemMessage("%sLoadingProgress : %.2f at %.2fs; Speed:%.3f (%s)",indent,fProgress,fTimeDiff,fSpeed,GetProgressStatusString());
  */

#ifdef HK_DEBUG_SLOW
  if (m_iStartCtr>0)
  {
    VASSERT_MSG(fProgress>=m_fOldPercentage,"Progress is running backwards");
    m_fOldPercentage = fProgress;
  }
#endif

}

void VAppProgressStatus::OnStatusStringChanged()
{
  VProgressStatus::OnStatusStringChanged();
  m_pApp->TriggerLoadSceneStatus(VIS_API_LOADMODELSTATUS_PROGRESS,GetCurrentProgress(),GetProgressStatusString());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// class VisionApp_cl
///////////////////////////////////////////////////////////////////////////////////////////////////

VisCallback_cl VisionApp_cl::OnUpdateAnimatonBegin(VIS_PROFILE_CALLBACKS_UPDATEANIMATIONBEGIN);
VisCallback_cl VisionApp_cl::OnUpdateAnimatonFinished(VIS_PROFILE_CALLBACKS_UPDATEANIMATIONFINISHED);
VisCallback_cl VisionApp_cl::OnUpdatePhysicsFinished(VIS_PROFILE_CALLBACKS_UPDATEPHYSICSFINISHED);

VisionApp_cl::VisionApp_cl(const char *pszAuthKey) : IVisApp_cl(pszAuthKey)
{
  m_bUpdateScreen = true;
  m_LoadingProgress.m_pApp = this;
  m_bInputInitialized = false;
  m_iInitializeCount = 0;

  Vision::Game.ResetUpdatedEntitiesList();
}

VisionApp_cl::~VisionApp_cl()
{
  VASSERT(m_iInitializeCount==0);
}

////////////////////////////////////////////////////////////////////////////////////////
// Main run loop
////////////////////////////////////////////////////////////////////////////////////////


void VisionApp_cl::UpdateTimer()
{
  // update timer(s)
  IVTimer *pTimer = Vision::GetTimer();
  pTimer->Update();
}

//Update, render and display the scene
bool VisionApp_cl::Run()
{
  static bool bInsideGameLoop = false;

  // Make sure the game loop isn't executed recursively. In Windows builds, this could for example happen if a shown message box triggers
  // a repaint message. This case needs to be handled by the code calling the game loop.
  if(bInsideGameLoop)
  {
    // We can't report this error in a form that could trigger a message box, so reporting a warning and breaking the debugger is the best we can do.
    Vision::Error.Warning("VisionApp_cl::Run called recursively! This is usually caused by triggering a repaint from inside the game loop.");

#if defined(HK_DEBUG)
    VDBGBREAK;
#endif

    // Just skip the game loop - this may invoke weird behavior if the calling code expects the game loop to complete, but is better
    // than recursing
    return true;
  }

  bInsideGameLoop = true;

  //Update the scene
  m_iUpdateSceneTickCount = 1; // by default one simulation tick per loop
  if (m_spUpdateSceneController!=NULL)
    m_iUpdateSceneTickCount = m_spUpdateSceneController->GetUpdateTickCount();

  for (int i=0;i<m_iUpdateSceneTickCount;i++)
  {
    OnUpdateScene();
    if (i<m_iUpdateSceneTickCount-1) // the last one is performed after rendering
    {
      OnFinishScene();
      UpdateTimer();
    }
  }

  // update everything that has to be done once per loop rather than per simulation steps
  OnFrameUpdatePreRender();

  Vision::Profiling.Update();

  VASSERT_MSG(Vision::Renderer.GetRendererNode(0) != NULL, "No renderer node is set. This isn't supported anymore. Use a VSimpleRendererNode instead of registering the main context globally.");

  // If in debug build, perform a sanity check - no context registered with the main context should also be registered
  // with a renderer node!
#ifdef HK_DEBUG_SLOW
  static bool bContextErrorShown = false;
  if (!bContextErrorShown)
  {
    int iContextCount = Vision::Contexts.GetContextCount();
    for (int iContext = 0; iContext < iContextCount; iContext++)
    {
      for (int iRendererNode=0; iRendererNode<V_MAX_RENDERER_NODES; iRendererNode++)
      {
        IVRendererNode *pNode = Vision::Renderer.GetRendererNode(iRendererNode);
        VisRenderContext_cl* pContext = Vision::Contexts.GetContext(iContext);
        if (pNode != NULL && pNode->IsContextRegistered(Vision::Contexts.GetContext(iContext)))
        {
          Vision::Error.Warning("Context %s (%p) is registered globally AND in renderer node %s (%p). This may be intended, but it is most likely a porting issue introduced by porting from a pre-8.0 version of the Vision Engine.",
            pContext->GetName(), pContext, pNode->GetTypeId()->m_lpszClassName, pNode);
          bContextErrorShown = true;
        }
      }
    }
  }
#endif

  {
    INSERT_PERF_MARKER_SCOPE("BeginRendering");

    // Inform the renderer that we are now going to start rendering
    Vision::Renderer.BeginRendering();
    Vision::Callbacks.BeginRendering.TriggerCallbacks();
  }


  Vision::Renderer.SetCurrentRendererNode(NULL);
  VisRendererNodeDataObject_cl data(&Vision::Callbacks.OnRendererNodeSwitching, NULL);
  Vision::Callbacks.OnRendererNodeSwitching.TriggerCallbacks(&data);


  {
    INSERT_PERF_MARKER_SCOPE("PreRendererNodeContexts");
    Vision::Contexts.PerformVisibilityTests();
    Vision::Contexts.RenderContexts(-FLT_MAX, VIS_RENDERCONTEXTPRIORITY_SCENE);
  }

  {
    for (int iRendererNode=0; iRendererNode<V_MAX_RENDERER_NODES; iRendererNode++)
    {
      IVRendererNode *pNode = Vision::Renderer.GetRendererNode(iRendererNode);

      if (pNode != NULL && pNode->GetRenderingEnabled())
      {
        char buffer[192];
        sprintf(buffer, "RendererNode %d (%s)", iRendererNode, pNode->GetTypeId()->m_lpszClassName);
        INSERT_PERF_MARKER_SCOPE(buffer);

        VASSERT_MSG(pNode->IsInitialized(), "Renderer Node is registered and enabled, but not initialized");
        pNode->Execute();
      }
    }
  }

  {
    INSERT_PERF_MARKER_SCOPE("PostRendererNodeContexts");
    Vision::Renderer.SetCurrentRendererNode(NULL);
    Vision::Contexts.RenderContexts(VIS_RENDERCONTEXTPRIORITY_SCENE, FLT_MAX);
  }

  {
    INSERT_PERF_MARKER_SCOPE("EndRendering");

    // Tell the renderer that we have finished rendering
    Vision::Callbacks.EndRendering.TriggerCallbacks();
    Vision::Renderer.EndRendering();
  }

  //Finish the scene - the last tick is performed here
  if (m_iUpdateSceneTickCount>0)
    OnFinishScene();

  // update everything that has to be done once per loop rather than per simulation steps
  OnFrameUpdatePostRender();

  //Display the scene
  Vision::Callbacks.OnBeforeSwapBuffers.TriggerCallbacks();
#ifdef WIN32
  if (m_bUpdateScreen) // only supported on win32
#endif
    Vision::Video.UpdateScreen();

  if (m_iUpdateSceneTickCount>0) // same as for OnFinishScene
    UpdateTimer();

  bInsideGameLoop = false;

  return !WantsToQuit();
}


////////////////////////////////////////////////////////////////////////////////////////
// Engine init/deinit functions
////////////////////////////////////////////////////////////////////////////////////////

bool VisionApp_cl::IsInitialized() const
{
  return m_iInitializeCount>0;
}


bool VisionApp_cl::InitEngine(VisAppConfig_cl *pConfig)
{
  SetShaderProvider(new VisionShaderProvider_cl());

  // Use default application configuration if none is provided
  if (pConfig != NULL)
    m_appConfig = *pConfig;

  VASSERT(m_iInitializeCount==0);
  if (m_iInitializeCount==0)
  {
    VULPSTATUSMESSAGE_0("Initializing engine");
    ////////////////////////////////////////////////

    // Initialize engine  
    Vision::Init( &m_appConfig.m_engineConfig, s_pszAuthKey ); 

    // Initialize video
#if defined(WIN32) && !defined(_VISION_WINRT)
    if(m_appConfig.m_iInitFlags & VAPP_PEEK_ALL_MESSAGES_DEPRECATED)
      m_appConfig.m_windowConfig.SetPickAllMessage();
    Vision::Video.Init(&m_appConfig.m_windowConfig);

#elif defined(_VISION_PS3)
    *Vision::Video.GetGcmConfig() = m_appConfig.m_gcmConfig;
    Vision::Video.Init();

#else
    Vision::Video.Init();
#endif

    if ((m_appConfig.m_iInitFlags & VAPP_FULLSCREEN)!=0)
      m_appConfig.m_videoConfig.m_bFullScreen = true;

#if defined(WIN32) && defined(_VR_DX9)
    if ((m_appConfig.m_iInitFlags & VAPP_USE_NVPERFHUD)!=0)
      m_appConfig.m_videoConfig.m_bEnableNVPerfHUD = true;
#endif

    BOOL rVideoInit = Vision::Video.SetMode( m_appConfig.m_videoConfig);
    if ( !rVideoInit )
    {
      char szError[1024];
      sprintf(szError,"Unable to initialize video mode\n%s", Vision::Video.GetLastError().AsChar());
      Vision::Error.Warning (szError);

      // Keep a reference to ourself so that we don't get deleted during the engine deinit
      VSmartPtr<VisionApp_cl> keepRef = this;

      Vision::DeInit();
      Vision::Video.DeInit();
      SetShaderProvider(NULL);

      return false;
    }

    if ((m_appConfig.m_iInitFlags & VAPP_DEFER_IM_SHADER_CREATION) == 0)
    {
      // Create fixed-function shaders for immediate mode
      CreateIMShaders();
    }

    m_iInitFlags = m_appConfig.m_iInitFlags;

#ifdef WIN32
    if ((m_appConfig.m_iInitFlags & VAPP_USE_NVPERFHUD)!=0)
      m_iInitFlags &= ~VAPP_USE_DINPUT;
#endif

    //Initialize input
    if (m_appConfig.m_iInitFlags & VAPP_INIT_INPUT)
    {
      InitInput();
      //Vision::Profiling.InitInputMap();
    }

    ////////////////////////////////////////////////
    Vision::Contexts.GetMainRenderContext()->SetPriority(VIS_RENDERCONTEXTPRIORITY_DISPLAY);
    Vision::Contexts.GetMainRenderContext()->SetRenderLoop(new VisionRenderLoop_cl());

    VSimpleRendererNode* pRenderer = new VSimpleRendererNode(Vision::Contexts.GetMainRenderContext());
    pRenderer->InitializeRenderer();

    Vision::Renderer.SetRendererNode(0, pRenderer);   

    VULPSTATUSMESSAGE_0("Engine initialized");
    //Init custom data
    OnInitEngine();    
  }
  m_iInitializeCount++;
  return true;
}



void VisionApp_cl::DeInitEngine()
{
  if (m_iInitializeCount<=0)
  {
    VULPSTATUSMESSAGE_0("Too many calls to engine deinitialize");
    VASSERT(m_iInitializeCount>=0);
    return;
  }
  m_iInitializeCount--;
  if (m_iInitializeCount==0)
  {
    { // scope to hold app object valid
      IVisAppPtr spThisPtr = this; // prevent deleting this object inside this function

      VULPSTATUSMESSAGE_0("Deinitializing engine");
      OnDeInitEngine();

      //DeInit custom data
      Vision::Contexts.RemoveAllContexts();

      //DeInit input
      if (m_iInitFlags & VAPP_INIT_INPUT)
        DeInitInput();

      DeleteIMShaders();

      //DeInit engine
      Vision::DeInit();

      //DeInit video
      Vision::Video.DeInit();  

      SetShaderProvider(NULL);
    } // now this app object can be destroyed

    //Free all the plugins. Do this after Vision::DeInit to make sure entities are gone
    //Vision::Plugins.UnloadAllEnginePlugins();  //Moved to VisionExit so custom App's DLL isn't unloaded.

    VULPSTATUSMESSAGE_0("Engine Deinitialized");
  }
} 

bool VisionApp_cl::InitInput()
{
  VVERIFY_OR_RET_VAL(!m_bInputInitialized,false);

#if defined(WIN32) && !defined(_VISION_WINRT)
  int eInputModeKeyboard = VGL_DIRECTINPUT;
  if ( (m_iInitFlags & VAPP_USE_DINPUT_KEYBOARD) ==0)
    eInputModeKeyboard = VGL_WINDOWSINPUT;

  int eInputModeMouse = VGL_DIRECTINPUT;
  if ( (m_iInitFlags & VAPP_USE_DINPUT_MOUSE) ==0)
    eInputModeMouse = VGL_WINDOWSINPUT;

  VInputManagerPC::Init(eInputModeKeyboard, eInputModeMouse, (m_iInitFlags&VAPP_MOUSE_HIDECURSOR)!=0, 
    (m_iInitFlags&VAPP_MOUSE_NONEXCLUSIVE)==0,
    (m_iInitFlags&VAPP_KEYBOARD_NONEXCLUSIVE)==0);
#else
  VInputManager::Init();
#endif

  m_bInputInitialized = true;
  return true;
}

bool VisionApp_cl::DeInitInput()
{
  VVERIFY_OR_RET_VAL(m_bInputInitialized,false);

  VULPSTATUSMESSAGE_0("Deinitializing input");

  VInputManager::DeInit();

  m_bInputInitialized = false;  
  return true;

}

bool VisionApp_cl::IsInputInitialized()
{
  return m_bInputInitialized;
}

////////////////////////////////////////////////////////////////////////////////////////
// Entity creation callback
////////////////////////////////////////////////////////////////////////////////////////

//Called by the engine whenever it creates an entity
VisBaseEntity_cl *VisionApp_cl::OnCreateEntity(const char *pszClassName)
{
  VType *pType = Vision::GetTypeManager()->GetType(pszClassName);
  if (!pType)
  {
    Vision::Error.Warning("CreateEntity: Could not create entity of class %s, entity class is not registered", pszClassName);
    Vision::Error.AddReportGroupEntry(VIS_REPORTGROUPTYPE_MISSING_ENTITY_CLASS, pszClassName);
    return NULL;
  }
  VTypedObject *pInst = pType->CreateInstance();
  if (!pInst->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
  {
    VASSERT_MSG(FALSE,"Specified class is no derived from VisBaseEntity_cl");
    return NULL;
  }

  return (VisBaseEntity_cl *)pInst;
}


////////////////////////////////////////////////////////////////////////////////////////
// World loading status callback
////////////////////////////////////////////////////////////////////////////////////////

// Default implementaion initializes the physics after a new world is loaded
void VisionApp_cl::OnLoadSceneStatus(int iStatus, float fPercentage, const char* pszStatus)
{
  if (iStatus==VIS_API_LOADSCENESTATUS_FINISHED)
  {
    IVisPhysicsModule_cl *pPhysMod = GetPhysicsModule();
    if (pPhysMod)
      pPhysMod->OnNewWorldLoaded();
  }
}


VProgressStatus& VisionApp_cl::GetLoadingProgress()
{
  return m_LoadingProgress;
}


////////////////////////////////////////////////////////////////////////////////////////
// Engine init/deinit callbacks
////////////////////////////////////////////////////////////////////////////////////////


//Called AFTER the engine has been initialized
//Override this to initialize some of your engine data (like fonts)
void VisionApp_cl::OnInitEngine()
{
}



//Called BEFORE the engine gets deinitialized
//Override this to clean up some of your engine data (like fonts)
void VisionApp_cl::OnDeInitEngine()
{
}


////////////////////////////////////////////////////////////////////////////////////////
// vForge Integration Functionality
////////////////////////////////////////////////////////////////////////////////////////


BOOL VisionApp_cl::WantsToLeaveEditorPlayMode() const
{
#if defined(WIN32) && !defined(_VISION_WINRT)
  // default behaviour: the <ESC> key cancels the play-the-game mode
  //no input map used, because there is only one key in use (ESC)

  BOOL bStatus = VInputManagerPC::GetKeyboard().GetControlValue(CT_KB_ESC,0.f)>0.f;
  return bStatus;

#endif

  return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// Scene update and rendering
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void VisionApp_cl::OnRenderScene()
{
  //Render the scene. This will update the engine and call the installed render loop
  Vision::RenderSceneHelper();
}

// PROCESSING ORDER FOR SYNCHRONOUS PHYSICS:
//
// for each entity
// {
//   prethink
//   handle animations
// }
//
// perform physics
//
// for each entity
// {
//   think
//   handle notifications
// }



// PROCESSING ORDER FOR ASYNCHRONOUS PHYSICS:
//
// for each entity
// {
//   prethink
//   handle animations
// }
//
// for each entity
// {
//   think
//   handle notifications
// }
//
// perform physics (while rendering)
// 
// -> Note that this can lead to variations in behavior!


// Method that finalizes the scene. Has to be called between OnRenderScene and any game-related operations
void VisionApp_cl::OnFinishScene()
{
  Vision::Callbacks.OnFinishScene.TriggerCallbacks();
  // Fetch results of asynchronous physics simulation
  bool bAsyncPhysics;
  IVisPhysicsModule_cl *pPhysicsModule = Vision::GetApplication()->GetPhysicsModule();
  if (pPhysicsModule != NULL && pPhysicsModule->GetUseAsynchronousPhysics())
    bAsyncPhysics = true;
  else
    bAsyncPhysics = false; 
  if ( Vision::Editor.IsPlaying() && bAsyncPhysics )
  {
    FetchPhysicsResults();
  }
}

// Game loop that updates the scene
void VisionApp_cl::OnUpdateScene()
{
  VISION_PROFILE_FUNCTION(VIS_PROFILE_GAMELOOP);

  IVisSceneManager_cl *pSceneManager = Vision::GetSceneManager();
  VASSERT(pSceneManager);

  // Check whether we should use async physics
  IVisPhysicsModule_cl *pPhysicsModule = Vision::GetApplication()->GetPhysicsModule();
  bool bAsyncPhysics;
  if (pPhysicsModule != NULL && pPhysicsModule->GetUseAsynchronousPhysics())
    bAsyncPhysics = true;
  else
    bAsyncPhysics = false; 

  //Timer is not longer updated here, because it needs to be updated right after the frame flip
  float fElapsedTime = Vision::GetTimer()->GetTimeDifference();

  // Advance the scene update counter
  Vision::Game.SetUpdateSceneCount( Vision::Game.GetUpdateSceneCount() + 1 );

  Vision::Callbacks.OnUpdateSceneBegin.TriggerCallbacks();


  //Send any queued messages before we remove entities
  Vision::Game.ProcessMessageQueue();

  // Delete "dead" entities from previous frame
  {
    VISION_PROFILE_FUNCTION( VIS_PROFILE_GAMELOOP_UPDATELOOP );
    Vision::Game.FreeRemovedEntities();
  }

  // Run the pre-physics loop: statistics, prethink, events & animations
  if ( Vision::Editor.IsPlaying() )
    RunPreThink(fElapsedTime);

  //Process animation messages after processing animations
  Vision::Game.ProcessMessageQueue();

  // Run the physics simulation (if physics simulation is set to synchronous)
  if ( Vision::Editor.IsPlaying() && !bAsyncPhysics)
  {
    RunPhysics(fElapsedTime);
    FetchPhysicsResults();
  }

  // Run the post-physics loop: posthink
  if ( Vision::Editor.IsPlaying() )
    RunThink(fElapsedTime);

  // for the editor, we call the EditorThinkFunction function in every mode for every entity:
  if (Vision::Editor.IsInEditor())
  {
    const int iCount = VisBaseEntity_cl::ElementManagerGetSize();
    for (int i=0;i<iCount;i++)
    {
      VisBaseEntity_cl *pEntity = VisBaseEntity_cl::ElementManagerGet(i);
      if (pEntity)
        pEntity->EditorThinkFunction();
    }
  }

  // handle the lightsources (e.g. color animation)
  if (Vision::Editor.IsAnimatingOrPlaying())
    VisLightSource_cl::HandleAllLightSources(fElapsedTime);

  // update the core engine and module system
  RunUpdateLoop();
  Vision::Game.ResetUpdatedEntitiesList();

  // Kick off asynchronous physics simulation
  if ( Vision::Editor.IsPlaying() && bAsyncPhysics )
  {
    RunPhysics(fElapsedTime);
  }

  // Handle portal/visibility zone transitions
  VisObject3DVisData_cl::HandleAllNodeTransitions();

  //Handle render contexts
  VisRenderContext_cl::HandleAllRenderContexts(fElapsedTime);

  //Animate textures (in animate mode)
  VisTextureAnimInstance_cl::HandleAllAnims(Vision::Editor.IsAnimatingOrPlaying() ? fElapsedTime : 0.f);

  // scroll sky only when animating scene
  IVSky *pSky = Vision::World.GetActiveSky();
  if (pSky != NULL && Vision::Editor.IsAnimatingOrPlaying())
    pSky->Tick(fElapsedTime);

  Vision::Callbacks.OnUpdateSceneFinished.TriggerCallbacks();
}



void VisionApp_cl::OnFrameUpdatePreRender()
{
  // the UI timer is updated once per frame as opposed to the simulation timer
  IVTimer *pUITimer = Vision::GetUITimer();
  if (pUITimer!=Vision::GetTimer()) 
    pUITimer->Update();

  float fElapsedUITime = pUITimer->GetTimeDifference();
  IVisSceneManager_cl *pSceneManager = Vision::GetSceneManager();
  VASSERT(pSceneManager);

  // update the zone streaming
  pSceneManager->HandleZones(fElapsedUITime);

  // Update workload information of every thread (if requested)
  static float fTimeAccum = 0.f;
  fTimeAccum += fElapsedUITime;
  if (fTimeAccum>1.f)
  {
    // do this every second only to display an average
    Vision::GetThreadManager()->UpdateWorkloadTick(fTimeAccum);
    fTimeAccum = 0.f;
  }

  // trigger callback
  Vision::Callbacks.OnFrameUpdatePreRender.TriggerCallbacks(NULL);
}


void VisionApp_cl::OnFrameUpdatePostRender()
{
  float fElapsedUITime = Vision::GetUITimer()->GetTimeDifference();

  // Handle all resource managers. Performed after rendering to ensure rendering resources actually still exist...
  Vision::ResourceSystem.HandleAllResourceManager(fElapsedUITime);

  // Only update the input (and thus process the message queue) when playing the game.
  // This fixes the problem of the engine eating up vForge keyboard shortcut messages.
  // See ticket #16824 for details.
  // Do this only for PC, since on consoles skipping the update of the input leads in the vForge console-client
  // to endless key-triggering, when these keys are pressed prior to exporting/ running the scene from vForge.
#ifdef WIN32
  if (Vision::Editor.IsPlayingTheGame())
#endif
    Vision::Input.Update();

  // trigger callback
  Vision::Callbacks.OnFrameUpdatePostRender.TriggerCallbacks(NULL);
}



// Helper for OnRunGameLoop
// The update loop is responsible for deleting "dead" entities and updating the core engine's variables
void VisionApp_cl::RunUpdateLoop()
{
  VISION_PROFILE_FUNCTION( VIS_PROFILE_GAMELOOP_UPDATELOOP );

  // Delete "dead" entities from previous frame
  Vision::Game.FreeRemovedEntities();

  const VisEntityCollection_cl &updatedEntities = Vision::Game.GetUpdatedEntities();
  const int iNumEntities = updatedEntities.GetNumEntries();
  int &i = updatedEntities.m_iIterator; // use this iterator to keep counter in-sync if collection changes during loop
  for (i=0; i<iNumEntities; i++)
  {
    VisBaseEntity_cl *pEntity = updatedEntities.GetEntry(i);
    if (!pEntity)
      continue;

    VISION_PROFILE_FUNCTION(VIS_PROFILE_GAMELOOP_MODULESYSHANDLE);

    // send notifications to module system (e.g. all children like bound entities, lights, particles, ...)
    pEntity->Handle();

    // update visibility bounding box
    pEntity->UpdateVisBoundingBox();

    // update core engine variables
    if ( pEntity->IsCoreUpdateRequired() )
      pEntity->UpdateEntityChangedStatus();

    // reset the status flags
    pEntity->ResetStatusFlags();
  } 
}

// Helper for OnRunGameLoop
// Pre-physics loop: statistics, prethink, events & animations
void VisionApp_cl::RunPreThink(float fElapsedTime)
{
  {
    VISION_PROFILE_FUNCTION(VIS_PROFILE_GAMELOOP_PRETHINKFUNCTION);

    const VisEntityCollection_cl &relevantEntities = Vision::Game.GetPreThinkingEntities();
    int &i = relevantEntities.m_iIterator; // use this iterator to keep counter in-sync if collection changes during loop
    for (i=0; i<relevantEntities.GetNumEntries(); i++)
    {
      VisBaseEntity_cl *pEntity = relevantEntities.GetEntry( i );
      VASSERT(pEntity->GetPreThinkFunctionStatus());

      // entity code prethink
      pEntity->PreThinkFunction();
    }
  }

  OnUpdateAnimatonBegin.TriggerCallbacks();

  // run animated entities
  {
    VISION_PROFILE_FUNCTION(VIS_PROFILE_ANIMSYS_OVERALL);
    VISION_PROFILE_FUNCTION(VIS_PROFILE_GAMELOOP_ANIMATION);

    const VisEntityCollection_cl &relevantEntities = Vision::Game.GetAnimatedEntities();
    int &i = relevantEntities.m_iIterator; // use this iterator to keep counter in-sync if collection changes during loop
    for (i=0; i<relevantEntities.GetNumEntries(); i++)
    {
      VisBaseEntity_cl *pEntity = relevantEntities.GetEntry( i );
      VASSERT(pEntity->GetAnimConfig()!=NULL);

      //Process the skeletal and muscle animations
      //(Do animation before physics so the motion deltas are available for physics)
      pEntity->HandleAnimations(fElapsedTime);
    }
  }

  OnUpdateAnimatonFinished.TriggerCallbacks();
}


// Helper for OnRunGameLoop
// Run the physics simulation
void VisionApp_cl::RunPhysics(float fElapsedTime)
{
  VISION_PROFILE_FUNCTION( VIS_PROFILE_PHYSICS_RUNSIMULATION );
  IVisPhysicsModule_cl *pPhysicsModule = Vision::GetApplication()->GetPhysicsModule();
  if ( pPhysicsModule )
    pPhysicsModule->OnRunPhysics( fElapsedTime );
}

// Helper for OnRunGameLoop
// Fetch the physics results
void VisionApp_cl::FetchPhysicsResults()
{
  VISION_PROFILE_FUNCTION( VIS_PROFILE_PHYSICS_FETCHRESULTS );
  IVisPhysicsModule_cl *pPhysicsModule = Vision::GetApplication()->GetPhysicsModule();
  if ( pPhysicsModule )
    pPhysicsModule->FetchPhysicsResults();

  OnUpdatePhysicsFinished.TriggerCallbacks();
}


// Helper for OnRunGameLoop
// Post-physics loop: posthink & modulesystem-notifications
void VisionApp_cl::RunThink(float fElapsedTime)
{
  const VisEntityCollection_cl &relevantEntities = Vision::Game.GetThinkingEntities();

  VISION_PROFILE_FUNCTION(VIS_PROFILE_GAMELOOP_THINKFUNCTION);

  int &i = relevantEntities.m_iIterator; // use this iterator to keep counter in-sync if collection changes during loop
  for (i=0; i<relevantEntities.GetNumEntries(); i++)
  {
    VisBaseEntity_cl *pEntity = relevantEntities.GetEntry(relevantEntities.m_iIterator);
    VASSERT(pEntity->GetThinkFunctionStatus());

    // entity code postthink
    pEntity->ThinkFunction();
  }
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
