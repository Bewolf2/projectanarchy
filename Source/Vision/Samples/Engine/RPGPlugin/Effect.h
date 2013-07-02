/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_EFFECT_H__
#define RPG_PLUGIN_EFFECT_H__

#include <Vision/Samples/Engine/RPGPlugin/Attachment.h>
#include <Vision/Samples/Engine/RPGPlugin/BaseEntity.h>

class VFmodEvent;
typedef VSmartPtr<VFmodEvent> VFmodEventPtr;

struct RPG_EffectDefinition
{
  RPG_EffectDefinition();

  VString m_vfxFilename;
  hkvVec3 m_vfxPositionOffset;
  hkvVec3 m_vfxOrientationOffset;
  VString m_vfxBoneName;

  VString m_wallmarkTextureFilename;
  float m_wallmarkLifetime;
  float m_wallmarkFadeoutTime;

  float m_cameraShakeAmplitude;
  float m_cameraShakeDuration;

  float m_shaderFlashDuration;
  VColorRef m_shaderFlashColor;

  VString m_fmodEventGroupName;
  VString m_fmodEventName;
  VString m_sfxFilename;

  bool m_attachEffect;

  bool m_displayDebugInfo;
};

class RPG_Effect : public RPG_BaseEntity
{
public:
  RPG_Effect();

  void Create(RPG_EffectDefinition const& effectDefinition, VisBaseEntity_cl* parentEntity);
  void Create(RPG_EffectDefinition const& effectDefinition, hkvVec3 const& position = hkvVec3(0.f, 0.f, 0.f), hkvVec3 const& orientation = hkvVec3(0.f, 0.f, 0.f));

  void Pause();
  void Finish();
  bool IsLoopingEffect() const;

  void DisposeObject() HKV_OVERRIDE;
  void ThinkFunction() HKV_OVERRIDE;

  VisParticleEffect_cl* GetPersistentVisualEffect() const;

protected:
  //@{
  // VFX
  bool CreateVisualEffect(RPG_EffectDefinition const& effectDefinition, hkvVec3 const& position = hkvVec3(0.f, 0.f, 0.f), hkvVec3 const& orientation = hkvVec3(0.f, 0.f, 0.f));
  void ClearVisualEffectIfDead();
  //@}

  //@{
  // SFX
  bool CreateSoundEffect(RPG_EffectDefinition const& effectDefinition, hkvVec3 const& position = hkvVec3(0.f, 0.f, 0.f));
  void StopSound();
  //@}

  //@{
  // Wallmarks
  void CreateWallmark(RPG_EffectDefinition const& effectDefinition);
  //@}

  //@{
  // Shader Flash
  void FlashShader(RPG_EffectDefinition const& effectDefinition);
  //@}

  //@{
  // Camera Effects
  void StartCameraShake(RPG_EffectDefinition const& effectDefinition);
  void UpdateCameraShake(float const deltaTime);
  float GetCameraShakeNoise(float const arg) const;
  //@}

  //@{
  // Debugging
  void DebugDrawEffectPosition();
  void DebugDisplayParticleInformation(const VisParticleEffect_cl* particleEffect);
  //@}

  VisParticleEffectPtr m_persistentParticleEffect;
  RPG_Attachment* m_attachment;
  VFmodEventPtr m_persistentFmodEvent;
  VisBaseEntity_cl* m_parentEntity;

  //@{
  // Camera Effects
  bool m_cameraShaking;
  float m_cameraShakeMaxAmplitude;
  float m_cameraShakeDuration;
  float m_cameraShakeStartTime;
  //@}

  bool m_debugDisplay;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_Effect, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_Effect, RPG_PLUGIN_IMPEXP);
};

#endif  // RPG_PLUGIN_EFFECT_H__

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
