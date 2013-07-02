/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionPowerAttack.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionAttackBase.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/Explosion.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

RPG_Action_PowerAttack::RPG_Action_PowerAttack(RPG_Character *owner)
  : RPG_Action_AttackBase(AT_PowerAttack, owner)
{
  m_behaviorTriggerVarName = "MeleePowerAttacking";
  m_behaviorFireEventName = "POWER_ATTACK_FIRE";

  m_behaviorWeaponTrailStartEventName = "MELEE_WEAPON_ACTIVE_START";
  m_behaviorWeaponTrailEndEventName = "MELEE_WEAPON_ACTIVE_END";

  //m_debugDisplayInfo = true;
}

void RPG_Action_PowerAttack::Tick(float const deltaTime)
{
  RPG_Action_AttackBase::Tick(deltaTime);

  // MeleePowerAttacking was set to true when the attack was initiated. Is reset to false by Behavior when the attack anim finishes.
  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  VASSERT(!m_behaviorTriggerVarName.IsEmpty());
  if (!RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, m_behaviorTriggerVarName.AsChar()))
  {
    if (!m_hasFired)
    {
      VString msg;
      msg.Format("RPG_Action_PowerAttack never received expected %s event from Behavior.", m_behaviorFireEventName.AsChar());
      Vision::Error.Warning(msg.AsChar());
      //VASSERT_MSG(false, msg.AsChar());
      FireAttack();
    }
    End();
  }
  else
  {
    // we're currently attacking
    hkvVec3 const targetPosition = m_interactionEntity ? m_interactionEntity->GetPosition() : m_interactionPosition;
    FaceTargetPoint(targetPosition);

#ifdef _DEBUG
    if (m_debugDisplayInfo)
    {
      DebugDrawPlanarFrustum(
        m_characterOwner->GetPosition(), 
        m_characterOwner->GetCollisionRadius(),
        m_characterOwner->GetCharacterActionData().GetPowerAttackRange(), 
        m_characterOwner->GetCharacterActionData().GetPowerAttackAngle()
        );
    }
#endif
  }
}

void RPG_Action_PowerAttack::StartAttack()
{
  RPG_Action_AttackBase::StartAttack();

  m_characterOwner->CreateCharacterEffect(FX_PowerAttackSwing);
}

void RPG_Action_PowerAttack::FireAttack()
{
  RPG_Action_AttackBase::FireAttack();

  // attempt to damage the target
  m_outgoingDamage = CalculateDamage(m_characterOwner->GetCharacterActionData().GetPowerAttackDamageMultiplier());
  PopulateTargetList();
  TryDealDamage();

  // trigger effects
  m_characterOwner->CreateCharacterEffect(FX_PowerAttack);

#if(RPG_USE_EXPLOSIONS)
  hkvVec3 const& position = m_characterOwner->GetPosition();
  hkvVec3 const& direction = m_characterOwner->GetDirection();
  float const length = m_characterOwner->GetCharacterActionData().GetPowerAttackRange();
  float const strength = 8000.0f; // XXX - data drive this
  float const radius = 300.0f; // XXX - calculate this based on power attack range and angle
  RPG_ExplosionManager::s_instance.CreateExplosion(position, ET_HerringBone, strength, radius, length, direction);
#endif // RPG_USE_EXPLOSIONS

  ShakeCamera(50.f, 0.08f);
}

int RPG_Action_PowerAttack::CalculateDamage(float const damageMultiplier) const
{
  // Outgoing damage is calculated as follows:
  // Character's active weapon carries a min and max damage factor, which is multiplied against the character's base damage.
  // A random value between these calculated min and max values is then returned.
  VASSERT(m_characterOwner);
  const float characterBaseDamage = static_cast<float>(m_characterOwner->GetCharacterStats().GetBaseDamage());

  const RPG_InventoryItem* equippedWeapon = m_characterOwner->GetInventoryHandler().GetEquippedWeapon();
  if (equippedWeapon)
  {
    const float damageMin = characterBaseDamage * equippedWeapon->GetMinDamage();
    const float damageMax = characterBaseDamage * equippedWeapon->GetMaxDamage();
    const float damage = ((damageMin + Vision::Game.GetFloatRand() * (damageMax - damageMin)) + 0.5f) * damageMultiplier;
    return static_cast<int>(damage);
  }
  return 0;
}

void RPG_Action_PowerAttack::PopulateTargetList()
{
  // find potential targets in range
  m_targets.RemoveAll(); // ensure that the target list is empty before we attempt to populate it.
  VArray<RPG_DamageableEntity*> const& attackableEntities = RPG_GameManager::s_instance.GetAttackableEntities();
  for (int i = 0; i < attackableEntities.GetSize(); ++i)
  {
    RPG_DamageableEntity* attackableEntity = attackableEntities.GetAt(i);

    // ignore self
    if (attackableEntity == m_characterOwner)
    {
      continue;
    }

    if (IsTargetInAttackFrustum(attackableEntity))
    {
      m_targets.Add(attackableEntity);
    }
  }
}

