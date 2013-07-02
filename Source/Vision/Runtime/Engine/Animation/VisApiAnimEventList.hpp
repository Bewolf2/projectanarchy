/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimEventList.hpp

#ifndef VIS_ANIM_EVENT_LIST_HPP_INCLUDED
#define VIS_ANIM_EVENT_LIST_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimSequence.hpp>

/// \brief
///   Helper class to keep a time/ID pair that represents a single event
/// 
/// All properties are public to allow direct access.
class VisAnimEvent_cl
{
public:

  /// \brief
  ///   Constructor of the event class.
  /// 
  /// \param iID
  ///   integer id identifying the event
  /// 
  /// \param fTime
  ///   point in time the event occurs
  ///
  /// \param _bAutoRemove
  ///   Should the event automatically removed after execution
  inline VisAnimEvent_cl(int iID, float fTime, bool _bAutoRemove=false)
  {
    iEventID=iID;
    fTimeValue=fTime;
    bAutoRemove = _bAutoRemove;
  }
  

  /// \brief
  ///   Constructor of the event class.
  /// 
  /// Gets the id from the string with Vision::Animations.RegisterEvent.
  /// 
  /// \param szEvent
  ///   String describing the event
  /// 
  /// \param fTime
  ///   point in time the event occurs
  ///
  /// \param _bAutoRemove
  ///   Should the event automatically removed after execution
  VISION_APIFUNC VisAnimEvent_cl(const char *szEvent, float fTime, bool _bAutoRemove=false);
  

  /// \brief
  ///   Empty constructor of the event class.
  inline VisAnimEvent_cl()
  { 
    iEventID=0;
    fTimeValue=0.f;
    bAutoRemove=false;
  }


  /// \brief
  ///   Returns the string that was registered for this event ID.
  /// 
  /// The ID must have been registered via Vision::Animations.RegisterEvent, otherwise this
  /// functions asserts. See IsStringEvent.
  /// 
  /// \return
  ///   const char: String describing the event.
  VISION_APIFUNC const char *GetEventString() const;


  /// \brief
  ///   Indicates whether the event ID represents a string or absolute ID; wraps around
  ///   Vision::Animations.IsStringEvent.
  VISION_APIFUNC bool IsStringEvent() const;

  // public members for direct access
  float fTimeValue;
  int   iEventID;
  bool  bAutoRemove;

protected:
  // serialization
  V_DECLARE_SERIALX( VisAnimEvent_cl, VISION_APIFUNC );
  VISION_APIFUNC void SerializeX( VArchive &ar );
};


// defines
class VisAnimSequence_cl;


/// \brief
///   The VisAnimEventList_cl class keeps an array of events of type VisAnimEvent_cl in a sorted
///   list.
/// 
/// Multiple events can be exactly at the same time.
class VisAnimEventList_cl
{
public:
  
  /// \brief
  ///   Constructor of the event list class.
  VISION_APIFUNC VisAnimEventList_cl();


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisAnimEventList_cl();

    
  /// \brief
  ///   Adds a new event to the list.
  /// 
  /// There can be more than just one key at the same time and also several events with the same
  /// key.
  /// 
  /// The key is translated to an integer ID in this function. 
  /// 
  /// The event gets sorted into the list according to its time value.
  /// 
  /// \param fTime
  ///   time for new event.
  /// 
  /// \param szEventString
  ///   String that specifies the event.
  ///
  /// \param bAutoRemove
  ///   Should the event automatically removed after execution
  VISION_APIFUNC void AddEvent(float fTime, const char* szEventString, bool bAutoRemove = false);


  /// \brief
  ///   Adds a new event with the given key at the given time to the list.
  /// 
  /// If the ID represents a string event, the ID must have been generated with the
  /// VisAnimManager_cl::RegisterEvent function.
  /// 
  /// Furthermore, since archives store the string rather than the ID, for string events it is not
  /// guaranteed that the event ID remains the same after savegame loading.
  /// 
  /// It is also possible to add absolute event IDs that remain the same after savegame loading.
  /// These events don't have to be registered, but they must not use the highest integer bit. Thus
  /// negative numbers are not allowed.
  /// 
  /// There can be more than just one key at the same time and several events with the same key.
  /// 
  /// The event gets sorted into the list according to its time value.
  /// 
  /// \param fTime
  ///   time for new event.
  /// 
  /// \param iKey
  ///   int ID specifying event. Can be absolute or string ID.
  /// 
  /// \param bAutoRemove
  ///   indicates whether an event shall be removed automatically after processing
  VISION_APIFUNC void AddEvent(float fTime, int iKey, bool bAutoRemove = false);


