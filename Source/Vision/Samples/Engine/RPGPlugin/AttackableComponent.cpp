/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/AttackableComponent.h>

#include <Vision/Samples/Engine/RPGPlugin/DamageableEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>

// RPG_AttackableComponent
RPG_AttackableComponent::RPG_AttackableComponent()
  : RPG_HighlightableComponent()
{
  SetColor(VColorRef(32, 32, 32));
}

V_IMPLEMENT_SERIAL(RPG_AttackableComponent, RPG_HighlightableComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_AttackableComponent, RPG_HighlightableComponent, "(RPG) Attackable", VVARIABLELIST_FLAGS_NONE, "(RPG) Attackable")  
END_VAR_TABLE

void RPG_AttackableComponent::SetOwner( VisTypedEngineObject_cl *owner )
{
  VisTypedEngineObject_cl* prevOwner = GetOwner();

  RPG_HighlightableComponent::SetOwner(owner);

  if(owner)
  {
    if(owner->IsFrom(RPG_DamageableEntity))
    {
      RPG_DamageableEntity* ownerEntity = static_cast<RPG_DamageableEntity*>(owner);
      RPG_GameManager::s_instance.AddAttackableEntity(ownerEntity);
    }
  }
  else if(prevOwner)
  {
    if(prevOwner->IsFrom(RPG_DamageableEntity))
    {
      RPG_DamageableEntity* ownerEntity = static_cast<RPG_DamageableEntity*>(prevOwner);
      RPG_GameManager::s_instance.RemoveAttackableEntity(ownerEntity);
    }
  }
  else
  {
    Vision::Error.Warning("AttackableComponent::SetOwner(): owner is null and previous owner was also null.");
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
