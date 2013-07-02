/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_LEVEL_INFO_H__
#define RPG_LEVEL_INFO_H__

#include <Vision/Samples/Engine/RPGPlugin/PlayerSpawnPoint.h>

/// This class contains game data about a specific level. It's an entity in the scene so it can be data driven. 
/// @todo: figure out a way to automatically create a LevelInfo object when opening a scene if one doesn't already exist.
class RPG_LevelInfo : public RPG_BaseEntity
{
public:
  RPG_LevelInfo();

  static const VString& GetStaticKey();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  void Initialize() HKV_OVERRIDE;
  void DisposeObject() HKV_OVERRIDE;
  //void EditorThinkFunction() HKV_OVERRIDE;
  //void ThinkFunction() HKV_OVERRIDE;
  
  void SetInitialPlayerSpawnPoint(RPG_PlayerSpawnPoint* spawnPoint);
  RPG_PlayerSpawnPoint* GetInitialPlayerSpawnPoint() const;

  //@{
  // Minimap info
  VString GetMinimapImagePath() const;
  hkvVec2 GetMinimapImageDimensions() const;
  hkvVec2 GetMinimapImageOriginPosition() const;
  float GetMinimapImageScaleFudgeFactor() const;
  //@}

  void PostInitialize() HKV_OVERRIDE;

protected:
  // VisTypedEngineObject_cl
  //void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

  //void  OnVariableValueChanged (VisVariable_cl *pVar, const char *value) HKV_OVERRIDE;

  void PreloadFmodEventProject(VString const& projectFilename);

  //@{
  // Update methods
  // TODO - implement both server and client tick
  //virtual void ServerTick(float const deltaTime);
  //virtual void ClientTick(float const deltaTime);
  //@}
  
protected:

  VObjectReference<RPG_PlayerSpawnPoint> m_initialPlayerSpawnPoint;

  //@{
  // Minimap info
  VString m_minimapImagePath;             ///< path to the minimap image file associated with this level
  hkvVec2 m_minimapImageDimensions;       ///< dimensions of this level's minimap image file
  hkvVec2 m_minimapImageOriginPosition;   ///< x,y coordinates of the level origin on this level's minimap image file
  float m_minimapImageScaleFudgeFactor;   ///< optional 'fudge factor' to correct a scale error on the minimap image file
  //@}

  bool m_explosionManagerInitialized;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_LevelInfo, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_LevelInfo, RPG_PLUGIN_IMPEXP);
};

#endif // RPG_LEVEL_INFO_H__

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
