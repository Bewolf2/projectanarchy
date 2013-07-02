/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IPC\VMessage.hpp

#ifndef __IPC_MESSAGE_HPP__
#define __IPC_MESSAGE_HPP__

#include <Vision/Runtime/Base/VBase.hpp>

static const UINT MAX_MSG_DATA = 1024;
//HEADER_LAYOUT: 
// |name|numData|receiverId|senderId| 
static const UINT MSG_HEADER_SIZE = 4 * sizeof(UINT);

/// \brief
///   General message class used to communicate via channels. Used internally e.g. by the Asset Management system.
class Message
{
public:
  enum MessageIds
  {
    MSG_UNDEFINED = 0,      // the message is invalid

    MSG_PEER_IS_DEAD,       // the peer process is dead
    MSG_CHANNEL_CLOSED,     // the last communication channel to any peer has been closed
    MSG_SHUTDOWN_REQUEST,   // the peer process requested a shutdown

    MSG_USER                // first message ID that can be freely used
  };

protected:
  char m_data[MAX_MSG_DATA];
  UINT m_dataSize;
  UINT m_numData;
  mutable UINT m_readerPos;

  UINT m_forwardId;

  void CopyFrom(const Message& other);

public:
  Message(UINT messageId = MSG_UNDEFINED);
  Message(char* data, UINT dataSize);

  Message(const Message& other);

  ~Message();

  Message& operator=(const Message& other);

  UINT GetMessageId() const;
  UINT GetNumData() const;

  const char* AddData(const void* data, UINT sizeInByte);

  template<typename T>
  const T* GetData(UINT* sizeInByte = NULL) const;

  const char* GetDataPtr() const;
  UINT GetDataSize() const;

  void SetReceiverId(UINT id);
  UINT GetReceiverId() const;

  void SetSenderId(UINT id);
  UINT GetSenderId() const;

  void SetForwardId(UINT id);
  UINT GetForwardId() const;
};

inline Message::Message(UINT messageId)
{
  memset(this, 0, sizeof(Message));
  *reinterpret_cast<UINT*>(m_data) = messageId;
  m_dataSize = MSG_HEADER_SIZE;
  m_readerPos = m_dataSize;
}

inline Message::Message(char* data, UINT dataSize) :
  m_forwardId(0)
{
  memcpy_s(m_data, MAX_MSG_DATA, data, dataSize);
  m_numData = *(reinterpret_cast<const UINT*>(m_data) + 1);
  m_dataSize = dataSize;
  m_readerPos = MSG_HEADER_SIZE;
}

inline Message::Message(const Message& other)
{
  CopyFrom(other);
}

inline Message::~Message()
{
}

inline Message& Message::operator=(const Message& other)
{
  CopyFrom(other);
  return *this;
}

inline void Message::CopyFrom(const Message& other)
{
  m_dataSize = other.m_dataSize;
  memcpy(m_data, other.m_data, m_dataSize);
  m_numData = other.m_numData;

  m_forwardId = other.m_forwardId;

  m_readerPos = MSG_HEADER_SIZE;
}

inline UINT Message::GetMessageId() const
{
  return *reinterpret_cast<const UINT*>(m_data);
}

inline UINT Message::GetNumData() const
{
  return *(reinterpret_cast<const UINT*>(m_data) + 1);
}

inline const char* Message::AddData(const void* data, UINT sizeInByte)
{
  char* currentDataPtr = m_data + m_dataSize;
  memcpy(currentDataPtr, &sizeInByte, sizeof(UINT));

  currentDataPtr += sizeof(UINT);
  memcpy(currentDataPtr, data, sizeInByte);

  ++m_numData;
  *(reinterpret_cast<UINT*>(m_data) + 1) = m_numData;

  m_dataSize += sizeInByte + sizeof(UINT);

  return currentDataPtr;
}

template<typename T>
inline const T* Message::GetData(UINT* sizeInByte) const
{
  const char* currentDataPtr = m_data + m_readerPos;
  UINT size = *reinterpret_cast<const UINT*>(currentDataPtr);
  m_readerPos += size + sizeof(UINT);
  if (sizeInByte != NULL)
    *sizeInByte = size;

  return reinterpret_cast<const T*>(currentDataPtr + sizeof(UINT));
}

inline const char* Message::GetDataPtr() const 
{
  return m_data;
}

inline UINT Message::GetDataSize() const
{
  return m_dataSize;
}

inline void Message::SetReceiverId(UINT id)
{
  *(reinterpret_cast<UINT*>(m_data) + 2) = id;
}

inline UINT Message::GetReceiverId() const
{
  return *(reinterpret_cast<const UINT*>(m_data) + 2);
}

inline void Message::SetSenderId(UINT id)
{
  *(reinterpret_cast<UINT*>(m_data) + 3) = id;
}

inline UINT Message::GetSenderId() const
{
  return *(reinterpret_cast<const UINT*>(m_data) + 3);
}

inline void Message::SetForwardId(UINT id)
{
  m_forwardId = id;
}

inline UINT Message::GetForwardId() const
{
  return m_forwardId;
}


/// \brief
///  Wrapper class for a MSG_PEER_IS_DEAD
class MsgPeerIsDead : public Message
{
public:
  HANDLE m_processHandle;

  MsgPeerIsDead(HANDLE processHandle) 
  : Message(MSG_PEER_IS_DEAD), m_processHandle(processHandle)
  {
    AddData(&m_processHandle, sizeof(HANDLE));
  }

  MsgPeerIsDead(const Message& msg)
  : Message(MSG_PEER_IS_DEAD)
  {
    CopyFrom(msg);
    m_processHandle = *GetData<HANDLE>();
  }
};


/// \brief
///  Wrapper class for a MSG_CHANNEL_CLOSED
class MsgChannelClosed : public Message
{
public:
  int m_channelIndex;

  MsgChannelClosed(int channelIndex) 
  : Message(MSG_CHANNEL_CLOSED), m_channelIndex(channelIndex)
  {
    AddData(&m_channelIndex, sizeof(int));
  }

  MsgChannelClosed(const Message& msg)
  : Message(MSG_CHANNEL_CLOSED)
  {
    CopyFrom(msg);
    m_channelIndex = *GetData<int>();
  }
};

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
