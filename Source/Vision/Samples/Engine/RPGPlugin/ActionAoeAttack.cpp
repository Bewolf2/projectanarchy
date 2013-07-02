/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionAoeAttack.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionAttackBase.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/Explosion.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

RPG_Action_AoeAttack::RPG_Action_AoeAttack(RPG_Character *owner)
  : RPG_Action_AttackBase(AT_AoeAttack, owner)
{
  m_behaviorTriggerVarName = "AoeAttacking";
  m_behaviorFireEventName = "AOE_FIRE";

  m_behaviorWeaponTrailStartEventName = "MELEE_WEAPON_ACTIVE_START";
  m_behaviorWeaponTrailEndEventName = "MELEE_WEAPON_ACTIVE_END";

  m_debugDisplayInfo = true;
}

void RPG_Action_AoeAttack::Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition)
{
  RPG_Action_AttackBase::Start(forced, prevAction, interactionEntity, interactionPosition);

  m_characterOwner->CreateCharacterEffect(FX_AoeAttackBegin);
  m_characterOwner->CreateCharacterEffect(FX_AoeAttackChargeLoop);
}

void RPG_Action_AoeAttack::Cleanup(RPG_ActionType_e nextAction)
{
  // kill the charge loop effect if it's still running
  if (m_characterOwner->GetPersistentCharacterEffect(FX_AoeAttackChargeLoop))
  {
    m_characterOwner->PauseCharacterEffect(FX_AoeAttackChargeLoop);
  }

  RPG_Action_AttackBase::Cleanup(nextAction);
}

void RPG_Action_AoeAttack::Tick(float const deltaTime)
{
  RPG_Action_AttackBase::Tick(deltaTime);

  // AoeAttacking was set to true when the attack was initiated. Is reset to false by Behavior when the attack anim finishes.
  vHavokBehaviorComponent const *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  VASSERT(!m_behaviorTriggerVarName.IsEmpty());
  if (!RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, m_behaviorTriggerVarName.AsChar()))
  {
    if (!m_hasFired)
    {
      VString msg;
      msg.Format("RPG_Action_AoeAttack never received expected %s event from Behavior.", m_behaviorFireEventName.AsChar());
      Vision::Error.Warning(msg.AsChar());
      //VASSERT_MSG(false, msg.AsChar());
      FireAttack();
    }
    End();
  }
}

void RPG_Action_AoeAttack::StartAttack()
{
  RPG_Action_AttackBase::StartAttack();
}

void RPG_Action_AoeAttack::FireAttack()
{
  RPG_Action_AttackBase::FireAttack();

  m_outgoingDamage = CalculateDamage(m_characterOwner->GetCharacterActionData().GetAoeAttackDamageMultiplier());
  PopulateTargetList();
  TryDealDamage();

  // play any visual and/or sound effects associated with this action
  m_characterOwner->PauseCharacterEffect(FX_AoeAttackChargeLoop);
  m_characterOwner->CreateCharacterEffect(FX_AoeAttackFire);
  
#if(RPG_USE_EXPLOSIONS)
  float const aoeRadius = m_characterOwner->GetCharacterActionData().GetAoeAttackRange();
  hkvVec3 const& currentPosition = m_characterOwner->GetPosition();
  float const strength = 5000.0f; // XXX - data drive this
  float const radius = aoeRadius * 2.0f; // XXX - calculate this without magic numbers?
  RPG_ExplosionManager::s_instance.CreateExplosion(currentPosition, ET_Sphere, strength, radius);
#endif // RPG_USE_EXPLOSIONS

  ShakeCamera(50.f, 0.08f);

}

int RPG_Action_AoeAttack::CalculateDamage(float const damageMultiplier) const
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

void RPG_Action_AoeAttack::PopulateTargetList()
{
  float const aoeRadius = m_characterOwner->GetCharacterActionData().GetAoeAttackRange();
  hkvVec3 const& currentPosition = m_characterOwner->GetPosition();

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

    // find range to target character
    hkvVec3 const& targetPosition = attackableEntity->GetPosition();
    float currentRangeSquared = (currentPosition - targetPosition).getLengthSquared();
    if (currentRangeSquared <= aoeRadius * aoeRadius)
    {
      m_targets.Add(attackableEntity);
    }
  }
}

void RPG_Action_AoeAttack::TryDealDamage()
{
  float impactSpeed = 0.0f;
  if(m_characterOwner)
  {
    RPG_InventoryItem const* equippedWeapon = m_characterOwner->GetInventoryHandler().GetEquippedWeapon();
    RPG_CharacterActionData const& actionData = m_characterOwner->GetCharacterActionData();
    if(equippedWeapon)
    {
      impactSpeed = equippedWeapon->GetImpactSpeed() * actionData.GetAoeAttackImpactSpeedMultiplier();
    }
  }

  // deal damage to found targets
  for (int i = 0; i < m_targets.GetSize(); ++i)
  {
    RPG_DamageableEntity* attackableEntity = m_targets.GetAt(i);
    VASSERT(attackableEntity);

    hkvVec3 const impactDirection = (attackableEntity->GetPosition() - m_characterOwner->GetPosition()).getNormalized();
    attackableEntity->TakeDamage(m_outgoingDamage, impactDirection * impactSpeed);

    // apply knock back to characters
    RPG_InventoryItem const* equippedWeapon = m_characterOwner->GetInventoryHandler().GetEquippedWeapon();
    RPG_CharacterActionData const& actionData = m_characterOwner->GetCharacterActionData();
    float const knockBackStrength = equippedWeapon->GetKnockBackStrength() * actionData.GetAoeAttackKnockBackStrengthMultiplier();
    float const knockBackDuration = equippedWeapon->GetKnockBackDuration() * actionData.GetAoeAttackKnockBackDurationMultiplier();

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
