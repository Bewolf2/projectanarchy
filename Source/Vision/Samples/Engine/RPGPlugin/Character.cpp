/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Attachment.h>
#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/DestructibleEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryPickup.h>
#include <Vision/Samples/Engine/RPGPlugin/MeshTrailEffectComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUI.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionGameDebug.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionUserMessages.h>

#include <Animation/Physics2012Bridge/Instance/hkaRagdollInstance.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>

#include <Physics2012/Dynamics/Phantom/hkpShapePhantom.h>
#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>


V_IMPLEMENT_SERIAL(RPG_Character, RPG_DamageableEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_Character, RPG_DamageableEntity, "Character Entity", 0, "")
  // entity properties
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_eyeHeight, "Eye Height", "Height of this character's eyes", "150", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_sensorSize, "Ai SensorSize", "Character Sensor Size", "128", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_desiredSpeed, "Ai DesiredSpeed", "Character Desired Speed", "200", 0, 0);

  // base character stats
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_stats.m_strength, "Strength", "Strength augments weapon damage and critical damage.", "10", 0, 0);
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_stats.m_dexterity, "Dexterity", "Dexterity determines critical and dodge chances.", "10", 0, 0);
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_stats.m_spirit, "Spirit", "Spirit determines the character's max Mana and Spirit attack damage.", "10", 0, 0);
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_stats.m_vitality, "Vitality", "Vitality determines the character's max Health, armor bonus, and block chance.", "10", 0, 0);

  // health
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_stats.m_baseHealth, "Base Health", "Character's healthMax is calculated from Vitality. This is the base to which vitality bonus is added.", "200", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_stats.m_healthRegenerationEnabled, "Regenerate Health", "Does this character's health regenerate?", "TRUE", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_stats.m_healthRegenerationRate, "Health Regeneration Rate", "How many points per second does this character regenerate?", "1.5", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_stats.m_unlimitedHealth, "Unlimited Health", "If true, this character's health resets to max when reduced to 0, and doesn't trigger death.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_displayHealth, "Display Health", "Enables/Disables Character Health display.", "TRUE", 0, 0);

  // mana
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_stats.m_baseMana, "Base Mana", "ManaMax is calculated from Spirit. This is the base to which the derived bonus is added.", "30", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_stats.m_manaRegenerationEnabled, "Regenerate Mana", "Does this character's mana regenerate?", "TRUE", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_stats.m_manaRegenerationRate, "Mana Regeneration Rate", "How many points per second does this character regenerate?", "1.5", 0, 0);

  // actions
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_spawnAnimationDelay, "Spawn Animation Delay", "How many seconds should this character delay playing the spawn animation?", "0", 0, 0);

  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_healProjectileSpeed, "Heal Projectile Speed", "How fast does the heal projectile move?", "1000", 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_actionData.m_healProjectileLaunchBone, "Heal Projectile Launch Bone", "From which bone is the heal projectile spawned?", "", 0, 0, 0);

  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_rangedAttackProjectileSpeed, "Ranged Attack Projectile Speed", "How fast does the ranged attack projectile move?", "1000", 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_actionData.m_rangedAttackProjectileLaunchBone, "Ranged Attack Projectile Launch Bone", "From which bone is the ranged attck projectile spawned?", "", 0, 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_rangedAttackImpactSpeedMultiplier, "Ranged Attack Impact Speed Multiplier", "How fast does the enemy ragdoll move when killed by a projectile", "1", 0, 0);

  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_stats.m_baseDamage, "Base Damage", "Every character has a base damage stat, which is then modified by the equipped weapon and the character's strength", "20", 0, 0);
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_stats.m_baseAttackSpeed, "Base Attack Speed", "How many miliseconds does an attack take? Equipped weapon can modify this speed.", "400", 0, 0);

  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_aoeAttackDamageMultiplier, "AoE Attack Damage Multiplier", "Weapon damage is multiplied by this factor in an AoE attack.", "2", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_aoeAttackRange, "AoE Attack Range", "Enemies up to this range will be hit by an AoE attack.", "250", 0, 0);
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_actionData.m_aoeAttackManaCost, "AoE Attack Mana Cost", "Mana required to execute an AoE attack.", "5", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_aoeAttackImpactSpeedMultiplier, "AoE Attack Impact Speed Multiplier", "How fast does the enemy ragdoll move when killed by an AoE attack", "1.5", 0, 0);

  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_powerAttackDamageMultiplier, "Power Attack Damage Multiplier", "Weapon damage is multiplied by this factor in a Power attack.", "3", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_powerAttackRange, "Power Attack Range", "Enemies up to this range and within the angle will be hit by a Power attack.", "550", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_powerAttackAngle, "Power Attack Angle", "Enemies within this angle and in range will be hit by a Power attack.", "10", 0, 0);
  DEFINE_VAR_INT_AND_NAME(RPG_Character, m_actionData.m_powerAttackManaCost, "Power Attack Mana Cost", "Mana required to execute a Power attack.", "5", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_powerAttackImpactSpeedMultiplier, "Power Attack Impact Speed Multiplier", "How fast does the enemy ragdoll move when killed by a power attack", "2", 0, 0);
  
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_actionData.m_meleeAttackImpactSpeedMultiplier, "Melee Attack Impact Speed Multiplier", "How fast does the enemy ragdoll move when killed by a melee attack", "1", 0, 0);
  
  // ai
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Character, m_aggroRadius, "Aggro Radius", "The Range at which the character can detect an enemy when not player controlled", "600", 0, 0);

  // inventory & equipment
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_projectileMeshFilename, "Projectile Model", "Projectile model spawned by this character's ranged attack", "", 0, 0, "filepicker(.model)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_equipmentAttachmentBoneNames[ES_Helmet], "Bone Name: Helmet", "Bone Name for the Helmet Attachment", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_equipmentAttachmentBoneNames[ES_Chest], "Bone Name: Chest", "Bone Name for the Chest Attachment", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_equipmentAttachmentBoneNames[ES_Gloves], "Bone Name: Gloves", "Bone Name for the Gloves Attachment", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_equipmentAttachmentBoneNames[ES_Shoulder], "Bone Name: Shoulder", "Bone Name for the Weapon Shoulder", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_equipmentAttachmentBoneNames[ES_Belt], "Bone Name: Belt", "Bone Name for the Belt Attachment", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_equipmentAttachmentBoneNames[ES_Boots], "Bone Name: Boots", "Bone Name for the Boots Attachment", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_equipmentAttachmentBoneNames[ES_Weapon], "Bone Name: Weapon", "Bone Name for the Weapon Attachment", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipment[ES_Weapon], "Default Weapon", "Default Weapon Pickup Prefab", "", 0, 0, "filepicker(.vPrefab)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipment[ES_Helmet], "Default Helmet", "Default Helmet Pickup Prefab", "", 0, 0, "filepicker(.vPrefab)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipment[ES_Chest], "Default Chest", "Default Chest Pickup Prefab", "", 0, 0, "filepicker(.vPrefab)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipment[ES_Gloves], "Default Gloves", "Default Gloves Pickup Prefab", "", 0, 0, "filepicker(.vPrefab)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipment[ES_Shoulder], "Default Shoulder", "Default Shoulder Pickup Prefab", "", 0, 0, "filepicker(.vPrefab)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipment[ES_Belt], "Default Belt", "Default Belt Prefab", "", 0, 0, "filepicker(.vPrefab)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipment[ES_Boots], "Default Boots", "Default Boots Prefab", "", 0, 0, "filepicker(.vPrefab)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipmentScripts[ES_Weapon], "Default Weapon Script", "Default Weapon Pickup Script", "", 0, 0, "filepicker(.lua)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipmentScripts[ES_Helmet], "Default Helmet Script", "Default Helmet Pickup Script", "", 0, 0, "filepicker(.lua)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipmentScripts[ES_Chest], "Default Chest Script", "Default Chest Pickup Script", "", 0, 0, "filepicker(.lua)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipmentScripts[ES_Gloves], "Default Gloves Script", "Default Gloves Pickup Script", "", 0, 0, "filepicker(.lua)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipmentScripts[ES_Shoulder], "Default Shoulder Script", "Default Shoulder Pickup Script", "", 0, 0, "filepicker(.lua)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipmentScripts[ES_Belt], "Default Belt Script", "Default Belt Script", "", 0, 0, "filepicker(.lua)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Character, m_defaultEquipmentScripts[ES_Boots], "Default Boots Script", "Default Boots Script", "", 0, 0, "filepicker(.lua)");

  // death
  DEFINE_VAR_VSTRING(RPG_Character, m_healthPickupScriptName, "Script used to spawn health pickups on death.", "Scripts\\Pickups\\RPG_HealthPickup.lua", 0, 0, "filepicker(.lua)");
  DEFINE_VAR_INT(RPG_Character, m_numHealthPickupsDroppedOnDeathMin, "Minimum Health Pickups Dropped on death.", "3", 0, 0);
  DEFINE_VAR_INT(RPG_Character, m_numHealthPickupsDroppedOnDeathMax, "Maximum Health Pickups Dropped on death.", "5", 0, 0);
  DEFINE_VAR_VSTRING(RPG_Character, m_manaPickupScriptName, "Script used to spawn mana pickups on death.", "Scripts\\Pickups\\RPG_ManaPickup.lua", 0, 0, "filepicker(.lua)");
  DEFINE_VAR_INT(RPG_Character, m_numManaPickupsDroppedOnDeathMin, "Minimum Mana Pickups Dropped on death.", "3", 0, 0);
  DEFINE_VAR_INT(RPG_Character, m_numManaPickupsDroppedOnDeathMax, "Maximum Mana Pickups Dropped on death.", "5", 0, 0);

  // debugging
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_debugDisplayStats, "Display Debug Stats", "Enables/Disables Character Debug Rendering.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_debugDisplayMovementInfo, "Display Debug Movement Info", "Enables/Disables Character Movement Debug Rendering.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_debugDisplayInventory, "Display Inventory", "Enables/Disables Character Inventory Debug Rendering.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_debugDisplayBehaviorEvents, "Display Debug Behavior Events", "Enables/Disables Character Debug Rendering.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_debugDisplayNavmeshStatus, "Display Debug Navmesh Status", "Enables/Disables Character Debug Rendering.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_debugDisplayEffectInformation, "Display Effect Information", "Prints the filenames of effects called by this character.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Character, m_debugDisplayStateInformation, "Display State Information", "Prints information about this character's AI state.", "FALSE", 0, 0);
END_VAR_TABLE


