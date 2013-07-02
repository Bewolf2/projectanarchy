/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Vision.hpp

#ifndef VISION_HPP
#define VISION_HPP

//Add all vBase headers here
#include <Vision/Runtime/Engine/System/VisionIncludes.hpp>


#define WAIT_UNTIL_FINISHED(task) \
  if (task) \
  { \
    if (task->GetState() != TASKSTATE_UNASSIGNED) \
      Vision::GetThreadManager()->WaitForTask(task, true); \
  }


#include <Vision/Runtime/Engine/System/VisApiDefs.hpp>

#include <Vision/Runtime/Engine/System/VisApiConfig.hpp>


//deprecated
#include <Vision/Runtime/Engine/Input/VisApiKey.hpp>
#include <Vision/Runtime/Engine/Input/VisApiMouse.hpp>

#include <Vision/Runtime/Engine/System/VisionConsoleManager.hpp>
#include <Vision/Runtime/Engine/Input/VisApiInput.hpp>

#include <Vision/Runtime/Engine/System/ElementManager/VisApiElementManager.hpp>
#include <Vision/Runtime/Engine/System/VisApiTypedEngineObject.hpp>
#include <Vision/Runtime/Engine/System/VisApiBase.hpp>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>
#include <Vision/Runtime/Base/Math/Quaternion/hkvQuat.h>
#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
#include <Vision/Runtime/Base/Math/BoundingVolume/hkvBoundingSphere.h>
#include <Vision/Runtime/Base/Math/Plane/hkvPlane.h>

#include <Vision/Runtime/Engine/SceneElements/VisApiLightSource.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiParticleGroup.hpp>

#include <Vision/Runtime/Engine/SceneElements/VisApiObject3D.hpp>

#include <Vision/Runtime/Engine/Renderer/Shader/VisApiShaders.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisApiShaderSet.hpp>


#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>

#include <Vision/Runtime/Engine/SceneElements/VisApiContextCamera.hpp>
#include <Vision/Runtime/Engine/Renderer/Context/VisApiRenderContext.hpp>

#include <Vision/Runtime/Engine/System/Timer/VisionTimer.hpp>
#include <Vision/Runtime/Engine/Profiling/VisApiProfiling.hpp>

#include <Vision/Runtime/Engine/Renderer/VisApiRenderer.hpp>
#include <Vision/Runtime/Engine/Renderer/Context/VisApiRenderContext.hpp>

#include <Vision/Runtime/Engine/Renderer/State/VisApiRenderStates.hpp>

#include <Vision/Runtime/Engine/SceneElements/VisApiCamera.hpp>

#include <Vision/Runtime/Engine/Renderer/Video/VisApiVideo.hpp>

#include <Vision/Runtime/Engine/System/VisApiGame.hpp>
#include <Vision/Runtime/Engine/Physics/VisApiCollisionToolkit.hpp>
#include <Vision/Runtime/Engine/Input/VisApiMouse.hpp>
#include <Vision/Runtime/Engine/Network/IVisApiNetworkManager.hpp>


#if defined (_VISION_XENON)
  #include <xbdm.h>
  #include <Vision/Runtime/Engine/RemoteComm/VisApiDCHandlerXenon.hpp>

#elif defined (_VISION_PS3)
  #include <Vision/Runtime/Engine/RemoteComm/VisApiDCHandlerPS3.hpp>

#elif defined (_VISION_PSP2)
  #include <Vision/Runtime/Engine/RemoteComm/VisApiDCHandlerPSP2.hpp>

#endif


#include <Vision/Runtime/Engine/System/Error/VisApiError.hpp>
#include <Vision/Runtime/Engine/System/VisApiCoreMain.hpp>
#include <Vision/Runtime/Engine/Renderer/Context/VisApiContext.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiDisplay.hpp>
#include <Vision/Runtime/Engine/System/VisApiCallbacks.hpp>


#include <Vision/Runtime/Engine/Renderer/Texture/VisApiRenderableTexture.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiRenderableCubeMap.hpp>
#if defined(_VR_DX11)
  #include <Vision/Runtime/Engine/Renderer/Texture/VisApiRenderableTexture2DArray.hpp>
  #include <Vision/Runtime/Engine/Renderer/Texture/VisApiRenderableTexture3D.hpp>
