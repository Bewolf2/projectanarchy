/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCallbacks.hpp

#ifndef VCALLBACKS_HPP_INCLUDED
#define VCALLBACKS_HPP_INCLUDED


#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>

class VCallback;
class VisCallback_cl;
class IVisCallbackDataObject_cl;
class VMemoryStream;
class VMemoryOutStream;

/// \brief
///   Interface that receives a callback function notification
/// 
/// The virtual OnHandleCallback method gets called when any one of the callbacks this handler
/// is registered for is triggered.
///
/// Since a handler can be registered to multiple callbacks at the same time, the implementation
/// of the OnHandleCallback method has to find out which callback was actually triggered. For this
/// purpose, the m_pSender member of the callback data object can be evaluated.
/// 
/// A convenient way to register a handler to a callback is using the += operator, see example.
/// 
/// Callback handlers always have to be de-registered from all callbacks before they are destroyed.
/// 
/// The engine provides some commonly used global callbacks via Vision::Callbacks (see
/// VisCallbackManager_cl class)
/// 
/// For instance, to update a class instance once per game tick, the class can implement the
/// OnHandleCallback function and  register itself to the global
/// Vision::Callbacks.OnUpdateSceneBegin function.
/// 
/// Callbacks can also be used for other notifications, e.g. OnButtonPressed events in a GUI
/// system.
/// 
/// \sa class VCallback
/// \sa VCallback::RegisterCallback
/// \sa class VisCallbackManager_cl
/// \sa IVisCallbackDataObject_cl
/// 
/// \example
///   \code
///   MyClass::MyClass() {Vision::Callbacks.OnUpdateSceneBegin += this;} // register to the callback in the constructor
///   \endcode
class IVisCallbackHandler_cl
{
public:
  VBASE_IMPEXP IVisCallbackHandler_cl();
  VBASE_IMPEXP virtual ~IVisCallbackHandler_cl();

  /// \brief
  ///   Overridable function that gets called by all callbacks that this handler is registered to
  /// 
  /// A handler can be registered via VCallback::RegisterCallback or the += operator.
  /// 
  /// \param pData
  ///   Callback data object that is sent via VCallback::TriggerCallbacks. Some callbacks only send
  ///   the base data object. For custom callback implementations it might however be useful to
  ///   derive a custom data object class to access more information (e.g. for a GUI button
  ///   callback).
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) = 0;

  /// \brief
  ///   This function supplies the sorting key for the calling order. Handlers are called in ascending
  ///   key order.
  /// 
  /// This function is called when this handler is added to a callback as a listener. For comparison
  /// this function is also called for all handlers that are already in the list (the key values
  /// are not cached).
  /// 
  /// This function should return a value that is constant for all calls of the same callback. 
  /// If however the order should be changed during runtime, the callback list must be sorted manually 
  /// using VCallback::SortUp.
  /// 
  /// \param pCallback
  ///   The current callback instance that performs sorting.
  /// 
  /// \return
  ///   an arbitrary sorting key value. The value can be positive or negative. The
  ///   default return value is 0.
  virtual int GetCallbackSortingKey(VCallback *pCallback)
  {
    return 0;
  }

private:
  friend class VCallback;
#ifdef HK_DEBUG
  int m_iDebugRegisteredCount; ///< a simple reference counter that counts the number of registered callbacks so we can assert in the destructor
#endif
};



/// \brief
///   Manages callback handlers and triggers their callback functions.
/// 
/// Callback handlers (represented by the IVisCallbackHandler_cl interface) can be registered or
/// unregistered from a VCallback instance.
/// 
/// A callback event can be triggered either by a call of TriggerCallbacks or 
/// by IVisCallbackDataObject_cl::Trigger.
/// 
/// The engine callback manager (accessible via Vision::Callbacks) provides some built-in
/// callbacks, such as  per-tick events or on new scene loaded. See VisCallbackManager_cl.
/// 
/// The list of registered callback handlers remains sorted in ascending order determined by the
/// return value of the overridable IVisCallbackHandler_cl::GetCallbackSortingKey function.
/// 
/// \sa class VisCallbackManager_cl
/// \sa class IVisCallbackHandler_cl
class VCallback : private DynArray_cl<IVisCallbackHandler_cl *>
{
public:

