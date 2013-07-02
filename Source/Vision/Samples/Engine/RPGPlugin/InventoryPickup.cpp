/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/InventoryPickup.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryItem.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>

V_IMPLEMENT_SERIAL(RPG_InventoryPickup, RPG_Pickup, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_InventoryPickup, RPG_Pickup, "An inventory pickup item. Gives inventory to a character when used.", 0, "");
DEFINE_VAR_VSTRING(RPG_InventoryPickup, m_equippedPrefabFileName, "Prefab for the equipped item", "", 0, 0, "filepicker(.vPrefab)");
DEFINE_VAR_VSTRING(RPG_InventoryPickup, m_equippedModelFileName, "Model for the equipped item", "", 0, 0, "filepicker(.model)");
DEFINE_VAR_ENUM_AND_NAME(RPG_InventoryPickup, m_equipmentSlot, "Equipment Slot", "Slot this item can be equipped in", "None", "None/Helmet/Chest/Gloves/Shoulder/Belt/Boots/Weapon", 0, 0);
DEFINE_VAR_BOOL_AND_NAME(RPG_InventoryPickup, m_detachOnDeath, "Detach on Death?", "Should this item detach when its owning character dies?", "FALSE", 0, 0);
DEFINE_VAR_INT_AND_NAME(RPG_InventoryPickup, m_armorValue, "Armor Value", "Amount of armor this item adds when equipped ", "0", 0, 0);
DEFINE_VAR_VSTRING_AND_NAME(RPG_InventoryPickup, m_itemName, "Item Name", "The name of this item.", "", 0, 0, 0);
DEFINE_VAR_BOOL_AND_NAME(RPG_InventoryPickup, m_isShieldType, "Is Shield?", "Is this a Shield?", "FALSE", 0, 0);
DEFINE_VAR_FLOAT_AND_NAME(RPG_InventoryPickup, m_minDamage, "Min Damage", "Min Damage this item can do", "0.8", 0, 0);
DEFINE_VAR_FLOAT_AND_NAME(RPG_InventoryPickup, m_maxDamage, "Max Damage", "Max Damage this item can do", "1.2", 0, 0);
DEFINE_VAR_FLOAT_AND_NAME(RPG_InventoryPickup, m_speed, "Attack Speed", "Attack speed of this item", "1.0", 0, 0);
DEFINE_VAR_FLOAT_AND_NAME(RPG_InventoryPickup, m_range, "Attack Range", "Attack range of this item", "1.1", 0, 0);
DEFINE_VAR_FLOAT_AND_NAME(RPG_InventoryPickup, m_impactSpeed, "Impact Speed", "Speed of ragdoll after this weapon deals a killing blow", "2500", 0, 0);
DEFINE_VAR_FLOAT_AND_NAME(RPG_InventoryPickup, m_knockBackStrength, "Knock Back Strength", "Knock back strength of this item", "50.0", 0, 0);
DEFINE_VAR_FLOAT_AND_NAME(RPG_InventoryPickup, m_knockBackDuration, "Knock Back Duration", "Knock back duration of this item", "0.15", 0, 0);
END_VAR_TABLE


  RPG_InventoryPickup::RPG_InventoryPickup() 
  : RPG_Pickup()
  , m_pickupItem(NULL)
  , m_equippedPrefabFileName()
  , m_equippedModelFileName()
  , m_equipmentSlot(0)
  , m_detachOnDeath(FALSE)
  , m_itemName("")
  , m_armorValue(0)
  , m_isShieldType(FALSE)
  , m_minDamage(0.8f)
  , m_maxDamage(1.2f)
  , m_speed(1.0f)
  , m_range(1.1f)
  , m_impactSpeed(2500.0f)
  , m_knockBackStrength(50.0f)
  , m_knockBackDuration(0.15f)
{
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_InventoryPickup::GetDependencies(VResourceSnapshot &snapshot)
{
  RPG_Pickup::GetDependencies(snapshot);

  if(!m_equippedPrefabFileName.IsEmpty())
  {
    RPG_DependencyHelper::AddPrefabDependency(m_equippedPrefabFileName, snapshot);
  }

  if(!m_equippedModelFileName.IsEmpty())
  {
    RPG_DependencyHelper::AddModelDependency(m_equippedModelFileName, snapshot);
  }
}
#endif

void RPG_InventoryPickup::Initialize()
{
  RPG_Pickup::Initialize();

  // @todo: temp for testing
  m_pickupItem = new RPG_InventoryItem("");
}

void RPG_InventoryPickup::ThinkFunction()
{
  RPG_Pickup::ThinkFunction();
}

void RPG_InventoryPickup::SetPickupItem(RPG_InventoryItem* pickupItem)
{
  m_pickupItem = pickupItem;
}

RPG_InventoryItemPtr RPG_InventoryPickup::GetPickupItem() const
{
  return m_pickupItem;
}

void RPG_InventoryPickup::GivePickupItem(RPG_Character* receivingCharacter)
{
  // set the pickup item data
  m_pickupItem->SetEquippedPrefabFileName(m_equippedPrefabFileName);
  m_pickupItem->SetEquippedModelFileName(m_equippedModelFileName);
  m_pickupItem->SetName(m_itemName);
  m_pickupItem->SetDetachOnDeath(m_detachOnDeath);
  m_pickupItem->SetArmorValue(m_armorValue);
  m_pickupItem->SetMinDamage(m_minDamage);
  m_pickupItem->SetMaxDamage(m_maxDamage);
  m_pickupItem->SetSpeed(m_speed);
  m_pickupItem->SetRange(m_range);
  m_pickupItem->SetImpactSpeed(m_impactSpeed);
  m_pickupItem->SetKnockBackStrength(m_knockBackStrength);
  m_pickupItem->SetKnockBackDuration(m_knockBackDuration);

  // subtract 1 from equipment slot because of the way DEFINE_VAR_ENUM works.
  RPG_EquipmentSlot_e equipmentSlot = static_cast<RPG_EquipmentSlot_e>(m_equipmentSlot - 1);
  m_pickupItem->SetEquipmentSlot(equipmentSlot);

  receivingCharacter->GetInventoryHandler().AddInventory(m_pickupItem);
}

void RPG_InventoryPickup::OnPickup(RPG_Character* character)
{
  if (m_pickupItem)
  {
    GivePickupItem(character);
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
