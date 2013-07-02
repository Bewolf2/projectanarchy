/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/CharacterStats.h>

const int RPG_CHARACTER_EVENT_LOG_SIZE = 5;  // RPG_Character will store this many recent events in its m_recentCharacterEvents log

RPG_CharacterStats::RPG_CharacterStats()
  : m_strength(14)
  , m_dexterity(12)
  , m_spirit(10)
  , m_vitality(12)

  , m_baseDamage(20)
  , m_baseAttackSpeed(400)
  , m_baseHealth(200)
  , m_baseMana(30)

  , m_strengthToWeaponDamage(0.005f)
  , m_strengthToCriticalDamage(0.004f)
  , m_dexterityToCriticalChance(0.002f)
  , m_dexterityToDodgeChance(0.002f)
  , m_spiritToManaMax(1.0f)
  , m_spiritToSpiritDamage(0.005f)
  , m_vitalityToHealthMax(3.6f)
  , m_vitalityToArm(0.0025f)
  , m_vitalityToBlockChance(0.002f)

  , m_health(200)
  , m_healthMax(200)
  , m_healthRegenerationRate(1.5f)
  , m_healthRegenerationDelay(3.0f)
  , m_healthAccum(0.0f)
  , m_healthRegenerationEnabled(TRUE)
  , m_unlimitedHealth(FALSE)

  , m_mana(100)
  , m_manaMax(100)
  , m_manaRegenerationRate(3.0f)
  , m_manaAccum(0.0f)
  , m_manaRegenerationEnabled(TRUE)
  , m_unlimitedMana(FALSE)

  , m_experience(0)
  , m_experienceLevelThreshold(0)
  
  , m_healingAmount(100)

  , m_recentCharacterEvents(RPG_CHARACTER_EVENT_LOG_SIZE)
{
}

void RPG_CharacterStats::Tick(float const deltaTime)
{
  if(m_healthRegenerationEnabled && (Vision::GetTimer()->GetTime() - GetLastDamageTime() >= m_healthRegenerationDelay)) // @todo: do we need server time here?
  {
    UpdateHealthRegeneration(deltaTime);
  }

  if(m_manaRegenerationEnabled)
  {
    UpdateManaRegeneration(deltaTime);
  }
}

// Character base stats
void RPG_CharacterStats::SetStrength(int const strength)
{
  m_strength = strength;
}

int RPG_CharacterStats::GetStrength() const
{
  return m_strength;
}

void RPG_CharacterStats::SetDexterity(int const dexterity)
{
  m_dexterity = dexterity;
}

int RPG_CharacterStats::GetDexterity() const
{
  return m_dexterity;
}

void RPG_CharacterStats::SetSpirit(int const spirit)
{
  m_spirit = spirit;
}

int RPG_CharacterStats::GetSpirit() const
{
  return m_spirit;
}

void RPG_CharacterStats::SetVitality(int const vitality)
{
  m_vitality = vitality;
}

int RPG_CharacterStats::GetVitality() const
{
  return m_vitality;
}

// character base values
int RPG_CharacterStats::GetBaseDamage() const
{
  return m_baseDamage;
}

int RPG_CharacterStats::GetBaseAttackSpeed() const
{
  return m_baseAttackSpeed;
}

int RPG_CharacterStats::GetBaseHealth() const
{
  return m_baseHealth;
}

int RPG_CharacterStats::GetBaseMana() const
{
  return m_baseMana;
}

// character health
void RPG_CharacterStats::SetHealth(int const health)
{
  m_health = health;
  m_health = hkvMath::Min(m_health, m_healthMax);  // will not set m_health > m_healthMax
  m_health = hkvMath::Max(m_health, 0); // maintain 0 as health floor
}

int RPG_CharacterStats::GetHealth() const
{
  return m_health;
}

float RPG_CharacterStats::GetHealthPercentage() const
{
  return static_cast<float>(GetHealth()) / static_cast<float>(GetHealthMax());
}

void RPG_CharacterStats::SetHealthMax(int const healthMax)
{
  VASSERT(healthMax > 0);
  m_healthMax = healthMax;
  m_health = hkvMath::Min(m_health, m_healthMax); // automatically conform health to new max if out of bounds
}

int RPG_CharacterStats::GetHealthMax() const
{
  return m_healthMax;
}

void RPG_CharacterStats::SetHealthRegenerationRate(float const regenerationRate)
{
  m_healthRegenerationRate = regenerationRate;
}

void RPG_CharacterStats::UpdateHealthRegeneration(float const deltaTime)
{
  if (m_health < m_healthMax)
  {
    if (m_healthAccum < 0)
    {
      // clear any partial health burn that may have occurred
      m_healthAccum = 0;
    }
    m_healthAccum += m_healthRegenerationRate * deltaTime;
    if (m_healthAccum >= 1.f)
    {
      int newHealth = hkvMath::Min(m_health + static_cast<int>(hkvMath::floor(m_healthAccum)), m_healthMax);
      m_healthAccum -= hkvMath::floor(m_healthAccum);
      SetHealth(newHealth);
    }
  }
}

