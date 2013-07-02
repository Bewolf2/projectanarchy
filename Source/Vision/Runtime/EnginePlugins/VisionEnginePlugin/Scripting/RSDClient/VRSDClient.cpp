/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// This file will be part of a future Vision SDK and is not relevant yet

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/Base/RemoteComm/VMessage.hpp>
#include <Vision/Runtime/Base/RemoteComm/VConnection.hpp>
#include <Vision/Runtime/Base/RemoteComm/VTarget.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/RSDClient/VRSDClient.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VRSDClient VRSDClient::g_GlobalClient;


VRSDClient::VRSDClient()
  : m_UserDataAccessors(32)
{
  m_pClientLanguageImplementation = NULL;
  m_pConnection = NULL;
  m_bProfilingEnabled = false;
  m_pProfilingStack = NULL;
}

VRSDClient::~VRSDClient()
{
  SetClientLanguageImplementation(NULL);
  m_pConnection = NULL;

  // clear user data accessors
  for(int i = 0; i < m_UserDataAccessors.GetLength(); i++)
  {
    V_SAFE_DELETE(m_UserDataAccessors[i]);
  }

  V_SAFE_DELETE(m_pProfilingStack);

  unsigned int uiArraySize = m_pProfilingInformations.GetValidSize();
  for(unsigned int i = 0; i < uiArraySize; i++)
    V_SAFE_DELETE(m_pProfilingInformations[i]);
}

void VRSDClient::RegisterCallbacks()
{
  VTarget::OnConnection += this;
  VTarget::OnDisconnection += this;

  // make sure it is initialized even in release builds if the current executable is the editor
  // The editor starts the target system on port 4224 instead of port 4223 so the debugging of game scripts works parallel
  // to vForge
  if(Vision::Editor.IsInEditor())
    VTargetThread::Init(VTARGET_EDITOR_PORT);
}

void VRSDClient::UnregisterCallbacks()
{
  // If no scene is loaded in the editor VTargetThread::Deinit will never be called.
  if(Vision::Editor.IsInEditor())
    VTargetThread::DeInit(); 

  VTarget::OnConnection -= this;
  VTarget::OnDisconnection -= this;

  
}


void VRSDClient::RegisterUserDataAccessor(IVRSDUserDataAccessor* pUserDataAccessor)
{
  VASSERT(pUserDataAccessor);

  if(!pUserDataAccessor)
    return;

  m_UserDataAccessors.AddUnique(pUserDataAccessor); 
}

IVRSDUserDataAccessor* VRSDClient::GetUserDataAccessor(const char* pUserDataType)
{
  for(int i = 0; i < m_UserDataAccessors.GetLength(); i++)
  {
    IVRSDUserDataAccessor* pAccessor = m_UserDataAccessors[i];

    if(pAccessor->IsForUserDataType(pUserDataType))
      return pAccessor;
  }

  return NULL;
}

bool VRSDClient::StartProfiling()
{
  if(m_bProfilingEnabled)
  {
    StopProfiling();
    return false;
  }

  Vision::Callbacks.OnEditorModeChanged += this;

  // tell the client language implementation that we are now profiling (just in case it has something special to do)
  if(GetClientLanguageImplementation()->StartProfiling())
  {
    V_SAFE_DELETE(m_pProfilingStack);
    m_pProfilingStack = new VPListStack<VRSDProfilingSample*>();
    m_bProfilingEnabled = true;

    Vision::Error.SystemMessage("Starting script profiling..");

    return true;
  }

  /// \todo Editor Play Mode starten
//  if(!Vision::Editor.IsPlaying())
//    Vision::Editor.SetMode(VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR);

  return false;
}

bool VRSDClient::StopProfiling(unsigned int* puiProfilingResultCount /*= NULL*/)
{
  if(!m_bProfilingEnabled)
    return false;

  Vision::Callbacks.OnEditorModeChanged -= this;

  // tell the client language implementation that we are stopping to profile (just in case it has something special to do)
  if(GetClientLanguageImplementation()->StopProfiling())
  {
    m_bProfilingEnabled = false;

    if(puiProfilingResultCount)
      *puiProfilingResultCount = m_pProfilingInformations.GetValidSize();

    Vision::Error.SystemMessage("Stopped script profiling.");
    if(!SendProfilingResults())
      Vision::Error.Warning("Couldn't transmit profiling results!");
    else
      Vision::Error.SystemMessage("Sent profiling results.");

    V_SAFE_DELETE(m_pProfilingStack);

    unsigned int uiArraySize = m_pProfilingInformations.GetValidSize();
    for(unsigned int i = 0; i < uiArraySize; i++)
      V_SAFE_DELETE(m_pProfilingInformations[i]);

    m_pProfilingInformations.Reset();

    return true;
  }

  return false;
}


