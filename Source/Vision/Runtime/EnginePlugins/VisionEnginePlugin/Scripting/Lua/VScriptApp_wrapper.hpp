/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptRenderer_wrapper.hpp

#ifndef __VSCRIPTAPP_WRAPPER_HPP
#define __VSCRIPTAPP_WRAPPER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptFileSystem_wrapper.hpp>

/// 
/// \brief
///   Small wrapper to unify utility functions inside the scripting binding
/// 
class VScriptApp_wrapper
{
public:
  VScriptApp_wrapper() {}
  ~VScriptApp_wrapper() {}

  VScriptFileSystem_wrapper FileSystem;

  bool LoadScene(const char * szFileName)
  {
    VSceneLoader loader;
    return loader.LoadScene(szFileName);
  }

  void UnloadScene()
  {
    VSceneLoader::UnloadScene();
  }

  void ClearScene()
  {
    VSceneLoader::ClearScene();
  }

  bool IsInEditor()
  {
    return Vision::Editor.IsInEditor(); 
  }

  int GetEditorMode()
  {
    return (int)Vision::Editor.GetMode();
  }
  
  const char * GetPlatformName()
  {
    #if defined(_VISION_PS3)
	    return "PS3";
  	#elif defined(_VISION_PSP2)
  	  return "PSVITA";
  	#elif defined(_VISION_XENON)
  	  return "XBOX360";
  	#elif defined(_VISION_ANDROID)
  	  return "ANDROID";
  	#elif defined(_VISION_IOS)
  	  return "IOS";
  	#elif defined(_VISION_WIIU)
  	  return "WIIU";
  	#elif defined(_VISION_POSIX)
  	  return "POSIX";
  	#elif defined(_VR_DX9)
   	  #if defined(_WIN32)
   	    return "WIN32DX9";
   	  #else
   	    return "WIN64DX9";
   	  #endif
  	#elif defined(_VR_DX11)
  	  #if defined(_WIN32)
  	    return "WIN32DX11";
  	  #else
  	    return "WIN64DX11";
  	  #endif
  	#else
  	  return "UNDEFINED";
  	#endif
  }

  static bool LoadScript(lua_State *L, const char * szFileName) 
  {
    IVFileInStream *pIn = Vision::File.Open(szFileName);
    if (!pIn) return false;

    int iScriptLen = pIn->GetSize();
    VMemoryTempBuffer<16*1024> buffer(iScriptLen+1);
    char *szBuffer = (char *)buffer.GetBuffer();
    pIn->Read(szBuffer,iScriptLen);
    szBuffer[iScriptLen] = 0;
    pIn->Close();

    if (!LUA_ERRORCHECK(L, luaL_loadbuffer(L, szBuffer, iScriptLen, szFileName)))
      return false;

    if (!LUA_ERRORCHECK(L, lua_pcall (L, 0, LUA_MULTRET, 0)))
      return false;

    return true;
  }

};

#endif // __VSCRIPTAPP_WRAPPER_HPP

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
