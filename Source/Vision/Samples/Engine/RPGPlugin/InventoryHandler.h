/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_INVENTORY_HANDLER_H__
#define RPG_PLUGIN_INVENTORY_HANDLER_H__

class RPG_Character;
class RPG_InventoryItem;

enum RPG_EquipmentSlot_e
{
  ES_None=-1,
  ES_Helmet=0,
  ES_Chest,
  ES_Gloves,
  ES_Shoulder,
  ES_Belt,
  ES_Boots,
  ES_Weapon,
  ES_Count
};

class RPG_InventoryHandler
{
public:

  RPG_InventoryHandler();

  void Initialize(RPG_Character* characterOwner);
  void Tick(const float deltaTime);

  int AddInventory(RPG_InventoryItem* inventoryItem);
  void RemoveFromInventory(RPG_InventoryItem* inventoryItem);
  void DiscardInventory();

  void EquipArmor(RPG_EquipmentSlot_e armorSlot, int inventorySlot);
  void EquipWeapon(int inventorySlot);
  void EquipItem(RPG_EquipmentSlot_e equipmentSlot, int inventorySlot);

  const VRefCountedCollection<RPG_InventoryItem>& GetInventoryCollection() const;

  RPG_InventoryItem* GetEquippedWeapon() const;
  RPG_InventoryItem* GetEquippedArmor(RPG_EquipmentSlot_e armorSlot) const;
  RPG_InventoryItem* GetEquippedItem(RPG_EquipmentSlot_e itemSlot) const;

  void DebugDisplayInventory();

  //@todo: Temporary placeholder until the legitimate inventory handler goes in.
  static void AddDefaultInventory(RPG_Character& character);

protected:
  RPG_Character* m_characterOwner;

  VRefCountedCollection<RPG_InventoryItem> m_inventoryCollection;             ///< Collection of inventory items carried by this handler's character owner

  // Equipped weapons and armor map to slots in the character's inventory
  int m_equippedItems[ES_Count];
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
