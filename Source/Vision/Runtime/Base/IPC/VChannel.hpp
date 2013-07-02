/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __IPC_CHANNEL_HPP__
#define __IPC_CHANNEL_HPP__

#include <Vision/Runtime/Base/VBase.hpp>

class Message;
class Channel;

enum Mode {
  MODE_SERVER,
  MODE_CLIENT,
};

/// \brief
///  Interface for a listener to a channel
class IChannelListener
{
public:
  virtual void OnMessageReceived(const Message& msg) = 0;
};

/// \brief
///  Base class of a communication channel
///
/// You always need two instances of a channel to communicate: a server part and
/// a client part. 
/// The Channel uses windows events to notify its counterpart if a new message
/// is available. You need to call ProcessMessages once a notification arrived.
///
/// \see IOLoop
/// \see ChannelPipe
/// \see ChannelQueue
class Channel
{
protected:
  HANDLE m_pendingServerMessagesEvent;
  HANDLE m_pendingClientMessagesEvent;

  Mode m_mode;
  UINT m_id;

  IChannelListener* m_listener;

  bool m_connected;
  bool CreatePendingEvents(HANDLE serverMessagesEvent, HANDLE clientMessagesEvent);

  void TriggerPendingEvent();
  void ResetPendingEvent();

public:
  /// \brief
  ///  Creates a new channel
  ///
  /// \param id
  ///   The channels id needs to be a system-wide unique id
  ///
  /// \param mode
  ///   Specifies if this channel is the server or the client part of a channel pair
  ///
  /// \param listener
  ///   The listener attached to this channel
  ///
  /// \param serverMessagesEvent
  ///   if specified, the event to use as the event for signaling pending server
  ///   messages. If not specified, a named event is either created or opened,
  ///   depending on whether this is the client or the server end of the pipe.
  ///
  /// \param clientMessagesEvent
  ///   same as above, but for pending client messages.
  Channel(UINT id, Mode mode, IChannelListener* listener, 
    HANDLE serverMessagesEvent = NULL, HANDLE clientMessagesEvent = NULL);

  /// \brief
  ///   Breaks the connection and destroys the channel
  VBASE_IMPEXP virtual ~Channel();

  /// \brief
  ///  Connects the channel to its counterpart
  VBASE_IMPEXP virtual bool Connect() = 0;

  /// \brief
  ///  Send a message to the counterpart
  VBASE_IMPEXP virtual void Send(Message* msg) = 0;

  /// \brief
  ///  Processes messages
  VBASE_IMPEXP virtual void ProcessMessages() = 0;

  /// \brief
  ///   Cancels any currently active I/O operation, causing Connect, Send, or ProcessMessages
  ///   to return immediately.
  VBASE_IMPEXP virtual void CancelIO() { }

  /// \brief
  ///  Returns if this channel is still valid
  VBASE_IMPEXP virtual bool IsValid() = 0;

  /// \brief
  ///  Returns if this channel is already connected
  VBASE_IMPEXP virtual bool IsConnected() { return m_connected; }

  /// \brief
  ///  Returns the channel's unique id
  VBASE_IMPEXP UINT GetId();

  /// \brief
  ///  Returns the event that is triggered if new messages are available for this channel.
  VBASE_IMPEXP HANDLE GetPendingMessagesEvent(); 
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
