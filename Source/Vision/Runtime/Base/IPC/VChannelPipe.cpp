/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/BasePCH.h>
#include <iostream>

#include <Vision/Runtime/Base/IPC/VIPCTrace.hpp>
#include <Vision/Runtime/Base/IPC/VMessage.hpp>
#include <Vision/Runtime/Base/IPC/VChannelPipe.hpp>

namespace
{
  void SafeCloseHandle(HANDLE& handle)
  {
    if(handle != NULL)
    {
      CloseHandle(handle);
      handle = NULL;
    }
  }
}

// ----------------------------------------------------------------------------
// HelperFunctions
// ----------------------------------------------------------------------------
DWORD HandleOverlapped(HANDLE hFile, LPOVERLAPPED lpOverlapped,
                       HANDLE hCancelEvent, LPDWORD lpNumberOfBytesTransferred, DWORD dwTimeout)
{
  DWORD lastError = GetLastError();
  if (lastError != ERROR_IO_PENDING)
  {
    return lastError;
  }

  HANDLE waitObjects[] =
  {
    lpOverlapped->hEvent,
    hCancelEvent
  };

  ResetEvent(hCancelEvent);

  VASSERT_MSG(lpOverlapped->hEvent != NULL, "Expecting a valid event handle!");
  DWORD waitRes = WaitForMultipleObjects(V_ARRAY_SIZE(waitObjects), waitObjects, FALSE, dwTimeout);
  if (waitRes != WAIT_OBJECT_0)
  {
    CancelIo(hFile);

    // If the wait returned due to the cancel event being triggered, we want to return the error code from GetOverlappedResult; otherwise the waiting result is the error code.
    if(waitRes != WAIT_OBJECT_0 + 1)
    {
      return waitRes;
    }
  }

  BOOL bRes = GetOverlappedResult(hFile, lpOverlapped, lpNumberOfBytesTransferred, FALSE);
  return bRes ? NO_ERROR : GetLastError();
}

DWORD HandleOverlapped(HANDLE hFile, LPOVERLAPPED lpOverlapped, HANDLE hCancelEvent, DWORD dwTimeout)
{
  DWORD dummy;
  return HandleOverlapped(hFile, lpOverlapped, hCancelEvent, &dummy, dwTimeout);
}



// ----------------------------------------------------------------------------
// ChannelPipe
// ----------------------------------------------------------------------------
ChannelPipe::ChannelPipe(UINT id, Mode mode, 
  IChannelListener* listener) 
: Channel(id, mode, listener), m_pipe(INVALID_HANDLE_VALUE), 
  m_readEvent(NULL), m_writeEvent(NULL), m_connectTimeout(INFINITE)
{
  if (!CreatePipe())
    std::cout << "Could not create named pipe instance";

  m_readEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  m_writeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  m_cancelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  if (m_readEvent == NULL || m_writeEvent == NULL || m_cancelEvent == NULL)
  {
    std::cout << "Could not create overlapped notification events";
  }
}

ChannelPipe::~ChannelPipe() 
{
  if (m_pipe != INVALID_HANDLE_VALUE)
  {
    if (m_connected)
    {
      TRACE0("ChannelPipe: Disconnecting pipe.");
      //FlushFileBuffers(m_pipe);
      if (m_mode == MODE_SERVER)
      {
        DisconnectNamedPipe(m_pipe);
        TriggerPendingEvent(); // This gets the client to try to read, and thus detect the disconnect
      }
    }
    CloseHandle(m_pipe);
  }
  
  SafeCloseHandle(m_readEvent);
  SafeCloseHandle(m_writeEvent);
  SafeCloseHandle(m_cancelEvent);
}

bool ChannelPipe::CreatePipe()
{
  VString pipename;
  pipename.Format("\\\\.\\pipe\\webvision.%u", m_id); 
  TRACE1("Creating pipe: %s", pipename.AsChar());

  if (m_mode == MODE_SERVER)
  {
    m_pipe = CreateNamedPipe(pipename, 
                             PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
                             PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE, 
                             1,
                             BUFFER_SIZE,
                             BUFFER_SIZE,
                             5000,
                             NULL);
  } 
  else 
  {
    m_pipe = CreateFile(pipename,
                        GENERIC_READ | GENERIC_WRITE,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        NULL);
  }
  if (m_pipe == INVALID_HANDLE_VALUE) 
  {
    DWORD lastError = GetLastError();
    TRACE1("Creation of pipe failed with error %#.8x", lastError);
    return false;
  }
  return true;
}

