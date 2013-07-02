/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRSDLuaImplementation.hpp

/// This file will be part of a future Vision SDK and is not relevant yet
#if !defined _VISION_DOC


#ifndef VRSDCLIENTLUAIMPLEMENTATION_HPP_INCLUDED
#define VRSDCLIENTLUAIMPLEMENTATION_HPP_INCLUDED

  /// \brief
  ///   The Lua language implementation for the remote script debugger
  class VRSDClientLuaImplementation : public IVRSDClientLanguageImplementation
  {
    public:

      VRSDClientLuaImplementation();
      ~VRSDClientLuaImplementation();

      virtual bool GetCallstack(DynArray_cl<VRSDClientCallstackEntry>& Callstack, unsigned int& CallstackEntryCount);
      virtual bool GetLocalSymbols(DynArray_cl<VRSDScriptSymbol>& LocalSymbols, unsigned int& LocalSymbolCount);
      virtual bool GetGlobalSymbols(DynArray_cl<VRSDScriptSymbol>& GlobalSymbols, unsigned int& GlobalSymbolCount);
      virtual bool IsImplementation(const char* Name);

      virtual bool GetSubSymbolsForGlobal(char* GlobalName, DynArray_cl<VRSDScriptSymbol>& SubSymbols, unsigned int& SubSymbolCount);
      virtual bool GetSubSymbolsForLocal(char* LocalName, DynArray_cl<VRSDScriptSymbol>& SubSymbols, unsigned int& SubSymbolCount);

      virtual bool IsGlobalUserDataOfType(const char* Name, const char* Type);
      virtual bool IsLocalUserDataOfType(const char* Name, const char* Type);
     
      virtual bool GetGlobalType(const char*pVariableName, char * pUserDataTypeName);
      virtual bool GetLocalType(const char* pVariableName, char * pUserDataTypeName);

      virtual bool GetUserDataPointerFromGlobal(const char* szVariable, void** ppUserData, void** ppEnvironment);
      virtual bool GetUserDataPointerFromLocal(const char* szVariable, void** ppUserData, void** ppEnvironment);

      virtual bool UpdateLocalVariable(const char* Variable, const char* NewValue);
      virtual bool UpdateGlobalVariable(const char* Variable, const char* NewValue);
      virtual bool UpdateHiddenGlobalVariable(const void* pUserDataParent, const char* Variable, const char* NewValue);

      void ControlDebugHook(bool Enable);

      static void Lua_DebugHook(lua_State* L, lua_Debug* ar);

      void SetDebugInformation(lua_State* L, lua_Debug* ar);

      void ErrorScriptEvent(VRSDScriptEvent* Event);

    private:

      void AddSymbol(DynArray_cl<VRSDScriptSymbol>& Symbols, unsigned int& SymbolCount, const char* pSymbolName, const char* pSymbolContent, VRSDScriptSymbol::SymbolType SymbolType);

      static const char* GetCorrectLuaSourceString(const char* pIn)
      {
        // Need to handle the '@' sign otherwise step-into won't work (file not located error)!
        // From LUA documentation:
        // "source: If the function was defined in a string, then source is that string. If the 
        // function was defined in a file, then source starts with a '@' followed by the file name."

        return (pIn && pIn[0] == '@') ? pIn + 1 : pIn;
      }

    protected:

      virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

      lua_State* m_pLuaState;
      lua_Debug* m_pActivationRecord;

  };

#endif

#endif //_VISION_DOC

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
