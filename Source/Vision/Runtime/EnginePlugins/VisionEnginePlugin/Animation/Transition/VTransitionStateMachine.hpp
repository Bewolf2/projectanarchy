/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTransitionStateMachine.hpp

#ifndef VTRANSITIONSTATEMACHINE_HPP_INCLUDED
#define VTRANSITIONSTATEMACHINE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/AnimationModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionBase.hpp>

// File versions
#define VTRANSITIONSTATE_MACHINE_VERSION_0  0   // Initial version
#define VTRANSITIONSTATE_MACHINE_VERSION_1  1   // Enabled state added
#define VTRANSITIONSTATE_MACHINE_VERSION_CURRENT  VTRANSITIONSTATE_MACHINE_VERSION_1

// Transition events
#define EVENT_TRANSITION_STARTED        1     ///< Event that is fired when a transition is started
#define EVENT_TRANSITION_FINISHED       2     ///< Event that is fired when a transition is finished
#define EVENT_SEQUENCE_STARTED          3     ///< Event that is fired when a sequence is started
#define EVENT_SEQUENCE_FINISHED         4     ///< Event that is fired when a sequence is finished
#define EVENT_TRANSITION_START_REACHED  5     ///< Event that is fired when the time position is reached to start the final target transition of an Intermediate Animation blending

// Forward declarations
class VisAnimNormalizeMixerNode_cl;
class VTransitionStateMachine;
class StateAnimControl_cl;

// Smart pointer typedef
typedef VSmartPtr<VTransitionStateMachine> VTransitionStateMachinePtr;
typedef VSmartPtr<StateAnimControl_cl> StateAnimControlPtr;

typedef DynArray_cl<VisTypedEngineObject_cl*> TDynArrEngObj;

///
/// \brief
///   Single state animation control that keeps reference to its mixer input index.
/// 
class StateAnimControl_cl: public VisSkeletalAnimControl_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP(StateAnimControl_cl, ANIMATION_IMPEXP );

  ///
  /// \brief
  ///   Constructor that creates a default animation control and sets the mixer input index
  /// 
  StateAnimControl_cl(VisSkeleton_cl* pSkeleton):VisSkeletalAnimControl_cl(pSkeleton, VSKELANIMCTRL_DEFAULTS|VANIMCTRL_LOOP)
  {
    m_pCustomNextAnimSeq = NULL;
    m_iMixerInputIndex = -1;
  }

  ///
  /// \brief
  ///   Method that returns the custom set animation sequence that follows this one when used with an
  ///   Intermediate Animation Transition.
  ///
  /// \returns
  ///   Pointer to the follow-up animation sequence
  /// 
  ANIMATION_IMPEXP VisAnimSequence_cl* GetCustomNextAnimSeq();

  ///
  /// \brief
  ///   Method that allows to set a custom animation sequence that should be used as the follow-up
  ///   animation sequence when using an Intermediate Animation Transition
  ///
  /// \param sequence
  ///   Pointer to the follow-up animation sequence
  /// 
  ANIMATION_IMPEXP void SetCustomNextAnimSeq(VisAnimSequence_cl *sequence);
  
  ///
  /// \brief
  ///   Serializes the instance to the passed archive
  ///
  ANIMATION_IMPEXP virtual void Serialize(VArchive &ar);

  int m_iMixerInputIndex;     ///< Mixer input index of this animation control

private:
  VISION_APIFUNC StateAnimControl_cl() {}
  VisAnimSequence_cl *m_pCustomNextAnimSeq;
};


///
/// \brief
///   Object component class that can be used to control animation transitions on an entity
// 
/// The transition state machine is an optional component that can be added to an entity to 
/// control transitions between different animation states. By simply setting a new animation 
/// state the state machine handles all the blending and synchronization of the transition process.
///
/// The transition data itself is loaded from a transition table, which can be created with the 
/// animation tool.
/// 
class VTransitionStateMachine : public IVObjectComponent
{
public:
  V_DECLARE_SERIAL_DLLEXP(VTransitionStateMachine, ANIMATION_IMPEXP );
  V_DECLARE_VARTABLE(VTransitionStateMachine, ANIMATION_IMPEXP)
  
