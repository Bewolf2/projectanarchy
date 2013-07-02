/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_CHARACTER_STATS_H__
#define RPG_PLUGIN_CHARACTER_STATS_H__

#include <Vision/Runtime/Base/Container/VRingBuffer.hpp>

enum RPG_CharacterEventType_e
{
  CE_None=-1,
  CE_Damage=0,
  CE_Dodge,
  CE_Block,
  CE_Heal,
  CE_FailedManaSpend,
  CE_Count
};

/// Character event description for messaging and logging
struct RPG_CharacterEvent
{
  RPG_CharacterEvent();
  RPG_CharacterEvent(RPG_CharacterEventType_e const& eventType, float const eventTime, int const eventValue = 0, VString const eventMessage = "");
  void Clear();

  RPG_CharacterEventType_e m_eventType;   ///< The type of event being logged
  float m_eventTime;                      ///< When did this event occur?
  int m_eventValue;                       ///< Integer value associated with this event
};

class RPG_CharacterStats
{
public:
  RPG_CharacterStats();

  void Tick(float const deltaTime);

  //@{
  // Base Stats
  void  SetStrength(int const strength);
  int   GetStrength() const;
  void  SetDexterity(int const dexterity);
  int   GetDexterity() const;
  void  SetSpirit(int const spirit);
  int   GetSpirit() const;
  void  SetVitality(int const vitality);
  int   GetVitality() const;
  //@}

  //@{
  // Character Base Values
  int   GetBaseDamage() const;
  int   GetBaseAttackSpeed() const;
  int   GetBaseHealth() const;
  int   GetBaseMana() const;
  //@}

  //@{
  // Health
  void  SetHealth(int const health);
  int   GetHealth() const;
  float GetHealthPercentage() const;
  void  SetHealthMax(int const healthMax);
  int   GetHealthMax() const;
  int   CalculateHealthMax();
  void  SetHealthRegenerationRate(float const regenerationRate);
  void  SetHealthRegenerationEnabled(bool const enabled);
  void  SetUnlimitedHealth(bool unlimitedHealth);
  bool  HasUnlimitedHealth() const;
  //@}

  //@{
  // Mana
  void  SetMana(int const spirit);
  int   GetMana() const;
  void  SetManaMax(int const spiritMax);
  int   GetManaMax() const;
  void  SetManaRegenerationRate(float const regenerationRate);
  void  SetManaRegenerationEnabled(bool const enabled);
  void  SetUnlimitedMana(bool unlimitedMana);
  bool  HasUnlimitedMana() const;

  int   CalculateManaMax();
  //@}

  //@{
  // Experience
  void  SetExperience(int const experience);
  int   GetExperience() const;
  void  SetExperienceLevelThreshold(int const experienceThreshold);
  int   GetExperienceLevelThreshold() const;
  //@}

  //@{
  // Stat-Derived Modifiers
  float GetWeaponDamageModifier() const;
  float GetCritDamageModifier() const;
  float GetCritChance() const;
  float GetDodgeChance() const;
  float GetSpiritDamageModifier() const;
  float GetArmorBonus() const;
  float GetBlockChance() const;
  //@}

  void UpdateHealthRegeneration(float const deltaTime);
  void UpdateManaRegeneration(float const deltaTime);

  //@{
  // special abilities
  int GetHealingAmount() const;
  //@}

  //@{
  // Event Logging
  void LogCharacterEvent(RPG_CharacterEventType_e const& eventType, float const eventTime, int const eventValue = 0, VString const eventMessage = "");
  const VSimpleRingBuffer<RPG_CharacterEvent>& GetCharacterEventLog() const;
  bool GetLastCharacterEvent(RPG_CharacterEvent& returnEvent, RPG_CharacterEventType_e const& eventType = CE_None) const;
  float GetLastEventTime() const;
  float GetLastDamageTime() const;
  int   GetLastDamageAmount() const;
  float GetLastManaSpendFailTime() const;
  //@}

protected:
  //@{
  // base character stats
  // (Players modify these as their characters advance. Individual to each character.)
  int   m_strength;                                       ///< Strength augments the standard and critical damage this character does with any weapon.
  int   m_dexterity;                                      ///< Dexterity increases the character's critical strike chance and the character's dodge chance
  int   m_spirit;                                         ///< Spirit determines the charcter's maximum mana
  int   m_vitality;                                       ///< Vitality determines the character's maximum health and increases armor
  //@}

