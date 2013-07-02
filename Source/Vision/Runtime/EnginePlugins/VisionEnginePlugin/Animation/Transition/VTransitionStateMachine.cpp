/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionManager.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimNormalizeMixerNode.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


// -------------------------------------------------------------------------- //
// State Anim Control                                                         //
// -------------------------------------------------------------------------- //

VisAnimSequence_cl* StateAnimControl_cl::GetCustomNextAnimSeq()
{
  return m_pCustomNextAnimSeq;
}

void StateAnimControl_cl::SetCustomNextAnimSeq(VisAnimSequence_cl *sequence)
{
  m_pCustomNextAnimSeq = sequence;
}

V_IMPLEMENT_SERIAL( StateAnimControl_cl, VisSkeletalAnimControl_cl, 0, &g_VisionEngineModule);
void StateAnimControl_cl::Serialize( VArchive &ar )
{
  VisSkeletalAnimControl_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    int iVersion;
    ar >> iVersion;
    ar >> m_iMixerInputIndex;
  }
  else
  {
    ar << (int)0; // Version

    ar << m_iMixerInputIndex;
  }
}


// -------------------------------------------------------------------------- //
// Transition State Machine                                                   //
// -------------------------------------------------------------------------- //

VTransitionStateMachine::VTransitionStateMachine() 
  : m_pEventListener(16,NULL)
{
  VTransitionManager::GlobalManager().Instances().Add(this);
  
  m_pSequence = NULL;

  m_iEventListenerCount = 0;
  m_iActiveEventID = 0;
  m_bForwardAnimEvents = false;

  m_pEntity = NULL;
  m_pMesh = NULL;
  m_spAnimConfig = NULL;
  m_pTransition = NULL;
  m_pSequence = NULL;
  m_eIntermediateFollowUpTrigger = TRANSITION_TRIGGER_IMMEDIATE;

  m_fTransitionTime = 0.0f;

  m_bEnabled = true;
  m_bCreateAnimConfig = true;
  m_bIsBlending = false;
  m_bSyncBlending = false;
  m_bProcessOffsetDelta = false;
  
  m_bFinishBlending = false;
  m_bFinishSequence = false;
  m_bSyncSerialize = false;
  m_bIsIntermediateBlending = false;
  m_bStateChanged = false;
}

VTransitionStateMachine::~VTransitionStateMachine()
{	

}

