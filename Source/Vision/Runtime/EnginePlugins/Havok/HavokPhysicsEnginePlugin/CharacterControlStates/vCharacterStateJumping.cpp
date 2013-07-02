/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterStateJumping.h>
#include <Physics2012/Utilities/CharacterControl/StateMachine/hkpCharacterContext.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/CharacterControlStates/vCharacterInput.h>

vCharacterStateJumping::vCharacterStateJumping() : hkpCharacterStateJumping()
{
}

void vCharacterStateJumping::change(hkpCharacterContext& context, const hkpCharacterInput& input, hkpCharacterOutput& output)
{
  if (((vCharacterInput*)input.m_userData)->m_fly)
  {
    context.setState(HK_CHARACTER_FLYING, input, output);
    return;
  }

  hkpCharacterStateJumping::change(context, input, output);
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
