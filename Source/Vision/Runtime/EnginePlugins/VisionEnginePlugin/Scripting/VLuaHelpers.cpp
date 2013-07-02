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
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Components/VAnimationComponent.hpp>
//#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Components/VTimedValueComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VBitmask.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerBoxEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisionLuaModule_wrapper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/CubeMapHandle.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/PathCameraEntity.hpp>

#include <Vision/Runtime/Base/System/DisableStaticAnalysis.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#define VISION_WRAPPER_LOOKUP "VISION_WRAPPER_LOOKUP"

/****************************************************************************/
// Helper functions for working with LUA
/****************************************************************************/

void LUA_NewMetatable(lua_State* L, const char *pszName)
{  
  luaL_newmetatable(L,pszName);         // stack: ..., meta table, TOP

  // Link the metatable's index operator
  // to itself so it can find the functions
  lua_pushstring(L, "__index");         // stack: ..., meta table, string, TOP
  
  // pushes the metatable
  // (copy the original metatable)
  lua_pushvalue(L, -2);                 // stack: ..., meta table, string, meta table, TOP

  // metatable.__index = metatable
  lua_settable(L, -3);                  // stack: ..., meta table, TOP
}

//Registers metafunctions into a metatable on the stack
void LUA_RegMetatable(lua_State* L, const luaL_Reg* pReg)
{
  //this function requires a table!!!     stack: ..., table, TOP
  while (pReg && pReg->name)
  {
    lua_pushstring(L, pReg->name);      // stack: ..., table, string, TOP
    lua_pushcclosure(L, pReg->func, 0); // stack: ..., table, string, c function, TOP
    lua_settable(L, -3);                // stack: ..., table, TOP
    pReg++;
  }
}

//Registers a native class and methods for scripting
//IDEA: Add support so this function can be called multiple times so we
//      can register additional functions for a class ?
void LUA_CreateWrapperClass(lua_State* L, const NativeClassInfo *pInfo)
{
  VASSERT(pInfo != NULL);
  // make sure that the stack has enough place for the new wrapper class
  int iStackNeeded = 1; // for metatable
  const NativeClassInfo* pGoThrough = pInfo;
  while(pGoThrough)
  {
    const luaL_Reg* pReg = pGoThrough->m_pMethods;

    while (pReg && pReg->name)
    {
      iStackNeeded += 2;
      pReg++;
    }
    pGoThrough = pGoThrough->m_pParent;
  }

  lua_checkstack(L, iStackNeeded);

  LUA_NewMetatable(L, pInfo->pszClassName);

  while (pInfo)
  {
    //Add all the methods for this class
    const luaL_Reg* pReg = pInfo->m_pMethods;
    LUA_RegMetatable(L, pReg);

    //Add the methods for the parent class
    pInfo = pInfo->m_pParent;
  }
}

//Registers a new LUA data type and its meta methods (operators) and other methods
void LUA_OpenLibrary(lua_State* L, const VLUATypeInfo *pInfo, IVRSDUserDataAccessor* pUserDataAccessor /*= NULL*/)
{
  //Add all the meta methods for this class
  luaL_newmetatable(L, pInfo->pszTypeName);
  const luaL_Reg* pReg = pInfo->m_pMethods;
  LUA_RegMetatable(L, pReg);

  //Register a constructor for the new type
  if(pInfo->m_Constructor.func)
    lua_register(L, pInfo->m_Constructor.name, pInfo->m_Constructor.func);

  if(pUserDataAccessor)
    VRSDClient::GetGlobalClient().RegisterUserDataAccessor(pUserDataAccessor);
}

//Get a function of an object from its meta table and push it on the stack (0,+1,-)
void LUA_GetFunction(lua_State* L)
{
  // get the object's meta table
  lua_getmetatable(L, 1);
  // push the second argument (the function name) and get the function from the table
  lua_pushvalue(L, 2);
  lua_rawget(L, 3);
  // pop the meta table and move the function down
  lua_replace(L, 3);
}

int LUA_RegisterObject(lua_State* L)
{
  int id = luaL_ref(L, LUA_REGISTRYINDEX);
  return id;
}

void LUA_DeregisterObject(lua_State* L, int id)
{
  luaL_unref(L, LUA_REGISTRYINDEX, id);
}