  /// \brief
  ///   Removes a single event from the list. The event is defined by the exact time value and ID.
  /// 
  /// Since there can be several events with different keys at the same time you have to specify
  /// time and key of the event.
  /// 
  /// \param fTime
  ///   exact time of the event.
  /// 
  /// \param iKey
  ///   key ID of the event.
  /// 
  /// \return
  ///   bool bResult: true if event has been removed
  VISION_APIFUNC bool RemoveEvent(float fTime, int iKey);


  /// \brief
  ///   Removes all events with the given ID from the list. 
  /// 
  /// Since there can be several events with same keys at different time-slots you have to specify
  /// only the ID of the event, which will be removed from all assigned time-slots.
  /// 
  /// \param iKey
  ///   key ID of the event.
  /// 
  /// \return
  ///   int iNumEventsRemoved: number of removed of events
  VISION_APIFUNC int RemoveEvents(int iKey);


  /// \brief
  ///   Removes all events within a key ID range of [KeyRangeStart..KeyRangeEnd].
  /// 
  /// The whole sequence duration is searched for the key values.
  /// 
  /// \param iKeyRangeStart
  ///   start of key range (included).
  /// 
  /// \param iKeyRangeEnd
  ///   end of key range (included).
  /// 
  /// \return
  ///   int numberEvents: number of events removed
  VISION_APIFUNC int RemoveEvents(int iKeyRangeStart,int iKeyRangeEnd);


  /// \brief
  ///   Removes all events within a time interval of [fTimeRangeStart..fTimeRangeEnd]
  /// 
  /// \param fTimeRangeStart
  ///   start time (included).
  /// 
  /// \param fTimeRangeEnd
  ///   end time (included).
  /// 
  /// \return
  ///   int numberEvents: removed events count
  VISION_APIFUNC int RemoveEvents(float fTimeRangeStart, float fTimeRangeEnd);

    
  /// \brief
  ///   Removes all events from the list.
  VISION_APIFUNC void RemoveAllEvents();


  /// \brief
  ///   Fills a list of time values at which the ID appears in the list.
  /// 
  /// If fTimeList is NULL, this function returns the number of occurrences.
  /// 
  /// \param iEventKey
  ///   key value to search for
  /// 
  /// \param fTimeList
  ///   list for time values. Must be large enough.
  /// 
  /// \return
  ///   int iTimecount: number of ID matches found
  VISION_APIFUNC int GetEventTimes(int iEventKey, float* fTimeList) const;


  /// \brief
  ///   Returns the time value of the first event in the event list with the passed event key.
  /// 
  /// If there is no event with the passed event key, this function returns -1.0f.
  /// 
  /// \param iEventKey
  ///   key value to search for
  /// 
  /// \return
  ///   float fTime : time value of the first event with the passed event key
  VISION_APIFUNC float GetFirstEventTime(int iEventKey) const;


  /// \brief
  ///   Fills a list with key IDs of all events that occur at exactly the specified time.
  /// 
  /// If iKeyList is NULL, this function returns the number of events at the specified time.
  /// 
  /// \param fEventTime
  ///   time value to search for events
  /// 
  /// \param iKeyList
  ///   list for key values. Must be large enough
  /// 
  /// \return
  ///   int iKeycount: number of events at the specified time
  VISION_APIFUNC int GetEventKeys(float fEventTime, int* iKeyList) const;


  /// \brief
  ///   Prepares an internal list with all events within the specified time range.
  /// 
  /// Used internally to enumerate the events in a certain time range while playing back the
  /// animation.
  /// 
  /// You can get the prepared events by calling GetNextEvent.
  /// 
  /// \param fStartTimeValue
  ///   search start time
  /// 
  /// \param fEndTimeValue
  ///   search end time
  ///
  /// \param bLooped
  ///   Restart at the beginning of the list after reaching the end
  VISION_APIFUNC void PrepareCurrentEventSet(float fStartTimeValue, float fEndTimeValue, bool bLooped=true);