void VRSDClient::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  // check event source
  if(pData->m_pSender == &m_pClientLanguageImplementation->ScriptEvent)
  {
    VRSDScriptEvent* pEvent = static_cast<VRSDScriptEventCallbackItem*>(pData)->pScriptEvent;

    if(!pEvent)
      return;

    // If profiling is enabled the profiler will handle the script event,
    // Afterwards the return ensures that the standard debugging procedure is not followed
    if(m_bProfilingEnabled)
    {
      HandleScriptEventForProfiling(pEvent);
      return;
    }

    // Check if there is a connection from the remote debugger available
    if(!m_pConnection)
      return;

    // send event to debugger
    if(!SendScriptEvent(pEvent))
      return;

    // wait for debugger on what to do
    VMessage Msg;

    {
      VMutexLocker lock(m_ConnectionMutex);
      if(!m_pConnection)
        return;
      m_pConnection->Recv(&Msg);
    }

    while(Msg.IsValid() && Msg.GetMessageType() != 'CONT')
    {
      // check here for profiling messages as well since the async send may lead to them being received here
      if(Msg.GetMessageType() == 'BRSP')
      {
        StartProfiling();
        break;
      }
      else if(Msg.GetMessageType() == 'ERSP')
      {
        StopProfiling();
        break;
      }

      // process message (get callstack, get local variables, get global variables etc.)
      HandleDebuggingMessage(&Msg);

      // get the next message
      VMutexLocker lock(m_ConnectionMutex);
      if(!m_pConnection || !m_pConnection->Recv(&Msg))
        break;
    }
  }

  else if(pData->m_pSender == &VTarget::OnConnection)
  {
    VTargetConnectionCallbackItem* pTCCI = (VTargetConnectionCallbackItem*)pData;
    if(pTCCI->pConnection)
    {
      if(pTCCI->pConnection->IsFor("VRSD"))
      {
        m_pConnection = pTCCI->pConnection;

        // register message callback
        Vision::Error.AddCallback(EngineMessageCallback, this);

        Vision::Callbacks.OnUpdateSceneBegin += this;

        // Send connected callback
        OnConnected.TriggerCallbacks();
      }
    }
  }

  else if(pData->m_pSender == &VTarget::OnDisconnection)
  {
    VTargetConnectionCallbackItem* pTCCI = (VTargetConnectionCallbackItem*)pData;
    if(pTCCI->pConnection)
    {
      if(pTCCI->pConnection->IsFor("VRSD"))
      {
        {
          VMutexLocker lock(m_ConnectionMutex);
          m_pConnection = NULL;
        }

        // register message callback
        Vision::Error.RemoveCallback(EngineMessageCallback, this);

        Vision::Callbacks.OnUpdateSceneBegin -= this;

        // Send disconnected callback
        OnDisconnected.TriggerCallbacks();
      }
    }
  }

  else if(pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin)
  {
    VMessage* pMsg = NULL;

    {
      VMutexLocker lock(m_ConnectionMutex);

      if(!m_pConnection)
        return;

      pMsg = m_pConnection->GetNextASyncReceivedMessage();
    }

    if(!pMsg)
      return;

    // command messages
    switch(pMsg->GetMessageType())
    {
      // begin remote script profiling
      case 'BRSP':
        StartProfiling();
        break;

      // end remote script profiling
      case 'ERSP':
        StopProfiling();
        break;

      // script reload
      case 'RSRN':
        HandleScriptReloadMessage(pMsg);
        break;
    }

    V_SAFE_DELETE(pMsg);
  }

  else if(pData->m_pSender == &Vision::Callbacks.OnEditorModeChanged)
  {
    if(Vision::Editor.IsInEditor() && !Vision::Editor.IsPlaying() && m_bProfilingEnabled)
      StopProfiling();
  }


}

