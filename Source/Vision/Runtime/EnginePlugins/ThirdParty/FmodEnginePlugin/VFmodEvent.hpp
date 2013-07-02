/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodEvent.hpp

#ifndef VFMODEVENT_HPP_INCLUDED
#define VFMODEVENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>

/// Serialization versions
#define VFMOD_EVENT_VERSION_0         0x00000000             // Initial version
#define VFMOD_EVENT_VERSION_1         0x00000001         
#define VFMOD_EVENT_VERSION_CURRENT   VFMOD_EVENT_VERSION_1

///\brief
///  This class represents a single positionable event instance in the world.
///
/// This is the native counterpart of the vForge event shape.
/// Multiple event instances share the same event group as resource.
///
/// To position and orient the event in the scene, use the SetPosition/SetOrientation
/// member functions which are inherited from the VisObject3D_cl class.
///
///\see
///  VFmodEventGroup
class VFmodEvent : public VisObject3D_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  FMOD_IMPEXP VFmodEvent();

  FMOD_IMPEXP VFmodEvent(const char *szEventName, VFmodEventCollection *pOwner, VFmodEventGroup *pEventGroup, const hkvVec3 &vPos, int iFlags=VFMOD_FLAG_NONE);

  FMOD_IMPEXP VFmodEvent(int iEventIndex, VFmodEventCollection *pOwner, VFmodEventGroup *pEventGroup, const hkvVec3 &vPos, int iFlags=VFMOD_FLAG_NONE);

  FMOD_IMPEXP virtual ~VFmodEvent();

  /// \brief
  ///   Stops the event and removes it from the collection. This function should be called to destroy an event.
  FMOD_IMPEXP VOVERRIDE void DisposeObject();

  ///
  /// @}
  ///


  ///
  /// @name Event property functions
  /// @{
  ///

  /// \brief
  ///   Gets the event group used by this instance
  inline VFmodEventGroup* GetEventGroup() const { return m_spEventGroup; }

  /// \brief
  ///   Starts playing this event 
  /// 
  /// \param bAlsoInEditor
  ///   If true, then playback will also be started inside vForge
  ///
  /// \remarks
  ///   An event by default starts playing after creation unless it 
  ///   has been created with the VFMOD_FLAG_PAUSED flag.
  FMOD_IMPEXP void Start(bool bAlsoInEditor=false);

  /// \brief
  ///   Stops playing this event. 
  ///
  /// \param bImmediate
  ///   Set this to false if you want to make use of the "Fadeout time" property (for this the event also has to be created with the VFMOD_FLAG_NODISPOSE flag).
  ///
  FMOD_IMPEXP void Stop(bool bImmediate=true); 
  
  /// \brief
  ///   Indicates whether the event is currently playing
  FMOD_IMPEXP bool IsPlaying() const;

  /// \brief
  ///   Indicates whether the event has finished loading
  FMOD_IMPEXP bool IsReady() const;

  /// \brief
  ///   Returns the pause status of this event
  inline bool IsPaused() const {return (m_iFlags&VFMOD_FLAG_PAUSED)>0;}

  /// \brief
  ///   Pauses/ continues the event
  FMOD_IMPEXP void SetPaused(bool bStatus);

  /// \brief
  ///   Returns the mute status of this event
  inline bool IsMuted() const {return (m_iFlags&VFMOD_FLAG_MUTE)>0;}

  /// \brief
  ///   Mutes/ un-mutes the event
  FMOD_IMPEXP void SetMuted(bool bStatus);

  /// \brief
  ///   Returns true if this event is disposed automatically by Fmod manager when stopped
  ///   or finished playing. Returns false if event needs to be disposed via DisposeObject().
  inline bool IsAutoDisposed() const {return (m_iFlags&VFMOD_FLAG_NODISPOSE)==0;}

  /// \brief
  ///   Returns if this event has a specific flag set or not
  inline bool HasFlag(int iFlag) const {return (m_iFlags&iFlag)>0;}

  /// \brief
  ///   Returns flags
  inline int GetFlags() const { return m_iFlags; }

  /// \brief
  ///   Returns true if this event is info only, i.e. such events can not be actually played.
  ///   This happens, when the maximum instance count of an event is exceeded. In that case
  ///   certain instances (depend upon the "Max playbacks behavior" of this event) become 
  ///   info-only and will be queued, so that they can be recovered later on, when they will
  ///   have a higher priority than the already existing not info-only instances. 
  inline bool IsInfoOnly() const { return m_bInfoOnly; }

  /// \brief
  ///   Gets the index and name of the event 
  ///  
  /// \param pEventIndex
  ///   Address of a variable to receive the event index. Specify NULL to ignore. 
  ///
  /// \param pszEventName
  ///   Address of a variable to receive the event name. Specify NULL to ignore. 
  /// 
  /// \param pInfo
  ///   Address of an FMOD_EVENT_INFO structure to receive extended event information. Specify NULL to ignore. 
  /// 
  /// \return
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetInfo(int *pEventIndex, char **pszEventName, FMOD_EVENT_INFO *pInfo) const;

  /// \brief
  ///   Gets an event parameter by name 
  /// 
  /// \param szParamName
  ///   The name of the event parameter
  /// 
  /// \param pParameter
  ///   Address of a variable to receive the selected event parameter   
  /// 
  /// \return 
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetParameter(const char *szParamName, FMOD::EventParameter **pParameter) const;

  /// \brief
  ///   Gets an event parameter by index
  /// 
  /// \param iParamIndex
  ///   The index of an event parameter. Indices are 0 based. Pass -1 to retrieve this event's primary parameter. 
  /// 
  /// \param pParameter
  ///   Address of a variable to receive the selected event parameter   
  /// 
  /// \return 
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetParameter(int iParamIndex, FMOD::EventParameter **pParameter) const;

  /// \brief
  ///   Gets an event user property by name 
  ///  
  /// \param szPropertyName
  ///   Name of the user property to retrieve. This is the name that was specified in Fmod Designer. 
  ///
  /// \param pValue
  ///   Address of a variable to receive the event user property
  ///
  /// \param bInstanceValue
  ///   If TRUE then retrieve the per-instance user property value, if FALSE then retrieve the parent 
  ///   event's user property value
  /// 
  /// \return
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetProperty(const char *szPropertyName, void *pValue, bool bInstanceValue) const;

  /// \brief
  ///   Sets an event user property by name 
  ///  
  /// \param szPropertyName
  ///   Name of the user property to set. This is the name that was specified in FMOD Designer. 
  ///
  /// \param pValue
  ///   Pointer to the new value for this event user property
  ///
  /// \param bInstanceValue
  ///   If TRUE then set the per-instance user property value, if FALSE then set the user property 
  ///   value of all event instances and also the parent event.
  FMOD_IMPEXP void SetProperty(const char *szPropertyName, void *pValue, bool bInstanceValue);

  /// \brief
  ///   Gets an event built-in property by index
  ///  
  /// \param iPropertyIndex
  ///   Index of the property to retrieve. For more information please check the FMOD EX documentation.
  ///
  /// \param pValue
  ///   Address of a variable to receive the event built-in property
  ///
  /// \param bInstanceValue
  ///   If TRUE then retrieve the per-instance user property value, if FALSE then retrieve the parent 
  ///   event's user property value
  /// 
  /// \return
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetProperty(int iPropertyIndex, void *pValue, bool bInstanceValue) const;

  /// \brief
  ///   Sets an event built-in property by index 
  ///  
  /// \param iPropertyIndex
  ///   Index of the property to set. For more information please check the FMOD Ex documentation. 
  ///
  /// \param pValue
  ///   Pointer to the new value for this event built-in property. 
  ///
  /// \param bInstanceValue
  ///   If TRUE then set the per-instance user property value, if FALSE then set the user property 
  ///   value of all event instances and also the parent event.
  FMOD_IMPEXP void SetProperty(int iPropertyIndex, void *pValue, bool bInstanceValue);

  /// \brief
  ///   Gets the internal Fmod event
  inline FMOD::Event* GetFmodEvent() const { return m_pEvent; }

  /// \brief
  ///   Returns whether the event has a valid Fmod event
  inline bool IsValid() const { return m_pEvent!=NULL; }

  ///
  /// @}
  ///


  ///
  /// @name Overridden functions
  /// @{
  ///

  // overridden VisObject3D_cl notification. Used to update event's position/orientation
  FMOD_IMPEXP VOVERRIDE void OnObject3DChanged(int iO3DFlags);

  // triggered when event is destroyed
  FMOD_IMPEXP VOVERRIDE void OnDisposeObject();

  // overridden VisObject3D_cl notification. Handle incoming messages, e.g. triggers
  FMOD_IMPEXP VOVERRIDE void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

