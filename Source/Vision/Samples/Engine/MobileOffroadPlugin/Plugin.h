/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef MOBILEOFFROAD_PLUGIN_H
#define MOBILEOFFROAD_PLUGIN_H

// Declspec macros
#if (defined MOBILEOFFROAD_PLUGIN_EXPORTS)
#	define MOBILEOFFROAD_PLUGIN_IMPEXP __declspec(dllexport)
#elif (defined MOBILEOFFROAD_PLUGIN_IMPORTS)
#	define MOBILEOFFROAD_PLUGIN_IMPEXP __declspec(dllimport)
#else
#	define MOBILEOFFROAD_PLUGIN_IMPEXP
#endif

class MobileOffroadPlugin : public IVisPlugin_cl
{
public:

  void OnInitEnginePlugin();
  void OnDeInitEnginePlugin();

  const char *GetPluginName()
  {
    return "MobileOffroadPlugin";  //must match DLL name
  }

};

MobileOffroadPlugin g_mobileOffroadPlugin;

// declare a module for the serialization
DECLARE_THIS_MODULE(g_mobileOffroadModule, MAKE_VERSION(1,0),
"Mobile Offroad Plugin", "Havok",
  "", &g_mobileOffroadPlugin);

IVisPlugin_cl* GetEnginePlugin_MobileOffroadPlugin();

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