void* LUA_TestUserData(lua_State *L, int iStackIndex, const char * szTypeName)
{
  void *p = lua_touserdata(L, iStackIndex);
  if (p != NULL) {  /* value is a userdata? */
    if (lua_getmetatable(L, iStackIndex)) {  /* does it have a metatable? */
      lua_getfield(L, LUA_REGISTRYINDEX, szTypeName);  /* get correct metatable */
      if (lua_rawequal(L, -1, -2))  /* the same? */
      {
        lua_pop(L, 2);
        return p;
      }

      lua_pop(L,1);
      //try the swig registry as alternative spot for type def's
      VSWIG_Lua_get_class_registry(L);
      lua_getfield(L, -1, szTypeName);
      if (lua_rawequal(L, -1, -3))  /* the same? */
      {
        lua_pop(L, 3);
        return p;
      }

      lua_pop(L, 3);  /* remove both metatables and the swig registry */
    }
  }

  return NULL;  /* value is not a userdata with a metatable */
}

void LUA_GetSwigTypes(lua_State *L, VStrList *pTypes)
{
  VSWIG_Lua_get_class_registry(L);

  //start iterating over the swig class registry table
  lua_pushnil(L);

  int index=0;                  //stack: table, nil, TOP

  while (lua_next(L, -2) != 0)  //stack: table, key, value, TOP
  {
    // we only want string fields and no other keys
    if (lua_isstring(L, -2))
    {  
      pTypes->AddString(lua_tostring(L, -2));
      index++;
    }

    //the iterator needs the next key, so we remove the value
    lua_pop(L, 1);              //stack: table, key, TOP
  }

  lua_pop(L, 1); //remove swig table
}

void LUA_ResetObjectProxy(lua_State* L, VScriptComponent *pObj)
{
  // return if the id isn't set
  int id = pObj->GetScriptRefID();
  if (id<0)
    return;

  // clear the reference
  luaL_unref(L, LUA_REGISTRYINDEX, id);
  pObj->SetScriptRefID(LUA_NOREF);
}

//Returns the scripting proxy for an object
//A proxy is created if it doesn't exist yet
void LUA_PushObjectProxy(lua_State* L, VScriptComponent *pObj)
{
  if (pObj==NULL)
  {
    lua_pushnil(L);
    return;
  }
  
  // return id if it is already set
  int id = pObj->GetScriptRefID();
  if (id>=0)
  {
    lua_rawgeti(L, LUA_REGISTRYINDEX, id);
    return;
  }
  // create a proxy object and cache it
  VASSERT(pObj->GetOwner()!=NULL);
  LUA_PushObjectProxy(L, pObj->GetOwner());

  // create a new id and cache it
  lua_pushvalue(L, -1); //duplicate it, so we have it left over on the stack at the end
  id = luaL_ref(L, LUA_REGISTRYINDEX);
  
  pObj->SetScriptRefID(id);
}

void LUA_ResetWrapperLookupTable(lua_State* L)
{
  lua_newtable(L);                                            // ..., table, TOP
  lua_setfield(L, LUA_REGISTRYINDEX, VISION_WRAPPER_LOOKUP);  // ..., TOP
}

void LUA_PushObjectProxy(lua_State* L, VTypedObject *pObj, VisTypedEngineObject_cl* pOwner)
{
  if (pObj==NULL)
  {
    lua_pushnil(L);                                           // ..., nil, TOP
    return;                                                   // 1 value left on the stack
  }

  lua_getfield(L, LUA_REGISTRYINDEX, VISION_WRAPPER_LOOKUP);  // ..., table, TOP
                                              
  //in case the wrapper lookup table does not exist, we create it
  if(lua_isnil(L, -1))
  {
    lua_pop(L,1);                                             // ..., TOP
    lua_newtable(L);                                          // ..., table, TOP
    lua_setfield(L, LUA_REGISTRYINDEX, VISION_WRAPPER_LOOKUP);// ..., TOP
    lua_getfield(L, LUA_REGISTRYINDEX, VISION_WRAPPER_LOOKUP);// ..., table, TOP

    VASSERT_MSG(!lua_isnil(L, -1), "Could not set Lua wrapper lookup table in Lua registry.");
  }

  //we are referencing the wrapper by the pointer address of the original object
  lua_pushfstring(L, "%p", pObj);                             // ..., table, address, TOP
  lua_rawget(L, -2);                                          // ..., table, wrapper-obj or nil, TOP

  //in case there is no stored wrapper we need to create a new wrapper
  if(lua_isnil(L, -1))
  {                                                           // ..., table, nil, TOP
    lua_pop(L, 1);                                            // ..., table, TOP
    LUA_CreateNewWrapper(L, pObj, pOwner);                    // ..., table, wrapper-obj, TOP
    lua_pushfstring(L, "%p", pObj);                           // ..., table, wrapper-obj, address, TOP
    lua_pushvalue(L, -2);                                     // ..., table, wrapper-obj, address, wrapper-obj, TOP
    lua_rawset(L, -4);                                        // ..., table, wrapper-obj, TOP
  }

  //remove the pointer table
  lua_remove(L, -2);                                          // ..., wrapper-obj, TOP
                                                              // 1 value left on the stack
}