  /// \brief
  ///   Constructor; initializes this callback object.
  /// \param iCallbackID
  ///   optional id value; can be used to identify this callback when it is held in a collection
  /// \param pSerializationObject
  ///   optional default data object
  VBASE_IMPEXP VCallback(int iCallbackID=0, IVisCallbackDataObject_cl *pSerializationObject=NULL);

  ///
  /// @name Register/de-register Listeners
  /// @{
  ///

  /// \brief
  ///   Registers a callback handler, i.e. adds it to the list of handlers to be notified when the
  ///   event is triggered
  /// 
  /// \param pHandler
  ///   A callback handler to register
  /// 
  /// \return
  ///   \c TRUE if the handler could be registered, \c FALSE if not (e.g. it is already
  ///   registered)
  /// 
  /// \sa VCallback::DeregisterCallback
  /// \sa VCallback::IsRegistered
  /// \sa VCallback::TriggerCallbacks
  /// \sa class IVisCallbackHandler_cl
  /// 
  /// \example
  ///   \code
  ///   Vision::Callbacks.OnWorldInit.RegisterCallback(&myNewSceneNotification);
  ///   \endcode
  VBASE_IMPEXP BOOL RegisterCallback(IVisCallbackHandler_cl *pHandler);

  /// \brief
  ///   Unregisters a callback handler
  /// 
  /// \param pHandler
  ///   A callback handler to unregister from this callback
  /// 
  /// \return
  ///   \c TRUE if the handler could be unregistered, \c FALSE if not (e.g. if it wasn't
  ///   registered)
  /// 
  /// \sa VCallback::RegisterCallback
  /// \sa VCallback::TriggerCallbacks
  /// \sa class IVisCallbackHandler_cl
  VBASE_IMPEXP BOOL DeregisterCallback(IVisCallbackHandler_cl *pHandler);

  /// \brief
  ///   Indicates whether the passed handler is registered to this callback
  /// 
  /// \param pHandler
  ///   A handler to test
  /// 
  /// \return
  ///   \c TRUE if handler is registered in this callback; \c FALSE if not
  /// 
  /// \sa VCallback::RegisterCallback
  /// \sa VCallback::TriggerCallbacks
  /// \sa class IVisCallbackHandler_cl
  inline BOOL IsRegistered(IVisCallbackHandler_cl *pHandler) {VASSERT(pHandler);return GetElementPos(pHandler)>=0;}

  /// \brief
  ///   Operator to register a callback. Same as RegisterCallback(pHandler).
  inline VCallback& operator += (IVisCallbackHandler_cl &pHandler)
  {
    RegisterCallback(&pHandler);
    return *this;
  }

  /// \brief
  ///   Operator to register a callback. Same as RegisterCallback(pHandler).
  inline VCallback* operator += (IVisCallbackHandler_cl *pHandler)
  {
    RegisterCallback(pHandler);
    return this;
  }


  /// \brief
  ///   Operator to deregister a callback. Same as DeregisterCallback(pHandler).
  inline VCallback& operator -= (IVisCallbackHandler_cl &pHandler)
  {
    DeregisterCallback(&pHandler);
    return *this;
  }

  /// \brief
  ///   Operator to deregister a callback. Same as DeregisterCallback(pHandler).
  inline VCallback* operator -= (IVisCallbackHandler_cl *pHandler)
  {
    DeregisterCallback(pHandler);
    return this;
  }


  /// \brief
  ///   Indicates whether any callback is registered
  inline bool HasCallbacks() const
  {
    return m_iCallbackCount>0;
  }


  ///
  /// @}
  ///

  ///
  /// @name Trigger
  /// @{
  ///

