/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBroadcaster.hpp

#if !defined _VISION_VBroadcaster_HPP 
#define _VISION_VBroadcaster_HPP


#include <Vision/Runtime/Base/System/SenderReceiver/VSenderReceiver.hpp>


#ifdef _VISION_PS3
  #ifndef SPU
    #pragma diag_push
    #pragma diag_suppress=613
  #endif
#endif


#ifdef _VISION_PSP2
  #pragma diag_suppress=613
#endif

#ifdef _VISION_WIIU
  #pragma ghs nowarning 611
#endif

/// \brief
///   VSenderReceiver implementation that broadcasts received messages to
///   all registered listeners.
/// 
/// In addition to forwarding received messages, messages can be sent with the Send() function.
class VBroadcaster : public VSenderReceiver
{
public:

  /// \brief
  ///   Adds a new listener that is informed about an event
  inline void Add(VSenderReceiver *pReceiver)
  {
    VSenderReceiver::AddReceiver(pReceiver);
  }

  /// \brief
  ///   Adds a new listener that is informed about an event
  inline void Add(VSenderReceiver &pReceiver)
  {
    VSenderReceiver::AddReceiver(&pReceiver);
  }

  /// \brief
  ///   Removes an added listener
  inline void Remove(VSenderReceiver *pReceiver)
  {
    VSenderReceiver::RemoveReceiver(pReceiver);
  }

  /// \brief
  ///   Removes an added listener
  inline void Remove(VSenderReceiver &pReceiver)
  {
    VSenderReceiver::RemoveReceiver(&pReceiver);
  }

  VBASE_IMPEXP virtual void Send(int iMsg, void *pData) HKV_OVERRIDE
  { 
    VSenderReceiver::Send(iMsg, pData, this);
  }

  /// \copybrief VSenderReceiver::OnReceive()
  ///
  /// This implementation forwards received messages to the registered receivers. The
  /// original sender is replaced with this broadcaster.
  ///
  /// \param pSender
  ///   sender of the message
  /// \param iMsg
  ///   ID of the message
  /// \param pData
  ///   pointer to message data; may be \c NULL
  VBASE_IMPEXP virtual void OnReceive(VSenderReceiver *pSender, int iMsg, void* pData) 
  { 
    VSenderReceiver::Send(iMsg, pData, this);
  }

private:
  VBASE_IMPEXP virtual void AddReceiver(VSenderReceiver *pReceiver)  { VASSERT(FALSE); /*use Add instead*/ }
  VBASE_IMPEXP virtual void RemoveReceiver(VSenderReceiver *pReceiver) { VASSERT(FALSE); /*use Remove instead*/ }
};


#ifdef _VISION_PS3
  #ifndef SPU
    #pragma diag_pop
  #endif
#endif


#ifdef _VISION_PSP2
  #pragma diag_default=613
#endif

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
