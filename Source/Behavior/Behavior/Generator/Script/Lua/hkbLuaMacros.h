/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_LUA_MACROS_H
#define HKB_LUA_MACROS_H

// This file contains useful macros and utilities for exposing Havok data types in HavokScript.


#include <Behavior/Behavior/Generator/Script/Lua/hkbLuaDefines.h>

#define HKB_SCRIPT_INCLUDE_PATH(root, path) <root/path>

// By default Havok Script symbols are expected to come from the hkbScript library.  If you have purchased Havok Script 
// and you'd prefer they come from elsewhere set HKB_CONFIG_HAVOK_SCRIPT_SYMBOLS to HKB_HAVOK_SCRIPT_SYMBOLS_EXTERNAL
#if HKB_CONFIG_HAVOK_SCRIPT == HKB_HAVOK_SCRIPT_ENABLED

	#if defined( HKB_BUILD_SET_SCRIPT_SYMBOLS_EXTERNAL )

		#define HKB_CONFIG_HAVOK_SCRIPT_SYMBOLS HKB_HAVOK_SCRIPT_SYMBOLS_EXTERNAL
		
		// Must have script path set if linking externally.  Uncomment the next line and set to correct local path
		// #define HKB_HAVOK_SCRIPT_DIRECTORY_PATH ../Tools/HavokScript

		// Used for internal testing only
		#if defined(HKB_BUILD_SET_SCRIPT_DIRECTORY_PATH)
			#define HKB_HAVOK_SCRIPT_DIRECTORY_PATH ../Tools/HavokScript
		#endif

		#if !defined(HKB_HAVOK_SCRIPT_DIRECTORY_PATH)
			#error HKB_HAVOK_SCRIPT_DIRECTORY_PATH must be defined.  This should point at the location of your Havok Script path
		#endif

		// Change this to match your project's build level
		#if !defined(HKS_BUILDRELEASE) && !defined(HKS_BUILDDEVEL) && !defined(HKS_BUILDDEBUG)
			#define	HKS_BUILDDEVEL
		#endif

	#elif defined( HKB_BUILD_SET_SCRIPT_SYMBOLS_DISABLED )

		#define HKB_CONFIG_HAVOK_SCRIPT_SYMBOLS HKB_HAVOK_SCRIPT_SYMBOLS_DISABLED
		#undef HKB_CONFIG_HAVOK_SCRIPT
		#define HKB_CONFIG_HAVOK_SCRIPT HKB_HAVOK_SCRIPT_DISABLED

	#else

		// default
		#define HKB_CONFIG_HAVOK_SCRIPT_SYMBOLS HKB_HAVOK_SCRIPT_SYMBOLS_INTERNAL
		#define HKB_HAVOK_SCRIPT_DIRECTORY_PATH ../Tools/HavokScript
		#if !defined(HKS_BUILDRELEASE) && !defined(HKS_BUILDDEVEL) && !defined(HKS_BUILDDEBUG)
			#define	HKS_BUILDDEVEL
		#endif

	#endif

#else
	#define HKB_CONFIG_HAVOK_SCRIPT_SYMBOLS HKB_HAVOK_SCRIPT_SYMBOLS_DISABLED
#endif

#if HKB_CONFIG_HAVOK_SCRIPT_SYMBOLS == HKB_HAVOK_SCRIPT_SYMBOLS_INTERNAL
	
	namespace hkbInternal
	{
		struct lua_State;
	}

	#define lua_State hkbInternal::lua_State
	#define hkbScriptNamespace hkbInternal

#else

	struct lua_State;
	#define lua_State ::lua_State
	#define hkbScriptNamespace

#endif

// Registers a function hklua_funcname as funcname in Lua
#define HK_LUA_REGISTER_FUNCTION( L, f ) hkbLuaBase::registerFunction( L, #f, hklua_ ## f );

// Useful macro for checking the type of an argument
#define HKLUA_GETTYPE( type, L, n ) type##_check( L, n )

// The macros below are components used when declaring, defining or registering functions
// As macros cannot be easily overloaded, HKlua_StateMENT is defined for 0 through 4 arguments

#define HKLUA_CHECK_NUM_ARGS( type, name, narg ) { checkNumArgs( L, narg, #type ":" #name ); }

#define HKlua_StateMENT0( type, name, rtype, nout, arg1, arg2, arg3, arg4 ) \
	{ rtype ## _push( L, HKLUA_GETTYPE( type, L, 1 ).name() ); return nout; }

#define HKlua_StateMENT1( type, name, rtype, nout, arg1, arg2, arg3, arg4 ) \
	{ rtype ## _push( L, HKLUA_GETTYPE( type, L, 1 ).name( HKLUA_GETTYPE( arg1, L, 2 ) ) ); return nout; }

#define HKlua_StateMENT2( type, name, rtype, nout, arg1, arg2, arg3, arg4 ) \
	{ rtype ## _push( L, HKLUA_GETTYPE( type, L, 1 ).name( HKLUA_GETTYPE( arg1, L, 2 ), HKLUA_GETTYPE( arg2, L, 3 ) ) ); return nout; }

#define HKlua_StateMENT3( type, name, rtype, nout, arg1, arg2, arg3, arg4 ) \
	{ rtype ## _push( L, HKLUA_GETTYPE( type, L, 1 ).name( HKLUA_GETTYPE( arg1, L, 2 ), HKLUA_GETTYPE( arg2, L, 3 ), HKLUA_GETTYPE( arg3, L, 4 ) ) ); return nout; }

#define HKlua_StateMENT4( type, name, rtype, nout, arg1, arg2, arg3, arg4 ) \
	{ rtype ## _push( L, HKLUA_GETTYPE( type, L, 1 ).name( HKLUA_GETTYPE( arg1, L, 2 ), HKLUA_GETTYPE( arg2, L, 3 ), HKLUA_GETTYPE( arg3, L, 4 ), HKLUA_GETTYPE( arg4, L, 5 ) ) ); return nout; }

#define HKLUA_SIGNATURE( type, name ) type ## _ ## name( lua_State* L )


// Below are the three basic macros for exposing functionality:
// Declaring, defining and registering member functions

#define HKLUA_DECLARE( type, name, rtype, nout, narg, arg1, arg2, arg3, arg4 ) static int HK_CALL HKLUA_SIGNATURE( type, name );

#define HKLUA_DEFINE( type, name, rtype, nout, narg, arg1, arg2, arg3, arg4 ) \
	int hkbLuaBase:: HKLUA_SIGNATURE( type, name ) {					\
		HKLUA_CHECK_NUM_ARGS( type, name, ( narg + 1 ) );				\
		HKlua_StateMENT##narg( type, name, rtype, nout, arg1, arg2, arg3, arg4 ); }

#define HKLUA_REGISTER( type, name, rtype, nout, narg, arg1, arg2, arg3, arg4 ) \
	{ #name, type ## _ ## name },

// Special case handling of functions returning void
#define void_push( L, x ) ( x )

#endif // HKB_LUA_MACROS_H

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
