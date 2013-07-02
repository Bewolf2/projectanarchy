/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_STATE_MACHINE_H
#define HKB_STATE_MACHINE_H


extern const class hkClass hkbStateMachineDelayedTransitionInfoClass;
extern const class hkClass hkbStateMachineNestedStateMachineDataClass;
extern const class hkClass hkbStateMachineStateInfoClass;
extern const class hkClass hkbStateMachineTransitionInfoArrayClass;
extern const class hkClass hkbStateMachineProspectiveTransitionInfoClass;
extern const class hkClass hkbStateMachineActiveTransitionInfoClass;
extern const class hkClass hkbStateMachineTransitionInfoReferenceClass;
extern const class hkClass hkbStateMachineTransitionInfoClass;
extern const class hkClass hkbStateMachineTimeIntervalClass;
extern const class hkClass hkbStateMachineClass;

#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h>
#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/Condition/hkbCondition.h>
#include <Common/Base/Container/PointerMap/hkPointerMap.h>

class hkbTransitionEffect;
class hkbCondition;
class hkbGeneratorSyncInfo;
class hkbNodeInternalStateInfo;
class hkbSymbolIdMap;
class hkbStateChooser;
class hkbStateListener;
template <typename T> class hkLocalArray;

/// A state machine that transitions between generators.
///
/// An hkbStateMachine organizes a set of states, each of which has an hkbGenerator.  The state machine is
/// itself an hkbGenerator.  When not in a transition, it simply generates a pose by calling the generator
/// corresponding to the current state.  During a transition, it generates poses using an hkbTransitionEffect to
/// generate the pose (typically by blending the generator being transitioned from with the one being transitioned
/// to.
class hkbStateMachine : public hkbGenerator
{
	public:
		// +version(4)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct a state machine.
		hkbStateMachine();

	protected:

		hkbStateMachine( const hkbStateMachine& sm, hkbBehaviorGraph& rootBehavior );

	public:

			/// Destructor.
		~hkbStateMachine();

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

