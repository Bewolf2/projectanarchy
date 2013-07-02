/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>

#include <Vision/Runtime/Base/Input/VInput.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>

#include <Vision/Samples/Engine/RPGPlugin/LevelInfo.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerSpawnPoint.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerUI.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorWorldListener.h>

const VString FMOD_EVENT_PROJECT = "Sounds\\FMOD\\RPG.fdp"; // path to the FMOD event project used by the game

RPG_GameManager::RPG_GameManager()
  : m_characters()
  , m_attackableEntities()
  , m_levelInfo(NULL)
{
}

void RPG_GameManager::OneTimeInit()
{
  Vision::Callbacks.OnBeforeSceneLoaded += this;
  Vision::Callbacks.OnAfterSceneLoaded += this;
  Vision::Callbacks.OnBeforeSceneUnloaded += this;
  Vision::Callbacks.OnAfterSceneUnloaded += this;
}

void RPG_GameManager::OneTimeDeInit()
{
  Vision::Callbacks.OnBeforeSceneLoaded -= this;
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  Vision::Callbacks.OnBeforeSceneUnloaded -= this;
  Vision::Callbacks.OnAfterSceneUnloaded -= this;
}

void RPG_GameManager::AddCharacter(RPG_Character* character)
{
  if(character &&
     m_characters.Find(character) < 0)
  {
    m_characters.Add(character);
  }
}

void RPG_GameManager::RemoveCharacter(RPG_Character* character)
{
  int foundIndex = m_characters.Find(character);

  if(foundIndex >= 0)
  {
    m_characters.RemoveAt(foundIndex);
  }
}

VArray<RPG_Character*> const& RPG_GameManager::GetCharacters() const
{
  return m_characters;
}

void RPG_GameManager::AddAttackableEntity(RPG_DamageableEntity* entity)
{
  if(entity &&
     m_attackableEntities.Find(entity) < 0)
  {
    m_attackableEntities.Add(entity);
  }
}

void RPG_GameManager::RemoveAttackableEntity(RPG_DamageableEntity* entity)
{
  int foundIndex = m_attackableEntities.Find(entity);

  if(foundIndex >= 0)
  {
    m_attackableEntities.RemoveAt(foundIndex);
  }
}

VArray<RPG_DamageableEntity*> const& RPG_GameManager::GetAttackableEntities() const
{
  return m_attackableEntities;
}

VisBaseEntity_cl* RPG_GameManager::SpawnPlayer(const VString& prefabName)
{
  VisBaseEntity_cl* entity = NULL;

  if(m_levelInfo)
  {
      RPG_PlayerSpawnPoint* playerSpawnPoint = m_levelInfo->GetInitialPlayerSpawnPoint();

      if(playerSpawnPoint)
      {
        entity = CreateEntityFromPrefab(prefabName, playerSpawnPoint->GetPosition(), playerSpawnPoint->GetOrientation());
      }
      else
      {
        entity = CreateEntityFromPrefab(prefabName, hkvVec3(0.0f, 0.0f, 0.0f), hkvVec3(0.0f, 0.0f, 0.0f));
      }
  }

  return entity;
}

/// RPG CreateEntity Function. 
VisBaseEntity_cl* RPG_GameManager::CreateEntity(const char* className, const hkvVec3& position)
{
  VisBaseEntity_cl* entity = Vision::Game.CreateEntity(className, position);
  VASSERT(entity);
  
  return entity;
}

/// Creates an entity from a Lua script definition.
VisBaseEntity_cl* RPG_GameManager::CreateEntityFromScript(const VString& scriptName, const hkvVec3& position, const hkvVec3& orientation)
{
  VScriptResourceManager* scriptManager = static_cast<VScriptResourceManager*>(Vision::GetScriptManager());

  VScriptResource* scriptRes = scriptManager->LoadScriptFile(scriptName);
  IVScriptInstance* script = scriptRes->CreateScriptInstance();
  VASSERT(script);

  if(script)
  {
    // get the entity class type to create
    char* className = NULL;
    script->ExecuteFunctionArg("GetEntityClassType", "*>s", &className);

    // create the entity
    VisBaseEntity_cl* entity = Vision::Game.CreateEntity(className, position);
    VASSERT(entity);

    if(entity)
    {
      entity->SetOrientation(orientation);

      // attach the script
      scriptManager->SetScriptInstance(entity, script);

      if(entity->IsOfType(V_RUNTIME_CLASS(RPG_BaseEntity)))
      {
        RPG_BaseEntity* rpgEntity = static_cast<RPG_BaseEntity*>(entity);

#if (RPG_SERIALIZATION_SCRIPT_PROPERTIES)
        rpgEntity->InitializeProperties();
#endif
        rpgEntity->PostInitialize();
      }

      return entity;
    }
  }

  return NULL;
}

/// Creates an entity from a prefab file.
VisBaseEntity_cl* RPG_GameManager::CreateEntityFromPrefab(const VString& prefabName, const hkvVec3& position, const hkvVec3& orientation)
{
  if(!prefabName.IsEmpty())
  {
    VPrefab* prefab = VPrefabManager::GlobalManager().LoadPrefab(prefabName);

    VPrefabInstanceInfo info;
    {
      info.m_bOutputInstances = true;
      info.m_vInstancePos = position;
      info.m_vInstanceEuler = orientation;
    }

    prefab->Instantiate(info);

    VisBaseEntity_cl* entity = static_cast<VisBaseEntity_cl*>(info.m_Instances[0]);

    return entity;
  }

  return NULL;
}

