/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiCallbacks.hpp

#ifndef VISAPICALLBACKS_HPP_INCLUDED
#define VISAPICALLBACKS_HPP_INCLUDED

class VisCallback_cl;
class VisStaticMesh_cl;
class IVisVisibilityCollector_cl;
class VSoundObject;
#include <Vision/Runtime/Engine/Renderer/VisApiRendererNode.hpp>
#include <Vision/Runtime/Engine/Profiling/VisApiProfElements.hpp>
#include <Vision/Runtime/Engine/Renderer/VisApiRenderer.hpp>



/// \brief
///   Class derived from VCallback that provides profiling functionality
/// 
/// \sa class VCallback
/// \sa class VisCallbackManager_cl
/// \sa class IVisCallbackHandler_cl
class VisCallback_cl : public VCallback
{
public:

  /// \brief
  ///   Constructor
  inline VisCallback_cl(int iProfilingID=VIS_PROFILE_CALLBACKS_UNCOVERED)
  {
    m_iProfilingID = iProfilingID;
  }


  /// \brief
  ///   Calls VCallback::TriggerCallbacks and additionally profiles this function call
  /// 
  /// \param pData
  ///   The data object to send to all handlers. If NULL, this function creates a base object with
  ///   only the sender defined.
  /// 
  /// \sa VCallback::TriggerCallbacks
  /// \sa VCallback::RegisterCallback
  /// \sa VCallback::DeregisterCallback
  /// \sa class IVisCallbackHandler_cl
  /// \sa IVisCallbackHandler_cl::OnHandleCallback
  VISION_APIFUNC void TriggerCallbacks(IVisCallbackDataObject_cl *pData=NULL);

};


/// \brief
///   Helper interface to implement templated network callback functions
class IVNetworkCallback : public VisCallback_cl
{
public:
  /// \brief
  ///   Constructor
  /// \param iCallbackID
  ///   the ID identifying this callback for the purpose of network communication
  /// \param bSendLocal
  ///   Whether the callback should be triggered locally in addition to being added to the network queue.
  IVNetworkCallback(int iCallbackID, bool bSendLocal=true)
  {
    m_iCallbackID = iCallbackID;
    m_bSendLocal = bSendLocal;
  }

  /// \brief
  ///   Triggers this callback.
  ///
  /// The event that this callback has been triggered will be added to the output queue of the current
  /// network manager. Depending on the settings, the callback may additionally triggered locally.
  ///
  /// \param pData
  ///   The data object to send to all handlers. If NULL, this function creates a base object with
  ///   only the sender defined.
  ///
  /// \sa IVNetworkManager::TriggerCallbacks
  VISION_APIFUNC void TriggerCallbacks(IVisCallbackDataObject_cl *pData=NULL);

  bool m_bSendLocal; ///< Whether the callback should be triggered locally in addition to being added to the network queue.
};

/// \brief
///   Template helper class for conveniently implementing callbacks that can be triggered over the network
template <class DataObjClass> class VNetworkCallback : public IVNetworkCallback
{
public:
  VNetworkCallback(int iCallbackID, bool bSendLocal=true) : 
    IVNetworkCallback(iCallbackID,bSendLocal), m_OwnSerializationDataObj()
  {
    m_pSerializationObject = &m_OwnSerializationDataObj;
  }

  DataObjClass m_OwnSerializationDataObj;
};


/// \brief
///   Derived callback data object class that is used for the OnProgress callback function
/// 
/// The class members match the parameters passed to the IVisApp_cl::OnLoadSceneStatus function.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnProgress
/// \sa IVisApp_cl::OnLoadSceneStatus
class VisProgressDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance, flags, progress percentage and a
  ///   message string
  VisProgressDataObject_cl(VisCallback_cl *pSender, int iFlags, VProgressStatus &progress) 
    : IVisCallbackDataObject_cl(pSender), m_Progress(progress)
  {
    m_iStatusFlags = iFlags;
    m_fPercentage = progress.GetCurrentProgress();
    m_szMessage = progress.GetProgressStatusString();
  }
  
  /// \brief
  ///   The progress status object
  VProgressStatus &m_Progress;

  /// \brief
  ///   Member that describes a bitmask for the current loadworld status.
  ///   (VIS_API_LOADSCENESTATUS_START/VIS_API_LOADSCENESTATUS_PROGRESS/VIS_API_LOADSCENESTATUS_FINISHED)
  int m_iStatusFlags;

  /// \brief
  ///   Member that specifies the current progress in percent. Can also be retrieved via m_Progress.GetCurrentProgress()
  float m_fPercentage;

  /// \brief
  ///   String pointer for additional information about the current status. Can also be retrieved via m_Progress.GetProgressStatusString()
  const char *m_szMessage;
};


/// \brief
///   Derived callback data object class that is used for the OnNewModelLoaded callback function.
/// 
/// The m_pModel member is the pointer to the model that has been loaded.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnNewModelLoaded
class VDynamicMeshLoadedDataObject : public IVisCallbackDataObject_cl
{
public:
  
  /// \brief
  ///   Constructor that takes the sender callback instance and a model pointer
  VDynamicMeshLoadedDataObject(VisCallback_cl *pSender, VDynamicMesh *pMesh) : IVisCallbackDataObject_cl(pSender)
  {
    m_pNewMesh = pMesh;
  }

