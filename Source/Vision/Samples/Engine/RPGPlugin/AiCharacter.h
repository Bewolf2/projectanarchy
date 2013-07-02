/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_AI_CHARACTER_H__
#define RPG_PLUGIN_AI_CHARACTER_H__

#include <Vision/Samples/Engine/RPGPlugin/Character.h>

/// Character class for the Player
class RPG_AiCharacter : public RPG_Character
{
public:
  // Constructor needs to be public for FORCE_LINKDYNCLASS on mobile
  RPG_AiCharacter();

  float GetWanderRadius() const;

protected:

  float m_wanderRadius;           ///< distance this character can wander from its origin

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_AiCharacter, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_AiCharacter, RPG_PLUGIN_IMPEXP);
};

#endif // RPG_PLUGIN_AI_CHARACTER_H__

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