#endif
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiTextureManager.hpp>


#include <Vision/Runtime/Engine/System/IO/VisApiFile.hpp>

#include <Vision/Runtime/Engine/Physics/VisApiPhysics.hpp>
#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>
#include <Vision/Runtime/Engine/System/VisApiSystem.hpp>

#include <Vision/Runtime/Engine/System/Resource/VisApiResource.hpp>

#include <Vision/Runtime/Engine/SceneElements/VisApiSortedRenderCollection.hpp>

#include <Vision/Runtime/Engine/System/PluginSystem/VisApiPluginManager.hpp>
#include <Vision/Runtime/Engine/System/VisApiEditorManager.hpp>
#include <Vision/Runtime/Engine/System/VisApiBrowser.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimManager.hpp>

#include <Vision/Runtime/Engine/SceneManagement/VisionSceneManager.hpp>
#include <Vision/Runtime/Engine/Visibility/VisionVisibilityCollector.hpp>

#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiSky.hpp>
#include <Vision/Runtime/Engine/Renderer/RenderLoop/VisApiRenderLoopHelper.hpp>
#include <Vision/Runtime/Engine/System/Error/VisApiMessage.hpp>

#include <Vision/Runtime/Engine/SceneManagement/VisApiWorld.hpp>
#include <Vision/Runtime/Engine/System/VisApiFont.hpp>

#include <Vision/Runtime/Engine/Mesh/VisApiBaseMesh.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiStaticMesh.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiStaticMeshInstance.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiDynamicMesh.hpp>

#include <Vision/Runtime/Engine/SceneElements/VisApiObjectComponent.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiTriggerComponent.hpp>

#include <Vision/Runtime/Engine/Application/VisionApp.hpp>

#include <Vision/Runtime/Engine/Profiling/VisPerformanceMarker.hpp>





/// \brief
///   Static main class which is used to access the Vision Engine
/// 
/// The static Vision class is the central class in the Vision Engine and contains instances of the
/// other Vision classes and functions to initialize/deinitialize the engine. 
/// 
/// You can access the static class with Vision, e.g. to render the scene you would call the
/// Vision::RenderScene function. To access the video class you would for example use the
/// Vision::Video instance.
class Vision
{

public:

  ///
  /// @name Static Class Instance
  /// @{
  ///


  VISION_APIDATA static VisMain_cl Main;                        ///< Core engine main class
  VISION_APIDATA static VisRenderer_cl Renderer;                ///< Renderer setup
  VISION_APIDATA static VisVideo_cl Video;                      ///< Video mode setup
  VISION_APIDATA static VisRenderContextManager_cl Contexts;    ///< Rendering contexts

#ifdef SUPPORTS_KEYBOARD
  VISION_APIDATA static VisKey_cl Key;     ///< Deprecated; use VInput instead
#endif

#ifdef SUPPORTS_MOUSE
  VISION_APIDATA static VisMouse_cl Mouse; ///< Deprecated; use VInput instead
#endif


#if defined (_VISION_XENON)
  VISION_APIDATA static VisXenonDCHandler_cl XenonDCHandler;      ///< Xenon debug channel initialisation and handling
#endif



#if defined (_VISION_PS3)
  VISION_APIDATA static VisPS3DCHandler_cl PS3DCHandler;          ///< PS3 debug channel initialisation and handling
#endif



#if defined (_VISION_PSP2)
  VISION_APIDATA static VisPSP2DCHandler_cl PSP2DCHandler;        ///< PSP2 debug channel initialisation and handling
#endif



  VISION_APIDATA static VisPhysics_cl Physics;                    ///< Physics module setup
    
  VISION_APIDATA static VisCamera_cl Camera;                      ///< Camera setup
  VISION_APIDATA static VisGame_cl Game;                          ///< Game class for entity handling and general functions
  VISION_APIDATA static VisCollisionToolkit_cl CollisionToolkit;  ///< Collision toolkit class for surface manipulations and tracelines
  VISION_APIDATA static VisTextureManager_cl TextureManager;      ///< Texture manager for precaching, procedural textures and texture manipulations
  