  VDynamicMesh *m_pNewMesh;
};


/// \brief
///   Derived callback data object class that is used for the OnNewMeshFileLoaded callback function.
/// 
/// The m_pNewMesh member is the pointer to the mesh that has been loaded.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnNewMeshFileLoaded
class VisMeshFileLoadedDataObject_cl : public IVisCallbackDataObject_cl
{
public:
  
  /// \brief
  ///   Constructor that takes the sender callback instance and a model pointer
  VisMeshFileLoadedDataObject_cl(VisCallback_cl *pSender, VisStaticMesh_cl *pMesh) : IVisCallbackDataObject_cl(pSender)
  {
    m_pNewMesh = pMesh;
  }

  VisStaticMesh_cl *m_pNewMesh;
};


/// \brief
///   Derived callback data object class that is used for the OnRenderHook callback function
/// 
/// The m_iEntryConst member describes the current position within the renderloop and is one of the
/// VRH_PRE_RENDERING, VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY, ... constants.
/// 
/// The callback is triggered directly after rendering the respective batch of mesh buffer
/// instances with the same order constant. See VisMeshBufferObject_cl::SetOrder.
/// 
/// Custom render loops must trigger this callback (see VisionRenderLoop_cl implementation).
/// 
/// This mechanism allows plugins to hook into an existing render loop and render custom geometry.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnRenderHook
/// \sa VisMeshBufferObject_cl::SetOrder
class VisRenderHookDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance and a render hook constant
  VisRenderHookDataObject_cl(VisCallback_cl *pSender, unsigned int iEntryConst) : IVisCallbackDataObject_cl(pSender)
  {
    m_iEntryConst = iEntryConst;
  }
  

  /// \brief
  ///   The render hook constant that defines the current position inside the render loop.
  /// 
  /// Can be any enum value of VRenderHook_e
  /// 
  /// See also VisMeshBufferObject_cl::SetOrder for a list of constants.
  unsigned int m_iEntryConst;
};

/// \brief
///   Derived callback data object class that is used for the OnVideoChanged callback function
/// 
/// This data object provides a pointer to the new video config structure (class VVideoConfig).
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnVideoChanged
/// \sa class VisVideo_cl
class VisVideoChangedDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance and a new VVideoConfig pointer
  VisVideoChangedDataObject_cl(VisCallback_cl *pSender, VVideoConfig *pConfig) : IVisCallbackDataObject_cl(pSender)
  {
    m_pConfig = pConfig;
  }
  
  /// \brief
  ///   Pointer to the new VVideoConfig instance

  VVideoConfig *m_pConfig;
};




/// \brief
///   Derived callback data object class that is used for the
///   OnBeforeSceneLoaded/OnAfterSceneLoaded callback functions
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnBeforeSceneLoaded
/// \sa VisCallbackManager_cl::OnAfterSceneLoaded
class VisSceneLoadedDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance and the filename of the loaded scene as input
  VisSceneLoadedDataObject_cl(VisCallback_cl *pSender, const char *szFilename) : IVisCallbackDataObject_cl(pSender)
  {
    m_szSceneFileName = szFilename;
  }
  
  /// \brief
  ///   Filename of the loaded scene
  const char *m_szSceneFileName;
};



/// \brief
///   Derived callback data object class that is used for the
///   OnBeforeSceneExported callback functions
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnBeforeSceneExported
class VisBeforeSceneExportedObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance and the filename of the target scene file
  VisBeforeSceneExportedObject_cl(VisCallback_cl *pSender, const char *szExportPath) : IVisCallbackDataObject_cl(pSender)
  {
    m_szExportPath = szExportPath;
    m_bCancelExport = false;
  }

  /// \brief
  ///   Target path for the exported scene file
  const char *m_szExportPath;

  /// \brief
  ///   [out] Set this value to TRUE in order to cancel the export
  bool m_bCancelExport;

  /// \brief
  ///   [out] Contains the error message to be displayed in case the export is canceled via m_bCancelExport
  VString m_sErrMsg;
};



/// \brief
///   Derived callback data object class that is used for the OnRendererNodeSwitching callback function
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnRendererNodeSwitching
class VisRendererNodeDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance and the renderer node which will now be activated as input
  VisRendererNodeDataObject_cl(VisCallback_cl *pSender, IVRendererNode *pRendererNode = NULL) : IVisCallbackDataObject_cl(pSender)
  {
    m_pRendererNode = pRendererNode;
  }

  IVRendererNode *m_pRendererNode;  ///< Renderer node that will be handled after the callback.
};



/// \brief
///   Data object that gets triggered by the OnContextSwitching function
/// 
/// This class provides a pointer member to the next context to be rendered as well as the renderer node it is part of.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnContextSwitching
class VisContextDataObject_cl : public IVisCallbackDataObject_cl
{
public:
  
  /// \brief
  ///   Constructor that takes the sender callback instance, the context that is to be rendered, and the renderer node it belongs to as input
  VisContextDataObject_cl(VisCallback_cl *pSender, VisRenderContext_cl *pContext, IVRendererNode *pRendererNode = NULL) : IVisCallbackDataObject_cl(pSender)
  {
    m_pRendererNode = pRendererNode;
    m_pContext = pContext;
    m_bSkipRendering = false;
  }