		//////////////////////
		// hkbNode interface
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void handleEvent( const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;
		
			// hkbNode interface implementation.
		virtual void getInternalStateUser( const hkbBehaviorGraph& rootBehavior, hkReferencedObject& _internalState ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void setInternalStateUser(	const hkbContext& context,
											const hkReferencedObject& internalState,
											hkPointerMap<hkInt16,const hkbNodeInternalStateInfo*>& nodeIdToInternalState ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getActiveEvents( hkPointerMap<int, bool>& activeEvents ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getActiveVariablesSpecial( hkPointerMap<int, bool>& activeVariables ) const HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

			// hkbGenerator interface implementation.
		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual bool canRecycleOutput() const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void startEcho( hkReal echoDuration ) HK_OVERRIDE;

			/// A time interval bounded by events or hkReals.
			///
			/// If you specify the interval using fixed times (hkReals), then the times correspond to
			/// the amount of local state machine time that has passed since the state was entered.
			/// If you specify the interval using events, then the interval begins whenever the
			/// m_enterEventId event is received and ends whenever the m_exitEventId is
			/// received.
			///
			/// An interval is bounded on both ends either by events or fixed times.  You can't use
			/// an event on one end and a fixed time on the other.
			///
			/// When using either interval specification, you should try to ensure that the interval
			/// is at least one frame long.  Otherwise, the interval may open and close again in one
			/// frame, and its effects may not be processed.
			///
			/// If either m_enterEventId or m_exitEventId are not EVENT_ID_NULL, then they define the interval.
			/// Otherwise (if both are EVENT_ID_NULL), the m_enterTime and m_exitTime will define the interval.
		struct TimeInterval
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbStateMachine::TimeInterval );
			HK_DECLARE_REFLECTION();

				/// compare all members
			bool operator == ( const TimeInterval& ti );

			/// The event ID that bounds the beginning of the interval (set to EVENT_ID_NULL to use m_enterTime instead).
			hkInt32 m_enterEventId;	//+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_NONE")

			/// The event ID that bounds the end of the interval (set to EVENT_ID_NULL to use m_exitTime instead).
			hkInt32 m_exitEventId;	//+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_NONE")

			/// The time at which the interval is entered (used if both m_enterEventId and m_exitEventId are set to EVENT_ID_NULL).
			hkReal m_enterTime;	//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")

			/// The time at which the interval is exited (used if both m_enterEventId and m_exitEventId are set to EVENT_ID_NULL).
			/// Setting this to 0.0f means infinity; the inverval will not end.
			hkReal m_exitTime;	//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
		};

			/// A TransitionInfo stores information regarding a transition between states.
		class TransitionInfo
		{
			public:
				// +version(1)

				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbStateMachine::TransitionInfo );
				HK_DECLARE_REFLECTION();

					/// Construct with defaults.
				TransitionInfo();

					/// Construction given some basic info.
				TransitionInfo(	hkbEvent::Id eventId,
								int toStateId,
								hkbTransitionEffect* transition,
								hkbCondition* condition );

					/// Compare all members.
				bool operator == ( const TransitionInfo& ti );

					/// Whether the initiate interval uses explicit time bounds (as opposed to events).
				bool isInitiateIntervalTimeBounded() const;

					/// Whether the trigger interval uses explicit time bounds (as opposed to events).
				bool isTriggerIntervalTimeBounded() const;

			public:

					/// Flags indicating specialized transition behavior.
				enum TransitionFlags
				{
					//+defineflags(true)

						/// Only allow the transition to begin if the event arrives within the interval specified by m_triggerInterval.
					FLAG_USE_TRIGGER_INTERVAL = 0x1,

						/// Only allow the transition to begin within the interval specified by m_initiateInterval.
					FLAG_USE_INITIATE_INTERVAL = 0x2,

						/// Don't allow the transition to be interrupted while it is underway.
					FLAG_UNINTERRUPTIBLE_WHILE_PLAYING = 0x4,

						/// Don't allow the transition to be interrupted while it is
						/// waiting for the initiate interval to come.
					FLAG_UNINTERRUPTIBLE_WHILE_DELAYED = 0x8,

						/// Change states at the end of the transition instead of the beginning.
					FLAG_DELAY_STATE_CHANGE = 0x10,

						/// Disable the transition.
					FLAG_DISABLED = 0x20,

						/// Don't use this transition to return to the previous state.
					FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE = 0x40,

						/// Don't use this transition as a random transition.
					FLAG_DISALLOW_RANDOM_TRANSITION = 0x80,

						/// Disable the condition (effectively making it always true).
					FLAG_DISABLE_CONDITION = 0x100,

						/// Whether or not to allow this transition to do a self-transition.
						/// This flag is only considered for transitions from any state (see addTransitionFromAnyState()).
						/// Regular transitions specify both the source and destination state,
						/// so there is no need for an additional flag.
					FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE = 0x200,

						/// This transition is global, which means it can happen no
						/// matter what the active subgraph is.  This flag is only
						/// considered for transitions from-any-state (wildcards).
					FLAG_IS_GLOBAL_WILDCARD = 0x400,

						/// If you want the transition to be used as a regular (local) wildcard,
						/// set this flag.  This is useful if you want to have a different transition
						/// effect for a global wildcard transition than for local wildcard transition.
					FLAG_IS_LOCAL_WILDCARD = 0x800,

						/// Whether m_fromNestedStateId should be used.
					FLAG_FROM_NESTED_STATE_ID_IS_VALID = 0x1000,

						/// Whether m_toNestedStateId should be used.
					FLAG_TO_NESTED_STATE_ID_IS_VALID = 0x2000,

						/// Whether to delay until the end of the from generator, minus the blend lead time.
					FLAG_ABUT_AT_END_OF_FROM_GENERATOR = 0x4000,
				};

				enum InternalFlagBits
				{
						/// (internal) Whether the trigger interval has been entered.
					FLAG_INTERNAL_IN_TRIGGER_INTERVAL = 0x1,

						/// (internal) Whether the initiate interval has been entered.
					FLAG_INTERNAL_IN_INITIATE_INTERVAL = 0x2,
				};

					/// The interval in which the event must be received for the transition to occur.
					///
					/// This is only used if (m_flags & FLAG_USE_TRIGGER_INTERVAL) is true.
					/// You should make sure that the interval is longer than your timestep
					/// (eg, 1/30 sec), or else the interval may be missed.
				struct TimeInterval m_triggerInterval;

					/// The interval in which the transition may begin.
					///
					/// This is only used if (m_flags & FLAG_USE_BEGIN_INTERVAL) is true.
					/// If the transition is activated outside of this interval, the transition
					/// will be delayed until the interval begins.
					/// You should make sure that the interval is longer than your timestep
					/// (eg, 1/30 sec), or else the interval may be missed.
				struct TimeInterval m_initiateInterval;

					
					/// The transition to apply.
					///
					/// This is public but should be handled with care since it is reference counted.
					/// It's best to change this only by calling methods of hkbStateMachine.
				hkbTransitionEffect* m_transition;

					/// A condition that determines whether the transition should occur.
					///
					/// The transition only occurs if this condition evaluates to true.  If m_event == hkbEvent::EVENT_ID_NULL
					/// then the transition occurs as soon as the condition becomes true.  Otherwise, the transition occurs
					/// when the event is received and the condition is also true.
				hkbCondition* m_condition;

					/// the event that triggers the transition.
					
				hkInt32 m_eventId;	//+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_NONE")

					/// The state to which the transition goes.
				hkInt32 m_toStateId;

					/// The nested state (a state within the state machine that is inside the from-state)
					/// from which this transition must be done.
				hkInt32 m_fromNestedStateId;

					/// A nested state to which this transitions goes.
				hkInt32 m_toNestedStateId;

					/// Each transition has a priority.
				hkInt16 m_priority;

					/// Flags indicating specialized behavior (see TransitionInfoTransitionFlagBits).
				hkFlags< hkbStateMachine::TransitionInfo::TransitionFlags, hkInt16 > m_flags;

					/// A type for m_internalFlags, which combines bits from TransitionInfoInternalFlagBits.
				typedef hkUint8 InternalFlags;
		};

			/// A reference to a transition info within the state machine.  This indicates which list of transitions to use
			/// and what the index of the transition is in that list.
		struct TransitionInfoReference
		{
			// +version(1)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbStateMachine::TransitionInfoReference );
			HK_DECLARE_REFLECTION();

			TransitionInfoReference() : m_fromStateIndex(-2), m_transitionIndex(WILDCARD_STATE_INDEX), m_stateMachineId(SAME_STATE_MACHINE_ID) {}
			TransitionInfoReference( hkFinishLoadedObjectFlag flag ) {}

			static const hkInt16 WILDCARD_STATE_INDEX = -1;
			static const hkInt16 SAME_STATE_MACHINE_ID = -1;

				/// Whether this references a transition or not.
			bool isValid() const { return m_transitionIndex >= 0; }

				/// Invalidate this reference.
			void clear()
			{
				m_fromStateIndex = -2;
				m_transitionIndex = WILDCARD_STATE_INDEX;
				m_stateMachineId = SAME_STATE_MACHINE_ID;
			}

				/// The index of the state that the transition goes from, or -1 for a wildcard.
			hkInt16 m_fromStateIndex;

				/// The index of the transition in the list.
			hkInt16 m_transitionIndex;

				/// The id of the state machine from which this transition comes, or -1 if its from this state machine.
			hkInt16 m_stateMachineId;
		};

				/// Get a TransitionInfo from a TransitionInfoReference.
		TransitionInfo* getTransitionInfo( const hkbBehaviorGraph& rootBehavior, const TransitionInfoReference& transitionInfoReference ) const;

				/// Create a TE to be used in an active transition
		hkbTransitionEffect* createTransitionEffect( const hkbContext& context, const TransitionInfoReference& transitionInfoReference ) const;

				/// Get InternalFlags from a TransitionInfoReference.
		const TransitionInfo::InternalFlags* getInternalFlags( const TransitionInfoReference& transitionInfoReference ) const;

			/// Information regarding an active transition (for internal use).
		struct ActiveTransitionInfo
		{
			//+version(1)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbStateMachine::ActiveTransitionInfo );
			HK_DECLARE_REFLECTION();

			ActiveTransitionInfo() : m_transitionEffectInternalStateInfo(HK_NULL) {}
			ActiveTransitionInfo( hkFinishLoadedObjectFlag flag ): m_transitionInfoReference(flag), m_transitionInfoReferenceForTE(flag) {}
			~ActiveTransitionInfo();

				/// The address of the transition effect.  This is stored as an integer for serializing the internal state.
				/// When setting the internal state the address is used to look up the appropriate TE internal state.
			hkbTransitionEffect* m_transitionEffect;	//+nosave

				/// The internal state of the transition effect (only used when saving and restoring the internal state).
				/// Note that this memory is owned by ActiveTransitionInfo, but we have to be careful because the raw pointer
				/// is copied by setInternalState() from an hkbStateMachineInternalState to an hkbStateMachine.  
				/// To avoid deleting it twice, we always null out the pointer in the hkbStateMachine when
				/// setInternalStateUser() is called, so in practice only the hkbStateMachineInternalState ever
				/// has this pointer set to non-null (except for a brief period between setInternalState() and setInternalStateUser()).
			hkbNodeInternalStateInfo* m_transitionEffectInternalStateInfo;

				/// A reference to the transition info.
			struct TransitionInfoReference m_transitionInfoReference;

				/// A reference to the transition info where the TE came from (possibly different from the above).
				/// This is used to reconstruct the internal state.
			struct TransitionInfoReference m_transitionInfoReferenceForTE;

				/// The state that is being transitioned from.
			hkInt32 m_fromStateId;

				/// When using a global wildcard, the to-state will not be the one in the transition info,
				/// so we need an additional variable.
			hkInt32 m_toStateId;

				/// Whether this transition was chosen based on m_returnToPreviousStateEventId.
			hkBool m_isReturnToPreviousState;
		};

			/// Information regarding a transition that is under consideration (internal use).
		struct ProspectiveTransitionInfo
		{
			// +version(2)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbStateMachine::ProspectiveTransitionInfo );
			HK_DECLARE_REFLECTION();

			ProspectiveTransitionInfo() {}
			ProspectiveTransitionInfo( hkFinishLoadedObjectFlag flag ) : m_transitionInfoReference(flag), m_transitionInfoReferenceForTE(flag) {}

				/// The transition info from which this transition originated.
			struct TransitionInfoReference m_transitionInfoReference;

				/// The transition effect will be taken from this transition info if overridden.
			struct TransitionInfoReference m_transitionInfoReferenceForTE;

				/// When using a global wildcard, the to-state will not be the one in the transition info,
				/// so we need an additional variable.
			hkInt32 m_toStateId;

			void clear()
			{
				m_transitionInfoReference.clear();
				m_transitionInfoReferenceForTE.clear();
			}
		};

			/// An array of TransitionInfos wrapped for shared access.
		class TransitionInfoArray : public hkReferencedObject
		{
			//+vtable(1)
			public:

				HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
				HK_DECLARE_REFLECTION();

				TransitionInfoArray();

				bool hasEventlessTransitions() const;

				~TransitionInfoArray();

					/// The transitions.
				hkArray<class TransitionInfo> m_transitions;

					/// Whether any of the transitions have no event.
					/// Those ones require special processing during update().
				hkBool m_hasEventlessTransitions; //+nosave

					/// Whether any of the transitions have time-bounded trigger or initiate intervals.
					/// Those ones require special processing during update().
				hkBool m_hasTimeBoundedTransitions; //+nosave

			public:

				TransitionInfoArray( hkFinishLoadedObjectFlag flag );
		};

			/// An array of EventProperties wrapped for shared access.
		class EventPropertyArray : public hkReferencedObject
		{
				//+vtable(1)
			public:

				HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
				HK_DECLARE_REFLECTION();

				EventPropertyArray() {}

					/// The events.
				hkArray<class hkbEventProperty> m_events;

			public:

				EventPropertyArray( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_events(flag) {}
		};

			/// Information regarding a state of the state machine.
		class StateInfo : public hkbBindable
		{
			public:
				//+version(4)
				//+vtable(1)

				HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
				HK_DECLARE_REFLECTION();

				friend class hkbStateMachine;

					/// Construct with defaults.
				StateInfo();

			private:

					/// The StateInfo copy constructor takes a list of cloned listeners because the 
					/// hkbStateMachine clones the listeners as part of the decision whether to clone the state.
				StateInfo( const StateInfo& stateInfo, const hkLocalArray<hkbStateListener*>& clonedListeners );

			public:

					/// Construction given a generator and name.
				StateInfo( int stateId, hkbGenerator* generator, const char* name );

				~StateInfo();

					/// Set the generator associated with this state.
				void setGenerator( hkbGenerator* generator );

					/// Get the generator associated with this state.
				hkbGenerator* getGenerator() const;

					/// Get the number of transitions out of this state.
				int getNumTransitions() const;

					/// Get the TransitionInfo for one of the transitions out of this state.
				TransitionInfo& getTransitionInfo( int index ) const;

					/// Get all the transitions
				TransitionInfoArray* getTransitions() const;

					/// Get the state id.
				int getStateId() const;

					/// Add a listener to receive callbacks.
				void addListener( hkbStateListener* controller );

					/// Remove a listener to stop receiving receive callbacks.
				void removeListener( hkbStateListener* controller );

					/// Add enter notify event.
				void addEnterNotifyEvent( hkbEventBase::Id eventId, hkbEventPayload* eventPayload=HK_NULL );

					/// Add exit notify event.
				void addExitNotifyEvent( hkbEventBase::Id eventId, hkbEventPayload* eventPayload=HK_NULL );

					/// This is called by the hkbStateMachine when it is updated.
				void onUpdate( const hkbContext& context, hkReal timestep, hkbStateMachine& sm );

					/// This is called by the hkbStateMachine when the state is entered.
				void onEnter( const hkbContext& context, hkbStateMachine& sm );

					/// This is called by the hkbStateMachine when the state is exited.
				void onExit( const hkbContext& context, hkbStateMachine& sm );

				///////////////
				// properties
				///////////////

					/// A list of listeners that receive callbacks.
				hkArray<class hkbStateListener*> m_listeners;

					/// These events are sent when the state is entered.
				hkRefPtr<EventPropertyArray> m_enterNotifyEvents;

					/// These events are sent when the state is exited.
				hkRefPtr<EventPropertyArray> m_exitNotifyEvents;

			private:

					// The transitions out of this state.
				hkRefPtr<TransitionInfoArray> m_transitions;

					// The generator associated with this state.
				hkRefPtr<hkbGenerator> m_generator;

			public:

					/// The name of this state.
				hkStringPtr m_name;

			private:

					// The state id.
				int m_stateId;

			public:

					/// The state probability.  When choosing a random start state, each state is weighted according to its probability.
					/// The probabilities of all of the states being considered are normalized so that their sum is 1.
					/// When choosing a random transition, each transition in weighted according to the probability of the to-state.
				hkReal m_probability; //+default(1.0f)

					/// Enable this state.  Otherwise, it will be inaccessible.
				hkBool m_enable; //+default(true)
			
			private:

					// Flag to indicate whether this state has any eventless transitions
				hkBool m_hasEventlessTransitions; //+nosave

			public:

				StateInfo( hkFinishLoadedObjectFlag flag ) 
					: hkbBindable(flag), m_listeners(flag), m_enterNotifyEvents(flag), m_exitNotifyEvents(flag), m_transitions(flag), m_generator(flag), m_name(flag) {}
		};

			/// Add a state given a state index, generator and name.
		void addState( int stateId, hkbGenerator* generator, const char* name=HK_NULL );

			/// Remove the state with the specified index.
			///
			/// If ripple is true, the states are shifted to fill in the hole.  Otherwise the last state is used to fill the hole.
		void removeState( int stateId );

			/// Set the current state.
			///
			/// You should not call this method directly.  Use hkbBehaviorGraph::setStateOfStateMachine() instead.
			///
			/// If the state machine is inactive, or if ignoreTransitions is true,
			/// active and delayed transitions will be canceled, and the state will change
			/// instantaneously without applying a transition effect.  Otherwise, an attempt is made to
			/// find a transition to the desired state.  The transition is chosen in the usual way,
			/// taking into consideration everything that normally determines the suitability
			/// of a transition, except ignoring the event that is attached to the transition.
			/// If forceTransition is set to true, an instantaneous transition is made if
			/// no suitable transition is found.
			/// Note that it is not recommended that you use this function as a substitute for
			/// sending events to the state machine in order to induce transitions.
		void setState(	int stateId,
						const hkbContext& context,
						bool ignoreTransitions=false,
						bool forceTransition=false,
						bool clearStateHistory=false );

			/// Add a transition and returns the newly created TransitionInfo.
			/// Note that the returned TransitionInfo is only valid until you
			/// call addTransition() again, because each call to addTransition()
			/// may reallocate the internal array of TransitionInfos.
			///
			/// \param eventId The ID of the event that triggers the transition.
			/// \param fromState The state from which this transition occurs.
			/// \param toState The state to which this transition goes.
			/// \param transition The transition to apply.
			/// \param condition The condition that must be true for the transition to occur.
		TransitionInfo* addTransition(	hkbEvent::Id eventId,
										int fromStateId,
										int toStateId,
										hkbTransitionEffect* transition = HK_NULL,
										hkbCondition* condition = HK_NULL );

			/// Add from any state transition and return the newly created TransionInfo.
		TransitionInfo* addTransitionFromAnyState(	hkbEvent::Id eventId,
													int toStateId,
													hkbTransitionEffect* transition = HK_NULL,
													hkbCondition* condition = HK_NULL	);
			/// Remove a transition.
			///
			/// If ripple is true, the states are shifted to fill in the hole.  Otherwise the last state is used to fill the hole.
		void removeTransition( hkbEvent::Id, int fromStateId, int toStateId, bool ripple );

			/// Remove a from any state transition.
		void removeTransitionFromAnyState( hkbEvent::Id, int toStateId, bool ripple );

			/// Get the number of states.
		int getNumStates() const;

			/// Get the current state id.
		int getCurrentStateId() const;

			/// Get the current state id.
			///
			/// Instead of using this function, you might find it more convenient to use the
			/// TransitionInfo::FLAG_DELAY_STATE_TRANSITION flag to decide whether to change states
			/// at the beginning of a transition or at the end.
			///
			/// \param roundUp Determines which state to return if the state machine is in transition.
			///                If roundUp is false, the "from" state is returned.  Otherwise, the "to"
			///                state is returned.
		int getCurrentStateId( bool roundUp ) const;

			/// Get the StateInfo for a state given the index (in the range of 0 to getNumStates()-1).
		StateInfo& getStateInfoByIndex( int stateIndex );

			/// Get the StateInfo (const) for a state given the index (in the range of 0 to getNumStates()-1).
		const StateInfo& getStateInfoByIndex( int stateIndex ) const;

			/// Get the StateInfo for a state given the id.
		StateInfo& getStateInfoById( int stateId );

			/// Get the StateInfo (const) for a state given the id.
		const StateInfo& getStateInfoById( int stateId ) const;

			/// Returns true if the state machine is current in transition.
		bool isInTransition() const;

			/// Returns the list of from any state transitions
		hkArray<TransitionInfo>* getTransitionsFromAnyState() const;

			/// Returns true if deactivate() has not been called since the last call to activate().
		bool isActive();

			/// Returns the transition that is active.
		const ActiveTransitionInfo* getCurrentTransitionInfo() const;

			/// Returns the list of active transitions.
		const hkArray<ActiveTransitionInfo>& getActiveTransitionInfo();

			/// Return a state ID that has not been used yet (the smallest positive ID).
			///
			/// This method requires time linear in the number of states, so use it sparingly.
			/// It's better to choose the IDs yourself.
		int getUnusedStateId() const;

			/// Changes the ID of a state, and remaps all transitions and properties accordingly.  If the
			/// new ID is already in use, the IDs are swapped between the two states.
		void changeStateId( int oldId, int newId );

			/// Get a state index given the state ID.
		int getStateIndex( int stateId ) const;

			/// Begin a global transition. For internal use only.
		void beginGlobalTransition( const hkbContext& context, ProspectiveTransitionInfo& pti );

			/// Set next start state index override. For internal use only.
		void setNextStartStateIndexOverride( int nextStartStateIndexOveride );

	public:

		///////////////
		// properties
		///////////////

			/// If non-null, this event is sent at the beginning and end of a transition, or once for an instantaneous transition.
		class hkbEvent m_eventToSendWhenStateOrTransitionChanges;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			// an object that chooses the start state
		hkRefPtr<hkbStateChooser> m_startStateChooser;

			/// The start state.
		hkInt32 m_startStateId;

			/// If this event is received, the state machine returns to the previous state if there is an
			/// appropriate transition defined.
		hkInt32 m_returnToPreviousStateEventId;

			/// If this event is received, the state machine chooses a random transition from among
			/// those available.
			///
			/// The event of the transition is ignored, but all other considerations of whether
			/// the transition should be taken are considered.
		hkInt32 m_randomTransitionEventId;

			/// If the event is received, the state machine chooses a state with the id higher
			/// than the m_currentStateId and do a transition to that state.
			///
			/// The event of the transition is ignored, but all other considerations of whether
			/// the transition should be taken are considered.  If no appropriate transition is
			/// found, an immediate transition will occur.
		hkInt32 m_transitionToNextHigherStateEventId; //+default(-1)

			/// If the event is received, the state machine chooses a state with the id lower
			/// than the m_currentStateId and do a transition to that state.
			///
			/// The event of the transition is ignored, but all other considerations of whether
			/// the transition should be taken are considered.  If no appropriate transition is
			/// found, an immediate transition will occur.
		hkInt32 m_transitionToNextLowerStateEventId; //+default(-1)

			/// We use variables to sync the start state of the state machine.
			///
			/// The value of this index currently must be initialized to the state id which the user wants the
			/// start state to be. The state machine syncs it start state by setting the value of the start state
			/// to that of the variable during activate and sets the variable value to the toState when the transition begins.
		hkInt32 m_syncVariableIndex; //+default(-1)

			/// The id of the current state.
		hkInt32 m_currentStateId;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("The id of the current state.")
									//+serialized(false)

			/// This decides whether to transition when the m_currentStateId is maximum and m_transitionToNextHigherStateEventId
			/// is set m_currentStateId is minimum and m_transitionToNextLowerStateEventId is set.
			///
			/// If this is set to false there would be no transition. Otherwise there would be a transition but in the first case
			/// the next state id will be a lower than the current one and in the second case the next state id would be higher
			/// than the current state id.
		hkBool m_wrapAroundStateId;	//+default(true)

			/// The number of transitions that can be active at once.
			///
			/// When a transition B interrupts another transition A, the state machine can
			/// continue playing A an input to B.  If a transition is triggered when
			/// there are already m_maxSimultaneousTransitions active transitions, then
			/// the oldest transition will be immediately ended to make room for a new one.
		hkInt8 m_maxSimultaneousTransitions; //+default(32)

			/// Ways of setting the start state.
		enum StartStateMode
		{
				/// Set the start state to m_startStateId.
			START_STATE_MODE_DEFAULT,

				/// Set the start state from the variable whose index is m_syncVariableIndex.
			START_STATE_MODE_SYNC,

				/// Set the start state to a random state.
			START_STATE_MODE_RANDOM,

				/// Set the start state using m_startStateChooser.
			START_STATE_MODE_CHOOSER,
		};

			/// How to set the start state.
		hkEnum< StartStateMode, hkInt8 > m_startStateMode;

			/// How to deal with self-transitions (when the state machine is transitioned to while still active).
		enum StateMachineSelfTransitionMode
		{
				/// Stay in the current state.
			SELF_TRANSITION_MODE_NO_TRANSITION,

				/// Transition to the start state if a transition exists between the current state and the start state. 
			SELF_TRANSITION_MODE_TRANSITION_TO_START_STATE,

				/// Transition to the start state using a transition if there is one, or otherwise by abruptly changing states.
			SELF_TRANSITION_MODE_FORCE_TRANSITION_TO_START_STATE,
		};

			/// How to deal with self-transitions (when the state machine is transitioned to while still active).
		hkEnum< StateMachineSelfTransitionMode, hkInt8 > m_selfTransitionMode;	//+default(hkbStateMachine::SELF_TRANSITION_MODE_NO_TRANSITION)
																				//+hk.Description("Whether to transition to the start state during self-transition.")

			/// Whether or not the state machine is active (activate() called but not deactivate()).
		hkBool m_isActive;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
							//+hk.Description("Whether or not the state machine is active.")
							//+serialized(false)

	private:

			// Whether or not this transition should cause a transition to the previous state on the given event.
		bool isReturnToPreviousStateTransition( TransitionInfo& t, hkbEvent::Id eventId );

			// Given a stateId find the next closest state Id.
		bool isThereNextClosestStateId( const int stateId, hkbEvent::Id eventId, int& nextClosestStateId );

			// Whether or not a transition should be taken.  If you pass in NULL_EVENT, it will be matched with
			// the transition.  This is used for transitions that are triggered by hkbConditions or other
			// conditions without the necessity of an event.  If you want to ignore the event entirely,
			// treating each prospective transition as if its event has arrived, pass in ignoreEvents = true.
			// When processing transitions from any state, pass in true for fromAnyState.
		bool shouldTransition(	const hkbStateMachine* currentNestedStateMachine,
								TransitionInfo& t, 
								hkbEvent::Id eventId, 
								int toStateId, 
								bool ignoreEvents, 
								bool ignoreToState, 
								bool fromAnyState,
								bool isInTriggerInterval,
								const hkbContext& context );

			// Whether or not the state machine can transition right now.  It can't transition if there
			// is an uninterruptible transition active, or an uninterruptible delayed transition.
		bool canTransition( const hkbBehaviorGraph& rootBehavior ) const;

			// find all of the valid transitions (from the given array) out of a state
		void findValidTransitions(	const hkbStateMachine* currentNestedStateMachine,
									hkInt16 fromStateIndex,
									hkbEvent::Id eventId,
									int toStateId,
									bool ignoreEvents,
									bool ignoreToState,
									bool fromAnyState,
									const hkbContext& context,
									hkArray<TransitionInfoReference>& transitionsOut );

			// find all of the valid transitions that can be taken
		void findValidTransitions(	const hkbStateMachine* currentNestedStateMachine,
									hkbEvent::Id eventId, 
									int fromStateId,
									int toStateId,
									bool ignoreEvents,
									bool ignoreToState,
									const hkbContext& context,
									hkArray<TransitionInfoReference>& transitionsOut );

			// find a set of transition (from the given array) that should be taken
		void findBestTransitions(	const hkbStateMachine* currentNestedStateMachine,
									hkInt16 fromStateIndex,
									hkbEvent::Id eventId,
									int toStateId,
									bool ignoreEvents,
									bool ignoreToState,
									bool fromAnyState,
									const hkbContext& context,
									hkArray<TransitionInfoReference>& transitionsOut );

			// find a set of transition that should be taken
		void findBestTransitions( const hkbBehaviorGraph& behaviorGraph,
								  hkbEvent::Id eventId, 
								  int fromStateId,
								  int toStateId,
								  bool ignoreEvents,
								  bool ignoreToState,
								  hkbSymbolIdMap* eventIdMap,
								  const hkbContext& context,
								  hkArray<ProspectiveTransitionInfo>& ptis );

			// begin or delay a transition if it is higher priority than the active or delayed transition from a list of transitions
		void requestTransitions( const hkbContext& context, hkArray<ProspectiveTransitionInfo>& t, hkbEvent::Id eventId, bool setClearHistory, bool clearHistoryValue );

			// begin a transition
		void beginTransition( const hkbContext& context, ProspectiveTransitionInfo& t, bool isReturnToPreviousState );

			// end one of the active transitions (and all transitions previous to it)
		void endTransition( int activeTransitionIndex, const hkbContext& context, bool callRequestUpdateActiveNodes = true );

			// process an event
		void processEvent( const hkbContext& context, hkbEvent::Id eventId );

			// see if we need to begin or end a transition
		void updateTransitions( const hkbContext& context, hkbEvent::Id eventId, bool startDelayedAbuttedTransition );

			// remove all of the transitions to a particular state
		void removeTransitionsToState( int stateId );

			// from the given list, remove all of the transitions to a particular state
		static void HK_CALL removeTransitionsToState( int stateId, TransitionInfoArray* transitions );

			// set the state
		void setStateInternal( int stateId, bool isReturnToPreviousState, const hkbContext& context );

			// add the transition
		TransitionInfo* addTransitionInternal(	hkbEvent::Id eventId,
												int toStateId,
												hkbTransitionEffect* transition,
												hkbCondition* condition,
												hkArray<TransitionInfo>& transitions );

			// remove the transition
		void removeTransitionInternal( hkbEvent::Id eventId, int toStateId, bool ripple, hkArray<TransitionInfo>& transitions );

		/////////////////////////
		// transition intervals
		/////////////////////////

			// initialize a single transition interval (whether or not it is active)
		static void HK_CALL initTransitionInterval(	const hkbStateMachine::TimeInterval& interval,
													TransitionInfo::InternalFlags inFlag,
													TransitionInfo::InternalFlags& flags );

			// initialize the intervals in a TransitionInfo
		static TransitionInfo::InternalFlags HK_CALL initTransitionIntervals( const TransitionInfo& ti );

			// initialize all of the transition intervals in the list
		void initTransitionIntervals( const TransitionInfoArray* transitions, hkArray<TransitionInfo::InternalFlags>& flags );

			// update a single interval due to the passage of time
		void updateTransitionIntervalForTime(	hkReal timestep,
												const hkbStateMachine::TimeInterval& interval, 
												TransitionInfo::InternalFlags inFlag, 
												TransitionInfo::InternalFlags& flags );

			// update the transition intervals for a single TransitionInfo (due to the passage of time)
		void updateTransitionIntervalsForTime( hkReal timestep, const TransitionInfo& transitionInfo, TransitionInfo::InternalFlags& flags );

			// update the transition intervals for an array of TransitionInfos (due to the passage of time)
		void updateTransitionIntervalsForTime( hkReal timestep, const TransitionInfoArray* transitions, hkArray<TransitionInfo::InternalFlags>& flags );

			// update all of the transition intervals in the state machine (due to the passage of time)
		void updateTransitionIntervalsForTime( hkReal timestep );

			// update a single interval due to an event
		void updateTransitionIntervalForEvent(	hkbEvent::Id eventId, 
												const hkbStateMachine::TimeInterval& interval, 
												TransitionInfo::InternalFlags inFlag, 
												TransitionInfo::InternalFlags& flags );

			// update the transition intervals for an array of TransitionInfos (due to an event)
		void updateTransitionIntervalsForEvent( hkbEvent::Id eventId, const TransitionInfoArray* transitions, hkArray<TransitionInfo::InternalFlags>& flags );

			// update the transition intervals for a single TransitionInfo (due to an event)
		void updateTransitionIntervalsForEvent( hkbEvent::Id eventId, const TransitionInfo& transitionInfo, TransitionInfo::InternalFlags& flags );

			// update all of the transition intervals in the state machine (due to an event)
		void updateTransitionIntervalsForEvent( hkbEvent::Id eventId );

			// updates the all the active transitions
		void updateActiveTransitions( const hkbContext& context );

			// set the sync variable (if there is one) to the given state ID
		void setSyncVariable( const hkbContext& context, int stateId );

			// get the nested state machine template given the nested state machine index
		hkbStateMachine* getNestedStateMachineTemplate( const hkbBehaviorGraph& behaviorGraph, int stateIndex );

			// is the state machine within the current state in the given state
		bool isCurrentNestedStateId( const hkbStateMachine* currentNestedStateMachine, int nestedStateId );

			// return the current child generator, which could be a state generator or transition effect
		const hkbGenerator* getCurrentActiveChild() const;

			// change the local time and see if a delayed transition needs to be started
		bool allowDelayedAbuttedTransition( const hkbBehaviorGraph& rootBehavior, hkReal timestep );

			// is transition in abutment range
		bool isTransitionInAbutmentRange( const hkbGeneratorSyncInfo& mySyncInfo, hkbTransitionEffect* transitionEffect ) const;

			// get the current start state ID
		int getStartStateId( const hkbContext& context );

			// get a random state
		int getRandomState( const int* const stateIndices, int numStates, const hkbBehaviorGraph& behaviorGraph );

			// finish the object after loading from a packfile or cloning.
		void finish();

			// insert the active events from the given transitions into activeEvents
		static void insertActiveEvents( const hkbStateMachine::TransitionInfoArray* transitions, hkPointerMap<int,bool>& activeEvents );

	public:

		struct NestedStateMachineData
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbStateMachine::NestedStateMachineData );
			HK_DECLARE_REFLECTION();

			hkbStateMachine* m_nestedStateMachine; //+nosave
			hkbSymbolIdMap* m_eventIdMap; //+nosave
		};

