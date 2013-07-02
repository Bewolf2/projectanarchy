/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSenderReceiver.hpp

#ifndef _VISION_VSENDERRECEIVER_HPP
#define _VISION_VSENDERRECEIVER_HPP



class VArchive;
class VSenderReceiver;


DECLARE_LIST(VSenderReceiverList, VSenderReceiver, VBASE_IMPEXP)


/// \brief
///   Provides functionality for receiving messages from senders and for 
///   sending messages to a list of registered receivers.
///
/// Each listener is of type VSenderReceiver as well. Override the OnReceive function
/// to handle received messages.
///
/// Messages consist of a message ID and an optional data pointer.
class VSenderReceiver : public VTypedObject
{
  
  V_DECLARE_SERIAL_DLLEXP( VSenderReceiver, VBASE_IMPEXP )
  
public:

  /// \brief
  ///   Adds a receiver to the list
  /// \param pReceiver
  ///   the receiver to add
  inline void AddReceiver(VSenderReceiver *pReceiver)
  {
    m_ReceiverList.Append(pReceiver);
  }

  /// \brief
  ///   Removes a receiver from the list. 
  ///
  /// The list must contain this value, otherwise an assertion will be raised.
  ///
  /// \param pReceiver
  ///   the receiver to remove
  inline void RemoveReceiver(VSenderReceiver *pReceiver)
  {
    m_ReceiverList.Remove(pReceiver);
  }

  /// \brief
  ///   Receives a message.
  /// 
  /// The implementation in this base class does nothing. Override this method 
  /// in derived classes to implement custom message handling.
  /// 
  /// \param pSender
  ///   sender of the message
  /// \param iMsg
  ///   ID of the message
  /// \param pData
  ///   pointer to message data; may be \c NULL
  VBASE_IMPEXP virtual void OnReceive(VSenderReceiver* pSender, int iMsg, void *pData);

  /// \brief
  ///   Sends a message to all registered receivers.
  /// 
  /// This function calls the virtual OnReceive function on all registered receivers. The
  /// message sender passed to the OnReceive function will be this object.
  /// 
  /// \param iMsg
  ///   ID of the message
  /// \param pData
  ///   pointer to message data; may be \c NULL
  VBASE_IMPEXP virtual void Send(int iMsg, void *pData);

  /// \brief
  ///   Sends a message to all registered receivers, allowing to specify a sender.
  /// 
  /// This function calls the virtual OnReceive function on all registered receivers.
  /// 
  /// \param iMsg
  ///   ID of the message
  /// \param pData
  ///   pointer to message data; may be \c NULL
  /// \param pSender
  ///   the sender of the message
  VBASE_IMPEXP virtual void Send(int iMsg, void *pData, VSenderReceiver* pSender);
  
  
protected:
  /// \brief
  ///   Serializes this VSenderReceiver.
  /// \param ar
  ///   the archive to read form or write to
  /// \note
  ///   The base class does not perform any serialization; override this method
  ///   to implement serialization in derived classes.
  VBASE_IMPEXP virtual void Serialize(VArchive &ar);

  VSenderReceiverList m_ReceiverList; ///< List of all registered receivers
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
