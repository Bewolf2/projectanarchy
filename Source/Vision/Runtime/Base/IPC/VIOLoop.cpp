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
#include <Vision/Runtime/Base/IPC/VChannelQueue.hpp>
#include <Vision/Runtime/Base/IPC/VIOLoop.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


IOLoop::IOLoop(UINT id, IChannelListener* listener, Channel** channel)
: m_id(id), m_shouldRun(false), m_threadHandle(NULL)
{
  Channel* ioChannel;
  ChannelQueue::CreateQueuePair(id + 1, listener, this, channel, &ioChannel);
  AddChannel(ioChannel);
}

IOLoop::~IOLoop()
{
  if (m_threadHandle != NULL)
  {
    Stop();
  }

  for (int i = 0; i < m_channels.GetLength(); i++)
  {
    delete m_channels[i];
  }

  for (int processIdx = 0; processIdx < m_processes.GetLength(); ++processIdx)
  {
    if (m_processes[processIdx] != NULL)
    {
      CloseHandle(m_processes[processIdx]);
    }
  }
}

Channel* IOLoop::FindChannel(UINT id)
{
  for (int i = 0; i < m_channels.GetLength(); i++)
  {
    if (m_channels[i]->GetId() == id)
    {
      return m_channels[i];
    }
  }
  return NULL;
}

DWORD WINAPI IOLoop::RunIOLoop(LPVOID param)
{
  reinterpret_cast<IOLoop*>(param)->Run();
  return 0;
}

void IOLoop::Run()
{
  while (m_shouldRun)
  {
    //remove channels
    if (m_removeChannels.GetLength() > 0)
    {
      m_channelsMutex.Lock();
      m_removeChannelsMutex.Lock();
      for (int i = 0; i < m_removeChannels.GetLength(); i++)
      {
        m_channels.Remove(m_removeChannels[i]);
        delete m_removeChannels[i];
      }
      m_removeChannels.Reset();
      m_removeChannelsMutex.Unlock();
      m_channelsMutex.Unlock();
    }

    //get channel events
    int numChannels = m_channels.GetLength();
    int numProcesses = m_processes.GetLength();
    int numEvents = numChannels + numProcesses;
    if (numEvents < 1)
    {
      Sleep(10);
      continue;
    }
    else if (numEvents > MAXIMUM_WAIT_OBJECTS)
    {
      VASSERT("Too many Events" == NULL);
      continue;
    }

    HANDLE eventHandles[MAXIMUM_WAIT_OBJECTS];

    m_channelsMutex.Lock();
    for (int i = 0; i < numChannels; i++)
    {
      eventHandles[i] = m_channels[i]->GetPendingMessagesEvent();
    }
    m_channelsMutex.Unlock();
    m_processesMutex.Lock();
    for (int i = 0; i < numProcesses; i++)
    {
      eventHandles[numChannels + i] = m_processes[i];
    }
    m_processesMutex.Unlock();

    DWORD waitRes = WaitForMultipleObjects(numEvents, eventHandles, FALSE, 200);
    if (waitRes == WAIT_FAILED)
    {
      DWORD lastError = GetLastError();
      TRACE1("WaitForMultipleObjects failed with error code %#.8x", lastError);
      std::cout << "WaitForMultipleObjects failed with error code: " << lastError << std::endl;
      continue;
    }

    if (waitRes == WAIT_TIMEOUT)
      continue;
    
    int eventIndex = waitRes - WAIT_OBJECT_0;
    if (eventIndex >= 0 && eventIndex < numChannels) 
    {
      TRACE1("IOLoop: Event on channel %d", eventIndex);
      m_channels[eventIndex]->ProcessMessages();
      if (!m_channels[eventIndex]->IsValid())
      {
        TRACE1("IOLoop: Channel %d has been closed", eventIndex);
        MsgChannelClosed msg(eventIndex);
        msg.SetReceiverId(m_id);
        msg.SetSenderId(1);
        OnMessageReceived(msg);
      }
    }
    else if (eventIndex < numEvents)
    {
      TRACE1("IOLoop: Process %d signaled", eventIndex - numChannels);
      HANDLE processHandle = m_processes[eventIndex - numChannels];

      MsgPeerIsDead msg(processHandle);
      msg.SetReceiverId(m_id);
      msg.SetSenderId(1);
      OnMessageReceived(msg);

      m_processesMutex.Lock();
      CloseHandle(processHandle);
      m_processes.Remove(processHandle);
      m_processesMutex.Unlock();
    }
  }
}

void IOLoop::Start()
{
  if (m_threadHandle != NULL)
  {
    return;
  }

  m_shouldRun = true;
  m_threadHandle = CreateThread(NULL, 0, RunIOLoop, this, 0, NULL);
}

void IOLoop::Stop()
{
  if (m_threadHandle == NULL)
  {
    return;
  }

  m_shouldRun = false;

  m_channelsMutex.Lock();
  for (int i = 0; i < m_channels.GetLength(); ++i)
  {
    m_channels[i]->CancelIO();
  }
  m_channelsMutex.Unlock();

  DWORD res = WaitForSingleObject(m_threadHandle, 5000);
  if (res == WAIT_TIMEOUT)
  {
    VASSERT_MSG(FALSE, "IOLoop failed to terminate!");
  }

  CloseHandle(m_threadHandle);
  m_threadHandle = NULL;
}

void IOLoop::AddChannel(Channel* channel)
{
  m_channelsMutex.Lock();
  m_channels.Append(channel);
  m_channelsMutex.Unlock();
}

void IOLoop::RemoveChannel(Channel* channel)
{
  m_removeChannelsMutex.Lock();
  m_removeChannels.Append(channel);
  m_removeChannelsMutex.Unlock();
}

Channel* IOLoop::GetChannel(int index) const
{
  return m_channels[index];
}

void IOLoop::AddProcess(HANDLE process)
{
  m_processesMutex.Lock();
  m_processes.Append(process);
  m_processesMutex.Unlock();
}

void IOLoop::TerminateAllProcesses()
{
  for (int i = 0; i < m_processes.GetLength(); i++)
  {
    TerminateProcess(m_processes[i], 0);
  }
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
