/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __IPC_IOLOOP_HPP__
#define __IPC_IOLOOP_HPP__

#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>

#include <Vision/Runtime/Base/IPC/VChannel.hpp>

class Message;

/// \brief
///  Message loop that can listen to multiple channels
///
/// This message loop runs in a seperate thread. You need to implement
/// the OnMessageReceived method and notify your main thread 
/// so it can then process the messages sent to it.
///
/// \see Channel
/// \see ChannelPipe
/// \see ChannelQueue
/// \see Message
class IOLoop : public IChannelListener
{
protected:
  UINT m_id;

  static DWORD WINAPI RunIOLoop(LPVOID param);
  HANDLE m_threadHandle;

  VMutex m_channelsMutex;
  VPListT<Channel> m_channels;

  VMutex m_removeChannelsMutex;
  VPListT<Channel> m_removeChannels;  

  VMutex m_processesMutex;
  VPList m_processes;

  bool m_shouldRun;

  Channel* FindChannel(UINT id);

public:
  /// \brief
  ///  Creates a new loop
  ///
  /// \param id
  ///  the loop's id
  ///
  /// \param listener
  ///  Listener to the loop
  ///
  /// \param channel
  ///  The channel to send messages to the loop
  VBASE_IMPEXP IOLoop(UINT id, IChannelListener* listener, Channel** channel);

  /// \brief
  ///  Destroys the loop
  VBASE_IMPEXP virtual ~IOLoop();

  /// \brief
  ///  Runs the loop. Internal use only - DO NOT call this method.
  VBASE_IMPEXP void Run();

  /// \brief
  ///  Start the loop's thread 
  VBASE_IMPEXP void Start();

  /// \brief
  ///  Stops the IOLoop. Blocks until the thread has terminated
  VBASE_IMPEXP void Stop();

  /// \brief
  ///  adds a new channel to the loop
  VBASE_IMPEXP void AddChannel(Channel* channel);

  /// \brief
  ///  removes the channel from the loop
  VBASE_IMPEXP void RemoveChannel(Channel* channel);

  /// \brief
  ///  Returns the channel at the given index
  VBASE_IMPEXP Channel* GetChannel(int index) const;

  /// \brief
  ///  Adds a process to the loop. You will receive a MSG_PROCESS_IS_DEAD if 
  ///  the process exits unintentionally
  VBASE_IMPEXP void AddProcess(HANDLE process);

  /// \brief
  ///  forces all attached processes to terminate.
  ///
  /// Do not use this method to exit processes. Send a MSG_SHUTDOWN_REQUEST instead.
  VBASE_IMPEXP void TerminateAllProcesses();

  /// \brief
  ///  Callback for incoming messages
  VBASE_IMPEXP virtual void OnMessageReceived(const Message& msg) = 0;
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