  VISION_APIDATA static VisMessage_cl Message;                    ///< Screen messages handling
  VISION_APIDATA static VisFontResourceManager_cl Fonts;          ///< Global font manager
  VISION_APIDATA static VisError_cl Error;                        ///< Error class with errors, system messages and warnings
  VISION_APIDATA static VisProfiling_cl Profiling;                ///< Real time performance profiling  
  
  VISION_APIDATA static VisInput_cl   Input;                      ///< Input handling
  
  VISION_APIDATA static VisDisplay_cl Display;                    ///< Internal class
  VISION_APIDATA static VisConfig_t Config;                       ///< Currently valid configuration structure

  VISION_APIDATA static VisWorld_cl   World;                      ///< World related functions
  VISION_APIDATA static VisShaders_cl Shaders;                    ///< Shader setup functions
  VISION_APIDATA static VisSystem_cl  System;                     ///< System and CPU related functions
  VISION_APIDATA static VisFile_cl    File;                       ///< File related functions

  VISION_APIDATA static VisPluginManager_cl  Plugins;             ///< Plugin related functions
  VISION_APIDATA static VisEditorManager_cl  Editor;              ///< Editor related functions
  VISION_APIDATA static VisCallbackManager_cl Callbacks;          ///< Engine related callbacks
  
  VISION_APIDATA static VisRenderLoopHelper_cl RenderLoopHelper;  ///< Internal Render loop helper
  VISION_APIDATA static VisResourceSystem_cl ResourceSystem;      ///< Resource system
  VISION_APIDATA static VisAnimManager_cl Animations;             ///< Animation system

#ifdef SUPPORTS_BROWSER
  VISION_APIDATA static VisBrowserManager_cl Browser;             ///< Browser manager
#endif
  ///
  /// @}
  ///


  ///
  /// @name Initialisation / Deinitialisation
  /// @{
  ///


  /// \brief
  ///   Initialize the Vision Engine
  /// 
  /// This function initializes the Vision Engine according to the given configuration structure.
  /// The configuration structure is the main structure for setting up the engine and will be
  /// copied and set in this function.
  /// 
  /// This function has to be called before any other function of the Vision Engine can be called.
  /// 
  /// However, if you are using the engine application framework (e.g. VisionApp_cl class), calling
  /// this function is not necessary.
  /// 
  /// \param myConfig
  ///   Pointer to a configuration structure which will be copied.
  /// 
  /// \param pszAuthKey
  ///   Authenticity key.
  /// 
  /// \note
  ///   The Init function can also be used to reinitialize the engine. Calling the Init function
  ///   without calling the ReInit function does reinitialize the engine.
  /// 
  /// \sa VisionApp_cl::InitEngine
  VISION_APIFUNC static void Init(VisConfig_t *myConfig, const char *pszAuthKey);


  /// \brief
  ///   Returns whether the engine is initialized
  static inline BOOL IsInitialized() 
  {
    return m_bInitialized;
  }

  /// \brief
  ///   Indicates whether the engine is currently in the process of initialization.
  VISION_APIFUNC static BOOL IsInitializing();

  /// \brief
  ///   DEPRECATED: Call Vision::InitWorld() instead if you just want to clear and
  ///   re-init the World object. Alternatively, if you want to just De-init the 
  ///   World object (e.g. before shutting down), you can call Vision::DeInitWorld() 
  ///   instead. Calling either of these is not necessary before using a VSceneLoader
  ///   however.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC static void LoadWorld(const char *shouldBeNull)
  { ASSERT(shouldBeNull); InitWorld(); } 

  /// \brief
  ///   Prepares an empty world for loading a new scene
  /// \note
  ///   This is called automatically by VSceneLoader during a load. There is no need
  ///   to call it explicitly before loading a new scene.
  VISION_APIFUNC static void InitWorld();

  /// \brief
  ///   Unloads any loaded scene, clears out the world. Called by InitWorld, or before
  ///   shutting down the application.
  VISION_APIFUNC static void DeInitWorld();