void VTransitionStateMachine::DisposeObject()
{
  VTransitionManager::GlobalManager().Instances().SafeRemove(this);
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void VTransitionStateMachine::GetDependencies(VResourceSnapshot &snapshot)
{
  snapshot.AddDependency(m_spTransTable,NULL,false);
}
#endif

// -------------------------------------------------------------------------- //
// Initialization / Deinitialization                                          //
// -------------------------------------------------------------------------- //

void VTransitionStateMachine::Init(VisBaseEntity_cl *pEntity, VTransitionTable *pTable, bool bCreateAnimConfig)
{
  // Set parent entity
  m_pMesh = NULL;
  m_pEntity = pEntity;
  m_spTransTable = pTable;
  m_bCreateAnimConfig = bCreateAnimConfig;

  // Set Transition Filename when Transition Table was loaded from file
  if (pTable->GetFilename())
    TransitionTableFile = pTable->GetFilename();

  // Everything set
  if (m_pEntity==NULL || m_spTransTable==NULL || m_pEntity->GetMesh()==NULL || m_pEntity->GetMesh()->GetSkeleton() == NULL)
    return;

  m_pMesh = pEntity->GetMesh();

  // Setup the normalize mixer node that blends the two animations
  m_spNormalizeMixer = new VisAnimNormalizeMixerNode_cl(m_pMesh->GetSkeleton());

  // Set blending state
  m_bIsBlending = false;
  m_bSyncBlending = false;
  m_bFinishBlending = false;
  m_bFinishSequence = false;

  m_bSyncSerialize = false;
  m_bIsIntermediateBlending = false;

  // Create an animation config for the entity and connect it, if specified
  if (m_bCreateAnimConfig)
  {
    // Setup animation system
    VisAnimFinalSkeletalResult_cl* pFinalResult;
    if (m_pEntity->GetAnimConfig() == NULL)
    { 
      m_spAnimConfig = VisAnimConfig_cl::CreateSkeletalConfig(m_pMesh, &pFinalResult);
    }
    else
    {
      // Re-use existing
      m_spAnimConfig = m_pEntity->GetAnimConfig();
      pFinalResult = m_spAnimConfig->GetFinalResult();
    }

    // Couple transition state machine animation node to final result
    pFinalResult->SetSkeletalAnimInput(GetTransitionMixer());

    // Setup animation config in the owner entity
    m_pEntity->SetAnimConfig(m_spAnimConfig);

    // Set the initial animation state
    VisAnimSequence_cl *pSequence = NULL;
    if (!InitialAnimation.IsEmpty())
    {
      // Get new animation sequence 
      //try to search for the initial animSequence in the sequencesets of transitiontable first
      if (m_spTransTable)
      {
        pSequence = m_spTransTable->GetSequence(InitialAnimation);
      }
      if (!pSequence)
      {
        pSequence = m_pMesh->GetSequence(InitialAnimation);
      }
    }

    if (pSequence == NULL)
    {         
      // Fallback: Get first animation that is available in transitiontable
      if (pTable->GetNumSequenceDefs() > 0 && pTable->GetSequenceDefByIndex(0) && pTable->GetSequenceDefByIndex(0)->GetOwnerSequence()) 
      {
        pSequence = pTable->GetSequenceDefByIndex(0)->GetOwnerSequence();

        if (pSequence != NULL && (pSequence->GetType() != VIS_MODELANIM_SKELETAL))
          pSequence = NULL;   // Simple approach to solve incorrect animation failed
      }
    }

    SetState(pSequence);
  }
}

void VTransitionStateMachine::DeInit()
{
  if (m_spPrimaryStateAnimControl != NULL)
    m_spPrimaryStateAnimControl->RemoveEventListener(this);
  if (m_spSecondaryStateAnimControl != NULL)
    m_spSecondaryStateAnimControl->RemoveEventListener(this);

  m_SkeletalAnimControlList.Reset();

  m_spPrimaryStateAnimControl = NULL;
  m_spSecondaryStateAnimControl = NULL; 
  m_spNormalizeMixer = NULL;
  m_spTransTable = NULL;
  m_pTransition = NULL;
  m_pSequence = NULL;
  m_pEntity = NULL;
  m_pMesh = NULL;
}

bool VTransitionStateMachine::IsInitialized() const
{
  return (m_pMesh != NULL);
}

void VTransitionStateMachine::SetEnabled(BOOL bEnabled)
{
  m_bEnabled = bEnabled;

  if (bEnabled)
  {
    if (m_spAnimConfig != NULL && m_spNormalizeMixer != NULL)
      m_spAnimConfig->GetFinalResult()->SetSkeletalAnimInput(m_spNormalizeMixer);
    if (m_spPrimaryStateAnimControl != NULL)
      m_spPrimaryStateAnimControl->Resume();
    if (m_spSecondaryStateAnimControl != NULL)
      m_spSecondaryStateAnimControl->Resume();
  }
  else
  {
    if (m_spPrimaryStateAnimControl != NULL)
      m_spPrimaryStateAnimControl->Pause();
    if (m_spSecondaryStateAnimControl != NULL)
      m_spSecondaryStateAnimControl->Pause();
    if (m_spAnimConfig != NULL)
      m_spAnimConfig->GetFinalResult()->SetSkeletalAnimInput(NULL);
  }
}

void VTransitionStateMachine::OnThink()
{
  if (!m_bEnabled || !m_bIsBlending || m_spNormalizeMixer == NULL)
    return;

  // When blending process is finished fire transition finished event
  if (!m_spNormalizeMixer->IsEasingIn(m_spPrimaryStateAnimControl->m_iMixerInputIndex))
    TriggerEvent(EVENT_TRANSITION_FINISHED);

  // Calculate the offset delta for immediate transition manually
  if (m_pEntity->GetAnimConfig()->GetFlags() & APPLY_MOTION_DELTA &&  m_bProcessOffsetDelta)
    ProcessOffsetDelta();
}

void VTransitionStateMachine::OnEvent(INT_PTR iEvent)
{
  if (m_pOwner == NULL)
    return;

  // Sequence has finished
  if(m_pSequence)
  { 
    if (iEvent == EVENT_SEQUENCE_FINISHED)
    {
      // Intermediate blending mode, check whether the follow-up trigger is EndOfAnim
      // and perform the state change accordingly with the State stored in the
      // CustomNextSequence member of the responsible AnimControl instance
      if(m_bIsIntermediateBlending)
      {
        if(m_eIntermediateFollowUpTrigger == TRANSITION_TRIGGER_ENDOFANIM)
        {
          // Set the new state for the last part of the intermediate animation
          SetState(m_spPrimaryStateAnimControl->GetCustomNextAnimSeq());

          // Remove flag as we now set the last transition
          // blending sequence -> target sequence
          m_bIsIntermediateBlending = false;
        }
      }
      else
      {// Either check whether this sequence has a follow-up sequence.
        // Or remove the EVENT_SEQUENCE_FINISHED event manually. 
        if (!m_bIsBlending && m_pSequence->GetAnimationEndType() != ANIMATIONEND_TYPE_LOOP && m_pSequence->GetNumFollowUpAnimations() > 0)
        {
          SetState(m_pSequence->GetFollowUpAnimationRandom());
        }
        else 
        {
          // The event was ignored by the TSM, so we remove it and return from the function.
          m_spPrimaryStateAnimControl->GetEventList()->RemoveEvents(EVENT_SEQUENCE_FINISHED);
          return;
        }
      }   
    }    
  }

  if (m_pTransition)
  {
    // Blending has started
    if (iEvent == EVENT_TRANSITION_STARTED)
    {
      // Reset previous ease in / out components
      m_spNormalizeMixer->RemoveEaseIn(m_spSecondaryStateAnimControl->m_iMixerInputIndex);
      m_spNormalizeMixer->RemoveEaseOut(m_spPrimaryStateAnimControl->m_iMixerInputIndex);

      // Get current ease values
      float fPrimaryEaseValue = m_spNormalizeMixer->GetCurrentEaseValue(m_spPrimaryStateAnimControl->m_iMixerInputIndex);
      float fSecondaryEaseValue = m_spNormalizeMixer->GetCurrentEaseValue(m_spSecondaryStateAnimControl->m_iMixerInputIndex);

      // Get relative transition duration time. When we are switching back to the 
      // secondary animation state during the blending process state we don't use 
      // the complete transition time but rather the remaining transition time.
      float fTransitionTime = m_pTransition->m_fBlendDuration; /* fSecondaryEaseValue; */

      switch (m_pTransition->m_eType)
      {
      case TRANSITION_TYPE_IMMEDIATE:
        {
          // Stop source sequence and start fading out
          m_spSecondaryStateAnimControl->Pause();
          m_spNormalizeMixer->SetEaseOut(m_spSecondaryStateAnimControl->m_iMixerInputIndex, EASE_LINEAR, 0.0f, fTransitionTime, fSecondaryEaseValue, 0.0f);

          // Set target sequence to beginning and start fading in 
          m_spPrimaryStateAnimControl->Pause();		
          m_spNormalizeMixer->SetEaseIn(m_spPrimaryStateAnimControl->m_iMixerInputIndex, EASE_LINEAR, 0.0f, fTransitionTime, fPrimaryEaseValue, 1.0f);

          // Value that determines if offset delta has to be calculated manually
          m_bProcessOffsetDelta = true;
        }	
        break;

      case TRANSITION_TYPE_CROSSFADE:
        {
          // Continue playing source sequence and start fading out
          m_spNormalizeMixer->SetEaseOut(m_spSecondaryStateAnimControl->m_iMixerInputIndex, EASE_LINEAR, 0.0f, fTransitionTime, fSecondaryEaseValue, 0.0f);

          // Start target sequence and start fading in
          m_spPrimaryStateAnimControl->Resume();				
          m_spNormalizeMixer->SetEaseIn(m_spPrimaryStateAnimControl->m_iMixerInputIndex, EASE_LINEAR, 0.0f, fTransitionTime, fPrimaryEaseValue, 1.0f);

          // Value that determines if offset delta has to be calculated manually
          m_bProcessOffsetDelta = false;
        }
        break;

      case TRANSITION_TYPE_IMMEDIATE_SYNC:
        {
          // Stop source sequence at event time and start fading out
          m_spSecondaryStateAnimControl->Pause(); 
          m_spNormalizeMixer->SetEaseOut(m_spSecondaryStateAnimControl->m_iMixerInputIndex, EASE_LINEAR, 0.0f, fTransitionTime, fSecondaryEaseValue, 0.0f);

          // Set target sequence to event time and pause sequence
          float fEventTime = ((VisAnimSequence_cl*) m_spPrimaryStateAnimControl->GetAnimSequence())->GetEventList()->GetFirstEventTime(m_iActiveEventID);
          m_spPrimaryStateAnimControl->SetCurrentSequenceTime(fEventTime);
          m_spPrimaryStateAnimControl->Pause();				
          m_spNormalizeMixer->SetEaseIn(m_spPrimaryStateAnimControl->m_iMixerInputIndex, EASE_LINEAR, 0.0f, fTransitionTime, fPrimaryEaseValue, 1.0f);

          // Value that determines if offset delta has to be calculated manually
          m_bProcessOffsetDelta = true;
        }
        break;

      case TRANSITION_TYPE_CROSSFADE_SYNC:
        {
          // Continue playing source sequence and start blending at event time 
          m_spNormalizeMixer->SetEaseOut(m_spSecondaryStateAnimControl->m_iMixerInputIndex, EASE_LINEAR, 0.0f, fTransitionTime, fSecondaryEaseValue, 0.0f);

          // Start playing target sequence at event time and start fading in
          float fEventTime = ((VisAnimSequence_cl*)m_spPrimaryStateAnimControl->GetAnimSequence())->GetEventList()->GetFirstEventTime(m_iActiveEventID);
          m_spPrimaryStateAnimControl->SetCurrentSequenceTime(fEventTime);
          m_spPrimaryStateAnimControl->Resume();				
          m_spNormalizeMixer->SetEaseIn(m_spPrimaryStateAnimControl->m_iMixerInputIndex, EASE_LINEAR, 0.0f, fTransitionTime, fPrimaryEaseValue, 1.0f);

          // Value that determines if offset delta has to be calculated manually
          m_bProcessOffsetDelta = false;
        }
        break;
      }

      m_bIsBlending = true;
      m_bSyncBlending = false;
      m_fTransitionTime = 0.0f;
    }
    // Blending has finished
    else if (iEvent == EVENT_TRANSITION_FINISHED)
    {
      
      // Update flag for new anim control.
      // The transition has finished and therefore the loop flag is now set according to the sequence.
      if(m_pSequence != NULL)
      {
        unsigned int flag = m_spPrimaryStateAnimControl->GetFlags();
        if (m_pSequence->GetAnimationEndType() == ANIMATIONEND_TYPE_LOOP || m_pSequence->GetNumFollowUpAnimations() == 0 || m_bIsIntermediateBlending)
        {
          flag |= VANIMCTRL_LOOP;
        }
        else 
        {
          flag &= ~VANIMCTRL_LOOP;
          // We need to add the EVENT_SEQUENCE_FINISHED event as we are no longer looping.
          if (m_spPrimaryStateAnimControl->GetEventList()->GetFirstEventTime(EVENT_SEQUENCE_FINISHED) == -1.0f)
            m_spPrimaryStateAnimControl->GetEventList()->AddEvent(m_spPrimaryStateAnimControl->GetAnimSequence()->GetLength(), EVENT_SEQUENCE_FINISHED, false);

        }
        m_spPrimaryStateAnimControl->SetFlags(flag);
      }

      switch (m_pTransition->m_eType)
      {
      case TRANSITION_TYPE_IMMEDIATE:
        {
          // Start target sequence
          m_spPrimaryStateAnimControl->Resume();	
        }	
        break;

      case TRANSITION_TYPE_CROSSFADE:
        {
          // Stop source sequence
          m_spSecondaryStateAnimControl->Pause();
        }
        break;

      case TRANSITION_TYPE_IMMEDIATE_SYNC:
        {
          // Start target sequence
          m_spPrimaryStateAnimControl->Resume();
        }
        break;

      case TRANSITION_TYPE_CROSSFADE_SYNC:
        {
          // Stop source sequence
          m_spSecondaryStateAnimControl->Pause();
        }
        break;
      }


      // Execute transition settings clearance before new setting of the followup Intermediate animation
      m_fTransitionTime = 0.0f;
      m_bIsBlending = false;
      m_bSyncBlending = false;
      m_bProcessOffsetDelta = false;
      m_bFinishBlending = false;
      m_bFinishSequence = false; 

      // Remove all inactive animation controls
      RemoveInactiveStateAnimControls();

      // Only perform variable reset if we didn't do a SetState in TYPE_INTERMEDIATE
      if(m_bIsIntermediateBlending)
      {
        // Intermediate Animation used as blending
        // Set target animation according to trigger
        if(m_eIntermediateFollowUpTrigger == TRANSITION_TRIGGER_IMMEDIATE)
        {   
          // Start target animation immediately
          if(m_spPrimaryStateAnimControl->GetCustomNextAnimSeq())
          {
            SetState(m_spPrimaryStateAnimControl->GetCustomNextAnimSeq());
            m_spPrimaryStateAnimControl->SetCustomNextAnimSeq(NULL);
          }
          m_bIsIntermediateBlending = false;
        }
      } // endif (bIsBlendTypeIntermediate)
    } //endif (iEvent == EVENT_TRANSITION_FINISHED)
  }  //endif (m_pTransition != NULL)

  // For intermediate transition listen to EVENT_TRANSITION_START_REACHED
  if(iEvent == EVENT_TRANSITION_START_REACHED)
  {
    // To be on the save side check again if we really have a follow-up
    // trigger of type BEFORENDOFANIMATION
    if(m_eIntermediateFollowUpTrigger == TRANSITION_TRIGGER_BEFOREENDOFANIM)
    {
      //Setup everything for BeforeEndOfAnim trigger
      //Start target animation immediately as we reached the correct time point
      SetState(m_spPrimaryStateAnimControl->GetCustomNextAnimSeq());
    }    
  }

  // Process event to all registered event listeners
  // TODO: Check for correct behavior, when should it be actually executed? Is it required?
  SendToAllListeners(VIS_MSG_TRANSITIONSTATEMACHINE, iEvent, (INT_PTR)this); 
} 

// -------------------------------------------------------------------------- //
// State Handling                                                             //
// -------------------------------------------------------------------------- //

void VTransitionStateMachine::SetState(VisAnimSequence_cl *pTargetSequence) 
{
  if (!IsInitialized())
    return;

  // Do not set a state if disabled
  if (!m_bEnabled)
    return;

  // Check for null pointer
  if (!pTargetSequence)
    return;

  // When finish blending or finish sequence property is set we don't need to 
  // process any state handling.
  if (m_bFinishBlending || m_bFinishSequence)
    return;

  // If TSM is waiting for an event we don't process immediate state switches as it might 
  // cause unexpected results
  if (m_bSyncBlending)
    return;

  // Remember state change for network sync
  m_bStateChanged = true;

  // Check whether active animation sequence differs from new target animation 
  // sequence. If not, we don't need to do any state handling.
  if (m_spPrimaryStateAnimControl && m_spPrimaryStateAnimControl->GetAnimSequence() == pTargetSequence)
  {
    // Add event to get notified when sequence finishes. Here we need to add the event again
    // in case the sequence is playing in loop. The event is usually removed after one loop.
    if (m_spPrimaryStateAnimControl->GetEventList()->GetFirstEventTime(EVENT_SEQUENCE_FINISHED) == -1.0f)
      m_spPrimaryStateAnimControl->GetEventList()->AddEvent(pTargetSequence->GetLength(), EVENT_SEQUENCE_FINISHED, false);

    // If we set the same state again, we reset the sequence and play it back again
    if (!m_spPrimaryStateAnimControl->IsPlaying())
      m_spPrimaryStateAnimControl->Play(true);
    return;
  }

  // Initial state
  if (m_spPrimaryStateAnimControl == NULL)
  {
    m_spPrimaryStateAnimControl = GetFreeStateAnimControl(pTargetSequence);

    // Setup loop. We loop if ANIMATIONEND_TYPE_LOOP is set or if there are no follow up sequences
    VSequenceDef* pSeqDef = m_spTransTable->GetSequenceDef(pTargetSequence);
    unsigned int flag = m_spPrimaryStateAnimControl->GetFlags();  
    if (pSeqDef == NULL || pSeqDef->GetAnimationEndType() == ANIMATIONEND_TYPE_LOOP || pSeqDef->GetNumFollowUpAnimations() == 0)
    {
      flag |= VANIMCTRL_LOOP;
    }
    else 
    {
      flag &= ~VANIMCTRL_LOOP;
    }
    m_spPrimaryStateAnimControl->SetFlags(flag);

    m_spPrimaryStateAnimControl->Play(true);
    m_spNormalizeMixer->SetInputWeight(m_spPrimaryStateAnimControl->m_iMixerInputIndex, 1.0f);
  
    // Add event to get notified when sequence finishes (unless the event has been added already)
    if (m_spPrimaryStateAnimControl->GetEventList()->GetFirstEventTime(EVENT_SEQUENCE_FINISHED) == -1.0f)
      m_spPrimaryStateAnimControl->GetEventList()->AddEvent(pTargetSequence->GetLength(), EVENT_SEQUENCE_FINISHED, false);
  }
  // State switch
  else
  {
    // Check whether transition table has any transition data ( todo: put this initial )
    if (m_spTransTable->GetNumTransitionDefs() == 0 || m_spTransTable->GetNumTransitionSets() == 0)
    {
      Vision::Error.Warning("Transition Data is not initialized properly. Check whether the proper Transition File is applied.");
      return;
    }

    // Get active animation sequence
    VisAnimSequence_cl *pSourceSequence = (VisAnimSequence_cl*) m_spPrimaryStateAnimControl->GetAnimSequence();
    VASSERT(pSourceSequence);

    // When the TSM is currently blending inside a immediate synced transition and we want to 
    // switch to a new state we need to make sure to finish the old transition.
    if (m_bIsBlending && m_pTransition && m_pTransition->m_eType == TRANSITION_TYPE_IMMEDIATE_SYNC)
      TriggerEvent(EVENT_TRANSITION_FINISHED);

    // Lookup transition from source to target sequence
    m_pTransition = m_spTransTable->GetTransitionDef(pSourceSequence, pTargetSequence);
    if (!m_pTransition || m_pTransition->m_eType == TRANSITION_TYPE_NONE)
      return;

    StateAnimControl_cl *pOldAnimControl = NULL;
    StateAnimControl_cl *pNewAnimControl = NULL;
    m_bFinishBlending = m_pTransition->m_bFinishBlending;

    // Check whether it is a "Intermediate Animation"
    if(m_pTransition->m_eType == TRANSITION_TYPE_INTERMEDIATE)
    {
      VisAnimSequence_cl* pFinalTargetSequence = pTargetSequence;
      VTransitionDef* eIntermediateTransition = m_pTransition;

      // Check whether the intermediate animation sequence is set
      VASSERT(m_pTransition->m_BlendSequence != NULL);      

      // Set blending animation sequence as custom next animation
      // to be played after the blending animation
      pTargetSequence = m_pTransition->m_BlendSequence;
      
      // Get old and new animation control, new animation control is based on
      // intermediate animation as stored in the VTransitionDef
      pOldAnimControl = m_spPrimaryStateAnimControl;
      pNewAnimControl = GetFreeStateAnimControl(pTargetSequence);

      // Keep the final target sequence actually the target of the intermediate transition definition
      pNewAnimControl->SetCustomNextAnimSeq(pFinalTargetSequence);

      // If we have the FollowupTrigger for the next transition as a BeforeEndOfAnimation then we register
      // an event to the correct time position, so we know when to trigger the final target transition
      VTransitionDef* pFollowupTransition = m_spTransTable->GetTransitionDef(pTargetSequence, pFinalTargetSequence);
      
      // Store the follow-up trigger for later usage in VTransitionStateMachine::OnEvent
      m_eIntermediateFollowUpTrigger = m_pTransition->m_eFollowupTrigger;

      // Assign the TransitionDef of the actually performed sub-blending to the according member variable
      m_pTransition = m_spTransTable->GetTransitionDef(pSourceSequence, pTargetSequence);

      // In case of an invalid transition table this can actually be NULL.
      if (m_pTransition == NULL)
      {
      #ifdef HK_DEBUG
        VString vTemp;
        vTemp.Format("The intermediate transition (A->B->C) from A (%s) to C (%s) has an invalid configuration from A to B (%s)",
          pSourceSequence->GetName(), pFinalTargetSequence->GetName(), pTargetSequence->GetName());
        VASSERT_MSG(m_pTransition, vTemp.AsChar());
      #endif
        return;
      }

      if (eIntermediateTransition->m_eFollowupTrigger == TRANSITION_TRIGGER_BEFOREENDOFANIM)
      {
        // Calculate the time where to place the animation event to correctly trigger the blending between
        // blend sequence -> target sequence of the intermediate animation:
        //  - The correct time is Length(BlendSequence) - BlendDuration(TargetSequence)
        //  - If the subtraction is < 0 we place the event at time point 0
        float time = pNewAnimControl->GetAnimSequence()->GetLength() - pFollowupTransition->m_fBlendDuration;
        if(time < 0)
          time = 0;

        //Determine if the blend durations of the two blendings of the Intermediate Animation Transition
        //is longer than the sequence length of the intermediate blend sequence
        //If this is the case we need to deactivate bFinishBlending for the transition between
        //pSource -> blend sequence. Otherwise the triggering of the event EVENT_TRANSITION_START_REACHED
        //would be without effect. With the deactivation of bFinishBlending we generate an artifact in the 
        //animation blending causing a hard abort and switch to the transition (blend sequence -> pTarget)
        //Therefore we write a warning message to the vision logfile for the customer to check.
        //TODO: Problem is what happens if the m_bFinishBlending flag is reset by the value in m_pTransition?
        //      ANSWER: This does not happen in current code. However if this going to be introduced at some point
        //              one solution could be to change the m_pTransition member from pointer to object type
        //              and thus create a copy of the transition, as from the current perspective it is not
        //              necessary to keep a pointer of the transition, but an object copy should be sufficient.

        float eventTime =10000.0f; //Set value initially high as we compare eventTime to be smaller than the blend sum
        if(pFollowupTransition->m_eType == TRANSITION_TYPE_CROSSFADE_SYNC)
        {     
          //Determine the sync event and check if its time is < Transition(source->Blend).m_fBlendDuration
          VisAnimSequence_cl *seq = (VisAnimSequence_cl*)pNewAnimControl->GetAnimSequence();
          VisAnimEventList_cl* srcEventLst = seq->GetEventList();
          VisAnimEventList_cl *trgEventLst = pFinalTargetSequence->GetEventList();  

          srcEventLst->PrepareCurrentEventSet(0, seq->GetLength());

          VisAnimEvent_cl * syncEvent = NULL;

          for(int srcIndex=0; srcIndex < srcEventLst->GetEventCount(); srcIndex++)
          {
            VisAnimEvent_cl *srcEvent = srcEventLst->GetNextEvent();
            if(srcEvent)
            {
              trgEventLst->PrepareCurrentEventSet(0, pFinalTargetSequence->GetLength());
              for(int trgIndex=0; trgIndex < trgEventLst->GetEventCount();trgIndex++)
              {
                VisAnimEvent_cl *trgEvent = trgEventLst->GetNextEvent();
                if(srcEvent->IsStringEvent() && trgEvent->IsStringEvent())
                {
                  if(trgEvent && srcEvent->GetEventString() == trgEvent->GetEventString())
                  {
                    syncEvent = srcEvent;
                    break;
                  }
                }
              }
              if(syncEvent)
              {
                break;
              }
            }
          }
          
          //This check for NULL-pointer has been introduced in reply to HAnSoft tickets 2860 & 2862
          //In principal if the follow-up transition is a Crossfade-synced there the above search for the events should always
          //return a valid synced-Event. If however for any reason the above loop exists with NULL-pointer this will be caught
          //here to avoid invalid memory access exceptions. As a consequence of the failure of finding the matching events this 
          //might cause the second sub-blending of the Intermediate Transition not to trigger. To make the user aware of the problem
          //an according message is printed to the error log.
          if(syncEvent)
          {
            eventTime = syncEvent->fTimeValue;
          }          
          else
          {
            VString message = VString("The configuration of the Intermediate Transition indicate that you are using a synced Crossfade");
            message += " as second sub-blending and the follow-up trigger BEFOREENDOFANIM. However no matching synced events for the synced Crossfade";
            message += " could be found. This can cause invalid behavior of the Intermediate transition, e.g. the second sub-blending not starting.";
            Vision::Error.Warning(message.AsChar());
          }
        }
        
        if((pFollowupTransition->m_fBlendDuration + m_pTransition->m_fBlendDuration >= pNewAnimControl->GetAnimSequence()->GetLength()) || eventTime <= m_pTransition->m_fBlendDuration)
        {
          m_bFinishBlending = false;
          VString message = VString("The summed up blend durations of the sub-blendings of the Intermediate Transition from ");
          message += pOldAnimControl->GetAnimSequence()->GetName();
          message += " to ";
          message += pFinalTargetSequence->GetName();
          message += " is exceeding the blend sequence's sequence duration, which in conjunction with TRANSITION_TRIGGER_BEFOREENDOFANIM";
          message += " is not possible. Therefore the FinishBlending flag on the transition between ";
          message += pOldAnimControl->GetAnimSequence()->GetName();
          message += " and ";
          message += pNewAnimControl->GetAnimSequence()->GetName();
          message += " is set to false and the blending will be aborted as soon as the start time for the final blending has been reached!";
          Vision::Error.Warning(message.AsChar());
        }
        pNewAnimControl->GetEventList()->AddEvent(time, EVENT_TRANSITION_START_REACHED, false);
      }
            
      // Set the flag to indicate we are in Intermediate blending mode
      m_bIsIntermediateBlending = true;   
    }
    else
    {
      // Get old and new animation control. Make we sure we cancel any previous intermediate animation 
      // flags and set all CustomNextAnimSequences to NULL
      m_bIsIntermediateBlending = false;

      // We can not always assume the secondary AnimControl to be a valid value thus check it for NULL
      if(m_spSecondaryStateAnimControl)
        m_spSecondaryStateAnimControl->SetCustomNextAnimSeq(NULL);

      // On the other side the primary AnimControl is always valid, as it gets at least created upon 
      // entrance of this method
      m_spPrimaryStateAnimControl->SetCustomNextAnimSeq(NULL);
      pOldAnimControl = m_spPrimaryStateAnimControl;
      pNewAnimControl = GetFreeStateAnimControl(pTargetSequence);
    }

    // Until the transition has finished, both animations have to loop as we can't handle follow-up
    // animation during a transition (EVENT_SEQUENCE_FINISHED is ignored) and so any affected animation would simply stop playing.
    // We set the correct flags of primary animation control in the EVENT_TRANSITION_FINISHED event.
    if (m_pTransition->m_eType != TRANSITION_TYPE_IMMEDIATE)
    {
      pNewAnimControl->SetFlags(pNewAnimControl->GetFlags() | VANIMCTRL_LOOP);
      pOldAnimControl->SetFlags(pOldAnimControl->GetFlags() | VANIMCTRL_LOOP);

      // In case this is a follow-up sequence, the animation has stopped.
      if (!pOldAnimControl->IsPlaying())
        pOldAnimControl->Play(true);
    }
    else
    {
      pNewAnimControl->SetFlags(pNewAnimControl->GetFlags() &~VANIMCTRL_LOOP);
      pOldAnimControl->SetFlags(pOldAnimControl->GetFlags() &~VANIMCTRL_LOOP);
    }

    // Update animation controls. For non-synchronized transition the active/inactive state
    // is updated immediately. For synchronized we switch the active/inactive state after 
    // the animation event (that triggers the synchronized transition) has been triggered.
    if (m_pTransition->m_eType == TRANSITION_TYPE_IMMEDIATE || m_pTransition->m_eType == TRANSITION_TYPE_CROSSFADE)
    {
      m_spSecondaryStateAnimControl = pOldAnimControl;
      m_spPrimaryStateAnimControl = pNewAnimControl;
    }
    else
    {
      m_spSecondaryStateAnimControl = pNewAnimControl;
      m_spPrimaryStateAnimControl = pOldAnimControl;
    }

    // Add event to get notified when sequence finishes
    pNewAnimControl->GetEventList()->AddEvent(pTargetSequence->GetLength(), EVENT_SEQUENCE_FINISHED, false);

    // Fire event that blending can be processed
    TriggerEvent(EVENT_TRANSITION_STARTED);
  }

  // Animation behavior
  // Check whether we actually have any transition data loaded already
  if (m_spTransTable->GetNumSequenceDefs() <= 0)
    return;

  // Do not allow SetState for animation sequences that are not covered by our transition table
  // (for example, sequences from another animation set)
  m_pSequence = m_spTransTable->GetSequenceDef(pTargetSequence);
  if (m_pSequence == NULL)
    return;

  m_bFinishSequence = m_pSequence->GetFinishSequence();
}

void VTransitionStateMachine::SetState(const char* szName) 
{
  if (!IsInitialized())
    return;

  // Get new animation sequence 
  VisAnimSequence_cl *pTargetSequence = NULL;
  // Try to search for animSequence in the sequence sets of transition table first
  if (m_spTransTable)
  {
    pTargetSequence = m_spTransTable->GetSequence(szName, VIS_MODELANIM_SKELETAL);
  }
  // If it could not be found in the sequenceSets of the transition table, try to find it in the model
  if (!pTargetSequence)
  {
    pTargetSequence = m_pMesh->GetSequence(szName, VIS_MODELANIM_SKELETAL);
  }

  if (pTargetSequence)
    SetState(pTargetSequence);
}

VisAnimSequence_cl* VTransitionStateMachine::GetActiveState() 
{
  if (m_spPrimaryStateAnimControl && m_spPrimaryStateAnimControl->GetAnimSequence() != NULL)
    return (VisAnimSequence_cl*) m_spPrimaryStateAnimControl->GetAnimSequence();
  else
    return NULL;
}

VisAnimSequence_cl* VTransitionStateMachine::GetInactiveState() 
{
  if (m_spSecondaryStateAnimControl && m_spSecondaryStateAnimControl->GetAnimSequence() != NULL)
    return (VisAnimSequence_cl*) m_spSecondaryStateAnimControl->GetAnimSequence();
  else
    return NULL;
}

VisSkeletalAnimControl_cl* VTransitionStateMachine::GetActiveControl()
{
  if (m_spPrimaryStateAnimControl != NULL)
    return m_spPrimaryStateAnimControl;
  else
    return NULL;
}

VisSkeletalAnimControl_cl* VTransitionStateMachine::GetInactiveControl()
{
  if (m_spSecondaryStateAnimControl != NULL)
    return m_spSecondaryStateAnimControl;
  else
    return NULL;
}

bool VTransitionStateMachine::IsBlending()
{
  return m_bIsBlending;
}

bool VTransitionStateMachine::IsWaitingForSyncBlending()
{
  return m_bSyncBlending;
}


// -------------------------------------------------------------------------- //
// Offset Delta                                                            //
// -------------------------------------------------------------------------- //

void VTransitionStateMachine::ProcessOffsetDelta()
{
  // If both source and target sequences don't have any motion delta we don't need to proceed
  if (!m_spPrimaryStateAnimControl->GetAnimSequence()->HasOffsetDelta() && !m_spSecondaryStateAnimControl->GetAnimSequence()->HasOffsetDelta())
    return;

  m_fTransitionTime = Vision::GetTimer()->GetTimeDifference();

  hkvVec3 vSourceOffset(0.f, 0.f, 0.f);
  hkvVec3 vTargetOffset(0.f, 0.f, 0.f);

  // Motion delta of source sequence
  if (m_spSecondaryStateAnimControl->GetAnimSequence()->HasOffsetDelta())
  {
    // Get valid floor and ceiling key frames of the offset delta track [FloorKeyFrame .. CurrentSequenceTime .. CeilingKeyFrame]
    int iFloorID, iCeilingID;
    VisOffsetDeltaKeyFrameTrack_cl *pSourceTrack = m_spSecondaryStateAnimControl->GetAnimSequence()->GetOffsetDeltaTrack();
    pSourceTrack->GetValidFloorAndCeilingFrames(pSourceTrack->FindCeilingKeyframeIndex(m_spSecondaryStateAnimControl->GetCurrentSequenceTime()), iFloorID, iCeilingID, m_spSecondaryStateAnimControl->GetFlags() & VANIMCTRL_LOOP);
    VisOffsetDeltaKeyFrame_cl* pCeilingFrame = pSourceTrack->GetOffsetDeltaKeyFrame(iCeilingID);
    VisOffsetDeltaKeyFrame_cl* pFloorFrame = pSourceTrack->GetOffsetDeltaKeyFrame(iFloorID);

    // Calculate the time factor dependent of the time passed since the last frame
    float fTimeFactor = (m_fTransitionTime) / (pCeilingFrame->m_fTime - pFloorFrame->m_fTime);
    vSourceOffset += pCeilingFrame->m_OffsetDelta * fTimeFactor * m_spNormalizeMixer->GetCurrentEaseValue(m_spSecondaryStateAnimControl->m_iMixerInputIndex);
  }

  // Motion delta of target sequence
  if (m_spPrimaryStateAnimControl->GetAnimSequence()->HasOffsetDelta())
  {
    // Get valid floor and ceiling key frames of the offset delta track [FloorKeyFrame .. CurrentSequenceTime .. CeilingKeyFrame]
    int iFloorID, iCeilingID;
    VisOffsetDeltaKeyFrameTrack_cl *pTargetTrack = m_spPrimaryStateAnimControl->GetAnimSequence()->GetOffsetDeltaTrack();
    pTargetTrack->GetValidFloorAndCeilingFrames(pTargetTrack->FindCeilingKeyframeIndex(m_spPrimaryStateAnimControl->GetCurrentSequenceTime()), iFloorID, iCeilingID, m_spPrimaryStateAnimControl->GetFlags() & VANIMCTRL_LOOP);
    VisOffsetDeltaKeyFrame_cl* pCeilingFrame = pTargetTrack->GetOffsetDeltaKeyFrame(iCeilingID);
    VisOffsetDeltaKeyFrame_cl* pFloorFrame = pTargetTrack->GetOffsetDeltaKeyFrame(iFloorID);

    // Calculate the time factor dependent of the time passed since the last frame
    float fTimeFactor = (m_fTransitionTime) / (pCeilingFrame->m_fTime - pFloorFrame->m_fTime);
    vTargetOffset += pCeilingFrame->m_OffsetDelta * fTimeFactor * m_spNormalizeMixer->GetCurrentEaseValue(m_spPrimaryStateAnimControl->m_iMixerInputIndex);
  }

  // Calculate the overall offset delta
  hkvVec3 vOffset = vSourceOffset + vTargetOffset;
  vOffset = (vOffset.x < 0.f || vOffset.y < 0.f || vOffset.z < 0.f) ? vOffset : -vOffset;
  m_pEntity->IncMotionDeltaLocalSpace(vOffset);
}

StateAnimControl_cl* VTransitionStateMachine::GetFreeStateAnimControl(VisAnimSequence_cl* pSequence)
{
  StateAnimControl_cl* pAnimControl = NULL;

  // Get free animation control if available or check whether given sequence is set in mixer
  for(int i = 0; i < m_SkeletalAnimControlList.Count(); i++)
  { 
    StateAnimControl_cl* pTempAnimControl = (StateAnimControl_cl*)m_SkeletalAnimControlList.GetAt(i);
    if(pTempAnimControl != NULL && (pTempAnimControl->m_iMixerInputIndex == -1))
    {
      pAnimControl = pTempAnimControl;
      // Remove any old events in case we reuse the control
      pAnimControl->GetEventList()->RemoveAllEvents();
      break; 
    }
    else if (pTempAnimControl != NULL && pTempAnimControl->GetAnimSequence() == pSequence)
    {
      // Remove any old events in case we reuse the control
      pTempAnimControl->GetEventList()->RemoveAllEvents();
      return pTempAnimControl;
    }
  }

  // If there is no free control left add a new one
  if(pAnimControl == NULL)
  {
    pAnimControl = new StateAnimControl_cl(m_pMesh->GetSkeleton());
    pAnimControl->AddEventListener(this);
    m_SkeletalAnimControlList.Add(pAnimControl);
  }

  /// Add animation control to mixer for an empty mixer input
  int iMixerInputIndex = -1;
  for(int i = 0; i < m_spNormalizeMixer->GetInputCount(); i++)
  {
    IVisAnimResultGenerator_cl* pResultGenerator = m_spNormalizeMixer->GetMixerInput(i);
    if(!pResultGenerator)
    {
      iMixerInputIndex = i;
      break; 
    }
  }

  // Add given mixer input or add a new one if there was no empty mixer input
  if(iMixerInputIndex >= 0)
  {
    pAnimControl->m_iMixerInputIndex = iMixerInputIndex;
    m_spNormalizeMixer->SetMixerInput(iMixerInputIndex, pAnimControl, 0.f);
  }
  else
  {
    pAnimControl->m_iMixerInputIndex = m_spNormalizeMixer->AddMixerInput(pAnimControl, 0.f);
  }

  if(!VTransitionManager::GlobalManager().GetUseGlobalAnimationEvents())
  {
    // Add animation events that are associated with this sequence to the anim control
    VSequenceDef* sequenceDef = GetTransitionTable()->GetSequenceDef(pSequence);
    if((sequenceDef != NULL) && sequenceDef->GetNumAnimationEvents() > 0)
    {
      for(int i = 0; i < sequenceDef->GetNumAnimationEvents(); i++)
      {
        VisAnimEvent_cl* animEvent = sequenceDef->GetAnimationEventByIndex(i);
        pAnimControl->GetEventList()->AddEvent(animEvent->fTimeValue, animEvent->GetEventString());
      }
    }
  }

  pAnimControl->SetAnimSequence(pSequence);
  VASSERT(pAnimControl->m_iMixerInputIndex!=-1);
  return pAnimControl;
}

void VTransitionStateMachine::RemoveInactiveStateAnimControls()
{
  StateAnimControl_cl* pAnimControl = NULL;

  for(int i = 0; i < m_SkeletalAnimControlList.Count(); i++)
  {
    pAnimControl = (StateAnimControl_cl*)m_SkeletalAnimControlList.GetAt(i);
    if (pAnimControl != NULL && (pAnimControl->m_iMixerInputIndex != -1))
    {
      // Check if control has already been faded out and remove it from mixer
      float fCurrentWeight = m_spNormalizeMixer->GetCurrentEaseValue(pAnimControl->m_iMixerInputIndex);
      if ((fCurrentWeight==0) && (pAnimControl != m_spPrimaryStateAnimControl))
      {
        // Remove from mixer and reset sequence time
        bool bSuccessful = m_spNormalizeMixer->SetMixerInput(pAnimControl->m_iMixerInputIndex, NULL, 0.f);
        pAnimControl->SetCurrentSequenceTime(0.f);
        pAnimControl->m_iMixerInputIndex = -1;
        VASSERT(bSuccessful);
      } 
    }
  }
}


// -------------------------------------------------------------------------- //
// Event handling                                                             //
// -------------------------------------------------------------------------- //

void VTransitionStateMachine::TriggerEvent(INT_PTR iEvent)
{
  if (iEvent == EVENT_TRANSITION_FINISHED)
  {
    // Blending is finished, therefore stop blending immediately
    OnEvent(iEvent);
  }
  else if (iEvent == EVENT_TRANSITION_STARTED)
  {
    if (m_pTransition->m_eType == TRANSITION_TYPE_CROSSFADE_SYNC || m_pTransition->m_eType == TRANSITION_TYPE_IMMEDIATE_SYNC)
    {
      // Blending is synchronized, therefore set the sync state and wait for source sequence events
      m_bSyncBlending = true;
      m_bIsBlending = false;
    }
    if (m_pTransition->m_eType == TRANSITION_TYPE_IMMEDIATE || m_pTransition->m_eType == TRANSITION_TYPE_CROSSFADE)
    {
      // Blending is non-synchronized, therefore start blending immediately
      OnEvent(iEvent);
    }
  }
}

int VTransitionStateMachine::AddEventListener(VisTypedEngineObject_cl* pObj)
{
  VASSERT(pObj);
  m_pEventListener[m_iEventListenerCount++] = pObj;
  return m_iEventListenerCount-1;
}

bool VTransitionStateMachine::RemoveEventListener(VisTypedEngineObject_cl* pObj)
{
  int iIndex = m_pEventListener.GetElementPos(pObj);
  if (iIndex < 0) return false;
  m_iEventListenerCount--;
  for (int i = iIndex; i < m_iEventListenerCount; i++)
    m_pEventListener.GetDataPtr()[i] = m_pEventListener.GetDataPtr()[i+1];
  m_pEventListener.GetDataPtr()[m_iEventListenerCount] = NULL;
  return true;
}

void VTransitionStateMachine::RemoveAllEventListeners(void)
{
  m_pEventListener.Reset();
  m_iEventListenerCount = 0;
}

void VTransitionStateMachine::SendToAllListeners(int iID, INT_PTR iParamA, INT_PTR iParamB) const
{
  for(int i = 0; i < m_iEventListenerCount; i++)
    Vision::Game.PostMsg(m_pEventListener.GetDataPtr()[i], iID, iParamA, iParamB);
}

void VTransitionStateMachine::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  if (m_bIsBlending && iID != VIS_MSG_EVENT)
    return;

  // Check for [sequence started] / [sequence finished] / [transition start reached] events
  if (iParamA == EVENT_SEQUENCE_STARTED || iParamA == EVENT_SEQUENCE_FINISHED || iParamA == EVENT_TRANSITION_START_REACHED)
  {
    OnEvent(iParamA);
    return;
  }

  // Wait for source sequence events and start blending when sync state is active
  if (iID == VIS_MSG_EVENT)
  {
    // If enabled forward the animation event to the registered event listeners
    if (m_bForwardAnimEvents)
      SendToAllListeners(VIS_MSG_EVENT, iParamA, iParamB);

    // For synchronized blending check for the triggered event in the target sequence
    if (m_bSyncBlending)
    { 
      VisAnimSequence_cl *pSourceSequence = (VisAnimSequence_cl*) m_spPrimaryStateAnimControl->GetAnimSequence();

      // We only want to consider events from the anim control that is currently being faded out, which is still
      // stored in the m_spPrimaryStateAnimControl at this time. Otherwise the sync might be initiated due to events
      // from other ongoing animations.
      VisAnimControl_cl *pEventSenderAnimControl = (VisAnimControl_cl *)iParamB;
      if (pEventSenderAnimControl != m_spPrimaryStateAnimControl)
        return;

      VisAnimEventList_cl *pSourceEventList = pSourceSequence->GetEventList();
      if (pSourceEventList->GetEventCount() == 0)
        return;

      // Check whether event also exists in the target sequence
      VisAnimSequence_cl *pTargetSequence = (VisAnimSequence_cl*) m_spSecondaryStateAnimControl->GetAnimSequence();
      if (pTargetSequence->GetEventList()->GetFirstEventTime((int) iParamA) < 0.0f)
        return;

      pSourceEventList->PrepareCurrentEventSet(0.0f, m_spPrimaryStateAnimControl->GetAnimSequence()->GetLength());

      for (int i = 0; i < pSourceEventList->GetEventCount(); i++)
      {
        VisAnimEvent_cl *pSourceEvent = pSourceEventList->GetNextEvent();
        VString sTargetEventName = pSourceEvent->GetEventString();

        if (pSourceEvent->iEventID == iParamA)
        {
          // Set active event ID
          m_iActiveEventID = (int)iParamA;

          // Update active state, so that primary control now contains the target animation
          VSmartPtr<StateAnimControl_cl> pTempControl = m_spPrimaryStateAnimControl;
          m_spPrimaryStateAnimControl = m_spSecondaryStateAnimControl;
          m_spSecondaryStateAnimControl = pTempControl;

          // Fire event to start transition
          OnEvent(EVENT_TRANSITION_STARTED);

          // We do not need to look further
          break;
        }
      }
    }
  }
}


