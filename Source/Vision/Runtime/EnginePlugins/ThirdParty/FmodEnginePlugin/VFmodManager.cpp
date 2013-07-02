/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/FmodEnginePlugin.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiTriggerComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

#if defined(_VISION_XENON)
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodXenon.inl>
#else
VMapPtrToUInt g_FmodAllocations;
#endif

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


// static variables
VFmodManager VFmodManager::g_GlobalManager;
int VFmodManager::PROFILING_FMOD_OVERALL = 0;
int VFmodManager::PROFILING_FMOD_PUREUPDATE = 0;
int VFmodManager::PROFILING_FMOD_PURGE = 0;


// forward declaration of Fmod file callbacks
FMOD_RESULT F_CALLBACK VisionFM_Open(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata);
FMOD_RESULT F_CALLBACK VisionFM_Close(void *handle, void *userdata);
FMOD_RESULT F_CALLBACK VisionFM_Read(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
FMOD_RESULT F_CALLBACK VisionFM_Seek(void *handle, unsigned int pos, void *userdata);

// forward declaration of Fmod memory callbacks
void* F_CALLBACK VisionFM_Alloc(unsigned int size, FMOD_MEMORY_TYPE type, const char *sourcestr);
void* F_CALLBACK VisionFM_Realloc(void *ptr, unsigned int size, FMOD_MEMORY_TYPE type, const char *sourcestr);
void F_CALLBACK VisionFM_Free(void *ptr, FMOD_MEMORY_TYPE type, const char *sourcestr);


// extern variables
extern "C" int luaopen_FireLight(lua_State *);


#define _DEBUG_OUTPUT
#undef _DEBUG_OUTPUT


// -------------------------------------------------------------------------- //
// Constructor/ Destructor                                                 
// -------------------------------------------------------------------------- //
VFmodManager::VFmodManager() 
{
  m_bAnyStopped = false;
  m_pListenerObject = NULL;

  m_pSoundResourceManager = NULL;
  m_pEventGroupManager = NULL;

  m_pEventSystem = NULL;
  m_pSystem = NULL;
  pMemoryPool = NULL;
  m_pMasterGroup = NULL;
  m_pMusicGroup = NULL;

  m_fTimeLeftOver = 0.0f;

#if defined(_VISION_MOBILE)
  m_bMasterGroupPausedInForeground = false;
  m_bMusicGroupPausedInForeground = false;
  m_bMasterEventCategoryPausedInForeground = false;
#endif
}

VFmodManager::~VFmodManager()
{
}


// -------------------------------------------------------------------------- //
// Init/ Deinit                                                 
// -------------------------------------------------------------------------- //
VFmodManager& VFmodManager::GlobalManager() 
{
  return g_GlobalManager;
}

void VFmodManager::OneTimeInit()
{
  if (!PROFILING_FMOD_OVERALL)
  {
    PROFILING_FMOD_OVERALL = Vision::Profiling.GetFreeElementID();
    PROFILING_FMOD_PUREUPDATE = Vision::Profiling.GetFreeElementID();
    PROFILING_FMOD_PURGE = Vision::Profiling.GetFreeElementID();

    Vision::Profiling.AddGroup("Fmod");
    VProfilingNode *pOverall = Vision::Profiling.AddElement(PROFILING_FMOD_OVERALL, "Overall", TRUE);
      VProfilingNode *pUpdate = Vision::Profiling.AddElement(PROFILING_FMOD_PUREUPDATE, "  Pure update", TRUE, pOverall);  
        Vision::Profiling.AddElement(PROFILING_FMOD_PURGE, "  Purge instances", TRUE, pOverall);
  }

  Vision::RegisterModule(&g_FmodModule);

  m_pSoundResourceManager = new VFmodSoundResourceManager();
  Vision::ResourceSystem.RegisterResourceManager(m_pSoundResourceManager, VColorRef(0,255,100));
  m_pEventGroupManager = new VFmodEventGroupManager();
  Vision::ResourceSystem.RegisterResourceManager(m_pEventGroupManager, VColorRef(255,255,100));

  Vision::Callbacks.OnEngineInit += this;
  Vision::Callbacks.OnEngineDeInit += this;
  Vision::Callbacks.OnWorldInit += this;
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnUpdateSceneFinished += this;

#if defined(_VISION_MOBILE)
  Vision::Callbacks.OnLeaveForeground += this;
  Vision::Callbacks.OnEnterForeground += this;
#endif
 
  IVScriptManager::OnRegisterScriptFunctions += this;
  IVScriptManager::OnScriptProxyCreation += this;
}

void VFmodManager::OneTimeDeInit()
{
  Vision::UnregisterModule(&g_FmodModule);

  Vision::ResourceSystem.UnregisterResourceManager(m_pSoundResourceManager);
  V_SAFE_DELETE(m_pSoundResourceManager);
  Vision::ResourceSystem.UnregisterResourceManager(m_pEventGroupManager);
  V_SAFE_DELETE(m_pEventGroupManager);

  Vision::Callbacks.OnEngineInit -= this;
  Vision::Callbacks.OnEngineDeInit -= this;
  Vision::Callbacks.OnWorldInit -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
  Vision::Callbacks.OnUpdateSceneFinished -= this;

#if defined(_VISION_MOBILE)
  Vision::Callbacks.OnLeaveForeground -= this;
  Vision::Callbacks.OnEnterForeground -= this;
#endif

  IVScriptManager::OnRegisterScriptFunctions -= this;
  IVScriptManager::OnScriptProxyCreation -= this;
}

bool VFmodManager::IsInitialized() const
{
  return (m_pEventSystem != NULL);
}

void VFmodManager::InitFmodSystem()
{
  // Give the application a chance to modify the m_config.
  VFmodConfigCallbackData config(&OnBeforeInitializeFmod, m_config);
  OnBeforeInitializeFmod.TriggerCallbacks(&config);

  // Set debug output level. Has only effect when using Fmod logging libs, otherwise will return FMOD_ERR_UNSUPPORTED.
#ifdef HK_DEBUG_SLOW
  FMOD_WARNINGCHECK(FMOD::Debug_SetLevel(m_config.iDebugLevel));
#endif

  //  initialize memory
  {
    FMOD_RESULT result = FMOD_OK;
    if (!m_config.bUseMemoryPool)
      result = FMOD::Memory_Initialize(NULL, 0, VisionFM_Alloc, VisionFM_Realloc, VisionFM_Free, 0);
    else
    {
#ifndef _VISION_XENON
      pMemoryPool = vMemAlloc(m_config.iMemoryPoolSize);
#else
      pMemoryPool = VFmodXenonAlloc(m_config.iMemoryPoolSize, true);
#endif
      VASSERT(pMemoryPool != NULL);
      result = FMOD::Memory_Initialize(pMemoryPool,m_config.iMemoryPoolSize, NULL, NULL, NULL);
    }

    // In case the old Sound Plugin and the new Fmod Plugin are both loaded inside vForge (required for converting scenes),
    // it can happen that the Fmod system is already initialized by the old Sound Plugin. Then Memory_Initialize() will be 
    // called after Fmod system is already initialized, which is illegal. Therefore inside vForge failing to call Memory_Initialize()
    // in such a case is not treated as fatal error. Fmod will simply not use Vision De-/ Allocators or a preallocated memory block,
    // instead it will use it own de-/ allocators. This should be fine in case of converting scenes. Otherwise it should be anyway 
    // avoided to load both plugins simultaneously.
    if (!Vision::Editor.IsInEditor())
      FMOD_ERRORCHECK(result);
    else
    {
      if(result == FMOD_ERR_INITIALIZED)
        Vision::Error.Warning("Failed to initialize Fmod memory system, this is most probably because the old Sound Plugin is loaded, too. This should only be done for converting scenes.");
      else
        FMOD_ERRORCHECK(result);
    }
  }

  FMOD_ERRORCHECK(FMOD::EventSystem_Create(&m_pEventSystem)); 
#ifdef VFMOD_SUPPORTS_NETWORK
  if (m_config.bUseNetworkSystem)
  {
#ifdef _VISION_PS3
    bool bNetworkInitialized = VInitializeNetworkPS3();
    VASSERT(bNetworkInitialized == true);
#endif
    FMOD_ERRORCHECK(FMOD::NetEventSystem_Init(m_pEventSystem));
  }
#endif
  FMOD_ERRORCHECK(m_pEventSystem->getSystemObject(&m_pSystem));

  unsigned int iVersion = 0;
  FMOD_ERRORCHECK(m_pEventSystem->getVersion(&iVersion));
  VASSERT(iVersion >= FMOD_VERSION)

  FMOD_ERRORCHECK(m_pSystem->setSoftwareFormat(m_config.iSampleRate, m_config.iFormat, 0,m_config.iMaxInputChannels, m_config.iResampleMethod));

  // install file manager callbacks
  FMOD_ERRORCHECK(m_pSystem->setFileSystem(VisionFM_Open, VisionFM_Close, VisionFM_Read, VisionFM_Seek, NULL, NULL, 4096));

#ifdef _VISION_ANDROID
  // Increase DSP buffer size on Android
  {
    unsigned int iBlocksize = 0;
    int iNumblocks = 0;
    m_pSystem->getDSPBufferSize(&iBlocksize, &iNumblocks); 
    m_pSystem->setDSPBufferSize(iBlocksize*2, iNumblocks);

    #ifdef HK_DEBUG_SLOW
      int iBufferSize = iNumblocks*iBlocksize;
      Vision::Error.Warning("Fmod DSP buffer size increased from %i to %i, in order to get correct sound output", iBufferSize, iBufferSize*2);
    #endif
  }
#endif

#ifndef _VISION_PS3

  FMOD_RESULT result = m_pEventSystem->init(m_config.iMaxChannels, FMOD_INIT_NORMAL, NULL, FMOD_EVENT_INIT_NORMAL);
  FMOD_WARNINGCHECK(result);

#else

  VSpursHandler *spursHandler = Vision::GetSpursHandler();
  VASSERT(spursHandler != NULL);
  CellSpurs *spurs = spursHandler->GetSpurs();
  FMOD_PS3_EXTRADRIVERDATA extradriverdata;
  memset(&extradriverdata, 0, sizeof(FMOD_PS3_EXTRADRIVERDATA));
  extradriverdata.spurs = spurs;  /* Using SPURS */
  extradriverdata.spu_thread_priority = 16;  /* Default, THIS WILL BE IGNORED */
  FMOD_RESULT result = m_pEventSystem->init(m_config.iMaxChannels, FMOD_INIT_NORMAL, (void *)&extradriverdata, FMOD_EVENT_INIT_NORMAL);
  FMOD_WARNINGCHECK(result);

#endif

  // In case of missing sound card, we need to deinitialize the Fmod Event System.
  // However it is still ensured, that the application can be run, by preventing calls to native Fmod functions.
  if(result != FMOD_OK) 
  {
    DeInitFmodSystem();
    Vision::Error.Warning("The application will run without sound output!");
    return;
  }

  FMOD_ERRORCHECK(m_pSystem->set3DSettings(1.0, m_config.fFmodToVisionScale, 1.0f));

  // get the master channel group where all sound instances go to initially
  FMOD_ERRORCHECK(m_pSystem->getMasterChannelGroup(&m_pMasterGroup));
  VASSERT(m_pMasterGroup!=NULL);

  // create a special channel group for background music
  FMOD_ERRORCHECK(m_pSystem->createChannelGroup("music", &m_pMusicGroup));
  VASSERT(m_pMusicGroup!=NULL);

  IVisCallbackDataObject_cl data(&OnAfterInitializeFmod);
  OnAfterInitializeFmod.TriggerCallbacks(&data);
}

void VFmodManager::DeInitFmodSystem()
{
  IVisCallbackDataObject_cl dataBefore(&OnBeforeDeinitializeFmod);
  OnBeforeDeinitializeFmod.TriggerCallbacks(&dataBefore);

  m_pSoundResourceManager->PurgeUnusedResources();
  m_pEventGroupManager->PurgeUnusedResources();

  if (m_pEventSystem)
  { 
    m_pMusicGroup->release();
    m_pMusicGroup = NULL;

    FMOD_ERRORCHECK( m_pEventSystem->release());
#ifdef VFMOD_SUPPORTS_NETWORK
    if (m_config.bUseNetworkSystem)
      FMOD::NetEventSystem_Shutdown();
#endif
    m_pEventSystem = NULL;
    m_pSystem = NULL;
  }

  if (pMemoryPool)
  {
#ifndef _VISION_XENON
    vMemFree(pMemoryPool);
#else 
    VFmodXenonFree(pMemoryPool, true);
#endif
    pMemoryPool = NULL;
  }

  IVisCallbackDataObject_cl dataAfter(&OnAfterDeinitializeFmod);
  OnAfterDeinitializeFmod.TriggerCallbacks(&dataAfter);
}

// -------------------------------------------------------------------------- //
// Resources                                                
// -------------------------------------------------------------------------- //
VFmodSoundResource* VFmodManager::LoadSoundResource(const char *szFilename, int iUsageFlags)
{
  VASSERT(szFilename!=NULL);

  VFmodSoundResource *pRes = NULL;
  bool bStream = (iUsageFlags & VFMOD_RESOURCEFLAG_STREAM)>0;
  if (!bStream) // streaming creates unique resources
  {
    if (szFilename[0]=='\\' || szFilename[0]=='/') szFilename++; // make absolute filenames comparable
    int iCount = m_pSoundResourceManager->GetResourceCount();
    const unsigned int iFilenameHash = VPathHelper::GetHash(szFilename);
    for (int i=0;i<iCount;i++) 
    {
      pRes = (VFmodSoundResource*)m_pSoundResourceManager->GetResourceByIndex(i);
      if (!pRes || !pRes->GetFilename() || !pRes->GetFilename()[0])
        continue;
      if (!pRes->CompareFileName(szFilename, iFilenameHash)) 
        continue;

      // additionally see whether flags match...
      if (iUsageFlags==pRes->m_iSoundFlags)
        return pRes;
    }
  }

  if (Vision::File.Exists(szFilename))
  {
    // create a new resource (unique for streaming resources)
    pRes = new VFmodSoundResource(m_pSoundResourceManager, iUsageFlags);
    pRes->SetFilename(szFilename);
    pRes->EnsureLoaded();
    return pRes;
  }

  return NULL;
}

FMOD::EventProject* VFmodManager::LoadEventProject(const char *szEventProjectPath)
{
  VASSERT(szEventProjectPath!=NULL && m_pEventSystem!=NULL);

  // make absolute filenames comparable
  if (szEventProjectPath[0]=='\\' || szEventProjectPath[0]=='/') szEventProjectPath++; 

  VStaticString<FS_MAX_PATH> sEventProjectPathNoExt(szEventProjectPath);
  VFileHelper::GetFilenameNoExt(sEventProjectPathNoExt, szEventProjectPath); // ensure, that project-path has no .fdp extension

  // First check, whether event project has been loaded. Please note: Fmod does not support loading 
  // multiple projects with the same name in different directories!
  FMOD::EventProject *pEventProject = NULL;
  const char *szEventProjectName = VPathHelper::GetFilename(sEventProjectPathNoExt);
  m_pEventSystem->getProject(szEventProjectName, &pEventProject);

  // If event project has not been loaded, then try to load it now. First try to load the 
  // platform-specific exported event-project. When that fails, then try to load the 
  // platform-nonspecific version.
  if (!pEventProject)
  {
    // create path to the platform-specific exported event project (.fev) 
    VStaticString<FS_MAX_PATH> sEventProjectPath(sEventProjectPathNoExt);
    VPathHelper::GetFileDir(sEventProjectPathNoExt, sEventProjectPath);
    if(!sEventProjectPath.IsEmpty())
      sEventProjectPath += "/";
    sEventProjectPath += VFMOD_PLATFORM_STR; 
    sEventProjectPath += "/";
    sEventProjectPath += szEventProjectName;
    sEventProjectPath += ".fev"; // add .fev file-extension

    m_pEventSystem->load(sEventProjectPath, 0, &pEventProject);
    if (!pEventProject)
    {
      sEventProjectPath = sEventProjectPathNoExt;
      sEventProjectPath += ".fev"; // add .fev file-extension
      FMOD_WARNINGCHECK(m_pEventSystem->load(sEventProjectPath, 0, &pEventProject));
      if (!pEventProject)
        return NULL;
      Vision::Error.Warning("Fmod Warning: platform-specific %s.fev could not be loaded, fall back to platform-nonspecific version", sEventProjectPath.AsChar());
    }
  }

  return pEventProject;
}

VFmodEventGroup* VFmodManager::LoadEventGroup(const char *szEventProjectPath, const char *szEventGroupName)
{
  VASSERT(szEventProjectPath!=NULL && szEventGroupName!=NULL);

  VFmodEventGroup *pEventGroup = NULL;

  // make absolute filenames comparable
  if (szEventProjectPath[0]=='\\' || szEventProjectPath[0]=='/') szEventProjectPath++; 
  if (szEventGroupName[0]=='\\' || szEventGroupName[0]=='/') szEventGroupName++; 

  // concatenate project-path and group-name into one string separated by '|', in order to be able 
  // to implement VFmodEventGroupManager::CreateResource()
  VStaticString<2*FS_MAX_PATH> sResourceName(szEventProjectPath);
  VFileHelper::GetFilenameNoExt(sResourceName, szEventProjectPath); // ensure, that project-path has no .fdp extension
  sResourceName += "|";
  sResourceName += szEventGroupName;

  int iCount = m_pEventGroupManager->GetResourceCount();
  const unsigned int iResourceNameHash = VPathHelper::GetHash(sResourceName);
  for (int i=0;i<iCount;i++) 
  {
    pEventGroup = (VFmodEventGroup*)m_pEventGroupManager->GetResourceByIndex(i);
    if (!pEventGroup || !pEventGroup->GetFilename() || !pEventGroup->GetFilename()[0])
      continue;
    if (!pEventGroup->CompareFileName(sResourceName, iResourceNameHash)) 
      continue;
    return pEventGroup;
  }
 
  // create a new event group
  pEventGroup = new VFmodEventGroup(m_pEventGroupManager);
  pEventGroup->SetFilename(sResourceName);
  pEventGroup->EnsureLoaded();

  return pEventGroup;
}


// -------------------------------------------------------------------------- //
// Instances                                                
// -------------------------------------------------------------------------- //
VFmodSoundObject* VFmodManager::CreateSoundInstance(const char *szFilename, int iResourceUsageFlags, int iInstanceFlags, int iPriority)
{
  VFmodSoundResource *pRes = LoadSoundResource(szFilename, iResourceUsageFlags);
  if (!pRes)
    return NULL;

  // Note: Create all sound objects here as paused as we need to set AND update the
  // correct distance and attenuation first! Call Play() afterwards it set.
  VFmodSoundObject* pObj = pRes->CreateInstance(hkvVec3(0.0f,0.0f,0.0f), iInstanceFlags | VFMOD_FLAG_PAUSED, iPriority);
  if (pObj)
  {
    // Force sound source position update
    pObj->OnObject3DChanged(VisObject3D_cl::VIS_OBJECT3D_POSCHANGED);

    // Play if set
    if((iInstanceFlags & VFMOD_FLAG_PAUSED) == 0)
      pObj->Play();
  }

  return pObj;
}

VFmodEvent* VFmodManager::CreateEvent(const char *szEventProjectPath, const char *szEventName, int iFlags)
{
  VASSERT(szEventProjectPath!=NULL && szEventName!=NULL); 
  char szEventGroupName[FS_MAX_PATH];
  VFileHelper::GetFileDir(szEventName, szEventGroupName);
  const char *szEventRelativeName = VFileHelper::GetFilename(szEventName);

  VFmodEventGroup *pEventGroup = LoadEventGroup(szEventProjectPath, szEventGroupName);
  if (!pEventGroup)
    return NULL;

  VFmodEvent* pEvent = pEventGroup->CreateEvent(szEventRelativeName, hkvVec3(0.0f,0.0f,0.0f), iFlags | VFMOD_FLAG_PAUSED);
  if (pEvent)
  {
    // Force event position update
    pEvent->OnObject3DChanged(VisObject3D_cl::VIS_OBJECT3D_POSCHANGED);

    // Play if set
    if((iFlags & VFMOD_FLAG_PAUSED) == 0)
      pEvent->Start();
  }

  return pEvent;
}


// -------------------------------------------------------------------------- //
// Channel groups                                               
// -------------------------------------------------------------------------- //
void VFmodManager::SetVolumeAll(float fVol)
{
  if (IsInitialized())
  {
    m_pMasterGroup->setVolume(fVol);
    m_pMusicGroup->setVolume(fVol);
  }
}

void VFmodManager::SetMuteAll(bool bStatus)
{
  if (IsInitialized())
  {
    m_pMasterGroup->setMute(bStatus);
    m_pMusicGroup->setMute(bStatus);

    FMOD::EventCategory *pEventCategory = NULL;   
    FMOD_ERRORCHECK(m_pEventSystem->getCategoryByIndex(-1, &pEventCategory));

    pEventCategory->setMute(bStatus);
  }
}

void VFmodManager::SetPauseAll(bool bStatus)
{
  if (IsInitialized())
  {
    m_pMasterGroup->setPaused(bStatus);
    m_pMusicGroup->setPaused(bStatus);

    FMOD::EventCategory *pEventCategory = NULL;   
    FMOD_ERRORCHECK(m_pEventSystem->getCategoryByIndex(-1, &pEventCategory));

    pEventCategory->setPaused(bStatus);
  }
}


// -------------------------------------------------------------------------- //
// Misc                                           
// -------------------------------------------------------------------------- //
void VFmodManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnEngineInit)
  {
    InitFmodSystem();
    return;
  }

  if(pData->m_pSender == &Vision::Callbacks.OnEngineDeInit)
  {
    // release all resources before the engine deinitializes
    m_soundInstances.Clear(); 
    m_events.Clear();
    m_collisionMeshes.Clear();
    m_reverbs.Clear();

    m_bAnyStopped = false;

    DeInitFmodSystem();
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    RunTick(Vision::GetTimer()->GetTimeDifference());
#ifdef _DEBUG_OUTPUT
    SoundInstances().DebugOutput();
    Events().DebugOutput();
#endif
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnWorldInit)
  {
    if (!IsInitialized())
      return;

    VASSERT(m_pSystem!=NULL);

    // notify the sound engine about the world extents   
    float fRadius = 32000.0f;
    hkvAlignedBBox bbox;
    Vision::GetSceneManager()->GetSceneExtents(bbox);
    if (bbox.isValid())
      fRadius = bbox.getBoundingSphere().m_fRadius; 
    FMOD_ERRORCHECK(m_pSystem->setGeometrySettings(fRadius));

    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    // stop all instances and dispose them if possible (i.e VFMOD_FLAG_NODISPOSE flag not set)
    SoundInstances().StopAll(true); 
    Events().StopAll(true);

    // remove all collision meshes
    CollisionMeshes().Clear(); 

    // remove all reverbs
    Reverbs().Clear();

    m_bAnyStopped = false;
  
    return;
  }

  if (pData->m_pSender==&IVScriptManager::OnScriptProxyCreation)
  {
    VScriptCreateStackProxyObject * pScriptData = (VScriptCreateStackProxyObject *)pData;

    //process data only as far as not handled until now
    if (!pScriptData->m_bProcessed)
    {
      int iRetParams = 0;
      if (pScriptData->m_pInstance->IsOfType(V_RUNTIME_CLASS(VFmodSoundObject)))
        iRetParams = LUA_CallStaticFunction(pScriptData->m_pLuaState,"FireLight","VFmodSoundObject","Cast","O>O",pScriptData->m_pInstance);
      else if (pScriptData->m_pInstance->IsOfType(V_RUNTIME_CLASS(VFmodEvent)))
        iRetParams = LUA_CallStaticFunction(pScriptData->m_pLuaState,"FireLight","VFmodEvent","Cast","O>O",pScriptData->m_pInstance);

      if (iRetParams>0)
      {
        if(lua_isnil(pScriptData->m_pLuaState, -1))   
          lua_pop(pScriptData->m_pLuaState, iRetParams);
        else                                         
          pScriptData->m_bProcessed = true;
      }
    }
    return;
  }

  if(pData->m_pSender==&IVScriptManager::OnRegisterScriptFunctions)
  {
    IVScriptManager* pSM = Vision::GetScriptManager();
    if (pSM)
    {
      lua_State* pLuaState = ((VScriptResourceManager*)pSM)->GetMasterState();
      if(pLuaState)
      {
        lua_getglobal(pLuaState, "Fmod");
        int iType = lua_type(pLuaState, -1);
        lua_pop(pLuaState, 1);

        if(iType!=LUA_TUSERDATA)
        {
          luaopen_FireLight(pLuaState);
          int iRetParams = LUA_CallStaticFunction(pLuaState,"FireLight","VFmodManager","Cast","v>v", &VFmodManager::GlobalManager());
          if (iRetParams==1)
          {
            if(lua_isnil(pLuaState, -1))
            {
              lua_pop(pLuaState, iRetParams);
            }
            else
            {
              lua_setglobal(pLuaState, "Fmod");
              return;
            }
          }
        }
        else
        {
          return; //already loaded
        }
      }
      
      Vision::Error.Warning("Unable to create Lua Fmod Module, lua_State is NULL or cast failed!");
    }
    return;
  }