void RPG_CharacterStats::SetHealthRegenerationEnabled(bool const enabled)
{
  m_healthRegenerationEnabled = enabled;
}

int RPG_CharacterStats::CalculateHealthMax()
{
  return m_baseHealth + static_cast<int>(m_vitality * m_vitalityToHealthMax);
}

void RPG_CharacterStats::SetUnlimitedHealth(bool unlimitedHealth)
{
  m_unlimitedHealth = unlimitedHealth;
}

bool RPG_CharacterStats::HasUnlimitedHealth() const
{
  return m_unlimitedHealth;
}

float RPG_CharacterStats::GetLastDamageTime() const
{
  // look through the logged event buffer for recorded damage events (0 is oldest, size-1 is newest)
  float lastDamageTime = -1.f;
  for (unsigned int i = 0; i < m_recentCharacterEvents.GetSize(); ++i)
  {
    if (m_recentCharacterEvents[i].m_eventType == CE_Damage)
    {
      lastDamageTime = m_recentCharacterEvents[i].m_eventTime;
    }
  }
  return lastDamageTime;
}

int RPG_CharacterStats::GetLastDamageAmount() const
{
  // look through the logged event buffer for recorded damage events (0 is oldest, size-1 is newest)
  int lastDamageAmount = 0;
  for (unsigned int i = 0; i < m_recentCharacterEvents.GetSize(); ++i)
  {
    if (m_recentCharacterEvents[i].m_eventType == CE_Damage)
    {
      lastDamageAmount = m_recentCharacterEvents[i].m_eventValue;
    }
  }
  return lastDamageAmount;
}


// Mana
void RPG_CharacterStats::SetMana(int const mana)
{
  m_mana = mana;
  m_mana = hkvMath::Min(m_mana, m_manaMax);  // will not set m_mana > m_manaMax
  m_mana = hkvMath::Max(m_mana, 0); // don't allow mana < 0
}

int RPG_CharacterStats::GetMana() const
{
  return m_mana;
}

void RPG_CharacterStats::SetManaMax(int const manaMax)
{
  VASSERT(manaMax > 0);
  m_manaMax = manaMax;
  m_mana = hkvMath::Min(m_mana, m_manaMax); // automatically conform mana to new max if out of bounds
}

int RPG_CharacterStats::GetManaMax() const
{
  return m_manaMax;
}

void RPG_CharacterStats::SetManaRegenerationRate(float const regenerationRate)
{
  m_manaRegenerationRate = regenerationRate;
}

void RPG_CharacterStats::UpdateManaRegeneration(float const deltaTime)
{
  if (m_mana < m_manaMax)
  {
    if (m_manaAccum < 0)
    {
      // clear any partial mana burn that may have occurred
      m_manaAccum = 0;
    }
    m_manaAccum += m_manaRegenerationRate * deltaTime;
    if (m_manaAccum >= 1.f)
    {
      int newmana = hkvMath::Min(m_mana + static_cast<int>(hkvMath::floor(m_manaAccum)), m_manaMax);
      m_manaAccum -= hkvMath::floor(m_manaAccum);
      SetMana(newmana);
    }
  }
}

void RPG_CharacterStats::SetManaRegenerationEnabled(bool const enabled)
{
  m_manaRegenerationEnabled = enabled;
}

int RPG_CharacterStats::CalculateManaMax()
{
  return m_baseMana + static_cast<int>(m_spirit * m_spiritToManaMax);
}

float RPG_CharacterStats::GetLastManaSpendFailTime() const
{
  // look through the logged event buffer for recorded damage events (0 is oldest, size-1 is newest)
  float lastFailedSpendTime = -1.f;
  for (unsigned int i = 0; i < m_recentCharacterEvents.GetSize(); ++i)
  {
    if (m_recentCharacterEvents[i].m_eventType == CE_FailedManaSpend)
    {
      lastFailedSpendTime = m_recentCharacterEvents[i].m_eventTime;
    }
  }
  return lastFailedSpendTime;
}

// experience
void RPG_CharacterStats::SetExperience(int const experience)
{
  m_experience = experience;
}

int RPG_CharacterStats::GetExperience() const
{
  return m_experience;
}

void RPG_CharacterStats::SetExperienceLevelThreshold(int const experienceThreshold)
{
  m_experienceLevelThreshold = experienceThreshold;
}

int RPG_CharacterStats::GetExperienceLevelThreshold() const
{
  return m_experienceLevelThreshold;
}