			/// A transition that has been delayed and is waiting to initiate.
		struct DelayedTransitionInfo
		{
			//+version(1)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbStateMachine::DelayedTransitionInfo );
			HK_DECLARE_REFLECTION();

			DelayedTransitionInfo() {}
			DelayedTransitionInfo( hkFinishLoadedObjectFlag flag ) : m_delayedTransition(flag) {}

				/// The delayed transition.
			struct ProspectiveTransitionInfo m_delayedTransition;

				/// How long has this transition been delayed.  If it's too long then we start it.
			hkReal m_timeDelayed;

				/// Whether or not the delayed transition was chosen due to m_returnToPreviousStateEventId.
			hkBool m_isDelayedTransitionReturnToPreviousState;

				/// Whether or not the local time was within the abutment range of the end of the clip on the last frame.
				/// This is used when a transition is delayed due to using FLAG_ABUT_AT_END_OF_FROM_GENERATOR.
			hkBool m_wasInAbutRangeLastFrame;
		};

	private:

			// compute the nested state machine for a generator
		static void computeNestedStateMachine( hkbGenerator* gen, hkbStateMachine::NestedStateMachineData& );

	private:

			// the list of states
		hkArray<class StateInfo*> m_states;

			// the list of transitions from any state (don't have a specific from state)
		TransitionInfoArray* m_wildcardTransitions;

