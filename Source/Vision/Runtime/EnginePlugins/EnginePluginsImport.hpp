/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file EnginePluginsImport.hpp

#ifndef V_IMPORT_ENGINE_PLUGINS_HPP
#define V_IMPORT_ENGINE_PLUGINS_HPP

// We can import all the plugins we need for static linking here 
// We'll only get an error when we use them and they aren't linked in yet
// Use the plugin name (e.g. VisionEnginePlugin) with 
// VISION_PLUGIN_LOAD_STATIC or VISION_PLUGIN_ENSURE_LOADED 

VIMPORT IVisPlugin_cl* GetEnginePlugin_VisionEnginePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_SampleGamePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_SpeedtreeEnginePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_Speedtree5EnginePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_SoundEnginePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vPhysX();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vPhysX3();
VIMPORT IVisPlugin_cl* GetEnginePlugin_KynogonPlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vBullet();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vSubstanceAirCPU();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavok();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokAi();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokCloth();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokDestruction();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokBehavior();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokCharacter();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokVehicle();
VIMPORT IVisPlugin_cl* GetEnginePlugin_RakNetEnginePlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vScaleformPlugin();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin();

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