/// Constructor
RPG_Character::RPG_Character()
  : RPG_DamageableEntity()
  , m_eyeHeight(150.0f)
  , m_sensorSize(128.0f)
  , m_desiredSpeed(200.0f)
  , m_aggroRadius(500.0f)
  , m_knockBackStartTime(0.0f)
  , m_knockBackDuration(0.0f)
  , m_knockBackVector(0.0f, 0.0f, 0.0f)
  , m_locallyControlled(false)
  , m_dead(false)
  , m_feigningDeath(false)
  , m_timeOfDeath(0.0f)
  , m_deathDissolveDelay(10.0f)
  , m_deathImpulse(0.0f, 0.0f, 0.0f)
  , m_actionHandler()
  , m_actionData()
  , m_inventoryHandler()
  , m_stats()
  , m_projectileMeshFilename("")
  , m_displayHealth(FALSE)
  , m_equipmentAttachments()
  , m_defaultTeam(TEAM_None)
  , m_team(TEAM_None)
  , m_healthPickupScriptName()
  , m_numHealthPickupsDroppedOnDeathMin(3)
  , m_numHealthPickupsDroppedOnDeathMax(5)
  , m_manaPickupScriptName()
  , m_numManaPickupsDroppedOnDeathMin(3)
  , m_numManaPickupsDroppedOnDeathMax(5)
  , m_debugDisplayStats(FALSE)
  , m_debugDisplayMovementInfo(FALSE)
  , m_debugDisplayInventory(FALSE)
  , m_debugDisplayBehaviorEvents(FALSE)
  , m_debugDisplayNavmeshStatus(FALSE)
  , m_debugDisplayEffectInformation(FALSE)
  , m_debugDisplayStateInformation(FALSE)
{
  m_collisionRadius = 40.0f;
  m_collisionHeight = 160.0f;

  for(int index = 0; index < ES_Count; ++index)
  {
    m_equipmentAttachmentBoneNames[index] = "";
    m_defaultEquipment[index] = "";
    m_defaultEquipmentScripts[index] = "";
  }
  
  for (int i = 0; i < FX_Count; ++i)
  {
    m_characterEffectDefinitions[i] = RPG_EffectDefinition();
    m_characterEffects[i] = NULL;
  }
}