  IVRendererNode *m_pRendererNode;      ///< renderer node the context belongs to (note that contexts may be assigned to multiple renderer nodes)
  VisRenderContext_cl *m_pContext;      ///< context that is to be rendered
  bool m_bSkipRendering;                ///< if the callback sets this variable to true, rendering of the context is skipped within the current renderer node.
};


/// \brief
///   Derived callback data object class that is used for the OnRendererNodeChanged callback function
/// 
/// OnRendererNodeChanged is called whenever a new renderer node is registered (through Vision::Renderer.SetRendererNode),
/// or a renderer node is deregistered (by setting NULL to the specific renderer node index)
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnRendererNodeChanged
class VisRendererNodeChangedDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the sender callback instance as input. pRemovedNode must contain the renderer node that has been
  ///   deregistered (or NULL), pAddedNode must contain the renderer node that is registered.
  VisRendererNodeChangedDataObject_cl(VisCallback_cl *pSender, IVRendererNode *pRemovedNode, IVRendererNode *pAddedNode, unsigned int uiNodeIndex) : IVisCallbackDataObject_cl(pSender)
  {
    m_spRemovedNode = pRemovedNode;
    m_spAddedNode = pAddedNode;
    m_uiNodeIndex = uiNodeIndex;
  }

  IVRendererNodePtr m_spRemovedNode;      ///< the removed renderer node
  IVRendererNodePtr m_spAddedNode;        ///< the added renderer node
  unsigned int m_uiNodeIndex;              ///< the index at which the render node changed
};


/// \brief
///   Callback data object that provides a list of global render properties that changed (one bit representing one property)
class VisGlobalRendererSettingsDataObject_cl : public IVisCallbackDataObject_cl
{
public:
  inline VisGlobalRendererSettingsDataObject_cl(VisCallback_cl *pSender, VGlobalRenderPropertyFlags eChanged)
  {
    m_eChangedFlags = eChanged;
  }

  VGlobalRenderPropertyFlags m_eChangedFlags;
};


/// \brief
///   Data object that provides a pointer to a visibility collector (IVisVisibilityCollector_cl *)
class VisVisibilityCollectorDataObject_cl : public IVisCallbackDataObject_cl
{
public:
  inline VisVisibilityCollectorDataObject_cl(VisCallback_cl *pSender, IVisVisibilityCollector_cl *pCollector) : IVisCallbackDataObject_cl(pSender)
  {
    m_pCollector = pCollector;
  }

  IVisVisibilityCollector_cl *m_pCollector;
};


/// \brief
///   Derived callback data object class that is used when a console command is executed
/// 
/// \sa class IVisCallbackHandler_cl
class VisConsoleDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor that takes the console command
  VisConsoleDataObject_cl(const char* pszCommand) : IVisCallbackDataObject_cl(NULL)
  {
    m_pszCommand = pszCommand;
    m_bHandled = false;
  }
  
  /// \brief
  ///   The console command that is to be executed
  const char* m_pszCommand;
  
  /// \brief
  ///   Check this to see if another callback has already handled the console command Set this to
  ///   true if your callback handles the console command so it is only executed once
  bool m_bHandled;

};


/// \brief
///   Derived callback data object class that is used for the OnTraceLine callback function
/// 
/// The extra data members allow to hook into the traceline process and add custom trace hits.
/// 
/// Custom code might modify the result table (m_iResultTableCount/m_pFirstResultInfo member) via
/// VisTraceLineInfo_t::InsertTraceResult and fill out the structure in case there is a better hit.
/// 
/// If any hit has been detected, the m_bDetected should be set to TRUE.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa VisCallbackManager_cl::OnTraceLine
/// \sa VisCollisionToolkit_cl::TraceLine
class VisTraceLineDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor
  VisTraceLineDataObject_cl(VisCallback_cl *pSender, const hkvVec3& vRayStart, const hkvVec3& vRayEnd, int iFlags, int iResultTableCount, VisTraceLineInfo_t *pFirstResultInfo) 
    : IVisCallbackDataObject_cl(pSender), m_vRayStart(vRayStart), m_vRayEnd(vRayEnd), m_iTraceFlags(iFlags),
      m_iResultTableCount(iResultTableCount), m_pFirstResultInfo(pFirstResultInfo)
  {
    m_bDetected = FALSE;
  }
  
  /// \brief
  ///   Member that indicates whether a custom hit has been detected
  BOOL m_bDetected;

  /// \brief
  ///   Start of the trace ray (world position)
  const hkvVec3& m_vRayStart;

  /// \brief
  ///   End of the trace ray (world position)
  const hkvVec3& m_vRayEnd;

  /// \brief
  ///   Trace flags as globally specified via VisCollisionToolkit_cl::SetTraceLineSettings
  int m_iTraceFlags;

  /// \brief
  ///   Number of entries in the m_pFirstResultInfo array. Can be passed to static
  ///   VisTraceLineInfo_t::InsertTraceResult function
  int m_iResultTableCount;

  /// \brief
  ///   Pointer to first entry in trace result table. Can be passed to static
  ///   VisTraceLineInfo_t::InsertTraceResult function
  VisTraceLineInfo_t *m_pFirstResultInfo;
};



