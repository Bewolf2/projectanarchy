/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionHandler.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/Action.h>

// XXX TEST
#include <Vision/Samples/Engine/RPGPlugin/ActionAoeAttack.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionBlock.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionChallenge.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionDodge.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionDie.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionHeal.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionMeleeAttack.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionMove.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionPowerAttack.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionRangedAttack.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionSpawn.h>
// XXX

RPG_ActionData::RPG_ActionData()
  : m_Action(AT_None)
  , m_interactionEntity(NULL)
  , m_interactionPosition(0, 0, 0)
  , m_flags(0)
{
}

RPG_ActionData::RPG_ActionData(RPG_ActionType_e const& action, RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition, int const flags)
  : m_Action(action)
  , m_interactionEntity(interactionEntity)
  , m_interactionPosition(interactionPosition)
  , m_flags(flags)
{
}

void RPG_ActionData::Clear()
{
  m_Action = AT_None;
  m_interactionEntity = NULL;
  m_flags = 0;
}

/// Constructor
RPG_ActionHandler::RPG_ActionHandler()
  : m_characterOwner(NULL)
  , m_activeAction()
  , m_previousAction()
  , m_pendingAction()
  , m_endingAction(false)
  , m_chainingAction(false)
  , m_previousActionTime(0)
{
}

void RPG_ActionHandler::Initialize(RPG_Character* characterOwner)
{
  //@todo: data-drive this - actions a character can perform and the classes which handle them
  m_Actions[AT_MeleeAttack] = new RPG_Action_MeleeAttack(characterOwner);
  m_Actions[AT_RangedAttack] = new RPG_Action_RangedAttack(characterOwner);
  m_Actions[AT_PowerAttack] = new RPG_Action_PowerAttack(characterOwner);
  m_Actions[AT_AoeAttack] = new RPG_Action_AoeAttack(characterOwner);
  m_Actions[AT_Dodge] = new RPG_Action_Dodge(characterOwner);
  m_Actions[AT_Block] = new RPG_Action_Block(characterOwner);
  m_Actions[AT_Die] = new RPG_Action_Die(characterOwner);
  m_Actions[AT_Challenge] = new RPG_Action_Challenge(characterOwner);
  m_Actions[AT_Spawn] = new RPG_Action_Spawn(characterOwner);
  m_Actions[AT_Move] = new RPG_Action_Move(characterOwner);
  m_Actions[AT_Heal] = new RPG_Action_Heal(characterOwner);

  m_characterOwner = characterOwner;
}

void RPG_ActionHandler::Tick(const float deltaTime)
{
  if (m_activeAction.m_Action != AT_None)
  {
    if (m_Actions[m_activeAction.m_Action])
    {
      m_Actions[m_activeAction.m_Action]->Tick(deltaTime);
    }
  }
}

bool RPG_ActionHandler::CanPerformAction(RPG_ActionType_e const& action, bool const forceCheck /*= false*/) const
{
  if(action != AT_None &&
     m_activeAction.m_Action != AT_None &&
     m_pendingAction.m_Action == action)
  {
    return true;
  }

  if(!IsPerformingAction())
  {
    return true;  //@todo: temporary return value. Once Actions are sketched in, validation rules go here.
  }
  
  return false;
}

