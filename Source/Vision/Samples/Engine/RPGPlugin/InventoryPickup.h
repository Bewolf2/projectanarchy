/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_INVENTORY_PICKUP_H__
#define RPG_PLUGIN_INVENTORY_PICKUP_H__

#include <Vision/Samples/Engine/RPGPlugin/InventoryHandler.h>
#include <Vision/Samples/Engine/RPGPlugin/Pickup.h>

class RPG_Character;
class RPG_InventoryItem;
typedef VSmartPtr<RPG_InventoryItem> RPG_InventoryItemPtr;

class RPG_InventoryPickup : public RPG_Pickup
{
public:
  RPG_PLUGIN_IMPEXP RPG_InventoryPickup();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  void Initialize() HKV_OVERRIDE;
  void ThinkFunction() HKV_OVERRIDE;

  void SetPickupItem(RPG_InventoryItem* pickupItem);
  RPG_InventoryItemPtr GetPickupItem() const;
  void GivePickupItem(RPG_Character* receivingCharacter);

  virtual void OnPickup(RPG_Character* character);

protected:

  RPG_InventoryItemPtr m_pickupItem;
  VString m_equippedPrefabFileName;
  VString m_equippedModelFileName;
  int m_equipmentSlot;        ///< note: because of how the vision enum serialization works, this has a value of 1 greater than the actual value. ES_None should be -1, but will be 0 in this variable.
  BOOL m_detachOnDeath;
  VString m_itemName;

  //@{
  // Armor
  int m_armorValue;
  BOOL m_isShieldType;
  //@}

  //@{
  // Weapon
  float m_minDamage;
  float m_maxDamage;
  float m_speed;
  float m_range;
  float m_impactSpeed;
  float m_knockBackStrength;
  float m_knockBackDuration;
  //@}


private:
  V_DECLARE_SERIAL_DLLEXP(RPG_InventoryPickup, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_InventoryPickup, RPG_PLUGIN_IMPEXP);
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
