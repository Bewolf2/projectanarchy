/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionUserMessages.h>

#include <Vision/Runtime/Engine/Physics/IVisApiPhysicsModule.hpp>

namespace
{
  class RPG_ClosestRaycastResultIgnoreSourceTarget : public VisPhysicsRaycastBase_cl
  {
  public:
    RPG_ClosestRaycastResultIgnoreSourceTarget(VisTypedEngineObject_cl const* self, VisTypedEngineObject_cl const* target)
    : VisPhysicsRaycastBase_cl()
    , bHit(false)
    , pSelf(self)
    , pTarget(target)
    {
      iCollisionBitmask = hkpGroupFilter::calcFilterInfo(0, 0, 0, 0);
    }

    virtual void Reset()
    {
      VisPhysicsRaycastBase_cl::Reset();
      bHit = false;
      hit.Reset();
      iCollisionBitmask = hkpGroupFilter::calcFilterInfo(0, 0, 0, 0);
    }

    virtual bool onHit(VisPhysicsHit_t &hit)
    {
      if(hit.pHitObject == pSelf || hit.pHitObject == pTarget)
      {
        return true;
      }

      bHit = true;
      this->hit = hit;
      return false;
    }

    virtual bool allHits(){ return false; }

    VisPhysicsHit_t hit;
    bool bHit;
    VisTypedEngineObject_cl const* pSelf;
    VisTypedEngineObject_cl const* pTarget;
  };
}


// RPG_AiControllerComponent
RPG_AiControllerComponent::RPG_AiControllerComponent()
  : RPG_ControllerComponent()
  , m_aiState(ST_None)
  , m_isFirstStateUpdate(true)
  , m_stateStartTime(0.0f)
  , m_target(NULL)
{
}

V_IMPLEMENT_SERIAL(RPG_AiControllerComponent, RPG_ControllerComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_AiControllerComponent, RPG_ControllerComponent, "(RPG) AI Controller", 0, "(RPG) AI Controller")
END_VAR_TABLE

void RPG_AiControllerComponent::ServerTick(float deltaTime)
{
  VASSERT(GetOwner());
  if(static_cast<RPG_Character*>(GetOwner())->IsDead())
  {
    return;
  }

  SelectState();
  UpdateState(deltaTime);
}

RPG_AIState_e RPG_AiControllerComponent::GetState() const
{
  return m_aiState;
}

bool RPG_AiControllerComponent::IsInState(RPG_AIState_e state) const
{
  return state == GetState();
}

bool RPG_AiControllerComponent::SwitchState(RPG_AIState_e newState)
{
  if (GetState() != newState && CanEnterState(newState))
  {
    m_aiState = newState;
    m_isFirstStateUpdate = true;
    m_stateStartTime = Vision::GetTimer()->GetTime();
    return true;
  }
  return false;
}

void RPG_AiControllerComponent::UpdateState(float const deltaTime)
{
  if (m_characterOwner->ShouldDisplayDebugStateInformation())
  {
    DebugDisplayStateInformation();
  }

  RPG_AIState_e const state = GetState();
  switch(state)
  {
  case ST_Spawning:
    UpdateSpawning(deltaTime);
    break;
  case ST_Wandering:
    UpdateWandering(deltaTime);
    break;
  case ST_Challenging:
    UpdateChallenging(deltaTime);
    break;
  case ST_MovingToPosition:
    UpdateMovingToPosition(deltaTime);
    break;
  case ST_MeleeAttacking:
    UpdateMeleeAttacking(deltaTime);
    break;
  case ST_RangedAttacking:
    UpdateRangedAttacking(deltaTime);
    break;
  case ST_AoeAttacking:
    UpdateAoeAttacking(deltaTime);
    break;
  case ST_Fleeing:
    UpdateFleeing(deltaTime);
    break;
  case ST_Healing:
    UpdateHealing(deltaTime);
    break;
  default:
    VASSERT_MSG(false, "RPG_Ai Controller Component tried to update an unhandled state.");
    break;
  }
  m_isFirstStateUpdate = false;
}

/// Finds and sets a target within range to interact with (usually to attack).
/// @TODO: Implement teams and team relationships, as target must currently be a player.
bool RPG_AiControllerComponent::AcquireTarget()
{
  VArray<RPG_Character*> const& characters = RPG_GameManager::s_instance.GetCharacters();

  hkvVec3 const& currentPosition = m_characterOwner->GetPosition();

  bool returnVal = false;
  for(int index = 0; index < characters.GetSize(); ++index)
  {
    RPG_Character* character = characters.GetAt(index);

    // can't target yourself
    if(character == m_characterOwner)
    {
      continue;
    }

    // only target apparently alive players
    if(!character->IsOfType(V_RUNTIME_CLASS(RPG_PlayerCharacter)) ||
      character->IsDead() ||
      character->IsFeigningDeath())
    {
      continue;
    }

    float const aggroRadius = m_characterOwner->GetAggroRadius();
    hkvVec3 const& targetPosition = character->GetPosition();

    float const currentRangeSquared = (currentPosition - targetPosition).getLengthSquared();

    // check if they are within range
    if(currentRangeSquared <= aggroRadius * aggroRadius)
    {
      m_target = character;
      returnVal = true;
    }
  }
  return returnVal;
}

