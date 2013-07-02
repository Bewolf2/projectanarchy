/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_INVENTORY_ITEM_H__
#define RPG_PLUGIN_INVENTORY_ITEM_H__

#include <Vision/Samples/Engine/RPGPlugin/InventoryHandler.h>

class RPG_InventoryItem : public VRefCounter
{
public:
  RPG_InventoryItem(const VString& itemName);

  void SetName(const VString& name);
  const VString& GetName() const;

  //@{
  // Armor
  int GetArmorValue() const;
  void SetArmorValue(int const armorValue);
  bool IsShieldType() const;
  //@}

  //@{
  // Weapon
  float GetMinDamage() const;
  void  SetMinDamage(float const minDamage);
  float GetMaxDamage() const;
  void  SetMaxDamage(float const maxDamage);
  float GetSpeed() const;
  void  SetSpeed(float const speed);
  float GetRange() const;
  void  SetRange(float const range);
  float GetImpactSpeed() const;
  void  SetImpactSpeed(float const speed);
  float GetKnockBackStrength() const;
  void  SetKnockBackStrength(float const strength);
  float GetKnockBackDuration() const;
  void  SetKnockBackDuration(float const duration);
  //@}

  //@{
  // Equipment
  const VString& GetEquippedPrefabFileName() const;
  void SetEquippedPrefabFileName(VString const& fileName);
  const VString& GetEquippedModelFileName() const;
  void SetEquippedModelFileName(VString const& fileName);
  RPG_EquipmentSlot_e GetEquipmentSlot() const;
  void SetEquipmentSlot(RPG_EquipmentSlot_e slot);
  VisBaseEntity_cl* GetEquippedEntity() const;
  void SetEquippedEntity(VisBaseEntity_cl* val);
  bool GetDetachOnDeath() const;
  void SetDetachOnDeath(bool detachOnDeath);
  //@}

protected:
  VString m_itemName;

  //@{
  // Armor
  int m_armorValue;
  bool m_isShieldType;
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

  //@{
  // Equipment
  VString m_equippedPrefabFileName;
  VString m_equippedModelFileName;
  RPG_EquipmentSlot_e m_equipmentSlot;
  VisBaseEntity_cl* m_equippedEntity;
  bool m_detachOnDeath;
  //@}
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
