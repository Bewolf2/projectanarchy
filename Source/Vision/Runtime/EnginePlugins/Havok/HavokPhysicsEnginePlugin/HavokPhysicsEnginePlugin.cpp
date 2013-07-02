/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdoll.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokBallAndSocketConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokHingeConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPrismaticConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdollConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStiffSpringConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokGenericConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPointToPathConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPointToPlaneConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPulleyConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VThrowItemComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VLineFollowerComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VFpsCameraEntity.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp> // redefines new (have to include AFTER Havok headers as they define per class new which will not work if 'new' is #defined to something else

// ***********************************************************************************************
// A Vision plugin that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
//
// ***********************************************************************************************
//
//
// ***********************************************************************************************

//Swig Havok Physics Lua module
extern "C" int luaopen_Physics(lua_State *);

//Lua script registration helpers
static void EnsureHavokScriptRegistration();

class vHavok_cl : public IVisPlugin_cl
{
public:

  void OnInitEnginePlugin();
  void OnDeInitEnginePlugin();
  
  const char *GetPluginName()
  {
    return "vHavok";  //must match DLL name
  }
};

vHavok_cl g_HavokPlugin;

// declare a module for the serialization
DECLARE_THIS_MODULE(g_vHavokModule, MAKE_VERSION(1,0),
                    "Havok Plugin", "Havok",
                    "Module for the Havok binding", &g_HavokPlugin);

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_vHavok()
{
  return &g_HavokPlugin;
}

#if ((defined _DLL) || (defined _WINDLL)) && !defined(VBASE_LIB)
//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_vHavok();
}
#endif // _DLL or _WINDLL




/***************************************************************************
  This file is responsible for automatically registering the
  sample Havok physics module with the engine.
****************************************************************************/

//We register a callback to install the physics module AFTER engine init.
//If we set the physics module in InitEnginePlugin, we might loose it
//if another plugin registers a new application object afterwards.
class VInitHavokPhysicsEngineCallBack : public IVisCallbackHandler_cl
{
  //Handle the callback
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData)
  {
    IVisAppPtr spApp =  Vision::GetApplication();

    if (pData->m_pSender == &Vision::Callbacks.OnEngineInit)
    {
      // Register physics module
      VSmartPtr<vHavokPhysicsModule> spPhysMod = new vHavokPhysicsModule();
      // spPhysMod->SetPhysicsTickCount(50); // since we are throwing objects, we need a high simulation framerate
      bool bSuceeded = spApp->SetPhysicsModule(spPhysMod); 
	    VASSERT_ALWAYS_MSG(bSuceeded, "Couldn't register Havok Physics plugin.");
	  return;
    }
    if(pData->m_pSender==&IVScriptManager::OnRegisterScriptFunctions)
    {
      EnsureHavokScriptRegistration();
      return;
    }
    if (pData->m_pSender==&IVScriptManager::OnScriptProxyCreation)
    {
      VScriptCreateStackProxyObject * pScriptData = (VScriptCreateStackProxyObject *)pData;

      //process data only as far as not handled until now
      if(!pScriptData->m_bProcessed)
      {
        int iRetParams = 0;

        if(pScriptData->m_pInstance->IsOfType(V_RUNTIME_CLASS(vHavokCharacterController)))
        {
          //call lua cast function for vHavokCharacterController which will push the result on the stack top
          iRetParams = LUA_CallStaticFunction(pScriptData->m_pLuaState, "Physics",
            "vHavokCharacterController", "Cast", "C>C", pScriptData->m_pInstance);
        }
        else if(pScriptData->m_pInstance->IsOfType(V_RUNTIME_CLASS(vHavokRigidBody)))
        {
          //call lua cast function for vHavokRigidBody which will push the result on the stack top
          iRetParams = LUA_CallStaticFunction(pScriptData->m_pLuaState, "Physics", 
            "vHavokRigidBody", "Cast", "C>C", pScriptData->m_pInstance);
        }
        else if (pScriptData->m_pInstance->IsOfType(V_RUNTIME_CLASS(vHavokRagdoll)))
        {
          //call lua cast function for vHavokRagdoll which will push the result on the stack top
          iRetParams = LUA_CallStaticFunction(pScriptData->m_pLuaState, "Physics", 
            "vHavokRagdoll", "Cast", "C>C", pScriptData->m_pInstance);
        }

        if(iRetParams>0)
        {
          if(lua_isnil(pScriptData->m_pLuaState, -1))   lua_pop(pScriptData->m_pLuaState, iRetParams);
          else                                          pScriptData->m_bProcessed = true;
        }
      }
      return;
    }
    else 
    {
      if (pData->m_pSender == &Vision::Callbacks.OnEngineDeInit)
      {
        spApp->SetPhysicsModule(NULL);
      }
    }    
  }  
};


