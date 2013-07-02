/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/VisionHavokBehaviorHelper.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

#include <Behavior/Behavior/World/hkbWorld.h>

namespace RPG_VisionHavokBehaviorHelper
{
  bool BehaviorHasVarNamed(vHavokBehaviorComponent const& behaviorComponent, char const *varName)
  {
    int const idx = behaviorComponent.m_character->getWorld()->getVariableId(varName);
    return idx >= 0 ? behaviorComponent.m_character->getBehavior()->hasVariable(idx) : false;
  }

  template<class VAR_TYPE>
  VAR_TYPE BehaviorGetVarNamed(vHavokBehaviorComponent const& behaviorComponent, char const *varName)
  {
    VAR_TYPE valOut = VAR_TYPE();
    hkbCharacter const *const character = behaviorComponent.m_character;
    hkbBehaviorGraph /*const*/ *const characterBehavior = character->getBehavior();

    int const idx = character->getWorld()->getVariableId(varName);
    if(idx >= 0 && characterBehavior->hasVariable(idx))
    {
      valOut = characterBehavior->getVariableValueWord<VAR_TYPE>(idx);
    }
    return valOut;
  }

  template float BehaviorGetVarNamed<float>(vHavokBehaviorComponent const& behaviorComponent, char const *varName);
  template bool BehaviorGetVarNamed<bool>(vHavokBehaviorComponent const& behaviorComponent, char const *varName);
  template int BehaviorGetVarNamed<int>(vHavokBehaviorComponent const& behaviorComponent, char const *varName);
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