#if defined(_VISION_MOBILE)
  // Pause Sound when in background
  if (pData->m_pSender == &Vision::Callbacks.OnLeaveForeground)
  {
    if (IsInitialized())
    {
      // From the FMOD Ex documentation:
      // "Channels will not have their per channel pause state overwritten, 
      // so that when a channelgroup is unpaused, the paused state of the channels will 
      // correct as they were set on a per channel basis."
      m_pMasterGroup->getPaused(&m_bMasterGroupPausedInForeground);
      m_pMusicGroup->getPaused(&m_bMusicGroupPausedInForeground);

      m_pMasterGroup->setPaused(true);
      m_pMusicGroup->setPaused(true);

      // Get master event category and pause it as well
      FMOD::EventCategory *pEventCategory = NULL;   
      FMOD_ERRORCHECK(m_pEventSystem->getCategoryByIndex(-1, &pEventCategory));

      pEventCategory->getPaused(&m_bMasterEventCategoryPausedInForeground);
      pEventCategory->setPaused(true);
    }

    return;
  }

  if (pData->m_pSender == &Vision::Callbacks.OnEnterForeground)
  {
    if (IsInitialized())
    {
      m_pMasterGroup->setPaused(m_bMasterGroupPausedInForeground);
      m_pMusicGroup->setPaused(m_bMusicGroupPausedInForeground);

      FMOD::EventCategory *pEventCategory = NULL;   
      FMOD_ERRORCHECK(m_pEventSystem->getCategoryByIndex(-1, &pEventCategory));

      pEventCategory->setPaused(m_bMasterEventCategoryPausedInForeground);
    }

    return;
  }
