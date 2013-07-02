/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/CharacterActionData.h>

RPG_CharacterActionData::RPG_CharacterActionData()
  : m_spawnAnimationDelay(0.f)

  , m_healProjectileSpeed(1000.f)
  , m_healProjectileLaunchBone("")

  , m_rangedAttackProjectileSpeed(1000.f)
  , m_rangedAttackProjectileLaunchBone("")
  , m_rangedAttackImpactSpeedMultiplier(1.f)

  , m_aoeAttackDamageMultiplier(1.f)
  , m_aoeAttackRange(0.f)
  , m_aoeAttackManaCost(0)
  , m_aoeAttackImpactSpeedMultiplier(1.5f)
  , m_aoeAttackKnockBackStrengthMultiplier(1.5f)
  , m_aoeAttackKnockBackDurationMultiplier(1.5f)

  , m_powerAttackDamageMultiplier(1.f)
  , m_powerAttackRange(0.f)
  , m_powerAttackAngle(0.f)
  , m_powerAttackManaCost(0)
  , m_powerAttackImpactSpeedMultiplier(2.f)
  , m_powerAttackKnockBackStrengthMultiplier(2.f)
  , m_powerAttackKnockBackDurationMultiplier(1.f)

  , m_rangedAttackKnockBackStrengthMultiplier(0.f)
  , m_rangedAttackKnockBackDurationMultiplier(0.f)

  , m_meleeAttackImpactSpeedMultiplier(1.f)
  , m_meleeAttackKnockBackStrengthMultiplier(1.f)
  , m_meleeAttackKnockBackDurationMultiplier(1.f)
{
}

float RPG_CharacterActionData::GetSpawnAnimationDelay() const
{
  return m_spawnAnimationDelay;
}

void RPG_CharacterActionData::SetSpawnAnimationDelay(float delay)
{
  m_spawnAnimationDelay = delay;
}

float RPG_CharacterActionData::GetHealProjectileSpeed() const
{
  return m_healProjectileSpeed;
}

void RPG_CharacterActionData::SetHealProjectileSpeed(float const val)
{
  m_healProjectileSpeed = val;
}

VString const& RPG_CharacterActionData::GetHealProjectileLaunchBone() const
{
  return m_healProjectileLaunchBone;
}

void RPG_CharacterActionData::SetHealProjectileLaunchBone(VString const& val)
{
  m_healProjectileLaunchBone = val;
}

float RPG_CharacterActionData::GetRangedAttackProjectileSpeed() const
{
  return m_rangedAttackProjectileSpeed;
}

void RPG_CharacterActionData::SetRangedAttackProjectileSpeed(float const val)
{
  m_rangedAttackProjectileSpeed = val;
}

VString const& RPG_CharacterActionData::GetRangedAttackProjectileLaunchBone() const
{
  return m_rangedAttackProjectileLaunchBone;
}

void RPG_CharacterActionData::SetRangedAttackProjectileLaunchBone(VString const& val)
{
  m_rangedAttackProjectileLaunchBone = val;
}

float RPG_CharacterActionData::GetRangedAttackImpactSpeedMultiplier() const
{
  return m_rangedAttackImpactSpeedMultiplier;
}

void RPG_CharacterActionData::SetRangedAttackImpactSpeedMultiplier( float const val )
{
  m_rangedAttackImpactSpeedMultiplier = val;
}

float RPG_CharacterActionData::GetAoeAttackDamageMultiplier() const
{
  return m_aoeAttackDamageMultiplier;
}

void RPG_CharacterActionData::SetAoeAttackDamageMultiplier(float const val)
{
  m_aoeAttackDamageMultiplier = val;
}

float RPG_CharacterActionData::GetAoeAttackRange() const
{
  return m_aoeAttackRange;
}

void RPG_CharacterActionData::SetAoeAttackRange(float const val)
{
  m_aoeAttackRange = val;
}

int RPG_CharacterActionData::GetAoeAttackManaCost() const
{
  return m_aoeAttackManaCost;
}

void RPG_CharacterActionData::SetAoeAttackManaCost(int const val)
{
  m_aoeAttackManaCost = val;
}

float RPG_CharacterActionData::GetAoeAttackImpactSpeedMultiplier() const
{
  return m_aoeAttackImpactSpeedMultiplier;
}

void RPG_CharacterActionData::SetAoeAttackImpactSpeedMultiplier( float const val )
{
  m_aoeAttackImpactSpeedMultiplier = val;
}

