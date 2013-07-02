/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/Effect.h>
#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>
#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

V_IMPLEMENT_SERIAL(RPG_Effect, RPG_BaseEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_Effect, RPG_BaseEntity, "Effect Entity Class", 0, "");
END_VAR_TABLE

RPG_Effect::RPG_Effect()
  : RPG_BaseEntity()
  , m_attachment(NULL)
  , m_parentEntity(NULL)

  , m_cameraShaking(false)
  , m_cameraShakeMaxAmplitude(0.f)
  , m_cameraShakeDuration(0.f)
  , m_cameraShakeStartTime(0.f)

  , m_debugDisplay(false)
{
}

void RPG_Effect::ThinkFunction()
{
  ClearVisualEffectIfDead();

  if (m_cameraShaking)
  {
    UpdateCameraShake(Vision::GetTimer()->GetTimeDifference());
  }

  DebugDrawEffectPosition();
}

void RPG_Effect::Create(RPG_EffectDefinition const& effectDefinition, VisBaseEntity_cl* parentEntity)
{
  VASSERT(parentEntity);
  m_parentEntity = parentEntity;

  bool validBone = false;
  if (!effectDefinition.m_vfxBoneName.IsEmpty())
  {
    if (m_parentEntity->GetMesh()->GetSkeleton()->GetBoneIndexByName(effectDefinition.m_vfxBoneName.AsChar()) != -1)
    {
      validBone = true;
    }
    else
    {
      // warn the user if a bad bone name has been supplied.
      VString msg;
      msg.Format("Trying to spawn an RPG_Effect on character %s, at a nonexistent bone: %s.", parentEntity->GetTypeId()->m_lpszClassName, effectDefinition.m_vfxBoneName.AsChar());
      Vision::Error.Warning(msg.AsChar());
    }
  }

  // attach this effect object to its parent entity, either at a bone or at its root
  if (validBone)
  {
    // valid bone.
    // create a proxy to attach the particle system to the named bone if none already exists
    if (!m_attachment)
    {
      m_attachment = new RPG_Attachment(m_parentEntity);
    }
    VASSERT(m_attachment);

    // attach the proxy to the parent bone
    m_attachment->Attach(this, effectDefinition.m_vfxBoneName, effectDefinition.m_vfxPositionOffset, effectDefinition.m_vfxOrientationOffset);
  }
  else
  {
    // attach this effect to its owning entity generally
    AttachToParent(m_parentEntity);
    ResetLocalTransformation(); // Set this RPG_Effect's local space position and orientation to (0/0/0)
  }

  UpdateBinding();  // Recompute the world space transformation from the current local space transformation

  // Create the effect. Sound and visual components will attach to this effect object, which is already attached to its parent.
  Create(effectDefinition, GetPosition(), GetOrientation());
}

void RPG_Effect::Create(RPG_EffectDefinition const& effectDefinition, hkvVec3 const& position /*= hkvVec3(0.f, 0.f, 0.f)*/, hkvVec3 const& orientation /*= hkvVec3(0.f, 0.f, 0.f)*/)
{
  m_debugDisplay = effectDefinition.m_displayDebugInfo;

  // if a visual effect for this type has been specified, play it
  if (!effectDefinition.m_vfxFilename.IsEmpty())
  {
    // see whether this effect has already been created for this character
    if (m_persistentParticleEffect && !m_persistentParticleEffect->IsDead())
    {
      m_persistentParticleEffect->Restart();
    }
    else
    {
      // if we weren't able to restart an existing effect, create a new one.
      VVERIFY_MSG(CreateVisualEffect(effectDefinition, position, orientation), "Tried to create an undefined visual effect.");
    }
  }

  // if an FMOD event exists for this effect, play it
  if (!effectDefinition.m_fmodEventGroupName.IsEmpty() && !effectDefinition.m_fmodEventName.IsEmpty())
  {
    VVERIFY_MSG(CreateSoundEffect(effectDefinition, position), "Tried to create an undefined sound effect.");
  }
  // otherwise, if a sound effect for this type has been specified, play it
  else if (!effectDefinition.m_sfxFilename.IsEmpty())
  {
    VVERIFY_MSG(RPG_VisionEffectHelper::PlaySoundFile(effectDefinition.m_sfxFilename, position), "Tried to play an undefined sound file.");
  }

  // if wallmark information exists for this effect, create the wallmark.
  if (!effectDefinition.m_wallmarkTextureFilename.IsEmpty())
  {
    CreateWallmark(effectDefinition);
  }

  if (effectDefinition.m_cameraShakeDuration > 0.f)
  {
    StartCameraShake(effectDefinition);
  }

  if (m_parentEntity && effectDefinition.m_shaderFlashDuration > 0.f) // shader flash requires a parent entity (obviously)
  {
    FlashShader(effectDefinition);
  }

  DebugDrawEffectPosition();
}