void RPG_ActionHandler::PerformAction(RPG_ActionType_e const& action, bool forceAction /*= false*/, RPG_BaseEntity* const interactionEntity /*= NULL*/, 
                                      hkvVec3 const& interactionPosition /*= hkvVec3(0, 0, 0)*/, int const flags /*= 0*/)
{
  // if the action and the target are the same, we're just updating flags or interaction.
  bool updatingActionParams = false;
  
  if(m_activeAction.m_Action == action)
  {
    if(m_activeAction.m_interactionEntity != interactionEntity ||
       !m_activeAction.m_interactionPosition.isIdentical(interactionPosition) ||
       m_activeAction.m_flags != flags)
    {
      updatingActionParams = true;
    }
  }

  if(!forceAction && 
     m_activeAction.m_Action != AT_None &&
     action != AT_None)
  {  
    // there's an action already playing, and we aren't forcing the new one. perfor more checks.
    if(m_Actions[m_activeAction.m_Action]->CanOverrideActionWith(action) ||
       m_Actions[action]->CanThisActionOverride(m_activeAction.m_Action))
    {
      forceAction = true;
    }
  }

  if (forceAction || updatingActionParams || CanPerformAction(action))
  {
    // bundle the action into a struct for easier network replication
    RPG_ActionData actionData(action, interactionEntity, interactionPosition, flags);

    // simulate the action locally
    ProcessAction(actionData, forceAction);

    // replicate the action to the server
    //@todo: Once the network branch is integrated, here's where we replicate this action to the server.
    /*
    Work that needs to happen here:
    - Find out whether this is a non-authoritative client.
    - If all we're doing is updating action flags, preserve bandwidth by calling a client->server UpdateActionFlags() with just that data
    - If we're doing a new Action, call a client->server ProcessAction() using the constructed struct.
    */
  }
  else if(action != AT_None &&
     m_Actions[m_activeAction.m_Action]->CanChainAction(action))
  {
    // can't process the new action, so check to see if we can chain it
    RPG_ActionData actionData(action, interactionEntity, interactionPosition, flags);
    m_pendingAction = actionData;
  }
}

void RPG_ActionHandler::StartAction(RPG_ActionType_e const& action, RPG_ActionType_e const previousAction, bool forceAction /* = false */, 
                                    RPG_BaseEntity* const interactionEntity /* = NULL */, hkvVec3 const& interactionPosition /* = hkvVec3(0,0,0) */,
                                    int const flags /* = 0 */)
{
  if (action != AT_None)
  {
    if (m_Actions[action])
    {
      // pass on the start to the Action instance
      m_Actions[action]->UpdateFlags(flags);
      m_Actions[action]->Start(forceAction, previousAction, interactionEntity, interactionPosition);
      m_previousActionTime = Vision::GetTimer()->GetTime();  //@todo: once networking is in, we need to get the server time here
    }
    else
    {
      Vision::Error.Warning("Attempted to start a Action that hasn't been instantiated on this character.");
    }
  }
}

void RPG_ActionHandler::EndAction(RPG_ActionType_e const& action /*= AT_None*/, RPG_ActionType_e const& nextAction /*= AT_None*/)
{
  if (IsPerformingAction())
  {
    // if the move we've been ordered to end matches the pending move, clear it
    if (action != AT_None && m_pendingAction.m_Action == action)
    {
      m_pendingAction.Clear();
    }

    // if no action has been specified, or the specified action is the current action, end it
    if (action == AT_None || IsPerformingAction(action))
    {
      PerformAction(AT_None, true);
    }
  }
}

void RPG_ActionHandler::CleanUpAction(RPG_ActionType_e const& action /*= AT_None*/, RPG_ActionType_e const& nextAction /*= AT_None*/)
{
  if (action != AT_None)
  {
    if (m_Actions[action])
    {
      m_Actions[action]->Cleanup(nextAction);
    }
  }
}

bool RPG_ActionHandler::IsPerformingAction( RPG_ActionType_e const& action ) const
{
  return (m_activeAction.m_Action == action && !m_endingAction);
}

bool RPG_ActionHandler::IsPerformingAction() const
{
  return (m_activeAction.m_Action != AT_None && !m_endingAction);
}

/// Sets a flag on the active Action.
void RPG_ActionHandler::SetActiveActionFlags(int const flags/*=0*/)
{
  if (m_activeAction.m_Action != AT_None)
  {
    m_activeAction.m_flags = flags;
    if (m_Actions[m_activeAction.m_Action])
    {
      m_Actions[m_activeAction.m_Action]->UpdateFlags(flags);
    }
  }
}