// -------------------------------------------------------------------------- //
// Reload Table                                                         //
// -------------------------------------------------------------------------- //

void VTransitionStateMachine::ReloadTable(const char *szFilename)
{
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)GetOwner();

  // Do not reload the transition file when we are in interactive mode in AnimTool.
  // As the transition file gets reloaded when the TSM gets added to its owner 
  // we need to check whether we want to reload the file as we might be currently
  // in the AnimTool holding temporary transition data in the memory.
  if (!szFilename || szFilename == NULL || !strcmp(szFilename, "AnimToolWorkData"))
    return;

  DeInit();
  VDynamicMesh *pMesh = NULL;
  VTransitionTable *pTable = NULL;
  if (pEntity)
    pMesh = pEntity->GetMesh();

  // Get transition file - if no transition file is specified create a default transition table
  if (pMesh)
    pTable = VTransitionManager::GlobalManager().LoadTransitionTable(pMesh, szFilename);

  Init(pEntity, pTable, m_bCreateAnimConfig);
}

void VTransitionStateMachine::ReloadDefaultTable()
{
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)GetOwner();

  DeInit();
  VDynamicMesh *pMesh = NULL;
  VTransitionTable *pTable = NULL;
  if (pEntity)
    pMesh = pEntity->GetMesh();

  // Get transition file - if no transition file is specified create a default transition table
  if (pMesh)
    pTable = VTransitionManager::GlobalManager().CreateDefaultTransitionTable(pMesh);

  if (pTable)
    Init(pEntity, pTable, m_bCreateAnimConfig);
}


