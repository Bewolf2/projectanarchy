/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptManager.hpp

#ifndef VSCRIPTMANAGER_HPP_INCLUDED
#define VSCRIPTMANAGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>

extern int PROFILING_SCRIPTING;
extern int PROFILING_SCRIPTOBJ_TICK;
extern int PROFILING_SCRIPTOBJ_EXECUTEFUNCTION;
extern int PROFILING_SCRIPTOBJ_CREATETHREAD;
extern int PROFILING_SCRIPTOBJ_DISCARDTHREAD;

// this callback will be used for script thinking
#define VSCRIPT_THINK_CALLBACK  Vision::Callbacks.OnScriptThink

// Callbacks for all scripts
#define VSCRIPT_FUNC_ONUPDATESCENEBEGIN         0x000001
#define VSCRIPT_FUNC_ONUPDATESCENEFINISHED      0x000002
#define VSCRIPT_FUNC_ONTHINK                    0x000004
#define VSCRIPT_FUNC_ONCREATE                   0x000080
#define VSCRIPT_FUNC_ONDESTROY                  0x000100
#define VSCRIPT_FUNC_ONAFTERSCENELOADED         0x000400
#define VSCRIPT_FUNC_ONBEFORESCENEUNLOADED      0x008000
#define VSCRIPT_FUNC_ONVIDEOCHANGED             0x010000

// Object3D callbacks
#define VSCRIPT_FUNC_ONSERIALIZE                0x000008
#define VSCRIPT_FUNC_ONCOLLISION                0x000010
//#define VSCRIPT_FUNC_ONTOUCH                    0x000020 //deprecated
//#define VSCRIPT_FUNC_ONVIRTUALHIT               0x000040 //deprecated
#define VSCRIPT_FUNC_ONTRIGGER                  0x000200
//#define VSCRIPT_FUNC_ONTRIGGERBYID              0x000800 //deprecated
#define VSCRIPT_FUNC_ONTRANSITIONEVENT          0x001000
#define VSCRIPT_FUNC_ONANIMATIONEVENT           0x002000
#define VSCRIPT_FUNC_ONEXPOSE                   0x004000


/// \class  VScriptUserDataSerializeObject
///
/// \brief  Script user data serialize object. 
///
class VScriptUserDataSerializeObject : public IVisCallbackDataObject_cl
{
  public:

    VScriptUserDataSerializeObject(VisCallback_cl* pSender, VArchive* Archive, lua_State* L, int Type)
      : IVisCallbackDataObject_cl(pSender), pArchive(Archive), pLuaState(L), iType(Type), bProcessed(false)
    {
    }

    //! Contains a pointer to the archive from which the type should be read or written to. 
    VArchive* pArchive;
    
    //! Contains a pointer to the current Lua state. 
    lua_State* pLuaState;
    
    //! Contains when reading the type id. 
    int iType;

    //! Set this member to true when you processed your custom type in the callback. 
    bool bProcessed;

};

/// \brief
///   Callback data object used for various terrain related callbacks
class VScriptCreateStackProxyObject : public IVisCallbackDataObject_cl
{
public:
  inline VScriptCreateStackProxyObject(
      VCallback *pSender, VTypedObject * pInstance,
      lua_State * pLuaState, VisTypedEngineObject_cl * pIntendedOwner=NULL )
    : IVisCallbackDataObject_cl(pSender),
      m_pInstance(pInstance),
      m_pIntendedOwner(pIntendedOwner),
      m_pLuaState(pLuaState),
      m_bProcessed(false)
  {
  }

  //! Contains a pointer to the instance, of which a proxy is needed. 
  VTypedObject * m_pInstance;

  //! Contains a pointer to the intended owner of the object.
  //  The intended owner will be NULL as long as no initialization is required for the
  //  instance or the proxy is generated for an already initialized instance.
  //  Objects like components may require a initialization before getting attached to an owner,
  //  in such situations the intended owner will be a valid pointer,
  VisTypedEngineObject_cl * m_pIntendedOwner;

  //! Contains a pointer to the current Lua state. 
  lua_State * m_pLuaState;

  //! Set this member to true when you created an object proxy. Do not process the call if this is already set to true.
  bool m_bProcessed;
};

class VScriptResource;
class VScriptInstance;
struct lua_Debug;

/// \brief
///   Data object triggered by the static IVScriptManager::OnDebugEvent callback.
/// 
/// This object provides LUA specific debug event members (current function, line, etc.).
/// 
/// Note that the data inside this callback is only valid while the callback is called.
class VScriptDebugEventObject : public IVisCallbackDataObject_cl
{
public:
  VScriptDebugEventObject( VisCallback_cl *pSender) : IVisCallbackDataObject_cl(pSender)
  {
    m_iCurrentLine = -1;
    m_szCurrentFunction = 0;
    m_iLineOfFuncDefinition = -1;
    m_szSource = 0;
    m_iEventType = -1;
    m_pScriptResource = NULL;
    m_pScriptObject = NULL;
    m_iStackDepth = 0;
    m_pStackInfo = NULL;
  }