  //@{
  // character base values
  // (Tuned by designers, but not accessible to players. Consistent across character class.)
  int   m_baseDamage;                                     ///< Every character has a base damage stat, which is then modified by the equipped weapon and the character's strength
  int   m_baseAttackSpeed;                                ///< How many miliseconds does an attack take? Equipped weapon can modify this speed.
  int   m_baseHealth;                                     ///< Character's healthMax is calculated from Vitality. This is the base to which vitality bonus is added.
  int   m_baseMana;                                       ///< ManaMax is calculated from Spirit. This is the base to which the derived bonus is added.
  //@}

  //@{
  // stat derivation factors
  // (tuned by designers to adjust the effects of base stats on gameplay)
  float m_strengthToWeaponDamage;                         ///< Factor by which Strength increases weapon damage ((DAM * factor) + DAM)
  float m_strengthToCriticalDamage;                       ///< Factor by which Strength increases critical damage ((DAM * factor) + DAM)
  float m_dexterityToCriticalChance;                      ///< Factor by which Dexterity increases critical chance
  float m_dexterityToDodgeChance;                         ///< Factor by which Dexterity increases dodge chance
  float m_spiritToManaMax;                                ///< Conversion factor by which MaxMana is derived from Spirit
  float m_spiritToSpiritDamage;                           ///< Factor by which Spirit increases Spirit damage.
  float m_vitalityToHealthMax;                            ///< Conversion factor by which MaxHealth is derived from Vitality
  float m_vitalityToArm;                                  ///< Factor by which vitality increases armor
  float m_vitalityToBlockChance;                          ///< Factor by which vitality increases block chance
  //@}

  //@{
  // health 
  int   m_health;                                         ///< This character's current health.
  int   m_healthMax;                                      ///< This character's max health.
  float m_healthRegenerationRate;				                  ///< How fast does this character regenerate health (points/second)?
  float m_healthRegenerationDelay;				                ///< How long does this character wait after taking damage before restarting health regeneration (seconds)?
  float m_healthAccum;										                ///< Internally used accumulator for health regeneration
  BOOL  m_healthRegenerationEnabled;			                ///< When true, Health will regenerate over time.
  BOOL  m_unlimitedHealth;									              ///< Similar to God mode, in that character never dies, but still takes damage and registers effects.
  //@}

  //@{
  // mana
  int   m_mana;                                           ///< Mana is a renewable resource used to fire special abilities. This is its current value.
  int   m_manaMax;                                        ///< The character's Spirit stat determines his or her max mana.
  float m_manaRegenerationRate;				                    ///< How fast does this character regenerate mana (points/second)?
  float m_manaAccum;										                  ///< Internally used accumulator for mana regeneration
  BOOL  m_manaRegenerationEnabled;			                  ///< When true, mana will regenerate over time. Set false to suppress.
  BOOL  m_unlimitedMana;                                  ///< Characters with unlimited mana will simply refill their mana pool when an event would have depleted it
  //@}

  //@{
  // experience
  int   m_experience;                                     ///< Experience is earned by defeating enemies and completing quests.
  int   m_experienceLevelThreshold;                       ///< When a character's Experience Level Threshold is reached, the character advances a level.
  //@}

  //@{
  // special abilities
  int   m_healingAmount;                                  ///< How much healing does a healing spell cast by this character apply?
  //@}

  //@{
  // Event Logging
  VSimpleRingBuffer<RPG_CharacterEvent> m_recentCharacterEvents; ///< last few events to have been logged for this character
  void DebugPrintLastCharacterEventLogged() const;
  void DebugPrintCharacterEventLog() const;
  //@}

  friend class RPG_Character;                             ///< needed for the Character var table, unless there's a way to create nested var tables.
};

#endif // RPG_PLUGIN_CHARACTER_STATS_H__

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
