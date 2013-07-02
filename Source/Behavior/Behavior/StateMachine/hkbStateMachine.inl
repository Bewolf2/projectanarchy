/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE bool hkbStateMachine::TimeInterval::operator == ( const TimeInterval& ti )
{
	return (	( m_enterEventId == ti.m_enterEventId ) &&
				( m_exitEventId == ti.m_exitEventId ) &&
				( m_enterTime == ti.m_enterTime ) &&
				( m_exitTime == ti.m_exitTime ) );
}

HK_FORCE_INLINE hkbStateMachine::TransitionInfo::TransitionInfo()
:	m_transition(HK_NULL),
	m_condition(HK_NULL),
	m_eventId(hkbEvent::EVENT_ID_NULL),
	m_toStateId(0),
	m_fromNestedStateId(0),
	m_toNestedStateId(0),
	m_priority(0),
	m_flags(0)
{
	m_initiateInterval.m_enterEventId = hkbEvent::EVENT_ID_NULL;
	m_initiateInterval.m_exitEventId = hkbEvent::EVENT_ID_NULL;
	m_triggerInterval.m_enterEventId = hkbEvent::EVENT_ID_NULL;
	m_triggerInterval.m_exitEventId = hkbEvent::EVENT_ID_NULL;
	m_initiateInterval.m_enterTime = 0.0f;
	m_initiateInterval.m_exitTime = 0.0f;
	m_triggerInterval.m_enterTime = 0.0f;
	m_triggerInterval.m_exitTime = 0.0f;
}

HK_FORCE_INLINE hkbStateMachine::TransitionInfo::TransitionInfo(	hkbEvent::Id eventId, 
																	int toStateId, 
																	hkbTransitionEffect* transition,
																	hkbCondition* condition )
:	m_transition(transition),
	m_condition(condition),
	m_eventId(eventId),
	m_toStateId(toStateId),
	m_fromNestedStateId(0),
	m_toNestedStateId(0),
	m_priority(0),
	m_flags(0)
{
	m_initiateInterval.m_enterEventId = hkbEvent::EVENT_ID_NULL;
	m_initiateInterval.m_exitEventId = hkbEvent::EVENT_ID_NULL;
	m_triggerInterval.m_enterEventId = hkbEvent::EVENT_ID_NULL;
	m_triggerInterval.m_exitEventId = hkbEvent::EVENT_ID_NULL;
	m_initiateInterval.m_enterTime = 0.0f;
	m_initiateInterval.m_exitTime = 0.0f;
	m_triggerInterval.m_enterTime = 0.0f;
	m_triggerInterval.m_exitTime = 0.0f;
}

HK_FORCE_INLINE bool hkbStateMachine::TransitionInfo::operator == ( const TransitionInfo& ti )
{
	return (	( m_triggerInterval == ti.m_triggerInterval ) &&
				( m_initiateInterval == ti.m_initiateInterval ) &&
				( m_transition == ti.m_transition ) &&
				( m_condition == ti.m_condition ) &&
				( m_eventId == ti.m_eventId ) &&
				( m_toStateId == ti.m_toStateId ) &&
				( m_fromNestedStateId == ti.m_fromNestedStateId ) &&
				( m_toNestedStateId == ti.m_toNestedStateId ) &&
				( m_priority == ti.m_priority ) &&
				( m_flags == ti.m_flags ) );
}

HK_FORCE_INLINE hkbStateMachine::StateInfo::StateInfo()
:	m_stateId(0),
	m_probability(1.0f),
	m_enable(true),
	m_hasEventlessTransitions(false)
{
}

HK_FORCE_INLINE hkbStateMachine::StateInfo::StateInfo( int stateId, hkbGenerator* generator, const char* name )
:	m_generator(generator),
	m_name(name),
	m_stateId(stateId),
	m_probability(1.0f),
	m_enable(true),
	m_hasEventlessTransitions(false)
{
}

