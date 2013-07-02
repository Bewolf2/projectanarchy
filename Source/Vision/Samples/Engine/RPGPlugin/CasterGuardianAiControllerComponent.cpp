/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/CasterGuardianAiControllerComponent.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/CasterGuardian.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

V_IMPLEMENT_SERIAL(RPG_CasterGuardianAiControllerComponent, RPG_AiControllerComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_CasterGuardianAiControllerComponent, RPG_AiControllerComponent, "(RPG) Caster Guardian AI Controller", 0, "(RPG) Caster Guardian AI Controller")
END_VAR_TABLE

  // RPG_GuardianAiControllerComponent
RPG_CasterGuardianAiControllerComponent::RPG_CasterGuardianAiControllerComponent()
  : RPG_AiControllerComponent()
  , m_lastAttackTime(0.0f)
  , m_attackInterval(0.0f)
  , m_attackIntervalMin(2.5f)
  , m_attackIntervalMax(4.0f)
  , m_attackMaxRange(480.0f)
  , m_aoeMinInterval(1.0f)
  , m_lastAoeTime(0.0f)
  , m_wanderOrigin(0.0f, 0.0f, 0.0f)
  , m_wanderAngle(0.0f)
  , m_wanderIdleTime(0.0f)
  , m_healingTarget(NULL)
  , m_healMinInterval(1.0f)
  , m_lastHealTime(0.0f)
  , m_holdOffRange(400.0f)
  , m_fleeMaxDuration(2.0f)
  , m_fleeMinInterval(1.0f)
  , m_lastFleeTime(0.0f)
{
  //m_debugDisplayStateInformation = true;
}

void RPG_CasterGuardianAiControllerComponent::SelectState()
{
  if (IsInState(ST_Spawning))
  {
    return; // don't do anything if I'm not done spawning
  }
  else
  {
    // if we're performing the spawn action, but not yet in the spawn state, set the state
    if (m_characterOwner->GetActionHandler().IsPerformingAction(AT_Spawn))
    {
      SwitchState(ST_Spawning);
      return;
    }
  }

  // if we need to heal anybody, that takes precedence over anything else, but not if we're being crowded
  if(IsReadyToHeal() && !HasHealingTarget() && !IsTargetTooClose())
  {
    if (AcquireHealingTarget())
    {
      SwitchState(ST_Healing);
      return;
    }
  }

  if (!IsInState(ST_Healing))
  {
    // try to find a target to attack if we don't have one and don't need to heal anyone
    if(!HasValidTarget())
    {
      ClearTarget();
      if (AcquireTarget())
      {
        SwitchState(ST_Challenging);
        return;
      }
    }

    // if there are no nearby targets, or if our current target is out of range, just wander
    if(!IsValidTargetInAggroRange())
    {
      ClearTarget();
      if (!IsInState(ST_Wandering))
      {
        SwitchState(ST_Wandering);
        return;
      }
    }
    // if I do have a target in range and I'm not currently challenging it...
    else if(HasValidTarget() && !IsInState(ST_Challenging))
    {
      // if my target is uncomfortably close, and I'm ready to flee...
      if (IsTargetTooClose() && (IsReadyToFlee()))
      {
        // if I'm not already doing AoE attacking or fleeing...
        if (!IsInState(ST_AoeAttacking) && !IsInState(ST_Fleeing))
        {
          // cast an AoE attack before I flee.
          SwitchState(ST_AoeAttacking); // Caster will Flee after AoE attack finishes
          return;
        }
      }
      else if (IsTargetTooFar())
      {
        SwitchState(ST_MovingToPosition);
        return;
      }
      // otherwise, if my target is a comfortable distance from me, shoot at it.
      else if (!IsInState(ST_RangedAttacking))
      {
        SwitchState(ST_RangedAttacking);
        return;
      }
    }
  }
}

/// Tells the guardian to wander around.
void RPG_CasterGuardianAiControllerComponent::UpdateWandering(float const deltaTime)
{
  if(m_isFirstStateUpdate)
  {
    m_wanderOrigin = m_characterOwner->GetPosition();
    m_wanderAngle = Vision::Game.GetFloatRand() * 2.0f * hkvMath::pi();

    m_characterOwner->GetActionHandler().PerformAction(AT_None, true);
  }

  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Move) &&
    m_wanderIdleTime <= 0.f)
  {
    RPG_CasterGuardian* guardian = static_cast<RPG_CasterGuardian*>(m_characterOwner);
    hkReal const velocityThresholdSqr = 0.25f * 0.25f;
    float const twoPi = hkvMath::pi() * 2.0f;

    m_wanderAngle += Vision::Game.GetFloatRand() > 0.5f ? twoPi / 3.1f : -twoPi / 2.9f;
    m_wanderAngle = hkvMath::mod(m_wanderAngle, twoPi);

    // calculate the new destination
    hkVector4 destination;
    RPG_VisionHavokConversion::VisionToHavokPoint(m_wanderOrigin + hkvVec3(hkvMath::cosRad(m_wanderAngle), hkvMath::sinRad(m_wanderAngle), 0.0f) * guardian->GetWanderRadius(), destination);

    hkVector4 closestPoint;
    hkaiPackedKey faceKey = vHavokAiModule::GetInstance()->GetAiWorld()->getDynamicQueryMediator()->getClosestPoint(destination, 10.0f, closestPoint);
    if (faceKey != HKAI_INVALID_PACKED_KEY)
    {
      hkvVec3 vClosestPoint;
      RPG_VisionHavokConversion::HavokToVisionPoint(closestPoint, vClosestPoint);

      m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, NULL, vClosestPoint);

      m_wanderIdleTime = Vision::Game.GetFloatRand() * 5.0f + 5.0f;
    }
  }
  else
  {
    m_wanderIdleTime -= deltaTime;
  }
}

