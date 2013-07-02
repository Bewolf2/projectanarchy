/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_AI_SPAWN_POINT_H__
#define RPG_AI_SPAWN_POINT_H__

#include <Vision/Samples/Engine/RPGPlugin/SpawnPoint.h>

class RPG_Character;

/// Ai Spawn Point class. Has a data driven prefab to spawn, and can be hooked up to a trigger.
class RPG_AiSpawnPoint: public RPG_SpawnPoint
{
public:
  RPG_AiSpawnPoint();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  void InitFunction() HKV_OVERRIDE;

  void PostInitialize() HKV_OVERRIDE;

protected:
  // VisBaseEntity_cl
  void ThinkFunction() HKV_OVERRIDE;

  // VisTypedEngineObject_cl
  void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

  void OnSpawn();

protected:
  VString m_prefabName;   ///< name of the prefab to spawn.
  VString m_spawnScript;      ///< script to use when spawning an entity.
  bool m_spawned;         ///< whether or not we've already spawned a character.  @todo: expand to support spawning waves.

  VSmartPtr<VisTriggerSourceComponent_cl> m_onDead;     ///< Component that triggers the OnDead event
  RPG_Character *m_character;                           ///< Character that gets spawned (@todo: add support for spawning waves of characters)

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_AiSpawnPoint, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_AiSpawnPoint, RPG_PLUGIN_IMPEXP);
};

#endif // RPG_AI_SPAWN_POINT_H__

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
