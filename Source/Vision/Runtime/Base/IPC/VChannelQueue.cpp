/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/BasePCH.h>
#include <Vision/Runtime/Base/IPC/VMessage.hpp>
#include <Vision/Runtime/Base/IPC/VChannelQueue.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

ChannelQueue::ChannelQueue(UINT id, Mode mode, IChannelListener* listener,
  HANDLE serverMessagesEvent, HANDLE clientMessagesEvent)
: Channel(id, mode, listener, serverMessagesEvent, clientMessagesEvent)
{
  if (m_mode == MODE_SERVER)
  {
    m_server2ClientQueue = new VPListT<Message>();
    m_client2ServerQueue = new VPListT<Message>();

    m_s2cMutex = new VMutex();
    m_c2sMutex = new VMutex();
  }
}

ChannelQueue::~ChannelQueue()
{
  if (m_mode == MODE_SERVER)
  {
    delete m_server2ClientQueue;
    delete m_client2ServerQueue;

    delete m_s2cMutex;
    delete m_c2sMutex;
  }
}

void ChannelQueue::CreateQueuePair(UINT id, 
  IChannelListener* serverSideListener, IChannelListener* clientSideListener, 
  Channel** serverSideInstance, Channel** clientSideInstance)
{
  HANDLE serverEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  HANDLE serverEventDup = NULL;
  DuplicateHandle(GetCurrentProcess(), serverEvent, GetCurrentProcess(), &serverEventDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
  VASSERT_MSG(serverEvent != NULL && serverEventDup != NULL, "Creation/Duplication of server handles failed!");

  HANDLE clientEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  HANDLE clientEventDup = NULL;
  DuplicateHandle(GetCurrentProcess(), clientEvent, GetCurrentProcess(), &clientEventDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
  VASSERT_MSG(clientEvent != NULL && clientEventDup != NULL, "Creation/Duplication of client handles failed!");

  ChannelQueue* serverQ = new ChannelQueue(id, MODE_SERVER, serverSideListener, serverEvent, clientEvent);
  ChannelQueue* clientQ = new ChannelQueue(id, MODE_CLIENT, clientSideListener, serverEventDup, clientEventDup);

  clientQ->m_client2ServerQueue = serverQ->m_client2ServerQueue;
  clientQ->m_server2ClientQueue = serverQ->m_server2ClientQueue;

  clientQ->m_s2cMutex = serverQ->m_s2cMutex;
  clientQ->m_c2sMutex = serverQ->m_c2sMutex;

  *serverSideInstance = serverQ;
  *clientSideInstance = clientQ;
}

bool ChannelQueue::Connect()
{
  m_connected = true;
  return true;
}

void ChannelQueue::Send(Message* msg)
{
  if (m_mode == MODE_SERVER)
  {
    m_s2cMutex->Lock();
    m_server2ClientQueue->Append(msg);
    m_s2cMutex->Unlock();
  }
  else
  {
    m_c2sMutex->Lock();
    m_client2ServerQueue->Append(msg);
    m_c2sMutex->Unlock();
  }

  TriggerPendingEvent();
}

bool ChannelQueue::TryDequeue(Message& msg)
{
  VPListT<Message>* queue = m_mode == MODE_SERVER ? 
    m_client2ServerQueue : m_server2ClientQueue;
  VMutex* mutex = m_mode == MODE_SERVER ? m_c2sMutex : m_s2cMutex;
  bool res;

  mutex->Lock();
  if (queue->IsEmpty())
  {
    res = false;
  }
  else
  {
    Message* tmp = queue->Get(0);
    queue->RemoveAt(0);
    msg = *tmp;
    delete tmp;
    res = true;
  }
  mutex->Unlock();

  return res;
}

void ChannelQueue::ProcessMessages()
{
  Message msg;
  while (TryDequeue(msg))
  {
    m_listener->OnMessageReceived(msg);
  }  

  ResetPendingEvent();
}

bool ChannelQueue::IsValid()
{
  return m_server2ClientQueue != NULL && m_client2ServerQueue != NULL;
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
