/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Physics2012/Vehicle/hkpVehicleInstance.h>
#include <Physics2012/Utilities/Actions/Reorient/hkpReorientAction.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

class Vehicle
{
public:
  Vehicle();
  ~Vehicle();

  void Init();
  void SetInput(float steering, float acceleration, bool brake, bool reverse, bool fixedControl);
  void Update();
  float GetMPH();
  void Reorient(bool enable);
  void DeInit();

private:
  void CacheEntities();
  void CacheEffects();
  void InitPhysics();
  void InitHUD();
  void InitSounds();

  void UpdateHUD();
  void UpdateWheels();
  void UpdateEffects();
  void UpdateSounds();
  void UpdateCameraEntity();
  void UpdateLight();

  void DeInitPhysics();
  void DeInitHUD();
  void DeInitSounds();

  static const int WHEEL_COUNT = 4;
  
  // Physics
  hkpWorld* m_world;
  hkpVehicleInstance* m_instance;
  hkpReorientAction* m_reorientAction;

  // Entities
  VisBaseEntity_cl* m_chassis;
  VisBaseEntity_cl* m_wheel[WHEEL_COUNT];
  VisBaseEntity_cl* m_camera;
  
  // Effects
  VisParticleEffect_cl* m_wheelFX[WHEEL_COUNT];
  VisParticleEffect_cl* m_tyremarkFX[WHEEL_COUNT];

  // HUD
  VSmartPtr<VisScreenMask_cl> m_speedometer;
  VSmartPtr<VisScreenMask_cl> m_speedometerArrow;

  // Sounds
  VFmodSoundObject* m_engineSound;
  hkReal m_engineSoundPitch;

  // Lights
  VisLightSource_cl* m_shadowLight;
  hkvVec3 m_shadowLightOffset;
};

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
