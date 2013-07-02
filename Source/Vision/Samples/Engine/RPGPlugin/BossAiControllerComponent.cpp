/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/BossAiControllerComponent.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/Boss.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

V_IMPLEMENT_SERIAL(RPG_BossAiControllerComponent, RPG_AiControllerComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_BossAiControllerComponent, RPG_AiControllerComponent, "(RPG) Boss AI Controller", 0, "(RPG) Boss AI Controller")
END_VAR_TABLE

  // RPG_GuardianAiControllerComponent
RPG_BossAiControllerComponent::RPG_BossAiControllerComponent()
  : RPG_AiControllerComponent()
  , m_lastAttackTime(0.0f)
  , m_attackInterval(0.0f)
  , m_attackIntervalMin(2.5f)
  , m_attackIntervalMax(4.0f)
  , m_rangedAttackRange(700.f)
  , m_wanderOrigin(0.0f, 0.0f, 0.0f)
  , m_wanderAngle(0.0f)
  , m_wanderIdleTime(0.0f)
  , m_holdOffRange(300.0f)
  , m_fleeMaxDuration(2.0f)
  , m_fleeMinInterval(1.0f)
  , m_lastFleeTime(0.0f)
  , m_aoeMinInterval(1.0f)
  , m_lastAoeTime(0.0f)
{
}

void RPG_BossAiControllerComponent::SelectState()
{
  // try to find a target if we don't have one
  if(!HasValidTarget())
  {
    ClearTarget();
    if (AcquireTarget())
    {
      SwitchState(ST_Challenging);
      return;
    }
  }

  if (IsInState(ST_Challenging))
  {
    return;
  }

  if (!IsValidTargetInAggroRange())
  {
    // if there are no nearby targets, or if our current target is out of range, just wander
    ClearTarget();
    if (!IsInState(ST_Wandering))
    {
      SwitchState(ST_Wandering);
      return;
    }
  }

  if(HasValidTarget())
  {
    RPG_AIState_e selectedAttack = ChooseAttack();
    if (!IsInState(selectedAttack))
    {
      SwitchState(selectedAttack);
    }
  }
}

RPG_AIState_e RPG_BossAiControllerComponent::ChooseAttack() const
{
  VASSERT(m_characterOwner);
  VASSERT(m_target);

  // holdoff takes precedence over everything
  if (m_characterOwner->IsTargetWithinRange(m_target, m_holdOffRange))
  {
    return ST_AoeAttacking;
  }

  // if target is beyond ranged attack range, close distance
  if (!m_characterOwner->IsTargetWithinRange(m_target, m_rangedAttackRange))
  {
    return ST_MovingToPosition;
  }

  if (m_characterOwner->IsTargetWithinRange(m_target, m_characterOwner->GetEquippedWeaponRange()))
  {
    return ST_MeleeAttacking;
  }

  if (m_characterOwner->IsTargetWithinRange(m_target, m_rangedAttackRange))
  {
    return ST_RangedAttacking;
  }

  return ST_None;
}

/// Tells the guardian to wander around.
void RPG_BossAiControllerComponent::UpdateWandering(float const deltaTime)
{
  VASSERT(m_characterOwner);

  if(!m_isFirstStateUpdate)
  {
    m_wanderOrigin = m_characterOwner->GetPosition();
    m_wanderAngle = Vision::Game.GetFloatRand() * 2.0f * hkvMath::pi();

    m_characterOwner->GetActionHandler().PerformAction(AT_None, true);
  }

  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Move) &&
    m_wanderIdleTime <= 0.f)
  {
    RPG_Boss* guardian = static_cast<RPG_Boss*>(m_characterOwner);
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

/// Tells the guardian to move to the target and attack them.
void RPG_BossAiControllerComponent::UpdateMeleeAttacking(float const deltaTime)
{
  VASSERT(m_target);

  // if not performing a melee attack
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_MeleeAttack))
  {
    if(!HasValidTargetInMeleeRange())
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
    }
  }
}

void RPG_BossAiControllerComponent::UpdateRangedAttacking(float const deltaTime)
{
  VASSERT(m_characterOwner);
  VASSERT(m_target);

  // if not performing a ranged attack
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_RangedAttack))
  {
    if(!m_characterOwner->IsTargetWithinRange(m_target, m_rangedAttackRange))
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
    }
  }
}

void RPG_BossAiControllerComponent::UpdateAoeAttacking(float const deltaTime)
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
      SwitchState(ST_None);
    }
  }
}

void RPG_BossAiControllerComponent::UpdateMovingToPosition(float const deltaTime)
{
  VASSERT(m_characterOwner);
  VASSERT(m_target);

  // move toward my target
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Move))
  {
    m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, m_target);
  }
}

void RPG_BossAiControllerComponent::UpdateFleeing(float const deltaTime)
{
  VASSERT(m_characterOwner);
  VASSERT(m_target);

  bool const fleeing = FleeFromPosition(m_target->GetPosition());
  bool const fleeTimedOut = Vision::GetTimer()->GetTime() - m_stateStartTime >= m_fleeMaxDuration;

  // exit the state if we're no longer in flee range, if we failed to find a valid flee point, or if we timed out
  if (!m_characterOwner->IsTargetWithinRange(m_target, m_holdOffRange) || !fleeing || fleeTimedOut)
  {
    m_lastFleeTime = Vision::GetTimer()->GetTime();
    SwitchState(ST_None);
  }
}

bool RPG_BossAiControllerComponent::HasValidTargetInMeleeRange() const
{
  VASSERT(m_characterOwner);

  if (m_target && m_characterOwner->IsTargetWithinAttackRange(m_target))
  {
    return true;
  }
  return false;
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