  /// \brief
  ///   Deinitialize the Vision Engine
  /// 
  /// This function deinitializes the Vision Engine, deletes all elements (entities,
  /// light sources, etc.) and releases all the allocated memory.
  /// 
  /// However, if you are using the engine application framework (e.g. VisionApp_cl class), calling
  /// this function is not necessary.
  /// 
  /// \note
  ///   Important: Don't call Vision::Video.DeInit() before Vision::DeInit. The correct calling
  ///   order is: 
  ///   \code
  ///   Vision::DeInit(); 
  ///   Vision::Video.DeInit();
  ///   \endcode
  VISION_APIFUNC static void DeInit();
  
  /// \brief
  ///   Shuts down the engine
  /// 
  /// Cleans up the last remaining engine state and unloads all plugins
  /// (DeInitRegisteredPlugins and UnloadAllEnginePlugins)
  /// 
  /// \note
  ///   Important: This should be the last engine function you call
  VISION_APIFUNC static void Shutdown();
  

  ///
  /// @}
  ///


  ///
  /// @name Renderer Update
  /// @{
  ///


  /// \brief
  ///   Update and render the current scene
  /// 
  /// The IVisSceneManager_cl interface now handles the scene updating and rendering. This function
  /// just wraps around it for convenience and backwards compatibility.
  /// 
  /// RenderScene causes the scene to be updated (entity thinking, animations, physics, events) and
  /// rendered. 
  /// 
  /// This function should not be called if a valid world isn't loaded yet (use RenderScreenMasks
  /// instead to render any non-scene content).
  /// 
  /// The VisionApp_cl::Run function shows how you can render the scene followed by
  /// Vision::Video.UpdateScreen in your main loop to execute your application.
  /// 
  /// \sa VisionApp_cl::Run
  /// \sa VisionApp_cl::OnRenderScene
  static void RenderScene()
  {
    Vision::GetApplication()->OnUpdateScene();
    Vision::GetApplication()->OnRenderScene();
    Vision::GetApplication()->OnFinishScene();
  }



  /// \brief
  ///   Internal helper function that does all the rendering preparations and calls the installed
  ///   render loop 
  /// 
  /// Called from VisionApp_cl::OnRenderScene.
  /// 
  /// \sa VisionApp_cl::OnRenderScene
  static VISION_APIFUNC void RenderSceneHelper();
  

  /// \brief
  ///   Renders only the screen masks of the current scene.
  /// 
  /// RenderScreenMasks can be used instead of the RenderScene function if only screen masks are to
  /// be rendered. This is useful in a context where no world is loaded yet (e.g. the
  /// VisionApp_cl::OnLoadSceneStatus progress callback function). Vision::Video.UpdateScreen has
  /// to be called after this function.
  /// 
  /// However, for more control a custom renderloop can be temporarily installed that renders
  /// screenmasks or GUI elements.
  /// 
  /// \sa VisionApp_cl::OnLoadSceneStatus
  VISION_APIFUNC static void RenderScreenMasks();

  
  ///
  /// @}
  ///


  ///
  /// @name Version Information
  /// @{
  ///


  /// \brief
  ///   Get the version of the Vision Engine as a string (e.g. "WIN32 DIRECTX9 - 2013,1,0,0 - DEBUG")
  VISION_APIFUNC static const char *GetVersion();
  

  /// \brief
  ///   Get the engine's archive version
  /// 
  /// This function returns the serialization archive version supported by this engine version.
  /// All Serialize functions will serialize data which is compatible to this version.
  /// 
  /// For more information, please refer to the Serialization chapter in the Programmer's
  /// documentation.
  /// 
  /// \return
  ///   the current archive version of the Vision Engine.
  /// 
  /// \note
  ///   Always use this function for retrieving the archive version instead of directly using the
  ///   VISION_ARCHIVE_VERSION define. You might already have a newer engine version which has been
  ///   compiled with updated headers, while the headers have not been updated on your system yet.
  /// 
  /// \note
  ///   Archives with a newer version than the current archive version of the engine can't be
  ///   loaded by the engine. It is also not guaranteed that archives with an older version than
  ///   the current archive version of the engine will still be loaded correctly.
  /// 
  /// \sa Vision::IsValidArchive
  /// 
  /// \example
  ///   \code
  ///   VFile outFile( "test.ar", VFile::modeCreate | VFile::modeWrite );
  ///   VArchive outArch( &outFile, VArchive::save );
  ///   outArch << Vision::GetArchiveVersion();
  ///   \endcode
  VISION_APIFUNC static int GetArchiveVersion();