void RPG_CasterGuardianAiControllerComponent::UpdateMovingToPosition(float const deltaTime)
{
  VASSERT(m_characterOwner);
  VASSERT(m_target);

  // move toward my target
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Move))
  {
    m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, m_target);
  }
}

/// Tells the guardian to move to the target and attack them.
void RPG_CasterGuardianAiControllerComponent::UpdateRangedAttacking(float const deltaTime)
{
  // if not performing a Caster attack
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_RangedAttack))
  {
    if(!m_characterOwner->IsTargetWithinAttackRange(m_target))
    {
      // move to the target if they aren't within attack range
      if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Move))
      {
        m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, m_target);
      }
    }
    else if (hkMath::fabs(m_lastAttackTime) < HK_REAL_EPSILON || 
      Vision::GetTimer()->GetTime() - m_lastAttackTime >= m_attackInterval)
    {
      // if this is the first attack, OR it's time to attack, OR the target has run out of range, then attack

      m_characterOwner->GetActionHandler().PerformAction(AT_RangedAttack, true, m_target);
      m_lastAttackTime = Vision::GetTimer()->GetTime();   // @todo: usual caveats about making sure GetTime() pulls a trustworthy server time once networking goes in
      m_attackInterval = m_attackIntervalMin + Vision::Game.GetFloatRand() * (m_attackIntervalMax - m_attackIntervalMin);

      //// @TODO: move this somewhere more appropriate so the timing is better. maybe have it respond to an animation event? also maybe need a local position offset defined some way.
      //RPG_CasterGuardian* caster = static_cast<RPG_CasterGuardian*>(m_characterOwner);
      //const VString& attackEffectName = caster->GetRangedAttackEffectName();

      //if(!attackEffectName.IsEmpty())
      //{
      //  RPG_VisionEffectParticleParams particleEffectParams;
      //  particleEffectParams.m_effectFilename = attackEffectName;
      //  particleEffectParams.m_position = caster->GetPosition();
      //  particleEffectParams.m_orientation = caster->GetOrientation();

      //  RPG_VisionEffectHelper::CreateParticleEffect(particleEffectParams);
      //}
    }
  }
}

void RPG_CasterGuardianAiControllerComponent::UpdateAoeAttacking(float const deltaTime)
{
  if (m_isFirstStateUpdate)
  {
    m_characterOwner->GetActionHandler().PerformAction(AT_AoeAttack, true);
  }
  else
  {
    if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_AoeAttack))
    {
      // AoeAttack action has completed.
      m_lastAoeTime = Vision::GetTimer()->GetTime();   // @todo: usual caveats about making sure GetTime() pulls a trustworthy server time once networking goes in
      SwitchState(ST_Fleeing);
    }
  }
}

void RPG_CasterGuardianAiControllerComponent::UpdateHealing(float const deltaTime)
{
  RPG_CasterGuardian* casterOwner = static_cast<RPG_CasterGuardian*>(m_characterOwner);

  // if the target is dead or fully healed, we can bail
  if(m_healingTarget->IsDead() || m_healingTarget->IsFeigningDeath() || !WantsToHeal(m_healingTarget))
  {
    ClearHealingTarget();
    m_lastHealTime = Vision::GetTimer()->GetTime();   // @todo: usual caveats about making sure GetTime() pulls a trustworthy server time once networking goes in
    SwitchState(ST_None);
    return;
  }

  // if not performing a healing action
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Heal))
  {
    if(!m_characterOwner->IsTargetWithinAttackRange(m_healingTarget))
    {
      // move to the target if they aren't within healing range
      if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Move))
      {
        m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, m_healingTarget);
      }
    }
    else
    {
      // perform the heal action
      m_characterOwner->GetActionHandler().PerformAction(AT_Heal, true, m_healingTarget);

      // @TODO: move this somewhere more appropriate so the timing is better. maybe have it respond to an animation event? also maybe need a local position offset defined some way.
      //const VString& healingEffectName = casterOwner->GetHealingEffectName();

      //if(!healingEffectName.IsEmpty())
      //{
      //  RPG_VisionEffectParticleParams particleEffectParams;
      //  particleEffectParams.m_effectFilename = healingEffectName;
      //  particleEffectParams.m_position = casterOwner->GetPosition();
      //  particleEffectParams.m_orientation = casterOwner->GetOrientation();

      //  RPG_VisionEffectHelper::CreateParticleEffect(particleEffectParams);
      //}
    }
  }
}