/// \brief
///   Generic callback data object class that is used for various callbacks that gather a new
///   filename as a replacement for a standard filename
/// 
/// See for instance VisCallbackManager_cl::OnGatherMaterialsFilename.
class VisFilenameDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor
  VisFilenameDataObject_cl(VisCallback_cl *pSender, const char *szOrigFilename) 
    : IVisCallbackDataObject_cl(pSender)
  {
    m_szOrigFilename = szOrigFilename;
    m_bChanged = false;
  }

  /// \brief
  ///   Holds the original filename
  const char* m_szOrigFilename;
  /// \brief
  ///   Empty by default; Assign a string in case a replacement filename should be used
  VStaticString<FS_MAX_PATH> m_sNewFilename;

  /// \brief
  ///   Set this flag to true to confirm that the new filename should be used.
  bool m_bChanged;
};

/// \brief
///   Sound callback data object.
class VisSoundDataObject_cl : public IVisCallbackDataObject_cl
{
public:

  /// \brief
  ///   Constructor
  VisSoundDataObject_cl(VisCallback_cl *pSender, VSoundObject* pSndObj ) 
    : IVisCallbackDataObject_cl(pSender), m_pSoundObject(pSndObj), m_iEventIndex(-1)
  {
    m_sEventName[0]=0;
  }

  /// Source sound object
  VSoundObject* m_pSoundObject;
  /// Event index when it's used as event callback data object. -1 otherwise.
  int m_iEventIndex;
  /// Event name when it's used as event callback data object. empty otherwise.
  char m_sEventName[128];
};

class IVisBackgroundResourceRestorer_cl;

/// \brief
///   Data object for the Vision::Callbacks.OnBackgroundingRestore callback
class VisBackgroundRestoreObject_cl : public IVisCallbackDataObject_cl
{
public:
  /// \brief
  ///   Constructor.
  /// \param pSender
  ///   the callback instance that sent this callback
  /// \param percentage
  ///   the percentage of the restore progress
  /// \param pBackgroundResourceRestorer
  ///   the object that restores the resources
  VisBackgroundRestoreObject_cl(VCallback *pSender, float percentage,
    IVisBackgroundResourceRestorer_cl* pBackgroundResourceRestorer) 
    : IVisCallbackDataObject_cl(pSender), m_percentage(percentage)
    , m_pBackgroundResourceRestorer(pBackgroundResourceRestorer)
  {}

  float m_percentage; ///< the percentage of the restore progress
  IVisBackgroundResourceRestorer_cl* m_pBackgroundResourceRestorer; ///< the object that restores the resources
};

/// \brief
///   The Vision engine provides some pre-defined callbacks that can be accessed via
///   Vision::Callbacks
/// 
/// \sa VCallback::RegisterCallback
/// \sa VCallback::DeregisterCallback
/// \sa class IVisCallbackHandler_cl
/// \sa IVisCallbackHandler_cl::OnHandleCallback
class VisCallbackManager_cl
{
public:
  VisCallbackManager_cl();

  /// \brief
  ///   This callback gets triggered by the engine when a new world has been initialized
  ///   or cleared and re-initialized.
  VisCallback_cl OnWorldInit;

  /// \brief
  ///   This callback gets triggered by the engine when deinitialising a world 
  /// Deprecated - Please use OnBeforeSceneUnloaded or OnAfterSceneUnloaded
  VisCallback_cl OnWorldDeInit;

  /// \brief
  ///   This callback gets triggered by the engine while loading a scene or a model. The passed
  ///   data object is of type VisProgressDataObject_cl which provides some additional information
  /// 
  /// This callback gets called while loading a world or a model.
  /// 
  /// The passed data object is of type VisProgressDataObject_cl which keeps the information about
  /// the loading progress.
  /// Loading of scenes can be aborted in this callback by calling the SetAbort() function on the passed 
  /// VisProgressDataObject_cl::m_Progress member. Note that there can still occur some callbacks even after
  /// loading has been aborted, because the actual abort can only take place with a defined granularity.
  /// For instance, it will not be aborted inside the de-serialization process of a single shape.
  VisCallback_cl OnProgress;

  /// \brief
  ///   This callback gets triggered by the scene implementation at the beginning of a scene update
  ///   (OnUpdateScene function)
  /// 
  /// For custom application implementations (classes derived from IVisApp_cl), this callback has
  /// to be triggered manually. See implementation of VisionApp_cl class.
  /// 
  /// \sa VisionApp_cl::OnUpdateScene()
  VisCallback_cl OnUpdateSceneBegin;

  /// \brief
  ///   This callback gets triggered by the scene implementation at the end of a scene update
  ///   (OnUpdateScene function)
  /// 
  /// For custom application implementations (classes derived from IVisApp_cl), this callback has
  /// to be triggered manually. See implementation of VisionApp_cl class.
  /// Since static function VisObject3DVisData_cl::HandleAllNodeTransitions() has been called already when OnUpdateSceneFinished is 
  /// triggered in the default application class, this callback should not be used to update positions of objects (including the camera object)
  /// that rely on portal visibility.
  /// 
  /// \sa VisionApp_cl::OnUpdateScene()
  VisCallback_cl OnUpdateSceneFinished;

  /// \brief
  ///   This callback gets triggered by the scene implementation at the beginning of one Run loop (before rendering). By default it is called from inside VisionApp_cl::OnFrameUpdatePreRender
  ///
  /// There is an important difference to OnUpdateSceneBegin/OnUpdateSceneFinished callbacks. These two callbacks are triggered once for every simulation
  /// tick, which can be 0, 1 or multiple times an application loop. OnFrameUpdate on the other hand is always triggered exactly once per loop, so it is a good place
  /// to update all states that are not based on simulation ticks/timer. GUI update is a good example.
  VisCallback_cl OnFrameUpdatePreRender;