  /// \brief
  ///   Pointer to the script resource. This contains the filename of the script file
  VScriptResource *m_pScriptResource;

  /// \brief
  ///   Pointer to the script object that triggered the debug event
  VScriptInstance *m_pScriptObject;

  /// \brief
  ///   The current line number of execution.
  int m_iCurrentLine;

  /// \brief
  ///   The current function in which the execution takes place.
  const char *m_szCurrentFunction;

  /// \brief
  ///   The line where the current function was defined.
  int m_iLineOfFuncDefinition;

  /// \brief
  ///   The current source (filename)
  const char *m_szSource;

  /// \brief
  ///   The event type. 0 = CALL (a function was called) 1 = RET  (lua returns from a function) 2 =
  ///   LINE (a new line is executed)
  int m_iEventType;

  /// \brief
  ///   Number of debug information pointers in the m_pStackInfo list. This is the stack depth of
  ///   the call stack
  int m_iStackDepth;

  /// \brief
  ///   Array of m_iStackDepth objects. m_pStackInfo[0] describes the position where the error
  ///   occurred.
  lua_Debug *m_pStackInfo;
};


/// \brief
///   Data object triggered by the static IVScriptManager::OnError callback.
/// 
/// This object provides LUA specific error members.
/// 
/// Note that the data inside this callback is only valid while the callback is called.
class VScriptErrorDataObject : public IVisCallbackDataObject_cl
{
public:
  VScriptErrorDataObject(VisCallback_cl *pSender) : IVisCallbackDataObject_cl(pSender)
  {
    m_iStackDepth = 0;
    m_pStackInfo = NULL;
    m_pScriptResource = NULL;
    m_pScriptObject = NULL;
    m_szErrorMsg = NULL;
  }
  
  /// \brief
  ///   Pointer to the script resource. This contains the filename of the script file.
  VScriptResource *m_pScriptResource;

  /// \brief
  ///   Pointer to the script object that triggered the error.
  VScriptInstance *m_pScriptObject;


  /// \brief
  ///   String with the error message.
  const char *m_szErrorMsg;

  /// \brief
  ///   Number of debug information pointers in the m_pStackInfo list. This is the stack depth of
  ///   the call stack.
  int m_iStackDepth;

  /// \brief
  ///   Array of m_iStackDepth objects. m_pStackInfo[0] describes the position where the error
  ///   occurred.
  lua_Debug *m_pStackInfo;
  
  /// \brief
  ///   The error message which Lua returned.
  const char* m_szErrorMessage;
};


/// \brief
///   Implements the IVScriptManager interface for LUA and handles all script resources
class VScriptResourceManager : public VisResourceManager_cl, public IVisCallbackHandler_cl, public IVScriptManager
{
public:

  ///
  /// @name Init / Deinit
  /// @{
  ///

  /// \brief
  ///   Constructor
  SCRIPT_IMPEXP VScriptResourceManager();

  /// \brief
  ///   Destructor
  SCRIPT_IMPEXP virtual ~VScriptResourceManager();

  /// \brief
  ///   Needs to be called once at initialization time.
  SCRIPT_IMPEXP void OneTimeInit();

  /// \brief
  ///   Needs to be called once at deinitialization time.
  SCRIPT_IMPEXP void OneTimeDeInit();

  ///
  /// @}
  ///

  ///
  /// @name Ivscriptmanager Implementation
  /// @{
  ///

  /// \brief
  ///   Overridden IVScriptManager function
  virtual IVScriptInstance* CreateScriptInstanceFromFile(const char *szFilename);

  /// \brief
  ///   Overridden IVScriptManager function
  virtual BOOL ValidateScript(const char *szText, int iLen, IVLog *pLog);

  /// \brief
  ///   Overridden IVScriptManager function. Returns the script instance of an object if it exists.
  SCRIPT_IMPEXP virtual IVScriptInstance* GetScriptInstance( VisTypedEngineObject_cl *pObj);

  /// \brief
  ///   Overridden IVScriptManager function. Associates the script instance with the object
  SCRIPT_IMPEXP virtual void SetScriptInstance( VisTypedEngineObject_cl *pObj, IVScriptInstance* pInst);

  ///
  /// @}
  ///

  ///
  /// @name Lua Specific
  /// @{
  ///
  