HK_FORCE_INLINE hkbGenerator* hkbStateMachine::StateInfo::getGenerator() const 
{ 
	return m_generator; 
}

HK_FORCE_INLINE int hkbStateMachine::StateInfo::getNumTransitions() const 
{ 
	return ( m_transitions == HK_NULL ) ? 0 : m_transitions->m_transitions.getSize(); 
}

HK_FORCE_INLINE int hkbStateMachine::StateInfo::getStateId() const
{
	return m_stateId;
}

HK_FORCE_INLINE hkbStateMachine::TransitionInfo& hkbStateMachine::StateInfo::getTransitionInfo( int index ) const
{ 
	return m_transitions->m_transitions[index]; 
}

HK_FORCE_INLINE hkbStateMachine::TransitionInfoArray* hkbStateMachine::StateInfo::getTransitions() const
{
	return m_transitions;
}

#ifndef HK_PLATFORM_SPU

HK_FORCE_INLINE hkbStateMachine::TransitionInfo* hkbStateMachine::addTransition(	hkbEvent::Id eventId, 
																					int fromStateId, 
																					int toStateId, 
																					hkbTransitionEffect* transition,
																					hkbCondition* condition )

{
	int fromStateIndex = getStateIndex( fromStateId );
	HK_ASSERT2( 0xf3e9a4d2, fromStateIndex != -1, "Invalid state index.\n" );

	StateInfo* si = m_states[fromStateIndex];

	// create a transition array of there isn't one already
	if ( si->m_transitions == HK_NULL )
	{
		si->m_transitions.setAndDontIncrementRefCount( new TransitionInfoArray() );
	}

	return addTransitionInternal( eventId, toStateId, transition, condition, m_states[fromStateIndex]->m_transitions->m_transitions );
}

HK_FORCE_INLINE hkbStateMachine::TransitionInfo* hkbStateMachine::addTransitionFromAnyState(	hkbEvent::Id eventId, 
																								int toStateId, 
																								hkbTransitionEffect* transition,
																								hkbCondition* condition )

{
	if ( m_wildcardTransitions == HK_NULL )
	{
		m_wildcardTransitions = new TransitionInfoArray();
	}

	TransitionInfo* info = addTransitionInternal( eventId, toStateId, transition, condition, m_wildcardTransitions->m_transitions );
	info->m_flags.orWith( TransitionInfo::FLAG_IS_LOCAL_WILDCARD );
	return info;
}
#endif //!SPU

HK_FORCE_INLINE void hkbStateMachine::removeTransition( hkbEvent::Id eventId, int fromStateId, int toStateId, bool ripple )
{
	int fromStateIndex = getStateIndex( fromStateId );

	if ( fromStateIndex >= 0 )
	{
		removeTransitionInternal( eventId, toStateId, ripple, m_states[fromStateIndex]->m_transitions->m_transitions );
	}
}

HK_FORCE_INLINE void hkbStateMachine::removeTransitionFromAnyState( hkbEvent::Id eventId, int toStateId, bool ripple )
{
	HK_ASSERT( 0xdf76ad65, m_wildcardTransitions != HK_NULL );

	removeTransitionInternal( eventId, toStateId, ripple, m_wildcardTransitions->m_transitions );
}

HK_FORCE_INLINE hkArray<hkbStateMachine::TransitionInfo>* hkbStateMachine::getTransitionsFromAnyState() const
{
	if ( m_wildcardTransitions == HK_NULL )
	{
		return HK_NULL;
	}
	else
	{
		return &m_wildcardTransitions->m_transitions;
	}
}

HK_FORCE_INLINE bool hkbStateMachine::isActive()
{ 
	return m_isActive; 
}

HK_FORCE_INLINE const hkbStateMachine::ActiveTransitionInfo* hkbStateMachine::getCurrentTransitionInfo() const
{
	int count = m_activeTransitions.getSize();

	if ( count > 0 )
	{
		return &m_activeTransitions[count-1];
	}

	return HK_NULL;
}