// -------------------------------------------------------------------------- //
// Access to Members                                                          //
// -------------------------------------------------------------------------- //

VTransitionTable *VTransitionStateMachine::GetTransitionTable() const
{
  return m_spTransTable;
}

VisAnimNormalizeMixerNode_cl *VTransitionStateMachine::GetTransitionMixer() const
{
  return m_spNormalizeMixer;
}


// -------------------------------------------------------------------------- //
// IVObjectComponent Virtual Overrides                                        //
// -------------------------------------------------------------------------- //

void VTransitionStateMachine::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  VisBaseEntity_cl *pOldEntity = m_pEntity;
  IVObjectComponent::SetOwner(pOwner);

  // Ensure that we never get an owner that differs from the previously set entity (via ::Init)
  VASSERT(pOwner == NULL || m_pEntity == NULL || m_pEntity == pOwner);

  // add or remove from manager according to whether we have an owner or not
  if (pOwner)
  {
    if (!TransitionTableFile.IsEmpty())
      ReloadTable(TransitionTableFile);
    else
      ReloadDefaultTable();
  }
  else
  {
    DeInit();

    // Remove the anim config if no other component uses it.
    if (m_spAnimConfig != NULL)
    {
      if (pOldEntity != NULL && m_bCreateAnimConfig && 
        m_spAnimConfig.GetPtr()->GetRefCount() <= 2)
      {
        VASSERT_MSG(m_spAnimConfig.GetPtr()->GetRefCount() == 2, "The transition state machine component was attached while the anim config of the entity has been changed.");
        pOldEntity->SetAnimConfig(NULL);
      }
      m_spAnimConfig = NULL;
    }
  }
}

