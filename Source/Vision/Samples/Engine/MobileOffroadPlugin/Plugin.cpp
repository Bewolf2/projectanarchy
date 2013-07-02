/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/MobileOffroadPlugin/MobileOffroadPCH.h>
#include <Vision/Samples/Engine/MobileOffroadPlugin/Plugin.h>
#include <Vision/Samples/Engine/MobileOffroadPlugin/MobileOffroadGame.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

#if defined( _VISION_ANDROID )
#include <Common/Base/System/Io/Platform/Android/hkAndroidAssetManagerFileSystem.h>
#endif

VIMPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavok();

// Placeholder class to auto load this plugin from the scene
class MobileOffroadPluginPlaceholder : public VisBaseEntity_cl { V_DECLARE_SERIAL_DLLEXP(MobileOffroadPluginPlaceholder, MOBILEOFFROAD_PLUGIN_IMPEXP); };
V_IMPLEMENT_SERIAL(MobileOffroadPluginPlaceholder, VisBaseEntity_cl, 0, &g_mobileOffroadModule);

//  Use this to get and initialize the plugin when you link statically
IVisPlugin_cl* GetEnginePlugin_MobileOffroadPlugin()
{
  return &g_mobileOffroadPlugin;
}

#if ((defined _DLL) || (defined _WINDLL)) && !defined(VBASE_LIB)
//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_MobileOffroadPlugin();
}
#endif // _DLL or _WINDLL

void MobileOffroadPlugin::OnInitEnginePlugin()
{
  // register our module when the plugin is initialized
  Vision::RegisterModule(&g_mobileOffroadModule);

  VISION_PLUGIN_ENSURE_LOADED(vHavok);
  VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);

#if defined( _VISION_ANDROID )
  // register the internal Android Asset Manager File System with our Android Application
  hkAndroidAssetManagerFileSystem::s_manager = AndroidApplication->activity->assetManager;
#endif

  printf("-------------INIT MobileOffroadPlugin--------------------");

  MobileOffRoadGame::GetInstance().OneTimeInit();
}

void MobileOffroadPlugin::OnDeInitEnginePlugin()
{
  printf("-------------DEINIT MobileOffroadPlugin--------------------");

  MobileOffRoadGame::GetInstance().OneTimeDeInit();

  // de-register our module when the plugin is de-initialized
  Vision::UnregisterModule(&g_mobileOffroadModule);
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
