/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_ACTION_ATTACK_BASE_H__
#define RPG_PLUGIN_ACTION_ATTACK_BASE_H__

#include <Vision/Samples/Engine/RPGPlugin/Action.h>

class RPG_DamageableEntity;

class RPG_Action_AttackBase : public RPG_Action
{
public:
  RPG_Action_AttackBase(RPG_ActionType_e identifier, RPG_Character *owner);

  virtual void Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition) HKV_OVERRIDE;
  virtual void Cleanup(RPG_ActionType_e nextAction) HKV_OVERRIDE; /// occurs after an action has been told to shut down. Perform any required character state restoration here.
  virtual void Tick(float const deltaTime) HKV_OVERRIDE;
  virtual void OnHavokBehaviorEvent(const VString& eventName) HKV_OVERRIDE;

protected:
  virtual void StartAttack();
  virtual void FireAttack();
  virtual void PopulateTargetList() {};

  void ShakeCamera(float const amplitude, float const duration);

  VString m_behaviorTriggerVarName;
  VString m_behaviorFireEventName;
  VString m_behaviorWeaponTrailStartEventName;
  VString m_behaviorWeaponTrailEndEventName;

  VArray<RPG_DamageableEntity*> m_targets;
  int m_outgoingDamage;
  bool m_hasFired;
};

#endif  // RPG_PLUGIN_ACTION_ATTACK_BASE_H__

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
