/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptInstance.hpp

#ifndef VSCRIPT_INSTANCE_HPP_INCLUDED
#define VSCRIPT_INSTANCE_HPP_INCLUDED

class VScriptInstanceCollection;
class VScriptInstance;
class VScriptMember;

/// \brief
///   Archive class that implements important funtionality to allow for full object serialization from inside a script.
///
/// That is, objects are stored in a table and saved as a full list later. The list is loaded again regardless of whether
/// the script's serialize function needs the objects or not.
class VScriptArchive : public VArchive
{
public:
  /// \brief
  ///   List entry for deferred saving
  /// \internal
  struct VObjectClassPair_t
  {
    const VTypedObject* m_pObj;
    const VType* m_pClass;
  };


  VScriptArchive(IVFileInStream* pInStream, IVFileOutStream* pOutStream)
     : VArchive(NULL,pInStream,pOutStream,Vision::GetTypeManager()),
       m_DeferredObjStored(0)
  {
    m_iDeferredObjectCount = m_iCurrentDeferredObject = 0;
    m_pDeferredObjLoaded = NULL;
  }


  /// \brief
  ///   Overridden object read function: Grabs object from the loaded list instead
  virtual VTypedObject* ReadObject( const VType* pClassRefRequested, unsigned int* pObjectSize = NULL )
  {
    if (m_pDeferredObjLoaded==NULL || m_iCurrentDeferredObject>=m_iDeferredObjectCount)
    {
      VASSERT_MSG(FALSE,"Archive corruption: Reading more objects than stored");
      return NULL;
    }
    return m_pDeferredObjLoaded[m_iCurrentDeferredObject++];
  }

  /// \brief
  ///   Overridden object write function: Stores object/class for deferred serialization
  virtual void WriteObject( const VTypedObject* pObj, const VType* pForceClass)
  {
    VObjectClassPair_t &pair(m_DeferredObjStored[m_iDeferredObjectCount++]);
    pair.m_pObj = pObj;
    pair.m_pClass = pForceClass;
  }


  int m_iDeferredObjectCount, m_iCurrentDeferredObject;
  DynArray_cl<VObjectClassPair_t> m_DeferredObjStored;
  VTypedObject **m_pDeferredObjLoaded; ///< set from outside
};



/// \brief
///   Collection class that keeps a list of script instances. Additionally adds convenience
///   functions.
class VScriptInstanceCollection : public VRefCountedCollection<VScriptInstance>
{
public:
  SCRIPT_IMPEXP VScriptInstanceCollection();
  SCRIPT_IMPEXP void FlagForDisposal(VScriptInstance *pObj);
  SCRIPT_IMPEXP void SafeRemove(VScriptInstance *pElement);
  SCRIPT_IMPEXP void RemoveFlagged();
  
  SCRIPT_IMPEXP void AddWaitingObject(VScriptInstance *pObj);
  SCRIPT_IMPEXP void HandleWaitingObjects(float dtime);
  SCRIPT_IMPEXP void AddSuspendedObject(VScriptInstance *pObj);
  SCRIPT_IMPEXP void HandleSuspendedObjects(float dtime);
public:
  bool m_bAnyFlaggedForDisposal;

  int m_iWaitingObjectCount, m_iSuspendedObjectCount;
  DynArray_cl<VScriptInstance *>m_WaitingObjects;
  DynArray_cl<VScriptInstance *>m_SuspendedObjects;

};


enum VScriptInstanceConstants {
  ARGV_MAX_NESTED_TABLES = 4
};

/// \brief
///   Implements the IVScriptInstance interface using LUA
/// 
/// Represents a single compiled script instance of a script resource (VScriptResource).
class VScriptInstance : public IVScriptInstance
{
public:

  SCRIPT_IMPEXP VScriptInstance();
  SCRIPT_IMPEXP virtual ~VScriptInstance();
  SCRIPT_IMPEXP virtual void DisposeObject();

  SCRIPT_IMPEXP bool Init(bool disposeOnFinsh=true);
  SCRIPT_IMPEXP bool DeInit();

  inline bool IsFinished() const {return !m_bHasSuspendedThreads;}

  SCRIPT_IMPEXP void WaitSeconds(lua_State *pState, float seconds);

  SCRIPT_IMPEXP void SetScriptName(const char* scriptName)
  {
    m_ScriptName=scriptName;
  }

  /// \brief
  ///   Overridden function to execute a function call on this script object. See interface IVScriptInstance::ExecuteFunctionArgV for detailed description
  SCRIPT_IMPEXP virtual BOOL ExecuteFunctionArgV(const char *szFunction, const char *szArgFormat, va_list argPtr);

  /// \brief
  ///   Overridden function. See interface IVScriptInstance::HasFunction for detailed description
  SCRIPT_IMPEXP virtual BOOL HasFunction(const char *szFunction);
  
