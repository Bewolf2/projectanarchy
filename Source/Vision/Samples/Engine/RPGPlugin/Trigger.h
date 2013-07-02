/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_TRIGGER_H__
#define RPG_TRIGGER_H__

#include <Physics2012/Dynamics/Phantom/hkpAabbPhantom.h>

class RPG_Trigger;

/// Basic AabbPhantom class used internally for the trigger. Catches entry / exit of physics objects
/// @todo: WARNING: this may not work with async physics as it is currently, not entirely sure.
/// @todo: Add support for cylinders.
/// @todo: Take into account dying entities. If a trigger can ever do anything over time to an entity, we'll need to make sure the trigger knows about a dead entity.
class RPG_AabbPhantom : public hkpAabbPhantom
{
public:
  RPG_AabbPhantom(RPG_Trigger* triggerOwner, const hkAabb& aabb, hkUint32 collisionFilterInfo = 0);

  // hkpAabbPhantom implementation
  void addOverlappingCollidable(hkpCollidable* handle) HK_OVERRIDE;
  void removeOverlappingCollidable(hkpCollidable* handle) HK_OVERRIDE;

public:
  RPG_Trigger* m_trigger;
};

#include <Vision/Samples/Engine/RPGPlugin/BaseEntity.h>

/// Ai Spawn Point class. Has a data driven prefab to spawn, and can be hooked up to a trigger.
/// NOTE: this should only trigger on the Server or in a single player game.
class RPG_Trigger: public RPG_BaseEntity
{
public:
  RPG_Trigger();

  void OnOverlappingCollidableAdded(hkpCollidable *handle);
  void OnOverlappingCollidableRemoved(hkpCollidable *handle);

  void InitFunction() HKV_OVERRIDE;
  void DisposeObject() HKV_OVERRIDE;
  void EditorThinkFunction() HKV_OVERRIDE;
  void ThinkFunction() HKV_OVERRIDE;

  void PostInitialize() HKV_OVERRIDE;

protected:
  // VisTypedEngineObject_cl
  //void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

  //@{
  // Update methods
  // TODO - implement both server and client tick
  virtual void ServerTick(float const deltaTime);
  //virtual void ClientTick(float const deltaTime);
  //@}

  //@{
  // Trigger handling
  // Trigger Entry / Exit
  void ProcessEnter(VisBaseEntity_cl* entity);
  void ProcessInside(VisBaseEntity_cl* entity);
  void ProcessExit(VisBaseEntity_cl* entity);

  //@{
  // Subclasses can override these methods for custom functionality.
  virtual void OnEnter(VisBaseEntity_cl* entity) {}
  virtual void OnInside(VisBaseEntity_cl* entity) {}
  virtual void OnExit(VisBaseEntity_cl* entity) {}
  //@}

  bool CanTrigger() const;
  bool IsValidEntity(VisBaseEntity_cl* entity) const;

  //@}

protected:
  int m_maxTriggerCount;          ///< the max number of times this can trigger. 0 = infinite.
  int m_curTriggerCount;          ///< the current trigger count.
  float m_retriggerDelay;         ///< minimum time in seconds between triggering.
  BOOL m_acceptOnlyPlayers;       ///< when TRUE, only accept players.
  BOOL m_triggerOnInside;         ///< when TRUE, trigger an OnInside event in addition to the OnEnter / OnExit events.
  float m_lastEnteredTime;        ///< last time at which an entity entered and OnEnter was triggered.

  VSmartPtr<VisTriggerSourceComponent_cl> m_onEnterSource;    ///< Component that triggers the OnEnter event
  VSmartPtr<VisTriggerSourceComponent_cl> m_onExitSource;    ///< Component that triggers the OnExit event
  VSmartPtr<VisTriggerSourceComponent_cl> m_onInsideSource;   ///< Component that triggers the OnInside event

  hkvAlignedBBox m_aabb;                                        ///< Vision bounding box that is used to construct the havok Aabb phantom. Also used for debug / vForge rendering.
  RPG_AabbPhantom* m_aabbPhantom;

  VArray<VisBaseEntity_cl*> m_enteredEntities;
  VArray<VisBaseEntity_cl*> m_exitedEntities;
  VArray<VisBaseEntity_cl*> m_insideEntities;                ///< array of entities currently inside the trigger.

  //@{
  // Editor / Debug 
  BOOL m_displayDebug;                                          ///< set to TRUE and the trigger will be visible in the game, not just in the editor.
  VColorRef m_boundsDisplayColor;                               ///< The color to use when displaying the trigger bounds.
  float m_boundsDisplayLineWidth;                                 ///< the Width of the ine used when displaying the trigger bounds.
  //@}

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_Trigger, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_Trigger, RPG_PLUGIN_IMPEXP);
};

#endif // RPG_TRIGGER_H__

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
