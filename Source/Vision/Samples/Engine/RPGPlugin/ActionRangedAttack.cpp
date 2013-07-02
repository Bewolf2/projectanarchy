/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionRangedAttack.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionAttackBase.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/Projectile.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

float const s_faceToTargetTime = 1.0f;     ///< time in seconds to rotate to the target while executing the attack.
float const s_1DivFaceToTargetTime = 1.0f / s_faceToTargetTime;
float const s_retargetingRadius = 100.0f; ///< distance within which we assume that a missed click onto scene geometry is an attack on an entity

RPG_Action_RangedAttack::RPG_Action_RangedAttack(RPG_Character* const owner)
  : RPG_Action_AttackBase(AT_RangedAttack, owner)
  , m_attacking(false)
  , m_updatedInteractionEntity(NULL)
  , m_updatedInteractionPosition(0.0f, 0.0f, 0.0f)
  , m_updatedInteraction(false)
{
  m_behaviorTriggerVarName = "RangedAttacking";
  m_behaviorFireEventName = "RANGED_ATTACK_FIRE";
  m_behaviorChargeStartEventName = "RANGED_ATTACK_CHARGE_START";

  m_behaviorWeaponTrailStartEventName = "RANGED_ATTACK_CHARGE_START";
  m_behaviorWeaponTrailEndEventName = "RANGED_ATTACK_FIRE";
}

/// Perform Cleanup of this action
void RPG_Action_RangedAttack::Cleanup(RPG_ActionType_e nextAction)
{
  m_attacking = false;

  // kill the charge loop effect if it's still running
  if (m_characterOwner->GetPersistentCharacterEffect(FX_RangedAttackChargeLoop))
  {
    m_characterOwner->PauseCharacterEffect(FX_RangedAttackChargeLoop);
  }

  RPG_Action_AttackBase::Cleanup(nextAction);
}