BOOL VTransitionStateMachine::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return false;

  // Can only attach to entities
  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
  {
    sErrorMsgOut = "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
    return FALSE;
  }
  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)pObject;

  // Check whether entity has a model
  BOOL bHasModel = (pEntity->GetMesh() != NULL);
  if (!bHasModel)
  {
    sErrorMsgOut = "Parent entity needs to have a model file set.";
    return FALSE;
  }

  // Check whether the model has any animation sets
  BOOL bHasAnims = (pEntity->GetMesh()->GetSequenceSetCollection() != NULL);
  if (bHasAnims)
    bHasAnims = (pEntity->GetMesh()->GetSequenceSetCollection()->Count() != 0);
  if (!bHasAnims)
  {
    sErrorMsgOut = "The parent entity needs a model file with at least one skeletal animation.";
    return FALSE;
  }

  return TRUE;
}


void VTransitionStateMachine::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  // Don't perform any update/invalidation code in case we don't have an owner yet.
  // SetOwner will perform an initialization once it gets called.
  if (GetOwner() == NULL)
    return;

  if (!strcmp(pVar->GetName(), "TransitionTableFile"))
  {
    if (!TransitionTableFile.IsEmpty())
      ReloadTable(TransitionTableFile);
    else
      ReloadDefaultTable();

    return;
  }
  else if (!strcmp(pVar->GetName(), "m_bEnabled"))
  {
    SetEnabled(m_bEnabled);
    return;
  }
}