void LUA_PushObjectProxy(lua_State* L, hkvVec3* pVector)
{
  VSWIG_PUSH_PROXY(L, hkvVec3, pVector)
}

void LUA_PushObjectProxy(lua_State* L, VColorRef *pColorRef)
{
  VSWIG_PUSH_PROXY(L, VColorRef, pColorRef)
}

void LUA_PushObjectProxy(lua_State* L, hkvAlignedBBox *pBox)
{
  VSWIG_PUSH_PROXY(L, hkvAlignedBBox, pBox)
}

void LUA_PushBitmask(lua_State* L, unsigned int iMask)
{
  VSWIG_Lua_NewPointerObj(L, new VBitmask(iMask), SWIGTYPE_p_VBitmask, VLUA_MANAGE_MEM_BY_LUA);
}

VisTypedEngineObject_cl * LUA_GetObject(lua_State* L, int iStackIndex)
{
  VSWIG_CONVERT_POINTER(L, iStackIndex, VisTypedEngineObject_cl, pInstance)

  return pInstance;
}

//Puts a wrapper object on the stack for a native object
void LUA_CreateNewWrapper(lua_State*L, VTypedObject* pObject, VisTypedEngineObject_cl* pIntendedOwner)
{
  VASSERT_MSG(pObject!=NULL, "Cannot create a Lua wrapper for a NULL value.")

  // trigger callback
  VisCallback_cl &cbCreateProxy = IVScriptManager::OnScriptProxyCreation;;
  VScriptCreateStackProxyObject data(&cbCreateProxy, pObject, L, pIntendedOwner);
  cbCreateProxy.TriggerCallbacks(&data);

  //do not create an object proxy on the stack if a callback already performed this step...
  if(data.m_bProcessed) return;

  //no callback handled the data so we do the default handling here
  if(pObject->IsOfType(V_RUNTIME_CLASS(PathCameraEntity)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_PathCameraEntity, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(TriggerBoxEntity_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_TriggerBoxEntity_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(CubeMapHandle_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_CubeMapHandle_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisBaseEntity_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisLightSource_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisLightSource_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisParticleEffect_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisParticleEffect_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisPath_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisPath_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisStaticMeshInstance_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisStaticMeshInstance_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisObject3D_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VTransitionStateMachine)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VTransitionStateMachine, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisTriggerSourceComponent_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisTriggerSourceComponent_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisTriggerTargetComponent_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisTriggerTargetComponent_cl, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VAnimationComponent)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VAnimationComponent, VLUA_MANAGE_MEM_MANUAL);
  //else if(pObject->IsOfType(V_RUNTIME_CLASS(VTimedValueComponent)))
  //  VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VTimedValueComponent, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(IVRendererNode)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_IVRendererNode, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VPostProcessingBaseComponent)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VPostProcessingBaseComponent, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(IVObjectComponent)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_IVObjectComponent, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VDialog)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VDialog, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VWindowBase)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VWindowBase, VLUA_MANAGE_MEM_MANUAL);
  else if(pObject->IsOfType(V_RUNTIME_CLASS(VisTypedEngineObject_cl)))
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VisTypedEngineObject_cl, VLUA_MANAGE_MEM_MANUAL);
  else // base class VTypedObject
    VSWIG_Lua_NewPointerObj(L,pObject, SWIGTYPE_p_VTypedObject, VLUA_MANAGE_MEM_MANUAL);
}