void RPG_Character::CalcPositionForTargeting(hkvVec3& targetOut) const
{
  targetOut = 0.5f * (GetEyePosition() + GetPosition());
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_Character::GetDependencies(VResourceSnapshot &snapshot)
{
  RPG_DamageableEntity::GetDependencies(snapshot);

  RPG_DependencyHelper::AddModelDependency(m_projectileMeshFilename, snapshot);

  for(int index = 0; index < ES_Count; ++index)
  {
    RPG_DependencyHelper::AddPrefabDependency(m_defaultEquipment[index], snapshot);
    RPG_DependencyHelper::AddScriptEntityDependency(m_defaultEquipmentScripts[index], snapshot);
  }

  for(int index = 0; index < FX_Count; ++index)
  {
    RPG_DependencyHelper::AddParticleEffectDependency(m_characterEffectDefinitions[index].m_vfxFilename, snapshot);
    RPG_DependencyHelper::AddTextureDependency(m_characterEffectDefinitions[index].m_wallmarkTextureFilename, snapshot);
  }

  RPG_DependencyHelper::AddScriptEntityDependency(m_healthPickupScriptName, snapshot);
  RPG_DependencyHelper::AddScriptEntityDependency(m_manaPickupScriptName, snapshot);
}
#endif

void RPG_Character::Initialize()
{
  RPG_DamageableEntity::Initialize();

  // initialize this character's team to the default for its class
  SetTeam(m_defaultTeam);
}

void RPG_Character::InitializeProperties()
{
  InitializeCharacterEffects();
  RPG_DamageableEntity::InitializeProperties();
}

/// Called to shutdown the character
void RPG_Character::DisposeObject()
{
  if(Vision::Editor.IsPlaying())
  {
    RPG_GameManager::s_instance.RemoveCharacter(this);
  }

  m_characterController = NULL;
  m_havokBehavior = NULL;
  m_controller = NULL;

  RPG_DamageableEntity::DisposeObject();
}

/// Tick / Entity update
void RPG_Character::ThinkFunction()
{
  const float deltaTime = Vision::GetTimer()->GetTimeDifference();

  // dead characters don't do anything but decompose.
  if (IsDead())
  {
    UpdateDeathState(deltaTime);
    return;
  }

  // TODO - implement both server and client tick
  ServerTick(deltaTime);
  UpdateBehaviorWorldFromModel();
}

void RPG_Character::ServerTick(float const deltaTime)
{
  // @TODO: dispose entity when dead after a certain amount of time.
  if (!IsDead())
  {
    m_stats.Tick(deltaTime);

    if (m_displayHealth)
    {
      RPG_VisionGameDebug::DisplayCharacterHealth(*this);
      RPG_VisionGameDebug::UpdateEventDisplay(*this);
    }
  }

  if (!IsDead() || m_actionHandler.IsPerformingAction(AT_Die))
  {
    m_actionHandler.Tick(deltaTime);
    m_inventoryHandler.Tick(deltaTime);
  }

//#ifdef _DEBUG   // @note (kmack): don't prevent debug output for designers & artists using release builds
  if (m_debugDisplayStats && !IsDead())
  {
    RPG_VisionGameDebug::DebugDisplayCharacterStats(*this);
  }
  if (m_debugDisplayMovementInfo && !IsDead())
  {
    RPG_VisionGameDebug::DebugDisplayMovementInfo(*this);
  }
  if (m_debugDisplayInventory && !IsDead())
  {
    m_inventoryHandler.DebugDisplayInventory();
  }
  if (m_debugDisplayNavmeshStatus && !IsDead())
  {
    RPG_VisionGameDebug::DebugDisplayNavmeshStatus(*this);
  }
//#endif

  if(m_controller)
  {
    m_controller->ServerTick(deltaTime);

    m_havokBehavior->SetFloatVar("MoveSpeed", m_controller->GetSpeed());
  }
  else
  {
    if(m_characterController)
    {
      RemoveComponent(m_characterController);
      m_characterController = NULL;
    }
  }

  // kill characters who leave the navmesh
  if(!IsOnNavMesh())
  {
    Die();
  }
}

//void RPG_Character::ClientTick(float const deltaTime)
//{
//}

hkvVec3 const RPG_Character::GetEyePosition() const
{
  hkvVec3 position = GetPosition();
  position.z += m_eyeHeight;

  return position;
}

/// Sets whether or not this character is locally controlled
void RPG_Character::SetLocallyControlled(bool locallyControlled)
{
  m_locallyControlled = locallyControlled;

  //if(m_locallyControlled)
  //{
  //  RPG_PlayerUI::s_instance.SetPlayer(this);

  //  RPG_PlayerCamera* camera = static_cast<RPG_PlayerCamera*>(Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_PlayerCamera)));

  //  if(camera)
  //  {
  //    camera->SetOwner(this);
  //  }
  //}
}

/// Gets whether or not this character is locally controlled
bool RPG_Character::GetLocallyControlled() const
{
  return m_locallyControlled;
}

const RPG_ActionHandler& RPG_Character::GetActionHandler() const
{
  return m_actionHandler;
}

RPG_ActionHandler& RPG_Character::GetActionHandler()
{
  return m_actionHandler;
}

const RPG_InventoryHandler& RPG_Character::GetInventoryHandler() const
{
  return m_inventoryHandler;
}

RPG_InventoryHandler& RPG_Character::GetInventoryHandler()
{
  return m_inventoryHandler;
}

const RPG_CharacterStats& RPG_Character::GetCharacterStats() const
{
  return m_stats;
}

RPG_CharacterStats& RPG_Character::GetCharacterStats()
{
  return m_stats;
}

const RPG_CharacterActionData& RPG_Character::GetCharacterActionData() const
{
  return m_actionData;
}

RPG_CharacterActionData& RPG_Character::GetCharacterActionData()
{
  return m_actionData;
}

/// Applies damage to this character
///
/// param[in] damageAmount    amount of damage to apply, before armor / resistances
///
/// @return int   the amount of damage actually applied
int RPG_Character::TakeDamage(const int damageAmount, const hkvVec3& impactVelocity /*= hkvVec3(0,0,0)*/)
// XXX TODO -
// If knockback is not implemented, it is misleading to take in impactVelocity here,
// as impactVelocity is currently ignored except when the character is killed.
{
  // ignore damage if dead
  if (IsDead())
  {
    return 0;
  }

  // subtract armor from the damage dealt
  int appliedDamage = damageAmount - GetArmor();
  
  // check for dodge or block
  const float damageRoll = Vision::Game.GetFloatRand();

  // test dodge first - dodge is preferable, as it evades all damage
  if (damageRoll <= GetCharacterStats().GetDodgeChance())
  {
    // dodged the attack. Take no damage.
#ifdef _DEBUG
    const VString msg = "Dodged Attack";
    Vision::Error.SystemMessage(msg.AsChar());
    Vision::Message.Add(1, msg.AsChar());
#endif
    m_stats.LogCharacterEvent(CE_Dodge, Vision::GetTimer()->GetTime());
    return 0;
  }
  // if a dodge didn't happen, see whether we're able to block
  else if (IsShieldEquipped() && damageRoll <= GetCharacterStats().GetBlockChance())
  {
    // blocked the attack. Reduce damage.
#ifdef _DEBUG
    const VString msg = "Blocked Attack";
    Vision::Error.SystemMessage(msg.AsChar());
    Vision::Message.Add(1, msg.AsChar());
#endif
    const float blockDamageReduction = 0.5f;  // @todo: damage reduction factor may be better implemented as a factor of the equipped shield item instead.
    appliedDamage -= static_cast<int>(static_cast<float>(appliedDamage) * blockDamageReduction);  // subtracting applied factor, rather than simply multiplying, allows us to specify block factor as the % reduced
    m_stats.LogCharacterEvent(CE_Block, Vision::GetTimer()->GetTime());
  }
  
  appliedDamage = hkvMath::Max(appliedDamage, 0);

  // set a new health value based on the damage applied
  m_stats.SetHealth(m_stats.GetHealth() - appliedDamage);

  // record applied damage
  m_stats.LogCharacterEvent(CE_Damage, Vision::GetTimer()->GetTime(), appliedDamage);

  //DisplayDamage(appliedDamage);
  CreateCharacterEffect(FX_TakeHit);

  // should this character die?
  if (m_stats.GetHealth() <= 0)
  {
    if (m_stats.HasUnlimitedHealth())
    {
      m_stats.SetHealth(m_stats.GetHealthMax());
    }
    else
    {
      m_deathImpulse = impactVelocity;
      Die();
    }
  }

  return appliedDamage;
}

void RPG_Character::ApplyKnockBack( const hkvVec3& knockBackVector, const float duration )
{
  // This intentionally overwrites any previous knockback, as stacking knockbacks when
  // multiple enemies are attacking the player renders the game unplayable.
  m_knockBackStartTime = Vision::GetTimer()->GetCurrentTime();
  m_knockBackDuration = duration;
  m_knockBackVector = knockBackVector;
}

/// Applies healing to this character
///
/// param[in] healingAmount    amount of healing to apply
///
/// @return int   the amount of healing actually applied
int RPG_Character::AddHealth(const int healthAmount)
{
  // ignore healing if dead
  if (IsDead())
  {
    return 0;
  }

  // SetHealth will automatically account for healing greater than max health, so we can keep it simple here.
  m_stats.SetHealth(m_stats.GetHealth() + healthAmount);

  // green flash & other effects
  CreateCharacterEffect(FX_ReceiveHeal);

  // just return the amount we wanted to heal.
  return healthAmount;
}

void RPG_Character::AddMana(const int manaAmount)
{
  // ignore if dead
  if (IsDead())
  {
    return;
  }

  m_stats.SetMana(m_stats.GetMana() + manaAmount);

  // orange flash & other effects
  CreateCharacterEffect(FX_ReceiveMana);
}

void RPG_Character::Die()
{
  m_dead = true;
  m_timeOfDeath = Vision::GetTimer()->GetTime();

  StopAllPersistentCharacterEffects();
  SetWeaponTrailEnabledForEquippedWeapon(false);
  DetachEquipmentOnDeath();

  // override any active Action with the Dying Action.
  GetActionHandler().PerformAction(AT_Die, true);
}

bool RPG_Character::IsAttacking() const
{
  return m_actionHandler.IsPerformingAction(AT_MeleeAttack) ||
    m_actionHandler.IsPerformingAction(AT_PowerAttack) ||
    m_actionHandler.IsPerformingAction(AT_AoeAttack) ||
    m_actionHandler.IsPerformingAction(AT_RangedAttack);
}

bool RPG_Character::IsDoingSpecialAttack() const
{
  return m_actionHandler.IsPerformingAction(AT_PowerAttack) ||
    m_actionHandler.IsPerformingAction(AT_AoeAttack);
}

bool RPG_Character::IsTargetWithinAttackRange(RPG_DamageableEntity* target) const
{
  return IsTargetWithinRange(target, GetEquippedWeaponRange());
}

bool RPG_Character::IsTargetWithinAttackRange(const hkvVec3& targetPosition) const
{
  return IsTargetWithinRange(targetPosition, GetEquippedWeaponRange());
}

bool RPG_Character::IsTargetWithinRange(RPG_DamageableEntity* target, float const range) const
{
  float const rangeSquared = range * range;

  hkvVec3 const& currentPosition = GetPosition();

  float currentRangeSquared = (currentPosition - target->GetPosition()).getLengthSquared();

  // subtract radius of both characters
  currentRangeSquared -= GetCollisionRadius() * GetCollisionRadius();
  currentRangeSquared -= target->GetCollisionRadius() * target->GetCollisionRadius();
  
  if(currentRangeSquared <= rangeSquared)
  {
    return true;
  }

  return false;
}

bool RPG_Character::IsTargetWithinRange(const hkvVec3& targetPosition, float const range) const
{
  float const rangeSquared = range * range;

  hkvVec3 const& currentPosition = GetPosition();

  float currentRangeSquared = (currentPosition - targetPosition).getLengthSquared();

  // subtract my radius
  currentRangeSquared -= GetCollisionRadius() * GetCollisionRadius();

  if(currentRangeSquared <= rangeSquared)
  {
    return true;
  }

  return false;
}

bool RPG_Character::IsDead() const
{
  return m_dead;
}

bool RPG_Character::IsFeigningDeath() const
{
  return m_feigningDeath;
}

void RPG_Character::SpawnDeathPickups()
{
  // spawn health pickups
  int numHealthPickupsToSpawn = m_numHealthPickupsDroppedOnDeathMin;

  VASSERT(m_numHealthPickupsDroppedOnDeathMax >= m_numHealthPickupsDroppedOnDeathMin);
  if(m_numHealthPickupsDroppedOnDeathMin != m_numHealthPickupsDroppedOnDeathMax)
  {
    numHealthPickupsToSpawn = m_numHealthPickupsDroppedOnDeathMin + Vision::Game.GetRand() % (m_numHealthPickupsDroppedOnDeathMax - m_numHealthPickupsDroppedOnDeathMin + 1);
  }

  for(int index = 0; index < numHealthPickupsToSpawn; ++index)
  {
    RPG_GameManager::s_instance.CreateEntityFromScript(m_healthPickupScriptName, GetEyePosition(), GetOrientation());
  }

  // spawn mana pickups
  int numManaPickupsToSpawn = m_numManaPickupsDroppedOnDeathMin;

  VASSERT(m_numManaPickupsDroppedOnDeathMax >= m_numManaPickupsDroppedOnDeathMin);
  if(m_numManaPickupsDroppedOnDeathMin != m_numManaPickupsDroppedOnDeathMax)
  {
    numManaPickupsToSpawn = m_numManaPickupsDroppedOnDeathMin + Vision::Game.GetRand() % (m_numManaPickupsDroppedOnDeathMax - m_numManaPickupsDroppedOnDeathMin + 1);
  }

  for(int index = 0; index < numManaPickupsToSpawn; ++index)
  {
    RPG_GameManager::s_instance.CreateEntityFromScript(m_manaPickupScriptName, GetEyePosition(), GetOrientation());
  }
}

const hkvVec3& RPG_Character::GetDeathImpulse() const
{
  return m_deathImpulse;
}

vHavokBehaviorComponent *RPG_Character::GetBehaviorComponent()
{
  return m_havokBehavior;
}

vHavokBehaviorComponent const *RPG_Character::GetBehaviorComponent() const
{
  return m_havokBehavior;
}

void RPG_Character::OnHavokBehaviorEvent(const VString& eventName)
{
#ifdef _DEBUG
  if (m_debugDisplayBehaviorEvents)
  {
    VString msg;
    msg.Format("Behavior Event: %s", eventName.AsChar());
    Vision::Error.SystemMessage(msg.AsChar());
    Vision::Message.Add(1, msg.AsChar());
  }
#endif

  // pass this event on to the action handler
  m_actionHandler.OnHavokBehaviorEvent(eventName);

  if (eventName == "LEFT_FOOT_DOWN" || eventName == "RIGHT_FOOT_DOWN")
  {
    //Vision::Error.SystemMessage("Event: Footstep");
    CreateCharacterEffect(FX_Footstep);
  }
}

void RPG_Character::SetController(RPG_ControllerComponent *newController)
{
  IVObjectComponent *const controller = Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_ControllerComponent));
  if(controller)
  {
    RemoveComponent(controller);
  }

  if(newController)
  {
    AddComponent(newController);
  }

  m_controller = newController;
}

