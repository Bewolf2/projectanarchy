/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_PROJECTILE_H__
#define RPG_PLUGIN_PROJECTILE_H__

#include <Vision/Samples/Engine/RPGPlugin/BaseEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/Effect.h>

class vHavokRigidBody;
class RPG_Character;
class RPG_DamageableEntity;

enum RPG_ProjectileEffect_e
{
  PFX_None=-1,
  PFX_Inflight=0,
  PFX_Impact,
  PFX_Count
};

/// Basic projectile class. Moves kinematically, uses a Havok shape phantom for CCD, and can deal damage upon impact.
class RPG_Projectile : public RPG_BaseEntity
{
public:
  RPG_PLUGIN_IMPEXP RPG_Projectile();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  void ThinkFunction() HKV_OVERRIDE;
  void DisposeObject() HKV_OVERRIDE;

  void SetCharacterOwner(RPG_Character* character);
  void Fire(hkvVec3 position, hkvVec3 direction, float speed);
  void Fire(hkvVec3 position, VisBaseEntity_cl* target, bool trackTarget, float speed);

  void SetProjectileMesh(VString const& meshFilename);
  void SetProjectileEffect(RPG_ProjectileEffect_e effectType, RPG_EffectDefinition const& effectDefinition);

  bool ProcessCollision(VisTypedEngineObject_cl* target, hkvVec3 const& position, hkvVec3 const& normal);

protected:
  //@{
  // User-determined projectile behavior
  virtual bool ShouldHit(RPG_DamageableEntity* target);
  virtual void OnHit(RPG_DamageableEntity* target, hkvVec3 const& position, hkvVec3 const& normal);
  virtual hkvVec3 Move();
  //@}

  void CreateShapePhantom();
  hkvVec3 SweepPhantom( hkvVec3 newPosition );

  void CreateEffect(RPG_ProjectileEffect_e effectType);
  void CreateEffect(RPG_ProjectileEffect_e effectType, hkvVec3 const& position, hkvVec3 const& orientation);

  static hkvVec3 NormalToYpr(hkvVec3 const &normal);

  RPG_Character* m_characterOwner;

  VString m_projectileMeshFilename;

  //@{
  // FX
  RPG_EffectDefinition m_effectDefinitions[PFX_Count];
  RPG_Effect* m_effects[PFX_Count];
  //@}

  //@{
  // basic velocity-driven movement
  hkvVec3 m_currentPosition;
  float m_speed;
  hkvVec3 m_direction;
  VisBaseEntity_cl* m_targetToTrack;
  //@}

  //@{
  // continuous collision detection
  hkpSimpleShapePhantom* m_shapePhantom;
  //@}

  //@{
  // auto-destruct
  float m_spawnTime;
  float m_maxLifeTime;
  bool m_shouldDestroy;
  //@}

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_Projectile, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_Projectile, RPG_PLUGIN_IMPEXP);
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
