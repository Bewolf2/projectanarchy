/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VConnection.hpp

#ifndef __VCONNECTION_INCLUDED__
#define __VCONNECTION_INCLUDED__

  class VMessage;

  /// \brief
  ///   This class describes a connection in the remote communication system
  class VConnection
  {
    public:

      /// \fn VConnection(int iSocket, char* pIdentifier, int iReceiveBufferSize = 16*1024)
      ///
      /// \brief  Constructor. 
      ///
      /// \param  iSocket Index of the socket to use. 
      /// \param [in] pIdentifier The identifier of the connection - this has to be a four letter code e.g "VRSD" or "VRES". Has to be unique per system using the VTarget functionality.
      /// \param  iReceiveBufferSize  Size of the receive buffer. 
      ///
      VConnection(int iSocket, char* pIdentifier, int iReceiveBufferSize = 16*1024);

      /// \fn ~VConnection()
      ///
      /// \brief  Destructor.
      ///
      ~VConnection();

      /// \fn VBASE_IMPEXP bool SendAsync(VMessage* pMessage)
      ///
      /// \brief  Sends a message asynchronously. The message will be enqueued in a queue and if the connection becomes writable it will be sent. 
      ///
      /// \param [in] pMessage  The message object to send. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      VBASE_IMPEXP bool SendAsync(VMessage* pMessage);

      /// \fn VBASE_IMPEXP bool Send(VMessage* pMessage)
      ///
      /// \brief  Sends the given message synchronous on the connection. 
      ///
      /// \param [in] pMessage  The message to send. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      VBASE_IMPEXP bool Send(VMessage* pMessage);

      /// \fn VBASE_IMPEXP bool Recv(VMessage* pMessage)
      ///
      /// \brief  Receives a message from the connection. This call can block if there is no message in the queue!
      ///
      /// \param [in] pMessage  The message object to receive the content into.
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      VBASE_IMPEXP bool Recv(VMessage* pMessage);

      /// \fn VBASE_IMPEXP inline int GetSocket()
      ///
      /// \brief  Gets the internal socket identifier of the connection. 
      ///
      /// \return The socket. 
      ///
      VBASE_IMPEXP inline int GetSocket()
      {
        return m_iSocket;
      }

      /// \fn VBASE_IMPEXP inline const char* GetIdentifier()
      ///
      /// \brief  Gets the identifier of the connection (e.g. "VRSD" or "VRES"). 
      ///
      /// \return null if it fails, else the identifier. 
      ///
      VBASE_IMPEXP inline const char* GetIdentifier()
      {
        return m_pIdentifier;
      }

      /// \fn VBASE_IMPEXP inline bool IsFor(const char* pIdentifier)
      ///
      /// \brief  Query if the given 'pIdentifier' is the same as this connections identifier. 
      ///
      /// \param [in] pIdentifier The identifier. 
      ///
      /// \return true if the given identifier matches the identifier of the connection, false if not. 
      ///
      VBASE_IMPEXP inline bool IsFor(const char* pIdentifier)
      {
        return (pIdentifier && !strcmp(pIdentifier, m_pIdentifier));
      }

      /// \fn VBASE_IMPEXP bool IsValid()
      ///
      /// \brief  Query if this connection is a valid (open) connection.
      ///
      /// \return true if valid, false if not. 
      ///
      VBASE_IMPEXP bool IsValid();

      /// \fn VBASE_IMPEXP VMessage* GetNextASyncReceivedMessage()
      ///
      /// \brief  Gets the next received message from the asynchronous queue. 
      ///
      /// \return null if there is no message in the queue, else the next asynchronously received message. 
      ///
      VBASE_IMPEXP VMessage* GetNextASyncReceivedMessage();

      /// \fn VBASE_IMPEXP VMessage* GetNextASyncMessageToSend()
      ///
      /// \brief  Gets the next asynchronously message to send. 
      ///
      /// \return null if there is no message to send, else the next asynchronously message to send. 
      ///
      VBASE_IMPEXP VMessage* GetNextASyncMessageToSend();

    private:

      /// \fn bool SendImpl(VMessage* pMsg)
      ///
      /// \brief  Send implementation.
      ///
      /// \param [in] pMsg  The message to send.
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      bool SendImpl(VMessage* pMsg);

      /// \fn bool RecvImpl(VMessage* pMsg)
      ///
      /// \brief  Receive implementation. 
      ///
      /// \param [in,out] pMsg  The message object to fill the data into.
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      bool RecvImpl(VMessage* pMsg);

      friend class VTarget;

      /// \fn void SetIdentifier(char* pIdentifier)
      ///
      /// \brief  Sets the identifier of the connection. 
      ///
      /// \param [in,out] pIdentifier The identifier. 
      ///
      void SetIdentifier(char* pIdentifier);

      /// \fn void SetInvalid()
      ///
      /// \brief  Sets the connection to an invalid state. 
      ///
      void SetInvalid();

      /// \fn void AddASyncReceivedMessage(VMessage* pMessage)
      ///
      /// \brief  Adds a asynchronously received message. 
      ///
      /// \param [in] pMessage  The message to add to the queue. 
      ///
      void AddASyncReceivedMessage(VMessage* pMessage);

      /// \fn void Close()
      ///
      /// \brief  Closes the connection.
      ///
      void Close();

      //! Stores the socket identifier of this connection. 
      int m_iSocket;

      //! Stores the list of enqueued messages to send. 
      VPListT<VMessage> m_EnqueuedMessages;
      
      //! Stores the list of received messages. 
      VPListT<VMessage> m_ReceivedMessages;

      //! The mutex guarding the receive queue. 
      VMutex m_ReceiveQueueMutex;
      
      //! The mutex guarding the send queue. 
      VMutex m_SendQueueMutex;

      //! The mutex guarding the send function. 
      VMutex m_SendMutex;
      
      //! The mutex guarding the receive function. 
      VMutex m_RecvMutex;

      //! Stores the identifier of the connection. 
      char* m_pIdentifier;
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