void LUA_CreateLocalsTable(lua_State* L)
{
  lua_newtable( L );    //new globals table
  
  lua_newtable( L );    //metatable
  lua_pushliteral( L, "__index" );
  lua_pushvalue( L, LUA_GLOBALSINDEX );  //original globals
  // -4 = newglobals -3 = metatable, 
  // -2 = __index    -1 = globals, 
  lua_settable( L, -3 );  // metatable["__index"] = globals , pop2
  
  // -2 = newglobals, -1 = metatable
  lua_setmetatable( L, -2 ); //newglobals.metatable = metatable ,pop
  
  //add "G" field to new globals table so script can access original globals
  //Scripts can then use G to add variables to the original globals table
  // -1 = newglobals
  lua_pushliteral( L, "G" );
  lua_pushvalue( L, LUA_GLOBALSINDEX );  //original globals
  // -3 = newglobals, -2 = "G", -1 = globals
  lua_settable( L, -3 );  // newglobals["G"] = globals

  // -1 = newglobals
  lua_pushliteral( L, "LOCAL" );
  lua_pushvalue( L, -2  );  // newglobals
  // -3 = newglobals, -2 = "LOCAL", -1 = newglobals
  lua_settable( L, -3 );  // newglobals["LOCAL"] = newglobals

  
  // -1 = newglobals
  lua_replace( L, LUA_GLOBALSINDEX );    //replace state's globals with newglobals

  //register static globals:
  VLUA_CreateGlobalInstances(L);
}

////////////////////////////////////////////////////////////////////////
// Helper functions for accessing arguments from wrapper functions
////////////////////////////////////////////////////////////////////////


bool LUA_GetValue(lua_State* L, int id, bool &value)
{
  if (!lua_isboolean(L, id))  return false;
  value = (lua_toboolean(L,id)!=0);
  return true;
}

bool LUA_GetValue(lua_State* L, int id, int &value)
{
  if (lua_isboolean(L, id))
  { 
    value = (lua_toboolean(L,id)!=0);
    return true;
  }
  if (!lua_isnumber(L, id)) return false;
  value = (int)lua_tointeger(L,id);
  return true;
}

bool LUA_GetValue(lua_State* L, int id, float &value)
{
  if (!lua_isnumber(L, id)) return false;
  value = (float)lua_tonumber(L,id);
  return true;
}

bool LUA_GetValue(lua_State* L, int id, double &value)
{
  if (!lua_isnumber(L, id)) return false;
  value = (double)lua_tonumber(L,id);
  return true;
}

bool LUA_GetValue(lua_State* L, int id, VString &value)
{
  if (!lua_isstring(L, id)) return false;
  value = lua_tolstring(L,id, NULL);
  return true;
}

//NB. The string pointers only valid while the object is still on the stack
bool LUA_GetValue(lua_State* L, int id, const char* &value)
{
  if (!lua_isstring(L, id)) return false;
  value = lua_tolstring(L,id, NULL);
  return true;
}


#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=178
#endif

static int getfield (lua_State *L, const char *key, int d)
{
  int res;
  lua_getfield(L, -1, key);
  if (lua_isnumber(L, -1))
    res = (int)lua_tointeger(L, -1);
  else {
    if (d < 0)
      return luaL_error(L, "field " LUA_QS " missing in date table", key);
    res = d;
  }
  lua_pop(L, 1);
  return res;
}

#if defined(__SNC__)
#pragma diag_pop
#endif

bool LUA_GetFloatField(lua_State* L, int id, const char *pszField, float &value)
{
  lua_getfield(L,id, pszField);
  bool bRes = LUA_GetValue(L, -1, value);  
  lua_pop(L, 1);  
  return bRes;
}


bool LUA_GetValue(lua_State* L, int id, hkvVec3& value)
{
  int iType = lua_type(L,id);
  if (iType <= LUA_TNIL)
  {
    return false;
  }

  VASSERT(lua_isuserdata(L,id));

  swig_lua_userdata *pUserData = (swig_lua_userdata *)lua_touserdata(L,id);
  if (pUserData==NULL || VSWIG_TypeCheckStruct(pUserData->type,SWIGTYPE_p_hkvVec3)==NULL)
    return false;

  value = *((hkvVec3*) pUserData->ptr);

  return true;
}

bool LUA_GetValue(lua_State* L, int id, VColorRef &value)
{
  int iType = lua_type(L,id);
  if (iType <= LUA_TNIL)
  {
    return false;
  }

  VASSERT(lua_isuserdata(L,id));

  swig_lua_userdata *pUserData = (swig_lua_userdata *)lua_touserdata(L,id);
  if (pUserData==NULL || VSWIG_TypeCheckStruct(pUserData->type,SWIGTYPE_p_VColorRef)==NULL)
    return false;

  value = *((VColorRef *)pUserData->ptr);

  return true;
}

bool LUA_GetValue(lua_State* L, int id, hkvAlignedBBox &value)
{
  int iType = lua_type(L,id);
  if (iType <= LUA_TNIL)
  {
    return false;
  }

  VASSERT(lua_isuserdata(L,id));

  swig_lua_userdata *pUserData = (swig_lua_userdata *)lua_touserdata(L,id);
  if (pUserData==NULL || VSWIG_TypeCheckStruct(pUserData->type,SWIGTYPE_p_hkvAlignedBBox)==NULL)
    return false;

  value = *((hkvAlignedBBox *)pUserData->ptr);

  return true;
}

