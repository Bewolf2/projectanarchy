/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptIncludes.hpp

#ifndef VSCRIPTINCLUDES_HPP_INCLUDED
#define VSCRIPTINCLUDES_HPP_INCLUDED

//This file includes the macros and headers that you need for using the LUA scripting classes

#include <Vision/Runtime/Engine/System/Vision.hpp>

//Lua headers
extern "C" 
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#define LUA_ERRORCHECK(L,status)  VScriptResourceManager::LuaErrorCheck(L,status,NULL)
//this prefix is also used in luarun.swg, so please check also this file before changing the prefix!
#define LUA_HIDDEN_GLOBAL_PREFIX "$node"

//forwards:
class VScriptResource;
class VScriptInstance;
class VScriptResourceManager;

typedef VSmartPtr<VScriptResource> VScriptResourcePtr;
typedef VSmartPtr<VScriptInstance> VScriptInstancePtr;

#if defined (V_DYNLINK_ENABLED)

  #ifdef SCRIPTMODULE_EXPORTS
    #define SCRIPT_IMPEXP V_DYNLINK_EXPORT
  #elif defined SCRIPTMODULE_IMPORTS
    #define SCRIPT_IMPEXP V_DYNLINK_IMPORT
  #else
    #define SCRIPT_IMPEXP V_DYNLINK_NONE
  #endif

#elif defined (V_STATICLINK_ENABLED)
  #define SCRIPT_IMPEXP
#else
  #error "Linkage mode not set!"
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/RSDClient/VRSDClient.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/SwigTypeDataAccessor.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VBitmask.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptRenderer_wrapper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VLuaHelpers.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptInstance.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptResource.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptComponent.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VLuaLib.hpp>

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
