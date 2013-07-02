/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/AiSpawnPoint.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>

#include <Vision/Runtime/Engine/SceneElements/VisApiTriggerComponent.hpp>


V_IMPLEMENT_SERIAL(RPG_AiSpawnPoint, RPG_SpawnPoint, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_AiSpawnPoint, RPG_SpawnPoint, "Ai Spawn Point Class", 0, "")  
  DEFINE_VAR_VSTRING_AND_NAME(RPG_AiSpawnPoint, m_prefabName, "Prefab", "Prefab for the Ai Character", "", 0, 0, "filepicker(.vPrefab)");
  DEFINE_VAR_VSTRING_AND_NAME(RPG_AiSpawnPoint, m_spawnScript, "Spawn Script file", "Script to attach to the class", "", 0, 0, "filepicker(.lua)");
END_VAR_TABLE


RPG_AiSpawnPoint::RPG_AiSpawnPoint()
  : RPG_SpawnPoint()
  , m_prefabName()
  , m_spawnScript()
  , m_spawned(false)
  , m_character(NULL)
{
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_AiSpawnPoint::GetDependencies(VResourceSnapshot &snapshot)
{
  RPG_SpawnPoint::GetDependencies(snapshot);

  if(!m_prefabName.IsEmpty())
  {
    RPG_DependencyHelper::AddPrefabDependency(m_prefabName, snapshot);
  }
  else if(!m_spawnScript.IsEmpty())
  {
    RPG_DependencyHelper::AddScriptEntityDependency(m_spawnScript, snapshot);
  }
}
#endif

// Deliberate use of InitFunction: I want to add these components in vForge/CreateEntity; otherwise serialization will create them
void RPG_AiSpawnPoint::InitFunction()
{
  RPG_SpawnPoint::InitFunction();

  VisTriggerTargetComponent_cl* spawnTarget = new VisTriggerTargetComponent_cl("Spawn", VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
  AddComponent(spawnTarget);

  m_onDead = new VisTriggerSourceComponent_cl("OnDead", VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
  AddComponent(m_onDead);
}

void RPG_AiSpawnPoint::ThinkFunction()
{
  if (m_spawned)
  {
    if (m_character != NULL && m_character->IsDead())
    {
      if(m_onDead)
        m_onDead->TriggerAllTargets();
      m_character = NULL;
    }
  }
}

void RPG_AiSpawnPoint::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  RPG_SpawnPoint::MessageFunction(iID, iParamA, iParamB);

  switch(iID)
  {
    case VIS_MSG_TRIGGER:
    {
      // if a trigger is calling this object, spawn the NPC.
      OnSpawn();

      break;
    }

    //case VIS_MSG_EDITOR_GETLINKS:
    //{
    //  VShapeLinkConfig* linkConfig = reinterpret_cast<VShapeLinkConfig*>(iParamA);

    //  // setup the number of links
    //  linkConfig->AllocateLinks(1);

    //  // create the trigger target
    //  VShapeLinkInfo& triggerTarget(linkConfig->GetLinkInfo(0));
    //  triggerTarget.m_eType = VShapeLinkInfo::LINK_TARGET;
    //  triggerTarget.m_sDisplayName = "Trigger Target";
    //  triggerTarget.m_sUniqueStringID = "trigger_target";

    //  break;
    //}

    //case VIS_MSG_EDITOR_CANLINK:
    //{
    //  VShapeLink* link = reinterpret_cast<VShapeLink*>(iParamA);

    //  VisTypedEngineObject_cl* otherObject = link->m_pOtherObject;

    //  if(!link->m_pOtherObject)
    //  {
    //    link->m_bResult = false;
    //  }
    //  else if(link->m_pOtherObject->IsOfType(V_RUNTIME_CLASS(VisTriggerSourceComponent_cl)))
    //  {
    //    // allow a trigger source to link to this
    //    link->m_bResult = true;
    //  }

    //  break;
    //}


    default:
    {
      break;
    }
  }
}

void RPG_AiSpawnPoint::PostInitialize()
{
  RPG_SpawnPoint::PostInitialize();

  // Cache trigger source: note that this may not exist in serialized scenes if nothing referenced it
  if(!m_onDead)
    m_onDead = static_cast<VisTriggerSourceComponent_cl*>(Components().GetComponentOfTypeAndName(V_RUNTIME_CLASS(VisTriggerSourceComponent_cl), "OnDead"));
}

///< Spawn the Ai Character and perform any setup we need to of them.
void RPG_AiSpawnPoint::OnSpawn()
{
  // only spawn if we haven't already. @todo: add support for spawning waves of characters.
  if(m_spawned)
  {
    return;
  }

  m_spawned = true;

  bool success = false;

  if(!m_prefabName.IsEmpty())
  {
    VisBaseEntity_cl* entity = RPG_GameManager::s_instance.CreateEntityFromPrefab(m_prefabName, GetPosition(), GetOrientation());
    
    if(entity)
    {
      if(entity->IsOfType(V_RUNTIME_CLASS(RPG_Character)))
      {
        m_character = static_cast<RPG_Character*>(entity);
      }

      success = true;
    }
  }
  else if(!m_spawnScript.IsEmpty())
  {
    VisBaseEntity_cl* entity = RPG_GameManager::s_instance.CreateEntityFromScript(m_spawnScript, GetPosition(), GetOrientation());
    VASSERT(entity);

    if(entity)
    {
      if(entity->IsOfType(V_RUNTIME_CLASS(RPG_Character)))
      {
        m_character = static_cast<RPG_Character*>(entity);
      }

      success = true;
    }
  }

  if(!success)  
  {
    Vision::Error.Warning("Ai Spawner has no Prefab defined, and no valid Script definition!");
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