void RPG_AiControllerComponent::SetTarget(RPG_DamageableEntity* target)
{
  m_target = target;
}

/// Returns the current target.
RPG_DamageableEntity* RPG_AiControllerComponent::GetTarget()
{
  return m_target;
}

void RPG_AiControllerComponent::ClearTarget()
{
  m_target = NULL;
}

bool RPG_AiControllerComponent::HasTarget() const
{
  return m_target != NULL;
}

bool RPG_AiControllerComponent::HasValidTarget() const
{
  if (HasTarget())
  {
    VArray<RPG_DamageableEntity*> const& attackableEntities = RPG_GameManager::s_instance.GetAttackableEntities();
    if(attackableEntities.Find(m_target) < 0)
    {
      return false;
    }

    if(m_target->IsFrom(RPG_Character))
    {
      RPG_Character* targetCharacter = static_cast<RPG_Character*>(m_target);
      return !targetCharacter->IsDead() && !targetCharacter->IsFeigningDeath();
    }
    else
    {
      return true;
    }
  }
  return false;
}

bool RPG_AiControllerComponent::HasLineOfSightToTarget() const
{
  bool left, center, right;
  return HasLineOfSightToTarget(left, center, right);
}

bool RPG_AiControllerComponent::HasLineOfSightToTarget(bool& left, bool& center, bool& right) const
{
  VASSERT(m_characterOwner);
  VASSERT(m_target);

  RPG_ClosestRaycastResultIgnoreSourceTarget raycastResult(m_characterOwner, m_target);

  hkvVec3 rayStart;
  m_characterOwner->CalcPositionForTargeting(rayStart);

  hkvVec3 rayEnd;
  m_target->CalcPositionForTargeting(rayEnd);

  hkvVec3 directionToTarget = rayEnd - rayStart;
  directionToTarget.normalizeIfNotZero();

  // check from the center
  center = true;
  raycastResult.Reset();
  raycastResult.vRayStart = rayStart;
  raycastResult.vRayEnd = rayEnd;
  Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&raycastResult);
  if (m_characterOwner->ShouldDisplayDebugStateInformation())
  {
    if(raycastResult.bHit)
    {
      Vision::Game.DrawSingleLine(raycastResult.vRayStart, raycastResult.vRayEnd, VColorRef(208, 24, 24), 3.0f);
    }
    else
    {
      Vision::Game.DrawSingleLine(raycastResult.vRayStart, raycastResult.vRayEnd, VColorRef(24, 208, 24), 3.0f);
    }
  }
  if(raycastResult.bHit)
  {
    center = false;
  }

  // check from the left side
  left = true;
  raycastResult.Reset();
  raycastResult.vRayStart = rayStart + hkvVec3(-directionToTarget.y, directionToTarget.x, 0.0f) * m_characterOwner->GetCollisionRadius();
  raycastResult.vRayEnd = rayEnd + hkvVec3(-directionToTarget.y, directionToTarget.x, 0.0f) * m_characterOwner->GetCollisionRadius();
  Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&raycastResult);
  if (m_characterOwner->ShouldDisplayDebugStateInformation())
  {
    if(raycastResult.bHit)
    {
      Vision::Game.DrawSingleLine(raycastResult.vRayStart, raycastResult.vRayEnd, VColorRef(208, 24, 24), 3.0f);
    }
    else
    {
      Vision::Game.DrawSingleLine(raycastResult.vRayStart, raycastResult.vRayEnd, VColorRef(24, 208, 24), 3.0f);
    }
  }
  if(raycastResult.bHit)
  {
    left = false;
  }

  // check from the right side
  right = true;
  raycastResult.Reset();
  raycastResult.vRayStart = rayStart + hkvVec3(directionToTarget.y, -directionToTarget.x, 0.0f) * m_characterOwner->GetCollisionRadius();
  raycastResult.vRayEnd = rayEnd + hkvVec3(directionToTarget.y, -directionToTarget.x, 0.0f) * m_characterOwner->GetCollisionRadius();
  Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&raycastResult);
  if (m_characterOwner->ShouldDisplayDebugStateInformation())
  {
    if(raycastResult.bHit)
    {
      Vision::Game.DrawSingleLine(raycastResult.vRayStart, raycastResult.vRayEnd, VColorRef(208, 24, 24), 3.0f);
    }
    else
    {
      Vision::Game.DrawSingleLine(raycastResult.vRayStart, raycastResult.vRayEnd, VColorRef(24, 208, 24), 3.0f);
    }
  }
  if(raycastResult.bHit)
  {
    right = false;
  }

  return left && center && right;
}

