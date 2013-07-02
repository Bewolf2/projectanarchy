/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiTriggerComponent.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


enum VSCRIPT_COMPONENT_VERSION {
  VERSION_INITIAL = 1,
  VERSION_CUSTOM_EXPOSED_MEMBERS,

  VERSION_CURRENT = VERSION_CUSTOM_EXPOSED_MEMBERS
};



/****************************************************************************/
// VScriptMember class
/****************************************************************************/

VScriptMember::VScriptMember() :
  m_szType(NULL), m_szName(NULL), m_szValue(NULL)
{
}

VScriptMember::VScriptMember(const char * szName, const char * szValue, const char * szType)
{
  VASSERT_MSG(szName!=NULL, "Please always specify a name for a member!");

  m_szType = szType==NULL ? NULL : strdup(szType);
  m_szName = szName==NULL ? NULL : strdup(szName);
  m_szValue = szValue==NULL ? NULL : strdup(szValue);
}

VScriptMember::~VScriptMember()
{
  V_SAFE_DELETE(m_szType);
  V_SAFE_DELETE(m_szName);
  V_SAFE_DELETE(m_szValue);
}

void VScriptMember::SetValue(const char * szValue)
{
  V_SAFE_DELETE(m_szValue);
  m_szValue = szValue==NULL ? NULL : strdup(szValue);
}

void VScriptMember::operator=(const VScriptMember &copy)
{
  m_szType = copy.m_szType==NULL ? NULL : strdup(copy.m_szType);
  m_szName = copy.m_szName==NULL ? NULL : strdup(copy.m_szName);
  m_szValue = copy.m_szValue==NULL ? NULL : strdup(copy.m_szValue);
}

bool VScriptMember::operator==(const VScriptMember & other) const
{
  return m_szName==other.m_szName||(m_szName!=NULL&&other.m_szName!=NULL&&strcmp(m_szName, other.m_szName)==0);
};

bool VScriptMember::operator!=(const VScriptMember & other) const
{
  return( (m_szName!=other.m_szName&&(m_szName==NULL||other.m_szName==NULL||strcmp(m_szName, other.m_szName)!=0)) );
};


/****************************************************************************/
// VScriptComponent class
/****************************************************************************/

extern VModule g_VisionEngineModule;

V_IMPLEMENT_SERIAL( VScriptComponent, IVObjectComponent, 0, &g_VisionEngineModule );

VScriptComponent::VScriptComponent() :
  m_spInstance(NULL),
  m_pLastSenderAnimControl(NULL),
  m_iScriptRefID(LUA_NOREF),
  m_iFunctions(0),
  ScriptFile(),
  m_bScriptThinkEnabled(true),
  m_CustomExposeVars(),
  m_DefaultExposeVars()
{
}

VScriptComponent::~VScriptComponent()
{
  if (m_spInstance.GetPtr())
  {
    SetScriptInstance(NULL);
  }

  if (m_iScriptRefID>=0)
  {
    lua_State* L = VScriptResourceManager::GlobalManager().GetMasterState();
    LUA_ResetObjectProxy(L, this);
  }
}
  
void VScriptComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  // On detaching of the old owner, call the OnDestroy function of the script.
  // We have to do this here since the owner of the script component won't be
  // known anymore in the constructor (this "self" will be NIL in Lua). [#21576]
  if (pOwner == NULL && GetOwner() != NULL)
  {
    if (m_iFunctions & VSCRIPT_FUNC_ONDESTROY)
      m_spInstance->ExecuteFunctionArg("OnDestroy", "*");
  }

  // Reset the script object if we are changing objects.
  // Don't destroy it for GetOwner() == NULL, because it might have beeen 
  // created in the deserialization step
  if (GetOwner() != NULL && m_iScriptRefID >= 0)
  {
    lua_State* L = VScriptResourceManager::GlobalManager().GetMasterState();
    LUA_ResetObjectProxy(L, this);
  }

  // set the new owner
  IVObjectComponent::SetOwner(pOwner);

  // ensure that the latest resource is loaded when working in the editor and caching is disabled
  if (pOwner && Vision::Editor.IsInEditor() && !Vision::Editor.UseEditorScriptCaching())
  {
    VScriptResource* pRes = NULL;

    // The script file can be empty, e.g. when the script component is deserialized from a prefab
    if (ScriptFile.IsEmpty())
    {
      // But in this case, the instance should be available and then we can directly get the resource
      if (m_spInstance != NULL)
        pRes = m_spInstance->GetResource();
    }
    else
    {
      // If the filename is available, we can use it to query the resource and ensure it is up to date
      pRes = (VScriptResource*)VScriptResourceManager::GlobalManager().GetResourceByName(ScriptFile);
    }

    // will reload resource if timestamp is deprecated
    if (pRes != NULL)
      pRes->CheckFileModified(Vision::File.GetManager(), TRUE, TRUE); 
  }

  if (pOwner != NULL && !ScriptFile.IsEmpty())
  {
    VScriptInstance* pScriptInstance = (VScriptInstance*)VScriptResourceManager::GlobalManager().CreateScriptInstanceFromFile(ScriptFile);
    if(pScriptInstance)
    {
      SetScriptInstance(pScriptInstance);
    }
  }
}

