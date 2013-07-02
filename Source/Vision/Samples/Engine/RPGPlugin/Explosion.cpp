/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Explosion.h>

#include <Physics2012/Utilities/Actions/Wind/hkpWindRegion.h>

// We use debug lines to visualize explosions.
#include <Common/Visualize/hkDebugDisplay.h>
#include <Common/Visualize/Shape/hkDisplayBox.h>

// ////////////////////////////////////////////////////////////////////////
// EXPLOSION
// ////////////////////////////////////////////////////////////////////////

RPG_Explosion::RPG_Explosion()
  : m_type(ET_Sphere)
  , m_impulseStrength(5000.0f)
  , m_radius(1000.0f)
  , m_length(1000.0f)
  , m_direction(0.0f, 1.0f, 0.0f)
{
}

RPG_Explosion::RPG_Explosion(hkvVec3 const& center, RPG_ExplosionType_e type, float const strength, float const radius, float const length, hkvVec3 const& direction)
  : m_type(type)
  , m_impulseStrength(strength)
  ,	m_radius(radius)
  , m_length(length)
  , m_direction(direction)
  , m_center(center)
  , m_phantom(NULL)
{
}

RPG_Explosion::~RPG_Explosion()
{
  hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();

  if(m_phantom)
  {
    world->markForWrite();

    world->removePhantom(m_phantom);

    m_phantom->removeReference();
    m_phantom = NULL;

    world->unmarkForWrite();
  }
}

void RPG_Explosion::Initialize()
{
  hkAabb aabb;
  CreateAabb(aabb);
  int filterInfo = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM0, 0, 0, 0);
  VASSERT_MSG(!m_phantom, "This RPG_Explosion has already been initialized.");
  if(!m_phantom)
  {
    m_phantom = new hkpAabbPhantom(aabb, filterInfo);

    hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
    world->markForWrite();

    world->addPhantom( m_phantom );

    world->unmarkForWrite();
  }
}

void RPG_Explosion::Apply()
{
  hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();

  // get collidables
  world->markForRead();

  m_phantom->ensureDeterministicOrder();
  hkArray<hkpCollidable*>& collidables = m_phantom->getOverlappingCollidables();

  world->unmarkForRead();

  // apply impulses
  world->markForWrite();

  const int numCollidables = collidables.getSize();
  for ( int i = 0; i < numCollidables; ++i )
  {
    hkpRigidBody* rb = hkpGetRigidBody( collidables[i] );

    // Ignore other phantoms and fixed rigid bodies.
    if ( (rb != HK_NULL) && !rb->isFixed() )
    {
      hkVector4 const& position = rb->getPosition();
      hkVector4 const impulse = GetImpulse(position);
      rb->applyLinearImpulse(impulse);
    }
  }

  world->unmarkForWrite();
}

void RPG_Explosion::Deinitialize()
{
  hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();

  if(m_phantom)
  {
    world->markForWrite();

    world->removePhantom(m_phantom);

    m_phantom->removeReference();
    m_phantom = NULL;

    world->unmarkForWrite();
  }
}

hkVector4 RPG_Explosion::GetImpulse( hkVector4 const& positionHk )
{
  hkvVec3 impulse(0.0f, 0.0f, 0.0f);

  hkvVec3 position;
  vHavokConversionUtils::PhysVecToVisVecLocal(positionHk, position);

  switch(m_type)
  {
  case ET_Sphere:
    {
      impulse = GetSphereImpulse(position);
    }
    break;

  case ET_HerringBone:
    {
      impulse = GetHerringBoneImpulse(position);
    }
    break;

  case ET_None:
  case ET_Count:
  default:
    {
      VASSERT_MSG(false, "Please specify a valid RPG_ExplosionType_e.")
    }
    break;
  }

  hkVector4 impulseHk;
  vHavokConversionUtils::VisVecToPhysVecLocal(impulse, impulseHk);
  return impulseHk;
}