  /// \brief
  ///   Gets a pointer to the next event from the list. The list has to be prepared via
  ///   PrepareCurrentEventSet.
  /// 
  /// GetNextEvent can be called multiple times until it returns NULL when the list is empty.
  /// 
  /// \return
  ///   VisAnimEvent_cl* pEvent: next event or NULL
  VISION_APIFUNC VisAnimEvent_cl* GetNextEvent();


  /// \brief
  ///   Calculates the memory the event list consumes.
  /// 
  /// \return
  ///   int: size in memory
  VISION_APIFUNC int CalcMemSize(void) const;


  /// \brief
  ///   Returns the number of events in this list.
  /// 
  /// \return
  ///   int iCount: number of events
  inline int GetEventCount() const
  {
    return m_iEventCount;
  }


  /// \brief
  ///   Returns the event ID value for an event index.
  /// 
  /// \param iIndex
  ///   Event index in list. Must be in valid range [0..GetEventCount()-1]
  /// 
  /// \return
  ///   int iID: event ID
  inline int GetEventValueByIndex(int iIndex) const
  {
    return m_iEventID.GetDataPtr()[iIndex];
  }


  /// \brief
  ///   Returns the event time for an event index.
  /// 
  /// \param iIndex
  ///   Event index in the list. Must be in valid range [0..GetEventCount()-1]
  /// 
  /// \return
  ///   float fTime: event time
  inline float GetEventTimeByIndex(int iIndex) const
  {
    return m_fEventTime.GetDataPtr()[iIndex];
  }


  /// \brief
  ///   Sets the owner sequence in case this event list is owned by an animation sequence. This function sets the sequence length
  /// 
  /// \param pAnimSequence
  ///   owner sequence
  VISION_APIFUNC void SetAnimSequence(const VisAnimSequence_cl* pAnimSequence);

  /// \brief
  ///   Sets the length (in seconds) of the sequence in case the list is not owned by an anim sequence
  inline void SetSequenceLength(float fTime)
  {
    VASSERT(fTime>0);
    m_fSequenceLength = fTime;
  }

  /// \brief
  ///   Returns the length of the sequence in seconds
  VISION_APIFUNC float GetSequenceLength() const;

private:
  float m_fStartTimeValue;            ///< start time of prepared events
  float m_fEndTimeValue;              ///< end time of prepared events
  float m_fSequenceLength;            /// length of animation sequence
  const VisAnimSequence_cl *m_pAnimSequence;// animation sequence event list was created for
  bool  m_bDirection;                 ///< true means forward, false means backward

  // events
  int m_iEventCount;
  DynArray_cl<int>    m_iEventID;     ///< sorted list of Event IDs
  DynArray_cl<float>  m_fEventTime; ///< sorted list of Event Times
  DynArray_cl<bool>   m_bEventAutoRemove; ///<sorted list of Events to be removed after execution
  int m_iCurrentEvent;            ///< current list index, for next prepared event
  int m_iCurrentLoop;
  bool m_bLooped;                 ///< Whether the currently prepared event set loops around
  VisAnimEvent_cl m_CurrentEvent; ///< keeps the next prepared event

  // serialization
  V_DECLARE_SERIALX( VisAnimEventList_cl, VISION_APIFUNC );
  VISION_APIFUNC void SerializeX( VArchive &ar );
};


// IVAnimationEventTrigger Serialization Versions
#define ANIMATIONEVENTTRIGGER_VERSION_0          0     // Initial version
#define ANIMATIONEVENTTRIGGER_VERSION_CURRENT    0     // Current version

///
/// \brief
///   Struct that represents information about one animation event trigger.
///
struct VEventTriggerInfo_t
{
public:

  VEventTriggerInfo_t() 
  {
    m_pSequence = NULL;
    m_iEventID = 0;
  }

  virtual ~VEventTriggerInfo_t() { }