  /// \brief
  ///   Loads a script resource (internal use)
  SCRIPT_IMPEXP VScriptResource* LoadScriptFile(const char *szFilename);

  /// \brief
  ///   Returns the LUA specific master state
  inline lua_State* GetMasterState() 
  {
    VASSERT_MSG(m_pMasterState!=NULL, "Lua Master state not initialized!");
    return m_pMasterState;
  }

  /// \brief
  ///   Returns the collection of all script instances in the scene
  inline VScriptInstanceCollection& Instances() {return m_Instances;}

  /// \brief
  ///   Accesses the global instance of a LUA script manager
  SCRIPT_IMPEXP static VScriptResourceManager& GlobalManager();

  /// \brief
  ///   Helper function to output debug information
  SCRIPT_IMPEXP void ShowDebugInfo(IVRenderInterface *pRI);

  /// \fn SCRIPT_IMPEXP bool Require(const char* pFileName)
  ///
  /// \brief  Loads an additional lua file (e.g. for global tool functions). 
  ///
  /// \param [in] pFileName The filename of the file. 
  ///
  /// \return true if it succeeds, false if it fails. 
  ///
  SCRIPT_IMPEXP bool Require(const char* pFileName);

  /// \brief
  ///   Sets the global instance of the game script. For more information about the game script,
  ///   please refer to the documentation.
  inline virtual void SetGameScript(IVScriptInstance* pScriptObj) 
  {
    IVScriptManager::SetGameScript(pScriptObj);

    m_iGameScriptFunctions = 0;
    if(pScriptObj!=NULL)
    {
      if (pScriptObj->HasFunction("OnUpdateSceneBegin"))     m_iGameScriptFunctions |= VSCRIPT_FUNC_ONUPDATESCENEBEGIN;
      if (pScriptObj->HasFunction("OnUpdateSceneFinished"))  m_iGameScriptFunctions |= VSCRIPT_FUNC_ONUPDATESCENEFINISHED;    
    }
  }
  /// \brief
  ///   Sets the global instance of the scene script. For more information about the scene script,
  ///   please refer to the documentation.
  VISION_APIFUNC virtual void SetSceneScript(IVScriptInstance* pScriptObj) 
  {
    IVScriptManager::SetSceneScript(pScriptObj);

    m_iSceneScriptFunctions = 0;
    if(pScriptObj!=NULL)
    {
      if (pScriptObj->HasFunction("OnUpdateSceneBegin"))     m_iSceneScriptFunctions |= VSCRIPT_FUNC_ONUPDATESCENEBEGIN;
      if (pScriptObj->HasFunction("OnUpdateSceneFinished"))  m_iSceneScriptFunctions |= VSCRIPT_FUNC_ONUPDATESCENEFINISHED;
    }
  }

  ///
  /// @}
  ///

  ///
  /// @name Static Helper Functions
  /// @{
  ///

  
  /// \brief
  ///   Always returns the scripting component of an object. Creates and adds it if it doesn't
  ///   exist.
  SCRIPT_IMPEXP static VScriptComponent* GetScriptComponent( VisTypedEngineObject_cl *pObj);
  
  /// \brief
  ///   Indicates whether an object has a script component and returns it.
  SCRIPT_IMPEXP static VScriptComponent* HasScriptComponent( VisTypedEngineObject_cl *pObj);

  
  /// \brief
  ///   Lua specific function to get the state
  SCRIPT_IMPEXP static VScriptInstance* GetScriptInstanceForState(lua_State* state, bool CheckIfValidInstance = true );

  /// \brief
  ///   Lua specific function to link the lua state with object
  SCRIPT_IMPEXP static void SetScriptInstanceForState(lua_State* state, VScriptInstance* pObj);
  
  /// \brief
  ///   Lua specific function
  SCRIPT_IMPEXP static void DiscardThread(lua_State* pThread);

  /// \brief
  ///   Helper function to forward LUA error to log
  SCRIPT_IMPEXP static bool LuaErrorCheck(lua_State *L, int status, IVLog *pLog = NULL);
  
  ///
  /// @}
  ///

public:
  // statistics:
  static int g_iThreadsCreated;
  static int g_iThreadsRecycled;
  static int g_iFunctionsCalled;
  static int g_iFunctionsFailed;

  //! Callback to listen to userdata serialize events. 
  SCRIPT_IMPEXP static VisCallback_cl OnUserDataSerialize;

private:
  SCRIPT_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  int DumpVisionGlobals(lua_State *L) const;

  static VScriptResourceManager g_GlobalManager;
  
  VScriptInstanceCollection m_Instances;
  lua_State*  m_pMasterState;
  bool m_bInitialized;

  int m_iGameScriptFunctions;
  int m_iSceneScriptFunctions;
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
