/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvCallback.hpp

#ifndef HKV_CALLBACK_HPP_INCLUDED
#define HKV_CALLBACK_HPP_INCLUDED

#include <Common/Base/Container/Array/hkArray.h>

class hkvCallback;
class hkvCallbackData;

/// \brief
///   Interface that receives a callback function notification
/// 
/// This interface has only one overridable: handleCallback.
/// 
/// This function gets called by all callbacks that this handler is registered to.
/// 
/// A convenient way to register a handler to a callback is using the += operator, see example.
/// 
/// In the destructor, a handler has to de-register from all callbacks again.
/// 
/// A handler can be registered to multiple callbacks at the same time. To distinguish the callback
/// in the handleCallback function, the m_pSender member of the data object can be used.
/// 
/// Callbacks can also be used for other notifications, e.g. events in a GUI system.
/// 
/// \sa class hkvCallback
/// \sa hkvCallback::registerCallback
class hkvCallbackHandler
{
public:
  ASSETFRAMEWORK_IMPEXP hkvCallbackHandler();
  ASSETFRAMEWORK_IMPEXP virtual ~hkvCallbackHandler();

  /// \brief
  ///   Overridable function that gets called by all callbacks that this handler is registered to
  /// 
  /// A handler can be registered via VCallback::RegisterCallback or the += operator.
  /// 
  /// \param data
  ///   Callback data object that is sent via hkvCallback::triggerCallbacks. Some callbacks only send
  ///   the base data object. For custom callback implementations it might however be useful to
  ///   derive a custom data object class to access more information (e.g. for a GUI button
  ///   callback).
  virtual void handleCallback(hkvCallbackData* data) = 0;

private:
  friend class hkvCallback;
#ifdef _DEBUG
  int m_debugRegisteredCount; ///< a simple reference counter that counts the number of registered callbacks so we can assert in the destructor
#endif
};



/// \brief
///   An instance of hkvCallback triggers the handleCallback function on a list of registered
///   callback handlers.
/// 
/// Callback handlers (represented by the hkvCallbackHandler interface) can be registered or
/// unregistered from a callback instance. A callback event is triggered by a call to
/// triggerCallbacks.
/// 
/// \sa class hkvCallbackHandler
class hkvCallback
{
public:

  /// \brief
  ///   Constructor
  ASSETFRAMEWORK_IMPEXP hkvCallback();

  /// @name Registering/De-registering of Callback Handlers
  /// @{

  /// \brief
  ///   Registers a callback handler, i.e. adds it to the list of handlers to be notified when the
  ///   event is triggered
  /// 
  /// \param handler
  ///   A callback handler to register
  /// 
  /// \return
  ///   true if the handler could be registered, false if not (e.g., if it is already
  ///   registered)
  /// 
  /// \sa hkvCallback::deregisterCallback
  /// \sa hkvCallback::isRegistered
  /// \sa hkvCallback::triggerCallbacks
  /// \sa class hkvCallbackHandler
  ASSETFRAMEWORK_IMPEXP hkBool registerCallback(hkvCallbackHandler* handler);

  /// \brief
  ///   De-registers a callback handler
  /// 
  /// \param handler
  ///   A callback handler to de-register from this callback
  /// 
  /// \return
  ///   true, if the handler could be de-registered, false if not (e.g. if it wasn't
  ///   registered)
  /// 
  /// \sa hkvCallback::registerCallback
  /// \sa hkvCallback::triggerCallbacks
  /// \sa class hkvCallbackHandler
  ASSETFRAMEWORK_IMPEXP hkBool deregisterCallback(hkvCallbackHandler* handler);

  /// \brief
  ///   Indicates whether the passed handler is registered to this callback
  /// 
  /// \param handler
  ///   A handler to test
  /// 
  /// \return
  ///   true if pHandler is registered in this callback
  /// 
  /// \sa hkvCallback::registerCallback
  /// \sa hkvCallback::triggerCallbacks
  /// \sa class hkvCallbackHandler
  ASSETFRAMEWORK_IMPEXP hkBool isRegistered(hkvCallbackHandler* handler) const;

  /// \brief
  ///   Operator to register a callback. Same as registerCallback(handler).
  inline hkvCallback& operator += (hkvCallbackHandler& handler)
  {
    registerCallback(&handler);
    return *this;
  }

  /// \brief
  ///   Operator to register a callback. Same as registerCallback(handler).
  inline hkvCallback* operator += (hkvCallbackHandler* handler)
  {
    registerCallback(handler);
    return this;
  }


  /// \brief
  ///   Operator to de-register a callback. Same as deregisterCallback(pHandler).
  inline hkvCallback& operator -= (hkvCallbackHandler& handler)
  {
    deregisterCallback(&handler);
    return *this;
  }