// -------------------------------------------------------------------------- //
// Serialization                                                              //
// -------------------------------------------------------------------------- //

V_IMPLEMENT_SERIAL( VTransitionStateMachine, IVObjectComponent, 0, &g_VisionEngineModule );
void VTransitionStateMachine::Serialize( VArchive &ar )
{
  // No deep object serialize when performing a sync operation (SetSyncState / GetSyncState)
  if (!m_bSyncSerialize)
    IVObjectComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    int iVersion;
    ar >> iVersion;
    ar >> TransitionTableFile;
    ar >> m_bCreateAnimConfig;
    ar >> InitialAnimation;

    if (iVersion >= VTRANSITIONSTATE_MACHINE_VERSION_1)
      ar >> m_bEnabled;
    else
      m_bEnabled = TRUE;
  }
  else
  {
    int iVersion = VTRANSITIONSTATE_MACHINE_VERSION_CURRENT;
    ar << iVersion;

    ar << TransitionTableFile;
    ar << m_bCreateAnimConfig;
    ar << InitialAnimation;

    ar << m_bEnabled;
  }
}


#define SYNCSTATEVERSION_1  0x80000000    // Initial version number
#define SYNCSTATEVERSION_CURRENT         SYNCSTATEVERSION_1

void VTransitionStateMachine::SetSyncState(VArchive &ar)
{
  VASSERT(ar.IsLoading());

  // prepare object to be serialized for sync, no deep object serialization
  m_bSyncSerialize = true;

  int iVersion;
  ar >> iVersion;

  ar >> m_iActiveEventID;
  ar >> m_fTransitionTime;
  ar >> m_bIsBlending;
  ar >> m_bSyncBlending;
  ar >> m_bProcessOffsetDelta;
  ar >> m_bFinishBlending;

  // load anim config
  VisAnimConfig_cl *pAnimConfig;
  ar >> pAnimConfig;
  if (pAnimConfig != NULL)
  {
    m_spAnimConfig = pAnimConfig;
    m_pEntity->SetAnimConfig(pAnimConfig);
  }

  VisAnimNormalizeMixerNode_cl *pTempMixer = NULL;
  ar >> pTempMixer; m_spNormalizeMixer = pTempMixer;

  // reset NormalizeMixer and AnimControlList
  // will be refilled 
  m_SkeletalAnimControlList.Reset();

  int animCtrlListCount;
  ar >> animCtrlListCount;
  for(int il=0; il<animCtrlListCount; ++il)
  {
    // create new StateAnimControl and read from Archive
    StateAnimControl_cl* newAnimCtrl = NULL;
    ar >> newAnimCtrl;
    m_SkeletalAnimControlList.Add(newAnimCtrl);
    newAnimCtrl->AddEventListener(this);
  }

  // reload current transition
  int transitionDefID;
  ar >> transitionDefID;
  m_pTransition = (transitionDefID >= 0) ? m_spTransTable->GetTransitionDef(transitionDefID) : NULL;

  // reload transition sequence data
  VisAnimSequence_cl *animSequence;
  animSequence = (VisAnimSequence_cl *)ar.ReadProxyObject();
  if (animSequence)
  {
    m_pSequence = m_spTransTable->GetSequenceDef(animSequence);
    m_bFinishSequence = m_pSequence->GetFinishSequence();
  }
  else
  {
    m_pSequence = NULL;
    m_bFinishSequence = false;
  }

  // reassign primary & secondary state anim controls
  StateAnimControl_cl *pTempAnimControl = NULL;
  ar >> pTempAnimControl; m_spPrimaryStateAnimControl = pTempAnimControl;
  ar >> pTempAnimControl; m_spSecondaryStateAnimControl = pTempAnimControl;

  // release object sync serialized lock
  m_bSyncSerialize = false;
}

