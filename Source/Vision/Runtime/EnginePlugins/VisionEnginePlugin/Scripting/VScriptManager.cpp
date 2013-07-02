/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/RSDClient/VRSDClient.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/RSDClient/VRSDLuaImplementation.hpp>
#include <Vision/Runtime/Base/RemoteComm/VTarget.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiObjectComponentCollection.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

int PROFILING_SCRIPTING       = 0;
int PROFILING_SCRIPTOBJ_TICK  = 0;
int PROFILING_SCRIPTOBJ_EXECUTEFUNCTION = 0;
int PROFILING_SCRIPTOBJ_CREATETHREAD = 0;
int PROFILING_SCRIPTOBJ_DISCARDTHREAD = 0;

int VScriptResourceManager::g_iThreadsCreated = 0;
int VScriptResourceManager::g_iFunctionsCalled = 0;
int VScriptResourceManager::g_iThreadsRecycled = 0;
int VScriptResourceManager::g_iFunctionsFailed = 0;

VisCallback_cl VScriptResourceManager::OnUserDataSerialize;

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=552
#endif

#if defined(__ghs__)
#pragma ghs nowarning 550
#endif

#if defined(WIN32) && !defined(_VISION_WINRT)
static bool bIgnoreErrors = false;
#endif

#if defined(__SNC__)
#pragma diag_pop
#endif

#if defined(__ghs__)
#pragma ghs endnowarning
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static helper functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

VScriptComponent* VScriptResourceManager::GetScriptComponent( VisTypedEngineObject_cl *pObj)
{
  VScriptComponent* pComp = pObj->Components().GetComponentOfType<VScriptComponent>();
  if (!pComp)
  {
    pComp = V_CREATE_INSTANCE(VScriptComponent);
    pObj->AddComponent(pComp);
  }
  return pComp;
}

VScriptComponent* VScriptResourceManager::HasScriptComponent( VisTypedEngineObject_cl *pObj)
{
  if (!pObj)
    return NULL;
  VScriptComponent* pComp = pObj->Components().GetComponentOfType<VScriptComponent>();
  return pComp;
}


IVScriptInstance* VScriptResourceManager::GetScriptInstance( VisTypedEngineObject_cl *pObj)
{
  VScriptComponent* pComp = HasScriptComponent( pObj);
  if (!pComp)
    return NULL;
  return (VScriptInstance*) pComp->GetScriptInstance();
}

void VScriptResourceManager::SetScriptInstance( VisTypedEngineObject_cl *pObj, IVScriptInstance* _pInst)
{
  VScriptInstancePtr spInst = (VScriptInstance*) _pInst;
  if (!spInst)
  {
    if (!HasScriptComponent(pObj))
      return;
  }
  else
  {
    //pInst->AddRef(); //so detaching doesn't free the object
    
    //detach the instance if already attached to an component
    VScriptComponent* pOldParentComponent = (VScriptComponent*) spInst->GetParentComponent();
    if (pOldParentComponent)
      pOldParentComponent->SetScriptInstance(NULL);
  }

  //get the target component
  VScriptComponent* pComp = VScriptResourceManager::GetScriptComponent(pObj);
  pComp->SetScriptInstance(spInst);
  if (spInst)
  {
    VASSERT(spInst->GetParentComponent() == pComp);
//    if (pInst)
//      pInst->Release();
   }
}