void VScriptComponent::SetScriptInstance(VScriptInstance* pInstance)
{
  if (m_spInstance != NULL)
  {
    DeregisterCallbacks();

    // Dispose our instance. Necessary to remove the assignment to its streaming zone (see ticket #25491)
    // (Note: script instances are unique per component, and thus not shared).
    m_spInstance->DisposeObject();
  }

  m_spInstance = pInstance;
  m_iFunctions = 0;

  if (pInstance != NULL)
  {
    pInstance->SetParentComponent(this);
    m_iFunctions = CheckAvailableFunctions(pInstance);
    RegisterCallbacks();

    TriggerOnExpose();

    //on a new script instance, call the OnCreate function of the script
    if (m_iFunctions & VSCRIPT_FUNC_ONCREATE)
      m_spInstance->ExecuteFunctionArg("OnCreate", "*");
  }
}

void VScriptComponent::TriggerOnExpose()
{
  // check for the OnExpose callback
  if ((m_iFunctions & VSCRIPT_FUNC_ONEXPOSE) == 0)
    return;

  // collect vars from the OnExpose callback (in order to be 
  // sure to have all defined members initialized)
  m_spInstance->ExecuteFunctionArg("OnExpose", "*");

  if (Vision::Editor.IsInEditor())
  {
    //collect those vars
    m_DefaultExposeVars.Clear();
    m_spInstance->GetMembers(m_DefaultExposeVars);

    // remove vars that are outdated from the customized list
    if (Vision::Editor.GetMode() == VisEditorManager_cl::EDITORMODE_NONE)
    {
      m_CustomExposeVars.IntersectWith(m_DefaultExposeVars); 
    }
  }

  // update already customized members, which can be a subset of all 
  // defined vars of the OnExpose callback (also serialized)
  m_spInstance->ExecuteCustomMembers(m_CustomExposeVars);
}

void VScriptComponent::RegisterCallbacks()
{
  //register on the correct callbacks
  if (m_iFunctions & VSCRIPT_FUNC_ONUPDATESCENEBEGIN)
      Vision::Callbacks.OnUpdateSceneBegin += this;
  if (m_iFunctions & VSCRIPT_FUNC_ONTHINK)
      Vision::Callbacks.OnScriptThink += this;
  if (m_iFunctions & VSCRIPT_FUNC_ONUPDATESCENEFINISHED)
      Vision::Callbacks.OnUpdateSceneFinished += this;
  if (m_iFunctions & VSCRIPT_FUNC_ONAFTERSCENELOADED)
    Vision::Callbacks.OnAfterSceneLoaded += this;
  if (m_iFunctions & VSCRIPT_FUNC_ONBEFORESCENEUNLOADED)
    Vision::Callbacks.OnBeforeSceneUnloaded += this;
  if (m_iFunctions & VSCRIPT_FUNC_ONVIDEOCHANGED)
    Vision::Callbacks.OnVideoChanged += this;
}  


void VScriptComponent::DeregisterCallbacks()
{
  //deregister on the correct callbacks
  if (m_iFunctions & VSCRIPT_FUNC_ONUPDATESCENEBEGIN)
      Vision::Callbacks.OnUpdateSceneBegin -= this;
  if (m_iFunctions & VSCRIPT_FUNC_ONTHINK)
      Vision::Callbacks.OnScriptThink -= this;
  if (m_iFunctions & VSCRIPT_FUNC_ONUPDATESCENEFINISHED)
      Vision::Callbacks.OnUpdateSceneFinished -= this;
  if (m_iFunctions & VSCRIPT_FUNC_ONAFTERSCENELOADED)
    Vision::Callbacks.OnAfterSceneLoaded -= this;
  if (m_iFunctions & VSCRIPT_FUNC_ONBEFORESCENEUNLOADED)
    Vision::Callbacks.OnBeforeSceneUnloaded -= this;
  if (m_iFunctions & VSCRIPT_FUNC_ONVIDEOCHANGED)
    Vision::Callbacks.OnVideoChanged -= this;
}  