  /// \brief
  ///   Helper function to perform a version number check on the passed archive
  /// 
  /// IsValidArchive checks whether an archive is valid for loading. An archive is valid if  the
  /// loading version is set for the archive and the archive version is compatible with the current
  /// engine version.
  /// 
  /// This function has to be called in the game code after the loading version for an archive has
  /// been set and before any engine objects are loaded.
  /// 
  /// \param ar
  ///   Archive to test.
  /// 
  /// \return
  ///   \c TRUE if the archive is valid for loading.
  /// 
  /// \note
  ///   Archives with a newer version than the current archive version of the engine can't be
  ///   loaded by the engine. It is also not guaranteed that archives with an older version than
  ///   the current archive version of the engine will still be loaded correctly.
  /// 
  /// \sa Vision::GetArchiveVersion
  /// 
  /// \example
  ///   \code
  ///   int iLoadingVersion;
  ///   VFile inFile( "test.ar", VFile::modeRead );
  ///   VArchive inArch( &inFile, VArchive::load );
  ///   inArch >> iLoadingVersion;
  ///   inArch.SetLoadingVersion( iLoadingVersion );
  ///   BOOL bIsValid = Vision::IsValidArchive(inArch);
  ///   \endcode
  VISION_APIFUNC static BOOL IsValidArchive( VArchive &ar );
  

  ///
  /// @}
  ///


  ///
  /// @name Misc Functions
  /// @{
  ///


  /// \brief
  ///   Access the global instance of the thread manager.
  /// 
  /// See VThreadManager class.
  /// 
  /// \return
  ///   a pointer to the global thread manager.
  VISION_APIFUNC static VThreadManager *GetThreadManager();



#ifdef _VISION_PS3

  /// \brief
  ///   Access the global instance of the SPURS Handler on PS3.
  /// 
  /// See VSpursHandler class.
  /// 
  /// \return
  ///   VSpursHandler *: Pointer to global SPURS Handler instance.
  VISION_APIFUNC static VSpursHandler *GetSpursHandler();


  /// \brief
  ///   Sets the global instance of the SPURS Handler on PS3.
  /// 
  /// See VSpursHandler class.
  /// 
  /// Use this method if multiple libraries from the Vision SDK are to use SPURS cooperatively
  /// through a common SPURS Handler instance.
  /// 
  /// \param pSpursHandler
  ///   Pointer to global SPURS Handler instance.
  VISION_APIFUNC static void SetSpursHandler(VSpursHandler *pSpursHandler);

#endif



  /// \brief
  ///   Accesses the script manager that is currently installed. Can be NULL.
  /// 
  /// The VisionEnginePlugin implements and installs a script manager using Lua as a scripting
  /// language.
  /// 
  /// All scripting related operations in the engine go via this global instance.
  /// 
  /// \return
  ///   the script manager interface.
  /// 
  /// \sa IVScriptManager
  /// \sa IVScriptInstance
  VISION_APIFUNC static IVScriptManager *GetScriptManager();


  /// \brief
  ///   Installs an instance of the script manager and replaces the old one. Can be NULL.
  VISION_APIFUNC static void SetScriptManager(IVScriptManager *pManager);

  /// \brief
  ///   Installs an instance of the network manager and replaces the old one. Can be NULL.
  ///
  /// \param pManager
  ///   new network manager instance
  VISION_APIFUNC static void SetNetworkManager(IVNetworkManager *pManager);

  /// \brief
  ///   Returns the current network manager instance
  ///
  /// Network related functionality is called through this global manager implementation
  ///
  /// \return
  ///   current manager instance
  ///
  /// \sa IVNetworkManager
  VISION_APIFUNC static IVNetworkManager* GetNetworkManager();


  /// \brief
  ///   Access the global action manager to spawn custom actions.
  /// 
  /// The debug console uses this action manager. See class VActionManager.
  /// 
  /// \return
  ///   a pointer to the global action manager.
  VISION_APIFUNC static VActionManager *GetActionManager();


