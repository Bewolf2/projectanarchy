/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/RangedGuardianAiControllerComponent.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/RangedGuardian.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

V_IMPLEMENT_SERIAL(RPG_RangedGuardianAiControllerComponent, RPG_AiControllerComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_RangedGuardianAiControllerComponent, RPG_AiControllerComponent, "(RPG) Ranged Guardian AI Controller", 0, "(RPG) Ranged Guardian AI Controller")
END_VAR_TABLE

  // RPG_GuardianAiControllerComponent
RPG_RangedGuardianAiControllerComponent::RPG_RangedGuardianAiControllerComponent()
  : RPG_AiControllerComponent()
  , m_lastAttackTime(0.0f)
  , m_attackInterval(0.0f)
  , m_attackIntervalMin(2.5f)
  , m_attackIntervalMax(4.0f)
  , m_attackMaxRange(480)
  , m_attackLineOfSightPosition(0.0f, 0.0f, 0.0f)
  , m_attackMovingToLineOfSight(false)
  , m_attackMovingToLineOfSightStartTime(0.0f)
  , m_wanderOrigin(0.0f, 0.0f, 0.0f)
  , m_wanderAngle(0.0f)
  , m_wanderIdleTime(0.0f)
  , m_holdOffRange(200.0f)
  , m_fleeMaxDuration(2.0f)
  , m_fleeMinInterval(1.0f)
  , m_lastFleeTime(0.0f)
{
  //m_debugDisplayStateInformation = true;
}

void RPG_RangedGuardianAiControllerComponent::SelectState()
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

  if(!IsValidTargetInAggroRange())
  {
    // if there are no nearby targets, or if our current target is out of range, just wander
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
    if (IsTargetTooClose() && IsReadyToFlee() && !m_attackMovingToLineOfSight)
    {
      SwitchState(ST_Fleeing);
      return;
    }
    else if (IsTargetTooFar())
    {
      SwitchState(ST_MovingToPosition);
      return;
    }
    else if (m_attackMovingToLineOfSight)
    {
      SwitchState(ST_MovingToPosition);
      return;
    }
    else if (!IsInState(ST_RangedAttacking))
    {
      SwitchState(ST_RangedAttacking);
      return;
    }
  }
}

/// Tells the guardian to wander around.
void RPG_RangedGuardianAiControllerComponent::UpdateWandering(float const deltaTime)
{
  // initialize the wander state
  if(m_isFirstStateUpdate)
  {
    m_wanderOrigin = m_characterOwner->GetPosition();
    m_wanderAngle = Vision::Game.GetFloatRand() * 2.0f * hkvMath::pi();

    m_characterOwner->GetActionHandler().PerformAction(AT_None, true);
  }

  // update the wander state
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_Move) &&
    m_wanderIdleTime <= 0.f)
  {
    RPG_RangedGuardian* guardian = static_cast<RPG_RangedGuardian*>(m_characterOwner);
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

void RPG_RangedGuardianAiControllerComponent::UpdateMovingToPosition(float const deltaTime)
{
  VASSERT(m_characterOwner);
  VASSERT(m_target);

  if(HasLineOfSightToTarget())
  {
    m_attackMovingToLineOfSight = false;
  }

  float const elapsedTime = Vision::GetTimer()->GetTime() - m_attackMovingToLineOfSightStartTime;
  if(elapsedTime >= 0.3f && !m_characterOwner->GetActionHandler().IsPerformingAction(AT_Move))
  {
    // probably stuck
    m_attackMovingToLineOfSight = false;
  }

  if(IsTargetTooFar())
  {
    m_attackMovingToLineOfSight = false;
  }

  if(m_attackMovingToLineOfSight)
  {
    if (m_characterOwner->ShouldDisplayDebugStateInformation())
    {
      Vision::Game.DrawSingleLine(m_characterOwner->GetPosition(), m_attackLineOfSightPosition, VColorRef(208, 24, 208), 3.0f);
    }

    m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, NULL, m_attackLineOfSightPosition);
  }

  // move toward my target
  else
  {
    m_characterOwner->GetActionHandler().PerformAction(AT_Move, false, m_target);
  }
}