  friend class VTransitionStateMachineNetworkSyncGroup;

  ///
  /// @name Constructor / Destructor
  /// @{
  ///
  

  ///
  /// \brief
  ///   Basic constructor which just initializes the members variables.
  ///
  /// Use the Init() function to initialize the transition state machine.
  ///
  ANIMATION_IMPEXP VTransitionStateMachine();


  ///
  /// \brief
  ///   Destructor
  ///
  ANIMATION_IMPEXP ~VTransitionStateMachine();


  ///
  /// \brief
  ///   Removes the component from the VTransitionManager class
  ///
  ANIMATION_IMPEXP virtual void DisposeObject();


  ///
  /// @}
  ///


  ///
  /// @name Initialization / Deinitialization
  /// @{
  ///


  ///
  /// \brief
  ///   Initialize the transition state machine via code
  /// 
  /// Use this function when creating and adding a transition state machine component via code.
  /// 
  /// When adding component through vForge, the artist will have specified a transition file which
  /// will automatically be loaded by the component. A dedicated initialization of the component is
  /// thus not necessary in that case.
  /// 
  /// \param pEntity
  ///   Future owner of the component. Has to be the same one as later passed to AddComponent
  /// 
  /// \param pTable
  ///   Transition table to be used by this state machine
  /// 
  /// \param bCreateAnimConfig
  ///   Specifies whether the component will create an anim config for the  passed owner entity and
  ///   set/replace this anim config on the entity. You can set this parameter to false if you want
  ///   to manually hook the animation (accessible via GetTransitionMixer) into your entity's
  ///   animation graph.
  ///
  ANIMATION_IMPEXP void Init(VisBaseEntity_cl *pEntity, VTransitionTable *pTable, bool bCreateAnimConfig = true);


  ///
  /// \brief
  ///   Deinitialize transition state machine by release all its references to reference counted objects
  ///
  ANIMATION_IMPEXP void DeInit();

  ///
  /// \brief
  ///   Returns whether this transition state machine has been successfully initialized
  /// \return
  ///   \c true if the transition state machine has been successfully initialized; \c false
  ///   if not.
  ///
  ANIMATION_IMPEXP bool IsInitialized() const;


#ifdef SUPPORTS_SNAPSHOT_CREATION
  ANIMATION_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot);
