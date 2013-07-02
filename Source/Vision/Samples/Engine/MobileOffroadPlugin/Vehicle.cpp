/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/MobileOffroadPlugin/MobileOffroadPCH.h>
#include <Vision/Samples/Engine/MobileOffroadPlugin/Vehicle.h>
#include <Vision/Samples/Engine/MobileOffroadPlugin/VehicleSetup.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

enum 
{
  GROUND_LAYER = 1,
  CHASSIS_LAYER,
  WHEEL_LAYER
};

Vehicle::Vehicle()
  : m_world(HK_NULL)
  , m_instance(HK_NULL)
  , m_reorientAction(HK_NULL)
  , m_chassis(HK_NULL)
  , m_camera(HK_NULL)
  , m_speedometer(NULL)
  , m_speedometerArrow(NULL)
  , m_engineSound(NULL)
  , m_shadowLight(NULL)
{
  for (int i=0; i<WHEEL_COUNT; ++i)
  {
    m_wheel[i] = NULL;
    m_wheelFX[i] = NULL;
    m_tyremarkFX[i] = NULL;
  }
}

Vehicle::~Vehicle()
{
}

void Vehicle::Init()
{
  // Cache
  CacheEntities();
  CacheEffects();

  // Init
  InitPhysics();
  InitHUD();
  InitSounds();
}

void Vehicle::Update()
{
  UpdateHUD();
  UpdateWheels();
  UpdateEffects();
  UpdateSounds();
  UpdateCameraEntity();
  UpdateLight();
}

float Vehicle::GetMPH()
{
  if (m_instance == HK_NULL)
    return 0.f;

  return m_instance->calcMPH();
}

void Vehicle::DeInit()
{
  DeInitPhysics();
  DeInitHUD();
  DeInitSounds();
}

void Vehicle::Reorient(bool enable)
{
  if (m_reorientAction == HK_NULL || m_world == HK_NULL)
    return;

  if (enable)
  {
    if (m_reorientAction->getWorld() == HK_NULL)
    {
      m_world->markForWrite();
      m_world->addAction(m_reorientAction);
      m_world->unmarkForWrite();
    }
  }
  else
  {
    if (m_reorientAction->getWorld() != HK_NULL)
    {
      m_world->markForWrite();
      m_world->removeAction(m_reorientAction);
      m_world->unmarkForWrite();
    }
  }
}

void Vehicle::CacheEntities()
{
  m_chassis = Vision::Game.SearchEntity("Cruiser");
  VASSERT(m_chassis);

  m_camera = Vision::Game.SearchEntity("Camera");
  VASSERT(m_camera);

  for (int wi = 0; wi < 4; wi ++)
  {
    VString entityName;
    entityName.Format("CruiserWheel%d", wi);

    m_wheel[wi] = Vision::Game.SearchEntity(entityName);
    VASSERT(m_wheel[wi]);
  }

  m_shadowLight = Vision::Game.SearchLightSource("CruiserShadow");
  VASSERT(m_shadowLight);
  m_shadowLightOffset = m_shadowLight->GetRotationMatrix().transformDirection(hkvVec3(2500.0f, 0.0f, 0.0f));
}

void Vehicle::CacheEffects()
{
  VisParticleEffectFile_cl* fxDustTrail = VisParticleGroupManager_cl::GlobalManager().LoadFromFile("Particles\\Car_DustTrail_Sand_SingleTire.xml");
  VASSERT(fxDustTrail);
  VisParticleEffectFile_cl* fxTyreMark = VisParticleGroupManager_cl::GlobalManager().LoadFromFile("Particles\\Car_Tyremark_Sand.xml");
  VASSERT(fxTyreMark);

  for (int wi = 0; wi < 4; wi ++)
  {
    m_wheelFX[wi] = fxDustTrail->CreateParticleEffectInstance(hkvVec3(0.f, 0.f, 0.f), hkvVec3(0.f, 0.f, 0.f), 1.0f);
    m_tyremarkFX[wi] = fxTyreMark->CreateParticleEffectInstance(hkvVec3(0.f, 0.f, 0.f), hkvVec3(0.f, 0.f, 0.f), 1.0f);

    // Tweak the particle color
    m_wheelFX[wi]->SetAmbientColor(VColorRef(75, 75, 75, 255));
    m_tyremarkFX[wi]->SetAmbientColor(VColorRef(75, 75, 75, 255));
  }
}