RPG_ControllerComponent* RPG_Character::GetController()
{
  return m_controller;
}

const RPG_ControllerComponent* RPG_Character::GetController() const
{
  return m_controller;
}

/// Returns the Aggro radius. Used for Ai characters not under player control. This could include a player under certain status effects.
float RPG_Character::GetAggroRadius() const
{
  return m_aggroRadius;
}

int RPG_Character::GetMinDamage() const
{
  const RPG_InventoryItem* equippedWeapon = m_inventoryHandler.GetEquippedWeapon();
  if (equippedWeapon)
  {
    float damage = m_stats.GetBaseDamage() * equippedWeapon->GetMinDamage();
    damage += damage * m_stats.GetWeaponDamageModifier();
    return static_cast<int>(damage);
  }
  else
  {
    return m_stats.GetBaseDamage();
  }
}

int RPG_Character::GetMaxDamage() const
{
  const RPG_InventoryItem* equippedWeapon = m_inventoryHandler.GetEquippedWeapon();
  if (equippedWeapon)
  {
    float damage = m_stats.GetBaseDamage() * equippedWeapon->GetMaxDamage();
    damage += damage * m_stats.GetWeaponDamageModifier();
    return static_cast<int>(damage);
  }
  else
  {
    return m_stats.GetBaseDamage();
  }
}

