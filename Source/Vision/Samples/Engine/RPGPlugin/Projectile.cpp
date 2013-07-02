/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Projectile.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

#include <Physics2012/Collide/Query/CastUtil/hkpLinearCastInput.h>

V_IMPLEMENT_SERIAL(RPG_Projectile, RPG_BaseEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_Projectile, RPG_BaseEntity, "Projectile Entity Class", 0, "");
END_VAR_TABLE

class RPG_ProjectilePointCollector : public hkpCdPointCollector
{
public:

  HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AGENT, RPG_ProjectilePointCollector);

  RPG_ProjectilePointCollector(RPG_Projectile* owner)
    : hkpCdPointCollector()
    , m_owner(owner)
    , m_hit(false)
    , m_hitPoint(0.0f, 0.0f, 0.0f)
  {
    VASSERT_MSG(m_owner, "You need to specify the owning projectile when creating RPG_ProjectilePointCollector");
    reset();
  }

  virtual ~RPG_ProjectilePointCollector()
  {

  }

  bool m_hit;
  hkvVec3 m_hitPoint;

protected:

  virtual void addCdPoint(const hkpCdPoint& event)
  {
    if(!m_owner)
    {
      return;
    }

    // event.m_cdBodyA is always the projectile phantom
    const hkpWorldObject* hitObject = hkpGetWorldObject(event.m_cdBodyB.getRootCollidable());

    vHavokUserDataType_e eType = V_USERDATA_UNDEFINED;
    void *pObject = vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void *)hitObject->getUserData(), eType);
    if(pObject)
    {
      bool shouldContinue = true;
      hkvVec3 contactPosition, contactNormal;
      RPG_VisionHavokConversion::HavokToVisionPoint(event.getContact().getPosition(), contactPosition);
      RPG_VisionHavokConversion::HavokToVisionDirection(event.getContact().getNormal(), contactNormal);

      if(eType == V_USERDATA_OBJECT)
      {
        VisTypedEngineObject_cl* typedObject = static_cast<VisTypedEngineObject_cl*>(pObject);
        shouldContinue = m_owner->ProcessCollision(typedObject, contactPosition, contactNormal);
      }
      else if(eType == V_USERDATA_STATIC || eType == V_USERDATA_TERRAIN)
      {
        shouldContinue = m_owner->ProcessCollision(NULL, contactPosition, contactNormal);
      }

      if(!shouldContinue)
      {
        m_hit = true;
        m_hitPoint = contactPosition;
        m_earlyOutDistance = 0.0f;
      }
    }
  }

  RPG_Projectile* m_owner;
};

const float RPG_PROJECTILE_DEFAULT_RADIUS = 10.0f;