BOOL VRSDClient::EngineMessageCallback(const char* szMessage, VisApiMessageType_e messageType, void* pUserData)
{
  if(!pUserData)
    return TRUE;

  VRSDClient* pClient = (VRSDClient*)pUserData;
  if(!pClient->m_pConnection)
    return TRUE;

  VMessage* pMsg = new VMessage('LOGE', 5 + (int)strlen(szMessage));
  pMsg->Write((int)messageType);
  pMsg->Write(szMessage);

  pClient->m_pConnection->SendAsync(pMsg);

  return TRUE;
}

void VRSDClient::HandleScriptReloadMessage(VMessage* pMessage)
{
  if(!pMessage)
    return;

  char* pFileName = NULL;
  char* pNewContent = NULL;
  if(pMessage->ReadString(&pFileName) && pMessage->ReadString(&pNewContent))
  {
    VScriptResourceManager* pSM = (VScriptResourceManager*)Vision::GetScriptManager();

    VScriptResource* pRes = (VScriptResource*)pSM->GetResourceByName(pFileName);

    if(pRes)
      pRes->ReloadAndReplace(pNewContent);
  }
}

void VRSDClient::HandleScriptEventForProfiling(VRSDScriptEvent* pScriptEvent)
{
  if(!pScriptEvent)
    return;

  const char* pFileName = pScriptEvent->pFileName ? pScriptEvent->pFileName : "";
  const char* pFunctionName = pScriptEvent->pFunctionName ? pScriptEvent->pFunctionName : "";
  int iLineDefined = pScriptEvent->iLineDefined;


  // Note: This code is not very memory friendly..
  if(pScriptEvent->eEventType == VRSDScriptEvent::EVENT_ENTER_FUNCTION)
  {
    m_pProfilingStack->Push(new VRSDProfilingSample(pFileName, pFunctionName, pScriptEvent->iLineDefined));
  }
  else if(pScriptEvent->eEventType == VRSDScriptEvent::EVENT_LEAVE_FUNCTION)
  {
    // safe guard
    if(!m_pProfilingStack->IsEmpty())
    {
      uint64 uiStopTime = VGLGetTimer();
      VRSDProfilingSample* pSample = m_pProfilingStack->Pop();

      uint64 uiTimeTaken = uiStopTime - pSample->m_uiStartTime;

      // Store the profiling information
      UpdateProfilingInformation(pFileName, iLineDefined == -1 ? "(native)" : pFunctionName, iLineDefined, uiTimeTaken);

      V_SAFE_DELETE(pSample);
    }
  }
}

bool VRSDClient::SendProfilingResults()
{
  if(!m_pConnection)
    return false;

  unsigned int uiArraySize = m_pProfilingInformations.GetValidSize();
  
  VMessage* pMsg = new VMessage('RRSP', uiArraySize * 128 + 4);
  
  // Count of elements
  pMsg->Write(uiArraySize);

  // Write elements
  for(unsigned int i = 0; i < uiArraySize; i++)
  {
    VRSDProfilingInformation* pCurrent = m_pProfilingInformations[i];

    pMsg->Write(pCurrent->m_pFunctionName);
    pMsg->Write(pCurrent->m_pFileName);
    pMsg->Write(pCurrent->m_iLineDefined);
    pMsg->Write(pCurrent->m_iCalls);
    pMsg->Write((unsigned int)pCurrent->GetAverageTime());
    pMsg->Write((unsigned int)pCurrent->m_uiMaxTime);
    pMsg->Write((unsigned int)pCurrent->m_uiMinTime);
  }

  VMutexLocker lock(m_ConnectionMutex);
  bool bRet = m_pConnection && m_pConnection->SendAsync(pMsg);
  
  return bRet;
}

void VRSDClient::UpdateProfilingInformation(const char* pFileName, const char* pFunctionName, int iLineDefined, uint64 uiTimeTaken)
{
  // check if there is a profiling information for this file and function already in the array
  VRSDProfilingInformation* pInfo = NULL;
  unsigned int uiArraySize = m_pProfilingInformations.GetValidSize();
  for(unsigned int i = 0; i < uiArraySize; i++)
  {
    VRSDProfilingInformation* pCurrent = m_pProfilingInformations[i];

    if(pCurrent->Matches(pFunctionName, pFileName, iLineDefined))
    {
      pInfo = pCurrent;
      break;
    }
  }

  if(pInfo)
    pInfo->AddSample(uiTimeTaken);

  else
  {
    pInfo = new VRSDProfilingInformation(pFunctionName, pFileName, iLineDefined, uiTimeTaken);
    m_pProfilingInformations[uiArraySize] = pInfo;
  }
}