float RPG_Character::GetAttackSpeed() const
{
  const RPG_InventoryItem* equippedWeapon = m_inventoryHandler.GetEquippedWeapon();
  if (equippedWeapon)
  {
    return m_stats.GetBaseAttackSpeed() * equippedWeapon->GetSpeed();
  }
  else
  {
    return static_cast<float>(m_stats.GetBaseAttackSpeed());
  }
}

float RPG_Character::GetDPS() const
{
  return 1000.0f/GetAttackSpeed() * ((GetMinDamage() + GetMaxDamage()) /2.0f); // atk/second * avg adjusted damage
}

int RPG_Character::GetArmor() const
{
  // armor is calculated as a float, and fractional component is discarded for the return val
  float armor=0.0f;
  for (int i=ES_None+1; i<ES_Count; ++i)
  {
    const RPG_InventoryItem* equippedArmor = m_inventoryHandler.GetEquippedArmor(static_cast<RPG_EquipmentSlot_e>(i));
    if (equippedArmor)
    {
      armor += equippedArmor->GetArmorValue();
    }
  }
  armor += armor * m_stats.GetArmorBonus();
  return static_cast<int>(armor);
}

bool RPG_Character::IsShieldEquipped() const
{
  for (int i=ES_None+1; i<ES_Count; ++i)
  {
    const RPG_InventoryItem* equippedArmor = m_inventoryHandler.GetEquippedArmor(static_cast<RPG_EquipmentSlot_e>(i));
    if (equippedArmor)
    {
      if (equippedArmor->IsShieldType())
      {
        return true;
      }
    }
  }
  return false;
}