bool LUA_GetValue(lua_State* L, int id, VisTypedEngineObject_cl* &value)
{
  int iType = lua_type(L,id);
  if (iType < 0)
  {
    return false;
  }
  else if (iType==LUA_TNIL)
  {
    value = NULL;
    return true;
  }

  VASSERT(lua_isuserdata(L,id));

  swig_lua_userdata *pUserData = (swig_lua_userdata *)lua_touserdata(L,id);

  if (pUserData==NULL || VSWIG_TypeCheckStruct(pUserData->type,SWIGTYPE_p_VisTypedEngineObject_cl)==NULL)
    return false;
  
  //this is just a weak test because also simple wrapper objects like vectors pass it...
  value = (VisTypedEngineObject_cl *)pUserData->ptr;
  return true;
}

bool LUA_GetValue(lua_State* L, int id, VTypedObject* &value)
{
  int iType = lua_type(L,id);
  if (iType < 0)
  {
    return false;
  }
  else if (iType==LUA_TNIL)
  {
    value = NULL;
    return true;
  }

  VASSERT(lua_isuserdata(L,id));

  swig_lua_userdata *pUserData = (swig_lua_userdata *)lua_touserdata(L,id);

  if (pUserData==NULL || VSWIG_TypeCheckStruct(pUserData->type,SWIGTYPE_p_VTypedObject)==NULL)
    return false;

  //this is just a weak test because also simple wrapper objects like vectors pass it...
  value = (VTypedObject *)pUserData->ptr;
  return true;
}


bool LUA_GetValue(lua_State* L, int id, IVObjectComponent* &value)
{
  int iType = lua_type(L,id);
  if(iType < 0)
  {
    return false;
  }
  else if(iType==LUA_TNIL)
  {
    value = NULL;
    return true;
  }

  VASSERT(lua_isuserdata(L,id));

  swig_lua_userdata *pUserData = (swig_lua_userdata *)lua_touserdata(L,id);

  if(pUserData==NULL || VSWIG_TypeCheckStruct(pUserData->type,SWIGTYPE_p_IVObjectComponent)==NULL)
    return false;

  value = (IVObjectComponent *)pUserData->ptr;

  return true;
}

void* LUA_GetSwigType(lua_State* L, int iStackIndex)
{
  if (lua_isuserdata(L, iStackIndex))
  {
    void *pInstance;

    for (int i = 0; swig_types[i]; i++)
    { //try to cast
      if( VSWIG_Lua_ConvertPtr(L, iStackIndex,(void**)&pInstance, swig_types[i],0) >=0 )
      {
        return swig_types[i];
      }
    }
  }

  return NULL;
}

void* LUA_GetSwigType(lua_State* L, const char * szSwigTypeName)
{
  for (int i = 0; swig_types[i]; i++)
  { 
    if(strcmp(swig_types[i]->str, szSwigTypeName)==0)
      return swig_types[i];
  }

  return NULL;
}

int LUA_PushGlobalUserData(lua_State* L, const char* szVariable)
{
  VASSERT(L);

  VMemoryTempBuffer<512> copyBuffer(szVariable); // operate on a copy string in the tokenizer
  
  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  char* pCurrent = Tokenizer.Next();
  int iStackItems = 0;

  while(pCurrent)
  {
    if(iStackItems == 0)
      lua_getfield(L, LUA_GLOBALSINDEX, pCurrent);
    else
    {
      lua_pushstring(L, pCurrent);
      lua_gettable(L, -2);
    }

    iStackItems++;
    pCurrent = Tokenizer.Next();
    
    //stop if the result is nil
    if(lua_isnil(L,-1)) break;
  }

  //collapse stack if ther is no user data...
  if(lua_isnil(L,-1) || !lua_isuserdata(L,-1))
  {
    lua_pop(L, iStackItems);
    iStackItems = 0;
  }

  return iStackItems;
}


#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=552
#endif