bool RPG_Effect::CreateVisualEffect(RPG_EffectDefinition const& effectDefinition, hkvVec3 const& position /*= hkvVec3(0.f, 0.f, 0.f)*/, hkvVec3 const& orientation /*= hkvVec3(0.f, 0.f, 0.f)*/)
{
  VString vfxFilename = effectDefinition.m_vfxFilename;

  if (m_debugDisplay)
  {
    VString msg;
    if (m_parentEntity)
    {
      msg += m_parentEntity->GetTypeId()->m_lpszClassName;
      msg += " --> ";
    }
    msg += "Creating Particle FX: ";
    msg += vfxFilename.AsChar();
    Vision::Error.SystemMessage(msg.AsChar());
    Vision::Message.Add(1, msg.AsChar());
  }

  VisParticleEffect_cl* particleEffect = RPG_VisionEffectHelper::CreateParticleEffect(vfxFilename, position, orientation);

  if (!particleEffect)
  {
    Vision::Error.Warning("Create Particle Effect failed: %s", vfxFilename.AsChar());
    return false;
  }

  particleEffect->AttachToParent(this);

  // apply position offset, relative to the RPG_Effect object's transform
  if (!effectDefinition.m_vfxPositionOffset.isZero())
  {
    particleEffect->SetLocalPosition(effectDefinition.m_vfxPositionOffset);
  }

  // apply orientation offset
  if (!effectDefinition.m_vfxOrientationOffset.isZero())
  {
    particleEffect->SetLocalOrientation(effectDefinition.m_vfxOrientationOffset);
  }
  // by default, emitters update their position over time. We need to call TeleportSpawnPosition() to force them to update immediately.
  particleEffect->TeleportSpawnPosition();

  // if this is a persistent effect, add it to the character's effect list.
  if (particleEffect && RPG_VisionEffectHelper::IsPersistentEffect(particleEffect))
  {
    if (GetParent())
    {
      particleEffect->SetRemoveWhenFinished(false); // if we're storing an effect here, we want to be responsible for its removal.
      m_persistentParticleEffect = particleEffect;
    }
    else
    {
      // a persistent effect was created, but not attached to a parent entity. Make some noise about this and shut it down.
      VString msg;
      msg.Format("Effect: %s is a looping effect, which must be attached to an entity capable of cleaning it up.", vfxFilename.AsChar());
      Vision::Error.Warning(msg.AsChar());
      //VASSERT_MSG(false, "Effects containing looping particle systems MUST be parented to an entity which can manage their shutdown and cleanup.");

      particleEffect->SetFinished();
    }
  }
  
  DebugDisplayParticleInformation(particleEffect);

  return true;
}

VisParticleEffect_cl* RPG_Effect::GetPersistentVisualEffect() const
{
  return m_persistentParticleEffect;
}

void RPG_Effect::Pause()
{
  if (m_persistentParticleEffect)
  {
    m_persistentParticleEffect->SetPause(true);
  }

  StopSound();
}

void RPG_Effect::StopSound()
{
  if (m_persistentFmodEvent && m_persistentFmodEvent->IsPlaying())
  {
    m_persistentFmodEvent->Stop();
  }
}

void RPG_Effect::Finish()
{
  // stop the effect and clear references to it
  if (m_persistentParticleEffect)
  {
    m_persistentParticleEffect->SetFinished();
    m_persistentParticleEffect = NULL;
  }

  StopSound();
  m_persistentFmodEvent = NULL;

  if (m_attachment)
  {
    m_attachment->Detach(false);
    V_SAFE_DELETE(m_attachment);
    m_attachment = NULL;
  }

  m_parentEntity = NULL;
}

void RPG_Effect::ClearVisualEffectIfDead()
{
  if (m_persistentParticleEffect && m_persistentParticleEffect->IsDead())
  {
    m_persistentParticleEffect = NULL;
  }
}

