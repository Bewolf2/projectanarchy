/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_GAME_MANAGER_H__
#define RPG_PLUGIN_GAME_MANAGER_H__

#include <Vision/Samples/Engine/RPGPlugin/Effect.h>
#include <Vision/Samples/Engine/RPGPlugin/Renderer.h>

class RPG_Character;
class RPG_DamageableEntity;
class RPG_PlayerSpawnPoint;
class RPG_LevelInfo;

enum RPG_Team_e
{
  TEAM_None=-1,
  TEAM_Blue=0,
  TEAM_Red,
  TEAM_Count
};

/// Runtime class for managing game data. Things such as grabbing entities, spawning players, and accessing level information should go through here.
class RPG_GameManager : public IVisCallbackHandler_cl
{
public:
  RPG_GameManager();

  void OneTimeInit();
  void OneTimeDeInit();

  void AddCharacter(RPG_Character* character);
  void RemoveCharacter(RPG_Character* character);
  VArray<RPG_Character*> const& GetCharacters() const;

  void AddAttackableEntity(RPG_DamageableEntity* entity);
  void RemoveAttackableEntity(RPG_DamageableEntity* entity);
  VArray<RPG_DamageableEntity*> const& GetAttackableEntities() const;

  VisBaseEntity_cl* SpawnPlayer(const VString& prefabName);

  VisBaseEntity_cl* CreateEntity(const char* className, const hkvVec3& position);
  VisBaseEntity_cl* CreateEntityFromScript(const VString& scriptName, const hkvVec3& position, const hkvVec3& orientation);
  VisBaseEntity_cl* CreateEntityFromPrefab(const VString& prefabName, const hkvVec3& position, const hkvVec3& orientation);

  RPG_Effect* CreateEffect(RPG_EffectDefinition const& effectDefinition, VisBaseEntity_cl* parentEntity);
  RPG_Effect* CreateEffect(RPG_EffectDefinition const& effectDefinition, const hkvVec3& position);

  void SetLevelInfo(RPG_LevelInfo* levelInfo);
  RPG_LevelInfo* GetLevelInfo();

  const VString& GetFmodEventProject() const;

protected:
  void FindLevelInfo();

private:
  void OnBeforeSceneLoaded(char const *sceneFileName);

  void OnAfterSceneLoaded();

  void OnBeforeSceneUnloaded();

  void OnAfterSceneUnloaded();

  void OnUpdateSceneBegin();

  // IVisCallbackHandler_cl interface
  void OnHandleCallback(IVisCallbackDataObject_cl *callbackData);

public:
  static RPG_GameManager s_instance;

private:
  VString m_sceneFileName;

  RPG_ViewParams m_storedViewParams;

  VArray<RPG_Character*> m_characters;
  VArray<RPG_DamageableEntity*> m_attackableEntities;

  RPG_LevelInfo* m_levelInfo;
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
