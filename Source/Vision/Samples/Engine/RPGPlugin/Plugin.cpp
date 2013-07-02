/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/MeshTrailEffectComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponentManager.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionTimer.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>

#include <Ai/Pathfinding/Character/Behavior/hkaiPathFollowingBehavior.h>

VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokAi();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokBehavior();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin();

// Plugin class
namespace
{
  RPG_Plugin s_Plugin;

  class RPG_PluginInitializer : public IVisCallbackHandler_cl
  {
  protected:
    // IVisCallbackHandler_cl
    void OnHandleCallback(IVisCallbackDataObject_cl *callback_data) HKV_OVERRIDE;
  } s_PluginInitializer;

  // Plugin_OnEngineInit
  class Plugin_OnEngineInit : public IVisCallbackHandler_cl
  {
    // IVisCallbackHandler_cl
    void OnHandleCallback(IVisCallbackDataObject_cl *callbackData) HKV_OVERRIDE;
  }
  s_Plugin_OnEngineInit;

  // Plugin_OnEngineDeInit
  class Plugin_OnEngineDeInit : public IVisCallbackHandler_cl
  {
    // IVisCallbackHandler_cl
    void OnHandleCallback(IVisCallbackDataObject_cl *callbackData) HKV_OVERRIDE;
  }
  s_Plugin_OnEngineDeInit;

  IVTimerPtr s_Plugin_Timer;

  int const kFPS = 30;
}

DECLARE_THIS_MODULE(g_RPGPluginModule, MAKE_VERSION(0, 1), "RPGPlugin", "Havok", "Havok RPG Plugin", &s_Plugin);

IVisPlugin_cl *GetEnginePlugin_RPGPlugin()
{
  return &s_Plugin;
}

#ifndef VBASE_LIB
VEXPORT IVisPlugin_cl *GetEnginePlugin()
{
  return GetEnginePlugin_RPGPlugin();
}
#endif
#if (RPG_UI_SCALEFORM)
extern "C" IVisPlugin_cl* GetEnginePlugin_vScaleformPlugin();
#endif
void RPG_Plugin::OnInitEnginePlugin()
{
  // Module
  Vision::Error.SystemMessage("RPGPlugin:OnInitEnginePlugin()");
  Vision::RegisterModule(&g_RPGPluginModule);
  RPG_VisionModuleTypes::ForceStaticLink();

  // Timer
  s_Plugin_Timer = new RPG_VisionTimer;

  // Havok AI plugin (chains to Havok plugin)
  VISION_PLUGIN_ENSURE_LOADED(vHavokAi);
  VISION_PLUGIN_ENSURE_LOADED(vHavokBehavior);

  // Fmod
  VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);

  // Scaleform
#if (RPG_UI_SCALEFORM)
  GetEnginePlugin_vScaleformPlugin()->InitEnginePlugin();
  VScaleformManager::GlobalManager().EnableMultithreadedAdvance(false);
#endif

  // Hook engine init
  Vision::Callbacks.OnEngineInit += &s_Plugin_OnEngineInit;
  Vision::Callbacks.OnEngineDeInit += &s_Plugin_OnEngineDeInit;

  // Hook physics init
  vHavokPhysicsModule::OnBeforeInitializePhysics += &s_PluginInitializer;
  vHavokPhysicsModule::OnBeforeWorldCreated += &s_PluginInitializer;
  vHavokPhysicsModule::OnAfterDeInitializePhysics += &s_PluginInitializer;

  // Local systems
  RPG_GameManager::s_instance.OneTimeInit();
  RPG_HighlightableComponentManager::s_instance.OneTimeInit();
  RPG_MeshTrailEffectComponentManager::s_instance.OneTimeInit();
}

#if (RPG_DUMP_CRT_OBJECTS_ON_EXIT)
# include <crtdbg.h>
#endif

