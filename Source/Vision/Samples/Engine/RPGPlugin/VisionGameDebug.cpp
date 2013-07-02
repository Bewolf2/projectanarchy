/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/VisionGameDebug.h>
#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/CharacterStats.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#if (RPG_SCREENSHOT_NODEBUGTEXT)
void RPG_VisionGameDebug::DisplayCharacterHealth(RPG_Character const& character){}
void RPG_VisionGameDebug::DisplayEvent(RPG_Character const& character, RPG_CharacterEvent const& event, UBYTE alpha /*= 255*/, float verticalOffsetAdjust /*= 0.0f*/){}
void RPG_VisionGameDebug::UpdateEventDisplay(RPG_Character const& character){}
void RPG_VisionGameDebug::DebugDisplayCharacterStats(RPG_Character const& character){}
void RPG_VisionGameDebug::DebugDisplayMovementInfo(RPG_Character const& character){}
void RPG_VisionGameDebug::DebugDisplayNavmeshStatus(RPG_Character const& character){}
#else
void RPG_VisionGameDebug::DisplayCharacterHealth(RPG_Character const& character)
{
  RPG_CharacterStats const& characterStats = character.GetCharacterStats();

  const float verticalOffset = 250.f; //@todo: Get the character's height here & replace offset magic number
  VString msg, tempLine; //@todo: Is there a less crufty way to build these strings?

  tempLine.Format("HP: %i / %i\n", characterStats.GetHealth(), characterStats.GetHealthMax());
  msg += tempLine;

  const hkvVec3 vPos = character.GetPosition() + hkvVec3(0.0f, 0.0f, verticalOffset);

  const float damageDisplayTimeRemaining = kDamageDisplayDuration - (Vision::GetTimer()->GetTime() - characterStats.GetLastEventTime());
  const UBYTE maxAlpha = 255;
  const UBYTE minAlpha = 96;
  if (damageDisplayTimeRemaining > 0)
  {
    // recently logged an event. highlight the indicator
    const UBYTE alpha = static_cast<UBYTE>((damageDisplayTimeRemaining * (maxAlpha - minAlpha) / kDamageDisplayDuration) + minAlpha); 
    if (static_cast<float>(characterStats.GetHealth()) / static_cast<float>(characterStats.GetHealthMax()) < 0.25f)
    {
      Vision::Message.SetTextColor(VColorRef(255,96,96, alpha));
    }
    else
    {
      Vision::Message.SetTextColor(VColorRef(127,208,208, alpha));
    }
  }
  else
  {
    // no recent events
    const float criticalHealthPercentage = 0.25f;
    if (static_cast<float>(characterStats.GetHealth()) / static_cast<float>(characterStats.GetHealthMax()) <= criticalHealthPercentage)
    {
      Vision::Message.SetTextColor(VColorRef(208,127,127, minAlpha));
    }
    else
    {
      Vision::Message.SetTextColor(VColorRef(127,127,208, minAlpha));
    }
  }
  Vision::Message.DrawMessage3D(msg, vPos);
  Vision::Message.SetTextColor(); // restore default white
}

void RPG_VisionGameDebug::DisplayEvent(RPG_Character const& character, RPG_CharacterEvent const& event, UBYTE alpha /*= 255*/, float verticalOffsetAdjust /*= 0.0f*/)
{
  const float verticalOffset = 290.f;
  VString msg;
  switch (event.m_eventType)
  {
  case CE_Damage:
    msg.Format("-%i\n", event.m_eventValue);
    Vision::Message.SetTextColor(VColorRef(208,127,127,alpha));
  	break;
  case CE_Block:
    msg = "Blocked";
    Vision::Message.SetTextColor(VColorRef(208,208,127,alpha));
    break;
  case CE_Dodge:
    msg = "Dodged";
    Vision::Message.SetTextColor(VColorRef(208,127,208,alpha));
    break;
  case CE_Heal:
    msg.Format("+%i\n", event.m_eventValue);
    Vision::Message.SetTextColor(VColorRef(127,208,127,alpha));
    break;
  case CE_FailedManaSpend:
    msg = "Insufficient Mana!";
    Vision::Message.SetTextColor(VColorRef(208,127,127,alpha));
    break;
  default:
    msg = "";
    break;
  }

  const hkvVec3 vPos = character.GetPosition() + hkvVec3(0.0f, 0.0f, verticalOffset + verticalOffsetAdjust);
  VVERIFY(Vision::Message.DrawMessage3D(msg, vPos));
  Vision::Message.SetTextColor(); // restore default white
}

void RPG_VisionGameDebug::UpdateEventDisplay(RPG_Character const& character)
{
  RPG_CharacterStats const& characterStats = character.GetCharacterStats();
  float const damageDisplayTimeRemaining = kDamageDisplayDuration - (Vision::GetTimer()->GetTime() - characterStats.GetLastEventTime());
  if (damageDisplayTimeRemaining > 0)
  {
    // fade out
    const UBYTE maxAlpha = 255;
    const UBYTE alpha = static_cast<UBYTE>(damageDisplayTimeRemaining * maxAlpha / kDamageDisplayDuration);

    // float up
    const float maxHeightAdjust = 64.0f;
    const float verticalOffsetAdjust = static_cast<float>(maxHeightAdjust - (damageDisplayTimeRemaining * maxHeightAdjust / kDamageDisplayDuration));

    //DisplayDamage(character, characterStats.GetLastDamageAmount(), alpha, verticalOffsetAdjust);
    RPG_CharacterEvent event;
    if (characterStats.GetLastCharacterEvent(event))
    {
      DisplayEvent(character, event, alpha, verticalOffsetAdjust);
    }
  }
}