int LUA_CallStaticFunction(  lua_State* L,
                              const char * szModuleName,
                              const char * szTypeName,
                              const char * szFunctionName,
                              const char * szArgFormat,
                              ... )
{
  va_list argPtr;
  va_start(argPtr, szArgFormat);

  int iParams = 0;
  int iReturnParams = -1;

  //prepare for function call
  lua_getglobal(L, szModuleName);                                       //stack: ..., table, TOP
  
  if(lua_isnil(L, -1)) 
  {
    va_end(argPtr);
    return 0;
  }

  //module found - we try to call the static function
  lua_pushfstring(L, "%s_%s", szTypeName, szFunctionName);              //stack: ..., table, fn name, TOP
  lua_gettable(L, -2);                                                  //stack: ..., table, function, TOP

  //return IVObjectComponent if there is no function with that name
  if(lua_isnil(L, -1))                                                  //stack: ..., table, nil?, TOP
  {
    lua_remove(L, -2);                                                  //stack: ..., nil
    va_end(argPtr);
    return 0;
  }

  while (szArgFormat && szArgFormat[0] && iReturnParams == -1)
  {
    char argFormat = *szArgFormat++;

    switch (argFormat) 
    {
      case 'C':  // component
        VSWIG_Lua_NewPointerObj(L, va_arg(argPtr, IVObjectComponent *), SWIGTYPE_p_IVObjectComponent, 0);
        iParams++;
        break;

      case 'O':  // typed engine object
        VSWIG_Lua_NewPointerObj(L, va_arg(argPtr, VisTypedEngineObject_cl *), SWIGTYPE_p_VisTypedEngineObject_cl, 0);
        iParams++;
        break;

      case 'E':  // entity
        VSWIG_Lua_NewPointerObj(L, va_arg(argPtr, VisBaseEntity_cl *), SWIGTYPE_p_VisBaseEntity_cl, 0);
        iParams++;
        break;

      case 'T':  // typed object
        VSWIG_Lua_NewPointerObj(L, va_arg(argPtr, VTypedObject *), SWIGTYPE_p_VTypedObject, 0);
        iParams++;
        break;

      case 'v':  // void * (treat as long)
        VSWIG_Lua_NewPointerObj(L, va_arg(argPtr, unsigned long *), SWIGTYPE_p_unsigned_long, 0);
        iParams++;
        break;

      case '>': //count return params (type is not important)
        iReturnParams = (int)strlen(szArgFormat);
        break;

      default:
        Vision::Error.Warning("Invalid type specifier '%c' for function call '%s'", argFormat, szFunctionName);
        iReturnParams = 0;
        break;
    }
  }                                                                   //stack: ..., table, param0, ..., paramN, TOP

  lua_call(L, iParams, iReturnParams);                                //stack: ..., table, [result,] TOP

  //remove the module table, result remains (if present)
  int iRemoveIndex = -1-iReturnParams;
  lua_remove(L, iRemoveIndex);                                        //stack: ..., [result,] TOP

  va_end(argPtr);

  return iReturnParams;
}

#if defined(__SNC__)
#pragma diag_pop
#endif


////////////////////////////////////////////////////////////////////////
// Small helper to get a look inside the Lua stack
////////////////////////////////////////////////////////////////////////