  /// \brief
  ///   Access the global type manager which is used for RTTI management in the engine.
  /// 
  /// This manager can be used to register own type modules for serialization.
  /// 
  /// \return
  ///   a pointer to the global type manager.
  VISION_APIFUNC static VTypeManager *GetTypeManager();


  /// \brief
  ///   Register a module within the global type manager
  /// 
  /// The module contains the class type information that the engine needs for serialization.
  /// 
  /// All classes inside the passed module are available inside the type manager (see
  /// GetTypeManager).
  /// 
  /// This function wraps around GetTypeManager()->RegisterModule(pModule).
  /// 
  /// Every engine plugin might have its own module that is registered at plugin initialization
  /// time.
  /// 
  /// \param pModule
  ///   Pointer to the module to register.
  /// 
  /// \return
  ///   \c TRUE if registration was successful.
  VISION_APIFUNC static BOOL RegisterModule( VModule *pModule);
  

  /// \brief
  ///   Unregister a module from the global type manager
  /// 
  /// The module contains the class type information that the engine needs for serialization.
  /// 
  /// This function wraps around GetTypeManager()->UnregisterModule(pModule).
  /// 
  /// \param pModule
  ///   Pointer to the module to unregister.
  /// 
  /// \return
  ///   \c TRUE if successful.
  VISION_APIFUNC static BOOL UnregisterModule( VModule *pModule);


  /// \brief
  ///   Returns the global VModule instance that is used by the engine. This can be used to register
  ///   types in plugins rather than creating own modules.
  VISION_APIFUNC static VModule *GetEngineModule();
  

  /// \brief
  ///   Set the global application pointer. This can be used to override some of the default engine
  ///   behavior.
  /// 
  /// The application pointer is reference counted so you don't have to free it again.
  ///
  /// \param pApp
  ///   The new application interface to set.
  VISION_APIFUNC static void SetApplication(IVisApp_cl* pApp);


  /// \brief
  ///   Returns the current global application pointer
  /// 
  /// The application is set via SetApplication.
  /// 
  /// If no application is set via the application code, this function creates and sets an instance
  /// of the VisionApp_cl class.
  inline static IVisApp_cl *GetApplication()
  {
    if (m_spApp.GetPtr() == NULL)
      m_spApp = new VisionApp_cl;
    return m_spApp.GetPtr();
  }


  /// \brief
  ///   Sets the global scene manager instance
  /// 
  /// The application holds a reference to the current global scene manager instance, so no custom
  /// cleanup is necessary.
  /// 
  /// \param pSceneManager
  ///   Pointer to the new scene manager. The engine increases its reference counter (smart
  ///   pointer).
  /// 
  /// \sa IVisSceneManager_cl
  /// \sa VisionSceneManager_cl
  VISION_APIFUNC static void SetSceneManager(IVisSceneManager_cl *pSceneManager);


  /// \brief
  ///   Returns the global scene manager instance
  /// 
  /// If no scene manager has been set at this point, the engine creates an instance of class
  /// VisionSceneManager_cl.
  /// 
  /// Thus it is always guaranteed that there is a valid scene manager when requested.
  /// 
  /// \return
  ///   the current scene manager. By default, this is an instance of class VisionSceneManager_cl.
  VISION_APIFUNC static IVisSceneManager_cl *GetSceneManager();

  /// \brief
  ///   Sets the global console manager instance
  /// 
  /// The application holds a reference to the current global console manager instance, so no custom
  /// cleanup is necessary.
  /// 
  /// \param pConsoleManager
  ///   Pointer to the new console manager. The engine increases its reference counter (smart
  ///   pointer).
  /// 
  /// \sa IVConsoleManager
  /// \sa VisionConsoleManager_cl
  VISION_APIFUNC static void SetConsoleManager(IVConsoleManager *pConsoleManager);


  /// \brief
  ///   Returns the global console manager instance
  /// 
  /// If no console manager has been set at this point, the engine creates an instance of class
  /// Default ConsoleManager.
  /// 
  /// Thus it is always guaranteed that there is a valid console manager when requested.
  /// 
  /// \return
  ///   the current console manager. By default, this is an instance of class VisionConsoleManager_cl.
  VISION_APIFUNC static IVConsoleManager *GetConsoleManager();

