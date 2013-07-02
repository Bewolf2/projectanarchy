/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionHeal.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/ProjectileHealing.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

float const s_faceToTargetTime = 1.0f;     ///< time in seconds to rotate to the target while executing the attack.
float const s_1DivFaceToTargetTime = 1.0f / s_faceToTargetTime;

RPG_Action_Heal::RPG_Action_Heal(RPG_Character *owner)
  : RPG_Action(AT_Heal, owner)
  , m_behaviorTriggerVarName("Healing")
  , m_behaviorFireEventName("HEAL_FIRE")
  , m_outgoingHealing(0)
  , m_healing(false)
  , m_hasHealed(false)
{
}

void RPG_Action_Heal::Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition)
{
  RPG_Action::Start(forced, prevAction, interactionEntity, interactionPosition);
  StartHeal();
}

/// Perform Cleanup of this action
void RPG_Action_Heal::Cleanup(RPG_ActionType_e nextAction)
{
  m_healing = false;
  m_hasHealed = false;

  // kill the charge loop effect if it's still running
  if (m_characterOwner->GetPersistentCharacterEffect(FX_CastHealChargeLoop))
  {
    m_characterOwner->PauseCharacterEffect(FX_CastHealChargeLoop);
  }

  RPG_Action::Cleanup(nextAction);
}

void RPG_Action_Heal::Tick(float const deltaTime)
{
  RPG_Action::Tick(deltaTime);
  if(!m_interactionEntity)
  {
    End();
    return;
  }

  if(m_healing)
  {
    // Healing was set to true when the action was initiated. Is reset to false by Behavior when the attack anim finishes.
    vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
    VVERIFY_OR_RET(behaviorComponent);

    VASSERT(!m_behaviorTriggerVarName.IsEmpty());
    if (!RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, m_behaviorTriggerVarName.AsChar()))
    {
      End();
      return;
    }
    else
    {
      // we're currently healing
      if(m_interactionEntity)
      {
        FaceTargetPoint(m_interactionEntity->GetPosition());
      }
    }

    //DisplayHealing();
  }
  else
  {
    StartHeal();
  }
}

int RPG_Action_Heal::CalculateOutgoingHealing() const
{
  return m_characterOwner->GetCharacterStats().GetHealingAmount();
}

//void RPG_Action_Heal::DisplayHealing() const
//{
//  const float vOffset = 400.f;
//  VString msg, tempLine;
//
//  tempLine.Format("Healing: %i\n", m_outgoingHealing);
//  msg += tempLine;
//
//  VASSERT(m_characterOwner);
//  const hkvVec3 vPos = m_characterOwner->GetPosition() + hkvVec3(0.0f, 0.0f, vOffset);
//
//  Vision::Message.SetTextColor(VColorRef(208,127,127,255));
//  Vision::Message.DrawMessage3D(msg, vPos);
//  Vision::Message.SetTextColor(); // restore default white
//}

/// Start the heal action
void RPG_Action_Heal::StartHeal()
{
  m_healing = true;
  m_hasHealed = false;

  StartHealingChargeEffects();

  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  VASSERT(!m_behaviorTriggerVarName.IsEmpty());
  behaviorComponent->SetBoolVar(m_behaviorTriggerVarName, true); // tell the behavior component to begin the Heal animation. Healing will be reset to 0.0 when the attack anim finishes.
}

void RPG_Action_Heal::TryHeal()
{
  m_outgoingHealing = CalculateOutgoingHealing();
  if(m_interactionEntity &&
    m_interactionEntity->IsFrom(RPG_Character))
  {
    m_characterOwner->PauseCharacterEffect(FX_CastHealChargeLoop);

    if (m_interactionEntity == m_characterOwner)
    {
      // self-heal applies healing directly
      m_characterOwner->CreateCharacterEffect(FX_CastHealSelf);
      RPG_Character* target = static_cast<RPG_Character*>(m_interactionEntity);
      target->AddHealth(m_outgoingHealing);
    }
    else
    {
      // healing another character launches a projectile
      m_characterOwner->CreateCharacterEffect(FX_CastHealOther);
      FireHealingProjectile();
    }
    m_hasHealed = true;
  }
}

void RPG_Action_Heal::OnHavokBehaviorEvent(const VString& eventName)
{
  Vision::Error.SystemMessage(eventName.AsChar());
  if (eventName == m_behaviorFireEventName)
  {
    TryHeal();
  }
}

void RPG_Action_Heal::StartHealingChargeEffects()
{
  VASSERT(m_characterOwner);
  m_characterOwner->CreateCharacterEffect(FX_CastHealChargeLoop);
}

void RPG_Action_Heal::FireHealingProjectile()
{
  VASSERT(m_characterOwner);
  VASSERT(m_interactionEntity);
  VASSERT_MSG(m_interactionEntity != m_characterOwner, "Healing projectile shouldn't be spawned for self-heals.");

  RPG_Projectile_Healing* projectile = static_cast<RPG_Projectile_Healing*>(RPG_GameManager::s_instance.CreateEntity("RPG_Projectile_Healing", m_characterOwner->GetPosition()));
  projectile->SetCharacterOwner(m_characterOwner);
  projectile->SetProjectileEffect(PFX_Inflight ,m_characterOwner->GetCharacterEffectDefinition(FX_HealProjectile));
  projectile->SetProjectileEffect(PFX_Impact ,m_characterOwner->GetCharacterEffectDefinition(FX_HealProjectileImpact));

  hkvVec3 firePosition = m_characterOwner->GetPosition();
  VString const& boneName = m_characterOwner->GetCharacterActionData().GetHealProjectileLaunchBone();

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

  float const speed = m_characterOwner->GetCharacterActionData().GetHealProjectileSpeed();
  projectile->Fire(firePosition, m_interactionEntity, true, speed);
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