// derived modifiers
float RPG_CharacterStats::GetWeaponDamageModifier() const
{
  return m_strength * m_strengthToWeaponDamage;
}

float RPG_CharacterStats::GetCritDamageModifier() const
{
  return m_strength * m_strengthToCriticalDamage;
}

float RPG_CharacterStats::GetCritChance() const
{
  return m_dexterity * m_dexterityToCriticalChance;
}

float RPG_CharacterStats::GetDodgeChance() const
{
  return m_dexterity * m_dexterityToDodgeChance;
}

float RPG_CharacterStats::GetSpiritDamageModifier() const
{
  return m_spirit * m_spiritToSpiritDamage;
}

float RPG_CharacterStats::GetArmorBonus() const
{
  return m_vitality * m_vitalityToArm;
}

float RPG_CharacterStats::GetBlockChance() const
{
  return m_vitality * m_vitalityToBlockChance;
}

void RPG_CharacterStats::LogCharacterEvent(RPG_CharacterEventType_e const& eventType, float const eventTime, int const eventValue /*= 0*/, VString const eventMessage /*= ""*/)
{
  VASSERT(eventType != CE_None);
  m_recentCharacterEvents.Add(RPG_CharacterEvent(eventType, eventTime, eventValue, eventMessage));

  //DebugPrintLastCharacterEventLogged();
  //DebugPrintCharacterEventLog();  // print the entire contents of the log to validate data
}

const VSimpleRingBuffer<RPG_CharacterEvent>& RPG_CharacterStats::GetCharacterEventLog() const
{
  return m_recentCharacterEvents;
}

void RPG_CharacterStats::DebugPrintLastCharacterEventLogged() const
{
#ifdef _DEBUG
  if (m_recentCharacterEvents.GetSize() > 0)
  {
    VString msg;
    const RPG_CharacterEvent event = m_recentCharacterEvents.GetLast();
    msg.Format("Event Logged: Type: %i, Time: %f, Value: %i, Message: %s\n", event.m_eventType, event.m_eventTime, event.m_eventValue, "<FIXME: NO MESSAGE>");
    Vision::Error.SystemMessage(msg.AsChar());
  }
#endif
}

void RPG_CharacterStats::DebugPrintCharacterEventLog() const
{
#ifdef _DEBUG
  VString msg;
  for (unsigned int i = 0; i < m_recentCharacterEvents.GetSize(); ++i)
  {
    const RPG_CharacterEvent event = m_recentCharacterEvents[i];
    msg.Format("EventType: %i, Time: %f, Value: %i, Message: %s\n", event.m_eventType, event.m_eventTime, event.m_eventValue, "<FIXME: NO MESSAGE>");
    Vision::Error.SystemMessage(msg.AsChar());
  }
  msg = "-------------------------------------------------------";
  Vision::Error.SystemMessage(msg.AsChar());
#endif
}

bool RPG_CharacterStats::GetLastCharacterEvent(RPG_CharacterEvent& returnEvent, RPG_CharacterEventType_e const& eventType /*= CE_None*/) const
{
  if (eventType == CE_None)
  {
    if (m_recentCharacterEvents.GetSize() > 0)
    {
      returnEvent = m_recentCharacterEvents.GetLast();
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    bool returnValue = false;
    for (unsigned int i = 0; i < m_recentCharacterEvents.GetSize(); ++i)
    {
      const RPG_CharacterEvent event = m_recentCharacterEvents[i];
      if (event.m_eventType == eventType)
      {
        returnEvent = event;
        returnValue = true;
      }
    }
    return returnValue;
  }
}

float RPG_CharacterStats::GetLastEventTime() const
{
  if (m_recentCharacterEvents.GetSize() > 0)
  {
    return m_recentCharacterEvents.GetLast().m_eventTime;
  }
  else
  {
    return -1.f;
  }
}

int RPG_CharacterStats::GetHealingAmount() const
{
  return m_healingAmount;
}

void RPG_CharacterStats::SetUnlimitedMana(bool unlimitedMana)
{
  m_unlimitedMana = unlimitedMana;
}

bool RPG_CharacterStats::HasUnlimitedMana() const
{
  return m_unlimitedMana;
}

RPG_CharacterEvent::RPG_CharacterEvent()
  : m_eventType(CE_None)
  , m_eventTime(0.f)
  , m_eventValue(0)
{
}

RPG_CharacterEvent::RPG_CharacterEvent(RPG_CharacterEventType_e const& eventType, float const eventTime, int const eventValue /*= 0*/, VString const eventMessage /*= ""*/)
  : m_eventType(eventType)
  , m_eventTime(eventTime)
  , m_eventValue(eventValue)
{
}

void RPG_CharacterEvent::Clear()
{
  m_eventType = CE_None;
  m_eventTime = 0.f;
  m_eventValue = 0;
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