//-----------------------------------------------------------------------------------

void VRSDClient::HandleDebuggingMessage(VMessage* pMessage)
{
  if(!pMessage || !m_pClientLanguageImplementation)
    return;

  // Handle debugging messages here

  switch(pMessage->GetMessageType())
  {
    case 'RSRN':
      HandleScriptReloadMessage(pMessage);
      break;

    case 'GCST':
      {
        // get the callstack from the scripting language implementation
        DynArray_cl<VRSDClientCallstackEntry> Callstack(16);
        unsigned int CallstackEntryCount = 0;
        
        m_pClientLanguageImplementation->GetCallstack(Callstack, CallstackEntryCount);
        
        // send it
        SendCallstack(Callstack, CallstackEntryCount);
      }

      break;

    case 'GGSY':
      {
        // get the global symbol collection from the scripting language implementation
        DynArray_cl<VRSDScriptSymbol> globalSymbols(32, VRSDScriptSymbol());
        unsigned int iGlobalSymbolCount = 0;

        m_pClientLanguageImplementation->GetGlobalSymbols(globalSymbols, iGlobalSymbolCount);

        // send it
        SendSymbols('GSYC', globalSymbols, iGlobalSymbolCount);
      }


      break;

    case 'GLSY':
      {
        // get the local symbol collection from the scripting language implementation
        DynArray_cl<VRSDScriptSymbol> localSymbols(16, VRSDScriptSymbol());
        unsigned int iLocalSymbolCount = 0;

        m_pClientLanguageImplementation->GetLocalSymbols(localSymbols, iLocalSymbolCount);

        // send it
        SendSymbols('LSYC', localSymbols, iLocalSymbolCount);
      }

      break;

    case 'GSFG':
      {
        // read the symbol name for which the subsymbols are requested
        char* pSymbolName = NULL;
        if(pMessage->ReadString(&pSymbolName))
        {
          // get the global symbol collection from the scripting language implementation
          DynArray_cl<VRSDScriptSymbol> globalSymbols(16, VRSDScriptSymbol());
          unsigned int iGlobalSymbolCount = 0;

          if(m_pClientLanguageImplementation->GetSubSymbolsForGlobal(pSymbolName, globalSymbols, iGlobalSymbolCount))
          {
            // send it
            SendSymbols('GSSC', globalSymbols, iGlobalSymbolCount, pSymbolName);
          }
        }
      }
      break;

    case 'GSFL':
      {
        // read the symbol name for which the subsymbols are requested
        char* pSymbolName = NULL;
        if(pMessage->ReadString(&pSymbolName))
        {
          // get the global symbol collection from the scripting language implementation
          DynArray_cl<VRSDScriptSymbol> localSymbols(16, VRSDScriptSymbol());
          unsigned int iLocalSymbolCount = 0;

          if(m_pClientLanguageImplementation->GetSubSymbolsForLocal(pSymbolName, localSymbols, iLocalSymbolCount))
          {
            // send it
            SendSymbols('LSSC', localSymbols, iLocalSymbolCount, pSymbolName);
          }
        }
      }
      break;

      // get userdata type (local / global)
    case 'GUDT':
    case 'LUDT':
      {
        // read first the variable name we should check
        char* pVariableName = NULL;
        if(!pMessage->ReadString(&pVariableName))
          break;

        char pUserDataTypeName[512];
        bool success;

        if(pMessage->GetMessageType() == 'GUDT')
          success = m_pClientLanguageImplementation->GetGlobalType(pVariableName, pUserDataTypeName);
        else
          success = m_pClientLanguageImplementation->GetLocalType(pVariableName, pUserDataTypeName);

        if(success)
        {
          VMessage msg('VUDT', (int)strlen(pUserDataTypeName) + 5);
          msg.Write(pUserDataTypeName);

          VMutexLocker lock(m_ConnectionMutex);
          if(m_pConnection)
            m_pConnection->Send(&msg);
        }
        else
        {
          // if no type is found submit that
          VMessage msg('VUDT', 5);
          msg.Write("");

          VMutexLocker lock(m_ConnectionMutex);
          if(m_pConnection)
            m_pConnection->Send(&msg);
        }
      }
      break;

    // update the value of a variable (non userdata = strings, bools, numbers)
    case 'LVCU':
    case 'GVCU':
      UpdateVariable(pMessage);
      break;

    // request for user data update
    case 'LUDU':
    case 'GUDU':
      UpdateUserDataVariable(pMessage);
      break;

    // request for local user data members
    case 'LUDM':
    case 'GUDM':
      HandleUserDataMemberRequest(pMessage);
      break;
  }
}