  /// \brief
  ///   Causes the callback to call the IVisCallbackHandler_cl::OnHandleCallback on all registered
  ///   handlers
  /// 
  /// The trigger function can be called by custom code. However, for the callbacks provided in the
  /// VisCallbackManager_cl (accessible via Vision::Callbacks) this should not be done.
  /// 
  /// The calling order for all registered handlers is determined by the value provided by the
  /// overridable IVisCallbackHandler_cl::GetCallbackSortingKey function, or, if not overridden, by
  /// the order the handlers are registered.
  /// 
  /// The trigger loop can be aborted from outside using the Break function on the callback.
  /// 
  /// \param pData
  ///   The data object to send to all handlers. If NULL, this function creates a base object with
  ///   only the sender defined.
  /// 
  /// \sa VCallback::RegisterCallback
  /// \sa VCallback::DeregisterCallback
  /// \sa class IVisCallbackHandler_cl
  /// \sa IVisCallbackHandler_cl::OnHandleCallback
  VBASE_IMPEXP void TriggerCallbacks(IVisCallbackDataObject_cl *pData=NULL);

  /// \brief
  ///   This function can be called from inside a callback handler to abort subsequent calls of
  ///   OnHandleCallback for remaining listeners in the list
  /// 
  /// This way the callback can be aborted without de-registering the remaining items.
  /// 
  /// Calling this function outside the trigger loop does not have any effect.
  inline void Break()
  {
    m_iTriggerIterator = m_iCallbackCount;
  }



  ///
  /// @}
  ///

  ///
  /// @name Sorting
  /// @{
  ///

  /// \brief
  ///   Manually sorts the handler list in ascending order using the
  ///   IVisCallbackHandler_cl::GetCallbackSortingKey function
  VBASE_IMPEXP void SortUp();

  ///
  /// @}
  ///

private:
  VBASE_IMPEXP BOOL RemoveAt(int iIndex);
  VBASE_IMPEXP void InsertAt(IVisCallbackHandler_cl *pHandler, int iIndex);
  int m_iCallbackCount;
  int m_iTriggerIterator;
public:
  int m_iProfilingID; ///< ID of the profiling counter to use for profiling this callback (not used in this base class)
  int m_iCallbackID; ///< Used to identify a callback in a list of callbacks
  IVisCallbackDataObject_cl *m_pSerializationObject; ///< Default data object for callbacks (not used in this base class)
};


/// \brief
///   Base callback data object that is passed to the IVisCallbackHandler_cl::OnHandleCallback
///   function
/// 
/// For custom callbacks requiring additional information to be transmitted, a class can be 
/// derived from this base class to add custom data to it.
/// 
/// \sa class IVisCallbackHandler_cl
/// \sa IVisCallbackHandler_cl::OnHandleCallback
/// \sa VCallback::TriggerCallbacks
/// \sa class VisCallback_cl
class IVisCallbackDataObject_cl
{
public:
  /// \brief
  ///   Constructor that takes the sender callback instance.
  /// \param pSender
  ///   the sender instance, default \c NULL. If a sender instance is specified,
  ///   the callback can be triggered with the Trigger() method of this class.
  inline IVisCallbackDataObject_cl(VCallback *pSender=NULL) 
  {
    m_pSender = pSender;
  }

  /// \brief
  ///   Destructor.
  virtual ~IVisCallbackDataObject_cl() 
  {
  }

  /// \brief
  ///   Serializes this data object.
  /// \param ar
  ///   the archive object to write to or read from.
  VBASE_IMPEXP virtual void Serialize(VArchive &ar);

  /// \brief
  ///   public pointer to the sender callback instance. Should be set for all callbacks.
  VCallback *m_pSender;

  /// \brief
  ///   Calls TriggerCallbacks on the sender member and passes this data object.
  /// \note
  ///   Can only be called successfully if a valid sender has been passed when constructing 
  ///   this data object. If this isn't the case, an assertion will be raised and the callback
  ///   will not be triggered.
  inline void Trigger()
  {
    VASSERT(m_pSender!=NULL);
    if (m_pSender)
      m_pSender->TriggerCallbacks(this);
  }
};