float RPG_CharacterActionData::GetAoeAttackKnockBackStrengthMultiplier() const
{
  return m_aoeAttackKnockBackStrengthMultiplier;
}

void RPG_CharacterActionData::SetAoeAttackKnockBackStrengthMultiplier( float const val )
{
  m_aoeAttackKnockBackStrengthMultiplier = val;
}

float RPG_CharacterActionData::GetAoeAttackKnockBackDurationMultiplier() const
{
  return m_aoeAttackKnockBackDurationMultiplier;
}

void RPG_CharacterActionData::SetAoeAttackKnockBackDurationMultiplier( float const val )
{
  m_aoeAttackKnockBackDurationMultiplier = val;
}

float RPG_CharacterActionData::GetPowerAttackDamageMultiplier() const
{
  return m_powerAttackDamageMultiplier;
}

void RPG_CharacterActionData::SetPowerAttackDamageMultiplier(float const val)
{
  m_powerAttackDamageMultiplier = val;
}

int RPG_CharacterActionData::GetPowerAttackManaCost() const
{
  return m_powerAttackManaCost;
}

void RPG_CharacterActionData::SetPowerAttackManaCost(int const val)
{
  m_powerAttackManaCost = val;
}

float RPG_CharacterActionData::GetPowerAttackRange() const
{
  return m_powerAttackRange;
}

void RPG_CharacterActionData::SetPowerAttackRange(float const val)
{
  m_powerAttackRange = val;
}

float RPG_CharacterActionData::GetPowerAttackAngle() const
{
  return m_powerAttackAngle;
}

void RPG_CharacterActionData::SetPowerAttackAngle(float const val)
{
  m_powerAttackAngle = val;
}

float RPG_CharacterActionData::GetPowerAttackImpactSpeedMultiplier() const
{
  return m_powerAttackImpactSpeedMultiplier;
}

void RPG_CharacterActionData::SetPowerAttackImpactSpeedMultiplier( float const val )
{
  m_powerAttackImpactSpeedMultiplier = val;
}

float RPG_CharacterActionData::GetMeleeAttackImpactSpeedMultiplier() const
{
  return m_meleeAttackImpactSpeedMultiplier;
}

void RPG_CharacterActionData::SetMeleeAttackImpactSpeedMultiplier( float const val )
{
  m_meleeAttackImpactSpeedMultiplier = val;
}
float RPG_CharacterActionData::GetPowerAttackKnockBackStrengthMultiplier() const
{
  return m_powerAttackKnockBackStrengthMultiplier;
}

void RPG_CharacterActionData::SetPowerAttackKnockBackStrengthMultiplier( float const val )
{
  m_powerAttackKnockBackStrengthMultiplier = val;
}

float RPG_CharacterActionData::GetPowerAttackKnockBackDurationMultiplier() const
{
  return m_powerAttackKnockBackDurationMultiplier;
}

void RPG_CharacterActionData::SetPowerAttackKnockBackDurationMultiplier( float const val )
{
  m_powerAttackKnockBackDurationMultiplier = val;
}

float RPG_CharacterActionData::GetRangedAttackKnockBackStrengthMultiplier() const
{
  return m_rangedAttackKnockBackStrengthMultiplier;
}

void RPG_CharacterActionData::SetRangedAttackKnockBackStrengthMultiplier( float const val )
{
  m_rangedAttackKnockBackStrengthMultiplier = val;
}

float RPG_CharacterActionData::GetRangedAttackKnockBackDurationMultiplier() const
{
  return m_rangedAttackKnockBackDurationMultiplier;
}

void RPG_CharacterActionData::SetRangedAttackKnockBackDurationMultiplier( float const val )
{
  m_rangedAttackKnockBackDurationMultiplier = val;
}

float RPG_CharacterActionData::GetMeleeAttackKnockBackStrengthMultiplier() const
{
  return m_meleeAttackKnockBackStrengthMultiplier;
}

void RPG_CharacterActionData::SetMeleeAttackKnockBackStrengthMultiplier( float const val )
{
  m_meleeAttackKnockBackStrengthMultiplier = val;
}

float RPG_CharacterActionData::GetMeleeAttackKnockBackDurationMultiplier() const
{
  return m_meleeAttackKnockBackDurationMultiplier;
}

void RPG_CharacterActionData::SetMeleeAttackKnockBackDurationMultiplier( float const val )
{
  m_meleeAttackKnockBackDurationMultiplier = val;
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