#endif
  ///
  /// @}
  ///


  ///
  /// @name Setting and retrieving the State
  /// @{
  ///

  /// \brief
  ///   Sets the enabled state of the transition machine.
  ANIMATION_IMPEXP void SetEnabled(BOOL bEnabled);

  /// \brief
  ///   Returns the enabled state of the transition machine.
  inline bool IsEnabled() const
  {
    return (m_bEnabled == TRUE);
  }

  ///
  /// \brief
  ///   Sets a new animation state by animation sequence pointer
  ///
  /// The pTargetSequence animation is set as the active animation state for
  /// the transition state machine. 
  ///
  /// If no state was set before then the animation is immediately set in the 
  /// transition mixer. Otherwise the blending is initiated, depending on the
  /// type of the result transition (cross fade, synchronized cross fade, ...).
  /// 
  /// \param pTargetSequence
  ///   New animation state
  ///
  ANIMATION_IMPEXP virtual void SetState(VisAnimSequence_cl *pTargetSequence);
  

  ///
  /// \brief
  ///   Sets a new animation state by animation name
  ///
  /// See SetState(VisAnimSequence_cl*) for more information.
  ///
  /// \param szName
  ///   Name of the animation that is set as new animation state
  ///
  ANIMATION_IMPEXP void SetState(const char *szName);
  

  ///
  /// \brief
  ///   Indicates whether the state machine is currently blending between
  ///   the active and inactive animation state.
  ///
  /// \returns
  ///   TRUE if transition state machine is still blending, FALSE otherwise
  ///
  ANIMATION_IMPEXP bool IsBlending();


  ///
  /// \brief
  /// Checks whether an Intermediate Blending is currently being processed.
  /// \return
  /// returns true if a intermediate blending is being processed, else false
  ///
  ANIMATION_IMPEXP bool IsIntermediateBlending(){return m_bIsIntermediateBlending;}


  ///
  /// \brief
  ///   Indicates whether the state machine is currently waiting for a synchronized blend operation
  ///
  /// Synchronized transitions require the state machine to start the blend operation at
  /// specified events only. This function indicates whether the state machine is currently
  /// in the time period between setting the new state and starting the blending.
  ///
  /// \returns
  ///   TRUE if the state machine is waiting to reach a sync event to start the blending  
  ///
  ANIMATION_IMPEXP bool IsWaitingForSyncBlending();


  ///
  /// \brief
  ///   Gets the active state of the state machine (target animation state)
  ///
  /// \returns
  ///   Pointer to currently active animation sequence, or NULL if TransitionStateMachine has not 
  ///   been initialized.
  ///
  ANIMATION_IMPEXP VisAnimSequence_cl* GetActiveState();


  ///
  /// \brief
  ///   Gets inactive state of the state machine (source animation state)
  ///
  /// \returns
  ///   Pointer to recently active animation sequence, or NULL if TransitionStateMachine has not 
  ///   been initialized.
  ///
  ANIMATION_IMPEXP VisAnimSequence_cl* GetInactiveState();


  ///
  /// \brief
  ///   Gets the animation control of the active animation
  ///
  /// Both the active and inactive animations have a skeletal animation control
  /// which takes care of the playback. The transition mixer node takes both
  /// animation controls as input and blends between them.
  ///
  /// \returns
  ///   Animation control that is responsible for the playback of the active animation
  ///
  ANIMATION_IMPEXP VisSkeletalAnimControl_cl* GetActiveControl();


  ///
  /// \brief
  ///   Gets the animation control of the inactive animation
  ///
  /// See GetActiveControl for more information.
  ///
  /// \returns
  ///   Animation control that is responsible for the playback of the inactive animation
  ///
  ANIMATION_IMPEXP VisSkeletalAnimControl_cl* GetInactiveControl();


  ///
  /// @}
  ///


  ///
  /// @name Access to Members
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the transition table used by this state machine
  ///
  ANIMATION_IMPEXP VTransitionTable *GetTransitionTable() const;


  ///
  /// \brief
  ///   Gets the animation mixer node that is controlled by the state machine
  ///
  /// The transition state machine uses a standard normalize mixer node for
  /// blending between the animations. This function gives you access to this
  /// mixer node and allows you to integrate into the animation graph of your
  /// entity.
  /// 
  /// \returns
  ///   Pointer to the transition mixer node  
  ///
  /// \note
  ///   You only need to take care of the transition mixer in case you explicitly 
  ///   instructed the state machine to NOT create an animation configuration.
  ///   See Init() function for details.
  ///
  ANIMATION_IMPEXP VisAnimNormalizeMixerNode_cl *GetTransitionMixer() const;


  ///
  /// @}
  ///


  ///
  /// @name Event Listener
  /// @{
  ///


  ///
  /// \brief
  ///   Adds a listener to the state machine that will receive events via the MessageFunction.
  ///
  /// By default only transition events will be forwarded to the registered event listeners, either 
  /// EVENT_TRANSITION_STARTED or EVENT_TRANSITION_FINISHED that are triggered when a transitions starts 
  /// or finishes. In addition you can specify whether also animation events should be forwarded to
  /// the registered event listeners, see SetForwardingAnimEvents(). This is disabled by default.
  ///
  /// The following values are passed to the receiver's message function:
  /// - iID: VIS_MSG_TRANSITIONSTATEMACHINE for transition events or VIS_MSG_EVENT for animation events (if enabled)
  /// - iParamA: Either EVENT_TRANSITION_STARTED or EVENT_TRANSITION_FINISHED or the respective EventID of the animation event
  /// 
  /// \param pObj
  ///   Base object to be added as a listener, for instance an entity
  /// 
  /// \returns
  ///   Index of the listener in the internal list
  ///
  /// \see RemoveEventListener()
  /// \see SetForwardingAnimEvents()
  /// \see GetForwardingAnimEvents()
  ///
  ANIMATION_IMPEXP int AddEventListener(VisTypedEngineObject_cl* pObj);


  ///
  /// \brief
  ///   Removes a listener from the state machine
  /// 
  /// \param pObj
  ///   Listener to be removed
  /// 
  /// \returns
  ///   TRUE if the listener could be removed, otherwise FALSE
  ///
  /// \see AddEventListener()
  ///
  ANIMATION_IMPEXP bool RemoveEventListener(VisTypedEngineObject_cl* pObj);


  ///
  /// \brief
  ///   Removes all event listeners from the state machine
  ///
  ANIMATION_IMPEXP void RemoveAllEventListeners();


  ///
  /// \brief
  ///   Gets a pointer to a registered event listener
  /// 
  /// \param iIndex
  ///   Index of listener in list
  /// 
  /// \returns
  ///   Listener with the specified index
  ///
  inline VisTypedEngineObject_cl* GetEventListener(int iIndex) const
  {
    VASSERT((iIndex >= 0) && (iIndex < m_iEventListenerCount));
    return m_pEventListener.GetDataPtr()[iIndex];
  }


  ///
  /// \brief
  ///   Returns the total number of registered event listeners
  /// 
  /// \returns
  ///   Number of event listeners
  ///
  inline int GetEventListenerCount() const 
  {
    return m_iEventListenerCount;
  }


  ///
  /// \brief
  ///   Specifies whether the TSM should forward also animation events to the registered event listeners
  /// 
  /// \param bEnable
  ///   TRUE if the transition state machine will forward also the animation events to the registered listeners
  ///
  ANIMATION_IMPEXP inline void SetForwardingAnimEvents(bool bEnable)
  {
    m_bForwardAnimEvents = bEnable;
  }


  ///
  /// \brief
  ///   Determines whether the TSM is forwarding also animation events to the registered event listeners
  /// 
  /// \returns
  ///   TRUE if the transition state machine is forwarding also the animation events to the registered listeners
  ///
  ANIMATION_IMPEXP inline bool GetForwardingAnimEvents()
  {
    return m_bForwardAnimEvents;
  }


  ///
  /// @}
  ///


  ///
  /// @name IVObjectComponent Virtual Overrides
  /// @{
  ///

  ///
  /// \brief
  ///   Initializes the state machine once the deserialization is finished and the parent
  ///   object is fully available.
  ///
  ANIMATION_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context);


  ///
  /// \brief
  ///   Overridden function. Ensures that this component can only be attached to entities
  ///   with a valid model loaded.
  ///
  ANIMATION_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);


  ///
  /// \brief
  ///   Overridden function. Sets the new owner of the components.
  ///
  /// The owner of the component will typically only be set once (when the component
  /// is added to the entity).
  ///
  /// SetOwner() takes care of loading the initialization/deinitialization involved
  /// with owner changes.
  ///
  /// \param pOwner
  ///   New owner of this component
  ///
  ANIMATION_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner);


  ///
  /// \brief
  ///   Updates the the state machine for the current frame.
  /// 
  /// The OnThink() function takes care of all actions that are not automatically
  /// handled by the transition mixer node. This includes special event and motion
  /// delta handling.
  ///
  /// \note
  ///   You do not have to call this function manually, since the VTransitionManager
  ///   class will take care of this
  ///
  ANIMATION_IMPEXP void OnThink();


  ///
  /// @}
  ///


  ///
  /// @name Serialization
  /// @{
  ///


  ///
  /// \brief
  ///   Serializes the instance to the passed archive
  ///
  ANIMATION_IMPEXP virtual void Serialize(VArchive &ar);


  ///
  /// \brief
  ///   Override this function to make sure that we get a serialization callback
  ///
  virtual VBool WantsDeserializationCallback(const VSerializationContext &context) { return context.m_eType!=VSerializationContext::VSERIALIZATION_EDITOR; }


  ///
  /// \brief
  ///   Set the state machine according the given sync state stored in the passed archive
  ///
  ANIMATION_IMPEXP virtual void SetSyncState(VArchive &ar);
  
  ///
  /// \brief
  ///   Stores the current state machine sync state in the passed archive
  ///
  ANIMATION_IMPEXP virtual void GetSyncState(VArchive &ar);

  ///
  /// @}
  ///
  


  ///
  /// \brief
  ///   Sets the initial animation
  ///
  /// \param szAnimationName
  ///   string with the name of the initial animation that should be set
  ///
  /// The animation sequence is first searched for in the transition table, and then in the list of
  /// animation sequences defined by the model.
  ANIMATION_IMPEXP void SetInitialAnimation(const char* szAnimationName) { InitialAnimation = szAnimationName; }


  ///
  /// \brief
  ///   Gets the initial animation
  ///
  /// \return
  ///   string with the name of the initial animation that should be set
  ///
  ANIMATION_IMPEXP const VString& GetInitialAnimation() const { return InitialAnimation; }

  ///
  /// @name Network related
  /// @{
  ///

  ANIMATION_IMPEXP int GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList);

  ///
  /// @}
  ///

