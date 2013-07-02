/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/FmodEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

class vFmodEnginePlugin : public IVisPlugin_cl
{
public:

  void OnInitEnginePlugin();
  void OnDeInitEnginePlugin();
  
  const char *GetPluginName()
  {
    return "vFmodEnginePlugin";
  }
  
};

vFmodEnginePlugin g_FmodEnginePlugin;


// module for serialization/RTTI
DECLARE_THIS_MODULE( g_FmodModule, MAKE_VERSION(1, 0),
                     "Fmod", "Havok", "Havok Fmod", &g_FmodEnginePlugin );

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin()
{
  return &g_FmodEnginePlugin;
}

#if ((defined _DLL) || (defined _WINDLL)) && !defined(VBASE_LIB)
//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_vFmodEnginePlugin();
}
#endif // _DLL or _WINDLL



////////////////////////////////////////////////////////////////////////////
// These functions are called directly by modules that statically link
// against this library. We give these function unique names so we don't 
// get any naming conflicts with other modules.
////////////////////////////////////////////////////////////////////////////

void vFmodEnginePlugin::OnInitEnginePlugin()
{
  // this registers the module for serialization and the resource manager
  VFmodManager::GlobalManager().OneTimeInit();

  FORCE_LINKDYNCLASS(VFmodCollisionMeshInstance);
  FORCE_LINKDYNCLASS(VFmodEvent);
  FORCE_LINKDYNCLASS(VFmodReverb);
  FORCE_LINKDYNCLASS(VFmodSoundObject);
}


// plugin deinit function called by the engine
void vFmodEnginePlugin::OnDeInitEnginePlugin()
{
   // unregister all
  VFmodManager::GlobalManager().OneTimeDeInit();
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
