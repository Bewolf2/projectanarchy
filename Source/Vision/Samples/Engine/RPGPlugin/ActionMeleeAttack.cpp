/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionMeleeAttack.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionAttackBase.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

float const s_faceToTargetTime = 1.0f;     ///< time in seconds to rotate to the target while executing the attack.
float const s_1DivFaceToTargetTime = 1.0f / s_faceToTargetTime;

RPG_Action_MeleeAttack::RPG_Action_MeleeAttack(RPG_Character *owner)
  : RPG_Action_AttackBase(AT_MeleeAttack, owner)
  , m_attacking(false)
  , m_updatedInteractionEntity(NULL)
  , m_updatedInteractionPosition(0.0f, 0.0f, 0.0f)
  , m_updatedInteraction(false)
{
  m_behaviorTriggerVarName = "MeleeAttacking";
  m_behaviorFireEventName = "MELEE_ATTACK_FIRE";

  m_behaviorWeaponTrailStartEventName = "MELEE_WEAPON_ACTIVE_START";
  m_behaviorWeaponTrailEndEventName = "MELEE_WEAPON_ACTIVE_END";
}

/// Perform Cleanup of this action
void RPG_Action_MeleeAttack::Cleanup(RPG_ActionType_e nextAction)
{
  m_attacking = false;

  RPG_Action_AttackBase::Cleanup(nextAction);
}

void RPG_Action_MeleeAttack::Tick(float const deltaTime)
{
  RPG_Action_AttackBase::Tick(deltaTime);
  if(!m_interactionEntity)
  {
    End();
    return;
  }

  if(m_attacking)
  {
    // MeleeAttacking was set to true when death was initiated. Is reset to false by Behavior when the attack anim finishes.
    vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
    VVERIFY_OR_RET(behaviorComponent);

    VASSERT(!m_behaviorTriggerVarName.IsEmpty());
    if (!RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, m_behaviorTriggerVarName.AsChar()))
    {
      if (m_flags == 1)
      {
        if (m_updatedInteraction)
        {
          m_updatedInteraction = false;
          m_interactionEntity = m_updatedInteractionEntity;
          ValidateInteractionEntity();
          m_interactionPosition = m_updatedInteractionPosition;
        }

        if (m_interactionEntity && m_interactionEntity->Components().GetComponentOfType(V_RUNTIME_CLASS(RPG_AttackableComponent)))
        {
          // continuing attack
          IncrementMeleeAttackType();

          m_startTime = Vision::GetTimer()->GetTime();
          StartAttack();
        }
        else
        {
          // if we've gotten to the end and haven't fired the attack, fire it now
          if (!m_hasFired)
          {
            VString msg;
            msg.Format("RPG_Action_MeleeAttack never received expected %s event from Behavior.", m_behaviorFireEventName.AsChar());
            Vision::Error.Warning(msg.AsChar());
            //VASSERT_MSG(false, msg.AsChar());
            FireAttack();
          }
          End();
        }
      }
      else
      {
        // if we've gotten to the end and haven't fired the attack, fire it now
        if (!m_hasFired)
        {
          VString msg;
          msg.Format("RPG_Action_MeleeAttack never received expected %s event from Behavior.", m_behaviorFireEventName.AsChar());
          Vision::Error.Warning(msg.AsChar());
          //VASSERT_MSG(false, msg.AsChar());
          FireAttack();
        }
        End();
      }
    }
    else
    {
      // we're currently attacking
      if(m_interactionEntity)
      {
        FaceTargetPoint(m_interactionEntity->GetPosition());
      }
    }

    //DebugDisplayDamage();
  }
  else
  {
    StartAttack();
  }
}