void RPG_Character::UpdateBehaviorWorldFromModel()
{
  VASSERT(m_havokBehavior);
  VASSERT(m_havokBehavior->m_character);
  if(m_havokBehavior && m_havokBehavior->m_character)
  {
    // Behavior transform update
    hkQsTransform worldFromModel;
    {
      RPG_VisionHavokConversion::VisionToHavokRotationQuaternion(GetRotationMatrix(), worldFromModel.m_rotation);
      worldFromModel.setScale(hkVector4::getConstant<HK_QUADREAL_1>());
      RPG_VisionHavokConversion::VisionToHavokPoint(GetPosition(), worldFromModel.m_translation);
    }

    m_havokBehavior->m_character->setWorldFromModel(worldFromModel);
  }
}

void RPG_Character::MessageFunction(int id, INT_PTR paramA, INT_PTR paramB)
{
  RPG_DamageableEntity::MessageFunction(id, paramA, paramB);

  switch(id)
  {
  case RPG_VisionUserMessages::kPostAiCharacterStep:
    {
      union { INT_PTR ip; float f; } const INT_PTR__float = { paramA };

      hkaiCharacter const* aiCharacter = reinterpret_cast<hkaiCharacter const*>(paramB);
      float const deltaTime = INT_PTR__float.f;

      if(m_characterController)
      {
        hkVector4 aiDisplacement;
        aiDisplacement.setMul(deltaTime, aiCharacter->getVelocity());

        hkvVec3 displacement;
        RPG_VisionHavokConversion::HavokToVisionPoint(aiDisplacement, displacement);

        IncMotionDeltaWorldSpace(displacement);

        if(m_knockBackDuration > 0.0f)
        {
          const float currentTime = Vision::GetTimer()->GetCurrentTime();
          const float knockBackTimeNormalized = (currentTime - m_knockBackStartTime) / m_knockBackDuration;
          if(knockBackTimeNormalized < 1.0f)
          {
            const hkvVec3 knockBackVector = m_knockBackVector * (1.0f - knockBackTimeNormalized);
            IncMotionDeltaWorldSpace(knockBackVector);
          }
          else
          {
            m_knockBackDuration = 0.0f;
            m_knockBackStartTime = 0.0f;
            m_knockBackVector.setZero();
          }
        }

        m_characterController->Step(INT_PTR__float.f, 1, INT_PTR__float.f);

        vHavokPhysicsModule::GetInstance()->MarkForWrite();
        const_cast<hkaiCharacter*>(aiCharacter)->setPosition(m_characterController->GetCharacterProxy()->getPosition());
        vHavokPhysicsModule::GetInstance()->UnmarkForWrite();
      }

      // Update behavior animation
      VASSERT(m_havokBehavior);
      VASSERT(m_havokBehavior->m_character);
      if(m_havokBehavior && m_havokBehavior->m_character)
      {
        VASSERT(m_controller);
        if(m_controller)
        {
          m_havokBehavior->SetFloatVar("MoveSpeed", m_controller->GetSpeed());
        }
      }
    }
    break;
  }
}

VBool RPG_Character::WantsDeserializationCallback(const VSerializationContext& context)
{
  return TRUE;
}

void RPG_Character::OnDeserializationCallback(const VSerializationContext& context)
{
  RPG_DamageableEntity::OnDeserializationCallback(context);
}

void RPG_Character::PostInitialize()
{
  RPG_DamageableEntity::PostInitialize();

  if (m_debugDisplayEffectInformation)
  {
    ActivateCharacterEffectDebugDisplay();
  }

  if(!Vision::Editor.IsPlaying())
  {
    return;
  }

  // Behavior animation init
  m_havokBehavior = static_cast<vHavokBehaviorComponent *>(Components().GetComponentOfType(V_RUNTIME_CLASS(vHavokBehaviorComponent)));

  if(!m_havokBehavior->m_character)
  {
    m_havokBehavior->InitVisionCharacter(this);
  }

  UpdateBehaviorWorldFromModel();

  // AI controller init
  if(GetControllerComponentType())
  {
    m_controller =  static_cast<RPG_ControllerComponent*>(GetControllerComponentType()->CreateInstance());
    
    m_controller->SetSensorSize(m_sensorSize);
    m_controller->SetDesiredSpeed(m_desiredSpeed);

    AddComponent(m_controller);
  }
  else
  {
    m_controller = static_cast<RPG_ControllerComponent*>(Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_ControllerComponent)));
  }
  VASSERT_MSG(m_controller, "This character requires an AI component. Please put an (RPG) AI Controller or (RPG) Player AI Controller on it.");

  // create attachment proxies
  for(int index = 0; index < ES_Count; ++index)
  {
    m_equipmentAttachments.Add(new RPG_Attachment(this));
  }

  // Havok Character Proxy init
  m_characterController = new vHavokCharacterController;
  m_characterController->Capsule_Radius = m_collisionRadius;
  m_characterController->Character_Bottom = hkvVec3(0.0f, 0.0f, m_collisionRadius);
  m_characterController->Character_Top = hkvVec3(0.0f, 0.0f, m_collisionHeight - m_collisionRadius);
  AddComponent(m_characterController);

  // Initialize health
  m_stats.SetHealthMax(m_stats.CalculateHealthMax());
  m_stats.SetHealth(m_stats.GetHealthMax());

  // Initialize mana
  m_stats.SetManaMax(m_stats.CalculateManaMax());
  m_stats.SetMana(m_stats.GetManaMax());

  // @temp: apply temporary experience values
  m_stats.SetExperienceLevelThreshold(1000);  // @temp: test value
  m_stats.SetExperience(386); // @temp: test value

  m_inventoryHandler.Initialize(this);
  //RPG_InventoryHandler::AddDefaultInventory(*this);

  m_actionHandler.Initialize(this);

  RPG_GameManager::s_instance.AddCharacter(this);

  // setup default equipment
  SetUpDefaultEquipment();

  // play this character's spawn effect
  GetActionHandler().PerformAction(AT_Spawn, true);

  // if this character has any ambient effects defined, start them up now.
  CreateCharacterEffect(FX_Ambient);
}