  /// \brief
  ///   This callback gets triggered by the scene implementation at the end of one Run loop (after rendering). By default it is called from inside VisionApp_cl::OnFrameUpdatePostRender
  VisCallback_cl OnFrameUpdatePostRender;

  /// \brief
  ///   This callback gets triggered by the application implementation at the end of its Run method 
  ///   when OnFinishScene is called. This is triggered after rendering has been completed and before
  //    the final screen is presented, and should not to be confused OnUpdateSceneFinished which 
  ///   happens earlier when OnUpdateScene finishes.
  ///
  /// For custom application implementations (classes derived from IVisApp_cl), this callback has
  /// to be triggered manually. See implementation of VisionApp_cl class.
  /// 
  /// \sa VisionApp_cl::OnFinishScene()
  VisCallback_cl OnFinishScene;

  /// \brief
  ///  This callback is triggered by the application implementation after Vision::Renderer.BeginRendering() has been called.
  ///
  /// Listeners may use this callback to perform custom rendering outside of the registered contexts and renderer nodes.
  VisCallback_cl BeginRendering;

  /// \brief
  ///   This callback is triggered by the application implementation before Vision::Renderer.EndRendering() is called.
  VisCallback_cl EndRendering;

  
  /// \brief
  ///   This callback gets called AFTER the engine has been initialized
  /// 
  /// Plugins can register to this callback to be notified when the engine gets initialized.
  /// 
  /// Note that at calling time of OnEngineInit the Video mode is not yet initialized so GPU
  /// dependent resources such as textures or mesh buffers cannot be created.
  /// 
  /// It is possible that initialization and deinitialization happens more than once.
  /// 
  /// \sa VisCallbackManager_cl::OnEngineDeInit()
  VisCallback_cl OnEngineInit;

  /// \brief
  ///   This callback gets called BEFORE the engine gets deinitialized, but after the video has
  ///   been deinitialized
  /// 
  /// Plugins can register to this callback to be notified when the engine gets deinitialized.
  /// Final cleanup can be performed here.
  /// 
  /// It is possible that initialization and deinitialization happens more than once.
  /// 
  /// \sa VisCallbackManager_cl::OnEngineInit()
  VisCallback_cl OnEngineDeInit;

  /// \brief
  ///   This callback gets called BEFORE the engine gets deinitialized
  /// 
  /// In contrast to VisCallbackManager_cl::OnEngineDeInit the video context is still valid while this callback is
  /// triggered.
  /// 
  /// It is thus useful to perform some final resource cleanup that rely on the graphics context,
  /// such as textures etc.
  /// 
  /// \sa VisCallbackManager_cl::OnEngineDeInit()
  VisCallback_cl OnEngineDeInitializing;
  
  /// \brief
  ///   This callback gets triggered by the engine after a model file has been loaded.
  /// 
  /// The data object can be casted to VDynamicMeshLoadedDataObject which has a m_pModel member.
  VisCallback_cl OnNewModelLoaded;

  /// \brief
  ///   This callback gets triggered by the engine after a mesh file has been loaded.
  /// 
  /// The data object can be casted to VisMeshFileLoadedDataObject_cl which has a m_pMesh member.
  VisCallback_cl OnNewMeshFileLoaded;

  /// \brief
  ///   This callback gets triggered by the engine right before a texture is loaded.
  /// 
  /// The data object can be casted to VisTextureLoadingDataObject_cl which allows accessing the
  /// filename and modifying the loading flags.
  /// This callback can be used to provide custom texture formats; however, there is a dedicated interface for that: IVTextureFormatProvider that also interfaces with vForge.
  VisCallback_cl OnNewTextureLoading;

  /// \brief
  ///   This callback gets triggered by the render loop implementation at various places inside the render pipeline
  /// 
  /// The data object can be casted to VisRenderHookDataObject_cl which has a member that describes
  /// the entry point constant.
  /// 
  /// This mechanism is very useful for modular plugins that hook into any existing render loops to
  /// render custom geometry.
  VisCallback_cl OnRenderHook;

  /// \brief
  ///   This callback gets triggered by the engine after a new render context has been activated
  /// 
  /// This is useful to initialize context specific data.
  /// 
  /// The data object does not contain specific information. Instead, the new context can be
  /// accessed via VisRenderContext_cl::GetCurrentContext()
  VisCallback_cl OnContextSwitched;

  /// \brief
  ///   This callback gets called right before a context is to be rendered. See VisionApp_cl::Run implementation.
  /// 
  /// \sa VisContextDataObject_cl
  /// \sa VisionApp_cl
  VisCallback_cl OnContextSwitching;

  /// \brief
  ///   This callback gets triggered by the engine when a new renderer node is to be handled.
  /// 
  /// This is useful to initialize data specific to a certain renderer node. For instance, the mirror and water implementations
  /// use this function to perform operations which are specific for each view.
  VisCallback_cl OnRendererNodeSwitching;

