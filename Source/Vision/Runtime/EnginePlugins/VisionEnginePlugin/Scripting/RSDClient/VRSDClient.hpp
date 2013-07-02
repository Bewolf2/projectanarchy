/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRSDClient.hpp

/// This file will be part of a future Vision SDK and is not relevant yet
#if !defined _VISION_DOC

#ifndef VRSDCLIENT_HPP_INCLUDED
#define VRSDCLIENT_HPP_INCLUDED

  class VMessage;
  class VConnection;


  /// \brief
  ///   Describes a callstack entry for transmission with the remote debugger
  class VRSDClientCallstackEntry
  {
    public:

      /// \brief
      ///   The default constructor
      VRSDClientCallstackEntry()
        : pFunctionName(NULL), pFileName(NULL), iLineNumber(-1)
      {}

      /// \brief
      ///   Constructor taking the function name, file name and line number
      ///
      /// \param FunctionName
      ///   The function name of this callstack entry
      ///
      /// \param FileName
      ///   The file name in which this callstack entry occured
      ///
      /// \param LineNumber
      ///   The number of the line of this callstack entry
      ///
      VRSDClientCallstackEntry(const char* FunctionName, const char* FileName, int LineNumber)
        : pFunctionName(FunctionName), pFileName(FileName), iLineNumber(LineNumber), iLineDefined(-1)
      {}

      //! The function name of the callstack entry. 
      const char* pFunctionName;
      
      //! The file name of the callstack entry. 
      const char* pFileName;
      
      //! The line number of the callstack entry. 
      int   iLineNumber;
      
      //! where was the function defined (if not applicable, simply pass -1). 
      int   iLineDefined;
  };

  /// \brief
  ///   Describes a script symbol (variable) for transmission with the remote debugger
  class VRSDScriptSymbol
  {
    public:

      /// \enum SymbolType
      ///
      /// \brief  The possible types of a symbol. Changes to this have to be kept in sync with the remote debugger managed implementation.
      ///
      enum SymbolType
      {
        SYMBOL_CLASS = 0,
        SYMBOL_TABLE,
        SYMBOL_FUNCTION,
        SYMBOL_STRING,
        SYMBOL_NUMBER,
        SYMBOL_USERDATA,
        SYMBOL_BOOLEAN
      };

      /// \fn VRSDScriptSymbol()
      ///
      /// \brief  Default constructor. 
      ///
      VRSDScriptSymbol()
        : m_eSymbolType(SYMBOL_USERDATA), m_szSymbolName(NULL), m_szSymbolContent(NULL), m_bCanBeUpdated(true)
      {}
      /// \fn VRSDScriptSymbol(const char* SymbolName, SymbolType SymbolType, const char* SymbolContent)
      ///
      /// \brief  Constructor taking the symbol name, type and content. 
      ///
      /// \param [in,out] SymbolName  The name of the symbol. 
      /// \param  SymbolType  Type of the symbol. 
      /// \param [in,out] SymbolContent If non-null, the symbol content. 
      ///
      VRSDScriptSymbol(const char* szSymbolName, SymbolType eSymbolType, const char* szSymbolContent)
        : m_eSymbolType(eSymbolType), m_szSymbolName(NULL), m_szSymbolContent(NULL), m_bCanBeUpdated(true)
      {
        SetSymbolName(szSymbolName);
        SetSymbolContent(szSymbolContent);
      }

      ~VRSDScriptSymbol()
      {
        vStrFree(m_szSymbolName);
        vStrFree(m_szSymbolContent);
      }

      inline void SetSymbolName(const char* szSymbolName)
      { 
        vStrFree(m_szSymbolName);
        m_szSymbolName = vStrDup(szSymbolName);
      }
      
      inline void SetSymbolContent(const char* szSymbolContent)
      {
        vStrFree(m_szSymbolContent);
        m_szSymbolContent = vStrDup(szSymbolContent);
      }

      inline void SetCanBeUpdatedByDebugger(bool bCanBeUpdated)
      {
        m_bCanBeUpdated = bCanBeUpdated;
      }

      inline const char* GetSymbolName() const { return m_szSymbolName; }
      inline const char* GetSymbolContent() const { return m_szSymbolContent; }
      inline bool IsUpdateableByDebugger() const { return m_bCanBeUpdated; }
      
    //! Stores the symbol type. 
    SymbolType m_eSymbolType;

  protected:
    //! Stores the symbol name. 
    char* m_szSymbolName;

    //! Stores the symbol content (a string representation). 
    char* m_szSymbolContent;

    //! Stores if the variable may be updated by the debugger
    bool m_bCanBeUpdated;
  };

  /// \brief
  ///   Describes a script event (line event, script error event, ..) for transmission with the remote debugger
  class VRSDScriptEvent
  {
    public:

      /// \enum ExecutionType
      ///
      /// \brief  Type of execution event. This defines in which context the event happened.
      ///
      enum ExecutionType
      {
        EXECUTION_TYPE_SCRIPT = 0,
        EXECUTION_TYPE_NATIVE
      };

      /// \enum EventType
      ///
      /// \brief  Type of script event. This defines where the event happened in the script.
      ///
      enum EventType
      {
        EVENT_UNKNOWN = -1,
        EVENT_LINE = 0,
        EVENT_ENTER_FUNCTION,
        EVENT_LEAVE_FUNCTION,
        EVENT_SCRIPT_ERROR
      };

      /// \fn VRSDScriptEvent(EventType EventType, int LineNumber, ExecutionType ExecutionType,
      ///     const char* FileName, const char* FunctionName, int LineDefined)
      ///
      /// \brief  Constructor.
      ///
      /// \param  EventType Type of the event. 
      /// \param  LineNumber  The line number. 
      /// \param  ExecutionType Type of the execution. 
      /// \param [in,out] FileName  If non-null, filename of the file. 
      /// \param [in,out] FunctionName  If non-null, name of the function. 
      /// \param  LineDefined The line defined. 
      ///
      VRSDScriptEvent(EventType EventType, int LineNumber, ExecutionType ExecutionType, const char* FileName, const char* FunctionName, int LineDefined)
        : iLineNumber(LineNumber), eExecutionType(ExecutionType), eEventType(EventType), pFileName(FileName), pFunctionName(FunctionName), pErrorMessage(NULL), iLineDefined(LineDefined)
      {}

      /// \fn VRSDScriptEvent(const char* ErrorMessage, int LineNumber, ExecutionType ExecutionType,
      ///     const char* FileName, const char* FunctionName, int LineDefined)
      ///
      /// \brief  Constructor. 
      ///
      /// \param [in,out] ErrorMessage  If non-null, message describing the error. 
      /// \param  LineNumber  The line number. 
      /// \param  ExecutionType Type of the execution. 
      /// \param [in,out] FileName  If non-null, filename of the file. 
      /// \param [in,out] FunctionName  If non-null, name of the function. 
      /// \param  LineDefined The line defined. 
      ///
      VRSDScriptEvent(const char* ErrorMessage, int LineNumber, ExecutionType ExecutionType, const char* FileName, const char* FunctionName, int LineDefined)
        : iLineNumber(LineNumber), eExecutionType(ExecutionType), eEventType(EVENT_SCRIPT_ERROR), pFileName(FileName), pFunctionName(FunctionName), pErrorMessage(ErrorMessage), iLineDefined(LineDefined)
      {}


      //! Stores the line number of the script event. 
      int iLineNumber;

      //! Stores the execution type of the event. 
      ExecutionType eExecutionType;

      //! Stores the event type. 
      EventType eEventType;
      
      //! Stores the file name of the event. 
      const char* pFileName;

      //! Stores the function name where the event occured. 
      const char* pFunctionName;

      //! Stores the error message if there is one. 
      const char* pErrorMessage;
      
      //! Stores the line where the current function was defined. 
      int iLineDefined;
  };

  /// \brief
  ///   A callback item used in the script event callback system
  class VRSDScriptEventCallbackItem : public IVisCallbackDataObject_cl
  {
    public:

      /// \fn VRSDScriptEventCallbackItem(VisCallback_cl *Sender, VRSDScriptEvent* ScriptEvent)
      ///
      /// \brief  Constructor for the callback item taking the sender and the script event.
      ///
      /// \param [in,out] Sender  If non-null, the sender. 
      /// \param [in,out] ScriptEvent If non-null, the script event. 
      ///
      VRSDScriptEventCallbackItem(VisCallback_cl *Sender, VRSDScriptEvent* ScriptEvent)
        : IVisCallbackDataObject_cl(Sender), pScriptEvent(ScriptEvent)
      {}

      //! Stores the script event. 
      VRSDScriptEvent* pScriptEvent;

  };
  
  /// \brief
  ///   The interface for a language implementation for the remote debugger
  //    If another script language is implemented this can be used to make it remote debuggable
  class IVRSDClientLanguageImplementation : public IVisCallbackHandler_cl
  {
    public:

      /// \fn IVRSDClientLanguageImplementation()
      ///
      /// \brief  Default constructor. 
      ///
      IVRSDClientLanguageImplementation()
      {
      }

      /// \fn virtual ~IVRSDClientLanguageImplementation()
      ///
      /// \brief  Destructor. 
      ///
      virtual ~IVRSDClientLanguageImplementation()
      {
      }

      /// \fn virtual bool GetCallstack(DynArray_cl<VRSDClientCallstackEntry>& Callstack,
      ///     unsigned int& CallstackEntryCount)
      ///
      /// \brief  Gets a callstack from the language implementation. 
      ///
      /// \param [in,out] Callstack the callstack. 
      /// \param [in,out] CallstackEntryCount number of callstack entries. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool GetCallstack(DynArray_cl<VRSDClientCallstackEntry>& Callstack, unsigned int& CallstackEntryCount)
      {
        return false;
      }

      /// \fn virtual bool GetGlobalSymbols(DynArray_cl<VRSDScriptSymbol>& GlobalSymbols,
      ///     unsigned int& GlobalSymbolCount)
      ///
      /// \brief  Gets the current global symbols from the language implementation. 
      ///
      /// \param [in,out] globalSymbols the global symbols. 
      /// \param [in,out] iGlobalSymbolCount number of global symbols. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool GetGlobalSymbols(DynArray_cl<VRSDScriptSymbol>& globalSymbols, unsigned int& iGlobalSymbolCount)
      {
        return false;
      }

      /// \fn virtual bool GetLocalSymbols(DynArray_cl<VRSDScriptSymbol>& LocalSymbols,
      ///     unsigned int& LocalSymbolCount)
      ///
      /// \brief  Gets the current local symbols. 
      ///
      /// \param [in,out] localSymbols  the local symbols. 
      /// \param [in,out] iLocalSymbolCount  number of local symbols. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool GetLocalSymbols(DynArray_cl<VRSDScriptSymbol>& localSymbols, unsigned int& iLocalSymbolCount)
      {
        return false;
      }

      /// \fn virtual bool GetSubSymbolsForGlobal(char* GlobalName,
      ///     DynArray_cl<VRSDScriptSymbol>& SubSymbols, unsigned int& SubSymbolCount)
      ///
      /// \brief  Gets a collection of sub symbols for global symbol. 
      ///
      /// \param [in,out] szGlobalName  Name of the global. 
      /// \param [in,out] subSymbols  the sub symbols collection. 
      /// \param [in,out] iSubSymbolCount  number of sub symbols. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool GetSubSymbolsForGlobal(char* szGlobalName, DynArray_cl<VRSDScriptSymbol>& subSymbols, unsigned int& iSubSymbolCount)
      {
        return false;
      }

      /// \fn virtual bool GetSubSymbolsForLocal(char* LocalName,
      ///     DynArray_cl<VRSDScriptSymbol>& SubSymbols, unsigned int& SubSymbolCount)
      ///
      /// \brief  Gets a collection of sub symbols for local symbol. 
      ///
      /// \param [in,out] szLocalName Name of the local. 
      /// \param [in,out] subSymbols  the sub symbols. 
      /// \param [in,out] iSubSymbolCount  number of sub symbols. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool GetSubSymbolsForLocal(char* szLocalName, DynArray_cl<VRSDScriptSymbol>& subSymbols, unsigned int& iSubSymbolCount)
      {
        return false;
      }

      /// \fn virtual bool IsImplementation(const char* Name)
      ///
      /// \brief  Query if 'Name' is the name of the current implementation. 
      ///
      /// \param [in,out] Name  The name to query for. 
      ///
      /// \return true if the current implementation is of the type of 'Name', false if not. 
      ///
      virtual bool IsImplementation(const char* Name)
      {
        return false;
      }

      /// \fn virtual bool IsLocalUserDataOfType(const char* Name, const char* Type)
      ///
      /// \brief  Query if the local variable 'Name' is a user data object of type 'Type'
      ///
      /// \param [in] Name  If non-null, the name. 
      /// \param [in] Type  If non-null, the type. 
      ///
      /// \return true if the local is of type 'Type', false if not. 
      ///
      virtual bool IsLocalUserDataOfType(const char* Name, const char* Type)
      {
        return false;
      }

      /// \fn virtual bool IsGlobalUserDataOfType(const char* Name, const char* Type)
      ///
      /// \brief  Query if the global variable 'Name' is a user data object of type 'Type'
      ///
      /// \param [in] Name  If non-null, the name. 
      /// \param [in] Type  If non-null, the type. 
      ///
      /// \return true if the global is of type 'Type', false if not. 
      ///
      virtual bool IsGlobalUserDataOfType(const char* Name, const char* Type)
      {
        return false;
      }

      virtual bool GetGlobalType(const char*pVariableName, char * pUserDataTypeName)
      {
        return false;
      }

      virtual bool GetLocalType(const char* pVariableName, char * pUserDataTypeName)
      {
        return false;
      }

      /// \fn virtual bool GetUserDataPointerFromLocal(const char* Variable, void** UserDataPointer)
      ///
      /// \brief  Gets a user data pointer from a local variable. 
      ///
      /// \param [in] szVariable  The variable. 
      /// \param [in,out] ppUserData A pointer to the pointer where the userdata pointer should be stored
      /// \param [in,out] ppEnvironment A pointer to the context
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool GetUserDataPointerFromLocal(const char* szVariable, void** ppUserData, void ** ppEnvironemnt)
      {
        return false;
      }

      /// \fn virtual bool GetUserDataPointerFromGlobal(const char* Variable, void** UserDataPointer)
      ///
      /// \brief  Gets a user data pointer from a global variable. 
      ///
      /// \param [in,out] szVariable  If non-null, the variable. 
      /// \param [in,out] ppUserData A pointer to the pointer where the userdata pointer should be stored
      /// \param [in,out] ppEnvironment A pointer to the context
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool GetUserDataPointerFromGlobal(const char* szVariable, void** ppUserData, void ** ppEnvironemnt)
      {
        return false;
      }

      /// \fn virtual bool UpdateLocalVariable(const char* Variable, const char* NewValue)
      ///
      /// \brief  Updates a local variable. 
      ///
      /// \param [in,out] Variable  If non-null, the variable. 
      /// \param [in,out] NewValue  If non-null, the new value. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool UpdateLocalVariable(const char* Variable, const char* NewValue)
      {
        return false;
      }

      /// \fn virtual bool UpdateGlobalVariable(const char* Variable, const char* NewValue)
      ///
      /// \brief  Updates a global variable. 
      ///
      /// \param [in,out] Variable  If non-null, the variable. 
      /// \param [in,out] NewValue  If non-null, the new value. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool UpdateGlobalVariable(const char* Variable, const char* NewValue)
      {
        return false;
      }

      /// \fn 
      ///   virtual bool UpdateHiddenGlobalVariable(const void* pUserDataParent, const char* Variable, 
      ///   const char* NewValue)
      ///
      /// \brief
      ///   Updates a hidden global variable (= member added to user data variable at runtime)
      ///
      /// \param [in] pUserDataParent
      ///   The user data pointer to the parent containing the variable as a member.
      /// \param [in] Variable
      ///   If non-null, the variable.
      /// \param [in] NewValue
      ///   If non-null, the new value.
      virtual bool UpdateHiddenGlobalVariable(const void* pUserDataParent, const char* Variable, 
        const char* NewValue)
      {
        return false;
      }

      /// \fn virtual bool StartProfiling()
      ///
      /// \brief  Starts the profiling process. 
      ///
      /// \return true if it succeeds, false if the profiling can't be started. 
      ///
      virtual bool StartProfiling()
      {
        return true;
      }

      /// \fn virtual bool StopProfiling()
      ///
      /// \brief  Stops the profiling process. 
      ///
      /// \return true if it succeeds, false if the profiling can't be stopped. 
      ///
      virtual bool StopProfiling()
      {
        return true;
      }

      //! Stores the script event. 
      VisCallback_cl ScriptEvent;

    protected:

      /// \fn SCRIPT_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData)
      ///
      /// \brief  Callback function which will be called by the remote script debugger client. This callback can be used to switch on/off things when there is actually a debugger connection or not.
      ///
      /// \param [in,out] pData If non-null, the data. 
      ///
      SCRIPT_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData)
      {
      }
  };

  /// \brief
  ///   Interface for user data accessors (get members of a userdata type and update a member from a string
  /// \sa Vector3DataAccessor
  class IVRSDUserDataAccessor
  {
    public:

      /// \fn IVRSDUserDataAccessor(const char* pUserDataType)
      ///
      /// \brief  Constructor taking the name of the user data type the accessor is for. 
      ///
      /// \param [in,out] pUserDataType If non-null, type of the user data. 
      ///
      IVRSDUserDataAccessor(const char* pUserDataType)
		    : m_sUserDataType(pUserDataType)
      {
      }

      /// \fn virtual ~IVRSDUserDataAccessor()
      ///
      /// \brief  Destructor. 
      ///
      virtual ~IVRSDUserDataAccessor()
      {
      }

      /// \fn virtual bool GetUserDataMembers(void* pUserDataPointer,
      ///     DynArray_cl<VRSDScriptSymbol>& Members, unsigned int& MemberCount)
      ///
      /// \brief  Gets the user data members for a user data pointer. 
      ///
      /// \param [in] pUserDataPointer  The user data pointer. 
      /// \param [in,out] pEnvironment  Pointer to the environment (e.g lua_State). 
      /// \param [in,out] members The collection where the members should be placed in.
      /// \param [in,out] iMemberCount The number of members.
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool GetUserDataMembers(void* pUserDataPointer, void *pEnvironment, 
        DynArray_cl<VRSDScriptSymbol>& members, unsigned int& iMemberCount) = 0;

      /// \fn virtual bool UpdateMemberFromString(void* pUserDataPointer, char* pMemberName,
      ///     char* pString)
      ///
      /// \brief  Updates a member from a string value. 
      ///
      /// \param [in,out] pUserDataPointer  The user data pointer. 
      /// \param [in,out] pEnvironment  Pointer to the environment (e.g lua_State). 
      /// \param [in,out] szMemberName The name of the member which should be updated.
      /// \param [in,out] szString The string content of the member.
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      virtual bool UpdateMemberFromString(void* pUserDataPointer, void *pEnvironment, 
        const char* szMemberName, const char* szString) = 0;

      /// \fn inline bool IsForUserDataType(const char* pUserDataType)
      ///
      /// \brief  Query if this accessor is for user data type 'pUserDataType'. 
      ///
      /// \param [in,out] pUserDataType The name of the userdata to check against. 
      ///
      /// \return true if this accessor is for user data objects of type 'pUserDataType', false if not. 
      ///
      inline bool IsForUserDataType(const char* pUserDataType)
      {
        return (pUserDataType && (m_sUserDataType == pUserDataType));
      }

      /// \fn inline const char* GetUserDataType()
      ///
      /// \brief  Gets the user data type. 
      ///
      /// \return The user data type this accessor works for.
      ///
	    inline const char* GetUserDataType()
	    {
		    return m_sUserDataType.AsChar();
	    }

    protected:

      /// \fn void Helper_AddMember(DynArray_cl<VRSDScriptSymbol>& Members, unsigned int& MemberCount,
      ///     char* pMemberName, char* pMemberContent, VRSDScriptSymbol::SymbolType MemberType)
      ///
      /// \brief  Helper function to add a member to the member collection.
      ///
      /// \param [in,out] Members The member collection to work on.
      /// \param [in,out] MemberCount Number of members. 
      /// \param [in] pMemberName The name of the member. 
      /// \param [in] pMemberContent  The content of the member variable.
      /// \param  MemberType  Type of the member. 
      ///
      void Helper_AddMember(DynArray_cl<VRSDScriptSymbol>& dynArrMembers, unsigned int& iMemberCount, 
        const char* szMemberName, const char* szMemberContent, VRSDScriptSymbol::SymbolType eMemberType, 
        bool bCanBeUpdatedByDebugger = true)
      {
        dynArrMembers[iMemberCount].m_eSymbolType = eMemberType;
        dynArrMembers[iMemberCount].SetSymbolName(szMemberName);
        dynArrMembers[iMemberCount].SetSymbolContent(szMemberContent);
        dynArrMembers[iMemberCount].SetCanBeUpdatedByDebugger(bCanBeUpdatedByDebugger);

        iMemberCount++;
      }

      //! Stores the user data type name this accessor works for. 
      VString m_sUserDataType;

  };

  /// \class  VRSDProfilingSample
  ///
  /// \brief  Helper class to store a profiling sample. 
  ///
  class VRSDProfilingSample
  {
    public:

      /// \fn VRSDProfilingSample(const char* pFunctionName, const char* pFileName, int iLineDefined)
      ///
      /// \brief  Default Constructor. 
      ///
      /// \param [in] pFunctionName Name of the function. 
      /// \param [in] pFileName Filename of the file. 
      /// \param  iLineDefined  The line the function was defined in. 
      ///
      VRSDProfilingSample(const char* pFunctionName, const char* pFileName, int iLineDefined)
        : m_iLineDefined(iLineDefined), m_uiStartTime(VGLGetTimer()), m_FunctionName(pFunctionName), m_FileName(pFileName)
      {
      }

      int m_iLineDefined;
      uint64 m_uiStartTime;

      VMemoryTempBuffer<256> m_FunctionName;
      VMemoryTempBuffer<256> m_FileName;
  };

  /// \class  VRSDProfilingInformation
  ///
  /// \brief  Helper class storing information for one function in one file. 
  ///
  class VRSDProfilingInformation
  {
    public:

      /// \fn VRSDProfilingInformation(const char* pFunctionName, const char* pFileName, int iLineDefined,
      ///     uint64 uiTimeTaken)
      ///
      /// \brief  Default Constructor. 
      ///
      /// \param [in] pFunctionName Name of the function. 
      /// \param [in] pFileName Filename of the file. 
      /// \param  iLineDefined  The line defined. 
      /// \param  uiTimeTaken The time taken by the first sample. 
      ///
      VRSDProfilingInformation(const char* pFunctionName, const char* pFileName, int iLineDefined, uint64 uiTimeTaken)
        : m_iCalls(1), m_iLineDefined(iLineDefined), m_uiTotalTime(uiTimeTaken), m_uiMinTime(uiTimeTaken), m_uiMaxTime(uiTimeTaken), m_pFunctionName(NULL), m_pFileName(NULL)
      {
        VASSERT(pFunctionName);
        m_pFunctionName = vStrDup(pFunctionName);
        VASSERT(pFileName);
        m_pFileName = vStrDup(pFileName);
      }

      ~VRSDProfilingInformation()
      {
        vStrFree(m_pFunctionName);
        vStrFree(m_pFileName);
      }

      void AddSample(uint64 uiTimeTaken)
      {
        if(uiTimeTaken < m_uiMinTime)
          m_uiMinTime = uiTimeTaken;
        else if(uiTimeTaken > m_uiMaxTime)
          m_uiMaxTime = uiTimeTaken;

        m_uiTotalTime += uiTimeTaken;

        m_iCalls++;
      }

      uint64 GetAverageTime()
      {
        return m_uiTotalTime / m_iCalls;
      }

      bool Matches(const char* pFunctionName, const char* pFileName, int iLineDefined)
      {
        if(iLineDefined != m_iLineDefined)
          return false;

        if(pFunctionName && m_pFunctionName)
        {
          if(strcmp(pFunctionName, m_pFunctionName))
            return false;
        }

        if(pFileName && m_pFileName)
        {
          if(strcmp(pFileName, m_pFileName))
            return false;
        }

        return true;
      }

      int m_iCalls;
      int m_iLineDefined;
      uint64 m_uiTotalTime;
      uint64 m_uiMinTime;
      uint64 m_uiMaxTime;

      char* m_pFunctionName;
      char* m_pFileName;

  };

  class VRSDMessage;

  /// \brief
  ///   The client of the remote debugger
  class VRSDClient : public IVisCallbackHandler_cl
  {
    public:

      /// \fn SCRIPT_IMPEXP VRSDClient()
      ///
      /// \brief  Constructor for the remote script debugger client.
      ///
      SCRIPT_IMPEXP VRSDClient();

      /// \fn SCRIPT_IMPEXP ~VRSDClient()
      ///
      /// \brief  Destructor. 
      ///
      SCRIPT_IMPEXP ~VRSDClient();

      /// \fn SCRIPT_IMPEXP bool IsConnected()
      ///
      /// \brief  Query if the rsd client is connected to a debugger. 
      ///
      /// \return true if connected, false if not. 
      ///
      SCRIPT_IMPEXP bool IsConnected();

      /// \fn SCRIPT_IMPEXP static VRSDClient& GetGlobalClient()
      ///
      /// \brief  Gets the global client instance. 
      ///
      /// \return The global client instance. 
      ///
      SCRIPT_IMPEXP static VRSDClient& GetGlobalClient();

      //! Callback when a connection to a remote debugger happened. 
      VisCallback_cl OnConnected;
      
      //! Callback when a disconnection occured. 
      VisCallback_cl OnDisconnected;
      
      //! Callback when a messsage was received. 
      VisCallback_cl OnMessageReceived;

      //! Callback when a connection error occured. 
      VisCallback_cl OnConnectionError;

      /// \fn void SetClientLanguageImplementation(IVRSDClientLanguageImplementation* pClientLanguageImplementation)
      ///
      /// \brief  Sets the client language implementation for the client. 
      ///
      /// \param [in,out] pClientLanguageImplementation The client language implementation. 
      ///
      void SetClientLanguageImplementation(IVRSDClientLanguageImplementation* pClientLanguageImplementation);

      /// \fn IVRSDClientLanguageImplementation* GetClientLanguageImplementation()
      ///
      /// \brief  Gets the client language implementation. 
      ///
      /// \return The client language implementation. (This may be NULL if none has been set) 
      ///
      IVRSDClientLanguageImplementation* GetClientLanguageImplementation();

      /// \fn SCRIPT_IMPEXP static BOOL EngineMessageCallback(const char* szMessage,
      ///     VisApiMessageType_e messageType, void* pUserData)
      ///
      /// \brief  Callback, called by the engine when the engines wants to print a message. These messages will be send over the remote connection and displayed in the remote debugger.
      ///
      /// \param [in,out] szMessage The engine message.
      /// \param  messageType Type of the message. 
      /// \param [in,out] pUserData Userdata pointer supplied back by the engine. In this case it is a pointer to the client.
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      SCRIPT_IMPEXP static BOOL EngineMessageCallback(const char* szMessage, VisApiMessageType_e messageType, void* pUserData);

      /// \fn SCRIPT_IMPEXP void RegisterUserDataAccessor(IVRSDUserDataAccessor* pUserDataAccessor)
      ///
      /// \brief  Register a user data accessor. This function can be used to register a user data accessor for a user data type.
      ///
      /// \param [in] pUserDataAccessor The user data accessor. 
      ///
      SCRIPT_IMPEXP void RegisterUserDataAccessor(IVRSDUserDataAccessor* pUserDataAccessor);

      /// \fn SCRIPT_IMPEXP IVRSDUserDataAccessor* GetUserDataAccessor(const char* pUserDataType)
      ///
      /// \brief  Gets a user data accessor for a specific user data type. 
      ///
      /// \param [in] pUserDataType The name of the user data type.
      ///
      /// \return null if no user data accessor is found for the name, else the correct user data accessor. 
      ///
      SCRIPT_IMPEXP IVRSDUserDataAccessor* GetUserDataAccessor(const char* pUserDataType);

      /// \fn SCRIPT_IMPEXP void RegisterCallbacks()
      ///
      /// \brief  Register callbacks with the engine. 
      ///
      SCRIPT_IMPEXP void RegisterCallbacks();

      /// \fn SCRIPT_IMPEXP void UnregisterCallbacks()
      ///
      /// \brief  Unregister callbacks with the engine. 
      ///
      SCRIPT_IMPEXP void UnregisterCallbacks();

      /// \fn SCRIPT_IMPEXP bool StartProfiling()
      ///
      /// \brief  Starts the profiling. (Internally used)
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      SCRIPT_IMPEXP bool StartProfiling();

      /// \fn SCRIPT_IMPEXP bool StopProfiling()
      ///
      /// \brief  Stops the profiling. (Internally used)
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      SCRIPT_IMPEXP bool StopProfiling(unsigned int* puiProfilingResultCount = NULL);

    private:

      /// \fn SCRIPT_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData)
      ///
      /// \brief  Handles callbacks by the script implementation to send script events to the remote debugger.
      ///
      /// \param [in,out] pData If non-null, the data. 
      ///
      SCRIPT_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

      /// \fn SCRIPT_IMPEXP void HandleDebuggingMessage(VMessage* pMessage)
      ///
      /// \brief Internal function to handle and parse the incoming messages by the remote debugger.
      ///
      /// \param [in,out] pMessage  If non-null, the message. 
      ///
      SCRIPT_IMPEXP void HandleDebuggingMessage(VMessage* pMessage);

      // specific cases of HandleDebuggingMessage
      void UpdateVariable(VMessage* pMessage);
      void UpdateUserDataVariable(VMessage* pMessage);
      void HandleUserDataMemberRequest(VMessage* pMessage);

      /// \fn SCRIPT_IMPEXP void HandleScriptReloadMessage(VMessage* pMessage)
      ///
      /// \brief  Handle script reload message. 
      ///
      /// \param [in] pMessage  The message to handle. 
      ///
      SCRIPT_IMPEXP void HandleScriptReloadMessage(VMessage* pMessage);

      /// \fn SCRIPT_IMPEXP void HandleScriptEventForProfiling(VRSDScriptEvent* pScriptEvent)
      ///
      /// \brief  Handle script event for profiling purposes. 
      ///
      /// \param [in] pScriptEvent  The script event which is necessary for the profiling. 
      ///
      SCRIPT_IMPEXP void HandleScriptEventForProfiling(VRSDScriptEvent* pScriptEvent);

      // helper function to send special debugging messages

      /// \fn SCRIPT_IMPEXP bool SendCallstack(DynArray_cl<VRSDClientCallstackEntry>& Callstack,
      ///     unsigned int& CallstackEntryCount)
      ///
      /// \brief  Send callstack to the remote debugger. 
      ///
      /// \param [in,out] Callstack the callstack. 
      /// \param [in,out] CallstackEntryCount number of callstack entries. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      SCRIPT_IMPEXP bool SendCallstack(DynArray_cl<VRSDClientCallstackEntry>& Callstack, unsigned int& CallstackEntryCount);

      /// \fn SCRIPT_IMPEXP bool SendSymbols(int Type, DynArray_cl<VRSDScriptSymbol>& Symbols,
      ///     unsigned int& SymbolCount, char* pParentSymbol = NULL)
      ///
      /// \brief  Sends a collection symbols. 
      ///
      /// \param  Type  The type of the symbol collection. 
      /// \param [in,out] Symbols the symbols. 
      /// \param [in,out] SymbolCount number of symbols. 
      /// \param [in,out] pParentSymbol If non-null, the parent symbol. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      SCRIPT_IMPEXP bool SendSymbols(int Type, DynArray_cl<VRSDScriptSymbol>& Symbols, unsigned int& SymbolCount, char* pParentSymbol = NULL);

      /// \fn SCRIPT_IMPEXP bool SendScriptEvent(VRSDScriptEvent* pScriptEvent)
      ///
      /// \brief  Sends script event to the remote debugger. 
      ///
      /// \param [in,out] pScriptEvent  If non-null, the script event. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      SCRIPT_IMPEXP bool SendScriptEvent(VRSDScriptEvent* pScriptEvent);

      /// \fn SCRIPT_IMPEXP void UpdateProfilingInformation(const char* pFileName,
      ///     const char* pFunctionName, int iLineDefined, uint64 uiTimeTaken)
      ///
      /// \brief  Updates a profiling information entry with a new time sample. 
      ///
      /// \param [in] pFileName If non-null, filename of the file. 
      /// \param [in] pFunctionName If non-null, name of the function. 
      /// \param  iLineDefined  The line defined. 
      /// \param  uiTimeTaken The user interface time taken. 
      ///
      SCRIPT_IMPEXP void UpdateProfilingInformation(const char* pFileName, const char* pFunctionName, int iLineDefined, uint64 uiTimeTaken);

      /// \fn SCRIPT_IMPEXP bool SendProfilingResults()
      ///
      /// \brief  Sends the profiling results to the remote debugger. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      SCRIPT_IMPEXP bool SendProfilingResults();

      //! Stores the connection the remote debugger is on. 
      VConnection* m_pConnection;

      //! Stores the current client language implementation. 
      IVRSDClientLanguageImplementation* m_pClientLanguageImplementation;

      //! Global instance of the client. 
      static VRSDClient g_GlobalClient;

      //! The list of user data accessors. 
      VPListT<IVRSDUserDataAccessor> m_UserDataAccessors;

      //! Stores if profiling is enabled or not. 
      bool m_bProfilingEnabled;

      VPListStack<VRSDProfilingSample*>* m_pProfilingStack;

      DynArray_cl<VRSDProfilingInformation*> m_pProfilingInformations;

      //! Stores the connection guarding mutex;
      VMutex m_ConnectionMutex;
  };


#endif

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
