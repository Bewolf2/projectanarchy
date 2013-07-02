/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/DestructibleEntity.h>

#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryItem.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>

namespace
{
  hkaiAvoidanceSolver::AvoidanceProperties const s_AvoidanceProperties;
}

V_IMPLEMENT_SERIAL(RPG_DestructibleEntity, RPG_DamageableEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_DestructibleEntity, RPG_DamageableEntity, "A destructible prop.", 0, "")
  DEFINE_VAR_BOOL_AND_NAME(RPG_DestructibleEntity, m_removeCollisionAfterDestruction, "Remove collision after destruction", "Should this object's collision be removed when the object is destroyed?", "FALSE", 0, 0);

  DEFINE_VAR_INT_AND_NAME(RPG_DestructibleEntity, m_healthMax, "Health", "Amount of damage taken before destruction.", "1", 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_postDestructionMeshFilename, "Post-Destruction Mesh", "Replacement mesh for after destruction. If empty, object will be removed from scene.", "", 0, 0, "filepicker(.model)");
  
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_Ambient].m_vfxFilename, "Ambient Effect Filename", "Name of the particle effect to play while pickup is active.", "", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_Ambient].m_fmodEventGroupName, "Ambient Effect Sound Fmod Group Name", "Name of the fmod event group which contains this effect's sound.", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_Ambient].m_fmodEventName, "Ambient Effect Sound Fmod Event name", "Name of the fmod event to play with this effect.", "", 0, 0, 0);
  
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_TakeHit].m_vfxFilename, "Takehit Effect Filename", "Name of the particle effect to play when object is hit.", "", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_TakeHit].m_fmodEventGroupName, "Ambient Effect Sound Fmod Groupname", "Name of the fmod event group which contains this effect's sound.", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_TakeHit].m_fmodEventName, "Ambient Effect Sound Fmod Event name", "Name of the fmod event to play with this effect.", "", 0, 0, 0);
  
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_Destroy].m_vfxFilename, "Destroy Effect Filename", "Name of the particle effect to play when destroyed.", "", 0, 0, "filepicker(.xml)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_Destroy].m_fmodEventGroupName, "Ambient Effect Sound Fmod Groupname", "Name of the fmod event group which contains this effect's sound.", "", 0, 0, 0);
  DEFINE_VAR_VSTRING_AND_NAME(RPG_DestructibleEntity, m_effectDefinitions[DEFX_Destroy].m_fmodEventName, "Ambient Effect Sound Fmod Event name", "Name of the fmod event to play with this effect.", "", 0, 0, 0);
  
END_VAR_TABLE