void Vehicle::InitPhysics()
{
  hkpRigidBody* rigidBody = HK_NULL;

  // Get HK Rigid Body from the entity
  {
    vHavokRigidBody *vRigidBody = (vHavokRigidBody *)m_chassis->Components().GetComponentOfType("vHavokRigidBody");

    // Check vHavokRigidBody creation
    VASSERT(vRigidBody != NULL);

    // Get rigid body
    rigidBody = vRigidBody->GetHkRigidBody();
    VASSERT(rigidBody != HK_NULL);
  }

  // Get World
  m_world = rigidBody->getWorld();
  m_world->markForWrite();

  // Create vehicle instance
  {
    // Setup rigid body
    rigidBody->setMotionType(hkpMotion::MOTION_BOX_INERTIA);

    // Specify control axis for our vehicle
    hkVector4      up( 0,  0,  1);
    hkVector4 forward( 1,  0,  0);
    hkVector4   right( 0, -1,  0);
    VehicleSetup setup(up, forward, right);

    m_instance = new hkpVehicleInstance(rigidBody);

    setup.buildVehicle(m_world, *m_instance);
    m_instance->m_wheelCollide->addToWorld(m_world);

    // Set collision filters
    m_instance->getChassis()->setCollisionFilterInfo(hkpGroupFilter::calcFilterInfo(CHASSIS_LAYER, 0));
    m_instance->m_wheelCollide->setCollisionFilterInfo(hkpGroupFilter::calcFilterInfo(WHEEL_LAYER, 0));

    m_world->addAction(m_instance);
  }

  // Add reorient action
  {
    hkVector4 rotationAxis(1.0f, 0.0f, 0.0f); 	 
    hkVector4 upAxis(0.0f, 0.0f, 1.0f);
    m_reorientAction = new hkpReorientAction(rigidBody, rotationAxis, upAxis);
  }

  m_world->unmarkForWrite();
}

void Vehicle::InitHUD()
{
  // Speedometer
  m_speedometer = new VisScreenMask_cl("Textures\\Speed.tga");
  m_speedometer->SetTransparency(VIS_TRANSP_ALPHA);
  m_speedometer->SetDepthWrite(FALSE);
  m_speedometer->SetFiltering(TRUE);

  // Speedometer's arrow counterpart
  m_speedometerArrow = new VisScreenMask_cl("Textures\\SpeedArrow.tga");
  m_speedometerArrow->SetTransparency(VIS_TRANSP_ALPHA);
  m_speedometerArrow->SetDepthWrite(FALSE);
  m_speedometerArrow->SetFiltering(TRUE);
  m_speedometerArrow->SetOrder(-10);

  // Move to bottom right corner
  float x, y;
  m_speedometerArrow->GetTargetSize(x, y);
  m_speedometerArrow->SetRotationCenter(x / 2, y / 2);
  m_speedometer->SetPos(32, Vision::Video.GetYRes() - y - 32);
  m_speedometerArrow->SetPos(32, Vision::Video.GetYRes() - y - 32);
}

void Vehicle::InitSounds()
{
  m_engineSound = VFmodManager::GlobalManager().LoadSoundResource("Sounds//EngineLoop.ogg", VFMOD_RESOURCEFLAG_2D)->CreateInstance(hkvVec3(0.f, 0.f, 0.f), VFMOD_FLAG_LOOPED | VFMOD_FLAG_NODISPOSE);
}

void Vehicle::SetInput(float steering, float acceleration, bool brake, bool reverse, bool fixedControl)
{
  if (m_instance == HK_NULL)
    return;

  // Drive the car with our input
  hkpVehicleDriverInputAnalogStatus* deviceStatus = (hkpVehicleDriverInputAnalogStatus*)m_instance->m_deviceStatus;

  deviceStatus->m_handbrakeButtonPressed = brake;
  deviceStatus->m_reverseButtonPressed = reverse;
  deviceStatus->m_positionY = acceleration;

  if (fixedControl)
  {
    deviceStatus->m_positionX = hkMath::clamp(steering, -1.f, 1.f);
  }
  else
  {
    const hkReal delta = Vision::GetTimer()->GetTimeDifference();
    deviceStatus->m_positionX = hkMath::clamp(deviceStatus->m_positionX + steering * delta, hkReal(-1.0f), hkReal(1.0f));

    if (hkMath::equal(steering, 0.f))
    {
      float difference = 2.0f * delta;
      deviceStatus->m_positionX += difference * (deviceStatus->m_positionX > 0 ? -1 : 1);

      if (hkMath::equal(deviceStatus->m_positionX, 0.f, difference * 1.5f))
      {
        deviceStatus->m_positionX = 0.f;
      }
    }
  }
}

void Vehicle::UpdateWheels()
{
  if (m_instance == HK_NULL)
    return;

  for (int wi = 0; wi < 4; wi ++)
  {
    if (m_wheel[wi] != NULL)
    {
      hkVector4 pos;
      hkQuaternion rot;
      m_instance->calcCurrentPositionAndRotation(m_instance->getChassis(), m_instance->m_suspension, wi, pos, rot);
      rot.normalize();
      hkTransform trans(rot, pos);

      // Convert to Vision space
      hkvVec3 position;
      hkvMat3 rotation;
      vHavokConversionUtils::PhysTransformToVisMatVec(trans, rotation, position);

      // Update wheel entity
      m_wheel[wi]->SetPosition(position);

      // Rotate the wheels on the left hand side of the car 180 degrees so their model is facing the right way
      if (wi % 2 == 1)
      {
        hkvMat3 flipWheelMatrix;
        flipWheelMatrix.setRotationMatrix(hkvVec3(1,0,0), 180.f);
        rotation = rotation.multiply(flipWheelMatrix);
      }
      
      m_wheel[wi]->SetRotationMatrix(rotation);
    }
  }
}

