/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Pickup.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryItem.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>

V_IMPLEMENT_SERIAL_ABSTRACT(RPG_Pickup, RPG_BaseEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_Pickup, RPG_BaseEntity, "A generic pickup that can do something when a player walks over / near it.", 0, "");
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Pickup, m_pickupRadius, "Pickup Radius", "Radius within which a player should activate this pickup", "200.0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Pickup, m_rotateSpeed, "Rotation Speed", "Rotation speed (degrees per second)", "10.0", 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Pickup, m_effectDefinitions[PKFX_Spawn].m_vfxFilename, "Spawn Effect Filename", "Name of the particle effect to play when this pickup spawns.", "", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Pickup, m_effectDefinitions[PKFX_Ambient].m_vfxFilename, "Ambient Effect Filename", "Name of the particle effect to play while pickup is active.", "", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Pickup, m_effectDefinitions[PKFX_Pickup].m_vfxFilename, "Pickup Effect Filename", "Name of the particle effect to play when picked up.", "", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_Pickup, m_pickupEffectFilename, "[DEPRECATED] Pickup Effect Filename", "[DEPRECATED] Name of the particle effect to play when picked up.", "Particles/Splash/Particles_Rainbow_Splash_01.xml", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_BOOL(RPG_Pickup, m_usePhysics, "Whether or not this pickup uses a physics rigid body.", "FALSE", 0, 0);
  DEFINE_VAR_FLOAT(RPG_Pickup, m_mass, "The mass of this pickup. Range is 0-1", "0.3", 0, 0);
  DEFINE_VAR_FLOAT(RPG_Pickup, m_magnetMaxDistance, "Max distance at which the pickup magnet will attract to the player", "1000", 0, 0);
  DEFINE_VAR_FLOAT(RPG_Pickup, m_magnetSpeedMultiplier, "The speed multiplier for magnet attraction towards the player", "200", 0, 0);
  DEFINE_VAR_FLOAT(RPG_Pickup, m_spawnIdleTime, "Idle time after spawning before the magnet attraction starts working", "3", 0, 0);
  DEFINE_VAR_FLOAT(RPG_Pickup, m_verticalSpawnForceMin, "Vertical Spawn Force Minimum", "2000", 0, 0);
  DEFINE_VAR_FLOAT(RPG_Pickup, m_verticalSpawnForceMax, "Vertical Spawn Force Maximum", "4000", 0, 0);
  DEFINE_VAR_FLOAT(RPG_Pickup, m_horizontalSpawnForceMin, "Horizontal Spawn Force Minimum", "500", 0, 0);
  DEFINE_VAR_FLOAT(RPG_Pickup, m_horizontalSpawnForceMax, "Horizontal Spawn Force Maximum", "1500", 0, 0);
END_VAR_TABLE


const float RPG_HEALTH_PICKUP_MAGNET_SPEED_MULTIPLIER_CONVERSION_VALUE = 1000000.0f;        // this keeps the multiplier we have to tune a little smaller


