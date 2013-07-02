/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_DESTRUCTIBLEENTITY_H__
#define RPG_PLUGIN_DESTRUCTIBLEENTITY_H__

#include <Vision/Samples/Engine/RPGPlugin/DamageableEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryHandler.h>
#include <Vision/Samples/Engine/RPGPlugin/Effect.h>

enum RPG_DestructibleEntityEffect_e
{
  DEFX_None=-1,
  DEFX_Ambient=0,
  DEFX_TakeHit,
  DEFX_Destroy,
  DEFX_Count
};

class RPG_DestructibleEntity : public RPG_DamageableEntity
{
public:
  RPG_PLUGIN_IMPEXP RPG_DestructibleEntity();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  void PostInitialize() HKV_OVERRIDE;

  void DisposeObject() HKV_OVERRIDE;

  void ThinkFunction() HKV_OVERRIDE;

  virtual int TakeDamage(const int damageAmount, const hkvVec3& impactVelocity = hkvVec3(0,0,0)) HKV_OVERRIDE;

protected:
  void AddComponents();
  void SetDestroyed();

  // AI
  void AddObstacle();
  void RemoveObstacle();

  //@{
  // FX
  void SetEffectDefaults();
  void CreateEffect(RPG_DestructibleEntityEffect_e effectType);
  void CreateEffect(RPG_DestructibleEntityEffect_e effectType, hkvVec3 const& position, hkvVec3 const& orientation);
  void StopEffect(RPG_DestructibleEntityEffect_e effectType);
  RPG_EffectDefinition m_effectDefinitions[DEFX_Count];
  RPG_Effect* m_effects[DEFX_Count];
  //@}

  // properties
  VString m_postDestructionMeshFilename;
  int m_healthMax;
  BOOL m_removeCollisionAfterDestruction;

  // state
  bool m_isDestroyed;
  int m_health;

  // AI
  hkRefPtr<hkaiCharacter> m_aiCharacter;
  hkRefPtr<hkaiBehavior> m_aiBehavior;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_DestructibleEntity, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_DestructibleEntity, RPG_PLUGIN_IMPEXP);
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