bool RPG_Action_PowerAttack::IsTargetInAttackFrustum(RPG_DamageableEntity* const target)
{
  float const attackRange = m_characterOwner->GetCharacterActionData().GetPowerAttackRange();
  float const attackAngle = m_characterOwner->GetCharacterActionData().GetPowerAttackAngle();
  hkvVec3 const& characterOwnerPosition = m_characterOwner->GetPosition();
  hkvVec3 const& characterOwnerFacing = m_characterOwner->GetDirection();
  hkvVec3 const& targetPosition = target->GetPosition();

  // find range to target character
  float currentRangeSquared = (characterOwnerPosition - targetPosition).getLengthSquared();
  if (currentRangeSquared <= attackRange * attackRange)
  {
    // if target is in range, is target within angle of a frustum offset behind the character owner?
    float const frustumBaseOffset = m_characterOwner->GetCollisionRadius() / hkvMath::tanDeg(attackAngle);  // move the frustum base behind the character such that the walls of the frustum will align to the character's width
    hkvVec3 const frustumBase = characterOwnerPosition - frustumBaseOffset * characterOwnerFacing;
    hkvVec3 frustumBaseToTarget = target->GetPosition() - frustumBase;
    frustumBaseToTarget.z = 0.f; // get rid of the up component
    frustumBaseToTarget.normalizeIfNotZero();

    if (characterOwnerFacing.dot(frustumBaseToTarget) >= hkvMath::cosDeg(attackAngle))
    {
      // target is within the angle of the frustum base, now is the target in front of my character owner?
      hkvVec3 characterOwnerToTarget = target->GetPosition() - characterOwnerPosition;
      characterOwnerToTarget.z = 0.f;
      if (characterOwnerFacing.dot(characterOwnerToTarget) > 0) // no need to normalize, since we only care about the sign of the dot.
      {
        return true;
      }
    }
  }
  return false;
}

void RPG_Action_PowerAttack::TryDealDamage()
{
  float impactSpeed = 0.0f;
  if(m_characterOwner)
  {
    RPG_InventoryItem const* equippedWeapon = m_characterOwner->GetInventoryHandler().GetEquippedWeapon();
    RPG_CharacterActionData const& actionData = m_characterOwner->GetCharacterActionData();
    if(equippedWeapon)
    {
      impactSpeed = equippedWeapon->GetImpactSpeed() * actionData.GetPowerAttackImpactSpeedMultiplier();
    }
  }

  // deal damage to found targets
  for (int i = 0; i < m_targets.GetSize(); ++i)
  {
    RPG_DamageableEntity* attackableEntity = m_targets.GetAt(i);
    VASSERT(attackableEntity);
    
    hkvVec3 attackLine = m_characterOwner->GetDirection();
    attackLine.z = 0.0f;
    attackLine.normalize();
    float const lineProjection = attackableEntity->GetPosition().dot(attackLine) - m_characterOwner->GetPosition().dot(attackLine);
    hkvVec3 const& closestPointOnAttackLine = m_characterOwner->GetPosition() + lineProjection * attackLine;
    hkvVec3 impactDirection = (attackableEntity->GetPosition() - closestPointOnAttackLine);
    impactDirection.z = 0.0f;
    if(impactDirection.normalizeIfNotZero() != HKV_SUCCESS)
    {
      if(Vision::Game.GetFloatRand() > 0.5)
      {
        impactDirection.x = attackLine.y;
        impactDirection.y = -attackLine.x;
      }
      else
      {
        impactDirection.x = -attackLine.y;
        impactDirection.y = -attackLine.x;
      }
    }
    impactDirection += attackLine;
    impactDirection.normalize();

    attackableEntity->TakeDamage(m_outgoingDamage, impactDirection * impactSpeed);

    // apply knock back to characters
    RPG_InventoryItem const* equippedWeapon = m_characterOwner->GetInventoryHandler().GetEquippedWeapon();
    RPG_CharacterActionData const& actionData = m_characterOwner->GetCharacterActionData();
    float const knockBackStrength = equippedWeapon->GetKnockBackStrength() * actionData.GetPowerAttackKnockBackStrengthMultiplier();
    float const knockBackDuration = equippedWeapon->GetKnockBackDuration() * actionData.GetPowerAttackKnockBackDurationMultiplier();

    attackableEntity->ApplyKnockBack(impactDirection * knockBackStrength, knockBackDuration);

#ifdef _DEBUG
    if (m_debugDisplayInfo)
    {
      Vision::Game.DrawSingleLine(m_characterOwner->GetPosition(), attackableEntity->GetPosition(), VColorRef(208, 24, 24), 3.0f);
    }
#endif

    if (OwnerIsLocalPlayer())
    {
      // create a screen shake effect when local player hits an enemy
      ShakeCamera(50.f, 0.08f);
    }
  }
}

void RPG_Action_PowerAttack::DebugDrawPlanarFrustum(hkvVec3 const& pos, float const baseWidth, float const range, float const angle)
{
  // draw the attack range and angle
  hkvVec3 vForward = m_characterOwner->GetDirection();
  vForward.z = 0.f;
  vForward.normalizeIfNotZero();

  hkvVec3 vRight = hkvVec3(0.f, 0.f, 0.f);
  vRight.x = vForward.x * hkvMath::cosDeg(angle) - vForward.y * hkvMath::sinDeg(angle);
  vRight.y = vForward.y * hkvMath::cosDeg(angle) + vForward.x * hkvMath::sinDeg(angle);

  hkvVec3 vLeft = hkvVec3(0.f, 0.f, 0.f);
  vLeft.x = vForward.x * hkvMath::cosDeg(-angle) - vForward.y * hkvMath::sinDeg(-angle);
  vLeft.y = vForward.y * hkvMath::cosDeg(-angle) + vForward.x * hkvMath::sinDeg(-angle);

  float const frustumBaseOffset = baseWidth / hkvMath::tanDeg(angle);
  hkvVec3 const frustumBase = pos - frustumBaseOffset * vForward;
  float const legLength = (range + frustumBaseOffset) / hkvMath::cosDeg(angle);

  Vision::Game.DrawSingleLine(frustumBase, frustumBase + vForward * (range + frustumBaseOffset), VColorRef(128, 128, 128));
  Vision::Game.DrawSingleLine(frustumBase, frustumBase + vRight * legLength, VColorRef(24, 208, 24));
  Vision::Game.DrawSingleLine(frustumBase, frustumBase + vLeft * legLength, VColorRef(208, 24, 24));
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