void VRSDClient::UpdateVariable(VMessage* pMessage)
{
  // read first the variable name we should update
  char* pVariableName = NULL;
  if(!pMessage->ReadString(&pVariableName))
    return;

  // read the new value for the variable
  char* pNewValue = NULL;
  if(!pMessage->ReadString(&pNewValue))
    return;

  if(pMessage->GetMessageType() == 'LVCU')
    m_pClientLanguageImplementation->UpdateLocalVariable(pVariableName, pNewValue);
  else
    m_pClientLanguageImplementation->UpdateGlobalVariable(pVariableName, pNewValue);
}

void VRSDClient::UpdateUserDataVariable(VMessage* pMessage)
{
  // read first the variable name we should update
  char* pVariableName = NULL;
  if(!pMessage->ReadString(&pVariableName))
    return;

  // get the userdata type of the variable
  char* pUserDataType = NULL;
  if(!pMessage->ReadString(&pUserDataType))
    return;

  // read the member which should be updated
  char* pMember = NULL;
  if(!pMessage->ReadString(&pMember))
    return;

  // read the new value for the member
  char* pNewValue = NULL;
  if(!pMessage->ReadString(&pNewValue))
    return;

  // get the corresponding user data accessor by the user data type
  IVRSDUserDataAccessor* pUserDataAccessor = GetUserDataAccessor(pUserDataType);
  SwigTypeDataAccessor universalDataAccessor("universal");

  if(pUserDataAccessor == NULL)
    pUserDataAccessor = &universalDataAccessor;

  void* pUserDataPointer = NULL;
  void* pEnvironment = NULL;

  if(pMessage->GetMessageType() == 'LUDU')
  {
    if(!m_pClientLanguageImplementation->GetUserDataPointerFromLocal(pVariableName, &pUserDataPointer, &pEnvironment))
      return;
  }
  else //if(pMessage->GetMessageType() == 'GUDU')
  {
    VASSERT(pMessage->GetMessageType() == 'GUDU');
    if(!m_pClientLanguageImplementation->GetUserDataPointerFromGlobal(pVariableName, &pUserDataPointer, &pEnvironment))
      return;
  }

  // Update member
  if (!pUserDataAccessor->UpdateMemberFromString(pUserDataPointer, pEnvironment, pMember, pNewValue))
  {
    // Check if the member is a hidden global (created at runtime, but member of the user object logically)
    m_pClientLanguageImplementation->UpdateHiddenGlobalVariable(pUserDataPointer, pMember, pNewValue);
  }
}

void VRSDClient::HandleUserDataMemberRequest(VMessage* pMessage)
{
  // read first the variable name we should check
  char* pVariableName = NULL;
  if(!pMessage->ReadString(&pVariableName))
    return;

  // get the userdata type of the variable
  char* pUserDataType = NULL;
  if(!pMessage->ReadString(&pUserDataType))
    return;

  // get the corresponding user data accessor by the user data type
  IVRSDUserDataAccessor* pUserDataAccessor = GetUserDataAccessor(pUserDataType);
  SwigTypeDataAccessor universalDataAccessor("universal");

  if(pUserDataAccessor==NULL)
    pUserDataAccessor = &universalDataAccessor;

  DynArray_cl<VRSDScriptSymbol> members(16, VRSDScriptSymbol());
  unsigned int iMemberCount = 0;

  // Only get the user data member if there is an accessor
  void* pUserDataPointer = NULL;
  void* pEnvironment = NULL;

  if(pMessage->GetMessageType() == 'LUDM')
  {
    if(m_pClientLanguageImplementation->GetUserDataPointerFromLocal(pVariableName, &pUserDataPointer, &pEnvironment))
      pUserDataAccessor->GetUserDataMembers(pUserDataPointer, pEnvironment, members, iMemberCount);
  }
  else if(pMessage->GetMessageType() == 'GUDM')
  {
    if(m_pClientLanguageImplementation->GetUserDataPointerFromGlobal(pVariableName, &pUserDataPointer, &pEnvironment))
      pUserDataAccessor->GetUserDataMembers(pUserDataPointer, pEnvironment, members, iMemberCount);
  }


  SendSymbols('SMUD', members, iMemberCount, pVariableName);
}

