/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptInput_wrapper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptDebug_wrapper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptScreen_wrapper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptRenderer_wrapper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptUtil_wrapper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptApp_wrapper.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisionLuaModule_wrapper.hpp>

#if defined(SUPPORTS_MULTITOUCH)
VVirtualThumbStick* VScriptInput_wrapper::s_pVirtualThumbStick = NULL;
#endif

void VLUA_CreateGlobalInstances(lua_State* L)
{
    //at this point we try to avoid access to the globals,
    //which wrap around not fully initialized static classes
    if(Vision::IsInitialized()||Vision::IsInitializing())
    {
      VSWIG_PUSH_PROXY(L, VisGame_cl, &Vision::Game)
      lua_setglobal(L, "Game");

      IVConsoleManager *pConsoleManager = Vision::GetConsoleManager();
      VSWIG_PUSH_PROXY(L, IVConsoleManager, pConsoleManager)
      lua_setglobal(L, "Console");  

      IVTimer *pTimer = Vision::GetTimer();
      VSWIG_PUSH_PROXY(L, IVTimer, pTimer)
      lua_setglobal(L, "Timer");  

      static VScriptApp_wrapper app;
      VSWIG_PUSH_PROXY(L, VScriptApp_wrapper, &app)
      lua_setglobal(L, "Application");

      static VScriptDebug_wrapper debug;
      VSWIG_PUSH_PROXY(L, VScriptDebug_wrapper, &debug)
      lua_setglobal(L, "Debug");

      static VScriptInput_wrapper input;
      VSWIG_PUSH_PROXY(L, VScriptInput_wrapper, &input)
      lua_setglobal(L, "Input");

      static VScriptRenderer_wrapper renderer;
      VSWIG_PUSH_PROXY(L, VScriptRenderer_wrapper, &renderer)
      lua_setglobal(L, "Renderer");

      static VScriptScreen_wrapper screen;
      VSWIG_PUSH_PROXY(L, VScriptScreen_wrapper, &screen)
      lua_setglobal(L, "Screen");

      static VScriptUtil_wrapper util;
      VSWIG_PUSH_PROXY(L, VScriptUtil_wrapper, &util)
      lua_setglobal(L, "Util");

      VGUIManager *pManager = &VGUIManager::GlobalManager();
      VSWIG_PUSH_PROXY(L, VGUIManager, pManager)
      lua_setglobal(L, "GUI");
    }
}

void VLUA_OpenLibraryVArchive(lua_State* L);

/****************************************************************************/
// Registration of libraries and classes
/****************************************************************************/
void VLUA_OpenLibraries (lua_State* L)
{
  //Load SWIG generated modules
  luaopen_Vision(L);

  //add old style varchive handling
  VLUA_OpenLibraryVArchive(L);

  //register swig types for vRSD
  VStrList types;
  LUA_GetSwigTypes(L, &types);
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
