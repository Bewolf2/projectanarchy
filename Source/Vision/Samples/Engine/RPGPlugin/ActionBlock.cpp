/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionBlock.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

RPG_Action_Block::RPG_Action_Block(RPG_Character *owner)
  : RPG_Action(AT_Block, owner)
  , m_behaviorTriggerVarName("Blocking")
{
}

void RPG_Action_Block::Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition)
{
  RPG_Action::Start(forced, prevAction, interactionEntity, interactionPosition);

  //Vision::Error.SystemMessage("StartAction: Block");

  // cancel any ongoing move
  VVERIFY_OR_RET(m_characterOwner);
  m_characterOwner->GetController()->CancelRequestedPath();

  // play any visual and/or sound effects associated with this action
  m_characterOwner->CreateCharacterEffect(FX_Block);

  // grab the behavior component and tell it to set the Block variable
  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  VASSERT(!m_behaviorTriggerVarName.IsEmpty());
  behaviorComponent->SetBoolVar(m_behaviorTriggerVarName.AsChar(), true);  // tell the behavior component to execute the block event. Blocking resets to false when the block anim is complete.
}

void RPG_Action_Block::Tick(float const deltaTime)
{
  RPG_Action::Tick(deltaTime);

  // Blocking was set to true when block was initiated. Is reset to false by Behavior when the block anim finishes.
  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  VASSERT(!m_behaviorTriggerVarName.IsEmpty());
  if (!RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, m_behaviorTriggerVarName.AsChar()))
  {
    End();
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