void RPG_Plugin::OnDeInitEnginePlugin()
{
#if (RPG_DUMP_CRT_OBJECTS_ON_EXIT)
  _CrtMemDumpAllObjectsSince(NULL);
#endif

  // Local systems
  RPG_GameManager::s_instance.OneTimeDeInit();
  RPG_MeshTrailEffectComponentManager::s_instance.OneTimeDeInit();
  RPG_HighlightableComponentManager::s_instance.OneTimeDeInit();

  // Unhook physics init
  vHavokPhysicsModule::OnAfterDeInitializePhysics -= &s_PluginInitializer;
  vHavokPhysicsModule::OnBeforeWorldCreated -= &s_PluginInitializer;
  vHavokPhysicsModule::OnBeforeInitializePhysics -= &s_PluginInitializer;

  // Unhook engine init
  Vision::Callbacks.OnEngineDeInit -= &s_Plugin_OnEngineDeInit;
  Vision::Callbacks.OnEngineInit -= &s_Plugin_OnEngineInit;

  // Scaleform
#if (RPG_UI_SCALEFORM)
  GetEnginePlugin_vScaleformPlugin()->DeInitEnginePlugin();
#endif
  // Timer
  s_Plugin_Timer = NULL;

  // Module
  Vision::Error.SystemMessage("RPGPlugin:OnDeInitEnginePlugin()");
  Vision::UnregisterModule(&g_RPGPluginModule);
}

char const *RPG_Plugin::GetPluginName()
{
  // Must match vPlugin name
  return "RPGPlugin";
}

void RPG_PluginInitializer::OnHandleCallback(IVisCallbackDataObject_cl *callback_data)
{
  if(&vHavokPhysicsModule::OnBeforeInitializePhysics == callback_data->m_pSender)
  {
    VISION_HAVOK_SYNC_STATICS();
    VISION_HAVOK_SYNC_PER_THREAD_STATICS(static_cast<vHavokPhysicsModuleCallbackData *>(callback_data)->GetHavokModule());

    // hkaiCharacter created by this dll will have a different vtable than the one expected by the AI module (for vdb viewer)
    // The vtable-class(?) mapping is apparently many-to-one, so re-register here with our vtable
    // (Actually the vtables in this module and the vtables elsewhere are unique, so being many-to-one is a happy accident)
    // TODO redundant in the statically linked case
    hkVtableClassRegistry::getInstance().registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos, hkBuiltinTypeRegistry::StaticLinkedClasses);
  }
  else if(&vHavokPhysicsModule::OnBeforeWorldCreated == callback_data->m_pSender)
  {
    vHavokPhysicsModule *const havok_module = static_cast<vHavokBeforeWorldCreateDataObject_cl *>(callback_data)->GetHavokModule();
    {
      havok_module->SetUseAsynchronousPhysics(false);

#if defined (_VISION_MOBILE)
      havok_module->SetPhysicsTickCount(kFPS, 1, true);
#else
      havok_module->SetPhysicsTickCount(kFPS);
#endif
      havok_module->SetEnabledVisualDebugger(TRUE);
    }
  }
  else if(&vHavokPhysicsModule::OnAfterDeInitializePhysics == callback_data->m_pSender)
  {
    // TODO
    vHavokAiModule::GetInstance()->getCharacterBehaviors().clearAndDeallocate();

    VISION_HAVOK_UNSYNC_PER_THREAD_STATICS(static_cast<vHavokPhysicsModuleCallbackData *>(callback_data)->GetHavokModule());
    VISION_HAVOK_UNSYNC_STATICS();
  }
}

void Plugin_OnEngineInit::OnHandleCallback(IVisCallbackDataObject_cl *callbackData)
{
#if defined (_VISION_MOBILE)
  Vision::GetApplication()->SetSceneUpdateController(new VFixStepSceneUpdateController(kFPS, 1));
#else
  Vision::GetApplication()->SetSceneUpdateController(new VFixStepSceneUpdateController(kFPS, 10));
#endif
  Vision::SetTimer(new VFixStepTimer(kFPS));
}

void Plugin_OnEngineDeInit::OnHandleCallback(IVisCallbackDataObject_cl *callbackData)
{
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
