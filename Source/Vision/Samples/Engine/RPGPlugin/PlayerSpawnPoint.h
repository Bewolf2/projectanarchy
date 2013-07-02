/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLAYER_SPAWN_POINT_H__
#define RPG_PLAYER_SPAWN_POINT_H__

#include <Vision/Samples/Engine/RPGPlugin/SpawnPoint.h>

/// Player SpawnPoint class. Players use these to spawn
class RPG_PlayerSpawnPoint : public RPG_SpawnPoint
{
public:
  RPG_PlayerSpawnPoint();

  void Initialize() HKV_OVERRIDE;
  //void DisposeObject() HKV_OVERRIDE;
  //void ThinkFunction() HKV_OVERRIDE;

  void PostInitialize() HKV_OVERRIDE;

protected:
  // VisTypedEngineObject_cl
  //void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB ) HKV_OVERRIDE;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_PlayerSpawnPoint, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_PlayerSpawnPoint, RPG_PLUGIN_IMPEXP);
};

#endif // RPG_PLAYER_SPAWN_POINT_H__

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
