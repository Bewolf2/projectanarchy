/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef AE_UTILITY_SCRIPT_SCRIPT_H
#define AE_UTILITY_SCRIPT_SCRIPT_H

#include <Vision/Runtime/Base/VBase.hpp>

extern "C"
{
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}


namespace V_NS_LUAWRAPPER
{
	/*! This class encapsulates ONE Lua-Script. It makes it easier to to interact with the script, to get data
		out of it (configuration files), to register C-functions to it and to call script-functions. It is possible
		to load more than one Lua-File into one Lua-Script, one can dynamically generate code and pass it as
		a string to the script. It ALSO allows to construct the VLuaWrapper with a working lua_State-Pointer and thus
		only simplify interaction with an already existing Script (for example, when a C-Function is called in a Script,
		it passes its lua_State to that Function). 
		\note Lua starts counting at 1, not at 0! VLuaWrapper does this too! That means, when you query the first parameter or return-value
		passed to your function, you need to query for value 1, not for value 0.
	*/
	class VLuaWrapper
	{
	public:
		/// \brief Generates a NEW Lua-Script, which is empty.
		VLuaWrapper (void);
		/// \brief Takes an EXISTING Lua-Script and allows to get easier access to it.
		VLuaWrapper (lua_State* s);
		/// \brief Destroys the Lua-Script, if it was created, but leaves it intact, if this instance did not generate the Lua-Script.
		~VLuaWrapper ();

    /// \brief Closes the script and reopens it, thus resetting any state.
    void Clear (void);

		/// \brief Loads a file into the Script. Returns false, if the file contains errors or cannot be opened.
		bool ExecuteFile (const char* szFile, VString& out_sError) const;
		/// \brief Executes a string containing Lua-Code. Returns true, if no errors occurred. Returns false otherwise.
		bool ExecuteString (const VString& sString, VString& out_sError) const;

		/// \brief Opens a Lua-Table inside the Script, with the given name. Needs to be in scope. Returns false, if it's not possible (the table does not exist...).
		bool OpenTable (const char* szTable);
		/// \brief Opens the Table n, that was passed to a C-Function on its Parameter-Stack. Start counting at 1, not 0.
		bool OpenTable (unsigned int iFunctionParameter);

		/// \brief Closes the table opened last.
		void CloseTable (void);
		/// \brief Closes all open Tables.
		void CloseAllTables (void);

		/// \brief Pushes an existing table onto Luas Stack. Either one that is in local scope, or a global table.
		void PushTable (const char* szTableName, bool bGlobalTable);

		/// \brief Returns how many Parameters were passed to the called C-Function.
		unsigned int getNumberOfFunctionParameters (void) const;

		/// \brief Checks, whether the Variable with the given Name exists.
		bool isVariableAvailable (const char* szVariable) const;
		/// \brief Checks, whether the Function with the given Name exists.
		bool isFunctionAvailable (const char* szFunction) const;

		/// \brief Checks the nth Parameter passed to a C-Function for its type. Start counting at 1, not 0.
		bool isParameterInt (unsigned int iParameter) const;
		/// \brief Checks the nth Parameter passed to a C-Function for its type. Start counting at 1, not 0.
		bool isParameterBool (unsigned int iParameter) const;
		/// \brief Checks the nth Parameter passed to a C-Function for its type. Start counting at 1, not 0.
		bool isParameterFloat (unsigned int iParameter) const;
		/// \brief Checks the nth Parameter passed to a C-Function for its type. Start counting at 1, not 0.
		bool isParameterTable (unsigned int iParameter) const;
		/// \brief Checks the nth Parameter passed to a C-Function for its type. Start counting at 1, not 0.
		bool isParameterString (unsigned int iParameter) const;
		/// \brief Checks the nth Parameter passed to a C-Function for its type. Start counting at 1, not 0.
		bool isParameterNil (unsigned int iParameter) const;

		/// \brief Sets the Variable with the given name (in scope) with the given value.
		void setVariable (const char* szName, int iValue) const;
		/// \brief Sets the Variable with the given name (in scope) with the given value.
		void setVariable (const char* szName, bool bValue) const;
		/// \brief Sets the Variable with the given name (in scope) with the given value.
		void setVariable (const char* szName, float fValue) const;
		/// \brief Sets the Variable with the given name (in scope) with the given value.
		void setVariable (const char* szName, const VString& szValue) const;
		/// \brief Sets the Variable with the given name (in scope) with the given value.
		void setVariable (const char* szName, const char* szValue) const;
		/// \brief Sets the Variable with the given name (in scope) with the given value.
		void setVariable (const char* szName, const char* szValue, unsigned int len) const;

		/// \brief Returns the Value of the Variable with the given name, or the default-value, if it does not exist.
		int getIntVariable (const char* szName, int iDefault = 0) const;
		/// \brief Returns the Value of the Variable with the given name, or the default-value, if it does not exist.
		bool getBoolVariable (const char* szName, bool bDefault = false) const;
		/// \brief Returns the Value of the Variable with the given name, or the default-value, if it does not exist.
		float getFloatVariable (const char* szName, float fDefault = 0.0f) const;
		/// \brief Returns the Value of the Variable with the given name, or the default-value, if it does not exist.
		const char* getStringVariable (const char* szName, const char* szDefault="") const;

		/// \brief Returns the Value of the nth Parameter, or the default-value, if it does not exist. Start counting at 1, not 0.
		int getIntParameter (unsigned int iParameter) const;
		/// \brief Returns the Value of the nth Parameter, or the default-value, if it does not exist. Start counting at 1, not 0.
		bool getBoolParameter (unsigned int iParameter) const;
		/// \brief Returns the Value of the nth Parameter, or the default-value, if it does not exist. Start counting at 1, not 0.
		float getFloatParameter (unsigned int iParameter) const;
		/// \brief Returns the Value of the nth Parameter, or the default-value, if it does not exist. Start counting at 1, not 0.
		const char* getStringParameter (unsigned int iParameter) const;