bool RPG_Effect::CreateSoundEffect(RPG_EffectDefinition const& effectDefinition, hkvVec3 const& position /*= hkvVec3(0.f, 0.f, 0.f)*/)
{
  if (m_debugDisplay)
  {
    VString msg;
    if (m_parentEntity)
    {
      msg += m_parentEntity->GetTypeId()->m_lpszClassName;
      msg += " --> ";
    }
    msg += "Playing Sound Event: ";
    msg += effectDefinition.m_fmodEventGroupName.AsChar();
    msg += "/";
    msg += effectDefinition.m_fmodEventName.AsChar();
    Vision::Error.SystemMessage(msg.AsChar());
    Vision::Message.Add(1, msg.AsChar());
  }

  VFmodEvent* event = RPG_VisionEffectHelper::PlayFmodSoundEvent(effectDefinition.m_fmodEventGroupName, effectDefinition.m_fmodEventName, position, VFMOD_FLAG_NODISPOSE);  // hang onto character events for reuse
  if (event)
  {
    event->AttachToParent(this);

    // if this is a looped sound event, we need to cache it on the character for manual shutdown.
    if (RPG_VisionEffectHelper::IsLoopedFmodEvent(event))
    {
      // looped sound
      m_persistentFmodEvent = event;
    }

    return true;
  }
  Vision::Error.Warning("Sound event failed to play: %s/%s", effectDefinition.m_fmodEventGroupName.AsChar(), effectDefinition.m_fmodEventName.AsChar());
  return false;
}

bool RPG_Effect::IsLoopingEffect() const
{
  return ((m_persistentParticleEffect && RPG_VisionEffectHelper::IsPersistentEffect(m_persistentParticleEffect)) ||
    (m_persistentFmodEvent && RPG_VisionEffectHelper::IsLoopedFmodEvent(m_persistentFmodEvent)));
}

void RPG_Effect::CreateWallmark(RPG_EffectDefinition const& effectDefinition)
{
  RPG_VisionEffectWallmarkParams wallmarkParams;
  {
    wallmarkParams.m_textureFilename = effectDefinition.m_wallmarkTextureFilename.AsChar();
    wallmarkParams.m_position = m_parentEntity->GetPosition();
    wallmarkParams.m_origin = wallmarkParams.m_position;
    wallmarkParams.m_origin.z += 150.f;
    wallmarkParams.m_angle = 360.f * Vision::Game.GetFloatRand();
    wallmarkParams.m_radius = 200.f + 30.f * Vision::Game.GetFloatRand();
    wallmarkParams.m_depth = wallmarkParams.m_radius * 2.f;
    wallmarkParams.m_lifetime = effectDefinition.m_wallmarkLifetime;
    wallmarkParams.m_fadeoutTime = effectDefinition.m_wallmarkFadeoutTime;

    VProjectedWallmark* wallmark = RPG_VisionEffectHelper::CreateProjectedWallmark(wallmarkParams);
    VASSERT_MSG(wallmark, "Wallmark creation failed.");
  }
}

void RPG_Effect::StartCameraShake(RPG_EffectDefinition const& effectDefinition)
{
  m_cameraShakeMaxAmplitude = effectDefinition.m_cameraShakeAmplitude;
  m_cameraShakeDuration = effectDefinition.m_cameraShakeDuration;
  m_cameraShakeStartTime = Vision::GetTimer()->GetTime();
  m_cameraShaking = true;
}

void RPG_Effect::UpdateCameraShake(float const deltaTime)
{
  VASSERT(m_cameraShaking);

  VisContextCamera_cl* camera = VisRenderContext_cl::GetCurrentContext()->GetCamera();
  VASSERT(camera);

  // end camera shake on timeout
  float const shakeTime = Vision::GetTimer()->GetTime() - m_cameraShakeStartTime;
  if (shakeTime >= m_cameraShakeDuration)
  {
    m_cameraShakeDuration = 0.f;
    m_cameraShakeStartTime = 0.f;
    m_cameraShakeMaxAmplitude = 0.f;
    m_cameraShaking = false;

    camera->ResetLocalTransformation();
    return;
  }

  // apply noise to the camera's position
  camera->ResetLocalTransformation();   // don't allow position updates to be cumulative
  hkvVec3 camPosition = camera->GetPosition();
  camPosition.x += GetCameraShakeNoise(camPosition.x);
  camPosition.y += GetCameraShakeNoise(camPosition.y);
  camPosition.z += GetCameraShakeNoise(camPosition.z);
  camera->SetPosition(camPosition);
}