bool RPG_AiControllerComponent::IsValidTargetInAggroRange() const
{
  VASSERT(m_characterOwner);
  return m_target && m_characterOwner->IsTargetWithinRange(m_target, m_characterOwner->GetAggroRadius());
}

bool RPG_AiControllerComponent::FleeFromPosition(hkvVec3 const& targetPosition, float const fleeDistance /*= 300.f*/)
{
  VASSERT(m_characterOwner);
  hkvVec3 const& currentPosition = m_characterOwner->GetPosition();

  // find a position away from the target
  hkvVec3 const& fleeDirection = (currentPosition - targetPosition).getNormalized(); //direction from target to me
  hkvVec3 const& fleePosition = currentPosition + fleeDirection * fleeDistance;

  // if fleePosition is valid, just use it
  if (RPG_ControllerUtil::IsPointOnNavmesh(fleePosition))
  {
    m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, NULL, fleePosition);
    if (m_characterOwner->ShouldDisplayDebugStateInformation())
    {
      Vision::Game.DrawSingleLine(currentPosition, fleePosition, VColorRef(24, 208, 208), 3.0f);
    }
    return true;
  }
  else
  {
    hkvVec3 nearestFleePosition;
    if (RPG_ControllerUtil::GetClosestPointOnNavMesh(fleePosition, 5.0f, nearestFleePosition))
    {
      m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, NULL, nearestFleePosition);
      if (m_characterOwner->ShouldDisplayDebugStateInformation())
      {
        Vision::Game.DrawSingleLine(currentPosition, nearestFleePosition, VColorRef(24, 208, 208), 3.0f);
      }
      return true;
    }
  }
  return false;
}

void RPG_AiControllerComponent::UpdateSpawning(float const deltaTime)
{
  // if we entered this state and weren't already performing its associated action, start it up.
  if (m_isFirstStateUpdate && !m_characterOwner->GetActionHandler().IsPerformingAction(AT_Spawn))
  {
    m_characterOwner->GetActionHandler().PerformAction(AT_Spawn, true, m_target);
  }
  else
  {
    // when the action is no longer being performed, exit the state.
    if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Spawn))
    {
      SwitchState(ST_None);
    }
  }
}

void RPG_AiControllerComponent::UpdateChallenging(float const deltaTime)
{
  if (m_isFirstStateUpdate)
  {
    // target is newly acquired. Begin the challenge.
    m_characterOwner->GetActionHandler().PerformAction(AT_Challenge, true, m_target);
  }
  else
  {
    if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Challenge))
    {
      // challenge SA has completed.
      SwitchState(ST_None);
    }
  }
}

void RPG_AiControllerComponent::DebugDisplayStateInformation() const
{
  const float vOffset = 0.f;
  VString msg, tempLine, stateName;

  RPG_AIState_e const state = GetState();
  switch(state)
  {
  case ST_None:
    stateName = "None";
  case ST_Spawning:
    stateName = "Spawning";
    break;
  case ST_Wandering:
    stateName = "Wandering";
    break;
  case ST_Challenging:
    stateName = "Challenging";
    break;
  case ST_MovingToPosition:
    stateName = "Moving to Position";
    break;
  case ST_MeleeAttacking:
    stateName = "Melee Attacking";
    break;
  case ST_RangedAttacking:
    stateName = "Ranged Attacking";
    break;
  case ST_AoeAttacking:
    stateName = "AoE Attacking";
    break;
  case ST_Fleeing:
    stateName = "Fleeing";
    break;
  case ST_Healing:
    stateName = "Healing";
    break;
  default:
    VASSERT_MSG(false, "Hey developer! Update your DebugDisplayStateInformation() to accommodate any newly-defined states.");
    break;
  }

  tempLine.Format("State: %s\n", stateName.AsChar());
  msg += tempLine;

  VASSERT(m_characterOwner);
  const hkvVec3 vPos = m_characterOwner->GetPosition() + hkvVec3(0.0f, 0.0f, vOffset);

  Vision::Message.SetTextColor(VColorRef(208,127,127,255));
  Vision::Message.DrawMessage3D(msg, vPos);
  Vision::Message.SetTextColor(); // restore default white
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
