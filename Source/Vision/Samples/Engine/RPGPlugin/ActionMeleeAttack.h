/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_ACTION_MELEE_ATTACK_H__
#define RPG_PLUGIN_ACTION_MELEE_ATTACK_H__

#include <Vision/Samples/Engine/RPGPlugin/Action.h>
#include <Vision/Samples/Engine/RPGPlugin/ActionAttackBase.h>

class RPG_Action_MeleeAttack : public RPG_Action_AttackBase
{
public:
  RPG_Action_MeleeAttack(RPG_Character *owner);

  virtual void Cleanup(RPG_ActionType_e nextAction) HKV_OVERRIDE; /// occurs after an action has been told to shut down. Perform any required character state restoration here.
  virtual void Tick(float const deltaTime) HKV_OVERRIDE;
  virtual void UpdateInteraction(RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition) HKV_OVERRIDE;

protected:
  virtual void StartAttack() HKV_OVERRIDE;
  virtual void FireAttack() HKV_OVERRIDE;

  void IncrementMeleeAttackType();
  int CalculateOutgoingDamage() const;
  void DebugDisplayDamage() const;
  void TryDealDamage();


  bool m_attacking;       ///< whether or not we're actually attacking.
  RPG_BaseEntity* m_updatedInteractionEntity;
  hkvVec3 m_updatedInteractionPosition;
  bool m_updatedInteraction;
};

#endif

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