void LUA_stackDump (lua_State *L)
{
  int i;
  int top = lua_gettop(L);

  if(top==0)
  {
    Vision::Error.SystemMessage("0 elements on the stack");
    return;
  }

  Vision::Error.SystemMessage("%d element(s) on the stack\n====================================",top);

  for (i = 1; i <= top; i++) {  // repeat for each level
    int t = lua_type(L, i);
    switch (t) {

      case LUA_TSTRING:  // strings
        Vision::Error.SystemMessage("%2d string: '%s'", -top+i-1, lua_tostring(L, i));
        break;

      case LUA_TBOOLEAN:  // booleans
        Vision::Error.SystemMessage("%2d bool: %s", -top+i-1, lua_toboolean(L, i) ? "true" : "false");
        break;

      case LUA_TNUMBER:  // numbers
        Vision::Error.SystemMessage("%2d number: %g", -top+i-1, lua_tonumber(L, i));
        break;

      case LUA_TNIL:  // nil
        Vision::Error.SystemMessage("%2d nil", -top+i-1);
        break;

      case LUA_TLIGHTUSERDATA:  // light user data
        Vision::Error.SystemMessage("%2d light user data: 0x%p", -top+i-1, (void *)lua_touserdata(L, i));
        break;

      case LUA_TFUNCTION:  // function
        Vision::Error.SystemMessage("%2d %s function: 0x%p", -top+i-1, lua_iscfunction(L, -1)==TRUE?"c":"lua", lua_topointer(L, -1));
        break;

      case LUA_TTHREAD:  // thread
        Vision::Error.SystemMessage("%2d thread: 0x%p", -top+i-1, (void *)lua_tothread(L, i));
        break;

      case LUA_TTABLE:  // table
        {
          VLuaTableIterator tableIter(L, -1);
          bool bFirst = true;

          while(tableIter.HasNext())
          {
            VScriptMember * pVar = tableIter.Next();

            if(bFirst)
            {
              bFirst = false;
              Vision::Error.SystemMessage("%2d table :0x%p {\n    (%s) %s = %s", -top+i-1, lua_topointer(L, i), pVar->GetType(), pVar->GetName(), pVar->GetValue());
            }
            else
            {
              Vision::Error.SystemMessage("    (%s) %s = %s", pVar->GetType(), pVar->GetName(), pVar->GetValue());
            }
          }

          //was the loop ever executed?
          if(bFirst)
            Vision::Error.SystemMessage("%2d table :0x%p {}", -top+i-1, lua_topointer(L, i));
          else
            Vision::Error.SystemMessage("    }");
        }
        
        break;

      // other values
      default:  
        if(lua_isuserdata(L, i)) //not light user data!
        {
          //check if it is a swig data type
          swig_type_info* type = (swig_type_info *)LUA_GetSwigType(L, i);

          //it is a swig data type if the result is not NULL
          if(type!=NULL)
          {
            Vision::Error.SystemMessage("%2d SWIG Type [%s]: 0x%p", -top+i-1, type->str, (void *)type->clientdata);
            break;
          }
        }
        Vision::Error.SystemMessage("%2d unknown: %s", -top+i-1, lua_typename(L, t));
        break;

    }
  }
}

const char * LUA_GetDataTypeName(lua_State* L, int iStackIndex )
{
  // if the variable is a table, get all child variables (one level deep only)
  switch(lua_type(L, iStackIndex))
  {
    case LUA_TNIL:       return "nil"; //nothing known about the type...
    case LUA_TBOOLEAN:   return "boolean";
    case LUA_TNUMBER:    return "number";
    case LUA_TSTRING:    return "string";
    case LUA_TTABLE:     return "table";    
    case LUA_TFUNCTION:  return "function";
    case LUA_TTHREAD:    return "thread";
    case LUA_TLIGHTUSERDATA: return "lightuserdata";
    default: break; //user data  is left...
  }

  if(lua_isuserdata(L, iStackIndex))
  {
    //check if it is a swig data type
    swig_type_info* type = (swig_type_info *)LUA_GetSwigType(L, iStackIndex);

    //it is a swig data type if the result is not NULL
    if(type!=NULL)
      return type->str;

    return "userdata";
  }

  VASSERT_MSG(false, "VLuaHelper: Discoverd an unknown data type!!!")
  return "unknown";
}

void VLuaTableIterator::init()
{
  m_iStackItems = lua_gettop(m_pLuaState);
  lua_pushnil(m_pLuaState); //to start the iteration  stack: ..., nil, TOP
}

void VLuaTableIterator::deInit()
{
  int iRemoveItems = lua_gettop(m_pLuaState)-m_iStackItems;
  if(iRemoveItems>0) lua_pop(m_pLuaState, iRemoveItems);

  V_SAFE_DELETE(m_pCurrent);
}

bool VLuaTableIterator::hasNext()
{
  int iStackItems = lua_gettop(m_pLuaState);
  int iExpectedItems = m_iStackItems+1;

  if(iStackItems>iExpectedItems)                         // stack: ..., oldKey , oldValue, TOP
    lua_pop(m_pLuaState, iStackItems-iExpectedItems);    // stack: ..., oldKey , TOP

  //is the table specified relative to the top?
  if(m_iTableIndex<0 && m_iTableIndex>LUA_REGISTRYINDEX)
    return lua_next(m_pLuaState, m_iTableIndex-1)==TRUE; // stack: ..., newKey , newValue, TOP
  else
    return lua_next(m_pLuaState, m_iTableIndex)==TRUE;   // stack: ..., newKey , newValue, TOP
}

