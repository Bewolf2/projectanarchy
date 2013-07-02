/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_EXPLOSION_H__
#define RPG_PLUGIN_EXPLOSION_H__

#include <Physics2012/Utilities/Actions/Wind/hkpWind.h>
#include <Physics2012/Utilities/Actions/Wind/hkpWindRegion.h>

#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

enum RPG_ExplosionType_e
{
  ET_None=-1,
  ET_Sphere=0,
  ET_HerringBone,
  ET_Count
};

class RPG_Explosion
{
public:
  RPG_Explosion();
  RPG_Explosion(hkvVec3 const& center, RPG_ExplosionType_e type, float const strength, float const radius, float const length, hkvVec3 const& direction);
  virtual ~RPG_Explosion();

  void Initialize();
  void Apply();
  void Deinitialize();

private:
  hkVector4 GetImpulse(hkVector4 const& positionHk);

  hkvVec3 GetSphereImpulse(hkvVec3 const& position);
  hkvVec3 GetHerringBoneImpulse(hkvVec3 const& position);

  void CreateAabb(hkAabb &aabb);

  RPG_ExplosionType_e m_type;
  /// The strength of the explosion impulse.
  float m_impulseStrength;
  /// The radius of the explosion.
  float m_radius;
  /// The length of the explosion volume.
  float m_length; // used for ET_HerringBone only
  /// The direction of the explosion volume.
  hkvVec3 m_direction; // used for ET_HerringBone only
  /// The center of the explosion.
  hkvVec3 m_center;

  hkpAabbPhantom* m_phantom;

};

class RPG_ExplosionManager : public hkpWorldPostSimulationListener
{
public:
  RPG_ExplosionManager();
  virtual ~RPG_ExplosionManager();

  void CreateExplosion(hkvVec3 const& center, RPG_ExplosionType_e type = ET_Sphere, float const strength = 5000.0f, float const radius = 1000.0f, float const length = 1000.0f, hkvVec3 const& direction = hkvVec3(0.0f, 1.0f, 0.0f));

  virtual void postSimulationCallback(hkpWorld* world);

private:
  VArray<RPG_Explosion> m_explosions;

public:
  static RPG_ExplosionManager s_instance;
};

#endif //RPG_PLUGIN_EXPLOSION_H__

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