  /// \brief
  ///   Overridden function. See interface IVScriptInstance::RunScriptCode for detailed description
  SCRIPT_IMPEXP virtual BOOL RunScriptCode(const char *szText, bool bUseGlobals = false);
  
  /// \brief
  ///   Called once per simulation tick
  SCRIPT_IMPEXP void Tick(float dtime);

  /// \brief
  ///   DEPRECATED
  SCRIPT_IMPEXP void ResumeAll(float dtime);

  /// \brief
  ///   Internal function: Sets the resource that contains the compiled script and state for this instance
  SCRIPT_IMPEXP bool SetResource(VScriptResource *pRes);
  
  /// \brief
  ///   Returns the resource used for this instance
  SCRIPT_IMPEXP VScriptResource* GetResource();
  
  /// \brief
  ///   Internal function: Executes the OnSerialize method of the script instance to load/save a chunk of data
  SCRIPT_IMPEXP void ScriptSerialize(VArchive &ar);

  /// \brief  Gets all the members previously collected in the OnExpose Callback. 
  /// \param  members The new members will be stored in here if not already present.
  SCRIPT_IMPEXP void GetMembers(LinkedList_cl<VScriptMember> &members);

  /// \brief Generates and executes Lua code so that all specified members will set up on the self table.
  /// \param members The members to initialize.
  SCRIPT_IMPEXP void ExecuteCustomMembers(LinkedList_cl<VScriptMember> &members);

  /// \brief
  ///   Internal function: Executes one of the collision methods of the script instance for vision physics.
  SCRIPT_IMPEXP bool ExecScriptCollisionFunc(const char *pszFunctionName, VisCollisionInfo_t *pColInfo);

  /// \brief
  ///   Executes one of the collision methods of the script instance. The
  ///   VScriptCollisionInfo structure is intended to be used independent of the physics engine.
  SCRIPT_IMPEXP bool ExecuteCollisionFunc(const char *pszFunctionName, VScriptCollisionInfo *pColInfo);

  //serialization
  V_DECLARE_SERIAL_DLLEXP( VScriptInstance,  SCRIPT_IMPEXP )

  /// \brief
  ///   Overridden script object serialization function
  SCRIPT_IMPEXP virtual void Serialize( VArchive &ar );
  SCRIPT_IMPEXP virtual VBool WantsDeserializationCallback(const VSerializationContext &context) {return context.m_eType!=VSerializationContext::VSERIALIZATION_EDITOR;}
  SCRIPT_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context);
  

  // debug
  SCRIPT_IMPEXP void DebugOutput(IVRenderInterface *pRI, const char *szPrepend, float &x, float &y, bool b3DPos);

  // \brief Immediately execute a defined Lua function of the specified script instance, allowing direct access to the Lua state to setup the function parameters.
  // \param szLuaFunc The function to be called (of the script instance)
  // \param func The callback which prepares the function parameters (Lua stack items) invoked by this function, e.g.: int MyCallback(VScriptInstance *, lua_State *, void *pUserData).
  // The return value of this callback is the number of stack items to be processed by Lua!
  // \param pUserData (\b optional) User data pointer.
  // \return The boolean return value of your callback
  //SCRIPT_IMPEXP bool ExecuteNativeCallback(const char * szLuaFunc, int (*pFunc)(VScriptInstance *, lua_State *, void *pUserData), void *pUserData = NULL);

protected:

  /// \brief
  ///   Internal use
  /// \internal
  enum VThreadState_e
  {
    Finished,
    Running, ///< Suspended
    Reuse,
  };

  /// \brief
  ///   Internal use
  /// \internal
  struct VLuaThreadInfo
  {
    lua_State *pThread;     ///< lua thread
    int iRefKey;            ///< key value under which the reference is hold
    VThreadState_e eState;  ///< internal state
    float fWaitTime;        ///< current time to wait

    const char *GetStatusString(char *szBuffer) const;
  };

  friend class VScriptInstanceCollection;
  friend class VScriptResourceManager;

  VLuaThreadInfo* PrepareFunctionCall(const char *szFunction);
  bool DoFunctionCall(VLuaThreadInfo *pInfo, int narg);

  void ResumeValue(int returnValue);
  void Resume(int nargs);
  void DiscardThread(lua_State *pThread, bool bForReuse);
  VLuaThreadInfo* CreateNewThread();
  void DiscardCreatedThreads();
  VLuaThreadInfo* FindThread(lua_State *pThread) const;

  VScriptResourcePtr m_spResource;
  VString m_ScriptName;

  int m_iCreatedThreads;
  bool m_bHasSuspendedThreads;

//  VLuaThreadInfo m_MainState;
  DynArray_cl<VLuaThreadInfo> m_CreatedThreads;

  bool m_bDisposeOnFinish, m_bFlagForDisposal;
};

#endif // VSCRIPT_INSTANCE_HPP_INCLUDED

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