//this option is disabled because it is very slow and just required for debugging
#define VLUA_DUMP_TABLES
#undef VLUA_DUMP_TABLES
VScriptMember * VLuaTableIterator::next()
{
  char szValue[1024];

  #ifdef VLUA_DUMP_TABLES
    VString sBuffer;
  #endif

  int iType = lua_type(m_pLuaState, -1);
  switch (iType) {
    case LUA_TSTRING:
      {
        const char * szLuaString = lua_tostring(m_pLuaState, -1);
        const size_t sLen = strlen(szLuaString)+1;

        VASSERT_MSG(sLen<1024, "Lua string exceeds 1024 byte temp buffer size and will be truncated!");

        memcpy(szValue, szLuaString, hkvMath::Min(sizeof(char)*1024, sLen));
        szValue[1023] = 0; //terminate in case the length is exceeded
        break;
      }
    case LUA_TBOOLEAN:       strcpy(szValue, lua_toboolean(m_pLuaState, -1) ? "true" : "false"); break;
    case LUA_TNUMBER:        sprintf(szValue, "%f", lua_tonumber(m_pLuaState, -1)); break;
    case LUA_TNIL:           strcpy(szValue, "nil"); break;
    case LUA_TLIGHTUSERDATA: sprintf(szValue, "0x%p",(void *)lua_touserdata(m_pLuaState, -1)); break;
    case LUA_TFUNCTION:      sprintf(szValue, "[%s] 0x%p", lua_iscfunction(m_pLuaState, -1)==TRUE?"c":"lua", lua_topointer(m_pLuaState, -1)); break;
    case LUA_TTHREAD:        sprintf(szValue, "0x%p", (void *)lua_tothread(m_pLuaState, -1)); break;
    case LUA_TTABLE:         
      #ifndef VLUA_DUMP_TABLES
        sprintf(szValue, "0x%p", (void *)lua_topointer(m_pLuaState, -1));
        break;
      #else
        {
          //avoid loops!
          int iStackElems = lua_gettop(m_pLuaState);
          const void * pCurrent = lua_topointer(m_pLuaState,-1);
          for(int i=2/*skip the first, cuz it's the current table*/;i<=iStackElems;i++)
          {
            if(lua_type(m_pLuaState, -i)==LUA_TTABLE && lua_topointer(m_pLuaState,-i)==pCurrent)
            {
              //recursion detected...!
              sBuffer.Format("0x%p {...}", pCurrent);
              break;
            }
          }

          //we can go on investigating the table if no recursion was detected...
          if(sBuffer.IsEmpty())
          {
            VLuaTableIterator tableIter(m_pLuaState, -1);
            bool bFirst = true;

            while(tableIter.HasNext())
            {
              VScriptMember * pVar = tableIter.Next();
              
              if(bFirst)
              {
                bFirst = false;
                sBuffer.Format("0x%p {(%s) %s = %s", pCurrent, pVar->GetType(), pVar->GetName(), pVar->GetValue());
              }
              else
              {
                sBuffer.Format("%s, (%s) %s = %s", sBuffer.AsChar(), pVar->GetType(), pVar->GetName(), pVar->GetValue());
              }
            }

            //was the loop ever executed?
            if(bFirst)
            {
              sBuffer.Format("0x%p {}", pCurrent);
            }
            else
            {
              sBuffer += " }";
            }
          }
        }
        break;
      #endif

    default:  
      if(lua_isuserdata(m_pLuaState, -1)) //not light user data!
      {
        //check if it is a swig data type
        swig_type_info* type = (swig_type_info *)LUA_GetSwigType(m_pLuaState, -1);
        if(type!=NULL)
        {
          sprintf(szValue, "0x%p", (void *)type->clientdata);
          break; //exit the switch!
        }
      }
      sprintf(szValue, "0x%p", (void *)lua_topointer(m_pLuaState, -1)); //unknown
      break;
  }

  //then the key...
  char szKey[512];
  iType = lua_type(m_pLuaState, -2);
  if(iType==LUA_TSTRING)
  {
    const char * szLuaKey =  lua_tostring(m_pLuaState, -2);
    const size_t sLen = strlen(szLuaKey)+1;

    VASSERT_MSG(sLen<512, "Lua key exceeds 512 byte temp buffer size, it will be truncated!");

    memcpy(szKey, szLuaKey, hkvMath::Min(sizeof(char)*512,sLen));
    szKey[511] = 0; //just in case we exceed the length
  }
  else
  {
    sprintf(szKey, "%d", m_iTableIndex);   
  }
  
  V_SAFE_DELETE(m_pCurrent);

  #ifndef VLUA_DUMP_TABLES
    m_pCurrent = new VScriptMember(szKey, szValue, LUA_GetDataTypeName(m_pLuaState, -1));
  #else
    m_pCurrent = new VScriptMember(szKey, sBuffer.IsEmpty() ? szValue : sBuffer.AsChar(), LUA_GetDataTypeName(m_pLuaState, -1));
  #endif

  return m_pCurrent;
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
