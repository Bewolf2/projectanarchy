/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ControllerComponent.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionUserMessages.h>

V_IMPLEMENT_SERIAL(RPG_ControllerComponent, RPG_BaseComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_ControllerComponent, RPG_BaseComponent, "(RPG) Controller", 0, "(RPG) Controller")
  DEFINE_VAR_FLOAT_AND_NAME(RPG_ControllerComponent, m_sensorSize, "SensorSize", "Character Sensor Size", "128", 0, NULL);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_ControllerComponent, m_desiredSpeed, "DesiredSpeed", "Character Desired Speed", "200", 0, NULL);
END_VAR_TABLE

namespace
{
  // AiWorldListener
  class AiWorldListener : public hkaiWorld::Listener
  {
  public:
    void AddController(RPG_ControllerComponent *controller);

    void RemoveController(RPG_ControllerComponent *controller);

  private:
    void AddListener();

    void RemoveListener();

    // hkaiWorld::Listener
    void postCharacterStepCallback(hkaiWorld::CharacterStepCallbackContext const& ctx) HK_OVERRIDE;

  private:
    VRefCountedCollection<RPG_ControllerComponent> m_controllers;
  }
  s_AiWorldListener;

  hkaiAvoidanceSolver::AvoidanceProperties const s_AvoidanceProperties;
}

// RPG_ControllerComponent
RPG_ControllerComponent::RPG_ControllerComponent()
  : RPG_BaseComponent(),
  m_characterOwner(NULL),
  m_cachedDirection(0.0f, 0.0f, 0.0f),
  m_cachedSpeed(0.0f),
  m_sensorSize(128.0f),
  m_desiredSpeed(200.0f)
{
}

void RPG_ControllerComponent::RequestPath(hkvVec3 const& dest)
{
  hkVector4 scaledDest;
  RPG_VisionHavokConversion::VisionToHavokPoint(dest, scaledDest);
  m_aiBehavior->requestPath(scaledDest);
}

void RPG_ControllerComponent::CancelRequestedPath()
{
  m_aiBehavior->cancelRequestedPath();
}

void RPG_ControllerComponent::SetSensorSize(float sensorSize)
{
  m_sensorSize = sensorSize;
}

void RPG_ControllerComponent::SetDesiredSpeed(float desiredSpeed)
{
  m_desiredSpeed = desiredSpeed;
}

void RPG_ControllerComponent::OnAfterCharacterStep(float deltaTime)
{
  hkVector4 const& aiVelocity = m_aiCharacter->getVelocity();

  // update m_cachedDirection and m_cachedSpeed
  RPG_VisionHavokConversion::HavokToVisionPoint(aiVelocity, m_cachedDirection);
  m_cachedSpeed = m_cachedDirection.getLengthAndNormalize();

  // calculate displacement for accumulation
  hkVector4 aiDisplacement;
  aiDisplacement.setMul(deltaTime, aiVelocity);

  hkvVec3 motionDelta;
  RPG_VisionHavokConversion::HavokToVisionPoint(aiDisplacement, motionDelta);

  // update character
  RPG_Character *controllerCharacter = static_cast<RPG_Character*>(GetOwner());
  {
    // vHavokCharacterController consumes motion delta; set world-space displacement from AI
    controllerCharacter->SetMotionDeltaWorldSpace(motionDelta);

    // Step vHavokCharacterController and update owning entity
    controllerCharacter->GetHavokCharacterControllerComponent()->Step(deltaTime, 1, deltaTime);
    controllerCharacter->GetHavokCharacterControllerComponent()->UpdateOwner();

    // Project Vision entity position to navmesh and update AI character
    hkVector4 characterPos;
    RPG_VisionHavokConversion::VisionToHavokPoint(controllerCharacter->GetPosition(), characterPos);
  
    m_aiCharacter->setPosition(characterPos);
    {
      hkaiWorld *const aiWorld = vHavokAiModule::GetInstance()->GetAiWorld();
      hkaiCharacter *const aiCharacter = m_aiCharacter;

      hkaiCharacterUtil::ProjectToMeshSettings projectToMeshSettings;
      hkaiCharacterUtil::integrateMotionAndProjectToMesh(0.0f, aiWorld, &aiCharacter, 1, projectToMeshSettings);
    }
  }
}

BOOL RPG_ControllerComponent::CanAttachToObject(VisTypedEngineObject_cl *object, VString& errorOut)
{
  if(!RPG_BaseComponent::CanAttachToObject(object, errorOut))
  {
    return FALSE;
  }

  bool const isCharacterEntity = (object->IsOfType(V_RUNTIME_CLASS(RPG_Character)) == TRUE);
  if(!isCharacterEntity)
  {
    errorOut = "RPG_Character (or derived) required";
    return FALSE;
  }

  return TRUE;
}

