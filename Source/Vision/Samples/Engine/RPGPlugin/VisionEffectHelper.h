/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_VISION_EFFECT_HELPER_H__
#define RPG_PLUGIN_VISION_EFFECT_HELPER_H__

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>

struct RPG_VisionEffectWallmarkParams
{
  char const *m_textureFilename;
  hkvVec3 m_position;
  hkvVec3 m_origin;
  float m_radius;
  float m_depth;
  float m_angle;
  float m_lifetime;
  float m_fadeoutTime;
};

struct RPG_VisionEffectParticleParams
{
  char const *m_effectFilename;
  hkvVec3 m_position;
  hkvVec3 m_orientation;
};

struct RPG_VisionEffectSoundParams
{
  char const *m_soundFilename;
  hkvVec3 m_position;
};

// Note: you should precache any referenced assets in the scene script file,
// otherwise you'll experience loading hitches
class RPG_VisionEffectHelper
{
public:

  //@{
  // Loading methods. Use these to cache / preload an effect
  static bool LoadTexture(char const* fileName);

  static bool LoadParticleEffect(char const* fileName);

  static bool LoadSoundEffect(char const* fileName);

  static bool LoadFmodEventGroup(VString const& eventGroupName);

  static bool LoadMesh(char const* fileName);
  //@}

  //@{
  // Effect creation
  static class VProjectedWallmark *CreateProjectedWallmark(RPG_VisionEffectWallmarkParams const& params);

  static class VisParticleEffect_cl *CreateParticleEffect(RPG_VisionEffectParticleParams const& params);
  static class VisParticleEffect_cl *CreateParticleEffect(VString const& particleFilename, hkvVec3 const& position = hkvVec3(0.f, 0.f, 0.f), hkvVec3 const& orientation = hkvVec3(0.f, 0.f, 0.f));
  //@}

  //@{
  // Sound playback
  static class VFmodSoundObject *CreateSoundEffect(RPG_VisionEffectSoundParams const& params);
  static bool PlaySoundFile(VString const& sfxFilename, hkvVec3 const& position = hkvVec3(0.f, 0.f, 0.f));
  static VFmodEvent* PlayFmodSoundEvent(VString const& eventGroupName, VString const& eventName, hkvVec3 const& position = hkvVec3(0.0f,0.0f,0.0f), int flags=VFMOD_FLAG_NONE);
  //@}

  //@{
  // Effect convenience functions
  static bool IsPersistentEffect(const VisParticleEffect_cl* effect);
  static bool IsLoopedFmodEvent(const VFmodEvent* event);
  //@}

private:

  static class VManagedResource *GetEffectResource(const char* resourceType, const char* szFilename);

};

#endif

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
