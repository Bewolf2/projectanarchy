/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_CASTER_GUARDIAN_AI_CONTROLLER_COMPONENT_H__
#define RPG_PLUGIN_CASTER_GUARDIAN_AI_CONTROLLER_COMPONENT_H__

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>

// Caster Guardian AI controller component
class RPG_CasterGuardianAiControllerComponent : public RPG_AiControllerComponent
{
public:
  // Constructor needs to be public for FORCE_LINKDYNCLASS on mobile
  RPG_CasterGuardianAiControllerComponent();

protected:
  //@{
  // State Machine Management
  void SelectState() HKV_OVERRIDE;
  //@}

  //@{
  // Individual State Updates
  void UpdateWandering(float const deltaTime) HKV_OVERRIDE;
  void UpdateMovingToPosition(float const deltaTime) HKV_OVERRIDE;
  void UpdateRangedAttacking(float const deltaTime) HKV_OVERRIDE;
  void UpdateAoeAttacking(float const deltaTime) HKV_OVERRIDE;
  void UpdateFleeing(float const deltaTime) HKV_OVERRIDE;
  void UpdateHealing(float const deltaTime) HKV_OVERRIDE;
  //@}

  //@{
  // Conditions
  bool HasHealingTarget() const;
  bool WantsToHeal(const RPG_Character* healingTarget) const;
  bool IsReadyToHeal() const;
  bool IsTargetTooClose() const;
  bool IsTargetTooFar() const;
  bool IsReadyToFlee() const;
  bool IsReadyToAoeAttack() const;
  bool ShouldAoeAttack() const;
  //@}

  //@{
  // Commands
  bool AcquireHealingTarget();
  void ClearHealingTarget();
  //@}

  //@{
  // Attacking state
  float m_lastAttackTime;     ///< When did this character last attack?
  float m_attackInterval;     ///< Randomly assigned delay between attacks
  float m_attackIntervalMin;
  float m_attackIntervalMax;
  float m_attackMaxRange;
  //@}

  //@{
  // AoE Attacking state
  float m_aoeMinInterval;
  float m_lastAoeTime;
  //@}

  //@{
  // Wander state
  hkvVec3 m_wanderOrigin;
  float m_wanderAngle;
  float m_wanderIdleTime;
  //@}

  //@{
  // Healing state
  RPG_Character* m_healingTarget;
  float m_healMinInterval;
  float m_lastHealTime;
  //@}

  //@{
  // Fleeing state
  float m_holdOffRange;
  float m_fleeMaxDuration;
  float m_fleeMinInterval;
  float m_lastFleeTime;
  //@}

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_CasterGuardianAiControllerComponent, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_CasterGuardianAiControllerComponent, RPG_PLUGIN_IMPEXP);
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