  /// \brief
  ///   Operator to de-register a callback. Same as deregisterCallback(pHandler).
  inline hkvCallback* operator -= (hkvCallbackHandler* pHandler)
  {
    deregisterCallback(pHandler);
    return this;
  }

  /// \brief
  ///   Indicates whether any callback is registered
  inline bool hasCallbacks() const
  {
    return !m_handlers.isEmpty();
  }

  /// @}

  /// @name Trigger
  /// @{

  /// \brief
  ///   Causes this callback to call the hkvCallbackHandler::handleCallback on all registered
  ///   callback handlers
  /// 
  /// The trigger loop can be aborted from outside using the breakTriggerLoop function 
  /// on the callback.
  /// 
  /// \param data
  ///   The data object to send to all handlers. Must not be NULL.
  /// 
  /// \sa hkvCallback::RegisterCallback
  /// \sa hkvCallback::DeregisterCallback
  /// \sa class hkvCallbackHandler
  /// \sa hkvCallbackHandler::handleCallback
  ASSETFRAMEWORK_IMPEXP void triggerCallbacks(hkvCallbackData* data) const;

  /// \brief
  ///   This function can be called from inside a callback handler to abort subsequent calls of
  ///   handleCallback for remaining listeners in the list
  /// 
  /// This way the callback can be aborted without de-registering the remaining items.
  /// 
  /// Calling this function outside the trigger loop does not have any effect.
  inline void breakTriggerLoop() const
  {
    m_triggerIterator = m_handlers.getSize();
  }

  /// @}

private:
  mutable int m_triggerIterator;
  hkArray<hkvCallbackHandler*> m_handlers;
};


/// \brief
///   Base callback data object that is passed to the hkvCallbackHandler::handleCallback
///   function
/// 
/// For custom callbacks, a class can be derived from this base class to add custom data to it.
/// 
/// \sa class hkvCallbackHandler
/// \sa hkvCallbackHandler::handleCallback
/// \sa hkvCallback::triggerCallbacks
/// \sa class hkvCallback
class hkvCallbackData
{
public:
  /// \brief
  ///   Constructor that takes the sender callback instance.
  inline hkvCallbackData(const hkvCallback* sender) 
  {
    m_sender = sender;
  }

  /// \brief
  ///   Destructor.
  virtual ~hkvCallbackData()
  {
  }

  /// \brief
  ///   Returns the sender stored in this callback data.
  inline const hkvCallback* getSender() const
  {
    return m_sender;
  }

  /// \brief
  ///   Calls triggerCallbacks on the sender member and passes this data object
  inline void trigger()
  {
    VASSERT(m_sender!=NULL);
    m_sender->triggerCallbacks(this);
  }

private:
  /// \brief
  ///   the sender callback instance.
  const hkvCallback* m_sender;
};


/// \brief
///   Wrapper class for an hkvCallback. Allocates the callback on the first use and de-allocates it once no more hkvCallbackHandler are mapped.
///   Use this instead of a static hkvCallback to avoid errors due to uninitialized havok memory system, as hkvCallback uses a hkArray internally.
class hkvCallbackWrapper
{
public:
  hkvCallbackWrapper() : m_callback(NULL) {}

  ~hkvCallbackWrapper()
  {
    VASSERT_MSG(m_callback == NULL, "~hkvCallbackWrapper: the hkvCallback is still present, thus it still has mapped callbacks! Remove them before destroying this class.");
  }

  /// \brief
  ///   Operator to register a callback. Passes to handler to the encapsulated hkvCallback.
  inline void operator += (hkvCallbackHandler& handler)
  {
    *this += &handler;
  }

  /// \brief
  ///   Operator to register a callback. Passes to handler to the encapsulated hkvCallback.
  inline void operator += (hkvCallbackHandler* handler)
  {
    if (m_callback == NULL)
      m_callback = new hkvCallback();

    m_callback->registerCallback(handler);
  }

  /// \brief
  ///   Operator to de-register a callback. Passes to handler to the encapsulated hkvCallback.
  inline void operator -= (hkvCallbackHandler& handler)
  {
    *this -= &handler;
  }

  /// \brief
  ///   Operator to de-register a callback. Passes to handler to the encapsulated hkvCallback.
  inline void operator -= (hkvCallbackHandler* pHandler)
  {
    if (m_callback == NULL)
      return;

    m_callback->deregisterCallback(pHandler);
    
    if (!m_callback->hasCallbacks())
    {
      delete m_callback;
      m_callback = NULL;
    }
  }

  /// \brief
  ///   Same as the GetCallback function.
  inline operator hkvCallback*() {return m_callback;}

  /// \brief
  ///   Returns the encapsulated hkvCallback.
  inline const hkvCallback* GetCallback() const
  {
    return m_callback;
  }

private:
  hkvCallback* m_callback;
};


#endif //HKV_CALLBACK_HPP_INCLUDED

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