void RPG_Action_RangedAttack::Tick(float const deltaTime)
{
  RPG_Action_AttackBase::Tick(deltaTime);

  if(m_attacking)
  {
    // RangedAttacking was set to true when the action was initiated. Is reset to false by Behavior when the attack anim finishes.
    vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
    VVERIFY_OR_RET(behaviorComponent);

    VASSERT(!m_behaviorTriggerVarName.IsEmpty());
    if (!RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, m_behaviorTriggerVarName.AsChar()))
    {
      if (m_flags == 1)
      {
        if(m_updatedInteraction)
        {
          m_updatedInteraction = false;
          if(!m_updatedInteractionEntity && m_interactionEntity)
          {
            // Do not lose the player's intended target
            ValidateInteractionEntity();
            if(!m_interactionEntity->Components().GetComponentOfType(V_RUNTIME_CLASS(RPG_AttackableComponent)))
            {
              m_interactionEntity = NULL;
            }
            else
            {
              if(m_interactionEntity->GetPosition().getDistanceToSquared(m_updatedInteractionPosition) > s_retargetingRadius * s_retargetingRadius)
              {
                m_interactionEntity = NULL;
              }
            }
          }
          else
          {
            m_interactionEntity = m_updatedInteractionEntity;
            ValidateInteractionEntity();
          }
          m_interactionPosition = m_updatedInteractionPosition;
        }
        m_startTime = Vision::GetTimer()->GetTime();
        StartAttack();
      }
      else
      {
        // if we've gotten to the end and haven't fired the projectile, fire it now
        if (!m_hasFired)
        {
          VString msg;
          msg.Format("RPG_Action_RangedAttack never received expected %s event from Behavior.", m_behaviorFireEventName.AsChar());
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
      hkvVec3 const targetPosition = m_interactionEntity ? m_interactionEntity->GetPosition() : m_interactionPosition;
      FaceTargetPoint(targetPosition);

#ifdef _DEBUG
      if (m_debugDisplayInfo)
      {
        Vision::Game.DrawSingleLine(m_characterOwner->GetPosition(), targetPosition, VColorRef(255, 0, 255));
      }
#endif
    }
  }
  else
  {
    StartAttack();
  }
}

int RPG_Action_RangedAttack::CalculateOutgoingDamage() const
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

void RPG_Action_RangedAttack::DisplayDamage() const
{
  const float vOffset = 400.f;
  VString msg, tempLine;

  tempLine.Format("Dealing Damage: %i\n", m_outgoingDamage);
  msg += tempLine;

  VASSERT(m_characterOwner);
  const hkvVec3 vPos = m_characterOwner->GetPosition() + hkvVec3(0.0f, 0.0f, vOffset);

  Vision::Message.SetTextColor(VColorRef(208,127,127,255));
  Vision::Message.DrawMessage3D(msg, vPos);
  Vision::Message.SetTextColor(); // restore default white
}

/// Perform the actual attack
void RPG_Action_RangedAttack::StartAttack()
{
  RPG_Action_AttackBase::StartAttack();

  m_attacking = true;
  m_outgoingDamage = CalculateOutgoingDamage();
}

void RPG_Action_RangedAttack::UpdateInteraction(RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition)
{
  m_updatedInteractionEntity = interactionEntity;
  m_updatedInteractionPosition = interactionPosition;
  if(!m_interactionEntity && m_updatedInteractionEntity &&
    m_updatedInteractionEntity->GetPosition().getDistanceToSquared(m_interactionPosition) < s_retargetingRadius * s_retargetingRadius)
  {
    m_interactionEntity = m_updatedInteractionEntity;
    ValidateInteractionEntity();
  }
  m_updatedInteraction = true;
}

void RPG_Action_RangedAttack::FireAttack()
{
  RPG_Action_AttackBase::FireAttack();

  RPG_Projectile* projectile = static_cast<RPG_Projectile*>(RPG_GameManager::s_instance.CreateEntity("RPG_Projectile", m_characterOwner->GetPosition()));
  projectile->SetCharacterOwner(m_characterOwner);
  projectile->SetProjectileMesh(m_characterOwner->GetProjectileMeshFilename());
  projectile->SetProjectileEffect(PFX_Inflight ,m_characterOwner->GetCharacterEffectDefinition(FX_RangedAttackProjectile));
  projectile->SetProjectileEffect(PFX_Impact ,m_characterOwner->GetCharacterEffectDefinition(FX_RangedAttackImpact));

  hkvVec3 firePosition = m_characterOwner->GetPosition();
  VString const& boneName = m_characterOwner->GetCharacterActionData().GetRangedAttackProjectileLaunchBone();

  bool foundBone = false;

  int boneIndex = -1;

  if(!boneName.IsEmpty())
  {
    boneIndex = m_characterOwner->GetMesh()->GetSkeleton()->GetBoneIndexByName(boneName.AsChar());

    if(boneIndex == -1)
    {
      Vision::Error.Warning("RPG_Action_RangedAttack::FireAttack - Supplied bone name doesn't exist on this skeleton: %s", boneName.AsChar());
      //VASSERT_MSG(boneIndex != -1, "Supplied bone name doesn't exist on this skeleton.");
    }
  }

  if(boneIndex != -1)
  {
    // find the projectile launch bone's position in worldspace
    hkvVec3 boneWorldSpaceTranslation;
    hkvQuat boneWorldSpaceRotation;
    VVERIFY(m_characterOwner->GetBoneCurrentWorldSpaceTransformation(boneIndex, boneWorldSpaceTranslation, boneWorldSpaceRotation));
    firePosition = boneWorldSpaceTranslation;
  }
  else
  {
    // no bone identified - just raise the projectile up a bit
    firePosition.z += 100.0f;
  }
  
  float const speed = m_characterOwner->GetCharacterActionData().GetRangedAttackProjectileSpeed();

  if(m_interactionEntity)
  {
    projectile->Fire(firePosition, m_interactionEntity, false, speed);
  }
  else
  {
    hkvVec3 targetPosition = m_interactionPosition;
    targetPosition.z = firePosition.z;
    
    hkvVec3 fireDirection = targetPosition - firePosition; 
    if(fireDirection.normalizeIfNotZero() == HKV_SUCCESS)
    {
      projectile->Fire(firePosition, fireDirection, speed);
    }
  }

  // trigger effects
  m_characterOwner->PauseCharacterEffect(FX_RangedAttackChargeLoop);
  m_characterOwner->CreateCharacterEffect(FX_RangedAttackFire);
}

void RPG_Action_RangedAttack::OnHavokBehaviorEvent(const VString& eventName)
{
  if (eventName == m_behaviorChargeStartEventName)
  {
    m_characterOwner->CreateCharacterEffect(FX_RangedAttackChargeLoop);
  }
  else
  {
    RPG_Action_AttackBase::OnHavokBehaviorEvent(eventName);
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