#ifdef SUPPORTS_SNAPSHOT_CREATION
  FMOD_IMPEXP VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  ///
  /// @}
  ///


  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIAL_DLLEXP( VFmodEvent, FMOD_IMPEXP );

  FMOD_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  /// \brief
  ///   Overridden function to finalize event once the deserialization is finished
  FMOD_IMPEXP VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);

  /// \brief
  ///   Overridden function to indicate that we need a deserialization callback
  FMOD_IMPEXP VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context) {return context.m_eType!=VSerializationContext::VSERIALIZATION_EDITOR;}

  ///
  /// @}
  ///


  ///
  /// @name Misc
  /// @{
  ///

  /// \brief  
  ///   Helper function to set/remove flags
  ///
  /// \param iMask
  ///   Bit mask.
  ///
  /// \param bStatus
  ///   Set or Remove flag.
  void inline Helper_SetFlag(int iMask, bool bStatus) {if (bStatus) m_iFlags|=iMask; else m_iFlags &= (~iMask);}

  ///
  /// @}
  ///


  ///
  /// @name Internal functions
  /// @{
  ///

  // Initializes the event
  void Init();  
  
  // Reset to an info-only-event, when this event instance gets stolen
  void Reset(); 

  // Updates the event, only for internal use (+vForge)
  FMOD_IMPEXP void Update(bool bForceUpdate=false);

  // Sets the play-status of the event
  void SetPlaying(bool bStatus); 

  ///
  /// @}
  ///