			// state id to index map type
		typedef hkPointerMap<int,int> StateIdToIndexMap;

			// state id to index map
		StateIdToIndexMap* m_stateIdToIndexMap; //+nosave

		HKB_BEGIN_INTERNAL_STATE(0);

			// the list of currently active transitions
		hkArray<struct hkbStateMachine::ActiveTransitionInfo> m_activeTransitions; //+nosave

			// internal flags for the transitions of the current state
		hkArray<hkUint8> m_transitionFlags; //+nosave

			// internal flags for the wildcard transitions
		hkArray<hkUint8> m_wildcardTransitionFlags; //+nosave

			// The delayed transitions. This list consists of all transitions which have their initiate intervals flag
			// or their abutment flag set. All the transitions in the list have the same priority.
		hkArray<struct hkbStateMachine::DelayedTransitionInfo> m_delayedTransitions; //+nosave

			// how many seconds we've been in the current state
		hkReal m_timeInState; //+nosave

			// what was the last local time from the syncInfo
		hkReal m_lastLocalTime;	//+nosave

		// This is commented out on purpose.  It forces the internal state to have a member for 
		// the output property of the same name.
		//hkInt32 m_currentStateId;

			// the state we were in before the current one
		hkInt32 m_previousStateId; //+nosave

			// the index of the next start state if overridden (-1 means not overridden)
		hkInt32 m_nextStartStateIndexOverride; //+nosave

			// whether the state or transition has changed lately
		hkBool m_stateOrTransitionChanged; //+nosave

			// Whether or not to perform an echo next time update() is called.
		hkBool m_echoNextUpdate; //+nosave

			// Flag to indicate whether this state machine has any eventless wildcard transitions.
		hkBool m_hasEventlessWildcardTransitions; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbStateMachine( hkFinishLoadedObjectFlag f );

};

#include <Behavior/Behavior/StateMachine/hkbStateMachine.inl>

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