VInitHavokPhysicsEngineCallBack cbInit;



// initialize the plugin
void vHavok_cl::OnInitEnginePlugin()
{
  Vision::Callbacks.OnEngineInit.RegisterCallback(&cbInit);
  Vision::Callbacks.OnEngineDeInit.RegisterCallback(&cbInit);

  IVScriptManager::OnRegisterScriptFunctions.RegisterCallback(&cbInit);
  IVScriptManager::OnScriptProxyCreation.RegisterCallback(&cbInit);

  Vision::RegisterModule(&g_vHavokModule);

  // register the Havok module with the vision engine action manager
  VActionManager *pManager = Vision::GetActionManager();
  pManager->RegisterModule(&g_vHavokModule);

  FORCE_LINKDYNCLASS(vHavokRigidBody);
  FORCE_LINKDYNCLASS(vHavokRagdoll);
  FORCE_LINKDYNCLASS(vHavokCharacterController);
  FORCE_LINKDYNCLASS(vHavokBallAndSocketConstraint);
  FORCE_LINKDYNCLASS(vHavokHingeConstraint);
  FORCE_LINKDYNCLASS(vHavokRagdollConstraint);
  FORCE_LINKDYNCLASS(vHavokPrismaticConstraint);
  FORCE_LINKDYNCLASS(vHavokStiffSpringConstraint);
  FORCE_LINKDYNCLASS(vHavokGenericConstraint);
  FORCE_LINKDYNCLASS(vHavokPointToPathConstraint);
  FORCE_LINKDYNCLASS(vHavokPointToPlaneConstraint);
  FORCE_LINKDYNCLASS(vHavokPulleyConstraint);
  FORCE_LINKDYNCLASS(VThrowItemComponent);
  FORCE_LINKDYNCLASS(VLineFollowerComponent);
  FORCE_LINKDYNCLASS(VFpsCameraEntity);

  EnsureHavokScriptRegistration();

  return;
}

// de-initialize the plugin
void vHavok_cl::OnDeInitEnginePlugin()
{
  // Unregister the physics module before the plugin is unloaded.
  // Necessary since the engine will otherwise reference garbage memory.
  IVisAppPtr spApp =  Vision::GetApplication();
  if (spApp->GetPhysicsModule() != NULL && spApp->GetPhysicsModule()->GetType() == IVisPhysicsModule_cl::HAVOK)
  {
    spApp->SetPhysicsModule(NULL);
  }

  Vision::Callbacks.OnEngineInit.DeregisterCallback(&cbInit);
  Vision::Callbacks.OnEngineDeInit.DeregisterCallback(&cbInit);

  IVScriptManager::OnRegisterScriptFunctions.DeregisterCallback(&cbInit);
  IVScriptManager::OnScriptProxyCreation.DeregisterCallback(&cbInit);

  // Unregister the Havok module with the vision engine action manager
  VActionManager *pManager = Vision::GetActionManager();
  pManager->UnregisterModule(&g_vHavokModule);

  Vision::UnregisterModule(&g_vHavokModule);
}

//Helper function to ensure that the Havok Physics script module has been loaded
static void EnsureHavokScriptRegistration()
{
  IVScriptManager* pSM = Vision::GetScriptManager();
  if(pSM)
  {
    lua_State* pLuaState = ((VScriptResourceManager*)pSM)->GetMasterState();
    if(pLuaState)
    {
      luaopen_Physics(pLuaState);
    }
    else
    {
      Vision::Error.Warning("Unable to create Lua Havok Physics Module, lua_State is NULL");
    }
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
