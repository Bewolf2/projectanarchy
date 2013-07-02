/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_PLUGIN_H__
#define RPG_PLUGIN_PLUGIN_H__

// Plugin class
class RPG_Plugin : public IVisPlugin_cl
{
  // IVisPlugin_cl
  void OnInitEnginePlugin() HKV_OVERRIDE;

  void OnDeInitEnginePlugin() HKV_OVERRIDE;

  char const *GetPluginName() HKV_OVERRIDE;
};

// Plugin access
IVisPlugin_cl *GetEnginePlugin_RPGPlugin();

// Plugin module
extern VModule g_RPGPluginModule;

namespace RPG_VisionModuleTypes
{
  void ForceStaticLink();
}

// Declspec macros
#if (defined RPG_PLUGIN_EXPORTS)
#	define RPG_PLUGIN_IMPEXP __declspec(dllexport)
#elif (defined RPG_PLUGIN_IMPORTS)
#	define RPG_PLUGIN_IMPEXP __declspec(dllimport)
#else
#	define RPG_PLUGIN_IMPEXP
#endif

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
