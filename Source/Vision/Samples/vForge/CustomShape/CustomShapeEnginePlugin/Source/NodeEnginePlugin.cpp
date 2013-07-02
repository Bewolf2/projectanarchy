/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

////////////////////////////////////////////////////////////////////////////
//
// This project is a sample engine plugin DLL that contains the classes used
// by the engine and vForge for integrating it into the Vision Engine
// 
////////////////////////////////////////////////////////////////////////////
#include <Vision/Samples/vForge/CustomShape/CustomShapeEnginePlugin/Source/CustomShapeEnginePluginPCH.h>
#include <Vision/Samples/vForge/CustomShape/CustomShapeEnginePlugin/Source/VNode.hpp>


// This class implements the IVisPlugin_cl interface. The engine queries an instance of this class via 
// the exported GetEnginePlugin class.
class DemoEnginePlugin_cl : public IVisPlugin_cl
{
public:

  // this function is only called once since an external init counter takes care
  VOVERRIDE void OnInitEnginePlugin()
  {
    Vision::RegisterModule(&g_NodeModule);
    VNodeMananger_cl::GlobalManager().OneTimeInit();
  }

  // only called once
  VOVERRIDE void OnDeInitEnginePlugin()
  {
    Vision::UnregisterModule(&g_NodeModule);
    VNodeMananger_cl::GlobalManager().OneTimeDeInit();
  }
  
  VOVERRIDE const char *GetPluginName()
  {
    return "NodeEnginePlugin";  //must match DLL name
  }
};


// our global instance of the plugin descriptor:
DemoEnginePlugin_cl g_DemoEnginePlugin;

// this plugin also declares a module which is needed for RTTI/serialization
DECLARE_THIS_MODULE( g_NodeModule, MAKE_VERSION(1, 0),
                     "Nodes", "Havok", "Vision Engine Nodes", &g_DemoEnginePlugin );

////////////////////////////////////////////////////////////////////////////
// These functions are called directly by modules that statically link
// against this library. We give these function unique names so we don't 
// get any naming conflicts with other modules.
////////////////////////////////////////////////////////////////////////////

//  Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_NodeEnginePlugin()
{
  return &g_DemoEnginePlugin;
}

#if (defined _DLL) || (defined _WINDLL)

//  The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin()
{
  return GetEnginePlugin_NodeEnginePlugin();
}

#endif // _DLL or _WINDLL

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
