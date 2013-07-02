/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/DamageableEntity.h>

V_IMPLEMENT_SERIAL(RPG_DamageableEntity, RPG_BaseEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_DamageableEntity, RPG_BaseEntity, "An entity that can take damage.", 0, "")
  DEFINE_VAR_FLOAT_AND_NAME(RPG_DamageableEntity, m_collisionRadius, "Collision Radius", "Collision shape radius", "100", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(RPG_DamageableEntity, m_collisionHeight, "Collision Height", "Collision shape height", "200", 0, 0);
END_VAR_TABLE


RPG_DamageableEntity::RPG_DamageableEntity() 
  : RPG_BaseEntity()
  , m_collisionRadius(100.f)
  , m_collisionHeight(200.f)
{
}

int RPG_DamageableEntity::TakeDamage(const int damageAmount, const hkvVec3& impactVelocity /*= hkvVec3(0,0,0)*/)
{
  // Since RPG_DamageableEntity has no concept of damage, this is ignored and implementation is left to derived classes.
  return 0;
}

void RPG_DamageableEntity::ApplyKnockBack( const hkvVec3& knockBackVector, const float duration )
{
  // Since RPG_DamageableEntity has no concept of knockback, this is ignored and implementation is left to derived classes.
  return;
}

float RPG_DamageableEntity::GetCollisionRadius() const
{
  return m_collisionRadius;
}

float RPG_DamageableEntity::GetCollisionHeight() const
{
  return m_collisionHeight;
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