bool ChannelPipe::Connect()
{
  if (m_connected)
  {
    return true;
  }

  if (m_mode == MODE_SERVER)
  {
    ResetEvent(m_readEvent);

    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(OVERLAPPED));
    overlapped.hEvent = m_readEvent;

    // Start overlapped operation. This may complete immediately, or may
    // go to the background. The result and the last error tell us what
    // happened.
    BOOL connectRes = ConnectNamedPipe(m_pipe, &overlapped);
    if (connectRes)
    {
      // Completed in foreground
      m_connected = true;
    }
    else
    {
      DWORD connectReturnCode = HandleOverlapped(m_pipe, &overlapped, m_cancelEvent, m_connectTimeout);
      if (connectReturnCode == NO_ERROR || connectReturnCode == ERROR_PIPE_CONNECTED)
      {
        m_connected = true;
      }
    }
  }
  else
  {
    DWORD pipeMode = PIPE_READMODE_MESSAGE;
    m_connected = SetNamedPipeHandleState(m_pipe, &pipeMode,
      NULL, NULL) ? true : false;
  }
  return m_connected;
}

void ChannelPipe::Send(Message* msg)
{
  VMutexLocker sendLock(&m_sendProtect);

  if (!m_connected)
  {
    m_outputQueue.Add(msg);
  }
  else 
  {
    while (!m_outputQueue.IsEmpty())
    {
      Message* queuedMsg = m_outputQueue[0];
      if (DoSend(queuedMsg))
      {
        m_outputQueue.RemoveAt(0);
      }
      else
      {
        // Sending the queue failed; queue the incoming message as well, then return
        m_outputQueue.Add(msg);
        return;
      }
    }

    if (!DoSend(msg))
    {
      m_outputQueue.Add(msg);
    }
  }
}

bool ChannelPipe::DoSend(Message* msg)
{
  VASSERT(m_connected);

  ResetEvent(m_writeEvent);
  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(OVERLAPPED));
  overlapped.hEvent = m_writeEvent;

  DWORD bytesWritten;
  BOOL writeRes = WriteFile(m_pipe, msg->GetDataPtr(), (DWORD)msg->GetDataSize(),
    &bytesWritten, &overlapped);
  if (!writeRes)
  {
    DWORD writeReturnCode = HandleOverlapped(m_pipe, &overlapped, m_cancelEvent, INFINITE);
    if (writeReturnCode != NO_ERROR)
    {
      TRACE1("Pipe write failed with error %#.8x", writeReturnCode);
      std::cout << "Write to pipe failed with error code: " << writeReturnCode;
      return false;
    }
  }

  TriggerPendingEvent();
  delete msg;

  return true;
}

void ChannelPipe::ProcessMessages()
{
  TRACE0("Processing messages...");

  if (!m_connected) return;

  char buffer[MAX_MSG_DATA];
  DWORD bytesRead = 1;

  while (bytesRead != 0)
  {
    TRACE0("Reading from pipe...");

    ResetEvent(m_readEvent);
    OVERLAPPED overlapped;
    ZeroMemory(&overlapped, sizeof(OVERLAPPED));
    overlapped.hEvent = m_readEvent;

    BOOL readRes = ReadFile(m_pipe, buffer, MAX_MSG_DATA, &bytesRead, &overlapped);
    if (!readRes)
    {
      DWORD readReturnCode = HandleOverlapped(m_pipe, &overlapped, &bytesRead, INFINITE);
      if (readReturnCode == ERROR_BROKEN_PIPE || readReturnCode == ERROR_PIPE_NOT_CONNECTED)
      {
        TRACE0("Pipe broken; closing client end.");
        CloseHandle(m_pipe);
        m_pipe = INVALID_HANDLE_VALUE;
        m_connected = false;
        break;
      }
      else if (readReturnCode != NO_ERROR)
      {
        TRACE1("Pipe read failed with error %#.8x", readReturnCode);
        std::cout << "Read from pipe failed with error code: " << readReturnCode;
        break;
      }
    }

    //process Message
    if (m_listener != NULL)
    {
      Message msg(buffer, bytesRead);
      if (msg.GetSenderId() == 0)
      {
        //should not happen
        VASSERT(false);
        //msg.SetSenderId(m_id);
      }
      m_listener->OnMessageReceived(msg);
    }

    //more Messages?
    if (!PeekNamedPipe(m_pipe, buffer, MAX_MSG_DATA, &bytesRead, NULL, NULL))
    {
      break;
    }
  }

  //reset event when finished processing
  ResetPendingEvent();

  TRACE0("Finished processing messages.");
}

void ChannelPipe::CancelIO()
{
  SetEvent(m_cancelEvent);
}

bool ChannelPipe::IsValid()
{
  return m_pipe != INVALID_HANDLE_VALUE;
}

void ChannelPipe::SetConnectTimeout(UINT timeout)
{
  m_connectTimeout = timeout;
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
