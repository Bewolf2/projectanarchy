/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/InventoryHandler.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryItem.h>

RPG_InventoryHandler::RPG_InventoryHandler()
  : m_characterOwner(NULL)
  , m_inventoryCollection()
{
  for(int index = 0; index < ES_Count; ++index)
  {
    m_equippedItems[index] = ES_None;
  }
}

void RPG_InventoryHandler::Initialize(RPG_Character* characterOwner)
{
  m_characterOwner = characterOwner;
}

void RPG_InventoryHandler::Tick(const float deltaTime)
{
  VASSERT(m_characterOwner);  // never ok for this handler not to have a valid owner.
}

int RPG_InventoryHandler::AddInventory(RPG_InventoryItem* inventoryItem)
{
  return m_inventoryCollection.AddUnique(inventoryItem);
}

void RPG_InventoryHandler::RemoveFromInventory(RPG_InventoryItem* inventoryItem)
{
  m_inventoryCollection.SafeRemove(inventoryItem);
}

void RPG_InventoryHandler::DiscardInventory()
{
  m_inventoryCollection.Clear();
}

void RPG_InventoryHandler::EquipArmor(RPG_EquipmentSlot_e armorSlot, int inventorySlot)
{
  EquipItem(armorSlot, inventorySlot);
}

void RPG_InventoryHandler::EquipWeapon(int inventorySlot)
{
  EquipItem(ES_Weapon, inventorySlot);
}

void RPG_InventoryHandler::EquipItem(RPG_EquipmentSlot_e equipmentSlot, int inventorySlot)
{
  // bail if the item is already equipped
  if(inventorySlot == m_equippedItems[equipmentSlot])
  {
    return;
  }

  RPG_InventoryItem* item = m_inventoryCollection.GetAt(inventorySlot);

  if(item &&
    item->GetEquipmentSlot() == equipmentSlot)
  {
    // detach the old item
    m_characterOwner->DetachEquipment(equipmentSlot);
    item->SetEquippedEntity(NULL);

    // attach the new one
    m_equippedItems[equipmentSlot] = inventorySlot;

    if(!item->GetEquippedPrefabFileName().IsEmpty())
    {
      item->SetEquippedEntity(m_characterOwner->AttachEquipmentPrefab(equipmentSlot, item->GetEquippedPrefabFileName()));
    }
    else if(!item->GetEquippedModelFileName().IsEmpty())
    {
      item->SetEquippedEntity(m_characterOwner->AttachEquipmentModel(equipmentSlot, item->GetEquippedModelFileName()));
    }
  }
}

RPG_InventoryItem* RPG_InventoryHandler::GetEquippedWeapon() const
{
  return GetEquippedItem(ES_Weapon);
}

RPG_InventoryItem* RPG_InventoryHandler::GetEquippedArmor(RPG_EquipmentSlot_e armorSlot) const
{
  return GetEquippedItem(armorSlot);
}

RPG_InventoryItem* RPG_InventoryHandler::GetEquippedItem(RPG_EquipmentSlot_e itemSlot) const
{
  if(m_equippedItems[itemSlot] > -1 &&
     m_equippedItems[itemSlot] < m_inventoryCollection.Count())
  {
    return m_inventoryCollection.GetAt(m_equippedItems[itemSlot]);
  }
  else
  {
    return NULL;
  }
}

const VRefCountedCollection<RPG_InventoryItem>& RPG_InventoryHandler::GetInventoryCollection() const
{
  return m_inventoryCollection;
}