#endif
}

void VFmodManager::RunTick(float fTimeDelta)
{
  if (!IsInitialized())
    return;

  VISION_PROFILE_FUNCTION(PROFILING_FMOD_OVERALL);

  // profiling scope
  {
    VISION_PROFILE_FUNCTION(PROFILING_FMOD_PUREUPDATE);

    VASSERT(m_pEventSystem!=NULL);
    
    // update Fmod listener attributes
    VisObject3D_cl *pListener = m_pListenerObject ? m_pListenerObject : Vision::Camera.GetMainCamera();
    if (!pListener)
      return;

    hkvVec3 vCamPos = pListener->GetPosition();
    hkvVec3 vDir(pListener->GetObjDir()),
            vRight(pListener->GetObjDir_Right()),
            vUp(pListener->GetObjDir_Up());

    vUp = -vUp; // compensate for coordinate system
    m_pEventSystem->set3DListenerAttributes(0, (FMOD_VECTOR *)&vCamPos, NULL, (FMOD_VECTOR *)&vDir, (FMOD_VECTOR *)&vUp); // no speed (yet)
    
    
    // update all sound objects 
    SoundInstances().Update();

    // update all events 
    Events().Update();

    // update Fmod event system
    m_fTimeLeftOver += fTimeDelta;
    if (m_fTimeLeftOver > m_config.fTimeStep)
    { 
      m_pEventSystem->update();
#ifdef VFMOD_SUPPORTS_NETWORK
      if (m_config.bUseNetworkSystem)
        FMOD::NetEventSystem_Update();
#endif
      m_fTimeLeftOver = hkvMath::mod (m_fTimeLeftOver, m_config.fTimeStep);
    }
  } 
  
  // do not purge sounds/ events in vForge, in order to allow toggling playback via hotspot button
  if (Vision::Editor.IsInEditor())
    return;  

  if (m_bAnyStopped)
  {
    VISION_PROFILE_FUNCTION(PROFILING_FMOD_PURGE);

    // all sounds/ events that have finished playing are removed from handling
    SoundInstances().PurgeNotPlaying();
    Events().PurgeNotPlaying();

    m_bAnyStopped = false; // reset any stopped flag
  }
}