float RPG_Effect::GetCameraShakeNoise(float const arg) const
{
  // simple linear attenuation as time runs out
  float const shakeTime = Vision::GetTimer()->GetTime() - m_cameraShakeStartTime;
  float const shakeScalar = (m_cameraShakeDuration - shakeTime)/m_cameraShakeDuration;
  VASSERT(0.f <= shakeScalar && shakeScalar <= 1.f);

  // @todo: if a cheapo random factor doesn't wind up looking good enough, replace this with a perlin noise generator
  return ((Vision::Game.GetFloatRand() * m_cameraShakeMaxAmplitude) - (m_cameraShakeMaxAmplitude / 2)) * shakeScalar;
}

void RPG_Effect::DisposeObject()
{
  // ensure that attachment is cleared in the event of a termination without Finish()
  if (m_attachment)
  {
    V_SAFE_DELETE(m_attachment);
    m_attachment = NULL;
  }

  RPG_BaseEntity::DisposeObject();
}

void RPG_Effect::FlashShader(RPG_EffectDefinition const& effectDefinition)
{
  VASSERT_MSG(m_parentEntity, "FlashShader cannot be called without a valid parent entity. (This entity must have a HighlightableComponent component as well.)");
  RPG_HighlightableComponent *highlightComp = static_cast<RPG_HighlightableComponent *> (m_parentEntity->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(RPG_HighlightableComponent)));
  if (highlightComp)
  {
    highlightComp->Flash(effectDefinition.m_shaderFlashColor, effectDefinition.m_shaderFlashDuration);
  }
}

void RPG_Effect::DebugDrawEffectPosition()
{
  if (!m_debugDisplay)
  {
    return;
  }

  const float crossSize = 100.0f;
  VColorRef crossColor = VColorRef(32, 32, 32);
  const VColorRef linkColor = VColorRef(208, 208, 24);

  // find the actual position of the effect elements and draw it
  hkvVec3 const& pos = GetPosition();
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(crossSize, 0.f, 0.f), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(-crossSize, 0.f, 0.f), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(0.f, crossSize, 0.f), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(0.f, -crossSize, 0.f), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(0.f, 0.f, crossSize), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(0.f, 0.f, -crossSize), crossColor);

  // draw a link to the parent if any
  if (m_parentEntity)
  {
    Vision::Game.DrawSingleLine(pos, m_parentEntity->GetPosition(), linkColor);
  }
}

void RPG_Effect::DebugDisplayParticleInformation(const VisParticleEffect_cl* particleEffect)
{
  if (!m_debugDisplay)
  {
    return;
  }

  const float crossSize = 100.0f;
  VColorRef crossColor = VColorRef(208, 24, 208);
  const VColorRef linkColor = VColorRef(24, 208, 208);

  // find the actual position of the effect elements and draw it
  hkvVec3 const& pos = particleEffect->GetPosition();
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(crossSize, 0.f, 0.f), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(-crossSize, 0.f, 0.f), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(0.f, crossSize, 0.f), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(0.f, -crossSize, 0.f), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(0.f, 0.f, crossSize), crossColor);
  Vision::Game.DrawSingleLine(pos, pos + hkvVec3(0.f, 0.f, -crossSize), crossColor);

  // draw a link to the parent if any
  if (m_parentEntity)
  {
    Vision::Game.DrawSingleLine(pos, GetPosition(), linkColor);
  }
}

RPG_EffectDefinition::RPG_EffectDefinition()
  : m_vfxFilename("")
  , m_vfxPositionOffset()
  , m_vfxOrientationOffset()
  , m_vfxBoneName()
  , m_wallmarkTextureFilename("")
  , m_wallmarkLifetime(0.f)
  , m_wallmarkFadeoutTime(0.f)
  , m_cameraShakeAmplitude(0.f)
  , m_cameraShakeDuration(0.f)
  , m_shaderFlashDuration(0.f)
  , m_shaderFlashColor()
  , m_fmodEventGroupName("")
  , m_fmodEventName("")
  , m_sfxFilename("")
  , m_attachEffect(true)
  , m_displayDebugInfo(false)
{
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