RPG_DestructibleEntity::RPG_DestructibleEntity() 
  : RPG_DamageableEntity()
  , m_postDestructionMeshFilename("")
  , m_healthMax(1)
  , m_removeCollisionAfterDestruction(FALSE)
  , m_isDestroyed(false)
  , m_health(1)
{
  m_collisionRadius = 100.0f;
  m_collisionHeight = 200.0f;

  for (int i = 0; i < DEFX_Count; ++i)
  {
    m_effectDefinitions[i] = RPG_EffectDefinition();
    m_effects[i] = NULL;
  }
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_DestructibleEntity::GetDependencies(VResourceSnapshot &snapshot)
{
  RPG_DamageableEntity::GetDependencies(snapshot);

  for (int i = 0; i < DEFX_Count; ++i)
  {
    if (!m_effectDefinitions[i].m_vfxFilename.IsEmpty())
    {
      RPG_DependencyHelper::AddParticleEffectDependency(m_effectDefinitions[i].m_vfxFilename, snapshot);
    }
  }

  RPG_DependencyHelper::AddModelDependency(m_postDestructionMeshFilename, snapshot);
}
#endif

void RPG_DestructibleEntity::PostInitialize()
{
  RPG_DamageableEntity::PostInitialize();

  // Preload the mesh and effect to be used when this prop is destroyed.
  RPG_VisionEffectHelper::LoadMesh(m_postDestructionMeshFilename);

  AddComponents();

  AddObstacle();

  m_health = m_healthMax;
  CreateEffect(DEFX_Ambient);
}

void RPG_DestructibleEntity::DisposeObject()
{
  RemoveObstacle();

  RPG_DamageableEntity::DisposeObject();
}

void RPG_DestructibleEntity::AddComponents()
{
  // Add an attackable component if none was added in the editor
  if (!static_cast<RPG_AttackableComponent*>(Components().GetComponentOfType(V_RUNTIME_CLASS(RPG_AttackableComponent))))
  {
    RPG_AttackableComponent* attackableComponent = new RPG_AttackableComponent;
    AddComponent(attackableComponent);
  }

  // Add a rigid body component if none already exists
  if (!Components().GetComponentOfType("vHavokRigidBody"))
  {
    // Rigid Body
    vHavokRigidBody* rigidBody = new vHavokRigidBody;
    rigidBody->Shape_Type = ShapeType_CYLINDER;
    rigidBody->Shape_Radius = m_collisionRadius;
    rigidBody->Shape_Height = m_collisionHeight;
    rigidBody->Havok_MotionType = MotionType_FIXED;
    AddComponent(rigidBody);
  }
}

void RPG_DestructibleEntity::ThinkFunction()
{
  if(m_isDestroyed && m_postDestructionMeshFilename.IsEmpty())
  {
    DisposeObject();
  }
}

int RPG_DestructibleEntity::TakeDamage( const int damageAmount, const hkvVec3& impactVelocity /*= hkvVec3(0,0,0)*/ )
{
  CreateEffect(DEFX_TakeHit);
  if(damageAmount >= m_health)
  {
    SetDestroyed();
    return m_health;
  }
  else
  {
    m_health -= damageAmount;
    return damageAmount;
  }
}

void RPG_DestructibleEntity::SetDestroyed()
{
  m_isDestroyed = true;

  // remove the attackable component
  RPG_AttackableComponent* attackableComponent = static_cast<RPG_AttackableComponent*>(Components().GetComponentOfType(V_RUNTIME_CLASS(RPG_AttackableComponent)));
  if (attackableComponent)
  {
    RemoveComponent(attackableComponent);
  }

  vHavokRigidBody* rigidBodyComponent = static_cast<vHavokRigidBody*>(Components().GetComponentOfType("vHavokRigidBody"));
  if (rigidBodyComponent)
  {
    RemoveComponent(rigidBodyComponent);
  }

  // stop the ambient effect
  StopEffect(DEFX_Ambient);
  CreateEffect(DEFX_Destroy, GetPosition(), GetOrientation());

  // remove collision if so instructed
  if (m_removeCollisionAfterDestruction)
  {
    RemoveObstacle();
  }

  if(!m_postDestructionMeshFilename.IsEmpty())
  {
    // swap the mesh
    SetMesh(m_postDestructionMeshFilename);
  }
  else
  {
    DisposeObject();
  }
}

void RPG_DestructibleEntity::CreateEffect(RPG_DestructibleEntityEffect_e effectType)
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

void RPG_DestructibleEntity::CreateEffect(RPG_DestructibleEntityEffect_e effectType, hkvVec3 const& position, hkvVec3 const& orientation)
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

void RPG_DestructibleEntity::SetEffectDefaults()
{
  RPG_DestructibleEntityEffect_e effectType;

  effectType = DEFX_TakeHit;
  {
    m_effectDefinitions[effectType].m_shaderFlashDuration = 1.f;
    m_effectDefinitions[effectType].m_shaderFlashColor = VColorRef(150, 200, 255);
  }
}

void RPG_DestructibleEntity::StopEffect(RPG_DestructibleEntityEffect_e effectType)
{
  if (m_effects[effectType])
  {
    m_effects[effectType]->Finish();

    // clear the effect slot reference
    m_effects[effectType] = NULL;
  }
}

void RPG_DestructibleEntity::AddObstacle()
{
  if(!Vision::Editor.IsPlaying())
    return;

  VASSERT(!m_aiBehavior);
  if(m_aiBehavior)
    return;

  // Add AI obstacle
  hkaiCharacter::Cinfo characterCinfo;
  {
    RPG_VisionHavokConversion::VisionToHavokPoint(GetPosition(), characterCinfo.m_initialPosition);
    RPG_VisionHavokConversion::VisionToHavokDirection(GetDirection(), characterCinfo.m_initialForward);
    characterCinfo.m_up = hkVector4::getConstant<HK_QUADREAL_0010>();
    characterCinfo.m_avoidanceProperties = &s_AvoidanceProperties;
    {
      hkReal const radius = RPG_VisionHavokConversion::VisionToHavokScalar(m_collisionRadius);
      {
        characterCinfo.m_radius = radius;
        characterCinfo.m_desiredSpeed = 0.0f;
      }
    }
  }

  m_aiCharacter = new hkaiCharacter(characterCinfo);
  m_aiCharacter->removeReference();

  hkaiPathFollowingBehavior::Cinfo behaviorCinfo;
  m_aiBehavior = new hkaiPathFollowingBehavior(m_aiCharacter, vHavokAiModule::GetInstance()->GetAiWorld(), behaviorCinfo);
  m_aiBehavior->removeReference();

  vHavokAiModule::GetInstance()->getCharacterBehaviors().pushBack(m_aiBehavior);
}

void RPG_DestructibleEntity::RemoveObstacle()
{
  if(m_aiBehavior)
  {
    int const aiBehaviorIndex = vHavokAiModule::GetInstance()->getCharacterBehaviors().indexOf(m_aiBehavior);
    VASSERT(aiBehaviorIndex >= 0);
    if(aiBehaviorIndex >= 0)
    {
      vHavokAiModule::GetInstance()->getCharacterBehaviors().removeAt(aiBehaviorIndex);
    }

    m_aiBehavior = NULL;
    m_aiCharacter = NULL;
  }
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