protected:

  ///
  /// @name Network related
  /// @{
  ///

  ///
  /// \brief
  ///   Returns true if the state of the TSM changed (will be cleared as soon as the state is synced via the network)
  ///
  inline bool HasStateChanged() { return m_bStateChanged; }

  ///
  /// \brief
  ///   Clear the state changed flag of the TSM
  ///
  inline void ClearStateChange() { m_bStateChanged = false; }

  ///
  /// @}
  ///

  ANIMATION_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);


  ///
  /// \brief
  ///   Event handling for fading in/out events
  ///
  ANIMATION_IMPEXP void OnEvent(INT_PTR iEvent);


  ///
  /// \brief
  ///   Process offset delta calculations for immediate transitions manually
  ///
  ANIMATION_IMPEXP void ProcessOffsetDelta();


  ///
  /// \brief
  ///   Trigger fading in/out events
  ///
  ANIMATION_IMPEXP void TriggerEvent(INT_PTR iEvent);


  ///
  /// \brief
  ///   Sends a message to all listeners.
  /// 
  /// \param iID
  ///   Message ID that can be either VIS_MSG_EVENT or VIS_MSG_TRANSITIONSTATEMACHINE  
  ///
  /// \param iParamA
  ///   Event to be sent.
  ///
  /// \param iParamB
  ///   Pointer to a VisAnimControl_cl or a VTransitionStateMachine, depending on parameter iID.
  ///
  ANIMATION_IMPEXP void SendToAllListeners(int iID, INT_PTR iParamA, INT_PTR iParamB) const;


  ///
  /// \brief
  ///   Event listener to sequence events
  ///
  ANIMATION_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);


  /// 
  /// \brief
  ///   Reloads the transition data with the specified filename
  /// 
  /// \param szFilename
  ///   Filename of the transition file to load
  /// 
  void ReloadTable(const char *szFilename);


  /// 
  /// \brief
  ///   Creates a default transition table with a single default transition
  /// 
  void ReloadDefaultTable();


  /// 
  /// \brief
  ///   Returns a free animation control for a new animation state.
  ///
  /// When blending from one animation state to another a new animation control
  /// has to be set up for the new animation sequence. When no free animation
  /// controls area available a new animation control will be added to the 
  /// animation control list.
  /// 
  /// \param pSequence
  ///
  ///
  /// \return
  ///   VisSkeletalAnimControl_cl: Pointer to a free skeletal animation control. 
  ///
  StateAnimControl_cl* GetFreeStateAnimControl(VisAnimSequence_cl* pSequence);


  /// 
  /// \brief
  ///   Removes all inactive animation controls from our state animation control list.
  ///
  void RemoveInactiveStateAnimControls();


  /// 
  /// \brief
  ///   Reset all skeletal anim control event listeners stored in aEventListeners.
  ///   Internally delete the allocated TDynArrEngObj* instances.
  /// 
  /// \param aEventListeners
  ///
  void ResetAllSkeletalAnimControlEventListeners( DynArray_cl<TDynArrEngObj*> &aEventListeners );


  /// 
  /// \brief
  ///   Get all skeletal anim control event listeners and store them in aEventListeners
  ///   Internally allocate TDynArrEngObj* instances to store the event listener pointers.
  /// 
  /// \param aEventListeners
  ///
  void GetAndDisableAllSkeletalAnimControlEventListeners( DynArray_cl<TDynArrEngObj*> &aEventListeners );



