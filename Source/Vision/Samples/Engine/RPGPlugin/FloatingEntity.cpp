/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/FloatingEntity.h>

#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>

V_IMPLEMENT_SERIAL(RPG_FloatingEntity, RPG_BaseEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_FloatingEntity, RPG_BaseEntity, "A floating prop.", 0, "")
  DEFINE_VAR_FLOAT_AND_NAME(RPG_FloatingEntity, m_maxYaw, "Max Yaw", "How far from the X axis will this object yaw?", "0.5", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_FloatingEntity, m_maxPitch, "Max Pitch", "How far does this object pitch down the X axis?", "2.5", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_FloatingEntity, m_maxRoll, "Max Roll", "How far does this roll around the X axis?", "1.0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_FloatingEntity, m_yawFrequency, "Yaw Frequency", "How fast does this object yaw?", "1.0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_FloatingEntity, m_pitchFrequency, "Pitch Frequency", "How fast does this object pitch down the X axis?", "2.0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_FloatingEntity, m_rollFrequency, "Roll Frequency", "How fast does this roll around the X axis?", "1.0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_FloatingEntity, m_bobAmplitude, "Bob Amplitude", "How far does this object rise and fall vertically?", "25.0", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_FloatingEntity, m_bobFrequency, "Bob Frequency", "How fast does this object rise and fall vertically?", "1.5", 0, 0);
END_VAR_TABLE

RPG_FloatingEntity::RPG_FloatingEntity() 
  : RPG_BaseEntity()
  , m_maxYaw(0.f)
  , m_maxPitch(0.f)
  , m_maxRoll(0.f)
  , m_yawFrequency(1.f)
  , m_pitchFrequency(1.f)
  , m_rollFrequency(1.f)
  , m_bobAmplitude(0.f)
  , m_bobFrequency(1.f)
  , m_baseOrientation()
  , m_basePosition()
  , m_yaw(0.f)
  , m_pitch(0.f)
  , m_roll(0.f)
  , m_bob(0.f)
{
}


#ifdef SUPPORTS_SNAPSHOT_CREATION
void RPG_FloatingEntity::GetDependencies(VResourceSnapshot &snapshot)
{
  RPG_BaseEntity::GetDependencies(snapshot);
}
#endif

void RPG_FloatingEntity::PostInitialize()
{
  m_baseOrientation = GetOrientation();
  m_basePosition = GetPosition();

  // randomize starting positions to prevent objects from looking too synchronous
  float const randomOffsetMax = 100.f;
  m_yaw = Vision::Game.GetFloatRand() * randomOffsetMax;
  m_pitch = Vision::Game.GetFloatRand() * randomOffsetMax;
  m_roll = Vision::Game.GetFloatRand() * randomOffsetMax;
  m_bob = Vision::Game.GetFloatRand() * randomOffsetMax;
}

void RPG_FloatingEntity::ThinkFunction()
{
  float const deltaTime = Vision::GetTimer()->GetTimeDifference();

  m_yaw += deltaTime * m_yawFrequency;
  m_pitch += deltaTime * m_pitchFrequency;
  m_roll += deltaTime * m_rollFrequency;
  m_bob += deltaTime * m_bobFrequency;

  hkvVec3 const newOrientation = hkvVec3(
    hkvMath::sinRad(m_yaw) * m_maxYaw + m_baseOrientation.x, 
    hkvMath::sinRad(m_pitch) * m_maxPitch + m_baseOrientation.y, 
    hkvMath::sinRad(m_roll) * m_maxRoll + m_baseOrientation.z
    );

  SetOrientation(newOrientation);

  hkvVec3 newPosition = hkvVec3(m_basePosition.x, m_basePosition.y, m_basePosition.z);
  newPosition.z += m_bobAmplitude * hkvMath::sinRad(m_bob);
  SetPosition(newPosition);
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
