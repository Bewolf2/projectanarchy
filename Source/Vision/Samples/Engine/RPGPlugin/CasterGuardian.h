/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_CASTER_GUARDIAN_H__
#define RPG_PLUGIN_CASTER_GUARDIAN_H__

#include <Vision/Samples/Engine/RPGPlugin/AiCharacter.h>

/// Character class for the Caster Guardian
class RPG_CasterGuardian : public RPG_AiCharacter
{
public:
  // Constructor needs to be public for FORCE_LINKDYNCLASS on mobile
  RPG_CasterGuardian();

  VType* GetControllerComponentType() HKV_OVERRIDE;

  const VString& GetRangedAttackEffectName();
  const VString& GetHealingEffectName();

  float GetHealingThresholdPercentage() const;
  BOOL CanHealSelf() const;

  void PostInitialize() HKV_OVERRIDE;

protected:

  virtual void InitializeCharacterEffects() HKV_OVERRIDE;


  VString m_rangedAttackEffectFileName;                   ///< file name of the effect used for ranged attacks
  VString m_healingEffectFileName;                        ///< file name of the effect used for healing

  //@{
  // Healing State
  float m_healingThresholdPercentage;
  BOOL m_canHealSelf;
  //@}

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_CasterGuardian, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_CasterGuardian, RPG_PLUGIN_IMPEXP);
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
