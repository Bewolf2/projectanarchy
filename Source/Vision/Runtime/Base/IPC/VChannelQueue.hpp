/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __IPC_CHANNELQUEUE_HPP__
#define __IPC_CHANNELQUEUE_HPP__

#include <Vision/Runtime/Base/VBase.hpp>

#include <Vision/Runtime/Base/IPC/VChannel.hpp>

class Message;

/// \brief
///  Channel that can be used to communicate with other threads
///
/// Use the CreateQueuePair method to create a new channel pair
///
/// \see IOLoop
/// \see ChannelPipe
/// \see Channel
class ChannelQueue : public Channel
{
private:
  VPListT<Message>* m_server2ClientQueue;
  VMutex* m_s2cMutex;

  VPListT<Message>* m_client2ServerQueue;
  VMutex* m_c2sMutex;

  bool TryDequeue(Message& msg);
  ChannelQueue(UINT id, Mode mode, IChannelListener* listener, 
    HANDLE serverMessagesEvent, HANDLE clientMessagesEvent);

public:
  /// \brief
  ///  Creates a new channel pair 
  VBASE_IMPEXP static void CreateQueuePair(UINT id, 
    IChannelListener* serverSideListener,
    IChannelListener* clientSideListener,
    Channel** serverSideInstance, Channel** clientSideInstance);

  /// \brief
  ///  Breaks the connection and destroys the channel
 VBASE_IMPEXP  ~ChannelQueue();

  /// \brief
  ///  Connects the channel to its counterpart
  VBASE_IMPEXP bool Connect();

  /// \brief
  ///  Send a message to the counterpart
  VBASE_IMPEXP void Send(Message* msg);

  /// \brief
  ///  Processes messages
  VBASE_IMPEXP void ProcessMessages();

  /// \brief
  ///  Returns whether this channel is still valid
  VBASE_IMPEXP bool IsValid();
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
