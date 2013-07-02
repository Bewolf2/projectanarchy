/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/ProjectileHealing.h>

#include <Vision/Samples/Engine/RPGPlugin/Character.h>

V_IMPLEMENT_SERIAL(RPG_Projectile_Healing, RPG_Projectile, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_Projectile_Healing, RPG_Projectile, "Healing Projectile Entity Class", 0, "");
END_VAR_TABLE

RPG_PLUGIN_IMPEXP RPG_Projectile_Healing::RPG_Projectile_Healing()
  : RPG_Projectile()
{
}

bool RPG_Projectile_Healing::ShouldHit(RPG_DamageableEntity* target)
{
  if(!target)
  {
    // pass through non-entities
    return false;
  }

  if(target == m_characterOwner)
  {
    // pass through firing character
    return false;
  }

  if(m_targetToTrack && target != m_targetToTrack)
  {
    // hit only tracked target
    return false;
  }

  if(target->IsFrom(RPG_Character))
  {
    RPG_Character* targetCharacter = static_cast<RPG_Character*>(target);
    if(targetCharacter->GetTeam() == m_characterOwner->GetTeam())
    {
      return true;
    }
  }

  return true;
}

void RPG_Projectile_Healing::OnHit(RPG_DamageableEntity* target, hkvVec3 const& position, hkvVec3 const& normal)
{
  RPG_Character* targetCharacter = static_cast<RPG_Character*>(target);
  if (targetCharacter)
  {
    targetCharacter->AddHealth(m_characterOwner->GetCharacterStats().GetHealingAmount());
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