void RPG_ControllerComponent::SetOwner(VisTypedEngineObject_cl *owner)
{
  RPG_BaseComponent::SetOwner(owner);

  RPG_Character *characterOwner = static_cast<RPG_Character*>(GetOwner());

  if(characterOwner)
  {
    hkvVec3 const& pos = characterOwner->GetPosition();
    hkvVec3 const dir = characterOwner->GetDirection();

    hkaiCharacter::Cinfo characterCinfo;
    {
      RPG_VisionHavokConversion::VisionToHavokPoint(pos, characterCinfo.m_initialPosition);
      RPG_VisionHavokConversion::VisionToHavokDirection(dir, characterCinfo.m_initialForward);
      characterCinfo.m_up = hkVector4::getConstant<HK_QUADREAL_0010>();
      characterCinfo.m_avoidanceProperties = &s_AvoidanceProperties;
      {
        hkReal const radius = RPG_VisionHavokConversion::VisionToHavokScalar(characterOwner->GetCollisionRadius());
        hkReal const desiredSpeed = RPG_VisionHavokConversion::VisionToHavokScalar(m_desiredSpeed);
        hkReal const sensorSize = RPG_VisionHavokConversion::VisionToHavokScalar(m_sensorSize);
        hkReal const height = RPG_VisionHavokConversion::VisionToHavokScalar(characterOwner->GetCollisionHeight());
        {
          characterCinfo.m_radius = radius;
          characterCinfo.m_desiredSpeed = desiredSpeed;
        }
      }
    }

    m_aiCharacter = new hkaiCharacter(characterCinfo);
    m_aiCharacter->removeReference();

    hkaiPathFollowingBehavior::Cinfo behaviorCinfo;
    m_aiBehavior = new hkaiPathFollowingBehavior(m_aiCharacter, vHavokAiModule::GetInstance()->GetAiWorld(), behaviorCinfo);
    m_aiBehavior->removeReference();

    vHavokAiModule::GetInstance()->getCharacterBehaviors().pushBack(m_aiBehavior);

    s_AiWorldListener.AddController(this);
  }
  else
  {
    s_AiWorldListener.RemoveController(this);

    int const aiBehaviorIndex = vHavokAiModule::GetInstance()->getCharacterBehaviors().indexOf(m_aiBehavior);
    VASSERT(aiBehaviorIndex >= 0);
    if(aiBehaviorIndex >= 0)
    {
      vHavokAiModule::GetInstance()->getCharacterBehaviors().removeAt(aiBehaviorIndex);
    }

    m_aiBehavior = NULL;
    m_aiCharacter = NULL;
  }

  m_characterOwner = characterOwner;
}

// RPG_ControllerUtil
void RPG_ControllerUtil::CalcDirection(hkvVec3& resultDir, hkvVec3 const& currentDir, hkvVec3 const& desiredDir, float t)
{
  if(currentDir.dot(desiredDir) < 0.99f)
  {
    float const dot = currentDir.dot(desiredDir);
    float const theta = hkvMath::acosRad(dot) * hkvMath::clamp(t, 0.f, 1.f);
    hkvVec3 const vec = (desiredDir - currentDir * dot).getNormalized();
    resultDir = currentDir * hkvMath::cosRad(theta) + vec * hkvMath::sinRad(theta);
  }
  else
  {
    resultDir = desiredDir;
  }
}

bool RPG_ControllerUtil::GetClosestPointOnNavMesh(hkvVec3 const& position, float testRadius, hkvVec3& outPosition)
{
  VASSERT(vHavokAiModule::GetInstance()->GetAiWorld());

  hkaiWorld const *const aiWorld = vHavokAiModule::GetInstance()->GetAiWorld();
  if(!aiWorld)
    return false;

  hkVector4 inPos, outPos;
  RPG_VisionHavokConversion::VisionToHavokPoint(position, inPos);
  hkaiPackedKey faceKey = aiWorld->getDynamicQueryMediator()->getClosestPoint(inPos, testRadius, outPos);

  if(faceKey != HKAI_INVALID_PACKED_KEY)
  {
    RPG_VisionHavokConversion::HavokToVisionPoint(outPos, outPosition);
    return true;
  }

  return false;
}

bool RPG_ControllerUtil::IsPointOnNavmesh(hkvVec3 const& position, float const testRadius)
{
  hkvVec3 outPos;
  return GetClosestPointOnNavMesh(position, testRadius, outPos);
}

// AiWorldListener
void AiWorldListener::AddController(RPG_ControllerComponent *controller)
{
  VASSERT(controller);
  if(controller)
  {
    if(m_controllers.Count() == 0)
    {
      AddListener();
    }

    m_controllers.AddUnique(controller);
  }
}

void AiWorldListener::RemoveController(RPG_ControllerComponent *controller)
{
  VASSERT(controller);
  if(controller)
  {
    m_controllers.Remove(controller);

    if(m_controllers.Count() == 0)
    {
      RemoveListener();
    }
  }
}

void AiWorldListener::AddListener()
{
  vHavokAiModule::GetInstance()->GetAiWorld()->addListener(this);
}

void AiWorldListener::RemoveListener()
{
  vHavokAiModule::GetInstance()->GetAiWorld()->removeListener(this);
}

void AiWorldListener::postCharacterStepCallback(hkaiWorld::CharacterStepCallbackContext const& ctx)
{
  int const numControllers = m_controllers.Count();
  for(int i = 0; i < numControllers; i++)
  {
    RPG_ControllerComponent *const controller = m_controllers.GetAt(i);
    controller->OnAfterCharacterStep(ctx.m_timestep);
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