void RPG_VisionGameDebug::DebugDisplayCharacterStats(RPG_Character const& character)
{
  const float verticalOffset = 350.f; //@todo: Get the character's height here & replace offset magic number
  VString msg, tempLine; //@todo: Is there a less crufty way to build these strings?

  RPG_CharacterStats const& stats = character.GetCharacterStats();
  {
    tempLine.Format("STR: %i, DEX: %i, SPIRIT: %i, VITALITY: %i\n", stats.GetStrength(), stats.GetDexterity(), stats.GetSpirit(), stats.GetVitality());
    msg += tempLine;
    tempLine.Format("Health: \t %i/%i, Mana: \t %i/%i\n", stats.GetHealth(), stats.GetHealthMax(), stats.GetMana(), stats.GetManaMax());
    msg += tempLine;
    tempLine.Format("Bonuses:\n");
    msg += tempLine;
    tempLine.Format("+STR Wpn Dam: %.1f%%%%, +STR Crit Dam: %.1f%%%%\n", stats.GetWeaponDamageModifier() *100, stats.GetCritDamageModifier() *100);
    msg += tempLine;
    tempLine.Format("+DEX Crit Chance: %.1f%%%%, +DEX Dodge Chance: %.1f%%%%\n", stats.GetCritChance() *100, stats.GetDodgeChance() *100);
    msg += tempLine;
    tempLine.Format("+SPIRIT Spirit Dam: %.1f%%%%\n", stats.GetSpiritDamageModifier() *100);
    msg += tempLine;
    tempLine.Format("+VITALITY Arm Bonus: %.1f%%%%, +VITALITY Block Chance: %.1f%%%%\n", stats.GetArmorBonus() *100, stats.GetBlockChance() *100);
    msg += tempLine;
    tempLine.Format("Combat Values:\n");
    msg += tempLine;
    tempLine.Format("Adjusted Weap. Damage: %i-%i, DPS: %.1f\n", character.GetMinDamage(), character.GetMaxDamage(), character.GetDPS());
    msg += tempLine;
    tempLine.Format("Adjusted Armor: %i\n", character.GetArmor());
    msg += tempLine;
  }

  const hkvVec3 vPos = character.GetPosition() + hkvVec3(0.0f, 0.0f, verticalOffset);
  Vision::Message.DrawMessage3D(msg, vPos);
}

void RPG_VisionGameDebug::DebugDisplayMovementInfo(RPG_Character const& character)
{
  const float verticalOffset = 350.f; //@todo: Get the character's height here & replace offset magic number
  VString msg, tempLine; //@todo: Is there a less crufty way to build these strings?

  tempLine.Format("-- DEBUG DISPLAY: MOVEMENT --\n");
  msg += tempLine;

  if (character.GetBehaviorComponent())
  {
    tempLine.Format("HK Behavior MoveSpeed: %.1f\n", RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<float>(*character.GetBehaviorComponent(), "MoveSpeed"));
    msg += tempLine;
  }

  if (character.GetAiController())
  {
    tempLine.Format("AI Controller Speed: %.1f\n", character.GetAiController()->GetSpeed());
    msg += tempLine;
  }

  const hkvVec3 vPos = character.GetPosition() + hkvVec3(0.0f, 0.0f, verticalOffset);
  Vision::Message.DrawMessage3D(msg, vPos);
}

void RPG_VisionGameDebug::DebugDisplayNavmeshStatus(RPG_Character const& character)
{
  const float verticalOffset = 350.f; //@todo: Get the character's height here & replace offset magic number
  VString msg, tempLine; //@todo: Is there a less crufty way to build these strings?

  tempLine.Format("-- DEBUG DISPLAY: NAVMESH STATUS --\n");
  msg += tempLine;

  if (character.IsOnNavMesh())
  {
    tempLine.Format("ON Navmesh\n");
    msg += tempLine;
    Vision::Message.SetTextColor(VColorRef(24,208,24,255));
  }
  else
  {
    tempLine.Format("OFF Navmesh\n");
    msg += tempLine;
    Vision::Message.SetTextColor(VColorRef(208,24,24,255));

    if (character.GetAiController())
    {
      hkvVec3 navMeshPosition;
      character.GetAiController()->GetClosestPointOnNavMesh(character.GetPosition(), 5.f, navMeshPosition);
      Vision::Game.DrawSingleLine(character.GetPosition(), navMeshPosition, VColorRef(208,24,24), 3.0f);
    }
  }

  const hkvVec3 vPos = character.GetPosition() + hkvVec3(0.0f, 0.0f, verticalOffset);
  VVERIFY(Vision::Message.DrawMessage3D(msg, vPos));
  Vision::Message.SetTextColor(); // restore default white
}

float const RPG_VisionGameDebug::kDamageDisplayDuration = 1.0f;
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
