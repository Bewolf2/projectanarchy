/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_AI_CONTROLLER_COMPONENT_H__
#define RPG_PLUGIN_AI_CONTROLLER_COMPONENT_H__

#include <Vision/Samples/Engine/RPGPlugin/ControllerComponent.h>

class RPG_DamageableEntity;

enum RPG_AIState_e
{
  ST_None=-1,
  ST_Spawning=0,
  ST_Wandering,
  ST_Challenging,
  ST_MovingToPosition,
  ST_MeleeAttacking,
  ST_RangedAttacking,
  ST_AoeAttacking,
  ST_Fleeing,
  ST_Healing,
  ST_Count
};

// AI controller component
class RPG_AiControllerComponent : public RPG_ControllerComponent
{
public:
  // Constructor needs to be public for FORCE_LINKDYNCLASS on mobile
  RPG_AiControllerComponent();

protected:
  //@{
  // Common utility methods
  void SetTarget(RPG_DamageableEntity* target);
  RPG_DamageableEntity* GetTarget();
  void ClearTarget();
  bool HasTarget() const;
  bool HasValidTarget() const;
  bool HasLineOfSightToTarget() const;
  bool HasLineOfSightToTarget(bool& left, bool& center, bool& right) const;
  //@}

  //@{
  // State Handling
  virtual void SelectState() {}   // override in child classes
  virtual void UpdateState(float const deltaTime);
  RPG_AIState_e GetState() const;
  bool IsInState(RPG_AIState_e state) const;
  bool SwitchState(RPG_AIState_e newState);
  virtual bool CanEnterState(RPG_AIState_e newState) {return true;}
  //@}

  //@{
  // Commonly-Used States
  virtual void UpdateSpawning(float const deltaTime);
  virtual void UpdateWandering(float const deltaTime) {};
  virtual void UpdateChallenging(float const deltaTime);
  virtual void UpdateMovingToPosition(float const deltaTime) {};
  virtual void UpdateMeleeAttacking(float const deltaTime) {};
  virtual void UpdateRangedAttacking(float const deltaTime) {};
  virtual void UpdateAoeAttacking(float const deltaTime) {};
  virtual void UpdateFleeing(float const deltaTime) {};
  virtual void UpdateHealing(float const deltaTime) {};
  //@}

  //@{
  // Common utility methods
  bool AcquireTarget();
  bool IsValidTargetInAggroRange() const;
  bool FleeFromPosition(hkvVec3 const& targetPosition, float const fleeDistance = 300.f);
  //@}

  void DebugDisplayStateInformation() const;

  // RPG_AiControllerComponent
  void ServerTick(float deltaTime) HKV_OVERRIDE;

protected:
  RPG_AIState_e m_aiState;
  bool m_isFirstStateUpdate;
  float m_stateStartTime;
  RPG_DamageableEntity *m_target;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_AiControllerComponent, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_AiControllerComponent, RPG_PLUGIN_IMPEXP);
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