HK_FORCE_INLINE const hkArray<hkbStateMachine::ActiveTransitionInfo>& hkbStateMachine::getActiveTransitionInfo()
{
	return m_activeTransitions;
}

HK_FORCE_INLINE void hkbStateMachine::setNextStartStateIndexOverride( int nextStartStateIndexOverride )
{
	m_nextStartStateIndexOverride = nextStartStateIndexOverride;
}

HK_FORCE_INLINE int hkbStateMachine::getCurrentStateId() const
{
	return m_currentStateId;
}

HK_FORCE_INLINE int hkbStateMachine::getCurrentStateId( bool roundUp ) const
{
	int transitionCount = m_activeTransitions.getSize();

	if ( transitionCount > 0 )
	{
		const ActiveTransitionInfo& ati = m_activeTransitions[transitionCount-1];

		return ( roundUp ? ati.m_toStateId : ati.m_fromStateId );
	}
	else
	{
		return m_currentStateId;
	}
}

HK_FORCE_INLINE int hkbStateMachine::getNumStates() const
{
	return m_states.getSize();
}

HK_FORCE_INLINE hkbStateMachine::StateInfo& hkbStateMachine::getStateInfoByIndex( int stateIndex )
{
	return *m_states[stateIndex];
}

HK_FORCE_INLINE const hkbStateMachine::StateInfo& hkbStateMachine::getStateInfoByIndex( int stateIndex ) const
{
	return *m_states[stateIndex];
}

HK_FORCE_INLINE hkbStateMachine::StateInfo& hkbStateMachine::getStateInfoById( int stateId )
{
	int stateIndex = getStateIndex( stateId );
	HK_ASSERT2( 0x19af3d7c, stateIndex != -1, "Invalid state id" );
	hkbStateMachine::StateInfo* si = m_states[stateIndex];
	return *si;
}

HK_FORCE_INLINE const hkbStateMachine::StateInfo& hkbStateMachine::getStateInfoById( int stateId ) const
{
	int stateIndex = getStateIndex( stateId );
	HK_ASSERT2( 0x17af3d9c, stateIndex != -1, "Invalid state id" );
	hkbStateMachine::StateInfo* si = m_states[stateIndex];
	return *si;
}

HK_FORCE_INLINE bool hkbStateMachine::isInTransition() const
{
	return ( m_activeTransitions.getSize() > 0 );
}

HK_FORCE_INLINE bool hkbStateMachine::isReturnToPreviousStateTransition( TransitionInfo& t, hkbEvent::Id eventId )
{
	return (	( eventId != hkbEvent::EVENT_ID_NULL ) && 
				( eventId == m_returnToPreviousStateEventId ) &&
				( t.m_toStateId == m_previousStateId ) &&
				( m_previousStateId != m_currentStateId ) && // this indicates that there is no history
				( !( t.m_flags.get() & TransitionInfo::FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE ) ) );
}

HK_FORCE_INLINE bool hkbStateMachine::isCurrentNestedStateId( const hkbStateMachine* currentNestedStateMachine, int nestedStateId )
{
	return (	( currentNestedStateMachine != HK_NULL ) && 
				( nestedStateId == currentNestedStateMachine->getCurrentStateId() ) );
}

