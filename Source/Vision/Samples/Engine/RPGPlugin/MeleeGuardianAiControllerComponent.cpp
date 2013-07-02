/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/MeleeGuardianAiControllerComponent.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/MeleeGuardian.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

V_IMPLEMENT_SERIAL(RPG_MeleeGuardianAiControllerComponent, RPG_AiControllerComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_MeleeGuardianAiControllerComponent, RPG_AiControllerComponent, "(RPG) Guardian AI Controller", 0, "(RPG) Guardian AI Controller")
END_VAR_TABLE

// RPG_GuardianAiControllerComponent
RPG_MeleeGuardianAiControllerComponent::RPG_MeleeGuardianAiControllerComponent()
  : RPG_AiControllerComponent()
  , m_lastAttackTime(0.0f)
  , m_attackInterval(0.0f)
  , m_attackIntervalMin(2.5f)
  , m_attackIntervalMax(4.0f)
  , m_wanderOrigin(0.0f, 0.0f, 0.0f)
  , m_wanderAngle(0.0f)
  , m_wanderIdleTime(0.0f)
{
}

void RPG_MeleeGuardianAiControllerComponent::SelectState()
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
    }
  }

  if(!IsValidTargetInAggroRange())
  {
    // if there are no nearby targets, or if our current target is out of range, just wander
    ClearTarget();
    if (!IsInState(ST_Wandering))
    {
      SwitchState(ST_Wandering);
    }
  }
  else if(HasValidTarget() && !IsInState(ST_Challenging))
  {
    if (!IsInState(ST_MeleeAttacking))
    {
      SwitchState(ST_MeleeAttacking);
    }
  }
}

/// Tells the guardian to wander around.
void RPG_MeleeGuardianAiControllerComponent::UpdateWandering(float const deltaTime)
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
    RPG_MeleeGuardian* guardian = static_cast<RPG_MeleeGuardian*>(m_characterOwner);
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
void RPG_MeleeGuardianAiControllerComponent::UpdateMeleeAttacking(float const deltaTime)
{
  // if not performing a melee attack
  if(!m_characterOwner->GetActionHandler().IsPerformingAction(AT_MeleeAttack))
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
      // if this is the first attack, OR it's time to attack, OR the target has run out of melee range, then attack

      m_characterOwner->GetActionHandler().PerformAction(AT_MeleeAttack, true, m_target);
      m_lastAttackTime = Vision::GetTimer()->GetTime();   // @todo: usual caveats about making sure GetTime() pulls a trustworthy server time once networking goes in
      m_attackInterval = m_attackIntervalMin + Vision::Game.GetFloatRand() * (m_attackIntervalMax - m_attackIntervalMin);
    }
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