void RPG_InventoryHandler::DebugDisplayInventory()
{
  const float verticalOffset = 350.f; //@todo: Get the character's height here & replace offset magic number
  VString msg, tempLine; //@todo: Is there a less crufty way to build these strings?

  // add inventory list
  for (int i = 0; i < m_inventoryCollection.Count (); ++i)
  {
    tempLine.Format("Item #%i - %s\n", i, m_inventoryCollection.GetAt(i)->GetName().AsChar());
    msg += tempLine;
  }

  tempLine.Format("Equipped:\n");
  msg += tempLine;

  // add equipped weapon info
  if (GetEquippedWeapon())
  {
    tempLine.Format("Equipped weapon: %s (Dmg: %.1f%%%% - %.1f%%%%, Spd: %.1f%%%%, Rng: %.1f)\n", 
      GetEquippedWeapon()->GetName().AsChar(), 
      GetEquippedWeapon()->GetMinDamage(), 
      GetEquippedWeapon()->GetMaxDamage(),
      GetEquippedWeapon()->GetSpeed(),
      GetEquippedWeapon()->GetRange()
      );
  }
  else
  {
    tempLine.Format("Equipped weapon: None\n");
  }
  msg += tempLine;

  // add equipped armor info
  tempLine.Format("Equipped armor: %s (%i), %s (%i), %s (%i),\n      %s (%i), %s (%i), %s (%i)\n", 
    GetEquippedArmor(ES_Helmet) ? GetEquippedArmor(ES_Helmet)->GetName().AsChar() : "<None>", 
    GetEquippedArmor(ES_Helmet) ? GetEquippedArmor(ES_Helmet)->GetArmorValue() : 0,

    GetEquippedArmor(ES_Chest) ? GetEquippedArmor(ES_Chest)->GetName().AsChar() : "<None>", 
    GetEquippedArmor(ES_Chest) ? GetEquippedArmor(ES_Chest)->GetArmorValue() : 0,

    GetEquippedArmor(ES_Gloves) ? GetEquippedArmor(ES_Gloves)->GetName().AsChar() : "<None>", 
    GetEquippedArmor(ES_Gloves) ? GetEquippedArmor(ES_Gloves)->GetArmorValue() : 0,

    GetEquippedArmor(ES_Shoulder) ? GetEquippedArmor(ES_Shoulder)->GetName().AsChar() : "<None>",
    GetEquippedArmor(ES_Shoulder) ? GetEquippedArmor(ES_Shoulder)->GetArmorValue() : 0,

    GetEquippedArmor(ES_Belt) ? GetEquippedArmor(ES_Belt)->GetName().AsChar() : "<None>", 
    GetEquippedArmor(ES_Belt) ? GetEquippedArmor(ES_Belt)->GetArmorValue() : 0,

    GetEquippedArmor(ES_Boots) ? GetEquippedArmor(ES_Boots)->GetName().AsChar() : "<None>",
    GetEquippedArmor(ES_Boots) ? GetEquippedArmor(ES_Boots)->GetArmorValue() : 0
    );
  msg += tempLine;

  // display the message
  const hkvVec3 vPos = m_characterOwner->GetPosition() + hkvVec3(0.0f, 0.0f, verticalOffset);
  Vision::Message.DrawMessage3D(msg, vPos);
}

//@todo: Temporary placeholder until the legitimate inventory handler goes in.
void RPG_InventoryHandler::AddDefaultInventory(RPG_Character& character)
{
  // @todo: temporary for testing inventory handler. We'll want to create legitimate inventory item classes and specifiers.
  RPG_InventoryHandler& inventoryHandler = character.GetInventoryHandler();
  {
    RPG_InventoryItem* newItem = new RPG_InventoryItem("TestAxe");
    {
      newItem->SetMinDamage(0.8f);
      newItem->SetMaxDamage(1.2f);
      newItem->SetSpeed(1.0f);
      newItem->SetRange(150.0f);
      newItem->SetEquipmentSlot(ES_Weapon);
      //newItem->SetEquippedPrefabFileName("Prefabs\\Editor\\Axis.vprefab");
    }

    int const itemIndex = inventoryHandler.AddInventory(newItem);
    inventoryHandler.EquipWeapon(itemIndex);
  }

  {
    RPG_InventoryItem *newItem = new RPG_InventoryItem("TestHelmet");
    {
      newItem->SetArmorValue(2);
    }

    int const itemIndex = inventoryHandler.AddInventory(newItem);
    inventoryHandler.EquipArmor(ES_Helmet, itemIndex);
  }

  {
    RPG_InventoryItem *newItem = new RPG_InventoryItem("TestChestPiece");
    {
      newItem->SetArmorValue(3);
    }

    int const itemIndex = inventoryHandler.AddInventory(newItem);
    inventoryHandler.EquipArmor(ES_Chest, itemIndex);
  }

  {
    RPG_InventoryItem *newItem = new RPG_InventoryItem("TestGloves");
    {
      newItem->SetArmorValue(2);
    }

    int const itemIndex = inventoryHandler.AddInventory(newItem);
    inventoryHandler.EquipArmor(ES_Gloves, itemIndex);
  }

  {
    RPG_InventoryItem *newItem = new RPG_InventoryItem("TestShoulders");
    {
      newItem->SetArmorValue(2);
    }

    int const itemIndex = inventoryHandler.AddInventory(newItem);
    inventoryHandler.EquipArmor(ES_Shoulder, itemIndex);
  }

  {
    RPG_InventoryItem *newItem = new RPG_InventoryItem("TestBelt");
    {
      newItem->SetArmorValue(1);
    }

    int const itemIndex = inventoryHandler.AddInventory(newItem);
    inventoryHandler.EquipArmor(ES_Belt, itemIndex);
  }

  {
    RPG_InventoryItem *newItem = new RPG_InventoryItem("TestBoots");
    {
      newItem->SetArmorValue(2);
    }

    int const itemIndex = inventoryHandler.AddInventory(newItem);
    inventoryHandler.EquipArmor(ES_Boots, itemIndex);
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