HK_FORCE_INLINE bool hkbStateMachine::shouldTransition(	const hkbStateMachine* currentNestedStateMachine,
														TransitionInfo& t, 
														hkbEvent::Id eventId, 
														int toStateId, 
														bool ignoreEvents, 
														bool ignoreToState,
														bool fromAnyState,
														bool isInTriggerInterval,
														const hkbContext& context )
{
	hkStringPtr errorStr;
	hkbStateMachine::StateInfo& toStateInfo = getStateInfoById( t.m_toStateId );
	return (	(	ignoreToState || ( t.m_toStateId == toStateId ) )

				&&

				( toStateInfo.m_enable && ( !context.getRootBehavior()->m_checkNodeValidity || (toStateInfo.m_generator != HK_NULL && toStateInfo.m_generator->isValid(context.getCharacter(), errorStr) ) ) )

				&&

				(	( !fromAnyState ) || 
					(	( t.m_flags.get() & TransitionInfo::FLAG_IS_LOCAL_WILDCARD ) &&
						(	( m_currentStateId != t.m_toStateId ) || 
							( t.m_flags.get() & TransitionInfo::FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE ) ) ) )

				&&

				(	!( t.m_flags.get() & TransitionInfo::FLAG_USE_TRIGGER_INTERVAL ) ||
					( isInTriggerInterval ) )

				&&

				(	!( t.m_flags.get() & TransitionInfo::FLAG_DISABLED ) )

				&&

				(	( ignoreEvents ) ||
					( t.m_eventId == eventId ) ||
					(	isReturnToPreviousStateTransition( t, eventId ) ) ||
					(	( eventId != hkbEvent::EVENT_ID_NULL ) &&
						( eventId == m_randomTransitionEventId ) &&
						( !( t.m_flags.get() & TransitionInfo::FLAG_DISALLOW_RANDOM_TRANSITION ) ) ) )

				&&

				(	!(t.m_flags.get() & TransitionInfo::FLAG_FROM_NESTED_STATE_ID_IS_VALID ) ||
					isCurrentNestedStateId( currentNestedStateMachine, t.m_fromNestedStateId ) )

				&&

				(	( t.m_condition == HK_NULL ) ||
					( t.m_flags.get() & TransitionInfo::FLAG_DISABLE_CONDITION ) ||
					( t.m_condition->isTrue( context ) ) )

				);
}

HK_FORCE_INLINE void hkbStateMachine::initTransitionInterval(	const hkbStateMachine::TimeInterval& interval, 
																TransitionInfo::InternalFlags inFlag, 
																TransitionInfo::InternalFlags& flags )
{
	// close the interval
	flags &= ~inFlag;

	// if the following condition is true, it either means that we're using
	// events and the first endpoint is missing, or we're using fixed times.
	if ( interval.m_enterEventId == hkbEvent::EVENT_ID_NULL )
	{
		// see if we're using events (with the first endpoint missing)
		if ( interval.m_exitEventId != hkbEvent::EVENT_ID_NULL )
		{
			flags |= inFlag;
		}
		// if we're not using events, see if the interval starts immediately
		else if ( interval.m_enterTime <= 0.0f )
		{
			flags |= inFlag;
		}
	}
}

HK_FORCE_INLINE hkbStateMachine::TransitionInfo::InternalFlags hkbStateMachine::initTransitionIntervals( const TransitionInfo& ti )
{
	TransitionInfo::InternalFlags flags = 0;

	if ( ti.m_flags.get() & TransitionInfo::FLAG_USE_INITIATE_INTERVAL )
	{
		initTransitionInterval( ti.m_initiateInterval, TransitionInfo::FLAG_INTERNAL_IN_INITIATE_INTERVAL, flags );
	}

	if ( ti.m_flags.get() & TransitionInfo::FLAG_USE_TRIGGER_INTERVAL )
	{
		initTransitionInterval( ti.m_triggerInterval, TransitionInfo::FLAG_INTERNAL_IN_TRIGGER_INTERVAL, flags );
	}

	return flags;
}

HK_FORCE_INLINE void hkbStateMachine::updateTransitionIntervalForTime(	hkReal timestep, 
																		const hkbStateMachine::TimeInterval& interval, 
																		TransitionInfo::InternalFlags inFlag, 
																		TransitionInfo::InternalFlags& flags )
{
	HK_ASSERT( 0x349120a0, interval.m_enterEventId == hkbEvent::EVENT_ID_NULL );
	HK_ASSERT( 0x359f20a0, interval.m_exitEventId == hkbEvent::EVENT_ID_NULL );

	// check for overlap between the elapsed time interval with this interval
	if (	( m_timeInState + timestep >= interval.m_enterTime ) &&
			(	( interval.m_exitTime <= 0.0f ) || // this indicates infinity (no exit time)
				( m_timeInState <= interval.m_exitTime ) ) )
	{
		flags |= inFlag;
	}
	else
	{
		flags &= ~inFlag;
	}
}