RPG_PLUGIN_IMPEXP RPG_Projectile::RPG_Projectile()
: RPG_BaseEntity()
, m_characterOwner(NULL)
, m_projectileMeshFilename("")
, m_currentPosition(0.0f, 0.0f, 0.0f)
, m_speed(5.0f)
, m_direction(0.0f, 0.0f, 0.0f)
, m_targetToTrack(NULL)
, m_shapePhantom(NULL)
, m_spawnTime(0.0f)
, m_maxLifeTime(5.0f)
, m_shouldDestroy(false)
{
  for(int i = 0; i < PFX_Count; ++i)
  {
    m_effectDefinitions[i] = RPG_EffectDefinition();
    m_effects[i] = NULL;
  }
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_Projectile::GetDependencies(VResourceSnapshot &snapshot)
{
  RPG_BaseEntity::GetDependencies(snapshot);
  
  for (int i = 0; i < PFX_Count; ++i)
  {
    if (!m_effectDefinitions[i].m_vfxFilename.IsEmpty())
    {
      RPG_DependencyHelper::AddParticleEffectDependency(m_effectDefinitions[i].m_vfxFilename, snapshot);
    }
  }
}
#endif

void RPG_Projectile::ThinkFunction()
{
  // expire this projectile if it hasn't hit anything within a set lifetime
  if(m_shouldDestroy ||
    Vision::GetTimer()->GetTime() - m_spawnTime > m_maxLifeTime)
  {
    DisposeObject();
    return;
  }

  hkvVec3 newPosition = Move();

  m_currentPosition = SweepPhantom(newPosition);

  SetPosition(m_currentPosition); // sets visual mesh to correspond to the end point of the linear cast
}

void RPG_Projectile::DisposeObject()
{
  if(m_shapePhantom)
  {
    hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
    VASSERT(world);
    if(world)
    {
      vHavokPhysicsModule::GetInstance()->MarkForWrite();
      world->removePhantom(m_shapePhantom);
      vHavokPhysicsModule::GetInstance()->UnmarkForWrite();
    }

    V_SAFE_DELETE(m_shapePhantom);
    m_shapePhantom = NULL;
  }

  RPG_BaseEntity::DisposeObject();
}

void RPG_Projectile::SetCharacterOwner(RPG_Character* character)
{
  m_characterOwner = character;
}

void RPG_Projectile::Fire(hkvVec3 position, hkvVec3 direction, float speed)
{
  VASSERT(m_characterOwner);
  m_spawnTime = Vision::GetTimer()->GetTime();

  m_currentPosition = position;
  SetPosition(position);

  VASSERT_MSG(direction.isNormalized(), "Please normalize your projectile fire direction.");
  m_direction = direction;
  m_speed = speed;

  hkvMat3 vRotation;
  vRotation.setLookInDirectionMatrix(m_direction);
  SetRotationMatrix(vRotation);

  CreateShapePhantom();

  // create inflight effect
  CreateEffect(PFX_Inflight);
}

void RPG_Projectile::Fire(hkvVec3 position, VisBaseEntity_cl* target, bool trackTarget, float speed)
{
  if(trackTarget)
  {
    m_targetToTrack = target;
  }

  hkvVec3 directionToTarget = target->GetPosition() - GetPosition();
  if(directionToTarget.normalizeIfNotZero() == HKV_SUCCESS)
  {
    Fire(position, directionToTarget, speed);
  }
}

void RPG_Projectile::SetProjectileMesh(VString const& meshFilename)
{
  if (!meshFilename.IsEmpty())
  {
    m_projectileMeshFilename = meshFilename;
  }
}

void RPG_Projectile::SetProjectileEffect(RPG_ProjectileEffect_e effectType, RPG_EffectDefinition const& effectDefinition)
{
  m_effectDefinitions[effectType] = effectDefinition;
}

bool RPG_Projectile::ProcessCollision(VisTypedEngineObject_cl* target, hkvVec3 const& position, hkvVec3 const& normal)
{
  RPG_DamageableEntity* targetEntity = NULL;
  if(target)
  {
    if(target->IsFrom(IVObjectComponent))
    {
      target = static_cast<IVObjectComponent*>(target)->GetOwner();
    }

    if(target->IsFrom(RPG_DamageableEntity))
    {
      targetEntity = static_cast<RPG_DamageableEntity*>(target);
    }
  }

  if(ShouldHit(targetEntity))
  {
    OnHit(targetEntity, position, normal);
    m_shouldDestroy = true;
    return false; // process this hit and ignore subsequent hits
  }
  else
  {
    return true; // ignore the hit
  }
}

bool RPG_Projectile::ShouldHit(RPG_DamageableEntity* target)
{
  VASSERT_MSG(m_characterOwner, "Please set your projectile's character owner.");
  if(!target || !m_characterOwner) 
  {
    return true;
  }

  if(target->IsFrom(RPG_Character))
  {
    RPG_Character* targetCharacter = static_cast<RPG_Character*>(target);
    if(targetCharacter->GetTeam() != m_characterOwner->GetTeam())
    {
      return true;
    }
  }
  else if(target->Components().GetComponentOfType(V_RUNTIME_CLASS(RPG_AttackableComponent)))
  {
    return true;
  }

  return false;
}

void RPG_Projectile::OnHit(RPG_DamageableEntity* target, hkvVec3 const& position, hkvVec3 const& normal)
{
  hkvVec3 orientation = NormalToYpr(normal);

  if(!target)
  {
    // null target entity, so we have hit static geometry or terrain
    CreateEffect(PFX_Impact, position, orientation);
    return;
  }

  float damage = 0.0f;

  RPG_InventoryItem const* equippedWeapon = m_characterOwner->GetInventoryHandler().GetEquippedWeapon();
  if(equippedWeapon)
  {
    damage = static_cast<float>(m_characterOwner->GetCharacterStats().GetBaseDamage());

    float const damageMin = damage * equippedWeapon->GetMinDamage();
    float const damageMax = damage * equippedWeapon->GetMaxDamage();
    damage = (damageMin + Vision::Game.GetFloatRand() * (damageMax - damageMin)) + 0.5f;

    float impactSpeed = 0.0f;
    if(m_characterOwner)
    {
      RPG_InventoryItem const* equippedWeapon = m_characterOwner->GetInventoryHandler().GetEquippedWeapon();
      RPG_CharacterActionData const& actionData = m_characterOwner->GetCharacterActionData();
      if(equippedWeapon)
      {
        impactSpeed = equippedWeapon->GetImpactSpeed() * actionData.GetRangedAttackImpactSpeedMultiplier();
      }
    }

    target->TakeDamage(static_cast<int>(damage), -normal * impactSpeed);
    CreateEffect(PFX_Impact, position, orientation);
  }
}

hkvVec3 RPG_Projectile::Move()
{
  if(m_targetToTrack)
  {
    hkvVec3 targetPosition = m_targetToTrack->GetPosition();
    targetPosition.z += 0.5f * m_targetToTrack->GetBoundingBox().getSizeZ();
    hkvVec3 directionToTarget = targetPosition - GetPosition();
    if(directionToTarget.normalizeIfNotZero() == HKV_SUCCESS)
    {
      m_direction = directionToTarget;
    }
    else
    {
      return targetPosition;
    }
  }

  float deltaTime = Vision::GetTimer()->GetTimeDifference();
  return m_currentPosition + m_direction * m_speed * deltaTime;
}

void RPG_Projectile::CreateShapePhantom()
{
  hkReal const radius = RPG_VisionHavokConversion::VisionToHavokScalar(RPG_PROJECTILE_DEFAULT_RADIUS);
  hkpSphereShape* sphere = new hkpSphereShape(radius);

  hkVector4 havokPosition;
  RPG_VisionHavokConversion::VisionToHavokPoint(GetPosition(), havokPosition);

  hkQuaternion havokRotation;
  RPG_VisionHavokConversion::VisionToHavokRotationQuaternion(GetRotationMatrix(), havokRotation);

  hkTransform transform;
  {
    transform.setRotation(havokRotation);
    transform.setTranslation(havokPosition);
  }

  vHavokConversionUtils::VisMatVecToPhysTransform(GetRotationMatrix(), GetPosition(), transform);

  m_shapePhantom = new hkpSimpleShapePhantom(sphere, transform,
    hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DYNAMIC, 0, 0, 0));
	sphere->removeReference();

  hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  VASSERT_MSG(world, "Please create a physics world before creating any projectiles.");
  if(world)
  {
    vHavokPhysicsModule::GetInstance()->MarkForWrite();
    world->addPhantom(m_shapePhantom);
    vHavokPhysicsModule::GetInstance()->UnmarkForWrite();
  }
  else
  {
    V_SAFE_DELETE(m_shapePhantom);
    m_shapePhantom = NULL;
  }
}