  /// \brief
  ///   This callback gets triggered by the engine after the visibility has been computed for the
  ///   scene
  /// 
  /// This is useful to perform some custom visibility based on the scene result (e.g. visibility
  /// objects)
  /// 
  /// The data object does not contain specific information. Instead, the current context can be
  /// accessed via VisRenderContext_cl::GetCurrentContext()
  VisCallback_cl OnVisibilityPerformed;

  /// \brief
  ///   This callback gets triggered by the engine before the video mode or screen resolution is
  ///   changed
  /// 
  /// The data object can be casted to VisVideoChangedDataObject_cl which has a member that
  /// describes the new video config.
  VisCallback_cl OnBeforeVideoChanged;

  /// \brief
  ///   This callback gets triggered by the engine after the video mode or screen resolution has
  ///   changed
  /// 
  /// The data object can be casted to VisVideoChangedDataObject_cl which has a member that
  /// describes the new video config.
  VisCallback_cl OnVideoChanged;

  /// \brief
  ///   This callback gets triggered by the engine after the video mode has been initialized
  /// 
  /// The data object can be casted to VisVideoChangedDataObject_cl which has a member that
  /// describes the new video config. This callback gets triggered from inside VisVideo_cl::SetMode.
  /// Unlike other initialization callbacks this callback can be used to create GPU resources.
  /// Furthermore the main context (Vision::Context.GetMainRenderContext) is available at this point.
  VisCallback_cl OnVideoInitialized;

  /// \brief
  ///   This callback gets triggered by the engine right before the video mode is to be de-initialized
  /// 
  /// The data object can be casted to VisVideoChangedDataObject_cl which has a member that
  /// describes the current video config. This callback gets triggered from inside VisVideo_cl::DeInit.
  /// Since the video mode is still set and initialized, GPU resources should be released here.
  VisCallback_cl OnVideoDeinitializing;


  /// \brief
  ///   This callback gets called inside Vision::Editor.SetMode(), after the the editor play mode
  ///   has changed
  /// 
  /// The passed data object can be casted to VisEditorModeChangedDataObject_cl which provides
  /// information about the old mode and the new mode.
  /// 
  /// This callback is only relevant when the application runs inside vForge, so it can listen to
  /// this event.
  /// 
  /// For example, when switching to EDITORMODE_PLAYING_IN_GAME mode, the application can
  /// initialize a HUD etc.
  /// 
  /// Note, that when switching the editor mode the engine instances (e.g. entities) are not yet
  /// created.
  /// 
  /// Thus in most situations the OnBeforeSceneLoaded resp. the OnAfterSceneLoaded callback might
  /// be more suitable, because for these callbacks vForge mimics the same behaviour as starting
  /// the exported scene with the scene viewer.
  /// 
  /// vForge triggers another callback OnEditorModeChanging right before it changes the mode.
  /// 
  /// \sa IVisCallbackHandler_cl::OnEditorModeChanging
  /// \sa IVisCallbackHandler_cl::OnBeforeSceneLoaded
  /// \sa IVisCallbackHandler_cl::OnAfterSceneLoaded
  VisCallback_cl OnEditorModeChanged;


  /// \brief
  ///   This callback gets called inside VisRenderer_cl::SetRendererNode, after a new node has been set
  /// 
  /// Changing the renderer node usually means switching to new shader sets (e.g. deferred shading shaders)
  /// so this callback can be used to assign new shaders to custom geometry
  VisCallback_cl OnRendererNodeChanged;

  /// \brief
  ///   This callback is triggered whenever the reference context is recreated
  VisCallback_cl OnReferenceContextChanged;

  /// \brief
  ///   This callback is triggered by the scene loader class right before a new .vscene file is to
  ///   be loaded
  /// 
  /// This callback is useful to perform some per-scene de-initialisation.
  /// 
  /// There is no engine mechanism to call this function. Instead the vscene loading code is
  /// responsible for triggering this callback.
  /// 
  /// The VSceneLoader class shipped with the SDK triggers this callback.
  /// 
  /// The passed data object can be casted to VisSceneLoadedDataObject_cl which stores the filename
  /// of the new scene.
  /// 
  /// The OnWorldDeInit gets still triggered inside the scene loading code.
  /// 
  /// To smoothly integrate game applications into vForge, this callback gets also triggered by
  /// vForge directly before all  engine instances are created for playing in the editor.
  VisCallback_cl OnBeforeSceneLoaded;

  /// \brief
  ///   This callback is triggered by the scene loader class after a new .vscene file has been
  ///   successfully loaded
  /// 
  /// This callback is useful to perform some per-scene initialisation.
  /// 
  /// There is no engine mechanism to call this function. Instead the vscene loading code is
  /// responsible for triggering this callback.
  /// 
  /// The VSceneLoader class shipped with the SDK triggers this callback.
  /// 
  /// The passed data object can be casted to VisSceneLoadedDataObject_cl which stores the filename
  /// of the new scene.
  /// 
  /// The OnWorldInit gets still triggered inside the scene loading code, i.e. after the V3D
  /// file has been loaded which is referenced by the vscene file.
  /// 
  /// To smoothly integrate game applications into vForge, this callback gets also triggered by
  /// vForge directly after all  engine instances have been created for playing in the editor.
  VisCallback_cl OnAfterSceneLoaded;

