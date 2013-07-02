/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>

#include <Vision/Samples/Engine/RPGPlugin/Trigger.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

RPG_AabbPhantom::RPG_AabbPhantom(RPG_Trigger* triggerOwner, const hkAabb& aabb, hkUint32 collisionFilterInfo)
  : hkpAabbPhantom(aabb, collisionFilterInfo)
  , m_trigger(triggerOwner)
{
}

void RPG_AabbPhantom::addOverlappingCollidable(hkpCollidable* handle)
{
  m_trigger->OnOverlappingCollidableAdded(handle);

  hkpAabbPhantom::addOverlappingCollidable(handle);
}

void RPG_AabbPhantom::removeOverlappingCollidable(hkpCollidable* handle)
{
  m_trigger->OnOverlappingCollidableRemoved(handle);

  hkpAabbPhantom::removeOverlappingCollidable(handle);
}

V_IMPLEMENT_SERIAL(RPG_Trigger, RPG_BaseEntity, 0, &g_RPGPluginModule);

// inherit the var table from RPG_BaseEntity because TriggerBoxEntity_cl doesn't have one and throws linker errors
START_VAR_TABLE(RPG_Trigger, RPG_BaseEntity, "RPG Trigger Entity", 0, "")
  DEFINE_VAR_INT_AND_NAME(RPG_Trigger, m_maxTriggerCount, "Max Trigger Count", "The maximum number of times this can trigger before disabling.", "1", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Trigger, m_retriggerDelay, "Retrigger Delay", "The minimum time allowed between trigger events.", "0.5", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Trigger, m_acceptOnlyPlayers, "Accept Only Players", "Enable to accept only player entities as instigators.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Trigger, m_triggerOnInside, "Trigger OnInside event", "Enable to fire the OnInside event every tick while an entity is inside this trigger.", "FALSE", 0, 0);
  DEFINE_VAR_BOOL_AND_NAME(RPG_Trigger, m_displayDebug, "Debug Display", "Enable this and the trigger will draw in the game, not just in the Editor.", "FALSE", 0, 0);
  DEFINE_VAR_COLORREF_AND_NAME(RPG_Trigger, m_boundsDisplayColor, "Bounds Color", "The color of the bounds that are drawn for this trigger.", "127,127,0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_Trigger, m_boundsDisplayLineWidth, "Bounds Line Width", "The bounds of the width drawn for this trigger.", "1.0", 0, 0);
END_VAR_TABLE


RPG_Trigger::RPG_Trigger()
  : RPG_BaseEntity()
  , m_maxTriggerCount(1)
  , m_curTriggerCount(0)
  , m_retriggerDelay(0.5f)
  , m_acceptOnlyPlayers(FALSE)
  , m_triggerOnInside(FALSE)
  , m_lastEnteredTime(0.0f)
  , m_aabbPhantom(NULL)
  , m_insideEntities()
  , m_displayDebug(FALSE)
  , m_boundsDisplayColor(127, 127, 0)
  , m_boundsDisplayLineWidth(1.0f)
{
}

void RPG_Trigger::OnOverlappingCollidableAdded(hkpCollidable *handle)
{
  // check for phantoms (character proxy objects are phantoms)
  if(handle->getType() == hkpWorldObject::BROAD_PHASE_PHANTOM)
  {
    hkpEntity* hkEntity = static_cast<hkpEntity*>(handle->getOwner());

    vHavokUserDataType_e eType = V_USERDATA_UNDEFINED;
    void *pObject = vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void*)hkEntity->getUserData(), eType);

    if (pObject && eType==V_USERDATA_OBJECT)
    {
      // pObject should be a VisTypedEngineObject_cl.
      VisTypedEngineObject_cl* object = (VisTypedEngineObject_cl*)pObject;

      if(object->IsOfType(V_RUNTIME_CLASS(vHavokCharacterController)))
      {
        m_enteredEntities.Add((VisBaseEntity_cl*)(static_cast<vHavokCharacterController*>(object)->GetOwner()));
      }
    }
  }
}