/// Attaches an equipment prefab to the bone associated with the desired equipment slot
VisBaseEntity_cl* RPG_Character::AttachEquipmentPrefab(RPG_EquipmentSlot_e slot, const VString& prefabName)
{
  RPG_Attachment* attachment = m_equipmentAttachments.GetAt(slot);

  VisBaseEntity_cl* entity = RPG_GameManager::s_instance.CreateEntityFromPrefab(prefabName,  hkvVec3(0.0f, 0.0f, 0.0f),  hkvVec3(0.0f, 0.0f, 0.0f));

  if (entity)
  {
  attachment->Attach(entity, m_equipmentAttachmentBoneNames[slot], hkvVec3(0.0f, 0.0f, 0.0f));
  }
  return entity;
}

/// Attaches an equipment model to the bone associated with the desired equipment slot
VisBaseEntity_cl* RPG_Character::AttachEquipmentModel(RPG_EquipmentSlot_e slot, const VString& modelName)
{
  RPG_Attachment* attachment = m_equipmentAttachments.GetAt(slot);

  VisBaseEntity_cl* entity = RPG_GameManager::s_instance.CreateEntity("VisBaseEntity_cl", hkvVec3(0.0f, 0.0f, 0.0f));
  entity->SetMesh(modelName);
  if (entity)
  {
  attachment->Attach(entity, m_equipmentAttachmentBoneNames[slot], hkvVec3(0.0f, 0.0f, 0.0f));
  }
  return entity;
}

/// Detaches anything attached to the bone associated with the desired equipment slot
void RPG_Character::DetachEquipment(RPG_EquipmentSlot_e slot, bool disposeObject /*= true*/)
{
  RPG_Attachment* attachment = m_equipmentAttachments.GetAt(slot);

  attachment->Detach(disposeObject);
}

void RPG_Character::DetachEquipmentOnDeath()
{
  for(int i = 0; i < m_equipmentAttachments.Count(); ++i)
  {
    RPG_InventoryItem* inventoryItem = m_inventoryHandler.GetEquippedItem(static_cast<RPG_EquipmentSlot_e>(i));
    if (inventoryItem)
    {
      if (inventoryItem->GetDetachOnDeath())
      {
        DetachEquipment(static_cast<RPG_EquipmentSlot_e>(i));
        inventoryItem->SetEquippedEntity(NULL);
      }
    }
  }
}

void RPG_Character::CreateCharacterEffect(RPG_CharacterEffect_e const& effectType)
{
  // create an entry for this instantiated effect, if needed
  if (!m_characterEffects[effectType])
  {
    m_characterEffects[effectType] = static_cast<RPG_Effect*>(RPG_GameManager::s_instance.CreateEntity("RPG_Effect", GetPosition()));
  }
  VASSERT(m_characterEffects[effectType]);

  // whether this effect has been newly created, or is being recycled, its position and orientation need to be aligned with the character.
  m_characterEffects[effectType]->SetPosition(GetPosition());
  m_characterEffects[effectType]->SetOrientation(GetOrientation());

  // create this effect's visual and audio components
  if (m_characterEffectDefinitions[effectType].m_attachEffect)
  {
    m_characterEffects[effectType]->Create(m_characterEffectDefinitions[effectType], this); // attached effect
  }
  else
  {
    // if we're not attached, but we've been given a bone name, spawn unattached at that bone's current location.
    int boneIndex = -1;
    if (!m_characterEffectDefinitions[effectType].m_vfxBoneName.IsEmpty())
    {
      boneIndex = GetMesh()->GetSkeleton()->GetBoneIndexByName(m_characterEffectDefinitions[effectType].m_vfxBoneName.AsChar());
      if(boneIndex == -1)
      {
        Vision::Error.Warning("RPG_Character::CreateCharacterEffect - Supplied bone name doesn't exist on this skeleton: %s", m_characterEffectDefinitions[effectType].m_vfxBoneName.AsChar());
      }
    }

    if(boneIndex != -1)
    {
      // find the projectile launch bone's position in worldspace
      hkvVec3 boneWorldSpaceTranslation;
      hkvQuat boneWorldSpaceRotation;
      VVERIFY(GetBoneCurrentWorldSpaceTransformation(boneIndex, boneWorldSpaceTranslation, boneWorldSpaceRotation));
      hkvVec3 boneWorldSpaceOrientation;
      boneWorldSpaceRotation.getAsEulerAngles(boneWorldSpaceOrientation.z, boneWorldSpaceOrientation.y, boneWorldSpaceOrientation.x); // zyx. Because fancy, that's why.
      m_characterEffects[effectType]->Create(m_characterEffectDefinitions[effectType], boneWorldSpaceTranslation, boneWorldSpaceOrientation);  // unattached effect at this character's location
  }
    else
    {
      m_characterEffects[effectType]->Create(m_characterEffectDefinitions[effectType], GetPosition(), GetOrientation());  // unattached effect at this character's location
    }
  }
}

VisParticleEffect_cl* RPG_Character::GetPersistentCharacterEffect(RPG_CharacterEffect_e const& effectType) const
{
  if (m_characterEffects[effectType])
  {
    return m_characterEffects[effectType]->GetPersistentVisualEffect();
  }
  return NULL;
}

void RPG_Character::PauseCharacterEffect(RPG_CharacterEffect_e const& effectType)
{
  if (m_characterEffects[effectType])
  {
    m_characterEffects[effectType]->Pause();
  }
}

void RPG_Character::FinishCharacterEffect(RPG_CharacterEffect_e const& effectType)
{
  if (m_characterEffects[effectType])
  {
    m_characterEffects[effectType]->Finish();

    // clear the effect slot reference
    m_characterEffects[effectType] = NULL;
  }
}

void RPG_Character::StopAllPersistentCharacterEffects()
{
  for (int i = 0; i < FX_Count; ++i)
  {
    RPG_CharacterEffect_e const effectType = static_cast<RPG_CharacterEffect_e>(i);
    FinishCharacterEffect(effectType);
  }
}