bool VScriptResourceManager::LuaErrorCheck(lua_State *L, int status, IVLog *pLog) 
{
  if ( status==0 || status==LUA_YIELD )
    return true;

  const char *szErrorLatin1 = lua_tostring(L, -1);

  // LUA does not natively support unicode (utf8). Assume LATIN-1 encoding.
  VMemoryTempBuffer<1024> tmpBuffer;
  const int iErrorStrSize = VString::ConvertLatin1ToUTF8(szErrorLatin1, 
    static_cast<int>(strlen(szErrorLatin1)), NULL, 0);
  tmpBuffer.EnsureCapacity(iErrorStrSize+1);
  char *szError = reinterpret_cast<char*>(tmpBuffer.GetBuffer());

  VString::ConvertLatin1ToUTF8(szErrorLatin1, static_cast<int>(strlen(szErrorLatin1)), 
    szError, iErrorStrSize);
  szError[iErrorStrSize] = '\0';

  if (pLog) 
    pLog->Error(szError);
  else
    Vision::Error.Warning("Lua Error : %s",szError);

  lua_pop(L, 1); // remove error message

#if defined(WIN32) && !defined(_VISION_WINRT)

  // If a script error occurs and the user is currently in the editor we ask him if he wants to launch the script debugger
  // We give him the option to ignore the error as well as all future errors in this session
  //
  // Note: Skip syntax errors, since we won't get a callstack for them.
  if(!VRSDClient::GetGlobalClient().IsConnected() && Vision::Editor.IsInEditor() && status != LUA_ERRSYNTAX && Vision::Editor.IsAnimatingOrPlaying())
  {
    // Ask the user if he wants to connect to the remote debugger
    VString Setting;
    bool bShouldAsk = true;
    if(Vision::Game.GetSettingsMap().Get("AskForDebuggerOnScriptError", Setting))
    {
      if(!_stricmp(Setting, "false"))
        bShouldAsk = false;
    }

    if(!bIgnoreErrors && bShouldAsk)
    {
      VString ErrorMsg("A Lua script error occurred!\n\n");
      ErrorMsg += szError;
      ErrorMsg += "\n\nPress 'Retry' to start the script debugger.\nPress 'Abort' to ignore the error once.\nPress 'Ignore' to ignore the error for this run.";

      int RetValue = MessageBox(Vision::Video.GetCurrentConfig()->m_hWnd, ErrorMsg, "Script Error", MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION | MB_DEFBUTTON2);

      if(RetValue == IDIGNORE)
        bIgnoreErrors = true;
      else if(RetValue == IDABORT)
        return false;
      else
      {
        // make sure the target system is initialized
        VTargetThread::Init();

        // Start the remote debugger application
        const char* pProjectPath = Vision::Editor.GetProjectPath();
        const char* pExecutablePath = Vision::Editor.GetExecutablePath();
        if(pProjectPath && pExecutablePath)
        {
          VString Executable = VFileHelper::CombineDirAndFile(pExecutablePath, "vRSD.exe");

          VString CommandLine(" \"");
          CommandLine += pProjectPath;
          CommandLine += "\" -connect-to-localhost -port 4224";

          STARTUPINFO si;
          PROCESS_INFORMATION pi;
          memset(&si, 0, sizeof(STARTUPINFO));
          memset(&pi, 0, sizeof(PROCESS_INFORMATION));
          si.cb = sizeof(STARTUPINFO);

          ANALYSIS_IGNORE_WARNING_ONCE(6335)
          if(CreateProcess(Executable, CommandLine.GetChar(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
          {
            if(!VTargetThread::WaitForConnection("VRSD", 20))
            {
              MessageBox(NULL, "Debugger didn't react during timeout value.\nContinuing execution.", "Debugger Error", MB_OK | MB_ICONEXCLAMATION);
            }
          }
          else
          {
            MessageBox(NULL, "Couldn't start debugger process.", "Debugger Error", MB_OK | MB_ICONEXCLAMATION);
          }
        }
      }
    }
  }

#endif

  // Send the error event to the remote debugger if connected
  if(VRSDClient::GetGlobalClient().IsConnected())
  {
    // get information about activation from lua
    lua_Debug AR;
    
    if(lua_getstack(L, 0, &AR))
    {
      if(lua_getinfo(L, "nSl", &AR))
      {
        int Line = AR.currentline;
        int LineDefined = AR.linedefined;
        VRSDScriptEvent::ExecutionType ExecType = !strcmp(AR.what, "Lua") ? VRSDScriptEvent::EXECUTION_TYPE_SCRIPT : VRSDScriptEvent::EXECUTION_TYPE_NATIVE;
        char* FileName = (char*)AR.source;
        char* FunctionName = (char*)AR.name;
        VRSDScriptEvent Event(szError, Line, ExecType, FileName, FunctionName, LineDefined);

        if(VRSDClient::GetGlobalClient().GetClientLanguageImplementation()->IsImplementation("Lua"))
        {
          VRSDClientLuaImplementation* CLI = ((VRSDClientLuaImplementation*)VRSDClient::GetGlobalClient().GetClientLanguageImplementation());
          CLI->SetDebugInformation(L, &AR);
          CLI->ErrorScriptEvent(&Event);
        }
      }
    }


    // exit early because the error handling happened in the remote debugger
    return false;
  }
  

  // break of execution happens in the editor

  VScriptErrorDataObject data(&IVScriptManager::OnError);
  data.m_pScriptObject = GetScriptInstanceForState(L, false);
  if( data.m_pScriptObject )
    data.m_pScriptResource = data.m_pScriptObject->GetResource();


  // Change to support Lua error stack tracing
  data.m_iStackDepth = 0;
  DynObjArray_cl<lua_Debug> stack;
  while( lua_getstack( L, data.m_iStackDepth, &stack[data.m_iStackDepth] ) )
  {
    if( lua_getinfo( L, "nSl", &stack[data.m_iStackDepth] ) )
    {
      //Vision::Error.Warning("Lua-Debug [%d]: Line(%d) Name(%s) Function defined @(%d) Source(%s)",data.m_iStackDepth,stack[data.m_iStackDepth].currentline,stack[data.m_iStackDepth].name,stack[data.m_iStackDepth].linedefined,stack[data.m_iStackDepth].source);
    }
    data.m_iStackDepth++;
  }

  data.m_pStackInfo = stack.GetDataPtr();
  data.m_szErrorMessage = szError;
  IVScriptManager::OnError.TriggerCallbacks(&data);

  return false;
}


void VScriptResourceManager::SetScriptInstanceForState(lua_State* pLuaState, VScriptInstance* pObj)
{  
  //this code makes it possible to access the script resource by the Lua state
  //(which can be shared by all objects having the same script
  VASSERT_MSG(pLuaState!=NULL, "Supplied Lua state is not valid!");

                                                  // stack: ..., TOP
  lua_pushlightuserdata(pLuaState, pLuaState);    // stack: ..., KEY(ptr to L), TOP
  lua_pushlightuserdata(pLuaState, pObj);         // stack: ..., KEY(ptr to L), VALUE(ptr to obj), TOP
  lua_settable(pLuaState, LUA_GLOBALSINDEX);      // stack: ..., TOP
}


VScriptInstance* VScriptResourceManager::GetScriptInstanceForState(lua_State* pLuaState, bool CheckIfValidInstance /*=true*/)
{
  VASSERT_MSG(pLuaState!=NULL, "Supplied Lua state is not valid!");
                                                  // stack: ..., TOP
  lua_pushlightuserdata(pLuaState, pLuaState);    // stack: ..., KEY(ptr to L), TOP
  lua_gettable(pLuaState, LUA_GLOBALSINDEX);      // stack: ..., VALUE(ptr to obj), TOP
  VScriptInstance *pScriptObj = (VScriptInstance*)lua_touserdata(pLuaState, -1);

  #ifdef HK_DEBUG_SLOW
    if( CheckIfValidInstance )
    {
      VASSERT(pScriptObj);
    }
  #endif

  //be nice... clean up the stack
  lua_pop(pLuaState, 1);                          // stack: ..., TOP

  return pScriptObj;
}


void VScriptResourceManager::DiscardThread(lua_State* pLuaState)
{ 
  //cleans data created in SetScriptInstanceForState
  if(pLuaState==NULL)
    return;
                                                  // stack: ..., TOP
  lua_pushlightuserdata(pLuaState, pLuaState);    // stack: ..., KEY(ptr to L), TOP
  lua_pushnil(pLuaState);                         // stack: ..., KEY(ptr to L), nil TOP
  lua_settable(pLuaState, LUA_GLOBALSINDEX);      // stack: ..., TOP
}


VScriptResourceManager VScriptResourceManager::g_GlobalManager;

VScriptResourceManager::VScriptResourceManager()
  : VisResourceManager_cl("Scripts", VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER)
  , IVScriptManager()
  , m_Instances()
  , m_pMasterState(NULL)
  , m_bInitialized(false)
  , m_iGameScriptFunctions(0)
  , m_iSceneScriptFunctions(0)
{
}

VScriptResourceManager::~VScriptResourceManager()
{
}


VScriptResourceManager& VScriptResourceManager::GlobalManager()
{
  return g_GlobalManager;
}

//Allocator for LUA
static void* vision_alloc(void *ud, void *ptr, size_t osize, size_t nsize) 
{
  (void)ud;
  (void)osize;
  if (nsize == 0) 
  {
    VBaseDealloc(ptr);
    return NULL;
  }
  else
  {
    void* pNewPtr = VBaseAlloc(nsize);
    memcpy(pNewPtr, ptr, osize < nsize ? osize : nsize);
    VBaseDealloc(ptr);
    return pNewPtr;
  }
}

//Panic handler for LUA
static int vision_panic (lua_State *L)
{
  (void)L;  /* to avoid warnings */
  
  Vision::Error.Warning("LUA PANIC: unprotected error in call to Lua API (%s)\n",
                   lua_tostring(L, -1));

  VASSERT(false); //get a break point
  return 0;
}

// debug hook function
static void Lua_DebugHook( lua_State *L, lua_Debug *ar )
{
  // get information from lua, so we can fill our debug event structure
  if( lua_getinfo( L, "nSl", ar ) )
  {
    VScriptDebugEventObject data(&IVScriptManager::OnDebugEvent);
    data.m_iCurrentLine = ar->currentline;
    data.m_iLineOfFuncDefinition = ar->linedefined;
    data.m_szCurrentFunction = ar->name;
    data.m_szSource = ar->source;
    data.m_iEventType = ar->event;

    // Change to support Lua error stack tracing
    data.m_iStackDepth = 0;
    DynObjArray_cl<lua_Debug> stack;
    while( lua_getstack( L, data.m_iStackDepth, &stack[data.m_iStackDepth] ) )
    {
      if( lua_getinfo( L, "nSl", &stack[data.m_iStackDepth] ) )
      {
        //Vision::Error.Warning("Lua-Debug [%d]: Line(%d) Name(%s) Function defined @(%d) Source(%s)",data.m_iStackDepth,stack[data.m_iStackDepth].currentline,stack[data.m_iStackDepth].name,stack[data.m_iStackDepth].linedefined,stack[data.m_iStackDepth].source);
      }
      data.m_iStackDepth++;
    }

    data.m_pStackInfo = stack.GetDataPtr();

    IVScriptManager::OnDebugEvent.TriggerCallbacks(&data);
  }
}

void VScriptResourceManager::OneTimeInit()
{
  if(m_bInitialized)
    return;

  Vision::SetScriptManager(this);
  Vision::ResourceSystem.RegisterResourceManager(this, VColorRef(200,200,200));
  Vision::Callbacks.OnWorldDeInit += this;
  Vision::Callbacks.OnBeforeSceneLoaded += this;
  Vision::Callbacks.OnAfterSceneLoaded += this;
  Vision::Callbacks.OnAfterSceneUnloaded += this;
  Vision::Callbacks.OnBeforeSceneUnloaded += this;
#if defined(WIN32)
  Vision::Callbacks.OnEditorModeChanged += this;
#endif
  Vision::Callbacks.OnUpdateSceneBegin += this;
  Vision::Callbacks.OnUpdateSceneFinished += this;
  Vision::Callbacks.OnEngineDeInitializing += this;
  Vision::Callbacks.OnEngineInit += this;
  Vision::Callbacks.OnVideoChanged +=this;

  // profiling:
  if (!PROFILING_SCRIPTING)
  {
    PROFILING_SCRIPTING = Vision::Profiling.GetFreeElementID();
    PROFILING_SCRIPTOBJ_TICK = Vision::Profiling.GetFreeElementID();
    PROFILING_SCRIPTOBJ_EXECUTEFUNCTION = Vision::Profiling.GetFreeElementID();
    PROFILING_SCRIPTOBJ_CREATETHREAD = Vision::Profiling.GetFreeElementID();
    PROFILING_SCRIPTOBJ_DISCARDTHREAD = Vision::Profiling.GetFreeElementID();
    Vision::Profiling.AddGroup("Scripting");
    VProfilingNode *pOverall = Vision::Profiling.AddElement(PROFILING_SCRIPTING,   "Scripting Overall", TRUE);
      Vision::Profiling.AddElement(PROFILING_SCRIPTOBJ_TICK,   "Script instance tick", TRUE, pOverall);
      Vision::Profiling.AddElement(PROFILING_SCRIPTOBJ_EXECUTEFUNCTION,   "Execute script function", TRUE, pOverall);
      Vision::Profiling.AddElement(PROFILING_SCRIPTOBJ_CREATETHREAD,      "Create thread", TRUE, pOverall);
      Vision::Profiling.AddElement(PROFILING_SCRIPTOBJ_DISCARDTHREAD,     "Discard thread", TRUE, pOverall);
  }

  // create the master state
  m_pMasterState = lua_newstate(&vision_alloc, NULL);
  VASSERT_MSG(m_pMasterState, "Creation of Lua master state failed!");

  lua_atpanic(m_pMasterState, &vision_panic); //Install an error handler

  // TODO: Have a flag so we can disable debug hooking even in editor mode for performance?
  if (Vision::Editor.IsInEditor())
    lua_sethook( m_pMasterState, Lua_DebugHook, LUA_MASKRET | LUA_MASKCALL | LUA_MASKLINE, 0 );

  Vision::Error.SystemMessage("Scripting: Open master state");

	// open all lua base libraries
  luaL_openlibs(m_pMasterState);

  // Vision specific libs
  VLUA_OpenLibraries(m_pMasterState);


  VRSDClient::GetGlobalClient().RegisterCallbacks();
  VRSDClient::GetGlobalClient().SetClientLanguageImplementation(new VRSDClientLuaImplementation());

  m_bInitialized = true;
}

void VScriptResourceManager::OneTimeDeInit()
{
  if(!m_bInitialized)
    return;

  SetSceneScript(NULL);
  SetGameScript(NULL);
  m_Instances.Clear();
  PurgeUnusedResources();
  
  Vision::ResourceSystem.UnregisterResourceManager(this);
  Vision::Callbacks.OnWorldDeInit -= this;
  #if defined(WIN32)
  Vision::Callbacks.OnEditorModeChanged -= this;
  #endif
  Vision::Callbacks.OnUpdateSceneBegin -= this;
  Vision::Callbacks.OnUpdateSceneFinished -= this;
  Vision::Callbacks.OnEngineDeInitializing -= this;
  Vision::Callbacks.OnBeforeSceneLoaded -= this;
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  Vision::Callbacks.OnAfterSceneUnloaded -= this;
  Vision::Callbacks.OnBeforeSceneUnloaded -= this;
  Vision::Callbacks.OnEngineInit -= this;
  Vision::Callbacks.OnVideoChanged -=this;
  VRSDClient::GetGlobalClient().UnregisterCallbacks();
  VRSDClient::GetGlobalClient().SetClientLanguageImplementation( NULL );

  Vision::Error.SystemMessage("Scripting: Close master state");

  // deinit debug hook
  lua_sethook( m_pMasterState, Lua_DebugHook, 0, 0 );

  // close the master state
  LUA_STACK_DUMP(m_pMasterState);
  lua_close(m_pMasterState);
  m_pMasterState = NULL; //remember that we closed the master state!

  if (Vision::GetScriptManager()==this)
    Vision::SetScriptManager(NULL);

  m_bInitialized = false;
}


VScriptResource* VScriptResourceManager::LoadScriptFile(const char *szFilename)
{
  if (!szFilename || !szFilename[0])
    return NULL;
  VScriptResource *pRes = (VScriptResource *)GetResourceByName(szFilename);
  if (!pRes)
  {
    pRes = new VScriptResource(this);
    pRes->SetFilename(szFilename);
  }
  pRes->EnsureLoaded();
  return pRes;
}


void VScriptResourceManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  VISION_PROFILE_FUNCTION(PROFILING_SCRIPTING);  //TODO: Different element for tracking callbacks?

  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneBegin)
  {
    if (!IsPaused()&&(!Vision::Editor.IsInEditor()||Vision::Editor.GetMode()>=(int)VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR))
    {
      float dtime = Vision::GetTimer()->GetTimeDifference();
      
      //TODO: Update scripts at a lower rate than native objects?
      // m_fPassedTime += dtime;
      // while (m_fPassedTime> m_fScriptTimestep)
      
      // handle all objects that have at least one waiting thread:
      if (m_Instances.m_iWaitingObjectCount>0)
        m_Instances.HandleWaitingObjects(dtime);
      if (m_Instances.m_iSuspendedObjectCount>0)
        m_Instances.HandleSuspendedObjects(dtime);

      // remove "dead" script instances
      if (m_Instances.m_bAnyFlaggedForDisposal)
        m_Instances.RemoveFlagged();

      if (Vision::Profiling.GetDebugRenderFlags()&DEBUGRENDERFLAG_SCRIPTSTATISTICS)
        ShowDebugInfo(Vision::Contexts.GetMainRenderContext()->GetRenderInterface());

      if (m_Instances.Count()>0)
      {
        // see whether we have to call the script thinking
        if (m_fThinkingInterval>0.f)
        {
          // every x seconds
          m_fThinkingPos += dtime;
          while (m_fThinkingPos>=m_fThinkingInterval)
          {
            m_fThinkingPos-=m_fThinkingInterval;
            VSCRIPT_THINK_CALLBACK.TriggerCallbacks();
          }
        } else
        {
          // every frame
          VSCRIPT_THINK_CALLBACK.TriggerCallbacks();
        }
      }

      if (m_iGameScriptFunctions&VSCRIPT_FUNC_ONUPDATESCENEBEGIN)
        m_spGameScript->ExecuteFunction("OnUpdateSceneBegin");
      if (m_iSceneScriptFunctions&VSCRIPT_FUNC_ONUPDATESCENEBEGIN)
        m_spSceneScript->ExecuteFunction("OnUpdateSceneBegin");
    } // !IsPaused
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    if (!IsPaused()&&(!Vision::Editor.IsInEditor()||Vision::Editor.GetMode()>=(int)VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR))
    {
      if (m_iGameScriptFunctions&VSCRIPT_FUNC_ONUPDATESCENEFINISHED)
        m_spGameScript->ExecuteFunction("OnUpdateSceneFinished");
      if (m_iSceneScriptFunctions&VSCRIPT_FUNC_ONUPDATESCENEFINISHED)
        m_spSceneScript->ExecuteFunction("OnUpdateSceneFinished");
    }
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    int iRemaining = m_Instances.Count();
    
    if (m_Instances.m_bAnyFlaggedForDisposal)
      m_Instances.RemoveFlagged(); // almost all should be flagged
    iRemaining = m_Instances.Count();
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnVideoChanged)
  {
    if (!IsPaused() && (!Vision::Editor.IsInEditor() || Vision::Editor.GetMode() >= (int) VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR))
    {
      if (m_iGameScriptFunctions & VSCRIPT_FUNC_ONVIDEOCHANGED)
        m_spGameScript->ExecuteFunction("OnVideoChanged");
      if (m_iSceneScriptFunctions & VSCRIPT_FUNC_ONVIDEOCHANGED)
        m_spSceneScript->ExecuteFunction("OnVideoChanged");
    }
  }

  // The script manager should only react to editor mode changes on Windows (e.g. in vForge)
  // The console clients get the change message only once (to change from not playing to playing mode)
  // thus cleanup of things isn't necessary and changes variables created by OnSerialize calls
#if defined(WIN32)
  if (pData->m_pSender==&Vision::Callbacks.OnEditorModeChanged)
  {
    // any resources changed by the editor?
    VisEditorModeChangedDataObject_cl *pModeObj = (VisEditorModeChangedDataObject_cl *)pData;
    if (pModeObj->m_eNewMode==VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR
     || pModeObj->m_eNewMode==VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME)
    {
      ReloadModifiedResourceFiles(Vision::File.GetManager(), TRUE, TRUE);

      //dump globals of all resources
      int iResourceCount = GetResourceCount();
      int iActualResourceCount = 0;
      int iNumGlobals = 0;
      for(int i = 0; i < iResourceCount; i++)
      {
        VScriptResource* pResource = static_cast<VScriptResource*>(GetResourceByIndex(i));
        if (pResource == NULL)
          continue;

        iNumGlobals += DumpVisionGlobals(pResource->m_pResourceState);
        ++iActualResourceCount;
      }
      
      //dump overall globals
      iNumGlobals += DumpVisionGlobals(m_pMasterState);

      if(iResourceCount>0)
        Vision::Error.SystemMessage("Scripting: Dumped unused globals in %d resources, %d globals are still in use.", iActualResourceCount, iNumGlobals);

      //also reset the mapping of obj-pointer - to - wrapper
      LUA_ResetWrapperLookupTable(m_pMasterState);

#if defined(WIN32) && !defined(_VISION_WINRT)
      bIgnoreErrors = false;
#endif
    }
    return;
  }
#endif

  if (pData->m_pSender==&Vision::Callbacks.OnEngineDeInitializing)
  {
    SetSceneScript(NULL);
    SetGameScript(NULL);
    m_Instances.Clear();
    PurgeUnusedResources();
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnBeforeSceneLoaded)
  {
    //perform a full garbage collection cycle before loading the scene
    //(getting rid of object generated in 'OnExpose')
    if(m_pMasterState)
      lua_gc(m_pMasterState,LUA_GCCOLLECT, 0);

    if (m_spGameScript)  m_spGameScript->ExecuteFunction("OnBeforeSceneLoaded");
    if (m_spSceneScript) m_spSceneScript->ExecuteFunction("OnBeforeSceneLoaded");

    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneLoaded)
  {
    if (m_spGameScript)  m_spGameScript->ExecuteFunction("OnAfterSceneLoaded");
    if (m_spSceneScript) m_spSceneScript->ExecuteFunction("OnAfterSceneLoaded");

    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnBeforeSceneUnloaded)
  {
    if (m_spGameScript)  m_spGameScript->ExecuteFunction("OnBeforeSceneUnloaded");
    if (m_spSceneScript) m_spSceneScript->ExecuteFunction("OnBeforeSceneUnloaded");

    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneUnloaded)
  {
    if (m_spGameScript)  m_spGameScript->ExecuteFunction("OnAfterSceneUnloaded");
    if (m_spSceneScript) m_spSceneScript->ExecuteFunction("OnAfterSceneUnloaded");

    //perform a full garbage collection cycle after the scene is unloaded
    if(m_pMasterState)
      lua_gc(m_pMasterState,LUA_GCCOLLECT, 0);

    return;
  }
  if (pData->m_pSender==&Vision::Callbacks.OnEngineInit)
  {
    // CALLBACK to register all other libs...
    OnRegisterScriptFunctions.TriggerCallbacks(NULL);
  }
}

/*
lua_State* VScriptResourceManager::GetParentState()
{
  // first check per-scene script
  VScriptInstance *p = (VScriptInstance *)m_spSceneScript.m_pPtr;
  if (p && p->m_spResource && p->m_spResource->m_pResourceState)
    return p->m_spResource->m_pResourceState;

  // next the master script file
  p = (VScriptInstance *)m_spMasterScript.m_pPtr;
  if (p && p->m_spResource && p->m_spResource->m_pResourceState)
    return p->m_spResource->m_pResourceState;

  // finally the global main state
  return GetMasterState();
}
*/

int VScriptResourceManager::DumpVisionGlobals(lua_State *L) const
{
  if(L==NULL)
    return 0;

  lua_pushnil(L);                                   // stack : ..., nil, TOP

  int iNumLeftGlobals = 0;

  while (lua_next(L, LUA_GLOBALSINDEX) != 0)        // stack : ..., key, value, TOP
  {
    //we don't need the value...
    lua_pop(L, 1);                                  // stack : ..., key, TOP

    int iType = lua_type(L, -1);
    //vision globals always have string names
    if (iType==LUA_TSTRING)
    {  
      const char* pSymbolName = lua_tostring(L, -1);

      //filter "$node" prefix (that's what we are looking for)
      if(pSymbolName==strstr(pSymbolName, LUA_HIDDEN_GLOBAL_PREFIX))
      {
        lua_pushnil(L);                             // stack : ..., key, nil, TOP
        lua_setglobal(L,pSymbolName);               // stack : ..., key, TOP
      }
      else
      {
        iNumLeftGlobals++;
      }
    }
    else
    {
      iNumLeftGlobals++;
    }
  }                                                 // stack : ..., TOP

  return iNumLeftGlobals;
}

IVScriptInstance* VScriptResourceManager::CreateScriptInstanceFromFile(const char *szFilename)
{
  if (!szFilename || !szFilename[0])
    return NULL;
  VScriptResource *pScriptRes = LoadScriptFile(szFilename);
  if (!pScriptRes || !pScriptRes->IsLoaded())
    return NULL;
  return pScriptRes->CreateScriptInstance();
}


BOOL VScriptResourceManager::ValidateScript(const char *szText, int iLen, IVLog *pLog)
{
  char blank = 0;
  if (!szText)
    szText = &blank;
  if (iLen<0)
    iLen = (int)strlen(szText);
  if (iLen==0)
  {
    pLog->Error("No source code specified");
    return FALSE;
  }

  lua_State *pMasterState = GetMasterState();
  VASSERT_MSG(pMasterState, "Script manager not initialized");
  
  //Create a new thread
  lua_State *pTempThread = lua_newthread(pMasterState);
  //Make own locals table for pTempThread so it doesn't pollute the parent state
  LUA_CreateLocalsTable(pTempThread);
  lua_pop(pMasterState,1); // do not leave thread on the stack

  // load the string
  if (!VScriptResourceManager::LuaErrorCheck(pTempThread,luaL_loadstring(pTempThread, szText),pLog))
    return FALSE;

  // run once so function names are known
  if (!VScriptResourceManager::LuaErrorCheck(pTempThread,lua_pcall(pTempThread, 0, LUA_MULTRET, 0), pLog))
    return FALSE;

  if (pLog) pLog->Info("OK.");
  return TRUE;
}


void VScriptResourceManager::ShowDebugInfo(IVRenderInterface *pRI)
{
  char szLine[1024];
  float yk = 12.f;
  float xk = 10.f;
  pRI->DrawText2D(10.f,yk, "Scripting overview", V_RGBA_WHITE);yk+=12.f;
  sprintf(szLine,"Number of script instances \t: %i", Instances().Count());
    pRI->DrawText2D(10.f,yk, szLine, V_RGBA_WHITE);yk+=12.f;
  sprintf(szLine,"Lua threads created \t: %i", g_iThreadsCreated);
    pRI->DrawText2D(40.f,yk, szLine, V_RGBA_WHITE);yk+=10.f;
  sprintf(szLine,"Lua threads recycled \t: %i", g_iThreadsRecycled);
    pRI->DrawText2D(40.f,yk, szLine, V_RGBA_WHITE);yk+=10.f;
  sprintf(szLine,"Lua functions called \t: %i", g_iFunctionsCalled);
    pRI->DrawText2D(40.f,yk, szLine, V_RGBA_WHITE);yk+=10.f;
  sprintf(szLine,"...function calls failed \t: %i", g_iFunctionsFailed);
    pRI->DrawText2D(40.f,yk, szLine, V_RGBA_WHITE);yk+=10.f;
  yk+=4;

  VScriptInstance *pObj;
  bool bNewline = false;
  pObj = (VScriptInstance *)GetGameScript();
  if (pObj)
  {
    pObj->DebugOutput(pRI,"Game script :",xk,yk,false);
    bNewline = true;
  }
  pObj = (VScriptInstance *)GetSceneScript();
  if (pObj)
  {
    pObj->DebugOutput(pRI,"Scene Script : ",xk,yk,false);
    bNewline = true;
  }
  if (bNewline)
    yk += 2.f;
  for (int i=0;i<Instances().Count();i++)
  {
    pObj = Instances().GetAt(i);
    sprintf(szLine,"%i. ",i);
    pObj->DebugOutput(pRI,szLine,xk,yk,true);
  }
}

bool VScriptResourceManager::Require(const char* pFileName)
{
  IVFileInStream *pIn = Vision::File.Open(pFileName);
  if (!pIn)
    return false;

  int iScriptLen = pIn->GetSize();
  VMemoryTempBuffer<16*1024> buffer(iScriptLen+1);
  char *szBuffer = (char *)buffer.GetBuffer();
  pIn->Read(szBuffer,iScriptLen);
  szBuffer[iScriptLen] = 0;
  pIn->Close();

  if (!LUA_ERRORCHECK(m_pMasterState, luaL_loadbuffer(m_pMasterState, szBuffer, iScriptLen, pFileName)))
    return false;

  if (!LUA_ERRORCHECK(m_pMasterState, lua_pcall (m_pMasterState, 0, LUA_MULTRET, 0)))
    return false;

  return true;
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