void VTransitionStateMachine::GetSyncState(VArchive &ar)
{
  VASSERT(!ar.IsLoading());

  // prepare object to be serialized for sync, no deep object serialization
  m_bSyncSerialize = true;

  ar << (int)SYNCSTATEVERSION_CURRENT;

  ar << m_iActiveEventID;
  ar << m_fTransitionTime;
  ar << m_bIsBlending;
  ar << m_bSyncBlending;
  ar << m_bProcessOffsetDelta;
  ar << m_bFinishBlending;

  // important here to reset the AnimControl EventListeners, to avoid that our serialization
  // gets to the actual entity(s).
  // Will be reassigned afterwards after writing the anim sequences and anim config
  DynArray_cl<TDynArrEngObj*> eventListeners;
  GetAndDisableAllSkeletalAnimControlEventListeners(eventListeners);

  // write AnimConfig to archive
  ar << m_pEntity->GetAnimConfig();
  ar << m_spNormalizeMixer;

  // write SkeletalAnimControls to archive
  int animCtrlListCount = m_SkeletalAnimControlList.Count();
  ar << animCtrlListCount;
  for(int il=0; il<animCtrlListCount; ++il)
    ar << (VisTypedEngineObject_cl *) m_SkeletalAnimControlList.GetAt(il);

  // write Transition ID to archive
  if (m_pTransition) ar << m_pTransition->m_iID;
  else ar << (int)-1;

  // write current sequence proxy to archive
  if (m_pSequence != NULL) ar.WriteProxyObject(m_pSequence->GetOwnerSequence()); 
  else ar.WriteObject(NULL);

  // reassign anim control event listeners
  ResetAllSkeletalAnimControlEventListeners(eventListeners);


  // write primary and secondary animControl Sequences to archive (as ProxyObject)
  ar <<  (VisTypedEngineObject_cl *) m_spPrimaryStateAnimControl.GetPtr();
  ar <<  (VisTypedEngineObject_cl *) m_spSecondaryStateAnimControl.GetPtr();

  // release object sync serialized lock
  m_bSyncSerialize = false;
}