void RPG_ActionHandler::ProcessAction(RPG_ActionData const& actionData, bool const forceAction /*= false*/)
{
  // if we're already doing the requested move, ignore the call except to update flags or interaction entities.
  if (actionData.m_Action == m_activeAction.m_Action)
  {
    // same move, updated flags
    if (actionData.m_Action != AT_None && actionData.m_flags != m_activeAction.m_flags)
    {
      m_activeAction.m_flags = actionData.m_flags;
      //@todo: if this is happening on the authoritative server, replicate this flag change to clients.
      if (m_Actions[actionData.m_Action])
      {
        m_Actions[actionData.m_Action]->UpdateFlags(actionData.m_flags);
      }
    }

    // same move, updated interaction entity
    if(actionData.m_interactionEntity != m_activeAction.m_interactionEntity ||
        !actionData.m_interactionPosition.isIdentical(m_activeAction.m_interactionPosition))
    {
      m_activeAction.m_interactionEntity = actionData.m_interactionEntity;
      m_activeAction.m_interactionPosition = actionData.m_interactionPosition;

      if(m_Actions[actionData.m_Action])
      {
        m_Actions[actionData.m_Action]->UpdateInteraction(actionData.m_interactionEntity, actionData.m_interactionPosition);

        // clear the pending action, as enough data has changed for the pending action to probably not be the desired behavior.
        m_pendingAction.Clear();
      }
    }

    // clear the pending action if this was forced
    if(forceAction)
    {
      m_pendingAction.Clear();
    }

    // no further action required, since this isn't a new move.
    return;
  }

  // if we're chaining from an action that needs to complete first, defer this action
  if (m_endingAction)
  {
    m_pendingAction = actionData;
    return;
  }

  // verify that we can do this move
  if (actionData.m_Action != AT_None && !forceAction && !m_chainingAction && !CanPerformAction(actionData.m_Action))
  {
    Vision::Error.Warning("Cannot perform requested Action.");
    return;
  }

  // ready to go.
  m_previousAction = m_activeAction;

  // stop the previous action, if any
  if (m_activeAction.m_Action != AT_None)
  {
    m_endingAction = true;
    m_activeAction.m_Action = AT_None;
    RPG_ActionType_e nextAction = actionData.m_Action;
    if (nextAction == AT_None && m_pendingAction.m_Action != AT_None)
    {
      nextAction = m_pendingAction.m_Action;
    }
    CleanUpAction(m_previousAction.m_Action, nextAction);
    m_endingAction = false;
  }

  // if we need to start a pending action, do it now
  if (actionData.m_Action == AT_None && m_pendingAction.m_Action != AT_None)
  {
    // Recursively Process the pending action if one exists.
    
    m_chainingAction = true;
    m_activeAction.m_Action = m_previousAction.m_Action;  // restore this value, since the pending action call will need it.
    ProcessAction(m_pendingAction);
    m_chainingAction = false;
    
    if (m_activeAction.m_Action == m_pendingAction.m_Action)
    {
      m_pendingAction.Clear();
      return;
    }
  }

  // set the new Action
  m_activeAction = actionData;

  // @todo: Once the network branch is in, replicate these values.
  // - m_replicatedAction = m_activeAction;
  // - force net update

  if (m_activeAction.m_Action != AT_None)
  {
    StartAction(m_activeAction.m_Action, m_previousAction.m_Action, forceAction, m_activeAction.m_interactionEntity, m_activeAction.m_interactionPosition, m_activeAction.m_flags);

    // if this was a forced action, clear any pending action, as this was an interrupt of the current action
    if (forceAction)
    {
      m_pendingAction.Clear();
    }
  }
}

void RPG_ActionHandler::OnHavokBehaviorEvent(const VString& eventName)
{
  if(m_activeAction.m_Action == AT_None)
    return;

  if (m_Actions[m_activeAction.m_Action])
  {
    m_Actions[m_activeAction.m_Action]->OnHavokBehaviorEvent(eventName);
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