hkvVec3 RPG_Explosion::GetSphereImpulse( hkvVec3 const& position )
{
  hkvVec3 impulse = position - m_center;
  float const distance = impulse.getLengthAndNormalize();
  float const strengthScale = hkvMath::Max(0.0f, (m_radius - distance) / m_radius);

  if(impulse.z >= 0.0f)
  {
    impulse.z += 0.1f;
  }
  return impulse * strengthScale * m_impulseStrength;
}

hkvVec3 RPG_Explosion::GetHerringBoneImpulse( hkvVec3 const& position )
{
  hkvVec3 impulse(0.0f, 0.0f, 0.0f);

  hkvVec3 attackLine = m_direction;
  attackLine.z = 0.0f;
  attackLine.normalize();
  float const lineProjection = hkvMath::Max(0.0f, position.dot(attackLine) - m_center.dot(attackLine));
  hkvVec3 const& closestPointOnAttackLine = m_center + lineProjection * attackLine;
  impulse = (position - closestPointOnAttackLine);
  impulse.z = 0.0f;
  float distanceLineToImpact = impulse.getLengthAndNormalize();
  if(hkvMath::isFloatEqual(0.0f, distanceLineToImpact))
  {
    if(Vision::Game.GetFloatRand() > 0.5)
    {
      impulse.x = attackLine.y;
      impulse.y = -attackLine.x;
    }
    else
    {
      impulse.x = -attackLine.y;
      impulse.y = -attackLine.x;
    }
  }
  impulse += attackLine;
  impulse.normalize();
  
  float const strengthScale = hkvMath::Max(0.0f, (m_length - lineProjection) / m_length) *
    hkvMath::Max(0.0f, (m_radius - distanceLineToImpact) / m_radius);

  impulse.z = 0.1f;
  return impulse * strengthScale * m_impulseStrength;
}

void RPG_Explosion::CreateAabb( hkAabb &aabb )
{
  hkVector4 centerHk, directionHk;
  vHavokConversionUtils::VisVecToPhysVecLocal(m_center, centerHk);
  vHavokConversionUtils::VisVecToPhysVecLocal(m_direction, directionHk);

  switch(m_type)
  {
  case ET_Sphere:
    {
      aabb.setFromCenterRadius(centerHk, VIS2HK_FLOAT_SCALED(m_radius));
    }
    break;

  case ET_HerringBone:
    {
      hkVector4 endPointHk;
      endPointHk.setAddMul(centerHk, directionHk, VIS2HK_FLOAT_SCALED(m_length));
      aabb.setFromLine(centerHk, endPointHk);
      aabb.expandBy(VIS2HK_FLOAT_SCALED(m_radius));
    }
    break;

  case ET_None:
  case ET_Count:
  default:
    {
      VASSERT_MSG(false, "Please specify a valid RPG_ExplosionType_e.")
    }
    break;
  }
}

RPG_ExplosionManager::RPG_ExplosionManager()
{

}

RPG_ExplosionManager::~RPG_ExplosionManager()
{
  m_explosions.RemoveAll();
}

void RPG_ExplosionManager::CreateExplosion(hkvVec3 const& center, RPG_ExplosionType_e type, float const strength, float const radius, float const length, hkvVec3 const& direction)
{
#if (HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED) && !(HAVOK_FORCE_SINGLETHREADED)  // Will be on PC, PS3 etc.
  hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  world->lock();
#endif  

  RPG_Explosion explosion(center, type, strength, radius, length, direction);
  m_explosions.Append(explosion);

#if (HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED) && !(HAVOK_FORCE_SINGLETHREADED)  // Will be on PC, PS3 etc.
  world->unlock();
#endif
}

void RPG_ExplosionManager::postSimulationCallback(hkpWorld* world)
{
  int const numExplosions = m_explosions.GetSize();
  for(int i = 0; i < numExplosions; ++i)
  {
    RPG_Explosion& explosion = m_explosions[i];
    explosion.Initialize();
    explosion.Apply();
    explosion.Deinitialize();
  }

  m_explosions.RemoveAll();
}

RPG_ExplosionManager RPG_ExplosionManager::s_instance;

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
