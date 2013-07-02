/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Action.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionHandler.h>
#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

float const s_faceToTargetTime = 1.0f;     ///< time in seconds to rotate to the target point while executing the action.
float const s_1DivFaceToTargetTime = 1.0f / s_faceToTargetTime;

RPG_Action::RPG_Action(RPG_ActionType_e identifier, RPG_Character *owner)
  : m_identifier(identifier)
  , m_characterOwner(owner)
  , m_flags(0)
  , m_interactionEntity(NULL)
  , m_interactionPosition()
  , m_startTime(-1.0f)
  , m_debugDisplayInfo(false)
{
}

void RPG_Action::Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition)
{
  // log this action's starting time
  m_startTime = Vision::GetTimer()->GetTime();

  // cache interaction entity and/or position
  m_interactionEntity = interactionEntity;
  m_interactionPosition = interactionPosition;
}

void RPG_Action::End(RPG_ActionType_e nextAction)
{
  VVERIFY_OR_RET(m_characterOwner);
  m_characterOwner->GetActionHandler().EndAction(nextAction);
}

void RPG_Action::Tick(float const deltaTime)
{
  ValidateInteractionEntity();
}

void RPG_Action::Cleanup(RPG_ActionType_e nextAction)
{
  m_interactionEntity = NULL;
  m_interactionPosition = hkvVec3(0, 0, 0);
}

void RPG_Action::UpdateFlags(const int flags)
{
  m_flags = flags;
}

void RPG_Action::UpdateInteraction(RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition)
{
}

bool RPG_Action::CanChainAction(RPG_ActionType_e const& nextAction) const
{
  return false;
}


bool RPG_Action::CanOverrideActionWith(RPG_ActionType_e const& newAction) const
{
  return false;
}


bool RPG_Action::CanThisActionOverride(RPG_ActionType_e const& actionToOverride) const
{
  return false;
}


bool RPG_Action::CanPerform(bool const forceCheck/*=FALSE*/) const
{
  return true;
}

void RPG_Action::FaceTargetPoint(const hkvVec3& targetPoint)
{
  // face the supplied target point
  float currentTime = Vision::GetTimer()->GetTime();
  float accumTime = (currentTime - m_startTime) * s_1DivFaceToTargetTime;

  if(accumTime > 0.0f &&
    accumTime <= 1.0f)
  {
    hkvVec3 const up(0.0f, 0.0f, 1.0f);
    hkvVec3 attackDirection = targetPoint - m_characterOwner->GetPosition();
    attackDirection.makeOrthogonalTo(up);

    if(attackDirection.normalizeIfNotZero() == HKV_SUCCESS)
    {
      hkvVec3 interpolatedDirection;
      RPG_ControllerUtil::CalcDirection(interpolatedDirection, m_characterOwner->GetDirection(), attackDirection, accumTime);

      m_characterOwner->SetDirection(interpolatedDirection);
    }
  }
}

bool RPG_Action::OwnerIsLocalPlayer() const
{
  return m_characterOwner->IsOfType(V_RUNTIME_CLASS(RPG_PlayerCharacter));  // @todo: when network play is implemented, test for locality as well
}

void RPG_Action::ValidateInteractionEntity()
{
  if(m_interactionEntity)
  {
    // XXX TODO
    // This will need to be refactored/expanded later to support actions that interact with entities
    // that do not have RPG_AttackableComponent attached, e.g. treasure chests or doors.
    VArray<RPG_DamageableEntity*> const& attackableEntities = RPG_GameManager::s_instance.GetAttackableEntities();
    if(attackableEntities.Find(static_cast<RPG_DamageableEntity*>(m_interactionEntity)) < 0)
    {
      m_interactionEntity = NULL;
    }
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
