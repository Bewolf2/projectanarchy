/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/HavokBehaviorEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp> // redefines new (have to include AFTER Havok headers as they define per class new which will not work if 'new' is #defined to something else

VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavok();

// ***********************************************************************************************
// A Vision plugin that uses Havok for Behavior
// ***********************************************************************************************
class vHavokBehavior_cl : public IVisPlugin_cl
{
public:

	virtual void OnInitEnginePlugin() HKV_OVERRIDE;
	virtual void OnDeInitEnginePlugin() HKV_OVERRIDE;

	virtual const char *GetPluginName() HKV_OVERRIDE
	{
		return "vHavokBehavior";  //must match DLL name
	}

};

vHavokBehavior_cl g_HavokBehaviorPlugin;

// declare a module for the serialization
DECLARE_THIS_MODULE(g_vHavokBehaviorModule, MAKE_VERSION(1,0),
                    "Havok Behavior Plugin", "Havok",
                    "Module for the Havok Behavior binding", &g_HavokBehaviorPlugin);

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_vHavokBehavior()
{
  return &g_HavokBehaviorPlugin;
}

#if ((defined _DLL) || (defined _WINDLL)) && !defined(VBASE_LIB)
//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_vHavokBehavior();
}
#endif // _DLL or _WINDLL


/***************************************************************************
  This file is responsible for automatically registering the
  Havok Behavior module with the engine.
****************************************************************************/

// Initialize the plugin
void vHavokBehavior_cl::OnInitEnginePlugin()
{
	Vision::Error.SystemMessage( "vHavokBehavior_cl:OnInitEnginePlugin()" );

#if ( (defined _DLL) || (defined _WINDLL) ) && !defined(VBASE_LIB)
	// This plugin depends on the main Havok Physics plugin being loaded
	VISION_PLUGIN_ENSURE_LOADED( vHavok );
#endif
	
	// Register to sync statics after the main Havok plugin
	Vision::RegisterModule(&g_vHavokBehaviorModule);

	vHavokBehaviorModule* module = vHavokBehaviorModule::GetInstance();

#ifdef VLUA_USE_SWIG_BINDING
	IVScriptManager::OnRegisterScriptFunctions += module;
	IVScriptManager::OnScriptProxyCreation += module;
#endif

	// Register physics callbacks
	vHavokPhysicsModule::OnBeforeInitializePhysics += module;
	vHavokPhysicsModule::OnAfterDeInitializePhysics += module;
	vHavokPhysicsModule::OnUnsyncHavokStatics += module;
	vHavokPhysicsModule::OnBeforeWorldCreated += module;

	// Register VDB callbacks
	vHavokVisualDebugger::OnCreatingContexts += module;
	vHavokVisualDebugger::OnAddingDefaultViewers += module;

	FORCE_LINKDYNCLASS(vHavokBehaviorComponent);

	EnsureHavokBehaviorScriptRegistration();
}

// De-initialize the plugin
void vHavokBehavior_cl::OnDeInitEnginePlugin()
{
	Vision::Error.SystemMessage( "vHavokBehavior_cl:OnDeInitEnginePlugin()" );

	vHavokBehaviorModule* module = vHavokBehaviorModule::GetInstance();
	if ( module )
	{
		vHavokPhysicsModule::OnBeforeInitializePhysics -= module;
		vHavokPhysicsModule::OnAfterDeInitializePhysics -= module;
		vHavokPhysicsModule::OnUnsyncHavokStatics -= module;
		vHavokPhysicsModule::OnBeforeWorldCreated -= module;
		vHavokVisualDebugger::OnCreatingContexts -= module;
		vHavokVisualDebugger::OnAddingDefaultViewers -= module;

#ifdef VLUA_USE_SWIG_BINDING
		IVScriptManager::OnRegisterScriptFunctions -= module;
		IVScriptManager::OnScriptProxyCreation -= module;
#endif

	}

	Vision::UnregisterModule(&g_vHavokBehaviorModule);

	// To match the VISION_PLUGIN_ENSURE_LOADED
	GetEnginePlugin_vHavok()->DeInitEnginePlugin();
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