private: 
  friend class VFmodManager;
  friend class VFmodEventCollection;

  VFmodEventCollection *m_pOwner;
  VFmodEventGroupPtr m_spEventGroup;

  int m_iFlags; 
  VString m_sEventName;   ///< name of the event relative to the parent event-group

  FMOD::Event *m_pEvent;
  bool m_bIsPlaying;
  bool m_bPlayedOnce;
  bool m_bInfoOnly;
  bool m_bStartPlayback;
};


/// \brief
///   A collection of event instances where each one is ref-counted. Also has some specific event functions such as StopAll().
class VFmodEventCollection : public VRefCountedCollection<VFmodEvent>
{
public:
  /// \brief
  ///   Internal function for debug output on screen
  FMOD_IMPEXP void DebugOutput();

  /// \brief
  ///   Internal function for updating all events in this collection
  FMOD_IMPEXP void Update();

  /// \brief
  ///   Removes all events from this collection that can be disposed (i.e. not playing anymore and VFMOD_FLAG_NODISPOSE flag not set)
  FMOD_IMPEXP void PurgeNotPlaying();

  /// \brief
  ///   Calls the Stop function on all instances and purges all events from this collection that can be disposed (i.e. not playing anymore and VFMOD_FLAG_NODISPOSE flag not set)
  FMOD_IMPEXP void StopAll(bool bReasonIsUnloadWorld = false);

  /// \brief
  ///   Returns the first occurrence (or NULL) of an event in this collection with specified object key. The object key can be set in vForge.
  FMOD_IMPEXP VFmodEvent* SearchEvent(const char* szName) const;
};


/// \brief
///   Event callback data object
class VFmodEventDataObject : public IVisCallbackDataObject_cl
{
public:
  /// \brief
  ///   Constructor
  VFmodEventDataObject(VisCallback_cl *pSender, VFmodEvent* pEvent, FMOD_EVENT_CALLBACKTYPE eType, void *pParam1, void *pParam2) 
    : IVisCallbackDataObject_cl(pSender), m_pEvent(pEvent), m_eType(eType), m_pParam1(pParam1), m_pParam2(pParam2)
  {
  }

  // Event
  VFmodEvent* m_pEvent;

  // Fmod event callback type
  FMOD_EVENT_CALLBACKTYPE m_eType;

  // First pointer param, when it's used. NULL otherwise. 
  // This param will be used depend upon the corresponding event callback type. Please check FMOD Ex documentation for further information.
  void *m_pParam1;

  // Second pointer param, when it's used. NULL otherwise. 
  // This param will be used depend upon the corresponding event callback type. Please check FMOD Ex documentation for further information.
  void *m_pParam2;
};

#endif // VFMODEVENT_HPP_INCLUDED

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
