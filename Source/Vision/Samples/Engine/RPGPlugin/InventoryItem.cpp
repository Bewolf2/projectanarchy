/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/InventoryItem.h>

RPG_InventoryItem::RPG_InventoryItem(const VString& itemName)
  : VRefCounter()
  , m_itemName(itemName)
  , m_minDamage(0.8f)
  , m_maxDamage(1.2f)
  , m_speed(1.0f)
  , m_range(1.1f)
  , m_impactSpeed(2500.0f)
  , m_knockBackStrength(50.0f)
  , m_knockBackDuration(0.15f)
  , m_armorValue(0)
  , m_isShieldType(false)
  , m_equippedPrefabFileName()
  , m_equippedModelFileName()
  , m_equipmentSlot(ES_None)
  , m_equippedEntity(NULL)
  , m_detachOnDeath(false)
{
}

void RPG_InventoryItem::SetName(const VString& name)
{
  m_itemName = name;
}

const VString& RPG_InventoryItem::GetName() const
{
  return m_itemName;
}

int RPG_InventoryItem::GetArmorValue() const
{
  return m_armorValue;
}

void RPG_InventoryItem::SetArmorValue(int const armorValue)
{
  m_armorValue = armorValue;
}

bool RPG_InventoryItem::IsShieldType() const
{
  return m_isShieldType;
}

float RPG_InventoryItem::GetMinDamage() const
{
  return m_minDamage;
}

void RPG_InventoryItem::SetMinDamage(float const minDamage)
{
  m_minDamage = minDamage;
}

float RPG_InventoryItem::GetMaxDamage() const
{
  return m_maxDamage;
}

void RPG_InventoryItem::SetMaxDamage(float const maxDamage)
{
  m_maxDamage = maxDamage;
}

float RPG_InventoryItem::GetSpeed() const
{
  return m_speed;
}

void RPG_InventoryItem::SetSpeed(float const speed)
{
  m_speed = speed;
}

float RPG_InventoryItem::GetRange() const
{
  return m_range;
}

void RPG_InventoryItem::SetRange(float const range)
{
  m_range = range;
}

float RPG_InventoryItem::GetImpactSpeed() const
{
  return m_impactSpeed;
}

void RPG_InventoryItem::SetImpactSpeed(float const speed)
{
  m_impactSpeed = speed;
}

float RPG_InventoryItem::GetKnockBackStrength() const
{
  return m_knockBackStrength;
}

void RPG_InventoryItem::SetKnockBackStrength( float const strength )
{
  m_knockBackStrength = strength;
}

float RPG_InventoryItem::GetKnockBackDuration() const
{
  return m_knockBackDuration;
}

void RPG_InventoryItem::SetKnockBackDuration( float const duration )
{
  m_knockBackDuration = duration;
}

const VString& RPG_InventoryItem::GetEquippedPrefabFileName() const
{
  return m_equippedPrefabFileName;
}

void RPG_InventoryItem::SetEquippedPrefabFileName(VString const& fileName)
{
  m_equippedPrefabFileName = fileName;
}

const VString& RPG_InventoryItem::GetEquippedModelFileName() const
{
  return m_equippedModelFileName;
}

void RPG_InventoryItem::SetEquippedModelFileName(VString const& fileName)
{
  m_equippedModelFileName = fileName;
}

RPG_EquipmentSlot_e RPG_InventoryItem::GetEquipmentSlot() const
{
  return m_equipmentSlot;
}

void RPG_InventoryItem::SetEquipmentSlot(RPG_EquipmentSlot_e slot)
{
  m_equipmentSlot = slot;
}

VisBaseEntity_cl* RPG_InventoryItem::GetEquippedEntity() const
{
  return m_equippedEntity;
}

void RPG_InventoryItem::SetEquippedEntity(VisBaseEntity_cl* val)
{
  m_equippedEntity = val;
}

bool RPG_InventoryItem::GetDetachOnDeath() const
{
  return m_detachOnDeath;
}

void RPG_InventoryItem::SetDetachOnDeath(bool detachOnDeath)
{
  m_detachOnDeath = detachOnDeath;
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