  VisAnimSequence_cl* m_pSequence;       ///< Animation sequence to which the effect should be triggered 
  int m_iEventID;                        ///< ID of the animation event that triggers the effect
};

/// 
/// \brief
///   Component that listens to animation events of the owner entity and invokes a trigger.
/// 
///   This component registers itself as an event listener to the root animation control of
///   the parent owner entity. It is possible to specify the animation sequence and the
///   animation event that should cause the trigger, either by setting the members of the
///   component directly or by providing a list with the respective event trigger information.
/// 
///   This class is not used anywhere in the engine directly but in derived classes in the
///   VisionEnginePlugin.
/// 
/// \note
///   Please note that this animation event trigger component currently only supports 
///   the predefined animation control and the animation mixer nodes in the engine: 
///   VisAnimControl_cl, VisAnimLayerMixerNode_cl and VisAnimNormalizeMixerNode_cl.
///   In order to support custom mixer nodes you will need to customize this component 
///   accordingly.
class IVAnimationEventTrigger : public IVObjectComponent, IVisCallbackHandler_cl
{
public: 

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor
  VISION_APIFUNC IVAnimationEventTrigger(int iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~IVAnimationEventTrigger();

  ///
  /// @}
  ///

  ///
  /// @name VTypedObject Virtual Overrides
  /// @{
  ///

  #if defined(WIN32) || defined(_VISION_DOC)

  /// \brief
  ///   Overridable that is called by the editor to retrieve per-instance variable display hints.
  VISION_APIFUNC virtual void GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo) HKV_OVERRIDE;

  #endif

  /// \brief
  ///   Virtual function that is called whenever a message is sent to this object
  ///
  /// \param iID
  ///   The ID constant of the message.
  /// 
  /// \param iParamA
  ///   message data value
  /// 
  /// \param iParamB
  ///   message data value
  VISION_APIFUNC virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes
  VISION_APIFUNC virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overridden function. Blob shadows can be attached to VisObject3D_cl instances
  VISION_APIFUNC virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief
  ///   Overridden function to respond to variable changes
  VISION_APIFUNC virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(IVAnimationEventTrigger, VISION_APIDATA)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(IVAnimationEventTrigger, VISION_APIDATA)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   Binary archive
  VISION_APIFUNC virtual void Serialize( VArchive &ar );

  ///
  /// @}
  ///

  ///
  /// @name IVisCallbackHandler_cl Overrides
  /// @{
  ///

  /// \brief
  ///   Callback method that takes care of updating the managed instances each frame
  VISION_APIFUNC virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  ///
  /// @}
  ///

  ///
  /// @name Assigning info list
  /// @{
  ///

  /// \brief
  ///   Set the info list that contains an array of pointers containing information for 
  ///   multiple animation event triggers.
  ///
  /// \param pInfoList
  ///   List of VEventTriggerInfo_t pointers
  /// 
  /// \param iInfoCount
  ///   Number of pointers
  VISION_APIFUNC void SetEventTriggerInfoList(VEventTriggerInfo_t *pInfoList, int iInfoCount);

  ///
  /// @}
  ///

protected:

  /// \brief
  ///   Initializes the component.
  VISION_APIFUNC virtual bool CommonInit();

  /// \brief
  ///   Overridable that can be implemented to respond to animation events. The base implementation is empty.
  VISION_APIFUNC virtual void OnAnimationEvent() {}

  /// \brief
  ///   Sets the event id and sequence for the VEventTriggerInfo_t object
  VISION_APIFUNC bool GetEventTriggerInfoBaseData(VEventTriggerInfo_t* pInfo);

  // Exposed to vForge:
  VString AnimationName;                          ///< Name of the animation to which the effect should be triggered 
  VString TriggerEvent;                           ///< Name of the animation event that triggers the effect

  // Not exposed to vForge:
  VEventTriggerInfo_t* m_pEventTriggerInfoList;   ///< List of event trigger info pointers
  VEventTriggerInfo_t* m_pActiveTriggerInfo;      ///< Active event trigger info 
  int m_iEventTriggerInfoCount;                   ///< Number of event triggers
};

#endif //VIS_ANIM_EVENT_LIST_HPP_INCLUDED

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
