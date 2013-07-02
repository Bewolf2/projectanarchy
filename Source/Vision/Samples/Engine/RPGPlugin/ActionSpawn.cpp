/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionSpawn.h>

// TODO Spawn bug hack
#include <Vision/Samples/Engine/RPGPlugin/Boss.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

namespace
{
  // TODO Spawn bug hack
  float const kHackSpawnVisibilityTime = 0.1f;
}

RPG_Action_Spawn::RPG_Action_Spawn(RPG_Character *owner)
  : RPG_Action(AT_Spawn, owner)
  , m_behaviorTriggerVarName("Spawning")
  , m_animationPlayed(false)
	, m_animStartTime(0.0f)
{
}

void RPG_Action_Spawn::Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition)
{
  RPG_Action::Start(forced, prevAction, interactionEntity, interactionPosition);
  VVERIFY_OR_RET(m_characterOwner);
  m_animationPlayed = false;

  // cancel any ongoing move
  m_characterOwner->GetController()->CancelRequestedPath();

  // play any visual and/or sound effects associated with this action
  m_characterOwner->CreateCharacterEffect(FX_Spawn);

  m_characterOwner->SetVisible(false);
}

void RPG_Action_Spawn::Tick(float const deltaTime)
{
  RPG_Action::Tick(deltaTime);

  VVERIFY_OR_RET(m_characterOwner);
  // TODO Boss spawning doesn't work properly
  if(m_characterOwner->IsFrom(RPG_Boss))
  {
    m_characterOwner->SetVisible(true);
    End();
    return;
  }

  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  float const currTime = Vision::GetTimer()->GetTime();
  if (m_animationPlayed)
  {
    // TODO Spawn bug hack
    VASSERT(m_characterOwner);
    if(m_characterOwner && (currTime - m_animStartTime > kHackSpawnVisibilityTime))
    {
      m_characterOwner->SetVisible(true);
    }

    //  was set to true when the action was initiated. Is reset to false by Behavior when the anim finishes.
    VASSERT(!m_behaviorTriggerVarName.IsEmpty());
    if (!RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, m_behaviorTriggerVarName.AsChar()))
    {
      // TODO Spawn bug hack
      if(m_characterOwner)
      {
        m_characterOwner->SetVisible(true);
      }
      End();
    }
  }
  else
  {
    if (currTime - m_startTime >= m_characterOwner->GetCharacterActionData().GetSpawnAnimationDelay())
    {
      m_animStartTime = currTime;
      PlayAnimation();
    }
  }
}

void RPG_Action_Spawn::PlayAnimation()
{
  // grab the behavior component and tell it to set the associated variable
  VASSERT(!m_behaviorTriggerVarName.IsEmpty());
  m_characterOwner->GetBehaviorComponent()->SetBoolVar(m_behaviorTriggerVarName.AsChar(), true);  // tell the behavior component to execute the associated animation. Resets to false when the anim is complete.

  m_animationPlayed = true;
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
