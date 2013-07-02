/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionAttackBase.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/ControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

RPG_Action_AttackBase::RPG_Action_AttackBase(RPG_ActionType_e identifier, RPG_Character *owner)
  : RPG_Action(identifier, owner)
  , m_behaviorTriggerVarName("")
  , m_behaviorFireEventName("")
  , m_behaviorWeaponTrailStartEventName("")
  , m_behaviorWeaponTrailEndEventName("")
  , m_targets()
  , m_outgoingDamage(0)
  , m_hasFired(false)
{

}

void RPG_Action_AttackBase::Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition)
{
  RPG_Action::Start(forced, prevAction, interactionEntity, interactionPosition);

  // cancel any ongoing move
  VASSERT(m_characterOwner);
  m_characterOwner->GetController()->CancelRequestedPath();

  StartAttack();
}

void RPG_Action_AttackBase::Cleanup(RPG_ActionType_e nextAction)
{
  m_characterOwner->SetWeaponTrailEnabledForEquippedWeapon(false);  // ensure that early termination still shuts off weapon trails
  RPG_Action::Cleanup(nextAction);
}

void RPG_Action_AttackBase::Tick(float const deltaTime)
{
  RPG_Action::Tick(deltaTime);
}

void RPG_Action_AttackBase::OnHavokBehaviorEvent(const VString& eventName)
{
  //Vision::Error.SystemMessage(eventName.AsChar());
  if (eventName == m_behaviorFireEventName)
  {
    FireAttack();
  }

  // handle weapon trails
  if (eventName == m_behaviorWeaponTrailStartEventName)
  {
    m_characterOwner->SetWeaponTrailEnabledForEquippedWeapon(true);
  }
  else if (eventName == m_behaviorWeaponTrailEndEventName)
  {
    m_characterOwner->SetWeaponTrailEnabledForEquippedWeapon(false);
  }
}

void RPG_Action_AttackBase::StartAttack()
{
  // trigger the animation
  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  VASSERT(!m_behaviorTriggerVarName.IsEmpty());
  behaviorComponent->SetBoolVar(m_behaviorTriggerVarName.AsChar(), true);  // tell the behavior component to execute the attack event. MeleePowerAttacking resets to false when the attack anim is complete.

  m_hasFired = false;
}

void RPG_Action_AttackBase::FireAttack()
{
  m_hasFired = true;
}

void RPG_Action_AttackBase::ShakeCamera(float const amplitude, float const duration)
{
  RPG_EffectDefinition effectDef;
  effectDef.m_cameraShakeAmplitude = amplitude;
  effectDef.m_cameraShakeDuration = duration;
  RPG_GameManager::s_instance.CreateEffect(effectDef, m_interactionEntity->GetPosition());
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