/// Finds and stores the level info object.
void RPG_GameManager::FindLevelInfo()
{
  // find the level info object
  VisBaseEntity_cl* levelInfo = Vision::Game.SearchEntity(RPG_LevelInfo::GetStaticKey());

  if(levelInfo &&
    levelInfo->IsOfType(V_RUNTIME_CLASS(RPG_LevelInfo)))
  {
    // store the level info object if we found one
    m_levelInfo = static_cast<RPG_LevelInfo*>(levelInfo);
  }
}

void RPG_GameManager::SetLevelInfo(RPG_LevelInfo* levelInfo)
{
  if(m_levelInfo && levelInfo)
  {
    Vision::Error.Warning("Overwriting the existing LevelInfo stored in RPG_GameManager. This probably means that there are multiple LevelInfo objects in your scene.");
  }

  m_levelInfo = levelInfo;
}

RPG_LevelInfo* RPG_GameManager::GetLevelInfo()
{
  if(!m_levelInfo)
  {
    Vision::Error.Warning("This level does not have a LevelInfo object. Please create one in vForge.");
  }

  return m_levelInfo;
}

const VString& RPG_GameManager::GetFmodEventProject() const
{
  return FMOD_EVENT_PROJECT;
}

RPG_Effect* RPG_GameManager::CreateEffect(RPG_EffectDefinition const& effectDefinition, VisBaseEntity_cl* parentEntity)
{
  RPG_Effect* effect = static_cast<RPG_Effect*>(CreateEntity("RPG_Effect", hkvVec3()));
  effect->Create(effectDefinition, parentEntity);
  return effect;
}

RPG_Effect* RPG_GameManager::CreateEffect(RPG_EffectDefinition const& effectDefinition, const hkvVec3& position)
{
  RPG_Effect* effect = static_cast<RPG_Effect*>(CreateEntity("RPG_Effect", hkvVec3()));
  effect->Create(effectDefinition, position);
  return effect;
}

void RPG_GameManager::OnBeforeSceneLoaded(char const *sceneFileName)
{
  m_sceneFileName = sceneFileName;

  Vision::Callbacks.OnUpdateSceneBegin += this;

  // Add our own Havok Behavior world listener in order to listen to Behavior events
  vHavokBehaviorModule *const havok_behavior_module = vHavokBehaviorModule::GetInstance();
  {
    havok_behavior_module->getBehaviorWorld()->addListener(&RPG_HavokBehaviorWorldListener::s_instance);
  }
}

void RPG_GameManager::OnAfterSceneLoaded()
{
  RPG_RendererUtil::StoreViewParams(m_storedViewParams);

  // Set up game view params
  RPG_ViewParams viewParams = m_storedViewParams;
  {
    viewParams.m_projectionType = VIS_PROJECTIONTYPE_PERSPECTIVE;
    viewParams.m_nearClip = 50.f;
    viewParams.m_farClip = 2500.f;
    viewParams.m_fovX = 0.f;
    viewParams.m_fovY = 50.f;
  }

  RPG_RendererUtil::LoadViewParams(viewParams);

  // Local player
  VisBaseEntity_cl* playerEntity = SpawnPlayer("Prefabs\\Demo_Player_Hero.vprefab");

  if(playerEntity)
  {
    RPG_PlayerControllerComponent *const playerController = static_cast<RPG_PlayerControllerComponent*>
      (playerEntity->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_PlayerControllerComponent)));

    VASSERT(playerController);
    if(playerController)
    {
      RPG_PlayerUI::s_instance.SetController(playerController);
    }
  }
}

void RPG_GameManager::OnBeforeSceneUnloaded()
{
  RPG_PlayerUI::s_instance.SetController(NULL);

  RPG_RendererUtil::LoadViewParams(m_storedViewParams);
}

void RPG_GameManager::OnAfterSceneUnloaded()
{
  m_sceneFileName = "";

  Vision::Callbacks.OnUpdateSceneBegin -= this;

  vHavokBehaviorModule *const havok_behavior_module = vHavokBehaviorModule::GetInstance();
  {
    havok_behavior_module->getBehaviorWorld()->removeListener(&RPG_HavokBehaviorWorldListener::s_instance);
  }
}

void RPG_GameManager::OnUpdateSceneBegin()
{
  // TOOD: In a future release the HB world listener will be moved and updated inside the HB engine plugin
  RPG_HavokBehaviorWorldListener::s_instance.SendEnqueuedNotifications();
}

void RPG_GameManager::OnHandleCallback(IVisCallbackDataObject_cl *callbackData)
{
  if(&Vision::Callbacks.OnBeforeSceneLoaded == callbackData->m_pSender)
  {
    OnBeforeSceneLoaded(static_cast<VisSceneLoadedDataObject_cl*>(callbackData)->m_szSceneFileName);
  }
  else if(&Vision::Callbacks.OnAfterSceneLoaded == callbackData->m_pSender)
  {
    OnAfterSceneLoaded();
  }
  else if(&Vision::Callbacks.OnBeforeSceneUnloaded == callbackData->m_pSender)
  {
    OnBeforeSceneUnloaded();
  }
  else if(&Vision::Callbacks.OnAfterSceneUnloaded == callbackData->m_pSender)
  {
    OnAfterSceneUnloaded();
  }
  else if(&Vision::Callbacks.OnUpdateSceneBegin == callbackData->m_pSender)
  {
    OnUpdateSceneBegin();
  }
  else
  {
    VASSERT_MSG(false, "RPG_GameManager::OnHandleCallback: unexpected callback sender");
  }
}

RPG_GameManager RPG_GameManager::s_instance;

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
