/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/VBase.hpp>

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

class vLuaScript
{
public:
  vLuaScript (void);
  vLuaScript (lua_State* s);
  ~vLuaScript ();
  
  void Clear (void);

  bool ExecuteFile (const char* szFile, VString& out_sError) const;
  bool ExecuteString (const VString& sString, VString& out_sError) const;

  unsigned int getNumberOfFunctionParameters (void) const;

  void setVariable (const char* szName, int iValue) const;
  void setVariable (const char* szName, bool bValue) const;

  int getIntParameter (unsigned int iParameter) const;
  const char* getStringParameter (unsigned int iParameter) const;

  void RegisterCFunction (const char* szFunctionName, lua_CFunction pFunction) const;

private:
  lua_State* mp_State;
  bool m_bReleaseOnExit;
};


inline unsigned int vLuaScript::getNumberOfFunctionParameters (void) const
{
  return ((int) lua_gettop (mp_State));
}

inline void vLuaScript::setVariable (const char* szName, int iValue) const
{
  lua_pushinteger (mp_State, iValue);
  lua_setfield (mp_State, LUA_GLOBALSINDEX, szName);
}

inline void vLuaScript::setVariable (const char* szName, bool bValue) const
{
  lua_pushboolean (mp_State, bValue);
  lua_setfield (mp_State, LUA_GLOBALSINDEX, szName);
}

inline int vLuaScript::getIntParameter (unsigned int iParameter) const 
{
  return ((int) (lua_tointeger (mp_State, iParameter)));
}

inline const char* vLuaScript::getStringParameter (unsigned int iParameter) const 
{
  return (lua_tostring (mp_State, iParameter));
}

inline void vLuaScript::RegisterCFunction (const char* szFunctionName, lua_CFunction pFunction) const
{
  lua_pushcfunction (mp_State, pFunction);
  lua_setglobal (mp_State, szFunctionName);
}

inline vLuaScript::vLuaScript (void)
{
  mp_State = NULL;
  m_bReleaseOnExit = true;

  Clear ();
}

inline vLuaScript::vLuaScript (lua_State* s)
{
  mp_State = s;
  m_bReleaseOnExit = false;
}

inline vLuaScript::~vLuaScript ()
{
  if (m_bReleaseOnExit)
    lua_close (mp_State);
}

inline void vLuaScript::Clear (void)
{
  if (!m_bReleaseOnExit)
    return;

  if (mp_State)
    lua_close (mp_State);

  mp_State = luaL_newstate ();
  luaL_openlibs (mp_State);
}

inline bool vLuaScript::ExecuteFile (const char* szFile, VString& out_sError) const
{
  const int iKB = 64;
  const int iBufferSize = 1024*iKB+1;

  VString sBuffer;
  char szBuffer[iBufferSize] = "";

  IVFileInStream *pFile = Vision::File.Open (szFile);

  VASSERT (pFile != NULL);
  
  if (!pFile)
    return (false);

  while (!pFile->IsEOF ())
  {
    size_t iRead = pFile->Read (szBuffer, iBufferSize-1);
    szBuffer[iRead] = '\0';
    sBuffer += szBuffer;
  }

  pFile->Close ();

  bool bRes = ExecuteString (sBuffer, out_sError);

  if (!bRes)
    printf ("%s", out_sError.AsChar ());

  return (bRes);
}  


inline bool vLuaScript::ExecuteString (const VString& sString, VString& out_sError) const
{
  int error = luaL_loadbuffer (mp_State, sString.AsChar(), sString.GetSize(), "chunk");
      
  if (error != 0)
  {
    out_sError = lua_tostring (mp_State, -1);
    return (false);
  }

  error = lua_pcall (mp_State, 0, 0, 0);

  if (error != 0)
  {
    out_sError = lua_tostring (mp_State, -1);
    return (false);
  }

  return (true);
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