bool VFmodManager::GetMemoryStats(int *pCurrentMemSize, int *pMaxMemSize, bool bBlocking) const
{
  if (!IsInitialized())
    return false;
  FMOD_RESULT result = FMOD::Memory_GetStats(pCurrentMemSize, pMaxMemSize, bBlocking);
  return (result==FMOD_OK);
}

bool VFmodManager::SetAmbientReverbProperties(VFmodReverbProps &properties)
{
  if (!IsInitialized())
    return false;
  FMOD_REVERB_PROPERTIES prop = properties;
  FMOD_RESULT result = m_pEventSystem->setReverbAmbientProperties(&prop);
  return (result==FMOD_OK);
}

int VFmodManager::GetActiveSourceVoiceCount() const
{
  int iActiveSourceCount = 0;

  int iSoundInstanceCount = m_soundInstances.Count();
  for (int i=0;i<iSoundInstanceCount;i++)
  {
    VFmodSoundObject* pInst = m_soundInstances.GetAt(i);
    if (pInst->IsPlaying())
      iActiveSourceCount++;
  }

  int iEventCount = m_events.Count();
  for (int i=0;i<iEventCount;i++)
  {
    VFmodEvent* pEvent = m_events.GetAt(i);
    if (pEvent->IsPlaying())
      iActiveSourceCount++;
  }

  return iActiveSourceCount;
}

