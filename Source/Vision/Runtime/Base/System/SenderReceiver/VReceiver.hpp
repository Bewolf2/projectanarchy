/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VReceiver.hpp

#ifndef _VISION_VRECEIVER_HPP
#define _VISION_VRECEIVER_HPP

#include <Vision/Runtime/Base/System/SenderReceiver/VSenderReceiver.hpp>

class  VReceiver;

/// \brief
///   Callback function for handling messages sent to a VReceiver
/// \param mon
///   the VReceiver that called this callback
typedef   void (*RECEIVER_CALLBACK)(VReceiver *mon);


/// \brief
///   Receives messages from a VSenderReceiver or VBroadcaster
/// 
/// Messages can be handled either by registering a callback function, or by deriving
/// from this class and overriding the ProcessMessage function.
/// 
/// In any case, the VReceiver must be added to a sender! This can be done with
/// VBroadcaster::Add() or VSenderReceiver::AddRecevier().
class VReceiver : public VSenderReceiver
{
public:

  /// \brief
  ///   Constructor of the VReceiver class
  VBASE_IMPEXP VReceiver();

  /// \brief
  ///   Constructor of the VReceiver class
  /// \param cb
  ///   the callback function to call when a message is received
  /// \param cookie
  ///   arbitrary value to identify this receiver from the callback function
  VBASE_IMPEXP VReceiver(RECEIVER_CALLBACK cb, void *cookie = NULL);
  
  /// \brief
  ///   Sets the callback function for handling received messages.
  /// \param cb
  ///   the callback function to call when a message is received
  /// \param cookie
  ///   arbitrary value to identify this receiver from the callback function
  VBASE_IMPEXP void SetCallback(RECEIVER_CALLBACK cb, void *cookie = NULL);
  
  /// \brief
  ///   Is called if a messages is received. 
  ///
  /// This implementation calls the ProcessMessage function if this receiver
  /// is enabled.
  /// 
  /// \param pSender
  ///   sender of the message
  /// \param iMsg
  ///   ID of the message
  /// \param pData
  ///   pointer to message data; may be \c NULL
  /// \sa VReceiver::Enable()
  /// \sa VReceiver::Disable()
  VBASE_IMPEXP virtual void OnReceive(VSenderReceiver* pSender, int iMsg, void *pData) HKV_OVERRIDE;
  
  /// \brief
  ///   Calls the callback function if one has been set.
  /// \sa VReceiver::SetCallback()
  VBASE_IMPEXP virtual void ProcessMessage();

  /// \brief
  ///   Disables message handling for this receiver.
  inline void Disable()  { m_bActive = FALSE; }

  /// \brief
  ///   Enables message handling for this receiver.
  inline void Enable()   { m_bActive = TRUE; }


  RECEIVER_CALLBACK m_pCallback; ///< callback pointer
  VBool m_bActive; ///< if \c TRUE, ProcessMessage is called when a message is received
  void *m_pCookie; ///< pointer to client data (e.g., a this pointer)

  VSenderReceiver *m_pSender; ///< sender of the currently handled message
  int m_iMsg; ///< id of the currently handled message
  void *m_pData; ///< data pointer of the currently handled message
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
