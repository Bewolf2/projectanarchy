/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_PROJECTILE_HEALING_H__
#define RPG_PLUGIN_PROJECTILE_HEALING_H__

#include <Vision/Samples/Engine/RPGPlugin/Projectile.h>

/// Basic projectile class. Moves using vHavokRigidBody and can deal damage upon impact.
class RPG_Projectile_Healing : public RPG_Projectile
{
public:
  RPG_PLUGIN_IMPEXP RPG_Projectile_Healing();

protected:
  virtual bool ShouldHit(RPG_DamageableEntity* target) HKV_OVERRIDE;
  virtual void OnHit(RPG_DamageableEntity* target, hkvVec3 const& position, hkvVec3 const& normal) HKV_OVERRIDE;

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_Projectile_Healing, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_Projectile_Healing, RPG_PLUGIN_IMPEXP);
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