HK_FORCE_INLINE bool hkbStateMachine::TransitionInfo::isInitiateIntervalTimeBounded() const
{
	return (	( m_flags.get() & TransitionInfo::FLAG_USE_INITIATE_INTERVAL ) &&
				( m_initiateInterval.m_enterEventId == hkbEvent::EVENT_ID_NULL ) &&
				( m_initiateInterval.m_exitEventId == hkbEvent::EVENT_ID_NULL ) );
}

HK_FORCE_INLINE bool hkbStateMachine::TransitionInfo::isTriggerIntervalTimeBounded() const
{
	return (	( m_flags.get() & TransitionInfo::FLAG_USE_TRIGGER_INTERVAL ) &&
				( m_triggerInterval.m_enterEventId == hkbEvent::EVENT_ID_NULL ) &&
				( m_triggerInterval.m_exitEventId == hkbEvent::EVENT_ID_NULL ) );

}

HK_FORCE_INLINE void hkbStateMachine::updateTransitionIntervalsForTime(	hkReal timestep, 
																		const TransitionInfo& transitionInfo, 
																		TransitionInfo::InternalFlags& flags )
{
	// an interval is bounded by reals if and only if the event is null
	if ( transitionInfo.isInitiateIntervalTimeBounded() )
	{
		updateTransitionIntervalForTime(	timestep,
											transitionInfo.m_initiateInterval, 
											TransitionInfo::FLAG_INTERNAL_IN_INITIATE_INTERVAL, 
											flags );
	}

	if ( transitionInfo.isTriggerIntervalTimeBounded() )
	{
		updateTransitionIntervalForTime(	timestep,
											transitionInfo.m_triggerInterval, 
											TransitionInfo::FLAG_INTERNAL_IN_TRIGGER_INTERVAL, 
											flags );
	}
}

HK_FORCE_INLINE void hkbStateMachine::updateTransitionIntervalForEvent(	hkbEvent::Id eventId, 
																		const hkbStateMachine::TimeInterval& interval, 
																		TransitionInfo::InternalFlags inFlag, 
																		TransitionInfo::InternalFlags& flags )
{
	HK_ASSERT( 0x401bc7fe, eventId != hkbEvent::EVENT_ID_NULL );

	int mask = ( interval.m_enterEventId != eventId ) - 1;
	flags |= ( inFlag & mask );

	mask = ( interval.m_exitEventId == eventId ) - 1;
	flags &= ( ~inFlag | mask );

	// the above code is a branch-free way of doing this:
	//
	// if ( interval.m_enterEventId == eventId )
	// {
	// 	   flags |= inFlag;
	// }
	//
	// if ( interval.m_exitEventId == eventId )
	// {
	//	   flags &= ~inFlag;
	// }
}

HK_FORCE_INLINE void hkbStateMachine::updateTransitionIntervalsForEvent(	hkbEvent::Id eventId,
																			const TransitionInfo& transitionInfo,
																			TransitionInfo::InternalFlags& flags )
{
	// Note that we don't need to check whether events or fixed times are being used
	// to define the interval, because if fixed times are being used the events
	// will be set to EVENT_ID_NULL anyway so we won't get a match.
	if ( transitionInfo.m_flags.get() & TransitionInfo::FLAG_USE_INITIATE_INTERVAL )
	{
		updateTransitionIntervalForEvent(	eventId, 
											transitionInfo.m_initiateInterval, 
											TransitionInfo::FLAG_INTERNAL_IN_INITIATE_INTERVAL, 
											flags );
	}

	if ( transitionInfo.m_flags.get() & TransitionInfo::FLAG_USE_TRIGGER_INTERVAL )
	{
		updateTransitionIntervalForEvent(	eventId, 
											transitionInfo.m_triggerInterval, 
											TransitionInfo::FLAG_INTERNAL_IN_TRIGGER_INTERVAL, 
											flags );
	}
}

