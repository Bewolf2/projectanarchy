/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/BasePCH.h>
#include <Vision/Runtime/Base/IPC/VChannel.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

Channel::Channel(UINT id, Mode mode, IChannelListener* listener, 
  HANDLE serverMessagesEvent, HANDLE clientMessagesEvent)
: m_id(id), m_mode(mode), m_listener(listener), m_connected(false),
  m_pendingServerMessagesEvent(NULL), m_pendingClientMessagesEvent(NULL)
{
  if (!CreatePendingEvents(serverMessagesEvent, clientMessagesEvent))
    VASSERT(false);
}

Channel::~Channel()
{
  ResetEvent(m_pendingServerMessagesEvent);
  ResetEvent(m_pendingClientMessagesEvent);

  CloseHandle(m_pendingServerMessagesEvent);
  CloseHandle(m_pendingClientMessagesEvent);
}

bool Channel::CreatePendingEvents(HANDLE serverMessagesEvent, HANDLE clientMessagesEvent)
{
  VString serverEventname;
  serverEventname.Format("events.webvision.server.%u", m_id);
  VString clientEventname;
  clientEventname.Format("events.webvision.client.%u", m_id); 

  if (m_mode == MODE_SERVER)
  {
    SECURITY_ATTRIBUTES securityAttributes;
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.lpSecurityDescriptor = NULL;
    securityAttributes.bInheritHandle = TRUE;

    m_pendingServerMessagesEvent = serverMessagesEvent != NULL
      ? serverMessagesEvent
      : CreateEvent(&securityAttributes, TRUE, FALSE, serverEventname);
    m_pendingClientMessagesEvent = clientMessagesEvent != NULL
      ? clientMessagesEvent
      : CreateEvent(&securityAttributes, TRUE, FALSE, clientEventname); 
  }
  else
  {
    m_pendingServerMessagesEvent = serverMessagesEvent != NULL
      ? serverMessagesEvent
      : OpenEvent(EVENT_ALL_ACCESS, FALSE, serverEventname);
    m_pendingClientMessagesEvent = clientMessagesEvent != NULL
      ? clientMessagesEvent
      : OpenEvent(EVENT_ALL_ACCESS, FALSE, clientEventname);
  }

  if (m_pendingServerMessagesEvent == NULL || m_pendingClientMessagesEvent == NULL)
  {
    return false;
  }

  return true;
}

void Channel::TriggerPendingEvent()
{
  if (m_mode == MODE_SERVER)
  {
    SetEvent(m_pendingClientMessagesEvent);
  }
  else
  {
    SetEvent(m_pendingServerMessagesEvent);
  }
}

void Channel::ResetPendingEvent()
{
  if (m_mode == MODE_SERVER)
  {
    ResetEvent(m_pendingServerMessagesEvent);
  }
  else
  {
    ResetEvent(m_pendingClientMessagesEvent);
  }
}

UINT Channel::GetId()
{
  return m_id;
}

HANDLE Channel::GetPendingMessagesEvent()
{
  if (m_mode == MODE_SERVER)
  {
    return m_pendingServerMessagesEvent;
  }
  else
  {
    return m_pendingClientMessagesEvent;
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