void RPG_Action_MeleeAttack::IncrementMeleeAttackType()
{
  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  // how many attacks does this character have defined?
  int chainCount = RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<int>(*behaviorComponent, "MeleeChainCount");
  VASSERT_MSG(chainCount > 0, "This character's MeleeChainCount behavior variable must be > 0.");
  chainCount = hkvMath::Max(chainCount, 1);

  // increment the current melee attack type within the chainCount
  int meleeAttackType = RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<int>(*behaviorComponent, "MeleeAttackType");
  meleeAttackType = (meleeAttackType + 1) % chainCount;

  // write the value back to the Behavior project
  //Vision::Error.SystemMessage("MeleeAttackType: %i", meleeAttackType);
  behaviorComponent->SetWordVar("MeleeAttackType", meleeAttackType);
}

/// Perform the actual attack
void RPG_Action_MeleeAttack::StartAttack()
{
  RPG_Action_AttackBase::StartAttack();

  m_characterOwner->CreateCharacterEffect(FX_MeleeBasicAttackSwing);
  m_attacking = true;
}

void RPG_Action_MeleeAttack::FireAttack()
{
  RPG_Action_AttackBase::FireAttack();

  // create the attack impact effect
  m_characterOwner->CreateCharacterEffect(FX_MeleeBasicAttack);

  // try to deal damage
  m_outgoingDamage = CalculateOutgoingDamage();
  TryDealDamage();
}

void RPG_Action_MeleeAttack::UpdateInteraction(RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition)
{
  m_updatedInteractionEntity = interactionEntity;
  m_updatedInteractionPosition = interactionPosition;
  m_updatedInteraction = true;
}

int RPG_Action_MeleeAttack::CalculateOutgoingDamage() const
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
    const float damage = (damageMin + Vision::Game.GetFloatRand() * (damageMax - damageMin)) + 0.5f;
    return static_cast<int>(damage);
  }
  return 0;
}

void RPG_Action_MeleeAttack::TryDealDamage()
{
  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VASSERT(behaviorComponent);

  // @todo: there's a slim possibility of a 'bullet through paper' missed event if MeleeWeaponActive goes true and false in the same tick under a low framerate environment. May want to engineer a properly-acknowledged event.
  if (!behaviorComponent ||
    !RPG_VisionHavokBehaviorHelper::BehaviorHasVarNamed(*behaviorComponent, "MeleeWeaponActive") ||
    RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, "MeleeWeaponActive"))
  {
    if(m_interactionEntity &&
      m_interactionEntity->IsFrom(RPG_DamageableEntity))
    {
      RPG_DamageableEntity* target = static_cast<RPG_DamageableEntity*>(m_interactionEntity);

      hkvVec3 const impactDirection = (target->GetPosition() - m_characterOwner->GetPosition()).getNormalized();

      float impactSpeed = 0.0f;
      if(m_characterOwner)
      {
        RPG_InventoryItem const* equippedWeapon = m_characterOwner->GetInventoryHandler().GetEquippedWeapon();
        RPG_CharacterActionData const& actionData = m_characterOwner->GetCharacterActionData();
        if(equippedWeapon)
        {
          impactSpeed = equippedWeapon->GetImpactSpeed() * actionData.GetMeleeAttackImpactSpeedMultiplier();
        }
      }

      target->TakeDamage(m_outgoingDamage, impactDirection * impactSpeed);

      if (OwnerIsLocalPlayer())
      {
        // create a screen shake effect when local player hits an enemy
        ShakeCamera(50.f, 0.08f);
      }
    }
  }
}

void RPG_Action_MeleeAttack::DebugDisplayDamage() const
{
#ifdef _DEBUG
  const float vOffset = 400.f;
  VString msg, tempLine;

  tempLine.Format("Dealing Damage: %i\n", m_outgoingDamage);
  msg += tempLine;

  VASSERT(m_characterOwner);
  const hkvVec3 vPos = m_characterOwner->GetPosition() + hkvVec3(0.0f, 0.0f, vOffset);

  Vision::Message.SetTextColor(VColorRef(208,127,127,255));
  Vision::Message.DrawMessage3D(msg, vPos);
  Vision::Message.SetTextColor(); // restore default white
#endif
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