/// Tells the guardian to move to the target and attack them.
void RPG_RangedGuardianAiControllerComponent::UpdateRangedAttacking(float const deltaTime)
{
  // if not performing a ranged attack
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_RangedAttack))
  {
    bool left, center, right;
    if(!HasLineOfSightToTarget(left, center, right))
    {
      // determine movement direction
      hkvVec3 vectorFromTarget = m_characterOwner->GetPosition() - m_target->GetPosition();
      float const lineOfSightMoveAngle = HKVMATH_PI * 0.25;
      hkvVec3 const lineOfSightLeftVectorFromTarget = hkvVec3(
        vectorFromTarget.x * hkvMath::cosRad(lineOfSightMoveAngle) + vectorFromTarget.y * hkvMath::sinRad(lineOfSightMoveAngle),
        vectorFromTarget.y * hkvMath::cosRad(lineOfSightMoveAngle) - vectorFromTarget.x * hkvMath::sinRad(lineOfSightMoveAngle),
        0.0f);
      hkvVec3 const lineOfSightOffsetLeft = lineOfSightLeftVectorFromTarget - vectorFromTarget;
      hkvVec3 const lineOfSightRightVectorFromTarget = hkvVec3(
        vectorFromTarget.x * hkvMath::cosRad(-lineOfSightMoveAngle) + vectorFromTarget.y * hkvMath::sinRad(-lineOfSightMoveAngle),
        vectorFromTarget.y * hkvMath::cosRad(-lineOfSightMoveAngle) - vectorFromTarget.x * hkvMath::sinRad(-lineOfSightMoveAngle),
        0.0f);
      hkvVec3 const lineOfSightOffsetRight = lineOfSightRightVectorFromTarget - vectorFromTarget;

      float distanceToNearestObjectOnLeft = FLT_MAX;
      float distanceToNearestObjectOnRight = FLT_MAX;

      VisPhysicsRaycastClosestResult_cl result;
      result.vRayStart = 0.5f * (m_characterOwner->GetEyePosition() + m_characterOwner->GetPosition());
      result.vRayEnd = result.vRayStart + lineOfSightOffsetLeft;
      Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&result);
      if(result.bHit)
      {
        distanceToNearestObjectOnLeft = result.closestHit.fHitFraction;
      }

      result.Reset();
      result.vRayStart = 0.5f * (m_characterOwner->GetEyePosition() + m_characterOwner->GetPosition());
      result.vRayEnd = result.vRayStart + lineOfSightOffsetRight;
      Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&result);
      if(result.bHit)
      {
        distanceToNearestObjectOnRight = result.closestHit.fHitFraction;
      }

      if(distanceToNearestObjectOnLeft > distanceToNearestObjectOnRight)
      {
        m_attackLineOfSightPosition = m_characterOwner->GetPosition() + lineOfSightOffsetLeft;
      }
      else if(distanceToNearestObjectOnLeft < distanceToNearestObjectOnRight)
      {
        m_attackLineOfSightPosition = m_characterOwner->GetPosition() + lineOfSightOffsetRight;
      }
      else if(left && !right)
      {
        m_attackLineOfSightPosition = m_characterOwner->GetPosition() + lineOfSightOffsetLeft;
      }
      else if(right && !left)
      {
        m_attackLineOfSightPosition = m_characterOwner->GetPosition() + lineOfSightOffsetRight;
      }
      else
      {
        if(Vision::Game.GetFloatRand() > 0.5f)
        {
          m_attackLineOfSightPosition = m_characterOwner->GetPosition() + lineOfSightOffsetLeft;
        }
        else
        {
          m_attackLineOfSightPosition = m_characterOwner->GetPosition() + lineOfSightOffsetRight;
        }
      }

      hkVector4 destination;
      RPG_VisionHavokConversion::VisionToHavokPoint(m_attackLineOfSightPosition, destination);

      hkVector4 closestPoint;
      hkaiPackedKey faceKey = vHavokAiModule::GetInstance()->GetAiWorld()->getDynamicQueryMediator()->getClosestPoint(destination, 10.0f, closestPoint);
      if (faceKey != HKAI_INVALID_PACKED_KEY)
      {
        RPG_VisionHavokConversion::HavokToVisionPoint(closestPoint, m_attackLineOfSightPosition);

        m_attackMovingToLineOfSight = true;
        m_attackMovingToLineOfSightStartTime = Vision::GetTimer()->GetTime();
        SwitchState(ST_None);
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

void RPG_RangedGuardianAiControllerComponent::UpdateFleeing(float const deltaTime)
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

bool RPG_RangedGuardianAiControllerComponent::IsTargetTooClose() const
{
  if (m_target)
  {
    return m_characterOwner->IsTargetWithinRange(m_target, m_holdOffRange);
  }
  return false;
}

bool RPG_RangedGuardianAiControllerComponent::IsTargetTooFar() const
{
  if (m_target)
  {
    return !m_characterOwner->IsTargetWithinRange(m_target, m_attackMaxRange);  // @todo: get this value from the weapon's range?
  }
  return false;
}

bool RPG_RangedGuardianAiControllerComponent::IsReadyToFlee() const
{
  return Vision::GetTimer()->GetTime() - m_lastFleeTime >= m_fleeMinInterval;
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