void RPG_Character::ActivateCharacterEffectDebugDisplay()
{
  for (int i = 0; i < FX_Count; ++i)
  {
    RPG_CharacterEffect_e const effectType = static_cast<RPG_CharacterEffect_e>(i);
    if (!m_characterEffectDefinitions[effectType].m_vfxFilename.IsEmpty())
    {
      m_characterEffectDefinitions[effectType].m_displayDebugInfo = true;
    }
  }
}

void RPG_Character::UpdateDeathState(float const deltaTime)
{
  if (Vision::GetTimer()->GetTime() - m_timeOfDeath >= m_deathDissolveDelay)
  {
    hkvVec3 effectPosition = GetPosition();
    
    int const boneIndex = hkvMath::Min(1, GetMesh()->GetSkeleton()->GetBoneCount() - 1); // Attempt to grab the character's root (bone 1). Otherwise, grab the origin (bone 0).
    if(GetMesh()->GetSkeleton()->GetBone(boneIndex))
    {
      // find the location of the character's root bone in worldspace
      hkvVec3 boneWorldSpaceTranslation;
      hkvQuat boneWorldSpaceRotation;
      if (GetBoneCurrentWorldSpaceTransformation(boneIndex, boneWorldSpaceTranslation, boneWorldSpaceRotation))
      {
        effectPosition = boneWorldSpaceTranslation;
      }
    }

    // spawn the death dissolve effect
    RPG_GameManager::s_instance.CreateEffect(m_characterEffectDefinitions[FX_DeathDissolve], effectPosition);

    // clear me out.
    DisposeObject();
  }
}

bool RPG_Character::IsOnNavMesh(float const testRadius /*= 1.f*/) const
{
  return RPG_ControllerUtil::IsPointOnNavmesh(GetPosition(), testRadius);
}

void RPG_Character::Teleport(hkvVec3 const& position)
{
  m_characterController->SetPosition(position);
  SetPosition(position);
  UpdateBinding();
}

RPG_EffectDefinition const& RPG_Character::GetCharacterEffectDefinition(RPG_CharacterEffect_e const& effectType) const
{
  return m_characterEffectDefinitions[effectType];
}

VString const& RPG_Character::GetProjectileMeshFilename() const
{
  return m_projectileMeshFilename;
}

void RPG_Character::SetTeam(RPG_Team_e const& team)
{
  m_team = team;
}

RPG_Team_e RPG_Character::GetTeam() const
{
  return m_team;
}

void RPG_Character::SetWeaponTrailEnabledForEquippedWeapon(bool const enabled)
{
  const RPG_InventoryItem* equippedWeapon = GetInventoryHandler().GetEquippedWeapon();
  if (equippedWeapon)
  {
    VisBaseEntity_cl* eqippedWeaponEntity = equippedWeapon->GetEquippedEntity();
    if (eqippedWeaponEntity)
    {
      RPG_MeshTrailEffectComponent* meshTrailComponent = static_cast<RPG_MeshTrailEffectComponent*>(eqippedWeaponEntity->Components().GetComponentOfType(V_RUNTIME_CLASS(RPG_MeshTrailEffectComponent)));
      if (meshTrailComponent)
      {
        meshTrailComponent->SetEnabled(enabled);
      }
    }
  }
}

float RPG_Character::GetEquippedWeaponRange() const
{
  const RPG_InventoryItem* equippedWeapon = GetInventoryHandler().GetEquippedWeapon();
  float equippedWeaponRange = 0.0f;

  if (equippedWeapon)
  {
    equippedWeaponRange = equippedWeapon->GetRange();
  }
  else
  {
    Vision::Error.Warning("Character is missing an equipped weapon. Using default range.");
    equippedWeaponRange = 150.f;  // Fall back to a magic number and warn the client that we're doing this.
  }
  return equippedWeaponRange;
}

void RPG_Character::SetUpDefaultEquipment()
{
  for(int index = 0; index < ES_Count; ++index)
  {
    // check for a bone name for this slot
    if(!m_equipmentAttachmentBoneNames[index].IsEmpty())
    {
      VisBaseEntity_cl* baseEntity = NULL;

      if(!m_defaultEquipment[index].IsEmpty())
      {
        // create the attachment from a prefab if we have one
        baseEntity = RPG_GameManager::s_instance.CreateEntityFromPrefab(m_defaultEquipment[index], hkvVec3(0.0f, 0.0f, 0.0f), hkvVec3(0.0f, 0.0f, 0.0f));
      }
      else if(!m_defaultEquipmentScripts[index].IsEmpty())
      {
        // if we have no prefab, create a script entity if we have one
        baseEntity = RPG_GameManager::s_instance.CreateEntityFromScript(m_defaultEquipmentScripts[index], hkvVec3(0.0f, 0.0f, 0.0f), hkvVec3(0.0f, 0.0f, 0.0f));
      }

      // attach the entity if it's an inventory pickup
      if(baseEntity &&
        baseEntity->IsFrom(RPG_InventoryPickup))
      {
        RPG_InventoryPickup* defaultWeaponPickup = static_cast<RPG_InventoryPickup*>(baseEntity);

        defaultWeaponPickup->GivePickupItem(this);
        defaultWeaponPickup->DisposeObject();
        GetInventoryHandler().EquipItem(static_cast<RPG_EquipmentSlot_e>(index), GetInventoryHandler().GetInventoryCollection().Count() - 1); // bails gracefully if supplied item is already equipped.
        SetWeaponTrailEnabledForEquippedWeapon(false);  // begin with weapon trails disabled
      }
    }
  }
}

void RPG_Character::SetVisible(bool visible)
{
  unsigned int visibleBitmask;
  if (visible)
  {
    visibleBitmask = VIS_ENTITY_VISIBLE;
  }
  else
  {
    visibleBitmask = VIS_ENTITY_INVISIBLE;
  }

  // set the character's visibility
  SetVisibleBitmask(visibleBitmask);

  // set inventory visibility
  for(int i = 0; i < m_equipmentAttachments.Count(); ++i)
  {
    RPG_InventoryItem* inventoryItem = m_inventoryHandler.GetEquippedItem(static_cast<RPG_EquipmentSlot_e>(i));
    if (inventoryItem)
    {
      VisBaseEntity_cl* equippedEntity = inventoryItem->GetEquippedEntity();
      if (equippedEntity)
      {
        equippedEntity->SetVisibleBitmask(visibleBitmask);
      }
    }
  }
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