hkvVec3 RPG_Projectile::SweepPhantom(hkvVec3 newPosition)
{
  hkVector4 hCurrentPosition, hNewPosition;
  RPG_VisionHavokConversion::VisionToHavokPoint(m_currentPosition, hCurrentPosition);
  RPG_VisionHavokConversion::VisionToHavokPoint(newPosition, hNewPosition);

  hkpLinearCastInput lci;
  {
    lci.m_startPointTolerance = 0.01f;
    lci.m_maxExtraPenetration = 0.01f;
    lci.m_to = hNewPosition;
  }
  RPG_ProjectilePointCollector collector(this);

  VASSERT(m_shapePhantom);
  if(m_shapePhantom)
  {
    // move shape phantom to the last frame's position, and sweep toward the current frame's position
    vHavokPhysicsModule::GetInstance()->MarkForWrite();
    m_shapePhantom->setPositionAndLinearCast(hCurrentPosition, lci, collector, HK_NULL);
    vHavokPhysicsModule::GetInstance()->UnmarkForWrite();
  }

  if(collector.m_hit)
  {
    return collector.m_hitPoint;
  }
  else
  {
    return newPosition;
  }
}

void RPG_Projectile::CreateEffect(RPG_ProjectileEffect_e effectType)
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

void RPG_Projectile::CreateEffect(RPG_ProjectileEffect_e effectType, hkvVec3 const& position, hkvVec3 const& orientation)
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

hkvVec3 RPG_Projectile::NormalToYpr(hkvVec3 const &normal)
{
  float pitch = 0.0f, roll = 0.0f;
  if(!hkvMath::isFloatEqual(normal.x, 0.0f) && !hkvMath::isFloatEqual(normal.z, 0.0f))
  {
    pitch = hkvMath::atan2Deg(normal.x, normal.z);
    roll = hkvMath::atan2Deg(-normal.y, normal.x / hkvMath::sinDeg(pitch));
  }
  else if(hkvMath::isFloatEqual(normal.x, 0.0f))
  {
    // sin(pitch) is zero and cos(roll) is nonzero
    if(normal.z >= 0.0f)
    {
      pitch = 0.0f;
      roll = hkvMath::atan2Deg(-normal.y, normal.z);
    }
    else
    {
      pitch = 180.0f;
      roll = hkvMath::atan2Deg(-normal.y, -normal.z);
    }
  }
  else if(hkvMath::isFloatEqual(normal.z, 0.0f))
  {
    // cos(pitch) is zero and cos(roll) is nonzero
    if(normal.y >= 0.0f)
    {
      pitch = 90.0f;
      roll = hkvMath::atan2Deg(-normal.y, normal.x);
    }
    else
    {
      pitch = 270.0f;
      roll = hkvMath::atan2Deg(-normal.y, -normal.x);
    }
  }
  else
  {
    // cos(roll) is zero
    pitch = 0.0f;
    if(normal.y <= 0.0f)
    {
      roll = 90.0f;
    }
    else
    {
      roll = 270.0f;
    }
  }

#ifdef _DEBUG
  {
    hkvMat3 rotation;
    rotation.setFromEulerAngles(roll, pitch, 0.0f);
    hkvVec3 rotatedUp = rotation.transformDirection(hkvVec3(0.0f, 0.0f, 1.0f));
    float dot = rotatedUp.dot(normal);
    VASSERT(hkvMath::isFloatEqual(dot, 1.0f));
  }
#endif

  // @note: entities take in orientation as YPR, as opposed to the RPY used in hkvMat3::setFromEulerAngles
  hkvVec3 orientationYpr(0.0f, pitch, roll);
  return orientationYpr;
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