  /// \brief
  ///   This callback is triggered by the scene loader class right before a .vscene file is to
  ///   be unloaded and world deinitialized.
  /// 
  /// This callback is useful to perform some per-scene de-initialisation.
  /// 
  /// There is no engine mechanism to call this function. Instead the vscene unloading code is
  /// responsible for triggering this callback.
  /// 
  /// The VSceneLoader class shipped with the SDK triggers this callback.
  /// 
  /// This callback does not have any specific parameters
  /// 
  VisCallback_cl OnBeforeSceneUnloaded;

  /// \brief
  ///   This callback is triggered by the scene loader class after a .vscene file has been
  ///   successfully unloaded and the world deinitialized.
  /// 
  /// There is no engine mechanism to call this function. Instead the vscene unloading code is
  /// responsible for triggering this callback.
  /// 
  /// The VSceneLoader class shipped with the SDK triggers this callback.
  /// 
  /// This callback does not have any specific parameters
  ///
  VisCallback_cl OnAfterSceneUnloaded;

  /// \brief
  ///   This callback is triggered by vForge right before the scene export process is started. You
  ///   can use this callback to cancel the export.
  ///
  /// The passed data object can be casted to VisBeforeSceneExportedObject_cl. It stores the path to the
  /// target scene file and allows to cancel the export with an error message.
  VisCallback_cl OnBeforeSceneExported;

  /// \brief
  ///   This callback is triggered by vForge right after the scene export has finished. No special
  ///   data object class is passed to this callback
  VisCallback_cl OnAfterSceneExported;

  /// \brief
  ///   This callback is triggered right before the application goes to the background.
  /// 
  /// At this point all graphics resources are still valid. 
  ///
  /// On Android, there should be made sure that processing time is kept to a minimum while 
  /// handling this callback, otherwise Android might force termination of the application.
  /// 
  /// \sa OnEnterBackground
  VisCallback_cl OnLeaveForeground;

  /// \brief
  ///   This callback gets triggered by the engine when the application goes to the background
  ///   and needs to release GPU resources.
  /// 
  /// For Win32/DirectX 9 applications, this event indicates a lost device. 
  /// On mobile devices the application is about to go to the background. This is especially
  /// important on Android devices, as all GPU resources will be lost there.
  /// 
  /// When an application goes to the background, user-created resources will be invalid afterwards. 
  /// Your code has to make sure that these resource are appropriately cleaned up and, if necessary, 
  /// recreated.
  /// 
  /// This is the place where you should clean up the resources. To actually recreate the
  /// resources, handle the OnLeaveBackground callback.
  ///
  /// After receiving this callback on mobile platforms, all state should be saved since the 
  /// application can be terminated by the OS to regain memory.
  ///
  /// \sa OnLeaveBackground
  VisCallback_cl OnEnterBackground;

  /// \brief
  ///   This callback gets gets triggered by the engine when the application comes back
  ///   from the background and is about the enter the foreground.
  /// 
  /// On Win32/DirectX 9 applications, this indicates a Direct3D device recovery from a lost state.
  /// 
  /// When the application comes back to the foreground, user-created resources will be invalid. 
  /// Your code has to make sure that these resources are appropriately cleaned up and, if necessary, 
  /// recreated. This is the place where you should recreate the resources.
  ///
  /// To properly handle recovery OnBackgroundRestore has to be handled too.
  ///
  /// \sa OnEnterBackground
  /// \sa OnEnterForeground
  /// \sa OnBackgroundRestore
  VisCallback_cl OnLeaveBackground;

  /// \brief
  ///   This callback gets gets triggered by the engine when the application is in foreground again and
  ///   all affected resources have been reloaded.
  /// 
  /// On Win32/DirectX 9 applications, this indicates that the DirectX9 device has completely recovered
  /// from a lost state.
  VisCallback_cl OnEnterForeground;

  /// \brief
  ///   This callback starts the automatic restoration process of all affected resources.
  ///
  /// The passed data object has to be casted to VisBackgroundRestoreObject_cl which lets you control
  /// the restoration process by calling VisBackgroundRestoreObject_cl::Tick() every frame until
  /// VisBackgroundRestoreObject_cl::IsFinished() returns true.
  ///
  /// \sa VisBackgroundRestoreObject_cl
  VisCallback_cl OnBackgroundRestore;

  /// \brief
  ///   This callback gets called when the shader resources have changed that require an update of
  ///   the shader assignments
  /// 
  /// It gets triggered by the vForge shader editing tool to notify that all compiled techniques need 
  /// to be updated. The background restoration process can also trigger this callback if shader
  /// unloading/reloading is enabled for backgrounding.
  /// 
  /// Any new shape type that requires shader updating in vForge should listen to this function.
  VisCallback_cl OnReassignShaders;

  /// \brief
  ///   This callback gets called by vForge to gather lightmap information for custom lightmapped
  ///   meshes
  /// 
  /// The data object can be casted to class VLightmapInfoDataObject_cl, which contains a reference
  /// to a custom lightmap information object (class VLightmapSceneInfo)
  /// 
  /// Please refer to the documentation for detailed information about support for custom
  /// lightmapped geometry.
  /// 
  /// \sa VisCallbackManager_cl::OnLightmapFileLoaded()
  /// \sa VLightmapSceneInfo
  VisCallback_cl OnGatherLightmapInfo;

