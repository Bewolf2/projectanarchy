/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/LevelInfo.h>

#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/Explosion.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

static const VString s_levelInfoKey = "Level Info";       ///< static name used for the Object Key property

V_IMPLEMENT_SERIAL(RPG_LevelInfo, RPG_BaseEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE (RPG_LevelInfo, RPG_BaseEntity, "RPG LevelInfo Entity", 0, "")
  DEFINE_VAR_OBJECT_REFERENCE (RPG_LevelInfo, m_initialPlayerSpawnPoint, "Initial Player Spawn", 0, 0, 0 ); 
  DEFINE_VAR_VSTRING_AND_NAME (RPG_LevelInfo, m_minimapImagePath, "Minimap Image File", "Path to the map image file to be used in this scene's minimap. (.jpg)", "", 0, 0, "filepicker(.jpg)");
  DEFINE_VAR_FLOAT_AND_NAME (RPG_LevelInfo, m_minimapImageDimensions.x, "Minimap Image Dimensions: X", "XY dimensions of the minimap image file.", "512.0", 0,0);
  DEFINE_VAR_FLOAT_AND_NAME (RPG_LevelInfo, m_minimapImageDimensions.y, "Minimap Image Dimensions: Y", "XY dimensions of the minimap image file.", "512.0", 0,0);
  DEFINE_VAR_FLOAT_AND_NAME (RPG_LevelInfo, m_minimapImageOriginPosition.x, "Minimap Image Origin Position: X", "XY coordinates of the world origin in the minimap image file.", "256.0", 0,0);
  DEFINE_VAR_FLOAT_AND_NAME (RPG_LevelInfo, m_minimapImageOriginPosition.y, "Minimap Image Origin Position: Y", "XY coordinates of the world origin in the minimap image file.", "256.0", 0,0);
  DEFINE_VAR_FLOAT_AND_NAME (RPG_LevelInfo, m_minimapImageScaleFudgeFactor, "Minimap Image Scale Fudge Factor", "A 'fudge' factor to allow corrections to minor scale errors in the minimap file.", "1.0", 0,0);
END_VAR_TABLE


RPG_LevelInfo::RPG_LevelInfo()
  : RPG_BaseEntity()
  , m_minimapImagePath("")
  , m_minimapImageDimensions(0.f, 0.f)
  , m_minimapImageOriginPosition(0.f, 0.f)
  , m_minimapImageScaleFudgeFactor(1.f)
  , m_explosionManagerInitialized(false)
{
}

// returns the static entity key name for this object
const VString& RPG_LevelInfo::GetStaticKey()
{
  return s_levelInfoKey;
}

#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_LevelInfo::GetDependencies(VResourceSnapshot &snapshot)
{
  RPG_BaseEntity::GetDependencies(snapshot);

  // XXX TODO - might want to data drive this
  RPG_DependencyHelper::AddPrefabDependency("Prefabs\\Demo_Player_Hero.vprefab", snapshot);
  RPG_DependencyHelper::AddParticleEffectDependency("Particles/UI/RPG_UI_SelectDestination.xml", snapshot);
}
#endif

void RPG_LevelInfo::Initialize()
{
  RPG_BaseEntity::Initialize();

  if(Vision::Editor.IsInEditor())
  {
#if !defined(_VISION_MOBILE)
    Vision::Editor.SetVariableInEditor(this, "ShapeName", s_levelInfoKey, false, false);
    Vision::Editor.SetVariableInEditor(this, "ObjectKey", s_levelInfoKey, false, false);
#endif
  }

  if(!Vision::Editor.IsPlayingTheGame())
  {
    // automatically hookup the level info if one doesn't already exist.
    RPG_GameManager::s_instance.SetLevelInfo(this);
  }

}

void RPG_LevelInfo::DisposeObject()
{
  hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  VASSERT_MSG(world, "The physics world should still exist while RPG_LevelInfo is being disposed.");
  if(world && m_explosionManagerInitialized)
  {
    world->markForWrite();
    world->removeWorldPostSimulationListener(&RPG_ExplosionManager::s_instance);
    world->unmarkForWrite();
    m_explosionManagerInitialized = false;
  }

  RPG_GameManager::s_instance.SetLevelInfo(NULL);

  VFmodManager::GlobalManager().SetListenerObject(NULL);

  RPG_BaseEntity::DisposeObject();
}

void RPG_LevelInfo::PostInitialize()
{
  RPG_BaseEntity::PostInitialize();

  if(Vision::Editor.IsPlayingTheGame())
  {
    RPG_GameManager::s_instance.SetLevelInfo(this);
  }

  hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  VASSERT_MSG(world, "The physics world should exist before RPG_LevelInfo::PostInitialize is called.");
  VASSERT(!m_explosionManagerInitialized);  
  if(world)
  {
    world->markForWrite();
    world->addWorldPostSimulationListener(&RPG_ExplosionManager::s_instance);
    world->unmarkForWrite();
    m_explosionManagerInitialized = true;
  }

  VisBaseEntity_cl* audioListener = RPG_GameManager::s_instance.CreateEntity("VisBaseEntity_cl", GetPosition());
  VFmodManager::GlobalManager().SetListenerObject(audioListener);

  PreloadFmodEventProject(RPG_GameManager::s_instance.GetFmodEventProject());
}

void RPG_LevelInfo::SetInitialPlayerSpawnPoint(RPG_PlayerSpawnPoint* spawnPoint)
{
  // NOTE: doesn't propogate to the editor yet.
  // @todo: enable this once we know the proper syntax.

  //if(Vision::Editor.IsInEditor() &&
  //   !Vision::Editor.IsPlayingTheGame())
  //{
  //  char formatted[ 256 ]; //Caution with the length, there is risk of a buffer overflow
  //  sprintf( formatted, "%p", spawnPoint );

  //  Vision::Editor.SetVariableInEditor(this, "m_initialPlayerSpawnPoint", formatted, true, true);
  //}
  //else
  {
    m_initialPlayerSpawnPoint.SetReferencedObject(spawnPoint);
  }
}

RPG_PlayerSpawnPoint* RPG_LevelInfo::GetInitialPlayerSpawnPoint() const
{
  return static_cast<RPG_PlayerSpawnPoint*>(m_initialPlayerSpawnPoint.GetReferencedObject());
}

VString RPG_LevelInfo::GetMinimapImagePath() const
{
  return m_minimapImagePath;
}

hkvVec2 RPG_LevelInfo::GetMinimapImageDimensions() const
{
  return m_minimapImageDimensions;
}

hkvVec2 RPG_LevelInfo::GetMinimapImageOriginPosition() const
{
  return m_minimapImageOriginPosition;
}

float RPG_LevelInfo::GetMinimapImageScaleFudgeFactor() const
{
  return m_minimapImageScaleFudgeFactor;
}

void RPG_LevelInfo::PreloadFmodEventProject(VString const& projectFilename)
{
  VASSERT_MSG(!projectFilename.IsEmpty(), "No FMod Event Project was specified.");

  FMOD::EventProject* eventProject = VFmodManager::GlobalManager().LoadEventProject(projectFilename.AsChar());
  VVERIFY_MSG(eventProject != NULL, "Specified FMod Event Project failed to load.");

  int numGroups;
  eventProject->getNumGroups(&numGroups);

  for(int index = 0; index < numGroups; ++index)
  {
    FMOD::EventGroup* eventGroup;
    eventProject->getGroupByIndex(index, true, &eventGroup);

    //eventGroup->loadEventData(FMOD_EVENT_RESOURCE_STREAMS_AND_SAMPLES, FMOD_EVENT_NONBLOCKING);
    eventGroup->loadEventData(FMOD_EVENT_RESOURCE_STREAMS_AND_SAMPLES, FMOD_EVENT_DEFAULT);
  }
}

//void RPG_LevelInfo::OnVariableValueChanged( VisVariable_cl *pVar, const char *value )
//{
//  if (!strcmp(pVar->GetName(), "m_initialPlayerSpawnPoint"))
//  {
//    if (value == NULL)
//      return;
//
//    VTypedObject* pTemp = NULL;
//    if (sscanf(value, "%p", &pTemp) == -1)
//      return;
//
//    if (pTemp == NULL)
//      return;
//
//    m_initialPlayerSpawnPoint.SetReferencedObject(pTemp);
//  }
//}

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
