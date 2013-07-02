/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __IPC_CHANNELPIPE_HPP__
#define __IPC_CHANNELPIPE_HPP__

#include <Vision/Runtime/Base/VBase.hpp>

#include <Vision/Runtime/Base/IPC/VChannel.hpp>

class Message;

/// \brief
///  Channel that can be used to communicate with other processes
///
/// The implementation uses Windows named pipes in synchronous mode. 
/// This means all read and write operations are blocking.
///
/// \see IOLoop
/// \see Channel
/// \see ChannelQueue
class ChannelPipe : public Channel
{
private:
  static const int BUFFER_SIZE = 4096;

  DWORD m_connectTimeout;

  HANDLE m_pipe;
  HANDLE m_readEvent;
  HANDLE m_writeEvent;
  HANDLE m_cancelEvent;

  LinkedList_cl<Message*> m_outputQueue;

  VMutex m_sendProtect;

  bool CreatePipe();
  bool DoSend(Message* msg);

public:
  /// \brief
  ///  Creates a new channel
  VBASE_IMPEXP ChannelPipe(UINT id, Mode mode, IChannelListener* listener);

  /// \brief
  ///  Breaks the connection and destroys the channel
  VBASE_IMPEXP ~ChannelPipe();

  /// \brief
  ///  Connects the channel to its counterpart
  ///
  /// If this method is called on a server mode pipe, this method will block the 
  /// current thread until a client mode pipe connects to it.
  VBASE_IMPEXP bool Connect();

  /// \brief
  ///  Send a message to the counterpart
  ///
  /// \param msg
  ///  the message to be sent, the channel takes ownership of this message
  ///
  /// Messages get queued if the channel is not connected.
  VBASE_IMPEXP void Send(Message* msg);

  /// \brief
  ///  Processes messages
  VBASE_IMPEXP void ProcessMessages();

  /// \brief
  ///   Cancels any currently active I/O operation, causing Connect, Send, or ProcessMessages
  ///   to return immediately.
  VBASE_IMPEXP virtual void CancelIO() HKV_OVERRIDE;

  /// \brief
  ///  Returns if this channel is still valid
  VBASE_IMPEXP bool IsValid();

  /// \brief
  ///   Sets the timeout for the server side of the pipe to wait for the client 
  ///   side to establish a connection.
  ///
  /// \param timeout
  ///   the timeout value in milliseconds
  VBASE_IMPEXP void SetConnectTimeout(UINT timeout);
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