  /// \brief
  ///   This callback gets called when a lightmap information file is loaded that provides
  ///   information for custom lightmap primitives
  /// 
  /// It is either triggered by vForge after lighting is finished or by the scene loader class that
  /// detected a lightmap file embedded into an exported scene.
  /// 
  /// Please refer to the documentation for detailed information about support for custom
  /// lightmapped geometry.
  /// 
  /// \sa VisCallbackManager_cl::OnGatherLightmapInfo()
  /// \sa VLightmapSceneInfo
  VisCallback_cl OnLightmapFileLoaded;

  /// \brief
  ///   This callback gets called from vForge when the "Reset static lighting" option is selected
  ///   from the menu. No specific parameters are passed
  VisCallback_cl OnResetStaticLighting;

  /// \brief
  ///   This callback gets called by script managers either every frame or every x seconds
  /// 
  /// Script objects may register to this callback to call a script function.
  /// 
  /// The default script manager in the plugin uses this callback for every script object that
  /// implemented a OnThink function.
  VisCallback_cl OnScriptThink;

  /// \brief
  ///   This callback gets called right before vForge changes the mode via
  ///   Vision::Editor.SetMode(), and also before vForge re-creates engine instances
  /// 
  /// The passed data object can be casted to VisEditorModeChangedDataObject_cl which provides
  /// information about the old mode and the new mode.
  /// 
  /// \sa IVisCallbackHandler_cl::OnEditorModeChanged
  /// \sa IVisCallbackHandler_cl::OnBeforeSceneLoaded
  /// \sa IVisCallbackHandler_cl::OnAfterSceneLoaded
  VisCallback_cl OnEditorModeChanging;


  /// \brief
  ///   This callback gets called when a console command is executed
  /// 
  /// The passed data object can be casted to a VisConsoleDataObject_cl object to retrieve the
  /// command string
  /// 
  /// \sa VisConsoleDataObject_cl
  VisCallback_cl OnConsoleCommand;

  /// \brief
  ///   This callback gets called after the loaded/unloaded status of any resource has changed.
  /// 
  /// The passed data object can be casted to VisResourceInfoDataObject_cl which provides
  /// information about the resource that changed its status.
  /// 
  /// For a resource that has been loaded, the VisResourceInfoDataObject_cl::m_iAction member is
  /// set to VRESOURCECHANGEDFLAG_LOADEDRESOURCE resp. VRESOURCECHANGEDFLAG_UNLOADEDRESOURCE for
  /// unloading.
  /// In DEBUG configuration of the engine also the VRESOURCECHANGEDFLAG_RESOURCE_ADDREF/VRESOURCECHANGEDFLAG_RESOURCE_RELEASE
  /// constant is passed when any resource increments or decrements its refcounter.
  /// 
  /// \sa VisResourceInfoDataObject_cl
  VisCallback_cl OnResourceChanged;

  /// \brief
  ///   This callback is triggered right at the beginning of
  ///   IVisVisibilityCollector_cl::OnDoVisibilityDetermination implementations. The data object
  ///   can be casted to VisVisibilityCollectorDataObject_cl
  VisCallback_cl OnStartVisibilityDetermination;

  /// \brief
  ///   This callback is triggered at the end of
  ///   IVisVisibilityCollector_cl::OnDoVisibilityDetermination implementations. The data object
  ///   can be casted to VisVisibilityCollectorDataObject_cl
  VisCallback_cl OnFinishVisibilityDetermination;

  /// \brief
  ///   This callback is triggered by vForge once the visibility has been rebuilt.
  VisCallback_cl OnRebuiltVisibility;

  /// \brief
  ///   This callback is triggered for every call of Vision::CollisionToolkit.TraceLine.
  /// 
  /// The passed data object can be casted to VisTraceLineDataObject_cl. The data object provides
  /// some extra members that can be filled in by custom code.
  /// 
  /// This callback allows programmers to hook into the traceline processing and add custom trace
  /// hits.
  /// 
  /// Use the static VisTraceLineInfo_t::InsertTraceResult function to insert custom results into
  /// the table.
  VisCallback_cl OnTraceLine;

  /// \brief
  ///   This callback is triggered for every material.xml file to be loaded or saved
  /// 
  /// Hook into this callback to pass a replacement filename other than the default one.
  /// 
  /// The data object can be casted to VisFilenameDataObject_cl. On the data object, assign the
  /// target filename to the m_sNewFilename member and set m_bChanged to true. The m_szOrigFilename
  /// member holds the source name of the resource (e.g. model file) and not the material's
  /// filename.
  VisCallback_cl OnGatherMaterialsFilename;
  
  /// \brief
  ///   This callback gets triggered by the engine when the system runs low on memory
  /// 
  /// The passed data object will be a generic IVisCallbackDataObject_cl object and does not 
  /// contain any additional data.
  /// 
  /// After receiving this callback you should free all unused data to regain memory.
  VisCallback_cl OnMemoryWarning;

  /// \brief
  ///   This callback is triggered just before the back buffer is presented on the screen.
  ///
  /// For custom application implementations (classes derived from IVisApp_cl), this callback has
  /// to be triggered manually. The implementation of VisionApp_cl already takes care of this.
  VisCallback_cl OnBeforeSwapBuffers;

  /// \brief
  ///   This callback is triggered whenever the global fog settings (set via VisWorld_cl::SetFogParameters) change
  VisCallback_cl OnGlobalRenderSettingsChanged;
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