/// Finds a target to heal
bool RPG_CasterGuardianAiControllerComponent::AcquireHealingTarget()
{
  RPG_CasterGuardian* casterOwner = static_cast<RPG_CasterGuardian*>(m_characterOwner);

  VArray<RPG_Character*> const& characters = RPG_GameManager::s_instance.GetCharacters();

  hkvVec3 const& currentPosition = m_characterOwner->GetPosition();

  for(int index = 0; index < characters.GetSize(); ++index)
  {
    RPG_Character* character = characters.GetAt(index);

    // check whether or not we can target ourself
    if(character == m_characterOwner &&
       !casterOwner->CanHealSelf())
    {
      continue;
    }

    // ignore players and dead characters
    // @TODO: fancier filtering once we have teams
    if(character->IsOfType(V_RUNTIME_CLASS(RPG_PlayerCharacter)) ||
       character->IsDead() ||
       character->IsFeigningDeath())
    {
      continue;
    }

    float aggroRadius = m_characterOwner->GetAggroRadius();
    hkvVec3 const& targetPosition = character->GetPosition();

    float currentRangeSquared = (currentPosition - targetPosition).getLengthSquared();

    // check if they are within range
    if(currentRangeSquared <= aggroRadius * aggroRadius)
    {
      // check if they're below the healing threshold
      if(WantsToHeal(character))
      {
        m_healingTarget = character;
        return true;
      }
    }
  }
  return false;
}

void RPG_CasterGuardianAiControllerComponent::UpdateFleeing(float const deltaTime)
{
  VASSERT(m_characterOwner);
  VASSERT(m_target);

  bool const fleeing = FleeFromPosition(m_target->GetPosition());
  bool const fleeTimedOut = Vision::GetTimer()->GetTime() - m_stateStartTime >= m_fleeMaxDuration;

  // exit the state if we're no longer in flee range, if we failed to find a valid flee point, or if we timed out
  if (!m_characterOwner->IsTargetWithinRange(m_target, m_holdOffRange) || !fleeing || fleeTimedOut)
  {
    m_lastFleeTime = Vision::GetTimer()->GetTime();   // @todo: usual caveats about making sure GetTime() pulls a trustworthy server time once networking goes in
    SwitchState(ST_None);
  }
}

bool RPG_CasterGuardianAiControllerComponent::WantsToHeal(const RPG_Character* healingTarget) const
{
  RPG_CasterGuardian* casterOwner = static_cast<RPG_CasterGuardian*>(m_characterOwner);
  VASSERT(casterOwner);
  return healingTarget->GetCharacterStats().GetHealthPercentage() < casterOwner->GetHealingThresholdPercentage() * 0.01f;
}

bool RPG_CasterGuardianAiControllerComponent::HasHealingTarget() const
{
  return m_healingTarget != NULL;
}

void RPG_CasterGuardianAiControllerComponent::ClearHealingTarget()
{
  m_healingTarget = NULL;
}

bool RPG_CasterGuardianAiControllerComponent::IsReadyToHeal() const
{
  return Vision::GetTimer()->GetTime() - m_lastHealTime >= m_healMinInterval;
}

bool RPG_CasterGuardianAiControllerComponent::IsTargetTooClose() const
{
  if (m_target)
  {
    return m_characterOwner->IsTargetWithinRange(m_target, m_holdOffRange);
  }
  return false;
}

bool RPG_CasterGuardianAiControllerComponent::IsTargetTooFar() const
{
  if (m_target)
  {
    return !m_characterOwner->IsTargetWithinRange(m_target, m_attackMaxRange);  // @todo: get this value from the weapon's range?
  }
  return false;
}

bool RPG_CasterGuardianAiControllerComponent::IsReadyToFlee() const
{
  return Vision::GetTimer()->GetTime() - m_lastFleeTime >= m_fleeMinInterval;
}

//bool RPG_CasterGuardianAiControllerComponent::IsValidTargetInAggroRange() const
//{
//  return m_target && IsTargetWithinRange(m_characterOwner->GetAggroRadius());
//}

bool RPG_CasterGuardianAiControllerComponent::IsReadyToAoeAttack() const
{
  return Vision::GetTimer()->GetTime() - m_lastAoeTime >= m_aoeMinInterval;
}

bool RPG_CasterGuardianAiControllerComponent::ShouldAoeAttack() const
{
  return Vision::Game.GetFloatRand() < 0.3f;
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
