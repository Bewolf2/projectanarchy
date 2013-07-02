/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef AE_UTILITY_SCRIPT_SCRIPTINLINE_H
#define AE_UTILITY_SCRIPT_SCRIPTINLINE_H

namespace V_NS_LUAWRAPPER
{
	inline int VLuaWrapper::ReturnToScript (void) const 
	{
		return (m_States.m_iParametersPushed);
	}

	inline unsigned int VLuaWrapper::getNumberOfFunctionParameters (void) const
	{
		return ((int) lua_gettop (mp_State));
	}

	inline bool VLuaWrapper::isParameterBool (unsigned int iParameter) const
	{
		return (lua_type (mp_State, iParameter + s_ParamOffset) == LUA_TBOOLEAN);
	}

	inline bool VLuaWrapper::isParameterFloat (unsigned int iParameter) const
	{
		return (lua_type (mp_State, iParameter + s_ParamOffset) == LUA_TNUMBER);
	}

	inline bool VLuaWrapper::isParameterInt (unsigned int iParameter) const
	{
		return (lua_type (mp_State, iParameter + s_ParamOffset) == LUA_TNUMBER);
	}

	inline bool VLuaWrapper::isParameterString (unsigned int iParameter) const
	{
		return (lua_type (mp_State, iParameter + s_ParamOffset) == LUA_TSTRING);
	}

	inline bool VLuaWrapper::isParameterNil (unsigned int iParameter) const
	{
		return (lua_type (mp_State, iParameter + s_ParamOffset) == LUA_TNIL);
	}

	inline bool VLuaWrapper::isParameterTable (unsigned int iParameter) const
	{
		return (lua_type (mp_State, iParameter + s_ParamOffset) == LUA_TTABLE);
	}

	inline void VLuaWrapper::PushParameter (int iParameter)
	{
		lua_pushinteger (mp_State, iParameter);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushParameter (bool bParameter)
	{
		lua_pushboolean (mp_State, bParameter);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushParameter (float fParameter)
	{
		lua_pushnumber (mp_State, fParameter);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushParameter (const char* szParameter)
	{
		lua_pushstring (mp_State, szParameter);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushParameter (const char* szParameter, unsigned int length)
	{
		lua_pushlstring (mp_State, szParameter, length);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushParameter (const VString& sParameter)
	{
		lua_pushlstring (mp_State, sParameter.AsChar (), sParameter.GetSize());
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushParameterNil (void)
	{
		lua_pushnil (mp_State);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushReturnValue (int iParameter)
	{
		lua_pushinteger (mp_State, iParameter);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushReturnValue (bool bParameter)
	{
		lua_pushboolean (mp_State, bParameter);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushReturnValue (float fParameter)
	{
		lua_pushnumber (mp_State, fParameter);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushReturnValue (const char* szParameter)
	{
		lua_pushstring (mp_State, szParameter);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushReturnValue (const char* szParameter, unsigned int length)
	{
		lua_pushlstring (mp_State, szParameter, length);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushReturnValue (const VString& sParameter)
	{
		lua_pushlstring (mp_State, sParameter.AsChar (), sParameter.GetSize());
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::PushReturnValueNil (void)
	{
		lua_pushnil (mp_State);
		m_States.m_iParametersPushed++;
	}

	inline void VLuaWrapper::setVariable (const char* szName, int iValue) const
	{
		lua_pushinteger (mp_State, iValue);
		lua_setfield (mp_State, (m_States.m_iOpenTables == 0) ? LUA_GLOBALSINDEX : -2, szName);
	}

	inline void VLuaWrapper::setVariable (const char* szName, float fValue) const
	{
		lua_pushnumber (mp_State, fValue);
		lua_setfield (mp_State, (m_States.m_iOpenTables == 0) ? LUA_GLOBALSINDEX : -2, szName);
	}

	inline void VLuaWrapper::setVariable (const char* szName, bool bValue) const
	{
		lua_pushboolean (mp_State, bValue);
		lua_setfield (mp_State, (m_States.m_iOpenTables == 0) ? LUA_GLOBALSINDEX : -2, szName);
	}

	inline void VLuaWrapper::setVariable (const char* szName, const char* szValue) const
	{
		lua_pushstring (mp_State, szValue);
		lua_setfield (mp_State, (m_States.m_iOpenTables == 0) ? LUA_GLOBALSINDEX : -2, szName);
	}

	inline void VLuaWrapper::setVariable (const char* szName, const char* szValue, unsigned int len) const
	{
		lua_pushlstring (mp_State, szValue, len);
		lua_setfield (mp_State, (m_States.m_iOpenTables == 0) ? LUA_GLOBALSINDEX : -2, szName);
	}

	inline void VLuaWrapper::setVariable (const char* szName, const VString& sValue) const
	{
		lua_pushlstring (mp_State, sValue.AsChar (), sValue.GetSize());
		lua_setfield (mp_State, (m_States.m_iOpenTables == 0) ? LUA_GLOBALSINDEX : -2, szName);
	}

	inline void VLuaWrapper::PushTable (const char* szTableName, bool bGlobalTable)
	{
		lua_pushstring (mp_State, szTableName);
		lua_gettable (mp_State, ((bGlobalTable) || (m_States.m_iOpenTables == 0)) ? LUA_GLOBALSINDEX : -2);
		m_States.m_iParametersPushed++;
	}

	inline int VLuaWrapper::getIntParameter (unsigned int iParameter) const 
	{
		return ((int) (lua_tointeger (mp_State, iParameter + s_ParamOffset)));
	}

	inline bool VLuaWrapper::getBoolParameter (unsigned int iParameter) const
	{
		return (lua_toboolean (mp_State, iParameter + s_ParamOffset) != 0);
	}

	inline float VLuaWrapper::getFloatParameter (unsigned int iParameter) const
	{
		return ((float) (lua_tonumber (mp_State, iParameter + s_ParamOffset)));
	}

	inline const char* VLuaWrapper::getStringParameter (unsigned int iParameter) const 
	{
		return (lua_tostring (mp_State, iParameter + s_ParamOffset));
	}

}

#pragma once

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
