/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ActionDie.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/AiControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>

#include <Animation/Physics2012Bridge/Instance/hkaRagdollInstance.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BlobShadow.hpp>

RPG_Action_Die::RPG_Action_Die(RPG_Character *owner)
  : RPG_Action(AT_Die, owner)
  , m_behaviorTriggerVarName("Dying")
  , m_behaviorFireEventName("DEATH_ANIMATION_BEGUN")
{
}

void RPG_Action_Die::Start(bool forced, RPG_ActionType_e prevAction, RPG_BaseEntity *interactionEntity, hkvVec3 const& interactionPosition)
{
  RPG_Action::Start(forced, prevAction, interactionEntity, interactionPosition);

  //Vision::Error.Warning("StartAction: Dying");
  VVERIFY_OR_RET(m_characterOwner);

  // remove the blob shadow component
  VBlobShadow *blobShadow = static_cast<VBlobShadow *>(m_characterOwner->Components().GetComponentOfType(V_RUNTIME_CLASS(VBlobShadow)));
  if(blobShadow)
  {
    m_characterOwner->RemoveComponent(blobShadow);
  }

  // remove the attackable component
  RPG_AttackableComponent* attackableComponent = static_cast<RPG_AttackableComponent*>(m_characterOwner->Components().GetComponentOfType(V_RUNTIME_CLASS(RPG_AttackableComponent)));
  if (attackableComponent)
  {
    m_characterOwner->RemoveComponent(attackableComponent);
  }

  // remove the controller component
  m_characterOwner->SetController(NULL);

  // remove the character controller component
  vHavokCharacterController* havokCharacterControllerComponent = static_cast<vHavokCharacterController*>(m_characterOwner->Components().GetComponentOfType(V_RUNTIME_CLASS(vHavokCharacterController)));
  if(havokCharacterControllerComponent)
  {
    m_characterOwner->RemoveComponent(havokCharacterControllerComponent);
  }

  // remove the collision mesh
  m_characterOwner->SetCollisionMesh(NULL);

  // play any visual and/or sound effects associated with this action
  m_characterOwner->CreateCharacterEffect(FX_Die);

  // spawn death pickups for this character (health and mana globes)
  m_characterOwner->SpawnDeathPickups();

  // grab the behavior component and tell it to set the dying variable
  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  VASSERT(!m_behaviorTriggerVarName.IsEmpty())
  behaviorComponent->SetBoolVar(m_behaviorTriggerVarName.AsChar(), true);  // tell the behavior component to execute the death event. Dying resets to false when the death anim is complete.
}

void RPG_Action_Die::Tick(float const deltaTime)
{
  RPG_Action::Tick(deltaTime);

  // Dying was set to true when death was initiated. Is reset to false by Behavior when the death anim finishes.
  vHavokBehaviorComponent *const behaviorComponent = m_characterOwner->GetBehaviorComponent();
  VVERIFY_OR_RET(behaviorComponent);

  VASSERT(!m_behaviorTriggerVarName.IsEmpty())
  if (!RPG_VisionHavokBehaviorHelper::BehaviorGetVarNamed<bool>(*behaviorComponent, m_behaviorTriggerVarName.AsChar()))
  {
    End();
  }
}

void RPG_Action_Die::OnHavokBehaviorEvent( const VString& eventName )
{
  if(eventName == m_behaviorFireEventName)
  {
    VASSERT(m_characterOwner);
    if(m_characterOwner)
    {
      vHavokBehaviorComponent* havokBehavior = m_characterOwner->GetBehaviorComponent();
      VASSERT(havokBehavior);
      VASSERT(havokBehavior->m_character);
      if(havokBehavior && havokBehavior->m_character)
      {
        hkaRagdollInstance* ragdoll = havokBehavior->m_character->getRagdoll();
        if(ragdoll)
        {
          hkpRigidBody* rootBoneBody = ragdoll->getRigidBodyOfBone(0);
          if(rootBoneBody)
          {
            hkvVec3 impulse = m_characterOwner->GetDeathImpulse();
            float magnitude = impulse.getLength();
            impulse.z = 0.0f;
            if(impulse.normalizeIfNotZero() != HKV_SUCCESS)
            {
              impulse = hkvVec3(0.0f, 0.0f, 1.0f);
            }
            else
            {
              impulse.z = 0.1f;
            }
            impulse *= magnitude;

            hkVector4 impulseHk;
            RPG_VisionHavokConversion::VisionToHavokPoint(impulse, impulseHk);
            impulseHk.mul(rootBoneBody->getMass());
            vHavokPhysicsModule::GetInstance()->MarkForWrite();
            rootBoneBody->applyLinearImpulse(impulseHk);
            vHavokPhysicsModule::GetInstance()->UnmarkForWrite();
          }
        }
      }
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