RPG_Pickup::RPG_Pickup() 
  : RPG_BaseEntity()
  , m_pickupRadius(200.0f)
  , m_rotateSpeed(10.0f)
  , m_pickupEffectFilename("")
  , m_usePhysics(FALSE)
  , m_rigidBodyComponent()
  , m_elapsedTime(0.0f)
  , m_mass(0.3f)
  , m_magnetMaxDistance(1000.0f)
  , m_magnetSpeedMultiplier(200.0f)
  , m_spawnIdleTime(3.0f)
  , m_verticalSpawnForceMin(2000.0f)
  , m_verticalSpawnForceMax(4000.0f)
  , m_horizontalSpawnForceMin(500.0f)
  , m_horizontalSpawnForceMax(1500.0f)
  , m_magnetMaxDistanceInverseSquared(0.0f)
  , m_currentMagnetSpeed(0.0f)
  , m_currentMagnetVelocity(0.0f, 0.0f, 0.0f)
{
  for (int i = 0; i < PKFX_Count; ++i)
  {
    m_effectDefinitions[i] = RPG_EffectDefinition();
    m_effects[i] = NULL;
  }
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_Pickup::GetDependencies(VResourceSnapshot &snapshot)
{
  RPG_BaseEntity::GetDependencies(snapshot);

  for (int i = 0; i < PKFX_Count; ++i)
  {
    if (!m_effectDefinitions[i].m_vfxFilename.IsEmpty())
    {
      RPG_DependencyHelper::AddParticleEffectDependency(m_effectDefinitions[i].m_vfxFilename, snapshot);
    }
  }

  // deprecated
  if(!m_pickupEffectFilename.IsEmpty())
  {
    RPG_DependencyHelper::AddParticleEffectDependency(m_pickupEffectFilename, snapshot);
  }
}
#endif

void RPG_Pickup::Initialize()
{
  RPG_BaseEntity::Initialize();
}

void RPG_Pickup::PostInitialize()
{
  RPG_BaseEntity::PostInitialize();

  // setup a rigid body and apply initial spawn forces if we need to.
  if(m_usePhysics)
  {
    // setup the inverse square of the max distance
    m_magnetMaxDistanceInverseSquared = 1.0f / (m_magnetMaxDistance * m_magnetMaxDistance);

    // create a rotation
    hkvVec3 rotation = GetOrientation();
    rotation.x = Vision::Game.GetFloatRand() * 360.0f;
    SetOrientation(rotation);

    // setup a rigid body component
    m_rigidBodyComponent = static_cast<vHavokRigidBody*>(Components().GetComponentOfType(V_RUNTIME_CLASS(vHavokRigidBody)));

    if(!m_rigidBodyComponent)
    {
      m_rigidBodyComponent = new vHavokRigidBody();
      VASSERT(m_rigidBodyComponent);
      m_rigidBodyComponent->Initialize();
      m_rigidBodyComponent->Shape_Type = ShapeType_SPHERE;
      m_rigidBodyComponent->Havok_QualityType = QualityType_MOVING;
      m_rigidBodyComponent->Havok_CollisionLayer = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM4; // this pickup should only collide with static geometry and terrain
      m_rigidBodyComponent->Havok_Mass = m_mass;

      AddComponent(m_rigidBodyComponent);
    }

    VASSERT(m_verticalSpawnForceMax >= m_verticalSpawnForceMin);
    float energy = m_verticalSpawnForceMin + (Vision::Game.GetFloatRand() * (m_verticalSpawnForceMax - m_verticalSpawnForceMin));

    m_rigidBodyComponent->ApplyLinearImpulse( hkvVec3(0.0f, 0.0f, 1.0f) * energy);

    // Since the object is already rotated, just move forward
    VASSERT(m_horizontalSpawnForceMax >= m_horizontalSpawnForceMin);
    energy = m_horizontalSpawnForceMin + (Vision::Game.GetFloatRand() * (m_horizontalSpawnForceMax - m_horizontalSpawnForceMin));

    m_rigidBodyComponent->ApplyLinearImpulse(GetDirection().getNormalized() * energy);
  }

  CreateEffect(PKFX_Spawn, GetPosition(), GetOrientation());
  CreateEffect(PKFX_Ambient);
}

void RPG_Pickup::ThinkFunction()
{
  const float deltaTime = Vision::GetTimer()->GetTimeDifference();

  if(m_elapsedTime < m_spawnIdleTime)
  {
    // don't do anything if we haven't been idle long enough
    m_elapsedTime += deltaTime;

    return;
  }

  if(m_rigidBodyComponent)
  {
    RemoveComponent(m_rigidBodyComponent);
    m_rigidBodyComponent = NULL;
    m_currentMagnetSpeed = 0.0f;
    m_currentMagnetVelocity.setZero();
  }

  UpdateRotation(deltaTime);
  UpdateMagnetForces(deltaTime);

  IncPosition(deltaTime * m_currentMagnetVelocity);

  CheckCharacterContact();
}

void RPG_Pickup::CheckCharacterContact()
{
  VArray<RPG_Character*> const& characters = RPG_GameManager::s_instance.GetCharacters();

  hkvVec3 const& currentPosition = GetPosition();

  for(int index = 0; index < characters.GetSize(); ++index)
  {
    RPG_Character* character = characters.GetAt(index);

    // only target players
    if(!character->IsOfType(V_RUNTIME_CLASS(RPG_PlayerCharacter)))
    {
      continue;
    }

    hkvVec3 const& targetPosition = character->GetPosition();

    float currentRangeSquared = (currentPosition - targetPosition).getLengthSquared();

    if(currentRangeSquared <= m_pickupRadius * m_pickupRadius)
    {
      // call OnPickup for subclasses
      OnPickup(character);

      // play the pickup effect
      CreateEffect(PKFX_Pickup, GetPosition(), GetOrientation());

      // setup this object for deletion
      DisposeObject();
    }
  }
}

void RPG_Pickup::UpdateRotation(float const deltaTime)
{
  IncOrientation(hkvVec3 (m_rotateSpeed * deltaTime,0,0));
}

void RPG_Pickup::UpdateMagnetForces(float const deltaTime)
{
  if(m_usePhysics)
  {
    // find the player
    VArray<RPG_Character*> const& characters = RPG_GameManager::s_instance.GetCharacters();

    hkvVec3 const& currentPosition = GetPosition();

    RPG_PlayerCharacter* player = NULL;

    // find a player in range
    for(int index = 0; index < characters.GetSize(); ++index)
    {
      RPG_Character* character = characters.GetAt(index);

      // only target apparently alive players
      if(!character->IsOfType(V_RUNTIME_CLASS(RPG_PlayerCharacter)) ||
        character->IsDead() ||
        character->IsFeigningDeath())
      {
        continue;
      }

      player = static_cast<RPG_PlayerCharacter*>(character);

      break;
    }

    if(player)
    {
      hkvVec3 targetPosition = player->GetPosition();
      targetPosition.z += 0.5f * player->GetBoundingBox().getSizeZ();

      float distance = (currentPosition - targetPosition).getLength();

      if(distance < m_pickupRadius)
      {
        distance = m_pickupRadius;
      }

      float const distanceSquared = distance * distance;

      if(distanceSquared < m_magnetMaxDistance * m_magnetMaxDistance)
      {
        hkvVec3 direction = targetPosition - GetPosition();
        direction.normalizeIfNotZero();

        float magnitude = m_magnetSpeedMultiplier * (1.0f / distanceSquared - m_magnetMaxDistanceInverseSquared);
        m_currentMagnetSpeed += deltaTime * magnitude * RPG_HEALTH_PICKUP_MAGNET_SPEED_MULTIPLIER_CONVERSION_VALUE;
        m_currentMagnetVelocity = m_currentMagnetSpeed * direction;
      }
    }
  }
}

void RPG_Pickup::CreateEffect(RPG_PickupEffect_e effectType)
{
  // create an entry for this instantiated effect, if needed
  if (!m_effects[effectType])
  {
    m_effects[effectType] = static_cast<RPG_Effect*>(RPG_GameManager::s_instance.CreateEntity("RPG_Effect", GetPosition()));
  }
  VASSERT(m_effects[effectType]);

  // create this effect's visual and audio components
  m_effects[effectType]->Create(m_effectDefinitions[effectType], this);
}

void RPG_Pickup::CreateEffect(RPG_PickupEffect_e effectType, hkvVec3 const& position, hkvVec3 const& orientation)
{
  // create an entry for this instantiated effect, if needed
  if (!m_effects[effectType])
  {
    m_effects[effectType] = static_cast<RPG_Effect*>(RPG_GameManager::s_instance.CreateEntity("RPG_Effect", GetPosition()));
  }
  VASSERT(m_effects[effectType]);

  // create this effect's visual and audio components
  m_effects[effectType]->Create(m_effectDefinitions[effectType], position, orientation);
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
