/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_RANGED_GUARDIAN_AI_CONTROLLER_COMPONENT_H__
#define RPG_PLUGIN_RANGED_GUARDIAN_AI_CONTROLLER_COMPONENT_H__

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>

// Ranged Guardian AI controller component
class RPG_RangedGuardianAiControllerComponent : public RPG_AiControllerComponent
{
public:
  // Constructor needs to be public for FORCE_LINKDYNCLASS on mobile
  RPG_RangedGuardianAiControllerComponent();

protected:
  //@{
  // States
  void SelectState() HKV_OVERRIDE;
  //@}

  //@{
  // Individual State Updates
  void UpdateWandering(float const deltaTime) HKV_OVERRIDE;
  void UpdateMovingToPosition(float const deltaTime) HKV_OVERRIDE;
  void UpdateRangedAttacking(float const deltaTime) HKV_OVERRIDE;
  void UpdateFleeing(float const deltaTime) HKV_OVERRIDE;
  //@}

  //@{
  // Conditions
  bool IsTargetTooClose() const;
  bool IsTargetTooFar() const;
  bool IsReadyToFlee() const;
  //@}

  //@{
  // Attacking state
  float m_lastAttackTime;     ///< When did this character last attack?
  float m_attackInterval;     ///< Randomly assigned delay between attacks
  float m_attackIntervalMin;
  float m_attackIntervalMax;
  float m_attackMaxRange;
  hkvVec3 m_attackLineOfSightPosition;
  bool m_attackMovingToLineOfSight;
  float m_attackMovingToLineOfSightStartTime;
  //@}

  //@{
  // Wander state
  hkvVec3 m_wanderOrigin;
  float m_wanderAngle;
  float m_wanderIdleTime;
  //@}

  //@{
  // Fleeing state
  float m_holdOffRange;
  float m_fleeMaxDuration;
  float m_fleeMinInterval;
  float m_lastFleeTime;
  //@}

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_RangedGuardianAiControllerComponent, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_RangedGuardianAiControllerComponent, RPG_PLUGIN_IMPEXP);
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