		/// \brief Pushes a Parameter on the Stack to be passed to the next Function called
		void PushParameter (int iParam);
		/// \brief Pushes a Parameter on the Stack to be passed to the next Function called
		void PushParameter (bool bParam);
		/// \brief Pushes a Parameter on the Stack to be passed to the next Function called
		void PushParameter (float fParam);
		/// \brief Pushes a Parameter on the Stack to be passed to the next Function called
		void PushParameter (const char* szParam);
		/// \brief Pushes a Parameter on the Stack to be passed to the next Function called
		void PushParameter (const char* szParam, unsigned int length);
		/// \brief Pushes a Parameter on the Stack to be passed to the next Function called
		void PushParameter (const VString& sParam);
		/// \brief Pushes a Parameter on the Stack to be passed to the next Function called
		void PushParameterNil (void);

		/// \brief Pushes a value as a Return value for a called C-Function
		void PushReturnValue (int iParam);
		/// \brief Pushes a value as a Return value for a called C-Function
		void PushReturnValue (bool bParam);
		/// \brief Pushes a value as a Return value for a called C-Function
		void PushReturnValue (float fParam);
		/// \brief Pushes a value as a Return value for a called C-Function
		void PushReturnValue (const char* szParam);
		/// \brief Pushes a value as a Return value for a called C-Function
		void PushReturnValue (const char* szParam, unsigned int length);
		/// \brief Pushes a value as a Return value for a called C-Function
		void PushReturnValue (const VString& sParam);
		/// \brief Pushes a value as a Return value for a called C-Function
		void PushReturnValueNil (void);

		/// \brief Prepares a Function to be called. After that the Parameters can be pushed.
		bool PrepareFunctionCall (const char* szFunctionName, bool bCheckFunctionExistence = true);
		/// \brief Calls the prepared Function with the previously pushed Parameters.
		void CallPreparedFunction (unsigned int iExpectedReturnValues = 0);
		/// \brief Call this after you called a prepared lua-function, that returned some values. If zero values were returned, this function is optional.
		void DiscardReturnValues (void);
		
		/// \brief Checks the nth return-value passed to a C-Function for its type. Start counting at 1, not 0.
		bool isReturnValueInt (unsigned int iReturnValue) const;
		/// \brief Checks the nth return-value passed to a C-Function for its type. Start counting at 1, not 0.
		bool isReturnValueBool (unsigned int iReturnValue) const;
		/// \brief Checks the nth return-value passed to a C-Function for its type. Start counting at 1, not 0.
		bool isReturnValueFloat (unsigned int iReturnValue) const;
		/// \brief Checks the nth return-value passed to a C-Function for its type. Start counting at 1, not 0.
		bool isReturnValueTable (unsigned int iReturnValue) const;
		/// \brief Checks the nth return-value passed to a C-Function for its type. Start counting at 1, not 0.
		bool isReturnValueString (unsigned int iReturnValue) const;

		/// \brief Returns the Value of the nth return-value. Start counting at 1, not 0.
		int getIntReturnValue (unsigned int iReturnValue) const;
		/// \brief Returns the Value of the nth return-value. Start counting at 1, not 0.
		bool getBoolReturnValue (unsigned int iReturnValue) const;
		/// \brief Returns the Value of the nth return-value. Start counting at 1, not 0.
		float getFloatReturnValue (unsigned int iReturnValue) const;
		/// \brief Returns the Value of the nth return-value. Start counting at 1, not 0.
		const char* getStringReturnValue (unsigned int iReturnValue) const;


		/// \brief Return the value of this Function in a called C-Function.
		int ReturnToScript (void) const;

		/// \brief Gets the Light-User-Data set for the currently called Function.
		void* getFunctionLightUserData (void);

		/// \brief Registers a C-Function to the Script under a certain Name. Sets the Lightuserdata for this particular Function.
		void RegisterCFunction (const char* szFunctionName, lua_CFunction pFunction, void* pLightUserData = NULL) const;

		/// \brief Returns an array of all variables in the currently open table
		void EnumVariables (VStrList& sNames);
    /// \brief Returns an array of all variables in the currently open table
    void EnumTables (VStrList& sNames);
		/// \brief Returns an array of all functions in the currently open table
		void EnumFunctions (VStrList& sNames);

	private:
		/// \brief An Allocator for Lua. Optimizes (in theory) the allocations.
		//static void* lua_allocator (void* ud, void* ptr, size_t osize, size_t nsize);

		/// \brief The Lua-State for the Script.
		lua_State* mp_State;

		/// \brief If this script created the Lua-State, it also releases it on exit.
		bool m_bReleaseOnExit;

		struct VScriptStates
		{
			VScriptStates () : m_iParametersPushed (0), m_iOpenTables (0), m_iLuaReturnValues (0) {}

			/// \brief How many Parameters were pushed for the next function-call.
			int m_iParametersPushed;

			/// \brief How many Tables have been opened inside the Lua-Script.
			int m_iOpenTables;

			/// \brief How many values the called lua-function should return
			int m_iLuaReturnValues;
		};

		VScriptStates m_States;

		static const int s_ParamOffset = 0; // should be one, to start counting at 0, instead of 1
	};
}

// has to be included AFTER the declaration
#include <Vision/Tools/Libraries/ShaderLibExport/LuaWrapper/vLuaWrapper_Inline.hpp>

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