int VScriptComponent::CheckAvailableFunctions(VScriptInstance* pInstance)
{
  int iRes = 0;
  
  //check which functions are present
  if (pInstance->HasFunction("OnUpdateSceneBegin"))     iRes |= VSCRIPT_FUNC_ONUPDATESCENEBEGIN;
  if (pInstance->HasFunction("OnUpdateSceneFinished"))  iRes |= VSCRIPT_FUNC_ONUPDATESCENEFINISHED;
  if (pInstance->HasFunction("OnAfterSceneLoaded"))     iRes |= VSCRIPT_FUNC_ONAFTERSCENELOADED;
  if (pInstance->HasFunction("OnBeforeSceneUnloaded"))  iRes |= VSCRIPT_FUNC_ONBEFORESCENEUNLOADED;
  if (pInstance->HasFunction("OnVideoChanged"))         iRes |= VSCRIPT_FUNC_ONVIDEOCHANGED;
  if (pInstance->HasFunction("OnThink"))                iRes |= VSCRIPT_FUNC_ONTHINK;
  if (pInstance->HasFunction("OnCreate"))               iRes |= VSCRIPT_FUNC_ONCREATE;
  if (pInstance->HasFunction("OnDestroy"))              iRes |= VSCRIPT_FUNC_ONDESTROY;
  if (pInstance->HasFunction("OnSerialize"))            iRes |= VSCRIPT_FUNC_ONSERIALIZE;
  if (pInstance->HasFunction("OnCollision"))            iRes |= VSCRIPT_FUNC_ONCOLLISION;
  if (pInstance->HasFunction("OnTrigger"))              iRes |= VSCRIPT_FUNC_ONTRIGGER; 
  if (pInstance->HasFunction("OnTransitionEvent"))      iRes |= VSCRIPT_FUNC_ONTRANSITIONEVENT;
  if (pInstance->HasFunction("OnAnimationEvent"))       iRes |= VSCRIPT_FUNC_ONANIMATIONEVENT;
  if (pInstance->HasFunction("OnExpose"))               iRes |= VSCRIPT_FUNC_ONEXPOSE;
  
  return iRes;
}


//Implement IVisCallbackHandler_cl
void VScriptComponent::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  VISION_PROFILE_FUNCTION(PROFILING_SCRIPTING);  //TODO: Different category?
  VASSERT(m_spInstance);
  if (!m_spInstance || !Vision::Editor.IsAnimatingOrPlaying())
    return;
    
  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneBegin)
  {
    if (m_iFunctions & VSCRIPT_FUNC_ONUPDATESCENEBEGIN)
      m_spInstance->ExecuteFunctionArg("OnUpdateSceneBegin", "*");
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnScriptThink)
  {
    if ((m_iFunctions & VSCRIPT_FUNC_ONTHINK) && !Vision::GetScriptManager()->IsPaused() && m_bScriptThinkEnabled)
    {
      m_spInstance->ExecuteFunctionArg("OnThink", "*");
    }
    return;
  }
  
  if (pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished)
  {
    if (m_iFunctions & VSCRIPT_FUNC_ONUPDATESCENEFINISHED)
      m_spInstance->ExecuteFunctionArg("OnUpdateSceneFinished", "*");
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneLoaded)
  {
    if (m_iFunctions & VSCRIPT_FUNC_ONAFTERSCENELOADED)
      m_spInstance->ExecuteFunctionArg("OnAfterSceneLoaded", "*");
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnBeforeSceneUnloaded)
  {
    if (m_iFunctions & VSCRIPT_FUNC_ONBEFORESCENEUNLOADED)
      m_spInstance->ExecuteFunctionArg("OnBeforeSceneUnloaded", "*");
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnVideoChanged)
  {
    if (m_iFunctions & VSCRIPT_FUNC_ONVIDEOCHANGED)
      m_spInstance->ExecuteFunctionArg("OnVideoChanged", "*");
    return;
  }

}


