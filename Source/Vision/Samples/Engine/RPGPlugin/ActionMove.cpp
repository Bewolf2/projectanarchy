/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionMove.h>

#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

float const s_faceToTargetTime = 1.0f;     ///< time in seconds to rotate to the target while executing the attack.
float const s_1DivFaceToTargetTime = 1.0f / s_faceToTargetTime;

RPG_Action_Move::RPG_Action_Move(RPG_Character *owner)
  : RPG_Action(AT_Move, owner)
  , m_controllerComponent(NULL)
  , m_positionChanged(false)
{
}

void RPG_Action_Move::Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition)
{
  RPG_Action::Start(forced, prevAction, interactionEntity, interactionPosition);

  m_characterOwner->CreateCharacterEffect(FX_Moving);

  //Vision::Error.Warning("StartAction: Move");

  m_controllerComponent = m_characterOwner->GetController();

  m_positionChanged = true;
}

/// Perform Cleanup of this action
void RPG_Action_Move::Cleanup(RPG_ActionType_e nextAction)
{
  if(m_controllerComponent)
  {
    m_controllerComponent->CancelRequestedPath();
  }

  m_characterOwner->PauseCharacterEffect(FX_Moving);

  RPG_Action::Cleanup(nextAction);
}

void RPG_Action_Move::Tick(float const deltaTime)
{
  RPG_Action::Tick(deltaTime);

  if(!m_controllerComponent)
  {
    End();
  }

  if(m_interactionEntity &&
     !m_interactionPosition.isIdentical(m_interactionEntity->GetPosition()))
  {
    m_interactionPosition = m_interactionEntity->GetPosition();
    m_positionChanged = true;
  }

  hkReal const velocityThreshold = 0.05f;
  if(m_controllerComponent->GetSpeed() > velocityThreshold)
  {
    hkvVec3 const desiredDir = m_controllerComponent->GetDirection();

    hkvVec3 newDir;
    RPG_ControllerUtil::CalcDirection(newDir, m_characterOwner->GetDirection(), desiredDir, 0.5f * (30.f * deltaTime));
    m_characterOwner->SetDirection(newDir);
  }

  if(m_positionChanged)
  {
    m_controllerComponent->RequestPath(m_interactionPosition);
    m_positionChanged = false;
  }
  else if(m_flags & MF_AttackRange)
  {
    // if we only need to move within attack range, check for that
    if(m_interactionEntity && m_interactionEntity->IsFrom(RPG_DamageableEntity))
    {
      if(m_characterOwner->IsTargetWithinAttackRange(static_cast<RPG_DamageableEntity*>(m_interactionEntity)))
      {
        End();
      }
    }
    else
    {
      if(m_characterOwner->IsTargetWithinAttackRange(m_interactionPosition))
      {
        End();
      }
    }
  }
  else if(m_controllerComponent->GetSpeed() <= velocityThreshold)
  {
    End();
  }
}

void RPG_Action_Move::UpdateInteraction(RPG_BaseEntity* const interactionEntity, hkvVec3 const& interactionPosition)
{
  if(interactionEntity &&
     m_interactionEntity != interactionEntity)
  {
    m_interactionEntity = interactionEntity;
    m_interactionPosition = m_interactionEntity->GetPosition();
    m_positionChanged = true;
  }
  else if(!interactionEntity &&
          !m_interactionPosition.isIdentical(interactionPosition))
  {
    m_interactionEntity = NULL;
    m_interactionPosition = interactionPosition;
    m_positionChanged = true;
  }
}

bool RPG_Action_Move::CanChainAction(RPG_ActionType_e const& nextAction) const
{
  if(nextAction == AT_MeleeAttack)
  {
    return true;
  }

  return false;
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
