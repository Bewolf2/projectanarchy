/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimEventListener.hpp

#ifndef VIS_ANIM_EVENT_LISTENER_HPP_INCLUDED
#define VIS_ANIM_EVENT_LISTENER_HPP_INCLUDED


/// \brief
///   Base class that supports the processing of animation events.
/// 
/// Use the event handler functionality to register your own classes as listeners for events.
/// VisAnimControl_cl and IVisAnimMixerNode_cl derive from this class in order to process
/// animation events. The animation control invokes animation events, either from the actual
/// animation sequence or custom ones that have been added manually. The mixer node supports 
/// the processing of these events through the animation tree hierarchy.
class VisAnimEventListener_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor
  VISION_APIFUNC VisAnimEventListener_cl();


  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisAnimEventListener_cl();

  ///
  /// @}
  ///

  ///
  /// @name Events
  /// @{
  ///

  /// \brief
  ///   Adds a listener to this instance that will receive events via the MessageFunction.
  /// 
  /// A message will be sent with iID=VIS_MSG_EVENT, iParamA=pEventEntry->iEventID and
  /// iParamB=(VisAnimControl_cl*) to all listeners.
  /// 
  /// \param pObj
  ///   base object to be added, for instance an entity
  /// 
  /// \return
  ///   int iIndex : The index of the listener in the list. Do not cache this index, because
  ///   removing listeners changes the indexing.
  /// 
  /// \sa VisAnimEventList_cl;
  VISION_APIFUNC int AddEventListener(VisTypedEngineObject_cl* pObj);


  /// \brief
  ///   Adds a listener to this instance that will receive events via the MessageFunction.
  ///   If the object is already in the list it won't be added again.
  /// 
  /// A message will be sent with iID=VIS_MSG_EVENT, iParamA=pEventEntry->iEventID and
  /// iParamB=(VisAnimControl_cl*) to all listeners.
  /// 
  /// \param pObj
  ///   base object to be added, for instance an entity
  /// 
  /// \return
  ///   int iIndex : The index of the listener in the list. Do not cache this index, because
  ///   removing listeners changes the indexing.
  /// 
  /// \sa VisAnimEventList_cl;
  VISION_APIFUNC int AddEventListenerUnique(VisTypedEngineObject_cl* pObj);


  /// \brief
  ///   RemoveEventListener removes the specified listener from list.
  /// 
  /// \param pObj
  ///   Listener to be removed
  /// 
  /// \return
  ///   bool bResult: true if the listener could be removed.
  VISION_APIFUNC bool RemoveEventListener(VisTypedEngineObject_cl* pObj);


  /// \brief
  ///   Returns a pointer to a registered listener.
  /// 
  /// The index must be in valid range [0..GetEventListenerCount()-1].
  /// 
  /// \param iIndex
  ///   index of listener in list.
  /// 
  /// \return
  ///   VisTypedEngineObject_cl* listener: The listener with index iIndex.
  inline VisTypedEngineObject_cl* GetEventListener(int iIndex) const
  {
    VASSERT((iIndex>=0)&&(iIndex<m_iEventListenerCount));
    return m_pEventListener.GetDataPtr()[iIndex];
  }


  /// \brief
  ///   Returns the number of registered event listeners.
  /// 
  /// \return
  ///   int: number of event listeners.
  inline int GetEventListenerCount() const 
  {
    return m_iEventListenerCount;
  }


  /// \brief
  ///   Remove all listeners from list.
  /// 
  /// Use it for example if you change the sequence.
  VISION_APIFUNC void RemoveAllEventListeners();


  /// \brief
  ///   Sends a message to all listeners.
  /// 
  /// Used internally for the event processing but can also be used to send custom events that are
  /// not in the event lists.
  /// 
  /// This function uses Vision::Game.PostMsg to send a message with iID=VIS_MSG_EVENT,
  /// iParamA=pEventEntry->iEventID  iParamB=(VisAnimControl_cl*) to all listeners.
  /// 
  /// VisGame_cl::PostMsg queues the messages and sends them in the next update tick.
  /// 
  /// \param pEventEntry
  ///   Event to be sent.
  VISION_APIFUNC void SendToAllListeners(const VisAnimEvent_cl* pEventEntry) const;


  /// \brief
  ///   Forwards an event message to all listeners.
  /// 
  /// The event listener can also function as an event processor where it forwards all 
  /// received events.
  /// 
  /// \param eventID
  ///   ID of the event to be forwarded.
  /// 
  /// \param sender
  ///   Pointer to actual sender of the event.
  VISION_APIFUNC void ForwardToAllListeners(const int eventID, INT_PTR sender) const;


  /// \brief
  ///   Specifies whether the listener should forward all received events from to its registered event listeners.
  /// 
  /// The base implementation does not do anything as the base class for all anim result generators 
  /// has no information about any child nodes that might process animation events. This function
  /// is overridden in the animation mixer nodes where animation events can be processed from its
  /// child nodes up the animation tree hierarchy.
  /// 
  /// \param bEnable
  ///   TRUE if the listener should forward all received events to its registered event listeners
  VISION_APIFUNC virtual void SetAnimEventsForwarding(bool bEnable)
  {
    m_bForwardAnimEvents = bEnable;
  }


  /// \brief
  ///   Determines whether the listener is forwarding all received events to its registered event listeners.
  /// 
  /// \returns
  ///   TRUE if the listener is forwarding all received events to its registered event listeners
  VISION_APIFUNC inline bool GetAnimEventsForwarding()
  {
    return m_bForwardAnimEvents;
  }

  ///
  /// @}
  ///

protected:

  int m_iEventListenerCount;                                ///< number of listeners which get notified about animation events
  DynArray_cl<VisTypedEngineObject_cl*>m_pEventListener;    ///< collection of listeners which get notified about animation events
  bool m_bForwardAnimEvents;                                ///< If TRUE the mixer node will forward all animation events of its child anim result generators (either mixer or anim control)

};

#endif //VIS_ANIM_CONTROL_HPP_INCLUDED

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
