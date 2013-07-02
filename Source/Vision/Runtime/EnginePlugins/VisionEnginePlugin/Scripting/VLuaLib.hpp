/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLuaLib.hpp

#ifndef VLUALIB_HPP_INCLUDED
#define VLUALIB_HPP_INCLUDED

// SWIG Generated Vision module containing the lua wrappers
extern "C" int luaopen_Vision(lua_State* L);

//the engine lib supports loading of Game, Messages, ...
SCRIPT_IMPEXP extern const luaL_Reg VisionEngineLib[];

//Called after engine initialization to provide global static objects in lua
//and data accessors for SWIG types
SCRIPT_IMPEXP void VLUA_CreateGlobalInstances(lua_State* L);

//Called during initialization to register our libraries and classes
SCRIPT_IMPEXP void VLUA_OpenLibraries (lua_State* L);

#endif //VLUALIB_HPP_INCLUDED

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