void VTransitionStateMachine::GetAndDisableAllSkeletalAnimControlEventListeners( DynArray_cl<TDynArrEngObj*> &aEventListeners )
{
  aEventListeners.Reset();
  aEventListeners.SetDefaultValue( NULL );

  // Disable ALL EventListeners of our SkeletalAnimControls, but keep them
  // in an own temporary list.
  int animCtrlListCount = m_SkeletalAnimControlList.Count();
  aEventListeners.Resize(animCtrlListCount);
  for(int il=0; il<animCtrlListCount; ++il)
  {
    int eventCount = m_SkeletalAnimControlList.GetAt(il)->GetEventListenerCount();

    // allocated array will be deleted in ResetAllSkeletalAnimControlEventListeners(...)
    TDynArrEngObj* newArray = new TDynArrEngObj(eventCount, NULL);
    for (int jl=0; jl<eventCount; ++jl)
    {
      newArray->Get(jl) = m_SkeletalAnimControlList.GetAt(il)->GetEventListener(jl);
    }
    m_SkeletalAnimControlList.GetAt(il)->RemoveAllEventListeners();
    aEventListeners[il] = newArray;
  }
}

void VTransitionStateMachine::ResetAllSkeletalAnimControlEventListeners( DynArray_cl<TDynArrEngObj*> &aEventListeners )
{
  int arrayLength = aEventListeners.GetValidSize();
  for(int il=0; il<arrayLength; ++il)
  {
    TDynArrEngObj* eventListenerArray = aEventListeners[il];
    int eventCount = eventListenerArray->GetValidSize();
    for (int jl=0; jl<eventCount; ++jl)
    {
      m_SkeletalAnimControlList.GetAt(il)->AddEventListener(eventListenerArray->Get(jl));
    }

    // delete allocated array
    delete eventListenerArray;
  }
}

void VTransitionStateMachine::OnDeserializationCallback(const VSerializationContext &context)
{
  // If this method gets called during a sync serialize then something is wrong => the
  // sync serialization should never serialize this state machine instance itself.
  VVERIFY_OR_RET(m_bSyncSerialize == false);

  // Initialize the state machine once the owner is guaranteed to be fully loaded
  if (!TransitionTableFile.IsEmpty())
    ReloadTable(TransitionTableFile);
  else
    ReloadDefaultTable();
}

int VTransitionStateMachine::GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList)
{
  int iCount = IVObjectComponent::GetSynchronizationGroupList(context, pDestList);
  pDestList[iCount++].Set(this, &VTransitionStateMachineNetworkSyncGroup::g_GroupInstanceT);
  return iCount;
}


START_VAR_TABLE(VTransitionStateMachine,IVObjectComponent,"Animation transition state machine. Can be attached to entities, as long as they have a model assigned.",VCOMPONENT_ALLOW_MULTIPLE, "TransitionStateMachine" )
  DEFINE_VAR_BOOL_AND_NAME(VTransitionStateMachine, m_bEnabled, "Enabled", "Enable or disable animations", "True", 0, 0);
  DEFINE_VAR_VSTRING(VTransitionStateMachine, TransitionTableFile, "Filename of the transition table (.vTransition)", "", 0,0, "filepicker(.vTransition)");
  DEFINE_VAR_VSTRING(VTransitionStateMachine, InitialAnimation, "Name of the initial animation set in the state machine", "", 0, 0, "dropdownlist(Animation)");
END_VAR_TABLE


////////////////////////////////////////////////////////////////////////////////////
// class VTransitionStateMachineNetworkSyncGroup
////////////////////////////////////////////////////////////////////////////////////
VTransitionStateMachineNetworkSyncGroup VTransitionStateMachineNetworkSyncGroup::g_GroupInstanceT;


bool VTransitionStateMachineNetworkSyncGroup::QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings)
{
  VTransitionStateMachine *pComponent = (VTransitionStateMachine *)instanceInfo.m_pInstance;
  __int64 iNewHash = 0x0fffffff00000000; // arbitrary hash for NULL string
  const char* szName = pComponent->GetActiveState()->GetName();
  if (szName && szName[0])
    iNewHash = VStringUtil::Hash(szName);

  // same name hash as last frame?
  if (iNewHash==instanceInfo.m_iLastUpdateDataHash)
    return false;
 
  instanceInfo.m_iLastUpdateDataHash = iNewHash;
  return true;
}


void VTransitionStateMachineNetworkSyncGroup::Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar)
{
  VTransitionStateMachine *pComponent = (VTransitionStateMachine *)instanceInfo.m_pInstance;

  if (ar.IsLoading())
  {
    //receive the new transition state (if present)
    char buffer[1024];
    bool bAllocated = false;
    char* szName = ar.ReadEncryptedString(buffer,sizeof(buffer),bAllocated);
    
    //set new state if the name is valid
    if (szName != NULL && szName[0])
    {
      // reset these flags, since they prevent the TSM to accept a new state
      pComponent->m_bSyncBlending = false;
      pComponent->m_bFinishBlending = false;
      pComponent->m_bFinishSequence = false;

      pComponent->SetState(szName);
    }

    if (bAllocated) VBaseDealloc(szName);
  }
  else
  {
    ANALYSIS_IGNORE_WARNING_ONCE(6246)

    //send the name of the new transition state
    const char* szName = pComponent->GetActiveState()->GetName();
    ar.WriteEncryptedString(szName); //send new state
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
