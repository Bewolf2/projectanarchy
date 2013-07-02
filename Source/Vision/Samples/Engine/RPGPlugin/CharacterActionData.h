/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_CHARACTER_ACTION_DATA_H__
#define RPG_PLUGIN_CHARACTER_ACTION_DATA_H__

class RPG_CharacterActionData
{
public:
  RPG_CharacterActionData();

  //@{
  // Spawn
  float GetSpawnAnimationDelay() const;
  void SetSpawnAnimationDelay(float delay);
  //@}

  //@{
  // ActionHeal
  float GetHealProjectileSpeed() const;
  void SetHealProjectileSpeed(float const val);
  VString const& GetHealProjectileLaunchBone() const;
  void SetHealProjectileLaunchBone(VString const& val);
  //@}

  //@{
  // ActionRangedAttack
  float GetRangedAttackProjectileSpeed() const;
  void SetRangedAttackProjectileSpeed(float const val);
  VString const& GetRangedAttackProjectileLaunchBone() const;
  void SetRangedAttackProjectileLaunchBone(VString const& val);
  float GetRangedAttackImpactSpeedMultiplier() const;
  void SetRangedAttackImpactSpeedMultiplier(float const val);
  //@}

  //@{
  // ActionAoeAttack
  float GetAoeAttackDamageMultiplier() const;
  void SetAoeAttackDamageMultiplier(float const val);
  float GetAoeAttackRange() const;
  void SetAoeAttackRange(float const val);
  int GetAoeAttackManaCost() const;
  void SetAoeAttackManaCost(int const val);
  float GetAoeAttackImpactSpeedMultiplier() const;
  void SetAoeAttackImpactSpeedMultiplier(float const val);
  float GetAoeAttackKnockBackStrengthMultiplier() const;
  void SetAoeAttackKnockBackStrengthMultiplier(float const val);
  float GetAoeAttackKnockBackDurationMultiplier() const;
  void SetAoeAttackKnockBackDurationMultiplier(float const val);
  //@}

  //@{
  // ActionPowerAttack
  float GetPowerAttackDamageMultiplier() const;
  void SetPowerAttackDamageMultiplier(float const val);
  float GetPowerAttackRange() const;
  void SetPowerAttackRange(float const val);
  float GetPowerAttackAngle() const;
  void SetPowerAttackAngle(float const val);
  int GetPowerAttackManaCost() const;
  void SetPowerAttackManaCost(int const val);
  float GetPowerAttackImpactSpeedMultiplier() const;
  void SetPowerAttackImpactSpeedMultiplier(float const val);
  float GetPowerAttackKnockBackStrengthMultiplier() const;
  void SetPowerAttackKnockBackStrengthMultiplier(float const val);
  float GetPowerAttackKnockBackDurationMultiplier() const;
  void SetPowerAttackKnockBackDurationMultiplier(float const val);
  //@}


  //@{
  // ActionRangedAttack
  float GetRangedAttackKnockBackStrengthMultiplier() const;
  void SetRangedAttackKnockBackStrengthMultiplier(float const val);
  float GetRangedAttackKnockBackDurationMultiplier() const;
  void SetRangedAttackKnockBackDurationMultiplier(float const val);
  //@}

  //@{
  // ActionMeleeAttack
  float GetMeleeAttackImpactSpeedMultiplier() const;
  void SetMeleeAttackImpactSpeedMultiplier(float const val);
  float GetMeleeAttackKnockBackStrengthMultiplier() const;
  void SetMeleeAttackKnockBackStrengthMultiplier(float const val);
  float GetMeleeAttackKnockBackDurationMultiplier() const;
  void SetMeleeAttackKnockBackDurationMultiplier(float const val);
  //@}

protected:
  //@{
  // Spawn
  float m_spawnAnimationDelay;
  //@}

  //@{
  // ActionHeal
  float m_healProjectileSpeed;
  VString m_healProjectileLaunchBone;
  //@}

  //@{
  // ActionRangedAttack
  float m_rangedAttackProjectileSpeed;
  VString m_rangedAttackProjectileLaunchBone;
  float m_rangedAttackImpactSpeedMultiplier;
  //@}

  //@{
  // ActionAoeAttack
  float m_aoeAttackDamageMultiplier;    ///< Weapon damage is multiplied by this factor in an AoE attack.
  float m_aoeAttackRange;               ///< Enemies up to this range will be hit by an AoE attack.
  int m_aoeAttackManaCost;              ///< Mana spend required to execute this action
  float m_aoeAttackImpactSpeedMultiplier;
  float m_aoeAttackKnockBackStrengthMultiplier; ///< Multiplier applied to the weapon's knock back strength.
  float m_aoeAttackKnockBackDurationMultiplier; ///< Multiplier applied to the weapon's knock back duration.
  //@}

  //@{
  // ActionPowerAttack
  float m_powerAttackDamageMultiplier;  ///< Weapon damage is multiplied by this factor in a Power attack.
  float m_powerAttackRange;             ///< Targets in front of the attacker to this range will be hit.
  float m_powerAttackAngle;             ///< Targets within range and < this many degrees from attacker's facing will be hit.
  int m_powerAttackManaCost;            ///< Mana spend required to execute a Power atack.
  float m_powerAttackImpactSpeedMultiplier;
  float m_powerAttackKnockBackStrengthMultiplier; ///< Multiplier applied to the weapon's knock back strength.
  float m_powerAttackKnockBackDurationMultiplier; ///< Multiplier applied to the weapon's knock back duration.
  //@}

  //@{
  // ActionRangedAttack
  float m_rangedAttackKnockBackStrengthMultiplier; ///< Multiplier applied to the weapon's knock back strength.
  float m_rangedAttackKnockBackDurationMultiplier; ///< Multiplier applied to the weapon's knock back duration.
  //@}

  //@{
  // ActionMeleeAttack
  float m_meleeAttackImpactSpeedMultiplier;
  float m_meleeAttackKnockBackStrengthMultiplier; ///< Multiplier applied to the weapon's knock back strength.
  float m_meleeAttackKnockBackDurationMultiplier; ///< Multiplier applied to the weapon's knock back duration.
  //@}

  friend class RPG_Character;         ///< needed for the Character var table, unless there's a way to created nested var tables.
};

#endif // RPG_PLUGIN_CHARACTER_ACTION_DATA_H__

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