int VFmodManager::GetExistingSourceVoiceCount() const
{ 
  return (m_soundInstances.Count() + m_events.Count());
}

#if defined (WIN32) && !defined(_VISION_WINRT)

bool VFmodManager::GetDesignerPath(VString &sDesignerPath) const
{
  // first retrieve version string
  char szVersion[16];
  sprintf(szVersion,"%x.%.2x.%.2x", (FMOD_VERSION >> 16) & 0xFFFF, (FMOD_VERSION >> 8) & 0xFF, (FMOD_VERSION >> 0) & 0xFF);

  // try to find the designer in the installed Vision SDK first
  sDesignerPath = VFMOD_DESIGNER_INSTALLED_VISION_DIR;  
  sDesignerPath += szVersion;
  sDesignerPath += VFMOD_DESIGNER_EXE_FOLDER;
  sDesignerPath += VFMOD_DESIGNER_EXE;
  if (VFileHelper::Exists(sDesignerPath))
    return true;

  const char* szThirdPartyPath = getenv("HAVOK_THIRDPARTY_DIR");
  if (!szThirdPartyPath)
    return false;

  sDesignerPath = szThirdPartyPath;
  sDesignerPath += VFMOD_DESIGNER_DIR;  
  sDesignerPath += szVersion;
  sDesignerPath += VFMOD_DESIGNER_EXE_FOLDER;
  sDesignerPath += VFMOD_DESIGNER_EXE;

  return true;
}