//Implement VisTypedEngineObject_cl::MessageFunction
void VScriptComponent::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  VISION_PROFILE_FUNCTION(PROFILING_SCRIPTING);  //TODO: Different category?
  
  // Return immediately when this component has no script instance
  if (!m_spInstance)
    return;

  if (iID == VIS_MSG_TRIGGER)
  {
    if (m_iFunctions&VSCRIPT_FUNC_ONTRIGGER)
    {
      VisTriggerSourceComponent_cl *pTriggerSrc = (VisTriggerSourceComponent_cl *)iParamA;
      VisTriggerTargetComponent_cl *pTriggerTgt = (VisTriggerTargetComponent_cl *)iParamB;
      m_spInstance->ExecuteFunctionArg("OnTrigger", "*ss",pTriggerSrc->GetComponentName(), pTriggerTgt->GetComponentName());
    }

    return;
  }

  if (iID == VIS_MSG_TRANSITIONSTATEMACHINE)
  {
    if (m_iFunctions&VSCRIPT_FUNC_ONTRANSITIONEVENT)
    {
      
      m_spInstance->ExecuteFunctionArg("OnTransitionEvent", "*i", iParamA);
    }
    return;
  }

  if (iID == VIS_MSG_EVENT) // aka animation event
  {
    if (m_iFunctions&VSCRIPT_FUNC_ONANIMATIONEVENT)
    {
      VisAnimControl_cl *pSender = (VisAnimControl_cl *)iParamB;
      
      //do not call Lua function if the animation has been paused
      if(!pSender->IsPlaying() && m_pLastSenderAnimControl == pSender) return;
      m_pLastSenderAnimControl = pSender;

      //get the name of the animation sequence
      const char * szAnimSequence = pSender->GetAnimSequence() ? pSender->GetAnimSequence()->GetName() : NULL;

      if(Vision::Animations.IsStringEvent((int)iParamA))
      {
        const char * szAnimationEventName = Vision::Animations.GetEventString((int)iParamA);
        m_spInstance->ExecuteFunctionArg("OnAnimationEvent", "*ss", szAnimationEventName, szAnimSequence);
      }
      else
      {
        m_spInstance->ExecuteFunctionArg("OnAnimationEvent", "*is", iParamA, szAnimSequence);
      }
    }
    return;
  }

}


void VScriptComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  IVObjectComponent::OnVariableValueChanged(pVar, value);
  if (!strcmp(pVar->name,"ScriptFile"))
  {
    const char *szOldFile = NULL;
    if (this->m_spInstance!=NULL && m_spInstance->GetResource()!=NULL)
      szOldFile = m_spInstance->GetResource()->GetFilename();
    if (VStringHelper::SafeCompare(szOldFile,value)!=0)
      SetOwner(GetOwner()); // apply script
  }
}

bool VScriptComponent::SetVariable(const char * szName, const char * szValue)
{
  if (IVObjectComponent::SetVariable(szName, szValue))
    return true;

  if (VStringUtil::IsEmpty(szName) || !VStringHelper::SafeCompare(szName, "ComponentID", true))
    return false;

  //try to find already customized vars
  for(int i=0;i<m_CustomExposeVars.GetSize();i++)
  {
    if(strcmp(m_CustomExposeVars[i].GetName(), szName)==0)
    {
      m_CustomExposeVars[i].SetValue(szValue);
      return true;
    }
  }

  //create a new custom varaible
  if(szValue != NULL && szValue[0] != '\0')
    m_CustomExposeVars.Add(VScriptMember(szName, szValue)); //type determinated via value
  else
    m_CustomExposeVars.Add(VScriptMember(szName, "nil")); //typeless

  return true;
}



int VScriptComponent::GetSupportedFunctions()
{
  return m_iFunctions;
}


void VScriptComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    if (iLocalVersion<VERSION_INITIAL||iLocalVersion>VERSION_CURRENT)
      Vision::Error.FatalError("Invalid script serialization version - please re-export map");

    // This is a workaround for [#21287] : This component must be immediately added to the owner's component list,
    // otherwise script function calls on it create a new script component which results in one object having two script components
    // [8.1: need to serialize it here]
    VisTypedEngineObject_cl *pOwner = (VisTypedEngineObject_cl *)ar.ReadObject(NULL);
    if (pOwner != NULL && !pOwner->Components().Contains(this))
      ((VObjectComponentCollection &)pOwner->Components()).Add(this);
    // additionally we have to temporarily pretend the owner is set:
    m_pOwner = pOwner;

    m_iScriptRefID = -1;
    VScriptInstance *pInstance = NULL;
    ar >> pInstance;
    // we shouldn't call SetScriptInstance since it calls the Lua OnCreate function, which may try to access the object
    //SetScriptInstance(pInstance);
    m_spInstance = pInstance;

    if(iLocalVersion >= VERSION_CUSTOM_EXPOSED_MEMBERS)
    {
      int iNumOfSerializedMembers;
      ar >> iNumOfSerializedMembers;

      m_CustomExposeVars.Clear();

      // read all members storead as Name+Type
      for(int i = 0;i < iNumOfSerializedMembers; i++)
      {
        char szBuffer1[64];
        char szBuffer2[64];
        bool bAllocated1 = false;
        bool bAllocated2 = false;
        const char *szName = ar.ReadEncryptedString(szBuffer1, 64, bAllocated1);
        const char *szValue = ar.ReadEncryptedString(szBuffer2, 64, bAllocated2);

        m_CustomExposeVars.Add(VScriptMember(szName, szValue));

        if(bAllocated1) V_SAFE_DELETE(szName);
        if(bAllocated2) V_SAFE_DELETE(szValue);
      }
    }

    if (pInstance != NULL) 
    {
      pInstance->SetParentComponent(this);
      m_iFunctions = CheckAvailableFunctions(pInstance);
      RegisterCallbacks();

      if (iLocalVersion >= VERSION_CUSTOM_EXPOSED_MEMBERS)
        TriggerOnExpose();

      m_spInstance->ScriptSerialize(ar);
    }
    // Owner should be set later in SetOwner:
    m_pOwner = NULL;
  }
  else
  {
    //write
    ar << iLocalVersion;
    ar << GetOwner();
    ar << m_spInstance;

    if(iLocalVersion>=VERSION_CUSTOM_EXPOSED_MEMBERS)
    {
      //do not serialize any expose vars if no OnExpose callback is present (so you can delete)
      int iNumOfSerializedMembers = (m_iFunctions & VSCRIPT_FUNC_ONEXPOSE) == 0 ? 0 : m_CustomExposeVars.GetSize();
      ar << iNumOfSerializedMembers;      

      for (int i = 0;i < iNumOfSerializedMembers; i++)
      {
        ar.WriteEncryptedString(m_CustomExposeVars[i].GetName());
        ar.WriteEncryptedString(m_CustomExposeVars[i].GetValue());
      }
    }

    if (m_spInstance!=NULL)
      m_spInstance->ScriptSerialize(ar);
  }
}

void VScriptComponent::GetCustomDisplayName(VString &sDestName)
{
  sDestName.Format("Lua Script : %s",VFileHelper::GetFilename(ScriptFile));
}

#ifdef WIN32

int VScriptComponent::GetCustomVariableInfo(DynObjArray_cl<VisVariable_cl> &customVarList)
{
  int iCount = customVarList.GetValidSize();
  IVObjectComponent::GetCustomVariableInfo(customVarList);

  //m_runtimeExposeVars is not filled with vars outside the editor
  if(!Vision::Editor.IsInEditor())
    return customVarList.GetValidSize()-iCount;
  
  if(!m_DefaultExposeVars.IsEmpty())
  {
    //there are some vars of the OnExpose callback, so add them to the provided collection
    int j = customVarList.GetFreePos();

    for(int i=0;i<m_DefaultExposeVars.GetSize();i++)
    {
      customVarList[j].name = m_DefaultExposeVars[i].GetName();
      customVarList[j].desc = m_DefaultExposeVars[i].GetType();
      customVarList[j].defaultValue = m_DefaultExposeVars[i].GetValue();
      customVarList[j].type = VULPTYPE_STRING;    //the lua type will be in the description!
      customVarList[j].category = "Lua variable";
      customVarList[j].clsOffset = 0;
      customVarList[j].editorHintString = "";
      customVarList[j].extraInfo = 0;
      customVarList[j].hintFlags = DISPLAY_HINT_NONE;
      customVarList[j].m_pNextVar = NULL;
      customVarList[j].offset = 0;
      j++;
    }
  }

  int iNewEntires = customVarList.GetValidSize()-iCount;
  return iNewEntires;
}
#endif // WIN32

START_VAR_TABLE(VScriptComponent,IVObjectComponent,"Lua script component",VCOMPONENT_ALLOW_MULTIPLE, "Lua Script" )
DEFINE_VAR_VSTRING(VScriptComponent, ScriptFile, "Filename of the script file", "", 0, 0, "scriptfile");
END_VAR_TABLE

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