void Vehicle::UpdateHUD()
{
  if (m_speedometerArrow != NULL)
  {
    float minAngle = 90.f;
    float maxAngle = -270.f;
    float maxSpeed = 120.f;

    m_speedometerArrow->SetRotationAngle(minAngle + (maxAngle - minAngle) * (m_instance->calcMPH() / maxSpeed));
  }
}

void Vehicle::UpdateEffects()
{
  if (m_instance == HK_NULL)
    return;

  // Update particle effect position and intensity
  for (int wi = 0; wi < 4; wi ++)
  {
    if (m_wheel[wi] != NULL)
    {

      // Tyremarks
      m_instance->m_tyreMarks->updateTyremarksInfo(Vision::GetTimer()->GetTimeDifference(), m_instance);
      hkReal alpha = m_instance->m_tyreMarks->m_tyremarksWheel[wi]->getTyremarkPoint(0).m_pointLeft(3);

      if (alpha > 0.1f)
      {
        // Dust Particles
        float speedForFullIntesity = 30.f;
        float intensity = m_instance->calcMPH() / speedForFullIntesity;
        m_wheelFX[wi]->SetIntensity(intensity);

        if (m_wheelFX[wi]->IsPaused())
        {
          m_wheelFX[wi]->SetPause(false);
        }

        m_wheelFX[wi]->SetPosition(m_wheel[wi]->GetPosition());

        // Tyremarks
        if (m_tyremarkFX[wi]->IsPaused())
        {
          m_tyremarkFX[wi]->SetPause(false);
        }

        m_tyremarkFX[wi]->SetPosition(m_wheel[wi]->GetPosition());
      }
      else
      {
        // Turn Dust particles off
        if (m_wheelFX[wi]->IsPaused() == false)
        {
          m_wheelFX[wi]->SetPause(true);
        }

        // Turn Tyremarks off
        if (m_tyremarkFX[wi]->IsPaused() == false)
        {
          m_tyremarkFX[wi]->SetPause(true);
        }
      }
    }
  }
}

void Vehicle::UpdateSounds()
{
  if (m_instance == HK_NULL)
    return;

  // Calculate pitch with respect to the engine RPM and the current gear
  const hkReal damping = 10.0;
  hkReal pitch = 0.5f + ((m_instance->calcRPM() - 1000.f) / 4000.f);
  hkReal diff = Vision::GetTimer()->GetTimeDifference() * damping * (pitch - m_engineSoundPitch);

  // Apply with damping (to eliminate sudden pitch changes)
  if (hkMath::fabs(diff) < hkMath::fabs(pitch - m_engineSoundPitch))
  {
    m_engineSoundPitch += diff;
  }
  else
  {
    m_engineSoundPitch = pitch;
  }
  
  if (m_engineSound != NULL)
  {
    // Set the pitch of the engine sound to the calculated value
    m_engineSound->SetPitch(m_engineSoundPitch);
  }
}

void Vehicle::UpdateCameraEntity()
{
  static float previousX = 0.f;

  // Update position
  m_camera->SetPosition(m_chassis->GetPosition());

  // Update orientation
  hkvVec3 orientation = m_chassis->GetActualOrientation();
  hkpVehicleDriverInputAnalogStatus* deviceStatus = (hkpVehicleDriverInputAnalogStatus*)m_instance->m_deviceStatus;

  // Side to side tilt (Smooth)
  float inputX = previousX + (deviceStatus->m_positionX - previousX) * 0.2f;
  previousX = inputX;
  orientation.z += inputX * 40.f;

  // Apply orientation
  m_camera->SetOrientation(orientation);
}

void Vehicle::UpdateLight()
{
  m_shadowLight->SetPosition(m_chassis->GetPosition() - m_shadowLightOffset);
}

void Vehicle::DeInitPhysics()
{
  m_world->markForWrite();

  // Remove wheel collide from world
  m_instance->m_wheelCollide->removeFromWorld();

  // Remove and clean up the vehicle instance
  //m_world->removeAction(m_instance); // Already removed in Plugin
  delete m_instance;
  m_instance = HK_NULL;

  // Remove reorient action
  if (m_reorientAction->getWorld() != HK_NULL)
  {
    m_world->removeAction(m_reorientAction);
  }
  delete m_reorientAction;
  m_reorientAction = HK_NULL;

  m_world->unmarkForWrite();
}

void Vehicle::DeInitHUD()
{
  if (m_speedometer != NULL)
  {
    m_speedometer->Unload();
    m_speedometer = NULL;
  }
  
  if (m_speedometerArrow != NULL)
  {
    m_speedometerArrow->Unload();
    m_speedometerArrow = NULL;
  }
}

void Vehicle::DeInitSounds()
{
  if (m_engineSound != NULL)
  {
    m_engineSound->Stop();
    m_engineSound->DisposeObject();
    m_engineSound = NULL;
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