HK_FORCE_INLINE hkbStateMachine::TransitionInfo* hkbStateMachine::getTransitionInfo( const hkbBehaviorGraph& rootBehavior, const TransitionInfoReference& transitionInfoReference ) const
{
	HK_ASSERT2( 0x4671bc82, transitionInfoReference.m_fromStateIndex >= TransitionInfoReference::WILDCARD_STATE_INDEX, "invalid m_fromStateIndex" );
	HK_ASSERT2( 0x71b29b5a, transitionInfoReference.m_transitionIndex >= 0, "invalid m_transitionIndex" );

	if ( transitionInfoReference.m_fromStateIndex == TransitionInfoReference::WILDCARD_STATE_INDEX )
	{
		const hkbStateMachine* stateMachine =	(transitionInfoReference.m_stateMachineId == TransitionInfoReference::SAME_STATE_MACHINE_ID) ?
												this:
												rootBehavior.getStateMachineTemplateFromId( transitionInfoReference.m_stateMachineId );

		return &stateMachine->m_wildcardTransitions->m_transitions[transitionInfoReference.m_transitionIndex];
	}
	else
	{
		return &getStateInfoByIndex( transitionInfoReference.m_fromStateIndex ).getTransitionInfo( transitionInfoReference.m_transitionIndex );
	}
}

HK_FORCE_INLINE hkbTransitionEffect* hkbStateMachine::createTransitionEffect( const hkbContext& context, const TransitionInfoReference& transitionInfoReference ) const
{
	hkbBehaviorGraph& rootBehavior = *context.getRootBehavior();

	// Create our TE
	hkbTransitionEffect* te = static_cast<hkbTransitionEffect*>( getTransitionInfo(rootBehavior, transitionInfoReference)->m_transition->cloneNode( rootBehavior ) );

	// Fix up any bindings if the TE is "borrowed" from another behavior graph
	if ( transitionInfoReference.m_stateMachineId != TransitionInfoReference::SAME_STATE_MACHINE_ID && te->hasBindings() )
	{
		hkbBehaviorGraph* smBehavior = rootBehavior.getBehaviorTemplateFromStateMachineTemplate( static_cast<hkbStateMachine*>( rootBehavior.getNodeTemplate( this ) ) );
		hkbBehaviorGraph* teBehavior = rootBehavior.getBehaviorTemplateFromStateMachineTemplate( rootBehavior.getStateMachineTemplateFromId( transitionInfoReference.m_stateMachineId ) );
		if ( teBehavior != smBehavior )
		{
			te->computePatchedBindings( context, *teBehavior );
		}
	}
	
	// the state machine manages a reference to TEs because we clone them but in some cases they may not ever get used
	te->addReference();

	// assign a unique ID to the transition effect
	te->m_id = rootBehavior.getUniqueIdForTransitionEffect();

	return te;
}

HK_FORCE_INLINE const hkbStateMachine::TransitionInfo::InternalFlags* hkbStateMachine::getInternalFlags( const TransitionInfoReference& transitionInfoReference ) const
{
	HK_ASSERT2( 0x871c8d09, transitionInfoReference.m_fromStateIndex >= TransitionInfoReference::WILDCARD_STATE_INDEX, "invalid m_fromStateIndex" );
	HK_ASSERT2( 0x712bc8a0, transitionInfoReference.m_transitionIndex >= 0, "invalid m_transitionIndex" );

	if ( transitionInfoReference.m_fromStateIndex == TransitionInfoReference::WILDCARD_STATE_INDEX )
	{
		return &m_wildcardTransitionFlags[transitionInfoReference.m_transitionIndex];
	}
	else
	{
		return &m_transitionFlags[transitionInfoReference.m_transitionIndex];
	}
}

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