  /// \brief
  ///   Sets the global timer instance for the engine
  /// 
  /// With setting a custom timer object it is possible to influence all types of engine simulations, i.e. fix them 
  /// to a global simulation tick count etc.
  ///
  /// This timer is used by all simulations inside the engine and plugins. Setting a custom timer through this function
  /// will thus affect all types of simulations that are bound to application timing, including animation, physics, entity movement etc.
  /// However, some UI specific functions do use a distinct timer, that can be decoupled from the application timer. For instance, the
  /// debug console runs at independent normal speed. The UI timer can be set independently through SetUITimer.
  ///
  /// The engine manages a reference to the current global timer instance, so no custom
  /// cleanup is necessary.
  /// 
  /// \param pTimer
  ///   Pointer to the new timer instance. The engine increases its reference counter (smart pointer).
  /// 
  /// \sa IVTimer
  /// \sa SetUITimer
  VISION_APIFUNC static void SetTimer(IVTimer* pTimer);


  /// \brief
  ///   Returns the global engine timer instance
  /// 
  /// If no timer has been set at this point, the engine creates an instance of class
  /// VDefaultTimer, thus it is always guaranteed that there is a valid timer when 
  /// requested.
  ///
  /// This timer is used by all simulations inside the engine and plugins. Setting a custom timer through the SetTimer function
  /// will thus affect all types of simulations that are bound to application timing, including animation, physics, entity movement etc.
  /// However, some UI specific functions do use a distinct timer, that can be decoupled from the application timer. For instance, the
  /// debug console runs at independent normal speed. The UI timer can be set independently through SetUITimer.
  /// 
  /// \return
  ///   the current timer. By default, this is an instance of class VDefaultTimer.
  ///
  /// \sa IVTimer
  /// \sa VDefaultTimer
  /// \sa SetTimer
  /// \sa GetUITimer
  /// \sa SetUITimer
  VISION_APIFUNC static IVTimer* GetTimer();

  /// \brief
  ///   Sets a timer instance that is used for UI specific timing. NULL can be passed to reset to default class VDefaultTimer 
  VISION_APIFUNC static void SetUITimer(IVTimer* pTimer);

  /// \brief
  ///   This timer is used by sub-systems that are not bound to the simulation time, but rather need 'real' time. For instance the debug console which should not be affected by slow motion etc.
  VISION_APIFUNC static IVTimer* GetUITimer();


  ///
  /// @}
  ///

private:
  friend class VisCollisionToolkit_cl;
  friend class VisLightSource_cl;
  friend class VisConvexVolume_cl;
  friend class VisParticleGroup_cl;
  friend class VisTextureManager_cl;
  friend class VisWorld_cl;
  friend class VisWorldLoaderHelper_cl;
  friend struct SelfDeInitVisionHelper_cl;

  /// \brief
  ///   Constructor of the Vision class
  Vision();

  void RegisterActionsLIB(void);

  /// \brief
  ///   Internally updates the configuration structures
  static void setConfiguration();
  
  /// \brief
  ///   Loads all the textures in the texture queue
  static void LoadAllTextures();  

  /// \brief
  ///   Converts 5 detail steps down to 3, "rounding" down
  static VIS_CFG_Detail3Steps D5_2_D3 (VIS_CFG_Detail5Steps d5);
 
  VISION_APIDATA static BOOL m_bInitialized;         ///< TRUE if engine is already initialised

  static VActionManager m_actionManager;
  static VTypeManager m_typeManager;

  VISION_APIDATA static IVisAppPtr m_spApp;                         ///< Smart pointer to installed application
  static IVScriptManager *g_pScriptManager;
  static IVNetworkManager *g_pNetworkManager;
  static IVisSceneManagerPtr m_spSceneManager;
  static IVConsoleManagerPtr m_spConsoleManager; 
  static IVTimerPtr m_spEngineTimer; 
  static IVTimerPtr m_spUITimer; 

};

// if the linker complains that the following functions are missing
// you did link a release build against a debug version or vice versa
#ifdef HK_DEBUG_SLOW
void VISION_APIFUNC VisionInterDebugReleaseLinkingCheck();
#else
void VISION_APIFUNC VisionInterReleaseDebugLinkingCheck();
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