void RPG_Trigger::OnOverlappingCollidableRemoved(hkpCollidable *handle)
{
  // check for phantoms (character proxy objects are phantoms)
  if(handle->getType() == hkpWorldObject::BROAD_PHASE_PHANTOM)
  {
    hkpEntity* hkEntity = static_cast<hkpEntity*>(handle->getOwner());

    vHavokUserDataType_e eType = V_USERDATA_UNDEFINED;
    void *pObject = vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void*)hkEntity->getUserData(), eType);

    if (pObject && eType==V_USERDATA_OBJECT)
    {
      // pObject should be a VisTypedEngineObject_cl.
      VisTypedEngineObject_cl* object = (VisTypedEngineObject_cl*)pObject;

      if(object->IsOfType(V_RUNTIME_CLASS(vHavokCharacterController)))
      {
        m_exitedEntities.Add((VisBaseEntity_cl*)(static_cast<vHavokCharacterController*>(object)->GetOwner()));
      }
    }
  }
}

// Deliberate use of InitFunction: I want to add these components in vForge/CreateEntity; otherwise serialization will create them
void RPG_Trigger::InitFunction()
{
  RPG_BaseEntity::InitFunction();

  m_onEnterSource = new VisTriggerSourceComponent_cl("OnEnter", VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
  AddComponent(m_onEnterSource);

  m_onExitSource = new VisTriggerSourceComponent_cl("OnExit", VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
  AddComponent(m_onExitSource);

  m_onInsideSource = new VisTriggerSourceComponent_cl("OnInside", VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT);
  AddComponent(m_onInsideSource);
}

void RPG_Trigger::DisposeObject()
{
  if(m_aabbPhantom)
  {
    vHavokPhysicsModule::GetInstance()->MarkForWrite();
    {
      vHavokPhysicsModule::GetInstance()->GetPhysicsWorld()->removePhantom(m_aabbPhantom);
      m_aabbPhantom->removeReference();
    }
    vHavokPhysicsModule::GetInstance()->UnmarkForWrite();

    m_aabbPhantom = NULL;
  }

  m_insideEntities.RemoveAll();

  RPG_BaseEntity::DisposeObject();
}

/// Used for Editor drawing
void RPG_Trigger::EditorThinkFunction()
{
  if(!Vision::Editor.IsPlayingTheGame() || m_displayDebug)
  {
    // draw the bounding box if we're in the editor and not playing the game, or in the editor and m_displayDebug is TRUE
    // DrawBoundingBox(TRUE, m_boundsDisplayColor, m_boundsDisplayLineWidth);
  }
}

void RPG_Trigger::ThinkFunction()
{
  if(!Vision::Editor.IsInEditor() && m_displayDebug)
  {
    // draw the bounding box if we aren't in the editor but want to draw debug info.
    DrawBoundingBox(TRUE, m_boundsDisplayColor, m_boundsDisplayLineWidth);
  }

  const float deltaTime = Vision::GetTimer()->GetTimeDifference();

  // TODO - implement both server and client tick
  ServerTick(deltaTime);
}

void RPG_Trigger::ServerTick(float const deltaTime)
{
  // Entered
  for(int i = 0; i < m_enteredEntities.GetSize(); ++i)
  {
    VisBaseEntity_cl *const entity = m_enteredEntities.GetAt(i);
    ProcessEnter(entity);
  }
  m_enteredEntities.RemoveAll();

  // Exited
  for(int i = 0; i < m_exitedEntities.GetSize(); ++i)
  {
    VisBaseEntity_cl *const entity = m_exitedEntities.GetAt(i);
    ProcessExit(entity);
  }
  m_exitedEntities.RemoveAll();

  for(int index = 0; index < m_insideEntities.GetSize(); ++index)
  {
    VisBaseEntity_cl* entity = m_insideEntities.GetAt(index);

    ProcessInside(entity);
  }
}

void RPG_Trigger::PostInitialize()
{
  RPG_BaseEntity::PostInitialize();

  if(Vision::Editor.IsPlayingTheGame() && !m_displayDebug)
  {
    // Trigger should be invisible during the game unless we're displaying the mesh for debug purposes.
    SetVisibleBitmask(VIS_ENTITY_INVISIBLE);
  }

  // create a box based off of the entity scaling. a scale of (1,1,1) = .001 meter cube in havok units.
  
  m_aabb.setInvalid();

  m_aabb.set(-GetScaling() * 0.5f, GetScaling() * 0.5f);

  hkvMat4 aabbTransform(GetRotationMatrix(), GetPosition());
  m_aabb.transformFromOrigin(aabbTransform);

  hkVector4 minBoundary;
  RPG_VisionHavokConversion::VisionToHavokPoint(m_aabb.m_vMin, minBoundary);

  hkVector4 maxBoundary;
  RPG_VisionHavokConversion::VisionToHavokPoint(m_aabb.m_vMax, maxBoundary);

  // Create the AABB
  hkAabb fixedAabb;
  fixedAabb.setEmpty();
  fixedAabb.m_min = minBoundary;
  fixedAabb.m_max = maxBoundary;

  // Create the phantom
  m_aabbPhantom = new RPG_AabbPhantom(this, fixedAabb);

  // Set entity type for the triggers' data collection
  //m_aabbPhantom->setUserData( (hkUlong)this );

  // Add the phantom to the world
  
  vHavokPhysicsModule::GetInstance()->MarkForWrite();
  {
    vHavokPhysicsModule::GetInstance()->GetPhysicsWorld()->addPhantom(m_aabbPhantom);
  }
  vHavokPhysicsModule::GetInstance()->UnmarkForWrite();


  // Set color
  //HK_SET_OBJECT_COLOR((hkUlong)fixedPhantomCallbackShapeRB->getCollidable(), m_fixedPhantomCallbackShape->m_colourNoHit);

  // Cache trigger sources: note that these may not exist in serialized scenes if nothing referenced them
  if(!m_onEnterSource)
    m_onEnterSource = static_cast<VisTriggerSourceComponent_cl*>(Components().GetComponentOfTypeAndName(V_RUNTIME_CLASS(VisTriggerSourceComponent_cl), "OnEnter"));

  if(!m_onExitSource)
    m_onExitSource = static_cast<VisTriggerSourceComponent_cl*>(Components().GetComponentOfTypeAndName(V_RUNTIME_CLASS(VisTriggerSourceComponent_cl), "OnExit"));

  if(!m_onInsideSource)
    m_onInsideSource = static_cast<VisTriggerSourceComponent_cl*>(Components().GetComponentOfTypeAndName(V_RUNTIME_CLASS(VisTriggerSourceComponent_cl), "OnInside"));
}

/// Handles an entity entering the Trigger area.
void RPG_Trigger::ProcessEnter(VisBaseEntity_cl* entity)
{
  if(CanTrigger() &&
     IsValidEntity(entity))
  {
    m_lastEnteredTime = Vision::GetTimer()->GetTime();

    if(m_maxTriggerCount > 0)
    {
      ++m_curTriggerCount;
    }

    m_insideEntities.Add(entity);

    OnEnter(entity);
    if(m_onEnterSource)
      m_onEnterSource->TriggerAllTargets();
    TriggerScriptEvent("OnEnter", "*o", entity);
  }
}

/// Handles an entity inside the Trigger area.
void RPG_Trigger::ProcessInside(VisBaseEntity_cl* entity)
{
  OnInside(entity);
  if(m_onInsideSource)
    m_onInsideSource->TriggerAllTargets();
  TriggerScriptEvent("OnInside", "*o", entity);
}

/// Handles an entity leaving the Trigger area.
void RPG_Trigger::ProcessExit(VisBaseEntity_cl* entity)
{
  // check if this entity is in inside the trigger.
  int foundIndex = m_insideEntities.Find(entity);

  if(foundIndex > -1)
  {
    m_insideEntities.RemoveAt(foundIndex);

    OnExit(entity);
    if(m_onExitSource)
      m_onExitSource->TriggerAllTargets();
    TriggerScriptEvent("OnExit", "*o", entity);
  }
}

bool RPG_Trigger::CanTrigger() const
{
  if(m_retriggerDelay > Vision::GetTimer()->GetTime() - m_lastEnteredTime)
  {
    // we can't trigger again until enough time has passed
    return false;
  }

  if(m_maxTriggerCount > 0 &&
    m_curTriggerCount >= m_maxTriggerCount)
  {
    // we can't trigger again if we're already past our max
    return false;
  }

  return true;
}

/// Checks if this is a valid entity for triggering.
bool RPG_Trigger::IsValidEntity(VisBaseEntity_cl* entity) const
{
  if(!entity)
  {
    return false;
  }

  // make sure this entity isn't Entering again before leaving.
  if(m_insideEntities.Find(entity) > -1)
  {
    //VASSERT_MSG(false, "RPG_Trigger::IsValidEntity - Entity is trying to Enter more than once before Leaving!");
    return false;
  }


  bool canTrigger = false;

  if(m_acceptOnlyPlayers)
  {
    // make sure this is a player
    if(entity->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_PlayerControllerComponent)))
    {
      canTrigger = true;
    }
  }
  else
  {
    // go ahead if we have no restrictions
    canTrigger = true;
  }

  return canTrigger;
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