#endif

// -------------------------------------------------------------------------- //
// Fmod file callbacks                                                
// -------------------------------------------------------------------------- //
FMOD_RESULT F_CALLBACK VisionFM_Open(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
{
  IVFileInStream *pStream = Vision::File.Open(name);
  if (!pStream)
    return FMOD_ERR_FILE_NOTFOUND;

  *filesize = pStream->GetSize();
  *handle = pStream;

  return FMOD_OK;
}

FMOD_RESULT F_CALLBACK VisionFM_Close(void *handle, void *userdata)
{
  if (!handle)
    return FMOD_ERR_INVALID_PARAM;

  IVFileInStream *pStream = (IVFileInStream *)handle;
  pStream->Close();

  return FMOD_OK;
}

FMOD_RESULT F_CALLBACK VisionFM_Read(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
{
  if (!handle)
    return FMOD_ERR_INVALID_PARAM;

  IVFileInStream *pStream = (IVFileInStream *)handle;
  *bytesread = (unsigned int)pStream->Read(buffer, sizebytes);
  if (*bytesread == 0)
    return FMOD_ERR_FILE_EOF;

  return FMOD_OK;
}


FMOD_RESULT F_CALLBACK VisionFM_Seek(void *handle, unsigned int pos, void *userdata)
{
  if (!handle)
    return FMOD_ERR_INVALID_PARAM;

  IVFileInStream *pStream = (IVFileInStream *)handle;
  if (!pStream->SetPos(pos, VFS_SETPOS_SET))
    return FMOD_ERR_FILE_COULDNOTSEEK;

  return FMOD_OK;
}


// -------------------------------------------------------------------------- //
// Fmod memory callbacks                                           
// -------------------------------------------------------------------------- //
void* F_CALLBACK VisionFM_Alloc(unsigned int size, FMOD_MEMORY_TYPE type, const char *sourcestr)
{
#ifndef _VISION_XENON
  void* ptr = vMemAlloc(size);
  g_FmodAllocations.SetAt(ptr, size);
  return ptr;
#else
  return VFmodXenonAlloc(size, (type & FMOD_MEMORY_XBOX360_PHYSICAL)!=0);
#endif
}

void* F_CALLBACK VisionFM_Realloc(void *ptr, unsigned int size, FMOD_MEMORY_TYPE type, const char *sourcestr)
{
#ifndef _VISION_XENON
  if (ptr)
  {
    unsigned int iOldSize = g_FmodAllocations[ptr];
    g_FmodAllocations.RemoveKey(ptr);

    void* pNewPtr = vMemAlloc(size);
    g_FmodAllocations.SetAt(pNewPtr, size);

    memcpy(pNewPtr, ptr, iOldSize < size ? iOldSize : size);
    vMemFree(ptr);

    return pNewPtr;
  }
  else
  {
    void* ptr = vMemAlloc(size);
    g_FmodAllocations.SetAt(ptr, size);
    return vMemAlloc(size);
  }
#else
  return VFmodXenonRealloc(ptr, size, (type & FMOD_MEMORY_XBOX360_PHYSICAL)!=0);
#endif
}

void F_CALLBACK VisionFM_Free(void *ptr, FMOD_MEMORY_TYPE type, const char *sourcestr)
{
#ifndef _VISION_XENON
  g_FmodAllocations.RemoveKey(ptr);
  vMemFree(ptr);
#else
  VFmodXenonFree(ptr, (type & FMOD_MEMORY_XBOX360_PHYSICAL)!=0);
#endif
}


// -------------------------------------------------------------------------- //
// Fmod error handling                                              
// -------------------------------------------------------------------------- //
void FMOD_ErrorCheck(FMOD_RESULT result, bool bFatal, int line, const char *szFile)
{
  if (result == FMOD_OK)
    return;

  if (bFatal)
  {
    Vision::Error.FatalError("Fmod Error %d in line %i:\n%s", result, line, FMOD_ErrorString(result));

    // it is allowed for certain functions to fail, for example under sound stress-tests Fmod may fail to find a free channel
    // this is NOT an error and thus should not break the code
    VASSERT(!"Error triggered in Fmod binding. Check log for details");
  }

  Vision::Error.Warning("Fmod Error %d in line %i:\n%s", result, line, FMOD_ErrorString(result));
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