/// \brief
///   Data object that is sent by callbacks that need to pass a pointer to a VTypedObject
class VTypedObjectCallbackData : public IVisCallbackDataObject_cl
{
public:
  inline VTypedObjectCallbackData(VCallback *pSender, VTypedObject *pObj) : IVisCallbackDataObject_cl(pSender)
  {
    m_pObject = pObj;
  }
  VTypedObject *m_pObject; ///< the actual pointer to the object
};


/// \brief
///   Provides functionality for recording callback events and for playing them back
///   at a later time or in a different context.
class VCallbackSenderQueue
{
public:
  /// \brief
  ///   Constructor; initializes the callback queue.
  /// \param bByID
  ///   selects the mode in which callbacks are referenced in the recorded callback events. 
  ///   - If \c true, callbacks are referenced by their ID. This way, events can be played back
  ///     in a different context than the one in which they have been recorded, provided that
  ///     a callback with the same ID can be found.
  ///   - If \c false, callbacks are referenced directly by pointer. This is more performant than
  ///     referencing by ID, but the callbacks can only be played back in the context they have been
  ///     recorded, as the pointers need to remain valid.
  VBASE_IMPEXP VCallbackSenderQueue(bool bByID=true);

  /// \brief
  ///   Destructor.
  VBASE_IMPEXP ~VCallbackSenderQueue();

  /// \brief
  ///   Clears all currently recorded callback events.
  VBASE_IMPEXP void ResetQueue();

  /// \brief
  ///   Records the occurrence of a callback event in the internal queue.
  /// \param data
  ///   the data object of the callback event
  /// \note
  ///   The callback is identified by the m_pSender field of the data object, thus the data object
  ///   must have a valid sender!
  VBASE_IMPEXP void Trigger(IVisCallbackDataObject_cl &data);

  /// \brief
  ///   Plays back the callback events recorded in the passed archive object.
  /// \note
  ///   The playback of callback events is stopped when invalid data is read from the archive
  ///   or a callback referenced in the archive cannot be found. Any subsequent events in the
  ///   archive will thus be ignored.
  /// \param ar
  ///   an archive object containing recorded callback events
  /// \return
  ///   the number of events that have successfully been played back
  VBASE_IMPEXP int ProcessQueue(VArchive &ar);

  /// \brief
  ///   Returns the data stream containing callback events recorded by this callback queue.
  /// \return
  ///   the memory stream containing the recorded callback events, or \c NULL, if no events have
  ///   yet been recorded
  inline VMemoryStream* GetDataStream()
  {
    if (m_pArchive)
      m_pArchive->Flush();
    return m_spMemStream;
  }

  /// \brief
  ///   Registers a callback to be available for playback of recorded events.
  /// \param pCallback
  ///   the callback to register. If the callback is already registered, this call has
  ///   no effect.
  inline void RegisterCallbackFunction(VCallback *pCallback)
  {
    m_Callbacks.AddUnique(pCallback);
  }

  /// \brief
  ///   De-registers a callback that has been registered before with RegisterCallbackFunction.
  /// \param pCallback
  ///   the callback to de-register. If \c pCallback has not been registered before, this call
  ///   has no effect.
  inline void DeregisterCallbackFunction(VCallback *pCallback)
  {
    int iIndex = m_Callbacks.Find(pCallback);
    if (iIndex>=0)
      m_Callbacks.RemoveAt(iIndex);
  }

  /// \brief
  ///   Finds a registered callback by its ID.
  /// \param iID
  ///   the ID of the callback to find
  /// \return
  ///   the callback, or \c NULL if no callback with the passed ID is currently registered
  VBASE_IMPEXP VCallback* FindCallbackByID(int iID);

protected:
  VPList m_Callbacks; ///< List of registered callbacks
  VMutex m_TriggerMutex; ///< Mutex to protect against concurrent access to critical data
  VArchive *m_pArchive; ///< Archive to record callback events
  VSmartPtr<VMemoryStream> m_spMemStream; ///< Memory stream holding recorded callbacks
  VMemoryOutStream *m_pOutStream; ///< Wrapper around m_spMemStream for writing
  int m_iQueueLength; ///< Number of callback events currently recorded
  bool m_bByID; ///< Whether recorded callbacks are referenced by ID or by pointer
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