private:

  // Exposed to vForge:

  VString TransitionTableFile;        ///< parameter
  VString InitialAnimation;           ///< parameter

  // Not exposed to vForge: 

  VSmartPtr<VisAnimNormalizeMixerNode_cl> m_spNormalizeMixer;           ///< Animation normalize mixer responsible for handling the transitions
  DynArray_cl<VisTypedEngineObject_cl*> m_pEventListener;               ///< Collection of listeners which get notified about transition events
  int m_iEventListenerCount;                                            ///< Number of event listeners
  int m_iActiveEventID;                                                 ///< Active animation event ID (important for synchronized transitions)
  bool m_bForwardAnimEvents;                                            ///< If TRUE the Transition State Machine will forward animation events also to the registered event listeners (default false)

  VRefCountedCollection<StateAnimControl_cl> m_SkeletalAnimControlList; ///< Skeletal animation control list  
  StateAnimControlPtr m_spPrimaryStateAnimControl;                      ///< Primary Animation control (Currently active animation sequence)
  StateAnimControlPtr m_spSecondaryStateAnimControl;                    ///< Secondary Animation control (Last active animation sequence)

  VisBaseEntity_cl *m_pEntity;                                          ///< Owner entity
  VDynamicMesh *m_pMesh;                                                ///< Model of entity
  VSmartPtr<VisAnimConfig_cl> m_spAnimConfig;                           ///< Hold a reference to the anim config

  VTransitionTablePtr m_spTransTable;                                   ///< Transition Table of this Transition State Machine
  VTransitionDef* m_pTransition;                                        ///< Active blending transition
  VSequenceDef* m_pSequence;                                            ///< Additional data of currently active animation sequence
  VTransitionTrigger_e m_eIntermediateFollowUpTrigger;                  ///< FollowUp trigger that has been associated with the currently running Intermediate animation transition               

  float m_fTransitionTime;                                              ///< Passed transition time since last blending started  

  BOOL m_bEnabled;                                                      ///< Determines if animations are computed
  bool m_bCreateAnimConfig;                                             ///< specifies whether it is the component's responsibility to create the anim config
  bool m_bIsBlending;                                                   ///< If TRUE the Transition State Machine is currently in the blending state
  bool m_bSyncBlending;                                                 ///< If TRUE the Transition State Machine is currently waiting for an event to initiate the blending
  bool m_bProcessOffsetDelta;                                           ///< If TRUE the Transition State Machine processes the motion delta manually 
  
  bool m_bFinishBlending;                                               ///< If TRUE the Transition State Machine will finish the current blending before processing any other state handling
  bool m_bFinishSequence;                                               ///< If TRUE the Transition State Machine will finish the current sequence before processing any other state handling
  bool m_bSyncSerialize;                                                ///< If TRUE the next call on Serialize is performed through a SetSyncState or GetSyncState call; used internally
  bool m_bIsIntermediateBlending;                                       ///< Flag indicating whether we're currently in intermediate blending mode.
  
  bool m_bStateChanged;                                                 ///< Is set to TRUE until the network synchronied the state
};

///
/// \brief
///   Collection of transition state machine instances.
///
class VTransitionStateMachineCollection : public VRefCountedCollection<VTransitionStateMachine>
{
};

/// \brief
///   Implements IVNetworkSynchronizationGroup as a synchronization group that synchronizes the transitions of the TSM component. 
///
class VTransitionStateMachineNetworkSyncGroup : public IVNetworkSynchronizationGroup
{
public:
  ANIMATION_IMPEXP VTransitionStateMachineNetworkSyncGroup() { }
  ANIMATION_IMPEXP ~VTransitionStateMachineNetworkSyncGroup() { }
  ANIMATION_IMPEXP virtual const char *GetName() const HKV_OVERRIDE {return "AnimationTransition";}
  ANIMATION_IMPEXP virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_TransitionStateMachine;}
  ANIMATION_IMPEXP virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  ANIMATION_IMPEXP virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;

public:
  static VTransitionStateMachineNetworkSyncGroup g_GroupInstanceT;
};

#endif // VTRANSITIONSTATEMACHINE_HPP_INCLUDED

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
