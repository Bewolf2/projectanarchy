/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_PICKUP_H__
#define RPG_PLUGIN_PICKUP_H__

#include <Vision/Samples/Engine/RPGPlugin/BaseEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryHandler.h>
#include <Vision/Samples/Engine/RPGPlugin/Effect.h>

enum RPG_PickupEffect_e
{
  PKFX_None=-1,
  PKFX_Spawn=0,
  PKFX_Ambient,
  PKFX_Pickup,
  PKFX_Count
};

class RPG_Pickup : public RPG_BaseEntity
{
public:
  RPG_PLUGIN_IMPEXP RPG_Pickup();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  void Initialize() HKV_OVERRIDE;
  void PostInitialize() HKV_OVERRIDE;
  void ThinkFunction() HKV_OVERRIDE;

  virtual void OnPickup(RPG_Character* character) = 0;

protected:
  void UpdateRotation(float const deltaTime);
  void UpdateMagnetForces(float const deltaTime);
  void CheckCharacterContact();

  void CreateEffect(RPG_PickupEffect_e effectType);
  void CreateEffect(RPG_PickupEffect_e effectType, hkvVec3 const& position, hkvVec3 const& orientation);


  float m_pickupRadius;
  float m_rotateSpeed;  //rotation speed (degrees per second)

  //@{
  // FX
  RPG_EffectDefinition m_effectDefinitions[PKFX_Count];
  RPG_Effect* m_effects[PKFX_Count];

  VString m_pickupEffectFilename; ///< @todo: DEPRECATED
  //@}

  BOOL m_usePhysics;      ///< if enabled, a rigid body will be created for this pickup.

  VSmartPtr<vHavokRigidBody> m_rigidBodyComponent;
  float m_elapsedTime;

  float m_mass;
  float m_magnetMaxDistance;
  float m_magnetSpeedMultiplier;
  float m_spawnIdleTime;
  float m_verticalSpawnForceMin;
  float m_verticalSpawnForceMax;
  float m_horizontalSpawnForceMin;
  float m_horizontalSpawnForceMax;

  float m_magnetMaxDistanceInverseSquared;
  float m_currentMagnetSpeed;
  hkvVec3 m_currentMagnetVelocity;
  
private:
  V_DECLARE_SERIAL_DLLEXP(RPG_Pickup, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_Pickup, RPG_PLUGIN_IMPEXP);
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