//-----------------------------------------------------------------------------------

bool VRSDClient::SendScriptEvent(VRSDScriptEvent* pScriptEvent)
{
  // create a script event message with some memory preallocated for the data of the script event
  VMessage Msg('SEVT', 128);
  Msg.Write(pScriptEvent->iLineNumber);
  Msg.Write(pScriptEvent->eEventType);
  Msg.Write(pScriptEvent->eExecutionType);
  Msg.Write(pScriptEvent->pFileName);
  Msg.Write(pScriptEvent->pFunctionName);
  Msg.Write(pScriptEvent->iLineDefined);
  Msg.Write(pScriptEvent->pErrorMessage);

  VMutexLocker lock(m_ConnectionMutex);
  bool bSuccess = m_pConnection && m_pConnection->Send(&Msg);

  return bSuccess;
}

bool VRSDClient::SendCallstack(DynArray_cl<VRSDClientCallstackEntry> &Callstack, unsigned int &CallstackEntryCount)
{
  // create a callstack message and preallocate some memory for the callstack entries
  VMessage Msg('CAST', CallstackEntryCount * 64);
  Msg.Write(CallstackEntryCount);

  for(unsigned int i = 0; i < CallstackEntryCount; i++)
  {
    Msg.Write(Callstack[i].pFunctionName);
    Msg.Write(Callstack[i].pFileName);
    Msg.Write(Callstack[i].iLineNumber);
    Msg.Write(Callstack[i].iLineDefined);
  }

  VMutexLocker lock(m_ConnectionMutex);
  bool bSuccess = m_pConnection && m_pConnection->Send(&Msg);

  return bSuccess;
}

bool VRSDClient::SendSymbols(int Type, DynArray_cl<VRSDScriptSymbol>& Symbols, unsigned int& SymbolCount, char* pParentSymbol /*= NULL*/)
{
  if(Type != 'LSYC' && Type != 'GSYC' && Type != 'GSSC' && Type != 'LSSC' && Type != 'SMUD')
    return false;

  // create a symbol collection message
  VMessage Msg(Type, SymbolCount * 64);
  Msg.Write(SymbolCount);

  for(unsigned int i = 0; i < SymbolCount; i++)
  {
    // symbol name
    Msg.Write(Symbols[i].GetSymbolName());

    // full symbolname
    if(pParentSymbol)
    {
      VMemoryTempBuffer<512> TempBuffer((int)strlen(pParentSymbol) + (int)strlen(Symbols[i].GetSymbolName()) + 2);
      sprintf(TempBuffer.AsChar(), "%s.%s", pParentSymbol, Symbols[i].GetSymbolName());
      Msg.Write(TempBuffer.AsChar());
    }
    else
    {
      Msg.Write(Symbols[i].GetSymbolName());
    }

    Msg.Write(Symbols[i].GetSymbolContent());
    Msg.Write(Symbols[i].m_eSymbolType);
    Msg.Write(Symbols[i].IsUpdateableByDebugger() ? 1 : 0);
  }

  VMutexLocker lock(m_ConnectionMutex);
  bool bSuccess = m_pConnection && m_pConnection->Send(&Msg);

  return bSuccess;
}

bool VRSDClient::IsConnected()
{
  return m_pConnection != NULL;
}

void VRSDClient::SetClientLanguageImplementation(IVRSDClientLanguageImplementation *pClientLanguageImplementation)
{

  // if there is already a script implementation set
  // unregister the script event and clean it up
  if(m_pClientLanguageImplementation)
  {
    m_pClientLanguageImplementation->ScriptEvent -= this;
    delete m_pClientLanguageImplementation;
    m_pClientLanguageImplementation = NULL;
  }

  // copy the new value
  m_pClientLanguageImplementation = pClientLanguageImplementation;

  // subscribe to events if this is a valid implementation
  if(m_pClientLanguageImplementation)
    m_pClientLanguageImplementation->ScriptEvent += this;
}

IVRSDClientLanguageImplementation* VRSDClient::GetClientLanguageImplementation()
{
  return m_pClientLanguageImplementation;
}

VRSDClient& VRSDClient::GetGlobalClient()
{
  return g_GlobalClient;
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
